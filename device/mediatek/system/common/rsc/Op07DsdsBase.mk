# For SIM Configuration: Dual SIM
include device/mediatek/system/common/rsc/DsdsBase.mk

# RAT Configuration: 4M and L+L
include device/mediatek/system/common/rsc/RatMode4mFddBase.mk

MTK_RSC_XML_OPTR := OP07_SPEC0407_SEGDEFAULT

# For CXP-Switable NA features, configured with PRODUCT_SYSTEM_DEFAULT_PROPERTIES in device.mk

MTK_RSC_SYSTEM_PROPERTIES += \
    persist.vendor.mtk_uce_support=1 \
    persist.vendor.mtk_rcs_support=1 \
    persist.vendor.mtk_rtt_support=1 \
    persist.vendor.vzw_device_type=0

# RCS related configurations
MTK_RSC_APKS += \
    AutoConfigService:SYSTEM:priv-app \
    Rcse:SYSTEM:app \
    RcsStack:SYSTEM:priv-app \
    RcsUaService:SYSTEM:priv-app

MTK_RSC_MODULES += \
    com.mediatek.ims.rcsua \
    com.mediatek.ims.rcsua.xml

# ----------------------------------------------------------------------------
# OP07 specific APK and Jar
# ----------------------------------------------------------------------------
MTK_RSC_APKS += \
    OP07cellbroadcastreceiver:SYSTEM:app \
    OP07Dialer:SYSTEM:app  \
    OP07TeleService:SYSTEM:app  \
    OP07Telecomm:SYSTEM:app  \
    OP07Contacts:PRODUCT:app  \
    OP07Browser:PRODUCT:app  \
    OP07Entitlement:SYSTEM:app  \
    OP07Entitlement-Fcm:SYSTEM:app  \
    OP07Settings:PRODUCT:app \
    OP07Wos:SYSTEM:app

MTK_RSC_MODULES += \
    OP07Telephony \
    libeap-aka

