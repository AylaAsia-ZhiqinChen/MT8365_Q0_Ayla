// Customer ID=13943; Build=0x75f5e; Copyright (c) 2015-2017 Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

// The following need to be locations in system memory that are not used by the Xtensa program
// except for the IPC in main().
// See:
//    aliases
//    target/dsp.c

// Shared memory as seen from Xtensa
#define SHMEM_BASE      0xC0000000
#define SHMEM_END       0xC000FFFF
#define SHMEM_SIZE         0x10000

// Location in shared memory of the read and write indices
#define ADDR_RIDX       0xC000F000
#define ADDR_WIDX       0xC000F008

// Number, size, and location of the data buffers
#define BUF_CNT         4
#define BUF_SIZE        1024
#define BUF_BASE        0xC0000000

// Location of the result FIFO
#define RES_BASE        0xC000F100


// Set to 0 for full logging in the xtsc-PID.log files
// Set to 1 for logging in accordance with the LOGGING #define
#if 1
//#define LOGGING off
#define LOGGING elab_only
#endif


// Print task assignment and result in dispatch.cpp
#define PRINT_TASK_INFO

