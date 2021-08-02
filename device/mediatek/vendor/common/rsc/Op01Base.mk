# For rat config
include device/mediatek/vendor/common/rsc/RatMode6mBase.mk

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

#For DSBP
MTK_RSC_VENDOR_PROPERTIES += \
        persist.vendor.radio.mtk_dsbp_support=0 \
        ro.vendor.mtk_md_sbp_custom_value=1

# For MCF(Modem configuration framework)
MTK_RSC_VENDOR_PROPERTIES += ro.vendor.mtk_mcf_support=0

