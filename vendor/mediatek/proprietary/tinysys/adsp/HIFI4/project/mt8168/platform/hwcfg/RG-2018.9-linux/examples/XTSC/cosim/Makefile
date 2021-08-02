# Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2010 Tensilica Inc.  ALL RIGHTS RESERVED.
# These coded instructions, statements, and computer programs are the
# copyrighted works and confidential proprietary information of
# Tensilica Inc.  They may be adapted and modified by bona fide
# purchasers for internal use, but neither the original nor any adapted
# or modified version may be disclosed or distributed to third parties
# in any manner, medium, or form, in whole or in part, without the prior
# written consent of Tensilica Inc.

DIRS = systemc.on.top \
       verilog.on.top

.PHONY: clean all xtsc-run run

default: all

clean all xtsc-run run:
	for dir in .. $(DIRS); do \
	  if [ x$$dir != x.. ]; then \
	    if [ -d $$dir ]; then \
	      if (cd $$dir && $(MAKE) $@); then \
		true; else exit $$?; fi; \
	    else true; fi; \
	  else true; fi; \
	done


