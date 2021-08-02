$(error Makefile disabled because config might not support this example)
# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of Tensilica Inc.
# They may not be modified, copied, reproduced, distributed, or disclosed to
# third parties in any manner, medium, or form, in whole or in part, without
# the prior written consent of Tensilica Inc.

COMP_NAME = xtsc_tx_loader_vp
TARGET_OBJECTS = target/producer.out target/consumer.out
CUSTOM_TARGET_CFLAGS = -mlsp=tiny

EXAMPLE_NAME = $(subst _vp,,$(COMP_NAME))
TARGET_SOURCE_PATH = ../../$(EXAMPLE_NAME)

include ../Makefile.include
include ../Makefile.common

custom_target: $(TARGET_OBJECTS) job1.scr job2.scr
	xt-load --hex job1.scr -o job1.img
	xt-load --hex job2.scr -o job2.img

custom_clean:
	-$(RM) job1.img
	-$(RM) job2.img

.PRECIOUS: %.scr

.SUFFIXES : .scr

%.scr:
	$(CP) $(TARGET_SOURCE_PATH)/$@ $@

ADDITIONAL_CORE_INTERFACES = -add=tx_xfer -add=tie=OUTQ1 -add=tie=INQ1 -add=sysout=CoreHalted

.PHONY: create_xtsc_wire_logic_vp clean_xtsc_wire_logic_vp 

LOGIC_XML = xtsc_wire_logic_vp/xtsc_wire_logic_vp.xml
XML_LIBRARIES = $(CORE_XML) $(LOGIC_XML)

all: create_xtsc_wire_logic_vp
clean: clean_xtsc_wire_logic_vp

create_xtsc_wire_logic_vp: $(LOGIC_XML)

$(LOGIC_XML): xtsc_wire_logic_vp/SRC_DIR/xtsc_wire_logic_vp.cpp xtsc_wire_logic_vp/SRC_DIR/xtsc_wire_logic_vp.h
	cd xtsc_wire_logic_vp && $(MAKE)
        
xtsc_wire_logic_vp/SRC_DIR/xtsc_wire_logic_vp.cpp:
	-$(RMDIR) xtsc_wire_logic_vp
	xt-genvpmodel -module=xtsc_wire_logic \
          -version=$(SYNOPSYS_VIRTUAL_PROTOTYPING_VERSION) \
          -definition_file=../../xtsc_tx_loader/logic.def
	-mkdir SRC_DIR
	cp -p xtsc_wire_logic_vp/SRC_DIR/xtsc_wire_logic_vp.def SRC_DIR/.
        
clean_xtsc_wire_logic_vp:
	-$(RMDIR) xtsc_wire_logic_vp
	-$(RMDIR) SRC_DIR

