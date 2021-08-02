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
#include <xtensa/tie/example.h>

typedef unsigned short u16;

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());
#define LOG1(msg, arg1) { TIMESTAMP; fprintf(stdout, msg, arg1); fflush(stdout); }


int main(int argc, char *argv[]) {
  u16   data;
  int   i;

  LOG1("Program Name: %s \n", argv[0]);

  for (i=0; i<8; ++i) {
    data = READ_Q2();
    LOG1("data=0x%04x \n", data);
  }

  return 0;
}

