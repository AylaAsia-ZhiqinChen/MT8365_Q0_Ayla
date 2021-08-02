# For rat config
include device/mediatek/common/rsc/RatMode6mBase.mk

# Add common operator package properties
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.operator.optr=OP01 \
        ro.vendor.operator.spec=SPEC0200 \
        ro.vendor.operator.seg=SEGC \
        persist.vendor.operator.optr=OP01 \
        persist.vendor.operator.spec=SPEC0200 \
        persist.vendor.operator.seg=SEGC

# For CT VoLTE
MTK_RSC_VENDOR_PROPERTIES += persist.vendor.mtk_ct_volte_support=0

# Set locale to simplified Chinese for China operator project
MTK_RSC_SYSTEM_PROPERTIES += persist.sys.locale=zh-Hans-CN

#For DSBP
MTK_RSC_VENDOR_PROPERTIES += \
        persist.vendor.radio.mtk_dsbp_support=0 \
        ro.vendor.mtk_md_sbp_custom_value=1

# For MCF(Modem configuration framework)
MTK_RSC_VENDOR_PROPERTIES += ro.vendor.mtk_mcf_support=0

# MAPC configuration file
ifneq ($(wildcard $(MTK_TARGET_PROJECT_FOLDER)/custom.conf),)
else ifneq ($(wildcard vendor/mediatek/proprietary/operator/SPEC/OP01/SPEC0200/SEGC/custom.conf),)
MTK_RSC_COPY_FILES += \
        vendor/mediatek/proprietary/operator/SPEC/OP01/SPEC0200/SEGC/custom.conf:SYSTEM:custom.conf
endif

MTK_RSC_MODULES += OP01Telephony

ifeq ($(wildcard vendor/mediatek/internal/build vendor/mediatek/libs_internal),vendor/mediatek/internal/build vendor/mediatek/libs_internal)
  ifneq ($(wildcard vendor/mediatek/proprietary/operator/prebuilts/3rd-party/CTMLib),)
    MTK_RSC_MODULES += ctm
  endif
endif

ifneq ($(wildcard vendor/mediatek/proprietary/operator/packages/apps/TxtViewer),)
MTK_RSC_APKS += TXTViewer:SYSTEM:app
endif

MTK_RSC_APKS += OP01Dialer:SYSTEM:app
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

MTK_RSC_XML_OPTR := OP01_SPEC0200_SEGC

