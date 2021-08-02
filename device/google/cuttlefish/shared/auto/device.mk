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

################################################
# Begin GCE specific configurations

DEVICE_MANIFEST_FILE += device/google/cuttlefish/shared/config/manifest.xml
DEVICE_MANIFEST_FILE += device/google/cuttlefish/shared/auto/manifest.xml

TARGET_BUILD_SYSTEM_ROOT_IMAGE ?= true

$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)
$(call inherit-product, device/google/cuttlefish/shared/device.mk)

################################################
# Begin general Android Auto Embedded configurations

PRODUCT_COPY_FILES += \
    packages/services/Car/car_product/init/init.bootstat.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw//init.bootstat.rc \
    packages/services/Car/car_product/init/init.car.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw//init.car.rc


PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.gsm.xml

# Auto core hardware permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/car_core_hardware.xml:system/etc/permissions/car_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.type.automotive.xml:system/etc/permissions/android.hardware.type.automotive.xml \

# Enable landscape
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.screen.landscape.xml:system/etc/permissions/android.hardware.screen.landscape.xml

# Location permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml

# Broadcast Radio permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.broadcastradio.xml:system/etc/permissions/android.hardware.broadcastradio.xml

PRODUCT_PROPERTY_OVERRIDES += \
    keyguard.no_require_sim=true \
    ro.cdma.home.operator.alpha=Android \
    ro.cdma.home.operator.numeric=302780 \
    vendor.rild.libpath=libcuttlefish-ril.so \

# vehicle HAL
PRODUCT_PACKAGES += android.hardware.automotive.vehicle@2.0-service

# Broadcast Radio
PRODUCT_PACKAGES += android.hardware.broadcastradio@2.0-service

# DRM HAL
PRODUCT_PACKAGES += android.hardware.drm@1.2-service.clearkey

# GPS HAL
PRODUCT_PACKAGES += \
    android.hardware.gnss@2.0-service

# Cell network connection
PRODUCT_PACKAGES += \
    MmsService \
    Phone \
    PhoneService \
    Telecom \
    TeleService \
    libcuttlefish-ril \
    libcuttlefish-rild \

# DRM Properities
PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=true

BOARD_IS_AUTOMOTIVE := true

$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base.mk)
$(call inherit-product, frameworks/native/build/phone-xhdpi-2048-dalvik-heap.mk)
$(call inherit-product, packages/services/Car/car_product/build/car.mk)

# Placed here due to b/110784510
PRODUCT_BRAND := generic

PRODUCT_ENFORCE_RRO_TARGETS := framework-res
PRODUCT_ENFORCE_RRO_EXCLUDED_OVERLAYS := device/google/cuttlefish/shared/overlay
