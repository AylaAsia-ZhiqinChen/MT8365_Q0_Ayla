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

-set_core_parm=SimTargetOutput=core0_output.log
-set_core_parm=SimPinLevelInterfaces=status
//-set_core_parm=SimVcdHandle=waveforms
-core_program=target/source.out
-create_core=core0

-set_core_parm=SimTargetOutput=core1_output.log
-set_core_parm=SimPinLevelInterfaces=control
//-set_core_parm=SimVcdHandle=waveforms
-core_program=target/sink.out
-create_core=core1

-set_signal_parm=vcd_handle=waveforms

-connect_core_signal=core0,TIE_status,core0_to_core1
-connect_signal_core=core0_to_core1,TIE_control,core1

