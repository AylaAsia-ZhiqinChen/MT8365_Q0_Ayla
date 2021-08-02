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
#include "audio_task.h"
#include "va_process.h"
#include "dsp_state.h"
#include "dsp_state_implement.h"
#ifdef CFG_HW_RES_MGR
#include "hw_res_mgr.h"
#endif
#ifdef CFG_DSP_ULPLL_SUPPORT
#include "va_state.h"
#endif
#include "timers.h"

#include "mt_printf.h"
#include "systimer.h"


#ifdef CFG_DSP_ULPLL_SUPPORT
extern void va_ulpll_afe_switch_check(int dsp_state, int va_state);
#endif

#ifdef CFG_DSP_SWITCH_STATE_EN
struct switch_timer {
    TimerHandle_t timer;
    int timer_count;
};

static struct switch_timer g_timer;

static void dsp_state_switch_timer_cb(TimerHandle_t timer)
{
    /* TODO check polling flag */

    g_timer.timer_count--;
    /* check poll OK count */
    if (g_timer.timer_count == 0)
        dsp_state_event_proc(DSP_EVT_AP_SWTICH_DONE);
}

static int dsp_state_start_switch_timer(int time, int count)
{
    int tickcount;

    g_timer.timer_count = count;
    tickcount = time * configTICK_RATE_HZ / 1000;
    g_timer.timer = xTimerCreate("Switch Timer",
                       tickcount, pdTRUE, (void *)0,
                       dsp_state_switch_timer_cb);
    xTimerStart(g_timer.timer, 0);
    return 0;
}

static int dsp_state_stop_switch_timer(void)
{
    if (xTimerIsTimerActive(g_timer.timer)) {
        xTimerStop(g_timer.timer, 0);
        xTimerDelete(g_timer.timer, 0);
    }
    g_timer.timer_count = 0;
    return 0;
}
#endif

#ifdef CFG_VA_DYM_PREPROC_SUPPORT
void va_bypass_preproc(int preproc_bypass)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = aud_get_audio_task(TASK_SCENE_VA)->task_priv;
    struct va_proc_mgr* va_mgr = NULL;
    if (priv == NULL) {
        PRINTF_E("[time:%llu] could not get audio task of va! wait next switch!\n", read_systimer_stamp_ns());
        /* todo request hw resource first */
        dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_DYM_PREPROC, DSP_SYS_HW_DRAM);
        dsp_hw_res_request(DSP_HW_RES_CLK, DSP_HW_USER_DYM_PREPROC, DSP_CLK_PLL);
        return;
    } else {
        va_mgr = (struct va_proc_mgr*)priv->priv;
    }

    PRINTF_E("[time:%llu] va_bypass_preproc enter %d\n", read_systimer_stamp_ns(), preproc_bypass);

    if (preproc_bypass) {
        va_set_params(va_mgr, CMD_SET_BYPASS_PREPROC, NULL);
        dsp_hw_res_request(DSP_HW_RES_CLK, DSP_HW_USER_DYM_PREPROC, DSP_CLK_13M);
        dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_DYM_PREPROC, DSP_SYS_HW_NONE);
    } else {
        va_set_params(va_mgr, CMD_CLR_BYPASS_PREPROC, NULL);
        dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_DYM_PREPROC, DSP_SYS_HW_DRAM);
        dsp_hw_res_request(DSP_HW_RES_CLK, DSP_HW_USER_DYM_PREPROC, DSP_CLK_PLL);
        /* for effp process need reset when input discontinuous */
        va_set_params(va_mgr, CMD_SET_AEC_WW_START, NULL);
    }
}

void va_reset_bypass_process()
{
    AUDIO_COMMON_TASK_PRIVATE* priv = aud_get_audio_task(TASK_SCENE_VA)->task_priv;
    struct va_proc_mgr* va_mgr = NULL;
    int bypass = 0;

    if (priv == NULL)
        return;

#ifdef DSP_STATE_SUPPORT
    va_mgr = (struct va_proc_mgr*)priv->priv;
    va_get_params(va_mgr, CMD_GET_BYPASS_PREPROC, &bypass);
    if ((dsp_state_get() == DSP_STAT_NORMAL) && (bypass == 1))
        va_bypass_preproc(0);
#endif
}

#endif

static int dsp_state_idle_event_proc(int event)
{
    if (event == DSP_EVT_INIT)
        dsp_state_switch(DSP_STAT_NORMAL);

    PRINTF_E("[time:%llu]dsp idle ev:%d\n", read_systimer_stamp_ns(), event);
    return 0;
}

static int dsp_state_idle_exit(void)
{
    //TODO
#ifdef CFG_HW_RES_MGR
    /* 1. set DSPPLL rate to target value */
    dsp_hw_res_lock();
    /* 2. DSP_HW_RES_CLK => 26M_D_2 */
    dsp_hw_res_request(DSP_HW_RES_CLK, DSP_HW_USER_TOP_CTRL, DSP_CLK_13M);
    dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_TOP_CTRL, DSP_SYS_HW_NONE);
    dsp_hw_res_unlock();
