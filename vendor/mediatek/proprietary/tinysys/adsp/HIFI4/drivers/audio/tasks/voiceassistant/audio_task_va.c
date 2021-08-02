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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/


#include <string.h>
#include "afe_drv_api_if.h"
#include "afe_drv_misc.h"
#include "audio_rtos_header_group.h"
#include "audio_task.h"
#include "audio_task_io_buf_ctrl.h"
#include "audio_task_va.h"
#include "audio_task_top_ctrl.h"
#include <audio_drv_log.h>
#include "systimer.h" //debug
#include "audio_shared_info.h"
#include "va_process.h"
#include "va_state.h"
#include "ww_process.h"
#include "dsp_state.h"
#include "mtk_heap.h"
#include "adsp_excep.h"

//#define PROCESSING_TIME_CHECK
#define VA_MAX_MSG_QUEUE_SIZE 20
const VA_TASK_OPS g_va_task_dsp_ops;
const VA_TASK_OPS g_va_task_dsp_aec_ops;

/* TODO, not use global variable */
static struct va_proc_mgr g_va_proc_mgr;

#if defined(CFG_VA_PROCESS_CLI_SUPPORT)
#include "cli.h"
static const char *va_state[4] = { "VA_STAT_IDLE", "VA_STAT_VAD", "VA_STAT_WAKEWORD",
                                   "VA_STAT_VOICEUPLOAD" };
static const char *dsp_state[4] = { "DSP_STAT_IDLE", "DSP_STAT_NORMAL", "DSP_STAT_SWITCH",
                                   "DSP_STAT_LOWPOWER" };

//#define VA_TEST_DEBUG
#ifdef VA_TEST_DEBUG
#include <xtensa/tie/xt_timer.h>
int cal_dsp_mcps(void)
{
    unsigned int ccount_start;
    unsigned int ccount_end;
    unsigned long long t_start;
    unsigned long long t_end;

    t_end = t_start = read_systimer_stamp_ns();
    ccount_start = XT_RSR_CCOUNT();
    while (t_end - t_start < 100000000) {
        //vTaskDelay(1/portTICK_PERIOD_MS);
        t_end = read_systimer_stamp_ns();
    }
    ccount_end = XT_RSR_CCOUNT();

    return (int)(((unsigned long long)(ccount_end - ccount_start) * 1000000000 / (t_end - t_start)));
}
#endif

#ifdef CFG_VA_WW_MCPS_CALC
REFER_MCPS_CALC(ww_mcps)
#endif
#ifdef CFG_VA_VAD_MCPS_CALC
REFER_MCPS_CALC(vad_mcps)
#endif
#ifdef CFG_VA_PREPROC_MCPS_CALC
REFER_MCPS_CALC(preproc_mcps)
#endif

static void va_process_cmd(int cmd_id)
{
    int state;
    struct va_proc_mgr *va_mgr = &g_va_proc_mgr;
#ifdef VA_TEST_DEBUG
    static uint32_t irq_save;
#endif

    switch (cmd_id) {
     case 0:
         va_set_params(va_mgr, CMD_SET_VAD_FORCE_OK, NULL);
         FreeRTOS_CLIPutString("force vad pass\r\n");
         break;
     case 1:
         va_set_params(va_mgr, CMD_SET_WW_FORCE_OK, NULL);
         FreeRTOS_CLIPutString("force keyword pass\r\n");
         break;
     case 2:
         va_set_params(va_mgr, CMD_SET_WW_FORCE_FAIL, NULL);
         FreeRTOS_CLIPutString("force keyword fail\r\n");
         break;
     case 3:
         va_state_set_ww_timeout_en(0);
         FreeRTOS_CLIPutString("force disable wakeword timeout\r\n");
         break;
     case 4:
         va_state_set_ww_timeout_en(1);
         FreeRTOS_CLIPutString("force enable wakeword timeout\r\n");
         break;
     case 5:
         va_process_voice_upload_set(va_mgr, VA_SET, VA_VOICE_UPLOAD_DONE);
         FreeRTOS_CLIPutString("force voice upload done\r\n");
         break;
     case 6:
         state = va_state_get();
         if ((state >= 0) && (state < VA_STAT_NUM))
             FreeRTOS_CLIPrintf("current state : %s\r\n",va_state[state]);
         else
             FreeRTOS_CLIPutString("current state : Unknow\r\n");
         break;
#ifdef VA_TEST_DEBUG
     case 7:
         FreeRTOS_CLIPrintf("current mcps:%d\n", cal_dsp_mcps());
         break;
     case 8:
         irq_save = DRV_Reg32(0x11220894);
         DRV_WriteReg32(0x11220894, 0);
         FreeRTOS_CLIPrintf("force the audio irq disable:%x\n", irq_save);
         break;
     case 9:
         FreeRTOS_CLIPrintf("force the audio irq enable:%x\n", irq_save);
         DRV_WriteReg32(0x11220894, irq_save);
         break;
     case 10:
         va_set_params(va_mgr, CMD_SET_VAD_FORCE_FAIL, NULL);
         FreeRTOS_CLIPutString("force keep in vad\r\n");
         break;
     case 11:
         va_set_params(va_mgr, CMD_CLR_VAD_FORCE_FAIL, NULL);
         FreeRTOS_CLIPutString("disable force keep in vad\r\n");
         break;
     case 12:
         va_set_params(va_mgr, CMD_SET_WW_FORCE_CONTINUE, NULL);
         FreeRTOS_CLIPutString("force keep in ww\r\n");
         break;
     case 13:
         va_set_params(va_mgr, CMD_CLR_WW_FORCE_CONTINUE, NULL);
         FreeRTOS_CLIPutString("disable force keep in ww\r\n");
         break;
#endif
#ifdef CFG_VA_VAD_MCPS_CALC
     case 14:
         mcps_calc_dump(vad_mcps);
         break;
#endif
#ifdef CFG_VA_WW_MCPS_CALC
     case 15:
         mcps_calc_dump(ww_mcps);
         break;
#endif
#ifdef CFG_VA_PREPROC_MCPS_CALC
     case 16:
         mcps_calc_dump(preproc_mcps);
         break;
#endif
    case 17:
        state = dsp_state_get();
        if ((state >= 0) && (state < DSP_STAT_NUM))
            FreeRTOS_CLIPrintf("current state : %s\r\n",dsp_state[state]);
        else
            FreeRTOS_CLIPutString("current state : Unknow\r\n");
        break;
    default:
        FreeRTOS_CLIPutString("Unknow command\r\n");
        break;
    }
}

static void print_va_process_usage(void)
{
    FreeRTOS_CLIPutString("usage: va_process_cmd [va cmd] \r\n");
    FreeRTOS_CLIPutString(" va cmd:\r\n");
    FreeRTOS_CLIPutString("    0: force vad pass\r\n");
    FreeRTOS_CLIPutString("    1: force keyword pass\r\n");
    FreeRTOS_CLIPutString("    2: force keyword fail\r\n");
    FreeRTOS_CLIPutString("    3: force disable wakeword timeout.\r\n");
    FreeRTOS_CLIPutString("    4: force enable wakeword timeout\r\n");
    FreeRTOS_CLIPutString("    5: force voice upload done\r\n");
    FreeRTOS_CLIPutString("    6: get current va state.\r\n");
}

