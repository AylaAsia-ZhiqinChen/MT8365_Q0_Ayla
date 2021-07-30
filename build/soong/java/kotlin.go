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
	"bytes"
	"encoding/base64"
	"encoding/binary"
	"strings"

	"android/soong/android"

	"github.com/google/blueprint"
)

var kotlinc = pctx.AndroidGomaStaticRule("kotlinc",
	blueprint.RuleParams{
		Command: `rm -rf "$classesDir" "$srcJarDir" "$kotlinBuildFile" && mkdir -p "$classesDir" "$srcJarDir" && ` +
			`${config.ZipSyncCmd} -d $srcJarDir -l $srcJarDir/list -f "*.java" $srcJars && ` +
			`${config.GenKotlinBuildFileCmd} $classpath $classesDir $out.rsp $srcJarDir/list > $kotlinBuildFile &&` +
			`${config.KotlincCmd} ${config.JavacHeapFlags} $kotlincFlags ` +
			`-jvm-target $kotlinJvmTarget -Xbuild-file=$kotlinBuildFile && ` +
			`${config.SoongZipCmd} -jar -o $out -C $classesDir -D $classesDir && ` +
			`rm -rf "$srcJarDir"`,
		CommandDeps: []string{
			"${config.KotlincCmd}",
			"${config.KotlinCompilerJar}",
			"${config.GenKotlinBuildFileCmd}",
			"${config.SoongZipCmd}",
			"${config.ZipSyncCmd}",
		},
		Rspfile:        "$out.rsp",
		RspfileContent: `$in`,
	},
	"kotlincFlags", "classpath", "srcJars", "srcJarDir", "classesDir", "kotlinJvmTarget", "kotlinBuildFile")

// kotlinCompile takes .java and .kt sources and srcJars, and compiles the .kt sources into a classes jar in outputFile.
func kotlinCompile(ctx android.ModuleContext, outputFile android.WritablePath,
	srcFiles, srcJars android.Paths,
	flags javaBuilderFlags) {

	var deps android.Paths
	deps = append(deps, flags.kotlincClasspath...)
	deps = append(deps, srcJars...)

	ctx.Build(pctx, android.BuildParams{
		Rule:        kotlinc,
		Description: "kotlinc",
		Output:      outputFile,
		Inputs:      srcFiles,
		Implicits:   deps,
		Args: map[string]string{
			"classpath":       flags.kotlincClasspath.FormJavaClassPath("-classpath"),
			"kotlincFlags":    flags.kotlincFlags,
			"srcJars":         strings.Join(srcJars.Strings(), " "),
			"classesDir":      android.PathForModuleOut(ctx, "kotlinc", "classes").String(),
			"srcJarDir":       android.PathForModuleOut(ctx, "kotlinc", "srcJars").String(),
			"kotlinBuildFile": android.PathForModuleOut(ctx, "kotlinc-build.xml").String(),
			// http://b/69160377 kotlinc only supports -jvm-target 1.6 and 1.8
			"kotlinJvmTarget": "1.8",
		},
	})
}

var kapt = pctx.AndroidGomaStaticRule("kapt",
	blueprint.RuleParams{
		Command: `rm -rf "$srcJarDir" "$kotlinBuildFile" "$kaptDir" && mkdir -p "$srcJarDir" "$kaptDir" && ` +
			`${config.ZipSyncCmd} -d $srcJarDir -l $srcJarDir/list -f "*.java" $srcJars && ` +
			`${config.GenKotlinBuildFileCmd} $classpath "" $out.rsp $srcJarDir/list > $kotlinBuildFile &&` +
			`${config.KotlincCmd} ${config.KotlincSuppressJDK9Warnings} ${config.JavacHeapFlags} $kotlincFlags ` +
			`-Xplugin=${config.KotlinKaptJar} ` +
			`-P plugin:org.jetbrains.kotlin.kapt3:sources=$kaptDir/sources ` +
			`-P plugin:org.jetbrains.kotlin.kapt3:classes=$kaptDir/classes ` +
			`-P plugin:org.jetbrains.kotlin.kapt3:stubs=$kaptDir/stubs ` +
			`-P plugin:org.jetbrains.kotlin.kapt3:correctErrorTypes=true ` +
			`-P plugin:org.jetbrains.kotlin.kapt3:aptMode=stubsAndApt ` +
			`-P plugin:org.jetbrains.kotlin.kapt3:javacArguments=$encodedJavacFlags ` +
			`$kaptProcessorPath ` +
			`$kaptProcessor ` +
			`-Xbuild-file=$kotlinBuildFile && ` +
			`${config.SoongZipCmd} -jar -o $out -C $kaptDir/sources -D $kaptDir/sources && ` +
			`rm -rf "$srcJarDir"`,
		CommandDeps: []string{
			"${config.KotlincCmd}",
			"${config.KotlinCompilerJar}",
			"${config.KotlinKaptJar}",
			"${config.GenKotlinBuildFileCmd}",
			"${config.SoongZipCmd}",
			"${config.ZipSyncCmd}",
		},
		Rspfile:        "$out.rsp",
		RspfileContent: `$in`,
	},
	"kotlincFlags", "encodedJavacFlags", "kaptProcessorPath", "kaptProcessor",
	"classpath", "srcJars", "srcJarDir", "kaptDir", "kotlinJvmTarget", "kotlinBuildFile")

