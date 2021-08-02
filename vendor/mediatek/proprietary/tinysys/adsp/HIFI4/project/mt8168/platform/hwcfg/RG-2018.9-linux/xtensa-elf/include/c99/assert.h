/* assert.h standard header */
#ifndef _YVALS
 #include <c99/yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
#undef assert	/* remove existing definition */

#ifdef NDEBUG
 #define assert(test)	((void)0)

#else /* NDEBUG */
  #define _STRIZE(x)	_VAL(x)
  #define _VAL(x)	#x

 #ifdef _WIN32_WCE
  #define _WIDEN(x) _SYSCH(x)

   #define assert(test)	((test) ? (void)0 \
	: _CSTD _Assert(_WIDEN(__FILE__) _WIDEN(":") _WIDEN(_STRIZE(__LINE__)) \
		_WIDEN(" ") _WIDEN(_STRIZE(test))))

_C_LIB_DECL
_CRTIMP2 void _Assert(const _Sysch_t *);
_END_C_LIB_DECL

 #else /* _WIN32_WCE */

 #if 199901L <= __STDC_VERSION__

  #ifdef __cplusplus
   #define _FUNNAME	0

  #else /* __cplusplus */
   #define _FUNNAME	__func__
  #endif /* __cplusplus */

 #else /* 199901L <= __STDC_VERSION__ */
  #define _FUNNAME	0
 #endif /* 199901L <= __STDC_VERSION__ */

  #ifdef _VERBOSE_DEBUGGING
   #include <stdio.h>

   #define assert(test)	((test) ? (void)_CSTD fprintf(stderr, \
	__FILE__ ":" _STRIZE(__LINE__) " " #test " -- OK\n") \
	: _CSTD _Assert(__FILE__ ":" _STRIZE(__LINE__) " " #test, \
		_FUNNAME))

  #else /* _VERBOSE_DEBUGGING */
   #define assert(test)	((test) ? (void)0 \
	: _CSTD _Assert(__FILE__ ":" _STRIZE(__LINE__) " " #test, _FUNNAME))
  #endif /* _VERBOSE_DEBUGGING */

_C_LIB_DECL
_CRTIMP2 void _Assert(const char *, const char *) __attribute__ ((__noreturn__));
_END_C_LIB_DECL
 #endif /* _WIN32_WCE */

 #endif /* NDEBUG */
_C_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
