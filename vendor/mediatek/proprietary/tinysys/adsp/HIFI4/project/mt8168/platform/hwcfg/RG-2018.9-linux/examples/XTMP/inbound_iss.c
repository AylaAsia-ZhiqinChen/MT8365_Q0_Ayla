/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2009 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
 * Example for inbound PIF transactions.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "iss/mp.h"
#include "xtmp_config.h"
#include "xtmp_options.h"
#include "inbound.h"

static u32  pifWidth;
static bool bigEndian;

/* Device data structure that will be passed to callback functions. */
typedef struct {
  XTMP_device    source;
  XTMP_connector target;
} DmaRegisterInfo;

/*
 * Utility function: return a printable string corresponding to the 
 * transaction type. 
 */
static const char *
xfer_type(XTMP_deviceXfer* xfer)
{
  switch (XTMP_xferGetType(xfer)) {
  case XTMP_READ:
    return "READ";
  case XTMP_WRITE:
    return "WRITE";
  case XTMP_BLOCKREAD:
    return "BLOCK-READ";
  case XTMP_BLOCKWRITE:
    return "BLOCK-WRITE";
  case XTMP_RCW:
    return "RCW";
  default:
    return "???";
  }
}

/*
 * This is the "post" callback function for a simple DMA register.  It is 
 * attached to the outbound PIF of the core, and when it receives a read 
 * or write request, it generates a translated inbound PIF request to the 
 * data RAM.  When the response from the inbound PIF request comes back, 
 * the device returns the response for the core's original request. 
 */
static XTMP_deviceStatus
dmaRegisterPost(void *deviceData, XTMP_deviceXfer *xfer)
{
  u32 word;
  DmaRegisterInfo *dmaRegInfo = (DmaRegisterInfo *) deviceData;
  
  if (XTMP_xferIsRequest(xfer)) {
    XTMP_transactionType xtype = XTMP_xferGetType(xfer);
    XTMP_address addr = XTMP_xferGetAddress(xfer);
    u32 *data = XTMP_xferGetData(xfer);
    
    /* Allocate a new xfer and data buffer for the inbound request. */
    XTMP_address newAddr = DRAM_TARGET_PADDR;
    u32 *newData = (u32 *) malloc(pifWidth);
    XTMP_deviceXfer *newXfer = XTMP_deviceXferNew(xtype, newAddr, 4, newData);
    
    /* For simplicity, we assume that all accesses are 32-bit only */
    if (xtype == XTMP_WRITE) {
      /* Copy the data to be written into the new xfer. */
      word = XTMP_extract(data, addr, 4, pifWidth, bigEndian);
      XTMP_insert(newData, newAddr, word, 4, pifWidth, bigEndian);
      // word = XTMP_PIF_EXTRACT(32) (data, addr);
      // XTMP_PIF_INSERT(32) (newData, newAddr, word);
    }
    else if (xtype != XTMP_READ) {
      fprintf(stderr, "Unexpected %s request.\n", xfer_type(xfer));
      return XTMP_DEVICE_ERROR;
    }
    
    /* Keep track of the original xfer in the user data field. */
    XTMP_xferSetUserData(newXfer, xfer);

    printf("\t=> received %s request to %x", xfer_type(xfer), addr);
    if (xtype == XTMP_WRITE)
      printf(", data 0x%08x", word);
    printf("\n\t\t=> issuing inbound request to 0x%x\n", newAddr);
    fflush(stdout);
    
    /*
     * Issue the inbound PIF request.  Indicate that the originating device 
     * is the inboundSourceDevice, so that responses get sent to it. 
     */
    XTMP_post(dmaRegInfo->target, dmaRegInfo->source, newXfer);
  }
  else if (XTMP_xferGetStatus(xfer) != XTMP_OK) {
    fprintf(stderr, "Inbound PIF request was not handled successfully.\n");
    return XTMP_DEVICE_ERROR;
  }
  else {
    /* Response processing */

    XTMP_deviceXfer *origXfer = XTMP_xferGetUserData(xfer);

    XTMP_address newAddr = XTMP_xferGetAddress(xfer);
    u32 *newData = XTMP_xferGetData(xfer);

    XTMP_address addr = XTMP_xferGetAddress(origXfer);
    u32 *data = XTMP_xferGetData(origXfer);
  
    switch (XTMP_xferGetType(xfer)) {
    case XTMP_READ:
      /* For simplicity, we assume that all accesses are 32-bit only */
      word = XTMP_extract(newData, newAddr, 4, pifWidth, bigEndian);
      XTMP_insert(data, addr, word, 4, pifWidth, bigEndian);
      // word = XTMP_PIF_EXTRACT(32) (newData, newAddr);
      // XTMP_PIF_INSERT(32) (data, addr, word);
      printf("\t\t=> inbound read receives value 0x%08x\n", word);
      /* FALLTHRU */
    case XTMP_WRITE:
      /* Deallocate the inbound xfer and its data buffer. */
      free(newData);
      XTMP_deviceXferFree(xfer);
      break;

    default:
      fprintf(stderr, "Unexpected inbound  %s response.\n", xfer_type(xfer));
      return XTMP_DEVICE_ERROR;
    }

    /* Respond with a success status to the original request. */
    printf("\t=> returning response\n");
    XTMP_respond(origXfer, XTMP_OK);
  }

  return XTMP_DEVICE_OK;
}


