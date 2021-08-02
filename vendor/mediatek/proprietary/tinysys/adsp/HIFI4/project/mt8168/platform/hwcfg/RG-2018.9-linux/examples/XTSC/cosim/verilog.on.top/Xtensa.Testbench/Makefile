# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Cadence Design Systems, Inc.

EXAMPLE_NAME = Xtensa.Testbench
TARGET_OBJECTS = target/memory_test.out
NO_TIE = 1

TARGET_SOURCE_PATH = ../../../xtsc_tlm2pin_memory_transactor

include ../Makefile.include
include ../Makefile.common

ifdef XTSC_RUN_COSIM_USE_DSO
.PHONY: dso dso_clean

xtsc-run: dso

clean: dso_clean

dso:
	$(MAKE) -f ../../dso/Makefile VENDOR=$(XTSC_RUN_COSIM_VENDOR)

dso_clean:
	$(MAKE) -f ../../dso/Makefile VENDOR=$(XTSC_RUN_COSIM_VENDOR) clean

else
CUSTOM_TARGET_CFLAGS = -D COSIM_WITHOUT_DSO

$(warning )
$(warning ***********************************************************************************************)
$(warning *                                        WARNING                                              *)
$(warning *   Running this example without a peek/poke DSO can result in Xtensa runtime exceptions      *)
$(warning *   during calls such as printf that require semi-hosting features that use peek/poke.        *)
$(warning *   See "Peek/Poke Access to Verilog Memories" in "The xtsc_tlm2pin_memory_transactor Class"  *)
$(warning *   chapter of the xtsc_ug.pdf                                                                *)
$(warning ***********************************************************************************************)
$(warning )

endif
