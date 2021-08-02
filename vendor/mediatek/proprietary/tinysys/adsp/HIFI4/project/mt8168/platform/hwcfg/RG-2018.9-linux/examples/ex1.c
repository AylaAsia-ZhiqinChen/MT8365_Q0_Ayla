/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2003-2006 by Tensilica Inc.  ALL RIGHTS RESERVED.
 *
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any adapted
 * or modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the prior
 * written consent of Tensilica Inc.
 */

/* An example program */

#include <stdio.h>

#define NUM_ELEMENTS 100
#define NUM_ITERS    1000

/* Simple array computation */
void calc_array (int *x)
{
  int i, iters;

  for (i = 0; i < NUM_ELEMENTS; i++)
    x[i] = 1;

  for (iters = 0; iters < NUM_ITERS; iters++) 
    for (i = 1; i < NUM_ELEMENTS; i++)
      x[i] += x[i-1];
}

main()
{
  int a[NUM_ELEMENTS];
  calc_array(a);
  printf("Done computing the array\n");
}
