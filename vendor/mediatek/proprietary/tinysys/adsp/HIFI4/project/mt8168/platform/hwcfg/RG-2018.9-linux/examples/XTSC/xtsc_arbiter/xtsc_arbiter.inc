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
-core_program=target/core0.out
-create_core=core0

-set_core_parm=SimTargetOutput=core1_output.log
-core_program=target/core1.out
-create_core=core1

-create_memory=shr_mem

-set_arbiter_parm=num_masters=2
-set_arbiter_parm=translation_file=address_translations.txt
-create_arbiter=arbiter

-connect=arbiter,master_port,,shr_mem
-connect=core0,pif,slave_port[0],arbiter
-connect=core1,pif,slave_port[1],arbiter
