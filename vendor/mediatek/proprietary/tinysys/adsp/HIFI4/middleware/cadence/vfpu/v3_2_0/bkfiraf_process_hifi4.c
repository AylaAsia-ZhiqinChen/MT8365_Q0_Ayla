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
  Computes a real FIR filter (direct-form) using IR stored in vector h. The 
  real data input is stored in vector x. The filter output result is stored 
  in vector y. The filter calculates N output samples using M coefficients 
  and requires last M+N-1 samples in the delay line.
  These functions implement FIR filter described in previous chapter with no 
  limitation on size of data block, alignment and length of impulse response 
  for the cost of performance.
  NOTE: user application is not responsible for management of delay lines

  Precision: 
  32x16    32-bit data, 16-bit coefficients, 32-bit outputs
  32x32    32-bit data, 32-bit coefficients, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f        floating point

  Input:
  x[N]      - input samples, Q31, floating point
  h[M]      - filter coefficients in normal order, Q31, Q15, floating point
  N         - length of sample block
  M         - length of filter
  Output:
  y[N]      - input samples, Q31, floating point 

  Restrictions:
  x,y should not be overlapping
-------------------------------------------------------------------------*/
/* Portable data types. */
#include "NatureDSP_types.h"
/* Signal Processing Library API. */
#include "NatureDSP_Signal.h"
/* Common utility and macros declarations. */
#include "common.h"
#include "bkfiraf.h"

