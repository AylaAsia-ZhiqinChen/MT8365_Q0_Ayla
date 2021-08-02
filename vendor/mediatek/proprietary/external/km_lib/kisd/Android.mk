#ifeq ($(MTK_DRM_KEY_MNG_SUPPORT), yes)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	kisd_main.c

LOCAL_SHARED_LIBRARIES := libcutils liblog libnetutils libc libtz_uree liburee_meta_drmkeyinstall_v2

LOCAL_C_INCLUDES += \
    $(call include-path-for, trustzone-uree) \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include/tz_cross \
    device/mediatek/system/core/include \
    device/mediatek/bionic/libc/include \
    $(TOP)/system/core/base/include

LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Werror -Wall

LOCAL_MODULE := libkisd
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

LOCAL_MULTILIB := both

include $(MTK_STATIC_LIBRARY)

#endif
