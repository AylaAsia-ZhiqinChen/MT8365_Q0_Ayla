LOCAL_PATH := $(call my-dir)

ifeq (,$(wildcard vendor/mediatek/proprietary/hardware/keymaster_mtee))
include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.keymaster@4.0-service.mtee
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SHARED_LIBRARIES := liblog libcutils libbase libdl libutils libhardware libhidlbase libhidltransport libkeymaster4support libkeymaster4 libcrypto libkeymaster_messages android.hardware.keymaster@4.0 libtz_uree
LOCAL_INIT_RC := android.hardware.keymaster@4.0-service.mtee.rc
LOCAL_SRC_FILES := $(call get-prebuilt-src-arch,arm arm64)_$(TARGET_ARCH_VARIANT)_$(TARGET_CPU_VARIANT)/android.hardware.keymaster@4.0-service.mtee
include $(BUILD_PREBUILT)
endif
