/* math.h standard header */
#ifndef _MATH
#define _MATH
#include <ymath.h>

 #if defined(__cplusplus)
#include <xtgmath.h>
 #endif /* defined(__cplusplus) */

#include <xtensa/config/core.h>
#if XCHAL_HAVE_PDX && !defined(__NO_INLINE_FPCONVERT) && __XTENSA__ && __XCC__
#include <xtensa/tie/xt_pdxn.h>
#else
#if XCHAL_HAVE_DFP && !defined(__NO_INLINE_FPCONVERT) && __XTENSA__ && __XCC__
#include <xtensa/tie/xt_DFP.h>
#else
#if XCHAL_HAVE_FP && !defined(__NO_INLINE_FPCONVERT) && __XTENSA__ && __XCC__
#include <xtensa/tie/xt_FP.h>
#endif
#endif
#endif

#if XCHAL_HAVE_HIFI3_VFPU==1 || XCHAL_HAVE_HIFI4_VFPU==1 || XCHAL_HAVE_FUSION_FP || XCHAL_HAVE_FUSIONG_SP_VFPU==1 || XCHAL_HAVE_FUSIONG_DP_VFPU==1 || XCHAL_HAVE_PDX4==1
#define USE_VFPU 1
#endif

#if XCHAL_HAVE_HIFI3_VFPU==1 || XCHAL_HAVE_HIFI4_VFPU==1 || XCHAL_HAVE_FUSION_FP
#define VFPU_RESULT_FIRST 1
#endif

#if USE_VFPU
#include <stdint.h>
typedef float float32_t;
typedef double float64_t;
#endif

_C_STD_BEGIN
		/* CODES FOR is* FUNCTIONS */
#define _FP_LT	1
#define _FP_EQ	2
#define _FP_GT	4

		/* CODES FOR ilogb FUNCTIONS */

 #if _ILONG
  #define _FP_ILOGB0	(-0x7fffffff - _C2)
  #define _FP_ILOGBNAN	0x7fffffff

 #else /* _ILONG */
  #define _FP_ILOGB0	(-0x7fff - _C2)
  #define _FP_ILOGBNAN	0x7fff
 #endif /* _ILONG */

 #if _HAS_C9X
		/* TYPES */

 #if _FEVAL == 1
typedef double float_t;
typedef double double_t;

 #elif _FEVAL == 2
typedef long double float_t;
typedef long double double_t;

 #else /* _FEVAL */
typedef float float_t;
typedef double double_t;
 #endif /* _FEVAL */

		/* MACROS */

 #if 245 < __EDG_VERSION__ && !defined(__cplusplus)
  #define HUGE_VAL	((double)__INFINITY__)
  #define HUGE_VALF	__INFINITY__
  #define HUGE_VALL	((long double)__INFINITY__)
  #define INFINITY	__INFINITY__
  #define NAN		__NAN__

 #elif defined(__EDG__) || 0 < _MSC_VER
  #define HUGE_VAL	_CSTD _Inf._Double
  #define HUGE_VALF	_CSTD _FInf._Float
  #define HUGE_VALL	_CSTD _LInf._Long_double
  #define INFINITY	_CSTD _FInf._Float
  #define NAN		_CSTD _FNan._Float

 #else /* !defined(__EDG__) */
  #ifndef _HUGE_ENUF
   #define _HUGE_ENUF	1e+300	/* _HUGE_ENUF*_HUGE_ENUF must overflow */
  #endif /* _HUGE_ENUF */

  #define HUGE_VAL	((double)INFINITY)
  #define HUGE_VALF	((float)INFINITY)
  #define HUGE_VALL	((long double)INFINITY)
  #define INFINITY	((float)(_HUGE_ENUF * _HUGE_ENUF))
  #define NAN		((float)(INFINITY * 0.0F))
 #endif /* 245 < __EDG_VERSION__ */

#define FP_INFINITE		_INFCODE
#define FP_NAN			_NANCODE
#define FP_NORMAL		_FINITE
#define FP_SUBNORMAL	_DENORM
#define FP_ZERO			0

 #if _HAS_C9X_FAST_FMA
  #define FP_FAST_FMA	1
  #define FP_FAST_FMAF	1
  #define FP_FAST_FMAL	1
 #endif /* _HAS_C9X_FAST_FMA */

 #define FP_ILOGB0		_FP_ILOGB0
 #define FP_ILOGBNAN	_FP_ILOGBNAN

#define MATH_ERRNO			1
#define MATH_ERREXCEPT		2
#define math_errhandling	(MATH_ERRNO | MATH_ERREXCEPT)	/* do both */

_C_LIB_DECL
_CRTIMP2 int _CDECL _FFpcomp(float, float);
_CRTIMP2 int _CDECL _Fpcomp(double, double);
_CRTIMP2 int _CDECL _LFpcomp(long double, long double);

_CRTIMP2 int _CDECL _FDclass(float);
_CRTIMP2 int _CDECL _Dclass(double);
_CRTIMP2 int _CDECL _LDclass(long double);

_CRTIMP2 int _CDECL _FDsign(float);
_CRTIMP2 int _CDECL _Dsign(double);
_CRTIMP2 int _CDECL _LDsign(long double);
_END_C_LIB_DECL

 #if defined(__cplusplus)
		// TEMPLATE FUNCTION _FPCOMP
inline int _FPCOMP(float _Left, float _Right)
	{	// compare _Left and _Right
	return (_FFpcomp(_Left, _Right));
	}

inline int _FPCOMP(double _Left, double _Right)
	{	// compare _Left and _Right
	return (_Fpcomp(_Left, _Right));
	}

inline int _FPCOMP(long double _Left, long double _Right)
	{	// compare _Left and _Right
	return (_LFpcomp(_Left, _Right));
	}

template<class _Ty1,
	class _Ty2> inline
	int _FPCOMP(_Ty1 _Left, _Ty2 _Right)
	{	// compare two floating-point arguments
	typedef typename _STD _Common_float_type<_Ty1, _Ty2>::type type;
	return (_FPCOMP((type)_Left, (type)_Right));
	}

		// TEMPLATE FUNCTION fpclassify
inline int fpclassify(float _Left)
	{	// classify argument
	return (_FDtest(&_Left));
	}

inline int fpclassify(double _Left)
	{	// classify argument
	return (_Dtest(&_Left));
	}

inline int fpclassify(long double _Left)
	{	// classify argument
	return (_LDtest(&_Left));
	}

template<class _Ty> inline
	int fpclassify(_Ty _Left)
	{	// classify floating-point argument
	typedef typename _STD _Promote_to_float<_Ty>::type type;
	return (_CSTD fpclassify((type)_Left));
	}

		// TEMPLATE FUNCTION signbit
inline bool signbit(float _Left)
	{	// test sign bit
	return (_FDsign(_Left) != 0);
	}

inline bool signbit(double _Left)
	{	// test sign bit
	return (_Dsign(_Left) != 0);
	}

inline bool signbit(long double _Left)
	{	// test sign bit
	return (_LDsign(_Left) != 0);
	}

template<class _Ty> inline
	bool signbit(_Ty _Left)
	{	// test floating-point sign bit
	typedef typename _STD _Promote_to_float<_Ty>::type type;
	return (_CSTD signbit((type)_Left));
	}

		// CLASSIFICATION TEMPLATES
template<class _Ty> inline
	bool isfinite(_Ty _Left)
	{	// test for finite
	return (fpclassify(_Left) <= 0);
	}

