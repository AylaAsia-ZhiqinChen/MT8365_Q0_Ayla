// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include "../common.inc"

-set_make_parm=testbench_name=Testbench
-set_make_parm=verilog_files=$(XTSC_SCRIPT_FILE_PATH)/Testbench.v

-set_wrapper_parm=vcd_handle=waveforms
-create_wrapper=Xtensa0

-create_clock=clk

-connect_clock_wrapper=clk,CLK

-set_core_parm=SimTargetProgram=$(XTSC_SCRIPT_FILE_PATH)/target/hello.out
-set_core_parm=SimPinLevelInterfaces=BReset
-create_core=core0

-connect=Xtensa0,,BReset,core0


