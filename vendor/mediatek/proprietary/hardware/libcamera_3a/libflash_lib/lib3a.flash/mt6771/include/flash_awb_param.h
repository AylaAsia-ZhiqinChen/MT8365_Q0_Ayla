/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef _FLASH_AWB_PNVRAM_H
#define _FLASH_AWB_PNVRAM_H

//#include <camera_custom_nvram.h>
#include "camera_flash_algo_tuning.h"

//#define FLASH_AWB_PC_SIM

#define FLASH_AWB_PARAM_H_REVISION    5363001

typedef struct
{
   MINT32     flashDuty;
   MINT32     flashStep;
   MUINT32    flashAwbWeight;
   AWB_GAIN_T FlashPreferenceGain;
}FLASH_AWB_PASS_FLASH_INFO_T;

typedef struct
{
   FLASH_AWB_ALGO_TUNING_PARAM_T flash_awb_tuning_param;
}FLASH_AWB_INIT_T, *PFLASH_AWB_INIT_T;

typedef struct
{
   MUINT32 Hr;
   MUINT32 Mr;
   MUINT32 Lr;
   MUINT32 Midx;
}FLASH_AWB_DISTANCE_INFO_T;


typedef struct
{
   int x1;
   int x2;
   int x3;
   int x4;
   int x5;

   int y0;
   int y1;
   int y2;
   int y3;
   int y4;
   int y5;
}REFLECTION_TBL_IDX;


typedef struct
{
   int   * pEstNoFlashY;
   int   * pEstFlashY;
   double* aeCoef;
   double* pureCoef;

   int   i4SceneLV; // current scene LV

   int   FlashDuty;
   int   FlashStep;
}FLASH_AWB_INPUT_T;


#ifndef FLASH_AWB_IF_PARAM
#define FLASH_AWB_IF_PARAM

typedef struct
{
   AWB_GAIN_T NoFlashWBGain;
   AWB_GAIN_T PureFlashWBGain;
   MUINT32    PureFlashWeight;
   AWB_GAIN_T FlashPreferenceGain[FLASH_LV_INDEX_NUM];
}FLASH_AWB_CAL_GAIN_INPUT_T;

typedef struct
{
   AWB_GAIN_T rAWBGain;
} FLASH_AWB_OUTPUT_T;

#endif


#endif
