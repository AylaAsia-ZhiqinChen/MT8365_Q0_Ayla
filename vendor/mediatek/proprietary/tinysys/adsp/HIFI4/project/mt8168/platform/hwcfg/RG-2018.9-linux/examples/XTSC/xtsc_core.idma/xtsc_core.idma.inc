// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

-logging=$(XTSC_SCRIPT_FILE_PATH)/../TextLogger.txt

-core_program=target/hello.out
-create_core=core0

-create_memory=sysmem

-set_arbiter_parm=num_masters=2
-create_arbiter=arbiter

-set_source_parm=script_file=$(XTSC_SCRIPT_FILE_PATH)/TrigIn_iDMA.vec
-set_source_parm=bit_width=1
-create_source=TrigIn_iDMA

-set_wire_parm=bit_width=1
-set_wire_parm=write_file=$(XTSC_SCRIPT_FILE_PATH)/TrigOut_iDMA.dat
-create_wire=TrigOut_iDMA

-connect=TrigIn_iDMA,,TrigIn_iDMA,core0
-connect=core0,TrigOut_iDMA,wire_write,TrigOut_iDMA
-connect=core0,sys,slave_ports,arbiter
-connect=arbiter,master_port,,sysmem

