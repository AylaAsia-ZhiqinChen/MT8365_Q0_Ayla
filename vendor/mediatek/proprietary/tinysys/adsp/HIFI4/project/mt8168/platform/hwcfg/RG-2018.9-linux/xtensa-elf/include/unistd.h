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

#ifndef _UNISTD_H
#define _UNISTD_H

#if __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <stdint.h>

/* Constants for access  */
#define F_OK (0x0)
#define X_OK (0x1)
#define W_OK (0x2)
#define R_OK (0x4)

/* Constants for lseek, fcntl  */
#ifndef SEEK_SET
#define SEEK_SET (0x0)
#define SEEK_CUR (0x1)
#define SEEK_END (0x2)
#endif

/* Constants for identifying file streams. */
#define STDIN_FILENO  (0x0)
#define STDOUT_FILENO (0x1)
#define STDERR_FILENO (0x2)

/* Prototypes for implemented functions.  */
int          close(int);
void        _exit(int);
pid_t        getpid(void);
int          link(const char *, const char *);
off_t        lseek(int, off_t, int);
ssize_t      read(int, void *, size_t);
int          rmdir(const char *);
void        *sbrk(intptr_t);
int          unlink(const char *);
ssize_t      write(int, const void *, size_t);

extern char   *optarg;
extern int    optind, opterr, optopt;

/* ****************************************************
 * Unsupported unistd.h features
 */

#if 0

