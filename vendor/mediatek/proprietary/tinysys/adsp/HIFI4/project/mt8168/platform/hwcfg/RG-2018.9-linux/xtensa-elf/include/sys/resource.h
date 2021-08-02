/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2010 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef	_SYS_RESOURCE_H
#define	_SYS_RESOURCE_H

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int rlim_t;

struct rlimit
{
  rlim_t rlim_cur;
  rlim_t rlim_max;
};


struct rusage
{
  struct timeval ru_utime;
  struct timeval ru_stime;
};


  /* Unimplemented portions of resource.h.  */
#if 0
PRIO_PROCESS
PRIO_PGRP
PRIO_USER
RLIM_INFINITY
RLIM_SAVED_MAX
RLIM_SAVED_CUR
RUSAGE_SELF
RUSAGE_CHILDREN
RLIMIT_CORE
RLIMIT_CPU
RLIMIT_DATA
RLIMIT_FSIZE
RLIMIT_NOFILE
RLIMIT_STACK
RLIMIT_AS
int  getpriority(int, id_t);
int  getrlimit(int, struct rlimit *);
int  getrusage(int, struct rusage *);
int  setpriority(int, id_t, int);
int  setrlimit(int, const struct rlimit *);
#endif

#ifdef __cplusplus
}
#endif
#endif
