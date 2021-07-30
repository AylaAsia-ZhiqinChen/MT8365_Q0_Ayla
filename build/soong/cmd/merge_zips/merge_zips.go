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

package main

import (
	"errors"
	"flag"
	"fmt"
	"hash/crc32"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"sort"

	"github.com/google/blueprint/pathtools"

	"android/soong/jar"
	"android/soong/third_party/zip"
)

type fileList []string

func (f *fileList) String() string {
	return `""`
}

func (f *fileList) Set(name string) error {
	*f = append(*f, filepath.Clean(name))

	return nil
}

type zipsToNotStripSet map[string]bool

func (s zipsToNotStripSet) String() string {
	return `""`
}

func (s zipsToNotStripSet) Set(zip_path string) error {
	s[zip_path] = true

	return nil
}

var (
	sortEntries      = flag.Bool("s", false, "sort entries (defaults to the order from the input zip files)")
	emulateJar       = flag.Bool("j", false, "sort zip entries using jar ordering (META-INF first)")
	emulatePar       = flag.Bool("p", false, "merge zip entries based on par format")
	stripDirs        fileList
	stripFiles       fileList
	zipsToNotStrip   = make(zipsToNotStripSet)
	stripDirEntries  = flag.Bool("D", false, "strip directory entries from the output zip file")
	manifest         = flag.String("m", "", "manifest file to insert in jar")
	pyMain           = flag.String("pm", "", "__main__.py file to insert in par")
	prefix           = flag.String("prefix", "", "A file to prefix to the zip file")
	ignoreDuplicates = flag.Bool("ignore-duplicates", false, "take each entry from the first zip it exists in and don't warn")
)

func init() {
	flag.Var(&stripDirs, "stripDir", "directories to be excluded from the output zip, accepts wildcards")
	flag.Var(&stripFiles, "stripFile", "files to be excluded from the output zip, accepts wildcards")
	flag.Var(&zipsToNotStrip, "zipToNotStrip", "the input zip file which is not applicable for stripping")
}

func main() {
	flag.Usage = func() {
		fmt.Fprintln(os.Stderr, "usage: merge_zips [-jpsD] [-m manifest] [--prefix script] [-pm __main__.py] output [inputs...]")
		flag.PrintDefaults()
	}

	// parse args
	flag.Parse()
	args := flag.Args()
	if len(args) < 1 {
		flag.Usage()
		os.Exit(1)
	}
	outputPath := args[0]
	inputs := args[1:]

	log.SetFlags(log.Lshortfile)

	// make writer
	output, err := os.Create(outputPath)
	if err != nil {
		log.Fatal(err)
	}
	defer output.Close()

	var offset int64
	if *prefix != "" {
		prefixFile, err := os.Open(*prefix)
		if err != nil {
			log.Fatal(err)
		}
		offset, err = io.Copy(output, prefixFile)
		if err != nil {
			log.Fatal(err)
		}
	}

	writer := zip.NewWriter(output)
	defer func() {
		err := writer.Close()
		if err != nil {
			log.Fatal(err)
		}
	}()
	writer.SetOffset(offset)

	// make readers
	readers := []namedZipReader{}
	for _, input := range inputs {
		reader, err := zip.OpenReader(input)
		if err != nil {
			log.Fatal(err)
		}
		defer reader.Close()
		namedReader := namedZipReader{path: input, reader: &reader.Reader}
		readers = append(readers, namedReader)
	}

	if *manifest != "" && !*emulateJar {
		log.Fatal(errors.New("must specify -j when specifying a manifest via -m"))
	}

	if *pyMain != "" && !*emulatePar {
		log.Fatal(errors.New("must specify -p when specifying a Python __main__.py via -pm"))
	}

	// do merge
	err = mergeZips(readers, writer, *manifest, *pyMain, *sortEntries, *emulateJar, *emulatePar,
		*stripDirEntries, *ignoreDuplicates, []string(stripFiles), []string(stripDirs), map[string]bool(zipsToNotStrip))
	if err != nil {
		log.Fatal(err)
	}
}

// a namedZipReader reads a .zip file and can say which file it's reading
type namedZipReader struct {
	path   string
	reader *zip.Reader
}

// a zipEntryPath refers to a file contained in a zip
type zipEntryPath struct {
	zipName   string
	entryName string
}

func (p zipEntryPath) String() string {
	return p.zipName + "/" + p.entryName
}

// a zipEntry is a zipSource that pulls its content from another zip
type zipEntry struct {
	path    zipEntryPath
	content *zip.File
}

func (ze zipEntry) String() string {
	return ze.path.String()
}

