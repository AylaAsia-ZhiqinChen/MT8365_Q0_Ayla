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
#include "google_wwe_adaptor.h"
#include "hotword_dsp_api.h"
#include "ww_process.h"
#include "mtk_heap.h"

#define GVA_INPUT_BITWIDTH      16
#define GVA_INPUT_CHANNEL_NUM   1
#define GVA_KEYWORD_LEN_MS      1000
#define GVA_KEYWORD_CONFIDENCE  100

struct google_wwe_priv {
    uint8_t force_pass;
};

static int google_wwe_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    struct ww_private *ww_priv = (struct ww_private *)(obj->priv);
    struct google_wwe_priv *priv;
    int Gg_WW_Verion, MaxAudPreamble, ret;

    priv = (struct google_wwe_priv *)(MTK_pvPortMalloc(sizeof(struct google_wwe_priv), MTK_eMemDefault));
    configASSERT(priv != NULL);
    memset(priv, 0, sizeof(struct google_wwe_priv));

    Gg_WW_Verion = GoogleHotwordVersion();
    MaxAudPreamble = GoogleHotwordDspGetMaximumAudioPreambleMs();
    PRINTF_I("%s GoogleHotwordVersion %d, Maximum Audio Preamble Ms %d\n",
        __func__, Gg_WW_Verion, MaxAudPreamble);

#ifndef DYNAMIC_LOAD_MODEL
    PRINTF_D("%s GoogleHotwordDspInit+ &en_all_mmap 0x%x\n", __func__, &en_all_mmap[0]);
    ret = GoogleHotwordDspInit((void *)(&en_all_mmap[0]));
#else
    PRINTF_D("%s GoogleHotwordDspInit+ &en_all_mmap 0x%x\n", __func__, ww_priv->config.model);
    PRINTF_D("%s GoogleHotwordDspInit+ en_all_mmap size 0x%x\n", __func__, ww_priv->config.model_size);
    ret = GoogleHotwordDspInit((void *)(ww_priv->config.model));
#endif
    PRINTF_D("%s GoogleHotwordDspInit- kGoogleHotwordRequiredDataAlignment 0x%x\n",
         __func__, kGoogleHotwordRequiredDataAlignment);
    if(ret != 1) {
        PRINTF_E("%s GoogleHotwordDspInit fail!\n", __func__);
        goto INIT_FAIL;
    }

    ww_priv->priv = (void *)priv;

    return 0;

INIT_FAIL:
    MTK_vPortFree(priv);
    return -1;
}

static int google_wwe_uninit(struct va_proc_obj *obj)
{
    struct ww_private *ww_priv = (struct ww_private *)(obj->priv);
    struct google_wwe_priv *priv = (struct google_wwe_priv *)ww_priv->priv;

    PRINTF_D("%s+\n", __func__);

    MTK_vPortFree(priv);
    ww_priv->priv = NULL;

    return 0;
}

static int google_wwe_set_params(struct va_proc_obj *obj, int cmd, void *data)
{
    struct ww_private *ww_priv = (struct ww_private *)(obj->priv);
    struct google_wwe_priv *priv = (struct google_wwe_priv *)ww_priv->priv;

    PRINTF_D("%s, cmd %d\n", __func__, cmd);
    switch (cmd) {
        case CMD_SET_WW_FORCE_OK:
            priv->force_pass = 1;
            break;
        default:
            break;
    }

    return 0;
}

static int google_wwe_reset(struct va_proc_obj *obj)
{
    PRINTF_D("%s+\n", __func__);

    //GoogleHotwordDspReset();

    return 0;
}

static int google_wwe_process(struct va_proc_obj *obj,
    char *inbuf, char *outbuf, int frames)
{
    struct ww_private *ww_priv = (struct ww_private *)(obj->priv);
    struct google_wwe_priv *priv = (struct google_wwe_priv *)ww_priv->priv;
    int preamble_length_ms, ret;

    ret = GoogleHotwordDspProcess(inbuf, frames, &preamble_length_ms);
    if (ret || priv->force_pass) {
        GoogleHotwordDspReset();
        ww_priv->detect_result = 1;
        ww_priv->wakeword = "OK!Google";
        ww_priv->begin_idx =
            ww_priv->cur_data_idx - (ww_priv->config.rate / 1000) * GVA_KEYWORD_LEN_MS;
        if (ww_priv->begin_idx < 0)
            ww_priv->begin_idx = 0;
        ww_priv->end_idx = ww_priv->cur_data_idx;
        ww_priv->confidence = GVA_KEYWORD_CONFIDENCE;
        priv->force_pass = 0;
        PRINTF_I("%s OK Google hotword pass: preamble_length_ms %d, begin_idx %llu, end_idx %llu\n",
            __func__, preamble_length_ms, ww_priv->begin_idx, ww_priv->end_idx);
    }

    return 0;
}

void google_wwe_adaptor_register(struct algo_ops *ops)
{
    PRINTF_D("%s+\n", __func__);

    ops->init = google_wwe_init;
    ops->uninit = google_wwe_uninit;
    ops->set_params = google_wwe_set_params;
    ops->get_params = NULL;
    ops->reset = google_wwe_reset;
    ops->process = google_wwe_process;
}

