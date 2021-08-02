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

#define LOG_TAG "AudioPolicyManagerCustomImpl"
//#define LOG_NDEBUG 0

//#define VERY_VERBOSE_LOGGING
#if defined(MTK_AUDIO_DEBUG)
#if defined(CONFIG_MT_ENG_BUILD)
#define LOG_NDEBUG 0
#define VERY_VERBOSE_LOGGING
#endif
#endif
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#define AUDIO_POLICY_XML_CONFIG_FILE_PATH_MAX_LENGTH 128
#define AUDIO_POLICY_XML_CONFIG_FILE_NAME "audio_policy_configuration.xml"

#include <inttypes.h>
#include <math.h>

#include <AudioPolicyManagerInterface.h>
//#include <AudioPolicyEngineInstance.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include <media/AudioParameter.h>
#include <soundtrigger/SoundTrigger.h>
#include <system/audio.h>
#include <audio_policy_conf.h>
#include <Serializer.h>
#include "TypeConverter.h"
#include <policy.h>
#include "AudioPolicyManagerCustomImpl.h"
#include "AudioPolicyServiceCustomImpl.h"
#if defined(MTK_AUDIO)
#include <media/mediarecorder.h>
#include "AudioPolicyParameters.h"
#include <media/AudioUtilmtk.h>
#endif
#include <media/MtkLogger.h>
#define MTK_LOG_LEVEL_SILENCE (MT_AUDIO_ENG_BUILD_LEVEL + 1) // only enable if necessary
//<< for MTK_AUDIO_GAIN_TABLE
#include "AudioCompFltCustParam.h"
#include "AudioGainTableParam.h"
#include "AudioParamParser.h"
#include "AudioPolicyParameters.h"
//MTK_AUDIO_GAIN_TABLE>>

// <<for both MTK_AUDIO_GAIN_TABLE and MTK_AUDIO_GAIN_NVRAM
// total 63.5 dB
static const float KeydBPerStep = 0.25f;
static const float KeyvolumeStep = 255.0f;
// shouldn't need to touch these
static const float KeydBConvert = -KeydBPerStep * 2.302585093f / 20.0f;
static const float KeydBConvertInverse = 1.0f / KeydBPerStep;
// for both MTK_AUDIO_GAIN_TABLE and MTK_AUDIO_GAIN_NVRAM>>

namespace android {
//for MTK_AUDIO_GAIN_TABLE only
const char *kFltAudioTypeFileName[] = {"PlaybackACF", "PlaybackHCF", "", "", "", "PlaybackDRC", "PlaybackDRC"};
const char *kStringUpdateFLT[] = {"UpdateACFHCFParameters=0", "UpdateACFHCFParameters=1", "", "", "UpdateACFHCFParameters=2", "UpdateACFHCFParameters=3", "UpdateACFHCFParameters=4"};

void gainTableACFXmlChangedCb(AppHandle *appHandle, const char *audioTypeName)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        // reload XML file
        AppOps* appOps = appOpsGetInstance();
        (void) appHandle;
        if (appOps == NULL) {
            ALOGE("%s(), Error: AppOps == NULL", __FUNCTION__);
            return;
        }

        if (appOps->appHandleReloadAudioType(appHandle, audioTypeName) == APP_ERROR) {
            ALOGE("%s(), Reload xml fail!(audioType = %s)", __FUNCTION__, audioTypeName);
        } else {

            if (strcmp(audioTypeName, PLAY_DIGI_AUDIOTYPE_NAME) == 0 ||
                strcmp(audioTypeName, SPEECH_VOL_AUDIOTYPE_NAME) == 0 ||
                strcmp(audioTypeName, VOIP_VOL_AUDIOTYPE_NAME) == 0 ||
                strcmp(audioTypeName, VOLUME_AUDIOTYPE_NAME) == 0 ||
                strcmp(audioTypeName, GAIN_MAP_AUDIOTYPE_NAME) == 0) {
                ALOGD("ReloadAudioVolume %s", audioTypeName);
                AudioSystem::setParameters(String8("ReloadAudioVolume"));
            }
            else if ((!FeatureOption::MTK_AURISYS_FRAMEWORK_SUPPORT)  && strcmp(audioTypeName, kFltAudioTypeFileName[AUDIO_COMP_FLT_AUDIO]) == 0) {
                // "PlaybackACF"
                ALOGD("PlaybackACF:UpdateACFHCFParameters=0 +");
                AudioSystem::setParameters(0, String8(kStringUpdateFLT[AUDIO_COMP_FLT_AUDIO]));
                ALOGD("PlaybackACF:UpdateACFHCFParameters=0 -");
            } else if ((!FeatureOption::MTK_AURISYS_FRAMEWORK_SUPPORT)  && strcmp(audioTypeName, kFltAudioTypeFileName[AUDIO_COMP_FLT_HEADPHONE]) == 0) {
                // "PlaybackHCF"
                ALOGD("PlaybackHCF:UpdateACFHCFParameters=1 +");
                AudioSystem::setParameters(0, String8(kStringUpdateFLT[AUDIO_COMP_FLT_HEADPHONE]));
                ALOGD("PlaybackHCF:UpdateACFHCFParameters=1 -");
            }
        }
    }
    return;
}

static int initXMLUpdate()
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        /* XML changed callback process */
        AppOps* appOps = appOpsGetInstance();
        if (appOps == NULL) {
            ALOGE("%s(), Error: AppOps == NULL", __FUNCTION__);
            return -1;
        }
        appOps->appHandleRegXmlChangedCb(appOps->appHandleGetInstance(), gainTableACFXmlChangedCb);
        ALOGD("Register gainTableACFXmlChangedCb");
    }
    return 0;
}

static int deinitXMLUpdate()
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        appOpsDelInstance();
        ALOGD("UnRegister gainTableACFXmlChangedCb");
    }
    return 0;
}

AudioPolicyManagerCustomImpl::AudioPolicyManagerCustomImpl()
{
    mAudioPolicyManager = NULL;
    mVolumeStream = AUDIO_STREAM_DEFAULT;
    mVolumeIndex = -1;
    mVolumeDevice = AUDIO_DEVICE_NONE;
    mDeviceOfCheckAndSetVolume = AUDIO_DEVICE_NONE;
    memset(&mGainTable, 0, sizeof(mGainTable));
    mGainTableSceneIndex = 0;
    mGainTableSceneCount = 1;
    mTty_Ctm = AUD_TTY_OFF;
    mFMDirectAudioPatchEnable = false;
    mSkipFMVolControl = false;
    mUsbHeadsetConnect = false;
    mNeedRemapVoiceVolumeIndex = false;
    memset(&mAudioCustVolumeTable, 0, sizeof(mAudioCustVolumeTable));
    InitializeMTKLogLevel("vendor.af.policy.debug");
    initSamplerateArray(0);
    mHifiVolumeMode = false;
    mHifiState = -1;
    mSampleRateForce = false;
    mFmPortId = 0;
}


AudioPolicyManagerCustomImpl::~AudioPolicyManagerCustomImpl()
{
    ALOGD("%s()", __FUNCTION__);
    deinitXMLUpdate();
    freeGainTable();
}

status_t AudioPolicyManagerCustomImpl::common_set(AudioPolicyManager *audioPolicyManger)
{
    mAudioPolicyManager = audioPolicyManger;
    ALOGD("Set mAudioPolicyManager with %p", audioPolicyManger);
    return NO_ERROR;
}
audio_stream_type_t AudioPolicyManagerCustomImpl::gainTable_getVolumeStream()
{
    return mVolumeStream;
}
int AudioPolicyManagerCustomImpl::gainTable_getVolumeIndex()
{
    return mVolumeIndex;
}
audio_devices_t AudioPolicyManagerCustomImpl::gainTable_getVolumeDevice()
{
    return mVolumeDevice;
}
status_t AudioPolicyManagerCustomImpl::gainTable_setVolumeStream(audio_stream_type_t stream)
{
    mVolumeStream = stream;
    return NO_ERROR;
}
status_t AudioPolicyManagerCustomImpl::gainTable_setVolumeIndex(int index)
{
    mVolumeIndex = index;
    return NO_ERROR;
}
status_t AudioPolicyManagerCustomImpl::gainTable_setVolumeDevice(audio_devices_t device)
{
    mVolumeDevice = device;
    return NO_ERROR;
}
status_t AudioPolicyManagerCustomImpl::gainTable_getCustomAudioVolume(void)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        if (mAudioPolicyManager == NULL) {
            ALOGE("FatalErr on %s, mAudioPolicyManager unint ", __FUNCTION__);
            return NO_INIT;
        }

        if (allocateGainTable()) {
            ALOGE("error, load GainTable failed!!");
            mAudioPolicyManager->mAudioPolicyVendorControl.setCustomVolumeStatus(false);
        } else {
            mAudioPolicyManager->mAudioPolicyVendorControl.setCustomVolumeStatus(true);
        }
        return NO_ERROR;
    } else if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        mAudioCustVolumeTable.bRev = CUSTOM_VOLUME_REV_1;
        mAudioCustVolumeTable.bReady = 0;
        mAudioPolicyManager->mpClientInterface->getCustomAudioVolume(&mAudioCustVolumeTable);
        if (mAudioCustVolumeTable.bReady != 0) {
            ALOGD("mUseCustomVolume true");
            mAudioPolicyManager->mAudioPolicyVendorControl.setCustomVolumeStatus(true);
        } else {
            ALOGD("mUseCustomVolume false");
           mAudioPolicyManager-> mAudioPolicyVendorControl.setCustomVolumeStatus(false);
        }
        return NO_ERROR;
    } else {
        ALOGE("GainTable Feature option not available");
        return INVALID_OPERATION;
    }
}
float AudioPolicyManagerCustomImpl::gainTable_getVolumeDbFromComputeVolume(VolumeSource volsrc, int index, audio_devices_t device, float volumeDB)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        if (mAudioPolicyManager == NULL) {
            ALOGE("FatalErr on %s, mAudioPolicyManager unint ", __FUNCTION__);
            return 0.0;
        }
        auto stream = mAudioPolicyManager->getVolumeCurves(volsrc).getStreamTypes().front();

        if (mAudioPolicyManager->mAudioPolicyVendorControl.getCustomVolumeStatus()) {
            if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
                volumeDB = Volume::AmplToDb(computeGainTableCustomVolume(stream, index, device));
            } else {
                volumeDB = Volume::AmplToDb(computeCustomVolume(stream, index, device));
            }
            MTK_ALOGS(4,"%s, volumeDB %f, volsrc %d, stream %d, index %d, device %d", __FUNCTION__, volumeDB, volsrc, stream, index, device);
        } else {
            ALOGW("%s,not Customer Volume, Using Android Volume Curve", __FUNCTION__);
            auto &curves = mAudioPolicyManager->getVolumeCurves(volsrc);
            volumeDB = curves.volIndexToDb(Volume::getDeviceCategory(device), index);
        }
    } else {
        ALOGE("GainTable Feature option not available");
    }
    return volumeDB;
}
audio_devices_t AudioPolicyManagerCustomImpl::gainTable_getDeviceFromComputeVolume(VolumeSource volumeSource, int index, audio_devices_t device)
{
    (void) index;
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        if (mAudioPolicyManager == NULL) {
            ALOGE("FatalErr on %s, mAudioPolicyManager unint ", __FUNCTION__);
            return device;
        }
        audio_stream_type_t stream = AUDIO_STREAM_DEFAULT;
        for (int i = 0; i < AUDIO_STREAM_FOR_POLICY_CNT; i++) {
            if (volumeSource == mAudioPolicyManager->toVolumeSource((audio_stream_type_t)i)) {
                stream = (audio_stream_type_t)i;
                break;
            }
        }

        DeviceVector Streamdevices = mAudioPolicyManager->mEngine->getOutputDevicesForStream(stream, false /*fromCache*/);
        if ((device & Streamdevices.types()) && (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_MUSIC, SONIFICATION_HEADSET_MUSIC_DELAY) ||
                mAudioPolicyManager->mLimitRingtoneVolume)) {
            device = Streamdevices.types();
        }
    } else {
        ALOGE("GainTable Feature option not available");
    }
    return device;
}

float AudioPolicyManagerCustomImpl::gainTable_getCorrectVolumeDbFromComputeVolume(VolumeSource volumeSource, float volumeDB, audio_devices_t device)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        if (mAudioPolicyManager == NULL) {
            ALOGE("FatalErr on %s, mAudioPolicyManager unint ", __FUNCTION__);
            return volumeDB;
        }
        if ((device & AUDIO_DEVICE_OUT_AUX_DIGITAL) &&
             (volumeSource == mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_RING) ||
             (volumeSource == mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_ALARM)) ||
             (volumeSource == mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_NOTIFICATION)))) {
            ALOGD("AUDIO_DEVICE_OUT_AUX_DIGITAL device = 0x%x volumeSource = %d", device, volumeSource);
            if (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_MUSIC, SONIFICATION_HEADSET_MUSIC_DELAY)) {
                if (volumeDB < SONIFICATION_AUX_DIGITAL_VOLUME_FACTOR_DB) {
                    while(volumeDB < SONIFICATION_AUX_DIGITAL_VOLUME_FACTOR_DB)
                        volumeDB = volumeDB + (SONIFICATION_AUX_DIGITAL_VOLUME_FACTOR_DB*(-1));
                }
            }
        }
    } else {
        ALOGE("GainTable Feature option not available");
    }
    return volumeDB;
}

audio_devices_t AudioPolicyManagerCustomImpl::gainTable_checkInvalidDeviceFromCheckAndSetVolume(const sp<AudioOutputDescriptor>& outputDesc, audio_devices_t device)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        if (mAudioPolicyManager == NULL) {
            ALOGE("FatalErr on %s, mAudioPolicyManager unint ", __FUNCTION__);
            return device;
        }
        mDeviceOfCheckAndSetVolume = device;
        if ((device != AUDIO_DEVICE_NONE) && !outputDesc->isDuplicated()) {
            if ((device & outputDesc->supportedDevices().types()) == 0) {
                ALOGE("%s invalid set device [0x%x] volume to mId[%d](device support 0x%x), use [0x%x]",
                    __FUNCTION__, device, outputDesc->getId(), outputDesc->supportedDevices().types(), outputDesc->devices().types());
                device = AUDIO_DEVICE_NONE;
            }
        }
    } else {
        ALOGE("GainTable Feature option not available");
    }
    return device;
}

status_t AudioPolicyManagerCustomImpl::gainTable_applyAnalogGainFromCheckAndSetVolume(VolumeSource volumeSource, int index,
                                                                                                   const sp<AudioOutputDescriptor>& outputDesc,
                                                                                                   audio_devices_t device,
                                                                                                   int delayMs, bool force)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        (void) device;
        audio_stream_type_t stream = AUDIO_STREAM_DEFAULT;
        for (int i = 0; i < AUDIO_STREAM_FOR_POLICY_CNT; i++) {
            if (volumeSource == mAudioPolicyManager->toVolumeSource((audio_stream_type_t)i)) {
                stream = (audio_stream_type_t)i;
                break;
            }
        }
        if (mAudioPolicyManager == NULL) {
            ALOGE("FatalErr on %s, mAudioPolicyManager unint ", __FUNCTION__);
            return NO_INIT;
        } else if (mAudioPolicyManager->mPrimaryOutput == NULL) {
            ALOGVV("mPrimaryOutput is not ready");
            return NO_INIT;
        }
        audio_devices_t originDevice = mDeviceOfCheckAndSetVolume;
        if (outputDesc->isDuplicated()) {
            originDevice = mDeviceOfCheckAndSetVolume & (mAudioPolicyManager->mPrimaryOutput->supportedDevices().types());
        }
        if (outputDesc->sharesHwModuleWith(mAudioPolicyManager->mPrimaryOutput) && originDevice != AUDIO_DEVICE_NONE) {
            checkAndSetGainTableAnalogGain(stream, index, outputDesc, outputDesc->devices().types(), delayMs, force);
        }
        return NO_ERROR;
        } else if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        (void) force;
        if (volumeSource == mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_VOICE_CALL) ||
            volumeSource == mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_BLUETOOTH_SCO)) {
            float voiceVolume;
            // Force voice volume to max for bluetooth SCO as volume is managed by the headset
            if (volumeSource == mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_VOICE_CALL)) {
                if ((mAudioPolicyManager->mEngine->getPhoneState() == AUDIO_MODE_IN_COMMUNICATION)) {
                    return NO_ERROR;
                }
                if (mAudioPolicyManager->mAudioPolicyVendorControl.getCustomVolumeStatus()) {
                    voiceVolume = computeCustomVolume((audio_stream_type_t) volumeSource, index, device);
                } else {
                    auto &curves = mAudioPolicyManager->getVolumeCurves(volumeSource);
                    voiceVolume = (float)index / (float)curves.getVolumeIndexMax();
                }
            } else {
                voiceVolume = 1.0;
                // it should mute BT if changing to BT from HP/SPK. But hal desn't support 0
                // keep the same value, at least original won't burst before changing device (ALPS02474519)
                // if(outputDesc->mMuteTid[stream] == gettid()) {
                    // voiceVolume = mLastVoiceVolume;
                // }
            }

            if (voiceVolume != mAudioPolicyManager->mLastVoiceVolume && outputDesc == mAudioPolicyManager->mPrimaryOutput) {
                mAudioPolicyManager->mpClientInterface->setVoiceVolume(voiceVolume, delayMs);
                mAudioPolicyManager->mLastVoiceVolume = voiceVolume;
            }
        }
        return NO_ERROR;
    } else {
        ALOGE("GainTable Feature option not available");
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::gainTable_setVolumeFromCheckAndSetVolume(VolumeSource volsrc, int index,
                                           const sp<AudioOutputDescriptor>& outputDesc,
                                           audio_devices_t device,
                                           int delayMs, bool force, float volumeDb)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
    if (mAudioPolicyManager == NULL) {
        ALOGE("FatalErr on %s, mAudioPolicyManager unint ", __FUNCTION__);
        return NO_INIT;
    }
    // for VT notify tone when incoming call. it's volume will be adusted in hardware.
    if ((volsrc == mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_BLUETOOTH_SCO)) && outputDesc->isActive(volsrc) && mAudioPolicyManager->mEngine->getPhoneState() == AUDIO_MODE_IN_CALL && index != 0) {
        volumeDb = 0.0f;
    }
    if (outputDesc->setVolume(volumeDb, volsrc, mAudioPolicyManager->getVolumeCurves(volsrc).getStreamTypes(), device, delayMs, force)) {
        if (outputDesc == mAudioPolicyManager->mPrimaryOutput) {
            MTK_ALOGV("checkAndSetVolume volsrc = %d index = %d mId = %d device = %s(%s) delayMs = %d force = %d [%s/%s/%d]"
            , volsrc, index, outputDesc->getId(), toString(device).c_str(), toString(mDeviceOfCheckAndSetVolume).c_str(), delayMs, force, toString(mVolumeStream).c_str(), toString(mVolumeDevice).c_str(), mVolumeIndex);
        } else {
            MTK_ALOGV("checkAndSetVolume volsrc = %d index = %d mId = %d device = %s(%s) delayMs = %d force = %d [%s/%s/%d]"
            , volsrc,  index, outputDesc->getId(), toString(device).c_str(), toString(mDeviceOfCheckAndSetVolume).c_str(), delayMs, force, toString(mVolumeStream).c_str(), toString(mVolumeDevice).c_str(), mVolumeIndex);
        }
    }
    return NO_ERROR;
    } else {
        ALOGE("GainTable Feature option not available");
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::gainTable_routeAndApplyVolumeFromStopSource(const sp<SwAudioOutputDescriptor>& outputDesc, DeviceVector device,
                                           audio_stream_type_t stream, bool force)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        (void) stream;
        if (mAudioPolicyManager == NULL) {
            ALOGE("FatalErr on %s, mAudioPolicyManager unint ", __FUNCTION__);
            return NO_INIT;
        }
        DeviceVector prevDevice = outputDesc->mDevices;
        mAudioPolicyManager->setOutputDevices(outputDesc, device, force, outputDesc->latency()*2);
        //gain table need apply analog volume after stopOutput
        if (prevDevice == device) {
            mAudioPolicyManager->applyStreamVolumes(outputDesc, device.types(), outputDesc->latency()*2);
        }
        return NO_ERROR;
    } else {
        ALOGE("GainTable Feature option not available");
        return INVALID_OPERATION;
    }
}

