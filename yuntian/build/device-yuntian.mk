##########################################
# Yuntian Makefile Configuration
#
#  Author:      caicai
#  Version:     1.0.0
#  Date:        2017.02.07
##########################################
include yuntian/build/yuntian_build_pre.mk

# @echo "copy mag $(CUSTOM_HAL_MSENSORLIB)"

$(warning $(CUSTOM_HAL_MSENSORLIB))

# build as 64bit
ifeq ($(strip $(MTK_K64_SUPPORT)),yes)
#afx33 库文件
ifneq (,$(filter afx, $(CUSTOM_HAL_MSENSORLIB)))
CUST_LIB_FILE=device/mediateksample/$(MTK_TARGET_PROJECT)/overlay/yuntian/msensor/afx/lib64/libvtclib.so
ifeq ($(CUST_LIB_FILE), $(wildcard $(CUST_LIB_FILE)))
PRODUCT_COPY_FILES += $(CUST_LIB_FILE):vendor/lib64/libvtclib.so
else
PRODUCT_COPY_FILES += yuntian/msensor/afx/lib64/libvtclib.so:vendor/lib64/libvtclib.so
endif
endif
#mmc3630x 库文件
ifneq (,$(filter mmc3630x, $(CUSTOM_HAL_MSENSORLIB)))
CUST_LIB_FILE=device/mediateksample/$(MTK_TARGET_PROJECT)/overlay/yuntian/msensor/mmc3630/lib64/libmemsic.so
CUST_LIB_FILE1=device/mediateksample/$(MTK_TARGET_PROJECT)/overlay/yuntian/msensor/mmc3630/lib64/libksensor.so
ifeq ($(CUST_LIB_FILE), $(wildcard $(CUST_LIB_FILE)))
ifeq ($(CUST_LIB_FILE1), $(wildcard $(CUST_LIB_FILE1)))
PRODUCT_COPY_FILES += $(CUST_LIB_FILE):vendor/lib64/libmemsic.so
PRODUCT_COPY_FILES += $(CUST_LIB_FILE1):vendor/lib64/libksensor.so
endif
else
PRODUCT_COPY_FILES += yuntian/msensor/mmc3630/lib64/libmemsic.so:vendor/lib64/libmemsic.so
PRODUCT_COPY_FILES += yuntian/msensor/mmc3630/lib64/libksensor.so:vendor/lib64/libksensor.so
endif
endif

