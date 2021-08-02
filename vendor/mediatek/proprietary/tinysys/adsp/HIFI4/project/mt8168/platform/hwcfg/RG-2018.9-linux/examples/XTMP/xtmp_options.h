/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2003-2009 by Tensilica Inc.  ALL RIGHTS RESERVED.
 *
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any adapted
 * or modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the prior
 * written consent of Tensilica Inc.
 */

/*
 * Declarations for option processing in XTMP examples.
 */

#define LEN 256

typedef struct {
  char prog_name[LEN];  /* Xtensa executable */
  char trace_file[LEN]; /* trace output file */
  char tdk[LEN];        /* extra tdk */
  int  trace_level;     /* level of trace detail */
  int  cycle_limit;     /* number of cycles to run (-1 means forever) */
  int  debug_port;      /* debugger port */
  int  jtag_port;	/* jtag simulation listener port */
  char enable_debug;    /* enable debugging */
  char xxdebug;         /* information for Xplorer */
  char summary;         /* simulation summary */
  char turbo;           /* turbo simulation */
  unsigned ooo_cycles;  /* out-of-order cycle limit for functional simulation */
  unsigned sample;      /* cycle-accurate sample size when switching */
} xtmp_options;

void
init_options(xtmp_options *opt);

int
get_options(xtmp_options *opt, int argc, char **argv);