bool AudioPolicyManagerCustomImpl::gainTable_skipAdjustGainFromSetStreamVolumeIndex(audio_devices_t curDevice, audio_devices_t wantDevice)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        if ((curDevice & wantDevice) == 0) {
            return true;
        }
    } else {
        ALOGE("GainTable Feature option not available");
    }
    return false;
}

audio_devices_t AudioPolicyManagerCustomImpl::gainTable_replaceApplyDeviceFromSetStreamVolumeIndex(audio_devices_t outputDevice, audio_devices_t curDevice)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        (void) curDevice;
        return outputDevice;
    } else {
        ALOGE("GainTable Feature option not available");
        return curDevice;
    }
}

void AudioPolicyManagerCustomImpl::gainTable_initXML()
{
    initXMLUpdate();
}
status_t AudioPolicyManagerCustomImpl::fm_initOutputIdForApp(void)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        mFMOutput = mAudioPolicyManager->mPrimaryOutput;
        return NO_ERROR;
    } else {
        return INVALID_OPERATION;
    }
}

DeviceVector AudioPolicyManagerCustomImpl::fm_correctDeviceFromSetDeviceConnectionStateInt(const sp<AudioOutputDescriptor>& outputDesc, DeviceVector device, bool force)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        // For FM Radio to detect BT/USB Connected
        if (outputDesc == mFMOutput && force) {
            device = outputDesc->supportedDevices().filter(device);
        }
    }
    return device;
}

status_t AudioPolicyManagerCustomImpl::common_setPolicyManagerCustomParameters(int par1, int par2, int par3, int par4)
{
#if defined(MTK_AUDIO)
    if (mAudioPolicyManager == NULL) {
        ALOGE("FatalErr on %s, mAudioPolicyManager unint ", __FUNCTION__);
        return NO_INIT;
    }
    audio_devices_t curDevice =Volume::getDeviceForVolume(mAudioPolicyManager->mPrimaryOutput->devices().types());
    MTK_ALOGS(MT_AUDIO_USERDEBUG_BUILD_LEVEL, "setPolicyManagerCustomParameters par1 = %d par2 = %d par3 = %d par4 = %d curDevice = 0x%x", par1, par2, par3, par4, curDevice);
    status_t volStatus;
    switch(par1) {
        case POLICY_SET_NUM_HS_POLE: {
            mAudioPolicyManager->mAudioPolicyVendorControl.setNumOfHeadsetPole(par2);
            break;
        }
        case POLICY_SET_FM_PRESTOP: {
            if (FeatureOption::MTK_FM_ENABLE) {
                for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
                    sp<AudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.valueAt(i);
                    if (desc->sharesHwModuleWith(mFMOutput) && !desc->isDuplicated()) {
                        if (par2) {
                            ALOGD("mute for FM app with Handle %d", mAudioPolicyManager->mOutputs.keyAt(i));
                            mAudioPolicyManager->setVolumeSourceMute(mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_MUSIC), true, desc);
                        } else {
                            ALOGD("unmute for FM app with Handle %d", mAudioPolicyManager->mOutputs.keyAt(i));
                            mAudioPolicyManager->setVolumeSourceMute(mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_MUSIC), false, desc);
                        }
                    }
                }
            }
            break;
        }
        case POLICY_SET_SCENE_GAIN:
        case POLICY_LOAD_VOLUME: {
            if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
                    ALOGD("%s(), reload GainTable param", __FUNCTION__);
                    if (par1 == POLICY_LOAD_VOLUME) {
                        if (allocateGainTable()) {
                        ALOGE("error, load GainTable failed!!");
                    }
                    } else {
                        updateCurrentSceneIndexOfGainTable();
                    }
            } else if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
                if (par1 == POLICY_SET_SCENE_GAIN) {
                    ALOGW("Don't support POLICY_SET_SCENE_GAIN on MTK_AUDIO_GAIN_NVRAM");
                    break;
                }
                loadCustomVolume();
            }
            for (size_t j = 0; j < mAudioPolicyManager->mOutputs.size(); j++) {
                sp<AudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.valueAt(j);
                for(int i = 0; i < AUDIO_STREAM_CNT; i++) {
                    if (i == AUDIO_STREAM_PATCH) {
                        continue;
                    } else if (desc->isActive(mAudioPolicyManager->toVolumeSource((audio_stream_type_t) i))) {
                         //ALOGD("update() mIoHandle %d mId %d device %04x size %zu stream %d", mAudioPolicyManager->mOutputs.keyAt(i), desc->getId(), desc->devices().types(), mAudioPolicyManager->mOutputs.size(), i);
                        auto &curves = mAudioPolicyManager->getVolumeCurves((audio_stream_type_t) i);
                        volStatus = mAudioPolicyManager->checkAndSetVolume(curves, mAudioPolicyManager->toVolumeSource((audio_stream_type_t)i), curves.getVolumeIndex(desc->devices().types()), desc, desc->devices().types(), 50, true);
                    }
                }
            }
            break;
        }
        case POLICY_SET_TTY_MODE: {
            if (FeatureOption::MTK_TTY_SUPPORT) {
                ALOGD("SetTtyMode = %d", par2);
                mTty_Ctm = (tty_mode_t) par2;
            }
            break;
        }
        case POLICY_SET_HIFI_STATE: {
        if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
                ALOGD("POLICY_SET_HIFI_STATE set hifi  state = %d", par2);
                setHIFIState(par2);
            } break;
        }
        case POLICY_SET_AUDIO_RATE:
            break;
        case POLICY_SET_BT_VGS_SETTING: {
            bool tmp = mAudioPolicyManager->mAudioPolicyVendorControl.getBTSupportVGS();
            mAudioPolicyManager->mAudioPolicyVendorControl.setBTSupportVGS(par2 ? true : false);
            ALOGD("mBTSupportVGS [%d] -> [%d]", tmp, mAudioPolicyManager->mAudioPolicyVendorControl.getBTSupportVGS());
            break;
        }
        default:
            break;
    }
    return NO_ERROR;
#else
    ALOGD("setPolicyManagerParameters (invalid) par1 = %d par2 = %d par3 = %d par4 = %d", par1, par2, par3, par4);
    return INVALID_OPERATION;
#endif
}


float AudioPolicyManagerCustomImpl::computeGainTableCustomVolume(audio_stream_type_t stream, int index, audio_devices_t device)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        float volume = 1.0;
        device_category deviceCategory = Volume::getDeviceCategory(device);

        if (mAudioPolicyManager->mAudioPolicyVendorControl.getVoiceReplaceDTMFStatus() && stream == AUDIO_STREAM_DTMF) {
            // normalize new index from 0~15(audio) to 0~7(voice)
            // int tempindex = index;
            auto &DTMFcurves = mAudioPolicyManager->getVolumeCurves(AUDIO_STREAM_DTMF);
            auto &VOIPcurves = mAudioPolicyManager->getVolumeCurves(AUDIO_STREAM_VOICE_CALL);
            float DTMFvolInt = (fCUSTOM_VOLUME_MAPPING_STEP * (index - DTMFcurves.getVolumeIndexMin())) / (DTMFcurves.getVolumeIndexMax() - DTMFcurves.getVolumeIndexMin());
            index = (DTMFvolInt * (VOIPcurves.getVolumeIndexMax() - VOIPcurves.getVolumeIndexMin())/ (fCUSTOM_VOLUME_MAPPING_STEP)) + VOIPcurves.getVolumeIndexMin();
            //MTK_ALOGI("volumecheck refine DTMF index [%d] to Voice index [%d]", tempindex, index);
            stream = AUDIO_STREAM_VOICE_CALL;
        }

        if (stream == AUDIO_STREAM_PATCH) {//ALPS04437227 android forbid  AUDIO_STREAM_PATCH use volume other than 1
            ALOGI("-computeGainTableCustomVolume volume %f stream %d, index %d, device 0x%x [%d/0x%x/%d]", volume, stream, index, device, mVolumeStream, mVolumeDevice, mVolumeIndex);
            return 1.0;
        }

        if (FeatureOption::MTK_TTY_SUPPORT) {
            if(mAudioPolicyManager->isInCall() == true && mTty_Ctm != AUD_TTY_OFF) {
                deviceCategory = Volume::getDeviceCategory(getNewDeviceForTty(device, mTty_Ctm));
                stream = AUDIO_STREAM_VOICE_CALL;
            }
        }
        GAIN_DEVICE gainDevice;
        if (deviceCategory == DEVICE_CATEGORY_SPEAKER) {
            gainDevice = GAIN_DEVICE_SPEAKER;
            if ((device & AUDIO_DEVICE_OUT_WIRED_HEADSET)||
                 (device & AUDIO_DEVICE_OUT_WIRED_HEADPHONE)) {
                gainDevice = GAIN_DEVICE_HSSPK;
            } else if (device & AUDIO_DEVICE_OUT_BUS) {
                gainDevice = GAIN_DEVICE_USB;
            } else if (device & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER) {
                gainDevice = GAIN_DEVICE_BT_A2DP_SPK;
            }
        } else if (deviceCategory == DEVICE_CATEGORY_HEADSET) {
            if (device & AUDIO_DEVICE_OUT_WIRED_HEADSET) {
                if (mAudioPolicyManager->mAudioPolicyVendorControl.getNumOfHeadsetPole() == 5)
                    gainDevice = GAIN_DEVICE_HEADSET_5POLE;
                else
                    gainDevice = GAIN_DEVICE_HEADSET;
            } else if (device & AUDIO_DEVICE_OUT_WIRED_HEADPHONE) {
                gainDevice = GAIN_DEVICE_HEADPHONE;
            } else if (device & AUDIO_DEVICE_OUT_USB_HEADSET) {
                gainDevice = GAIN_DEVICE_USB;
            } else if (device & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP) {
                gainDevice = GAIN_DEVICE_BT_A2DP;
            } else if (device & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES) {
                gainDevice = GAIN_DEVICE_BT_A2DP_HP;
            } else {
                gainDevice = GAIN_DEVICE_HEADSET;
            }
        } else if (deviceCategory == DEVICE_CATEGORY_EARPIECE) {
            gainDevice = GAIN_DEVICE_EARPIECE ;
        } else if (deviceCategory == DEVICE_CATEGORY_EXT_MEDIA) {
            if (device & AUDIO_DEVICE_OUT_USB_DEVICE)
                gainDevice = GAIN_DEVICE_USB;
            else
                gainDevice = GAIN_DEVICE_SPEAKER;
        } else {
            gainDevice = GAIN_DEVICE_SPEAKER;
        }

        ALOG_ASSERT(index >= 0 && index < GAIN_VOL_INDEX_SIZE, "invalid index");
        uint8_t customGain;
        if (mAudioPolicyManager->mEngine->getPhoneState() == AUDIO_MODE_IN_CALL && stream == AUDIO_STREAM_VOICE_CALL) {
            customGain = KeyvolumeStep - mGainTable.nonSceneGain.ringbackToneGain[gainDevice][index].digital;
        } else {
            customGain = KeyvolumeStep - mGainTable.sceneGain[mGainTableSceneIndex].streamGain[stream][gainDevice][index].digital;
        }
        volume = linearToLog(customGain);
        MTK_ALOGV("-computeGainTableCustomVolume customGain 0x%x, volume %f stream %d, index %d, device 0x%x [%d/0x%x/%d]", customGain, volume, stream, index, device, mVolumeStream, mVolumeDevice, mVolumeIndex);
        return volume;
    } else {
        ALOGW("%s unsupport, stream %d, index %d, device %d", __FUNCTION__, stream, index, device);
        return 0.0;
    }
}

audio_stream_type_t AudioPolicyManagerCustomImpl::selectGainTableActiveStream(audio_stream_type_t requestStream)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        audio_stream_type_t activeStream = requestStream;

        if (mAudioPolicyManager->isInCall()) {
            if (requestStream == AUDIO_STREAM_BLUETOOTH_SCO)
                activeStream = AUDIO_STREAM_BLUETOOTH_SCO;
            else if (requestStream == AUDIO_STREAM_VOICE_CALL)
                activeStream = AUDIO_STREAM_VOICE_CALL;
            else
                activeStream = AUDIO_STREAM_DEFAULT;
        }
        else if (mAudioPolicyManager->isStreamActive((audio_stream_type_t)AUDIO_STREAM_BLUETOOTH_SCO)) {
            activeStream = AUDIO_STREAM_BLUETOOTH_SCO;
        }
        else if (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_VOICE_CALL)) {
            activeStream = AUDIO_STREAM_VOICE_CALL;
        }
        else if (mAudioPolicyManager->isStreamActive((AUDIO_STREAM_RING))) {
            activeStream = AUDIO_STREAM_RING;
        }
        else if (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_ALARM)) {
            activeStream = AUDIO_STREAM_ALARM;
        }
        else if (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_ACCESSIBILITY)) {
            activeStream = AUDIO_STREAM_ACCESSIBILITY;
        }
        else if (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_NOTIFICATION)) {
            activeStream = AUDIO_STREAM_NOTIFICATION;
        }
        else if (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_ENFORCED_AUDIBLE)) {
            activeStream = AUDIO_STREAM_ENFORCED_AUDIBLE;
        }
        else if (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_MUSIC)) {
          activeStream = AUDIO_STREAM_MUSIC;
        }
        else if (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_TTS)) {
            activeStream = AUDIO_STREAM_TTS;
        }
        else if (mAudioPolicyManager->isStreamActive(AUDIO_STREAM_SYSTEM)) {
            activeStream = AUDIO_STREAM_SYSTEM;
        }
        else if (mAudioPolicyManager->isStreamActive((AUDIO_STREAM_DTMF))) {
            activeStream = AUDIO_STREAM_DTMF;
        }
        else {
            activeStream = AUDIO_STREAM_DEFAULT;
        }
        return activeStream;
    } else {
        ALOGW("%s unsupport, requestStream %d", __FUNCTION__, requestStream);
        return AUDIO_STREAM_DEFAULT;
    }
}

