// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include "../common.inc"

-xtensa_params=$(XTSC_SCRIPT_FILE_PATH)/../../../TIE/example.tdk

-set_make_parm=testbench_name=Testbench
-set_make_parm=verilog_files=$(XTSC_SCRIPT_FILE_PATH)/Testbench.v,$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/queue.v


-set_wrapper_parm=vcd_handle=waveforms_core0
-create_wrapper=Xtensa0

-create_clock=clk0
-connect_clock_wrapper=clk0,CLK

-set_core_parm=SimTargetProgram=$(XTSC_SCRIPT_FILE_PATH)/target/producer.out
-set_core_parm=SimTargetOutput=core0_output.log
-set_core_parm=SimPinLevelInterfaces=OUTQ1
-create_core=core0

-connect=core0,OUTQ1,,Xtensa0



-set_wrapper_parm=vcd_handle=waveforms_core1
-create_wrapper=Xtensa1

-set_clock_parm=vcd_handle=waveforms_core1
-create_clock=clk1

-set_core_parm=SimTargetProgram=$(XTSC_SCRIPT_FILE_PATH)/target/consumer.out
-set_core_parm=SimTargetOutput=core1_output.log
-set_core_parm=SimPinLevelInterfaces=INQ1
-create_core=core1

-connect=Xtensa1,,INQ1,core1


