/* wchar.h standard header */
#ifndef _WCHAR
#define _WCHAR
#ifndef _YVALS
 #include <c99/yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
		/* MACROS */
#ifndef NULL
 #define NULL 	_NULL
#endif /* NULL */

#define WCHAR_MIN	_WCMIN
#define WCHAR_MAX	_WCMAX
#define WEOF	((_CSTD wint_t)(-1))

#ifndef _Mbstinit

 #ifdef __cplusplus
#define _Mbstinit(x)	_Mbstatet x

 #else /* __cplusplus */
#define _Mbstinit(x)	_Mbstatet x = {0}
 #endif /* __cplusplus */

#endif /* _Mbstinit */

		/* TYPE DEFINITIONS */
#ifndef _MBSTATET
#include <mbstatet.h>
#endif /* _MBSTATET */

 #if !defined(_SIZE_T) && !defined(_SIZET) && !defined(_SIZE_T_DEFINED) \
	&& !defined(_BSD_SIZE_T_DEFINED_)
  #define _SIZE_T
  #define _SIZET
  #define _BSD_SIZE_T_DEFINED_
  #define _SIZE_T_DEFINED
  #define _STD_USING_SIZE_T
typedef _Sizet size_t;
 #endif /* !defined(_SIZE_T) etc. */

struct tm;
struct _Dnk_filet;

 #ifndef _FILET
  #define _FILET
typedef struct _Dnk_filet _Filet;
 #endif /* _FILET */

 #if !defined(_WCHART) && !defined(_WCHAR_T_DEFINED)
  #define _WCHART
  #define _WCHAR_T_DEFINED
typedef _Wchart wchar_t;
 #endif /* _WCHART etc. */

 #if 1200 <= _MSC_VER
 #ifndef _WCTYPE_T_DEFINED
  #define _WCTYPE_T_DEFINED
  #ifndef _WCTYPET
   #define _WCTYPET
typedef _Sizet wctype_t;
  #endif /* _WCTYPET */

 #ifndef _WINTT
  #define _WINTT
typedef _Wintt wint_t;
 #endif /* _WINTT */

 #endif /* _WCTYPE_T_DEFINED */

 #else /* 1200 <= _MSC_VER */
 #ifndef _WINTT
  #define _WINTT
typedef _Wintt wint_t;
 #endif /* _WINTT */

 #endif /* 1200 <= _MSC_VER */

_C_LIB_DECL
		/* stdio DECLARATIONS */
_CRTIMP2 wint_t _CDECL fgetwc(_Filet *);
_CRTIMP2 wchar_t *_CDECL fgetws(wchar_t *_Restrict, int,
	_Filet *_Restrict);
_CRTIMP2 wint_t _CDECL fputwc(wchar_t, _Filet *);
_CRTIMP2 int _CDECL fputws(const wchar_t *_Restrict,
	_Filet *_Restrict);
_CRTIMP2 int _CDECL fwide(_Filet *, int);
_CRTIMP2 int _CDECL fwprintf(_Filet *_Restrict,
	const wchar_t *_Restrict, ...);
_CRTIMP2 int _CDECL fwscanf(_Filet *_Restrict,
	const wchar_t *_Restrict, ...);
_CRTIMP2 wint_t _CDECL getwc(_Filet *);
_CRTIMP2 wint_t _CDECL getwchar(void);
_CRTIMP2 wint_t _CDECL putwc(wchar_t, _Filet *);
_CRTIMP2 wint_t _CDECL putwchar(wchar_t);
_CRTIMP2 int _CDECL swprintf(wchar_t *_Restrict, size_t,
	const wchar_t *_Restrict, ...);
_CRTIMP2 int _CDECL swscanf(const wchar_t *_Restrict,
	const wchar_t *_Restrict, ...);
_CRTIMP2 wint_t _CDECL ungetwc(wint_t, _Filet *);
_CRTIMP2 int _CDECL vfwprintf(_Filet *_Restrict,
	const wchar_t *_Restrict, _Va_list);
_CRTIMP2 int _CDECL vswprintf(wchar_t *_Restrict, size_t,
	const wchar_t *_Restrict, _Va_list);
