/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2019. All rights reserved.
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
#include <errno.h>
#include "audio_task.h"
#include "audio_dump_helper.h"
#include "audio_rtos_header_group.h"
#include "audio_shared_info.h"
#include "mtk_heap.h"
#include "afe_drv_misc.h"

uint32_t g_dump_status;
NORMAL_SECTION_DATA audio_dump_helper_t g_dump_help[DSP_DATA_DUMP_NUM];
NORMAL_SECTION_FUNC static void audio_dump_handler(const audio_dump_helper_t* helper, uint32_t dumped_size);

NORMAL_SECTION_FUNC static void print_helper(audio_dump_helper_t* helper)
{
    PRINTF_D("======== print AUDIO_DUMP_HELPER ========\n");
    PRINTF_D("  priv_param:\n");
    PRINTF_D("  running: %d\n", helper->running);
    PRINTF_D("  inited: %d\n", helper->inited);
    PRINTF_D("  dump_position: %d\n", helper->dump_position);
    PRINTF_D("  request_size: %u\n", helper->request_size);
    PRINTF_D("  dumped_size: %u\n", helper->dumped_size);
    PRINTF_D("  period_size_bytes: %u\n", helper->period_size_bytes);
    PRINTF_D("  period_cnt: %u\n", helper->period_cnt);
    PRINTF_D("  bytes_per_frame: %u\n", helper->bytes_per_frame);
    PRINTF_D("  data_buf_size: %u\n", helper->data_buf_size);
    PRINTF_D("  period_handler: %p\n", helper->period_handler);
    PRINTF_D("====== print AUDIO_DUMP_HELPER end ======\n");
}

NORMAL_SECTION_FUNC int audio_dump_helper_init(struct host_debug_param* init_param, audio_dump_helper_t* helper)
{
    uint32_t period_size_bytes, period_cnt;
    uint32_t shm_size_bytes;
    struct io_ipc_ring_buf_shared* share;
    void *shm_addr = NULL;

    memset(helper, 0, sizeof(audio_dump_helper_t));

    period_size_bytes = init_param->period_size * init_param->chnum * init_param->bitwidth / 8;
    period_cnt = init_param->period_cnt;
    shm_size_bytes = period_size_bytes * period_cnt + sizeof(struct io_ipc_ring_buf_shared);
    shm_addr = MTK_pvPortMalloc(shm_size_bytes, get_adsp_heap_type(ADSP_MEM_NORMAL_NCACHE));
    configASSERT(shm_addr != NULL);

    PRINTF_D("%s, real period size %u, cnt %u\n", __func__, period_size_bytes, period_cnt);


    share = (struct io_ipc_ring_buf_shared*)shm_addr;
    memset(share, 0, sizeof(struct io_ipc_ring_buf_shared));
    share->start_addr = (uint32_t)share + sizeof(struct io_ipc_ring_buf_shared);
    share->size_bytes = shm_size_bytes - sizeof(struct io_ipc_ring_buf_shared);
    share->ring_buffer_dir = RING_BUF_TYPE_RECORD;

    helper->pshared_ring_buf = share;
    helper->period_cnt = period_cnt;
    helper->period_size_bytes = period_size_bytes;
    helper->data_buf_size = period_cnt * period_size_bytes;
    helper->bytes_per_frame = init_param->chnum * init_param->bitwidth / 8;
    helper->period_handler = audio_dump_handler;

    helper->running = 0;
    helper->request_size = init_param->request_bytes;
    helper->dumped_size = 0;
    helper->inited = 1;
    helper->dump_position = init_param->dump_position;

    /* for ack return */
    init_param->shared_base = (uint32_t)helper->pshared_ring_buf;
    init_param->inited = helper->inited;

    print_helper(helper);
    PRINTF_D("shm_addr:%p, struct shared size:%d, start:0x%x, bytes:%d\n", shm_addr, sizeof(struct io_ipc_ring_buf_shared), share->start_addr, share->size_bytes);

    return 0;
}

