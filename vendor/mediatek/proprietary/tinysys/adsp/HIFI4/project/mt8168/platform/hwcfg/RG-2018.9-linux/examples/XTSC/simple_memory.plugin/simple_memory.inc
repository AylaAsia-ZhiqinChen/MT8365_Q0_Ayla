// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=$(XTSC_SCRIPT_FILE_PATH)/../TextLogger.txt

-load_library=$(XTSC_SCRIPT_FILE_PATH)/simple_memory.plugin

-no_default_memories

-core_program=target/hello.out
-create_core=core0

-set_simple_memory_parm=byte_width=8
-set_simple_memory_parm=start_byte_address=0x40000000
-set_simple_memory_parm=memory_byte_size=0x00040000
-create_simple_memory=rom

-set_simple_memory_parm=byte_width=8
-set_simple_memory_parm=start_byte_address=0x40020000
-set_simple_memory_parm=memory_byte_size=0x80000000
-create_simple_memory=ram

-set_router_parm=routing_table=$(XTSC_SCRIPT_FILE_PATH)/routing.tab
-set_router_parm=num_slaves=2
-set_router_parm=default_port_num=1
-create_router=router

-connect=core0,pif,slave_port,router
-connect=router,master_port[0],slave_port,rom
-connect=router,master_port[1],slave_port,ram

