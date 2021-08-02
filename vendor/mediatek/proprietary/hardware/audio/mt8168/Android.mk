# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
# MediaTek Inc. (C) 2016. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.

#  only if use yusu audio will build this.
#ifneq ($(TARGET_BUILD_PDK),true)
ifeq ($(strip $(BOARD_USES_MTK_AUDIO)),true)

LOCAL_PATH:= $(call my-dir)
LOCAL_COMMON_PATH := common_symlink

include $(CLEAR_VARS)

ifeq ($(MTK_PLATFORM),MT8168)

ifeq ($(strip $(MTK_HIGH_RESOLUTION_AUDIO_SUPPORT)),yes)
    LOCAL_CFLAGS += -DMTK_HD_AUDIO_ARCHITECTURE
endif

ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
  LOCAL_CFLAGS += -DDEBUG_AUDIO_PCM
endif

ifeq ($(MTK_DIGITAL_MIC_SUPPORT),yes)
  LOCAL_CFLAGS += -DMTK_DIGITAL_MIC_SUPPORT
endif

ifeq ($(strip $(MTK_AUDENH_SUPPORT)),yes)
  LOCAL_CFLAGS += -DMTK_AUDENH_SUPPORT
endif

ifeq ($(strip $(MTK_BESLOUDNESS_SUPPORT)),yes)
  LOCAL_CFLAGS += -DMTK_BESLOUDNESS_SUPPORT
endif

ifeq ($(strip $(MTK_BESSURROUND_SUPPORT)),yes)
  LOCAL_CFLAGS += -DMTK_BESSURROUND_SUPPORT
endif

ifeq ($(strip $(MTK_HDMI_MULTI_CHANNEL_SUPPORT)),yes)
  LOCAL_CFLAGS += -DMTK_HDMI_MULTI_CHANNEL_SUPPORT
endif

ifeq ($(strip $(MTK_2IN1_SPK_SUPPORT)),yes)
  LOCAL_CFLAGS += -DUSING_2IN1_SPEAKER
endif

ifeq ($(strip $(MTK_USE_ANDROID_MM_DEFAULT_CODE)),yes)
  LOCAL_CFLAGS += -DANDROID_DEFAULT_CODE
endif

ifeq ($(strip $(DMNR_TUNNING_AT_MODEMSIDE)),yes)
  LOCAL_CFLAGS += -DDMNR_TUNNING_AT_MODEMSIDE
endif

ifeq ($(MTK_DUAL_MIC_SUPPORT),yes)
  LOCAL_CFLAGS += -DMTK_DUAL_MIC_SUPPORT
endif

ifeq ($(MTK_VOW_SUPPORT),yes)
  LOCAL_CFLAGS += -DMTK_VOW_SUPPORT
endif

ifeq ($(MTK_MAGICONFERENCE_SUPPORT),yes)
  LOCAL_CFLAGS += -DMTK_MAGICONFERENCE_SUPPORT
endif

ifeq ($(MTK_HAC_SUPPORT),yes)
  LOCAL_CFLAGS += -DMTK_HAC_SUPPORT
endif

ifeq ($(MTK_VIBSPK_SUPPORT),yes)
  LOCAL_CFLAGS += -DMTK_VIBSPK_SUPPORT
endif

ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
  LOCAL_CFLAGS += -DCONFIG_MT_ENG_BUILD
endif

LOCAL_CFLAGS += -DUPLINK_LOW_LATENCY
LOCAL_CFLAGS += -DDOWNLINK_LOW_LATENCY

# For New DC Trim Calibration Flow
LOCAL_CFLAGS += -DNEW_PMIC_DCTRIM_FLOW

LOCAL_CFLAGS += -DSW_BTCVSD_ENABLE
LOCAL_CFLAGS += -DMTK_SUPPORT_BTCVSD_ALSA
LOCAL_CFLAGS += -DMTK_SUPPORT_AUDIO_DEVICE_API3

