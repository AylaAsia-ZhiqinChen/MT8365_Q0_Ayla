#
# Copyright (C) 2011 The Android Open-Source Project
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

ifneq (,$(filter $(TARGET_PRODUCT),hikey960_tv hikey_tv))
# Setup TV Build
USE_OEM_TV_APP := true
$(call inherit-product, device/google/atv/products/atv_base.mk)
PRODUCT_CHARACTERISTICS := tv
PRODUCT_AAPT_PREF_CONFIG := tvdpi
PRODUCT_IS_ATV := true
else
# Adjust the dalvik heap to be appropriate for a tablet.
$(call inherit-product, frameworks/native/build/tablet-10in-xhdpi-2048-dalvik-heap.mk)
endif

# Set vendor kernel path
PRODUCT_VENDOR_KERNEL_HEADERS := device/linaro/hikey/kernel-headers

# Hikey was first supported from Marshmallow 6.0, API level 23
# PRODUCT_SHIPPING_API_LEVEL is required from Pie build by the CTS test of
# CtsOsTestCases android.os.cts.BuildTest#testSdkInt
# as reported here:
# https://bugs.linaro.org/show_bug.cgi?id=4068
# https://source.android.com/setup/start/build-numbers
# Here we set it to 25 first, because Gatekeeper function
# needs to be supported when set to 26 or above,
# which we could not boot successfully with the default implementation
# enabled yet. Will back to set it to the latest number when we have
# all necessary features supported.
PRODUCT_SHIPPING_API_LEVEL := 25

# Set custom settings
DEVICE_PACKAGE_OVERLAYS := device/linaro/hikey/overlay
ifneq (,$(filter $(TARGET_PRODUCT),hikey960_tv hikey_tv))
# Set TV Custom Settings
DEVICE_PACKAGE_OVERLAYS += device/google/atv/overlay
endif

#Force navkeys on
PRODUCT_PROPERTY_OVERRIDES += qemu.hw.mainkeys=0

# Add openssh support for remote debugging and job submission
PRODUCT_PACKAGES += ssh sftp scp sshd ssh-keygen sshd_config start-ssh

# Add wifi-related packages
PRODUCT_PACKAGES += libwpa_client wpa_supplicant hostapd wificond
PRODUCT_PROPERTY_OVERRIDES += wifi.interface=wlan0 \
                              wifi.supplicant_scan_interval=15

# Build and run only ART
PRODUCT_RUNTIMES := runtime_libart_default

# Build default bluetooth a2dp and usb audio HALs
PRODUCT_PACKAGES += audio.a2dp.default \
		    audio.usb.default \
		    audio.r_submix.default \
		    tinyplay

PRODUCT_PACKAGES += \
    android.hardware.audio@2.0-service \
    android.hardware.audio@2.0-impl \
    android.hardware.audio.effect@2.0-impl \
    android.hardware.broadcastradio@1.0-impl \
    android.hardware.soundtrigger@2.0-impl

PRODUCT_PACKAGES += vndk_package

PRODUCT_PACKAGES += \
    android.hardware.drm@1.0-impl \
    android.hardware.drm@1.0-service \
    android.hardware.bluetooth.audio@2.0-impl

PRODUCT_PACKAGES += libGLES_android

# Graphics HAL
PRODUCT_PACKAGES += \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.composer@2.1-service \
    android.hardware.graphics.mapper@2.0-impl

# Memtrack
PRODUCT_PACKAGES += memtrack.default \
    android.hardware.memtrack@1.0-service \
    android.hardware.memtrack@1.0-impl

ifeq ($(HIKEY_USE_LEGACY_TI_BLUETOOTH), true)
PRODUCT_PACKAGES += android.hardware.bluetooth@1.0-service.hikey uim
else
PRODUCT_PACKAGES += android.hardware.bluetooth@1.0-service.btlinux
endif

# PowerHAL
PRODUCT_PACKAGES += \
	android.hardware.power@1.1-impl \
	android.hardware.power@1.1-service.hikey-common

#GNSS HAL
PRODUCT_PACKAGES += \
    android.hardware.gnss@1.0-impl

# Keymaster HAL
PRODUCT_PACKAGES += \
    android.hardware.keymaster@3.0-impl \
    android.hardware.keymaster@3.0-service


# Sensor HAL
ifneq ($(TARGET_SENSOR_MEZZANINE),)
TARGET_USES_NANOHUB_SENSORHAL := true
NANOHUB_SENSORHAL_LID_STATE_ENABLED := true
NANOHUB_SENSORHAL_SENSORLIST := $(LOCAL_PATH)/sensorhal/sensorlist_$(TARGET_SENSOR_MEZZANINE).cpp
NANOHUB_SENSORHAL_DIRECT_REPORT_ENABLED := true
NANOHUB_SENSORHAL_DYNAMIC_SENSOR_EXT_ENABLED := true

