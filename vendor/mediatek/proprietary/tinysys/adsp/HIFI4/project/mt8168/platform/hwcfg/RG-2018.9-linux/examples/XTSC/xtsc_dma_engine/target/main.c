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
#include <stdlib.h>
#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/xtruntime.h>
#include <../../include/xtsc/xtsc_dma_request.h>

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)                    { TIMESTAMP; fprintf(stdout, msg);                   fflush(stdout); }
#define LOG1(msg, arg1)             { TIMESTAMP; fprintf(stdout, msg, arg1);             fflush(stdout); }
#define LOG2(msg, arg1, arg2)       { TIMESTAMP; fprintf(stdout, msg, arg1, arg2);       fflush(stdout); }


typedef unsigned short  u16;
typedef unsigned int    u32;


#define NUM_WORDS 512
#define INT_NUMBER 6


u16 ping[NUM_WORDS] __attribute__((section(".dram0.data"), aligned(256)));
u16 pong[NUM_WORDS] __attribute__((section(".dram0.data"), aligned(256)));
u16 ramp            __attribute__((section(".dram0.data"))) = 0;
u32 sad             __attribute__((section(".dram0.data"))) = 0;
volatile
u32 dma_done        __attribute__((section(".dram0.data"))) = 0;

// The DMA programming registers
volatile xtsc_dma_request    *req  = (xtsc_dma_request*)    0x20001000;
volatile xtsc_dma_descriptor *desc = (xtsc_dma_descriptor*) 0x20001100;

// Source data physical address (not virtual)
u32 source_addr = 0x20002000;



void interrupt_handler(int num) {
  dma_done = 1;
}



// Wait until DMA completes
void wait_for_dma(void) {
  while (!dma_done) {
    ;  // Spin
  }
}



void program_dma(u32 source_address8, u32 destination_address8, u32 size8) {
  // Clear dma_done flag
  dma_done = 0;

  // Program DMA request registers (except num_descriptors)
  req->notify_address8          = 0x20000000;
  req->notify_value             = 0x00000001;
  req->turboxim_event_id        = 0x00000000;

  // Program 1 set of DMA descriptor registers
  desc->source_address8         = source_address8;
  desc->destination_address8    = destination_address8;
  desc->size8                   = size8;
  desc->num_transfers           = 0x00000010;

  // Now program num_descriptors to start DMA engine
  req->num_descriptors          = 0x00000001;
}



// Compute the sum of absolute difference between the memory data and a ramp
void crunch(u16 *buf) {
  u32 i;
  for (i=0; i<NUM_WORDS; ++i) {
    sad += abs(buf[i] - ramp);
    ramp += 1;
  }
}



int main(int argc, char *argv[]) {
  u32 i;
  u32 size8 = NUM_WORDS * sizeof(ping[0]);

  xthal_set_region_attribute((int*)0x20000000, 0x20000000, XCHAL_CA_BYPASS, 0);
  
  LOG1("Program Name: %s \n", argv[0]);

  _xtos_set_interrupt_handler_arg(INT_NUMBER, interrupt_handler, (void*)INT_NUMBER);
  _xtos_ints_on(1 << INT_NUMBER);
  
  for (i=0; i<4; i+=2) {
    program_dma(source_addr+size8*(i+0), (u32)ping, size8);
    if (i != 0) {
      crunch(pong);
    }
    wait_for_dma();
    program_dma(source_addr+size8*(i+1), (u32)pong, size8);
    crunch(ping);
    wait_for_dma();
  }
  crunch(pong);

  LOG1("Sum of absolute difference = %d \n", sad);
  return 0;
}

