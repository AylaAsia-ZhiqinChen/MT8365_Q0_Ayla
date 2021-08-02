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
 * MediaTek Inc. (C) 2019. All rights reserved.
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

#define LOG_TAG "MtkCam/CalibrationProvider"
//
#include <sys/stat.h> //For stat
#include <hardware/camera3.h>
#include <cutils/properties.h>
#include <mtkcam/drv/IHalSensor.h>
#include <mtkcam/utils/std/Log.h>
#include <mtkcam/utils/std/ULog.h>
#include "CalibrationProvider.h"
#include "CalibrationConvertor/CalibrationConvertor.h"
#if (1==HAS_HAL3_FEATURE)
#include <mtkcam3/feature/stereo/hal/stereo_setting_provider.h>
#endif

#include <mtkcam/utils/LogicalCam/IHalLogicalDeviceList.h>
#include <cmath>

CAM_ULOG_DECLARE_MODULE_ID(MOD_UTILITY);

#define EXTERN_EEPROM_PATH      "/sdcard/EEPROM_STEREO"
#define PROPERTY_DUMP_CAL       "vendor.STEREO.dump_cal"

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_ULOGMV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_ULOGMD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_ULOGMI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_ULOGMW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_ULOGME("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

using namespace NSCam;
using namespace android;
using namespace NSCalibrationProvider;
using namespace NSCalibrationConvertor;
/******************************************************************************
 *
 ******************************************************************************/
ICalibrationProvider*
ICalibrationProvider::
getInstance(
    int32_t const   i4OpenId
)
{
    CalibrationProvider* p = new CalibrationProvider(i4OpenId);
    if  ( CC_UNLIKELY( !p ) )
    {
        MY_LOGE("No Memory");
        return  NULL;
    }
    //
    return  p;
}

/******************************************************************************
 *
 ******************************************************************************/
CalibrationProvider::
~CalibrationProvider()
{
    _3rdPartyConvertor = nullptr;
}


/******************************************************************************
 *
 ******************************************************************************/
CalibrationProvider::
CalibrationProvider(
    int32_t const   i4OpenId
)
{
    __init(i4OpenId);
}

/******************************************************************************
 *
 ******************************************************************************/
bool
CalibrationProvider::
getCalibration(ENUM_CALIBRATION_FORMAT format, CalibrationResult *result) const
{
    if(result == nullptr)
    {
        return false;
    }

    if(E_CALIBRATION_GOOGLE_FORMAT == format)
    {
        if(_3rdPartyConvertor->convertToGoogleFormat(*((CalibrationResultInGoogleFormat *)result)))
        {
            MY_LOGD("[#%d] Convert calibration to Google format from 3rd party", _queryParam.openId);
            return true;
        }
        else if(__loadMTKCalibrationInGoogleFormat((CalibrationResultInGoogleFormat *)result))
        {
            MY_LOGD("[#%d] Convert calibration to Google format from MTK", _queryParam.openId);
            return true;
        }
    }
    else if(E_CALIBRATION_MTK_FORMAT == format)
    {
        return __loadMTKCalibration((CalibrationResultInMTKFormat *)result);
    }

    return false;
}

/******************************************************************************
 *
 ******************************************************************************/
void
CalibrationProvider::
__init(int32_t const   i4OpenId)
{
    //Init _queryParam
    _queryParam.openId = i4OpenId;

    auto pHalDeviceList = MAKE_HalLogicalDeviceList();
    if(pHalDeviceList)
    {
        _queryParam.physicalSensorIDs = pHalDeviceList->getSensorId(i4OpenId);

        for(auto &id : _queryParam.physicalSensorIDs)
        {
            std::string name = pHalDeviceList->queryDriverName(id);
            _queryParam.physicalSensorNames.push_back(name);
        }
    }

    _3rdPartyConvertor = new CalibrationConvertor(_queryParam);
}

/******************************************************************************
 *
 ******************************************************************************/
bool
CalibrationProvider::
__loadMTKCalibrationInGoogleFormat(CalibrationResultInGoogleFormat *result) const
{
    if(result == nullptr)
    {
        MY_LOGE("Invalid result: NULL");
        return false;
    }

    //TODO: Read calibration in MTK flow
    // if(/*load calibration from somewhere failed*/)
    {
        // Fill calibration data by sensor & module spec
        result->lensPoseRotation = {0, 0, 0, 0};
        result->lensPoseTranslation = {0, 0, 0};

#if (1==HAS_HAL3_FEATURE)
        auto pHalDeviceList = MAKE_HalLogicalDeviceList();
        if(nullptr == pHalDeviceList)
        {
            MY_LOGE("Cannot get logical device list");
            return false;
        }

        SensorStaticInfo const* pStaticInfo = nullptr;
        const int SENSOR_COUNT = pHalDeviceList->queryNumberOfSensors();
        int logicalDeviceId = -1;
        std::vector<int> physicalSensorIDs;
        if(_queryParam.openId >= SENSOR_COUNT)
        {
            logicalDeviceId = _queryParam.openId;
            physicalSensorIDs = _queryParam.physicalSensorIDs;
        }
        else
        {
            // Find if sensor exist in logical multicam
            const int DEVICE_COUNT = pHalDeviceList->queryNumberOfDevices();
            for(auto id = SENSOR_COUNT; id < DEVICE_COUNT; ++id)
            {
                physicalSensorIDs = pHalDeviceList->getSensorId(id);
                if(physicalSensorIDs.size() == 2 &&
                   (_queryParam.openId == physicalSensorIDs[0] ||
                    _queryParam.openId == physicalSensorIDs[1]))
                {
                    logicalDeviceId = id;
                    break;
                }
            }
        }

        if(logicalDeviceId < SENSOR_COUNT)
        {
            MY_LOGI("No calibation data for sensor %d", _queryParam.openId);
            return false;
        }
        else
        {
            int rotation = StereoSettingProvider::getModuleRotation(logicalDeviceId);
            if(rotation == 90 ||
               rotation == 270)
            {
                result->lensPoseRotation[2] = 1.0f;
            }

            //NOTICE: This removes the possibility of sensor index exchanging
            if(_queryParam.openId == logicalDeviceId ||     // Logical multicam itself
               _queryParam.openId == physicalSensorIDs[0])  // Main cam of the logical multicam
            {
                result->lensPoseTranslation = {0, 0, 0};
            }
            else
            {
                float baseline = StereoSettingProvider::getStereoBaseline(logicalDeviceId)/100.0f;  //in meter
                int   position = StereoSettingProvider::getSensorRelativePosition(logicalDeviceId);
                if(1 == position)
                {
                    baseline = -baseline;
                }

                switch(rotation)
                {
                    case 0:
                        result->lensPoseTranslation = {baseline, 0, 0};
                    break;
                    case 90:
                        result->lensPoseTranslation = {0, -baseline, 0};
                    break;
                    case 270:
                        // Front cam
                        result->lensPoseTranslation = {0, baseline, 0};
                    break;
                    case 180:
                    default:
                        MY_LOGW("This shuold not happen");
                }
            }

            int queryId = (_queryParam.openId == logicalDeviceId) ? physicalSensorIDs[0] : _queryParam.openId;
            float fovHorizontal, fovVertical;
            pStaticInfo = pHalDeviceList->querySensorStaticInfo(queryId);
            if(pStaticInfo)
            {
                if(!StereoSettingProvider::getSensorFOV(queryId, fovHorizontal, fovVertical))
                {
                    fovHorizontal = pStaticInfo->horizontalViewAngle;
                    fovVertical   = pStaticInfo->verticalViewAngle;
                    MY_LOGD("Cannot get FOV of sensor %d from setting, use sensor static info(%f, %f)",
                            queryId, fovHorizontal, fovVertical);
                }

                if(fovHorizontal >= 0.0f)
                {
                    float w = pStaticInfo->captureWidth;
                    float h = pStaticInfo->captureHeight;
                    float fx = w/(2*tan(StereoHAL::degreeToRadians(fovHorizontal/2.0f)));
                    float fy = h/(2*tan(StereoHAL::degreeToRadians(fovVertical/2.0f)));
                    result->lensIntrinsicCalibration = {fx, fy, w/2.0f, h/2.0f, 0};
                }
                else
                {
                    MY_LOGE("Invalid FOV: %f", fovHorizontal);
                }
            }
            else
            {
                MY_LOGE("Cannot get sensor static info of sernsor %d", queryId);
            }
        }
#endif

        if(result->lensIntrinsicCalibration.size() == 0)
        {
            result->lensIntrinsicCalibration = {0, 0, 0, 0, 0};
        }

        result->lensDistortion = {0, 0, 0, 0, 0};
    }

    return true;
}

/******************************************************************************
 *
 ******************************************************************************/
bool
CalibrationProvider::
__loadMTKCalibration(CalibrationResultInMTKFormat *result) const
{
    bool isSucceed = ( __loadMTKCalibrationFromFile(result) ||
                       __loadMTKCalibrationFrom3rdParty(result) ||
                       __loadMTKCalibrationFromEEPROM(result) ||
                       __loadMTKCalibrationFromSetting(result) );

    if(isSucceed &&
       ::property_get_bool(PROPERTY_DUMP_CAL, false))
    {
        FILE *eepromFile = fopen(EXTERN_EEPROM_PATH, "wb+");
        if(eepromFile) {
            fwrite(result->calibration, 1, CAM_CAL_Stereo_Data_SIZE, eepromFile);
            fclose(eepromFile);
            eepromFile = NULL;
        } else {
            MY_LOGE("Cannot open %s", EXTERN_EEPROM_PATH);
        }
    }

    return isSucceed;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
CalibrationProvider::
__loadMTKCalibrationFromFile(CalibrationResultInMTKFormat *result) const
{
    struct stat st;
    if(!stat(EXTERN_EEPROM_PATH, &st) &&
       st.st_size == CAM_CAL_Stereo_Data_SIZE)
    {
        MY_LOGI("Read calibration from %s", EXTERN_EEPROM_PATH);
        FILE *eepromFile = fopen(EXTERN_EEPROM_PATH, "r");
        if(eepromFile) {
            size_t readSize = fread(result->calibration, CAM_CAL_Stereo_Data_SIZE, 1, eepromFile);
            fclose(eepromFile);
            eepromFile = NULL;

            if(readSize < CAM_CAL_Stereo_Data_SIZE) {
                MY_LOGE("Incorrect calibration size, read %zu, expect %d", readSize, CAM_CAL_Stereo_Data_SIZE);
                return false;
            }

            result->source = E_MTK_CALIBRATION_SOURCE_EXTERNAL;
        } else {
            MY_LOGE("Cannot open %s", EXTERN_EEPROM_PATH);
            return false;
        }

        return true;
    }

    return false;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
CalibrationProvider::
__loadMTKCalibrationFrom3rdParty(CalibrationResultInMTKFormat *result) const
{
    if(_3rdPartyConvertor->convertToMTKFormat(*result))
    {
        MY_LOGI("Read calibration from 3rd party");
        result->source = E_MTK_CALIBRATION_SOURCE_3RDPARTY;
        return true;
    }

    return false;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
CalibrationProvider::
__loadMTKCalibrationFromEEPROM(CalibrationResultInMTKFormat *result) const
{
    //1. Get calibration data
    CAM_CAL_DATA_STRUCT calibrationData;
    CamCalDrvBase *pCamCalDrvObj = CamCalDrvBase::createInstance();
    if(pCamCalDrvObj == nullptr)
    {
        MY_LOGE("Cannot get CamCalDrv");
        return false;
    }

    MINT32 err = 0;
    auto pHalDeviceList = MAKE_HalLogicalDeviceList();
    if(pHalDeviceList == nullptr)
    {
        MY_LOGE("Cannot get Logical Device List");
        return false;
    }
    int sensorDevIdx = pHalDeviceList->querySensorDevIdx(_queryParam.physicalSensorIDs[0]);
    err = pCamCalDrvObj->GetCamCalCalData(sensorDevIdx,
                                          CAMERA_CAM_CAL_DATA_STEREO_DATA,
                                          (void *)&calibrationData);

    bool hasCalibration = false;
    if(!err) {
        //check if calibration is valid

        for(auto &c : calibrationData.Stereo_Data.Data) {
            if(c != 0xFF) {
                hasCalibration = true;
                break;
            }
        }

        if(hasCalibration) {
            MY_LOGI("Read calibration from EEPROM");
            ::memcpy(result->calibration, calibrationData.Stereo_Data.Data, CAM_CAL_Stereo_Data_SIZE);
            result->source = E_MTK_CALIBRATION_SOURCE_EEPROM;
        } else {
            MY_LOGW("Invlid stereo calibration data");
        }
    }

    pCamCalDrvObj->destroyInstance();

    return hasCalibration;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
CalibrationProvider::
__loadMTKCalibrationFromSetting(CalibrationResultInMTKFormat *result) const
{
#if (1==HAS_HAL3_FEATURE)
    size_t size = StereoSettingProvider::getCalibrationData(result->calibration);
    if(size > 0) {
        MY_LOGI("Read calibration from setting, size %zu", size);
        result->source = E_MTK_CALIBRATION_SOURCE_SETTING;
        return true;
    }
#endif

    return false;
}