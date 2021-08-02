#ifeq ($(MTK_DRM_KEY_MNG_SUPPORT), yes)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := liburee_meta_drmkeyinstall_v2
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_SRC_FILES := drmkey_ree.c

LOCAL_C_INCLUDES += \
    $(call include-path-for, trustzone-uree) \
    $(MTK_PATH_SOURCE)/hardware/meta/common/inc \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include/tz_cross \
    $(TOP)/system/core/base/include

LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Werror -DLTM_DESC -Wall
#LOCAL_CFLAGS += -Wno-unused-parameter -Werror -DLTM_DESC
#LOCAL_CFLAGS := $(filter-out -Werror=implicit-function-declaration, $(LOCAL_CFLAGS))

LOCAL_SHARED_LIBRARIES += libcutils libnetutils libc liblog
LOCAL_SHARED_LIBRARIES += libtz_uree

LOCAL_MULTILIB := both

LOCAL_PRELINK_MODULE := false
include $(MTK_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := liburee_meta_drmkey_if
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_SRC_FILES := meta_drmkey_install.c

LOCAL_C_INCLUDES += \
    $(call include-path-for, trustzone-uree) \
    $(MTK_PATH_SOURCE)/hardware/meta/common/inc \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include/tz_cross \
    $(TOP)/system/core/base/include

LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Werror -DLTM_DESC -Wall
#LOCAL_CFLAGS := $(filter-out -Werror=implicit-function-declaration, $(LOCAL_CFLAGS))

LOCAL_SHARED_LIBRARIES += libcutils liblog libnetutils libc liburee_meta_drmkeyinstall_v2

LOCAL_MULTILIB := both

LOCAL_PRELINK_MODULE := false
include $(MTK_STATIC_LIBRARY)

#endif
