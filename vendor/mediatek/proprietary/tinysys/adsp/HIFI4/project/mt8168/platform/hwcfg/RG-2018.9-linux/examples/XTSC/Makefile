# Makefile for XTSC examples.

# Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Cadence Design Systems, Inc.

# Note: This Xtensa configuration is incompatible with the following examples:
#       xtsc_core.idma
#       pif2sb_bridge
#       pif2sb_bridge.plugin
#       simple_memory
#       simple_memory.plugin
#       xtsc_wire.shared.memory
#       xtsc_wire
#       tie_pin
#       sc_fifo
#       template_queue_pop
#       template_queue_push
#       xtsc_queue.shared.memory
#       xtsc_queue
#       xtsc_queue.diamond
#       xtsc_queue_pin
#       xtsc_tx_loader
#       xtsc_lookup
#       xtsc_lookup.ram
#       xtsc_lookup_pin
#       xtsc_udma


PRELIM_DIRS = \
	hello_world \
	hello_world.standalone \
	multicore.standalone \
	System.Control.and.Debug.Framework.UART \
	template_command_handler \
	template_memory \
	xtsc_arbiter \
        xtsc_cache \
	xtsc_dma_engine \
	xtsc_memory \
	xtsc_memory.shared.memory.dispatch \
	xtsc_memory.shared.memory.mutex \
	xtsc_memory_trace \
	xtsc_mmio \
	xtsc_pin2tlm_memory_transactor \
	xtsc_router \
	xtsc_tlm2pin_memory_transactor \
	xtsc_tlm22xttlm_transactor \
	xtsc_wire.diamond \
	xtsc_wire_logic \
	xtsc_xttlm2tlm2_transactor \


EXAMPLE_DIRS := $(strip $(PRELIM_DIRS))


PRELIM_DIRS := $(strip $(PRELIM_DIRS))

DIRS = $(PRELIM_DIRS)



# These examples require a host C++ compiler (gcc/MSVC)
# or do not support testing xtsc-run in parallel with sc_main
NON_XTSC_RUN_DIRS = \
                        pif2sb_bridge \
                        pif2sb_bridge.plugin \
                        sc_fifo \
                        simple_memory \
                        simple_memory.plugin \
                        template_command_handler \
                        template_lookup \
                        template_memory \
                        template_queue_pop \
                        template_queue_push \
                        template_wire_read \
                        template_wire_write \
                        xtsc_memory.shared.memory.dispatch \
                        xtsc_queue.shared.memory \
                        xtsc_wire.shared.memory \

XTSC_RUN_DIRS = $(filter-out $(DIRS_WITH_TARGET_CODE),$(filter-out $(NON_XTSC_RUN_DIRS),$(EXAMPLE_DIRS))) xtsc-run

DIRS_all = $(DIRS:=_all)
DIRS_target = $(DIRS:=_target) xtsc-run_target
DIRS_run = $(DIRS:=_run)
DIRS_xtsc_run = $(XTSC_RUN_DIRS:=_xtsc_run)
DIRS_clean = $(DIRS:=_clean) xtsc-run_clean

NO_TIE = 1
include Makefile.include

.PHONY: default all target tie run xtsc-run clean xmp
.PHONY: tie_clean dir_clean vclean 
.PHONY: tie.clean dir.clean very.clean
.PHONY: $(DIRS_all) $(DIRS_run) $(DIRS_xtsc_run) $(DIRS_clean)

ALL_TARGET = all
RUN_TARGET = run

default: xtsc-run


all: $(DIRS_all)
$(DIRS_all):
	$(MAKE) -C $(@:_all=) $(ALL_TARGET) $(FWD_RUNARGS)

target: $(DIRS_target)
$(DIRS_target):
	$(MAKE) -C $(@:_target=) target $(FWD_RUNARGS)

run: $(DIRS_run)
$(DIRS_run):
	$(MAKE) -C $(@:_run=) $(RUN_TARGET) $(FWD_RUNARGS)

xtsc-run: $(DIRS_xtsc_run)
$(DIRS_xtsc_run):
	$(MAKE) -C $(@:_xtsc_run=) xtsc-run $(FWD_XTSCRUNARGS)

tie:
	@echo "The example TIE is not compatible with this Xtensa processor."
	@echo "Unable to continue building this example."
	@exit 1

xmp: 
	$(MAKE) -C XMP

clean: tie.clean $(DIRS_clean)


EXAMPLE_TDK=TIE/example.tdk
tie_clean: tie.clean
tie.clean:
	-$(RMDIR) $(EXAMPLE_TDK)


vclean: very.clean
very.clean: clean
	-$(MAKE) -C XMP clean


$(DIRS_clean):
	$(MAKE) -C $(@:_clean=) clean


dir_clean: dir.clean
dir.clean: $(DIRS_clean)


