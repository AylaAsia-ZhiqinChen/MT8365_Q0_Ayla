/*
 *    Mediatek HiFi 4 Redistribution Version  <0.0.1>
 */

/* ------------------------------------------------------------------------ */
/* Copyright (c) 2016 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED.  */
/* These coded instructions, statements, and computer programs (ÿCadence    */
/* Librariesÿ) are the copyrighted works of Cadence Design Systems Inc.	    */
/* Cadence IP is licensed for use with Cadence processor cores only and     */
/* must not be used for any other processors and platforms. Your use of the */
/* Cadence Libraries is subject to the terms of the license agreement you   */
/* have entered into with Cadence Design Systems, or a sublicense granted   */
/* to you by a direct Cadence licensee.                                     */
/* ------------------------------------------------------------------------ */
/*  IntegrIT, Ltd.   www.integrIT.com, info@integrIT.com                    */
/*                                                                          */
/* DSP Library                                                              */
/*                                                                          */
/* This library contains copyrighted materials, trade secrets and other     */
/* proprietary information of IntegrIT, Ltd. This software is licensed for  */
/* use with Cadence processor cores only and must not be used for any other */
/* processors and platforms. The license to use these sources was given to  */
/* Cadence, Inc. under Terms and Condition of a Software License Agreement  */
/* between Cadence, Inc. and IntegrIT, Ltd.                                 */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2015-2016 IntegrIT, Limited.                      */
/*                      All Rights Reserved.                                */
/* ------------------------------------------------------------------------ */

#include <errno.h>
#include <float.h>

/* DSP Library API */
#include "NatureDSP_Signal.h"
/* Common helper macros. */
#include "common.h"
/* Inline functions for floating-point exceptions and environment control. */
#include "__fenv.h"
/* Tables */
#include "pif_tbl.h"
#include "atanf_tbl.h"
/* +/-Infinity, single precision */
#include "inff_tbl.h"
/* sNaN/qNaN, single precision. */
#include "nanf_tbl.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(float32_t,scl_atan2f,( float32_t y, float32_t x ))
#else
/* If non-zero, set errno and raise floating-point exceptions on errors. */
#define SCL_ATAN2F_ERRH     1

/*===========================================================================
  Scalar matematics:
  scl_atan2          full quadrant Arctangent        
===========================================================================*/

/*-------------------------------------------------------------------------
Vectorized Floating-Point Full-Quadrant Arc Tangent
The functions compute the arc tangent of the ratios y[N]/x[N] and store the
result to output vector z[N]. Functions output is in radians.

NOTE:
1. Scalar function is compatible with standard ANSI C routines and set 
   errno and exception flags accordingly
2. Scalar function assigns EDOM to errno whenever y==0 and x==0.

Special cases:
     y    |   x   |  result   |  extra conditions    
  --------|-------|-----------|---------------------
   +/-0   | -0    | +/-pi     |
   +/-0   | +0    | +/-0      |
   +/-0   |  x    | +/-pi     | x<0
   +/-0   |  x    | +/-0      | x>0
   y      | +/-0  | -pi/2     | y<0
   y      | +/-0  |  pi/2     | y>0
   +/-y   | -inf  | +/-pi     | finite y>0
   +/-y   | +inf  | +/-0      | finite y>0
   +/-inf | x     | +/-pi/2   | finite x
   +/-inf | -inf  | +/-3*pi/4 | 
   +/-inf | +inf  | +/-pi/4   |

Input:
  y[N]  vector of numerator values
  x[N]  vector of denominator values
  N     length of vectors
Output:
  z[N]  results
Domain:
  N>0
---------------------------------------------------------------------------*/

