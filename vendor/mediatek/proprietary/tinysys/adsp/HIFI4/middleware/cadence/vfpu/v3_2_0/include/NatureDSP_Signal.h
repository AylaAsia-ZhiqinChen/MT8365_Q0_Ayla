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
#ifndef __NATUREDSP_SIGNAL_H__
#define __NATUREDSP_SIGNAL_H__

#include "NatureDSP_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
  FIR filters and related functions:

  bkfir      Block real FIR filter 
  cxfir      Complex block FIR filter
  firdec     Decimating block real FIR filter
  firinterp  Interpolating block real FIR filter
  fir_convol Convolution
  fir_xcorr  Correlation
  fir_acorr  Autocorrelation 
  fir_blms   Blockwise Adaptive LMS algorithm for real data
===========================================================================*/

/*-------------------------------------------------------------------------
  Real FIR filter.
  Computes a real FIR filter (direct-form) using IR stored in vector h. 
  The real data input is stored in vector x. The filter output result is 
  stored in vector y. The filter calculates N output samples using M 
  coefficients and requires last M-1 samples in the delay line which is 
  updated in circular manner for each new sample.
  User has an option to set IR externally or copy from original location 
  (i.e. from the slower constant memory). In the first case, user is 
  responsible for right alignment, ordering and zero padding of filter 
  coefficients - usually array is composed from zeroes (left padding), 
  reverted IR and right zero padding.

  Precision: 
  24x24p   use 24-bit data packing for internal delay line buffer
           and internal coefficients storage
  32x16    32-bit data, 16-bit coefficients, 32-bit outputs
  32x32    32-bit data, 32-bit coefficients, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f        floating point

  Input:
  x[N]        input samples, Q31, Q15, floating point
  h[M]        filter coefficients in normal order, Q31, Q15, floating point
  N           length of sample block, should be a multiple of 4
  M           length of filter, should be a multiple of 4
  extIR       if zero, IR is copied from original location, otherwise 
              not but user should keep alignment, order of coefficients 
              and zero padding requirements shown below
  Output:
  y[N]      - input samples, Q31, Q15, floating point

  Alignment, ordering and zero padding for external IR  (extIR!=0)
  ------------------------+----------+--------------+--------------+----------------
  Function	              |Alignment,|Left zero     |   Coefficient| Right zero 
                          | bytes    |padding, bytes|   order      | padding, bytes
  ------------------------+----------+--------------+--------------+----------------
  bkfir24x24p_init        |     8    |((-M&4)+1)*3  |  inverted    |  7
  bkfir32x16_init (M>32)  |     8    |     10       |  inverted    |  6
  bkfir32x16_init (M<=32) |     8    |      2       |  inverted    |  6
  bkfir32x32_init         |     8    |      4       |  inverted    |  12
  bkfir32x32ep_init       |     8    |      4       |  inverted    |  12
  bkfirf_init             |     8    |      0       |  direct      |  0
  ------------------------+----------+--------------+--------------+----------------

  Restrictions:
  x,y should not be overlapping
  x,h - aligned on a 8-bytes boundary
  N,M - multiples of 4 
-------------------------------------------------------------------------*/
typedef void * bkfir24x24p_handle_t;
typedef void * bkfir32x16_handle_t;
typedef void * bkfir32x32_handle_t;
typedef void * bkfir32x32ep_handle_t;
typedef void * bkfirf_handle_t;

void bkfir24x24p_process  ( bkfir24x24p_handle_t  handle,     f24 * restrict y, const     f24   * restrict x, int N);
void bkfir32x16_process   ( bkfir32x16_handle_t   handle, int32_t * restrict y, const int32_t   * restrict x, int N);
void bkfir32x32_process   ( bkfir32x32_handle_t   handle, int32_t * restrict y, const int32_t   * restrict x, int N);
void bkfir32x32ep_process ( bkfir32x32ep_handle_t handle, int32_t * restrict y, const int32_t   * restrict x, int N);
void bkfirf_process     ( bkfirf_handle_t      handle, float32_t * restrict y , const float32_t * restrict x, int N);
/* Allocation routine for filters. Returns: size of memory in bytes to be allocated */
size_t bkfir24x24p_alloc  (int M, int extIR);
size_t bkfir32x16_alloc   (int M, int extIR);
size_t bkfir32x32_alloc   (int M, int extIR);
size_t bkfir32x32ep_alloc (int M, int extIR);
size_t bkfirf_alloc       (int M, int extIR); 

/* Initialization for filters. Returns: handle to the object */
bkfir24x24p_handle_t  bkfir24x24p_init (void * objmem, int M, int extIR, const    f24   * restrict h);
bkfir32x16_handle_t   bkfir32x16_init  (void * objmem, int M, int extIR, const int16_t  * restrict h);
bkfir32x32_handle_t   bkfir32x32_init  (void * objmem, int M, int extIR, const int32_t  * restrict h);
bkfir32x32ep_handle_t bkfir32x32ep_init(void * objmem, int M, int extIR, const int32_t  * restrict h);
bkfirf_handle_t       bkfirf_init      (void * objmem, int M, int extIR, const float32_t* restrict h);

/*-------------------------------------------------------------------------
  Real FIR filter.
  Computes a real FIR filter (direct-form) using IR stored in vector h. The 
  real data input is stored in vector x. The filter output result is stored 
  in vector y. The filter calculates N output samples using M coefficients 
  and requires last M+N-1 samples in the delay line.
  These functions implement FIR filter described in previous chapter with no 
  limitation on size of data block, alignment and length of impulse response 
  for the cost of performance.
  NOTE: user application is not responsible for management of delay lines

  Precision: 
  32x16    32-bit data, 16-bit coefficients, 32-bit outputs
  32x32    32-bit data, 32-bit coefficients, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f        floating point

  Input:
  x[N]      - input samples, Q31, floating point
  h[M]      - filter coefficients in normal order, Q31, Q15, floating point
  N         - length of sample block
  M         - length of filter
  Output:
  y[N]      - input samples, Q31, floating point 

  Restrictions:
  x,y should not be overlapping
-------------------------------------------------------------------------*/
typedef void * bkfira32x16_handle_t;
typedef void * bkfira32x32_handle_t;
typedef void * bkfira32x32ep_handle_t;
typedef void * bkfiraf_handle_t;

void bkfira32x16_process   (bkfira32x16_handle_t   handle, int32_t   * restrict y, const int32_t   * restrict x, int N);
void bkfira32x32_process   (bkfira32x32_handle_t   handle, int32_t   * restrict y, const int32_t   * restrict x, int N);
void bkfira32x32ep_process (bkfira32x32ep_handle_t handle, int32_t   * restrict y, const int32_t   * restrict x, int N);
void bkfiraf_process       (bkfiraf_handle_t       handle, float32_t * restrict y, const float32_t * restrict x , int N);

/* Allocation routine for filters. Returns: size of memory in bytes to be allocated */
size_t bkfira32x16_alloc  (int M);
size_t bkfira32x32_alloc  (int M);
size_t bkfira32x32ep_alloc(int M);
size_t bkfiraf_alloc      (int M);

/* Initialization for filters. Returns: handle to the object */
bkfira32x16_handle_t   bkfira32x16_init   (void * objmem, int M, const int16_t  * restrict h);
bkfira32x32_handle_t   bkfira32x32_init   (void * objmem, int M, const int32_t  * restrict h);
bkfira32x32ep_handle_t bkfira32x32ep_init (void * objmem, int M, const int32_t  * restrict h);
bkfira32x16_handle_t   bkfiraf_init       (void * objmem, int M, const float32_t* restrict h);

/*-------------------------------------------------------------------------
  Complex Block FIR Filter
  Computes a complex FIR filter (direct-form) using complex IR stored in 
  vector h. The complex data input is stored in vector x. The filter output 
  result is stored in vector r. The filter calculates N output samples using
  M coefficients, requires last N-1 samples in the delay line which is 
  updated in circular manner for each new sample. Real and imaginary parts 
  are interleaved and real parts go first (at even indexes).
  User has an option to set IR externally or copy from original location 
  (i.e. from the slower constant memory). In the first case, user is 
  responsible for right alignment, ordering and zero padding of filter 
  coefficients - usually array is composed from zeroes (left padding), 
  reverted IR and right zero padding.

  Precision: 
  32x16    32-bit data, 16-bit coefficients, 32-bit outputs
  32x32    32-bit data, 32-bit coefficients, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f        floating point

  Input:
  h[M]   complex filter coefficients; h[0] is to be multiplied with the newest 
         sample , Q31, Q15, floating point
  x[N]   input samples, Q31, floating point
  N      length of sample block (in complex samples) 
  M      length of filter 
  extIR  if zero, IR is copied from original location, otherwise 
         not but user should keep alignment, order of coefficients 
         and zero padding requirements shown below
  Output:
  y[N]   output samples, Q31, floating point

  Alignment, ordering and zero padding for external IR  (extIR!=0)
  -----------------+----------+--------------+--------------+----------------
  Function	       |Alignment,|Left zero     |   Coefficient| Right zero 
                   | bytes    |padding, bytes|   order      | padding, bytes
  -----------------+----------+--------------+--------------+----------------
  cxfir32x16_init  |     8    |    4         |  inverted    |  4
  cxfir32x32_init  |     8    |    0         | inverted and |  0
                   |          |              | conjugated   |
  cxfir32x32ep_init|     8    |    0         | inverted and |  0
                   |          |              | conjugated   |
  cxfirf_init      |     8    |    0         | direct       |  0
  -----------------+----------+--------------+--------------+----------------

  Restriction:
  x,y - should not overlap
  x,h - aligned on a 8-bytes boundary
  N,M - multiples of 4
-------------------------------------------------------------------------*/
typedef void* cxfir32x16_handle_t;
typedef void* cxfir32x32_handle_t;
typedef void* cxfir32x32ep_handle_t;
typedef void* cxfirf_handle_t;

void cxfir32x16_process  (cxfir32x16_handle_t   handle, complex_fract32 * restrict y, const complex_fract32 * restrict x,  int N);
void cxfir32x32_process  (cxfir32x32_handle_t   handle, complex_fract32 * restrict y, const complex_fract32 * restrict x,  int N);
void cxfir32x32ep_process(cxfir32x32ep_handle_t handle, complex_fract32 * restrict y, const complex_fract32 * restrict x,  int N);
void cxfirf_process      (cxfirf_handle_t       handle, complex_float   * restrict y, const complex_float   * restrict x , int N);

/* Allocation routine for filters. Returns: size of memory in bytes to be allocated */
size_t cxfir32x16_alloc  (int M, int extIR);
size_t cxfir32x32_alloc  (int M, int extIR);
size_t cxfir32x32ep_alloc(int M, int extIR);
size_t cxfirf_alloc      (int M, int extIR);

/* Initialization for filters. Returns: handle to the object */
cxfir32x16_handle_t   cxfir32x16_init  (void * objmem, int M, int extIR, const complex_fract16 * restrict h);
cxfir32x32_handle_t   cxfir32x32_init  (void * objmem, int M, int extIR, const complex_fract32 * restrict h);
cxfir32x32ep_handle_t cxfir32x32ep_init(void * objmem, int M, int extIR, const complex_fract32 * restrict h);
cxfirf_handle_t       cxfirf_init      (void * objmem, int M, int extIR, const complex_float  * restrict h);

