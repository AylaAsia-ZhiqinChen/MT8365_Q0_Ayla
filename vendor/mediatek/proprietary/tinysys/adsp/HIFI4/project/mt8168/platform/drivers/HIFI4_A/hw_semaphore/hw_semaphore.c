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
 * THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
 * applicable license agreements with MediaTek Inc.
 */

#include <driver_api.h>
#include <hw_semaphore.h>
#include <mt_printf.h>
#include <mt_reg_base.h>


static unsigned int hw_sema_ref_count[ADSP_HW_SEMA_MAX] = {0};

/**
  * Get HW semaphore
  * @param sema_id: semaphore id
  * @param timeout: 0 - no timeout; other value -retry count
  * @return
  *     - HW_SEMA_OK: Success
  *     - HW_SEMA_HAVE_GOT: this HW semaphore has been held
  *     - HW_SEMA_INVALID: Invalid HW semaphore ID
  *     - HW_SEMA_TIMEOUT: timeout
  */
int hw_semaphore_get(ADSP_HW_SEMAPHORE_ID sema_id, unsigned int timeout)
{
    unsigned int sema_reg = DSP_RG_SEMAPHORE00 + (sema_id << 2);
    unsigned int try_cnt = 0;

    if (sema_id >= ADSP_HW_SEMA_MAX)
        return HW_SEMA_INVALID;

    if (DRV_Reg32(sema_reg) == 0x1)
    {
        hw_sema_ref_count[sema_id]++;
        return HW_SEMA_HAVE_GOT;
    }
    while (1)
    {
        DRV_WriteReg32(sema_reg, 0x1);
        if (DRV_Reg32(sema_reg) == 0x1)
        {
            hw_sema_ref_count[sema_id]++;
            return HW_SEMA_OK;
        }
        if (timeout > 0 && (try_cnt++ >= timeout))
            return HW_SEMA_TIMEOUT;
    }
}

/**
  * Release HW semaphore
  * @param sema_id: semaphore id
  * @return
  *     - HW_SEMA_OK: Success
  *     - HW_SEMA_INVAID: Invalid HW semaphore ID
  *     - HW_SEMA_HAVE_GOT: this HW semaphore has been held
  *     - HW_SEMA_FREE_ERR: Free Hw semaphore  error
  */
int hw_semaphore_release(ADSP_HW_SEMAPHORE_ID sema_id)
{
    unsigned int sema_reg = DSP_RG_SEMAPHORE00 + (sema_id << 2);

    if (sema_id >= ADSP_HW_SEMA_MAX)
        return HW_SEMA_INVALID;

    if (hw_sema_ref_count[sema_id] > 1)
    {
        hw_sema_ref_count[sema_id]--;
        return HW_SEMA_HAVE_GOT;
    }
    else
    {
        if (DRV_Reg32(sema_reg) == 0x1)
        {
            DRV_WriteReg32(sema_reg, 0x1);
            hw_sema_ref_count[sema_id] = 0;
            return HW_SEMA_OK;
        }
        else
        {
            return HW_SEMA_FREE_ERR;
        }
    }
}

