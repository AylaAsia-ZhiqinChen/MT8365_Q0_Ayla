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

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/

#include "camera_custom_eis.h"


/* EIS Configurations
 *
 *  => DO NOT modify
 */
#define EIS_NONE_FACTOR                 (100)
#define EIS_FACTOR                      (120)
#define EIS_FHD_FACTOR                  (125)
#define EIS_FHD_DUAL_FACTOR             (120)
#define EIS_4K_FACTOR                   (106)
#define EIS_START_FRAME                 (18)
#define EIS_4K_RECORD_FPS               (30)

#define FWDEIS_FRAMES_FHD               (25)
#define FWDEIS_FRAMES_4K2K              (25)

#define EIS_MV_WIDTH_WIDE               (48)
#define EIS_MV_HEIGHT_WIDE              (27)
#define EIS_MV_WIDTH                    (48)
#define EIS_MV_HEIGHT                   (36)

/* EIS Limitations
 *
 *  => Modify here if there is HW limitation
 */
#define SUPPORT_EIS_MODE_GYRO           (1)
#define SUPPORT_EIS_MODE_IMAGE          (0)
#define SUPPORT_EIS_GYRO_VHDR           (0)
#define SUPPORT_EIS_VHDR_TUNING         (0)


/* EIS Policies
 *
 *  => Not recommend to modify. You should carefully use it
 */
#define EIS_IS_FORCE_EIS12              (1)
#define ENABLE_EIS_EIS22                (0)
#define ENABLE_EIS_EIS30                (1)

#define ENABLE_EIS_DEJELLO              (1)
#define ENABLE_EIS_FIXED_FRAME_RATE     (1)

#define ENABLE_FWDEIS_QUEUE             (1)
#define ENABLE_FWDEIS_QUEUE_4K2K        (1)

#define ENABLE_EIS_FOV_WARP_COMBINE_FHD (1)
#define ENABLE_EIS_FOV_WARP_COMBINE_4K  (1)


static inline MBOOL isLimitEIS12()
{
#if defined(MTK_EIS_IS_GMO_PROJ)
    return true;
#endif
    return EIS_IS_FORCE_EIS12;
}


void EISCustom::getEISData(EIS_Customize_Para_t *a_pDataOut)
{
    a_pDataOut->sensitivity   = CUSTOMER_EIS_SENSI_LEVEL_ADVTUNE;
    a_pDataOut->filter_small_motion = 0;    // 0 or 1
    a_pDataOut->adv_shake_ext = 1;          // 0 or 1
    a_pDataOut->stabilization_strength = 0.9;   // 0.5~0.95
    a_pDataOut->new_tru_th = 25;            // 0~100
    a_pDataOut->vot_th = 4;                 // 1~16
    a_pDataOut->votb_enlarge_size = 0;      // 0~1280
    a_pDataOut->min_s_th = 40;              // 10~100
    a_pDataOut->vec_th = 0;                 // 0~11   should be even
    a_pDataOut->spr_offset = 0;             // 0 ~ MarginX/2
    a_pDataOut->spr_gain1 = 0;              // 0~127
    a_pDataOut->spr_gain2 = 0;              // 0~127
    a_pDataOut->gmv_pan_array[0] = 0;       // 0~5
    a_pDataOut->gmv_pan_array[1] = 0;       // 0~5
    a_pDataOut->gmv_pan_array[2] = 0;       // 0~5
    a_pDataOut->gmv_pan_array[3] = 1;       // 0~5

    a_pDataOut->gmv_sm_array[0] = 0;    //0~5
    a_pDataOut->gmv_sm_array[1] = 0;    //0~5
    a_pDataOut->gmv_sm_array[2] = 0;    //0~5
    a_pDataOut->gmv_sm_array[3] = 1;    //0~5

    a_pDataOut->cmv_pan_array[0] = 0;   //0~5
    a_pDataOut->cmv_pan_array[1] = 0;   //0~5
    a_pDataOut->cmv_pan_array[2] = 0;   //0~5
    a_pDataOut->cmv_pan_array[3] = 1;   //0~5

    a_pDataOut->cmv_sm_array[0] = 0;    //0~5
    a_pDataOut->cmv_sm_array[1] = 1;    //0~5
    a_pDataOut->cmv_sm_array[2] = 2;    //0~5
    a_pDataOut->cmv_sm_array[3] = 4;    //0~5

    a_pDataOut->vot_his_method = ABSOLUTE_HIST; //ABSOLUTE_HIST or SMOOTH_HIST
    a_pDataOut->smooth_his_step = 3; // 2~6
    a_pDataOut->eis_debug = 0;
}