/*-------------------------------------------------------------------------
  Decimating Block Real FIR Filter
  Computes a real FIR filter (direct-form) with decimation using IR stored 
  in vector h. The real data input is stored in vector x. The filter output 
  result is stored in vector r. The filter calculates N output samples using
  M coefficients and requires last D*N+M-1 samples on the delay line.
  NOTE:
  - To avoid aliasing IR should be synthesized in such a way to be narrower 
    than input sample rate divided to 2D.
  - user application is not responsible for management of delay lines

  Precision: 
  32x16    32-bit data, 16-bit coefficients, 32-bit outputs
  32x32    32-bit data, 32-bit coefficients, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f         floating point

  Input:
  h[M]          filter coefficients; h[0] is to be multiplied with the newest 
                sample, Q31, Q15, floating point
  D             decimation factor (has to be 2, 3 or 4)
  N             length of output sample block
  M             length of filter
  x[D*N]        input samples, Q31, floating point
  Output:
  y[N]          output samples, Q31, floating point

  Restriction:
  x,h,r should not overlap
  x, h - aligned on an 8-bytes boundary
  N - multiple of 8
  D should exceed 1

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  D - 2, 3 or 4
-------------------------------------------------------------------------*/
typedef void* firdec32x16_handle_t;
typedef void* firdec32x32_handle_t;
typedef void* firdec32x32ep_handle_t;
typedef void* firdecf_handle_t;

void firdec32x16_process  (firdec32x16_handle_t   handle, int32_t * restrict y, const int32_t   * x, int N);
void firdec32x32_process  (firdec32x32_handle_t   handle, int32_t * restrict y, const int32_t   * x, int N);
void firdec32x32ep_process(firdec32x32ep_handle_t handle, int32_t * restrict y, const int32_t   * x, int N);
void firdecf_process      (firdecf_handle_t     handle, float32_t * restrict y, const float32_t * x, int N);

/* Allocation routine for decimators. returns: size of memory in bytes to be allocated */
size_t firdec32x16_alloc  (int D, int M);
size_t firdec32x32_alloc  (int D, int M);
size_t firdec32x32ep_alloc(int D, int M);
size_t firdecf_alloc      (int D, int M);

/* Initialization of decimator. Returns: handle to the decimator object */
firdec32x16_handle_t   firdec32x16_init  (void * objmem, int D, int M, const int16_t   * restrict h);
firdec32x32_handle_t   firdec32x32_init  (void * objmem, int D, int M, const int32_t   * restrict h);
firdec32x32ep_handle_t firdec32x32ep_init(void * objmem, int D, int M, const int32_t   * restrict h);
firdecf_handle_t       firdecf_init      (void * objmem, int D, int M, const float32_t * restrict h);


/*-------------------------------------------------------------------------
  Interpolating Block Real FIR Filter
  Computes a real FIR filter (direct-form) with interpolation using IR stored 
  in vector h. The real data input is stored in vector x. The filter output 
  result is stored in vector y. The filter calculates N*D output samples 
  using M*D coefficients and requires last N+M*D-1 samples on the delay line.
  NOTE:
  user application is not responsible for management of delay lines

  Precision: 
  32x16    32-bit data, 16-bit coefficients, 32-bit outputs
  32x32    32-bit data, 32-bit coefficients, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f        floating point

  Input:
  h[M*D]        filter coefficients; h[0] is to be multiplied with the 
                newest sample,Q31, Q15, floating point
  D             interpolation ratio
  N             length of input sample block
  M             length of subfilter. Total length of filter is M*D
  x[N]          input samples,Q31, floating point
  Output:
  y[N*D]        output samples,Q31, floating point

  Restrictions:
  x,h,y should not overlap
  x,h - aligned on an 8-bytes boundary
  N   - multiple of 8
  M   - multiple of 4
  D should be >1

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  D   - 2, 3 or 4

-------------------------------------------------------------------------*/
typedef void* firinterp32x16_handle_t;
typedef void* firinterp32x32_handle_t;
typedef void* firinterp32x32ep_handle_t;
typedef void* firinterpf_handle_t;
void firinterp32x16_process  (firinterp32x16_handle_t   handle, int32_t  * restrict y, const int32_t  * restrict x, int N);
void firinterp32x32_process  (firinterp32x32_handle_t   handle, int32_t  * restrict y, const int32_t  * restrict x, int N);
void firinterp32x32ep_process(firinterp32x32ep_handle_t handle, int32_t  * restrict y, const int32_t  * restrict x, int N);
void firinterpf_process      (firinterpf_handle_t       handle, float32_t* restrict y, const float32_t* restrict x, int N);

/* Allocation routine for interpolator. returns: size of memory in bytes to be allocated */
size_t firinterp32x16_alloc  (int D, int M);
size_t firinterp32x32_alloc  (int D, int M);
size_t firinterp32x32ep_alloc(int D, int M);
size_t firinterpf_alloc      (int D, int M);

/* Allocation routine for interpolator. Returns: size of memory in bytes to be allocated */
firinterp32x16_handle_t   firinterp32x16_init    (void * objmem, int D, int M, const int16_t   * restrict h);
firinterp32x32_handle_t   firinterp32x32_init    (void * objmem, int D, int M, const int32_t   * restrict h);
firinterp32x32ep_handle_t firinterp32x32ep_init  (void * objmem, int D, int M, const int32_t   * restrict h);
firinterpf_handle_t       firinterpf_init        (void * objmem, int D, int M, const float32_t * restrict h);

/*-------------------------------------------------------------------------
  Circular Convolution
  Performs circular convolution between vectors x (of length N) and y (of 
  length M)  resulting in vector r of length N.

  Precision: 
  32x16    32x16-bit data, 32-bit outputs
  32x32    32x32-bit data, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f         floating point

  Input:
  x[N]          input data (Q31 or floating point)
  y[M]          input data (Q31,Q15 or floating point)
  N             length of x
  M             length of y
  Output:
  r[N]          output data,Q31 or floating point

  Restriction:
  x,y,r should not overlap
  x,y,r - aligned on an 8-bytes boundary
  N,M   - multiples of 4 and >0
-------------------------------------------------------------------------*/
void fir_convol32x32 (int32_t * restrict r,
            const int32_t * restrict x,
            const int32_t * restrict y,
            int N,
            int M);
void fir_convol32x32ep (int32_t * restrict r,
            const int32_t * restrict x,
            const int32_t * restrict y,
            int N,
            int M);
void fir_convol32x16 ( int32_t * restrict r,
            const int32_t * restrict x,
            const int16_t * restrict y,
            int N,
            int M);
void fir_convolf ( float32_t * restrict r,
             const float32_t * restrict x,
             const float32_t * restrict y,
             int N,
             int M);

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
#define FIR_CONVOLA32X16_SCRATCH_SIZE( N, M )    (((((N)+(M)-1)+1)&(~1))*4+((M)+8)*2)
#define FIR_CONVOLA32X32_SCRATCH_SIZE( N, M )      ( 4*( 2*(M) + (N) + 6 ) )
#define FIR_CONVOLA32X32EP_SCRATCH_SIZE( N, M )     (( (((N)+(M)-1 + 3)&~3) + (((M)+3)&~3) + 4)*4)
#define FIR_CONVOLAF_SCRATCH_SIZE( N, M )        ( 4*( 2*(M) + (N) + 6 ) )

void fir_convola32x16 (void    * restrict s,
                       int32_t * restrict r,
                 const int32_t * restrict x,
                 const int16_t * restrict y,
                 int N,
                 int M);
void fir_convola32x32 (void * restrict s,
                       int32_t  * restrict r,
                 const int32_t  * restrict x,
                 const int32_t  * restrict y,
                 int N,
                 int M);
void fir_convola32x32ep (void * restrict s,
                       int32_t  * restrict r,
                 const int32_t  * restrict x,
                 const int32_t  * restrict y,
                 int N,
                 int M);
void fir_convolaf     (void       * restrict s,
                       float32_t  * restrict r,
                 const float32_t  * restrict x,
                 const float32_t  * restrict y,
                 int N,
                 int M);

/*-------------------------------------------------------------------------
  Circular Correlation
  Estimates the circular cross-correlation between vectors x (of length N) 
  and y (of length M)  resulting in vector r of length N. It is a similar 
  to correlation but x is read in opposite direction.

  Precision: 
  32x16    32x16-bit data, 32-bit outputs
  32x32    32x32-bit data, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f         floating point

  Input:
  x[N]          input data Q31 or floating point
  y[M]          input data Q31,Q15 or floating point
  N             length of x
  M             length of y
  Output:
  r[N]          output data,Q31 or floating point

  Restriction:
  x,y,r should not overlap
  x,y,r - aligned on an 8-bytes boundary
  N,M   - multiples of 4 and >0
-------------------------------------------------------------------------*/
void fir_xcorr32x16 (  int32_t * restrict r,
            const int32_t * restrict x,
            const int16_t * restrict y,
            int N,
            int M);
void fir_xcorr32x32 (  int32_t * restrict r,
            const int32_t * restrict x,
            const int32_t * restrict y,
            int N,
            int M);
void fir_xcorr32x32ep (  int32_t * restrict r,
            const int32_t * restrict x,
            const int32_t * restrict y,
            int N,
            int M);
void fir_xcorrf (  float32_t * restrict r,
             const float32_t * restrict x,
             const float32_t * restrict y,
             int N,
             int M);
void cxfir_xcorrf ( complex_float * restrict r,
             const  complex_float * restrict x,
             const  complex_float * restrict y,
             int N,
             int M);

/*-------------------------------------------------------------------------
  Circular Correlation
  Estimates the circular cross-correlation between vectors x (of length N) 
  and y (of length M)  resulting in vector r of length N. It is a similar 
  to correlation but x is read in opposite direction.
  These functions implement the circular correlation algorithm described in
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
                  FIR_XCORRA32X32_SCRATCH_SIZE(N,M) or
                  FIR_XCORRA32X32EP_SCRATCH_SIZE(N,M) or
                  FIR_XCORRA32X16_SCRATCH_SIZE(N,M) or
                  FIR_XCORRAF_SCRATCH_SIZE(N,M) or
                  CXFIR_XCORRAF_SCRATCH_SIZE(N,M) bytes
  x[N]          input data Q31 or floating point
  y[M]          input data Q31, Q15 or floating point
  N             length of x
  M             length of y
  Output:
  r[N]          output data, Q31 or floating point

  Restrictions:
  x,y,r should not overlap
  s        - must be aligned on an 8-bytes boundary
  N,M      - must be >0
  N >= M-1 - minimum allowed length of vector x is the length of y minus one

  Notes:
  All functions may speculatively load 1 entry beyond the upper
  boundary of vector x, i.e. x[N+1]; and 1 entry beyond the lower
  boundary of vector y, i.e. y[-1].
-------------------------------------------------------------------------*/
#define FIR_XCORRA32X16_SCRATCH_SIZE( N, M )    (((((N)+(M)-1)+1)&(~1))*4+((M)+8)*2)
#define FIR_XCORRA32X32_SCRATCH_SIZE( N, M )    ( 4*( 2*(M) + (N) + 6 ) )
#define FIR_XCORRA32X32EP_SCRATCH_SIZE( N, M )    (( (((N)+(M)-1 + 3)&~3) + (((M)+3)&~3) + 4)*4)
#define FIR_XCORRAF_SCRATCH_SIZE( N, M )          ( 4*( (((N)+(M)) & ~1) + (((M)+3) & ~3) ) )
#define CXFIR_XCORRAF_SCRATCH_SIZE( N, M )        ( 8*( 2*(M) + (N) + 6 ) )

