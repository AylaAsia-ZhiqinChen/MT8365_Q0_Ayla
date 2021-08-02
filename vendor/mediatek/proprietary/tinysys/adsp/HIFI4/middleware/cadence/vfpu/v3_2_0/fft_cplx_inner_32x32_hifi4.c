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
    Inner stages of complex-valued FFT: 32-bit data, 32-bit twiddle factors
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

int fft_cplx_inner_32x32( int32_t * restrict y,
                          int32_t * restrict x,
                    const int32_t *          twd,
                    int * bexp, int N )
{
  const ae_int32x2 *          X0;
  const ae_int32x2 *          X1;
  const ae_int32x2 *          X2;
  const ae_int32x2 *          X3;
        ae_int32x2 * restrict Y0;
        ae_int32x2 * restrict Y1;
        ae_int32x2 * restrict Y2;
        ae_int32x2 * restrict Y3;
  const ae_f32x2   *          TWD;

  int m, n, logN;
  int step, stride;
  int nsa, shift, shiftSum;

  NASSERT_ALIGN8( y   );
  NASSERT_ALIGN8( x   );
  NASSERT_ALIGN8( twd );

  NASSERT( N >= 16 && N <= 4096 );

  NASSERT( 0 <= *bexp && *bexp <= 3 );

  logN = 30 - NSA( N );

  nsa = *bexp;

  shiftSum = 0;

  //----------------------------------------------------------------------------
  // Perform second through the second to the last stages.

  {
    int32_t * X;
    int32_t * Y;

    ae_int32x2 a0, a1, a2, a3;
    ae_int32x2 b0, b1, b2, b3;
    ae_f32x2   c0, c1, c2, c3;

    ae_f32x2 tw1, tw2, tw3;

    X = ( ( ( logN + 1 ) & 2 ) ? x : y );
    Y = ( ( ( logN + 1 ) & 2 ) ? y : x );

    for ( stride=N/16, step=4; stride>4; stride/=4, step*=4 )
    {
      shiftSum += ( shift = 3 - nsa );

      WUR_AE_SAR( shift );

      X0 = (ae_int32x2*)( (uintptr_t)X + 0*stride*8 );
      X1 = (ae_int32x2*)( (uintptr_t)X + 1*stride*8 );
      X2 = (ae_int32x2*)( (uintptr_t)X + 2*stride*8 );
      X3 = (ae_int32x2*)( (uintptr_t)X + 3*stride*8 );

      Y0 = (ae_int32x2*)( (uintptr_t)Y + 0*stride*8 );
      Y1 = (ae_int32x2*)( (uintptr_t)Y + 1*stride*8 );
      Y2 = (ae_int32x2*)( (uintptr_t)Y + 2*stride*8 );
      Y3 = (ae_int32x2*)( (uintptr_t)Y + 3*stride*8 );

      __Pragma( "loop_count min=1" );
      for ( m=0; m*(4*4*stride)<N; m++ )
      {
        TWD = (ae_f32x2*)twd;

        __Pragma( "ymemory( X0 )" );
        __Pragma( "ymemory( X1 )" );
        __Pragma( "ymemory( X2 )" );
        __Pragma( "ymemory( X3 )" );
        __Pragma( "loop_count min=4, factor=4" );
        for ( n=0; n<stride; n++ )
        {
          tw2 = ae_f32x2_loadi( TWD,       1*8 );
          tw3 = ae_f32x2_loadi( TWD,       2*8 );
          ae_f32x2_loadxp( tw1, TWD, +3*step*8 );

          //
          // Group 0
          //

          AE_L32X2_XP( a0, X0, +4*stride*8 );
          AE_L32X2_XP( a1, X1, +4*stride*8 );
          AE_L32X2_XP( a2, X2, +4*stride*8 );
          AE_L32X2_XP( a3, X3, +4*stride*8 );

          AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
          AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

          b3 = AE_MUL32JS( b3 );

          AE_ADDANDSUB32S( c0, c2, b0, b1 );
          AE_ADDANDSUB32S( c1, c3, b2, b3 );

          c1 = AE_MULFC32RAS( c1, tw1 );
          c2 = AE_MULFC32RAS( c2, tw2 );
          c3 = AE_MULFC32RAS( c3, tw3 );

          AE_S32X2RNG_XP( c0, Y0, +4*stride*8 );
          AE_S32X2RNG_XP( c2, Y1, +4*stride*8 );
          AE_S32X2RNG_XP( c1, Y2, +4*stride*8 );
          AE_S32X2RNG_XP( c3, Y3, +4*stride*8 );

          //
          // Group 1
          //

          AE_L32X2_XP( a0, X0, +4*stride*8 );
          AE_L32X2_XP( a1, X1, +4*stride*8 );
          AE_L32X2_XP( a2, X2, +4*stride*8 );
          AE_L32X2_XP( a3, X3, +4*stride*8 );

          AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
          AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

          b3 = AE_MUL32JS( b3 );

          AE_ADDANDSUB32S( c0, c2, b0, b1 );
          AE_ADDANDSUB32S( c1, c3, b2, b3 );

          c1 = AE_MULFC32RAS( c1, tw1 );
          c2 = AE_MULFC32RAS( c2, tw2 );
          c3 = AE_MULFC32RAS( c3, tw3 );

          AE_S32X2RNG_XP( c0, Y0, +4*stride*8 );
          AE_S32X2RNG_XP( c2, Y1, +4*stride*8 );
          AE_S32X2RNG_XP( c1, Y2, +4*stride*8 );
          AE_S32X2RNG_XP( c3, Y3, +4*stride*8 );

          //
          // Group 2
          //

          AE_L32X2_XP( a0, X0, +4*stride*8 );
          AE_L32X2_XP( a1, X1, +4*stride*8 );
          AE_L32X2_XP( a2, X2, +4*stride*8 );
          AE_L32X2_XP( a3, X3, +4*stride*8 );

          AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
          AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

          b3 = AE_MUL32JS( b3 );

          AE_ADDANDSUB32S( c0, c2, b0, b1 );
          AE_ADDANDSUB32S( c1, c3, b2, b3 );

          c1 = AE_MULFC32RAS( c1, tw1 );
          c2 = AE_MULFC32RAS( c2, tw2 );
          c3 = AE_MULFC32RAS( c3, tw3 );

          AE_S32X2RNG_XP( c0, Y0, +4*stride*8 );
          AE_S32X2RNG_XP( c2, Y1, +4*stride*8 );
          AE_S32X2RNG_XP( c1, Y2, +4*stride*8 );
          AE_S32X2RNG_XP( c3, Y3, +4*stride*8 );

          //
          // Group 3
          //

          AE_L32X2_XP( a0, X0, -3*4*stride*8 + 8 );
          AE_L32X2_XP( a1, X1, -3*4*stride*8 + 8 );
          AE_L32X2_XP( a2, X2, -3*4*stride*8 + 8 );
          AE_L32X2_XP( a3, X3, -3*4*stride*8 + 8 );

          AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
          AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

          b3 = AE_MUL32JS( b3 );

          AE_ADDANDSUB32S( c0, c2, b0, b1 );
          AE_ADDANDSUB32S( c1, c3, b2, b3 );

          c1 = AE_MULFC32RAS( c1, tw1 );
          c2 = AE_MULFC32RAS( c2, tw2 );
          c3 = AE_MULFC32RAS( c3, tw3 );

          AE_S32X2RNG_XP( c0, Y0, -3*4*stride*8 + 8 );
          AE_S32X2RNG_XP( c2, Y1, -3*4*stride*8 + 8 );
          AE_S32X2RNG_XP( c1, Y2, -3*4*stride*8 + 8 );
          AE_S32X2RNG_XP( c3, Y3, -3*4*stride*8 + 8 );
        }
        X0 +=  15*stride;
        X1 +=  15*stride;
        X2 +=  15*stride;
        X3 +=  15*stride;

        Y0 +=  15*stride;
        Y1 +=  15*stride;
        Y2 +=  15*stride;
        Y3 +=  15*stride;
      }

      {
        // Swap input/output buffers between successive stages.
        int32_t * T = X; X = Y; Y = T;
      }

      AE_CALCRNG3();

      nsa = 3 - RUR_AE_SAR();
    }
  }

  __Pragma( "no_reorder" );

  //----------------------------------------------------------------------------
  // Perform the next to last stage.

  {
    ae_int32x2 a0, a1, a2, a3;
    ae_int32x2 b0, b1, b2, b3;
    ae_f32x2   c0, c1, c2, c3;

    TWD = (const ae_f32x2*)( (uintptr_t)twd + 3*step*8 );

    X0 = (ae_int32x2*)x;
    Y0 = (ae_int32x2*)y;

    if ( stride == 4 )
    {
      //
      // Next to last stage for FFT size an even power of two.
      //

      ae_f32x2 tw11, tw12, tw13;
      ae_f32x2 tw21, tw22, tw23;
      ae_f32x2 tw31, tw32, tw33;

      shiftSum += ( shift = 3 - nsa );

      WUR_AE_SAR( shift );
      
      tw21 = ae_f32x2_loadi( TWD,       1*8 );
      tw31 = ae_f32x2_loadi( TWD,       2*8 );
      ae_f32x2_loadxp( tw11, TWD, +3*step*8 );

      tw22 = ae_f32x2_loadi( TWD,       1*8 );
      tw32 = ae_f32x2_loadi( TWD,       2*8 );
      ae_f32x2_loadxp( tw12, TWD, +3*step*8 );

      tw23 = ae_f32x2_loadi( TWD,       1*8 );
      tw33 = ae_f32x2_loadi( TWD,       2*8 );
      ae_f32x2_loadxp( tw13, TWD, +3*step*8 );

      __Pragma( "loop_count min=4, factor=4" );
      for ( n=0; n<N/(4*4); n++ )
      {
        //
        // Group 0
        //

        AE_L32X2_IP( a0, X0,  +4*8 );
        AE_L32X2_IP( a1, X0,  +4*8 );
        AE_L32X2_IP( a2, X0,  +4*8 );
        AE_L32X2_XP( a3, X0, -11*8 );

        AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
        AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

        b3 = AE_MUL32JS( b3 );

        AE_ADDANDSUB32S( c0, c2, b0, b1 );
        AE_ADDANDSUB32S( c1, c3, b2, b3 );

        AE_S32X2RNG_IP( c0, Y0,  +4*8 );
        AE_S32X2RNG_IP( c2, Y0,  +4*8 );
        AE_S32X2RNG_IP( c1, Y0,  +4*8 );
        AE_S32X2RNG_XP( c3, Y0, -11*8 );

        //
        // Group 1
        //

        AE_L32X2_IP( a0, X0,  +4*8 );
        AE_L32X2_IP( a1, X0,  +4*8 );
        AE_L32X2_IP( a2, X0,  +4*8 );
        AE_L32X2_XP( a3, X0, -11*8 );

        AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
        AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

        b3 = AE_MUL32JS( b3 );

        AE_ADDANDSUB32S( c0, c2, b0, b1 );
        AE_ADDANDSUB32S( c1, c3, b2, b3 );

        c1 = AE_MULFC32RAS( c1, tw11 );
        c2 = AE_MULFC32RAS( c2, tw21 );
        c3 = AE_MULFC32RAS( c3, tw31 );

        AE_S32X2RNG_IP( c0, Y0,  +4*8 );
        AE_S32X2RNG_IP( c2, Y0,  +4*8 );
        AE_S32X2RNG_IP( c1, Y0,  +4*8 );
        AE_S32X2RNG_XP( c3, Y0, -11*8 );

        //
        // Group 2
        //

        AE_L32X2_IP( a0, X0,  +4*8 );
        AE_L32X2_IP( a1, X0,  +4*8 );
        AE_L32X2_IP( a2, X0,  +4*8 );
        AE_L32X2_XP( a3, X0, -11*8 );

        AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
        AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

        b3 = AE_MUL32JS( b3 );

        AE_ADDANDSUB32S( c0, c2, b0, b1 );
        AE_ADDANDSUB32S( c1, c3, b2, b3 );

        c1 = AE_MULFC32RAS( c1, tw12 );
        c2 = AE_MULFC32RAS( c2, tw22 );
        c3 = AE_MULFC32RAS( c3, tw32 );

        AE_S32X2RNG_IP( c0, Y0,  +4*8 );
        AE_S32X2RNG_IP( c2, Y0,  +4*8 );
        AE_S32X2RNG_IP( c1, Y0,  +4*8 );
        AE_S32X2RNG_XP( c3, Y0, -11*8 );

        //
        // Group 3
        //

        AE_L32X2_IP( a0, X0, +4*8 );
        AE_L32X2_IP( a1, X0, +4*8 );
        AE_L32X2_IP( a2, X0, +4*8 );
        AE_L32X2_IP( a3, X0, +1*8 );

        AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
        AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

        b3 = AE_MUL32JS( b3 );

        AE_ADDANDSUB32S( c0, c2, b0, b1 );
        AE_ADDANDSUB32S( c1, c3, b2, b3 );

        c1 = AE_MULFC32RAS( c1, tw13 );
        c2 = AE_MULFC32RAS( c2, tw23 );
        c3 = AE_MULFC32RAS( c3, tw33 );

        AE_S32X2RNG_IP( c0, Y0, +4*8 );
        AE_S32X2RNG_IP( c2, Y0, +4*8 );
        AE_S32X2RNG_IP( c1, Y0, +4*8 );
        AE_S32X2RNG_IP( c3, Y0, +1*8 );
      }

      AE_CALCRNG3();

      nsa = 3 - RUR_AE_SAR();
    }
    else if ( stride == 2 )
    {
      //
      // Next to last stage for FFT size an odd power of two.
      //

      ae_f32x2 tw1, tw2, tw3;

      shiftSum += ( shift = 3 - nsa );

      WUR_AE_SAR( shift );
      
      tw1 = ae_f32x2_loadi( TWD, 0*8 );
      tw2 = ae_f32x2_loadi( TWD, 1*8 );
      tw3 = ae_f32x2_loadi( TWD, 2*8 );

      __Pragma( "loop_count min=4, factor=4" );
      for ( n=0; n<N/(2*4); n++ )
      {
        //
        // Group 0
        //

        AE_L32X2_IP( a0, X0, +2*8 );
        AE_L32X2_IP( a1, X0, +2*8 );
        AE_L32X2_IP( a2, X0, +2*8 );
        AE_L32X2_XP( a3, X0, -5*8 );

        AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
        AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

        b3 = AE_MUL32JS( b3 );

        AE_ADDANDSUB32S( c0, c2, b0, b1 );
        AE_ADDANDSUB32S( c1, c3, b2, b3 );

        AE_S32X2RNG_IP( c0, Y0, +2*8 );
        AE_S32X2RNG_IP( c2, Y0, +2*8 );
        AE_S32X2RNG_IP( c1, Y0, +2*8 );
        AE_S32X2RNG_XP( c3, Y0, -5*8 );

        //
        // Group 1
        //

        AE_L32X2_IP( a0, X0, +2*8 );
        AE_L32X2_IP( a1, X0, +2*8 );
        AE_L32X2_IP( a2, X0, +2*8 );
        AE_L32X2_IP( a3, X0, +1*8 );

        AE_ADDANDSUBRNG32( b0, b2, a0, a2 );
        AE_ADDANDSUBRNG32( b1, b3, a3, a1 );

        b3 = AE_MUL32JS( b3 );

        AE_ADDANDSUB32S( c0, c2, b0, b1 );
        AE_ADDANDSUB32S( c1, c3, b2, b3 );

        c1 = AE_MULFC32RAS( c1, tw1 );
        c2 = AE_MULFC32RAS( c2, tw2 );
        c3 = AE_MULFC32RAS( c3, tw3 );

        AE_S32X2RNG_IP( c0, Y0, +2*8 );
        AE_S32X2RNG_IP( c2, Y0, +2*8 );
        AE_S32X2RNG_IP( c1, Y0, +2*8 );
        AE_S32X2RNG_IP( c3, Y0, +1*8 );
      }

      AE_CALCRNG3();

      nsa = 3 - RUR_AE_SAR();
    }
  }

  *bexp = nsa;

  return (shiftSum);

} // fft_cplx_inner_32x32()
