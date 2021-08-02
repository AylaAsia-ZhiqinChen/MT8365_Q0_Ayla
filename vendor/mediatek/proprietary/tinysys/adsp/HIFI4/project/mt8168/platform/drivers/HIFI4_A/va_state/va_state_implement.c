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
#include "va_state.h"
#include "va_state_implement.h"
#ifdef CFG_HW_RES_MGR
#include "hw_res_mgr.h"
#endif
#ifdef CFG_DSP_ULPLL_SUPPORT
#include "dsp_state.h"
#endif
#include "timers.h"
#include "mt_printf.h"
#include "systimer.h"
#ifdef CFG_VA_DYM_PREPROC_SUPPORT
extern void va_bypass_preproc(int preproc_bypass);
extern void va_reset_bypass_process();
#endif

struct ww_timer {
    TimerHandle_t timer;
    int timer_count;
};

static struct ww_timer g_ww_timer;

static void va_state_ww_timeout_callback(TimerHandle_t timer)
{
    if ((--(g_ww_timer.timer_count)) == 0){
        va_state_set_ww_timeout();
        PRINTF_E("[time:%llu]WW TIMER TIMEOUT\n", read_systimer_stamp_ns());
    }
}

static int va_state_start_ww_timeout(int time, int count)
{
    int tickcount;

    if (!va_state_get_ww_timeout_en())
        return 0;

    tickcount = time * configTICK_RATE_HZ / 1000;
    g_ww_timer.timer = xTimerCreate("WW Timeout",
                       tickcount, pdTRUE, (void *)0,
                       va_state_ww_timeout_callback);
    g_ww_timer.timer_count = count;
    xTimerStart(g_ww_timer.timer, 0);
    PRINTF_E("[time:%llu]WW TIMER START\n", read_systimer_stamp_ns());
    return 0;
}

static int va_state_stop_ww_timeout(void)
{
    if (!va_state_get_ww_timeout_en())
        return 0;

    if (xTimerIsTimerActive(g_ww_timer.timer))
        xTimerStop(g_ww_timer.timer, 0);

    if (g_ww_timer.timer != NULL) {
        xTimerDelete(g_ww_timer.timer, 0);
        g_ww_timer.timer = NULL;
    }
    return 0;
}

#ifdef CFG_HW_RES_MGR
static void va_state_hw_res_set(int clk, int spm_req)
{
    int cur_clk;

    dsp_hw_res_lock();
    cur_clk = dsp_hw_res_get_setting(DSP_HW_RES_CLK, DSP_HW_USER_VA_TASK);
    if (cur_clk >= clk) {
        dsp_hw_res_request(DSP_HW_RES_CLK, DSP_HW_USER_VA_TASK, clk);
        dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_VA_TASK, spm_req);
    } else {
        dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_VA_TASK, spm_req);
        dsp_hw_res_request(DSP_HW_RES_CLK, DSP_HW_USER_VA_TASK, clk);
    }
    dsp_hw_res_unlock();
}
#endif

static int va_state_idle_enter(void)
{
#ifdef CFG_HW_RES_MGR
    va_state_hw_res_set(CFG_VA_IDLE_DSP_CLK, CFG_VA_IDLE_SYS_HW);
#endif
    PRINTF_E("[time:%llu]va idle enter\n", read_systimer_stamp_ns());
    return 0;
}

static int va_state_idle_event_proc(int event)
{
    int next_state = VA_STAT_IDLE;
    int ret = 0;

    switch(event) {
    case VA_EVT_START:
        if (va_state_is_valid(VA_STAT_VAD))
            next_state = VA_STAT_VAD;
        else if ((!va_state_is_valid(VA_STAT_VAD)) && va_state_is_valid(VA_STAT_WAKEWORD))
            next_state = VA_STAT_WAKEWORD;
        else if ((!va_state_is_valid(VA_STAT_VAD)) && (!va_state_is_valid(VA_STAT_WAKEWORD)))
            next_state = VA_STAT_VOICEUPLOAD;
        break;
    default:
        break;
    }
    if (next_state != VA_STAT_IDLE)
        ret = va_state_switch(next_state);
    PRINTF_E("[time:%llu]va idle ev:%d, nst:%d\n", read_systimer_stamp_ns(), event, next_state);
    return ret;
}