void EISCustom::getEISPlusData(EIS_PLUS_Customize_Para_t *a_pDataOut, MUINT32 config)
{
    a_pDataOut->warping_mode = EIS_WARP_METHOD_6_4_ADAPTIVE;
    a_pDataOut->search_range_x = 64;        // 32~64
    a_pDataOut->search_range_y = 64;        // 32~64
    a_pDataOut->crop_ratio     = getEISFactor(config) - EIS_NONE_FACTOR; // 10~40
    a_pDataOut->gyro_still_time_th = 0;
    a_pDataOut->gyro_max_time_th   = 0;
    a_pDataOut->gyro_similar_th    = 0;
    a_pDataOut->stabilization_strength = 0.9;   // 0.5~0.95
}

void EISCustom::getEIS30Data(EIS30_Customize_Tuning_Para_t *a_pDataOut)
{
    a_pDataOut->stabilization_strength = 0.9;
    a_pDataOut->stabilization_level    = 4;
    a_pDataOut->gyro_still_mv_th       = 1;
    a_pDataOut->gyro_still_mv_diff_th  = 1;
}

MBOOL EISCustom::isForcedEIS12()
{
    return isLimitEIS12() ||
           ( !isEnabledEIS30() && !isEnabledEIS22() );
}

MBOOL EISCustom::isEnabledEIS22()
{
    return ENABLE_EIS_EIS22 && !isLimitEIS12();
}

MBOOL EISCustom::isEnabledEIS30()
{
    return ENABLE_EIS_EIS30 && ( SUPPORT_EIS_MODE_GYRO || SUPPORT_EIS_MODE_IMAGE ) &&
           !isLimitEIS12();
}

MBOOL EISCustom::isEnabledFixedFPS()
{
    unsigned int variableFPS = ::property_get_int32(EIS_VARIABLE_FPS, 0);
    return variableFPS ? false : ENABLE_EIS_FIXED_FRAME_RATE;
}
MBOOL EISCustom::isEnabledGyroMode()
{
    return SUPPORT_EIS_MODE_GYRO && isEnabledEIS30();
}

MBOOL EISCustom::isEnabledImageMode()
{
    return SUPPORT_EIS_MODE_IMAGE && isEnabledEIS30();
}

MBOOL EISCustom::isEnabledForwardMode(MUINT32 cfg)
{
#if defined(MTK_EIS_IS_ENG_LOAD)
    return false;
#endif
    MBOOL FwdMode = ENABLE_FWDEIS_QUEUE && isEnabledEIS30();
    if (cfg == VIDEO_CFG_4K2K)
    {
        FwdMode = FwdMode && ENABLE_FWDEIS_QUEUE_4K2K;
    }
    return FwdMode;
}

MBOOL EISCustom::isEnabledFOVWarpCombine(MUINT32 cfg)
{
    return (cfg == VIDEO_CFG_4K2K) ? ENABLE_EIS_FOV_WARP_COMBINE_4K : ENABLE_EIS_FOV_WARP_COMBINE_FHD;
}

double EISCustom::getEISRatio(MUINT32 cfg, MUINT32 /*mask*/)
{
    return ( 100.0 / getEISFactor(cfg) );
}

MUINT32 EISCustom::getEIS12Factor()
{
    return EIS_FACTOR;
}

MUINT32 EISCustom::getEISFactor(MUINT32 cfg, MUINT32 /*mask*/)
{
    MUINT32 factor = 0;
#if defined(MTK_HAVE_DUAL_ZOOM_SUPPORT)
    factor = (cfg == VIDEO_CFG_FHD) ? ::property_get_int32(VAR_EIS_CUSTOM_FACTOR, EIS_FHD_DUAL_FACTOR) :
                                      ::property_get_int32(VAR_EIS_CUSTOM_FACTOR, EIS_4K_FACTOR);
#else
    factor = (cfg == VIDEO_CFG_FHD) ? ::property_get_int32(VAR_EIS_CUSTOM_FACTOR, EIS_FHD_FACTOR) :
                                      ::property_get_int32(VAR_EIS_CUSTOM_FACTOR, EIS_4K_FACTOR);
#endif
    return factor;
}

