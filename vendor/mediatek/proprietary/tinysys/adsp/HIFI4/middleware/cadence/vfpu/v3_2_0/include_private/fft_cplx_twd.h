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
    Twiddle factor tables and descriptor structures for complex-valued FFTs
	IntegrIT, 2006-2014
*/

#ifndef __FFT_CPLX_TWD_H
#define __FFT_CPLX_TWD_H

/* Datatypes and helper definitions. */
#include "common.h"

/*
 * 16-bit twiddle factor tables.
 */

extern const int16_t fft_cplx_x16_twd_16  [];
extern const int16_t fft_cplx_x16_twd_32  [];
extern const int16_t fft_cplx_x16_twd_64  [];
extern const int16_t fft_cplx_x16_twd_128 [];
extern const int16_t fft_cplx_x16_twd_256 [];
extern const int16_t fft_cplx_x16_twd_512 [];
extern const int16_t fft_cplx_x16_twd_1024[];
extern const int16_t fft_cplx_x16_twd_2048[];
extern const int16_t fft_cplx_x16_twd_4096[];

/*
 * 16x16 and 32x16-bit complex-valued FFT descriptor structure.
 */

typedef struct
{
    int        N;
    cint16_ptr twd;

} fft_cplx_x16_descr_t;

/*
 * 32-bit twiddle factor tables.
 */

extern const int32_t fft_cplx_x32_twd_16  [];
extern const int32_t fft_cplx_x32_twd_32  [];
extern const int32_t fft_cplx_x32_twd_64  [];
extern const int32_t fft_cplx_x32_twd_128 [];
extern const int32_t fft_cplx_x32_twd_256 [];
extern const int32_t fft_cplx_x32_twd_512 [];
extern const int32_t fft_cplx_x32_twd_1024[];
extern const int32_t fft_cplx_x32_twd_2048[];
extern const int32_t fft_cplx_x32_twd_4096[];

/*
 * 32x32-bit complex-valued FFT descriptor structure.
 */

typedef struct
{
    int        N;
    cint32_ptr twd;

} fft_cplx_x32_descr_t;

#endif // __FFT_CPLX_TWD_H
