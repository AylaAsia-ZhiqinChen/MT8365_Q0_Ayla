# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

COMP_NAME = xtsc_wire_logic_vp
TARGET_OBJECTS = 

EXAMPLE_NAME = $(subst _vp,,$(COMP_NAME))
TARGET_SOURCE_PATH = ../../$(EXAMPLE_NAME)

NO_XTSC_CORE_VP = 1
NO_TIE = 1

include ../Makefile.include
include ../Makefile.common

.PHONY: create_xtsc_wire_logic_vp clean_xtsc_wire_logic_vp 

LOGIC_XML = xtsc_wire_logic_vp/xtsc_wire_logic_vp.xml

all: create_xtsc_wire_logic_vp
clean: clean_xtsc_wire_logic_vp

create_xtsc_wire_logic_vp: $(LOGIC_XML)

$(LOGIC_XML): xtsc_wire_logic_vp/SRC_DIR/xtsc_wire_logic_vp.cpp xtsc_wire_logic_vp/SRC_DIR/xtsc_wire_logic_vp.h
	cd xtsc_wire_logic_vp && $(MAKE)
        
xtsc_wire_logic_vp/SRC_DIR/xtsc_wire_logic_vp.cpp:
	-$(RMDIR) xtsc_wire_logic_vp
	xt-genvpmodel -module=xtsc_wire_logic \
          -version=$(SYNOPSYS_VIRTUAL_PROTOTYPING_VERSION) \
          -definition_file=../../xtsc_wire_logic/logic.def
	-mkdir SRC_DIR
	cp -p xtsc_wire_logic_vp/SRC_DIR/xtsc_wire_logic_vp.def SRC_DIR/.
        
clean_xtsc_wire_logic_vp:
	-$(RMDIR) xtsc_wire_logic_vp
	-$(RMDIR) SRC_DIR

.PHONY: create_xtsc_wire_source_ext_vp clean_xtsc_wire_source_ext_vp 

SOURCE_XML = xtsc_wire_source_ext_vp/xtsc_wire_source_ext_vp.xml

all: create_xtsc_wire_source_ext_vp
clean: clean_xtsc_wire_source_ext_vp

create_xtsc_wire_source_ext_vp: $(SOURCE_XML)

$(SOURCE_XML): xtsc_wire_source_ext_vp/SRC_DIR/xtsc_wire_source_ext_vp.cpp xtsc_wire_source_ext_vp/SRC_DIR/xtsc_wire_source_ext_vp.h
	cd xtsc_wire_source_ext_vp && $(MAKE)
        
xtsc_wire_source_ext_vp/SRC_DIR/xtsc_wire_source_ext_vp.cpp:
	-$(RMDIR) xtsc_wire_source_ext_vp
	xt-genvpmodel -module=xtsc_wire_source \
          -version=$(SYNOPSYS_VIRTUAL_PROTOTYPING_VERSION) \
          -definition_file=../../xtsc_wire_logic/source.def
	-mkdir SRC_DIR
	cp -p xtsc_wire_source_ext_vp/SRC_DIR/xtsc_wire_source_ext_vp.def SRC_DIR/.
        
clean_xtsc_wire_source_ext_vp:
	-$(RMDIR) xtsc_wire_source_ext_vp
	-$(RMDIR) SRC_DIR
        
XML_LIBRARIES = $(LOGIC_XML) $(SOURCE_XML)
