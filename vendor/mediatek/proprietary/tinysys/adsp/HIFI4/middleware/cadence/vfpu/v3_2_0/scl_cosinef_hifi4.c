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
#include "inv2pif_tbl.h"
#include "sinf_tbl.h"
/* sNaN/qNaN, single precision. */
#include "nanf_tbl.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(float32_t,scl_cosinef,( float32_t x ))
#else
/* If non-zero, set errno and raise floating-point exceptions on errors. */
#define SCL_COSINEF_ERRH    1

/*===========================================================================
  Scalar matematics:
  scl_cosine            Cosine    
===========================================================================*/

/*-------------------------------------------------------------------------
  Sine/Cosine 
  Fixed-point functions calculate sin(pi*x) or cos(pi*x) for numbers written 
  in Q31 or Q15 format. Return results in the same format. 
  Floating point functions compute sin(x) or cos(x)
  Two versions of functions available: regular version (vec_sine32x32, 
  vec_cosine32x32, , vec_sinef, vec_cosinef) 
  with arbitrary arguments and faster version (vec_sine32x32_fast, 
  vec_cosine32x32_fast) that 
  apply some restrictions.
  NOTE:
  1.  Scalar floating point functions are compatible with standard ANSI C
      routines and set errno and exception flags accordingly
  2.  Floating point functions limit the range of allowable input values:
      [-102940.0, 102940.0] Whenever the input value does not belong to this
      range, the result is set to NaN.

  Precision: 
  32x32  32-bit inputs, 32-bit output. Accuracy: 1700 (7.9e-7)
  f      floating point. Accuracy 2 ULP

  Input:
  x[N]  input data,Q31 or floating point
  N     length of vectors
  Output:
  y[N]  output data,Q31 or floating point

  Restriction:
  Regular versions (vec_sine32x32, vec_cosine32x32, vec_sinef, vec_cosinef):
  x,y - should not overlap

  Faster versions (vec_sine32x32_fast, vec_cosine32x32_fast):
  x,y - should not overlap
  x,y - aligned on 8-byte boundary
  N   - multiple of 2

  Scalar versions:
  ----------------
  return result in Q31 or floating point
-------------------------------------------------------------------------*/

float32_t scl_cosinef( float32_t x )
{
  /*
   * Reference C code for SCL_COSINEF_ERRH == 0:
   *
   *   float32_t x2,y,ys,yc;
   *   int sx,n,j,k,sc;
   *   sx=takesignf(x);
   *   x=sx?-x:x;
   *   // argument reduction
   *   k=(int)STDLIB_MATH(floorf)(x*inv4pif.f);
   *   n=k+1;
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
   *            y-=c[0].f*jj;
   *            y-=c[1].f*jj;
   *            y-=c[2].f*jj;
   *       t=y; y-=c[3].f*jj; t=(t-y); t-=c[3].f*jj; dx = t; 
   *       t=y; y-=c[4].f*jj; t=(t-y); t-=c[4].f*jj; dx = (dx+t); 
   *       t=y; y-=c[5].f*jj; t=(t-y); t-=c[5].f*jj; dx = (dx+t); 
   *       y=(y+dx);
   *       x=y;
   *   }
   *   // adjust signs
   *   sc = ( ((n+2)>>2) & 1 );
   *   // compute sine/cosine via minmax polynomial
   *   x2=x*x;
   *   ys=      polysinf_tbl[0].f;
   *   ys=ys*x2+polysinf_tbl[1].f;
   *   ys=ys*x2+polysinf_tbl[2].f;
   *   ys=ys*x2;
   *   ys=ys*x+x;
   *   yc=      polycosf_tbl[0].f;
   *   yc=yc*x2+polycosf_tbl[1].f;
   *   yc=yc*x2+polycosf_tbl[2].f;
   *   yc=yc*x2+1.f;
   *   // select sine/cosine
   *   y = (n&2) ? ys:yc;
   *   // apply the sign
   *   y=changesignf(y,sc);
   *   return y;
   */

  /* Polynomial coeff table pointer. */
  const xtfloat * ptbl;
  /* Input value and its absolute; output value */
  xtfloatx2 x0, x1, y0;
  /* Reducted input value and its 2nd and 3rd powers; correction term. */
  xtfloatx2 p, p2, p3, dp;
  /* Polynomial value; sine and cosine approximations. */
  xtfloatx2 g, sn, cs;
  /* Output sign; integer reprentation of output value */
  ae_int32x2 sy, y0_i;
  /* Auxiliary floating-point vars. */
  xtfloatx2 t, r;
  /* Input value segment number. */
  xtfloatx2 jf;
  ae_int32x2 ji;
  /* pi/2 splitted into 7-bit chunks. */
  xtfloatx2 c0, c1, c2, c3, c4, c5;
  /* Polynomial coeffs. */
  xtfloatx2 cf0, cf1, cf2;
  /* Select the cosine. */
  xtbool2 b_cs;

#if SCL_COSINEF_ERRH != 0
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
#if SCL_COSINEF_ERRH != 0
    errno = EDOM;
#endif
    return (qNaNf.f);
  }

  if ( xtbool2_extract_0( XT_OLT_SX2( sinf_maxval.f, XT_ABS_SX2(x0) ) ) )
  {
    __Pragma( "frequency_hint never" );
    return (qNaNf.f);
  }

  x1 = XT_ABS_SX2( x0 );