void fir_xcorra32x16 (void    * restrict s,
                      int32_t * restrict r,
                const int32_t * restrict x,
                const int16_t * restrict y,
                int N,
                int M);
void fir_xcorra32x32 (void * restrict s,
                       int32_t * restrict r,
                 const int32_t * restrict x,
                 const int32_t * restrict y,
                 int N,
                 int M);
void fir_xcorra32x32ep (void * restrict s,
                       int32_t * restrict r,
                 const int32_t * restrict x,
                 const int32_t * restrict y,
                 int N,
                 int M);
void fir_xcorraf      (void      * restrict s,
                       float32_t * restrict r,
                 const float32_t * restrict x,
                 const float32_t * restrict y,
                 int N,
                 int M);
void cxfir_xcorraf    (void          * restrict s,
                       complex_float * restrict r,
                 const complex_float * restrict x,
                 const complex_float * restrict y,
                 int N,
                 int M);

/*-------------------------------------------------------------------------
  Circular Autocorrelation 
  Estimates the auto-correlation of vector x. Returns autocorrelation of 
  length N.

  Precision: 
  32x32    32x32-bit data, 32-bit outputs
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f        floating point

  Input:
  x[N]      input data, Q31 or floating point
  N         length of x
  Output:
  r[N]      output data, Q31 or floating point

  Restrictions:
  x,r should not overlap
  x,r - aligned on an 8-bytes boundary
  N   - multiple of 4 and >0
-------------------------------------------------------------------------*/
void fir_acorr32x32 (   int32_t * restrict r,
                  const int32_t * restrict x,
                  int N);
void fir_acorr32x32ep (   int32_t * restrict r,
                  const int32_t * restrict x,
                  int N);
void fir_acorrf (       float32_t* restrict r,
                  const float32_t* restrict x,
                  int N);

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
#define FIR_ACORRA32X32_SCRATCH_SIZE( N )      (((N)&1) ? FIR_XCORRA32X32_SCRATCH_SIZE(N,N) : (2*(N)+4)*4)
#define FIR_ACORRA32X32EP_SCRATCH_SIZE( N )    (((N)&1) ? FIR_XCORRA32X32EP_SCRATCH_SIZE(N,N) : (2*(N)+4)*4)
#define FIR_ACORRAF_SCRATCH_SIZE( N )           4*( 2*(N) + (((N)+3) & ~3) )
void fir_acorra32x32 ( void * restrict s,
                       int32_t* restrict r,
                 const int32_t* restrict x,
                 int N );
void fir_acorra32x32ep ( void * restrict s,
                       int32_t* restrict r,
                 const int32_t* restrict x,
                 int N );
void fir_acorraf     ( void       * restrict s,
                       float32_t  * restrict r,
                 const float32_t  * restrict x,
                 int N );

/*-------------------------------------------------------------------------
  Blockwise Adaptive LMS Algorithm for Real Data
  Blockwise LMS algorithm performs filtering of reference samples x[N+M-1],
  computation of error e[N] over a block of input samples r[N] and makes
  blockwise update of IR to minimize the error output.
  Algorithm includes FIR filtering, calculation of correlation between the 
  error output e[N] and reference signal x[N+M-1] and IR taps update based
  on that correlation.
NOTES: 
  1. The algorithm must be provided with the normalization factor, which is
     the power of the reference signal times N - the number of samples in a
     data block. This can be calculated using the vec_power32x32() or 
     vec_power16x16() function. In order to avoid the saturation of the 
     normalization factor, it may be biased, i.e. shifted to the right.
     If it's the case, then the adaptation coefficient must be also shifted
     to the right by the same number of bit positions.
  2. this algorithm consumes less CPU cycles per block than single 
     sample algorithm at similar convergence rate.
  3. Right selection of N depends on the change rate of impulse response:
     on static or slow varying channels convergence rate depends on
     selected mu and M, but not on N.

  Precision: 
  16x32    32-bit coefficients, 16-bit data, 16-bit output
  32x32    32-bit coefficients, 32-bit data, 32-bit output
  32x32ep  the same as above but using 72-bit accumulator for intermediate 
           computations
  f         floating point

  Input:
  h[M]     impulse response, Q31 or floating point
  r[N]	   input data vector (near end). First in time value is in 
           r[0], 32bit or 16-bit, Qx or floating point
  x[N+M-1] reference data vector (far end). First in time value is in x[0],  
           32bit or 16-bit, Qx or floating point
  norm     normalization factor: power of signal multiplied by N, 32-bit  
           or floating point
           Fixed-point format for the 32x32-bit variant: Q(2*x-31-bias)
           Fixed-point format for the 32x16-bit variant: Q(2*x+1-bias)
  mu       adaptation coefficient (LMS step), Q(31-bias) or Q(15-bias)
  N        length of data block
  M        length of h
  Output:
  e[N]     estimated error, Q31,Q15 or floating point
  h[M]     updated impulse response, Q31 or floating point

  Restriction:
  x,r,h,e - should not overlap
  x,r,h,e - aligned on a 8-bytes boundary
  N,M     - multiples of 8 and >0
-------------------------------------------------------------------------*/
void fir_blms32x32 ( int32_t* restrict e, int32_t* restrict h,
                const int32_t* restrict r,
                const int32_t* restrict x,
                int32_t norm,
                int32_t mu,
                int   N,
                int   M);
void fir_blms32x32ep ( int32_t* restrict e, int32_t* restrict h,
                const int32_t* restrict r,
                const int32_t* restrict x,
                int32_t norm,
                int32_t mu,
                int   N,
                int   M);
void fir_blms16x32 ( int32_t * restrict e, int32_t * restrict h,
                const int16_t * restrict r,
                const int16_t * restrict x,
                int32_t   norm,
                int16_t   mu,
                int   N,
                int   M);
void fir_blmsf      ( float32_t * e, float32_t * h, const float32_t * r,
                const float32_t * x, 
                float32_t norm, float32_t mu, 
                int          N, int       M );

/*===========================================================================
  IIR filters:
  bqriir     Biquad real block IIR
  latr       Lattice block Real IIR
===========================================================================*/

/*-------------------------------------------------------------------------
  Biquad Real IIR
  Computes a IIR filter (cascaded IIR direct form I or II using 5 
  coefficients per biquad + gain term). Real input data are stored
  in vector x. The filter output result is stored in vector r. The filter 
  calculates N output samples using SOS and G matrices.
  NOTES:
  1. Biquad coefficients may be derived from standard SOS and G matrices 
  generated by MATLAB. However, typically biquad stages have big peaks 
  in their step response which may cause undesirable overflows at the 
  intermediate outputs. To avoid that the additional scale factors 
  coef_g[M] may be applied. These per-section scale factors may require 
  some tuning to find a compromise between quantization noise and possible 
  overflows. Output of the last section is directed to an additional 
  multiplier, with the gain factor being a power of two, either negative 
  or non-negative. It is specified through the gain parameter of filter
  initialization function.

  Precision: 
  32x16 32-bit data, 16-bit coefficients, 32-bit intermediate stage outputs
  32x32 32-bit data, 32-bit coefficients, 32-bit intermediate stage outputs 
  f     floating point (DF I, DF II and DF IIt)

  Input:
  N             length of input sample block
  M             number of biquad sections
  coef_sos[M*5] filter coefficients stored in blocks of 5 numbers: 
                b0 b1 b2 a1 a2. 
                For fixed-point funcions, fixed point format of filter 
                coefficients is Q1.14 for 32x16, or Q1.30 for 32x16 and 
                32x32 
  coef_g[M]     scale factor for each section, Q15 (for fixed-point 
                functions only). 
  gain          total gain shift amount applied to output signal of the
                last section, -48..15
  x[N]          input samples, Q31 or floating point 
  Output:
  r[N]          output data, Q31 or floating point 

  Restriction:
  x,r,coef_g,coef_sos  must not overlap
  x,r                  must be aligned on 8-byte boundary 
  N                    must be a multiple of 2
-------------------------------------------------------------------------*/

typedef void* bqriir32x16_df1_handle_t;
typedef void* bqriir32x16_df2_handle_t;
typedef void* bqriir32x32_df1_handle_t;
typedef void* bqriir32x32_df2_handle_t;
typedef void* bqriirf_df1_handle_t;
typedef void* bqriirf_df2_handle_t;
typedef void* bqriirf_df2t_handle_t;
typedef void* bqciirf_df1_handle_t;

void bqriir32x16_df1_process(bqriir32x16_df1_handle_t _bqriir,int32_t * restrict r,const int32_t *x, int N);
void bqriir32x16_df2_process(bqriir32x16_df2_handle_t _bqriir,int32_t * restrict r,const int32_t *x, int N);
void bqriir32x32_df1_process(bqriir32x32_df1_handle_t _bqriir,int32_t * restrict r,const int32_t *x, int N);
void bqriir32x32_df2_process(bqriir32x32_df2_handle_t _bqriir,int32_t * restrict r,const int32_t *x, int N);
void bqriirf_df1_process (bqriirf_df1_handle_t     _bqriir,                  float32_t     * restrict r, const float32_t     * x, int N);
void bqriirf_df2_process (bqriirf_df2_handle_t     _bqriir,                  float32_t     * restrict r, const float32_t     * x, int N);
void bqriirf_df2t_process(bqriirf_df2t_handle_t    _bqriir,                  float32_t     * restrict r, const float32_t     * x, int N);
void bqciirf_df1_process (bqciirf_df1_handle_t     _bqriir,                  complex_float * restrict r, const complex_float * x, int N);

/* Allocation routine for iir filters. Returns: size of memory in bytes to be allocated */
size_t bqriir32x16_df1_alloc(int M);
size_t bqriir32x16_df2_alloc(int M);
size_t bqriir32x32_df1_alloc(int M);
size_t bqriir32x32_df2_alloc(int M);
size_t bqriirf_df1_alloc(int M);
size_t bqriirf_df2_alloc(int M);
size_t bqriirf_df2t_alloc(int M);
size_t bqciirf_df1_alloc(int M);

/* Initialization routine for iir filters. Returns: handle to the object */
bqriir32x16_df1_handle_t bqriir32x16_df1_init(void * objmem, int M, 
                                              const int16_t * coef_sos,
                                              const int16_t * coef_g,
                                              int16_t         gain );
bqriir32x16_df2_handle_t bqriir32x16_df2_init(void * objmem,  int M,
                                              const int16_t * coef_sos,
                                              const int16_t * coef_g,
                                              int16_t         gain );
bqriir32x32_df1_handle_t bqriir32x32_df1_init(void * objmem,  int M, 
                                              const int32_t * coef_sos,
                                              const int16_t * coef_g,
                                              int16_t         gain );