MUINT32 EISCustom::get4K2KRecordFPS()
{
    return EIS_4K_RECORD_FPS;
}

MUINT32 EISCustom::getForwardStartFrame()
{
    return isEnabledForwardMode() ? EIS_START_FRAME : 1;
}

MUINT32 EISCustom::getForwardFrames(MUINT32 cfg)
{
    MBOOL FwdFrames = 0;
    if( isEnabledForwardMode(cfg) )
    {
        FwdFrames = (cfg == VIDEO_CFG_4K2K) ? FWDEIS_FRAMES_4K2K : FWDEIS_FRAMES_FHD;
    }
    return FwdFrames;
}

MUINT32 EISCustom::getEISMode(MUINT32 mask)
{
    MUINT32 eisMode = EIS_MODE_OFF;
    if( isForcedEIS12() ||
        ( generateEIS30FusionMode(mask, eisMode) == false &&
          generateEIS30GyroMode(mask, eisMode) == false &&
          generateEIS30ImageMode(mask, eisMode) == false &&
          generateEIS22Mode(mask, eisMode) == false ) )
    {
        EIS_MODE_ENABLE_EIS_12(eisMode);
    }
    appendEISMode(mask, eisMode);
    return eisMode;
}

void EISCustom::getMVNumber(MINT32 width, MINT32 height, MINT32 *mvWidth, MINT32 *mvHeight)
{
    float ratio = (width != 0) ? (float)height/width : 0;
    MINT32 mv_width = ::property_get_int32(VAR_EIS_MV_WIDTH, 0);
    MINT32 mv_height = ::property_get_int32(VAR_EIS_MV_HEIGHT, 0);

    if (mv_width != 0 && mv_height != 0)
    {
        *mvWidth = mv_width;
        *mvHeight = mv_height;
        return;
    }

    if (mvWidth != 0 && mvHeight != 0)
    {
        if(ratio < (((3.0/4)+(9.0/16))/2))//16:9
        {
            *mvWidth = EIS_MV_WIDTH_WIDE;
            *mvHeight = EIS_MV_HEIGHT_WIDE;
        }
        else //4:3 and others
        {
            *mvWidth = EIS_MV_WIDTH;
            *mvHeight = EIS_MV_HEIGHT;
        }
    }
}

void EISCustom::appendEISMode(MUINT32 /*mask*/, MUINT32 &eisMode)
{
    if( EIS_MODE_IS_EIS_25_ENABLED(eisMode) )
    {
        if( ENABLE_EIS_DEJELLO && EIS_MODE_IS_EIS_GYRO_ENABLED(eisMode) )
        {
            EIS_MODE_ENABLE_EIS_DEJELLO(eisMode);
        }
    }
}

MBOOL EISCustom::generateEIS22Mode(MUINT32 mask, MUINT32 &eisMode)
{
    MBOOL ret = false;
    // Get EIS Mask
    MBOOL is4K2K = (mask & USAGE_MASK_4K2K) ? true : false;
    MBOOL isVHDR = (mask & USAGE_MASK_VHDR) ? true : false;
    if( isEnabledEIS22() && !is4K2K && !isVHDR )
    {
        EIS_MODE_ENABLE_EIS_22(eisMode);
        ret = true;
    }
    return ret;
}

