// Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2012 Tensilica Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

#include "../common.inc"

-no_default_local_mem

-set_make_parm=testbench_name=Testbench
-set_make_parm=verilog_files=$(XTSC_SCRIPT_FILE_PATH)/Testbench.v


-set_wrapper_parm=vcd_handle=waveforms
-create_wrapper=Xtensa0

-create_clock=clk
-connect_clock_wrapper=clk,CLK

-set_core_parm=SimTargetOutput=core0_output.log
-core_program=$(XTSC_SCRIPT_FILE_PATH)/target/core0.out
-create_core=core0
-create_memory=core0_pif

-set_router_parm=routing_table=$(XTSC_SCRIPT_FILE_PATH)/routing.tab
-set_router_parm=num_slaves=2
-set_router_parm=default_port_num=0
-create_router=router
-set_mmio_parm=definition_file=$(XTSC_SCRIPT_FILE_PATH)/mmio.txt
-set_mmio_parm=byte_width=8
-set_mmio_parm=swizzle_bytes=false
-create_mmio=mmio

-connect=core0,pif,slave_port,router
-connect=router,master_port[0],,core0_pif
-connect=router,master_port[1],,mmio
-connect=mmio,BInterrupt06,,Xtensa0
-connect=Xtensa0,TIE_EXPSTATE,EXPSTATE,mmio


-create_wrapper=Xtensa1

-set_core_parm=SimTargetOutput=core1_output.log
-set_core_parm=SimPinLevelInterfaces=EXPSTATE,BInterrupt06
-core_program=$(XTSC_SCRIPT_FILE_PATH)/target/core1.out
-create_core=core1
-create_memory=core1_pif

-connect=core1,EXPSTATE,,Xtensa1
-connect=Xtensa1,,BInterrupt06,core1
-connect=core1,pif,slave_port,core1_pif

