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
	NatureDSP_Signal library. FFT part
    Inner stages of complex-valued FFT: 16-bit data, 16-bit twiddle factors
    C code optimized for HiFi4
	IntegrIT, 2006-2014
*/

/* Common utility macros. */
#include "common.h"
/* Twiddle factor tables and FFT descriptor structure. */
#include "fft_cplx_twd.h"
/* Inner FFT stages shared between forward and inverse FFT kernels. */
#include "fft_cplx_common.h"

/*------------------------------------------------------------------------------
  Inner radix-4 stages of complex-valued FFT of size 2^n, n=4..12.

  Notes:
    1. Inner means second through the next to last stages.
    2. Data are not permuted.
    3. At each stage data are automatically downscaled to avoid overflows.

  Precision: 
    32x32  32-bit input/output data, 32-bit twiddle factors
    32x16  32-bit input/output data, 16-bit twiddle factors
    16x16  16-bit input/output data, 16-bit twiddle factors

  Input:
    x[2*N]/y[2*N]  complex input data. Real and imaginary data are interleaved 
                   and real data goes first.
                   16x16,32x16:                  input data in y[2*N]
                   32x32, (log2(N)+1)/2 is even: input data in y[2*N]
                   32x32, (log2(N)+1)/2 is odd:  input data in x[2*N]
   twd[2*N*3/4]    complex twiddle factor table

  Output:
    y[2*N]         complex output data. Real and imaginary data are interleaved 
                   and real data goes first.

  Input/Output:
    bexp           common block exponent, that is the minimum number of
                   redundant sign bits over input (output) data

  Returned value: total right shift amount applied to dynamically scale the data

  Restrictions:
    x[],y[],twd[] - must not overlap
    x[],y[],twd[] - must be aligned on 8-byte boundary

------------------------------------------------------------------------------*/

