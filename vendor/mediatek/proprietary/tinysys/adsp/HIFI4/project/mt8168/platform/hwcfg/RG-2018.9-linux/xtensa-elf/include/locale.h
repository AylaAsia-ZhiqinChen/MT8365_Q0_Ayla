/* locale.h standard header */
#ifndef _LOCALE
#define _LOCALE
 #ifndef _YVALS
  #include <yvals.h>
 #endif /* _YVALS */

_C_STD_BEGIN

		/* MACROS */
#ifndef NULL
 #define NULL	_NULL
#endif /* NULL */

		/* LOCALE CATEGORY INDEXES */
#define _LC_COLLATE		1
#define _LC_CTYPE		2
#define _LC_MONETARY	3
#define _LC_NUMERIC		4
#define _LC_TIME		5
#define _LC_MESSAGES	6
#define _NCAT			7	/* one more than last index */

		/* LOCALE CATEGORY MASKS */
#define _CATMASK(n)	((1 << (n)) >> 1)
#define _M_COLLATE	_CATMASK(_LC_COLLATE)
#define _M_CTYPE	_CATMASK(_LC_CTYPE)
#define _M_MONETARY	_CATMASK(_LC_MONETARY)
#define _M_NUMERIC	_CATMASK(_LC_NUMERIC)
#define _M_TIME		_CATMASK(_LC_TIME)
#define _M_MESSAGES	_CATMASK(_LC_MESSAGES)
#define _M_ALL		(_CATMASK(_NCAT) - 1)

		/* LOCALE CATEGORY HANDLES */
#define LC_COLLATE	_CATMASK(_LC_COLLATE)
#define LC_CTYPE	_CATMASK(_LC_CTYPE)
#define LC_MONETARY	_CATMASK(_LC_MONETARY)
#define LC_NUMERIC	_CATMASK(_LC_NUMERIC)
#define LC_TIME		_CATMASK(_LC_TIME)
#define LC_MESSAGE	_CATMASK(_LC_MESSAGES)	/* retained */
#define LC_MESSAGES	_CATMASK(_LC_MESSAGES)
#define LC_ALL		(_CATMASK(_NCAT) - 1)

		/* TYPE DEFINITIONS */
struct lconv
	{	/* locale-specific information */
		/* controlled by LC_MONETARY */
	char *currency_symbol;
	char *int_curr_symbol;
	char *mon_decimal_point;
	char *mon_grouping;
	char *mon_thousands_sep;
	char *negative_sign;
	char *positive_sign;

	char frac_digits;
	char n_cs_precedes;
	char n_sep_by_space;
	char n_sign_posn;
	char p_cs_precedes;
	char p_sep_by_space;
	char p_sign_posn;

	char int_frac_digits;
/* vv added with C99 vv */
	char int_n_cs_precedes;
	char int_n_sep_by_space;
	char int_n_sign_posn;
	char int_p_cs_precedes;
	char int_p_sep_by_space;
	char int_p_sign_posn;
/* ^^ added with C99 ^^ */

		/* controlled by LC_NUMERIC */
	char *decimal_point;
	char *grouping;
	char *thousands_sep;
	char *_Frac_grouping;
	char *_Frac_sep;
	char *_False;
	char *_True;

		/* controlled by LC_MESSAGES */
	char *_No;
	char *_Yes;
	};

struct _Linfo;

		/* DECLARATIONS */
_C_LIB_DECL
struct lconv *localeconv(void);
char *setlocale(int, const char *);
extern struct lconv _TLS_QUAL _Locale;
_END_C_LIB_DECL

_C_STD_END
#endif /* _LOCALE */

#ifdef _STD_USING
using _CSTD lconv; using _CSTD localeconv; using _CSTD setlocale;
#endif /* _STD_USING */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