static int cli_cmd_va_process_set(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    const char *param_id;
    int param_id_len, param_cnt;
    unsigned int msg_cmd;

    param_cnt = FreeRTOS_CLIGetNumberOfParameters(pcCommandString);
    if (param_cnt > 1) {
        print_va_process_usage();
        return 0;
    }

    param_id = FreeRTOS_CLIGetParameter(pcCommandString, 1, &param_id_len);
    if (param_id == NULL || strncmp(param_id, "-h", param_id_len) == 0) {
        print_va_process_usage();
        return 0;
    }

    if (mt_str2ul(param_id, &msg_cmd) != 0) {
        FreeRTOS_CLIPutString("Unknown input id\r\n");
        return 0;
    }

    va_process_cmd(msg_cmd);

    return 0;
}

static const CLI_Command_Definition_t cli_cmd_va_process_cmd =
{
    "va_process_cmd",
    "\r\nva_process_cmd [cmd id|-h]\r\n",
    cli_cmd_va_process_set,
    -1
};

static void cli_va_process_register(void)
{
    FreeRTOS_CLIRegisterCommand(&cli_cmd_va_process_cmd);
}
#endif

NORMAL_SECTION_FUNC static void va_task_constructor(AUDIO_TASK* this)
{
    AUDIO_COMMON_TASK_PRIVATE* priv;
    static struct va_proc_mgr *va_mgr = &g_va_proc_mgr;
    void *reserve_sram = NULL;

    this->msg_queue = aud_create_msg_queue(VA_MAX_MSG_QUEUE_SIZE);
    this->irq_queue = xQueueCreate(VA_MAX_MSG_QUEUE_SIZE, sizeof(int));
    this->task_priv = pvPortMalloc(sizeof(AUDIO_COMMON_TASK_PRIVATE));
    memset(this->task_priv, 0, sizeof(AUDIO_COMMON_TASK_PRIVATE));
    priv = this->task_priv;
    priv->priv = (void *)va_mgr;

    /* TODO temply use fix value 32KB */
    reserve_sram = (void *)(MTK_pvPortMalloc(CFG_VA_TASK_RESERVE_SRAM, get_adsp_heap_type(ADSP_MEM_LP_NCACHE)));
    configASSERT(reserve_sram != NULL);
    PRINTF_E("%s, reserve_sram:%x\n", __func__, (uint32_t)reserve_sram);
    task_reserve_pool_init(&priv->lpmem_pool, (void *)reserve_sram, CFG_VA_TASK_RESERVE_SRAM);


    priv->shared_ring_buffer = NULL;

    priv->input_io_ctrl_num = 0;
    priv->output_io_ctrl_num = 0;

    memset(priv->priv, 0, sizeof(struct va_proc_mgr));
    va_mgr->priv = (void *)(priv);

    priv->processing_status = PROCESS_STAT_REQUIRE;

    priv->local_process_buf = NULL;
    priv->local_process_buf_size = 0;
    priv->local_process_buf_size_max = 0;

#if defined(CFG_VA_VAD_DEFAULT_ON)
    va_process_create(va_mgr, VA_PROC_VAD);
#endif
#if defined(CFG_VA_WW_DEFAULT_ON)
    va_process_create(va_mgr, VA_PROC_WW);
#endif
#if defined(CFG_VA_PREPROC_DEFAULT_ON)
    va_process_create(va_mgr, VA_PROC_PREPROC);
#endif

#if defined(CFG_VA_PROCESS_CLI_SUPPORT)
    cli_va_process_register();
#endif
}

static void va_irq_callback(int io_path_handler)
{
    BaseType_t ret = 0;
    BaseType_t needYield = 0;
#ifdef AUDIO_LOG_DEBUG
    //AUD_DRV_LOG_D("%s+ io_path_handler %d.\n", __func__,io_path_handler);
#endif
#if defined(PROCESSING_TIME_CHECK)
    unsigned long long systimer_start,systimer_end, systimer_diff;
    systimer_start = read_systimer_stamp_ns();
#endif
    ret = xQueueSendToBackFromISR(aud_get_audio_task(TASK_SCENE_VA)->irq_queue,
                      &io_path_handler, NULL);
    if (ret != pdPASS) {
        configASSERT(0);
        return;
    }
    vTaskNotifyGiveFromISR(aud_get_audio_task(TASK_SCENE_VA)->thread_handler,
                   &needYield);

    portYIELD_FROM_ISR(needYield);
#ifdef AUDIO_LOG_DEBUG
    //AUD_DRV_LOG_D("%s-.\n", __func__);
#endif

#if defined(PROCESSING_TIME_CHECK)
    systimer_end = read_systimer_stamp_ns();
    systimer_diff = systimer_end - systimer_start;
    AUD_DRV_LOG_D("%s systimer_start %llu, systimer_end %llu, systimer_diff %llu\n", __func__, systimer_end, systimer_end, systimer_diff);
#endif

}

#ifdef DSP_AFE_LP26M_SUPPORT
static int afe_source_reset_timing(struct va_proc_mgr *mgr)
{
    int dsp_state = dsp_state_get();
    int va_state = va_state_get();

    /* first enter */
    if (dsp_state == DSP_STAT_LOWPOWER &&
        mgr->afe_source == 0 &&
        va_state == VA_STAT_VAD) {
        return 1;
    }
    //first enter
    if (dsp_state == DSP_STAT_NORMAL &&
        mgr->afe_source == 1 &&
        (va_state == VA_STAT_VAD || mgr->afe_reset_event == 1)) {
        return 2;
    }
}
#endif


static inline THREAD_IO_CTRL* get_va_type_output_io_ctrl(THREAD_IO_CTRL io_ctrl[], int num)
{
    THREAD_IO_CTRL *ctrl = NULL;
    int type, i;

    for (i = 0; i < num; ++i) {
         type = DAI_GET_TYPE(io_ctrl[i].io_UID);
         if (type == DAI_VA_RECORD_TYPE) {
             ctrl = &io_ctrl[i];
             break;
         }
    }
    return ctrl;
}

static inline int is_need_offer_output_io_ctrl(THREAD_IO_CTRL io_ctrl[], int num, int va_state)
{
    THREAD_IO_CTRL *ctrl = NULL;
    int type, i;
    int ret = 0;

    for (i = 0; i < num; ++i) {
         type = DAI_GET_TYPE(io_ctrl[i].io_UID);
         ctrl = &io_ctrl[i];
         if ((ctrl->stream_enabled > 0) &&
             (type != DAI_VA_RECORD_TYPE || va_state == VA_STAT_VOICEUPLOAD)) {
             ret = 1;
             break;
         }
    }
    return ret;
}

static inline int va_task_msg_empty(AUDIO_TASK *task)
{
    return (int)((uxQueueSpacesAvailable(task->irq_queue) ==
                 VA_MAX_MSG_QUEUE_SIZE) &&
                 (uxQueueSpacesAvailable(task->msg_queue) ==
                 VA_MAX_MSG_QUEUE_SIZE));
}
/* TODO for VAD=>WW Latency Reduce */
static inline int va_need_continue_proc(AUDIO_TASK *task, int frames)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = task->task_priv;
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;
    int is_config_support = CFG_VA_PROCESS_SPEED_UP_EN;
    int is_msg_empty = va_task_msg_empty(task);
    int is_va_available = va_process_support_speed_up(va_mgr, frames);

    return is_config_support&&is_msg_empty&&is_va_available;
}