ifeq ($(MTK_AUDIO_BLOUD_CUSTOMPARAMETER_REV),MTK_AUDIO_BLOUD_CUSTOMPARAMETER_V5)
  LOCAL_CFLAGS += -DMTK_AUDIO_BLOUD_CUSTOMPARAMETER_V5
else
  ifeq ($(strip $(MTK_AUDIO_BLOUD_CUSTOMPARAMETER_REV)),MTK_AUDIO_BLOUD_CUSTOMPARAMETER_V4)
    LOCAL_CFLAGS += -DMTK_AUDIO_BLOUD_CUSTOMPARAMETER_V4
  endif
endif

ifeq ($(MTK_BSP_PACKAGE),yes)
 LOCAL_CFLAGS += -DMTK_BSP_PACKAGE
endif

# Playback must be 24bit when using sram
#LOCAL_CFLAGS += -DPLAYBACK_USE_24BITS_ONLY

LOCAL_HEADER_LIBRARIES += libprimaryaudiohal_headers

# MTK Audio Tuning Tool Version
ifneq ($(MTK_AUDIO_TUNING_TOOL_VERSION),)
  ifneq ($(strip $(MTK_AUDIO_TUNING_TOOL_VERSION)),V1)
    MTK_AUDIO_TUNING_TOOL_V2_PHASE:=$(shell echo $(MTK_AUDIO_TUNING_TOOL_VERSION) | sed 's/V2.//g')
    LOCAL_CFLAGS += -DMTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT
    LOCAL_CFLAGS += -DMTK_AUDIO_TUNING_TOOL_V2_PHASE=$(MTK_AUDIO_TUNING_TOOL_V2_PHASE)

    LOCAL_SRC_FILES+= \
        $(LOCAL_COMMON_PATH)/V3/speech_driver/SpeechParamParser.cpp \
        $(LOCAL_COMMON_PATH)/speech_driver/SpeechConfig.cpp

    ifneq ($(MTK_AUDIO_TUNING_TOOL_V2_PHASE),1)
      LOCAL_CFLAGS += -DMTK_AUDIO_GAIN_TABLE
      LOCAL_CFLAGS += -DMTK_NEW_VOL_CONTROL

      LOCAL_SRC_FILES += $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAGainController.cpp
      LOCAL_SRC_FILES += $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioGainTableParamParser.cpp

    endif
  endif
endif

ifeq (yes,$(strip $(MTK_POWERAQ_SUPPORT)))
  #LOCAL_LDLIBS := -lm -llog
  LOCAL_CFLAGS += -DMTK_POWERAQ_SUPPORT
  LOCAL_SRC_FILES += $(LOCAL_COMMON_PATH)/V3/aud_drv/MTKAudioPowerAQCmdHandler.cpp
  LOCAL_SHARED_LIBRARIES += libpoweraq_mtk
endif

# Speaker Customization
ifeq ($(strip $(MTK_AUDIO_SPEAKER_PATH)),int_spk_amp)

else ifeq ($(strip $(MTK_AUDIO_SPEAKER_PATH)),int_lo_buf)
  LOCAL_CFLAGS += -DUSING_EXTAMP_LO
else ifeq ($(strip $(MTK_AUDIO_SPEAKER_PATH)),int_hp_buf)
  LOCAL_CFLAGS += -DUSING_EXTAMP_HP
else ifeq ($(strip $(MTK_AUDIO_SPEAKER_PATH)),2_in_1_spk)

else ifeq ($(strip $(MTK_AUDIO_SPEAKER_PATH)),3_in_1_spk)

else ifeq ($(findstring smartpa_dynamic_detect, $(MTK_AUDIO_SPEAKER_PATH)),smartpa_dynamic_detect)
  LOCAL_CFLAGS += -DSMARTPA_DYNAMIC_DETECT
