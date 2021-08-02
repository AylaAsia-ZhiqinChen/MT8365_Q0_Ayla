# These paths are tweaked by release script
ifeq ($(strip $(NFC_CHIP_SUPPORT)), yes)
JNI_COMPONENTS := ../includes/packages

LOCAL_PATH:= $(call my-dir)

STFACTORY_CFLAGS := -Wall -Wextra -DST21NFC -DST21NFC_DTA -fvisibility=hidden

# Overwrite the MW version information here if you need. It s a 18-chars string.
# LOCAL_CFLAGS += -DST_MTKTOOLS_VERSION="ST 1.0.0-20161231"

STFACTORY_SRC_FILES :=  \
	st_mt_daemon.cpp  \
	st_mt_glue.cpp  \
	st_mt_loop.cpp  \
	st_mt_server.cpp

STFACTORY_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	liblog \
	libchrome \
    	libbase \

STFACTORY_STATIC_LIBRARIES := libxml2

# // START vendor version
#########################
# Vendor version
#########################
include $(CLEAR_VARS)

LOCAL_CFLAGS += $(STFACTORY_CFLAGS) -DVENDOR_VERSION

LOCAL_SRC_FILES += $(STFACTORY_SRC_FILES)
LOCAL_SHARED_LIBRARIES := $(STFACTORY_SHARED_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(STFACTORY_STATIC_LIBRARIES)

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE := nfcstackp-vendor
LOCAL_MODULE_TAGS := optional

LOCAL_HEADER_LIBRARIES := \
    jni_headers \
    libhardware_headers \

LOCAL_STATIC_LIBRARIES += \
    libstnfc-nci-vendor \
    libstnfcutils-vendor \
    nfc_nci.st21nfc.static \
    nfc_jni_files_for_factory_vendor \

LOCAL_SHARED_LIBRARIES += \
    libdl \
    libhardware \
    libpower \
    libz \
    libhidlbase \


include $(BUILD_EXECUTABLE)
# ---------Just for Test----------
include $(CLEAR_VARS)

LOCAL_SRC_FILES := st_factorytests.cpp

LOCAL_CFLAGS += $(STFACTORY_CFLAGS) -DVENDOR_VERSION

LOCAL_HEADER_LIBRARIES := \
    jni_headers \
    libhardware_headers \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libchrome \
    libbase \

# Just for the JNI headers, code will be stripped anyway.
LOCAL_STATIC_LIBRARIES += \
    nfc_jni_files_for_factory_vendor \

LOCAL_MODULE := st_factorytests-vendor
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

include $(BUILD_EXECUTABLE)

endif
# // END vendor version

