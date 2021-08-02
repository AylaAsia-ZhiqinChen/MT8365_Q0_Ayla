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
    32-bit twiddle factor table for 512-point real-valued fwd/inv FFT
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
static const int32_t fft_real_x32_spc[512*1/4*2] =
{
  (int32_t)0x00000000,(int32_t)0x80000000,(int32_t)0xfe6de2e0,(int32_t)0x800277a6,
  (int32_t)0xfcdbd541,(int32_t)0x8009de7e,(int32_t)0xfb49e6a3,(int32_t)0x80163440,
  (int32_t)0xf9b82684,(int32_t)0x80277872,(int32_t)0xf826a462,(int32_t)0x803daa6a,
  (int32_t)0xf6956fb7,(int32_t)0x8058c94c,(int32_t)0xf50497fb,(int32_t)0x8078d40d,
  (int32_t)0xf3742ca2,(int32_t)0x809dc971,(int32_t)0xf1e43d1c,(int32_t)0x80c7a80a,
  (int32_t)0xf054d8d5,(int32_t)0x80f66e3c,(int32_t)0xeec60f31,(int32_t)0x812a1a3a,
  (int32_t)0xed37ef91,(int32_t)0x8162aa04,(int32_t)0xebaa894f,(int32_t)0x81a01b6d,
  (int32_t)0xea1debbb,(int32_t)0x81e26c16,(int32_t)0xe8922622,(int32_t)0x82299971,
  (int32_t)0xe70747c4,(int32_t)0x8275a0c0,(int32_t)0xe57d5fda,(int32_t)0x82c67f14,
  (int32_t)0xe3f47d96,(int32_t)0x831c314e,(int32_t)0xe26cb01b,(int32_t)0x8376b422,
  (int32_t)0xe0e60685,(int32_t)0x83d60412,(int32_t)0xdf608fe4,(int32_t)0x843a1d70,
  (int32_t)0xdddc5b3b,(int32_t)0x84a2fc62,(int32_t)0xdc597781,(int32_t)0x85109cdd,
  (int32_t)0xdad7f3a2,(int32_t)0x8582faa5,(int32_t)0xd957de7a,(int32_t)0x85fa1153,
  (int32_t)0xd7d946d8,(int32_t)0x8675dc4f,(int32_t)0xd65c3b7b,(int32_t)0x86f656d3,
  (int32_t)0xd4e0cb15,(int32_t)0x877b7bec,(int32_t)0xd3670446,(int32_t)0x88054677,
  (int32_t)0xd1eef59e,(int32_t)0x8893b125,(int32_t)0xd078ad9e,(int32_t)0x8926b677,
  (int32_t)0xcf043ab3,(int32_t)0x89be50c3,(int32_t)0xcd91ab39,(int32_t)0x8a5a7a31,
  (int32_t)0xcc210d79,(int32_t)0x8afb2cbb,(int32_t)0xcab26fa9,(int32_t)0x8ba0622f,
  (int32_t)0xc945dfec,(int32_t)0x8c4a142f,(int32_t)0xc7db6c50,(int32_t)0x8cf83c30,
  (int32_t)0xc67322ce,(int32_t)0x8daad37b,(int32_t)0xc50d1149,(int32_t)0x8e61d32e,
  (int32_t)0xc3a94590,(int32_t)0x8f1d343a,(int32_t)0xc247cd5a,(int32_t)0x8fdcef66,
  (int32_t)0xc0e8b648,(int32_t)0x90a0fd4e,(int32_t)0xbf8c0de3,(int32_t)0x91695663,
  (int32_t)0xbe31e19b,(int32_t)0x9235f2ec,(int32_t)0xbcda3ecb,(int32_t)0x9306cb04,
  (int32_t)0xbb8532b0,(int32_t)0x93dbd6a0,(int32_t)0xba32ca71,(int32_t)0x94b50d87,
  (int32_t)0xb8e31319,(int32_t)0x9592675c,(int32_t)0xb796199b,(int32_t)0x9673db94,
  (int32_t)0xb64beacd,(int32_t)0x9759617f,(int32_t)0xb5049368,(int32_t)0x9842f043,
  (int32_t)0xb3c0200c,(int32_t)0x99307ee0,(int32_t)0xb27e9d3c,(int32_t)0x9a22042d,
  (int32_t)0xb140175b,(int32_t)0x9b1776da,(int32_t)0xb0049ab3,(int32_t)0x9c10cd70,
  (int32_t)0xaecc336c,(int32_t)0x9d0dfe54,(int32_t)0xad96ed92,(int32_t)0x9e0effc1,
  (int32_t)0xac64d510,(int32_t)0x9f13c7d0,(int32_t)0xab35f5b5,(int32_t)0xa01c4c73,
  (int32_t)0xaa0a5b2e,(int32_t)0xa1288376,(int32_t)0xa8e21106,(int32_t)0xa2386284,
  (int32_t)0xa7bd22ac,(int32_t)0xa34bdf20,(int32_t)0xa69b9b68,(int32_t)0xa462eeac,
  (int32_t)0xa57d8666,(int32_t)0xa57d8666,(int32_t)0xa462eeac,(int32_t)0xa69b9b68,
  (int32_t)0xa34bdf20,(int32_t)0xa7bd22ac,(int32_t)0xa2386284,(int32_t)0xa8e21106,
  (int32_t)0xa1288376,(int32_t)0xaa0a5b2e,(int32_t)0xa01c4c73,(int32_t)0xab35f5b5,
  (int32_t)0x9f13c7d0,(int32_t)0xac64d510,(int32_t)0x9e0effc1,(int32_t)0xad96ed92,
  (int32_t)0x9d0dfe54,(int32_t)0xaecc336c,(int32_t)0x9c10cd70,(int32_t)0xb0049ab3,
  (int32_t)0x9b1776da,(int32_t)0xb140175b,(int32_t)0x9a22042d,(int32_t)0xb27e9d3c,
  (int32_t)0x99307ee0,(int32_t)0xb3c0200c,(int32_t)0x9842f043,(int32_t)0xb5049368,
  (int32_t)0x9759617f,(int32_t)0xb64beacd,(int32_t)0x9673db94,(int32_t)0xb796199b,
  (int32_t)0x9592675c,(int32_t)0xb8e31319,(int32_t)0x94b50d87,(int32_t)0xba32ca71,
  (int32_t)0x93dbd6a0,(int32_t)0xbb8532b0,(int32_t)0x9306cb04,(int32_t)0xbcda3ecb,
  (int32_t)0x9235f2ec,(int32_t)0xbe31e19b,(int32_t)0x91695663,(int32_t)0xbf8c0de3,
  (int32_t)0x90a0fd4e,(int32_t)0xc0e8b648,(int32_t)0x8fdcef66,(int32_t)0xc247cd5a,
  (int32_t)0x8f1d343a,(int32_t)0xc3a94590,(int32_t)0x8e61d32e,(int32_t)0xc50d1149,
  (int32_t)0x8daad37b,(int32_t)0xc67322ce,(int32_t)0x8cf83c30,(int32_t)0xc7db6c50,
  (int32_t)0x8c4a142f,(int32_t)0xc945dfec,(int32_t)0x8ba0622f,(int32_t)0xcab26fa9,
  (int32_t)0x8afb2cbb,(int32_t)0xcc210d79,(int32_t)0x8a5a7a31,(int32_t)0xcd91ab39,
  (int32_t)0x89be50c3,(int32_t)0xcf043ab3,(int32_t)0x8926b677,(int32_t)0xd078ad9e,
  (int32_t)0x8893b125,(int32_t)0xd1eef59e,(int32_t)0x88054677,(int32_t)0xd3670446,
  (int32_t)0x877b7bec,(int32_t)0xd4e0cb15,(int32_t)0x86f656d3,(int32_t)0xd65c3b7b,
  (int32_t)0x8675dc4f,(int32_t)0xd7d946d8,(int32_t)0x85fa1153,(int32_t)0xd957de7a,
  (int32_t)0x8582faa5,(int32_t)0xdad7f3a2,(int32_t)0x85109cdd,(int32_t)0xdc597781,
  (int32_t)0x84a2fc62,(int32_t)0xdddc5b3b,(int32_t)0x843a1d70,(int32_t)0xdf608fe4,
  (int32_t)0x83d60412,(int32_t)0xe0e60685,(int32_t)0x8376b422,(int32_t)0xe26cb01b,
  (int32_t)0x831c314e,(int32_t)0xe3f47d96,(int32_t)0x82c67f14,(int32_t)0xe57d5fda,
  (int32_t)0x8275a0c0,(int32_t)0xe70747c4,(int32_t)0x82299971,(int32_t)0xe8922622,
  (int32_t)0x81e26c16,(int32_t)0xea1debbb,(int32_t)0x81a01b6d,(int32_t)0xebaa894f,
  (int32_t)0x8162aa04,(int32_t)0xed37ef91,(int32_t)0x812a1a3a,(int32_t)0xeec60f31,
  (int32_t)0x80f66e3c,(int32_t)0xf054d8d5,(int32_t)0x80c7a80a,(int32_t)0xf1e43d1c,
  (int32_t)0x809dc971,(int32_t)0xf3742ca2,(int32_t)0x8078d40d,(int32_t)0xf50497fb,
  (int32_t)0x8058c94c,(int32_t)0xf6956fb7,(int32_t)0x803daa6a,(int32_t)0xf826a462,
  (int32_t)0x80277872,(int32_t)0xf9b82684,(int32_t)0x80163440,(int32_t)0xfb49e6a3,
  (int32_t)0x8009de7e,(int32_t)0xfcdbd541,(int32_t)0x800277a6,(int32_t)0xfe6de2e0
};

static const fft_real_x32_descr_t fft_real_x32_descr =
{ 
  512, fft_cplx_x32_twd_256, fft_real_x32_spc
};

const fft_handle_t h_fft_real_x32_512  = (fft_handle_t)&fft_real_x32_descr;
const fft_handle_t h_ifft_real_x32_512 = (fft_handle_t)&fft_real_x32_descr;
