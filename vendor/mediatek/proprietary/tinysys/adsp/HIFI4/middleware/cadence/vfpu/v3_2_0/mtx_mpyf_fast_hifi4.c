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
   Matrix multiply
 * Optimized code for HiFi4
 */

/* Cross-platform data type definitions. */
#include "NatureDSP_types.h"
#include "NatureDSP_Signal.h"
/* Common helper macros. */
#include "common.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void,mtx_mpyf_fast,( float32_t * z, const float32_t * x,  const float32_t * y, int M, int N, int P ))
#else
/*-------------------------------------------------------------------------
  These functions compute the expression z = 2^lsh * x * y for the matrices 
  x and y. The columnar dimension of x must match the row dimension of y. 
  The resulting matrix has the same number of rows as x and the same number 
  of columns as y.
  Note:
  In the fixed-point routines, rows of matrices z and y may be stored in 
  non consecutive manner. Matrix x will have all the elements in contiguous 
  memory locations.

  Two versions of functions available: regular version (mtx_mpy32x32, 
  mtx_mpy16x16, mtx_mpyf) with arbitrary arguments and faster version 
  mtx_mpy32x32_fast, mtx_mpy16x16_fast, mtx_mpyf_fast) that apply some 
  restrictions.

  Precision: 
  32x32 32-bit inputs, 32-bit output
  16x16 16-bit inputs, 16-bit output
  f     floating point

  Input:
  x[M*N]      input matrix,Q31, Q15 or floating point
  y[N][P]     input matrix y. For fixed point routines, these are N 
              vectors of size P,Q31 or Q15. For floating point, this 
              is just a matrix of size NxP.
  M           number of rows in matrix x and z
  N           number of columns in matrix x and number of rows in matrix y
  P           number of columns in matrices y and z
  lsh         additional left shift
  Output:
  z[M][P]     output matrix z. For fixed point routines, these are M 
              vectors of size P Q31 or Q15. For floating point, this 
              is single matrix of size MxP
  Scratch:
  pScr        size in bytes defined by macros SCRATCH_MTX_MPY32X32,
              SCRATCH_MTX_MPY16X16

  Restrictions:
  For regular routines (mtx_mpy32x32, mtx_mpy16x16, mtx_mpyf):
  x,y,z should not overlap

  For faster routines (mtx_mpy32x32_fast, mtx_mpy16x16_fast, mtx_mpyf_fast):
  x,y,z should not overlap
  x - aligned on 8-byte boundary
  all rows which addresses are written to y[] - aligned on 8-byte boundary
  N is a multiple of 4,M=8,P=2  
-------------------------------------------------------------------------*/
#define SZ_F32 (sizeof(float32_t))

void mtx_mpyf_fast( float32_t * z, const float32_t * x,  const float32_t * y, int M, int N, int P )
{
  const xtfloatx2 *restrict pX0;
  const xtfloatx2 *restrict pX1;
  const xtfloatx2 *restrict pX2;
  const xtfloatx2 *restrict pX3;
  const xtfloatx2 *restrict pY0;
  const xtfloatx2 *restrict pY1;
        xtfloatx2 *restrict pZ;
  xtfloatx2 y0, y1, t;
  xtfloatx2 x0, x1, x2, x3;
  xtfloatx2 acc00, acc01, acc10, acc11, acc20, acc21, acc30, acc31;
  xtfloatx2 z00, z10, z20, z30, z01, z11, z21, z31;
  ae_valign vz;
  int m, n;

  NASSERT(x);
  NASSERT(y);
  NASSERT(z);
  NASSERT((z != x) && (z != y));
  NASSERT(N%4==0 && M==8 && P==2);
  NASSERT_ALIGN(x,8);
  NASSERT_ALIGN(y,8);
  
  pZ = (xtfloatx2 *)(z);
  vz = AE_ZALIGN64();
  /* If N<4 then clear output matrix and return */
  if (N < 4)
  {
    z00 = (xtfloatx2)0.0f;
    for (m = 0; m < 8; m++)
    {
      XT_SASX2IP(z00, vz, pZ);
    }
    XT_SASX2POSFP(vz, pZ);
    return;
  }
  
  pX0 = (const xtfloatx2 *)(x);
  pY0 = (const xtfloatx2 *)(y);
  pY1 = (const xtfloatx2 *)(y+2);
  AE_SETCBEGIN0(y);
  AE_SETCEND0(y+2*N);
  /* Compute multiplication by 4 rows */
  for (m = 0; m < 2; m++)
  {
    pX1 = (const xtfloatx2 *)((float32_t *)pX0 + N);
    pX2 = (const xtfloatx2 *)((float32_t *)pX1 + N);
    pX3 = (const xtfloatx2 *)((float32_t *)pX2 + N);

    acc00 = acc01 = acc10 = acc11 = 
    acc20 = acc21 = acc30 = acc31 = (xtfloatx2)0.0f;

    __Pragma("super_swp ii=9, unroll=2")
    __Pragma("loop_count min=2,factor=2")
    for (n = 0; n < (N>>1); n++)
    {
      XT_LSX2IP(x0, pX0, 2*SZ_F32);
      XT_LSX2IP(x1, pX1, 2*SZ_F32);
      XT_LSX2IP(x2, pX2, 2*SZ_F32);
      XT_LSX2IP(x3, pX3, 2*SZ_F32);

      XT_LSX2XC(y0, pY0, 4*SZ_F32);
      XT_LSX2XC(y1, pY1, 4*SZ_F32);

      t = XT_SEL32_LL_SX2(y0, y1);
      y0 = XT_SEL32_HH_SX2(y0, y1);
      y1 = t;

      XT_MADD_SX2(acc00, x0, y0);
      XT_MADD_SX2(acc01, x0, y1);
      XT_MADD_SX2(acc10, x1, y0);
      XT_MADD_SX2(acc11, x1, y1);
      XT_MADD_SX2(acc20, x2, y0);
      XT_MADD_SX2(acc21, x2, y1);
      XT_MADD_SX2(acc30, x3, y0);
      XT_MADD_SX2(acc31, x3, y1);
    }
    z00 = XT_SEL32_HL_SX2(acc00, acc01);
    z01 = XT_SEL32_LH_SX2(acc00, acc01);
    z00 = z00 + z01;
    z10 = XT_SEL32_HL_SX2(acc10, acc11);
    z11 = XT_SEL32_LH_SX2(acc10, acc11);
    z10 = z10 + z11;
    z20 = XT_SEL32_HL_SX2(acc20, acc21);
    z21 = XT_SEL32_LH_SX2(acc20, acc21);
    z20 = z20 + z21;
    z30 = XT_SEL32_HL_SX2(acc30, acc31);
    z31 = XT_SEL32_LH_SX2(acc30, acc31);
    z30 = z30 + z31;

    XT_SASX2IP(z00, vz, pZ);
    XT_SASX2IP(z10, vz, pZ);
    XT_SASX2IP(z20, vz, pZ);
    XT_SASX2IP(z30, vz, pZ);
    /* jump to the next 4 rows */
    pX0 = pX3;
  }
  XT_SASX2POSFP(vz, pZ);
} /* mtx_mpyf_fast() */
#endif
