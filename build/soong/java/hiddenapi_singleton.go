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
	"android/soong/android"
)

func init() {
	android.RegisterSingletonType("hiddenapi", hiddenAPISingletonFactory)
}

type hiddenAPISingletonPathsStruct struct {
	stubFlags android.OutputPath
	flags     android.OutputPath
	metadata  android.OutputPath
}

var hiddenAPISingletonPathsKey = android.NewOnceKey("hiddenAPISingletonPathsKey")

// hiddenAPISingletonPaths creates all the paths for singleton files the first time it is called, which may be
// from a ModuleContext that needs to reference a file that will be created by a singleton rule that hasn't
// yet been created.
func hiddenAPISingletonPaths(ctx android.PathContext) hiddenAPISingletonPathsStruct {
	return ctx.Config().Once(hiddenAPISingletonPathsKey, func() interface{} {
		return hiddenAPISingletonPathsStruct{
			stubFlags: android.PathForOutput(ctx, "hiddenapi", "hiddenapi-stub-flags.txt"),
			flags:     android.PathForOutput(ctx, "hiddenapi", "hiddenapi-flags.csv"),
			metadata:  android.PathForOutput(ctx, "hiddenapi", "hiddenapi-greylist.csv"),
		}
	}).(hiddenAPISingletonPathsStruct)
}

func hiddenAPISingletonFactory() android.Singleton {
	return &hiddenAPISingleton{}
}

type hiddenAPISingleton struct {
	flags, metadata android.Path
}

// hiddenAPI singleton rules
func (h *hiddenAPISingleton) GenerateBuildActions(ctx android.SingletonContext) {
	// Don't run any hiddenapi rules if UNSAFE_DISABLE_HIDDENAPI_FLAGS=true
	if ctx.Config().IsEnvTrue("UNSAFE_DISABLE_HIDDENAPI_FLAGS") {
		return
	}

	stubFlagsRule(ctx)

	// These rules depend on files located in frameworks/base, skip them if running in a tree that doesn't have them.
	if ctx.Config().FrameworksBaseDirExists(ctx) {
		h.flags = flagsRule(ctx)
		h.metadata = metadataRule(ctx)
	} else {
		h.flags = emptyFlagsRule(ctx)
	}
}

// Export paths to Make.  INTERNAL_PLATFORM_HIDDENAPI_FLAGS is used by Make rules in art/ and cts/.
// Both paths are used to call dist-for-goals.
func (h *hiddenAPISingleton) MakeVars(ctx android.MakeVarsContext) {
	if ctx.Config().IsEnvTrue("UNSAFE_DISABLE_HIDDENAPI_FLAGS") {
		return
	}

	ctx.Strict("INTERNAL_PLATFORM_HIDDENAPI_FLAGS", h.flags.String())

	if h.metadata != nil {
		ctx.Strict("INTERNAL_PLATFORM_HIDDENAPI_GREYLIST_METADATA", h.metadata.String())
	}
}

