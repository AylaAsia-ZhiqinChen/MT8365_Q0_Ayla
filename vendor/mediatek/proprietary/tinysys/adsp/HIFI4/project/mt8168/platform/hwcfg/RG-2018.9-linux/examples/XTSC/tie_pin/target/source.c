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
#include <xtensa/tie/example.h>

typedef unsigned int            u32;

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)                    { TIMESTAMP; fprintf(stdout, msg);                   fflush(stdout); }
#define LOG1(msg, arg1)             { TIMESTAMP; fprintf(stdout, msg, arg1);             fflush(stdout); }
#define LOG2(msg, arg1, arg2)       { TIMESTAMP; fprintf(stdout, msg, arg1, arg2);       fflush(stdout); }
#define LOG3(msg, arg1, arg2, arg3) { TIMESTAMP; fprintf(stdout, msg, arg1, arg2, arg3); fflush(stdout); }

int main(int argc, char *argv[]) {
  u32 i;
  LOG1("Program Name: %s \n", argv[0]);

  WUR_SBUF_0(0x76543210);
  WUR_SBUF_1(0x0000BA98);

  for (i=0; i<20; ++i) {
    write_status();  // SBUF => status

    LOG3("%s wrote status = 0x%05x%08x \n", argv[0], RUR_SBUF_1(), RUR_SBUF_0());

    ADD_SBUF(1);
  }

  return 0;
}