bqriir32x32_df2_handle_t bqriir32x32_df2_init(void * objmem,  int M, 
                                              const int32_t * coef_sos,
                                              const int16_t * coef_g,
                                              int16_t         gain );
bqriirf_df1_handle_t bqriirf_df1_init        (void * objmem, int M,
                                              const float32_t* coef_sos, int16_t gain );
bqriirf_df2_handle_t bqriirf_df2_init        (void * objmem, int M,
                                              const float32_t * coef_sos, int16_t gain);
bqriirf_df2t_handle_t bqriirf_df2t_init      (void * objmem, int M, 
                                              const float32_t * coef_sos, int16_t gain);
bqciirf_df1_handle_t bqciirf_df1_init        (void * objmem, int M,
                                              const float32_t * coef_sos, int16_t gain);

/*-------------------------------------------------------------------------
  Lattice Block Real IIR
  Computes a real cascaded lattice autoregressive IIR filter using reflection 
  coefficients stored in vector k. The real data input are stored in vector x.
  The filter output result is stored in vector r.  Input scaling is done before 
  the first cascade for normalization and overflow protection..

  Precision: 
  32x32  32-bit data, 32-bit coefficients
  32x16  32-bit data, 16-bit coefficients
  f      single precision floating point

  Input:
  N      length of input sample block
  M      number of reflection coefficients
  scale  input scale factor g, Q31, Q15 or floating point
  k[M]   reflection coefficients, Q31, Q15 or floating point
  x[N]   input samples, Q31 or floating point
  Output:
  r[N]   output data, Q31 or floating point

  Restriction:
  x,r,k should not overlap

  PERFORMANCE NOTE:
  for optimum performance follow rules:
  M - from the range 1...8
-------------------------------------------------------------------------*/
typedef void* latr32x32_handle_t;
typedef void* latr32x16_handle_t;
typedef void* latrf_handle_t;
void latr32x32_process (latr32x32_handle_t handle, int32_t   * r, const int32_t   * x, int N);
void latr32x16_process (latr32x16_handle_t handle, int32_t   * r, const int32_t   * x, int N);
void latrf_process     (latrf_handle_t     handle, float32_t * r, const float32_t * x, int N);

/* Allocation routine for lattice filters. Returns: size of memory in bytes to be allocated */
size_t latr32x32_alloc(int M);
size_t latr32x16_alloc(int M);
size_t latrf_alloc    (int M);

/* Initialization routine for lattice filters. Returns: handle to the object */
latr32x32_handle_t latr32x32_init(void * objmem, int M, const int32_t * restrict k, int32_t scale);
latr32x16_handle_t latr32x16_init(void * objmem, int M, const int16_t * restrict k, int16_t scale);
latrf_handle_t     latrf_init    (void * objmem, int M, const float32_t * restrict k, float32_t scale);

/*===========================================================================
  Vector matematics:
  vec_dot              Vector Dot Product
  vec_add              Vector Sum
  vec_power            Power of a Vector
  vec_shift,vec_scale  Vector Scaling with Saturation
  vec_recip            Reciprocal on Q31/Q15 Numbers
  vec_divide           Division of Q31/Q15 Numbers
  vec_log              Logarithm 
  vec_antilog          Antilogarithm         
  vec_sqrt             Square Root
  vec_sine,vec_cosine  Sine/Cosine
  vec_tan              Tangent 
  vec_atan             Arctangent 
  vec_bexp             Common Exponent
  vec_min,vec_max      Vector Min/Max
  vec_poly             Vector polynomial approximation
===========================================================================*/

/*-------------------------------------------------------------------------
  Vector Dot product
  Two versions of routines are available: regular versions (without suffix _fast)
  that work with arbitrary arguments, faster versions 
  (with suffix _fast) apply some restrictions.

  Precision: 
  64x32  64x32-bit data, 64-bit output (fractional multiply Q63xQ31->Q63)
  64x64  64x64-bit data, 64-bit output (fractional multiply Q63xQ63->Q63)
  64x64i 64x64-bit data, 64-bit output (low 64 bit of integer multiply)
  32x32  32x32-bit data, 64-bit output
  32x16  32x16-bit data, 64-bit output
  16x16  16x16-bit data, 64-bit output for regular version and 32-bit for 
                        fast version
  f      single precision floating point

  Input:
  x[N]  input data, Q31 or floating point
  y[N]  input data, Q31, Q15, or floating point
  N	length of vectors
  Returns: dot product of all data pairs, Q31 or Q63

  Restrictions:
  Regular versions (without suffix _fast):
  None
  Faster versions (with suffix _fast):
  x,y - aligned on 8-byte boundary
  N   - multiple of 4

  vec_dot16x16_fast utilizes 32-bit saturating accumulator, so, input data 
  should be scaled properly to avoid erroneous results
-------------------------------------------------------------------------*/
int64_t   vec_dot64x32 (const int64_t   * restrict x,const int32_t   * restrict y,int N);
int64_t   vec_dot64x64 (const int64_t   * restrict x,const int64_t   * restrict y,int N);
int64_t   vec_dot64x64i(const int64_t   * restrict x,const int64_t   * restrict y,int N);
int64_t   vec_dot32x32 (const int32_t   * restrict x,const int32_t   * restrict y,int N);
int64_t   vec_dot32x16 (const int32_t   * restrict x,const int16_t   * restrict y,int N);
int64_t   vec_dot16x16 (const int16_t   * restrict x,const int16_t   * restrict y,int N);
float32_t vec_dotf     (const float32_t * restrict x,const float32_t * restrict y,int N);

int64_t vec_dot64x32_fast (const int64_t * restrict x,const int32_t * restrict y,int N);
int64_t vec_dot64x64_fast (const int64_t * restrict x,const int64_t * restrict y,int N);
int64_t vec_dot64x64i_fast(const int64_t * restrict x,const int64_t * restrict y,int N);
int64_t vec_dot32x32_fast (const int32_t * restrict x,const int32_t * restrict y,int N);
int64_t vec_dot32x16_fast (const int32_t * restrict x,const int16_t * restrict y,int N);
int32_t vec_dot16x16_fast (const int16_t * restrict x,const int16_t * restrict y,int N);

/*-------------------------------------------------------------------------
  Vector Sum
  This routine makes pair wise saturated summation of vectors.
  Two versions of routines are available: regular versions (vec_add32x32, 
  vec_add16x16, vec_addf) work with arbitrary arguments, faster versions 
  (vec_add32x32_fast, vec_add16x16_fast) apply some 
  restrictions.

  Precision: 
  32x32 32-bit inputs, 32-bit output
  16x16 16-bit inputs, 16-bit output
  f     single precision floating point

  Input:
  x[N]   input data
  y[N]   input data
  N      length of vectors
  Output:
  z[N]   output data

  Restriction:
  Regular versions (vec_add32x32, vec_add16x16, vec_addf):
  x,y,z - should not be overlapped
  Faster versions (vec_add32x32_fast, vec_add16x16_fast):
  z,x,y - aligned on 8-byte boundary
  N   - multiple of 4
-------------------------------------------------------------------------*/
void vec_add32x32 ( int32_t * restrict z,
              const int32_t * restrict x,
              const int32_t * restrict y,
              int N);
void vec_add32x32_fast
            ( int32_t * restrict z,
              const int32_t * restrict x,
              const int32_t * restrict y,
              int N );
void vec_addf ( float32_t * restrict z,
              const float32_t * restrict x,
              const float32_t * restrict y,
              int N);
void vec_add16x16 ( int16_t * restrict z,
              const int16_t * restrict x,
              const int16_t * restrict y,
              int N);
void vec_add16x16_fast
            ( int16_t * restrict z,
              const int16_t * restrict x,
              const int16_t * restrict y,
              int N );

/*-------------------------------------------------------------------------
  Power of a Vector
  This routine computes power of vector with scaling output result by rsh 
  bits. Accumulation is done in 64-bit wide accumulator and output may 
  scaled down with saturation by rsh bits. So, if representation of x 
  input is Qx, result will be represented in Q(2x-rsh) format.
  Two versions of routines are available: regular versions (vec_power32x32, 
  vec_power16x16, vec_powerf) work with arbitrary arguments, faster versions 
  (vec_power32x32_fast, vec_power16x16_fast) apply some restrictions.

  Precision: 
  32x32 32x32-bit data, 64-bit output
  16x16 16x16-bit data, 64-bit output
  f     single precision floating point

  Input:
  x[N]  input data, Q31, Q15 or floating point
  rsh   right shift of result
  N     length of vector

  Returns: Sum of squares of a vector, Q(2x-rsh)

  Restrictions:
  for vec_power32x32(): rsh in range 31...62
  for vec_power16x16(): rsh in range 0...31
  For regular versions (vec_power32x32, vec_power16x16, vec_powerf):
  none

  For faster versions (vec_power32x32_fast, vec_power16x16_fast ):
  x - aligned on 8-byte boundary
  N   - multiple of 4
-------------------------------------------------------------------------*/
int64_t     vec_power32x32 ( const int32_t * restrict x,int rsh,int N);
int64_t     vec_power16x16 ( const int16_t * restrict x,int rsh,int N);
float32_t   vec_powerf     ( const float32_t * restrict x,int N);

int64_t     vec_power32x32_fast ( const int32_t * restrict x,int rsh,int N);
int64_t     vec_power16x16_fast ( const int16_t * restrict x,int rsh,int N);

/*-------------------------------------------------------------------------
  Vector Scaling with Saturation
  These routines make shift with saturation of data values in the vector 
  by given scale factor (degree of 2). 
  Functions vec_scale() make multiplication of Q31 vector to Q31 coefficient 
  which is not a power of 2 forming Q31 result.
  Two versions of routines are available: regular versions (vec_shift32x32, 
  vec_shift16x16, vec_shiftf, vec_scale16x16, vec_scale32x32, vec_scalef, 
  vec_scale_sf) work with arbitrary arguments, faster versions 
  (vec_shift32x32_fast, vec_shift16x16_fast, vec_scale16x16_fast, 
  vec_scale32x32_fast) apply some additional restricitons.

  For floating point:
  Function vec_shiftf makes shift without saturation of data values in the vector
  by given scale factor (degree of 2). 
  Functions vec_scalef() and vec_scale_sf() make multiplication of input vector
  to coefficient which is not a power of 2.
  Two versions of routines are available: 
    without saturation - vec_scalef;
    with saturation - vec_scale_sf; 

Precision:
32x32 32-bit input, 32-bit output
16x16 16-bit input, 16-bit output
f     single precision floating point

  Input:
  x[N]  input data , Q31,Q15 or floating point
  t     shift count. If positive, it shifts left with saturation, if
  negative it shifts right
  s     scale factor, Q31,Q15 or floating point
  N     length of vector
  fmin  minimum output value (only for vec_scale_sf)
  fmax  maximum output value (only for vec_scale_sf)

  Output:
  y[N]  output data, Q31, Q15 or floating point

  Restrictions:
  For regular versions (vec_shift32x32, vec_shift16x16, vec_shiftf, 
  vec_scale16x16, vec_scale32x32, vec_scalef, vec_scale_sf):
  x,y should not overlap
  t   should be in range -31...31 for fixed-point functions and -129...146 
      for floating point

  For vec_scale_sf - fmin<=fmax;

  For faster versions (vec_shift32x32_fast, vec_shift16x16_fast, 
  vec_scale16x16_fast, vec_scale32x32_fast):
  x,y should not overlap
  t should be in range -31...31 
  x,y - aligned on 8-byte boundary
  N   - multiple of 4 
-------------------------------------------------------------------------*/
void vec_shift32x32 (     int32_t * restrict y,
                    const int32_t * restrict x,
                    int t,
                    int N);