static int va_state_idle_exit(void)
{
#ifdef CFG_VA_DYM_PREPROC_SUPPORT
    va_reset_bypass_process();
#endif

    return 0;
}

#ifdef CFG_DSP_ULPLL_SUPPORT
static void va_ulpll_afe_switch(int ulpll_switch)
{
    if (ulpll_switch) {
        dsp_hw_res_request(DSP_HW_RES_ULPLL, DSP_HW_USER_VA_TASK, DSP_ULPLL_ON);
        dsp_hw_res_request(DSP_HW_RES_AUD26M, DSP_HW_USER_VA_TASK, DSP_AUD_26M_ULPLL);
    } else {
        dsp_hw_res_request(DSP_HW_RES_AUD26M, DSP_HW_USER_VA_TASK, DSP_AUD_26M_DCXO);
        dsp_hw_res_request(DSP_HW_RES_ULPLL, DSP_HW_USER_VA_TASK, DSP_ULPLL_OFF);
    }
}

void va_ulpll_afe_switch_check(int dsp_state, int va_state)
{
    int setting = dsp_hw_res_get_setting(DSP_HW_RES_AUD26M, DSP_HW_USER_VA_TASK);
    if (dsp_state == DSP_STAT_LOWPOWER) {
        if (setting == DSP_AUD_26M_DCXO) {
             va_ulpll_afe_switch(1);
        } else if ((setting == DSP_AUD_26M_ULPLL) &&
                   (va_state == VA_STAT_VOICEUPLOAD)) {
             va_ulpll_afe_switch(0);
        }
    } else if (dsp_state == DSP_STAT_NORMAL) {
        if (setting == DSP_AUD_26M_ULPLL) {
             va_ulpll_afe_switch(0);
        }
    }
}
#endif

static int va_state_vad_enter(void)
{
    //TODO
#ifdef CFG_HW_RES_MGR
    va_state_hw_res_set(CFG_VA_VAD_DSP_CLK, CFG_VA_VAD_SYS_HW);
#endif
    PRINTF_E("[time:%llu]vad enter\n", read_systimer_stamp_ns());
    return 0;
}

static int va_state_vad_event_proc(int event)
{
    int next_state = VA_STAT_VAD;
    int ret = 0;

    switch(event) {
    case VA_EVT_STOP:
        next_state = VA_STAT_IDLE;
        break;
    case VA_EVT_VAD_SUCCESS:
    case VA_EVT_VAD_DISABLE:
        if (va_state_is_valid(VA_STAT_WAKEWORD))
            next_state = VA_STAT_WAKEWORD;
        else
            next_state = VA_STAT_VOICEUPLOAD;
        break;
    case VA_EVT_FORCE_VOICE_UPLOAD:
        next_state = VA_STAT_VOICEUPLOAD;
        break;
    default:
        break;
    }
    if (next_state != VA_STAT_VAD) {
        ret = va_state_switch(next_state);
        PRINTF_E("[time:%llu]vad ev:%d, nst:%d\n", read_systimer_stamp_ns(), event, next_state);
    }
    return ret;
}

static int va_state_ww_enter(void)
{
    //TODO
#ifdef CFG_HW_RES_MGR
    va_state_hw_res_set(CFG_VA_WW_DSP_CLK, CFG_VA_WW_SYS_HW);
#endif
    /* 5. Add VA_EVT_WW_TIMEOUT event timer */
    va_state_clr_ww_timeout();
    va_state_start_ww_timeout(CFG_VA_WW_TIMEOUT_LEN, 1);
    PRINTF_E("[time:%llu]ww enter\n", read_systimer_stamp_ns());
    return 0;
}

static int va_state_ww_exit(void)
{
    //TODO
    /* 1. delete VA_EVT_WW_TIMEOUT event timer */
    va_state_stop_ww_timeout();
    PRINTF_E("[time:%llu]ww exit\n", read_systimer_stamp_ns());
    return 0;
}