// stubFlagsRule creates the rule to build hiddenapi-stub-flags.txt out of dex jars from stub modules and boot image
// modules.
func stubFlagsRule(ctx android.SingletonContext) {
	// Public API stubs
	publicStubModules := []string{
		"android_stubs_current",
	}

	// Add the android.test.base to the set of stubs only if the android.test.base module is on
	// the boot jars list as the runtime will only enforce hiddenapi access against modules on
	// that list.
	if inList("android.test.base", ctx.Config().BootJars()) && !ctx.Config().UnbundledBuildUsePrebuiltSdks() {
		publicStubModules = append(publicStubModules, "android.test.base.stubs")
	}

	// System API stubs
	systemStubModules := []string{
		"android_system_stubs_current",
	}

	// Test API stubs
	testStubModules := []string{
		"android_test_stubs_current",
	}

	// Core Platform API stubs
	corePlatformStubModules := []string{
		"core.platform.api.stubs",
	}

	// Allow products to define their own stubs for custom product jars that apps can use.
	publicStubModules = append(publicStubModules, ctx.Config().ProductHiddenAPIStubs()...)
	systemStubModules = append(systemStubModules, ctx.Config().ProductHiddenAPIStubsSystem()...)
	testStubModules = append(testStubModules, ctx.Config().ProductHiddenAPIStubsTest()...)
	if ctx.Config().IsEnvTrue("EMMA_INSTRUMENT") {
		publicStubModules = append(publicStubModules, "jacoco-stubs")
	}

	publicStubPaths := make(android.Paths, len(publicStubModules))
	systemStubPaths := make(android.Paths, len(systemStubModules))
	testStubPaths := make(android.Paths, len(testStubModules))
	corePlatformStubPaths := make(android.Paths, len(corePlatformStubModules))

	moduleListToPathList := map[*[]string]android.Paths{
		&publicStubModules:       publicStubPaths,
		&systemStubModules:       systemStubPaths,
		&testStubModules:         testStubPaths,
		&corePlatformStubModules: corePlatformStubPaths,
	}

	var bootDexJars android.Paths

	ctx.VisitAllModules(func(module android.Module) {
		// Collect dex jar paths for the modules listed above.
		if j, ok := module.(Dependency); ok {
			name := ctx.ModuleName(module)
			for moduleList, pathList := range moduleListToPathList {
				if i := android.IndexList(name, *moduleList); i != -1 {
					pathList[i] = j.DexJar()
				}
			}
		}

		// Collect dex jar paths for modules that had hiddenapi encode called on them.
		if h, ok := module.(hiddenAPIIntf); ok {
			if jar := h.bootDexJar(); jar != nil {
				bootDexJars = append(bootDexJars, jar)
			}
		}
	})

	var missingDeps []string
	// Ensure all modules were converted to paths
	for moduleList, pathList := range moduleListToPathList {
		for i := range pathList {
			if pathList[i] == nil {
				pathList[i] = android.PathForOutput(ctx, "missing")
				if ctx.Config().AllowMissingDependencies() {
					missingDeps = append(missingDeps, (*moduleList)[i])
				} else {
					ctx.Errorf("failed to find dex jar path for module %q",
						(*moduleList)[i])
				}
			}
		}
	}

	// Singleton rule which applies hiddenapi on all boot class path dex files.
	rule := android.NewRuleBuilder()

	outputPath := hiddenAPISingletonPaths(ctx).stubFlags
	tempPath := android.PathForOutput(ctx, outputPath.Rel()+".tmp")

	rule.MissingDeps(missingDeps)

	rule.Command().
		Tool(pctx.HostBinToolPath(ctx, "hiddenapi")).
		Text("list").
		FlagForEachInput("--boot-dex=", bootDexJars).
		FlagWithInputList("--public-stub-classpath=", publicStubPaths, ":").
		FlagWithInputList("--system-stub-classpath=", systemStubPaths, ":").
		FlagWithInputList("--test-stub-classpath=", testStubPaths, ":").
		FlagWithInputList("--core-platform-stub-classpath=", corePlatformStubPaths, ":").
		FlagWithOutput("--out-api-flags=", tempPath)

	commitChangeForRestat(rule, tempPath, outputPath)

	rule.Build(pctx, ctx, "hiddenAPIStubFlagsFile", "hiddenapi stub flags")
}

