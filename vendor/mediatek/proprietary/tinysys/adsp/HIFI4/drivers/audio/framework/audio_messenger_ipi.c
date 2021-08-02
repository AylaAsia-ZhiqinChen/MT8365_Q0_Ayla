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
#include "audio_messenger_ipi.h"

#include "audio_task.h"
#include <errno.h>

#ifdef CFG_MTK_AUDIODSP_SUPPORT
#ifdef CFG_IPC_SUPPORT
#include "adsp_ipi.h"
#endif
#else
#endif

#include "audio_drv_log.h"
#include "audio_memory_control.h"
#include "audio_shared_info.h"

/*==============================================================================
 *                     LOG
 *============================================================================*/

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "[IPI]"

/*==============================================================================
 *                     private global members
 *============================================================================*/


/*==============================================================================
 *                     private functions - declaration
 *============================================================================*/

/* msg related */
#ifdef CFG_IPC_SUPPORT
uint16_t get_message_buf_size(const ipi_msg_t *p_ipi_msg);

static void check_msg_format(const ipi_msg_t *p_ipi_msg, unsigned int len);

static void audio_ipi_msg_dispatcher(int id, void *data, unsigned int len);


/*==============================================================================
 *                     private functions - implementation
 *============================================================================*/

uint16_t get_message_buf_size(const ipi_msg_t *p_ipi_msg)
{
    if (p_ipi_msg->data_type == AUDIO_IPI_MSG_ONLY) {
        return IPI_MSG_HEADER_SIZE;
    } else if (p_ipi_msg->data_type == AUDIO_IPI_PAYLOAD) {
        return (IPI_MSG_HEADER_SIZE + p_ipi_msg->payload_size);
    } else if (p_ipi_msg->data_type == AUDIO_IPI_DMA) {
        return (IPI_MSG_HEADER_SIZE + IPI_MSG_DMA_INFO_SIZE);
    } else {
        return 0;
    }
}


static void check_msg_format(const ipi_msg_t *p_ipi_msg, unsigned int len)
{
    AUD_ASSERT(p_ipi_msg->magic == IPI_MSG_MAGIC_NUMBER);
    AUD_ASSERT(get_message_buf_size(p_ipi_msg) == len);
}


static void audio_ipi_msg_dispatcher(int id, void *data, unsigned int len)
{
    ipi_msg_t *p_ipi_msg = NULL;
    AUDIO_TASK *task    = NULL;

    AUD_LOG_V("%s(), data = %p, len = %u", __func__, data, len);

    if (data == NULL) {
        AUD_LOG_W("%s(), drop msg due to data = NULL", __func__);
        return;
    }
    if (len < IPI_MSG_HEADER_SIZE || len > MAX_IPI_MSG_BUF_SIZE) {
        AUD_LOG_W("%s(), drop msg due to len(%u) error!!", __func__, len);
        return;
    }

    p_ipi_msg = (ipi_msg_t *)data;
    check_msg_format(p_ipi_msg, len);

    //print_msg_info(__FUNCTION__, "get msg", p_ipi_msg);

    //task = get_task_by_scene(p_ipi_msg->task_scene);
    task = aud_get_audio_task(p_ipi_msg->task_scene);
    if (task == NULL) {
        AUD_LOG_W("%s(), task = NULL, drop msg. task_scene = %d. msg_id = 0x%x",
                  __func__, p_ipi_msg->task_scene, p_ipi_msg->msg_id);
        return;
    }

    task->ops->recv_message(task, p_ipi_msg);
}


/*==============================================================================
 *                     public functions - implementation
 *============================================================================*/

void audio_messenger_ipi_init(void)
{
    ipi_status retval = ERROR;

    retval = adsp_ipi_registration(IPI_AUDIO, (ipi_handler_t)audio_ipi_msg_dispatcher, "audio");

    if (retval != DONE) {
        AUD_LOG_E("%s(), scp_ipi_registration fail!!", __func__);
    }
}