else
  ifeq ($(findstring smartpa, $(MTK_AUDIO_SPEAKER_PATH)), smartpa)
    LOCAL_CFLAGS += -DEXT_SPK_SUPPORT	# for old volume control

    ifeq ($(findstring maxim, $(MTK_AUDIO_SPEAKER_PATH)), maxim)
      LOCAL_CFLAGS += -DMTK_MAXIM_SPEAKER_SUPPORT

      LOCAL_SRC_FILES += $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAPlaybackHandlerSpeakerProtection.cpp \
        $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAPcmDataCaptureIn.cpp
    endif
  endif
endif

# Smart Pa
LOCAL_SRC_FILES += $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioSmartPaController.cpp

LOCAL_SHARED_LIBRARIES += libxml2


# POWR HAL Control for low latency & low power
#LOCAL_CFLAGS += -DMTK_POWERHAL_AUDIO_LATENCY
#LOCAL_CFLAGS += -DMTK_POWERHAL_AUDIO_POWER
LOCAL_SHARED_LIBRARIES += \
    libhidlbase \
    libhidltransport \
    libhwbinder \
    vendor.mediatek.hardware.mtkpower@1.0 \
    android.hardware.power@1.0 \
    vendor.mediatek.hardware.power@2.0 \
    android.hardware.bluetooth.a2dp@1.0

LOCAL_SRC_FILES += \
    $(LOCAL_COMMON_PATH)/utility/audio_lock.c \
    $(LOCAL_COMMON_PATH)/utility/audio_time.c \
    $(LOCAL_COMMON_PATH)/utility/audio_ringbuf.c \
    $(LOCAL_COMMON_PATH)/aud_drv/audio_hw_hal.cpp \
    $(LOCAL_COMMON_PATH)/aud_drv/AudioMTKFilter.cpp \
    $(LOCAL_COMMON_PATH)/aud_drv/AudioMTKHeadsetMessager.cpp \
    $(LOCAL_COMMON_PATH)/aud_drv/AudioUtility.cpp \
    $(LOCAL_COMMON_PATH)/aud_drv/AudioFtmBase.cpp \
    $(LOCAL_COMMON_PATH)/aud_drv/WCNChipController.cpp \
    $(LOCAL_COMMON_PATH)/speech_driver/SpeechDriverFactory.cpp \
    $(LOCAL_COMMON_PATH)/speech_driver/SpeechDriverDummy.cpp \
    $(LOCAL_COMMON_PATH)/speech_driver/SpeechEnhancementController.cpp \
    $(LOCAL_COMMON_PATH)/speech_driver/SpeechPcmMixerBase.cpp \
    $(LOCAL_COMMON_PATH)/speech_driver/SpeechPcmMixerBGSPlayer.cpp \
    $(LOCAL_COMMON_PATH)/speech_driver/SpeechPcm2way.cpp \
    $(LOCAL_COMMON_PATH)/speech_driver/SpeechUtility.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAFMController.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioSpeechEnhanceInfo.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioSpeechEnhLayer.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioPreProcess.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSADriverUtility.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSASampleRateController.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAHardware.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSADataProcessor.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAPlaybackHandlerBase.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAPlaybackHandlerVoice.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAPlaybackHandlerFMTransmitter.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAPlaybackHandlerBTCVSD.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureHandlerBase.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureHandlerVoice.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureHandlerFMRadio.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureHandlerANC.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureHandlerBT.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataClient.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataClientSyncIO.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderBase.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderNormal.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderVoice.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderVoiceUL.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderVoiceDL.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderVoiceMix.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderFMRadio.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderANC.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderBTSCO.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderBTCVSD.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderEchoRef.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderEchoRefBTCVSD.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderEchoRefBTSCO.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderEchoRefExt.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACodecDeviceBase.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACodecDeviceOutReceiverPMIC.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACodecDeviceOutEarphonePMIC.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACodecDeviceOutSpeakerPMIC.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACodecDeviceOutSpeakerEarphonePMIC.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACodecDeviceOutExtSpeakerAmp.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSADeviceConfigManager.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAParamTuner.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/LoopbackManager.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSADeviceParser.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioBTCVSDControl.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioVolumeFactory.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/SpeechDataProcessingHandler.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioUSBPhoneCallController.cpp \
    $(LOCAL_COMMON_PATH)/V3/speech_driver/SpeechDriverLAD.cpp \
    $(LOCAL_COMMON_PATH)/V3/speech_driver/SpeechMessengerECCCI.cpp \
    $(LOCAL_COMMON_PATH)/V3/speech_driver/SpeechVMRecorder.cpp \
    $(LOCAL_COMMON_PATH)/V3/speech_driver/AudioALSASpeechLoopbackController.cpp \
    aud_drv/AudioALSAHardwareResourceManager.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAStreamIn.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureHandlerVOW.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureDataProviderVOW.cpp \
    $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAVoiceWakeUpController.cpp \
    aud_drv/AudioALSAStreamManager.cpp \
    aud_drv/AudioALSAStreamOut.cpp \
    aud_drv/AudioALSAVolumeController.cpp \
    aud_drv/AudioFtm.cpp \
    aud_drv/HDMITxController.cpp \
    $(LOCAL_COMMON_PATH)/speech_driver/AudioALSASpeechPhoneCallController.cpp \
    $(LOCAL_COMMON_PATH)/aud_drv/AudioCustParamClient.cpp \
    $(LOCAL_COMMON_PATH)/aud_drv/AudioEventThreadManager.cpp

