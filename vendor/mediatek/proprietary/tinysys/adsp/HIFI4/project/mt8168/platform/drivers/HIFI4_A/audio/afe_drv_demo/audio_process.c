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
#include "audio_rtos_header_group.h"
#include "afe_drv_api_demo_synced_api.h"
#include "afe_drv_pcm.h"
#include "audio_task.h"
#include "audio_shared_info.h"
#if defined(CFG_IPC_SUPPORT)
#include "adsp_ipi.h"
#include "audio_messenger_ipi.h"
#endif
#include "afe_drv_api_if.h"
#include "audio_drv_log.h"

TaskHandle_t g_xThreadHandleDemoCode1 = NULL;
TaskHandle_t g_xThreadHandleDemoCode2 = NULL;

#define PCM_SAMPLING_RATE                   16000
#define PCM_CHANNEL_NUM                         8
#define PCM_BITWIDTH                           16
#define PCM_BITWIDTH_BYTE     (PCM_BITWIDTH >> 3)
#define PCM_PERIOD_SIZE     PCM_SAMPLING_RATE/50  // default 10ms per period
#define PCM_PERIOD_COUNT                        2
#define PCM_RECORD_TIME                         6  // for recording data time , unit : second

#define ECHO_REF_PATH_LAG
//#define ECHO_REF_PATH_SIMULTANEOUS
#define PCM_EREF_SAMPLING_RATE              48000
#define PCM_EREF_CHANNEL_NUM                    2

#define DMIC_RECORD_PATH
uint32_t g_dsp_sram_buf_addr = 0x1FCC0000;
uint32_t g_dsp_sram_buf_offset = 0x1FCC0000;
#define DSP_SRAM_BUFFER_ADDR                  (0x1FCC0000)  // 4M SRAM
#if defined(FPGA)
uint32_t g_dsp_sram_buf_sz = 0x40000;     // 256K
#else
uint32_t g_dsp_sram_buf_sz = PCM_SAMPLING_RATE*PCM_CHANNEL_NUM*PCM_BITWIDTH_BYTE*PCM_RECORD_TIME;
#endif

void vThreadTaskDemoCode1(void* pvParameters)
{
    int cnt;
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("start record demo\n");
#endif
    record_synced_api_demo();
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("end record demo\n");
#endif

    for( ;; )
    {
#ifdef AUDIO_LOG_DEBUG
        AUD_DRV_LOG_D("idle\n");
#endif
        vTaskDelay( 300000 * portTICK_PERIOD_MS );
        cnt++;
    }
}

void vThreadTaskDemoCode2(void* pvParameters)
{
    int cnt;
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("start playback demo\n");
#endif
    playback_synced_api_demo();
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("end playback demo\n");
#endif

    for( ;; )
    {
#ifdef AUDIO_LOG_DEBUG
        AUD_DRV_LOG_D("idle\n");
#endif
        vTaskDelay( 300000 * portTICK_PERIOD_MS );
        cnt++;
    }
}


void create_record_demo_thread(void)
{
    /* param for thread */
    BaseType_t xTaskCreateReturned;
    /* param for thread end */

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("create thread...\n");
#endif
    /* Create the task, storing the handle. */
    xTaskCreateReturned = xTaskCreate(
                vThreadTaskDemoCode1,       /* Function that implements the task. */
                "Audio_record_demo_thread",     /* Text name for the task. */
                2 * configMINIMAL_STACK_SIZE,     /* whatever */ /* Stack size in words, not bytes. */
                ( void * ) 1,    /* Parameter passed into the task. */
                tskIDLE_PRIORITY,/* Priority at which the task is created. */
                &g_xThreadHandleDemoCode1 );      /* Used to pass out the created task's handle. */

    configASSERT( xTaskCreateReturned == pdPASS );

}

