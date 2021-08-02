#define LOG_TAG "AudioALSAHardwareResourceManager"

#include "AudioALSAHardwareResourceManager.h"
#include "AudioALSACodecDeviceOutEarphonePMIC.h"

#include <utils/threads.h>

#include "AudioType.h"
#include <AudioLock.h>
#include "AudioAssert.h"

#include "AudioALSADriverUtility.h"
#include "audio_custom_exp.h"
#include "AudioCustParamClient.h"
#include "AudioDeviceInt.h"
#include "AudioALSADeviceConfigManager.h"

#include "WCNChipController.h"

static const char* PROPERTY_KEY_EXTDAC = "vendor.af.resouce.extdac_support";

#define VENDOR_MIXER_XML_PATH "/vendor/etc/mixer_paths.xml"

#define A2DP_DEFAULT_LANTENCY (100)

namespace android
{

static const char *SIDEGEN[] =
{
    "AFE_SGEN_I0I1",   "AFE_SGEN_I2",     "AFE_SGEN_I3I4",   "AFE_SGEN_I5I6",
    "AFE_SGEN_I7I8",   "AFE_SGEN_I9",     "AFE_SGEN_I10I11", "AFE_SGEN_I12I13",
    "AFE_SGEN_I14",    "AFE_SGEN_I15I16", "AFE_SGEN_I17I18", "AFE_SGEN_I19I20",
    "AFE_SGEN_I21I22", "AFE_SGEN_O0O1",   "AFE_SGEN_O2",     "AFE_SGEN_O3O4",
    "AFE_SGEN_O5O6",   "AFE_SGEN_O7O8",   "AFE_SGEN_O9O10",  "AFE_SGEN_O11",
    "AFE_SGEN_O12",    "AFE_SGEN_O13O14", "AFE_SGEN_O15O16", "AFE_SGEN_O17O18",
    "AFE_SGEN_O19O20", "AFE_SGEN_O21O22", "AFE_SGEN_O23O24", "AFE_SGEN_OFF",
    "AFE_SGEN_O3", "AFE_SGEN_O4"
};

static const char *SIDEGEN_SAMEPLRATE[] = {
    "AFE_SGEN_8K", "AFE_SGEN_11K", "AFE_SGEN_12K", "AFE_SGEN_16K", "AFE_SGEN_22K",
    "AFE_SGEN_24K", "AFE_SGEN_32K", "AFE_SGEN_44K", "AFE_SGEN_48K"
};


AudioALSAHardwareResourceManager *AudioALSAHardwareResourceManager::mAudioALSAHardwareResourceManager = NULL;

const char* AudioALSAHardwareResourceManager::mAudioRouteDevice[ROUTE_DEVICE_NUM] =
{
    [ROUTE_DEVICE_OUT_RECEIVER] = "receiver",
    [ROUTE_DEVICE_OUT_SPEAKER] = "speaker",
    [ROUTE_DEVICE_OUT_HEADPHONE] = "headphone",
    [ROUTE_DEVICE_OUT_SPEAKER_AND_HEADPHONE] = "speaker-and-headphone",
    [ROUTE_DEVICE_IN_BUILTIN_MIC] = "builtin-mic",
    [ROUTE_DEVICE_IN_BACK_MIC] = "back-mic",
    [ROUTE_DEVICE_IN_HEADSET_MIC] = "headset-mic",
};

const char* AudioALSAHardwareResourceManager::mAudioRoutePath[ROUTE_PATH_NUM] =
{
    [ROUTE_NORMARL_PLAYBACK_RECEIVER] = "normal-playback receiver",
    [ROUTE_NORMARL_PLAYBACK_SPEAKER] = "normal-playback speaker",
    [ROUTE_NORMARL_PLAYBACK_HEADPHONE] = "normal-playback headphone",
    [ROUTE_NORMARL_PLAYBACK_SPEAKER_AND_HEADPHONE] = "normal-playback speaker-and-headphone",
    [ROUTE_NORMARL_PLAYBACK_BLUETOOTH_SCO] = "normal-playback bt-sco",
    [ROUTE_NORMARL_PLAYBACK_FM] = "normal-playback fm",
    [ROUTE_LOW_LATENCY_PLAYBACK_RECEIVER] = "low-latency-playback receiver",
    [ROUTE_LOW_LATENCY_PLAYBACK_SPEAKER] = "low-latency-playback speaker",
    [ROUTE_LOW_LATENCY_PLAYBACK_HEADPHONE] = "low-latency-playback headphone",
    [ROUTE_LOW_LATENCY_PLAYBACK_SPEAKER_AND_HEADPHONE] = "low-latency-playback speaker-and-headphone",
};

AudioALSAHardwareResourceManager *AudioALSAHardwareResourceManager::getInstance()
{
    static AudioLock mGetInstanceLock;
    AL_AUTOLOCK(mGetInstanceLock);

    if (mAudioALSAHardwareResourceManager == NULL)
    {
        mAudioALSAHardwareResourceManager = new AudioALSAHardwareResourceManager();
    }
    ASSERT(mAudioALSAHardwareResourceManager != NULL);
    return mAudioALSAHardwareResourceManager;
}

status_t AudioALSAHardwareResourceManager::ResetDevice(void)
{
    return NO_ERROR;
}

AudioALSAHardwareResourceManager::AudioALSAHardwareResourceManager() :
    mMixer(AudioALSADriverUtility::getInstance()->getMixer()),
#ifdef SMART_PA_SUPPORT
    mSmartPaController(AudioSmartPaController::getInstance()),
#endif
    mPcmDL(NULL),
    mDeviceConfigManager(NULL),
    mOutputDevices(AUDIO_DEVICE_NONE),
    mInputDevice(AUDIO_DEVICE_NONE),
    mOutputDeviceSampleRate(44100),
    mInputDeviceSampleRate(48000),
    mIsChangingInputDevice(false),
    mStartOutputDevicesCount(0),
    mStartInputDeviceCount(0),
    mMicInverse(false),
    mBuiltInMicSpecificType(BUILTIN_MIC_DEFAULT),
    mNumHSPole(4),
    mHeadchange(false),
    mSpkPcmOut(NULL),
    mAudioRoute(NULL)
{
    ALOGD("%s()", __FUNCTION__);
    init();
    AudioALSACodecDeviceOutEarphonePMIC::getInstance();
    memset((void *)&mSpkPcmConfig, 0, sizeof(mSpkPcmConfig));
}


AudioALSAHardwareResourceManager::~AudioALSAHardwareResourceManager()
{
    ALOGD("%s()", __FUNCTION__);
    if (mAudioRoute)
        audio_route_free(mAudioRoute);
}

/**
 * output devices
 */
status_t AudioALSAHardwareResourceManager::setOutputDevice(const audio_devices_t new_devices, const uint32_t sample_rate)
{
    ALOGD("+%s(), new_devices = 0x%x, mStartOutputDevicesCount = %u", __FUNCTION__, new_devices, mStartOutputDevicesCount);

    ASSERT(mStartOutputDevicesCount == 0);

    mOutputDevices = new_devices;
    mOutputDeviceSampleRate = sample_rate;

    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::startOutputDevice(const audio_devices_t new_devices, const uint32_t SampleRate)
{
    ALOGD("+%s(), new_devices = 0x%x, mOutputDevices = 0x%x, mStartOutputDevicesCount = %u SampleRate = %d",
          __FUNCTION__, new_devices, mOutputDevices, mStartOutputDevicesCount, SampleRate);

    AL_AUTOLOCK(mLock);

    if (new_devices == mOutputDevices)
    {
        // don't need to do anything
    }
    else if(AUDIO_DEVICE_NONE != mOutputDevices)
    {
        changeOutputDevice_l(new_devices, SampleRate);
    }
    else
    {
        startOutputDevice_l(new_devices, SampleRate);
    }

    mStartOutputDevicesCount++;

    ALOGD("-%s(), mOutputDevices = 0x%x, mStartOutputDevicesCount = %u", __FUNCTION__, mOutputDevices, mStartOutputDevicesCount);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::stopOutputDevice()
{
    ALOGD("+%s(), mOutputDevices = 0x%x, mStartOutputDevicesCount = %u", __FUNCTION__, mOutputDevices, mStartOutputDevicesCount);

    AL_AUTOLOCK(mLock);

    if (mStartOutputDevicesCount > 1) {
        // don't need to do anything
    } else {
        if (mStartOutputDevicesCount == 1 && mOutputDevices != AUDIO_DEVICE_NONE) {
            stopOutputDevice_l();
        } else {
          //  ASSERT(0);
            if (mStartOutputDevicesCount < 0) {
                mStartOutputDevicesCount = 0;
            }
        }
    }

    if (mStartOutputDevicesCount > 0) {
        mStartOutputDevicesCount--;
    }

    ALOGD("-%s(), mOutputDevices = 0x%x, mStartOutputDevicesCount = %u", __FUNCTION__, mOutputDevices, mStartOutputDevicesCount);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::changeOutputDevice(const audio_devices_t new_devices)
{
    ALOGD("+%s(), mOutputDevices: 0x%x => 0x%x", __FUNCTION__, mOutputDevices, new_devices);

    AL_AUTOLOCK(mLock);
    changeOutputDevice_l(new_devices, mOutputDeviceSampleRate);

    ALOGD("-%s(), mOutputDevices: 0x%x", __FUNCTION__, mOutputDevices);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::startOutputDevice_l(const audio_devices_t new_devices, const uint32_t SampleRate)
{
    mOutputDevices = new_devices;
    mOutputDeviceSampleRate = SampleRate;

    if (new_devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
        new_devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE))
    {
        OpenHeadphoneSpeakerPath(SampleRate);
    }
    else if (new_devices == AUDIO_DEVICE_OUT_EARPIECE)
    {
        OpenReceiverPath(SampleRate);
    }
    else if (new_devices == AUDIO_DEVICE_OUT_SPEAKER)
    {
        OpenSpeakerPath(SampleRate);
    }
    else if (new_devices == AUDIO_DEVICE_OUT_WIRED_HEADSET ||
             new_devices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE)
    {
        OpenHeadphonePath(SampleRate);
    }

    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::stopOutputDevice_l()
{
    if (mOutputDevices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
        mOutputDevices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE))
    {
        CloseHeadphoneSpeakerPath();
    }
    else if (mOutputDevices == AUDIO_DEVICE_OUT_EARPIECE)
    {
        CloseReceiverPath();
    }
    else if (mOutputDevices == AUDIO_DEVICE_OUT_SPEAKER)
    {
        CloseSpeakerPath();
    }
    else if (mOutputDevices == AUDIO_DEVICE_OUT_WIRED_HEADSET ||
             mOutputDevices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE)
    {
        CloseHeadphonePath();
    }


    mOutputDevices = AUDIO_DEVICE_NONE;

    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::changeOutputDevice_l(const audio_devices_t new_devices, const uint32_t SampleRate)
{
    stopOutputDevice_l();
    startOutputDevice_l(new_devices, SampleRate);

    return NO_ERROR;
}

bool AudioALSAHardwareResourceManager::isSharedOutDevice(audio_devices_t device)
{
    // these devices cannot exist at the same time
    audio_devices_t sharedDevice = AUDIO_DEVICE_OUT_EARPIECE |
                                   AUDIO_DEVICE_OUT_SPEAKER |
                                   AUDIO_DEVICE_OUT_WIRED_HEADSET |
                                   AUDIO_DEVICE_OUT_WIRED_HEADPHONE;

    if ((device & ~sharedDevice) != 0)
    {
        return false;
    }

    return true;
}

/**
 * input devices
 */
status_t AudioALSAHardwareResourceManager::setInputDevice(const audio_devices_t new_devices)
{
    ALOGD("+%s(), new_devices = 0x%x, mStartInputDeviceCount = %u", __FUNCTION__, new_devices, mStartInputDeviceCount);

    return NO_ERROR;
}

void AudioALSAHardwareResourceManager::setMIC1Mode(bool isphonemic __unused)
{
    // Not Supported
}

void AudioALSAHardwareResourceManager::setMIC2Mode(bool isphonemic __unused)
{
    // Not Supported
}

status_t AudioALSAHardwareResourceManager::startInputDevice(const audio_devices_t new_device)
{
    ALOGD("+%s(), new_device = 0x%x, mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, new_device, mInputDevice,mStartInputDeviceCount);

    AL_AUTOLOCK(mLockin);

    if (((mInputDevice & new_device) & ~AUDIO_DEVICE_BIT_IN) != 0)
    {
        ALOGW("%s(), input device already opened", __FUNCTION__);
        if (new_device != AUDIO_DEVICE_IN_SPK_FEED)
        {
            mStartInputDeviceCount++;
        }
        ALOGD("-%s(), mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, mStartInputDeviceCount);
        return NO_ERROR;
    }

    int retval = 0;
    if (new_device == AUDIO_DEVICE_IN_BUILTIN_MIC)
    {
        OpenBuiltInMicPath();
    }
#ifdef MTK_DUAL_MIC_SUPPORT
    else if (new_device == AUDIO_DEVICE_IN_BACK_MIC)
    {
        OpenBackMicPath();
    }
#endif
    else if (new_device == AUDIO_DEVICE_IN_WIRED_HEADSET)
    {
        OpenWiredHeadsetMicPath();
    }
    else
    {
        ALOGW("%s device(0x%x) not handled", __FUNCTION__, new_device);
    }

    mInputDevice |= new_device;
    if (new_device != AUDIO_DEVICE_IN_SPK_FEED)
    {
        mStartInputDeviceCount++;
    }

    ALOGD("-%s(), mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, mStartInputDeviceCount);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::stopInputDevice(const audio_devices_t stop_device)
{
    ALOGD("+%s(), mInputDevice = 0x%x, stop_device = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, stop_device,mStartInputDeviceCount);

    AL_AUTOLOCK(mLockin);

    if (((mInputDevice & stop_device) & ~AUDIO_DEVICE_BIT_IN) == 0)
    {
        ALOGW("%s(), input device not opened yet, do nothing", __FUNCTION__);
        return NO_ERROR;
    }

    if (stop_device != AUDIO_DEVICE_IN_SPK_FEED)
    {
        mStartInputDeviceCount--;
        if (mStartInputDeviceCount > 0)
        {
            ALOGD("-%s(), mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, mStartInputDeviceCount);
            return NO_ERROR;
        }
    }

    int retval = 0;

    if (stop_device == AUDIO_DEVICE_IN_BUILTIN_MIC)
    {
        CloseBuiltInMicPath();
    }
#ifdef MTK_DUAL_MIC_SUPPORT
    else if (stop_device == AUDIO_DEVICE_IN_BACK_MIC)
    {
        CloseBackMicPath();
    }
#endif
    else if (stop_device == AUDIO_DEVICE_IN_WIRED_HEADSET)
    {
        CloseWiredHeadsetMicPath();
    }
    else if (stop_device == AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET)
    {
        // none
    }
    else
    {
        ALOGW("%s device(0x%x) not handled", __FUNCTION__, stop_device);
    }

    mInputDevice &= ((~stop_device) | AUDIO_DEVICE_BIT_IN);
    if (mInputDevice == AUDIO_DEVICE_BIT_IN) { mInputDevice = AUDIO_DEVICE_NONE; }

    ALOGD("-%s(), mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, mStartInputDeviceCount);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::changeInputDevice(const audio_devices_t new_device)
{
    ALOGD("+%s(), mInputDevice: 0x%x => 0x%x", __FUNCTION__, mInputDevice, new_device);

    stopInputDevice(getInputDevice());
    startInputDevice(new_device);

    ALOGD("-%s(), mInputDevice: 0x%x", __FUNCTION__, mInputDevice);
    return NO_ERROR;
}



status_t AudioALSAHardwareResourceManager::setHWGain2DigitalGain(const uint32_t gain)
{
    ALOGD("%s(), gain = 0x%x", __FUNCTION__, gain);

    const uint32_t kMaxAudioHWGainValue = 0x80000;

    if (gain > kMaxAudioHWGainValue)
    {
        ALOGE("%s(), gain(0x%x) > kMaxAudioHWGainValue(0x%x)!! return!!", __FUNCTION__, gain, kMaxAudioHWGainValue);
        return BAD_VALUE;
    }

    int retval = 0;
    if (WCNChipController::GetInstance()->IsFMMergeInterfaceSupported() == true)
    {
        retval = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Audio Mrgrx Volume"), 0, gain);
    }
    else
    {
        retval = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "HW Gain1 Volume"), 0, gain);
    }

    if (retval != 0)
    {
        ALOGE("%s(), retval = %d", __FUNCTION__, retval);
        ASSERT(retval == 0);
    }

    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::setInterruptRate(const uint32_t rate __unused)
{
    // Not Supported
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::setInterruptRate2(const uint32_t rate __unused)
{
    // Not Supported
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::setULInterruptRate(const uint32_t rate __unused)
{
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::openAddaOutput(const uint32_t sample_rate)
{
    ALOGD("+%s(), sample_rate = 0x%x", __FUNCTION__, sample_rate);
    AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());
    struct pcm_config mConfig;

    // DL  setting
    memset(&mConfig, 0, sizeof(mConfig));
    mConfig.channels = 2;
    mConfig.rate = sample_rate;
    mConfig.period_size = 1024;
    mConfig.period_count = 2;
    mConfig.format = PCM_FORMAT_S16_LE;
    mConfig.start_threshold = 0;
    mConfig.stop_threshold = 0;
    mConfig.silence_threshold = 0;

    ASSERT(mPcmDL == NULL);
    int pcmIdx = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl1Meida);
    int cardIdx = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl1Meida);
    mPcmDL = pcm_open(cardIdx, pcmIdx, PCM_OUT, &mConfig);
    ASSERT(mPcmDL != NULL);
    ALOGV("%s(), mPcmDL = %p", __FUNCTION__, mPcmDL);

    pcm_start(mPcmDL);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::closeAddaOutput()
{
    ALOGD("+%s()", __FUNCTION__);
    AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());
    pcm_stop(mPcmDL);
    pcm_close(mPcmDL);
    mPcmDL = NULL;

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::setSgenMode(const sgen_mode_t sgen_mode)
{
    if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_SideGen_Switch"), SIDEGEN[sgen_mode]))
    {
        ALOGE("Error: Audio_SideGen_Switch invalid value");
    }

    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::setSgenSampleRate(const sgen_mode_samplerate_t sample_rate)
{
    ALOGD("setSgenSampleRate = %d", sample_rate);
    if (sample_rate < 0 || sample_rate >= 9)
    {
        ALOGE("%s, sgen_rate is not in range", __FUNCTION__);
        return UNKNOWN_ERROR;
    }

    if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_SideGen_SampleRate"), SIDEGEN_SAMEPLRATE[sample_rate]))
    {
        ALOGE("Error: Audio_SideGen_SampleRate invalid value");
    }

    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::EnableSideToneFilter(const bool enable __unused)
{
    // Not Supported
    return NO_ERROR;
}

bool AudioALSAHardwareResourceManager::GetExtDacPropertyEnable()
{
    return AudioALSADriverUtility::getInstance()->GetPropertyValue(PROPERTY_KEY_EXTDAC);
}

status_t AudioALSAHardwareResourceManager::SetExtDacGpioEnable(bool bEnable __unused)
{
    // Not Supported
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::setMicType(void)
{
    // Not Supported
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::setSPKCurrentSensor(bool bSwitch __unused)
{
    // Not Supported
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::setSPKCurrentSensorPeakDetectorReset(bool bSwitch __unused)
{
    // Not Supported
    return NO_ERROR;
}


audio_devices_t AudioALSAHardwareResourceManager::getOutputDevice()
{
    return mOutputDevices;
}

audio_devices_t AudioALSAHardwareResourceManager::getInputDevice()
{
    return mInputDevice & ((~AUDIO_DEVICE_IN_SPK_FEED) | AUDIO_DEVICE_BIT_IN);
}


status_t AudioALSAHardwareResourceManager::setMicInverse(bool bMicInverse)
{
    ALOGD("%s(), bMicInverse = %d", __FUNCTION__, bMicInverse);
#ifdef MTK_DUAL_MIC_SUPPORT
    mMicInverse = bMicInverse;
#else
    ALOGD("%s(), not support", __FUNCTION__);
#endif
    return NO_ERROR;
}

void AudioALSAHardwareResourceManager::EnableAudBufClk(bool bEanble __unused)
{
    // Not Supported
}


bool AudioALSAHardwareResourceManager::getMicInverse(void)
{
    ALOGD("%s(), mMicInverse = %d", __FUNCTION__, mMicInverse);
    return mMicInverse;
}


void AudioALSAHardwareResourceManager::setAudioDebug(const bool enable __unused)
{
    // Not Supported
}

int AudioALSAHardwareResourceManager::getNonSmartPAType()
{
//fixme
#if 0
    if (mSmartPaController->isSmartPADynamicDetectSupport()) {
        // customer can define how to discriminate non SmartPA type
        return AUDIO_SPK_EXTAMP_LO;
    } else {
#endif
#if defined(USING_EXTAMP_HP)
        return AUDIO_SPK_EXTAMP_HP;
#elif defined(USING_EXTAMP_LO)
        return AUDIO_SPK_EXTAMP_LO;
#else
        return AUDIO_SPK_INTAMP;
#endif
#if 0
    }
#endif
}

status_t AudioALSAHardwareResourceManager::OpenReceiverPath(const uint32_t SampleRate __unused)
{
    applyRouteDevice(ROUTE_DEVICE_OUT_RECEIVER, true);
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::CloseReceiverPath()
{
    applyRouteDevice(ROUTE_DEVICE_OUT_RECEIVER, false);
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::OpenHeadphonePath(const uint32_t SampleRate __unused)
{
    applyRouteDevice(ROUTE_DEVICE_OUT_HEADPHONE, true);
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::CloseHeadphonePath()
{
    applyRouteDevice(ROUTE_DEVICE_OUT_HEADPHONE, false);
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::OpenSpeakerPath(const uint32_t SampleRate __unused)
{
#if defined(SMART_PA_SUPPORT)
    mSmartPaController->speakerOn(SampleRate, mOutputDevices);
#else
    applyRouteDevice(ROUTE_DEVICE_OUT_SPEAKER, true);
#endif
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseSpeakerPath()
{
#if defined(SMART_PA_SUPPORT)
    mSmartPaController->speakerOff();
#else
    applyRouteDevice(ROUTE_DEVICE_OUT_SPEAKER, false);
#endif
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::OpenHeadphoneSpeakerPath(const uint32_t SampleRate __unused)
{
#if defined(SMART_PA_SUPPORT)
    mSmartPaController->speakerOn(SampleRate, mOutputDevices);
    applyRouteDevice(ROUTE_DEVICE_OUT_HEADPHONE, true);
#else
    applyRouteDevice(ROUTE_DEVICE_OUT_SPEAKER_AND_HEADPHONE, true);
#endif
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseHeadphoneSpeakerPath()
{
#if defined(SMART_PA_SUPPORT)
    mSmartPaController->speakerOff();
    applyRouteDevice(ROUTE_DEVICE_OUT_HEADPHONE, false);
#else
    applyRouteDevice(ROUTE_DEVICE_OUT_SPEAKER_AND_HEADPHONE, false);
#endif
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::OpenBuiltInMicPath()
{
    applyRouteDevice(ROUTE_DEVICE_IN_BUILTIN_MIC, true);
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseBuiltInMicPath()
{
    applyRouteDevice(ROUTE_DEVICE_IN_BUILTIN_MIC, false);
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::OpenBackMicPath()
{
    applyRouteDevice(ROUTE_DEVICE_IN_BACK_MIC, true);
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseBackMicPath()
{
    applyRouteDevice(ROUTE_DEVICE_IN_BACK_MIC, false);
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::OpenWiredHeadsetMicPath()
{
    applyRouteDevice(ROUTE_DEVICE_IN_HEADSET_MIC, true);
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseWiredHeadsetMicPath()
{
    applyRouteDevice(ROUTE_DEVICE_IN_HEADSET_MIC, false);
    return NO_ERROR;
}

void AudioALSAHardwareResourceManager::HpImpeDanceDetect(void)
{
    return;
}

status_t AudioALSAHardwareResourceManager::setRoutePath(const uint32_t scenario, const audio_devices_t devices)
{
    switch (scenario)
    {
    case ROUTE_NORMAL_PLAYBACK:
        if (devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
            devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE)) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_SPEAKER_AND_HEADPHONE, true);
        } else if (devices == AUDIO_DEVICE_OUT_EARPIECE) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_RECEIVER, true);
        } else if (devices == AUDIO_DEVICE_OUT_SPEAKER) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_SPEAKER, true);
        } else if (devices == AUDIO_DEVICE_OUT_WIRED_HEADSET ||
                devices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_HEADPHONE, true);
        } else if (devices == AUDIO_DEVICE_OUT_BLUETOOTH_SCO ||
                   devices == AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET ||
                   devices == AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_BLUETOOTH_SCO, true);
        } else if (devices == AUDIO_DEVICE_IN_FM_TUNER) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_FM, true);
        } else {
            ALOGV("%s() scenario(%u) devices(0x%x) not handled", __FUNCTION__, scenario, devices);
        }
        break;
    case ROUTE_LOW_LATENCY_PLAYBACK:
        if (devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
            devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE)) {
            applyRoutePath(ROUTE_LOW_LATENCY_PLAYBACK_SPEAKER_AND_HEADPHONE, true);
        } else if (devices == AUDIO_DEVICE_OUT_EARPIECE) {
            applyRoutePath(ROUTE_LOW_LATENCY_PLAYBACK_RECEIVER, true);
        } else if (devices == AUDIO_DEVICE_OUT_SPEAKER) {
            applyRoutePath(ROUTE_LOW_LATENCY_PLAYBACK_SPEAKER, true);
        } else if (devices == AUDIO_DEVICE_OUT_WIRED_HEADSET ||
                   devices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE) {
            applyRoutePath(ROUTE_LOW_LATENCY_PLAYBACK_HEADPHONE, true);
        } else {
            ALOGV("%s() scenario(%u) devices(0x%x) not handled", __FUNCTION__, scenario, devices);
        }
        break;
    default:
        break;
    }

    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::resetRoutePath(const uint32_t scenario, const audio_devices_t devices)
{
    switch (scenario)
    {
    case ROUTE_NORMAL_PLAYBACK:
        if (devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
            devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE)) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_SPEAKER_AND_HEADPHONE, false);
        } else if (devices == AUDIO_DEVICE_OUT_EARPIECE) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_RECEIVER, false);
        } else if (devices == AUDIO_DEVICE_OUT_SPEAKER) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_SPEAKER, false);
        } else if (devices == AUDIO_DEVICE_OUT_WIRED_HEADSET ||
                    devices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_HEADPHONE, false);
        } else if (devices == AUDIO_DEVICE_OUT_BLUETOOTH_SCO ||
                   devices == AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET ||
                   devices == AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_BLUETOOTH_SCO, false);
        } else if (devices == AUDIO_DEVICE_IN_FM_TUNER) {
            applyRoutePath(ROUTE_NORMARL_PLAYBACK_FM, false);
        } else {
            ALOGV("%s() scenario(%u) devices(0x%x) not handled", __FUNCTION__, scenario, devices);
        }
        break;
    case ROUTE_LOW_LATENCY_PLAYBACK:
        if (devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
            devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE)) {
            applyRoutePath(ROUTE_LOW_LATENCY_PLAYBACK_SPEAKER_AND_HEADPHONE, false);
        } else if (devices == AUDIO_DEVICE_OUT_EARPIECE) {
            applyRoutePath(ROUTE_LOW_LATENCY_PLAYBACK_RECEIVER, false);
        } else if (devices == AUDIO_DEVICE_OUT_SPEAKER) {
            applyRoutePath(ROUTE_LOW_LATENCY_PLAYBACK_SPEAKER, false);
        } else if (devices == AUDIO_DEVICE_OUT_WIRED_HEADSET ||
                   devices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE) {
            applyRoutePath(ROUTE_LOW_LATENCY_PLAYBACK_HEADPHONE, false);
        } else {
            ALOGV("%s() scenario(%u) devices(0x%x) not handled", __FUNCTION__, scenario, devices);
        }
        break;
    default:
        break;
    }

    return NO_ERROR;
}

void AudioALSAHardwareResourceManager::init()
{
    mAudioRoute = audio_route_init(AudioALSADeviceParser::getInstance()->GetCardIndex(), VENDOR_MIXER_XML_PATH);
    if (!mAudioRoute) {
        mAudioRoute = audio_route_init(AudioALSADeviceParser::getInstance()->GetCardIndex(), NULL);
        ALOG_ASSERT(mAudioRoute != NULL, "audio_route_init fail");
    }
}

void AudioALSAHardwareResourceManager::applyRouteDevice(const uint32_t route_device, const bool enable)
{
    if (!mAudioRoute) {
        ALOGW("%s() invalid audio route", __FUNCTION__);
        return;
    }

    ALOGV("%s() %s route %s", __FUNCTION__, (enable ? "apply" : "reset"), mAudioRouteDevice[route_device]);

    if (enable)
        audio_route_apply_and_update_path(mAudioRoute, mAudioRouteDevice[route_device]);
    else
        audio_route_reset_and_update_path(mAudioRoute, mAudioRouteDevice[route_device]);
}

void AudioALSAHardwareResourceManager::applyRoutePath(const uint32_t route_path, const bool enable)
{
    if (!mAudioRoute) {
        ALOGW("%s() invalid audio route", __FUNCTION__);
        return;
    }

    ALOGV("%s() %s route %s", __FUNCTION__, (enable ? "apply" : "reset"), mAudioRoutePath[route_path]);

    if (enable)
        audio_route_apply_and_update_path(mAudioRoute, mAudioRoutePath[route_path]);
    else
        audio_route_reset_and_update_path(mAudioRoute, mAudioRoutePath[route_path]);

}

void AudioALSAHardwareResourceManager::setCodecSampleRate(const uint32_t sample_rate) {
    ALOGD("+%s(), sample_rate = %d", __FUNCTION__, sample_rate);

    int retval = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Codec_ADC_SampleRate"), 0, sample_rate);
    if (retval != 0) {
        ALOGE("%s(), Set mixer Codec_ADC_SampleRate error(), ret = %d!!!", __FUNCTION__, retval);
    }

    retval = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Codec_DAC_SampleRate"), 0, sample_rate);
    if (retval != 0) {
        ALOGE("%s(), Set mixer Codec_DAC_SampleRate error(), ret = %d!!!", __FUNCTION__, retval);
    }

    retval = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Codec_ADC2_SampleRate"), 0, sample_rate);
    if (retval != 0) {
        ALOGE("%s(), Set mixer Codec_ADC2_SampleRate error(), ret = %d!!!", __FUNCTION__, retval);
    }

    ALOGD("-%s()", __FUNCTION__);
}


void AudioALSAHardwareResourceManager::resetA2dpDeviceLatency(void) {
    mA2dpDeviceLatency = A2DP_DEFAULT_LANTENCY;
}

} // end of namespace android
