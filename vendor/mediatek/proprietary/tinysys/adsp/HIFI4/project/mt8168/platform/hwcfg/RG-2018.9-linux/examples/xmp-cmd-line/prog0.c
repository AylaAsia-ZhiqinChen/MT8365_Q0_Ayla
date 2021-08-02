/* $Id$ */

/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2003-2009 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
#include <string.h>
#include <xtensa/config/core.h>
#include <xtensa/tie/xt_core.h>
#include <xtensa/xmp-library.h>
#include <xtensa/system/mpsystem.h>

extern xmp_atomic_int_t counter;

int main(int argc, char * argv[])
{
  int i;
  int val = 0;
  int att = 0;

  printf("Core 0 reached main\n");
  xmp_init(XMP_NUM_CORES, 0);
  printf("Core 0 past xmp_init\n");

  for (i = 0; i < 100; i++) {
    int saved;
    do {
      saved = val;
      val = xmp_atomic_int_conditional_increment(&counter, 1, val);
      att++;
    } while (val != saved);
    val ++;
    printf("%d: %d, %d\n", i, xmp_atomic_int_value(&counter), att);
  }

  /* This test is for very low level functionality. So we can't use
     other primitives to be sure that the other cores finish.  So we
     just kinda hack something here.  */
check_again:
  for (val = 0; val < 10000; val++)
    while (xmp_atomic_int_value(&counter) != 0)
      printf("i = %d, value = %d, att = %d\n", 
	     i, xmp_atomic_int_value(&counter), att);

  if (xmp_atomic_int_value(&counter) != 0)
    goto check_again;

  printf("final i = %d, value = %d, att = %d\n", 
	 i, xmp_atomic_int_value(&counter), att);
  /* If we ever get here, we passed.  */
  printf("PASSED\n");
  return 0;
}
