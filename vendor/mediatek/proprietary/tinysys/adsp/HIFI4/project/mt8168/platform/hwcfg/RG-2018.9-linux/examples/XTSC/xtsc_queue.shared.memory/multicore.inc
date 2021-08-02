// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2016 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#environment

#ifndef NumCores
#error You must define NumCores between 2 and 16
#endif

#ifndef TURBO
#define TURBO 0
#endif

#ifndef LOG
#define LOG 1
#endif

#ifndef LOGGING
#define LOGGING TextLogger.txt
#endif

#ifndef TOTAL
#define TOTAL 20
#endif

#ifndef DEPTH
#define DEPTH 10
#endif

#lua_beg
  for N=0,$(NumCores)-1 do
    xtsc.write("--define=NumCores=$(NumCores)")
    xtsc.write("--define=N=" .. N)
    xtsc.write("--define=TURBO=$(TURBO)")
    xtsc.write("--define=LOG=$(LOG)")
    xtsc.write("--define=LOGGING=$(LOGGING)")
    xtsc.write("--define=TOTAL=$(TOTAL)")
    xtsc.write("--define=DEPTH=$(DEPTH)")
    xtsc.write("--define=MULTICORE_PROCESS=1")
    xtsc.write("--include=coreN.inc")
  end
#lua_end

