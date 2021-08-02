LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libtz_uree
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := uree.c \
                   uree_mem.c \
                   dump_sec_mem.c \
                   uree_dbg.c
LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include \
	system/core/include \
    $(word 1,$(TARGET_BOARD_KERNEL_HEADERS))

LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Werror
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_HEADER_LIBRARIES := libcutils_headers
#LOCAL_CFLAGS += ${TZ_CFLAG}
#LOCAL_LDFLAGS += --gc-sections
#LOCAL_ASFLAGS += -DASSEMBLY
#LOCAL_STATIC_LIBRARIES += libc_tz libtest
include $(MTK_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := libtz_uree_sys
#LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := uree.c \
                   uree_mem.c \
                   dump_sec_mem.c \
                   uree_dbg.c

LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/trustzone/mtee/include \
    $(word 1,$(TARGET_BOARD_KERNEL_HEADERS))

LOCAL_CFLAGS += -Wall -Wno-unused-parameter -Werror
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_HEADER_LIBRARIES := libcutils_headers
#LOCAL_CFLAGS += ${TZ_CFLAG}
#LOCAL_LDFLAGS += --gc-sections
#LOCAL_ASFLAGS += -DASSEMBLY
#LOCAL_STATIC_LIBRARIES += libc_tz libtest
include $(MTK_SHARED_LIBRARY)