status_t AudioPolicyManagerCustomImpl::checkAndSetGainTableAnalogGain(audio_stream_type_t stream, int index, const sp<AudioOutputDescriptor>& outputDesc,audio_devices_t device,
                                                           int delayMs, bool force)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        audio_stream_type_t activeStream = selectGainTableActiveStream(stream);
        if (activeStream <= AUDIO_STREAM_DEFAULT) {
            return NO_ERROR;
        }
        // audio_devices_t curDevice  = getDeviceForVolume(device);

        bool anyShareHwModuleOutputStreamActive = false;
        for (size_t j = 0; j < mAudioPolicyManager->mOutputs.size(); j++) {
            sp<AudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.valueAt(j);
            if (outputDesc->sharesHwModuleWith(desc)) {
                if (desc->isActive(mAudioPolicyManager->toVolumeSource(stream))) {
                    anyShareHwModuleOutputStreamActive = true;
                    break;
                }
            }
        }

        if (!mAudioPolicyManager->isInCall() && !anyShareHwModuleOutputStreamActive) {
            return NO_ERROR;
        }

        if (activeStream != stream) {
            auto &curves = mAudioPolicyManager->getVolumeCurves(activeStream);
            index = curves.getVolumeIndex(device); // mStreams.valueFor((audio_stream_type_t)activeStream).getVolumeIndex(device);
        }

        if ((activeStream == AUDIO_STREAM_VOICE_CALL || activeStream == AUDIO_STREAM_BLUETOOTH_SCO) &&
            outputDesc != mAudioPolicyManager->mPrimaryOutput &&
            !(((sp<SwAudioOutputDescriptor>&)outputDesc)->mFlags & AUDIO_OUTPUT_FLAG_VOIP_RX)) {
            // in voice, set to primary only once, skip others
            return NO_ERROR;
        }

        if (outputDesc->getMuteCount(mAudioPolicyManager->toVolumeSource(activeStream)) != 0) {    //ALPS02455793. If music stream muted, don't pass music stream volume
            ALOGVV("checkAndSetGainTableAnalogGain() active %d stream %d muted count %d",
                  activeStream, stream, outputDesc->getMuteCount(mAudioPolicyManager->toVolumeSource(activeStream)));
            return NO_ERROR;
        }

        if (mVolumeStream != activeStream || mVolumeIndex != index || mVolumeDevice != device || force ) {
            MTK_ALOGS(MT_AUDIO_USERDEBUG_BUILD_LEVEL, "computeAndSetAnalogGain stream %d(%d), device 0x%x(0x%x), index %d(%d)", activeStream, mVolumeStream, device, mVolumeDevice, index, mVolumeIndex);
            mVolumeStream = activeStream;
            mVolumeIndex  = index ;
            mVolumeDevice = device;
            AudioParameter param = AudioParameter();
            param.addInt(String8("volumeStreamType"), activeStream);
            param.addInt(String8("volumeDevice"), device);
            param.addInt(String8("volumeIndex"), index);
            mAudioPolicyManager->mpClientInterface->setParameters(mAudioPolicyManager->mPrimaryOutput->mIoHandle, param.toString(),delayMs);
        }
        return NO_ERROR;
    } else {
        ALOGW("%s unsupport, stream %d, index %d, device %d, mId %d delayMs %d force %d", __FUNCTION__, stream, index, device, outputDesc->getId(), delayMs, force);
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::fm_addAudioPatch(audio_patch_handle_t handle, const sp<AudioPatch>& patch)
{
    bool bFMeable = false;
    sp<SwAudioOutputDescriptor> outputDesc = mFMOutput;
    if (FeatureOption::MTK_FM_ENABLE) {
        ssize_t index = mAudioPolicyManager->mAudioPatches.indexOfKey(handle);

        if (index >= 0) {
            ALOGW("addAudioPatch() patch %d already in", handle);
            return ALREADY_EXISTS;
        }
        if (isFMDirectMode(patch)) {
            if (outputDesc != NULL) {
                ALOGV("audiopatch Music+");
                // creat a client for FM direct Mode>>
                audio_config_base_t clientConfig = {.sample_rate = 48000,
                    .format = AUDIO_FORMAT_PCM,
                    .channel_mask = AUDIO_CHANNEL_OUT_STEREO };
                mFmPortId = AudioPort::getNextUniqueId();
                audio_attributes_t resultAttr = {.usage = AUDIO_USAGE_MEDIA,
                    .content_type = AUDIO_CONTENT_TYPE_MUSIC};

                sp<TrackClientDescriptor> FmClientDesc =
                    new TrackClientDescriptor(mFmPortId, -1, (audio_session_t) 0, resultAttr, clientConfig,
                                              AUDIO_PORT_HANDLE_NONE, AUDIO_STREAM_MUSIC,
                                              mAudioPolicyManager->mEngine->getProductStrategyForAttributes(resultAttr),
                                              mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_MUSIC),
                                              (audio_output_flags_t)AUDIO_FLAG_NONE, false,
                                              {});
                 if (FmClientDesc == NULL) {
                    ALOGD("FmClientDesc = NULL");
                    return INVALID_OPERATION;
                 }
                outputDesc->addClient(FmClientDesc);
                status_t status = outputDesc->start();
                if (status != NO_ERROR) {
                    return status;
                }
                outputDesc->setClientActive(FmClientDesc, true);
                mFmPortId = FmClientDesc->portId();
                MTK_ALOGV("FmClientDesc->portId() %d active %d volume source %d, stream %d, curActiveCount %d", FmClientDesc->portId(),
                        FmClientDesc->active(), FmClientDesc->volumeSource(), FmClientDesc->stream(), outputDesc->mProfile->curActiveCount);
                // creat a client for FM direct Mode>>
                bFMeable = true;
                mFMDirectAudioPatchEnable = true;
                DeviceVector currentDevice = mAudioPolicyManager->getNewOutputDevices(outputDesc, false /*fromCache*/);
                audio_devices_t patchDevice = patch->mPatch.sinks[0].ext.device.type;
                if (patch->mPatch.num_sinks == 2) { // force flag is from ALPS03443673
                    patchDevice = patchDevice | patch->mPatch.sinks[1].ext.device.type;
                }
                // It will auto correct the right routing device ALPS02988442. Alarm stop before 80002000->0x0a
                mAudioPolicyManager->setOutputDevices(outputDesc, currentDevice, (currentDevice.types() != patchDevice));
            }
        }
    }
    status_t status = mAudioPolicyManager->mAudioPatches.addAudioPatch(handle, patch);

    if (FeatureOption::MTK_FM_ENABLE) {
        if (bFMeable && status == NO_ERROR) {
            sp<TrackClientDescriptor> FmClientDesc = outputDesc->getClient(mFmPortId);
            if (FmClientDesc != NULL) {
                MTK_ALOGV("mFmPortId %d volumeSource %d", mFmPortId, FmClientDesc->volumeSource());
                // Change to 500 ms from 2 * Latency in order to covers FM dirty signal
                DeviceVector device = mAudioPolicyManager->getNewOutputDevices(outputDesc, false /*fromCache*/);
                auto &curves = mAudioPolicyManager->getVolumeCurves(FmClientDesc->volumeSource());
                mAudioPolicyManager->checkAndSetVolume(curves, FmClientDesc->volumeSource(),
                                  curves.getVolumeIndex(device.types()), outputDesc, device.types(), 500, true);
                mAudioPolicyManager->applyStreamVolumes(outputDesc, device.types(), 500, true);
            } else {
                ALOGW("no FM client, mFmPortId %d", mFmPortId);
            }
        }
    }
    return status;
}

status_t AudioPolicyManagerCustomImpl::fm_removeAudioPatch(audio_patch_handle_t handle)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        ssize_t index = mAudioPolicyManager->mAudioPatches.indexOfKey(handle);
        if (index < 0) {
            ALOGW("removeAudioPatch() patch %d not in", handle);
            return ALREADY_EXISTS;
        }
        MTK_ALOGV("removeAudioPatch() handle %d af handle %d", handle,
                          mAudioPolicyManager->mAudioPatches.valueAt(index)->mAfPatchHandle);
        sp<SwAudioOutputDescriptor> outputDesc = mFMOutput;
        const sp<AudioPatch> patch = mAudioPolicyManager->mAudioPatches.valueAt(index);
        sp<TrackClientDescriptor> client = outputDesc->getClient(mFmPortId);
        if (isFMDirectMode(patch)) {
            if (outputDesc != NULL) {
                if (client != NULL) {
                    ALOGV("audiopatch Music-");
                    // need to remove client here
                    // decrement usage count of this stream on the output
                    outputDesc->setClientActive(client, false);
                    outputDesc->removeClient(mFmPortId);
                    outputDesc->stop();
                    mFmPortId = 0;
                    mFMDirectAudioPatchEnable = false;
                    MTK_ALOGV("%s outputDesc->mProfile->curActiveCount %d", __FUNCTION__, outputDesc->mProfile->curActiveCount);
                    DeviceVector newDevice = mAudioPolicyManager->getNewOutputDevices(outputDesc, false /*fromCache*/);
                    mAudioPolicyManager->setOutputDevices(outputDesc, newDevice, false, outputDesc->latency()*2);
                }
            }
        }
    }
    return mAudioPolicyManager->mAudioPatches.removeAudioPatch(handle);
}

status_t AudioPolicyManagerCustomImpl::fm_applyGainFromCheckAndSetVolume(VolumeSource volumeSource, int index, const sp<AudioOutputDescriptor>& outputDesc,audio_devices_t device,
                                                           int delayMs, bool force)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        (void) force;
        if (!mSkipFMVolControl && volumeSource == mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_MUSIC)
                && outputDesc == mFMOutput && (device & (AUDIO_DEVICE_OUT_WIRED_HEADSET | AUDIO_DEVICE_OUT_WIRED_HEADPHONE | AUDIO_DEVICE_OUT_SPEAKER))) {
            for (ssize_t i = 0; i < (ssize_t)mAudioPolicyManager->mAudioPatches.size(); i++) {
                MTK_ALOGS(4, "%s size %zu/%zu", __FUNCTION__, i, mAudioPolicyManager->mAudioPatches.size());
                sp<AudioPatch> patchDesc = mAudioPolicyManager->mAudioPatches.valueAt(i);
                if (isFMDirectMode(patchDesc)) {
                    MTK_ALOGS(4, "%s, Do modify audiopatch volume",__FUNCTION__);
                    struct audio_port_config *config;
                    sp<AudioPortConfig> audioPortConfig;
                    sp<DeviceDescriptor>  deviceDesc;
                    config = &(patchDesc->mPatch.sinks[0]);
                    bool bOrignalDeviceRemoved = false;
                    if (config->role == AUDIO_PORT_ROLE_SINK) {
                        deviceDesc = mAudioPolicyManager->mAvailableOutputDevices.getDeviceFromId(config->id);
                    } else {
                        ALOGV("1st deviceDesc NULL");
                        break;
                    }
                    if (deviceDesc == NULL) {
                        bOrignalDeviceRemoved = true;// Headset is removed
                        ALOGV("bOrignalDeviceRemoved Device %x replace %x", device, config->ext.device.type);
                        deviceDesc = mAudioPolicyManager->mAvailableOutputDevices.getDevice(device, String8(""), AUDIO_FORMAT_DEFAULT);
                        if (deviceDesc == NULL) {
                            ALOGV("2nd deviceDesc NULL");
                            break;
                        }
                    }
                    audioPortConfig = deviceDesc;
                    struct audio_port_config newConfig;
                    audioPortConfig->toAudioPortConfig(&newConfig, config);
                    if (bOrignalDeviceRemoved == true)
                        newConfig.ext.device.type = config->ext.device.type;
                    newConfig.config_mask = AUDIO_PORT_CONFIG_GAIN | newConfig.config_mask;
                    newConfig.gain.mode = AUDIO_GAIN_MODE_JOINT | newConfig.gain.mode;
                    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
                        newConfig.gain.values[0] = -300 * (getStreamMaxLevels(volumeSource) - index);
                    } else {
                        newConfig.gain.values[0] = index;   // pass volume index directly
                    }
                if ((device != newConfig.ext.device.type || bOrignalDeviceRemoved) && index != 0)// For switch and pop between hp and speaker
                        newConfig.ext.device.type = device; // Device change, Don't un-mute, wait next createAudioPatch
                    mAudioPolicyManager->mpClientInterface->setAudioPortConfig(&newConfig, delayMs);
                }
            }
        }
        return NO_ERROR;
    } else {
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::fm_muteStrategyFromCheckOutputForStrategy(const audio_attributes_t &attr, DeviceVector oldDevice, DeviceVector newDevice)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        if (mAudioPolicyManager->followsSameRouting(attr, attributes_initializer(AUDIO_USAGE_MEDIA))) {
            DeviceVector oriOldDevice = oldDevice;
            DeviceVector oriNewDevice = newDevice;
            oldDevice.remove(oldDevice.getDevicesFromTypeMask(AUDIO_DEVICE_OUT_SPEAKER|AUDIO_DEVICE_OUT_WIRED_HEADSET|AUDIO_DEVICE_OUT_WIRED_HEADPHONE));
            newDevice.remove(newDevice.getDevicesFromTypeMask(AUDIO_DEVICE_OUT_SPEAKER|AUDIO_DEVICE_OUT_WIRED_HEADSET|AUDIO_DEVICE_OUT_WIRED_HEADPHONE));
            // ALPS03221274, when playback FM, route to Headset from BT. there is a missing sound of track from BT track Headset.
            // And then it will mute and the unmute when entering direct mode
            // If input device is disconnected first, FM active information will disappear, so remove FMActive
            if (oldDevice.isEmpty() && newDevice.isEmpty()) {
                MTK_ALOGS(MT_AUDIO_USERDEBUG_BUILD_LEVEL, "FM mute in-direct primary first, oldDevice 0x%x -> newDevice 0x%x", oriOldDevice.types(), oriNewDevice.types());
                product_strategy_t strategy = mAudioPolicyManager->mEngine->getProductStrategyForAttributes(attr);
                mAudioPolicyManager->setStrategyMute(strategy, true, mFMOutput);
                mAudioPolicyManager->setStrategyMute(strategy, false, mFMOutput, MUTE_TIME_MS, newDevice.types());
            }
        }
        return NO_ERROR;
    } else {
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::fm_checkSkipVolumeFromCheckOutputForStrategy(const audio_attributes_t &attr, DeviceVector oldDevice, DeviceVector newDevice)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        bool FMcaseBetweenSPKHP = false;
        oldDevice.remove(oldDevice.getDevicesFromTypeMask(AUDIO_DEVICE_OUT_SPEAKER|AUDIO_DEVICE_OUT_WIRED_HEADSET|AUDIO_DEVICE_OUT_WIRED_HEADPHONE));
        newDevice.remove(newDevice.getDevicesFromTypeMask(AUDIO_DEVICE_OUT_SPEAKER|AUDIO_DEVICE_OUT_WIRED_HEADSET|AUDIO_DEVICE_OUT_WIRED_HEADPHONE));

        if (mAudioPolicyManager->followsSameRouting(attr, attributes_initializer(AUDIO_USAGE_MEDIA)) && isFMActive()) {
            if (oldDevice.isEmpty() && newDevice.isEmpty()) {
                    FMcaseBetweenSPKHP = true;
                }
        }
        mSkipFMVolControl = FMcaseBetweenSPKHP;
        return NO_ERROR;
    } else {
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::fm_releaseSkipVolumeFromCheckOutputForStrategy(void)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        mSkipFMVolControl = false;
        return NO_ERROR;
    } else {
        return INVALID_OPERATION;
    }
}

bool AudioPolicyManagerCustomImpl::fm_checkFirstMusicFromStartSource(const sp<AudioOutputDescriptor>& outputDesc, audio_stream_type_t stream)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        if (mFMDirectAudioPatchEnable && stream == AUDIO_STREAM_MUSIC &&
            outputDesc->getActivityCount(mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_MUSIC)) == 2) {
            return true;
        }
    }
    return false;
}

uint32_t AudioPolicyManagerCustomImpl::fm_extendMuteFromCheckDeviceMuteStrategies(const sp<AudioOutputDescriptor>& outputDesc, VolumeSource volumeSource, uint32_t muteDurationMs, uint32_t extendDurationMs)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        //get first attribute to check if it is used for MEDIA
        (void) outputDesc;
        if (volumeSource == mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_MUSIC)
                && isFMActive() && muteDurationMs < extendDurationMs) {
            return extendDurationMs;
        } else {
            return muteDurationMs;
        }
    } else {
        return muteDurationMs;
    }
}

status_t AudioPolicyManagerCustomImpl::fm_signalAPProutingFromSetOutputDevice(const sp<SwAudioOutputDescriptor>& outputDesc, bool force)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        if (force && outputDesc == mFMOutput && isFMActive()) {
            mAudioPolicyManager->nextAudioPortGeneration();
            mAudioPolicyManager->mpClientInterface->onAudioPatchListUpdate();
            return NO_ERROR;
        } else {
            return INVALID_OPERATION;
        }
    } else {
        return INVALID_OPERATION;
    }
}

uint32_t AudioPolicyManagerCustomImpl::fm_extendSleepFromCheckDeviceMuteStrategies(const sp<AudioOutputDescriptor>& outputDesc, uint32_t muteWaitMs)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        // 430 is the experimental value
        #define WAIT_HW_GAIN_MUTE_TIME (430)
        if (outputDesc == mFMOutput && isFMDirectActive()) {
            if (muteWaitMs < WAIT_HW_GAIN_MUTE_TIME) {
                usleep((WAIT_HW_GAIN_MUTE_TIME - muteWaitMs) * 1000);
                return WAIT_HW_GAIN_MUTE_TIME;
            } else {
                return muteWaitMs;
            }
        } else {
            return muteWaitMs;
        }
    } else {
        return muteWaitMs;
    }
}

status_t AudioPolicyManagerCustomImpl::usbPhoneCall_connectFromSetDeviceConnectionState(audio_devices_t device,
                                                                                      audio_policy_dev_state_t state,
                                                                                      const char *device_address,
                                                                                      const char *device_name,
                                                                                      audio_format_t encodedFormat)
{
    if (FeatureOption::MTK_USB_PHONECALL) {
        status_t status;
        audio_devices_t autodevice = AUDIO_DEVICE_NONE;
        if (device == AUDIO_DEVICE_OUT_USB_DEVICE || device == AUDIO_DEVICE_OUT_USB_HEADSET) {
            autodevice = AUDIO_DEVICE_OUT_BUS;
        } else if (device == AUDIO_DEVICE_IN_USB_DEVICE || device == AUDIO_DEVICE_IN_USB_HEADSET) {
            autodevice = AUDIO_DEVICE_IN_BUS;
        }
        status = mAudioPolicyManager->setDeviceConnectionStateInt(device, state, device_address, device_name, encodedFormat);
        if (autodevice != AUDIO_DEVICE_NONE && status == NO_ERROR) {
            if (device == AUDIO_DEVICE_OUT_USB_HEADSET) {
                if (state == AUDIO_POLICY_DEVICE_STATE_AVAILABLE) {
                    mUsbHeadsetConnect = true;
                } else if (state == AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE) {
                    mUsbHeadsetConnect = false;
                }
            }
            ALOGV("Auto trigger device: 0x%x connect/disconnect", autodevice);
            mAudioPolicyManager->setDeviceConnectionStateInt(autodevice, state, String8("usb_phone_call"), "primary-usb bus", encodedFormat);
        }
        return status;
    } else {
        return mAudioPolicyManager->setDeviceConnectionStateInt(device, state, device_address, device_name, encodedFormat);
    }
}

