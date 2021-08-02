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
    Real data circular auto-correlation, floating point, no requirements on vectors
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

/* Align address on a specified boundary. */
#define ALIGNED_ADDR( addr, align ) \
      (void*)( ( (uintptr_t)(addr) + ( (align) - 1 ) ) & ~( (align) - 1 ) )

#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void ,fir_acorraf,(     void       * restrict s,
                      float32_t  * restrict r,
                const float32_t  * restrict x,
                int N ))
#else

/*-------------------------------------------------------------------------
  Circular Autocorrelation 
  Estimates the auto-correlation of vector x. Returns autocorrelation of 
  length N.
  These functions implement the circular autocorrelation algorithm described
  in the previous chapter with no limitations on x vector length and
  alignment at the cost of increased processing complexity. In addition, this
  implementation variant requires scratch memory area.

  Precision: 
  32x32    32x32-bit data, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f         floating point

  Input:
  s[]       scratch area of 
            FIR_ACORRA32X32_SCRATCH_SIZE(N) or
            FIR_ACORRA32X32EP_SCRATCH_SIZE(N) or
            FIR_ACORRAF_SCRATCH_SIZE(N) bytes
  x[N]      input data Q31 or floating point
  N         length of x
  Output:
  r[N]      output data, Q31 or floating point

  Restrictions:
  x,r,s should not overlap
  N   - must be non-zero
  s   - aligned on an 8-bytes boundary
-------------------------------------------------------------------------*/
void fir_acorraf(     void       * restrict s,
                      float32_t  * restrict r,
                const float32_t  * restrict x,
                int N )
{
    fir_xcorraf(s,r,x,x,N,N);
}
#endif
