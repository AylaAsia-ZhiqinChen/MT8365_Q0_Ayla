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

static struct va_state g_va_state;

NORMAL_SECTION_FUNC int va_state_init(void)
{
    int i;

    g_va_state.sema = xSemaphoreCreateMutex();
    for (i = 0; i < VA_STAT_NUM; i++) {
        g_va_state.state_list[i].state = i;
        /* default all state is enable */
        g_va_state.state_list[i].enable = 0;
        g_va_state.state_list[i].ops = NULL;
    }
    g_va_state.cur_state = &(g_va_state.state_list[VA_STAT_IDLE]);

    return 0;
}

NORMAL_SECTION_FUNC int va_state_uninit(void)
{
    vSemaphoreDelete(g_va_state.sema);
    return 0;
}

NORMAL_SECTION_FUNC int va_state_register_ops(int type, struct virt_state_ops *ops)
{
    if (type < 0 || type >= VA_STAT_NUM)
        return -1;

    g_va_state.state_list[type].ops = ops;

    return 0;
}

int va_state_get(void)
{
    int state;

    xSemaphoreTake(g_va_state.sema, portMAX_DELAY);
    if (g_va_state.cur_state == NULL)
        state = -1;
    else
        state = g_va_state.cur_state->state;
    xSemaphoreGive(g_va_state.sema);
    return state;
}

int va_state_get_internal(void)
{
    int state;

    if (g_va_state.cur_state == NULL)
        state = -1;
    else
        state = g_va_state.cur_state->state;
    return state;
}

int va_state_switch(int next_state)
{
    struct virt_state *current = g_va_state.cur_state;
    struct virt_state *next;
    int ret = 0;

    if (next_state < 0 || next_state >= VA_STAT_NUM)
        return -1;

    next = &(g_va_state.state_list[next_state]);

    if (current->ops->exit)
        current->ops->exit();

    g_va_state.cur_state = next;

    if (next->ops->enter) {
        ret = next->ops->enter();
        if (ret != 0)
            return ret;
    }

    return 0;
}

int va_state_event_proc(int event)
{
    int ret;
    if (event < 0 || event > VA_EVT_NUM)
        return -1;

    /* TODO, lock the event process or state switch? */
    xSemaphoreTake(g_va_state.sema, portMAX_DELAY);
    ret = g_va_state.cur_state->ops->event_proc(event);
    xSemaphoreGive(g_va_state.sema);
    return ret;
}

int va_state_enable(int state, int enable)
{
    struct virt_state *target;

    if (state < 0 || state >= VA_STAT_NUM)
        return -1;

    target = &(g_va_state.state_list[state]);
    xSemaphoreTake(g_va_state.sema, portMAX_DELAY);
    if (!enable)
        target->enable = 0;
    else
        target->enable = 1;
    xSemaphoreGive(g_va_state.sema);
    return 0;
}

/* TODO Semaphore lock check */
int va_state_is_valid(int state)
{
    struct virt_state *target;
    int valid;

    if (state < 0 || state >= VA_STAT_NUM)
        return -1;

    target = &(g_va_state.state_list[state]);

    valid = target->enable;

    return valid;
}

void va_state_set_ww_timeout(void)
{
   g_va_state.ww_timeout = 1;
}

void va_state_clr_ww_timeout(void)
{
    g_va_state.ww_timeout = 0;
}

int va_state_get_ww_timeout(void)
{
    return g_va_state.ww_timeout;
}

void va_state_set_ww_timeout_en(int enable)
{
    g_va_state.ww_timeout_en = enable;
}

int va_state_get_ww_timeout_en(void)
{
    return g_va_state.ww_timeout_en;
}
