# These paths are tweaked by release script
ifeq ($(strip $(NFC_CHIP_SUPPORT)),yes)
JNI_COMPONENTS := ../../../packages/apps/ST-Nfc

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

# // START system version
include $(CLEAR_VARS)
#################################################
## Normal nfcstackp version (in system partition)
LOCAL_CFLAGS += $(STFACTORY_CFLAGS)
LOCAL_SRC_FILES += $(STFACTORY_SRC_FILES)
LOCAL_SHARED_LIBRARIES := $(STFACTORY_SHARED_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(STFACTORY_STATIC_LIBRARIES)

#LOCAL_MODULE := st_mtktools
LOCAL_PROPRIETARY_MODULE := false
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE := nfcstackp
LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES += \
	libandroidicu \
	libnativehelper \
	libstnfc-nci \

LOCAL_STATIC_LIBRARIES += nfc_jni_files_for_factory

include $(BUILD_EXECUTABLE)

# ---------Just for Test----------
include $(CLEAR_VARS)

LOCAL_SRC_FILES := st_factorytests.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libnativehelper \
    libchrome \
    libbase \

LOCAL_MODULE := st_factorytests
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := mtk

include $(BUILD_EXECUTABLE)

endif
# // END system version

