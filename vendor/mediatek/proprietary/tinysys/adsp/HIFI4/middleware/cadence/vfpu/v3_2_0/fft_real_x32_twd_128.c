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
    32-bit twiddle factor table for 128-point real-valued fwd/inv FFT
  IntegrIT, 2006-2014
*/

/* Datatypes and helper definitions. */
#include "common.h"
/* Descriptor structures for real-valued FFTs. */
#include "fft_real_twd.h"
/* Twiddle factor tables for complex-valued FFTs. */
#include "fft_cplx_twd.h"

// N = 128;
// spc = -1j*exp(-2j*pi*(0:N/4-1)/N);
static const int32_t fft_real_x32_spc[128*1/4*2] =
{
  (int32_t)0x00000000,(int32_t)0x80000000,(int32_t)0xf9b82684,(int32_t)0x80277872,
  (int32_t)0xf3742ca2,(int32_t)0x809dc971,(int32_t)0xed37ef91,(int32_t)0x8162aa04,
  (int32_t)0xe70747c4,(int32_t)0x8275a0c0,(int32_t)0xe0e60685,(int32_t)0x83d60412,
  (int32_t)0xdad7f3a2,(int32_t)0x8582faa5,(int32_t)0xd4e0cb15,(int32_t)0x877b7bec,
  (int32_t)0xcf043ab3,(int32_t)0x89be50c3,(int32_t)0xc945dfec,(int32_t)0x8c4a142f,
  (int32_t)0xc3a94590,(int32_t)0x8f1d343a,(int32_t)0xbe31e19b,(int32_t)0x9235f2ec,
  (int32_t)0xb8e31319,(int32_t)0x9592675c,(int32_t)0xb3c0200c,(int32_t)0x99307ee0,
  (int32_t)0xaecc336c,(int32_t)0x9d0dfe54,(int32_t)0xaa0a5b2e,(int32_t)0xa1288376,
  (int32_t)0xa57d8666,(int32_t)0xa57d8666,(int32_t)0xa1288376,(int32_t)0xaa0a5b2e,
  (int32_t)0x9d0dfe54,(int32_t)0xaecc336c,(int32_t)0x99307ee0,(int32_t)0xb3c0200c,
  (int32_t)0x9592675c,(int32_t)0xb8e31319,(int32_t)0x9235f2ec,(int32_t)0xbe31e19b,
  (int32_t)0x8f1d343a,(int32_t)0xc3a94590,(int32_t)0x8c4a142f,(int32_t)0xc945dfec,
  (int32_t)0x89be50c3,(int32_t)0xcf043ab3,(int32_t)0x877b7bec,(int32_t)0xd4e0cb15,
  (int32_t)0x8582faa5,(int32_t)0xdad7f3a2,(int32_t)0x83d60412,(int32_t)0xe0e60685,
  (int32_t)0x8275a0c0,(int32_t)0xe70747c4,(int32_t)0x8162aa04,(int32_t)0xed37ef91,
  (int32_t)0x809dc971,(int32_t)0xf3742ca2,(int32_t)0x80277872,(int32_t)0xf9b82684
};

static const fft_real_x32_descr_t fft_real_x32_descr =
{ 
  128, fft_cplx_x32_twd_64, fft_real_x32_spc
};

const fft_handle_t h_fft_real_x32_128  = (fft_handle_t)&fft_real_x32_descr;
const fft_handle_t h_ifft_real_x32_128 = (fft_handle_t)&fft_real_x32_descr;
