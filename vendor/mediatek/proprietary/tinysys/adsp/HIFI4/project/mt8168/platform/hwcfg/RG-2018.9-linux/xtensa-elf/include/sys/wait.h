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

#ifndef	_SYS_WAIT_H
#define	_SYS_WAIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#define WNOHANG   1
#define WUNTRACED 2

#define WEXITSTATUS(code)  (((code) >> 8) & 0xff)
#define WIFCONTINUED(code)
#define WIFEXITED(code)    (((code) & 0xff) == 0)
#define WIFSIGNALED(code)  ((((code) & 0x7f) != 0) && (((code) & 0x7f) != 0x7f))
#define WIFSTOPPED(code)   (((code) & 0xff) == 0x7f)
#define WSTOPSIG(code)     (((code) >> 8) & 0xff)
#define WTERMSIG(code)     (((code) & 0x7f)

pid_t  wait(int *);


/* Unimpplemented.  */
#if 0
#include <signal.h>
#include <sys/resource.h>

WEXITED
WSTOPPED
WCONTINUED
WNOHANG
WNOWAIT
idtype_t
P_ALL
P_PID
P_PGID
pid_t  wait3(int *, int, struct rusage *);
int    waitid(idtype_t, id_t, siginfo_t *, int);
pid_t  waitpid(pid_t, int *, int);
#endif


#ifdef __cplusplus
}
#endif
#endif
