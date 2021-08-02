// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

// Shared memory as seen from Xtensa

// Large work area
#define SHMEM_WORK_BASE     0xC0000000
#define SHMEM_WORK_END      0xC0000FFF
#define SHMEM_WORK_SIZE         0x1000
#define SHMEM_WORK_ERRORS   0xC0000400
#define SHMEM_WORK_FLAGS    0xC0000800

// Small lock area
#define SHMEM_LOCK_BASE     0xC0010000
#define SHMEM_LOCK_END      0xC00100FF
#define SHMEM_LOCK_SIZE          0x100



// Lowest numbered level-sensitive interupt
#define IntNum          0
#define BInterruptXX    BInterrupt00