#if SCL_COSINEF_ERRH != 0
  /* Sample floating-point exception flags. */
  fstate = XT_AE_MOVVFCRFSR();
#endif

  /* Determine the pi/2-wide segment the input value belongs to. */
  jf = XT_MUL_SX2( x1, inv2pif.f );
  jf = XT_FIROUND_SX2( jf );
  ji = XT_TRUNC_SX2( jf, 0 );

  /* Determine the output sign. */
  sy = AE_ADD32( ji, AE_MOVI(1) );
  sy = AE_SRLI32( sy, 1 );
  sy = AE_SLLI32( sy, 31 );
  
  /*
   * Range reduction.
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
  p2 = XT_MUL_SX2( p, p );
  p3 = XT_MUL_SX2( p2, p );

#if SCL_COSINEF_ERRH != 0
  /* Suppress spurious exception flags and restore original status flags. */
  XT_AE_MOVFCRFSRV( fstate );
#endif

  /*
   * Polynomial approximation: g(p) = sin(p)/p or g(p) = cos(p).
   */

  /* Use cosine approximation for an even-numbered segment. */
  b_cs = AE_LE32( AE_ZERO32(), AE_SLLI32( ji, 31 ) );

  /* Select the coeff table. */
  ptbl = (xtfloat*)( xtbool2_extract_0( b_cs ) ? polycosf_tbl : polysinf_tbl );

  /* Common part of polynomial evaluation. */
  cf0 = ptbl[0];                             g = cf0;
  cf1 = ptbl[1]; XT_MADDN_SX2( cf1, g, p2 ); g = cf1;
  cf2 = ptbl[2]; XT_MADDN_SX2( cf2, g, p2 ); g = cf2;

  /* Finalize evaluation for sine and cosine. */
  sn = p                ; XT_MADDN_SX2( sn, g, p3 );
  cs = (xtfloatx2)(1.0f); XT_MADDN_SX2(cs, g, p2);

  /* Select sine or cosine approximation. */
  y0 = sn; XT_MOVT_SX2( y0, cs, b_cs );

  /* Adjust the output sign. */
  y0_i = XT_AE_MOVINT32X2_FROMXTFLOATX2( y0 );
  y0_i = AE_XOR32( sy, y0_i );
  y0 = XT_AE_MOVXTFLOATX2_FROMINT32X2( y0_i );

  return ( XT_HIGH_S( y0 ) );

} /* scl_cosinef() */
#endif
