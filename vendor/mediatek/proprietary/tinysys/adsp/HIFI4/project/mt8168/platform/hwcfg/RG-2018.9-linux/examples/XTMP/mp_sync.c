/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2009 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * 
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any adapted
 * or modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the prior
 * written consent of Tensilica Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <xtensa/tie/xt_sync.h>
#include "xtmp_config.h"

int delay()
{
  int i, s, n = rand() & 0xff;
  for (i = 0; i < n; i++)
    s += i;
  return s;
}

static void tiny_puts(const char *s);
static void tiny_putn(unsigned n);


/* A simple spin-lock implementation using the S32C1I instruction.
 * Lock is aquired by successfully changing its value from 0 to 1,
 * and released by successfully changing its value from 1 to 0.
 * NOTES:
 * - The initial value of the lock location must be 0. In simulation,
 *   this is assured when the shared memory is created by calling
 *   XTMP_pifMemoryNew in mp_iss.c
 * - In this example, the entire shared memory (including the lock)
 *   is placed in a bypass region. This avoids the need for manual
 *   cache management that would be required if the shared memory
 *   were placed in a cached region.
 */
static inline void
atomic_compare_set(int *addr, int cmp, int val)
{
  XT_WSR_SCOMPARE1(cmp);
  do {
    XT_S32C1I(val, addr, 0);
  } while (val != cmp);
}

#define LOCK_ACQUIRE(addr) atomic_compare_set(addr, 0, 1)
#define LOCK_RELEASE(addr) atomic_compare_set(addr, 1, 0)


/* Producer-consumer synchronization example.
 * A spin-lock is used to guard accesses to the shared resource (count).
 * Each increment of the count represents an item produced by the producer,
 * and the consumer consumes the items when they become available.
 */
int main()
{
  int *lock =  (int *) XTMP_BYPASS_VADDR;
  int *count = lock + 1;
  int i, n = 20;

#ifdef PRODUCER
  for (i = 0; i < n; i++) {
    LOCK_ACQUIRE(lock);
    (*count)++;
    tiny_puts("produce "); tiny_putn(*count); tiny_puts("\n");
    LOCK_RELEASE(lock);
  }
#else
  for (i = 0; i < n;) {
    LOCK_ACQUIRE(lock);
    if (i < *count) {
      i++;
      tiny_puts("consume "); tiny_putn(i); tiny_puts("\n");
    }
    LOCK_RELEASE(lock);
    delay();
  }
#endif

  return 0;
}


#define TINY_OUTMEM(buf,len)  write(1,buf,len)
#define TINY_OUTCHAR(c)       do{char __c=(c); TINY_OUTMEM(&__c,1);}while(0)

static void
tiny_puts(const char *s)
{
  const char *t = s;
  if (s == 0) return;
  while (*t) t++;
  TINY_OUTMEM(s,t-s);
}

static void
tiny_putn(unsigned n)
{
  /* avoids division  */
  int digit, i, seen = 0;
  for (digit = 0; n >= 1000000000; digit++)
    n -= 1000000000;
  for (i = 9;;) {
    seen += digit;
    if (seen)
      TINY_OUTCHAR('0'+digit);
    for (digit = 0; n >= 100000000; digit++)
      n -= 100000000;
    if (--i == 0) {
      TINY_OUTCHAR('0'+digit);
      return;
    }
    n = ((n << 1) + (n << 3));
  }
}

