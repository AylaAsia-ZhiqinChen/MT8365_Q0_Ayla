// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.


#ifndef N
#error You must define N between 0 and (NumCores-1)
#endif

#include "common.h"

-no_default_memories

#ifdef LOGGING
-logging=$(LOGGING)
#endif

#ifndef TURBO
#define TURBO 0
#endif

-set_xtsc_parm=turbo=$(TURBO)

-core_program=target/dsp.out
-set_core_parm=SimLogDisassembly=true
-create_core=core$(N)

-set_router_parm=num_slaves=2
-set_router_parm=default_port_num=0
-set_router_parm=routing_table=<;>1 $(SHMEM_BASE) $(SHMEM_END)
-create_router=router$(N)

-create_memory=sysmem$(N)

-set_memory_parm=start_byte_address=$(SHMEM_BASE)
-set_memory_parm=memory_byte_size=$(SHMEM_SIZE)
-set_memory_parm=page_byte_size=$(SHMEM_SIZE)
-set_memory_parm=host_shared_memory=true
-create_memory=shmem$(N)

-connect=core$(N),pif,slave_port,router$(N)
-connect=router$(N),master_port[0],,sysmem$(N)
-connect=router$(N),master_port[1],,shmem$(N)

