// Copyright 2015 Google Inc. All rights reserved.
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
	"fmt"
	"path/filepath"
	"regexp"
	"strings"

	"android/soong/android"
)

// Efficiently converts a list of include directories to a single string
// of cflags with -I prepended to each directory.
func includeDirsToFlags(dirs android.Paths) string {
	return android.JoinWithPrefix(dirs.Strings(), "-I")
}

func includeFilesToFlags(files android.Paths) string {
	return android.JoinWithPrefix(files.Strings(), "-include ")
}

func ldDirsToFlags(dirs []string) string {
	return android.JoinWithPrefix(dirs, "-L")
}

func libNamesToFlags(names []string) string {
	return android.JoinWithPrefix(names, "-l")
}

var indexList = android.IndexList
var inList = android.InList
var filterList = android.FilterList
var removeListFromList = android.RemoveListFromList
var removeFromList = android.RemoveFromList

var libNameRegexp = regexp.MustCompile(`^lib(.*)$`)

func moduleToLibName(module string) (string, error) {
	matches := libNameRegexp.FindStringSubmatch(module)
	if matches == nil {
		return "", fmt.Errorf("Library module name %s does not start with lib", module)
	}
	return matches[1], nil
}

func flagsToBuilderFlags(in Flags) builderFlags {
	return builderFlags{
		globalFlags:     strings.Join(in.GlobalFlags, " "),
		arFlags:         strings.Join(in.ArFlags, " "),
		asFlags:         strings.Join(in.AsFlags, " "),
		cFlags:          strings.Join(in.CFlags, " "),
		toolingCFlags:   strings.Join(in.ToolingCFlags, " "),
		toolingCppFlags: strings.Join(in.ToolingCppFlags, " "),
		conlyFlags:      strings.Join(in.ConlyFlags, " "),
		cppFlags:        strings.Join(in.CppFlags, " "),
		yaccFlags:       strings.Join(in.YaccFlags, " "),
		aidlFlags:       strings.Join(in.aidlFlags, " "),
		rsFlags:         strings.Join(in.rsFlags, " "),
		ldFlags:         strings.Join(in.LdFlags, " "),
		libFlags:        strings.Join(in.libFlags, " "),
		tidyFlags:       strings.Join(in.TidyFlags, " "),
		sAbiFlags:       strings.Join(in.SAbiFlags, " "),
		yasmFlags:       strings.Join(in.YasmFlags, " "),
		toolchain:       in.Toolchain,
		coverage:        in.Coverage,
		tidy:            in.Tidy,
		sAbiDump:        in.SAbiDump,

		systemIncludeFlags: strings.Join(in.SystemIncludeFlags, " "),

		groupStaticLibs: in.GroupStaticLibs,

		proto:            in.proto,
		protoC:           in.protoC,
		protoOptionsFile: in.protoOptionsFile,
	}
}

func addPrefix(list []string, prefix string) []string {
	for i := range list {
		list[i] = prefix + list[i]
	}
	return list
}

func addSuffix(list []string, suffix string) []string {
	for i := range list {
		list[i] = list[i] + suffix
	}
	return list
}

var shlibVersionPattern = regexp.MustCompile("(?:\\.\\d+(?:svn)?)+")

// splitFileExt splits a file name into root, suffix and ext. root stands for the file name without
// the file extension and the version number (e.g. "libexample"). suffix stands for the
// concatenation of the file extension and the version number (e.g. ".so.1.0"). ext stands for the
// file extension after the version numbers are trimmed (e.g. ".so").
func splitFileExt(name string) (string, string, string) {
	// Extract and trim the shared lib version number if the file name ends with dot digits.
	suffix := ""
	matches := shlibVersionPattern.FindAllStringIndex(name, -1)
	if len(matches) > 0 {
		lastMatch := matches[len(matches)-1]
		if lastMatch[1] == len(name) {
			suffix = name[lastMatch[0]:lastMatch[1]]
			name = name[0:lastMatch[0]]
		}
	}

	// Extract the file name root and the file extension.
	ext := filepath.Ext(name)
	root := strings.TrimSuffix(name, ext)
	suffix = ext + suffix

	return root, suffix, ext
}

// linkDirOnDevice/linkName -> target
func makeSymlinkCmd(linkDirOnDevice string, linkName string, target string) string {
	dir := filepath.Join("$(PRODUCT_OUT)", linkDirOnDevice)
	return "mkdir -p " + dir + " && " +
		"ln -sf " + target + " " + filepath.Join(dir, linkName)
}
