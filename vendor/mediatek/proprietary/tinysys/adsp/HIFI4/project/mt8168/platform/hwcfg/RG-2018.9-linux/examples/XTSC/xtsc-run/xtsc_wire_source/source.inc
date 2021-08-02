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

// Allow reset stragglers because we are not resetting the whole system, only xtsc_core
-set_core_parm=SimAllowResetStragglers=true
-set_core_parm=SimTargetOutput=core0_output.log
-core_program=idle.out
-create_core=core0

-set_source_parm=script_file=BReset.vec
-create_source=BReset

-connect=BReset,,BReset,core0

-logging=off
