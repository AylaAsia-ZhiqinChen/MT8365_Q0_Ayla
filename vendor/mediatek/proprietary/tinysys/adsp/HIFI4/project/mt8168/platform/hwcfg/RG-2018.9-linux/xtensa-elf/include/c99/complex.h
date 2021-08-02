/* complex.h standard header */
#ifndef _COMPLEX
#define _COMPLEX
#include <c99/yvals.h>
#include <xtgmath.h>
_C_STD_BEGIN

 #if _HAS_CPP0X
#include <xtgmath.h>
 #endif /* _HAS_CPP0X */
 #ifndef _HAS_DINKUM_COMPLEX
  #define _HAS_DINKUM_COMPLEX	__GNUC__ < 4 \
	&& _HAS_NAMESPACE && _ALT_NS != 2	/* safe to call _CSTD fns from _STD */
 #endif /* _HAS_DINKUM_COMPLEX */

		/* MACROS */
#define _Fcomplex	_Fcomplex	/* signal definitions */
#define _DCOMPLEX_(re, im)	_Cbuild(re, im)
#define _FCOMPLEX_(re, im)	_FCbuild(re, im)
#define _LCOMPLEX_(re, im)	_LCbuild(re, im)

 #if defined(__cplusplus) && __GNUC__ < 3	/* C++ or old gcc */
  #define _Complex_I	_FCbuild(0.0F, 1.0F)

		// TYPES
  #ifndef _C_COMPLEX_T
   #define _C_COMPLEX_T

typedef struct _C_double_complex
	{	/* double complex */
	double _Val[2];
	} _C_double_complex;

typedef struct _C_float_complex
	{	/* float complex */
	float _Val[2];
	} _C_float_complex;

typedef struct _C_ldouble_complex
	{	/* long double complex */
	long double _Val[2];
	} _C_ldouble_complex;
  #endif /* _C_COMPLEX_T */

_C_LIB_DECL
typedef _C_double_complex _Dcomplex;
typedef _C_float_complex _Fcomplex;
typedef _C_ldouble_complex _Lcomplex;

inline double cimag(_Dcomplex _Left)
	{	// get imaginary part
	return (_Left._Val[1]);
	}

inline double creal(_Dcomplex _Left)
	{	// get real part
	return (_Left._Val[0]);
	}

inline float cimagf(_Fcomplex _Left)
	{	// get imaginary part
	return (_Left._Val[1]);
	}

inline float crealf(_Fcomplex _Left)
	{	// get real part
	return (_Left._Val[0]);
	}

inline long double cimagl(_Lcomplex _Left)
	{	// get imaginary part
	return (_Left._Val[1]);
	}

inline long double creall(_Lcomplex _Left)
	{	// get real part
	return (_Left._Val[0]);
	}
_END_C_LIB_DECL

 #else /* defined(__cplusplus) etc. */
		/* TYPES */

 #if 199901L <= __STDC_VERSION__ || defined(__XTENSA__)

  #if defined(__GNUC__) && __GNUC__ < 3
   #define _Complex	__complex__
  #endif /* defined(__GNUC__) && __GNUC__ < 3 */

typedef double _Complex _Dcomplex;
typedef float _Complex _Fcomplex;
typedef long double _Complex _Lcomplex;

  #ifndef __cplusplus
   #define complex	_Complex
  #endif /* __cplusplus */

  #if __EDG__ || 0x570 <= __SUNPRO_CC
   #define _Complex_I	((float _Complex)__I__)

  #else /* __EDG__ || 0x570 <= __SUNPRO_CC */
   #if defined (TENSILICA) || 1
    #if defined(__GNUC__)
     #define _Complex_I	1.0fi
    #else
     #define _Complex_I	_FCbuild(0.0F, 1.0F)
    #endif
   #else
    #define _Complex_I	_FCbuild(0.0F, 1.0F)
   #endif
  #endif /* __EDG__ || 0x570 <= __SUNPRO_CC */

  #if __EDG__ || 0x570 <= __SUNPRO_CC
  #define _Cbuild(re, im)	(*(_Dcomplex *)(double []){re, im})
