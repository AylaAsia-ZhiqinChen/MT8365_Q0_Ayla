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
#include "aec_dummy.h"

struct aec_dummy {
    int in_chnum;
    int out_chnum;
    int bitwidth;
    int rate;
    int flag;
};

static struct aec_dummy g_aec_handle;

int aec_dummy_init(aec_dummy_handle_t **handle, struct aec_dummy_setting setting)
{
    struct aec_dummy *aec = &g_aec_handle;

    aec->in_chnum = setting.in_chnum;
    aec->out_chnum = setting.out_chnum;
    aec->bitwidth = setting.bitwidth;
    aec->rate = setting.rate;

    *handle = (aec_dummy_handle_t *)aec;
    return 0;
}

int aec_dummy_uninit(aec_dummy_handle_t *handle)
{
    return 0;
}

int aec_dummy_process(aec_dummy_handle_t *handle, char *inbuf, char *outbuf, int frames, int *beamforming)
{
    struct aec_dummy *aec = (struct aec_dummy *)handle;
    int mcps_proc;
    int mcps_start;
    int mcps_stop;
    int i, j;
    static int rand = 0;

    mcps_proc = (int)((long long)frames * AEC_DUMMY_MCPS / (long long)aec->rate);
    mcps_stop = mcps_start = XT_RSR_CCOUNT();
    if (aec->bitwidth == 16) {
        int16_t *in = (int16_t *)inbuf;
        int16_t *out = (int16_t *)outbuf;

        for (i = 0; i < frames; i++) {
            for(j = 0; j < aec->out_chnum; j++)
                *(out + i * aec->out_chnum + j) = *(in + i * aec->in_chnum + j);
        }
    } else if (aec->bitwidth == 32) {
        int32_t *in = (int32_t *)inbuf;
        int32_t *out = (int32_t *)outbuf;

        for (i = 0; i < frames; i++) {
            for(j = 0; j < aec->out_chnum; j++)
                *(out + i * aec->out_chnum + j) = *(in + i * aec->in_chnum + j);
        }
    }

    while (mcps_stop - mcps_start < mcps_proc) {
        mcps_stop = XT_RSR_CCOUNT();
    }

    *beamforming = AEC_DUMMY_BEAMFORMINT + rand%10;
    if (rand++ > 20)
        rand = 0;

    return 0;
}
