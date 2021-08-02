# For rat config
include device/mediatek/vendor/common/rsc/RatMode6mBase.mk

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

#For DSBP
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.mtk_md_sbp_custom_value=0

