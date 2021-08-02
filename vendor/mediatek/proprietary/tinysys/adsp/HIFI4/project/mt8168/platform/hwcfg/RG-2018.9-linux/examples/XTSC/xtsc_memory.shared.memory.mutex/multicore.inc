// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#environment

#ifndef NumCores
#error You must define NumCores between 1 and 256
#endif

#lua_beg
  for N=0,$(NumCores)-1 do
    xtsc.write("--define=NumCores=$(NumCores)")
    xtsc.write("--define=N=" .. N)
    xtsc.write("--define=MULTICORE_PROCESS=1")
    xtsc.write("--include=$(XTSC_SCRIPT_FILE_PATH_ESC)/coreNN.inc")
  end
#lua_end

