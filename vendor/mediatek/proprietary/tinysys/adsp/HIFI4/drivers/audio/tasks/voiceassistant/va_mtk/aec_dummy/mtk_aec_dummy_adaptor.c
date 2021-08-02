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

#include "mt_printf.h"
#include "systimer.h"
#include "aec_dummy.h"
#include "preproc_process.h"

struct aec_private {
    uint8_t in_chnum;
    uint8_t out_chnum;
    uint8_t mics_chnum;
    uint8_t refs_chnum;
    int    beamforming;
};

struct aec_private g_aec_private;

static int  aec_dummy_adaptor_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    aec_dummy_handle_t *handle;
    struct aec_dummy_setting setting;
    struct preproc_private *priv = (struct preproc_private *)(obj->priv);
    struct preproc_config *config = (struct preproc_config *)(&(priv->config));

    g_aec_private.in_chnum = config->in_chnum;
    g_aec_private.mics_chnum = config->in_chnum - CFG_AUDIO_ECHO_REF_CH;
    g_aec_private.refs_chnum = CFG_AUDIO_ECHO_REF_CH;
    g_aec_private.out_chnum = CFG_VA_AEC_OUT_CH_NUM;

    setting.in_chnum = config->in_chnum;
    setting.out_chnum = g_aec_private.out_chnum;
    setting.bitwidth = config->bitwidth;
    setting.rate = config->rate;
    aec_dummy_init(&handle, setting);
    priv->priv = (void *)(handle);
    config->out_chnum = setting.out_chnum;

    return 0;
}

static int aec_dummy_adaptor_uninit(struct va_proc_obj *obj)
{
    return 0;
}

static int aec_dummy_adaptor_reset(struct va_proc_obj *obj)
{
    return 0;
}

static int aec_dummy_adaptor_process(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames)
{
    struct preproc_private *priv = (struct preproc_private *)(obj->priv);
    aec_dummy_handle_t *handle;
    int ret = 0;

    handle = (aec_dummy_handle_t *)(priv->priv);
    ret = aec_dummy_process(handle, inbuf, outbuf, frames, &(g_aec_private.beamforming));
    return ret;
}

static int aec_dummy_adaptor_set_params(struct va_proc_obj *obj, int cmd, void *data)
{
    switch(cmd) {
    case CMD_SET_AEC_CH_CONFIG:
      if (data != NULL) {
          struct aec_ch_setting *setting = (struct aec_ch_setting *)data;
          g_aec_private.in_chnum = setting->in_chnum;
          g_aec_private.mics_chnum = setting->mics_chnum;
          g_aec_private.refs_chnum = setting->refs_chnum;
          g_aec_private.out_chnum = setting->out_chnum;
      }
        break;
    default:
        break;
    }

    return 0;
}

static int aec_dummy_adaptor_get_params(struct va_proc_obj *obj, int cmd, void *data)
{
    switch(cmd) {
    case CMD_GET_BEAMFORMING_RESULT:
        {
            int *beamforming = (int *)data;
            *beamforming = g_aec_private.beamforming;
        }
        break;
    default:
        break;
    }

    return 0;
}

void mtk_aec_dummy_adaptor_register(struct algo_ops *ops)
{
    ops->init = aec_dummy_adaptor_init;
    ops->uninit = aec_dummy_adaptor_uninit;
    ops->set_params = aec_dummy_adaptor_set_params;
    ops->get_params = aec_dummy_adaptor_get_params;
    ops->reset = aec_dummy_adaptor_reset;
    ops->process = aec_dummy_adaptor_process;
}
