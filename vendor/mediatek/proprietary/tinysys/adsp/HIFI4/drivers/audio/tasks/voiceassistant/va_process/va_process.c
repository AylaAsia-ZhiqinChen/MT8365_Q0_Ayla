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
#include "va_state.h"
#include "dsp_state.h" 
#include "va_process.h"
#include "vad_process.h"
#include "preproc_process.h"
#include "ww_process.h"
#include "chdemux_process.h"
#include "audio_shared_info.h"
#include "mtk_heap.h"
#include "audio_task.h"
#include <errno.h>
#include "adsp_excep.h"

#include "mt_printf.h"
#include "systimer.h"

#ifdef DUMP_TO_HOST_SUPPORT
#include "audio_dump_helper.h"
#endif

int va_proc_buf_create(struct va_proc_buf *proc_buf,
    struct va_pcm_format format, int frames, int is_ring_buf, int mem_type, void *task)
{
    int heap_type;
    IO_BUF_CTRL_INIT_PARAM params;
    AUDIO_COMMON_TASK_PRIVATE* priv = (AUDIO_COMMON_TASK_PRIVATE*)(task);

    proc_buf->rate = format.rate;
    proc_buf->chnum = format.chnum;
    proc_buf->bitwidth = format.bitwidth;
    proc_buf->frame_size = proc_buf->chnum * proc_buf->bitwidth / 8;
    proc_buf->buf_size = frames * proc_buf->frame_size;
    proc_buf->mem_type = mem_type;
    /* how to warp buffer alloca interface */
    if (mem_type != ADSP_MEM_TASK_RESERVE) {
        heap_type = get_adsp_heap_type(mem_type);
        proc_buf->buf = (char *)MTK_pvPortMalloc(proc_buf->buf_size, heap_type);
    } else {
        proc_buf->buf = task_reserve_pool_alloc(&priv->lpmem_pool, proc_buf->buf_size);
    }
    configASSERT(proc_buf->buf!=NULL);

    proc_buf->is_ring_buf = is_ring_buf;
    if (is_ring_buf) {
        params.buffer_dir = RING_BUF_TYPE_RECORD; /* now fix to record */
        params.start_addr = (void *)proc_buf->buf;
        params.size_bytes = proc_buf->buf_size;
        params.reset_to_full = 0;
        ring_buf_init(&(proc_buf->ring_buf), &params, RING_BUF_LOCAL_THREAD);
        proc_buf->cur_in = proc_buf->cur_out = NULL;
    } else {
        proc_buf->cur_in = proc_buf->cur_out = proc_buf->buf;
    }

    return 0;
}

int va_proc_buf_destroy(struct va_proc_buf *proc_buf, void *task)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = (AUDIO_COMMON_TASK_PRIVATE*)(task);
    if (proc_buf->buf == NULL)
        return 0;

    if (proc_buf->mem_type != ADSP_MEM_TASK_RESERVE) {
        MTK_vPortFree(proc_buf->buf);
    } else {
        task_reserve_pool_free(&priv->lpmem_pool, proc_buf->buf);
    }
    memset(proc_buf, 0, sizeof(struct va_proc_buf));
    return 0;
}

int va_proc_buf_obtain_buffer(struct va_proc_buf *proc_buf,
    char **buf, int size_frames, int flag)
{
    int ret = 0;
    int cur_to_end;

    configASSERT(proc_buf!=NULL);
    configASSERT(proc_buf->buf!=NULL);

    *buf = NULL;
    if (!proc_buf->is_ring_buf) {
        *buf = proc_buf->buf;
        ret = (int)proc_buf->buf_size / (int)proc_buf->frame_size;
        return ret;
    }
    if (flag == VA_PROC_BUF_WRITE) {
        *buf = (char*)ring_buf_get_hw_ptr(&(proc_buf->ring_buf));
        ret = (int)ring_buf_get_remain_space(&(proc_buf->ring_buf));
        cur_to_end = (int)(ring_buf_get_size(&(proc_buf->ring_buf)) -
            ring_buf_get_hw_ofs(&(proc_buf->ring_buf)));
    } else {
        *buf = (char*)ring_buf_get_appl_ptr(&(proc_buf->ring_buf));
        ret = (int)ring_buf_get_data_watermark(&(proc_buf->ring_buf));
        cur_to_end = (int)(ring_buf_get_size(&(proc_buf->ring_buf)) -
            ring_buf_get_appl_ofs(&(proc_buf->ring_buf)));
    }

    ret /= proc_buf->frame_size;
    cur_to_end /= proc_buf->frame_size;

    if (ret > size_frames)
        ret = size_frames;

    if (ret > cur_to_end)
        ret = cur_to_end;

    return ret;
}

