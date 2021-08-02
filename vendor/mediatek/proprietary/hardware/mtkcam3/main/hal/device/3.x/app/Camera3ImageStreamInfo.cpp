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

#include "Camera3ImageStreamInfo.h"
#include "MyUtils.h"
//
using namespace android;
using namespace NSCam;
using namespace NSCam::v3;


/******************************************************************************
 *
 ******************************************************************************/
CAM_ULOG_DECLARE_MODULE_ID(MOD_CAMERA_DEVICE);

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


/******************************************************************************
 *
 ******************************************************************************/
static MUINT32 mapRotationToTransform(StreamRotation rotation)
{
    switch (rotation)
    {
    case StreamRotation::ROTATION_270:
        return HAL_TRANSFORM_ROT_90;

    case StreamRotation::ROTATION_180:
        return HAL_TRANSFORM_ROT_180;

    case StreamRotation::ROTATION_90:
        return HAL_TRANSFORM_ROT_270;

    case StreamRotation::ROTATION_0:
    default:
        return 0;
    }
    return 0;
}


/******************************************************************************
 *
 ******************************************************************************/
Camera3ImageStreamInfo::
Camera3ImageStreamInfo(CreationInfo const& info)
    : IImageStreamInfo()
    , mInfo(info)
    , mSensorId(info.mSensorId)
    , mStreamId(info.mStream.v3_2.id)
    , mTransform(mapRotationToTransform(info.mStream.v3_2.rotation))
    , mHalUsage(info.mStream.v3_2.streamType==StreamType::OUTPUT ? info.mHalStream.v3_3.v3_2.producerUsage : info.mHalStream.v3_3.v3_2.consumerUsage)
    , mConsumerUsage(info.mStream.v3_2.streamType==StreamType::OUTPUT ? info.mStream.v3_2.usage : mHalUsage)
{
}


/******************************************************************************
 *
 ******************************************************************************/
V3_4::Stream const&
Camera3ImageStreamInfo::
getStream() const
{
    return mInfo.mStream;
}


/******************************************************************************
 *
 ******************************************************************************/
MSize
Camera3ImageStreamInfo::
getLandscapeSize() const
{
    if ( !(getImgSize().w >= getImgSize().h) ){
        MSize ret;
        ret.w = getImgSize().h;
        ret.h = getImgSize().w;
        return ret;
    }
    return getImgSize();
}


/******************************************************************************
 *
 ******************************************************************************/
MUINT64
Camera3ImageStreamInfo::
getUsageForConsumer() const
{
    return mConsumerUsage;
}


/******************************************************************************
 *
 ******************************************************************************/
MUINT64
Camera3ImageStreamInfo::
getUsageForAllocator() const
{
    // bitwise combination of Hal usage and Hal's client usage.
    return (mHalUsage | mInfo.mStream.v3_2.usage);
}


/******************************************************************************
 *
 ******************************************************************************/
MINT
Camera3ImageStreamInfo::
getImgFormat() const
{
    return mInfo.mImageBufferInfo.imgFormat;
}


/******************************************************************************
 *
 ******************************************************************************/
MINT
Camera3ImageStreamInfo::
getOriImgFormat() const
{
    return mInfo.mOriImgFormat;
}


/******************************************************************************
 *
 ******************************************************************************/
MSize
Camera3ImageStreamInfo::
getImgSize() const
{
    return MSize(mInfo.mImageBufferInfo.imgWidth, mInfo.mImageBufferInfo.imgHeight);
}


/******************************************************************************
 *
 ******************************************************************************/
Camera3ImageStreamInfo::BufPlanes_t const&
Camera3ImageStreamInfo::
getBufPlanes() const
{
    return mInfo.mImageBufferInfo.bufPlanes;
}


/******************************************************************************
 *
 ******************************************************************************/
