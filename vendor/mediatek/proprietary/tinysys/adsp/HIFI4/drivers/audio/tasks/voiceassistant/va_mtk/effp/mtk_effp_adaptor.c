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
#include "preproc_process.h"
#include "mtk_heap.h"

#include "EFFP_Interface.h"
#include "EFFP_Memory.h"

#define __tcm_mem_size__ (1024 * 16)
#define __sram_mem_size__ (1024 *337)//current bypass mode use 32bytes, normal mode not use

extern int holiday_model[];
extern int model_size;
static int g_beam;
static int realized = 0;

static int effp_adaptor_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    struct preproc_private *priv = (struct preproc_private *)(obj->priv);
    struct preproc_config *config = (struct preproc_config *)(&(priv->config));
    int ret = 0;
    int require_sram_size, require_tcm_size;
    void *sram_ptr, *tcm_ptr;
    struct ffp_mem_setting *mem_setting;
    EFFP_MemSpecifier require_mem, supplyMem;
    EFFP_MemConfig mem_config;
    EFFP_MemBlock mic_config;

    mem_setting =
        (struct ffp_mem_setting *)(MTK_pvPortMalloc(sizeof(struct ffp_mem_setting),
            get_adsp_heap_type(ADSP_MEM_NORMAL_CACHE)));
    configASSERT(mem_setting != NULL);
    memset(mem_setting, 0, sizeof(struct ffp_mem_setting));
    priv->priv = (void *)(mem_setting);
    sram_ptr = tcm_ptr = NULL;
    printf("EFFP version: %s\n", EFFP_version());
    if (!realized) {
        if (!EFFP_realize())
            PRINTF_E("%s::EFFP_realize fail, err=%d\n", __func__, EFFP_getErrorCode());
        else
            realized = 1;
    }
    EFFP_MemBlock_reset(&mic_config);
    mic_config.ptr = (void*)holiday_model;
    mic_config.byteNum = model_size * sizeof(int);
    config->out_chnum = CFG_VA_AEC_OUT_CH_NUM;
    ret = EFFP_prepare(config->in_chnum - CFG_AUDIO_ECHO_REF_CH, CFG_AUDIO_ECHO_REF_CH,
                    mic_config, config->proc_frames, config->out_chnum);
    if (ret == 0) {
        PRINTF_E("%s::EFFP_prepare fail, err=%d\n", __func__, EFFP_getErrorCode());
        MTK_vPortFree(mem_setting);
        mem_setting = NULL;
        return -1;
    }
    PRINTF_I("effp_value_id_input_signals_byte_num = %d\n",
             EFFP_getValue_int(effp_value_id_input_signals_byte_num));
    PRINTF_I("effp_value_id_output_signals_byte_num = %d\n",
             EFFP_getValue_int(effp_value_id_output_signals_byte_num));

    EFFP_MemSpecifier_reset(&supplyMem);
    EFFP_MemSpecifier_reset(&require_mem);
    EFFP_MemSpecifier_setAdequate(&supplyMem, effp_mem_id_sram_general);
    EFFP_MemSpecifier_setSize(&supplyMem, effp_mem_id_tcm_temp, __tcm_mem_size__);

    if (EFFP_queryMem(&supplyMem, &require_mem) == 0) {
            PRINTF_E("%s::EFFP_queryMem fail, err=%d\n", __func__, EFFP_getErrorCode());
            EFFP_conclude();
            MTK_vPortFree(mem_setting);
            mem_setting = NULL;
            return -1;
    }

    require_sram_size = EFFP_MemSpecifier_getSize(&require_mem, effp_mem_id_sram_general);
    if (require_sram_size) {
        mem_setting->sram_addr =
            MTK_pvPortMalloc(__sram_mem_size__ + 8, get_adsp_heap_type(ADSP_MEM_NORMAL_CACHE));
        sram_ptr = (void*)(0x8 - ((TickType_t)(mem_setting->sram_addr) & 0x7) + (TickType_t)mem_setting->sram_addr);
        PRINTF_I("sram addr:%p<-->%p\n", mem_setting->sram_addr, sram_ptr);
    }
    PRINTF_I("require_sram_size:%d\n", require_sram_size);
    require_tcm_size = EFFP_MemSpecifier_getSize(&require_mem, effp_mem_id_tcm_temp);
    if (require_tcm_size) {
        mem_setting->tcm_addr =
            MTK_pvPortMalloc(__tcm_mem_size__ + 8, get_adsp_heap_type(ADSP_MEM_NORMAL_CACHE));

        tcm_ptr = (void*)(0x8 - ((TickType_t)(mem_setting->tcm_addr) & 0x7) + (TickType_t)mem_setting->tcm_addr);
        PRINTF_I("tcmMem addr:%p<-->%p\n", mem_setting->tcm_addr, tcm_ptr);
    }

    PRINTF_I("require_tcm_size:%d\n", require_tcm_size);

    EFFP_MemConfig_set(&mem_config,
                       effp_mem_id_sram_general,
                       sram_ptr,
                       require_sram_size);
    EFFP_MemConfig_set(&mem_config,
                       effp_mem_id_tcm_temp,
                       tcm_ptr,
                       require_tcm_size);

    if (EFFP_dispatchMem(&mem_config) == 0) {
        PRINTF_E("%s::EFFP_dispatchMem fail, err=%d\n", __func__, EFFP_getErrorCode());
        EFFP_conclude();
        MTK_vPortFree(mem_setting->tcm_addr);
        MTK_vPortFree(mem_setting->sram_addr);
        MTK_vPortFree(mem_setting);
        mem_setting = NULL;
        return -1;
    }

    if (EFFP_resetStream() == 0) {
        PRINTF_E("%s::EFFP_resetStream fail, err=%d\n", __func__, EFFP_getErrorCode());
        EFFP_conclude();
        MTK_vPortFree(mem_setting->tcm_addr);
        MTK_vPortFree(mem_setting->sram_addr);
        MTK_vPortFree(mem_setting);
        mem_setting = NULL;
        return -1;
    }

    return ret;
}

