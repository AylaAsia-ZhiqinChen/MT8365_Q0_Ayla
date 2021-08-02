/* errno.h standard header */
#ifndef _ERRNO
#define _ERRNO
#ifndef _YVALS
 #include <c99/yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN

		/* ERROR CODES */

#define EDOM           0x0021
#define EFAULT         0x000E
#define EFBIG          0x001B
#define EFPOS          0x0098
#define EMLINK         0x001F
#define ENFILE         0x0017
#define ENOENT         0x0002
#define ENOSPC         0x001C
#define ENOTTY         0x0019
#define EPIPE          0x0020
#define ERANGE         0x0022
#define EROFS          0x001E
#define ESPIPE         0x001D

 #if defined(__BORLANDC__)
  #define E2BIG         0x0014
  #define EACCES        0x0005
  #define EAGAIN        0x002A
  #define EBADF         0x0006
  #define EBUSY         0x002C
  #define ECHILD        0x0018
  #define EEXIST        0x0023
  #define EINTR         0x0027
  #define EINVAL        0x0013
  #define EIO           0x0028
  #define EISDIR        0x002E
  #define EMFILE        0x0004
  #define ENODEV        0x000F
  #define ENOEXEC       0x0015
  #define ENOMEM        0x0008
  #define ENOTDIR       0x002D
  #define ENXIO         0x0029
  #define EPERM         0x0025
  #define ESRCH         0x0026
  #define EXDEV         0x0016

 #else /* defined(__BORLANDC__) */
  #define E2BIG         0x0007
  #define EACCES        0x000D
  #define EAGAIN        0x000B
  #define EBADF         0x0009
  #define EBUSY         0x0010
  #define ECHILD        0x000A
  #define EEXIST        0x0011
  #define EINTR         0x0004
  #define EINVAL        0x0016
  #define EIO           0x0005
  #define EISDIR        0x0015
  #define EMFILE        0x0018
  #define ENODEV        0x0013
  #define ENOEXEC       0x0008
  #define ENOMEM        0x000C
  #define ENOTDIR       0x0014
  #define ENXIO         0x0006
  #define EPERM         0x0001
  #define ESRCH         0x0003
  #define EXDEV         0x0012
 #endif /* defined(__BORLANDC__) */

 #if defined(__BORLANDC__)
  #define EBADMSG       0x004A
  #define ECANCELED     0x007D
  #define EDEADLK       0x0024
  #define EILSEQ        0x0058
  #define EINPROGRESS   0x0073
  #define EMSGSIZE      0x005A
  #define ENAMETOOLONG  0x0030
  #define ENOLCK        0x006F
  #define ENOSYS        0x0059
  #define ENOTEMPTY     0x0031
  #define ENOTSUP       0x005F
  #define ETIMEDOUT     0x006E

 #elif defined(_MSL_WCHAR_T_TYPE)
  #define EBADMSG       0x004A
  #define ECANCELED     0x007D
  #define EDEADLK       0x0023
  #define EILSEQ        0x0058
  #define EINPROGRESS   0x0073
  #define EMSGSIZE      0x005A
  #define ENAMETOOLONG  0x0024
  #define ENOLCK        0x004D
  #define ENOSYS        0x0026
  #define ENOTEMPTY     0x0027
  #define ENOTSUP       0x005F
  #define ETIMEDOUT     0x006E

 #elif defined(_WIN32_C_LIB)
  #define EBADMSG       0x004A
  #define ECANCELED     0x007D
  #define EDEADLK       0x0024
  #define EILSEQ        0x002A
  #define EINPROGRESS   0x0073
  #define EMSGSIZE      0x005A
  #define ENAMETOOLONG  0x0026
  #define ENOLCK        0x0027
  #define ENOSYS        0x0028
  #define ENOTEMPTY     0x0029
  #define ENOTSUP       0x005F
  #define ETIMEDOUT     0x006E

 #elif defined(_LINUX_C_LIB)
  #define EBADMSG       0x004A
  #define ECANCELED     0x007D
  #define EDEADLK       0x0023
  #define EILSEQ        0x0054
  #define EINPROGRESS   0x0073
  #define EMSGSIZE      0x005A
  #define ENAMETOOLONG  0x0024
  #define ENOLCK        0x0025
  #define ENOSYS        0x0026
  #define ENOTEMPTY     0x0027
  #define ENOTSUP       0x005F
  #define ETIMEDOUT     0x006E

  #define EWOULDBLOCK	EAGAIN
  #define EAFNOSUPPORT	0x0061

 #elif defined (__APPLE__)
  #define EBADMSG		0x005E	/* 94, bad message */
  #define ECANCELED		0x0059	/* 89, operation canceled */
  #define EDEADLK		0x000B	/* 11, resource deadlock avoided */
  #define EILSEQ		0x005C	/* 92, illegal bye sequence */
  #define EDOATTR		0x005D	/* 93, attribute not found */
  #define EINPROGRESS   0x0024	/* 36, operation now in progress */
  #define EMSGSIZE      0x0028	/* 40, message too long */
  #define ELOOP			0x003E	/* 62, too many sym links */
  #define ENAMETOOLONG  0x003F	/* 63, file name too long */
  #define ENOLCK        0x004D	/* 77, no locks available */
  #define ENOSYS        0x004E	/* 78, function not implemented */
  #define ENOTEMPTY     0x0042	/* 66, directory not empty */
  #define ENOTSUP       0x002D	/* 45, operation not supported */
  #define ETIMEDOUT     0x003C	/* 60, Operation timed out */

  #ifdef EAGAIN
   #undef EAGAIN
  #endif /* EAGAIN */

  #define EAGAIN		0x0023	/* 35, resource temp unavailable */

  #define EWOULDBLOCK   EAGAIN
  #define EAFNOSUPPORT  0x002F	/* 47, address family not supported */

 #else /* library type */
  #define EBADMSG       0x004D
  #define ECANCELED     0x002F
  #define EDEADLK       0x002D
  #define EILSEQ        0x0058
  #define EINPROGRESS   0x0096
  #define EMSGSIZE      0x0061
  #define ENAMETOOLONG  0x004E
  #define ENOLCK        0x002E
  #define ENOSYS        0x0059
  #define ENOTEMPTY     0x005D
  #define ENOTSUP       0x0030
  #define ETIMEDOUT     0x0091
 #endif /* library type */

		/* POSIX SUPPLEMENT */
