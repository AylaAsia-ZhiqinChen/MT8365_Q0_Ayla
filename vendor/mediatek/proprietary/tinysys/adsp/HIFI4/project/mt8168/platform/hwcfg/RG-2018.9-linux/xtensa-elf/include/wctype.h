/* wctype.h standard header */
#ifndef _WCTYPE
#define _WCTYPE
#ifndef _YVALS
 #include <yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
		/* MACROS */
#define WEOF	((_CSTD wint_t)(-1))

		/* TYPE DEFINITIONS */

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

 #ifndef _WCTRANST
  #define _WCTRANST
typedef _Sizet wctrans_t;
 #endif /* _WCTRANST */

 #else /* 1200 <= _MSC_VER */
typedef _Sizet wctrans_t;

 #if !defined(__BORLANDC__) && !defined(_WCTYPE_T_DEFINED)
typedef _Sizet wctype_t;

 #elif !defined(_WCTYPE_T_DEFINED)
  #define _WCTYPE_T_DEFINED
typedef _Wchart wctype_t;
 #endif /* !defined(__BORLANDC__) */

 #ifndef _WINTT
  #define _WINTT
typedef _Wintt wint_t;
 #endif /* _WINTT */

 #endif /* 1200 <= _MSC_VER */

		/* ctype DECLARATIONS */
 #include <xwcc.h>

_C_LIB_DECL
wctrans_t wctrans(const char *);
wctype_t wctype(const char *);
_END_C_LIB_DECL

 #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
		/* INLINES, FOR C++ */
_C_LIB_DECL
inline wint_t (towctrans)(wint_t _Wc, wctrans_t _Off)
	{	// translate by category
	return (_Towctrans(_Wc, _Off));
	}
_END_C_LIB_DECL

 #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
	/* DECLARATIONS AND MACRO OVERRIDES, FOR C */
_C_LIB_DECL
wint_t (towctrans)(wint_t, wctrans_t);
_END_C_LIB_DECL
 #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
_C_STD_END
#endif /* _WCTYPE */

#ifdef _STD_USING
using _CSTD wctrans_t; using _CSTD wctype_t; using _CSTD wint_t;

using _CSTD iswalnum; using _CSTD iswalpha; using _CSTD iswcntrl;
using _CSTD iswctype; using _CSTD iswdigit; using _CSTD iswgraph;
using _CSTD iswlower; using _CSTD iswprint; using _CSTD iswpunct;
using _CSTD iswspace; using _CSTD iswupper; using _CSTD iswxdigit;
using _CSTD towctrans; using _CSTD towlower; using _CSTD towupper;
using _CSTD wctrans; using _CSTD wctype;

 #if _HAS_C9X
using _CSTD iswblank;
 #endif /* _IS_C9X */

#endif /* _STD_USING */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
