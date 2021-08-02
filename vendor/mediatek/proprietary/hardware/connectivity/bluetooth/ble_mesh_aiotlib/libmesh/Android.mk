LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/hardware/connectivity/bluetooth/ble_mesh))
include $(CLEAR_VARS)
LOCAL_MODULE := libmesh
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib64
LOCAL_SHARED_LIBRARIES_64 := libcutils liblog libutils libc++ libc libm libdl
LOCAL_MULTILIB := 64
LOCAL_SRC_FILES_64 := arm64_$(TARGET_ARCH_VARIANT)_$(TARGET_CPU_VARIANT)/libmesh.so
include $(BUILD_PREBUILT)
endif

ifeq (,$(wildcard vendor/mediatek/proprietary/hardware/connectivity/bluetooth/ble_mesh))
include $(CLEAR_VARS)
LOCAL_MODULE := libmesh
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
LOCAL_SHARED_LIBRARIES := libcutils liblog libutils libc++ libc libm libdl
LOCAL_MULTILIB := 32
LOCAL_SRC_FILES_32 := arm_$(TARGET_ARCH_VARIANT)_$(TARGET_CPU_VARIANT)/libmesh.so
include $(BUILD_PREBUILT)
endif
