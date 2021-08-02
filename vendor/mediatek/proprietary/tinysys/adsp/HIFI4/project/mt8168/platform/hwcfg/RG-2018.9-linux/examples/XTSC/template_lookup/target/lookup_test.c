// Customer ID=13943; Build=0x75f5e; Copyright (c) 2006-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
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
#include <xtensa/tie/example.h>

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)              { TIMESTAMP; fprintf(stdout, msg);             fflush(stdout); }
#define LOG1(msg, arg1)       { TIMESTAMP; fprintf(stdout, msg, arg1);       fflush(stdout); }
#define LOG2(msg, arg1, arg2) { TIMESTAMP; fprintf(stdout, msg, arg1, arg2); fflush(stdout); }

typedef unsigned int   u32;
typedef unsigned short u16;
typedef unsigned char  u8;

volatile u8 keys[] = {
  0x33, 0x77, 0xbb, 0xff,
  0x11, 0x33, 0x55, 0x77,
  0x99, 0xbb, 0xdd, 0xff,
  0x00, 0x11, 0x22, 0x33
};


int main(int argc, char *argv[]) {
  int  i;

  LOG("Xtensa core - starting.\n");

  for (i=0; i<sizeof(keys)/sizeof(keys[0]); i++) {
    u32 value = lookuptable(keys[i]);
    LOG2("0x%02x => 0x%08x \n", keys[i], value);
  }

  LOG("Xtensa core - done.\n");

  return 0;
}

