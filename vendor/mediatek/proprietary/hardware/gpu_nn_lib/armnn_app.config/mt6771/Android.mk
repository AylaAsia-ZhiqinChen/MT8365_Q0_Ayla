LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/hardware/gpu_nn))
include $(CLEAR_VARS)
LOCAL_MODULE := armnn_app.config
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/etc
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := arm/armnn_app.config
include $(BUILD_PREBUILT)
endif
