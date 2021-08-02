/*
 * Copyright (C) 2019 The Android Open Source Project
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

#include <VtsHalHidlTargetTestBase.h>
#include <android-base/logging.h>

#include "supplicant_hidl_test_utils.h"
#include "supplicant_hidl_test_utils_1_2.h"

using ::android::sp;
using ::android::hardware::wifi::supplicant::V1_2::ISupplicant;
using ::android::hardware::wifi::supplicant::V1_2::ISupplicantP2pIface;
using ::android::hardware::wifi::supplicant::V1_2::ISupplicantStaIface;
using ::android::hardware::wifi::supplicant::V1_2::ISupplicantStaNetwork;

sp<ISupplicant> getSupplicant_1_2() {
    return ISupplicant::castFrom(getSupplicant());
}

sp<ISupplicantStaIface> getSupplicantStaIface_1_2() {
    return ISupplicantStaIface::castFrom(getSupplicantStaIface());
}

sp<ISupplicantStaNetwork> createSupplicantStaNetwork_1_2() {
    return ISupplicantStaNetwork::castFrom(createSupplicantStaNetwork());
}

sp<ISupplicantP2pIface> getSupplicantP2pIface_1_2() {
    return ISupplicantP2pIface::castFrom(getSupplicantP2pIface());
}
