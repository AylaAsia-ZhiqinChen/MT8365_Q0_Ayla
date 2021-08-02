// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#environment

#ifndef Q
#define Q Q01
#endif

#ifndef DEPTH
#define DEPTH 10
#endif

-set_queue_parm=num_producers=0
-set_queue_parm=num_consumers=0
-set_queue_parm=depth=$(DEPTH)
-set_queue_parm=bit_width=96
-set_queue_parm=host_shared_memory=true
-create_queue=$(Q)

-print=Try "man $(Q)"
-cmd

