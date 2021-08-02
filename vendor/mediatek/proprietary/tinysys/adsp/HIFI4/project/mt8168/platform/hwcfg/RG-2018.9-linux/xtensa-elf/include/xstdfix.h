/* xstdfix.h internal header -- fixed-point arithmetic from C TR18037 */
#ifndef _XSTDFIX
#define _XSTDFIX
#include "fixed.h"	/* fixed-point compiler runtime */

_C_STD_BEGIN
_EXTERN_C

/* NOTE: This header must be preceded by one of the headers xxfixed_*.h */

 #ifdef _FIXED_SAT
  #define IS_SAT	_FIXED_SAT

 #else /* _FIXED_SAT */
  #define IS_SAT	1	/* saturated by default */
 #endif /* _FIXED_SAT */

	/* fixed mask codes */
#define FIXED_hr	(FX_SHORT)
#define FIXED_uhr	(FX_SHORT| FX_UNSIGNED)
#define FIXED_hk	(FX_SHORT | FX_ACCUM)
#define FIXED_uhk	(FX_SHORT | FX_UNSIGNED | FX_ACCUM)

#define FIXED_r		(FX_DEFAULT)
#define FIXED_ur	(FX_DEFAULT | FX_UNSIGNED)
#define FIXED_k		(FX_DEFAULT | FX_ACCUM)
#define FIXED_uk	(FX_DEFAULT | FX_UNSIGNED | FX_ACCUM)

#define FIXED_lr	(FX_LONG)
#define FIXED_ulr	(FX_LONG | FX_UNSIGNED)
#define FIXED_lk	(FX_LONG | FX_ACCUM)
#define FIXED_ulk	(FX_LONG | FX_UNSIGNED | FX_ACCUM)

	/* fixed conversion codes */
#define FIXED_OFLO	1
#define FIXED_NEG	2

	/* property macros */
#define FIXED_MASK	_FX_CONCAT(FIXED_, FIXED_SUF)
#define IS_SIGNED	((FIXED_MASK & FX_UNSIGNED) == 0)

#define FTYPE	_FX_CONCAT(_Fixed_, FIXED_SUF)
#define ITYPE	_FX_CONCAT(int_, _FX_CONCAT(FIXED_SUF, _t))
#define UTYPE	_FX_CONCAT(uint_, _FX_CONCAT(FIXED_SUF, _t))

 #if IS_SIGNED
  #define XTYPE1	signed

 #else /* IS_SIGNED */
  #define XTYPE1	unsigned
 #endif /* IS_SIGNED */

 #if FIXED_MASK & FX_LONG
  #define XTYPE	XTYPE1 long int

 #else /* FIXED_MASK & FX_LONG */
  #define XTYPE	XTYPE1 int
 #endif /* FIXED_MASK & FX_LONG */

#define FCONST(x)	x##_val(FIXED_SUF)
#define FFUN(fun)	_FX_CONCAT(_FX_CONCAT(fun, _), FIXED_SUF)
#define FLIT(lit)	_FX_CONCAT(lit, FIXED_SUF)
#define FNAME(fun)	_FX_CONCAT(_, FFUN(fun))
#define PFUN(fun)	_FX_CONCAT(FIXED_SUF, fun)
#define SFUN(fun)	_FX_CONCAT(fun, FIXED_SUF)

 #if _HAS_FIXED_POINT_LANGUAGE_SUPPORT
  #define Eps_val(suf)	(_Half(suf) >> FBITS - 1)
  #define Max_val(suf)	(-(Min_val(suf) + Eps_val(suf)))
  #define Min_val(suf)	(-_Half(suf) << IBITS + 1)
  #define Nan_val(suf)	(-_Half(suf) << IBITS + 1)
  #define Rteps_val(suf)	(_Half(suf) >> FBITS / 2)

 #else /* _HAS_FIXED_POINT_LANGUAGE_SUPPORT */
  #define Eps_val(suf)	_Fixed_eps_##suf
  #define Max_val(suf)	_Fixed_max_##suf
  #define Min_val(suf)	_Fixed_min_##suf
  #define Nan_val(suf)	_Fixed_nan_##suf
  #define Rteps_val(suf)	_Fixed_rteps_##suf
 #endif /* _HAS_FIXED_POINT_LANGUAGE_SUPPORT */

	/* properties for current fixed type */
#define WORDBITS		(IS_SIGNED + FBITS + IBITS)
#define MSBIT			((UTYPE)1 << (WORDBITS - 1))

 #if IBITS == 0
  #define MAXINT		0	/* largest representable integer */

 #elif IBITS < WORDBITS
  #define MAXINT		(((UTYPE)1 << IBITS) - 1)

 #else /* IBITS */
  #define MAXINT		((UTYPE)(-1))
 #endif /* IBITS */

 #if IS_SIGNED
  #define MAXVAL		((UTYPE)(-1) >> 1)	/* parameters for signed */
  #define MININT		MAXINT	/* positive -- one less than most neg mag */
  #define MINVAL		SIGNBIT
  #define SIGNBIT		MSBIT

  #define INTMAG(x)		(NEGATIVE(x) ? 0 - (x) : (x))
  #define NEGATIVE(x)	(((x) & SIGNBIT) != 0)

 #else /* IS_SIGNED */
  #define MAXVAL		((UTYPE)(-1))	/* parameters for unsigned */
  #define MININT		0
  #define MINVAL		0
  #define SIGNBIT		0

  #define INTMAG(x)		(x)
  #define NEGATIVE(x)	0
 #endif /* IS_SIGNED */

 #if FBITS == 0
  #define LSH(x)		(x)
  #define RSH(x)		(x)

 #elif FBITS < WORDBITS
  #define LSH(x)		((x) << FBITS)
  #define RSH(x)		((x) >> FBITS)

 #else /* FBITS */
  #define LSH(x)		0
  #define RSH(x)		0
 #endif /* FBITS */

