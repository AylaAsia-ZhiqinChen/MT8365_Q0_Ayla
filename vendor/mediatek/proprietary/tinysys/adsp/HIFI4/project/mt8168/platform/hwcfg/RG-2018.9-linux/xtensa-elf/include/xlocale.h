/* xlocale.h internal header */
#ifndef _XLOCALE
#define _XLOCALE
#include <ctype.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <xstate.h>
#include <xtinfo.h>
#include <xwctype.h>
_C_STD_BEGIN
		/* macros for _Getloc and friends */
#define _M_ALL		(_CATMASK(_NCAT) - 1)
#define ADDR(p, q, ty)	(*(ty *)((char *)p + q->_Offset))
#define NEWADDR(p, q, ty)	\
	(ADDR(p, q, ty) != ADDR(&_Clocale, q, ty))
#define MAXLIN	256
#define TABSIZ	((UCHAR_MAX + 2) * sizeof (short))

		/* type definitions */
enum _Lcode
	{	/* codes for locale parsing tables */
	L_GSTRING, L_NAME, L_NOTE, L_SETVAL, L_STATE,
	L_STRING, L_TABLE, L_VALUE, L_WCTYPE,
	L_WSTRING
	};

typedef struct _Locitem
	{	/* parsing table entry */
	const char *_Name;
	size_t _Offset;
	enum _Lcode _Code;
	} _Locitem;

typedef struct _Linfo
	{	/* locale description */
	const char *_Name;	/* must be first */
	struct _Linfo *_Next;

		/* controlled by LC_COLLATE */
	_Statab _Costate;
	_Statab _WCostate;

		/* controlled by LC_CTYPE */
	const short *_Ctype;
	const short *_Tolotab;
	const short *_Touptab;
	char _Mbcurmax;
	_Statab _Mbstate;
	_Statab _Wcstate;
	const _Wctab *_Wctrans;
	const _Wctab *_Wctype;

		/* controlled by LC_MONETARY and LC_NUMERIC */
	struct lconv _Lc;

		/* controlled by LC_TIME */
	_Tinfo _Times;
	} _Linfo;

		/* declarations */
_C_LIB_DECL
const char *_Defloc(int);
_Linfo *_Findloc(const char *, size_t);
void _Freeloc(_Linfo *);
_Linfo *_Getloc(const char *, const char *);
const char *_Locsum(const char *, unsigned long *);
int _Locterm(const char **, unsigned long *);
int _Locvar(char, unsigned long);
int _Makeloc(FILE *, char *, _Linfo *);
int _Makestab(_Linfo *, const _Locitem *, const char *);
int _Makewct(_Linfo *, const _Locitem *, const char *);
const _Locitem *_Readloc(FILE *, char *, const char **);
_Linfo *_Setloc(size_t, _Linfo *);
const char *_Skip(const char *);
extern _Linfo _Clocale;
_EXTERN_CONST_DECL _Locitem _Loctab[];
_END_C_LIB_DECL
_C_STD_END
#endif /* _XLOCALE */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
