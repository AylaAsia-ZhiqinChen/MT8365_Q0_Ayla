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

/* DSP Library API */
#include "NatureDSP_Signal.h"
/* Common helper macros. */
#include "common.h"
/* Tables */
#include "expf_tbl.h"
#include "alog10f_tbl.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void,vec_antilog10f,(float32_t * restrict y, const float32_t* restrict x, int N))
#else
/*===========================================================================
  Vector matematics:
  vec_antilog          Antilogarithm         
===========================================================================*/

/*-------------------------------------------------------------------------
  Antilogarithm
  These routines calculate antilogarithm (base2, natural and base10). 
  Fixed-point functions accept inputs in Q25 and form outputs in Q16.15 
  format and return 0x7FFFFFFF in case of overflow and 0 in case of 
  underflow.

  Precision:
  32x32  32-bit inputs, 32-bit outputs. Accuracy: 4*e-5*y+1LSB
  f      floating point: Accuracy: 2 ULP
  NOTE:
  1.  Floating point functions are compatible with standard ANSI C routines 
      and set errno and exception flags accordingly

  Input:
  x[N]  input data,Q25 or floating point 
  N     length of vectors
  Output:
  y[N]  output data,Q16.15 or floating point  

  Restriction:
  x,y should not overlap

  Scalar versions:
  ----------------
  fixed point functions return result in Q16.15

-------------------------------------------------------------------------*/

