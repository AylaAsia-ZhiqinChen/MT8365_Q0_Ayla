/* Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2011 by Tensilica Inc.  ALL RIGHTS RESERVED.
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

#ifndef __BASIC_OP_XTENSA__H
#define __BASIC_OP_XTENSA__H

#include <xtensa/tie/xt_hifi2.h>
#include <stdint.h>

/* for xtensa architecture, overflow and carry are states */

#define SetOverflow(a) WUR_AE_OVERFLOW(a)
#define GetOverflow() RUR_AE_OVERFLOW()
#if 0
#define SetCarry(a) WUR_XD2_CARRY(a)
#define GetCarry() RUR_XD2_CARRY()
#endif


#ifndef MAX_32
#define MAX_32 (int32_t)0x7fffffffL
#endif /* MAX_32 */
#ifndef MIN_32
#define MIN_32 (int32_t)0x80000000L
#endif /* MIN_32 */

#ifndef MAX_16
#define MAX_16 (int16_t)0x7fff
#endif /* MAX_16 */
#ifndef MIN_16
#define MIN_16 (int16_t)0x8000
#endif /* MIN_16 */

static inline int16_t shl(int16_t var1, int16_t var2)
{
  return AE_SLAA16S_scalar(var1, var2);
}
static inline int16_t shr(int16_t var1, int16_t var2)
{
  return AE_SRAA16S_scalar(var1, var2);
}
static inline int32_t L_shl(int32_t L_var1, int32_t var2)
{
  return AE_SLAA32S_scalar(L_var1, var2);
}
static inline int32_t L_shr(int32_t L_var1, int32_t var2)
{
  return AE_SRAA32S_scalar(L_var1, var2);
}
static inline int16_t shr_r(int16_t var1, int16_t var2)
{
  return AE_SRAA16RS_scalar (var1, var2);
}
static inline int32_t L_shr_r(int32_t L_var1, int16_t var2)
{
  return AE_SRAA32RS_scalar(L_var1, var2);
}
static inline int16_t sature(int32_t L_var1)
{
  /*  Some versions of the ITU reference code resets the overflow unconditionally.
      Define EMULATE_BIT_EXACT to emulate that behavior.  */ 
#ifdef EMULATE_BIT_EXACT
  SetOverflow(0);
#endif /* EMULATE_BIT_EXACT */
  return AE_SAT16X4_scalar(L_var1);
}
static inline int16_t saturate(int32_t L_var1)
{
  return AE_SAT16X4_scalar(L_var1);
}
static inline int16_t add(int16_t var1, int16_t var2)
{
  return AE_ADD16S_scalar(var1, var2);
}
static inline int16_t sub(int16_t var1, int16_t var2)
{
  return AE_SUB16S_scalar(var1, var2);
}
static inline int16_t abs_s(int16_t var1)
{
  return AE_ABS16S_scalar(var1);
}
static inline int16_t mult(int16_t var1, int16_t var2)
{
  return AE_MULFP16X4S_scalar(var1, var2);
}
static inline int32_t L_mult(int16_t L_var1, int16_t var2)
{
  return AE_MULF16X4SS_scalar(L_var1, var2);
}
static inline int16_t negate(int16_t var1)
{
  return AE_NEG16S_scalar(var1);
}
static inline int16_t extract_h(int32_t L_var1)
{
  return AE_TRUNC16X4F32_scalar(L_var1);
}
static inline int16_t extract_l(int32_t L_var1)
{
  return AE_CVT16X4_scalar(L_var1);
}
static inline int32_t L_mac(int32_t L_var3, int16_t var1, int16_t var2)
{
  return AE_MULAF16X4SS_scalar(L_var3, var1, var2);
}
static inline int32_t L_msu(int32_t L_var3, int16_t var1, int16_t var2)
{
  return AE_MULSF16X4SS_scalar(L_var3, var1, var2);
}
static inline int32_t L_add(int32_t L_var1, int32_t var2)
{
   return AE_ADD32S_scalar(L_var1, var2);
}
#if !NO_ETSI_ROUND
static inline int16_t round(int32_t L_var1)
{
  return AE_ROUND16X4F32SASYM_scalar(L_var1);
}
#endif
static inline int16_t round32(int32_t L_var1)
{
  return AE_ROUND16X4F32SASYM_scalar(L_var1);
}
static inline int16_t etsiopround(int32_t L_var1)
{
  return AE_ROUND16X4F32SASYM_scalar(L_var1);
}
static inline int32_t L_sub(int32_t L_var1, int32_t var2)
{
   return AE_SUB32S_scalar(L_var1, var2);
}
static inline int32_t L_negate(int32_t L_var1)
{
  return AE_NEG32S_scalar(L_var1);
}
static inline int16_t mult_r(int16_t var1, int16_t var2)
{
  return AE_MULFP16X4RAS_scalar(var1, var2);
}
static inline int16_t mac_r(int32_t L_var3, int16_t var1, int16_t var2)
{
  ae_int32 tmp = L_var3;
  tmp = AE_MULAF16X4SS_scalar(tmp, var1, var2);
  return AE_ROUND16X4F32SASYM_scalar(tmp);;
}
static inline int16_t msu_r(int32_t L_var3, int16_t var1, int16_t var2)
{
  ae_int32 tmp = L_var3;
  tmp = AE_MULSF16X4SS_scalar(tmp, var1, var2);
  return AE_ROUND16X4F32SASYM_scalar(tmp);;
}
static inline int32_t L_deposit_h(int16_t var1)
{
  return AE_CVT32X2F16_10_scalar(var1);
}
static inline int32_t L_deposit_l(int16_t var1)
{
  return AE_SEXT32X2D16_10_scalar(var1);
}
static inline int32_t L_abs(int32_t L_var1)
{
  return AE_ABS32S_scalar(L_var1);
}
static inline int16_t norm_s(int16_t var1)
{
  return AE_NSAZ16_0_scalar(var1);
}
static inline int16_t norm_l(int32_t L_var1)
{
  return AE_NSAZ32_L(L_var1);
}
static inline int16_t div_s(int16_t var1, int16_t var2)
{
  /* standard code exits for these 2 conditions */
  if (var2 == 0) return 0;
  if ((var1 >  var2) ||  (var1 <  0) ||  (var2 <  0)) return 0;
  if (var1 == var2) return (int16_t)0x7fff;


#if XCHAL_HAVE_DIV32
  {
    int v1 = 0x8000 * var1;
    return v1/var2;
  }
#else /* not XCHAL_HAVE_DIV32 */
  {
       int16_t var_out;

        {
                ae_int64 d_num_result;
                ae_int32x2 d_tmp, d_denom;

                d_denom = AE_MOVDA32X2(var2, var2);
                d_tmp = AE_MOVDA32X2(var1, 0);
                d_num_result = AE_MOVINT64_FROMINT32X2(d_tmp);

                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);

                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);

                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);

                d_tmp = AE_MOVINT32X2_FROMINT64(d_num_result);
                var_out = AE_MOVAD32_L(d_tmp);
                return (int16_t)var_out;

        }

  }
