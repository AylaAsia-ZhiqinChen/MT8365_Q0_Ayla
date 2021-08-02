# Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2018 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
# 
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Cadence Design Systems, Inc.



EXAMPLE_NAME       = multicore.standalone$(EXE)
HOST_OBJECTS       = sc_main.$(OBJ)

# Edit the following 6 macros as required to point to your Xtensa registry
# or registries, to identify your config or configs, and to point to your
# TDK  or TDK's (if any).
XTENSA0_SYSTEM     = /proj/mtk05764/ALPS_Base/alps-dev-p0.mp1-mt8168.ep-adsp/vendor/mediatek/proprietary/tinysys/hifi_dsp/source/pub/project/mt8168/common/hwcfg/RG-2018.9-linux/config
XTENSA0_CORE       = hifi4_Aquila_E2_PROD
TIE0_FILE          = 
XTENSA1_SYSTEM     = $(XTENSA0_SYSTEM)
XTENSA1_CORE       = $(XTENSA0_CORE)
TIE1_FILE          = $(TIE0_FILE)

# Edit to identify your target program object files (which will be built
# from corresponding C source files).
TARGET0_OBJECTS    = target/core0.out
TARGET1_OBJECTS    = target/core1.out

# Edit as required for any desired target C compiler flags
TARGET0_CFLAGS     = -g 
TARGET1_CFLAGS     = $(TARGET0_CFLAGS)

# Edit as required for any desired target link libraries
TARGET0_LIBS       = -lm
TARGET1_LIBS       = $(TARGET0_LIBS)

# NOTE: Multiple variants of ISS/XTSC libraries, when available and installed, will be found inside XTTOOLS in
#       a directory that depends on the compiler and SystemC version used.  For example:
# lib/iss:                         directory contains 32-bit libraries compatible with gcc 4.4.X using SystemC 2.3.0
# lib/iss-GCC-4.1:                 directory contains 32-bit libraries compatible with gcc 4.1.X using SystemC 2.3.0
# lib64/iss:                       directory contains 64-bit libraries compatible with gcc 4.4.X using SystemC 2.3.0
# lib64/iss-GCC-4.1:               directory contains 64-bit libraries compatible with gcc 4.1.X using SystemC 2.3.0
# lib64/iss-GCC-4.8/systemc-2.3.1: directory contains 64-bit libraries compatible with gcc 4.8.X using SystemC 2.3.1
# lib64/iss-GCC-5.2/systemc-2.3.1: directory contains 64-bit libraries compatible with gcc 5.2.X using SystemC 2.3.1
# lib64/iss-GCC-5.2/systemc-2.3.2: directory contains 64-bit libraries compatible with gcc 5.2.X using SystemC 2.3.2

# NOTE: Each of the above directories contains a version of xtsc-run linked against the XTSC libraries in that 
#       directory.  Exception:  The version of xtsc-run linked using the XTSC libraries in lib/iss is found in 
#       the bin directory.

# NOTE: When using this make file to compile sc_main, the iss, iss-GCC-4.1, iss-GCC-4.8, or iss-GCC-5.2 directory is
#       chosen based upon the g++ version found on your path.  When using this make file to run xtsc-run, the xtsc-run
#       version used is the one on your path.

# NOTE: If you are using gcc 4.8.X, then you should set LD_LIBRARY_PATH to point to the lib (or lib64) directory of
#       your gcc 4.8 installation.

# NOTE: If you are using gcc 5.2.X, then you should set LD_LIBRARY_PATH to point to the lib64 directory of
#       your gcc 5.2 installation.

# NOTE: By default, 32-bit binaries are built.  If you are on a 64-bit version of MS Windows or Linux and wish to
#       build 64-bit binaries, uncomment the following line to define XTSC_64BIT or pass XTSC_64BIT=1 to make.

# XTSC_64BIT   = 1

# NOTE: If desired, you can use SystemC 2.3.1 when compiling the simulator by defining
#       XTSC_USE_SYSTEMC_2_3_1.  If compiling one of the *.plugin examples and then
#       running it using xtsc-run then you must also ensure that the xtsc-run on your
#       path is from the appropriate systemc-2.3.1 sub-directory listed above.

# NOTE: If desired, you can use SystemC 2.3.2 when compiling the simulator by defining
#       XTSC_USE_SYSTEMC_2_3_2.  If compiling one of the *.plugin examples and then
#       running it using xtsc-run then you must also ensure that the xtsc-run on your
#       path is from the appropriate systemc-2.3.2 sub-directory listed above.

# NOTE: If desired, you can use the shared library version of SystemC by defining
#       XTSC_USE_SHARED_LIBRARY.