void vec_antilog10f (float32_t * restrict y, const float32_t* restrict x, int N)
{
  const xtfloatx2 *          X0  = (xtfloatx2*)x;
  const xtfloatx2 *          X1  = (xtfloatx2*)x;
  const ae_int32  *          TBL = (ae_int32*)expftbl_Q30;
        xtfloatx2 * restrict Y   = (xtfloatx2*)y;

  ae_valign X0_va, X1_va, Y_va;

  xtfloatx2 x0, x1, y0, y1;
  ae_int32x2 tb0, tb1, tb2, tb3, tb4, tb5, tb6;
  ae_int32x2 u0, u1, e0, e1, n0;
  ae_f32x2 f0;
  ae_int64 wh, wl;
  xtbool2 b2_nan;

  int n;

  if ( N<=0 ) return;

  X0_va = AE_LA64_PP(X0);
  X1_va = AE_LA64_PP(X1);
  Y_va = AE_ZALIGN64();

  for ( n=0; n<(N>>1); n++ )
  {
    XT_LASX2IP(x0, X0_va, X0);

    x0 = XT_MAX_SX2(alog10fminmax[0].f, x0);
    x0 = XT_MIN_SX2(alog10fminmax[1].f, x0);

    u0 = XT_TRUNC_SX2(x0, 25);

    /* scale input to 1/log10(2) and convert to Q31 */
    wh = AE_MUL32_HH(u0, invlog10_2_Q29);
    wl = AE_MUL32_LL(u0, invlog10_2_Q29);
    e0 = AE_TRUNCA32X2F64S(wh, wl, -22);
    wh = AE_SLLI64(wh, 32-22);
    wl = AE_SLLI64(wl, 32-22);   
    u0 = AE_TRUNCI32X2F64S(wh, wl, 0);
    u0 = AE_SRLI32(u0,1);

    tb0 = AE_L32_I( TBL, 0*4 );
    tb1 = AE_L32_I( TBL, 1*4 );
    tb2 = AE_L32_I( TBL, 2*4 );
    tb3 = AE_L32_I( TBL, 3*4 );
    tb4 = AE_L32_I( TBL, 4*4 );
    tb5 = AE_L32_I( TBL, 5*4 );
    tb6 = AE_L32_I( TBL, 6*4 );

    /*
     * Compute 2^t in Q30 where t is in Q31. Use a combination of Estrin's
     * method and Horner's scheme to evaluate the fixed-point polynomial.                                     
     */

    f0 = tb2; AE_MULAFP32X2RAS(f0, tb1, u0); tb1 = f0;
    f0 = tb4; AE_MULAFP32X2RAS(f0, tb3, u0); tb2 = f0;
    f0 = tb6; AE_MULAFP32X2RAS(f0, tb5, u0); tb3 = f0;

    u1 = AE_MULFP32X2RAS(u0, u0);

    f0 = tb0;                               n0 = f0; 
    f0 = tb1; AE_MULAFP32X2RAS(f0, u1, n0); n0 = f0;
    f0 = tb2; AE_MULAFP32X2RAS(f0, u1, n0); n0 = f0;
    f0 = tb3; AE_MULAFP32X2RAS(f0, u1, n0); n0 = f0;

    /* convert back to the floating point  */
    x0 = XT_FLOAT_SX2(n0, 30);

    e0 = AE_ADD32(e0, 254);
    e1 = AE_SRAI32(e0, 1);
    e0 = AE_SUB32(e0, e1);

    u0 = AE_SLAI32(e0, 23);
    u1 = AE_SLAI32(e1, 23);
    y0 = XT_AE_MOVXTFLOATX2_FROMINT32X2(u0);
    y1 = XT_AE_MOVXTFLOATX2_FROMINT32X2(u1);

    XT_LASX2IP(x1, X1_va, X1);
    b2_nan = XT_UN_SX2(x1, x1);
    XT_MOVT_SX2( x0, x1, b2_nan );

    y0 = XT_MUL_SX2(y0, y1);
    y0 = XT_MUL_SX2(x0, y0);

    XT_SASX2IP(y0, Y_va, Y);
  }

  XT_SASX2POSFP(Y_va, Y);

  if ( N&1 )
  {
    x0 = XT_LSI((xtfloat*)X0, 0);

    b2_nan = XT_UN_SX2(x0, x0);

    x0 = XT_MAX_SX2(alog10fminmax[0].f, x0);
    x0 = XT_MIN_SX2(alog10fminmax[1].f, x0);

    u0 = XT_TRUNC_SX2(x0, 25);

    /* scale input to 1/log10(2) and convert to Q31 */
    wh = AE_MUL32_HH(u0, invlog10_2_Q29);
    wl = AE_MUL32_LL(u0, invlog10_2_Q29);
    e0 = AE_TRUNCA32X2F64S(wh, wl, -22);
    wh = AE_SLLI64(wh, 32-22);
    wl = AE_SLLI64(wl, 32-22);   
    u0 = AE_TRUNCI32X2F64S(wh, wl, 0);
    u0 = AE_SRLI32(u0,1);

    tb0 = AE_L32_I( TBL, 0*4 );
    tb1 = AE_L32_I( TBL, 1*4 );
    tb2 = AE_L32_I( TBL, 2*4 );
    tb3 = AE_L32_I( TBL, 3*4 );
    tb4 = AE_L32_I( TBL, 4*4 );
    tb5 = AE_L32_I( TBL, 5*4 );
    tb6 = AE_L32_I( TBL, 6*4 );

    /*
     * Compute 2^t in Q30 where t is in Q31. Use a combination of Estrin's
     * method and Horner's scheme to evaluate the fixed-point polynomial.                                     
     */

    f0 = tb2; AE_MULAFP32X2RAS(f0, tb1, u0); tb1 = f0;
    f0 = tb4; AE_MULAFP32X2RAS(f0, tb3, u0); tb2 = f0;
    f0 = tb6; AE_MULAFP32X2RAS(f0, tb5, u0); tb3 = f0;

    u1 = AE_MULFP32X2RAS(u0, u0);

    f0 = tb0;                               n0 = f0; 
    f0 = tb1; AE_MULAFP32X2RAS(f0, u1, n0); n0 = f0;
    f0 = tb2; AE_MULAFP32X2RAS(f0, u1, n0); n0 = f0;
    f0 = tb3; AE_MULAFP32X2RAS(f0, u1, n0); n0 = f0;

    /* convert back to the floating point  */
    x1 = XT_FLOAT_SX2(n0, 30);

    e0 = AE_ADD32(e0, 254);
    e1 = AE_SRAI32(e0, 1);
    e0 = AE_SUB32(e0, e1);

    u0 = AE_SLAI32(e0, 23);
    u1 = AE_SLAI32(e1, 23);
    y0 = XT_AE_MOVXTFLOATX2_FROMINT32X2(u0);
    y1 = XT_AE_MOVXTFLOATX2_FROMINT32X2(u1);

    XT_MOVT_SX2( y0, x0, b2_nan );

    y0 = XT_MUL_SX2(y0, y1);
    y0 = XT_MUL_SX2(x1, y0);

    XT_SSX(XT_HIGH_S(y0), (xtfloat*)Y, 0);
  }

} /* vec_antilog10f() */
#endif
