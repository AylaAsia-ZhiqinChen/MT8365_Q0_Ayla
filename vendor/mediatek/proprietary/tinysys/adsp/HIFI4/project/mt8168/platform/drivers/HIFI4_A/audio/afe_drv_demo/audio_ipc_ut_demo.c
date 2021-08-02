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

/*
 * adsp_ipi_send(IPI_AUDIO, xxx) host to dsp
 * adsp_ipi_send(IPI_FAKE_HOST_XX, XXX) dsp to host
 */

#if defined(FAKE_HOST_IPC_UT)
#include "audio_messenger_ipi.h"
#include "audio_task.h"
#include "adsp_ipi.h"
#include "audio_shared_info.h"
#include "driver_api.h"
#include "adsp_ipi_platform.h"

extern struct adsp_common_info* adsp_info;
extern uint32_t g_dsp_sram_buf_addr;
extern uint32_t g_dsp_sram_buf_sz;
extern uint32_t g_dsp_sram_buf_offset;
DSP_MSG_HW_PARAM g_dsp_hw_param_ack;
uint32_t *g_pHwOffset;
uint32_t *g_pApplOffset;

void fake_host_ipc_register(void);

#define FAKE_HOST_IPC_DEBUG
//#define PROCESSING_TIME_CHECK

void dsp_ul_irq_handler(struct ipi_msg_t *ipi_msg)
{
    uint32_t src_addr_copy;
    int32_t copy_size, buf_to_end;
    // Check available ul data
#if defined(PROCESSING_TIME_CHECK)
    unsigned long long systimer_start,systimer_end, systimer_diff;
    systimer_start = read_systimer_stamp_ns();
#endif
    copy_size =  *g_pHwOffset - *g_pApplOffset;

    if(copy_size < 0)
    {
#if defined(FAKE_HOST_IPC_DEBUG)
        PRINTF_E("hw offset 0x%x, appl offset 0x%x, size_bytes %d, copy_size %d\n",
            *g_pHwOffset, *g_pApplOffset,g_dsp_hw_param_ack.SharedRingBuffer.size_bytes, copy_size);
#endif
        copy_size += g_dsp_hw_param_ack.SharedRingBuffer.size_bytes;
    }

    src_addr_copy =  g_dsp_hw_param_ack.SharedRingBuffer.start_addr + *g_pApplOffset;
#if defined(FAKE_HOST_IPC_DEBUG)
    PRINTF_E("%s+ start addr 0x%x, appl offset 0x%x, hw offset 0x%x, ring buffer size %d, data copy from 0x%x to 0x%x, copy_size %d.\n", __func__,
            g_dsp_hw_param_ack.SharedRingBuffer.start_addr, *g_pHwOffset, *g_pApplOffset,
            g_dsp_hw_param_ack.SharedRingBuffer.size_bytes,
            src_addr_copy, g_dsp_sram_buf_offset, copy_size);
#endif
if(g_dsp_sram_buf_offset < (g_dsp_sram_buf_sz + g_dsp_sram_buf_addr))
{
    buf_to_end = g_dsp_hw_param_ack.SharedRingBuffer.size_bytes - *g_pApplOffset;
    if(copy_size <= buf_to_end)
    {
        memcpy((void *)g_dsp_sram_buf_offset, (void *)src_addr_copy, copy_size);
        g_dsp_sram_buf_offset += copy_size;
    } else {
        memcpy((void *)g_dsp_sram_buf_offset, (void *)src_addr_copy, buf_to_end);
        g_dsp_sram_buf_offset += buf_to_end;
        memcpy((void *)g_dsp_sram_buf_offset, (void *) (g_dsp_hw_param_ack.SharedRingBuffer.start_addr), copy_size- buf_to_end);
        g_dsp_sram_buf_offset += (copy_size- buf_to_end);
    }
}

   *g_pApplOffset = *g_pHwOffset;
#if defined(PROCESSING_TIME_CHECK)
    systimer_end = read_systimer_stamp_ns();
    systimer_diff = systimer_end - systimer_start;
    PRINTF_E("%s t(s %llu,e %llu,diff %llu)\n", __func__, systimer_start, systimer_end, systimer_diff);
#endif
}

