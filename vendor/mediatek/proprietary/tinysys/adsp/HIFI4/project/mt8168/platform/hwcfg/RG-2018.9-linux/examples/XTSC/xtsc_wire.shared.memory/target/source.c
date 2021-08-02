// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
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
#include <xtensa/tie/example.h>

typedef unsigned int            u32;

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)                    { TIMESTAMP; fprintf(stdout, msg);                   fflush(stdout); }
#define LOG1(msg, arg1)             { TIMESTAMP; fprintf(stdout, msg, arg1);             fflush(stdout); }
#define LOG2(msg, arg1, arg2)       { TIMESTAMP; fprintf(stdout, msg, arg1, arg2);       fflush(stdout); }
#define LOG3(msg, arg1, arg2, arg3) { TIMESTAMP; fprintf(stdout, msg, arg1, arg2, arg3); fflush(stdout); }



void usage(int argc, char *argv[]) {
  printf("Usage: %s <Log> <Total>" "\n", argv[0]);
  printf("Where:" "\n");
  printf("  <Log>       is 1 to turn on printing of wire data and 0 to turn it off." "\n");
  printf("  <Total>     is the number of wire data items to process before quiting (0 means never quit)." "\n");
  exit(-1);
}



int main(int argc, char *argv[]) {
  u32 i;
  u32 log               = 0;    // From <Log>
  u32 total             = 0;    // From <Total>

  if (argc != 3)              usage(argc, argv);
  log       = atoi(argv[1]);
  total     = atoi(argv[2]);
  if (log > 1)                usage(argc, argv);

  LOG1("Program Name: %s \n", argv[0]);

  WUR_SBUF_0(0x76543210);
  WUR_SBUF_1(0x0000BA98);

  for (i=0; !total || i<total; ++i) {
    write_status();  // SBUF => status
    if (log != 0) {
      LOG3("%s wrote status = 0x%05x%08x \n", argv[0], RUR_SBUF_1(), RUR_SBUF_0());
    }
    ADD_SBUF(1);
  }

  return 0;
}