#ifndef E2BIG
 #define E2BIG	1000
#endif /* E2BIG */

#ifndef EACCES
 #define EACCES	1001
#endif /* EACCES */

#ifndef EADDRINUSE
 #define EADDRINUSE	1002
#endif /* EADDRINUSE */

#ifndef EADDRNOTAVAIL
 #define EADDRNOTAVAIL	1003
#endif /* EADDRNOTAVAIL */

#ifndef EAFNOSUPPORT
 #define EAFNOSUPPORT	1004
#endif /* EAFNOSUPPORT */

#ifndef EAGAIN
 #define EAGAIN	1005
#endif /* EAGAIN */

#ifndef EALREADY
 #define EALREADY	1006
#endif /* EALREADY */

#ifndef EBADF
 #define EBADF	1007
#endif /* EBADF */

#ifndef EBADMSG
 #define EBADMSG	1008
#endif /* EBADMSG */

#ifndef EBUSY
 #define EBUSY	1009
#endif /* EBUSY */

#ifndef ECANCELED
 #define ECANCELED	1019
#endif /* ECANCELED */

#ifndef ECHILD
 #define ECHILD	1011
#endif /* ECHILD */

#ifndef ECONNABORTED
 #define ECONNABORTED	1012
#endif /* ECONNABORTED */

#ifndef ECONNREFUSED
 #define ECONNREFUSED	1013
#endif /* ECONNREFUSED */

