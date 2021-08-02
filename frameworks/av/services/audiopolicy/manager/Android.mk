LOCAL_PATH:= $(call my-dir)

ifneq ($(USE_CUSTOM_AUDIO_POLICY), 1)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    AudioPolicyFactory.cpp

LOCAL_SHARED_LIBRARIES := \
    libaudiopolicymanagerdefault

LOCAL_STATIC_LIBRARIES := \
    libaudiopolicycomponents

LOCAL_C_INCLUDES += \
    $(call include-path-for, audio-utils)

LOCAL_HEADER_LIBRARIES := \
    libaudiopolicycommon \
    libaudiopolicyengine_interface_headers \
    libaudiopolicymanager_interface_headers

LOCAL_CFLAGS := -Wall -Werror

LOCAL_MULTILIB := $(AUDIOSERVER_MULTILIB)

LOCAL_MODULE:= libaudiopolicymanager

# MTK_AUDIO For AudioParamParser
LOCAL_C_INCLUDES += \
    external/libxml2/include \
    external/icu/icu4c/source/common \
    $(MTK_PATH_SOURCE)/external/AudioParamParser/ \
    $(MTK_PATH_SOURCE)/external/AudioParamParser/include
LOCAL_CFLAGS += -DSYS_IMPL

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

LOCAL_SHARED_LIBRARIES += libaudiopolicycustomextensions
LOCAL_SHARED_LIBRARIES += libaudioutilmtk

LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/cgen/cfgfileinc \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/custom_volume \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/gain_table
include $(BUILD_SHARED_LIBRARY)

endif #ifneq ($(USE_CUSTOM_AUDIO_POLICY), 1)