status_t AudioPolicyManagerCustomImpl::usbPhoneCall_setOutputDeviceFromUpdateCallRouting(const sp<SwAudioOutputDescriptor>& outputDesc,
                                             DeviceVector rxDevice,
                                             bool force,
                                             int delayMs,
                                             sp<DeviceDescriptor> &txDevice,
                                             uint32_t *muteWaitMs)
{
    if (FeatureOption::MTK_USB_PHONECALL) {
        (void) force;
        bool usbOutputChanged = false;
        if (rxDevice.types() == AUDIO_DEVICE_OUT_BUS) {
            usbOutputChanged = checkUsbSuspend(rxDevice.types());
            audio_attributes_t attr = { .source = AUDIO_SOURCE_VOICE_COMMUNICATION };
             sp<DeviceDescriptor>  txSourceDevice = mAudioPolicyManager->mEngine->getInputDeviceForAttributes(attr);
             txDevice = txSourceDevice;
            if (usbOutputChanged && txDevice->type() == AUDIO_DEVICE_IN_BUS) {
                // closeUsbInputs is better
                mAudioPolicyManager->checkCloseInputs();
            }
        }
        *muteWaitMs = mAudioPolicyManager->setOutputDevices(outputDesc, rxDevice, true, delayMs);
        if (rxDevice.types() != AUDIO_DEVICE_OUT_BUS) {
            usbOutputChanged = checkUsbSuspend();
            audio_attributes_t attr = { .source = AUDIO_SOURCE_VOICE_COMMUNICATION };
            sp<DeviceDescriptor> txSourceDevice = mAudioPolicyManager->mEngine->getInputDeviceForAttributes(attr);
            txDevice = txSourceDevice;
            if (usbOutputChanged && txDevice->type() != AUDIO_DEVICE_IN_BUS) {
                // closePrimaryUsbInputs is better
                mAudioPolicyManager->checkCloseInputs();
            }
        }
        return NO_ERROR;
    } else {
        (void) outputDesc;
        (void) rxDevice;
        (void) force;
        (void) delayMs;
        (void) txDevice;
        (void) muteWaitMs;
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::usbPhoneCall_setCurModeFromSetPhoneState(audio_mode_t state)
{
    if (FeatureOption::MTK_USB_PHONECALL) {
        mAudioPolicyManager->mAudioPolicyVendorControl.setCurMode(state);
        return NO_ERROR;
    } else {
        (void) state;
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::usbPhoneCall_closeAllInputsFromSetPhoneState(void)
{
    if (FeatureOption::MTK_USB_PHONECALL) {
        if (checkUsbSuspend()) {
            // closePrimaryUsbInputs is better
            mAudioPolicyManager->checkCloseInputs();
        }
        return NO_ERROR;
    } else {
        return INVALID_OPERATION;
    }
}

audio_devices_t AudioPolicyManagerCustomImpl::usbPhoneCall_addCurrentVolumeIndexFromSetStreamVolumeIndex(audio_stream_type_t stream,
                                                  int index,
                                                  audio_devices_t device)
{
    if (FeatureOption::MTK_USB_PHONECALL) {
        if ((device == AUDIO_DEVICE_OUT_USB_DEVICE && !mUsbHeadsetConnect) ||
            (device == AUDIO_DEVICE_OUT_USB_HEADSET && mUsbHeadsetConnect)) {
            // USB_PRIMARY is an alias of USB_DEVICE for purposes of volume index, however it uses SPK mapping table for digital gain
            device |= AUDIO_DEVICE_OUT_BUS;
            for (int curStream = 0; curStream < AUDIO_STREAM_FOR_POLICY_CNT; curStream++) {
                if (!mAudioPolicyManager->streamsMatchForvolume(stream, (audio_stream_type_t)curStream)) {
                    continue;
                }
                auto &curves = mAudioPolicyManager->getVolumeCurves((audio_stream_type_t)curStream);
                curves.addCurrentVolumeIndex(AUDIO_DEVICE_OUT_BUS, index);
            }
        }
        return device;
    } else {
        (void) stream;
        (void) index;
        return device;
    }
}

DeviceVector AudioPolicyManagerCustomImpl::usbPhoneCall_correctDeviceFromGetDevicesForStream(DeviceVector devices)
{
    if (FeatureOption::MTK_USB_PHONECALL) {
        /*Filter USB_PRIMARY out of results, as AudioService doesn't know about it
          and doesn't really need to.*/
        if (devices.types() & AUDIO_DEVICE_OUT_BUS) {
            if (!mUsbHeadsetConnect) {
                devices.merge(mAudioPolicyManager->getAvailableOutputDevices().getDevicesFromTypeMask(AUDIO_DEVICE_OUT_USB_DEVICE));
            } else {
                devices.merge(mAudioPolicyManager->getAvailableOutputDevices().getDevicesFromTypeMask(AUDIO_DEVICE_OUT_USB_HEADSET));
             }
            devices.merge(mAudioPolicyManager->getAvailableOutputDevices().getDevicesFromTypeMask(AUDIO_DEVICE_OUT_BUS));
         }
        return devices;
    } else {
        return devices;
    }
}

bool AudioPolicyManagerCustomImpl::usbPhoneCall_isSupportUSBPhoneCallDevice(const String8& address, sp<DeviceDescriptor> device)
{
    if (FeatureOption::MTK_USB_PHONECALL) {
        return (device_distinguishes_on_address(device->type()) && address == String8("usb_phone_call"));
    } else {
        (void) address;
        (void) device;
        return false;
    }
}

status_t AudioPolicyManagerCustomImpl::gainNvram_remapIndexRangeFromInitStreamVolume(audio_stream_type_t stream,
                                            int *indexMin,
                                            int *indexMax)
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        if (stream == AUDIO_STREAM_VOICE_CALL) {
            if (*indexMin > 0) {
                mNeedRemapVoiceVolumeIndex = true;
                *indexMin = *indexMin - 1;
                *indexMax = *indexMax - 1;
                ALOGV("Correct stream %d, min %d, max %d", stream , *indexMin, *indexMax);
            }
        }
        return NO_ERROR;
    } else {
        (void) stream;
        (void) indexMin;
        (void) indexMax;
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::gainNvram_remapIndexFromSetStreamVolumeIndex(audio_stream_type_t stream,
                                                  int *index,
                                                  audio_devices_t device)
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {   //  Remapping M min index = 1 to MTK min index = 0
        (void) device;
        if (stream == AUDIO_STREAM_VOICE_CALL) {
            if (mNeedRemapVoiceVolumeIndex == true) {
                *index = *index - 1;
            }
        }
        return NO_ERROR;
    } else {
        (void) stream;
        (void) index;
        (void) device;
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::gainNvram_remapIndexFromGetStreamVolumeIndex(audio_stream_type_t stream,
                                                      int *index,
                                                      audio_devices_t device)
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {   //  Remapping M min index = 1 to MTK min index = 0
        (void) device;
        if (stream == AUDIO_STREAM_VOICE_CALL) {
            if (mNeedRemapVoiceVolumeIndex == true) {
                *index = *index + 1;
                ALOGV("Correct stream %d device %08x index %d", stream, device, *index);
            }
        }
        return NO_ERROR;
    } else {
        (void) stream;
        (void) index;
        (void) device;
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::lowLatency_updatePrimaryModuleDeviceFromSetPhoneState(audio_mode_t state, audio_devices_t rxDevice)
{
#if defined(MTK_LOW_LATENCY)
    if (state == AUDIO_MODE_NORMAL) {
        updatePrimaryModuleDevice(rxDevice);
    }
    return NO_ERROR;
#else
    (void) state;
    (void) rxDevice;
    return INVALID_OPERATION;
#endif
}

status_t AudioPolicyManagerCustomImpl::lowLatency_CheckSpeakerProtectionDevice(const sp<IOProfile>& outProfile)
{
#if defined(MTK_LOW_LATENCY)
    if ((AUDIO_OUTPUT_FLAG_MMAP_NOIRQ & outProfile->getFlags())) {
        //sp<DeviceDescriptor> devDesc = outProfile->getSupportedDeviceByAddress(AUDIO_DEVICE_OUT_SPEAKER, String8(""));
        sp<DeviceDescriptor> devDesc = outProfile->getSupportedDevices().getDevice(AUDIO_DEVICE_OUT_SPEAKER, String8(""), AUDIO_FORMAT_DEFAULT);
        if (devDesc != NULL) {
            String8 command = mAudioPolicyManager->mpClientInterface->getParameters(0, String8("GetSpeakerProtection"));
            AudioParameter param = AudioParameter(command);
            int valueInt;
            if (param.getInt(String8("GetSpeakerProtection"), valueInt) == NO_ERROR &&
            valueInt == 1) {
                outProfile->removeSupportedDevice(devDesc);
                if (outProfile->supportsDevice(devDesc) == false) {
                    ALOGD("Remove SPK From the output(Flag = 0x%x)", outProfile->getFlags());
                } else {
                    ALOGW("Remove SPK Fail from the output(Flag = 0x%x)", outProfile->getFlags());
                }
            } else {
                ALOGD("Not support GetSpeakerProtection");
            }
        }
    }
    return NO_ERROR;
#else
    (void) outProfile;
    return INVALID_OPERATION;
#endif
}

bool AudioPolicyManagerCustomImpl::lowLatency_skipSelectedDeviceFormSetOutputDevice(const sp<AudioOutputDescriptor>& outputDesc, audio_devices_t device)
{
#if defined(MTK_LOW_LATENCY)  //  For fast mixer doesn't support Speaker
    if ((device == (AUDIO_DEVICE_OUT_SPEAKER|AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
        device == (AUDIO_DEVICE_OUT_SPEAKER|AUDIO_DEVICE_OUT_WIRED_HEADPHONE)) &&
            ((device & outputDesc->supportedDevices().types()) != device)) {
        ALOGV("The Output Support 0x%x, but not support 0x%x", outputDesc->supportedDevices().types(), device);
        return true;
    }
    return false;
#else
    (void) outputDesc;
    (void) device;
    return false;
#endif
}

bool AudioPolicyManagerCustomImpl::lowLatency_skipOutputCheckFromGetOutputsForDevice(audio_devices_t device, const SwAudioOutputCollection& openOutputs, size_t i)
{
#if defined(MTK_LOW_LATENCY)
    // fast output don't support VOIP ALPS03398659/ALPS02401994
    if (AUDIO_MODE_IN_COMMUNICATION == mAudioPolicyManager->mEngine->getPhoneState()) {
        if((openOutputs.valueAt(i)->mProfile != 0) &&
           (openOutputs.valueAt(i)->mProfile->getFlags() & AUDIO_OUTPUT_FLAG_FAST) &&
           !(openOutputs.valueAt(i)->mProfile->getFlags() & AUDIO_OUTPUT_FLAG_PRIMARY)) {
           ALOGV("fast output don't support VOIP, outout flags 0x%x,phone state 0x%x, device 0x%x",
                openOutputs.valueAt(i)->mProfile->getFlags(), mAudioPolicyManager->mEngine->getPhoneState(), device);
            return true;
        }
    }
    return false;
#else
    (void) i;
    (void) device;
    (void) openOutputs;
    return false;
#endif
}

bool AudioPolicyManagerCustomImpl::lowLatency_stopToRouteFromStopSource(audio_devices_t newDevice, const sp<AudioOutputDescriptor>& outputDesc)
{
#if defined(MTK_LOW_LATENCY)  //  ALPS02626190, should reroute by active output
    return (newDevice != outputDesc->devices().types());
#else
    (void) newDevice;
    (void) outputDesc;
    return false;
#endif
}

bool AudioPolicyManagerCustomImpl::lowLatency_startToRouteFromStartSource(const sp<AudioOutputDescriptor>& outputDesc, bool beFirstActive)
{
#if defined(MTK_LOW_LATENCY)    // Skip that Primary output is stopped on different device
    return (outputDesc->isActive() || beFirstActive);
#else
    (void) outputDesc;
    (void) beFirstActive;
    return false;
#endif
}

bool AudioPolicyManagerCustomImpl::lowLatency_isOutputActiveFromStartSource(const sp<AudioOutputDescriptor>& outputDesc)
{
#if defined(MTK_LOW_LATENCY)
    return !outputDesc->isActive();
#else
    (void) outputDesc;
    return false;
#endif
}

bool AudioPolicyManagerCustomImpl::lowLatency_setOutputFirstActiveFromStartSource(const sp<SwAudioOutputDescriptor>& outputDesc, bool beFirstActive, DeviceVector device)
{
#if defined(MTK_LOW_LATENCY)
    if (beFirstActive) {
        outputDesc->setOutputFirstActive(true);
        for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
            sp<SwAudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.valueAt(i);
            if (desc != outputDesc && outputDesc->sharesHwModuleWith(desc)) {
                if (desc->isActive() && desc->devices() != device) {
                    outputDesc->setOutputFirstActive(false);
                    return false;
                }
            }
        }
        return true;
    }
    return false;
#else
    (void) outputDesc;
    (void) beFirstActive;
    (void) device;
    return false;
#endif
}

bool AudioPolicyManagerCustomImpl::lowLatency_checkOutputFirstActiveFromSetOutputDevice(const sp<AudioOutputDescriptor>& outputDesc)
{
#if defined(MTK_LOW_LATENCY)
    if (outputDesc->mOutputFirstActive == true && (outputDesc != mAudioPolicyManager->mPrimaryOutput || !mAudioPolicyManager->isInCall())) {
        outputDesc->setOutputFirstActive(false);
        return true;
    }
    return false;
#else
    (void) outputDesc;
    return false;
#endif
}

status_t AudioPolicyManagerCustomImpl::besLoudness_signalDupOutputFromSetOutputDevice(const sp<SwAudioOutputDescriptor>& outputDesc,
                                             const DeviceVector &device,
                                             int delayMs)
{
    if (FeatureOption::MTK_BESLOUDNESS_ENABLE) {
        AudioParameter param;
        param.addInt(String8("AudioFlinger_routing"), (int)device.types());
        for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
            sp<SwAudioOutputDescriptor> outputdesc = mAudioPolicyManager->mOutputs.valueAt(i);
            if (outputDesc == outputdesc) {
                mAudioPolicyManager->mpClientInterface->setParameters(outputdesc->mIoHandle, param.toString(), delayMs);
                break;
            }
        }
        return NO_ERROR;
    } else {
        (void) outputDesc;
        (void) device;
        (void) delayMs;
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::offload_invalidateMusicForInCallFromSetPhoneState(audio_mode_t state, int oldState)
{
    if (state == AUDIO_MODE_IN_CALL && oldState != AUDIO_MODE_IN_CALL) {
        for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
            sp<SwAudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.valueAt(i);
            if (desc->isActive(mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_MUSIC)) && (desc->mFlags & AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD)) {
                mAudioPolicyManager->mpClientInterface->invalidateStream(AUDIO_STREAM_MUSIC);
                ALOGD("Invalidate Offload Music because of unsupport offload in Call");
                break;
            }
        }
    }
    return NO_ERROR;
}

bool AudioPolicyManagerCustomImpl::offload_isInCallFromIsOffloadSupported(void)
{
    if (mAudioPolicyManager->mEngine->getPhoneState() == AUDIO_MODE_IN_CALL) {
        return true; // MTK Platforms don't support offload in phone call state
    } else {
        return false;
    }
}

bool AudioPolicyManagerCustomImpl::debug_skipShowLog()
{
#if defined(MTK_AUDIO_DEBUG)
    return true;
#else
    return false;
#endif
}

bool AudioPolicyManagerCustomImpl::debug_showGetOutputForAttr(audio_devices_t device, const audio_config_t *config, audio_output_flags_t flags, audio_stream_type_t stream, audio_io_handle_t output)
{
#if defined(MTK_AUDIO_DEBUG)
    MTK_ALOGS(MT_AUDIO_USERDEBUG_BUILD_LEVEL, "getOutputForAttr() device 0x%x, sample_rate %d, format %x, channel_mask %x, flags %x stream %d, output %d",
          device, config->sample_rate, config->format, config->channel_mask, flags, stream, output);
    return true;
#else
    (void) device;
    (void) config;
    (void) flags;
    (void) stream;
    (void) output;
    return false;
#endif
}

bool AudioPolicyManagerCustomImpl::debug_showGetInputForAttr(AudioPolicyInterface::input_type_t inputType, audio_devices_t device, audio_io_handle_t input)
{
#if defined(MTK_AUDIO_DEBUG)
    MTK_ALOGS(MT_AUDIO_USERDEBUG_BUILD_LEVEL, "getInputForAttr() returns input type = %d device =0x%x *input = %d", inputType, device, input);
    return true;
#else
    (void) inputType;
    (void) device;
    (void) input;
    return false;
#endif
}

bool AudioPolicyManagerCustomImpl::debug_showSetOutputDevice(const sp<SwAudioOutputDescriptor>& outputDesc, const DeviceVector & devices, bool force, int delayMs)
{
#if defined(MTK_AUDIO_DEBUG)
    for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
        sp<AudioOutputDescriptor> outputdesc = mAudioPolicyManager->mOutputs.valueAt(i);
        if (outputDesc == outputdesc) {
            if (devices.isEmpty() && !force) {
                ALOGVV("setOutputDevices() mIoHandle %d mId %d device %04x(%04x)(%04x) delayMs %d force %d size %zu", mAudioPolicyManager->mOutputs.keyAt(i), outputDesc->getId(), devices.types(), outputDesc->mDevices.types(), outputDesc->supportedDevices().types(), delayMs, force, mAudioPolicyManager->mOutputs.size());
            } else {
                MTK_ALOGS(MT_AUDIO_USERDEBUG_BUILD_LEVEL, "setOutputDevices() mIoHandle %d mId %d device %04x(%04x)(%04x) delayMs %d force %d size %zu", mAudioPolicyManager->mOutputs.keyAt(i), outputDesc->getId(), devices.types(), outputDesc->mDevices.types(), outputDesc->supportedDevices().types(), delayMs, force, mAudioPolicyManager->mOutputs.size());
            }
            break;
        }
        if (i == mAudioPolicyManager->mOutputs.size())
            ALOGV("setOutputDevices() device %s delayMs %d force %d outputsize %zu", devices.toString().c_str(), delayMs, force, mAudioPolicyManager->mOutputs.size());
    }
    return true;
#else
    (void) outputDesc;
    (void) devices;
    (void) force;
    (void) delayMs;
    return false;
#endif
}

float AudioPolicyManagerCustomImpl::linearToLog(int volume)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
    return volume ? exp(float(KeyvolumeStep - volume) * KeydBConvert) : 0;
    } else {
    ALOGW("%s unsupport, volume %d", __FUNCTION__, volume);
    return 0.0;
    }
}

int AudioPolicyManagerCustomImpl::logToLinear(float volume)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        return volume ? KeyvolumeStep - int(KeydBConvertInverse * log(volume) + 0.5) : 0;
    } else {
        ALOGW("%s unsupport, volume %f", __FUNCTION__, volume);
        return 0;
    }
}

