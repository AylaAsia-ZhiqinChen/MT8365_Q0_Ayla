# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of Tensilica Inc.
# They may not be modified, copied, reproduced, distributed, or disclosed to
# third parties in any manner, medium, or form, in whole or in part, without
# the prior written consent of Tensilica Inc.

COMP_NAME = xtsc_tx_loader_sd
TARGET_OBJECTS = target/producer.out target/consumer.out
CUSTOM_TARGET_CFLAGS = -mlsp=tiny

EXAMPLE_NAME = $(subst _sd,,$(COMP_NAME))
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
          -definition_file=../../xtsc_tx_loader/logic.def \
        
clean_xtsc_wire_logic_sd:
	-$(RMDIR) xtsc_wire_logic_sd