int 
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params p;
  XTMP_core core;
  XTMP_connector outboundPIF, inboundPIF;
  XTMP_memory sysRam, sysRom;
  XTMP_address sysRamAddr, sysRomAddr;
  u32 sysRamSize, sysRomSize;
  xtmp_options options;
  xtmp_options *opt = &options;

  XTMP_device dmaRegister;
  XTMP_deviceFunctions dmaRegisterFunctions = { dmaRegisterPost, NULL, NULL };
  DmaRegisterInfo dmaRegisterInfo;

#if !XTMP_HAS_DATA_RAM || !XTMP_DATA_RAM0_INBOUND
  {
    fprintf(stderr, "This example requires inbound PIF access to DRAM0.\n");
    return 0;
  }
#endif

  /* Default option values */
  init_options(opt);
  strcpy(opt->prog_name, "inbound.out");

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

  pifWidth = XTMP_pifWidth(core);
  bigEndian = XTMP_isBigEndian(core);

  outboundPIF = XTMP_connectorNew("outboundPIF", pifWidth, 0);
  XTMP_connectToCore(core, XTMP_PT_PIF, 0, outboundPIF, 0);
    
  if (XTMP_getSysRamAddress(p, &sysRamAddr) && 
      XTMP_getSysRamSize(p, &sysRamSize)) {
    sysRam = XTMP_pifMemoryNew("sysRam", pifWidth, bigEndian, sysRamSize);
    XTMP_addMapEntry(outboundPIF, sysRam, sysRamAddr, sysRamAddr);
  }
    
  if (XTMP_getSysRomAddress(p, &sysRomAddr) && 
      XTMP_getSysRomSize(p, &sysRomSize)) {
    sysRom = XTMP_pifMemoryNew("sysRom", pifWidth, bigEndian, sysRomSize);
    XTMP_memorySetReadOnly(sysRom, true);
    XTMP_addMapEntry(outboundPIF, sysRom, sysRomAddr, sysRomAddr);
  }

  /*
   * Create a PIF register that will be used to trigger inbound requests.
   */
  dmaRegister = XTMP_deviceNew("dmaRegister",
                               &dmaRegisterFunctions,
                               &dmaRegisterInfo,
                               pifWidth,
                               4);
  XTMP_addMapEntry(outboundPIF, dmaRegister,
                   XTMP_BYPASS_PADDR, XTMP_BYPASS_PADDR);
    
  /*
   * Create a connector that will be used to send inbound requests to the core.
   * This is done by mapping the core's data RAM address range to the
   * outbund interface of the connector.
   */
  inboundPIF = XTMP_connectorNew("inboundPIF", pifWidth, 0);
  XTMP_addMapEntryRange(inboundPIF,
                        core, 
			XTMP_DATA_RAM0_PADDR, 
			XTMP_DATA_RAM0_SIZE,
			XTMP_DATA_RAM0_PADDR);

  /*
   * Initialize device data passed to dmaRegisterPost.
   */
  dmaRegisterInfo.source = dmaRegister;
  dmaRegisterInfo.target = inboundPIF;

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

  /* Must return to main, which does cleanup */
  return 0;
}