audio_devices_t AudioPolicyManagerCustomImpl::getNewDeviceForTty(audio_devices_t device, tty_mode_t tty_mode)
{
    if (FeatureOption::MTK_TTY_SUPPORT) {
        audio_devices_t OutputDeviceForTty = AUDIO_DEVICE_NONE;

        if (device & AUDIO_DEVICE_OUT_SPEAKER) {
            if (tty_mode == AUD_TTY_VCO) {
                ALOGV("%s(), speaker, TTY_VCO", __FUNCTION__);
#if defined(ENABLE_EXT_DAC)
                OutputDeviceForTty = AUDIO_DEVICE_OUT_EARPIECE;
#else
                OutputDeviceForTty = AUDIO_DEVICE_OUT_WIRED_HEADSET;
#endif
            } else if (tty_mode == AUD_TTY_HCO) {
                ALOGV("%s(), speaker, TTY_HCO", __FUNCTION__);
#if defined(ENABLE_EXT_DAC)
                OutputDeviceForTty = AUDIO_DEVICE_OUT_EARPIECE;
#else
                OutputDeviceForTty = AUDIO_DEVICE_OUT_SPEAKER;
#endif
            } else if (tty_mode == AUD_TTY_FULL) {
                ALOGV("%s(), speaker, TTY_FULL", __FUNCTION__);
#if defined(ENABLE_EXT_DAC)
                OutputDeviceForTty = AUDIO_DEVICE_OUT_EARPIECE;
#else
                OutputDeviceForTty = AUDIO_DEVICE_OUT_WIRED_HEADSET;
#endif
            }
        } else if ((device == AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
                 (device == AUDIO_DEVICE_OUT_WIRED_HEADPHONE)) {
            if (tty_mode == AUD_TTY_VCO) {
                ALOGV("%s(), headset, TTY_VCO", __FUNCTION__);
#if defined(ENABLE_EXT_DAC)
                OutputDeviceForTty = AUDIO_DEVICE_OUT_EARPIECE;
#else
                OutputDeviceForTty = AUDIO_DEVICE_OUT_WIRED_HEADSET;
#endif
            } else if (tty_mode == AUD_TTY_HCO) {
                ALOGV("%s(), headset, TTY_HCO", __FUNCTION__);
                OutputDeviceForTty = AUDIO_DEVICE_OUT_EARPIECE;
            } else if (tty_mode == AUD_TTY_FULL) {
                ALOGV("%s(), headset, TTY_FULL", __FUNCTION__);
#if defined(ENABLE_EXT_DAC)
                OutputDeviceForTty = AUDIO_DEVICE_OUT_EARPIECE;
#else
                OutputDeviceForTty = AUDIO_DEVICE_OUT_WIRED_HEADSET;
#endif
            }
        }
        ALOGV("getNewDeviceForTty() tty_mode=%d, device=0x%x, OutputDeviceForTty=0x%x", tty_mode, device, OutputDeviceForTty);
        return OutputDeviceForTty;
    } else {
        ALOGW("%s unsupport device 0x%x tty_mode %d", __FUNCTION__, device, tty_mode);
        return device;
    }
}

bool AudioPolicyManagerCustomImpl::isFMDirectMode(const sp<AudioPatch>& patch)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        if (patch->mPatch.sources[0].type == AUDIO_PORT_TYPE_DEVICE &&
            patch->mPatch.sinks[0].type == AUDIO_PORT_TYPE_DEVICE &&
            (patch->mPatch.sources[0].ext.device.type == AUDIO_DEVICE_IN_FM_TUNER)) {
            return true;
        } else {
            return false;
        }
    } else {
        (void) patch;
        ALOGW("%s unsupport", __FUNCTION__);
        return false;
    }
}

int AudioPolicyManagerCustomImpl::getHIFIState(void)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        String8 command = mAudioPolicyManager->mpClientInterface->getParameters(0, String8("hifi_state"));
        AudioParameter param = AudioParameter(command);
        int valueInt;
        if (param.getInt(String8("hifi_state"), valueInt) == NO_ERROR) {
            MTK_ALOGV("%s(), get hifi_state:%d", __FUNCTION__, valueInt);
        } else {
            MTK_ALOGW("%s(), get hifi_state fail", __FUNCTION__);
        }
        return valueInt;
    } else {
        return 0;
    }
}

bool AudioPolicyManagerCustomImpl::setHIFIState(int enable)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        MTK_ALOGD("%s() enable = %d, oldHiFiState = %d", __FUNCTION__, enable, mHifiState);

            if (enable != mHifiState) {
                mSampleRateForce = true;
                MTK_ALOGV("%s() mSampleRateForce true ", __FUNCTION__);
            } else {
                MTK_ALOGV("%s() enable == oldHiFiState", __FUNCTION__);
                return false;
        }

        AudioParameter param;
        int samplerate = getMaxTrackSampleRate();

        if (enable) {
            param.addInt(String8("hifi_state"), 1);
            mAudioPolicyManager->mpClientInterface->setParameters(AUDIO_IO_HANDLE_NONE, param.toString(), 0);
            mHifiState = 1;

            mSampleRate_Policy = SampleRate_ForceReplace;
            policyForceReplace(AUDIO_STREAM_MUSIC, samplerate);
            MTK_ALOGV("%s(), set Policy as ForceReplace with sampling rate(%d)", __FUNCTION__, samplerate);
        } else {
            /* For update music stream with default sampling rate 48000 */
            param.addInt(String8("hifi_state"), 0);
            mHifiState = 0;
            mSampleRate_Policy = SampleRate_ForceReplace;
            policyForceReplace(AUDIO_STREAM_MUSIC, 48000);
            mSampleRate_Policy = SampleRate_Do_nothing;
            mAudioPolicyManager->mpClientInterface->setParameters(AUDIO_IO_HANDLE_NONE, param.toString(), 0);
            MTK_ALOGV("%s() , set Policy as SampleRate_Do_nothing with sampling rate(48000)", __FUNCTION__);
        }

        return true;
    } else {
        (void) enable;
        ALOGE("%s Unsupport Function", __FUNCTION__);
        return false;
    }
}
bool AudioPolicyManagerCustomImpl::setFMIndirectMode(uint32_t sampleRate)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        ALOGV("setFMIndirectMode SampleRate = %d ", sampleRate);
        AudioParameter param;
        if (sampleRate > 48000) {
            param.addInt(String8("FM_DIRECT_CONTROL"), 0);
            mAudioPolicyManager->mpClientInterface->setParameters(mFMOutput->mIoHandle, param.toString(),0);
            return true;
        } else {
            param.addInt(String8("FM_DIRECT_CONTROL"), 1);
            mAudioPolicyManager->mpClientInterface->setParameters(mFMOutput->mIoHandle, param.toString(),0);
            return false;
        }
    } else {
        ALOGV("setFMIndirectMode SampleRate = %d ", sampleRate);
    }
    return true;
}


bool AudioPolicyManagerCustomImpl::isFMActive(void)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        for (ssize_t i = 0; i < (ssize_t)mAudioPolicyManager->mAudioPatches.size(); i++) {
            ALOGVV("%s size %zu/ %zu", __FUNCTION__, i, mAudioPolicyManager->mAudioPatches.size());
            sp<AudioPatch> patchDesc = mAudioPolicyManager->mAudioPatches.valueAt(i);
            if (isFMDirectMode(patchDesc)||
                (patchDesc->mPatch.sources[0].type == AUDIO_PORT_TYPE_DEVICE
                &&patchDesc->mPatch.sources[0].ext.device.type == AUDIO_DEVICE_IN_FM_TUNER)) {
                ALOGV("FM Active");
                return true;
            }
        }
    }
    return false;
}

bool AudioPolicyManagerCustomImpl::isFMDirectActive(void)
{
    if (FeatureOption::MTK_FM_ENABLE) {
        for (ssize_t i = 0; i < (ssize_t)mAudioPolicyManager->mAudioPatches.size(); i++) {
            sp<AudioPatch> patchDesc = mAudioPolicyManager->mAudioPatches.valueAt(i);
            if (isFMDirectMode(patchDesc)) {
                ALOGV("FM Direct Active");
                return true;
            }
        }
    }
    return false;
}


bool AudioPolicyManagerCustomImpl::checkUsbSuspend(audio_devices_t device)
{
    if (FeatureOption::MTK_USB_PHONECALL) {
        audio_io_handle_t usbOutput = mAudioPolicyManager->mOutputs.getUsbOutput();
        if (usbOutput == 0) {
            mAudioPolicyManager->mAudioPolicyVendorControl.setUsbSuspended(false);
            return false;
        }

        if (mAudioPolicyManager->mAudioPolicyVendorControl.getUsbSuspended()) {
            if (mAudioPolicyManager->mPrimaryOutput->devices().types() != AUDIO_DEVICE_OUT_BUS ||
                mAudioPolicyManager->mEngine->getPhoneState() != AUDIO_MODE_IN_CALL) {
                if (mAudioPolicyManager->mAudioPolicyVendorControl.getStillInCallWithoutEnteringNormal()) {
                        ALOGD("For USB Phone Call do nothing since hal is in incall state");
                        return false;
                }
                // Restore USB HAL, after leaving Primary USB
                status_t ret = mAudioPolicyManager->mpClientInterface->restoreOutput(usbOutput);
                if (ret == NO_ERROR) {
                    mAudioPolicyManager->mAudioPolicyVendorControl.setUsbSuspended(false);
                    ALOGD("mUsbSuspended = false");
                    return true;
                } else {
                    ALOGE("checkUsbSuspend Error, ret = %d Should mUsbSuspended = false, but fail", ret);
                    return false;
                }
            }
        } else {
            if ((mAudioPolicyManager->mPrimaryOutput->devices().types() == AUDIO_DEVICE_OUT_BUS || device == AUDIO_DEVICE_OUT_BUS) &&
                mAudioPolicyManager->mEngine->getPhoneState() == AUDIO_MODE_IN_CALL) {
                // Suspend USB HAL, before routing to Primary USB
                status_t ret = mAudioPolicyManager->mpClientInterface->suspendOutput(usbOutput);
                if (ret == NO_ERROR) {
                    mAudioPolicyManager->mAudioPolicyVendorControl.setUsbSuspended(true);
                    ALOGD("mUsbSuspended = true");
                    return true;
                } else {
                    ALOGE("checkUsbSuspend Error, ret = %d Should mUsbSuspended = true, but fail", ret);
                    return false;
                }
            }
        }
    } else {
        ALOGW("%s unsupport, device 0x%x", __FUNCTION__, device);
    }
    return false;
}

int AudioPolicyManagerCustomImpl::mapVol(float &vol, float unitstep)
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        int index = (vol + 0.5)/unitstep;
        vol -= (index * unitstep);
        return index;
    } else {
        ALOGW("%s unsupport, vol %f unitstep %f", __FUNCTION__, vol, unitstep);
        return 0;
    }
}

int AudioPolicyManagerCustomImpl::mappingVoiceVol(float &vol, float unitstep)
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {

        #define ROUNDING_NUM (1)

        if (vol < unitstep) {
            return 1;
        }
        if (vol < (unitstep * 2 + ROUNDING_NUM)) {
            vol -= unitstep;
            return 2;
        } else if (vol < (unitstep * 3 + ROUNDING_NUM)) {
            vol -= unitstep * 2;
            return 3;
        } else if (vol < (unitstep * 4 + ROUNDING_NUM)) {
            vol -= unitstep * 3;
            return 4;
        } else if (vol < (unitstep * 5 + ROUNDING_NUM)) {
            vol -= unitstep * 4;
            return 5;
        } else if (vol < (unitstep * 6 + ROUNDING_NUM)) {
            vol -= unitstep * 5;
            return 6;
        } else if (vol < (unitstep * 7 + ROUNDING_NUM)) {
            vol -= unitstep * 6;
            return 7;
        } else {
            ALOGW("vole = %f unitstep = %f", vol, unitstep);
            return 0;
        }
    } else {
        ALOGW("%s unsupport, vol %f unitstep %f", __FUNCTION__, vol, unitstep);
        return 0;
    }
}


int AudioPolicyManagerCustomImpl::getStreamMaxLevels(int stream)
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        return (int) mAudioCustVolumeTable.audiovolume_level[stream];
    } else {
        ALOGW("%s unsupport, stream %d", __FUNCTION__, stream);
        return 0;
    }
}

// this function will map vol 0~100 , base on customvolume map to 0~255 , and do linear calculation to set mastervolume
float AudioPolicyManagerCustomImpl::mapVoltoCustomVol(unsigned char array[], int volmin, int volmax, float &vol ,int stream)
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        //ALOGVV("+MapVoltoCustomVol vol = %f stream = %d volmin = %d volmax = %d", vol, stream, volmin, volmax);
        CustomVolumeType vol_stream = (CustomVolumeType) stream;
        audio_stream_type_t audio_stream = (audio_stream_type_t) stream;

        if (vol_stream == CUSTOM_VOL_TYPE_VOICE_CALL || vol_stream == CUSTOM_VOL_TYPE_SIP) {
            return mapVoiceVoltoCustomVol(array, volmin, volmax, vol, stream);
        } else if (vol_stream >= CUSTOM_NUM_OF_VOL_TYPE || vol_stream < CUSTOM_VOL_TYPE_VOICE_CALL) {
            ALOGE("%s %d Error : stream = %d", __FUNCTION__, __LINE__, stream);
            audio_stream = AUDIO_STREAM_MUSIC;
            vol_stream = CUSTOM_VOL_TYPE_MUSIC;
        }

        float volume =0.0;
        if (vol == 0) {
            volume = array[0];
            vol = volume;
            return volume;
        } else {    // map volume value to custom volume
            int dMaxLevels = getStreamMaxLevels(vol_stream);
            auto &curves = mAudioPolicyManager->getVolumeCurves(audio_stream);
            int streamDescmIndexMax = curves.getVolumeIndexMax();// streamDesc.getVolumeIndexMax();
            if (dMaxLevels <= 0) {
                ALOGE("%s %d Error : dMaxLevels = %d", __FUNCTION__, __LINE__, dMaxLevels);
                dMaxLevels = 1;
            }
            if (streamDescmIndexMax <= 0) {
                ALOGE("%s %d Error : streamDescmIndexMax = %d", __FUNCTION__, __LINE__, streamDescmIndexMax);
                streamDescmIndexMax = 1;
            }

            float unitstep = fCUSTOM_VOLUME_MAPPING_STEP / dMaxLevels;
            if (vol < (fCUSTOM_VOLUME_MAPPING_STEP / streamDescmIndexMax)) {
                volume = array[0];
                vol = volume;
                return volume;
            }
            int Index = mapVol(vol, unitstep);
            float Remind = (1.0 - (vol / unitstep));
            if (Index != 0) {
                volume = ((array[Index] - (array[Index] - array[Index-1]) * Remind) + 0.5);
            } else {
                volume = 0;
            }
            //ALOGVV("%s vol [%f] unitstep [%f] Index [%d] Remind [%f] volume [%f]", __FUNCTION__, vol, unitstep, Index, Remind, volume);
        }
        // -----clamp for volume
        if (volume > 253.0) {
            volume = fCUSTOM_VOLUME_MAPPING_STEP;
        } else if (volume <= array[0]) {
            volume = array[0];
        }
        vol = volume;
        //ALOGVV("%s volume [%f] vol [%f]", __FUNCTION__, volume, vol);
        return volume;
    } else {
        ALOGW("%s unsupport, array[0] %d volmin %d volmax %d vol %f stream %d", __FUNCTION__, array[0], volmin, volmax, vol, stream);
        return 0.0;
    }
}

