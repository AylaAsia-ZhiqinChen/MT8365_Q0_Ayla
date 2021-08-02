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

/*
 * Uniprocessor example described in Section 2.6.1.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "iss/mp.h"
#include "xtmp_options.h"

int 
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core;
  XTMP_connector connector;
  XTMP_memory sysRam, sysRom, dataRam;
  XTMP_address sysRamAddr, sysRomAddr, dataRamAddr;
  u32 sysRamSize, sysRomSize, dataRamSize;
  u32 pifWidth, dramWidth;
  bool bigEndian;
  xtmp_options options; 	
  xtmp_options *opt = &options;

  /* Default option values */
  init_options(opt);
  strcpy(opt->prog_name, "sieve.out");

  if (get_options(opt, argc, argv) != 0) {
    exit(1);
  }

  p = XTMP_paramsNewFromPath(xtensaSystemDirs, XTENSA_CORE, 0);
  if (!p) {
    fprintf(stderr, "Cannot create XTMP_params "XTENSA_CORE"\n");
    exit(1);
  }

  core = XTMP_coreNew("cpu", p, 0);
  if (!core) {
    fprintf(stderr, "Cannot create XTMP_core cpu\n");
    exit(1);
  }

  bigEndian = XTMP_isBigEndian(core);
  pifWidth = XTMP_pifWidth(core);

  if (pifWidth) {
    connector = XTMP_connectorNew("connector", pifWidth, 0);
    XTMP_connectToCore(core, XTMP_PT_PIF, 0, connector, 0);

    if (XTMP_getSysRamAddress(p, &sysRamAddr) && 
        XTMP_getSysRamSize(p, &sysRamSize)) {
      sysRam = XTMP_pifMemoryNew("sysRam", pifWidth, bigEndian, sysRamSize);
      XTMP_addMapEntry(connector, sysRam, sysRamAddr, sysRamAddr);
    }

    if (XTMP_getSysRomAddress(p, &sysRomAddr) && 
        XTMP_getSysRomSize(p, &sysRomSize)) {
      sysRom = XTMP_pifMemoryNew("sysRom", pifWidth, bigEndian, sysRomSize);
      XTMP_memorySetReadOnly(sysRom, true);
      XTMP_addMapEntry(connector, sysRom, sysRomAddr, sysRomAddr);
    }
  }

  if (XTMP_getLocalMemoryAddress(p, XTMP_PT_DRAM, 0, &dataRamAddr) && 
      XTMP_getLocalMemorySize(p, XTMP_PT_DRAM, 0, &dataRamSize)) {
    dramWidth = XTMP_loadStoreWidth(core);
    dataRam = XTMP_localMemoryNew("dataRam", dramWidth, bigEndian, dataRamSize);
    XTMP_connectToCore(core, XTMP_PT_DRAM, 0, dataRam, dataRamAddr);
  }

  if (!XTMP_loadProgram(core, opt->prog_name, 0)) {
    fprintf(stderr, "Cannot load %s\n", opt->prog_name);
    exit(1);
  }

  XTMP_setRelaxedSimulationCycleLimit(opt->ooo_cycles);
  if (opt->turbo) {
    if (XTMP_switchSimMode(core, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Could not switch core to functional mode\n");
    }
  }
  if (opt->trace_file[0])
    XTMP_setTraceFile(core, opt->trace_file);
  if (opt->trace_level >= 0)
    XTMP_setTraceLevel(core, opt->trace_level);
  if (opt->enable_debug) {
    opt->debug_port = XTMP_enableDebug(core, opt->debug_port);
    if (!opt->xxdebug)
      fprintf(stderr, "Waiting for debugger on port %d\n", opt->debug_port);
    XTMP_setWaitForDebugger(core, true);
  }

  XTMP_stepSystem(opt->cycle_limit);

  if (opt->summary)
    XTMP_printSummary(core, false);

  if (opt->turbo && !strcmp(opt->prog_name, "sieve.out"))
    fprintf(stderr, "\nNote: To see the full benefit of TurboXim, " 
            "run with --program=sieve_long.out\n\n");

  /* Must return to main, which does cleanup */
  return 0;
}