/* #define _Cmulcc(x, y)	((x) * (y)) */
/* #define _Cmulcr(x, y)	((x) * (y)) */

  #define _FCbuild(re, im)	(*(_Fcomplex *)(float []){re, im})
/* #define _FCmulcc(x, y)	((x) * (y)) */
/* #define _FCmulcr(x, y)	((x) * (y)) */

  #define _LCbuild(re, im)	(*(_Lcomplex *)(long double []){re, im})
/* #define _LCmulcc(x, y)	((x) * (y)) */
/* #define _LCmulcr(x, y)	((x) * (y)) */

  #elif 0 < __GNUC__
/* #define _Cbuild(re, im)	(*(_Dcomplex *)(double []){re, im}) */
  #define _Cmulcc(x, y)	((x) * (y))
  #define _Cmulcr(x, y)	((x) * (y))

/* #define _FCbuild(re, im)	(*(_Fcomplex *)(float []){re, im}) */
  #define _FCmulcc(x, y)	((x) * (y))
  #define _FCmulcr(x, y)	((x) * (y))

/* #define _LCbuild(re, im)	(*(_Lcomplex *)(long double []){re, im}) */
  #define _LCmulcc(x, y)	((x) * (y))
  #define _LCmulcr(x, y)	((x) * (y))
  #endif /* compiler type */

 #else /* 199901L <= __STDC_VERSION__ */
 #ifndef _C_COMPLEX_T
  #define _C_COMPLEX_T

typedef struct _C_double_complex
	{	/* double complex */
	double _Val[2];
	} _C_double_complex;

typedef struct _C_float_complex
	{	/* float complex */
	float _Val[2];
	} _C_float_complex;

typedef struct _C_ldouble_complex
	{	/* long double complex */
	long double _Val[2];
	} _C_ldouble_complex;
 #endif /* _C_COMPLEX_T */

typedef _C_double_complex _Dcomplex;
typedef _C_float_complex _Fcomplex;
typedef _C_ldouble_complex _Lcomplex;

  #define _Complex_I	_FCbuild(0.0F, 1.0F)
 #endif /* 199901L <= __STDC_VERSION__ */

_C_LIB_DECL
_CRTIMP2 double cimag(_Dcomplex);
_CRTIMP2 double creal(_Dcomplex);
_CRTIMP2 float cimagf(_Fcomplex);
_CRTIMP2 float crealf(_Fcomplex);
_CRTIMP2 long double cimagl(_Lcomplex);
_CRTIMP2 long double creall(_Lcomplex);
_END_C_LIB_DECL
 #endif /* defined(__cplusplus) etc. */

		/* MACROS */

 #if _HAS_C9X_IMAGINARY_TYPE
 #define imaginary	_Imaginary
 #define _Imaginary_I	((float _Imaginary)_Complex_I)
 #define I	_Imaginary_I

 #else /* _HAS_C9X_IMAGINARY_TYPE */
 #define I	_Complex_I
 #endif /* _HAS_C9X_IMAGINARY_TYPE */

		/* FUNCTIONS */