PRODUCT_PACKAGES += \
    context_hub.default \
    android.hardware.sensors@1.0-service \
    android.hardware.sensors@1.0-impl \
    android.hardware.contexthub@1.0-service \
    android.hardware.contexthub@1.0-impl

# Nanohub tools
PRODUCT_PACKAGES += stm32_flash nanoapp_cmd nanotool

PRODUCT_COPY_FILES += \
    device/linaro/hikey/init.common.nanohub.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.nanohub.rc

# Copy sensors config file(s)
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.ambient_temperature.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.ambient_temperature.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.relative_humidity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.relative_humidity.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml

# Argonkey VL53L0X proximity driver is not available yet. So we are going to copy conf file for neonkey only
ifeq ($(TARGET_SENSOR_MEZZANINE),neonkey)
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml
endif

# VR HAL
PRODUCT_COPY_FILES += \
    frameworks/native/services/vr/virtual_touchpad/idc/vr-virtual-touchpad-1.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/vr-virtual-touchpad-1.idc \
    frameworks/native/data/etc/android.hardware.vr.high_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vr.high_performance.xml \
    frameworks/native/data/etc/android.hardware.vr.headtracking-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vr.headtracking.xml

PRODUCT_PACKAGES += \
    vr.default \
    android.hardware.vr@1.0-service \
    android.hardware.vr@1.0-impl

endif


ifneq (,$(filter $(TARGET_PRODUCT),hikey960_tv hikey_tv))
# TV Specific Packages
PRODUCT_PACKAGES += \
    TvSettings \
    LiveTv \
    google-tv-pairing-protocol \
    TvProvision \
    LeanbackSampleApp \
    TvSampleLeanbackLauncher \
    TvProvider \
    SettingsIntelligence \
    tv_input.default \
    com.android.media.tv.remoteprovider \
    InputDevices
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=260
else

# Use Launcher3QuickStep
PRODUCT_PACKAGES += Launcher3QuickStep
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=160
endif

# Copy hardware config file(s)
PRODUCT_COPY_FILES +=  \
        device/linaro/hikey/etc/permissions/android.hardware.screen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.screen.xml \
        device/linaro/hikey/etc/permissions/android.software.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.xml \
        frameworks/native/data/etc/android.software.cts.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.cts.xml \
        frameworks/native/data/etc/android.software.app_widgets.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.app_widgets.xml \
        frameworks/native/data/etc/android.software.backup.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.backup.xml \
        frameworks/native/data/etc/android.software.voice_recognizers.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.voice_recognizers.xml \
        frameworks/native/data/etc/android.hardware.ethernet.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.ethernet.xml \
        frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml \
        frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml \
        frameworks/native/data/etc/android.software.device_admin.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.device_admin.xml

# Include BT modules
$(call inherit-product, device/linaro/hikey/wpan/ti-wpan-products.mk)

PRODUCT_COPY_FILES += \
        frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml \
        frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \
        frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml \
        frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml \
        device/linaro/hikey/wpa_supplicant.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant.conf \
        $(LOCAL_PATH)/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant_overlay.conf \
        $(LOCAL_PATH)/p2p_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/p2p_supplicant_overlay.conf

# audio policy configuration
USE_XML_AUDIO_POLICY_CONF := 1
PRODUCT_COPY_FILES += \
    device/linaro/hikey/audio/audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/a2dp_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/a2dp_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/usb_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml

# Copy media codecs config file
PRODUCT_COPY_FILES += \
        device/linaro/hikey/etc/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml \
        frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml

PRODUCT_COPY_FILES += \
    device/linaro/hikey/init.common.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.common.rc \

PRODUCT_PACKAGES += \
    android.hardware.renderscript@1.0.vndk-sp\
    android.hardware.graphics.composer@2.1.vndk-sp\
    android.hardware.graphics.allocator@2.0.vndk-sp\
    android.hardware.graphics.mapper@2.0.vndk-sp\
    android.hardware.graphics.common@1.0.vndk-sp\
    libvndksupport.vndk-sp\
    libbinder.vndk-sp\
    libhwbinder.vndk-sp\
    libbase.vndk-sp\
    libfmq.vndk-sp\
    libcutils.vndk-sp\
    libhardware.vndk-sp\
    libhidlbase.vndk-sp\
    libhidltransport.vndk-sp\
    libui.vndk-sp\
    libutils.vndk-sp\
    libc++.vndk-sp\
    libRS_internal.vndk-sp\
    libRSDriver.vndk-sp\
    libRSCpuRef.vndk-sp\
    libbcinfo.vndk-sp\
    libblas.vndk-sp\
    libft2.vndk-sp\
    libpng.vndk-sp\
    libcompiler_rt.vndk-sp\
    libbacktrace.vndk-sp\
    libunwind.vndk-sp\
    libunwindstack.vndk-sp\
    liblzma.vndk-sp\
    libion.vndk-sp\
