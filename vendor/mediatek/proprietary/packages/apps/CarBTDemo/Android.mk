LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)


LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-subdir-java-files)

#LOCAL_JAVA_LIBRARIES := mediatek-common
#LOCAL_JAVA_LIBRARIES += mediatek-framework
LOCAL_JAVA_LIBRARIES := javax.obex

#LOCAL_STATIC_JAVA_LIBRARIES := android.bluetooth.client.pbap
LOCAL_STATIC_JAVA_LIBRARIES := \
	libarity \
	com.android.vcard \
	android-support-v13 \
	android-support-v4 \
	jsr305\
	androidx.annotation_annotation
	

LOCAL_PACKAGE_NAME := CarBTDemo
LOCAL_CERTIFICATE := platform
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_PRIVILEGED_MODULE := true

include $(BUILD_PACKAGE)
include $(CLEAR_VARS)
LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := libarity:lib/pinyin4j-2.5.0.jar
include $(BUILD_MULTI_PREBUILT)
########################
#include $(call all-makefiles-under,$(LOCAL_PATH))
