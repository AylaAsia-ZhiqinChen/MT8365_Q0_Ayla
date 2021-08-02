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

#include "va_process.h"
#include "mtk_heap.h"
#include "swVAD.h"

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

static int vad_adaptor_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    initSwVad(CFG_VA_VAD_BITWIDTH);
    return 0;
}

static int vad_adaptor_uninit(struct va_proc_obj *obj)
{
    return 0;
}

static int vad_adaptor_reset(struct va_proc_obj *obj)
{
    resetSwVad();
    initSwVad(CFG_VA_VAD_BITWIDTH); // reset history data

    return 0;
}

static int vad_adaptor_process(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames)
{
    MIC_INPUT input;
    int isTooNoisy;  //If the environment is too noisy, please turn-on FFP and wake word engine
    int ret = 0;

    input.inData = inbuf;
    ret = getVadStatus(&input);
    isTooNoisy= getNoisyStatus();
    ret = process_ret_to_va_ret_value(0, ret);
    return 0;
}

void mtk_vad_adaptor_register(struct algo_ops *ops)
{
    ops->init = vad_adaptor_init;
    ops->uninit = vad_adaptor_uninit;
    ops->set_params = NULL;
    ops->get_params = NULL;
    ops->reset = vad_adaptor_reset;
    ops->process = vad_adaptor_process;
}
