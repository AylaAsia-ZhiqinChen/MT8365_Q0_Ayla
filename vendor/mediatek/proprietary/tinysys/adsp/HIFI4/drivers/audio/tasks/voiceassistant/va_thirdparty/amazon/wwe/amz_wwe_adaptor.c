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

#include "pryon_lite.h"
#include "models_alexa.h"
#include "va_process.h"
#include "ww_process.h"
#include "mtk_heap.h"

/*TODO : set local variable*/
static char* decoderBuffer; // should be an array large enough to hold the largest decoder
/*TODO : set by cli or fixed in configuraiton*/
int m_detectThreshold;
// decoder handle
static PryonLiteDecoderHandle sDecoder = NULL;
PryonLiteSessionInfo sessionInfo;

static void loadModel(PryonLiteDecoderConfig* config)
{
    // In order to detect keywords, the decoder uses a model which defines the parameters,
    // neural network weights, classifiers, etc that are used at runtime to process the audio
    // and give detection results.

    // Each model is packaged in two formats:
    // 1. A .bin file that can be loaded from disk (via fopen, fread, etc)
    // 2. A .cpp file that can be hard-coded at compile time
    config->sizeofModel = prlBinaryModelLen;
    config->model = prlBinaryModelData;

    PRINTF_I("%s config->sizeofModel  %d, config->model 0x%x, &prlBinaryModelData  0x%x\n", \
            __func__,  config->sizeofModel, (uint32_t)config->model, (uint32_t)&prlBinaryModelData);
}

void detectionCallback(PryonLiteDecoderHandle handle, const PryonLiteResult* result)
{
    struct ww_private *priv = (struct ww_private *)(result->userData);

    if (PryonLiteDecoder_IsDecoderInitialized(handle)) {
        priv->detect_result = 1;
        priv->wakeword = result->keyword;
        priv->begin_idx = result->beginSampleIndex;
        priv->end_idx = result->endSampleIndex;
        priv->confidence = result->confidence;
        PRINTF_I("%s keyword %s, begin_idx %lld, end_idx %lld, confidence %d.\n", \
            __func__, priv->wakeword, priv->begin_idx, \
            priv->end_idx, priv->confidence);
    }
}

NORMAL_SECTION_FUNC static int wwe_adaptor_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    struct ww_private *priv = (struct ww_private *)(obj->priv);
    PryonLiteDecoderConfig amz_ww_cfg = PryonLiteDecoderConfig_Default;
    PryonLiteModelAttributes modelAttributes;

    /*TODO : move codes to another c file*/
    PryonLiteError err;
    loadModel(&amz_ww_cfg);

    PRINTF_I("AMZ_WW Wakeword init\n");

    err = PryonLite_GetModelAttributes(amz_ww_cfg.model, amz_ww_cfg.sizeofModel, &modelAttributes);

    if (err != PRYON_LITE_ERROR_OK)
    {
        PRINTF_E("%s AMZ WW Get Model attributes fail!\n", __func__);
        return -1;
    }

    decoderBuffer = (char *)MTK_pvPortMalloc(modelAttributes.requiredDecoderMem + 1, \
            get_adsp_heap_type(ADSP_MEM_LP_CACHE));

    amz_ww_cfg.decoderMem = decoderBuffer;
    amz_ww_cfg.sizeofDecoderMem = modelAttributes.requiredDecoderMem;

    // initialize decoder
    /*TODO : config*/
    m_detectThreshold = 500;
    amz_ww_cfg.detectThreshold = m_detectThreshold; // default threshold
    amz_ww_cfg.resultCallback = detectionCallback; //register detection handler
    //amz_ww_cfg.vadCallback = vadCallback; // register VAD handler
    amz_ww_cfg.useVad = 0; // disable voice activity detector
    amz_ww_cfg.userData = (void *)(priv);

    PRINTF_I("AMZ_WW Wakeword decoder init, mem %d.\n", modelAttributes.requiredDecoderMem);

    err = PryonLiteDecoder_Initialize(&amz_ww_cfg, &sessionInfo, &sDecoder);

    if (err != PRYON_LITE_ERROR_OK)
    {
        PRINTF_E(" %s PryonLiteDecoder_Initialize fail err %d\n", __func__, err);
    }

    // Set detection threshold for all keywords
    //(this function can be called any time after decoder initialization)
    int detectionThreshold = m_detectThreshold;

    PRINTF_I("AMZ_WW Wakeword set detect threadhold\n");
    err = PryonLiteDecoder_SetDetectionThreshold(sDecoder, NULL, detectionThreshold);

    if (err != PRYON_LITE_ERROR_OK)
    {
        PRINTF_E(" %s PryonLiteDecoder_SetDetectionThreshold fail, err %d\n", __func__, err);
    }
    return err;
}

NORMAL_SECTION_FUNC static int wwe_adaptor_uninit(struct va_proc_obj *obj)
{
    PryonLiteError err;
    PRINTF_I("AMZ_WW Wakeword decoder destroy\n");

    err = PryonLiteDecoder_Destroy(&sDecoder);
    if (err != PRYON_LITE_ERROR_OK)
    {
        PRINTF_E("%s PryonLiteDecoder_Destroy fail err %d\n", __func__, err);
    }

    MTK_vPortFree(decoderBuffer);
    return 0;
}


static int wwe_adaptor_process(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames)
{
    int ret;

    ret = PryonLiteDecoder_PushAudioSamples(sDecoder, (const short *)inbuf, frames);
    if (ret != PRYON_LITE_ERROR_OK)
    {
        // handle error
        PRINTF_E("%s PryonLiteDecoder_PushAudioSamples error\n", __func__);
    }
    return ret;
}

NORMAL_SECTION_FUNC void amz_wwe_adaptor_register(struct algo_ops *ops)
{
    ops->init = wwe_adaptor_init;
    ops->uninit = wwe_adaptor_uninit;
    ops->set_params = NULL;
    ops->get_params = NULL;
    ops->reset = NULL;
    ops->process = wwe_adaptor_process;
}
