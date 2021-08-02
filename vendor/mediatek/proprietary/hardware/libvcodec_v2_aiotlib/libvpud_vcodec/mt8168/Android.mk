LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/hardware/libvcodec_v2))
include $(CLEAR_VARS)
LOCAL_MODULE := libvpud_vcodec
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_SUFFIX := .so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SHARED_LIBRARIES := libcutils libvcodecdrv libvcodec_utility liblog libion libion_mtk
LOCAL_MULTILIB := 32
LOCAL_SRC_FILES_32 := arm/libvpud_vcodec.so
include $(BUILD_PREBUILT)
endif
