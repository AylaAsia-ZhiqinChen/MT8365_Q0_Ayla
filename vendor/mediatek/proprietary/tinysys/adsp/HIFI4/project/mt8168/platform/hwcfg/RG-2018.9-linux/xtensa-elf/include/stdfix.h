/* stdfix.h TR header -- fixed-point visible library from C TR18037 */
#ifndef _STDFIX
#define _STDFIX
#ifndef _YVALS
 #include <yvals.h>
#endif /* _YVALS */

_C_STD_BEGIN
_EXTERN_C

 #if _HAS_FIXED_POINT
 #ifndef _HAS_FIXED_POINT_LANGUAGE_SUPPORT

  #if !defined(__cplusplus) && 305 <= __EDG_VERSION__
   #define _HAS_FIXED_POINT_LANGUAGE_SUPPORT	1

  #else /* !defined(__cplusplus) etc. */
   #define _HAS_FIXED_POINT_LANGUAGE_SUPPORT	0
  #endif /* !defined(__cplusplus) etc. */

 #endif /* _HAS_FIXED_POINT_LANGUAGE_SUPPORT */
 #endif /* _HAS_FIXED_POINT */

	/* keyword macros */
#define accum	_Accum
#define fract	_Fract
#define sat		_Sat

	/* types for bits return values */
typedef signed char int_hr_t;
typedef signed short int_r_t;
typedef signed long int_lr_t;

typedef unsigned char uint_uhr_t;
typedef unsigned short uint_ur_t;
typedef unsigned long uint_ulr_t;

typedef signed short int_hk_t;
typedef signed long int_k_t;
typedef _Longlong int_lk_t;

typedef unsigned short uint_uhk_t;
typedef unsigned long uint_uk_t;
typedef _ULonglong uint_ulk_t;

	/* fractional bits macros */
#define SFRACT_FBIT		7
#define FRACT_FBIT		15
#define LFRACT_FBIT		31

#define USFRACT_FBIT	8
#define UFRACT_FBIT		16
#define ULFRACT_FBIT	32

#define SACCUM_FBIT		7
#define ACCUM_FBIT		15
#define LACCUM_FBIT		31

#define USACCUM_FBIT	8
#define UACCUM_FBIT		16
#define ULACCUM_FBIT	32

	/* integer bits macros */
#define SACCUM_IBIT		8
#define ACCUM_IBIT		16
#define LACCUM_IBIT		32

#define USACCUM_IBIT	8
#define UACCUM_IBIT		16
#define ULACCUM_IBIT	32

 #if _HAS_FIXED_POINT_LANGUAGE_SUPPORT
 #define _FIXED_VAL(x)	x
typedef signed short _Fract _Fixed_hr;
typedef signed _Fract _Fixed_r;
typedef signed long _Fract _Fixed_lr;

typedef unsigned short _Fract _Fixed_uhr;
typedef unsigned _Fract _Fixed_ur;
typedef unsigned long _Fract _Fixed_ulr;

typedef signed short _Accum _Fixed_hk;
typedef signed _Accum _Fixed_k;
typedef signed long _Accum _Fixed_lk;

typedef unsigned short _Accum _Fixed_uhk;
typedef unsigned _Accum _Fixed_uk;
typedef unsigned long _Accum _Fixed_ulk;

	/* macros */
 #define _Half(suffix)	0.5##suffix
 #define _FRACT_MIN(suffix, prefix)	(-_Half(suffix) - _Half(suffix))
 #define _ACCUM_MIN(suffix, prefix)	(-prefix##_MAX - prefix##_EPSILON)

 #define _FRACT_MAX(suffix, prefix)	(_Half(suffix) \
	- prefix##_EPSILON + _Half(suffix))
 #define _ACCUM_MAX(suffix, prefix)	((((_Half(suffix) << prefix##_IBIT) \
	- prefix##_EPSILON) << 1) + prefix##_EPSILON)

 #define _FRACT_EPSILON(suffix, prefix)	(_Half(suffix) >> prefix##_FBIT - 1)
 #define _ACCUM_EPSILON(suffix, prefix)	(_Half(suffix) >> prefix##_FBIT - 1)

 #else /* _HAS_FIXED_POINT_LANGUAGE_SUPPORT */
 #define _FIXED_VAL(x)	(x)._Val

typedef struct {uint_uhr_t _Val; } _Fixed_hr;
typedef struct {uint_ur_t _Val; } _Fixed_r;
typedef struct {uint_ulr_t _Val; } _Fixed_lr;

typedef struct {uint_uhr_t _Val; } _Fixed_uhr;
typedef struct {uint_ur_t _Val; } _Fixed_ur;
typedef struct {uint_ulr_t _Val; } _Fixed_ulr;

