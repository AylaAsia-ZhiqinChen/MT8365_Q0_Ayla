// Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=../TextLogger.txt
-set_core_parm=SimTargetOutput=core0_output.log
-core_program=target/core0.out
-create_core=core0
-create_memory=core0_pif
-set_core_parm=SimTargetOutput=core1_output.log
-core_program=target/core1.out
-create_core=core1
-create_memory=core1_pif
-set_router_parm=routing_table=routing.tab
-set_router_parm=num_slaves=2
-set_router_parm=default_port_num=0
-create_router=router
-set_mmio_parm=definition_file=mmio.txt
-set_mmio_parm=byte_width=8
-set_mmio_parm=swizzle_bytes=false
-create_mmio=mmio

-connect=core0,pif,slave_port,router
-connect=router,master_port[0],slave_port,core0_pif
-connect=router,master_port[1],slave_port,mmio

-connect=core1,EXPSTATE,EXPSTATE,mmio
-connect=mmio,BInterrupt06,BInterrupt06,core1
-connect=core1,pif,,core1_pif
