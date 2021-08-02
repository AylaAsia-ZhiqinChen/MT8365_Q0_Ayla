/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2003-2006 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * 
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any adapted
 * or modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the prior
 * written consent of Tensilica Inc.
 */

#include <xtensa/config/core.h>

#ifndef SIEVE_SIZE
#define SIEVE_SIZE 256
#endif

#if (XCHAL_NUM_DATARAM > 0)
/* Put sieve in Data RAM */
char sieve[SIEVE_SIZE] __attribute__ ((section(".dram0.data")));
#else
char sieve[SIEVE_SIZE];
#endif

/* Tiny alternatives to printf() */
static void tiny_puts(const char *s);
static void tiny_putn(unsigned n);

int main(int argc, char **argv)
{
  int i, j, k, p;

  /* To see the TurboXim speedup, define ITERS to 100+. */
#ifndef ITERS
#define ITERS 1
#endif
  for (k = 0; k < ITERS; k++) {
    for (i = 0; i < SIEVE_SIZE; i++) {
      sieve[i] = 0;
    }
    for (i = 0; i < SIEVE_SIZE; i++) {
      if (sieve[i] == 0) {
        p = 2*i+3;
        if (k == ITERS-1) {
          tiny_putn(p);
          tiny_puts("\n");
        }
        for (j = i; j < SIEVE_SIZE; j += p) {
	  sieve[j] = 1;
        }
      }
    }
  }

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
