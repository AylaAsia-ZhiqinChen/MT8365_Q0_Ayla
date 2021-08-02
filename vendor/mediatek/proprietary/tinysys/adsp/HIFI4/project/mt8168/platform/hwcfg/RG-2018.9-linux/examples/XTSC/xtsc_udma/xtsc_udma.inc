// Copyright (c) 2006-2014 by Cadence Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Inc.



-logging=$(XTSC_SCRIPT_FILE_PATH)/../TextLogger.txt

-ndlm

-set_core_parm=SimTargetOutput=core0_output.log
-core_program=target/main.out
-create_core=core0

-memory_port=iram0
-create_memory

-memory_port=dram0
-create_memory

-memory_port=dram1
-create_memory

-memory_port=pif
-set_memory_parm=request_fifo_depth=16
-set_memory_parm=block_write_delay=10
-set_memory_parm=block_read_delay=10
-create_memory=system_ram

-set_arbiter_parm=num_masters=2
-set_arbiter_parm=response_delay=0
-set_arbiter_parm=arbitration_policy=0,0,0;1,1,0
-set_arbiter_parm=request_fifo_depth=1
-create_arbiter=arbiter_pif

-set_arbiter_parm=num_masters=2
-set_arbiter_parm=response_delay=0
-set_arbiter_parm=arbitration_policy=0,0,0;1,1,0
-create_arbiter=arbiter_dram0p0

-set_arbiter_parm=num_masters=2
-set_arbiter_parm=response_delay=0
-set_arbiter_parm=arbitration_policy=0,0,0;1,1,0
-create_arbiter=arbiter_dram0p1

-set_arbiter_parm=num_masters=2
-set_arbiter_parm=response_delay=0
-set_arbiter_parm=arbitration_policy=0,0,0;1,1,0
-create_arbiter=arbiter_dram1p0

-set_arbiter_parm=num_masters=2
-set_arbiter_parm=response_delay=0
-set_arbiter_parm=arbitration_policy=0,0,0;1,1,0
-create_arbiter=arbiter_dram1p1

-set_router_parm=num_slaves=4
-set_router_parm=address_routing_bits=18,18,4,4
-set_router_parm=master_byte_width=8
-set_router_parm=immediate_timing=true
-create_router=router_udma

-set_xtsc_udma_parm=ram_byte_width=8
-set_xtsc_udma_parm=ram_start_addresses=0x1E000000,0x1E040000
-set_xtsc_udma_parm=ram_byte_sizes=0x00040000,0x00040000
-set_xtsc_udma_parm=pif_byte_width=8
-create_xtsc_udma=udma

-connect=udma,pif_master_port,slave_port[0],arbiter_pif
-connect=udma,ram_master_port,slave_port,router_udma
-connect=core0,dram0p0,slave_port[1],arbiter_dram0p0
-connect=core0,dram0p1,slave_port[1],arbiter_dram0p1
-connect=core0,dram1p0,slave_port[1],arbiter_dram1p0
-connect=core0,dram1p1,slave_port[1],arbiter_dram1p1
-connect=core0,pif,slave_port[1],arbiter_pif
-connect=core0,iram0,,iram0
-connect=core0,IVP_RER,rer_export,udma
-connect=core0,IVP_WER,wer_export,udma
-connect=arbiter_pif,master_port,slave_port,system_ram
-connect=udma,sync_intr_port,BInterrupt15,core0
-connect=udma,error_intr_port,BInterrupt12,core0
-connect=router_udma,master_port[0],slave_port[0],arbiter_dram0p0
-connect=router_udma,master_port[1],slave_port[0],arbiter_dram0p1
-connect=router_udma,master_port[2],slave_port[0],arbiter_dram1p0
-connect=router_udma,master_port[3],slave_port[0],arbiter_dram1p1
-connect=arbiter_dram0p0,master_port,slave_port[0],dram0
-connect=arbiter_dram0p1,master_port,slave_port[1],dram0
-connect=arbiter_dram1p0,master_port,slave_port[0],dram1
-connect=arbiter_dram1p1,master_port,slave_port[1],dram1
