// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#environment

#include "$(XTSC_SCRIPT_FILE_PATH)/common.h"

#ifndef NumCores
#error You must define NumCores between 1 and 256
#endif

#ifndef N
#error You must define N between 0 and (NumCores-1)
#endif

#ifndef LOGGING
#define LOGGING $(XTSC_SCRIPT_FILE_PATH_ESC)/TextLogger.txt
#elifeq ($(LOGGING),)
#define LOGGING $(XTSC_SCRIPT_FILE_PATH_ESC)/TextLogger.txt
#elifeq ($(LOGGING),1)
#define LOGGING $(XTSC_SCRIPT_FILE_PATH_ESC)/TextLogger.txt
#elifeq ($(LOGGING),0)
#define LOGGING off
#endif

#ifndef TURBO
#define TURBO 0
#endif

#ifndef TOTAL
#define TOTAL 5
#endif

#ifndef WORDS
#define WORDS 16
#endif

#ifndef CHECKS
#define CHECKS 10
#endif

#ifndef MULTICORE_PROCESS
#define MULTICORE_PROCESS 0
#endif

#lua_beg
  NumCores = tonumber("$(NumCores)")
  if (not NumCores or (NumCores < 1) or (NumCores > 256) or (NumCores ~= math.floor(NumCores))) then
    xtsc.throw("NumCores must be an integer between 1 and 256")
  end
  xtsc.write("-user_state=NumCores=" .. NumCores)

  N = tonumber("$(N)")
  if (not N or (N >= NumCores)) or (N ~= math.floor(N)) then
    xtsc.throw("N=" .. N .. " must be an integer between 0 and (NumCores - 1)=" .. (NumCores-1))
  end

  TURBO = tonumber("$(TURBO)")
  if (not TURBO or (TURBO > 1) or (TURBO ~= math.floor(TURBO))) then
    xtsc.throw("TURBO must be 0 or 1")
  end

  TOTAL = tonumber("$(TOTAL)")
  if (not TOTAL or (TOTAL ~= math.floor(TOTAL))) then
    xtsc.throw("TOTAL must be a non-negative integer")
  end

  WORDS = tonumber("$(WORDS)")
  if (not WORDS or (WORDS < 1) or (WORDS > 256) or (WORDS ~= math.floor(WORDS))) then
    xtsc.throw("WORDS must be an integer between 1 and 256")
  end

  CHECKS = tonumber("$(CHECKS)")
  if (not CHECKS or (CHECKS < 1) or (CHECKS > 256) or (CHECKS ~= math.floor(CHECKS))) then
    xtsc.throw("CHECKS must be an integer between 1 and 256")
  end

  first = (N == 0) 
  last  = (N == (NumCores - 1))
  turbo = (TURBO == 1)
  NN    = string.format("%02X", N)        -- Nibbles of this core

  single_core_process = ("$(MULTICORE_PROCESS)" ~= "1")
  xtsc.write("-user_state=single_core_process=" .. (single_core_process and "1" or "0"))
  use_arbiters = not single_core_process and (NumCores > 1)

  if (first or single_core_process) then
    xtsc.write("-logging=$(LOGGING)")
    xtsc.write("-no_default_memories")
  end

  -- When using multiple single-core simulations:
  --   Use eoe.lua to hold all of them  at end-of-elaboration until the go   file is externally created
  --   Use eos.lua to hold the last one at end-of-simulation  until the stop file is externally created
  if (single_core_process and (NumCores > 1)) then
    xtsc.write("-user_state=error_addr=" .. string.format("0x%08x", tonumber($(SHMEM_WORK_ERRORS)) + N*4))
    xtsc.write("-user_state=NN=" .. NN)
    xtsc.write("-set_xtsc_parm=lua_script_file_eoe=$(XTSC_SCRIPT_FILE_PATH_ESC)/eoe.lua")
    if (last) then
      xtsc.write("-user_state=last=yes")
      xtsc.write("-set_xtsc_parm=lua_script_file_eos=$(XTSC_SCRIPT_FILE_PATH_ESC)/eos.lua")
    end
  end

  if (turbo) then
    xtsc.write("-set_xtsc_parm=turbo=true")
  end

  -- Create source to drive BInterruptXX for coreNN
  xtsc.write("-set_source_parm=bit_width=1")
  xtsc.write("-set_source_parm=script_file=$(XTSC_SCRIPT_FILE_PATH_ESC)/coreNN_BInterruptXX.vec")
  xtsc.write("-create_source=core" .. NN .. "_BInterruptXX")

  -- Create coreNN
  xtsc.write("-set_core_parm=SimTargetArgs=" .. N .. "," .. TOTAL .. "," .. WORDS .. "," .. CHECKS)
  xtsc.write("-core_program=$(XTSC_SCRIPT_FILE_PATH_ESC)/target/main.out")
