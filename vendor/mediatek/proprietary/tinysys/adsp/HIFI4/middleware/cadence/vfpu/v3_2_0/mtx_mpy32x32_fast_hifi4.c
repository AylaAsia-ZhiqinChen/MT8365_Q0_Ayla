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
void mtx_mpy32x32_fast (  
                     int32_t** restrict z,
               const int32_t*  restrict x,
               const int32_t** restrict y,
               int M, int N, int P, int lsh )
{ 
    ae_valign az;
    ae_f64 B0,B1,B2,B3,B4,B5,B6,B7;
    ae_int32x2 X0,X1,X2,X3,C0,C1,Y0,Y1;
    const ae_int32x2* restrict px;
    const ae_int32x2** restrict py;
    const ae_int32x2* restrict py0;
    const ae_int32x2* restrict py1;
          ae_int32x2* restrict pz;
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
    __Pragma("loop_count min=2")
    for(m=0; m<8; m+=4)
    {
        py=(const ae_int32x2**)y;
        px=(const ae_int32x2*)(x+m*N);
        B0=B1=B2=B3=B4=B5=B6=B7=AE_ZERO64();
        __Pragma("loop_count min=2, factor=2")
        for(n=0; n<(N>>1); n++)
        {
            NASSERT_ALIGN8(y[n]);
            NASSERT_ALIGN8(y[n+1]);
            py0=py[0];
            py1=py[1];
            py+=2;
            X3=AE_L32X2_X (px,12*N);
            X2=AE_L32X2_X (px, 8*N);
            X1=AE_L32X2_X (px, 4*N);
            AE_L32X2_IP(X0,px,8);
            Y0=AE_L32X2_I(py0,0);
            Y1=AE_L32X2_I(py1,0);
            C0=AE_SEL32_HH(Y0,Y1);
            C1=AE_SEL32_LL(Y0,Y1);
            AE_MULAAFD32RA_HH_LL(B0,X0,C0);
            AE_MULAAFD32RA_HH_LL(B1,X0,C1);
            AE_MULAAFD32RA_HH_LL(B2,X1,C0);
            AE_MULAAFD32RA_HH_LL(B3,X1,C1);

            AE_MULAAFD32RA_HH_LL(B4,X2,C0);
            AE_MULAAFD32RA_HH_LL(B5,X2,C1);
            AE_MULAAFD32RA_HH_LL(B6,X3,C0);
            AE_MULAAFD32RA_HH_LL(B7,X3,C1);
        }
        X0=AE_TRUNCA32X2F64S(B0,B1,16+lsh);
        X1=AE_TRUNCA32X2F64S(B2,B3,16+lsh);
        X2=AE_TRUNCA32X2F64S(B4,B5,16+lsh);
        X3=AE_TRUNCA32X2F64S(B6,B7,16+lsh);
        // save results
        pz=(ae_int32x2*)(z[0]);
        az=AE_ZALIGN64();
        AE_SA32X2_IP(X0,az,pz);
        AE_SA64POS_FP(az,pz);
        pz=(ae_int32x2*)(z[1]);
        az=AE_ZALIGN64();
        AE_SA32X2_IP(X1,az,pz);
        AE_SA64POS_FP(az,pz);

        pz=(ae_int32x2*)(z[2]);
        az=AE_ZALIGN64();
        AE_SA32X2_IP(X2,az,pz);
        AE_SA64POS_FP(az,pz);
        pz=(ae_int32x2*)(z[3]);
        az=AE_ZALIGN64();
        AE_SA32X2_IP(X3,az,pz);
        AE_SA64POS_FP(az,pz);
        z+=4;
    }
}