# NOTE: If desired, you can use a static version of the original ASI/OSCI SystemC 2.3.0, 
#       2.3.1, or 2.3.2 library that you compiled (source available from www.accellera.org).  
#       To use your compiled version of one of the SystemC libraries, define
#       XTSC_USE_OSCI and set the SYSTEMC_INC, TLM2_INC, SYSTEMC_LIB_PATH, and 
#       SYSTEMC_LIBS macros according to your setup (also define XTSC_USE_SYSTEMC_2_3_1
#       or XTSC_USE_SYSTEMC_2_3_2 if applicable).  Do NOT define XTSC_USE_SHARED_LIBRARY.

# NOTE: See the "Getting Started" Chapter in the XTSC User's Guide (xtsc_ug.pdf).

# XTSC_USE_OSCI           = 1
# XTSC_USE_SYSTEMC_2_3_1  = 1
# XTSC_USE_SYSTEMC_2_3_2  = 1
# XTSC_USE_SHARED_LIBRARY = 1

# NOTE: Using the -libdebug flag when building the example TDK makes TIE wires visible
#       to xt-gdb and Xplorer when running the Xtensa Instruction Set Simulator; however,
#       this can slow down simulation speed.  If desired, comment out the following line
#       to disable TIE wire visibility.
TCFLAGS            = -libdebug

SYSTEMC_INC        = 
TLM2_INC           = 
SYSTEMC_LIB_PATH   = 
SYSTEMC_LIBS       = 

XTSC_LIB_PATH      = -L $(XTTOOLS)/$(LIBDIR)/$(ISSDIR)
CXX                = g++
ARCH_FLAGS         = -m32
TOOLS_SUFFIX       = 
LIB_SUFFIX         = 
ISS_SUFFIX         = -UNKNOWN_GCC_VERSION

ifeq ($(findstring 5.2.,$(shell $(CXX) -dumpversion)),5.2.)
ISS_SUFFIX         = -GCC-5.2
endif
ifeq ($(findstring 4.8.,$(shell $(CXX) -dumpversion)),4.8.)
ISS_SUFFIX         = -GCC-4.8
endif
ifeq ($(findstring 4.4.,$(shell $(CXX) -dumpversion)),4.4.)
ISS_SUFFIX         = 
endif
ifeq ($(findstring 4.1.,$(shell $(CXX) -dumpversion)),4.1.)
ISS_SUFFIX         = -GCC-4.1
endif

ifdef XTSC_64BIT
TOOLS_SUFFIX       = -64
LIB_SUFFIX         = 64
ARCH_FLAGS         = -m64 -fPIC
EXTRA_TC_FLAGS     = -x86_64
endif

LIBDIR             = lib$(LIB_SUFFIX)
ISSDIR             = iss$(ISS_SUFFIX)
XTSC_RUN           = xtsc-run

ifeq ($(XTSC_USE_OSCI),) # {
SYSTEMC_INC        = -I $(XTTOOLS)/include/systemc-2.3.0-static
TLM2_INC           =
SYSTEMC_LIB_PATH   = 
ifneq ($(XTSC_USE_SHARED_LIBRARY),) # {
SYSTEMC_INC        = -I $(XTTOOLS)/include/systemc-2.3.0-shared
SYSTEMC_LIBS       = -l systemc_sh
ifneq ($(XTSC_USE_SYSTEMC_2_3_1),) # {
SYSTEMC_INC        = -I $(XTTOOLS)/include/systemc-2.3.1-shared
TLM2_INC           = -I $(XTTOOLS)/include/tlm
XTSC_LIB_PATH      = -L $(XTTOOLS)/$(LIBDIR)/$(ISSDIR)/systemc-2.3.1
XTSC_RPATH         = -Wl,-rpath,$(XTTOOLS)/$(LIBDIR)/$(ISSDIR)/systemc-2.3.1
endif # }
ifneq ($(XTSC_USE_SYSTEMC_2_3_2),) # {
SYSTEMC_INC        = -I $(XTTOOLS)/include/systemc-2.3.2-shared
TLM2_INC           = -I $(XTTOOLS)/include/tlm
XTSC_LIB_PATH      = -L $(XTTOOLS)/$(LIBDIR)/$(ISSDIR)/systemc-2.3.2
XTSC_RPATH         = -Wl,-rpath,$(XTTOOLS)/$(LIBDIR)/$(ISSDIR)/systemc-2.3.2
endif # }
else # } {
SYSTEMC_LIBS       = -l systemc
ifneq ($(XTSC_USE_SYSTEMC_2_3_1),) # {
SYSTEMC_INC        = -I $(XTTOOLS)/include/systemc-2.3.1-static
TLM2_INC           = -I $(XTTOOLS)/include/tlm
XTSC_LIB_PATH      = -L $(XTTOOLS)/$(LIBDIR)/$(ISSDIR)/systemc-2.3.1
endif # }
ifneq ($(XTSC_USE_SYSTEMC_2_3_2),) # {
SYSTEMC_INC        = -I $(XTTOOLS)/include/systemc-2.3.2-static
TLM2_INC           = -I $(XTTOOLS)/include/tlm
XTSC_LIB_PATH      = -L $(XTTOOLS)/$(LIBDIR)/$(ISSDIR)/systemc-2.3.2
endif # }
endif # }
endif # }