float32_t scl_atan2f( float32_t y, float32_t x )
{
  /*
   * Reference C code for SCL_ATAN2F_ERRH == 0:
   *
   *   const union ufloat32uint32* p;
   *   int sx,sy,big;
   *   sx=takesignf(x);
   *   sy=takesignf(y);
   *   x=fabs(x);
   *   y=fabs(y);
   *   if(x==0.f && y==0.f)
   *   {
   *     // The actual result depends on input signs.
   *     x = 1.f;
   *     y = 0.f;
   *   }
   *  
   *   big=x>y;
   *   if (big)
   *   {
   *     x=y/x;
   *   }
   *   else
   *   {
   *     // compare x==y is necessary to support (+/-Inf, +/-Inf) cases
   *     x=(x==y)? 1.0f : x/y;
   *   }
   *   p=(x<0.5f) ? atanftbl1:atanftbl2;
   *   // approximate atan(x)/x-1
   *   y=    p[0].f;
   *   y=x*y+p[1].f;
   *   y=x*y+p[2].f;
   *   y=x*y+p[3].f;
   *   y=x*y+p[4].f;
   *   y=x*y+p[5].f;
   *   y=x*y+p[6].f;
   *   y=x*y+p[7].f;
   *   // convert result to true atan(x)
   *   y=x*y+x;
   *   if(!big) y=pi2f.f-y;
   *   if(sx)   y=pif.f -y;
   *   if(sy)   y=-y;
   *   return   y;
   */

  /* Polynomial coeff table pointer. */
  const xtfloat * ptbl;
  /* Input values; output value; reducted input value and its 2nd and 4th powers. */
  xtfloatx2 x0, y0, z0, z1, p0, p1, p2;
  /* Numerator; denominator; reciprocal; quotient */
  xtfloatx2 num, den, rcp, quo;
  /* Auxiliary var; error term */
  xtfloatx2 s, eps;
  /* Temporary; input value signs */
  ae_int32x2 t, sx, sy;
  /* Selected polynomial coeffs. */
  xtfloatx2 cf0, cf1, cf2, cf3, cf4, cf5, cf6, cf7;
  /* Inf/Inf; x/Inf; 0/0; x and y are subnormal */
  xtbool2 b_num_inf, b_den_inf, b_eqz, b_subn;
  /* x less than y; x is negative; p is less than 0.5f. */
  xtbool2 b_xlty, b_sx, b_lt05;

#if SCL_ATAN2F_ERRH != 0
  /* Floating-point Status and Control Register values. */
  ae_int64 fstate;
#endif

  x0 = (xtfloatx2)x;
  y0 = (xtfloatx2)y;

  if ( xtbool2_extract_0( XT_UN_SX2( y0, x0 ) ) )
  {
    __Pragma( "frequency_hint never" );
#if SCL_ATAN2F_ERRH != 0
    errno = EDOM;
#endif
    return (qNaNf.f);
  }

  /* Keep sign of x as a boolean. */
  sx = XT_AE_MOVINT32X2_FROMXTFLOATX2( x0 );
  b_sx = AE_LT32( sx, AE_ZERO32() );

  /* Keep y sign as a binary value. */
  sy = XT_AE_MOVINT32X2_FROMXTFLOATX2( y0 );
  sy = AE_SRLI32( sy, 31 );
  sy = AE_SLLI32( sy, 31 );

  x0 = XT_ABS_SX2( x0 );
  y0 = XT_ABS_SX2( y0 );

#if SCL_ATAN2F_ERRH != 0
  /* Sample floating-point exception flags. */
  fstate = XT_AE_MOVVFCRFSR();
#endif

  /* num <= den */
  num = XT_MIN_SX2( x0, y0 );
  den = XT_MAX_SX2( y0, x0 );

  /* Scale up numerator and denominator if BOTH are subnormal. */
  b_subn = XT_OLT_SX2( num, FLT_MIN );
  s = XT_MUL_SX2( num, 8388608.f ); XT_MOVT_SX2( num, s, b_subn );
  s = XT_MUL_SX2( den, 8388608.f ); XT_MOVT_SX2( den, s, b_subn );

  /* Classify numerator and denominator. */
  b_num_inf = XT_OEQ_SX2( num, plusInff.f ); /* Inf/Inf */
  b_den_inf = XT_OEQ_SX2( den, plusInff.f ); /* x/Inf   */
  b_eqz = XT_OEQ_SX2( den, (xtfloatx2)(0.0f) );  /* 0/0     */
  /* Initial appromimation for 1/den. */
  rcp = XT_RECIP0_SX2( den );
  /* Newton-Raphson iteration for 1/den. */
  eps = (xtfloatx2)(1.0f);
  XT_MSUB_SX2( eps, rcp, den );
  XT_MADD_SX2( rcp, rcp, eps );
  /* Approximation for the quotient num/den. */
  quo = XT_MUL_SX2( num, rcp );
  /* Refine the quotient by a modified Newton-Raphson iteration. */
  eps = num;
  XT_MSUB_SX2( eps, quo, den );
  XT_MADD_SX2( quo, rcp, eps );

  /* Force conventional results for special cases. */
  XT_MOVT_SX2(quo, (xtfloatx2)(0.0f), b_den_inf); /* x/Inf -> 0   */
  XT_MOVT_SX2(quo, (xtfloatx2)(1.0f), b_num_inf); /* Inf/Inf -> 1 */
  XT_MOVT_SX2(quo, (xtfloatx2)(0.0f), b_eqz); /* 0/0 -> 0     */

  p0 = quo;

  b_xlty = XT_OLT_SX2( x0, y0 );
  b_lt05 = XT_OLT_SX2( p0, (xtfloatx2)0.5f );

  /* Select coeffs from sets #1, #2 by reducted input value. */
  ptbl = (xtfloat*)( xtbool2_extract_0( b_lt05 ) ? atanftbl1 : atanftbl2 );

  cf0 = ptbl[0]; cf1 = ptbl[1]; cf2 = ptbl[2]; cf3 = ptbl[3];
  cf4 = ptbl[4]; cf5 = ptbl[5]; cf6 = ptbl[6]; cf7 = ptbl[7];

  /*
   * Compute the approximation to z(p) = tan(p)/p-1. We use Estrin's rule to
   * shorten the dependency path and allow for parallel computations at the cost
   * of additional multiplications.
   */

  p1 = XT_MUL_SX2( p0, p0 ); /* p^2 */
  p2 = XT_MUL_SX2( p1, p1 ); /* p^4 */

  XT_MADD_SX2( cf1, cf0, p0 ); cf0 = cf1;
  XT_MADD_SX2( cf3, cf2, p0 ); cf1 = cf3;
  XT_MADD_SX2( cf5, cf4, p0 ); cf2 = cf5;
  XT_MADD_SX2( cf7, cf6, p0 ); cf3 = cf7;

  XT_MADD_SX2( cf1, cf0, p1 ); cf0 = cf1;
  XT_MADD_SX2( cf3, cf2, p1 ); cf1 = cf3;

  XT_MADD_SX2( cf1, cf0, p2 ); cf0 = cf1;

  /* atan(x) = z(p)*p+p */
  XT_MADD_SX2( p0, cf0, p0 ); z0 = p0;

  /* if ( x0<y0 ) z0 = pi2f.f - z0; */
  z1 = XT_SUB_SX2( pi2f.f, z0 ); XT_MOVT_SX2( z0, z1, b_xlty );
  /* if ( sx ) z0 = pif.f - z0; */
  z1 = XT_SUB_SX2( pif.f, z0 ); XT_MOVT_SX2( z0, z1, b_sx );
  /* if ( sy ) z0 = -z0; */
  t = XT_AE_MOVINT32X2_FROMXTFLOATX2( z0 );
  t = AE_XOR32( t, sy );
  z0 = XT_AE_MOVXTFLOATX2_FROMINT32X2( t );

#if SCL_ATAN2F_ERRH != 0
  /* Suppress spurious exception flags and restore original status flags. */
  XT_AE_MOVFCRFSRV( fstate );
  /* Assign EDOM to errno whenever x==0 and y==0. */
  if ( xtbool2_extract_0( b_eqz ) ) { __Pragma( "frequency_hint never" ); errno = EDOM; };
#endif

  return ( XT_HIGH_S( z0 ) );

} /* scl_atan2f() */
#endif
