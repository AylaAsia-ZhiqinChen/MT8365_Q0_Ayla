VOB_COMPONENTS := vendor/mediatek/proprietary/hardware/stnfc/src
NFA := $(VOB_COMPONENTS)/nfa
NFC := $(VOB_COMPONENTS)/nfc

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk

ifneq ($(NCI_VERSION),)
LOCAL_CFLAGS += -DNCI_VERSION=$(NCI_VERSION) -O0 -g
endif

LOCAL_CFLAGS += -Wall -Wextra -Wno-unused-parameter -Werror -DST21NFC -DST21NFC_DTA -DTARGET_TYPE_LINUX

ifneq (,$(filter userdebug eng,$(TARGET_BUILD_VARIANT)))
LOCAL_CFLAGS += -DDCHECK_ALWAYS_ON
endif

define all-cpp-files-under
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.cpp" -and -not -name ".*") \
 )
endef

LOCAL_SRC_FILES:= $(call all-cpp-files-under, .)

LOCAL_C_INCLUDES += \
    vendor/mediatek/proprietary/packages/apps/ST-Nfc/nci/jni \
    vendor/mediatek/proprietary/hardware/stnfc/dta_st/dta/common/inc \
    vendor/mediatek/proprietary/hardware/stnfc/dta_st/dta/common/porting/linux \
    $(VOB_COMPONENTS)/include \
    $(VOB_COMPONENTS)/gki/ulinux \
    $(VOB_COMPONENTS)/gki/common \
    $(VOB_COMPONENTS)/hal/include \
    $(NFA)/include \
    $(NFC)/include

LOCAL_SHARED_LIBRARIES := \
    libandroidicu \
    libnativehelper \
    libcutils \
    libutils \
    liblog \
    libchrome \
    libbase \
    libdl \
    libstnfc-nci \
    libnfc_st_dta

LOCAL_STATIC_LIBRARIES := libxml2

LOCAL_PROPRIETARY_MODULE := false
LOCAL_MODULE := libnfc_st_dta_jni
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
