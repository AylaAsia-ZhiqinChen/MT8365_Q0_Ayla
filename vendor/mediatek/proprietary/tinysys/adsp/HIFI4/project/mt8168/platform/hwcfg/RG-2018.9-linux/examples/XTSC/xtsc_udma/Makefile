$(error Makefile disabled because config might not support this example)
# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2014 Tensilica Inc.  ALL RIGHTS RESERVED.
#
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

EXAMPLE_NAME = xtsc_udma$(EXE)
HOST_OBJECTS = sc_main.$(OBJ)
TARGET_OBJECTS = target/main.out
NO_TIE = 1
EXAMPLE_DIR = $(CURDIR)

include $(EXAMPLE_DIR)/../Makefile.include
include $(EXAMPLE_DIR)/../Makefile.common

sc_main.cpp: sc_main
very.clean: sc_main.clean
