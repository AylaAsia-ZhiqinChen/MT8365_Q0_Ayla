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

#ifndef	_SIGNAL_H
#define	_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Most of signal.h is way overkill for Xtensa without an operating
   system.  So just implement the minimum amount to allow dinkumware's
   files to compile without warning or error.  */

#define SIGHUP  1
#define SIGINT  2
#define SIGQUIT 3
#define SIGILL  4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGEMT  7
#define SIGFPE  8
#define SIGKILL 9
#define SIGBUS  10
#define SIGSEGV 11
#define SIGSYS  12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15

typedef int sig_atomic_t;
typedef unsigned long sigset_t;

#define SIG_ERR  ((void (*)(int))-1)
#define SIG_DFL  ((void (*)(int))0)
#define SIG_IGN  ((void (*)(int))1)

  /* Unimplemented.  */
#if 0
SIG_HOLD
sigevent
SIGEV_NONE
SIGEV_SIGNAL
SIGEV_THREAD
sigval
SIGRTMIN
SIGRTMAX
SIGTERM
SIGUSR1
SIGUSR2
SIGCHLD
SIGCONT
SIGSTOP
SIGTSTP
SIGTTIN
SIGTTOU
SIGPOLL
SIGPROF
SIGURG
SIGVTALRM
SIGXCPU
SIGXFSZ
sigaction
SA_NOCLDSTOP
SIG_BLOCK
SIG_UNBLOCK
SIG_SETMASK
SA_ONSTACK
SA_RESETHAND
SA_RESTART
SA_SIGINFO
SA_NOCLDWAIT
SA_NODEFER
SS_ONSTACK
SS_DISABLE
MINSIGSTKSZ
SIGSTKSZ
ucontext_t
stack_t
sigstack
siginfo_t
ILL_ILLOPC
ILL_ILLOPN
ILL_ILLADR
ILL_ILLTRP
ILL_PRVOPC
ILL_PRVREG
ILL_COPROC
ILL_BADSTK
FPE_INTDIV
FPE_INTOVF
FPE_FLTDIV
FPE_FLTOVF
FPE_FLTUND
FPE_FLTRES
FPE_FLTINV
FPE_FLTSUB
SEGV_MAPERR
SEGV_ACCERR
BUS_ADRALN
BUS_ADRERR
BUS_OBJERR
TRAP_BRKPT
TRAP_TRACE
CLD_EXITED
CLD_KILLED
CLD_DUMPED
CLD_TRAPPED
CLD_STOPPED
CLD_CONTINUED
POLL_IN
POLL_OUT
POLL_MSG
POLL_ERR
POLL_PRI
POLL_HUP
SI_USER
SI_QUEUE
SI_TIMER
SI_ASYNCIO
SI_MESGQ
void (*bsd_signal(int, void (*)(int)))(int);
int    kill(pid_t, int);
int    killpg(pid_t, int);
int    pthread_kill(pthread_t, int);
int    pthread_sigmask(int, const sigset_t *, sigset_t *);
int    raise(int);
int    sigaction(int, const struct sigaction *, struct sigaction *);
int    sigaddset(sigset_t *, int);
int    sigaltstack(const stack_t *, stack_t *);
int    sigdelset(sigset_t *, int);
int    sigemptyset(sigset_t *);
int    sigfillset(sigset_t *);
int    sighold(int);
int    sigignore(int);
int    siginterrupt(int, int);
int    sigismember(const sigset_t *, int);
void (*signal(int, void (*)(int)))(int);
int    sigpause(int);
int    sigpending(sigset_t *);
int    sigprocmask(int, const sigset_t *, sigset_t *);
int    sigqueue(pid_t, int, const union sigval);
int    sigrelse(int);
void (*sigset(int, void (*)(int)))(int);
int    sigstack(struct sigstack *ss,
           struct sigstack *oss); (LEGACY)
int    sigsuspend(const sigset_t *);
int    sigtimedwait(const sigset_t *, siginfo_t *,
           const struct timespec *);
int    sigwait(const sigset_t *set, int *sig);
int    sigwaitinfo(const sigset_t *, siginfo_t *);

#endif

#ifdef __cplusplus
}
#endif
#endif
