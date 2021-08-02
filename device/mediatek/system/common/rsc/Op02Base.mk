# For rat config
include device/mediatek/system/common/rsc/RatMode5mFddBase.mk

# Set locale to simplified Chinese for China operator project
MTK_RSC_SYSTEM_PROPERTIES += persist.sys.locale=zh-Hans-CN

# MAPC configuration file
ifneq ($(wildcard $(MTK_TARGET_PROJECT_FOLDER)/custom.conf),)
else ifneq ($(wildcard vendor/mediatek/proprietary/operator/SPEC/OP02/custom.conf),)
MTK_RSC_COPY_FILES += \
        vendor/mediatek/proprietary/operator/SPEC/OP02/custom.conf:SYSTEM:custom.conf
endif

MTK_RSC_MODULES += OP02Telephony

MTK_RSC_APKS += OP02Dialer:SYSTEM:app
MTK_RSC_APKS += OP02Settings:PRODUCT:app
MTK_RSC_APKS += OP02SystemUI:PRODUCT:app
MTK_RSC_APKS += OP02Mms:SYSTEM:app
MTK_RSC_APKS += OP02Stk:SYSTEM:priv-app
MTK_RSC_APKS += OP02StkOverlay:SYSTEM:overlay
