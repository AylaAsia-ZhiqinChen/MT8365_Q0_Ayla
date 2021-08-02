# For SIM Configuration: Dual SIM
include device/mediatek/vendor/common/rsc/DsdsBase.mk

# RAT Configuration: 4M and L+L
include device/mediatek/vendor/common/rsc/RatMode4mFddBase.mk

MTK_RSC_VENDOR_PROPERTIES += \
    persist.vendor.radio.mtk_ps2_rat=L/W/G \

# Operator package properties: default OP08
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.operator.optr=OP07 \
        ro.vendor.operator.spec=SPEC0407 \
        ro.vendor.operator.seg=SEGDEFAULT \
        persist.vendor.operator.optr=OP07 \
        persist.vendor.operator.spec=SPEC0407 \
        persist.vendor.operator.seg=SEGDEFAULT

MTK_RSC_XML_OPTR := OP07_SPEC0407_SEGDEFAULT

# For DSBP
MTK_RSC_VENDOR_PROPERTIES += \
        persist.vendor.radio.mtk_dsbp_support=1 \
        ro.vendor.mtk_md_sbp_custom_value=7

# For CXP
MTK_RSC_VENDOR_PROPERTIES += \
    ro.vendor.mtk_carrierexpress_pack=no

# For CXP-Switable NA Features, configured with PRODUCT_PROPERTY_OVERRIDES in device.mk
MTK_RSC_VENDOR_PROPERTIES += \
    persist.vendor.volte_support=1 \
    persist.vendor.mtk.volte.enable=1 \
    persist.vendor.mtk_wfc_support=1 \
    persist.vendor.mtk.wfc.enable=1 \
    persist.vendor.vilte_support=1 \
    persist.vendor.mtk.ims.video.enable=1 \
    persist.vendor.viwifi_support=1 \
    persist.vendor.mtk_rcs_ua_support=1

# For MCF(Modem configuration framework)
MTK_RSC_VENDOR_PROPERTIES += ro.vendor.mtk_mcf_support=0

# ----------------------------------------------------------------------------
# OP07 specific APK and Jar
# ----------------------------------------------------------------------------

