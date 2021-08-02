# Based on OM project
include device/mediatek/vendor/common/rsc/OmBase.mk

# ----------------------------------------------------------------------------
# Diagnostic Monitoring Collector(DMC) Framework
# ----------------------------------------------------------------------------
# [Vendor Partition]
MTK_RSC_VENDOR_PROPERTIES += \
    ro.vendor.mtk_dmc_support=1

MTK_RSC_MODULES += \
    dmc_core \
    pkm_service \
    libapmonitor_vendor \
    libpkm

# HIDL manifest
DEVICE_MANIFEST_FILE += device/mediatek/common/project_manifest/manifest_dmc.xml
DEVICE_MANIFEST_FILE += device/mediatek/common/project_manifest/manifest_apmonitor.xml
