# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of Tensilica Inc.
# They may not be modified, copied, reproduced, distributed, or disclosed to
# third parties in any manner, medium, or form, in whole or in part, without
# the prior written consent of Tensilica Inc.

COMP_NAME = xtsc_dma_engine_sd
TARGET_OBJECTS = target/main.out

EXAMPLE_NAME = $(subst _sd,,$(COMP_NAME))
TARGET_SOURCE_PATH = ../../$(EXAMPLE_NAME)

NO_TIE = 1

MAXLIBS += --maxlib maxlib.conf

include ../Makefile.include
include ../Makefile.common

CORE_MEMORY_INTERFACES += -add=memory=dram0*
ADDITIONAL_CORE_INTERFACES  = -add=inbound=inbound_pif
ADDITIONAL_CORE_INTERFACES += -add=sysin=BInterrupt06

.PHONY: create_xtsc_mmio_sd clean_xtsc_mmio_sd 

MMIO_DSO = xtsc_mmio_sd/libxtsc_mmio_sd.so

all: create_xtsc_mmio_sd
clean: clean_xtsc_mmio_sd

create_xtsc_mmio_sd: $(MMIO_DSO)

$(MMIO_DSO): xtsc_mmio_sd/xtsc_mmio_sd.cpp xtsc_mmio_sd/xtsc_mmio_sd.h
	cd xtsc_mmio_sd && $(MAKE)
        
xtsc_mmio_sd/xtsc_mmio_sd.cpp:
	-$(RMDIR) xtsc_mmio_sd
	xt-gensdmodel -module=xtsc_mmio \
          -version=$(SOCDESIGNER_VERSION_NUMBER) \
          -definition_file=../../xtsc_dma_engine/mmio.def \
        
clean_xtsc_mmio_sd:
	-$(RMDIR) xtsc_mmio_sd
        

