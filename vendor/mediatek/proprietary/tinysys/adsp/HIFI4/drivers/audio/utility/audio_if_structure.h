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
#ifndef __AUDIO_IF_STRUCTURE_H__
#define __AUDIO_IF_STRUCTURE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "audio_shared_info.h"

typedef struct Audio_Format_Config {
    uint32_t sample_rate;
    uint8_t channel_num;
    uint8_t bitwidth; /* 16bits or 32bits */
    uint32_t period_size; /* in frames */
    uint32_t period_count;
} AUDIO_FMT_CONFIG;

typedef struct TDM_config {
    struct TDM_config_shared tdm_cfg_shared;
    uint8_t channel_num;
} TDM_CONFIG;

enum i2s_coclock_mode {
    I2S_COCLK,
    I2S_SEPCLK,
};

typedef struct I2S_config {
    uint8_t i2s_slave;
    uint8_t i2s_mode_on; //if 0, no data delay one bit after LRCK edge
    uint8_t RJ_on;
    unsigned int mclk_rate;
    enum i2s_coclock_mode i2s_coclock_mode;
    uint8_t bck_ivt;
    uint8_t lrck_ivt;
} I2S_CONFIG;

typedef struct DMIC_config {
    uint8_t dmic_slave;
    uint8_t channel_num;
} DMIC_CONFIG;

/*** addr data ***/
typedef struct buffer_addr {
    void* start_addr;
    uint32_t size_bytes;
} AUDIO_DMA_ADDR;

// define in api_data_structure.h end==

typedef struct ioctl_TDM_config_inner {
    uint8_t lrck_half_delay; /* true or false */
    uint8_t lrck_invert; /* true or false */
    uint32_t lrck_width;
    uint8_t bck_invert; /* true or false */
    uint8_t i2s_format; /* true or false, if true, the MSB should/will be delayed one bck from the lrck edge */
    uint8_t data_half_delay; /* TDM out only, true or false */
    uint8_t is_coclock;
    uint8_t bck_per_channel;
} TDM_CONFIG_IOCTL;

typedef struct ioctl_TDM_config {
    int path_num;
    TDM_CONFIG_IOCTL tdm_config;
} IOCTL_TDM_CONFIG;

typedef struct ioctl_DMIC_config_inner {
    uint8_t lrck_half_delay; /* true or false */
    uint8_t lrck_invert; /* true or false */
    uint32_t lrck_width;
    uint8_t bck_invert; /* true or false */
    uint8_t i2s_format; /* true or false, if true, the MSB should/will be delayed one bck from the lrck edge */
    uint8_t data_half_delay; /* TDM out only, true or false */
    uint8_t is_coclock;
    uint8_t bck_per_channel;
} DMIC_CONFIG_IOCTL;


typedef struct ioctl_memif_config {
    int path_num;
    AUDIO_FMT_CONFIG memif_config;
} IOCTL_memif_config;

typedef struct ioctl_I2S_config_inner {
    uint8_t i2s_slave;
    uint8_t i2s_mode_on; //if 0, no data delay one bit after LRCK edge
    uint8_t RJ_on;
    unsigned int mclk_rate;
    uint8_t is_coclock;
    uint8_t bck_ivt;
    uint8_t lrck_ivt;
} I2S_CONFIG_IOCTL;

typedef struct ioctl_I2S_config {
    int path_num;
    I2S_CONFIG_IOCTL i2s_config;
} IOCTL_I2S_CONFIG;

#endif // end of __AUDIO_IF_STRUCTURE_H__

