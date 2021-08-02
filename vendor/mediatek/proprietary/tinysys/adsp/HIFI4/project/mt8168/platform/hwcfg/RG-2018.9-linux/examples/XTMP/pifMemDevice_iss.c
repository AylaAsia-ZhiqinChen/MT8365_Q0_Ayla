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
 * A custom XTMP device which implements memory attached to the PIF.
 *
 * This is similiar to the built-in XTMP_memory device. It accepts read, 
 * write, block-read and block-write transactions, one at a time and 
 * responds after the configured delays.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "iss/mp.h"
#include "xtmp_config.h"
#include "xtmp_options.h"

static u32  pifWidth;
static bool bigEndian;

/*
 * Delay parameters for each transaction type. Since the device will be 
 * connected to the PIF, each delay value must be at least 1, which is
 * the inherent latency of the PIF.  An additional delay would correspond
 * to the time expended within the device itself.
 */
#define READ_DELAY 1
#define BLOCK_READ_DELAY 1
#define BLOCK_READ_REPEAT 1
#define WRITE_DELAY 1
#define BLOCK_WRITE_DELAY 1
#define BLOCK_WRITE_REPEAT 1

#define PAGE_BITS  16
#define PAGE_SIZE  (1 << PAGE_BITS)
#define PAGE_MASK  (PAGE_SIZE - 1)
#define PAGE_COUNT (1 << (32 - PAGE_BITS))

typedef struct {
  u32             *pages[PAGE_COUNT];	/* data allocated in pages */
  XTMP_deviceXfer *xfer;		/* transaction being processed */
  XTMP_time        responseTime;	/* when to respond */
  u32              transferNumber;	/* for block transactions only */
} SysMemData;


/*
 * A simple memory model that allocates space only for "pages" in use.
 */
static u32 *
getWordAddress(XTMP_address addr, u32 **pages)
{
  u32 pageIdx = addr >> PAGE_BITS;
  if (!pages[pageIdx])
    pages[pageIdx] = malloc(PAGE_SIZE);
  return pages[pageIdx] + ((addr & PAGE_MASK) >> 2);
}

/*
 * Read data from the location corresponding to the requested address.
 */
static XTMP_deviceStatus
memPeek(void *deviceData, u32 *dst, XTMP_address addr, u32 size)
{
  SysMemData *sysMemData = (SysMemData *) deviceData;
  u32 *buf = getWordAddress(addr, sysMemData->pages);
  u32 sz = (size < 4) ? size : 4;
  u32 i;
  for (i = 0; i < size; i += 4) {
    u32 val = XTMP_extract(buf++, addr+i, sz, 4, bigEndian);
    XTMP_insert(dst, addr+i, val, sz, pifWidth, bigEndian);
  }
  return XTMP_DEVICE_OK;
}

/*
 * Write data into the location corresponding to the requested address.
 */
static XTMP_deviceStatus
memPoke(void *deviceData, XTMP_address addr, u32 size, const u32 *src)
{
  SysMemData *sysMemData = (SysMemData *) deviceData;
  u32 *buf = getWordAddress(addr, sysMemData->pages);
  u32 sz = (size < 4) ? size : 4;
  u32 i;
  for (i = 0; i < size; i += 4) {
    u32 val = XTMP_extract(src, addr+i, sz, pifWidth, bigEndian);
    XTMP_insert(buf++, addr+i, val, sz, 4, bigEndian);
  }
  return XTMP_DEVICE_OK;
}

/*
 * Device function for the "post" callback.
 */
