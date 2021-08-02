/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef ANDROID_HARDWARE_AUDIO_V4_0_PRIMARYDEVICE_H
#define ANDROID_HARDWARE_AUDIO_V4_0_PRIMARYDEVICE_H

#include <android/hardware/audio/4.0/IPrimaryDevice.h>

#define MAJOR_VERSION 4
#define MINOR_VERSION 0

#include <VersionMacro.h>

#include <hidl/Status.h>

#include <hidl/MQDescriptor.h>

#include "Device.h"

namespace android {
namespace hardware {
namespace audio {
namespace V4_0 {
namespace implementation {

using ::android::hardware::audio::common::V4_0::AudioConfig;
using ::android::hardware::audio::common::V4_0::AudioInputFlag;
using ::android::hardware::audio::common::V4_0::AudioMode;
using ::android::hardware::audio::common::V4_0::AudioOutputFlag;
using ::android::hardware::audio::common::V4_0::AudioPort;
using ::android::hardware::audio::common::V4_0::AudioPortConfig;
using ::android::hardware::audio::common::V4_0::AudioSource;
using ::android::hardware::audio::V4_0::DeviceAddress;
using ::android::hardware::audio::V4_0::IDevice;
using ::android::hardware::audio::V4_0::IPrimaryDevice;
using ::android::hardware::audio::V4_0::IStreamIn;
using ::android::hardware::audio::V4_0::IStreamOut;
using ::android::hardware::audio::V4_0::ParameterValue;
using ::android::hardware::audio::V4_0::Result;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using ::android::sp;

struct PrimaryDevice : public IPrimaryDevice {
    explicit PrimaryDevice(audio_hw_device_mtk_t *device);

    // Methods from ::android::hardware::audio::V4_0::IDevice follow.
    Return<Result> initCheck()  override;
    Return<Result> setMasterVolume(float volume)  override;
    Return<void> getMasterVolume(getMasterVolume_cb _hidl_cb)  override;
    Return<Result> setMicMute(bool mute)  override;
    Return<void> getMicMute(getMicMute_cb _hidl_cb)  override;
    Return<Result> setMasterMute(bool mute)  override;
    Return<void> getMasterMute(getMasterMute_cb _hidl_cb)  override;
    Return<void> getInputBufferSize(const AudioConfig &config,
                                    getInputBufferSize_cb _hidl_cb)  override;
    Return<void> openOutputStream(int32_t ioHandle, const DeviceAddress& device,
                                  const AudioConfig& config, AudioOutputFlagBitfield flags,
                                  const SourceMetadata& sourceMetadata,
                                  openOutputStream_cb _hidl_cb) override;

    Return<void> openInputStream(int32_t ioHandle, const DeviceAddress& device,
                                 const AudioConfig& config, AudioInputFlagBitfield flags,
                                 AudioSource source, openInputStream_cb _hidl_cb);
    Return<void> openInputStream(int32_t ioHandle, const DeviceAddress& device,
                                 const AudioConfig& config, AudioInputFlagBitfield flags,
                                 const SinkMetadata& sinkMetadata,
                                 openInputStream_cb _hidl_cb) override;

    Return<bool> supportsAudioPatches()  override;
    Return<void> createAudioPatch(const hidl_vec<AudioPortConfig>& sources,
        const hidl_vec<AudioPortConfig> &sinks,
        createAudioPatch_cb _hidl_cb)  override;
    Return<Result> releaseAudioPatch(int32_t patch)  override;
    Return<void> getAudioPort(const AudioPort &port, getAudioPort_cb _hidl_cb)  override;
    Return<Result> setAudioPortConfig(const AudioPortConfig &config)  override;

    Return<Result> setScreenState(bool turnedOn)  override;

    Return<void> getHwAvSync(getHwAvSync_cb _hidl_cb) override;
    Return<void> getParameters(const hidl_vec<ParameterValue>& context,
                               const hidl_vec<hidl_string>& keys,
                               getParameters_cb _hidl_cb) override;
    Return<Result> setParameters(const hidl_vec<ParameterValue>& context,
                                 const hidl_vec<ParameterValue>& parameters) override;
    Return<void> getMicrophones(getMicrophones_cb _hidl_cb) override;
    Return<Result> setConnectedState(const DeviceAddress& address, bool connected) override;

    Return<void> debug(const hidl_handle& fd, const hidl_vec<hidl_string>& options) override;

    // Methods from ::android::hardware::audio::V4_0::IPrimaryDevice follow.
    Return<Result> setVoiceVolume(float volume)  override;
    Return<Result> setMode(AudioMode mode)  override;
    Return<void> getBtScoNrecEnabled(getBtScoNrecEnabled_cb _hidl_cb)  override;
    Return<Result> setBtScoNrecEnabled(bool enabled)  override;
    Return<void> getBtScoWidebandEnabled(getBtScoWidebandEnabled_cb _hidl_cb)  override;
    Return<Result> setBtScoWidebandEnabled(bool enabled)  override;
    Return<void> getTtyMode(getTtyMode_cb _hidl_cb)  override;
    Return<Result> setTtyMode(IPrimaryDevice::TtyMode mode)  override;
    Return<void> getHacEnabled(getHacEnabled_cb _hidl_cb)  override;
    Return<Result> setHacEnabled(bool enabled)  override;

    Return<Result> setBtScoHeadsetDebugName(const hidl_string& name) override;
    Return<void> getBtHfpEnabled(getBtHfpEnabled_cb _hidl_cb) override;
    Return<Result> setBtHfpEnabled(bool enabled) override;
    Return<Result> setBtHfpSampleRate(uint32_t sampleRateHz) override;
    Return<Result> setBtHfpVolume(float volume) override;
    Return<Result> updateRotation(IPrimaryDevice::Rotation rotation) override;

private:
    sp<Device> mDevice;

    virtual ~PrimaryDevice();
};

}  // namespace implementation
}  // namespace V4_0
}  // namespace audio
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_AUDIO_V4_0_PRIMARYDEVICE_H
