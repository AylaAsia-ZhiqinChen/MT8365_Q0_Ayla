This file describes how to build and run the XTSC examples.

The top-level XTSC example directory (the directory this README.txt file is in)
is refered to as <xtsc_examples_root> both in this README.text and in the "Xtensa
SystemC (XTSC) User's Guide" (the XTSC UG is found in file xtsc_ug.pdf).
The XTSC UG also uses the symbol <xtensa_tools_root> to refer to the
location where your Xtensa tools are installed (i.e. your XtensaTools
directory).  In these instructions, this directory will be referenced using
the environment variable XTENSA_SW_TOOLS.

Some of these examples may be incompatible with your Xtensa processor
configuration.  The source code for any incompatible examples is still
provided but the Makefile in the example directory will be disabled and, in
addition, the top-level Makefile (and Visual C++ solution file on MS Windows)
will not visit that directory.

Please refer to the XTSC UG (xtsc_ug.pdf) for more information about these
examples.




The following table shows file extensions and their purpose in the XTSC
examples:

Extension      Purpose
------------   ------------------------------------------------------------------
.c             Xtensa C source file.
.cpp           Host simulator C++ source file.
.h             C/C++ header file.
.inc           --include file for use with xtsc-run.
.log           log4xtensa logging output file (typically, xtsc.log).
.rom           File defining address/value pairs for use with an xtsc_lookup.
.tab           File containing routing information for xtsc_router.
.txt           log4xtensa configuration file (typically, TextLogger.txt).  Also
               an address translations file for xtsc_arbiter.
.vec           Script file containing test vectors for use by a testbench module.
.cmd           File meant to be sourced from the XTSC command prompt
.lua           Lua script file
.vc100.sln     Microsoft Visual C++ 2010 solution file.
.vc100.vcxproj Microsoft Visual C++ 2010 project file.
.vc110.sln     Microsoft Visual C++ 2012 solution file.
.vc110.vcxproj Microsoft Visual C++ 2012 project file.
.vc120.sln     Microsoft Visual C++ 2013 solution file.
.vc120.vcxproj Microsoft Visual C++ 2013 project file.
.vc140.sln     Microsoft Visual C++ 2015 solution file.
.vc140.vcxproj Microsoft Visual C++ 2015 project file.
.vc141.sln     Microsoft Visual C++ 2017 solution file.
.vc141.vcxproj Microsoft Visual C++ 2017 project file.


Note:  In the instructions below a dollar sign ($) is used as the generic
shell prompt for both Linux (bash, csh, etc) and MS Windows (cmd shell).



Running the XTSC Examples using xtsc-run

Most of the XTSC examples can be run by passing the xtsc-run program a
script file (*.inc) instead of building the simulator by compiling
sc_main.cpp (which requires a host compiler).  This section explains how to
run the XTSC examples using only xt-xcc to compile the target program and
xtsc-run to assemble the system and run the simulation (using the *.inc
script file specific to each example).  Subsequent sections explain how to
compile the simulator from sc_main.cpp using a host compiler.

To run the XTSC examples using xtsc-run, you must have both xt-xcc and
xtsc-run on your path as shown below:

Linux:
    $ export PATH=$XTENSA_SW_TOOLS/bin:$PATH        # bash like shell
    $ setenv PATH $XTENSA_SW_TOOLS/bin:$PATH        # csh  like shell

MS Windows cmd prompt:
    $ set PATH=%XTENSA_SW_TOOLS%\lib\iss;%XTENSA_SW_TOOLS%\bin;%PATH%

The Makefile system in the XTSC examples includes a Makefile target called
xtsc-run which can be used to compile the target program and then run the
simulation using xtsc-run.  If this is done in the top-level XTSC examples
directory then each example sub-directory which supports xtsc-run is visited
in turn.  For example:

Linux:
    $ cd <xtsc_examples_root>
    $ make xtsc-run

MS Windows:
    $ cd <xtsc_examples_root>
    $ xt-make xtsc-run

If you only want to run one example, change into that example's sub-directory
and run make there.  For example:

