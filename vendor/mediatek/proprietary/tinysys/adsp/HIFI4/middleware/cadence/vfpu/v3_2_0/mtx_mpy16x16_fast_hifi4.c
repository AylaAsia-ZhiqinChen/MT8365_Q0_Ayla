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
#include "NatureDSP_types.h"
#include "common.h"

/*===========================================================================
  Matrix Operations:
  mtx_mpy              Matrix Multiply
  mtx_vecmpy           Matrix by Vector Multiply
===========================================================================*/
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
void mtx_mpy16x16_fast (  
                     int16_t** restrict z,
               const int16_t*  restrict x,
               const int16_t** restrict y,
               int M, int N, int P, int lsh )
{
    ae_valign ay,az;
    ae_int64 B0,B1,B2,B3,B4,B5,B6,B7;
    ae_int16x4 X0,X1,X2,X3,Y0,Y1;
    ae_int32x2 Z0,Z1,Z2,Z3;
    const ae_int16x4 * restrict px;
    const ae_int32x2 * restrict pz;
    const ae_int32x2 * restrict py;
    const ae_int16x4 * restrict py0;
    const ae_int16x4 * restrict py1;
    const ae_int16x4 * restrict py2;
    const ae_int16x4 * restrict py3;
    int16_t* restrict zz;

    int m,n;
    NASSERT((N&3)==0);
    NASSERT(M==8);
    NASSERT(P==2);
    NASSERT_ALIGN8(x);
    if (N<=0 || M<=0)    /* exceptional situation */
    {
        for (m=0; m<M; m++) z[m][0]=z[m][1]=0;
        return ;
    }

  
    pz=(const ae_int32x2 *)z;
    az=AE_LA64_PP(pz);
    __Pragma("loop_count min=2,max=2")
    for(m=0; m<2; m++)
    {
        B0=B1=B2=B3=B4=B5=B6=B7=AE_ZERO64();
        px=(const ae_int16x4 *)(x);
        py=(const ae_int32x2 *)y;
        ay=AE_LA64_PP(py);

        __Pragma("loop_count min=1")
        for(n=0; n<((N>>2)); n++)
        {
          AE_LA32X2_IP(Z0, ay, py);
          AE_LA32X2_IP(Z1, ay, py);
            py0=(const ae_int16x4 *)AE_MOVAD32_H(Z0);
            py1=(const ae_int16x4 *)AE_MOVAD32_L(Z0);
            py2=(const ae_int16x4 *)AE_MOVAD32_H(Z1);
            py3=(const ae_int16x4 *)AE_MOVAD32_L(Z1);
            X0=AE_L16X4_I (py0,0);
            X1=AE_L16X4_I (py1,0);
            X2=AE_L16X4_I (py2,0); 
            X3=AE_L16X4_I (py3,0);
            // transpose 4x2 to 2x4
            {
            ae_int16x4 t0,t1;
            t0=AE_SEL16_7362(X0,X1);
            t1=AE_SEL16_7362(X2,X3);
            Y0=AE_SEL16_7632(t0,t1);
            Y1=AE_SEL16_5410(t0,t1);
            }
            // load 4 x and multiply 4 raws
            X1=AE_L16X4_X (px,2*N);
            X2=AE_L16X4_X (px,4*N);
            X3=AE_L16X4_X (px,6*N);
            AE_L16X4_IP(X0,px,8);
            AE_MULAAAAQ16(B0,X0,Y0);
            AE_MULAAAAQ16(B1,X0,Y1);
            AE_MULAAAAQ16(B2,X1,Y0);
            AE_MULAAAAQ16(B3,X1,Y1);
            AE_MULAAAAQ16(B4,X2,Y0);
            AE_MULAAAAQ16(B5,X2,Y1);
            AE_MULAAAAQ16(B6,X3,Y0);
            AE_MULAAAAQ16(B7,X3,Y1);
            
        }
        Z0=AE_TRUNCA32X2F64S(B0,B1,lsh+33);
        Z1=AE_TRUNCA32X2F64S(B2,B3,lsh+33);
        Z2=AE_TRUNCA32X2F64S(B4,B5,lsh+33);
        Z3=AE_TRUNCA32X2F64S(B6,B7,lsh+33);
        Y0=AE_ROUND16X4F32SASYM(Z0,Z1);
        Y1=AE_ROUND16X4F32SASYM(Z2,Z3);

        AE_LA32X2_IP(Z0,az,pz);
        AE_LA32X2_IP(Z1,az,pz);

        zz=(int16_t *)AE_MOVAD32_H(Z0);
        zz[0]=AE_MOVAD16_3(Y0);
        zz[1]=AE_MOVAD16_2(Y0);
        zz=(int16_t *)AE_MOVAD32_L(Z0);
        zz[0]=AE_MOVAD16_1(Y0);
        zz[1]=AE_MOVAD16_0(Y0);
        zz=(int16_t *)AE_MOVAD32_H(Z1);
        zz[0]=AE_MOVAD16_3(Y1);
        zz[1]=AE_MOVAD16_2(Y1);
        zz=(int16_t *)AE_MOVAD32_L(Z1);
        zz[0]=AE_MOVAD16_1(Y1);
        zz[1]=AE_MOVAD16_0(Y1);
        x+=4*N;
    }
}
