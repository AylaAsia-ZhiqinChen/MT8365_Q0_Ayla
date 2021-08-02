# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2018 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Cadence Design Systems, Inc.

# Note: This Xtensa configuration may be incompatible with the example systems in the following sub-directories:
#       tie_pin_vp
#       xtsc_lookup_vp
#       xtsc_lookup_pin_vp
#       xtsc_queue_vp
#       xtsc_queue_pin_vp
#       xtsc_queue_vp.diamond
#       xtsc_queue_producer_vp
#       xtsc_tx_loader_vp
#       xtsc_wire_vp
#       xtsc_wire_source_vp

include Makefile.include


XTSC_CORE_VP = xtsc_core_vp_hifi4_Aquila_E2_PROD

DIRS =                                  \
        xtsc_arbiter_vp                 \
        xtsc_dma_engine_vp              \
        xtsc_lookup_driver_vp           \
        xtsc_master_vp                  \
        xtsc_memory_vp                  \
        xtsc_mmio_vp                    \
        xtsc_queue_consumer_vp          \
        xtsc_router_vp                  \
        xtsc_slave_vp                   \
	xtsc_wire_vp.diamond            \
        xtsc_wire_logic_vp              \


ALL_EXAMPLE_SUBDIRS =                   \
        xtsc_arbiter_vp                 \
        xtsc_dma_engine_vp              \
        xtsc_lookup_driver_vp           \
        xtsc_lookup_vp                  \
        xtsc_lookup_pin_vp              \
        xtsc_master_vp                  \
        xtsc_memory_vp                  \
        xtsc_mmio_vp                    \
        xtsc_queue_consumer_vp          \
        xtsc_queue_vp                   \
        xtsc_queue_pin_vp               \
        xtsc_queue_vp.diamond           \
        xtsc_queue_producer_vp          \
        xtsc_router_vp                  \
        xtsc_slave_vp                   \
	xtsc_tx_loader_vp               \
        xtsc_wire_vp                    \
        tie_pin_vp                      \
	xtsc_wire_vp.diamond            \
        xtsc_wire_logic_vp              \
        xtsc_wire_source_vp             \



default: all

.PHONY: all clean run create_xtsc_core_vp clean_xtsc_core_vp 


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




all: create_xtsc_core_vp

clean: clean_xtsc_core_vp

create_xtsc_core_vp: 
	if [ ! -d $(XTSC_CORE_VP) ]; then \
          xt-genvpmodel -name=$(XTSC_CORE_VP) -version=$(SYNOPSYS_VIRTUAL_PROTOTYPING_VERSION) \
                --del=sysio=BInterrupt && \
          cd $(XTSC_CORE_VP) && $(MAKE) ; \
	else true; fi;
        
clean_xtsc_core_vp:
	if [ -d $(XTSC_CORE_VP) ]; then \
	  rm -rf $(XTSC_CORE_VP) ; \
	else true; fi;
        