Linux:
    $ cd <xtsc_examples_root>
    $ cd xtsc_memory
    $ make xtsc-run

MS Windows:
    $ cd <xtsc_examples_root>
    $ cd xtsc_memory
    $ xt-make xtsc-run

Note: The default target in the XTSC examples Makefile system is in fact
xtsc-run, so simple "make" is equivalent to "make xtsc-run" on Linux and
"xt-make" is equivalent to "xt-make xtsc-run" on MS Windows.

Note: Except for the plugin examples, the example sub-directories which
require a host compiler do not have an xtsc-run include script (*.inc).  If
you go to one of this sub-directories and try to do a make xtsc-run, you
will get an error.  For example,
    $ cd <xtsc_examples_root>
    $ cd template_memory
    $ make xtsc-run
    xtsc-run failed, std::exception caught:
    Cannot open include file 'template_memory.inc' found in command ...




Host Requirements for Building the Simulator from sc_main.cpp:

Note: This README.txt covers common build options when using SystemC 2.3.0.
      Please see file Makefile.include for more advanced options such as
      building a 64 bit simulator, using SystemC 2.2.0, 2.3.1, or 2.3.2,
      and/or using shared libraries.

Warning: Shared and static versions of SystemC and the XTSC libraries
         should not be mixed in the same simulation (including plugins).
         If any part of the simulation is using a shared library 
         version of either SystemC or one of the XTSC libraries, then all
         SystemC and XTSC libraries in the simulation should be shared
         libraries built using the same version of SystemC and XTSC.
         Failure to heed this warning can result in link time errors
         and bizarre run time failures.

*) Linux: The default ISS libraries are built with gcc 4.4.x, and you
will need gcc 4.4.x to compile the examples so they are compatible with
the default ISS libraries.  Alternative sets of libraries are built with
gcc 4.1.x and gcc 5.2.x.

*) Windows: Microsoft Visual C++ 2010, 2012, 2013, 2015, or 2017 is required to
build these examples (or any other XTSC program).

*) Windows: Using a Debug build of a simulator built with Microsoft Visual C++
2015 or 2017 requires that Microsoft Visual C++ 2013 also be installed in order
to provide MSVCR120D.DLL which some components of the ISS depend upon (see
..\..\config\libcas-ref-cored-x86_64.dll).




Building the Simulator Using MS Visual C++ Solution/Project Files (Windows only):

On MS Windows, you can build and run the examples using the provided MS Visual C++
project and solution files.
For MS Visual C++ 2010 there is an xtsc.vc100.sln file in this directory that has
the projects for all the examples.  
For MS Visual C++ 2012, the name of the solution file is xtsc.vc110.sln.  
For MS Visual C++ 2013, the name of the solution file is xtsc.vc120.sln.  
For MS Visual C++ 2015, the name of the solution file is xtsc.vc140.sln.  
For MS Visual C++ 2017, the name of the solution file is xtsc.vc141.sln.  
If you only want to deal with a single example, then use the solution file in that
example's subdirectory.  For example:
  For MS Visual C++ 2010:
    hello_world\hello_world.vc100.sln
  For MS Visual C++ 2012:
    hello_world\hello_world.vc110.sln
  For MS Visual C++ 2013:
    hello_world\hello_world.vc120.sln
  For MS Visual C++ 2015:
    hello_world\hello_world.vc140.sln
  For MS Visual C++ 2017:
    hello_world\hello_world.vc141.sln

To setup your environment for building, running, and debug of XTSC examples,
you need to add the following two locations to the PATH environment variable:
1) So xt-xcc, xt-gdb, etc can be found:
       %XTENSA_SW_TOOLS%\bin
