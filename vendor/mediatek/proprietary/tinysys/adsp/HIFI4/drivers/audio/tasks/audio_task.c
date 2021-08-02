/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 * Author: Garlic Tseng <garlic.tseng@mediatek.com>
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

#include <string.h>
#include "afe_drv_misc.h"
#include "audio_rtos_header_group.h"
#include "audio_task.h"
#include "audio_task_io_buf_ctrl.h"
#include "audio_task_va.h"
#include "audio_task_top_ctrl.h"
#include "audio_shared_info.h"
#include "mtk_heap.h"

static const uint16_t g_taskscene_to_tid[TASK_SCENE_SIZE] = {  /*remap scenes to task id*/
    TASK_SCENE_AUDIO_CONTROLLER,
    TASK_SCENE_VA,
};

static AUDIO_TASK g_audio_task_arr[TASK_SCENE_SIZE] = {
    [TASK_SCENE_AUDIO_CONTROLLER] = {
        .thread_id = TASK_SCENE_AUDIO_CONTROLLER,
        .ops = &g_aud_task_top_ctrl_ops,
        .thread_name = "aud_t_audio_ctrl",
        .thread_stack_dep = 4 * configMINIMAL_STACK_SIZE,
        .thread_priority = tskIDLE_PRIORITY,
    },
    [TASK_SCENE_VA] = {
        .thread_id = TASK_SCENE_VA,
        .thread_name = "aud_t_va",
        .ops = &g_aud_task_va_ops,
        .thread_stack_dep = 16 * configMINIMAL_STACK_SIZE,
        .thread_priority = tskIDLE_PRIORITY,
    },
};

NORMAL_SECTION_FUNC void init_audio_task(const uint16_t thread_id)
{
    configASSERT(thread_id<TASK_SCENE_SIZE);
    AUDIO_TASK* aud_task = &g_audio_task_arr[thread_id];

    if (aud_task->ops && aud_task->ops->constructor) {
        aud_task->ops->constructor(aud_task);
    }

    if (aud_task->ops && aud_task->ops->create_task_loop) {
        aud_task->ops->create_task_loop(aud_task);
    } else {
        PRINTF_W("%s you don't need a loop for %d thread?\n",
             __func__, thread_id);
    }
}

NORMAL_SECTION_FUNC void deinit_audio_task(const uint16_t thread_id)
{
    configASSERT(thread_id<TASK_SCENE_SIZE);
    AUDIO_TASK* aud_task = &g_audio_task_arr[thread_id];

    if (aud_task->ops && aud_task->ops->destroy_task_loop) {
        aud_task->ops->destroy_task_loop(aud_task);
    }

    if (aud_task->ops && aud_task->ops->destructor) {
        aud_task->ops->destructor(aud_task);
    } else {
        PRINTF_W("%s you don't need a loop for %d thread?\n",
             __func__, thread_id);
    }
}


NORMAL_SECTION_FUNC QueueHandle_t aud_create_msg_queue(int queue_size)
{
    return xQueueCreate(queue_size, sizeof(ipi_msg_t));
}

AUDIO_TASK* aud_get_audio_task(const uint16_t thread_id)
{
    uint16_t tid_map;

    configASSERT(thread_id<TASK_SCENE_SIZE);
    tid_map = g_taskscene_to_tid[thread_id];
    return &g_audio_task_arr[tid_map];
}

/* some common ops function */
void aud_create_task_loop_common(AUDIO_TASK* this)
{
    BaseType_t ret;

    /* Create the task, storing the handle. */
    ret = xTaskCreate(this->ops->task_loop_func, this->thread_name,
              this->thread_stack_dep, (void*)this,
              this->thread_priority, &this->thread_handler);

    configASSERT(ret==pdPASS);
}

void aud_destroy_task_loop_common(AUDIO_TASK* this)
{
#if (INCLUDE_vTaskDelete == 1)
    if (this->thread_handler != 0) {
        vTaskDelete(this->thread_handler);
        this->thread_handler = 0;
    }
//#else
//  configASSERT(0); //TBD
#endif
}

