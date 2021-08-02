// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

-logging=../TextLogger.txt
-set_core_parm=SimTargetOutput=core0_output.log
-core_program=target/memory_test.out
-create_core=core0

-create_vcd=waveforms

-memory_pin_port=pif
-create_tlm2pin=tlm2pin_pif
-set_memory_pin_parm=vcd_handle=waveforms
-create_memory_pin=mem_pif
-connect=core0,pif,slave_ports,tlm2pin_pif
-connect=tlm2pin_pif,pif,pif,mem_pif
-connect=tlm2pin_pif,debug_ports,debug_exports,mem_pif

-memory_pin_port=iram0
-create_tlm2pin=tlm2pin_iram0
-set_memory_pin_parm=vcd_handle=waveforms
-create_memory_pin=mem_iram0
-connect=core0,iram0,slave_ports,tlm2pin_iram0
-connect=tlm2pin_iram0,iram0,iram0,mem_iram0
-connect=tlm2pin_iram0,debug_ports,debug_exports,mem_iram0

-memory_pin_port=iram1
-create_tlm2pin=tlm2pin_iram1
-set_memory_pin_parm=vcd_handle=waveforms
-create_memory_pin=mem_iram1
-connect=core0,iram1,slave_ports,tlm2pin_iram1
-connect=tlm2pin_iram1,iram1,iram1,mem_iram1
-connect=tlm2pin_iram1,debug_ports,debug_exports,mem_iram1

-memory_pin_port=dram0
-create_tlm2pin=tlm2pin_dram0
-set_memory_pin_parm=vcd_handle=waveforms
-create_memory_pin=mem_dram0
-connect=core0,dram0,slave_ports,tlm2pin_dram0
-connect=tlm2pin_dram0,dram0,dram0,mem_dram0
-connect=tlm2pin_dram0,debug_ports,debug_exports,mem_dram0

-memory_pin_port=dram1
-create_tlm2pin=tlm2pin_dram1
-set_memory_pin_parm=vcd_handle=waveforms
-create_memory_pin=mem_dram1
-connect=core0,dram1,slave_ports,tlm2pin_dram1
-connect=tlm2pin_dram1,dram1,dram1,mem_dram1
-connect=tlm2pin_dram1,debug_ports,debug_exports,mem_dram1