2) So the ISS DLL and xtsc-run (for building plugins) can be found (add one,
   and only one, of the following):
     For MS Visual C++ 2010:
       %XTENSA_SW_TOOLS%\lib\iss
     For MS Visual C++ 2012:
       %XTENSA_SW_TOOLS%\lib\iss-vc110
     For MS Visual C++ 2013 (64-bit, SystemC 2.3.1):
       %XTENSA_SW_TOOLS%\lib64\iss-vc120\systemc-2.3.1;%XTENSA_SW_TOOLS%\lib64\iss-vc120
     For MS Visual C++ 2015 (64-bit, SystemC 2.3.1):
       %XTENSA_SW_TOOLS%\lib64\iss-vc140\systemc-2.3.1;%XTENSA_SW_TOOLS%\lib64\iss-vc140
     For MS Visual C++ 2015 (64-bit, SystemC 2.3.2):
       %XTENSA_SW_TOOLS%\lib64\iss-vc140\systemc-2.3.2;%XTENSA_SW_TOOLS%\lib64\iss-vc140
     For MS Visual C++ 2017 (64-bit, SystemC 2.3.2):
       %XTENSA_SW_TOOLS%\lib64\iss-vc141\systemc-2.3.2;%XTENSA_SW_TOOLS%\lib64\iss-vc141

An easy way to get a properly setup cmd shell is to right-click on the desired
config's params entry in the System Overview view of Xtensa Xplorer and then
select one of the "Open command shell for config" entries (there will be one
entry for each supported version of MS Visual C++ that Xplorer detected on
your system).  For example:
  System Overview>Configuratons>D_106micro-params>Open command shell for config (VS2010)

An alternative is use the Control Panel to change your baseline PATH
environment variable:
   Start>Control Panel>System>Advanced system settings>Advanced>Environment Variables

Note: The MSVC 2010, 2012, and 2013 *.sln and *.vcxproj files provided by
      Cadence support static libraries with SystemC 2.3.0.  The project files
      would have to be modified by you in order to support SystemC 2.2.0 or
      2.3.1 or to support shared XTSC and SystemC libraries.  
      Configuration Properties that may need to be adjusted include:
        C/C++>Preprocessor>Preprocessor Definitions
        C/C++>General>Additional Include Directories
        Linker>General>Additional Library Directories
        Linker>Input>Additional Dependencies
      For shared libraries with SystemC 2.2.0, your PATH will also need
      the following path added in front of the PATH mentioned above:
        For MS Visual C++ 2010:
          %XTENSA_SW_TOOLS%\lib\iss\systemc-2.2.0
        For MS Visual C++ 2012:
          %XTENSA_SW_TOOLS%\lib\iss-vc110\systemc-2.2.0
      The MSVC 2015 and 2017 *.sln and *.vcxproj files support both static
      libraries (Debug|Release) and shared libraries (DebugDLL|ReleaseDLL).

Note: To run an XTSC example, put one (and only one) of the following on
      your PATH depending on which version of MS Visual C++ the example was
      built with:
        For MS Visual C++ 2010:
          %XTENSA_SW_TOOLS%\lib\iss
        For MS Visual C++ 2012:
          %XTENSA_SW_TOOLS%\lib\iss-vc110
        For MS Visual C++ 2013 (64-bit, SystemC 2.3.1):
          %XTENSA_SW_TOOLS%\lib64\iss-vc120\systemc-2.3.1;%XTENSA_SW_TOOLS%\lib64\iss-vc120
        For MS Visual C++ 2015 (64-bit, SystemC 2.3.1):
          %XTENSA_SW_TOOLS%\lib64\iss-vc140\systemc-2.3.1;%XTENSA_SW_TOOLS%\lib64\iss-vc140
        For MS Visual C++ 2015 (64-bit, SystemC 2.3.2):
          %XTENSA_SW_TOOLS%\lib64\iss-vc140\systemc-2.3.2;%XTENSA_SW_TOOLS%\lib64\iss-vc140
        For MS Visual C++ 2017 (64-bit, SystemC 2.3.2):
          %XTENSA_SW_TOOLS%\lib64\iss-vc141\systemc-2.3.2;%XTENSA_SW_TOOLS%\lib64\iss-vc141
      Failure to do this step or accidentally putting the wrong path will
      typically result in the executable failing to start or crashing
      shortly after starting.




Building the Simulator Using Makefiles on Linux:

From the command line, first set up your environment to work with both
the host compiler (gcc) and Xtensa Tools.  To do this:
1.  Ensure your PATH points to a supported version of gcc (4.4.x, 4.1.x, or 5.2.x).
2.  Set up the PATH environment variable to point to:
        $XTENSA_SW_TOOLS/bin
