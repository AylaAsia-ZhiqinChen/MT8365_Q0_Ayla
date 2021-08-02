LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/trustzone/mtee/protect))
include $(CLEAR_VARS)
LOCAL_MODULE := trustzone-wv
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/trustzone/bin
LOCAL_MODULE_STEM := tz-wv.img
LOCAL_SRC_FILES := arm/tz-wv.img
include $(BUILD_PREBUILT)
endif
