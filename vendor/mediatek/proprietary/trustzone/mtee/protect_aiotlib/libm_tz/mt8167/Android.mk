LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/trustzone/mtee/protect))
include $(CLEAR_VARS)
LOCAL_MODULE := libm_tz
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_SUFFIX := .a
LOCAL_PROPRIETARY_MODULE := true
LOCAL_UNINSTALLABLE_MODULE := true
LOCAL_MULTILIB := 64
LOCAL_SRC_FILES_64 := arm64/libm_tz.a
include $(BUILD_PREBUILT)
endif

ifeq (,$(wildcard vendor/mediatek/proprietary/trustzone/mtee/protect))
include $(CLEAR_VARS)
LOCAL_MODULE := libm_tz
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_SUFFIX := .a
LOCAL_PROPRIETARY_MODULE := true
LOCAL_UNINSTALLABLE_MODULE := true
LOCAL_MULTILIB := 32
LOCAL_SRC_FILES_32 := arm/libm_tz.a
include $(BUILD_PREBUILT)
endif
