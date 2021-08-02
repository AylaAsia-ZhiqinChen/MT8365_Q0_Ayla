// Copyright (c) 2006-2015 by Cadence Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Inc.


-logging=$(XTSC_SCRIPT_FILE_PATH)/../TextLogger.txt
-no_default_memory

-set_core_parm=SimTargetOutput=core0_output.log
-core_program=target/main.out
-create_core=core0

-memory_port=pif
-create_memory=system_memory

-set_xtsc_cache_parm=profile_cache=true
-set_xtsc_cache_parm=cache_byte_size=0x20000
-set_xtsc_cache_parm=byte_width=8
-set_xtsc_cache_parm=line_byte_width=128
-set_xtsc_cache_parm=num_ways=16
-set_xtsc_cache_parm=write_allocate=true
-set_xtsc_cache_parm=write_back=true
-set_xtsc_cache_parm=replacement_policy=RR
-set_xtsc_cache_parm=use_pif_attribute=true
-create_xtsc_cache=cache

-set_router_parm=immediate_timing=true
-set_router_parm=num_slaves=2
-set_router_parm=default_port_num=0
-set_router_parm=routing_table=$(XTSC_SCRIPT_FILE_PATH)/router.tab
-create_router=router

-set_mmio_parm=definition_file=$(XTSC_SCRIPT_FILE_PATH)/mmio.def
-set_mmio_parm=swizzle_bytes=false
-create_mmio=mmio

-connect=core0,pif,slave_port,cache
-connect=cache,master_port,slave_port,router
-connect=router,master_port[0],slave_port,system_memory
-connect=router,master_port[1],slave_port,mmio
-connect=core0,BInterrupt00,BInterrupt00,mmio

