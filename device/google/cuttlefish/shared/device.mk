#
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
#

# Enable updating of APEXes
$(call inherit-product, $(SRC_TARGET_DIR)/product/updatable_apex.mk)

PRODUCT_COPY_FILES += device/google/cuttlefish_kernel/4.14-x86_64/kernel:kernel

PRODUCT_SHIPPING_API_LEVEL := 29
PRODUCT_OTA_ENFORCE_VINTF_KERNEL_REQUIREMENTS := false

DISABLE_RILD_OEM_HOOK := true

# Properties that are not vendor-specific. These will go in the product
# partition, instead of the vendor partition, and do not need vendor
# sepolicy
PRODUCT_PRODUCT_PROPERTIES := \
    persist.adb.tcp.port=5555 \
    persist.traced.enable=1 \
    persist.heapprofd.enable=1 \
    ro.com.google.locationfeatures=1 \
 
# Explanation of specific properties:
#   debug.hwui.swap_with_damage avoids boot failure on M http://b/25152138
#   ro.opengles.version OpenGLES 3.0
PRODUCT_PROPERTY_OVERRIDES += \
    tombstoned.max_tombstone_count=500 \
    bt.rootcanal_test_console=off \
    debug.hwui.swap_with_damage=0 \
    ro.carrier=unknown \
    ro.com.android.dataroaming=false \
    ro.hardware.virtual_device=1 \
    ro.logd.size=1M \
    ro.opengles.version=196608 \
    wifi.interface=wlan0 \

# Below is a list of properties we probably should get rid of.
PRODUCT_PROPERTY_OVERRIDES += \
    wlan.driver.status=ok

#
# Packages for various cuttlefish-specific tests
#
PRODUCT_PACKAGES += \
    vsoc_guest_region_e2e_test \
    vsoc_managed_region_e2e_test \
    vsoc_driver_test

#
# Packages for various GCE-specific utilities
#
PRODUCT_PACKAGES += \
    audiotop \
    dhcpcd_wlan0 \
    gce_fs_monitor \
    socket_forward_proxy \
    socket_vsock_proxy \
    usbforward \
    VSoCService \
    wpa_supplicant.vsoc.conf \
    vsoc_input_service \
    vport_trigger \
    rename_netiface \
    ip_link_add \
    setup_wifi \
    vsock_logcat \

#
# Packages for AOSP-available stuff we use from the framework
#
PRODUCT_PACKAGES += \
    dhcpcd-6.8.2 \
    dhcpcd-6.8.2.conf \
    e2fsck \
    ip \
    network \
    perf \
    scp \
    sleep \
    tcpdump \
    wpa_supplicant \
    wificond \

#
# Packages for the OpenGL implementation
#

# SwiftShader provides a software-only implementation that is not thread-safe
PRODUCT_PACKAGES += \
    libEGL_swiftshader \
    libGLESv1_CM_swiftshader \
    libGLESv2_swiftshader

DEVICE_PACKAGE_OVERLAYS := device/google/cuttlefish/shared/overlay
PRODUCT_AAPT_CONFIG := normal large xlarge hdpi xhdpi
# PRODUCT_AAPT_PREF_CONFIG is intentionally not set to pick up every density resources.

