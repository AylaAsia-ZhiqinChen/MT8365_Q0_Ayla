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
 * A custom XTMP device which implements the ReadConditionalWrite 
 * transaction. 
 *
 * The device is attached to a memory region that is marked bypass, so 
 * that requests go to it directly, instead of possibly through a cache.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iss/mp.h"
#include "xtmp_config.h"
#include "xtmp_options.h"

/* The size of the custom device */
#define DEVICE_SIZE 4096

static u32 pifWidth;
static bool bigEndian;

typedef struct {
  XTMP_memory memory;
  u32 busyCount;
} DeviceData;

static XTMP_deviceStatus
memPeek(void *deviceData, u32 *dst, u32 addr, u32 size)
{
  return XTMP_peek(((DeviceData *)deviceData)->memory, dst, addr, size);
}

static XTMP_deviceStatus
memPoke(void *deviceData, u32 addr, u32 size, const u32 *src)
{
  return XTMP_poke(((DeviceData *)deviceData)->memory, addr, size, src);
}

/*
 * Device function for the "post" callback.
 */
static XTMP_deviceStatus
memPost(void *deviceData, XTMP_deviceXfer *xfer)
{
  DeviceData* devData = (DeviceData *)deviceData;
  XTMP_address addr = XTMP_xferGetAddress(xfer);
  u32 size = XTMP_xferGetSize(xfer);
  u32 *data = XTMP_xferGetData(xfer);
  u32 read_data, write_data, compare_data, transfer_num;
  u32 buf[4];
  
  switch (XTMP_xferGetType(xfer)) {
  case XTMP_READ:
  case XTMP_WRITE:
    /* 
     * Forward request to memory, which will respond directly to the core. 
     */
    XTMP_post(devData->memory, NULL, xfer);
    break;
  case XTMP_RCW:
    transfer_num = XTMP_xferGetTransferNumber(xfer);
    printf("Received RCW request for address 0x%x, transfer %d\n",
	   addr, transfer_num);
    if (++devData->busyCount % 4) {
      /* Accept every fourth RCW request xfer. */
      printf("Responding with status XTMP_NACC\n");
      XTMP_respond(xfer, XTMP_NACC);
      break;
    }
    if (transfer_num == 0) {
      /*
       * The compare data arrives in the first RCW transfer.
       * Save it in the user data field. 
       */
      compare_data = XTMP_extract(data, addr, 4, pifWidth, bigEndian);
      XTMP_xferSetUserData(xfer, (void*)compare_data);
    }
    else if (transfer_num == 1) {
      /*
       * The write data arrives in the second RCW transfer.
       * Capture it into a local variable.
       */
      write_data = XTMP_extract(data, addr, 4, pifWidth, bigEndian);
      /*
       * Retrieve compare data from the location in which it was saved 
       * earlier. 
       */
      compare_data = (u32)XTMP_xferGetUserData(xfer);
      /*
       * A simplification would be to obtain the compare data directly
       * when the second transfer is received, by using the API function 
       * XTMP_xferGetCompareData.  In this case, no action needs to be 
       * taken when the first transfer arrives, apart from accepting it. 
       */
      if (compare_data != XTMP_xferGetCompareData(xfer)) {
	fprintf (stderr, "Compare data does not match value returned "
		 "by XTMP_xferGetCompareData");
	exit(1);
      }
      /*
       * It is now necessary to read data from the specified address. 
       * That is done here by peeking from the memory, to keep this 
       * example simple.  It would be more realistic to allocate a new 
       * xfer, post it to the memory and wait for the data to return. 
       */
      memPeek(deviceData, buf, addr, size);
      read_data = XTMP_extract(buf, addr, 4, pifWidth, bigEndian);
      printf ("Read: 0x%x, Compare: 0x%x, Write: 0x%x\n",
	      read_data, compare_data, write_data);
      /*
       * If the data that is read from the target address matches the 
       * compare data, a subsequent write must be performed. 
       */
      if (read_data == compare_data) {
	/* Simply poke the data to keep this simple. */
	printf("Compare succeeded, writing value 0x%x to memory.\n\n",
	       write_data);
	memPoke(deviceData, addr, size, data);
      }
      else {
	printf("Compare failed, no write performed to memory.\n\n");
      }
      /* Insert read data into the xfer and respond. */
      XTMP_insert(data, addr, read_data, 4, pifWidth, bigEndian);
      XTMP_respond(xfer, XTMP_OK);
    }
    else {
      fprintf(stderr, "Invalid transfer number received "
	      "for an RCW transaction");
      return XTMP_DEVICE_ERROR;
    }
    break;
  default:
    fprintf(stderr, "Invalid request received by device\n");
    return XTMP_DEVICE_ERROR;
  }

  return XTMP_DEVICE_OK;
}

