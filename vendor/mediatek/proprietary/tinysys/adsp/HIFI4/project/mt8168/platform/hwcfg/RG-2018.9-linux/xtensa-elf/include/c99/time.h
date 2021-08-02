/* time.h standard header */
#ifndef _TIME
#define _TIME
#ifndef _YVALS
 #include <c99/yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
		/* macros */
#ifndef NULL
 #define NULL	_NULL
#endif /* NULL */

#define CLOCKS_PER_SEC	_CPS

		/* type definitions */

 #if !defined(_SIZE_T) && !defined(_SIZET) && !defined(_SIZE_T_DEFINED) \
	&& !defined(_BSD_SIZE_T_DEFINED_)
  #define _SIZE_T
  #define _SIZET
  #define _BSD_SIZE_T_DEFINED_
  #define _SIZE_T_DEFINED
  #define _STD_USING_SIZE_T
typedef _Sizet size_t;
 #endif /* !defined(_SIZE_T) etc. */

 #if !defined(_CLOCK_T) && !defined(__clock_t_defined) \
	&& !defined(_BSD_CLOCK_T_DEFINED_)
  #define _CLOCK_T
  #define __clock_t_defined
  #define _BSD_CLOCK_T_DEFINED_
  #define _STD_USING_CLOCK_T
typedef long clock_t;
 #endif /* !defined(_CLOCK_T) etc. */

 #if !defined(_TIME_T) && !defined(__time_t_defined) \
	&& !defined(_BSD_TIME_T_DEFINED_) && !defined(_TIME_T_DEFINED)
  #define _TIME_T
  #define __time_t_defined
  #define _BSD_TIME_T_DEFINED_
  #define _TIME_T_DEFINED
  #define _STD_USING_TIME_T

typedef long time_t;
 #endif /* !defined(_TIME_T) etc. */

 #ifndef _TM_DEFINED
  #define _TM_DEFINED
struct tm
	{	/* date and time components */
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
	};
 #endif /* _TM_DEFINED */

 #if _HAS_POSIX_C_LIB || defined(__XTENSA__)

  #if defined(__linux) || defined(__linux__) || defined(__CYGWIN__) \
    || defined(__APPLE__) || 0x570 < __SUNPRO_CC || defined(__XTENSA__)

   #if !defined(_STRUCT_TIMESPEC)
    #define _STRUCT_TIMESPEC	1
struct timespec
	{	/* POSIX.4, needed for pthread_cond_timedwait */
	time_t tv_sec;
	long tv_nsec;
	};

    #if defined(__cplusplus) && 1 < _ALT_NS
_C_STD_END
using _CSTD timespec;
_C_STD_BEGIN
    #endif /* defined(__cplusplus) && 1 < _ALT_NS */

   #endif /* !defined(_STRUCT_TIMESPEC) */
  #endif /* defined(__linux) etc. */

  #if !defined(_CLOCKID_T) && !defined(__clockid_t_defined)
   #define _CLOCKID_T
   #define __clockid_t_defined
   #define _STD_USING_CLOCKID_T
typedef int clockid_t;
  #endif /* !defined(_CLOCKID_T) && !defined(__clockid_t_defined) */

 #endif /* _HAS_POSIX_C_LIB */

_EXTERN_C	/* low-level functions */
_CRTIMP2 time_t _CDECL time(time_t *);
_END_EXTERN_C

_C_LIB_DECL	/* declarations */
_CRTIMP2 char *_CDECL asctime(const struct tm *);
_CRTIMP2 clock_t _CDECL clock(void);
_CRTIMP2 char *_CDECL ctime(const time_t *);
_CRTIMP2 double _CDECL difftime(time_t, time_t);
_CRTIMP2 struct tm *_CDECL gmtime(const time_t *);
_CRTIMP2 struct tm *_CDECL localtime(const time_t *);
_CRTIMP2 time_t _CDECL mktime(struct tm *);
_CRTIMP2 size_t _CDECL strftime(char *_Restrict, size_t,
	const char *_Restrict, const struct tm *_Restrict);
_END_C_LIB_DECL

 #if __STDC_WANT_LIB_EXT1__
_C_LIB_DECL

  #if !defined(_ERRNO_T_DEFINED)
   #define _ERRNO_T_DEFINED
typedef int errno_t;
  #endif /* _ERRNO_T_DEFINED */

  #if !defined(_RSIZE_T_DEFINED)
   #define _RSIZE_T_DEFINED
typedef size_t rsize_t;
  #endif /* _RSIZE_T_DEFINED */

_CRTIMP2 errno_t _CDECL asctime_s(char *, rsize_t, const struct tm *);
_CRTIMP2 errno_t _CDECL ctime_s(char *, rsize_t, const time_t *);
_CRTIMP2 struct tm *_CDECL gmtime_s(const time_t *_Restrict,
	struct tm *_Restrict);
_CRTIMP2 struct tm *_CDECL localtime_s(const time_t *_Restrict,
	struct tm *_Restrict);
_END_C_LIB_DECL
 #endif /* __STDC_WANT_LIB_EXT1__ */
_C_STD_END
#endif /* _TIME */

 #if defined(_STD_USING)

  #ifdef _STD_USING_CLOCK_T
using _CSTD clock_t;
  #endif /* _STD_USING_CLOCK_T */

  #ifdef _STD_USING_SIZE_T
using _CSTD size_t;
  #endif /* _STD_USING_SIZE_T */

  #ifdef _STD_USING_TIME_T
using _CSTD time_t;
  #endif /* _STD_USING_TIME_T */

  #ifdef _STD_USING_CLOCKID_T
using _CSTD clockid_t;
  #endif /* _STD_USING_CLOCKID_T */

using _CSTD tm;
using _CSTD asctime; using _CSTD clock; using _CSTD ctime;
using _CSTD difftime; using _CSTD gmtime; using _CSTD localtime;
using _CSTD mktime; using _CSTD strftime; using _CSTD time;

 #if __STDC_WANT_LIB_EXT1__
using _CSTD errno_t;
using _CSTD rsize_t;

using _CSTD asctime_s;
using _CSTD ctime_s;
using _CSTD gmtime_s;
using _CSTD localtime_s;
 #endif /* __STDC_WANT_LIB_EXT1__ */

 #endif /* defined(_STD_USING) */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
