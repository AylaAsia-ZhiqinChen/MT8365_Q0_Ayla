# Copyright (C) 2009 The Android Open Source Project
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

LOCAL_PACKAGE_NAME := CtsJniTestCases

# Don't include this package in any target.
LOCAL_MODULE_TAGS := optional

# Include both the 32 and 64 bit versions
LOCAL_MULTILIB := both

# When built, explicitly put it in the data partition.
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA_APPS)

# Tag this module as a cts test artifact
LOCAL_COMPATIBILITY_SUITE := cts vts general-tests

LOCAL_STATIC_JAVA_LIBRARIES := ctstestrunner-axt androidx.test.rules

LOCAL_JNI_SHARED_LIBRARIES := \
  libjni_test_dlclose \
  libjninamespacea1 \
  libjninamespacea2 \
  libjninamespaceb \
  libjnicommon \
  libjnitest \
  libnativehelper_compat_libc++

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_SDK_VERSION := current
LOCAL_NDK_STL_VARIANT := c++_shared

LOCAL_USE_EMBEDDED_NATIVE_LIBS := false

include $(BUILD_CTS_PACKAGE)

# Include the associated library's makefile.
include $(call all-makefiles-under,$(LOCAL_PATH))
