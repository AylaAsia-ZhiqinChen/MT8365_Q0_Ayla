/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
* MediaTek Inc. (C) 2018. All rights reserved.
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
#include "afe_drv_pcm.h"
#include "afe_drv_misc.h"
#include "audio_rtos_header_group.h"
#include "semphr.h"
#include "driver_api.h"
#include "mt_reg_base.h"
#include "audio_drv_log.h"
#include "afe_drv_dts.h"

#define PCM_DATA_SAMPLING_RATE                   16000
#define PCM_EREF_SAMPLING_RATE                   48000
#define PCM_DATA_CHANNEL_NUM                         2
#define PCM_EREF_CHANNEL_NUM                         2
#define PCM_BITWIDTH                                32
#define PCM_BITWIDTH_BYTE          (PCM_BITWIDTH >> 3)
#define PCM_PERIOD_SIZE     PCM_DATA_SAMPLING_RATE/100  // default 10ms per period
#define PCM_RECORD_TIME                              3  // record time ,unit: sec

//#define ETDM_RECORD_PATH
//#define LOOPBACK_PATH
//#define INT_EREF_ETDMIN3
#define INT_EREF_DLM

#define DSP_SRAM_BUFFER_ADDR                  (0x1FCC0000)  // 4M SRAM
#if defined(FPGA)
#define DSP_SRAM_BUFFER_SIZE                  (0x40000)     // 256K
#elif defined(INT_EREF_ETDMIN3) || defined(INT_EREF_DLM)
#define DSP_SRAM_BUFFER_SIZE PCM_DATA_SAMPLING_RATE \
                             * (PCM_DATA_CHANNEL_NUM + PCM_EREF_CHANNEL_NUM) \
                             * PCM_BITWIDTH_BYTE * PCM_RECORD_TIME
#else
#define DSP_SRAM_BUFFER_SIZE PCM_DATA_SAMPLING_RATE \
                             * PCM_DATA_CHANNEL_NUM \
                             * PCM_BITWIDTH_BYTE * PCM_RECORD_TIME
#endif

