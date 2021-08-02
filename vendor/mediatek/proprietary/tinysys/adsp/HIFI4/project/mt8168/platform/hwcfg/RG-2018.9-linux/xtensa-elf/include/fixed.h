/* fixed.h internal header -- fixed-point compiler runtime from C TR18037 */
#ifndef _FIXED
#define _FIXED
#include <stdfix.h>	/* fixed-point visible library */

_C_STD_BEGIN
_EXTERN_C

	/* MACROS */
#define MAX_BITS	64

	/* fxmask* bits macros */
#define FX_TMASK	0x18
#define FX_SHORT	0x00
#define FX_DEFAULT	0x08
#define FX_LONG		0x10
#define FX_INTEGER	0x18

#define FX_ACCUM	0x20
#define FX_UNSIGNED	0x40
#define FX_SAT		0x80

#define FX_FRACT_OVERFLOW	0x1
#define FX_ACCUM_OVERFLOW	0x2
#define FX_FULL_PRECISION	0x4

#define FX_INDEX(mask)	(((mask) & (FX_TMASK | FX_ACCUM | FX_UNSIGNED)) >> 3)

#define F2_OFF	5	/* offset of field 2 in fxmask[cr2] */
#define F3_OFF	10	/* offset of field 3 in fxmask2 */

#define FX_SIGN(mask, value)	((value) & FX_SIGNBIT(mask))
#define FX_VALUE(mask, value)	(FX_SIGN(mask, value) \
	? (FX_SIGNBIT(mask) << 1) - (value) : (value))

#define FX_FRACBITS(mask)	\
	_Fixed_fracbits[FX_INDEX(mask)]
#define FX_MAXVAL(mask)	\
	_Fixed_maxval[FX_INDEX(mask)]
#define FX_ONE(mask)	\
	_Fixed_one[FX_INDEX(mask)]
#define FX_SIGNBIT(mask)	\
	_Fixed_signbit[FX_INDEX(mask)]

	/* TYPES */
typedef _ULonglong fxvalue;

typedef unsigned short fxmask1;	/* op/result */
typedef unsigned short fxmaskf;	/* op */
typedef unsigned short fxmaskg;	/* result */
typedef unsigned short fxmaskc;	/* op, result */
typedef unsigned short fxmaskr;	/* op1, op2 */

typedef unsigned long fxmask2;	/* op1, op2, result */

	/* ABI FUNCTIONS */
fxvalue _Fixed_negate(fxmask1, fxvalue);	/* -x */
fxvalue _Fixed_incr(fxmask1, fxvalue);	/* x+1 */
fxvalue _Fixed_decr(fxmask1, fxvalue);	/* x+1 */

fxvalue _Fixed_add(fxmask2, fxvalue, fxvalue);	/* x+y */
fxvalue _Fixed_subtract(fxmask2, fxvalue, fxvalue);	/* x-y */
fxvalue _Fixed_multiply(fxmask2, fxvalue, fxvalue);	/* x*y */
fxvalue _Fixed_divide(fxmask2, fxvalue, fxvalue);	/* x/y */

int _Fixed_eq(fxmaskr, fxvalue, fxvalue);	/* x==y */
int _Fixed_ne(fxmaskr, fxvalue, fxvalue);	/* x!=y */
int _Fixed_gt(fxmaskr, fxvalue, fxvalue);	/* x>y */
int _Fixed_lt(fxmaskr, fxvalue, fxvalue);	/* x<y */
int _Fixed_ge(fxmaskr, fxvalue, fxvalue);	/* x>=y */
int _Fixed_le(fxmaskr, fxvalue, fxvalue);	/* x<=y */

fxvalue _Fixed_shiftl(fxmask1, fxvalue, int);	/* x<<count */
fxvalue _Fixed_shiftr(fxmask1, fxvalue, int);	/* x>>count */

fxvalue _Fixed_conv(fxmaskc, fxvalue);	/* (T)x */
float _Fixed_to_float(fxmaskf, fxvalue);	/* (float)x */
double _Fixed_to_double(fxmaskf, fxvalue);	/* (double)x */
long double _Fixed_to_ldouble(fxmaskf,
	fxvalue);	/* (long double)x */

fxvalue _Fixed_from_float(fxmaskg, float);	/* (T)x */
fxvalue _Fixed_from_double(fxmaskg, double);	/* (T)x */
fxvalue _Fixed_from_ldouble(fxmaskg, long double);	/* (T)x */

	/* INTERNAL FUNCTIONS */
int _Fixed_unpack(fxmaskf, fxvalue *);
fxvalue _Fixed_pack(fxmask1, fxvalue, int);
fxvalue _Fixed_stof(fxmaskf, const char *, int, char **);

	/* INTERNAL OBJECTS */
extern int _Fixed_fracbits[];
extern fxvalue _Fixed_maxval[];
extern fxvalue _Fixed_one[];
extern fxvalue _Fixed_signbit[];

_END_EXTERN_C
_C_STD_END
#endif /* _FIXED */

/*
 * Copyright (c) 1992-2009 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.01:1611 */
