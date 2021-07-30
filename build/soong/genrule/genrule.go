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

package genrule

import (
	"fmt"
	"io"
	"strings"

	"github.com/google/blueprint"
	"github.com/google/blueprint/bootstrap"
	"github.com/google/blueprint/proptools"

	"android/soong/android"
	"android/soong/shared"
	"path/filepath"
)

func init() {
	android.RegisterModuleType("genrule_defaults", defaultsFactory)

	android.RegisterModuleType("gensrcs", GenSrcsFactory)
	android.RegisterModuleType("genrule", GenRuleFactory)
}

var (
	pctx = android.NewPackageContext("android/soong/genrule")
)

func init() {
	pctx.HostBinToolVariable("sboxCmd", "sbox")
}

type SourceFileGenerator interface {
	GeneratedSourceFiles() android.Paths
	GeneratedHeaderDirs() android.Paths
	GeneratedDeps() android.Paths
}

// Alias for android.HostToolProvider
// Deprecated: use android.HostToolProvider instead.
type HostToolProvider interface {
	android.HostToolProvider
}

type hostToolDependencyTag struct {
	blueprint.BaseDependencyTag
	label string
}

type generatorProperties struct {
	// The command to run on one or more input files. Cmd supports substitution of a few variables
	// (the actual substitution is implemented in GenerateAndroidBuildActions below)
	//
	// Available variables for substitution:
	//
	//  $(location): the path to the first entry in tools or tool_files
	//  $(location <label>): the path to the tool, tool_file, input or output with name <label>
	//  $(in): one or more input files
	//  $(out): a single output file
	//  $(depfile): a file to which dependencies will be written, if the depfile property is set to true
	//  $(genDir): the sandbox directory for this tool; contains $(out)
	//  $$: a literal $
	//
	// All files used must be declared as inputs (to ensure proper up-to-date checks).
	// Use "$(in)" directly in Cmd to ensure that all inputs used are declared.
	Cmd *string

	// Enable reading a file containing dependencies in gcc format after the command completes
	Depfile *bool

	// name of the modules (if any) that produces the host executable.   Leave empty for
	// prebuilts or scripts that do not need a module to build them.
	Tools []string

	// Local file that is used as the tool
	Tool_files []string `android:"path"`

	// List of directories to export generated headers from
	Export_include_dirs []string

	// list of input files
	Srcs []string `android:"path,arch_variant"`

	// input files to exclude
	Exclude_srcs []string `android:"path,arch_variant"`
}

type Module struct {
	android.ModuleBase
	android.DefaultableModuleBase

	// For other packages to make their own genrules with extra
	// properties
	Extra interface{}

	properties generatorProperties

	taskGenerator taskFunc

	deps       android.Paths
	rule       blueprint.Rule
	rawCommand string

	exportedIncludeDirs android.Paths

	outputFiles android.Paths
	outputDeps  android.Paths

	subName string
}

type taskFunc func(ctx android.ModuleContext, rawCommand string, srcFiles android.Paths) generateTask

type generateTask struct {
	in          android.Paths
	out         android.WritablePaths
	sandboxOuts []string
	cmd         string
}

func (g *Module) GeneratedSourceFiles() android.Paths {
	return g.outputFiles
}

func (g *Module) Srcs() android.Paths {
	return append(android.Paths{}, g.outputFiles...)
}

func (g *Module) GeneratedHeaderDirs() android.Paths {
	return g.exportedIncludeDirs
}

func (g *Module) GeneratedDeps() android.Paths {
	return g.outputDeps
}

func (g *Module) DepsMutator(ctx android.BottomUpMutatorContext) {
	if g, ok := ctx.Module().(*Module); ok {
		for _, tool := range g.properties.Tools {
			tag := hostToolDependencyTag{label: tool}
			if m := android.SrcIsModule(tool); m != "" {
				tool = m
			}
			ctx.AddFarVariationDependencies([]blueprint.Variation{
				{Mutator: "arch", Variation: ctx.Config().BuildOsVariant},
			}, tag, tool)
		}
	}
}

