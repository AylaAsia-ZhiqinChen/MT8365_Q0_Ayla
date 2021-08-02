# Add common operator package properties
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.operator.optr=OP02 \
        ro.vendor.operator.spec=SPEC0200 \
        ro.vendor.operator.seg=SEGA \
        persist.vendor.operator.optr=OP02 \
        persist.vendor.operator.spec=SPEC0200 \
        persist.vendor.operator.seg=SEGA

# For rat config
include device/mediatek/common/rsc/RatMode5mFddBase.mk

# For CT VoLTE
MTK_RSC_VENDOR_PROPERTIES += persist.vendor.mtk_ct_volte_support=0

# Set locale to simplified Chinese for China operator project
MTK_RSC_SYSTEM_PROPERTIES += persist.sys.locale=zh-Hans-CN

# MAPC configuration file
ifneq ($(wildcard $(MTK_TARGET_PROJECT_FOLDER)/custom.conf),)
else ifneq ($(wildcard vendor/mediatek/proprietary/operator/SPEC/OP02/custom.conf),)
MTK_RSC_COPY_FILES += \
        vendor/mediatek/proprietary/operator/SPEC/OP02/custom.conf:SYSTEM:custom.conf
endif

#For DSBP
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.mtk_md_sbp_custom_value=0

MTK_RSC_MODULES += OP02Telephony

MTK_RSC_APKS += OP02Dialer:SYSTEM:app
MTK_RSC_APKS += OP02Settings:PRODUCT:app
MTK_RSC_APKS += OP02SystemUI:PRODUCT:app
MTK_RSC_APKS += OP02Mms:SYSTEM:app
MTK_RSC_APKS += OP02Stk:SYSTEM:priv-app
MTK_RSC_APKS += OP02StkOverlay:SYSTEM:overlay

MTK_RSC_MODULES += mtkbootanimation
MTK_RSC_MODULES += libmtkbootanimation

MTK_RSC_XML_OPTR := OP02_SPEC0200_SEGA

