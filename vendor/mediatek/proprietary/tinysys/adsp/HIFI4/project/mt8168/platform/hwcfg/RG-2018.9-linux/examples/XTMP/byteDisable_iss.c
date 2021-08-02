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

static const char *
binary(u64 val, unsigned bytes)
{
  static char str[65] = { 0 };
  unsigned i;
  if (bytes > 64)
    bytes = 64;
  for (i = 0; i < bytes; i++)
    str[bytes-i-1] = (val & (1 << i)) ? '1' :'0';
  str[bytes] = 0;
  return str;
}


typedef struct {
  u8          *data;
  XTMP_address base;
  u32          size;
  u32          byteWidth;
  bool         bigEndian;
} LocalMemoryData;


static u8 *
getByteAddress(LocalMemoryData *localMemory, XTMP_address addr)
{
  return localMemory->data + (addr - localMemory->base);
}

static bool
isByteEnabled(u64 byteEnables, XTMP_address addr, u32 byteWidth, bool bigEndian)
{
  u32 offset = addr & (byteWidth - 1);
  u32 shift = bigEndian ? byteWidth - 1 - offset : offset;
  return (byteEnables >> shift) & 0x1;
}

/*
 * Read data from the location corresponding to the requested address, 
 * taking into consideration the specified byte enables.
 */
static XTMP_deviceStatus
memPeekByteEn(void *deviceData, u32 *dst, XTMP_address addr, u64 byteEnables)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  u8 *buf = getByteAddress(localMemory, addr);
  u32 i;
  for (i = 0; i < localMemory->byteWidth; i++)
    if (isByteEnabled(byteEnables, addr+i,
                      localMemory->byteWidth, localMemory->bigEndian))
      XTMP_insert(dst, addr+i, buf[i], 1,
                  localMemory->byteWidth, localMemory->bigEndian);
  return XTMP_DEVICE_OK;
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
  for (i = 0; i < size; i++)
    XTMP_insert(dst, addr+i, buf[i], 1,
                localMemory->byteWidth, localMemory->bigEndian);
  return XTMP_DEVICE_OK;
}

/*
 * Write data into the location corresponding to the requested address, 
 * taking into consideration the specified byte enables.
 */
static XTMP_deviceStatus
memPokeByteEn(void *deviceData, XTMP_address addr, const u32 *src, u64 byteEnables)
{
  LocalMemoryData *localMemory = (LocalMemoryData *) deviceData;
  u8 *buf = getByteAddress(localMemory, addr);
  u32 i;
  for (i = 0; i < localMemory->byteWidth; i++)
    if (isByteEnabled(byteEnables, addr+i,
                      localMemory->byteWidth, localMemory->bigEndian))
      buf[i] = XTMP_extract(src, addr+i, 1,
                            localMemory->byteWidth, localMemory->bigEndian);
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
  for (i = 0; i < size; i++)
    buf[i] = XTMP_extract(src, addr+i, 1,
                          localMemory->byteWidth, localMemory->bigEndian);
  return XTMP_DEVICE_OK;
}

/*
 * Device function for the "post" callback.
 */
XTMP_deviceStatus
memPost(void *deviceData, XTMP_deviceXfer *xfer)
{
  XTMP_address address = XTMP_xferGetAddress(xfer);
  u32 size = XTMP_xferGetSize(xfer);
  u64 byteEnables = XTMP_xferGetByteEnables(xfer);
  u32 *data = XTMP_xferGetData(xfer);
  u32 byteWidth = ((LocalMemoryData *) deviceData)->byteWidth;

  if (XTMP_xferIsRequest(xfer)) {
    switch (XTMP_xferGetType(xfer)) {
    case XTMP_READ:
      if (XTMP_xferUseByteEnables(xfer)) {
        printf("  Read %u bytes from 0x%08x with byte enables %s\n",
	       size, address, binary(byteEnables, byteWidth));
        memPeekByteEn(deviceData, data, address & -byteWidth, byteEnables);
      }
      else {
        //printf("  Read %u %s from 0x%08x\n",
        //       size, size == 1 ? "byte" : "aligned bytes", address);
        memPeek(deviceData, data, address, size);
      }
      return XTMP_respond(xfer, XTMP_OK);
    case XTMP_WRITE:
      if (XTMP_xferUseByteEnables(xfer)) {
        printf("  Write %u bytes to 0x%08x with byte enables %s\n",
	       size, address, binary(byteEnables, byteWidth));
        memPokeByteEn(deviceData, address & -byteWidth, data, byteEnables);
      }
      else {
        //printf("  Write %u %s to 0x%08x\n",
        //       size, size == 1 ? "byte" : "aligned bytes", address);
        memPoke(deviceData, address, size, data);
      }
      return XTMP_respond(xfer, XTMP_OK);
    default:
      break;
    }
  }
  /* Local memories receive only single read and write requests. */
  return XTMP_DEVICE_ERROR;
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

  XTMP_setFastAccessRaw(request, addr, start, end,
                        (u32 *) localMemory->data, 0 /* swizzle */);
  return XTMP_DEVICE_OK;
}


int 
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  char *tdkFiles[]= {"byte_disable_tdk", 0};
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
  LocalMemoryData localMemDevice;
  xtmp_options options;

  /* Default option values */
  init_options(&options);
  strcpy(options.prog_name, "byte_disable.out");

  if (get_options(&options, argc, argv) != 0) {
    exit(1);
  }

  p = XTMP_paramsNewFromPath(xtensaSystemDirs, XTENSA_CORE, tdkFiles);
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

    localMemDevice.data = (u8 *) malloc(dataRamSize);
    localMemDevice.base = dataRamAddr;
    localMemDevice.size = dataRamSize;
    localMemDevice.byteWidth = dataRamWidth;
    localMemDevice.bigEndian = bigEndian;

    /* Create a new device instance and connect it to the data RAM port. */
    dataRam = XTMP_deviceNew("localMemory", &deviceFuncs, &localMemDevice,
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

  return 0;
}