static inline int va_need_continue_offer(AUDIO_TASK *task, THREAD_IO_CTRL* dst_io_ctrl, int frames, int frame_size)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = task->task_priv;
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;
    int is_config_support = CFG_VA_UPLOAD_SPEED_UP_EN;
    int is_msg_empty = va_task_msg_empty(task);
    int is_dst_avail;
    int is_src_avail;
    int is_va_record_start = (int)(dst_io_ctrl->irq_enabled > 0);

    /* TODO, improve the ring buffer without 2*frames */
    is_src_avail = (int)(va_mgr->out->is_ring_buf && (va_proc_buf_get_remain_size(va_mgr->out, VA_PROC_BUF_READ) >= frames));
    /* TODO , now temp use 5 frames buffer threhold to handle the buffer full because of speed up */
    is_dst_avail = (int)((int)io_ctrl_buf_get_remain_space(dst_io_ctrl) > (5 * frames * frame_size));

    return is_config_support&&is_va_record_start&&is_msg_empty&&is_dst_avail&&is_src_avail;
}

#ifdef CFG_VA_SW_PRE_GAIN
/* TODO now just for test use, fix up 12DB */
/* But this will decrease mic AOP and make SNR worse */
#define MAX_VALUE_16BIT 0x7FFF
#define MIN_VALUE_16BIT 0xFFFF8000
#define MAX_VALUE_32BIT 0x7FFFFFFFLL
#define MIN_VALUE_32BIT 0xFFFFFFFF80000000LL

static void va_sw_pre_gain(char *buf, int chnum, int bits, int frames, int gain)
{
    int i;
    int total_frames = frames * chnum;
    if (bits == 16) {
        int16_t *out = (int16_t *)buf;
        int32_t temp;

        for (i = 0; i < total_frames; i++) {
            temp = (int32_t)(*(out + i));
            temp <<= 3;
            if (temp > (int32_t)MAX_VALUE_16BIT)
                temp = (int32_t)MAX_VALUE_16BIT;
            if (temp < (int32_t)MIN_VALUE_16BIT)
                temp = (int32_t)MIN_VALUE_16BIT;
            *(out + i) = (int16_t)temp;
        }
    } else if (bits == 32) {
        int32_t *out = (int32_t *)buf;
        int64_t temp;

        for (i = 0; i < total_frames; i++) {
            temp = (int64_t)(*(out + i));
            temp <<= 3;
            if (temp > (int64_t)MAX_VALUE_32BIT)
                temp = (int64_t)MAX_VALUE_32BIT;
            if (temp < (int64_t)MIN_VALUE_32BIT)
                temp = (int64_t)MIN_VALUE_32BIT;
            *(out + i) = (int32_t)temp;
        }
    }
}
#endif

#ifdef SIGNAL_TEST
static void test_signal_generate(void *mem, int frames, int bits, int chnum)
{
    int i;
    static int16_t value = 0;
    if (bits == 16) {
        int16_t *out = (int16_t *)mem;

        for (i = 0; i < frames; i++) {
            if (chnum == 1)
                *(out + i) = value;
            else if (chnum == 2) {
                *(out + 2*i + 0) = value;
                *(out + 2*i + 1) = value;
            }
            if (++value >= 16321)
                value = -16321;
        }
    } else if (bits == 32) {
        int32_t value32 = 0;
        int32_t *out = (int32_t *)mem;

        for (i = 0; i < frames; i++) {
            value32 = ((int32_t)value)<<16;
            if (chnum == 1)
                *(out + i) = value32;
            else if (chnum == 2) {
                *(out + 2*i + 0) = value32;
                *(out + 2*i + 1) = value32;
            }
            if (++value >= 16321)
                value = -16321;
        }
    }
}
#endif

