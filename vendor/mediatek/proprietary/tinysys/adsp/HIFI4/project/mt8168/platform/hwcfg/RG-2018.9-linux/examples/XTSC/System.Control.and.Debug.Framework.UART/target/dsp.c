// Customer ID=13943; Build=0x75f5e; Copyright (c) 2016-2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
// These coded instructions, statements, and computer programs are the
// copyrighted works and confidential proprietary information of
// Cadence Design Systems, Inc.  They may be adapted and modified by bona fide
// purchasers for internal use, but neither the original nor any adapted
// or modified version may be disclosed or distributed to third parties
// in any manner, medium, or form, in whole or in part, without the prior
// written consent of Cadence Design Systems, Inc.
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <xtensa/config/core.h>
#include <xtensa/xtruntime.h>
#include "../common.h"


typedef unsigned int u32;


u32  ps = 0;


void interrupt_handler() {
  _xtos_ints_off(1<<IntNum);
}


void enable_interrupt_and_wait() {
  _xtos_ints_on(1<<IntNum);
  asm(
    "waiti 0"         "\n"
    "rsil %0, 15"     "\n"
    : "=a" (ps)
  );
}


// status: returns 0 on success, -1 on failure
int write_char(char chr) {
#if XCHAL_HAVE_LE == 1
  *(volatile u32 *)(VADR_UART_WR) = chr;
#else
  *(volatile u32 *)(VADR_UART_WR) = (((u32)chr) << 24);
#endif
  return (((*(volatile u32 *)(VADR_UART_WR)) == 0) ? 0 : -1);
}


void write_c_str(char *msg) {
  while (*msg) {
    while (write_char(*msg));
    msg += 1;
  }
}


void write_line(char *msg) {
  write_c_str(msg);
  write_c_str("\n");
}


// status: returns 0 on success, -1 on failure
int read_char(char *p_chr) {
  u32 val = *(volatile u32 *)(VADR_UART_RD);
  if (val == 0xFFFFFFFF) return -1;
#if XCHAL_HAVE_LE == 1
  *p_chr = (val & 0xFF);
#else
  *p_chr = ((val >> 24) & 0xFF);
#endif
  // printf(">%d<\n", (int)*p_chr);
  return 0;
}


int read_line(char *line, int len) {
  int i;
  for (i=0; i<len-1; ++i) {
    while (read_char(&line[i]));
    if (line[i] == NEW_LINE_CHAR) break;
  }
  line[i] = 0;
  return i;
}



/**
 * This program listens for trivial example commands coming in on the uart, "performs" the command,
 * and sends a one line response back out through the uart.
 */
int main(int argc, char *argv[]) {
  char buf[BUF_LEN];

  xthal_set_region_attribute((int*)0x20000000, 0x20000000, XCHAL_CA_BYPASS, 0);
  
  _xtos_set_interrupt_handler(IntNum, interrupt_handler);
  asm("rsil %0, 15" : "=a" (ps));

  while (1) {
    read_line(buf, BUF_LEN);
    if (strcmp(buf, "help") == 0) {
      write_line("Supported commands are: e|help|pi|pie|quit|sleep");
    }
    else if (strcmp(buf, "quit") == 0) {
      break;
    }
    else if (strcmp(buf, "e") == 0)  {
      write_line("2.71828");
    }
    else if (strcmp(buf, "pi") == 0)  {
      write_line("3.14159");
    }
    else if (strcmp(buf, "pie") == 0)  {
      write_line("lemon with graham cracker crust is best");
    }
    else if (strcmp(buf, "sleep") == 0)  {
      printf("%lu: Going to sleep" "\n", clock());
      write_line("Zzzzzz...");
      enable_interrupt_and_wait();
      printf("%lu: Woke up" "\n", clock());
    }
    else {
      write_c_str("Unrecognized cmd (try 'help'): ");
      write_line(buf);
    }
  }
  return 0;
}