func (g *Module) GenerateAndroidBuildActions(ctx android.ModuleContext) {
	g.subName = ctx.ModuleSubDir()

	if len(g.properties.Export_include_dirs) > 0 {
		for _, dir := range g.properties.Export_include_dirs {
			g.exportedIncludeDirs = append(g.exportedIncludeDirs,
				android.PathForModuleGen(ctx, ctx.ModuleDir(), dir))
		}
	} else {
		g.exportedIncludeDirs = append(g.exportedIncludeDirs, android.PathForModuleGen(ctx, ""))
	}

	locationLabels := map[string][]string{}
	firstLabel := ""

	addLocationLabel := func(label string, paths []string) {
		if firstLabel == "" {
			firstLabel = label
		}
		if _, exists := locationLabels[label]; !exists {
			locationLabels[label] = paths
		} else {
			ctx.ModuleErrorf("multiple labels for %q, %q and %q",
				label, strings.Join(locationLabels[label], " "), strings.Join(paths, " "))
		}
	}

	if len(g.properties.Tools) > 0 {
		seenTools := make(map[string]bool)

		ctx.VisitDirectDepsBlueprint(func(module blueprint.Module) {
			switch tag := ctx.OtherModuleDependencyTag(module).(type) {
			case hostToolDependencyTag:
				tool := ctx.OtherModuleName(module)
				var path android.OptionalPath

				if t, ok := module.(android.HostToolProvider); ok {
					if !t.(android.Module).Enabled() {
						if ctx.Config().AllowMissingDependencies() {
							ctx.AddMissingDependencies([]string{tool})
						} else {
							ctx.ModuleErrorf("depends on disabled module %q", tool)
						}
						break
					}
					path = t.HostToolPath()
				} else if t, ok := module.(bootstrap.GoBinaryTool); ok {
					if s, err := filepath.Rel(android.PathForOutput(ctx).String(), t.InstallPath()); err == nil {
						path = android.OptionalPathForPath(android.PathForOutput(ctx, s))
					} else {
						ctx.ModuleErrorf("cannot find path for %q: %v", tool, err)
						break
					}
				} else {
					ctx.ModuleErrorf("%q is not a host tool provider", tool)
					break
				}

				if path.Valid() {
					g.deps = append(g.deps, path.Path())
					addLocationLabel(tag.label, []string{path.Path().String()})
					seenTools[tag.label] = true
				} else {
					ctx.ModuleErrorf("host tool %q missing output file", tool)
				}
			}
		})

		// If AllowMissingDependencies is enabled, the build will not have stopped when
		// AddFarVariationDependencies was called on a missing tool, which will result in nonsensical
		// "cmd: unknown location label ..." errors later.  Add a dummy file to the local label.  The
		// command that uses this dummy file will never be executed because the rule will be replaced with
		// an android.Error rule reporting the missing dependencies.
		if ctx.Config().AllowMissingDependencies() {
			for _, tool := range g.properties.Tools {
				if !seenTools[tool] {
					addLocationLabel(tool, []string{"***missing tool " + tool + "***"})
				}
			}
		}
	}

	if ctx.Failed() {
		return
	}

	for _, toolFile := range g.properties.Tool_files {
		paths := android.PathsForModuleSrc(ctx, []string{toolFile})
		g.deps = append(g.deps, paths...)
		addLocationLabel(toolFile, paths.Strings())
	}

	var srcFiles android.Paths
	for _, in := range g.properties.Srcs {
		paths, missingDeps := android.PathsAndMissingDepsForModuleSrcExcludes(ctx, []string{in}, g.properties.Exclude_srcs)
		if len(missingDeps) > 0 {
			if !ctx.Config().AllowMissingDependencies() {
				panic(fmt.Errorf("should never get here, the missing dependencies %q should have been reported in DepsMutator",
					missingDeps))
			}

			// If AllowMissingDependencies is enabled, the build will not have stopped when
			// the dependency was added on a missing SourceFileProducer module, which will result in nonsensical
			// "cmd: label ":..." has no files" errors later.  Add a dummy file to the local label.  The
			// command that uses this dummy file will never be executed because the rule will be replaced with
			// an android.Error rule reporting the missing dependencies.
			ctx.AddMissingDependencies(missingDeps)
			addLocationLabel(in, []string{"***missing srcs " + in + "***"})
		} else {
			srcFiles = append(srcFiles, paths...)
			addLocationLabel(in, paths.Strings())
		}
	}

	task := g.taskGenerator(ctx, String(g.properties.Cmd), srcFiles)

	for _, out := range task.out {
		addLocationLabel(out.Rel(), []string{filepath.Join("__SBOX_OUT_DIR__", out.Rel())})
	}

	referencedDepfile := false

	rawCommand, err := android.Expand(task.cmd, func(name string) (string, error) {
		// report the error directly without returning an error to android.Expand to catch multiple errors in a
		// single run
		reportError := func(fmt string, args ...interface{}) (string, error) {
			ctx.PropertyErrorf("cmd", fmt, args...)
			return "SOONG_ERROR", nil
		}

		switch name {
		case "location":
			if len(g.properties.Tools) == 0 && len(g.properties.Tool_files) == 0 {
				return reportError("at least one `tools` or `tool_files` is required if $(location) is used")
			}
			paths := locationLabels[firstLabel]
			if len(paths) == 0 {
				return reportError("default label %q has no files", firstLabel)
			} else if len(paths) > 1 {
				return reportError("default label %q has multiple files, use $(locations %s) to reference it",
					firstLabel, firstLabel)
			}
			return locationLabels[firstLabel][0], nil
		case "in":
			return "${in}", nil
		case "out":
			return "__SBOX_OUT_FILES__", nil
		case "depfile":
			referencedDepfile = true
			if !Bool(g.properties.Depfile) {
				return reportError("$(depfile) used without depfile property")
			}
			return "__SBOX_DEPFILE__", nil
		case "genDir":
			return "__SBOX_OUT_DIR__", nil
		default:
			if strings.HasPrefix(name, "location ") {
				label := strings.TrimSpace(strings.TrimPrefix(name, "location "))
				if paths, ok := locationLabels[label]; ok {
					if len(paths) == 0 {
						return reportError("label %q has no files", label)
					} else if len(paths) > 1 {
						return reportError("label %q has multiple files, use $(locations %s) to reference it",
							label, label)
					}
					return paths[0], nil
				} else {
					return reportError("unknown location label %q", label)
				}
			} else if strings.HasPrefix(name, "locations ") {
				label := strings.TrimSpace(strings.TrimPrefix(name, "locations "))
				if paths, ok := locationLabels[label]; ok {
					if len(paths) == 0 {
						return reportError("label %q has no files", label)
					}
					return strings.Join(paths, " "), nil
				} else {
					return reportError("unknown locations label %q", label)
				}
			} else {
				return reportError("unknown variable '$(%s)'", name)
			}
		}
	})

	if err != nil {
		ctx.PropertyErrorf("cmd", "%s", err.Error())
		return
	}

	if Bool(g.properties.Depfile) && !referencedDepfile {
		ctx.PropertyErrorf("cmd", "specified depfile=true but did not include a reference to '${depfile}' in cmd")
	}

	// tell the sbox command which directory to use as its sandbox root
	buildDir := android.PathForOutput(ctx).String()
	sandboxPath := shared.TempDirForOutDir(buildDir)

	// recall that Sprintf replaces percent sign expressions, whereas dollar signs expressions remain as written,
	// to be replaced later by ninja_strings.go
	depfilePlaceholder := ""
	if Bool(g.properties.Depfile) {
		depfilePlaceholder = "$depfileArgs"
	}

	genDir := android.PathForModuleGen(ctx)
	// Escape the command for the shell
	rawCommand = "'" + strings.Replace(rawCommand, "'", `'\''`, -1) + "'"
	g.rawCommand = rawCommand
	sandboxCommand := fmt.Sprintf("$sboxCmd --sandbox-path %s --output-root %s -c %s %s $allouts",
		sandboxPath, genDir, rawCommand, depfilePlaceholder)

	ruleParams := blueprint.RuleParams{
		Command:     sandboxCommand,
		CommandDeps: []string{"$sboxCmd"},
	}
	args := []string{"allouts"}
	if Bool(g.properties.Depfile) {
		ruleParams.Deps = blueprint.DepsGCC
		args = append(args, "depfileArgs")
	}
	g.rule = ctx.Rule(pctx, "generator", ruleParams, args...)

	g.generateSourceFile(ctx, task)

}