// flagsRule creates a rule to build hiddenapi-flags.csv out of flags.csv files generated for boot image modules and
// the greylists.
func flagsRule(ctx android.SingletonContext) android.Path {
	var flagsCSV android.Paths

	var greylistIgnoreConflicts android.Path

	ctx.VisitAllModules(func(module android.Module) {
		if h, ok := module.(hiddenAPIIntf); ok {
			if csv := h.flagsCSV(); csv != nil {
				flagsCSV = append(flagsCSV, csv)
			}
		} else if ds, ok := module.(*Droidstubs); ok && ctx.ModuleName(module) == "hiddenapi-lists-docs" {
			greylistIgnoreConflicts = ds.removedDexApiFile
		}
	})

	if greylistIgnoreConflicts == nil {
		ctx.Errorf("failed to find removed_dex_api_filename from hiddenapi-lists-docs module")
		return nil
	}

	rule := android.NewRuleBuilder()

	outputPath := hiddenAPISingletonPaths(ctx).flags
	tempPath := android.PathForOutput(ctx, outputPath.Rel()+".tmp")

	stubFlags := hiddenAPISingletonPaths(ctx).stubFlags

	rule.Command().
		Tool(android.PathForSource(ctx, "frameworks/base/tools/hiddenapi/generate_hiddenapi_lists.py")).
		FlagWithInput("--csv ", stubFlags).
		Inputs(flagsCSV).
		FlagWithInput("--greylist ",
			android.PathForSource(ctx, "frameworks/base/config/hiddenapi-greylist.txt")).
		FlagWithInput("--greylist-ignore-conflicts ",
			greylistIgnoreConflicts).
		FlagWithInput("--greylist-max-p ",
			android.PathForSource(ctx, "frameworks/base/config/hiddenapi-greylist-max-p.txt")).
		FlagWithInput("--greylist-max-o-ignore-conflicts ",
			android.PathForSource(ctx, "frameworks/base/config/hiddenapi-greylist-max-o.txt")).
		FlagWithInput("--blacklist ",
			android.PathForSource(ctx, "frameworks/base/config/hiddenapi-force-blacklist.txt")).
		FlagWithInput("--greylist-packages ",
			android.PathForSource(ctx, "frameworks/base/config/hiddenapi-greylist-packages.txt")).
		FlagWithOutput("--output ", tempPath)

	commitChangeForRestat(rule, tempPath, outputPath)

	rule.Build(pctx, ctx, "hiddenAPIFlagsFile", "hiddenapi flags")

	return outputPath
}

// emptyFlagsRule creates a rule to build an empty hiddenapi-flags.csv, which is needed by master-art-host builds that
// have a partial manifest without frameworks/base but still need to build a boot image.
func emptyFlagsRule(ctx android.SingletonContext) android.Path {
	rule := android.NewRuleBuilder()

	outputPath := hiddenAPISingletonPaths(ctx).flags

	rule.Command().Text("rm").Flag("-f").Output(outputPath)
	rule.Command().Text("touch").Output(outputPath)

	rule.Build(pctx, ctx, "emptyHiddenAPIFlagsFile", "empty hiddenapi flags")

	return outputPath
}

// metadataRule creates a rule to build hiddenapi-greylist.csv out of the metadata.csv files generated for boot image
// modules.
func metadataRule(ctx android.SingletonContext) android.Path {
	var metadataCSV android.Paths

	ctx.VisitAllModules(func(module android.Module) {
		if h, ok := module.(hiddenAPIIntf); ok {
			if csv := h.metadataCSV(); csv != nil {
				metadataCSV = append(metadataCSV, csv)
			}
		}
	})

	rule := android.NewRuleBuilder()

	outputPath := hiddenAPISingletonPaths(ctx).metadata

	rule.Command().
		Tool(android.PathForSource(ctx, "frameworks/base/tools/hiddenapi/merge_csv.py")).
		Inputs(metadataCSV).
		Text(">").
		Output(outputPath)

	rule.Build(pctx, ctx, "hiddenAPIGreylistMetadataFile", "hiddenapi greylist metadata")

	return outputPath
}

// commitChangeForRestat adds a command to a rule that updates outputPath from tempPath if they are different.  It
// also marks the rule as restat and marks the tempPath as a temporary file that should not be considered an output of
// the rule.
func commitChangeForRestat(rule *android.RuleBuilder, tempPath, outputPath android.WritablePath) {
	rule.Restat()
	rule.Temporary(tempPath)
	rule.Command().
		Text("(").
		Text("if").
		Text("cmp -s").Input(tempPath).Output(outputPath).Text(";").
		Text("then").
		Text("rm").Input(tempPath).Text(";").
		Text("else").
		Text("mv").Input(tempPath).Output(outputPath).Text(";").
		Text("fi").
		Text(")")
}
