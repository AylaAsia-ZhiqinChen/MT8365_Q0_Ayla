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
    Code optimized for HiFi4
    Floating point DCT 
    Integrit, 2006-2015
*/

#include "NatureDSP_Signal.h"
#include "baseop.h"
#include "common.h"

#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(int,dctf,( float32_t * restrict y,float32_t * restrict x,int N))
#else

#define SZ_CF32 (sizeof(complex_float))
#define SZ_F32  (sizeof(float32_t))

/*
exp(i*pi/2*(0:N-1)/N), N=64
*/
static const union ufloat32uint32 ALIGN(8) dct_twd64[128]=
{
{0x3f800000},{0x00000000},{0x3f7fec43},{0x3cc90ab0},{0x3f7fb10f},{0x3d48fb30},{0x3f7f4e6d},{0x3d96a905},
{0x3f7ec46d},{0x3dc8bd36},{0x3f7e1324},{0x3dfab273},{0x3f7d3aac},{0x3e164083},{0x3f7c3b28},{0x3e2f10a2},
{0x3f7b14be},{0x3e47c5c2},{0x3f79c79d},{0x3e605c13},{0x3f7853f8},{0x3e78cfcc},{0x3f76ba07},{0x3e888e93},
{0x3f74fa0b},{0x3e94a031},{0x3f731447},{0x3ea09ae5},{0x3f710908},{0x3eac7cd4},{0x3f6ed89e},{0x3eb8442a},
{0x3f6c835e},{0x3ec3ef15},{0x3f6a09a7},{0x3ecf7bca},{0x3f676bd8},{0x3edae880},{0x3f64aa59},{0x3ee63375},
{0x3f61c598},{0x3ef15aea},{0x3f5ebe05},{0x3efc5d27},{0x3f5b941a},{0x3f039c3d},{0x3f584853},{0x3f08f59b},
{0x3f54db31},{0x3f0e39da},{0x3f514d3d},{0x3f13682a},{0x3f4d9f02},{0x3f187fc0},{0x3f49d112},{0x3f1d7fd1},
{0x3f45e403},{0x3f226799},{0x3f41d870},{0x3f273656},{0x3f3daef9},{0x3f2beb4a},{0x3f396842},{0x3f3085bb},
{0x3f3504f3},{0x3f3504f3},{0x3f3085bb},{0x3f396842},{0x3f2beb4a},{0x3f3daef9},{0x3f273656},{0x3f41d870},
{0x3f226799},{0x3f45e403},{0x3f1d7fd1},{0x3f49d112},{0x3f187fc0},{0x3f4d9f02},{0x3f13682a},{0x3f514d3d},
{0x3f0e39da},{0x3f54db31},{0x3f08f59b},{0x3f584853},{0x3f039c3d},{0x3f5b941a},{0x3efc5d27},{0x3f5ebe05},
{0x3ef15aea},{0x3f61c598},{0x3ee63375},{0x3f64aa59},{0x3edae880},{0x3f676bd8},{0x3ecf7bca},{0x3f6a09a7},
{0x3ec3ef15},{0x3f6c835e},{0x3eb8442a},{0x3f6ed89e},{0x3eac7cd4},{0x3f710908},{0x3ea09ae5},{0x3f731447},
{0x3e94a031},{0x3f74fa0b},{0x3e888e93},{0x3f76ba07},{0x3e78cfcc},{0x3f7853f8},{0x3e605c13},{0x3f79c79d},
{0x3e47c5c2},{0x3f7b14be},{0x3e2f10a2},{0x3f7c3b28},{0x3e164083},{0x3f7d3aac},{0x3dfab273},{0x3f7e1324},
{0x3dc8bd36},{0x3f7ec46d},{0x3d96a905},{0x3f7f4e6d},{0x3d48fb30},{0x3f7fb10f},{0x3cc90ab0},{0x3f7fec43}
};

