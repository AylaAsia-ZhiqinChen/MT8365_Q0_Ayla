/* stdalign.h standard header */
#ifndef _STDALIGN
#define _STDALIGN
#ifndef _YVALS
 #include <yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
 #define __alignas_is_defined	1
 #define __alignof_is_defined	1

 #ifndef __cplusplus
 #define alignas	_Alignas
 #define alignof	_Alignof
 #endif /* __cplusplus */
_C_STD_END
#endif /* _STDALIGN */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