void create_playback_demo_thread(void)
{
    /* param for thread */
    BaseType_t xTaskCreateReturned;
    /* param for thread end */

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("create thread...\n");
#endif
    /* Create the task, storing the handle. */
    xTaskCreateReturned = xTaskCreate(
                vThreadTaskDemoCode2,       /* Function that implements the task. */
                "Audio_playback_demo_thread",   /* Text name for the task. */
                2 * configMINIMAL_STACK_SIZE,     /* whatever */ /* Stack size in words, not bytes. */
                ( void * ) 1,    /* Parameter passed into the task. */
                tskIDLE_PRIORITY,/* Priority at which the task is created. */
                &g_xThreadHandleDemoCode2 );      /* Used to pass out the created task's handle. */

    configASSERT( xTaskCreateReturned == pdPASS );

}

#if defined(FAKE_HOST_IPC_UT)
TaskHandle_t g_xThreadHandleIPCDemo = NULL;

void vThreadTaskIPCDemo(void* pvParameters)
{
    ipi_msg_t p_ipi_msg;
    uint8_t task_scene;
    HOST_MSG_HW_PARAM fe_hw_param_ptr;
    HOST_MSG_HW_PARAM be_hw_param_ptr;

    PRINTF_E("%s start.\n",__func__);

    task_scene = TASK_SCENE_AUDIO_CONTROLLER;
#if defined(AUDIO_DMIC_SUPPORT)
#endif

    //Create Voice Assistant task
    PRINTF_E("Create Voice Assistant task.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                   AUDIO_IPI_LAYER_TO_DSP,
                   AUDIO_IPI_MSG_ONLY,
                   AUDIO_IPI_MSG_BYPASS_ACK,
                   MSG_TO_DSP_CREATE_VA_T,
                   0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,
              &p_ipi_msg,
              get_message_buf_size(&p_ipi_msg),
              0, /* default don't wait */IPI_ADSP2AP);
    PRINTF_E("Host : Startup.\n");
   //Host : Startup
    task_scene = TASK_SCENE_VA;
    packing_ipimsg(&p_ipi_msg,task_scene,
                   AUDIO_IPI_LAYER_TO_DSP,
                   AUDIO_IPI_MSG_ONLY,
                   AUDIO_IPI_MSG_NEED_ACK,
                   MSG_TO_DSP_HOST_PORT_STARTUP,
                   0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
              get_message_buf_size(&p_ipi_msg),
              0, /* default don't wait */IPI_ADSP2AP);
    //DSP : Startup
    PRINTF_E("DSP : Startup.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_NEED_ACK,
                  MSG_TO_DSP_DSP_PORT_STARTUP,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    //Host : HW param
    PRINTF_E("Host : hw param.\n");
    fe_hw_param_ptr.sample_rate = PCM_SAMPLING_RATE;
    fe_hw_param_ptr.channel_num = PCM_CHANNEL_NUM;
    fe_hw_param_ptr.bitwidth = PCM_BITWIDTH;
    fe_hw_param_ptr.period_size = PCM_PERIOD_SIZE;
    fe_hw_param_ptr.period_count = PCM_PERIOD_COUNT<<3;

    PRINTF_E("Host : hw param.\n");

    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_PAYLOAD,
                  AUDIO_IPI_MSG_NEED_ACK,
                  MSG_TO_DSP_HOST_HW_PARAMS,
                  sizeof(fe_hw_param_ptr), 0, &fe_hw_param_ptr);

   adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    //DSP : HW param
    PRINTF_E("DSP : hw param.\n");
    be_hw_param_ptr.sample_rate = PCM_SAMPLING_RATE;
    be_hw_param_ptr.channel_num = PCM_CHANNEL_NUM;
    be_hw_param_ptr.bitwidth = PCM_BITWIDTH;
    be_hw_param_ptr.period_size = PCM_PERIOD_SIZE;
    be_hw_param_ptr.period_count = PCM_PERIOD_COUNT;

    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_PAYLOAD,
                  AUDIO_IPI_MSG_NEED_ACK,
                  MSG_TO_DSP_DSP_HW_PARAMS,
                  sizeof(be_hw_param_ptr), 0, &be_hw_param_ptr);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