_CRTIMP2 int _CDECL vwprintf(const wchar_t *_Restrict, _Va_list);
_CRTIMP2 int _CDECL wprintf(const wchar_t *_Restrict, ...);
_CRTIMP2 int _CDECL wscanf(const wchar_t *_Restrict, ...);

 #if _HAS_C9X
_CRTIMP2 int _CDECL vfwscanf(_Filet *_Restrict,
	const wchar_t *_Restrict, _Va_list);
_CRTIMP2 int _CDECL vswscanf(const wchar_t *_Restrict,
	const wchar_t *_Restrict, _Va_list);
_CRTIMP2 int _CDECL vwscanf(const wchar_t *_Restrict, _Va_list);
 #endif /* _IS_C9X */

		/* stdlib DECLARATIONS */
_CRTIMP2 size_t _CDECL mbrlen(const char *_Restrict,
	size_t, mbstate_t *_Restrict);
_CRTIMP2 size_t _CDECL mbrtowc(wchar_t *_Restrict, const char *,
	size_t, mbstate_t *_Restrict);
_CRTIMP2 size_t _CDECL mbsrtowcs(wchar_t *_Restrict,
	const char **_Restrict, size_t, mbstate_t *_Restrict);
_CRTIMP2 int _CDECL mbsinit(const mbstate_t *);
_CRTIMP2 size_t _CDECL wcrtomb(char *_Restrict,
	wchar_t, mbstate_t *_Restrict);
_CRTIMP2 size_t _CDECL wcsrtombs(char *_Restrict,
	const wchar_t **_Restrict, size_t, mbstate_t *_Restrict);
_CRTIMP2 long _CDECL wcstol(const wchar_t *_Restrict,
	wchar_t **_Restrict, int);

 #if _HAS_C9X
_CRTIMP2 _Longlong _CDECL wcstoll(const wchar_t *_Restrict,
	wchar_t **_Restrict, int);
_CRTIMP2 _ULonglong _CDECL wcstoull(const wchar_t *_Restrict,
	wchar_t **_Restrict, int);
 #endif /* _IS_C9X */

		/* string DECLARATIONS */
_CRTIMP2 wchar_t *_CDECL wcscat(wchar_t *_Restrict, const wchar_t *_Restrict);
_CRTIMP2 int _CDECL wcscmp(const wchar_t *, const wchar_t *);
_CRTIMP2 wchar_t *_CDECL wcscpy(wchar_t *_Restrict, const wchar_t *_Restrict);
_CRTIMP2 size_t _CDECL wcslen(const wchar_t *);
_CRTIMP2 int _CDECL wcsncmp(const wchar_t *, const wchar_t *, size_t);
_CRTIMP2 wchar_t *_CDECL wcsncpy(wchar_t *_Restrict,
	const wchar_t *_Restrict, size_t);

_CRTIMP2 int _CDECL wcscoll(const wchar_t *, const wchar_t *);
_CRTIMP2 size_t _CDECL wcscspn(const wchar_t *, const wchar_t *);
_CRTIMP2 wchar_t *_CDECL wcsncat(wchar_t *_Restrict,
	const wchar_t *_Restrict, size_t);
_CRTIMP2 size_t _CDECL wcsspn(const wchar_t *, const wchar_t *);
_CRTIMP2 wchar_t *_CDECL wcstok(wchar_t *_Restrict, const wchar_t *_Restrict,
	wchar_t **_Restrict);
_CRTIMP2 size_t _CDECL wcsxfrm(wchar_t *_Restrict,
	const wchar_t *_Restrict, size_t);
_CRTIMP2 int _CDECL wmemcmp(const wchar_t *, const wchar_t *, size_t);
_CRTIMP2 wchar_t *_CDECL wmemcpy(wchar_t *_Restrict,
	const wchar_t *_Restrict, size_t);
_CRTIMP2 wchar_t *_CDECL wmemmove(wchar_t *, const wchar_t *, size_t);
_CRTIMP2 wchar_t *_CDECL wmemset(wchar_t *, wchar_t, size_t);

		/* time DECLARATIONS */
_CRTIMP2 size_t _CDECL wcsftime(wchar_t *_Restrict, size_t,
	const wchar_t *_Restrict, const struct tm *_Restrict);

