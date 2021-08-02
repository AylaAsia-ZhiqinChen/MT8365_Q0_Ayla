/* ctype.h standard header */
#ifndef _CTYPE
#define _CTYPE
#ifndef _YVALS
 #include <c99/yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
		/* _Ctype code bits */
#define _XB		0x400 /* extra blank */
#define _XA		0x200 /* extra alphabetic */
#define _XS		0x100 /* extra space */
#define _BB		0x80 /* BEL, BS, etc. */
#define _CN		0x40 /* CR, FF, HT, NL, VT */
#define _DI		0x20 /* '0'-'9' */
#define _LO		0x10 /* 'a'-'z' */
#define _PU		0x08 /* punctuation */
#define _SP		0x04 /* space */
#define _UP		0x02 /* 'A'-'Z' */
#define _XD		0x01 /* '0'-'9', 'A'-'F', 'a'-'f' */

#define EOF		(-1)

_C_LIB_DECL
typedef const short *_Ctype_t;

_CRTIMP2 _Ctype_t _CDECL _Getpctype(void);
_CRTIMP2 _Ctype_t _CDECL _Getptolower(void);
_CRTIMP2 _Ctype_t _CDECL _Getptoupper(void);

 #if !_MULTI_THREAD || _COMPILER_TLS && !_GLOBAL_LOCALE
extern _CRTIMP2 _Ctype_t _TLS_QUAL _Ctype;
extern _CRTIMP2 _Ctype_t _TLS_QUAL _Tolotab;
extern _CRTIMP2 _Ctype_t _TLS_QUAL _Touptab;

  #define _Getpctype()		_Ctype
  #define _Getptolower()	_Tolotab
  #define _Getptoupper()	_Touptab
 #endif /* !_MULTI_THREAD etc. */

#define _Getchrtype(x)	_Getpctype()[(int)(x)]
#define _CTolower(x)	_Getptolower()[(int)(x)]
#define _CToupper(x)	_Getptoupper()[(int)(x)]
_END_C_LIB_DECL

 #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
		// INLINES, for C++
_C_LIB_DECL
inline int _CDECL isalnum(int _Ch)
	{	// check alnum bits
	return (_Getchrtype(_Ch) & (_DI|_LO|_UP|_XA));
	}

inline int _CDECL isalpha(int _Ch)
	{	// check alpha bits
	return (_Getchrtype(_Ch) & (_LO|_UP|_XA));
	}

inline int _CDECL iscntrl(int _Ch)
	{	// check cntrl bits
	return (_Getchrtype(_Ch) & _BB);
	}

inline int _CDECL isdigit(int _Ch)
	{	// check digit bits
	return (_Getchrtype(_Ch) & _DI);
	}

inline int _CDECL isgraph(int _Ch)
	{	// check graph bits
	return (_Getchrtype(_Ch) & (_DI|_LO|_PU|_UP|_XA));
	}

inline int _CDECL islower(int _Ch)
	{	// check alnum bits
	return (_Getchrtype(_Ch) & _LO);
	}

inline int _CDECL isprint(int _Ch)
	{	// check print bits
	return (_Getchrtype(_Ch) & (_DI|_LO|_PU|_SP|_UP|_XA));
	}

inline int _CDECL ispunct(int _Ch)
	{	// check punct bits
	return (_Getchrtype(_Ch) & _PU);
	}

inline int _CDECL _Isspace(int _Ch)
	{	// check space bits
	return (_Getchrtype(_Ch) & (_CN|_SP|_XS));
	}

inline int _CDECL isspace(int _Ch)
	{	// check space bits
	return (_Isspace(_Ch));
	}

inline int _CDECL isupper(int _Ch)
	{	// check upper bits
	return (_Getchrtype(_Ch) & _UP);
	}

inline int _CDECL isxdigit(int _Ch)
	{	// check xdigit bits
	return (_Getchrtype(_Ch) & _XD);
	}

 #if _HAS_C9X
