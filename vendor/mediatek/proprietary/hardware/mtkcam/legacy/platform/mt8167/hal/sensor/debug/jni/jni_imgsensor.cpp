/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "IMGSENSOR_JNI"

#include <utils/Trace.h>
#include <cutils/properties.h>

#include <jni.h>
#include <errno.h>
#include <fcntl.h>

#include <utils/Log.h>
#include <android/log.h>
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include <String8.h>

#include <inc/camera_custom_sensor.h>
#include <hal/sensor/sensor_drv.h>
#include <hal/sensor/seninf_drv.h>

using android::String8;

/******************************************************************************
*
*******************************************************************************/
#define MY_LOGV(fmt, arg...)        ALOGV("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        ALOGD("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        ALOGI("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        ALOGW("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        ALOGE("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)

static String8 gSensorNames[2];

typedef struct
{
    // Part 1 : Basic information
    MUINT32 SensorPreviewResolutionX;
    MUINT32 SensorPreviewResolutionY;
    MUINT32 SensorPreviewResolutionX_ZSD;
    MUINT32 SensorPreviewResolutionY_ZSD;
    MUINT32 SensorFullResolutionX;
    MUINT32 SensorFullResolutionY;
    MUINT32 SensorClockFreq;              /* MHz */
    MUINT32 SensorCameraPreviewFrameRate;
    MUINT32 SensorVideoFrameRate;
    MUINT32 SensorStillCaptureFrameRate;
    MUINT32 SensorWebCamCaptureFrameRate;
    MUINT32 SensorClockPolarity;          /* SENSOR_CLOCK_POLARITY_HIGH/SENSOR_CLOCK_POLARITY_Low */
    MUINT32 SensorClockFallingPolarity;
    MUINT32 SensorClockRisingCount;       /* 0..15 */
    MUINT32 SensorClockFallingCount;      /* 0..15 */
    MUINT32 SensorClockDividCount;        /* 0..15 */
    MUINT32 SensorPixelClockCount;        /* 0..15 */
    MUINT32 SensorDataLatchCount;         /* 0..15 */
    MUINT32 SensorHsyncPolarity;
    MUINT32 SensorVsyncPolarity;
    MUINT32 SensorInterruptDelayLines;
    MUINT32 SensorResetActiveHigh;
    MUINT32 SensorResetDelayCount;
    MUINT32 SensroInterfaceType;
    MUINT32 SensorOutputDataFormat;
    MUINT32 SensorMIPILaneNumber;   /* lane number : 1, 2, 3, 4 */
    MUINT32 CaptureDelayFrame;
    MUINT32 PreviewDelayFrame;
    MUINT32 VideoDelayFrame;
    MUINT32 HighSpeedVideoDelayFrame;
    MUINT32 SlimVideoDelayFrame;
    MUINT32 YUVAwbDelayFrame;
    MUINT32 YUVEffectDelayFrame;
    MUINT32 Custom1DelayFrame;
    MUINT32 Custom2DelayFrame;
    MUINT32 Custom3DelayFrame;
    MUINT32 Custom4DelayFrame;
    MUINT32 Custom5DelayFrame;
    MUINT32 SensorGrabStartX_PRV;       /* MSDK_SCENARIO_ID_CAMERA_PREVIEW */
    MUINT32 SensorGrabStartY_PRV;
    MUINT32 SensorGrabStartX_CAP;       /* MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG & ZSD */
    MUINT32 SensorGrabStartY_CAP;
    MUINT32 SensorGrabStartX_VD;        /* MSDK_SCENARIO_ID_VIDEO_PREVIEW */
    MUINT32 SensorGrabStartY_VD;
    MUINT32 SensorGrabStartX_VD1;       /* MSDK_SCENARIO_ID_VIDEO_PREVIEW */
    MUINT32 SensorGrabStartY_VD1;
    MUINT32 SensorGrabStartX_VD2;       /* MSDK_SCENARIO_ID_VIDEO_PREVIEW */
    MUINT32 SensorGrabStartY_VD2;
    MUINT32 SensorGrabStartX_CST1;      /* SENSOR_SCENARIO_ID_CUSTOM1 */
    MUINT32 SensorGrabStartY_CST1;
    MUINT32 SensorGrabStartX_CST2;      /* SENSOR_SCENARIO_ID_CUSTOM2 */
    MUINT32 SensorGrabStartY_CST2;
    MUINT32 SensorGrabStartX_CST3;       /* SENSOR_SCENARIO_ID_CUSTOM3 */
    MUINT32 SensorGrabStartY_CST3;
    MUINT32 SensorGrabStartX_CST4;       /* SENSOR_SCENARIO_ID_CUSTOM4 */
    MUINT32 SensorGrabStartY_CST4;
    MUINT32 SensorGrabStartX_CST5;       /* SENSOR_SCENARIO_ID_CUSTOM5 */
    MUINT32 SensorGrabStartY_CST5;
    MUINT32 SensorDrivingCurrent;
    MUINT32 SensorMasterClockSwitch;
    MUINT32 AEShutDelayFrame;             /* The frame of setting shutter default 0 for TG int */
    MUINT32 AESensorGainDelayFrame;   /* The frame of setting sensor gain */
    MUINT32 AEISPGainDelayFrame;
    MUINT32 MIPIDataLowPwr2HighSpeedTermDelayCount;
    MUINT32 MIPIDataLowPwr2HSSettleDelayM0;/*Preview Settle delay*/
    MUINT32 MIPIDataLowPwr2HSSettleDelayM1;/*Capture Settle delay*/
    MUINT32 MIPIDataLowPwr2HSSettleDelayM2;/*video Settle delay*/
    MUINT32 MIPIDataLowPwr2HSSettleDelayM3;/*video1 Settle delay*/
    MUINT32 MIPIDataLowPwr2HSSettleDelayM4;/*video2 Settle delay*/
    MUINT32 MIPIDataLowPwr2HighSpeedSettleDelayCount;
    MUINT32 MIPICLKLowPwr2HighSpeedTermDelayCount;
    MUINT32 SensorWidthSampling;
    MUINT32 SensorHightSampling;
    MUINT32 SensorPacketECCOrder;
    MUINT32 iHDR_First_IS_LE;
    MUINT32 iHDRSupport;
    MUINT32 PDAF_Support;
    MUINT32 SensorModeNum;
    MUINT32 virtualChannelSupport;
    MUINT32 MIPIsensorType;
    MUINT32 SettleDelayMode;
    MUINT32 IMGSENSOR_DPCM_TYPE_PRE;
    MUINT32 IMGSENSOR_DPCM_TYPE_CAP;
    MUINT32 IMGSENSOR_DPCM_TYPE_VD;
    MUINT32 IMGSENSOR_DPCM_TYPE_VD1;
    MUINT32 IMGSENSOR_DPCM_TYPE_VD2;
    MUINT32 PerFrameCTL_Support;
    MUINT32 SCAM_DataNumber;
    MUINT32 SCAM_DDR_En;
    MUINT32 SCAM_CLK_INV;
    // Part 2 : Config information
    MUINT32 SensorImageMirror;
    MUINT32 EnableShutterTansfer;            /* capture only */
    MUINT32 EnableFlashlightTansfer;        /* flash light capture only */
    MUINT32 SensorOperationMode;
    MUINT32 ImageTargetWidth;        /* image captured width */
    MUINT32 ImageTargetHeight;        /* image captuerd height */
    MUINT32 CaptureShutter;            /* capture only */
    MUINT32 FlashlightDuty;            /* flash light capture only */
    MUINT32 FlashlightOffset;        /* flash light capture only */
    MUINT32 FlashlightShutFactor;    /* flash light capture only */
    MUINT32 FlashlightMinShutter;
    MUINT32 MetaMode; /* capture only */
    MUINT32 DefaultPclk;                /*Sensor pixel clock(Ex:24000000) */
    MUINT32 Pixels;                     /* Sensor active pixel number */
    MUINT32 Lines;                      /* Sensor active line number */
    MUINT32 Shutter;                    /* ensor current shutter */
    MUINT32 FrameLines;                 /* valid+dummy lines for minimum shutter */
    //Part 3 : Resolution Information
    MUINT32 SensorPreviewWidth;
    MUINT32 SensorPreviewHeight;
    MUINT32 SensorCapWidth;
    MUINT32 SensorCapHeight;
    MUINT32 SensorVideoWidth;
    MUINT32 SensorVideoHeight;
    MUINT32 SensorVideo1Width;
    MUINT32 SensorVideo1Height;
    MUINT32 SensorVideo2Width;
    MUINT32 SensorVideo2Height;
    MUINT32 SensorCustom1Width;   // new for custom
    MUINT32 SensorCustom1Height;
    MUINT32 SensorCustom2Width;
    MUINT32 SensorCustom2Height;
    MUINT32 SensorCustom3Width;
    MUINT32 SensorCustom3Height;
    MUINT32 SensorCustom4Width;
    MUINT32 SensorCustom4Height;
    MUINT32 SensorCustom5Width;
    MUINT32 SensorCustom5Height;
    MUINT32 SensorEffectivePreviewWidth;
    MUINT32 SensorEffectivePreviewHeight;
    MUINT32 SensorEffectiveCapWidth;
    MUINT32 SensorEffectiveCapHeight;
    MUINT32 SensorEffectiveVideoWidth;
    MUINT32 SensorEffectiveVideoHeight;
    MUINT32 SensorPreviewWidthOffset;           /* from effective width to output width*/
    MUINT32 SensorPreviewHeightOffset;          /* rom effective height to output height */
    MUINT32 SensorCapWidthOffset;              /* from effective width to output width */
    MUINT32 SensorCapHeightOffset;             /* rom effective height to output height */
    MUINT32 SensorVideoWidthOffset;             /* from effective width to output width */
    MUINT32 SensorVideoHeightOffset;            /* from effective height to output height */
    MUINT32 ITEMS_COUNT;
    //Part4:
} RELAYOUT_SENSORDRV_INFO_STRUCT, *PRELAYOUT_SENSORDRV_INFO_STRUCT;

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
    //MY_LOGI("+iHDRSupport %d", pInfo->iHDRSupport);
    pRelayout->iHDRSupport = (MUINT32)pInfo->iHDRSupport;
    //MY_LOGI("-iHDRSupport %d", pRelayout->iHDRSupport);
    pRelayout->PDAF_Support = pInfo->PDAF_Support;
    pRelayout->SensorModeNum = pInfo->SensorModeNum;
    pRelayout->MIPIsensorType = (MUINT32)pInfo->MIPIsensorType;
    pRelayout->SettleDelayMode = (MUINT32)pInfo->SettleDelayMode;
    pRelayout->virtualChannelSupport = (MUINT32)pInfo->virtualChannelSupport;
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
    pRelayout->SensorOperationMode = (MUINT32)pInfo->SensorOperationMode;
    pRelayout->ImageTargetWidth = pInfo->ImageTargetWidth;
    pRelayout->ImageTargetHeight = pInfo->ImageTargetHeight;
    pRelayout->CaptureShutter = pInfo->CaptureShutter;
    pRelayout->FlashlightDuty = pInfo->FlashlightDuty;
    pRelayout->FlashlightOffset = pInfo->FlashlightOffset;
    pRelayout->FlashlightShutFactor = pInfo->FlashlightShutFactor;
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
    MUINT32 *start = &pRelayout->SensorPreviewResolutionX;
    MUINT32 *end   = &pRelayout->ITEMS_COUNT;
    pRelayout->ITEMS_COUNT = end - start;
}

