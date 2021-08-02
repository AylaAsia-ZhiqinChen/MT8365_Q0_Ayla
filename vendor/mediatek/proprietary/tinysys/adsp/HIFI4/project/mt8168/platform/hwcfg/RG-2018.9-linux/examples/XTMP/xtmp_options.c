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
 * Functions for option processing in XTMP examples.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getopt.h"
#include "xtmp_options.h"

void
init_options(xtmp_options *opt)
{
  memset(opt, 0, sizeof(xtmp_options));
  opt->trace_level = -1;
  opt->cycle_limit = -1;
  opt->ooo_cycles = 1000000;
}

int
get_options(xtmp_options *opt, int argc, char **argv)
{
  struct option long_options[] = {
	{"program",       required_argument, 0, 'p'},
	{"trace",         optional_argument, 0, 't'},
	{"trace-file",    required_argument, 0, 'f'},
	{"cycle-limit",   required_argument, 0, 'c'},
	{"debug",         optional_argument, 0, 'd'},
	{"jport",         required_argument, 0, 'j'},
	{"xxdebug",       no_argument,       0, 'x'},
	{"summary",       no_argument,       0, 's'},
	{"turbo",         no_argument,       0, 'T'},
	{"ooo-cycles",    required_argument, 0, 'O'},
	{"tdk",           required_argument, 0, 'K'},
	{"sample",        required_argument, 0, 'S'},
	{"help",          no_argument,       0, 'h'},
	{0, 0, 0, 0}
  };

  char usage[]="\nUsage: %s [option]...\n"
    "  --program=PROGFILE             Xtensa program to run\n"
    "  --trace[=LEVEL]                trace level (default is 2)\n"
    "  --trace-file=FILE              trace file\n"
    "  --cycle-limit=NUM              cycle limit for simulation\n"
    "  --debug[=PORTNUM]              wait for debugger (on port PORTNUM)\n"
    "  --xxdebug                      core information for Xtensa Xplorer\n"
    "  --summary                      print simulation summary\n"
    "  --turbo                        fast functional simulation mode\n"
    "  --ooo-cycles                   out-of-order cycle limit for turbo\n"
    "  --help                         this message\n";

  int c, errflag, opt_index;
  extern char* optarg;
  extern int optind;

  /* Read command-line arguments */
  c = errflag = 0;
  while (c >= 0) {
    c = getopt_long(argc, argv, "h", long_options, &opt_index);
    switch (c) {
     case 'p':
	if (!optarg) {
	  errflag++;
	} else {
	  strcpy(opt->prog_name, optarg);
	}
	break;
      case 'K':
      	if (!optarg) {
	  errflag++;
	} else {
	  strcpy(opt->tdk, optarg);
	}
	break;
      case 't':
	if (optarg) {
	  if ((opt->trace_level = atoi(optarg)) < 0) 
	    errflag++;
	} else {
	  opt->trace_level = 2;
	}
	break;
      case 'T':
	opt->turbo = 1;
	break;
      case 'O':
	if (optarg) {
	  opt->ooo_cycles = atoi(optarg); 
	} else {
	  errflag++;
	}
	break;
      case 'S':
	if (optarg) {
	  opt->sample = atoi(optarg);
	} else {
	  errflag++;
	}
	break;
      case 'f':
	if (!optarg) {
	  errflag++;
	} else {
	  strcpy(opt->trace_file, optarg);
	}
	break;
      case 'c':
        if (!optarg) {
          errflag++;
        } else {
          opt->cycle_limit = atoi(optarg);
        }
        break;
      case 'x':
	opt->xxdebug = 1;
	/* --xxdebug implies --debug. */
	/* FALLTHRU */
      case 'd':
	if (optarg) {
	  if ((opt->debug_port = atoi(optarg)) < 0) 
	    errflag++;
	  else
            opt->enable_debug = 1;
	} else {
	  opt->enable_debug = 1;
	  opt->debug_port = 0;
	}
	break;
      case 'j':
	if (!optarg || ((opt->jtag_port = atoi(optarg)) < 0)) {
	    errflag++;
	}
	break;
      case 's':
	opt->summary = 1;
        break;
      case 'h':
      case '?':
	fprintf(stderr, usage, argv[0]);
	return(1);
      case -1: 	  /* Detect the end of the options. */
        break;

      default:
	errflag++;
	break;
    }
  }

  /* Program name is required */
  if (errflag || optind < argc) {
    fprintf(stderr, usage, argv[0]);
    exit(2);
  }

  return errflag;
}
