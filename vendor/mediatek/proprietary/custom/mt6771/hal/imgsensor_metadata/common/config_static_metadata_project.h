/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2017. All rights reserved.
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


STATIC_METADATA2_BEGIN(PLATFORM, PROJECT, DEFAULT)
//------------------------------------------------------------------------------
//  android.request
//------------------------------------------------------------------------------
    //==========================================================================
    //==========================================================================
//------------------------------------------------------------------------------
//  android.scaler
//------------------------------------------------------------------------------
    //==========================================================================
    CONFIG_METADATA_BEGIN(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_WITH_DURATIONS_CANDIDATES)

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64) //13mp 4:3
        CONFIG_ENTRY_VALUE(4096, MINT64)        // width
        CONFIG_ENTRY_VALUE(3072, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(66666666, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64) //13mp 16:9
        CONFIG_ENTRY_VALUE(4096, MINT64)        // width
        CONFIG_ENTRY_VALUE(2304, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(66666666, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64) //16mp 4:3
        CONFIG_ENTRY_VALUE(4032, MINT64)
        CONFIG_ENTRY_VALUE(3024, MINT64)
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)
        CONFIG_ENTRY_VALUE(66666666, MINT64)
        CONFIG_ENTRY_VALUE(33333333, MINT64)

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64) //8mp 4:3
        CONFIG_ENTRY_VALUE(3264, MINT64)        // width
        CONFIG_ENTRY_VALUE(2448, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(50000000, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64) //8mp 16:9
        CONFIG_ENTRY_VALUE(3840, MINT64)        // width
        CONFIG_ENTRY_VALUE(2160, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(50000000, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(2560, MINT64)        // width
        CONFIG_ENTRY_VALUE(1920, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(1920, MINT64)        // width
        CONFIG_ENTRY_VALUE(1088, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(1440, MINT64)        // width
        CONFIG_ENTRY_VALUE(1088, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(1280, MINT64)        // width
        CONFIG_ENTRY_VALUE(960, MINT64)         // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(1280, MINT64)        // width
        CONFIG_ENTRY_VALUE(720, MINT64)         // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(640, MINT64)         // width
        CONFIG_ENTRY_VALUE(480, MINT64)         // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(320, MINT64)         // width
        CONFIG_ENTRY_VALUE(240, MINT64)         // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64) //13mp 4:3
        CONFIG_ENTRY_VALUE(4096, MINT64)        // width
        CONFIG_ENTRY_VALUE(3072, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(100000000, MINT64)   // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64) //13mp 16:9
        CONFIG_ENTRY_VALUE(4096, MINT64)        // width
        CONFIG_ENTRY_VALUE(2304, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(50000000, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64) //13mp 4:3
        CONFIG_ENTRY_VALUE(4032, MINT64)        // width
        CONFIG_ENTRY_VALUE(3024, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(100000000, MINT64)   // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64) //8mp 4:3
        CONFIG_ENTRY_VALUE(3264, MINT64)        // width
        CONFIG_ENTRY_VALUE(2448, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(50000000, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64) //8mp 4:3
        CONFIG_ENTRY_VALUE(3840, MINT64)        // width
        CONFIG_ENTRY_VALUE(2160, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(50000000, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(2560, MINT64)        // width
        CONFIG_ENTRY_VALUE(1920, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(2560, MINT64)        // width
        CONFIG_ENTRY_VALUE(1440, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1920, MINT64)        // width
        CONFIG_ENTRY_VALUE(1440, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1920, MINT64)        // width
        CONFIG_ENTRY_VALUE(1088, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1920, MINT64)        // width
        CONFIG_ENTRY_VALUE(1080, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1440, MINT64)        // width
        CONFIG_ENTRY_VALUE(1088, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1440, MINT64)        // width
        CONFIG_ENTRY_VALUE(1080, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1440, MINT64)        // width
        CONFIG_ENTRY_VALUE(1080, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1280, MINT64)        // width
        CONFIG_ENTRY_VALUE( 960, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1280, MINT64)        // width
        CONFIG_ENTRY_VALUE( 720, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE( 720, MINT64)        // width
        CONFIG_ENTRY_VALUE( 480, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE( 640, MINT64)        // width
        CONFIG_ENTRY_VALUE( 480, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE( 352, MINT64)        // width
        CONFIG_ENTRY_VALUE( 288, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE( 320, MINT64)        // width
        CONFIG_ENTRY_VALUE( 240, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE( 192, MINT64)        // width
        CONFIG_ENTRY_VALUE( 144, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE( 192, MINT64)        // width
        CONFIG_ENTRY_VALUE( 108, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE( 176, MINT64)        // width
        CONFIG_ENTRY_VALUE( 144, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE( 160, MINT64)        // width
        CONFIG_ENTRY_VALUE(  96, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)           // stall duration

    CONFIG_METADATA_END()

//------------------------------------------------------------------------------
//  android.reprocess
//------------------------------------------------------------------------------
    //==========================================================================

//------------------------------------------------------------------------------
//  android.jpeg
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
STATIC_METADATA_END()

