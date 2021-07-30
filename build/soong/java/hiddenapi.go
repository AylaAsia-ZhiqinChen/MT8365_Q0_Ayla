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
	"strings"

	"github.com/google/blueprint"

	"android/soong/android"
)

var hiddenAPIGenerateCSVRule = pctx.AndroidStaticRule("hiddenAPIGenerateCSV", blueprint.RuleParams{
	Command:     "${config.Class2Greylist} --stub-api-flags ${stubAPIFlags} $in $outFlag $out",
	CommandDeps: []string{"${config.Class2Greylist}"},
}, "outFlag", "stubAPIFlags")

type hiddenAPI struct {
	flagsCSVPath    android.Path
	metadataCSVPath android.Path
	bootDexJarPath  android.Path
}

func (h *hiddenAPI) flagsCSV() android.Path {
	return h.flagsCSVPath
}

func (h *hiddenAPI) metadataCSV() android.Path {
	return h.metadataCSVPath
}

func (h *hiddenAPI) bootDexJar() android.Path {
	return h.bootDexJarPath
}

type hiddenAPIIntf interface {
	flagsCSV() android.Path
	metadataCSV() android.Path
	bootDexJar() android.Path
}

var _ hiddenAPIIntf = (*hiddenAPI)(nil)

func (h *hiddenAPI) hiddenAPI(ctx android.ModuleContext, dexJar android.ModuleOutPath, implementationJar android.Path,
	uncompressDex bool) android.ModuleOutPath {

	if !ctx.Config().IsEnvTrue("UNSAFE_DISABLE_HIDDENAPI_FLAGS") {
		name := ctx.ModuleName()

		// Modules whose names are of the format <x>-hiddenapi provide hiddenapi information
		// for the boot jar module <x>. Otherwise, the module provides information for itself.
		// Either way extract the name of the boot jar module.
		bootJarName := strings.TrimSuffix(name, "-hiddenapi")

		// If this module is on the boot jars list (or providing information for a module
		// on the list) then extract the hiddenapi information from it, and if necessary
		// encode that information in the generated dex file.
		//
		// It is important that hiddenapi information is only gathered for/from modules on
		// that are actually on the boot jars list because the runtime only enforces access
		// to the hidden API for the bootclassloader. If information is gathered for modules
		// not on the list then that will cause failures in the CtsHiddenApiBlacklist...
		// tests.
		if inList(bootJarName, ctx.Config().BootJars()) {
			// Derive the greylist from classes jar.
			flagsCSV := android.PathForModuleOut(ctx, "hiddenapi", "flags.csv")
			metadataCSV := android.PathForModuleOut(ctx, "hiddenapi", "metadata.csv")
			hiddenAPIGenerateCSV(ctx, flagsCSV, metadataCSV, implementationJar)
			h.flagsCSVPath = flagsCSV
			h.metadataCSVPath = metadataCSV

			// If this module is actually on the boot jars list and not providing
			// hiddenapi information for a module on the boot jars list then encode
			// the gathered information in the generated dex file.
			if name == bootJarName {
				hiddenAPIJar := android.PathForModuleOut(ctx, "hiddenapi", name+".jar")
				h.bootDexJarPath = dexJar
				hiddenAPIEncodeDex(ctx, hiddenAPIJar, dexJar, uncompressDex)
				dexJar = hiddenAPIJar
			}
		}
	}

	return dexJar
}

func hiddenAPIGenerateCSV(ctx android.ModuleContext, flagsCSV, metadataCSV android.WritablePath,
	classesJar android.Path) {

	stubFlagsCSV := hiddenAPISingletonPaths(ctx).stubFlags

	ctx.Build(pctx, android.BuildParams{
		Rule:        hiddenAPIGenerateCSVRule,
		Description: "hiddenapi flags",
		Input:       classesJar,
		Output:      flagsCSV,
		Implicit:    stubFlagsCSV,
		Args: map[string]string{
			"outFlag":      "--write-flags-csv",
			"stubAPIFlags": stubFlagsCSV.String(),
		},
	})

	ctx.Build(pctx, android.BuildParams{
		Rule:        hiddenAPIGenerateCSVRule,
		Description: "hiddenapi metadata",
		Input:       classesJar,
		Output:      metadataCSV,
		Implicit:    stubFlagsCSV,
		Args: map[string]string{
			"outFlag":      "--write-metadata-csv",
			"stubAPIFlags": stubFlagsCSV.String(),
		},
	})

}

var hiddenAPIEncodeDexRule = pctx.AndroidStaticRule("hiddenAPIEncodeDex", blueprint.RuleParams{
	Command: `rm -rf $tmpDir && mkdir -p $tmpDir && mkdir $tmpDir/dex-input && mkdir $tmpDir/dex-output && ` +
		`unzip -o -q $in 'classes*.dex' -d $tmpDir/dex-input && ` +
		`for INPUT_DEX in $$(find $tmpDir/dex-input -maxdepth 1 -name 'classes*.dex' | sort); do ` +
		`  echo "--input-dex=$${INPUT_DEX}"; ` +
		`  echo "--output-dex=$tmpDir/dex-output/$$(basename $${INPUT_DEX})"; ` +
		`done | xargs ${config.HiddenAPI} encode --api-flags=$flagsCsv $hiddenapiFlags && ` +
		`${config.SoongZipCmd} $soongZipFlags -o $tmpDir/dex.jar -C $tmpDir/dex-output -f "$tmpDir/dex-output/classes*.dex" && ` +
		`${config.MergeZipsCmd} -D -zipToNotStrip $tmpDir/dex.jar -stripFile "classes*.dex" $out $tmpDir/dex.jar $in`,
	CommandDeps: []string{
		"${config.HiddenAPI}",
		"${config.SoongZipCmd}",
		"${config.MergeZipsCmd}",
	},
}, "flagsCsv", "hiddenapiFlags", "tmpDir", "soongZipFlags")

func hiddenAPIEncodeDex(ctx android.ModuleContext, output android.WritablePath, dexInput android.Path,
	uncompressDex bool) {

	flagsCSV := hiddenAPISingletonPaths(ctx).flags

	// The encode dex rule requires unzipping and rezipping the classes.dex files, ensure that if it was uncompressed
	// in the input it stays uncompressed in the output.
	soongZipFlags := ""
	hiddenapiFlags := ""
	tmpOutput := output
	tmpDir := android.PathForModuleOut(ctx, "hiddenapi", "dex")
	if uncompressDex {
		soongZipFlags = "-L 0"
		tmpOutput = android.PathForModuleOut(ctx, "hiddenapi", "unaligned", "unaligned.jar")
		tmpDir = android.PathForModuleOut(ctx, "hiddenapi", "unaligned")
	}
	// If frameworks/base doesn't exist we must be building with the 'master-art' manifest.
	// Disable assertion that all methods/fields have hidden API flags assigned.
	if !ctx.Config().FrameworksBaseDirExists(ctx) {
		hiddenapiFlags = "--no-force-assign-all"
	}

	ctx.Build(pctx, android.BuildParams{
		Rule:        hiddenAPIEncodeDexRule,
		Description: "hiddenapi encode dex",
		Input:       dexInput,
		Output:      tmpOutput,
		Implicit:    flagsCSV,
		Args: map[string]string{
			"flagsCsv":       flagsCSV.String(),
			"tmpDir":         tmpDir.String(),
			"soongZipFlags":  soongZipFlags,
			"hiddenapiFlags": hiddenapiFlags,
		},
	})

	if uncompressDex {
		TransformZipAlign(ctx, output, tmpOutput)
	}
}