MBOOL EISCustom::generateEIS30FusionMode(MUINT32 mask, MUINT32 &eisMode)
{
    unsigned int gyroOnly = ::property_get_int32(EIS_FORCE_GYRO_ONLY, 0);
    unsigned int imageOnly = ::property_get_int32(EIS_FORCE_IMAGE_ONLY, 0);
    if (gyroOnly || imageOnly)
    {
        return false;
    }
    MBOOL ret = false;
    // Get EIS Mask
    MBOOL isDualZoom  = (mask & USAGE_MASK_DUAL_ZOOM) ? true : false;
    MBOOL is4K2K      = (mask & USAGE_MASK_4K2K) ? true : false;
    MBOOL isMulti     = (mask & USAGE_MASK_MULTIUSER) ? true : false;
    MBOOL isVHDR      = (mask & USAGE_MASK_VHDR) ? true : false;
    MBOOL vHDRFusionCompatible = !isVHDR || (SUPPORT_EIS_GYRO_VHDR && SUPPORT_EIS_VHDR_TUNING);
    VIDEO_CFG vdo_cfg = is4K2K ? VIDEO_CFG_4K2K : VIDEO_CFG_FHD;
    if( isEnabledEIS30() && isEnabledImageMode() && isEnabledGyroMode() &&
        !is4K2K && !isDualZoom && !isMulti && vHDRFusionCompatible )
    {
        EIS_MODE_ENABLE_EIS_30(eisMode);
        EIS_MODE_ENABLE_EIS_IMAGE(eisMode);
        EIS_MODE_ENABLE_EIS_GYRO(eisMode);
        if( isEnabledForwardMode(vdo_cfg) )
        {
            EIS_MODE_ENABLE_EIS_QUEUE(eisMode);
        }
        ret = true;
    }
    return ret;
}

MBOOL EISCustom::generateEIS30GyroMode(MUINT32 mask, MUINT32 &eisMode)
{
    unsigned int imageOnly = ::property_get_int32(EIS_FORCE_IMAGE_ONLY, 0);
    if (imageOnly)
    {
        return false;
    }
    MBOOL ret = false;
    // Get EIS Mask
    MBOOL is4K2K      = (mask & USAGE_MASK_4K2K) ? true : false;
    MBOOL isVHDR      = (mask & USAGE_MASK_VHDR) ? true : false;
    MBOOL vHDRGyroCompatible = !isVHDR || (SUPPORT_EIS_GYRO_VHDR && SUPPORT_EIS_VHDR_TUNING);
    VIDEO_CFG vdo_cfg = (mask & USAGE_MASK_4K2K) ? VIDEO_CFG_4K2K : VIDEO_CFG_FHD;
    if( isEnabledEIS30() && isEnabledGyroMode() &&
        !is4K2K && vHDRGyroCompatible )
    {
        EIS_MODE_ENABLE_EIS_30(eisMode);
        EIS_MODE_ENABLE_EIS_GYRO(eisMode);
        if( isEnabledForwardMode(vdo_cfg) )
        {
            EIS_MODE_ENABLE_EIS_QUEUE(eisMode);
        }
        ret = true;
    }
    return ret;
}

MBOOL EISCustom::generateEIS30ImageMode(MUINT32 mask, MUINT32 &eisMode)
{
    unsigned int gyroOnly = ::property_get_int32(EIS_FORCE_GYRO_ONLY, 0);
    if (gyroOnly)
    {
        return false;
    }
    MBOOL ret = false;
    // Get EIS Mask
    MBOOL is4K2K      = (mask & USAGE_MASK_4K2K) ? true : false;
    MBOOL isDualZoom  = (mask & USAGE_MASK_DUAL_ZOOM) ? true : false;
    MBOOL isMulti     = (mask & USAGE_MASK_MULTIUSER) ? true : false;
    MBOOL isVHDR      = (mask & USAGE_MASK_VHDR) ? true : false;
    MBOOL vHDRImageCompatible = !isVHDR || SUPPORT_EIS_VHDR_TUNING;
    VIDEO_CFG vdo_cfg = is4K2K ? VIDEO_CFG_4K2K : VIDEO_CFG_FHD;
    if( isEnabledEIS30() && isEnabledImageMode() &&
        !is4K2K && !isDualZoom && !isMulti && vHDRImageCompatible )
    {
        EIS_MODE_ENABLE_EIS_30(eisMode);
        EIS_MODE_ENABLE_EIS_IMAGE(eisMode);
        if( isEnabledForwardMode(vdo_cfg) )
        {
            EIS_MODE_ENABLE_EIS_QUEUE(eisMode);
        }
        ret = true;
    }
    return ret;
}