THREAD_IO_CTRL* aud_get_thread_io(AUDIO_COMMON_TASK_PRIVATE* priv,
                  int dir, int t_io_UID)
{
    int io_size;
    THREAD_IO_CTRL* io_ctrl_arr;
    int i;

    configASSERT(priv!=NULL);

    if (dir == AUD_THREAD_DIR_OUT) {
        io_size = priv->output_io_ctrl_num;
        io_ctrl_arr = priv->output_io_ctrl;
    } else if (dir == AUD_THREAD_DIR_IN) {
        io_size = priv->input_io_ctrl_num;
        io_ctrl_arr = priv->input_io_ctrl;
    } else {
        configASSERT(0);
        return NULL;
    }

    for (i=0;i<io_size;++i) {
        if (io_ctrl_arr[i].io_UID==t_io_UID)
            return &io_ctrl_arr[i];
    }

    return NULL;
}

THREAD_IO_CTRL* add_and_init_thread_io(AUDIO_COMMON_TASK_PRIVATE* priv,
                       int dir, int t_io_UID)
{
    //TODO if t_io_UID is not valid, assigned an UID to it?
    if (dir == AUD_THREAD_DIR_OUT) {
        priv->output_io_ctrl_num++;
        configASSERT(priv->output_io_ctrl_num <= AUDIO_TASK_PRIVATE_MAX_OUTPUT_IO_CTRL);
        memset(&priv->output_io_ctrl[priv->output_io_ctrl_num-1], 0, sizeof(THREAD_IO_CTRL));
        priv->output_io_ctrl[priv->output_io_ctrl_num-1].io_UID = t_io_UID;
        return &priv->output_io_ctrl[priv->output_io_ctrl_num-1];
    } else {
        priv->input_io_ctrl_num++;
        configASSERT(priv->input_io_ctrl_num <= AUDIO_TASK_PRIVATE_MAX_INPUT_IO_CTRL);
        memset(&priv->output_io_ctrl[priv->output_io_ctrl_num-1], 0, sizeof(THREAD_IO_CTRL));
        priv->input_io_ctrl[priv->input_io_ctrl_num-1].io_UID = t_io_UID;
        return &priv->input_io_ctrl[priv->input_io_ctrl_num-1];
    }
}

void delete_thread_io(AUDIO_COMMON_TASK_PRIVATE* priv, int dir, THREAD_IO_CTRL* ctrl)
{
    //TODO how to dynamic manage the io control
    ctrl->io_UID = 0xFFFFFFFF;
    if (dir == AUD_THREAD_DIR_OUT)
        priv->output_io_ctrl_num--;
    else
        priv->input_io_ctrl_num--;
}

uint32_t io_ctrl_get_period_size_bytes(THREAD_IO_CTRL* in_io_ctrl)
{
    configASSERT(in_io_ctrl!=NULL);
    return in_io_ctrl->period_size * in_io_ctrl->channel_num
           * (in_io_ctrl->bitwidth / 8);
}

uint32_t io_ctrl_get_buf_size_bytes(THREAD_IO_CTRL* in_io_ctrl)
{
    configASSERT(in_io_ctrl!=NULL);
    return io_ctrl_get_period_size_bytes(in_io_ctrl) * in_io_ctrl->period_count;
}

/*
 * aud_hw_copy_from_ring_buffer:
 *   if there is enough data in buffer, copy it out and updata hw buf.
 *   otherwise return failed and change nothing
 *   return 0 if success, -pdFREERTOS_ERRNO_EIO if no data
 */
int aud_hw_copy_from_ring_buffer(char* dst_buf, THREAD_IO_CTRL* src_io_ctrl,
                 uint32_t size_to_copy)
{
    uint32_t src_hw_ptr, src_buf_size_bytes;
    char* copy_addr;
    uint32_t src_avail_bytes;

    src_avail_bytes = io_ctrl_buf_get_data_watermark(src_io_ctrl);
    if (src_avail_bytes < size_to_copy)
        return -pdFREERTOS_ERRNO_EIO;

    src_buf_size_bytes = io_ctrl_buf_get_size(src_io_ctrl);

    src_hw_ptr = io_ctrl_buf_get_hw_ofs(src_io_ctrl);

    copy_addr = (char *)io_ctrl_buf_get_start_addr(src_io_ctrl)
            + src_hw_ptr;

    volatile_memcpy((void*)dst_buf, (void*)copy_addr, size_to_copy);
    src_hw_ptr += size_to_copy;
#if 0
    configASSERT(src_hw_ptr <= src_buf_size_bytes);
    if (src_hw_ptr == src_buf_size_bytes)
        src_hw_ptr = 0;
#endif
    src_hw_ptr %= src_buf_size_bytes;
    io_ctrl_buf_set_hw_ofs(src_io_ctrl, src_hw_ptr);
    return 0;
}
/*
 * aud_appl_copy_from_ring_buffer:
 *   if there is enough data in buffer, copy it out and updata appl buf.
 *   otherwise return failed and change nothing
 *   return 0 if success, -pdFREERTOS_ERRNO_EIO if no data
 */