_C_LIB_DECL
_CRTIMP2 double cabs(_Dcomplex);
_CRTIMP2 _Dcomplex cacos(_Dcomplex);
_CRTIMP2 _Dcomplex cacosh(_Dcomplex);
_CRTIMP2 double carg(_Dcomplex);
_CRTIMP2 _Dcomplex casin(_Dcomplex);
_CRTIMP2 _Dcomplex casinh(_Dcomplex);
_CRTIMP2 _Dcomplex catan(_Dcomplex);
_CRTIMP2 _Dcomplex catanh(_Dcomplex);
_CRTIMP2 _Dcomplex ccos(_Dcomplex);
_CRTIMP2 _Dcomplex ccosh(_Dcomplex);
_CRTIMP2 _Dcomplex cexp(_Dcomplex);
/* _CRTIMP2 double cimag(_Dcomplex); */
/* _CRTIMP2 _Dcomplex clog(_Dcomplex); */
_CRTIMP2 _Dcomplex clog10(_Dcomplex);
_CRTIMP2 _Dcomplex conj(_Dcomplex);
_CRTIMP2 _Dcomplex cpow(_Dcomplex, _Dcomplex);
_CRTIMP2 _Dcomplex cproj(_Dcomplex);
/* _CRTIMP2 double creal(_Dcomplex); */
_CRTIMP2 _Dcomplex csin(_Dcomplex);
_CRTIMP2 _Dcomplex csinh(_Dcomplex);
_CRTIMP2 _Dcomplex csqrt(_Dcomplex);
_CRTIMP2 _Dcomplex ctan(_Dcomplex);
_CRTIMP2 _Dcomplex ctanh(_Dcomplex);
_CRTIMP2 double norm(_Dcomplex);	/* added with TR1 */

_CRTIMP2 float cabsf(_Fcomplex);
_CRTIMP2 _Fcomplex cacosf(_Fcomplex);
_CRTIMP2 _Fcomplex cacoshf(_Fcomplex);
_CRTIMP2 float cargf(_Fcomplex);
_CRTIMP2 _Fcomplex casinf(_Fcomplex);
_CRTIMP2 _Fcomplex casinhf(_Fcomplex);
_CRTIMP2 _Fcomplex catanf(_Fcomplex);
_CRTIMP2 _Fcomplex catanhf(_Fcomplex);
_CRTIMP2 _Fcomplex ccosf(_Fcomplex);
_CRTIMP2 _Fcomplex ccoshf(_Fcomplex);
_CRTIMP2 _Fcomplex cexpf(_Fcomplex);
/* _CRTIMP2 float cimagf(_Fcomplex); */
_CRTIMP2 _Fcomplex clogf(_Fcomplex);
_CRTIMP2 _Fcomplex clog10f(_Fcomplex);
_CRTIMP2 _Fcomplex conjf(_Fcomplex);
_CRTIMP2 _Fcomplex cpowf(_Fcomplex, _Fcomplex);
_CRTIMP2 _Fcomplex cprojf(_Fcomplex);
/* _CRTIMP2 float crealf(_Fcomplex); */
_CRTIMP2 _Fcomplex csinf(_Fcomplex);
_CRTIMP2 _Fcomplex csinhf(_Fcomplex);
_CRTIMP2 _Fcomplex csqrtf(_Fcomplex);
_CRTIMP2 _Fcomplex ctanf(_Fcomplex);
_CRTIMP2 _Fcomplex ctanhf(_Fcomplex);
_CRTIMP2 float normf(_Fcomplex);	/* added with TR1 */

_CRTIMP2 long double cabsl(_Lcomplex);
_CRTIMP2 _Lcomplex cacosl(_Lcomplex);
_CRTIMP2 _Lcomplex cacoshl(_Lcomplex);
_CRTIMP2 long double cargl(_Lcomplex);
_CRTIMP2 _Lcomplex casinl(_Lcomplex);
_CRTIMP2 _Lcomplex casinhl(_Lcomplex);
_CRTIMP2 _Lcomplex catanl(_Lcomplex);
_CRTIMP2 _Lcomplex catanhl(_Lcomplex);
_CRTIMP2 _Lcomplex ccosl(_Lcomplex);
_CRTIMP2 _Lcomplex ccoshl(_Lcomplex);
_CRTIMP2 _Lcomplex cexpl(_Lcomplex);
/* _CRTIMP2 long double cimagl(_Lcomplex); */
_CRTIMP2 _Lcomplex clogl(_Lcomplex);
_CRTIMP2 _Lcomplex clog10l(_Lcomplex);
_CRTIMP2 _Lcomplex conjl(_Lcomplex);
_CRTIMP2 _Lcomplex cpowl(_Lcomplex, _Lcomplex);
_CRTIMP2 _Lcomplex cprojl(_Lcomplex);
/* _CRTIMP2 long double creall(_Lcomplex); */
_CRTIMP2 _Lcomplex csinl(_Lcomplex);
_CRTIMP2 _Lcomplex csinhl(_Lcomplex);
_CRTIMP2 _Lcomplex csqrtl(_Lcomplex);
_CRTIMP2 _Lcomplex ctanl(_Lcomplex);
_CRTIMP2 _Lcomplex ctanhl(_Lcomplex);
_CRTIMP2 long double norml(_Lcomplex);	/* added with TR1 */

