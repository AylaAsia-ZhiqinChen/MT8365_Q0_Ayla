// Customer ID=13943; Build=0x75f5e; Copyright (c) 2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// This file defines aliases associated with the user state example discussed
// in the xtsc-run tutorial appendix of the XTSC User's Guide (xtsc_ug.pdf).

// To use it:
//   xtsc-run -i=hello_world.inc -cmd -set_xtsc_parm=lua_script_files=pc_changed.lua
//   ./hello_world   -xtsc_command_prompt=true -lua_script_files=pc_changed.lua
//   hello_world.exe -xtsc_command_prompt=true -lua_script_files=pc_changed.lua
// Then source this file from the cmd: prompt using the period (.) command:
//   cmd: . pc_changed.aliases.cmd
//   cmd: n
//   ...

#environment

;# Use alias wpc to wait until the pc_changed_event occurs
#ifdef XTSC_USE_SYSTEMC_2_2_0
alias wpc=sc wait xtsc_simulation.pc_changed_event
#else
alias wpc=sc wait xtsc_simulation.lua_script_file_thread_0.pc_changed_event
#endif

;# Use alias cnt to show the user state pc_changed_count
alias cnt=xtsc xtsc_user_state_get core0.pc_changed_count

;# Use alias n to wait for pc_changed_event and then show pc_changed_count
alias n=wpc;cnt

;# We delay one delta-cycle to ensure pc_changed.lua has started running
sc wait 0
