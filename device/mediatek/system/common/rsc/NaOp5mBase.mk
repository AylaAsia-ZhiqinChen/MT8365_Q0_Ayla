
# For SIM Configuration: Single SIM
include device/mediatek/system/common/rsc/SsBase.mk

# Rat Configuration: 5M
include device/mediatek/system/common/rsc/RatMode5mC2kBase.mk

MTK_RSC_XML_OPTR := OP08_SPEC0200_SEGDEFAULT

MTK_RSC_APKS += \
	CarrierExpress:SYSTEM:priv-app

MTK_RSC_SYSTEM_PROPERTIES += \
	persist.vendor.mtk_uce_support=1 \
	persist.vendor.mtk_rcs_support=1 \
	persist.vendor.mtk_rtt_support=1 \
	persist.vendor.vzw_device_type=3

# ----------------------------------------------------------------------------
# OP07 specific APK and Jar
# ----------------------------------------------------------------------------
MTK_RSC_APKS += \
    OP07cellbroadcastreceiver:SYSTEM:app \
    OP07TeleService:SYSTEM:app  \
    OP07Telecomm:SYSTEM:app  \
    OP07Dialer:SYSTEM:app  \
    OP07Contacts:PRODUCT:app  \
    OP07Browser:PRODUCT:app  \
    OP07Entitlement:SYSTEM:app  \
    OP07Entitlement-Fcm:SYSTEM:app  \
    OP07Settings:PRODUCT:app \
    OP07Wos:SYSTEM:app \


MTK_RSC_MODULES += \
    OP07Telephony \
	libeap-aka

# ----------------------------------------------------------------------------
# OP08 specific APK and Jar
# ----------------------------------------------------------------------------
MTK_RSC_APKS += \
    OP08Contacts:PRODUCT:app \
    OP08Ims:SYSTEM:app \
    OP08Wos:SYSTEM:app \
    OP08Dialer:SYSTEM:app \
    OP08cellbroadcastreceiver:SYSTEM:app \
    OP08TeleService:SYSTEM:app \
    OP08Settings:PRODUCT:app \
    OP08SystemUI:SYSTEM:app \
    OP08Telecom:SYSTEM:app

MTK_RSC_MODULES += OP08Telephony

# ----------------------------------------------------------------------------
# OP12 specific APK and Jar
# ----------------------------------------------------------------------------
MTK_RSC_APKS += \
    OP12TeleService:SYSTEM:app \
    OP12Settings:PRODUCT:app \
    OP12Telecomm:SYSTEM:app \
    OP12Dialer:SYSTEM:app \
    OP12Ims:SYSTEM:app


MTK_RSC_MODULES += \
    OP12Telephony \
    OP12CellbroadcastReceiver

# ----------------------------------------------------------------------------
# OP20 specific APK and Jar
# ----------------------------------------------------------------------------
MTK_RSC_APKS += \
    OP20SystemUI:SYSTEM:app \
    OP20TeleService:SYSTEM:app \
    OP20Settings:PRODUCT:app
