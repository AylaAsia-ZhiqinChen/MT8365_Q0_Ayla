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
    DCT Type II: 32-bit data, 16-bit twiddle factors
    C code optimized for HiFi4
    IntegrIT, 2006-2014
*/

/* Library API. */
#include "NatureDSP_Signal.h"
/* Common utility macros. */
#include "common.h"
/* Twiddle factor tables for complex-valued FFTs. */
#include "fft_cplx_twd.h"

// Real-to-complex spectrum conversion twiddle factors:
// N = 32;
// spc = [(1+1j)*2^-0.5,-1j*exp(-2*pi*1j*(1:N/4-1)/N)];
static const int16_t ALIGN(8) dct_x16_spc_32[32*1/4*2] = {
  (int16_t)0x5a82,(int16_t)0x5a82,(int16_t)0xe707,(int16_t)0x8276,
  (int16_t)0xcf04,(int16_t)0x89be,(int16_t)0xb8e3,(int16_t)0x9592,
  (int16_t)0xa57e,(int16_t)0xa57e,(int16_t)0x9592,(int16_t)0xb8e3,
  (int16_t)0x89be,(int16_t)0xcf04,(int16_t)0x8276,(int16_t)0xe707
};

// Conjugate-pair split-radix algorithm twiddle factors:
// N = 32;
// spr = reshape([1j*exp(-2*pi*1j*(1:N/4)/(4*N)); ...
//                1j*exp(-2*pi*1j*(N/2-1:-1:N/4)/(4*N))],1,N/2);
static const int16_t ALIGN(8) dct_x16_spr_32[32*1/2*2] = {
  (int16_t)0x0648,(int16_t)0x7fd9,(int16_t)0x55f6,(int16_t)0x5ed7,
  (int16_t)0x0c8c,(int16_t)0x7f62,(int16_t)0x5134,(int16_t)0x62f2,
  (int16_t)0x12c8,(int16_t)0x7e9d,(int16_t)0x4c40,(int16_t)0x66d0,
  (int16_t)0x18f9,(int16_t)0x7d8a,(int16_t)0x471d,(int16_t)0x6a6e,
  (int16_t)0x1f1a,(int16_t)0x7c2a,(int16_t)0x41ce,(int16_t)0x6dca,
  (int16_t)0x2528,(int16_t)0x7a7d,(int16_t)0x3c57,(int16_t)0x70e3,
  (int16_t)0x2b1f,(int16_t)0x7885,(int16_t)0x36ba,(int16_t)0x73b6,
  (int16_t)0x30fc,(int16_t)0x7642,(int16_t)0x30fc,(int16_t)0x7642
};

/*-------------------------------------------------------------------------
  Discrete Cosine Transform
  These functions apply DCT (Type II) to input
  NOTES:
  1. DCT runs in-place algorithm so INPUT DATA WILL APPEAR DAMAGED after 
     the call.
  2. Fixed point DCT routines dynamically downscale data in order to avoid
     overflows, but it is user's responsibility to upscale input data prior to
     DCT computation, otherwise the DCT precision may degrade.

  Precision: 
  32x32  32-bit input/outputs, 32-bit twiddles
  32x16  32-bit input/outputs, 16-bit twiddles
  f      floating point

  Input:
  x[N]          input signal
  N             DCT size
  Output:
  y[N]          transform output
  returned value:
  total number of right shifts occurred during scaling procedure
  (always 0 for floating point function)
  Restriction:
  x,y should not overlap
  x,y - aligned on 8-bytes boundary
  N   - 32 for fixed point routines, 32 or 64 for floating point
-------------------------------------------------------------------------*/