#define offsetof(st, m) ((size_t)&((st*)0)->m)

const char *classPathName = "com/mediatek/commands/GenSensorConfigs";

static jstring com_mediatek_imgsensor_getSensorName(JNIEnv *env, jobject thiz, jint sensorDevId)
{
    int index = -1;
    if(sensorDevId == SENSOR_MAIN) {
        index = 0;
    } else if(sensorDevId == SENSOR_SUB) {
        index = 1;
    }
    return env->NewStringUTF(gSensorNames[index].string());
}


static jint com_mediatek_imgsensor_nativeGetImgsensorInfo(JNIEnv* env, jobject thiz,
        jintArray main, jintArray sub) {
    int ret = 0;
    int i = 0;
    SENSORDRV_INFO_STRUCT sensorDrvInfo[2];
    RELAYOUT_SENSORDRV_INFO_STRUCT relayoutSensorDrvInfo[2];
    SensorDrv *const pSensorDrv = SensorDrv::get();
    SeninfDrv *const pSeninfDrv = SeninfDrv::createInstance();
    if(!pSeninfDrv) {
        //MY_LOGE("pSeninfDrv == NULL");
                return 0;
    }

    ret = pSeninfDrv->init();
    if(ret < 0) {
        //MY_LOGE("pSeninfDrv->init() fail");
                return 0;
    }
    pSeninfDrv->setMclk1(1, 1, 1, 0, 1, 0, 0);
    pSeninfDrv->setMclk2(1, 1, 1, 0, 1, 0, 0);
    //pSeninfDrv->setMclk3(1, 1, 1, 0, 1, 0, 0);  /* No main2 */

    int const iSensorsList = pSensorDrv->impSearchSensor(NULL);

    for (MUINT32 sensorDev = SENSOR_MAIN; sensorDev <= SENSOR_SUB
        && (iSensorsList & sensorDev == sensorDev); sensorDev <<= 1) {
        pSensorDrv->init((SENSOR_DEV_ENUM)sensorDev);
        memset(&sensorDrvInfo[i], 0, sizeof(SENSORDRV_INFO_STRUCT));
        pSensorDrv->getInfo2((SENSOR_DEV_ENUM)sensorDev,&sensorDrvInfo[i]);
        relayout_SENSORDRV_INFO(&relayoutSensorDrvInfo[i], &sensorDrvInfo[i]);
        int length = offsetof(RELAYOUT_SENSORDRV_INFO_STRUCT, ITEMS_COUNT)/sizeof(MUINT32);
        NSFeature::SensorInfoBase* pSensorInfo = NULL;
        if(sensorDev == SENSOR_MAIN) {
            pSensorInfo = pSensorDrv->getMainSensorInfo();
            gSensorNames[0] = pSensorInfo->getDrvName();
            //MY_LOGI("main sensor %s is found", gSensorNames[0].string());
            env->SetIntArrayRegion(main, /*start*/0, length
                , reinterpret_cast<jint*>(&relayoutSensorDrvInfo[i].SensorPreviewResolutionX));
            if (env->ExceptionCheck()) {
            return android::BAD_VALUE;
            }
        } else if(sensorDev == SENSOR_SUB) {
            pSensorInfo = pSensorDrv->getSubSensorInfo();
            gSensorNames[1] = pSensorInfo->getDrvName();
            //MY_LOGI("sub sensor %s is found", gSensorNames[1].string());
            env->SetIntArrayRegion(sub, /*start*/0, length
                , reinterpret_cast<jint*>(&relayoutSensorDrvInfo[i].SensorPreviewResolutionX));
            if (env->ExceptionCheck()) {
            return android::BAD_VALUE;
            }
        }
        i++;
    }

    pSeninfDrv->setMclk1(0, 1, 1, 0, 1, 0, 0);
    pSeninfDrv->setMclk2(0, 1, 1, 0, 1, 0, 0);
    //pSeninfDrv->setMclk3(0, 1, 1, 0, 1, 0, 0); /* No main2 */

    ret = pSeninfDrv->uninit();
    if(ret < 0) {
        //MY_LOGE("pSeninfDrv->uninit() fail");
                return 0;
    }
    pSeninfDrv->destroyInstance();
    return iSensorsList;
}

static JNINativeMethod imgsensorMethods[] = {
    { "nativeGetImgsensorInfo",
      "([I[I)I",
      (void*)com_mediatek_imgsensor_nativeGetImgsensorInfo },
    { "nativeGetSensorName",
      "(I)Ljava/lang/String;",
      (void *)com_mediatek_imgsensor_getSensorName },
};


/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        //MY_LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        //MY_LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, classPathName,
    		imgsensorMethods, sizeof(imgsensorMethods) / sizeof(imgsensorMethods[0]))) {
    	return JNI_FALSE;
    }
   return JNI_TRUE;
}

/*
 * This is called by the VM when the shared library is first loaded.
 */

typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    //MY_LOGI("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
    	//MY_LOGE("ERROR: GetEnv failed");
    	goto bail;
    }
    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE) {
    	//MY_LOGE("ERROR: registerNatives failed");
    	goto bail;
    }
    result = JNI_VERSION_1_4;

    bail:
        return result;
}


