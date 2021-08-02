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
#include <string.h>
#include "afe_drv_api_if.h"
#include "afe_drv_ops_dispatcher.h"
#include "audio_rtos_header_group.h"
#include "semphr.h"
#include "audio_drv_log.h"
#include "mt8168-afe-reg.h"
#include "afe_drv_misc.h"

//#define USE_DSP_SRAM
static int g_path_occupy_arr[DSP_PATH_NUM];

/* for synced irq */
static SemaphoreHandle_t g_sem_syned_irq_arr[DSP_PATH_NUM];
static uint32_t g_old_hw_ptr[DSP_PATH_NUM];

static void record_synced_irq_handler(int handler)
{
    static BaseType_t xNeedYield = pdFALSE;
#ifdef AUDIO_LOG_DEBUG
    //AUD_DRV_LOG_D("+%s, handler = %d.\n", __func__,handler);
#endif
    xSemaphoreGiveFromISR(g_sem_syned_irq_arr[handler], &xNeedYield);
    portYIELD_FROM_ISR(xNeedYield);
}

/*
 * aud_drv_pcm_open:
 *     open for stream path. It will let the path be occupied.
 *     you muse close it if you don't need it
 * parameter:
 *     handler: The path handler.
 *     stream_config: The path config.
 *     irq_handler: irq handler to get data, if null, the synced irq will be use
 *     addr_data: will update DMA addr and size into it
 * return value:
 *     if failed, return a negitive error num, otherwise return 0
 */

NORMAL_SECTION_FUNC int aud_drv_pcm_open(int handler, AUDIO_FMT_CONFIG pcm_config,
                   void (*irq_handler)(int),AUDIO_DMA_ADDR* addr_data)
{
    struct dsp_path_data* dsp_data = aud_drv_get_path_data();
    const struct mtk_base_memif_data *memif_data;
    int dma_buffer_size;
    int ret;
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s, handler = %d.\n", __func__,handler);
#endif

    if (handler < 0 || handler >= DSP_PATH_NUM) {
        PRINTF_E("handler error! DSP_PATH_NUM %d, handler %d\n",
            DSP_PATH_NUM, handler);
        return -1; //TODO!!
    }

    if (g_path_occupy_arr[handler]) {
        PRINTF_E("path %d is occupied\n", handler);
        return -1; //TODO!!
    }

    memcpy(&dsp_data[handler].memif_config,&pcm_config,sizeof(AUDIO_FMT_CONFIG));
    dsp_data[handler].memif_config = pcm_config;


    if (dsp_data[handler].memif) {
#ifdef AUDIO_LOG_DEBUG
        AUD_DRV_LOG_D("%s, memif exist\n", __func__);
#endif
        memif_data = dsp_data[handler].memif->data;
    } else {
#ifdef AUDIO_LOG_DEBUG
        AUD_DRV_LOG_D("%s, no memif\n", __func__);
#endif
        goto NO_MEMIF;
    }


#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s sample_rate %d, channel_num %d, bitwidth %d, period_size %d, period_count %d\n", __func__,
                   dsp_data[handler].memif_config.sample_rate, dsp_data[handler].memif_config.channel_num,
                   dsp_data[handler].memif_config.bitwidth, dsp_data[handler].memif_config.period_size,
                   dsp_data[handler].memif_config.period_count);