func (ze zipEntry) IsDir() bool {
	return ze.content.FileInfo().IsDir()
}

func (ze zipEntry) CRC32() uint32 {
	return ze.content.FileHeader.CRC32
}

func (ze zipEntry) Size() uint64 {
	return ze.content.FileHeader.UncompressedSize64
}

func (ze zipEntry) WriteToZip(dest string, zw *zip.Writer) error {
	return zw.CopyFrom(ze.content, dest)
}

// a bufferEntry is a zipSource that pulls its content from a []byte
type bufferEntry struct {
	fh      *zip.FileHeader
	content []byte
}

func (be bufferEntry) String() string {
	return "internal buffer"
}

func (be bufferEntry) IsDir() bool {
	return be.fh.FileInfo().IsDir()
}

func (be bufferEntry) CRC32() uint32 {
	return crc32.ChecksumIEEE(be.content)
}

func (be bufferEntry) Size() uint64 {
	return uint64(len(be.content))
}

func (be bufferEntry) WriteToZip(dest string, zw *zip.Writer) error {
	w, err := zw.CreateHeader(be.fh)
	if err != nil {
		return err
	}

	if !be.IsDir() {
		_, err = w.Write(be.content)
		if err != nil {
			return err
		}
	}

	return nil
}

type zipSource interface {
	String() string
	IsDir() bool
	CRC32() uint32
	Size() uint64
	WriteToZip(dest string, zw *zip.Writer) error
}

// a fileMapping specifies to copy a zip entry from one place to another
type fileMapping struct {
	dest   string
	source zipSource
}

func mergeZips(readers []namedZipReader, writer *zip.Writer, manifest, pyMain string,
	sortEntries, emulateJar, emulatePar, stripDirEntries, ignoreDuplicates bool,
	stripFiles, stripDirs []string, zipsToNotStrip map[string]bool) error {

	sourceByDest := make(map[string]zipSource, 0)
	orderedMappings := []fileMapping{}

	// if dest already exists returns a non-null zipSource for the existing source
	addMapping := func(dest string, source zipSource) zipSource {
		mapKey := filepath.Clean(dest)
		if existingSource, exists := sourceByDest[mapKey]; exists {
			return existingSource
		}

		sourceByDest[mapKey] = source
		orderedMappings = append(orderedMappings, fileMapping{source: source, dest: dest})
		return nil
	}

	if manifest != "" {
		if !stripDirEntries {
			dirHeader := jar.MetaDirFileHeader()
			dirSource := bufferEntry{dirHeader, nil}
			addMapping(jar.MetaDir, dirSource)
		}

		contents, err := ioutil.ReadFile(manifest)
		if err != nil {
			return err
		}

		fh, buf, err := jar.ManifestFileContents(contents)
		if err != nil {
			return err
		}

		fileSource := bufferEntry{fh, buf}
		addMapping(jar.ManifestFile, fileSource)
	}

	if pyMain != "" {
		buf, err := ioutil.ReadFile(pyMain)
		if err != nil {
			return err
		}
		fh := &zip.FileHeader{
			Name:               "__main__.py",
			Method:             zip.Store,
			UncompressedSize64: uint64(len(buf)),
		}
		fh.SetMode(0700)
		fh.SetModTime(jar.DefaultTime)
		fileSource := bufferEntry{fh, buf}
		addMapping("__main__.py", fileSource)
	}

	if emulatePar {
		// the runfiles packages needs to be populated with "__init__.py".
		newPyPkgs := []string{}
		// the runfiles dirs have been treated as packages.
		existingPyPkgSet := make(map[string]bool)
		// put existing __init__.py files to a set first. This set is used for preventing
		// generated __init__.py files from overwriting existing ones.
		for _, namedReader := range readers {
			for _, file := range namedReader.reader.File {
				if filepath.Base(file.Name) != "__init__.py" {
					continue
				}
				pyPkg := pathBeforeLastSlash(file.Name)
				if _, found := existingPyPkgSet[pyPkg]; found {
					panic(fmt.Errorf("found __init__.py path duplicates during pars merging: %q.", file.Name))
				} else {
					existingPyPkgSet[pyPkg] = true
				}
			}
		}
		for _, namedReader := range readers {
			for _, file := range namedReader.reader.File {
				var parentPath string /* the path after trimming last "/" */
				if filepath.Base(file.Name) == "__init__.py" {
					// for existing __init__.py files, we should trim last "/" for twice.
					// eg. a/b/c/__init__.py ---> a/b
					parentPath = pathBeforeLastSlash(pathBeforeLastSlash(file.Name))
				} else {
					parentPath = pathBeforeLastSlash(file.Name)
				}
				populateNewPyPkgs(parentPath, existingPyPkgSet, &newPyPkgs)
			}
		}
		for _, pkg := range newPyPkgs {
			var emptyBuf []byte
			fh := &zip.FileHeader{
				Name:               filepath.Join(pkg, "__init__.py"),
				Method:             zip.Store,
				UncompressedSize64: uint64(len(emptyBuf)),
			}
			fh.SetMode(0700)
			fh.SetModTime(jar.DefaultTime)
			fileSource := bufferEntry{fh, emptyBuf}
			addMapping(filepath.Join(pkg, "__init__.py"), fileSource)
		}
	}
	for _, namedReader := range readers {
		_, skipStripThisZip := zipsToNotStrip[namedReader.path]
		for _, file := range namedReader.reader.File {
			if !skipStripThisZip {
				if skip, err := shouldStripEntry(emulateJar, stripFiles, stripDirs, file.Name); err != nil {
					return err
				} else if skip {
					continue
				}
			}

			if stripDirEntries && file.FileInfo().IsDir() {
				continue
			}

			// check for other files or directories destined for the same path
			dest := file.Name

			// make a new entry to add
			source := zipEntry{path: zipEntryPath{zipName: namedReader.path, entryName: file.Name}, content: file}

			if existingSource := addMapping(dest, source); existingSource != nil {
				// handle duplicates
				if existingSource.IsDir() != source.IsDir() {
					return fmt.Errorf("Directory/file mismatch at %v from %v and %v\n",
						dest, existingSource, source)
				}

				if ignoreDuplicates {
					continue
				}

				if emulateJar &&
					file.Name == jar.ManifestFile || file.Name == jar.ModuleInfoClass {
					// Skip manifest and module info files that are not from the first input file
					continue
				}

				if source.IsDir() {
					continue
				}

				if existingSource.CRC32() == source.CRC32() && existingSource.Size() == source.Size() {
					continue
				}

				return fmt.Errorf("Duplicate path %v found in %v and %v\n",
					dest, existingSource, source)
			}
		}
	}

	if emulateJar {
		jarSort(orderedMappings)
	} else if sortEntries {
		alphanumericSort(orderedMappings)
	}

	for _, entry := range orderedMappings {
		if err := entry.source.WriteToZip(entry.dest, writer); err != nil {
			return err
		}
	}

	return nil
}

