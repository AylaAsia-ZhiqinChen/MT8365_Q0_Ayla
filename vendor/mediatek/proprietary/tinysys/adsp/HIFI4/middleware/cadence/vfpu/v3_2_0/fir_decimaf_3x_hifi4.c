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
    Real FIR Filter with decimation (3x)
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
#include "fir_decimaf_3x.h"
#if (XCHAL_HAVE_HIFI4_VFPU)

/*
    3x decimator:
    Input/output:
    delay[M] - circular delay line
    Input:
    p        - pointer to delay line
    x[N*3]   - input signal
    h[M]     - impulse response
    N        - number of output samples
    Output:
    z[N]     - output samples
    Restrictions:
    N>0, M>0
    N should be a multiple of 8
    M should be a multiple of 4
    delay should be aligned on 8 byte boundary

    Returns:
    updated pointer to delay line
*/
float32_t * fir_decimaf_3x(float32_t * restrict z, float32_t * restrict delay, float32_t * restrict p, const float32_t * restrict h, const float32_t * restrict x, int M, int N)
{
          xtfloatx2 *restrict pz;
          xtfloatx2 *pp;
    const xtfloatx2 *pd;
    const xtfloatx2 *restrict ph0;
    const xtfloatx2 *restrict ph1;
    const xtfloatx2 *restrict px;
    xtfloatx2 X98, X76, X65, X54, X43, X32, X10, X0_1, X_12, X_23, X_34,
              X01, X23, X45, X67, X89, X1011;
    xtfloatx2 H01, H23;
    xtfloatx2 Y00, Y01, Y30, Y31, Y60, Y61, Y90, Y91;
    xtfloatx2 Y03, Y69;
    ae_valign al_z;
    int m, n;
    
    NASSERT(x);
    NASSERT(z);
    NASSERT(h);
    NASSERT(delay);
    NASSERT(p);
    NASSERT_ALIGN(delay,8);
    NASSERT(N>0);
    NASSERT(M>0);
    NASSERT(M%4==0);

    /* set circular buffer boundaries */
    WUR_AE_CBEGIN0( (uintptr_t)( delay + 0 ) );
    WUR_AE_CEND0  ( (uintptr_t)( delay + M ) );
    /* initialize pointers */
    pz = (xtfloatx2 *)(z);
    pp = (xtfloatx2 *)(p);
    px = (xtfloatx2 *)(x);
    al_z = AE_ZALIGN64();
    /* compute by 12 input samples (4 output samples) */
    __Pragma("loop_count min=1")
    for (n = 0; n < (N>>2); n++)
    {
        Y00 = Y01 = Y30 = Y31 =
          Y60 = Y61 = Y90 = Y91 = (xtfloatx2)(0.0f);
        ph0 = (const xtfloatx2*)h;
        ph1 = ph0 + 1;
        /* load input samples */
        X10 = XT_LSX2RI(px, 0);
        X32 = XT_LSX2RI(px, 2*sizeof(float32_t));
        X54 = XT_LSX2RI(px, 4*sizeof(float32_t));
        X76 = XT_LSX2RI(px, 6*sizeof(float32_t));
        X98 = XT_LSX2RI(px, 8*sizeof(float32_t));

        /* prepare for reverse loading of the delay line */
        pd = (const xtfloatx2*)(pp);
        AE_ADDCIRC32X2_XC(castxcc(ae_int32x2,pd), -2*(int)sizeof(float32_t));

        /* compute by 4 taps for 4 samples */
        __Pragma("loop_count min=1")
        for (m = 0; m < (M>>2); m++)
        {
            /* load filter coefficients */
            XT_LSX2IP(H01, ph0, 4*sizeof(float32_t));
            XT_LSX2IP(H23, ph1, 4*sizeof(float32_t));
            /* load delay line */
            XT_LSX2RIC(X_12, pd);
            XT_LSX2RIC(X_34, pd);
            X0_1 = XT_SEL32_LH_SX2(X10, X_12);
            X_23 = XT_SEL32_LH_SX2(X_12, X_34);
            XT_MADD_SX2(Y00, H01, X0_1);
            XT_MADD_SX2(Y01, H23, X_23);
            XT_MADD_SX2(Y30, H01, X32);
            XT_MADD_SX2(Y31, H23, X10);
            X65 = XT_SEL32_LH_SX2(X76, X54);
            X43 = XT_SEL32_LH_SX2(X54, X32);
            XT_MADD_SX2(Y60, H01, X65);
            XT_MADD_SX2(Y61, H23, X43);
            XT_MADD_SX2(Y90, H01, X98);
            XT_MADD_SX2(Y91, H23, X76);
            /* shift the delay line for the next iteration */
            X98 = X54;
            X76 = X32;
            X54 = X10;
            X32 = X_12;
            X10 = X_34;
        }
        /* save computed samples */
        Y00 = Y00 + Y01;
        Y30 = Y30 + Y31;
        Y60 = Y60 + Y61;
        Y90 = Y90 + Y91;
        Y01 = XT_SEL32_LH_SX2(Y00, Y30);
        Y31 = XT_SEL32_HL_SX2(Y00, Y30);
        Y61 = XT_SEL32_LH_SX2(Y60, Y90);
        Y91 = XT_SEL32_HL_SX2(Y60, Y90);
        Y03 = Y01 + Y31;
        Y69 = Y61 + Y91;
        XT_SASX2IP(Y03, al_z, pz);
        XT_SASX2IP(Y69, al_z, pz);
        /* update delay line*/
        XT_LSX2IP(X01, px, 2*sizeof(float32_t));
        XT_LSX2IP(X23, px, 2*sizeof(float32_t));
        XT_LSX2IP(X45, px, 2*sizeof(float32_t));
        XT_LSX2IP(X67, px, 2*sizeof(float32_t));
        XT_LSX2IP(X89, px, 2*sizeof(float32_t));
        XT_LSX2IP(X1011, px, 2*sizeof(float32_t));
        XT_SSX2XC(X01, pp, 2*sizeof(float32_t));
        XT_SSX2XC(X23, pp, 2*sizeof(float32_t));
        XT_SSX2XC(X45, pp, 2*sizeof(float32_t));
        XT_SSX2XC(X67, pp, 2*sizeof(float32_t));
        XT_SSX2XC(X89, pp, 2*sizeof(float32_t));
        XT_SSX2XC(X1011, pp, 2*sizeof(float32_t));
    }
    XT_SASX2POSFP(al_z,pz);
    return (float32_t*)pp;
}
#endif
