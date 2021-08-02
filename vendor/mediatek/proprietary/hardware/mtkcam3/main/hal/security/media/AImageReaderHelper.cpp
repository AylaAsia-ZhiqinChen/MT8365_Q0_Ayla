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

#define LOG_TAG "AImageReaderHelper"

#include "AImageReaderHelper.h"

#include <cmath>

#include <mtkcam/utils/std/ULog.h>

#include <sync/sync.h>

#include <vndk/hardware_buffer.h>

#include <vendor/mediatek/hardware/gpu/1.0/IGraphicExt.h>

// ------------------------------------------------------------------------

namespace NSCam {

CAM_ULOG_DECLARE_MODULE_ID(MOD_CAMERA_HAL_SERVER);

using ::android::sp;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_handle;
using ::android::hardware::graphics::common::V1_0::PixelFormat;
using ::vendor::mediatek::hardware::gpu::V1_0::IGraphicExt;
using ::vendor::mediatek::hardware::gpu::V1_0::GRALLOC_EXTRA_ATTRIBUTE_QUERY;
using ::vendor::mediatek::hardware::gpu::V1_0::GRALLOC_EXTRA_RESULT;

// ------------------------------------------------------------------------

int AImageReaderHelper::initAImageReader()
{
    if (mImageReader != nullptr || mImageReaderAnw != nullptr)
    {
        CAM_ULOGME("image reader has been initialized: mImageReader(0x%" PRIxPTR \
                ") mImageReaderAnw(0x%" PRIxPTR ")",
                reinterpret_cast<intptr_t>(mImageReader),
                reinterpret_cast<intptr_t>(mImageReaderAnw));
        return AMEDIA_ERROR_INVALID_OPERATION;
    }

    // [Consumer]
    // AImageReader constructor with an additional parameter for the consumer usage.
    auto ret = AImageReader_newWithUsage(mWidth, mHeight, mFormat, mUsage,
            mMaxImages, &mImageReader);
    if (ret != AMEDIA_OK || mImageReader == nullptr)
    {
        CAM_ULOGME("create image reader failed: ret(%d) mImageReader(0x%" PRIxPTR \
                ")", ret, reinterpret_cast<intptr_t>(mImageReader));
        return ret;
    }

    // [Consumer]
    // Get the native_handle_t corresponding to the ANativeWindow owned by the
    // AImageReader provided.
    ret = AImageReader_getWindowNativeHandle(mImageReader, &mImageReaderAnw);
    if (ret != AMEDIA_OK || mImageReaderAnw == nullptr)
    {
        CAM_ULOGME("get native handle owned by image reader failed: ret(%d) " \
                "mImageReader(0x%" PRIxPTR ") mImageReaderAnw(0x%" PRIxPTR ")",
                ret, reinterpret_cast<intptr_t>(mImageReader),
                reinterpret_cast<intptr_t>(mImageReaderAnw));
        return ret;
    }

    // Set the AImageReader_ImageListener::onImageAvailable() listener of
    // this image reader.
    // Calling this method will replace previously registered listeners.
    ret = AImageReader_setImageListener(
            mImageReader, &mImageReaderImageAvailableListener);
    if (ret != AMEDIA_OK)
    {
        CAM_ULOGME("set ImageListener failed: ret(%d) " \
                "mImageReader(0x%" PRIxPTR ")", ret,
                reinterpret_cast<intptr_t>(mImageReader));
        return ret;
    }

    // Set the AImageReader_BufferRemovedListener::onBufferRemoved() of
    // this image reader.
    //
    // This callback is called when an old AHardwareBuffer is about
    // to be removed from the image reader.
    //
    // Note that registering this callback is optional unless the user
    // holds on extra reference to AHardwareBuffer returned from
    // AImage_getHardwareBuffer by calling AHardwareBuffer_acquire or
    // creating external graphic objects, such as EglImage, from it.
    ret = AImageReader_setBufferRemovedListener(
            mImageReader, &mImageReaderBufferRemovedListener);
    if (ret != AMEDIA_OK)
    {
        CAM_ULOGME("set BufferRemovedListener failed: ret(%d) " \
                "mImageReader(0x%" PRIxPTR ")", ret,
                reinterpret_cast<intptr_t>(mImageReader));
        return ret;
    }

    return ret;
}

size_t AImageReaderHelper::getAcquiredImageCount()
{
    // return 0 if the image callback is available
    if (mImageAvailable_cb)
        return 0;

    // the default behavior is to return the amount of acquired images
    std::lock_guard<std::mutex> l(mAcquiredImageCount.second);
    return mAcquiredImageCount.first;
}

int32_t AImageReaderHelper::getWidth() const
{
    int32_t width;
    media_status_t status = AImageReader_getWidth(mImageReader, &width);
    if (status != AMEDIA_OK)
        CAM_ULOGME("get width failed");
    return width;
}

int32_t AImageReaderHelper::getHeight() const
{
    int32_t height;
    media_status_t status = AImageReader_getHeight(mImageReader, &height);
    if (status != AMEDIA_OK)
        CAM_ULOGME("get height failed");
    return height;
}

int32_t AImageReaderHelper::getFormat() const
{
    int32_t format;
    media_status_t status = AImageReader_getFormat(mImageReader, &format);
    if (status != AMEDIA_OK)
        CAM_ULOGME("get format failed");
    return format;
}

sp<AImageReaderHelper::HGraphicBufferProducer> AImageReaderHelper::getHGBPFromHandle()
{
    const native_handle_t *handle(mImageReaderAnw);
    if (handle == nullptr)
        return nullptr;

    if (handle->numFds != 0  ||
            handle->numInts < std::ceil(sizeof(size_t) / sizeof(int)))
        return nullptr;

    return convertNativeHandleToHGBP(handle);
}

bool AImageReaderHelper::getSecureHandle(
        const hidl_handle& bufferHandle, SECHAND& secureHandle)
{
    // get IGraphicExt service
    sp<IGraphicExt> graphicExt = IGraphicExt::getService();
    if (!graphicExt)
    {
        CAM_ULOGME("failed to get IGraphicExt interface");
        return false;
    }

    // Reference: vendor/mediatek/proprietary/hardware/gpu_hidl/service/GraphicExt.cpp
    graphicExt->gralloc_extra_query(
            hidl_handle(bufferHandle),
            GRALLOC_EXTRA_ATTRIBUTE_QUERY::GRALLOC_EXTRA_GET_SECURE_HANDLE,
            [&secureHandle](auto retval, const auto& dataHandle)
            {
                auto ret = GRALLOC_EXTRA_RESULT(retval);
                if (ret != GRALLOC_EXTRA_RESULT::GRALLOC_EXTRA_OK)
                {
                    CAM_ULOGME("gralloc_extra_query failed: %s",
                    toString(ret).c_str());
                    return;
                }

                // NOTE: the gralloc_extra implementation place the secure handle into
                // the address of &dataHandle->data[0]
                secureHandle = *reinterpret_cast<const SECHAND *>(dataHandle->data);
            });

    return true;
}

bool AImageReaderHelper::getPixelFormat(
        const hidl_handle& bufferHandle, PixelFormat& pixelFormat)
{
    // get IGraphicExt service
    sp<IGraphicExt> graphicExt = IGraphicExt::getService();
    if (!graphicExt)
    {
        CAM_ULOGME("failed to get IGraphicExt interface");
        return false;
    }

    // Reference: vendor/mediatek/proprietary/hardware/gpu_hidl/service/GraphicExt.cpp
    graphicExt->gralloc_extra_query(
            hidl_handle(bufferHandle),
            GRALLOC_EXTRA_ATTRIBUTE_QUERY::GRALLOC_EXTRA_GET_FORMAT,
            [&pixelFormat](auto retval, const auto& dataHandle)
            {
                auto ret = GRALLOC_EXTRA_RESULT(retval);
                if (ret != GRALLOC_EXTRA_RESULT::GRALLOC_EXTRA_OK)
                {
                    CAM_ULOGME("gralloc_extra_query failed: %s",
                    toString(ret).c_str());
                    return;
                }

                // NOTE: the gralloc_extra implementation place the format into
                // the address of &dataHandle->data[0]
                pixelFormat = *reinterpret_cast<const PixelFormat*>(
                    dataHandle->data);
            });

    return true;
}

// Reference: frameworks/av/media/utils/AImageReaderUtils.cpp
// AOSP design: Retrieves HGraphicBufferProducer corresponding to
// the native_handle_t provided (this native handle MUST have been obtained
// by AImageReader_getWindowNativeHandle()).
sp<AImageReaderHelper::HGraphicBufferProducer> AImageReaderHelper::convertNativeHandleToHGBP(
        const native_handle_t* handle)
{
    // Read the size of the halToken vec<uint8_t>
    hidl_vec<uint8_t> halToken;
    halToken.setToExternal(
        reinterpret_cast<uint8_t *>(const_cast<int *>(&(handle->data[1]))),
        handle->data[0]);
    sp<HGraphicBufferProducer> hgbp =
        HGraphicBufferProducer::castFrom(
                ::android::retrieveHalInterface(halToken));
    return hgbp;
}

void AImageReaderHelper::setImageCallback(ImageAvailable_cb cb)
{
    mImageAvailable_cb = cb;
}

void AImageReaderHelper::handleImageAvailable(AImageReader* reader)
{
    // delegate to the user-defined callback
    if (mImageAvailable_cb)
    {
        mImageAvailable_cb(reader);
        return;
    }

    // NOTE: The default behavior is to count the acquired images

    // make sure AImage will be deleted automatically when it goes out of
    // scope.
    auto imageDeleter = [this](AImage* img)
    {
        if (mAsync)
            // AImage_deleteAsync() with kNoFenceFd is equivalent to AImage_delete()
            AImage_deleteAsync(img, kNoFenceFd);
        else
            AImage_delete(img);
    };
    std::unique_ptr<AImage, decltype(imageDeleter)> outImage(nullptr, imageDeleter);

    // default operation if no registered callback
    AImage *image = nullptr;
    int acquireFenceFd = kNoFenceFd;
    media_status_t status = [&]
    {
        return mAsync ?
            AImageReader_acquireNextImageAsync(
                    reader, &image, &acquireFenceFd) :
            AImageReader_acquireNextImage(reader, &image);
    }();

    if (status != AMEDIA_OK || image == nullptr)
    {
        CAM_ULOGME("acquire the next image failed: status(%d) image(0x%" PRIxPTR ")",
                status, reinterpret_cast<intptr_t>(image));
        return;
    }

    if (acquireFenceFd != kNoFenceFd)
    {
        // wait fence (if exist) for 1000 ms
        int err = sync_wait(acquireFenceFd, 1000);

        // the image must be sent back the the system and
        // delete the AImage object from memory even if the failure of sync_wait()
        if (err < 0)
            CAM_ULOGMW("sync_wait() failed: %s", strerror(errno));
    }

    outImage.reset(image);

    // get the hardware buffer handle of the input image
    // intended for GPU and/or hardware access
    AHardwareBuffer *outBuffer = nullptr;
    status = AImage_getHardwareBuffer(outImage.get(), &outBuffer);
    if (status != AMEDIA_OK || outBuffer == nullptr)
    {
        CAM_ULOGME("get hardware buffer failed: status(%d) outBuffer(0x%" PRIxPTR ")",
                status, reinterpret_cast<intptr_t>(outBuffer));
        return;
    }

    // get a description of the AHardwareBuffer
    AHardwareBuffer_Desc outDesc;
    AHardwareBuffer_describe(outBuffer, &outDesc);

    const native_handle_t *outBufferHandle = AHardwareBuffer_getNativeHandle(outBuffer);
    if (outBufferHandle == nullptr)
    {
        CAM_ULOGME("get native handle failed");
        return;
    }

    if ((outDesc.usage & AHARDWAREBUFFER_USAGE_PROTECTED_CONTENT) ==
            AHARDWAREBUFFER_USAGE_PROTECTED_CONTENT)
    {
        // get the secure handle of a buffer handle
        SECHAND secureHandle;
        if (getSecureHandle(hidl_handle(outBufferHandle), secureHandle))
            CAM_ULOGMD("secure handle(0x%x)", secureHandle);
        else
            CAM_ULOGME("get secure handle failed");
    }

    // TODO: add possible buffer checks
    std::lock_guard<std::mutex> l(mAcquiredImageCount.second);
    mAcquiredImageCount.first++;
}

} // namespace NSCam
