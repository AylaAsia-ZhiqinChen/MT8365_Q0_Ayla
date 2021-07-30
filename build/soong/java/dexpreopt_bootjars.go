// Copyright 2019 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package java

import (
	"path/filepath"
	"sort"
	"strings"

	"android/soong/android"
	"android/soong/dexpreopt"

	"github.com/google/blueprint/pathtools"
	"github.com/google/blueprint/proptools"
)

func init() {
	android.RegisterSingletonType("dex_bootjars", dexpreoptBootJarsFactory)
}

// The image "location" is a symbolic path that with multiarchitecture
// support doesn't really exist on the device. Typically it is
// /system/framework/boot.art and should be the same for all supported
// architectures on the device. The concrete architecture specific
// content actually ends up in a "filename" that contains an
// architecture specific directory name such as arm, arm64, mips,
// mips64, x86, x86_64.
//
// Here are some example values for an x86_64 / x86 configuration:
//
// bootImages["x86_64"] = "out/soong/generic_x86_64/dex_bootjars/system/framework/x86_64/boot.art"
// dexpreopt.PathToLocation(bootImages["x86_64"], "x86_64") = "out/soong/generic_x86_64/dex_bootjars/system/framework/boot.art"
//
// bootImages["x86"] = "out/soong/generic_x86_64/dex_bootjars/system/framework/x86/boot.art"
// dexpreopt.PathToLocation(bootImages["x86"])= "out/soong/generic_x86_64/dex_bootjars/system/framework/boot.art"
//
// The location is passed as an argument to the ART tools like dex2oat instead of the real path. The ART tools
// will then reconstruct the real path, so the rules must have a dependency on the real path.

type bootImageConfig struct {
	name         string
	modules      []string
	dexLocations []string
	dexPaths     android.WritablePaths
	dir          android.OutputPath
	symbolsDir   android.OutputPath
	images       map[android.ArchType]android.OutputPath
}

type bootImage struct {
	bootImageConfig

	installs           map[android.ArchType]android.RuleBuilderInstalls
	vdexInstalls       map[android.ArchType]android.RuleBuilderInstalls
	unstrippedInstalls map[android.ArchType]android.RuleBuilderInstalls

	profileInstalls android.RuleBuilderInstalls
}

func newBootImage(ctx android.PathContext, config bootImageConfig) *bootImage {
	image := &bootImage{
		bootImageConfig: config,

		installs:           make(map[android.ArchType]android.RuleBuilderInstalls),
		vdexInstalls:       make(map[android.ArchType]android.RuleBuilderInstalls),
		unstrippedInstalls: make(map[android.ArchType]android.RuleBuilderInstalls),
	}

	return image
}

func concat(lists ...[]string) []string {
	var size int
	for _, l := range lists {
		size += len(l)
	}
	ret := make([]string, 0, size)
	for _, l := range lists {
		ret = append(ret, l...)
	}
	return ret
}

func dexpreoptBootJarsFactory() android.Singleton {
	return &dexpreoptBootJars{}
}

func skipDexpreoptBootJars(ctx android.PathContext) bool {
	if ctx.Config().UnbundledBuild() {
		return true
	}

	if len(ctx.Config().Targets[android.Android]) == 0 {
		// Host-only build
		return true
	}

	return false
}

type dexpreoptBootJars struct {
	defaultBootImage *bootImage
	otherImages      []*bootImage
}

// dexpreoptBoot singleton rules
func (d *dexpreoptBootJars) GenerateBuildActions(ctx android.SingletonContext) {
	if skipDexpreoptBootJars(ctx) {
		return
	}

	global := dexpreoptGlobalConfig(ctx)

	// Skip recompiling the boot image for the second sanitization phase. We'll get separate paths
	// and invalidate first-stage artifacts which are crucial to SANITIZE_LITE builds.
	// Note: this is technically incorrect. Compiled code contains stack checks which may depend
	//       on ASAN settings.
	if len(ctx.Config().SanitizeDevice()) == 1 &&
		ctx.Config().SanitizeDevice()[0] == "address" &&
		global.SanitizeLite {
		return
	}

	// Always create the default boot image first, to get a unique profile rule for all images.
	d.defaultBootImage = buildBootImage(ctx, defaultBootImageConfig(ctx))
	if global.GenerateApexImage {
		d.otherImages = append(d.otherImages, buildBootImage(ctx, apexBootImageConfig(ctx)))
	}

	dumpOatRules(ctx, d.defaultBootImage)
}

