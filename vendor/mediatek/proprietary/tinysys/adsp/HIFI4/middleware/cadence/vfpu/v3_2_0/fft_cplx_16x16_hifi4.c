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
  NatureDSP Signal Processing Library. FFT part
    Complex-valued forward FFT: 16-bit data, 16-bit twiddle factors
    C code optimized for HiFi4
  IntegrIT, 2006-2014
*/

/* Library API. */
#include "NatureDSP_Signal.h"
/* Common utility macros. */
#include "common.h"
/* Twiddle factor tables and FFT descriptor structure. */
#include "fft_cplx_twd.h"
/* Inner FFT stages shared between forward and inverse FFT kernels. */
#include "fft_cplx_common.h"

#define MAX(a,b)   ( (a)>(b) ? (a) : (b) )

/*-------------------------------------------------------------------------
  Complex-valued forward FFT of size 2^n, n=4..12.

  NOTES:
  1. Input and output data are naturally ordered, i.e. the bit-reversal
     permutation is incorporated into the transform.
  2. Some of FFT computation stages are performed in-place, so INPUT DATA
     WILL APPEAR DAMAGED after the call.
  3. FFT routines dynamically downscale data at each stage in order to avoid
     overflows.
  4. To avoid degradation of FFT precision, input data should be upscaled
     so that the minimum number of redundant sign bits does not exceed 3
     bit positions, and this number must be passed to the FFT routine via
     the bexp parameter. 

  Precision: 
  32x32  32-bit input/output data, 32-bit twiddle factors
  32x16  32-bit input/output data, 16-bit twiddle factors
  16x16  16-bit input/output data, 16-bit twiddle factors
 
  Input:
  x[2*N]     complex input signal. Real and imaginary data are interleaved 
             and real data goes first
  bexp       common block exponent, that is the minimum number of redundant
             sign bits over input data x[2*N]
  h          FFT descriptor handle, identifies the transform size N and 
             constant data tables
  Output:
  y[2*N]     output spectrum. Real and imaginary data are interleaved and 
             real data goes first

  Returned value: total right shift amount applied to dynamically scale the data

  Restrictions:
  x[],y[] - must not overlap
  x[],y[] - must be aligned on 8-byte boundary

  PERFORMANCE NOTE:
  none
-------------------------------------------------------------------------*/