typedef struct {uint_uhk_t _Val; } _Fixed_hk;
typedef struct {uint_uk_t _Val; } _Fixed_k;
typedef struct {uint_ulk_t _Val; } _Fixed_lk;

typedef struct {uint_uhk_t _Val; } _Fixed_uhk;
typedef struct {uint_uk_t _Val; } _Fixed_uk;
typedef struct {uint_ulk_t _Val; } _Fixed_ulk;

extern _Fixed_hr _Fixed_min_hr, _Fixed_max_hr, _Fixed_eps_hr,
	_Fixed_nan_hr, _Fixed_rteps_hr;
extern _Fixed_r _Fixed_min_r, _Fixed_max_r, _Fixed_eps_r,
	_Fixed_nan_r, _Fixed_rteps_r;
extern _Fixed_lr _Fixed_min_lr, _Fixed_max_lr, _Fixed_eps_lr,
	_Fixed_nan_lr, _Fixed_rteps_lr;

extern _Fixed_uhr _Fixed_min_uhr, _Fixed_max_uhr, _Fixed_eps_uhr,
	_Fixed_nan_uhr, _Fixed_rteps_uhr;
extern _Fixed_ur _Fixed_min_ur, _Fixed_max_ur, _Fixed_eps_ur,
	_Fixed_nan_ur, _Fixed_rteps_ur;
extern _Fixed_ulr _Fixed_min_ulr, _Fixed_max_ulr, _Fixed_eps_ulr,
	_Fixed_nan_ulr, _Fixed_rteps_ulr;

extern _Fixed_hk _Fixed_min_hk, _Fixed_max_hk, _Fixed_eps_hk,
	_Fixed_nan_hk, _Fixed_rteps_hk;
extern _Fixed_k _Fixed_min_k,_Fixed_max_k,_Fixed_eps_k,
	_Fixed_nan_k, _Fixed_rteps_k;
extern _Fixed_lk _Fixed_min_lk, _Fixed_max_lk, _Fixed_eps_lk,
	_Fixed_nan_lk, _Fixed_rteps_lk;

extern _Fixed_uhk _Fixed_min_uhk, _Fixed_max_uhk, _Fixed_eps_uhk,
	_Fixed_nan_uhk, _Fixed_rteps_uhk;
extern _Fixed_uk _Fixed_min_uk, _Fixed_max_uk, _Fixed_eps_uk,
	_Fixed_nan_uk, _Fixed_rteps_uk;
extern _Fixed_ulk _Fixed_min_ulk, _Fixed_max_ulk, _Fixed_eps_ulk,
	_Fixed_nan_ulk, _Fixed_rteps_ulk;

 #define _FRACT_MIN(suffix, prefix)	_Fixed_min_##suffix
 #define _ACCUM_MIN(suffix, prefix)	_Fixed_min_##suffix

 #define _FRACT_MAX(suffix, prefix)	_Fixed_max_##suffix
 #define _ACCUM_MAX(suffix, prefix)	_Fixed_max_##suffix

 #define _FRACT_EPSILON(suffix, prefix)	_Fixed_eps_##suffix
 #define _ACCUM_EPSILON(suffix, prefix)	_Fixed_eps_##suffix
 #endif /* _HAS_FIXED_POINT_LANGUAGE_SUPPORT */

	/* minimum value macros */
#define SFRACT_MIN		_FRACT_MIN(hr, SFRACT)
#define FRACT_MIN		_FRACT_MIN(r, FRACT)
#define LFRACT_MIN		_FRACT_MIN(lr, LFRACT)

#define SACCUM_MIN		_ACCUM_MIN(hk, SACCUM)
#define ACCUM_MIN		_ACCUM_MIN(k, ACCUM)
#define LACCUM_MIN		_ACCUM_MIN(lk, LACCUM)

	/* maximum value macros */
#define SFRACT_MAX		_FRACT_MAX(hr, SFRACT)
#define FRACT_MAX		_FRACT_MAX(r, FRACT)
#define LFRACT_MAX		_FRACT_MAX(lr, LFRACT)

#define USFRACT_MAX		_FRACT_MAX(uhr, USFRACT)
#define UFRACT_MAX		_FRACT_MAX(ur, UFRACT)
#define ULFRACT_MAX		_FRACT_MAX(ulr, ULFRACT)

#define SACCUM_MAX		_ACCUM_MAX(hk, SACCUM)
#define ACCUM_MAX		_ACCUM_MAX(k, ACCUM)
#define LACCUM_MAX		_ACCUM_MAX(lk, LACCUM)