XTTOOLS            = /mtkeda/xtensa/Xplorer-7.0.9/XtDevTools/install/tools/RG-2018.9-linux/XtensaTools
TOOLS              = /mtkeda/xtensa/Xplorer-7.0.9/XtDevTools/install/tools/RG-2018.9-linux/XtensaTools/Tools

ifdef RUNARGS
FWD_RUNARGS        =RUNARGS="$(RUNARGS)"
endif

ifdef XTSCRUNARGS
FWD_XTSCRUNARGS    =XTSCRUNARGS="$(XTSCRUNARGS)"
endif

TARGET_CXX         = $(XTTOOLS)/bin/xt-xcc

LINK               = $(CXX)
DEBUG_CXXFLAGS     = -UNDEBUG -DDEBUG -Wall -O0 -g
#DEBUG_CXXFLAGS    = -DNDEBUG -UDEBUG -Wall -O3
COMMON_CXXFLAGS    = $(ARCH_FLAGS) -c $(DEBUG_CXXFLAGS) -Wno-parentheses
LDFLAGS            = $(ARCH_FLAGS)
LDOUT              = -o 
OBJ                = o
EXE                = 
RM                 = rm -f
RMDIR              = rm -rf

ifneq ($(XTSC_USE_SHARED_LIBRARY),) # {
XTSC_LIB_VER       = _sh
endif # }

ISS_LIB_PATH       = -L $(XTTOOLS)/$(LIBDIR)/$(ISSDIR)
LIB_PATH           = -L $(XTTOOLS)/$(LIBDIR)

INCLUDES           = \
        -I $(XTTOOLS)/include \
        $(SYSTEMC_INC) \
        $(TLM2_INC)

XTENSA_LIBS        = \
        -l xtsc_comp$(XTSC_LIB_VER) \
        -l xtsc$(XTSC_LIB_VER) \
        -l xtmp \
        -l log4xtensa \
        -l xtlua \
        -l xtparams \
        -l dl \
        -l rt \
        -l pthread

RUN_TIME_PATH      = $(XTSC_RPATH)
RUN_TIME_PATH     += -Wl,-rpath,$(XTTOOLS)/$(LIBDIR)/$(ISSDIR)
RUN_TIME_PATH     += -Wl,-rpath,$(XTTOOLS)/$(LIBDIR)
ifeq ($(ISS_SUFFIX),)
RUN_TIME_PATH     += -Wl,-rpath,$(TOOLS)$(TOOLS_SUFFIX)/$(LIBDIR)
endif
ifeq ($(ISS_SUFFIX),-GCC-4.8)
CXX_PATH           = $(shell dirname $(shell which $(CXX)))
RUN_TIME_PATH     += -Wl,-rpath,$(CXX_PATH)/$(shell $(CXX) $(ARCH_FLAGS) -print-multi-os-directory)
endif
ifeq ($(ISS_SUFFIX),-GCC-5.2)
CXX_PATH           = $(shell dirname $(shell which $(CXX)))
RUN_TIME_PATH     += -Wl,-rpath,$(CXX_PATH)/$(shell $(CXX) $(ARCH_FLAGS) -print-multi-os-directory)
COMMON_CXXFLAGS   += -std=gnu++11
endif

D                  = -D

LIBS               = $(SYSTEMC_LIB_PATH) $(XTSC_LIB_PATH) $(ISS_LIB_PATH) $(LIB_PATH) $(RUN_TIME_PATH) $(XTENSA_LIBS) $(SYSTEMC_LIBS)
DEFINES            = \
        $(D) CONFIG0_NAME=\"$(XTENSA0_CORE)\" \
        $(D) XTENSA0_REGISTRY=\"$(XTENSA0_SYSTEM)\" \
        $(D) TDK0_DIR=\"$(TDK0_DIR)\" \
        $(D) TARGET0_PROGRAM=\"$(TARGET0_OBJECTS)\" \
        $(D) CONFIG1_NAME=\"$(XTENSA1_CORE)\" \
        $(D) XTENSA1_REGISTRY=\"$(XTENSA1_SYSTEM)\" \
        $(D) TDK1_DIR=\"$(TDK1_DIR)\" \
        $(D) TARGET1_PROGRAM=\"$(TARGET1_OBJECTS)\" \
        $(ADDITIONAL_DEFINES)

CXXFLAGS           = $(INCLUDES) $(DEFINES) $(COMMON_CXXFLAGS)



