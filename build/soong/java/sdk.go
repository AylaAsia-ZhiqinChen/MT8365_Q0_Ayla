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
	"android/soong/java/config"
	"fmt"
	"path/filepath"
	"runtime"
	"sort"
	"strconv"
	"strings"

	"github.com/google/blueprint/pathtools"
)

func init() {
	android.RegisterPreSingletonType("sdk_versions", sdkPreSingletonFactory)
	android.RegisterSingletonType("sdk", sdkSingletonFactory)
	android.RegisterMakeVarsProvider(pctx, sdkMakeVars)
}

var sdkVersionsKey = android.NewOnceKey("sdkVersionsKey")
var sdkFrameworkAidlPathKey = android.NewOnceKey("sdkFrameworkAidlPathKey")
var apiFingerprintPathKey = android.NewOnceKey("apiFingerprintPathKey")

type sdkContext interface {
	// sdkVersion eturns the sdk_version property of the current module, or an empty string if it is not set.
	sdkVersion() string
	// minSdkVersion returns the min_sdk_version property of the current module, or sdkVersion() if it is not set.
	minSdkVersion() string
	// targetSdkVersion returns the target_sdk_version property of the current module, or sdkVersion() if it is not set.
	targetSdkVersion() string
}

func sdkVersionOrDefault(ctx android.BaseContext, v string) string {
	switch v {
	case "", "current", "system_current", "test_current", "core_current":
		return ctx.Config().DefaultAppTargetSdk()
	default:
		return v
	}
}

// Returns a sdk version as a number.  For modules targeting an unreleased SDK (meaning it does not yet have a number)
// it returns android.FutureApiLevel (10000).
func sdkVersionToNumber(ctx android.BaseContext, v string) (int, error) {
	switch v {
	case "", "current", "test_current", "system_current", "core_current":
		return ctx.Config().DefaultAppTargetSdkInt(), nil
	default:
		n := android.GetNumericSdkVersion(v)
		if i, err := strconv.Atoi(n); err != nil {
			return -1, fmt.Errorf("invalid sdk version %q", n)
		} else {
			return i, nil
		}
	}
}

func sdkVersionToNumberAsString(ctx android.BaseContext, v string) (string, error) {
	n, err := sdkVersionToNumber(ctx, v)
	if err != nil {
		return "", err
	}
	return strconv.Itoa(n), nil
}