_CRTIMP2 _Dcomplex (_Cbuild)(double, double);
_CRTIMP2 _Dcomplex (_Cmulcc)(_Dcomplex, _Dcomplex);
_CRTIMP2 _Dcomplex (_Cmulcr)(_Dcomplex, double);
_CRTIMP2 _Dcomplex (_Cdivcc)(_Dcomplex, _Dcomplex);
_CRTIMP2 _Dcomplex (_Cdivcr)(_Dcomplex, double);
_CRTIMP2 _Dcomplex (_Caddcc)(_Dcomplex, _Dcomplex);
_CRTIMP2 _Dcomplex (_Caddcr)(_Dcomplex, double);
_CRTIMP2 _Dcomplex (_Csubcc)(_Dcomplex, _Dcomplex);
_CRTIMP2 _Dcomplex (_Csubcr)(_Dcomplex, double);

_CRTIMP2 _Fcomplex (_FCbuild)(float, float);
_CRTIMP2 _Fcomplex (_FCmulcc)(_Fcomplex, _Fcomplex);
_CRTIMP2 _Fcomplex (_FCmulcr)(_Fcomplex, float);
_CRTIMP2 _Fcomplex (_FCdivcc)(_Fcomplex, _Fcomplex);
_CRTIMP2 _Fcomplex (_FCdivcr)(_Fcomplex, float);
_CRTIMP2 _Fcomplex (_FCaddcc)(_Fcomplex, _Fcomplex);
_CRTIMP2 _Fcomplex (_FCaddcr)(_Fcomplex, float);
_CRTIMP2 _Fcomplex (_FCsubcc)(_Fcomplex, _Fcomplex);
_CRTIMP2 _Fcomplex (_FCsubcr)(_Fcomplex, float);

_CRTIMP2 _Lcomplex (_LCbuild)(long double, long double);
_CRTIMP2 _Lcomplex (_LCmulcc)(_Lcomplex, _Lcomplex);
_CRTIMP2 _Lcomplex (_LCmulcr)(_Lcomplex, long double);
_CRTIMP2 _Lcomplex (_LCdivcc)(_Lcomplex, _Lcomplex);
_CRTIMP2 _Lcomplex (_LCdivcr)(_Lcomplex, long double);
_CRTIMP2 _Lcomplex (_LCaddcc)(_Lcomplex, _Lcomplex);
_CRTIMP2 _Lcomplex (_LCaddcr)(_Lcomplex, long double);
_CRTIMP2 _Lcomplex (_LCsubcc)(_Lcomplex, _Lcomplex);
_CRTIMP2 _Lcomplex (_LCsubcr)(_Lcomplex, long double);
_END_C_LIB_DECL

#ifdef __cplusplus
	// double complex OVERLOADS
inline _Dcomplex acos(_Dcomplex _Left)
	{	// compute cacos
	return (cacos(_Left));
	}

inline _Dcomplex acosh(_Dcomplex _Left)
	{	// compute cacosh
	return (cacosh(_Left));
	}

inline _Dcomplex asin(_Dcomplex _Left)
	{	// compute casin
	return (casin(_Left));
	}

inline _Dcomplex asinh(_Dcomplex _Left)
	{	// compute casinh
	return (casinh(_Left));
	}

inline _Dcomplex atan(_Dcomplex _Left)
	{	// compute catan
	return (catan(_Left));
	}