inline int _CDECL isblank(int _Ch)
	{	// check blank bits
	return (_Getchrtype(_Ch) & (_SP|_XB));
	}
 #endif /* _IS_C9X */

inline int _CDECL tolower(int _Ch)
	{	// convert to lower case
	return (_Ch == EOF ? EOF : _CTolower((unsigned char)_Ch));
	}

inline int _CDECL toupper(int _Ch)
	{	// convert to upper case
	return (_Ch == EOF ? EOF : _CToupper((unsigned char)_Ch));
	}
_END_C_LIB_DECL

 #else /* defined(__cplusplus) etc. */
		/* declarations and macro overrides, for C */
_C_LIB_DECL
_CRTIMP2 int _CDECL isalnum(int);
_CRTIMP2 int _CDECL isalpha(int);
_CRTIMP2 int _CDECL iscntrl(int);
_CRTIMP2 int _CDECL isdigit(int);
_CRTIMP2 int _CDECL isgraph(int);
_CRTIMP2 int _CDECL islower(int);
_CRTIMP2 int _CDECL isprint(int);
_CRTIMP2 int _CDECL ispunct(int);
_CRTIMP2 int _CDECL isspace(int);
_CRTIMP2 int _CDECL isupper(int);
_CRTIMP2 int _CDECL isxdigit(int);
_CRTIMP2 int _CDECL tolower(int);
_CRTIMP2 int _CDECL toupper(int);

 #if _HAS_C9X
_CRTIMP2 int _CDECL isblank(int);
 #endif /* _IS_C9X */

_END_C_LIB_DECL

  #define isalnum(c)		(_Getchrtype(c) & (_DI|_LO|_UP|_XA))

  #ifdef isalpha
   #define _dnk_isalpha(c)	(_Getchrtype(c) & (_LO|_UP|_XA))

  #else /* _dnk_isalpha */
   #define isalpha(c)		(_Getchrtype(c) & (_LO|_UP|_XA))
  #endif /* _dnk_isalpha */

  #define iscntrl(c)		(_Getchrtype(c) & _BB)
  #define isdigit(c)		(_Getchrtype(c) & _DI)
  #define isgraph(c)		(_Getchrtype(c) & (_DI|_LO|_PU|_UP|_XA))

  #ifdef islower
   #define _dnk_islower(c)	(_Getchrtype(c) & _LO)

  #else /* _dnk_islower */
   #define islower(c)		(_Getchrtype(c) & _LO)
  #endif /* _dnk_islower */

  #define isprint(c)		(_Getchrtype(c) & (_DI|_LO|_PU|_SP|_UP|_XA))
  #define ispunct(c)		(_Getchrtype(c) & _PU)
  #define isspace(c)		(_Getchrtype(c) & (_CN|_SP|_XS))
  #define isupper(c)		(_Getchrtype(c) & _UP)
  #define isxdigit(c)		(_Getchrtype(c) & _XD)

 #if _HAS_C9X
  #define isblank(c)		(_Getchrtype(c) & (_SP|_XB))
 #endif /* _IS_C9X */

 #ifndef _DNK_RENAME
  #define tolower(c)	_CTolower(c)
  #define toupper(c)	_CToupper(c)
 #endif /* _DNK_RENAME */

 #endif /* defined(__cplusplus) etc. */
_C_STD_END
#endif /* _CTYPE */

#ifdef _STD_USING
using _CSTD isalnum; using _CSTD isalpha; using _CSTD iscntrl;
using _CSTD isdigit; using _CSTD isgraph; using _CSTD islower;
using _CSTD isprint; using _CSTD ispunct; using _CSTD isspace;
using _CSTD isupper; using _CSTD isxdigit; using _CSTD tolower;
using _CSTD toupper; using _CSTD _Isspace;

 #if _HAS_C9X
using _CSTD isblank;
 #endif /* _IS_C9X */

#endif /* _STD_USING */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
