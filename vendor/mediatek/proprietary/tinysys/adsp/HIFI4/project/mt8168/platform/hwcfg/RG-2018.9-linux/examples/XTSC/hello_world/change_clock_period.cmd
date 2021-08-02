// Customer ID=13943; Build=0x75f5e; Copyright (c) 2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

// This file can be sourced from the XTSC command prompt to run the simulation for 1000 clock periods and then change the clock period
// on the xtsc_core and xtsc_memory models to 7 ns and then continue the simulation.
// To use it:
//   xtsc-run -i=hello_world.inc -set_xtsc_parm=enable_dynamic_clock_period=true -cmd
//   ./hello_world   -enable_dynamic_clock_period=true -xtsc_command_prompt=true
//   hello_world.exe -enable_dynamic_clock_period=true -xtsc_command_prompt=true
// Then source this file from the cmd: prompt using the period (.) command:
//   cmd: . change_clock_period.cmd
// After running look in xtsc.log for the change_clock_period calls
// From the XTSC command prompt you may also want to try:
//   cmd: help
//    ...

;# Wait for a 1000 clock periods
sc wait 1000

;# Change clock on core0
core0 change_clock_period 7000

;# Change clock on core0_pif
core0_pif change_clock_period 7000

;# Continue the simulation
c
