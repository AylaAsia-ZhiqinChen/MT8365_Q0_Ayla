/*
 *    Mediatek HiFi 4 Redistribution Version  <0.0.1>
 */

/* ------------------------------------------------------------------------ */
/* Copyright (c) 2016 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED.  */
/* These coded instructions, statements, and computer programs (“Cadence    */
/* Libraries”) are the copyrighted works of Cadence Design Systems Inc.	    */
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
/*
 * Least Mean Squares
 * C code optimized for HiFi4
 */

/* Cross-platform data type definitions. */
#include "NatureDSP_types.h"
/* Common helper macros. */
#include "common.h"
/* Basic operations for the reference code. */
#include "baseop.h"
/* Statistical functions */
#include "NatureDSP_Signal.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void,fir_blmsf,( float32_t * e, float32_t * h, const float32_t * r,
                const float32_t * x, float32_t norm, float32_t mu, int N, int M ))
#else

/*-------------------------------------------------------------------------
  Blockwise Adaptive LMS Algorithm for Real Data
  Blockwise LMS algorithm performs filtering of reference samples x[N+M-1],
  computation of error e[N] over a block of input samples r[N] and makes
  blockwise update of IR to minimize the error output.
  Algorithm includes FIR filtering, calculation of correlation between the 
  error output e[N] and reference signal x[N+M-1] and IR taps update based
  on that correlation.
NOTES: 
  1. The algorithm must be provided with the normalization factor, which is
     the power of the reference signal times N - the number of samples in a
     data block. This can be calculated using the vec_power32x32() or 
     vec_power16x16() function. In order to avoid the saturation of the 
     normalization factor, it may be biased, i.e. shifted to the right.
     If it's the case, then the adaptation coefficient must be also shifted
     to the right by the same number of bit positions.
  2. this algorithm consumes less CPU cycles per block than single 
     sample algorithm at similar convergence rate.
  3. Right selection of N depends on the change rate of impulse response:
     on static or slow varying channels convergence rate depends on
     selected mu and M, but not on N.

  Precision: 
  16x32    32-bit coefficients, 16-bit data, 16-bit output
  32x32    32-bit coefficients, 32-bit data, 32-bit output
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f         floating point

  Input:
  h[M]     impulse response, Q31 or floating point
  r[N]	   input data vector (near end). First in time value is in 
           r[0], 32bit or 16-bit, Qx or floating point
  x[N+M-1] reference data vector (far end). First in time value is in x[0],  
           32bit or 16-bit, Qx or floating point
  norm     normalization factor: power of signal multiplied by N, 32-bit  
           or floating point
           Fixed-point format for the 32x32-bit variant: Q(2*x-31-bias)
           Fixed-point format for the 32x16-bit variant: Q(2*x+1-bias)
  mu       adaptation coefficient (LMS step), Q(31-bias) or Q(15-bias)
  N        length of data block
  M        length of h
  Output:
  e[N]     estimated error, Q31,Q15 or floating point
  h[M]     updated impulse response, Q31 or floating point

  Restriction:
  x,r,h,e - should not overlap
  x,r,h,e - aligned on a 8-bytes boundary
  N,M     - multiples of 8 and >0
-------------------------------------------------------------------------*/
void fir_blmsf( float32_t * e, float32_t * h, const float32_t * r,
                const float32_t * x, float32_t norm, float32_t mu, int N, int M )
{
  float32_t b;
  int m, n;

  const xtfloatx2 *restrict pX1;
  const xtfloatx2 *restrict pX0;
  const xtfloatx2 *restrict pR;
        xtfloatx2 *restrict pE;
  const xtfloatx2 *restrict pH;
        xtfloatx2 *restrict pH_wr;
  xtfloatx2 s0, s1, s2, s3;
  xtfloatx2 s0_, s1_, s2_, s3_;
  xtfloatx2 x01, x12, x23, x34, x45, x56, x67;
  xtfloatx2 h01, h23, s01, s23;

  NASSERT(e);
  NASSERT(h);
  NASSERT(r);
  NASSERT(x);
  NASSERT_ALIGN(e, 8);
  NASSERT_ALIGN(h, 8);
  NASSERT_ALIGN(r, 8);
  NASSERT_ALIGN(x, 8);
  NASSERT(N>0 && M>0);
  NASSERT(M % 8 == 0 && N % 8 == 0);

  /* estimate error */
  pR = (const xtfloatx2*)r;
  pE = (      xtfloatx2*)e;
  __Pragma("loop_count min=1");
  for (n = 0; n < N; n += 4)
  {
    xtfloatx2 r01, r23;
    pX0 = (const xtfloatx2 *)(x+n);
    pX1 = (const xtfloatx2 *)(x+n+2);
    pH  = (const xtfloatx2 *)(h+M-2);

    s0 =s1 =s2 =s3 =(xtfloatx2)(0.0f);
    s0_=s1_=s2_=s3_=(xtfloatx2)(0.0f);
    /* preload data from x */
    XT_LSX2IP(x01, pX0, 4*sizeof(float32_t));
    XT_LSX2IP(x23, pX1, 4*sizeof(float32_t));
    x12 = XT_SEL32_LH_SX2(x01, x23);

    __Pragma("loop_count min=1");
    for (m = 0; m < (M>>2); m++)
    {
        /* load data from x */
        XT_LSX2IP(x45, pX0, 4*sizeof(float32_t));
        XT_LSX2IP(x67, pX1, 4*sizeof(float32_t));
        x34 = XT_SEL32_LH_SX2(x23, x45);
        x56 = XT_SEL32_LH_SX2(x45, x67);
        /* load data from h */
        XT_LSX2RIP(h01, pH, -2*(int)sizeof(float32_t));
        XT_LSX2RIP(h23, pH, -2*(int)sizeof(float32_t));
        /* compute convolution of 4 values */
        XT_MADD_SX2(s0, x01, h01);
        XT_MADD_SX2(s1, x12, h01);
        XT_MADD_SX2(s2, x23, h01);
        XT_MADD_SX2(s3, x34, h01);
        
        XT_MADD_SX2(s0_, x23, h23);
        XT_MADD_SX2(s1_, x34, h23);
        XT_MADD_SX2(s2_, x45, h23);
        XT_MADD_SX2(s3_, x56, h23);
        /* shift input line for the next iteration */
        x01 = x45;
        x12 = x56;
        x23 = x67;
    }
    s0 = s0 + s0_;
    s1 = s1 + s1_;
    s2 = s2 + s2_;
    s3 = s3 + s3_;
    s0_ = XT_SEL32_HL_SX2(s0, s1);
    s1_ = XT_SEL32_LH_SX2(s0, s1);
    s2_ = XT_SEL32_HL_SX2(s2, s3);
    s3_ = XT_SEL32_LH_SX2(s2, s3);
    s01 = s0_ + s1_;
    s23 = s2_ + s3_;
    /* compute and save error */
    XT_LSX2IP(r01, pR, 2*sizeof(float32_t));
    XT_LSX2IP(r23, pR, 2*sizeof(float32_t));
    s01 = XT_SUB_SX2(r01, s01);
    s23 = XT_SUB_SX2(r23, s23);
    XT_SSX2IP(s01, pE, 2*sizeof(float32_t));
    XT_SSX2IP(s23, pE, 2*sizeof(float32_t));
  }
  /* update impluse response */
  b = mu / norm;
  pH_wr = (xtfloatx2*)((uintptr_t)(h + M - 2));
  pH = pH_wr;
  __Pragma("loop_count min=1");
  for (m = 0; m < M; m += 4)
  {
    xtfloatx2 err01, err23;
    pX0 = (const xtfloatx2 *)(x+m);
    pX1 = (const xtfloatx2 *)(x+m+2);
    pE  = (      xtfloatx2 *)(e);

    s0 =s1 =s2 =s3 =(xtfloatx2)(0.0f);
    s0_=s1_=s2_=s3_=(xtfloatx2)(0.0f);
    /* preload data from x */
    XT_LSX2IP(x01, pX0, 4*sizeof(float32_t));
    XT_LSX2IP(x23, pX1, 4*sizeof(float32_t));
    x12 = XT_SEL32_LH_SX2(x01, x23);

    __Pragma("loop_count min=1");
    for (n = 0; n < (N>>2); n++)
    {
        /* load data from x */
        XT_LSX2IP(x45, pX0, 4*sizeof(float32_t));
        XT_LSX2IP(x67, pX1, 4*sizeof(float32_t));
        x34 = XT_SEL32_LH_SX2(x23, x45);
        x56 = XT_SEL32_LH_SX2(x45, x67);
        /* load data from e */
        XT_LSX2IP(err01, pE, 2*(int)sizeof(float32_t));
        XT_LSX2IP(err23, pE, 2*(int)sizeof(float32_t));
        /* compute correlation of 4 values */
        XT_MADD_SX2(s0, x01, err01);
        XT_MADD_SX2(s1, x12, err01);
        XT_MADD_SX2(s2, x23, err01);
        XT_MADD_SX2(s3, x34, err01);
        
        XT_MADD_SX2(s0_, x23, err23);
        XT_MADD_SX2(s1_, x34, err23);
        XT_MADD_SX2(s2_, x45, err23);
        XT_MADD_SX2(s3_, x56, err23);
        /* shift input line for the next iteration */
        x01 = x45;
        x12 = x56;
        x23 = x67;
    }
    s0 = s0 + s0_;
    s1 = s1 + s1_;
    s2 = s2 + s2_;
    s3 = s3 + s3_;
    s0_ = XT_SEL32_HL_SX2(s0, s1);
    s1_ = XT_SEL32_LH_SX2(s0, s1);
    s2_ = XT_SEL32_HL_SX2(s2, s3);
    s3_ = XT_SEL32_LH_SX2(s2, s3);
    s01 = s0_ + s1_;
    s23 = s2_ + s3_;
    /* update and save IR */
    XT_LSX2RIP(h01, pH, -2*(int)sizeof(float32_t));
    XT_LSX2RIP(h23, pH, -2*(int)sizeof(float32_t));
    XT_MADD_SX2(h01, (xtfloatx2)b, s01);
    XT_MADD_SX2(h23, (xtfloatx2)b, s23);
    XT_SSX2RIP(h01, pH_wr, -2*(int)sizeof(float32_t));
    XT_SSX2RIP(h23, pH_wr, -2*(int)sizeof(float32_t));
  }
} /* fir_blmsf() */
#endif