NORMAL_SECTION_FUNC static uint32_t audio_dump_get_free_space(audio_dump_helper_t* helper)
{
    uint32_t ret;
    struct io_ipc_ring_buf_shared *pshared = helper->pshared_ring_buf;

    //wr addr == rd addr means empty, we never fill it to full
    if (pshared->ptr_to_appl_offset_bytes <= pshared->ptr_to_hw_offset_bytes) {
        ret = pshared->ptr_to_appl_offset_bytes + pshared->size_bytes - pshared->ptr_to_hw_offset_bytes;
    } else {
        ret = pshared->ptr_to_appl_offset_bytes - pshared->ptr_to_hw_offset_bytes;
    }

    //don't fill it to full
    if (ret > AUDIO_DUMP_PERIOD_ALIGN)
        ret -= AUDIO_DUMP_PERIOD_ALIGN;
    else
        ret = 0;

    return ret;
}

//caller must garantee the size is smaller then buffer remain size
NORMAL_SECTION_FUNC static void audio_dump_copy_to_shm(audio_dump_helper_t* helper, char* addr,
                                   uint32_t size)
{
    char* base_addr = (char*)(helper->pshared_ring_buf->start_addr);
    uint32_t rd_offset = helper->pshared_ring_buf->ptr_to_appl_offset_bytes;
    uint32_t wr_offset = helper->pshared_ring_buf->ptr_to_hw_offset_bytes;
    uint32_t data_buf_size = helper->pshared_ring_buf->size_bytes;
    uint32_t avail_size, free_bytes, old_cnt, new_cnt;


    while (size) {
        if (rd_offset > wr_offset)
            avail_size = rd_offset - wr_offset;
        else
            avail_size = data_buf_size - wr_offset;
        if (avail_size > size)
            avail_size = size;
        volatile_memcpy(base_addr + wr_offset, addr, avail_size);

        addr += avail_size;
        size -= avail_size;
        wr_offset += avail_size;
        if (wr_offset == data_buf_size) {
            wr_offset = 0;
        }
        helper->pshared_ring_buf->ptr_to_hw_offset_bytes = wr_offset;
        old_cnt = helper->dumped_size / helper->period_size_bytes;
        helper->dumped_size += avail_size;
        new_cnt = helper->dumped_size / helper->period_size_bytes;
        free_bytes = audio_dump_get_free_space(helper);

        if (helper->dumped_size == helper->request_size
            || (free_bytes < (helper->data_buf_size >> 1) && old_cnt != new_cnt)) {
                helper->period_handler(helper, helper->dumped_size);
        }
    }
}

NORMAL_SECTION_FUNC int audio_dump_feed_data(int dump_position, char* buf, int frames)
{
    audio_dump_helper_t* helper;
    uint32_t size, once_copy_size, size_request;
    char *char_addr = buf;
    static int err_count = 0;

    if (dump_position > DSP_DATA_DUMP_NUM) {
        PRINTF_E("%s, dump_postion error:%d, ignore init\n", __func__, dump_position);
        return -EINVAL;
    }
    helper = &g_dump_help[dump_position];
    if (helper->inited == 0 || helper->running == 0) {
        if (err_count == 0) {
            print_helper(helper);
            PRINTF_E("%s not inited or running, no need to dump.\n", __func__);
        }
        err_count = 1;
        return -EINVAL;
    }

    size = helper->bytes_per_frame * frames;
    if (size + helper->dumped_size > helper->request_size) {
        size = helper->request_size - helper->dumped_size;
    }
    size_request = size;
    while (size) {
        once_copy_size = audio_dump_get_free_space(helper);
        if (once_copy_size > size) {
            once_copy_size = size;
        }
        audio_dump_copy_to_shm(helper, char_addr, once_copy_size);
        char_addr += once_copy_size;
        size -= once_copy_size;

        if (size) {// avail free not enough, not block va_process
            PRINTF_E("%s audio dump avail free not enough,drop:%d.\n", __func__, size);
        }
        break;
    }
    err_count = 0;

    return size_request - size;

}