/* Unsupported POSIX version related stuff. */
    _POSIX_VERSION
    _POSIX2_VERSION
    _POSIX2_C_VERSION
    _XOPEN_VERSION
    _POSIX_VERSION
    _POSIX2_VERSION
    _POSIX2_C_VERSION
    _XOPEN_VERSION
    _XOPEN_XCU_VERSION
    _POSIX2_C_BIND
    _POSIX2_C_VERSION
    _POSIX2_CHAR_TERM
    _POSIX2_LOCALEDEF
    _POSIX2_UPE
    _POSIX2_VERSION

    /* Macros and definitions that describe whether a given feature is
       present. Define if you implement one of these. */
    _XOPEN_XPG2
    _XOPEN_XPG3
    _XOPEN_XPG4
    _XOPEN_UNIX
    _POSIX_CHOWN_RESTRICTED
    _POSIX_NO_TRUNC
    _POSIX_VDISABLE
    _POSIX_SAVED_IDS
    _POSIX_JOB_CONTROL

    _POSIX_CHOWN_RESTRICTED
    _POSIX_NO_TRUNC
    _POSIX_VDISABLE

    _POSIX_THREADS
    _POSIX_THREAD_ATTR_STACKADDR
    _POSIX_THREAD_ATTR_STACKSIZE
    _POSIX_THREAD_PROCESS_SHARED
    _POSIX_THREAD_SAFE_FUNCTIONS

    _POSIX2_C_BIND
    _POSIX2_C_DEV
    _POSIX2_CHAR_TERM
    _POSIX2_FORT_DEV
    _POSIX2_FORT_RUN
    _POSIX2_LOCALEDEF
    _POSIX2_SW_DEV
    _POSIX2_UPE
    _XOPEN_CRYPT
    _XOPEN_ENH_I18N
    _XOPEN_LEGACY
    _XOPEN_REALTIME
    _XOPEN_REALTIME_THREADS
    _XOPEN_SHM
    _XBS5_ILP32_OFF32
    _XBS5_ILP32_OFFBIG
    _XBS5_LP64_OFF64
    _XBS5_LPBIG_OFFBIG

    _POSIX_ASYNCHRONOUS_IO
    _POSIX_MEMLOCK
    _POSIX_MEMLOCK_RANGE
    _POSIX_MESSAGE_PASSING
    _POSIX_PRIORITY_SCHEDULING
    _POSIX_REALTIME_SIGNALS
    _POSIX_SEMAPHORES
    _POSIX_SHARED_MEMORY_OBJECTS
    _POSIX_SYNCHRONIZED_IO
    _POSIX_TIMERS

    _POSIX_FSYNC
    _POSIX_MAPPED_FILES
    _POSIX_MEMORY_PROTECTION

    _POSIX_PRIORITIZED_IO
    _XOPEN_REALTIME_THREADS

    _POSIX_THREAD_PRIORITY_SCHEDULING
    _POSIX_THREAD_PRIO_INHERIT
    _POSIX_THREAD_PRIO_PROTECT

    _POSIX_ASYNC_IO
    _POSIX_PRIO_IO
    _POSIX_SYNC_IO
     NULL /* defined for dinkum libc in _yvals.h  */

    _CS_PATH
    _CS_XBS5_ILP32_OFF32_CFLAGS
    _CS_XBS5_ILP32_OFF32_LDFLAGS
    _CS_XBS5_ILP32_OFF32_LIBS
    _CS_XBS5_ILP32_OFF32_LINTFLAGS
    _CS_XBS5_ILP32_OFFBIG_CFLAGS
    _CS_XBS5_ILP32_OFFBIG_LDFLAGS
    _CS_XBS5_ILP32_OFFBIG_LIBS
    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS
    _CS_XBS5_LP64_OFF64_CFLAGS
    _CS_XBS5_LP64_OFF64_LDFLAGS
    _CS_XBS5_LP64_OFF64_LIBS
    _CS_XBS5_LP64_OFF64_LINTFLAGS
    _CS_XBS5_LPBIG_OFFBIG_CFLAGS
    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS
    _CS_XBS5_LPBIG_OFFBIG_LIBS
    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS

    _SC_2_C_BIND
    _SC_2_C_DEV
    _SC_2_C_VERSION
    _SC_2_FORT_DEV
    _SC_2_FORT_RUN
    _SC_2_LOCALEDEF
    _SC_2_SW_DEV
    _SC_2_UPE
    _SC_2_VERSION
    _SC_ARG_MAX
    _SC_AIO_LISTIO_MAX
    _SC_AIO_MAX
    _SC_AIO_PRIO_DELTA_MAX
    _SC_ASYNCHRONOUS_IO
    _SC_ATEXIT_MAX
    _SC_BC_BASE_MAX
    _SC_BC_DIM_MAX
    _SC_BC_SCALE_MAX
    _SC_BC_STRING_MAX
    _SC_CHILD_MAX
    _SC_CLK_TCK
    _SC_COLL_WEIGHTS_MAX
    _SC_DELAYTIMER_MAX
    _SC_EXPR_NEST_MAX
    _SC_FSYNC
    _SC_GETGR_R_SIZE_MAX
    _SC_GETPW_R_SIZE_MAX
    _SC_IOV_MAX
    _SC_JOB_CONTROL
    _SC_LINE_MAX
    _SC_LOGIN_NAME_MAX
    _SC_MAPPED_FILES
    _SC_MEMLOCK
    _SC_MEMLOCK_RANGE
    _SC_MEMORY_PROTECTION
    _SC_MESSAGE_PASSING
    _SC_MQ_OPEN_MAX
    _SC_MQ_PRIO_MAX
    _SC_NGROUPS_MAX
    _SC_OPEN_MAX
    _SC_PAGESIZE
    _SC_PAGE_SIZE
    _SC_PASS_MAX (LEGACY)
    _SC_PRIORITIZED_IO
    _SC_PRIORITY_SCHEDULING
    _SC_RE_DUP_MAX
    _SC_REALTIME_SIGNALS
    _SC_RTSIG_MAX
    _SC_SAVED_IDS
    _SC_SEMAPHORES
    _SC_SEM_NSEMS_MAX
    _SC_SEM_VALUE_MAX
    _SC_SHARED_MEMORY_OBJECTS
    _SC_SIGQUEUE_MAX
    _SC_STREAM_MAX
    _SC_SYNCHRONIZED_IO
    _SC_THREADS
    _SC_THREAD_ATTR_STACKADDR
    _SC_THREAD_ATTR_STACKSIZE
    _SC_THREAD_DESTRUCTOR_ITERATIONS
    _SC_THREAD_KEYS_MAX
    _SC_THREAD_PRIORITY_SCHEDULING
    _SC_THREAD_PRIO_INHERIT
    _SC_THREAD_PRIO_PROTECT
    _SC_THREAD_PROCESS_SHARED
    _SC_THREAD_SAFE_FUNCTIONS
    _SC_THREAD_STACK_MIN
    _SC_THREAD_THREADS_MAX
    _SC_TIMERS
    _SC_TIMER_MAX
    _SC_TTY_NAME_MAX
    _SC_TZNAME_MAX
    _SC_VERSION
    _SC_XOPEN_VERSION
    _SC_XOPEN_CRYPT
    _SC_XOPEN_ENH_I18N
    _SC_XOPEN_SHM
    _SC_XOPEN_UNIX
    _SC_XOPEN_XCU_VERSION
    _SC_XOPEN_LEGACY
    _SC_XOPEN_REALTIME
    _SC_XOPEN_REALTIME_THREADS
    _SC_XBS5_ILP32_OFF32
    _SC_XBS5_ILP32_OFFBIG
    _SC_XBS5_LP64_OFF64
    _SC_XBS5_LPBIG_OFFBIG

    F_LOCK
    F_ULOCK
    F_TEST
    F_TLOCK

    _PC_ASYNC_IO
    _PC_CHOWN_RESTRICTED
    _PC_FILESIZEBITS
    _PC_LINK_MAX
    _PC_MAX_CANON
    _PC_MAX_INPUT
    _PC_NAME_MAX
    _PC_NO_TRUNC
    _PC_PATH_MAX
    _PC_PIPE_BUF
    _PC_PRIO_IO
    _PC_SYNC_IO
    _PC_VDISABLE

     Type Definitions

    The useconds_t type is defined as described in <sys/types.h>.

    The intptr_t type is defined as described in <inttypes.h>.

