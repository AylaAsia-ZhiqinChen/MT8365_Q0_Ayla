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

/* 2 32-bit words to hold 40-bit FIFO data */
typedef u32 FIFO_DATA[2];

/* Main FIFO data structure */
typedef struct {
  unsigned   depth;      /* maximum number of entries in the FIFO */
  unsigned   count;      /* current number of entries in the FIFO */
  unsigned   writeIndex; /* index of the next FIFO entry write */
  unsigned   readIndex;  /* index of the next FIFO entry read */
  bool       push;       /* was there a push request? */
  bool       pop;        /* was there a pop request? */
  bool       full;       /* is the FIFO full? */
  bool       empty;      /* is the FIFO empty? */
  FIFO_DATA *data;       /* buffer to hold the FIFO data */
  XTMP_event notFullEvent; /* fires when FIFO goes from full to not full */
  XTMP_event notEmptyEvent;/* fires when FIFO goes from emtpy to not empty */
} FIFO;


void
FIFO_Push(XTMP_tiePort port, XTMP_core core, void *portData, u32 *data)
{
  FIFO *fifo = (FIFO *) portData;
  fifo->push = true;
  fifo->full = (fifo->count == fifo->depth);
  if (!fifo->full) {
    fifo->data[fifo->writeIndex][0] = data[0];
    fifo->data[fifo->writeIndex][1] = data[1];
    fifo->writeIndex = (fifo->writeIndex + 1) % fifo->depth;
  }
}

void
FIFO_Full(XTMP_tiePort port, XTMP_core core, void *portData, u32 *data)
{
  FIFO *fifo = (FIFO *) portData;
  *data = fifo->full;
  if (!fifo->full) {
    fifo->count++;
    if (fifo->count == 1)
      XTMP_fireEvent(fifo->notEmptyEvent);
  }
}


void
FIFO_Pop(XTMP_tiePort port, XTMP_core core, void *portData, u32 *data)
{
  FIFO *fifo = (FIFO *) portData;
  fifo->pop = true;
  fifo->empty = (fifo->count == 0);
}

void
FIFO_Empty(XTMP_tiePort port, XTMP_core core, void *portData, u32 *data)
{
  FIFO *fifo = (FIFO *) portData;
  if (fifo->pop) 
    *data = fifo->empty;
  else /* Empty can be invoked from NOTRDY interface without a Pop */
    *data = (fifo->count == 0) && !fifo->push;
  fifo->push = false;
  fifo->pop = false;
}

void
FIFO_Data(XTMP_tiePort port, XTMP_core core, void *portData, u32 *data)
{
  FIFO *fifo = (FIFO *) portData;
  data[0] = fifo->data[fifo->readIndex][0];
  data[1] = fifo->data[fifo->readIndex][1];
  fifo->readIndex = (fifo->readIndex + 1) % fifo->depth;
  fifo->count--;   
  if (fifo->count == fifo->depth - 1)
    XTMP_fireEvent(fifo->notFullEvent);
}


int
XTMP_main(int argc, char** argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  char *tdkFiles[]= {"queue_tdk", 0};
  XTMP_params params;
  XTMP_core producer, consumer;
  XTMP_memory sysmem1, sysmem2;
  FIFO fifo;
  u32 pifWidth;
  bool bigEndian;
  xtmp_options options;
  xtmp_options *opt = &options;

  init_options(opt);
  opt->summary = true;
  if (get_options(opt, argc, argv) != 0) {
    exit(1);
  }
    
  /* Initialize FIFO data */
  fifo.depth = 4;
  fifo.count = 0;
  fifo.writeIndex = 0;
  fifo.readIndex = 0;
  fifo.push = false;
  fifo.pop = false;
  fifo.data = (FIFO_DATA*) malloc(fifo.depth * sizeof(FIFO_DATA));

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

  /* Connect ports for the output TIE queue FIFO_OUT */
  XTMP_connectToTiePort(producer, &fifo, "FIFO_OUT_PushReq", FIFO_Push);
  XTMP_connectToTiePort(producer, &fifo, "FIFO_OUT_Full",    FIFO_Full);

  /* Connect ports for the input TIE queue FIFO_IN */
  XTMP_connectToTiePort(consumer, &fifo, "FIFO_IN_PopReq", FIFO_Pop);
  XTMP_connectToTiePort(consumer, &fifo, "FIFO_IN_Empty",  FIFO_Empty);
  XTMP_connectToTiePort(consumer, &fifo, "FIFO_IN",        FIFO_Data);

  fifo.notFullEvent = XTMP_getTieStallRecheckEvent(producer, "FIFO_OUT");
  fifo.notEmptyEvent = XTMP_getTieStallRecheckEvent(consumer, "FIFO_IN");

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