#define USACCUM_MAX		_ACCUM_MAX(uhk, USACCUM)
#define UACCUM_MAX		_ACCUM_MAX(uk, UACCUM)
#define ULACCUM_MAX		_ACCUM_MAX(ulk, ULACCUM)

	/* epsilon macros */
#define SFRACT_EPSILON	_FRACT_EPSILON(hr, SFRACT)
#define FRACT_EPSILON	_FRACT_EPSILON(r, FRACT)
#define LFRACT_EPSILON	_FRACT_EPSILON(lr, LFRACT)

#define USFRACT_EPSILON	_FRACT_EPSILON(uhr, USFRACT)
#define UFRACT_EPSILON	_FRACT_EPSILON(ur, UFRACT)
#define ULFRACT_EPSILON	_FRACT_EPSILON(ulr, ULFRACT)

#define SACCUM_EPSILON	_ACCUM_EPSILON(hk, SACCUM)
#define ACCUM_EPSILON	_ACCUM_EPSILON(k, ACCUM)
#define LACCUM_EPSILON	_ACCUM_EPSILON(lk, LACCUM)

#define USACCUM_EPSILON	_ACCUM_EPSILON(uhk, USACCUM)
#define UACCUM_EPSILON	_ACCUM_EPSILON(uk, UACCUM)
#define ULACCUM_EPSILON	_ACCUM_EPSILON(ulk, ULACCUM)

	/* functions */
signed int mulir(signed int, _Fixed_r);
signed long mulilr(signed long, _Fixed_lr);
signed int mulik(signed int, _Fixed_k);
signed long mulilk(signed long, _Fixed_lk);

unsigned int muliur(unsigned int, _Fixed_ur);
unsigned long muliulr(unsigned long, _Fixed_ulr);
unsigned int muliuk(unsigned int, _Fixed_uk);
unsigned long muliulk(unsigned long, _Fixed_ulk);

signed int divir(signed int, _Fixed_r);
signed long divilr(signed long, _Fixed_lr);
signed int divik(signed int, _Fixed_k);
signed long divilk(signed long, _Fixed_lk);

unsigned int diviur(unsigned int, _Fixed_ur);
unsigned long diviulr(unsigned long, _Fixed_ulr);
unsigned int diviuk(unsigned int, _Fixed_uk);
unsigned long diviulk(unsigned long, _Fixed_ulk);

signed int idivr(_Fixed_r, _Fixed_r);
signed long idivlr(_Fixed_lr, _Fixed_lr);
signed int idivk(_Fixed_k, _Fixed_k);
signed long idivlk(_Fixed_lk, _Fixed_lk);

unsigned int idivur(_Fixed_ur, _Fixed_ur);
unsigned long idivulr(_Fixed_ulr, _Fixed_ulr);
unsigned int idivuk(_Fixed_uk, _Fixed_uk);
unsigned long idivulk(_Fixed_ulk, _Fixed_ulk);

_Fixed_r rdivi(signed int, signed int);
_Fixed_lr lrdivi(signed long, signed long);
_Fixed_k kdivi(signed int, signed int);
_Fixed_lk lkdivi(signed long, signed long);

_Fixed_ur urdivi(unsigned int, unsigned int);
_Fixed_ulr ulrdivi(unsigned long, unsigned long);
_Fixed_uk ukdivi(unsigned int, unsigned int);
_Fixed_ulk ulkdivi(unsigned long, unsigned long);

_Fixed_hr abshr(_Fixed_hr);	/* generic absfx */
_Fixed_r absr(_Fixed_r);
_Fixed_lr abslr(_Fixed_lr);
_Fixed_hk abshk(_Fixed_hk);
_Fixed_k absk(_Fixed_k);
_Fixed_lk abslk(_Fixed_lk);

_Fixed_hr roundhr(_Fixed_hr, int);	/* generic roundfx */
_Fixed_r roundr(_Fixed_r, int);
_Fixed_lr roundlr(_Fixed_lr, int);
_Fixed_hk roundhk(_Fixed_hk, int);
_Fixed_k roundk(_Fixed_k, int);
_Fixed_lk roundlk(_Fixed_lk, int);

_Fixed_uhr rounduhr(_Fixed_uhr, int);
_Fixed_ur roundur(_Fixed_ur, int);
_Fixed_ulr roundulr(_Fixed_ulr, int);
_Fixed_uhk rounduhk(_Fixed_uhk, int);
_Fixed_uk rounduk(_Fixed_uk, int);
_Fixed_ulk roundulk(_Fixed_ulk, int);