static int va_state_ww_event_proc(int event)
{
    int next_state = VA_STAT_WAKEWORD;
    int ret = 0;

    switch(event) {
    case VA_EVT_STOP:
        next_state = VA_STAT_IDLE;
        break;
    case VA_EVT_WW_SUCCESS:
    case VA_EVT_FORCE_VOICE_UPLOAD:
        next_state = VA_STAT_VOICEUPLOAD;
        break;
    case VA_EVT_WW_FAIL:
    case VA_EVT_WW_TIMEOUT:
        if (va_state_is_valid(VA_STAT_VAD))
            next_state = VA_STAT_VAD;
        else
            next_state = VA_STAT_WAKEWORD;
        break;
    default:
        break;
    }
    if (next_state != VA_STAT_WAKEWORD) {
        ret = va_state_switch(next_state);
        PRINTF_E("[time:%llu]ww ev:%d, nst:%d\n", read_systimer_stamp_ns(), event, next_state);
    }
    return ret;
}

static int va_state_voiceupload_enter(void)
{
    //TODO
#ifdef CFG_HW_RES_MGR
    va_state_hw_res_set(CFG_VA_VOICE_UPLOAD_DSP_CLK, CFG_VA_VOICE_UPLOAD_SYS_HW);
#ifdef CFG_DSP_ULPLL_SUPPORT
    int dsp_state = dsp_state_get();
    dsp_hw_res_lock();
    if (dsp_hw_res_get_enable(DSP_HW_RES_ULPLL))
        va_ulpll_afe_switch_check(dsp_state, va_state_get_internal());
    dsp_hw_res_unlock();
#endif
#endif
    PRINTF_E("[time:%llu]upload enter\n", read_systimer_stamp_ns());
    return 0;
}

static int va_state_voiceupload_exit(void)
{
#ifdef CFG_VA_DYM_PREPROC_SUPPORT
    va_reset_bypass_process();
#endif

    return 0;
}

static int va_state_voiceupload_event_proc(int event)
{
    int next_state = VA_STAT_VOICEUPLOAD;
    int ret = 0;

    switch(event) {
    case VA_EVT_STOP:
        next_state = VA_STAT_IDLE;
        break;
    case VA_EVT_VOICE_UPLOAD_DONE:
        if (va_state_is_valid(VA_STAT_VAD))
            next_state = VA_STAT_VAD;
        else if ((!va_state_is_valid(VA_STAT_VAD)) && va_state_is_valid(VA_STAT_WAKEWORD))
            next_state = VA_STAT_WAKEWORD;
        else
            next_state = VA_STAT_IDLE;
        break;
    default:
        break;
    }
    if (next_state != VA_STAT_VOICEUPLOAD)
        ret = va_state_switch(next_state);
    PRINTF_E("[time:%llu]upload ev:%d, nst:%d\n", read_systimer_stamp_ns(), event, next_state);
    return ret;
}

struct virt_state_ops g_va_idle_ops = {
    .enter = va_state_idle_enter,
    .exit = va_state_idle_exit,
    .event_proc = va_state_idle_event_proc,
};

struct virt_state_ops g_va_vad_ops = {
    .enter = va_state_vad_enter,
    .event_proc = va_state_vad_event_proc,
};

struct virt_state_ops g_va_ww_ops = {
    .enter = va_state_ww_enter,
    .exit = va_state_ww_exit,
    .event_proc = va_state_ww_event_proc,
};

struct virt_state_ops g_va_voiceupload_ops = {
    .enter = va_state_voiceupload_enter,
    .exit = va_state_voiceupload_exit,
    .event_proc = va_state_voiceupload_event_proc,
};

NORMAL_SECTION_FUNC int va_state_implement_init(void)
{
    va_state_init();

    va_state_register_ops(VA_STAT_IDLE, &g_va_idle_ops);
    va_state_register_ops(VA_STAT_VAD, &g_va_vad_ops);
    va_state_register_ops(VA_STAT_WAKEWORD, &g_va_ww_ops);
    va_state_register_ops(VA_STAT_VOICEUPLOAD, &g_va_voiceupload_ops);
    va_state_set_ww_timeout_en(CFG_VA_WW_TIMEOUT_EN);

    /* default enable IDLE and VOICEUPLOAD state, others need user config */
    va_state_enable(VA_STAT_IDLE, 1);
    va_state_enable(VA_STAT_VOICEUPLOAD, 1);

    /* default state is IDLE */
    va_state_idle_enter();
    return 0;
}

NORMAL_SECTION_FUNC int va_state_implement_uninit(void)
{
    va_state_uninit();
    return 0;
}
