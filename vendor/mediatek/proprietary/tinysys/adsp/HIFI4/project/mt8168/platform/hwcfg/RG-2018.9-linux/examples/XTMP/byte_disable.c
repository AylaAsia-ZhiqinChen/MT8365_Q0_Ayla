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

#include <stdio.h>
#include <xtensa/config/core.h>
#include <xtensa/tie/byte_disable.h>

static const char *
binary(unsigned val, unsigned bytes)
{ 
  static char str[17] = { 0 };
  unsigned i;
  if (bytes > 16)
    bytes = 16;
  for (i = 0; i < bytes; i++)
    str[bytes-i-1] = (val & (1 << i)) ? '1' :'0';
  str[bytes] = 0;
  return str;
}

#if (XCHAL_NUM_DATARAM > 0)
/* Put the variable in data RAM */
unsigned int x[4] __attribute__ ((section(".dram0.data")));
#else
unsigned int x[4];
#endif

int main(int argc, char **argv)
{
  unsigned data = 0x12345678;
  unsigned i, disable, val;

  for (i = 0; i < 4; i++) {
    for (disable = 0; disable < 16; disable++) {
      x[i] = 0; 
      printf("Store 0x%08x to 0x%08x with byte disables %s\n",
             data, x+i, binary(disable, 4));
      store_bd(data, x+i, disable); // store with byte disables
      val = x[i]; // regular load with all bytes enabled
      printf("    Read back from 0x%08x: 0x%08x\n", x+i, val);

      x[i] = data; // regular store with all bytes enabled
      printf("Load 0x%08x from 0x%08x with byte disables %s\n",
             data, x+i, binary(disable, 4));
      val = load_bd(x+i, disable); // load with byte disables
      printf("    Result is 0x%08x\n", val);
    }
  } 
  return 0;
}
