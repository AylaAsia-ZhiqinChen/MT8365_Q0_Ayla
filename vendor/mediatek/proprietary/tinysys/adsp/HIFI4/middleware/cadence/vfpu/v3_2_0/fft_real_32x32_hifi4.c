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
    Real-valued forward FFT: 32-bit data, 32-bit twiddle factors
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

/*-------------------------------------------------------------------------
  Real-valued forward FFT of size 2^n, n=5..13.

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
  x[N]          real input signal
  bexp          common block exponent, that is the minimum number of redundant
                sign bits over input data x[N]
  h             FFT descriptor handle, identifies the transform size N and 
                constant data tables
  Output:
  y[2*(N/2+1)]  output spectrum samples, 0th to N/2 taps. Real and imaginary data
                are interleaved and real data goes first

  Returned value: total right shift amount applied to dynamically scale the data

  Restrictions:
  x[],y[] - must not overlap
  x[],y[] - must be aligned on 8-byte boundary

  PERFORMANCE NOTE:
  none
-------------------------------------------------------------------------*/

int fft_real32x32( int32_t * restrict y,
                   int32_t * restrict x,
                   int bexp, fft_handle_t h )
{
  const fft_real_x32_descr_t * descr = (const fft_real_x32_descr_t*)h;

  const ae_int32x2 *          X0;
  const ae_int32x2 *          X1;
        ae_int32x2 * restrict Y0;
        ae_int32x2 * restrict Y1;
  const ae_int32x2 *          SPC;

  const int32_t * twd;
  const int32_t * spc;

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
  // Perform the half-sized complex-valued forward FFT
  //
  // MATLAB code:
  //  y(1:N/2) = fft(x(1:2:N)+1j*x(2:2:N));

  {
    fft_cplx_x32_descr_t fftCplx;

    fftCplx.N   = N/2;
    fftCplx.twd = twd;

    shiftSum = fft_cplx32x32( y, x, bexp, (fft_handle_t)&fftCplx );
  }

  //----------------------------------------------------------------------------
  // Apply the in-place real-to-complex spectrum conversion
  //
  // MATLAB code:
  //  tw = -1j*exp(-2*pi*1j*(0:N/4-1)/N);
  //  a0 = y(1:N/4);
  //  a1 = [y(1),wrev(y(N/4+2:N/2))];
  //  b0 = 1/2*(a0+conj(a1));
  //  b1 = 1/2*(a0-conj(a1)).*tw;
  //  a0 = b0+b1;
  //  a1 = conj(b0-b1);
  //  y(1:N/2+1) = [a0,conj(y(N/4+1)),wrev(a1)];
  //

  {
    ae_int32x2 a0, a1;
    ae_int32x2 b0, b1;
    ae_f32x2   c0, c1;
    ae_int32x2 tw;

    AE_CALCRNG1();

    bexp = 1 - RUR_AE_SAR();

    shiftSum += ( shift = 1 - bexp );

    WUR_AE_SAR( shift + 1 );

    X0 = (const ae_int32x2*)( (uintptr_t)y +       0*8 );
    X1 = (const ae_int32x2*)( (uintptr_t)y + (N/2-1)*8 );
    Y0 = (      ae_int32x2*)( (uintptr_t)y +       0*8 );
    Y1 = (      ae_int32x2*)( (uintptr_t)y +     N/2*8 );

    SPC = (const ae_int32x2*)spc;

    // Real and imaginary components are swapped, that is equivalent to
    // conjugation followed by multiplication by 1j:
    // 1j*conj(a1)
    a1 = AE_L32X2_RI( X0, 0 );

    __Pragma( "ymemory( SPC )" );
    __Pragma( "loop_count min=8, factor=8" );
    for ( n=0; n<N/4; n++ )
    {
      AE_L32X2_IP( tw, SPC, +8 );

      AE_L32X2_IP( a0, X0, +8 );

      // -conj(a1)
      a1 = AE_MUL32JS( a1 );

      // b1 <- 1/2*(a0-conj(a1));
      // b0 <- 1/2*(a0+conj(a1));
      AE_ADDANDSUBRNG32( b1, b0, a0, a1 );

      // b1 <- 1/2*(a0-conj(a1)).*spc
      b1 = AE_MULFC32RAS( b1, tw );

      // c0 <- b0+b1
      // c1 <- b0-b1
      AE_ADDANDSUB32S( c0, c1, b0, b1 );

      // j*(b0-b1)
      c1 = AE_MUL32JS( c1 );

      // b0+b1
      AE_S32X2_IP( c0, Y0, +8 );
      // j*conj(j*(b0-b1)) == conj(b0-b1)
      AE_S32X2_RIP( c1, Y1, -8 );

      // 1j*conj(a1)
      AE_L32X2_RIP( a1, X1, -8 );
    }

    // 1j*conj(y(N/4+1))
    a1 = AE_SRAA32S( a1, shift );
    // -conj(y(N/4+1))
    a1 = AE_MUL32JS( a1 );
    // conj(y(N/4+1))
    a1 = AE_NEG32S( a1 );

    AE_S32X2_I( a1, Y1, 0 );
  }

  return (shiftSum);

} // fft_real32x32()