static XTMP_deviceStatus
memPost(void *deviceData, XTMP_deviceXfer *xfer)
{
  SysMemData *sysMemData = (SysMemData *) deviceData;

  if (XTMP_xferIsRequest(xfer)) {
    if (sysMemData->xfer != NULL) {
      /*
       * Reject request if already processing another.  An XTMP_NACC 
       * response must be returned in the current cycle itself.
       */
      XTMP_respond(xfer, XTMP_NACC);
    }
    else {
      XTMP_time now = XTMP_clockTime(); /* current cycle */
      /*
       * Store the transfer record.  It will be processed later in the 
       * ticker function. 
       */
      sysMemData->xfer = xfer;

      /* Keep track of when the response is to be issued. */
      switch (XTMP_xferGetType(xfer)) {
      case XTMP_READ:
	sysMemData->responseTime = now + READ_DELAY;
	break;
      case XTMP_WRITE:
	sysMemData->responseTime = now + WRITE_DELAY;
	break;
      case XTMP_BLOCKREAD:
	/* A block read transaction has one request, but multiple responses. */
	sysMemData->responseTime = now + BLOCK_READ_DELAY;
	/* Initialize the transfer number. */
	sysMemData->transferNumber = 0;
	break;
      case XTMP_BLOCKWRITE:
	/*
	 * The transfer number could potentially change, so the current 
	 * transfer accepted must be noted down. 
	 */
	sysMemData->transferNumber = XTMP_xferGetTransferNumber(xfer);
	/* A block write transaction has multiple requests and one response. */
        if (sysMemData->transferNumber == 0)
          sysMemData->responseTime = now + BLOCK_WRITE_DELAY;
        else
          sysMemData->responseTime = now + BLOCK_WRITE_REPEAT;
	break;
      default:
        return XTMP_DEVICE_ERROR;
      }
    }
    return XTMP_DEVICE_OK;
  }
  else {
    return XTMP_DEVICE_ERROR;
  }
}

/*
 * This is the user thread function for the ticker thread.  It will wake 
 * up on every cycle and respond to any requests that have matured. 
 */
static void
memTicker(void *threadData)
{
  while (!XTMP_haveAllCoresExited()) {	/* terminate if the core has exited  */

    SysMemData *sysMemData = (SysMemData *) threadData;
    XTMP_deviceXfer *xfer = sysMemData->xfer;

    /*
     * Check if there is a request being processed, and
     * if it is the right time to respond, if there is one. 
     */
    if (xfer &&	(sysMemData->responseTime == XTMP_clockTime())) {

      XTMP_address address = XTMP_xferGetAddress(xfer);
      XTMP_address blockAddress;
      XTMP_address currentAddress;
      u32          currentOffset;
      u32          size = XTMP_xferGetSize(xfer);
      u32         *data = XTMP_xferGetData(xfer);
      u32         *transferData;
      u8           firstTransfer;
      u8           transferNum;
      
      switch (XTMP_xferGetType(xfer)) {

      case XTMP_READ:
	/* Read data and respond. */
	memPeek(sysMemData, data, address, size);
	if (XTMP_respond(xfer, XTMP_OK) == XTMP_DEVICE_RESPONSE_NACC)
          sysMemData->responseTime++;
        else
	  sysMemData->xfer = NULL;
	break;

      case XTMP_WRITE:
	/* Write data and respond. */
	memPoke(sysMemData, address, size, data);
	if (XTMP_respond(xfer, XTMP_OK) == XTMP_DEVICE_RESPONSE_NACC)
          sysMemData->responseTime++;
        else
	  sysMemData->xfer = NULL;
	break;

      case XTMP_BLOCKREAD:
	transferNum = sysMemData->transferNumber;
	XTMP_xferSetTransferNumber(xfer, transferNum);
	
        /* With critical-word-first, address may not be block-size aligned. */
        blockAddress = address & (-size);
        firstTransfer = (address - blockAddress) / pifWidth;
        currentOffset = (firstTransfer + transferNum) * pifWidth;     
        /* Adjust the offset for possible wrap-around. */
        if (currentOffset >= size)
          currentOffset -= size;
        /* Identify the address for this transfer. */
        currentAddress = blockAddress + currentOffset;
	
        /* Locate the beginning of the data buffer for this transfer. */
	transferData = XTMP_xferGetBlockTransferData(xfer, transferNum);
	/* Read data into the buffer. */
	memPeek(sysMemData, transferData, currentAddress, pifWidth);
	if (transferNum == size/pifWidth - 1) {
	  /* Done with transaction. */
	  XTMP_xferSetLastData(xfer, 1);
	  sysMemData->xfer = NULL;
	}
	else {
	  /*
	   * Transaction partially complete.  Last data indication must 
	   * be set to 0 and the next transfer must be scheduled.
	   */
	  XTMP_xferSetLastData(xfer, 0);
	  sysMemData->responseTime = XTMP_clockTime() + BLOCK_READ_REPEAT;
	}
	if (XTMP_respond(xfer, XTMP_OK) == XTMP_DEVICE_RESPONSE_NACC)
          sysMemData->responseTime++;
        else
	  sysMemData->transferNumber++;
	break;

      case XTMP_BLOCKWRITE:
	transferNum = sysMemData->transferNumber;
	/* Identify the address for this transfer. */
	currentAddress = address + transferNum * pifWidth;
	/* Locate the beginning of the data buffer for this transfer. */
	transferData = XTMP_xferGetBlockTransferData(xfer, transferNum);
	/* Write data out of the buffer. */
	memPoke(sysMemData, currentAddress, pifWidth, transferData);
	/*
	 * Indicate that this transfer has been processed, so that the 
	 * next transfer can be accepted. 
	 */
	sysMemData->xfer = NULL;
	/*
	 * There is only one response for block writes, at the end of the 
	 * transaction. 
	 */
	if (transferNum == size/pifWidth - 1)
	  XTMP_respond(xfer, XTMP_OK);
	break;

      default:
	fprintf(stderr, "Received unsupported transaction.\n");
	exit(1);
      }

    }
    /* Wake up again in the next cycle. */
    XTMP_wait(1);
  }
}


