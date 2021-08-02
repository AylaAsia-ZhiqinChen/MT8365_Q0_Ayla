/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
**
** Copyright 2010, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/


#define LOG_NDEBUG 0
#define LOG_TAG "ImgsensorProfiles"

#include <stdlib.h>
#include <utils/Log.h>
#include <utils/Vector.h>
#include <cutils/properties.h>
#include <expat.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include "../sensor_drv.h"

using namespace android;
#include "ImgsensorProfiles.h"

#define LITERAL_TO_STRING_INTERNAL(x)    #x
#define LITERAL_TO_STRING(x) LITERAL_TO_STRING_INTERNAL(x)

#define CHECK(condition)                                \
    LOG_ALWAYS_FATAL_IF(                                \
            !(condition),                               \
            "%s",                                       \
            __FILE__ ":" LITERAL_TO_STRING(__LINE__)    \
            " CHECK(" #condition ") failed.")

static void relayout_SENSORDRV_INFO(PRELAYOUT_SENSORDRV_INFO_STRUCT pRelayout, PSENSORDRV_INFO_STRUCT pInfo)
{
    // Part 1 : Basic information
    pRelayout->SensorPreviewResolutionX = pInfo->SensorPreviewResolutionX;
    pRelayout->SensorPreviewResolutionY = pInfo->SensorPreviewResolutionY;
    pRelayout->SensorPreviewResolutionX_ZSD = pInfo->SensorPreviewResolutionX_ZSD;
    pRelayout->SensorPreviewResolutionY_ZSD = pInfo->SensorPreviewResolutionY_ZSD;
    pRelayout->SensorFullResolutionX = pInfo->SensorFullResolutionX;
    pRelayout->SensorFullResolutionY = pInfo->SensorFullResolutionY;
    pRelayout->SensorClockFreq = pInfo->SensorClockFreq;
    pRelayout->SensorCameraPreviewFrameRate = pInfo->SensorCameraPreviewFrameRate;
    pRelayout->SensorVideoFrameRate = pInfo->SensorVideoFrameRate;
    pRelayout->SensorStillCaptureFrameRate = pInfo->SensorStillCaptureFrameRate;
    pRelayout->SensorWebCamCaptureFrameRate = pInfo->SensorWebCamCaptureFrameRate;
    pRelayout->SensorClockPolarity = pInfo->SensorClockPolarity;
    pRelayout->SensorClockFallingPolarity = pInfo->SensorClockFallingPolarity;
    pRelayout->SensorClockRisingCount = pInfo->SensorClockRisingCount;
    pRelayout->SensorClockFallingCount = pInfo->SensorClockFallingCount;
    pRelayout->SensorClockDividCount = pInfo->SensorClockDividCount;
    pRelayout->SensorPixelClockCount = pInfo->SensorPixelClockCount;
    pRelayout->SensorDataLatchCount = pInfo->SensorDataLatchCount;
    pRelayout->SensorHsyncPolarity = pInfo->SensorHsyncPolarity;
    pRelayout->SensorVsyncPolarity = pInfo->SensorVsyncPolarity;
    pRelayout->SensorInterruptDelayLines = pInfo->SensorInterruptDelayLines;
    pRelayout->SensorResetActiveHigh = pInfo->SensorResetActiveHigh;
    pRelayout->SensorResetDelayCount = pInfo->SensorResetDelayCount;
    pRelayout->SensroInterfaceType = (MUINT32)pInfo->SensroInterfaceType;
    pRelayout->SensorOutputDataFormat = (MUINT32)pInfo->SensorOutputDataFormat;
    pRelayout->SensorMIPILaneNumber = (MUINT32)pInfo->SensorMIPILaneNumber;
    pRelayout->CaptureDelayFrame = pInfo->CaptureDelayFrame;
    pRelayout->PreviewDelayFrame = pInfo->PreviewDelayFrame;
    pRelayout->VideoDelayFrame = pInfo->VideoDelayFrame;
    pRelayout->HighSpeedVideoDelayFrame = pInfo->HighSpeedVideoDelayFrame;
    pRelayout->SlimVideoDelayFrame = pInfo->SlimVideoDelayFrame;
    pRelayout->YUVAwbDelayFrame = pInfo->YUVAwbDelayFrame;
    pRelayout->YUVEffectDelayFrame = pInfo->YUVEffectDelayFrame;
    pRelayout->Custom1DelayFrame = pInfo->Custom1DelayFrame;
    pRelayout->Custom2DelayFrame = pInfo->Custom2DelayFrame;
    pRelayout->Custom3DelayFrame = pInfo->Custom3DelayFrame;
    pRelayout->Custom4DelayFrame = pInfo->Custom4DelayFrame;
    pRelayout->Custom5DelayFrame = pInfo->Custom5DelayFrame;
    pRelayout->SensorGrabStartX_PRV = pInfo->SensorGrabStartX_PRV;
    pRelayout->SensorGrabStartY_PRV = pInfo->SensorGrabStartY_PRV;
    pRelayout->SensorGrabStartX_CAP = pInfo->SensorGrabStartX_CAP;
    pRelayout->SensorGrabStartY_CAP = pInfo->SensorGrabStartY_CAP;
    pRelayout->SensorGrabStartX_VD = pInfo->SensorGrabStartX_VD;
    pRelayout->SensorGrabStartY_VD = pInfo->SensorGrabStartY_VD;
    pRelayout->SensorGrabStartX_VD1 = pInfo->SensorGrabStartX_VD1;
    pRelayout->SensorGrabStartY_VD1 = pInfo->SensorGrabStartY_VD1;
    pRelayout->SensorGrabStartX_VD2 = pInfo->SensorGrabStartX_VD2;
    pRelayout->SensorGrabStartY_VD2 = pInfo->SensorGrabStartY_VD2;
    pRelayout->SensorGrabStartX_CST1 = pInfo->SensorGrabStartX_CST1;
    pRelayout->SensorGrabStartY_CST1 = pInfo->SensorGrabStartY_CST1;
    pRelayout->SensorGrabStartX_CST2 = pInfo->SensorGrabStartX_CST2;
    pRelayout->SensorGrabStartY_CST2 = pInfo->SensorGrabStartY_CST2;
    pRelayout->SensorGrabStartX_CST3 = pInfo->SensorGrabStartX_CST3;
    pRelayout->SensorGrabStartY_CST3 = pInfo->SensorGrabStartY_CST3;
    pRelayout->SensorGrabStartX_CST4 = pInfo->SensorGrabStartX_CST4;
    pRelayout->SensorGrabStartY_CST4 = pInfo->SensorGrabStartY_CST4;
    pRelayout->SensorGrabStartX_CST5 = pInfo->SensorGrabStartX_CST5;
    pRelayout->SensorGrabStartY_CST5 = pInfo->SensorGrabStartY_CST5;
    pRelayout->SensorDrivingCurrent = pInfo->SensorDrivingCurrent;
    pRelayout->AEShutDelayFrame = pInfo->AEShutDelayFrame;
    pRelayout->AESensorGainDelayFrame = pInfo->AESensorGainDelayFrame;
    pRelayout->AEISPGainDelayFrame = pInfo->AEISPGainDelayFrame;
    pRelayout->MIPIDataLowPwr2HighSpeedTermDelayCount = pInfo->MIPIDataLowPwr2HighSpeedTermDelayCount;
    pRelayout->MIPIDataLowPwr2HSSettleDelayM0 = pInfo->MIPIDataLowPwr2HSSettleDelayM0;
    pRelayout->MIPIDataLowPwr2HSSettleDelayM1 = pInfo->MIPIDataLowPwr2HSSettleDelayM1;
    pRelayout->MIPIDataLowPwr2HSSettleDelayM2 = pInfo->MIPIDataLowPwr2HSSettleDelayM2;
    pRelayout->MIPIDataLowPwr2HSSettleDelayM3 = pInfo->MIPIDataLowPwr2HSSettleDelayM3;
    pRelayout->MIPIDataLowPwr2HSSettleDelayM4 = pInfo->MIPIDataLowPwr2HSSettleDelayM4;
    pRelayout->MIPIDataLowPwr2HighSpeedSettleDelayCount = pInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount;
    pRelayout->MIPICLKLowPwr2HighSpeedTermDelayCount = pInfo->MIPICLKLowPwr2HighSpeedTermDelayCount;
    pRelayout->SensorWidthSampling = pInfo->SensorWidthSampling;
    pRelayout->SensorHightSampling = pInfo->SensorHightSampling;
    pRelayout->SensorPacketECCOrder = pInfo->SensorPacketECCOrder;
    pRelayout->iHDR_First_IS_LE = pInfo->iHDR_First_IS_LE;
    pRelayout->iHDRSupport = pInfo->iHDRSupport;
    pRelayout->PDAF_Support = pInfo->PDAF_Support;
    pRelayout->SensorModeNum = pInfo->SensorModeNum;
    pRelayout->MIPIsensorType = (MUINT32)pInfo->MIPIsensorType;
    pRelayout->SettleDelayMode = (MUINT32)pInfo->SettleDelayMode;
    pRelayout->IMGSENSOR_DPCM_TYPE_PRE = pInfo->IMGSENSOR_DPCM_TYPE_PRE;
    pRelayout->IMGSENSOR_DPCM_TYPE_CAP = pInfo->IMGSENSOR_DPCM_TYPE_CAP;
    pRelayout->IMGSENSOR_DPCM_TYPE_VD = pInfo->IMGSENSOR_DPCM_TYPE_VD;
    pRelayout->IMGSENSOR_DPCM_TYPE_VD1 = pInfo->IMGSENSOR_DPCM_TYPE_VD1;
    pRelayout->IMGSENSOR_DPCM_TYPE_VD2 = pInfo->IMGSENSOR_DPCM_TYPE_VD2;
    pRelayout->PerFrameCTL_Support = pInfo->PerFrameCTL_Support;
    pRelayout->SCAM_DataNumber = (MUINT32)pInfo->SCAM_DataNumber;
    pRelayout->SCAM_DDR_En = pInfo->SCAM_DDR_En;
    pRelayout->SCAM_CLK_INV = pInfo->SCAM_CLK_INV;
    // Part 2 : Config information
    pRelayout->SensorImageMirror = (MUINT32)pInfo->SensorImageMirror;
    pRelayout->EnableShutterTansfer = pInfo->EnableShutterTansfer;
    pRelayout->EnableFlashlightTansfer = pInfo->EnableFlashlightTansfer;
    pRelayout->SensorOperationMode = pInfo->SensorOperationMode;
    pRelayout->ImageTargetWidth = pInfo->ImageTargetWidth;
    pRelayout->ImageTargetHeight = pInfo->ImageTargetHeight;
    pRelayout->CaptureShutter = (MUINT32)pInfo->CaptureShutter;
    pRelayout->FlashlightDuty = pInfo->FlashlightDuty;
    pRelayout->FlashlightOffset = pInfo->FlashlightOffset;
    pRelayout->FlashlightShutFactor = (MUINT32)pInfo->FlashlightShutFactor;
    pRelayout->FlashlightMinShutter = pInfo->FlashlightMinShutter;
    pRelayout->MetaMode = (MUINT32)pInfo->MetaMode;
    pRelayout->DefaultPclk = pInfo->DefaultPclk;
    pRelayout->Pixels = pInfo->Pixels;
    pRelayout->Lines = pInfo->Lines;
    pRelayout->Shutter = (MUINT32)pInfo->Shutter;
    pRelayout->FrameLines = pInfo->FrameLines;
    //Part 3 : Resolution Information
    pRelayout->SensorPreviewWidth = pInfo->SensorPreviewWidth;
    pRelayout->SensorPreviewHeight = pInfo->SensorPreviewHeight;
    pRelayout->SensorCapWidth = pInfo->SensorCapWidth;
    pRelayout->SensorCapHeight = pInfo->SensorCapHeight;
    pRelayout->SensorVideoWidth = pInfo->SensorVideoWidth;
    pRelayout->SensorVideoHeight = pInfo->SensorVideoHeight;
    pRelayout->SensorVideo1Width = pInfo->SensorVideo1Width;
    pRelayout->SensorVideo1Height = pInfo->SensorVideo1Height;
    pRelayout->SensorVideo2Width = pInfo->SensorVideo2Width;
    pRelayout->SensorVideo2Height = pInfo->SensorVideo2Height;
    pRelayout->SensorCustom1Width = pInfo->SensorCustom1Width;   // new for custom
    pRelayout->SensorCustom1Height = pInfo->SensorCustom1Height;
    pRelayout->SensorCustom2Width = pInfo->SensorCustom2Width;
    pRelayout->SensorCustom2Height = pInfo->SensorCustom2Height;
    pRelayout->SensorCustom3Width = pInfo->SensorCustom3Width;
    pRelayout->SensorCustom3Height = pInfo->SensorCustom3Height;
    pRelayout->SensorCustom4Width = pInfo->SensorCustom4Width;
    pRelayout->SensorCustom4Height = pInfo->SensorCustom4Height;
    pRelayout->SensorCustom5Width = pInfo->SensorCustom5Width;
    pRelayout->SensorCustom5Height = pInfo->SensorCustom5Height;
    pRelayout->SensorEffectivePreviewWidth = pInfo->SensorEffectivePreviewWidth;
    pRelayout->SensorEffectivePreviewHeight = pInfo->SensorEffectivePreviewHeight;
    pRelayout->SensorEffectiveCapWidth = pInfo->SensorEffectiveCapWidth;
    pRelayout->SensorEffectiveCapHeight = pInfo->SensorEffectiveCapHeight;
    pRelayout->SensorEffectiveVideoWidth = pInfo->SensorEffectiveVideoWidth;
    pRelayout->SensorEffectiveVideoHeight = pInfo->SensorEffectiveVideoHeight;
    pRelayout->SensorPreviewWidthOffset = pInfo->SensorPreviewWidthOffset;           /* from effective width to output width*/
    pRelayout->SensorPreviewHeightOffset = pInfo->SensorPreviewHeightOffset;          /* rom effective height to output height */
    pRelayout->SensorCapWidthOffset = pInfo->SensorCapWidthOffset;              /* from effective width to output width */
    pRelayout->SensorCapHeightOffset = pInfo->SensorCapHeightOffset;             /* rom effective height to output height */
    pRelayout->SensorVideoWidthOffset = pInfo->SensorVideoWidthOffset;             /* from effective width to output width */
    pRelayout->SensorVideoHeightOffset = pInfo->SensorVideoHeightOffset;            /* from effective height to output height */
}

static void restore_SENSORDRV_INFO(PSENSORDRV_INFO_STRUCT pInfo, PRELAYOUT_SENSORDRV_INFO_STRUCT pRelayout)
{
    // Part 1 : Basic information
    pInfo->SensorPreviewResolutionX = pRelayout->SensorPreviewResolutionX;
    pInfo->SensorPreviewResolutionY = pRelayout->SensorPreviewResolutionY;
    pInfo->SensorPreviewResolutionX_ZSD = pRelayout->SensorPreviewResolutionX_ZSD;
    pInfo->SensorPreviewResolutionY_ZSD = pRelayout->SensorPreviewResolutionY_ZSD;
    pInfo->SensorFullResolutionX = pRelayout->SensorFullResolutionX;
    pInfo->SensorFullResolutionY = pRelayout->SensorFullResolutionY;
    pInfo->SensorClockFreq = pRelayout->SensorClockFreq;
    pInfo->SensorCameraPreviewFrameRate = pRelayout->SensorCameraPreviewFrameRate;
    pInfo->SensorVideoFrameRate = pRelayout->SensorVideoFrameRate;
    pInfo->SensorStillCaptureFrameRate = pRelayout->SensorStillCaptureFrameRate;
    pInfo->SensorWebCamCaptureFrameRate = pRelayout->SensorWebCamCaptureFrameRate;
    pInfo->SensorClockPolarity = pRelayout->SensorClockPolarity;
    pInfo->SensorClockFallingPolarity = pRelayout->SensorClockFallingPolarity;
    pInfo->SensorClockRisingCount = pRelayout->SensorClockRisingCount;
    pInfo->SensorClockFallingCount = pRelayout->SensorClockFallingCount;
    pInfo->SensorClockDividCount = pRelayout->SensorClockDividCount;
    pInfo->SensorPixelClockCount = pRelayout->SensorPixelClockCount;
    pInfo->SensorDataLatchCount = pRelayout->SensorDataLatchCount;
    pInfo->SensorHsyncPolarity = pRelayout->SensorHsyncPolarity;
    pInfo->SensorVsyncPolarity = pRelayout->SensorVsyncPolarity;
    pInfo->SensorInterruptDelayLines = pRelayout->SensorInterruptDelayLines;
    pInfo->SensorResetActiveHigh = pRelayout->SensorResetActiveHigh;
    pInfo->SensorResetDelayCount = pRelayout->SensorResetDelayCount;
    pInfo->SensroInterfaceType = (ACDK_SENSOR_INTERFACE_TYPE_ENUM)pRelayout->SensroInterfaceType;
    pInfo->SensorOutputDataFormat = (ACDK_SENSOR_OUTPUT_DATA_FORMAT_ENUM)pRelayout->SensorOutputDataFormat;
    pInfo->SensorMIPILaneNumber = (ACDK_SENSOR_MIPI_LANE_NUMBER_ENUM)pRelayout->SensorMIPILaneNumber;
    pInfo->CaptureDelayFrame = pRelayout->CaptureDelayFrame;
    pInfo->PreviewDelayFrame = pRelayout->PreviewDelayFrame;
    pInfo->VideoDelayFrame = pRelayout->VideoDelayFrame;
    pInfo->HighSpeedVideoDelayFrame = pRelayout->HighSpeedVideoDelayFrame;
    pInfo->SlimVideoDelayFrame = pRelayout->SlimVideoDelayFrame;
    pInfo->YUVAwbDelayFrame = pRelayout->YUVAwbDelayFrame;
    pInfo->YUVEffectDelayFrame = pRelayout->YUVEffectDelayFrame;
    pInfo->Custom1DelayFrame = pRelayout->Custom1DelayFrame;
    pInfo->Custom2DelayFrame = pRelayout->Custom2DelayFrame;
    pInfo->Custom3DelayFrame = pRelayout->Custom3DelayFrame;
    pInfo->Custom4DelayFrame = pRelayout->Custom4DelayFrame;
    pInfo->Custom5DelayFrame = pRelayout->Custom5DelayFrame;
    pInfo->SensorGrabStartX_PRV = pRelayout->SensorGrabStartX_PRV;
    pInfo->SensorGrabStartY_PRV = pRelayout->SensorGrabStartY_PRV;
    pInfo->SensorGrabStartX_CAP = pRelayout->SensorGrabStartX_CAP;
    pInfo->SensorGrabStartY_CAP = pRelayout->SensorGrabStartY_CAP;
    pInfo->SensorGrabStartX_VD = pRelayout->SensorGrabStartX_VD;
    pInfo->SensorGrabStartY_VD = pRelayout->SensorGrabStartY_VD;
    pInfo->SensorGrabStartX_VD1 = pRelayout->SensorGrabStartX_VD1;
    pInfo->SensorGrabStartY_VD1 = pRelayout->SensorGrabStartY_VD1;
    pInfo->SensorGrabStartX_VD2 = pRelayout->SensorGrabStartX_VD2;
    pInfo->SensorGrabStartY_VD2 = pRelayout->SensorGrabStartY_VD2;
    pInfo->SensorGrabStartX_CST1 = pRelayout->SensorGrabStartX_CST1;
    pInfo->SensorGrabStartY_CST1 = pRelayout->SensorGrabStartY_CST1;
    pInfo->SensorGrabStartX_CST2 = pRelayout->SensorGrabStartX_CST2;
    pInfo->SensorGrabStartY_CST2 = pRelayout->SensorGrabStartY_CST2;
    pInfo->SensorGrabStartX_CST3 = pRelayout->SensorGrabStartX_CST3;
    pInfo->SensorGrabStartY_CST3 = pRelayout->SensorGrabStartY_CST3;
    pInfo->SensorGrabStartX_CST4 = pRelayout->SensorGrabStartX_CST4;
    pInfo->SensorGrabStartY_CST4 = pRelayout->SensorGrabStartY_CST4;
    pInfo->SensorGrabStartX_CST5 = pRelayout->SensorGrabStartX_CST5;
    pInfo->SensorGrabStartY_CST5 = pRelayout->SensorGrabStartY_CST5;
    pInfo->SensorDrivingCurrent = pRelayout->SensorDrivingCurrent;
    pInfo->AEShutDelayFrame = pRelayout->AEShutDelayFrame;
    pInfo->AESensorGainDelayFrame = pRelayout->AESensorGainDelayFrame;
    pInfo->AEISPGainDelayFrame = pRelayout->AEISPGainDelayFrame;
    pInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = pRelayout->MIPIDataLowPwr2HighSpeedTermDelayCount;
    pInfo->MIPIDataLowPwr2HSSettleDelayM0 = pRelayout->MIPIDataLowPwr2HSSettleDelayM0;
    pInfo->MIPIDataLowPwr2HSSettleDelayM1 = pRelayout->MIPIDataLowPwr2HSSettleDelayM1;
    pInfo->MIPIDataLowPwr2HSSettleDelayM2 = pRelayout->MIPIDataLowPwr2HSSettleDelayM2;
    pInfo->MIPIDataLowPwr2HSSettleDelayM3 = pRelayout->MIPIDataLowPwr2HSSettleDelayM3;
    pInfo->MIPIDataLowPwr2HSSettleDelayM4 = pRelayout->MIPIDataLowPwr2HSSettleDelayM4;
    pInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = pRelayout->MIPIDataLowPwr2HighSpeedSettleDelayCount;
    pInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = pRelayout->MIPICLKLowPwr2HighSpeedTermDelayCount;
    pInfo->SensorWidthSampling = pRelayout->SensorWidthSampling;
    pInfo->SensorHightSampling = pRelayout->SensorHightSampling;
    pInfo->SensorPacketECCOrder = pRelayout->SensorPacketECCOrder;
    pInfo->iHDR_First_IS_LE = pRelayout->iHDR_First_IS_LE;
    pInfo->iHDRSupport = pRelayout->iHDRSupport;
    pInfo->PDAF_Support = pRelayout->PDAF_Support;
    pInfo->SensorModeNum = pRelayout->SensorModeNum;
    pInfo->MIPIsensorType = (SENSOR_MIPI_TYPE_ENUM)pRelayout->MIPIsensorType;
    pInfo->SettleDelayMode = (SENSOR_SETTLEDELAY_MODE_ENUM)pRelayout->SettleDelayMode;
    pInfo->IMGSENSOR_DPCM_TYPE_PRE = pRelayout->IMGSENSOR_DPCM_TYPE_PRE;
    pInfo->IMGSENSOR_DPCM_TYPE_CAP = pRelayout->IMGSENSOR_DPCM_TYPE_CAP;
    pInfo->IMGSENSOR_DPCM_TYPE_VD = pRelayout->IMGSENSOR_DPCM_TYPE_VD;
    pInfo->IMGSENSOR_DPCM_TYPE_VD1 = pRelayout->IMGSENSOR_DPCM_TYPE_VD1;
    pInfo->IMGSENSOR_DPCM_TYPE_VD2 = pRelayout->IMGSENSOR_DPCM_TYPE_VD2;
    pInfo->PerFrameCTL_Support = pRelayout->PerFrameCTL_Support;
    pInfo->SCAM_DataNumber = (SENSOR_SCAM_DATA_CHANNEL_ENUM)pRelayout->SCAM_DataNumber;
    pInfo->SCAM_DDR_En = pRelayout->SCAM_DDR_En;
    pInfo->SCAM_CLK_INV = pRelayout->SCAM_CLK_INV;
    // Part 2 : Config information
    pInfo->SensorImageMirror = (ACDK_SENSOR_IMAGE_MIRROR_ENUM)pRelayout->SensorImageMirror;
    pInfo->EnableShutterTansfer = pRelayout->EnableShutterTansfer;
    pInfo->EnableFlashlightTansfer = pRelayout->EnableFlashlightTansfer;
    pInfo->SensorOperationMode = (ACDK_SENSOR_OPERATION_MODE_ENUM)pRelayout->SensorOperationMode;
    pInfo->ImageTargetWidth = pRelayout->ImageTargetWidth;
    pInfo->ImageTargetHeight = pRelayout->ImageTargetHeight;
    pInfo->CaptureShutter = pRelayout->CaptureShutter;
    pInfo->FlashlightDuty = pRelayout->FlashlightDuty;
    pInfo->FlashlightOffset = pRelayout->FlashlightOffset;
    pInfo->FlashlightShutFactor = pRelayout->FlashlightShutFactor;
    pInfo->FlashlightMinShutter = pRelayout->FlashlightMinShutter;
    pInfo->MetaMode = (ACDK_CAMERA_OPERATION_MODE_ENUM)pRelayout->MetaMode;
    pInfo->DefaultPclk = pRelayout->DefaultPclk;
    pInfo->Pixels = pRelayout->Pixels;
    pInfo->Lines = pRelayout->Lines;
    pInfo->Shutter = pRelayout->Shutter;
    pInfo->FrameLines = pRelayout->FrameLines;
    //Part 3 : Resolution Information
    pInfo->SensorPreviewWidth = pRelayout->SensorPreviewWidth;
    pInfo->SensorPreviewHeight = pRelayout->SensorPreviewHeight;
    pInfo->SensorCapWidth = pRelayout->SensorCapWidth;
    pInfo->SensorCapHeight = pRelayout->SensorCapHeight;
    pInfo->SensorVideoWidth = pRelayout->SensorVideoWidth;
    pInfo->SensorVideoHeight = pRelayout->SensorVideoHeight;
    pInfo->SensorVideo1Width = pRelayout->SensorVideo1Width;
    pInfo->SensorVideo1Height = pRelayout->SensorVideo1Height;
    pInfo->SensorVideo2Width = pRelayout->SensorVideo2Width;
    pInfo->SensorVideo2Height = pRelayout->SensorVideo2Height;
    pInfo->SensorCustom1Width = pRelayout->SensorCustom1Width;   // new for custom
    pInfo->SensorCustom1Height = pRelayout->SensorCustom1Height;
    pInfo->SensorCustom2Width = pRelayout->SensorCustom2Width;
    pInfo->SensorCustom2Height = pRelayout->SensorCustom2Height;
    pInfo->SensorCustom3Width = pRelayout->SensorCustom3Width;
    pInfo->SensorCustom3Height = pRelayout->SensorCustom3Height;
    pInfo->SensorCustom4Width = pRelayout->SensorCustom4Width;
    pInfo->SensorCustom4Height = pRelayout->SensorCustom4Height;
    pInfo->SensorCustom5Width = pRelayout->SensorCustom5Width;
    pInfo->SensorCustom5Height = pRelayout->SensorCustom5Height;
    pInfo->SensorEffectivePreviewWidth = pRelayout->SensorEffectivePreviewWidth;
    pInfo->SensorEffectivePreviewHeight = pRelayout->SensorEffectivePreviewHeight;
    pInfo->SensorEffectiveCapWidth = pRelayout->SensorEffectiveCapWidth;
    pInfo->SensorEffectiveCapHeight = pRelayout->SensorEffectiveCapHeight;
    pInfo->SensorEffectiveVideoWidth = pRelayout->SensorEffectiveVideoWidth;
    pInfo->SensorEffectiveVideoHeight = pRelayout->SensorEffectiveVideoHeight;
    pInfo->SensorPreviewWidthOffset = pRelayout->SensorPreviewWidthOffset;           /* from effective width to output width*/
    pInfo->SensorPreviewHeightOffset = pRelayout->SensorPreviewHeightOffset;          /* rom effective height to output height */
    pInfo->SensorCapWidthOffset = pRelayout->SensorCapWidthOffset;              /* from effective width to output width */
    pInfo->SensorCapHeightOffset = pRelayout->SensorCapHeightOffset;             /* rom effective height to output height */
    pInfo->SensorVideoWidthOffset = pRelayout->SensorVideoWidthOffset;             /* from effective width to output width */
    pInfo->SensorVideoHeightOffset = pRelayout->SensorVideoHeightOffset;            /* from effective height to output height */
}


Mutex ImgsensorProfiles::sLock;
String8 ImgsensorProfiles::mXmlFile;
bool ImgsensorProfiles::sIsInitialized = false;
ImgsensorProfiles *ImgsensorProfiles::sInstance = NULL;

ImgsensorProfiles::ImgsensorProfiles()
                 : mCurrentCameraIdx(-1)
                 , mMap()
{
    mpSensorDrvInfos[0] = 0;
    mpSensorDrvInfos[1] = 0;
    mpSensorDrvInfos[2] = 0;
}



/*static*/ ImgsensorProfiles*
ImgsensorProfiles::getInstance()
{
    ALOGV("getInstance");
    Mutex::Autolock lock(sLock);

    if (!sIsInitialized) {
        char value[PROPERTY_VALUE_MAX];
        if (property_get("vendor.imgsensor.configs.xml", value, NULL) <= 0) {
            const char *defaultXmlFile = "/sdcard/imgsensor.configs.xml";
            mXmlFile = defaultXmlFile;
        } else {
            mXmlFile = value;
        }
        //
        FILE *fp = fopen(mXmlFile.string(), "r");
        if (fp == NULL) {
            ALOGW("could not find imgsensor.config.xml file");
            return NULL;
        } else {
            fclose(fp);  // close the file first.
        }
        sInstance = new ImgsensorProfiles();
        CHECK(sInstance != NULL);
        sIsInitialized = true;
    }

    return sInstance;
}

void
ImgsensorProfiles::
getSensorDrvInfoFromXmlFile(PSENSORDRV_INFO_STRUCT pSensorDrvInfos[3])
{
    FILE *fp = NULL;
    //
    mpSensorDrvInfos[0] = pSensorDrvInfos[0];
    mpSensorDrvInfos[1] = pSensorDrvInfos[1];
    mpSensorDrvInfos[2] = pSensorDrvInfos[2];
    relayout_SENSORDRV_INFO(&mRelayoutSensorDrvInfos[0], mpSensorDrvInfos[0]);
    relayout_SENSORDRV_INFO(&mRelayoutSensorDrvInfos[1], mpSensorDrvInfos[1]);
    relayout_SENSORDRV_INFO(&mRelayoutSensorDrvInfos[2], mpSensorDrvInfos[2]);
    //
    CHECK((fp = fopen(mXmlFile.string(), "r")));

    XML_Parser parser = ::XML_ParserCreate(NULL);
    CHECK(parser != NULL);

    ::XML_SetUserData(parser, this);
    ::XML_SetElementHandler(parser, startElementHandler, NULL);

    /*
      FIXME:
      expat is not compiled with -DXML_DTD. We don't have DTD parsing support.

      if (!::XML_SetParamEntityParsing(parser, XML_PARAM_ENTITY_PARSING_ALWAYS)) {
          ALOGE("failed to enable DTD support in the xml file");
          return UNKNOWN_ERROR;
      }
     */

    const int BUFF_SIZE = 512;
    for (;;) {
        void *buff = ::XML_GetBuffer(parser, BUFF_SIZE);
        if (buff == NULL) {
            ALOGE("failed to in call to XML_GetBuffer()");
            goto exit;
        }

        int bytes_read = ::fread(buff, 1, BUFF_SIZE, fp);
        if (bytes_read < 0) {
            ALOGE("failed in call to read");
            goto exit;
        }

        CHECK(::XML_ParseBuffer(parser, bytes_read, bytes_read == 0));

        if (bytes_read == 0) break;  // done parsing the xml file
    }

    restore_SENSORDRV_INFO(mpSensorDrvInfos[0], &mRelayoutSensorDrvInfos[0]);
    restore_SENSORDRV_INFO(mpSensorDrvInfos[1], &mRelayoutSensorDrvInfos[1]);
    restore_SENSORDRV_INFO(mpSensorDrvInfos[2], &mRelayoutSensorDrvInfos[2]);

exit:
    ::XML_ParserFree(parser);
    ::fclose(fp);
    return;
}

/*static*/ void
ImgsensorProfiles::startElementHandler(void *userData, const char *name, const char **atts) {
    ImgsensorProfiles *profiles = (ImgsensorProfiles *) userData;
    if (strcmp("sensor", name) == 0) {
        CHECK(!strcmp("sensorDevId", atts[0]));
        int devId = ::atoi(atts[1]);
        profiles->mMap.clear();
        profiles->mCurrentCameraIdx++;
        /*switch(devId) {
            case SENSOR_MAIN:
                profiles->mCurrentCameraIdx = 0;
                break;
            case SENSOR_SUB:
                profiles->mCurrentCameraIdx = 1;
                break;
            case SENSOR_MAIN_2:
                profiles->mCurrentCameraIdx = 2;
                break;
            default:
                break;
        }*/
        if(profiles->mCurrentCameraIdx >= 0) {
            profiles->mapTag2ImgInfoItems(&profiles->mRelayoutSensorDrvInfos[profiles->mCurrentCameraIdx]);
        }
    } else if(strcmp("imgsensor_info", name) == 0) {
        CHECK(profiles->mCurrentCameraIdx >= 0);
        uintptr_t ptr = profiles->mMap.valueFor(String8(atts[0]));
        uint32_t oldVal = *reinterpret_cast<uint32_t*>(ptr);
        uint32_t newVal = ::atoi(atts[1]);
        if(oldVal != newVal) {
            ALOGI("change [%s] from %d to %d", atts[0], oldVal, newVal);
            *reinterpret_cast<uint32_t*>(ptr) = newVal;
        }
    } else if(strcmp("imgsensor_mode", name) == 0) {
        createImgsensorMode(atts, profiles);
    } else if(strcmp("scenario", name) == 0) {
        CHECK(!strcmp("value", atts[0]) && profiles->mCurrentCameraIdx >= 0);
        switch(atts[1][0]) {
            case 'p': //pre
                profiles->mapTag2PreScenItems(&profiles->mRelayoutSensorDrvInfos[profiles->mCurrentCameraIdx]);
                break;
            case 'c': //cap
                profiles->mapTag2CapScenItems(&profiles->mRelayoutSensorDrvInfos[profiles->mCurrentCameraIdx]);
                break;
            case 'n': //normal video
                profiles->mapTag2VdoScenItems(&profiles->mRelayoutSensorDrvInfos[profiles->mCurrentCameraIdx]);
                break;
            case 'h': //hs video
                profiles->mapTag2Vdo1ScenItems(&profiles->mRelayoutSensorDrvInfos[profiles->mCurrentCameraIdx]);
                break;
            case 's': //slim video
                profiles->mapTag2Vdo2ScenItems(&profiles->mRelayoutSensorDrvInfos[profiles->mCurrentCameraIdx]);
                break;
            default:
                break;
        }
    }
}

/*static*/ void
ImgsensorProfiles::createImgsensorMode(const char **atts, ImgsensorProfiles *profiles)
{
    CHECK(!strcmp("startx",                   atts[0])  &&
          !strcmp("starty",                    atts[2])  &&
          !strcmp("grabwindow_width",          atts[4])  &&
          !strcmp("grabwindow_height",         atts[6])  &&
          !strcmp("mipi_data_lp2hs_settle_dc", atts[8])  &&
          !strcmp("DPCM_INFO",                 atts[10]) &&
          profiles->mCurrentCameraIdx >= 0);
    for(int i=0; i <= 5; i++) {
        uintptr_t ptr = profiles->mMap.valueFor(String8(atts[2*i]));
        uint32_t oldVal = *reinterpret_cast<uint32_t*>(ptr);
        uint32_t newVal = ::atoi(atts[2*i + 1]);
        if(oldVal != newVal) {
            ALOGI("change [%s] from %d to %d", atts[2*i], oldVal, newVal);
            *reinterpret_cast<uint32_t*>(ptr) = newVal;
        }
    }
}


status_t ImgsensorProfiles::add(const String8& key, const uintptr_t value)
{
    if (mMap.indexOfKey(key) < 0) {
        mMap.add(key, value);
        return NO_ERROR;
    } else {
        mMap.replaceValueFor(key, value);
        return ALREADY_EXISTS;
    }
}

void
ImgsensorProfiles::mapTag2PreScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo) {
    #define tagcast(mem) (reinterpret_cast<uintptr_t>(&pSensorDrvInfo->mem))
    add(String8("startx"), tagcast(SensorGrabStartX_PRV));
    add(String8("starty"), tagcast(SensorGrabStartY_PRV));
    add(String8("grabwindow_width"), tagcast(SensorPreviewWidth));
    add(String8("grabwindow_height"), tagcast(SensorPreviewHeight));
    add(String8("mipi_data_lp2hs_settle_dc"), tagcast(MIPIDataLowPwr2HSSettleDelayM0));
    add(String8("DPCM_INFO"), tagcast(IMGSENSOR_DPCM_TYPE_PRE));
}

