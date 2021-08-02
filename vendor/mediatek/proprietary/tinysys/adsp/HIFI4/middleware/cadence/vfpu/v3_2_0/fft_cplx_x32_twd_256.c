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
    32-bit twiddle factor table for 256-point complex-valued fwd/inv FFT
  IntegrIT, 2006-2014
*/

/* Datatypes and helper definitions. */
#include "common.h"
/* Twiddle factor tables for complex-valued FFTs */
#include "fft_cplx_twd.h"

static const fft_cplx_x32_descr_t fft_cplx_x32_256_descr = { 256, fft_cplx_x32_twd_256 };

const fft_handle_t h_fft_cplx_x32_256  = (fft_handle_t)&fft_cplx_x32_256_descr;
const fft_handle_t h_ifft_cplx_x32_256 = (fft_handle_t)&fft_cplx_x32_256_descr;

// N = 256;
// tw = reshape(exp(-2j*pi*[1;2;3]*(0:N/4-1)/N),1,3*N/4);
const int32_t ALIGN(8) fft_cplx_x32_twd_256[256*3/4*2] =
{
  (int32_t)0x7fffffff,(int32_t)0x00000000,(int32_t)0x7fffffff,(int32_t)0x00000000,
  (int32_t)0x7fffffff,(int32_t)0x00000000,(int32_t)0x7ff62182,(int32_t)0xfcdbd541,
  (int32_t)0x7fd8878e,(int32_t)0xf9b82684,(int32_t)0x7fa736b4,(int32_t)0xf6956fb7,
  (int32_t)0x7fd8878e,(int32_t)0xf9b82684,(int32_t)0x7f62368f,(int32_t)0xf3742ca2,
  (int32_t)0x7e9d55fc,(int32_t)0xed37ef91,(int32_t)0x7fa736b4,(int32_t)0xf6956fb7,
  (int32_t)0x7e9d55fc,(int32_t)0xed37ef91,(int32_t)0x7ce3ceb2,(int32_t)0xe3f47d96,
  (int32_t)0x7f62368f,(int32_t)0xf3742ca2,(int32_t)0x7d8a5f40,(int32_t)0xe70747c4,
  (int32_t)0x7a7d055b,(int32_t)0xdad7f3a2,(int32_t)0x7f0991c4,(int32_t)0xf054d8d5,
  (int32_t)0x7c29fbee,(int32_t)0xe0e60685,(int32_t)0x776c4edb,(int32_t)0xd1eef59e,
  (int32_t)0x7e9d55fc,(int32_t)0xed37ef91,(int32_t)0x7a7d055b,(int32_t)0xdad7f3a2,
  (int32_t)0x73b5ebd1,(int32_t)0xc945dfec,(int32_t)0x7e1d93ea,(int32_t)0xea1debbb,
  (int32_t)0x78848414,(int32_t)0xd4e0cb15,(int32_t)0x6f5f02b2,(int32_t)0xc0e8b648,
  (int32_t)0x7d8a5f40,(int32_t)0xe70747c4,(int32_t)0x7641af3d,(int32_t)0xcf043ab3,
  (int32_t)0x6a6d98a4,(int32_t)0xb8e31319,(int32_t)0x7ce3ceb2,(int32_t)0xe3f47d96,
  (int32_t)0x73b5ebd1,(int32_t)0xc945dfec,(int32_t)0x64e88926,(int32_t)0xb140175b,
  (int32_t)0x7c29fbee,(int32_t)0xe0e60685,(int32_t)0x70e2cbc6,(int32_t)0xc3a94590,
  (int32_t)0x5ed77c8a,(int32_t)0xaa0a5b2e,(int32_t)0x7b5d039e,(int32_t)0xdddc5b3b,
  (int32_t)0x6dca0d14,(int32_t)0xbe31e19b,(int32_t)0x5842dd54,(int32_t)0xa34bdf20,
  (int32_t)0x7a7d055b,(int32_t)0xdad7f3a2,(int32_t)0x6a6d98a4,(int32_t)0xb8e31319,
  (int32_t)0x5133cc94,(int32_t)0x9d0dfe54,(int32_t)0x798a23b1,(int32_t)0xd7d946d8,
  (int32_t)0x66cf8120,(int32_t)0xb3c0200c,(int32_t)0x49b41533,(int32_t)0x9759617f,
  (int32_t)0x78848414,(int32_t)0xd4e0cb15,(int32_t)0x62f201ac,(int32_t)0xaecc336c,
  (int32_t)0x41ce1e65,(int32_t)0x9235f2ec,(int32_t)0x776c4edb,(int32_t)0xd1eef59e,
  (int32_t)0x5ed77c8a,(int32_t)0xaa0a5b2e,(int32_t)0x398cdd32,(int32_t)0x8daad37b,
  (int32_t)0x7641af3d,(int32_t)0xcf043ab3,(int32_t)0x5a82799a,(int32_t)0xa57d8666,
  (int32_t)0x30fbc54d,(int32_t)0x89be50c3,(int32_t)0x7504d345,(int32_t)0xcc210d79,
  (int32_t)0x55f5a4d2,(int32_t)0xa1288376,(int32_t)0x2826b928,(int32_t)0x8675dc4f,
  (int32_t)0x73b5ebd1,(int32_t)0xc945dfec,(int32_t)0x5133cc94,(int32_t)0x9d0dfe54,
  (int32_t)0x1f19f97b,(int32_t)0x83d60412,(int32_t)0x72552c85,(int32_t)0xc67322ce,
  (int32_t)0x4c3fdff4,(int32_t)0x99307ee0,(int32_t)0x15e21445,(int32_t)0x81e26c16,
  (int32_t)0x70e2cbc6,(int32_t)0xc3a94590,(int32_t)0x471cece7,(int32_t)0x9592675c,
  (int32_t)0x0c8bd35e,(int32_t)0x809dc971,(int32_t)0x6f5f02b2,(int32_t)0xc0e8b648,
  (int32_t)0x41ce1e65,(int32_t)0x9235f2ec,(int32_t)0x03242abf,(int32_t)0x8009de7e,
  (int32_t)0x6dca0d14,(int32_t)0xbe31e19b,(int32_t)0x3c56ba70,(int32_t)0x8f1d343a,
  (int32_t)0xf9b82684,(int32_t)0x80277872,(int32_t)0x6c242960,(int32_t)0xbb8532b0,
  (int32_t)0x36ba2014,(int32_t)0x8c4a142f,(int32_t)0xf054d8d5,(int32_t)0x80f66e3c,
  (int32_t)0x6a6d98a4,(int32_t)0xb8e31319,(int32_t)0x30fbc54d,(int32_t)0x89be50c3,
  (int32_t)0xe70747c4,(int32_t)0x8275a0c0,(int32_t)0x68a69e81,(int32_t)0xb64beacd,
  (int32_t)0x2b1f34eb,(int32_t)0x877b7bec,(int32_t)0xdddc5b3b,(int32_t)0x84a2fc62,
  (int32_t)0x66cf8120,(int32_t)0xb3c0200c,(int32_t)0x25280c5e,(int32_t)0x8582faa5,
  (int32_t)0xd4e0cb15,(int32_t)0x877b7bec,(int32_t)0x64e88926,(int32_t)0xb140175b,
  (int32_t)0x1f19f97b,(int32_t)0x83d60412,(int32_t)0xcc210d79,(int32_t)0x8afb2cbb,
  (int32_t)0x62f201ac,(int32_t)0xaecc336c,(int32_t)0x18f8b83c,(int32_t)0x8275a0c0,
  (int32_t)0xc3a94590,(int32_t)0x8f1d343a,(int32_t)0x60ec3830,(int32_t)0xac64d510,
  (int32_t)0x12c8106f,(int32_t)0x8162aa04,(int32_t)0xbb8532b0,(int32_t)0x93dbd6a0,
  (int32_t)0x5ed77c8a,(int32_t)0xaa0a5b2e,(int32_t)0x0c8bd35e,(int32_t)0x809dc971,
  (int32_t)0xb3c0200c,(int32_t)0x99307ee0,(int32_t)0x5cb420e0,(int32_t)0xa7bd22ac,
  (int32_t)0x0647d97c,(int32_t)0x80277872,(int32_t)0xac64d510,(int32_t)0x9f13c7d0,
  (int32_t)0x5a82799a,(int32_t)0xa57d8666,(int32_t)0x00000000,(int32_t)0x80000000,
  (int32_t)0xa57d8666,(int32_t)0xa57d8666,(int32_t)0x5842dd54,(int32_t)0xa34bdf20,
  (int32_t)0xf9b82684,(int32_t)0x80277872,(int32_t)0x9f13c7d0,(int32_t)0xac64d510,
  (int32_t)0x55f5a4d2,(int32_t)0xa1288376,(int32_t)0xf3742ca2,(int32_t)0x809dc971,
  (int32_t)0x99307ee0,(int32_t)0xb3c0200c,(int32_t)0x539b2af0,(int32_t)0x9f13c7d0,
  (int32_t)0xed37ef91,(int32_t)0x8162aa04,(int32_t)0x93dbd6a0,(int32_t)0xbb8532b0,
  (int32_t)0x5133cc94,(int32_t)0x9d0dfe54,(int32_t)0xe70747c4,(int32_t)0x8275a0c0,
  (int32_t)0x8f1d343a,(int32_t)0xc3a94590,(int32_t)0x4ebfe8a5,(int32_t)0x9b1776da,
  (int32_t)0xe0e60685,(int32_t)0x83d60412,(int32_t)0x8afb2cbb,(int32_t)0xcc210d79,
  (int32_t)0x4c3fdff4,(int32_t)0x99307ee0,(int32_t)0xdad7f3a2,(int32_t)0x8582faa5,
  (int32_t)0x877b7bec,(int32_t)0xd4e0cb15,(int32_t)0x49b41533,(int32_t)0x9759617f,
  (int32_t)0xd4e0cb15,(int32_t)0x877b7bec,(int32_t)0x84a2fc62,(int32_t)0xdddc5b3b,
  (int32_t)0x471cece7,(int32_t)0x9592675c,(int32_t)0xcf043ab3,(int32_t)0x89be50c3,
  (int32_t)0x8275a0c0,(int32_t)0xe70747c4,(int32_t)0x447acd50,(int32_t)0x93dbd6a0,
  (int32_t)0xc945dfec,(int32_t)0x8c4a142f,(int32_t)0x80f66e3c,(int32_t)0xf054d8d5,
  (int32_t)0x41ce1e65,(int32_t)0x9235f2ec,(int32_t)0xc3a94590,(int32_t)0x8f1d343a,
  (int32_t)0x80277872,(int32_t)0xf9b82684,(int32_t)0x3f1749b8,(int32_t)0x90a0fd4e,
  (int32_t)0xbe31e19b,(int32_t)0x9235f2ec,(int32_t)0x8009de7e,(int32_t)0x03242abf,
  (int32_t)0x3c56ba70,(int32_t)0x8f1d343a,(int32_t)0xb8e31319,(int32_t)0x9592675c,
  (int32_t)0x809dc971,(int32_t)0x0c8bd35e,(int32_t)0x398cdd32,(int32_t)0x8daad37b,
  (int32_t)0xb3c0200c,(int32_t)0x99307ee0,(int32_t)0x81e26c16,(int32_t)0x15e21445,
  (int32_t)0x36ba2014,(int32_t)0x8c4a142f,(int32_t)0xaecc336c,(int32_t)0x9d0dfe54,
  (int32_t)0x83d60412,(int32_t)0x1f19f97b,(int32_t)0x33def287,(int32_t)0x8afb2cbb,
  (int32_t)0xaa0a5b2e,(int32_t)0xa1288376,(int32_t)0x8675dc4f,(int32_t)0x2826b928,
  (int32_t)0x30fbc54d,(int32_t)0x89be50c3,(int32_t)0xa57d8666,(int32_t)0xa57d8666,
  (int32_t)0x89be50c3,(int32_t)0x30fbc54d,(int32_t)0x2e110a62,(int32_t)0x8893b125,
  (int32_t)0xa1288376,(int32_t)0xaa0a5b2e,(int32_t)0x8daad37b,(int32_t)0x398cdd32,
  (int32_t)0x2b1f34eb,(int32_t)0x877b7bec,(int32_t)0x9d0dfe54,(int32_t)0xaecc336c,
  (int32_t)0x9235f2ec,(int32_t)0x41ce1e65,(int32_t)0x2826b928,(int32_t)0x8675dc4f,
  (int32_t)0x99307ee0,(int32_t)0xb3c0200c,(int32_t)0x9759617f,(int32_t)0x49b41533,
  (int32_t)0x25280c5e,(int32_t)0x8582faa5,(int32_t)0x9592675c,(int32_t)0xb8e31319,
  (int32_t)0x9d0dfe54,(int32_t)0x5133cc94,(int32_t)0x2223a4c5,(int32_t)0x84a2fc62,
  (int32_t)0x9235f2ec,(int32_t)0xbe31e19b,(int32_t)0xa34bdf20,(int32_t)0x5842dd54,
  (int32_t)0x1f19f97b,(int32_t)0x83d60412,(int32_t)0x8f1d343a,(int32_t)0xc3a94590,
  (int32_t)0xaa0a5b2e,(int32_t)0x5ed77c8a,(int32_t)0x1c0b826a,(int32_t)0x831c314e,
  (int32_t)0x8c4a142f,(int32_t)0xc945dfec,(int32_t)0xb140175b,(int32_t)0x64e88926,
  (int32_t)0x18f8b83c,(int32_t)0x8275a0c0,(int32_t)0x89be50c3,(int32_t)0xcf043ab3,
  (int32_t)0xb8e31319,(int32_t)0x6a6d98a4,(int32_t)0x15e21445,(int32_t)0x81e26c16,
  (int32_t)0x877b7bec,(int32_t)0xd4e0cb15,(int32_t)0xc0e8b648,(int32_t)0x6f5f02b2,
  (int32_t)0x12c8106f,(int32_t)0x8162aa04,(int32_t)0x8582faa5,(int32_t)0xdad7f3a2,
  (int32_t)0xc945dfec,(int32_t)0x73b5ebd1,(int32_t)0x0fab272b,(int32_t)0x80f66e3c,
  (int32_t)0x83d60412,(int32_t)0xe0e60685,(int32_t)0xd1eef59e,(int32_t)0x776c4edb,
  (int32_t)0x0c8bd35e,(int32_t)0x809dc971,(int32_t)0x8275a0c0,(int32_t)0xe70747c4,
  (int32_t)0xdad7f3a2,(int32_t)0x7a7d055b,(int32_t)0x096a9049,(int32_t)0x8058c94c,
  (int32_t)0x8162aa04,(int32_t)0xed37ef91,(int32_t)0xe3f47d96,(int32_t)0x7ce3ceb2,
  (int32_t)0x0647d97c,(int32_t)0x80277872,(int32_t)0x809dc971,(int32_t)0xf3742ca2,
  (int32_t)0xed37ef91,(int32_t)0x7e9d55fc,(int32_t)0x03242abf,(int32_t)0x8009de7e,
  (int32_t)0x80277872,(int32_t)0xf9b82684,(int32_t)0xf6956fb7,(int32_t)0x7fa736b4
};
