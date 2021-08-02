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
    Real interpolating FIR Filter
    C code optimized for HiFi4
    IntegrIT, 2006-2015
 */

/* Cross-platform data type definitions. */
#include "NatureDSP_types.h"
/* Common helper macros. */
#include "common.h"
/* Basic operations for the reference code. */
#include "baseop.h"
/* Filters and transformations */
#include "NatureDSP_Signal.h"
#include "fir_interpf_Dx.h"
#if (XCHAL_HAVE_HIFI4_VFPU)

/*-------------------------------------------------------------------------
Real Interpolation FIR Filter
These functions implement a finite impulse response (FIR) filter for real-
valued samples with interpolation. The functions generate the interpolated
filtered response of the input data x and store the result in the output
vector z. The number of input samples is specified by the argument N and
the length of the output vector is D times more, where D is an interpolation
factor.

Impulse response h of M*D coefficients is stored in the polyphase form, i.e.
as a sequence of smaller filters h(d) of M coefficients for each of D
interpolation phases:
h[d*M+m] = h(d,m), where m = 0..M-1, d = 0..D-1.

The functions maintain the filter state in the structured variable state,
which must be declared and initialized before calling the function in the
same way as for the real FIR filter (see fir functions).

Input:
x[N]    input samples
N       number of input samples
state   filter state
Output:
z[N*D]  interpolated samples
state   updated filter state
Initialization macro:
See fir_init()
Domain:
Whole range
Restrictions:
N>0, M>0, D>1
N should be a multiple of 8
M should be a multiple of 4
delay line (state->d) should be aligned on 8-byte boundary
---------------------------------------------------------------------------*/
float32_t * fir_interpf_Dx(float32_t * restrict z, float32_t * restrict delay, float32_t * restrict p, const float32_t * restrict h, const float32_t * restrict x, int M, int D, int N)
{
        xtfloat   *restrict pZ;
        xtfloatx2 *restrict pX;
  const xtfloatx2 *restrict pH0;
  const xtfloatx2 *restrict pH1;
  const xtfloatx2 *restrict pDld;
        xtfloatx2 *restrict pDst;
  xtfloatx2 acc0a, acc0b, acc1a, acc1b;
  xtfloatx2 acc2a, acc2b, acc3a, acc3b;
  xtfloatx2 x01, x23;
  xtfloatx2 x32, x21, x10, x0_1, x_12, x_23, x_34;
  xtfloatx2 h01, h23;
  xtfloat   s0, s1, s2, s3;
  int n, j, m;

  NASSERT(x);
  NASSERT(z);
  NASSERT(N>0);
  NASSERT(M>0);

  /* set bounds of the delay line */
  WUR_AE_CBEGIN0((uintptr_t)(delay));
  WUR_AE_CEND0((uintptr_t)(delay + M));
  /* initialize pointers */
  pDst = (xtfloatx2 *)p;
  pX   = (xtfloatx2 *)x;

  /* Process by 4 input samples (4*D output samples) */
  __Pragma("loop_count min=1")
  for (n = 0; n < (N>>2); n++)
  {
      pH0  = (const xtfloatx2 *)(h);
      pH1  = pH0 + 1;
      pZ   = (xtfloat *)(z+(n<<2)*D);
      __Pragma("loop_count min=1")
      for (j = 0; j < D; j++)
      {
          acc0a = acc0b = acc1a = acc1b = (xtfloatx2)(0.0f);
          acc2a = acc2b = acc3a = acc3b = (xtfloatx2)(0.0f);
          /* preload input samples */
          x10 = XT_LSX2RI(pX, 0);
          x32 = XT_LSX2RI(pX, 2*sizeof(float32_t));
          x21 = XT_SEL32_LH_SX2(x32, x10);
          /* prepare for reverse loading of the delay line */
          pDld = pDst;
          AE_ADDCIRC32X2_XC(castxcc(ae_int32x2,pDld), -2*(int)sizeof(float32_t));

          /* kernel loop: compute by 4 taps for each sample */
          __Pragma("loop_count min=1")
          for (m = 0; m < (M>>2); m++)
          {
            /* load filter coefficients */
            XT_LSX2IP(h01, pH0, 4*sizeof(float32_t));
            XT_LSX2IP(h23, pH1, 4*sizeof(float32_t));
            /* load samples from the delay line */
            XT_LSX2RIC(x_12, pDld);
            XT_LSX2RIC(x_34, pDld);
            x0_1 = XT_SEL32_LH_SX2(x10, x_12);
            x_23 = XT_SEL32_LH_SX2(x_12, x_34);
            /* multiply */
            XT_MADD_SX2(acc0a, h01, x0_1);
            XT_MADD_SX2(acc1a, h01, x10);
            XT_MADD_SX2(acc2a, h01, x21);
            XT_MADD_SX2(acc3a, h01, x32);
            XT_MADD_SX2(acc0b, h23, x_23);
            XT_MADD_SX2(acc1b, h23, x_12);
            XT_MADD_SX2(acc2b, h23, x0_1);
            XT_MADD_SX2(acc3b, h23, x10 );
            /* shift whole delay line */
            x32 = x_12;
            x21 = x_23;
            x10 = x_34;
          }
          /* save computed samples */
          acc0a = acc0a + acc0b;
          acc1a = acc1a + acc1b;
          acc2a = acc2a + acc2b;
          acc3a = acc3a + acc3b;
          s0 = XT_RADD_SX2(acc0a);
          s1 = XT_RADD_SX2(acc1a);
          s2 = XT_RADD_SX2(acc2a);
          s3 = XT_RADD_SX2(acc3a);
          XT_SSXP(s0, pZ, D*sizeof(float32_t));
          XT_SSXP(s1, pZ, D*sizeof(float32_t));
          XT_SSXP(s2, pZ, D*sizeof(float32_t));
          XT_SSXP(s3, pZ, (-3*D+1)*(int)sizeof(float32_t));
      }
      /* update the delay line */
      XT_LSX2IP(x01, pX  , 2*sizeof(float32_t));
      XT_LSX2IP(x23, pX  , 2*sizeof(float32_t));
      XT_SSX2XC(x01, pDst, 2*sizeof(float32_t));
      XT_SSX2XC(x23, pDst, 2*sizeof(float32_t));
  }
  return (float32_t*)pDst;
} /* fir_interpf() */
#endif
