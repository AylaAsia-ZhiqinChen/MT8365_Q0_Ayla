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
#include "ww_dummy.h"

struct ww_dummy {
    int ww_flag;
    int rate;
    int chnum;
    int bitwidth;
    long long cur_pos;
    ww_dummy_cb_t cb;
    void *user_data;
};

static struct ww_dummy g_ww_handle;
static const char *ww_wakeword = "Hi dummy";

int ww_dummy_init(ww_dummy_handle_t **handle, int rate, int chnum, int bitwidth)
{
    struct ww_dummy *ww = &g_ww_handle;
    ww->ww_flag = 0;
    ww->rate = rate;
    ww->chnum = chnum;
    ww->bitwidth = bitwidth;
    ww->cur_pos = 0;
    *handle = (ww_dummy_handle_t *)ww;
    return 0;
}

int ww_dummy_uninit(ww_dummy_handle_t *handle)
{
    return 0;
}

int ww_dummy_set_flag(ww_dummy_handle_t *handle, int flag)
{
    struct ww_dummy *ww = (struct ww_dummy *)handle;
    ww->ww_flag = flag;
    return 0;
}

int ww_dummy_set_cb(ww_dummy_handle_t *handle, ww_dummy_cb_t cb, void *user)
{
    struct ww_dummy *ww = (struct ww_dummy *)handle;
    ww->cb = cb;
    ww->user_data = user;
    return 0;
}

int ww_dummy_process(ww_dummy_handle_t *handle, char *buf, int frames)
{
    struct ww_dummy *ww = (struct ww_dummy *)handle;
    int mcps_proc;
    int mcps_start;
    int mcps_stop;

    mcps_proc = (int)((long long)frames * WW_DUMMY_MCPS / (long long)ww->rate);
    mcps_stop = mcps_start = XT_RSR_CCOUNT();
    while (mcps_stop - mcps_start < mcps_proc) {
        if (ww->ww_flag == WW_DUMMY_FORCE_OK) {
            ww->cb(ww->user_data, 950,  ww->cur_pos - (long long)(ww->rate), ww->cur_pos, ww_wakeword);
            ww->ww_flag = WW_DUMMY_FORCE_NG;
            break;
        }
        mcps_stop = XT_RSR_CCOUNT();
    }
    ww->cur_pos += (long long)frames;
    return 0;
}
