
                                      Overview                                  

The Xtensa SystemC for Cadence Virtual System Platform package (XTSC_VSP) makes the
Xtensa SystemC package (XTSC) available in the Cadence Virtual System Platform (VSP)
environment.  Primarily this means that the Xtensa ISS is exposed in SimVision and
ncsim as a "tlmcpu" so that its registers and memory contents are visible and its
program can be debugged at the source code level.

The XTSC_VSP package is comprised of the following elements:

1) The xt-genvspmodel program which is used to create a wrapper around the xtsc_core
   model containing the Xtensa ISS that exposes it as a "tlmcpu" to SimVision/ncsim.
   This wrapper, called xtsc_core_vsp, can exposed any desired subset of the interfaces
   that a particular Xtensa configuration and optional TDK has so that they can be
   connected to other models in the simulation.
2) The XTSC core and component library models (libxtsc and libxtsc_core) built against
   the supported Incisive/Xcelium releases.  This means all the XTSC component models
   such as xtsc_memory, xtsc_router, xtsc_arbiter, xtsc_queue, xtsc_lookup, and so on are
   available in VSP.
3) An additional small library (libxtsc_vsp), that exposes the XTSC global parameters
   as VSP parameters (that is, as cci_params).
4) A set of XTSC_VSP examples that mirror the basic XTSC examples and that show each
   XTSC component model being used in a VSP simulation.



                                Support Matrix

Cadence Release   Compiler   Arch    XTSC_VSP Library Location
---------------   --------   -----   -------------------------------------------------
Incisive 14.2     gcc-4.1    32Bit   $XTENSA_SW_TOOLS/lib/iss-GCC-4.1/cadence-IUS142
Incisive 14.2     gcc-4.1    64Bit   $XTENSA_SW_TOOLS/lib64/iss-GCC-4.1/cadence-IUS142
Incisive 14.2     gcc-4.4    32Bit   $XTENSA_SW_TOOLS/lib/iss/cadence-IUS142
Incisive 14.2     gcc-4.4    64Bit   $XTENSA_SW_TOOLS/lib64/iss/cadence-IUS142
Incisive 15.2     gcc-4.8    64Bit   $XTENSA_SW_TOOLS/lib64/iss-GCC-4.8/cadence-IUS152
Xcelium 17.04     gcc-4.8    64Bit   $XTENSA_SW_TOOLS/lib64/iss-GCC-4.8/cadence-IUS1704
Xcelium 18.03     gcc-4.8    64Bit   $XTENSA_SW_TOOLS/lib64/iss-GCC-4.8/cadence-IUS1803
 

Note: XTSC_VSP is only supported on Redhat Enterprise Linux versions that the underlying
      Incisive/Xcelium version supports.



                                 Resource Locations

The location in which you (or Xtensa Xplorer) have installed Tensilica's Xtensa software
tools is called <xtensa_sw_tools> (this corresponds to the XTENSA_SW_TOOLS environment
variable).  An example location might be:
  /opt/local/xplorer/RG-2018.9/XtDevTools/install/tools/RG-2018.9-linux/XtensaTools

The location in which you (or Xtensa Xplorer) have installed an Xtensa configuration
is called <xtensa_root>.  An example location for the sample_config config might be:
  /opt/local/xplorer/RG-2018.9/XtDevTools/install/builds/RG-2018.9-linux/sample_config

The XTSC_VSP libraries are located in a sub-directory of <xtensa_sw_tools> which depends
on the GCC version, the simulator architecture (32 bit or 64 bit), and the Incisive/Xcelium
(IUS) version.  The sub-directory is shown in the last column of the Support Matrix above.

The xt-genvspmodel program is located at:
  <xtensa_sw_tools>/bin/xt-genvspmodel

The xtsc_vsp.h header file is located at:
  <xtensa_sw_tools>/include/xtsc_vsp/xtsc_vsp.h

The XTSC headers are located in:
  <xtensa_sw_tools>/include/xtsc

The XTSC_VSP examples are located in:
  <xtensa_root>/examples/XTSC/xtsc_vsp



                               Supporting Documentation

The xt-genvspmodel program supports the --help option to print extensive on-line help.

The installed XTSC_VSP examples mirror the basic (OSCI) XTSC examples located one
directory up and documented in the "Xtensa SystemC (XTSC) Users Guide" (xtsc_ug.pdf) in
a chapter containing the model name corresponding to the example name.  For example, the
xtsc_memory XTSC_VSP example is located in:
  <xtensa_root>/examples/XTSC/xtsc_vsp/xtsc_memory
... and is based on the basic XTSC example in:
  <xtensa_root>/examples/XTSC/xtsc_memory
... and is documented in the XTSC UG in the chapter called:
  "The xtsc_memory Class"

The capabilities of XTSC parameters are documented in the XTSC UG in Section 6.2.6
"The xtsc_parms Class".

