# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of Tensilica Inc.
# They may not be modified, copied, reproduced, distributed, or disclosed to
# third parties in any manner, medium, or form, in whole or in part, without
# the prior written consent of Tensilica Inc.

COMP_NAME = xtsc_wire_logic_sd
TARGET_OBJECTS = 

EXAMPLE_NAME = $(subst _sd,,$(COMP_NAME))
TARGET_SOURCE_PATH = ../../$(EXAMPLE_NAME)

NO_XTSC_CORE_SD = 1
NO_TIE = 1

include ../Makefile.include
include ../Makefile.common

.PHONY: create_xtsc_wire_logic_sd clean_xtsc_wire_logic_sd 

LOGIC_DSO = xtsc_wire_logic_sd/libxtsc_wire_logic_sd.so

all: create_xtsc_wire_logic_sd
clean: clean_xtsc_wire_logic_sd

create_xtsc_wire_logic_sd: $(LOGIC_DSO)

$(LOGIC_DSO): xtsc_wire_logic_sd/xtsc_wire_logic_sd.cpp xtsc_wire_logic_sd/xtsc_wire_logic_sd.h
	cd xtsc_wire_logic_sd && $(MAKE)
        
xtsc_wire_logic_sd/xtsc_wire_logic_sd.cpp:
	-$(RMDIR) xtsc_wire_logic_sd
	xt-gensdmodel -module=xtsc_wire_logic \
          -version=$(SOCDESIGNER_VERSION_NUMBER) \
          -definition_file=../../xtsc_wire_logic/logic.def \
        
clean_xtsc_wire_logic_sd:
	-$(RMDIR) xtsc_wire_logic_sd

.PHONY: create_xtsc_wire_source_ext_sd clean_xtsc_wire_source_ext_sd 

SOURCE_DSO = xtsc_wire_source_ext_sd/libxtsc_wire_source_ext_sd.so

all: create_xtsc_wire_source_ext_sd
clean: clean_xtsc_wire_source_ext_sd

create_xtsc_wire_source_ext_sd: $(SOURCE_DSO)

$(SOURCE_DSO): xtsc_wire_source_ext_sd/xtsc_wire_source_ext_sd.cpp xtsc_wire_source_ext_sd/xtsc_wire_source_ext_sd.h
	cd xtsc_wire_source_ext_sd && $(MAKE)
        
xtsc_wire_source_ext_sd/xtsc_wire_source_ext_sd.cpp:
	-$(RMDIR) xtsc_wire_source_ext_sd
	xt-gensdmodel -module=xtsc_wire_source \
          -version=$(SOCDESIGNER_VERSION_NUMBER) \
          -definition_file=../../xtsc_wire_logic/source.def \
        
clean_xtsc_wire_source_ext_sd:
	-$(RMDIR) xtsc_wire_source_ext_sd
        
