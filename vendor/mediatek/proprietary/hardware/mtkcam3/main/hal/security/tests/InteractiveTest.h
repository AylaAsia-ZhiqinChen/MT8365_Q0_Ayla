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

#ifndef VENDOR_MEDIATEK_HARDWARE_CAMERA3_MAIN_HAL_SECURITY_TESTS_INTERACTIVETEST_H
#define VENDOR_MEDIATEK_HARDWARE_CAMERA3_MAIN_HAL_SECURITY_TESTS_INTERACTIVETEST_H

#include "TestDefs.h"

#include <system/window.h>
#include <camera/NdkCameraMetadataTags.h>

#include <utility>
#include <memory>

// ------------------------------------------------------------------------

__BEGIN_DECLS
struct ACameraManager;
struct ACameraIdList;
struct ACameraMetadata;
__END_DECLS

namespace NSCam {

class ACameraHelper;
class AImageReaderHelper;

namespace tests {

/**
 * This test is for testing Camera2 VNDK API under interactive mode.
 */
class InteractiveTest
{
public:
    InteractiveTest();
    virtual ~InteractiveTest();

    int run();

protected:
    // ACameraManager is opaque type that provides access to camera service
    ACameraManager* mCameraManager = nullptr;

    // struct to hold list of camera device Ids.
    // This can refer to either the Ids of connected camera devices
    // returned from ACameraManager_getCameraIdList,
    // or the physical camera Ids passed into
    // ACameraDevice_createCaptureRequest_withPhysicalIds.
    ACameraIdList* mCameraIdList = nullptr;

    std::unique_ptr<ACameraHelper> mCameraHelper;
    std::unique_ptr<AImageReaderHelper> mReaderHelper;

    static bool isCapabilitySupported(const ACameraMetadata& staticInfo,
            acamera_metadata_enum_android_request_available_capabilities_t cap);
    static bool getLensFacing(const ACameraMetadata& staticInfo, uint8_t& facing);

    virtual bool processCaptureRequests(const char* id);
    bool startProcessCaptureRequest(
            const char* id, const TestDefs::ReaderUsageAndAFormat& readerUsageAndFormat,
            int32_t readerMaxImages, bool readerAsync, uint32_t requestCount);
    bool stopProcessCaptureRequest();

    size_t getAcquiredImageCount() const;
}; // class Camera2VendorTest

} // namespace tests
} // namespace NSCam

#endif // VENDOR_MEDIATEK_HARDWARE_CAMERA3_MAIN_HAL_SECURITY_TESTS_INTERACTIVETEST_H
