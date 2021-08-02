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
    16-bit twiddle factor table for 256-point complex-valued fwd/inv FFT
  IntegrIT, 2006-2014
*/

/* Datatypes and helper definitions. */
#include "common.h"
/* Twiddle factor tables for complex-valued FFTs */
#include "fft_cplx_twd.h"

static const fft_cplx_x16_descr_t fft_cplx_x16_256_descr = { 256, fft_cplx_x16_twd_256 };

const fft_handle_t h_fft_cplx_x16_256  = (fft_handle_t)&fft_cplx_x16_256_descr;
const fft_handle_t h_ifft_cplx_x16_256 = (fft_handle_t)&fft_cplx_x16_256_descr;

// N = 256;
// tw = reshape(exp(-2j*pi*[1;2;3]*(0:N/4-1)/N),1,3*N/4);
const int16_t ALIGN(8) fft_cplx_x16_twd_256[256*3/4*2] =
{
  (int16_t)0x7fff,(int16_t)0x0000,(int16_t)0x7fff,(int16_t)0x0000,
  (int16_t)0x7fff,(int16_t)0x0000,(int16_t)0x7ff6,(int16_t)0xfcdc,
  (int16_t)0x7fd9,(int16_t)0xf9b8,(int16_t)0x7fa7,(int16_t)0xf695,
  (int16_t)0x7fd9,(int16_t)0xf9b8,(int16_t)0x7f62,(int16_t)0xf374,
  (int16_t)0x7e9d,(int16_t)0xed38,(int16_t)0x7fa7,(int16_t)0xf695,
  (int16_t)0x7e9d,(int16_t)0xed38,(int16_t)0x7ce4,(int16_t)0xe3f4,
  (int16_t)0x7f62,(int16_t)0xf374,(int16_t)0x7d8a,(int16_t)0xe707,
  (int16_t)0x7a7d,(int16_t)0xdad8,(int16_t)0x7f0a,(int16_t)0xf055,
  (int16_t)0x7c2a,(int16_t)0xe0e6,(int16_t)0x776c,(int16_t)0xd1ef,
  (int16_t)0x7e9d,(int16_t)0xed38,(int16_t)0x7a7d,(int16_t)0xdad8,
  (int16_t)0x73b6,(int16_t)0xc946,(int16_t)0x7e1e,(int16_t)0xea1e,
  (int16_t)0x7885,(int16_t)0xd4e1,(int16_t)0x6f5f,(int16_t)0xc0e9,
  (int16_t)0x7d8a,(int16_t)0xe707,(int16_t)0x7642,(int16_t)0xcf04,
  (int16_t)0x6a6e,(int16_t)0xb8e3,(int16_t)0x7ce4,(int16_t)0xe3f4,
  (int16_t)0x73b6,(int16_t)0xc946,(int16_t)0x64e9,(int16_t)0xb140,
  (int16_t)0x7c2a,(int16_t)0xe0e6,(int16_t)0x70e3,(int16_t)0xc3a9,
  (int16_t)0x5ed7,(int16_t)0xaa0a,(int16_t)0x7b5d,(int16_t)0xdddc,
  (int16_t)0x6dca,(int16_t)0xbe32,(int16_t)0x5843,(int16_t)0xa34c,
  (int16_t)0x7a7d,(int16_t)0xdad8,(int16_t)0x6a6e,(int16_t)0xb8e3,
  (int16_t)0x5134,(int16_t)0x9d0e,(int16_t)0x798a,(int16_t)0xd7d9,
  (int16_t)0x66d0,(int16_t)0xb3c0,(int16_t)0x49b4,(int16_t)0x9759,
  (int16_t)0x7885,(int16_t)0xd4e1,(int16_t)0x62f2,(int16_t)0xaecc,
  (int16_t)0x41ce,(int16_t)0x9236,(int16_t)0x776c,(int16_t)0xd1ef,
  (int16_t)0x5ed7,(int16_t)0xaa0a,(int16_t)0x398d,(int16_t)0x8dab,
  (int16_t)0x7642,(int16_t)0xcf04,(int16_t)0x5a82,(int16_t)0xa57e,
  (int16_t)0x30fc,(int16_t)0x89be,(int16_t)0x7505,(int16_t)0xcc21,
  (int16_t)0x55f6,(int16_t)0xa129,(int16_t)0x2827,(int16_t)0x8676,
  (int16_t)0x73b6,(int16_t)0xc946,(int16_t)0x5134,(int16_t)0x9d0e,
  (int16_t)0x1f1a,(int16_t)0x83d6,(int16_t)0x7255,(int16_t)0xc673,
  (int16_t)0x4c40,(int16_t)0x9930,(int16_t)0x15e2,(int16_t)0x81e2,
  (int16_t)0x70e3,(int16_t)0xc3a9,(int16_t)0x471d,(int16_t)0x9592,
  (int16_t)0x0c8c,(int16_t)0x809e,(int16_t)0x6f5f,(int16_t)0xc0e9,
  (int16_t)0x41ce,(int16_t)0x9236,(int16_t)0x0324,(int16_t)0x800a,
  (int16_t)0x6dca,(int16_t)0xbe32,(int16_t)0x3c57,(int16_t)0x8f1d,
  (int16_t)0xf9b8,(int16_t)0x8027,(int16_t)0x6c24,(int16_t)0xbb85,
  (int16_t)0x36ba,(int16_t)0x8c4a,(int16_t)0xf055,(int16_t)0x80f6,
  (int16_t)0x6a6e,(int16_t)0xb8e3,(int16_t)0x30fc,(int16_t)0x89be,
  (int16_t)0xe707,(int16_t)0x8276,(int16_t)0x68a7,(int16_t)0xb64c,
  (int16_t)0x2b1f,(int16_t)0x877b,(int16_t)0xdddc,(int16_t)0x84a3,
  (int16_t)0x66d0,(int16_t)0xb3c0,(int16_t)0x2528,(int16_t)0x8583,
  (int16_t)0xd4e1,(int16_t)0x877b,(int16_t)0x64e9,(int16_t)0xb140,
  (int16_t)0x1f1a,(int16_t)0x83d6,(int16_t)0xcc21,(int16_t)0x8afb,
  (int16_t)0x62f2,(int16_t)0xaecc,(int16_t)0x18f9,(int16_t)0x8276,
  (int16_t)0xc3a9,(int16_t)0x8f1d,(int16_t)0x60ec,(int16_t)0xac65,
  (int16_t)0x12c8,(int16_t)0x8163,(int16_t)0xbb85,(int16_t)0x93dc,
  (int16_t)0x5ed7,(int16_t)0xaa0a,(int16_t)0x0c8c,(int16_t)0x809e,
  (int16_t)0xb3c0,(int16_t)0x9930,(int16_t)0x5cb4,(int16_t)0xa7bd,
  (int16_t)0x0648,(int16_t)0x8027,(int16_t)0xac65,(int16_t)0x9f14,
  (int16_t)0x5a82,(int16_t)0xa57e,(int16_t)0x0000,(int16_t)0x8000,
  (int16_t)0xa57e,(int16_t)0xa57e,(int16_t)0x5843,(int16_t)0xa34c,
  (int16_t)0xf9b8,(int16_t)0x8027,(int16_t)0x9f14,(int16_t)0xac65,
  (int16_t)0x55f6,(int16_t)0xa129,(int16_t)0xf374,(int16_t)0x809e,
  (int16_t)0x9930,(int16_t)0xb3c0,(int16_t)0x539b,(int16_t)0x9f14,
  (int16_t)0xed38,(int16_t)0x8163,(int16_t)0x93dc,(int16_t)0xbb85,
  (int16_t)0x5134,(int16_t)0x9d0e,(int16_t)0xe707,(int16_t)0x8276,
  (int16_t)0x8f1d,(int16_t)0xc3a9,(int16_t)0x4ec0,(int16_t)0x9b17,
  (int16_t)0xe0e6,(int16_t)0x83d6,(int16_t)0x8afb,(int16_t)0xcc21,
  (int16_t)0x4c40,(int16_t)0x9930,(int16_t)0xdad8,(int16_t)0x8583,
  (int16_t)0x877b,(int16_t)0xd4e1,(int16_t)0x49b4,(int16_t)0x9759,
  (int16_t)0xd4e1,(int16_t)0x877b,(int16_t)0x84a3,(int16_t)0xdddc,
  (int16_t)0x471d,(int16_t)0x9592,(int16_t)0xcf04,(int16_t)0x89be,
  (int16_t)0x8276,(int16_t)0xe707,(int16_t)0x447b,(int16_t)0x93dc,
  (int16_t)0xc946,(int16_t)0x8c4a,(int16_t)0x80f6,(int16_t)0xf055,
  (int16_t)0x41ce,(int16_t)0x9236,(int16_t)0xc3a9,(int16_t)0x8f1d,
  (int16_t)0x8027,(int16_t)0xf9b8,(int16_t)0x3f17,(int16_t)0x90a1,
  (int16_t)0xbe32,(int16_t)0x9236,(int16_t)0x800a,(int16_t)0x0324,
  (int16_t)0x3c57,(int16_t)0x8f1d,(int16_t)0xb8e3,(int16_t)0x9592,
  (int16_t)0x809e,(int16_t)0x0c8c,(int16_t)0x398d,(int16_t)0x8dab,
  (int16_t)0xb3c0,(int16_t)0x9930,(int16_t)0x81e2,(int16_t)0x15e2,
  (int16_t)0x36ba,(int16_t)0x8c4a,(int16_t)0xaecc,(int16_t)0x9d0e,
  (int16_t)0x83d6,(int16_t)0x1f1a,(int16_t)0x33df,(int16_t)0x8afb,
  (int16_t)0xaa0a,(int16_t)0xa129,(int16_t)0x8676,(int16_t)0x2827,
  (int16_t)0x30fc,(int16_t)0x89be,(int16_t)0xa57e,(int16_t)0xa57e,
  (int16_t)0x89be,(int16_t)0x30fc,(int16_t)0x2e11,(int16_t)0x8894,
  (int16_t)0xa129,(int16_t)0xaa0a,(int16_t)0x8dab,(int16_t)0x398d,
  (int16_t)0x2b1f,(int16_t)0x877b,(int16_t)0x9d0e,(int16_t)0xaecc,
  (int16_t)0x9236,(int16_t)0x41ce,(int16_t)0x2827,(int16_t)0x8676,
  (int16_t)0x9930,(int16_t)0xb3c0,(int16_t)0x9759,(int16_t)0x49b4,
  (int16_t)0x2528,(int16_t)0x8583,(int16_t)0x9592,(int16_t)0xb8e3,
  (int16_t)0x9d0e,(int16_t)0x5134,(int16_t)0x2224,(int16_t)0x84a3,
  (int16_t)0x9236,(int16_t)0xbe32,(int16_t)0xa34c,(int16_t)0x5843,
  (int16_t)0x1f1a,(int16_t)0x83d6,(int16_t)0x8f1d,(int16_t)0xc3a9,
  (int16_t)0xaa0a,(int16_t)0x5ed7,(int16_t)0x1c0c,(int16_t)0x831c,
  (int16_t)0x8c4a,(int16_t)0xc946,(int16_t)0xb140,(int16_t)0x64e9,
  (int16_t)0x18f9,(int16_t)0x8276,(int16_t)0x89be,(int16_t)0xcf04,
  (int16_t)0xb8e3,(int16_t)0x6a6e,(int16_t)0x15e2,(int16_t)0x81e2,
  (int16_t)0x877b,(int16_t)0xd4e1,(int16_t)0xc0e9,(int16_t)0x6f5f,
  (int16_t)0x12c8,(int16_t)0x8163,(int16_t)0x8583,(int16_t)0xdad8,
  (int16_t)0xc946,(int16_t)0x73b6,(int16_t)0x0fab,(int16_t)0x80f6,
  (int16_t)0x83d6,(int16_t)0xe0e6,(int16_t)0xd1ef,(int16_t)0x776c,
  (int16_t)0x0c8c,(int16_t)0x809e,(int16_t)0x8276,(int16_t)0xe707,
  (int16_t)0xdad8,(int16_t)0x7a7d,(int16_t)0x096b,(int16_t)0x8059,
  (int16_t)0x8163,(int16_t)0xed38,(int16_t)0xe3f4,(int16_t)0x7ce4,
  (int16_t)0x0648,(int16_t)0x8027,(int16_t)0x809e,(int16_t)0xf374,
  (int16_t)0xed38,(int16_t)0x7e9d,(int16_t)0x0324,(int16_t)0x800a,
  (int16_t)0x8027,(int16_t)0xf9b8,(int16_t)0xf695,(int16_t)0x7fa7
};
