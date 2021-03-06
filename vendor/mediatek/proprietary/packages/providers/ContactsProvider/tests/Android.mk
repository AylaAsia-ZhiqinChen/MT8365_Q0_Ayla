LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# We only want this apk build for tests.
LOCAL_MODULE_TAGS := tests

LOCAL_STATIC_JAVA_LIBRARIES := \
    MtkContactsProviderTestUtils \
    androidx.test.rules \
    mockito-target-minus-junit4

LOCAL_JAVA_LIBRARIES := android.test.runner android.test.base android.test.mock \
mediatek-telephony-base

# Only compile source java files in this apk.
LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := MtkContactsProviderTests
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_COMPATIBILITY_SUITE := device-tests

LOCAL_INSTRUMENTATION_FOR := MtkContactsProvider
LOCAL_CERTIFICATE := shared

LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)