inline _Dcomplex atanh(_Dcomplex _Left)
	{	// compute catanh
	return (catanh(_Left));
	}

inline _Dcomplex cos(_Dcomplex _Left)
	{	// compute ccos
	return (ccos(_Left));
	}

inline _Dcomplex cosh(_Dcomplex _Left)
	{	// compute ccosh
	return (ccosh(_Left));
	}

inline _Dcomplex exp(_Dcomplex _Left)
	{	// compute cexp
	return (cexp(_Left));
	}

/* inline _Dcomplex log(_Dcomplex _Left)
 *	{	// compute clog
 *	return (clog(_Left)); }
 */

inline _Dcomplex log10(_Dcomplex _Left)
	{	// compute clog10
	return (clog10(_Left));
	}

inline _Dcomplex pow(_Dcomplex _Left, _Dcomplex _Right)
	{	// compute cpow
	return (cpow(_Left, _Right));
	}

inline _Dcomplex sin(_Dcomplex _Left)
	{	// compute csin
	return (csin(_Left));
	}

inline _Dcomplex sinh(_Dcomplex _Left)
	{	// compute csinh
	return (csinh(_Left));
	}

inline _Dcomplex sqrt(_Dcomplex _Left)
	{	// compute csqrt
	return (csqrt(_Left));
	}

inline _Dcomplex tan(_Dcomplex _Left)
	{	// compute ctan
	return (ctan(_Left));
	}

inline _Dcomplex tanh(_Dcomplex _Left)
	{	// compute ctanh
	return (ctanh(_Left));
	}

inline double abs(_Dcomplex _Left)
	{	// compute cabs
	return (cabs(_Left));
	}

inline double arg(_Dcomplex _Left)
	{	// compute carg
	return (carg(_Left));
	}

inline double fabs(_Dcomplex _Left)
	{	// compute cabs
	return (cabs(_Left));
	}

inline double imag(_Dcomplex _Left)
	{	// compute cimag
	return (cimag(_Left));
	}

inline double real(_Dcomplex _Left)
	{	// compute creal
	return (creal(_Left));
	}

	// float complex OVERLOADS
inline _Fcomplex acos(_Fcomplex _Left)
	{	// compute cacos
	return (cacosf(_Left));
	}

inline _Fcomplex acosh(_Fcomplex _Left)
	{	// compute cacosh
	return (cacoshf(_Left));
	}

inline _Fcomplex asin(_Fcomplex _Left)
	{	// compute casin
	return (casinf(_Left));
	}

inline _Fcomplex asinh(_Fcomplex _Left)
	{	// compute casinh
	return (casinhf(_Left));
	}

inline _Fcomplex atan(_Fcomplex _Left)
	{	// compute catan
	return (catanf(_Left));
	}

inline _Fcomplex atanh(_Fcomplex _Left)
	{	// compute catanh
	return (catanhf(_Left));
	}

inline _Fcomplex conj(_Fcomplex _Left)
	{	// compute conj
	return (conjf(_Left));
	}

inline _Fcomplex cos(_Fcomplex _Left)
	{	// compute ccos
	return (ccosf(_Left));
	}

inline _Fcomplex cosh(_Fcomplex _Left)
	{	// compute ccosh
	return (ccoshf(_Left));
	}

inline _Fcomplex cproj(_Fcomplex _Left)
	{	// compute cproj
	return (cprojf(_Left));
	}

inline _Fcomplex exp(_Fcomplex _Left)
	{	// compute cexp
	return (cexpf(_Left));
	}

inline _Fcomplex log(_Fcomplex _Left)
	{	// compute clog
	return (clogf(_Left));
	}

inline _Fcomplex log10(_Fcomplex _Left)
	{	// compute clog10
	return (clog10f(_Left));
	}

inline float norm(_Fcomplex _Left)
	{	// compute norm -- added with TR1
	return (normf(_Left));
	}

inline _Fcomplex pow(_Fcomplex _Left, _Fcomplex _Right)
	{	// compute cpow
	return (cpowf(_Left, _Right));
	}

