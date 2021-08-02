LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-java-files-under,src)

LOCAL_PACKAGE_NAME := AgingTest
#LOCAL_PRIVILEGED_MODULE := true
LOCAL_JAVA_LIBRARIES := mediatek-framework

CUST_MANIFEST_FILE=device/mediateksample/$(MTK_TARGET_PROJECT)/overlay/yuntian/packages/apps/AgingTest/AndroidManifest.xml
ifeq ($(CUST_MANIFEST_FILE), $(wildcard $(CUST_MANIFEST_FILE)))
LOCAL_MANIFEST_FILE = ../../../../$(CUST_MANIFEST_FILE)
endif

include $(BUILD_PACKAGE)

# Use the following include to make our test apk.
#include $(call all-makefiles-under,$(LOCAL_PATH))
