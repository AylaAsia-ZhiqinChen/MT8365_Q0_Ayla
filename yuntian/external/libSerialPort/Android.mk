LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_LDLIBS    := -llog
LOCAL_SRC_FILES := SerialPort.c

LOCAL_MODULE := libSerialPort

include $(BUILD_SHARED_LIBRARY)

