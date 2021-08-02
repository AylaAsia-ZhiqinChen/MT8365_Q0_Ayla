// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.


#include <stdio.h>
#include <time.h>
#include <xtensa/config/core.h>

#if XCHAL_HAVE_S32C1I
#include <xtensa/tie/xt_sync.h>
#endif

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)                { TIMESTAMP; fprintf(stdout, msg);              fflush(stdout); }
#define LOG1(msg, arg1)         { TIMESTAMP; fprintf(stdout, msg, arg1);        fflush(stdout); }
#define LOG2(msg, arg1, arg2)   { TIMESTAMP; fprintf(stdout, msg, arg1, arg2);  fflush(stdout); }


typedef unsigned int u32;

#if (XCHAL_HAVE_PTP_MMU == 0) && (XCHAL_NUM_DATARAM > 0)
#if (XCHAL_NUM_XLMI > 0)      || (XCHAL_NUM_DATARAM > 1)
#define HAS_TWO_LOCAL_MEMORIES_AND_NO_PTP_MMU
#endif
#endif

#ifdef HAS_TWO_LOCAL_MEMORIES_AND_NO_PTP_MMU
volatile char *pDRAM0 = (char *) XCHAL_DATARAM0_VADDR;
#if (XCHAL_NUM_XLMI > 0)
volatile char *pXLMI0 = (char *) XCHAL_XLMI0_VADDR;
volatile char *pMEM2  = (char *) XCHAL_XLMI0_VADDR;
#else
volatile char *pMEM2  = (char *) XCHAL_DATARAM1_VADDR;
#endif
#endif

u32 table[] = {
  0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff,
  0x00001111, 0x22223333, 0x44445555, 0x66667777,
  0x88889999, 0xaaaabbbb, 0xccccdddd, 0xeeeeffff,
  0x00000000, 0x11111111, 0x22222222, 0x33333333
};


#if XCHAL_HAVE_S32C1I
u32 memory_location = 0x00000000;

u32 test_rcw(u32 *pmem, u32 compare, u32 value) {
  XT_WSR_SCOMPARE1(compare);
  XT_S32C1I(value, pmem, 0);
  return value;     // This is now the old value
}
#endif


int main(int argc, char *argv[]) {
  int i;

  LOG("Xtensa core - starting.\n");
  if (argc > 1) {
    LOG1("argv[1]: %s \n", argv[1]);
  }
  LOG1("Program Name: %s \n", argv[0]);

  for (i=0; i<16; i++) {
    table[i] += 1;
  }

#if XCHAL_HAVE_S32C1I
  {

    u32 compare = 0x11111111;
    u32 new_value = 0x22222222;
    u32 old_value;
    old_value = test_rcw(&memory_location, compare, new_value);
    LOG2("test_rcw compare=0x%08x old_value=0x%08x \n", compare, old_value);

    compare   = old_value;
    new_value = 0x33333333;
    old_value = test_rcw(&memory_location, compare, new_value);
    LOG2("test_rcw compare=0x%08x old_value=0x%08x \n", compare, old_value);

    for (i=0; i<32; i++) {
      compare   = old_value;
      old_value = test_rcw(&memory_location, compare, i);
    }
  }
#endif


#ifdef HAS_TWO_LOCAL_MEMORIES_AND_NO_PTP_MMU
  {
    char c;
    char array[256];
    for (i=0; i<64; i++) {
      pDRAM0[i] = (char) i;
      pMEM2 [i] = (char) i;
    }

    for (i=0; i<64; i++) {
      array[i] = pDRAM0[i];
    }

#if (XCHAL_NUM_XLMI > 0)
    // Try to induce some load_retired calls
    if (array[0] == 0)  c=pXLMI0[0];
    if (array[1] == 1)  c=pXLMI0[1];
    if (array[2] == 2)  c=pXLMI0[2];
    if (array[3] == 0)  c=pXLMI0[3];
    if (array[4] == 0)  c=pXLMI0[4];
    if (array[5] == 5)  c=pXLMI0[5];
    if (array[6] == 6)  c=pXLMI0[6];
#endif
  }
#endif

  LOG("Xtensa core - done.\n");

  return 0;
}

