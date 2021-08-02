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
#include "vad_process.h"
#ifdef VA_DUMMY_VAD_SUPPORT
#include "mtk_vad_dummy_adaptor.h"
#endif
#ifdef VA_MTK_VAD_SUPPORT
#include "mtk_vad_adaptor.h"
#endif
#include "chdemux_process.h"

#include "mt_printf.h"
#include "systimer.h"
#include "audio_task.h"
#include "mtk_heap.h"

#if (CFG_VA_VAD_WW_IN_ONE != 0)
static const char *debug_wakeword = "Hi vad";
#endif

#ifdef CFG_VA_VAD_MCPS_CALC
DEFINE_MCPS_CALC(vad_mcps)
#endif

static struct algo_ops algo_adaptor;

NORMAL_SECTION_FUNC static int vad_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    struct vad_private *priv = (struct vad_private *)(obj->priv);
    struct vad_config *config = (struct vad_config *)(&(priv->config));

    config->rate = in.rate;
    config->chnum = in.chnum;
    config->bitwidth = in.bitwidth;

    if ((config->bitwidth != CFG_VA_VAD_BITWIDTH) ||
        (config->chnum != CFG_VA_VAD_CHNUM)) {
        /* Need temp buffer and format convert */
        struct chdemux_out out_config;
        uint32_t tmp_buf_size;
        int mem_type = get_adsp_heap_type(ADSP_MEM_LP_CACHE);

        chdemux_proc_create(&(priv->fmt_cvt));
        out_config.bitwidth = CFG_VA_VAD_BITWIDTH;
        out_config.out_chnum = CFG_VA_VAD_CHNUM;
        out_config.out_chidx[0] = 0;
        priv->fmt_cvt->set_params(priv->fmt_cvt, CMD_SET_CHDEMUX_OUT, &out_config);
        priv->fmt_cvt->init(priv->fmt_cvt, in, NULL, frames);
        tmp_buf_size = frames * CFG_VA_VAD_CHNUM * CFG_VA_VAD_BITWIDTH / 8;
        priv->tmp_buf = (char *)(MTK_pvPortMalloc(tmp_buf_size, mem_type));
        configASSERT(priv->tmp_buf != NULL);
        PRINTF_E("do vad format convert\n");
    }

    /* There is no output for VAD, so its output is its input */
    if (out != NULL) {
        out->rate = in.rate;
        out->chnum = in.chnum;
        out->bitwidth = in.bitwidth;
    }

    priv->cur_data_idx = 0;
    priv->debug_cmd = 0;
    priv->state = 0;
#ifdef CFG_VA_VAD_MCPS_CALC
    mcps_calc_init(&vad_mcps);
#endif
    obj->state = PROC_STAT_INIT;

    if (algo_adaptor.init != NULL)
        algo_adaptor.init(obj, in, out, frames);

    PRINTF_E("%s [time:%llu] rate(%d), bitwidth(%d), chnum(%d)\n", __func__, read_systimer_stamp_ns(), config->rate, config->bitwidth, config->chnum);
    return 0;
}

NORMAL_SECTION_FUNC static int vad_uninit(struct va_proc_obj *obj)
{
    struct vad_private *priv = (struct vad_private *)(obj->priv);

    PRINTF_E("%s [time:%llu]\n", __func__, read_systimer_stamp_ns());
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
    priv->cur_data_idx = 0;
    priv->debug_cmd = 0;
    priv->state = 0;

    if (algo_adaptor.uninit != NULL)
        algo_adaptor.uninit(obj);

    obj->state = PROC_STAT_UNINIT;
    return 0;
}