int countlshr(_Fixed_hr);	/* generic countlsfx */
int countlsr(_Fixed_r);
int countlslr(_Fixed_lr);
int countlshk(_Fixed_hk);
int countlsk(_Fixed_k);
int countlslk(_Fixed_lk);

int countlsuhr(_Fixed_uhr);
int countlsur(_Fixed_ur);
int countlsulr(_Fixed_ulr);
int countlsuhk(_Fixed_uhk);
int countlsuk(_Fixed_uk);
int countlsulk(_Fixed_ulk);

int_hr_t bitshr(_Fixed_hr);
int_r_t bitsr(_Fixed_r);
int_lr_t bitslr(_Fixed_lr);
int_hk_t bitshk(_Fixed_hk);
int_k_t bitsk(_Fixed_k);
int_lk_t bitslk(_Fixed_lk);

uint_uhr_t bitsuhr(_Fixed_uhr);
uint_ur_t bitsur(_Fixed_ur);
uint_ulr_t bitsulr(_Fixed_ulr);
uint_uhk_t bitsuhk(_Fixed_uhk);
uint_uk_t bitsuk(_Fixed_uk);
uint_ulk_t bitsulk(_Fixed_ulk);

_Fixed_hr hrbits(int);
_Fixed_r rbits(int);
_Fixed_lr lrbits(int);
_Fixed_hk hkbits(int);
_Fixed_k kbits(int);
_Fixed_lk lkbits(int);

_Fixed_uhr uhrbits(unsigned int);
_Fixed_ur urbits(unsigned int);
_Fixed_ulr ulrbits(unsigned int);
_Fixed_uhk uhkbits(unsigned int);
_Fixed_uk ukbits(unsigned int);
_Fixed_ulk ulkbits(unsigned int);

_Fixed_hr strtofxhr(const char *_Restrict, char **_Restrict);
_Fixed_r strtofxr(const char *_Restrict, char **_Restrict);
_Fixed_lr strtofxlr(const char *_Restrict, char **_Restrict);
_Fixed_hk strtofxhk(const char *_Restrict, char **_Restrict);
_Fixed_k strtofxk(const char *_Restrict, char **_Restrict);
_Fixed_lk strtofxlk(const char *_Restrict, char **_Restrict);

_Fixed_uhr strtofxuhr(const char *_Restrict, char **_Restrict);
_Fixed_ur strtofxur(const char *_Restrict, char **_Restrict);
_Fixed_ulr strtofxulr(const char *_Restrict, char **_Restrict);
_Fixed_uhk strtofxuhk(const char *_Restrict, char **_Restrict);
_Fixed_uk strtofxuk(const char *_Restrict, char **_Restrict);
_Fixed_ulk strtofxulk(const char *_Restrict, char **_Restrict);

_Fixed_hk acos_hk(_Fixed_hk);
_Fixed_hk acosh_hk(_Fixed_hk);
_Fixed_hk asinh_hk(_Fixed_hk);
_Fixed_hk asin_hk(_Fixed_hk);
_Fixed_hk atan_hk(_Fixed_hk);
_Fixed_hk atanh_hk(_Fixed_hk);
_Fixed_hk atan2_hk(_Fixed_hk, _Fixed_hk);
_Fixed_hk ceil_hk(_Fixed_hk);
_Fixed_hk cos_hk(_Fixed_hk);
_Fixed_hk cosh_hk(_Fixed_hk);
_Fixed_hk erf_hk(_Fixed_hk);
_Fixed_hk erfc_hk(_Fixed_hk);
_Fixed_hk exp_hk(_Fixed_hk);
_Fixed_hk expm1_hk(_Fixed_hk);
_Fixed_hk exp2_hk(_Fixed_hk);
_Fixed_hk fdim_hk(_Fixed_hk, _Fixed_hk);
_Fixed_hk floor_hk(_Fixed_hk);
_Fixed_hk fmax_hk(_Fixed_hk, _Fixed_hk);
_Fixed_hk fmin_hk(_Fixed_hk, _Fixed_hk);
_Fixed_hk fmod_hk(_Fixed_hk, _Fixed_hk);
_Fixed_hk frexp_hk(_Fixed_hk, int *);
int ilogb_hk(_Fixed_hk);
_Fixed_hk ldexp_hk(_Fixed_hk, int);
_Fixed_hk lgamma_hk(_Fixed_hk);
_Fixed_hk log_hk(_Fixed_hk);
_Fixed_hk log1p_hk(_Fixed_hk);
_Fixed_hk log10_hk(_Fixed_hk);
_Fixed_hk log2_hk(_Fixed_hk);
_Longlong llrint_hk(_Fixed_hk);
_Longlong llround_hk(_Fixed_hk);
long lrint_hk(_Fixed_hk);
long lround_hk(_Fixed_hk);
_Fixed_hk modf_hk(_Fixed_hk, _Fixed_hk *);
_Fixed_hk nearbyint_hk(_Fixed_hk);
_Fixed_hk nextafter_hk(_Fixed_hk, _Fixed_hk);
_Fixed_hk nexttoward_hk(_Fixed_hk, _Fixed_lk);
_Fixed_hk pow_hk(_Fixed_hk, _Fixed_hk);
_Fixed_hk remainder_hk(_Fixed_hk, _Fixed_hk);
_Fixed_hk remquo_hk(_Fixed_hk, _Fixed_hk, int *);
_Fixed_hk rint_hk(_Fixed_hk);
_Fixed_hk round_hk(_Fixed_hk);
_Fixed_hk scalbln_hk(_Fixed_hk, long);
_Fixed_hk scalbn_hk(_Fixed_hk, int);
_Fixed_hk sin_hk(_Fixed_hk);
_Fixed_hk sinh_hk(_Fixed_hk);
_Fixed_hk sqrt_hk(_Fixed_hk);
_Fixed_hk tan_hk(_Fixed_hk);
_Fixed_hk tanh_hk(_Fixed_hk);
_Fixed_hk tgamma_hk(_Fixed_hk);
_Fixed_hk trunc_hk(_Fixed_hk);

