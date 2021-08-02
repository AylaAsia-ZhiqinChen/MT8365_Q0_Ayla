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

#ifndef _VA_PROCESS_H_
#define _VA_PROCESS_H_
#include "audio_rtos_header_group.h"
#include "audio_task.h"
#include "audio_task_io_buf_ctrl.h"

#ifdef CFG_MCPS_CALC_SUPPORT
#include "cli.h"
#include <xtensa/tie/xt_timer.h>

struct mcps_calc {
    uint64_t count;
    uint64_t total_cc;
    uint32_t max;
    uint32_t min;
    uint32_t start;
    uint32_t stop;
};

#define DEFINE_MCPS_CALC(a)  struct mcps_calc a;
#define REFER_MCPS_CALC(a)   extern struct mcps_calc a;
static inline void mcps_calc_init(struct mcps_calc *a)
{
    a->count = 0;
    a->total_cc = 0;
    a->max = 0;
    a->min = 0xFFFFFFFF;
}
static inline void mcps_calc_before_proc(struct mcps_calc *a)
{
    a->start = XT_RSR_CCOUNT();
}
static inline void mcps_calc_after_proc(struct mcps_calc *a)
{
    uint32_t mcps_delta;

    a->stop = XT_RSR_CCOUNT();
    if (a->start <= a->stop)
        mcps_delta = a->stop - a->start;
    else
        mcps_delta = 0xFFFFFFFF - (a->start - a->stop);
    a->count++;
    a->total_cc += (uint64_t)(mcps_delta);
    if (a->max < mcps_delta)
        a->max = mcps_delta;
    if (a->min > mcps_delta)
        a->min = mcps_delta;
}
static inline void mcps_calc_dump(struct mcps_calc a)
{
    FreeRTOS_CLIPrintf("count:%llu, cc:%llu, avg:%d, max:%d, min:%d\n",
             a.count,
             a.total_cc,
             (uint32_t)(a.total_cc / a.count),
             a.max,
             a.min);
}
#endif

#define VA_MAX_BUF      3

enum {
    VA_PROC_VAD = 0,
    VA_PROC_PREPROC,
    VA_PROC_WW,
    VA_PROC_CHDEMUX,
    VA_PROC_NUM,
};

enum {
    VA_ERROR = 1,
    VA_VAD_FAIL,
    VA_VAD_SUCESS,
    VA_WW_CONTINUE,
    VA_WW_SUCESS,
    VA_WW_FAIL,
};

enum {
    VA_SET = 0,
    VA_CLEAR,
};

enum {
    VA_VOICE_UPLOAD_DONE = 0x1,
    VA_VOICE_UPLOAD_FORCE_ON = 0x2,
};

enum {
    PROC_STAT_UNINIT = 0,
    PROC_STAT_INIT,
};

enum {
    CMD_GET_WW_RESULT,
    CMD_GET_BEAMFORMING_RESULT,
    CMD_GET_BYPASS_PREPROC,

    CMD_SET_AEC_CH_CONFIG,
    CMD_SET_AEC_WW_OK,
    CMD_SET_AEC_WW_START,
    CMD_SET_WW_CONFIG,
    CMD_SET_CHDEMUX_OUT,
    CMD_SET_WW_LOAD_MODEL,

    /* Following is for test */
    CMD_SET_VAD_FORCE_OK,
    CMD_SET_VAD_FORCE_FAIL,
    CMD_CLR_VAD_FORCE_FAIL,
    CMD_SET_WW_FORCE_OK,
    CMD_SET_WW_FORCE_FAIL,
    CMD_SET_WW_FORCE_CONTINUE,
    CMD_CLR_WW_FORCE_CONTINUE,
    CMD_SET_BYPASS_PREPROC,
    CMD_CLR_BYPASS_PREPROC
};

enum {
    VA_PROC_BUF_WRITE = 0x1,
    VA_PROC_BUF_READ = 0x2,
};

enum {
    VA_IN_NODATA = -500,
    VA_OUT_NOMEM = -501,
};

struct va_pcm_format {
    uint32_t rate;
    uint8_t chnum;
    uint8_t bitwidth;
};

struct va_ww_idx_mgr {
    uint32_t pre_roll_size;
    uint32_t cur_index_offset;
    uint32_t buffer_size;
};

