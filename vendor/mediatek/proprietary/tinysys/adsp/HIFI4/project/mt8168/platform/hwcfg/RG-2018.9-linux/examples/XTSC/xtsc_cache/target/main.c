// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <xtensa/config/core.h>
#include <xtensa/xtruntime.h>
#include <xtensa/hal.h>

typedef unsigned short  u16;
typedef unsigned int    u32;

#define INT_NUMBER 0
#define M_SIZE     8

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)                    { TIMESTAMP; fprintf(stdout, msg);                   fflush(stdout); }
#define LOG1(msg, arg1)             { TIMESTAMP; fprintf(stdout, msg, arg1);             fflush(stdout); }
#define LOG2(msg, arg1, arg2)       { TIMESTAMP; fprintf(stdout, msg, arg1, arg2);       fflush(stdout); }

// Interrupt flag
u32 intr_done    = 0;

// MMIO register
u32* mmio        = (u32*) 0x20000000;

void interrupt_handler(int num) {
  LOG("Enter ISR\n");
  intr_done = 1;
  LOG("Clear MMIO\n");
  *mmio = 0;
}

void wait_for_interrupt(void) {
  while (!intr_done) {
    ;  // Spin
  }
}

void write_mmio() {
  // Clear interrupt flag
  intr_done = 0;
  LOG("Set MMIO\n");
  *mmio = 1;
}


int main(void) {
  u32 A[M_SIZE][M_SIZE];
  u32 B[M_SIZE][M_SIZE];
  u32 C[M_SIZE][M_SIZE];
  u32 i, j, ii, sum;
  u32 failed = 0;
  int err = 0;

  _xtos_set_interrupt_handler_arg(INT_NUMBER, interrupt_handler, (void*)INT_NUMBER);
  _xtos_ints_on(1 << INT_NUMBER);

  /* Set bypass cache region */
  err = xthal_set_region_attribute(mmio, 16, XCHAL_CA_BYPASS, 0);
  if (err != 0) {
    LOG("This processor configuration does not support 'xthal_set_region_attribute' function. \n");
    LOG("passed! \n");
    return 0;
  }
  /* Initial values */
  for (i = 0; i < M_SIZE; i++)
    for (j = 0; j < M_SIZE; j++)
      A[i][j] = 1;

  for (i = 0; i < M_SIZE; i++)
    for (j = 0; j < M_SIZE; j++)
      B[i][j] = 1;

  for (i = 0; i < M_SIZE; i++)
    for (j = 0; j < M_SIZE; j++)
      C[i][j] = 0;

  /* C = A x B */
  for (i = 0; i < M_SIZE; i++) {
    for (j = 0; j < M_SIZE; j++) {
      sum = 0;
      for (ii = 0; ii < M_SIZE; ii++) {
        sum += (A[i][ii] * B[ii][j]);
      }
      C[i][j] = sum;
    }
  }

  /* Check results */
  for (i = 0; i < M_SIZE; i++) {
    for (j = 0; j < M_SIZE; j++) {
      if (C[i][j] != M_SIZE)
        failed = 1;
    }
  }

  // Notify MMIO
  write_mmio();

  // Wait for the interrupt
  wait_for_interrupt();

  if (failed == 0) {
    LOG("passed! \n");
  }
  else {
    LOG("failed! \n");
  }
  return 0;
}