void vec_shift16x16 (     int16_t * restrict y,
                    const int16_t * restrict x,
                    int t,
                    int N);
void vec_shiftf     (     float32_t * restrict y,
                    const float32_t * restrict x,
                    int t,
                    int N);
void vec_scale16x16 (     int16_t * restrict y,
                    const int16_t * restrict x,
                    int16_t s,
                    int N);
void vec_scale32x32 (     int32_t * restrict y,
                    const int32_t * restrict x,
                    int32_t s,
                    int N);
void vec_scalef     (     float32_t * restrict y,
                    const float32_t * restrict x,
                    float32_t s,
                    int N);
void vec_scale_sf   (     float32_t * restrict y,
                    const float32_t * restrict x,
                    float32_t s, float32_t fmin, float32_t fmax,
                    int N);
void vec_shift32x32_fast
                  ( int32_t * restrict y,
                    const int32_t * restrict x,
                    int t,
                    int N );
void vec_shift16x16_fast
                  ( int16_t * restrict y,
                    const int16_t * restrict x,
                    int t,
                    int N );
void vec_scale16x16_fast
                  ( int16_t * restrict y,
                    const int16_t * restrict x,
                    int16_t s,
                    int N );
void vec_scale32x32_fast
                  ( int32_t * restrict y,
                    const int32_t * restrict x,
                    int32_t s,
                    int N );

/*-------------------------------------------------------------------------
  Reciprocal on Q31/Q15 Numbers
  These routines return the fractional and exponential portion of the 
  reciprocal of a vector x of Q31 or Q15 numbers. Since the reciprocal is 
  always greater than 1, it returns fractional portion frac in Q(31-exp) 
  or Q(15-exp) format and exponent exp so true reciprocal value in the 
  Q0.31/Q0.15 may be found by shifting fractional part left by exponent 
  value.

  Mantissa accuracy is 1 LSB, so relative accuracy is:
  vec_recip16x16, scl_recip16x16                   6.2e-5 
  scl_recip32x32                                   2.4e-7 
  vec_recip32x32                                   9.2e-10

  Precision: 
  32x32  32-bit input, 32-bit output. 
  16x16  16-bit input, 16-bit output. 

  Input:
  x[N]    input data, Q31 or Q15
  N       length of vectors

  Output:
  frac[N] fractional part of result, Q(31-exp) or Q(15-exp)
  exp[N]  exponent of result 

  Restriction:
  x,frac,exp should not overlap

  Scalar versions:
  ----------------
  Return packed value: 
  scl_recip32x32():
  bits 23:0 fractional part
  bits 31:24 exponent
  scl_recip16x16():
  bits 15:0 fractional part
  bits 31:16 exponent
-------------------------------------------------------------------------*/
void vec_recip32x32 (
                  int32_t * restrict frac, 
                  int16_t *exp, 
                  const int32_t * restrict x, 
                  int N);
void vec_recip16x16 (
                  int16_t * restrict frac, 
                  int16_t *exp, 
                  const int16_t * restrict x, 
                  int N);
uint32_t scl_recip32x32 (int32_t x);
uint32_t scl_recip16x16 (int16_t x);

/*-------------------------------------------------------------------------
  Division
  These routines perform pair wise division of vectors. Operands are 
  represented in Q31 or Q15 format. It returns the fractional and exponential 
  portion of the division result. Since the division may generate result 
  greater than 1, it returns fractional portion frac in Q(31-exp) or or 
  Q(15-exp) format and exponent exp so true division result in the Q0.31 may 
  be found by shifting fractional part left by exponent value.
  Additional routine makes integer division of 64-bit number to 32-bit 
  denominator forming 32-bit result. If result is overflown, 0x7fffffff 
  or 0x80000000 is returned depending on the signs of inputs.
  For division to 0, the result is not defined 

  Two versions of routines are available: regular versions (vec_divide64x32i, 
  vec_divide32x32, vec_divide16x16) work with arbitrary arguments, faster 
  versions (vec_divide32x32_fast, vec_divide16x16_fast) apply some 
  restrictions.

  Accuracy is measured as accuracy of fractional part (mantissa):
  vec_divide64x32i,scl_divide64x32                      :   1 LSB   
  vec_divide32x32,vec_divide32x32_fast                  :   2 LSB (1.8e-9) 
  scl_divide32x32                                       :   2 LSB (4.8e-7) 
  vec_divide16x16, scl_divide16x16,vec_divide16x16_fast :   2 LSB (1.2e-4)

  Precision: 
  64x32i integer division, 64-bit nominator, 32-bit denominator, 32-bit output. 
  32x32  fractional division, 32-bit inputs, 32-bit output. 
  16x16  fractional division, 16-bit inputs, 16-bit output. 

  Input:
  x[N]    nominator,64-bit integer, Q31 or Q15
  y[N]    denominator,32-bit integer,Q31 or Q15
  N       length of vectors
  Output:
  frac[N] fractional parts of result, Q(31-exp) or Q(15-exp)
  exp[N]  exponents of result 

  Restriction:
  For regular versions (vec_divide64x32i, vec_divide32x32, vec_divide16x16) :
  x,y,frac,exp should not overlap

  For faster versions (vec_divide32x3_fast, vec_divide16x16_fast) :
  x,y,frac,exp should not overlap
  x, y, frac to be aligned by 8-byte boundary, N - multiple of 4.

  Scalar versions:
  ----------------
  scl_divide64x32i(): integer remainder
  scl_divide32x32() : packed value: bits 23-0 fractional part, 
                      bits 31-24 exponent
  scl_divide16x16() : packed value: bits 15-0 fractional part, 
                      bits 31-16 exponent
-------------------------------------------------------------------------*/
void vec_divide64x32i
                (int32_t * restrict frac, 
                 const int64_t * restrict x, 
                 const int32_t * restrict y, int N);
void vec_divide32x32 
                (int32_t * restrict frac, 
                 int16_t *exp, 
                 const int32_t * restrict x, 
                 const int32_t * restrict y, int N);
void vec_divide16x16 
                (int16_t * restrict frac, 
                 int16_t *exp, 
                 const int16_t * restrict x, 
                 const int16_t * restrict y, int N);
void vec_divide32x32_fast 
                (int32_t * restrict frac, 
                 int16_t *exp, 
                 const int32_t * restrict x, 
                 const int32_t * restrict y, int N);
void vec_divide16x16_fast 
                (int16_t * restrict frac, 
                 int16_t *exp, 
                 const int16_t * restrict x, 
                 const int16_t * restrict y, int N);

int32_t  scl_divide64x32(int64_t x,int32_t y);
uint32_t scl_divide32x32(int32_t x,int32_t y);
uint32_t scl_divide16x16(int16_t x,int16_t y);

/*-------------------------------------------------------------------------
  Logarithm:
  Different kinds of logarithm (base 2, natural, base 10). Fixed point 
  functions represent results in Q25 format or return 0x80000000 on negative 
  of zero input.

  Precision:
  32x32  32-bit inputs, 32-bit outputs
  f      floating point

  Accuracy :
  vec_log2_32x32,scl_log2_32x32       730 (2.2e-5)
  vec_logn_32x32,scl_logn_32x32       510 (1.5e-5)
  vec_log10_32x32,scl_log10_32x32     230 (6.9e-6)
  floating point                      2 ULP

  NOTES:
  1.  Scalar Floating point functions are compatible with standard ANSI C routines 
      and set errno and exception flags accordingly.
  2.  Floating point functions limit the range of allowable input values:
      A) If x<0, the result is set to NaN. In addition, scalar floating point
         functions assign the value EDOM to errno and raise the "invalid" 
         floating-point exception.
      B) If x==0, the result is set to minus infinity. Scalar floating  point
         functions assign the value ERANGE to errno and raise the "divide-by-zero"
         floating-point exception.

  Input:
  x[N]  input data, Q16.15 or floating point 
  N     length of vectors
  Output:
  y[N]  result, Q25 or floating point 

  Restriction:
  x,y should not overlap

  Scalar versions:
  ----------------
  return result in Q25 or floating point
-------------------------------------------------------------------------*/
void vec_log2_32x32 (int32_t * restrict y,const int32_t * restrict x, int N);
void vec_logn_32x32 (int32_t * restrict y,const int32_t * restrict x, int N);
void vec_log10_32x32(int32_t * restrict y,const int32_t * restrict x, int N);
void vec_log2f     (float32_t * restrict y,const float32_t * restrict x, int N);
void vec_lognf     (float32_t * restrict y,const float32_t * restrict x, int N);
void vec_log10f    (float32_t * restrict y,const float32_t * restrict x, int N);
int32_t scl_log2_32x32 (int32_t x);
int32_t scl_logn_32x32 (int32_t x);
int32_t scl_log10_32x32(int32_t x);
float32_t scl_log2f (float32_t x);
float32_t scl_lognf (float32_t x);
float32_t scl_log10f(float32_t x);

/*-------------------------------------------------------------------------
  Antilogarithm
  These routines calculate antilogarithm (base2, natural and base10). 
  Fixed-point functions accept inputs in Q25 and form outputs in Q16.15 
  format and return 0x7FFFFFFF in case of overflow and 0 in case of 
  underflow.

  Precision:
  32x32  32-bit inputs, 32-bit outputs. Accuracy: 4*e-5*y+1LSB
  f      floating point: Accuracy: 2 ULP
  NOTE:
  1.  Floating point functions are compatible with standard ANSI C routines 
      and set errno and exception flags accordingly

  Input:
  x[N]  input data,Q25 or floating point 
  N     length of vectors
  Output:
  y[N]  output data,Q16.15 or floating point  

  Restriction:
  x,y should not overlap

  Scalar versions:
  ----------------
  fixed point functions return result in Q16.15

-------------------------------------------------------------------------*/
void vec_antilog2_32x32 (int32_t * restrict y, const int32_t* restrict x, int N);
void vec_antilogn_32x32 (int32_t * restrict y, const int32_t* restrict x, int N);
void vec_antilog10_32x32(int32_t * restrict y, const int32_t* restrict x, int N);
void vec_antilog2f (float32_t * restrict y, const float32_t* restrict x, int N);
void vec_antilognf (float32_t * restrict y, const float32_t* restrict x, int N);
void vec_antilog10f(float32_t * restrict y, const float32_t* restrict x, int N);
int32_t scl_antilog2_32x32 (int32_t x);
int32_t scl_antilogn_32x32 (int32_t x);
int32_t scl_antilog10_32x32(int32_t x);
float32_t scl_antilog2f (float32_t x);
float32_t scl_antilognf (float32_t x);
float32_t scl_antilog10f(float32_t x);