static void va_handle_irq(int io_path_handler)
{
    AUDIO_COMMON_TASK_PRIVATE* priv;
    struct va_proc_mgr* va_mgr;
    THREAD_IO_CTRL* in_io_ctrl;
    THREAD_IO_CTRL* out_io_ctrl;
    int out_port_num, i, ret;
    int status;
    uint32_t afe_hw_ptr;
    uintptr_t period_size, period_frames;
    char *proc_buf;
    int count = 0;

    priv = aud_get_audio_task(TASK_SCENE_VA)->task_priv;
    va_mgr = (struct va_proc_mgr*)priv->priv;
    in_io_ctrl = &priv->input_io_ctrl[0];
    status = priv->processing_status;

    /* sync harware point to ring buffer control */
    afe_hw_ptr = ((uintptr_t)aud_drv_pcm_get_ptr(io_path_handler)
        - (uintptr_t)io_ctrl_buf_get_start_addr(in_io_ctrl));
    io_ctrl_buf_set_hw_ofs(in_io_ctrl, afe_hw_ptr);

    //if in is not enable or out is not enable, just ignore the irq
    if (in_io_ctrl->stream_enabled == 0)
        return;

    out_port_num = priv->output_io_ctrl_num;
    /* period frames fix in all process status */
    period_frames = in_io_ctrl->period_size;

    while(1) {
        if (status == PROCESS_STAT_REQUIRE) {
            int obtain_frames;

            if (!va_task_msg_empty(aud_get_audio_task(TASK_SCENE_VA)))
                break;

            period_size = io_ctrl_get_period_size_bytes(in_io_ctrl);
            /* check AFE DMA buffer size, if < 1 period, wait for next irq */
            if (io_ctrl_buf_get_data_watermark(in_io_ctrl) < period_size)
                break;

            obtain_frames = va_proc_buf_obtain_buffer(va_mgr->in, (char **)&proc_buf, period_frames, VA_PROC_BUF_WRITE);
            /* obtain_frames must be 0 or period_frames */
            configASSERT(((obtain_frames==0)||(obtain_frames==period_frames)));

            if (count++ > 1)
                 PRINTF_E("Warning, count:%x, remain:%d, period:%d\n", count, io_ctrl_buf_get_data_watermark(in_io_ctrl), period_size);

            /* copy data from AFE DMA buffer to VA in buffer */
            ret = aud_appl_copy_from_ring_buffer((char *)proc_buf,
                in_io_ctrl, period_size);
            if (ret)
                break; //no enough data
#ifdef CFG_VA_SW_PRE_GAIN
            /* TODO now fix up 12DB */
            va_sw_pre_gain(proc_buf, va_mgr->in->chnum, va_mgr->in->bitwidth, period_frames, 0);
#endif
#ifdef SIGNAL_TEST
            test_signal_generate(proc_buf, period_frames, va_mgr->in->bitwidth, va_mgr->in->chnum);
#endif
            /* update write point */
            va_proc_buf_release_buffer(va_mgr->in, (char *)proc_buf, period_frames, VA_PROC_BUF_WRITE);
            /* Go to Process State */
            status = PROCESS_STAT_PROC;
        } else if (status == PROCESS_STAT_PROC) {
            int va_state, va_state_new;
            int is_need_offer;

            va_state = va_state_get();
            if (va_state == VA_STAT_IDLE) {
                status = PROCESS_STAT_OFFER;
                continue;
            }

            do {
                /* TODO how to improve buffer latency for different record */
                ret = va_process(va_mgr, proc_buf, period_frames);
                if (ret == VA_IN_NODATA) {
                    /* If in buffer is not enough, go to require state */
                    status = PROCESS_STAT_REQUIRE;
                    continue;
                }
                if (proc_buf != NULL)
                    proc_buf = NULL;
            } while (va_need_continue_proc(aud_get_audio_task(TASK_SCENE_VA), period_frames));
            va_state_new = va_state_get();

            /* TODO, may not need reset buffer here, already reset in the host trigger stop */
            if ((va_state != va_state_new) && (va_state_new == VA_STAT_VOICEUPLOAD)) {
                /* TODO now upload notify is in the process, need remove to here? */
                /* reset DSP-AP share buffer */
                out_io_ctrl =
                    get_va_type_output_io_ctrl(priv->output_io_ctrl,
                                           out_port_num);
                if (out_io_ctrl != NULL) {
                    io_ctrl_buf_reset(out_io_ctrl, 0);
                    out_io_ctrl->proc_frames = 0;
                }
            }

#ifdef DSP_AFE_LP26M_SUPPORT
            /* TODO afe reset process */
            if (afe_source_reset_timing() == 1) {
                
            } else if (afe_source_reset_timing() == 2) {
                
            }
#endif
            is_need_offer =
                is_need_offer_output_io_ctrl(priv->output_io_ctrl,
                                         out_port_num,
                                         va_state_new);
            if (is_need_offer)
                status = PROCESS_STAT_OFFER;
            else
                status = PROCESS_STAT_REQUIRE;
        } else if (status == PROCESS_STAT_OFFER) {
            /* If some output buffer is full, just drop the data, 
               do not wait for this */
            for (i = 0; i < out_port_num; ++i) {
                uint32_t frame_size;
                int obtain_frames;
                int type = DAI_GET_TYPE(priv->output_io_ctrl[i].io_UID);
                int is_continue_offer = 0;

                out_io_ctrl = &priv->output_io_ctrl[i];
                if (out_io_ctrl->stream_enabled <= 0)
                    continue;

                if ((type == DAI_VA_RECORD_TYPE && va_state_get() != VA_STAT_VOICEUPLOAD))
                    continue;

                frame_size = (uint32_t)out_io_ctrl->channel_num * out_io_ctrl->bitwidth / 8;

                /* TODO speed up only for va voice upload, how to process other type? */
                do {
                    obtain_frames = va_get_voice_upload_buffer(va_mgr, (void **)&proc_buf, period_frames, type);
                    /* we should get the request frames from buffer */
                    //configASSERT(obtain_frames==period_frames);
                    if ((obtain_frames < (int)period_frames) || (proc_buf == NULL))
                        break;
                    ret = aud_hw_copy_to_ring_buffer(out_io_ctrl,
                          proc_buf, (uint32_t)(period_frames * frame_size));
                    if (ret != 0) {
                        PRINTF_E("output io:%d has no space, will drop data\n", priv->output_io_ctrl[i].io_UID);
                        /* TODO should not jump, or when buffer full, there will no irq notify to AP */
                        //continue;
                    }
                    /* TODO : how to process buffer full because of dsp speed up */
                    /* 1. Enlarge ADSP-CPU buffer size, at least 500ms */
                    /* 2. Speed up, at least remain free buffer  */
                    va_update_voice_upload_buffer(va_mgr, (char *)proc_buf, period_frames, type);
                    out_io_ctrl->proc_frames += period_frames;
                    is_continue_offer =
                        va_need_continue_offer(aud_get_audio_task(TASK_SCENE_VA),
                                               out_io_ctrl, period_frames, frame_size);
                } while(is_continue_offer);

                /* TODO upload speed control to reduce dual-record latency*/

                if (out_io_ctrl->irq_enabled > 0 && out_io_ctrl->proc_frames >= out_io_ctrl->period_size) {
                    ipi_msg_t ipi_msg_irqul;
                    struct dsp_ipc_msg_irq irq_msg;

                    out_io_ctrl->proc_frames %= out_io_ctrl->period_size;
                    irq_msg.dai_id = DAI_UNPACK_ID(priv->output_io_ctrl[i].io_UID);
                    /*TODO temp use sharebuffer point as the message */
                    memcpy((void *)(&irq_msg.share_ring_buf), priv->shared_ring_buffer, sizeof(struct io_ipc_ring_buf_shared));
                    audio_send_ipi_msg(&ipi_msg_irqul,
                               TASK_SCENE_VA,
                               AUDIO_IPI_LAYER_TO_KERNEL,
                               AUDIO_IPI_PAYLOAD,
                               AUDIO_IPI_MSG_BYPASS_ACK,
                               MSG_TO_HOST_DSP_IRQUL,    //AUDIO_DSP_TASK_IRQUL
                               sizeof(struct dsp_ipc_msg_irq),
                               sizeof(struct dsp_ipc_msg_irq),
                               &irq_msg);
                }

            }
            status = PROCESS_STAT_REQUIRE;
        }
    }

    priv->processing_status = status;
}

void va_recv_message(AUDIO_TASK *this,
    struct ipi_msg_t *ipi_msg)
{
    BaseType_t ret;
    ret = xQueueSendToBack(aud_get_audio_task(TASK_SCENE_VA)->msg_queue,
                   ipi_msg, 0);
    if (ret != pdTRUE) {
        PRINTF_E("%s, send msg failed\n", __func__);
        return;
    }

    xTaskNotifyGive(aud_get_audio_task(TASK_SCENE_VA)->thread_handler);
}

static inline void va_host_hw_params_to_io_ctrl(HOST_MSG_HW_PARAM* hw_params,
    THREAD_IO_CTRL* ctrl)
{
    ctrl->sample_rate = hw_params->sample_rate;
    ctrl->channel_num = hw_params->channel_num;
    ctrl->bitwidth = hw_params->bitwidth;
    ctrl->period_size = hw_params->period_size;
    ctrl->period_count = hw_params->period_count;
}

static inline void va_io_ctrl_to_dsp_hw_params(THREAD_IO_CTRL* ctrl,
    DSP_MSG_HW_PARAM* hw_params)
{
    uint32_t mem_type;
    hw_params->sample_rate = ctrl->sample_rate;
    hw_params->channel_num = ctrl->channel_num;
    hw_params->bitwidth = ctrl->bitwidth;
    hw_params->period_size = ctrl->period_size;
    hw_params->period_count = ctrl->period_count;
    switch(ctrl->mem_type) {
    case ADSP_MEM_LP_NCACHE:
    case ADSP_MEM_LP_CACHE:
    case ADSP_MEM_TASK_RESERVE:
        mem_type = AFE_MEM_TYPE_LP;
        break;
    case ADSP_MEM_NORMAL_CACHE:
    case ADSP_MEM_NORMAL_NCACHE:
        mem_type = AFE_MEM_TYPE_DRAM;
        break;
    default:
        mem_type = AFE_MEM_TYPE_AFE_SRAM;
        break;
    }
    hw_params->adsp_dma.mem_type = mem_type;
    hw_params->adsp_dma.dma_paddr = (uint32_t)ctrl->dma_buffer_addr.start_addr;
}

