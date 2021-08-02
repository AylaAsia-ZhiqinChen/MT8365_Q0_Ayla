/* tgmath.h standard header */
#ifndef _TGMATH
#define _TGMATH
#include <complex.h>
#include <math.h>

#ifndef __cplusplus

 #if __clang__ && __GNUC_MINOR__ == 2	/* compiler test */
#include <xtgmath1.h>

 #else /* __clang__ */
#include <xtgmath0.h>
 #endif /* __clang__ */

#endif /* __cplusplus */
#endif /* _TGMATH */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
