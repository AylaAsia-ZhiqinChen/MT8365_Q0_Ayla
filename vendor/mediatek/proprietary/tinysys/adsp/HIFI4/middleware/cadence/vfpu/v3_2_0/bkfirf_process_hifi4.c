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
  NatureDSP Signal Processing Library. FIR part
    Real block FIR filter, floating point
    C code optimized for HiFi4
  IntegrIT, 2006-2015
*/

/*-------------------------------------------------------------------------
  Real FIR filter.
  Computes a real FIR filter (direct-form) using IR stored in vector h. 
  The real data input is stored in vector x. The filter output result is 
  stored in vector y. The filter calculates N output samples using M 
  coefficients and requires last M-1 samples in the delay line which is 
  updated in circular manner for each new sample.
  User has an option to set IR externally or copy from original location 
  (i.e. from the slower constant memory). In the first case, user is 
  responsible for right alignment, ordering and zero padding of filter 
  coefficients - usually array is composed from zeroes (left padding), 
  reverted IR and right zero padding.

  Precision: 
  24x24p   use 24-bit data packing for internal delay line buffer
           and internal coefficients storage
  32x16    32-bit data, 16-bit coefficients, 32-bit outputs
  32x32    32-bit data, 32-bit coefficients, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f        floating point

  Input:
  x[N]        input samples, Q31, Q15, floating point
  h[M]        filter coefficients in normal order, Q31, Q15, floating point
  N           length of sample block, should be a multiple of 4
  M           length of filter, should be a multiple of 4
  extIR       if zero, IR is copied from original location, otherwise 
              not but user should keep alignment, order of coefficients 
              and zero padding requirements shown below
  Output:
  y[N]      - input samples, Q31, Q15, floating point

  Alignment, ordering and zero padding for external IR  (extIR!=0)
  ------------------------+----------+--------------+--------------+----------------
  Function	              |Alignment,|Left zero     |   Coefficient| Right zero 
                          | bytes    |padding, bytes|   order      | padding, bytes
  ------------------------+----------+--------------+--------------+----------------
  bkfir24x24p_init        |     8    |((-M&4)+1)*3  |  inverted    |  7
  bkfir32x16_init (M>32)  |     8    |     10       |  inverted    |  6
  bkfir32x16_init (M<=32) |     8    |      2       |  inverted    |  6
  bkfir32x32_init         |     8    |      4       |  inverted    |  12
  bkfir32x32ep_init       |     8    |      4       |  inverted    |  12
  bkfirf_init             |     8    |      0       |  direct      |  0
  ------------------------+----------+--------------+--------------+----------------

  Restrictions:
  x,y should not be overlapping
  x,h - aligned on a 8-bytes boundary
  N,M - multiples of 4 
-------------------------------------------------------------------------*/
/* Portable data types. */
#include "NatureDSP_types.h"
/* Signal Processing Library API. */
#include "NatureDSP_Signal.h"
/* Common utility and macros declarations. */
#include "common.h"
#include "bkfirf.h"

#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void,bkfirf_process,( bkfirf_handle_t _bkfir, 
                         float32_t * restrict  y,
                   const float32_t * restrict  x, int N ))
#else

/* Circular load with using CBEGIN1/CEND1 */
#define XT_LSX2XC1(reg, addr, offs)\
{\
    ae_int32x2 t;\
    AE_L32X2_XC1(t, addr, offs);\
    reg = XT_AE_MOVXTFLOATX2_FROMINT32X2(t);\
}

