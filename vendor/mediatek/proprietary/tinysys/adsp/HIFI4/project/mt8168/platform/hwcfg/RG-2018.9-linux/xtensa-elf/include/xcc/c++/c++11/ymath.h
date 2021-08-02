/* ymath.h internal header */
#ifndef _YMATH
#define _YMATH
#include <yvals.h>
_C_STD_BEGIN
 _C_LIB_DECL

		/* MACROS FOR _FPP_TYPE */
#define _FPP_NONE	0	/* software emulation of FPP */
#define _FPP_X86	1	/* Intel Pentium */
#define _FPP_SPARC	2	/* Sun SPARC */
#define _FPP_MIPS	3	/* SGI MIPS */
#define _FPP_S390	4	/* IBM S/390 */
#define _FPP_PPC	5	/* Motorola PowerPC */
#define _FPP_HPPA	6	/* Hewlett-Packard PA-RISC */
#define _FPP_ALPHA	7	/* Compaq Alpha */
#define _FPP_ARM	8	/* ARM ARM */
#define _FPP_M68K	9	/* Motorola 68xxx */
#define _FPP_SH4	10	/* Hitachi SH4 */
#define _FPP_IA64	11	/* Intel IA64 */
#define _FPP_WIN	12	/* Microsoft Windows */
#define _FPP_XTENSA	13	/* Xtensa */

		/* MACROS FOR _Dtest RETURN (0 => ZERO) */
#define _DENORM		(-2)	/* C9X only */
#define _FINITE		(-1)
#define _INFCODE	1
#define _NANCODE	2

		/* MACROS FOR _Feraise ARGUMENT */

 #if _FPP_TYPE == _FPP_X86
  #define _FE_DIVBYZERO	0x04
  #define _FE_INEXACT	0x20
  #define _FE_INVALID	0x01
  #define _FE_OVERFLOW	0x08
  #define _FE_UNDERFLOW	0x10

 #elif _FPP_TYPE == _FPP_SPARC
  #define _FE_DIVBYZERO	0x02
  #define _FE_INEXACT	0x01
  #define _FE_INVALID	0x10
  #define _FE_OVERFLOW	0x08
  #define _FE_UNDERFLOW	0x04

 #elif _FPP_TYPE == _FPP_MIPS
  #define _FE_DIVBYZERO	0x08
  #define _FE_INEXACT	0x01
  #define _FE_INVALID	0x10
  #define _FE_OVERFLOW	0x04
  #define _FE_UNDERFLOW	0x02

 #elif _FPP_TYPE == _FPP_S390
  #define _FE_DIVBYZERO	0x08
  #define _FE_INEXACT	0x01
  #define _FE_INVALID	0x10
  #define _FE_OVERFLOW	0x04
  #define _FE_UNDERFLOW	0x02

 #elif _FPP_TYPE == _FPP_PPC
  #define _FE_DIVBYZERO	0x02
  #define _FE_INEXACT	0x01
  #define _FE_INVALID	0x10
  #define _FE_OVERFLOW	0x08
  #define _FE_UNDERFLOW	0x04

 #elif _FPP_TYPE == _FPP_HPPA
  #define _FE_DIVBYZERO	0x08
  #define _FE_INEXACT	0x01
  #define _FE_INVALID	0x10
  #define _FE_OVERFLOW	0x04
  #define _FE_UNDERFLOW	0x02

 #elif _FPP_TYPE == _FPP_ALPHA
  #define _FE_DIVBYZERO	0x02
  #define _FE_INEXACT	0x10
  #define _FE_INVALID	0x01
  #define _FE_OVERFLOW	0x04
  #define _FE_UNDERFLOW	0x08

 #elif _FPP_TYPE == _FPP_ARM
  #define _FE_DIVBYZERO	0x02
  #define _FE_INEXACT	0x10
  #define _FE_INVALID	0x01
  #define _FE_OVERFLOW	0x04
  #define _FE_UNDERFLOW	0x08

 #elif _FPP_TYPE == _FPP_M68K
  #define _FE_DIVBYZERO	0x02
  #define _FE_INEXACT	0x01
  #define _FE_INVALID	0x10
  #define _FE_OVERFLOW	0x08
  #define _FE_UNDERFLOW	0x04

 #elif _FPP_TYPE == _FPP_SH4
  #define _FE_DIVBYZERO	0x08
  #define _FE_INEXACT	0x01
  #define _FE_INVALID	0x10
  #define _FE_OVERFLOW	0x04
  #define _FE_UNDERFLOW	0x02

 #elif _FPP_TYPE == _FPP_IA64
  #define _FE_DIVBYZERO	0x04
  #define _FE_INEXACT	0x20
  #define _FE_INVALID	0x01
  #define _FE_OVERFLOW	0x08
  #define _FE_UNDERFLOW	0x10

 #elif _FPP_TYPE == _FPP_WIN
  #define _FE_DIVBYZERO	0x08
  #define _FE_INEXACT	0x01
  #define _FE_INVALID	0x10
  #define _FE_OVERFLOW	0x04
  #define _FE_UNDERFLOW	0x02

 #elif __XTENSA__
  /* By defining them this way, we never need to shift or mask.  */
  #define _FE_DIVBYZERO	0x08
  #define _FE_INEXACT	0x01
  #define _FE_INVALID	0x10
  #define _FE_OVERFLOW	0x04
  #define _FE_UNDERFLOW	0x02

 #else /* _FPP_TYPE == _FPP_NONE or unknown */
  #undef _FPP_TYPE
  #define _FPP_TYPE	_FPP_NONE

  #define _FE_DIVBYZERO	0x04	/* dummy same as Pentium */
  #define _FE_INEXACT	0x20
  #define _FE_INVALID	0x01
  #define _FE_OVERFLOW	0x08
  #define _FE_UNDERFLOW	0x10
 #endif /* _FPP_TYPE */

