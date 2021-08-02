/* xtgmath0.h header for EDG and GCC
 */

 #if __EDG__
  #define _CARGR(x, fd, ff, fl) \
	__generic(x,,, fd, ff, fl,,,)(x)
  #define _CARGRI(return_type, x, fd, ff, fl) \
	__generic(x,,, fd, ff, fl,,,)(x)
  #define _CARGCX(x, cfd, cff, cfl) \
	__generic(x,,, cfd, cff, cfl, cfd, cff, cfl)(x)
  #define _CARGRC(x, fd, ff, fl, cfd, cff, cfl) \
	__generic(x,,, fd, ff, fl, cfd, cff, cfl)(x)
  #define _CARGRCX(x, fd, ff, fl, cfd, cff, cfl) \
	__generic(x,,, fd, ff, fl, cfd, cff, cfl)(x)
  #define _CARG2R(x, y, fd, ff, fl) \
	__generic(x, y,, fd, ff, fl,,,)(x, y)
  #define _CARG2RX(x, y, fd, ff, fl) \
	__generic(x,,, fd, ff, fl,,,)(x, y)
  #define _CARG2RC(x, y, fd, ff, fl, cfd, cff, cfl) \
	__generic(x, y,, fd, ff, fl, cfd, cff, cfl)(x, y)
  #define _CARG3R(x, y, z, fd, ff, fl) \
	__generic(x, y, z, fd, ff, fl,,,)(x, y, z)
  #define _CARG3RX(x, y, z, fd, ff, fl) \
	__generic(x, y,, fd, ff, fl,,,)(x, y, z)

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
  #if defined(TENSILICA) || 1
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
  #else
  #define _GCCARG3R(x, y, z, args, fund, funf, funl) \
