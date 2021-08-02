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
#ifndef _MTK_PLATFORM_HARDWARE_MTKCAM_V3_TEMPLATE_REQUEST_CUSTOM_INFO_H_
#define _MTK_PLATFORM_HARDWARE_MTKCAM_V3_TEMPLATE_REQUEST_CUSTOM_INFO_H_
//
#include <utils/String8.h>
#include <mtkcam/drv/IHalSensor.h>


/******************************************************************************
 *
 ******************************************************************************/
namespace android {
namespace NSTemplateRequest {


/******************************************************************************
 *
 ******************************************************************************/
void
showCustInfo();


/******************************************************************************
 *  Info
 ******************************************************************************/
class Info
{
protected:  ////    Data Members.
    int32_t         mSensorIndex;
    int32_t         mSensorType;
    int32_t         mRawFmtType;
    char const*     mSensorDrvName;

public:     ////    Instantiation.
    Info()
        : mSensorIndex(0)
        , mSensorType(0)
        , mRawFmtType(0)
        , mSensorDrvName(nullptr)
    {}
    //
    Info(
        uint_t const    uSensorIndex,
        uint_t const    eSensorType,
        uint_t const    eRawFmtType,
        char const*     szSensorDrvName
    )
        : mSensorIndex(uSensorIndex)
        , mSensorType(eSensorType)
        , mRawFmtType(eRawFmtType)
        , mSensorDrvName(szSensorDrvName)
    {
    }

public:     ////    Accessors.
    int32_t         getDeviceId()      const   { return mSensorIndex; }
    int32_t         getSensorType()    const   { return mSensorType; }
    bool            isMonoSensor()      const   { return (mRawFmtType == NSCam::SENSOR_RAW_MONO)? true : false; }
    char const*     getSensorDrvName() const   { return mSensorDrvName; }
};


/******************************************************************************
 *  Request Metadata
 ******************************************************************************/
#define REQUEST_METADATA_BEGIN2(PREFIX,SENSORNAME) \
    extern "C" \
    status_t \
    constructCustRequestMetadata_##PREFIX##_##SENSORNAME( \
        NSCam::IMetadata&       rMetadata, \
        int const               requestType, \
        Info const&             rInfo \
    ) \
    {

#define REQUEST_METADATA_END() \
        return  OK; \
    }

#define REQUEST_METADATA_BEGIN(SENSORNAME) \
    extern "C" \
    status_t \
    constructCustRequestMetadata_DEVICE_##SENSORNAME( \
        NSCam::IMetadata&       rMetadata, \
        int const               requestType \
    ) \
    {

#define REQUEST_METADATA_END() \
        return  OK; \
    }
#define PREFIX_FUNCTION_REQUEST_METADATA "constructCustRequestMetadata"

/******************************************************************************
 *
 ******************************************************************************/
};  // namespace NSTemplateRequest
};  // namespace android
#endif  //_MTK_PLATFORM_HARDWARE_MTKCAM_V3_TEMPLATE_REQUEST_CUSTOM_INFO_H_