/*-------------------------------------------------------------------------
  Square Root
  These routines calculate square root.
  NOTE: functions return 0x80000000 on negative argument
  Two versions of functions available: regular version (vec_sqrt32x32) with 
  arbitrary arguments and faster version (vec_sqrt32x32_fast) that apply some 
  restrictions.

  Precision: 
  32x32  32-bit inputs, 32-bit output. Accuracy: (2.6e-7*y+1LSB)

  Input:
  x[N]  input data,Q31 
  N     length of vectors
  Output:
  y[N]  output data,Q31

  Restriction:
  Regular versions (vec_sqrt32x32):
  x,y - should not overlap

  Faster versions (vec_sqrt32x32_fast):
  x,y - should not overlap
  x,y - aligned on 8-byte boundary
  N   - multiple of 2

  Scalar versions:
  ----------------
  return result, Q31
-------------------------------------------------------------------------*/
void vec_sqrt32x32 (int32_t* restrict y, const int32_t* restrict x, int N);
void vec_sqrt32x32_fast (int32_t* restrict y, const int32_t* restrict x, int N);
int32_t scl_sqrt32x32(int32_t x);

/*-------------------------------------------------------------------------
  Sine/Cosine 
  Fixed-point functions calculate sin(pi*x) or cos(pi*x) for numbers written 
  in Q31 or Q15 format. Return results in the same format. 
  Floating point functions compute sin(x) or cos(x)
  Two versions of functions available: regular version (vec_sine32x32, 
  vec_cosine32x32, , vec_sinef, vec_cosinef) 
  with arbitrary arguments and faster version (vec_sine32x32_fast, 
  vec_cosine32x32_fast) that 
  apply some restrictions.
  NOTE:
  1.  Scalar floating point functions are compatible with standard ANSI C
      routines and set errno and exception flags accordingly
  2.  Floating point functions limit the range of allowable input values:
      [-102940.0, 102940.0] Whenever the input value does not belong to this
      range, the result is set to NaN.

  Precision: 
  32x32  32-bit inputs, 32-bit output. Accuracy: 1700 (7.9e-7)
  f      floating point. Accuracy 2 ULP

  Input:
  x[N]  input data,Q31 or floating point
  N     length of vectors
  Output:
  y[N]  output data,Q31 or floating point

  Restriction:
  Regular versions (vec_sine32x32, vec_cosine32x32, vec_sinef, vec_cosinef):
  x,y - should not overlap

  Faster versions (vec_sine32x32_fast, vec_cosine32x32_fast):
  x,y - should not overlap
  x,y - aligned on 8-byte boundary
  N   - multiple of 2

  Scalar versions:
  ----------------
  return result in Q31 or floating point
-------------------------------------------------------------------------*/
void vec_sine32x32 (  int32_t * restrict y,
                const int32_t * restrict x,
                int N);
void vec_sine32x32_fast (  int32_t * restrict y,
                const int32_t * restrict x,
                int N);
void vec_cosine32x32 (int32_t * restrict y,
                const int32_t * restrict x,
                int N);
void vec_cosine32x32_fast (int32_t * restrict y,
                const int32_t * restrict x,
                int N);
void vec_sinef     (  float32_t * restrict y,
                const float32_t * restrict x,
                int N);
void vec_cosinef(     float32_t * restrict y,
                const float32_t * restrict x,
                int N);
int32_t scl_sine32x32   (int32_t x);
int32_t scl_cosine32x32 (int32_t x);
float32_t scl_sinef   (float32_t x);
float32_t scl_cosinef (float32_t x);

/*-------------------------------------------------------------------------
  Tangent 
  Fixed point functions calculate tan(pi*x) for number written in Q31. 
  Floating point functions compute tan(x)
  
  Precision: 
  32x32  32-bit inputs, 32-bit outputs. Accuracy: (1.3e-4*y+1LSB)
                                        if abs(y)<=464873(14.19 in Q15) 
                                        or abs(x)<pi*0.4776
  f      floating point.                Accuracy: 2 ULP

  NOTE:
  1.  Scalar floating point function is compatible with standard ANSI C routines 
      and set errno and exception flags accordingly
  2.  Floating point functions limit the range of allowable input values: [-9099, 9099]
      Whenever the input value does not belong to this range, the result is set to NaN.

  Input:
  x[N]   input data,Q31 or floating point
  N      length of vectors
  Output:
  y[N]   result, Q16.15 or floating point

  Restriction:
  x,y - should not overlap

  Scalar versions:
  ----------------
  Return result, Q16.15 or floating point
-------------------------------------------------------------------------*/
void vec_tan32x32 ( 
              int32_t* restrict   y, 
              const int32_t *restrict x,
              int N);
void vec_tanf ( 
                    float32_t *restrict y, 
              const float32_t *restrict x,
              int N);
int32_t scl_tan32x32 (int32_t x);
float32_t scl_tanf (float32_t x);

/*-------------------------------------------------------------------------
  Arctangent 
  Functions calculate arctangent of number. Fixed point functions 
  scale output to pi so it is always in range -0x20000000 : 0x20000000 
  which corresponds to the real phases +pi/4 and represent input and output 
  in Q31
  NOTE:
  1.  Scalar floating point function is compatible with standard ANSI C
      routines and sets errno and exception flags accordingly

  Accuracy:
  32 bit version: 42    (2.0e-8)
  floating point: 2 ULP

  Precision: 
  32x32  32-bit inputs, 32-bit output.
  f      floating point
 
  Input:
  x[N]   input data, Q31 or floating point
  N      length of vectors
  Output:
  z[N]   result, Q31 or floating point

  Restriction:
  x,z should not overlap

  Scalar versions:
  ----------------
  return result, Q31 or floating point
-------------------------------------------------------------------------*/
void vec_atan32x32 (int32_t * restrict z, 
              const int32_t * restrict x, 
              int N );
void vec_atanf (    float32_t * restrict z, 
              const float32_t * restrict x, 
              int N );
int32_t scl_atan32x32 (int32_t x);
float32_t scl_atanf (float32_t x);


/*-------------------------------------------------------------------------
Vectorized Floating-Point Full-Quadrant Arc Tangent
The functions compute the arc tangent of the ratios y[N]/x[N] and store the
result to output vector z[N]. Functions output is in radians.

NOTE:
1. Scalar function is compatible with standard ANSI C routines and set 
   errno and exception flags accordingly
2. Scalar function assigns EDOM to errno whenever y==0 and x==0.

Special cases:
     y    |   x   |  result   |  extra conditions    
  --------|-------|-----------|---------------------
   +/-0   | -0    | +/-pi     |
   +/-0   | +0    | +/-0      |
   +/-0   |  x    | +/-pi     | x<0
   +/-0   |  x    | +/-0      | x>0
   y      | +/-0  | -pi/2     | y<0
   y      | +/-0  |  pi/2     | y>0
   +/-y   | -inf  | +/-pi     | finite y>0
   +/-y   | +inf  | +/-0      | finite y>0
   +/-inf | x     | +/-pi/2   | finite x
   +/-inf | -inf  | +/-3*pi/4 | 
   +/-inf | +inf  | +/-pi/4   |

Input:
  y[N]  vector of numerator values
  x[N]  vector of denominator values
  N     length of vectors
Output:
  z[N]  results
Domain:
  N>0
---------------------------------------------------------------------------*/
void vec_atan2f (float32_t * z, const float32_t * y, const float32_t * x,  int N );
float32_t scl_atan2f (float32_t y, float32_t x);

/*-------------------------------------------------------------------------
  Common Exponent 
  These functions determine the number of redundant sign bits for each value 
  (as if it was loaded in a 32-bit register) and returns the minimum number 
  over the whole vector. This may be useful for a FFT implementation to 
  normalize data.  
  NOTES:
  Faster version of functions make the same task but in a different manner - 
  they compute exponent of maximum absolute value in the array. It allows 
  faster computations but not bitexact results - if minimum value in the 
  array will be -2^n , fast function returns max(0,30-n) while non-fast 
  function returns (31-n).
  Floating point function returns 0-floor(log2(max(abs(x)))). Returned 
  result will be always in range [-129...146]. 
  Special cases
  x       | result
  --------+-------
  0       |    0
  +/-Inf  | -129
  NaN     |    0

  If dimension N<=0 functions return 0

  Precision: 
  32 32-bit inputs 
  16 16-bit inputs 
  f  single precision floating point

  Input:
  x[N] input data
  N    length of vector
  Returned value: minimum exponent

  Restriction:
  Regular versions (vec_bexp16x16, vec_bexp32x32):
  none

  Faster versions (vec_bexp16_fast, vec_bexp32_fast):
  x   - aligned on 8-byte boundary
  N   - a multiple of 4
-------------------------------------------------------------------------*/
int vec_bexp32 (const int32_t * restrict x, int N);
int vec_bexp16 (const int16_t * restrict x, int N);
int vec_bexpf  (const float32_t * restrict x, int N);
int scl_bexp32 (int32_t x);
int scl_bexp16 (int16_t x);
int scl_bexpf  (float32_t x);
int vec_bexp32_fast (const int32_t * restrict x, int N);
int vec_bexp16_fast (const int16_t * restrict x, int N);
/*-------------------------------------------------------------------------
  Vector Min/Max
  These routines find maximum/minimum value in a vector.
  Two versions of functions available: regular version (vec_min32x32, 
  vec_max32x32, vec_min16x16, vec_min16x16) with arbitrary arguments and 
  faster version (vec_min32x32_fast, vec_max32x32_fast, vec_min16x16_fast, 
  vec_min16x16_fast) that apply some restrictions
  NOTE: functions return zero if N is less or equal to zero

  Precision: 
  32x32 32-bit data, 32-bit output
  16x16 16-bit data, 16-bit output
  f     single precision floating point
  
  Input:
  x[N]  input data
  N     length of vector
  Function return minimum or maximum value correspondingly

  Restriction:
  For regular routines:
  none
  For faster routines:
  x aligned on 8-byte boundary
  N   - multiple of 4
-------------------------------------------------------------------------*/
int32_t vec_min32x32 (const int32_t* restrict x, int N);
int16_t vec_min16x16 (const int16_t* restrict x, int N);
float32_t vec_minf     (const float32_t* restrict x, int N);
int32_t vec_max32x32 (const int32_t* restrict x, int N);
int16_t vec_max16x16 (const int16_t* restrict x, int N);
float32_t vec_maxf     (const float32_t* restrict x, int N);
int32_t vec_min32x32_fast (const int32_t* restrict x, int N);
int16_t vec_min16x16_fast (const int16_t* restrict x, int N);
int32_t vec_max32x32_fast (const int32_t* restrict x, int N);
int16_t vec_max16x16_fast (const int16_t* restrict x, int N);

/*-------------------------------------------------------------------------
  integer to float conversion
  routine converts integer to float and scales result up by 2^t.

  Precision: 
  f     single precision floating point

  Output:
  y[N]  output data
  Input:
  x[N]  input data
  t     scale factor
  N     length of vector

  Restriction:
  t should be in range -126...126
-------------------------------------------------------------------------*/
void   vec_int2float ( float32_t  * restrict y, const int32_t  * restrict x, int t, int N);
float32_t scl_int2float (int32_t x, int t);

