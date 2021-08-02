/* MediaTek Inc. (C) 2010. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver's
* applicable license agreements with MediaTek Inc.
*/


#ifndef _MTK_DEVICE_APC_H
#define _MTK_DEVICE_APC_H

#include "typedefs.h"

#define DEVAPC_AO_INFRA_BASE    0x1000E000  // for INFRA/PERI
#define DEVAPC_AO_MM_BASE       0x1001C000  // for MM


/*******************************************************************************
 * REGISTER ADDRESS DEFINATION
 ******************************************************************************/
#define DEVAPC0_MAS_DOM_0           ((volatile unsigned int*)(DEVAPC_AO_INFRA_BASE+0x0A00))
#define DEVAPC0_MAS_DOM_1           ((volatile unsigned int*)(DEVAPC_AO_INFRA_BASE+0x0A04))
#define DEVAPC0_MAS_DOM_2           ((volatile unsigned int*)(DEVAPC_AO_INFRA_BASE+0x0A08))
#define DEVAPC0_MAS_DOM_3           ((volatile unsigned int*)(DEVAPC_AO_INFRA_BASE+0x0A0C))
#define DEVAPC0_MAS_SEC             ((volatile unsigned int*)(DEVAPC_AO_INFRA_BASE+0x0B00))
#define DEVAPC_AO_INFRA_APC_CON     ((volatile unsigned int*)(DEVAPC_AO_INFRA_BASE+0x0F00))
#define DEVAPC_AO_MM_APC_CON        ((volatile unsigned int*)(DEVAPC_AO_MM_BASE+0x0F00))

#define MOD_NO_IN_1_DEVAPC          16
//#define MOD_INFRA_MCU_BIU_CONF      26
#define DEVAPC_APC_CON_CTRL         (0x1 << 0)
#define DEVAPC_APC_CON_EN           0x1

/* device apc attribute */
typedef enum {
    E_L0=0,
    E_L1,
    E_L2,
    E_L3,
    E_MAX_APC_ATTR
}APC_ATTR;

/* domain index */
typedef enum {
	E_DOMAIN_0 = 0,
	E_DOMAIN_1,
	E_DOMAIN_2,
	E_DOMAIN_3,
	E_DOMAIN_4,
	E_DOMAIN_5,
	E_DOMAIN_6,
	E_DOMAIN_7
} E_MASK_DOM;

/* domain index */
typedef enum {
    NON_SECURE_TRAN = 0,
    SECURE_TRAN ,
}E_TRANSACTION;

/* DOMAIN_SETUP */
#define DOMAIN_0     0
#define DOMAIN_1     1
#define DOMAIN_2     2
#define DOMAIN_3     3
#define DOMAIN_4     4
#define DOMAIN_5     5
#define DOMAIN_6     6
#define DOMAIN_7     7
#define DOMAIN_8     8
#define DOMAIN_9     9
#define DOMAIN_10    10
#define DOMAIN_11    11
#define DOMAIN_12    12
#define DOMAIN_13    13
#define DOMAIN_14    14
#define DOMAIN_15    15

/* Masks for Domain Control for DEVAPC3 */
#define MASTER_MSDC0    5

static inline unsigned int uffs(unsigned int x)
{
    unsigned int r = 1;

    if (!x)
        return 0;
    if (!(x & 0xffff)) {
        x >>= 16;
        r += 16;
    }
    if (!(x & 0xff)) {
        x >>= 8;
        r += 8;
    }
    if (!(x & 0xf)) {
        x >>= 4;
        r += 4;
    }
    if (!(x & 3)) {
        x >>= 2;
        r += 2;
    }
    if (!(x & 1)) {
        x >>= 1;
        r += 1;
    }
    return r;
}

#define reg_read16(reg)          __raw_readw(reg)
#define reg_read32(reg)          __raw_readl(reg)
#define reg_write16(reg,val)     __raw_writew(val,reg)
#define reg_write32(reg,val)     __raw_writel(val,reg)

#define reg_set_bits(reg,bs)     ((*(volatile u32*)(reg)) |= (u32)(bs))
#define reg_clr_bits(reg,bs)     ((*(volatile u32*)(reg)) &= ~((u32)(bs)))

#define reg_set_field(reg,field,val) \
     do {    \
         volatile unsigned int tv = reg_read32(reg); \
         tv &= ~(field); \
         tv |= ((val) << (uffs((unsigned int)field) - 1)); \
         reg_write32(reg,tv); \
     } while(0)

#define reg_get_field(reg,field,val) \
     do {    \
         volatile unsigned int tv = reg_read32(reg); \
         val = ((tv & (field)) >> (uffs((unsigned int)field) - 1)); \
     } while(0)

#endif
