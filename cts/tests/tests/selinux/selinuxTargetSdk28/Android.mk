#
# Copyright (C) 2018 The Android Open Source Project
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

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := tests
LOCAL_MULTILIB := both

LOCAL_STATIC_JAVA_LIBRARIES := \
    ctstestrunner-axt \
    compatibility-device-util-axt \

LOCAL_JAVA_LIBRARIES := android.test.base.stubs android.test.runner.stubs

LOCAL_JNI_SHARED_LIBRARIES := \
    libc++ \
    libcrypto \
    libcts_jni \
    libctsselinux_jni \
    libnativehelper \
    libnativehelper_compat_libc++ \
    libpackagelistparser \
    libpcre2 \
    libselinux \

LOCAL_SRC_FILES := $(call all-java-files-under, src common)
LOCAL_PACKAGE_NAME := CtsSelinuxTargetSdk28TestCases
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_COMPATIBILITY_SUITE := cts vts general-tests

LOCAL_MIN_SDK_VERSION := 21

include $(BUILD_CTS_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
