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

/* This function is used to insert random delays before each write
 * to the TIE queue. You can modify it to vary the delays or remove
 * the call from main() to make the FIFO writes go faster.
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
  unsigned char c;
  unsigned int  u, j;

  srand('W');

  /* To see the TurboXim speedup, define ITERS to 100+.
   * It must be the same value as in queue_reader.c
   */
#ifndef ITERS
#define ITERS 1
#endif
  for (j = 0; j < ITERS; j++) {
    /* Each write to the 40-bit TIE queue writes a letter 'a'..'z','A'..'Z'
     * in the upper 8 bits, and the index 1..52 in the lower 32 bits.
     */
    for (u = 1, c = 'a'; u <= 52; u++) {
      delay();
      WriteFifo(c, u);
      if (++c > 'z')
        c = 'A';
    }
  }
  
  return 0;
}


      
