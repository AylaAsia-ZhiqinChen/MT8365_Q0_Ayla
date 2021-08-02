/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2004-2006 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
 * Example for functions described in Section 3.2.5.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iss/mp.h"
#include "xtmp_config.h"

/* The size of the custom device */
#define DEVICE_SIZE 4096

/* Uncomment the line below if you don't want to use the configuration
 * specific macros XTMP_PIF_INSERT and XTMP_PIF_EXTRACT from xtmp_config.h
 */
/* #define _GENERIC_DEVICE_ */

typedef struct {
  u32  data[DEVICE_SIZE/4];
  u32  byteWidth;
  bool bigEndian;
} DeviceData;

static XTMP_deviceStatus
devicePeek(void *deviceData, u32 *dst, XTMP_address offset, u32 size)
{
  DeviceData *devData = (DeviceData *) deviceData;
#ifndef _GENERIC_DEVICE_
  XTMP_PIF_INSERT(32)(dst, offset, devData->data[offset/4]);
#else
  XTMP_insert(dst, offset, devData->data[offset/4],
              4, devData->byteWidth, devData->bigEndian); 
#endif
  return XTMP_DEVICE_OK;
}

static XTMP_deviceStatus
devicePoke(void *deviceData, XTMP_address offset, u32 size, const u32 *src)
{
  DeviceData *devData = (DeviceData *) deviceData;
#ifndef _GENERIC_DEVICE_
  devData->data[offset/4] = XTMP_PIF_EXTRACT(32)(src, offset);
#else
  devData->data[offset/4] = 
    XTMP_extract(src, offset, 4, devData->byteWidth, devData->bigEndian);
#endif
  return XTMP_DEVICE_OK;
}

/* Initialize identifiable values to verify that peek and poke work */
static void
initRefValues(u32 *data, u32 *refValue, int n)
{
  int i;
  u32 value1 = 0x12345678, value2 = 0x87654321;
  for (i = 0; i < n; i++) {
    refValue[i] = (data[i] != value1 ? value1 : value2);
    value1 += 0x11111111;
    value2 += 0x11111111;
  }
}

/* Compare the data with the expected values */
static bool
valuesMatch(u32 *data, u32 *refValue, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    if (data[i] != refValue[i]) {
      printf("data[%d]=0x%08x, expected 0x%08x -- ", i, data[i], refValue[i]);
      return false;
    }
  }
  return true;
}