void
ImgsensorProfiles::mapTag2CapScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo) {
    #define tagcast(mem) (reinterpret_cast<uintptr_t>(&pSensorDrvInfo->mem))
    add(String8("startx"), tagcast(SensorGrabStartX_CAP));
    add(String8("starty"), tagcast(SensorGrabStartY_CAP));
    add(String8("grabwindow_width"), tagcast(SensorCapWidth));
    add(String8("grabwindow_height"), tagcast(SensorCapHeight));
    add(String8("mipi_data_lp2hs_settle_dc"), tagcast(MIPIDataLowPwr2HSSettleDelayM1));
    add(String8("DPCM_INFO"), tagcast(IMGSENSOR_DPCM_TYPE_CAP));
}

void
ImgsensorProfiles::mapTag2VdoScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo) {
    #define tagcast(mem) (reinterpret_cast<uintptr_t>(&pSensorDrvInfo->mem))
    add(String8("startx"), tagcast(SensorGrabStartX_VD));
    add(String8("starty"), tagcast(SensorGrabStartY_VD));
    add(String8("grabwindow_width"), tagcast(SensorVideoWidth));
    add(String8("grabwindow_height"), tagcast(SensorVideoHeight));
    add(String8("mipi_data_lp2hs_settle_dc"), tagcast(MIPIDataLowPwr2HSSettleDelayM2));
    add(String8("DPCM_INFO"), tagcast(IMGSENSOR_DPCM_TYPE_VD));
}

