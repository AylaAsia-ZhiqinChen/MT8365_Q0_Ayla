/* xmath.h internal header */
#ifndef _XMATH
#define _XMATH
#include <ymath.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>

		/* FLOAT PROPERTIES */
#ifndef _D0
 #define _D0	3	/* little-endian, small long doubles */
 #define _D1	2
 #define _D2	1
 #define _D3	0

 #define _DBIAS	0x3fe
 #define _DOFF	4

 #define _FBIAS	0x7e
 #define _FOFF	7
 #define _FRND	1

 #define _DLONG	0
 #define _LBIAS	0x3fe
 #define _LOFF	4

#elif _D0 == 0		/* other params defined in <c99/yvals.h> */
 #define _D1	1	/* big-endian */
 #define _D2	2
 #define _D3	3

#else /* _D0 */
 #define _D1	2	/* little-endian */
 #define _D2	1
 #define _D3	0
#endif /* _D0 */

 #define _V0	0	/* Half properties */
 #define _VBIAS	0x0e
 #define _VOFF	10

		/* IEEE 754 double properties */
#define _DFRAC	((unsigned short)((1 << _DOFF) - 1))
#define _DMASK	((unsigned short)(0x7fff & ~_DFRAC))
#define _DMAX	((unsigned short)((1 << (15 - _DOFF)) - 1))
#define _DSIGN	((unsigned short)0x8000)

#define HUGE_EXP	(int)(_DMAX * 900L / 1000)

#define VHUGE_EXP	(int)(_VMAX * 900L / 1000)

		/* IEEE 754 float properties */
#define _FFRAC	((unsigned short)((1 << _FOFF) - 1))
#define _FMASK	((unsigned short)(0x7fff & ~_FFRAC))
#define _FMAX	((unsigned short)((1 << (15 - _FOFF)) - 1))
#define _FSIGN	((unsigned short)0x8000)

#define FHUGE_EXP	(int)(_FMAX * 900L / 1000)

 #if _D0 == 0
  #define _F0	0	/* big-endian */
  #define _F1	1

 #else /* _D0 == 0 */
  #define _F0	1	/* little-endian */
  #define _F1	0
 #endif /* _D0 == 0 */

		/* IEEE 754 long double properties */
#define _LFRAC	((unsigned short)(-1))
#define _LMASK	((unsigned short)0x7fff)
#define _LMAX	((unsigned short)0x7fff)
#define _LSIGN	((unsigned short)0x8000)

#define LHUGE_EXP	(int)(_LMAX * 900L / 1000)

 #if _D0 == 0
  #define _L0	0	/* big-endian */
  #define _L1	1
  #define _L2	2
  #define _L3	3
  #define _L4	4
  #define _L5	5	/* 128-bit only */
  #define _L6	6
  #define _L7	7
  #define _Lg	_L7

 #elif _DLONG == 0
  #define _L0	3	/* little-endian, 64-bit long doubles */
  #define _L1	2
  #define _L2	1
  #define _L3	0
  #define _Lg	_L3
  #define _L4	xxx	/* should never be used */
  #define _L5	xxx
  #define _L6	xxx
  #define _L7	xxx

 #elif _DLONG == 1
  #define _L0	4	/* little-endian, 80-bit long doubles */
  #define _L1	3
  #define _L2	2
  #define _L3	1
  #define _L4	0
  #define _Lg	_L4
  #define _L5	xxx	/* should never be used */
  #define _L6	xxx
  #define _L7	xxx

 #else /* _DLONG */
  #define _L0	7	/* little-endian, 128-bit long doubles */
  #define _L1	6
  #define _L2	5
  #define _L3	4
  #define _L4	3
  #define _L5	2
  #define _L6	1
  #define _L7	0
  #define _Lg	_L7
 #endif /* _DLONG */

 #define _Fg	_F1	/* least-significant 16-bit word */
 #define _Dg	_D3

		/* return values for _Stopfx/_Stoflt */
#define FL_ERR	0
#define FL_DEC	1
#define FL_HEX	2
#define FL_INF	3
#define FL_NAN	4
#define FL_NEG	8

#if TENSILICA || 1
/* Define macros that are used to access double, float, and long
   double with char *.  */

#if _D0 == 0
#define _CHAR_D0 0
#define _CHAR_F0 0
#define _CHAR_L0 0
#else
#define _CHAR_D0 7
#define _CHAR_F0 3
#define _CHAR_L0 7
#endif

#define _CHAR_DSIGN ((unsigned char) 0x80)
#define _CHAR_FSIGN ((unsigned char) 0x80)
#define _CHAR_LSIGN ((unsigned char) 0x80)