/*
N=32;  
twd = exp(-2j*pi*[1;2;3]*(0:N/4-1)/N);
twd_ri = reshape([real(twd(:).');imag(twd(:).')],1,2*numel(twd));
*/
static const union ufloat32uint32 ALIGN(8) fft_twd32[48]=
{
{0x3f800000},{0x00000000},{0x3f800000},{0x00000000},{0x3f800000},{0x00000000},
{0x3f7b14be},{0xbe47c5c2},{0x3f6c835e},{0xbec3ef15},{0x3f54db31},{0xbf0e39da},
{0x3f6c835e},{0xbec3ef15},{0x3f3504f3},{0xbf3504f3},{0x3ec3ef15},{0xbf6c835e},
{0x3f54db31},{0xbf0e39da},{0x3ec3ef15},{0xbf6c835e},{0xbe47c5c2},{0xbf7b14be},
{0x3f3504f3},{0xbf3504f3},{0x248d3132},{0xbf800000},{0xbf3504f3},{0xbf3504f3},
{0x3f0e39da},{0xbf54db31},{0xbec3ef15},{0xbf6c835e},{0xbf7b14be},{0xbe47c5c2},
{0x3ec3ef15},{0xbf6c835e},{0xbf3504f3},{0xbf3504f3},{0xbf6c835e},{0x3ec3ef15},
{0x3e47c5c2},{0xbf7b14be},{0xbf6c835e},{0xbec3ef15},{0xbf0e39da},{0x3f54db31}
};