__builtin_choose_expr( \
	__builtin_types_compatible_p(__typeof__(x), long double) \
		|| __builtin_types_compatible_p(__typeof__(y), long double) \
		|| __builtin_types_compatible_p(__typeof__(x), long double), \
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
  #endif

/* real or complex, one argument, six functions */
  #define _GCCARGRC(x, args, fund, funf, funl, cfund, cfunf, cfunl) \
__builtin_choose_expr( \
	__builtin_classify_type(x) == 9, \
	__builtin_choose_expr( \
		__builtin_types_compatible_p(__typeof__(__real__ x), long double), \
		cfunl args, \
		__builtin_choose_expr( \
			__builtin_types_compatible_p(__typeof__(__real__ x), double) \
				|| __builtin_classify_type(__real__ x) == 1, \
			cfund args, \
			cfunf args \
			) \
		), \
	__builtin_choose_expr( \
		__builtin_types_compatible_p(__typeof__(__real__ x), long double), \
		funl args, \
		__builtin_choose_expr( \
			__builtin_types_compatible_p(__typeof__(__real__ x), double) \
				|| __builtin_classify_type(__real__ x) == 1, \
			funf args, \
			fund args \
			) \
		) \
	)

/* real and/or complex, two arguments, six functions */
  #define _GCCARG2C(x, y, args, fund, funf, funl, cfund, cfunf, cfunl) \
__builtin_choose_expr( \
	__builtin_classify_type(x) == 9 \
		|| __builtin_classify_type(y) == 9, \
	__builtin_choose_expr( \
		__builtin_types_compatible_p(__typeof__(__real__ x), long double) \
			|| __builtin_types_compatible_p(__typeof__(__real__ y), \
				long double), \
		cfunl args, \
		__builtin_choose_expr( \
			__builtin_types_compatible_p(__typeof__(__real__ x), double) \
				|| __builtin_classify_type(x) == 1 \
				|| __builtin_types_compatible_p(__typeof__(__real__ y), \
					double) \
				|| __builtin_classify_type(y) == 1, \
			cfund args, \
			cfunf args \
			) \
		), \
	__builtin_choose_expr( \
		__builtin_types_compatible_p(__typeof__(__real__ x), long double) \
			|| __builtin_types_compatible_p(__typeof__(__real__ y), \
				long double), \
		funl args, \
		__builtin_choose_expr( \
			__builtin_types_compatible_p(__typeof__(x), double) \
				|| __builtin_classify_type(x) == 1 \
				|| __builtin_types_compatible_p(__typeof__(y), double) \
				|| __builtin_classify_type(y) == 1, \
			fund args, \
			funf args \
			) \
		) \
	)

  #define _CARGR(x, fd, ff, fl) \
	_GCCARGX(x, (x), fd, ff, fl)
  #define _CARGRI(return_type, x, fd, ff, fl) \
	_GCCARGX(x, (x), fd, ff, fl)
  #define _CARGCX(x, cfd, cff, cfl) \
	_GCCARGX(x, (x), cfd, cff, cfl)
  #define _CARGRC(x, fd, ff, fl, cfd, cff, cfl) \
	_GCCARGRC(x, (x), ff, fd, fl, cfd, cff, cfl)
  #define _CARGRCX(x, fd, ff, fl, cfd, cff, cfl) \
	_GCCARGRC(x, (x), ff, fd, fl, cfd, cff, cfl)
  #define _CARG2R(x, y, fd, ff, fl) \
	_GCCARG3R(x, y, y, (x, y), fd, ff, fl)
  #define _CARG2RX(x, y, fd, ff, fl) \
	_GCCARGX(x, (x, y), fd, ff, fl)
  #define _CARG2RC(x, y, fd, ff, fl, cfd, cff, cfl) \
	_GCCARG2C(x, y, (x, y), fd, ff, fl, cfd, cff, cfl)
  #define _CARG3R(x, y, z, fd, ff, fl) \
	_GCCARG3R(x, y, z, (x, y, z), fd, ff, fl)
  #define _CARG3RX(x, y, z, fd, ff, fl) \
	_GCCARG3R(x, y, y, (x, y, z), fd, ff, fl)

 #elif 2 <= __GNUC__ || defined(__APPLE__) && 3 <= __GNUC__
  #define _FLT_TYPE(expression)	_FLT_OR_DBL(__typeof__ (expression))

  #define _FLT_OR_DBL(mytype)	__typeof__ (*(0 \
	? (__typeof__ (0 ? (mytype *)0 \
		: (void *)!((mytype)0.5 && (mytype)0.5 - 1)))0 \
	: (__typeof__ (0 ? (double *)0 \
		: (void *)((mytype)0.5 && (mytype)0.5 - 1)))0))

  #define _CARGR(x, fd, ff, fl) \
	(__extension__({ \
	_FLT_TYPE(x) _Ans; \
	if (sizeof (_Ans) == sizeof (double)) \
		_Ans = fd(x); \
	else if (sizeof (_Ans) == sizeof (float)) \
	_Ans = ff(x); \
	else \
		_Ans = fl(x); \
	_Ans; \
	}))

  #define _CARGRI(return_type, x, fd, ff, fl) \
	(__extension__({ \
	return_type _Ans; \
	if (sizeof (_FLT_TYPE(x)) == sizeof (double)) \
		_Ans = fd(x); \
	else if (sizeof (_FLT_TYPE(x)) == sizeof (float)) \
		_Ans = ff(x); \
	else \
		_Ans = fl(x); \
	_Ans; \
	}))

  #define _CARGCX(x, cfd, cff, cfl) \
	(__extension__({ \
	_FLT_TYPE(__real__ (x)) _Ans; \
	if (sizeof (__real__ (_Ans)) == sizeof (double)) \
		_Ans = cfd(x); \
	else if (sizeof (__real__ (_Ans)) == sizeof (float)) \
		_Ans = cff(x); \
	else \
		_Ans = cfl(x); \
	_Ans; \
	}))

  #define _CARGRC(x, fd, ff, fl, cfd, cff, cfl) \
	(__extension__({ \
	_FLT_TYPE(x) _Ans; \
	if (sizeof (__real__ (_Ans)) == sizeof (double)) \
		if (sizeof (_Ans) == sizeof (double)) \
			_Ans = fd(x); \
		else \
			_Ans = cfd(x); \
	else if (sizeof (__real__ (_Ans)) == sizeof (float)) \
		if (sizeof (_Ans) == sizeof (float)) \
			_Ans = ff(x); \
		else \
			_Ans = cff(x); \
	else \
		if (sizeof (_Ans) == sizeof (long double)) \
			_Ans = fl(x); \
		else \
			_Ans = cfl(x); \
	_Ans; \
	}))

  #define _CARGRCX(x, fd, ff, fl, cfd, cff, cfl) \
	(__extension__({ \
	_FLT_TYPE(__real (x)) _Ans; \
	if (sizeof (_Ans) == sizeof (double)) \
		if (sizeof (x) == sizeof (double)) \
			_Ans = fd(x); \
		else \
			_Ans = cfd(x); \
	else if (sizeof (_Ans) == sizeof (float)) \
		if (sizeof (x) == sizeof (float)) \
			_Ans = ff(x); \
		else \
			_Ans = cff(x); \
	else \
		if (sizeof (x) == sizeof (long double)) \
			_Ans = fl(x); \
		else \
			_Ans = cfl(x); \
	_Ans; \
	}))

  #define _CARG2R(x, y, fd, ff, fl) \
	(__extension__({ \
	_FLT_TYPE((x) + (y)) _Ans; \
	if (sizeof (_Ans) == sizeof (double)) \
		_Ans = fd(x, y); \
	else if (sizeof (_Ans) == sizeof (float)) \
		_Ans = ff(x, y); \
	else \
		_Ans = fl(x, y); \
	_Ans; \
	}))

  #define _CARG2RX(x, y, fd, ff, fl) \
	(__extension__({ \
	_FLT_TYPE(x) _Ans; \
	if (sizeof (_Ans) == sizeof (double)) \
		_Ans = fd(x, y); \
	else if (sizeof (_Ans) == sizeof (float)) \
		_Ans = ff(x, y); \
	else \
		_Ans = fl(x, y); \
	_Ans; \
	}))

  #define _CARG2RC(x, y, fd, ff, fl, cfd, cff, cfl) \
	(__extension__({ \
	_FLT_TYPE((x) + (y)) _Ans; \
	if (sizeof (__real__ (_Ans)) == sizeof (double)) \
		if (sizeof (_Ans) == sizeof (double)) \
			_Ans = fd(x, y); \
		else \
			_Ans = cfd(x, y); \
	else if (sizeof (__real__ (_Ans)) == sizeof (float)) \
		if (sizeof (_Ans) == sizeof (float)) \
			_Ans = ff(x, y); \
		else \
			_Ans = cff(x, y); \
	else \
		if (sizeof (_Ans) == sizeof (long double)) \
			_Ans = fl(x, y); \
		else \
			_Ans = cfl(x, y); \
	_Ans; \
	}))

  #define _CARG3R(x, y, z, fd, ff, fl) \
	(__extension__({ \
	_FLT_TYPE((x) + (y) + (z)) _Ans; \
	if (sizeof (_Ans) == sizeof (double)) \
		_Ans = fd(x, y, z); \
	else if (sizeof (_Ans) == sizeof (float)) \
		_Ans = ff(x, y, z); \
	else \
		_Ans = fl(x, y, z); \
	_Ans; \
	}))

  #define _CARG3RX(x, y, z, fd, ff, fl) \
	(__extension__({ \
	_FLT_TYPE((x) + (y)) _Ans; \
	if (sizeof (_Ans) == sizeof (double)) \
		_Ans = fd(x, y, z); \
	else if (sizeof (_Ans) == sizeof (float)) \
		_Ans = ff(x, y, z); \
	else \
		_Ans = fl(x, y, z); \
	_Ans; \
	}))

 #else /* compiler type */
  #error type-generic math functions not supported
 #endif /* compiler type */

