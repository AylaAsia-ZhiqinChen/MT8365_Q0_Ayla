                       XTSC and the OSCAR SystemC Library from ASTC

The main README.txt file in this directory provides instructions for building and running the XTSC
examples using the SystemC library from OSCI/ASI.  This file provides additional information needed
when running the XTSC examples using the OSCAR SystemC library from ASTC on Linux.

Note:  Using XTSC with the OSCAR SystemC library from ASTC on MS Windows is not supported.


To use the OSCAR SystemC library from ASTC with the XTSC examples on Linux:

1. Set up your environment as necessary for using the OSCAR SystemC library from ASTC.  Instructions
   for doing this should come from ASTC and should include setting your PATH and the VLAB_HOME and
   RLM_LICENSE environment variables,  

2. Set the XTTOOLS environment variable to point to your installation of XtensaTools.  The XTTOOLS
   environment variable setting should match the XTTOOLS setting in Makefile.include.

3. Update your PATH and LD_LIBRARY_PATH environment variables for a version of OSCAR SystemC library from ASTC supported by XTSC
   (you can tell if an ASTC OSCAR version is supported by XTSC by looking for its directory in
   $XTTOOLS/lib64/iss).  Using OSCAR-3.0.9 as an example:
   bash:
     export PATH=$XTTOOLS/lib64/iss/ASTC-OSCAR-3.0.9:$XTTOOLS/bin:$PATH
     export LD_LIBRARY_PATH=$XTTOOLS/lib64/iss/ASTC-OSCAR-3.0.9:$XTTOOLS/lib64/iss:$XTTOOLS/lib64:$VLAB_HOME/lib:$LD_LIBRARY_PATH
   csh:
     setenv PATH $XTTOOLS/lib64/iss/ASTC-OSCAR-3.0.9:$XTTOOLS/bin:$PATH
     setenv LD_LIBRARY_PATH $XTTOOLS/lib64/iss/ASTC-OSCAR-3.0.9:$XTTOOLS/lib64/iss:$XTTOOLS/lib64:$VLAB_HOME/lib:$LD_LIBRARY_PATH

4. Comment out the error check at the very end of Makefile.include regarding XTSC_USE_OSCI and
   XTSC_USE_SHARED_LIBRARY.

With the above changes the XTSC examples can be run using the version of xtsc-run built against the
OSCAR SystemC library from ASTC by simply executing the make utility in the desired example
directory.  For example:
        cd hello_world
        make

If you want to compile the simulator from sc_main.cpp instead of using xtsc-run, then the following
additional steps are needed.

5. Set up your environment for 64-bit gcc-4.4.5.

6. Edit the Makefile in the desired XTSC example directory to add 3 lines before and 3 lines after the
   following 2 lines:
        include $(EXAMPLE_DIR)/../Makefile.include
        include $(EXAMPLE_DIR)/../Makefile.common

   After editing it should look like this: 
        XTSC_64BIT              = 1
        XTSC_USE_OSCI           = 1
        XTSC_USE_SHARED_LIBRARY = 1

        include $(EXAMPLE_DIR)/../Makefile.include
        include $(EXAMPLE_DIR)/../Makefile.common

        XTSC_OSCAR         = $(XTTOOLS)/$(LIBDIR)/$(ISSDIR)/ASTC-OSCAR-3.0.9
        XTSC_LIB_PATH      = -L $(XTSC_OSCAR) -Wl,-rpath,$(XTSC_OSCAR)
        SYSTEMC_INC        = -I $(VLAB_HOME)/include
        SYSTEMC_LIB_PATH   = -L $(VLAB_HOME)/lib
        SYSTEMC_LIBS       = -l oscar -l semantic
        ADDITIONAL_DEFINES = -D SC_INCLUDE_FX -D SC_INCLUDE_DYNAMIC_PROCESSES

With the above changes the XTSC examples can be compiled against the OSCAR SystemC library from ASTC
by executing the make utility on the all and run targets in the desired example directory.  For
example:
        cd hello_world
        make all run