NORMAL_SECTION_FUNC static void audio_dump_trigger_start(audio_dump_helper_t* helper)
{
    helper->running = 1;
    g_dump_status |= (0x1 << helper->dump_position);
}
NORMAL_SECTION_FUNC static void audio_dump_trigger_stop(audio_dump_helper_t* helper)
{
    if (helper->running==0) {
        PRINTF_E("%s not running, might already closed.\n", __func__);
        return;
    }
    helper->running = 0;
    g_dump_status &= ~(0x1 << helper->dump_position);
}

NORMAL_SECTION_FUNC int audio_dump_helper_uninit(struct host_debug_param* init_param, audio_dump_helper_t* helper)
{
    int ret;

    ret = init_param->dump_position;
    if (ret > DSP_DATA_DUMP_NUM) {
        PRINTF_E("%s, dump_position error:%d, ignore uninit \n", __func__, ret);
	return -EINVAL;
    }

    helper->period_handler(helper, helper->dumped_size);
    MTK_vPortFree((void*)helper->pshared_ring_buf);
    memset(helper, 0, sizeof(audio_dump_helper_t));

    return 0;
}

NORMAL_SECTION_FUNC int audio_dump_handle_ipc_msg(ipi_msg_t *msg_ptr)
{
    int ret;
    audio_dump_helper_t* helper;
    struct host_debug_param* dbg_param = (struct host_debug_param*)msg_ptr->payload;

    ret = dbg_param->dump_position;
    if (ret > DSP_DATA_DUMP_NUM) {
        PRINTF_E("%s, dump_postion error:%d, ignore msg\n", __func__, ret);
        return -EINVAL;
    }
    helper = &g_dump_help[ret];
    switch (msg_ptr->msg_id) {
        case MSG_TO_DSP_DEBUG_START:
            audio_dump_helper_init(dbg_param, helper);
            audio_dump_trigger_start(helper);
            msg_ptr->payload_size = sizeof(struct host_debug_param);
            print_helper(helper);
            break;
        case MSG_TO_DSP_DEBUG_STOP:
            audio_dump_trigger_stop(helper);
            audio_dump_helper_uninit(dbg_param, helper);
            msg_ptr->payload_size = sizeof(struct host_debug_param);
            print_helper(helper);

            break;
        }
    return 0;
}

NORMAL_SECTION_FUNC static void audio_dump_handler(const audio_dump_helper_t* helper, uint32_t dumped_size)
{
    ipi_msg_t ipi_msg;
    struct dsp_debug_irq_param irq_param_to_host;

    irq_param_to_host.dump_position = helper->dump_position;
    irq_param_to_host.irq_notify_done = (dumped_size >= helper->request_size);

    audio_send_ipi_msg(&ipi_msg,
        TASK_SCENE_AUDIO_CONTROLLER,
        AUDIO_IPI_LAYER_TO_KERNEL,
        AUDIO_IPI_PAYLOAD,
        AUDIO_IPI_MSG_BYPASS_ACK,
        MSG_TO_HOST_DSP_DEBUG_IRQ,
        sizeof(struct dsp_debug_irq_param),
        sizeof(struct dsp_debug_irq_param),
        &irq_param_to_host);
}

int audio_dump_helper_write(int dump_position, char* buf, int frames)
{
    int ret = 0;

    if (dump_position > DSP_DATA_DUMP_NUM) {
        PRINTF_E("%s, dump_postion error:%d, ignore write \n", __func__, dump_position);
        return -EINVAL;
    }

    if (g_dump_status & (0x1 << dump_position))
        ret = audio_dump_feed_data(dump_position, buf, frames);

    return ret;
}
