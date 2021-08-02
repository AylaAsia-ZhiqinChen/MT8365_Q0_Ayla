// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

/**
 * This xtsc-run script implements the real system which includes an xtsc_core (core0)
 * and a uart model whose datapath is comprised of two host OS shared memory queues
 * (xtensa_rd_queue and xtensa_wr_queue) that are driven by a zombie system (term.inc)
 * in a different host OS process that implements the terminal talking to core0 in this
 * system.
 */

#include "$(XTSC_SCRIPT_FILE_PATH)/common.h"

-logging=$(XTSC_SCRIPT_FILE_PATH)/TextLogger.txt

#ifndef TURBO
#elifeq ($(TURBO),0)
#elifeq ($(TURBO),false)
#else
-set_xtsc_parm=turbo=true
#endif

-user_state=BLOCK=0
#ifndef BLOCK
#elifeq ($(BLOCK),0)
#elifeq ($(BLOCK),false)
#else
-user_state=BLOCK=1
#endif

-user_state=ADDR_UART_WR=$(ADDR_UART_WR)
-add_lua_script_file=$(XTSC_SCRIPT_FILE_PATH)/uart_wr.lua

-set_source_parm=bit_width=1
-set_source_parm=script_file=$(XTSC_SCRIPT_FILE_PATH_ESC)/BInterruptXX.vec
-create_source=$(BInterruptXX)

-no_default_memories
-core_program=$(XTSC_SCRIPT_FILE_PATH)/target/dsp.out
-create_core=core0

-set_router_parm=routing_table=<;>1 $(ADDR_UART_BEG) $(ADDR_UART_END)
-set_router_parm=num_slaves=2
-set_router_parm=default_port_num=0
-create_router=router

-memory_port=pif
-create_memory=sysmem

-set_memory_parm=script_file=$(XTSC_SCRIPT_FILE_PATH)/uart_rd.vec
-set_memory_parm=use_fast_access=false
-create_memory=uartregs

-set_queue_parm=host_shared_memory=true
-set_queue_parm=bit_width=8
-set_queue_parm=depth=$(QUEUE_DEPTH)
-set_queue_parm=num_producers=1
-set_queue_parm=num_consumers=0
-create_queue=xtensa_wr_queue

-set_queue_parm=host_shared_memory=true
-set_queue_parm=bit_width=8
-set_queue_parm=depth=$(QUEUE_DEPTH)
-set_queue_parm=num_producers=0
-set_queue_parm=num_consumers=1
-create_queue=xtensa_rd_queue

-connect=core0,pif,slave_port,router
-connect=router,master_port[0],,sysmem
-connect=router,master_port[1],,uartregs
-connect=$(BInterruptXX),m_write,$(BInterruptXX),core0
