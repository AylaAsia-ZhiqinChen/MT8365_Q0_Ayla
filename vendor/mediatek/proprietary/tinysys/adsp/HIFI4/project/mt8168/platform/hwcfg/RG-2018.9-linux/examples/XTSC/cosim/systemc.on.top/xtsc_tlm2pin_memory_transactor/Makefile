# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of Tensilica Inc.
# They may not be modified, copied, reproduced, distributed, or disclosed to
# third parties in any manner, medium, or form, in whole or in part, without
# the prior written consent of Tensilica Inc.

EXAMPLE_NAME = xtsc_tlm2pin_memory_transactor
TARGET_OBJECTS = target/memory_test.out
NO_TIE = 1

TARGET_SOURCE_PATH = ../../../$(EXAMPLE_NAME)

include ../Makefile.include
include ../Makefile.common


.PHONY: dso dso_clean

xtsc-run: dso

clean: dso_clean

dso:
	$(MAKE) -f ../../dso/Makefile VENDOR=$(XTSC_RUN_COSIM_VENDOR)

dso_clean:
	$(MAKE) -f ../../dso/Makefile clean VENDOR=$(XTSC_RUN_COSIM_VENDOR)