#
# General files
#
PRODUCT_COPY_FILES += \
    device/google/cuttlefish/shared/config/audio_policy.conf:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy.conf \
    device/google/cuttlefish/shared/config/camera_v3.json:$(TARGET_COPY_OUT_VENDOR)/etc/config/camera.json \
    device/google/cuttlefish/shared/config/init.common.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.common.rc \
    device/google/cuttlefish/shared/config/init.cutf_ivsh.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.cutf_ivsh.rc \
    device/google/cuttlefish/shared/config/init.cutf_cvm.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.cutf_cvm.rc \
    device/google/cuttlefish/shared/config/init.product.rc:$(TARGET_COPY_OUT_PRODUCT)/etc/init/init.rc \
    device/google/cuttlefish/shared/config/ueventd.rc:$(TARGET_COPY_OUT_VENDOR)/ueventd.rc \
    device/google/cuttlefish/shared/config/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml \
    device/google/cuttlefish/shared/config/media_codecs_google_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video.xml \
    device/google/cuttlefish/shared/config/media_codecs_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_performance.xml \
    device/google/cuttlefish/shared/config/media_profiles.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml \
    frameworks/av/media/libeffects/data/audio_effects.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_telephony.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_configuration_generic.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/primary_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/primary_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
    frameworks/av/services/audiopolicy/config/surround_sound_configuration_5_0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/surround_sound_configuration_5_0.xml \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.low_latency.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.camera.full.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.full.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.raw.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.raw.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.ethernet.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.software.app_widgets.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.app_widgets.xml \
    system/bt/vendor_libs/test_vendor_lib/data/controller_properties.json:vendor/etc/bluetooth/controller_properties.json \


#
# The fstab requires special handling. For system-as-root builds, we *must*
# retrieve the vendor partition mount options from DTB, as system must be
# "pristine" to support GSI. For builds with an initrd, we prefer not to
# rely on DTB, and *must* retrieve the partition mount options from an fstab
# in the initrd instead. (In either case, the fstab *must also* be installed to
# /vendor/etc)
#
ifeq ($(TARGET_BUILD_SYSTEM_ROOT_IMAGE),true)
PRODUCT_COPY_FILES += \
    device/google/cuttlefish/shared/config/fstab.dtb:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.cutf_ivsh \
    device/google/cuttlefish/shared/config/fstab.dtb:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.cutf_cvm \

else ifeq ($(TARGET_USE_DYNAMIC_PARTITIONS),true)
PRODUCT_COPY_FILES += \
    device/google/cuttlefish/shared/config/fstab.initrd-dynamic-partitions:$(TARGET_COPY_OUT_RAMDISK)/fstab.cutf_ivsh \
    device/google/cuttlefish/shared/config/fstab.initrd-dynamic-partitions:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.cutf_ivsh \
    device/google/cuttlefish/shared/config/fstab.initrd-dynamic-partitions:$(TARGET_COPY_OUT_RAMDISK)/fstab.cutf_cvm \
    device/google/cuttlefish/shared/config/fstab.initrd-dynamic-partitions:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.cutf_cvm \

else
PRODUCT_COPY_FILES += \
    device/google/cuttlefish/shared/config/fstab.initrd:$(TARGET_COPY_OUT_RAMDISK)/fstab.cutf_ivsh \
    device/google/cuttlefish/shared/config/fstab.initrd:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.cutf_ivsh \
    device/google/cuttlefish/shared/config/fstab.initrd:$(TARGET_COPY_OUT_RAMDISK)/fstab.cutf_cvm \
    device/google/cuttlefish/shared/config/fstab.initrd:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.cutf_cvm \

endif


#
# USB Specific
#
PRODUCT_COPY_FILES += \
    device/google/cuttlefish/shared/config/init.hardware.usb.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.cutf_ivsh.usb.rc

# Packages for HAL implementations

#
# Atrace HAL
#
PRODUCT_PACKAGES += \
    android.hardware.atrace@1.0-service

#
# Authsecret HAL
#
PRODUCT_PACKAGES += \
    android.hardware.authsecret@1.0-service

#
# Hardware Composer HAL
#
PRODUCT_PACKAGES += \
    hwcomposer.drm_minigbm \
    hwcomposer.cutf_ivsh \
    hwcomposer.cutf_cvm \
    hwcomposer-stats \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.composer@2.1-service

#
# Gralloc HAL
#
PRODUCT_PACKAGES += \
    gralloc.minigbm \
    gralloc.cutf \
    android.hardware.graphics.mapper@2.0-impl-2.1 \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service

#
# Bluetooth HAL and Compatibility Bluetooth library (for older revs).
#
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-service.sim \
    android.hardware.bluetooth.audio@2.0-impl