.PHONY: all target run clean xtsc-run
BASE_NAME         := $(EXAMPLE_NAME)

.PHONY: default

default: xtsc-run

all: target

target: $(TARGET0_OBJECTS)  $(TARGET1_OBJECTS) 

$(EXAMPLE_NAME): $(HOST_OBJECTS)
	$(LINK) $(LDFLAGS) $(LDOUT)$@ $(HOST_OBJECTS) $(LIBS)

$(TARGET0_OBJECTS): $(TARGET0_OBJECTS:.out=.c)
	$(TARGET_CXX) --xtensa-system=$(XTENSA0_SYSTEM) --xtensa-core=$(XTENSA0_CORE) --xtensa-params=$(TDK0_DIR) \
                      $(TARGET0_CFLAGS) -o $@ $< $(TARGET0_LIBS)

$(TARGET1_OBJECTS): $(TARGET1_OBJECTS:.out=.c)
	$(TARGET_CXX) --xtensa-system=$(XTENSA1_SYSTEM) --xtensa-core=$(XTENSA1_CORE) --xtensa-params=$(TDK1_DIR) \
                      $(TARGET1_CFLAGS) -o $@ $< $(TARGET1_LIBS)

%.$(OBJ): %.cpp
	$(CXX) $(CXXFLAGS) $<

all: $(EXAMPLE_NAME)

run: $(EXAMPLE_NAME)
	./$(EXAMPLE_NAME) $(RUNARGS)

clean:
	-$(RM) target/*.out *.$(OBJ)
	-$(RM) $(EXAMPLE_NAME)
	-$(RM) *.log


info:
	-uname -a
	-pwd
	@echo PATH=$(PATH)
	@echo LIBS=$(LIBS)
	@echo XTSC_USE_OSCI=$(XTSC_USE_OSCI)
	@echo XTSC_USE_SHARED_LIBRARY=$(XTSC_USE_SHARED_LIBRARY)
	@echo XTSC_64BIT=$(XTSC_64BIT)
	@echo SYSTEMC_INC=$(SYSTEMC_INC)
	@echo TLM2_INC=$(TLM2_INC)
	@echo SYSTEMC_LIB_PATH=$(SYSTEMC_LIB_PATH)
	@echo SYSTEMC_LIBS=$(SYSTEMC_LIBS)
	@echo XTSC_LIB_PATH=$(XTSC_LIB_PATH)
	@echo XTTOOLS=$(XTTOOLS)
	@echo LIBDIR=$(LIBDIR)
	@echo ISSDIR=$(ISSDIR)
	@echo CXX=$(CXX)
	@echo CXXFLAGS=$(CXXFLAGS)
	@echo LINK=$(LINK)
	@echo LDFLAGS=$(LDFLAGS)
	@echo TARGET_CXX=$(TARGET_CXX)
	-which $(CXX)
	-$(CXX) -v
ifneq ($(CXX),$(LINK))
	@echo Linker:
	-which $(LINK)
endif
	-which $(TARGET_CXX)


TDK0_DIR           = $(TIE0_FILE:.tie=.tdk)
TDK1_DIR           = $(TIE1_FILE:.tie=.tdk)

$(TARGET0_OBJECTS): $(TDK0_DIR)
$(TARGET1_OBJECTS): $(TDK1_DIR)

ifneq ($(TDK0_DIR),)
$(TDK0_DIR): $(TIE0_FILE)
	XTENSA_SYSTEM=$(XTENSA0_SYSTEM) XTENSA_CORE=$(XTENSA0_CORE) XTENSA_PARAMS=$(TDK0_DIR) \
	$(XTTOOLS)/bin/tc $(TCFLAGS)  -d $@ $<       
endif

ifneq ($(TDK1_DIR),)
$(TDK1_DIR): $(TIE1_FILE)
	XTENSA_SYSTEM=$(XTENSA1_SYSTEM) XTENSA_CORE=$(XTENSA1_CORE) XTENSA_PARAMS=$(TDK1_DIR) \
	$(XTTOOLS)/bin/tc $(TCFLAGS)  -d $@ $<       
endif

.PHONY: vclean
.PHONY: very.clean

vclean: very.clean
very.clean: clean
	-$(RMDIR) $(TDK0_DIR)
	-$(RMDIR) $(TDK1_DIR)

ifneq ($(strip $(XTENSA0_CORE)),$(strip $(XTENSA1_CORE)))
ifneq ($(strip $(TIE0_FILE)),)
ifeq ($(strip $(TIE0_FILE)),$(strip $(TIE1_FILE)))
$(error If XTENSA0_CORE is different than XTENSA1_CORE, then TIE0_FILE, if defined, must be different than TIE1_FILE)
endif
endif
endif

xtsc-run: target
	$(XTSC_RUN) -include=$(BASE_NAME).inc $(XTSCRUNARGS)


