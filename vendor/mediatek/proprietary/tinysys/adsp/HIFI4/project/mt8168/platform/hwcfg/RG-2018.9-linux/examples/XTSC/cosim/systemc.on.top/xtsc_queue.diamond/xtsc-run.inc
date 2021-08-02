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

-set_core_parm=SimTargetOutput=core0_output.log
-set_core_parm=SimPinLevelInterfaces=OPQ
-set_core_parm=SimTargetProgram=$(XTSC_SCRIPT_FILE_PATH)/target/producer.out
-create_core=core0

-set_core_parm=SimTargetOutput=core1_output.log
-set_core_parm=SimPinLevelInterfaces=IPQ
-set_core_parm=SimTargetProgram=$(XTSC_SCRIPT_FILE_PATH)/target/consumer.out
-create_core=core1

-set_proxy_parm=module_name=queue
-set_proxy_parm=vcd_handle=waveforms
-set_proxy_parm=verilog_file=$(XTSC_SCRIPT_FILE_PATH)/../../verilog.sources/queue.diamond.v
-create_proxy=Q1

-connect_clock_proxy=CLK,Q1
-connect=core0,OPQ,,Q1
-connect=Q1,,IPQ,core1
