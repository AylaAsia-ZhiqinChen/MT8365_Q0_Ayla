# Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2013 Tensilica Inc.  ALL RIGHTS RESERVED.
# 
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

EXAMPLE_NAME = xtsc_wire.diamond
HOST_OBJECTS = sc_main.o
export core0_PROGRAM=target/source.out
export core1_PROGRAM=target/sink.out
TARGET_OBJECTS = $(core0_PROGRAM) $(core1_PROGRAM)
NO_TIE = 1
EXAMPLE_DIR = $(CURDIR)

TARGET_SOURCE_PATH = ../../$(EXAMPLE_NAME)

ADDITIONAL_CORE_INTERFACES = -add=tie=EXPSTATE -add=tie=IMPWIRE

include ../Makefile.include
include ../Makefile.common