#define SATURATE(min)	((min) != 0 ? MINVAL : MAXVAL)
#define TO_FTYPE(x)		(*(FTYPE *)&(x))
#define TO_UTYPE(x)		(*(UTYPE *)&(x))

	/* supplemental types (to simplify building type names) */
typedef uint_uhr_t uint_hr_t;
typedef uint_ur_t uint_r_t;
typedef uint_ulr_t uint_lr_t;

typedef uint_uhr_t int_uhr_t;
typedef uint_ur_t int_ur_t;
typedef uint_ulr_t int_ulr_t;

typedef uint_uhk_t uint_hk_t;
typedef uint_uk_t uint_k_t;
typedef uint_ulk_t uint_lk_t;

typedef uint_uhk_t int_uhk_t;
typedef uint_uk_t int_uk_t;
typedef uint_ulk_t int_ulk_t;

	/* type DBLU_TYPE */

 #if FIXED_MASK < FIXED_lk
typedef _ULonglong DBLU_TYPE;

  #define DBLU_GET(left)		((UTYPE)(left))
  #define DBLU_SET(left, right)	((left) = (DBLU_TYPE)(right))
  #define DBLU_LT(left, right)	((left) < (right))

  #define DBLU_ADD(left, right)	{(left) += (right); }
  #define DBLU_SUB(left, right)	{(left) -= (right); }
  #define DBLU_MUL(left, right)	{(left) *= (right); }
  #define DBLU_LSH(left, right)	{(left) <<= (right); }
  #define DBLU_RSH(left, right)	{(left) >>= (right); }

 #else /* FIXED_MASK < FIXED_lk */
typedef struct {
	UTYPE val[2];
	} DBLU_TYPE;

  #define DBLU_GET(left)		((left).val[0])
  #define DBLU_SET(left, right)	\
	((left).val[1] = 0, (left).val[0] = (UTYPE)(right))
  #define DBLU_LT(left, right)	((left).val[1] < (right).val[1] \
	|| !((right).val[1] < (left).val[1]) && (left).val[0] < (right).val[0])

  #define DBLU_ADD(left, right)	\
	{UTYPE _Lsw = (left).val[0]; \
	_Lsw = (UTYPE)(_Lsw + (right).val[0]); \
	if (_Lsw < (left).val[0] || _Lsw < (right).val[0]) \
		++(left).val[1]; \
	(left).val[1] = (UTYPE)((left).val[1] + (right).val[1]); \
	(left).val[0] = _Lsw; \
	}

  #define DBLU_SUB(left, right)	\
	{if ((left).val[0] < (right).val[0]) \
		--(left).val[1]; \
	(left).val[1] = (UTYPE)((left).val[1] - (right).val[1]); \
	(left).val[0] = (UTYPE)((left).val[0] - (right).val[0]); \
	}

  #define DBLU_MUL(left, right)	\
	{DBLU_TYPE _Ans = {0}; \
	if ((right) != 0 && ((left).val[0] != 0 || (left).val[1] != 0)) \
		{UTYPE bit = MSBIT; \
		for (; bit != 0; bit >>= 1) \
			{DBLU_LSH(_Ans, 1); \
			if (((right) & bit) != 0) \
				DBLU_ADD(_Ans, left); \
			} \
		} \
	(left) = _Ans; \
			}

  #define DBLU_LSH(left, right)	\
	{int _Count = (right); \
	for (; 0 < _Count; --_Count) \
		{(left).val[1] <<= 1; \
		if (((left).val[0] & MSBIT) != 0) \
			(left).val[1] |= 1; \
		(left).val[0] <<= 1; \
		} \
		}

  #define DBLU_RSH(left, right)	\
	{int _Count = (right); \
	for (; 0 < _Count; --_Count) \
		{(left).val[0] >>= 1; \
		if (((left).val[1] & 1) != 0) \
			(left).val[0] |= MSBIT; \
		(left).val[1] >>= 1; \
		} \
		}
 #endif /* FIXED_MASK < FIXED_lk */

	/* functions */
UTYPE FNAME(Div)(DBLU_TYPE, UTYPE, int, int, int);
UTYPE FNAME(Mul)(UTYPE, UTYPE, int, int, int);

FTYPE FNAME(Atan)(FTYPE, int);
FTYPE FNAME(Dint)(FTYPE, int);
FTYPE FNAME(Log)(FTYPE, int);
void FNAME(Log2)(FTYPE, FTYPE, FTYPE [6]);
int FNAME(Quad)(FTYPE *, FTYPE *);
FTYPE FNAME(Sin)(FTYPE, int);
FTYPE FNAME(Tgamma)(FTYPE, int *);

extern FTYPE FNAME(Fixed_rteps);
extern FTYPE FNAME(Gamma_big);

_END_EXTERN_C
_C_STD_END
#endif /* _XSTDFIX */

/*
 * Copyright (c) 1992-2009 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.01:1611 */