#build as 32bit
else
#afx33 库文件
ifneq (,$(filter afx, $(CUSTOM_HAL_MSENSORLIB)))
CUST_LIB_FILE=device/mediateksample/$(MTK_TARGET_PROJECT)/overlay/yuntian/msensor/afx/lib/libvtclib.so
ifeq ($(CUST_LIB_FILE), $(wildcard $(CUST_LIB_FILE)))
PRODUCT_COPY_FILES += $(CUST_LIB_FILE):vendor/lib/libvtclib.so
else
PRODUCT_COPY_FILES += yuntian/msensor/afx/lib/libvtclib.so:vendor/lib/libvtclib.so
endif
endif
#mmc3630x 库文件
ifneq (,$(filter mmc3630x, $(CUSTOM_HAL_MSENSORLIB)))
CUST_LIB_FILE=device/mediateksample/$(MTK_TARGET_PROJECT)/overlay/yuntian/msensor/mmc3630/lib/libmemsic.so
CUST_LIB_FILE1=device/mediateksample/$(MTK_TARGET_PROJECT)/overlay/yuntian/msensor/mmc3630/lib/libksensor.so
ifeq ($(CUST_LIB_FILE), $(wildcard $(CUST_LIB_FILE)))
ifeq ($(CUST_LIB_FILE1), $(wildcard $(CUST_LIB_FILE1)))
PRODUCT_COPY_FILES += $(CUST_LIB_FILE):vendor/lib/libmemsic.so
PRODUCT_COPY_FILES += $(CUST_LIB_FILE1):vendor/lib/libksensor.so
endif
else
PRODUCT_COPY_FILES += yuntian/msensor/mmc3630/lib/libmemsic.so:vendor/lib/libmemsi.so
PRODUCT_COPY_FILES += yuntian/msensor/mmc3630/lib/libksensor.so:vendor/lib/libksensor.so
endif
endif
endif
PRODUCT_PACKAGES += FactoryTest
PRODUCT_PACKAGES += ImeiSettings
PRODUCT_PACKAGES += AgingTest
PRODUCT_PACKAGES += libFactorytest_jni
PRODUCT_COPY_FILES += yuntian/packages/apps/AgingTest/data/BeatPlucker.mp3:system/media/Test.mp3
#yuntian longyao add自定义开关机动画铃声和logo
#custom logo
$(shell rm -rf  vendor/mediatek/proprietary/bootable/bootloader/lk/dev/logo/out ; mkdir vendor/mediatek/proprietary/bootable/bootloader/lk/dev/logo/out)
ifneq ($(shell find  vendor/mediatek/proprietary/bootable/bootloader/lk/dev/logo/  -name $(BOOT_LOGO)),)
$(shell cp -rf  vendor/mediatek/proprietary/bootable/bootloader/lk/dev/logo/$(BOOT_LOGO)  vendor/mediatek/proprietary/bootable/bootloader/lk/dev/logo/out/)
ifneq ($(shell find  device/mediateksample/$(MTK_TARGET_PROJECT)/  -name custom),)
ifneq ($(shell find  device/mediateksample/$(MTK_TARGET_PROJECT)/custom/  -name logo),)
ifneq ($(shell find  device/mediateksample/$(MTK_TARGET_PROJECT)/custom/logo/  -name $(BOOT_LOGO)),)
$(shell cp -rf  device/mediateksample/$(MTK_TARGET_PROJECT)/custom/logo/$(BOOT_LOGO)/*  vendor/mediatek/proprietary/bootable/bootloader/lk/dev/logo/out/$(BOOT_LOGO)/)
endif
endif
endif
else
$(error  projectconfig.mk  BOOT_LOGO Setting error, Noexist!   )
endif

#custom animation

ifneq ($(wildcard device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim),)
ifneq ($(shell find device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim  -name bootanimation.zip),)
PRODUCT_COPY_FILES += device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim/bootanimation.zip:system/media/bootanimation.zip
else
PRODUCT_COPY_FILES += yuntian/bootanim/$(BOOT_LOGO)/bootanimation.zip:system/media/bootanimation.zip
endif
ifneq ($(shell find device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim  -name bootaudio.mp3),)
PRODUCT_COPY_FILES += device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim/bootaudio.mp3:system/media/bootaudio.mp3
endif
ifneq ($(shell find device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim  -name shutanimation.zip),)
PRODUCT_COPY_FILES += device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim/shutanimation.zip:system/media/shutanimation.zip
endif
ifneq ($(shell find device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim  -name shutdownanimation.zip),)
PRODUCT_COPY_FILES += device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim/shutdownanimation.zip:system/media/shutanimation.zip
endif
ifneq ($(shell find device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim  -name shutaudio.mp3),)
PRODUCT_COPY_FILES += device/mediateksample/$(MTK_TARGET_PROJECT)/custom/bootanim/shutaudio.mp3:system/media/shutaudio.mp3
endif
else
ifneq ($(wildcard yuntian/bootanim/$(BOOT_LOGO)),)
ifneq ($(shell find yuntian/bootanim/$(BOOT_LOGO)  -name bootanimation.zip),)
PRODUCT_COPY_FILES += yuntian/bootanim/$(BOOT_LOGO)/bootanimation.zip:system/media/bootanimation.zip
endif
endif
endif
PRODUCT_PACKAGES += mtkbootanimation
PRODUCT_PACKAGES += libmtkbootanimation
#yuntian longyao end
