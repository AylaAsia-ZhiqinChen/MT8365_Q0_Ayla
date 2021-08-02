# Copyright (C) 2017 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := gralloc.cutf_ivsh-future
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
    gralloc.cpp \
    mapper.cpp

LOCAL_C_INCLUDES += \
    device/google/cuttlefish_common \
    device/google/cuttlefish_kernel \
    hardware/libhardware/include \
    system/core/base/include

LOCAL_CFLAGS := \
    -DLOG_TAG=\"gralloc_vsoc\" \
    -Wno-missing-field-initializers \
    -Wall -Werror \
    $(VSOC_VERSION_CFLAGS)

LOCAL_SHARED_LIBRARIES := \
    libbase \
    libcutils \
    cuttlefish_auto_resources \
    libcuttlefish_fs \
    liblog \
    vsoc_lib

LOCAL_HEADER_LIBRARIES := \
    libhardware_headers

ifeq (0, $(shell test $(PLATFORM_SDK_VERSION) -ge 21; echo $$?))
LOCAL_MODULE_RELATIVE_PATH := hw
else
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
endif

LOCAL_VENDOR_MODULE := true

# See b/67109557
ifeq (true, $(TARGET_TRANSLATE_2ND_ARCH))
LOCAL_MULTILIB := first
endif
include $(BUILD_SHARED_LIBRARY)

include $(call first-makefiles-under,$(LOCAL_PATH))