LOCAL_CFLAGS += -fexceptions

# Override from common/V3
LOCAL_SRC_FILES += \
    aud_drv/AudioALSAPlaybackHandlerNormal.cpp \
    aud_drv/AudioALSAPlaybackHandlerFast.cpp \
    aud_drv/AudioALSAPlaybackHandlerBTSCO.cpp \
    aud_drv/AudioALSAPlaybackHandlerHDMI.cpp \
    aud_drv/AudioALSACaptureHandlerNormal.cpp \
    aud_drv/AudioALSACaptureHandlerAEC.cpp \
    aud_drv/AudioALSALoopbackController.cpp

# hfp client
ifeq ($(strip $(MTK_BT_PROFILE_HFP_CLIENT)),yes)
    LOCAL_CFLAGS += -DMTK_BT_PROFILE_HFP_CLIENT

    LOCAL_SRC_FILES += \
        aud_drv/AudioHfpController.cpp \
        aud_drv/AudioHfpProcessing.cpp \
        aud_drv/AudioHfpDLTaskSink.cpp \
        aud_drv/AudioHfpULTaskSink.cpp \
        aud_drv/AudioHfpDLTaskSource.cpp \
        aud_drv/AudioHfpULTaskSource.cpp \
        aud_drv/AudioHfpDLEchoRefSource.cpp

    LOCAL_SRC_FILES += \
        $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSAPlaybackHandlerHfpClient.cpp \
        $(LOCAL_COMMON_PATH)/V3/aud_drv/AudioALSACaptureHandlerNull.cpp

    ifeq ($(strip $(MTK_BT_CHIP)),MTK_CONSYS_MT8168)
        LOCAL_CFLAGS += -DMTK_BT_AUDIO_USING_BTCVSD
    endif
endif

ifeq ($(MTK_ADSP_VA_SUPPORT),yes)
    LOCAL_CFLAGS += -DMTK_ADSP_VA_SUPPORT
    LOCAL_HEADER_LIBRARIES += pcm_merge_common_includes

    LOCAL_SRC_FILES += aud_drv/AudioALSAAdspVaController.cpp \
        aud_drv/AudioALSACaptureHandlerHotword.cpp \
        aud_drv/AudioALSACaptureDataProviderSoundTrigger.cpp \
        aud_drv/AudioALSACaptureDataClientSoundTrigger.cpp \
        aud_drv/AudioALSACaptureDataProviderHwCM.cpp \
        aud_drv/AudioALSACaptureDataClientVoiceRecProc.cpp
    LOCAL_SHARED_LIBRARIES += libmsb
endif

