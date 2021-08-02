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
#include <math.h>

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)              { TIMESTAMP; fprintf(stdout, msg);             fflush(stdout); }
#define LOG1(msg, arg1)       { TIMESTAMP; fprintf(stdout, msg, arg1);       fflush(stdout); }
#define LOG2(msg, arg1, arg2) { TIMESTAMP; fprintf(stdout, msg, arg1, arg2); fflush(stdout); }

typedef unsigned int u32;

volatile u32 table[] = {
  0x00112233, 0x44556677, 0x8899aabb, 0xccddeeff,
  0x00001111, 0x22223333, 0x44445555, 0x66667777,
  0x88889999, 0xaaaabbbb, 0xccccdddd, 0xeeeeffff,
  0x00000000, 0x11111111, 0x22222222, 0x33333333
};


int main(int argc, char *argv[]) {
  int           i;
  u32           total = 0;
  volatile u32 *p_data = &table[0];

  LOG("Xtensa core - starting.\n");

  for (i=0; i<16; i++) {
    table[i] += 1;
  }

  for (i=0; i<4; i++) {
    total += table[i];
  }

  table[0] = 0xbabecafe;
  table[2] = 0x2baddad8;

  for (i=4; i<16; i++) {
    total += *(p_data+i);
  }

  LOG1("Total = 0x%08x \n", total);

  return 0;
}

