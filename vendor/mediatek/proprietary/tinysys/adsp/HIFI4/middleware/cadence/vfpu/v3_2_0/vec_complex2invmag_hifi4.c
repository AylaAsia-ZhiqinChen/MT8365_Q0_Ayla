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
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void,vec_complex2invmag,(float32_t  * restrict y, const complex_float  * restrict x, int N))
#else
/*===========================================================================
  Vector matematics:
  vec_complex2invmag     complex magnitude (reciprocal)
===========================================================================*/
/*-------------------------------------------------------------------------
  Complex magnitude
  routines compute complex magnitude or its reciprocal

  Precision: 
  f     single precision floating point

  Output:
  y[N]  output data
  Input:
  x[N]  input complex data
  N     length of vector

  Restriction:
  none
-------------------------------------------------------------------------*/
#define sz_i32  (int)sizeof(int32_t)
#define sz_f32  (int)sizeof(float32_t)
void       vec_complex2invmag (float32_t  * restrict y, const complex_float  * restrict x, int N)
{
  /*
  * union ufloat32uint32 R, I, T, X, Z, TMP, U0, U1;
  * float32_t mnt_re, mnt_im;
  * T.u = 0x7f800000;
  * TMP.u = 0x7f000000;
  * 
  * R.f = fabsf( crealf(x) );
  * I.f = fabsf( cimagf(x) );
  * 
  * if (isnan(R.f) || isnan(I.f)) return NAN;
  * if (isinf(R.f) || isinf(I.f)) return 0;
  * 
  * 
  * U0.u = R.u&T.u;
  * U1.u = I.u&T.u;
  * X.u = MAX(U0.u, U1.u);
  * T.u = (T.u - X.u );
  * T.f = fminf(T.f, TMP.f); 
  * mnt_re = R.f*T.f;
  * mnt_im = I.f*T.f;
  * Z.f = 1.f / sqrtf(mnt_re*mnt_re + mnt_im*mnt_im);
  * Z.f = Z.f*T.f;
  * return Z.f;
  */
  /* Current block index; overall number of blocks; number of values in the current block */
  int blkIx,blkNum,blkLen;
  /* Block size, blkLen <= blkSize */
  const int blkSize = MAX_ALLOCA_SZ/(2*sz_f32);
  /* Allocate a fixed-size scratch area on the stack. */
  float32_t ALIGN(8) scr[2*blkSize];
  const xtfloatx2 * restrict X = (const xtfloatx2 *)x;
  const xtfloatx2 *          S_rd;
        xtfloatx2 * restrict S_wr;
        xtfloatx2 * restrict Y = (xtfloatx2 *)y;
  int n, _N;
  
  xtfloatx2 x0, x1, y0, xre, xim, I0;
  xtfloat l;
  ae_int32x2 t0, t1, nsa, u0, u1;
  ae_int32x2 e0, e1;
  ae_int32x2 nsa0;
  xtbool2 b0, b1;
  ae_valign Y_va;
  if (N <= 0) return;
  Y_va = AE_ZALIGN64();
  I0 = XT_AE_MOVXTFLOATX2_FROMINT32X2(0x7f800000);
  /*
  * Data are processed in blocks of scratch area size. Further, the algorithm
  * implementation is splitted in order to feed the optimizing compiler with a
  * few loops of managable size.
  */
  _N = N&(~1);
  blkNum = (_N + blkSize - 1) / blkSize;
  for (blkIx = 0; blkIx<blkNum; blkIx++)
  {
    blkLen = XT_MIN(_N - blkIx*blkSize, blkSize);
    {
      S_wr = (xtfloatx2*)scr;

      X = (xtfloatx2*)((uintptr_t)x + 2 * blkIx*blkSize*sz_f32);
      for (n = 0; n<(blkLen >> 1); n++)
      {
        XT_LSX2IP(x0, X, sizeof(complex_float));
        XT_LSX2IP(x1, X, sizeof(complex_float));
        x0 = XT_ABS_SX2(x0);
        x1 = XT_ABS_SX2(x1);

        xre = XT_SEL32_HH_SX2(x0, x1);
        xim = XT_SEL32_LL_SX2(x0, x1);

        t0 = XT_AE_MOVINT32X2_FROMXTFLOATX2(xre);
        t1 = XT_AE_MOVINT32X2_FROMXTFLOATX2(xim);

        nsa = AE_MAX32(t0, t1);
        nsa = AE_SRLI32(nsa, 23);
        nsa = AE_SUB32(nsa, 127);

        nsa = AE_MIN32(nsa, 127);

        e0 = AE_SUB32(127, nsa);
        b0 = AE_EQ32(e0, 0);
        nsa0 = AE_SLLI32(e0, 23);
        AE_MOVT32X2(nsa0, 0x00400000, b0);
        y0 = XT_AE_MOVXTFLOATX2_FROMINT32X2(nsa0);
        xre = XT_MUL_SX2(xre, y0);
        xim = XT_MUL_SX2(xim, y0);

        x0 = XT_MUL_SX2(xre, xre);
        x1 = XT_MUL_SX2(xim, xim);
        x0 = XT_ADD_SX2(x0, x1);
        x1 = XT_AE_MOVXTFLOATX2_FROMINT32X2(nsa0);
        XT_SSX2IP(x0, S_wr, 2 * sz_f32);
        XT_SSX2IP(x1, S_wr, 2 * sz_f32);
      }
    }
    __Pragma("no_reorder");
    {
      S_rd = (xtfloatx2*)scr;
      Y = (xtfloatx2*)((uintptr_t)y + blkIx*blkSize*sz_f32);
      Y_va = AE_ZALIGN64();
      for (n = 0; n<(blkLen >> 1); n++)
      {
        XT_LSX2IP(x0, S_rd, 2 * sz_f32);
        XT_LSX2IP(x1, S_rd, 2 * sz_f32);
        y0 = XT_RSQRT_SX2(x0);
        b0 = XT_OEQ_SX2(x0, 0.0f);
        b1 = XT_OEQ_SX2(x0, I0);
        t0 = XT_AE_MOVINT32X2_FROMXTFLOATX2(y0);
        AE_MOVT32X2(t0, 0x7f800000, b0);
        AE_MOVT32X2(t0, 0x0, b1);
        y0 = XT_AE_MOVXTFLOATX2_FROMINT32X2(t0);
        y0 = XT_MUL_SX2(y0, x1);
        XT_SASX2IP(y0, Y_va, Y);
      }
      XT_SASX2POSFP(Y_va, Y); 
    }
  }
  if (N & 1)
  {
    XT_LSX2IP(x0, X, sizeof(complex_float));
    x0 = XT_ABS_SX2(x0);

    xre = XT_SEL32_HH_SX2(x0, x0);
    xim = XT_SEL32_LL_SX2(x0, x0);

    t0 = XT_AE_MOVINT32X2_FROMXTFLOATX2(xre);
    t1 = XT_AE_MOVINT32X2_FROMXTFLOATX2(xim);

    u0 = AE_SRLI32(t0, 23);
    u1 = AE_SRLI32(t1, 23);
    u0 = AE_AND32(u0, 0xFF);
    u1 = AE_AND32(u1, 0xFF);
    e0 = AE_SUB32(u0, 127);
    e1 = AE_SUB32(u1, 127);
    nsa = AE_MAX32(e0, e1);
    b0 = AE_LT32(127, nsa);
    AE_MOVT32X2(nsa, 127, b0);

    e0 = AE_SUB32(127, nsa);
    b0 = AE_EQ32(e0, 0);
    nsa0 = AE_SLLI32(e0, 23);
    AE_MOVT32X2(nsa0, 0x00400000, b0);
    y0 = XT_AE_MOVXTFLOATX2_FROMINT32X2(nsa0);
    xre = XT_MUL_SX2(xre, y0);
    xim = XT_MUL_SX2(xim, y0);

    x0 = XT_MUL_SX2(xre, xre);
    x1 = XT_MUL_SX2(xim, xim);
    x0 = XT_ADD_SX2(x0, x1);
    y0 = XT_RSQRT_SX2(x0);
    b0 = XT_OEQ_SX2(x0, 0.0f);
    b1 = XT_OEQ_SX2(x0, I0);
    t0 = XT_AE_MOVINT32X2_FROMXTFLOATX2(y0);
    AE_MOVT32X2(t0, 0x7f800000, b0);
    AE_MOVT32X2(t0, 0x0, b1);
    y0 = XT_AE_MOVXTFLOATX2_FROMINT32X2(t0);
    x0 = XT_AE_MOVXTFLOATX2_FROMINT32X2(nsa0);
    y0 = XT_MUL_SX2(y0, x0);
    l = XT_LOW_S(y0);
    XT_SSX(l, (xtfloat *)Y, 0);
  }
} /* vec_complex2invmag() */
#endif
