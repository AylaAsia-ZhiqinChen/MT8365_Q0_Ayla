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
#include "xtmp_config.h"

#define DEVICE_ADDRESS	XTMP_BYPASS_VADDR

unsigned long *slowReg = (unsigned long *) DEVICE_ADDRESS;

main()
{
  int i;
  unsigned long pattern = 0x12345678;
 
  if (XTMP_HAS_BYPASS_REGION) {
    for (i = 0; i < 10; i++) {
      printf("Writing 0x%08x\n", pattern);
      *slowReg = pattern;
      pattern += 0x11111111;
      printf("Reading back 0x%08x\n", *slowReg);
    }
  }
}
