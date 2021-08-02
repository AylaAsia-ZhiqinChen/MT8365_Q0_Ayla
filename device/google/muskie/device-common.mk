#
# Copyright (C) 2017 The Android Open-Source Project
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

LOCAL_PATH := device/google/muskie

# Audio XMLs
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/mixer_paths_tavil.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths_tavil.xml \
    $(LOCAL_PATH)/audio_platform_info_tavil.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_platform_info_tavil.xml \
    $(LOCAL_PATH)/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml

# Bug 62375603
PRODUCT_PROPERTY_OVERRIDES += audio.adm.buffering.ms=3
PRODUCT_PROPERTY_OVERRIDES += vendor.audio.adm.buffering.ms=3
PRODUCT_PROPERTY_OVERRIDES += audio_hal.period_multiplier=2
PRODUCT_PROPERTY_OVERRIDES += af.fast_track_multiplier=1

# Whether by default, the eSIM system UI, including that in SUW and Settings, will be shown.
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += esim.enable_esim_system_ui_by_default=false

# Pro audio feature
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.audio.pro.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.pro.xml

# Enable AAudio MMAP/NOIRQ data path.
# 1 is AAUDIO_POLICY_NEVER  means only use Legacy path.
# 2 is AAUDIO_POLICY_AUTO   means try MMAP then fallback to Legacy path.
# 3 is AAUDIO_POLICY_ALWAYS means only use MMAP path.
PRODUCT_PROPERTY_OVERRIDES += aaudio.mmap_policy=2
# 1 is AAUDIO_POLICY_NEVER  means only use SHARED mode
# 2 is AAUDIO_POLICY_AUTO   means try EXCLUSIVE then fallback to SHARED mode.
# 3 is AAUDIO_POLICY_ALWAYS means only use EXCLUSIVE mode.
PRODUCT_PROPERTY_OVERRIDES += aaudio.mmap_exclusive_policy=2

# Increase the apparent size of a hardware burst from 1 msec to 2 msec.
# A "burst" is the number of frames processed at one time.
# That is an increase from 48 to 96 frames at 48000 Hz.
# The DSP will still be bursting at 48 frames but AAudio will think the burst is 96 frames.
# A low number, like 48, might increase power consumption or stress the system.
PRODUCT_PROPERTY_OVERRIDES += aaudio.hw_burst_min_usec=2000

# Enable SM log mechanism by default
ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
PRODUCT_PROPERTY_OVERRIDES += \
    persist.radio.smlog_switch=1 \
    ro.radio.log_prefix="modem_log_" \
    ro.radio.log_loc="/data/vendor/modem_dump"

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init.logging.userdebug.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.$(PRODUCT_HARDWARE).logging.rc
else
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init.logging.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.$(PRODUCT_HARDWARE).logging.rc
endif

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/init-common.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init-$(PRODUCT_HARDWARE).rc \
    $(LOCAL_PATH)/init.common.usb.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.$(PRODUCT_HARDWARE).usb.rc \
    $(LOCAL_PATH)/init.insmod.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/init.insmod.cfg \
    $(LOCAL_PATH)/init.insmod_charger.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/init.insmod_charger.cfg

# Input device files
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/synaptics_dsxv26.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/synaptics_dsxv26.idc

include device/google/wahoo/device.mk

# Kernel modules
ifeq (,$(filter-out walleye_kasan, $(TARGET_PRODUCT)))
# if TARGET_PRODUCT == *_kasan
BOARD_VENDOR_KERNEL_MODULES += \
    device/google/wahoo-kernel/kasan/synaptics_dsx_core_htc.ko \
    device/google/wahoo-kernel/kasan/synaptics_dsx_rmi_dev_htc.ko \
    device/google/wahoo-kernel/kasan/synaptics_dsx_fw_update_htc.ko \
    device/google/wahoo-kernel/kasan/htc_battery.ko \
    device/google/wahoo-kernel/kasan/wlan.ko
