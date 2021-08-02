/* xstdio.h internal header */
#ifndef _XSTDIO
#define _XSTDIO
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

 #if _HAS_C9X
#include <stdint.h>
 #endif /* _IS_C9X */

#ifndef _YVALS
 #include <c99/yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
		/* bits for _Mode in FILE */
#define _MOPENR	0x1
#define _MOPENW	0x2
#define _MOPENA	0x4
#define _MTRUNC	0x8
#define _MCREAT	0x10
#define _MBIN	0x20
#define _MEXCL	0x40	/* open only */

#define _MALBUF	0x40
#define _MALFIL	0x80
#define _MEOF	0x100
#define _MERR	0x200
#define _MLBF	0x400
#define _MNBF	0x800
#define _MREAD	0x1000
#define _MWRITE	0x2000
#define _MBYTE	0x4000
#define _MWIDE	0x8000

		/* codes for _Printf and _Scanf */
#define _FSP	0x01
#define _FPL	0x02
#define _FMI	0x04
#define _FNO	0x08
#define _FZE	0x10
#define _WMAX	(INT_MAX-9/10)

		/* macros for _Scanf and friends */
#define GET(px)	\
	(++(px)->nchar, (*(px)->pfn)((px)->arg, 0, 1))
#define GETN(px)	(0 <= --(px)->nget ? GET(px) \
	: (++(px)->nchar, EOF))
#define UNGET(px, ch)	\
	(--(px)->nchar, (*(px)->pfn)((px)->arg, ch, 0))
#define UNGETN(px, ch)	do if ((int)(ch) != EOF) UNGET(px, ch); \
	else --(px)->nchar; while (0)

		/* macros for atomic file locking */

 #if _FILE_OP_LOCKS
  #define _Lockfileatomic(str)		_Lockfile(str)
  #define _Unlockfileatomic(str)	_Unlockfile(str)

 #else /* _FILE_OP_LOCKS */
  #define _Lockfileatomic(str)		(void)0
  #define _Unlockfileatomic(str)	(void)0
 #endif /* _FILE_OP_LOCKS */

		/* type definitions */
typedef struct
	{	/* print formatting information */
	union
		{	/* long or long double value */
		_Longlong li;
		_ULonglong uli;
		long double ld;
		} v;
	void *(*pfn)(void *, const char *, size_t);
	void *arg;
	char *s;
	int n0, nz0, n1, nz1, n2, nz2;
	int argno, prec, nchar, width;
	unsigned short flags;
	char qual;
	char secure;	/* added for _HAS_LIB_EXT1 */
	char sep;	/* added for _HAS_FIXED_POINT %v */
	} _Pft;

typedef struct
	{	/* scan formatting information */
	int (*pfn)(void *, int, int);
	void *arg;
	va_list ap;
	const char *s;
	int nchar, nget, width;
	size_t prec;
	char noconv, qual, stored;
	char secure;	/* added for _HAS_LIB_EXT1 */
	char sep;	/* added for _HAS_FIXED_POINT %v */
	} _Sft;

 #if _HAS_C9X

 #else /* _HAS_C9X */

 #if !defined(_SYS_INT_TYPES_H) && !defined(_INTMAXT)
  #define _INTMAXT
typedef _Longlong intmax_t;
typedef _ULonglong uintmax_t;
 #endif /* !defined(_SYS_INT_TYPES_H) etc. */

 #endif /* _IS_C9X */

		/* declarations */
_C_LIB_DECL
_CRTIMP2 void _CDECL _Closreg(void);
_CRTIMP2 FILE *_CDECL _Fofind(void);
_CRTIMP2 void _CDECL _Fofree(FILE *);
_CRTIMP2 FILE *_CDECL _Foprep(const _Sysch_t *, const _Sysch_t *,
	FILE *, _FD_TYPE, int);
_CRTIMP2 _FD_TYPE _CDECL _Fopen(const _Sysch_t *, unsigned int, int);
_CRTIMP2 int _CDECL _Frprep(FILE *);
_CRTIMP2 int _CDECL _Ftmpnam(char *, int);
_CRTIMP2 int _CDECL _Fwprep(FILE *);
_CRTIMP2 void _CDECL _Genld(_Pft *, char, char *, short, short);
_CRTIMP2 int _CDECL _Getfld(_Sft *);
_CRTIMP2 int _CDECL _Getfloat(_Sft *, void *);
_CRTIMP2 int _CDECL _Getint(_Sft *, void *);
_CRTIMP2 int _CDECL _Getstr(_Sft *, int);
_CRTIMP2 void _CDECL _Ldtob(_Pft *, char);
_CRTIMP2 void _CDECL _Litob(_Pft *, char);
_CRTIMP2 int _CDECL _Printf(void *(*)(void *, const char *, size_t),
	void *, const char *, va_list, int);
_CRTIMP2 int _CDECL _Putfld(_Pft *, va_list *, char, char *);
_CRTIMP2 int _CDECL _Putstr(_Pft *, const wchar_t *);
_CRTIMP2 int _CDECL _Puttxt(_Pft *, const char *);
_CRTIMP2 int _CDECL _Scanf(int (*)(void *, int, int),
	void *, const char *, va_list, int);
_CRTIMP2 void _Vacopy(va_list *, va_list);
_END_C_LIB_DECL
_C_STD_END
#endif /* _XSTDIO */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