The individual parameters used by the XTSC_VSP examples are documented in the "Xtensa
SystemC (XTSC) Reference Manual" which is available in PDF (xtsc_rm.pdf), HTML tar ball
(xtsc_rm.zip), and in the original doxygen comments in the XTSC header files in:
  <xtensa_sw_tools>/include/xtsc

Note: All Xtensa documentation is available in Xtensa Xplorer by following the drop-down
menu sequence:
        Help>PDF Documentation
Typically, this documentation can be found in your file system in a directory related to
your Xtensa Tools installation.  For example:
    $ cd /opt/local/xplorer/RG-2018.9/XtDevTools/downloads/RG-2018.9/docs




                              Checking Your Environment

Use the following steps to generate, build, and run a test XTSC_VSP simulation to check
your environment:
1. Ensure you are on a 64-bit RHEL machine supported by your Incisive/Xcelium version:
   $ cat /etc/redhat-release
   $ uname -m                                           # Should be x86_64 (not i686)
2. Ensure your environment is setup for Cadence Incisive/Xcelium tools by checking that
   ncsim is on your path and is a version listed in the above Support Matrix:
   $ ncsim -version
3. Ensure a version of GCC compatible with XTSC_VSP (see Support Matrix) is on your path:
   $ g++ -dumpversion
4. If you are building a 64-bit executable, set the XTSC_64BIT environment variable to 1
   (if you are building a 32-bit executable, do NOT define XTSC_64BIT):
   $ export XTSC_64BIT=1                                # bash
   $ setenv XTSC_64BIT 1                                # csh
5. Check that GCC and ncsim are both 32-bit or 64-bit and are consistent with the
   existance and value of the XTSC_64BIT environment variable:
   $ ncbits             # 32 / 64
   $ g++ -v             # Target should be i686 (32 bit) or x86_64 (64 bit)
6. Ensure the XTENSA_SW_TOOLS environment variable is set to point to your installation
   of Tensilica’s Tools and that $(XTENSA_SW_TOOLS)/bin is on your path:
   $ echo $XTENSA_SW_TOOLS
   $ which xt-genvspmodel
7. Ensure the XTENSA_SYSTEM and XTENSA_CORE environment variables are defined properly:
   $ ls $XTENSA_SYSTEM/$XTENSA_CORE-params
8. Add the current working directory (.) to your LD_LIBRARY_PATH environment variable:
   $ export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH          # bash
   $ setenv LD_LIBRARY_PATH .:$LD_LIBRARY_PATH          # csh
9. Enable log4xtensa by default.  This can be a tremendous aid to debugging to both you
   and the Tensilica support team when things do not go as planned:
   $ export XTSC_TEXTLOGGER_CONFIG_FILE=TextLogger.txt          # bash
   $ setenv XTSC_TEXTLOGGER_CONFIG_FILE TextLogger.txt          # csh
   Note: The logging output will be sent to file xtsc.log.
10.Create a working directory somewhere that you have write privileges to:
   $ mkdir work
   $ cd work
11.Run xt-genvspmodel to generate an XTSC_VSP wrapper for your configuration and put it
   in a directory called xtsc_core_vsp:
   $ xt-genvspmodel --version=cadence-IUS152 --name=xtsc_core_vsp
12.Change into the newly created directory
   $ cd xtsc_core_vsp
13.Run make to build your XTSC_VSP wrapper, a simple "Hello, World" target program, a
   Cadence VSP system containing the newly built model and any required xtsc_memory
   models, and to run the simulation using ncsim.
   $ make clean all run

If everything is set up correctly, then the last step should result in console output
that is first the make clean, then the compiling and linking from make all, then the
actual run of the simulation which should stop at the ncsim> prompt.  If you type 'c'
here to continue the simulation you should get the "Hello, World" message.  After 
this type 'q' to quit ncsim.

Note: If you encounter a problem with the last step you can do "make info" to get
environment information to help you debug the problem yourself or to send to Cadence
Tensilica Support to get help from us.




                            Running the XTSC_VSP Examples


To build and run an XTSC_VSP example, set up your environment as explained above and
then change into one of the XTSC_VSP example directories.  For example:
   $ cd <xtensa_root>/examples/XTSC/xtsc_vsp/xtsc_memory

To build the example:
   $ make clean all

To run the example without debug:
   $ make run

To run the example with debug at the ncsim prompt:
   $ make debug

To run the example in SimVision:
   $ make eswdebug

To help trouble-shoot build issues:
   $ make info




                                   The Example TDK

Some of the examples (e.g tie_pin, xtsc_lookup_pin, and xtsc_queue_pin) rely upon the
example TDK.  In these cases, the example TDK will be built automatically when you run
make in that example's directory (if it has not already been built).  The example TDK is
located in:
   <xtensa_root>/examples/XTSC/TIE

