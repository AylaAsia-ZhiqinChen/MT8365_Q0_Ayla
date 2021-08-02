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
    Real-valued inverse FFT: 32-bit data, 16-bit twiddle factors
    C code optimized for HiFi4
  IntegrIT, 2006-2014
*/

/* Library API. */
#include "NatureDSP_Signal.h"
/* Common utility macros. */
#include "common.h"
/* Real-valued FFT descriptor structure. */
#include "fft_real_twd.h"
/* Complex-valued FFT descriptor structure and twiddle factor tables. */
#include "fft_cplx_twd.h"

#define MAX(a,b)   ( (a)>(b) ? (a) : (b) )

/*-------------------------------------------------------------------------
  Real-valued inverse FFT of size 2^n, n=5..13.

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
  x[2*(N/2+1)] input spectrum, 0th to N/2 taps. Real and imaginary data are
               interleaved and real data goes first
  bexp         common block exponent, that is the minimum number of redundant
               sign bits over input data x[2*N]
  h            FFT descriptor handle, identifies the transform size N and 
               constant data tables
  Output:
  y[2*N]       real output signal

  Returned value: total right shift amount applied to dynamically scale the data

  Restrictions:
  x[],y[] - must not overlap
  x[],y[] - must be aligned on 8-byte boundary

  PERFORMANCE NOTE:
  none
-------------------------------------------------------------------------*/

int ifft_real32x16( int32_t * restrict y, int32_t * restrict x, int bexp, fft_handle_t h )
{
  const fft_real_x16_descr_t * descr = (const fft_real_x16_descr_t*)h;

  const ae_int32x2 *          X0;
  const ae_int32x2 *          X1;
        ae_int32x2 * restrict Y0;
        ae_int32x2 * restrict Y1;
  const ae_int16x4 *          SPC;

  const int16_t * twd;
  const int16_t * spc;

  int n, N;
  int shift, shiftSum;

  NASSERT_ALIGN8( y );
  NASSERT_ALIGN8( x );

  NASSERT( !( descr->N & ( descr->N - 1 ) ) );

  NASSERT( descr->N >= 32 && descr->N <= 8192 );

  N   = descr->N;
  twd = descr->twd;
  spc = descr->spc;

  //----------------------------------------------------------------------------
  // Apply the in-place real-to-complex spectrum conversion. Here we use a 
  // slightly modified algorithm which avoids direct conjugation of twiddle
  // factors.
  //
  // MATLAB code:
  //  tw = -1j*exp(-2*pi*1j*(0:N/4-1)/N);
  //  a0 = x(1:N/4);
  //  a1 = wrev(x(N/4+2:N/2+1));
  //  b0 = (-conj(a0)-a1);
  //  b1 = (-conj(a0)+a1).*tw;
  //  a0 = -conj(b0+b1);
  //  a1 = -(b0-b1);
  //  x = [a0,2*conj(x(N/4+1)),wrev(a1(2:N/4))];
  //

  {
    ae_int32x2 a0, a1;
    ae_int32x2 b0, b1;
    ae_f32x2   c0, c1;
    ae_int32x2 t0;
    ae_int16x4 tw;

    xtbool4 b4;

    shiftSum = shift = MAX( 0, 2 - bexp );

    WUR_AE_SAR( shift );

    X0 = (const ae_int32x2*)( (uintptr_t)x +   0*8 );
    X1 = (const ae_int32x2*)( (uintptr_t)x + N/2*8 );
    Y0 = (      ae_int32x2*)( (uintptr_t)x +   0*8 );
    Y1 = (      ae_int32x2*)( (uintptr_t)x + N/2*8 );

    SPC = (const ae_int16x4*)spc;

    b4 = AE_MOVBA4( 5 );

    __Pragma( "ymemory( X0 )" );
    __Pragma( "ymemory( X1 )" );
    __Pragma( "ymemory( SPC )" );
    __Pragma( "loop_count min=4, factor=4" );
    for ( n=0; n<N/(4*2); n++ )
    {
      AE_L16X4_IP( tw, SPC, +8 );

      //
      // Group 0
      //

      // Real and imaginary components are swapped, that is equivalent to
      // conjugation followed by multiplication by 1j:
      // 1j*conj(a0)
      AE_L32X2_RIP( a0, X0, +8 );
      // -conj(a0)
      a0 = AE_MUL32JS( a0 );

      // a1
      AE_L32X2_XP( a1, X1, -8 );

      // b0 <- -conj(a0)-a1
      // b1 <- -conj(a0)+a1
      AE_ADDANDSUBRNG32( b1, b0, a0, a1 );

      // b1*tw
      b1 = AE_MULFC32X16RAS_H( b1, tw );

      // c0 <-  (b0+b1)
      // c1 <- -(b0-b1)
      AE_ADDANDSUB32S( c0, c1, b1, b0 );

      t0 = AE_NEG32S( c0 );
      // -conj(b0+b1)
      c0 = AE_SEL32_HL( t0, c0 );

      AE_S32X2RNG_IP( c0, Y0, +8 );
      AE_S32X2RNG_XP( c1, Y1, -8 );

      //
      // Group 1
      //

      // 1j*conj(a0)
      AE_L32X2_RIP( a0, X0, +8 );
      // -conj(a0)
      a0 = AE_MUL32JS( a0 );

      // a1
      AE_L32X2_XP( a1, X1, -8 );

      // b0 <- -conj(a0)-a1
      // b1 <- -conj(a0)+a1
      AE_ADDANDSUBRNG32( b1, b0, a0, a1 );

      // b1*tw
      b1 = AE_MULFC32X16RAS_L( b1, tw );

      // c0 <-  (b0+b1)
      // c1 <- -(b0-b1
      AE_ADDANDSUB32S( c0, c1, b1, b0 );

      t0 = AE_NEG32S( c0 );
      // -conj(b0+b1)
      c0 = AE_SEL32_HL( t0, c0 );

      AE_S32X2RNG_IP( c0, Y0, +8 );
      AE_S32X2RNG_XP( c1, Y1, -8 );
    }

    // 1j*conj(x(N/4+1))
    a0 = AE_L32X2_RI( X0, 0 );
    // 2*1j*conj(x(N/4+1))
    a0 = AE_SRAA32S( a0, shift-1 );
    // -2*1j*conj(x(N/4+1))
    a0 = AE_NEG32S( a0 );
    // 2*conj(x(N/4+1))
    a0 = AE_MUL32JS( a0 );

    AE_S32X2RNG_I( a0, Y0, 0 );

    AE_CALCRNG3();

    bexp = 3 - RUR_AE_SAR();
  }

  //----------------------------------------------------------------------------
  // Pass the converted spectrum to half-sized complex-valued inverse FFT.
  //
  // MATLAB code:
  //  y = N/2*ifft(x);
  //  y = reshape([real(y);imag(y)],1,N);
  //

  {
    fft_cplx_x16_descr_t fftCplx;

    fftCplx.N   = N/2;
    fftCplx.twd = twd;

    shiftSum += ifft_cplx32x16( y, x, bexp, (fft_handle_t)&fftCplx );
  }

  return (shiftSum);

} // ifft_real32x16()