int aud_appl_copy_from_ring_buffer(char* dst_buf, THREAD_IO_CTRL* src_io_ctrl,
                   uint32_t size_to_copy)
{
    uint32_t src_appl_ptr, src_buf_size_bytes;
    char* copy_addr;
    uint32_t src_avail_bytes;

    src_avail_bytes = io_ctrl_buf_get_data_watermark(src_io_ctrl);
    if (src_avail_bytes < size_to_copy)
    {
        //PRINTF_E("%s : data not enough src_avail_bytes %d, size_to_copy %d\n", __func__, src_avail_bytes,size_to_copy);
        return -pdFREERTOS_ERRNO_EIO;
    }

//  PRINTF_E("%s : data enough src_avail_bytes %d, size_to_copy %d\n", __func__, src_avail_bytes,size_to_copy);

    src_buf_size_bytes = io_ctrl_buf_get_size(src_io_ctrl);

    src_appl_ptr = io_ctrl_buf_get_appl_ofs(src_io_ctrl);

    copy_addr = (char*)io_ctrl_buf_get_start_addr(src_io_ctrl)
            + src_appl_ptr;

    volatile_memcpy((void*)dst_buf, (void*)copy_addr, size_to_copy);
    src_appl_ptr += size_to_copy;
#if 0
    configASSERT(src_appl_ptr <= src_buf_size_bytes);
    if (src_appl_ptr == src_buf_size_bytes)
        src_appl_ptr = 0;
#endif
    src_appl_ptr %= src_buf_size_bytes;
    io_ctrl_buf_set_appl_ofs(src_io_ctrl, src_appl_ptr);
    return 0;
}

/*
 * aud_hw_copy_to_ring_buffer:
 *   if there is enough space in buffer, copy it out and updata hw buf.
 *   otherwise return failed and change nothing
 *   return 0 if success, -pdFREERTOS_ERRNO_ENOBUFS if no space
 */
int aud_hw_copy_to_ring_buffer(THREAD_IO_CTRL* dst_io_ctrl, char* src_buf,
                   uint32_t size_to_copy)
{
    uint32_t dst_hw_ptr, dst_buf_size_bytes;
    char* copy_addr;
    uint32_t dst_remain_bytes;

    dst_remain_bytes = io_ctrl_buf_get_remain_space(dst_io_ctrl);

    if (dst_remain_bytes < size_to_copy)
        return -pdFREERTOS_ERRNO_ENOBUFS;

    dst_buf_size_bytes = io_ctrl_buf_get_size(dst_io_ctrl);

    dst_hw_ptr = io_ctrl_buf_get_hw_ofs(dst_io_ctrl);

    copy_addr = (char*)io_ctrl_buf_get_start_addr(dst_io_ctrl);
    copy_addr += dst_hw_ptr;

    volatile_memcpy((void*)copy_addr, (void*)src_buf, size_to_copy);
    dst_hw_ptr += size_to_copy;
#if 0
    configASSERT(dst_hw_ptr <= dst_buf_size_bytes);
    if (dst_hw_ptr == dst_buf_size_bytes)
        dst_hw_ptr = 0;
#endif
    dst_hw_ptr %= dst_buf_size_bytes;
    io_ctrl_buf_set_hw_ofs(dst_io_ctrl, dst_hw_ptr);

    return 0;
}

/*
 * aud_appl_copy_to_ring_buffer:
 *   if there is enough space in buffer, copy it out and updata appl buf.
 *   otherwise return failed and change nothing
 *   return 0 if success, -pdFREERTOS_ERRNO_ENOBUFS if no space
 */
int aud_appl_copy_to_ring_buffer(THREAD_IO_CTRL* dst_io_ctrl, char* src_buf,
                 uint32_t size_to_copy)
{
    uint32_t dst_appl_ptr, dst_buf_size_bytes;
    char* copy_addr;
    uint32_t dst_remain_bytes;

    dst_remain_bytes = io_ctrl_buf_get_remain_space(dst_io_ctrl);

    if (dst_remain_bytes < size_to_copy)
        return -pdFREERTOS_ERRNO_ENOBUFS;

    dst_buf_size_bytes = io_ctrl_buf_get_size(dst_io_ctrl);

    dst_appl_ptr = io_ctrl_buf_get_appl_ofs(dst_io_ctrl);

    copy_addr = (char*)io_ctrl_buf_get_start_addr(dst_io_ctrl);
    copy_addr += dst_appl_ptr;

    volatile_memcpy((void*)copy_addr, (void*)src_buf, size_to_copy);
    dst_appl_ptr += size_to_copy;
#if 0
    configASSERT(dst_appl_ptr <= dst_buf_size_bytes);
    if (dst_appl_ptr == dst_buf_size_bytes)
        dst_appl_ptr = 0;
#endif
    dst_appl_ptr %= dst_buf_size_bytes;
    io_ctrl_buf_set_appl_ofs(dst_io_ctrl, dst_appl_ptr);

    return 0;
}

