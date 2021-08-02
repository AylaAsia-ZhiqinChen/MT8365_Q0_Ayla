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
void mtx_mpy32x32 (  void* pScr,
                     int32_t** restrict z,
               const int32_t*  restrict x,
               const int32_t** restrict y,
               int M, int N, int P, int lsh )
{
    ae_valign ax0,ax1,ax2,ax3,ay,az;
    ae_f64 B0,B1,B2,B3,B4,B5,B6,B7;
    ae_int32x2 X0,X1,X2,X3,Y0,Y1;
    const ae_int32x2* restrict px0;
    const ae_int32x2* restrict px1;
    const ae_int32x2* restrict px2;
    const ae_int32x2* restrict px3;
    const ae_int32x2* restrict py;
    const ae_int32x2* restrict pz;
          ae_int32x2* restrict py0;
          ae_int32x2* restrict py1;
          ae_int32*   restrict zm0;
          ae_int32*   restrict zm1;
          ae_int32*   restrict zm2;
          ae_int32*   restrict zm3;
    const ae_int32* restrict yn0;
    const ae_int32* restrict yn1;
    int idxp0,idxp1;

    int m,n,p;
    int K;
    int32_t * restrict y0;
    int32_t * restrict y1;
    if (N<=0 || M<=0)    /* exceptional situation */
    {
        for (m=0; m<M; m++) 
        for (p=0; p<P; p++)
                z[m][p]=0;
        return ;
    }
    K = ((N+1)&(~1));
    y0 = (int32_t *)pScr;
    if((N&1)==0) y0+=2; // shift to the odd bank for even N (optimization for low-bank ordering)
    y1 = y0 + K;

    NASSERT_ALIGN8(y0);
    NASSERT_ALIGN8(y1);

    WUR_AE_CBEGIN0( (uintptr_t)( y0 + 0 ) );
    WUR_AE_CEND0  ( (uintptr_t)( y0 + K ) );
    WUR_AE_CBEGIN1( (uintptr_t)( y1 + 0 ) );
    WUR_AE_CEND1  ( (uintptr_t)( y1 + K ) );

    py0=(ae_int32x2*)y0;
    py1=(ae_int32x2*)y1;

    //----------------------------------------------------------------
    // process by 4 rows
    //----------------------------------------------------------------
    if(M>=4)
    {
        //__Pragma("loop_count min=1")
        for(p=0; p<P; p+=2)
        {
            idxp0=p*4;
            idxp1=idxp0+4;
            idxp1=XT_MIN(idxp1,(P-1)*4);
            /* first, copy 2 columns to the aligned storage and pad with zeroes */
            py=(const ae_int32x2*)y;
            ay=AE_LA64_PP(py);
            __Pragma("no_unroll")
            for(n=0; n<(N&(~1)); n+=2)
            {
                AE_LA32X2_IP(Y0,ay,py);
                yn0=(const ae_int32*)AE_MOVAD32_H(Y0);
                yn1=(const ae_int32*)AE_MOVAD32_L(Y0);
                Y0=AE_L32_X(yn0,idxp0);
                Y1=AE_L32_X(yn1,idxp0);
                Y0=AE_SEL32_LL(Y0,Y1);
                AE_S32X2_XC(Y0,py0,8);
                Y0=AE_L32_X(yn0,idxp1);
                Y1=AE_L32_X(yn1,idxp1);
                Y0=AE_SEL32_LL(Y0,Y1);
                AE_S32X2_XC1(Y0,py1,8);
            }
            if (N&1) 
            { 
                AE_LA32X2_IP(Y0,ay,py);
                yn0=(const ae_int32*)AE_MOVAD32_H(Y0);
                yn1=(const ae_int32*)AE_MOVAD32_L(Y0);
                Y0=AE_L32_X(yn0,idxp0);
                Y0=AE_SEL32_LL(Y0,Y1=AE_ZERO32());
                AE_S32X2_XC(Y0,py0,8);
                Y0=AE_L32_X(yn0,idxp1);
                Y0=AE_SEL32_LL(Y0,Y1=AE_ZERO32());
                AE_S32X2_XC1(Y0,py1,8);
            }
            __Pragma("no_reorder")
            // multiply
            pz=(const ae_int32x2*)z;
            __Pragma("loop_count min=1")
            for(m=0; m<(M&~3); m+=4)
            {
                az=AE_LA64_PP(pz);
                AE_LA32X2_IP(Y0,az,pz);
                AE_LA32X2_IP(Y1,az,pz);
                zm0=(ae_int32*)AE_MOVAD32_H(Y0);
                zm1=(ae_int32*)AE_MOVAD32_L(Y0);
                zm2=(ae_int32*)AE_MOVAD32_H(Y1);
                zm3=(ae_int32*)AE_MOVAD32_L(Y1);
                B0=B1=B2=B3=B4=B5=B6=B7=AE_ZERO64();
                px0=(const ae_int32x2*)(x+(m+0)*N);
                px1=(const ae_int32x2*)(((const int32_t*)px0)+N);
                px2=px0+N;
                px3=px1+N;
                ax0=AE_LA64_PP(px0);
                ax1=AE_LA64_PP(px1);
                ax2=AE_LA64_PP(px2);
                ax3=AE_LA64_PP(px3);
                py0=(ae_int32x2*)y0;
                py1=(ae_int32x2*)y1;
                __Pragma("loop_count min=1")
                for(n=0; n<K; n+=2)
                {
                    AE_LA32X2_IP(X0,ax0,px0);
                    AE_LA32X2_IP(X1,ax1,px1);
                    AE_LA32X2_IP(X2,ax2,px2);
                    AE_LA32X2_IP(X3,ax3,px3);
                    AE_L32X2_XC (Y0,py0,8);
                    AE_L32X2_XC1(Y1,py1,8);
                    AE_MULAAFD32RA_HH_LL(B0,X0,Y0);
                    AE_MULAAFD32RA_HH_LL(B1,X0,Y1);
                    AE_MULAAFD32RA_HH_LL(B2,X1,Y0);
                    AE_MULAAFD32RA_HH_LL(B3,X1,Y1);
                    AE_MULAAFD32RA_HH_LL(B4,X2,Y0);
                    AE_MULAAFD32RA_HH_LL(B5,X2,Y1);
                    AE_MULAAFD32RA_HH_LL(B6,X3,Y0);
                    AE_MULAAFD32RA_HH_LL(B7,X3,Y1);
                }
                X0=AE_TRUNCA32X2F64S(B0,B1,16+lsh);
                X2=AE_TRUNCA32X2F64S(B2,B3,16+lsh);
                X1=AE_SEL32_HH(X0,X0);
                X3=AE_SEL32_HH(X2,X2);
                AE_S32_L_X(X0,zm0,idxp1);
                AE_S32_L_X(X1,zm0,idxp0);
                AE_S32_L_X(X2,zm1,idxp1);
                AE_S32_L_X(X3,zm1,idxp0);

                X0=AE_TRUNCA32X2F64S(B4,B5,16+lsh);
                X2=AE_TRUNCA32X2F64S(B6,B7,16+lsh);
                X1=AE_SEL32_HH(X0,X0);
                X3=AE_SEL32_HH(X2,X2);
                AE_S32_L_X(X0,zm2,idxp1);
                AE_S32_L_X(X1,zm2,idxp0);
                AE_S32_L_X(X2,zm3,idxp1);
                AE_S32_L_X(X3,zm3,idxp0);
            }
        } 
    }
    //----------------------------------------------------------------
    // process last up to 3 rows if M is not a multiple of 4
    //----------------------------------------------------------------
    if (M&3)
    {
        //__Pragma("loop_count min=1")
        for(p=0; p<P; p+=2)
        {
            idxp0=p*4;
            idxp1=idxp0+4;
            idxp1=XT_MIN(idxp1,(P-1)*4);
            /* first, copy 2 columns to the aligned storage and pad with zeroes */
            py=(const ae_int32x2*)y;
            ay=AE_LA64_PP(py);
            __Pragma("no_unroll")
            for(n=0; n<(N&(~1)); n+=2)
            {
                AE_LA32X2_IP(Y0,ay,py);
                yn0=(const ae_int32*)AE_MOVAD32_H(Y0);
                yn1=(const ae_int32*)AE_MOVAD32_L(Y0);
                Y0=AE_L32_X(yn0,idxp0);
                Y1=AE_L32_X(yn1,idxp0);
                Y0=AE_SEL32_LL(Y0,Y1);
                AE_S32X2_XC(Y0,py0,8);
                Y0=AE_L32_X(yn0,idxp1);
                Y1=AE_L32_X(yn1,idxp1);
                Y0=AE_SEL32_LL(Y0,Y1);
                AE_S32X2_XC1(Y0,py1,8);
            }
            if (N&1) 
            { 
                AE_LA32X2_IP(Y0,ay,py);
                yn0=(const ae_int32*)AE_MOVAD32_H(Y0);
                yn1=(const ae_int32*)AE_MOVAD32_L(Y0);
                Y0=AE_L32_X(yn0,idxp0);
                Y0=AE_SEL32_LL(Y0,Y1=AE_ZERO32());
                AE_S32X2_XC(Y0,py0,8);
                Y0=AE_L32_X(yn0,idxp1);
                Y0=AE_SEL32_LL(Y0,Y1=AE_ZERO32());
                AE_S32X2_XC1(Y0,py1,8);
            }
            __Pragma("no_reorder")
            // multiply
            __Pragma("loop_count min=1")
            for(m=(M&~3); m<M; m++)
            {
                zm0 = (ae_int32*)z[m];
                B0=B1=AE_ZERO64();
                px0=(const ae_int32x2*)(x+m*N);
                ax0=AE_LA64_PP(px0);
                py0=(ae_int32x2*)y0;
                py1=(ae_int32x2*)y1;
                __Pragma("loop_count min=1")
                for(n=0; n<K; n+=2)
                {
                    AE_LA32X2_IP(X0,ax0,px0);
                    AE_L32X2_XC (Y0,py0,8);
                    AE_L32X2_XC1(Y1,py1,8);
                    AE_MULAAFD32RA_HH_LL(B0,X0,Y0);
                    AE_MULAAFD32RA_HH_LL(B1,X0,Y1);
                }
                X0=AE_TRUNCA32X2F64S(B0,B1,16+lsh);
                X1=AE_SEL32_HH(X0,X0);
                AE_S32_L_X(X0,zm0,idxp1);
                AE_S32_L_X(X1,zm0,idxp0);
            }
        } 
    }
}