NORMAL_SECTION_FUNC static void *va_io_ctrl_dma_alloc(AUDIO_COMMON_TASK_PRIVATE* priv,
    THREAD_IO_CTRL* ctrl, int mem_type, uint32_t specific_addr, uint32_t size_bytes, int ring_buf_type)
{
    void *buf = NULL;
    int final_mem_type = ADSP_MEM_TYPE_NUM;
    IO_BUF_CTRL_INIT_PARAM params;

    switch (mem_type) {
    case AFE_MEM_TYPE_AFE_SRAM:
        buf = (void *)(specific_addr);
        break;
    case AFE_MEM_TYPE_LP:
        if (task_reserve_pool_valid(&priv->lpmem_pool)) {
            buf =
                task_reserve_pool_alloc(&priv->lpmem_pool, size_bytes);
            final_mem_type = ADSP_MEM_TASK_RESERVE;
        }
        if (buf == NULL) {
            buf =
                (void *)(MTK_pvPortMalloc(size_bytes, get_adsp_heap_type(ADSP_MEM_LP_NCACHE)));
            final_mem_type = ADSP_MEM_LP_NCACHE;
        }
        break;
    default:
        buf =
        (void *)(MTK_pvPortMalloc(size_bytes, get_adsp_heap_type(ADSP_MEM_NORMAL_NCACHE)));
        final_mem_type = ADSP_MEM_NORMAL_NCACHE;
        break;
    }
    PRINTF_E("%s, io ctrl start_addr:%x, type:%d\n", __func__, (uint32_t)buf, final_mem_type);
    if (buf == NULL)
        configASSERT(buf != NULL);

    params.buffer_dir = RING_BUF_TYPE_RECORD;
    if (ring_buf_type == RING_BUF_IO_LOCAL) {
        params.start_addr = buf;
        params.size_bytes = size_bytes;
    } else {
        IO_IPC_RING_BUF_SHARED* share = (IO_IPC_RING_BUF_SHARED*)buf;

        memset(share, 0, sizeof(IO_IPC_RING_BUF_SHARED));
        share->start_addr = (uint32_t)share +
            sizeof(IO_IPC_RING_BUF_SHARED);
        share->size_bytes = size_bytes - sizeof(IO_IPC_RING_BUF_SHARED);
        share->ring_buffer_dir = RING_BUF_TYPE_RECORD;
        
        params.start_addr = (void *)((uint32_t)share + sizeof(IO_IPC_RING_BUF_SHARED));
        params.size_bytes = size_bytes - sizeof(IO_IPC_RING_BUF_SHARED);
        params.hw_offset_ptr = (uint32_t *)(&share->ptr_to_hw_offset_bytes);
        params.appl_offset_ptr = (uint32_t *)(&share->ptr_to_appl_offset_bytes);
    }
    params.reset_to_full = 0;
    io_ctrl_buf_init(ctrl, &params, ring_buf_type);
    PRINTF_E("remain:%x, size:%x, hw_ofs:%x, apl_ofs:%x\n", io_ctrl_buf_get_remain_space(ctrl), io_ctrl_buf_get_size(ctrl), io_ctrl_buf_get_hw_ofs(ctrl), io_ctrl_buf_get_appl_ofs(ctrl));

    ctrl->mem_type = final_mem_type;
    ctrl->buffer_max = size_bytes;
    ctrl->dma_buffer_addr.start_addr = buf;
    ctrl->dma_buffer_addr.size_bytes = size_bytes;
    ctrl->buffer_allocated = 1;

    return buf;
}

NORMAL_SECTION_FUNC static void va_io_ctrl_dma_free(AUDIO_COMMON_TASK_PRIVATE* priv,
    THREAD_IO_CTRL* ctrl)
{
    void *buf = ctrl->dma_buffer_addr.start_addr;

    if ((buf == 0) || (!ctrl->buffer_allocated))
        return;
    io_ctrl_buf_reset(ctrl, 0);

    if (ctrl->mem_type == ADSP_MEM_TASK_RESERVE)
        task_reserve_pool_free(&priv->lpmem_pool, buf);
    else if((ctrl->mem_type == ADSP_MEM_LP_NCACHE) ||
            (ctrl->mem_type == ADSP_MEM_NORMAL_NCACHE))
        MTK_vPortFree(buf);

    ctrl->dma_buffer_addr.start_addr = 0;
    ctrl->dma_buffer_addr.size_bytes = 0;
    ctrl->buffer_allocated = 0;
}

NORMAL_SECTION_FUNC static void va_host_startup(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s.\n", __func__);
#endif
}

NORMAL_SECTION_FUNC void va_host_hw_params(AUDIO_TASK* this, ipi_msg_t* msg_ptr) // without static for ice debug
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;
    IO_IPC_RING_BUF_SHARED* va_shared_ring_buffer = priv->shared_ring_buffer;
    THREAD_IO_CTRL* ctrl;
    HOST_MSG_HW_PARAM ipc_hw_params;
    DSP_MSG_HW_PARAM ipc_hw_params_ack;
    uint32_t size_bytes;
    uint32_t dai_id;
    int is_va_type;
    int is_hostless;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s \n", __func__);
#endif
    AUDIO_IPC_COPY_HOST_HW_PARAM(msg_ptr->payload, &ipc_hw_params);

    //TODO hw parameters capability check
    dai_id = DAI_UNPACK_HOSTLESS_ID(ipc_hw_params.dai_id);
    is_va_type = (int)(DAI_GET_TYPE(ipc_hw_params.dai_id) == DAI_VA_RECORD_TYPE);
    is_hostless = DAI_IS_HOSTLESS(ipc_hw_params.dai_id);

    AUD_DRV_LOG_D("dai_id:%d periods:%d, period size:%d, srate:%d, ch:%d, bits:%d\n",
                  dai_id, ipc_hw_params.period_count, ipc_hw_params.period_size,
                  ipc_hw_params.sample_rate, ipc_hw_params.channel_num, ipc_hw_params.bitwidth);

    /* check if add the output before */
    ctrl = aud_get_thread_io(priv, AUD_THREAD_DIR_OUT, dai_id);
    if (ctrl == NULL) {
        ctrl = add_and_init_thread_io(priv, AUD_THREAD_DIR_OUT, dai_id);
    }

    /* if output is not enabled, it can be config or reconfiged */
    if (ctrl->stream_enabled <= 0) {
        /* if buffer already allocated, free it and reset it */
        va_io_ctrl_dma_free(priv, ctrl);
        va_shared_ring_buffer = NULL;

        /* get the format */
        va_host_hw_params_to_io_ctrl(&ipc_hw_params, ctrl);
        if (is_va_type) {
            struct va_pcm_format in;
            struct va_pcm_format out;

            if (va_process_state(va_mgr) == PROC_STAT_INIT)
                va_process_uninit(va_mgr);
            /* TODO fix sample rate to dsp part or fix value */
            in.rate = ctrl->sample_rate;
            in.chnum = ctrl->channel_num;
            in.bitwidth = ctrl->bitwidth;
            va_process_init(va_mgr, in, &out, ipc_hw_params.period_size);

            /* TODO fix sample rate not change now */
            ctrl->channel_num = out.chnum;
            ctrl->bitwidth = out.bitwidth;
            ctrl->proc_frames = 0;
        }

        if (is_va_type && !is_hostless && (ctrl->channel_num != ipc_hw_params.channel_num))
            AUD_DRV_LOG_D("warning the format is different with va output(%d , %d)\n",
                ctrl->channel_num, ipc_hw_params.channel_num);
        /* Calculate output share buffer size */
        /* Should be multiple of the va process period size, or there will be memcpy error */
        if (is_hostless)
            size_bytes = ((ctrl->sample_rate * CFG_VA_UPLOAD_BUF_LEN) / 1000) /
                         ctrl->period_size * ctrl->period_size;
        else
            size_bytes = ctrl->period_size * ctrl->period_count;
        size_bytes = size_bytes * ctrl->channel_num * ctrl->bitwidth / 8;

        /* for AP share buffer, need a ipc ring buffer share memory */
        size_bytes += sizeof(IO_IPC_RING_BUF_SHARED);

        /* share buffer allocate */
        /* TODO share buffer size check */
        /* now fix use DRAM as AP share buffer */
        
        va_shared_ring_buffer =
            (IO_IPC_RING_BUF_SHARED*)va_io_ctrl_dma_alloc(priv, ctrl,
                                     AFE_MEM_TYPE_DRAM,
                                     0,
                                     size_bytes,
                                     RING_BUF_IO_IPC);
        configASSERT(va_shared_ring_buffer!=NULL);
        priv->shared_ring_buffer = va_shared_ring_buffer;
        AUD_DRV_LOG_D("ap share buf  addr:%x, size:%d\n", (uint32_t)va_shared_ring_buffer, size_bytes);
        AUD_DRV_LOG_D("%s config\n", __func__);

    }

    /* for va record target, sometimes need force read */
    if (is_va_type && !is_hostless &&
       (va_state_get() != VA_STAT_VOICEUPLOAD))
        va_process_voice_upload_set(va_mgr, VA_SET, VA_VOICE_UPLOAD_FORCE_ON);

    //Set dsp real hw parameter setting to host
    va_io_ctrl_to_dsp_hw_params(ctrl, &ipc_hw_params_ack);
    msg_ptr->payload_size = sizeof(DSP_MSG_HW_PARAM);
    AUDIO_IPC_COPY_DSP_HW_PARAM(&ipc_hw_params_ack, msg_ptr->payload);

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("-%s\n", __func__);
#endif
}