// buildBootImage takes a bootImageConfig, creates rules to build it, and returns a *bootImage.
func buildBootImage(ctx android.SingletonContext, config bootImageConfig) *bootImage {
	global := dexpreoptGlobalConfig(ctx)

	image := newBootImage(ctx, config)

	bootDexJars := make(android.Paths, len(image.modules))

	ctx.VisitAllModules(func(module android.Module) {
		// Collect dex jar paths for the modules listed above.
		if j, ok := module.(interface{ DexJar() android.Path }); ok {
			name := ctx.ModuleName(module)
			if i := android.IndexList(name, image.modules); i != -1 {
				bootDexJars[i] = j.DexJar()
			}
		}
	})

	var missingDeps []string
	// Ensure all modules were converted to paths
	for i := range bootDexJars {
		if bootDexJars[i] == nil {
			if ctx.Config().AllowMissingDependencies() {
				missingDeps = append(missingDeps, image.modules[i])
				bootDexJars[i] = android.PathForOutput(ctx, "missing")
			} else {
				ctx.Errorf("failed to find dex jar path for module %q",
					image.modules[i])
			}
		}
	}

	// The path to bootclasspath dex files needs to be known at module GenerateAndroidBuildAction time, before
	// the bootclasspath modules have been compiled.  Copy the dex jars there so the module rules that have
	// already been set up can find them.
	for i := range bootDexJars {
		ctx.Build(pctx, android.BuildParams{
			Rule:   android.Cp,
			Input:  bootDexJars[i],
			Output: image.dexPaths[i],
		})
	}

	profile := bootImageProfileRule(ctx, image, missingDeps)

	if !global.DisablePreopt {
		targets := ctx.Config().Targets[android.Android]
		if ctx.Config().SecondArchIsTranslated() {
			targets = targets[:1]
		}

		for _, target := range targets {
			buildBootImageRuleForArch(ctx, image, target.Arch.ArchType, profile, missingDeps)
		}
	}

	return image
}

