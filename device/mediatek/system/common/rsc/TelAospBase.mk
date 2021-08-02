# Properties to control telephony add-on
MTK_RSC_SYSTEM_PROPERTIES += \
        ro.vendor.mtk_telephony_add_on_policy=1
MTK_RSC_SYSTEM_PROPERTIES += \
        ro.vendor.mtk_telephon_add_on_pkg_file=telephony_add_on_pkg.txt

# Package control file
MTK_RSC_COPY_FILES += \
        device/mediatek/system/common/telephony_add_on_pkg.txt:SYSTEM:telephony_add_on_pkg.txt

#Enable IMS debug properties for telephony add-on disabled.
ifneq ($(wildcard vendor/mediatek/internal/ims_enable),)
    ifeq ($(strip $(MSSI_MTK_IMS_SUPPORT)), yes)
        MTK_RSC_SYSTEM_PROPERTIES += persist.dbg.wfc_avail_ovr=1
        MTK_RSC_SYSTEM_PROPERTIES += persist.dbg.volte_avail_ovr=1
        MTK_RSC_SYSTEM_PROPERTIES += persist.dbg.vt_avail_ovr=1
        MTK_RSC_SYSTEM_PROPERTIES += ro.vendor.mtk_ims_notification=1
    endif
else
    MTK_RSC_SYSTEM_PROPERTIES += persist.dbg.wfc_avail_ovr=0
    MTK_RSC_SYSTEM_PROPERTIES += persist.dbg.volte_avail_ovr=0
    MTK_RSC_SYSTEM_PROPERTIES += persist.dbg.vt_avail_ovr=0
    MTK_RSC_SYSTEM_PROPERTIES += ro.vendor.mtk_ims_notification=0
endif

# AOSP APK
MTK_RSC_APKS += CarrierConfig:PRODUCT:priv-app
MTK_RSC_APKS += Dialer:PRODUCT:priv-app
MTK_RSC_APKS += SystemUI:PRODUCT:priv-app
MTK_RSC_APKS += TeleService:SYSTEM:priv-app
MTK_RSC_APKS += TelephonyProvider:SYSTEM:priv-app
MTK_RSC_APKS += MmsService:SYSTEM:priv-app
MTK_RSC_APKS += messaging:SYSTEM:priv-app
MTK_RSC_APKS += ContactsProvider:SYSTEM:priv-app
MTK_RSC_APKS += Telecom:SYSTEM:priv-app
MTK_RSC_APKS += Contacts:PRODUCT:priv-app
MTK_RSC_APKS += Settings:PRODUCT:priv-app
MTK_RSC_APKS += CellBroadcastReceiver:SYSTEM:priv-app
MTK_RSC_APKS += Stk:SYSTEM:priv-app