/* process block of samples */
void bkfirf_process( bkfirf_handle_t _bkfir, 
                         float32_t * restrict  y,
                   const float32_t * restrict  x, int N )
{
    bkfirf_t *state;
    const xtfloatx2* restrict pX;
          xtfloatx2* restrict pD;
    const xtfloatx2* restrict pH;
          xtfloatx2*          pZ;
    xtfloatx2 h01, h23;
    xtfloatx2 x32, x21, x10, x0_1, x_12, x_23, x_34;
    xtfloatx2 s0, s1;
    xtfloatx2 t0, t1, t2, t3;
    xtfloatx2 acc0, acc1, acc2, acc3;
    xtfloatx2 acc0_, acc1_, acc2_, acc3_;
    ae_valign yalign;
    int n, m, M;

    NASSERT(x);
    NASSERT(y);
    NASSERT(_bkfir);

    if(N<=0) return;
    state=(bkfirf_t*)_bkfir;
    M=state->M;
    NASSERT(state->h);
    NASSERT(state->d);
    NASSERT(state->p);
    NASSERT_ALIGN(state->h, 8);
    NASSERT_ALIGN(state->d, 8);
    NASSERT_ALIGN(state->p, 8);
    NASSERT_ALIGN(x, 8);
    NASSERT(M>0);
    NASSERT(N%4==0);
    NASSERT((state->M%4)==0);

    pD = (      xtfloatx2 *)state->p;
    pH = (const xtfloatx2 *)state->h;
    pX = (const xtfloatx2 *)(x+2);
    pZ = (      xtfloatx2 *)(y);

    WUR_AE_CBEGIN0((uintptr_t)(state->d));
    WUR_AE_CEND0((uintptr_t)(state->d + M));
    WUR_AE_CBEGIN1((uintptr_t)(state->h));
    WUR_AE_CEND1((uintptr_t)(state->h + M));
    yalign=AE_ZALIGN64();
    
    /* process by 4 samples */
    __Pragma("loop_count min=1")
    for (n = 0; n < (N>>2); n++)
    {
      /* load input samples */
      XT_LSX2RIP(x32, pX, -2*(int)sizeof(float32_t));
      XT_LSX2RIP(x10, pX, 6*sizeof(float32_t));
      x21 = XT_SEL32_LH_SX2(x32, x10);
      s0 = XT_SEL32_LH_SX2(x10, x10);
      s1 = XT_SEL32_LH_SX2(x32, x32);

      acc0 = acc0_ = acc1 = acc1_ = 
        acc2 = acc2_ = acc3 = acc3_ = (xtfloatx2)(0.0f);
      AE_ADDCIRC32X2_XC(castxcc(ae_int32x2,pD), -2*(int)sizeof(float32_t));
      
      /* main loop: process by 4 taps for 4 samples */
      __Pragma("loop_count min=1")
      for (m = 0; m < (M>>2); m++)
      {
        /* load samples from the delay line */
        XT_LSX2RIC(x_12, pD);
        XT_LSX2RIC(x_34, pD);
        x0_1 = XT_SEL32_LH_SX2(x10, x_12);
        x_23  = XT_SEL32_LH_SX2(x_12, x_34);
        /* load filter coefficients */
        XT_LSX2XC1(h01, castxcc(ae_int32x2,pH), 2*sizeof(float32_t));
        XT_LSX2XC1(h23, castxcc(ae_int32x2,pH), 2*sizeof(float32_t));
        /* SIMD multiply */
        XT_MADD_SX2(acc3, x32,  h01);
        XT_MADD_SX2(acc2, x21,  h01);
        XT_MADD_SX2(acc1, x10,  h01);
        XT_MADD_SX2(acc0, x0_1, h01);

        XT_MADD_SX2(acc3_, x10,  h23);
        XT_MADD_SX2(acc2_, x0_1, h23);
        XT_MADD_SX2(acc1_, x_12, h23);
        XT_MADD_SX2(acc0_, x_23, h23);
        /* shift delay line for the next iteration */
        x32 = x_12;
        x21 = x_23;
        x10 = x_34;
      }
      AE_ADDCIRC32X2_XC(castxcc(ae_int32x2,pD), 2*sizeof(float32_t));

      acc0 = acc0 + acc0_;
      acc1 = acc1 + acc1_;
      acc2 = acc2 + acc2_;
      acc3 = acc3 + acc3_;
      t0 = XT_SEL32_HL_SX2(acc0, acc1);
      t1 = XT_SEL32_LH_SX2(acc0, acc1);
      t2 = XT_SEL32_HL_SX2(acc2, acc3);
      t3 = XT_SEL32_LH_SX2(acc2, acc3);
      acc0 = t0 + t1;
      acc1 = t2 + t3;
      /* store filtered samples */
      XT_SASX2IP(acc0, yalign, pZ);
      XT_SASX2IP(acc1, yalign, pZ);
      /* update the delay line */
      XT_SSX2XC(s0, pD, 2*sizeof(float32_t));
      XT_SSX2XC(s1, pD, 2*sizeof(float32_t));
    }
    AE_SA64POS_FP(yalign,pZ);

    state->p = (float32_t*)pD;
}/* bkfirf_process() */
#endif
