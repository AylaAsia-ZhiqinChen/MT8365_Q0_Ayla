/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <cutils/config_utils.h>
#include <cutils/misc.h>
#include <utils/Timers.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/SortedVector.h>
#include <media/AudioParameter.h>
#include <media/AudioPolicy.h>
#include "AudioPolicyInterface.h"

#include <AudioPolicyManagerInterface.h>
#include <AudioPolicyManagerObserver.h>
#include <AudioGain.h>
#include <AudioPort.h>
#include <AudioPatch.h>
#include <DeviceDescriptor.h>
#include <IOProfile.h>
#include <HwModule.h>
#include <AudioInputDescriptor.h>
#include <AudioOutputDescriptor.h>
#include <AudioPolicyMix.h>
#include <EffectDescriptor.h>
#include <SoundTriggerSession.h>
#include <utils/threads.h>
#include <media/AudioVolumeGroup.h>
#include "AudioGainTableParam.h"  // MTK_AUDIO
#include "custom_extensions/AudioPolicyVendorControl.h"  // MTK_AUDIO
#include "custom_extensions/AudioPolicyManagerCustomInterface.h"  // MTK_AUDIO
#include "AudioCustomVolume.h"  // MTK_AUDIO
namespace android {
    // MTK_AUDIO
#define SONIFICATION_AUX_DIGITAL_VOLUME_FACTOR 0.5
#define SONIFICATION_AUX_DIGITAL_VOLUME_FACTOR_DB (-6)


class AudioPolicyManager;
class AudioPolicyManagerCustomImpl : public AudioPolicyManagerCustomInterface {
    public:
        explicit AudioPolicyManagerCustomImpl();
        virtual ~AudioPolicyManagerCustomImpl();
        virtual status_t common_set(AudioPolicyManager *audioPolicyManger);
        virtual audio_stream_type_t gainTable_getVolumeStream();
        virtual int gainTable_getVolumeIndex();
        virtual audio_devices_t gainTable_getVolumeDevice();
        virtual status_t gainTable_setVolumeStream(audio_stream_type_t stream);
        virtual status_t gainTable_setVolumeIndex(int index);
        virtual status_t gainTable_setVolumeDevice(audio_devices_t device);
        virtual status_t gainTable_getCustomAudioVolume(void);
        virtual float gainTable_getVolumeDbFromComputeVolume(VolumeSource volsrc, int index, audio_devices_t device, float volumeDB);
        virtual audio_devices_t gainTable_getDeviceFromComputeVolume(VolumeSource volumeSource, int index, audio_devices_t device);
        virtual float gainTable_getCorrectVolumeDbFromComputeVolume(VolumeSource volumeSource, float volumeDB, audio_devices_t device);
        virtual audio_devices_t gainTable_checkInvalidDeviceFromCheckAndSetVolume(const sp<AudioOutputDescriptor>& outputDesc, audio_devices_t device);
        virtual status_t gainTable_applyAnalogGainFromCheckAndSetVolume(VolumeSource volumeSource, int index,
                                           const sp<AudioOutputDescriptor>& outputDesc,
                                           audio_devices_t device,
                                           int delayMs, bool force);
        virtual status_t gainTable_setVolumeFromCheckAndSetVolume(VolumeSource volsrc, int index,
                                           const sp<AudioOutputDescriptor>& outputDesc,
                                           audio_devices_t device,
                                           int delayMs, bool force, float volumeDb);
        virtual status_t gainTable_routeAndApplyVolumeFromStopSource(const sp<SwAudioOutputDescriptor>& outputDesc, DeviceVector device,
                                           audio_stream_type_t stream, bool force);
        virtual bool gainTable_skipAdjustGainFromSetStreamVolumeIndex(audio_devices_t curDevice, audio_devices_t wantDevice);
        virtual audio_devices_t gainTable_replaceApplyDeviceFromSetStreamVolumeIndex(audio_devices_t outputDevice, audio_devices_t curDevice);
        virtual void gainTable_initXML();
        virtual status_t common_setPolicyManagerCustomParameters(int par1, int par2, int par3, int par4);
        virtual status_t fm_initOutputIdForApp(void);
        virtual DeviceVector fm_correctDeviceFromSetDeviceConnectionStateInt(const sp<AudioOutputDescriptor>& outputDesc, DeviceVector device, bool force);
        virtual status_t fm_addAudioPatch(audio_patch_handle_t handle, const sp<AudioPatch>& patch);
        virtual status_t fm_removeAudioPatch(audio_patch_handle_t handle);
        virtual status_t fm_applyGainFromCheckAndSetVolume(VolumeSource volumeSource, int index, const sp<AudioOutputDescriptor>& outputDesc,audio_devices_t device,
                                                           int delayMs, bool force);
        virtual status_t fm_muteStrategyFromCheckOutputForStrategy(const audio_attributes_t &attr, DeviceVector oldDevice, DeviceVector newDevice);
        virtual status_t fm_checkSkipVolumeFromCheckOutputForStrategy(const audio_attributes_t &attr, DeviceVector oldDevice, DeviceVector newDevice);
        virtual status_t fm_releaseSkipVolumeFromCheckOutputForStrategy(void);
        virtual bool fm_checkFirstMusicFromStartSource(const sp<AudioOutputDescriptor>& outputDesc, audio_stream_type_t stream);
        virtual uint32_t fm_extendMuteFromCheckDeviceMuteStrategies(const sp<AudioOutputDescriptor>& outputDesc, VolumeSource volumeSource, uint32_t muteDurationMs, uint32_t extendDurationMs);
        virtual status_t fm_signalAPProutingFromSetOutputDevice(const sp<SwAudioOutputDescriptor>& outputDesc, bool force);
        virtual bool setFMIndirectMode(uint32_t sampleRate);
        virtual uint32_t fm_extendSleepFromCheckDeviceMuteStrategies(const sp<AudioOutputDescriptor>& outputDesc, uint32_t muteWaitMs);
        virtual status_t usbPhoneCall_connectFromSetDeviceConnectionState(audio_devices_t device,
                                                      audio_policy_dev_state_t state,
                                                      const char *device_address,
                                                      const char *device_name,
                                                      audio_format_t encodedFormat);
        virtual status_t usbPhoneCall_setOutputDeviceFromUpdateCallRouting(const sp<SwAudioOutputDescriptor>& outputDesc,
                                             DeviceVector rxDevice,
                                             bool force,
                                             int delayMs,
                                             sp<DeviceDescriptor> &txDevice,
                                             uint32_t *muteWaitMs);
        virtual status_t usbPhoneCall_setCurModeFromSetPhoneState(audio_mode_t state);
        virtual status_t usbPhoneCall_closeAllInputsFromSetPhoneState(void);
        virtual audio_devices_t usbPhoneCall_addCurrentVolumeIndexFromSetStreamVolumeIndex(audio_stream_type_t stream,
                                                  int index,
                                                  audio_devices_t device);
        virtual DeviceVector usbPhoneCall_correctDeviceFromGetDevicesForStream(DeviceVector devices);
        virtual bool usbPhoneCall_isSupportUSBPhoneCallDevice(const String8& address, sp<DeviceDescriptor> device);
        virtual status_t gainNvram_remapIndexRangeFromInitStreamVolume(audio_stream_type_t stream,
                                                int *indexMin,
                                                int *indexMax);
        virtual status_t gainNvram_remapIndexFromSetStreamVolumeIndex(audio_stream_type_t stream,
                                                  int *index,
                                                  audio_devices_t device);
        virtual status_t gainNvram_remapIndexFromGetStreamVolumeIndex(audio_stream_type_t stream,
                                                  int *index,
                                                  audio_devices_t device);
        virtual status_t lowLatency_updatePrimaryModuleDeviceFromSetPhoneState(audio_mode_t state, audio_devices_t rxDevice);
        virtual status_t lowLatency_CheckSpeakerProtectionDevice(const sp<IOProfile>& outProfile);
        virtual bool lowLatency_skipSelectedDeviceFormSetOutputDevice(const sp<AudioOutputDescriptor>& outputDesc, audio_devices_t device);
        virtual bool lowLatency_skipOutputCheckFromGetOutputsForDevice(audio_devices_t device, const SwAudioOutputCollection& openOutputs, size_t i);
        virtual bool lowLatency_stopToRouteFromStopSource(audio_devices_t newDevice, const sp<AudioOutputDescriptor>& outputDesc);
        virtual bool lowLatency_startToRouteFromStartSource(const sp<AudioOutputDescriptor>& outputDesc, bool beFirstActive);
        virtual bool lowLatency_isOutputActiveFromStartSource(const sp<AudioOutputDescriptor>& outputDesc);
        virtual bool lowLatency_setOutputFirstActiveFromStartSource(const sp<SwAudioOutputDescriptor>& outputDesc, bool beFirstActive, DeviceVector device);
        virtual bool lowLatency_checkOutputFirstActiveFromSetOutputDevice(const sp<AudioOutputDescriptor>& outputDesc);
        virtual status_t besLoudness_signalDupOutputFromSetOutputDevice(const sp<SwAudioOutputDescriptor>& outputDesc,
                                             const DeviceVector &device,
                                             int delayMs);
        virtual status_t offload_invalidateMusicForInCallFromSetPhoneState(audio_mode_t state, int oldState);
        virtual bool offload_isInCallFromIsOffloadSupported(void);
        virtual bool debug_skipShowLog();
        virtual bool debug_showGetOutputForAttr(audio_devices_t device, const audio_config_t *config, audio_output_flags_t flags, audio_stream_type_t stream, audio_io_handle_t output);
        virtual bool debug_showGetInputForAttr(AudioPolicyInterface::input_type_t inputType, audio_devices_t device, audio_io_handle_t input);
        virtual bool debug_showSetOutputDevice(const sp<SwAudioOutputDescriptor>& outputDesc, const DeviceVector & device, bool force, int delayMs);
        virtual status_t hifiAudio_startOutputSamplerate(audio_port_handle_t portId, int samplerate);
        virtual status_t hifiAudio_stopOutputSamplerate(audio_port_handle_t portId, int samplerate);