/*
 * Fast-access read callback for LittleEndian request and memory interface
 */
static void
memFastReadLE(void *deviceData, u32 *dst, XTMP_address addr, u32 size)
{
  SysMemData *sysMemData = (SysMemData *) deviceData;
  u32 *buf = getWordAddress(addr, sysMemData->pages);
  u32 i;
  switch(size) {
  case 1:
    dst[0] = XTMP_extract32LE8(buf, addr);
    break;
  case 2:
    dst[0] = XTMP_extract32LE16(buf, addr);
    break;
  case 4:
    dst[0] = XTMP_extract32LE32(buf, addr);
    break;
  default:
    for (i = 0; i < size/4; i++) {
      dst[i] = XTMP_extract32LE32(&buf[i], addr+i*4);
    }
    break;
  }
}

/*
 * Fast-access write callback for LittleEndian request and memory interface
 */
static void
memFastWriteLE(void *deviceData, XTMP_address addr, u32 size, const u32 *src)
{
  SysMemData *sysMemData = (SysMemData *) deviceData;
  u32 *buf = getWordAddress(addr, sysMemData->pages);
  u32 i;
  switch(size) {
  case 1:
    XTMP_insert32LE8(buf, addr, src[0]);
    break;
  case 2:
    XTMP_insert32LE16(buf, addr, src[0]);
    break;
  case 4:
    XTMP_insert32LE32(buf, addr, src[0]);
    break;
  default: /* 8, 16 */
    for (i = 0; i < size/4; i++) {
      XTMP_insert32LE32(&buf[i], addr+i*4, src[i]);
    }
    break;
  }
}

/*
 * Fast-access read callback for BigEndian request and memory interface
 */
