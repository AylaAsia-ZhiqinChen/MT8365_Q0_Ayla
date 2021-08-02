// Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2012 Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=../TextLogger.txt
-no_default_local_memories

-core_program=target/main.out
-create_core=core0

-set_router_parm=immediate_timing=true
-set_router_parm=num_slaves=3
-set_router_parm=default_port_num=0
-set_router_parm=routing_table=core0_router.tab
-create_router=core0_router

-set_dma_parm=byte_width=8
-set_dma_parm=clear_notify_value=true
-set_dma_parm=reg_base_address=0x20001000
-create_dma=dma

-set_router_parm=immediate_timing=true
-set_router_parm=num_slaves=3
-set_router_parm=default_port_num=0
-set_router_parm=routing_table=dma_router.tab
-create_router=dma_router

-set_arbiter_parm=num_masters=2
-create_arbiter=pif_arbiter

-set_arbiter_parm=num_masters=2
-create_arbiter=mmio_arbiter

-set_mmio_parm=definition_file=mmio.def
-set_mmio_parm=swizzle_bytes=false
-create_mmio=mmio

-memory_port=pif
-set_memory_parm=initial_value_file=pifmem.dat
-create_memory=pifmem

-memory_port=dram0
-create_memory=dram0

-connect=core0,pif,slave_port,core0_router
-connect=core0_router,master_port[1],slave_port,dma
-connect=core0_router,master_port[0],slave_port[1],pif_arbiter
-connect=dma,master_port,slave_port,dma_router
-connect=dma_router,master_port[1],inbound_pif,core0
-connect=dma_router,master_port[0],slave_port[0],pif_arbiter
-connect=dma_router,master_port[2],slave_port[1],mmio_arbiter
-connect=core0_router,master_port[2],slave_port[0],mmio_arbiter
-connect=mmio_arbiter,master_port,slave_port,mmio
-connect=mmio,BInterrupt06,BInterrupt06,core0
-connect=pif_arbiter,master_port,slave_port,pifmem
-connect=core0,dram0,,dram0

