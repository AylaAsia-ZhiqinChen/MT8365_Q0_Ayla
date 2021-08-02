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
-set_core_parm=SimTargetOutput=core0_output.log
-core_program=target/ram_test.out
-create_core=core0

-set_lookup_parm=ram=true
-set_lookup_parm=address_bit_width=41
-set_lookup_parm=data_bit_width=32
-set_lookup_parm=has_ready=false
-set_lookup_parm=default_data=0xBAD1BAD1
-create_lookup=lram

-connect=core0,lookup_ram,,lram
