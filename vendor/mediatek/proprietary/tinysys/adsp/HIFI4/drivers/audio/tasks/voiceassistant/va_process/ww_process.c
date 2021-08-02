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
#include "ww_process.h"
#include "chdemux_process.h"
#ifdef VA_DUMMY_WW_SUPPORT
#include "mtk_ww_dummy_adaptor.h"
#endif
#ifdef CFG_VA_MTK_WW_LITE_SUPPORT
#include "mtk_wwe_lite_adaptor.h"
#endif
#ifdef CFG_AMZ_WW_LITE_SUPPORT
#include "amz_wwe_adaptor.h"
#endif

#ifdef CFG_GOOGLE_WW_LITE_SUPPORT
#include "google_wwe_adaptor.h"
#endif

#include "mt_printf.h"
#include "systimer.h"
#include "audio_task.h"
#include "mtk_heap.h"

#ifdef CFG_VA_WW_MCPS_CALC
DEFINE_MCPS_CALC(ww_mcps)
#endif

static const char *debug_wakeword = "Hi wakeword";
static struct algo_ops algo_adaptor;

#ifdef DYNAMIC_LOAD_MODEL
/* use static sram to store the model data that from share buffer in dram */
static char g_adsp_model[CFG_VA_MODEL_LEN];
#endif

NORMAL_SECTION_FUNC static int ww_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    struct ww_private *priv = (struct ww_private *)(obj->priv);
    struct ww_config *config = (struct ww_config *)(&(priv->config));

    config->rate = in.rate;
    config->bitwidth = in.bitwidth;
    config->chnum = in.chnum;
    priv->cur_data_idx = 0;
    priv->detect_result = 0;

    if ((config->bitwidth != CFG_VA_WW_BITWIDTH) ||
        (config->chnum != CFG_VA_WW_CHNUM)) {
        /* Need temp buffer and format convert */
        struct chdemux_out out_config;
        uint32_t tmp_buf_size;
        int mem_type = get_adsp_heap_type(ADSP_MEM_LP_CACHE);

        chdemux_proc_create(&(priv->fmt_cvt));
        out_config.bitwidth = CFG_VA_WW_BITWIDTH;
        out_config.out_chnum = CFG_VA_WW_CHNUM;
        out_config.out_chidx[0] = 0;
        priv->fmt_cvt->set_params(priv->fmt_cvt, CMD_SET_CHDEMUX_OUT, &out_config);
        priv->fmt_cvt->init(priv->fmt_cvt, in, NULL, frames);
        tmp_buf_size = frames * CFG_VA_WW_CHNUM * CFG_VA_WW_BITWIDTH / 8;
        priv->tmp_buf = (char *)(MTK_pvPortMalloc(tmp_buf_size, mem_type));
        configASSERT(priv->tmp_buf != NULL);
        PRINTF_E("do ww format convert\n");
    }


    if (out != NULL) {
        out->rate = in.rate;
        out->chnum = in.chnum;
        out->bitwidth = in.bitwidth;
    }

    if (algo_adaptor.init != NULL)
        algo_adaptor.init(obj, in, NULL, frames);
#ifdef CFG_VA_WW_MCPS_CALC
    mcps_calc_init(&ww_mcps);
#endif
    obj->state = PROC_STAT_INIT;
    PRINTF_E("%s [time:%llu] rate(%d), bitwidth(%d), chnum(%d)\n", __func__, read_systimer_stamp_ns(), config->rate, config->bitwidth, config->chnum);
    return 0;
}

NORMAL_SECTION_FUNC static int ww_uninit(struct va_proc_obj *obj)
{
    struct ww_private *priv = (struct ww_private *)(obj->priv);

    if (obj->state != PROC_STAT_INIT)
        return -1;

    if (priv->fmt_cvt != NULL) {
        chdemux_proc_destroy(priv->fmt_cvt);
        priv->fmt_cvt = NULL;
    }
    if (priv->tmp_buf) {
        MTK_vPortFree(priv->tmp_buf);
        priv->tmp_buf = NULL;
    }

    if (algo_adaptor.uninit != NULL)
        algo_adaptor.uninit(obj);

    priv->cur_data_idx = 0;
    priv->detect_result = 0;

    obj->state = PROC_STAT_UNINIT;
    PRINTF_E("%s [time:%llu]\n", __func__, read_systimer_stamp_ns());
    return 0;
}

static int ww_set_params(struct va_proc_obj *obj, int cmd, void *data)
{
    struct ww_private *priv = (struct ww_private *)(obj->priv);

    switch(cmd){
    case CMD_SET_WW_FORCE_OK:
        {
            priv->debug_cmd = CMD_SET_WW_FORCE_OK;
        }
        break;
    case CMD_SET_WW_FORCE_FAIL:
        {
            priv->debug_cmd = CMD_SET_WW_FORCE_FAIL;
        }
        break;
    case CMD_SET_WW_FORCE_CONTINUE:
        {
            priv->debug_cmd = CMD_SET_WW_FORCE_CONTINUE;
        }
        break;
    case CMD_CLR_WW_FORCE_CONTINUE:
        {
            priv->debug_cmd = 0;
        }
        break;
#ifdef DYNAMIC_LOAD_MODEL
    case CMD_SET_WW_LOAD_MODEL:
        {
            struct ww_model_dma *dma = (struct ww_model_dma *)data;
            memcpy((void *)g_adsp_model, dma->addr, dma->size);
            priv->config.model = g_adsp_model;
            priv->config.model_size = dma->size;
            PRINTF_E("%s g_adsp_model[0] = %d, g_adsp_model[1] = %d\n", __func__, g_adsp_model[0], g_adsp_model[1]);
        }
        break;
#endif
    break;
    default:
        break;
    }
    if (algo_adaptor.set_params != NULL)
        algo_adaptor.set_params(obj, cmd, data);
    PRINTF_E("%s [time:%llu]\n", __func__, read_systimer_stamp_ns());
    return 0;
}