void
ImgsensorProfiles::mapTag2Vdo1ScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo) {
    #define tagcast(mem) (reinterpret_cast<uintptr_t>(&pSensorDrvInfo->mem))
    add(String8("startx"), tagcast(SensorGrabStartX_VD1));
    add(String8("starty"), tagcast(SensorGrabStartY_VD1));
    add(String8("grabwindow_width"), tagcast(SensorVideo1Width));
    add(String8("grabwindow_height"), tagcast(SensorVideo1Height));
    add(String8("mipi_data_lp2hs_settle_dc"), tagcast(MIPIDataLowPwr2HSSettleDelayM3));
    add(String8("DPCM_INFO"), tagcast(IMGSENSOR_DPCM_TYPE_VD1));
}

void
ImgsensorProfiles::mapTag2Vdo2ScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo) {
    #define tagcast(mem) (reinterpret_cast<uintptr_t>(&pSensorDrvInfo->mem))
    add(String8("startx"), tagcast(SensorGrabStartX_VD2));
    add(String8("starty"), tagcast(SensorGrabStartY_VD2));
    add(String8("grabwindow_width"), tagcast(SensorVideo2Width));
    add(String8("grabwindow_height"), tagcast(SensorVideo2Height));
    add(String8("mipi_data_lp2hs_settle_dc"), tagcast(MIPIDataLowPwr2HSSettleDelayM4));
    add(String8("DPCM_INFO"), tagcast(IMGSENSOR_DPCM_TYPE_VD2));
}

