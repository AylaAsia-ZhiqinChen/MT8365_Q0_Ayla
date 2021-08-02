/*
 * Copyright 2016 The Android Open Source Project
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

#define LOG_TAG "vendor.mediatek.hardware.imsa@1.0-service"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Looper.h>
#include <utils/Log.h>
#include <utils/StrongPointer.h>
#include <vendor/mediatek/hardware/imsa/1.0/IImsa.h>
#include "Imsa.h"
#include "ImsaClient.h"

using vendor::mediatek::hardware::imsa::V1_0::IImsa;
using vendor::mediatek::hardware::imsa::V1_0::implementation::Imsa;
using vendor::mediatek::hardware::imsa::V1_0::IImsaIndication;
using ::android::hardware::configureRpcThreadpool;
using ::android::hardware::joinRpcThreadpool;
using ::android::hardware::Return;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_array;
using ::android::hardware::Void;
namespace NS_VENDOR = vendor::mediatek::hardware::imsa::V1_0;

#define UNUSED(x) (x)   //eliminate "warning: unused parameter"

extern "C" void eventNotify(int8_t* buffer, int32_t request_id, int32_t length);


struct ImsaImpl : public NS_VENDOR::IImsa {
    sp<NS_VENDOR::IImsaIndication> mImsaIndication;
    Return<void> setResponseFunctions(const sp<IImsaIndication>& imsaIndication);
    Return<void> writeEvent(int32_t request_id, int32_t length, const hidl_vec<int8_t>& value);
    Return<void> writeBytes(const hidl_vec<int8_t>& value);
    Return<void> writeInt(int32_t value);
};

static ImsaClient *imsaClient = new ImsaClient();
static android::sp<ImsaImpl> g_imsa_service;

int main() {
    configureRpcThreadpool(1, true /* callerWillJoin */);
    // Setup hwbinder service
    g_imsa_service = new ImsaImpl();
    CHECK_EQ(g_imsa_service->registerAsService("imsa"), android::NO_ERROR)
        << "Failed to register imsa HAL";

    // create recv thread and try to connect the IMSM socket
    IMSA_Connect();
    ALOGD("IMSA : hidl service start");

    joinRpcThreadpool();
    ALOGD("IMSA : hidl service is terminating...");
    return 0;
}

hidl_vec<int8_t> byteToVectorList(int8_t* buffer, int length) {
    hidl_vec<int8_t> data;
    data.setToExternal(buffer, length);
    return data;
}

void eventNotify(int8_t* buffer, int32_t request_id, int32_t length) {
    ALOGD("IMSA : eventNotify from IMSM : buffer = %p, request_id = %d, length = %d, buffer size = %zu ",
        buffer, request_id, length, sizeof(buffer));
    if (g_imsa_service != NULL && g_imsa_service->mImsaIndication != NULL) {
        auto ret = g_imsa_service->mImsaIndication->readEvent(byteToVectorList(buffer, length), request_id, length);
        if (!ret.isOk()) {
            ALOGE("IMSA : eventNotify failed !!");
        }

    }
}

Return<void> ImsaImpl::writeEvent(int32_t request_id, int32_t length, const hidl_vec<int8_t>& value) {
    ALOGD("IMSA : writeEvent: request_id=%d, length=%d, value address = %p, value size = %zu", request_id, length, value.data(), value.size());
    char* data = (char *)value.data();
    imsaClient->writeEvent(request_id, length, data);
    return Void();
}

Return<void> ImsaImpl::writeBytes(const hidl_vec<int8_t>& value) {
    ALOGD("IMSA writeBytes: ");
    UNUSED(value);
    return Void();
}

Return<void> ImsaImpl::writeInt(int32_t value) {
    ALOGD("IMSA writeInt: value = %d", value);
    UNUSED(value);
    return Void();
}

Return<void> ImsaImpl::setResponseFunctions(const sp<IImsaIndication>& imsaIndication) {
    ALOGD("IMSA : setResponseFunctions !!");
    mImsaIndication = imsaIndication;
/*for test
    if(mImsaIndication != NULL) {
        ALOGD("IMSA HIDL : callback to java part test !!");
        int testdata[] = {1, 2, 3};
        void *indication = testdata;
        hidl_vec<int8_t> data;
        data.setToExternal((int8_t *) indication, sizeof(indication));
        mImsaIndication->readEvent(data, 123, 3);
    }
*/
    return Void();
}
