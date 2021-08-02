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
 * C code optimized for HiFi4
 */

/* Cross-platform data type definitions. */
#include "NatureDSP_types.h"
/* Common helper macros. */
#include "common.h"
/* Basic operations for the reference code. */
#include "baseop.h"

#include "NatureDSP_Signal.h"

#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(int,fft_cplxf_ie,(complex_float * y, complex_float * x, const complex_float* twd, int twdstep, int N ))
#else

#define SZ_CF32 (sizeof(complex_float))
#define LOG2_SZ_CF32 3/* log2(sizeof(complex_float)) */
/* 1.0/sqrt(2.0) */
static const union ufloat32uint32 _invsqrt2f_ = { 0x3f3504f3 };

/*-------------------------------------------------------------------------
  These functions make FFT on complex data with optimized memory usage.
  Scaling  : Fixed data scaling at each stage
  NOTES:
  1. Bit-reversing reordering is done here.
  2. FFT runs in-place algorithm so INPUT DATA WILL APPEAR DAMAGED after 
     the call
  3. Forward/inverse FFT of size N may be supplied with constant data
     (twiddle factors) of a larger-sized FFT = N*twdstep.

  Precision: 
  f             floating point
 
  Input:
  x[N]                  complex input signal. Real and imaginary data are interleaved 
                        and real data goes first
  twd[N*twdstep*3/4]    twiddle factor table of a complex-valued FFT of size N*twdstep
  N                     FFT size
  twdstep               twiddle step 
  Output:
  y[N]                  output spectrum. Real and imaginary data are interleaved and 
                        real data goes first

  Returned value: total number of right shifts occurred during scaling 
                  procedure

  Restrictions:
  x,y - should not overlap
  x,y - aligned on 8-bytes boundary
  N   - power of 2 and >=8 for floating point
-------------------------------------------------------------------------*/
int fft_cplxf_ie    (complex_float * y, complex_float * x, const complex_float* twd, int twdstep, int N )
{
  const xtfloatx2 *restrict p_twd;
  const xtfloatx2 *restrict p0_ld;
  const xtfloatx2 *restrict p1_ld;
  const xtfloatx2 *restrict p2_ld;
  const xtfloatx2 *restrict p3_ld;
        xtfloatx2 *restrict p0_st;
        xtfloatx2 *restrict p1_st;
        xtfloatx2 *restrict p2_st;
        xtfloatx2 *restrict p3_st;
        xtfloatx2 *restrict p4_st;
        xtfloatx2 *restrict p5_st;
        xtfloatx2 *restrict p6_st;
        xtfloatx2 *restrict p7_st;
  xtfloatx2 tw1, tw2, tw3;
  xtfloatx2 a0, a1, a2, a3;
  xtfloatx2 b0, b1, b2, b3;
  int N4, logN, stride;
  int m, n;
  unsigned int idx, bitrevstride;

  NASSERT( x );
  NASSERT( y );
  NASSERT( twd );
  NASSERT( x != y );
  NASSERT_ALIGN( x, 8 );
  NASSERT_ALIGN( y, 8 );
  NASSERT_ALIGN( twd, 8 );
  NASSERT( twdstep >= 1 );
  NASSERT( N>=8 && 0 == (N&(N-1)) );

  N4 = N>>2;
  logN = 32 - NSA( N4 );

  /* Set the pointer to the twiddle table            *
   * and set bounds of the table for circular loads. */
  p_twd = (const xtfloatx2 *)(twd);
  WUR_AE_CBEGIN0((uintptr_t)(twd));
  WUR_AE_CEND0  ((uintptr_t)(twd+3*twdstep*(N4)));

  if (N==16)
  {
    p0_st = (xtfloatx2 *)(x);
    p1_st = p0_st+4;
    p2_st = p1_st+4;
    p3_st = p2_st+4;
    p0_ld = p0_st;
    p1_ld = p1_st;
    p2_ld = p2_st;
    p3_ld = p3_st;
    /* Radix-4 butterfly */
    __Pragma("ymemory (p_twd)")
    for ( n=0; n<4; n++ )
    {
      /* load input samples */
      XT_LSX2IP(a0, p0_ld, SZ_CF32);
      XT_LSX2IP(a1, p1_ld, SZ_CF32);
      XT_LSX2IP(a2, p2_ld, SZ_CF32);
      XT_LSX2IP(a3, p3_ld, SZ_CF32);

      /* compute butterfly */
      /* 1-st substage */
      b0 = a0 + a2;
      b1 = a1 + a3;
      b2 = a0 - a2;
      b3 = a1 - a3;
      /* 2-nd substage */
      a0 = b0 + b1;
      a2 = b0 - b1;
      /* a1 <- b2-j*b3 */
      /* a3 <- b2+j*b3 */
      b3 = XT_SEL32_LH_SX2(b3, b3);
      b3 = XT_CONJC_S(b3);
      a1 = b2 + b3;
      a3 = b2 - b3;

      /* multiply by twiddle factors */
      XT_LSX2IP(tw1, p_twd, SZ_CF32);
      XT_LSX2IP(tw2, p_twd, SZ_CF32);
      XT_LSX2XC(tw3, p_twd, (twdstep*3-2)*SZ_CF32);
      b0 = a0;
      b1 = XT_MULC_S(a1, tw1);
      b2 = XT_MULC_S(a2, tw2);
      b3 = XT_MULC_S(a3, tw3);

      /* Two middle quartiles are swapped on all but the last stage to use the bit reversal
       * permutation instead of the digit reverse. */
      XT_SSX2IP(b0, p0_st, SZ_CF32);
      XT_SSX2IP(b2, p1_st, SZ_CF32);
      XT_SSX2IP(b1, p2_st, SZ_CF32);
      XT_SSX2IP(b3, p3_st, SZ_CF32);
    }

    idx = 0;
    bitrevstride = 0x08000000U;

    /* Radix-4 butterfly with bit reversal permutation */
    p0_ld = (const xtfloatx2 *)(x);
    p1_ld = p0_ld+1;
    p2_ld = p1_ld+1;
    p3_ld = p2_ld+1;
    p0_st = (xtfloatx2 *)(y);
    p1_st = p0_st+4;
    p2_st = p1_st+4;
    p3_st = p2_st+4;
    
    __Pragma("loop_count min=2, factor=2")
    for ( n=0; n<N4; n++ )
    {
      XT_LSX2IP(a0, p0_ld, 4*SZ_CF32);
      XT_LSX2IP(a1, p1_ld, 4*SZ_CF32);
      XT_LSX2IP(a2, p2_ld, 4*SZ_CF32);
      XT_LSX2IP(a3, p3_ld, 4*SZ_CF32);

      b0 = a0 + a2;
      b1 = a1 + a3;
      b2 = a0 - a2;
      b3 = a1 - a3;
      
      a0 = b0 + b1;
      a2 = b0 - b1;
      /* a1 <- b2-j*b3 */
      /* a3 <- b2+j*b3 */
      b3 = XT_CONJC_S(b3);
      b3 = XT_SEL32_LH_SX2(b3, b3);
      a1 = b2 - b3;
      a3 = b2 + b3;
      
      XT_SSX2X(a0, p0_st, idx);
      XT_SSX2X(a1, p1_st, idx);
      XT_SSX2X(a2, p2_st, idx);
      XT_SSX2X(a3, p3_st, idx);

      idx = AE_ADDBRBA32(idx, bitrevstride);
    }

    return 0;
  }
  /*---------------------------------------------------------------------------*
   * Perform first through the next to last stages. We use DIF,                *
   * all permutations are deferred until the last stage.                       */
  for (stride = N4; stride > 2; stride >>= 2)
  {
    p0_st = (xtfloatx2 *)(x);
    
    __Pragma("loop_count min=1")
    for ( m=0; m<N4; m+=stride )
    {
      p1_st = (xtfloatx2 *)((uintptr_t)p0_st + 8 * stride);
      p2_st = (xtfloatx2 *)((uintptr_t)p1_st + 8 * stride);
      p3_st = (xtfloatx2 *)((uintptr_t)p2_st + 8 * stride);
      p0_ld = p0_st;
      p1_ld = p1_st;
      p2_ld = p2_st;
      p3_ld = p3_st;
      /* Radix-4 butterfly */
      __Pragma("ymemory (p_twd)")
      __Pragma("loop_count min=2")
      for ( n=0; n<stride; n++ )
      {
        /* load input samples */
        XT_LSX2IP(a0, p0_ld, SZ_CF32);
        XT_LSX2IP(a1, p1_ld, SZ_CF32);
        XT_LSX2IP(a2, p2_ld, SZ_CF32);
        XT_LSX2IP(a3, p3_ld, SZ_CF32);

        /* compute butterfly */
        /* 1-st substage */
        b0 = a0 + a2;
        b1 = a1 + a3;
        b2 = a0 - a2;
        b3 = a1 - a3;
        /* 2-nd substage */
        a0 = b0 + b1;
        a2 = b0 - b1;
        /* a1 <- b2-j*b3 */
        /* a3 <- b2+j*b3 */
        b3 = XT_SEL32_LH_SX2(b3, b3);
        b3 = XT_CONJC_S(b3);
        a1 = b2 + b3;
        a3 = b2 - b3;

        /* multiply by twiddle factors */
        XT_LSX2IP(tw1, p_twd, SZ_CF32);
        XT_LSX2IP(tw2, p_twd, SZ_CF32);
        XT_LSX2XC(tw3, p_twd, (twdstep*3-2)*SZ_CF32);
        b0 = a0;
        b1 = XT_MULC_S(a1, tw1);
        b2 = XT_MULC_S(a2, tw2);
        b3 = XT_MULC_S(a3, tw3);

        /* Two middle quartiles are swapped on all but the last stage to use the bit reversal
         * permutation instead of the digit reverse. */
        XT_SSX2IP(b0, p0_st, SZ_CF32);
        XT_SSX2IP(b2, p1_st, SZ_CF32);
        XT_SSX2IP(b1, p2_st, SZ_CF32);
        XT_SSX2IP(b3, p3_st, SZ_CF32);
      }
      p0_st = p3_st;
    }
    twdstep <<= 2;
  }
  
  /*----------------------------------------------------------------------------
   Last stage (radix-8 or radix-4 for even powers of two) with bit reversal
   permutation.*/
  idx = 0;
  bitrevstride = 0x80000000U >> (logN-3+LOG2_SZ_CF32);
  if ( stride > 1 )
  {
    /* Radix-8 butterfly */
    xtfloatx2 a4, a5, a6, a7;
    xtfloatx2 b4, b5, b6, b7;
    xtfloatx2 a3_, a7_, invsqrt2_;
    int N8 = N4 >> 1;
    bitrevstride <<= 1;
    
    invsqrt2_ = XT_LSI((xtfloat *)&_invsqrt2f_, 0);/* 1.0/sqrt(2.0) */
    p0_ld = (const xtfloatx2 *)(x);
    p1_ld = p0_ld+4;

    p0_st = (xtfloatx2 *)(y);
    p1_st = p0_st+N8;
    p2_st = p1_st+N8;
    p3_st = p2_st+N8;
    p4_st = p3_st+N8;
    p5_st = p4_st+N8;
    p6_st = p5_st+N8;
    p7_st = p6_st+N8;
    
    __Pragma("loop_count min=1")
    for ( n=0; n<N8; n++ )
    {
      /* Load input samples */
      XT_LSX2IP(a0, p0_ld, SZ_CF32);
      XT_LSX2IP(a1, p0_ld, SZ_CF32);
      XT_LSX2IP(a2, p0_ld, SZ_CF32);
      XT_LSX2IP(a3, p0_ld, 5*SZ_CF32);
      XT_LSX2IP(a4, p1_ld, SZ_CF32);
      XT_LSX2IP(a5, p1_ld, SZ_CF32);
      XT_LSX2IP(a6, p1_ld, SZ_CF32);
      XT_LSX2IP(a7, p1_ld, 5*SZ_CF32);

      /* Compute butterfly */
      /* 1-st substage */
      b0 = a0 + a4;
      b1 = a1 + a5;
      b2 = a2 + a6;
      b3 = a3 + a7;
      b4 = a0 - a4;
      b5 = a1 - a5;
      b6 = a2 - a6;
      b7 = a3 - a7;
      /* 2-nd substage */
      /* b6 <- -j*b6 */
      /* b7 <- -j*b7 */
      b6 = XT_SEL32_LH_SX2(b6, b6);
      b6 = XT_CONJC_S(b6);
      b7 = XT_SEL32_LH_SX2(b7, b7);
      b7 = XT_CONJC_S(b7);

      a0 = b0 + b2;
      a1 = b1 + b3;
      a2 = b4 + b6;
      a3 = b5 + b7;
      a4 = b0 - b2;
      a5 = b1 - b3;
      a6 = b4 - b6;
      a7 = b5 - b7;
      /* 3-rd substage */
      /* b3 <- ( 1-j)/sqrt(2.0)*b6 */
      /* b5 <- ( 0-j)*b5           */
      /* b7 <- (-1-j)/sqrt(2.0)*b7 */
      a3_= XT_SEL32_LH_SX2(a3, a3);
      a3_= XT_CONJC_S(a3_);
      a3 = a3_ + a3;
      a3 = a3*invsqrt2_;
      a5 = XT_SEL32_LH_SX2(a5, a5);
      a5 = XT_CONJC_S(a5);
      a7_= XT_SEL32_LH_SX2(a7, a7);
      a7_= XT_CONJC_S(a7_);
      a7 = a7_ - a7;
      a7 = a7*invsqrt2_;

      b0 = a0 + a1;
      b1 = a2 + a3;
      b2 = a4 + a5;
      b3 = a6 + a7;
      b4 = a0 - a1;
      b5 = a2 - a3;
      b6 = a4 - a5;
      b7 = a6 - a7;
      /* Store samples */
      XT_SSX2X(b0, p0_st, idx);
      XT_SSX2X(b1, p1_st, idx);
      XT_SSX2X(b2, p2_st, idx);
      XT_SSX2X(b3, p3_st, idx);
      XT_SSX2X(b4, p4_st, idx);
      XT_SSX2X(b5, p5_st, idx);
      XT_SSX2X(b6, p6_st, idx);
      XT_SSX2X(b7, p7_st, idx);

      idx = AE_ADDBRBA32(idx, bitrevstride);
    }
  }
  else
  {
    /* Radix-4 butterfly */
    p0_ld = (const xtfloatx2 *)(x);
    p1_ld = p0_ld+1;
    p2_ld = p1_ld+1;
    p3_ld = p2_ld+1;
    p0_st = (xtfloatx2 *)(y);
    p1_st = p0_st+N4;
    p2_st = p1_st+N4;
    p3_st = p2_st+N4;
    
    __Pragma("loop_count min=2, factor=2")
    for ( n=0; n<N4; n++ )
    {
      XT_LSX2IP(a0, p0_ld, 4*SZ_CF32);
      XT_LSX2IP(a1, p1_ld, 4*SZ_CF32);
      XT_LSX2IP(a2, p2_ld, 4*SZ_CF32);
      XT_LSX2IP(a3, p3_ld, 4*SZ_CF32);

      b0 = a0 + a2;
      b1 = a1 + a3;
      b2 = a0 - a2;
      b3 = a1 - a3;
      
      a0 = b0 + b1;
      a2 = b0 - b1;
      /* a1 <- b2-j*b3 */
      /* a3 <- b2+j*b3 */
      b3 = XT_CONJC_S(b3);
      b3 = XT_SEL32_LH_SX2(b3, b3);
      a1 = b2 - b3;
      a3 = b2 + b3;
      
      XT_SSX2X(a0, p0_st, idx);
      XT_SSX2X(a1, p1_st, idx);
      XT_SSX2X(a2, p2_st, idx);
      XT_SSX2X(a3, p3_st, idx);

      idx = AE_ADDBRBA32(idx, bitrevstride);
    }
  }
  return 0;
} /* fft_cplxf_ie() */
#endif