#if defined(ECHO_REF_PATH_SIMULTANEOUS)
    task_scene = TASK_SCENE_VA_AEC;
    PRINTF_E("DSP : AEC hw param.\n");

    be_hw_param_ptr.sample_rate = 48000;
    be_hw_param_ptr.channel_num = 4;
    be_hw_param_ptr.bitwidth = 16;
    be_hw_param_ptr.period_size = PCM_PERIOD_SIZE;
    be_hw_param_ptr.period_count = PCM_PERIOD_COUNT;
    be_hw_param_ptr.tdm_config.mclk_freq = 16000 * 256;
    be_hw_param_ptr.tdm_config.slave_mode = 1;
    be_hw_param_ptr.tdm_config.lrck_invert = 0;
    be_hw_param_ptr.tdm_config.lrck_width = 16;
    be_hw_param_ptr.tdm_config.bck_invert = 1;
    be_hw_param_ptr.tdm_config.format = ETDM_FORMAT_I2S;
    be_hw_param_ptr.tdm_config.data_half_delay = 0;

    packing_ipimsg(&p_ipi_msg,task_scene,
                AUDIO_IPI_LAYER_TO_DSP,
                AUDIO_IPI_PAYLOAD,
                AUDIO_IPI_MSG_NEED_ACK,
                MSG_TO_DSP_DSP_HW_PARAMS,
                sizeof(be_hw_param_ptr), 0, &be_hw_param_ptr);
    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);
    aud_drv_set_tdm_use_sgen(DSP_INT_EREF_TDMIN3, 1);
    task_scene = TASK_SCENE_VA;