func decodeSdkDep(ctx android.BaseContext, sdkContext sdkContext) sdkDep {
	v := sdkContext.sdkVersion()
	// For PDK builds, use the latest SDK version instead of "current"
	if ctx.Config().IsPdkBuild() && (v == "" || v == "current") {
		sdkVersions := ctx.Config().Get(sdkVersionsKey).([]int)
		latestSdkVersion := 0
		if len(sdkVersions) > 0 {
			latestSdkVersion = sdkVersions[len(sdkVersions)-1]
		}
		v = strconv.Itoa(latestSdkVersion)
	}

	numericSdkVersion, err := sdkVersionToNumber(ctx, v)
	if err != nil {
		ctx.PropertyErrorf("sdk_version", "%s", err)
		return sdkDep{}
	}

	toPrebuilt := func(sdk string) sdkDep {
		var api, v string
		if strings.Contains(sdk, "_") {
			t := strings.Split(sdk, "_")
			api = t[0]
			v = t[1]
		} else {
			api = "public"
			v = sdk
		}
		dir := filepath.Join("prebuilts", "sdk", v, api)
		jar := filepath.Join(dir, "android.jar")
		// There's no aidl for other SDKs yet.
		// TODO(77525052): Add aidl files for other SDKs too.
		public_dir := filepath.Join("prebuilts", "sdk", v, "public")
		aidl := filepath.Join(public_dir, "framework.aidl")
		jarPath := android.ExistentPathForSource(ctx, jar)
		aidlPath := android.ExistentPathForSource(ctx, aidl)
		lambdaStubsPath := android.PathForSource(ctx, config.SdkLambdaStubsPath)

		if (!jarPath.Valid() || !aidlPath.Valid()) && ctx.Config().AllowMissingDependencies() {
			return sdkDep{
				invalidVersion: true,
				modules:        []string{fmt.Sprintf("sdk_%s_%s_android", api, v)},
			}
		}

		if !jarPath.Valid() {
			ctx.PropertyErrorf("sdk_version", "invalid sdk version %q, %q does not exist", v, jar)
			return sdkDep{}
		}

		if !aidlPath.Valid() {
			ctx.PropertyErrorf("sdk_version", "invalid sdk version %q, %q does not exist", v, aidl)
			return sdkDep{}
		}

		return sdkDep{
			useFiles: true,
			jars:     android.Paths{jarPath.Path(), lambdaStubsPath},
			aidl:     android.OptionalPathForPath(aidlPath.Path()),
		}
	}

	toModule := func(m, r string, aidl android.Path) sdkDep {
		ret := sdkDep{
			useModule:          true,
			modules:            []string{m, config.DefaultLambdaStubsLibrary},
			systemModules:      m + "_system_modules",
			frameworkResModule: r,
			aidl:               android.OptionalPathForPath(aidl),
		}

		if m == "core.current.stubs" {
			ret.systemModules = "core-system-modules"
		} else if m == "core.platform.api.stubs" {
			ret.systemModules = "core-platform-api-stubs-system-modules"
		}
		return ret
	}

	// Ensures that the specificed system SDK version is one of BOARD_SYSTEMSDK_VERSIONS (for vendor apks)
	// or PRODUCT_SYSTEMSDK_VERSIONS (for other apks or when BOARD_SYSTEMSDK_VERSIONS is not set)
	if strings.HasPrefix(v, "system_") && numericSdkVersion != android.FutureApiLevel {
		allowed_versions := ctx.DeviceConfig().PlatformSystemSdkVersions()
		if ctx.DeviceSpecific() || ctx.SocSpecific() {
			if len(ctx.DeviceConfig().SystemSdkVersions()) > 0 {
				allowed_versions = ctx.DeviceConfig().SystemSdkVersions()
			}
		}
		if len(allowed_versions) > 0 && !android.InList(strconv.Itoa(numericSdkVersion), allowed_versions) {
			ctx.PropertyErrorf("sdk_version", "incompatible sdk version %q. System SDK version should be one of %q",
				v, allowed_versions)
		}
	}

	if ctx.Config().UnbundledBuildUsePrebuiltSdks() && v != "" {
		return toPrebuilt(v)
	}

	switch v {
	case "":
		return sdkDep{
			useDefaultLibs:     true,
			frameworkResModule: "framework-res",
		}
	case "current":
		return toModule("android_stubs_current", "framework-res", sdkFrameworkAidlPath(ctx))
	case "system_current":
		return toModule("android_system_stubs_current", "framework-res", sdkFrameworkAidlPath(ctx))
	case "test_current":
		return toModule("android_test_stubs_current", "framework-res", sdkFrameworkAidlPath(ctx))
	case "core_current":
		return toModule("core.current.stubs", "", nil)
	default:
		return toPrebuilt(v)
	}
}

func sdkPreSingletonFactory() android.Singleton {
	return sdkPreSingleton{}
}

type sdkPreSingleton struct{}

func (sdkPreSingleton) GenerateBuildActions(ctx android.SingletonContext) {
	sdkJars, err := ctx.GlobWithDeps("prebuilts/sdk/*/public/android.jar", nil)
	if err != nil {
		ctx.Errorf("failed to glob prebuilts/sdk/*/public/android.jar: %s", err.Error())
	}

	var sdkVersions []int
	for _, sdkJar := range sdkJars {
		dir := filepath.Base(filepath.Dir(filepath.Dir(sdkJar)))
		v, err := strconv.Atoi(dir)
		if scerr, ok := err.(*strconv.NumError); ok && scerr.Err == strconv.ErrSyntax {
			continue
		} else if err != nil {
			ctx.Errorf("invalid sdk jar %q, %s, %v", sdkJar, err.Error())
		}
		sdkVersions = append(sdkVersions, v)
	}

	sort.Ints(sdkVersions)

	ctx.Config().Once(sdkVersionsKey, func() interface{} { return sdkVersions })
}

func sdkSingletonFactory() android.Singleton {
	return sdkSingleton{}
}

type sdkSingleton struct{}

func (sdkSingleton) GenerateBuildActions(ctx android.SingletonContext) {
	if ctx.Config().UnbundledBuildUsePrebuiltSdks() || ctx.Config().IsPdkBuild() {
		return
	}

	createSdkFrameworkAidl(ctx)
	createAPIFingerprint(ctx)
}