static void
memFastReadBE(void *deviceData, u32 *dst, XTMP_address addr, u32 size)
{
  SysMemData *sysMemData = (SysMemData *) deviceData;
  u32 *buf = getWordAddress(addr, sysMemData->pages);
  u32 i;
  switch(size) {
  case 1:
    dst[0] = XTMP_extract32BE8(buf, addr);
    break;
  case 2:
    dst[0] = XTMP_extract32BE16(buf, addr);
    break;
  case 4:
    dst[0] = XTMP_extract32BE32(buf, addr);
    break;
  default:
    for (i = 0; i < size/4; i++) {
      u32 dst_word = size/4 - i - 1;
      dst[dst_word] = XTMP_extract32BE32(&buf[i], addr+i*4);
    }
    break;
  }
}

/*
 * Fast-access write callback for BigEndian request and memory interface
 */
static void
memFastWriteBE(void *deviceData, XTMP_address addr, u32 size, const u32 *src)
{
  SysMemData *sysMemData = (SysMemData *) deviceData;
  u32 *buf = getWordAddress(addr, sysMemData->pages);
  u32 i;
  switch(size) {
  case 1:
    XTMP_insert32BE8(buf, addr, src[0]);
    break;
  case 2:
    XTMP_insert32BE16(buf, addr, src[0]);
    break;
  case 4:
    XTMP_insert32BE32(buf, addr, src[0]);
    break;
  default: /* 8, 16 */
    for (i = 0; i < size/4; i++) {
      u32 src_word = size/4 - i - 1;
      XTMP_insert32BE32(&buf[i], addr+i*4, src[src_word]);
    }
    break;
  }
}

/* 
 * Fast-access request handler
 */
static XTMP_deviceStatus
memFastAccess(void *deviceData,
              XTMP_fastAccessRequest request,
              XTMP_address addr)
{
  /* Chose the fast-access data transfer method */
#if !defined(FAST_ACCESS_RAW) && !defined(FAST_ACCESS_CALLBACKS) && !defined(FAST_ACCESS_PEEKPOKE)
#define FAST_ACCESS_RAW       1
#define FAST_ACCESS_CALLBACKS 0
#define FAST_ACCESS_PEEKPOKE  0
#endif

  if (FAST_ACCESS_RAW) {
    SysMemData *sysMemData = (SysMemData *) deviceData;
    u32 pageStart = addr & ~PAGE_MASK;
    XTMP_setFastAccessRaw(request, addr, pageStart, pageStart+PAGE_SIZE-1,
                          getWordAddress(pageStart, sysMemData->pages),
                          bigEndian ? 3 : 0 /* swizzle */);
  }
  else if (FAST_ACCESS_CALLBACKS) {
    if (bigEndian) {
      XTMP_setFastAccessCallBacks(request, addr, 0, 0xffffffff,
				  memFastReadBE, memFastWriteBE, deviceData);
    } else {
      XTMP_setFastAccessCallBacks(request, addr, 0, 0xffffffff,
				  memFastReadLE, memFastWriteLE, deviceData);
    }
  }
  else if (FAST_ACCESS_PEEKPOKE) {
    XTMP_setFastAccessPeekPoke(request, addr, 0, 0xffffffff);
  }
  else {
    XTMP_denyFastAccess(request, addr, 0, 0xffffffff);    
  }
  return XTMP_DEVICE_OK;
}


int 
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core;
  XTMP_userThread tickerThread;
  XTMP_device sysMem;
  XTMP_deviceFunctions sysMemFuncs = { memPost, memPeek, memPoke };
  SysMemData sysMemData = { { 0 }, 0, 0, 0 };
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
    /* Create a system memory device for all PIF accesses */
    sysMem = XTMP_deviceNew("sysMemDevice", &sysMemFuncs, &sysMemData,
			    pifWidth, 0);

    XTMP_connectToCore(core, XTMP_PT_PIF, 0, sysMem, 0);

    XTMP_setDeviceFastAccessHandler(sysMem, memFastAccess);
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
  else {
    tickerThread = XTMP_userThreadNew("ticker", memTicker, &sysMemData);
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
