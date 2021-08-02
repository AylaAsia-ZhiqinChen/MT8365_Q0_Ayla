-- Customer ID=13943; Build=0x75f5e; Copyright (c) 2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
-- These coded instructions, statements, and computer programs are the
-- copyrighted works and confidential proprietary information of
-- Tensilica Inc.  They may be adapted and modified by bona fide
-- purchasers for internal use, but neither the original nor any adapted
-- or modified version may be disclosed or distributed to third parties
-- in any manner, medium, or form, in whole or in part, without the prior
-- written consent of Tensilica Inc.

-- This Lua script file runs the simulation for 1000 clock periods and then changes the clock period
-- on the xtsc_core and xtsc_memory models to 7 ns.
-- To run it automatically using the lua_script_files parameter:
--   xtsc-run -i=hello_world.inc -set_xtsc_parm=enable_dynamic_clock_period=true -set_xtsc_parm=lua_script_files=change_clock_period.lua
--   ./hello_world   -enable_dynamic_clock_period=true -lua_script_files=change_clock_period.lua
--   hello_world.exe -enable_dynamic_clock_period=true -lua_script_files=change_clock_period.lua
-- To use it manually from the lua command prompt, do one of the following . . .:
--   xtsc-run -i=hello_world.inc -set_xtsc_parm=enable_dynamic_clock_period=true -lua
--   ./hello_world   -enable_dynamic_clock_period=true -lua_command_prompt=true
--   hello_world.exe -enable_dynamic_clock_period=true -lua_command_prompt=true
-- And then use dofile from the lua command prompt to run this script:
--   > dofile("change_clock_period.lua")
--   > c
-- After running look in xtsc.log for the change_clock_period calls
-- From the lua command prompt you may also want to try
--   > help
--    ...

xtsc.cmd("sc wait 1000")                                -- Wait for a 1000 clock periods
xtsc.cmd("core0 change_clock_period 7000")              -- Change the clock on core0
xtsc.cmd("core0_pif change_clock_period 7000")          -- Change the clock on core0_pif