ifeq ($(MTK_AIS_AUDIOHAL_PROCESS_SUPPORT),yes)
    LOCAL_HEADER_LIBRARIES += capiv2_api_headers
endif

ifeq ($(ENABLE_AUDIO_DUMP),true)
  LOCAL_SRC_FILES += AudioDumpInterface.cpp
  LOCAL_CFLAGS += -DENABLE_AUDIO_DUMP
endif

#ifeq ($(MTK_VIBSPK_SUPPORT),yes)
  LOCAL_SRC_FILES += $(LOCAL_COMMON_PATH)/aud_drv/AudioVIBSPKControl.cpp
#endif

ifeq ($(strip $(MTK_TTY_SUPPORT)),yes)
  LOCAL_CFLAGS += -DMTK_TTY_SUPPORT
endif

LOCAL_STATIC_LIBRARIES := \
    libmedia_helper

LOCAL_HEADER_LIBRARIES += libaudio_system_headers libmedia_headers

ifeq ($(findstring MTK_AOSP_ENHANCEMENT, $(MTK_GLOBAL_CFLAGS)),)
  LOCAL_CFLAGS += -DMTK_BASIC_PACKAGE
endif

ifeq ($(strip $(MTK_ENABLE_MD1)),yes)
  LOCAL_CFLAGS += -D__MTK_ENABLE_MD1__
endif

ifeq ($(strip $(MTK_ENABLE_MD2)),yes)
  LOCAL_CFLAGS += -D__MTK_ENABLE_MD2__
endif

ifeq ($(strip $(MTK_ENABLE_MD5)),yes)
  LOCAL_CFLAGS += -D__MTK_ENABLE_MD5__
endif

LOCAL_HEADER_LIBRARIES += libcutils_headers libutils_headers libhardware_headers

LOCAL_SHARED_LIBRARIES += \
    libcutils \
    libutils \
    libalsautils \
    libhardware_legacy \
    libhardware \
    libdl \
    libaudioutils \
    libtinyalsa \
    libtinycompress \
    libtinyxml \
    libaudioroute \
    libaudiotoolkit_vendor \
    liblog \

# CallStack
LOCAL_HEADER_LIBRARIES += libladder_headers
LOCAL_SHARED_LIBRARIES += libladder

RAT_CONFIG = $(strip $(MTK_PROTOCOL1_RAT_CONFIG))
ifneq (, $(RAT_CONFIG))
  ifneq (,$(findstring C,$(RAT_CONFIG)))
    LOCAL_CFLAGS += -DAUDIO_C2K_SUPPORT
    LOCAL_SRC_FILES+= $(LOCAL_COMMON_PATH)/V3/speech_driver/SpeechMessengerEVDO.cpp

  endif
endif

ifeq ($(HAVE_AEE_FEATURE),yes)
    LOCAL_SHARED_LIBRARIES += libaedv
    LOCAL_CFLAGS += -DHAVE_AEE_FEATURE
endif

ifeq ($(TELEPHONY_DFOSET),yes)
    LOCAL_SHARED_LIBRARIES += libdfo
endif

ifeq ($(MTK_WB_SPEECH_SUPPORT),yes)
  LOCAL_CFLAGS += -DMTK_WB_SPEECH_SUPPORT
endif

# SRS Processing
ifeq ($(strip $(HAVE_SRSAUDIOEFFECT_FEATURE)),yes)
LOCAL_CFLAGS += -DHAVE_SRSAUDIOEFFECT
endif
# SRS Processing

# Audio HD Record
ifeq ($(MTK_AUDIO_HD_REC_SUPPORT),yes)
    LOCAL_CFLAGS += -DMTK_AUDIO_HD_REC_SUPPORT
endif
# Audio HD Record

# MTK VoIP
ifeq ($(MTK_VOIP_ENHANCEMENT_SUPPORT),yes)
    LOCAL_CFLAGS += -DMTK_VOIP_ENHANCEMENT_SUPPORT
endif
# MTK VoIP

