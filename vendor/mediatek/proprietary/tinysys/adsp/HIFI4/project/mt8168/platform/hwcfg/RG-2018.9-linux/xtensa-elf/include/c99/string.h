/* string.h standard header */
#ifndef _STRING
#define _STRING

#ifndef _YVALS
 #include <c99/yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
		/* macros */
#ifndef NULL
 #define NULL	_NULL
#endif /* NULL */

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

		/* declarations */
_C_LIB_DECL
_CRTIMP2 int _CDECL memcmp(const void *, const void *, size_t);
_CRTIMP2 void *_CDECL memcpy(void *_Restrict, const void *_Restrict, size_t);
_CRTIMP2 void *_CDECL memset(void *, int, size_t);
_CRTIMP2 char *_CDECL strcat(char *_Restrict, const char *_Restrict);
#include <xtensa/tie/xt_scmpr.h>
#ifndef __VECTOR_STRCMP
_CRTIMP2 int _CDECL strcmp(const char *, const char *);
#endif
_CRTIMP2 char *_CDECL strcpy(char *_Restrict, const char *_Restrict);
_CRTIMP2 size_t _CDECL strlen(const char *);
_CRTIMP2 size_t _CDECL strnlen(const char *, size_t);

_CRTIMP2 void *_CDECL memmove(void *, const void *, size_t);
_CRTIMP2 int _CDECL strcoll(const char *, const char *);
_CRTIMP2 size_t _CDECL strcspn(const char *, const char *);
_CRTIMP2 char *_CDECL strerror(int);
_CRTIMP2 char *_CDECL strncat(char *_Restrict, const char *_Restrict, size_t);
_CRTIMP2 int _CDECL strncmp(const char *, const char *, size_t);
_CRTIMP2 char *_CDECL strncpy(char *_Restrict, const char *_Restrict, size_t);
_CRTIMP2 size_t _CDECL strspn(const char *, const char *);
_CRTIMP2 char *_CDECL strtok(char *_Restrict, const char *_Restrict);
_CRTIMP2 size_t _CDECL strxfrm(char *_Restrict, const char *_Restrict, size_t);

 #if _ADDED_C_LIB
_CRTIMP2 char *strdup(const char *);
_CRTIMP2 int strcasecmp(const char *, const char *);
_CRTIMP2 int strncasecmp(const char *, const char *, size_t);
_CRTIMP2 char *strtok_r(char *, const char *, char **);
 #endif /* _ADDED_C_LIB */

_END_C_LIB_DECL

 #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
		// INLINES AND OVERLOADS, for C++
 #define _Const_return const

_C_LIB_DECL
_CRTIMP2 const char *_CDECL strchr(const char *, int);
_CRTIMP2 const char *_CDECL strpbrk(const char *, const char *);
_CRTIMP2 const char *_CDECL strrchr(const char *, int);
_CRTIMP2 const char *_CDECL strstr(const char *, const char *);
_END_C_LIB_DECL

extern "C++" {
inline char *_CDECL strchr(char *_Str, int _Ch)
	{	// call with const first argument
	return ((char *)_CSTD strchr((const char *)_Str, _Ch));
	}

inline char *_CDECL strpbrk(char *_Str1, const char *_Str2)
	{	// call with const first argument
	return ((char *)_CSTD strpbrk((const char *)_Str1, _Str2));
	}

inline char *_CDECL strrchr(char *_Str, int _Ch)
	{	// call with const first argument
	return ((char *)_CSTD strrchr((const char *)_Str, _Ch));
	}

inline char *_CDECL strstr(char *_Str1, const char *_Str2)
	{	// call with const first argument
	return ((char *)_CSTD strstr((const char *)_Str1, _Str2));
	}
}	// extern "C++"

 #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES)*/
_C_LIB_DECL
 #define _Const_return