int 
XTMP_main(int argc, char **argv)
{
  char *xtensaSystemDirs[] = { XTENSA_SYSTEM, 0 };
  XTMP_params params;
  XTMP_core core;
  XTMP_connector connector;
  XTMP_memory sysRam, sysRom;
  XTMP_device device;
  XTMP_deviceFunctions deviceFuncs = { NULL, devicePeek, devicePoke }; 
  XTMP_deviceStatus status;
  DeviceData deviceData;
  u32 pifWidth;
  bool bigEndian;


  /* Test peeks and pokes for: 
   *  system RAM, XLMI, data RAM and custom device 
   */
  char *memName[4] = { "SysRAM", "XLMI", "DataRAM", "customDevice" };
  bool memConfigured[4] = { 
    XTMP_HAS_SYS_RAM, XTMP_HAS_XLMI, XTMP_HAS_DATA_RAM, XTMP_HAS_BYPASS_REGION
  };
  XTMP_address memVaddr[4] = { 
    XTMP_SYS_RAM_VADDR, XTMP_XLMI_VADDR, XTMP_DATA_RAM0_VADDR, XTMP_BYPASS_VADDR
  };
  XTMP_address memPaddr[4] = { 
    XTMP_SYS_RAM_PADDR, XTMP_XLMI_PADDR, XTMP_DATA_RAM0_PADDR, XTMP_BYPASS_PADDR
  };
  
  u32 buffer[4], refValue[4];
  unsigned numTests = 0, numFails = 0, i, j;
  bool fail;

  params = XTMP_paramsNewFromPath(xtensaSystemDirs, XTENSA_CORE, 0);
  if (!params) {
    fprintf(stderr, "Cannot create XTMP_params "XTENSA_CORE"\n");
    exit(1);
  }

  core = XTMP_coreNew("cpu", params, 0);
  if (!core) {
    fprintf(stderr, "Cannot create XTMP_core cpu\n");
    exit(1);
  }

  bigEndian = XTMP_isBigEndian(core);
  pifWidth = XTMP_pifWidth(core);

  if (pifWidth) {
    connector = XTMP_connectorNew("connector", pifWidth, 0);
    XTMP_connectToCore(core, XTMP_PT_PIF, 0, connector, 0);
    if (XTMP_HAS_SYS_RAM) {
      sysRam = XTMP_pifMemoryNew("sysRam", pifWidth, bigEndian, XTMP_SYS_RAM_SIZE);
      XTMP_addMapEntry(connector,sysRam,XTMP_SYS_RAM_PADDR,XTMP_SYS_RAM_PADDR);
    }
    if (XTMP_HAS_SYS_ROM) {
      sysRom = XTMP_pifMemoryNew("sysRom", pifWidth, bigEndian, XTMP_SYS_ROM_SIZE);
      XTMP_addMapEntry(connector,sysRom,XTMP_SYS_ROM_PADDR,XTMP_SYS_ROM_PADDR);
    }
    if (XTMP_HAS_BYPASS_REGION) {
      deviceData.byteWidth = pifWidth;
      deviceData.bigEndian = bigEndian;
      device = XTMP_deviceNew("device", &deviceFuncs, &deviceData,
                              deviceData.byteWidth, DEVICE_SIZE);
      XTMP_addMapEntry(connector, device, XTMP_BYPASS_PADDR, 0);
    }
  }
  
  if (!XTMP_loadProgram(core, "empty.out", 0)) {
    fprintf(stderr, "Cannot load empty.out\n");
    exit(1);
  }

  if (XTMP_HAS_BYPASS_REGION) {
    numTests++;
    printf("TEST%4u: XTMP_peekPhysical from unmapped PIF location: ", numTests);
    status = XTMP_peekPhysical(core, buffer, XTMP_BYPASS_PADDR+DEVICE_SIZE, 1);
    fail = (status == XTMP_DEVICE_OK);
    numFails += fail;
    printf("%s\n", (fail ? "FAIL" : "PASS"));

    numTests++;
    printf("TEST%4u: XTMP_peekVirtual from unmapped PIF location: ", numTests);
    status = XTMP_peekVirtual(core, buffer, XTMP_BYPASS_VADDR+DEVICE_SIZE, 1);
    fail = (status == XTMP_DEVICE_OK);
    numFails += fail;
    printf("%s\n", (fail ? "FAIL" : "PASS"));
  }

  /* Loop through memories */
  for (i = 0; i < 4; i++) {

    if (!memConfigured[i])
      continue;

    /* Loop through several addresses for each memory */
    for (j = 0; j < 4; j++) {

      numTests++;
      printf("TEST%4u: XTMP_peekVirtual_%u from %s: ", numTests, j, memName[i]);
      status = XTMP_peekVirtual(core, buffer, memVaddr[i], 4);
      fail = (status != XTMP_DEVICE_OK);
      numFails += fail;
      printf("%s\n", (fail ? "FAIL" : "PASS"));

      initRefValues(buffer, refValue, 4);
    
      numTests++;
      printf("TEST%4u: XTMP_pokeVirtual_%u to %s: ", numTests, j, memName[i]);
      memcpy(buffer, refValue, 4 * sizeof(u32));
      status = XTMP_pokeVirtual(core, buffer, memVaddr[i], 4);
      fail = (status != XTMP_DEVICE_OK);
      numFails += fail;
      printf("%s\n", (fail ? "FAIL" : "PASS"));
      
      /* Clear the internal PIF buffer */
      memset(buffer, 0, 4 * sizeof(u32));
      status = XTMP_pokeVirtual(core, buffer, memVaddr[i] + 4*4, 4);

      numTests++;
      printf("TEST%4u: XTMP_peekVirtual_%u readback check: ", numTests, j);
      memset(buffer, 0, 4 * sizeof(u32));
      status = XTMP_peekVirtual(core, buffer, memVaddr[i], 4);
      fail = (status != XTMP_DEVICE_OK || !valuesMatch(buffer, refValue, 4));
      numFails += fail;
      printf("%s\n", (fail ? "FAIL" : "PASS"));

      /* Clear the internal PIF buffer */
      memset(buffer, 0, 4 * sizeof(u32));
      status = XTMP_pokeVirtual(core, buffer, memVaddr[i] + 4*4, 4);

      numTests++;
      printf("TEST%4u: XTMP_peekPhysical_%u readback check: ", numTests, j);
      memset(buffer, 0, 4 * sizeof(u32));
      status = XTMP_peekPhysical(core, buffer, memPaddr[i], 4);
      fail = (status != XTMP_DEVICE_OK || !valuesMatch(buffer, refValue, 4));
      numFails += fail;
      printf("%s\n", (fail ? "FAIL" : "PASS"));

      initRefValues(buffer, refValue, 4);

      numTests++;
      printf("TEST%4u: XTMP_pokePhysical_%u to %s: ", numTests, j, memName[i]);
      memcpy(buffer, refValue, 4 * sizeof(u32));
      status = XTMP_pokePhysical(core, buffer, memPaddr[i], 4);
      fail = (status != XTMP_DEVICE_OK);
      numFails += fail;
      printf("%s\n", (fail ? "FAIL" : "PASS"));

      /* Clear the internal PIF buffer */
      memset(buffer, 0, 4 * sizeof(u32));
      status = XTMP_pokePhysical(core, buffer, memVaddr[i] + 4*4, 4);

      numTests++;
      printf("TEST%4u: XTMP_peekVirtual_%u readback check: ", numTests, j);
      memset(buffer, 0, 4 * sizeof(u32));
      status = XTMP_peekVirtual(core, buffer, memVaddr[i], 4);
      fail = (status != XTMP_DEVICE_OK || !valuesMatch(buffer, refValue, 4));
      numFails += fail;
      printf("%s\n", (fail ? "FAIL" : "PASS"));

      /* Clear the internal PIF buffer */
      memset(buffer, 0, 4 * sizeof(u32));
      status = XTMP_pokePhysical(core, buffer, memVaddr[i] + 4*4, 4);

      numTests++;
      printf("TEST%4u: XTMP_peekPhysical_%u readback check: ", numTests, j);
      memset(buffer, 0, 4 * sizeof(u32));
      status = XTMP_peekPhysical(core, buffer, memPaddr[i], 4);
      fail = (status != XTMP_DEVICE_OK || !valuesMatch(buffer, refValue, 4));
      numFails += fail;
      printf("%s\n", (fail ? "FAIL" : "PASS"));

      memVaddr[i] += 4;
      memPaddr[i] += 4;

    }
    
  }

  printf("Number of failures: %u\n", numFails);
  return numFails;
}
