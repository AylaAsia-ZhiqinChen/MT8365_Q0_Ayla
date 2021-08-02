// Customer ID=13943; Build=0x75f5e; Copyright (c) 2014 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

/*
 * Run Instructions:
 *
 *   To run using the xtsc_lookup executable:
 *      ./xtsc_lookup -xtsc_command_prompt=true -SimMonitorInterfaces=*lut
 *      cmd: . example.cmd
 *
 *   To run using xtsc-run requires adding the following SimMonitorInterfaces line to xtsc_lookup.inc just before
 *   the -create_core line:
 *     -set_core_parm=SimMonitorInterfaces=*lut
 *     -create_core=core0
 *   Then run xtsc-run like this:
 *     xtsc-run -include=xtsc_lookup.inc -set_xtsc_parm=xtsc_command_prompt=true
 *      cmd: . example.cmd
 *   
 */

;# Source the aliases.cmd file to get convenience aliases
. aliases.cmd

;# Here are all the aliases that are defined
aliases

;# List of XTSC command handlers
?

;# Commands supported by xtsc_lookup tbl
man tbl

;# Contents of xtsc_lookup tbl
tbl dump

;# Wait for first lookup data and then dump the lut interface
w;d

;# Do it a second time
w;d

;# And do it a third time
w;d

;# Use tbl poke to change value at addresss 0xbb to 0xaaaaaaaa
tbl poke 0xbb 0xaaaaaaaa

;# Use tbl peek to confirm the change
tbl peek 0xbb

;# Now use c command to continue the simulation
c