#endif
    PRINTF_E("[time:%llu]dsp idle exit\n", read_systimer_stamp_ns());
    return 0;
}

static int dsp_state_normal_enter(void)
{
#ifdef CFG_HW_RES_MGR
    //TODO
    dsp_hw_res_lock();
    /* 1. set DSP_HW_RES_SPM_REQ enable */
    dsp_hw_res_enable(DSP_HW_RES_SPM_REQ, 1);
    /* 2. set DSP_HW_RES_CLK     enable */
    dsp_hw_res_enable(DSP_HW_RES_CLK, 1);

#ifdef CFG_DSP_ULPLL_SUPPORT
    if (dsp_hw_res_get_enable(DSP_HW_RES_ULPLL)) {
        /* 3.1. do afe clock change check */
        va_ulpll_afe_switch_check(dsp_state_get_internal(), VA_STAT_NUM);

        /* 3.2. set ULPLL resource off  */
        dsp_hw_res_request(DSP_HW_RES_ULPLL, DSP_HW_USER_TOP_CTRL, DSP_ULPLL_OFF);
    }
#endif

#ifdef CFG_VA_DYM_PREPROC_SUPPORT
    if (va_state_get() != VA_STAT_VOICEUPLOAD)
        va_bypass_preproc(0);
#endif
    dsp_hw_res_unlock();
#endif
    /* 4. Extra control */
    PRINTF_E("[time:%llu]dsp normal enter\n", read_systimer_stamp_ns());
    return 0;
}

static int dsp_state_normal_exit(void)
{
    //TODO
    PRINTF_E("[time:%llu]dsp normal exit\n", read_systimer_stamp_ns());
    return 0;
}

static int dsp_state_normal_event_proc(int event)
{
    int next_state = DSP_STAT_NORMAL;
    int ret = 0;

    switch(event) {
    case DSP_EVT_AP_SUSPEND:
#ifdef CFG_DSP_SWITCH_STATE_EN
        next_state = DSP_STAT_SWITCH;
#else
        next_state = DSP_STAT_LOWPOWER;
#endif
        break;
    case DSP_EVT_AP_RESUME:
    case DSP_EVT_AP_SWTICH_DONE:
        break;
    default:
        break;
    }
    if (next_state != DSP_STAT_NORMAL)
        ret = dsp_state_switch(next_state);
    PRINTF_E("[time:%llu]dsp normal ev:%d, nst:%d\n", read_systimer_stamp_ns(), event, next_state);
    return ret;
}

static int dsp_state_lowpower_enter(void)
{
#ifdef CFG_HW_RES_MGR
    //TODO
    dsp_hw_res_lock();
    /* 1. set DSP_HW_RES_SPM_REQ enable */
    dsp_hw_res_enable(DSP_HW_RES_SPM_REQ, 1);
    /* 2. set DSP_HW_RES_CLK     disable */
    dsp_hw_res_enable(DSP_HW_RES_CLK, 1);

#ifdef CFG_DSP_ULPLL_SUPPORT
    /* 3.1 enable and check current setting*/
    if (dsp_hw_res_get_enable(DSP_HW_RES_ULPLL)) {
        dsp_hw_res_request(DSP_HW_RES_ULPLL, DSP_HW_USER_TOP_CTRL, DSP_ULPLL_ON);

        /* 3.2 do afe clock change check */
        va_ulpll_afe_switch_check(dsp_state_get_internal(), VA_STAT_NUM);
    }
#endif

#ifdef CFG_VA_DYM_PREPROC_SUPPORT
    va_bypass_preproc(1);
#endif
    dsp_hw_res_unlock();

#endif
    /* 4. force update va status */
    PRINTF_E("[time:%llu]dsp lowpower enter\n", read_systimer_stamp_ns());
    return 0;
}

static int dsp_state_lowpower_exit(void)
{
//TODO
    PRINTF_E("[time:%llu]dsp lowpower exit\n", read_systimer_stamp_ns());
    return 0;
}

