#
# Copyright 2016 The Android Open Source Project
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

PRODUCT_HARDWARE := sargo

include device/google/bonito/device-common.mk

DEVICE_PACKAGE_OVERLAYS += device/google/bonito/sargo/overlay

PRODUCT_COPY_FILES += \
    device/google/bonito/nfc/libnfc-nxp.sargo.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nxp.conf

PRODUCT_PRODUCT_PROPERTIES += ro.com.google.ime.height_ratio=1.2

# Vibrator HAL
PRODUCT_PRODUCT_PROPERTIES +=\
    ro.vibrator.hal.click.duration=8 \
    ro.vibrator.hal.tick.duration=5 \
    ro.vibrator.hal.heavyclick.duration=12 \
    ro.vibrator.hal.short.voltage=110 \
    ro.vibrator.hal.long.voltage=80 \
    ro.vibrator.hal.long.frequency.shift=10

# DRV2624 Haptics Waveform
PRODUCT_COPY_FILES += \
    device/google/bonito/vibrator/drv2624/drv2624_S4.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/drv2624.bin

# camera front flashColor
PRODUCT_PROPERTY_OVERRIDES += \
    persist.camera.front.flashColor=0xffe1c1

# Add white point compensated coefficient
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.display.adaptive_white_coefficient=0.0031,0.5535,-87.498,0.0031,0.5535,-87.498,0.0031,0.5535,-87.498
