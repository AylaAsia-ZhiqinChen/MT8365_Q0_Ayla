/* xwcc.h internal header */
#ifndef _XWCC
#define _XWCC

_C_LIB_DECL
_CRTIMP2 int _CDECL _Iswctype(wint_t, wctype_t);
_CRTIMP2 wint_t _CDECL _Towctrans(wint_t, wctrans_t);

_END_C_LIB_DECL

  #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
		// INLINES, FOR C++
_C_LIB_DECL
inline int _CDECL iswalnum(wint_t _Wc)
	{	// test for alnum
	return (_Iswctype(_Wc, 1));
	}

inline int _CDECL iswalpha(wint_t _Wc)
	{	// test for alpha
	return (_Iswctype(_Wc, 2));
	}

inline int _CDECL iswcntrl(wint_t _Wc)
	{	// test for cntrl
	return (_Iswctype(_Wc, 3));
	}

inline int _CDECL iswctype(wint_t _Wc, wctype_t _Off)
	{	// test for ctype category
	return (_Iswctype(_Wc, _Off));
	}

inline int _CDECL iswdigit(wint_t _Wc)
	{	// test for digit
	return (_Iswctype(_Wc, 4));
	}

inline int _CDECL iswgraph(wint_t _Wc)
	{	// test for graph
	return (_Iswctype(_Wc, 5));
	}

inline int _CDECL iswlower(wint_t _Wc)
	{	// test for lower
	return (_Iswctype(_Wc, 6));
	}

inline int _CDECL iswprint(wint_t _Wc)
	{	// test for print
	return (_Iswctype(_Wc, 7));
	}

inline int _CDECL iswpunct(wint_t _Wc)
	{	// test for punct
	return (_Iswctype(_Wc, 8));
	}

inline int _CDECL iswspace(wint_t _Wc)
	{	// test for space
	return (_Iswctype(_Wc, 9));
	}

inline int _CDECL iswupper(wint_t _Wc)
	{	// test for upper
	return (_Iswctype(_Wc, 10));
	}

inline int _CDECL iswxdigit(wint_t _Wc)
	{	// test for xdigit
	return (_Iswctype(_Wc, 11));
	}

inline wint_t _CDECL towlower(wint_t _Wc)
	{	// translate to lower
	return (_Towctrans(_Wc, 1));
	}

inline wint_t _CDECL towupper(wint_t _Wc)
	{	// translate to upper
	return (_Towctrans(_Wc, 2));
	}

 #if _HAS_C9X
inline int _CDECL iswblank(wint_t _Wc)
	{	// check blank bits
	return (_Iswctype(_Wc, 12));
	}
 #endif /* _IS_C9X */

_END_C_LIB_DECL

  #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
	/* DECLARATIONS AND MACRO OVERRIDES, FOR C */
_C_LIB_DECL
_CRTIMP2 int _CDECL iswalnum(wint_t);
_CRTIMP2 int _CDECL iswalpha(wint_t);
_CRTIMP2 int _CDECL iswcntrl(wint_t);
_CRTIMP2 int _CDECL iswctype(wint_t, wctype_t);
_CRTIMP2 int _CDECL iswdigit(wint_t);
_CRTIMP2 int _CDECL iswgraph(wint_t);
_CRTIMP2 int _CDECL iswlower(wint_t);
_CRTIMP2 int _CDECL iswprint(wint_t);
_CRTIMP2 int _CDECL iswpunct(wint_t);
_CRTIMP2 int _CDECL iswspace(wint_t);
_CRTIMP2 int _CDECL iswupper(wint_t);
_CRTIMP2 int _CDECL iswxdigit(wint_t);

_CRTIMP2 wint_t _CDECL towlower(wint_t);
_CRTIMP2 wint_t _CDECL towupper(wint_t);

 #if _HAS_C9X
_CRTIMP2 int _CDECL iswblank(wint_t);
 #endif /* _IS_C9X */

_END_C_LIB_DECL

#ifndef _DNK_RENAME
 #define iswalnum(wc)	_Iswctype(wc, 1)
 #define iswalpha(wc)	_Iswctype(wc, 2)
 #define iswcntrl(wc)	_Iswctype(wc, 3)
 #define iswctype(wc, off)	_Iswctype(wc, off)
 #define iswdigit(wc)	_Iswctype(wc, 4)
 #define iswgraph(wc)	_Iswctype(wc, 5)
 #define iswlower(wc)	_Iswctype(wc, 6)
 #define iswprint(wc)	_Iswctype(wc, 7)
 #define iswpunct(wc)	_Iswctype(wc, 8)
 #define iswspace(wc)	_Iswctype(wc, 9)
 #define iswupper(wc)	_Iswctype(wc, 10)
 #define iswxdigit(wc)	_Iswctype(wc, 11)
 #define towctrans(wc, off)	_Towctrans(wc, off)
 #define towlower(wc)	_Towctrans(wc, 1)
 #define towupper(wc)	_Towctrans(wc, 2)
#endif /* _DNK_RENAME */

 #if _HAS_C9X
 #define iswblank(wc)	_Iswctype(wc, 12)
 #endif /* _IS_C9X */

  #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */

#endif /* _XWCC */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