func buildBootImageRuleForArch(ctx android.SingletonContext, image *bootImage,
	arch android.ArchType, profile android.Path, missingDeps []string) {

	global := dexpreoptGlobalConfig(ctx)

	symbolsDir := image.symbolsDir.Join(ctx, "system/framework", arch.String())
	symbolsFile := symbolsDir.Join(ctx, image.name+".oat")
	outputDir := image.dir.Join(ctx, "system/framework", arch.String())
	outputPath := image.images[arch]
	oatLocation := pathtools.ReplaceExtension(dexpreopt.PathToLocation(outputPath, arch), "oat")

	rule := android.NewRuleBuilder()
	rule.MissingDeps(missingDeps)

	rule.Command().Text("mkdir").Flag("-p").Flag(symbolsDir.String())
	rule.Command().Text("rm").Flag("-f").
		Flag(symbolsDir.Join(ctx, "*.art").String()).
		Flag(symbolsDir.Join(ctx, "*.oat").String()).
		Flag(symbolsDir.Join(ctx, "*.invocation").String())
	rule.Command().Text("rm").Flag("-f").
		Flag(outputDir.Join(ctx, "*.art").String()).
		Flag(outputDir.Join(ctx, "*.oat").String()).
		Flag(outputDir.Join(ctx, "*.invocation").String())

	cmd := rule.Command()

	extraFlags := ctx.Config().Getenv("ART_BOOT_IMAGE_EXTRA_ARGS")
	if extraFlags == "" {
		// Use ANDROID_LOG_TAGS to suppress most logging by default...
		cmd.Text(`ANDROID_LOG_TAGS="*:e"`)
	} else {
		// ...unless the boot image is generated specifically for testing, then allow all logging.
		cmd.Text(`ANDROID_LOG_TAGS="*:v"`)
	}

	invocationPath := outputPath.ReplaceExtension(ctx, "invocation")

	cmd.Tool(global.Tools.Dex2oat).
		Flag("--avoid-storing-invocation").
		FlagWithOutput("--write-invocation-to=", invocationPath).ImplicitOutput(invocationPath).
		Flag("--runtime-arg").FlagWithArg("-Xms", global.Dex2oatImageXms).
		Flag("--runtime-arg").FlagWithArg("-Xmx", global.Dex2oatImageXmx)

	if profile != nil {
		cmd.FlagWithArg("--compiler-filter=", "speed-profile")
		cmd.FlagWithInput("--profile-file=", profile)
	} else if global.PreloadedClasses.Valid() {
		cmd.FlagWithInput("--image-classes=", global.PreloadedClasses.Path())
	}

	if global.DirtyImageObjects.Valid() {
		cmd.FlagWithInput("--dirty-image-objects=", global.DirtyImageObjects.Path())
	}

	cmd.
		FlagForEachInput("--dex-file=", image.dexPaths.Paths()).
		FlagForEachArg("--dex-location=", image.dexLocations).
		Flag("--generate-debug-info").
		Flag("--generate-build-id").
		FlagWithOutput("--oat-symbols=", symbolsFile).
		Flag("--strip").
		FlagWithOutput("--oat-file=", outputPath.ReplaceExtension(ctx, "oat")).
		FlagWithArg("--oat-location=", oatLocation).
		FlagWithOutput("--image=", outputPath).
		FlagWithArg("--base=", ctx.Config().LibartImgDeviceBaseAddress()).
		FlagWithArg("--instruction-set=", arch.String()).
		FlagWithArg("--instruction-set-variant=", global.CpuVariant[arch]).
		FlagWithArg("--instruction-set-features=", global.InstructionSetFeatures[arch]).
		FlagWithArg("--android-root=", global.EmptyDirectory).
		FlagWithArg("--no-inline-from=", "core-oj.jar").
		Flag("--abort-on-hard-verifier-error")

	if global.BootFlags != "" {
		cmd.Flag(global.BootFlags)
	}

	if extraFlags != "" {
		cmd.Flag(extraFlags)
	}

	cmd.Textf(`|| ( echo %s ; false )`, proptools.ShellEscape(failureMessage))

	installDir := filepath.Join("/system/framework", arch.String())
	vdexInstallDir := filepath.Join("/system/framework")

	var extraFiles android.WritablePaths
	var vdexInstalls android.RuleBuilderInstalls
	var unstrippedInstalls android.RuleBuilderInstalls

	// dex preopt on the bootclasspath produces multiple files.  The first dex file
	// is converted into to 'name'.art (to match the legacy assumption that 'name'.art
	// exists), and the rest are converted to 'name'-<jar>.art.
	// In addition, each .art file has an associated .oat and .vdex file, and an
	// unstripped .oat file
	for i, m := range image.modules {
		name := image.name
		if i != 0 {
			name += "-" + m
		}

		art := outputDir.Join(ctx, name+".art")
		oat := outputDir.Join(ctx, name+".oat")
		vdex := outputDir.Join(ctx, name+".vdex")
		unstrippedOat := symbolsDir.Join(ctx, name+".oat")

		extraFiles = append(extraFiles, art, oat, vdex, unstrippedOat)

		// Install the .oat and .art files.
		rule.Install(art, filepath.Join(installDir, art.Base()))
		rule.Install(oat, filepath.Join(installDir, oat.Base()))

		// The vdex files are identical between architectures, install them to a shared location.  The Make rules will
		// only use the install rules for one architecture, and will create symlinks into the architecture-specific
		// directories.
		vdexInstalls = append(vdexInstalls,
			android.RuleBuilderInstall{vdex, filepath.Join(vdexInstallDir, vdex.Base())})

		// Install the unstripped oat files.  The Make rules will put these in $(TARGET_OUT_UNSTRIPPED)
		unstrippedInstalls = append(unstrippedInstalls,
			android.RuleBuilderInstall{unstrippedOat, filepath.Join(installDir, unstrippedOat.Base())})
	}

	cmd.ImplicitOutputs(extraFiles)

	rule.Build(pctx, ctx, image.name+"JarsDexpreopt_"+arch.String(), "dexpreopt "+image.name+" jars "+arch.String())

	// save output and installed files for makevars
	image.installs[arch] = rule.Installs()
	image.vdexInstalls[arch] = vdexInstalls
	image.unstrippedInstalls[arch] = unstrippedInstalls
}

const failureMessage = `ERROR: Dex2oat failed to compile a boot image.
It is likely that the boot classpath is inconsistent.
Rebuild with ART_BOOT_IMAGE_EXTRA_ARGS="--runtime-arg -verbose:verifier" to see verification errors.`

func bootImageProfileRule(ctx android.SingletonContext, image *bootImage, missingDeps []string) android.WritablePath {
	global := dexpreoptGlobalConfig(ctx)

	if !global.UseProfileForBootImage || ctx.Config().IsPdkBuild() || ctx.Config().UnbundledBuild() {
		return nil
	}
	return ctx.Config().Once(bootImageProfileRuleKey, func() interface{} {
		tools := global.Tools

		rule := android.NewRuleBuilder()
		rule.MissingDeps(missingDeps)

		var bootImageProfile android.Path
		if len(global.BootImageProfiles) > 1 {
			combinedBootImageProfile := image.dir.Join(ctx, "boot-image-profile.txt")
			rule.Command().Text("cat").Inputs(global.BootImageProfiles).Text(">").Output(combinedBootImageProfile)
			bootImageProfile = combinedBootImageProfile
		} else if len(global.BootImageProfiles) == 1 {
			bootImageProfile = global.BootImageProfiles[0]
		} else {
			// If not set, use the default.  Some branches like master-art-host don't have frameworks/base, so manually
			// handle the case that the default is missing.  Those branches won't attempt to build the profile rule,
			// and if they do they'll get a missing deps error.
			defaultProfile := "frameworks/base/config/boot-image-profile.txt"
			path := android.ExistentPathForSource(ctx, defaultProfile)
			if path.Valid() {
				bootImageProfile = path.Path()
			} else {
				missingDeps = append(missingDeps, defaultProfile)
				bootImageProfile = android.PathForOutput(ctx, "missing")
			}
		}

		profile := image.dir.Join(ctx, "boot.prof")

		rule.Command().
			Text(`ANDROID_LOG_TAGS="*:e"`).
			Tool(tools.Profman).
			FlagWithInput("--create-profile-from=", bootImageProfile).
			FlagForEachInput("--apk=", image.dexPaths.Paths()).
			FlagForEachArg("--dex-location=", image.dexLocations).
			FlagWithOutput("--reference-profile-file=", profile)

		rule.Install(profile, "/system/etc/boot-image.prof")

		rule.Build(pctx, ctx, "bootJarsProfile", "profile boot jars")

		image.profileInstalls = rule.Installs()

		return profile
	}).(android.WritablePath)
}

