LOCAL_PATH:= $(call my-dir)

# This is used for building out an library for omacp plugin share
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
#LOCAL_JAVA_LIBRARIES += mediatek-framework telephony-common
LOCAL_JAVA_LIBRARIES := mediatek-common
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_MODULE := com.mediatek.omacp.ext
LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_FLAG_FILES := proguard.flags
include $(BUILD_STATIC_JAVA_LIBRARY)
