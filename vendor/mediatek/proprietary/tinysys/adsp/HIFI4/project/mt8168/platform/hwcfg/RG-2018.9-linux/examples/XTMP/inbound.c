/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2006 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
#include "xtmp_config.h"
#include "inbound.h"

#define NEW_VALUE(x)		((x) + 0x12345678)

int main(int argc, char **argv)
{
#if XTMP_HAS_DATA_RAM && XTMP_DATA_RAM0_INBOUND
  unsigned x, y;

  /* Inbound read from data RAM. */
  printf("\nRead from PIF device:\n");
  x = *(unsigned*)(XTMP_BYPASS_VADDR);
  printf("... gets value 0x%08x.\n", x);

  /* Inbound write to data RAM. */
  printf("\nWrite to PIF device:\n");
  *(unsigned*)(XTMP_BYPASS_VADDR) = NEW_VALUE(x);
  printf("... writes value 0x%08x.\n", NEW_VALUE(x));

  printf("\nRead the location just written to verify it was updated:\n");
  y = *(unsigned*)(XTMP_BYPASS_VADDR);
  /* Check with an inbound PIF read. */
  if (y != NEW_VALUE(x)) {
    printf("Error: wrote %x but read back %x\n", NEW_VALUE(x), y), 
    exit(1);
  }
  /* Check by directly reading from data RAM. */
  y = *(unsigned*)(DRAM_TARGET_VADDR);
  if (y != NEW_VALUE(x)) {
    printf("Error: wrote %x but read back %x\n", NEW_VALUE(x), y), 
    exit(1);
  }
  
  printf("\nWrite original value back:\n");
  *(unsigned*)(XTMP_BYPASS_VADDR) = x;

  printf("\nVerify the write:\n");
  /* Check with an inbound PIF read. */
  y = *(unsigned*)(XTMP_BYPASS_VADDR);
  if (y != x) {
    printf("Error: wrote %x but read back %x\n", x, y), 
    exit(1);
  }
  /* Check by directly reading from data RAM. */
  y = *(unsigned*)(DRAM_TARGET_VADDR);
  if (y != x) {
    printf("Error: wrote %x but read back %x\n", NEW_VALUE(x), y), 
    exit(1);
  }
#endif
  return 0;
}
