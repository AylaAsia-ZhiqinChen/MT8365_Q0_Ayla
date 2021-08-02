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
/* Value of 2/pi, 4/pi, etc. */
#include "inv2pif_tbl.h"
/* Angular argument reduction constants. */
#include "sinf_tbl.h"
/* tan/cotan approximation polynomial coeffs. */
#include "tanf_tbl.h"
/* sNaN/qNaN, single precision. */
#include "nanf_tbl.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(float32_t,scl_tanf,(float32_t x))
#else
/* If non-zero, set errno and raise floating-point exceptions on errors. */
#define SCL_TANF_ERRH     1

/*===========================================================================
  Scalar matematics:
  scl_tan             Tangent    
===========================================================================*/

/*-------------------------------------------------------------------------
  Tangent 
  Fixed point functions calculate tan(pi*x) for number written in Q31. 
  Floating point functions compute tan(x)
  
  Precision: 
  32x32  32-bit inputs, 32-bit outputs. Accuracy: (1.3e-4*y+1LSB)
                                        if abs(y)<=464873(14.19 in Q15) 
                                        or abs(x)<pi*0.4776
  f      floating point.                Accuracy: 2 ULP

  NOTE:
  1.  Scalar floating point function is compatible with standard ANSI C routines 
      and set errno and exception flags accordingly
  2.  Floating point functions limit the range of allowable input values: [-9099, 9099]
      Whenever the input value does not belong to this range, the result is set to NaN.

  Input:
  x[N]   input data,Q31 or floating point
  N      length of vectors
  Output:
  y[N]   result, Q16.15 or floating point

  Restriction:
  x,y - should not overlap

  Scalar versions:
  ----------------
  Return result, Q16.15 or floating point
-------------------------------------------------------------------------*/