    protected:
        enum output_samplerate_index
        {
            OUTPUT_RATE_8_INDEX =0,
            OUTPUT_RATE_11_025_INDEX,
            OUTPUT_RATE_16_INDEX,
            OUTPUT_RATE_22_05_INDEX,
            OUTPUT_RATE_32_INDEX,
            OUTPUT_RATE_44_1_INDEX,
            OUTPUT_RATE_48_INDEX,
            OUTPUT_RATE_88_2_INDEX,
            OUTPUT_RATE_96_INDEX,
            OUTPUT_RATE_176_4_INDEX,
            OUTPUT_RATE_192_INDEX,
            OUTPUT_NUM_RATE_INDEX
        };
        enum output_samplerate
        {
            OUTPUT_RATE_8 = 8000,
            OUTPUT_RATE_11_025 = 11025,
            OUTPUT_RATE_16 = 16000,
            OUTPUT_RATE_22_05 = 22050,
            OUTPUT_RATE_32 = 32000,
            OUTPUT_RATE_44_1 = 44100,
            OUTPUT_RATE_48 = 48000,
            OUTPUT_RATE_88_2 = 88200,
            OUTPUT_RATE_96 = 96000,
            OUTPUT_RATE_176_4 = 176400,
            OUTPUT_RATE_192 = 192000,
        };
        enum output_samplerate_policy
        {
            SampleRate_Do_nothing =0,
            SampleRate_First_Start,
            SampleRate_ForceReplace,
        };
        uint32_t mSampleRateArray[OUTPUT_NUM_RATE_INDEX];
        uint32_t mPolicySampleRate;