int va_proc_buf_release_buffer(struct va_proc_buf *proc_buf,
    char *buf, int size_frames, int flag)
{
    int ret = 0;
    char *cur_ptr;
    uint32_t data_size, buf_size;
    uint32_t read_offset, write_offset;
    uint32_t size_bytes;

    configASSERT(proc_buf!=NULL);
    configASSERT(proc_buf->buf!=NULL);

    if (buf == NULL || (size_frames == 0))
        return ret;

    if (!proc_buf->is_ring_buf)
        return (int)proc_buf->buf_size / (int)proc_buf->frame_size;

    if (flag == VA_PROC_BUF_WRITE)
        cur_ptr = (char *)ring_buf_get_hw_ptr(&(proc_buf->ring_buf));
    else
        cur_ptr = (char *)ring_buf_get_appl_ptr(&(proc_buf->ring_buf));
    configASSERT(cur_ptr==buf);

    size_bytes = (uint32_t)va_proc_buf_frames_to_bytes(proc_buf, size_frames);

    buf_size = ring_buf_get_size(&(proc_buf->ring_buf));
    read_offset = ring_buf_get_appl_ofs(&(proc_buf->ring_buf));

    if (flag == VA_PROC_BUF_WRITE) {
        data_size = ring_buf_get_data_watermark(&(proc_buf->ring_buf));
        write_offset = ring_buf_get_hw_ofs(&(proc_buf->ring_buf));
        if (data_size + size_bytes > buf_size) {
            /* push read point first */
            uint32_t read_size;
            read_size = (data_size + size_bytes) - buf_size;
            read_offset = (read_offset + read_size) % buf_size;
            ring_buf_set_appl_ofs(&(proc_buf->ring_buf), read_offset);
        }
        write_offset = (write_offset + size_bytes) % buf_size;
        ring_buf_set_hw_ofs(&(proc_buf->ring_buf), write_offset);
        proc_buf->cur_in = buf;
        ret = size_frames;
    } else {
        read_offset = (read_offset + size_bytes) % buf_size;
        ring_buf_set_appl_ofs(&(proc_buf->ring_buf), read_offset);
        proc_buf->cur_out = buf;
        ret = size_frames;
    }
    return ret;
}

int va_proc_buf_get_remain_size(struct va_proc_buf *proc_buf, int flag)
{
    int ret;

    if (!proc_buf->is_ring_buf)
        return (int)proc_buf->buf_size / (int)proc_buf->frame_size;

    if (flag == VA_PROC_BUF_WRITE) {
        ret = (int)ring_buf_get_remain_space(&(proc_buf->ring_buf));
    } else {
        ret = (int)ring_buf_get_data_watermark(&(proc_buf->ring_buf));
    }
    return ret / (int)proc_buf->frame_size;
}

int va_proc_buf_frames_to_bytes(struct va_proc_buf *proc_buf, int frames)
{
    return (int)proc_buf->frame_size * frames;
}

int va_proc_buf_bytes_to_frames(struct va_proc_buf *proc_buf, int bytes)
{
    return bytes/(int)proc_buf->frame_size;
}

#if 0
static int va_process_is_enable(struct va_proc_mgr *mgr, int type)
{
    if (mgr->proc_list[type] != NULL && !(mgr->proc_list[type]->bypass))
        return 1;
    return 0;
}
#endif

static int va_ww_calc_start_pos(struct va_proc_mgr *mgr, struct ww_result *ww_info)
{
    long long delta;
    int start;

    delta = ww_info->cur_idx - ww_info->begin_idx;
    /* TODO add the preroll buffer setting */
    delta += mgr->ww_idx_mgr.pre_roll_size;

    /* if start is not period alignment, force alignment */
    if ((uint32_t)delta % mgr->period_frames != 0)
        delta = (delta / mgr->period_frames + 1) * mgr->period_frames;

    delta *= (long long)mgr->out->frame_size;
    if (ring_buf_get_data_watermark(&(mgr->out->ring_buf)) < (uint32_t)delta)
        start = (int)ring_buf_get_appl_ofs(&(mgr->out->ring_buf));
    else
        start = (int)ring_buf_get_hw_ofs(&(mgr->out->ring_buf)) - (int)delta;
    if (start < 0)
        start += (int)ring_buf_get_size(&(mgr->out->ring_buf));
    return start;
}

NORMAL_SECTION_FUNC static void va_process_ap_notify(struct va_proc_mgr *mgr, struct ww_result *ww_info, int start_pos)
{
    ipi_msg_t ipi_msg_notify;
    struct dsp_ipc_va_notify info;

    if (ww_info == NULL) {
        info.type = VA_NOTIFY_VAD_PASS;
        PRINTF_E("%s [time:%llu] VAD OK to voice upload\n", __func__, read_systimer_stamp_ns());
    } else {
        info.type = VA_NOTIFY_WAKEWORD_PASS;
        strcpy(info.wakeword, ww_info->wakeword);
        /* TODO: need reset CPU share buffer */
        PRINTF_E("%s [time:%llu] WW OK to voice upload\n", __func__, read_systimer_stamp_ns());
        PRINTF_E("wakeword: %s, begin:%llu, end:%llu, cur:%llu, confidence:%d, start:%d, hw ofs:%d\n",
        ww_info->wakeword,
        ww_info->begin_idx,
        ww_info->end_idx,
        ww_info->cur_idx,
        ww_info->confidence,
        start_pos,
        ring_buf_get_hw_ofs(&(mgr->out->ring_buf)));
    }
    audio_send_ipi_msg(&ipi_msg_notify,
                   TASK_SCENE_VA,
                   AUDIO_IPI_LAYER_TO_KERNEL,
                   AUDIO_IPI_PAYLOAD,
                   AUDIO_IPI_MSG_BYPASS_ACK,
                   MSG_TO_HOST_VA_NOTIFY,
                   sizeof(struct dsp_ipc_va_notify),
                   sizeof(struct dsp_ipc_va_notify),
                   &info);
}

