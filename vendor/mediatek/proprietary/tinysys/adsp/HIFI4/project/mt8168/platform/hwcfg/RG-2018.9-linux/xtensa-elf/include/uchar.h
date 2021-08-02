/* uchar.h added header for TR 19769 */
#ifndef _UCHAR
#define _UCHAR
#ifndef _YVALS
 #include <yvals.h>
#endif /* _YVALS */

		/* macros */
#ifndef NULL
 #define NULL	_NULL
#endif /* NULL */

#ifndef __STDC_UTF_16__
 #define __STDC_UTF_16__	1
#endif /* __STDC_UTF_16__ */

#ifndef __STDC_UTF_32__
 #define __STDC_UTF_32__	1
#endif /* __STDC_UTF_32__ */

 #if !defined(_CHAR16T)
  #define _CHAR16T

  #ifdef __cplusplus

 #if _HAS_CHAR16_T_LANGUAGE_SUPPORT

 #else /* _HAS_CHAR16_T_LANGUAGE_SUPPORT */
typedef unsigned short char16_t;
typedef unsigned int char32_t;
 #endif /* _HAS_CHAR16_T_LANGUAGE_SUPPORT */

  #else /* __cplusplus */
typedef unsigned short char16_t;
typedef unsigned int char32_t;
  #endif /* __cplusplus */

 #endif /* !defined(_CHAR16T) */

_C_STD_BEGIN
_C_LIB_DECL
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

		/* declarations */
size_t mbrtoc16(char16_t *_Restrict, const char *_Restrict,
	size_t, mbstate_t *_Restrict);
size_t c16rtomb(char *_Restrict, char16_t,
	mbstate_t *_Restrict);

size_t mbrtoc32(char32_t *_Restrict, const char *_Restrict,
	size_t, mbstate_t *_Restrict);
size_t c32rtomb(char *_Restrict, char32_t,
	mbstate_t *_Restrict);
_END_C_LIB_DECL
_C_STD_END
#endif /* _UCHAR */

 #if defined(_STD_USING)

  #ifdef _STD_USING_SIZE_T
using _CSTD size_t;
  #endif /* _STD_USING_SIZE_T */

using _CSTD mbstate_t;
using _CSTD mbrtoc16; using _CSTD c16rtomb;
using _CSTD mbrtoc32; using _CSTD c32rtomb;
 #endif /* defined(_STD_USING) */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