#endif

_C_STD_BEGIN
 #if !defined(MRTDLL) && !defined(_M_CEE_PURE)
_C_LIB_DECL
 #endif /* defined(MRTDLL) etc. */

_CRTIMP2P int _PCDECL _Stopfx(const char **, char **);
_CRTIMP2P int _PCDECL _Stoflt(const char *, const char *, char **,
	long[], int);
_CRTIMP2P int _PCDECL _Stoxflt(const char *, const char *, char **,
	long[], int);
_CRTIMP2P int _PCDECL _WStopfx(const wchar_t **, wchar_t **);
_CRTIMP2P int _PCDECL _WStoflt(const wchar_t *, const wchar_t *, wchar_t **,
	long[], int);
_CRTIMP2P int _PCDECL _WStoxflt(const wchar_t *, const wchar_t *, wchar_t **,
	long[], int);

		/* double declarations */
typedef union
	{	/* pun floating type as integer array */
	unsigned short _Sh[8];
	double _Val;
	} _Dval;

_CRTIMP2P unsigned short *_PCDECL _Plsw(double *);
_CRTIMP2P unsigned short *_PCDECL _Pmsw(double *);

_CRTIMP2P double _PCDECL _Atan(double, int);
_CRTIMP2P short _PCDECL _Dint(double *, short);
_CRTIMP2P short _PCDECL _Dnorm(_Dval *);
_CRTIMP2P short _PCDECL _Dscale(double *, long);
_CRTIMP2P short _PCDECL _Dscalex(double *, long, int);
_CRTIMP2P short _PCDECL _Dunscale(short *, double *);
_CRTIMP2P double _PCDECL _Hypot(double, double, int *);

 #if defined(__ARM_CLRUNTIME)	/* compiler test */
_CRTIMP2P double _PCDECL __attribute__((overloadable)) _Poly(double,
	const double *, int);
_CRTIMP2P double _PCDECL __attribute__((overloadable)) _Poly(double,
	_EXTERN_CONST double *, int);

 #else /* defined(__ARM_CLRUNTIME) */
_CRTIMP2P double _PCDECL _Poly(double, const double *, int);
 #endif /* defined(__ARM_CLRUNTIME) */

_CRTIMP2P _EXTERN_CONST_DECL _Dconst _Eps, _Rteps;
_CRTIMP2P _EXTERN_CONST_DECL double _Zero;

_CRTIMP2P double _PCDECL _Xp_getw(const double *, int);
_CRTIMP2P double *_PCDECL _Xp_setn(double *, int, long);
_CRTIMP2P double *_PCDECL _Xp_setw(double *, int, double);
_CRTIMP2P double *_PCDECL _Xp_addh(double *, int, double);
_CRTIMP2P double *_PCDECL _Xp_mulh(double *, int, double);
_CRTIMP2P double *_PCDECL _Xp_movx(double *, int, const double *);
_CRTIMP2P double *_PCDECL _Xp_addx(double *, int,
	const double *, int);
_CRTIMP2P double *_PCDECL _Xp_subx(double *, int,
	const double *, int);
_CRTIMP2P double *_PCDECL _Xp_ldexpx(double *, int, int);
_CRTIMP2P double *_PCDECL _Xp_mulx(double *, int,
	const double *, int, double *);
_CRTIMP2P double *_PCDECL _Xp_invx(double *, int, double *);
_CRTIMP2P double *_PCDECL _Xp_sqrtx(double *, int, double *);

		/* float declarations */
typedef union
	{	/* pun floating type as integer array */
	unsigned short _Sh[8];
	float _Val;
	} _Fval;

_CRTIMP2P unsigned short *_PCDECL _FPlsw(float *);
_CRTIMP2P unsigned short *_PCDECL _FPmsw(float *);

_CRTIMP2P float _PCDECL _FAtan(float, int);
_CRTIMP2P short _PCDECL _FDint(float *, short);
_CRTIMP2P short _PCDECL _FDnorm(_Fval *);
_CRTIMP2P short _PCDECL _FDscale(float *, long);
_CRTIMP2P short _PCDECL _FDscalex(float *, long, int);
_CRTIMP2P short _PCDECL _FDunscale(short *, float *);
_CRTIMP2P float _PCDECL _FHypot(float, float, int *);

 #if defined(__OXILI_CLRUNTIME)	/* compiler test */
_CRTIMP2P float _PCDECL __attribute__((overloadable)) _FPoly(float,
	const float *, int);
