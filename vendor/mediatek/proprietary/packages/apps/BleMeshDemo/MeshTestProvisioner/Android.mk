LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

# Bluild all java files in the java subdirectory
LOCAL_SRC_FILES := $(call all-java-files-under, java)

ifeq ($(TARGET_BUILD_APPS),)
support_library_root_dir := frameworks/support
else
support_library_root_dir := prebuilts/sdk/current/support
endif

LOCAL_STATIC_JAVA_LIBRARIES := provisionerslideswitch androidtreeview android-support-v4 android-support-v7-appcompat provisionergson recyclerview

LOCAL_RESOURCE_DIR := $(addprefix $(LOCAL_PATH)/, res) \
    $(support_library_root_dir)/v7/appcompat/res


# if need use R file , need config "LOCAL_AAPT_FLAGS" attribute
LOCAL_AAPT_FLAGS := --auto-add-overlay \
              --extra-packages com.example.slideview \
              --extra-packages com.unnamed.b.atv \
              --extra-packages android.support.v7.appcompat

LOCAL_STATIC_JAVA_ARR_LIBRARIES := provisionerslideswitch \
                                 androidtreeview \
                                 provisionergson \
                                 recyclerview \

# Name of the APK to build
LOCAL_PACKAGE_NAME := MeshTestProvisioner

LOCAL_USE_AAPT2 := true

LOCAL_PRIVATE_PLATFORM_APIS := true

LOCAL_CERTIFICATE := platform

LOCAL_DEX_PREOPT := false

# Tell it to build an APK
include $(BUILD_PACKAGE)

##################################################
include $(CLEAR_VARS)

LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := provisionerslideswitch:libs/slideswitch.aar \
                                   androidtreeview:libs/androidtreeview.aar \
                                   provisionergson:libs/gson.jar \
                                   recyclerview:libs/recyclerview.jar \

include $(BUILD_MULTI_PREBUILT)

# Use the following include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))




