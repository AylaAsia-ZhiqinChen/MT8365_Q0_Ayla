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
 * An example of transaction forwarding described in Section 5.5.1.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "iss/mp.h"
#include "xtmp_options.h"

/* An example device that forwards transactions to either
 * the system RAM or the catch-all memory on the PIF.
 */
typedef struct {
  XTMP_memory  pifMem;
  XTMP_memory  sysRam;
  XTMP_address sysRamAddr;
  u32          sysRamSize; 
} ForwardDeviceData;

/* Select the memory to which to forward the transaction */
static XTMP_memory
selectMemory(XTMP_address addr, ForwardDeviceData *deviceData)
{
  if ((addr >= deviceData->sysRamAddr) && 
      (addr <=  deviceData->sysRamAddr + deviceData->sysRamSize - 1))
    return deviceData->sysRam;
  else
    return deviceData->pifMem;
}

static XTMP_address
translateAddress(XTMP_address addr, ForwardDeviceData *deviceData)
{
  if ((addr >= deviceData->sysRamAddr) && 
      (addr <=  deviceData->sysRamAddr + deviceData->sysRamSize - 1))
    return (addr - deviceData->sysRamAddr);
  else
    return addr;
}

/* post, peek and poke callbacks simply forward
 * transactions to the selected memory
 */

static XTMP_deviceStatus
forwPost(void *devData, XTMP_deviceXfer *xfer)
{
  ForwardDeviceData *deviceData = (ForwardDeviceData *) devData;
  XTMP_address addr = XTMP_xferGetAddress(xfer);
  XTMP_memory memory = selectMemory(addr, deviceData);
  XTMP_address translatedAddr = translateAddress(addr, deviceData);
  XTMP_xferSetAddress(xfer, translatedAddr);
  return XTMP_post(memory, 0, xfer);
}

static XTMP_deviceStatus
forwPeek(void *devData, u32 *dst, XTMP_address addr, u32 size)
{
  ForwardDeviceData *deviceData = (ForwardDeviceData *) devData;
  XTMP_memory memory = selectMemory(addr, deviceData);
  XTMP_address translatedAddr = translateAddress(addr, deviceData);
  return XTMP_peek(memory, dst, translatedAddr, size);
}

static XTMP_deviceStatus
forwPoke(void *devData, XTMP_address addr, u32 size, const u32 *src)
{
  ForwardDeviceData *deviceData = (ForwardDeviceData *) devData;
  XTMP_memory memory = selectMemory(addr, deviceData);
  XTMP_address translatedAddr = translateAddress(addr, deviceData);
  return XTMP_poke(memory, translatedAddr, size, src);
}

/*
 * Fast-access handler
 */
static XTMP_deviceStatus
forwFastAccess(void *devData,
               XTMP_fastAccessRequest request,
               XTMP_address addr)
{
  ForwardDeviceData *deviceData = (ForwardDeviceData *) devData;
  XTMP_memory memory = selectMemory(addr, deviceData);
  XTMP_address translatedAddr = translateAddress(addr, deviceData);

  /* Forward the fast access request */
  if (XTMP_requestFastAccess(memory, request, translatedAddr) != XTMP_DEVICE_OK)
    return XTMP_DEVICE_ERROR;

  if (memory == deviceData->pifMem)
    /* sysRam addresses should not be mapped to the catch-all pifMem */
    if (!XTMP_removeFastAccessBlock(request, addr,
                                    deviceData->sysRamAddr,
				    deviceData->sysRamAddr + 
                                    deviceData->sysRamSize - 1))
      return XTMP_DEVICE_ERROR;

  return XTMP_DEVICE_OK;
}

int 
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core;
  XTMP_address sysRamAddr;
  u32 sysRamSize;
  u32 pifWidth;
  bool bigEndian;
  XTMP_device device;
  ForwardDeviceData deviceData = { 0 };
  XTMP_deviceFunctions deviceFuncs = { forwPost, forwPeek, forwPoke };
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
    if (XTMP_getSysRamAddress(p, &sysRamAddr) && 
        XTMP_getSysRamSize(p, &sysRamSize)) {
      deviceData.sysRam = 
        XTMP_pifMemoryNew("sysRam", pifWidth, bigEndian, sysRamSize);
      deviceData.sysRamAddr = sysRamAddr;
      deviceData.sysRamSize = sysRamSize;
    }
    /* Catch-all PIF memory for everything that isn't in SysRAM */
    deviceData.pifMem = XTMP_pifMemoryNew("pifMem", pifWidth, bigEndian, 0);

    device = XTMP_deviceNew("device", &deviceFuncs, &deviceData, pifWidth, 0);
    XTMP_connectToCore(core, XTMP_PT_PIF, 0, device, 0);

    XTMP_setDeviceFastAccessHandler(device, forwFastAccess);
  }

  if (!XTMP_loadProgram(core, opt->prog_name, 0)) {
    fprintf(stderr, "Cannot load %s\n", opt->prog_name);
    exit(1);
  }

  XTMP_setRelaxedSimulationCycleLimit(opt->ooo_cycles);
  if (opt->turbo) {
    if (XTMP_switchSimMode(core, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Could not switch cpu to functional mode\n");
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
