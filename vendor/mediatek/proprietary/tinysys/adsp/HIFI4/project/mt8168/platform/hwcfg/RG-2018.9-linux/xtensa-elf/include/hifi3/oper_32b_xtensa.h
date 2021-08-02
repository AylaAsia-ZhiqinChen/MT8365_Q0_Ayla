/* Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
 *
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any
 * adapted or modified version may be disclosed or distributed to
 * third parties in any manner, medium, or form, in whole or in part,
 * without the prior written consent of Tensilica Inc.
 *
 * This software and its derivatives are to be executed solely on
 * products incorporating a Tensilica processor.
 */
#ifndef __OPER_32B_XTENSA__H
#define __OPER_32B_XTENSA__H
#include "basic_op_xtensa.h"

static __inline__ void
L_Extract (int32_t L_32, int16_t *hi, int16_t *lo)
{
  ae_int32 v = L_32;
  *hi = (int16_t)AE_TRUNC16X4F32_scalar(v);
  *lo = (int16_t)(ae_int16)(ae_int32)AE_SRLI_32(AE_SLLI_32(v, 16), 17);
}

static __inline__ int32_t
L_Comp (int16_t hi, int16_t lo)
{
  ae_int16 h = (ae_int16)hi;
  ae_int32 L_32 = AE_CVT32X2F16_10_scalar(h);
  ae_f16 l = (ae_int16)lo;
  ae_f16 one = (ae_int16)(int16_t) 1;
  AE_MULAF16SS_00_scalar(L_32, l, one);
  return L_32;
}


static __inline__ int32_t Mpy_32 (int16_t hi1, int16_t lo1, int16_t hi2, int16_t lo2)
{
    int L_32;

    L_32 = L_mult (hi1, hi2);
    short m1 = mult(hi1, lo2);
    short m2 = mult(lo1, hi2);
    L_32 = L_mac (L_32, m1 , 1);
    L_32 = L_mac (L_32, m2 , 1);

    return (L_32);

}



static __inline__ int32_t Mpy_32_16 (int16_t hi, int16_t lo, int16_t n)
{
  ae_int32 L_32;
  ae_int16 hs = hi;
  ae_int16 ls = lo;
  ae_int16 ns = n;

  ae_f16x4 a1 = AE_SEL16I(hs, ls, 8);
  ae_f16x4 a2 = AE_SEL16I((short) 1,ns, 8);

  ae_f16x4 a3 = AE_MULFP16X4S(a1, a2);
  L_32 = AE_MULF16SS_30_scalar(a1, a2);
  AE_MULAF16SS_30_scalar(L_32, a2, a3);
  return L_32;
}


static __inline__ int32_t Div_32 (int32_t L_num, int16_t denom_hi, int16_t denom_lo)
{
    int16_t approx, hi, lo, n_hi, n_lo;
    int32_t L_32;

    /* First approximation: 1 / L_denom = 1/denom_hi */

    approx = div_s ((int16_t) 0x3fff, denom_hi);

    /* 1/L_denom = approx * (2.0 - L_denom * approx) */

    L_32 = Mpy_32_16 (denom_hi, denom_lo, approx);

    L_32 = L_sub ((int32_t) 0x7fffffffL, L_32);

    L_Extract (L_32, &hi, &lo);

    L_32 = Mpy_32_16 (hi, lo, approx);

    /* L_num * (1/L_denom) */

    L_Extract (L_32, &hi, &lo);
    L_Extract (L_num, &n_hi, &n_lo);
    L_32 = Mpy_32 (n_hi, n_lo, hi, lo);
    L_32 = L_shl (L_32, 2);

    return (L_32);
}



#endif
