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
-core_program=target/hello.out
-create_core=core0

-memory_port=dram0
-create_memory=dram0

-memory_port=pif
-create_memory=pif

-set_master_tlm2_parm=byte_width=8
-set_master_tlm2_parm=script_file=master_tlm2.vec
-create_master_tlm2=master_tlm2

-set_tlm22xttlm_parm=byte_width=8
-create_tlm22xttlm=tlm22xttlm

-connect=core0,dram0,,dram0
-connect=core0,pif,,pif
-connect=master_tlm2,initiator_socket,target_socket,tlm22xttlm
-connect=tlm22xttlm,master_port,inbound_pif,core0

