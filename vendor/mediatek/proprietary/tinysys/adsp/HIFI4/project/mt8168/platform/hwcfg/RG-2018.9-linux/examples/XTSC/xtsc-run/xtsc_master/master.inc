// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2012 Tensilica Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Tensilica Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Tensilica Inc.

-logging=TextLogger.txt
-squelch_ctor=verbose

-set_master_parm=script_file=request.vec
-create_master=master

-set_memory_parm=byte_width=4
-set_memory_parm=start_byte_address=0
-set_memory_parm=memory_byte_size=0
-set_memory_parm=num_ports=1
-create_memory=mem

-connect=master,,,mem
