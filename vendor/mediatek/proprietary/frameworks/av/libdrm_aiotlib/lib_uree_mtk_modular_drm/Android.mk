LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/frameworks/av/libdrm))
include $(CLEAR_VARS)
LOCAL_MODULE := lib_uree_mtk_modular_drm
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_SUFFIX := .so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES_64 := libion libion_mtk libtz_uree libc
LOCAL_MULTILIB := 64
LOCAL_SRC_FILES_64 := arm64_$(TARGET_ARCH_VARIANT)_$(TARGET_CPU_VARIANT)/lib_uree_mtk_modular_drm.so
include $(BUILD_PREBUILT)
endif

ifeq (,$(wildcard vendor/mediatek/proprietary/frameworks/av/libdrm))
include $(CLEAR_VARS)
LOCAL_MODULE := lib_uree_mtk_modular_drm
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_SUFFIX := .so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := libion libion_mtk libtz_uree libc
LOCAL_MULTILIB := 32
LOCAL_SRC_FILES_32 := arm_$(TARGET_ARCH_VARIANT)_$(TARGET_CPU_VARIANT)/lib_uree_mtk_modular_drm.so
include $(BUILD_PREBUILT)
endif