void
ImgsensorProfiles::mapTag2ImgInfoItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo) {
    //mMap.clear();
    #define tagcast(mem) (reinterpret_cast<uintptr_t>(&pSensorDrvInfo->mem))
    mMap.add(String8("SensorClockPolarity"), tagcast(SensorClockPolarity));
    mMap.add(String8("SensorClockFallingPolarity"), tagcast(SensorClockFallingPolarity));
    mMap.add(String8("SensorInterruptDelayLines"), tagcast(SensorInterruptDelayLines));
    mMap.add(String8("SensorResetActiveHigh"), tagcast(SensorResetActiveHigh));
    mMap.add(String8("SensorResetDelayCount"), tagcast(SensorResetDelayCount));
    mMap.add(String8("SensorHsyncPolarity"), tagcast(SensorHsyncPolarity));
    mMap.add(String8("SensorVsyncPolarity"), tagcast(SensorVsyncPolarity));
    mMap.add(String8("sensor_interface_type"), tagcast(SensroInterfaceType));
    mMap.add(String8("mipi_sensor_type"), tagcast(MIPIsensorType));
    mMap.add(String8("mipi_settle_delay_mode"), tagcast(SettleDelayMode));
    mMap.add(String8("sensor_output_dataformat"), tagcast(SensorOutputDataFormat));
    mMap.add(String8("cap_delay_frame"), tagcast(CaptureDelayFrame));
    mMap.add(String8("pre_delay_frame"), tagcast(PreviewDelayFrame));
    mMap.add(String8("video_delay_frame"), tagcast(VideoDelayFrame));
    mMap.add(String8("hs_video_delay_frame"), tagcast(HighSpeedVideoDelayFrame));
    mMap.add(String8("slim_video_delay_frame"), tagcast(SlimVideoDelayFrame));
    mMap.add(String8("isp_driving_current"), tagcast(SensorDrivingCurrent));
    mMap.add(String8("ae_shutter_delay_frame"), tagcast(AEShutDelayFrame));
    mMap.add(String8("ae_sensor_gain_delay_frame"), tagcast(AESensorGainDelayFrame));
    mMap.add(String8("ae_ispgain_delay_frame"), tagcast(AEISPGainDelayFrame));
    mMap.add(String8("ihdr_support"), tagcast(iHDRSupport));
    mMap.add(String8("ihdr_le_firstline"), tagcast(iHDR_First_IS_LE));
    mMap.add(String8("mipi_lane_num"), tagcast(SensorMIPILaneNumber));
    mMap.add(String8("mclk"), tagcast(SensorClockFreq));
    mMap.add(String8("SensorClockDividCount"), tagcast(SensorClockDividCount));
    mMap.add(String8("SensorClockRisingCount"), tagcast(SensorClockRisingCount));
    mMap.add(String8("SensorClockFallingCount"), tagcast(SensorClockFallingCount));
    mMap.add(String8("SensorPixelClockCount"), tagcast(SensorPixelClockCount));
    mMap.add(String8("SensorDataLatchCount"), tagcast(SensorDataLatchCount));
    mMap.add(String8("MIPIDataLowPwr2HighSpeedTermDelayCount"), tagcast(MIPIDataLowPwr2HighSpeedTermDelayCount));
    mMap.add(String8("MIPICLKLowPwr2HighSpeedTermDelayCount"), tagcast(MIPICLKLowPwr2HighSpeedTermDelayCount));
    mMap.add(String8("SensorWidthSampling"), tagcast(SensorWidthSampling));
    mMap.add(String8("SensorHightSampling"), tagcast(SensorHightSampling));
    mMap.add(String8("SensorPacketECCOrder"), tagcast(SensorPacketECCOrder));
}