void record_synced_api_demo(void)
{
#if defined(FPGA)
    AUDIO_FMT_CONFIG pcm_setting = {
        .sample_rate = 16000,
        .channel_num = 8,
        .bitwidth = 32,
        .period_size = 160, // 10ms
        .period_count = 2,
    };
#else
    AUDIO_FMT_CONFIG pcm_setting = {
        .sample_rate = PCM_DATA_SAMPLING_RATE,
        .channel_num = PCM_DATA_CHANNEL_NUM,
        .bitwidth = PCM_BITWIDTH,
        .period_size = PCM_PERIOD_SIZE, // 10ms
        .period_count = 2,
    };
#endif

#ifndef ETDM_RECORD_PATH
    int record_path_handler = DSP_DMIC;
#else
    int record_path_handler = DSP_TDM_IN_3;
    TDM_CONFIG tdm_setting = {
        .tdm_cfg_shared = {
            .mclk_freq = PCM_DATA_SAMPLING_RATE * 256,
            .slave_mode = 1,
            .lrck_invert = 0,
            .lrck_width = PCM_BITWIDTH,
            .bck_invert = 0,
            .format = ETDM_FORMAT_I2S,
            .bck_per_channel = PCM_BITWIDTH,
            .data_mode = TDMIN3_DATA_MODE,
            .clock_mode = TDMIN3_CLOCK_MODE,
        },
    };

    aud_drv_set_tdm_config(record_path_handler, tdm_setting);
    //aud_drv_set_tdm_use_sgen(record_path_handler, 1);
#endif


#ifdef INT_EREF_ETDMIN3
    pcm_setting.channel_num = PCM_DATA_CHANNEL_NUM + PCM_EREF_CHANNEL_NUM;

    DMIC_CONFIG dmic_setting = {
        .channel_num = PCM_DATA_CHANNEL_NUM,
    };
    aud_drv_set_dmic_config(record_path_handler, dmic_setting);

    int eref_path_handler = DSP_INT_EREF_TDMIN3;
    TDM_CONFIG tdm_setting = {
        .tdm_cfg_shared = {
            .mclk_freq = PCM_EREF_SAMPLING_RATE * 256,
            .slave_mode = 1,
            .lrck_invert = 0,
            .lrck_width = PCM_BITWIDTH,
            .bck_invert = 0,
            .format = ETDM_FORMAT_I2S,
            .bck_per_channel = PCM_BITWIDTH,
            .data_mode = TDMIN3_DATA_MODE,
            .clock_mode = TDMIN3_CLOCK_MODE,
         },
        .channel_num = PCM_EREF_CHANNEL_NUM,
    };
    aud_drv_set_tdm_config(eref_path_handler, tdm_setting);
    //aud_drv_set_tdm_use_sgen(eref_path_handler, 1);
#endif

#ifdef INT_EREF_DLM
    pcm_setting.channel_num = PCM_DATA_CHANNEL_NUM + PCM_EREF_CHANNEL_NUM;

    DMIC_CONFIG dmic_setting = {
        .channel_num = PCM_DATA_CHANNEL_NUM,
    };
    aud_drv_set_dmic_config(record_path_handler, dmic_setting);
#endif

#ifdef LOOPBACK_PATH
    int loopback_path_handler = DSP_TDM_OUT_2;
    tdm_setting.tdm_cfg_shared.slave_mode = 0, // master
    aud_drv_set_tdm_config(loopback_path_handler, tdm_setting);
    aud_drv_set_tdm_use_sgen(loopback_path_handler, 1);

    aud_drv_set_loopback(MT8570_TDMI_3, MT8570_TDMO_2, 1); // sink, source, slave
#endif

    int ret;
    const uint32_t file_size = DSP_SRAM_BUFFER_SIZE; //for test purpose

    uintptr_t record_file_start_addr = DSP_SRAM_BUFFER_ADDR;
    uint32_t record_ptr_byte = 0;
    AUDIO_DMA_ADDR record_dma_addr;

    const uint32_t read_chunk_size = pcm_setting.channel_num * (pcm_setting.bitwidth / 8)
                     * pcm_setting.period_size;
    int real_read_size = 0;
    int real_read_size_frames = 0;


    int dma_buffer_size = pcm_setting.channel_num * (pcm_setting.bitwidth / 8)
                 * pcm_setting.period_size * pcm_setting.period_count;

    configASSERT( dma_buffer_size <= DSP_SRAM_BUFFER_SIZE );
    PRINTF_I("channel_num=%d, bitwidth=%d, period_size=%d, period_count=%d,dma_buffer_size=%d,file_size=%d.\n",
        pcm_setting.channel_num,pcm_setting.bitwidth, pcm_setting.period_size, pcm_setting.period_count,dma_buffer_size,file_size);

    memset(&record_dma_addr, 0, sizeof(AUDIO_DMA_ADDR));


#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("wait for start record...\n");
#endif
    unsigned int start_record = DRV_Reg32(DSP1_GPR09); // 0x1D063054
    while (start_record != 1) {
        start_record = DRV_Reg32(DSP1_GPR09);
    }
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("start_record=%d, start record!!\n", start_record);
#endif


#ifdef INT_EREF_ETDMIN3
    ret = aud_drv_pcm_open(eref_path_handler, pcm_setting,
                 NULL, &record_dma_addr);
    if (ret < 0) {
        PRINTF_E("eref aud_drv_pcm_open err! %d\n", ret);
        goto AUDIO_RECORD_PREPARE_FAILED;
    }
    ret = aud_drv_pcm_start(eref_path_handler);
    if (ret < 0) {
        PRINTF_E("eref aud_drv_pcm_start err! %d\n", ret);
        goto AUDIO_RECORD_START_FAILED;
    }
#endif

    ret = aud_drv_pcm_open(record_path_handler, pcm_setting,
                 NULL, &record_dma_addr);
    if (ret < 0) {
        PRINTF_E("data aud_drv_pcm_open err! %d\n", ret);
        goto AUDIO_RECORD_PREPARE_FAILED;
    }
    ret = aud_drv_pcm_start(record_path_handler); //start record and IRQ
    if (ret < 0) {
        PRINTF_E("data aud_drv_pcm_start err! %d\n", ret);
        goto AUDIO_RECORD_START_FAILED;
    }

#ifdef LOOPBACK_PATH
    ret = aud_drv_pcm_open(loopback_path_handler, pcm_setting,
                 NULL, &record_dma_addr);
    if (ret < 0) {
        PRINTF_E("loopback aud_drv_pcm_open err! %d\n", ret);
        goto AUDIO_RECORD_PREPARE_FAILED;
    }
    ret = aud_drv_pcm_start(loopback_path_handler);
    if (ret < 0) {
        PRINTF_E("loopback aud_drv_pcm_start err! %d\n", ret);
        goto AUDIO_RECORD_START_FAILED;
    }
#endif

    while(record_ptr_byte != file_size){
        real_read_size = read_chunk_size;
        if (file_size - record_ptr_byte < real_read_size)
            real_read_size = file_size - record_ptr_byte;
        real_read_size_frames = aud_drv_pcm_bytes_to_frames(record_path_handler, real_read_size);
        real_read_size_frames = aud_drv_pcm_read(record_path_handler,
                             (void*)(record_file_start_addr + record_ptr_byte),
                             real_read_size_frames);
        real_read_size = aud_drv_pcm_frames_to_bytes(record_path_handler, real_read_size_frames);
        if (real_read_size > 0)
            record_ptr_byte += real_read_size;
        else
            PRINTF_E("real_read_size err! %d\n", real_read_size);
    }

    PRINTF_E("record finish!!\n");
    PRINTF_E("record_file_start_addr 0x%x\n", record_file_start_addr);
    PRINTF_E("file_size 0x%x\n", file_size);
    PRINTF_E("record_dma_addr.start_addr 0x%x\n", (uintptr_t)record_dma_addr.start_addr);
    PRINTF_E("record_dma_addr.size_bytes 0x%x\n", record_dma_addr.size_bytes);

AUDIO_RECORD_START_FAILED:
#ifdef LOOPBACK_PATH
    aud_drv_pcm_stop(loopback_path_handler);
#endif

#ifdef INT_EREF_ETDMIN3
    aud_drv_pcm_stop(eref_path_handler);
#endif

    aud_drv_pcm_stop(record_path_handler);

AUDIO_RECORD_PREPARE_FAILED:
#ifdef LOOPBACK_PATH
    aud_drv_pcm_close(loopback_path_handler);
#endif

#ifdef INT_EREF_ETDMIN3
    aud_drv_pcm_close(eref_path_handler);
#endif

    aud_drv_pcm_close(record_path_handler);
}