/*-------------------------------------------------------------------------
  float to integer conversion
  routine scale floating point input down by 2^t and convert it to integer 
  with saturation

  Precision: 
  f     single precision floating point

  Output:
  y[N]  output data
  Input:
  x[N]  input data
  t     scale factor
  N     length of vector

  Restriction:
  t should be in range -126...126
-------------------------------------------------------------------------*/
void   vec_float2int (  int32_t * restrict y, const float32_t * restrict x, int t, int N);
int32_t scl_float2int (float32_t x, int t);

/*-------------------------------------------------------------------------
  Complex magnitude
  routines compute complex magnitude or its reciprocal

  Precision: 
  f     single precision floating point

  Output:
  y[N]  output data
  Input:
  x[N]  input complex data
  N     length of vector

  Restriction:
  none
-------------------------------------------------------------------------*/
void       vec_complex2mag    (float32_t  * restrict y, const complex_float  * restrict x, int N);
void       vec_complex2invmag (float32_t  * restrict y, const complex_float  * restrict x, int N);
float32_t  scl_complex2mag    (complex_float x);
float32_t  scl_complex2invmag (complex_float x);

/*-------------------------------------------------------------------------
  Polynomial approximation
  Functions calculate polynomial approximation for all values from given 
  vector. Fixed point functions take polynomial coefficients in Q31 precision. 
  NOTE:
  approximation is calculated like Taylor series that is why overflow may 
  potentially occur if cumulative sum of coefficients given from the last 
  to the first coefficient is bigger that 1. To avoid this negative effect,
  all the coefficients may be scaled down and result will be shifted left 
  after all intermediate computations.

  Precision: 
  32x32  32-bit inputs, 32-bit coefficients, 32-bit output.
  f      floating point

  Input:
  x[N]    input data, Q31 or floating point
  N       length of vector
  lsh     additional left shift for result
  c[M+1]  coefficients (M=4 coefficients for vec_poly4_xxx 
          and M=8 coefficients for vec_poly8_xxx), Q31 or floating point
  Output:			
  z[N]    result, Q31 or floating point

  Restriction:
  x,c,z should not overlap
  lsh   should be in range 0...31
-------------------------------------------------------------------------*/
void vec_poly4_32x32 (int32_t * restrict z, const int32_t * restrict x, const int32_t * restrict c, int lsh,int N );
void vec_poly8_32x32 (int32_t * restrict z, const int32_t * restrict x, const int32_t * restrict c, int lsh,int N );
void vec_poly4f      (float32_t * restrict z, const float32_t * restrict x, const float32_t * restrict c, int N );
void vec_poly8f      (float32_t * restrict z, const float32_t * restrict x, const float32_t * restrict c, int N );
/*===========================================================================
  Matrix Operations:
  mtx_mpy              Matrix Multiply
  mtx_vecmpy           Matrix by Vector Multiply
  mtx_inv              Matrix inversion
===========================================================================*/
/*-------------------------------------------------------------------------
  These functions compute the expression z = 2^lsh * x * y for the matrices 
  x and y. The columnar dimension of x must match the row dimension of y. 
  The resulting matrix has the same number of rows as x and the same number 
  of columns as y.
  Note:
  In the fixed-point routines, rows of matrices z and y may be stored in 
  non consecutive manner. Matrix x will have all the elements in contiguous 
  memory locations.

  Two versions of functions available: regular version (mtx_mpy32x32, 
  mtx_mpy16x16, mtx_mpyf) with arbitrary arguments and faster version 
  mtx_mpy32x32_fast, mtx_mpy16x16_fast, mtx_mpyf_fast) that apply some 
  restrictions.

  Precision: 
  32x32 32-bit inputs, 32-bit output
  16x16 16-bit inputs, 16-bit output
  f     floating point

  Input:
  x[M*N]      input matrix,Q31, Q15 or floating point
  y[N][P]     input matrix y. For fixed point routines, these are N 
              vectors of size P,Q31 or Q15. For floating point, this 
              is just a matrix of size NxP.
  M           number of rows in matrix x and z
  N           number of columns in matrix x and number of rows in matrix y
  P           number of columns in matrices y and z
  lsh         additional left shift
  Output:
  z[M][P]     output matrix z. For fixed point routines, these are M 
              vectors of size P Q31 or Q15. For floating point, this 
              is single matrix of size MxP
  Scratch:
  pScr        size in bytes defined by macros SCRATCH_MTX_MPY32X32,
              SCRATCH_MTX_MPY16X16

  Restrictions:
  For regular routines (mtx_mpy32x32, mtx_mpy16x16, mtx_mpyf):
  x,y,z should not overlap

  For faster routines (mtx_mpy32x32_fast, mtx_mpy16x16_fast, mtx_mpyf_fast):
  x,y,z should not overlap
  x - aligned on 8-byte boundary
  all rows which addresses are written to y[] - aligned on 8-byte boundary
  N is a multiple of 4,M=8,P=2  
-------------------------------------------------------------------------*/
void mtx_mpy32x32 (  void* pScr,
                     int32_t** restrict z,
               const int32_t*  restrict x,
               const int32_t** restrict y,
               int M, int N, int P, int lsh );
void mtx_mpy16x16 (  void* pScr,
                     int16_t** restrict z,
               const int16_t*  restrict x,
               const int16_t** restrict y,
               int M, int N, int P, int lsh );
void mtx_mpy32x32_fast (  int32_t** restrict z,
               const int32_t*  restrict x,
               const int32_t** restrict y,
               int M, int N, int P, int lsh );
void mtx_mpy16x16_fast (  int16_t** restrict z,
               const int16_t*  restrict x,
               const int16_t** restrict y,
               int M, int N, int P, int lsh );
void mtx_mpyf (      float32_t*  restrict z,
               const float32_t*  restrict x,
               const float32_t*  restrict y,
               int M, int N, int P);
void mtx_mpyf_fast ( float32_t*  restrict z,
               const float32_t*  restrict x,
               const float32_t*  restrict y,
               int M, int N, int P);

#define SCRATCH_MTX_MPY32X32(M,N,P) (((((N)+1)&(~1))+1)*2*sizeof(int32_t))
#define SCRATCH_MTX_MPY16X16(M,N,P) ((((N)+3)&(~3))*4*sizeof(int16_t))

/*-------------------------------------------------------------------------
  Matrix by Vector Multiply
  These functions compute the expression z = 2^lsh * x * y for the matrices 
  x and vector y. 

  Two versions of functions available: regular version (mtx_vecmpy32x32, 
  mtx_vecmpy16x16,mtx_vecmpyf) with arbitrary arguments and faster version 
  (mtx_vecmpy32x32_fast, mtx_vecmpy16x16_fast, mtx_vecmpyf_fast) that 
  apply some restrictions.

  Precision: 
  32x32 32-bit input, 32-bit output
  16x16 16-bit input, 16-bit output
  f     floating point

  Input:
  x[M*N] input matrix,Q31,Q15 or floating point
  y[N]   input vector,Q31,Q15 or floating point
  M      number of rows in matrix x
  N      number of columns in matrix x
  lsh    additional left shift
  Output:
  z[M]   output vector,Q31,Q15 or floating point

  Restriction:
  For regular routines (mtx_vecmpy32x32, mtx_vecmpy16x16, mtx_vecmpyf)
  x,y,z should not overlap

  For faster routines (mtx_vecmpy32x32_fast, mtx_vecmpy16x16_fast,
  mtx_vecmpyf_fast)
  x,y,z should not overlap
  x,y   aligned on 8-byte boundary
  N and M are multiples of 4
-------------------------------------------------------------------------*/
void mtx_vecmpy32x32 (  int32_t* restrict z,
               const int32_t* restrict x,
               const int32_t* restrict y,
               int M, int N, int lsh);
void mtx_vecmpy16x16 (  int16_t* restrict z,
               const int16_t* restrict x,
               const int16_t* restrict y,
               int M, int N, int lsh);
void mtx_vecmpy32x32_fast (  int32_t* restrict z,
               const int32_t* restrict x,
               const int32_t* restrict y,
               int M, int N, int lsh);
void mtx_vecmpy16x16_fast (  int16_t* restrict z,
               const int16_t* restrict x,
               const int16_t* restrict y,
               int M, int N, int lsh);
void mtx_vecmpyf (   float32_t* restrict z,
               const float32_t* restrict x,
               const float32_t* restrict y,
               int M, int N);
void mtx_vecmpyf_fast ( float32_t* restrict z,
               const    float32_t* restrict x,
               const    float32_t* restrict y,
               int M, int N);

/*-------------------------------------------------------------------------
  These functions implement in-place matrix inversion by Gauss elimination 
  with full pivoting

  Precision: 
  f     floating point

  Input:
  x[N*N]      input matrix
  Output:
  x[N*N]      result
  N is 2,3 or 4

  Restrictions:
  none
-------------------------------------------------------------------------*/
void mtx_inv2x2f(float32_t *x);
void mtx_inv3x3f(float32_t *x);
void mtx_inv4x4f(float32_t *x);

/*===========================================================================
  Fast Fourier Transforms:
  fft_cplx             FFT on Complex Data
  fft_real             FFT on Real Data
  ifft_cplx            IFFT on Complex Data
  ifft_real            Inverse FFT Forming Real Data
  dct                  Discrete Cosine Transform

  All FFT routines use dynamic scaling and return number of right shifts 
  occurred during computations

===========================================================================*/

typedef const void * fft_handle_t;

/*-------------------------------------------------------------------------
  Complex-valued forward FFT of size 2^n, n=4..12.

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
  x[2*N]     complex input signal. Real and imaginary data are interleaved 
             and real data goes first
  bexp       common block exponent, that is the minimum number of redundant
             sign bits over input data x[2*N]
  h          FFT descriptor handle, identifies the transform size N and 
             constant data tables
  Output:
  y[2*N]     output spectrum. Real and imaginary data are interleaved and 
             real data goes first

  Returned value: total right shift amount applied to dynamically scale the data

  Restrictions:
  x[],y[] - must not overlap
  x[],y[] - must be aligned on 8-byte boundary

  PERFORMANCE NOTE:
  none
-------------------------------------------------------------------------*/
// FFT handles for 32x16 and 16x16
extern const fft_handle_t h_fft_cplx_x16_16;
extern const fft_handle_t h_fft_cplx_x16_32;
extern const fft_handle_t h_fft_cplx_x16_64;
extern const fft_handle_t h_fft_cplx_x16_128;
extern const fft_handle_t h_fft_cplx_x16_256;
extern const fft_handle_t h_fft_cplx_x16_512;
extern const fft_handle_t h_fft_cplx_x16_1024;
extern const fft_handle_t h_fft_cplx_x16_2048;
extern const fft_handle_t h_fft_cplx_x16_4096;
// FFT handles for 32x32
extern const fft_handle_t h_fft_cplx_x32_16;
extern const fft_handle_t h_fft_cplx_x32_32;
extern const fft_handle_t h_fft_cplx_x32_64;
extern const fft_handle_t h_fft_cplx_x32_128;
extern const fft_handle_t h_fft_cplx_x32_256;
extern const fft_handle_t h_fft_cplx_x32_512;
extern const fft_handle_t h_fft_cplx_x32_1024;
extern const fft_handle_t h_fft_cplx_x32_2048;
extern const fft_handle_t h_fft_cplx_x32_4096;