func (g *Module) generateSourceFile(ctx android.ModuleContext, task generateTask) {
	desc := "generate"
	if len(task.out) == 0 {
		ctx.ModuleErrorf("must have at least one output file")
		return
	}
	if len(task.out) == 1 {
		desc += " " + task.out[0].Base()
	}

	var depFile android.ModuleGenPath
	if Bool(g.properties.Depfile) {
		depFile = android.PathForModuleGen(ctx, task.out[0].Rel()+".d")
	}

	params := android.BuildParams{
		Rule:            g.rule,
		Description:     "generate",
		Output:          task.out[0],
		ImplicitOutputs: task.out[1:],
		Inputs:          task.in,
		Implicits:       g.deps,
		Args: map[string]string{
			"allouts": strings.Join(task.sandboxOuts, " "),
		},
	}
	if Bool(g.properties.Depfile) {
		params.Depfile = android.PathForModuleGen(ctx, task.out[0].Rel()+".d")
		params.Args["depfileArgs"] = "--depfile-out " + depFile.String()
	}

	ctx.Build(pctx, params)

	for _, outputFile := range task.out {
		g.outputFiles = append(g.outputFiles, outputFile)
	}
	g.outputDeps = append(g.outputDeps, task.out[0])
}

// Collect information for opening IDE project files in java/jdeps.go.
func (g *Module) IDEInfo(dpInfo *android.IdeInfo) {
	dpInfo.Srcs = append(dpInfo.Srcs, g.Srcs().Strings()...)
	for _, src := range g.properties.Srcs {
		if strings.HasPrefix(src, ":") {
			src = strings.Trim(src, ":")
			dpInfo.Deps = append(dpInfo.Deps, src)
		}
	}
}

