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

#ifndef _BITS_TYPES_H
#define _BITS_TYPES_H

#ifndef _TOPLEVEL
#error Never directly include something from the bits directory
#endif

#include <yvals.h>

#ifdef __cplusplus
extern "C" {
#endif
  
typedef struct pthread_once_t
{
  int is_initialized;
  int init_executed;
} pthread_once_t;

  /* typedef long clock_t; -- conditionally declared here and in
     time.h  */
 #if !defined(_CLOCK_T) && !defined(__clock_t_defined) \
        && !defined(_BSD_CLOCK_T_DEFINED_)
  #define _CLOCK_T
  #define __clock_t_defined
  #define _BSD_CLOCK_T_DEFINED_
  #define _STD_USING_CLOCK_T
typedef long clock_t;
 #endif

  /* typedef long size_t; -- conditionally declared both here and in
     various other places.  */
#if !defined(_SIZE_T) && !defined(_SIZET) && !defined(_SIZE_T_DEFINED)	\
  && !defined(_BSD_SIZE_T_DEFINED_)
#define _SIZE_T
#define _SIZET
#define _BSD_SIZE_T_DEFINED_
#define _SIZE_T_DEFINED
#define _STD_USING_SIZE_T
  typedef _Sizet size_t;
#endif

typedef short dev_t;
typedef unsigned short gid_t;
typedef unsigned short uid_t;
typedef unsigned short ino_t;
typedef long key_t;
typedef unsigned int mode_t;
typedef unsigned short nlink_t;
typedef long off_t;
typedef int pid_t;
typedef int ssize_t;
typedef long suseconds_t;
typedef long time_t;
typedef unsigned long timer_t;
typedef long blksize_t;
typedef long blkcnt_t;

#ifdef __cplusplus
}
#endif

#endif
