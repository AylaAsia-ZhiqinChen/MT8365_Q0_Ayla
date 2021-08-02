// Customer ID=13943; Build=0x75f5e; Copyright (c) 2016-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

/*
* This xtsc-run script implements the zombie system which implements the terminal
* talking to the real system (implemented in system.inc).  This zombie system is
* comprised of the two host OS shared memory queues (xtensa_rd_queue and
* xtensa_wr_queue) used to model the datapaths of the uart and a Lua script (term.lua)
* to drive them.
*/

#include "$(XTSC_SCRIPT_FILE_PATH)/common.h"

-user_state=NEW_LINE_CHAR=$(NEW_LINE_CHAR)
-add_lua_script_file=term.lua

-set_queue_parm=host_shared_memory=true
-set_queue_parm=bit_width=8
-set_queue_parm=depth=$(QUEUE_DEPTH)
-set_queue_parm=num_consumers=1
-set_queue_parm=num_producers=0
-create_queue=xtensa_wr_queue

-set_queue_parm=host_shared_memory=true
-set_queue_parm=bit_width=8
-set_queue_parm=depth=$(QUEUE_DEPTH)
-set_queue_parm=num_consumers=0
-set_queue_parm=num_producers=1
-create_queue=xtensa_rd_queue

