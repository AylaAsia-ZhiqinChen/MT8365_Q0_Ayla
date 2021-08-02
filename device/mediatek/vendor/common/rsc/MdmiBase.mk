# Based on DMC framework
include device/mediatek/vendor/common/rsc/DmcBase.mk

# ----------------------------------------------------------------------------
# Modem Analysis Public Interface(MAPI)
# ----------------------------------------------------------------------------
# [Vendor Partition]
MTK_RSC_VENDOR_PROPERTIES += \
    ro.vendor.mtk_mdmi_support=1

MTK_RSC_MODULES += \
    libtranslator_mdmi_v2.8.2
