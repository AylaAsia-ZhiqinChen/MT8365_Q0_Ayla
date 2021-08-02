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

int main()
{
  int i;
  int val = 0;
  int att = 0;

  xmp_init(XMP_NUM_CORES, 0);
  for (i = 0; i < 100; i++) {
    int saved;
    do {
      saved = val;
      val = xmp_atomic_int_conditional_increment(&counter, 1, val);
      att++;
      fprintf(stderr, "@");
    } while (val != saved);
    val ++;
  }
  fprintf(stderr, "Prog 2 Done\n");
  return 0;
}
