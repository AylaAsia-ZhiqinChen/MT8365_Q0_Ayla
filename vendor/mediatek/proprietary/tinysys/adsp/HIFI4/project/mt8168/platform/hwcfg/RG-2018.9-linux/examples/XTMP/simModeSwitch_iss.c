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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "iss/mp.h"
#include "xtmp_options.h"

/*
 * This function performs periodic simulation mode switching
 * for the specified simulation components. simMode indicates
 * the starting simulation mode, and sampleCycles is the
 * number of cycles to run in the cycle-accurate mode; 
 * in the TurboXim mode, 100 * sampleCycles will be run.
 *
 * A sampled profile is collected, by enabling profiling in the
 * cycle-accurate mode and disabling it in the TurboXim mode.
 *
 * In this example, all the components are XTMP_core's, but
 * the same mechanism can be used for custom XTMP_device's.
 */
static void
periodicSwitching(XTMP_component component[], int numComponents,
                  XTMP_simMode simMode, int sampleCycles)
{
  int i, numCycles;
  const char *client_cmd;

  /* Load the profile client in disabled mode */
  for (i = 0; i < numComponents; i++) {
    XTMP_loadClient(component[i], "profile --disable");
  }

  for (;;) {
    if (simMode == XTMP_CYCLE_ACCURATE) {
      numCycles = sampleCycles;
      client_cmd = "profile enable";
    }
    else {
      /* TurboXim sample = 100X cycle-accurate sample */
      numCycles =  100 * sampleCycles;
      client_cmd = "profile disable";
    } 
    
    /* Enable profiling in cycle-accurate mode, disable it in TurboXim */
    for (i = 0; i < numComponents; i++) {
      XTMP_sendClientCommand(component[i], client_cmd);
    }

    /* Advance the simulation for one sample */
    XTMP_stepSystem(numCycles);
    if (XTMP_haveAllCoresExited()) 
      return;

    /* Choose simulation mode for the next sample */
    if (simMode == XTMP_CYCLE_ACCURATE)
      simMode = XTMP_FUNCTIONAL;
    else
      simMode = XTMP_CYCLE_ACCURATE;

    for (;;) {
      bool allReady = true;

      /* First phase: prepare to switch simulation mode */
      for (i = 0; i < numComponents; i++) {
        XTMP_simModeResponse switchResponse = 
          XTMP_prepareToSwitchSimMode(component[i], simMode, 0);
        if (switchResponse == XTMP_SIM_MODE_ERROR) {
          fprintf(stderr, "Error in switching simulation mode for %s\n",
                  XTMP_getComponentName(component[i]));
          exit(1);
        }
        else if (switchResponse == XTMP_SIM_MODE_PENDING) {
          allReady = false;
        } 
      }

      /* Second phase: 
       * If everybody's ready, do the switch
       */
      if (allReady) {
        for (i = 0; i < numComponents; i++) {
          if (XTMP_switchSimMode(component[i], simMode) != XTMP_DEVICE_OK) {
            fprintf(stderr, "Cannot switch simulation mode for %s\n",
                    XTMP_getComponentName(component[i]));
            exit(1);
          }
        }
        break;
      }
      else {
        /* Otherwise, advance by 1 cycle and repeat the prepare step */
        XTMP_stepSystem(1);
        if (XTMP_haveAllCoresExited()) 
          return;
      }
    }
  }
}


int
XTMP_main(int argc, char** argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core;
  XTMP_memory sysmem;
  XTMP_component component[1];
  XTMP_simMode simMode;
  u32 pifWidth; 
  int numComponents, i;
  bool bigEndian;
  xtmp_options options;
  xtmp_options *opt = &options;

  init_options(opt);
  strcpy(opt->prog_name, "sieve_long.out");
  opt->summary = true;
  opt->sample = 10000; /* default: 10000 cycles in the accurate mode */

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
    sysmem = XTMP_pifMemoryNew("sysmem", pifWidth, bigEndian, 0);
    XTMP_connectToCore(core, XTMP_PT_PIF, 0, sysmem, 0);
  }

  if (!XTMP_loadProgram(core, opt->prog_name, 0)) {
    fprintf(stderr, "Cannot load %s\n", opt->prog_name);
    exit(1);
  }

  XTMP_setRelaxedSimulationCycleLimit(opt->ooo_cycles);

  simMode = XTMP_CYCLE_ACCURATE;
  numComponents = 1;
  component[0] = core;

  if (opt->turbo) {
    for (i = 0; i < numComponents; i++) {
      if (XTMP_switchSimMode(component[i], XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
        fprintf(stderr, "Cannot switch %s to TurboXim mode\n",
                XTMP_getComponentName(component[i]));
        exit(1);
      }
    }
    simMode = XTMP_FUNCTIONAL;
  }

  if (opt->sample) {
    periodicSwitching(component, numComponents, simMode, opt->sample);
  }
  else {
    if (!opt->turbo && !strcmp(opt->prog_name, "sieve_long.out")) {
      fprintf(stderr,
              "Warning: this example may run for a long time "
              "in the cycle-accurate mode\n"
              "         (with --sample=0 and without --turbo)\n");
    }
    XTMP_stepSystem(opt->cycle_limit);
  }
  
  if (opt->summary)
    XTMP_printSummary(core, false);

  
  return 0;
}
