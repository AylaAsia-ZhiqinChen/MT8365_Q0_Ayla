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

#ifndef	_FCNTL_H
#define	_FCNTL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <xtensa/simcall-fcntl.h>
#define	_FOPEN		(-1)
#define	_FREAD		0x0001
#define	_FWRITE		0x0002
#define	_FAPPEND        _SIMC_O_APPEND
#define	_FCREAT		_SIMC_O_CREAT
#define	_FTRUNC		_SIMC_O_TRUNC
#define	_FEXCL		_SIMC_O_EXCL
#define	_FNONBLOCK	_SIMC_O_NONBLOCK
#define	_FNDELAY	_SIMC_O_NONBLOCK

#define	O_ACCMODE	(O_RDONLY | O_WRONLY | O_RDWR)

#define	O_RDONLY	0
#define	O_WRONLY	1
#define	O_RDWR		2
#define	O_APPEND	_FAPPEND
#define	O_CREAT		_FCREAT
#define	O_TRUNC		_FTRUNC
#define	O_EXCL		_FEXCL
#define	O_NDELAY	_FNDELAY
#define	O_NONBLOCK	_FNONBLOCK

#define _FBINARY        _SIMC_O_BINARY
#define _FTEXT          _SIMC_O_TEXT

#define O_BINARY	_FBINARY
#define O_TEXT		_FTEXT

#define	FD_CLOEXEC	1

#define	F_DUPFD		0
#define	F_GETFD		1
#define	F_SETFD		2
#define	F_GETFL		3
#define	F_SETFL		4
#define	F_GETLK  	7
#define	F_SETLK  	8
#define	F_SETLKW 	9

#include <sys/types.h>
#include <sys/stat.h>		/* sigh. for the mode bits for open/creat */

extern int open (const char *, int, ...);
extern int creat (const char *, mode_t);
extern int fcntl (int, int, ...);

  /* Unsupported posix  */
#if 0
FD_CLOEXEC
F_RDLCK
F_UNLCK
F_WRLCK

O_NOCTTY
O_DSYNC
O_RSYNC
O_SYNC

struct 
{
  short l_type;
  short l_whence;
  off_t l_start;
  off_t l_len;
  pid_t l_pid;
} flock;

#endif

#ifdef __cplusplus
}
#endif
#endif	/* !_FCNTL_ */
