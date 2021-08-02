LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/frameworks/opt/libimsma_bin))
include $(CLEAR_VARS)
LOCAL_MODULE := libvcodec_capenc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES_64 := liblog
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
LOCAL_MULTILIB := 64
LOCAL_SRC_FILES_64 := arm64_$(TARGET_ARCH_VARIANT)_$(TARGET_CPU_VARIANT)/libvcodec_capenc.so
include $(BUILD_PREBUILT)
endif

ifeq (,$(wildcard vendor/mediatek/proprietary/frameworks/opt/libimsma_bin))
include $(CLEAR_VARS)
LOCAL_MODULE := libvcodec_capenc
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
LOCAL_MULTILIB := 32
LOCAL_SRC_FILES_32 := arm_$(TARGET_ARCH_VARIANT)_$(TARGET_CPU_VARIANT)/libvcodec_capenc.so
include $(BUILD_PREBUILT)
endif
