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
 * MediaTek Inc. (C) 2010. All rights reserved.
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

#ifndef _MTK_HARDWARE_MTKCAM_INCLUDE_MTKCAM_DEF_IMAGEFORMAT_H_
#define _MTK_HARDWARE_MTKCAM_INCLUDE_MTKCAM_DEF_IMAGEFORMAT_H_
//
#include <system/graphics.h>
#include "utils/gralloc/graphics_mtk_defs.h"

/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {

/******************************************************************************
 *
 * @enum
 * @brief Transformation definitions.
 *        ROT_90 is applied CLOCKWISE and AFTER TRANSFORM_FLIP_{H|V}.
 *
 ******************************************************************************/
enum
{
    /* do not rotate or flip image */
    eTransform_None                     = 0x00,

    /* flip source image horizontally (around the vertical axis) */
    eTransform_FLIP_H                   = 0x01,

    /* flip source image vertically (around the horizontal axis)*/
    eTransform_FLIP_V                   = 0x02,

    /* rotate source image 90 degrees clockwise */
    eTransform_ROT_90                   = 0x04,

    /* rotate source image 180 degrees */
    eTransform_ROT_180                  = 0x03,

    /* rotate source image 270 degrees clockwise */
    eTransform_ROT_270                  = 0x07,
};


/******************************************************************************
 *
 * @enum EImageFormat
 * @brief Image format Enumeration.
 *
 ******************************************************************************/
enum EImageFormat
{
    eImgFmt_IMPLEMENTATION_DEFINED  = HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED,


    eImgFmt_RAW16                   = HAL_PIXEL_FORMAT_RAW16,                   /*!< raw 16-bit 1 plane */
    eImgFmt_RAW_OPAQUE              = HAL_PIXEL_FORMAT_RAW_OPAQUE,              /*!< raw 1 plane */

    /*
     * This format is used to carry task-specific data which does not have a
     * standard image structure. The details of the format are left to the two
     * endpoints.
     *
     * Buffers of this format must have a height of 1, and width equal to their
     * size in bytes.
     */
    eImgFmt_BLOB                    = HAL_PIXEL_FORMAT_BLOB,

    eImgFmt_RGBA8888                = HAL_PIXEL_FORMAT_RGBA_8888,               /*!< RGBA (32-bit; LSB:R, MSB:A), 1 plane */
    eImgFmt_RGBX8888                = HAL_PIXEL_FORMAT_RGBX_8888,               /*!< RGBX (32-bit; LSB:R, MSB:X), 1 plane */
    eImgFmt_RGB888                  = HAL_PIXEL_FORMAT_RGB_888,                 /*!< RGB 888 (24-bit), 1 plane (RGB) */
    eImgFmt_RGB565                  = HAL_PIXEL_FORMAT_RGB_565,                 /*!< RGB 565 (16-bit), 1 plane */
    eImgFmt_BGRA8888                = HAL_PIXEL_FORMAT_BGRA_8888,               /*!< BGRA (32-bit; LSB:B, MSB:A), 1 plane */

    eImgFmt_YUY2                    = HAL_PIXEL_FORMAT_YCbCr_422_I,             /*!< 422 format, 1 plane (YUYV) */

    eImgFmt_NV16                    = HAL_PIXEL_FORMAT_YCbCr_422_SP,            /*!< 422 format, 2 plane (Y),(UV) */
    eImgFmt_NV21                    = HAL_PIXEL_FORMAT_YCrCb_420_SP,            /*!< 420 format, 2 plane (Y),(VU) */

    eImgFmt_YV12                    = HAL_PIXEL_FORMAT_YV12,                    /*!< 420 format, 3 plane (Y),(V),(U) */

    eImgFmt_Y8                      = HAL_PIXEL_FORMAT_Y8,                      /*!<  8-bit Y plane */
    eImgFmt_Y800                    = eImgFmt_Y8,                               /*!< deprecated; Replace it with eImgFmt_Y8 */
    eImgFmt_Y16                     = HAL_PIXEL_FORMAT_Y16,                     /*!< 16-bit Y plane */

    eImgFmt_CAMERA_OPAQUE           = HAL_PIXEL_FORMAT_CAMERA_OPAQUE,           /*!< Opaque format, RAW10 + Metadata */

    /**************************************************************************
     * 0x2000 - 0x2FFF
     *
     * This range is reserved for pixel formats that are specific to the HAL implementation.
     **************************************************************************/
    eImgFmt_UNKNOWN                 = 0x0000,                                   /*!< unknow */
    eImgFmt_VENDOR_DEFINED_START    = 0x2000,                                   /*!< vendor definition start */

    /* please add YUV format after eImgFmt_YUV_START */
    eImgFmt_YUV_START               = eImgFmt_VENDOR_DEFINED_START,
    eImgFmt_YVYU                    = eImgFmt_YUV_START,                        /*!< 422 format, 1 plane (YVYU) */
    eImgFmt_UYVY,                                                               /*!< 422 format, 1 plane (UYVY) */
    eImgFmt_VYUY,                                                               /*!< 422 format, 1 plane (VYUY) */

    eImgFmt_NV61,                                                               /*!< 422 format, 2 plane (Y),(VU) */
    eImgFmt_NV12,                                                               /*!< 420 format, 2 plane (Y),(UV) */
    eImgFmt_NV12_BLK,                                                           /*!< 420 format block mode, 2 plane (Y),(UV) */
    eImgFmt_NV21_BLK,                                                           /*!< 420 format block mode, 2 plane (Y),(VU) */

    eImgFmt_YV16,                                                               /*!< 422 format, 3 plane (Y),(V),(U) */
    eImgFmt_I420,                                                               /*!< 420 format, 3 plane (Y),(U),(V) */
    eImgFmt_I422,                                                               /*!< 422 format, 3 plane (Y),(U),(V) */


    eImgFmt_YUYV_Y210,                                                          /*!< 422 format, 16bit, 1 plane (YUYV) = Y210 */
    eImgFmt_YVYU_Y210,                                                          /*!< 422 format, 16bit, 1 plane (YVYU)            */
    eImgFmt_UYVY_Y210,                                                          /*!< 422 format, 16bit, 1 plane (UYVY)            */
    eImgFmt_VYUY_Y210,                                                          /*!< 422 format, 16bit, 1 plane (VYUY)            */

    eImgFmt_YUV_P210,                                                           /*!< 422 format, 16bit, 2 plane (Y),(UV) = P210 */
    eImgFmt_YVU_P210,                                                           /*!< 422 format, 16bit, 2 plane (Y),(VU)          */

    eImgFmt_YUV_P210_3PLANE,                                                    /*!< 422 format, 16bit, 3 plane (Y),(U),(V)       */

    eImgFmt_YUV_P010,                                                           /*!< 420 format, 16bit, 2 plane (Y),(UV) = P010 */
    eImgFmt_YVU_P010,                                                           /*!< 420 format, 16bit, 2 plane (Y),(VU)          */

    eImgFmt_YUV_P010_3PLANE,                                                    /*!< 420 format, 16bit, 3 plane (Y),(U),(V)       */

    eImgFmt_MTK_YUYV_Y210,                                                      /*!< 422 format, 10bit, 1 plane (YUYV) = Y210 */
    eImgFmt_MTK_YVYU_Y210,                                                      /*!< 422 format, 10bit, 1 plane (YVYU)            */
    eImgFmt_MTK_UYVY_Y210,                                                      /*!< 422 format, 10bit, 1 plane (UYVY)            */
    eImgFmt_MTK_VYUY_Y210,                                                      /*!< 422 format, 10bit, 1 plane (VYUY)            */

    eImgFmt_MTK_YUV_P210,                                                       /*!< 422 format, 10bit, 2 plane (Y),(UV) = P210 */
    eImgFmt_MTK_YVU_P210,                                                       /*!< 422 format, 10bit, 2 plane (Y),(VU)          */

    eImgFmt_MTK_YUV_P210_3PLANE,                                                /*!< 422 format, 10bit, 3 plane (Y),(U),(V)       */

    eImgFmt_MTK_YUV_P010,                                                       /*!< 420 format, 10bit, 2 plane (Y),(UV) = P010 */
    eImgFmt_MTK_YVU_P010,                                                       /*!< 420 format, 10bit, 2 plane (Y),(VU)          */

    eImgFmt_MTK_YUV_P010_3PLANE,                                                /*!< 420 format, 10bit, 3 plane (Y),(U),(V)       */



    /* please add RGB format after eImgFmt_RGB_START */
    eImgFmt_RGB_START               = 0x2100,
    eImgFmt_ARGB8888                = eImgFmt_RGB_START,                        /*!< ARGB (32-bit; LSB:A, MSB:B), 1 plane */
    eImgFmt_ARGB888                 = eImgFmt_ARGB8888,                         /*!< deprecated; Replace it with eImgFmt_ARGB8888 */
    eImgFmt_RGB48,                                                              /*!< RGB 48(16x3, 48-bit; LSB:R, MSB:B), 1 plane */

    /* please add RAW format after eImgFmt_RAW_START */
    eImgFmt_RAW_START               = 0x2200,
    eImgFmt_BAYER8                  = eImgFmt_RAW_START,                        /*!< Bayer format, 8-bit */
    eImgFmt_BAYER10,                                                            /*!< Bayer format, 10-bit */
    eImgFmt_BAYER12,                                                            /*!< Bayer format, 12-bit */
    eImgFmt_BAYER14,                                                            /*!< Bayer format, 14-bit */

    eImgFmt_FG_BAYER8,                                                          /*!< Full-G (8-bit) */
    eImgFmt_FG_BAYER10,                                                         /*!< Full-G (10-bit) */
    eImgFmt_FG_BAYER12,                                                         /*!< Full-G (12-bit) */
    eImgFmt_FG_BAYER14,                                                         /*!< Full-G (14-bit) */

    /*IMGO/RRZO UF format*/
    eImgFmt_UFO_START,
    eImgFmt_UFO_BAYER8              = eImgFmt_UFO_START,                        /*!< UFO (8-bit) */
    eImgFmt_UFO_BAYER10,                                                        /*!< UFO (10-bit) */
    eImgFmt_UFO_BAYER12,                                                        /*!< UFO (12-bit) */
    eImgFmt_UFO_BAYER14,                                                        /*!< UFO (14-bit) */

    eImgFmt_UFEO_BAYER8,                                                        /*only for 6799 rrzo UF format*/
    eImgFmt_UFO_FG_BAYER8           = eImgFmt_UFEO_BAYER8,                      /*!< UFO Full-G(8-bit) */
    eImgFmt_UFEO_BAYER10,                                                        /*only for 6799 rrzo UF format*/
    eImgFmt_UFO_FG_BAYER10          = eImgFmt_UFEO_BAYER10,                     /*!< UFO Full-G(10-bit) */
    eImgFmt_UFEO_BAYER12,                                                        /*only for 6799 rrzo UF format*/
    eImgFmt_UFO_FG_BAYER12          = eImgFmt_UFEO_BAYER12,                     /*!< UFO Full-G(12-bit) */
    eImgFmt_UFEO_BAYER14,                                                        /*only for 6799 rrzo UF format*/
    eImgFmt_UFO_FG_BAYER14          = eImgFmt_UFEO_BAYER14,                     /*!< UFO Full-G(14-bit) */
    eImgFmt_UFO_END,
    eImgFmt_UFO_FG                  = eImgFmt_UFO_END,                          /*!< UFO (Full-G) */

    eImgFmt_BAYER10_MIPI,                                                       /*!< Bayer format, 10-bit (MIPI) */

    eImgFmt_BAYER8_UNPAK,                                                       /*!< Bayer format,unpaked 16-bit */
    eImgFmt_BAYER10_UNPAK,                                                      /*!< Bayer format,unpaked 16-bit */
    eImgFmt_BAYER12_UNPAK,                                                      /*!< Bayer format,unpaked 16-bit */
    eImgFmt_BAYER14_UNPAK,                                                      /*!< Bayer format,unpaked 16-bit */
    eImgFmt_BAYER15_UNPAK,                                                      /*!< Bayer format,unpaked 16-bit */

    eImgFmt_WARP_1PLANE,                                                        /*!< Warp format, 32-bit, 1 plane (X) */
    eImgFmt_WARP_2PLANE,                                                        /*!< Warp format, 32-bit, 2 plane (X), (Y) */
    eImgFmt_WARP_3PLANE,                                                        /*!< Warp format, 32-bit, 3 plane (X), (Y), (Z) */

    eImgFmt_BAYER16_APPLY_LSC,                                                  /*!< Bayer format, 16-bit, 0x221A */

    /* please add BLOB format after eImgFmt_BLOB_START */
    eImgFmt_BLOB_START              = 0x2300,
    eImgFmt_JPEG                    = eImgFmt_BLOB_START,                       /*!< JPEG format */
    eImgFmt_JPG_I420,                                                           /*!< JPEG 420 format, 3 plane (Y),(U),(V) */
    eImgFmt_JPG_I422,                                                           /*!< JPEG 422 format, 3 plane (Y),(U),(V) */

    /**************************************************************************
     * This section is used for non-image-format buffer.
     **************************************************************************/
    /* please add STATIC format after eImgFmt_STA_START */
    eImgFmt_STA_START               = 0x2400,
    eImgFmt_STA_BYTE                = eImgFmt_STA_START,                        /*!< statistic format, 8-bit */
    eImgFmt_STA_2BYTE,                                                          /*!< statistic format, 16-bit */
    eImgFmt_STA_4BYTE,                                                          /*!< statistic format, 32-bit */
    eImgFmt_STA_10BIT,                                                          /*!< statistic format, 10-bit */
    eImgFmt_STA_12BIT,                                                          /*!< statistic format, 12-bit */

};


/******************************************************************************
 *
 ******************************************************************************/
};  //namespace NSCam
#endif  //_MTK_HARDWARE_MTKCAM_INCLUDE_MTKCAM_DEF_IMAGEFORMAT_H_