_CRTIMP2P float _PCDECL __attribute__((overloadable)) _FPoly(float,
	_EXTERN_CONST float *, int);

 #else /* defined(__OXILI_CLRUNTIME) */
_CRTIMP2P float _PCDECL _FPoly(float, const float *, int);
 #endif /* defined(__OXILI_CLRUNTIME) */

_CRTIMP2P _EXTERN_CONST_DECL _Dconst _FEps, _FRteps;
_CRTIMP2P _EXTERN_CONST_DECL float _FZero;

_CRTIMP2P float _PCDECL _FXp_getw(const float *, int);
_CRTIMP2P float *_PCDECL _FXp_setn(float *, int, long);
_CRTIMP2P float *_PCDECL _FXp_setw(float *, int, float);
_CRTIMP2P float *_PCDECL _FXp_addh(float *, int, float);
_CRTIMP2P float *_PCDECL _FXp_mulh(float *, int, float);
_CRTIMP2P float *_PCDECL _FXp_movx(float *, int, const float *);
_CRTIMP2P float *_PCDECL _FXp_addx(float *, int,
	const float *, int);
_CRTIMP2P float *_PCDECL _FXp_subx(float *, int,
	const float *, int);
_CRTIMP2P float *_PCDECL _FXp_ldexpx(float *, int, int);
_CRTIMP2P float *_PCDECL _FXp_mulx(float *, int,
	const float *, int, float *);
_CRTIMP2P float *_PCDECL _FXp_invx(float *, int, float *);
_CRTIMP2P float *_PCDECL _FXp_sqrtx(float *, int, float *);

		/* long double declarations */
typedef union
	{	/* pun floating type as integer array */
	unsigned short _Sh[8];
	long double _Val;
	} _Lval;

#define LISNEG(x)	(*_LPmsw(&(x)) & _LSIGN)
#define LMAKEPOS(x)	(*_LPmsw(&(x)) &= ~_LSIGN)
#define LNEGATE(x)	(*_LPmsw(&(x)) ^= _LSIGN)

_CRTIMP2P unsigned short *_PCDECL _LPlsw(long double *);
_CRTIMP2P unsigned short *_PCDECL _LPmsw(long double *);

_CRTIMP2P long double _PCDECL _LAtan(long double, int);
_CRTIMP2P short _PCDECL _LDint(long double *, short);
_CRTIMP2P short _PCDECL _LDnorm(_Lval *);
_CRTIMP2P short _PCDECL _LDscale(long double *, long);
_CRTIMP2P short _PCDECL _LDscalex(long double *, long, int);
_CRTIMP2P short _PCDECL _LDunscale(short *, long double *);
_CRTIMP2P long double _PCDECL _LHypot(long double, long double, int *);
_CRTIMP2P long double _PCDECL _LPoly(long double, const long double *, int);

_CRTIMP2P _EXTERN_CONST_DECL _Dconst _LEps, _LRteps;
_CRTIMP2P _EXTERN_CONST_DECL long double _LZero;

_CRTIMP2P long double _PCDECL _LXp_getw(const long double *, int);
_CRTIMP2P long double *_PCDECL _LXp_setn(long double *, int, long);
_CRTIMP2P long double *_PCDECL _LXp_setw(long double *, int, long double);
_CRTIMP2P long double *_PCDECL _LXp_addh(long double *, int, long double);
_CRTIMP2P long double *_PCDECL _LXp_mulh(long double *, int, long double);
_CRTIMP2P long double *_PCDECL _LXp_movx(long double *, int,
	const long double *);
_CRTIMP2P long double *_PCDECL _LXp_addx(long double *, int,
	const long double *, int);
_CRTIMP2P long double *_PCDECL _LXp_subx(long double *, int,
	const long double *, int);
_CRTIMP2P long double *_PCDECL _LXp_ldexpx(long double *, int, int);
_CRTIMP2P long double *_PCDECL _LXp_mulx(long double *, int,
	const long double *, int, long double *);
_CRTIMP2P long double *_PCDECL _LXp_invx(long double *, int, long double *);
_CRTIMP2P long double *_PCDECL _LXp_sqrtx(long double *, int, long double *);

 #if !defined(MRTDLL) && !defined(_M_CEE_PURE)
_END_C_LIB_DECL
 #endif /* !defined(MRTDLL) etc. */
_C_STD_END

 #if !defined(_FINVERT)
  #define _FINVERT(x)	(FLIT(1.0) / (x))
 #endif /* !defined(_FINVERT) */

 #if !defined(_FDIV)
  #define _FDIV(x, y)	((x) / (y))
 #endif /* !defined(_FDIV) */

#endif /* _XMATH */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
