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
#include <xtensa/hal.h>
#include "xtensa/config/core.h"
#include "../common.h"

typedef unsigned int u32;

volatile u32 *p_ridx = (volatile u32 *)ADDR_RIDX;       // Pointer to Read  InDeX
volatile u32 *p_widx = (volatile u32 *)ADDR_WIDX;       // Pointer to Write InDeX


u32 wait_for_init() {
  xthal_set_region_attribute((int*)SHMEM_BASE, 0x20000000, XCHAL_CA_BYPASS, 0);
  if (*p_ridx == 0) {
    while (*p_widx == 0);
  }
  return 0;
}


int main(int argc, char *argv[]) {
  u32 dummy       = wait_for_init();
  u32 index       = *p_ridx;
  u32 buf_words   = BUF_SIZE / 4;
  u32 result      = 0;
  u32 *p_buffer   = 0;
  u32 *p_result   = 0;
  u32  i          = 0;

  while (1) {

    while (*p_widx == index);                           // Wait for something to do
    if (*p_widx >= BUF_CNT) break;                      // Host has signal us to quit
    p_buffer = (u32 *)(BUF_BASE + index * BUF_SIZE);    // Get pointer to our buffer
    p_result = (u32 *)(RES_BASE + index * 4);           // Get pointer to our result

    // Crunch
    result = 0;
    for (i=0; i<buf_words-1; i+=2) {
      result |= (p_buffer[i+0] & 0xFFFF0000);
      result |= (p_buffer[i+1] & 0xFFFF0000);
      result += ((p_buffer[i+1] & 0xFFFF) - (p_buffer[i] & 0xFFFF));
    }

    *p_result = result;                                 // Write result
    index = (index + 1) % BUF_CNT;                      // Compute next buffer index
    *p_ridx = index;                                    // Signal host we're done with previous buffer
  }

  return 0;
}