_CRTIMP2 wint_t _CDECL _Btowc(int);
_CRTIMP2 int _CDECL _Wctob(wint_t);
_CRTIMP2 double _CDECL _WStod(const wchar_t *, wchar_t **, long);
_CRTIMP2 float _CDECL _WStof(const wchar_t *, wchar_t **, long);
_CRTIMP2 long double _CDECL _WStold(const wchar_t *, wchar_t **, long);
_CRTIMP2 unsigned long _CDECL _WStoul(const wchar_t *, wchar_t **, int);
_END_C_LIB_DECL

 #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
		// INLINES AND OVERLOADS, FOR C++
  #define _WConst_return const

_C_LIB_DECL
_CRTIMP2 const wchar_t *_CDECL wmemchr(const wchar_t *, wchar_t, size_t);
_END_C_LIB_DECL

extern "C++" {
inline wchar_t *_CDECL wmemchr(wchar_t *_Str, wchar_t _Ch, size_t _Num)
	{	// call with const first argument
	return ((wchar_t *)wmemchr((const wchar_t *)_Str, _Ch, _Num));
	}
}	// extern "C++"

 #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
  #define _WConst_return

_C_LIB_DECL
_CRTIMP2 wchar_t *_CDECL wmemchr(const wchar_t *, wchar_t, size_t);
_END_C_LIB_DECL
 #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */

 #include <xwcstod.h>
 #include <xwstr.h>

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

_CRTIMP2 int _CDECL fwprintf_s(_Filet *_Restrict,
	const wchar_t *_Restrict, ...);
_CRTIMP2 int _CDECL fwscanf_s(_Filet *_Restrict,
	const wchar_t *_Restrict, ...);
_CRTIMP2 int _CDECL snwprintf_s(wchar_t *_Restrict, rsize_t,
	const wchar_t *_Restrict, ...);
_CRTIMP2 int _CDECL swprintf_s(wchar_t *_Restrict, rsize_t,
	const wchar_t *_Restrict, ...);
_CRTIMP2 int _CDECL swscanf_s(const wchar_t *_Restrict,
	const wchar_t *_Restrict, ...);
_CRTIMP2 int _CDECL vfwprintf_s(_Filet *_Restrict,
	const wchar_t *_Restrict,
	_Va_list);
_CRTIMP2 int _CDECL vfwscanf_s(_Filet *_Restrict,
	const wchar_t *_Restrict,
	_Va_list);
_CRTIMP2 int _CDECL vsnwprintf_s(wchar_t *_Restrict, rsize_t,
	const wchar_t *_Restrict,
	_Va_list);
_CRTIMP2 int _CDECL vswprintf_s(wchar_t *_Restrict, rsize_t,
	const wchar_t *_Restrict,
	_Va_list);
_CRTIMP2 int _CDECL vswscanf_s(const wchar_t *_Restrict,
	const wchar_t *_Restrict,
	_Va_list);
_CRTIMP2 int _CDECL vwprintf_s(const wchar_t *_Restrict,
	_Va_list);
_CRTIMP2 int _CDECL vwscanf_s(const wchar_t *_Restrict,
	_Va_list);
_CRTIMP2 int _CDECL wprintf_s(const wchar_t *_Restrict, ...);
_CRTIMP2 int _CDECL wscanf_s(const wchar_t *_Restrict, ...);

_CRTIMP2 errno_t _CDECL wcscpy_s(wchar_t *_Restrict, rsize_t,
	const wchar_t *_Restrict);
_CRTIMP2 errno_t _CDECL wcsncpy_s(wchar_t *_Restrict, rsize_t,
	const wchar_t *_Restrict, rsize_t);
_CRTIMP2 errno_t _CDECL wmemcpy_s(wchar_t *_Restrict, rsize_t,
	const wchar_t *_Restrict, rsize_t);
_CRTIMP2 errno_t _CDECL wmemmove_s(wchar_t *, rsize_t,
	const wchar_t *, rsize_t);
_CRTIMP2 errno_t _CDECL wcscat_s(wchar_t *_Restrict, rsize_t,
	const wchar_t *_Restrict);
_CRTIMP2 errno_t _CDECL wcsncat_s(wchar_t *_Restrict, rsize_t,
	const wchar_t *_Restrict, rsize_t);
_CRTIMP2 wchar_t *_CDECL wcstok_s(wchar_t *_Restrict, rsize_t *_Restrict,
	const wchar_t *_Restrict, wchar_t **_Restrict);