void dummy_audio_processer(void* in_ptr, int in_ptr_size,
               void* out_ptr, int out_ptr_size)
{
    return;
}

int task_reserve_pool_valid(struct task_reserve_pool *pool)
{
    return (int)pool->init;
}

void task_reserve_pool_init(struct task_reserve_pool *pool, void *addr, uint32_t size)
{
    int i = 0;
    uint32_t mem;

    if (addr == 0 || size == 0)
        return;

    pool->start = addr;
    pool->size = size;

    mem = (uint32_t)addr;
    if (mem % 16 != 0)
        mem = (mem & 0xFFFFFFF0) + 0x10;

    pool->node[i].in_used = 0;
    pool->node[i].start = (void *)mem;
    pool->node[i].size = size - (mem - (uint32_t)(addr));
    for (i = 1; i < 3; i++) {
        pool->node[i].in_used = 0;
        pool->node[i].start = 0;
        pool->node[i].size = 0;
    }
    pool->init = 1;
}

void task_reserve_pool_uninit(struct task_reserve_pool *pool)
{
    pool->init = 0;
}

void *task_reserve_pool_alloc(struct task_reserve_pool *pool, uint32_t req_size)
{
    void *addr = 0;
    int i = 0;
    uint32_t align_size;

    if (!pool->init)
        return 0;

    align_size = ((req_size>>3) + (uint32_t)((req_size%8) != 0))<<3;
    if (align_size == 0)
        return 0;

    for (i = 0; i< RESERVE_MAX_NODE; i++) {
        if (!(pool->node[i].in_used) && align_size <= pool->node[i].size) {
            break;
        }
    }
    if (i == RESERVE_MAX_NODE)
        return 0;

    addr = pool->node[i].start;
    pool->node[i].in_used = 1;
    //merge the left space to next free node
    if ((align_size <= pool->node[i].size) &&
        ((i + 1) < RESERVE_MAX_NODE) && !(pool->node[i + 1].in_used)) {
        pool->node[i + 1].start =
            (char *)(pool->node[i].start) + align_size;
        pool->node[i + 1].size += pool->node[i].size - align_size;
        pool->node[i].size = align_size;
    }

    return addr;
}

void task_reserve_pool_free(struct task_reserve_pool *pool, void *addr)
{
    int i = 0;

    if (!pool->init)
        return;

    if (addr == 0)
        return;

    for (i = 0; i< RESERVE_MAX_NODE; i++) {
        if ((pool->node[i].in_used) && (addr == pool->node[i].start)) {
            break;
        }
    }

    pool->node[i].in_used = 0;
    if (((i + 1) < RESERVE_MAX_NODE) &&
        !(pool->node[i + 1].in_used) &&
        (pool->node[i + 1].size != 0)) {
        pool->node[i].size += pool->node[i+1].size;
        pool->node[i + 1].start = 0;
        pool->node[i + 1].size = 0;
    }
    if ((i > 0) &&
        (!(pool->node[i-1].in_used))) {
        pool->node[i-1].size += pool->node[i].size;
        if (pool->node[i-1].start == 0)
            pool->node[i-1].start = pool->node[i].start;
        pool->node[i].start = 0;
        pool->node[i].size = 0;
    }
}

int get_adsp_heap_type(int mem_type)
{
    int heap_type;

    switch(mem_type) {
    case ADSP_MEM_LP_NCACHE:
    case ADSP_MEM_LP_CACHE:
        heap_type = MTK_eMemDefault;
        break;
    case ADSP_MEM_NORMAL_CACHE:
        heap_type = MTK_eMemDramNormal;
        break;
    case ADSP_MEM_NORMAL_NCACHE:
        heap_type = MTK_eMemDramNormalNC;
        break;
    default:
        heap_type = MTK_eMemDefault;
        break;
    }

    return heap_type;
}
