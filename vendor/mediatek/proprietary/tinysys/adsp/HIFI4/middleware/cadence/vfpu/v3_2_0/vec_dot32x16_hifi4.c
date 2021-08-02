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
#include "NatureDSP_Signal.h"
#include "common.h"


/*===========================================================================
  Vector matematics:
  vec_dot              Vector Dot Product
===========================================================================*/

/*-------------------------------------------------------------------------
  Vector Dot product
  Two versions of routines are available: regular versions (without suffix _fast)
  that work with arbitrary arguments, faster versions 
  (with suffix _fast) apply some restrictions.

  Precision: 
  64x32  64x32-bit data, 64-bit output (fractional multiply Q63xQ31->Q63)
  64x64  64x64-bit data, 64-bit output (fractional multiply Q63xQ63->Q63)
  64x64i 64x64-bit data, 64-bit output (low 64 bit of integer multiply)
  32x32  32x32-bit data, 64-bit output
  32x16  32x16-bit data, 64-bit output
  16x16  16x16-bit data, 64-bit output for regular version and 32-bit for 
                        fast version
  f      single precision floating point

  Input:
  x[N]  input data, Q31 or floating point
  y[N]  input data, Q31, Q15, or floating point
  N	length of vectors
  Returns: dot product of all data pairs, Q31 or Q63

  Restrictions:
  Regular versions (without suffix _fast):
  None
  Faster versions (with suffix _fast):
  x,y - aligned on 8-byte boundary
  N   - multiple of 4

  vec_dot16x16_fast utilizes 32-bit saturating accumulator, so, input data 
  should be scaled properly to avoid erroneous results
-------------------------------------------------------------------------*/
int64_t vec_dot32x16 (const int32_t * restrict x, const int16_t * restrict y, int N)
{
    static const int16_t ALIGN(8) seq[]={0,1,2,3};
    int n;
    ae_int32x2 x0,x1,x2,x3;
    ae_int16x4 y0;
    ae_f64 vaf;
    ae_int64 vai;
    const ae_f32x2 *    restrict  px = (const ae_f32x2 *)   x;
    const ae_int16x4 *  restrict  py = (const ae_int16x4 *) y;
    ae_valign y_align;
    ae_int16x4 m0,m1;
    xtbool4 b;
    if(N<=0) return 0;

    m1=AE_L16X4_I((const ae_int16x4*)seq,0);
    m0=AE_MOVDA16(N&3);
    b=AE_LT16(m1,m0);

    vaf = AE_ZERO64();
    if (((uintptr_t)px)&4)  // not aligned on 8-byte boundary
    {
        AE_L32_IP(x3, castxcc(ae_int32,px), 4);
        y_align = AE_LA64_PP(py);
        for (n=0; n<(N&~3); n+=4)
        {
            AE_L32X2_IP(x2, px, 8);
            x0=AE_SEL32_LH(x3,x2);
            AE_L32X2_IP(x3, px, 8);
            x1=AE_SEL32_LH(x2,x3);
            AE_LA16X4_IP(y0, y_align, py);
            AE_MULAAAAFQ32X16(vaf,x0,x1,y0);
        }
        // process tail: mask unneeded values from y0
            AE_L32X2_IP(x2, px, 8);
            x0=AE_SEL32_LH(x3,x2);
            AE_L32X2_IP(x3, px, 8);
            x1=AE_SEL32_LH(x2,x3);
        AE_LA16X4_IP(y0, y_align, py);
        AE_MOVF16X4(y0,AE_ZERO16(),b);
        AE_MULAAAAFQ32X16(vaf,x0,x1,y0);
    }
    else
    {                       // x is aligned on 8-byte boundary
        y_align = AE_LA64_PP(py);
        for (n=0; n<(N&~3); n+=4)
        {
            AE_L32X2_IP(x0, px, 8);
            AE_L32X2_IP(x1, px, 8);
            AE_LA16X4_IP(y0, y_align, py);
            AE_MULAAAAFQ32X16(vaf,x0,x1,y0);
        }
        // process tail: mask unneeded values from y0
        AE_L32X2_IP(x0, px, 8);
        AE_L32X2_IP(x1, px, 8);
        AE_LA16X4_IP(y0, y_align, py);
        AE_MOVF16X4(y0,AE_ZERO16(),b);
        AE_MULAAAAFQ32X16(vaf,x0,x1,y0);
    }
    vai = AE_SRAI64(vaf, 16);
    return_int64(vai);
}