#
# Audio HAL
#
PRODUCT_PACKAGES += \
    audio.primary.cutf \
    audio.r_submix.default \
    android.hardware.audio@5.0-impl:32 \
    android.hardware.audio.effect@5.0-impl:32 \
    android.hardware.audio@2.0-service \
    android.hardware.soundtrigger@2.0-impl \

#
# Drm HAL
#
PRODUCT_PACKAGES += \
    android.hardware.drm@1.2-service.clearkey

#
# Dumpstate HAL
#
PRODUCT_PACKAGES += \
    android.hardware.dumpstate@1.0-service.cuttlefish

#
# Camera
#
PRODUCT_PACKAGES += \
    camera.cutf \
    camera.cutf.jpeg \
    camera.device@3.2-impl \
    android.hardware.camera.provider@2.4-impl \
    android.hardware.camera.provider@2.4-service

#
# Gatekeeper
#
PRODUCT_PACKAGES += \
    gatekeeper.cutf \
    android.hardware.gatekeeper@1.0-impl \
    android.hardware.gatekeeper@1.0-service

#
# GPS
#
PRODUCT_PACKAGES += \
    android.hardware.gnss@2.0-service

# Health
PRODUCT_PACKAGES += \
    android.hardware.health@2.0-service.cuttlefish

# Health Storage
PRODUCT_PACKAGES += \
    android.hardware.health.storage@1.0-service.cuttlefish

# Input Classifier HAL
PRODUCT_PACKAGES += \
    android.hardware.input.classifier@1.0-service.default

#
# Sensors
#
PRODUCT_PACKAGES += \
    android.hardware.sensors@2.0-service.mock
#
# Thermal (mock)
#
PRODUCT_PACKAGES += \
    android.hardware.thermal@2.0-service.mock

#
# Lights
#
PRODUCT_PACKAGES += \
    lights.cutf \
    android.hardware.light@2.0-impl \
    android.hardware.light@2.0-service

#
# Keymaster HAL
#
PRODUCT_PACKAGES += \
     android.hardware.keymaster@4.0-impl \
     android.hardware.keymaster@4.0-service

#
# Power HAL
#
PRODUCT_PACKAGES += \
    power.cutf \
    android.hardware.power@1.0-impl \
    android.hardware.power@1.0-service


#
# PowerStats HAL
#
PRODUCT_PACKAGES += \
    android.hardware.power.stats@1.0-service.mock

#
# NeuralNetworks HAL
#
PRODUCT_PACKAGES += \
    android.hardware.neuralnetworks@1.2-service-sample-all \
    android.hardware.neuralnetworks@1.2-service-sample-float-fast \
    android.hardware.neuralnetworks@1.2-service-sample-float-slow \
    android.hardware.neuralnetworks@1.2-service-sample-minimal \
    android.hardware.neuralnetworks@1.2-service-sample-quant

#
# USB
PRODUCT_PACKAGES += \
    android.hardware.usb@1.0-service

# Vibrator HAL
PRODUCT_PACKAGES += \
    android.hardware.vibrator@1.3-service.example

PRODUCT_PACKAGES += \
    cuttlefish_dtb

# WLAN driver configuration files
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant_overlay.conf

# Recovery mode
ifneq ($(TARGET_NO_RECOVERY),true)

PRODUCT_COPY_FILES += \
    device/google/cuttlefish/shared/config/init.recovery.common.rc:recovery/root/init.recovery.common.rc \
    device/google/cuttlefish/shared/config/init.recovery.cutf_ivsh.rc:recovery/root/init.recovery.cutf_ivsh.rc \
    device/google/cuttlefish/shared/config/init.recovery.cutf_cvm.rc:recovery/root/init.recovery.cutf_cvm.rc \

endif

# Host packages to install
PRODUCT_HOST_PACKAGES += socket_forward_proxy socket_vsock_proxy