// this function will map vol 0~100 , base on customvolume map to 0~255 , and do linear calculation to set mastervolume
float AudioPolicyManagerCustomImpl::mapVoiceVoltoCustomVol(unsigned char array[], int volmin __unused, int volmax __unused, float &vol, int vol_stream_type)
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        vol = (int)vol;
        float volume = 0.0;
    //  StreamDescriptor &streamDesc = mStreams.valueFor((audio_stream_type_t)AUDIO_STREAM_VOICE_CALL);//mStreams[AUDIO_STREAM_VOICE_CALL];
        if (vol == 0) {
            volume = array[0];
        } else {
            int dMaxIndex = getStreamMaxLevels(AUDIO_STREAM_VOICE_CALL)-1;
            if (dMaxIndex < 0) {
                ALOGE("%s %d Error : dMaxIndex = %d", __FUNCTION__, __LINE__, dMaxIndex);
                dMaxIndex = 1;
            }
            if (vol >= fCUSTOM_VOLUME_MAPPING_STEP) {
                volume = array[dMaxIndex];
                //ALOGVV("%s volumecheck stream = %d index = %d volume = %f", __FUNCTION__, AUDIO_STREAM_VOICE_CALL, dMaxIndex, volume);
            } else {
                double unitstep = fCUSTOM_VOLUME_MAPPING_STEP / dMaxIndex;
                int Index = mappingVoiceVol(vol, unitstep);
                // boundary for array
                if (Index >= dMaxIndex) {
                    Index = dMaxIndex;
                }
                float Remind = (1.0 - (float)vol/unitstep) ;
                if (Index != 0) {
                    volume = (array[Index]  - (array[Index] - array[Index- 1]) * Remind)+0.5;
                } else {
                    volume =0;
                }
                //ALOGVV("%s volumecheck stream = %d index = %d volume = %f", __FUNCTION__, AUDIO_STREAM_VOICE_CALL, Index, volume);
                //ALOGVV("%s dMaxIndex [%d] vol [%f] unitstep [%f] Index [%d] Remind [%f] volume [%f]", __FUNCTION__, dMaxIndex, vol, unitstep, Index, Remind, volume);
            }
        }

         if (volume > CUSTOM_VOICE_VOLUME_MAX && vol_stream_type == CUSTOM_VOL_TYPE_VOICE_CALL) {
             volume = CUSTOM_VOICE_VOLUME_MAX;
         }
         else if (volume > 253.0) {
            volume = fCUSTOM_VOLUME_MAPPING_STEP;
         }
         else if (volume <= array[0]) {
             volume = array[0];
         }

         vol = volume;
         if (vol_stream_type == CUSTOM_VOL_TYPE_VOICE_CALL) {
             float degradeDb = (CUSTOM_VOICE_VOLUME_MAX - vol) / CUSTOM_VOICE_ONEDB_STEP;
             //ALOGVV("%s volume [%f] degradeDb [%f]", __FUNCTION__, volume, degradeDb);
             vol = fCUSTOM_VOLUME_MAPPING_STEP - (degradeDb * 4);
             volume = vol;
         }
         //ALOGVV("%s volume [%f] vol [%f]", __FUNCTION__, volume, vol);
             return volume;
     } else {
        ALOGW("%s unsupport, array[0] %d vol %f vol_stream_type %d", __FUNCTION__, array[0], vol, vol_stream_type);
        return 0.0;
    }
}

float AudioPolicyManagerCustomImpl::computeCustomVolume(int stream, int index, audio_devices_t device)
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        // check if force use exist , get output device for certain mode
        device_category deviceCategory = Volume::getDeviceCategory(device);
        // compute custom volume
        float volume = 0.0;
        int volmax = 0, volmin = 0; // volumeindex = 0;
        int custom_vol_device_mode, audiovolume_steamtype;
        int dMaxStepIndex = 0;

        //ALOGVV("%s volumecheck stream = %d index = %d device = %d", __FUNCTION__, stream, index, device);

        if (mAudioPolicyManager->mAudioPolicyVendorControl.getVoiceReplaceDTMFStatus() && stream == AUDIO_STREAM_DTMF) {
            // normalize new index from 0~15(audio) to 0~6(voice)
            // int tempindex = index;
            auto &DTMFcurves = mAudioPolicyManager->getVolumeCurves(AUDIO_STREAM_DTMF);
            auto &VOIPcurves = mAudioPolicyManager->getVolumeCurves(AUDIO_STREAM_VOICE_CALL);
            float DTMFvolInt = (fCUSTOM_VOLUME_MAPPING_STEP * (index - DTMFcurves.getVolumeIndexMin())) /
                (DTMFcurves.getVolumeIndexMax() - DTMFcurves.getVolumeIndexMin());
            index = (DTMFvolInt * (VOIPcurves.getVolumeIndexMax() - VOIPcurves.getVolumeIndexMin()) /
                (fCUSTOM_VOLUME_MAPPING_STEP)) + VOIPcurves.getVolumeIndexMin();
            //MTK_ALOGVV("volumecheck refine DTMF index [%d] to Voice index [%d]", tempindex, index);
            stream = (int) AUDIO_STREAM_VOICE_CALL;
        }

        if (FeatureOption::MTK_TTY_SUPPORT) {
            if(mAudioPolicyManager->isInCall() == true && mTty_Ctm != AUD_TTY_OFF) {
                deviceCategory = Volume::getDeviceCategory(getNewDeviceForTty(device, mTty_Ctm));
                stream = (int) AUDIO_STREAM_VOICE_CALL;
            }
        }
        auto &curves = mAudioPolicyManager->getVolumeCurves(mAudioPolicyManager->toVolumeSource((audio_stream_type_t)stream));
        float volInt = 1.0;
        if((curves.getVolumeIndexMax() -curves.getVolumeIndexMin()) > 0) {
            volInt = (fCUSTOM_VOLUME_MAPPING_STEP * (index - curves.getVolumeIndexMin())) / (curves.getVolumeIndexMax() -curves.getVolumeIndexMin());
        }

        if (deviceCategory == DEVICE_CATEGORY_SPEAKER) {
            custom_vol_device_mode = CUSTOM_VOLUME_SPEAKER_MODE;
            if ((device & AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
                 (device & AUDIO_DEVICE_OUT_WIRED_HEADPHONE))
                    custom_vol_device_mode = CUSTOM_VOLUME_HEADSET_SPEAKER_MODE;
        } else if (deviceCategory == DEVICE_CATEGORY_HEADSET) {
            custom_vol_device_mode = CUSTOM_VOLUME_HEADSET_MODE;
        } else if (deviceCategory == DEVICE_CATEGORY_EARPIECE) {
            custom_vol_device_mode = CUSTOM_VOLUME_NORMAL_MODE;
        } else {
            custom_vol_device_mode = CUSTOM_VOLUME_HEADSET_SPEAKER_MODE;
        }

        if ((stream == (int) AUDIO_STREAM_VOICE_CALL) && (mAudioPolicyManager->mEngine->getPhoneState() == AUDIO_MODE_IN_COMMUNICATION)) {
            audiovolume_steamtype = (int) CUSTOM_VOL_TYPE_SIP;
        } else if (stream >= (int) AUDIO_STREAM_VOICE_CALL && (stream < (int) AUDIO_STREAM_CNT)) {
            audiovolume_steamtype = stream;
        } else {
            audiovolume_steamtype = (int) CUSTOM_VOL_TYPE_MUSIC;
            ALOGE("%s %d Error : audiovolume_steamtype = %d", __FUNCTION__, __LINE__, audiovolume_steamtype);
        }

        dMaxStepIndex = getStreamMaxLevels(audiovolume_steamtype) - 1;

        if (dMaxStepIndex > CUSTOM_AUDIO_MAX_VOLUME_STEP - 1) {
            ALOGE("%s %d Error : dMaxStepIndex = %d", __FUNCTION__, __LINE__, dMaxStepIndex);
            dMaxStepIndex = CUSTOM_AUDIO_MAX_VOLUME_STEP - 1;
        } else if (dMaxStepIndex < 0) {
            ALOGE("%s %d Error : dMaxStepIndex = %d", __FUNCTION__, __LINE__, dMaxStepIndex);
            dMaxStepIndex = 0;
        }

        volmax = mAudioCustVolumeTable.audiovolume_steamtype[audiovolume_steamtype][custom_vol_device_mode][dMaxStepIndex];
        volmin = mAudioCustVolumeTable.audiovolume_steamtype[audiovolume_steamtype][custom_vol_device_mode][0];
        //ALOGVV("%s audiovolume_steamtype %d custom_vol_device_mode %d stream %d", __FUNCTION__, audiovolume_steamtype, custom_vol_device_mode, audiovolume_steamtype);
        //ALOGVV("%s getStreamMaxLevels(stream) %d volmax %d volmin %d volInt %f index %d", __FUNCTION__, getStreamMaxLevels(audiovolume_steamtype), volmax, volmin, volInt, index);
        volume = mapVoltoCustomVol(mAudioCustVolumeTable.audiovolume_steamtype[audiovolume_steamtype][custom_vol_device_mode], volmin, volmax, volInt, audiovolume_steamtype);

        volume = linearToLog(volume);
        //ALOGVV("stream = %d after computeCustomVolume , volInt = %f volume = %f volmin = %d volmax = %d", audiovolume_steamtype, volInt, volume, volmin, volmax);
        return volume;
    } else {
        ALOGW("%s unsupport, stream %d index %d device %d", __FUNCTION__, stream, index, device);
        return 0.0;
    }
}

void AudioPolicyManagerCustomImpl::loadCustomVolume()
{
    if (FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        mAudioCustVolumeTable.bRev = CUSTOM_VOLUME_REV_1;
        mAudioCustVolumeTable.bReady = 0;
#if 0
        //MTK_ALOGVV("B4 Update");
        for (int i = 0; i < CUSTOM_NUM_OF_VOL_TYPE; i++) {
            //MTK_ALOGVV("StreamType %d", i);
            for (int j = 0; j < CUSTOM_NUM_OF_VOL_MODE; j++) {
                //MTK_ALOGVV("DeviceType %d", j);
                for (int k = 0; k < CUSTOM_AUDIO_MAX_VOLUME_STEP; k++) {
                    //MTK_ALOGVV("[IDX]:[Value] %d, %d", k, mAudioCustVolumeTable.audiovolume_steamtype[i][j][k]);
                }
            }
        }
#endif
        mAudioPolicyManager->mpClientInterface->getCustomAudioVolume(&mAudioCustVolumeTable);
        if (mAudioCustVolumeTable.bReady != 0) {
            ALOGD("mUseCustomVolume true");
            mAudioPolicyManager->mAudioPolicyVendorControl.setCustomVolumeStatus(true);
        } else {
            ALOGD("mUseCustomVolume false");
            mAudioPolicyManager->mAudioPolicyVendorControl.setCustomVolumeStatus(false);
        }
#if 0
        //MTK_ALOGVV("After Update");
        for (int i = 0; i < CUSTOM_NUM_OF_VOL_TYPE; i++) {
            //MTK_ALOGVV("StreamType %d", i);
            for (int j = 0; j < CUSTOM_NUM_OF_VOL_MODE; j++) {
                //MTK_ALOGVV("DeviceType %d", j);
                for (int k = 0; k < CUSTOM_AUDIO_MAX_VOLUME_STEP; k++) {
                    //MTK_ALOGVV("[IDX]:[Value] %d, %d", k, mAudioCustVolumeTable.audiovolume_steamtype[i][j][k]);
                }
            }
        }
#endif
    } else {
        ALOGW("%s unsupport", __FUNCTION__);
    }
}

status_t AudioPolicyManagerCustomImpl::updatePrimaryModuleDevice(audio_devices_t rxDevice)
{
#if defined(MTK_LOW_LATENCY)
    // force restoring the device selection on other active outputs if it differs from the
    // one being selected for this output. Must run checkDeviceMuteStrategies if any stream is active, ALPS03074028
    for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
        sp<SwAudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.valueAt(i);
        if (desc != mAudioPolicyManager->mPrimaryOutput &&
                desc->isActive() &&
                mAudioPolicyManager->mPrimaryOutput->sharesHwModuleWith(desc) &&
                (rxDevice != desc->devices().types() || (!mAudioPolicyManager->mPrimaryOutput->isActive() && rxDevice != mAudioPolicyManager->mPrimaryOutput->devices().types()))) {
            mAudioPolicyManager->setOutputDevices(desc,
                            mAudioPolicyManager->getNewOutputDevices(desc, false /*fromCache*/),
                            true,
                            desc->latency()*2);
        }
    }
    return NO_ERROR;
#else
    ALOGW("%s unsupport, device 0x%x", __FUNCTION__, rxDevice);
    return INVALID_OPERATION;
#endif
}

status_t AudioPolicyManagerCustomImpl::freeGainTable(void)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
        if (mGainTable.sceneGain != NULL) {
            delete[] mGainTable.sceneGain;
            mGainTable.sceneGain = NULL;
            mGainTable.sceneCount = 0;
        }
        return NO_ERROR;
    } else {
        ALOGW("%s unsupport", __FUNCTION__);
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::allocateGainTable(void)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE || FeatureOption::MTK_AUDIO_GAIN_NVRAM) {
        String8 sceneCount = mAudioPolicyManager->mpClientInterface->getParameters(0, String8("GetGainTableSceneCount"));
        String8 sceneGain = mAudioPolicyManager->mpClientInterface->getParameters(0, String8("GetGainTableSceneTable="));
        String8 nonSceneGain = mAudioPolicyManager->mpClientInterface->getParameters(0, String8("GetGainTableNonSceneTable="));
        String8 newvalSceneGain;
        String8 newvalNonSceneGain;
        newvalSceneGain.appendFormat("%s", sceneGain.string() + String8("GetGainTableSceneTable=").size());
        newvalNonSceneGain.appendFormat("%s", nonSceneGain.string() + String8("GetGainTableNonSceneTable=").size());
        AudioParameter param = AudioParameter(sceneCount);
        int valueInt;
        status_t ret;
        if (param.getInt(String8("GetGainTableSceneCount"), valueInt) == NO_ERROR) {
            mGainTableSceneCount = valueInt;
            ALOGD("getGainTable, mGainTableSceneCount %d", mGainTableSceneCount);
        } else {
            ALOGE("error, load GetGainTableSceneIndex failed!!");
            mGainTableSceneCount = 1;
        }
        freeGainTable();
        mGainTable.sceneGain = new GainTableForScene[mGainTableSceneCount];
        mGainTable.sceneCount = mGainTableSceneCount;
        // Load sceneGain
        ret = AudioPolicyServiceCustomImpl::common_getDecodedData(newvalSceneGain, sizeof(GainTableForScene) * mGainTableSceneCount, mGainTable.sceneGain);
        if (ret) {
            ALOGE("Load GetGainTableSceneTable Fail ret = %d", ret);
        } else {
        // Load nonSceneGain
            ret = AudioPolicyServiceCustomImpl::common_getDecodedData(newvalNonSceneGain, sizeof(mGainTable.nonSceneGain), &(mGainTable.nonSceneGain));
            if (ret) {
                ALOGE("Load GetGainTableNonSceneTable Fail ret = %d", ret);
            }
        }

        updateCurrentSceneIndexOfGainTable();
        return ret;
    } else {
        ALOGW("%s unsupport", __FUNCTION__);
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::updateCurrentSceneIndexOfGainTable(void)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
    //  mGainTableSceneCount = mGainTable.sceneCount;
        String8 command = mAudioPolicyManager->mpClientInterface->getParameters(0, String8("GetGainTableCurrentSceneIndex"));
        AudioParameter param = AudioParameter(command);
        int valueInt;
        if (param.getInt(String8("GetGainTableCurrentSceneIndex"), valueInt) == NO_ERROR) {
            if (valueInt < mGainTableSceneCount) {
                mGainTableSceneIndex = valueInt;
                ALOGD("Using scene [%d]/[%d]", valueInt, mGainTableSceneCount);
            } else {
                ALOGE("error, load valueInt failed [%d]/[%d]", valueInt, mGainTableSceneCount);
            }
        } else {
            ALOGE("error, load GetGainTableSceneIndex failed!!");
        }
        return NO_ERROR;
    } else {
        ALOGW("%s unsupport", __FUNCTION__);
        return INVALID_OPERATION;
    }
}

int AudioPolicyManagerCustomImpl::getSceneIndexOfGainTable(String8 sceneName)
{
    if (FeatureOption::MTK_AUDIO_GAIN_TABLE) {
    //  mGainTableSceneCount = mGainTable.sceneCount;
        String8 command = mAudioPolicyManager->mpClientInterface->getParameters(0, String8("GetGainTableSceneIndex=") + sceneName);
        AudioParameter param = AudioParameter(command);
        int valueInt = 0;
        if (param.getInt(String8("GetGainTableSceneIndex"), valueInt) == NO_ERROR) {
            if (valueInt < mGainTableSceneCount) {
                ALOGD("Using scene [%d]/[%d]", valueInt, mGainTableSceneCount);
            } else {
                ALOGE("error, load valueInt failed [%d]/[%d]", valueInt, mGainTableSceneCount);
            }
        } else {
            ALOGE("error, load GetGainTableSceneIndex failed!!");
        }
        return valueInt;
    } else {
        (void) sceneName;
        ALOGW("%s unsupport", __FUNCTION__);
        return 0;
    }
}

uint32_t AudioPolicyManagerCustomImpl::getSampleRateIndex(uint32_t sampleRate)
{
    if (sampleRate >=  OUTPUT_RATE_192) {
        return OUTPUT_RATE_192_INDEX;
    } else if (sampleRate >= OUTPUT_RATE_176_4) {
        return OUTPUT_RATE_176_4_INDEX;
    } else if (sampleRate >= OUTPUT_RATE_96) {
        return OUTPUT_RATE_96_INDEX ;
    } else {
        return OUTPUT_RATE_48_INDEX;
    }
    return OUTPUT_RATE_48_INDEX;
}

uint32_t AudioPolicyManagerCustomImpl::getSampleRateCount()
{
    uint32_t index = 0;
    for (int i = 0 ; i < OUTPUT_NUM_RATE_INDEX ; i++) {
        index += mSampleRateArray[i];
    }
    MTK_ALOGV("%s index %d", __FUNCTION__, index);
    return index;
}

bool AudioPolicyManagerCustomImpl::checkFirstActive(void)
{
    uint32_t index = 0;
    for (int i = 0 ; i < OUTPUT_NUM_RATE_INDEX ; i++) {
        index += mSampleRateArray[i];
    }

    if (index == 1) {
        return true;
    }

    return false;
}

