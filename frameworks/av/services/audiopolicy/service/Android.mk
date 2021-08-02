LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    AudioPolicyService.cpp \
    AudioPolicyEffects.cpp \
    AudioPolicyInterfaceImpl.cpp \
    AudioPolicyClientImpl.cpp

LOCAL_C_INCLUDES := \
    frameworks/av/services/audioflinger \
    $(call include-path-for, audio-utils)

LOCAL_HEADER_LIBRARIES := \
    libaudiopolicycommon \
    libaudiopolicyengine_interface_headers \
    libaudiopolicymanager_interface_headers

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    liblog \
    libbinder \
    libaudioclient \
    libaudioutils \
    libhardware_legacy \
    libaudiopolicymanager \
    libmedia_helper \
    libmediametrics \
    libmediautils \
    libeffectsconfig \
    libsensorprivacy

LOCAL_EXPORT_SHARED_LIBRARY_HEADERS := \
    libsensorprivacy

LOCAL_STATIC_LIBRARIES := \
    libaudiopolicycomponents

LOCAL_MULTILIB := $(AUDIOSERVER_MULTILIB)

LOCAL_MODULE:= libaudiopolicyservice

LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -Wall -Werror

ifeq ($(MTK_MULTI_RECORD_SUPPORT),yes)
LOCAL_CFLAGS += -DMTK_MULTI_RECORD_SUPPORT
endif

ifeq ($(MTK_AUDIO),yes)
LOCAL_CFLAGS += -DMTK_AUDIO

LOCAL_CFLAGS += -DMTK_LOW_LATENCY

LOCAL_CFLAGS += -DMTK_AUDIO_FIX_DEFAULT_DEFECT


LOCAL_CFLAGS += -DMTK_AUDIO_DEBUG
ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
  LOCAL_CFLAGS += -DCONFIG_MT_ENG_BUILD
else ifeq ($(strip $(TARGET_BUILD_VARIANT)),userdebug)
  LOCAL_CFLAGS += -DCONFIG_MT_USERDEBUG_BUILD
endif
endif#MTK_AUDIO

# MTK_AUDIO For AudioParamParser
LOCAL_C_INCLUDES += \
    external/libxml2/include \
    external/icu/icu4c/source/common \
    $(MTK_PATH_SOURCE)/external/AudioParamParser/ \
    $(MTK_PATH_SOURCE)/external/AudioParamParser/include
LOCAL_CFLAGS += -DSYS_IMPL

LOCAL_SHARED_LIBRARIES += libaudiopolicycustomextensions
LOCAL_SHARED_LIBRARIES += libaudioutilmtk

LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/cgen/cfgfileinc \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/custom_volume \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/gain_table
include $(BUILD_SHARED_LIBRARY)

