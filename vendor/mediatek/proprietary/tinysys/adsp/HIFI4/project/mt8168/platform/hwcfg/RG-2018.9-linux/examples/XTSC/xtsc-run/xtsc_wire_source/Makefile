# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 Tensilica Inc.  ALL RIGHTS RESERVED.

# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

$(warning : WARNING: This Xtensa configuration has a writeback data cache.  The target output)
$(warning :          of this example may not be as expected the second and subsequent times )
$(warning :          through main (i.e. after BReset is asserted) because C runtime data may)
$(warning :          be in an inconsistent state.)

TARGET_OBJECTS = idle.out
SCRIPT = source.inc
NO_TIE = 1

include ../../Makefile.include
include ../Makefile.common
