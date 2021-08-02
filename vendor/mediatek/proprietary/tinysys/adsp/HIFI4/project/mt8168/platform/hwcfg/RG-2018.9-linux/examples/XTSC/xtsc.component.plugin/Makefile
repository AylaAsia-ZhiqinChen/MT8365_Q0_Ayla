# Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.

# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Cadence Design Systems, Inc.


# See README.txt
# Specify the desired copied and renamed XTSC component library module here 
export MODULE = xtsc_xttlm2tlm2_transactor_ext


LIBRARY_NAME = $(MODULE)
EXAMPLE_NAME = system$(EXE)
TEST_NAME    = $(LIBRARY)
XTSC_PLUGIN  = 1

LIBRARY_OBJECTS = module_plugin_interface.$(OBJ) $(MODULE).$(OBJ)

HOST_OBJECTS = sc_main.$(OBJ)

TARGET_OBJECTS =
NO_TIE = 1
EXAMPLE_DIR = $(CURDIR)

# ../Makefile.include
# ../Makefile.common
include $(EXAMPLE_DIR)/../Makefile.include
include $(EXAMPLE_DIR)/../Makefile.common

ADDITIONAL_DEFINES += \
        $(D) MODULE=$(MODULE) \
        $(D) MODULE_PARMS=$(MODULE)_parms \
        $(D) MODULE_PLUGIN_INTERFACE=$(MODULE)_plugin_interface \
        $(D) MODULE_HEADER=\"$(MODULE).h\"


.PHONY: xtsc-run sc_main
xtsc-run: library
all: library
sc_main: $(LIBRARY)
sc_main.cpp: sc_main

very.clean: sc_main.clean

HOST_SOURCES = module_plugin_interface.cpp $(MODULE).cpp sc_main.cpp


