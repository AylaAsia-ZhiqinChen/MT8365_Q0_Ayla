// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=../TextLogger.txt

-set_core_parm=SimTargetOutput=core0_output.log
-core_program=target/source.out
-create_core=core0

-set_core_parm=SimTargetOutput=core1_output.log
-core_program=target/sink.out
-create_core=core1

-set_wire_parm=bit_width=32
-create_wire=core0_to_core1

-connect=core0,EXPSTATE,wire_write,core0_to_core1
-connect=core0_to_core1,wire_read,IMPWIRE,core1
