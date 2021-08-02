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
    16-bit twiddle factor table for 512-point real-valued fwd/inv FFT
  IntegrIT, 2006-2014
*/

/* Datatypes and helper definitions. */
#include "common.h"
/* Descriptor structures for real-valued FFTs. */
#include "fft_real_twd.h"
/* Twiddle factor tables for complex-valued FFTs. */
#include "fft_cplx_twd.h"

// N = 512;
// spc = -1j*exp(-2j*pi*(0:N/4-1)/N);
static const int16_t fft_real_x16_spc[512*1/4*2] =
{
  (int16_t)0x0000,(int16_t)0x8000,(int16_t)0xfe6e,(int16_t)0x8002,
  (int16_t)0xfcdc,(int16_t)0x800a,(int16_t)0xfb4a,(int16_t)0x8016,
  (int16_t)0xf9b8,(int16_t)0x8027,(int16_t)0xf827,(int16_t)0x803e,
  (int16_t)0xf695,(int16_t)0x8059,(int16_t)0xf505,(int16_t)0x8079,
  (int16_t)0xf374,(int16_t)0x809e,(int16_t)0xf1e4,(int16_t)0x80c8,
  (int16_t)0xf055,(int16_t)0x80f6,(int16_t)0xeec6,(int16_t)0x812a,
  (int16_t)0xed38,(int16_t)0x8163,(int16_t)0xebab,(int16_t)0x81a0,
  (int16_t)0xea1e,(int16_t)0x81e2,(int16_t)0xe892,(int16_t)0x822a,
  (int16_t)0xe707,(int16_t)0x8276,(int16_t)0xe57d,(int16_t)0x82c6,
  (int16_t)0xe3f4,(int16_t)0x831c,(int16_t)0xe26d,(int16_t)0x8377,
  (int16_t)0xe0e6,(int16_t)0x83d6,(int16_t)0xdf61,(int16_t)0x843a,
  (int16_t)0xdddc,(int16_t)0x84a3,(int16_t)0xdc59,(int16_t)0x8511,
  (int16_t)0xdad8,(int16_t)0x8583,(int16_t)0xd958,(int16_t)0x85fa,
  (int16_t)0xd7d9,(int16_t)0x8676,(int16_t)0xd65c,(int16_t)0x86f6,
  (int16_t)0xd4e1,(int16_t)0x877b,(int16_t)0xd367,(int16_t)0x8805,
  (int16_t)0xd1ef,(int16_t)0x8894,(int16_t)0xd079,(int16_t)0x8927,
  (int16_t)0xcf04,(int16_t)0x89be,(int16_t)0xcd92,(int16_t)0x8a5a,
  (int16_t)0xcc21,(int16_t)0x8afb,(int16_t)0xcab2,(int16_t)0x8ba0,
  (int16_t)0xc946,(int16_t)0x8c4a,(int16_t)0xc7db,(int16_t)0x8cf8,
  (int16_t)0xc673,(int16_t)0x8dab,(int16_t)0xc50d,(int16_t)0x8e62,
  (int16_t)0xc3a9,(int16_t)0x8f1d,(int16_t)0xc248,(int16_t)0x8fdd,
  (int16_t)0xc0e9,(int16_t)0x90a1,(int16_t)0xbf8c,(int16_t)0x9169,
  (int16_t)0xbe32,(int16_t)0x9236,(int16_t)0xbcda,(int16_t)0x9307,
  (int16_t)0xbb85,(int16_t)0x93dc,(int16_t)0xba33,(int16_t)0x94b5,
  (int16_t)0xb8e3,(int16_t)0x9592,(int16_t)0xb796,(int16_t)0x9674,
  (int16_t)0xb64c,(int16_t)0x9759,(int16_t)0xb505,(int16_t)0x9843,
  (int16_t)0xb3c0,(int16_t)0x9930,(int16_t)0xb27f,(int16_t)0x9a22,
  (int16_t)0xb140,(int16_t)0x9b17,(int16_t)0xb005,(int16_t)0x9c11,
  (int16_t)0xaecc,(int16_t)0x9d0e,(int16_t)0xad97,(int16_t)0x9e0f,
  (int16_t)0xac65,(int16_t)0x9f14,(int16_t)0xab36,(int16_t)0xa01c,
  (int16_t)0xaa0a,(int16_t)0xa129,(int16_t)0xa8e2,(int16_t)0xa238,
  (int16_t)0xa7bd,(int16_t)0xa34c,(int16_t)0xa69c,(int16_t)0xa463,
  (int16_t)0xa57e,(int16_t)0xa57e,(int16_t)0xa463,(int16_t)0xa69c,
  (int16_t)0xa34c,(int16_t)0xa7bd,(int16_t)0xa238,(int16_t)0xa8e2,
  (int16_t)0xa129,(int16_t)0xaa0a,(int16_t)0xa01c,(int16_t)0xab36,
  (int16_t)0x9f14,(int16_t)0xac65,(int16_t)0x9e0f,(int16_t)0xad97,
  (int16_t)0x9d0e,(int16_t)0xaecc,(int16_t)0x9c11,(int16_t)0xb005,
  (int16_t)0x9b17,(int16_t)0xb140,(int16_t)0x9a22,(int16_t)0xb27f,
  (int16_t)0x9930,(int16_t)0xb3c0,(int16_t)0x9843,(int16_t)0xb505,
  (int16_t)0x9759,(int16_t)0xb64c,(int16_t)0x9674,(int16_t)0xb796,
  (int16_t)0x9592,(int16_t)0xb8e3,(int16_t)0x94b5,(int16_t)0xba33,
  (int16_t)0x93dc,(int16_t)0xbb85,(int16_t)0x9307,(int16_t)0xbcda,
  (int16_t)0x9236,(int16_t)0xbe32,(int16_t)0x9169,(int16_t)0xbf8c,
  (int16_t)0x90a1,(int16_t)0xc0e9,(int16_t)0x8fdd,(int16_t)0xc248,
  (int16_t)0x8f1d,(int16_t)0xc3a9,(int16_t)0x8e62,(int16_t)0xc50d,
  (int16_t)0x8dab,(int16_t)0xc673,(int16_t)0x8cf8,(int16_t)0xc7db,
  (int16_t)0x8c4a,(int16_t)0xc946,(int16_t)0x8ba0,(int16_t)0xcab2,
  (int16_t)0x8afb,(int16_t)0xcc21,(int16_t)0x8a5a,(int16_t)0xcd92,
  (int16_t)0x89be,(int16_t)0xcf04,(int16_t)0x8927,(int16_t)0xd079,
  (int16_t)0x8894,(int16_t)0xd1ef,(int16_t)0x8805,(int16_t)0xd367,
  (int16_t)0x877b,(int16_t)0xd4e1,(int16_t)0x86f6,(int16_t)0xd65c,
  (int16_t)0x8676,(int16_t)0xd7d9,(int16_t)0x85fa,(int16_t)0xd958,
  (int16_t)0x8583,(int16_t)0xdad8,(int16_t)0x8511,(int16_t)0xdc59,
  (int16_t)0x84a3,(int16_t)0xdddc,(int16_t)0x843a,(int16_t)0xdf61,
  (int16_t)0x83d6,(int16_t)0xe0e6,(int16_t)0x8377,(int16_t)0xe26d,
  (int16_t)0x831c,(int16_t)0xe3f4,(int16_t)0x82c6,(int16_t)0xe57d,
  (int16_t)0x8276,(int16_t)0xe707,(int16_t)0x822a,(int16_t)0xe892,
  (int16_t)0x81e2,(int16_t)0xea1e,(int16_t)0x81a0,(int16_t)0xebab,
  (int16_t)0x8163,(int16_t)0xed38,(int16_t)0x812a,(int16_t)0xeec6,
  (int16_t)0x80f6,(int16_t)0xf055,(int16_t)0x80c8,(int16_t)0xf1e4,
  (int16_t)0x809e,(int16_t)0xf374,(int16_t)0x8079,(int16_t)0xf505,
  (int16_t)0x8059,(int16_t)0xf695,(int16_t)0x803e,(int16_t)0xf827,
  (int16_t)0x8027,(int16_t)0xf9b8,(int16_t)0x8016,(int16_t)0xfb4a,
  (int16_t)0x800a,(int16_t)0xfcdc,(int16_t)0x8002,(int16_t)0xfe6e
};

static const fft_real_x16_descr_t fft_real_x16_descr =
{ 
  512, fft_cplx_x16_twd_256, fft_real_x16_spc
};

const fft_handle_t h_fft_real_x16_512  = (fft_handle_t)&fft_real_x16_descr;
const fft_handle_t h_ifft_real_x16_512 = (fft_handle_t)&fft_real_x16_descr;
