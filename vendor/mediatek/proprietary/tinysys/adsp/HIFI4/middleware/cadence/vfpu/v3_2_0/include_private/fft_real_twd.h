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
    Descriptor structures for real-valued FFTs
	IntegrIT, 2006-2014
*/

#ifndef __FFT_REAL_TWD_H
#define __FFT_REAL_TWD_H

/* Datatypes and helper definitions. */
#include "common.h"

// 16x16 and 32x16-bit real-valued FFT descriptor structure.
typedef struct
{
    int        N;   // Transform size, in real samples
    cint16_ptr twd; // Twiddle factor table for complex-valued FFT of size N/2
    cint16_ptr spc; // Twiddle factor table for spectrum conversion

} fft_real_x16_descr_t;

// 32x32-bit real-valued FFT descriptor structure.
typedef struct
{
    int        N;   // Transform size, in real samples
    cint32_ptr twd; // Twiddle factor table for complex-valued FFT of size N/2
    cint32_ptr spc; // Twiddle factor table for spectrum conversion

} fft_real_x32_descr_t;

#endif // __FFT_REAL_TWD_H
