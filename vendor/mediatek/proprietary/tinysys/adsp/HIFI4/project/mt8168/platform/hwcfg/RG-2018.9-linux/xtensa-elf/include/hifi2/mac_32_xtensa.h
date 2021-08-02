/* Customer ID=13943; Build=0x75f5e; Copyright (c) 2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
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
#ifndef __MAC_32_XTENSA__H
#define __MAC_32_XTENSA__H
#include "basic_op_xtensa.h"

static __inline__  int32_t Mac_32 (int32_t L_32, int16_t hi1, int16_t lo1, int16_t hi2, int16_t lo2)
{
    L_32 = L_mac (L_32, hi1, hi2);
    L_32 = L_mac (L_32, mult (hi1, lo2), 1);
    L_32 = L_mac (L_32, mult (lo1, hi2), 1);

    return (L_32);
}
static __inline__  int32_t Mac_32_16 (int32_t L_32, int16_t hi, int16_t lo, int16_t n)
{
    L_32 = L_mac (L_32, hi, n);
    L_32 = L_mac (L_32, mult (lo, n), 1);

    return (L_32);
}

#endif