3.  Although not specifically required when running these examples,
    Cadence recommends that you set the XTENSA_CORE (and possibly
    XTENSA_SYSTEM) environment variable to select this Xtensa
    configuration.  This will simplify using many of Cadence's
    tools such as xt-gdb and xtsc-run.

Then, invoke make to build and run the examples:
    $ cd <xtsc_examples_root>
    $ make clean all run

To just build one example, change to that example's subdirectory and
run make there.  For example:
    $ cd <xtsc_examples_root>
    $ cd xtsc_memory
    $ make clean all run




Building Using Makefiles on MS Windows:

From the command line, first set up your environment to work with both
the host compiler (cl) and Xtensa Tools.  To do this:
1.  Set up the LIB, INCLUDE, and PATH environment variables to use the
    desired version (2010, 2012, 2013, 2015, or 2017) of Microsoft Visual
    C++ (e.g., by running Microsoft's vsvars32.bat).
2.  Put the following XtensaTools sub-directory on your PATH:
      %XTENSA_SW_TOOLS%\bin
3.  In addition, put one (and only one) of the following on your PATH
    depending on which version of MS Visual C++ you are using:
    For MS Visual C++ 2010:
      %XTENSA_SW_TOOLS%\lib\iss
    For MS Visual C++ 2012:
      %XTENSA_SW_TOOLS%\lib\iss-vc110
    For MS Visual C++ 2013 (64-bit, SystemC 2.3.1):
      %XTENSA_SW_TOOLS%\lib64\iss-vc120\systemc-2.3.1;%XTENSA_SW_TOOLS%\lib64\iss-vc120
    For MS Visual C++ 2015 (64-bit, SystemC 2.3.2):
      %XTENSA_SW_TOOLS%\lib64\iss-vc140\systemc-2.3.2;%XTENSA_SW_TOOLS%\lib64\iss-vc140
    For MS Visual C++ 2017 (64-bit, SystemC 2.3.2):
      %XTENSA_SW_TOOLS%\lib64\iss-vc141\systemc-2.3.2;%XTENSA_SW_TOOLS%\lib64\iss-vc141
    Note: Failure to do this step or accidentally putting the wrong path
          will typically result in the executable failing to start or
          crashing shortly after starting.
4.  Although not specifically required when running these examples,
    Cadence recommends that you set the XTENSA_CORE (and possibly
    XTENSA_SYSTEM) environment variable to select this Xtensa
    configuration.  This will simplify using many of Cadence's
    tools such as xt-gdb and xtsc-run.

Then, invoke xt-make to build and run all the examples:
    $ cd <xtsc_examples_root>
    $ xt-make clean all run

This will build both Debug and Release versions for each
example and will run the Release version.  The Makefile's also have
".Debug" and ".Release" variants of the "all" and "run" targets, that
is "all.Debug", "all.Release", "run.Debug", and "run.Release".

To just build one example, change to that example's subdirectory and
run xt-make there.  For example:
    $ cd <xtsc_examples_root>
    $ cd xtsc_memory
    $ xt-make clean all run




Building target executables with custom flags (Linux or MS Windows):

You can use the CUSTOM_TARGET_CFLAGS macro to pass additional flags to 
xt-xcc for all target executables.  This macro can be added to the 
Makefile in the example directory. For example:
    CUSTOM_TARGET_CFLAGS=-O3

For multi-processor examples, the custom flags can be applied on a per
target executable basis by adding lines for target custom_target similar
to the following to the Makefile in the example directory (you may need
to remove "target/consumer.out" from the TARGET_OBJECTS macro definition
to prevent it from being made two times):
    custom_target:
		$(MAKE) target/consumer.out CUSTOM_TARGET_CFLAGS=-O3

You can use quotation marks to pass multiple flags.  For example:
    custom_target:
		$(MAKE) target/consumer.out CUSTOM_TARGET_CFLAGS="-O3 -keep"




The hello_world.standalone example:

The hello_world.standalone example is identical to the hello_world
example except that its Makefile does not include higher-level make
files and it has its own TextLogger.txt file.  This makes it suitable
for copying to some other location on your file system (for example,
as a starting point for writing your own XTSC application).  Because
it is meant to stand alone, this example is not visited by the top-
level Makefile or by the top-level Microsoft Visual C++ solution
file.  If you needed to make any modifications to the top-level
Makefile's and you want to use this stand alone example then you will
need to make those same modifications to the Makefile in the 
hello_world.standalone directory.  In addition, on MS Windows if you
are using TDK's then you will need to edit the TDK_DIR preprocessor
macro definition in the vcxproj file (see the discussion below about
editing vcxproj files).




The multicore.standalone example:

The multicore.standalone example is similar to the hello_world.standalone
example except it includes support for using two different core
configurations and/or two different TDK's.  See the instructions near
the beginning of the Makefile in the multicore.standalone example sub-
directory.  In addition, on MS Windows you may need to edit the following
preprocessor macro definitions in the vcxproj file (see the discussion
below about editing vcxproj files):
  CONFIG0_NAME          CONFIG1_NAME
  XTENSA0_REGISTRY      XTENSA1_REGISTRY
  TDK0_DIR              TDK1_DIR
  TARGET0_PROGRAM       TARGET1_PROGRAM




Creating a stand alone copy of one of the examples:

Most of the examples depend upon higher-level make files (specifically, upon
Makefile.include and Makefile.common) and the examples with TIE also depend
on the top-level TIE directory.  The following steps can be used to copy one
of these examples to a new location and make it stand alone so that it does
not depend on higher level make files and so that TIE, if any, is self-
contained in a sub-directory.

1.  Copy the example sub-directory of interest (e.g. hello_world, or
    simple_memory, or etc.) and its sub-directories to a new location.
2.  Edit the example Makefile in the new location so that it does not
    use the "include" keyword to include other make files but instead
    has their actual content in line:
    a. Replace the following line with the actual content of the
       Makefile.include file:
         include $(EXAMPLE_DIR)/../Makefile.include
    b. Replace the following line with the actual content of the
       Makefile.common file:
          include $(EXAMPLE_DIR)/../Makefile.common
3.  TextLogger.txt:
    a. Copy <xtsc_examples_root>/TextLogger.txt to the new directory.
    b. Change "../TextLogger.txt" to "TextLogger.txt" in:
       i)   The xtsc_initialize_parms line in the new sc_main.cpp file.
       ii)  In the <>.inc file used with xtsc-run (if the example has one).
       iii) In the <>.vcxproj file (MS Windows only).

