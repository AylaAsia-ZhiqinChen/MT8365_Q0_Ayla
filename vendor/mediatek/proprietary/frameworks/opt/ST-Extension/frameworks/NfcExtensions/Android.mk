#
# Copyright (C) 2008 The Android Open Source Project
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
#

# This makefile shows how to build your own shared library that can be
# shipped on the system of a phone, and included additional examples of
# including JNI code with the library and writing client applications against it.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := false
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE := com.st.android.nfc_extensions

LOCAL_AIDL_INCLUDES := $(LOCAL_PATH)/java
LOCAL_SRC_FILES := $(call all-java-files-under, java)  \
                   $(call all-Iaidl-files-under, java) \

include $(BUILD_JAVA_LIBRARY)

# To build a version usable in an SDK (exctract from a Google group):
#You need to make use of javac-check target. This has been implemented for this purpose.
#make javac-check-$(LOCAL_MODULE)
#So, if your module name is "ABCD"
#make javac-check-ABCD
#This generates a classes-full-debug.jar, in the common\obj\JAVA_LIBRARIES\ABCD_intermediates.
