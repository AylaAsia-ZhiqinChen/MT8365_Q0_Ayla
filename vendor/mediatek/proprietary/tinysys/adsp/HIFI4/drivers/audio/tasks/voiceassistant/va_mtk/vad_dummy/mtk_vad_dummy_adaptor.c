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
#include "vad_dummy.h"
#include "va_process.h"
#include "vad_process.h"

static inline int process_ret_to_va_ret_value(int state, int value)
{
    int ret;

    if (state == 0) {
        ret = (value == 1) ? VA_VAD_SUCESS : VA_VAD_FAIL;
    } else {
        ret = (value == 1) ? VA_WW_SUCESS : VA_WW_CONTINUE;
    }
    return ret;
}

static int vad_dummy_adaptor_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    vad_dummy_handle_t *handle;
    struct vad_private *priv = (struct vad_private *)(obj->priv);
    struct vad_config *config = (struct vad_config *)(&(priv->config));

    vad_dummy_init(&handle, config->rate, config->chnum, config->bitwidth);
    priv->priv = (void *)(handle);
    return 0;
}

static int vad_dummy_adaptor_uninit(struct va_proc_obj *obj)
{
    return 0;
}

static int vad_dummy_adaptor_process(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames)
{
    struct vad_private *priv = (struct vad_private *)(obj->priv);
    vad_dummy_handle_t *handle;
    char *vad_proc_buf = inbuf;
    int ret = 0;

    handle = (vad_dummy_handle_t *)(priv->priv);
    ret = vad_dummy_process(handle, vad_proc_buf, frames);
    ret = process_ret_to_va_ret_value(priv->state, ret);

    return ret;
}

void mtk_vad_dummy_adaptor_register(struct algo_ops *ops)
{
    ops->init = vad_dummy_adaptor_init;
    ops->uninit = vad_dummy_adaptor_uninit;
    ops->set_params = NULL;
    ops->get_params = NULL;
    ops->reset = NULL;
    ops->process = vad_dummy_adaptor_process;
}
