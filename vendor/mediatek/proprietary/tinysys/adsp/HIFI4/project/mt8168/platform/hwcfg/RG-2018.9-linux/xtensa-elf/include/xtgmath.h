/* xtgmath.h internal header */


#if __cplusplus < 201103
#include <c99/xtgmath.h>
#else /* rest of file */

 #if defined(__cplusplus)
#ifndef _XTGMATH
#define _XTGMATH
#include <xtr1common>

_STD_BEGIN
template<class _Ty>
	struct _Promote_to_float
	{	// promote integral to double
	typedef typename conditional<is_integral<_Ty>::value,
		double, _Ty>::type type;
	};

template<class _Ty1,
	class _Ty2>
	struct _Common_float_type
	{	// find type for two-argument math function
	typedef typename _Promote_to_float<_Ty1>::type _Ty1f;
	typedef typename _Promote_to_float<_Ty2>::type _Ty2f;
	typedef typename conditional<is_same<_Ty1f, long double>::value
		|| is_same<_Ty2f, long double>::value, long double,
		typename conditional<is_same<_Ty1f, double>::value
			|| is_same<_Ty2f, double>::value, double,
			float>::type>::type type;
	};
_STD_END

#define _CHECK_RETURN
#define _CRTDEFAULT
#define _CRTSPECIAL
#define _CRT_JIT_INTRINSIC

#define _GENERIC_MATH1(FUN, CRTTYPE) \
extern "C" _CHECK_RETURN CRTTYPE double FUN(double); \
template<class _Ty> inline \
	typename _STD enable_if< _STD is_integral<_Ty>::value, double>::type \
	FUN(_Ty _Left) \
	{ \
	return (_CSTD FUN((double)_Left)); \
	}

#define _GENERIC_MATH1X(FUN, ARG2, CRTTYPE) \
extern "C" _CHECK_RETURN CRTTYPE double FUN(double, ARG2); \
template<class _Ty> inline \
	typename _STD enable_if< _STD is_integral<_Ty>::value, double>::type \
	FUN(_Ty _Left, ARG2 _Arg2) \
	{ \
	return (_CSTD FUN((double)_Left, _Arg2)); \
	}

#define _GENERIC_MATH2_CALL(FUN, CRTTYPE, CALL_OPT) \
extern "C" _CHECK_RETURN CRTTYPE double CALL_OPT FUN( \
	double, double); \
template<class _Ty1, \
	class _Ty2> inline \
	typename _STD enable_if< _STD is_arithmetic<_Ty1>::value \
		&& _STD is_arithmetic<_Ty2>::value, \
			typename _STD _Common_float_type<_Ty1, _Ty2>::type>::type \
	FUN(_Ty1 _Left, _Ty2 _Right) \
	{ \
	typedef typename _STD _Common_float_type<_Ty1, _Ty2>::type type; \
	return (_CSTD FUN((type)_Left, (type)_Right)); \
	}

#define _GENERIC_MATH2(FUN, CRTTYPE) \
	_GENERIC_MATH2_CALL(FUN, CRTTYPE, )

_C_STD_BEGIN
extern "C" double pow(double, double);
float pow(float, float);
long double pow(long double, long double);

template<class _Ty1,
	class _Ty2> inline
	typename _STD enable_if< _STD is_arithmetic<_Ty1>::value
		&& _STD is_arithmetic<_Ty2>::value,
		typename _STD _Common_float_type<_Ty1, _Ty2>::type>::type
	pow(const _Ty1 _Left, const _Ty2 _Right)
	{	// bring mixed types to a common type
	typedef typename _STD _Common_float_type<_Ty1, _Ty2>::type type;
	return (_CSTD pow(type(_Left), type(_Right)));
	}

//_GENERIC_MATH1(abs, _CRTDEFAULT)	// has integer overloads
_GENERIC_MATH1(acos, _CRTDEFAULT)
_GENERIC_MATH1(asin, _CRTDEFAULT)
_GENERIC_MATH1(atan, _CRTDEFAULT)
_GENERIC_MATH2(atan2, _CRTDEFAULT)
_GENERIC_MATH1(ceil, _CRTSPECIAL)
_GENERIC_MATH1(cos, _CRTDEFAULT)
_GENERIC_MATH1(cosh, _CRTDEFAULT)
_GENERIC_MATH1(exp, _CRTDEFAULT)
_GENERIC_MATH1(floor, _CRTSPECIAL)
_GENERIC_MATH2(fmod, _CRTDEFAULT)
_GENERIC_MATH1X(frexp, int *, _CRTSPECIAL)
_GENERIC_MATH1X(ldexp, int, _CRTSPECIAL)
_GENERIC_MATH1(log, _CRTDEFAULT)
_GENERIC_MATH1(log10, _CRTDEFAULT)
//_GENERIC_MATH1(modf, _CRTDEFAULT)		// types must match
//_GENERIC_MATH2(pow, _CRTDEFAULT)	// hand crafted
_GENERIC_MATH1(sin, _CRTDEFAULT)
_GENERIC_MATH1(sinh, _CRTDEFAULT)
_GENERIC_MATH1(sqrt, _CRTDEFAULT)
_GENERIC_MATH1(tan, _CRTDEFAULT)
_GENERIC_MATH1(tanh, _CRTDEFAULT)

		// C99 MATH FUNCTIONS