void audio_ready_notify_host(void *info, uint32_t size)
{
#if !defined(CFG_AUDIO_DEMO)
     /*avoid ipc block without host in demo case*/
    ipi_msg_t p_ipi_msg;
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s+.\n", __func__);
#endif

    audio_send_ipi_msg(&p_ipi_msg,
               TASK_SCENE_AUDIO_CONTROLLER,
               AUDIO_IPI_LAYER_TO_KERNEL,
               AUDIO_IPI_PAYLOAD,
               AUDIO_IPI_MSG_BYPASS_ACK,
               MSG_TO_HOST_DSP_AUDIO_READY,
               size,
               size,
               info);

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("%s-.\n", __func__);
#endif
#endif
}

int audio_send_ipi_msg(
    struct ipi_msg_t *p_ipi_msg,
    uint8_t task_scene, /* task_scene_t */
    uint8_t target_layer, /* audio_ipi_msg_target_layer_t */
    uint8_t data_type, /* audio_ipi_msg_data_t */
    uint8_t ack_type, /* audio_ipi_msg_ack_t */
    uint16_t msg_id,
    uint32_t param1, /* data_size for payload & dma */
    uint32_t param2,
    void    *data_buffer) /* buffer for payload & dma */
{
#ifdef CFG_MTK_AUDIODSP_SUPPORT
    struct ipi_msg_dma_info_t *dma_info = NULL;
#endif

    uint32_t ipi_msg_len = 0;
    int ret = 0;

    if (p_ipi_msg == NULL) {
        AUD_LOG_E("%s(), p_ipi_msg = NULL, return", __func__);
        return -1;
    }

    memset(p_ipi_msg, 0, MAX_IPI_MSG_BUF_SIZE);

    p_ipi_msg->magic      = IPI_MSG_MAGIC_NUMBER;
    p_ipi_msg->task_scene = task_scene;
    p_ipi_msg->source_layer = AUDIO_IPI_LAYER_FROM_DSP;
    p_ipi_msg->target_layer = target_layer;
    p_ipi_msg->data_type  = data_type;
    p_ipi_msg->ack_type   = ack_type;
    p_ipi_msg->msg_id     = msg_id;
    p_ipi_msg->param1     = param1;
    p_ipi_msg->param2     = param2;

    if (p_ipi_msg->data_type == AUDIO_IPI_PAYLOAD) {
        AUD_ASSERT(data_buffer != NULL);
        AUD_ASSERT(p_ipi_msg->payload_size <= MAX_IPI_MSG_PAYLOAD_SIZE);
        memcpy(p_ipi_msg->payload, data_buffer, p_ipi_msg->payload_size);
    } else if (p_ipi_msg->data_type == AUDIO_IPI_DMA) {
        AUD_ASSERT(data_buffer != NULL);
        p_ipi_msg->dma_addr = (char *)data_buffer;
#ifdef CFG_MTK_AUDIODSP_SUPPORT
        if (param1 > 1) {
            dma_info = &p_ipi_msg->dma_info;
            dma_info->data_size = param1;
             //TODO, bypass for build pass
/*            ret = audio_ipi_dma_write_region(
                      p_ipi_msg->task_scene,
                      data_buffer,
                      dma_info->data_size,
                      &dma_info->rw_idx);
*/
            //print_msg_info(__func__, "dma dsp -> kernel", p_ipi_msg);
        }
#endif
    }

    if (ret != 0) {
        print_msg_info(__func__, "dma fail!!", p_ipi_msg);
        return ret;
    }

    ipi_msg_len = get_message_buf_size(p_ipi_msg);
    check_msg_format(p_ipi_msg, ipi_msg_len);

    return audio_send_ipi_msg_to_kernel(p_ipi_msg);
}