else ifeq (,$(filter-out walleye_kernel_debug_memory, $(TARGET_PRODUCT)))
# if TARGET == walleye_kernel_debug_memory
BOARD_VENDOR_KERNEL_MODULES += \
    device/google/wahoo-kernel/debug_memory/synaptics_dsx_core_htc.ko \
    device/google/wahoo-kernel/debug_memory/synaptics_dsx_rmi_dev_htc.ko \
    device/google/wahoo-kernel/debug_memory/synaptics_dsx_fw_update_htc.ko \
    device/google/wahoo-kernel/debug_memory/htc_battery.ko \
    device/google/wahoo-kernel/debug_memory/wlan.ko
else ifeq (,$(filter-out walleye_kernel_debug_locking, $(TARGET_PRODUCT)))
# if TARGET == walleye_kernel_debug_locking
BOARD_VENDOR_KERNEL_MODULES += \
    device/google/wahoo-kernel/debug_locking/synaptics_dsx_core_htc.ko \
    device/google/wahoo-kernel/debug_locking/synaptics_dsx_rmi_dev_htc.ko \
    device/google/wahoo-kernel/debug_locking/synaptics_dsx_fw_update_htc.ko \
    device/google/wahoo-kernel/debug_locking/htc_battery.ko \
    device/google/wahoo-kernel/debug_locking/wlan.ko
else ifeq (,$(filter-out walleye_kernel_debug_hang, $(TARGET_PRODUCT)))
# if TARGET == walleye_kernel_debug_hang
BOARD_VENDOR_KERNEL_MODULES += \
    device/google/wahoo-kernel/debug_hang/synaptics_dsx_core_htc.ko \
    device/google/wahoo-kernel/debug_hang/synaptics_dsx_rmi_dev_htc.ko \
    device/google/wahoo-kernel/debug_hang/synaptics_dsx_fw_update_htc.ko \
    device/google/wahoo-kernel/debug_hang/htc_battery.ko \
    device/google/wahoo-kernel/debug_hang/wlan.ko
else ifeq (,$(filter-out walleye_kernel_debug_api, $(TARGET_PRODUCT)))
# if TARGET == walleye_kernel_debug_api
BOARD_VENDOR_KERNEL_MODULES += \
    device/google/wahoo-kernel/debug_api/synaptics_dsx_core_htc.ko \
    device/google/wahoo-kernel/debug_api/synaptics_dsx_rmi_dev_htc.ko \
    device/google/wahoo-kernel/debug_api/synaptics_dsx_fw_update_htc.ko \
    device/google/wahoo-kernel/debug_api/htc_battery.ko \
    device/google/wahoo-kernel/debug_api/wlan.ko
else
BOARD_VENDOR_KERNEL_MODULES += \
    device/google/wahoo-kernel/synaptics_dsx_core_htc.ko \
    device/google/wahoo-kernel/synaptics_dsx_rmi_dev_htc.ko \
    device/google/wahoo-kernel/synaptics_dsx_fw_update_htc.ko \
    device/google/wahoo-kernel/htc_battery.ko \
    device/google/wahoo-kernel/wlan.ko
endif

PRODUCT_COPY_FILES += \
    device/google/muskie/nfc/libnfc-nxp.muskie.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nxp.conf

PRODUCT_COPY_FILES += \
    device/google/muskie/thermal-engine.conf:$(TARGET_COPY_OUT_VENDOR)/etc/thermal-engine.conf \
    device/google/muskie/thermal-engine-vr.conf:$(TARGET_COPY_OUT_VENDOR)/etc/thermal-engine-vr.conf

PRODUCT_COPY_FILES += \
    device/google/muskie/powerhint.json:$(TARGET_COPY_OUT_VENDOR)/etc/powerhint.json

# Wifi configuration file
PRODUCT_COPY_FILES += \
    device/google/muskie/WCNSS_qcom_cfg.ini:$(TARGET_COPY_OUT_VENDOR)/firmware/wlan/qca_cld/WCNSS_qcom_cfg.ini

#IMU calibration
PRODUCT_PROPERTY_OVERRIDES += \
  persist.config.calibration_fac=/persist/sensors/calibration/calibration.xml

# NFC/camera interaction workaround - DO NOT COPY TO NEW DEVICES
PRODUCT_PROPERTY_OVERRIDES += \
    ro.camera.notify_nfc=1

PRODUCT_CHECK_ELF_FILES := true