        uint32_t mSampleRateFocus;
        uint32_t mSampleRateFocusCount;
        Mutex    mSampleRateFocusLock;
        uint32_t mSampleRate_Policy;

        virtual status_t aaudio_invalidateStreamForInCallFromSetPhoneState(audio_mode_t state, int oldState);
        virtual bool aaudio_conidtionCheck(aaudio_cond_t cond, audio_input_flags_t inflags, audio_output_flags_t outflags, audio_mode_t state);
        virtual status_t aaudio_invalidateMMAPStream(void);
        virtual status_t aaudio_policyForceReplaceSampleRate(sp<SwAudioOutputDescriptor> desc);
        virtual audio_io_handle_t  multipleRecord_policySelectInput(sp<IOProfile>& profile,
                                                                    bool isSoundTrigger,
                                                                    audio_session_t session,
                                                                    const audio_config_base_t *config,
                                                                    audio_input_flags_t flags,
                                                                    bool forceOpen = false);
    private:
        AudioPolicyManager *mAudioPolicyManager;    // For public function
        audio_stream_type_t mVolumeStream;
        int                 mVolumeIndex;
        audio_devices_t     mVolumeDevice;
        GainTableParam mGainTable;
        int   mGainTableSceneIndex;
        int   mGainTableSceneCount;
        audio_devices_t mDeviceOfCheckAndSetVolume;
        bool mFMDirectAudioPatchEnable;
        bool mSkipFMVolControl;
        audio_port_handle_t mFmPortId;
        bool mUsbHeadsetConnect;
        bool mNeedRemapVoiceVolumeIndex;
        AUDIO_CUSTOM_VOLUME_STRUCT mAudioCustVolumeTable;
        float linearToLog(int volume);
        int logToLinear(float volume);
        float linearToLogHifi(int volume);
        int hifiIndexToAnalog(int index);
        bool setHIFIVolumeMode(uint32_t sampleRate);
        bool mHifiVolumeMode;
        int mHifiState;
        bool mSampleRateForce;
        status_t checkAndSetGainTableAnalogGain(audio_stream_type_t stream, int index, const sp<AudioOutputDescriptor>& outputDesc,audio_devices_t device,
                                                           int delayMs, bool force);
        audio_stream_type_t selectGainTableActiveStream(audio_stream_type_t requestStream);
        float computeGainTableCustomVolume(audio_stream_type_t stream, int index, audio_devices_t device);
        enum tty_mode_t
        {
            AUD_TTY_OFF  = 0,
            AUD_TTY_FULL = 1,
            AUD_TTY_VCO  = 2,
            AUD_TTY_HCO  = 4,
            AUD_TTY_ERR  = -1
        };
        tty_mode_t  mTty_Ctm;
        audio_devices_t getNewDeviceForTty(audio_devices_t device, tty_mode_t tty_mode);
        bool isFMDirectMode(const sp<AudioPatch>& patch);
        bool isFMActive(void);
        bool isFMDirectActive(void);
        // manages USB output suspend/restore according to phone state and USB usage
        // If changed, return true. MTK_AUDIO/MTK_USB_PHONECALL
        bool checkUsbSuspend(audio_devices_t device = AUDIO_DEVICE_NONE);
        int mapVol(float &vol, float unitstep);
        int mappingVoiceVol(float &vol, float unitstep);
        int getStreamMaxLevels(int stream);
        float mapVoltoCustomVol(unsigned char array[], int volmin, int volmax, float &vol ,int stream);
        float mapVoiceVoltoCustomVol(unsigned char array[], int volmin, int volmax, float &vol, int vol_stream_type);
        float computeCustomVolume(int stream, int index, audio_devices_t device);
        void loadCustomVolume();
        status_t updatePrimaryModuleDevice(audio_devices_t rxDevice);
        sp<SwAudioOutputDescriptor> mFMOutput;     // primary or deepbuf output descriptor
        status_t freeGainTable(void);
        status_t allocateGainTable(void);
        status_t updateCurrentSceneIndexOfGainTable(void);
        int getSceneIndexOfGainTable(String8 sceneName);
        uint32_t getSampleRateIndex(uint32_t sampleRate);
        uint32_t getSampleRateCount();
        bool checkFirstActive(void);
        bool checkStreamActive(void);
        void dumpSampleRateArray(void);
        uint32_t getFirstTrackSampleRate();
        bool isPrimarySupportSampleRate(uint32_t SampleRate);
        status_t policyFirstStart(audio_stream_type_t stream , uint32_t sampleRate);
        status_t policyRestore(audio_stream_type_t stream);
        status_t policyForceReplace(audio_stream_type_t stream, uint32_t sampleRate, bool forceReplace = false);
        status_t sampleRatePolicy(audio_stream_type_t stream, uint32_t sampleRate);
        status_t addSampleRateArray(audio_stream_type_t stream __unused,uint32_t sampleRate);
        status_t removeSampleRateArray(audio_stream_type_t stream __unused, uint32_t sampleRate);
        void initSamplerateArray(uint32_t init_sample_rate);
        uint32_t getMaxTrackSampleRate(audio_stream_type_t stream = AUDIO_STREAM_DEFAULT);
        audio_io_handle_t getPrimaryFastOutput();
        bool isSupportDeepBuffer(void);
        bool setHIFIState(int enable);
        int getHIFIState(void);
        bool isMMAPActive(void);
        audio_io_handle_t getVOIPRxOutput(void);
};

};