# DMNR 3.0
ifeq ($(strip $(MTK_HANDSFREE_DMNR_SUPPORT)),yes)
  LOCAL_CFLAGS += -DMTK_HANDSFREE_DMNR_SUPPORT
endif
# DMNR 3.0

# Native Audio Preprocess
ifeq ($(strip $(NATIVE_AUDIO_PREPROCESS_ENABLE)),yes)
    LOCAL_CFLAGS += -DNATIVE_AUDIO_PREPROCESS_ENABLE
endif
# Native Audio Preprocess

ifeq ($(MTK_INTERNAL_HDMI_SUPPORT),yes)
  LOCAL_CFLAGS += -DMTK_INTERNAL_HDMI_SUPPORT
endif

ifeq ($(MTK_INTERNAL_MHL_SUPPORT),yes)
  LOCAL_CFLAGS += -DMTK_INTERNAL_MHL_SUPPORT
endif


#Record must be 24bit when using sram
#LOCAL_CFLAGS += -DRECORD_INPUT_24BITS

# Audio Lock 2.0
LOCAL_CFLAGS += -DMTK_AUDIO_LOCK_ENABLE_TRACE
ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
#    LOCAL_CFLAGS += -DMTK_AUDIO_LOCK_ENABLE_LOG
endif

LOCAL_CFLAGS += -DMTK_DMIC_SR_LIMIT

# Force to output clock to HDMI TX before sending real data
LOCAL_CFLAGS += -DMTK_HDMI_FORCE_AUDIO_CLK

LOCAL_ARM_MODE := arm
LOCAL_MODULE := audio.primary.$(TARGET_BOARD_PLATFORM)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := both
include $(MTK_SHARED_LIBRARY)

#========================================================================

include $(CLEAR_VARS)
LOCAL_MODULE:= libprimaryaudiohal_headers
LOCAL_EXPORT_C_INCLUDE_DIRS:= \
    $(MTK_PATH_SOURCE)/external/AudioParamParser \
    $(MTK_PATH_SOURCE)/external/AudioParamParser/include \
    $(MTK_PATH_SOURCE)/hardware/ccci/include \
    $(call include-path-for, audio-utils) \
    $(call include-path-for, audio-effects) \
    $(call include-path-for, alsa-utils) \
    $(call include-path-for, audio-route) \
    $(MTK_PATH_SOURCE)/hardware/audio/$(shell echo $(MTK_PLATFORM) | tr A-Z a-z)/include \
    $(MTK_PATH_SOURCE)/hardware/audio/$(shell echo $(MTK_PLATFORM) | tr A-Z a-z)/common_symlink/utility \
    $(MTK_PATH_SOURCE)/hardware/audio/$(shell echo $(MTK_PLATFORM) | tr A-Z a-z)/common_symlink/V3/include \
    $(MTK_PATH_SOURCE)/external/AudioCompensationFilter \
    $(MTK_PATH_SOURCE)/external/AudioComponentEngine \
    $(MTK_PATH_SOURCE)/external/audiocustparam \
    $(MTK_PATH_SOURCE)/external/AudioSpeechEnhancement/V3/inc \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_utils/AudioToolkit \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/audiopolicy_parameters \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/cgen/cfgfileinc \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/customization \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/custom_volume \
    $(MTK_PATH_SOURCE)/external/audio_utils/common_headers/gain_table \
    $(MTK_PATH_SOURCE)/hardware/power/config/common/intf_types \
    $(MTK_PATH_CUSTOM)/hal/audioflinger/audio \
    external/tinyxml \
    frameworks/av/include \
    $(MTK_PATH_SOURCE)/external/blisrc/blisrc32 \
    $(MTK_PATH_SOURCE)/external/shifter \
    $(MTK_PATH_SOURCE)/external/limiter \
    $(MTK_PATH_SOURCE)/external/poweraq/ppc/adapter \
    $(MTK_PATH_SOURCE)/external/poweraq/ppc/model/resource/inc \

include $(BUILD_HEADER_LIBRARY)


endif
endif