inline _Fcomplex sin(_Fcomplex _Left)
	{	// compute csin
	return (csinf(_Left));
	}

inline _Fcomplex sinh(_Fcomplex _Left)
	{	// compute csinh
	return (csinhf(_Left));
	}

inline _Fcomplex sqrt(_Fcomplex _Left)
	{	// compute csqrt
	return (csqrtf(_Left));
	}

inline _Fcomplex tan(_Fcomplex _Left)
	{	// compute ctan
	return (ctanf(_Left));
	}

inline _Fcomplex tanh(_Fcomplex _Left)
	{	// compute ctanh
	return (ctanhf(_Left));
	}

inline float abs(_Fcomplex _Left)
	{	// compute cabs
	return (cabsf(_Left));
	}

inline float arg(_Fcomplex _Left)
	{	// compute carg
	return (cargf(_Left));
	}

inline float carg(_Fcomplex _Left)
	{	// compute carg
	return (cargf(_Left));
	}

inline float cimag(_Fcomplex _Left)
	{	// compute cimag
	return (cimagf(_Left));
	}

inline float creal(_Fcomplex _Left)
	{	// compute creal
	return (crealf(_Left));
	}

inline float fabs(_Fcomplex _Left)
	{	// compute cabs
	return (cabsf(_Left));
	}

inline float imag(_Fcomplex _Left)
	{	// compute cimag
	return (cimagf(_Left));
	}

inline float real(_Fcomplex _Left)
	{	// compute creal
	return (crealf(_Left));
	}

	// long double complex OVERLOADS
inline _Lcomplex acos(_Lcomplex _Left)
	{	// compute cacos
	return (cacosl(_Left));
	}

inline _Lcomplex acosh(_Lcomplex _Left)
	{	// compute cacosh
	return (cacoshl(_Left));
	}

inline _Lcomplex asin(_Lcomplex _Left)
	{	// compute casin
	return (casinl(_Left));
	}

inline _Lcomplex asinh(_Lcomplex _Left)
	{	// compute casinh
	return (casinhl(_Left));
	}

inline _Lcomplex atan(_Lcomplex _Left)
	{	// compute catan
	return (catanl(_Left));
	}

inline _Lcomplex atanh(_Lcomplex _Left)
	{	// compute catanh
	return (catanhl(_Left));
	}

inline _Lcomplex conj(_Lcomplex _Left)
	{	// compute conj
	return (conjl(_Left));
	}

inline _Lcomplex cos(_Lcomplex _Left)
	{	// compute ccos
	return (ccosl(_Left));
	}

inline _Lcomplex cosh(_Lcomplex _Left)
	{	// compute ccosh
	return (ccoshl(_Left));
	}

inline _Lcomplex cproj(_Lcomplex _Left)
	{	// compute cproj
	return (cprojl(_Left));
	}

inline _Lcomplex exp(_Lcomplex _Left)
	{	// compute cexp
	return (cexpl(_Left));
	}

inline _Lcomplex log(_Lcomplex _Left)
	{	// compute clog
	return (clogl(_Left));
	}

inline _Lcomplex log10(_Lcomplex _Left)
	{	// compute clog10
	return (clog10l(_Left));
	}

inline long double norm(_Lcomplex _Left)
	{	// compute norm -- added with TR1
	return (norml(_Left));
	}

inline _Lcomplex pow(_Lcomplex _Left, _Lcomplex _Right)
	{	// compute cpow
	return (cpowl(_Left, _Right));
	}

inline _Lcomplex sin(_Lcomplex _Left)
	{	// compute csin
	return (csinl(_Left));
	}

inline _Lcomplex sinh(_Lcomplex _Left)
	{	// compute csinh
	return (csinhl(_Left));
	}

inline _Lcomplex sqrt(_Lcomplex _Left)
	{	// compute csqrt
	return (csqrtl(_Left));
	}

inline _Lcomplex tan(_Lcomplex _Left)
	{	// compute ctan
	return (ctanl(_Left));
	}

