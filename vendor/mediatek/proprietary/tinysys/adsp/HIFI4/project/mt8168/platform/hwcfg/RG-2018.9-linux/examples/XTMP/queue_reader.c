/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2004-2006 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * 
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any adapted
 * or modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the prior
 * written consent of Tensilica Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <xtensa/tie/queue.h>

/* This function is used to insert random delays before each read
 * from the TIE queue. You can modify it to vary the delays or
 * remove the call from main() to make the FIFO writes go faster.
 */
static int delay()
{
  int i, s, n = rand() & 0xff;
  for (i = 0; i < n; i++)
    s += i;
  return s;
}

int main()
{
  unsigned i, j, u[52];
  unsigned char c[52];

  srand('R');

  /* To see the TurboXim speedup, define ITERS to 100+.
   * It must be the same value as in queue_writer.c
   */
#ifndef ITERS
#define ITERS 1
#endif
  for (j = 0; j < ITERS; j++) {
    /* Each read from the 40-bit TIE queue puts the upper 8 bits into
     * character array c, and the lower 32 bits into unsigned array u.
     */
    for (i = 0; i < 52; i++) {
      delay();
      u[i] = ReadFifoNumber();
      c[i] = ReadFifoLetter();
    }
  }

  /* Print the data read from the queue */
  for (i = 0; i < 52; i++) {
    printf("%2u %c\n", u[i], c[i]);
  }
}


      
