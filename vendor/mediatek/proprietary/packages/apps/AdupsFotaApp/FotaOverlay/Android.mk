ifeq ($(strip $(ADUPS_FOTA_SUPPORT_OVERLAY)), yes)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PACKAGE_NAME := FotaOverlay
LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_SRC_FILES := $(call all-java-files-under, res)
LOCAL_SDK_VERSION := current
LOCAL_IS_RUNTIME_RESOURCE_OVERLAY := true
ifeq ($(LOCAL_RRO_THEME),)
  LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/overlay
else
  LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/overlay/$(LOCAL_RRO_THEME)
endif
include $(BUILD_PACKAGE)

endif
