LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := mtee_gcpu_test
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_SRC_FILES := mtee_gcpu_test.c
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include

LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Werror

#LOCAL_CFLAGS += ${TZ_CFLAG}
#LOCAL_LDFLAGS += --gc-sections
#LOCAL_ASFLAGS += -DASSEMBLY
LOCAL_SHARED_LIBRARIES += libtz_uree
include $(MTK_EXECUTABLE)

