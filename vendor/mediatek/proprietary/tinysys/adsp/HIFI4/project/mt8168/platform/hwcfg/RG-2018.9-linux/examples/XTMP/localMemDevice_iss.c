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
#include "xtmp_config.h"
#include "xtmp_options.h"

typedef struct {
  u8          *data;
  XTMP_address base;
  u32          size;
  u32          byteWidth;
  bool         bigEndian;
  bool         hasBusy;
} LocalMemoryData;

static u8 *
getByteAddress(LocalMemoryData *localMemory, XTMP_address addr)
{
  return localMemory->data + (addr - localMemory->base);
}

/*
 * Read data from the location corresponding to the requested address.
 */
static XTMP_deviceStatus
memPeek(void *deviceData, u32 *dst, XTMP_address addr, u32 size)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  u8 *buf = getByteAddress(localMemory, addr);
  u32 i;
  for (i = 0; i < size; i++) {
    // Generic insert function
    XTMP_insert(dst, addr+i, buf[i], 1,
                localMemory->byteWidth, localMemory->bigEndian);
    // Config-specific insert macro for local data memory
    // XTMP_LS_INSERT(8)(dst, addr, buf[i]);
  }
  return XTMP_DEVICE_OK;
}

/*
 * Write data into the location corresponding to the requested address.
 */
static XTMP_deviceStatus
memPoke(void *deviceData, XTMP_address addr, u32 size, const u32 *src)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  u8 *buf = getByteAddress(localMemory, addr);
  u32 i;
  for (i = 0; i < size; i++) {
    // Generic extract function
    buf[i] = XTMP_extract(src, addr+i, 1,
                          localMemory->byteWidth, localMemory->bigEndian);
    // Config-specific extract macro for local data memory
    // buf[i] = XTMP_LS_EXTRACT(8)(src, addr);
  }
  return XTMP_DEVICE_OK;
}

/*
 * Determine if the memory is busy.
 * In this example, we use a simple random number computation.
 */
static bool
busy(LocalMemoryData *localMemory)
{
  static unsigned next = 1;
  if (!localMemory->hasBusy)
    return false;
  next = next * 1103515245 + 12345;
  return ((next >> 16) & ((1 << 15) - 1)) >> 14;
}
  
/*
 * Device function for the "post" callback.  Since the device is intended
 * for connection to an internal memory port, it will repond immediately
 * upon receiving a request.  A device counnected to the PIF would need to
 * wait for at least one cycle (to model PIF latency) and would require
 * a user thread function to implement the delay.
 */
static XTMP_deviceStatus
memPost(void *deviceData, XTMP_deviceXfer *xfer)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  XTMP_address address = XTMP_xferGetAddress(xfer);
  u32 size = XTMP_xferGetSize(xfer);
  u32 *data = XTMP_xferGetData(xfer);

  if (XTMP_xferIsRequest(xfer)) {
    if (busy(localMemory))
      return XTMP_respond(xfer, XTMP_NACC);
    if (XTMP_xferGetType(xfer) == XTMP_READ) {
      memPeek(deviceData, data, address, size);
      return XTMP_respond(xfer, XTMP_OK);
    }
    if (XTMP_xferGetType(xfer) == XTMP_WRITE) {
      memPoke(deviceData, address, size, data);
      return XTMP_respond(xfer, XTMP_OK);
    }
  }

  /* Local memories receive only single read and write requests. */
  return XTMP_DEVICE_ERROR;
}

/*
 * Fast-access read callback for little endian requests
 */

static void
memFastReadLE(void *deviceData, u32 *dst, XTMP_address addr, u32 size)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  u8 *buf = getByteAddress(localMemory, addr);
  u32 i;
  dst[0] = 0;
  for (i = 0; i < size; i++) {
    XTMP_insert32LE8(&dst[i>>2], i, buf[i]);
  }
}

/*
 * Fast-access read callback for big endian requests
 */

static void
memFastReadBE(void *deviceData, u32 *dst, XTMP_address addr, u32 size)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  u8 *buf = getByteAddress(localMemory, addr);
  u32 i;
  dst[0] = 0;
  for (i = 0; i < size; i++) {
    u32 dst_offset = size - i - 1;
    XTMP_insert32LE8(&dst[dst_offset>>2], dst_offset, buf[i]);
  }
}

/*
 * Fast-access write callback for little endian requests
 */

