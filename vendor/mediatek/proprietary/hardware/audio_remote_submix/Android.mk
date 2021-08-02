# Copyright (C) 2012 The Android Open Source Project
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
ifeq ($(strip $(BOARD_USES_MTK_AUDIO)),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := audio.r_submix.$(TARGET_BOARD_PLATFORM)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_HEADER_LIBRARIES  += libhardware_headers libaudio_system_headers libsystem_headers

LOCAL_SRC_FILES := \
	audio_hw.cpp
LOCAL_C_INCLUDES += \
	frameworks/av/include/ \
	frameworks/native/include/ \
	$(call include-path-for, audio-utils)
LOCAL_SHARED_LIBRARIES := liblog libcutils libutils libnbaio_mono
LOCAL_STATIC_LIBRARIES := libmedia_helper
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -Wno-unused-parameter

ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
  LOCAL_CFLAGS += -DCONFIG_MT_ENG_BUILD
else ifeq ($(strip $(TARGET_BUILD_VARIANT)),userdebug)
  LOCAL_CFLAGS += -DCONFIG_MT_USERDEBUG_BUILD
endif

include $(BUILD_SHARED_LIBRARY)

endif
