-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2014-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Cadence Design Systems, Inc.

--[[

Note:  This script and instructions illustrate an old way of using Lua to 
service ISS user simcalls.  A limitation of this old way is that the value
returned by the simcall to the Xtensa target program is from the preceeding
simcall.  An improved way of using Lua for servicing ISS user simcalls is
available using the xtsc_core_parms parameter "SimScriptFile" as shown in the
simcall.vec file (steps 1 and 2 below are the same in both files).


                          Overview

This script illustrates using ISS simcalls to communicate between an
Xtensa program and Lua code running in the simulator.  For more 
information see the simcall discussion in "The XTSC System Control and
Debug Framework" section of the xtsc-run tutorial in the appendix of
the XTSC User's Guide (xtsc_ug.pdf).


                        Instructions

1) ifdef-out the current content of target/hello.c and put the following
   in its place:
      #if 0
        // Original content here
      #else
      #include <stdio.h>
      #include <time.h>
      #include <xtensa/sim.h>

      #define TIMESTAMP fprintf(stdout, "%lu: ", clock());
      #define LOG2(msg, arg1, arg2) \
      { TIMESTAMP; fprintf(stdout, msg, arg1, arg2); fflush(stdout); }

      int main(int argc, char *argv[]) {
        int i, result;
        for (i=0; i<3; ++i) {
          result = xt_iss_simcall(0xbabeface, i, 2*i, i*i, 0, 0);
          LOG2("simcall result #%d is %d \n", i, result);
        }
        return 0;
      }
      #endif

2) Recompile the target program (use xt-make on MS Windows)
        make target

3) Run the simulation
     xtsc-run -i=hello_world.inc -set_xtsc_parm=lua_script_files=simcall.lua

--]]

while true do
  time = xtsc.cmd("sc wait core0.m_p.m_simcall_callback_event");
  arg1 = tonumber(xtsc.cmd("core0 get_simcall_arg 1"))
  arg1 = ((arg1 < 0) and 0x100000000 or 0) + arg1
  note = time                                .. " " .. 
         string.format("0x%08x", arg1)       .. " " .. 
         xtsc.cmd("core0 get_simcall_arg 2") .. " " .. 
         xtsc.cmd("core0 get_simcall_arg 3") .. " " .. 
         xtsc.cmd("core0 get_simcall_arg 4")
  xtsc.cmd("xtsc note " .. note);
  delta_cycles = tonumber(xtsc.cmd("sc sc_delta_count"))
  xtsc.cmd("core0 set_simcall_return_value " .. delta_cycles)
end

