/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 **
 ** Copyright 2010, The Android Open Source Project.
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

#ifndef ANDROID_IMGSENSORPROFILES_H
#define ANDROID_IMGSENSORPROFILES_H
#include <utils/Mutex.h>

enum {
    // Unsigned 8-bit integer (uint8_t)
    TYPE_BYTE = 0,
    //Unsigned 16-bit integer (uint8_t)
    TYPE_SHORT = 1,
    // Signed 32-bit integer (int32_t)
    TYPE_INT32 = 1,
    // 32-bit float (float)
    TYPE_FLOAT = 2,
    // Signed 64-bit integer (int64_t)
    TYPE_INT64 = 3,
    // 64-bit float (double)
    // Number of type fields
    NUM_TYPES
};

//
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
    //Part4:
} RELAYOUT_SENSORDRV_INFO_STRUCT, *PRELAYOUT_SENSORDRV_INFO_STRUCT;

class ImgsensorProfiles {
public:
        /**
     * Returns the singleton instance for subsequence queries.
     * or NULL if error.
     */
    static ImgsensorProfiles* getInstance();
    void   getSensorDrvInfoFromXmlFile(PSENSORDRV_INFO_STRUCT pSensorDrvInfos[3]);

private:
    ImgsensorProfiles& operator=(const ImgsensorProfiles&);  // Don't call me
    ImgsensorProfiles(const ImgsensorProfiles&);           // Don't call me
    ImgsensorProfiles();
    ~ImgsensorProfiles();                                 // Don't delete me

    // Customized element tag handler for parsing the xml configuration file.
    static void startElementHandler(void *userData, const char *name, const char **atts);
    static void createImgsensorMode(const char **atts, ImgsensorProfiles *profiles);
    //
    status_t add(const String8& key, const uintptr_t value);
    void mapTag2ImgInfoItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo);
    void mapTag2PreScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo);
    void mapTag2CapScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo);
    void mapTag2VdoScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo);
    void mapTag2Vdo1ScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo);
    void mapTag2Vdo2ScenItems(PRELAYOUT_SENSORDRV_INFO_STRUCT pSensorDrvInfo);

private:
    static bool sIsInitialized;
    static ImgsensorProfiles *sInstance;
    static Mutex sLock;
    static String8 mXmlFile;
    //
    int mCurrentCameraIdx;
    PSENSORDRV_INFO_STRUCT mpSensorDrvInfos[3];
	RELAYOUT_SENSORDRV_INFO_STRUCT mRelayoutSensorDrvInfos[3];
    //
    DefaultKeyedVector<String8,uintptr_t> mMap;
};

#endif
