# Based on DMC framework
include device/mediatek/common/rsc/DmcBase.mk

# ----------------------------------------------------------------------------
# Modem Analysis Public Interface(MAPI)
# ----------------------------------------------------------------------------
MTK_RSC_SYSTEM_PROPERTIES += \
    ro.vendor.system.mtk_mapi_support=1

MTK_RSC_VENDOR_PROPERTIES += \
    ro.vendor.mtk_mapi_support=1

MTK_RSC_MODULES += \
    mdi_redirector \
    mdi_redirector_ctrl \
    libtranslator_mapi_v3.0