static ipi_msg_t last_ipi_msg;
int audio_send_ipi_msg_to_kernel(const ipi_msg_t *p_ipi_msg)
{
    ipi_status send_status = ERROR;

    const int k_max_try_count = 10000;
    int try_count = 0;

    /* error handling */
    if (p_ipi_msg == NULL) {
        AUD_LOG_E("%s(), p_ipi_msg = NULL, return", __func__);
        return -1;
    }

    for (try_count = 0; try_count < k_max_try_count; try_count++) {
        send_status = adsp_ipi_send(
#if !defined(FAKE_HOST_IPC_UT)
                          IPI_AUDIO,
#else
                          IPI_AUDIO_FAKE_HOST,
#endif
                          (void *)p_ipi_msg,
                          get_message_buf_size(p_ipi_msg),
                          0, /* default don't wait */
                          IPI_ADSP2AP);

        if (send_status == DONE) {
            break;
        } else {
            AUD_LOG_V("%s(), #%d scp_ipi_send error %d",
                      __func__, try_count, send_status);
        }
    }

    if (send_status != DONE) {
        AUD_LOG_E("%s(), scp_ipi_send error %d", __func__, send_status);
        print_msg_info(__func__, "fail", p_ipi_msg);
        print_msg_info(__func__, "fail", &last_ipi_msg);
    } else {
        //print_msg_info(__func__, "pass", p_ipi_msg);
        //save for last msg
        memcpy((void *)&last_ipi_msg, (void *)p_ipi_msg, sizeof(ipi_msg_t));
    }

    return (send_status == DONE) ? 0 : -1;
}


int audio_send_ipi_msg_ack_back(ipi_msg_t *p_ipi_msg)
{
    if (p_ipi_msg->ack_type == AUDIO_IPI_MSG_NEED_ACK) {

        int ret = 0;
        AUD_LOG_V("%s(),task: %d, id: 0x%x, ack: %d, p1: 0x%x, p2: 0x%x ",
                  __func__,
                  p_ipi_msg->task_scene,
                  p_ipi_msg->msg_id,
                  p_ipi_msg->ack_type,
                  p_ipi_msg->param1,
                  p_ipi_msg->param2);
        p_ipi_msg->ack_type = AUDIO_IPI_MSG_ACK_BACK;
        ret = audio_send_ipi_msg_to_kernel(p_ipi_msg);
        // send fail , need to retry
        if (ret != 0) {
            p_ipi_msg->ack_type = AUDIO_IPI_MSG_NEED_ACK;
        }
        return ret;
    } else {
        return 0;
    }
    AUD_LOG_E("%s(),Error : IPC not support !!", __func__);
    return -1;

}

int packing_ipimsg(
    ipi_msg_t *p_ipi_msg,
    uint8_t task_scene, /* task_scene_t */
    uint8_t target_layer, /* audio_ipi_msg_target_layer_t */
    uint8_t data_type, /* audio_ipi_msg_data_t */
    uint8_t ack_type, /* audio_ipi_msg_ack_t */
    uint16_t msg_id,
    uint32_t param1,
    uint32_t param2,
    void    *data_buffer)
{
    memset(p_ipi_msg, 0, MAX_IPI_MSG_BUF_SIZE);

    p_ipi_msg->magic        = IPI_MSG_MAGIC_NUMBER;
    p_ipi_msg->task_scene   = task_scene;
    p_ipi_msg->target_layer = target_layer;
    p_ipi_msg->data_type    = data_type;
    p_ipi_msg->ack_type     = ack_type;
    p_ipi_msg->msg_id       = msg_id;
    p_ipi_msg->param1       = param1;
    p_ipi_msg->param2       = param2;

    if (p_ipi_msg->data_type == AUDIO_IPI_PAYLOAD) {
        AUD_ASSERT(data_buffer != NULL);
        AUD_ASSERT(p_ipi_msg->param1 <= MAX_IPI_MSG_PAYLOAD_SIZE);
        memcpy(p_ipi_msg->payload, data_buffer, p_ipi_msg->param1);
    } else if (p_ipi_msg->data_type == AUDIO_IPI_DMA) {
        AUD_ASSERT(data_buffer != NULL);
        p_ipi_msg->dma_addr = data_buffer;
    }
    return 0;
}


