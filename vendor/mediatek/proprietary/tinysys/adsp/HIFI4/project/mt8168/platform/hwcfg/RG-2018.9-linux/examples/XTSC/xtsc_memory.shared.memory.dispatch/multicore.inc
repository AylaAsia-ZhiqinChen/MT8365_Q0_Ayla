// Customer ID=13943; Build=0x75f5e; Copyright (c) 2015-2016 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#ifndef NumCores
#error You need to define NumCores.  For example: -define=NumCores=8
#endif

#ifndef TURBO
#define TURBO 0
#endif

#lua_beg
for N=0,$(NumCores)-1,1 do
  xtsc.write("--define=N=" .. N)
  xtsc.write("--define=TURBO=$(TURBO)")
  xtsc.write("--include=coreN.inc")
end
#lua_end
