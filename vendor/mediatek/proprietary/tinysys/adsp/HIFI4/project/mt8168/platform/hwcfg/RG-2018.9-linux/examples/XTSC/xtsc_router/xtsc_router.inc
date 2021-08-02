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

-set_core_parm=SimTargetOutput=core0_output.log
-core_program=target/router_test.out
-create_core=core0

-set_router_parm=routing_table=routing.tab
-set_router_parm=num_slaves=2
-set_router_parm=default_port_num=1
-create_router=router

-create_memory=sysrom
-create_memory=sysram

-connect=core0,pif,slave_port,router
-connect=router,master_port[0],,sysrom
-connect=router,master_port[1],,sysram
