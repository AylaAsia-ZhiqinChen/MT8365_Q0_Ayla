// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=../TextLogger.txt
-no_default_mem

-create_vcd=waveforms

-set_clock_parm=vcd_handle=waveforms
-create_clock=clk

-set_core_parm=SimTargetOutput=core0_output.log
-core_program=target/hello.out
-create_core=core0

-memory_port=dram0
-create_memory=core0_dram0

-memory_port=pif
-create_memory=core0_pif

-set_master_parm=script_file=master.vec
-create_master=master

-memory_pin_port=pif
-set_tlm2pin_parm=inbound_pif=true
-set_tlm2pin_parm=write_responses=false
-set_tlm2pin_parm=vcd_handle=waveforms
-create_tlm2pin=tlm2pin

-set_pin2tlm_parm=inbound_pif=true
-set_pin2tlm_parm=vcd_handle=waveforms
-create_pin2tlm=pin2tlm

-connect=core0,dram0,,core0_dram0
-connect=core0,pif,,core0_pif
-connect=master,,slave_port,tlm2pin
-connect=tlm2pin,pif,pif,pin2tlm
-connect=pin2tlm,master_port,inbound_pif,core0