#endif /* not XCHAL_HAVE_DIV32 */
}

static inline int16_t i_mult (int16_t var1, int16_t var2)

{
#ifdef ORIGINAL_G7231
  return var1 * var2;
#else
  return saturate(var1 * var2);
#endif
}

static inline int16_t s_max(int16_t var1, int16_t var2)
{
        return var1 > var2 ? var1 : var2;
}
static inline int16_t s_min(int16_t var1, int16_t var2)
{
        return var1 < var2 ? var1 : var2;
}
static inline int16_t lshl(int16_t var1, int16_t var2)
{
        return AE_SRLA16_scalar(var1, -var2);
}
static inline int16_t lshr(int16_t var1, int16_t var2)
{
        return AE_SRLA16_scalar(var1, var2);
}
static inline int32_t L_mult0(int16_t var1, int16_t var2)
{
        return var1*var2;
}
static inline int32_t L_mac0(int32_t L_var3, int16_t var1, int16_t var2)
{
        L_var3 = AE_MULA16S_scalar(L_var3, var1, var2);
        return  L_var3;
}
static inline int32_t L_msu0(int32_t L_var3, int16_t var1, int16_t var2)
{
        L_var3 = AE_MULS16S_scalar(L_var3, var1, var2);
        return L_var3;
}
static inline int32_t L_max(int32_t L_var1, int32_t L_var2)
{
        return L_var1 > L_var2 ? L_var1 : L_var2;
}
static inline int32_t L_min(int32_t L_var1, int32_t L_var2)
{
        return L_var1 < L_var2 ? L_var1 : L_var2;
}
static inline int32_t L_lshl(int32_t L_var1, int32_t var2)
{
        return AE_SRLA32_scalar(L_var1, -var2);
}
static inline int32_t L_lshr(int32_t L_var1, int32_t var2)
{
        return AE_SRLA32_scalar(L_var1, var2);
}
static inline int32_t L_shl_r(int32_t L_var1, int16_t var2)
{
        return AE_SRAA32RS_scalar(L_var1, -var2);
}
static inline int16_t shl_r(int16_t L_var1, int16_t var2)
{
        return AE_SRAA16RS_scalar(L_var1, -var2);
}
static inline int16_t s_and(int16_t var1, int16_t var2)
{
        return AE_INT16_AND_INT16(var1, var2);
}
static inline int16_t s_or(int16_t var1, int16_t var2)
{
        return AE_INT16_OR_INT16(var1, var2);
}
static inline int16_t s_xor(int16_t var1, int16_t var2)
{
        return AE_INT16_XOR_INT16(var1, var2);
}
static inline int32_t L_and(int32_t L_var1, int32_t L_var2)
{
        return AE_INT32_AND_INT32(L_var1, L_var2);
}
static inline int32_t L_or(int32_t L_var1, int32_t L_var2)
{
        return AE_INT32_OR_INT32(L_var1, L_var2);
}
static inline int32_t L_xor(int32_t L_var1, int32_t L_var2)
{
        return AE_INT32_XOR_INT32(L_var1, L_var2);
}
static inline void Mpy_32_16_ss(int32_t L_var1, int16_t var2, int32_t *L_varout_h, uint16_t *varout_l)
{
        ae_int64 d_out;
        ae_int32x2 d_var1;
        ae_int16x4 d_var2;
        d_var1 = AE_MOVDA32X2(L_var1, L_var1);
        d_var2 = AE_MOVDA16(var2);
        d_out = AE_MULF32X16_L0(d_var1, d_var2);
        d_out = AE_SAT48S(d_out);
        *(ae_int16 *)varout_l = AE_MOVINT16X4_FROMINT64(d_out);
        AE_S32M_I(d_out, (ae_q32s *)L_varout_h, 0);
        return;
}
static inline void Mpy_32_32_ss(int32_t L_var1, int32_t L_var2, int32_t *L_varout_h, uint32_t *L_varout_l)
{
        ae_int64 d_out;
        ae_int32x2 d_var1, d_var2;
        d_var1 = AE_MOVDA32X2(L_var1, L_var1);
        d_var2 = AE_MOVDA32X2(L_var2, L_var2);
        d_out = AE_MULF32S_LL(d_var1, d_var2);
        *(ae_int32 *)L_varout_l = AE_MOVINT32X2_FROMINT64(d_out);
        *(ae_int32 *)L_varout_h = AE_SEL32_HH(AE_MOVINT32X2_FROMINT64(d_out), AE_MOVINT32X2_FROMINT64(d_out));
        return;
}
static inline int16_t div_l(int32_t  L_num, int16_t den)
{
        int32_t   var_out = 0;
        int32_t   L_den;

        L_den = (int32_t)den << 16;

        if (L_den == 0)
        {
                /* printf("Division by 0 in div_l, Fatal error in "); printStack(); */
                return MAX_16;
        }

        if ((L_num < 0) || (L_den < 0))
        {
                /* printf("Division Error in div_l, Fatal error in "); printStack(); */
                return 0;
        }

        if (L_num >= L_den)
        {
                return MAX_16;
        }
        else
        {
                ae_int64 d_num_result;
                ae_int32x2 d_tmp, d_denom;

                L_num = L_num >> 1;
                L_den = L_den >> 1;

                d_denom = AE_MOVDA32X2(L_den, L_den);

                d_tmp = AE_MOVDA32X2(L_num, 0);
                d_num_result = AE_MOVINT64_FROMINT32X2(d_tmp);

                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);

                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);

                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);
                AE_DIV64D32_L(d_num_result, d_denom);

                d_tmp = AE_MOVINT32X2_FROMINT64(d_num_result);
                var_out = AE_MOVAD32_L(d_tmp);

                return (int16_t)var_out;
        }
}

#endif /* __BASIC_OP_XTENSA__H */

