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
 * Real Matrix Inversion
 * Optimized code for HiFi4
 */

/* Cross-platform data type definitions. */
#include "NatureDSP_types.h"
/* Common helper macros. */
#include "common.h"
/* Basic operations for the reference code. */
#include "baseop.h"
/* Matrix functions */
#include "NatureDSP_Signal.h"
#if !(XCHAL_HAVE_HIFI4_VFPU)
DISCARD_FUN(void,mtx_inv2x2f,(float32_t* x))
#else
/*-------------------------------------------------------------------------
  These functions implement in-place matrix inversion by Gauss elimination 
  with full pivoting

  Precision: 
  f     floating point

  Input:
  x[N*N]      input matrix
  Output:
  x[N*N]      result
  N is 2,3 or 4

  Restrictions:
  none
-------------------------------------------------------------------------*/
void mtx_inv2x2f(float32_t* x)
{
  xtfloatx2 *px;
  xtfloatx2 db, ca;
  xtfloatx2 a, b, c, d, r;
  ae_valign al_px;

  /* Load matrix */
  px = (xtfloatx2 *)x;
  a = XT_LSI((xtfloat *)px, 0*sizeof(float32_t));
  b = XT_LSI((xtfloat *)px, 1*sizeof(float32_t));
  c = XT_LSI((xtfloat *)px, 2*sizeof(float32_t));
  d = XT_LSI((xtfloat *)px, 3*sizeof(float32_t));

  /* Find the determinant and its reciprocal */
  r = a*d;
  XT_MSUB_SX2(r, b, c);
  r=XT_RECIP_SX2(r);
  /* Calculate matrix inversion */
  db = XT_SEL32_LL_SX2( d, -b);
  ca = XT_SEL32_HH_SX2(-c,  a);
  db = db*r;
  ca = ca*r;

  /* Save inverse matrix */
  px = (xtfloatx2 *)x;
  al_px = AE_ZALIGN64();
  XT_SASX2IP(db, al_px, px);
  XT_SASX2IP(ca, al_px, px);
  XT_SASX2POSFP(al_px, px);
}/* mtx_inv2x2f() */
#endif
