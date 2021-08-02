// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// 
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <xtensa/config/core.h>
#include <xtensa/tie/xt_sync.h>
#include <xtensa/xtruntime.h>
#include "../common.h"

typedef unsigned int u32;

u32 *lock_addr  = (u32 *) SHMEM_LOCK_BASE;
u32 *work_area  = (u32 *) SHMEM_WORK_BASE;
u32 *flag_area  = (u32 *) SHMEM_WORK_FLAGS;
u32 *error_area = (u32 *) SHMEM_WORK_ERRORS;
u32  ps = 0;


void usage(int argc, char *argv[]) {
  printf("Usage: %s <N> <TOTAL> <WORDS> <CHECKS>" "\n", argv[0]);
  printf("Where:" "\n");
  printf("  <N>         is this core's index between 0 and (NumCores - 1)." "\n");
  printf("  <TOTAL>     is the number of iterations to do (0 means run forever)." "\n");
  printf("  <WORDS>     is the number of u32 words to write (between 1 and 256)." "\n");
  printf("  <CHECKS>    is the number of times to check the work area (between 1 and 256)." "\n");
  exit(-1);
}


void interrupt_handler() {
  _xtos_ints_off(1<<IntNum);
}


void enable_interrupt_and_wait() {
  _xtos_ints_on(1<<IntNum);
  asm(
    "waiti 0"         "\n"
    "rsil %0, 15"     "\n"
    : "=a" (ps)
  );
}


u32 atomic_write(volatile u32 *addr, u32 old_value, u32 new_value) {
#if XCHAL_HAVE_S32C1I
  XT_WSR_SCOMPARE1(old_value);
  XT_S32C1I(new_value, addr, 0);
  return new_value;     // This is now the old value
#elif XCHAL_HAVE_EXCLUSIVE
  u32 value = XT_L32EX(addr);
  u32 result = !old_value;
  if (value != old_value) {
    // When using xtsc_host_mutex, L32EX and S32EX must always execute as pairs.
    XT_S32EX(value, addr);
  }
  else {
    u32 tmp = 0;
    XT_S32EX(new_value, addr);
    XT_GETEX(tmp);
    if (tmp) {
      result = old_value;
    }
  }
  return result;
#else
#error Cannot implement atomic_write() function because neither S32C1I nor L32EX/S32EX/GETEX are available on this config
#endif
}


void acquire_lock(volatile u32 *lock_addr, u32 watermark) {
  u32 old_value = 0;
  while (atomic_write(lock_addr, old_value, watermark) != old_value) {
    enable_interrupt_and_wait();
  }
}


void release_lock(volatile u32 *lock_addr) {
  *lock_addr = 0;
}


int main(int argc, char *argv[]) {
  u32 N         = 0;    // From <N> (our core index)
  u32 TOTAL     = 0;    // From <TOTAL>
  u32 WORDS     = 0;    // From <WORDS>
  u32 CHECKS    = 0;    // From <CHECKS>

  u32 unlimited = 0;    // 1=process forever (set if TOTAL is 0)
  u32 watermark = 0;    // 0xBABE0000 + N
  u32 i         = 0;    // Iterate TOTAL
  u32 j         = 0;    // Iterate CHECKS
  u32 k         = 0;    // Iterate WORDS
  u32 errors    = 0;    // Count trespasses

  if (argc != 5)                        usage(argc, argv);
  N         = atoi(argv[1]);
  TOTAL     = atoi(argv[2]);
  WORDS     = atoi(argv[3]);
  CHECKS    = atoi(argv[4]);
  if ((WORDS  < 1) || (WORDS  > 256))   usage(argc, argv);
  if ((CHECKS < 1) || (CHECKS > 256))   usage(argc, argv);

  unlimited = (TOTAL == 0) ? 1 : 0;

  _xtos_set_interrupt_handler(IntNum, interrupt_handler);
  asm("rsil %0, 15" : "=a" (ps));

  // Create our personalized watermark
  watermark = 0xBABE0000 + N;

  // Do iterations
  for (i=0; unlimited || (i<TOTAL); ++i) {

    // Acquire the lock
    acquire_lock(lock_addr, watermark);

    // Set flags
    flag_area[0] = watermark;
    flag_area[1] = i;

    // Mark our territory
    for (k=0; k<WORDS; ++k) {
      work_area[k] = watermark;
    }

    // Make sure no one else messes with our territory
    for (j=0; j<CHECKS; ++j) {
      for (k=0; k<WORDS; ++k) {
        if (work_area[k] != watermark) {
          errors += 1;
          error_area[N] = errors;
          printf("core%02X: trespass error: watermark=0x%08x work_area[%d]=0x%08x i=%d j=%d errors=%d" "\n",
                 N, watermark, k, work_area[k], i, j, errors);
          printf("errors stored at address 0x%08x" "\n", (u32)&(error_area[N]));
        }
      }
    }

    // Spin without releasing lock if there are any errors
    if (errors) while (1);

    flag_area[0] = 0;

    // Release the lock
    release_lock(lock_addr);

    // Pass some time bragging about it
    printf("core%02X did iteration i=%d" "\n", N, i);
  }

  return 0;
}