// Sets the given directory and all its ancestor directories as Python packages.
func populateNewPyPkgs(pkgPath string, existingPyPkgSet map[string]bool, newPyPkgs *[]string) {
	for pkgPath != "" {
		if _, found := existingPyPkgSet[pkgPath]; !found {
			existingPyPkgSet[pkgPath] = true
			*newPyPkgs = append(*newPyPkgs, pkgPath)
			// Gets its ancestor directory by trimming last slash.
			pkgPath = pathBeforeLastSlash(pkgPath)
		} else {
			break
		}
	}
}

func pathBeforeLastSlash(path string) string {
	ret := filepath.Dir(path)
	// filepath.Dir("abc") -> "." and filepath.Dir("/abc") -> "/".
	if ret == "." || ret == "/" {
		return ""
	}
	return ret
}

func shouldStripEntry(emulateJar bool, stripFiles, stripDirs []string, name string) (bool, error) {
	for _, dir := range stripDirs {
		dir = filepath.Clean(dir)
		patterns := []string{
			dir + "/",      // the directory itself
			dir + "/**/*",  // files recursively in the directory
			dir + "/**/*/", // directories recursively in the directory
		}

		for _, pattern := range patterns {
			match, err := pathtools.Match(pattern, name)
			if err != nil {
				return false, fmt.Errorf("%s: %s", err.Error(), pattern)
			} else if match {
				if emulateJar {
					// When merging jar files, don't strip META-INF/MANIFEST.MF even if stripping META-INF is
					// requested.
					// TODO(ccross): which files does this affect?
					if name != jar.MetaDir && name != jar.ManifestFile {
						return true, nil
					}
				}
				return true, nil
			}
		}
	}

	for _, pattern := range stripFiles {
		if match, err := pathtools.Match(pattern, name); err != nil {
			return false, fmt.Errorf("%s: %s", err.Error(), pattern)
		} else if match {
			return true, nil
		}
	}
	return false, nil
}

func jarSort(files []fileMapping) {
	sort.SliceStable(files, func(i, j int) bool {
		return jar.EntryNamesLess(files[i].dest, files[j].dest)
	})
}

func alphanumericSort(files []fileMapping) {
	sort.SliceStable(files, func(i, j int) bool {
		return files[i].dest < files[j].dest
	})
}
