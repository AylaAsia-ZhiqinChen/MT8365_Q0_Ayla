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
	"android/soong/android"
	"android/soong/genrule"
)

func init() {
	android.RegisterModuleType("cc_genrule", genRuleFactory)
}

type GenruleExtraProperties struct {
	Vendor_available   *bool
	Recovery_available *bool

	// This genrule is for recovery variant
	InRecovery bool `blueprint:"mutated"`
}

// cc_genrule is a genrule that can depend on other cc_* objects.
// The cmd may be run multiple times, once for each of the different arch/etc
// variations.
func genRuleFactory() android.Module {
	module := genrule.NewGenRule()

	module.Extra = &GenruleExtraProperties{}
	module.AddProperties(module.Extra)

	android.InitAndroidArchModule(module, android.HostAndDeviceSupported, android.MultilibBoth)

	return module
}
