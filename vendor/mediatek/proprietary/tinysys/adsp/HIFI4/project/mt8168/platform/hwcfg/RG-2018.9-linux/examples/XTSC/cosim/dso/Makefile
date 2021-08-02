# Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Cadence Design Systems, Inc.



# Instructions: 
# - Modify the following macros to point to the version of g++ to be used and the location
#   of your EDA tools installation.
# - When running make, pass in a make macro definition for VENDOR of cadence|mentor|synopsys.
#   For example:
#     make clean all VENDOR=cadence

ifndef XTSC_RUN_COSIM_GCC
XTSC_RUN_COSIM_GCC = g++
endif

CADENCE_INC 	= -I $(CDS_INST_DIR)/tools.lnx86/include
MENTOR_INC 	= -I $(MODELTECH_HOME)/include 
SYNOPSYS_INC 	= -I $(VCS_HOME)/include 

ifdef XTSC_RUN_COSIM_64BIT
arch=
else
arch=-m32
endif


ifeq ($(VENDOR),cadence)
CXX = $(XTSC_RUN_COSIM_GCC)
CXX_FLAGS = -fPIC $(arch)
LINK_FLAGS = $(arch)
INCLUDES = $(CADENCE_INC)
else
ifeq ($(VENDOR),mentor)
CXX = $(XTSC_RUN_COSIM_GCC)
CXX_FLAGS = -fPIC $(arch)
LINK_FLAGS = -Bsymbolic $(arch)
INCLUDES = $(MENTOR_INC)
else
ifeq ($(VENDOR),synopsys)
CXX = $(XTSC_RUN_COSIM_GCC)
CXX_FLAGS = -fPIC $(arch)
LINK_FLAGS = $(arch)
INCLUDES = $(SYNOPSYS_INC)
else
$(error macro VENDOR must be defined to one of cadence|mentor|synopsys)
endif
endif
endif


ME = $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
PATH_TO_DSO  = $(dir $(ME))

.PHONY: all cadence mentor synopsys clean clean-cadence clean-mentor clean-synopsys


MEMORIES = \
	   hifi4_Aquila_E2_PROD_pifmem \
	   hifi4_Aquila_E2_PROD_siomem \
	   hifi4_Aquila_E2_PROD_iram0 \
	   hifi4_Aquila_E2_PROD_iram1 \
	   hifi4_Aquila_E2_PROD_dram0 \
	   hifi4_Aquila_E2_PROD_dram1 \



DEFINES = -D $(VENDOR) -D PEEK_CONFIG_MEMORY=peek_$(MEMORY) -D POKE_CONFIG_MEMORY=poke_$(MEMORY)

DSO_VENDOR = dso/$(VENDOR)

all: 
	for memory in $(MEMORIES); do \
	  $(MAKE) -f $(ME) $(DSO_VENDOR) MEMORY=$$memory; \
	done


$(DSO_VENDOR): $(DSO_VENDOR)/lib_$(MEMORY).so


$(DSO_VENDOR)/lib_$(MEMORY).so: $(DSO_VENDOR)/$(MEMORY).o
	$(CXX) -shared $(LINK_FLAGS) -o $@ $<


$(DSO_VENDOR)/$(MEMORY).o: $(PATH_TO_DSO)/dso.cpp
	mkdir -p $(DSO_VENDOR)
	$(CXX) $(DEFINES) $(CXX_FLAGS) -c -g -O0 $(INCLUDES) -o $@ $<


clean: 
	-rm -rf $(DSO_VENDOR)