inline _Lcomplex tanh(_Lcomplex _Left)
	{	// compute ctanh
	return (ctanhl(_Left));
	}

inline long double abs(_Lcomplex _Left)
	{	// compute cabs
	return (cabsl(_Left));
	}

inline long double arg(_Lcomplex _Left)
	{	// compute carg
	return (cargl(_Left));
	}

inline long double carg(_Lcomplex _Left)
	{	// compute carg
	return (cargl(_Left));
	}

inline long double cimag(_Lcomplex _Left)
	{	// compute cimag
	return (cimagl(_Left));
	}

inline long double creal(_Lcomplex _Left)
	{	// compute creal
	return (creall(_Left));
	}

inline long double fabs(_Lcomplex _Left)
	{	// compute cabs
	return (cabsl(_Left));
	}

inline long double imag(_Lcomplex _Left)
	{	// compute cimag
	return (cimagl(_Left));
	}

inline long double real(_Lcomplex _Left)
	{	// compute creal
	return (creall(_Left));
	}
#endif /* __cplusplus */

_C_STD_END

		// GENERIC TEMPLATES

 #if _HAS_GENERIC_TEMPLATES
_C_STD_BEGIN
		// TEMPLATE CLASS _Rc_type (ADDITIONS TO <xtgmath.h>)
template<> struct _Rc_type<_Fcomplex>
	{	// determine if type is real or complex
	typedef char _Type;
	};

template<> struct _Rc_type<_Dcomplex>
	{	// determine if type is real or complex
	typedef char _Type;
	};

template<> struct _Rc_type<_Lcomplex>
	{	// determine if type is real or complex
	typedef char _Type;
	};

		// TEMPLATE CLASS _Real_type (ADDITIONS TO <xtgmath.h>)
template<> struct _Real_type<_Fcomplex>
	{	// determine equivalent real type
	typedef float _Type;
	};

template<> struct _Real_type<_Dcomplex>
	{	// determine equivalent real type
	typedef double _Type;
	};

template<> struct _Real_type<_Lcomplex>
	{	// determine equivalent real type
	typedef long double _Type;
	};

		// TEMPLATE CLASS _Combined_type (ADDITIONS TO <xtgmath.h>)
template<> struct _Combined_type<char, float>
	{	// determine combined type
	typedef _Fcomplex _Type;
	};

template<> struct _Combined_type<char, double>
	{	// determine combined type
	typedef _Dcomplex _Type;
	};

template<> struct _Combined_type<char, long double>
	{	// determine combined type
	typedef _Lcomplex _Type;
	};

	_TGEN_C0(carg)	// generic overloads
	_TGEN_C0(cimag)
	_TGEN_C(conj)
	_TGEN_C(cproj)
	_TGEN_C0(creal)

	_TGEN_C0(arg)	// added with TR1
//	_TGEN_C(conj)
	_TGEN_C0(imag)
	_TGEN_C0(norm)
	_TGEN_C0(real)
_C_STD_END

 #include <math.h>	/* define all overloads for complex functions */
 #endif /* _HAS_GENERIC_TEMPLATES */


	/* SPECIAL HANDLING FOR clog */

 #if _ALT_NS == 2
namespace _Dinkum_clog {
_C_LIB_DECL
_CRTIMP2 _CSTD _Dcomplex clog(_CSTD _Dcomplex);
_END_C_LIB_DECL
}	// namespace _Dinkum_clog

_C_STD_BEGIN
inline _Dcomplex log(_Dcomplex _Left)
	{	// compute clog
	return (_Dinkum_clog::clog(_Left));
	}
_C_STD_END

 #else /* _ALT_NS == 2 */
_C_STD_BEGIN
_C_LIB_DECL
_CRTIMP2 _Dcomplex clog(_Dcomplex);
_END_C_LIB_DECL

  #ifdef __cplusplus
inline _Dcomplex log(_Dcomplex _Left)
	{	// compute clog
	return (clog(_Left));
	}
  #endif /* __cplusplus */

