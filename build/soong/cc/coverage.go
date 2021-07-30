// Copyright 2017 Google Inc. All rights reserved.
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

package cc

import (
	"strconv"

	"android/soong/android"
)

type CoverageProperties struct {
	Native_coverage *bool

	NeedCoverageVariant bool `blueprint:"mutated"`
	NeedCoverageBuild   bool `blueprint:"mutated"`

	CoverageEnabled   bool `blueprint:"mutated"`
	IsCoverageVariant bool `blueprint:"mutated"`
}

type coverage struct {
	Properties CoverageProperties

	// Whether binaries containing this module need --coverage added to their ldflags
	linkCoverage bool
}

func (cov *coverage) props() []interface{} {
	return []interface{}{&cov.Properties}
}

func (cov *coverage) deps(ctx BaseModuleContext, deps Deps) Deps {
	if cov.Properties.NeedCoverageBuild {
		// Link libprofile-extras/libprofile-extras_ndk when coverage
		// variant is required.  This is a no-op unless coverage is
		// actually enabled during linking, when
		// '-uinit_profile_extras' is added (in flags()) to force the
		// setup code in libprofile-extras be linked into the
		// binary/library.
		//
		// We cannot narrow it further to only the 'cov' variant since
		// the mutator hasn't run (and we don't have the 'cov' variant
		// yet).
		if !ctx.useSdk() {
			deps.LateStaticLibs = append(deps.LateStaticLibs, "libprofile-extras")
		} else {
			deps.LateStaticLibs = append(deps.LateStaticLibs, "libprofile-extras_ndk")
		}
	}
	return deps
}

func (cov *coverage) flags(ctx ModuleContext, flags Flags) Flags {
	if !ctx.DeviceConfig().NativeCoverageEnabled() {
		return flags
	}

	if cov.Properties.CoverageEnabled {
		flags.Coverage = true
		flags.GlobalFlags = append(flags.GlobalFlags, "--coverage", "-O0")
		cov.linkCoverage = true

		// Override -Wframe-larger-than and non-default optimization
		// flags that the module may use.
		flags.CFlags = append(flags.CFlags, "-Wno-frame-larger-than=", "-O0")
	}

	// Even if we don't have coverage enabled, if any of our object files were compiled
	// with coverage, then we need to add --coverage to our ldflags.
	if !cov.linkCoverage {
		if ctx.static() && !ctx.staticBinary() {
			// For static libraries, the only thing that changes our object files
			// are included whole static libraries, so check to see if any of
			// those have coverage enabled.
			ctx.VisitDirectDepsWithTag(wholeStaticDepTag, func(m android.Module) {
				if cc, ok := m.(*Module); ok && cc.coverage != nil {
					if cc.coverage.linkCoverage {
						cov.linkCoverage = true
					}
				}
			})
		} else {
			// For executables and shared libraries, we need to check all of
			// our static dependencies.
			ctx.VisitDirectDeps(func(m android.Module) {
				cc, ok := m.(*Module)
				if !ok || cc.coverage == nil {
					return
				}

				if static, ok := cc.linker.(libraryInterface); !ok || !static.static() {
					return
				}

				if cc.coverage.linkCoverage {
					cov.linkCoverage = true
				}
			})
		}
	}

	if cov.linkCoverage {
		flags.LdFlags = append(flags.LdFlags, "--coverage")

		// Force linking of constructor/setup code in libprofile-extras
		flags.LdFlags = append(flags.LdFlags, "-uinit_profile_extras")
	}

	return flags
}

func (cov *coverage) begin(ctx BaseModuleContext) {
	// Coverage is disabled globally
	if !ctx.DeviceConfig().NativeCoverageEnabled() {
		return
	}

	var needCoverageVariant bool
	var needCoverageBuild bool

	if ctx.Host() {
		// TODO(dwillemsen): because of -nodefaultlibs, we must depend on libclang_rt.profile-*.a
		// Just turn off for now.
	} else if !ctx.nativeCoverage() {
		// Native coverage is not supported for this module type.
	} else {
		// Check if Native_coverage is set to false.  This property defaults to true.
		needCoverageVariant = BoolDefault(cov.Properties.Native_coverage, true)

		if sdk_version := ctx.sdkVersion(); ctx.useSdk() && sdk_version != "current" {
			// Native coverage is not supported for SDK versions < 23
			if fromApi, err := strconv.Atoi(sdk_version); err == nil && fromApi < 23 {
				needCoverageVariant = false
			}
		}

		if needCoverageVariant {
			// Coverage variant is actually built with coverage if enabled for its module path
			needCoverageBuild = ctx.DeviceConfig().CoverageEnabledForPath(ctx.ModuleDir())
		}
	}

	cov.Properties.NeedCoverageBuild = needCoverageBuild
	cov.Properties.NeedCoverageVariant = needCoverageVariant
}

func coverageMutator(mctx android.BottomUpMutatorContext) {
	if c, ok := mctx.Module().(*Module); ok && c.coverage != nil {
		needCoverageVariant := c.coverage.Properties.NeedCoverageVariant
		needCoverageBuild := c.coverage.Properties.NeedCoverageBuild
		if needCoverageVariant {
			m := mctx.CreateVariations("", "cov")

			// Setup the non-coverage version and set HideFromMake and
			// PreventInstall to true.
			m[0].(*Module).coverage.Properties.CoverageEnabled = false
			m[0].(*Module).coverage.Properties.IsCoverageVariant = false
			m[0].(*Module).Properties.HideFromMake = true
			m[0].(*Module).Properties.PreventInstall = true

			// The coverage-enabled version inherits HideFromMake,
			// PreventInstall from the original module.
			m[1].(*Module).coverage.Properties.CoverageEnabled = needCoverageBuild
			m[1].(*Module).coverage.Properties.IsCoverageVariant = true
		}
	}
}
