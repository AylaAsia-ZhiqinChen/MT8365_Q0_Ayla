LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_PRIVILEGED_MODULE := true
LOCAL_CERTIFICATE := platform

LOCAL_JAVA_LIBRARIES += telephony-common \
	mediatek-framework

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_JNI_SHARED_LIBRARIES += libSerialPort
LOCAL_PACKAGE_NAME := FactoryTest
LOCAL_PRIVILEGED_MODULE := true
LOCAL_DEX_PREOPT=false

#add by yuntian andy begin
CUST_MANIFEST_FILE=device/mediateksample/$(MTK_TARGET_PROJECT)/overlay/yuntian/packages/apps/FactoryTest/AndroidManifest.xml
ifeq ($(CUST_MANIFEST_FILE), $(wildcard $(CUST_MANIFEST_FILE)))
LOCAL_MANIFEST_FILE = ../../../../$(CUST_MANIFEST_FILE)
endif
#add by yuntian andy end

include $(BUILD_PACKAGE)


