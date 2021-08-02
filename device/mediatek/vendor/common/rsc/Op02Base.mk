# Add common operator package properties
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.operator.optr=OP02 \
        ro.vendor.operator.spec=SPEC0200 \
        ro.vendor.operator.seg=SEGA \
        persist.vendor.operator.optr=OP02 \
        persist.vendor.operator.spec=SPEC0200 \
        persist.vendor.operator.seg=SEGA

# For rat config
include device/mediatek/vendor/common/rsc/RatMode5mFddBase.mk

# For CT VoLTE
MTK_RSC_VENDOR_PROPERTIES += persist.vendor.mtk_ct_volte_support=0

#For DSBP
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.mtk_md_sbp_custom_value=0

