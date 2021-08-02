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

#include <stdlib.h>
#include <string.h>
#include "afe_drv_ops_implement.h"
#include "afe_drv_pcm.h"
#include "afe_drv_reg_rw.h"
#include "audio_rtos_header_group.h"
#include "mt8168-afe-reg.h"
#include "mt8168-afe-common.h"
#include "afe_drv_misc.h"
#include <interrupt.h>
#include "audio_drv_log.h"
#include "driver_api.h"
#include "mt_reg_base.h"
#include "types.h"
#include "semphr.h"

extern struct dsp_audio_ops g_afe_general_fe_ops;

/* constant */
static const struct mtk_base_memif_data g_memif_data[MT8168_AFE_MEMIF_NUM] = {
    [MT8168_AFE_MEMIF_VUL2] = {
        .name = "VUL2",
        .id = MT8168_AFE_MEMIF_VUL2,
        .reg_ofs_base = AFE_VUL_D2_BASE,
        .reg_ofs_cur = AFE_VUL_D2_CUR,
        .enable_reg = AFE_DAC_CON0,
        .enable_shift = 9,
    },
    [MT8168_AFE_MEMIF_TDM_IN] = {
        .name = "TDM IN",
        .id = MT8168_AFE_MEMIF_TDM_IN,
        .reg_ofs_base = AFE_HDMI_IN_2CH_BASE,
        .reg_ofs_cur = AFE_HDMI_IN_2CH_CUR,
        .enable_reg = AFE_HDMI_IN_2CH_CON0,
        .enable_shift = 0,
    },
    [MT8168_AFE_MEMIF_VUL] = {
        .name = "VUL IN",
        .id = MT8168_AFE_MEMIF_VUL,
        .reg_ofs_base = AFE_VUL_BASE,
        .reg_ofs_cur = AFE_VUL_CUR,
        .enable_reg = AFE_DAC_CON0,
        .enable_shift = 3,
    },
};
static struct mtk_base_afe_memif g_memif_afe[MT8168_AFE_MEMIF_NUM] = {
         [MT8168_AFE_MEMIF_VUL2] = { 
             .data = &g_memif_data[MT8168_AFE_MEMIF_VUL2],
             .irq_usage = MT8168_AFE_IRQ7,
             .const_irq = 1,
         },
         [MT8168_AFE_MEMIF_TDM_IN] = { 
             .data = &g_memif_data[MT8168_AFE_MEMIF_TDM_IN],
             .irq_usage = MT8168_AFE_IRQ10,
             .const_irq = 1,
         },
         [MT8168_AFE_MEMIF_VUL] = {
             .data = &g_memif_data[MT8168_AFE_MEMIF_VUL],
             .irq_usage = MT8168_AFE_IRQ4,
             .const_irq = 1,
         },
    };

static struct dsp_path_data g_dsp_path_data[] = {
    [DSP_VUL2] = {
        .memif = &g_memif_afe[MT8168_AFE_MEMIF_VUL2],
        .fe_num = 1,
        .be_num = 0,
        .fe_ops = {&g_afe_general_fe_ops},
    },

    [DSP_TDM_IN] = {
        .memif = &g_memif_afe[MT8168_AFE_MEMIF_TDM_IN],
        .fe_num = 1,
        .be_num = 0,
        .fe_ops = {&g_afe_general_fe_ops},
    },

    [DSP_VUL] = {
        .memif = &g_memif_afe[MT8168_AFE_MEMIF_VUL],
        .fe_num = 1,
        .be_num = 0,
        .fe_ops = {&g_afe_general_fe_ops},
    },
};

static const struct mtk_base_irq_data g_irq_data[MT8168_AFE_IRQ_NUM] = {
    [MT8168_AFE_IRQ7] = {
        .id = MT8168_AFE_IRQ7,
        .irq_cnt_reg = AFE_IRQ_MCU_CNT7,
        .irq_cnt_shift = 0,
        .irq_cnt_maskbit = 0x3ffff,
        .irq_en_reg = AFE_IRQ_MCU_CON,
        .irq_en_shift = 14,
        .irq_fs_reg = AFE_IRQ_MCU_CON,
        .irq_fs_shift = 24,
        .irq_fs_maskbit = 0xf,
        .irq_clr_reg = AFE_IRQ_MCU_CLR,
        .irq_clr_shift = 6,
    },
    [MT8168_AFE_IRQ10] = {
        .id = MT8168_AFE_IRQ10,
        .irq_cnt_reg = AFE_IRQ_MCU_CNT10,
        .irq_cnt_shift = 0,
        .irq_cnt_maskbit = 0x3ffff,
        .irq_en_reg = AFE_IRQ_MCU_CON2,
        .irq_en_shift = 4,
        .irq_fs_reg = -1,
        .irq_fs_shift = 0,
        .irq_fs_maskbit = 0x0,
        .irq_clr_reg = AFE_IRQ_MCU_CLR,
        .irq_clr_shift = 9,
    },
    [MT8168_AFE_IRQ4] = {
        .id = MT8168_AFE_IRQ4,
        .irq_cnt_reg = AFE_IRQ_MCU_CNT4,
        .irq_cnt_shift = 0,
        .irq_cnt_maskbit = 0x3ffff,
        .irq_en_reg = AFE_IRQ_MCU_CON,
        .irq_en_shift = 3,
        .irq_fs_reg = -1,
        .irq_fs_shift = 20,
        .irq_fs_maskbit = 0xf,
        .irq_clr_reg = AFE_IRQ_MCU_CLR,
        .irq_clr_shift = 3,
    },
};