template<class _Ty> inline
	bool isinf(_Ty _Left)
	{	// test for infinite
	return (fpclassify(_Left) == FP_INFINITE);
	}

template<class _Ty> inline
	bool isnan(_Ty _Left)
	{	// test for NaN
	return (fpclassify(_Left) == FP_NAN);
	}

template<class _Ty> inline
	bool isnormal(_Ty _Left)
	{	// test for normal
	return (fpclassify(_Left) == FP_NORMAL);
	}

		// COMPARISON TEMPLATES
template<class _Ty1, class _Ty2> inline
	bool isgreater(_Ty1 _Left, _Ty2 _Right)
	{	// test for _Left > _Right
	return ((_FPCOMP(_Left, _Right) & _FP_GT) != 0);
	}

template<class _Ty1, class _Ty2> inline
	bool isgreaterequal(_Ty1 _Left, _Ty2 _Right)
	{	// test for _Left >= _Right
	return ((_FPCOMP(_Left, _Right) & (_FP_EQ | _FP_GT)) != 0);
	}

template<class _Ty1, class _Ty2> inline
	bool isless(_Ty1 _Left, _Ty2 _Right)
	{	// test for _Left < _Right
	return ((_FPCOMP(_Left, _Right) & _FP_LT) != 0);
	}

template<class _Ty1, class _Ty2> inline
	bool islessequal(_Ty1 _Left, _Ty2 _Right)
	{	// test for _Left <= _Right
	return ((_FPCOMP(_Left, _Right) & (_FP_LT | _FP_EQ)) != 0);
	}

template<class _Ty1, class _Ty2> inline
	bool islessgreater(_Ty1 _Left, _Ty2 _Right)
	{	// test for _Left != _Right
	return ((_FPCOMP(_Left, _Right) & (_FP_LT | _FP_GT)) != 0);
	}

template<class _Ty1, class _Ty2> inline
	bool isunordered(_Ty1 _Left, _Ty2 _Right)
	{	// test for _Left unorderd w.r.t. _Right
	return (_FPCOMP(_Left, _Right) == 0);
	}

 #else /* defined(__cplusplus) */

 #if _CLANG	/* compiler test */
 #ifndef _Tg_promote
#define _Tg_promote(From, To) \
	static To __attribute__((__overloadable__)) __tg_promote(From)

_Tg_promote(int, double);
_Tg_promote(unsigned int, double);
_Tg_promote(long, double);
_Tg_promote(unsigned long, double);
_Tg_promote(long long, double);
_Tg_promote(unsigned long long, double);
_Tg_promote(float, float);
_Tg_promote(double, double);
_Tg_promote(long double, long double);

_Tg_promote(float _Complex, float _Complex);
_Tg_promote(double _Complex, double _Complex);
_Tg_promote(long double _Complex, long double _Complex);

static void __tg_promote(...)
	__attribute__((__overloadable__, __unavailable__));

#define _Tg_overload	__attribute__((__overloadable__, __always_inline__))

#define _Prom1(x)	(__typeof__(__tg_promote(x)))
#define _Prom2(x, y)	(__typeof__(__tg_promote(x) + __tg_promote(y)))
#define _Prom3(x, y, z)	(__typeof__(__tg_promote(x) + __tg_promote(y) \
	+ __tg_promote(z)))
 #endif /* _Tg_promote */

