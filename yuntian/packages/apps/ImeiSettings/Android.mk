LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_PRIVILEGED_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-java-files-under,src)

LOCAL_PACKAGE_NAME := ImeiSettings
#LOCAL_PRIVILEGED_MODULE := true
LOCAL_JAVA_LIBRARIES := telephony-common mediatek-framework

include $(BUILD_PACKAGE)