bool AudioPolicyManagerCustomImpl::checkStreamActive(void)
{
    for (int i = 0 ; i < OUTPUT_NUM_RATE_INDEX ; i++) {
        if (mSampleRateArray[i] != 0) {
            return true;
        }
    }
    return false;
}

void AudioPolicyManagerCustomImpl::dumpSampleRateArray(void)
{
//#define DEBUG_SAMPLERATE_ARRAY
#if defined(DEBUG_SAMPLERATE_ARRAY)
    for (int i = 0 ; i < OUTPUT_NUM_RATE_INDEX ; i++) {
        if (mSampleRateArray[i] > 0) {
            MTK_ALOGD("mSampleRateArray[%d] = %d ", i, mSampleRateArray[i]);
        }
    }
#endif
}

uint32_t AudioPolicyManagerCustomImpl::getMaxTrackSampleRate(audio_stream_type_t stream)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        uint32_t SampleRate = OUTPUT_RATE_48;

        if (mAudioPolicyManager->mPrimaryOutput->isActive(mAudioPolicyManager->toVolumeSource(AUDIO_STREAM_VOICE_CALL))
            || stream == AUDIO_STREAM_VOICE_CALL) {
            //use voice stream sample rate during in call
            audio_io_handle_t voipRxHandle = getVOIPRxOutput();
            if (voipRxHandle != AUDIO_IO_HANDLE_NONE) {
                ssize_t index = mAudioPolicyManager->mOutputs.indexOfKey(voipRxHandle);
                sp<SwAudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.valueAt(index);
                SampleRate = desc->mSamplingRate;
            }
            ALOGD("%s(),  use voice call sample rate %d", __FUNCTION__, SampleRate);
            return SampleRate;
        }

        int index = -1;
        for (int i = OUTPUT_NUM_RATE_INDEX-1 ;i >= 0 ; i--) {
            if (mSampleRateArray[i] > 0) {
                MTK_ALOGV("mSampleRateArray[%d] = %d", i, mSampleRateArray[i]);
                index = i;
                break;
            }
        }

        switch (index) {
            case OUTPUT_RATE_8_INDEX:
                SampleRate = OUTPUT_RATE_8;
                break;
            case OUTPUT_RATE_11_025_INDEX:
                SampleRate = OUTPUT_RATE_11_025;
                break;
            case OUTPUT_RATE_16_INDEX:
                SampleRate = OUTPUT_RATE_16;
                break;
            case OUTPUT_RATE_22_05_INDEX:
                SampleRate = OUTPUT_RATE_22_05;
                break;
            case OUTPUT_RATE_32_INDEX:
                SampleRate = OUTPUT_RATE_32;
                break;
            case OUTPUT_RATE_44_1_INDEX:
                SampleRate = OUTPUT_RATE_44_1;
                break;
            case OUTPUT_RATE_48_INDEX:
                SampleRate = OUTPUT_RATE_48;
                break;
            case OUTPUT_RATE_88_2_INDEX:
                SampleRate = OUTPUT_RATE_88_2;
                break;
            case OUTPUT_RATE_96_INDEX:
                SampleRate = OUTPUT_RATE_96;
                break;
            case OUTPUT_RATE_176_4_INDEX:
                SampleRate = OUTPUT_RATE_176_4;
                break;
            case OUTPUT_RATE_192_INDEX:
                SampleRate = OUTPUT_RATE_192;
                break;
            default:
                SampleRate = OUTPUT_RATE_48;
                break;
        }
        ALOGD("getMaxTrackSampleRate %d", SampleRate);
        return SampleRate;
    } else {
        return OUTPUT_RATE_48;
    }
}

uint32_t AudioPolicyManagerCustomImpl::getFirstTrackSampleRate()
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        uint32_t SampleRate = 0;
        for (int i = 0 ; i < OUTPUT_NUM_RATE_INDEX ; i++) {
            if (mSampleRateArray[i]) {
                switch (i) {
                    case OUTPUT_RATE_8_INDEX:
                        SampleRate = OUTPUT_RATE_8;
                        break;
                    case OUTPUT_RATE_11_025_INDEX:
                        SampleRate = OUTPUT_RATE_11_025;
                        break;
                    case OUTPUT_RATE_16_INDEX:
                        SampleRate = OUTPUT_RATE_16;
                        break;
                    case OUTPUT_RATE_22_05_INDEX:
                        SampleRate = OUTPUT_RATE_22_05;
                        break;
                    case OUTPUT_RATE_32_INDEX:
                        SampleRate = OUTPUT_RATE_32;
                        break;
                    case OUTPUT_RATE_44_1_INDEX:
                        SampleRate = OUTPUT_RATE_44_1;
                        break;
                    case OUTPUT_RATE_48_INDEX:
                        SampleRate = OUTPUT_RATE_48;
                        break;
                    case OUTPUT_RATE_88_2_INDEX:
                        SampleRate = OUTPUT_RATE_88_2;
                        break;
                    case OUTPUT_RATE_96_INDEX:
                        SampleRate = OUTPUT_RATE_96;
                        break;
                    case OUTPUT_RATE_176_4_INDEX:
                        SampleRate = OUTPUT_RATE_176_4;
                        break;
                    case OUTPUT_RATE_192_INDEX:
                        SampleRate = OUTPUT_RATE_192;
                        break;
                }
                break;
            }
        }
        return SampleRate;
    } else {
        return OUTPUT_RATE_48;
    }
}

bool AudioPolicyManagerCustomImpl::isPrimarySupportSampleRate(uint32_t SampleRate)
{
    MTK_ALOGV("isPrimarySupportSampleRate sampling rate = %d", SampleRate);
    return true;
/* ToDo :
    Check SampleRate is valid or not.
*/
}

status_t AudioPolicyManagerCustomImpl::policyFirstStart(audio_stream_type_t stream, uint32_t sampleRate)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        uint32_t SampleRate = 0;
        MTK_ALOGV("policyFirstStart stream = %d sampleRate = %d, checkFirstActive() %d mSampleRate_Policy = %d", stream, sampleRate, checkFirstActive(), mSampleRate_Policy);
        if (checkFirstActive() == true) {
            SampleRate = getFirstTrackSampleRate();
            if (isPrimarySupportSampleRate(SampleRate) && (mPolicySampleRate != SampleRate)) {
                mSampleRateFocusLock.lock();
                mPolicySampleRate = mSampleRateFocus ? mSampleRateFocus : SampleRate;
                mSampleRateFocusLock.unlock();
                SampleRate = mPolicySampleRate;
                AudioParameter param = AudioParameter();
                param.addInt(String8(AudioParameter::keySamplingRate), (int)mPolicySampleRate);
                mAudioPolicyManager->mpClientInterface->setParameters(mAudioPolicyManager->mPrimaryOutput->mIoHandle, param.toString());

                audio_io_handle_t fastOutput = getPrimaryFastOutput();
                if(fastOutput) {
                    mAudioPolicyManager->mpClientInterface->setParameters(fastOutput, param.toString());
                }
            }
            mSampleRateFocusLock.lock();
            mSampleRateFocus = 0;
            mSampleRateFocusLock.unlock();
            setFMIndirectMode(SampleRate);
        }
        return NO_ERROR;
    } else {
        (void) stream;
        (void) sampleRate;
        ALOGE("%s Unsupport Function", __FUNCTION__);
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::policyRestore(audio_stream_type_t stream)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        ALOGD("%s()+ mPolicySampleRate %d", __FUNCTION__, mPolicySampleRate);

        if (mSampleRate_Policy != SampleRate_Do_nothing && stream == AUDIO_STREAM_MUSIC) {
            mPolicySampleRate = OUTPUT_RATE_48;
            AudioParameter param = AudioParameter();
            param.addInt(String8(AudioParameter::keySamplingRate), (int)mPolicySampleRate);
            ALOGD("%s() set mPolicySampleRate %d", __FUNCTION__, mPolicySampleRate);
            bool bDeepBufferSupported = isSupportDeepBuffer();
            if(bDeepBufferSupported) {
                audio_io_handle_t deepBuffer_Iohandle = -1;
                for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
                     sp<SwAudioOutputDescriptor> outputdesc = mAudioPolicyManager->mOutputs.valueAt(i);
                     MTK_ALOGV("%s() size %zu, outputdesc->mFlags 0x%x", __FUNCTION__, i, outputdesc->mFlags);
                     if (outputdesc->mFlags & AUDIO_OUTPUT_FLAG_DEEP_BUFFER) {
                        deepBuffer_Iohandle = outputdesc->mIoHandle;
                        MTK_ALOGV("found deep buffer IO handle %d", deepBuffer_Iohandle);
                        break;
                     }
                }
                mAudioPolicyManager->mpClientInterface->setParameters(deepBuffer_Iohandle, param.toString());
                return NO_ERROR;
            }
            mAudioPolicyManager->mpClientInterface->setParameters(
                mAudioPolicyManager->mPrimaryOutput->mIoHandle, param.toString());
        }
        return NO_ERROR;
    } else {
        (void) stream;
        ALOGE("%s Unsupport Function", __FUNCTION__);
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::policyForceReplace(audio_stream_type_t stream, uint32_t sampleRate, bool forceReplace)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
            // only take place when music stream, FM is an exception for samplerate change
            MTK_ALOGV("%s() stream = %d sampleRate = %d forceReplace = %d", __FUNCTION__, stream, sampleRate, forceReplace);

            if ((stream == AUDIO_STREAM_MUSIC && !isMMAPActive()
                 && !mAudioPolicyManager->isStreamActive(AUDIO_STREAM_VOICE_CALL))
                 || stream == AUDIO_STREAM_VOICE_CALL || forceReplace) {
                MTK_ALOGV("%s() music stream and no FM mSampleRateFocus %d", __FUNCTION__, mSampleRateFocus);
                if (stream == AUDIO_STREAM_VOICE_CALL) {
                    MTK_ALOGV("voice call force replace samplerate with %d ", sampleRate);
                }
                if ((isPrimarySupportSampleRate(sampleRate) && (mPolicySampleRate != sampleRate)) || mSampleRateForce) {
                    mSampleRateFocusLock.lock();
                    mPolicySampleRate = mSampleRateFocus ? mSampleRateFocus : sampleRate;
                    mSampleRateFocusLock.unlock();
                    sampleRate = mPolicySampleRate;
                    mSampleRateForce = false;
                    AudioParameter param = AudioParameter();
                    param.addInt(String8(AudioParameter::keySamplingRate), (int)mPolicySampleRate);
                    audio_io_handle_t deepBuffer_Iohandle = -1;
                    for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
                        sp<SwAudioOutputDescriptor> outputdesc = mAudioPolicyManager->mOutputs.valueAt(i);
                        MTK_ALOGV("%s() size %zu, outputdesc->mFlags 0x%x", __FUNCTION__, i, outputdesc->mFlags);
                        if (outputdesc->mFlags & AUDIO_OUTPUT_FLAG_DEEP_BUFFER) {
                            deepBuffer_Iohandle = outputdesc->mIoHandle;
                            MTK_ALOGV("found deep buffer IO handle %d", deepBuffer_Iohandle);
                            break;
                        }
                    }

                    if (deepBuffer_Iohandle != -1) {
                        MTK_ALOGV("%s() set Iohandle %d mPolicySampleRate %d", __FUNCTION__, deepBuffer_Iohandle, mPolicySampleRate);
                        mAudioPolicyManager->mpClientInterface->setParameters(deepBuffer_Iohandle, param.toString());
                    } else {
                        MTK_ALOGV("%s() can't find  deep buffer handle mPolicySampleRate %d, force set primary", __FUNCTION__, mPolicySampleRate);
                        mAudioPolicyManager->mpClientInterface->setParameters(mAudioPolicyManager->mPrimaryOutput->mIoHandle, param.toString());
                    }
                }
            }
        return NO_ERROR;
    } else {
        (void) stream;
        (void) sampleRate;
        ALOGE("%s Unsupport Function", __FUNCTION__);
        return INVALID_OPERATION;
    }
}

// function to make deision for audio
status_t AudioPolicyManagerCustomImpl::sampleRatePolicy(audio_stream_type_t stream, uint32_t sampleRate)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        // and then default policy
        MTK_ALOGV("%s() mSampleRate_Policy= %d", __FUNCTION__, mSampleRate_Policy);
        if (mSampleRate_Policy == SampleRate_First_Start) {
            policyFirstStart(stream, sampleRate);
        } else if (mSampleRate_Policy == SampleRate_ForceReplace) {
            policyForceReplace(stream, sampleRate);
        } else if (mSampleRate_Policy == SampleRate_Do_nothing) {
            // do nothing
        }
        return NO_ERROR;
    } else {
        (void) stream;
        (void) sampleRate;
        ALOGE("%s Unsupport Function", __FUNCTION__);
        return INVALID_OPERATION;
    }
}


status_t AudioPolicyManagerCustomImpl::addSampleRateArray(audio_stream_type_t stream __unused, uint32_t sampleRate)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        uint32_t index = 0;
        index = getSampleRateIndex(sampleRate);
        MTK_ALOGV("%s() index = %d sampleRate = %d", __FUNCTION__, index, sampleRate);
        mSampleRateArray[index]++;
        dumpSampleRateArray();
        return NO_ERROR;
    } else {
        (void) sampleRate;
        ALOGE("%s Unsupport Function", __FUNCTION__);
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::removeSampleRateArray(audio_stream_type_t stream __unused, uint32_t sampleRate)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        uint32_t index = 0;
        index = getSampleRateIndex(sampleRate);
        MTK_ALOGD("%s() index = %d sampleRate = %d", __FUNCTION__, index, sampleRate);
        mSampleRateArray[index]--;
        dumpSampleRateArray();
        return NO_ERROR;
    } else {
        (void) sampleRate;
        ALOGE("%s Unsupport Function", __FUNCTION__);
        return INVALID_OPERATION;
    }
}


status_t AudioPolicyManagerCustomImpl::hifiAudio_startOutputSamplerate(audio_port_handle_t portId, int samplerate)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        ALOGD("%s() +output = %d  +samplerate = %d mPrimaryOutput = %d HifiState = %d",
            __FUNCTION__, portId, samplerate, mAudioPolicyManager->mPrimaryOutput->mIoHandle, mHifiState);
        status_t startOutputValue = mAudioPolicyManager->startOutput(portId);

        sp<SwAudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.getOutputForClient(portId);
        if (desc == 0) {
            ALOGW("hifiAudio_startOutputSamplerate() no output for client %d", portId);
            return BAD_VALUE;
        }
        sp<TrackClientDescriptor> client = desc->getClient(portId);

        ALOGV("hifiAudio_startOutputSamplerate() output %d, stream %d, session %d",
              portId, client->stream(), client->session());

        bool bDeepBufferSupported = isSupportDeepBuffer();
        MTK_ALOGV("bDeepBufferSupported %d desc->mFlags 0x%x", bDeepBufferSupported, desc->mFlags);
        bool bCouldRunWithHifiTrack = ((bDeepBufferSupported && (desc->mFlags & AUDIO_OUTPUT_FLAG_DEEP_BUFFER)) // if use deep buffer, check for deep buffer flag
        || (!bDeepBufferSupported && (desc->mFlags & AUDIO_OUTPUT_FLAG_PRIMARY)));
        if (mHifiState == -1) {
            AudioParameter param;
            bool bHifiState = getHIFIState();
            ALOGD("%s() force update mHifiState(%d) to %d", __FUNCTION__, mHifiState, bHifiState);
            param.addInt(String8("hifi_state"), bHifiState);
            mAudioPolicyManager->mpClientInterface->setParameters(AUDIO_IO_HANDLE_NONE, param.toString(), 0);
            mHifiState = bHifiState;
        }

    if (!bCouldRunWithHifiTrack && (!checkStreamActive()|| client->stream() == AUDIO_STREAM_VOICE_CALL)) {
        //ALPS04689735 switch non-hifi output samplerate back to default sample rate
        if (getHIFIState()) {
            samplerate = getMaxTrackSampleRate(client->stream());
            policyForceReplace(client->stream(), samplerate, true);
        }
    } else if (bCouldRunWithHifiTrack) {
        addSampleRateArray(client->stream(), (uint32_t) samplerate);
        if (getHIFIState()) {
            samplerate = getMaxTrackSampleRate();
            sampleRatePolicy(client->stream(), (uint32_t) samplerate);
        }
    }
        startOutputValue = mAudioPolicyManager->startOutput(portId);

        if (bCouldRunWithHifiTrack && (startOutputValue != NO_ERROR)) {
            removeSampleRateArray(client->stream(), (uint32_t) samplerate);
            if (getHIFIState()) {
                samplerate = getMaxTrackSampleRate();
                sampleRatePolicy(client->stream(), (uint32_t) samplerate);
            }
        }
        return startOutputValue;
    } else {
        (void) portId;
        (void) samplerate;
        ALOGE("%s Unsupport Function", __FUNCTION__);
        return INVALID_OPERATION;
    }
}

