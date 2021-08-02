# For rat config
include device/mediatek/system/common/rsc/RatMode6mBase.mk

# Set locale to simplified Chinese for China operator project
MTK_RSC_SYSTEM_PROPERTIES += persist.sys.locale=zh-Hans-CN

# MAPC configuration file
ifneq ($(wildcard $(MTK_TARGET_PROJECT_FOLDER)/custom.conf),)
else ifneq ($(wildcard vendor/mediatek/proprietary/operator/SPEC/OP01/SPEC0200/SEGC/custom.conf),)
MTK_RSC_COPY_FILES += \
        vendor/mediatek/proprietary/operator/SPEC/OP01/SPEC0200/SEGC/custom.conf:SYSTEM:custom.conf
endif

MTK_RSC_MODULES += OP01Telephony

MTK_RSC_APKS += OP01Dialer:SYSTEM:app

ifeq ($(wildcard vendor/mediatek/internal/build vendor/mediatek/libs_internal),vendor/mediatek/internal/build vendor/mediatek/libs_internal)
  ifneq ($(wildcard vendor/mediatek/proprietary/operator/prebuilts/3rd-party/CTMLib),)
    MTK_RSC_MODULES += ctm
  endif
endif

MTK_RSC_APKS += Op01Contacts:PRODUCT:app
MTK_RSC_APKS += OP01Telecom:SYSTEM:app
MTK_RSC_APKS += OP01TeleService:SYSTEM:app
MTK_RSC_APKS += OP01Mms:SYSTEM:app
MTK_RSC_APKS += OP01Email:SYSTEM:app
MTK_RSC_APKS += OP01Settings:PRODUCT:app
MTK_RSC_APKS += OP01SystemUI:PRODUCT:app
MTK_RSC_APKS += OP01SoundRecorder:SYSTEM:app
MTK_RSC_APKS += Op01WifiService:SYSTEM:app
MTK_RSC_APKS += UsbChecker:SYSTEM:app

ifneq ($(wildcard vendor/mediatek/proprietary/operator/packages/apps/TxtViewer),)
MTK_RSC_APKS += TXTViewer:SYSTEM:app
endif
