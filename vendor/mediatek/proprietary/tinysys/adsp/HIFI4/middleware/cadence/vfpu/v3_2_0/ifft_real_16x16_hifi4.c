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
    Real-valued inverse FFT: 16-bit data, 16-bit twiddle factors
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

int ifft_real16x16( int16_t * restrict y, int16_t * restrict x, int bexp, fft_handle_t h )
{
  const fft_real_x16_descr_t * descr = (const fft_real_x16_descr_t*)h;

  const ae_int16x4 *          X0;
  const ae_int16x4 *          X1;
        ae_int16x4 * restrict Y0;
        ae_int16x4 * restrict Y1;
  const ae_int16x4 *          SPC;

  ae_valign X1_va, Y1_va;

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
  // Apply the in-place real-to-complex spectrum conversion.
  //
  // MATLAB code:
  //  tw = -1j*exp(-2*pi*1j*(0:N/4-1)/N);
  //  a0 = x(1:N/4);
  //  a1 = wrev(x(N/4+2:N/2+1));
  //  b0 = a0+conj(a1);
  //  b1 = (a0-conj(a1)).*conj(tw);
  //  a0 = b0+b1;
  //  a1 = conj(b0-b1);
  //  x = [a0,2*conj(x(N/4+1)),wrev(a1(2:N/4))];
  //

  {
    ae_f32x2   a00, a01, a10, a11;
    ae_f32x2   b00, b01, b10, b11;
    ae_int16x4 t0, t1;

    ae_int16x4 tw;
    ae_int32x2 scl;

    xtbool4 b4;

    shiftSum = shift = MAX( 0, 2 - bexp );

    scl = AE_SLAA32S( 1, 31 - shift );

    WUR_AE_SAR( 0 );

    X0 = (const ae_int16x4*)( (uintptr_t)x +         0*2 );
    X1 = (const ae_int16x4*)( (uintptr_t)x + (2*N/2+1)*2 );
    Y0 = (      ae_int16x4*)( (uintptr_t)x +         0*2 );
    Y1 = (      ae_int16x4*)( (uintptr_t)x + (2*N/2+1)*2 );

    SPC = (const ae_int16x4*)spc;

    X1_va = AE_LA64_PP( X1 );
    Y1_va = AE_ZALIGN64();

    b4 = AE_MOVBA4( 5 );

    __Pragma( "loop_count min=4, factor=4" );
    for ( n=0; n<N/(4*2); n++ )
    {
      // tw0, tw1
      AE_L16X4_IP( tw, SPC, +2*4 );

      t0 = AE_NEG16S( tw );
      // conj(tw0), conj(tw1)
      AE_MOVT16X4( tw, t0, b4 );

      // a00, a01
      AE_L16X4_IP( t0, X0, +2*4 );
      // 1j*conj(a10), 1j*conj(a11)
      AE_LA16X4_RIP( t1, X1_va, X1 );

      // Real and imaginary components are swapped, that is equivalent to
      // conjugation followed by multiplication by 1j:
      // a11, a10
      t1 = AE_SHORTSWAP( t1 );

      a00 = AE_MULFP32X16X2S_H( scl, t0 );
      a01 = AE_MULFP32X16X2S_L( scl, t0 );
      a11 = AE_MULFP32X16X2S_H( scl, t1 );
      a10 = AE_MULFP32X16X2S_L( scl, t1 );

      // b0x <- a0x + conj(a1x)
      b00 = AE_ADDSUB32S( a00, a10 );
      b01 = AE_ADDSUB32S( a01, a11 );
      // b1x <- a0x - conj(a1x)
      b10 = AE_SUBADD32S( a00, a10 );
      b11 = AE_SUBADD32S( a01, a11 );

      // b1x*conj(twx)
      b10 = AE_MULFC32X16RAS_H( b10, tw );
      b11 = AE_MULFC32X16RAS_L( b11, tw );

      // a0x <- b0x + b1x
      // a1x <- b0x - b1x
      AE_ADDANDSUB32S( a00, a10, b00, b10 );
      AE_ADDANDSUB32S( a01, a11, b01, b11 );

      // 1j*(b0x-b1x)
      a10 = AE_MUL32JS( a10 );
      a11 = AE_MUL32JS( a11 );

      AE_RNG32X2( a00 );
      AE_RNG32X2( a01 );

      AE_RNG32X2( a10 );
      AE_RNG32X2( a11 );

      // b00 + b10, b01 + b11
      t0 = AE_ROUND16X4F32SASYM( a00, a01 );
      // conj(b00 - b10), conj(b01 - b11)
      t1 = AE_ROUND16X4F32SASYM( a10, a11 );

      AE_S16X4_IP( t0, Y0, +2*4 );
      // conj(b01 - b11), conj(b00 - b10)
      AE_SA16X4_RIP( t1, Y1_va, Y1 );
    }

    AE_SA64NEG_FP( Y1_va, Y1 );

    __Pragma( "no_reorder" );

    //
    // x(N/4+1) = 2*conj(x(N/4+1));
    //

    t0 = AE_L16X4_I( X0, 0 );

    a00 = AE_MULFP32X16X2S_H( scl, t0 );
    a00 = AE_SLAI32S( a00, 1 );

    b00 = AE_SEL32_LH( 0, a00 ); // re
    b01 = AE_SEL32_LL( 0, a00 ); // im

    b01 = AE_NEG32S( b01 );

    AE_RNG32X2( b00 );
    AE_RNG32X2( b01 );

    t0 = AE_ROUND16X4F32SASYM( b00, b00 );
    t1 = AE_ROUND16X4F32SASYM( b01, b01 );

    AE_S16_0_I( t0, (ae_int16*)Y0, 0 );
    AE_S16_0_I( t1, (ae_int16*)Y0, 2 );

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

    shiftSum += ifft_cplx16x16( y, x, bexp, (fft_handle_t)&fftCplx );
  }

  return (shiftSum);

} // ifft_real16x16()
