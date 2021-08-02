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
#include <assert.h>
#include <xtensa/tie/export_import.h>

/* Synchronization is done with TIE queues */
static void sync()
{
  unsigned q = 1;
#pragma no_reorder
  PushQ(q);
#pragma no_reorder
  q = PopQ();
#pragma no_reorder
}

int main()
{
  unsigned expected_val0 = 123;
  unsigned expected_val1 = 456;
  unsigned val0 = 0xabc;
  unsigned val1 = 0xdef;

  WUR_STATEOUT(expected_val0);
  sync();
  val0 = ReadWire();
  sync();
  WUR_STATEOUT(expected_val1);
  sync();
  val1 = ReadWire();
  
  if (val0 != expected_val0) {
    printf("FAILED: val0 = %d. Expected %d\n", val0, expected_val0);
    return (1);
  }
  if (val1 != expected_val1) {
    printf("FAILED: val1 = %d. Expected %d\n", val1, expected_val1);
    return (1);
  }
  printf("PASSED: val0 = %d, val1 = %d\n", val0, val1);

  return 0;
}