int fft_cplx32x32( int32_t * restrict y, int32_t * restrict x, int bexp, fft_handle_t h );
int fft_cplx32x16( int32_t * restrict y, int32_t * restrict x, int bexp, fft_handle_t h );
int fft_cplx16x16( int16_t * restrict y, int16_t * restrict x, int bexp, fft_handle_t h );

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
// FFT handles for 32x16 and 16x16
extern const fft_handle_t h_fft_real_x16_32;
extern const fft_handle_t h_fft_real_x16_64;
extern const fft_handle_t h_fft_real_x16_128;
extern const fft_handle_t h_fft_real_x16_256;
extern const fft_handle_t h_fft_real_x16_512;
extern const fft_handle_t h_fft_real_x16_1024;
extern const fft_handle_t h_fft_real_x16_2048;
extern const fft_handle_t h_fft_real_x16_4096;
extern const fft_handle_t h_fft_real_x16_8192;
// FFT handles for 32x32
extern const fft_handle_t h_fft_real_x32_32;
extern const fft_handle_t h_fft_real_x32_64;
extern const fft_handle_t h_fft_real_x32_128;
extern const fft_handle_t h_fft_real_x32_256;
extern const fft_handle_t h_fft_real_x32_512;
extern const fft_handle_t h_fft_real_x32_1024;
extern const fft_handle_t h_fft_real_x32_2048;
extern const fft_handle_t h_fft_real_x32_4096;
extern const fft_handle_t h_fft_real_x32_8192;

int fft_real32x32( int32_t * restrict y, int32_t * restrict x, int bexp, fft_handle_t h );
int fft_real32x16( int32_t * restrict y, int32_t * restrict x, int bexp, fft_handle_t h );
int fft_real16x16( int16_t * restrict y, int16_t * restrict x, int bexp, fft_handle_t h );

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
// FFT handles for 32x16 and 16x16
extern const fft_handle_t h_ifft_cplx_x16_16;
extern const fft_handle_t h_ifft_cplx_x16_32;
extern const fft_handle_t h_ifft_cplx_x16_64;
extern const fft_handle_t h_ifft_cplx_x16_128;
extern const fft_handle_t h_ifft_cplx_x16_256;
extern const fft_handle_t h_ifft_cplx_x16_512;
extern const fft_handle_t h_ifft_cplx_x16_1024;
extern const fft_handle_t h_ifft_cplx_x16_2048;
extern const fft_handle_t h_ifft_cplx_x16_4096;
// FFT handles for 32x32
extern const fft_handle_t h_ifft_cplx_x32_16;
extern const fft_handle_t h_ifft_cplx_x32_32;
extern const fft_handle_t h_ifft_cplx_x32_64;
extern const fft_handle_t h_ifft_cplx_x32_128;
extern const fft_handle_t h_ifft_cplx_x32_256;
extern const fft_handle_t h_ifft_cplx_x32_512;
extern const fft_handle_t h_ifft_cplx_x32_1024;
extern const fft_handle_t h_ifft_cplx_x32_2048;
extern const fft_handle_t h_ifft_cplx_x32_4096;

int ifft_cplx32x32( int32_t * restrict y, int32_t * restrict x, int bexp, fft_handle_t h );
int ifft_cplx32x16( int32_t * restrict y, int32_t * restrict x, int bexp, fft_handle_t h );
int ifft_cplx16x16( int16_t * restrict y, int16_t * restrict x, int bexp, fft_handle_t h );

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
// FFT handles for 32x16 and 16x16
extern const fft_handle_t h_ifft_real_x16_32;
extern const fft_handle_t h_ifft_real_x16_64;
extern const fft_handle_t h_ifft_real_x16_128;
extern const fft_handle_t h_ifft_real_x16_256;
extern const fft_handle_t h_ifft_real_x16_512;
extern const fft_handle_t h_ifft_real_x16_1024;
extern const fft_handle_t h_ifft_real_x16_2048;
extern const fft_handle_t h_ifft_real_x16_4096;
extern const fft_handle_t h_ifft_real_x16_8192;
// FFT handles for 32x32
extern const fft_handle_t h_ifft_real_x32_32;
extern const fft_handle_t h_ifft_real_x32_64;
extern const fft_handle_t h_ifft_real_x32_128;
extern const fft_handle_t h_ifft_real_x32_256;
extern const fft_handle_t h_ifft_real_x32_512;
extern const fft_handle_t h_ifft_real_x32_1024;
extern const fft_handle_t h_ifft_real_x32_2048;
extern const fft_handle_t h_ifft_real_x32_4096;
extern const fft_handle_t h_ifft_real_x32_8192;

int ifft_real32x32( int32_t * restrict y, int32_t * restrict x, int bexp, fft_handle_t h );
int ifft_real32x16( int32_t * restrict y, int32_t * restrict x, int bexp, fft_handle_t h );
int ifft_real16x16( int16_t * restrict y, int16_t * restrict x, int bexp, fft_handle_t h );

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
int fft_cplxf_ie    (complex_float  * y, complex_float * x, const complex_float  * twd, int twdstep, int N );

/*-------------------------------------------------------------------------
  These functions make FFT on real data with optimized memory usage.
    
  NOTES:
  1. Bit-reversing reordering is done here.
  2. INPUT DATA MAY APPEAR DAMAGED after the call
  3. Forward/inverse FFT of size N may be supplied with constant data
  (twiddle factors) of a larger-sized FFT = N*twdstep.

  Precision:
  f             floating point

  Input:
  x - real input signal. Real and imaginary data are interleaved
  and real data goes first:

  twd[N*twdstep*3/4]    twiddle factor table of a complex-valued 
                        FFT of size N*twdstep
  N                     FFT size
  twdstep               twiddle step

  Output:
  y - output spectrum. Real and imaginary data are interleaved and
  real data goes first
  --------------+----------+-----------------+----------------
  Function      |   Size   |  Allocated Size |  type         |
  --------------+----------+-----------------+----------------
  f_ie          |   N/2+1  |      N/2+1      |complex_float  |
  --------------+----------+-----------------+----------------

  Returned value: total number of right shifts occurred during scaling
  procedure

  Restrictions:
  x,y - should not overlap
  x,y - aligned on 8-bytes boundary
  N   - power of two and >=8 for floating point
-------------------------------------------------------------------------*/
int fft_realf_ie        (complex_float  * y,float32_t * x, const complex_float  * twd, int twdstep, int N);

/*-------------------------------------------------------------------------
  These functions make inverse FFT on complex data with optimized memory usage.
  Scaling  : Fixed data scaling at each stage
  NOTES:
  1. Bit-reversing reordering is done here.
  2. FFT runs in-place algorithm so INPUT DATA WILL APPEAR DAMAGED after 
     the call
  3. Forward/inverse FFT of size N may be supplied with constant data
     (twiddle factors) of a larger-sized FFT = N*twdstep.

  Precision: 
  f_ie                  floating point
 
  Input:
  x[N]                complex input signal. Real and imaginary data are interleaved 
                      and real data goes first

  twd[N*twdstep*3/4]  twiddle factor table of a complex-valued FFT of size N*twdstep
  N                   FFT size
  twdstep             twiddle step 

  Output:
  y[N]                output spectrum. Real and imaginary data are interleaved and 
                      real data goes first

  Returned value:     total number of right shifts occurred during scaling 
                      procedure

  Restrictions:
  x,y - should not overlap
  x,y - aligned on 8-bytes boundary
  N   - power of 2 and >=8 for floating point
-------------------------------------------------------------------------*/
int ifft_cplxf_ie    (complex_float  * y,complex_float  * x, const complex_float  * twd, int twdstep, int N);

/*-------------------------------------------------------------------------
  These functions make inverse FFT on real data with optimized memory usage.
  NOTES:
  1. Bit-reversing reordering is done here.
  2. INPUT DATA MAY APPEAR DAMAGED after the call
  3. Forward/inverse FFT of size N may be supplied with constant data
  (twiddle factors) of a larger-sized FFT = N*twdstep.

  Precision:
  f_ie          floating point

  Input:
  x - complex input signal. Real and imaginary data are interleaved
  and real data goes first:
  --------------+----------+-----------------+----------------
  Function      |   Size   |  Allocated Size |       type    |
  --------------+----------+-----------------+----------------
  f_ie          |   N/2+1  |      N/2+1      | complex_float |
  --------------+----------+-----------------+----------------

  twd[2*N*twdstep*3/4]  twiddle factor table of a complex-valued FFT of size N*twdstep
  N                     FFT size
  twdstep               twiddle step
  Output:
  y - output spectrum. Real and imaginary data are interleaved and
  real data goes first:
  --------------+----------+-----------------+-----------
  Function      |   Size   |  Allocated Size |  type    |
  --------------+----------+-----------------+-----------
  f_ie          |      N   |      N          | float32_t|
  --------------+----------+-----------------+-----------

  Returned value: total number of right shifts occurred during scaling
  procedure

  Restrictions:
  x,y - should not overlap
  x,y - aligned on 8-bytes boundary
  N   - power of two and >=8 for floating point
-------------------------------------------------------------------------*/
int ifft_realf_ie        (float32_t* y, complex_float * x, const  complex_float * twd, int twdstep, int N);

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
int dct_32x32( int32_t* y,int32_t* x,int N);
int dct_32x16( int32_t* y,int32_t* x,int N);
int dctf     ( float32_t * y,float32_t * x,int N               );

/*===========================================================================
  Identification Routines
  NatureDSP_Signal_get_library_version     Library Version Request
  NatureDSP_Signal_get_library_api_version Library API Version Request
===========================================================================*/

/*-------------------------------------------------------------------------
   Returns library version string
   Parameters:
    Input:
    none
    Output:
     version_string   Pre-allocated buffer for version string.
   Restrictions:
     version_string must points to a buffer large enough to hold up to
     30 characters.
-------------------------------------------------------------------------*/
void NatureDSP_Signal_get_library_version(char *version_string);

/*-------------------------------------------------------------------------
   Returns library API version string
   Parameters:
    Input:
    none
    Output:
     version_string   Pre-allocated buffer for API version string.
   Restrictions:
     version_string must points to a buffer large enough to hold up to
     30 characters.
-------------------------------------------------------------------------*/
void NatureDSP_Signal_get_library_api_version(char *version_string);

/*-------------------------------------------------------------------------
Returns non-zero if given function (by its address) is supported by
specific processor capabilities
Parameters:
    Input:
     fun    one of function from the list above

NOTE:
in gcc/xcc environment, calls of this function is not neccessary - if 
function pointer is non-zero it means it is supported. VisualStudio linker 
does not support section removal so this function might be used for 
running library under MSVC environment
-------------------------------------------------------------------------*/
typedef void(*NatureDSP_Signal_funptr)();
int __NatureDSP_Signal_isPresent(NatureDSP_Signal_funptr fun);
#define NatureDSP_Signal_isPresent(fun) __NatureDSP_Signal_isPresent((NatureDSP_Signal_funptr)fun)

#ifdef __cplusplus
}
#endif

#endif//__NATUREDSP_SIGNAL_H__