_Fixed_k acos_k(_Fixed_k);
_Fixed_k acosh_k(_Fixed_k);
_Fixed_k asinh_k(_Fixed_k);
_Fixed_k asin_k(_Fixed_k);
_Fixed_k atan_k(_Fixed_k);
_Fixed_k atanh_k(_Fixed_k);
_Fixed_k atan2_k(_Fixed_k, _Fixed_k);
_Fixed_k ceil_k(_Fixed_k);
_Fixed_k cos_k(_Fixed_k);
_Fixed_k cosh_k(_Fixed_k);
_Fixed_k erf_k(_Fixed_k);
_Fixed_k erfc_k(_Fixed_k);
_Fixed_k exp_k(_Fixed_k);
_Fixed_k expm1_k(_Fixed_k);
_Fixed_k exp2_k(_Fixed_k);
_Fixed_k fdim_k(_Fixed_k, _Fixed_k);
_Fixed_k floor_k(_Fixed_k);
_Fixed_k fmax_k(_Fixed_k, _Fixed_k);
_Fixed_k fmin_k(_Fixed_k, _Fixed_k);
_Fixed_k fmod_k(_Fixed_k, _Fixed_k);
_Fixed_k frexp_k(_Fixed_k, int *);
int ilogb_k(_Fixed_k);
_Fixed_k ldexp_k(_Fixed_k, int);
_Fixed_k lgamma_k(_Fixed_k);
_Fixed_k log_k(_Fixed_k);
_Fixed_k log1p_k(_Fixed_k);
_Fixed_k log10_k(_Fixed_k);
_Fixed_k log2_k(_Fixed_k);
_Longlong llrint_k(_Fixed_k);
_Longlong llround_k(_Fixed_k);
long lrint_k(_Fixed_k);
long lround_k(_Fixed_k);
_Fixed_k modf_k(_Fixed_k, _Fixed_k *);
_Fixed_k nearbyint_k(_Fixed_k);
_Fixed_k nextafter_k(_Fixed_k, _Fixed_k);
_Fixed_k nexttoward_k(_Fixed_k, _Fixed_lk);
_Fixed_k pow_k(_Fixed_k, _Fixed_k);
_Fixed_k remainder_k(_Fixed_k, _Fixed_k);
_Fixed_k remquo_k(_Fixed_k, _Fixed_k, int *);
_Fixed_k rint_k(_Fixed_k);
_Fixed_k round_k(_Fixed_k);
_Fixed_k scalbln_k(_Fixed_k, long);
_Fixed_k scalbn_k(_Fixed_k, int);
_Fixed_k sin_k(_Fixed_k);
_Fixed_k sinh_k(_Fixed_k);
_Fixed_k sqrt_k(_Fixed_k);
_Fixed_k tan_k(_Fixed_k);
_Fixed_k tanh_k(_Fixed_k);
_Fixed_k tgamma_k(_Fixed_k);
_Fixed_k trunc_k(_Fixed_k);

