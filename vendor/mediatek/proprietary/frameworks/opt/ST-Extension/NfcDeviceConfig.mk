# Device configuration file to be included from system device.mk
#   include vendor/mediatek/proprietary/frameworks/opt/ST-Extension/frameworks-vendor/NfcDeviceConfig.mk

# If using ST's dual-implementation SecureElement service, the following
# property controls the channel to use with eSE. Default is SPI.
# PRODUCT_PROPERTY_OVERRIDES += persist.st_use_apdu_gate_ese=1

################################################
## Configuration for ST NFC packages
PRODUCT_PACKAGES += \
    libstnfc-nci \
    libstnfc_nci_jni \
    Nfc_st \
    com.st.android.nfc_extensions \
    Tag

PRODUCT_COPY_FILES += \
   frameworks/native/data/etc/com.nxp.mifare.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/com.nxp.mifare.xml:mtk \
   vendor/mediatek/proprietary/frameworks/opt/ST-Extension/frameworks/NfcExtensions/com.st.android.nfc_extensions.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/com.st.android.nfc_extensions.xml:mtk

# To keep P2P support :
# PRODUCT_COPY_FILES += vendor/mediatek/proprietary/frameworks/opt/ST-Extension/conf/android.sofware.nfc.beam.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/android.sofware.nfc.beam.xml:mtk

# To support APDU Gate:
PRODUCT_PACKAGES += \
    com.android.nfc_extras \
    com.android.nfc_extras.xml
PRODUCT_COPY_FILES += \
   vendor/mediatek/proprietary/frameworks/opt/ST-Extension/conf/nfcee_access.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/nfcee_access.xml:mtk

# Init.rc files
PRODUCT_COPY_FILES += \
   vendor/mediatek/proprietary/frameworks/opt/ST-Extension/conf/init.system.st21nfc.rc:$(TARGET_COPY_OUT_SYSTEM)/etc/init/hw/init.stnfc.rc:mtk


################################################
## EM support
PRODUCT_PACKAGES += \
   nfcstackp \
   st_factorytests

################################################
## NFC Forum Digital support
PRODUCT_PACKAGES += \
   libnfc_st_dta_jni \
   libdtaNfaProvider \
   libnfc_st_dta \
   STNFCDta \
   ixitdata.xml
