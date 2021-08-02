
# For SIM Configuration: Single SIM
include device/mediatek/common/rsc/SsBase.mk

# Rat Configuration: 5M
include device/mediatek/common/rsc/RatMode5mC2kBase.mk

# Operator package properties: default OP08
MTK_RSC_VENDOR_PROPERTIES += \
        ro.vendor.operator.optr=OP08 \
        ro.vendor.operator.spec=SPEC0200 \
        ro.vendor.operator.seg=SEGDEFAULT \
        persist.vendor.operator.optr=OP08 \
        persist.vendor.operator.spec=SPEC0200 \
        persist.vendor.operator.seg=SEGDEFAULT

MTK_RSC_XML_OPTR := OP08_SPEC0200_SEGDEFAULT

# For DSBP
MTK_RSC_VENDOR_PROPERTIES += \
        persist.vendor.radio.mtk_dsbp_support=1 \
        ro.vendor.mtk_md_sbp_custom_value=8

# For CXP
MTK_RSC_VENDOR_PROPERTIES += \
	ro.vendor.mtk_carrierexpress_pack=na

MTK_RSC_APKS += \
	CarrierExpress:SYSTEM:priv-app

# For CXP-Switable NA Features, configured with PRODUCT_PROPERTY_OVERRIDES in device.mk
MTK_RSC_VENDOR_PROPERTIES += \
	persist.vendor.volte_support=1 \
	persist.vendor.mtk.volte.enable=1 \
	persist.vendor.mtk_wfc_support=1 \
	persist.vendor.mtk.wfc.enable=1 \
	persist.vendor.vilte_support=1 \
	persist.vendor.mtk.ims.video.enable=1 \
	persist.vendor.viwifi_support=1 \
	persist.vendor.mtk_rcs_ua_support=1

# For CXP-Switable NA features, configured with PRODUCT_SYSTEM_DEFAULT_PROPERTIES in device.mk

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
    OP08SystemUI:PRODUCT:app \
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
    OP12Ims:SYSTEM:app \
    OP12ImsPco:SYSTEM:app

MTK_RSC_MODULES += \
    OP12Telephony \
    OP12CellbroadcastReceiver

# ----------------------------------------------------------------------------
# OP20 specific APK and Jar
# ----------------------------------------------------------------------------
MTK_RSC_APKS += \
    OP20SystemUI:PRODUCT:app \
    OP20Settings:PRODUCT:app \
    OP20TeleService:SYSTEM:app
