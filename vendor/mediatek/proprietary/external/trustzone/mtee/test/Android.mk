LOCAL_PATH := $(call my-dir)

-include $(MTK_PATH_SOURCE)/trustzone/mtk/Setting.mk

include $(CLEAR_VARS)
LOCAL_MODULE := uree_test
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_SRC_FILES := test.c
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include \
    $(MTK_KERNEL_HEADERS)

LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Werror

LOCAL_MODULE_STEM_32 := uree_test32
LOCAL_MODULE_STEM_64 := $(LOCAL_MODULE)
LOCAL_MULTILIB := both

ifeq ($(TZ_ENABLE_TOMCRYPT_TEST),true)
LOCAL_CFLAGS += -DENABLE_TOMCRYPT_TEST
endif
LOCAL_SHARED_LIBRARIES += libtz_uree
LOCAL_SHARED_LIBRARIES += liblog
include $(MTK_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := uree_rng_test
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_SRC_FILES := uree_rng_test.c
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include \
    $(MTK_KERNEL_HEADERS)
LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Werror
LOCAL_SHARED_LIBRARIES += libtz_uree
include $(MTK_EXECUTABLE)