static int ww_get_params(struct va_proc_obj *obj, int cmd, void *data)
{
    struct ww_private *priv = (struct ww_private *)(obj->priv);

    switch(cmd){
    case CMD_GET_WW_RESULT:
        {
            struct ww_result *result = (struct ww_result *)data;

            if (obj->state != PROC_STAT_INIT)
                break;

            result->cur_idx = priv->cur_data_idx;
            result->begin_idx = priv->begin_idx;
            result->end_idx = priv->end_idx;
            result->wakeword = priv->wakeword;
            result->confidence = priv->confidence;
            priv->detect_result = 0;
        }
        break;
    default:
        break;
    }

    if (algo_adaptor.get_params != NULL)
        algo_adaptor.get_params(obj, cmd, data);

    return 0;
}

static int ww_reset(struct va_proc_obj *obj)
{
    //struct ww_private *priv = (struct ww_private *)(obj->priv);

    if (obj->state != PROC_STAT_INIT)
        return -1;

    if (algo_adaptor.reset != NULL)
        algo_adaptor.reset(obj);

    PRINTF_E("%s [time:%llu]\n", __func__, read_systimer_stamp_ns());
    return 0;
}

static int ww_debug_cmd_parse(struct ww_private *priv)
{
    int ret = 0;

    switch (priv->debug_cmd) {
    case CMD_SET_WW_FORCE_FAIL:
        ret = VA_WW_FAIL;
        priv->debug_cmd = 0;
        break;
    case CMD_SET_WW_FORCE_OK:
        ret = VA_WW_SUCESS;
        priv->detect_result = 1;
        priv->begin_idx = priv->cur_data_idx - priv->config.rate;
        priv->end_idx = priv->cur_data_idx;
        priv->wakeword = debug_wakeword;
        priv->confidence = 950;
        priv->debug_cmd = 0;
        break;
    case CMD_SET_WW_FORCE_CONTINUE:
        ret = VA_WW_CONTINUE;
        break;
    default:
        break;
    }

    return ret;
}

static int ww_process(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames)
{
    struct ww_private *priv = (struct ww_private *)(obj->priv);
    int ret = 0;
    int debug_result = 0;
    char *ww_proc_buf = inbuf;

    if (obj->state != PROC_STAT_INIT)
        return VA_ERROR;

#ifdef CFG_VA_WW_MCPS_CALC
    mcps_calc_before_proc(&ww_mcps);
#endif

    if (priv->fmt_cvt != NULL) {
        priv->fmt_cvt->process(priv->fmt_cvt, inbuf, priv->tmp_buf, frames);
        ww_proc_buf = priv->tmp_buf;
    }

    priv->cur_data_idx += (long long)frames;

    if (algo_adaptor.process != NULL)
        ret = algo_adaptor.process(obj, ww_proc_buf, outbuf, frames);
#ifdef CFG_VA_WW_MCPS_CALC
    mcps_calc_after_proc(&ww_mcps);
#endif
    if (priv->detect_result == 1) {
        ret = VA_WW_SUCESS;
        priv->detect_result = 0;
    } else if (priv->detect_result == 2) {
        ret = VA_WW_FAIL;
        priv->detect_result = 0;
    } else {
        ret = VA_WW_CONTINUE;
    }

    debug_result = ww_debug_cmd_parse(priv);
    if (debug_result != 0)
        return debug_result;

    return ret;
}

NORMAL_SECTION_FUNC int ww_proc_create(struct va_proc_obj **obj)
{
    struct va_proc_obj *ww;
    int mem_type = get_adsp_heap_type(ADSP_MEM_LP_CACHE);

    ww =
        (struct va_proc_obj *)(MTK_pvPortMalloc(sizeof(struct va_proc_obj), mem_type));
    configASSERT(ww != NULL);
    memset(ww, 0, sizeof(struct va_proc_obj));
    ww->type = VA_PROC_WW;
    ww->priv =
        (struct ww_private *)(MTK_pvPortMalloc(sizeof(struct ww_private), mem_type));
    configASSERT(ww->priv != NULL);
    memset(ww->priv, 0, sizeof(struct ww_private));

    ww->init = ww_init;
    ww->uninit = ww_uninit;
    ww->get_params = ww_get_params;
    ww->set_params = ww_set_params;
    ww->reset = ww_reset;
    ww->process = ww_process;
    ww->state = PROC_STAT_UNINIT;

    *obj = ww;

#ifdef VA_DUMMY_WW_SUPPORT
    mtk_ww_dummy_adaptor_register(&algo_adaptor);
#endif
#ifdef CFG_AMZ_WW_LITE_SUPPORT
    amz_wwe_adaptor_register(&algo_adaptor);
#endif
#ifdef CFG_VA_MTK_WW_LITE_SUPPORT
    mtk_wwe_lite_adaptor_register(&algo_adaptor);
#endif

#ifdef CFG_GOOGLE_WW_LITE_SUPPORT
    google_wwe_adaptor_register(&algo_adaptor);
#endif

    PRINTF_E("%s [time:%llu] wakeword create success\n", __func__, read_systimer_stamp_ns());
    return 0;
}

NORMAL_SECTION_FUNC int ww_proc_destroy(struct va_proc_obj *obj)
{
    if (obj->priv)
        MTK_vPortFree(obj->priv);
    MTK_vPortFree(obj);
    memset(&algo_adaptor, 0, sizeof(algo_adaptor));
    PRINTF_E("%s [time:%llu] wakeword destroy success\n", __func__, read_systimer_stamp_ns());
    return 0;
}
