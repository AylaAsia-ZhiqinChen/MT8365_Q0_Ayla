# Copyright 2007-2008 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CERTIFICATE := platform

LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
#LOCAL_MODULE_PATH=$(TARGET_OUT_VENDOR)/overlay
LOCAL_PACKAGE_NAME := OP02StkOverlay
LOCAL_SDK_VERSION := current

 include $(BUILD_RRO_PACKAGE)