static int va_process_for_state_change(struct va_proc_mgr *mgr, int from, int to, int event)
{
    int vad_always_on = CFG_VA_VAD_ALWAYS_ON;
    int ww_always_on = CFG_VA_WW_ALWAYS_ON;

    /* process setting change for jump to VAD */
    if (to == VA_STAT_VAD) {
        if (va_process_is_valid(mgr, VA_PROC_VAD) && (!vad_always_on))
            mgr->proc_list[VA_PROC_VAD]->reset(mgr->proc_list[VA_PROC_VAD]);
        /* TODO reset proc_in_buf => I think do not need to reset in buffer*/
        PRINTF_E("%s [time:%llu] state jump to VAD\n", __func__, read_systimer_stamp_ns());
    }

    /* process setting change for common part */
    if (from == VA_STAT_VAD &&
        (to == VA_STAT_WAKEWORD || to == VA_STAT_VOICEUPLOAD)) {
        PRINTF_E("%s [time:%llu] VAD OK, jump to state(%d)\n", __func__, read_systimer_stamp_ns(), to);
#if (CFG_VA_VAD_WW_IN_ONE == 0) && !defined(CFG_VA_WW_BUF_NO_RESET)
        ring_buf_reset(&(mgr->out->ring_buf), 0);
#endif
        if (va_process_is_valid(mgr, VA_PROC_PREPROC))
            mgr->proc_list[VA_PROC_PREPROC]->reset(mgr->proc_list[VA_PROC_PREPROC]);
    }

    if ((to == VA_STAT_WAKEWORD) && ((ww_always_on && from == VA_STAT_VAD) || (!ww_always_on))) {
        PRINTF_E("%s [time:%llu] wakeword reset\n", __func__, read_systimer_stamp_ns());
        if (va_process_is_valid(mgr, VA_PROC_WW))
            mgr->proc_list[VA_PROC_WW]->reset(mgr->proc_list[VA_PROC_WW]);
    }

    if (from == VA_STAT_VAD && to == VA_STAT_VOICEUPLOAD && event != VA_EVT_FORCE_VOICE_UPLOAD) {
            if (va_process_is_valid(mgr, VA_PROC_PREPROC))
                mgr->proc_list[VA_PROC_PREPROC]->set_params(mgr->proc_list[VA_PROC_PREPROC], CMD_SET_AEC_WW_START, NULL);
            va_process_ap_notify(mgr, NULL, 0);
    } else if (from == VA_STAT_WAKEWORD && to == VA_STAT_VOICEUPLOAD && event != VA_EVT_FORCE_VOICE_UPLOAD) {
            struct ww_result ww_info;
            int start_pos;
#if (CFG_VA_VAD_WW_IN_ONE != 0)
            int proc_type = VA_PROC_VAD;
#else
            int proc_type = VA_PROC_WW;
#endif

            mgr->proc_list[proc_type]->get_params(mgr->proc_list[proc_type], CMD_GET_WW_RESULT, &ww_info);
            start_pos = va_ww_calc_start_pos(mgr, &ww_info);
            /* if start_pos change, drop the data*/
            if (start_pos != (int)ring_buf_get_appl_ofs(&(mgr->out->ring_buf)))
                ring_buf_set_appl_ofs(&(mgr->out->ring_buf), (uint32_t)start_pos);

	    if (va_process_is_valid(mgr, VA_PROC_PREPROC)) {
                void *params_data = NULL;
                /* hardcode need to optimise according to ww_info. */
                struct aec_feedback_setting setting;
                setting.channel_id = 1;
                setting.conf = ww_info.confidence;
                setting.frame_time_diff = 1;
                params_data = &setting;
                mgr->proc_list[VA_PROC_PREPROC]->set_params(mgr->proc_list[VA_PROC_PREPROC], CMD_SET_AEC_WW_OK, params_data);
            }

            /* reset CPU share buffer after va_process*/
            va_process_ap_notify(mgr, &ww_info, start_pos);
    }
    if (from == VA_STAT_VOICEUPLOAD && to == VA_STAT_VOICEUPLOAD && event == VA_EVT_WW_SUCCESS) {
        struct ww_result ww_info;
        int start_pos;
#if (CFG_VA_VAD_WW_IN_ONE != 0)
            int proc_type = VA_PROC_VAD;
#else
            int proc_type = VA_PROC_WW;
#endif

        mgr->proc_list[proc_type]->get_params(mgr->proc_list[proc_type], CMD_GET_WW_RESULT, &ww_info);
        start_pos = va_ww_calc_start_pos(mgr, &ww_info);
        /* just notify, donot change anything */

        va_process_ap_notify(mgr, &ww_info, start_pos);
    }
    if (from == VA_STAT_VOICEUPLOAD && to == VA_STAT_WAKEWORD) {
	PRINTF_E("%s [time:%llu] CMD_SET_AEC_WW_START!!!\n", __func__, read_systimer_stamp_ns());
	mgr->proc_list[VA_PROC_WW]->set_params(mgr->proc_list[VA_PROC_WW], CMD_SET_AEC_WW_START, NULL);
    }
    return 0;
}