#ifdef INT_EREF_DLM
#define PLAYBACK_GOLDEN_SIZE (TONE_LONGPERIOD_SIZE*8)
#define PLAYBACK_GOLDEN_SIZE_BYTES (PLAYBACK_GOLDEN_SIZE*4)
static uint32_t playback_golden[PLAYBACK_GOLDEN_SIZE];
static uint32_t playback_ptr_bytes;
#endif

void playback_synced_api_demo(void) {
#ifdef INT_EREF_DLM
    AUDIO_FMT_CONFIG pcm_setting = {
        .sample_rate = PCM_EREF_SAMPLING_RATE,
        .channel_num = PCM_EREF_CHANNEL_NUM,
        .bitwidth = PCM_BITWIDTH,
        .period_size = PCM_EREF_SAMPLING_RATE / 2000, // 0.5ms
        .period_count = 2,
    };

    int playback_path_handler = DSP_INT_EREF_DLM;
    TDM_CONFIG tdm_setting = {
        .tdm_cfg_shared = {
            .mclk_freq = PCM_EREF_SAMPLING_RATE * 256,
            .slave_mode = 0,
            .lrck_invert = 0,
            .lrck_width = PCM_BITWIDTH,
            .bck_invert = 0,
            .format = ETDM_FORMAT_I2S,
            .bck_per_channel = PCM_BITWIDTH,
            .data_mode = TDMOUT2_DATA_MODE,
            .clock_mode = TDMOUT2_CLOCK_MODE,
        },
    };
    aud_drv_set_tdm_config(playback_path_handler, tdm_setting);


    int ret;
    AUDIO_DMA_ADDR playback_dma_addr;
    memset(&playback_dma_addr, 0, sizeof(AUDIO_DMA_ADDR));

    int write_size;
    int write_size_frames;
    int debug_cnt = 0;

    int dma_buffer_size = pcm_setting.channel_num * (pcm_setting.bitwidth / 8)
                 * pcm_setting.period_size * pcm_setting.period_count;

    //get golden data
    aud_drv_fill_test_data(playback_golden, PLAYBACK_GOLDEN_SIZE_BYTES, PCM_EREF_CHANNEL_NUM,
                   PCM_BITWIDTH, 1, 0);
    playback_ptr_bytes = 0;


    aud_drv_pcm_open(playback_path_handler, pcm_setting,
            NULL, &playback_dma_addr);
    if (ret < 0) {
        PRINTF_E("aud_drv_pcm_open err! %d\n", ret);
        goto AUDIO_PLAYBACK_PREPARE_WRITE_FAILED;
    }

    configASSERT(PLAYBACK_GOLDEN_SIZE_BYTES > dma_buffer_size);
    if (PLAYBACK_GOLDEN_SIZE_BYTES < dma_buffer_size) {
        PRINTF_E("dma_buffer_size(0x%x) must be less than PLAYBACK_GOLDEN_SIZE_BYTES(0x%x)\n",
                dma_buffer_size, PLAYBACK_GOLDEN_SIZE_BYTES);
        goto AUDIO_PLAYBACK_PREPARE_WRITE_FAILED;
    }
    write_size_frames = aud_drv_pcm_bytes_to_frames(playback_path_handler, dma_buffer_size);
    write_size_frames = aud_drv_pcm_write(playback_path_handler,
                playback_golden, write_size_frames);
    if (write_size_frames < 0) {
        PRINTF_E("first aud_drv_pcm_write err! %d\n", ret);
        goto AUDIO_PLAYBACK_PREPARE_WRITE_FAILED;
    }


    write_size = aud_drv_pcm_frames_to_bytes(playback_path_handler, write_size_frames);
    playback_ptr_bytes += write_size;

    //we expected we will write success when first write
    configASSERT(playback_ptr_bytes == dma_buffer_size);

    while (1) {
        write_size = PLAYBACK_GOLDEN_SIZE_BYTES - playback_ptr_bytes;
        write_size_frames = aud_drv_pcm_bytes_to_frames(playback_path_handler, write_size);
        write_size_frames = aud_drv_pcm_write(playback_path_handler,
                    playback_golden, write_size_frames);
        write_size = aud_drv_pcm_frames_to_bytes(playback_path_handler, write_size_frames);
        playback_ptr_bytes += write_size;
        configASSERT(playback_ptr_bytes <= PLAYBACK_GOLDEN_SIZE_BYTES);
        if (playback_ptr_bytes == PLAYBACK_GOLDEN_SIZE_BYTES) {
            playback_ptr_bytes = 0;
            debug_cnt = 0;
        } else {
            debug_cnt++;
        }

        if (debug_cnt > 20) {
            //hang? assert in development stage
            //TODO remove it
            configASSERT(0);
        }
    }

    aud_drv_pcm_stop(playback_path_handler);

AUDIO_PLAYBACK_PREPARE_WRITE_FAILED:
    aud_drv_pcm_close(playback_path_handler);
#else
    PRINTF_E("not define INT_EREF_DLM, do nothing\n");
    return;
#endif
}