void dsp_hw_param_ack_handler(struct ipi_msg_t *ipi_msg)
{
    AUDIO_IPC_COPY_DSP_HW_PARAM(ipi_msg->payload, &g_dsp_hw_param_ack);
    g_pHwOffset = (uint32_t *)g_dsp_hw_param_ack.SharedRingBuffer.ptr_to_hw_offset_bytes;
    g_pApplOffset = (uint32_t *)g_dsp_hw_param_ack.SharedRingBuffer.ptr_to_appl_offset_bytes;

#if defined(FAKE_HOST_IPC_DEBUG)
    PRINTF_E("%s %dbits, ch%d, fs %d, pz %d,pc %d.\n", __func__,
            g_dsp_hw_param_ack.bitwidth, g_dsp_hw_param_ack.channel_num,
            g_dsp_hw_param_ack.sample_rate, g_dsp_hw_param_ack.period_size,
            g_dsp_hw_param_ack.period_count);
    PRINTF_E("SharedRingBuffer start addr 0x%x, size_bytes %d, dir %d, appl ptr 0x%x, hw ptr 0x%x, hw ofs flag %d\n",
            g_dsp_hw_param_ack.SharedRingBuffer.start_addr,
            g_dsp_hw_param_ack.SharedRingBuffer.size_bytes,
            g_dsp_hw_param_ack.SharedRingBuffer.ring_buffer_dir,
            g_dsp_hw_param_ack.SharedRingBuffer.ptr_to_appl_offset_bytes,
            g_dsp_hw_param_ack.SharedRingBuffer.ptr_to_hw_offset_bytes,
            g_dsp_hw_param_ack.SharedRingBuffer.hw_offset_flag);
    PRINTF_E("g_pHwOffset %p, g_pApplOffset %p.\n", g_pHwOffset, g_pApplOffset);
#endif
}

void dsp_dsp_param_ack_handler(struct ipi_msg_t *ipi_msg)
{
#if defined(FAKE_HOST_IPC_DEBUG)
    DSP_MSG_HW_PARAM g_dsp_dsp_hw_param_ack;
    AUDIO_IPC_COPY_DSP_HW_PARAM(ipi_msg->payload, &g_dsp_dsp_hw_param_ack);
    PRINTF_D("%s : %dbits, ch%d, fs %d, pz %d,pc %d.\n", __func__,
            g_dsp_dsp_hw_param_ack.bitwidth, g_dsp_dsp_hw_param_ack.channel_num,
            g_dsp_dsp_hw_param_ack.sample_rate, g_dsp_dsp_hw_param_ack.period_size,
            g_dsp_hw_param_ack.period_count);
#endif
}

static void fake_host_audio_ipi_msg_dispatcher(int id, void *data, unsigned int len)
{
    ipi_msg_t *p_ipi_msg = NULL;

    //PRINTF_E("%s+\n",__func__);

    p_ipi_msg = (ipi_msg_t *)data;
    //TODO handle all message here without dispatch to different scenes.
    if (p_ipi_msg->ack_type == AUDIO_IPI_MSG_ACK_BACK) {
        switch(p_ipi_msg->msg_id) {
        case MSG_TO_DSP_HOST_HW_PARAMS:
            dsp_hw_param_ack_handler(p_ipi_msg);
            break;
        case MSG_TO_DSP_DSP_HW_PARAMS:
            dsp_dsp_param_ack_handler(p_ipi_msg);
            break;
        default:
            PRINTF_E("%s : Error : Unknow ack message %d\n", __func__, p_ipi_msg->msg_id);
            break;
        }
    } else {
        switch(p_ipi_msg->msg_id) {
        case MSG_TO_HOST_DSP_IRQUL:
            dsp_ul_irq_handler(p_ipi_msg);
            break;
        case MSG_TO_HOST_DSP_IRQDL:
            //TODO
            //dsp_dl_irq_handler(p_ipi_msg);
            break;
        case MSG_TO_HOST_VA_KEYWORD_PASS:
            //TODO
            //dsp_va_keyword_pass_handler(p_ipi_msg);
            break;
        case MSG_TO_HOST_DSP_AUDIO_READY:
            PRINTF_D("%s : DSP AUDIO REAY!\n",__func__);
            break;
        default:
            PRINTF_E("%s : Error : Unknow dsp message\n",__func__);
            break;
        }
    }
}

void fake_host_ipc_register(void)
{
    PRINTF_E("%s+\n",__func__);
    ipi_status retval = ERROR;

    retval = adsp_ipi_registration(IPI_AUDIO_FAKE_HOST, (ipi_handler_t)fake_host_audio_ipi_msg_dispatcher, "fake host ipc");

    if (retval != DONE)
        PRINTF_E("%s(), fake host ipi register fail!!", __func__);
}
#endif
