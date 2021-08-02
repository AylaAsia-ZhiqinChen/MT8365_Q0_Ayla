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
 * TIE port example: demonstrates the use of
 * an export state and an import wire.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "iss/mp.h"
#include "xtmp_options.h"


typedef struct {
  u32 value;
} TiePortData;


static void
stateExport(XTMP_tiePort port, XTMP_core core, void *tiePortData, u32 *data)
{
  TiePortData *d = (TiePortData *) tiePortData;
  assert(XTMP_getTiePortBitWidth(port) == 32);
  assert(XTMP_getTiePortDirection(port) == XTMP_TIE_PORT_OUT);
  d->value = *data;
#if DEBUG
  printf("Updated value of %s to %u at "PCT_LLU"\n",
         XTMP_getTiePortName(port), d->value, XTMP_clockTime());
#endif
}

static void
importWire(XTMP_tiePort port, XTMP_core core, void *tiePortData, u32 *data)
{
  TiePortData *d = (TiePortData *) tiePortData;
  assert(XTMP_getTiePortBitWidth(port) == 32);
  assert(XTMP_getTiePortDirection(port) == XTMP_TIE_PORT_IN);
  *data = d->value;
#if DEBUG
  printf("Read value %u from %s at "PCT_LLU"\n",
         d->value, XTMP_getTiePortName(port), XTMP_clockTime());
#endif
}

int
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  char *xtensaParamsDirs[] = { "export_import_tdk", 0 };
  XTMP_params p;
  XTMP_core core1, core2;
  XTMP_memory sysmem1, sysmem2;
  XTMP_queue q1, q2;
  u32 pifWidth;
  bool bigEndian;
  xtmp_options options; 	
  xtmp_options *opt = &options;
  TiePortData extdev1 = { 0 };
  TiePortData extdev2 = { 0 };

  /* Default option values */
  init_options(opt);

  if (get_options(opt, argc, argv) != 0) {
    exit(1);
  }

  p = XTMP_paramsNewFromPath(xtensaSystemDirs, XTENSA_CORE, xtensaParamsDirs);
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

  if (!XTMP_connectToTiePort(core1, &extdev1, "STATEOUT", stateExport)) {
    fprintf(stderr, "Could not hook up extdev1 to STATEOUT of cpu1\n");
    exit(1);
  }
  if (!XTMP_connectToTiePort(core2, &extdev1, "WIREIN", importWire)) {
    fprintf(stderr, "Could not hook up extdev1 to WIREIN of cpu2\n");
    exit(1);
  }
  if (!XTMP_connectToTiePort(core2, &extdev2, "STATEOUT", stateExport)) {
    fprintf(stderr, "Could not hook up extdev2 to STATEOUT of cpu2\n");
    exit(1);
  }
  if (!XTMP_connectToTiePort(core1, &extdev2, "WIREIN", importWire)) {
    fprintf(stderr, "Could not hook up extdev2 to WIREIN of cpu1\n");
    exit(1);
  }

  q1 = XTMP_queueNew("q1", 32, 1);
  q2 = XTMP_queueNew("q2", 32, 1);
  if (!XTMP_connectQueue(q1, core1, "Q_OUT", core2, "Q_IN")) {
    fprintf(stderr, "Could not connect core1 out-queue to core2 in-queue\n");
    exit(1);
  }
  if (!XTMP_connectQueue(q2, core2, "Q_OUT", core1, "Q_IN")) {
    fprintf(stderr, "Could not connect core2 out-queue to core1 in-queue\n");
    exit(1);
  }

  bigEndian = XTMP_isBigEndian(core1);
  pifWidth = XTMP_pifWidth(core1);

  if (pifWidth) {
    sysmem1 = XTMP_pifMemoryNew("ram1", pifWidth, bigEndian, 0);
    sysmem2 = XTMP_pifMemoryNew("ram2", pifWidth, bigEndian, 0);
    XTMP_connectToCore(core1, XTMP_PT_PIF, 0, sysmem1, 0);
    XTMP_connectToCore(core2, XTMP_PT_PIF, 0, sysmem2, 0);
  }

  if (!XTMP_loadProgram(core1, "export_import.out", NULL)) {
    fprintf(stderr, "Cannot load export_import.out on core1\n");
    exit(1);
  }
  if (!XTMP_loadProgram(core2, "export_import.out", NULL)) {
    fprintf(stderr, "Cannot load export_import.out on core2\n");
    exit(1);
  }

  XTMP_setRelaxedSimulationCycleLimit(opt->ooo_cycles);
  if (opt->turbo) {
    if (XTMP_switchSimMode(core1, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Cannot switch core1 to functional mode\n");
    }
    if (XTMP_switchSimMode(core2, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Cannot switch core2 to functional mode\n");
    }
  }

  XTMP_stepSystem(opt->cycle_limit);

  if (opt->summary) {
    XTMP_printSummary(core1, false);
    XTMP_printSummary(core2, false);
  }

  /* Must return to main, which does cleanup */
  return 0;
}