#ifndef ECONNRESET
 #define ECONNRESET	1014
#endif /* ECONNRESET */

#ifndef EDEADLK
 #define EDEADLK	1015
#endif /* EDEADLK */

#ifndef EDESTADDRREQ
 #define EDESTADDRREQ	1016
#endif /* EDESTADDRREQ */

#ifndef EDOM
 #define EDOM	1017
#endif /* EDOM */

#ifndef EEXIST
 #define EEXIST	1018
#endif /* EEXIST */

#ifndef EFAULT
 #define EFAULT	1019
#endif /* EFAULT */

#ifndef EFBIG
 #define EFBIG	1020
#endif /* EFBIG */

#ifndef EHOSTUNREACH
 #define EHOSTUNREACH	1021
#endif /* EHOSTUNREACH */

#ifndef EIDRM
 #define EIDRM	1022
#endif /* EIDRM */

#ifndef EILSEQ
 #define EILSEQ	1023
#endif /* EILSEQ */

#ifndef EINPROGRESS
 #define EINPROGRESS	1024
#endif /* EINPROGRESS */

#ifndef EINTR
 #define EINTR	1025
#endif /* EINTR */

#ifndef EINVAL
 #define EINVAL	1026
#endif /* EINVAL */

#ifndef EIO
 #define EIO	1027
#endif /* EIO */

#ifndef EISCONN
 #define EISCONN	1028
#endif /* EISCONN */

#ifndef EISDIR
 #define EISDIR	1029
#endif /* EISDIR */

#ifndef ELOOP
 #define ELOOP	1030
#endif /* ELOOP */

#ifndef EMFILE
 #define EMFILE	1031
#endif /* EMFILE */

#ifndef EMLINK
 #define EMLINK	1032
#endif /* EMLINK */

#ifndef EMSGSIZE
 #define EMSGSIZE	1033
#endif /* EMSGSIZE */

#ifndef ENAMETOOLONG
 #define ENAMETOOLONG	1034
#endif /* ENAMETOOLONG */

#ifndef ENETDOWN
 #define ENETDOWN	1035
#endif /* ENETDOWN */

#ifndef ENETRESET
 #define ENETRESET	1036
#endif /* ENETRESET */

#ifndef ENETUNREACH
 #define ENETUNREACH	1037
#endif /* ENETUNREACH */

#ifndef ENFILE
 #define ENFILE	1038
#endif /* ENFILE */

#ifndef ENOBUFS
 #define ENOBUFS	1039
#endif /* ENOBUFS */

#ifndef ENODATA
 #define ENODATA	1040
#endif /* ENODATA */

#ifndef ENODEV
 #define ENODEV	1041
#endif /* ENODEV */

#ifndef ENOENT
 #define ENOENT	10424
#endif /* ENOENT */

#ifndef ENOEXEC
 #define ENOEXEC	103
#endif /* ENOEXEC */

#ifndef ENOLCK
 #define ENOLCK	1044
#endif /* ENOLCK */

#ifndef ENOLINK
 #define ENOLINK	1045
#endif /* ENOLINK */

#ifndef ENOMEM
 #define ENOMEM	1046
#endif /* ENOMEM */

#ifndef ENOMSG
 #define ENOMSG	1047
#endif /* ENOMSG */

#ifndef ENOPROTOOPT
 #define ENOPROTOOPT	1048
#endif /* ENOPROTOOPT */

#ifndef ENOSPC
 #define ENOSPC	1049
#endif /* ENOSPC */

#ifndef ENOSR
 #define ENOSR	1050
#endif /* ENOSR */

#ifndef ENOSTR
 #define ENOSTR	1051
#endif /* ENOSTR */

#ifndef ENOSYS
 #define ENOSYS	1052
#endif /* ENOSYS */

#ifndef ENOTCONN
 #define ENOTCONN	1053
#endif /* ENOTCONN */

#ifndef ENOTDIR
 #define ENOTDIR	1054
