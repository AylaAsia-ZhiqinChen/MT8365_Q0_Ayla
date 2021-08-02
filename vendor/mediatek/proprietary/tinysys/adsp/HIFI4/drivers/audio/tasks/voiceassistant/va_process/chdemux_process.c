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
#include "chdemux_process.h"
#include "audio_task.h"
#include "mtk_heap.h"

NORMAL_SECTION_FUNC static int chdemux_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    struct chdemux_config *config = (struct chdemux_config *)(obj->priv);

    if (in.chnum < config->out_chnum)
        return -1;
    config->in_chnum = in.chnum;
    config->in_bitwidth = in.bitwidth;

    if (out != NULL) {
        out->chnum = config->out_chnum;
        out->bitwidth = config->out_bitwidth;
        out->rate = in.rate;
    }

    obj->state = PROC_STAT_INIT;
    return 0;
}

NORMAL_SECTION_FUNC static int chdemux_uninit(struct va_proc_obj *obj)
{
    obj->state = PROC_STAT_UNINIT;
    return 0;
}

static int chdemux_set_params(struct va_proc_obj *obj, int cmd, void *data)
{
    struct chdemux_config *config = (struct chdemux_config *)(obj->priv);
    int ret = 0;
    int i;

    switch (cmd) {
    case CMD_SET_CHDEMUX_OUT:
        {
            struct chdemux_out *out;
            if (data == NULL) {
                ret = -22;
                break;
            }
            if (obj->state != PROC_STAT_UNINIT)
                break;
            out = (struct chdemux_out *)(data);
            config->out_chnum = out->out_chnum;
            for (i = 0; i < config->out_chnum; i++)
                config->out_chidx[i] = out->out_chidx[i];
            config->out_bitwidth = out->bitwidth;
        }
        break;
    default:
        ret = -22;
        break;
    }

    return ret;
}

static int chdemux_process(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames)
{
    struct chdemux_config *config = (struct chdemux_config *)(obj->priv);
    int i, j;

    if (obj->state != PROC_STAT_INIT)
        return -1;

    if (config->in_bitwidth == 16) {
        int16_t *in = (int16_t *)(inbuf);
        if (config->out_bitwidth == 16) {
            int16_t *out = (int16_t *)(outbuf);

            for (i = 0; i < frames; i++) {
                for(j = 0; j < config->out_chnum; j++)
                    *(out + i * config->out_chnum + j) =
                        *(in + i * config->in_chnum + config->out_chidx[j]);
            }
        } else if (config->out_bitwidth == 32) {
            int32_t *out = (int32_t *)(outbuf);

            for (i = 0; i < frames; i++) {
                for(j = 0; j < config->out_chnum; j++)
                    *(out + i * config->out_chnum + j) =
                        ((int32_t)(*(in + i * config->in_chnum + config->out_chidx[j])))<<16;
            }
        }
    } else if (config->in_bitwidth == 32) {
        int32_t *in = (int32_t *)(inbuf);
        if (config->out_bitwidth == 32) {
            int32_t *out = (int32_t *)(outbuf);

            for (i = 0; i < frames; i++) {
                for(j = 0; j < config->out_chnum; j++)
                    *(out + i * config->out_chnum + j) =
                        *(in + i * config->in_chnum + config->out_chidx[j]);
            }
        } else if (config->out_bitwidth == 16) {
            int16_t *out = (int16_t *)(outbuf);

            for (i = 0; i < frames; i++) {
                for(j = 0; j < config->out_chnum; j++)
                    *(out + i * config->out_chnum + j) =
                        (int16_t)((*(in + i * config->in_chnum + config->out_chidx[j]))>>16);
            }
        }
    }

    return 0;
}

NORMAL_SECTION_FUNC int chdemux_proc_create(struct va_proc_obj **obj)
{
    struct va_proc_obj *chdemux;
    int mem_type = get_adsp_heap_type(ADSP_MEM_LP_CACHE);

    chdemux =
        (struct va_proc_obj *)(MTK_pvPortMalloc(sizeof(struct va_proc_obj), mem_type));
    memset(chdemux, 0, sizeof(struct va_proc_obj));
    chdemux->type = VA_PROC_CHDEMUX;
    chdemux->priv =
        (struct chdemux_config *)(MTK_pvPortMalloc(sizeof(struct chdemux_config), mem_type));
    configASSERT(chdemux->priv != NULL);
    memset(chdemux->priv, 0, sizeof(struct chdemux_config));

    chdemux->init = chdemux_init;
    chdemux->uninit = chdemux_uninit;
    chdemux->process = chdemux_process;
    chdemux->set_params = chdemux_set_params;
    chdemux->state = PROC_STAT_UNINIT;

    *obj = chdemux;
    return 0;
}

NORMAL_SECTION_FUNC int chdemux_proc_destroy(struct va_proc_obj *obj)
{
    if (obj->priv)
        MTK_vPortFree(obj->priv);
    MTK_vPortFree(obj);
    return 0;
}
