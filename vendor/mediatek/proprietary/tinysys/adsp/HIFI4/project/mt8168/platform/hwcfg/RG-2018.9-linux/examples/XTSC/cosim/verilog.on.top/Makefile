# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2013 Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

# Note: This Xtensa configuration is incompatible with the example systems in the following sub-directories:
#       xtsc_lookup_pin
#       xtsc_lookup.ram
#       xtsc_mmio
#       xtsc_queue_pin
#       xtsc_queue.diamond
#       xtsc_wire


DIRS =                                  \
        hello_world                     \
        xtsc_pin2tlm_memory_transactor  \
        xtsc_tlm2pin_memory_transactor  \
        xtsc_wire.diamond               \
        Xtensa.Testbench                \


.PHONY: clean all run

default: all

clean all run xtsc-run:
	if [ $@ != test ]; then echo In $$PWD ... ; fi; \
	for dir in .. $(DIRS); do \
	  if [ x$$dir != x.. ]; then \
	    if [ -d $$dir ]; then \
	      if [ $@ != test ]; then echo Doing $@ $$dir ... ; fi; \
	      if (cd $$dir && $(MAKE) $@); then \
		true; else exit $$?; fi; \
	    else true; fi; \
	  else true; fi; \
	done

all:

clean:

tie:
	cd .. && $(MAKE) tie