#endif
    // Host : prepare
    PRINTF_E("Host : prepare.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_BYPASS_ACK,
                  MSG_TO_DSP_HOST_PREPARE,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    // DSP : prepare
    PRINTF_E("DSP : prepare.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_NEED_ACK,
                  MSG_TO_DSP_DSP_PREPARE,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    // Host : trigger start
    PRINTF_E("Host : trigger start.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_NEED_ACK,
                  MSG_TO_DSP_HOST_TRIGGER_START,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

#if defined(ECHO_REF_PATH_SIMULTANEOUS)
    task_scene = TASK_SCENE_VA_AEC;
    PRINTF_E("DSP : AEC trigger start+\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                AUDIO_IPI_LAYER_TO_DSP,
                AUDIO_IPI_MSG_ONLY,
                AUDIO_IPI_MSG_NEED_ACK,
                MSG_TO_DSP_DSP_TRIGGER_START,
                0, 0, 0);
    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);
    task_scene = TASK_SCENE_VA;
#endif

    // DSP : trigger start
    PRINTF_E("DSP : trigger start.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_NEED_ACK,
                  MSG_TO_DSP_DSP_TRIGGER_START,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

#if defined(ECHO_REF_PATH_LAG)
    be_hw_param_ptr.sample_rate = 48000;
    be_hw_param_ptr.channel_num = 2;
    be_hw_param_ptr.bitwidth = 16;
    be_hw_param_ptr.period_size = PCM_PERIOD_SIZE;
    be_hw_param_ptr.period_count = PCM_PERIOD_COUNT;
    be_hw_param_ptr.tdm_config.mclk_freq = 16000 * 256;
    be_hw_param_ptr.tdm_config.slave_mode = 1;
    be_hw_param_ptr.tdm_config.lrck_invert = 0;
    be_hw_param_ptr.tdm_config.lrck_width = 16;
    be_hw_param_ptr.tdm_config.bck_invert = 1;
    be_hw_param_ptr.tdm_config.format = ETDM_FORMAT_I2S;

    vTaskDelay( 10 * portTICK_PERIOD_MS );

    task_scene = TASK_SCENE_VA_AEC;
    packing_ipimsg(&p_ipi_msg,task_scene,
                AUDIO_IPI_LAYER_TO_DSP,
                AUDIO_IPI_PAYLOAD,
                AUDIO_IPI_MSG_NEED_ACK,
                MSG_TO_DSP_DSP_HW_PARAMS,
                sizeof(be_hw_param_ptr), 0, &be_hw_param_ptr);
    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);
    aud_drv_set_tdm_use_sgen(DSP_INT_EREF_TDMIN3, 1);

    // DSP : trigger start
    PRINTF_E("DSP : AEC trigger start.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                AUDIO_IPI_LAYER_TO_DSP,
                AUDIO_IPI_MSG_ONLY,
                AUDIO_IPI_MSG_NEED_ACK,
                MSG_TO_DSP_DSP_TRIGGER_START,
                0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    vTaskDelay( 10 * portTICK_PERIOD_MS );

    // DSP : trigger stop
    PRINTF_E("DSP : AEC trigger stop.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                AUDIO_IPI_LAYER_TO_DSP,
                AUDIO_IPI_MSG_ONLY,
                AUDIO_IPI_MSG_BYPASS_ACK,
                MSG_TO_DSP_DSP_TRIGGER_STOP,
                0, 0, 0);
    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    task_scene = TASK_SCENE_VA;
    vTaskDelay( 10 * portTICK_PERIOD_MS );
#else
    vTaskDelay( 3000 * portTICK_PERIOD_MS );
#endif

    // Host : trigger stop
    PRINTF_E("Host : trigger stop.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_NEED_ACK,
                  MSG_TO_DSP_HOST_TRIGGER_STOP,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    // DSP : trigger stop
    PRINTF_E("DSP : trigger stop.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_NEED_ACK,
                  MSG_TO_DSP_DSP_TRIGGER_STOP,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    // Host : hw free
    PRINTF_E("Host : hw free.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_BYPASS_ACK,
                  MSG_TO_DSP_HOST_HW_FREE,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    // DSP : hw free
    PRINTF_E("DSP : hw free.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_BYPASS_ACK,
                  MSG_TO_DSP_DSP_HW_FREE,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    // Host : close
    PRINTF_E("Host : Close.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_BYPASS_ACK,
                  MSG_TO_DSP_HOST_CLOSE,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    // DSP : close
    PRINTF_E("DSP : Close.\n");
    packing_ipimsg(&p_ipi_msg,task_scene,
                  AUDIO_IPI_LAYER_TO_DSP,
                  AUDIO_IPI_MSG_ONLY,
                  AUDIO_IPI_MSG_BYPASS_ACK,
                  MSG_TO_DSP_DSP_CLOSE,
                  0, 0, 0);

    adsp_ipi_send(IPI_AUDIO,&p_ipi_msg,
                get_message_buf_size(&p_ipi_msg),
                0, /* default don't wait */IPI_ADSP2AP);

    PRINTF_E("test finish!\n");
    for (;;) {
        vTaskDelay( 30000 * portTICK_PERIOD_MS );
        PRINTF_E("idle\n");
    }

}

void create_demo_aud_framework_thread(void)
{
    /* param for thread */
    BaseType_t xTaskCreateReturned;
    /* param for thread end */


    PRINTF_E("%s create thread...\n",__func__);
    /* Create the task, storing the handle. */
    xTaskCreateReturned = xTaskCreate(
                vThreadTaskIPCDemo,       /* Function that implements the task. */
                "Audio_thread_2",          /* Text name for the task. */
                1 * configMINIMAL_STACK_SIZE,     /* whatever */ /* Stack size in words, not bytes. */
                ( void * ) 1,    /* Parameter passed into the task. */
                tskIDLE_PRIORITY,/* Priority at which the task is created. */
                &g_xThreadHandleIPCDemo );      /* Used to pass out the created task's handle. */

    configASSERT( xTaskCreateReturned == pdPASS );

}
#endif
void audio_process_init(void)
{
#if defined(FAKE_HOST_IPC_UT)
    create_demo_aud_framework_thread();
#else
    create_playback_demo_thread();
    create_record_demo_thread();
#endif
}