static void
memFastWriteLE(void *deviceData, XTMP_address addr, u32 size, const u32 *src)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  u8 *buf = getByteAddress(localMemory, addr);
  u32 i;
  for (i = 0; i < size; i++) {
    buf[i] = XTMP_extract32LE8(&src[i>>2], i);
  }
}

/*
 * Fast-access write callback for big endian requests
 */

static void
memFastWriteBE(void *deviceData, XTMP_address addr, u32 size, const u32 *src)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  u8 *buf = getByteAddress(localMemory, addr);
  u32 i;
  for (i = 0; i < size; i++) {
    u32 dst_offset = size - i - 1;
    buf[i] = XTMP_extract32LE8(&src[dst_offset>>2], dst_offset);
  }
}

/*
 * Fast-acess handler
 */
static XTMP_deviceStatus
memFastAccess(void *deviceData,
              XTMP_fastAccessRequest request,
              XTMP_address addr)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  XTMP_address start = localMemory->base;
  XTMP_address end = start + localMemory->size - 1;

  /* Chose the fast-access data transfer method */
#if !defined(FAST_ACCESS_RAW) && !defined(FAST_ACCESS_CALLBACKS) && !defined(FAST_ACCESS_PEEKPOKE)
#define FAST_ACCESS_RAW       1
#define FAST_ACCESS_CALLBACKS 0
#define FAST_ACCESS_PEEKPOKE  0
#endif

  if (FAST_ACCESS_RAW) {
    XTMP_setFastAccessRaw(request, addr, start, end,
                          (u32 *) localMemory->data, 0 /* swizzle */);
  }
  else if (FAST_ACCESS_CALLBACKS) {
    if (!XTMP_isFastAccessRequestBigEndian(request)) {
      XTMP_setFastAccessCallBacks(request, addr, start, end,
				  memFastReadLE, memFastWriteLE, deviceData);
    } else {
      XTMP_setFastAccessCallBacks(request, addr, start, end,
				  memFastReadBE, memFastWriteBE, deviceData);
    }
  }
  else if (FAST_ACCESS_PEEKPOKE) {
    XTMP_setFastAccessPeekPoke(request, addr, start, end);
  }
  else {
    XTMP_denyFastAccess(request, addr, start, end);
  }
  return XTMP_DEVICE_OK;
}


int 
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core;
  XTMP_connector connector;
  XTMP_device dataRam;
  XTMP_memory sysRam, sysRom;
  XTMP_address sysRamAddr, sysRomAddr, dataRamAddr;
  u32 sysRamSize, sysRomSize, dataRamSize;
  u32 pifWidth, dataRamWidth;
  bool bigEndian;
  XTMP_deviceFunctions deviceFuncs;
  LocalMemoryData dataRamStruct;
  xtmp_options options;

  /* Default option values */
  init_options(&options);
  strcpy(options.prog_name, "sieve.out");

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
  }

  if (XTMP_getLocalMemoryAddress(p, XTMP_PT_DRAM, 0, &dataRamAddr) && 
      XTMP_getLocalMemorySize(p, XTMP_PT_DRAM, 0, &dataRamSize)) {
    
    /* Initialize the device functions structure. */
    deviceFuncs.post = memPost;
    deviceFuncs.peek = memPeek;
    deviceFuncs.poke = memPoke;

    dataRamWidth = XTMP_loadStoreWidth(core);

    dataRamStruct.data = (u8 *) malloc(dataRamSize);
    dataRamStruct.base = dataRamAddr;
    dataRamStruct.size = dataRamSize;
    dataRamStruct.byteWidth = dataRamWidth;
    dataRamStruct.bigEndian = bigEndian;
    dataRamStruct.hasBusy = XTMP_DATA_RAM0_BUSY;

    /* Create a new device instance and connect it to the data RAM port. */
    dataRam = XTMP_deviceNew("dataRamDevice", &deviceFuncs, &dataRamStruct,
			     dataRamWidth, dataRamSize);
    XTMP_connectToCore(core, XTMP_PT_DRAM, 0, dataRam, dataRamAddr);

    XTMP_setDeviceFastAccessHandler(dataRam, memFastAccess); 
  }

  if (!XTMP_loadProgram(core, options.prog_name, 0)) {
    fprintf(stderr, "Cannot load %s\n", options.prog_name);
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

  if (options.turbo && !strcmp(options.prog_name, "sieve.out"))
    fprintf(stderr, "\nNote: To see the full benefit of TurboXim, "
            "run with --program=sieve_long.out\n\n");

  return 0;
}

