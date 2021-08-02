/* stddef.h standard header */
#ifndef _STDDEF
#define _STDDEF
#ifndef _YVALS
 #include <c99/yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
		/* macros */
#ifndef NULL
#if TENSILICA || 1
#ifdef __cplusplus
 #define NULL __null
#else
 #define NULL ((void *)0)
#endif
#else
 #define NULL	_NULL
#endif /* TENSILICA */
#endif /* NULL */

#ifndef offsetof

 #if __EDG__
  #define offsetof(T, member) ((_Sizet)__INTADDR__(&(((T *)0)->member)))

 #elif TENSILICA || 1
  #define offsetof(T, member) __builtin_offsetof (T, member)

 #elif defined(__cplusplus) && ((__GNUC__ == 3) && (3 < __GNUC_MINOR__))
  #define offsetof(T, member) \
	(__offsetof__ (reinterpret_cast<_CSTD size_t> \
	(&reinterpret_cast<char &> \
	(static_cast<T *>(0)->member))))

 #else /* __EDG__ */
  #define offsetof(T, member)	((_CSTD size_t)&(((T *)0)->member))
 #endif /* __EDG__ */

#endif /* offsetof */

		/* type definitions */

 #if !defined(_PTRDIFF_T) && !defined(_PTRDIFFT) \
	&& !defined(_PTRDIFF_T_DEFINED)
  #define _PTRDIFF_T
  #define _PTRDIFFT
  #define _PTRDIFF_T_DEFINED
  #define _STD_USING_PTRDIFF_T
typedef _Ptrdifft ptrdiff_t;
 #endif /* !defined(_PTRDIFF_T) etc. */

 #if !defined(_SIZE_T) && !defined(_SIZET) && !defined(_SIZE_T_DEFINED) \
	&& !defined(_BSD_SIZE_T_DEFINED_)
  #define _SIZE_T
  #define _SIZET
  #define _BSD_SIZE_T_DEFINED_
  #define _SIZE_T_DEFINED
  #define _STD_USING_SIZE_T
typedef _Sizet size_t;
 #endif /* !defined(_SIZE_T) etc. */

 #if !defined(_WCHART) && !defined(_WCHAR_T_DEFINED)
  #define _WCHART
  #define _WCHAR_T_DEFINED
typedef _Wchart wchar_t;
 #endif /* _WCHART etc. */

 #if defined(__need_wint_t) && defined (__CYGWIN__)
  #ifndef _WINTT
   #define _WINTT
typedef _Wintt wint_t;
  #endif /* _WINTT */

 #endif /* defined(__need_wint_t) && defined (__CYGWIN__) */

 #if __STDC_WANT_LIB_EXT1__

  #if !defined(_RSIZE_T_DEFINED)
   #define _RSIZE_T_DEFINED
typedef size_t rsize_t;
  #endif /* _RSIZE_T_DEFINED */

 #endif /* __STDC_WANT_LIB_EXT1__ */

 #ifdef __cplusplus

 #if _HAS_NULLPTR_T
typedef decltype(nullptr) nullptr_t;
 #else /* _HAS_NULLPTR_T */
typedef void *nullptr_t;
 #endif /* _HAS_NULLPTR_T */

 #endif /* __cplusplus */

 #if TENSILICA || 1
struct _max_align_t { char _data; } __attribute__((aligned(1<<_MEMBND)));
typedef struct _max_align_t max_align_t;
 #else
typedef double max_align_t;
 #endif /* TENSILICA */
_C_STD_END
#endif /* _STDDEF */

 #if defined(_STD_USING)

  #ifdef _STD_USING_PTRDIFF_T
using _CSTD ptrdiff_t;
  #endif /* _STD_USING_PTRDIFF_T */

  #ifdef _STD_USING_SIZE_T
using _CSTD size_t;
  #endif /* _STD_USING_SIZE_T */

 #if __STDC_WANT_LIB_EXT1__
using _CSTD rsize_t;

 #ifdef __cplusplus
using _CSTD nullptr_t;
using _CSTD max_align_t;
 #endif /* __cplusplus */

 #endif /* __STDC_WANT_LIB_EXT1__ */
 #endif /* defined(_STD_USING) */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
