// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

-logging=../TextLogger.txt
-set_core_parm=SimTargetOutput=core0_output.log
-core_program=target/memory_test.out
-create_core=core0


-memory_port=pif
-create_xttlm2tlm2=tran_pif
-create_memory_tlm2=mem_pif
-connect=core0,pif,slave_ports,tran_pif
-connect=tran_pif,initiator_sockets,target_sockets,mem_pif

-memory_port=iram0
-create_xttlm2tlm2=tran_iram0
-create_memory_tlm2=mem_iram0
-connect=core0,iram0,slave_ports,tran_iram0
-connect=tran_iram0,initiator_sockets,target_sockets,mem_iram0

-memory_port=iram1
-create_xttlm2tlm2=tran_iram1
-create_memory_tlm2=mem_iram1
-connect=core0,iram1,slave_ports,tran_iram1
-connect=tran_iram1,initiator_sockets,target_sockets,mem_iram1

-memory_port=dram0
-create_xttlm2tlm2=tran_dram0
-create_memory_tlm2=mem_dram0
-connect=core0,dram0,slave_ports,tran_dram0
-connect=tran_dram0,initiator_sockets,target_sockets,mem_dram0

-memory_port=dram1
-create_xttlm2tlm2=tran_dram1
-create_memory_tlm2=mem_dram1
-connect=core0,dram1,slave_ports,tran_dram1
-connect=tran_dram1,initiator_sockets,target_sockets,mem_dram1

