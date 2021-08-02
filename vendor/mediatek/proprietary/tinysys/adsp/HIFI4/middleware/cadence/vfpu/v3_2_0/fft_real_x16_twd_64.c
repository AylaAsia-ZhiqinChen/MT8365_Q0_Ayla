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
    16-bit twiddle factor table for 64-point real-valued fwd/inv FFT
  IntegrIT, 2006-2014
*/

/* Datatypes and helper definitions. */
#include "common.h"
/* Descriptor structures for real-valued FFTs. */
#include "fft_real_twd.h"
/* Twiddle factor tables for complex-valued FFTs. */
#include "fft_cplx_twd.h"

// N = 64;
// spc = -1j*exp(-2j*pi*(0:N/4-1)/N);
static const int16_t fft_real_x16_spc[64*1/4*2] =
{
  (int16_t)0x0000,(int16_t)0x8000,(int16_t)0xf374,(int16_t)0x809e,
  (int16_t)0xe707,(int16_t)0x8276,(int16_t)0xdad8,(int16_t)0x8583,
  (int16_t)0xcf04,(int16_t)0x89be,(int16_t)0xc3a9,(int16_t)0x8f1d,
  (int16_t)0xb8e3,(int16_t)0x9592,(int16_t)0xaecc,(int16_t)0x9d0e,
  (int16_t)0xa57e,(int16_t)0xa57e,(int16_t)0x9d0e,(int16_t)0xaecc,
  (int16_t)0x9592,(int16_t)0xb8e3,(int16_t)0x8f1d,(int16_t)0xc3a9,
  (int16_t)0x89be,(int16_t)0xcf04,(int16_t)0x8583,(int16_t)0xdad8,
  (int16_t)0x8276,(int16_t)0xe707,(int16_t)0x809e,(int16_t)0xf374
};

static const fft_real_x16_descr_t fft_real_x16_descr =
{ 
  64, fft_cplx_x16_twd_32, fft_real_x16_spc
};

const fft_handle_t h_fft_real_x16_64  = (fft_handle_t)&fft_real_x16_descr;
const fft_handle_t h_ifft_real_x16_64 = (fft_handle_t)&fft_real_x16_descr;