_CRTIMP2 char *_CDECL strchr(const char *, int);
_CRTIMP2 char *_CDECL strpbrk(const char *, const char *);
_CRTIMP2 char *_CDECL strrchr(const char *, int);
_CRTIMP2 char *_CDECL strstr(const char *, const char *);
_END_C_LIB_DECL
 #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */

 #ifndef _MEMCHR
  #define _MEMCHR

  #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
_C_LIB_DECL
_CRTIMP2 const void *_CDECL memchr(const void *, int, size_t);
_END_C_LIB_DECL

extern "C++" {
inline void *_CDECL memchr(void *_Str, int _Ch, size_t _Num)
	{	// call with const first argument
	return ((void *)_CSTD memchr((const void *)_Str, _Ch, _Num));
	}
}	// extern "C++"

  #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
_C_LIB_DECL
_CRTIMP2 void *_CDECL memchr(const void *, int, size_t);
_END_C_LIB_DECL
  #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */

 #endif /* _MEMCHR */

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

_CRTIMP2 errno_t _CDECL memcpy_s(void *_Restrict, rsize_t,
	const void *_Restrict, rsize_t);
_CRTIMP2 errno_t _CDECL memmove_s(void *, rsize_t,
	const void *, rsize_t);
_CRTIMP2 errno_t _CDECL memset_s(void *, rsize_t,
	int, rsize_t);

_CRTIMP2 errno_t _CDECL strcpy_s(char *_Restrict, rsize_t,
	const char *_Restrict);
_CRTIMP2 errno_t _CDECL strncpy_s(char *_Restrict, rsize_t,
	const char *_Restrict, rsize_t);
_CRTIMP2 errno_t _CDECL strcat_s(char *_Restrict, rsize_t,
	const char *_Restrict);
_CRTIMP2 errno_t _CDECL strncat_s(char *_Restrict, rsize_t,
	const char *_Restrict, rsize_t);
_CRTIMP2 char *_CDECL strtok_s(char *_Restrict, rsize_t *_Restrict,
	const char *_Restrict, char **_Restrict);

_CRTIMP2 errno_t _CDECL strerror_s(char *, rsize_t, errno_t);
_CRTIMP2 size_t _CDECL strerrorlen_s(errno_t);

_CRTIMP2 size_t _CDECL strnlen_s(const char *_Restrict, size_t);
_END_C_LIB_DECL
 #endif /* __STDC_WANT_LIB_EXT1__ */
_C_STD_END
#endif /* _STRING */

 #if defined(_STD_USING)

  #ifdef _STD_USING_SIZE_T
using _CSTD size_t;
  #endif /* _STD_USING_SIZE_T */

using _CSTD memchr; using _CSTD memcmp;
using _CSTD memcpy; using _CSTD memmove; using _CSTD memset;
using _CSTD strcat; using _CSTD strchr; using _CSTD strcmp;
using _CSTD strcoll; using _CSTD strcpy; using _CSTD strcspn;
using _CSTD strerror; using _CSTD strlen; using _CSTD strncat;
using _CSTD strncmp; using _CSTD strncpy; using _CSTD strpbrk;
using _CSTD strrchr; using _CSTD strspn; using _CSTD strstr;
using _CSTD strtok; using _CSTD strxfrm;

 #if _ADDED_C_LIB
using _CSTD strdup; using _CSTD strcasecmp;
using _CSTD strncasecmp; using _CSTD strtok_r;
 #endif /* _ADDED_C_LIB */

 #if __STDC_WANT_LIB_EXT1__
using _CSTD errno_t;
using _CSTD rsize_t;

using _CSTD memcpy_s;
using _CSTD memmove_s;
using _CSTD memset_s;
using _CSTD strcpy_s;
using _CSTD strncpy_s;
using _CSTD strcat_s;
using _CSTD strncat_s;
using _CSTD strtok_s;
using _CSTD strerror_s;
using _CSTD strerrorlen_s;
using _CSTD strnlen_s;
 #endif /* __STDC_WANT_LIB_EXT1__ */

 #endif /* defined(_STD_USING) */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
