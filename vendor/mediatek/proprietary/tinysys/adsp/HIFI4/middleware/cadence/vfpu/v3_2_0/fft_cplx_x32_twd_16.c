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
    32-bit twiddle factor table for 16-point complex-valued fwd/inv FFT
  IntegrIT, 2006-2014
*/

/* Datatypes and helper definitions. */
#include "common.h"
/* Twiddle factor tables for complex-valued FFTs */
#include "fft_cplx_twd.h"

static const fft_cplx_x32_descr_t fft_cplx_x32_16_descr = { 16, fft_cplx_x32_twd_16 };

const fft_handle_t h_fft_cplx_x32_16  = (fft_handle_t)&fft_cplx_x32_16_descr;
const fft_handle_t h_ifft_cplx_x32_16 = (fft_handle_t)&fft_cplx_x32_16_descr;

// N = 16;
// tw = reshape(exp(-2j*pi*[1;2;3]*(0:N/4-1)/N),1,3*N/4);
const int32_t ALIGN(8) fft_cplx_x32_twd_16[16*3/4*2] =
{
  (int32_t)0x7fffffff,(int32_t)0x00000000,(int32_t)0x7fffffff,(int32_t)0x00000000,
  (int32_t)0x7fffffff,(int32_t)0x00000000,(int32_t)0x7641af3d,(int32_t)0xcf043ab3,
  (int32_t)0x5a82799a,(int32_t)0xa57d8666,(int32_t)0x30fbc54d,(int32_t)0x89be50c3,
  (int32_t)0x5a82799a,(int32_t)0xa57d8666,(int32_t)0x00000000,(int32_t)0x80000000,
  (int32_t)0xa57d8666,(int32_t)0xa57d8666,(int32_t)0x30fbc54d,(int32_t)0x89be50c3,
  (int32_t)0xa57d8666,(int32_t)0xa57d8666,(int32_t)0x89be50c3,(int32_t)0x30fbc54d
};
