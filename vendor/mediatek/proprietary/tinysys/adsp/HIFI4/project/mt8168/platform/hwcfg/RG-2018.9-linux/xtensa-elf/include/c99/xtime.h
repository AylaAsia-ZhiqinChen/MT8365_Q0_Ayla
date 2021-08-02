/* xtime.h internal header */
#ifndef _XTIME
#define _XTIME
#include <xtinfo.h>
_C_STD_BEGIN
		/* macros */
#define WDAY	1	/* to get day of week right */

		/* type definitions */
typedef struct
	{	/* rule for daylight savings time */
	unsigned char wday, hour, day, mon, year;
	} Dstrule;

		/* internal declarations */
_C_LIB_DECL
_CRTIMP2 int _CDECL _Daysto(int, int);
_CRTIMP2 const char * _CDECL _Gentime(const struct tm *, const _Tinfo *,
	char, char, int *, char *);
_CRTIMP2 Dstrule * _CDECL _Getdst(const char *);
_CRTIMP2 const char * _CDECL _Gettime(const char *, int, int *);
_CRTIMP2 int _CDECL _Isdst(const struct tm *);
_CRTIMP2 const char * _CDECL _Getzone(void);
_CRTIMP2 struct tm * _CDECL _Ttotm(struct tm *, time_t, int);
_CRTIMP2 time_t _CDECL _Tzoff(void);
_END_C_LIB_DECL
_C_STD_END
#endif /* _XTIME */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
