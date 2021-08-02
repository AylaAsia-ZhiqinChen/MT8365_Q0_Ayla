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

#ifndef _AUDIO_TASK_H_
#define _AUDIO_TASK_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "audio_messenger_ipi.h"
#include "audio_if_structure.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "audio_shared_info.h"
#include "audio_task_io_buf_ctrl.h"

enum {
    ADSP_MEM_LP_NCACHE = 0,
    ADSP_MEM_LP_CACHE,
    ADSP_MEM_NORMAL_CACHE,
    ADSP_MEM_NORMAL_NCACHE,
    ADSP_MEM_TASK_RESERVE,
    ADSP_MEM_TYPE_NUM,
};

enum {
    AUD_THREAD_DIR_OUT,
    AUD_THREAD_DIR_IN,
    AUD_THREAD_DIR_NUM,
};

typedef struct afe_ctrl_struct {
    int io_path_handler;
    union {
        TDM_CONFIG tdm_config;
        I2S_CONFIG i2s_config;
        DMIC_CONFIG dmic_config;
    };
    AUDIO_FMT_CONFIG afe_fmt_config;
} AFE_CTRL_STRUCT;

typedef struct io_ipc_ring_buf IO_IPC_RING_BUF;

typedef struct io_ipc_ring_buf_shared IO_IPC_RING_BUF_SHARED;

typedef struct host_ipc_msg_hw_param HOST_MSG_HW_PARAM;
typedef struct host_ipc_msg_hw_free HOST_MSG_HW_FREE;
typedef struct host_ipc_msg_trigger  HOST_MSG_TRIGGER;

typedef struct dsp_ipc_msg_hw_param DSP_MSG_HW_PARAM;

typedef struct ipc_va_params IPC_VA_PARAMS;

struct thread_io_ctrl {
    /* io_UID, will be equal to host AFE DAI number if from AFE */
    /* (TBD if not from host?) */
    uint32_t io_UID;

    /* status */
    uint8_t stream_enabled;
    uint8_t irq_enabled;

    /* setting */
    /* we don't use HOST_MSG_HW_PARAM or AUDIO_FMT_CONFIG for flexibility */
    uint32_t sample_rate;
    uint8_t channel_num;
    uint8_t bitwidth; /* 16bits or 32bits */
    uint32_t period_size; /* in frames */
    uint32_t period_count;

    /* buffer related  */
    int buffer_allocated;
    int mem_type;
    AUDIO_DMA_ADDR  dma_buffer_addr;
    uint32_t buffer_max;
    IO_RING_BUF ring_buf;
    uint32_t proc_frames;

    /* connect to audio hw */
    AFE_CTRL_STRUCT* afe_ctrl;
};

enum {
    PROCESS_STAT_REQUIRE,
    PROCESS_STAT_PROC,
    PROCESS_STAT_OFFER,
    PROCESS_STAT_NUM,
};

#define AUDIO_TASK_PRIVATE_MAX_AFE_STRUCT_NUM 2
#define AUDIO_TASK_PRIVATE_MAX_INPUT_IO_CTRL 1
#define AUDIO_TASK_PRIVATE_MAX_OUTPUT_IO_CTRL 1

typedef struct va_params {
    uint32_t va_setting;   // Voice Assistant setting
    uint32_t va_state;     // Voice Assistant state
} VA_PARAMS;

/* a simple reserve memory pool, max support 3 node */
struct alloc_node {
    uint8_t in_used;
    void *start;
    uint32_t size;
};

#define RESERVE_MAX_NODE  3
struct task_reserve_pool {
    uint8_t init;
    void *start;
    uint32_t size;
    struct alloc_node node[RESERVE_MAX_NODE];
};