NORMAL_SECTION_FUNC static void va_host_prepare(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s.\n", __func__);
#endif
}

static void va_host_trigger_start(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    //TODO enable the path. will let in-port data start to feed in
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    THREAD_IO_CTRL* ctrl;
    HOST_MSG_TRIGGER ipc_trigger;
    uint32_t dai_id;
    int is_va_type;
    int is_hostless;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s. start\n", __func__);
#endif

    AUDIO_IPC_COPY_HOST_TRIGGER(msg_ptr->payload, &ipc_trigger);

    dai_id = DAI_UNPACK_HOSTLESS_ID(ipc_trigger.dai_id);
    is_va_type = (int)(DAI_GET_TYPE(ipc_trigger.dai_id) == DAI_VA_RECORD_TYPE);
    is_hostless = DAI_IS_HOSTLESS(ipc_trigger.dai_id);

    AUD_DRV_LOG_D("+%s.  id:%x\n", __func__, ipc_trigger.dai_id);

    ctrl = aud_get_thread_io(priv, AUD_THREAD_DIR_OUT, dai_id);

    if ((ctrl->stream_enabled == 0) && is_va_type)
        va_state_event_proc(VA_EVT_START);

    ctrl->stream_enabled++;
    /* for hostless va, do not need irq */
    if (!is_hostless)
        ctrl->irq_enabled++;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s.\n", __func__);
#endif
}

static void va_host_trigger_stop(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;
    THREAD_IO_CTRL* ctrl;
    HOST_MSG_TRIGGER ipc_trigger;
    uint32_t dai_id;
    int is_va_type;
    int is_hostless;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s. stop\n", __func__);
#endif
    AUDIO_IPC_COPY_HOST_TRIGGER(msg_ptr->payload, &ipc_trigger);

    dai_id = DAI_UNPACK_HOSTLESS_ID(ipc_trigger.dai_id);
    is_va_type = (int)(DAI_GET_TYPE(ipc_trigger.dai_id) == DAI_VA_RECORD_TYPE);
    is_hostless = DAI_IS_HOSTLESS(ipc_trigger.dai_id);

    ctrl = aud_get_thread_io(priv, AUD_THREAD_DIR_OUT, dai_id);
    (ctrl->stream_enabled)--;

    /* for hostless va, do not need irq */
    if (!is_hostless)
        ctrl->irq_enabled--;

    /* reset the buffer point to avoid restart action */
    if (ctrl->stream_enabled == 0) {
        io_ctrl_buf_reset(ctrl, 0);
        if (is_va_type) {
            va_state_event_proc(VA_EVT_STOP);
            va_process_reset(va_mgr);
        }
    }

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s.\n", __func__);
#endif
}

NORMAL_SECTION_FUNC static void va_host_hw_free(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    //TODO free the DMA?
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;
    THREAD_IO_CTRL* ctrl;
    HOST_MSG_HW_FREE ipc_hw_free;
    IO_IPC_RING_BUF_SHARED* va_shared_ring_buffer = priv->shared_ring_buffer;
    int dai_id;
    int is_va_type;
    int is_hostless;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s. start\n", __func__);
#endif
    AUDIO_IPC_COPY_HOST_HW_FREE(msg_ptr->payload, &ipc_hw_free);

    dai_id = DAI_UNPACK_HOSTLESS_ID(ipc_hw_free.dai_id);
    is_va_type = (int)(DAI_GET_TYPE(ipc_hw_free.dai_id) == DAI_VA_RECORD_TYPE);
    is_hostless = DAI_IS_HOSTLESS(ipc_hw_free.dai_id);

    ctrl = aud_get_thread_io(priv, AUD_THREAD_DIR_OUT, dai_id);

    if (ctrl == NULL)
        return;

    /* for va record, trigger voice upload done */
    if (is_va_type && !is_hostless)
        va_process_voice_upload_set(va_mgr, VA_SET, VA_VOICE_UPLOAD_DONE);

    /* the hostless may be still working */
    if (ctrl->stream_enabled > 0)
        return;

    /* remove the output control */
    va_io_ctrl_dma_free(priv, ctrl);
    va_shared_ring_buffer = NULL;

    if (is_va_type)
        va_process_uninit(va_mgr);

    delete_thread_io(priv, AUD_THREAD_DIR_OUT, ctrl);
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s. stop\n", __func__);
#endif
}

NORMAL_SECTION_FUNC static void va_host_close(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    //TODO do nothing or disable the FE?
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s.\n", __func__);
#endif
}

NORMAL_SECTION_FUNC static void va_dsp_startup(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s. start\n", __func__);
#endif
}

NORMAL_SECTION_FUNC static void va_dsp_hw_params(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    /* default only 1 input io ctrl */
    THREAD_IO_CTRL* ctrl = &priv->input_io_ctrl[0];
    AFE_CTRL_STRUCT* afe_ctrl = &priv->afe_struct[0];
    int ret;
    DSP_MSG_HW_PARAM ipc_hw_params_ack;
    HOST_MSG_HW_PARAM ipc_hw_params;
    AUDIO_DMA_ADDR record_dma_addr;
    int size_bytes;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s. start\n", __func__);
#endif

    /* copy hw params message from payload */
    AUDIO_IPC_COPY_HOST_HW_PARAM(msg_ptr->payload, &ipc_hw_params);

    /* TODO do hw_params check ? or already check in host? */
    if (ctrl->stream_enabled == 0) {
        /* make sure the target io is clear */
        va_io_ctrl_dma_free(priv, ctrl);

        /* Do format setting */
        va_host_hw_params_to_io_ctrl(&ipc_hw_params, ctrl);
        size_bytes = ctrl->period_size *
                     ctrl->period_count * ctrl->channel_num *
                     ctrl->bitwidth / 8;
        record_dma_addr.start_addr = va_io_ctrl_dma_alloc(priv, ctrl,
                             ipc_hw_params.adsp_dma.mem_type,
                             ipc_hw_params.adsp_dma.dma_paddr,
                             size_bytes,
                             RING_BUF_IO_LOCAL);
        record_dma_addr.size_bytes = size_bytes;

        afe_ctrl->afe_fmt_config.sample_rate = ipc_hw_params.sample_rate;
        afe_ctrl->afe_fmt_config.channel_num = ipc_hw_params.channel_num;
        afe_ctrl->afe_fmt_config.bitwidth = ipc_hw_params.bitwidth;
        afe_ctrl->afe_fmt_config.period_size = ipc_hw_params.period_size;
        afe_ctrl->afe_fmt_config.period_count = ipc_hw_params.period_count;
        /* the dsp side driver io define shuld be same with CPU side dai order */
        afe_ctrl->io_path_handler = ipc_hw_params.dai_id;

        ret = aud_drv_pcm_open(afe_ctrl->io_path_handler,
                   afe_ctrl->afe_fmt_config,
                   va_irq_callback,
                   &record_dma_addr);
        configASSERT(ret == 0);
        ctrl->afe_ctrl = afe_ctrl;
    }

    /* Set dsp real hw parameter setting to host */
    va_io_ctrl_to_dsp_hw_params(ctrl, &ipc_hw_params_ack);

    msg_ptr->payload_size = sizeof(DSP_MSG_HW_PARAM);
    AUDIO_IPC_COPY_DSP_HW_PARAM(&ipc_hw_params_ack, msg_ptr->payload);
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s. stop\n", __func__);
#endif
}

