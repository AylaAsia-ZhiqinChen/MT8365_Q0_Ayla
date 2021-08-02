LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk

ifneq ($(NCI_VERSION),)
LOCAL_CFLAGS += -DNCI_VERSION=$(NCI_VERSION) -O0 -g
endif

LOCAL_CFLAGS += -Wall -Wextra -Wno-unused-parameter -Werror -DST21NFC -DST21NFC_DTA
ifneq (,$(filter userdebug eng,$(TARGET_BUILD_VARIANT)))
LOCAL_CFLAGS += -DDCHECK_ALWAYS_ON
endif

LOCAL_SRC_FILES := $(call all-subdir-cpp-files)
LOCAL_SRC_FILES += \
	./../../nci/jni/CondVar.cpp \
	./../../nci/jni/DataQueue.cpp \
	./../../nci/jni/IntervalTimer.cpp \
	./../../nci/jni/NativeLlcpConnectionlessSocket.cpp \
	./../../nci/jni/NativeLlcpServiceSocket.cpp \
	./../../nci/jni/NativeLlcpSocket.cpp \
	./../../nci/jni/NativeP2pDevice.cpp \
	./../../nci/jni/PeerToPeer.cpp \
	./../../nci/jni/RouteDataSet.cpp \
	./../../nci/jni/PowerSwitch.cpp

LOCAL_C_INCLUDES += \
    vendor/mediatek/proprietary/hardware/stnfc/src/nfa/include \
    vendor/mediatek/proprietary/hardware/stnfc/src/nfc/include \
    vendor/mediatek/proprietary/hardware/stnfc/src/include \
    vendor/mediatek/proprietary/hardware/stnfc/src/gki/ulinux \
    vendor/mediatek/proprietary/hardware/stnfc/src/gki/common \
    vendor/mediatek/proprietary/hardware/stnfc/utils/include \
    \
    external/libxml2/include \
    frameworks/native/include \
    vendor/mediatek/proprietary/hardware/stnfc/src/nfc/int \
    vendor/mediatek/proprietary/hardware/stnfc/src/hal/include \
    vendor/mediatek/proprietary/hardware/stnfc/src/hal/int \
    $(LOCAL_PATH)/../../nci/jni \
    vendor/mediatek/proprietary/hardware/stnfc/utils/include \
    $(LOCAL_PATH)/../../st_dta/jni

LOCAL_SHARED_LIBRARIES := \
    libandroidicu \
    libnativehelper \
    libcutils \
    libutils \
    liblog \
    libstnfc-nci \
    libchrome \
    libbase \
    libdl \
    libnfc_st_dta_jni

LOCAL_STATIC_LIBRARIES := libxml2

LOCAL_PROPRIETARY_MODULE := false
LOCAL_MODULE := libstnfc_nci_jni
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