#endif /* ENOTDIR */

#ifndef ENOTEMPTY
 #define ENOTEMPTY	1055
#endif /* ENOTEMPTY */

#ifndef ENOTRECOVERABLE
 #define ENOTRECOVERABLE	1056
#endif /* ENOTRECOVERABLE */

#ifndef ENOTSOCK
 #define ENOTSOCK	1057
#endif /* ENOTSOCK */

#ifndef ENOTSUP
 #define ENOTSUP	1058
#endif /* ENOTSUP */

#ifndef ENOTTY
 #define ENOTTY	1059
#endif /* ENOTTY */

#ifndef ENXIO
 #define ENXIO	1060
#endif /* ENXIO */

#ifndef EOPNOTSUPP
 #define EOPNOTSUPP	1061
#endif /* EOPNOTSUPP */

#ifndef EOTHER
 #define EOTHER	1062
#endif /* EOTHER */

#ifndef EOVERFLOW
 #define EOVERFLOW	1063
#endif /* EOVERFLOW */

#ifndef EOWNERDEAD
 #define EOWNERDEAD	1064
#endif /* EOWNERDEAD */

#ifndef EPERM
 #define EPERM	1065
#endif /* EPERM */

#ifndef EPIPE
 #define EPIPE	1066
#endif /* EPIPE */

#ifndef EPROTO
 #define EPROTO	1067
#endif /* EPROTO */

#ifndef EPROTONOSUPPORT
 #define EPROTONOSUPPORT	1068
#endif /* EPROTONOSUPPORT */

#ifndef EPROTOTYPE
 #define EPROTOTYPE	1069
#endif /* EPROTOTYPE */

#ifndef ERANGE
 #define ERANGE	1070
#endif /* ERANGE */

#ifndef EROFS
 #define EROFS	1071
#endif /* EROFS */

#ifndef ESPIPE
 #define ESPIPE	1072
#endif /* ESPIPE */

#ifndef ESRCH
 #define ESRCH	1073
#endif /* ESRCH */

#ifndef ETIME
 #define ETIME	1074
#endif /* ETIME */

#ifndef ETIMEDOUT
 #define ETIMEDOUT	1075
#endif /* ETIMEDOUT */

#ifndef ETXTBSY
 #define ETXTBSY	1076
#endif /* ETXTBSY */

#ifndef EWOULDBLOCK
 #define EWOULDBLOCK	1077
#endif /* EWOULDBLOCK */

#ifndef EXDEV
 #define EXDEV	1078
#endif /* EXDEV */

		/* DECLARATIONS */
_C_LIB_DECL

 #if !_MULTI_THREAD || _COMPILER_TLS && !_GLOBAL_LOCALE
extern _CRTIMP2 int _TLS_QUAL _Errno;

  #define errno	(_CSTD _Errno)

 #elif defined (__XTENSA__)

#include <sys/reent.h>
#define errno  (_reent_ptr->_errno)

 #else /* !_MULTI_THREAD || _COMPILER_TLS && !_GLOBAL_LOCALE */
_CRTIMP2 int *_CDECL _Geterrno(void);

  #define errno	(*_CSTD _Geterrno())
 #endif /* !_MULTI_THREAD || _COMPILER_TLS && !_GLOBAL_LOCALE */

_END_C_LIB_DECL

 #if __STDC_WANT_LIB_EXT1__
_C_LIB_DECL

  #if !defined(_ERRNO_T_DEFINED)
   #define _ERRNO_T_DEFINED
typedef int errno_t;
  #endif /* _ERRNO_T_DEFINED */

_END_C_LIB_DECL
 #endif /* __STDC_WANT_LIB_EXT1__ */
_C_STD_END
#endif /* _ERRNO */

 #if defined(_STD_USING)

 #if __STDC_WANT_LIB_EXT1__
using _CSTD errno_t;
 #endif /* __STDC_WANT_LIB_EXT1__ */

 #endif /* defined(_STD_USING) */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