_C_STD_BEGIN
#undef acos
#define acos(x) \
	_CARGRC(x, acos, acosf, acosl, cacos, cacosf, cacosl)
#undef acosh
#define acosh(x) \
	_CARGRC(x, acosh, acoshf, acoshl, cacosh, cacoshf, cacoshl)
#undef asin
#define asin(x) \
	_CARGRC(x, asin, asinf, asinl, casin, casinf, casinl)
#undef asinh
#define asinh(x) \
	_CARGRC(x, asinh, asinhf, asinhl, casinh, casinhf, casinhl)
#undef atan
#define atan(x) \
	_CARGRC(x, atan, atanf, atanl, catan, catanf, catanl)
#undef atanh
#define atanh(x) \
	_CARGRC(x, atanh, atanhf, atanhl, catanh, catanhf, catanhl)
#undef atan2
#define atan2(y, x) \
	_CARG2R(y, x, atan2, atan2f, atan2l)
#undef carg
#define carg(x) \
	_CARGCX(x, carg, cargf, cargl)
#undef cbrt
#define cbrt(x) \
	_CARGR(x, cbrt, cbrtf, cbrtl)
#undef ceil
#define ceil(x) \
	_CARGR(x, ceil, ceilf, ceill)
#undef cimag
#define cimag(x) \
	_CARGCX(x, cimag, cimagf, cimagl)
#undef conj
#define conj(x) \
	_CARGRCX(x, conj, conjf, conjl, conj, conjf, conjl)
#undef copysign
#define copysign(x, y) \
	_CARG2R(x, y, copysign, copysignf, copysignl)
#undef cos
#define cos(x) \
	_CARGRC(x, cos, cosf, cosl, ccos, ccosf, ccosl)
#undef cosh
#define cosh(x) \
	_CARGRC(x, cosh, coshf, coshl, ccosh, ccoshf, ccoshl)
#undef cproj
#define cproj(x) \
	_CARGRCX(x, cproj, cprojf, cprojl, cproj, cprojf, cprojl)
#undef creal
#define creal(x) \
	_CARGCX(x, creal, crealf, creall)
#undef erf
#define erf(x) \
	_CARGR(x, erf, erff, erfl)
#undef erfc
#define erfc(x) \
	_CARGR(x, erfc, erfcf, erfcl)
#undef exp
#define exp(x) \
	_CARGRC(x, exp, expf, expl, cexp, cexpf, cexpl)
#undef exp2
#define exp2(x) \
	_CARGR(x, exp2, exp2f, exp2l)