NORMAL_SECTION_FUNC static void va_dsp_prepare(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    //TODO do nothing?
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s.\n", __func__);
#endif
}

static void va_dsp_trigger_start(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    int ret;
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    /* TODO dynamic specific target io id */
    THREAD_IO_CTRL* ctrl = &priv->input_io_ctrl[0];
    HOST_MSG_TRIGGER host_trigger;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s.\n", __func__);
#endif

    /* copy hw params message from payload */
    AUDIO_IPC_COPY_HOST_TRIGGER(msg_ptr->payload, &host_trigger);

    if (ctrl->stream_enabled <= 0) {
        priv->processing_status = PROCESS_STAT_REQUIRE;
        /* start to record and irq */
        ret = aud_drv_pcm_start(ctrl->afe_ctrl->io_path_handler);
        if (ret < 0) {
            PRINTF_E("aud_drv_pcm_start err! %d\n", ret);
            return;
        }
    }

    (ctrl->stream_enabled)++;
    (ctrl->irq_enabled)++;
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("-%s.\n", __func__);
#endif
}

static void va_dsp_trigger_stop(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    /* TODO dynamic specific target io id */
    THREAD_IO_CTRL* ctrl = &priv->input_io_ctrl[0];
    HOST_MSG_TRIGGER host_trigger;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s. start\n", __func__);
#endif

    /* copy hw params message from payload */
    AUDIO_IPC_COPY_HOST_TRIGGER(msg_ptr->payload, &host_trigger);

    (ctrl->stream_enabled)--;
    (ctrl->irq_enabled)--;
    if (ctrl->stream_enabled <= 0)
        aud_drv_pcm_stop(ctrl->afe_ctrl->io_path_handler);

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("-%s. stop\n", __func__);
#endif
}

NORMAL_SECTION_FUNC static void va_dsp_hw_free(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    /* TODO dynamic specific target io id */
    THREAD_IO_CTRL* ctrl = &priv->input_io_ctrl[0];
    HOST_MSG_HW_FREE ipc_hw_free;

    AUDIO_IPC_COPY_HOST_HW_FREE(msg_ptr->payload, &ipc_hw_free);
    
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s. start\n", __func__);
#endif

    if (ctrl->stream_enabled > 0)
        return;

    if (ctrl->afe_ctrl) {
        aud_drv_pcm_close(ctrl->afe_ctrl->io_path_handler);
        ctrl->afe_ctrl = 0;
    }

    va_io_ctrl_dma_free(priv, ctrl);

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s. stop\n", __func__);
#endif
}

NORMAL_SECTION_FUNC static void va_dsp_close(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s.\n", __func__);
#endif
}

NORMAL_SECTION_FUNC static void va_scene_set_parameters(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;
    int ret, type, state;
    IPC_VA_PARAMS* pVA_params;

    PRINTF_D("%s.\n", __func__);
    pVA_params = kal_pvPortMalloc(sizeof(IPC_VA_PARAMS));

    AUDIO_COPY_VA_PARAMS_SRC2DST(msg_ptr->payload,pVA_params);


   state = va_state_get();
   if (state != VA_STAT_IDLE) {
       PRINTF_E("%s va state is not idle(%d), not support va seting\n", __func__, state);
       return;
   }

   switch (pVA_params->va_type) {
    case VA_VAD:
        type = VA_PROC_VAD;
        break;
    case VA_KEYWORD:
        type = VA_PROC_WW;
        break;
    case VA_AEC:
        type = VA_PROC_PREPROC;
        break;
    default:
        type = VA_PROC_NUM;
        break;
    }

    if(pVA_params->enable_flag)
        ret = va_process_create(va_mgr, type);
    else
        ret = va_process_destroy(va_mgr, type);

    PRINTF_D("%s va setting %d, %d.\n", __func__, pVA_params->va_type, pVA_params->enable_flag);
    kal_vPortFree(pVA_params);
}

NORMAL_SECTION_FUNC static void va_scene_set_voice_upload_done(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
   AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
   struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;

   va_process_voice_upload_set(va_mgr, VA_SET, VA_VOICE_UPLOAD_DONE);
}

NORMAL_SECTION_FUNC static void va_scene_set_force_ok(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;
    int cmd;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s.\n", __func__);
#endif
    if (msg_ptr->msg_id == MSG_TO_DSP_SCENE_VA_VAD_FORCE_OK)
        cmd = CMD_SET_VAD_FORCE_OK;
    else if (msg_ptr->msg_id == MSG_TO_DSP_SCENE_VA_KEYWORD_FORCE_OK)
        cmd = CMD_SET_WW_FORCE_OK;
    va_set_params(va_mgr, cmd, NULL);
}

NORMAL_SECTION_FUNC static void va_scene_get_beamforming(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;
    int beamforming = 0;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s.\n", __func__);
#endif
    va_get_params(va_mgr, CMD_GET_BEAMFORMING_RESULT, &beamforming);
    *((uint32_t *)(msg_ptr->payload)) = beamforming;
}

#ifdef DYNAMIC_LOAD_MODEL
/* use dram variable to store the share buffer info*/
NORMAL_SECTION_DATA static struct ww_model_dma g_adsp_model_attr;

NORMAL_SECTION_FUNC static void va_load_mode_open(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    struct host_load_model_init_param* model_param = (struct host_load_model_init_param*)msg_ptr->payload;
    uint32_t shm_size_bytes = CFG_VA_MODEL_LEN;
    void *shm_addr = NULL;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s, real model size %u \n", __func__, model_param->model_size);
#endif

    if(model_param->model_size > CFG_VA_MODEL_LEN) {
        PRINTF_E("%s, ERROR model_size larger than shm_size %u \n", __func__, CFG_VA_MODEL_LEN);
        model_param->inited = 0;
        return;
    } else {
        shm_size_bytes = model_param->model_size;
    }

    /* malloc for share buffer between adsp and afe */
    shm_addr = (void *)MTK_pvPortMalloc(shm_size_bytes, get_adsp_heap_type(ADSP_MEM_NORMAL_NCACHE));
    configASSERT(shm_addr != NULL);
    g_adsp_model_attr.addr = shm_addr;
    g_adsp_model_attr.size = shm_size_bytes;

    /* for ack return */
    model_param->shared_base = (uint32_t)(g_adsp_model_attr.addr);
    model_param->inited = 1;
}

