# Makefile for customer xtsc-run directory build

# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2015 Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

# Note: This Xtensa configuration is incompatible with the following examples:
#       xtsc_queue_producer
#       xtsc_wire_source
#       xtsc_wire_source.BInterrupt

DIRS = \
	xtsc_lookup_driver \
	xtsc_master \
	xtsc_queue_consumer \
	xtsc_slave \

DIRS := $(strip $(DIRS))

DIRS_all = $(DIRS:=_all)
DIRS_target = $(DIRS:=_target)
DIRS_run = $(DIRS:=_run)
DIRS_xtsc_run = $(DIRS:=_xtsc_run)
DIRS_clean = $(DIRS:=_clean)

include ../Makefile.include

.PHONY: all target clean $(DIRS_all) $(DIRS_clean)

.DEFAULT_GOAL := xtsc-run

all: $(DIRS_all)
$(DIRS_all):
	$(MAKE) -C $(@:_all=) all $(BUILD_FLAG)


target: $(DIRS_target)
$(DIRS_target):
	$(MAKE) -C $(@:_target=) target 

run: $(DIRS_run)
$(DIRS_run):
	$(MAKE) -C $(@:_run=) run $(BUILD_FLAG)

xtsc-run: $(DIRS_xtsc_run)
$(DIRS_xtsc_run):
	$(MAKE) -C $(@:_xtsc_run=) xtsc-run $(BUILD_FLAG)

clean: $(DIRS_clean)
$(DIRS_clean):
	$(MAKE) -C $(@:_clean=) clean

