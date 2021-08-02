// Customer ID=13943; Build=0x75f5e; Copyright (c) 2005-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.


// This program read one datum from the upstream queue (if there is one), stores a
// modified counter in its bit field of the queue datum, and writes the datum to
// the downstream queue (if there is one).  If this is the last core, then the
// datum is optionally printed before being dropped on the floor.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <xtensa/tie/example.h>

typedef unsigned int            u32;

#define TIMESTAMP fprintf(stdout, "%lu: ", clock());

#define LOG(msg)                          { TIMESTAMP; fprintf(stdout, msg);                         fflush(stdout); }
#define LOG1(msg, arg1)                   { TIMESTAMP; fprintf(stdout, msg, arg1);                   fflush(stdout); }
#define LOG2(msg, arg1, arg2)             { TIMESTAMP; fprintf(stdout, msg, arg1, arg2);             fflush(stdout); }
#define LOG3(msg, arg1, arg2, arg3)       { TIMESTAMP; fprintf(stdout, msg, arg1, arg2, arg3);       fflush(stdout); }
#define LOG4(msg, arg1, arg2, arg3, arg4) { TIMESTAMP; fprintf(stdout, msg, arg1, arg2, arg3, arg4); fflush(stdout); }



void usage(int argc, char *argv[]) {
  printf("Usage: %s <N> <NumCores> <Log> <Total> <Turbo>" "\n", argv[0]);
  printf("Where:" "\n");
  printf("  <N>         is this core's index (position) between 0 and (<NumCores> - 1)." "\n");
  printf("  <NumCores>  is the total number of cores between 2 and 16." "\n");
  printf("  <Log>       is 1 to turn on printing of queue data and 0 to turn it off." "\n");
  printf("  <Total>     is the number of queue data items to process before quiting (0 means never quit)." "\n");
  printf("  <Turbo>     is 1 if TurboXim is being used, else is 0." "\n");
  exit(-1);
}



int main(int argc, char *argv[]) {
  u32 N                 = 0;    // From <N>
  u32 NumCores          = 0;    // From <NumCores>
  u32 log               = 0;    // From <Log>
  u32 total             = 0;    // From <Total>
  u32 turbo             = 0;    // From <Turbo>
  u32 first             = 0;    // First core in chain (core0)
  u32 last              = 0;    // Last core in chain
  u32 unlimited         = 0;    // Process forever (total is 0)
  u32 count             = 0;    // Count trips around the consume-produce loop
  u32 num_bits          = 0;    // Number of bits in our bit field in queue datum for our counter
  u32 index             = 0;    // Index (0, 1, or 2) of our bit field in queue data array
  u32 mask              = 0;    // Bits of our bit field in our data word
  u32 shift             = 0;    // Shift needed to align value with our bit field in our data word
  u32 interval          = 0;    // interval (trips around consume-produce loop) for printing progress when not doing it every time
  // NumCores           =   0  1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
  u32 num_bits_tab[17]  = { 0, 0, 32, 32, 32, 16, 16, 16,  8,  8,  8,  8,  8,  8,  4,  4,  4 };
  u32 data[3]           = { 0, 0, 0 };
  u32 datum             = 0;

  if (argc != 6)                           usage(argc, argv);

  N         = atoi(argv[1]);
  NumCores  = atoi(argv[2]);
  log       = atoi(argv[3]);
  total     = atoi(argv[4]);
  turbo     = atoi(argv[5]);
  last      = NumCores - 1;

  if ((NumCores < 2) || (NumCores > 16)) usage(argc, argv);
  if (N > last)                          usage(argc, argv);
  if (log > 1)                           usage(argc, argv);
  if (turbo > 1)                         usage(argc, argv);

  first     = (N == 0)              ? 1 : 0;
  last      = (N == (NumCores - 1)) ? 1 : 0;
  unlimited = (total == 0)          ? 1 : 0;
  num_bits  = num_bits_tab[NumCores];
  index     = (num_bits * N) / 32;
  shift     = (num_bits * N) - (index * 32);
  mask      = (num_bits == 32) ? 0xFFFFFFFF : (((1 << num_bits) - 1) << shift);
  interval  = (turbo == 1) ? 10001 : 101;

  printf("N=%d NumCores=%d first=%d last=%d num_bits=%d index=%d mask=0x%08x shift=%d" "\n", N, NumCores, first, last, num_bits, index, mask, shift);

  while (unlimited || (count < total)) {
    count += 1;

    if (!first) {
      READ_Q1();
      data[2] = RUR_QBUF_2();
      data[1] = RUR_QBUF_1();
      data[0] = RUR_QBUF_0();
      if (log != 0) {
        LOG4("core%X consumed 0x%08x%08x%08x \n", N, data[2], data[1], data[0]);
      }
      else if (last && ((count % interval) == 0)) {
        LOG4("core%X consumed 0x%08x%08x%08x \n", N, data[2], data[1], data[0]);
      }
    }

    if (!last) {
      datum  = data[index];
      datum &= ~mask;
      datum |= (((count + N) << shift) & mask);
      data[index] = datum;
      WUR_QBUF_2(data[2]);
      WUR_QBUF_1(data[1]);
      WUR_QBUF_0(data[0]);
      WRITE_Q1();
      if (log != 0) {
        LOG4("core%X produced 0x%08x%08x%08x \n", N, data[2], data[1], data[0]);
      }
    }

  }

  return 0;
}

