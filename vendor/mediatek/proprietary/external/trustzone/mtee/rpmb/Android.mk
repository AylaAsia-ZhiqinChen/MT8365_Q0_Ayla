LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := rpmb_svc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_SRC_FILES := rpmb_svc.c \
                   rpmb_api.c \
                   uree_rpmb.c

LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include \
	system/core/include

LOCAL_HEADER_LIBRARIES := libcutils_headers
LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Werror

LOCAL_SHARED_LIBRARIES += libtz_uree
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libcutils

LOCAL_INIT_RC := rpmb_svc.rc
include $(MTK_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := rpmb_test
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

LOCAL_SRC_FILES := rpmb_api.c \
		   rpmb_test.c \
		   uree_rpmb.c

LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include

LOCAL_SHARED_LIBRARIES += libtz_uree
LOCAL_SHARED_LIBRARIES += liblog
include $(MTK_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := rpmb_key_sts
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

LOCAL_SRC_FILES := rpmb_api.c \
                   rpmb_key_sts.c \

LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include

LOCAL_SHARED_LIBRARIES += liblog
include $(MTK_EXECUTABLE)
