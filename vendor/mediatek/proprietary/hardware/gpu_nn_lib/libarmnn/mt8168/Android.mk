LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/hardware/gpu_nn))
include $(CLEAR_VARS)
LOCAL_MODULE := libarmnn
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib64
LOCAL_SHARED_LIBRARIES_64 := liblog libcutils libcmdl libc++ libc libm libdl
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/armnn/include/mtk/wrapper
LOCAL_MULTILIB := 64
LOCAL_SRC_FILES_64 := arm64/libarmnn.so
include $(BUILD_PREBUILT)
endif

ifeq (,$(wildcard vendor/mediatek/proprietary/hardware/gpu_nn))
include $(CLEAR_VARS)
LOCAL_MODULE := libarmnn
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib
LOCAL_SHARED_LIBRARIES := liblog libcutils libcmdl libc++ libc libm libdl
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/armnn/include/mtk/wrapper
LOCAL_MULTILIB := 32
LOCAL_SRC_FILES_32 := arm/libarmnn.so
include $(BUILD_PREBUILT)
endif