MUINT32
Camera3ImageStreamInfo::
getTransform() const
{
    return mTransform;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
Camera3ImageStreamInfo::
setTransform(MUINT32 transform)
{
    mTransform = transform;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MUINT32
Camera3ImageStreamInfo::
getDataSpace() const
{
    return mInfo.mStream.v3_2.dataSpace;
}

/******************************************************************************
 *
 ******************************************************************************/
MBOOL
Camera3ImageStreamInfo::
isSecure() const
{
    return mInfo.mSecureInfo.type != SecType::mem_normal;
}

/******************************************************************************
 *
 ******************************************************************************/
SecureInfo const&
Camera3ImageStreamInfo::
getSecureInfo() const
{
    return mInfo.mSecureInfo;
}

/******************************************************************************
 *
 ******************************************************************************/
char const*
Camera3ImageStreamInfo::
getStreamName() const
{
    return mInfo.mStreamName.string();
}


/******************************************************************************
 *
 ******************************************************************************/
StreamId_T
Camera3ImageStreamInfo::
getStreamId() const
{
    return mStreamId;
}


/******************************************************************************
 *
 ******************************************************************************/
MUINT32
Camera3ImageStreamInfo::
getStreamType() const
{
    return static_cast<MUINT32>(mInfo.mStream.v3_2.streamType);
}


/******************************************************************************
 *
 ******************************************************************************/
size_t
Camera3ImageStreamInfo::
getMaxBufNum() const
{
    return mInfo.mHalStream.v3_3.v3_2.maxBuffers;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
Camera3ImageStreamInfo::
setMaxBufNum(size_t count)
{
    mInfo.mHalStream.v3_3.v3_2.maxBuffers = count;
}


/******************************************************************************
 *
 ******************************************************************************/
size_t
Camera3ImageStreamInfo::
getMinInitBufNum() const
{
    return 0;
}


/******************************************************************************
 *
 ******************************************************************************/
android::String8
Camera3ImageStreamInfo::
toString() const
{
    String8 s8Planes;
    auto const& bufPlanes = getBufPlanes();
    for (size_t i = 0; i < bufPlanes.count; i++) {
        auto const& plane = bufPlanes.planes[i];
        s8Planes += String8::format(" %zu/%zu", plane.rowStrideInBytes, plane.sizeInBytes);
    }

    String8 s8RealFormat, s8RequestFormat, s8OverrideFormat;
    String8 s8HalUsage, s8HalClientUsage;
    String8 s8Dataspace;
    String8 s8Offset = String8::format(" %zu", getImageBufferInfo().bufStep);
    if ( auto pGrallocHelper = IGrallocHelper::singleton() ) {
        s8RealFormat    = pGrallocHelper->queryPixelFormatName(mInfo.mImageBufferInfo.imgFormat);
        s8RequestFormat = pGrallocHelper->queryPixelFormatName((int)mInfo.mStream.v3_2.format);
        s8OverrideFormat= pGrallocHelper->queryPixelFormatName((int)mInfo.mHalStream.v3_3.v3_2.overrideFormat);
        s8HalUsage      = pGrallocHelper->queryGrallocUsageName(mHalUsage);
        s8HalClientUsage= pGrallocHelper->queryGrallocUsageName(mInfo.mStream.v3_2.usage);
        s8Dataspace     = pGrallocHelper->queryDataspaceName(mInfo.mStream.v3_2.dataSpace);
    }

    android::String8 os = android::String8::format(
        "%02" PRIu64 " %4dx%-4d %s "
        "t:%d/r:%d maxBuffers:%d d/s:0x%08x(%s) "
        "Hal-Client-usage:%#" PRIx64 "(%s) "
        "Real:%#x(%s) Request:%#x(%s) Override:%#x(%s) "
        "Hal-usage:%#" PRIx64 "(%s) HalStream::(consumer/producer)Usage:%#" PRIx64 "/%#" PRIx64 " "
        "bufStep: %s rowStrideInBytes/sizeInBytes:%s %s %p phy:%d",
        mStreamId, mInfo.mImageBufferInfo.imgWidth, mInfo.mImageBufferInfo.imgHeight, (StreamType::OUTPUT==mInfo.mStream.v3_2.streamType ? "OUT" : "IN "),
        mTransform, mInfo.mStream.v3_2.rotation, mInfo.mHalStream.v3_3.v3_2.maxBuffers,
        mInfo.mStream.v3_2.dataSpace, s8Dataspace.c_str(),
        mInfo.mStream.v3_2.usage, s8HalClientUsage.c_str(),                  //Hal-Client usage
        mInfo.mImgFormat, s8RealFormat.c_str(),                         //real format
        mInfo.mStream.v3_2.format, s8RequestFormat.c_str(),                  //request format
        mInfo.mHalStream.v3_3.v3_2.overrideFormat, s8OverrideFormat.c_str(),      //override format
        mHalUsage, s8HalUsage.c_str(),                                  //Hal-appended usage
        mInfo.mHalStream.v3_3.v3_2.consumerUsage, mInfo.mHalStream.v3_3.v3_2.producerUsage, //consumer/producer usages
        s8Offset.c_str(), s8Planes.c_str(), mInfo.mStreamName.string(), this, mSensorId
    );
    return os;
}


/******************************************************************************
 *
 ******************************************************************************/
void
Camera3ImageStreamInfo::
dumpState(android::Printer& printer, uint32_t indent) const
{
    printer.printFormatLine("%-*c%s", indent, ' ', toString().c_str());
}


/******************************************************************************
 *
 ******************************************************************************/
MINT
Camera3ImageStreamInfo::
getPhysicalCameraId() const
{
   return mSensorId;
}

/******************************************************************************
 *
 ******************************************************************************/
MINT
Camera3ImageStreamInfo::
getAllocImgFormat() const
{
    return mInfo.mImgFormat;
}


/******************************************************************************
 *
 ******************************************************************************/
IImageStreamInfo::BufPlanes_t const&
Camera3ImageStreamInfo::
getAllocBufPlanes() const
{
    return mInfo.mvbufPlanes;
}


/******************************************************************************
 *
 ******************************************************************************/
ImageBufferInfo const&
Camera3ImageStreamInfo::
getImageBufferInfo() const
{
    return mInfo.mImageBufferInfo;
}