Note: If you first built the example TDK while set up for 32-bit builds and then later
switch to 64-bit builds, you will need to rebuild the example TDK so that it includes
64-bit shared objects.  To rebuild the example TDK for 64-bit simulations, ensure your
environment is set up for 64 bits as explained above and then run make on the tie.clean
and tie targets from the base XTSC examples directory.  For example:
   $ cd <xtensa_root>/examples/XTSC
   $ make tie.clean tie




                               XTSC and VSP Parameters

The parameters in VSP (cci_params<>) allow the possibility of changing values in the
middle of a simulation run.  In XTSC, however, parameters are only used at construction
time and changing them has no meaning after the model is constructed.  Although, both
the XTSC global parameters and the xtsc_core parameters are exposed in XTSC_VSP as
cci_params<>, they still have this limitation and should only be changed prior to
running the simulator.

Many XTSC parameters can be changed from the command line that starts the simulation
using the +systemc_args+ construct.  For example:
    $ ncsim ... +systemc_args+-turbo=false
For more information see Section 6.2.6.3 "Passing Command Line Arguments to
extract_parms() in SystemC-Verilog Co-simulation" in the XTSC UG (the instructions there
also apply to XTSC_VSP).




                         The xtsc_core_vsp Command Processor

Each xtsc_core_vsp instance has a small command processor built in that you can
communicate with using the cmd tcl procedure at the ncsim prompt.  The built-in command
processor allows you to do such things as display the core's status, turn logging on and
off, assert/deassert RunStall, and switch between TurboXim Fast-Functional and 
Cycle-Accurate modes of simulation.  For example:
   ncsim> cmd help
   ncsim> cmd status
   ncsim> cmd switch

The XTSC_VSP examples always call the first core instance core0 and if there are two
instances then the second one is called core1.  In lieu of using cmd, you can use core0
or core1 to first switch to that core and then issue the command.  For example:
  ncsim> core0 status
  ncsim> core1 status




                     Converting sc_main.cpp from XTSC to XTSC_VSP

The basic steps for converting sc_main.cpp from using basic (OSCI) XTSC to using XTSC_VSP
can be seen by comparing one of the XTSC_VSP examples to its OSCI counterpart one
directory up.  For example, compare:
        xtsc_memory/sc_main.cpp
with:
        ../xtsc_memory/sc_main.cpp

These steps are:

1) Remove the calls to extract_parms for xtsc_initialize_parms and xtsc_core_parms
   because they will be handled internally by the XTSC_VSP library and the xtsc_core_vsp
   wrapper (respectively).  Calls to extract_parms for other components should be left
   in place.

2) Add "using namespace xtsc_vsp;" after the other "using namespace ..." lines.

3) Change all occurrences of "xtsc_initialize"
                          to "xtsc_vsp_initialize".

4) Change all occurrences of "xtsc_finalize"
                          to "xtsc_vsp_finalize".

5) Change all occurrences of "xtsc_core"
                          to "xtsc_core_vsp"
   (or whatever name is used for the XTSC_VSP wrapper generated by xt-genvspmodel).
   Exception:  Do not change calls to static functions of xtsc_core.  For example, do
   NOT change:
        xtsc_core::setup_multicore_debug(argc, argv);
   to:
        xtsc_core_vsp::setup_multicore_debug(argc, argv);       // Wrong !!!

6) Remove the arguments to the xtsc_core_vsp_parms constructor.  Also remove the
   parentheses.  For example, change:
        xtsc_core_vsp_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
   or (if step 5 has not yet been applied):
        xtsc_core_parms core_parms(CONFIG_NAME, XTENSA_REGISTRY, "");
   to:
        xtsc_core_vsp_parms core_parms;   // No parentheses!

7) Put "get_xtsc_core()." in front of each call to an xtsc_core member function.  For
   example, change:
        core0.load_program("target/memory_test.out");
   to:
        core0.get_xtsc_core().load_program("target/memory_test.out");

8) In most function calls that take a reference to an xtsc_core object as an argument,
   put ".get_xtsc_core()" after the xtsc_core object.  For example, change:
        xtsc_memory_parms memory_parms(core0, "pif");
   to:
        xtsc_memory_parms memory_parms(core0.get_xtsc_core(), "pif");
   Note: The exception to this rule is the xtsc_connect() method which in XTSC_VSP
   should take a reference to an xtsc_core_vsp object, not to an xtsc_core object.

9) Change the include for xtsc_core.h to point to the generated xtsc_core_vsp.h.  For
   example, change:
        #include <xtsc/xtsc_core_vsp.h>
   or (if step 5 has not yet been applied):
        #include <xtsc/xtsc_core.h>
   to:
        #include "xtsc_core_vsp/xtsc_core_vsp.h"
   The path and name will depend on what xt-genvspmodel called the wrapper and where the
   wrapper was placed in the file system.


