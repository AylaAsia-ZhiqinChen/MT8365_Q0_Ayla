# Inherit for devices that support 64-bit primary and 32-bit secondary zygote startup script
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)

# Inherit from those products. Most specific first.
#$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)

# Set target and base project for flavor build
MTK_TARGET_PROJECT := $(subst full_,,$(TARGET_PRODUCT))
MTK_BASE_PROJECT := $(MTK_TARGET_PROJECT)
MTK_PROJECT_FOLDER := $(LOCAL_PATH)
MTK_TARGET_PROJECT_FOLDER := $(LOCAL_PATH)

# This is where we'd set a backup provider if we had one
#$(call inherit-product, device/sample/products/backup_overlay.mk)
$(call inherit-product, $(LOCAL_PATH)/device.mk)

include $(LOCAL_PATH)/ProjectConfig.mk
include $(wildcard device/mediatek/build/core/mssi_fo.mk)
# set locales & aapt config.
PRODUCT_LOCALES := en_US es_ES zh_CN zh_TW ru_RU pt_BR fr_FR de_DE tr_TR it_IT in_ID ms_MY vi_VN ar_EG hi_IN th_TH bn_IN pt_PT ur_PK fa_IR nl_NL el_GR hu_HU tl_PH ro_RO cs_CZ iw_IL my_MM km_KH ko_KR pl_PL es_US bg_BG hr_HR lv_LV lt_LT sk_SK uk_UA de_AT da_DK fi_FI nb_NO sv_SE en_GB ja_JP

#AAPT configuration
PRODUCT_AAPT_CONFIG := normal large xlarge
PRODUCT_AAPT_PREF_CONFIG := xhdpi

# Set those variables here to overwrite the inherited values.
PRODUCT_MANUFACTURER := alps
PRODUCT_NAME := full_aiot8362am3_64_bsp
PRODUCT_DEVICE := aiot8362am3_64_bsp
PRODUCT_MODEL := aiot8362am3_64_bsp
PRODUCT_POLICY := android.policy_phone
PRODUCT_BRAND := alps

ifeq ($(TARGET_BUILD_VARIANT), eng)
KERNEL_DEFCONFIG ?= aiot8362am3_64_bsp_debug_defconfig
TARGET_KERNEL_USE_CLANG := false
TARGET_KERNEL_CROSS_COMPILE := $(PWD)/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-gnu-6.3.1/bin/aarch64-linux-gnu-
else
KERNEL_DEFCONFIG ?= aiot8362am3_64_bsp_defconfig
endif
PRELOADER_TARGET_PRODUCT ?= aiot8362am3_64_bsp
LK_PROJECT ?= aiot8362am3_64_bsp
TRUSTY_PROJECT ?= aiot8362am3_64_bsp

PRODUCT_CHARACTERISTICS := tablet
