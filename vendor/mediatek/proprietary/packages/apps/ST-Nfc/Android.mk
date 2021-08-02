ifneq ($(wildcard vendor/mediatek/proprietary/hardware/stnfc),)
ifeq ($(strip $(NFC_CHIP_SUPPORT)),yes)
LOCAL_PATH:= $(call my-dir)

#########################################
## ST Configuration
#########################################
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
        $(call all-java-files-under, src)

LOCAL_SRC_FILES += \
        $(call all-java-files-under, st)

LOCAL_SRC_FILES += \
        $(call all-java-files-under, st_dta)

LOCAL_SRC_FILES += \
        nci/src/com/android/nfc/dhimpl/NativeLlcpConnectionlessSocket.java \
        nci/src/com/android/nfc/dhimpl/NativeLlcpServiceSocket.java \
        nci/src/com/android/nfc/dhimpl/NativeLlcpSocket.java \
        nci/src/com/android/nfc/dhimpl/NativeP2pDevice.java


LOCAL_PACKAGE_NAME := Nfc_st
LOCAL_MODULE_OWNER := mtk
LOCAL_OVERRIDES_PACKAGES := Nfc NfcNci
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_CERTIFICATE := platform

LOCAL_JAVA_LIBRARIES := com.st.android.nfc_extensions
LOCAL_JNI_SHARED_LIBRARIES  := libstnfc_nci_jni libnfc_st_dta_jni

LOCAL_STATIC_JAVA_LIBRARIES := android-support-v4
LOCAL_STATIC_JAVA_LIBRARIES += nfc_settings_adapter

LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
endif
