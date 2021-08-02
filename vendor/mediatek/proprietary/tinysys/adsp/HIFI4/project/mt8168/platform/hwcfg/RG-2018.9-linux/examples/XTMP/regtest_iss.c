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

/*
 * Register test example that shows functionality described in Section 3.2.3.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "iss/mp.h"

static void
fail_test(int fail)
{
  printf(fail ? "FAILED\n" : "PASSED\n");
}

static void
register_check(XTMP_core core)
{
  XTMP_register reg, *regs;
  unsigned i, n;
  u32 *all_registers, nWords, totalWords;

  /* Get register count */
  n = XTMP_getRegisterCount( core );
  printf("XTMP_getRegisterCount returns %d registers\n", n);
  assert( n > 0 );

  /* Check to see that range check worked */
  reg = XTMP_getRegisterByIndex(core, n);
  printf("Range check on XTMP_getRegisterByIndex: ");
  fail_test(reg != NULL);

  reg = XTMP_getRegisterByName(core, "");
  printf("Empty string check on XTMP_getRegisterByName: ");
  fail_test(reg != NULL);

  reg = XTMP_getRegisterByName(core, NULL);
  printf("NULL string check on XTMP_getRegisterByName: ");
  fail_test(reg != NULL);

  /* Get all the registers */
  regs = (XTMP_register *)malloc(n * sizeof(XTMP_register));
  totalWords = 0;
  for (i = 0; i < n ; i++) {
    const char *name;
    unsigned width;
    regs[i] = XTMP_getRegisterByIndex(core, i);
    if (regs[i] == NULL)
      continue;
    name = XTMP_getRegisterName(regs[i]);
    width = XTMP_getRegisterBitWidth(regs[i]);
    /* printf("%3d %3d %s\n", i, width, name); */
    totalWords += (width / 32) + ((width % 32) != 0);
  }
  printf("Register words from iterating across all registers: %d\n",totalWords);
  XTMP_getAllRegisters(core, &all_registers, &nWords);
  printf("Register words returned by XTMP_getAllRegisters: %d\n", nWords);
  fail_test(totalWords != nWords);
}

int
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params params;
  XTMP_core core;

  params = XTMP_paramsNewFromPath(xtensaSystemDirs, XTENSA_CORE, 0);
  if (!params) {
    fprintf(stderr, "Cannot create XTMP_params "XTENSA_CORE"\n");
    exit(1);
  }

  core = XTMP_coreNew("cpu", params, 0);
  if (!core) {
    fprintf(stderr, "Cannot create XTMP_core cpu\n");
    exit(1);
  }

  register_check(core);

  return 0;
}