Note:  A Microsoft Visual C++ project file (<>.vcxproj) can be edited
by hand (e.g. in notepad) if you are careful.  Three things to keep in 
mind are: (1) have a backup copy of the vcxproj file, (2) do NOT have the
project file open in Visual C++ when you are editing the vcxproj file,
and (3) be careful to preserve the XML syntax and structure of the vcxproj
file.

If the example uses TIE, perform the following additional steps:

4.  Create a sub-directory in the new location called TIE.
5.  Copy <xtsc_examples_root>/TIE/example.tie to the new TIE sub-directory.
6.  Replace the tie rule in the new example Makefile with the tie and tdk
    rules from <xtsc_examples_root>/Makefile  You will be changing 2 lines
    that look like this:
        tie:
                $(MAKE) -C .. tie
    To 3 lines that look like this:
        tie: TIE/example.tdk
        TIE/example.tdk: TIE/example.tie
                $(XTTOOLS)/bin/tc $(TCFLAGS) -c -noopt -d $@ $<
    Important:  Be sure the first character on the last line above is a tab 
    (not a space).
7.  Change "../TIE/example.tdk" to "TIE/example.tdk" in:
    a.  The TDK_DIR macro definition line in the new Makefile.
    b.  The -xtensa_params command in the xtsc-run <>.inc file (if the
        example has one).
    c.  The TDK_DIR macro definition in the <>.vcxproj file (MS Windows Only).
        The macro is defined in two locations in the <>.vcxproj file (one
        for Debug and one for Release).