void _Feraise(int);

typedef union
	{	/* pun float types as integer array */
	unsigned short _Word[8];
	float _Float;
	double _Double;
	long double _Long_double;
	} _Dconst;

		/* double DECLARATIONS */
double _Cosh(double, double);
short _Dtest(double *);
double _Sinh(double, double);
double _Divide(double, double);
short _Exp(double *, double, long);
double _Log(double, int);
double _Recip(double);
double _Sin(double, unsigned int);
double _Sinx(double, unsigned int, int);

_EXTERN_CONST_DECL _Dconst _Denorm, _Hugeval, _Inf,
	_Nan, _Snan;

		/* float DECLARATIONS */
float _FCosh(float, float);
short _FDtest(float *);
float _FSinh(float, float);
float _FDivide(float, float);
short _FExp(float *, float, long);
float _FLog(float, int);
float _FRecip(float);
float _FSin(float, unsigned int);
float _FSinx(float, unsigned int, int);

_EXTERN_CONST_DECL _Dconst _FDenorm, _FInf, _FNan, _FSnan;

		/* long double DECLARATIONS */
long double _LCosh(long double, long double);
short _LDtest(long double *);
long double _LSinh(long double, long double);
long double _LDivide(long double, long double);
short _LExp(long double *, long double, long);
long double _LLog(long double, int);
long double _LRecip(long double);
long double _LSin(long double, unsigned int);
long double _LSinx(long double, unsigned int, int);

_EXTERN_CONST_DECL _Dconst _LDenorm, _LInf, _LNan, _LSnan;

 #if defined(__SUNPRO_CC)	/* compiler test */
float fmodf(float, float);
long double fmodl(long double, long double);
 #endif /* defined(__SUNPRO_CC) */

 #if defined(__BORLANDC__)	/* compiler test */
float fmodf(float, float);
float logf(float);
 #endif /* defined(__BORLANDC__) */

_END_C_LIB_DECL
_C_STD_END
#endif /* _YMATH */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