func (g *Module) AndroidMk() android.AndroidMkData {
	return android.AndroidMkData{
		Include:    "$(BUILD_PHONY_PACKAGE)",
		Class:      "FAKE",
		OutputFile: android.OptionalPathForPath(g.outputFiles[0]),
		SubName:    g.subName,
		Extra: []android.AndroidMkExtraFunc{
			func(w io.Writer, outputFile android.Path) {
				fmt.Fprintln(w, "LOCAL_ADDITIONAL_DEPENDENCIES :=", strings.Join(g.outputFiles.Strings(), " "))
			},
		},
		Custom: func(w io.Writer, name, prefix, moduleDir string, data android.AndroidMkData) {
			android.WriteAndroidMkData(w, data)
			if data.SubName != "" {
				fmt.Fprintln(w, ".PHONY:", name)
				fmt.Fprintln(w, name, ":", name+g.subName)
			}
		},
	}
}

func generatorFactory(taskGenerator taskFunc, props ...interface{}) *Module {
	module := &Module{
		taskGenerator: taskGenerator,
	}

	module.AddProperties(props...)
	module.AddProperties(&module.properties)

	return module
}

// replace "out" with "__SBOX_OUT_DIR__/<the value of ${out}>"
func pathToSandboxOut(path android.Path, genDir android.Path) string {
	relOut, err := filepath.Rel(genDir.String(), path.String())
	if err != nil {
		panic(fmt.Sprintf("Could not make ${out} relative: %v", err))
	}
	return filepath.Join("__SBOX_OUT_DIR__", relOut)

}

func NewGenSrcs() *Module {
	properties := &genSrcsProperties{}

	taskGenerator := func(ctx android.ModuleContext, rawCommand string, srcFiles android.Paths) generateTask {
		commands := []string{}
		outFiles := android.WritablePaths{}
		genDir := android.PathForModuleGen(ctx)
		sandboxOuts := []string{}
		for _, in := range srcFiles {
			outFile := android.GenPathWithExt(ctx, "", in, String(properties.Output_extension))
			outFiles = append(outFiles, outFile)

			sandboxOutfile := pathToSandboxOut(outFile, genDir)
			sandboxOuts = append(sandboxOuts, sandboxOutfile)

			command, err := android.Expand(rawCommand, func(name string) (string, error) {
				switch name {
				case "in":
					return in.String(), nil
				case "out":
					return sandboxOutfile, nil
				default:
					return "$(" + name + ")", nil
				}
			})
			if err != nil {
				ctx.PropertyErrorf("cmd", err.Error())
			}

			// escape the command in case for example it contains '#', an odd number of '"', etc
			command = fmt.Sprintf("bash -c %v", proptools.ShellEscape(command))
			commands = append(commands, command)
		}
		fullCommand := strings.Join(commands, " && ")

		return generateTask{
			in:          srcFiles,
			out:         outFiles,
			sandboxOuts: sandboxOuts,
			cmd:         fullCommand,
		}
	}

	return generatorFactory(taskGenerator, properties)
}

func GenSrcsFactory() android.Module {
	m := NewGenSrcs()
	android.InitAndroidModule(m)
	return m
}

type genSrcsProperties struct {
	// extension that will be substituted for each output file
	Output_extension *string
}

func NewGenRule() *Module {
	properties := &genRuleProperties{}

	taskGenerator := func(ctx android.ModuleContext, rawCommand string, srcFiles android.Paths) generateTask {
		outs := make(android.WritablePaths, len(properties.Out))
		sandboxOuts := make([]string, len(properties.Out))
		genDir := android.PathForModuleGen(ctx)
		for i, out := range properties.Out {
			outs[i] = android.PathForModuleGen(ctx, out)
			sandboxOuts[i] = pathToSandboxOut(outs[i], genDir)
		}
		return generateTask{
			in:          srcFiles,
			out:         outs,
			sandboxOuts: sandboxOuts,
			cmd:         rawCommand,
		}
	}

	return generatorFactory(taskGenerator, properties)
}

func GenRuleFactory() android.Module {
	m := NewGenRule()
	android.InitAndroidModule(m)
	android.InitDefaultableModule(m)
	return m
}

type genRuleProperties struct {
	// names of the output files that will be generated
	Out []string `android:"arch_variant"`
}

var Bool = proptools.Bool
var String = proptools.String

//
// Defaults
//
type Defaults struct {
	android.ModuleBase
	android.DefaultsModuleBase
}

func (*Defaults) GenerateAndroidBuildActions(ctx android.ModuleContext) {
}

func defaultsFactory() android.Module {
	return DefaultsFactory()
}

func DefaultsFactory(props ...interface{}) android.Module {
	module := &Defaults{}

	module.AddProperties(props...)
	module.AddProperties(
		&generatorProperties{},
		&genRuleProperties{},
	)

	android.InitDefaultsModule(module)

	return module
}