// kotlinKapt performs Kotlin-compatible annotation processing.  It takes .kt and .java sources and srcjars, and runs
// annotation processors over all of them, producing a srcjar of generated code in outputFile.  The srcjar should be
// added as an additional input to kotlinc and javac rules, and the javac rule should have annotation processing
// disabled.
func kotlinKapt(ctx android.ModuleContext, outputFile android.WritablePath,
	srcFiles, srcJars android.Paths,
	flags javaBuilderFlags) {

	var deps android.Paths
	deps = append(deps, flags.kotlincClasspath...)
	deps = append(deps, srcJars...)
	deps = append(deps, flags.processorPath...)

	kaptProcessorPath := flags.processorPath.FormTurbineClasspath("-P plugin:org.jetbrains.kotlin.kapt3:apclasspath=")

	kaptProcessor := ""
	if flags.processor != "" {
		kaptProcessor = "-P plugin:org.jetbrains.kotlin.kapt3:processors=" + flags.processor
	}

	encodedJavacFlags := kaptEncodeFlags([][2]string{
		{"-source", flags.javaVersion},
		{"-target", flags.javaVersion},
	})

	ctx.Build(pctx, android.BuildParams{
		Rule:        kapt,
		Description: "kapt",
		Output:      outputFile,
		Inputs:      srcFiles,
		Implicits:   deps,
		Args: map[string]string{
			"classpath":         flags.kotlincClasspath.FormJavaClassPath("-classpath"),
			"kotlincFlags":      flags.kotlincFlags,
			"srcJars":           strings.Join(srcJars.Strings(), " "),
			"srcJarDir":         android.PathForModuleOut(ctx, "kapt", "srcJars").String(),
			"kotlinBuildFile":   android.PathForModuleOut(ctx, "kapt", "build.xml").String(),
			"kaptProcessorPath": strings.Join(kaptProcessorPath, " "),
			"kaptProcessor":     kaptProcessor,
			"kaptDir":           android.PathForModuleOut(ctx, "kapt/gen").String(),
			"encodedJavacFlags": encodedJavacFlags,
		},
	})
}

// kapt converts a list of key, value pairs into a base64 encoded Java serialization, which is what kapt expects.
func kaptEncodeFlags(options [][2]string) string {
	buf := &bytes.Buffer{}

	binary.Write(buf, binary.BigEndian, uint32(len(options)))
	for _, option := range options {
		binary.Write(buf, binary.BigEndian, uint16(len(option[0])))
		buf.WriteString(option[0])
		binary.Write(buf, binary.BigEndian, uint16(len(option[1])))
		buf.WriteString(option[1])
	}

	header := &bytes.Buffer{}
	header.Write([]byte{0xac, 0xed, 0x00, 0x05}) // java serialization header

	if buf.Len() < 256 {
		header.WriteByte(0x77) // blockdata
		header.WriteByte(byte(buf.Len()))
	} else {
		header.WriteByte(0x7a) // blockdatalong
		binary.Write(header, binary.BigEndian, uint32(buf.Len()))
	}

	return base64.StdEncoding.EncodeToString(append(header.Bytes(), buf.Bytes()...))
}
