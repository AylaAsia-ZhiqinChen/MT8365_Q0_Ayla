# Copyright (C) 2016 The Android Open Source Project
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

# HAL module implemenation stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so

# Old hwcomposer, relies on GLES composition
include $(CLEAR_VARS)
include $(LOCAL_PATH)/hwcomposer.mk
LOCAL_CFLAGS += -DUSE_OLD_HWCOMPOSER -Wall -Werror
LOCAL_MODULE := hwcomposer.cutf_ivsh-deprecated

# See b/67109557
ifeq (true, $(TARGET_TRANSLATE_2ND_ARCH))
LOCAL_MULTILIB := first
endif

include $(BUILD_SHARED_LIBRARY)

# New hwcomposer, performs software composition
include $(CLEAR_VARS)
include $(LOCAL_PATH)/hwcomposer.mk
LOCAL_MODULE := hwcomposer.cutf_ivsh
LOCAL_VENDOR_MODULE := true

# See b/67109557
ifeq (true, $(TARGET_TRANSLATE_2ND_ARCH))
LOCAL_MULTILIB := first
endif

include $(BUILD_SHARED_LIBRARY)