NORMAL_SECTION_FUNC static void va_load_mode_close(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s.\n", __func__);
#endif

    if (g_adsp_model_attr.addr != NULL) {
        MTK_vPortFree((void *)(g_adsp_model_attr.addr));
        g_adsp_model_attr.addr = NULL;
    }
    /* todo... we don't need clear this sram */
}

NORMAL_SECTION_FUNC static void va_load_mode_write(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s.\n", __func__);
#endif

    /* set param to ww_process */
    va_set_params(va_mgr, CMD_SET_WW_LOAD_MODEL, (void *)&g_adsp_model_attr);
}
#endif

NORMAL_SECTION_FUNC static void va_handle_ipc_msg(AUDIO_TASK* this, ipi_msg_t* msg_ptr)
{
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s : msg_ptr->msg_id %d\n", __func__,msg_ptr->msg_id);
#endif

    const VA_TASK_OPS *va_task_ops;
    if(msg_ptr->task_scene== TASK_SCENE_VA)
        va_task_ops = &g_va_task_dsp_ops;
    else
        PRINTF_E("No define VA task scene operations\n");

    switch (msg_ptr->msg_id) {
    case MSG_TO_DSP_HOST_PORT_STARTUP:
        va_host_startup(this, msg_ptr);
        break;
    case MSG_TO_DSP_HOST_HW_PARAMS:
        va_host_hw_params(this, msg_ptr);
        break;
    case MSG_TO_DSP_HOST_PREPARE:
        va_host_prepare(this, msg_ptr);
        break;
    case MSG_TO_DSP_HOST_TRIGGER_START:
        va_host_trigger_start(this, msg_ptr);
        break;
    case MSG_TO_DSP_HOST_TRIGGER_STOP:
        va_host_trigger_stop(this, msg_ptr);
        break;
    case MSG_TO_DSP_HOST_HW_FREE:
        va_host_hw_free(this, msg_ptr);
        break;
    case MSG_TO_DSP_HOST_CLOSE:
        va_host_close(this, msg_ptr);
        break;
    case MSG_TO_DSP_DSP_PORT_STARTUP:
        va_task_ops->startup(this, msg_ptr);
        break;
    case MSG_TO_DSP_DSP_HW_PARAMS:
        va_task_ops->hw_params(this, msg_ptr);
        break;
    case MSG_TO_DSP_DSP_PREPARE:
        va_task_ops->prepare(this, msg_ptr);
        break;
    case MSG_TO_DSP_DSP_TRIGGER_START:
        va_task_ops->trigger_start(this, msg_ptr);
        break;
    case MSG_TO_DSP_DSP_TRIGGER_STOP:
        va_task_ops->trigger_stop(this, msg_ptr);
        break;
    case MSG_TO_DSP_DSP_HW_FREE:
        va_task_ops->hw_free(this, msg_ptr);
        break;
    case MSG_TO_DSP_DSP_CLOSE:
        va_task_ops->close(this, msg_ptr);
        break;
    case MSG_TO_DSP_SCENE_VA_VAD:
    case MSG_TO_DSP_SCENE_VA_KEYWORD:
    case MSG_TO_DSP_SCENE_VA_AEC:
    case MSG_TO_DSP_SCENE_VA_PREPROCESSING:
    case MSG_TO_DSP_SCENE_VA_RECORD:
        va_scene_set_parameters(this, msg_ptr);
        break;
    case MSG_TO_DSP_SCENE_VA_VOICE_UPLOAD_DONE:
        va_scene_set_voice_upload_done(this, msg_ptr);
        break;
    case MSG_TO_DSP_SCENE_VA_VAD_FORCE_OK:
    case MSG_TO_DSP_SCENE_VA_KEYWORD_FORCE_OK:
        va_scene_set_force_ok(this, msg_ptr);
        break;
    case MSG_TO_DSP_SCENE_VA_BEAMFORMING:
        va_scene_get_beamforming(this, msg_ptr);
        break;
#ifdef DYNAMIC_LOAD_MODEL
    case MSG_TO_DSP_LOAD_MODEL_OPEN:
        va_load_mode_open(this, msg_ptr);
        break;
    case MSG_TO_DSP_LOAD_MODEL_CLOSE:
        va_load_mode_close(this, msg_ptr);
        break;
    case MSG_TO_DSP_LOAD_MODEL_WRITE:
        va_load_mode_write(this, msg_ptr);
        break;
#endif
    default:
        PRINTF_E("%s, msgID not implement yet.\n", __func__);
        PRINTF_E("    msgID %d, thread_id %u\n",
             msg_ptr->msg_id, this->thread_id);
        break;
    }
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("-%s : msg_ptr->msg_id %d\n", __func__,msg_ptr->msg_id);
#endif
}

static void va_task_loop(void* void_this)
{
    AUDIO_TASK* this = void_this;
    BaseType_t ret;
    ipi_msg_t ipi_msg;
    int irq_msg;
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s\n", __func__);
#endif

    while (1) {
        //wait for queue
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        ret = xQueueReceive(this->irq_queue, &irq_msg, 0);
        if (ret==pdTRUE) {//irq signal
            va_handle_irq(irq_msg);
            continue;
        }

        ret = xQueueReceive(this->msg_queue, &ipi_msg, portMAX_DELAY);
        configASSERT(ret==pdTRUE);
        va_handle_ipc_msg(this, &ipi_msg);

        /* send ack back if need */
        audio_send_ipi_msg_ack_back(&ipi_msg);
    }
}


NORMAL_SECTION_FUNC static void va_task_destructor(AUDIO_TASK* this)
{
    AUDIO_COMMON_TASK_PRIVATE* priv = this->task_priv;
#if (defined(CFG_VA_VAD_DEFAULT_ON) || defined(CFG_VA_WW_DEFAULT_ON) || defined(CFG_VA_PREPROC_DEFAULT_ON))
    struct va_proc_mgr* va_mgr = (struct va_proc_mgr*)priv->priv;
#endif

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s\n", __func__);
#endif
#if defined(CFG_VA_VAD_DEFAULT_ON)
    va_process_destroy(va_mgr, VA_PROC_VAD);
#endif
#if defined(CFG_VA_WW_DEFAULT_ON)
    va_process_destroy(va_mgr, VA_PROC_WW);
#endif
#if defined(CFG_VA_PREPROC_DEFAULT_ON)
    va_process_destroy(va_mgr, VA_PROC_PREPROC);
#endif

    vQueueDelete(this->msg_queue);
    vQueueDelete(this->irq_queue);
    vPortFree(this->task_priv);
    if (priv->lpmem_pool.start != 0) {
        vPortFree(priv->lpmem_pool.start);
        task_reserve_pool_uninit(&priv->lpmem_pool);
    }
}

const AUDIO_TASK_OPS g_aud_task_va_ops = {
    .constructor = va_task_constructor,
    .destructor = va_task_destructor,
    .create_task_loop = aud_create_task_loop_common,
    .destroy_task_loop = aud_destroy_task_loop_common,
    .task_loop_func = va_task_loop,
    .recv_message = va_recv_message,
};

const VA_TASK_OPS g_va_task_dsp_ops = {
    .startup = va_dsp_startup,
    .hw_params = va_dsp_hw_params,
    .prepare = va_dsp_prepare,
    .trigger_start = va_dsp_trigger_start,
    .trigger_stop = va_dsp_trigger_stop,
    .hw_free = va_dsp_hw_free,
    .close = va_dsp_close,
};