static int vad_get_params(struct va_proc_obj *obj, int cmd, void *data)
{
    switch(cmd){
#if (CFG_VA_VAD_WW_IN_ONE != 0)
    case CMD_GET_WW_RESULT:
        {
            struct ww_result *result = (struct ww_result *)data;
            struct vad_private *priv = (struct vad_private *)(obj->priv);

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
#endif
    default:
        break;
    }

    if (algo_adaptor.get_params != NULL)
        algo_adaptor.get_params(obj, cmd, data);

    return 0;
}

static int vad_set_params(struct va_proc_obj *obj, int cmd, void *data)
{
    struct vad_private *priv = (struct vad_private *)(obj->priv);

    if (obj->state != PROC_STAT_INIT)
        return -1;

    switch(cmd) {
    case CMD_SET_VAD_FORCE_OK:
        priv->debug_cmd = CMD_SET_VAD_FORCE_OK;
        break;
    case CMD_SET_VAD_FORCE_FAIL:
        priv->debug_cmd = CMD_SET_VAD_FORCE_FAIL;
        break;
    case CMD_CLR_VAD_FORCE_FAIL:
        priv->debug_cmd = 0;
        break;
#if (CFG_VA_VAD_WW_IN_ONE != 0)
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
#endif
    default:
        break;
    }

    if (algo_adaptor.set_params != NULL)
        algo_adaptor.set_params(obj, cmd, data);

    PRINTF_E("%s [time:%llu]\n", __func__, read_systimer_stamp_ns());
    return 0;
}

static int vad_reset(struct va_proc_obj *obj)
{
    PRINTF_E("%s [time:%llu]\n", __func__, read_systimer_stamp_ns());
    if (obj->state != PROC_STAT_INIT)
        return -1;

    if (algo_adaptor.reset != NULL)
        algo_adaptor.reset(obj);
    return 0;
}

static int vad_debug_cmd_parse(struct vad_private *priv)
{
    int ret = 0;

    switch (priv->debug_cmd) {
    case CMD_SET_VAD_FORCE_OK:
        ret = VA_VAD_SUCESS;
#if (CFG_VA_VAD_WW_IN_ONE != 0)
        priv->state = 1;
#endif
        priv->debug_cmd = 0;
        break;
    case CMD_SET_VAD_FORCE_FAIL:
        ret = VA_VAD_FAIL;
        break;
#if (CFG_VA_VAD_WW_IN_ONE != 0)
    case CMD_SET_WW_FORCE_FAIL:
        ret = VA_WW_FAIL;
        priv->state = 0;
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
#endif
    default:
        break;
    }

    return ret;
}

static int vad_process(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames)
{
    struct vad_private *priv = (struct vad_private *)(obj->priv);
    int ret = 0;
    int debug_result = 0;
    char *vad_proc_buf = inbuf;

    if (obj->state != PROC_STAT_INIT)
        return VA_ERROR;

#ifdef CFG_VA_VAD_MCPS_CALC
    mcps_calc_before_proc(&vad_mcps);
#endif
    if (priv->fmt_cvt != NULL) {
        priv->fmt_cvt->process(priv->fmt_cvt, inbuf, priv->tmp_buf, frames);
        vad_proc_buf = priv->tmp_buf;
    }

    if (algo_adaptor.process != NULL)
        ret = algo_adaptor.process(obj, vad_proc_buf, outbuf, frames);

#ifdef CFG_VA_VAD_MCPS_CALC
    mcps_calc_after_proc(&vad_mcps);
#endif
    priv->cur_data_idx += (long long)frames;
    debug_result = vad_debug_cmd_parse(priv);
    if (debug_result != 0)
        return debug_result;

    return ret;
}

NORMAL_SECTION_FUNC int vad_proc_create(struct va_proc_obj **obj)
{
    struct va_proc_obj *vad;
    int mem_type = get_adsp_heap_type(ADSP_MEM_LP_CACHE);

    vad =
        (struct va_proc_obj *)(MTK_pvPortMalloc(sizeof(struct va_proc_obj), mem_type));
    configASSERT(vad != NULL);
    memset(vad, 0, sizeof(struct va_proc_obj));
    vad->type = VA_PROC_VAD;
    vad->priv =
        (struct vad_private *)(MTK_pvPortMalloc(sizeof(struct vad_private), mem_type));;
    configASSERT(vad->priv != NULL);
    memset(vad->priv, 0, sizeof(struct vad_private));

    vad->init = vad_init;
    vad->uninit = vad_uninit;
    vad->reset = vad_reset;
    vad->get_params = vad_get_params;
    vad->set_params = vad_set_params;
    vad->process = vad_process;
    vad->state = PROC_STAT_UNINIT;

    *obj = vad;
#ifdef VA_DUMMY_VAD_SUPPORT
    mtk_vad_dummy_adaptor_register(&algo_adaptor);
#endif
#ifdef VA_MTK_VAD_SUPPORT
    mtk_vad_adaptor_register(&algo_adaptor);
#endif
    PRINTF_E("%s [time:%llu] vad create success\n", __func__, read_systimer_stamp_ns());
    return 0;
}

NORMAL_SECTION_FUNC int vad_proc_destroy(struct va_proc_obj *obj)
{
    if (obj->priv)
        MTK_vPortFree(obj->priv);
    MTK_vPortFree(obj);
    memset(&algo_adaptor, 0, sizeof(algo_adaptor));
    PRINTF_E("%s [time:%llu] vad destroy success\n", __func__, read_systimer_stamp_ns());
    return 0;
}
