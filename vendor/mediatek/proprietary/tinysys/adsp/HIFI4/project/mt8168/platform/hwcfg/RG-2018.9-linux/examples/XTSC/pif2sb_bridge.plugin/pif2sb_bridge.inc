// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=$(XTSC_SCRIPT_FILE_PATH)/../TextLogger.txt

-load_library=$(XTSC_SCRIPT_FILE_PATH)/pif2sb_bridge.plugin

-no_default_memories

-core_program=target/hello.out
-create_core=core0

-set_master_parm=script_file=$(XTSC_SCRIPT_FILE_PATH)/traffic.vec
-create_master=traffic_master

-set_pif2sb_parm=pif_byte_width=8
-set_pif2sb_parm=unique_priority=0
-create_pif2sb=bridge

-set_pif2sb_parm=pif_byte_width=8
-set_pif2sb_parm=unique_priority=1
-create_pif2sb=traffic_bridge

-create_sb=bus

-create_sb_arbiter=arbiter

-set_sb_slow_mem_parm=start_address=0x40000000
-set_sb_slow_mem_parm=end_address=0x4003FFFF
-set_sb_slow_mem_parm=nr_wait_states=2
-create_sb_slow_mem=sysrom

-set_sb_fast_mem_parm=start_address=0x40020000
-set_sb_fast_mem_parm=end_address=0xC001FFFF
-create_sb_fast_mem=sysram

-create_clock=clk

-connect=core0,pif,pif,bridge
-connect=traffic_master,master_port,pif,traffic_bridge
-connect=bridge,direct,direct,bus
-connect=bridge,blocking,blocking,bus
-connect=traffic_bridge,direct,direct,bus
-connect=traffic_bridge,blocking,blocking,bus
-connect=bus,arbiter,arbiter,arbiter
-connect=bus,slave,slave,sysrom
-connect=bus,slave,slave,sysram
-connect_clock=clk,clock,bus
-connect_clock=clk,clock,sysrom
