LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/hardware/cpu_nn))
include $(CLEAR_VARS)
LOCAL_MODULE := libnir_neon_driver
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_SUFFIX := .so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib64
LOCAL_SHARED_LIBRARIES_64 := libbase liblog libcutils libc++ libc libm libdl
LOCAL_MULTILIB := 64
LOCAL_SRC_FILES_64 := arm64/libnir_neon_driver.so
include $(BUILD_PREBUILT)
endif

ifeq (,$(wildcard vendor/mediatek/proprietary/hardware/cpu_nn))
include $(CLEAR_VARS)
LOCAL_MODULE := libnir_neon_driver
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_SUFFIX := .so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib
LOCAL_SHARED_LIBRARIES := libbase liblog libcutils libc++ libc libm libdl
LOCAL_MULTILIB := 32
LOCAL_SRC_FILES_32 := arm/libnir_neon_driver.so
include $(BUILD_PREBUILT)
endif
