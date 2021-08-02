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
    32-bit twiddle factor table for 1024-point real-valued fwd/inv FFT
  IntegrIT, 2006-2014
*/

/* Datatypes and helper definitions. */
#include "common.h"
/* Descriptor structures for real-valued FFTs. */
#include "fft_real_twd.h"
/* Twiddle factor tables for complex-valued FFTs. */
#include "fft_cplx_twd.h"

// N = 1024;
// spc = -1j*exp(-2j*pi*(0:N/4-1)/N);
static const int32_t fft_real_x32_spc[1024*1/4*2] =
{
  (int32_t)0x00000000,(int32_t)0x80000000,(int32_t)0xff36f078,(int32_t)0x80009dea,
  (int32_t)0xfe6de2e0,(int32_t)0x800277a6,(int32_t)0xfda4d929,(int32_t)0x80058d2f,
  (int32_t)0xfcdbd541,(int32_t)0x8009de7e,(int32_t)0xfc12d91a,(int32_t)0x800f6b88,
  (int32_t)0xfb49e6a3,(int32_t)0x80163440,(int32_t)0xfa80ffcb,(int32_t)0x801e3895,
  (int32_t)0xf9b82684,(int32_t)0x80277872,(int32_t)0xf8ef5cbb,(int32_t)0x8031f3c2,
  (int32_t)0xf826a462,(int32_t)0x803daa6a,(int32_t)0xf75dff66,(int32_t)0x804a9c4d,
  (int32_t)0xf6956fb7,(int32_t)0x8058c94c,(int32_t)0xf5ccf743,(int32_t)0x80683143,
  (int32_t)0xf50497fb,(int32_t)0x8078d40d,(int32_t)0xf43c53cb,(int32_t)0x808ab180,
  (int32_t)0xf3742ca2,(int32_t)0x809dc971,(int32_t)0xf2ac246e,(int32_t)0x80b21baf,
  (int32_t)0xf1e43d1c,(int32_t)0x80c7a80a,(int32_t)0xf11c789a,(int32_t)0x80de6e4c,
  (int32_t)0xf054d8d5,(int32_t)0x80f66e3c,(int32_t)0xef8d5fb8,(int32_t)0x810fa7a0,
  (int32_t)0xeec60f31,(int32_t)0x812a1a3a,(int32_t)0xedfee92b,(int32_t)0x8145c5c7,
  (int32_t)0xed37ef91,(int32_t)0x8162aa04,(int32_t)0xec71244f,(int32_t)0x8180c6a9,
  (int32_t)0xebaa894f,(int32_t)0x81a01b6d,(int32_t)0xeae4207a,(int32_t)0x81c0a801,
  (int32_t)0xea1debbb,(int32_t)0x81e26c16,(int32_t)0xe957ecfb,(int32_t)0x82056758,
  (int32_t)0xe8922622,(int32_t)0x82299971,(int32_t)0xe7cc9917,(int32_t)0x824f0208,
  (int32_t)0xe70747c4,(int32_t)0x8275a0c0,(int32_t)0xe642340d,(int32_t)0x829d753a,
  (int32_t)0xe57d5fda,(int32_t)0x82c67f14,(int32_t)0xe4b8cd11,(int32_t)0x82f0bde8,
  (int32_t)0xe3f47d96,(int32_t)0x831c314e,(int32_t)0xe330734d,(int32_t)0x8348d8dc,
  (int32_t)0xe26cb01b,(int32_t)0x8376b422,(int32_t)0xe1a935e2,(int32_t)0x83a5c2b0,
  (int32_t)0xe0e60685,(int32_t)0x83d60412,(int32_t)0xe02323e5,(int32_t)0x840777d0,
  (int32_t)0xdf608fe4,(int32_t)0x843a1d70,(int32_t)0xde9e4c60,(int32_t)0x846df477,
  (int32_t)0xdddc5b3b,(int32_t)0x84a2fc62,(int32_t)0xdd1abe51,(int32_t)0x84d934b1,
  (int32_t)0xdc597781,(int32_t)0x85109cdd,(int32_t)0xdb9888a8,(int32_t)0x8549345c,
  (int32_t)0xdad7f3a2,(int32_t)0x8582faa5,(int32_t)0xda17ba4a,(int32_t)0x85bdef28,
  (int32_t)0xd957de7a,(int32_t)0x85fa1153,(int32_t)0xd898620c,(int32_t)0x86376092,
  (int32_t)0xd7d946d8,(int32_t)0x8675dc4f,(int32_t)0xd71a8eb5,(int32_t)0x86b583ee,
  (int32_t)0xd65c3b7b,(int32_t)0x86f656d3,(int32_t)0xd59e4eff,(int32_t)0x8738545e,
  (int32_t)0xd4e0cb15,(int32_t)0x877b7bec,(int32_t)0xd423b191,(int32_t)0x87bfccd7,
  (int32_t)0xd3670446,(int32_t)0x88054677,(int32_t)0xd2aac504,(int32_t)0x884be821,
  (int32_t)0xd1eef59e,(int32_t)0x8893b125,(int32_t)0xd13397e2,(int32_t)0x88dca0d3,
  (int32_t)0xd078ad9e,(int32_t)0x8926b677,(int32_t)0xcfbe389f,(int32_t)0x8971f15a,
  (int32_t)0xcf043ab3,(int32_t)0x89be50c3,(int32_t)0xce4ab5a2,(int32_t)0x8a0bd3f5,
  (int32_t)0xcd91ab39,(int32_t)0x8a5a7a31,(int32_t)0xccd91d3d,(int32_t)0x8aaa42b4,
  (int32_t)0xcc210d79,(int32_t)0x8afb2cbb,(int32_t)0xcb697db0,(int32_t)0x8b4d377c,
  (int32_t)0xcab26fa9,(int32_t)0x8ba0622f,(int32_t)0xc9fbe527,(int32_t)0x8bf4ac05,
  (int32_t)0xc945dfec,(int32_t)0x8c4a142f,(int32_t)0xc89061ba,(int32_t)0x8ca099da,
  (int32_t)0xc7db6c50,(int32_t)0x8cf83c30,(int32_t)0xc727016d,(int32_t)0x8d50fa59,
  (int32_t)0xc67322ce,(int32_t)0x8daad37b,(int32_t)0xc5bfd22e,(int32_t)0x8e05c6b7,
  (int32_t)0xc50d1149,(int32_t)0x8e61d32e,(int32_t)0xc45ae1d7,(int32_t)0x8ebef7fb,
  (int32_t)0xc3a94590,(int32_t)0x8f1d343a,(int32_t)0xc2f83e2a,(int32_t)0x8f7c8701,
  (int32_t)0xc247cd5a,(int32_t)0x8fdcef66,(int32_t)0xc197f4d4,(int32_t)0x903e6c7b,
  (int32_t)0xc0e8b648,(int32_t)0x90a0fd4e,(int32_t)0xc03a1368,(int32_t)0x9104a0ee,
  (int32_t)0xbf8c0de3,(int32_t)0x91695663,(int32_t)0xbedea765,(int32_t)0x91cf1cb6,
  (int32_t)0xbe31e19b,(int32_t)0x9235f2ec,(int32_t)0xbd85be30,(int32_t)0x929dd806,
  (int32_t)0xbcda3ecb,(int32_t)0x9306cb04,(int32_t)0xbc2f6513,(int32_t)0x9370cae4,
  (int32_t)0xbb8532b0,(int32_t)0x93dbd6a0,(int32_t)0xbadba943,(int32_t)0x9447ed2f,
  (int32_t)0xba32ca71,(int32_t)0x94b50d87,(int32_t)0xb98a97d8,(int32_t)0x9523369c,
  (int32_t)0xb8e31319,(int32_t)0x9592675c,(int32_t)0xb83c3dd1,(int32_t)0x96029eb6,
  (int32_t)0xb796199b,(int32_t)0x9673db94,(int32_t)0xb6f0a812,(int32_t)0x96e61ce0,
  (int32_t)0xb64beacd,(int32_t)0x9759617f,(int32_t)0xb5a7e362,(int32_t)0x97cda855,
  (int32_t)0xb5049368,(int32_t)0x9842f043,(int32_t)0xb461fc70,(int32_t)0x98b93828,
  (int32_t)0xb3c0200c,(int32_t)0x99307ee0,(int32_t)0xb31effcc,(int32_t)0x99a8c345,
  (int32_t)0xb27e9d3c,(int32_t)0x9a22042d,(int32_t)0xb1def9e9,(int32_t)0x9a9c406e,
  (int32_t)0xb140175b,(int32_t)0x9b1776da,(int32_t)0xb0a1f71d,(int32_t)0x9b93a641,
  (int32_t)0xb0049ab3,(int32_t)0x9c10cd70,(int32_t)0xaf6803a2,(int32_t)0x9c8eeb34,
  (int32_t)0xaecc336c,(int32_t)0x9d0dfe54,(int32_t)0xae312b92,(int32_t)0x9d8e0597,
  (int32_t)0xad96ed92,(int32_t)0x9e0effc1,(int32_t)0xacfd7ae8,(int32_t)0x9e90eb94,
  (int32_t)0xac64d510,(int32_t)0x9f13c7d0,(int32_t)0xabccfd83,(int32_t)0x9f979331,
  (int32_t)0xab35f5b5,(int32_t)0xa01c4c73,(int32_t)0xaa9fbf1e,(int32_t)0xa0a1f24d,
  (int32_t)0xaa0a5b2e,(int32_t)0xa1288376,(int32_t)0xa975cb57,(int32_t)0xa1affea3,
  (int32_t)0xa8e21106,(int32_t)0xa2386284,(int32_t)0xa84f2daa,(int32_t)0xa2c1adc9,
  (int32_t)0xa7bd22ac,(int32_t)0xa34bdf20,(int32_t)0xa72bf174,(int32_t)0xa3d6f534,
  (int32_t)0xa69b9b68,(int32_t)0xa462eeac,(int32_t)0xa60c21ee,(int32_t)0xa4efca31,
  (int32_t)0xa57d8666,(int32_t)0xa57d8666,(int32_t)0xa4efca31,(int32_t)0xa60c21ee,
  (int32_t)0xa462eeac,(int32_t)0xa69b9b68,(int32_t)0xa3d6f534,(int32_t)0xa72bf174,
  (int32_t)0xa34bdf20,(int32_t)0xa7bd22ac,(int32_t)0xa2c1adc9,(int32_t)0xa84f2daa,
  (int32_t)0xa2386284,(int32_t)0xa8e21106,(int32_t)0xa1affea3,(int32_t)0xa975cb57,
  (int32_t)0xa1288376,(int32_t)0xaa0a5b2e,(int32_t)0xa0a1f24d,(int32_t)0xaa9fbf1e,
  (int32_t)0xa01c4c73,(int32_t)0xab35f5b5,(int32_t)0x9f979331,(int32_t)0xabccfd83,
  (int32_t)0x9f13c7d0,(int32_t)0xac64d510,(int32_t)0x9e90eb94,(int32_t)0xacfd7ae8,
  (int32_t)0x9e0effc1,(int32_t)0xad96ed92,(int32_t)0x9d8e0597,(int32_t)0xae312b92,
  (int32_t)0x9d0dfe54,(int32_t)0xaecc336c,(int32_t)0x9c8eeb34,(int32_t)0xaf6803a2,
  (int32_t)0x9c10cd70,(int32_t)0xb0049ab3,(int32_t)0x9b93a641,(int32_t)0xb0a1f71d,
  (int32_t)0x9b1776da,(int32_t)0xb140175b,(int32_t)0x9a9c406e,(int32_t)0xb1def9e9,
  (int32_t)0x9a22042d,(int32_t)0xb27e9d3c,(int32_t)0x99a8c345,(int32_t)0xb31effcc,
  (int32_t)0x99307ee0,(int32_t)0xb3c0200c,(int32_t)0x98b93828,(int32_t)0xb461fc70,
  (int32_t)0x9842f043,(int32_t)0xb5049368,(int32_t)0x97cda855,(int32_t)0xb5a7e362,
  (int32_t)0x9759617f,(int32_t)0xb64beacd,(int32_t)0x96e61ce0,(int32_t)0xb6f0a812,
  (int32_t)0x9673db94,(int32_t)0xb796199b,(int32_t)0x96029eb6,(int32_t)0xb83c3dd1,
  (int32_t)0x9592675c,(int32_t)0xb8e31319,(int32_t)0x9523369c,(int32_t)0xb98a97d8,
  (int32_t)0x94b50d87,(int32_t)0xba32ca71,(int32_t)0x9447ed2f,(int32_t)0xbadba943,
  (int32_t)0x93dbd6a0,(int32_t)0xbb8532b0,(int32_t)0x9370cae4,(int32_t)0xbc2f6513,
  (int32_t)0x9306cb04,(int32_t)0xbcda3ecb,(int32_t)0x929dd806,(int32_t)0xbd85be30,
  (int32_t)0x9235f2ec,(int32_t)0xbe31e19b,(int32_t)0x91cf1cb6,(int32_t)0xbedea765,
  (int32_t)0x91695663,(int32_t)0xbf8c0de3,(int32_t)0x9104a0ee,(int32_t)0xc03a1368,
  (int32_t)0x90a0fd4e,(int32_t)0xc0e8b648,(int32_t)0x903e6c7b,(int32_t)0xc197f4d4,
  (int32_t)0x8fdcef66,(int32_t)0xc247cd5a,(int32_t)0x8f7c8701,(int32_t)0xc2f83e2a,
  (int32_t)0x8f1d343a,(int32_t)0xc3a94590,(int32_t)0x8ebef7fb,(int32_t)0xc45ae1d7,
  (int32_t)0x8e61d32e,(int32_t)0xc50d1149,(int32_t)0x8e05c6b7,(int32_t)0xc5bfd22e,
  (int32_t)0x8daad37b,(int32_t)0xc67322ce,(int32_t)0x8d50fa59,(int32_t)0xc727016d,
  (int32_t)0x8cf83c30,(int32_t)0xc7db6c50,(int32_t)0x8ca099da,(int32_t)0xc89061ba,
  (int32_t)0x8c4a142f,(int32_t)0xc945dfec,(int32_t)0x8bf4ac05,(int32_t)0xc9fbe527,
  (int32_t)0x8ba0622f,(int32_t)0xcab26fa9,(int32_t)0x8b4d377c,(int32_t)0xcb697db0,
  (int32_t)0x8afb2cbb,(int32_t)0xcc210d79,(int32_t)0x8aaa42b4,(int32_t)0xccd91d3d,
  (int32_t)0x8a5a7a31,(int32_t)0xcd91ab39,(int32_t)0x8a0bd3f5,(int32_t)0xce4ab5a2,
  (int32_t)0x89be50c3,(int32_t)0xcf043ab3,(int32_t)0x8971f15a,(int32_t)0xcfbe389f,
  (int32_t)0x8926b677,(int32_t)0xd078ad9e,(int32_t)0x88dca0d3,(int32_t)0xd13397e2,
  (int32_t)0x8893b125,(int32_t)0xd1eef59e,(int32_t)0x884be821,(int32_t)0xd2aac504,
  (int32_t)0x88054677,(int32_t)0xd3670446,(int32_t)0x87bfccd7,(int32_t)0xd423b191,
  (int32_t)0x877b7bec,(int32_t)0xd4e0cb15,(int32_t)0x8738545e,(int32_t)0xd59e4eff,
  (int32_t)0x86f656d3,(int32_t)0xd65c3b7b,(int32_t)0x86b583ee,(int32_t)0xd71a8eb5,
  (int32_t)0x8675dc4f,(int32_t)0xd7d946d8,(int32_t)0x86376092,(int32_t)0xd898620c,
  (int32_t)0x85fa1153,(int32_t)0xd957de7a,(int32_t)0x85bdef28,(int32_t)0xda17ba4a,
  (int32_t)0x8582faa5,(int32_t)0xdad7f3a2,(int32_t)0x8549345c,(int32_t)0xdb9888a8,
  (int32_t)0x85109cdd,(int32_t)0xdc597781,(int32_t)0x84d934b1,(int32_t)0xdd1abe51,
  (int32_t)0x84a2fc62,(int32_t)0xdddc5b3b,(int32_t)0x846df477,(int32_t)0xde9e4c60,
  (int32_t)0x843a1d70,(int32_t)0xdf608fe4,(int32_t)0x840777d0,(int32_t)0xe02323e5,
  (int32_t)0x83d60412,(int32_t)0xe0e60685,(int32_t)0x83a5c2b0,(int32_t)0xe1a935e2,
  (int32_t)0x8376b422,(int32_t)0xe26cb01b,(int32_t)0x8348d8dc,(int32_t)0xe330734d,
  (int32_t)0x831c314e,(int32_t)0xe3f47d96,(int32_t)0x82f0bde8,(int32_t)0xe4b8cd11,
  (int32_t)0x82c67f14,(int32_t)0xe57d5fda,(int32_t)0x829d753a,(int32_t)0xe642340d,
  (int32_t)0x8275a0c0,(int32_t)0xe70747c4,(int32_t)0x824f0208,(int32_t)0xe7cc9917,
  (int32_t)0x82299971,(int32_t)0xe8922622,(int32_t)0x82056758,(int32_t)0xe957ecfb,
  (int32_t)0x81e26c16,(int32_t)0xea1debbb,(int32_t)0x81c0a801,(int32_t)0xeae4207a,
  (int32_t)0x81a01b6d,(int32_t)0xebaa894f,(int32_t)0x8180c6a9,(int32_t)0xec71244f,
  (int32_t)0x8162aa04,(int32_t)0xed37ef91,(int32_t)0x8145c5c7,(int32_t)0xedfee92b,
  (int32_t)0x812a1a3a,(int32_t)0xeec60f31,(int32_t)0x810fa7a0,(int32_t)0xef8d5fb8,
  (int32_t)0x80f66e3c,(int32_t)0xf054d8d5,(int32_t)0x80de6e4c,(int32_t)0xf11c789a,
  (int32_t)0x80c7a80a,(int32_t)0xf1e43d1c,(int32_t)0x80b21baf,(int32_t)0xf2ac246e,
  (int32_t)0x809dc971,(int32_t)0xf3742ca2,(int32_t)0x808ab180,(int32_t)0xf43c53cb,
  (int32_t)0x8078d40d,(int32_t)0xf50497fb,(int32_t)0x80683143,(int32_t)0xf5ccf743,
  (int32_t)0x8058c94c,(int32_t)0xf6956fb7,(int32_t)0x804a9c4d,(int32_t)0xf75dff66,
  (int32_t)0x803daa6a,(int32_t)0xf826a462,(int32_t)0x8031f3c2,(int32_t)0xf8ef5cbb,
  (int32_t)0x80277872,(int32_t)0xf9b82684,(int32_t)0x801e3895,(int32_t)0xfa80ffcb,
  (int32_t)0x80163440,(int32_t)0xfb49e6a3,(int32_t)0x800f6b88,(int32_t)0xfc12d91a,
  (int32_t)0x8009de7e,(int32_t)0xfcdbd541,(int32_t)0x80058d2f,(int32_t)0xfda4d929,
  (int32_t)0x800277a6,(int32_t)0xfe6de2e0,(int32_t)0x80009dea,(int32_t)0xff36f078
};

static const fft_real_x32_descr_t fft_real_x32_descr =
{ 
  1024, fft_cplx_x32_twd_512, fft_real_x32_spc
};

const fft_handle_t h_fft_real_x32_1024  = (fft_handle_t)&fft_real_x32_descr;
const fft_handle_t h_ifft_real_x32_1024 = (fft_handle_t)&fft_real_x32_descr;
