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
    Complex-valued inverse FFT: 32-bit data, 16-bit twiddle factors
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

#define MAX(a,b)  ( (a)>(b) ? (a) : (b) )

/*-------------------------------------------------------------------------
  Complex-valued inverse FFT of size 2^n, n=4..12.

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
  x[2*N]     input spectrum. Real and imaginary data are interleaved 
             and real data goes first
  bexp       common block exponent, that is the minimum number of redundant
             sign bits over input data x[2*N]
  h          FFT descriptor handle, identifies the transform size N and 
             constant data tables
  Output:
  y[2*N]     complex output signal. Real and imaginary data are interleaved and 
             real data goes first

  Returned value: total right shift amount applied to dynamically scale the data

  Restrictions:
  x[],y[] - must not overlap
  x[],y[] - must be aligned on 8-byte boundary

  PERFORMANCE NOTE:
  none
-------------------------------------------------------------------------*/

int ifft_cplx32x16( int32_t * restrict y,
                    int32_t * restrict x,
                    int bexp, fft_handle_t h )
{
  const fft_cplx_x16_descr_t * descr = (const fft_cplx_x16_descr_t*)h;

  const ae_int32x2 *          X0;
  const ae_int32x2 *          X1;
  const ae_int32x2 *          X2;
  const ae_int32x2 *          X3;
        ae_int32x2 * restrict Y0;
        ae_int32x2 * restrict Y1;
        ae_int32x2 * restrict Y2;
        ae_int32x2 * restrict Y3;
  const ae_f16x4   *          TWD;

  const int16_t * twd;

  int n, N, logN;
  int nsa, shift, shiftSum;

  NASSERT_ALIGN8( x );
  NASSERT_ALIGN8( y );

  NASSERT( !( descr->N & ( descr->N - 1 ) ) );

  NASSERT( descr->N >= 16 && descr->N <= 4096 );

  twd  = descr->twd;
  N    = descr->N;
  logN = 30 - NSA( N );

  //----------------------------------------------------------------------------
  // Perform the first stage. We use DIF, all permutations are deferred
  // until the last stage. 

  {
    ae_int32x2 a00, a01, a02, a03;
    ae_int32x2 a10, a11, a12, a13;
    ae_int32x2 b00, b01, b02, b03;
    ae_int32x2 b10, b11, b12, b13;

    ae_f16x4 tw_12, tw_34, tw_56;

    shift = ( MAX( 0, 3-bexp ) + 1 )/2;

    shiftSum = 2*shift;

    WUR_AE_SAR( shift );

    X0 = (ae_int32x2*)( (uintptr_t)x + 0*N/4*8 );
    X1 = (ae_int32x2*)( (uintptr_t)x + 1*N/4*8 );
    X2 = (ae_int32x2*)( (uintptr_t)x + 2*N/4*8 );
    X3 = (ae_int32x2*)( (uintptr_t)x + 3*N/4*8 );

    Y0 = (ae_int32x2*)( (uintptr_t)x + 0*N/4*8 );
    Y1 = (ae_int32x2*)( (uintptr_t)x + 1*N/4*8 );
    Y2 = (ae_int32x2*)( (uintptr_t)x + 2*N/4*8 );
    Y3 = (ae_int32x2*)( (uintptr_t)x + 3*N/4*8 );

    TWD = (ae_f16x4*)twd;

    __Pragma( "ymemory( X0 )" );
    __Pragma( "ymemory( X1 )" );
    __Pragma( "ymemory( X2 )" );
    __Pragma( "ymemory( X3 )" );
    __Pragma( "loop_count min=1" );
    for ( n=0; n<N/(4*2); n++ )
    {
      tw_34 = ae_f16x4_loadi( TWD, 1*8 );
      tw_56 = ae_f16x4_loadi( TWD, 2*8 );

      ae_f16x4_loadip( tw_12, TWD, +3*8 );

      AE_L32X2_IP( a00, X0, +8 );
      AE_L32X2_IP( a01, X1, +8 );
      AE_L32X2_IP( a02, X2, +8 );
      AE_L32X2_IP( a03, X3, +8 );

      AE_L32X2_IP( a10, X0, +8 );
      AE_L32X2_IP( a11, X1, +8 );
      AE_L32X2_IP( a12, X2, +8 );
      AE_L32X2_IP( a13, X3, +8 );

      //
      // Real and imaginary parts of input data are swapped on the first and
      // last stages to inverse the FFT:
      //   conj(x) == -j*swap(x) =>
      //   ifft(x) == conj(fft(conj(x)) == swap(fft(swap(x)))
      //

      a00 = AE_SEL32_LH( a00, a00 );
      a01 = AE_SEL32_LH( a01, a01 );
      a02 = AE_SEL32_LH( a02, a02 );
      a03 = AE_SEL32_LH( a03, a03 );

      a10 = AE_SEL32_LH( a10, a10 );
      a11 = AE_SEL32_LH( a11, a11 );
      a12 = AE_SEL32_LH( a12, a12 );
      a13 = AE_SEL32_LH( a13, a13 );

      //
      // Butterfly 0
      //

      AE_ADDANDSUBRNG32( b00, b02, a00, a02 );
      AE_ADDANDSUBRNG32( b01, b03, a03, a01 );

      b03 = AE_MUL32JS( b03 );

      AE_ADDANDSUBRNG32( a00, a02, b00, b01 );
      AE_ADDANDSUBRNG32( a01, a03, b02, b03 );

      a01 = AE_MULFC32X16RAS_H( a01, tw_12 );
      a02 = AE_MULFC32X16RAS_L( a02, tw_12 );
      a03 = AE_MULFC32X16RAS_H( a03, tw_34 );

      //
      // Butterfly 1
      //

      AE_ADDANDSUBRNG32( b10, b12, a10, a12 );
      AE_ADDANDSUBRNG32( b11, b13, a13, a11 );

      b13 = AE_MUL32JS( b13 );

      AE_ADDANDSUBRNG32( a10, a12, b10, b11 );
      AE_ADDANDSUBRNG32( a11, a13, b12, b13 );

      a11 = AE_MULFC32X16RAS_L( a11, tw_34 );
      a12 = AE_MULFC32X16RAS_H( a12, tw_56 );
      a13 = AE_MULFC32X16RAS_L( a13, tw_56 );

      // Two middle quartiles are swapped to use bit reversal instead of
      // digit reversal at the last stage.
      AE_S32X2RNG_IP( a00, Y0, +8 );
      AE_S32X2RNG_IP( a02, Y1, +8 );
      AE_S32X2RNG_IP( a01, Y2, +8 );
      AE_S32X2RNG_IP( a03, Y3, +8 );
                           
      AE_S32X2RNG_IP( a10, Y0, +8 );
      AE_S32X2RNG_IP( a12, Y1, +8 );
      AE_S32X2RNG_IP( a11, Y2, +8 );
      AE_S32X2RNG_IP( a13, Y3, +8 );
    }

    AE_CALCRNG3();

    nsa = 3 - RUR_AE_SAR();
  }

  //----------------------------------------------------------------------------
  // Perform second through the next to last stages.

  shiftSum += fft_cplx_inner_32x16( x, y, twd, &nsa, N );

  //----------------------------------------------------------------------------
  // Last stage (radix-4 or radix-2 for odd powers of two) with bit reversal
  // permutation.

  if ( !( logN & 1 ) )
  {
    //
    // Last stage for FFT size an even power of two: radix-4.
    //

    ae_int32x2 a0, a1, a2, a3;
    ae_int32x2 b0, b1, b2, b3;
    ae_f32x2   c0, c1, c2, c3;

    uint32_t ix = 0;

    shiftSum += ( shift = MAX( 0, 2-nsa ) );

    WUR_AE_SAR( shift );

    X0 = (ae_int32x2*)x;

    Y0 = (ae_int32x2*)( (uintptr_t)y + 0*N/4*8 );
    Y1 = (ae_int32x2*)( (uintptr_t)y + 1*N/4*8 );
    Y2 = (ae_int32x2*)( (uintptr_t)y + 2*N/4*8 );
    Y3 = (ae_int32x2*)( (uintptr_t)y + 3*N/4*8 );

    __Pragma( "loop_count min=4, factor=4" );
    for ( n=0; n<N/4; n++ )
    {
      AE_L32X2_IP( a0, X0, +8 );
      AE_L32X2_IP( a1, X0, +8 );
      AE_L32X2_IP( a2, X0, +8 );
      AE_L32X2_IP( a3, X0, +8 );

      AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
      AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

      b3 = AE_MUL32JS( b3 );

      AE_ADDANDSUB32S( c0, c2, b0, b1 );
      AE_ADDANDSUB32S( c1, c3, b2, b3 );

      c0 = AE_SEL32_LH( c0, c0 );
      c1 = AE_SEL32_LH( c1, c1 );
      c2 = AE_SEL32_LH( c2, c2 );
      c3 = AE_SEL32_LH( c3, c3 );

      AE_S32X2RNG_X( c0, Y0, ix );
      AE_S32X2RNG_X( c1, Y1, ix );
      AE_S32X2RNG_X( c2, Y2, ix );
      AE_S32X2RNG_X( c3, Y3, ix );

      ix = AE_ADDBRBA32( ix, 1UL << ( 32 - ( 3 + logN-2 ) ) );
    }
  }
  else
  {
    //
    // Last stage for FFT size an odd power of two: radix-2.
    //

    ae_int32x2 a0, a1, b0, b1;

    uint32_t ix = 0;

    shiftSum += ( shift = MAX( 0, 1-nsa ) );

    WUR_AE_SAR( shift );

    X0 = (ae_int32x2*)x;

    Y0 = (ae_int32x2*)( (uintptr_t)y + 0*N/2*8 );
    Y1 = (ae_int32x2*)( (uintptr_t)y + 1*N/2*8 );

    __Pragma( "loop_count min=16, factor=16" );
    for ( n=0; n<N/2; n++ )
    {
      AE_L32X2_IP( a0, X0, +8 );
      AE_L32X2_IP( a1, X0, +8 );

      AE_ADDANDSUBRNG32( b0, b1, a0, a1 );

      b0 = AE_SEL32_LH( b0, b0 );
      b1 = AE_SEL32_LH( b1, b1 );

      AE_S32X2RNG_X( b0, Y0, ix );
      AE_S32X2RNG_X( b1, Y1, ix );

      ix = AE_ADDBRBA32( ix, 1UL << ( 32 - ( 3 + logN-1 ) ) );
    }
  }

  return (shiftSum);

} // ifft_cplx32x16()
