// Customer ID=13943; Build=0x75f5e; Copyright (c) 2007-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.


-core_program=main.out
-create_core=core0

-set_router_parm=num_slaves=2
-set_router_parm=routing_table=routing.tab
-set_router_parm=immediate_timing=true
-set_router_parm=default_port_num=0
-create_router=router

-memory_port=pif
-create_memory=pif

-set_mmio_parm=definition_file=mmio.txt
-set_mmio_parm=always_write=false
-set_mmio_parm=swizzle_bytes=false
-create_mmio=mmio

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt00.vec
-create_source=BInterrupt00

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt01.vec
-create_source=BInterrupt01

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt02.vec
-create_source=BInterrupt02

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt03.vec
-create_source=BInterrupt03

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt04.vec
-create_source=BInterrupt04

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt05.vec
-create_source=BInterrupt05

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt06.vec
-create_source=BInterrupt06

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt07.vec
-create_source=BInterrupt07

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt08.vec
-create_source=BInterrupt08

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt09.vec
-create_source=BInterrupt09

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt10.vec
-create_source=BInterrupt10

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt11.vec
-create_source=BInterrupt11

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt12.vec
-create_source=BInterrupt12

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt13.vec
-create_source=BInterrupt13

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt14.vec
-create_source=BInterrupt14

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt15.vec
-create_source=BInterrupt15

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt16.vec
-create_source=BInterrupt16

-set_source_parm=bit_width=1
-set_source_parm=control=true
-set_source_parm=script_file=BInterrupt17.vec
-create_source=BInterrupt17

-connect=core0,pif,slave_port,router
-connect=router,master_port[0],slave_port,pif
-connect=router,master_port[1],slave_port,mmio
-connect=mmio,BInterrupt00,control,BInterrupt00
-connect=mmio,BInterrupt01,control,BInterrupt01
-connect=mmio,BInterrupt02,control,BInterrupt02
-connect=mmio,BInterrupt03,control,BInterrupt03
-connect=mmio,BInterrupt04,control,BInterrupt04
-connect=mmio,BInterrupt05,control,BInterrupt05
-connect=mmio,BInterrupt06,control,BInterrupt06
-connect=mmio,BInterrupt07,control,BInterrupt07
-connect=mmio,BInterrupt08,control,BInterrupt08
-connect=mmio,BInterrupt09,control,BInterrupt09
-connect=mmio,BInterrupt10,control,BInterrupt10
-connect=mmio,BInterrupt11,control,BInterrupt11
-connect=mmio,BInterrupt12,control,BInterrupt12
-connect=mmio,BInterrupt13,control,BInterrupt13
-connect=mmio,BInterrupt14,control,BInterrupt14
-connect=mmio,BInterrupt15,control,BInterrupt15
-connect=mmio,BInterrupt16,control,BInterrupt16
-connect=mmio,BInterrupt17,control,BInterrupt17
-connect=BInterrupt00,,BInterrupt00,core0
-connect=BInterrupt01,,BInterrupt01,core0
-connect=BInterrupt02,,BInterrupt02,core0
-connect=BInterrupt03,,BInterrupt03,core0
-connect=BInterrupt04,,BInterrupt04,core0
-connect=BInterrupt05,,BInterrupt05,core0
-connect=BInterrupt06,,BInterrupt06,core0
-connect=BInterrupt07,,BInterrupt07,core0
-connect=BInterrupt08,,BInterrupt08,core0
-connect=BInterrupt09,,BInterrupt09,core0
-connect=BInterrupt10,,BInterrupt10,core0
-connect=BInterrupt11,,BInterrupt11,core0
-connect=BInterrupt12,,BInterrupt12,core0
-connect=BInterrupt13,,BInterrupt13,core0
-connect=BInterrupt14,,BInterrupt14,core0
-connect=BInterrupt15,,BInterrupt15,core0
-connect=BInterrupt16,,BInterrupt16,core0
-connect=BInterrupt17,,BInterrupt17,core0

