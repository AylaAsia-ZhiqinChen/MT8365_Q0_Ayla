LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# We only want this apk build for tests.
LOCAL_MODULE_TAGS := tests

# Include all test java files.
LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_JAVA_LIBRARIES := android.test.runner android.test.base
LOCAL_STATIC_JAVA_LIBRARIES := junit legacy-android-test
#LOCAL_PACKAGE_NAME := DownloadPublicApiAccessTests
LOCAL_PACKAGE_NAME := MtkDownloadPublicApiAccessTests
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_OVERRIDES_PACKAGES := DownloadPublicApiAccessTests

include $(BUILD_PACKAGE)