typedef struct audio_common_task_private {
    /* processing status & buffer */
    int processing_status;
    uint32_t local_process_buf_size;
    uint32_t local_process_buf_size_max;
    void* local_process_buf;

    /* AFE ctrl */
    AFE_CTRL_STRUCT afe_struct[AUDIO_TASK_PRIVATE_MAX_AFE_STRUCT_NUM];
    uint8_t afe_struct_num;

    /* input to thread and output from thread ctrl */
    THREAD_IO_CTRL input_io_ctrl[AUDIO_TASK_PRIVATE_MAX_INPUT_IO_CTRL];
    uint8_t input_io_ctrl_num;
    THREAD_IO_CTRL output_io_ctrl[AUDIO_TASK_PRIVATE_MAX_OUTPUT_IO_CTRL];
    uint8_t output_io_ctrl_num;

    struct task_reserve_pool lpmem_pool; //task reserve low power memory pool
    IO_IPC_RING_BUF_SHARED* shared_ring_buffer; // shared buffer between dsp and host

    void *priv;
} AUDIO_COMMON_TASK_PRIVATE;

/* the definition for the AUDIO_TASK class */
typedef struct audio_task AUDIO_TASK;
typedef struct audio_task_ops {
    /* constructor/destructor */
    void (*constructor)(AUDIO_TASK* this);
    void (*destructor)(AUDIO_TASK* this);

    /* create RTOS task */
    void (*create_task_loop)(AUDIO_TASK* this);
    void (*destroy_task_loop)(AUDIO_TASK* this);
    void (*task_loop_func)(void* void_this);

    /* receive ipi message */
    void (*recv_message)(AUDIO_TASK* this,struct ipi_msg_t *ipi_msg);
} AUDIO_TASK_OPS;

/* the definition for the AUDIO_TASK class */
struct audio_task {
    const uint16_t thread_id;
    const AUDIO_TASK_OPS* ops;
    const char thread_name[32];
    const unsigned short thread_stack_dep;
    const UBaseType_t thread_priority;

    /* attributes */
   TaskHandle_t thread_handler;  //xTaskNotifyGive & ulTaskNotifyTake can notify queue
    QueueHandle_t msg_queue; //should contain msg structure
    QueueHandle_t irq_queue; //should be binary queue?

    void* task_priv;
};

/* function declare */
void init_audio_task(const uint16_t thread_id);
void deinit_audio_task(const uint16_t thread_id);
QueueHandle_t aud_create_msg_queue(int queue_size);
AUDIO_TASK* aud_get_audio_task(const uint16_t thread_id); //should remove it?
void aud_create_task_loop_common(AUDIO_TASK* this);
void aud_destroy_task_loop_common(AUDIO_TASK* this);
THREAD_IO_CTRL* aud_get_thread_io(AUDIO_COMMON_TASK_PRIVATE* priv,
                  int dir, int t_io_UID);
THREAD_IO_CTRL* add_and_init_thread_io(AUDIO_COMMON_TASK_PRIVATE* priv,
                       int dir, int t_io_UID);
void delete_thread_io(AUDIO_COMMON_TASK_PRIVATE* priv, int dir,
                  THREAD_IO_CTRL* ctrl);

uint32_t io_ctrl_get_period_size_bytes(THREAD_IO_CTRL* in_io_ctrl);
uint32_t io_ctrl_get_buf_size_bytes(THREAD_IO_CTRL* in_io_ctrl);

//for THREAD_IO_CTRL buffer control
int aud_hw_copy_from_ring_buffer(char* dst_buf, THREAD_IO_CTRL* src_io_ctrl,
                 uint32_t size_to_copy);
int aud_appl_copy_from_ring_buffer(char* dst_buf, THREAD_IO_CTRL* src_io_ctrl,
                   uint32_t size_to_copy);
int aud_hw_copy_to_ring_buffer(THREAD_IO_CTRL* dst_io_ctrl, char* src_buf,
                   uint32_t size_to_copy);
int aud_appl_copy_to_ring_buffer(THREAD_IO_CTRL* dst_io_ctrl, char* src_buf,
                 uint32_t size_to_copy);

//dummy processing
void dummy_audio_processer(void* in_ptr, int in_ptr_size,
               void* out_ptr, int out_ptr_size);

//task reserve memory pool
int task_reserve_pool_valid(struct task_reserve_pool *pool);
void task_reserve_pool_init(struct task_reserve_pool *pool, void *addr, uint32_t size);
void task_reserve_pool_uninit(struct task_reserve_pool *pool);
void *task_reserve_pool_alloc(struct task_reserve_pool *pool, uint32_t req_size);
void task_reserve_pool_free(struct task_reserve_pool *pool, void *addr);

int get_adsp_heap_type(int mem_type);

#endif
