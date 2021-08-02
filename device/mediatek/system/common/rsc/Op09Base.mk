# For rat config
include device/mediatek/system/common/rsc/RatMode6mBase.mk

# Set locale to simplified Chinese for China operator project
MTK_RSC_SYSTEM_PROPERTIES += persist.sys.locale=zh-Hans-CN

# MAPC configuration file
ifneq ($(wildcard $(MTK_TARGET_PROJECT_FOLDER)/custom.conf),)
else ifneq ($(wildcard vendor/mediatek/proprietary/operator/SPEC/OP09/SPEC0212/SEGC/custom.conf),)
MTK_RSC_COPY_FILES += \
        vendor/mediatek/proprietary/operator/SPEC/OP09/SPEC0212/SEGC/custom.conf:SYSTEM:custom.conf
endif

MTK_RSC_MODULES += OP09CTelephony

MTK_RSC_APKS += OP09ClibDialer:SYSTEM:app
MTK_RSC_APKS += OP09ClibTelecom:SYSTEM:app
MTK_RSC_APKS += OP09MmsClib:SYSTEM:app
MTK_RSC_APKS += OP09ClibSettings:PRODUCT:app
MTK_RSC_APKS += OP09ClipSystemUI:PRODUCT:app