int 
XTMP_main(int argc, char **argv)
{
#if XTMP_HAS_BYPASS_REGION

  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core;
  XTMP_connector connector;
  XTMP_memory sysRom, sysRam;
  XTMP_address sysRamAddr, sysRomAddr;
  u32 sysRamSize, sysRomSize;
  XTMP_device device;
  XTMP_deviceFunctions deviceFuncs;
  DeviceData deviceData;
  xtmp_options options;

  /* Default option values */
  init_options(&options);
  if (get_options(&options, argc, argv) != 0) {
    exit(1);
  }
  strcpy(options.prog_name, "rcw.out");
  options.cycle_limit = -1;

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

  pifWidth = XTMP_pifWidth(core);
  bigEndian = XTMP_isBigEndian(core);

  if (pifWidth) {
    connector = XTMP_connectorNew("connector", pifWidth, 0);
    XTMP_connectToCore(core, XTMP_PT_PIF, 0, connector, 0);

    if (XTMP_getSysRomAddress(p, &sysRomAddr) && 
        XTMP_getSysRomSize(p, &sysRomSize)) {
      sysRom = XTMP_pifMemoryNew("sysRom", pifWidth, bigEndian, sysRomSize);
      XTMP_memorySetReadOnly(sysRom, true);
      XTMP_addMapEntry(connector, sysRom, sysRomAddr, sysRomAddr);
    }

    if (XTMP_getSysRamAddress(p, &sysRamAddr) && 
        XTMP_getSysRamSize(p, &sysRamSize)) {
      sysRam = XTMP_pifMemoryNew("sysRam", pifWidth, bigEndian, sysRamSize);
      XTMP_addMapEntry(connector, sysRam, sysRamAddr, sysRamAddr);
    }

    /*
     * Allocate a backing memory that will handle all requests received 
     * by the device, except RCW requests. 
     */
    deviceData.memory =
      XTMP_pifMemoryNew("memory", pifWidth, bigEndian, XTMP_BYPASS_PSIZE);
    deviceData.busyCount = 0;

    deviceFuncs.peek = memPeek;
    deviceFuncs.poke = memPoke;
    deviceFuncs.post = memPost;
	
    device = XTMP_deviceNew("device", &deviceFuncs, &deviceData,
                            pifWidth, DEVICE_SIZE);
    XTMP_addMapEntry(connector, device, XTMP_BYPASS_PADDR, 0);
  }

  if (!XTMP_loadProgram(core, options.prog_name, 0)) {
    fprintf(stderr, "Cannot load %s\n", options.prog_name);
    exit(1);
  }

  XTMP_setRelaxedSimulationCycleLimit(options.ooo_cycles);
  if (options.turbo) {
    if (XTMP_switchSimMode(core, XTMP_FUNCTIONAL) != XTMP_DEVICE_OK) {
      fprintf(stderr, "Could not switch core to functional mode\n");
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
  printf("Target program exit code is %d\n", XTMP_getTargetExitCode(core));

  if (options.summary)
    XTMP_printSummary(core, false);

#else
  fprintf(stderr, "This example will not work because "
                  "bypass memory region was not found\n");
#endif

  return 0;
}