#define _GENERIC_MATH1R(FUN, RET, CRTTYPE) \
extern "C" _CHECK_RETURN CRTTYPE RET FUN(double); \
template<class _Ty> inline \
	typename _STD enable_if< _STD is_integral<_Ty>::value, RET>::type \
	FUN(_Ty _Left) \
	{ \
	return (_CSTD FUN((double)_Left)); \
	}

		// TEMPLATE FUNCTION fma
#define _Fma	_CSTD fma

template<class _Ty1,
	class _Ty2,
	class _Ty3> inline
	typename _STD _Common_float_type<_Ty1,
		typename _STD _Common_float_type<_Ty2, _Ty3>::type>::type
	fma(_Ty1 _Left, _Ty2 _Middle, _Ty3 _Right)
	{	// bring mixed types to a common type
	typedef typename _STD _Common_float_type<_Ty1,
		typename _STD _Common_float_type<_Ty2, _Ty3>::type>::type type;
	return (_Fma((type)_Left, (type)_Middle, (type)_Right));
	}

		// TEMPLATE FUNCTION remquo
#define _Remquo	_CSTD remquo

template<class _Ty1,
	class _Ty2> inline
	typename _STD _Common_float_type<_Ty1, _Ty2>::type
	remquo(_Ty1 _Left, _Ty2 _Right, int *_Pquo)
	{	// bring mixed types to a common type
	typedef typename _STD _Common_float_type<_Ty1, _Ty2>::type type;
	return (_Remquo((type)_Left, (type)_Right, _Pquo));
	}

_GENERIC_MATH1(acosh, _CRTSPECIAL)
_GENERIC_MATH1(asinh, _CRTSPECIAL)
_GENERIC_MATH1(atanh, _CRTSPECIAL)
_GENERIC_MATH1(cbrt, _CRTSPECIAL)
_GENERIC_MATH2(copysign, _CRTSPECIAL)
_GENERIC_MATH1(erf, _CRTSPECIAL)
_GENERIC_MATH1(erfc, _CRTSPECIAL)
_GENERIC_MATH1(expm1, _CRTSPECIAL)
_GENERIC_MATH1(exp2, _CRTSPECIAL)
_GENERIC_MATH2(fdim, _CRTSPECIAL)
//_GENERIC_MATH3(fma, _CRTSPECIAL)	// hand crafted
_GENERIC_MATH2(fmax, _CRTSPECIAL)
_GENERIC_MATH2(fmin, _CRTSPECIAL)
_GENERIC_MATH2(hypot, _CRTSPECIAL)
_GENERIC_MATH1R(ilogb, int, _CRTSPECIAL)
_GENERIC_MATH1(lgamma, _CRTSPECIAL)
_GENERIC_MATH1R(llrint, long long, _CRTSPECIAL)
_GENERIC_MATH1R(llround, long long, _CRTSPECIAL)
_GENERIC_MATH1(log1p, _CRTSPECIAL)
_GENERIC_MATH1(log2, _CRTSPECIAL)
_GENERIC_MATH1(logb, _CRTSPECIAL)
_GENERIC_MATH1R(lrint, long, _CRTSPECIAL)
_GENERIC_MATH1R(lround, long, _CRTSPECIAL)
_GENERIC_MATH1(nearbyint, _CRTSPECIAL)
_GENERIC_MATH2(nextafter, _CRTSPECIAL)
_GENERIC_MATH1X(nexttoward, long double, _CRTSPECIAL)
_GENERIC_MATH2(remainder, _CRTSPECIAL)
//_GENERIC_MATH2X(remquo, _CRTSPECIAL)	// hand crafted
_GENERIC_MATH1(rint, _CRTSPECIAL)
_GENERIC_MATH1(round, _CRTSPECIAL)
_GENERIC_MATH1X(scalbln, long, _CRTSPECIAL)
_GENERIC_MATH1X(scalbn, int, _CRTSPECIAL)
_GENERIC_MATH1(tgamma, _CRTSPECIAL)
_GENERIC_MATH1(trunc, _CRTSPECIAL)
_C_STD_END
#endif /* _XTGMATH */

 #endif /* defined(__cplusplus) */

#endif /* __cplusplus < 201103 */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:1611 */
