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

#define LOG_TAG "MtkCam/JpegImageCallback"
//
#include <mtkcam/middleware/v1/IShot.h>
//
#include <mtkcam/utils/std/Log.h>
//
#include "JpegImageCallback.h"
//
using namespace android;
using namespace NSCam::v1::NSLegacyPipeline;
/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
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
JpegImageCallback::
JpegImageCallback(
    ImageCallbackParams params
) : IImageShotCallback(params)
{
    MY_LOGD("ctor(0x%x)", this);
}
/******************************************************************************
 *
 ******************************************************************************/
JpegImageCallback::
~JpegImageCallback()
{
    MY_LOGD("dtor(0x%x)", this);
}
/******************************************************************************
 *
 ******************************************************************************/
status_t
JpegImageCallback::
sendCallback(
    sp<IShotCallback> pShotCallback,
    StreamId_T const streamId,
    sp<IImageBuffer>& pBuffer,
    MBOOL bFinalImage
)
{
    status_t ret = UNKNOWN_ERROR;
    pBuffer->lockBuf(LOG_TAG, eBUFFER_USAGE_SW_READ_MASK);
    if(mParams.enableDumpBuffer)
    {
        dumpBuffer(pBuffer);
    }
    if(pShotCallback!=nullptr)
    {
        MUINT32 u4Size = pBuffer->getBitstreamSize();
        uint8_t const* puBuf = (uint8_t const*)pBuffer->getBufVA(0);
        pShotCallback->onCB_CompressedImage_packed(
                                mParams.timeStamp,
                                u4Size,
                                puBuf,
                                mParams.callbackIndex,
                                bFinalImage,
                                mParams.msgType);
        ret = OK;
    }
    pBuffer->unlockBuf(LOG_TAG);
    return ret;
}
/******************************************************************************
 *
 ******************************************************************************/
status_t
JpegImageCallback::
processFailData(
    MUINT32 const reqNo,
    StreamId_T const streamId,
    sp<IImageBuffer>& pBuffer
)
{
    status_t ret = UNKNOWN_ERROR;
    MY_LOGE("reqNo(%d) streamId(0x%x) receive metadata fail",
            reqNo, streamId);
    ret = OK;
    return ret;
}
/******************************************************************************
 *
 ******************************************************************************/
status_t
JpegImageCallback::
dumpBuffer(
    sp<IImageBuffer>& pBuffer
)
{
    
    MY_LOGD("dump");
    MSize size = pBuffer->getImgSize();
    std::string saveFileName = mParams.filePath +
                              std::string("/")+
                              mParams.streamName+
                              mParams.fileExtension;
    pBuffer->saveToFile(saveFileName.c_str());
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MUINT32
JpegImageCallback::
getStreamId()
{
    return mParams.streamId;
}
/******************************************************************************
 *
 ******************************************************************************/
MBOOL
JpegImageCallback::
isNeedImageCount()
{
    return mParams.mbNeedToCountForCapture;
}
/******************************************************************************
 *
 ******************************************************************************/
MBOOL
JpegImageCallback::
setDumpInfo(
    MBOOL enable,
    string filePath
)
{
    MY_LOGD("%d %s", enable, filePath.c_str());
    mParams.enableDumpBuffer = enable;
    mParams.filePath = filePath;
    return MTRUE;
}