#undef expm1
#define expm1(x) \
	_CARGR(x, expm1, expm1f, expm1l)
#undef fabs
#define fabs(x) \
	_CARGRCX(x, fabs, fabsf, fabsl, cabs, cabsf, cabsl)
#undef fdim
#define fdim(x, y) \
	_CARG2R(x, y, fdim, fdimf, fdiml)
#undef floor
#define floor(x) \
	_CARGR(x, floor, floorf, floorl)
#undef fma
#define fma(x, y, z) \
	_CARG3R(x, y, z, fma, fmaf, fmal)
#undef fmax
#define fmax(x, y) \
	_CARG2R(x, y, fmax, fmaxf, fmaxl)
#undef fmin
#define fmin(x, y) \
	_CARG2R(x, y, fmin, fminf, fminl)
#undef fmod
#define fmod(x, y) \
	_CARG2R(x, y, fmod, fmodf, fmodl)
#undef frexp
#define frexp(x, y) \
	_CARG2RX(x, y, frexp, frexpf, frexpl)
#undef hypot
#define hypot(x, y) \
	_CARG2R(x, y, hypot, hypotf, hypotl)
#undef ilogb
#define ilogb(x) \
	_CARGRI(int, x, ilogb, ilogbf, ilogbl)
#undef ldexp
#define ldexp(x, y) \
	_CARG2RX(x, y, ldexp, ldexpf, ldexpl)
#undef lgamma
#define lgamma(x) \
	_CARGR(x, lgamma, lgammaf, lgammal)
#undef llrint
#define llrint(x) \
	_CARGRI(_Longlong, x, llrint, llrintf, llrintl)
#undef llround
#define llround(x) \
	_CARGRI(_Longlong, x, llround, llroundf, llroundl)
#undef log
#define log(x) \
	_CARGRC(x, log, logf, logl, clog, clogf, clogl)
#undef log1p
#define log1p(x) \
	_CARGR(x, log1p, log1pf, log1pl)
#undef log10
#define log10(x) \
	_CARGR(x, log10, log10f, log10l)
#undef log2
#define log2(x) \
	_CARGR(x, log2, log2f, log2l)
#undef logb
#define logb(x) \
	_CARGR(x, logb, logbf, logbl)
#undef lrint
#define lrint(x) \
	_CARGRI(long, x, lrint, lrintf, lrintl)
#undef lround
#define lround(x) \
	_CARGRI(long, x, lround, lroundf, lroundl)
#undef nearbyint
#define nearbyint(x) \
	_CARGR(x, nearbyint, nearbyintf, nearbyintl)
#undef nextafter
#define nextafter(x, y) \
	_CARG2R(x, y, nextafter, nextafterf, nextafterl)
#undef nexttoward
#define nexttoward(x, y) \
	_CARG2RX(x, y, nexttoward, nexttowardf, nexttowardl)
#undef pow
#define pow(x, y) \
	_CARG2RC(x, y, pow, powf, powl, cpow, cpowf, cpowl)
#undef remainder
#define remainder(x, y) \
	_CARG2R(x, y, remainder, remainderf, remainderl)
#undef remquo
#define remquo(x, y, z) \
	_CARG3RX(x, y, z, remquo, remquof, remquol)
#undef rint
#define rint(x) \
	_CARGR(x, rint, rintf, rintl)
#undef round
#define round(x) \
	_CARGR(x, round, roundf, roundl)
#undef scalbln
#define scalbln(x, y) \
	_CARG2RX(x, y, scalbln, scalblnf, scalblnl)
#undef scalbn
#define scalbn(x, y) \
	_CARG2RX(x, y, scalbn, scalbnf, scalbnl)
#undef sin
#define sin(x) \
	_CARGRC(x, sin, sinf, sinl, csin, csinf, csinl)
#undef sinh
#define sinh(x) \
	_CARGRC(x, sinh, sinhf, sinhl, csinh, csinhf, csinhl)
#undef sqrt
#define sqrt(x) \
	_CARGRC(x, sqrt, sqrtf, sqrtl, csqrt, csqrtf, csqrtl)
#undef tan
#define tan(x) \
	_CARGRC(x, tan, tanf, tanl, ctan, ctanf, ctanl)
#undef tanh
#define tanh(x) \
	_CARGRC(x, tanh, tanhf, tanhl, ctanh, ctanhf, ctanhl)
#undef tgamma
#define tgamma(x) \
	_CARGR(x, tgamma, tgammaf, tgammal)
#undef trunc
#define trunc(x) \
	_CARGR(x, trunc, truncf, truncl)

 #if _HAS_CPP0X
#undef proj
#define proj(x) \
	_CARGRCX(x, cproj, cprojf, cprojl, cproj, cprojf, cprojl)
 #endif /* _HAS_CPP0X */
_C_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
