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

/* DSP Library API */
#include "NatureDSP_Signal.h"
/* Common helper macros. */
#include "common.h"
/* Inline functions for floating-point exceptions and environment control. */
#include "__fenv.h"
/* Tables */
#include "expf_tbl.h"
/* +/-Infinity, single precision */
#include "inff_tbl.h"
/* sNaN/qNaN, single precision. */
#include "nanf_tbl.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(float32_t,scl_antilognf,( float32_t x ))
#else
/* If non-zero, set errno and raise floating-point exceptions on errors. */
#define SCL_ANTILOGNF_ERRH     1

/*===========================================================================
  Scalar matematics:
  scl_antilog          Antilogarithm         
===========================================================================*/

/*-------------------------------------------------------------------------
  Antilogarithm
  These routines calculate antilogarithm (base2, natural and base10). 
  Fixed-point functions accept inputs in Q25 and form outputs in Q16.15 
  format and return 0x7FFFFFFF in case of overflow and 0 in case of 
  underflow.

  Precision:
  32x32  32-bit inputs, 32-bit outputs. Accuracy: 4*e-5*y+1LSB
  f      floating point: Accuracy: 2 ULP
  NOTE:
  1.  Floating point functions are compatible with standard ANSI C routines 
      and set errno and exception flags accordingly

  Input:
  x[N]  input data,Q25 or floating point 
  N     length of vectors
  Output:
  y[N]  output data,Q16.15 or floating point  

  Restriction:
  x,y should not overlap

  Scalar versions:
  ----------------
  fixed point functions return result in Q16.15

-------------------------------------------------------------------------*/

float32_t scl_antilognf( float32_t x )
{
  /*
   * Reference C coce for SCL_ANTILOGNF_ERRH == 0:
   *
   *   int32_t t, y;
   *   int e;
   *   int64_t a;
   *   
   *   // scale input to 1/ln(2) and convert to Q31
   *   x = frexpf(x, &e);
   *   
   *   t = (int32_t)STDLIB_MATH(ldexpf)(x, e + 24);
   *   a = ((int64_t)t*invln2_Q30) >> 22; // Q24*Q30->Q32
   *   t = ((uint32_t)a) >> 1;
   *   e = (int32_t)(a >> 32);
   *   // compute 2^t in Q30 where t is in Q31
   *   y = expftbl_Q30[0];
   *   y = satQ31((((int64_t)t*y) + (1LL << (31 - 1))) >> 31) + expftbl_Q30[1];
   *   y = satQ31((((int64_t)t*y) + (1LL << (31 - 1))) >> 31) + expftbl_Q30[2];
   *   y = satQ31((((int64_t)t*y) + (1LL << (31 - 1))) >> 31) + expftbl_Q30[3];
   *   y = satQ31((((int64_t)t*y) + (1LL << (31 - 1))) >> 31) + expftbl_Q30[4];
   *   y = satQ31((((int64_t)t*y) + (1LL << (31 - 1))) >> 31) + expftbl_Q30[5];
   *   y = satQ31((((int64_t)t*y) + (1LL << (31 - 1))) >> 31) + expftbl_Q30[6];
   *   // convert back to the floating point
   *   x = STDLIB_MATH(ldexpf)((float32_t)y, e - 30);
   *   return x;
   */

  /* Input value; output value; polynomial approximation */
  xtfloatx2 x0, y0, q;
  /* Fixed-point input value; polynomial value. */
  ae_int32x2 p, g;
  /* Polynomial coeffs. */
  ae_int32x2 cf0, cf1, cf2, cf3, cf4, cf5, cf6;
  /* Fractional part and its 2nd power. */
  ae_int32x2 f0, f1;
  /* Exponential parts. */
  ae_int32x2 e0, e1;
  /* Scale factors. */
  xtfloatx2 s0, s1;
  /* Auxiliary var. */
  ae_f32x2 t;
  /* Wide auxiliary vars. */
  ae_int64 w0, w1;

#if SCL_ANTILOGNF_ERRH != 0
  /* Floating-point Status and Control Register values. */
  ae_int64 fstate;
#endif

  x0 = (xtfloatx2)x;

  if ( xtbool2_extract_0( XT_UN_SX2( x0, x0 ) ) )
  {
    __Pragma( "frequency_hint never" );
#if SCL_ANTILOGNF_ERRH != 0
    errno = EDOM;
#endif
    return (qNaNf.f);
  }

#if SCL_ANTILOGNF_ERRH != 0
  /* Sample floating-point exception flags. */
  fstate = XT_AE_MOVVFCRFSR();
#endif

  p = XT_TRUNC_SX2( x0, 24 );

  /* Scale input by 1/ln(2) and convert to Q31 */
  w0 = AE_MUL32_HH( p, invln2_Q30 );
  e0 = AE_TRUNCA32X2F64S( w0, w0, -22 );
  w1 = AE_SLLI64( w0, 32-22 );
  f0 = AE_TRUNCI32X2F64S( w1, w1, 0 );
  f0 = AE_SRLI32( f0, 1 );

  /*
   * Compute 2^fract(x/ln2) in Q30, where f=x/ln2 is in Q31. Use a
   * combination of Estrin's rule and Horner's method to evaluate
   * the polynomial.
   */

  cf0 = expftbl_Q30[0]; cf1 = expftbl_Q30[1];
  cf2 = expftbl_Q30[2]; cf3 = expftbl_Q30[3];
  cf4 = expftbl_Q30[4]; cf5 = expftbl_Q30[5];
  cf6 = expftbl_Q30[6];

  f1 = AE_MULFP32X2RAS( f0, f0 );

  t = cf2; AE_MULAFP32X2RAS( t, f0, cf1 ); cf1 = t;
  t = cf4; AE_MULAFP32X2RAS( t, f0, cf3 ); cf2 = t;
  t = cf6; AE_MULAFP32X2RAS( t, f0, cf5 ); cf3 = t;

  t = cf0;                               g = t;
  t = cf1; AE_MULAFP32X2RAS( t, f1, g ); g = t;
  t = cf2; AE_MULAFP32X2RAS( t, f1, g ); g = t;
  t = cf3; AE_MULAFP32X2RAS( t, f1, g ); g = t;

  q = XT_FLOAT_SX2( g, 30 );

  /*
   * Calculate 2^int(x/ln2) * 2^fract(x/ln2)
   */

  e0 = AE_ADD32( e0, 254 );
  e1 = AE_SRAI32( e0, 1 );
  e0 = AE_SUB32( e0, e1 );
  e0 = AE_SLAI32( e0, 23 );
  e1 = AE_SLAI32( e1, 23 );
  s0 = XT_AE_MOVXTFLOATX2_FROMINT32X2( e0 );
  s1 = XT_AE_MOVXTFLOATX2_FROMINT32X2( e1 );

  s0 = XT_MUL_SX2( s0, s1 );
  y0 = XT_MUL_SX2( s0, q );

#if SCL_ANTILOGNF_ERRH != 0
  {
    /* Is input less than infinity; is output equal to infinity. */
    xtbool2 b_xfin, b_yinf;

    /* Suppress spurious exception flags and restore original status flags. */
    XT_AE_MOVFCRFSRV( fstate );

    b_xfin = XT_OLT_SX2( x0, plusInff.f );
    b_yinf = XT_OEQ_SX2( y0, plusInff.f );

    /* Check for an overflow. */
    if ( XT_ANDB( xtbool2_extract_0(b_xfin), xtbool2_extract_0(b_yinf) ) )
    {
      __Pragma( "frequency_hint never" );
      __feraiseexcept( FE_OVERFLOW );
      errno = ERANGE;
    }
  }
#endif

  return ( XT_HIGH_S(y0) );

} /* scl_antilognf() */
#endif
