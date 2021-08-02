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


#ifndef _FLASH_AE_GENERAL_TYPE_H_
#define _FLASH_AE_GENERAL_TYPE_H_

#include "flash_param.h"
#include "flash_awb_param.h"
#include "FlashAlg.h"

using namespace NS3A;

typedef enum{
    FLASH_AE_INPUT_P_STA_DATA,
    FLASH_AE_INPUT_P_EXP,
    FLASH_AE_INPUT_P_LAST_FRAME,
    FLASH_AE_INPUT_P_FACE_INFO,
    FLASH_AE_INPUT_P_STROBE_PROFILE,
    FLASH_AE_INPUT_P_PLINE,
    FLASH_AE_INPUT_P_TUNING,
    FLASH_AE_INPUT_P_FLASH_INFO,
    FLASH_AE_INPUT_P_DIR_NAME,
    FLASH_AE_INPUT_P_PRJ_NAME,
    FLASH_AE_INPUT_P_NUM,
    FLASH_AE_INPUT_P_ERR_BUF,
    FLASH_AE_INPUT_FLASH_TYPE,
    FLASH_AE_INPUT_LV,
    FLASH_AE_INPUT_ISO_AT_GAIN1X,
    FLASH_AE_INPUT_EV_COMP,
    FLASH_AE_INPUT_TAR,
    FLASH_AE_INPUT_EV_LEVEL,
    FLASH_AE_INPUT_IS_SAVE_BIN
} FlashAEGeneralInputArgument;

typedef enum{
    FLASH_AE_OUTPUT_P_IS_NEED_NEXT,
    FLASH_AE_OUTPUT_P_EXP_NEXT,
    FLASH_AE_OUTPUT_P_EXP,
    FLASH_AE_OUTPUT_P_IS_LOW_REF,
    FLASH_AE_OUTPUT_P_EQU_EXP,
    FLASH_AE_OUTPUT_P_DEBUG_INFO
} FlashAEGeneralOutputArgument;

typedef struct{
    int   type;
    void* data;
    int   size;
} FlashAEUnit;

typedef struct{
    int        number;
    FlashAEUnit* units;
} FlashAEGroup;

/* Flash API Input */
typedef struct{
    short* input;
} FlashAEInput_ShortPtr;

typedef struct{
    int input;
} FlashAEInput_Int;

typedef struct{
    int* input;
} FlashAEInput_IntPtr;

typedef struct{
    float input;
} FlashAEInput_Float;

typedef struct{
    const char* input;
} FlashAEInput_ConstCharPtr;

typedef struct{
    bool input;
} FlashAEInput_Bool;

typedef struct{
    FlashAlgExpPara* input;
} FlashAEInput_FlashAlgExpPara;

typedef struct{
    FlashAlgStaData* input;
} FlashAEInput_FlashAlgStaData;

typedef struct{
    FlashAlgFacePos* input;
} FlashAEInput_FlashAlgFacePos;

typedef struct{
    PLine* input;
} FlashAEInput_PLine;

typedef struct{
    FlashAlgStrobeProfile* input;
} FlashAEInput_FlashAlgStrobeProfile;

typedef struct{
    FLASH_TUNING_PARA* input;
} FlashAEInput_FLASH_TUNING_PARA;

typedef struct{
    FlashAlgInputInfoStruct* input;
} FlashAEInput_FlashAlgInputInfoStruct;

/* Flash API Output */
typedef struct{
    short* output;
} FlashAEOutput_ShortPtr;

typedef struct{
    int output;
} FlashAEOutput_Int;

typedef struct{
    int* output;
} FlashAEOutput_IntPtr;

typedef struct{
    FlashAlgExpPara* output;
} FlashAEOutput_FlashAlgExpPara;

typedef struct{
    FlashAlgStrobeProfile* output;
} FlashAEOutput_FlashAlgStrobeProfile;

typedef struct{
    FLASH_ALGO_DEBUG_INFO_T* output;
} FlashAEOutput_FLASH_ALGO_DEBUG_INFO_T;

#endif //_FLASH_AE_GENERAL_TYPE_H_