--xtsc.write("-set_core_parm=SimTargetOutput=core" .. NN .. ".log")
  xtsc.write("-set_core_parm=SimLogDisassembly=true")
  xtsc.write("-create_core=core" .. NN)

  -- Create wire PWaitModeNN
  xtsc.write("-set_wire_parm=bit_width=1")
  xtsc.write("-create_wire=PWaitMode" .. NN)

  -- Create routerNN
  xtsc.write("-set_router_parm=num_slaves=3")
  xtsc.write("-set_router_parm=default_port_num=0")
  xtsc.write("-set_router_parm=routing_table=<;>1 $(SHMEM_LOCK_BASE) $(SHMEM_LOCK_END);2 $(SHMEM_WORK_BASE) $(SHMEM_WORK_END)")
  xtsc.write("-create_router=router" .. NN)

  -- Create sysmemNN for core
  xtsc.write("-create_memory=sysmem" .. NN)

  if (first or single_core_process) then
    -- Create shared memory lock area - use mutex and disallow fast access
    xtsc.write("-set_memory_parm=host_shared_memory=true")
    xtsc.write("-set_memory_parm=host_mutex=true")
    xtsc.write("-set_memory_parm=use_fast_access=false")
    xtsc.write("-set_memory_parm=start_byte_address=$(SHMEM_LOCK_BASE)")
    xtsc.write("-set_memory_parm=memory_byte_size=$(SHMEM_LOCK_SIZE)")
    xtsc.write("-set_memory_parm=page_byte_size=$(SHMEM_LOCK_SIZE)")
    xtsc.write("-create_memory=shmem_lock")

    -- Create shared memory work area - no mutex needed and fast access is okay
    xtsc.write("-set_memory_parm=host_shared_memory=true")
    xtsc.write("-set_memory_parm=host_mutex=false")
    xtsc.write("-set_memory_parm=use_fast_access=true")
    xtsc.write("-set_memory_parm=start_byte_address=$(SHMEM_WORK_BASE)")
    xtsc.write("-set_memory_parm=memory_byte_size=$(SHMEM_WORK_SIZE)")
    xtsc.write("-set_memory_parm=page_byte_size=$(SHMEM_WORK_SIZE)")
    xtsc.write("-create_memory=shmem_work")

    if (use_arbiters) then
      xtsc.write("-set_arbiter_parm=num_masters=" .. NumCores)
      xtsc.write("-create_arbiter=shmem_work_arb")
      xtsc.write("-set_arbiter_parm=num_masters=" .. NumCores)
      xtsc.write("-create_arbiter=shmem_lock_arb")
      xtsc.write("-connect=shmem_work_arb,master_port,slave_port,shmem_work")
      xtsc.write("-connect=shmem_lock_arb,master_port,slave_port,shmem_lock")
    end
  end

  -- Connect them all
  xtsc.write("-connect=core"   .. NN .. "_BInterruptXX,,$(BInterruptXX),core" .. NN)
  xtsc.write("-connect=core"   .. NN .. ",PWaitMode,wire_write,PWaitMode"     .. NN)
  xtsc.write("-connect=core"   .. NN .. ",pif,slave_port,router"              .. NN)
  xtsc.write("-connect=router" .. NN .. ",master_port[0],slave_port,sysmem"   .. NN)
  if (use_arbiters) then
    xtsc.write("-connect=router" .. NN .. ",master_port[1],slave_port[" .. N .. "],shmem_lock_arb")
    xtsc.write("-connect=router" .. NN .. ",master_port[2],slave_port[" .. N .. "],shmem_work_arb")
  else
    xtsc.write("-connect=router" .. NN .. ",master_port[1],slave_port,shmem_lock")
    xtsc.write("-connect=router" .. NN .. ",master_port[2],slave_port,shmem_work")
  end


#lua_end