/* Prototypes for unimplemented functions.  */
int          access(const char *, int);
unsigned int alarm(unsigned int);
int          brk(void *);
int          chdir(const char *);
int          chroot(const char *); (LEGACY)
int          chown(const char *, uid_t, gid_t);
size_t       confstr(int, char *, size_t);
char        *crypt(const char *, const char *);
char        *ctermid(char *);
char        *cuserid(char *s); (LEGACY)
int          dup(int);
int          dup2(int, int);
void         encrypt(char[64], int);
int          execl(const char *, const char *, ...);
int          execle(const char *, const char *, ...);
int          execlp(const char *, const char *, ...);
int          execv(const char *, char *const []);
int          execve(const char *, char *const [], char *const []);
int          execvp(const char *, char *const []);
int          fchown(int, uid_t, gid_t);
int          fchdir(int);
int          fdatasync(int);
pid_t        fork(void);
long int     fpathconf(int, int);
int          fsync(int);
int          ftruncate(int, off_t);
char        *getcwd(char *, size_t);
int          getdtablesize(void); (LEGACY)
gid_t        getegid(void);
uid_t        geteuid(void);
gid_t        getgid(void);
int          getgroups(int, gid_t []);
long         gethostid(void);
char        *getlogin(void);
int          getlogin_r(char *, size_t);
int          getopt(int, char * const [], const char *);
int          getpagesize(void); (LEGACY)
char        *getpass(const char *); (LEGACY)
pid_t        getpgid(pid_t);
pid_t        getpgrp(void);
pid_t        getppid(void);
pid_t        getsid(pid_t);
uid_t        getuid(void);
char        *getwd(char *);
int          isatty(int);
int          lchown(const char *, uid_t, gid_t);
int          lockf(int, int, off_t);
int          nice(int);
long int     pathconf(const char *, int);
int          pause(void);
int          pipe(int [2]);
ssize_t      pread(int, void *, size_t, off_t);
int          pthread_atfork(void (*)(void), void (*)(void),
			    void(*)(void));
ssize_t      pwrite(int, const void *, size_t, off_t);
ssize_t      read(int, void *, size_t);
int          readlink(const char *, char *, size_t);
int          setgid(gid_t);
int          setpgid(pid_t, pid_t);
pid_t        setpgrp(void);
int          setregid(gid_t, gid_t);
int          setreuid(uid_t, uid_t);
pid_t        setsid(void);
int          setuid(uid_t);
unsigned int sleep(unsigned int);
void         swab(const void *, void *, ssize_t);
int          symlink(const char *, const char *);
void         sync(void);
long int     sysconf(int);
pid_t        tcgetpgrp(int);
int          tcsetpgrp(int, pid_t);
int          truncate(const char *, off_t);
char        *ttyname(int);
int          ttyname_r(int, char *, size_t);
useconds_t   ualarm(useconds_t, useconds_t);
int          usleep(useconds_t);
pid_t        vfork(void);
#endif



#if __cplusplus
}
#endif

#endif