#define _Tg_R(fn) \
static int _Tg_overload _Tg_##fn(float _Left) \
	{return (_F##fn(_Left));} \
static int _Tg_overload _Tg_##fn(double _Left) \
	{return (_##fn(_Left));} \
static int _Tg_overload _Tg_##fn(long double _Left) \
	{return (_L##fn(_Left));}
#define _Tg_R2i(fn, rty) \
static rty _Tg_overload _Tg_##fn(float _Left, float _Right) \
	{return (_F##fn(_Left, _Right));} \
static rty _Tg_overload _Tg_##fn(double _Left, double _Right) \
	{return (_##fn(_Left, _Right));} \
static rty _Tg_overload \
		_Tg_##fn(long double _Left, long double _Right) \
	{return (_L##fn(_Left, _Right));}
_Tg_R2i(Fpcomp, int)
#undef _FPCOMP
#define _FPCOMP(x, y)	_Tg_Fpcomp(_Prom2(x, y)(x), _Prom2(x, y)(y))

_Tg_R(Dclass)
#undef fpclassify
#define fpclassify(x)	_Tg_Dclass(_Prom1(x)(x))

_Tg_R(Dsign)
#undef signbit
#define signbit(x)	_Tg_Dsign(_Prom1(x)(x))

 #else /* _CLANG */

  #if __EDG__
   #define _CARGR(x, fd, ff, fl) \
	__generic(x,,, fd, ff, fl,,,)(x)
   #define _CARG2R(x, y, fd, ff, fl) \
	__generic(x, y,, fd, ff, fl,,,)(x, y)

  #elif 4 <= __GNUC__
/* real or complex, one argument, three functions */
  #define _GCCARGX(x, args, fund, funf, funl) \
__builtin_choose_expr( \
	__builtin_types_compatible_p(__typeof__(__real__ x), long double), \
	funl args, \
	__builtin_choose_expr( \
		__builtin_types_compatible_p(__typeof__(__real__ x), double) \
			|| __builtin_classify_type(__real__ x) == 1, \
		fund args, \
		funf args \
		) \
	)

/* real, three arguments, three functions */
  #define _GCCARG3R(x, y, z, args, fund, funf, funl) \
__builtin_choose_expr( \
	__builtin_types_compatible_p(__typeof__(x), long double) \
		|| __builtin_types_compatible_p(__typeof__(y), long double) \
		|| __builtin_types_compatible_p(__typeof__(z), long double), \
	funl args, \
	__builtin_choose_expr( \
		__builtin_types_compatible_p(__typeof__(x), double) \
			|| __builtin_classify_type(x) == 1 \
			||	__builtin_types_compatible_p(__typeof__(y), double) \
			|| __builtin_classify_type(y) == 1 \
			||	__builtin_types_compatible_p(__typeof__(z), double) \
			|| __builtin_classify_type(z) == 1, \
		fund args, \
		funf args \
		) \
	)

  #define _CARGR(x, fd, ff, fl) \
	_GCCARGX(x, (x), fd, ff, fl)
  #define _CARG2R(x, y, fd, ff, fl) \
	_GCCARG3R(x, y, y, (x, y), fd, ff, fl)

  #elif 2 <= __GNUC__
   #define _FLT_TYPE(expression)	_FLT_OR_DBL(__typeof__ (expression))

  #define _FLT_OR_DBL(mytype)	__typeof__ (*(0 \
	? (__typeof__ (0 ? (mytype *)0 \
		: (void *)!((mytype)0.5 && (mytype)0.5 - 1)))0 \
	: (__typeof__ (0 ? (double *)0 \
		: (void *)((mytype)0.5 && (mytype)0.5 - 1)))0))

   #define _CARGR(x, fd, ff, fl) \
	(__extension__({ \
	int _Ans; \
	if (sizeof (_FLT_TYPE(x)) == sizeof (double)) \
		_Ans = fd(x); \
	else if (sizeof (_FLT_TYPE(x)) == sizeof (float)) \
	_Ans = ff(x); \
	else \
		_Ans = fl(x); \
	_Ans; \
	}))

   #define _CARG2R(x, y, fd, ff, fl) \
	(__extension__({ \
	int _Ans; \
	if (sizeof (_FLT_TYPE((x) + (y))) == sizeof (double)) \
		_Ans = fd(x, y); \
	else if (sizeof (_FLT_TYPE((x) + (y))) == sizeof (float)) \
		_Ans = ff(x, y); \
	else \
		_Ans = fl(x, y); \
	_Ans; \
	}))

  #else /* compiler type */
   #define _ARG(x)	(sizeof ((x) + (float)0) == sizeof (float) ? 'f' \
	: sizeof ((x) + (double)0) == sizeof (double) ? 'd' \
	: 'l')
   #define _CARGR(x, fd, ff, fl)	\
	(_ARG(x) == 'f' ? ff((float)(x)) \
	: _ARG(x) == 'd' ? fd((double)(x)) \
	: fl((long double)(x)))
   #define _CARG2R(x, y, fd, ff, fl)	\
	(_ARG((x) + (y)) == 'f' ? ff((float)(x), (float)(y)) \
	: _ARG((x) + (y)) == 'd' ? fd((double)(x), (double)(y)) \
	: fl((long double)(x), (long double)(y)))
  #endif /* compiler type */

  #define _FPCOMP(x, y) \
	_CARG2R(x, y, _Fpcomp, _FFpcomp, _LFpcomp)
  #define fpclassify(x) \
	_CARGR(x, _Dclass, _FDclass, _LDclass)
  #define signbit(x) \
	_CARGR(x, _Dsign, _FDsign, _LDsign)
 #endif /* _CLANG */

  #define isfinite(x)	(fpclassify(x) <= 0)
  #define isinf(x)		(fpclassify(x) == FP_INFINITE)
  #define isnan(x)		(fpclassify(x) == FP_NAN)
  #define isnormal(x)	(fpclassify(x) == FP_NORMAL)

  #define isgreater(x, y)	\
	((_FPCOMP(x, y) & _FP_GT) != 0)
  #define isgreaterequal(x, y)	\
	((_FPCOMP(x, y) & (_FP_EQ | _FP_GT)) != 0)
  #define isless(x, y)	\
	((_FPCOMP(x, y) & _FP_LT) != 0)
  #define islessequal(x, y)	\
	((_FPCOMP(x, y) & (_FP_LT | _FP_EQ)) != 0)
  #define islessgreater(x, y)	\
	((_FPCOMP(x, y) & (_FP_LT | _FP_GT)) != 0)
  #define isunordered(x, y)	\
	(_FPCOMP(x, y) == 0)
 #endif /* defined(__cplusplus) */

 #else /* _HAS_C9X */
		/* MACROS */
 #define HUGE_VAL	_CSTD _Hugeval._Double
 #endif /* _IS_C9X */

_C_LIB_DECL
#ifdef __GNUC_GNU_INLINE__
#define __MATH_H_EINLINE extern inline
#else 
#define __MATH_H_EINLINE inline
#endif
#if defined(XT_FICEIL_S) && !defined(__NO_INLINE_FPCONVERT) && __XTENSA__ && __XCC__
__MATH_H_EINLINE xtfloat ceilf(xtfloat f) {return XT_FICEIL_S(f);}
__MATH_H_EINLINE xtfloat floorf(xtfloat f) {return XT_FIFLOOR_S(f);}
__MATH_H_EINLINE xtfloat roundf(xtfloat f) {return XT_FIROUND_S(f);}
__MATH_H_EINLINE xtfloat truncf(xtfloat f) {return XT_FITRUNC_S(f);}
__MATH_H_EINLINE xtfloat rintf(xtfloat f) {return XT_FIRINT_S(f);}
#if XCHAL_HAVE_PDX
__MATH_H_EINLINE xtfloat nearbyintf(xtfloat f) {
    unsigned uorig = XT_RUR_FSR();
    xtfloat o = XT_FIRINT_S(f);
    vbool1 vb = XT_UNEQ_S (o, f) & XT_OEQ_S (f, f);
    xtbool xb = vb;
    unsigned u = XT_RUR_FSR();
    if (xb) {
     u = uorig;
    }
    XT_WUR_FSR (u);
    return o;
}
#else
float nearbyintf(float);
#endif
#else
float ceilf(float);
float floorf(float);
float rintf(float);
float nearbyintf(float);
float roundf(float);
float truncf(float);
#endif
#if defined(XT_FICEIL_D) && !defined(__NO_INLINE_FPCONVERT) && __XTENSA__ && __XCC__
__MATH_H_EINLINE xtdouble ceil(xtdouble f) {return XT_FICEIL_D(f);}
__MATH_H_EINLINE xtdouble floor(xtdouble f) {return XT_FIFLOOR_D(f);}
__MATH_H_EINLINE xtdouble round(xtdouble f) {return XT_FIROUND_D(f);}
__MATH_H_EINLINE xtdouble trunc(xtdouble f) {return XT_FITRUNC_D(f);}
__MATH_H_EINLINE xtdouble rint(xtdouble f) {return XT_FIRINT_D(f);}

#if XCHAL_HAVE_PDX
__MATH_H_EINLINE xtdouble nearbyint(xtdouble f) {
    unsigned uorig = XT_RUR_FSR();
    xtdouble o = XT_FIRINT_D(f);
    vbool1 vb = XT_UNEQ_D (o, f) & XT_OEQ_D (f, f);
    xtbool xb = vb;
    unsigned u = XT_RUR_FSR();
    if (xb) {
     u = uorig;
    }
    XT_WUR_FSR (u);
    return o;
}
#else
double nearbyint(double);
#endif

#else
double ceil(double);
double floor(double);
double rint(double);
double nearbyint(double);
double round(double);
double trunc(double);
#endif
		/* double declarations */
_CRTIMP2 double _CDECL acos(double);
_CRTIMP2 double _CDECL asin(double);
_CRTIMP2 double _CDECL atan(double);
_CRTIMP2 double _CDECL atan2(double, double);
_CRTIMP2 double _CDECL exp(double);
_CRTIMP2 double _CDECL fabs(double);
_CRTIMP2 double _CDECL fmod(double, double);
_CRTIMP2 double _CDECL frexp(double, int *);
_CRTIMP2 double _CDECL ldexp(double, int);
_CRTIMP2 double _CDECL modf(double, double *);
_CRTIMP2 double _CDECL pow(double, double);
_CRTIMP2 double _CDECL sqrt(double);
_CRTIMP2 double _CDECL tan(double);
_CRTIMP2 double _CDECL tanh(double);

 #if _HAS_C9X
_CRTIMP2 double _CDECL acosh(double);
_CRTIMP2 double _CDECL asinh(double);
_CRTIMP2 double _CDECL atanh(double);
_CRTIMP2 double _CDECL cbrt(double);
_CRTIMP2 double _CDECL copysign(double, double);
_CRTIMP2 double _CDECL erf(double);
_CRTIMP2 double _CDECL erfc(double);
_CRTIMP2 double _CDECL exp2(double);
_CRTIMP2 double _CDECL expm1(double);
_CRTIMP2 double _CDECL fdim(double, double);
_CRTIMP2 double _CDECL fma(double, double, double);
_CRTIMP2 double _CDECL fmax(double, double);
_CRTIMP2 double _CDECL fmin(double, double);
_CRTIMP2 double _CDECL hypot(double, double);
_CRTIMP2 int _CDECL ilogb(double);
_CRTIMP2 double _CDECL lgamma(double);
_CRTIMP2 _Longlong _CDECL llrint(double);
_CRTIMP2 _Longlong _CDECL llround(double);
_CRTIMP2 double _CDECL log1p(double);
_CRTIMP2 double _CDECL logb(double);
_CRTIMP2 long _CDECL lrint(double);
_CRTIMP2 long _CDECL lround(double);
_CRTIMP2 double _CDECL nan(const char *);
_CRTIMP2 double _CDECL nextafter(double, double);
_CRTIMP2 double _CDECL nexttoward(double, long double);
_CRTIMP2 double _CDECL remainder(double, double);
_CRTIMP2 double _CDECL remquo(double, double, int *);
_CRTIMP2 double _CDECL scalbn(double, int);
_CRTIMP2 double _CDECL scalbln(double, long);
_CRTIMP2 double _CDECL tgamma(double);
 #endif /* _IS_C9X */

		/* float declarations */
_CRTIMP2 float _CDECL acosf(float);
_CRTIMP2 float _CDECL asinf(float);
_CRTIMP2 float _CDECL atanf(float);
_CRTIMP2 float _CDECL atan2f(float, float);
_CRTIMP2 float _CDECL expf(float);
_CRTIMP2 float _CDECL fabsf(float);
_CRTIMP2 float _CDECL fmodf(float, float);
_CRTIMP2 float _CDECL frexpf(float, int *);
_CRTIMP2 float _CDECL ldexpf(float, int);
_CRTIMP2 float _CDECL modff(float, float *);
_CRTIMP2 float _CDECL powf(float, float);
_CRTIMP2 float _CDECL sqrtf(float);
_CRTIMP2 float _CDECL tanf(float);
_CRTIMP2 float _CDECL tanhf(float);

 #if _HAS_C9X
_CRTIMP2 float _CDECL acoshf(float);
_CRTIMP2 float _CDECL asinhf(float);
_CRTIMP2 float _CDECL atanhf(float);
_CRTIMP2 float _CDECL cbrtf(float);
_CRTIMP2 float _CDECL copysignf(float, float);
_CRTIMP2 float _CDECL erff(float);
_CRTIMP2 float _CDECL erfcf(float);
_CRTIMP2 float _CDECL expm1f(float);
_CRTIMP2 float _CDECL exp2f(float);
_CRTIMP2 float _CDECL fdimf(float, float);
_CRTIMP2 float _CDECL fmaf(float, float, float);
_CRTIMP2 float _CDECL fmaxf(float, float);
_CRTIMP2 float _CDECL fminf(float, float);
_CRTIMP2 float _CDECL hypotf(float, float);
_CRTIMP2 int _CDECL ilogbf(float);
_CRTIMP2 float _CDECL lgammaf(float);
_CRTIMP2 _Longlong _CDECL llrintf(float);
_CRTIMP2 _Longlong _CDECL llroundf(float);
_CRTIMP2 float _CDECL log1pf(float);
_CRTIMP2 float _CDECL logbf(float);
_CRTIMP2 long _CDECL lrintf(float);
_CRTIMP2 long _CDECL lroundf(float);
_CRTIMP2 float _CDECL nanf(const char *);
_CRTIMP2 float _CDECL nextafterf(float, float);
_CRTIMP2 float _CDECL nexttowardf(float, long double);
_CRTIMP2 float _CDECL remainderf(float, float);
_CRTIMP2 float _CDECL remquof(float, float, int *);
_CRTIMP2 float _CDECL scalbnf(float, int);
_CRTIMP2 float _CDECL scalblnf(float, long);
_CRTIMP2 float _CDECL tgammaf(float);
 #endif /* _IS_C9X */

		/* long double declarations */
_CRTIMP2 long double _CDECL acosl(long double);
_CRTIMP2 long double _CDECL asinl(long double);
_CRTIMP2 long double _CDECL atanl(long double);
_CRTIMP2 long double _CDECL atan2l(long double, long double);
_CRTIMP2 long double _CDECL ceill(long double);
_CRTIMP2 long double _CDECL expl(long double);
_CRTIMP2 long double _CDECL fabsl(long double);
_CRTIMP2 long double _CDECL floorl(long double);
_CRTIMP2 long double _CDECL fmodl(long double, long double);
_CRTIMP2 long double _CDECL frexpl(long double, int *);
_CRTIMP2 long double _CDECL ldexpl(long double, int);
_CRTIMP2 long double _CDECL modfl(long double, long double *);
_CRTIMP2 long double _CDECL powl(long double, long double);
_CRTIMP2 long double _CDECL sqrtl(long double);
_CRTIMP2 long double _CDECL tanl(long double);
_CRTIMP2 long double _CDECL tanhl(long double);

 #if _HAS_C9X
_CRTIMP2 long double _CDECL acoshl(long double);
_CRTIMP2 long double _CDECL asinhl(long double);
_CRTIMP2 long double _CDECL atanhl(long double);
_CRTIMP2 long double _CDECL cbrtl(long double);
_CRTIMP2 long double _CDECL copysignl(long double, long double);
_CRTIMP2 long double _CDECL erfl(long double);
_CRTIMP2 long double _CDECL erfcl(long double);
_CRTIMP2 long double _CDECL exp2l(long double);
_CRTIMP2 long double _CDECL expm1l(long double);
_CRTIMP2 long double _CDECL fdiml(long double, long double);
_CRTIMP2 long double _CDECL fmal(long double, long double, long double);
_CRTIMP2 long double _CDECL fmaxl(long double, long double);
_CRTIMP2 long double _CDECL fminl(long double, long double);
_CRTIMP2 long double _CDECL hypotl(long double, long double);
_CRTIMP2 int _CDECL ilogbl(long double);
_CRTIMP2 long double _CDECL lgammal(long double);
_CRTIMP2 _Longlong _CDECL llrintl(long double);
_CRTIMP2 _Longlong _CDECL llroundl(long double);
_CRTIMP2 long double _CDECL log1pl(long double);
_CRTIMP2 long double _CDECL logbl(long double);
_CRTIMP2 long _CDECL lrintl(long double);
_CRTIMP2 long _CDECL lroundl(long double);
_CRTIMP2 long double _CDECL nanl(const char *);
_CRTIMP2 long double _CDECL nearbyintl(long double);
_CRTIMP2 long double _CDECL nextafterl(long double, long double);
_CRTIMP2 long double _CDECL nexttowardl(long double, long double);
_CRTIMP2 long double _CDECL remainderl(long double, long double);
_CRTIMP2 long double _CDECL remquol(long double, long double, int *);
_CRTIMP2 long double _CDECL rintl(long double);
_CRTIMP2 long double _CDECL roundl(long double);
_CRTIMP2 long double _CDECL scalbnl(long double, int);
_CRTIMP2 long double _CDECL scalblnl(long double, long);
_CRTIMP2 long double _CDECL tgammal(long double);
_CRTIMP2 long double _CDECL truncl(long double);
 #endif /* _IS_C9X */

#if (USE_VFPU && !VFPU_RESULT_FIRST)
		/* vector float declarations */
_CRTIMP2 void vecacosf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vecalog10f(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vecasinf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vecatanf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vecatan2f(const float32_t *y, const float32_t *x, float32_t *z, int n);
_CRTIMP2 void veccosf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void veccoshf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vecexpf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vecfmodf(const float32_t *x, const float32_t *y, float32_t *z, int n);
_CRTIMP2 void vecldexpf(const float32_t *x, const int32_t *n, float32_t *z, int n2);
_CRTIMP2 void veclogf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void veclog10f(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vecsinf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vecsinhf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vectanf(const float32_t *x, float32_t *z, int n);
_CRTIMP2 void vectanhf(const float32_t *x, float32_t *z, int n);
#endif /* USE_VFPU && !VFPU_RESULT_FIRST */

#if (XCHAL_HAVE_FUSIONG_DP_VFPU==1 || XCHAL_HAVE_PDX4==1)
		/* vector float declaration (requires DP VFPU) */
_CRTIMP2 void vecpowf(const float32_t *x, const float32_t *y, float32_t *z, int n);
		/* vector double declarations */
_CRTIMP2 void vecacos(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vecalog10(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vecasin(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vecatan(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vecatan2(const float64_t *y, const float64_t *x, float64_t *z, int n);
_CRTIMP2 void veccos(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void veccosh(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vecexp(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vecfmod(const float64_t *x, const float64_t *y, float64_t *z, int n);
_CRTIMP2 void vecldexp(const float64_t *x, const int32_t *n, float64_t *z, int n2);
_CRTIMP2 void veclog(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void veclog10(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vecpow(const float64_t *x, const float64_t *y, float64_t *z, int n);
_CRTIMP2 void vecsin(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vecsinh(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vectan(const float64_t *x, float64_t *z, int n);
_CRTIMP2 void vectanh(const float64_t *x, float64_t *z, int n);
#endif /* (XCHAL_HAVE_FUSION_DP_VFPU==1 || XCHAL_HAVE_PDX4==1) */

#ifdef VFPU_RESULT_FIRST
		/* vector float declarations */
_CRTIMP2 void vecacosf(float32_t * _Restrict z, const float32_t * _Restrict x, int n);
_CRTIMP2 void vecasinf(float32_t * _Restrict z, const float32_t * _Restrict x, int n);
_CRTIMP2 void vecatanf(float32_t * _Restrict z, const float32_t * _Restrict x, int n);
_CRTIMP2 void vecatan2f(float32_t *z, const float32_t *y, const float32_t *x, int n);
_CRTIMP2 void veccosf(float32_t * _Restrict y, const float32_t * _Restrict x, int n);
_CRTIMP2 void veclogf(float32_t * _Restrict y, const float32_t * _Restrict x, int n);
_CRTIMP2 void veclog10f(float32_t * _Restrict y, const float32_t * _Restrict x, int n);
_CRTIMP2 void veclog2f(float32_t * _Restrict y, const float32_t * _Restrict x, int n);
_CRTIMP2 void vecsinf(float32_t * _Restrict y, const float32_t * _Restrict x, int n);
_CRTIMP2 void vectanf(float32_t * _Restrict y, const float32_t * _Restrict x, int n);
#endif /* VFPU_RESULT_FIRST */

#if defined(__cplusplus)
inline double _CDECL cos(double _Left)
	{	// return cosine
	return (_Sinx(_Left, 1, 0));
	}

inline double _CDECL cosh(double _Left)
	{	// return hyperbolic cosine
	return (_Cosh(_Left, 1.0));
	}

inline double _CDECL log(double _Left)
	{	// return natural logarithm
	return (_Log(_Left, 0));
	}

inline double _CDECL log10(double _Left)
	{	// return base-10 logarithm
	return (_Log(_Left, 1));
	}

inline double _CDECL sin(double _Left)
	{	// return sine
	return (_Sinx(_Left, 0, 0));
	}

inline double _CDECL sinh(double _Left)
	{	// return hyperbolic sine
	return (_Sinh(_Left, 1.0));
	}

 #if _HAS_C9X
inline double _CDECL log2(double _Left)
	{	// return base-2 logarithm
	return (_Log(_Left, -1));
	}
 #endif /* _IS_C9X */

 #endif /* defined(__cplusplus) */
_END_C_LIB_DECL

 #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)
		// double INLINES, FOR C++
inline double _CDECL abs(double _Left)	// OVERLOADS
	{	// return absolute value
	return (fabs(_Left));
	}

		// float INLINES, FOR C++
_C_LIB_DECL

#if defined(USE_VFPU)
extern float _CDECL cosf(float);
#else
inline float _CDECL cosf(float _Left)
	{	// return cosine
	return (_FSinx(_Left, 1, 0));
	}
#endif

#if defined(USE_VFPU)
extern float _CDECL coshf(float _Left);
#else
inline float _CDECL coshf(float _Left)
	{	// return hyperbolic cosine
	return (_FCosh(_Left, 1.0F));
	}
#endif

#if defined(USE_VFPU)
extern float _CDECL logf(float);
#else
inline float _CDECL logf(float _Left)
	{	// return natural logarithm
	return (_FLog(_Left, 0));
	}
#endif

#if defined(USE_VFPU)
extern float _CDECL log10f(float);
#else
inline float _CDECL log10f(float _Left)
	{	// return base-10 logarithm
	return (_FLog(_Left, 1));
	}
#endif

#if defined(USE_VFPU)
extern float _CDECL sinf(float);
#else
inline float _CDECL sinf(float _Left)
	{	// return sine
	return (_FSinx(_Left, 0, 0));
	}
#endif

#if defined(USE_VFPU)
extern float _CDECL sinhf(float _Left);
#else
inline float _CDECL sinhf(float _Left)
	{	// return hyperbolic sine
	return (_FSinh(_Left, 1.0F));
	}
#endif

 #if _HAS_C9X
#if defined(USE_VFPU)
extern float _CDECL log2f(float);
#else
inline float _CDECL log2f(float _Left)
	{	// return base-2 logarithm
	return (_FLog(_Left, -1));
	}
#endif
 #endif /* _IS_C9X */

_END_C_LIB_DECL

inline float _CDECL abs(float _Left)	// OVERLOADS
	{	// return absolute value
	return (fabsf(_Left));
	}

inline float _CDECL acos(float _Left)
	{	// return arccosine
	return (acosf(_Left));
	}

inline float _CDECL asin(float _Left)
	{	// return arcsine
	return (asinf(_Left));
	}

inline float _CDECL atan(float _Left)
	{	// return arctangent
	return (atanf(_Left));
	}

inline float _CDECL atan2(float _Left, float _Right)
	{	// return arctangent
	return (atan2f(_Left, _Right));
	}

inline float _CDECL ceil(float _Left)
	{	// return ceiling
	return (ceilf(_Left));
	}

inline float _CDECL cos(float _Left)
	{	// return cosine
	return (_FSinx(_Left, 1, 0));
	}

inline float _CDECL cosh(float _Left)
	{	// return hyperbolic cosine
	return (_FCosh(_Left, 1.0F));
	}

inline float _CDECL exp(float _Left)
	{	// return exponential
	return (expf(_Left));
	}

inline float _CDECL fabs(float _Left)
	{	// return absolute value
	return (fabsf(_Left));
	}

inline float _CDECL floor(float _Left)
	{	// return floor
	return (floorf(_Left));
	}

inline float _CDECL fmod(float _Left, float _Right)
	{	// return modulus
	return (fmodf(_Left, _Right));
	}

inline float _CDECL frexp(float _Left, int *_Right)
	{	// unpack exponent
	return (frexpf(_Left, _Right));
	}

inline float _CDECL ldexp(float _Left, int _Right)
	{	// pack exponent
	return (ldexpf(_Left, _Right));
	}

inline float _CDECL log(float _Left)
	{	// return natural logarithm
	return (_FLog(_Left, 0));
	}

inline float _CDECL log10(float _Left)
	{	// return base-10 logarithm
	return (_FLog(_Left, 1));
	}

inline float _CDECL modf(float _Left, float *_Right)
	{	// unpack fraction
	return (modff(_Left, _Right));
	}

inline float _CDECL pow(float _Left, float _Right)
	{	// raise to power
	return (powf(_Left, _Right));
	}

inline float _CDECL sin(float _Left)
	{	// return sine
	return (_FSinx(_Left, 0, 0));
	}

inline float _CDECL sinh(float _Left)
	{	// return hyperbolic sine
	return (_FSinh(_Left, 1.0F));
	}

inline float _CDECL sqrt(float _Left)
	{	// return square root
	return (sqrtf(_Left));
	}

inline float _CDECL tan(float _Left)
	{	// return tangent
	return (tanf(_Left));
	}

inline float _CDECL tanh(float _Left)
	{	// return hyperbolic tangent
	return (tanhf(_Left));
	}

 #if _HAS_C9X
inline float _CDECL acosh(float _Left)
	{	// return hyperbolic arccosine
	return (acoshf(_Left));
	}

inline float _CDECL asinh(float _Left)
	{	// return hyperbolic arcsine
	return (asinhf(_Left));
	}

inline float _CDECL atanh(float _Left)
	{	// return hyperbolic arctangent
	return (atanhf(_Left));
	}

inline float _CDECL cbrt(float _Left)
	{	// return cube root
	return (cbrtf(_Left));
	}

inline float _CDECL copysign(float _Left, float _Right)
	{	// return copysign
	return (copysignf(_Left, _Right));
	}

inline float _CDECL erf(float _Left)
	{	// return erf
 	return (erff(_Left)); }

inline float _CDECL erfc(float _Left)
	{	// return erfc
	return (erfcf(_Left));
	}

inline float _CDECL exp2(float _Left)
	{	// return exp2
	return (exp2f(_Left));
	}

inline float _CDECL expm1(float _Left)
	{	// return expml
	return (expm1f(_Left));
	}

inline float _CDECL fdim(float _Left, float _Right)
	{	// return fdim
	return (fdimf(_Left, _Right));
	}

inline float _CDECL fma(float _Left, float _Right, float _Addend)
	{	// return fma
	return (fmaf(_Left, _Right, _Addend));
	}

inline float _CDECL fmax(float _Left, float _Right)
	{	// return fmax
	return (fmaxf(_Left, _Right));
	}

inline float _CDECL fmin(float _Left, float _Right)
	{	// return fmin
	return (fminf(_Left, _Right));
	}

inline float _CDECL hypot(float _Left, float _Right)
	{	// return hypot
	return (hypotf(_Left, _Right));
	}

inline int _CDECL ilogb(float _Left)
	{	// return ilogb
	return (ilogbf(_Left));
	}

inline float _CDECL lgamma(float _Left)
	{	// return lgamma
	return (lgammaf(_Left));
	}

inline _Longlong _CDECL llrint(float _Left)
	{	// return llrint
	return (llrintf(_Left));
	}

inline _Longlong _CDECL llround(float _Left)
	{	// return llround
	return (llroundf(_Left));
	}

inline float _CDECL log1p(float _Left)
	{	// return loglp
	return (log1pf(_Left));
	}

inline float _CDECL log2(float _Left)
	{	// return log2
	return (_FLog(_Left, -1));
	}

inline float _CDECL logb(float _Left)
	{	// return logb
	return (logbf(_Left));
	}

inline long _CDECL lrint(float _Left)
	{	// return lrint
	return (lrintf(_Left));
	}

inline long _CDECL lround(float _Left)
	{	// return lround
	return (lroundf(_Left));
	}

inline float _CDECL nearbyint(float _Left)
	{	// return nearbyint
	return (nearbyintf(_Left));
	}

inline float _CDECL nextafter(float _Left, float _Right)
	{	// return nextafter
	return (nextafterf(_Left, _Right));
	}

inline float _CDECL nexttoward(float _Left, long double _Right)
	{	// return nexttoward
	return (nexttowardf(_Left, _Right));
	}

inline float _CDECL remainder(float _Left, float _Right)
	{	// return remainder
	return (remainderf(_Left, _Right));
	}

inline float _CDECL remquo(float _Left, float _Right, int *_Pval)
	{	// return remquo
	return (remquof(_Left, _Right, _Pval));
	}

inline float _CDECL rint(float _Left)
	{	// return rint
	return (rintf(_Left));
	}

inline float _CDECL round(float _Left)
	{	// return round
	return (roundf(_Left));
	}

inline float _CDECL scalbn(float _Left, int _Right)
	{	// return scalbn
	return (scalbnf(_Left, _Right));
	}

inline float _CDECL scalbln(float _Left, long _Right)
	{	// return scalbln
	return (scalblnf(_Left, _Right));
	}

inline float _CDECL tgamma(float _Left)
	{	// return tgamma
	return (tgammaf(_Left));
	}

inline float _CDECL trunc(float _Left)
	{	// return trunc
	return (truncf(_Left));
	}
 #endif /* _IS_C9X */

		// long double INLINES, FOR C++
_C_LIB_DECL

inline long double _CDECL cosl(long double _Left)
	{	// return cosine
	return (_LSinx(_Left, 1, 0));
	}

inline long double _CDECL coshl(long double _Left)
	{	// return hyperbolic cosine
	return (_LCosh(_Left, 1.0L));
	}

inline long double _CDECL logl(long double _Left)
	{	// return natural logarithm
	return (_LLog(_Left, 0));
	}

inline long double _CDECL log10l(long double _Left)
	{	// return base-10 logarithm
	return (_LLog(_Left, 1));
	}

inline long double _CDECL sinl(long double _Left)
	{	// return sine
	return (_LSinx(_Left, 0, 0));
	}

inline long double _CDECL sinhl(long double _Left)
	{	// return hyperbolic sine
	return (_LSinh(_Left, 1.0L));
	}

 #if _HAS_C9X
inline long double _CDECL log2l(long double _Left)
	{	// return base-2 logarithm
	return (_LLog(_Left, -1));
	}
 #endif /* _IS_C9X */

_END_C_LIB_DECL

inline long double _CDECL abs(long double _Left)	// OVERLOADS
	{	// return absolute value
	return (fabsl(_Left));
	}

inline long double _CDECL acos(long double _Left)
	{	// return arccosine
	return (acosl(_Left));
	}

inline long double _CDECL asin(long double _Left)
	{	// return arcsine
	return (asinl(_Left));
	}

inline long double _CDECL atan(long double _Left)
	{	// return arctangent
	return (atanl(_Left));
	}

inline long double _CDECL atan2(long double _Left, long double _Right)
	{	// return arctangent
	return (atan2l(_Left, _Right));
	}

inline long double _CDECL ceil(long double _Left)
	{	// return ceiling
	return (ceill(_Left));
	}

inline long double _CDECL cos(long double _Left)
	{	// return cosine
	return (_LSinx(_Left, 1, 0));
	}

inline long double _CDECL cosh(long double _Left)
	{	// return hyperbolic cosine
	return (_LCosh(_Left, 1.0L));
	}

inline long double _CDECL exp(long double _Left)
	{	// return exponential
	return (expl(_Left));
	}

inline long double _CDECL fabs(long double _Left)
	{	// return absolute value
	return (fabsl(_Left));
	}

inline long double _CDECL floor(long double _Left)
	{	// return floor
	return (floorl(_Left));
	}

inline long double _CDECL fmod(long double _Left, long double _Right)
	{	// return modulus
	return (fmodl(_Left, _Right));
	}

inline long double _CDECL frexp(long double _Left, int *_Right)
	{	// unpack exponent
	return (frexpl(_Left, _Right));
	}

inline long double _CDECL ldexp(long double _Left, int _Right)
	{	// pack exponent
	return (ldexpl(_Left, _Right));
	}

inline long double _CDECL log(long double _Left)
	{	// return natural logarithm
	return (_LLog(_Left, 0));
	}

inline long double _CDECL log10(long double _Left)
	{	// return base-10 logarithm
	return (_LLog(_Left, 1));
	}

inline long double _CDECL modf(long double _Left, long double *_Right)
	{	// unpack fraction
	return (modfl(_Left, _Right));
	}

inline long double _CDECL pow(long double _Left, long double _Right)
	{	// raise to power
	return (powl(_Left, _Right));
	}

inline long double _CDECL sin(long double _Left)
	{	// return sine
	return (_LSinx(_Left, 0, 0));
	}

inline long double _CDECL sinh(long double _Left)
	{	// return hyperbolic sine
	return (_LSinh(_Left, 1.0L));
	}

inline long double _CDECL sqrt(long double _Left)
	{	// return square root
	return (sqrtl(_Left));
	}

inline long double _CDECL tan(long double _Left)
	{	// return tangent
	return (tanl(_Left));
	}

inline long double _CDECL tanh(long double _Left)
	{	// return hyperbolic tangent
	return (tanhl(_Left));
	}

 #if _HAS_C9X
inline long double _CDECL acosh(long double _Left)
	{	// return acosh
	return (acoshl(_Left));
	}

inline long double _CDECL asinh(long double _Left)
	{	// return asinh
	return (asinhl(_Left));
	}

inline long double _CDECL atanh(long double _Left)
	{	// return atanh
	return (atanhl(_Left));
	}

inline long double _CDECL cbrt(long double _Left)
	{	// return cbrt
	return (cbrtl(_Left));
	}

inline long double _CDECL copysign(long double _Left, long double _Right)
	{	// return copysign
	return (copysignl(_Left, _Right));
	}

inline long double _CDECL erf(long double _Left)
	{	// return erf
	return (erfl(_Left));
	}

inline long double _CDECL erfc(long double _Left)
	{	// return erfc
	return (erfcl(_Left));
	}

inline long double _CDECL exp2(long double _Left)
	{	// return exp2
	return (exp2l(_Left));
	}

inline long double _CDECL expm1(long double _Left)
	{	// return expml
	return (expm1l(_Left));
	}

inline long double _CDECL fdim(long double _Left, long double _Right)
	{	// return fdim
	return (fdiml(_Left, _Right));
	}

inline long double _CDECL fma(long double _Left, long double _Right,
	long double _Addend)
	{	// return fma
	return (fmal(_Left, _Right, _Addend));
	}

inline long double _CDECL fmax(long double _Left, long double _Right)
	{	// return fmax
	return (fmaxl(_Left, _Right));
	}

inline long double _CDECL fmin(long double _Left, long double _Right)
	{	// return fmin
	return (fminl(_Left, _Right));
	}

inline long double _CDECL hypot(long double _Left, long double _Right)
	{	// return hypot
	return (hypotl(_Left, _Right));
	}

inline int _CDECL ilogb(long double _Left)
	{	// return ilogb
	return (ilogbl(_Left));
	}

inline long double _CDECL lgamma(long double _Left)
	{	// return lgamma
	return (lgammal(_Left));
	}

inline _Longlong _CDECL llrint(long double _Left)
	{	// return llrint
	return (llrintl(_Left));
	}

inline _Longlong _CDECL llround(long double _Left)
	{	// return llround
	return (llroundl(_Left));
	}

inline long double _CDECL log1p(long double _Left)
	{	// return loglp
	return (log1pl(_Left));
	}

inline long double _CDECL log2(long double _Left)
	{	// return log2
	return (_LLog(_Left, -1));
	}

inline long double _CDECL logb(long double _Left)
	{	// return logb
	return (logbl(_Left));
	}

inline long _CDECL lrint(long double _Left)
	{	// return lrint
	return (lrintl(_Left));
	}

inline long _CDECL lround(long double _Left)
	{	// return lround
	return (lroundl(_Left));
	}

inline long double _CDECL nearbyint(long double _Left)
	{	// return nearbyint
	return (nearbyintl(_Left));
	}

inline long double _CDECL nextafter(long double _Left, long double _Right)
	{	// return nextafter
	return (nextafterl(_Left, _Right));
	}

inline long double _CDECL nexttoward(long double _Left, long double _Right)
	{	// return nexttoward
	return (nexttowardl(_Left, _Right));
	}

inline long double _CDECL remainder(long double _Left, long double _Right)
	{	// return remainder
	return (remainderl(_Left, _Right));
	}

inline long double _CDECL remquo(long double _Left, long double _Right,
	int *_Pval)
	{	// return remquo
	return (remquol(_Left, _Right, _Pval));
	}

inline long double _CDECL rint(long double _Left)
	{	// return rint
	return (rintl(_Left));
	}

inline long double _CDECL round(long double _Left)
	{	// return round
	return (roundl(_Left));
	}

inline long double _CDECL scalbn(long double _Left, int _Right)
	{	// return scalbn
	return (scalbnl(_Left, _Right));
	}

inline long double _CDECL scalbln(long double _Left, long _Right)
	{	// return scalbln
	return (scalblnl(_Left, _Right));
	}

inline long double _CDECL tgamma(long double _Left)
	{	// return tgamma
	return (tgammal(_Left));
	}

inline long double _CDECL trunc(long double _Left)
	{	// return trunc
	return (truncl(_Left));
	}
 #endif /* _IS_C9X */

 #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
_C_LIB_DECL
		/* double MACRO OVERRIDES, FOR C */
_CRTIMP2 double _CDECL cos(double);
_CRTIMP2 double _CDECL cosh(double);
_CRTIMP2 double _CDECL log(double);
_CRTIMP2 double _CDECL log10(double);
_CRTIMP2 double _CDECL sin(double);
_CRTIMP2 double _CDECL sinh(double);

#if !defined(USE_VFPU)
 #define cos(x)		_Sinx(x, 1, 0)
 #define cosh(x)	_Cosh(x, 1.0)
 #define log(x)		_Log(x, 0)
 #define log10(x)	_Log(x, 1)
 #define sin(x)		_Sinx(x, 0, 0)
 #define sinh(x)	_Sinh(x, 1.0)
#endif

 #if _HAS_C9X
_CRTIMP2 double _CDECL log2(double);

 #define log2(x)	_Log(x, -1)
 #endif /* _IS_C9X */

		/* float MACRO OVERRIDES, FOR C */
_CRTIMP2 float _CDECL cosf(float);
_CRTIMP2 float _CDECL coshf(float);
_CRTIMP2 float _CDECL logf(float);
_CRTIMP2 float _CDECL log10f(float);
_CRTIMP2 float _CDECL sinf(float);
_CRTIMP2 float _CDECL sinhf(float);

#if !defined(USE_VFPU)
 #define cosf(x)	_FSinx(x, 1, 0)
 #define logf(x)	_FLog(x, 0)
 #define log10f(x)	_FLog(x, 1)
 #define sinf(x)	_FSinx(x, 0, 0)
 #define coshf(x)	_FCosh(x, 1.0F)
 #define sinhf(x)	_FSinh(x, 1.0F)
#endif

 #if _HAS_C9X
_CRTIMP2 float _CDECL log2f(float);

#if !defined(USE_VFPU)
 #define log2f(x)	_FLog(x, -1)
#endif
 #endif /* _IS_C9X */

		/* long double MACRO OVERRIDES, FOR C */
_CRTIMP2 long double _CDECL cosl(long double);
_CRTIMP2 long double _CDECL coshl(long double);
_CRTIMP2 long double _CDECL logl(long double);
_CRTIMP2 long double _CDECL log10l(long double);
_CRTIMP2 long double _CDECL sinl(long double);
_CRTIMP2 long double _CDECL sinhl(long double);

 #define cosl(x)	_LSinx(x, 1, 0)
 #define coshl(x)	_LCosh(x, 1.0L)
 #define logl(x)	_LLog(x, 0)
 #define log10l(x)	_LLog(x, 1)
 #define sinl(x)	_LSinx(x, 0, 0)
 #define sinhl(x)	_LSinh(x, 1.0L)

 #if _HAS_C9X
_CRTIMP2 long double _CDECL log2l(long double);

 #define log2l(x)	_LLog(x, -1)
 #endif /* _IS_C9X */

_END_C_LIB_DECL
 #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
_C_STD_END

#endif /* _MATH */

 #if defined(_STD_USING)

  #if !defined(_NO_CPP_INLINES)
using _CSTD abs;
  #endif /* !defined(_NO_CPP_INLINES) */

using _CSTD acos; using _CSTD asin;
using _CSTD atan; using _CSTD atan2; using _CSTD ceil;
using _CSTD cos; using _CSTD cosh; using _CSTD exp;
using _CSTD fabs; using _CSTD floor; using _CSTD fmod;
using _CSTD frexp; using _CSTD ldexp; using _CSTD log;
using _CSTD log10; using _CSTD modf; using _CSTD pow;
using _CSTD sin; using _CSTD sinh; using _CSTD sqrt;
using _CSTD tan; using _CSTD tanh;

using _CSTD acosf; using _CSTD asinf;
using _CSTD atanf; using _CSTD atan2f; using _CSTD ceilf;
using _CSTD cosf; using _CSTD coshf; using _CSTD expf;
using _CSTD fabsf; using _CSTD floorf; using _CSTD fmodf;
using _CSTD frexpf; using _CSTD ldexpf; using _CSTD logf;
using _CSTD log10f; using _CSTD modff; using _CSTD powf;
using _CSTD sinf; using _CSTD sinhf; using _CSTD sqrtf;
using _CSTD tanf; using _CSTD tanhf;

using _CSTD acosl; using _CSTD asinl;
using _CSTD atanl; using _CSTD atan2l; using _CSTD ceill;
using _CSTD cosl; using _CSTD coshl; using _CSTD expl;
using _CSTD fabsl; using _CSTD floorl; using _CSTD fmodl;
using _CSTD frexpl; using _CSTD ldexpl; using _CSTD logl;
using _CSTD log10l; using _CSTD modfl; using _CSTD powl;
using _CSTD sinl; using _CSTD sinhl; using _CSTD sqrtl;
using _CSTD tanl; using _CSTD tanhl;

 #if _HAS_C9X
using _CSTD float_t; using _CSTD double_t;

using _CSTD acosh; using _CSTD asinh; using _CSTD atanh;
using _CSTD cbrt; using _CSTD erf; using _CSTD erfc;
using _CSTD expm1; using _CSTD exp2;
using _CSTD hypot; using _CSTD ilogb; using _CSTD lgamma;
using _CSTD log1p; using _CSTD log2; using _CSTD logb;
using _CSTD llrint; using _CSTD lrint; using _CSTD nearbyint;
using _CSTD rint; using _CSTD llround; using _CSTD lround;
using _CSTD fdim; using _CSTD fma; using _CSTD fmax; using _CSTD fmin;
using _CSTD round; using _CSTD trunc;
using _CSTD remainder; using _CSTD remquo;
using _CSTD copysign; using _CSTD nan; using _CSTD nextafter;
using _CSTD scalbn; using _CSTD scalbln;
using _CSTD nexttoward; using _CSTD tgamma;

using _CSTD acoshf; using _CSTD asinhf; using _CSTD atanhf;
using _CSTD cbrtf; using _CSTD erff; using _CSTD erfcf;
using _CSTD expm1f; using _CSTD exp2f;
using _CSTD hypotf; using _CSTD ilogbf; using _CSTD lgammaf;
using _CSTD log1pf; using _CSTD log2f; using _CSTD logbf;
using _CSTD llrintf; using _CSTD lrintf; using _CSTD nearbyintf;
using _CSTD rintf; using _CSTD llroundf; using _CSTD lroundf;
using _CSTD fdimf; using _CSTD fmaf; using _CSTD fmaxf; using _CSTD fminf;
using _CSTD roundf; using _CSTD truncf;
using _CSTD remainderf; using _CSTD remquof;
using _CSTD copysignf; using _CSTD nanf;
using _CSTD nextafterf; using _CSTD scalbnf; using _CSTD scalblnf;
using _CSTD nexttowardf; using _CSTD tgammaf;

using _CSTD acoshl; using _CSTD asinhl; using _CSTD atanhl;
using _CSTD cbrtl; using _CSTD erfl; using _CSTD erfcl;
using _CSTD expm1l; using _CSTD exp2l;
using _CSTD hypotl; using _CSTD ilogbl; using _CSTD lgammal;
using _CSTD log1pl; using _CSTD log2l; using _CSTD logbl;
using _CSTD llrintl; using _CSTD lrintl; using _CSTD nearbyintl;
using _CSTD rintl; using _CSTD llroundl; using _CSTD lroundl;
using _CSTD fdiml; using _CSTD fmal; using _CSTD fmaxl; using _CSTD fminl;
using _CSTD roundl; using _CSTD truncl;
using _CSTD remainderl; using _CSTD remquol;
using _CSTD copysignl; using _CSTD nanl;
using _CSTD nextafterl; using _CSTD scalbnl; using _CSTD scalblnl;
using _CSTD nexttowardl; using _CSTD tgammal;

using _CSTD fpclassify; using _CSTD signbit;
using _CSTD isfinite; using _CSTD isinf;
using _CSTD isnan; using _CSTD isnormal;
using _CSTD isgreater; using _CSTD isgreaterequal;
using _CSTD isless; using _CSTD islessequal;
using _CSTD islessgreater; using _CSTD isunordered;
 #endif /* _IS_C9X */

 #endif /* defined(_STD_USING) */

#if TENSILICA || 1
#if (__STRICT_ANSI__ == 0)

/* Lots of implementations define these, even if they aren't strictly
   part of the standard.  */

#define MAXFLOAT        3.40282347e+38F

#define M_E             2.7182818284590452354
#define M_LOG2E         1.4426950408889634074
#define M_LOG10E        0.43429448190325182765
#define M_LN2           0.69314718055994530942
#define M_LN10          2.30258509299404568402
#define M_PI            3.14159265358979323846
#define M_TWOPI         (M_PI * 2.0)
#define M_PI_2          1.57079632679489661923
#define M_PI_4          0.78539816339744830962
#define M_3PI_4         2.3561944901923448370E0
#define M_SQRTPI        1.77245385090551602792981
#define M_1_PI          0.31830988618379067154
#define M_2_PI          0.63661977236758134308
#define M_2_SQRTPI      1.12837916709551257390
#define M_SQRT2         1.41421356237309504880
#define M_SQRT1_2       0.70710678118654752440
#define M_LN2LO         1.9082149292705877000E-10
#define M_LN2HI         6.9314718036912381649E-1
#define M_SQRT3         1.73205080756887719000
#define M_IVLN10        0.43429448190325182765 /* 1 / log(10) */
#define M_LOG2_E        0.693147180559945309417
#define M_INVLN2        1.4426950408889633870E0  /* 1 / log(2) */

#endif
#endif

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
