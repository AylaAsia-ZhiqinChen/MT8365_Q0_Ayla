/* wrapper to suppress warnings for windows.h
 */
#ifndef _WRAPWIN_H
#define _WRAPWIN_H

 #if 1300 <= _MSC_VER
 #pragma warning(disable: 4047)
 #endif /* 1300 <= _MSC_VER */

 #include <windows.h>
#endif /* _WRAPWIN_H */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