/* TODO need manage the init state for multi-record */
NORMAL_SECTION_FUNC int va_process_init(struct va_proc_mgr *mgr, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
    struct va_pcm_format input;
    int need_out = 0;
    int frame_count;
    int ret;

    /* MIC RECORD => VAD => Common Process(AEC/CHDEMUX) => WAKEWORD => VOICE UPLOAD */

    configASSERT(out!=NULL);

    PRINTF_D("%s  start, in ch(%d), bitwidth(%d), rate(%d), frames(%d)\n",
             __func__,  in.chnum, in.bitwidth, in.rate, frames);
    memcpy(out, &in, sizeof(struct va_pcm_format));

    mgr->in = mgr->out = mgr->preproc = NULL;
    mgr->buf_num = 0;

    /* in buffer must be in any time, but if no VAD, just set to 1 period */
    if (mgr->proc_list[VA_PROC_VAD] != NULL) {
        mgr->proc_list[VA_PROC_VAD]->init(mgr->proc_list[VA_PROC_VAD], in, NULL, frames);
        frame_count = CFG_VA_VAD_BUF_LEN * in.rate / 1000;
        frame_count = frame_count / frames + (frame_count%frames != 0);
        frame_count *= frames;
    } else {
        frame_count = frames;
    }
    ret = va_proc_buf_create(&mgr->buf_list[mgr->buf_num], in, frame_count,
              (mgr->proc_list[VA_PROC_VAD] != NULL), CFG_VA_VAD_BUF_TYPE, mgr->priv);
    configASSERT(ret==0);
    mgr->in = &mgr->buf_list[mgr->buf_num];
    mgr->buf_num++;
    PRINTF_D("%s va in buf addr(%p), size(%d), rate(%d), frame_size(%d), ring(%d)\n",
             __func__, mgr->in->buf, mgr->in->buf_size,
             mgr->in->rate, mgr->in->frame_size, mgr->in->is_ring_buf);

    /* Common Pre-Processing Init */
    if (mgr->proc_list[VA_PROC_PREPROC]) {
        mgr->proc_list[VA_PROC_PREPROC]->init(mgr->proc_list[VA_PROC_PREPROC], in, out, frames);
        PRINTF_E("%s preproc output format ch(%d), bitwidth(%d), rate(%d)\n",
                 __func__, out->chnum, out->bitwidth, out->rate);
        if (out->chnum != CFG_VA_VOICE_UPLOAD_CH_NUM) {
            /* TODO not consider sample rate change now */
            frame_count = frames;
            ret = va_proc_buf_create(&mgr->buf_list[mgr->buf_num], *out, frame_count,
                       0, CFG_VA_PREPROC_BUF_TYPE, mgr->priv);
            configASSERT(ret==0);
            mgr->preproc = &mgr->buf_list[mgr->buf_num];
            mgr->buf_num++;
            PRINTF_D("%s va preproc buf addr(%p), size(%d), rate(%d), frame_size(%d), ring(%d)\n",
                 __func__, mgr->preproc->buf, mgr->preproc->buf_size,
                 mgr->preproc->rate, mgr->preproc->frame_size, mgr->preproc->is_ring_buf);

        }
    }

    /* case1. input and output channel number is differnt 
       case2. has ww, but not preproc, chdemux do buffer copy from in to out
    */
    if ((out->chnum != CFG_VA_VOICE_UPLOAD_CH_NUM) ||
        (out->bitwidth != CFG_VA_VOICE_UPLOAD_BITWIDTH) ||
        ((mgr->proc_list[VA_PROC_PREPROC] == NULL) && mgr->proc_list[VA_PROC_WW] != NULL)) {
        struct chdemux_out out_config;

        memcpy(&input, out, sizeof(struct va_pcm_format));
        chdemux_proc_create(&(mgr->proc_list[VA_PROC_CHDEMUX]));
        out_config.bitwidth = CFG_VA_VOICE_UPLOAD_BITWIDTH;
        out_config.out_chnum = CFG_VA_VOICE_UPLOAD_CH_NUM;
        /* TODO now fix output 1st channel */
        int i;
        for (i = 0; i < out_config.out_chnum; i++) {
            out_config.out_chidx[i] = i;
        }
        mgr->proc_list[VA_PROC_CHDEMUX]->set_params(mgr->proc_list[VA_PROC_CHDEMUX], CMD_SET_CHDEMUX_OUT, &out_config);
        mgr->proc_list[VA_PROC_CHDEMUX]->init(mgr->proc_list[VA_PROC_CHDEMUX], input, out, frames);
        PRINTF_E("%s chdemux output format ch(%d), bitwidth(%d), rate(%d)\n",
                 __func__, out->chnum, out->bitwidth, out->rate);
        need_out = 1;
    }

    if (mgr->proc_list[VA_PROC_WW]) {
        memcpy(&input, out, sizeof(struct va_pcm_format));
        mgr->proc_list[VA_PROC_WW]->init(mgr->proc_list[VA_PROC_WW], input, NULL, frames);
        mgr->ww_idx_mgr.pre_roll_size = CFG_VA_WW_PRE_ROLL_LEN *  input.rate / 1000;
        need_out = 1;
    }
#if (CFG_VA_VAD_WW_IN_ONE != 0)
    mgr->ww_idx_mgr.pre_roll_size = CFG_VA_WW_PRE_ROLL_LEN *  in.rate / 1000;
#endif
    if (need_out == 1) {
        if (mgr->proc_list[VA_PROC_WW] != NULL) {
            /* TODO reconfig out buffer size */
            //frame_count = (CFG_VA_WW_PRE_ROLL_LEN + VA_WW_TIMEOUT_LENGTH) * out->rate / 1000;
            frame_count = CFG_VA_WW_BUF_LEN * out->rate / 1000;
            frame_count = frame_count / frames + (frame_count%frames != 0);
            frame_count *= frames;
        } else {
            /* Only WakeWord Exist, the output need a ring buffer to pre-roll wakeword */
            frame_count = frames;
        }
        ret = va_proc_buf_create(&mgr->buf_list[mgr->buf_num], *out, frame_count,
                   1, CFG_VA_WW_BUF_TYPE, mgr->priv);
        configASSERT(ret==0);
        mgr->out = &mgr->buf_list[mgr->buf_num];
        mgr->buf_num++;
        PRINTF_D("%s va out buf addr(%p), size(%d), rate(%d), frame_size(%d), ring(%d)\n",
             __func__, mgr->out->buf, mgr->out->buf_size,
             mgr->out->rate, mgr->out->frame_size, mgr->out->is_ring_buf);

    }

    /* This is the case only has VAD */
    if (mgr->out == NULL)
        mgr->out = mgr->in;

    mgr->vp_flag = 0;
    mgr->period_frames = frames;
#if defined(CFG_VA_WW_TOUT_CONTINUE)
    mgr->ww_tout_continue = 0;
#endif
    mgr->state = PROC_STAT_INIT;
    PRINTF_E("%s [time:%llu] end\n", __func__, read_systimer_stamp_ns());
    return 0;
}

