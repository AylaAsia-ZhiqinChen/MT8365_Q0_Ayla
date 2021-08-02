# Cannot convert to Android.bp as resource copying has not
# yet implemented for soong as of 12/16/2016
LOCAL_PATH := $(call my-dir)

ifeq ($(MSSI_MTK_BT_HOST_SNOOP_LOG_SUPPORT), yes)
# Bluetooth stack conf file
# ========================================================
include $(CLEAR_VARS)
LOCAL_MODULE := mtk_bt_stack.conf
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/bluetooth
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# Bluetooth controller conf file
# ========================================================
include $(CLEAR_VARS)
LOCAL_MODULE := mtk_bt_fw.conf
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/bluetooth
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif
