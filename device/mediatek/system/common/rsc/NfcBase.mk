
# ===================  NFC APKs =====================================
MTK_RSC_APKS += \
    Nfc_st:SYSTEM:app \
    Tag:SYSTEM:priv-app

MTK_RSC_MODULES += \
    libstnfc-nci \
    libstnfc_nci_jni \
    com.st.android.nfc_extensions \
# ===================  NFC APKs =====================================

# ===================  NFC EM Support ===============================
MTK_RSC_MODULES += \
    nfcstackp \
    st_factorytests \
# ===================  NFC EM Support ===============================

# ===================  NFC Forum Digital support ====================
MTK_RSC_APKS += \
    STNFCDta:SYSTEM:app \

MTK_RSC_MODULES += \
    libnfc_st_dta_jni \
    com.st.android.nfc_dta_extensions \
    libdtaNfaProvider \
    libnfc_st_dta \
    ixitdata.xml \
# ===================  NFC Forum Digital support ====================