static int effp_adaptor_uninit(struct va_proc_obj *obj)
{
    struct preproc_private *priv = (struct preproc_private *)(obj->priv);
    struct ffp_mem_setting *mem_setting = (struct ffp_mem_setting *)priv->priv;

    if (EFFP_conclude() == 0) {
        PRINTF_E("EFFP_conclude fail, err=%d\n", EFFP_getErrorCode());
    }
    if (mem_setting && mem_setting->tcm_addr) {
        MTK_vPortFree(mem_setting->tcm_addr);
        mem_setting->tcm_addr= NULL;
    }
    if (mem_setting && mem_setting->sram_addr) {
        MTK_vPortFree(mem_setting->sram_addr);
        mem_setting->sram_addr = NULL;
    }
    if (mem_setting) {
        MTK_vPortFree(mem_setting);
        mem_setting = NULL;
    }
    return 0;
}

static int effp_adaptor_set_params(struct va_proc_obj *obj, int cmd, void *data)
{
    switch(cmd) {
    case CMD_SET_AEC_WW_OK:
        PRINTF_E("EFFP_feedbackRecognition enter!!!\n");
        if (data != NULL) {
            struct aec_feedback_setting *setting = (struct aec_feedback_setting *)data;
            if (EFFP_feedbackRecognition(setting->channel_id, setting->conf, setting->frame_time_diff) == 0)
                PRINTF_E("EFFP_feedbackRecognition fail, err=%d\n", EFFP_getErrorCode());
        }
        break;
    case CMD_SET_AEC_WW_START:
        PRINTF_E("EFFP_resetStream enter!!!\n");
        if (EFFP_resetStream() == 0)
            PRINTF_E("EFFP_resetStream fail, err=%d\n", EFFP_getErrorCode());
	break;
    default:
        break;
    }

    return 0;
}

static int effp_adaptor_get_params(struct va_proc_obj *obj, int cmd, void *data)
{
    switch(cmd) {
    case CMD_GET_BEAMFORMING_RESULT:
        *(int*)data = g_beam;
        break;
    default:
        break;
    }

    return 0;
}

static int effp_adaptor_reset(struct va_proc_obj *obj)
{
    if (EFFP_resetStream() == 0) {
        PRINTF_E("EFFP_resetStream fail, err=%d\n", EFFP_getErrorCode());
        return -1;
    }
    return 0;
}

static int effp_adaptor_process(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames)
{
    int ret = 0;
    struct preproc_private *priv = (struct preproc_private *)(obj->priv);
    struct preproc_config ffp_config = priv->config;
    EFFP_MemBlock input_signals, output_signals;
    input_signals.ptr = (void*)inbuf;
    input_signals.byteNum = ffp_config.bitwidth * ffp_config.in_chnum * frames / 8;
    output_signals.ptr = (void*)outbuf;
    output_signals.byteNum = ffp_config.bitwidth * ffp_config.out_chnum * frames / 8;
    ret = EFFP_processStream(input_signals, output_signals, &(g_beam));
    if (ret == 0) {
        PRINTF_E("EFFP_processStream fail, err=%d\n", EFFP_getErrorCode());
    }

    return ret;
}

void mtk_effp_adaptor_register(struct algo_ops *ops)
{
    ops->init = effp_adaptor_init;
    ops->uninit = effp_adaptor_uninit;
    ops->set_params = effp_adaptor_set_params;
    ops->get_params = effp_adaptor_get_params;
    ops->reset = effp_adaptor_reset;
    ops->process = effp_adaptor_process;
}
