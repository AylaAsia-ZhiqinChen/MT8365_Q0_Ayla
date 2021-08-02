#pragma once

#include "managerdefault/AudioPolicyManager.h"

namespace android {

class AudioPolicyManager;

typedef enum {
    AAUDIO_COND_GET_OUTPUT_FOR_DEVICE = 0,
    AAUDIO_COND_GET_INPUT_FOR_ATTR,
    AAUDIO_COND_GET_INPUT_FOR_DEVICE,
    AAUDIO_COND_MAX,
} aaudio_cond_t;

class AudioPolicyManagerCustomInterface
{
    public:
        virtual ~AudioPolicyManagerCustomInterface() {}
        virtual status_t common_set(AudioPolicyManager *audioPolicyManger) = 0;
        virtual audio_stream_type_t gainTable_getVolumeStream() = 0;
        virtual int gainTable_getVolumeIndex() = 0;
        virtual audio_devices_t gainTable_getVolumeDevice() = 0;
        virtual status_t gainTable_setVolumeStream(audio_stream_type_t stream) = 0;
        virtual status_t gainTable_setVolumeIndex(int index) = 0;
        virtual status_t gainTable_setVolumeDevice(audio_devices_t device) = 0;
        virtual status_t gainTable_getCustomAudioVolume(void) = 0;
        virtual float gainTable_getVolumeDbFromComputeVolume(VolumeSource volsrc, int index, audio_devices_t device, float volumeDB) = 0;
        virtual audio_devices_t gainTable_getDeviceFromComputeVolume(VolumeSource volumeSource, int index, audio_devices_t device) = 0;
        virtual float gainTable_getCorrectVolumeDbFromComputeVolume(VolumeSource volumeSource, float volumeDB, audio_devices_t device) = 0;
        virtual audio_devices_t gainTable_checkInvalidDeviceFromCheckAndSetVolume(const sp<AudioOutputDescriptor>& outputDesc, audio_devices_t device) = 0;
        virtual status_t gainTable_applyAnalogGainFromCheckAndSetVolume(VolumeSource volumeSource, int index,
                                           const sp<AudioOutputDescriptor>& outputDesc,
                                           audio_devices_t device,
                                           int delayMs, bool force) = 0;
        virtual status_t gainTable_setVolumeFromCheckAndSetVolume(VolumeSource volsrc, int index,
                                           const sp<AudioOutputDescriptor>& outputDesc,
                                           audio_devices_t device,
                                           int delayMs, bool force, float volumeDb) = 0;
        virtual status_t gainTable_routeAndApplyVolumeFromStopSource(const sp<SwAudioOutputDescriptor>& outputDesc, DeviceVector device,
                                           audio_stream_type_t stream, bool force) = 0;
        virtual bool gainTable_skipAdjustGainFromSetStreamVolumeIndex(audio_devices_t curDevice, audio_devices_t wantDevice) = 0;
        virtual audio_devices_t gainTable_replaceApplyDeviceFromSetStreamVolumeIndex(audio_devices_t outputDevice, audio_devices_t curDevice) = 0;
        virtual void gainTable_initXML();
        virtual status_t common_setPolicyManagerCustomParameters(int par1, int par2, int par3, int par4) = 0;
        virtual status_t fm_initOutputIdForApp(void) = 0;
        virtual DeviceVector fm_correctDeviceFromSetDeviceConnectionStateInt(const sp<AudioOutputDescriptor>& outputDesc, DeviceVector device, bool force) = 0;
        virtual status_t fm_addAudioPatch(audio_patch_handle_t handle, const sp<AudioPatch>& patch) = 0;
        virtual status_t fm_removeAudioPatch(audio_patch_handle_t handle) = 0;
        virtual status_t fm_applyGainFromCheckAndSetVolume(VolumeSource volumeSource, int index, const sp<AudioOutputDescriptor>& outputDesc,audio_devices_t device,
                                                           int delayMs, bool force) = 0;
        virtual status_t fm_muteStrategyFromCheckOutputForStrategy(const audio_attributes_t &attr, DeviceVector oldDevice, DeviceVector newDevice) = 0;
        virtual status_t fm_checkSkipVolumeFromCheckOutputForStrategy(const audio_attributes_t &attr, DeviceVector oldDevice, DeviceVector newDevice) = 0;
        virtual status_t fm_releaseSkipVolumeFromCheckOutputForStrategy(void) = 0;
        virtual bool fm_checkFirstMusicFromStartSource(const sp<AudioOutputDescriptor>& outputDesc, audio_stream_type_t stream) = 0;
        virtual uint32_t fm_extendMuteFromCheckDeviceMuteStrategies(const sp<AudioOutputDescriptor>& outputDesc, VolumeSource volumeSource, uint32_t muteDurationMs, uint32_t extendDurationMs) = 0;
        virtual status_t fm_signalAPProutingFromSetOutputDevice(const sp<SwAudioOutputDescriptor>& outputDesc, bool force) = 0;
        virtual uint32_t fm_extendSleepFromCheckDeviceMuteStrategies(const sp<AudioOutputDescriptor>& outputDesc, uint32_t muteWaitMs) = 0;
        virtual status_t usbPhoneCall_connectFromSetDeviceConnectionState(audio_devices_t device,
                                                      audio_policy_dev_state_t state,
                                                      const char *device_address,
                                                      const char *device_name,
                                                      audio_format_t encodedFormat) = 0;
        virtual status_t usbPhoneCall_setOutputDeviceFromUpdateCallRouting(const sp<SwAudioOutputDescriptor>& outputDesc,
                                             DeviceVector rxDevice,
                                             bool force,
                                             int delayMs,
                                             sp<DeviceDescriptor> &txDevice,
                                             uint32_t *muteWaitMs) = 0;
        virtual status_t usbPhoneCall_setCurModeFromSetPhoneState(audio_mode_t state) = 0;
        virtual status_t usbPhoneCall_closeAllInputsFromSetPhoneState(void) = 0;
        virtual audio_devices_t usbPhoneCall_addCurrentVolumeIndexFromSetStreamVolumeIndex(audio_stream_type_t stream,
                                                  int index,
                                                  audio_devices_t device) = 0;
        virtual DeviceVector usbPhoneCall_correctDeviceFromGetDevicesForStream(DeviceVector devices) = 0;
        virtual bool usbPhoneCall_isSupportUSBPhoneCallDevice(const String8& address, sp<DeviceDescriptor> device) = 0;
        virtual status_t gainNvram_remapIndexRangeFromInitStreamVolume(audio_stream_type_t stream,
                                            int *indexMin,
                                            int *indexMax) = 0;
        virtual status_t gainNvram_remapIndexFromSetStreamVolumeIndex(audio_stream_type_t stream,
                                                  int *index,
                                                  audio_devices_t device) = 0;
        virtual status_t gainNvram_remapIndexFromGetStreamVolumeIndex(audio_stream_type_t stream,
                                          int *index,
                                          audio_devices_t device) = 0;
        virtual status_t lowLatency_updatePrimaryModuleDeviceFromSetPhoneState(audio_mode_t state, audio_devices_t rxDevice) = 0;
        virtual status_t lowLatency_CheckSpeakerProtectionDevice(const sp<IOProfile>& outProfile) = 0;
        virtual bool lowLatency_skipSelectedDeviceFormSetOutputDevice(const sp<AudioOutputDescriptor>& outputDesc, audio_devices_t device) = 0;
        virtual bool lowLatency_skipOutputCheckFromGetOutputsForDevice(audio_devices_t device, const SwAudioOutputCollection& openOutputs, size_t i) = 0;
        virtual bool lowLatency_stopToRouteFromStopSource(audio_devices_t newDevice, const sp<AudioOutputDescriptor>& outputDesc) = 0;
        virtual bool lowLatency_startToRouteFromStartSource(const sp<AudioOutputDescriptor>& outputDesc, bool beFirstActive) = 0;
        virtual bool lowLatency_isOutputActiveFromStartSource(const sp<AudioOutputDescriptor>& outputDesc) = 0;
        virtual bool lowLatency_setOutputFirstActiveFromStartSource(const sp<SwAudioOutputDescriptor>& outputDesc, bool beFirstActive, DeviceVector device) = 0;
        virtual bool lowLatency_checkOutputFirstActiveFromSetOutputDevice(const sp<AudioOutputDescriptor>& outputDesc) = 0;
        virtual status_t besLoudness_signalDupOutputFromSetOutputDevice(const sp<SwAudioOutputDescriptor>& outputDesc,
                                             const DeviceVector &device,
                                             int delayMs) = 0;
        virtual status_t offload_invalidateMusicForInCallFromSetPhoneState(audio_mode_t state, int oldState) = 0;
        virtual bool offload_isInCallFromIsOffloadSupported(void) = 0;
        virtual bool debug_skipShowLog() = 0;
        virtual bool debug_showGetOutputForAttr(audio_devices_t device, const audio_config_t *config, audio_output_flags_t flags, audio_stream_type_t stream, audio_io_handle_t output) = 0;
        virtual bool debug_showGetInputForAttr(AudioPolicyInterface::input_type_t inputType, audio_devices_t device, audio_io_handle_t input) = 0;
        virtual bool debug_showSetOutputDevice(const sp<SwAudioOutputDescriptor>& outputDesc, const DeviceVector & device, bool force, int delayMs) = 0;
        virtual status_t hifiAudio_startOutputSamplerate(audio_port_handle_t portId, int samplerate) = 0;
        virtual status_t hifiAudio_stopOutputSamplerate(audio_port_handle_t portId, int samplerate) = 0;
        virtual status_t aaudio_invalidateStreamForInCallFromSetPhoneState(audio_mode_t state, int oldState) = 0;
        virtual bool aaudio_conidtionCheck(aaudio_cond_t cond, audio_input_flags_t inflags, audio_output_flags_t outflags, audio_mode_t state) = 0;
        virtual status_t aaudio_invalidateMMAPStream(void) = 0;
        virtual status_t aaudio_policyForceReplaceSampleRate(sp<SwAudioOutputDescriptor> desc) = 0;
        virtual audio_io_handle_t  multipleRecord_policySelectInput(sp<IOProfile>& profile,
                                                                    bool isSoundTrigger,
                                                                    audio_session_t session,
                                                                    const audio_config_base_t *config,
                                                                    audio_input_flags_t flags,
                                                                    bool forceOpen = false) = 0;
};

};