int audio_get_dma_from_msg(
    const ipi_msg_t *ipi_msg,
    void **data_buf,
    uint32_t *data_size)
{
    void *buf = NULL;
    uint32_t size = 0;

    int ret = 0;

    if (!ipi_msg || !data_buf || !data_size) {
        AUD_LOG_W("msg: %p, buf: %p, sz: %p NULL!!",
                  ipi_msg, data_buf, data_size);
        return -EFAULT;
    }

    if (ipi_msg->data_type != AUDIO_IPI_DMA) {
        print_msg_info(__func__, "not dma!!", ipi_msg);
        return -EFAULT;
    }

    size = ipi_msg->dma_info.data_size;
    AUDIO_ALLOC_BUFFER(buf, size);
#ifdef CFG_DMA_SUPPORT
    ret = audio_ipi_dma_read_region(
              ipi_msg->task_scene,
              buf,
              size,
              ipi_msg->dma_info.rw_idx);
#else
    // to-do, no dma
    AUD_LOG_E("%s(), to-do implement audio_ipi_read_region\n", __func__);
#endif
    if (ret != 0) {
        AUDIO_FREE_POINTER(buf);
    } else {
        *data_buf = buf;
        *data_size = size;
    }

    return ret;
}


void print_msg_info(
    const char *func_name,
    const char *description,
    const ipi_msg_t *p_ipi_msg)
{
    /* error handling */
    if (func_name == NULL || description == NULL || p_ipi_msg == NULL) {
        return;
    }

    if (p_ipi_msg->data_type == AUDIO_IPI_MSG_ONLY) {
        AUD_LOG_D("%s(), %s, task: %d, id: 0x%x, ack: %d, p1: 0x%x, p2: 0x%x",
                  func_name,
                  description,
                  p_ipi_msg->task_scene,
                  p_ipi_msg->msg_id,
                  p_ipi_msg->ack_type,
                  p_ipi_msg->param1,
                  p_ipi_msg->param2);
    } else if (p_ipi_msg->data_type == AUDIO_IPI_PAYLOAD) {
        AUD_LOG_D("%s(), %s, task: %d, id: 0x%x, ack: %d, data_size: 0x%x, p2: 0x%x",
                  func_name,
                  description,
                  p_ipi_msg->task_scene,
                  p_ipi_msg->msg_id,
                  p_ipi_msg->ack_type,
                  p_ipi_msg->payload_size,
                  p_ipi_msg->param2);
    } else if (p_ipi_msg->data_type == AUDIO_IPI_DMA) {
        AUD_LOG_D("%s(), %s, task: %d, id: 0x%x, ack: %d, data_size: 0x%x, p2: 0x%x, sz: %u, idx: %u",
                  func_name,
                  description,
                  p_ipi_msg->task_scene,
                  p_ipi_msg->msg_id,
                  p_ipi_msg->ack_type,
                  p_ipi_msg->param1,
                  p_ipi_msg->param2,
                  p_ipi_msg->dma_info.data_size,
                  p_ipi_msg->dma_info.rw_idx);
    }
}

#else
void print_msg_info(const char *func_name,const char *description,const ipi_msg_t *p_ipi_msg){}
int audio_send_ipi_msg_ack_back(ipi_msg_t *p_ipi_msg){return -1;}
int audio_send_ipi_msg(
    struct ipi_msg_t *p_ipi_msg,
    uint8_t task_scene, /* task_scene_t */
    uint8_t target_layer, /* audio_ipi_msg_target_layer_t */
    uint8_t data_type, /* audio_ipi_msg_data_t */
    uint8_t ack_type, /* audio_ipi_msg_ack_t */
    uint16_t msg_id,
    uint32_t param1, /* data_size for payload & dma */
    uint32_t param2,
    void    *data_buffer) /* buffer for payload & dma */
{
    return -1;
}
int packing_ipimsg(
    ipi_msg_t *p_ipi_msg,
    uint8_t task_scene, /* task_scene_t */
    uint8_t target_layer, /* audio_ipi_msg_target_layer_t */
    uint8_t data_type, /* audio_ipi_msg_data_t */
    uint8_t ack_type, /* audio_ipi_msg_ack_t */
    uint16_t msg_id,
    uint32_t param1,
    uint32_t param2,
    void    *data_buffer)
{
    return -1;
}
int audio_get_dma_from_msg(
    const ipi_msg_t *ipi_msg,
    void **data_buf,
    uint32_t *data_size)
{
    return -1;
}

#endif

