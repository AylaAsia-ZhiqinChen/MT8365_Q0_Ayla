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

-create_vcd=waveforms

-memory_port=pif
-set_memory_trace_parm=vcd_handle=waveforms
-set_memory_trace_parm=track_latency=true
-create_memory_trace=trace_pif
-create_memory=mem_pif
-connect=core0,pif,slave_ports,trace_pif
-connect=trace_pif,master_ports,,mem_pif

-memory_port=iram0
-set_memory_trace_parm=vcd_handle=waveforms
-set_memory_trace_parm=track_latency=true
-create_memory_trace=trace_iram0
-create_memory=mem_iram0
-connect=core0,iram0,slave_ports,trace_iram0
-connect=trace_iram0,master_ports,,mem_iram0

-memory_port=iram1
-set_memory_trace_parm=vcd_handle=waveforms
-set_memory_trace_parm=track_latency=true
-create_memory_trace=trace_iram1
-create_memory=mem_iram1
-connect=core0,iram1,slave_ports,trace_iram1
-connect=trace_iram1,master_ports,,mem_iram1

-memory_port=dram0
-set_memory_trace_parm=vcd_handle=waveforms
-set_memory_trace_parm=track_latency=true
-create_memory_trace=trace_dram0
-create_memory=mem_dram0
-connect=core0,dram0,slave_ports,trace_dram0
-connect=trace_dram0,master_ports,,mem_dram0

-memory_port=dram1
-set_memory_trace_parm=vcd_handle=waveforms
-set_memory_trace_parm=track_latency=true
-create_memory_trace=trace_dram1
-create_memory=mem_dram1
-connect=core0,dram1,slave_ports,trace_dram1
-connect=trace_dram1,master_ports,,mem_dram1

