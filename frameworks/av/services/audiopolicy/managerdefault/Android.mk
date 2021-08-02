LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= AudioPolicyManager.cpp

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    liblog \
    libaudiopolicy \
    libsoundtrigger

ifeq ($(USE_CONFIGURABLE_AUDIO_POLICY), 1)

ifneq ($(USE_XML_AUDIO_POLICY_CONF), 1)
$(error Configurable policy does not support legacy conf file)
endif #ifneq ($(USE_XML_AUDIO_POLICY_CONF), 1)

LOCAL_SHARED_LIBRARIES += libaudiopolicyengineconfigurable

else

LOCAL_SHARED_LIBRARIES += libaudiopolicyenginedefault

endif # ifeq ($(USE_CONFIGURABLE_AUDIO_POLICY), 1)

LOCAL_C_INCLUDES += \
    $(call include-path-for, audio-utils)

LOCAL_HEADER_LIBRARIES := \
    libaudiopolicycommon \
    libaudiopolicyengine_interface_headers \
    libaudiopolicymanager_interface_headers

LOCAL_STATIC_LIBRARIES := \
    libaudiopolicycomponents

LOCAL_SHARED_LIBRARIES += libmedia_helper
LOCAL_SHARED_LIBRARIES += libmediametrics

LOCAL_SHARED_LIBRARIES += libbinder libhidlbase libxml2

ifeq ($(USE_XML_AUDIO_POLICY_CONF), 1)
LOCAL_CFLAGS += -DUSE_XML_AUDIO_POLICY_CONF
endif #ifeq ($(USE_XML_AUDIO_POLICY_CONF), 1)

LOCAL_CFLAGS += -Wall -Werror

LOCAL_MULTILIB := $(AUDIOSERVER_MULTILIB)

LOCAL_MODULE:= libaudiopolicymanagerdefault

# MTK_AUDIO For AudioParamParser


LOCAL_C_INCLUDES += \
    external/libxml2/include \
    external/icu/icu4c/source/common \
    $(MTK_PATH_SOURCE)/external/AudioParamParser/ \
    $(MTK_PATH_SOURCE)/external/AudioParamParser/include

LOCAL_CFLAGS += -DSYS_IMPL

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
LOCAL_SHARED_LIBRARIES += libaudioutilmtk

LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/cgen/cfgfileinc \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/custom_volume \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/gain_table
include $(BUILD_SHARED_LIBRARY)
