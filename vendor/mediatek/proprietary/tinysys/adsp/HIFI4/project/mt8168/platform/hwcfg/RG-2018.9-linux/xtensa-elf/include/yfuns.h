/* yfuns.h functions header */
#ifndef _YFUNS
#define _YFUNS
#include <stddef.h>
_C_STD_BEGIN
 #if _USE_EXISTING_SYSTEM_NAMES

  #if defined(__BORLANDC__)
   #define _Environ	_environ

  #else /* defined(__BORLANDC__) */
   #define _Environ	environ
  #endif /* defined(__BORLANDC__) */

 #if _HAS_C9X

 #else /* _HAS_C9X */
 #define _Exit	_exit
 #endif /* _IS_C9X */

  #define _Close	close
  #define _Lseek	lseek
  #define _Read		read
  #define _Write	write
 #endif /* _USE_EXISTING_SYSTEM_NAMES */

		/* process control */
#if TENSILICA
#define _Envp	(_Environ)
#else
#define _Envp	(*_Environ)
#endif

		/* stdio functions */
#define _Fclose(str)	_Close(_FD_NO(str))
#define _Fread(str, buf, cnt)	_Read(_FD_NO(str), buf, cnt)
#define _Fwrite(str, buf, cnt)	_Write(_FD_NO(str), buf, cnt)

		/* interface declarations */
_EXTERN_C
extern const char **_Environ;
_NO_RETURN(_Exit(int));

 #if defined(__APPLE__) || defined(__CYGWIN__)
int _Close(int);
_Longlong _Lseek(int, _Longlong, int);
int _Read(int, void *, size_t);
int _Write(int, const void *, size_t);

 #else /* defined(__APPLE__) */
int _Close(int);
long _Lseek(int, long, int);
int _Read(int, unsigned char *, int);
int _Write(int, const unsigned char *, int);
 #endif /* defined(__APPLE__) */

_END_EXTERN_C
_C_STD_END
#endif /* _YFUNS */

/*
 * Portions copyright (c) 2010 by Tensilica, Inc.
 *
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
