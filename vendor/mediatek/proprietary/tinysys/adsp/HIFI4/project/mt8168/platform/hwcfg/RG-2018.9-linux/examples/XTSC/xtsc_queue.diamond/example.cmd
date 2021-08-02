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
 *   To run using the xtsc_queue.diamond executable:
 *      ./xtsc_queue.diamond -xtsc_command_prompt=true -core0.SimMonitorInterfaces=*OPQ -core1.SimMonitorInterfaces=*IPQ
 *      cmd: . example.cmd
 *
 *   To run using xtsc-run requires adding the following SimMonitorInterfaces lines to xtsc_queue.diamond.inc just before
 *   their respective -create_core line:
 *     -set_core_parm=SimMonitorInterfaces=*OPQ
 *     -create_core=core0
 *     ...
 *     -set_core_parm=SimMonitorInterfaces=*IPQ
 *     -create_core=core1
 *   Then run xtsc-run like this:
 *     xtsc-run -include=xtsc_queue.diamond.inc -set_xtsc_parm=xtsc_command_prompt=true
 *      cmd: . example.cmd
 *   
 */

;# Source our aliases
. aliases.cmd

;# List all aliases
aliases

;# List all Q1 commands
help Q1

;# Wait until core0 pushes then dump its OPQ interface
w0;d0

;# Disassemble 2 instructions on core0 starting at its PC
core0 dasm pc 2

;# Wait until core1 pops then dump its IPQ interface
w1;d1

;# Disassemble 2 instructions on core1 starting at its PC
core1 dasm pc 2

;# Stall core1
stall1

;# Wait until Q1 fills up then dump its contents
wf;dq

;# Now resume core1 and stall core0
run1;stall0

;# Wait until Q1 empties
we;d1

;# Push a test value into Q1
Q1 push 0xcafebabe

;# Wait until core1 pops then dump its IPQ interface
w1;d1

;# Now resume core0
run0

;# Wait until both cores finish
sc wait xtsc_all_cores_exited_event

;# Dump Q1 contents just before simulation ends
dq

;# Let simulation finish
c

