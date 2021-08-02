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

#include "utils/TestBase.h"
#include "InteractiveTest.h"

#include <mtkcam/utils/std/ULog.h>

#include <camera/NdkCameraManager.h>

#include <condition_variable>

// ------------------------------------------------------------------------

namespace NSCam {
namespace tests {

CAM_ULOG_DECLARE_MODULE_ID(MOD_CAMERA_HAL_SERVER);

// ------------------------------------------------------------------------

/**
 * This test fixture is for testing Camera2 VNDK API.
 */
class Camera2VendorTest : public TestBase, protected InteractiveTest
{
protected:
    // methods from NSCam::tests::InteractiveTest
    bool processCaptureRequests(const char* id) override;

private:
    void checkAcquiredImageCount();

    // NOTE: We enforce a timeout condition kAcquiredImageCountTimeout here
    //       to wait forever so that the system timeout occurs and
    //       generates the exception thereof.
    static constexpr int32_t kAcquiredImageCountTimeoutInSecond = 5;
    static constexpr std::chrono::milliseconds kAcquiredImageCountTimeout =
    std::chrono::milliseconds(kAcquiredImageCountTimeoutInSecond * 1000);

    mutable std::mutex mAcquiredImageCountTimeoutLock;
    std::condition_variable mCondAcquiredImageCountTimeout;
}; // class Camera2VendorTest


bool Camera2VendorTest::processCaptureRequests(const char* id)
{
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

        // TODO: enable more duration test
        for (auto& durationInSecond : { /*1,*/ 5/*, 10*/ })
        {
            // start capture
            startProcessCaptureRequest(id, readerUsageAndFormat,
                    TestDefs::kMaxAImagesStreaming, false, TestDefs::kRequestCount);

            // wait for a specific duration
            CAM_ULOGMD("wait for %d second(s)", durationInSecond);
            std::cout << "[----------] wait for " << durationInSecond << " second(s)\n";
            usleep(durationInSecond * 1000 * 1000);

            checkAcquiredImageCount();

            // stop capture
            stopProcessCaptureRequest();
        }
    }

    return true;
}

void Camera2VendorTest::checkAcquiredImageCount()
{
    // we need camera frames back from the Camera HAL3 impl.
    // Otherwise soldier on until the system timeout.
    size_t timeoutCount = 0;
    while (getAcquiredImageCount() == 0)
    {
        CAM_ULOGMW("we need camera frame back from the camera HAL3 impl:"
                " timeout(%zu)", timeoutCount++);
        std::unique_lock<std::mutex> _l(mAcquiredImageCountTimeoutLock);
        mCondAcquiredImageCountTimeout.wait_for(_l, kAcquiredImageCountTimeout);
    }
}

// ------------------------------------------------------------------------

TEST_F(Camera2VendorTest, CreateWindowNativeHandle)
{
    for (int i = 0; i < mCameraIdList->numCameras; i++)
    {
        const char* cameraId(mCameraIdList->cameraIds[i]);
        ASSERT_TRUE(cameraId != nullptr);

        ACameraMetadata* staticMetadata = nullptr;

        // query the capabilities of a camera device.
        // These capabilities are immutable for a given camera.
        //
        // The caller must call ACameraMetadata_free to free the memory of
        // the output characteristics
        camera_status_t ret = ACameraManager_getCameraCharacteristics(
                mCameraManager, cameraId, &staticMetadata);
        ASSERT_EQ(ret, ACAMERA_OK);
        ASSERT_NE(staticMetadata, nullptr);

        // get the camera lens facing
        uint8_t facing;
        ASSERT_TRUE(getLensFacing(*staticMetadata, facing)) <<
            "get lens facing failed";

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
}

} // namespace tests
} // namespace NSCam
