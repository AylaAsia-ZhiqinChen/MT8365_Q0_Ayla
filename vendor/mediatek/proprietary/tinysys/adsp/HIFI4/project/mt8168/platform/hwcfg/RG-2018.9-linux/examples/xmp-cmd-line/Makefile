# Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2009 Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

# This is an example Makefile to show how to use the
# xmp-makefile-fragment to build software targetting XMP subsystems
# outside of the Xtensa Xplorer Environment. Tensilica strongly 
# recommends that users develop their software inside Xplorer,
# but for those who choose not to do so, this example shows how.

# The file xmp-makefile-fragment has detailed instructions
# on what to do with this file.
#
# The core specific variables are already filled in for the
# software provided, but you will need to fill in the variables
# and paths that describe your subsystem.


####################################################################
# Variables that apply to the entire subsystem
XMP_PATH_TO_WORKSPACE = 
XMP_PROJECT_NAME = 
XMP_MEMORY_MAP = 
SHARED_DATA = 
CORE0_RESET = 
XMP_OTHER_CORE_RESET = 

####################################################################
# Core Specific Variables

CORE0_NAME = core0
PROG0_NAME = prog0.out
CORE0_LSP = sim

CORE1_NAME = core1
PROG1_NAME = prog1.out
CORE1_LSP = sim

CORE2_NAME = core2
PROG2_NAME = prog2.out
CORE2_LSP = sim

CORE3_NAME = core3
PROG3_NAME = prog3.out
CORE3_LSP = sim

include xmp-makefile-fragment

CC = xt-xcc
CFLAGS = -O2 -g

default: all

$(SHARED_DATA): shared-data.c
	$(CC) $(XMP_SHARED_CFLAGS) $(CFLAGS) $^ -o $@

$(PROG0_NAME): prog0.c $(SHARED_DATA)
	$(CC) $< -o $@ $(CFLAGS) $(XMP_CORE0_CFLAGS) 

$(PROG1_NAME): prog1.c $(SHARED_DATA)
	$(CC) $< -o $@ $(CFLAGS) $(XMP_CORE1_CFLAGS) 

$(PROG2_NAME): prog2.c $(SHARED_DATA)
	$(CC) $< -o $@ $(CFLAGS) $(XMP_CORE2_CFLAGS) 

$(PROG3_NAME): prog3.c $(SHARED_DATA)
	$(CC) $< -o $@ $(CFLAGS) $(XMP_CORE3_CFLAGS) 

all: $(SHARED_DATA) $(PROG0_NAME) $(PROG1_NAME) $(PROG2_NAME) $(PROG3_NAME) 
	@echo Done making all

clean:
	rm -f *.out *.o

test: all
	$(RUN4_COMMAND)