// Create framework.aidl by extracting anything that implements android.os.Parcelable from the SDK stubs modules.
func createSdkFrameworkAidl(ctx android.SingletonContext) {
	stubsModules := []string{
		"android_stubs_current",
		"android_test_stubs_current",
		"android_system_stubs_current",
	}

	stubsJars := make([]android.Paths, len(stubsModules))

	ctx.VisitAllModules(func(module android.Module) {
		// Collect dex jar paths for the modules listed above.
		if j, ok := module.(Dependency); ok {
			name := ctx.ModuleName(module)
			if i := android.IndexList(name, stubsModules); i != -1 {
				stubsJars[i] = j.HeaderJars()
			}
		}
	})

	var missingDeps []string

	for i := range stubsJars {
		if stubsJars[i] == nil {
			if ctx.Config().AllowMissingDependencies() {
				missingDeps = append(missingDeps, stubsModules[i])
			} else {
				ctx.Errorf("failed to find dex jar path for module %q",
					stubsModules[i])
			}
		}
	}

	rule := android.NewRuleBuilder()
	rule.MissingDeps(missingDeps)

	var aidls android.Paths
	for _, jars := range stubsJars {
		for _, jar := range jars {
			aidl := android.PathForOutput(ctx, "aidl", pathtools.ReplaceExtension(jar.Base(), "aidl"))

			rule.Command().
				Text("rm -f").Output(aidl)
			rule.Command().
				Tool(ctx.Config().HostToolPath(ctx, "sdkparcelables")).
				Input(jar).
				Output(aidl)

			aidls = append(aidls, aidl)
		}
	}

	combinedAidl := sdkFrameworkAidlPath(ctx)
	tempPath := combinedAidl.ReplaceExtension(ctx, "aidl.tmp")

	rule.Command().
		Text("rm -f").Output(tempPath)
	rule.Command().
		Text("cat").
		Inputs(aidls).
		Text("| sort -u >").
		Output(tempPath)

	commitChangeForRestat(rule, tempPath, combinedAidl)

	rule.Build(pctx, ctx, "framework_aidl", "generate framework.aidl")
}

func sdkFrameworkAidlPath(ctx android.PathContext) android.OutputPath {
	return ctx.Config().Once(sdkFrameworkAidlPathKey, func() interface{} {
		return android.PathForOutput(ctx, "framework.aidl")
	}).(android.OutputPath)
}

// Create api_fingerprint.txt
func createAPIFingerprint(ctx android.SingletonContext) {
	out := ApiFingerprintPath(ctx)

	rule := android.NewRuleBuilder()

	rule.Command().
		Text("rm -f").Output(out)
	cmd := rule.Command()

	if ctx.Config().PlatformSdkCodename() == "REL" {
		cmd.Text("echo REL >").Output(out)
	} else if ctx.Config().IsPdkBuild() {
		// TODO: get this from the PDK artifacts?
		cmd.Text("echo PDK >").Output(out)
	} else if !ctx.Config().UnbundledBuildUsePrebuiltSdks() {
		in, err := ctx.GlobWithDeps("frameworks/base/api/*current.txt", nil)
		if err != nil {
			ctx.Errorf("error globbing API files: %s", err)
		}

		cmd.Text("cat").
			Inputs(android.PathsForSource(ctx, in)).
			Text("|")

		if runtime.GOOS == "darwin" {
			cmd.Text("md5")
		} else {
			cmd.Text("md5sum")
		}

		cmd.Text("| cut -d' ' -f1 >").
			Output(out)
	} else {
		// Unbundled build
		// TODO: use a prebuilt api_fingerprint.txt from prebuilts/sdk/current.txt once we have one
		cmd.Text("echo").
			Flag(ctx.Config().PlatformPreviewSdkVersion()).
			Text(">").
			Output(out)
	}

	rule.Build(pctx, ctx, "api_fingerprint", "generate api_fingerprint.txt")
}

func ApiFingerprintPath(ctx android.PathContext) android.OutputPath {
	return ctx.Config().Once(apiFingerprintPathKey, func() interface{} {
		return android.PathForOutput(ctx, "api_fingerprint.txt")
	}).(android.OutputPath)
}

func sdkMakeVars(ctx android.MakeVarsContext) {
	if ctx.Config().UnbundledBuildUsePrebuiltSdks() || ctx.Config().IsPdkBuild() {
		return
	}

	ctx.Strict("FRAMEWORK_AIDL", sdkFrameworkAidlPath(ctx).String())
	ctx.Strict("API_FINGERPRINT", ApiFingerprintPath(ctx).String())
}
