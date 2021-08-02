# For rat config
include device/mediatek/common/rsc/RatMode6mBase.mk

# Add common operator package properties
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.operator.optr=OP09 \
        ro.vendor.operator.spec=SPEC0212 \
        ro.vendor.operator.seg=SEGC \
        persist.vendor.operator.optr=OP09 \
        persist.vendor.operator.spec=SPEC0212 \
        persist.vendor.operator.seg=SEGC

# For CT VoLTE
MTK_RSC_VENDOR_PROPERTIES += persist.vendor.mtk_ct_volte_support=1

# For CT Register
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.mtk_devreg_app=1 \
        ro.vendor.mtk_ct4greg_app=1

# Set locale to simplified Chinese for China operator project
MTK_RSC_SYSTEM_PROPERTIES += persist.sys.locale=zh-Hans-CN

#For DSBP
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.mtk_md_sbp_custom_value=0

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

MTK_RSC_XML_OPTR := OP09_SPEC0212_SEGC

