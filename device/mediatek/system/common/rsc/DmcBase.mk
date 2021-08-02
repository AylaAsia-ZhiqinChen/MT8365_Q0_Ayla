# Based on OM project
include device/mediatek/system/common/rsc/OmBase.mk

# ----------------------------------------------------------------------------
# Diagnostic Monitoring Collector(DMC) Framework
# ----------------------------------------------------------------------------
# [System Partition]
MTK_RSC_SYSTEM_PROPERTIES += \
    ro.vendor.system.mtk_dmc_support=1

MTK_RSC_APKS += \
    ApmService:SYSTEM:priv-app

MTK_RSC_MODULES += \
    apm-msg-defs \
    apm-service-defs
