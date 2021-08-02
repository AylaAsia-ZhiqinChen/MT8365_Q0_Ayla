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

#define LOG(msg)        { TIMESTAMP; fprintf(stdout, msg);       fflush(stdout); }
#define LOG1(msg, arg1) { TIMESTAMP; fprintf(stdout, msg, arg1); fflush(stdout); }

#define ADDR_BITS 8


int main(int argc, char *argv[]) {
  int offset;
  int errors = 0;

  LOG("Xtensa core - starting.\n");

  // Write all
  for (offset = 0; offset<(1<<ADDR_BITS); offset++) {
    lookup_ram_store(offset, offset);
  }

  // Read all
  for (offset = 0; offset<(1<<ADDR_BITS); offset++) {
    errors += lookup_ram_load(offset) != offset;
  }

  // Write, read interleaved
  for (offset = 0; offset<(1<<ADDR_BITS); offset++)  {
   lookup_ram_store(offset,offset);
   errors += lookup_ram_load(offset) != offset;
  }

  LOG1("Total errors = %d \n", errors);

  return errors;
}

