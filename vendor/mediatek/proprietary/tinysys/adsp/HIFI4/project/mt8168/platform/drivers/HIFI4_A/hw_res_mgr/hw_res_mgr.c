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
 */
#include "hw_res_mgr.h"
#include "audio_rtos_header_group.h"
#include "semphr.h"

#include "mt_printf.h"
#include "systimer.h"

struct hw_res_mgr {
    SemaphoreHandle_t sema;
    int16_t enable[DSP_HW_RES_NUM];
    int16_t request[DSP_HW_RES_NUM][DSP_HW_USER_NUM];
    int (*set[DSP_HW_RES_NUM])(int value);
};

struct hw_res_mgr g_hw_res_mgr;

NORMAL_SECTION_FUNC int dsp_hw_res_init(void)
{
    g_hw_res_mgr.sema = xSemaphoreCreateMutex();
    PRINTF_E("%s [time:%llu]\n", __func__, read_systimer_stamp_ns());
    return 0;
}

NORMAL_SECTION_FUNC int dsp_hw_res_uninit(void)
{
    vSemaphoreDelete(g_hw_res_mgr.sema);
    PRINTF_E("%s [time:%llu]\n", __func__, read_systimer_stamp_ns());
    return 0;
}

NORMAL_SECTION_FUNC int dsp_hw_res_register_ops(int type, dsp_hw_res_set ops)
{
    g_hw_res_mgr.set[type] = ops;
    return 0;
}

int dsp_hw_res_lock(void)
{
    xSemaphoreTake(g_hw_res_mgr.sema, portMAX_DELAY);
    return 0;
}

int dsp_hw_res_unlock(void)
{
    xSemaphoreGive(g_hw_res_mgr.sema);
    return 0;
}

int dsp_hw_res_get_setting(int type, int user)
{
    return (int)g_hw_res_mgr.request[type][user];
}

static int16_t dsp_hw_res_highest_request(int type)
{
    int i;
    int16_t highest;

    highest = g_hw_res_mgr.request[type][0];
    for (i = 0; i < DSP_HW_USER_NUM; i++) {
        if (g_hw_res_mgr.request[type][i] > highest)
            highest = g_hw_res_mgr.request[type][i];
    }
    PRINTF_E("hw_mgr highest:%d\n", highest);
    return highest;
}

int dsp_hw_res_enable(int type, int enable)
{
    int16_t highest;

    if (g_hw_res_mgr.enable[type] == (int16_t)enable)
        return 0;

    g_hw_res_mgr.enable[type] = (int16_t)enable;

    PRINTF_E("hw_mgr [time:%llu], type:%d, en:%d\n", read_systimer_stamp_ns(), type, enable);

    /* only enable is 1, need to force update hardware */
    if (enable == 0)
        return 0;

    /* force update hw */
    highest = dsp_hw_res_highest_request(type);

    g_hw_res_mgr.set[type](highest);

    return 0;
}

int dsp_hw_res_get_enable(int type)
{
    return g_hw_res_mgr.enable[type];
}

int dsp_hw_res_request(int type, int user, int request)
{
    int16_t highest;

    if (g_hw_res_mgr.request[type][user] == (int16_t)request)
        return 0;

    g_hw_res_mgr.request[type][user] = (int16_t)request;
    PRINTF_E("hw_mgr [time:%llu], type:%d, user:%d, req:%d\n", read_systimer_stamp_ns(), type, user, request);
    if (g_hw_res_mgr.enable[type] == 0)
        return 0;

    /* force update hw */
    highest = dsp_hw_res_highest_request(type);

    g_hw_res_mgr.set[type](highest);
    return 0;
}