var bootImageProfileRuleKey = android.NewOnceKey("bootImageProfileRule")

func dumpOatRules(ctx android.SingletonContext, image *bootImage) {
	var archs []android.ArchType
	for arch := range image.images {
		archs = append(archs, arch)
	}
	sort.Slice(archs, func(i, j int) bool { return archs[i].String() < archs[j].String() })

	var allPhonies android.Paths
	for _, arch := range archs {
		// Create a rule to call oatdump.
		output := android.PathForOutput(ctx, "boot."+arch.String()+".oatdump.txt")
		rule := android.NewRuleBuilder()
		rule.Command().
			// TODO: for now, use the debug version for better error reporting
			Tool(ctx.Config().HostToolPath(ctx, "oatdumpd")).
			FlagWithInputList("--runtime-arg -Xbootclasspath:", image.dexPaths.Paths(), ":").
			FlagWithList("--runtime-arg -Xbootclasspath-locations:", image.dexLocations, ":").
			FlagWithArg("--image=", dexpreopt.PathToLocation(image.images[arch], arch)).Implicit(image.images[arch]).
			FlagWithOutput("--output=", output).
			FlagWithArg("--instruction-set=", arch.String())
		rule.Build(pctx, ctx, "dump-oat-boot-"+arch.String(), "dump oat boot "+arch.String())

		// Create a phony rule that depends on the output file and prints the path.
		phony := android.PathForPhony(ctx, "dump-oat-boot-"+arch.String())
		rule = android.NewRuleBuilder()
		rule.Command().
			Implicit(output).
			ImplicitOutput(phony).
			Text("echo").FlagWithArg("Output in ", output.String())
		rule.Build(pctx, ctx, "phony-dump-oat-boot-"+arch.String(), "dump oat boot "+arch.String())

		allPhonies = append(allPhonies, phony)
	}

	phony := android.PathForPhony(ctx, "dump-oat-boot")
	ctx.Build(pctx, android.BuildParams{
		Rule:        android.Phony,
		Output:      phony,
		Inputs:      allPhonies,
		Description: "dump-oat-boot",
	})

}

// Export paths for default boot image to Make
func (d *dexpreoptBootJars) MakeVars(ctx android.MakeVarsContext) {
	image := d.defaultBootImage
	if image != nil {
		ctx.Strict("DEXPREOPT_IMAGE_PROFILE_BUILT_INSTALLED", image.profileInstalls.String())
		ctx.Strict("DEXPREOPT_BOOTCLASSPATH_DEX_FILES", strings.Join(image.dexPaths.Strings(), " "))
		ctx.Strict("DEXPREOPT_BOOTCLASSPATH_DEX_LOCATIONS", strings.Join(image.dexLocations, " "))

		var imageNames []string
		for _, current := range append(d.otherImages, image) {
			imageNames = append(imageNames, current.name)
			var arches []android.ArchType
			for arch, _ := range current.images {
				arches = append(arches, arch)
			}

			sort.Slice(arches, func(i, j int) bool { return arches[i].String() < arches[j].String() })

			for _, arch := range arches {
				ctx.Strict("DEXPREOPT_IMAGE_VDEX_BUILT_INSTALLED_"+current.name+"_"+arch.String(), current.vdexInstalls[arch].String())
				ctx.Strict("DEXPREOPT_IMAGE_"+current.name+"_"+arch.String(), current.images[arch].String())
				ctx.Strict("DEXPREOPT_IMAGE_BUILT_INSTALLED_"+current.name+"_"+arch.String(), current.installs[arch].String())
				ctx.Strict("DEXPREOPT_IMAGE_UNSTRIPPED_BUILT_INSTALLED_"+current.name+"_"+arch.String(), current.unstrippedInstalls[arch].String())
			}
		}
		ctx.Strict("DEXPREOPT_IMAGE_NAMES", strings.Join(imageNames, " "))
	}
}
