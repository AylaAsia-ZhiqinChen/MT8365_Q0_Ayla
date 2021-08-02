ifeq ($(strip $(ADUPS_FOTA_SUPPORT)), yes)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := AdupsFota
LOCAL_MODULE_TAGS := optional
ifeq ($(strip $(ADUPS_FOTA_WITH_ICON)), yes)
LOCAL_SRC_FILES := ./withIcon/AdupsFota.apk
else
LOCAL_SRC_FILES := ./noIcon/AdupsFota.apk
endif
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
LOCAL_CERTIFICATE := platform
include $(BUILD_PREBUILT)
endif
