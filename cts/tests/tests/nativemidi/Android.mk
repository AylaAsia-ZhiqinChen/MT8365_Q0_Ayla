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

#
# NativeMidiEchoTest
#
include $(CLEAR_VARS)

# Don't include this package in any target.
LOCAL_MODULE_TAGS := optional

# When built, explicitly put it in the data partition.
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA_APPS)

# Tag this module as a cts test artifact
LOCAL_COMPATIBILITY_SUITE := cts vts general-tests

LOCAL_SRC_FILES := $(call all-java-files-under, java)

LOCAL_STATIC_JAVA_LIBRARIES := compatibility-device-util-axt ctstestrunner-axt cts-midi-lib
LOCAL_JNI_SHARED_LIBRARIES := libnativemidi_jni
LOCAL_JAVA_LIBRARIES := android.test.base.stubs

# Must match the package name in CtsTestCaseList.mk
LOCAL_PACKAGE_NAME := CtsNativeMidiTestCases
LOCAL_MULTILIB := both

LOCAL_SDK_VERSION := current
LOCAL_NDK_STL_VARIANT := c++_static

include $(BUILD_CTS_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
