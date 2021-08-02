// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#environment

#define YIELD 1

#ifdef PRODUCER
-add_lua_script_file=$(XTSC_SCRIPT_FILE_PATH)/producer.lua
#elifdef CONSUMER
#define YIELD 0
#else
#error Your must do (at least) one of -define=PRODUCER=1 or -define=CONSUMER=1
#endif

#ifdef CONSUMER
-add_lua_script_file=$(XTSC_SCRIPT_FILE_PATH)/consumer.lua
#else
#define YIELD 0
#endif
-user_state=YIELD=$(YIELD)

#ifndef LOG
#define LOG 1
#endif
-user_state=LOG=$(LOG)

#ifndef LOGGING
#define LOGGING $(XTSC_SCRIPT_FILE_PATH)/TextLogger.txt
#elifeq ($(LOGGING),)
#define LOGGING $(XTSC_SCRIPT_FILE_PATH)/TextLogger.txt
#elifeq ($(LOGGING),1)
#define LOGGING $(XTSC_SCRIPT_FILE_PATH)/TextLogger.txt
#elifeq ($(LOGGING),0)
#define LOGGING off
#endif
-logging=$(LOGGING)

#ifndef TOTAL
#define TOTAL 20
#endif
-user_state=TOTAL=$(TOTAL)

#ifndef QUEUE
#define QUEUE no_core_queue
#endif
-user_state=QUEUE=$(QUEUE)

#ifndef DEPTH
#define DEPTH 10
#endif

#ifndef PRODUCER
-set_queue_parm=num_producers=0
#endif

#ifndef CONSUMER
-set_queue_parm=num_consumers=0
#endif

-set_queue_parm=host_shared_memory=true
-set_queue_parm=depth=$(DEPTH)
-set_queue_parm=bit_width=96
-create_queue=$(QUEUE)