NORMAL_SECTION_FUNC int va_process_uninit(struct va_proc_mgr *mgr)
{
    int i;

    PRINTF_E("%s [time:%llu] start\n", __func__, read_systimer_stamp_ns());
    /* VAD unInit */
    for (i = 0; i < VA_PROC_NUM; i++) {
        if (mgr->proc_list[i])
            mgr->proc_list[i]->uninit(mgr->proc_list[i]);
    }

    if (mgr->proc_list[VA_PROC_CHDEMUX]) {
        chdemux_proc_destroy(mgr->proc_list[VA_PROC_CHDEMUX]);
        mgr->proc_list[VA_PROC_CHDEMUX] = NULL;
    }

    for (i = 0; i < mgr->buf_num; i++) {
        va_proc_buf_destroy(&(mgr->buf_list[i]), mgr->priv);
    }
    mgr->in = mgr->out = mgr->preproc = NULL;
    mgr->buf_num = 0;

    mgr->vp_flag = 0;
    mgr->state = PROC_STAT_UNINIT;
    return 0;
}

int va_process_reset(struct va_proc_mgr *mgr)
{
    if (mgr->state != PROC_STAT_INIT)
        return -1;

    if (mgr->in && mgr->in->is_ring_buf)
        ring_buf_reset(&(mgr->in->ring_buf), 0);

    if (mgr->out && mgr->out->is_ring_buf)
        ring_buf_reset(&(mgr->out->ring_buf), 0);

    return 0;
}

static inline int va_vad_ww_process_ret_to_event(int ret_val, int state)
{
    int event;
    int ww_always_on = CFG_VA_WW_ALWAYS_ON;

    if (state == VA_STAT_VAD)
        event = (ret_val==VA_VAD_SUCESS) ? VA_EVT_VAD_SUCCESS : VA_EVT_VAD_FAIL;

    if (state == VA_STAT_WAKEWORD) {
        if (ret_val == VA_WW_CONTINUE)
            event = VA_EVT_WW_CONTINUE;
        else if (ret_val == VA_WW_SUCESS)
            event = VA_EVT_WW_SUCCESS;
#if (CFG_VA_VAD_WW_IN_ONE != 0)
        else if (ret_val == VA_VAD_FAIL)
            event = VA_WW_FAIL;
#endif
        else
            event = VA_WW_FAIL;
    }

    if ((ww_always_on) && (state == VA_STAT_VOICEUPLOAD) && (ret_val == VA_WW_SUCESS))
        event = VA_EVT_WW_SUCCESS;

    return event;
}

static inline int va_voice_upload_is_force_on(struct va_proc_mgr *mgr)
{
    return (int)((mgr->vp_flag & VA_VOICE_UPLOAD_FORCE_ON) != 0);
}

static inline int va_voice_upload_is_done(struct va_proc_mgr *mgr)
{
    return (int)((mgr->vp_flag & VA_VOICE_UPLOAD_DONE) != 0);
}