float32_t scl_tanf (float32_t x)
{
  /*
   * Reference C code for SCL_TANF_ERRH == 0:
   *
   *   float32_t x2,y,yt,yc;
   *   int sx,n,j,st;
   *   sx=takesignf(x);
   *   x=sx?-x:x;
   *   // argument reduction
   *   // process reduces x by integral multiple of pi/4.
   *   // The output is deduced to the sum of two single precision 
   *   // floating point values x+dx. 
   *   n=(int)STDLIB_MATH(ceilf)(x*inv4pif.f);
   *   j=n&~1;
   *  
   *   {
   *       float32_t dx,t,y=x,jj=(float32_t)j;
   *       const union ufloat32uint32 c[6]={
   *                                   {0x3f4a0000},
   *                                   {0xbb700000},
   *                                   {0xb6160000},
   *                                   {0x32080000},
   *                                   {0x2e060000},
   *                                   {0xa9b9ee5a}};
   *       dx=0.f;
   *            y-=c[0].f*jj;
   *            y-=c[1].f*jj;
   *            y-=c[2].f*jj;
   *       t=y; y-=c[3].f*jj; t=(t-y); t-=c[3].f*jj; dx = t; 
   *       t=y; y-=c[4].f*jj; t=(t-y); t-=c[4].f*jj; dx = (dx+t); 
   *       t=y; y-=c[5].f*jj; t=(t-y); t-=c[5].f*jj; dx = (dx+t); 
   *       y=(y+dx);
   *       x=y;
   *   }
   *  
   *   // compute tan via minmax polynomial
   *   x2=x*x;
   *   yt=      polytanf_tbl[0].f;
   *   yt=yt*x2+polytanf_tbl[1].f;
   *   yt=yt*x2+polytanf_tbl[2].f;
   *   yt=yt*x2+polytanf_tbl[3].f;
   *   yt=yt*x2+polytanf_tbl[4].f;
   *   yt=yt*x2+polytanf_tbl[5].f;
   *   #if POLYTAN_LEN>=7
   *   yt=yt*x2+polytanf_tbl[6].f;
   *   #endif
   *   yt=yt*x2;
   *   // dx is small enough (<3e-8) and wiil be used to modify 
   *   // tangent value computed by polynomial using derivation 
   *   // tg(x+dx)=tg(x)+dx/cos(x)^2
   *   // for 2 ULP operation it is enough to suppose 1/cos(x)^2 ~ 1
   *   // for 1 ULP operation, it should be computed accurately
   *   //
   *   // resulting value is decomposed as follows
   *   // tag(x+dx)~(P*x+dx)+x
   *   // The order of summation is crucial!
   *   yt = (yt*x) + x;
   *   yc=1.f/yt;
   *  
   *   // adjust sign
   *   n=(n>>1)&1;
   *   st = sx ^ n;
   *   // select tan/cotan
   *   y = n ? yc:yt;
   *   // apply the sign
   *   y=changesignf(y,st);
   *   return y;
   */

  /* Input value and its absolute; output value. */
  xtfloatx2 x0, x1, z0;
  /* Input sign; result sign; auxiliary var.  */
  ae_int32x2 sx, sz, s;
  /* Reducted input value and its powers; correction term. */
  xtfloatx2 p, p2, p3, p4, p8, dp;
  /* Auxiliary floating-point vars. */
  xtfloatx2 t, r;
  /* Input value segment number. */
  xtfloatx2 jf;
  ae_int32x2 ji;
  /* pi/2 splitted into 7-bit chunks. */
  xtfloatx2 c0, c1, c2, c3, c4, c5;
  /* Polynomial coeffs. */
  xtfloatx2 cf0, cf1, cf2, cf3, cf4, cf5, cf6;

#if SCL_TANF_ERRH != 0
  /* Floating-point Status and Control Register values. */
  ae_int64 fstate;
#endif

  /* pi/2 splitted into 7-bit chunks. */
  static const union ufloat32uint32 c[6] = {
    { 0x3fca0000 }, { 0xbbf00000 },
    { 0xb6960000 }, { 0x32880000 },
    { 0x2e860000 }, { 0xaa39ee5a }
  };

  x0 = (xtfloatx2)x;

  if ( xtbool2_extract_0( XT_UN_SX2( x0, x0 ) ) )
  {
    __Pragma( "frequency_hint never" );
#if SCL_TANF_ERRH != 0
    errno = EDOM;
#endif
    return (qNaNf.f);
  }

  if ( xtbool2_extract_0( XT_OLT_SX2( tanf_maxval, XT_ABS_SX2(x0) ) ) )
  {
    __Pragma( "frequency_hint never" );
    return (qNaNf.f);
  }

  x1 = XT_ABS_SX2( x0 );

#if SCL_TANF_ERRH != 0
  /* Sample floating-point exception flags. */
  fstate = XT_AE_MOVVFCRFSR();
#endif

  /* Determine the pi/2-wide segment the input value belongs to. */
  jf = XT_MUL_SX2( x1, inv2pif.f );
  jf = XT_FIROUND_SX2( jf );
  ji = XT_TRUNC_SX2( jf, 0 );
  ji = AE_SLLI32( ji, 31 );

  /*
   * Calculate the difference between the segment midpoint and input value.
   */

  c0 = (xtfloatx2)c[0].f; c1 = (xtfloatx2)c[1].f;
  c2 = (xtfloatx2)c[2].f; c3 = (xtfloatx2)c[3].f;
  c4 = (xtfloatx2)c[4].f; c5 = (xtfloatx2)c[5].f;

  p = x1;

  XT_MSUB_SX2( p, jf, c0 );
  XT_MSUB_SX2( p, jf, c1 );
  XT_MSUB_SX2( p, jf, c2 );

  r = XT_MUL_SX2(jf,c3); t = p; p = XT_SUB_SX2(p,r); t = XT_SUB_SX2(t,p); t = XT_SUB_SX2(t,r); dp = t;
  r = XT_MUL_SX2(jf,c4); t = p; p = XT_SUB_SX2(p,r); t = XT_SUB_SX2(t,p); t = XT_SUB_SX2(t,r); dp = XT_ADD_SX2(t,dp);
  r = XT_MUL_SX2(jf,c5); t = p; p = XT_SUB_SX2(p,r); t = XT_SUB_SX2(t,p); t = XT_SUB_SX2(t,r); dp = XT_ADD_SX2(t,dp);

  p = XT_ADD_SX2( p, dp );

  /*
   * Evaluate a polynomial approximation to y(p) = tan(p)/p-1. We use Estrin's rule to
   * shorten the dependency path and allow for parallel computations at the cost
   * of additional multiplications.
   */

  cf0 = (xtfloatx2)polytanf_tbl[0].f; cf1 = (xtfloatx2)polytanf_tbl[1].f;
  cf2 = (xtfloatx2)polytanf_tbl[2].f; cf3 = (xtfloatx2)polytanf_tbl[3].f;
  cf4 = (xtfloatx2)polytanf_tbl[4].f; cf5 = (xtfloatx2)polytanf_tbl[5].f;
  cf6 = (xtfloatx2)polytanf_tbl[6].f;

  p2 = XT_MUL_SX2( p, p );
  p3 = XT_MUL_SX2( p2, p );
  p4 = XT_MUL_SX2( p2, p2 );
  p8 = XT_MUL_SX2( p4, p4 );

  XT_MADD_SX2( cf2, cf1, p2 ); cf1 = cf2;
  XT_MADD_SX2( cf4, cf3, p2 ); cf2 = cf4;
  XT_MADD_SX2( cf6, cf5, p2 ); cf3 = cf6;

  XT_MADD_SX2( cf1, cf0, p4 ); cf0 = cf1;
  XT_MADD_SX2( cf3, cf2, p4 ); cf1 = cf3;

  XT_MADD_SX2( cf1, cf0, p8 ); cf0 = cf1;

  XT_MADD_SX2( p, p3, cf0 ); z0 = p;

  /* Compute the cotangent for odd-numbered segments. */
  if ( AE_MOVAD32_H( ji ) < 0 )
  {
    __Pragma( "frequency_hint frequent" );
    z0 = XT_RECIP_SX2( z0 );
  }

#if SCL_TANF_ERRH != 0
  /* Suppress spurious exception flags and restore original status flags. */
  XT_AE_MOVFCRFSRV( fstate );
#endif

  /* Compute the sign adjustment term. */
  sx = XT_AE_MOVINT32X2_FROMXTFLOATX2( x0 );
  sz = AE_XOR32( sx, ji );
  sz = AE_SRLI32( sz, 31 );
  sz = AE_SLLI32( sz, 31 );

  /* Adjust the sign. */
  s = XT_AE_MOVINT32X2_FROMXTFLOATX2( z0 );
  s = AE_XOR32( s, sz );
  z0 = XT_AE_MOVXTFLOATX2_FROMINT32X2( s );

  return ( XT_HIGH_S( z0 ) );

} /* scl_tanf() */
#endif
