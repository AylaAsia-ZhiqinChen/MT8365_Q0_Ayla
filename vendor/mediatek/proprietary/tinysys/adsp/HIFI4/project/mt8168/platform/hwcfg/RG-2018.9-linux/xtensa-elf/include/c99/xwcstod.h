/* xwcstod.h internal header */
#ifndef _WCSTOD
#define _WCSTOD

  #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
		/* INLINES, FOR C++ */
_C_LIB_DECL
inline double _CDECL wcstod(const wchar_t *_Restrict _Str,
	wchar_t **_Restrict _Endptr)
	{return (_WStod(_Str, _Endptr, 0));
	}

inline unsigned long _CDECL wcstoul(const wchar_t *_Restrict _Str,
	wchar_t **_Restrict _Endptr, int _Base)
	{return (_WStoul(_Str, _Endptr, _Base));
	}
_END_C_LIB_DECL

  #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
		/* MACROS AND DECLARATIONS, FOR C */
_C_LIB_DECL
double _CDECL wcstod(const wchar_t *, wchar_t **);
unsigned long _CDECL wcstoul(const wchar_t *, wchar_t **, int);
_END_C_LIB_DECL

   #define wcstod(str, endptr)	_WStod(str, endptr, 0)
   #define wcstoul(str, endptr, base)	_WStoul(str, endptr, base)
  #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */

#endif /* _WCSTOD */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