int va_process(struct va_proc_mgr *mgr, char *buf, int frames)
{
    int state, event;
    char *in;
    char *out;
    struct va_proc_obj *proc_obj = NULL;
    int vad_always_on = CFG_VA_VAD_ALWAYS_ON;
    int ww_always_on = CFG_VA_WW_ALWAYS_ON;
    int is_need_proc;
    int vad_event = -1;
    int ret = 0;

    state = va_state_get();

    is_need_proc = (buf != NULL) && va_process_is_valid(mgr, VA_PROC_VAD) &&
        (vad_always_on || (!vad_always_on && (state == VA_STAT_VAD) && va_state_is_valid(VA_STAT_VAD)));

    /* VAD Data Process */
    if (is_need_proc) {
        out = in = buf;
        proc_obj = mgr->proc_list[VA_PROC_VAD];
        ret = proc_obj->process(mgr->proc_list[VA_PROC_VAD], in, NULL, frames);
        /* TODO noise level checking */
        event = va_vad_ww_process_ret_to_event(ret, state);
        vad_event = va_vad_ww_process_ret_to_event(ret, VA_STAT_VAD);
    }
    /* State Process */
    if (state == VA_STAT_VAD) {
        if (!va_state_is_valid(VA_STAT_VAD))
            event = VA_EVT_VAD_DISABLE;

        if (va_voice_upload_is_force_on(mgr)) {
            event = VA_EVT_FORCE_VOICE_UPLOAD;
            va_process_voice_upload_set(mgr, VA_CLEAR, VA_VOICE_UPLOAD_FORCE_ON);
        }
        va_state_event_proc(event);
        state = va_state_get();

        if (state != VA_STAT_VAD)
            va_process_for_state_change(mgr, VA_STAT_VAD, state, event);
        else
            return 0;
    }

    if (state == VA_STAT_WAKEWORD || state == VA_STAT_VOICEUPLOAD) {
        /* common process */
        int obtain_frames;
        int is_in_avail = (int)(va_process_is_valid(mgr, VA_PROC_WW) ||
                          va_process_is_valid(mgr, VA_PROC_PREPROC) ||
                          va_process_is_valid(mgr, VA_PROC_CHDEMUX));

        if (is_in_avail) {
            /* for wakeword and voiceupload, the input is from ring buf in read point */
            obtain_frames = va_proc_buf_obtain_buffer(mgr->in, &in, frames, VA_PROC_BUF_READ);
            if (obtain_frames < frames)
                return VA_IN_NODATA; /* input buffer is not enough */
            va_proc_buf_release_buffer(mgr->in, in, frames, VA_PROC_BUF_READ);
            out = in;
        }

        if (va_process_is_valid(mgr, VA_PROC_PREPROC)) {
            struct va_proc_buf *proc_buf;

            if (mgr->preproc)
                proc_buf = mgr->preproc;
            else
                proc_buf = mgr->out;

            obtain_frames = va_proc_buf_obtain_buffer(proc_buf, &out, frames, VA_PROC_BUF_WRITE);
#if 0
            /* TODO need check write buffer size, if out buffer is not enough, need overwrite */
            /* if in ww stage, there may be overwrite */
            /* Do we need to notify warnning log? */
            if (obtain_frames < frames) {
            }
#endif
            /* make sure the in & out buffer is period align */
            mgr->proc_list[VA_PROC_PREPROC]->process(mgr->proc_list[VA_PROC_PREPROC], in, out, frames);
            va_proc_buf_release_buffer(proc_buf, out, frames, VA_PROC_BUF_WRITE);
        }

        if (va_process_is_valid(mgr, VA_PROC_CHDEMUX)) {
            in = out;

            obtain_frames = va_proc_buf_obtain_buffer(mgr->out, &out, frames, VA_PROC_BUF_WRITE);
#if 0
            /* TODO need check write buffer size */
            /* if in ww stage, there may be overwrite */
            /* Do we need to notify warnning log? */
            if (obtain_frames < frames) {
            }
#endif
            mgr->proc_list[VA_PROC_CHDEMUX]->process(mgr->proc_list[VA_PROC_CHDEMUX], in, out, frames);

            va_proc_buf_release_buffer(mgr->out, out, frames, VA_PROC_BUF_WRITE);
        }
    }

    is_need_proc = va_process_is_valid(mgr, VA_PROC_WW) &&
        ((ww_always_on && (state == VA_STAT_WAKEWORD || state == VA_STAT_VOICEUPLOAD)) ||
         (!ww_always_on && (state == VA_STAT_WAKEWORD)));
    /* Data Process */
    if (is_need_proc) {
        in = out;
        proc_obj = mgr->proc_list[VA_PROC_WW];
        /* make sure the in & out buffer is period align */
        ret = proc_obj->process(mgr->proc_list[VA_PROC_WW], in, NULL, frames);
        event = va_vad_ww_process_ret_to_event(ret, state);
    }

    /* State Process */
    if (ww_always_on &&
        (event == VA_EVT_WW_SUCCESS) &&
        (state == VA_STAT_VOICEUPLOAD))
            va_process_for_state_change(mgr, VA_STAT_VOICEUPLOAD, VA_STAT_VOICEUPLOAD, VA_EVT_WW_SUCCESS);
    if (state == VA_STAT_WAKEWORD) {
        int ww_tout = va_state_get_ww_timeout();
        /* wakeword process */
        if (ww_tout && (vad_event == -1)) {
            va_state_clr_ww_timeout();
            event = VA_EVT_WW_TIMEOUT;
        } else if (ww_tout && (vad_event == VA_EVT_VAD_FAIL)) {
#if defined(CFG_VA_WW_TOUT_CONTINUE)
            if (mgr->ww_tout_continue++ >= CFG_VA_WW_TOUT_CONTINUE) {
#endif
            va_state_clr_ww_timeout();
            event = VA_EVT_WW_TIMEOUT;
#if defined(CFG_VA_WW_TOUT_CONTINUE)
            mgr->ww_tout_continue = 0;
            }
#endif
        } else if (ww_tout && (vad_event == VA_EVT_VAD_SUCCESS)) {
#if defined(CFG_VA_WW_TOUT_CONTINUE)
            mgr->ww_tout_continue = 0;
#endif
        }

        if (va_voice_upload_is_force_on(mgr)) {
            event = VA_EVT_FORCE_VOICE_UPLOAD;
            va_process_voice_upload_set(mgr, VA_CLEAR, VA_VOICE_UPLOAD_FORCE_ON);
        }

        if (event == VA_EVT_WW_FAIL || event == VA_EVT_WW_TIMEOUT) {
#ifdef DSP_AFE_LP26M_SUPPORT
            mgr->afe_reset_event = 1;
#endif
        }
        va_state_event_proc(event);
        state = va_state_get();
        if (state != VA_STAT_WAKEWORD)
            va_process_for_state_change(mgr, VA_STAT_WAKEWORD, state, event);
    }

    if (state == VA_STAT_VOICEUPLOAD) {
        if (va_voice_upload_is_force_on(mgr))
            va_process_voice_upload_set(mgr, VA_CLEAR, VA_VOICE_UPLOAD_FORCE_ON);

        /* TODO if need check event at first? */
        if (va_voice_upload_is_done(mgr)) {
            va_process_voice_upload_set(mgr, VA_CLEAR, VA_VOICE_UPLOAD_DONE);
#ifdef DSP_AFE_LP26M_SUPPORT
            mgr->afe_reset_event = 1;
#endif
            va_state_event_proc(VA_EVT_VOICE_UPLOAD_DONE);
            state = va_state_get();
            if (state != VA_STAT_VOICEUPLOAD)
                    va_process_for_state_change(mgr, VA_STAT_VOICEUPLOAD, state, event);
        }
    }

    return 0;
}

