# Device configuration file to be included from vendor device.mk file, e.g.
#
# ifeq ($(strip $(NFC_CHIP_SUPPORT)), yes)
#   ifneq ($(wildcard $(MTK_TARGET_PROJECT_FOLDER)/script_DB10mtk*),)
#     NFC_RF_CONFIG_PATH := $(MTK_TARGET_PROJECT_FOLDER)
#   else
#     NFC_RF_CONFIG_PATH := device/mediatek/$(MTK_PLATFORM_DIR)
#   endif
#   include vendor/mediatek/proprietary/frameworks/opt/ST-Extension/frameworks-vendor/NfcDeviceConfigVendor.mk
# endif

ifeq ($(strip $(NFC_CHIP_SUPPORT)), yes)
DEVICE_MANIFEST_FILE += device/mediatek/vendor/common/project_manifest/manifest_nfc.xml

################################################
## Configuration for ST NFC packages
PRODUCT_PACKAGES += \
    android.hardware.nfc@1.2-service-st \
    nfc_nci.st21nfc.st \
    STFlashTool

PRODUCT_COPY_FILES += \
   frameworks/native/data/etc/android.hardware.nfc.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.xml:mtk \
   frameworks/native/data/etc/android.hardware.nfc.hce.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.hce.xml:mtk \
   frameworks/native/data/etc/android.hardware.nfc.hcef.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.hcef.xml:mtk \
   frameworks/native/data/etc/android.hardware.nfc.uicc.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.nfc.uicc.xml:mtk \

# Stack configuration files (common for ST stack and AOSP stack)
ifneq ($(strip $(TARGET_BUILD_VARIANT)),user)
   PRODUCT_COPY_FILES += \
      vendor/mediatek/proprietary/external/stnfctools-vendor/conf/libnfc-nci.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nci.conf:mtk
   ifeq ($(strip $(ST_NFC_CHIP_VERSION)),ST54J)
      PRODUCT_COPY_FILES += \
         vendor/mediatek/proprietary/external/stnfctools-vendor/conf/libnfc-hal-st.conf.st54j:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-hal-st.conf:mtk
   else
      PRODUCT_COPY_FILES += \
         vendor/mediatek/proprietary/external/stnfctools-vendor/conf/libnfc-hal-st.conf:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-hal-st.conf:mtk
   endif
else
  # Configuration files for user build, remove some logs for GSMA certif
   PRODUCT_COPY_FILES += \
      vendor/mediatek/proprietary/external/stnfctools-vendor/conf/libnfc-nci.conf.user:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-nci.conf:mtk
   ifeq ($(strip $(ST_NFC_CHIP_VERSION)),ST54J)
      PRODUCT_COPY_FILES += \
         vendor/mediatek/proprietary/external/stnfctools-vendor/conf/libnfc-hal-st.conf.st54j.user:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-hal-st.conf:mtk
   else
      PRODUCT_COPY_FILES += \
         vendor/mediatek/proprietary/external/stnfctools-vendor/conf/libnfc-hal-st.conf.user:$(TARGET_COPY_OUT_VENDOR)/etc/libnfc-hal-st.conf:mtk
   endif
endif

# Init.rc files
PRODUCT_COPY_FILES += \
   vendor/mediatek/proprietary/external/stnfctools-vendor/conf/init.vendor.st21nfc.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.stnfc.rc:mtk

# Copy the correct firmware and parameters of the ST21NFCD depending on the hardware layout.
ifeq ($(strip $(ST_NFC_CHIP_VERSION)),ST54J)
   PRODUCT_PACKAGES += st54j_firmware
   PRODUCT_COPY_FILES += \
      $(NFC_RF_CONFIG_PATH)/script_st54j.txt:$(TARGET_COPY_OUT_VENDOR)/etc/script_st54j.txt:mtk
else ifeq ($(strip $(ST_NFC_CHIP_VERSION)),ST54H)
   PRODUCT_PACKAGES += st21nfcd_firmware
   PRODUCT_COPY_FILES += \
      $(NFC_RF_CONFIG_PATH)/script_DB10mtk_UICC_eSE.txt:$(TARGET_COPY_OUT_VENDOR)/etc/st21nfc_conf.txt:mtk
else ifeq ($(strip $(ST_NFC_CHIP_VERSION)),ST21NFCD)
   PRODUCT_PACKAGES += st21nfcd_firmware
   PRODUCT_COPY_FILES += \
      $(NFC_RF_CONFIG_PATH)/script_DB10mtk_2UICC.txt:$(TARGET_COPY_OUT_VENDOR)/etc/st21nfc_conf.txt:mtk
else
   $(error ST_NFC_CHIP_VERSION "$(ST_NFC_CHIP_VERSION)" is not supported)
endif

################################################
## FM support
PRODUCT_PACKAGES += \
   nfcstackp-vendor \
   st_factorytests-vendor
   
endif
