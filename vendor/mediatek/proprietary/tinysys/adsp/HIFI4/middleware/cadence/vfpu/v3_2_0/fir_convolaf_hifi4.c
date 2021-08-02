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
  NatureDSP Signal Processing Library. FIR part
    Real data circular convolution, floating point, no requirements on vectors 
    length and alignment.
    C code optimized for HiFi4
  IntegrIT, 2006-2015
*/



/* Portable data types. */
#include "NatureDSP_types.h"
/* Signal Processing Library API. */
#include "NatureDSP_Signal.h"
/* Common utility and macros declarations. */
#include "common.h"
#include "raw_corrf.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void,fir_convolaf,(     void       * restrict s,
                       float32_t  * restrict r,
                 const float32_t  * restrict x,
                 const float32_t  * restrict y,
                 int N, int M ))
#else

/* Align address on a specified boundary. */
#define ALIGNED_ADDR( addr, align ) \
      (void*)( ( (uintptr_t)(addr) + ( (align) - 1 ) ) & ~( (align) - 1 ) )


/*-------------------------------------------------------------------------
  Circular Convolution
  Performs circular convolution between vectors x (of length N) and y (of 
  length M) resulting in vector r of length N.
  These functions implement the circular convolution algorithm described in
  the previous chapter with no limitations on x and y vectors length and
  alignment at the cost of increased processing complexity. In addition, this
  implementation variant requires scratch memory area.

  Precision: 
  32x16    32x16-bit data, 32-bit outputs
  32x32    32x32-bit data, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f         floating point

  Input:
  s[]           scratch area, 
                  FIR_CONVOLA32X16_SCRATCH_SIZE(N,M) or
                  FIR_CONVOLA32X32_SCRATCH_SIZE(N,M) or
                  FIR_CONVOLA32X32EP_SCRATCH_SIZE(N,M) or
                  FIR_CONVOLAF_SCRATCH_SIZE(N,M) bytes
  x[N]          input data Q31 or floating point
  y[M]          input data Q31, Q15 or floating point
  N             length of x
  M             length of y
  Output:
  r[N]          output data, Q31 or floating point

  Restriction:
  x,y,r should not overlap
  s        - must be aligned on an 8-bytes boundary
  N,M      - must be >0
  N >= M-1 - minimum allowed length of vector x is the length of y minus one
-------------------------------------------------------------------------*/
void fir_convolaf(     void       * restrict s,
                       float32_t  * restrict r,
                 const float32_t  * restrict x,
                 const float32_t  * restrict y,
                 int N, int M )
{
    //
    // Circular convolution algorithm:
    //
    //   r[n] = sum( x[mod(n-m,N)]*y[m] )
    //        m=0..M-1
    //
    //   where n = 0..N-1
    //
    const xtfloatx2 * restrict x_inp;
    const xtfloatx2 * restrict y_inp;
          xtfloatx2 * restrict buf;
    xtfloatx2 regx2;
    xtfloat reg;
    ae_valign al_inp, al_buf;
    int xbuf_len, ybuf_len;
    int n, m, T;

    NASSERT( s && r && x && y && N > 0 && M > 0 && N >= M-1 );
    NASSERT_ALIGN8( s );

    // Partition the scratch memory area.
    buf = (xtfloatx2 *)(s);
    // compute length of the buffer for x
    xbuf_len = (N+M) & ~1;
    // compute length of the buffer for y
    ybuf_len = (M+3) & ~3;
    ASSERT( (xbuf_len+ybuf_len)*(int)sizeof(float32_t) <= 
            (int)FIR_XCORRAF_SCRATCH_SIZE( N, M ) );

    //
    // Copy x[N] data into the scratch memory in a way that simplifies the
    // convolution calculation:
    //  x[N-(M-1)]..x[N-1] x[0] x[1]..x[N-1]
    //
    T = (M-1)>>1;
    x_inp = (const xtfloatx2 *)(x+N-(M-1));

    for (m = 0; m < 2; m++)
    {
        al_inp = XT_LASX2PP(x_inp);
        al_buf = AE_ZALIGN64();
        for ( n=0; n<T; n++ )
        {
            XT_LASX2IP(regx2, al_inp, x_inp);
            XT_SASX2IP(regx2, al_buf, buf);
        }
        XT_SASX2POSFP(al_buf, buf);
        if (!(M&1))
        {
            reg = XT_LSI((xtfloat *)x_inp, 0);
            XT_SSIP(reg, castxcc(xtfloat,buf), sizeof(float32_t));
        }
        T = (xbuf_len-(M-1))>>1;
        x_inp = (const xtfloatx2 *)x;
    }
    //
    // Copy y[M] data in reverse order into the scratch memory after x[N].
    // Round M to a next multiple of 4 and set excess values to zero.
    //
    y_inp = (const xtfloatx2 *)(y+M-1);
    al_inp = XT_LASX2PP(y_inp);
    for ( m=0; m<(M>>1); m++ )
    {
        XT_LASX2RIP(regx2, al_inp, y_inp);
        XT_SSX2IP(regx2, buf, 2*sizeof(float32_t));
    }
    if (M&1)
    {
        reg = XT_LSI((xtfloat *)y_inp, 0);
        XT_SSIP(reg, castxcc(xtfloat,buf), sizeof(float32_t));
    }
    reg = 0.0f;
    for (m = 0; m < ybuf_len-M+1; m++)
    {
        XT_SSIP(reg, castxcc(xtfloat,buf), sizeof(float32_t));
    }
    __Pragma("no_reorder")
    raw_corrf(r,(const float32_t *)s,(const float32_t *)s+xbuf_len,N,M);
} // fir_convolaf()
#endif
