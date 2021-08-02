/* xwctype.h internal header */
#ifndef _XWCTYPE
#define _XWCTYPE
#include <stddef.h>
#include <wctype.h>
_C_STD_BEGIN
		/* TYPE DEFINITIONS */
typedef struct
	{	/* wide-character type table */
	const char *_Name;
	size_t _Off;
	} _Wctab;
typedef const _Wctab *_PWctab;

		/* DECLARATIONS */
_C_LIB_DECL
const _Wctab *_Getpwctrtab(void);
const _Wctab *_Getpwctytab(void);

/* extern const _Wctab *_Wctrans, *_Wctype; */
_END_C_LIB_DECL
_C_STD_END
#endif /* _XWCTYPE */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
