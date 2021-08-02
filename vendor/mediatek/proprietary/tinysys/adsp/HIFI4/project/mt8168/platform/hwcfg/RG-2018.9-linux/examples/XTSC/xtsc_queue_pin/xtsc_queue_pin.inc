// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=../TextLogger.txt
-xtensa_params=../TIE/example.tdk

-create_vcd=waveforms

-set_clock_parm=vcd_handle=waveforms
-create_clock=clk

-set_core_parm=SimTargetOutput=core0_output.log
-set_core_parm=SimPinLevelInterfaces=OUTQ1
//-set_core_parm=SimVcdHandle=waveforms
-core_program=target/producer.out
-create_core=core0

-set_core_parm=SimTargetOutput=core1_output.log
-set_core_parm=SimPinLevelInterfaces=INQ1
//-set_core_parm=SimVcdHandle=waveforms
-core_program=target/consumer.out
-create_core=core1

-set_queue_pin_parm=depth=2
-set_queue_pin_parm=vcd_handle=waveforms
-set_queue_pin_parm=bit_width=96
-create_queue_pin=Q1

-connect=core0,OUTQ1,queue_push,Q1
-connect=Q1,queue_pop,INQ1,core1