#if 0 /* fix IRQ relation ship in memif description */
static int memif_specified_irqs[MT8570_AFE_MEMIF_NUM] = {
    [MT8168_AFE_MEMIF_VUL2] = MT8168_AFE_IRQ7,
    [MT8168_AFE_MEMIF_TDM_IN] = MT8168_AFE_IRQ10,
};
#endif

/* TODO fix use IRQ now */
static struct mtk_base_afe_irq g_irqs[MT8168_AFE_IRQ_NUM];

static void mt8168_afe_irq_init(void)
{
    int i;
    struct dsp_path_data* dsp_data = aud_drv_get_path_data();

    for (i = 0; i < DSP_PATH_NUM; i++) {
        struct mtk_base_afe_memif *memif = dsp_data[i].memif;

        if (memif->irq_usage < 0)
            continue;

        g_irqs[memif->irq_usage].irq_data = &(g_irq_data[memif->irq_usage]);
        g_irqs[memif->irq_usage].irq_occupyed = 1;
        dsp_data[i].irq = &(g_irqs[memif->irq_usage]);
    }

}

static void mt8168_afe_irq_handler(void)
{
    unsigned int status;
    unsigned int dsp_irq_mask;
    int i;
    struct dsp_path_data* dsp_data = aud_drv_get_path_data();

    status = aud_drv_get_reg(REGMAP_TYPE_AFE, AFE_IRQ_MCU_STATUS);
    dsp_irq_mask = aud_drv_get_reg(REGMAP_TYPE_AFE, AFE_IRQ_MCU_DSP_EN);

#ifdef AUDIO_LOG_DEBUG
    //AUD_DRV_LOG_D("+%s status=0x%x\n", __func__,status);
#endif
    status &= dsp_irq_mask;
    for (i = 0; i < DSP_PATH_NUM; i++) {
            struct mtk_base_afe_memif *memif = dsp_data[i].memif;
            int shift = dsp_data[i].irq->irq_data->irq_clr_shift;

            if (memif->irq_usage < 0)
                continue;

            if (((status & 1U << (shift)) != 0) &&
                (dsp_data[i].irq_callback != NULL)) {
                dsp_data[i].irq_callback(i);
            }
    }

    /* clear IRQ status*/
    /* TODO hardware semphore protect between dsp and cpu */
    aud_drv_set_reg_check_addr_val_mask(REGMAP_TYPE_AFE,
                                        AFE_IRQ_MCU_CLR,
                                        status,
                                        AFE_IRQ_STATUS_BITS);
}

/* constant end */

/* global variable */
/* yes, it can be dangerous,
   but we have no idea if the DSP OS can pass private data. */

struct dsp_path_data* aud_drv_get_path_data(void){
    return g_dsp_path_data;
}

struct mtk_base_afe_memif* aud_drv_get_afe_memif(void){
    return g_memif_afe;
}

SemaphoreHandle_t g_afe_ctrl_lock = NULL;

int audio_driver_init(void)
{
#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("+%s\n", __func__);
#endif

    aud_drv_regmap_set_size(REGMAP_TYPE_SIZE);
    aud_drv_regmap_set_addr_range(REGMAP_TYPE_AFE, (void*)AFE_REG_BASE,
                   AFE_REG_SIZE);

    mt8168_afe_irq_init();

    /*register irq handler*/
    request_irq(AUDIO_IRQn, mt8168_afe_irq_handler, "Afe_ISR_Handle");

    g_afe_ctrl_lock = xSemaphoreCreateBinary();

#ifdef AUDIO_LOG_DEBUG
    AUD_DRV_LOG_D("-%s\n", __func__);
#endif

    return 0;
}

/* free memory, might not used */
void audio_driver_deinit(void)
{
}
