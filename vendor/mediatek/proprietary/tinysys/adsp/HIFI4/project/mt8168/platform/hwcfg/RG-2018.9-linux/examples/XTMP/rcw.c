/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2009 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
#include <xtensa/config/core.h>
#include <xtensa/tie/xt_sync.h>
#include "xtmp_config.h"

#define SCOMPARE_VAL	0x12345678
#define ORIGINAL_VAL	0x11223344
#define VAL_TO_WRITE	0x55667788

int
main(int argc, char **argv)
{
#if XTMP_HAS_BYPASS_REGION
  unsigned *addr, val;

  /* Initialize the SCOMPARE1 register. */
  XT_WSR_SCOMPARE1(SCOMPARE_VAL);

  /*
   * Attempt an RCW operation to a memory location containing a value 
   * that does not match the value in the SCOMPARE1 register.  The target 
   * location should not get updated, but the value returned by the RCW 
   * should be the original value. 
   */
  addr = (unsigned *)XTMP_BYPASS_VADDR;
  *addr = ORIGINAL_VAL;
  val = VAL_TO_WRITE;
  XT_S32C1I(val, addr, 0);
  if (val != ORIGINAL_VAL) {
    printf("RCW operation returned incorrect value 0x%x\n", val);
    return 1;
  }
  val = *addr;
  if (val != ORIGINAL_VAL) {
    printf("Target address 0x%x is 0x%x after RCW, should be 0x%x.\n",
	   addr, val, ORIGINAL_VAL);
    return 1;
  }

  /*
   * This time, attempt an RCW operation to a memory location containing 
   * a value that does match the value in the SCOMPARE1 register.  The 
   * target location should now get updated.  Also, the value returned by 
   * the RCW should be the original value, which happens to be the one 
   * stored in SCOMPARE1. 
   */
  addr = (unsigned *)(XTMP_BYPASS_VADDR + 0x10);
  *addr = SCOMPARE_VAL;
  val = VAL_TO_WRITE;
  XT_S32C1I(val, addr, 0);
  if (val != SCOMPARE_VAL) {
    printf("RCW operation returned incorrect value 0x%x\n", val);
    return 1;
  }
  val = *addr;
  if (val != VAL_TO_WRITE) {
    printf("Target address 0x%x is 0x%x after RCW, should be 0x%x.\n",
	   addr, val, VAL_TO_WRITE);
    return 1;
  }
#endif  
  return 0;
}
