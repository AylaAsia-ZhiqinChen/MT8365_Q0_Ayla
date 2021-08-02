/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <string.h>
#include "main.h"
#include "logger.h"

struct log_ctrl
{
    uint32_t magic;
    uint32_t start;
    uint32_t size;
    uint32_t offset;
    int32_t full;
};

static struct log_ctrl *g_log_ctrl;
#define LOG_CTRL_MAGIC      0x67676f6c

void logger_puts(const char *str, uint32_t length)
{
    uint32_t left_size;

    if (str == NULL || length == 0)
        return;

    if (g_log_ctrl->magic != LOG_CTRL_MAGIC
        || g_log_ctrl->size == 0
        || g_log_ctrl->offset > g_log_ctrl->size)
        return;

    if (length >= g_log_ctrl->size)
    {
        memcpy((void*)g_log_ctrl->start, str + (length - g_log_ctrl->size),
            g_log_ctrl->size);
        g_log_ctrl->offset = 0;
        g_log_ctrl->full = 1;
    }
    else if (g_log_ctrl->size - g_log_ctrl->offset >= length)
    {
        memcpy((void *)(g_log_ctrl->start + g_log_ctrl->offset), str, length);
        g_log_ctrl->offset += length;
        if (g_log_ctrl->offset == g_log_ctrl->size)
        {
            g_log_ctrl->offset = 0;
            g_log_ctrl->full = 1;
        }
    }
    else
    {
        left_size = g_log_ctrl->size - g_log_ctrl->offset;
        memcpy((void *)(g_log_ctrl->start + g_log_ctrl->offset),
            str, left_size);
        memcpy((void *)g_log_ctrl->start,
            str + left_size, length - left_size);
        g_log_ctrl->offset = length - left_size;
        g_log_ctrl->full = 1;
    }
}

void logger_init(void)
{
    g_log_ctrl = (struct log_ctrl *)ADSP_LOGGER_BUF_ADDR;
    g_log_ctrl->magic  = LOG_CTRL_MAGIC;
    g_log_ctrl->start  = ADSP_LOGGER_BUF_ADDR + sizeof(struct log_ctrl);
    g_log_ctrl->size   = ADSP_LOGGER_BUF_SIZE - sizeof(struct log_ctrl);
    g_log_ctrl->offset = 0;
    g_log_ctrl->full   = 0;
}
