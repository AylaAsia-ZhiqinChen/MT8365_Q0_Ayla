// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

// To debug the xtsc-run system (i.e. to see what commands xtsc-run got from the above script):
// 1) Uncomment the dump_commands line
// 2) Run the system(s) - see README.txt
// 3) Search the log file with the appropriate process id (PID) for the commands:
//    Linux:
//      grep commands xtsc-PID.log
//    MS Windows:
//      findstr commands xtsc-PID.log
//-dump_commands


-logging=$(XTSC_SCRIPT_FILE_PATH)/TextLogger.txt

#ifndef core_source
#define core_source 0
#endif

#ifndef lua_source
#define lua_source 0
#endif

#ifndef core_sink
#define core_sink 0
#endif

#ifndef lua_sink
#define lua_sink 0
#endif

#ifndef TURBO
#define TURBO 0
#endif

#ifdef YIELD
-user_state=YIELD=$(YIELD)
#endif

#ifdef SLEEP
-user_state=SLEEP=$(SLEEP)
#endif

#ifndef LOG
#define LOG 1
#endif
-user_state=LOG=$(LOG)

#ifndef TOTAL
#define TOTAL 20
#endif
-user_state=TOTAL=$(TOTAL)


#lua_beg

  core_source = ("$(core_source)" ~= "0")
  lua_source  = ("$(lua_source)"  ~= "0")
  core_sink   = ("$(core_sink)"   ~= "0")
  lua_sink    = ("$(lua_sink)"    ~= "0")
  if (core_source and lua_source) then xtsc.throw("You cannot -define both core_source and lua_source") end
  if (core_sink   and lua_sink  ) then xtsc.throw("You cannot -define both core_sink   and lua_sink  ") end
  if (not core_source and not lua_source and not core_sink and not lua_sink) then
    core_source = true
    core_sink   = true
  end
  both = (core_source or lua_source) and (core_sink or lua_sink)

  if (core_source or core_sink) then
    xtsc.write("-xtensa_params=$(XTSC_SCRIPT_FILE_PATH_ESC)/../TIE/example.tdk")
    xtsc.write("-no_default_local_memories")
    TURBO = tonumber("$(TURBO)")
    if (not TURBO or (TURBO > 1)) then
      xtsc.throw("TURBO must be 0 or 1")
    else
      xtsc.write("-set_xtsc_parm=turbo=" .. TURBO)
    end
  end

  if (not both) then
    xtsc.write("-user_state=entity=" .. (core_source and "core_source" or lua_source and "lua_source" or core_sink and "core_sink" or "lua_sink"))
    if (core_sink or lua_sink) then
      xtsc.write("-user_state=last=yes")
    end
    xtsc.write("-add_lua_script_file=$(XTSC_SCRIPT_FILE_PATH_ESC)/eoe.lua")
  end

  xtsc.write("-set_wire_parm=host_shared_memory=true")
  xtsc.write("-set_wire_parm=bit_width=50")
  xtsc.write("-create_wire=core0_to_core1")

  if (core_source) then
    xtsc.write("-core_args=$(LOG),$(TOTAL)")
    xtsc.write("-core_program=$(XTSC_SCRIPT_FILE_PATH_ESC)/target/source.out")
    --xtsc.write("-set_core_parm=SimTargetOutput=$(XTSC_SCRIPT_FILE_PATH_ESC)/core0_output.log")
    xtsc.write("-create_core=core0")
    xtsc.write("-connect=core0,status,wire_write,core0_to_core1")
  elseif (lua_source) then
    xtsc.write("-add_lua_script_file=source.lua")
  end

  if (core_sink) then
    xtsc.write("-core_args=$(LOG),$(TOTAL)")
    xtsc.write("-core_program=$(XTSC_SCRIPT_FILE_PATH_ESC)/target/sink.out")
    --xtsc.write("-set_core_parm=SimTargetOutput=$(XTSC_SCRIPT_FILE_PATH_ESC)/core1_output.log")
    xtsc.write("-create_core=core1")
    xtsc.write("-connect=core0_to_core1,wire_read,control,core1")
  elseif (lua_sink) then
    xtsc.write("-add_lua_script_file=$(XTSC_SCRIPT_FILE_PATH_ESC)/sink.lua")
  end

#lua_end


