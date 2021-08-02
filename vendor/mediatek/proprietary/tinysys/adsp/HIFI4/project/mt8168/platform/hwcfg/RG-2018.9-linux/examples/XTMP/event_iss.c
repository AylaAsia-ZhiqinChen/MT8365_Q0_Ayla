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
 * A simple device example that uses events for user thread synchronization.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "iss/mp.h"
#include "xtmp_config.h"
#include "xtmp_options.h"

static u32  pifWidth;
static bool bigEndian;

#define DEVICE_ADDRESS XTMP_BYPASS_PADDR
#define DEVICE_SIZE    4

#define READ_CYCLES 20

typedef struct {
  XTMP_deviceXfer *xfer;                /* transaction being processed */
  XTMP_event       triggerEvent;        /* event used for sunhronization */
  u32              data;                /* device data */
  bool             busy;                /* an indication that device is busy */
  bool             shouldTerminate;     /* time to exit */
} SlowRegisterData;


void
devThreadFunc(void *threadData)
{
  XTMP_address addr;
  u32 *dst;
  
  SlowRegisterData *slowRegister = (SlowRegisterData *) threadData;

  while (1) {
    XTMP_waitOnEvent(slowRegister->triggerEvent);
    if (slowRegister->shouldTerminate)
      break;
    slowRegister->busy = true;
    XTMP_wait(READ_CYCLES);
    dst = XTMP_xferGetData(slowRegister->xfer);
    addr = XTMP_xferGetAddress(slowRegister->xfer);
    XTMP_insert(dst, addr, slowRegister->data, 4, pifWidth, bigEndian);
    // XTMP_PIF_INSERT(32)(dst, addr, slowRegister->data);
    printf("[Cycle "PCT_LLD"] Read from device returned 0x%08x\n",
           XTMP_clockTime(), slowRegister->data);
    XTMP_respond(slowRegister->xfer, XTMP_OK);
    slowRegister->busy = false;
  }
  printf("[Cycle "PCT_LLD"] Device thread terminated\n", XTMP_clockTime());
}

XTMP_deviceStatus
devPost(void *deviceData, XTMP_deviceXfer *xfer)
{
  SlowRegisterData *slowRegister = (SlowRegisterData *) deviceData;
  if (slowRegister->busy) {
    printf("[Cycle "PCT_LLD"] Device is busy\n", XTMP_clockTime());
    return XTMP_respond(xfer, XTMP_NACC);
  }
  else if (XTMP_xferGetType(xfer) == XTMP_WRITE) {    
    u32 *src = XTMP_xferGetData(xfer);
    XTMP_address addr = XTMP_xferGetAddress(xfer);
    slowRegister->data = XTMP_extract(src, addr, 4, pifWidth, bigEndian);
    // slowRegister->data = XTMP_PIF_EXTRACT(32)(src, addr);
    printf("[Cycle "PCT_LLD"] Wrote 0x%08x to device\n",
           XTMP_clockTime(), slowRegister->data);
    return XTMP_respond(xfer, XTMP_OK);
  }
  else if (XTMP_xferGetType(xfer) == XTMP_READ) {
    slowRegister->xfer = xfer;
    XTMP_fireEvent(slowRegister->triggerEvent);
    printf("[Cycle "PCT_LLD"] Device received read request\n", 
           XTMP_clockTime());
    return XTMP_DEVICE_OK;
  }

  fprintf(stderr, "Unexpected transaction in devPost\n");
  return XTMP_DEVICE_ERROR;
}

void
cleanupFunc(void *threadData)
{
  SlowRegisterData *slowRegister = (SlowRegisterData *) threadData;
  XTMP_waitOnEvent(XTMP_getAllCoresExitedEvent());
  printf("[Cycle "PCT_LLD"] Monitor thread requested termination\n", 
         XTMP_clockTime());
  slowRegister->shouldTerminate = true;
  XTMP_fireEvent(slowRegister->triggerEvent);
}


int 
XTMP_main(int argc, char **argv)
{
#if XTMP_HAS_BYPASS_REGION

  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core;
  XTMP_connector connector;
  XTMP_memory sysRam, sysRom;
  XTMP_address sysRamAddr, sysRomAddr;
  u32 sysRamSize, sysRomSize;

  XTMP_userThread devThread, monitorThread;
  XTMP_deviceFunctions devFuncs = { devPost, 0, 0 };
  XTMP_device slowRegisterDevice;
  SlowRegisterData slowRegister = { 0 };

  xtmp_options options;

  /* Default option values */
  init_options(&options);
 
  if (get_options(&options, argc, argv) != 0) {
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

    slowRegister.triggerEvent = XTMP_eventNew();

    slowRegisterDevice = XTMP_deviceNew("slowReg", &devFuncs, &slowRegister,
                                        pifWidth, DEVICE_SIZE);
    XTMP_addMapEntry(connector, slowRegisterDevice,
                     DEVICE_ADDRESS, DEVICE_ADDRESS);
  
    devThread = XTMP_userThreadNew("devThread", devThreadFunc, &slowRegister);
    monitorThread = XTMP_userThreadNew("monitor", cleanupFunc, &slowRegister);
  }
    
  if (!XTMP_loadProgram(core, "event.out", 0)) {
    fprintf(stderr, "Cannot load event.out\n");
    exit(1);
  }

  XTMP_setRelaxedSimulationCycleLimit(options.ooo_cycles);
  if (options.turbo) {
    if (XTMP_switchSimMode(core, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Could not switch cpu to functional mode\n");
    }
  }
  if (options.trace_file[0])
    XTMP_setTraceFile(core, options.trace_file);
  if (options.trace_level >= 0)
    XTMP_setTraceLevel(core, options.trace_level);
  if (options.enable_debug) {
    options.debug_port = XTMP_enableDebug(core, options.debug_port);
    if (!options.xxdebug)
      fprintf(stderr, "Waiting for debugger on port %d\n", options.debug_port);
    XTMP_setWaitForDebugger(core, true);
  }

  XTMP_stepSystem(options.cycle_limit);

  if (options.summary)
    XTMP_printSummary(core, false);

#else
  fprintf(stderr, "This example will not work because "
                  "bypass memory region was not found\n");
#endif

  return 0;
}
