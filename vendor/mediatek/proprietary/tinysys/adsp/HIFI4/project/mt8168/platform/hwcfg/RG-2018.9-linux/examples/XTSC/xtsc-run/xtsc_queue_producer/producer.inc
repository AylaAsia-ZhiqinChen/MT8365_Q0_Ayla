// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 Tensilica Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=../../TextLogger.txt
-xtensa-params=../../TIE/example.tdk

-set_producer_parm=script_file=producer.vec
-set_producer_parm=bit_width=16
-create_producer=producer

-set_queue_parm=bit_width=16
-create_queue=queue

-set_core_parm=SimTargetOutput=core0_output.log
-core_program=consumer.out
-create_core=core0

-connect=producer,queue_push,queue_push,queue
-connect=queue,queue_pop,INQ2,core0