NORMAL_SECTION_FUNC int va_process_create(struct va_proc_mgr *mgr, int type)
{
    int ret = -EINVAL;
    int state = -EINVAL;

    if ((mgr->proc_list[type] != 0) && (mgr->va_setting & (1<<type)))
        return -EINVAL;

    if (type == VA_PROC_VAD) {
        ret = vad_proc_create(&(mgr->proc_list[type]));
        state = VA_STAT_VAD;
    } else if (type == VA_PROC_PREPROC) {
        ret = preproc_proc_create(&(mgr->proc_list[type]));
    } else if (type == VA_PROC_WW) {
    /* for VAD WW process in one positon, not support wakeword process */
#if (CFG_VA_VAD_WW_IN_ONE == 0)
        ret = ww_proc_create(&(mgr->proc_list[type]));
        state = VA_STAT_WAKEWORD;
#endif
    }

    if (ret == 0)
        mgr->va_setting |= (1<<type);

    if (ret == 0 && state >= 0) {
        va_state_enable(state, 1);
#if (CFG_VA_VAD_WW_IN_ONE != 0)
        if (state == VA_STAT_VAD)
            va_state_enable(VA_STAT_WAKEWORD, 1);
#endif
    }
    PRINTF_E("%s process(%d) is create, setting(%x)\n", __func__, type, mgr->va_setting);
    return 0;
}

NORMAL_SECTION_FUNC int va_process_destroy(struct va_proc_mgr *mgr, int type)
{
    int ret;
    int state = -EINVAL;

    if ((mgr->proc_list[type] == 0) || !(mgr->va_setting & (1<<type)))
        return -EINVAL;

    if (type == VA_PROC_VAD) {
        ret = vad_proc_destroy(mgr->proc_list[type]);
        state = VA_STAT_VAD;
    } else if (type == VA_PROC_PREPROC) {
        ret = preproc_proc_destroy(mgr->proc_list[type]);
    } else if (type == VA_PROC_WW)  {
    /* for VAD WW process in one positon, not support wakeword process */
#if (CFG_VA_VAD_WW_IN_ONE == 0)
        ret = ww_proc_destroy(mgr->proc_list[type]);
        state = VA_STAT_WAKEWORD;
#endif
    }

    if (state >= 0) {
        va_state_enable(state, 0);
        mgr->proc_list[type] = NULL;
        mgr->va_setting &= ~(1<<type);
#if (CFG_VA_VAD_WW_IN_ONE != 0)
        if (state == VA_STAT_VAD)
            va_state_enable(VA_STAT_WAKEWORD, 0);
#endif
    }
    PRINTF_E("%s process(%d) is destroy, setting(%x)\n", __func__, type, mgr->va_setting);
    return ret;
}