struct aec_ch_setting {
    uint32_t in_chnum;
    uint32_t mics_chnum;
    uint32_t refs_chnum;
    uint32_t out_chnum;
};

struct ww_result {
    long long begin_idx;
    long long end_idx;
    long long cur_idx;

    const char *wakeword;
    int confidence;
};

struct va_proc_buf {
    char *buf;
    int is_ring_buf;
    int mem_type;
    IO_RING_BUF ring_buf;
    char *cur_in;
    char *cur_out;
    uint32_t buf_size;
    uint32_t frame_size;
    uint32_t rate;
    uint8_t chnum;
    uint8_t bitwidth;
};

struct va_proc_obj {
    int type;
    int state;
    int bypass;
    void *priv;
    int (*init)(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames);
    int (*uninit)(struct va_proc_obj *obj);
    int (*reset)(struct va_proc_obj *obj);
    int (*set_params)(struct va_proc_obj *obj, int cmd, void *data);
    int (*get_params)(struct va_proc_obj *obj, int cmd, void *data);
    int (*process)(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames);
};

struct algo_ops {
    int (*init)(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames);
    int (*uninit)(struct va_proc_obj *obj);
    int (*reset)(struct va_proc_obj *obj);
    int (*set_params)(struct va_proc_obj *obj, int cmd, void *data);
    int (*get_params)(struct va_proc_obj *obj, int cmd, void *data);
    int (*process)(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames);
};

/* TODO need manage the init state for multi-record */
struct va_proc_mgr {
    int state;
    int va_setting;

    int vp_flag;

    struct va_proc_obj *proc_list[VA_PROC_NUM];
    struct va_ww_idx_mgr ww_idx_mgr;

    struct va_proc_buf *in;
    struct va_proc_buf *out;
    struct va_proc_buf *preproc;

    int buf_num;
    struct va_proc_buf buf_list[VA_MAX_BUF];

#ifdef DSP_AFE_LP26M_SUPPORT
    int afe_reset_event;
    int afe_source;
#endif
    uint32_t period_frames;
#if defined(CFG_VA_WW_TOUT_CONTINUE)
    uint32_t ww_tout_continue;
#endif
    void *priv; /* point to task private */
};

static inline int va_process_state(struct va_proc_mgr *mgr)
{
    return mgr->state;
}

int va_proc_buf_create(struct va_proc_buf *proc_buf, struct va_pcm_format format, int frames, int is_ring_buf, int mem_type, void *task);
int va_proc_buf_destroy(struct va_proc_buf *buf, void *task);
int va_proc_buf_obtain_buffer(struct va_proc_buf *proc_buf, char **buf, int size_frames, int flag);
int va_proc_buf_release_buffer(struct va_proc_buf *proc_buf, char *buf, int size_frames, int flag);
int va_proc_buf_get_remain_size(struct va_proc_buf *proc_buf, int flag);
int va_proc_buf_frames_to_bytes(struct va_proc_buf *proc_buf, int frames);
int va_proc_buf_bytes_to_frames(struct va_proc_buf *proc_buf, int bytes);

int va_process_init(struct va_proc_mgr *mgr, struct va_pcm_format in, struct va_pcm_format *out, int frames);
int va_process_uninit(struct va_proc_mgr *mgr);
int va_process_reset(struct va_proc_mgr *mgr);
int va_process(struct va_proc_mgr *mgr, char *buf, int frames);
int va_set_params(struct va_proc_mgr *mgr, int cmd, void *params);
int va_get_params(struct va_proc_mgr *mgr, int cmd, void *params);

int va_process_is_valid(struct va_proc_mgr *mgr, int type);

int va_get_voice_upload_buffer(struct va_proc_mgr *mgr, void **out, int frames, int type);
int va_update_voice_upload_buffer(struct va_proc_mgr *mgr, void *buf, int frames, int type);


int va_process_create(struct va_proc_mgr *mgr, int type);
int va_process_destroy(struct va_proc_mgr *mgr, int type);
void va_process_voice_upload_set(struct va_proc_mgr *mgr, int type, int flag);
/* TODO for VAD=>WW Latency Reduce */
int va_process_support_speed_up(struct va_proc_mgr *mgr, int frames);
#endif