/* 1/sqrt(2.0) */
static const union ufloat32uint32 _invsqrt2f_ = { 0x3f3504f3 };

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
int dctf     ( float32_t * restrict y,float32_t * restrict x,int N)
/*
    Reference Matlab code:
    function y=dctf(x)
    N=numel(x);
    y(1:N/2)     =x(1:2:N);
    y(N:-1:N/2+1)=x(2:2:N);
    % take fft of N/2
    y=fft(y(1:2:N)+j*y(2:2:N));
    w=exp(i*pi/2*(0:N-1)/N);
    % DCT split algorithm
    Y0=y(1);
    T0=real(Y0)+imag(Y0);
    T1=real(Y0)-imag(Y0);
    z(1      )= real(T0);%*sqrt(2)/2;
    z(N/2+1  )= real(T1)*sqrt(2)/2;
    for k=2:N/4
        Y0=y(k);
        Y1=y(N/2+2-k);
        COSI=(w(4*(k-1)+1));
        W1=w(k);
        W2=w(N/2+2-k);
        S=Y0+Y1;
        D=Y0-Y1;
        T0=i*real(D)+imag(S);
        T1=i*imag(D)+real(S);
        Y0=  ( imag(T0)*imag(COSI)-real(T0)*real(COSI)) + ...
           i*( real(T0)*imag(COSI)+imag(T0)*real(COSI));
        T0=0.5*(T1-Y0);
        T1=0.5*(T1+Y0);
        z(k      )= real(T0)*real(W1)+imag(T0)*imag(W1);
        z(N+2-k  )= real(T0)*imag(W1)-imag(T0)*real(W1);
        z(N/2+2-k)= real(T1)*real(W2)-imag(T1)*imag(W2);
        z(N/2+k  )= real(T1)*imag(W2)+imag(T1)*real(W2);
    end
    W1=w(N/4+1);
    T0=y(N/4+1);
    z(N/4+1  )= real(T0)*real(W1)-imag(T0)*imag(W1);
    z(N+1-N/4)= real(T0)*imag(W1)+imag(T0)*real(W1);
    y=z;
*/
{
    const xtfloatx2 *restrict p0_twd;
    const xtfloatx2 *restrict p1_twd;
    const xtfloatx2 *restrict p2_twd;
    const xtfloatx2 *restrict p0_ld;
    const xtfloatx2 *restrict p1_ld;
    const xtfloatx2 *restrict p2_ld;
    const xtfloatx2 *restrict p3_ld;
          xtfloatx2 *restrict p0_stx2;
          xtfloatx2 *restrict p1_stx2;
          xtfloatx2 *restrict p2_stx2;
          xtfloatx2 *restrict p3_stx2;
          xtfloat   *restrict p0_st;
          xtfloat   *restrict p1_st;
          xtfloat   *restrict p2_st;
          xtfloat   *restrict p3_st;
    xtfloatx2 t0, t1, y0, y1,
              w1, w2, s, d, cosi, c05;
    xtfloat b0, b1, re, im, invsqrt2f;
    ae_int32x2 t32x2;
    int k, n, twd_stride;
    int N2, N4;

    NASSERT_ALIGN(x,8);
    NASSERT_ALIGN(y,8);
    NASSERT(x!=y);
    ASSERT(N==32 || N==64);

    N2 = N>>1;
    N4 = N2>>1;

    /* permute inputs */
    p0_ld  = (const xtfloatx2 *)x;
    p0_stx2 = (xtfloatx2 *)y;
    p1_stx2 = (xtfloatx2 *)(y+N-2);
    __Pragma("loop_count min=2")
    for (n=0; n<N4; n++)
    {
      /* y[n]    =x[2*n+0] */
      /* y[N-1-n]=x[2*n+1] */
      XT_LSX2IP(t0, p0_ld, SZ_CF32);
      XT_LSX2IP(t1, p0_ld, SZ_CF32);
      y0 = XT_SEL32_HH_SX2(t0, t1);
      y1 = XT_SEL32_LL_SX2(t1, t0);
      XT_SSX2IP(y0, p0_stx2,       SZ_CF32);
      XT_SSX2XP(y1, p1_stx2, -(int)SZ_CF32);
    }

    /* compute fft(N/2) */
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
    if(N == 32)
    {
      xtfloatx2 a0, a1, a2, a3;
      xtfloatx2 b0, b1, b2, b3;
      xtfloatx2 tw1, tw2, tw3;
      int idx, bitrevstride;

      p0_twd = (const xtfloatx2 *)fft_twd32;
      p0_stx2 = (xtfloatx2 *)(y);
      p1_stx2 = p0_stx2 + 4;
      p2_stx2 = p1_stx2 + 4;
      p3_stx2 = p2_stx2 + 4;
      p0_ld = p0_stx2;
      p1_ld = p1_stx2;
      p2_ld = p2_stx2;
      p3_ld = p3_stx2;
      /* Radix-4 butterfly */
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
        XT_LSX2IP(tw1, p0_twd, SZ_CF32);
        XT_LSX2IP(tw2, p0_twd, SZ_CF32);
        XT_LSX2XC(tw3, p0_twd, (2*3-2)*SZ_CF32);
        b0 = a0;
        b1 = XT_MULC_S(a1, tw1);
        b2 = XT_MULC_S(a2, tw2);
        b3 = XT_MULC_S(a3, tw3);
        /* Two middle quartiles are swapped on all but the last stage to use the bit reversal
         * permutation instead of the digit reverse. */
        XT_SSX2IP(b0, p0_stx2, SZ_CF32);
        XT_SSX2IP(b2, p1_stx2, SZ_CF32);
        XT_SSX2IP(b1, p2_stx2, SZ_CF32);
        XT_SSX2IP(b3, p3_stx2, SZ_CF32);
      }
      __Pragma("no_reorder")
      /*-------------------------------------
       Last stage (radix-4) with bit reversal
       permutation.*/
      idx = 0;
      bitrevstride = 0x80000000U >> 4;

      p0_ld = (const xtfloatx2 *)(y);
      p1_ld = p0_ld+1;
      p2_ld = p1_ld+1;
      p3_ld = p2_ld+1;
      p0_stx2 = (xtfloatx2 *)(x);
      p1_stx2 = p0_stx2+4;
      p2_stx2 = p1_stx2+4;
      p3_stx2 = p2_stx2+4;
    
      for ( n=0; n<4; n++ )
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
      
        XT_SSX2X(a0, p0_stx2, idx);
        XT_SSX2X(a1, p1_stx2, idx);
        XT_SSX2X(a2, p2_stx2, idx);
        XT_SSX2X(a3, p3_stx2, idx);

        idx = AE_ADDBRBA32(idx, bitrevstride);
      }
    }
    else
    {
      fft_cplxf_ie((complex_float*)x, (complex_float*)y, (complex_float*)fft_twd32, 1, 32);
    }

    /* make final DCT transformation of FFT outputs */
    twd_stride=1;
    XT_MOVEQZ(twd_stride, 2, N-32);
    p0_ld  = (const xtfloatx2 *)x;
    p1_ld  = (const xtfloatx2 *)x+N2-1;
    p0_twd = (const xtfloatx2 *)dct_twd64+4*twd_stride;
    p1_twd = (const xtfloatx2 *)dct_twd64+twd_stride;
    p2_twd = (const xtfloatx2 *)dct_twd64+(N2-1)*twd_stride;
    p0_st = (xtfloat *)y;
    p1_st = p0_st+N2;
    p2_st = p1_st-1;
    p3_st = p2_st+N2;

    /* Load constants */
    c05 = (xtfloatx2)0.5f;/* 0.5 */
    invsqrt2f = XT_LSI((xtfloat *)&_invsqrt2f_, 0);/* 1/sqrt(2) */

    XT_LSX2IP(y0, p0_ld, SZ_CF32);
    /* b0 = y0.re + y0.im */
    /* b1 = y0.re - y0.im */
    re = XT_HIGH_S(y0);
    im = XT_LOW_S (y0);
    b0=XT_ADD_S(re, im);
    b1=XT_SUB_S(re, im);
    XT_SSIP(b0, p0_st, SZ_F32);
    b1 = XT_MUL_S(b1, invsqrt2f);
    XT_SSIP(b1, p1_st, SZ_F32);

    __Pragma("loop_count min=2")
    for (k=1; k<N4; k++)
    {
      XT_LSX2IP(y0, p0_ld,       SZ_CF32);
      XT_LSX2XP(y1, p1_ld, -(int)SZ_CF32);
      XT_LSX2XP(cosi, p0_twd, 4*twd_stride*SZ_CF32);
      XT_LSX2XP(w1  , p1_twd,   twd_stride*SZ_CF32);
      XT_LSX2XP(w2  , p2_twd,  -twd_stride*SZ_CF32);
      
      s  = y0 + y1;
      d  = y0 - y1;
      /* t0.re = s.im; t0.im = d.re */
      t0 = XT_SEL32_LH_SX2(s, d);
      /* t0.re = s.re; t0.im = d.im */
      t1 = XT_SEL32_HL_SX2(s, d);

      /* y0 = t0*cosi           */
      /* t0 = 0.5*(t1+conj(y0)) */
      /* t1 = 0.5*(t1-conj(y0)) */
      cosi = cosi*c05;
      t1 = t1*c05;
      y0 = XT_MULMUX_S(t0, cosi, 1);
      XT_MADDMUX_S(y0, t0, cosi, 7);
      t0 = t1 + y0;
      t1 = t1 - y0;
      t0 = XT_MULCCONJ_S(w1, t0);
      t1 = XT_MULC_S    (w2, t1);
      
      /* y[k    ]= t0.re */
      /* y[N-k  ]= t0.im */
      re = XT_HIGH_S(t0);
      t32x2 = XT_AE_MOVINT32X2_FROMXTFLOATX2(t0);
      XT_SSIP(re, p0_st, SZ_F32);/* save real part */
      AE_S32_L_IP(t32x2, castxcc(ae_int32,p3_st), -(int)SZ_F32);/* save imag part */
      /* y[N/2-k]= t1.re */
      /* y[N/2+k]= t1.im */
      re = XT_HIGH_S(t1);
      t32x2 = XT_AE_MOVINT32X2_FROMXTFLOATX2(t1);
      XT_SSIP(re, p2_st, -(int)SZ_F32);/* save real part */
      AE_S32_L_IP(t32x2, castxcc(ae_int32,p1_st), SZ_F32);/* save imag part*/
    }
    t0 = XT_LSX2I(p0_ld, 0);
    w1 = XT_LSX2I(p1_twd, 0);
    t0 = XT_MULC_S(t0, w1);

    re = XT_HIGH_S(t0);
    im = XT_LOW_S (t0);
    XT_SSI(re, p0_st, 0);
    XT_SSI(im, p3_st, 0);

    return 0;
} /* dctf() */
#endif
