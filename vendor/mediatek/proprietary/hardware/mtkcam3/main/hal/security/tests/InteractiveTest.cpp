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

#define LOG_TAG "securecamera_test"

#include "InteractiveTest.h"

#include "utils/TestBase.h"
#include "media/AImageReaderHelper.h"
#include "camera/ACameraHelper.h"

#include <mtkcam/utils/std/ULog.h>

#include <camera/NdkCameraManager.h>

#include <VendorTagDescriptor.h>

// ------------------------------------------------------------------------

namespace NSCam {
namespace tests {

CAM_ULOG_DECLARE_MODULE_ID(MOD_CAMERA_HAL_SERVER);

using android::hardware::camera::common::V1_0::helper::VendorTagDescriptorCache;
using TestDefs::ReaderUsageAndAFormat;

// ------------------------------------------------------------------------

InteractiveTest::InteractiveTest()
{
    // create ACameraManager instance
    //
    // The ACameraManager is responsible for detecting, characterizing,
    // and connecting to ACameraDevice.
    //
    // The caller must call ACameraManager_delete to free the resources once
    // it is done using the ACameraManager instance.
    mCameraManager = ACameraManager_create();
    if (mCameraManager == nullptr)
    {
        CAM_ULOGME("create ACameraManager failed");
        return;
    }

    // create a list of currently connected camera devices, including
    // cameras that may be in use by other camera API clients.
    //
    // Non-removable cameras use integers starting at 0 for their
    // identifiers, while removable cameras have a unique identifier for each
    // individual device, even if they are the same model.
    //
    // ACameraManager_getCameraIdList will allocate and return an  ACameraIdList.
    // The caller must call ACameraManager_deleteCameraIdList to free the memory.
    camera_status_t ret = ACameraManager_getCameraIdList(
            mCameraManager, &mCameraIdList);
    if (ret != ACAMERA_OK)
    {
        CAM_ULOGME("get cameraIdList failed: ret(%d)", ret);
        return;
    }

    CAM_ULOGM_ASSERT(VendorTagDescriptorCache::getGlobalVendorTagCache(),
            "get global vendor tag cache failed");
    if (mCameraIdList->numCameras < 1)
    {
        CAM_ULOGMW("Device has no camera on board");
        return;
    }
}

InteractiveTest::~InteractiveTest()
{
    // delete a list of camera devices allocated via ACameraManager_getCameraIdList.
    if (mCameraIdList)
    {
        ACameraManager_deleteCameraIdList(mCameraIdList);
        mCameraIdList = nullptr;
    }

    // delete the ACameraManager instance and free its resources
    if (mCameraManager)
    {
        ACameraManager_delete(mCameraManager);
        mCameraManager = nullptr;
    }
}

int InteractiveTest::run()
{
    for (int i = 0; i < mCameraIdList->numCameras; i++)
    {
        const char* cameraId(mCameraIdList->cameraIds[i]);
        CAM_ULOGM_ASSERT(cameraId != nullptr, "invalid camera ID");

        ACameraMetadata* staticMetadata = nullptr;

        // query the capabilities of a camera device.
        // These capabilities are immutable for a given camera.
        //
        // The caller must call ACameraMetadata_free to free the memory of
        // the output characteristics
        camera_status_t ret = ACameraManager_getCameraCharacteristics(
                mCameraManager, cameraId, &staticMetadata);
        CAM_ULOGM_ASSERT(ret == ACAMERA_OK,
                "get camera characteristics failed(%d)", ret);
        CAM_ULOGM_ASSERT(staticMetadata != nullptr, "invalid static metadata");

        // get the camera lens facing
        uint8_t facing;
        CAM_ULOGM_ASSERT(getLensFacing(*staticMetadata, facing),
                "get lens facing failed");

        if (TestDefs::kEnableSecureImageDataTest) {
            bool isSecureImage = isCapabilitySupported(*staticMetadata,
                    ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_SECURE_IMAGE_DATA);

            CAM_ULOGMD("camera id(%s) isSupportSecureImage(%d) facing(%u)",
                    cameraId, isSecureImage, facing);

            // free a ACameraMetadata structure
            ACameraMetadata_free(staticMetadata);

            if (!isSecureImage)
            {
                CAM_ULOGMW("Camera does not support SECURE_IMAGE_DATA");
                continue;
            }
        } else {
            // we always use the front-facing camera
            if (facing != ACAMERA_LENS_FACING_FRONT)
                continue;

            bool isBackwardCompatible = isCapabilitySupported(*staticMetadata,
                    ACAMERA_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE);

            CAM_ULOGMD("camera id(%s) isSupportBackwardCompatible(%d) facing(%u)",
                    cameraId, isBackwardCompatible, facing);

            // free a ACameraMetadata structure
            ACameraMetadata_free(staticMetadata);

            if (!isBackwardCompatible)
            {
                CAM_ULOGMW("Camera does not support BACKWARD_COMPATIBLE");
                continue;
            }
        }

        processCaptureRequests(cameraId);
    }

    return EXIT_SUCCESS;
}

bool InteractiveTest::isCapabilitySupported(
        const ACameraMetadata& staticInfo,
        acamera_metadata_enum_android_request_available_capabilities_t cap)
{
    ACameraMetadata_const_entry entry;
    ACameraMetadata_getConstEntry(
            &staticInfo, ACAMERA_REQUEST_AVAILABLE_CAPABILITIES, &entry);
    if (entry.count == 0)
    {
        CAM_ULOGME("cannot find ACAMERA_REQUEST_AVAILABLE_CAPABILITIES in static metadata");
        return false;
    }

    for (uint32_t i = 0; i < entry.count; i++)
    {
        if (entry.data.u8[i] == cap)
            return true;
    }

    return false;
}

bool InteractiveTest::getLensFacing(const ACameraMetadata& staticInfo, uint8_t& facing)
{
    ACameraMetadata_const_entry entry;
    ACameraMetadata_getConstEntry(&staticInfo, ACAMERA_LENS_FACING, &entry);
    if (entry.count == 0)
    {
        CAM_ULOGME("cannot find ACAMERA_LENS_FACING in static metadata");
        return false;
    }

    facing = entry.data.u8[0];
    return true;
}

bool InteractiveTest::processCaptureRequests(const char* id)
{
    auto navigator = [](std::string message)
    {
        printf("Press ENTER to %s...\n", message.c_str());
        if (fflush(stdout))
            printf("end of flush: error indicator(%d)", ferror(stdout));
        (void)getchar();
    };

    for (auto& readerUsageAndFormat : TestDefs::kReaderUsageAndFormat)
    {
        if (TestDefs::kEnableSecureImageDataTest &&
           (readerUsageAndFormat.first != AHARDWAREBUFFER_USAGE_PROTECTED_CONTENT))
        {
            CAM_ULOGMW("SECURE_IMAGE_DATA_TEST is enabled: "
                    "skip usage other than PROTECTED_CONTENT");
            continue;
        }

        if (!TestDefs::kEnableSecureImageDataTest &&
           (readerUsageAndFormat.first == AHARDWAREBUFFER_USAGE_PROTECTED_CONTENT))
        {
            CAM_ULOGMW("SECURE_IMAGE_DATA_TEST is disabled: "
                    "skip usage PROTECTED_CONTENT");
            continue;
        }

        navigator("start capture");

        // start capture
        startProcessCaptureRequest(id, readerUsageAndFormat,
                TestDefs::kMaxAImagesStreaming, false, TestDefs::kRequestCount);

        navigator("stop capture");

        getAcquiredImageCount();

        // stop capture
        stopProcessCaptureRequest();
    }

    return true;
}

bool InteractiveTest::startProcessCaptureRequest(
        const char* id, const ReaderUsageAndAFormat& readerUsageAndFormat,
        int32_t readerMaxImages, bool readerAsync, uint32_t requestCount)
{
    (void) requestCount;
    mReaderHelper.reset(new AImageReaderHelper(
            TestDefs::kAImageWidth, TestDefs::kAImageHeight,
            readerUsageAndFormat.second, readerUsageAndFormat.first,
            readerMaxImages, readerAsync));
    int ret = mReaderHelper->initAImageReader();
    if (ret != AMEDIA_OK)
    {
        CAM_ULOGME("init image reader helper failed: ret(%d)", ret);
        return false;
    }

    mCameraHelper.reset(new ACameraHelper(id, mCameraManager));
    camera_status_t status =
        mCameraHelper->openCamera(mReaderHelper->getNativeWindow(),
                {}/*physicalImgReaders*/, false/*usePhysicalSettings*/, TEMPLATE_RECORD);
    if (status != ACAMERA_OK)
    {
        CAM_ULOGME("open camera failed: ret(%d)", status);
        return false;
    }

    if (!mCameraHelper->isCameraReady())
    {
        CAM_ULOGME("open camera failed");
        return false;
    }

    return true;
}

bool InteractiveTest::stopProcessCaptureRequest()
{
    mCameraHelper.reset(nullptr);
    mReaderHelper.reset(nullptr);

    return true;
}

size_t InteractiveTest::getAcquiredImageCount() const
{
   size_t acquiredImageCount = mReaderHelper->getAcquiredImageCount();
   CAM_ULOGMD("Acquired image count(%zu)", acquiredImageCount);
   return acquiredImageCount;
}

} // namespace tests
} // namespace NSCam