#endif

    if (irq_handler != NULL) {
        dsp_data[handler].irq_callback = irq_handler;
    } else {
        if (aud_drv_fe_id_to_dir(memif_data->id)==DIR_IN) {
            dsp_data[handler].irq_callback = record_synced_irq_handler;
            if (g_sem_syned_irq_arr[handler] == NULL) {
                g_sem_syned_irq_arr[handler] = xSemaphoreCreateBinary();
                configASSERT(g_sem_syned_irq_arr[handler] != NULL);
                g_old_hw_ptr[handler] = 0;
            }
        } else {
            //TODO for playback
        }
    }

    dma_buffer_size = dsp_data[handler].memif_config.channel_num * (dsp_data[handler].memif_config.bitwidth / 8)
        * dsp_data[handler].memif_config.period_size * dsp_data[handler].memif_config.period_count;


    //allocate DMA buffer
    if(addr_data->start_addr == NULL) {
#ifdef USE_DSP_SRAM
        // malloc dsp sram buffer
        addr_data->start_addr = kal_pvPortMalloc(dma_buffer_size);
        AUD_ASSERT(addr_data->start_addr != NULL);
        addr_data->size_bytes = dma_buffer_size;
        memset(addr_data->start_addr, 0, sizeof(addr_data->size_bytes));
#else
        addr_data->start_addr = (void*)((uintptr_t)(AFE_INTERNAL_SRAM_PHY_BASE));
        addr_data->size_bytes = dma_buffer_size;
        memset(addr_data->start_addr, 0, sizeof(addr_data->size_bytes));
        /*TODO: need to check how many remaining size can be used here!*/
        if(dma_buffer_size > AFE_INTERNAL_SRAM_SIZE)
            PRINTF_E("%s Error : dma_buffer_size(0x%x) is larger than AFE_INTERNAL_SRAM_SIZE(0x%x)!\n ",__func__, dma_buffer_size, AFE_INTERNAL_SRAM_SIZE);
#endif
     }
    dsp_data[handler].phy_addr = (uintptr_t)addr_data->start_addr;
    dsp_data[handler].dma_data.size_bytes = addr_data->size_bytes;
    dsp_data[handler].dma_data.start_addr = addr_data->start_addr;

NO_MEMIF:
    ret = do_pre_start_with_path_data(&dsp_data[handler]);
    g_path_occupy_arr[handler] = 1;
    return ret;
}

/*
 * aud_drv_pcm_start:
 *     start pcm streaming
 * parameter:
 *     handler: The path handler.
 * return value:
 *     if failed, return a negitive error num, otherwise return 0
 */

int aud_drv_pcm_start(int handler)
{
    struct dsp_path_data* dsp_data = aud_drv_get_path_data();
    int ret;

    PRINTF_I("+%s\n", __func__);
    ret = do_start_with_path_data(&dsp_data[handler]);
    return ret;
}

/*
 * aud_drv_pcm_get_ptr:
 *     get hw pointer
 * parameter:
 *     handler: The path handler.
 * return value:
 *     return the hw pointer of the path
 */

void* aud_drv_pcm_get_ptr(int handler)
{
    struct dsp_path_data* dsp_data = aud_drv_get_path_data();
    void* base_addr = dsp_data[handler].dma_data.start_addr;
    uint32_t dma_ptr_bias = get_hw_ptr_bias_with_path_data(&dsp_data[handler]);

    //return (void*)((uintptr_t)base_addr + dma_ptr_bias);
    return base_addr + dma_ptr_bias; //check this
}

static int read_once_from_dma(int handler, char* buf, int read_remain_size)
{
    struct dsp_path_data* dsp_data = aud_drv_get_path_data();
    uint32_t dma_size = dsp_data[handler].dma_data.size_bytes;
    uintptr_t start_addr = (uintptr_t)dsp_data[handler].dma_data.start_addr;
    uint32_t hw_ptr = ((uintptr_t)aud_drv_pcm_get_ptr(handler) - start_addr);
    uint32_t old_hw_ptr = g_old_hw_ptr[handler];
    int bytes_to_read;
    int read_twice;

    configASSERT( hw_ptr <= dma_size );

    if (hw_ptr >= old_hw_ptr) {
        bytes_to_read = hw_ptr - old_hw_ptr;
        read_twice = 0;
    } else {
        bytes_to_read = hw_ptr + dma_size - old_hw_ptr;
        read_twice = 1;
    }

    if (bytes_to_read > read_remain_size)
        bytes_to_read = read_remain_size;

    if (read_twice && (dma_size - old_hw_ptr > bytes_to_read))
        read_twice = 0;

    /* let's get data */
    if (!read_twice) {
        volatile_memcpy(buf, (void*)(start_addr + old_hw_ptr), bytes_to_read);
        g_old_hw_ptr[handler] += bytes_to_read;

    } else {
        uint32_t bytes_to_read_1 = dma_size - old_hw_ptr;
        uint32_t bytes_to_read_2 = bytes_to_read - bytes_to_read_1;
        volatile_memcpy(buf, (void*)(start_addr + old_hw_ptr), bytes_to_read_1);
        volatile_memcpy(buf + bytes_to_read_1, (void*)(start_addr),
                bytes_to_read_2);
        g_old_hw_ptr[handler] = bytes_to_read_2;
    }

    return bytes_to_read;
}