int fft_cplx16x16( int16_t * restrict y,
                   int16_t * restrict x,
                   int bexp, fft_handle_t h )
{
  const fft_cplx_x16_descr_t * descr = (const fft_cplx_x16_descr_t*)h;

  ae_int16x4 * restrict Y0;
  ae_int16x4 * restrict Y1;
  ae_int16x4 * restrict Y2;
  ae_int16x4 * restrict Y3;
  ae_int32x2 * restrict W0;
  ae_int32x2 * restrict W1;
  ae_int32x2 * restrict W2;
  ae_int32x2 * restrict W3;

  const ae_int16x4 *          TWD;

  const int16_t * twd;

  int n, N, logN;

  int nsa, shift, shiftSum;
  int isStageNumEven;

  NASSERT_ALIGN8( x );
  NASSERT_ALIGN8( y );

  NASSERT( !( descr->N & ( descr->N - 1 ) ) );

  NASSERT( descr->N >= 16 && descr->N <= 4096 );

  twd  = descr->twd;
  N    = descr->N;
  logN = 30 - NSA( N );

  isStageNumEven = !( ( logN + 1 ) & 2 );

  //----------------------------------------------------------------------------
  // Perform the first stage. We use DIF, all permutations are deferred
  // until the last stage. 

  {
    const ae_int16x4 * X0;
    const ae_int16x4 * X1;
    const ae_int16x4 * X2;
    const ae_int16x4 * X3;

    ae_int16x4 a0, a1, a2, a3;
    ae_int32x2 t0, t1, t2, t3;
    ae_int32x2 p0, p1, p2, p3;

    ae_int32x2 scl;

    ae_f32x2 a00, a01, a02, a03;
    ae_f32x2 a10, a11, a12, a13;
    ae_f32x2 b00, b01, b02, b03;
    ae_f32x2 b10, b11, b12, b13;

    ae_int16x4 tw_12, tw_34, tw_56;

    X0 = (ae_int16x4*)( (uintptr_t)x + 0*N/4*4 );
    X1 = (ae_int16x4*)( (uintptr_t)x + 1*N/4*4 );
    X2 = (ae_int16x4*)( (uintptr_t)x + 2*N/4*4 );
    X3 = (ae_int16x4*)( (uintptr_t)x + 3*N/4*4 );

    W0 = (ae_int32x2*)( (uintptr_t)x + 0*N/4*4 );
    W1 = (ae_int32x2*)( (uintptr_t)x + 1*N/4*4 );
    W2 = (ae_int32x2*)( (uintptr_t)x + 2*N/4*4 );
    W3 = (ae_int32x2*)( (uintptr_t)x + 3*N/4*4 );

    TWD = (ae_int16x4*)twd;

    shiftSum = shift = MAX( 0, 3 - bexp );

    WUR_AE_SAR( 0 );

    // The 16-bit scaling factor is negative for the second to last stages to
    // avoid truncation of 1<<15 to 32767. To ensure correct trasform sign we
    // vary the polarity of the first stage.
    scl = AE_SLAA32S( AE_MOVDA32X2( 1 - 2*isStageNumEven, 0 ), 31 - shift );

    __Pragma( "ymemory( TWD )" );
    __Pragma( "ymemory( X0 )" );
    __Pragma( "ymemory( X1 )" );
    __Pragma( "ymemory( X2 )" );
    __Pragma( "ymemory( X3 )" );
    __Pragma( "loop_count min=2, factor=2" );
    for ( n=0; n<N/(4*2); n++ )
    {
      AE_L16X4_IP( tw_12, TWD, +2*4 );
      AE_L16X4_IP( tw_34, TWD, +2*4 );
      AE_L16X4_IP( tw_56, TWD, +2*4 );

      AE_L16X4_IP( a0, X0, +2*4 );
      AE_L16X4_IP( a1, X1, +2*4 );
      AE_L16X4_IP( a2, X2, +2*4 );
      AE_L16X4_IP( a3, X3, +2*4 );

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
      a03 = AE_MULFC32X16RAS_H( a03, tw_34 );

      //
      // Butterfly 1
      //

      AE_ADDANDSUB32S( b10, b12, a10, a12 );
      AE_ADDANDSUB32S( b11, b13, a13, a11 );

      b13 = AE_MUL32JS( b13 );

      AE_ADDANDSUB32S( a10, a12, b10, b11 );
      AE_ADDANDSUB32S( a11, a13, b12, b13 );

      a11 = AE_MULFC32X16RAS_L( a11, tw_34 );
      a12 = AE_MULFC32X16RAS_H( a12, tw_56 );
      a13 = AE_MULFC32X16RAS_L( a13, tw_56 );

      //
      // 32->16-bit conversion, evaluation of min. NSA, output data storage
      //

      t0 = AE_SEL32_LL( a10, a00 );
      t1 = AE_SEL32_LL( a11, a01 );
      t2 = AE_SEL32_LL( a12, a02 );
      t3 = AE_SEL32_LL( a13, a03 );

      AE_RNG32X2( t0 );
      AE_RNG32X2( t1 );
      AE_RNG32X2( t2 );
      AE_RNG32X2( t3 );

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
      AE_S32X2RNG_IP( p0, W0, +2*4 );
      AE_S32X2RNG_IP( p2, W1, +2*4 );
      AE_S32X2RNG_IP( p1, W2, +2*4 );
      AE_S32X2RNG_IP( p3, W3, +2*4 );
    }

    AE_CALCRNG3();

    nsa = 3 - RUR_AE_SAR();
  }

  //----------------------------------------------------------------------------
  // Perform second through the next to last stages.

  shiftSum += fft_cplx_inner_16x16( x, twd, &nsa, N );

  //----------------------------------------------------------------------------
  // Last stage (radix-4 or radix-2 for odd powers of two) with bit reversal
  // permutation.

  {
    const ae_int32x2 * X0;
    const ae_int32x2 * X1;

    if ( !( logN & 1 ) )
    {
      //
      // Last stage for FFT size an even power of two: radix-4.
      //

      ae_int16x4 a0, a1, a2, a3;
      ae_int32x2 t0, t1, t2, t3;

      ae_int32x2 scl;

      ae_f32x2 a00, a01, a02, a03;
      ae_f32x2 a10, a11, a12, a13;
      ae_f32x2 b00, b01, b02, b03;
      ae_f32x2 b10, b11, b12, b13;

      uint32_t ix = 0;

      X0 = (ae_int32x2*)( (uintptr_t)x + 0*N/2*4 );
      X1 = (ae_int32x2*)( (uintptr_t)x + 1*N/2*4 );

      Y0 = (ae_int16x4*)( (uintptr_t)y + 0*N/4*4 );
      Y1 = (ae_int16x4*)( (uintptr_t)y + 1*N/4*4 );
      Y2 = (ae_int16x4*)( (uintptr_t)y + 2*N/4*4 );
      Y3 = (ae_int16x4*)( (uintptr_t)y + 3*N/4*4 );

      shiftSum += ( shift = MAX( 0, 2 - nsa ) );

      scl = AE_MOVDA32X2( -1L<<(31-shift), 0 );

      __Pragma( "loop_count min=2, factor=2" );
      for ( n=0; n<N/(4*2); n++ )
      {
        AE_L32X2_IP( t0, X0, +2*4 );
        AE_L32X2_IP( t1, X0, +2*4 );
        AE_L32X2_IP( t2, X1, +2*4 );
        AE_L32X2_IP( t3, X1, +2*4 );

        a0 = AE_MOVINT16X4_FROMINT32X2( t0 );
        a1 = AE_MOVINT16X4_FROMINT32X2( t1 );
        a2 = AE_MOVINT16X4_FROMINT32X2( t2 );
        a3 = AE_MOVINT16X4_FROMINT32X2( t3 );

        a00 = AE_MULFC32X16RAS_H( scl, a0 );
        a01 = AE_MULFC32X16RAS_L( scl, a0 );
        a02 = AE_MULFC32X16RAS_H( scl, a1 );
        a03 = AE_MULFC32X16RAS_L( scl, a1 );
        a10 = AE_MULFC32X16RAS_H( scl, a2 );
        a11 = AE_MULFC32X16RAS_L( scl, a2 );
        a12 = AE_MULFC32X16RAS_H( scl, a3 );
        a13 = AE_MULFC32X16RAS_L( scl, a3 );

        //
        // Butterfly 0
        //

        AE_ADDANDSUB32S( b00, b02, a00, a02 );
        AE_ADDANDSUB32S( b01, b03, a01, a03 );

        b03 = AE_SEL32_LH( b03, b03 );

        a00 = AE_ADD32   ( b00, b01 );
        a01 = AE_ADDSUB32( b02, b03 );
        a02 = AE_SUB32   ( b00, b01 );
        a03 = AE_SUBADD32( b02, b03 );

        //
        // Butterfly 1
        //

        AE_ADDANDSUB32S( b10, b12, a10, a12 );
        AE_ADDANDSUB32S( b11, b13, a11, a13 );

        b13 = AE_SEL32_LH( b13, b13 );

        a10 = AE_ADD32   ( b10, b11 );
        a11 = AE_ADDSUB32( b12, b13 );
        a12 = AE_SUB32   ( b10, b11 );
        a13 = AE_SUBADD32( b12, b13 );

        //
        // 32->16-bit conversion, evaluation of min. NSA, output data storage
        //

        AE_RNG32X2( a00 );
        AE_RNG32X2( a10 );

        AE_RNG32X2( a02 );
        AE_RNG32X2( a12 );

        AE_RNG32X2( a01 );
        AE_RNG32X2( a11 );

        AE_RNG32X2( a03 );
        AE_RNG32X2( a13 );

        a0 = AE_ROUND16X4F32SASYM( a00, a10 );
        a1 = AE_ROUND16X4F32SASYM( a01, a11 );
        a2 = AE_ROUND16X4F32SASYM( a02, a12 );
        a3 = AE_ROUND16X4F32SASYM( a03, a13 );

        AE_S16X4_X( a0, Y0, ix );
        AE_S16X4_X( a1, Y1, ix );
        AE_S16X4_X( a2, Y2, ix );
        AE_S16X4_X( a3, Y3, ix );

        ix = AE_ADDBRBA32( ix, 1UL << ( 32 - ( 3 + logN-3 ) ) );
      }
    }
    else
    {
      //
      // Last stage for FFT size an odd power of two: radix-2.
      //

      ae_int16x4 a0, a1;
      ae_int32x2 t0, t1;

      ae_int32x2 scl;

      ae_f32x2 a00, a01, a10, a11;
      ae_f32x2 b00, b01, b10, b11; 

      uint32_t ix = 0;

      X0 = (ae_int32x2*)( (uintptr_t)x + 0*N/2*4 );
      X1 = (ae_int32x2*)( (uintptr_t)x + 1*N/2*4 );
      Y0 = (ae_int16x4*)( (uintptr_t)y + 0*N/2*4 );
      Y1 = (ae_int16x4*)( (uintptr_t)y + 1*N/2*4 );

      shiftSum += ( shift = MAX( 0, 1 - nsa ) );

      scl = AE_MOVDA32X2( -1L<<(31-shift), 0 );

      __Pragma( "loop_count min=8, factor=8" );
      for ( n=0; n<N/(2*2); n++ )
      {
        AE_L32X2_IP( t0, X0, +2*4 );
        AE_L32X2_IP( t1, X1, +2*4 );

        a0 = AE_MOVINT16X4_FROMINT32X2( t0 );
        a1 = AE_MOVINT16X4_FROMINT32X2( t1 );

        a00 = AE_MULFC32X16RAS_H( scl, a0 );
        a01 = AE_MULFC32X16RAS_L( scl, a0 );
        a10 = AE_MULFC32X16RAS_H( scl, a1 );
        a11 = AE_MULFC32X16RAS_L( scl, a1 );

        b00 = AE_ADD32( a00, a01 );
        b01 = AE_SUB32( a00, a01 );

        b10 = AE_ADD32( a10, a11 );
        b11 = AE_SUB32( a10, a11 );

        AE_RNG32X2( b01 );
        AE_RNG32X2( b11 );

        AE_RNG32X2( b00 );
        AE_RNG32X2( b10 );

        a0 = AE_ROUND16X4F32SASYM( b00, b10 );
        a1 = AE_ROUND16X4F32SASYM( b01, b11 );

        AE_S16X4_X( a0, Y0, ix );
        AE_S16X4_X( a1, Y1, ix );

        ix = AE_ADDBRBA32( ix, 1UL << ( 32 - ( 3 + logN-2 ) ) );
      }
    }
  }

  return (shiftSum);

} // fft_cplx16x16()
