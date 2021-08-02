LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wall -Werror

LOCAL_SRC_FILES:= su.cpp

LOCAL_MODULE:= su

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
# yuntian longyao add
# Description:app root权限
LOCAL_MODULE_TAGS := optional

# yuntian longyao end
include $(BUILD_EXECUTABLE)
