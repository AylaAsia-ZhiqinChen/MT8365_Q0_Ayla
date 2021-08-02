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

void mtx_mpy16x16 (  void* pScr,
                     int16_t** restrict z,
               const int16_t*  restrict x,
               const int16_t** restrict y,
               int M, int N, int P, int lsh )
{

    ae_valign ax,ay,az;
    int m,n,p;
    int K;
    ae_int64 B0,B1,B2,B3;
    ae_int16x4 X0,X1,X2,X3,Y0,Y1,Y2,Y3;
    ae_int32x2 Z0,Z1;
    const ae_int16x4 * restrict px;
          ae_int16x4 * restrict py;
    const ae_int16x4 * restrict py0;
    const ae_int16x4 * restrict py1;
    const ae_int16x4 * restrict py2;
    const ae_int16x4 * restrict py3;
    const ae_int32x2 * restrict pz;

    int16_t * restrict y0;
    if (N<=0 || M<=0)    /* exceptional situation */
    {
        for (m=0; m<M; m++) 
        for (p=0; p<P; p++)
                z[m][p]=0;
        return ;
    }
    K = ((N+3)&(~3));
    y0 = (int16_t *)pScr;
    NASSERT_ALIGN8(y0);

    WUR_AE_CBEGIN0( (uintptr_t)( y0 + 0 ) );
    WUR_AE_CEND0  ( (uintptr_t)( y0 + K ) );

    py=( ae_int16x4*)y0;

    /*
        for bit mask:
        N&3  bmask0  bmask1
         0    11       11
         1    10       00
         2    11       00
         3    11       10
    */
   // mask=0xEC8F>>((N&3)<<2);
 //   bmask0=AE_MOVBA2(mask>>2);
  //  bmask1=AE_MOVBA2(mask);

    for(p=0; p<P; p+=4)
    {
        int idxp0,idxp1,idxp2,idxp3;
        idxp0=p*2;
        idxp1=idxp0+2;
        idxp2=idxp1+2;
        idxp3=idxp2+2;
        idxp1=XT_MIN(idxp1,(P-1)*2);
        idxp2=XT_MIN(idxp2,(P-1)*2);
        idxp3=XT_MIN(idxp3,(P-1)*2);
        /* copy 4 rows with zero padding */
        pz=(const ae_int32x2 *)y;
        az=AE_LA64_PP(pz);
        for(n=0; n<((N>>2)); n++)
       // for (n = 0; n<((N - 1)&~3); n += 4)
        {
            AE_LA32X2_IP(Z0,az,pz);
            AE_LA32X2_IP(Z1,az,pz);
            Z0=AE_ADD32(Z0,AE_MOVDA32(idxp0));
            Z1=AE_ADD32(Z1,AE_MOVDA32(idxp0));
            py0=(const ae_int16x4 *)AE_MOVAD32_H(Z0);
            py1=(const ae_int16x4 *)AE_MOVAD32_L(Z0);
            py2=(const ae_int16x4 *)AE_MOVAD32_H(Z1);
            py3=(const ae_int16x4 *)AE_MOVAD32_L(Z1);
            ay=AE_LA64_PP(py0); AE_LA16X4_IP(X0,ay,py0);
            ay=AE_LA64_PP(py1); AE_LA16X4_IP(X1,ay,py1);
            ay=AE_LA64_PP(py2); AE_LA16X4_IP(X2,ay,py2);
            ay=AE_LA64_PP(py3); AE_LA16X4_IP(X3,ay,py3);
            // transpose 4x4
            Y0=AE_SEL16_6420(X0,X1);
            Y1=AE_SEL16_6420(X2,X3);
            Y2=AE_SEL16_7531(X0,X1);
            Y3=AE_SEL16_7531(X2,X3);
            X0=AE_SEL16_6420(Y0,Y1);
            X1=AE_SEL16_6420(Y2,Y3);
            X2=AE_SEL16_7531(Y0,Y1);
            X3=AE_SEL16_7531(Y2,Y3);
            // save transposed matrix
            AE_S16X4_X (X0,py,6*K);
            AE_S16X4_X (X1,py,4*K);
            AE_S16X4_X (X2,py,2*K);
            AE_S16X4_XC(X3,py,8);
        }
        {
          // last up to 4 iterations with zero padding
          switch (N & 3)
          {
          case 3:
          {
            AE_LA32X2_IP(Z0, az, pz);
            AE_L32_IP(Z1, castxcc(ae_int32, pz), 0);
            Z0 = AE_ADD32(Z0, AE_MOVDA32(idxp0));
            Z1 = AE_ADD32(Z1, AE_MOVDA32(idxp0));
            py0 = (const ae_int16x4 *)AE_MOVAD32_H(Z0);
            py1 = (const ae_int16x4 *)AE_MOVAD32_L(Z0);
            py2 = (const ae_int16x4 *)AE_MOVAD32_H(Z1);
            ay = AE_LA64_PP(py0); AE_LA16X4_IP(X0, ay, py0);
            ay = AE_LA64_PP(py1); AE_LA16X4_IP(X1, ay, py1);
            ay = AE_LA64_PP(py2); AE_LA16X4_IP(X2, ay, py2);
            X3 = AE_ZERO16();
            // transpose 4x4
            Y0 = AE_SEL16_6420(X0, X1);
            Y1 = AE_SEL16_6420(X2, X3);
            Y2 = AE_SEL16_7531(X0, X1);
            Y3 = AE_SEL16_7531(X2, X3);
            X0 = AE_SEL16_6420(Y0, Y1);
            X1 = AE_SEL16_6420(Y2, Y3);
            X2 = AE_SEL16_7531(Y0, Y1);
            X3 = AE_SEL16_7531(Y2, Y3);
            // save transposed matrix
            AE_S16X4_X(X0, py, 6 * K);
            AE_S16X4_X(X1, py, 4 * K);
            AE_S16X4_X(X2, py, 2 * K);
            AE_S16X4_XC(X3, py, 8);
            break;
          }
          case 2:
            AE_LA32X2_IP(Z0, az, pz);
            Z0 = AE_ADD32(Z0, AE_MOVDA32(idxp0));
            py0 = (const ae_int16x4 *)AE_MOVAD32_H(Z0);
            py1 = (const ae_int16x4 *)AE_MOVAD32_L(Z0);
            ay = AE_LA64_PP(py0); AE_LA16X4_IP(X0, ay, py0);
            ay = AE_LA64_PP(py1); AE_LA16X4_IP(X1, ay, py1);
            X2 = AE_ZERO16();
            X3 = AE_ZERO16();
            // transpose 4x4
            Y0 = AE_SEL16_6420(X0, X1);
            Y1 = AE_SEL16_6420(X2, X3);
            Y2 = AE_SEL16_7531(X0, X1);
            Y3 = AE_SEL16_7531(X2, X3);
            X0 = AE_SEL16_6420(Y0, Y1);
            X1 = AE_SEL16_6420(Y2, Y3);
            X2 = AE_SEL16_7531(Y0, Y1);
            X3 = AE_SEL16_7531(Y2, Y3);
            // save transposed matrix
            AE_S16X4_X(X0, py, 6 * K);
            AE_S16X4_X(X1, py, 4 * K);
            AE_S16X4_X(X2, py, 2 * K);
            AE_S16X4_XC(X3, py, 8);
            break;
          case 1:
            AE_L32_IP(Z0, castxcc(ae_int32, pz), 0);
            Z0 = AE_ADD32(Z0, AE_MOVDA32(idxp0));
            py0 = (const ae_int16x4 *)AE_MOVAD32_H(Z0);
            ay = AE_LA64_PP(py0); AE_LA16X4_IP(X0, ay, py0);
            X1 = X2 = AE_ZERO16();
            X3 = AE_ZERO16();
            // transpose 4x4
            Y0 = AE_SEL16_6420(X0, X1);
            Y1 = AE_SEL16_6420(X2, X3);
            Y2 = AE_SEL16_7531(X0, X1);
            Y3 = AE_SEL16_7531(X2, X3);
            X0 = AE_SEL16_6420(Y0, Y1);
            X1 = AE_SEL16_6420(Y2, Y3);
            X2 = AE_SEL16_7531(Y0, Y1);
            X3 = AE_SEL16_7531(Y2, Y3);
            // save transposed matrix
            AE_S16X4_X(X0, py, 6 * K);
            AE_S16X4_X(X1, py, 4 * K);
            AE_S16X4_X(X2, py, 2 * K);
            AE_S16X4_XC(X3, py, 8);
            break;
          }
        }

        __Pragma("no_reorder")
        // main multiply loop for 4 rows

        if (K&4)
        {       // optimized for low-bit ordering: K is a multiple of 8
            ae_int16x4* py1;
            //const ae_int16x4* px1;
            __Pragma("ymemory(py1)")
            //__Pragma("ymemory(px1)")
            __Pragma("loop_count min=1")
            for(m=0; m<M; m++)
            {
                px=(const ae_int16x4 *)(x+m*N);
                py=( ae_int16x4*)y0;
                py1=( ae_int16x4*)(y0+K);
                ax=AE_LA64_PP(px);

                AE_LA16X4_IP(X0,ax,px); 
                Y2=AE_L16X4_X (py ,4*K);
                Y3=AE_L16X4_X (py1,4*K);
                AE_L16X4_IP(Y0,py ,8);
                AE_L16X4_IP(Y1,py1,8);
                B0=AE_MULZAAAAQ16(X0,Y0);
                B1=AE_MULZAAAAQ16(X0,Y1);
                B2=AE_MULZAAAAQ16(X0,Y2);
                B3=AE_MULZAAAAQ16(X0,Y3);
                for (n = 0; n<(K >> 3); n++)
                {
                  AE_LA16X4_IP(X0, ax, px);
                  Y2 = AE_L16X4_X(py, 4 * K);
                  Y3 = AE_L16X4_X(py1, 4 * K);
                  AE_L16X4_IP(Y0, py, 8);
                  AE_L16X4_IP(Y1, py1, 8);
                  AE_MULAAAAQ16(B0, X0, Y0);
                  AE_MULAAAAQ16(B1, X0, Y1);
                  AE_MULAAAAQ16(B2, X0, Y2);
                  AE_MULAAAAQ16(B3, X0, Y3);

                  AE_LA16X4_IP(X0, ax, px);
                  Y2 = AE_L16X4_X(py, 4 * K);
                  Y3 = AE_L16X4_X(py1, 4 * K);
                  AE_L16X4_IP(Y0, py, 8);
                  AE_L16X4_IP(Y1, py1, 8);
                  AE_MULAAAAQ16(B0, X0, Y0);
                  AE_MULAAAAQ16(B1, X0, Y1);
                  AE_MULAAAAQ16(B2, X0, Y2);
                  AE_MULAAAAQ16(B3, X0, Y3);
                }
                Z0=AE_TRUNCA32X2F64S(B0,B1,lsh+33);
                Z1=AE_TRUNCA32X2F64S(B2,B3,lsh+33);
                Y0=AE_ROUND16X4F32SASYM(Z0,Z1);
                                         AE_S16_0_X(Y0,(ae_int16*)z[m],idxp3);
                Y1=AE_SEL16_4321(Y0,Y0); AE_S16_0_X(Y1,(ae_int16*)z[m],idxp2);
                Y1=AE_SEL16_5432(Y0,Y0); AE_S16_0_X(Y1,(ae_int16*)z[m],idxp1);
                Y1=AE_SEL16_6543(Y0,Y0); AE_S16_0_X(Y1,(ae_int16*)z[m],idxp0);
            }
            py=( ae_int16x4*)y0;
        }
        else
        {       // optimized for low-bit ordering: K is a multiple of 8
            ae_int16x4* py1;
            const ae_int16x4* px1;
            __Pragma("ymemory(py1)")
            __Pragma("ymemory(px1)")
            __Pragma("loop_count min=1")
            for(m=0; m<M; m++)
            {
                B0=B1=B2=B3=AE_ZERO64();
                px=(const ae_int16x4 *)(x+m*N);
                py=( ae_int16x4*)y0;
                py1=( ae_int16x4*)(y0+4);
                ax=AE_LA64_PP(px);
                __Pragma("loop_count min=1")
                for(n=0; n<(K>>3); n++)
                {
                    AE_LA16X4_IP(X0,ax,px);
                    Y1=AE_L16X4_X (py,2*K);
                    Y2=AE_L16X4_X (py,4*K);
                    Y3=AE_L16X4_X (py,6*K);
                    AE_L16X4_IP(Y0,py,16);
                    AE_MULAAAAQ16(B0,X0,Y0);
                    AE_MULAAAAQ16(B1,X0,Y1);
                    AE_MULAAAAQ16(B2,X0,Y2);
                    AE_MULAAAAQ16(B3,X0,Y3);
                    px1=px;
                    AE_LA16X4_IP(X0,ax,px1);
                    px=px1;
                    Y1=AE_L16X4_X (py1,2*K);
                    Y2=AE_L16X4_X (py1,4*K);
                    Y3=AE_L16X4_X (py1,6*K);
                    AE_L16X4_IP(Y0,py1,16);
                    AE_MULAAAAQ16(B0,X0,Y0);
                    AE_MULAAAAQ16(B1,X0,Y1);
                    AE_MULAAAAQ16(B2,X0,Y2);
                    AE_MULAAAAQ16(B3,X0,Y3);
                }
                Z0=AE_TRUNCA32X2F64S(B0,B1,lsh+33);
                Z1=AE_TRUNCA32X2F64S(B2,B3,lsh+33);
                Y0=AE_ROUND16X4F32SASYM(Z0,Z1);
                                         AE_S16_0_X(Y0,(ae_int16*)z[m],idxp3);
                Y1=AE_SEL16_4321(Y0,Y0); AE_S16_0_X(Y1,(ae_int16*)z[m],idxp2);
                Y1=AE_SEL16_5432(Y0,Y0); AE_S16_0_X(Y1,(ae_int16*)z[m],idxp1);
                Y1=AE_SEL16_6543(Y0,Y0); AE_S16_0_X(Y1,(ae_int16*)z[m],idxp0);
            }
            py=( ae_int16x4*)y0;
        }
    }
}
