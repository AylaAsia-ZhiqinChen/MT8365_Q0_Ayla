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

#ifndef _AFE_DRV_PCM_H_
#define _AFE_DRV_PCM_H_

#include <stdint.h>
#include "mt8168-afe-common.h"
#include "mtk-base-afe.h"
#include "audio_if_structure.h"

//#include "api_data_structure.h"
// define in api_data_structure.h start==
enum {
    DSP_VUL2, /* to-do */
    DSP_TDM_IN,
    DSP_VUL,
    DSP_PATH_NUM
};

enum mt8168_data_dir {
    DIR_OUT,
    DIR_IN,
    DIR_NUM,
};

/* path data */
struct dsp_audio_ops;
#define MAX_PATH_SIZE 2
#define MAX_OPS_SIZE 2

/* Now just use memif, fe_ops irq */
struct dsp_path_data {
    int occupy;
    struct mtk_base_afe_memif* memif;
    AUDIO_FMT_CONFIG memif_config;

    int fe_num;
    int be_num;
    struct dsp_audio_ops* fe_ops[MAX_OPS_SIZE];
    struct dsp_audio_ops* be_ops[MAX_OPS_SIZE];

    int start_seq;
    int stop_seq;

    int path_set;
    int path[MAX_PATH_SIZE][2];

    int path_type; /* TDM, I2S......etc */

    uintptr_t phy_addr;
    AUDIO_DMA_ADDR dma_data;

    struct mtk_base_afe_irq *irq;
    void (*irq_callback)(int);

    void *path_priv; /* TDM, I2S......etc */
};

/*** reg type ***/

enum {
    REGMAP_TYPE_AFE,
    REGMAP_TYPE_SIZE,
};

/* get all the path */
struct dsp_path_data* aud_drv_get_path_data(void);

int audio_driver_init(void);
void audio_driver_deinit(void);
#endif