status_t AudioPolicyManagerCustomImpl::hifiAudio_stopOutputSamplerate(audio_port_handle_t portId, int samplerate)
{
    if (FeatureOption::MTK_HIFIAUDIO_SUPPORT) {
        ALOGD("%s() -output = %d -samplerate = %d mPrimaryOutput = %d HifiState = %d",
            __FUNCTION__, portId, samplerate, mAudioPolicyManager->mPrimaryOutput->mIoHandle, mHifiState);

        sp<SwAudioOutputDescriptor> outputDesc = mAudioPolicyManager->mOutputs.getOutputForClient(portId);
        if (outputDesc == 0) {
            ALOGW("hifiAudio_stopOutputSamplerate() no output for client %d", portId);
            return BAD_VALUE;
        }
        sp<TrackClientDescriptor> client = outputDesc->getClient(portId);

        ALOGV("hifiAudio_stopOutputSamplerate() output %d, stream %d, session %d",
              outputDesc->mIoHandle, client->stream(), client->session());

        bool bDeepBufferSupported = isSupportDeepBuffer();
         MTK_ALOGV("bDeepBufferSupported %d outputDesc->mFlags 0x%x", bDeepBufferSupported, outputDesc->mFlags);
         if ((outputDesc->isActive(mAudioPolicyManager->toVolumeSource((audio_stream_type_t) client->stream())) > 0)
             && ((bDeepBufferSupported && (outputDesc->mFlags & AUDIO_OUTPUT_FLAG_DEEP_BUFFER)) // if use deep buffer, check for deep buffer flag
             || (!bDeepBufferSupported && (outputDesc->mFlags & AUDIO_OUTPUT_FLAG_PRIMARY)))) {// if not use deep buffer, check for non-fast primary  output
             removeSampleRateArray(client->stream(), (uint32_t) samplerate);

            if (getHIFIState() && checkStreamActive()) {
                samplerate = getMaxTrackSampleRate();
                sampleRatePolicy(client->stream(), (uint32_t) samplerate);
            }
        }
        return mAudioPolicyManager->stopOutput(portId);
    } else{
        (void) portId;
        (void) samplerate;
        ALOGE("%s Unsupport Function", __FUNCTION__);
        return INVALID_OPERATION;
    }
}

void AudioPolicyManagerCustomImpl::initSamplerateArray(uint32_t init_sample_rate)
{
    for (int i = 0; i < OUTPUT_NUM_RATE_INDEX; i++) {
        mSampleRateArray[i] = 0;
    }
    mSampleRate_Policy = SampleRate_ForceReplace;
    if (init_sample_rate) {
        mPolicySampleRate = init_sample_rate;
    } else {
        mPolicySampleRate = 48000;
    }
    mSampleRateFocus = mSampleRateFocusCount = 0;
}

audio_io_handle_t AudioPolicyManagerCustomImpl::getPrimaryFastOutput()
{
    for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
      sp<SwAudioOutputDescriptor> outputDesc = mAudioPolicyManager->mOutputs.valueAt(i);
      if (!outputDesc->isDuplicated() && outputDesc->mFlags & AUDIO_OUTPUT_FLAG_FAST) {
          return mAudioPolicyManager->mOutputs.keyAt(i);
      }
    }
    return 0;
}

bool AudioPolicyManagerCustomImpl::isSupportDeepBuffer(void)
{
    for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
      sp<SwAudioOutputDescriptor> outputDesc = mAudioPolicyManager->mOutputs.valueAt(i);
      ALOGV("isSupportDeepBuffer, outputDesc->mFlags 0x%x", outputDesc->mFlags);
      if (outputDesc->mFlags & AUDIO_OUTPUT_FLAG_DEEP_BUFFER) {
          return true;
      }
    }
    return false;
}

bool AudioPolicyManagerCustomImpl::isMMAPActive(void)
{
    for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
      sp<SwAudioOutputDescriptor> outputDesc = mAudioPolicyManager->mOutputs.valueAt(i);
      if (outputDesc->mFlags & AUDIO_OUTPUT_FLAG_MMAP_NOIRQ) {
          return outputDesc->isActive();
      }
    }
    return false;
}

audio_io_handle_t AudioPolicyManagerCustomImpl::getVOIPRxOutput(void)
{
    for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
      sp<SwAudioOutputDescriptor> outputDesc = mAudioPolicyManager->mOutputs.valueAt(i);
      if (outputDesc->mFlags & AUDIO_OUTPUT_FLAG_VOIP_RX) {
          ALOGV("getVOIPRxOutput, outputDesc->mIoHandle 0x%x", outputDesc->mIoHandle);
          return outputDesc->mIoHandle;
      }
    }
    ALOGV("getVOIPRxOutput, mPrimaryOutput->mIoHandle 0x%x", mAudioPolicyManager->mPrimaryOutput->mIoHandle);
    return mAudioPolicyManager->mPrimaryOutput->mIoHandle;
}

status_t AudioPolicyManagerCustomImpl::aaudio_invalidateStreamForInCallFromSetPhoneState(audio_mode_t state, int oldState)
{
    if (oldState != AUDIO_MODE_IN_CALL && state == AUDIO_MODE_IN_CALL) {
        for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
            sp<SwAudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.valueAt(i);
            if (desc->mFlags & AUDIO_OUTPUT_FLAG_MMAP_NOIRQ) {
                for (int stream = 0; stream < AUDIO_STREAM_FOR_POLICY_CNT; stream++) {
                    if (desc->isActive(mAudioPolicyManager->toVolumeSource((audio_stream_type_t)stream))) {
                        ALOGD("Invalidate MMAP track, since entering incall mode");
                        mAudioPolicyManager->mpClientInterface->invalidateStream((audio_stream_type_t)stream);
                    }
                }
            }
        }

        for (const auto& activeInput : mAudioPolicyManager->mInputs.getActiveInputs()) {
            if (activeInput->mProfile->getFlags() & AUDIO_INPUT_FLAG_MMAP_NOIRQ) {
                MTK_ALOGI("setPhoneState() close input of AAudio");
                mAudioPolicyManager->closeInput(activeInput->mIoHandle);
            }
        }
    }

    return NO_ERROR;
}

bool AudioPolicyManagerCustomImpl::aaudio_conidtionCheck(aaudio_cond_t cond, audio_input_flags_t inflags, audio_output_flags_t outflags, audio_mode_t state)
{
    bool value = false;
    switch(cond) {
    case AAUDIO_COND_GET_OUTPUT_FOR_DEVICE:
        if ((outflags & (AUDIO_OUTPUT_FLAG_MMAP_NOIRQ)) != 0 && (state == AUDIO_MODE_IN_CALL)) {
            value = true;
            ALOGW("Not support MMAP mode at InCall mode");
        }
        break;
    case AAUDIO_COND_GET_INPUT_FOR_ATTR:
        if (state == AUDIO_MODE_IN_CALL) {
            value = true;
            ALOGW("getInputForAttr() unsupport AAudio at mode AUDIO_MODE_IN_CALL");
        }
        break;
    case AAUDIO_COND_GET_INPUT_FOR_DEVICE:
        if ((inflags & (AUDIO_INPUT_FLAG_MMAP_NOIRQ)) != 0 && (state == AUDIO_MODE_IN_CALL)) {
            value = true;
            ALOGW("getInputForDevice() unsupport AAudio at mode AUDIO_MODE_IN_CALL");
        }
        break;
    default:
        ALOGE("%s error cond %d", __FUNCTION__, cond);
        break;
    }
    return value;
}

status_t AudioPolicyManagerCustomImpl::aaudio_invalidateMMAPStream(void)
{
    for (size_t i = 0; i < mAudioPolicyManager->mOutputs.size(); i++) {
        sp<SwAudioOutputDescriptor> desc = mAudioPolicyManager->mOutputs.valueAt(i);
        if (desc->mFlags & AUDIO_OUTPUT_FLAG_MMAP_NOIRQ) {
            for (int stream = 0; stream < AUDIO_STREAM_FOR_POLICY_CNT; stream++) {
                if (desc->isActive(mAudioPolicyManager->toVolumeSource((audio_stream_type_t)stream))) {
                    ALOGD("Invalidate MMAP track");
                    mAudioPolicyManager->mpClientInterface->invalidateStream((audio_stream_type_t)stream);
                }
            }
        }
    }
    return NO_ERROR;
}

status_t AudioPolicyManagerCustomImpl::aaudio_policyForceReplaceSampleRate(  sp<SwAudioOutputDescriptor> desc) {
    /* For update music stream with default sampling rate 48000 */
    AudioParameter param;
    if (desc->mFlags & AUDIO_OUTPUT_FLAG_MMAP_NOIRQ) {
        if(mHifiState != 0 && mPolicySampleRate != 48000) {
            MTK_ALOGV("%s() , set Policy sampling rate(48000)", __FUNCTION__);
            mSampleRate_Policy = SampleRate_ForceReplace;
            policyForceReplace(AUDIO_STREAM_DEFAULT, 48000, 1);
            mAudioPolicyManager->mpClientInterface->setParameters(AUDIO_IO_HANDLE_NONE, param.toString(), 0);
        }
    }
    return NO_ERROR;
}

audio_io_handle_t  AudioPolicyManagerCustomImpl::multipleRecord_policySelectInput(sp<IOProfile>& profile,
                                                                                  bool isSoundTrigger,
                                                                                  audio_session_t session,
                                                                                  const audio_config_base_t *config,
                                                                                  audio_input_flags_t flags,
                                                                                  bool forceOpen) {
    Vector<sp <AudioInputDescriptor> > sameProfileInputs;
    Vector<sp <AudioInputDescriptor> > withSTProfileInputs;
    Vector<sp <AudioInputDescriptor> > nonSTProfileInputs;
    Vector<sp <AudioInputDescriptor> > activeNonIdleOnNonSTProfileInputs;
    Vector<sp <AudioInputDescriptor> > activeNonIdleOnSTProfileInputs;
    Vector<sp <AudioInputDescriptor> > sameSessionSTProfileInputs;

    for (size_t i = 0; i < mAudioPolicyManager->mInputs.size(); i++) {
        sp <AudioInputDescriptor> desc = mAudioPolicyManager->mInputs.valueAt(i);
        if (desc->mProfile == profile) {
            sameProfileInputs.add(desc);
            RecordClientVector clients = desc->clientsList();
            bool STCounted = false;
            bool nonSTCounted = false;
            bool activeNonIdleOnST = false;
            bool activeNonIdleOnNonST = false;
            bool sameSessionST = false;
            for (const auto& client : clients) {
                if (client->isSoundTrigger()) {
                    if (!STCounted) {
                        withSTProfileInputs.add(desc);
                        STCounted =  true;
                    }
                    if (!sameSessionST && session == client->session()) {
                        sameSessionSTProfileInputs.add(desc);
                        sameSessionST= true;
                    }
                    if (!activeNonIdleOnST && client->active() && client->appState() != APP_STATE_IDLE) {
                        activeNonIdleOnST = true;
                    }
                } else {
                    if (!nonSTCounted) {
                        nonSTProfileInputs.add(desc);
                        nonSTCounted = true;
                    }
                    if (!activeNonIdleOnNonST && client->active() && client->appState() != APP_STATE_IDLE) {
                        activeNonIdleOnNonST = true;
                    }
                }
            }
            if (activeNonIdleOnST) {
                activeNonIdleOnSTProfileInputs.add(desc);
            }
            if (activeNonIdleOnNonST) {
                activeNonIdleOnNonSTProfileInputs.add(desc);
            }
        }
    }

    if (isSoundTrigger) {
        if (sameSessionSTProfileInputs.size()) {
            ALOGW("ReplaceUsing sameSessionSTProfileInputs size=%zu", sameSessionSTProfileInputs.size());
            return sameSessionSTProfileInputs[0]->mIoHandle;
        } else if (activeNonIdleOnSTProfileInputs.size()) {
            ALOGW("ReplaceUsing activeNonIdleOnSTProfileInputs size=%zu", activeNonIdleOnSTProfileInputs.size());
            return activeNonIdleOnSTProfileInputs[0]->mIoHandle;
        }
    } else {
               if (activeNonIdleOnNonSTProfileInputs.size()) {
                      bool reuseInputFlag = true;
                      uint32_t curStreamIn_sample_rate = 0;
                      if (AudioSystem::getSamplingRate(activeNonIdleOnNonSTProfileInputs[0]->getId(), &curStreamIn_sample_rate) != NO_ERROR) {
                             curStreamIn_sample_rate = activeNonIdleOnNonSTProfileInputs[0]->mSamplingRate;
                         }
                      if (strncmp(profile->getTagName().string(), "primary input", strlen("primary input")) == 0) {
                             if ((!(activeNonIdleOnNonSTProfileInputs[0]->flagsToOpen & AUDIO_INPUT_FLAG_FAST))&& (flags & AUDIO_INPUT_FLAG_FAST)) {
                                  reuseInputFlag = false;
                             } else if (config->sample_rate > curStreamIn_sample_rate) {
                                  reuseInputFlag = false;
                             } else if (forceOpen) {
                                  reuseInputFlag = false;
                             } else if (config->sample_rate == curStreamIn_sample_rate) {
                                  if (audio_channel_count_from_in_mask(config->channel_mask) > audio_channel_count_from_in_mask(activeNonIdleOnNonSTProfileInputs[0]->mChannelMask)) {
                                  reuseInputFlag = false;
                             }
                         }
                 }
                ALOGD("multipleRecord New flags 0x%x rate %d, Old flags 0x%x curStreamRate %d, reuseInputFlag %d forceOpen %d",
                flags, config->sample_rate, activeNonIdleOnNonSTProfileInputs[0]->flagsToOpen, curStreamIn_sample_rate, reuseInputFlag, forceOpen);
            if (reuseInputFlag) {
                ALOGW("Reuse activeNonIdleOnNonSTProfileInputs size=%zu inputId=%d", activeNonIdleOnNonSTProfileInputs.size(), activeNonIdleOnNonSTProfileInputs[0]->mIoHandle);
                return activeNonIdleOnNonSTProfileInputs[0]->mIoHandle;
            }
        }
    }

    sp<AudioInputDescriptor> CloseinputDesc = NULL;
    if (isSoundTrigger) {
        if (withSTProfileInputs.size()) {
            ALOGW("%s #%d", __FUNCTION__, __LINE__);
            CloseinputDesc = withSTProfileInputs[0];
        } else if (nonSTProfileInputs.size()) {
            ALOGW("%s #%d", __FUNCTION__, __LINE__);
            CloseinputDesc = nonSTProfileInputs[0];
        } else {
            if (sameProfileInputs.size()) {
                ALOGW("%s #%d", __FUNCTION__, __LINE__);
                CloseinputDesc = sameProfileInputs[0];
            } else {
                ALOGE("isSoundTrigger %d", isSoundTrigger);
            }
        }
    } else {
        if (nonSTProfileInputs.size()) {
            uint32_t curStreamIn_sample_rate = 0;
            if (AudioSystem::getSamplingRate(nonSTProfileInputs[0]->getId(), &curStreamIn_sample_rate) != NO_ERROR) {
                curStreamIn_sample_rate = nonSTProfileInputs[0]->mSamplingRate;
            }
            // ALPS04927537
            ALOGD("multipleRecord nonSTProfileInputs size=%zu curInputFlag 0x%x reqFlag 0x%x curStreamRate %d reqRate %d", nonSTProfileInputs.size(),
                nonSTProfileInputs[0]->flagsToOpen, flags, curStreamIn_sample_rate, config->sample_rate);

            if ((strncmp(profile->getTagName().string(), "primary input", strlen("primary input")) == 0)) {
                    if ((nonSTProfileInputs[0]->flagsToOpen & AUDIO_INPUT_FLAG_FAST) && !(flags & AUDIO_INPUT_FLAG_FAST)) {
                    ALOGD("multipleRecord current is fast, request is formal, continue to use fast");
                    return nonSTProfileInputs[0]->mIoHandle;
                } else if ((!(nonSTProfileInputs[0]->flagsToOpen & AUDIO_INPUT_FLAG_FAST) && (flags & AUDIO_INPUT_FLAG_FAST))) {
                    ALOGD("multipleRecord close normal, and than open fast");
                } else if (forceOpen && curStreamIn_sample_rate < 48000) {
                    ALOGD("multipleRecord close general record, and then open special record (Possible Barge-In request 48K)");
                } else if ((config->sample_rate == curStreamIn_sample_rate) && (audio_channel_count_from_in_mask(config->channel_mask) > audio_channel_count_from_in_mask(nonSTProfileInputs[0]->mChannelMask))) {
                    ALOGD("multipleRecord same sampling rate, close less channel record, open more channel record");
                } else if (config->sample_rate <= curStreamIn_sample_rate) {
                    ALOGD("multipleRecord Reuse Rate inputId=%d", nonSTProfileInputs[0]->mIoHandle);
                    return nonSTProfileInputs[0]->mIoHandle;
                }
            }
            ALOGW("%s #%d", __FUNCTION__, __LINE__);
            CloseinputDesc = nonSTProfileInputs[0];
        } else if (withSTProfileInputs.size()) {
            ALOGW("%s #%d", __FUNCTION__, __LINE__);
            CloseinputDesc = withSTProfileInputs[0];
        } else {
            if (sameProfileInputs.size()) {
                ALOGW("%s #%d", __FUNCTION__, __LINE__);
                CloseinputDesc = sameProfileInputs[0];
            } else {
                ALOGE("isSoundTrigger %d", isSoundTrigger);
            }
        }
    }
        if (CloseinputDesc != NULL) {
#if 0
        size_t framecount = 0;
        uint32_t mWaitMs = 0;
        status_t getFrameCountStatus = AudioSystem::getFrameCount(CloseinputDesc->mIoHandle, &framecount);
        if (getFrameCountStatus == NO_ERROR) {
            ALOGD("closeInputId [%d], framecount [%zu] samplingRate [%d]", CloseinputDesc->mIoHandle, framecount, CloseinputDesc->mSamplingRate);
            mWaitMs = (framecount * 1000 * 2)/CloseinputDesc->mSamplingRate;    // Sleep 2*framecout to wait to close stream in
        } else {
            ALOGE("closeInputId [%d] getFrameCountStatus [%d]", CloseinputDesc->mIoHandle, getFrameCountStatus);
        }
        mAudioPolicyManager->closeInput(CloseinputDesc->mIoHandle);
        if (mWaitMs) {
            usleep(mWaitMs * 1000);
            ALOGD("%s sleep %d ms", __FUNCTION__, mWaitMs);
        }
#else
        mAudioPolicyManager->closeInput(CloseinputDesc->mIoHandle);
#endif
    } else {
        ALOGE("%s CloseinputDesc = NULL", __FUNCTION__);
    }
    return AUDIO_IO_HANDLE_NONE;
}
};
