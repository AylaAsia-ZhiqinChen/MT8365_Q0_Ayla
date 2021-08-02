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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "iss/mp.h"
#include "xtmp_options.h"

int
XTMP_main(int argc, char** argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  char *tdkFiles[]= {"queue_tdk", 0};
  XTMP_params params;
  XTMP_core producer, consumer;
  XTMP_memory sysmem1, sysmem2;
  XTMP_queue fifo;
  u32 pifWidth;
  bool bigEndian;
  xtmp_options options;
  xtmp_options *opt = &options;

  init_options(opt);
  opt->summary= true;
  if (get_options(opt, argc, argv) != 0) {
    exit(1);
  }
    
  params = XTMP_paramsNewFromPath(xtensaSystemDirs, XTENSA_CORE, tdkFiles);
  if (!params) {
    fprintf(stderr, "Cannot create XTMP_params "XTENSA_CORE"\n");
    exit(1);
  }

  consumer = XTMP_coreNew("consumer", params, 0);
  producer = XTMP_coreNew("producer", params, 0);
  if (!producer || !consumer) {
    fprintf(stderr, "Cannot create XTMP_core's\n");
    exit(1);
  }

  bigEndian = XTMP_isBigEndian(producer);
  pifWidth = XTMP_pifWidth(producer);

  if (pifWidth) {
    sysmem1 = XTMP_pifMemoryNew("sysmem1", pifWidth, bigEndian, 0);
    sysmem2 = XTMP_pifMemoryNew("sysmem2", pifWidth, bigEndian, 0);
    XTMP_connectToCore(producer, XTMP_PT_PIF, 0, sysmem1, 0);
    XTMP_connectToCore(consumer, XTMP_PT_PIF, 0, sysmem2, 0);
  }

  fifo = XTMP_queueNew("fifo", 40, 4);
  XTMP_connectQueue(fifo, producer, "FIFO_OUT", consumer, "FIFO_IN");

  XTMP_setEventDriven(producer, true);
  XTMP_setEventDriven(consumer, true);

  if (!XTMP_loadProgram(producer, "queue_writer.out", 0)) {
    fprintf(stderr, "Cannot load queue_writer\n");
    exit(1);
  }
  if (!XTMP_loadProgram(consumer, "queue_reader.out", 0)) {
    fprintf(stderr, "Cannot load queue_reader\n");
    exit(1);
  }

  XTMP_setRelaxedSimulationCycleLimit(opt->ooo_cycles);
  if (opt->turbo) {
    if (XTMP_switchSimMode(producer, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Cannot switch producer to functional mode\n");
    }
    if (XTMP_switchSimMode(consumer, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Cannot switch consumer to functional mode\n");
    }
  }
  if (opt->enable_debug) {
    u32 port1 = XTMP_enableDebug(producer, opt->debug_port);
    u32 port2 = XTMP_enableDebug(consumer, opt->debug_port);
    if (!opt->xxdebug) {
      fprintf(stderr, "producer waiting for debugger on port %u\n", port1);
      fprintf(stderr, "consumer waiting for debugger on port %u\n", port2);
    }
    XTMP_setWaitForDebugger(producer, true);
    XTMP_setWaitForDebugger(consumer, true);
  }

  XTMP_stepSystem(opt->cycle_limit);
  
  if (opt->summary) {
    XTMP_printSummary(producer, false);
    XTMP_printSummary(consumer, false);
  }
  
  return 0;
}
