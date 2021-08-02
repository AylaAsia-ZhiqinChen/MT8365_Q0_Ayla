/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2004-2009 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
 * Multiprocessor example described in Section 2.6.2.
 */

#include <stdlib.h>
#include <stdio.h>
#include "iss/mp.h"
#include "xtmp_config.h"
#include "xtmp_options.h"


int
XTMP_main(int argc, char **argv)
{
#if XTMP_HAS_BYPASS_REGION

  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core1, core2;
  XTMP_connector router1, router2;
  XTMP_memory ram1, ram2, rom1, rom2;
  XTMP_memory sharedMem;
  u32 pifWidth;
  bool bigEndian;
  xtmp_options options; 	
  xtmp_options *opt = &options;

  /* Default option values */
  init_options(opt);

  if (get_options(opt, argc, argv) != 0) {
    exit(1);
  }

  p = XTMP_paramsNewFromPath(xtensaSystemDirs, XTENSA_CORE, 0);
  if (!p) {
    fprintf(stderr, "Cannot create XTMP_params "XTENSA_CORE"\n");
    exit(1);
  }

  core1 = XTMP_coreNew("cpu1", p, 0);
  if (!core1) {
    fprintf(stderr, "Cannot create XTMP_core cpu1\n");
    exit(1);
  }
    
  core2 = XTMP_coreNew("cpu2", p, 0);
  if (!core2) {
    fprintf(stderr, "Cannot create XTMP_core cpu2\n");
    exit(1);
  }

  pifWidth = XTMP_pifWidth(core1);
  bigEndian = XTMP_isBigEndian(core1);

  if (XTMP_HAS_SYS_RAM) {
    ram1 = XTMP_pifMemoryNew("ram1", pifWidth, bigEndian, XTMP_SYS_RAM_SIZE);
    ram2 = XTMP_pifMemoryNew("ram2", pifWidth, bigEndian, XTMP_SYS_RAM_SIZE);
  }
  if (XTMP_HAS_SYS_ROM) {
    rom1 = XTMP_pifMemoryNew("rom1", pifWidth, bigEndian, XTMP_SYS_ROM_SIZE);
    XTMP_memorySetReadOnly(rom1, true);
    rom2 = XTMP_pifMemoryNew("rom2", pifWidth, bigEndian, XTMP_SYS_ROM_SIZE);
    XTMP_memorySetReadOnly(rom2, true);
  }
  
  router1 = XTMP_connectorNew("router1", pifWidth, 0);
  router2 = XTMP_connectorNew("router2", pifWidth, 0);

  XTMP_connectToCore(core1, XTMP_PT_PIF, 0, router1, 0);
  XTMP_connectToCore(core2, XTMP_PT_PIF, 0, router2, 0);

  /* Create 64K of shared memory */
  sharedMem = XTMP_pifMemoryNew("shmem", pifWidth, bigEndian, 0x10000);

  if (XTMP_HAS_SYS_RAM) {
    XTMP_addMapEntry(router1, ram1, XTMP_SYS_RAM_PADDR, XTMP_SYS_RAM_PADDR);
    XTMP_addMapEntry(router2, ram2, XTMP_SYS_RAM_PADDR, XTMP_SYS_RAM_PADDR);
  }
  if (XTMP_HAS_SYS_ROM) {
    XTMP_addMapEntry(router1, rom1, XTMP_SYS_ROM_PADDR, XTMP_SYS_ROM_PADDR);
    XTMP_addMapEntry(router2, rom2, XTMP_SYS_ROM_PADDR, XTMP_SYS_ROM_PADDR);
  }
  XTMP_addMapEntry(router1, sharedMem, XTMP_BYPASS_PADDR, 0);
  XTMP_addMapEntry(router2, sharedMem, XTMP_BYPASS_PADDR, 0);

  if (!XTMP_loadProgram(core1, "producer.out", NULL)) {
    fprintf(stderr, "Cannot load producer.out\n");
    exit(1);
  }
  if (!XTMP_loadProgram(core2, "consumer.out", NULL)) {
    fprintf(stderr, "Cannot load consumer.out\n");
    exit(1);
  }

  XTMP_setRelaxedSimulationCycleLimit(opt->ooo_cycles);
  if (opt->turbo) {
    if (XTMP_switchSimMode(core1, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Could not switch cpu1 to functional mode\n");
    }
    if (XTMP_switchSimMode(core2, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Could not switch cpu2 to functional mode\n");
    }
  }

  if (opt->enable_debug) {
    u32 port1 = XTMP_enableDebug(core1, 0);
    u32 port2 = XTMP_enableDebug(core2, 0);
    if (!opt->xxdebug) {
      fprintf(stderr, "Waiting for debugger for core1 on port %d\n", port1);
      fprintf(stderr, "Waiting for debugger for core2 on port %d\n", port2);
    }
    XTMP_setWaitForDebugger(core1, true);
    XTMP_setWaitForDebugger(core2, true);
  }

  XTMP_stepSystem(opt->cycle_limit);

  if (opt->summary) {
    XTMP_printSummary(core1, false);
    XTMP_printSummary(core2, false);
  }

#else
  fprintf(stderr, "This example will not work because "
                  "bypass memory region was not found\n");
#endif 

  /* Must return to main, which does cleanup */
  return 0;
}