_Fixed_lk acos_lk(_Fixed_lk);
_Fixed_lk acosh_lk(_Fixed_lk);
_Fixed_lk asinh_lk(_Fixed_lk);
_Fixed_lk asin_lk(_Fixed_lk);
_Fixed_lk atan_lk(_Fixed_lk);
_Fixed_lk atanh_lk(_Fixed_lk);
_Fixed_lk atan2_lk(_Fixed_lk, _Fixed_lk);
_Fixed_lk ceil_lk(_Fixed_lk);
_Fixed_lk cos_lk(_Fixed_lk);
_Fixed_lk cosh_lk(_Fixed_lk);
_Fixed_lk erf_lk(_Fixed_lk);
_Fixed_lk erfc_lk(_Fixed_lk);
_Fixed_lk exp_lk(_Fixed_lk);
_Fixed_lk expm1_lk(_Fixed_lk);
_Fixed_lk exp2_lk(_Fixed_lk);
_Fixed_lk fdim_lk(_Fixed_lk, _Fixed_lk);
_Fixed_lk floor_lk(_Fixed_lk);
_Fixed_lk fmax_lk(_Fixed_lk, _Fixed_lk);
_Fixed_lk fmin_lk(_Fixed_lk, _Fixed_lk);
_Fixed_lk fmod_lk(_Fixed_lk, _Fixed_lk);
_Fixed_lk frexp_lk(_Fixed_lk, int *);
int ilogb_lk(_Fixed_lk);
_Fixed_lk ldexp_lk(_Fixed_lk, int);
_Fixed_lk lgamma_lk(_Fixed_lk);
_Fixed_lk log_lk(_Fixed_lk);
_Fixed_lk log1p_lk(_Fixed_lk);
_Fixed_lk log10_lk(_Fixed_lk);
_Fixed_lk log2_lk(_Fixed_lk);
_Longlong llrint_lk(_Fixed_lk);
_Longlong llround_lk(_Fixed_lk);
long lrint_lk(_Fixed_lk);
long lround_lk(_Fixed_lk);
_Fixed_lk modf_lk(_Fixed_lk, _Fixed_lk *);
_Fixed_lk nearbyint_lk(_Fixed_lk);
_Fixed_lk nextafter_lk(_Fixed_lk, _Fixed_lk);
_Fixed_lk nexttoward_lk(_Fixed_lk, _Fixed_lk);
_Fixed_lk pow_lk(_Fixed_lk, _Fixed_lk);
_Fixed_lk remainder_lk(_Fixed_lk, _Fixed_lk);
_Fixed_lk remquo_lk(_Fixed_lk, _Fixed_lk, int *);
_Fixed_lk rint_lk(_Fixed_lk);
_Fixed_lk round_lk(_Fixed_lk);
_Fixed_lk scalbln_lk(_Fixed_lk, long);
_Fixed_lk scalbn_lk(_Fixed_lk, int);
_Fixed_lk sin_lk(_Fixed_lk);
_Fixed_lk sinh_lk(_Fixed_lk);
_Fixed_lk sqrt_lk(_Fixed_lk);
_Fixed_lk tan_lk(_Fixed_lk);
_Fixed_lk tanh_lk(_Fixed_lk);
_Fixed_lk tgamma_lk(_Fixed_lk);
_Fixed_lk trunc_lk(_Fixed_lk);

	/* concatenation macros */
#define _FX_CONCAT0(prefix, suffix)	prefix##suffix
#define _FX_CONCAT(prefix, suffix)		_FX_CONCAT0(prefix, suffix)

 #if _HAS_FIXED_POINT_LANGUAGE_SUPPORT
#define FIXED_GET(suffix, left)			((double)(right))
#define FIXED_SET(suffix, left)			((_FX_CONCAT(_Fixed_, suffix))(left))

#define FIXED_ADD(suffix, left, right)	((left) + (right))
#define FIXED_SUB(suffix, left, right)	((left) - (right))
#define FIXED_MUL(suffix, left, right)	((left) * (right))
#define FIXED_DIV(suffix, left, right)	((left) / (right))

#define FIXED_LSH(suffix, left, right)	((left) <<= (right))
#define FIXED_RSH(suffix, left, right)	((left) >>= (right))
#define FIXED_NEGATE(suffix, left)			(-(left))