int va_set_params(struct va_proc_mgr *mgr, int cmd, void *params)
{
    int ret = 0;

    PRINTF_E("%s cmd(%d) is set\n", __func__, cmd);
    switch(cmd) {
    case CMD_SET_AEC_CH_CONFIG:
    case CMD_SET_BYPASS_PREPROC:
    case CMD_CLR_BYPASS_PREPROC:
    case CMD_SET_AEC_WW_START:
        if (mgr->proc_list[VA_PROC_PREPROC]) {
            ret = mgr->proc_list[VA_PROC_PREPROC]->set_params(mgr->proc_list[VA_PROC_PREPROC], cmd, params);
        }
        break;
    case CMD_SET_VAD_FORCE_OK:
    case CMD_SET_VAD_FORCE_FAIL:
    case CMD_CLR_VAD_FORCE_FAIL:
        if (mgr->proc_list[VA_PROC_VAD]) {
            ret = mgr->proc_list[VA_PROC_VAD]->set_params(mgr->proc_list[VA_PROC_VAD], cmd, params);
        }
        break;
    case CMD_SET_WW_FORCE_OK:
    case CMD_SET_WW_FORCE_FAIL:
    case CMD_SET_WW_FORCE_CONTINUE:
    case CMD_CLR_WW_FORCE_CONTINUE:
    case CMD_SET_WW_LOAD_MODEL:
#if (CFG_VA_VAD_WW_IN_ONE == 0)
        if (mgr->proc_list[VA_PROC_WW]) {
            ret = mgr->proc_list[VA_PROC_WW]->set_params(mgr->proc_list[VA_PROC_WW], cmd, params);
        }
#else
        if (mgr->proc_list[VA_PROC_VAD]) {
            ret = mgr->proc_list[VA_PROC_VAD]->set_params(mgr->proc_list[VA_PROC_VAD], cmd, params);
        }
#endif
        break;
    default:
        break;
    }
    return ret;
}

int va_get_params(struct va_proc_mgr *mgr, int cmd, void *params)
{
    int ret = 0;

    PRINTF_E("%s cmd(%d) is get\n", __func__, cmd);
    switch(cmd) {
    case CMD_GET_BEAMFORMING_RESULT:
    case CMD_GET_BYPASS_PREPROC:
        if (mgr->proc_list[VA_PROC_PREPROC]) {
            ret = mgr->proc_list[VA_PROC_PREPROC]->get_params(mgr->proc_list[VA_PROC_PREPROC], cmd, params);
        }
        break;
    default:
        break;
    }
    return ret;
}

int va_get_voice_upload_buffer(struct va_proc_mgr *mgr, void **out, int frames, int type)
{
    char *read_ptr;
    struct va_proc_buf *proc_buf;
    int obtain_frames;
    int ret;

    /* TODO other type support */
    if (type == DAI_VA_RECORD_TYPE) {
        if (mgr->out)
            proc_buf = mgr->out;
        else
            proc_buf = mgr->in;

        if (va_proc_buf_get_remain_size(proc_buf, VA_PROC_BUF_READ) < frames)
            return -ENOMEM;

        obtain_frames = va_proc_buf_obtain_buffer(proc_buf, &read_ptr, frames, VA_PROC_BUF_READ);
        *out = (void *)read_ptr;
         ret = obtain_frames;
    }
    return ret;
}

int va_update_voice_upload_buffer(struct va_proc_mgr *mgr, void *buf, int frames, int type)
{
    struct va_proc_buf *proc_buf;

    /* TODO other type support */
    if (type == DAI_VA_RECORD_TYPE) {
        if (mgr->out)
            proc_buf = mgr->out;
        else
            proc_buf = mgr->in;
        va_proc_buf_release_buffer(proc_buf, (char *)buf, frames, VA_PROC_BUF_READ);
    }
    return 0;
}

int va_process_is_valid(struct va_proc_mgr *mgr, int type)
{
    if (mgr->proc_list[type] != NULL)
        return 1;

    return 0;
}

void va_process_voice_upload_set(struct va_proc_mgr *mgr, int type, int flag)
{
    if (type == VA_SET)
        mgr->vp_flag |= flag;
    else
        mgr->vp_flag &= ~flag;
}

/* TODO for VAD=>WW Latency Reduce */
int va_process_support_speed_up(struct va_proc_mgr *mgr, int frames)
{
    if (mgr->in == mgr->out)
        return 0;
    if ((!mgr->in->is_ring_buf) || (!mgr->out->is_ring_buf))
        return 0;
    if (va_state_get() != VA_STAT_WAKEWORD)
        return 0;
    if (va_proc_buf_get_remain_size(mgr->in, VA_PROC_BUF_READ) < frames)
        return 0;
    if (va_proc_buf_get_remain_size(mgr->out, VA_PROC_BUF_WRITE) < frames)
        return 0;
    return 1;
}
