#
# Copyright 2017 The Android Open-Source Project
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

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# This section generates wpa_supplicant.conf using the target product name and
# model as that file requires such build target specific fields.

LOCAL_MODULE := wpa_supplicant.vsoc.conf
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc/wifi
LOCAL_MODULE_STEM := wpa_supplicant.conf
LOCAL_MULTILIB := first

include $(BUILD_SYSTEM)/base_rules.mk

$(LOCAL_BUILT_MODULE): $(LOCAL_PATH)/gen_wpa_supplicant_conf.sh
	$(hide) echo "Generating $@"
	$(hide) mkdir -p $(dir $@)
	$(hide) $< "${TARGET_PRODUCT}" "${PRODUCT_MODEL}" \
	    "${PLATFORM_SDK_VERSION}" > $@

include $(CLEAR_VARS)

# This file is deprecated and is kept round for host tools compatibility with
# older target filesystem images. It is not necessary to pass a .dtb blob
# through to QEMU any more.

LOCAL_MODULE := initrd_root_dtb
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/config
LOCAL_MODULE_STEM := initrd-root.dtb
LOCAL_SRC_FILES := initrd-root.dtb

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE := system_root_dtb
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/config
LOCAL_MODULE_STEM := system-root.dtb
LOCAL_SRC_FILES := system-root.dtb

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE := gsi_fstab
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(HOST_OUT)/config
LOCAL_MODULE_STEM := gsi.fstab
LOCAL_SRC_FILES := gsi.fstab

include $(BUILD_PREBUILT)