_C_STD_END
 #endif /* _ALT_NS == 2 */

#endif /* _COMPLEX */

#ifdef _STD_USING
using _CSTD _Dcomplex; using _CSTD _Fcomplex; using _CSTD _Lcomplex;

 #if _ALT_NS == 2
using _Dinkum_clog::clog;	/* SPECIAL HANDLING FOR clog */

 #else /* _ALT_NS == 2 */
using _CSTD clog;
 #endif /* _ALT_NS == 2 */

using _CSTD _Cbuild; using _CSTD _Cmulcc; using _CSTD _Cmulcr;
using _CSTD _Cdivcc; using _CSTD _Cdivcr; using _CSTD _Caddcc;
using _CSTD _Caddcr; using _CSTD _Csubcc; using _CSTD _Csubcr;

using _CSTD _FCbuild; using _CSTD _FCmulcc; using _CSTD _FCmulcr;
using _CSTD _FCdivcc; using _CSTD _FCdivcr; using _CSTD _FCaddcc;
using _CSTD _FCaddcr; using _CSTD _FCsubcc; using _CSTD _FCsubcr;

using _CSTD _LCbuild; using _CSTD _LCmulcc; using _CSTD _LCmulcr;
using _CSTD _LCdivcc; using _CSTD _LCdivcr; using _CSTD _LCaddcc;
using _CSTD _LCaddcr; using _CSTD _LCsubcc; using _CSTD _LCsubcr;

using _CSTD cabs; using _CSTD cacos; using _CSTD cacosh;
using _CSTD carg; using _CSTD casin; using _CSTD casinh;
using _CSTD catan; using _CSTD catanh; using _CSTD ccos;
using _CSTD ccosh; using _CSTD cexp; using _CSTD cimag;
/* using _CSTD clog; */ using _CSTD conj; using _CSTD cpow;
using _CSTD cproj; using _CSTD creal; using _CSTD csin;
using _CSTD csinh; using _CSTD csqrt; using _CSTD ctan;
using _CSTD ctanh;

using _CSTD cabsf; using _CSTD cacosf; using _CSTD cacoshf;
using _CSTD cargf; using _CSTD casinf; using _CSTD casinhf;
using _CSTD catanf; using _CSTD catanhf; using _CSTD ccosf;
using _CSTD ccoshf; using _CSTD cexpf; using _CSTD cimagf;
using _CSTD clogf; using _CSTD conjf; using _CSTD cpowf;
using _CSTD cprojf; using _CSTD crealf; using _CSTD csinf;
using _CSTD csinhf; using _CSTD csqrtf; using _CSTD ctanf;
using _CSTD ctanhf;

using _CSTD cabsl; using _CSTD cacosl; using _CSTD cacoshl;
using _CSTD cargl; using _CSTD casinl; using _CSTD casinhl;
using _CSTD catanl; using _CSTD catanhl; using _CSTD ccosl;
using _CSTD ccoshl; using _CSTD cexpl; using _CSTD cimagl;
using _CSTD clogl; using _CSTD conjl; using _CSTD cpowl;
using _CSTD cprojl; using _CSTD creall; using _CSTD csinl;
using _CSTD csinhl; using _CSTD csqrtl; using _CSTD ctanl;
using _CSTD ctanhl;

using _CSTD abs; using _CSTD acos; using _CSTD acosh;
using _CSTD arg; using _CSTD imag; using _CSTD real;
using _CSTD asin; using _CSTD asinh; using _CSTD atan;
using _CSTD atanh; using _CSTD cos; using _CSTD cosh;
using _CSTD exp; using _CSTD log; using _CSTD pow;
using _CSTD sin; using _CSTD sinh; using _CSTD sqrt;
using _CSTD tan; using _CSTD tanh; using _CSTD fabs;

using _CSTD log10; using _CSTD norm;	/* added with TR1 */
#endif /* _STD_USING */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.40:1611 */
