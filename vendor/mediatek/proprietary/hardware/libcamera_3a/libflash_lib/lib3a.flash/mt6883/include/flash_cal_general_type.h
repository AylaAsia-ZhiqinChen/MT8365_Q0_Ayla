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

#ifndef _FLASH_CAL_GENERAL_TYPE_H_
#define _FLASH_CAL_GENERAL_TYPE_H_

#include "flash_param.h"
#include "flash_awb_param.h"
#include "FlashAlg.h"

using namespace NS3A;

typedef enum
{
    FLASH_CAL_INPUT_P_YTAB,
    FLASH_CAL_INPUT_P_DUTY_I,
    FLASH_CAL_INPUT_P_DUTY_LTI,
    FLASH_CAL_INPUT_P_CAL_DATA,
    FLASH_CAL_INPUT_P_GOLDEN_WB_GAIN,
    FLASH_CAL_INPUT_CAL_NUM,
    FLASH_CAL_INPUT_TOTAL_STEP,
    FLASH_CAL_INPUT_TOTAL_STEP_LT,
    FLASH_CAL_INPUT_DUAL_FLASH_ENABLE,
} FlashCalGeneralInputArgument;

typedef enum
{
    FLASH_CAL_OUTPUT_P_CHOOSE,
    FLASH_CAL_OUTPUT_P_OUT_WB,
    FLASH_CAL_OUTPUT_P_OUT_YTAB,
} FlashCalGeneralOutputArgument;

typedef struct
{
    int type;
    void* data;
    int size;
} FlashCalUnit;

typedef struct
{
    int number;
    FlashCalUnit* units;
} FlashCalGroup;

/* Flash API Input */
typedef struct
{
    short* input;
} FlashCalInput_ShortPtr;

typedef struct
{
    int input;
} FlashCalInput_Int;

typedef struct
{
    bool input;
} FlashCalInput_Bool;

typedef struct
{
    CalData* input;
} FlashCalInput_CalData;

typedef struct
{
    FLASH_AWB_ALGO_GAIN_N_T* input;
} FlashCalInput_FLASH_AWB_ALGO_GAIN_N_T;

/* Flash API Output */
typedef struct
{
    short* output;
} FlashCalOutput_ShortPtr;

typedef struct
{
    ChooseResult* output;
} FlashCalOutput_ChooseResult;

typedef struct
{
    AWB_GAIN_T* output;
} FlashCalOutput_AWB_GAIN_T;

#endif
