/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2018. All rights reserved.
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
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/


#include "afe_drv_pcm.h"
#include "afe_drv_reg_rw.h"
#include "audio_rtos_header_group.h"
#include "audio_drv_log.h"

//#define AFE_REG_LOG     // Enable AFE register log

#define MAX_REGMAP_AMOUNT 8
static void *g_ioremap_base_addr[MAX_REGMAP_AMOUNT];
static uint32_t g_ioremap_size[MAX_REGMAP_AMOUNT];
static int g_regmap_amount;

void aud_drv_regmap_set_size(int size)
{
    configASSERT(MAX_REGMAP_AMOUNT >= size);
    g_regmap_amount = size;
}

void aud_drv_regmap_set_addr_range(int idx, void *ioremap_reg, uint32_t size)
{
    configASSERT(g_regmap_amount >= idx);
    g_ioremap_base_addr[idx] = ioremap_reg;
    g_ioremap_size[idx] = size;
}


uint32_t aud_drv_get_reg(int idx, uintptr_t addr_offset)
{
    volatile uint32_t *addr_ptr = (volatile uint32_t *)(addr_offset + g_ioremap_base_addr[idx]);
    configASSERT(addr_offset <= g_ioremap_size[idx]);

    //PRINTF_D("%s, addr 0x%lx, val 0x%x\n", __func__, addr_offset, *addr_ptr);
    return *addr_ptr;
}

void aud_drv_set_reg_addr_val(int idx, uintptr_t addr_offset, uint32_t val)
{
    volatile uint32_t *addr_ptr = (volatile uint32_t *)(addr_offset + g_ioremap_base_addr[idx]);

    configASSERT(addr_offset <= g_ioremap_size[idx]);
#if defined(AFE_REG_LOG)
    AUD_DRV_LOG_D("%s, addr_offset 0x%x, val 0x%x\n", __func__, addr_offset, val);
#endif
    *addr_ptr = val;
#if defined(AFE_REG_LOG)
    AUD_DRV_LOG_D("%s, after write, we get 0x%x\n", __func__, aud_drv_get_reg(idx,addr_offset));
#endif
}

void aud_drv_set_reg_addr_val_mask(int idx, uintptr_t addr_offset,
                   uint32_t val, uint32_t mask)
{
    volatile uint32_t val_orig = aud_drv_get_reg(idx, addr_offset);
    volatile uint32_t val_to_write = (val_orig & (~mask)) | (val & mask);
    volatile uint32_t *addr_ptr = (volatile uint32_t *)(addr_offset + g_ioremap_base_addr[idx]);

    configASSERT(addr_offset <= g_ioremap_size[idx]);
#if defined(AFE_REG_LOG)
    AUD_DRV_LOG_D("%s, addr_offset 0x%x, val 0x%x, mask 0x%x, val_orig 0x%x, val_to_write 0x%x ,idx=%d ,addr=%p\n",
                  __func__, addr_offset, val, mask, val_orig, val_to_write,idx,addr_ptr);
#endif
    *addr_ptr = val_to_write;
#if defined(AFE_REG_LOG)
    AUD_DRV_LOG_D("%s, after write, we get 0x%x\n", __func__, aud_drv_get_reg(idx,addr_offset));
#endif
}

void aud_drv_set_reg_check_addr_val(int idx, int32_t addr_offset, uint32_t val) {
    if (addr_offset < 0) return;
    aud_drv_set_reg_addr_val(idx, (uintptr_t)addr_offset, val);
}

void aud_drv_set_reg_check_addr_val_mask(int idx, int32_t addr_offset,
                     uint32_t val, uint32_t mask) {
    if (addr_offset < 0) return;
    aud_drv_set_reg_addr_val_mask(idx, (uintptr_t)addr_offset, val, mask);
}
/* reg R/W end */