static int dsp_state_lowpower_event_proc(int event)
{
    int next_state = DSP_STAT_LOWPOWER;
    int ret = 0;

    switch(event) {
    case DSP_EVT_AP_SUSPEND:
    case DSP_EVT_AP_SWTICH_DONE:
        break;
    case DSP_EVT_AP_RESUME:
        next_state = DSP_STAT_NORMAL;
        break;
    default:
        break;
    }
    if (next_state != DSP_STAT_LOWPOWER)
        ret = dsp_state_switch(next_state);
    PRINTF_E("[time:%llu]dsp lowpower ev:%d, nst:%d\n", read_systimer_stamp_ns(), event, next_state);
    return ret;
}
#ifdef CFG_DSP_SWITCH_STATE_EN
static int dsp_state_switch_enter(void)
{
#ifdef CFG_HW_RES_MGR
    //TODO
    dsp_hw_res_lock();
    /* 1. set DSP_HW_RES_SPM_REQ to ON and DSP_HW_RES_CLK to DSPPLL level */
    dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_TOP_CTRL, CFG_DSP_SWITCH_SYS_HW);
    dsp_hw_res_request(DSP_HW_RES_CLK, DSP_HW_USER_TOP_CTRL, CFG_DSP_SWITCH_DSP_CLK);

    /* 2. set DSP_HW_RES_SPM_REQ disable */
    dsp_hw_res_enable(DSP_HW_RES_SPM_REQ, 0);
    /* 3. set DSP_HW_RES_CLK     disable */
    dsp_hw_res_enable(DSP_HW_RES_CLK, 0);
    dsp_hw_res_unlock();
#endif
    /* 5. create 10ms period timer to poll spm status */
    /* TODO, now use 1 second do the test */
    dsp_state_start_switch_timer(10, CFG_DSP_SWITCH_TIMEOUT_LEN / 10);
    PRINTF_E("[time:%llu]dsp switch enter\n", read_systimer_stamp_ns());
    return 0;
}

static int dsp_state_switch_exit(void)
{
    //TODO
    /* 1. Destroy 10ms period timer to poll spm status */
    dsp_state_stop_switch_timer();
#ifdef CFG_HW_RES_MGR
    /* 2. Set DSP_HW_RES_CLK to 26M_D_2 */
    dsp_hw_res_lock();
    dsp_hw_res_request(DSP_HW_RES_CLK, DSP_HW_USER_TOP_CTRL, DSP_CLK_13M);
    dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_TOP_CTRL, DSP_SYS_HW_NONE);
    dsp_hw_res_unlock();
#endif
    PRINTF_E("[time:%llu]dsp switch exit\n", read_systimer_stamp_ns());
    return 0;
}

static int dsp_state_switch_event_proc(int event)
{
    int next_state = DSP_STAT_SWITCH;
    int ret = 0;

    switch(event) {
    case DSP_EVT_AP_SUSPEND:
        break;
    case DSP_EVT_AP_RESUME:
        next_state = DSP_STAT_NORMAL;
        break;
    case DSP_EVT_AP_SWTICH_DONE:
        next_state = DSP_STAT_LOWPOWER;
        break;
    default:
        break;
    }
    if (next_state != DSP_STAT_SWITCH)
        ret = dsp_state_switch(next_state);
    PRINTF_E("[time:%llu]dsp switch ev:%d, nst:%d\n", read_systimer_stamp_ns(), event, next_state);
    return ret;
}
#endif

struct virt_state_ops g_dsp_idle_ops = {
    .exit = dsp_state_idle_exit,
    .event_proc = dsp_state_idle_event_proc,
};

struct virt_state_ops g_dsp_normal_ops = {
    .enter = dsp_state_normal_enter,
    .exit = dsp_state_normal_exit,
    .event_proc = dsp_state_normal_event_proc,
};

struct virt_state_ops g_dsp_lowpower_ops = {
    .enter = dsp_state_lowpower_enter,
    .exit = dsp_state_lowpower_exit,
    .event_proc = dsp_state_lowpower_event_proc,
};

#ifdef CFG_DSP_SWITCH_STATE_EN
struct virt_state_ops g_dsp_switch_ops = {
    .enter = dsp_state_switch_enter,
    .exit = dsp_state_switch_exit,
    .event_proc = dsp_state_switch_event_proc,
};
#endif

NORMAL_SECTION_FUNC int dsp_state_implement_init(void)
{
    dsp_state_init();
    dsp_state_register_ops(DSP_STAT_IDLE, &g_dsp_idle_ops);
    dsp_state_register_ops(DSP_STAT_NORMAL, &g_dsp_normal_ops);
#ifdef CFG_DSP_SWITCH_STATE_EN
    dsp_state_register_ops(DSP_STAT_SWITCH, &g_dsp_switch_ops);
#endif
    dsp_state_register_ops(DSP_STAT_LOWPOWER, &g_dsp_lowpower_ops);
    dsp_state_enable(DSP_STAT_IDLE, 1);
    dsp_state_enable(DSP_STAT_NORMAL, 1);
    /* Dyanmic control the switch state enable */
#ifdef CFG_DSP_SWITCH_STATE_EN
    dsp_state_enable(DSP_STAT_SWITCH, 1);
#endif
    dsp_state_enable(DSP_STAT_LOWPOWER, 1);
    return 0;
}

NORMAL_SECTION_FUNC int dsp_state_implement_uninit(void)
{
    dsp_state_uninit();
    return 0;
}
