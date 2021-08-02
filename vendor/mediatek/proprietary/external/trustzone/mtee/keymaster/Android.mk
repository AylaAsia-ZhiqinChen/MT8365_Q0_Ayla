LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := keystore.$(TARGET_BOARD_PLATFORM)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_CFLAGS += -fvisibility=hidden -DLOG_TAG=\"KM_UREE\"
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
    mtee_keymaster.cpp  \
    keymaster_mtee_api.cpp \
    MTKKeymaster_hal.cpp

LOCAL_C_INCLUDES += \
    hardware/libhardware/include \
    system/keymaster/include \
    vendor/mediatek/proprietary/external/trustzone/mtee/include \

LOCAL_SHARED_LIBRARIES := \
    libcrypto \
    libcutils \
    libkeymaster_messages \
    liblog \
    libutils \
    libtz_uree

include $(MTK_SHARED_LIBRARY)

ifeq ($(MTK_KEYMASTER_TEST_SUPPORT),yes)
# Unit tests for mtee keymaster
include $(CLEAR_VARS)
LOCAL_MODULE := mtee_keymaster_tests
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_CFLAGS += -DLOG_TAG=\"KM_TEST\"
LOCAL_SRC_FILES := \
    test/keymaster_test.cpp \
    test/keymaster_test_utils.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/test \
    hardware/libhardware/include \

LOCAL_CLANG_CFLAGS += -Wno-error=unused-const-variable -Wno-error=unused-private-field
LOCAL_MODULE_TAGS := tests
LOCAL_SHARED_LIBRARIES := \
    libcrypto \
    libkeymaster_messages \
    libhardware \
    liblog \
    libutils \
    libcutils \
    libtz_uree

LOCAL_C_INCLUDES += \
    system/keymaster

LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
include $(BUILD_NATIVE_TEST)
endif
