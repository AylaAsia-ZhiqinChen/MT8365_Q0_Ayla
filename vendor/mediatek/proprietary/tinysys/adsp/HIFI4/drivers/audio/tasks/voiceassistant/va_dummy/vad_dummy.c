/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
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

#include <xtensa/tie/xt_timer.h>
#include "audio_rtos_header_group.h"
#include "vad_dummy.h"

#include "mt_printf.h"
#include "systimer.h"

struct vad_dummy {
    int vad_flag;
    int rate;
    int chnum;
    int bitwidth;
};

static struct vad_dummy g_vad_handle;

int vad_dummy_init(vad_dummy_handle_t **handle, int rate, int chnum, int bitwidth)
{
    struct vad_dummy *vad = &g_vad_handle;
    vad->vad_flag = 0;
    vad->rate = rate;
    vad->chnum = chnum;
    vad->bitwidth = bitwidth;
    *handle = (vad_dummy_handle_t *)vad;
    return 0;
}

int vad_dummy_uninit(vad_dummy_handle_t *handle)
{
    return 0;
}

int vad_dummy_set_flag(vad_dummy_handle_t *handle, int flag)
{
    struct vad_dummy *vad = (struct vad_dummy *)handle;
    vad->vad_flag = flag;
    PRINTF_E("%s vad_flag(%d)\n", __func__, flag);
    return 0;
}

int vad_dummy_process(vad_dummy_handle_t *handle, char *buf, int frames)
{
    struct vad_dummy *vad = (struct vad_dummy *)handle;
    int mcps_proc;
    int mcps_start;
    int mcps_stop;
    int vad_result = 0; /* 0 is NG */

    mcps_proc = (int)((long long)frames * VAD_DUMMY_MCPS / (long long)vad->rate);
    mcps_stop = mcps_start = XT_RSR_CCOUNT();

    while (mcps_stop - mcps_start < mcps_proc) {
        if (vad->vad_flag == VAD_DUMMY_FORCE_OK) {
            vad_result = 1;
            vad->vad_flag = VAD_DUMMY_FORCE_NG;
            break;
        }
        mcps_stop = XT_RSR_CCOUNT();
    }
    return vad_result;
}