#define FIXED_EQ(suffix, left, right)	((left) == (right))
#define FIXED_LT(suffix, left, right)	((left) < (right))

 #else /* _HAS_FIXED_POINT_LANGUAGE_SUPPORT */
 #define _FIXED_DECL_GET(fun, to_type) \
	to_type _Fixed_##fun##_hr(_Fixed_hr); \
	to_type _Fixed_##fun##_r(_Fixed_r); \
	to_type _Fixed_##fun##_lr(_Fixed_lr); \
	to_type _Fixed_##fun##_hk(_Fixed_hk); \
	to_type _Fixed_##fun##_k(_Fixed_k); \
	to_type _Fixed_##fun##_lk(_Fixed_lk); \
	to_type _Fixed_##fun##_uhr(_Fixed_uhr); \
	to_type _Fixed_##fun##_ur(_Fixed_ur); \
	to_type _Fixed_##fun##_ulr(_Fixed_ulr); \
	to_type _Fixed_##fun##_uhk(_Fixed_uhk); \
	to_type _Fixed_##fun##_uk(_Fixed_uk); \
	to_type _Fixed_##fun##_ulk(_Fixed_ulk)

 #define _FIXED_DECL_SET(fun, from_type) \
	_Fixed_hr _Fixed_##fun##_hr(from_type); \
	_Fixed_r _Fixed_##fun##_r(from_type); \
	_Fixed_lr _Fixed_##fun##_lr(from_type); \
	_Fixed_hk _Fixed_##fun##_hk(from_type); \
	_Fixed_k _Fixed_##fun##_k(from_type); \
	_Fixed_lk _Fixed_##fun##_lk(from_type); \
	_Fixed_uhr _Fixed_##fun##_uhr(from_type); \
	_Fixed_ur _Fixed_##fun##_ur(from_type); \
	_Fixed_ulr _Fixed_##fun##_ulr(from_type); \
	_Fixed_uhk _Fixed_##fun##_uhk(from_type); \
	_Fixed_uk _Fixed_##fun##_uk(from_type); \
	_Fixed_ulk _Fixed_##fun##_ulk(from_type)

 #define _FIXED_DECL_F(fun) \
	_Fixed_hr _Fixed_##fun##_hr(_Fixed_hr); \
	_Fixed_r _Fixed_##fun##_r(_Fixed_r); \
	_Fixed_lr _Fixed_##fun##_lr(_Fixed_lr); \
	_Fixed_hk _Fixed_##fun##_hk(_Fixed_hk); \
	_Fixed_k _Fixed_##fun##_k(_Fixed_k); \
	_Fixed_lk _Fixed_##fun##_lk(_Fixed_lk); \
	_Fixed_uhr _Fixed_##fun##_uhr(_Fixed_uhr); \
	_Fixed_ur _Fixed_##fun##_ur(_Fixed_ur); \
	_Fixed_ulr _Fixed_##fun##_ulr(_Fixed_ulr); \
	_Fixed_uhk _Fixed_##fun##_uhk(_Fixed_uhk); \
	_Fixed_uk _Fixed_##fun##_uk(_Fixed_uk); \
	_Fixed_ulk _Fixed_##fun##_ulk(_Fixed_ulk)

 #define _FIXED_DECL_FF(fun) \
	_Fixed_hr _Fixed_##fun##_hr(_Fixed_hr, _Fixed_hr); \
	_Fixed_r _Fixed_##fun##_r(_Fixed_r, _Fixed_r); \
	_Fixed_lr _Fixed_##fun##_lr(_Fixed_lr, _Fixed_lr); \
	_Fixed_hk _Fixed_##fun##_hk(_Fixed_hk, _Fixed_hk); \
	_Fixed_k _Fixed_##fun##_k(_Fixed_k, _Fixed_k); \
	_Fixed_lk _Fixed_##fun##_lk(_Fixed_lk, _Fixed_lk); \
	_Fixed_uhr _Fixed_##fun##_uhr(_Fixed_uhr, _Fixed_uhr); \
	_Fixed_ur _Fixed_##fun##_ur(_Fixed_ur, _Fixed_ur); \
	_Fixed_ulr _Fixed_##fun##_ulr(_Fixed_ulr, _Fixed_ulr); \
	_Fixed_uhk _Fixed_##fun##_uhk(_Fixed_uhk, _Fixed_uhk); \
	_Fixed_uk _Fixed_##fun##_uk(_Fixed_uk, _Fixed_uk); \
	_Fixed_ulk _Fixed_##fun##_ulk(_Fixed_ulk, _Fixed_ulk)

 #define _FIXED_DECL_FX(fun, rtype) \
	_Fixed_hr _Fixed_##fun##_hr(_Fixed_hr, rtype); \
	_Fixed_r _Fixed_##fun##_r(_Fixed_r, rtype); \
	_Fixed_lr _Fixed_##fun##_lr(_Fixed_lr, rtype); \
	_Fixed_hk _Fixed_##fun##_hk(_Fixed_hk, rtype); \
	_Fixed_k _Fixed_##fun##_k(_Fixed_k, rtype); \
	_Fixed_lk _Fixed_##fun##_lk(_Fixed_lk, rtype); \
	_Fixed_uhr _Fixed_##fun##_uhr(_Fixed_uhr, rtype); \
	_Fixed_ur _Fixed_##fun##_ur(_Fixed_ur, rtype); \
	_Fixed_ulr _Fixed_##fun##_ulr(_Fixed_ulr, rtype); \
	_Fixed_uhk _Fixed_##fun##_uhk(_Fixed_uhk, rtype); \
	_Fixed_uk _Fixed_##fun##_uk(_Fixed_uk, rtype); \
	_Fixed_ulk _Fixed_##fun##_ulk(_Fixed_ulk, rtype)

 #define _FIXED_DECL_CMP(fun) \
	int _Fixed_##fun##_hr(_Fixed_hr, _Fixed_hr); \
	int _Fixed_##fun##_r(_Fixed_r, _Fixed_r); \
	int _Fixed_##fun##_lr(_Fixed_lr, _Fixed_lr); \
	int _Fixed_##fun##_hk(_Fixed_hk, _Fixed_hk); \
	int _Fixed_##fun##_k(_Fixed_k, _Fixed_k); \
	int _Fixed_##fun##_lk(_Fixed_lk, _Fixed_lk); \
	int _Fixed_##fun##_uhr(_Fixed_uhr, _Fixed_uhr); \
	int _Fixed_##fun##_ur(_Fixed_ur, _Fixed_ur); \
	int _Fixed_##fun##_ulr(_Fixed_ulr, _Fixed_ulr); \
	int _Fixed_##fun##_uhk(_Fixed_uhk, _Fixed_uhk); \
	int _Fixed_##fun##_uk(_Fixed_uk, _Fixed_uk); \
	int _Fixed_##fun##_ulk(_Fixed_ulk, _Fixed_ulk)

