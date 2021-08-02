# For SIM Configuration: Single SIM
include device/mediatek/system/common/rsc/SsBase.mk

# Rat Configuration: 5M
include device/mediatek/system/common/rsc/RatMode5mC2kBase.mk

MTK_RSC_XML_OPTR := OP236_SPEC0200_SEGDEFAULT

# For CXP-Switable NA features, configured with PRODUCT_SYSTEM_DEFAULT_PROPERTIES in device.mk
MTK_RSC_SYSTEM_PROPERTIES += \
    persist.vendor.mtk_uce_support=0 \
    persist.vendor.mtk_rcs_support=0 \
    persist.vendor.mtk_rtt_support=1 \
    persist.vendor.vzw_device_type=0

# ----------------------------------------------------------------------------
# OP236 specific APK and Jar
# ----------------------------------------------------------------------------
MTK_RSC_APKS += \
    OP236cellbroadcastreceiver:SYSTEM:app
