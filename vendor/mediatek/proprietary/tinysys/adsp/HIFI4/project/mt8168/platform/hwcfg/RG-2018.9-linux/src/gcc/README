README for GCC Source Files
------------------------------------------------------------------------

If for some reason you prefer to use GCC instead of the compiler provided
by Tensilica, you can obtain the standard GCC source files and build an
Xtensa cross compiler.

The Xtensa processor support is included in the standard GCC sources.
After obtaining the GCC source files, you only need to add your Xtensa
configuration information by copying the "xtensa-config.h" file in this
directory into the GCC source tree.  For GCC 3.4 and later versions, copy
this file into the "include" directory; for GCC 3.3 and earlier versions,
copy it into the "gcc/config/xtensa" directory.  You can then follow the
standard directions for building and installing a GCC cross compiler.

Please refer to the installation instructions included with GCC for
detailed directions.  (The instructions for the latest version of GCC
are also available at http://gcc.gnu.org/install/.)  The following
steps illustrate one way to build GCC, assuming you are using the
binutils from Tensilica's Xtensa Tools.

The following directories are referenced in the build steps below:

     $gccsrc	         top-level directory from the GCC source tar file
     $prefix	         new directory where you will install GCC
     $xtensa_tools_root  root directory of Tensilica's Xtensa Tools
     $xtensa_root        root directory for your Xtensa configuration
     $build	         new directory used temporarily for building GCC

We use shell variable syntax to refer to these directories.  You can
define the corresponding shell variables or you can just substitute
the appropriate directory names where they are referenced.

1. Obtain the GCC source tar file.  Extract the files to create the
$gccsrc directory.  Add your Xtensa configuration information by
copying a header file:

     cp $xtensa_root/src/gcc/xtensa-config.h $gccsrc/include

     or for GCC 3.3 and earlier versions:
     cp $xtensa_root/src/gcc/xtensa-config.h $gccsrc/gcc/config/xtensa

2. Create some of the directories where you want to install GCC.

     mkdir -p $prefix/bin $prefix/xtensa-elf
     (cd $prefix/xtensa-elf; mkdir arch bin include lib)

3. Add symbolic links to Xtensa Tools and Xtensa configuration files.
The following commands use Bourne shell (/bin/sh) syntax so you need
to either use a compatible shell or translate the commands to match
your shell.

     cd $prefix/bin
     for f in $xtensa_tools_root/bin/xt-*; do
       newf=`basename $f | sed -e s/xt-/xtensa-elf-/`
       rm -f $newf `basename $f`
       ln -s $f $newf
       ln -s $newf `basename $f`
     done

     cd $prefix/xtensa-elf/bin
     ln -s ../../bin/xtensa-elf-as as

     cd $prefix/xtensa-elf
     for d in arch include lib; do
       (cd $d; $gccsrc/symlink-tree $xtensa_root/xtensa-elf/$d)
       if [ -r $xtensa_tools_root/xtensa-elf/$d ]; then
         (cd $d; $gccsrc/symlink-tree $xtensa_tools_root/xtensa-elf/$d)
       fi
     done

4. Set up your environment.  Besides the PATH setting shown, be sure
to set XTENSA_CORE (and possibly XTENSA_SYSTEM) as needed to specify
the Xtensa configuration for which you are building GCC.

     PATH=$prefix/bin:$PATH

5. Create the $build directory and run the configure script.  The
$build directory should not be located inside either $prefix or
$gccsrc.

     mkdir $build
     cd $build
     $gccsrc/configure --target=xtensa-elf --prefix=$prefix \
       --enable-languages="c,c++" --disable-shared \
       --with-as=$prefix/bin/xtensa-elf-as \
       --with-ld=$prefix/bin/xtensa-elf-ld \
       --with-gxx-include-dir=$prefix/xtensa-elf/include/c++ \
       --with-newlib

6. Build and install GCC.

     cd $build
     make all install

