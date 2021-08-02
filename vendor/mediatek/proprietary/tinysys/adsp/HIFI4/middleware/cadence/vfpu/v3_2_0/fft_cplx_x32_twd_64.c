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
    32-bit twiddle factor table for 64-point complex-valued fwd/inv FFT
  IntegrIT, 2006-2014
*/

/* Datatypes and helper definitions. */
#include "common.h"
/* Twiddle factor tables for complex-valued FFTs */
#include "fft_cplx_twd.h"

static const fft_cplx_x32_descr_t fft_cplx_x32_64_descr = { 64, fft_cplx_x32_twd_64 };

const fft_handle_t h_fft_cplx_x32_64  = (fft_handle_t)&fft_cplx_x32_64_descr;
const fft_handle_t h_ifft_cplx_x32_64 = (fft_handle_t)&fft_cplx_x32_64_descr;

// N = 64;
// tw = reshape(exp(-2j*pi*[1;2;3]*(0:N/4-1)/N),1,3*N/4);
const int32_t ALIGN(8) fft_cplx_x32_twd_64[64*3/4*2] =
{
  (int32_t)0x7fffffff,(int32_t)0x00000000,(int32_t)0x7fffffff,(int32_t)0x00000000,
  (int32_t)0x7fffffff,(int32_t)0x00000000,(int32_t)0x7f62368f,(int32_t)0xf3742ca2,
  (int32_t)0x7d8a5f40,(int32_t)0xe70747c4,(int32_t)0x7a7d055b,(int32_t)0xdad7f3a2,
  (int32_t)0x7d8a5f40,(int32_t)0xe70747c4,(int32_t)0x7641af3d,(int32_t)0xcf043ab3,
  (int32_t)0x6a6d98a4,(int32_t)0xb8e31319,(int32_t)0x7a7d055b,(int32_t)0xdad7f3a2,
  (int32_t)0x6a6d98a4,(int32_t)0xb8e31319,(int32_t)0x5133cc94,(int32_t)0x9d0dfe54,
  (int32_t)0x7641af3d,(int32_t)0xcf043ab3,(int32_t)0x5a82799a,(int32_t)0xa57d8666,
  (int32_t)0x30fbc54d,(int32_t)0x89be50c3,(int32_t)0x70e2cbc6,(int32_t)0xc3a94590,
  (int32_t)0x471cece7,(int32_t)0x9592675c,(int32_t)0x0c8bd35e,(int32_t)0x809dc971,
  (int32_t)0x6a6d98a4,(int32_t)0xb8e31319,(int32_t)0x30fbc54d,(int32_t)0x89be50c3,
  (int32_t)0xe70747c4,(int32_t)0x8275a0c0,(int32_t)0x62f201ac,(int32_t)0xaecc336c,
  (int32_t)0x18f8b83c,(int32_t)0x8275a0c0,(int32_t)0xc3a94590,(int32_t)0x8f1d343a,
  (int32_t)0x5a82799a,(int32_t)0xa57d8666,(int32_t)0x00000000,(int32_t)0x80000000,
  (int32_t)0xa57d8666,(int32_t)0xa57d8666,(int32_t)0x5133cc94,(int32_t)0x9d0dfe54,
  (int32_t)0xe70747c4,(int32_t)0x8275a0c0,(int32_t)0x8f1d343a,(int32_t)0xc3a94590,
  (int32_t)0x471cece7,(int32_t)0x9592675c,(int32_t)0xcf043ab3,(int32_t)0x89be50c3,
  (int32_t)0x8275a0c0,(int32_t)0xe70747c4,(int32_t)0x3c56ba70,(int32_t)0x8f1d343a,
  (int32_t)0xb8e31319,(int32_t)0x9592675c,(int32_t)0x809dc971,(int32_t)0x0c8bd35e,
  (int32_t)0x30fbc54d,(int32_t)0x89be50c3,(int32_t)0xa57d8666,(int32_t)0xa57d8666,
  (int32_t)0x89be50c3,(int32_t)0x30fbc54d,(int32_t)0x25280c5e,(int32_t)0x8582faa5,
  (int32_t)0x9592675c,(int32_t)0xb8e31319,(int32_t)0x9d0dfe54,(int32_t)0x5133cc94,
  (int32_t)0x18f8b83c,(int32_t)0x8275a0c0,(int32_t)0x89be50c3,(int32_t)0xcf043ab3,
  (int32_t)0xb8e31319,(int32_t)0x6a6d98a4,(int32_t)0x0c8bd35e,(int32_t)0x809dc971,
  (int32_t)0x8275a0c0,(int32_t)0xe70747c4,(int32_t)0xdad7f3a2,(int32_t)0x7a7d055b
};