int fft_cplx_inner_16x16( int16_t * restrict y,
                    const int16_t *          twd,
                    int * bexp, int N )
{
  const ae_int32x2 *          X;
        ae_int32x2 * restrict Y;
  const ae_int16x4 *          TWD;

  int m, n, step, stride;
  int nsa, shift, shiftSum;

  ae_int16x4 a0, a1, a2, a3;
  ae_int32x2 t0, t1, t2, t3;
  ae_int32x2 p0, p1, p2, p3;
  ae_int32x2 scl;

  ae_f32x2 a00, a01, a02, a03;
  ae_f32x2 a10, a11, a12, a13;
  ae_f32x2 b00, b01, b02, b03;
  ae_f32x2 b10, b11, b12, b13;

  ae_int16x4 tw_12, tw_3_;
  ae_int16x4 tw_45, tw_6_;

  NASSERT_ALIGN8( y   );
  NASSERT_ALIGN8( twd );

  NASSERT( N >= 16 && N <= 4096 );

  NASSERT( 0 <= *bexp && *bexp <= 3 );

  nsa = *bexp;

  shiftSum = 0;

  for ( stride=N/16, step=4; stride>1; stride/=4, step*=4 )
  {
    shiftSum += ( shift = 3 - nsa );

    scl = AE_MOVDA32X2( -1L<<(31-shift), 0 );

    X = (ae_int32x2*)y;
    Y = (ae_int32x2*)y;

    TWD = (ae_int16x4*)twd;

    __Pragma( "loop_count min=1" );
    for ( m=0; m<stride/2; m++ )
    {
      __Pragma( "loop_count min=4, factor=4" );
      __Pragma( "concurrent" );
      for ( n=0; n<step; n++ )
      {
        AE_L16X4_IP( tw_12, TWD, +2*4            );
        AE_L16X4_XP( tw_3_, TWD, -2*4 + 3*step*4 );
        AE_L16X4_IP( tw_45, TWD, +2*4            );
        AE_L16X4_XP( tw_6_, TWD, -2*4 - 3*step*4 );

        AE_L32X2_XP( t0, X, +stride*4 );
        AE_L32X2_XP( t1, X, +stride*4 );
        AE_L32X2_XP( t2, X, +stride*4 );
        AE_L32X2_XP( t3, X, +stride*4 );

        a0 = AE_MOVINT16X4_FROMINT32X2( t0 );
        a1 = AE_MOVINT16X4_FROMINT32X2( t1 );
        a2 = AE_MOVINT16X4_FROMINT32X2( t2 );
        a3 = AE_MOVINT16X4_FROMINT32X2( t3 );

        a00 = AE_MULFC32X16RAS_H( scl, a0 );
        a10 = AE_MULFC32X16RAS_L( scl, a0 );
        a01 = AE_MULFC32X16RAS_H( scl, a1 );
        a11 = AE_MULFC32X16RAS_L( scl, a1 );
        a02 = AE_MULFC32X16RAS_H( scl, a2 );
        a12 = AE_MULFC32X16RAS_L( scl, a2 );
        a03 = AE_MULFC32X16RAS_H( scl, a3 );
        a13 = AE_MULFC32X16RAS_L( scl, a3 );

        //
        // Butterfly 0
        //

        AE_ADDANDSUB32S( b00, b02, a00, a02 );
        AE_ADDANDSUB32S( b01, b03, a03, a01 );

        b03 = AE_MUL32JS( b03 );

        AE_ADDANDSUB32S( a00, a02, b00, b01 );
        AE_ADDANDSUB32S( a01, a03, b02, b03 );

        a01 = AE_MULFC32X16RAS_H( a01, tw_12 );
        a02 = AE_MULFC32X16RAS_L( a02, tw_12 );
        a03 = AE_MULFC32X16RAS_H( a03, tw_3_ );

        //
        // Butterfly 1
        //

        AE_ADDANDSUB32S( b10, b12, a10, a12 );
        AE_ADDANDSUB32S( b11, b13, a13, a11 );

        b13 = AE_MUL32JS( b13 );

        AE_ADDANDSUB32S( a10, a12, b10, b11 );
        AE_ADDANDSUB32S( a11, a13, b12, b13 );

        a11 = AE_MULFC32X16RAS_H( a11, tw_45 );
        a12 = AE_MULFC32X16RAS_L( a12, tw_45 );
        a13 = AE_MULFC32X16RAS_H( a13, tw_6_ );

        //
        // 32->16-bit conversion, evaluation of min. NSA, output data storage
        //

        t0 = AE_SEL32_LL( a10, a00 );
        t1 = AE_SEL32_LL( a11, a01 );
        t2 = AE_SEL32_LL( a12, a02 );
        t3 = AE_SEL32_LL( a13, a03 );

        AE_RNG32X2( t3 );
        AE_RNG32X2( t2 );
        AE_RNG32X2( t1 );
        AE_RNG32X2( t0 );

        a0 = AE_ROUND16X4F32SASYM( a00, a10 );
        a1 = AE_ROUND16X4F32SASYM( a01, a11 );
        a2 = AE_ROUND16X4F32SASYM( a02, a12 );
        a3 = AE_ROUND16X4F32SASYM( a03, a13 );

        p0 = AE_MOVINT32X2_FROMINT16X4( a0 );
        p1 = AE_MOVINT32X2_FROMINT16X4( a1 );
        p2 = AE_MOVINT32X2_FROMINT16X4( a2 );
        p3 = AE_MOVINT32X2_FROMINT16X4( a3 );

        // Two middle quartiles are swapped to use bit reversal instead of
        // digit reversal at the last stage.
        AE_S32X2RNG_XP( p0, Y, +stride*4 );
        AE_S32X2RNG_XP( p2, Y, +stride*4 );
        AE_S32X2RNG_XP( p1, Y, +stride*4 );
        AE_S32X2RNG_XP( p3, Y, +stride*4 );
      }

      __Pragma( "no_reorder" );

      X = Y = (ae_int32x2*)( (uintptr_t)X - step*4*stride*4 + 2*4 );

      TWD = (ae_int16x4*)( (uintptr_t)TWD + 2*3*step*4 );
    }

    AE_CALCRNG3();

    nsa = 3 - RUR_AE_SAR();
  }

  *bexp = nsa;

  return (shiftSum);

} // fft_cplx_inner_16x16()
