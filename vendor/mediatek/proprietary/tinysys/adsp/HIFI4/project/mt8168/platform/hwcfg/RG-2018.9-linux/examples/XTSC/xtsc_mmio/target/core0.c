// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#include <stdio.h>
#include <time.h>
#include <xtensa/config/core.h>

typedef unsigned int u32;

// Address of MMIO output (drives interrupt #6 of core1)
volatile u32 *p_mmio_output = (u32 *) 0x20000000;

// Address of MMIO input (driven by EXPSTATE of core1)
volatile u32 *p_mmio_input  = (u32 *) 0x20000010;

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)                    { TIMESTAMP; fprintf(stdout, msg);                   fflush(stdout); }
#define LOG1(msg, arg1)             { TIMESTAMP; fprintf(stdout, msg, arg1);             fflush(stdout); }
#define LOG2(msg, arg1, arg2)       { TIMESTAMP; fprintf(stdout, msg, arg1, arg2);       fflush(stdout); }
#define LOG3(msg, arg1, arg2, arg3) { TIMESTAMP; fprintf(stdout, msg, arg1, arg2, arg3); fflush(stdout); }

int main(int argc, char *argv[]) {
  u32 num_loops = 0;
  u32 interrupt = 0;
  u32 quit      = 0;
  u32 data      = 0;

  xthal_set_region_attribute((int*)0x20000000, 0x20000000, XCHAL_CA_BYPASS, 0);
  
  LOG1("Program Name: %s \n", argv[0]);

  while (!quit) {
    data      = *p_mmio_input;
    quit      = (data == 0xDEADBEEF);
    interrupt = (data % 0x10 == 0);
    LOG3("%s read 0x%08x %s \n", argv[0], data, (quit ? "quitting" : interrupt ? "interrupting" : ""));
    if (interrupt) {
      *p_mmio_output = 1;
    }
  } 


  return 0;
}

