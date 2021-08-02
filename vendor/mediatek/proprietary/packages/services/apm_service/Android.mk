LOCAL_PATH:= $(call my-dir)

# APM service interface library, static and system
include $(CLEAR_VARS)
LOCAL_SRC_FILES := src/com/mediatek/apmonitor/IApmKpiMonitor.java
LOCAL_MODULE := apm-service-defs
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_PROPRIETARY_MODULE := false
include $(BUILD_STATIC_JAVA_LIBRARY)

# APM service APK
include $(CLEAR_VARS)
LOCAL_PACKAGE_NAME := ApmService
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_STATIC_JAVA_LIBRARIES := \
    android-support-v4 \
    mediatek-framework-net

# APM HIDL Interface
LOCAL_STATIC_JAVA_LIBRARIES += \
    apm-msg-defs \
    vendor.mediatek.hardware.apmonitor-V2.0-java

LOCAL_JAVA_LIBRARIES := \
    mediatek-common

LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_PROPRIETARY_MODULE := false
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_MODULE_OWNER := mtk

include $(BUILD_PACKAGE)