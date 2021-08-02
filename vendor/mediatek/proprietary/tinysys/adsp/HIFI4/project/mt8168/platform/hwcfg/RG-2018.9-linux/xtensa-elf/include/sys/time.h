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

#ifndef	_SYS_TIME_H
#define	_SYS_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

struct timeval 
{
  time_t         tv_sec;
  suseconds_t    tv_usec;
};

struct itimerval
{
  struct timeval it_interval;
  struct timeval it_value;
};

int   gettimeofday(struct timeval *, void *);

/* Obsolete but kept for compatibility   */

struct timezone
{
  int tz_minuteswest;
  int tz_dsttime;
};


/* Unimplemented.  */

#if 0
fd_set
long  fds_bits[]
ITIMER_REAL
ITIMER_VIRTUAL
ITIMER_PROF
void FD_CLR(int fd, fd_set *fdset)
int FD_ISSET(int fd, fd_set *fdset)
void FD_SET(int fd, fd_set *fdset)
void FD_ZERO(fd_set *fdset)
FD_SETSIZE
int   getitimer(int, struct itimerval *);
int   setitimer(int, const struct itimerval *, struct itimerval *);
int   select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
int   utimes(const char *, const struct timeval [2]);

#endif

#ifdef __cplusplus
}
#endif
#endif
