/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
#ifndef _VA_STATE_H_
#define _VA_STATE_H_
#include "virt_state.h"
#include "audio_rtos_header_group.h"
#include "semphr.h"

enum VA_STATE {
    VA_STAT_IDLE = 0,
    VA_STAT_VAD,
    VA_STAT_WAKEWORD,
    VA_STAT_VOICEUPLOAD,
    VA_STAT_NUM,
};

enum VA_EVENT {
    VA_EVT_START = 0,
    VA_EVT_STOP,
    VA_EVT_VAD_FAIL,
    VA_EVT_VAD_SUCCESS,
    VA_EVT_WW_CONTINUE,
    VA_EVT_WW_SUCCESS,
    VA_EVT_WW_FAIL,
    VA_EVT_WW_TIMEOUT,
    VA_EVT_FORCE_VOICE_UPLOAD,
    VA_EVT_VOICE_UPLOAD_DONE,
    /* TODO */
    VA_EVT_VAD_ENABLE,
    VA_EVT_VAD_DISABLE,
    VA_EVT_NUM,
};

struct va_state {
    SemaphoreHandle_t sema;
    struct virt_state *cur_state;
    struct virt_state state_list[VA_STAT_NUM];
    int ww_timeout;
    int ww_timeout_en;
};

/* used by implement */
int va_state_init(void);
int va_state_uninit(void);
int va_state_register_ops(int state, struct virt_state_ops *ops);
int va_state_switch(int next_state);

/* will be used by framework */
int va_state_get(void);
int va_state_get_internal(void);
int va_state_event_proc(int event);
int va_state_enable(int state, int enable);
int va_state_is_valid(int state);

void va_state_set_ww_timeout(void);
void va_state_clr_ww_timeout(void);
int va_state_get_ww_timeout(void);
void va_state_set_ww_timeout_en(int enable);
int va_state_get_ww_timeout_en(void);
#endif
