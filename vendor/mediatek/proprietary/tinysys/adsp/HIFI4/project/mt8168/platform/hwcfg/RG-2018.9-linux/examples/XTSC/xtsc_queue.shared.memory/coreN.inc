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
#error You must define NumCores between 2 and 16
#endif

#ifndef N
#error You must define N between 0 and (NumCores-1)
#endif

#ifndef TURBO
#define TURBO 0
#endif

#ifndef LOG
#define LOG 1
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

#ifndef TOTAL
#define TOTAL 20
#endif

#ifndef DEPTH
#define DEPTH 10
#endif

#ifndef MULTICORE_PROCESS
#define MULTICORE_PROCESS 0
#endif

#lua_beg
  NumCores = tonumber("$(NumCores)")
  if (not NumCores or (NumCores < 2) or (NumCores > 16)) then
    xtsc.throw("NumCores must be an integer between 2 and 16")
  end

  N = tonumber("$(N)")
  if (not N or (N >= NumCores)) then
    xtsc.throw("N must be an integer between 0 and (NumCores - 1)")
  end

  TURBO = tonumber("$(TURBO)")
  if (not TURBO or (TURBO > 1)) then
    xtsc.throw("TURBO must be 0 or 1")
  end

  LOG = tonumber("$(LOG)")
  if (not LOG or (LOG > 1)) then
    xtsc.throw("LOG must be 0 or 1")
  end

  TOTAL = tonumber("$(TOTAL)")
  if (not TOTAL) then
    xtsc.throw("Cannot convert TOTAL to a number")
  end

  DEPTH = tonumber("$(DEPTH)")
  if (not DEPTH) then
    xtsc.throw("Cannot convert DEPTH to a number")
  end

  first = (N == 0) 
  last  = (N == (NumCores - 1))
  turbo = (TURBO == 1)
  nU    = first and "" or string.format("%X", N-1)      -- Nibble of the  core Upstream
  nN    =                 string.format("%X", N)        -- Nibble of this core
  nD    = last  and "" or string.format("%X", N+1)      -- Nibble of the  core Downstream

  single_core_process = ("$(MULTICORE_PROCESS)" ~= "1")

  if (first or single_core_process) then
    xtsc.write("-logging=$(LOGGING)")
    xtsc.write("-no_default_local_memories")
    xtsc.write("-xtensa_params=$(XTSC_SCRIPT_FILE_PATH_ESC)/../TIE/example.tdk")
  end

  -- When using multiple single-core simulations, use eoe.lua to hold all of them at end-of-elaboration until the go file is externally created
  if (single_core_process) then
    if (last) then
      xtsc.write("-user_state=last=yes")
    end
    xtsc.write("-set_xtsc_parm=lua_script_file_eoe=$(XTSC_SCRIPT_FILE_PATH_ESC)/eoe.lua")
  end

  if (turbo) then
    xtsc.write("-set_xtsc_parm=turbo=true")
    if (single_core_process) then
      xtsc.write("-user_state=core=" .. N)
      xtsc.write("-set_xtsc_parm=lua_script_file_eos=$(XTSC_SCRIPT_FILE_PATH_ESC)/eos.lua")
    end
  end

  -- Create the core
  xtsc.write("-core_args=" .. N .. "," .. NumCores .. "," .. LOG .. "," .. TOTAL .. "," .. TURBO)
  xtsc.write("-core_program=$(XTSC_SCRIPT_FILE_PATH_ESC)/target/main.out")
  if (LOG == 1) then
    xtsc.write("-set_core_parm=SimTargetOutput=core" .. nN .. ".log")
  end
  xtsc.write("-create_core=core" .. nN)

  -- Optionally create upstream queue except for first core
  if (not first) then
    if (single_core_process) then
      -- Create the upstream queue
      xtsc.write("-set_queue_parm=depth=" .. DEPTH)
      xtsc.write("-set_queue_parm=bit_width=96")
      xtsc.write("-set_queue_parm=host_shared_memory=true")
      xtsc.write("-set_queue_parm=num_producers=0")
      xtsc.write("-create_queue=Q" .. nU .. nN)
      if (turbo) then
        xtsc.write("-add_lua_script_file=$(XTSC_SCRIPT_FILE_PATH_ESC)/turbo.consumer.lua")
      end
    end
    xtsc.write("-connect=Q" .. nU .. nN .. ",queue_pop,INQ1,core" .. nN)
  end

  -- Always create downstream queue except for last core
  if (not last) then
    -- Create the downstream queue
    xtsc.write("-set_queue_parm=depth=" .. DEPTH)
    xtsc.write("-set_queue_parm=bit_width=96")
    xtsc.write("-set_queue_parm=host_shared_memory=true")
    if (single_core_process) then
    xtsc.write("-set_queue_parm=num_consumers=0")
    end
    xtsc.write("-create_queue=Q" .. nN .. nD)
    xtsc.write("-connect=core" .. nN .. ",OUTQ1,queue_push,Q" .. nN .. nD)
    if (turbo and single_core_process) then
      xtsc.write("-add_lua_script_file=$(XTSC_SCRIPT_FILE_PATH_ESC)/turbo.producer.lua")
    end
  end

#lua_end

