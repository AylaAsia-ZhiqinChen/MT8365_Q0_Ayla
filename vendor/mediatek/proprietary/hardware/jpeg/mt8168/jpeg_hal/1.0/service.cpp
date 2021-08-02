
#define LOG_TAG "HIDL_JPEG"
#include <log/log.h>

#include <cutils/properties.h>
#include <hidl/HidlTransportSupport.h>
#include <stdio.h>
#include <string.h>

#include "JpegEnc.h"

using android::OK;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;
using android::status_t;

using vendor::mediatek::hardware::jpeg::V1_0::implementation::JpegEnc;

int main() {
    configureRpcThreadpool(1, true);
    sp<JpegEnc>  myTest = new  JpegEnc();
    status_t status = myTest->registerAsService();
    if (status != OK) {
        //LOG_ALWAYS_FATAL("Could not register service for UICC%d (%d)", i, status);
        return -1;
    }

    joinRpcThreadpool();
}