#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void,bkfiraf_process,( bkfiraf_handle_t _bkfir, 
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
void bkfiraf_process( bkfiraf_handle_t _bkfir, 
                         float32_t * restrict  y,
                   const float32_t * restrict  x, int N )
{
    bkfiraf_t *state;
    const xtfloatx2* restrict pX;
          xtfloatx2*          pY;
    const xtfloatx2* restrict pH0;
    const xtfloatx2* restrict pH1;
    const xtfloatx2* restrict pD_ld;
          xtfloatx2* restrict pD_st;
    xtfloatx2 h01, h23;
    xtfloatx2 x32, x21, x10, x0_1, x_12, x_23, x_34;
    xtfloatx2 x01, x23;
    xtfloatx2 s01, s23;
    xtfloatx2 t0, t1, t2, t3;
    xtfloatx2 acc0, acc1, acc2, acc3;
    xtfloatx2 acc0_, acc1_, acc2_, acc3_;
    xtfloat X0, A;
    ae_valign xalign, yalign;
    ae_valign delayalign;
    int n, m, M;

    NASSERT(x);
    NASSERT(y);
    NASSERT(_bkfir);

    if(N<=0) return;
    state=(bkfiraf_t*)_bkfir;
    M=state->M;
    NASSERT(M>0);
    NASSERT(state->h);
    NASSERT(state->d);
    NASSERT(state->p);
    NASSERT_ALIGN(state->h,8);
    NASSERT_ALIGN(state->d,8);
    NASSERT((state->M%4)==0);
    
    pX = (const xtfloatx2 *)(x);
    pY = (      xtfloatx2 *)(y);
    /* set bounds of the delay line */
    WUR_AE_CBEGIN0((uintptr_t)(state->d));
    WUR_AE_CEND0((uintptr_t)(state->d + M));
    /* set bounds of the IR coeffs */
    WUR_AE_CBEGIN1((uintptr_t)(state->h));
    WUR_AE_CEND1((uintptr_t)(state->h + M));

    pD_st = (   xtfloatx2 *)state->p;
    pH0 = (const xtfloatx2 *)state->h;
    pH1 = pH0+1;
    /* If pointer into the delay line is unaligned, *
     * compute 1 sample                             */
    if ((intptr_t)pD_st & 0x4)
    {
        /* load input sample */
        XT_LSIP(X0, castxcc(xtfloat,pX), sizeof(float32_t));
        /* prepare for loading from unaligned delay line */
        pD_ld = pD_st;
        AE_ADDCIRC32X2_XC(castxcc(ae_int32x2,pD_ld), -(int)sizeof(float32_t));
        XT_LASX2NEGPC(delayalign, pD_ld);
        /* save the new sample into the delay line */
        XT_SSXC(X0, castxcc(xtfloat,pD_st), sizeof(float32_t));
        x10 = X0;
        acc0 = acc0_ = (xtfloatx2)0.0f;

        __Pragma("loop_count min=1")
        for (m = 0; m < (M>>2); m++)
        {
            /* load samples from the delay line */
            XT_LASX2RIC(x_12, delayalign, pD_ld);
            XT_LASX2RIC(x_34, delayalign, pD_ld);
            x0_1 = XT_SEL32_LH_SX2(x10, x_12);
            x_23 = XT_SEL32_LH_SX2(x_12, x_34);
            /* load filter coefficients */
            XT_LSX2XC1(h01, castxcc(ae_int32x2,pH0), 4*sizeof(float32_t));
            XT_LSX2XC1(h23, castxcc(ae_int32x2,pH1), 4*sizeof(float32_t));
            /* SIMD multiply */
            XT_MADD_SX2(acc0 , x0_1, h01);
            XT_MADD_SX2(acc0_, x_23, h23);
            /* shift delay line for the next iteration */
            x10 = x_34;
        }
        acc0 = acc0 + acc0_;
        A = XT_RADD_SX2(acc0);
        XT_SSIP(A, castxcc(xtfloat,pY), sizeof(float32_t));
        /* correct input vector length */
        N--;
    }
    
    /* Main loop: compute by 4 samples */
    xalign = XT_LASX2PP(pX);
    yalign = AE_ZALIGN64();
    for (n = 0; n < (N>>2); n++)
    {
        /* load input samples */
        XT_LASX2IP(x01, xalign, pX);
        XT_LASX2IP(x23, xalign, pX);
        x32 = XT_SEL32_LH_SX2(x23, x23);
        x21 = XT_SEL32_HL_SX2(x23, x01);
        x10 = XT_SEL32_LH_SX2(x01, x01);
        s01 = x01;
        s23 = x23;

        acc0 = acc0_ = acc1 = acc1_ = 
          acc2 = acc2_ = acc3 = acc3_ = (xtfloatx2)0.0f;

        pD_ld = pD_st;
        AE_ADDCIRC32X2_XC(castxcc(ae_int32x2,pD_ld), -2*(int)sizeof(float32_t));
        
        /* main loop: process by 4 taps for 4 samples */
        __Pragma("loop_count min=1")
        for (m = 0; m < (M>>2); m++)
        {
            /* load samples from the delay line */
            XT_LSX2RIC(x_12, pD_ld);
            XT_LSX2RIC(x_34, pD_ld);
            x0_1 = XT_SEL32_LH_SX2(x10, x_12);
            x_23 = XT_SEL32_LH_SX2(x_12, x_34);
            /* load filter coefficients */
            XT_LSX2XC1(h01, castxcc(ae_int32x2,pH0), 4*sizeof(float32_t));
            XT_LSX2XC1(h23, castxcc(ae_int32x2,pH1), 4*sizeof(float32_t));
            /* SIMD multiply */
            XT_MADD_SX2(acc0, x0_1, h01);
            XT_MADD_SX2(acc1, x10,  h01);
            XT_MADD_SX2(acc2, x21,  h01);
            XT_MADD_SX2(acc3, x32,  h01);

            XT_MADD_SX2(acc0_, x_23, h23);
            XT_MADD_SX2(acc1_, x_12, h23);
            XT_MADD_SX2(acc2_, x0_1, h23);
            XT_MADD_SX2(acc3_, x10,  h23);

            /* shift input line for the next iteration */
            x32 = x_12;
            x21 = x_23;
            x10 = x_34;
        }
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
        /* save filtered samples */
        XT_SASX2IP(acc0, yalign, pY);
        XT_SASX2IP(acc1, yalign, pY);
        /* update the delay line */
        XT_SSX2XC(s01, pD_st, 2*sizeof(float32_t));
        XT_SSX2XC(s23, pD_st, 2*sizeof(float32_t));
    }
    AE_SA64POS_FP(yalign, pY);

    /* Compute remaining samples */
    for (n = N&(~3); n < N; n++)
    {
        XT_LSIP(X0, castxcc(xtfloat,pX), sizeof(float32_t));
        pD_ld = pD_st;
        AE_ADDCIRC32X2_XC(castxcc(ae_int32x2,pD_ld), -(int)sizeof(float32_t));
        XT_LASX2NEGPC(delayalign, pD_ld);

        XT_SSXC(X0, castxcc(xtfloat,pD_st), sizeof(float32_t));
        x10 = X0;
        acc0 = acc0_ = (xtfloatx2)0.0f;

        __Pragma("loop_count min=1")
        for (m = 0; m < (M>>2); m++)
        {
            /* load samples from the delay line */
            XT_LASX2RIC(x_12, delayalign, pD_ld);
            XT_LASX2RIC(x_34, delayalign, pD_ld);
            x0_1 = XT_SEL32_LH_SX2(x10, x_12);
            x_23 = XT_SEL32_LH_SX2(x_12, x_34);
            XT_LSX2XC1(h01, castxcc(ae_int32x2,pH0), 4*sizeof(float32_t));
            XT_LSX2XC1(h23, castxcc(ae_int32x2,pH1), 4*sizeof(float32_t));
            /* load filter coefficients */
            XT_MADD_SX2(acc0 , x0_1, h01);
            XT_MADD_SX2(acc0_, x_23, h23);
            /* shift input line for the next iteration */
            x10 = x_34;
        }
        acc0 = acc0 + acc0_;
        A = XT_RADD_SX2(acc0);
        XT_SSIP(A, castxcc(xtfloat,pY), sizeof(float32_t));
    }

    state->p = (float32_t*)pD_st;
}/* bkfiraf_process() */
#endif
