ifneq ($(strip $(TARGET_BUILD_VARIANT)),user)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#-----------------------------------------------------------
LOCAL_SRC_FILES += camtool.cpp

#-----------------------------------------------------------
LOCAL_CFLAGS += $(MTKCAM_CFLAGS)

LOCAL_SHARED_LIBRARIES += \
    liblog \
    libcutils \
    libutils

LOCAL_WHOLE_STATIC_LIBRARIES +=

LOCAL_STATIC_LIBRARIES +=

#-----------------------------------------------------------
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := camtool
LOCAL_MODULE_OWNER := mtk
LOCAL_PROPRIETARY_MODULE := false

#-----------------------------------------------------------
include $(MTK_EXECUTABLE)
endif
