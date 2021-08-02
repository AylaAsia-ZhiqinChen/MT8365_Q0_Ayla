LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_AAPT_INCLUDE_ALL_RESOURCES := true

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_PACKAGE_NAME := NonFrameworkLbs
LOCAL_SDK_VERSION := system_current
LOCAL_CERTIFICATE := platform
#yuntian longyao add
CUST_MANIFEST_FILE=device/mediateksample/$(MTK_TARGET_PROJECT)/overlay/vendor/mediatek/proprietary/packages/apps/GnssVisibilityControl/NonFrameworkLbs/AndroidManifest.xml
ifeq ($(CUST_MANIFEST_FILE), $(wildcard $(CUST_MANIFEST_FILE)))
LOCAL_MANIFEST_FILE = ../../../../../../../$(CUST_MANIFEST_FILE)
endif
#yuntian longyao end

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))

