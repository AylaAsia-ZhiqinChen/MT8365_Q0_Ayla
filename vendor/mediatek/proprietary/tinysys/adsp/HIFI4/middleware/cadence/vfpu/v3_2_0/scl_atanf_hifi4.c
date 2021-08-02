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
#include "pif_tbl.h"
#include "atanf_tbl.h"
/* +/-Infinity, single precision */
#include "inff_tbl.h"
/* sNaN/qNaN, single precision. */
#include "nanf_tbl.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(float32_t,scl_atanf,( float32_t x ))
#else
/* If non-zero, set errno and raise floating-point exceptions on errors. */
#define SCL_ATANF_ERRH     1

/*===========================================================================
  Scalar matematics:
  scl_atan          Arctangent        
===========================================================================*/

/*-------------------------------------------------------------------------
  Arctangent 
  Functions calculate arctangent of number. Fixed point functions 
  scale output to pi so it is always in range -0x20000000 : 0x20000000 
  which corresponds to the real phases +pi/4 and represent input and output 
  in Q31
  NOTE:
  1.  Scalar floating point function is compatible with standard ANSI C
      routines and sets errno and exception flags accordingly

  Accuracy:
  32 bit version: 42    (2.0e-8)
  floating point: 2 ULP

  Precision: 
  32x32  32-bit inputs, 32-bit output.
  f      floating point
 
  Input:
  x[N]   input data, Q31 or floating point
  N      length of vectors
  Output:
  z[N]   result, Q31 or floating point

  Restriction:
  x,z should not overlap

  Scalar versions:
  ----------------
  return result, Q31 or floating point
-------------------------------------------------------------------------*/

float32_t scl_atanf( float32_t x )
{
  /*
   * Reference C code:
   *
   *   float32_t y;
   *   int sx,big;
   *   const union ufloat32uint32* p;
   *
   *   // check for NaN on input
   *   if ( isnan(x) ) 
   *   { 
   *   #if SCL_ATANF_ERRH != 0           
   *       errno = EDOM; 
   *   #endif           
   *       return (qNaNf.f); 
   *   }
   *   // range reduction
   *   sx=x<0;
   *   x=sx?-x:x;
   *   big=x>1.0f;
   *   if (big) x=1.0f/x;
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
   *   if (big) y=pi2f.f-y;
   *   y=sx?-y:y; // apply original sign
   *   return y;
   */

  /* Polynomial coeff table pointer. */
  xtfloat * ptbl;
  /* Polynomial coeffs. */
  xtfloatx2 cf0, cf1, cf2, cf3, cf4, cf5, cf6, cf7;
  /* Input value and it 2nd and 4th powers; output value */
  xtfloatx2 x0, x1, x2, y0, y1;
  /* Integer representation of output value; input sign */
  ae_int32x2 y0_i, sx;
  /* Is >=0.5; is >1; is infinite */
  xtbool2 b_ge05, b_gt1, b_inf;

#if SCL_ATANF_ERRH != 0
  /* Floating-point Status and Control Register values. */
  ae_int64 fstate;
#endif

  x0 = (xtfloatx2)x;

  /* Check for NaN on input */
  if ( xtbool2_extract_0( XT_UN_SX2( x0, x0 ) ) ) 
  { 
    __Pragma( "frequency_hint never" );
#if SCL_ATANF_ERRH != 0
    errno = EDOM;
#endif
    return (qNaNf.f);
  }

  /* Extract the input sign. */
  sx = XT_AE_MOVINT32X2_FROMXTFLOATX2( x0 );
  sx = AE_SRLI32( sx, 31 );
  sx = AE_SLLI32( sx, 31 );

  x0 = XT_ABS_SX2( x0 );
  
  /*
   * Range reduction:
   *   a) x<=1.0: x <- x
   *   b) 1.0<x : x <- 1/x
   */

  b_gt1 = XT_OLT_SX2( (xtfloatx2)1.0f, x0 );
  b_inf = XT_OLE_SX2( plusInff.f, x0 );

#if SCL_ATANF_ERRH != 0
  /* Sample floating-point exception flags. */
  fstate = XT_AE_MOVVFCRFSR();
#endif

  if ( xtbool2_extract_0( b_gt1 ) )
  {
    x0 = XT_RECIP_SX2( x0 );
    /* Force a conventional result of zero for 1/Inf. */
    XT_MOVT_SX2( x0, (xtfloatx2)0.0f, b_inf );
  }

  /*
   * Select the polynomial and load coeffs.
   */

  b_ge05 = XT_OLE_SX2( (xtfloatx2)0.5f, x0 );

  ptbl = (xtfloat*)( xtbool2_extract_0( b_ge05 ) ? atanftbl2 : atanftbl1 );

  cf0 = ptbl[0]; cf1 = ptbl[1]; cf2 = ptbl[2]; cf3 = ptbl[3];
  cf4 = ptbl[4]; cf5 = ptbl[5]; cf6 = ptbl[6]; cf7 = ptbl[7];

  /*
   * Approximate p(x) = atan(x)/x-1. The polynomial is evaluated by Estrin's
   * rule, as this shortens the dependency path at the cost of a few additional
   * multiplications.
   */

  x1 = XT_MUL_SX2( x0, x0 ); /* x^2 */
  x2 = XT_MUL_SX2( x1, x1 ); /* x^4 */

  XT_MADD_SX2( cf1, cf0, x0 ); cf0 = cf1;
  XT_MADD_SX2( cf3, cf2, x0 ); cf1 = cf3;
  XT_MADD_SX2( cf5, cf4, x0 ); cf2 = cf5;
  XT_MADD_SX2( cf7, cf6, x0 ); cf3 = cf7;

  XT_MADD_SX2( cf1, cf0, x1 ); cf0 = cf1;
  XT_MADD_SX2( cf3, cf2, x1 ); cf1 = cf3;

  XT_MADD_SX2( cf1, cf0, x2 ); cf0 = cf1;

  /* atan(x) = p*x+x */
  XT_MADD_SX2( x0, cf0, x0 ); y0 = x0;

  /* Account for the range reduction. */
  y1 = XT_SUB_SX2( pi2f.f, y0 );
  XT_MOVT_SX2( y0, y1, b_gt1 );

#if SCL_ATANF_ERRH != 0
  /* Suppress spurious exception flags and restore original status flags. */
  XT_AE_MOVFCRFSRV( fstate );
#endif

  /* Propagate the input sign. */
  y0_i = XT_AE_MOVINT32X2_FROMXTFLOATX2( y0 );
  y0_i = AE_OR32( y0_i, sx );
  y0 = XT_AE_MOVXTFLOATX2_FROMINT32X2( y0_i );

  return ( XT_HIGH_S( y0 ) );

} /* scl_atanf() */
#endif
