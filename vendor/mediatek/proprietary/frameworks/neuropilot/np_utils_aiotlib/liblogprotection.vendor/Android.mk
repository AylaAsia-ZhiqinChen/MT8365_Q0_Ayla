LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/frameworks/neuropilot/np_utils))
include $(CLEAR_VARS)
LOCAL_MODULE := liblogprotection.vendor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib64
LOCAL_MODULE_STEM := liblogprotection
LOCAL_SHARED_LIBRARIES_64 := libcutils libc++ libc libm libdl
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
LOCAL_MULTILIB := 64
LOCAL_SRC_FILES_64 := arm64_$(TARGET_ARCH_VARIANT)_$(TARGET_CPU_VARIANT)/liblogprotection.so
include $(BUILD_PREBUILT)
endif

ifeq (,$(wildcard vendor/mediatek/proprietary/frameworks/neuropilot/np_utils))
include $(CLEAR_VARS)
LOCAL_MODULE := liblogprotection.vendor
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib
LOCAL_MODULE_STEM := liblogprotection
LOCAL_SHARED_LIBRARIES := libcutils libc++ libc libm libdl
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
LOCAL_MULTILIB := 32
LOCAL_SRC_FILES_32 := arm_$(TARGET_ARCH_VARIANT)_$(TARGET_CPU_VARIANT)/liblogprotection.so
include $(BUILD_PREBUILT)
endif
