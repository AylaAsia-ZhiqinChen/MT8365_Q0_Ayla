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

package android

import (
	"encoding/json"
	"strconv"
)

func init() {
	RegisterSingletonType("api_levels", ApiLevelsSingleton)
}

func ApiLevelsSingleton() Singleton {
	return &apiLevelsSingleton{}
}

type apiLevelsSingleton struct{}

func createApiLevelsJson(ctx SingletonContext, file WritablePath,
	apiLevelsMap map[string]int) {

	jsonStr, err := json.Marshal(apiLevelsMap)
	if err != nil {
		ctx.Errorf(err.Error())
	}

	ctx.Build(pctx, BuildParams{
		Rule:        WriteFile,
		Description: "generate " + file.Base(),
		Output:      file,
		Args: map[string]string{
			"content": string(jsonStr[:]),
		},
	})
}

func GetApiLevelsJson(ctx PathContext) WritablePath {
	return PathForOutput(ctx, "api_levels.json")
}

var apiLevelsMapKey = NewOnceKey("ApiLevelsMap")

func getApiLevelsMap(config Config) map[string]int {
	return config.Once(apiLevelsMapKey, func() interface{} {
		baseApiLevel := 9000
		apiLevelsMap := map[string]int{
			"G":     9,
			"I":     14,
			"J":     16,
			"J-MR1": 17,
			"J-MR2": 18,
			"K":     19,
			"L":     21,
			"L-MR1": 22,
			"M":     23,
			"N":     24,
			"N-MR1": 25,
			"O":     26,
			"O-MR1": 27,
			"P":     28,
			"Q":     29,
		}
		for i, codename := range config.PlatformVersionCombinedCodenames() {
			apiLevelsMap[codename] = baseApiLevel + i
		}

		return apiLevelsMap
	}).(map[string]int)
}

// Converts an API level string into its numeric form.
// * Codenames are decoded.
// * Numeric API levels are simply converted.
// * "minimum" and "current" are not currently handled since the former is
//   NDK specific and the latter has inconsistent meaning.
func ApiStrToNum(ctx BaseContext, apiLevel string) (int, error) {
	num, ok := getApiLevelsMap(ctx.Config())[apiLevel]
	if ok {
		return num, nil
	}
	return strconv.Atoi(apiLevel)
}

func (a *apiLevelsSingleton) GenerateBuildActions(ctx SingletonContext) {
	apiLevelsMap := getApiLevelsMap(ctx.Config())
	apiLevelsJson := GetApiLevelsJson(ctx)
	createApiLevelsJson(ctx, apiLevelsJson, apiLevelsMap)
}
