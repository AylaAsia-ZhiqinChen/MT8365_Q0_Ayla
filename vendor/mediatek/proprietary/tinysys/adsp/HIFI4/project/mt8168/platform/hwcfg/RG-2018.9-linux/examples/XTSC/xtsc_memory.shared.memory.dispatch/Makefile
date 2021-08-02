# Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2016 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
# 
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Cadence Design Systems, Inc.

EXAMPLE_NAME = dispatch$(EXE)
HOST_OBJECTS = dispatch.$(OBJ)
TARGET_OBJECTS = target/dsp.out
NO_TIE = 1
EXAMPLE_DIR = $(CURDIR)

# ../Makefile.include
# ../Makefile.common

include $(EXAMPLE_DIR)/../Makefile.include
include $(EXAMPLE_DIR)/../Makefile.common

ifeq ($(OS),Windows_NT)
LIBS = advapi32.lib
else
LIBS = -l rt
endif
