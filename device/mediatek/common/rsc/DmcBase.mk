# Based on OM project
include device/mediatek/common/rsc/OmBase.mk

# ----------------------------------------------------------------------------
# Diagnostic Monitoring Collector(DMC) Framework
# ----------------------------------------------------------------------------
# [System Partition]
MTK_RSC_SYSTEM_PROPERTIES += \
    ro.vendor.system.mtk_dmc_support=1

MTK_RSC_APKS += \
    ApmService:SYSTEM:priv-app

MTK_RSC_MODULES += \
    apm-service-defs

# [Vendor Partition]
MTK_RSC_VENDOR_PROPERTIES += \
    ro.vendor.mtk_dmc_support=1

MTK_RSC_MODULES += \
    apm-msg-defs \
    dmc_core \
    pkm_service \
    libapmonitor_vendor \
    libpkm

# HIDL manifest
ifneq ($(findstring manifest_dmc.xml, $(DEVICE_MANIFEST_FILE)), manifest_dmc.xml)
    DEVICE_MANIFEST_FILE += device/mediatek/common/project_manifest/manifest_dmc.xml
endif

ifneq ($(findstring manifest_apmonitor.xml, $(DEVICE_MANIFEST_FILE)), manifest_apmonitor.xml)
    DEVICE_MANIFEST_FILE += device/mediatek/common/project_manifest/manifest_apmonitor.xml
endif
