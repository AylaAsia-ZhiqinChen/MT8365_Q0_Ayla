# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2013 Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

# Note: This Xtensa configuration is incompatible with the example systems in the following sub-directories:
#       xtsc_lookup
#       xtsc_lookup_pin
#       xtsc_lookup.ram
#       xtsc_mmio
#       xtsc_queue
#       xtsc_queue_pin
#       xtsc_queue.diamond
#       xtsc_wire
#       tie_pin

include Makefile.include


PRELIM_DIRS =                           \
        xtsc_arbiter                    \
        xtsc_dma_engine                 \
        xtsc_memory                     \
        xtsc_pin2tlm_memory_transactor  \
        xtsc_router                     \
        xtsc_tlm2pin_memory_transactor  \
        xtsc_tlm22xttlm_transactor      \
        xtsc_wire.diamond               \
        xtsc_xttlm2tlm2_transactor      \


PRELIM_DIRS := $(strip $(PRELIM_DIRS))

DIRS = $(PRELIM_DIRS)

default: all

.PHONY: clean all run

ALL_DIRS   = $(DIRS:%=all-%)
CLEAN_DIRS = $(DIRS:%=clean-%)
RUN_DIRS   = $(DIRS:%=run-%)


all: $(ALL_DIRS)

clean: $(CLEAN_DIRS)

run: $(RUN_DIRS)


$(ALL_DIRS): all-% : %
	@echo Doing all $*
	cd $* && $(MAKE) all

$(CLEAN_DIRS): clean-% : %
	@echo Doing clean $*
	cd $* && $(MAKE) clean

$(RUN_DIRS): run-% : %
	@echo Doing run $*
	cd $* && $(MAKE) run