/*
 * aud_drv_pcm_read:
 *     read data. It will read data to buf.
 * parameter:
 *     handler: The path handler.
 *     buf: The read buffer.
 *     read_size: The data size we want to read.
 * return value:
 *     if failed, return a negitive error num, otherwise return real read size.
 */

int aud_drv_pcm_read(int handler, void* buf, int read_frames)
{

    char* char_buf = buf;
    int remain_size = aud_drv_pcm_frames_to_bytes(handler, read_frames);
    int ret;
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s, handler = %d,read_frames=%d\n", __func__,handler,read_frames);
#endif

    //read first time
    ret = read_once_from_dma(handler, char_buf, remain_size);
    remain_size -= ret;
    char_buf += ret;

    while (remain_size) {
        if (xSemaphoreTake(g_sem_syned_irq_arr[handler],
                   1000 * portTICK_PERIOD_MS) != pdTRUE) {
            PRINTF_I("wait more then 1s, might overflow?\n");
            PRINTF_I("(overflow should be checked in userspace?)\n");
        }
        ret = read_once_from_dma(handler, char_buf, remain_size);
        remain_size -= ret;
        char_buf += ret;
    }
    return read_frames;
}

/*
 * aud_drv_pcm_write:
 *     write data. It will get data from buf to dma.
 * parameter:
 *     handler: The path handler.
 *     buf: The playback buffer.
 *     read_size: The data size we want to write.
 * return value:
 *     if failed, return a negitive error num, otherwise return real read size.
 */

int aud_drv_pcm_write(int handler, void* buf, int read_size)
{
    return 0; //TODO
}

/*
 * aud_drv_pcm_stop:
 *     stop the path
 * parameter:
 *     handler: The path handler.
 * return value:
 *     none.
 */

int aud_drv_pcm_stop(int handler)
{
    struct dsp_path_data* dsp_data = aud_drv_get_path_data();
    int ret;

    PRINTF_I("+%s\n", __func__);
    ret = do_stop_with_path_data(&dsp_data[handler]);
    return ret;
}

/*
 * aud_drv_pcm_close:
 *     close the path, it will release the path and let it be available for other threads.
 * parameter:
 *     handler: The path handler.
 * return value:
 *     none.
 */

NORMAL_SECTION_FUNC int aud_drv_pcm_close(int handler)
{
    PRINTF_I("+%s\n", __func__);
    if (!g_path_occupy_arr[handler]) {
        PRINTF_E("path not pre_startd! handler %d\n", handler);
        return 0;
    }

    /* TODO free dma? */
    g_path_occupy_arr[handler] = 0;
    return 0;
}

/*
 * aud_drv_pcm_bytes_to_frames:
 *     change bytes to frames
 * parameter:
 *     handler: The path handler.
 *     bytes: The bytes number we want to convert.
 * return value:
 *     The frames.
 */

int aud_drv_pcm_bytes_to_frames(int handler, int bytes)
{
    struct dsp_path_data* dsp_data = aud_drv_get_path_data();
    int frame_size = dsp_data[handler].memif_config.bitwidth / 8
             * dsp_data[handler].memif_config.channel_num;
    return bytes / frame_size;
}

/*
 * aud_drv_pcm_frames_to_bytes:
 *     change frames to bytes
 * parameter:
 *     handler: The path handler.
 *     frames: The frames number we want to convert.
 * return value:
 *     The bytes.
 */

int aud_drv_pcm_frames_to_bytes(int handler, int frames)
{
    struct dsp_path_data* dsp_data = aud_drv_get_path_data();
    int frame_size = dsp_data[handler].memif_config.bitwidth / 8
             * dsp_data[handler].memif_config.channel_num;
    return frames * frame_size;
}

