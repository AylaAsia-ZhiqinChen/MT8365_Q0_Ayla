/* xstrxfrm.h internal header */
#ifndef _XSTRXFRM
#define _XSTRXFRM
#include <string.h>
#include <xstate.h>
_C_STD_BEGIN
		/* TYPES */
typedef struct Xfrm {	/* storage for transformations */
	const unsigned char *sbegin;
	const unsigned char *sin;
	const unsigned char *send;
	long weight;
	unsigned short phase, state, wc;
	} Xfrm;

		/* DECLARATIONS */
_C_LIB_DECL
_CRTIMP2 int _CDECL _Strcollx(const char *, const char *, _Statab *);
_CRTIMP2 size_t _CDECL _Strxfrmx(char *, const char *, size_t, _Statab *);

_CRTIMP2 size_t _CDECL _CStrxfrm(char *, size_t, Xfrm *, _Statab *);
_END_C_LIB_DECL
_C_STD_END
#endif /* _XSTRXFRM */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
