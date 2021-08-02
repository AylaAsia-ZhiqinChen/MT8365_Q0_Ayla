LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    AudioPolicyManagerCustomImpl.cpp \
    AudioPolicyServiceCustomImpl.cpp


LOCAL_C_INCLUDES := \
    $(TOPDIR)frameworks/av/services/audioflinger \
    $(call include-path-for, audio-utils) \
    $(TOPDIR)frameworks/av/services/audiopolicy/common/include \
    $(TOPDIR)frameworks/av/services/audiopolicy/engine/interface \
    $(TOPDIR)frameworks/av/services/audiopolicy/utilities \
    $(TOPDIR)frameworks/av/services/audiopolicy/service \
    $(TOPDIR)frameworks/av/services/audiopolicy/managerdefault \
    $(TOPDIR)frameworks/av/services/audiopolicy\
    $(audio_policy_engine_includes_common)

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    liblog \
    libbinder \
    libaudioclient \
    libhardware_legacy \
    libmediautils \
    libaudiopolicymanagerdefault \
    libmedia_helper\
    libsensorprivacy

LOCAL_STATIC_LIBRARIES := \
    libaudiopolicycomponents

LOCAL_EXPORT_SHARED_LIBRARY_HEADERS := \
            libsensorprivacy

LOCAL_HEADER_LIBRARIES := \
    libaudiopolicycommon \
    libaudiopolicyengine_interface_headers\
    libaudiopolicyengine_common_headers

LOCAL_C_INCLUDES += frameworks/av/services/audiopolicy/engineconfigurable/include

LOCAL_MULTILIB := $(AUDIOSERVER_MULTILIB)

LOCAL_MODULE:= libaudiopolicycustomextensions

LOCAL_CFLAGS += -Wall -Werror

ifeq ($(MTK_AUDIO),yes)
LOCAL_CFLAGS += -DMTK_AUDIO
LOCAL_CFLAGS += -DMTK_AUDIO_DEBUG

ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
  LOCAL_CFLAGS += -DCONFIG_MT_ENG_BUILD
else ifeq ($(strip $(TARGET_BUILD_VARIANT)),userdebug)
  LOCAL_CFLAGS += -DCONFIG_MT_USERDEBUG_BUILD
endif

    LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/external/AudioParamParser
    LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/external/AudioParamParser/include
    LOCAL_C_INCLUDES += external/libxml2/include
    LOCAL_C_INCLUDES += external/icu/icu4c/source/common
    LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/external/audio_utils/common_headers
    LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/audiopolicy_parameters
    LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/custom_volume
    LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/gain_table
    LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/external/audio_utils/common_utils/AudioToolkit
    LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/external/AudioCompensationFilter
    LOCAL_CFLAGS += -DSYS_IMPL

LOCAL_CFLAGS += -DMTK_LOW_LATENCY

LOCAL_SHARED_LIBRARIES += \
     libaudiotoolkit
LOCAL_SHARED_LIBRARIES += libaudioutilmtk

endif#MTK_AUDIO

LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/customization \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/cgen/cfgfileinc \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/gain_table

include $(BUILD_SHARED_LIBRARY)