_CRTIMP2 size_t _CDECL wcsnlen_s(const wchar_t *, size_t);

_CRTIMP2 errno_t _CDECL wcrtomb_s(size_t *_Restrict,
	char *_Restrict, rsize_t,
	wchar_t,
	mbstate_t *_Restrict);
_CRTIMP2 errno_t _CDECL mbsrtowcs_s(size_t *_Restrict,
	wchar_t *_Restrict, rsize_t,
	const char **_Restrict, rsize_t,
	mbstate_t *_Restrict);
_CRTIMP2 errno_t _CDECL wcsrtombs_s(size_t *_Restrict,
	char *_Restrict, rsize_t,
	const wchar_t **_Restrict, rsize_t,
	mbstate_t *_Restrict);
_END_C_LIB_DECL
 #endif /* __STDC_WANT_LIB_EXT1__ */
_C_STD_END
#endif /* _WCHAR */

 #if defined(_STD_USING)

  #ifdef _STD_USING_SIZE_T
using _CSTD size_t;
  #endif /* _STD_USING_SIZE_T */

using _CSTD mbstate_t; using _CSTD tm; using _CSTD wint_t;

using _CSTD btowc; using _CSTD fgetwc; using _CSTD fgetws; using _CSTD fputwc;
using _CSTD fputws; using _CSTD fwide; using _CSTD fwprintf;
using _CSTD fwscanf; using _CSTD getwc; using _CSTD getwchar;
using _CSTD mbrlen; using _CSTD mbrtowc; using _CSTD mbsrtowcs;
using _CSTD mbsinit; using _CSTD putwc; using _CSTD putwchar;
using _CSTD swprintf; using _CSTD swscanf; using _CSTD ungetwc;
using _CSTD vfwprintf; using _CSTD vswprintf; using _CSTD vwprintf;
using _CSTD wcrtomb; using _CSTD wprintf; using _CSTD wscanf;
using _CSTD wcsrtombs; using _CSTD wcstol; using _CSTD wcscat;
using _CSTD wcschr; using _CSTD wcscmp; using _CSTD wcscoll;
using _CSTD wcscpy; using _CSTD wcscspn; using _CSTD wcslen;
using _CSTD wcsncat; using _CSTD wcsncmp; using _CSTD wcsncpy;
using _CSTD wcspbrk; using _CSTD wcsrchr; using _CSTD wcsspn;
using _CSTD wcstod; using _CSTD wcstoul; using _CSTD wcsstr;
using _CSTD wcstok; using _CSTD wcsxfrm; using _CSTD wctob;
using _CSTD wmemchr; using _CSTD wmemcmp; using _CSTD wmemcpy;
using _CSTD wmemmove; using _CSTD wmemset; using _CSTD wcsftime;

 #if _HAS_C9X
using _CSTD vfwscanf; using _CSTD vswscanf; using _CSTD vwscanf;
using _CSTD wcstof; using _CSTD wcstold;
using _CSTD wcstoll; using _CSTD wcstoull;
 #endif /* _IS_C9X */

 #if __STDC_WANT_LIB_EXT1__
using _CSTD errno_t;
using _CSTD rsize_t;

using _CSTD fwprintf_s;
using _CSTD fwscanf_s;
using _CSTD snwprintf_s;
using _CSTD swprintf_s;
using _CSTD swscanf_s;
using _CSTD vfwprintf_s;
using _CSTD vfwscanf_s;
using _CSTD vsnwprintf_s;
using _CSTD vswprintf_s;
using _CSTD vswscanf_s;
using _CSTD vwprintf_s;
using _CSTD vwscanf_s;
using _CSTD wprintf_s;
using _CSTD wscanf_s;

using _CSTD wcscpy_s;
using _CSTD wcsncpy_s;
using _CSTD wmemcpy_s;
using _CSTD wmemmove_s;
using _CSTD wcscat_s;
using _CSTD wcsncat_s;
using _CSTD wcstok_s;

using _CSTD wcsnlen_s;

using _CSTD wcrtomb_s;
using _CSTD mbsrtowcs_s;
using _CSTD wcsrtombs_s;
 #endif /* __STDC_WANT_LIB_EXT1__ */

 #endif /* defined(_STD_USING) */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
