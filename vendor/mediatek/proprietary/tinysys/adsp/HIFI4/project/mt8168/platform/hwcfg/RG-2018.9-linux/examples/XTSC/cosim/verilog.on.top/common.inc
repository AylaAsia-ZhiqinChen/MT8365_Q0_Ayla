// Customer ID=13943; Build=0x75f5e; Copyright (c) 2008-2013 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#environment

-xtensa_core=hifi4_Aquila_E2_PROD
-logging=$(XTSC_SCRIPT_FILE_PATH)/../../TextLogger.txt

#ifndef XTSC_RUN_COSIM_GCC
#define XTSC_RUN_COSIM_GCC=g++
#endif

#ifndef XTSC_RUN_COSIM_VENDOR
#define XTSC_RUN_COSIM_VENDOR=cadence
#endif

#ifeq ($(XTSC_RUN_COSIM_VENDOR),cadence)
#define VENDOR_VERSION=IUS122
#define GCC_VERSION=4.4.5
#endif

#ifeq ($(XTSC_RUN_COSIM_VENDOR),mentor)
#define VENDOR_VERSION=10.1c
#define GCC_VERSION=4.5.2
#endif

#ifeq ($(XTSC_RUN_COSIM_VENDOR),synopsys)
#define VENDOR_VERSION=G-2012.09
#define GCC_VERSION=4.5.2
#endif

#ifndef XTSC_RUN_COSIM_VENDOR_VERSION
#define XTSC_RUN_COSIM_VENDOR_VERSION=$(VENDOR_VERSION)
#endif

#ifndef XTSC_RUN_COSIM_GCC_VERSION
#define XTSC_RUN_COSIM_GCC_VERSION=$(GCC_VERSION)
#endif

#ifndef EXTRA_PLUS_ARGS
#define EXTRA_PLUS_ARGS
#endif

#ifdef XTSC_RUN_COSIM_64BIT
#define ARCH_FLAG -m64 -fPIC
#else
#define ARCH_FLAG -m32
#endif

-cosim=verilog,$(XTSC_RUN_COSIM_VENDOR),$(XTSC_RUN_COSIM_VENDOR_VERSION),$(XTSC_RUN_COSIM_GCC_VERSION)
-dir=xtsc-run

#ifeq ($(XTSC_RUN_COSIM_VENDOR),cadence)
-set_make_parm=cxx=$(XTSC_RUN_COSIM_GCC)
-set_make_parm=ncverilog_args=+ncfatal+CUVMPW +ncfatal+BNDWRN +sv -sem2009 +access++R +nctimescale+1ps/1ps
-set_make_parm=ncsim_args=+dumpvars -licqueue $(EXTRA_PLUS_ARGS)
// sc_stop causes elaboration failure (ncverilog *E,ELBERR status 250) for IUS10.2 s010 and s012 but not s017
//-set_xtsc_parm=call_sc_stop_on_finalize=false
#endif

#ifeq ($(XTSC_RUN_COSIM_VENDOR),mentor)
-set_make_parm=modelsim_ini=$(XTSC_SCRIPT_FILE_PATH)/../verilog.sources/modelsim.ini
-set_make_parm=vlog_args=-mfcu
-set_make_parm=vsim_args=+dumpvars $(EXTRA_PLUS_ARGS)
#endif

#ifeq ($(XTSC_RUN_COSIM_VENDOR),synopsys)
-set_make_parm=cxx=$(XTSC_RUN_COSIM_GCC)
-set_make_parm=simv_args=+dumpvars +vcs+lic+wait -q $(EXTRA_PLUS_ARGS)
-set_make_parm=vcs_args=-sverilog -debug_all -cflags "-g $(ARCH_FLAG)" -timescale=1ps/1ps
#endif

