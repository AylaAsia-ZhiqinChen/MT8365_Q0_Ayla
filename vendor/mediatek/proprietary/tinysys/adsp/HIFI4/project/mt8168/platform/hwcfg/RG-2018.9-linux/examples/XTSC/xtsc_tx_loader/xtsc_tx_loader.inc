// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=../TextLogger.txt
-xtensa_params=../TIE/example.tdk

-set_producer_parm=script_file=loader_driver.vec
-set_producer_parm=control=true
-create_producer=loader_driver

-create_loader=loader

-core_program=
-set_core_parm=ProcessorID=0
-create_core=core0

-core_program=
-set_core_parm=ProcessorID=1
-create_core=core1

-set_queue_parm=depth=2
-set_queue_parm=bit_width=96
-create_queue=Q01

-set_queue_parm=depth=2
-set_queue_parm=bit_width=96
-create_queue=Q10

-set_logic_parm=definition_file=logic.def
-create_logic=logic

-connect=core0,OUTQ1,queue_push,Q01
-connect=Q01,queue_pop,INQ1,core1

-connect=core1,OUTQ1,queue_push,Q10
-connect=Q10,queue_pop,INQ1,core0

-connect=loader_driver,queue_push,queue_push,loader
-connect=loader,tx_xfer_out,tx_xfer_in,core0
-connect=core0,tx_xfer_out,tx_xfer_in,core1
-connect=core1,tx_xfer_out,tx_xfer_in,loader
-connect=core0,CoreHalted,CoreHalted0,logic
-connect=core1,CoreHalted,CoreHalted1,logic
-connect=logic,JobDone,control,loader_driver