int dct_32x16( int32_t * restrict y, int32_t * restrict x, int N )
{
  //
  // MATLAB reference code:
  //
  //  function y = dct_ref(x)
  //  % Compute DCT Type II by conjugate-pair split-radix algorithm.
  //  % Relates to MATLAB's implementation as dct(x)*(numel(x)/2)^0.5 == dct_ref(x).
  //  N = numel(x);
  //  % Reorder input data as required by the split-radix DCT-II algorithm
  //  x = [x(1:2:N-1)',wrev(x(2:2:N)')];
  //  % Real-valued FFT of size N through the complex-valued FFT of size N/2.
  //  s = fft(x(1:2:N-1)+1j*x(2:2:N));
  //  % Real-to-complex spectrum conversion twiddle factors
  //  spc = -1j*exp(-2*pi*1j*(1:N/4)/N);
  //  % Conjugate-pair split-radix algorithm twiddle factors
  //  spr = reshape([2^-0.5,exp(-2*pi*1j*(1:N/4)/(4*N)); ...
  //                      0,exp(-2*pi*1j*(N/2-1:-1:N/4)/(4*N))],1,N/2+2);
  //  y = [spr(1)*(real(s(1))+imag(s(1)));zeros(N/2-1,1); ...
  //       spr(1)*(real(s(1))-imag(s(1)));zeros(N/2-1,1)];
  //  for n=2:N/4
  //    a0 = s(n);
  //    a1 = s(N/2+2-n);
  //    b0 = 1/2*(a0+conj(a1));
  //    b1 = 1/2*(a0-conj(a1))*spc(n-1);
  //    a0 = (b0+b1)*spr((n-1)*2+1);
  //    a1 = conj(b0-b1)*spr((n-1)*2+2);
  //    y(n) = real(a0);
  //    y(N/2+2-n) = real(a1);
  //    y(N/2+n) = -imag(a1);
  //    y(N+2-n) = -imag(a0);
  //  end
  //  y(N/4+1) = real(conj(s(N/4+1))*spr(N/2+1));
  //  y(3*N/4+1) = -imag(conj(s(N/4+1))*spr(N/2+2));
  //

  const ae_int32x2 *          X0;
  const ae_int32x2 *          X1;
        ae_int32x2 * restrict Y0;
        ae_int32x2 * restrict Y1;
        ae_int32x2 * restrict Y2;
        ae_int32x2 * restrict Y3;
  const ae_int16x4 *          TWD;
  const ae_int16x4 *          SPC;
  const ae_int16x4 *          SPR;

  const int16_t * twd;
  const int16_t * spc;
  const int16_t * spr;

  int shift, shiftSum;

  int n;

  const int _N = 32;

  NASSERT_ALIGN8( y );
  NASSERT_ALIGN8( x );

  NASSERT( N == _N );

  // tw = exp(-2j*pi*[1;2;3]*(0:N/8-1)/(N/2));
  twd = fft_cplx_x16_twd_16;
  // spc = [2^-0.5,-1j*exp(-2*pi*1j*(1:N/4-1)/N)];
  spc = dct_x16_spc_32;
  // spr = reshape([1j*exp(-2*pi*1j*(1:N/4)/(4*N)); ...
  //                1j*exp(-2*pi*1j*(N/2-1:-1:N/4)/(4*N))],1,N/2);
  spr = dct_x16_spr_32;

  //----------------------------------------------------------------------------
  // Reorder input data as required by the split-radix DCT Type II algorithm.
  // Just in case, estimate the normalization shift amount.
  //

  {
    ae_int32x2 a0, a1, a2, a3;
    ae_int32x2 b0, b1, b2, b3;

    X0 = (const ae_int32x2*)x;

    WUR_AE_SAR( 0 );

    Y0 = (ae_int32x2*)( (uintptr_t)y +      0*4 );
    Y1 = (ae_int32x2*)( (uintptr_t)y + (_N-2)*4 );

    for ( n=0; n<_N/8; n++ )
    {
      AE_L32X2_IP( a0, X0, +8 );
      AE_L32X2_IP( a1, X0, +8 );
      AE_L32X2_IP( a2, X0, +8 );
      AE_L32X2_IP( a3, X0, +8 );

      b0 = AE_SEL32_HH( a0, a1 );
      b1 = AE_SEL32_HH( a2, a3 );
      b2 = AE_SEL32_LL( a1, a0 );
      b3 = AE_SEL32_LL( a3, a2 );

      AE_S32X2RNG_IP( b0, Y0, +8 );
      AE_S32X2RNG_IP( b1, Y0, +8 );

      AE_S32X2RNG_XP( b2, Y1, -8 );
      AE_S32X2RNG_XP( b3, Y1, -8 );
    }
  }

  __Pragma( "no_reorder" );

  //----------------------------------------------------------------------------
  // Apply the complex-valued FFT of size N/2.
  //

  //
  // First radix-4 stage
  //

  {
    ae_int32x2 a0, a1, a2, a3;
    ae_int32x2 b0, b1, b2, b3;
    ae_f32x2   c0, c1, c2, c3;

    ae_int16x4 tw1, tw2, tw3;

    AE_CALCRNG3();

    shiftSum = RUR_AE_SAR();

    X0 = (const ae_int32x2*)y;

    Y0 = (ae_int32x2*)( (uintptr_t)y + 0*4*8 );
    Y1 = (ae_int32x2*)( (uintptr_t)y + 1*4*8 );
    Y2 = (ae_int32x2*)( (uintptr_t)y + 2*4*8 );
    Y3 = (ae_int32x2*)( (uintptr_t)y + 3*4*8 );

    TWD = (const ae_int16x4*)twd;

    for ( n=0; n<_N/16; n++ )
    {
      tw2 = AE_L16X4_I( TWD, 1*2*4 );
      tw3 = AE_L16X4_I( TWD, 2*2*4 );
      AE_L16X4_IP( tw1, TWD, 3*2*4 );

      //---------------------------------------------

      AE_L32X2_IP( a0, X0,  +4*8 );
      AE_L32X2_IP( a1, X0,  +4*8 );
      AE_L32X2_IP( a2, X0,  +4*8 );
      AE_L32X2_XP( a3, X0, -11*8 );

      AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
      AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

      b3 = AE_MUL32JS( b3 );

      AE_ADDANDSUB32S( c0, c2, b0, b1 );
      AE_ADDANDSUB32S( c1, c3, b2, b3 );

      c1 = AE_MULFC32X16RAS_H( c1, tw1 );
      c2 = AE_MULFC32X16RAS_L( c2, tw1 );
      c3 = AE_MULFC32X16RAS_H( c3, tw2 );

      AE_S32X2RNG_IP( c0, Y0, +8 );
      AE_S32X2RNG_IP( c1, Y1, +8 );
      AE_S32X2RNG_IP( c2, Y2, +8 );
      AE_S32X2RNG_IP( c3, Y3, +8 );

      //---------------------------------------------

      AE_L32X2_IP( a0, X0,  +4*8 );
      AE_L32X2_IP( a1, X0,  +4*8 );
      AE_L32X2_IP( a2, X0,  +4*8 );
      AE_L32X2_XP( a3, X0, -11*8 );

      AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
      AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

      b3 = AE_MUL32JS( b3 );

      AE_ADDANDSUB32S( c0, c2, b0, b1 );
      AE_ADDANDSUB32S( c1, c3, b2, b3 );

      c1 = AE_MULFC32X16RAS_L( c1, tw2 );
      c2 = AE_MULFC32X16RAS_H( c2, tw3 );
      c3 = AE_MULFC32X16RAS_L( c3, tw3 );

      AE_S32X2RNG_IP( c0, Y0, +8 );
      AE_S32X2RNG_IP( c1, Y1, +8 );
      AE_S32X2RNG_IP( c2, Y2, +8 );
      AE_S32X2RNG_IP( c3, Y3, +8 );
    }
  }

  __Pragma( "no_reorder" );

  //
  // Second radix-4 stage
  //

  {
    ae_int32x2 a0, a1, a2, a3;
    ae_int32x2 b0, b1, b2, b3;
    ae_f32x2   c0, c1, c2, c3;

    AE_CALCRNG2();

    shiftSum += RUR_AE_SAR();

    X0 = (const ae_int32x2*)y;
    Y0 = (      ae_int32x2*)x;

    for ( n=0; n<_N/8; n++ )
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

      AE_S32X2RNG_IP( c0, Y0,  +4*8 );
      AE_S32X2RNG_IP( c1, Y0,  +4*8 );
      AE_S32X2RNG_IP( c2, Y0,  +4*8 );
      AE_S32X2RNG_XP( c3, Y0, -11*8 );
    }
  }

  __Pragma( "no_reorder" );

  //----------------------------------------------------------------------------
  // Apply the real-to-complex conversion to obtain the spectrum of reordered
  // real signal, and perform the conjugate-pair split-radix DCT Type II
  //

  {
    ae_int32x2 a0, a1, b0, b1;
    ae_f32x2   c0, c1, c2;
    ae_int32x2 d0;

    ae_int16x4 tw0, tw1, tw2;

    AE_CALCRNG1();

    shiftSum += ( shift = RUR_AE_SAR() );

    WUR_AE_SAR( shift + 1 );

    X0 = (const ae_int32x2*)( (uintptr_t)x +        0*8 );
    X1 = (const ae_int32x2*)( (uintptr_t)x + (_N/2-1)*8 );
    Y0 = (      ae_int32x2*)( (uintptr_t)y +        0*4 );
    Y1 = (      ae_int32x2*)( (uintptr_t)y + (_N/2-2)*4 );

    SPC = (const ae_int16x4*)spc;
    SPR = (const ae_int16x4*)spr;

    //
    // y(1)     =        (real(s(1))+imag(s(1)))
    // y(N/2+1) = spc(1)*(real(s(1))-imag(s(1)))
    //

    AE_L32X2_IP( a0, X0, +8 );

    AE_L16X4_IP( tw0, SPC, +2*4 );

    a0 = AE_SRAA32S( a0, shift );
    a1 = AE_SEL32_LH( a0, a0 );
    b0 = AE_ADDSUB32S( a1, a0 );
    d0 = AE_MULFP32X16X2RAS_H( b0, tw0 );
    d0 = AE_SEL32_HL( b0, d0 );

    //-------------------------------------------------

    AE_L16X4_IP( tw1, SPR, +2*4 );

    // a0 = s(n);
    AE_L32X2_IP( a0, X0, +8 );
    // a1 = s(N/2+2-n);
    // Real and imaginary components are swapped, that is equivalent to
    // conjugation followed by multiplication by 1j:
    // 1j*conj(a1);
    AE_L32X2_RIP( a1, X1, -8 );

    // -conj(a1)
    a1 = AE_MUL32JS( a1 );
    // b0 = 1/2*(a0+conj(a1));
    // b1 = 1/2*(a0-conj(a1));
    AE_ADDANDSUBRNG32( b1, b0, a0, a1 );
    // b1 = b1*spc(n);
    b1 = AE_MULFC32X16RAS_L( b1, tw0 );

    // b0+b1, b0-b1
    AE_ADDANDSUB32S( c0, c1, b0, b1 );

    // j*(b0-b1)
    c1 = AE_MUL32JS( c1 );
    // conj(b0-b1)
    c1 = AE_SEL32_LH( c1, c1 );

    // a0 = (b0+b1)*1j*spr((n-2)*2+1);
    a0 = AE_MULFC32X16RAS_H( c0, tw1 );
    // a1 = conj(b0-b1)*1j*spr((n-2)*2+2);
    a1 = AE_MULFC32X16RAS_L( c1, tw1 );

    b0 = AE_SEL32_HL( d0, a0 );
    b1 = AE_SEL32_LH( d0, a1 );

    d0 = AE_SEL32_LH( a1, a0 ); // H -> Y1 L -> Y3

    AE_S32X2_X ( b1, Y0, _N/2*4 );
    AE_S32X2_IP( b0, Y0,   +2*4 );

    for ( n=1; n<_N/8; n++ )
    {
      AE_L16X4_IP( tw0, SPC, +2*4 );

      AE_L16X4_IP( tw1, SPR, +2*4 );
      AE_L16X4_IP( tw2, SPR, +2*4 );

      //-------------------------------------------------

      // a0 = s(n);
      AE_L32X2_IP( a0, X0, +8 );
      // a1 = s(N/2+2-n);
      // 1j*conj(a1);
      AE_L32X2_RIP( a1, X1, -8 );

      // -conj(a1)
      a1 = AE_MUL32JS( a1 );
      // b0 = 1/2*(a0+conj(a1));
      // b1 = 1/2*(a0-conj(a1));
      AE_ADDANDSUBRNG32( b1, b0, a0, a1 );
      // b1 = b1*spc(n);
      b1 = AE_MULFC32X16RAS_H( b1, tw0 );

      // b0+b1, b0-b1
      AE_ADDANDSUB32S( c0, c1, b0, b1 );

      c2 = AE_NEG32S( c1 );
      // conj(b0-b1)
      c1 = AE_SEL32_HL( c1, c2 );

      // a0 = (b0+b1)*1j*spr((n-2)*2+1);
      // a1 = conj(b0-b1)*1j*spr((n-2)*2+2);
      a0 = AE_MULFC32X16RAS_H( c0, tw1 );
      a1 = AE_MULFC32X16RAS_L( c1, tw1 );

      b0 = AE_SEL32_LH( a1, d0 );
      b1 = AE_SEL32_HL( a0, d0 );

      d0 = AE_SEL32_LH( a0, a1 ); // H -> Y0, L -> Y2

      AE_S32X2_X ( b1, Y1, _N/2*4 );
      AE_S32X2_XP( b0, Y1,   -2*4 );

      //-------------------------------------------------

      // a0 = s(n);
      AE_L32X2_IP( a0, X0, +8 );
      // a1 = s(N/2+2-n);
      // 1j*conj(a1);
      AE_L32X2_RIP( a1, X1, -8 );

      // -conj(a1)
      a1 = AE_MUL32JS( a1 );
      // b0 = 1/2*(a0+conj(a1));
      // b1 = 1/2*(a0-conj(a1));
      AE_ADDANDSUBRNG32( b1, b0, a0, a1 );
      // b1 = b1*spc(n);
      b1 = AE_MULFC32X16RAS_L( b1, tw0 );

      // b0+b1, b0-b1
      AE_ADDANDSUB32S( c0, c1, b0, b1 );

      c2 = AE_NEG32S( c1 );
      // conj(b0-b1)
      c1 = AE_SEL32_HL( c1, c2 );

      // a0 = (b0+b1)*1j*spr((n-2)*2+1);
      // a1 = conj(b0-b1)*1j*spr((n-2)*2+2);
      a0 = AE_MULFC32X16RAS_H( c0, tw2 );
      a1 = AE_MULFC32X16RAS_L( c1, tw2 );

      b0 = AE_SEL32_HL( d0, a0 );
      b1 = AE_SEL32_LH( d0, a1 );

      d0 = AE_SEL32_LH( a1, a0 ); // H -> Y1, L -> Y3

      AE_S32X2_X ( b1, Y0, _N/2*4 );
      AE_S32X2_IP( b0, Y0,   +2*4 );
    }

    //
    // y(N/4+1) = real(conj(s(N/4+1))*spr(N/2-1));
    // y(3*N/4+1) = -imag(conj(s(N/4+1))*spr(N/2));
    //

    // spr(N/2-1) and spr(N/2) are actually identical
    // conj(spr(N/2-1))
    tw1 = AE_L16X4_RI( SPR, 0 );
    // x[2*N/4+0]
    a0 = AE_L32X2_I( X0, 0 );
    a0 = AE_SRAA32S( a0, shift );
    // x[2*N/4+0]*conj(spr(N/2-1))
    a0 = AE_MULFC32X16RAS_L( a0, tw1 );

    b0 = AE_SEL32_HH( a0, d0 );
    b1 = AE_SEL32_LL( a0, d0 );

    AE_S32X2_X( b1, Y1, _N/2*4 );
    AE_S32X2_I( b0, Y1,      0 );
  }

  return (shiftSum);

} // dct_32x16()