/* declarations for helper functions */
_FIXED_DECL_GET(get, double);
_FIXED_DECL_SET(set, double);

_FIXED_DECL_FF(add);
_FIXED_DECL_FF(sub);
_FIXED_DECL_FF(mul);
_FIXED_DECL_FF(div);

_FIXED_DECL_FX(lsh, int);
_FIXED_DECL_FX(rsh, int);

_FIXED_DECL_F(neg);

_FIXED_DECL_CMP(eq);
_FIXED_DECL_CMP(lt);

/* helpers */
#define FIXED_GET(suffix, left)	\
	_FX_CONCAT(_Fixed_get_, suffix)(left)
#define FIXED_SET(suffix, left)	\
	_FX_CONCAT(_Fixed_set_, suffix)(left)

#define FIXED_ADD(suffix, left, right)	\
	_FX_CONCAT(_Fixed_add_, suffix)(left, right)
#define FIXED_SUB(suffix, left, right)	\
	_FX_CONCAT(_Fixed_sub_, suffix)(left, right)
#define FIXED_MUL(suffix, left, right)	\
	_FX_CONCAT(_Fixed_mul_, suffix)(left, right)
#define FIXED_DIV(suffix, left, right)	\
	_FX_CONCAT(_Fixed_div_, suffix)(left, right)

#define FIXED_LSH(suffix, left, right)	\
	_FX_CONCAT(_Fixed_lsh_, suffix)(left, right)
#define FIXED_RSH(suffix, left, right)	\
	_FX_CONCAT(_Fixed_rsh_, suffix)(left, right)
#define FIXED_NEGATE(suffix, left)	\
	_FX_CONCAT(_Fixed_neg_, suffix)(left)

#define FIXED_EQ(suffix, left, right)	\
	_FX_CONCAT(_Fixed_eq_, suffix)(left, right)
#define FIXED_LT(suffix, left, right)	\
	_FX_CONCAT(_Fixed_lt_, suffix)(left, right)
 #endif /* _HAS_FIXED_POINT_LANGUAGE_SUPPORT */

 #if _HAS_FIXED_POINT_LANGUAGE_SUPPORT
 #define absfx(x)	__genericfx(x, \
	abshr,, absr,, abslr,, \
	abshk,, absk,, abslk,)(x)
 #define countlsfx(x)	__genericfx(x, \
	countlshr, countlsuhr, countlsr, countlsur, countlslr, countlsulr, \
	countlshk, countlsuhk, countlsk, countlsuk, countlslk, countlsulk)(x)
 #define roundfx(x, n)	__genericfx(x, \
	roundhr, rounduhr, roundr, roundur, roundlr, roundulr, \
	roundhk, rounduhk, roundk, rounduk, roundlk, roundulk)(x, n)
 #endif /* _HAS_FIXED_POINT_LANGUAGE_SUPPORT */

_END_EXTERN_C
_C_STD_END
 #endif /* _STDFIX */

/*
 * Copyright (c) 1992-2009 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.01:1611 */
