# Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of Tensilica Inc.
# They may not be modified, copied, reproduced, distributed, or disclosed to
# third parties in any manner, medium, or form, in whole or in part, without
# the prior written consent of Tensilica Inc.

COMP_NAME = xtsc_wire_source_sd
TARGET_OBJECTS = idle.out

$(warning : WARNING: This Xtensa configuration has a writeback data cache.  The target output)
$(warning :          of this example may not be as expected the second and subsequent times )
$(warning :          through main (i.e. after BReset is asserted) because C runtime data may)
$(warning :          be in an inconsistent state.)

EXAMPLE_NAME = $(subst _sd,,$(COMP_NAME))
TARGET_SOURCE_PATH = ../../xtsc-run/$(EXAMPLE_NAME)

ADDITIONAL_CORE_INTERFACES = -add=sysio=BReset

NO_TIE = 1

include ../Makefile.include
include ../Makefile.common

TIMEOUT = 240
