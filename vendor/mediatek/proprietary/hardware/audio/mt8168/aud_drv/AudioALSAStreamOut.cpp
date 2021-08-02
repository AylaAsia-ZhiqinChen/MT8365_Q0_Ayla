#define LOG_TAG  "AudioALSAStreamOut"
//#define LOG_NDEBUG 0

#include "AudioALSAStreamOut.h"

#include "AudioALSAStreamManager.h"
#include "AudioALSAPlaybackHandlerBase.h"
#include "AudioUtility.h"

#include "AudioALSASampleRateController.h"
#include "AudioALSAFMController.h"

#include <hardware/audio_mtk.h>
#include <inttypes.h>
#include "HDMITxController.h"
#include <media/TypeConverter.h>
#ifdef DOWNLINK_SUPPORT_PERFSERVICE
#include "perfservice_types.h"
#endif


#define MAX_KERNEL_BUFFER_SIZE      (49152)
#define BUFFER_SIZE_PER_ACCESSS     (8192)
#define FRAME_COUNT_MIN_PER_ACCESSS (256)

//#define VERY_VERY_VERBOSE_LOGGING
#ifdef VERY_VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#define TYPE_LOGVV(x, ...) ALOGVV("[%s] " x, TYPE_TO_STRING(mStreamOutType), ##__VA_ARGS__)
#define TYPE_LOGV(x, ...)  ALOGV("[%s] " x, TYPE_TO_STRING(mStreamOutType), ##__VA_ARGS__)
#define TYPE_LOGD(x, ...)  ALOGD("[%s] " x, TYPE_TO_STRING(mStreamOutType), ##__VA_ARGS__)
#define TYPE_LOGW(x, ...)  ALOGW("[%s] " x, TYPE_TO_STRING(mStreamOutType), ##__VA_ARGS__)
#define TYPE_LOGE(x, ...)  ALOGE("[%s] " x, TYPE_TO_STRING(mStreamOutType), ##__VA_ARGS__)


#define ENUM_TO_STRING(enum) #enum
#define TYPE_TO_STRING(type) ((type == STREAM_OUT_PRIMARY) ? "primary" : \
                              (type == STREAM_OUT_LOW_LATENCY) ? "low_latency" : \
                              (type == STREAM_OUT_HDMI_STEREO) ? "hdmi_stereo" : \
                              (type == STREAM_OUT_HDMI_MULTI_CHANNEL) ? "hdmi_direct" : "")

#define DOWNLINK_LOW_LATENCY_CPU_SPEED  (1196000)
#define DOWNLINK_LOW_POWER_CPU_SPEED    (1040000)

namespace android
{

uint32_t AudioALSAStreamOut::mDumpFileNum = 0;

static const audio_format_t       kDefaultOutputSourceFormat      = AUDIO_FORMAT_PCM_16_BIT;
static const audio_channel_mask_t kDefaultOutputSourceChannelMask = AUDIO_CHANNEL_OUT_STEREO;
static const uint32_t             kDefaultOutputSourceSampleRate  = 44100;
static const uint32_t kDefaultOutputBufferFrameCOunt[NUM_STREAM_OUT_TYPE] = {
    [STREAM_OUT_PRIMARY] = 4096,
    [STREAM_OUT_HDMI_STEREO] = 4096,
    [STREAM_OUT_HDMI_MULTI_CHANNEL] = 4096,
    [STREAM_OUT_VOICE_DL] = 128,
    [STREAM_OUT_LOW_LATENCY] = 480,
};

uint32_t AudioALSAStreamOut::mSuspendStreamOutHDMIStereoCount = 0;

AudioALSAStreamOut *AudioALSAStreamOut::mStreamOutHDMIStereo = NULL;

AudioLock AudioALSAStreamOut::mStreamOutsLock;
uint32_t mStreamOutHDMIStereoCount = 0;

AudioALSAStreamOut::AudioALSAStreamOut() :
    mStreamManager(AudioALSAStreamManager::getInstance()),
    mPlaybackHandler(NULL),
    mPCMDumpFile(NULL),
    mLockCount(0),
    mIdentity(0xFFFFFFFF),
    mSuspendCount(0),
    mStandby(true),
    mStreamOutType(STREAM_OUT_PRIMARY),
    mPresentedBytes(0),
    mPresentFrames(0),
    mLowLatencyMode(true),
    mOffload(false),
    mPaused(false),
    mStreamCbk(NULL),
    mCbkCookie(NULL),
    mOffloadVol(0x10000),
    mBufferSizePerAccess(0),
    mInternalSuspend(false),
    mWriteCount(0),
    mLastPresentationFrames(0),
    mLastPresentationValid(false),
    mRenderedBytes(0),
    mLastDspFrames(0)
{
    ALOGD("%s()", __FUNCTION__);

    memset(&mStreamAttributeSource, 0, sizeof(mStreamAttributeSource));
    memset(&mPresentedTime, 0, sizeof(timespec));
    memset(&mLastPresentationTimestamp, 0, sizeof(timespec));
#ifdef DOWNLINK_SUPPORT_PERFSERVICE
    mPerfServiceHandle = -1;
    mScreenOff = false;
    mIsLowPowerEnabled = false;
#endif
}

AudioALSAStreamOut::~AudioALSAStreamOut()
{
    ALOGD("%s()", __FUNCTION__);

    if (!mStandby) {
        ALOGW("%s(), not standby, mStandby %d, mPlaybackHandler %p",
              __FUNCTION__, mStandby, mPlaybackHandler);
        standbyStreamOut(false);
    }

    if (mStreamOutHDMIStereo == this)
    {
        mStreamOutHDMIStereo = NULL;
    }

#ifdef DOWNLINK_SUPPORT_PERFSERVICE
    if (mPerfServiceHandle >= 0)
        unregPerfServiceScn(&mPerfServiceHandle);
#endif

}

status_t AudioALSAStreamOut::set(
    uint32_t devices,
    int *format,
    uint32_t *channels,
    uint32_t *sampleRate,
    status_t *status,
    uint32_t flags)
{
    ALOGD("%s(), devices = 0x%x, format = 0x%x, channels = 0x%x, sampleRate = %d, flags = 0x%x",
          __FUNCTION__, devices, *format, *channels, *sampleRate, flags);

    AL_AUTOLOCK(mLock);

    *status = NO_ERROR;

    // device
    mStreamAttributeSource.output_devices = static_cast<audio_devices_t>(devices);
    mStreamAttributeSource.policyDevice = mStreamAttributeSource.output_devices;

    // check format
    if (*format == AUDIO_FORMAT_PCM_16_BIT ||
        *format == AUDIO_FORMAT_PCM_8_24_BIT ||
        *format == AUDIO_FORMAT_PCM_32_BIT) {
        mStreamAttributeSource.audio_format = static_cast<audio_format_t>(*format);
    } else if (*format == AUDIO_FORMAT_MP3) {
        ALOGD("%s(), format mp3", __FUNCTION__);
        mStreamAttributeSource.audio_format = static_cast<audio_format_t>(*format);
        mStreamAttributeSource.audio_offload_format = *format;
    } else if (*format == AUDIO_FORMAT_AAC_LC) {
        ALOGD("%s(), format aac", __FUNCTION__);
        mStreamAttributeSource.audio_format = static_cast<audio_format_t>(*format);
        mStreamAttributeSource.audio_offload_format = *format;
    } else {
        ALOGE("%s(), wrong format 0x%x, use 0x%x instead.", __FUNCTION__, *format, kDefaultOutputSourceFormat);

        *format = kDefaultOutputSourceFormat;
        *status = BAD_VALUE;
    }

    // check channel mask
    if (mStreamAttributeSource.output_devices == AUDIO_DEVICE_OUT_AUX_DIGITAL) { // HDMI
        if (*channels == AUDIO_CHANNEL_OUT_STEREO) {
            mStreamOutType = STREAM_OUT_HDMI_STEREO;

            mStreamAttributeSource.audio_channel_mask = *channels;
            mStreamAttributeSource.num_channels = popcount(*channels);

            mStreamOutHDMIStereo = this;
            mStreamOutHDMIStereoCount++;
            ALOGD("%s(), mStreamOutHDMIStereoCount =%d", __FUNCTION__, mStreamOutHDMIStereoCount);
        } else if (*channels == AUDIO_CHANNEL_OUT_5POINT1 ||
                   *channels == AUDIO_CHANNEL_OUT_7POINT1) {
            mStreamOutType = STREAM_OUT_HDMI_MULTI_CHANNEL;

            mStreamAttributeSource.audio_channel_mask = *channels;
            mStreamAttributeSource.num_channels = popcount(*channels);
        } else {
            ALOGE("%s(), wrong channels 0x%x, use 0x%x instead.", __FUNCTION__, *channels, kDefaultOutputSourceChannelMask);

            *channels = kDefaultOutputSourceChannelMask;
            *status = BAD_VALUE;
        }
    } else if (devices == AUDIO_DEVICE_OUT_SPEAKER_SAFE) { // Primary
        mStreamOutType = STREAM_OUT_VOICE_DL;
        mStreamAttributeSource.audio_channel_mask = *channels;
        mStreamAttributeSource.num_channels = popcount(*channels);
    } else if (*channels == kDefaultOutputSourceChannelMask || *channels == AUDIO_CHANNEL_OUT_MONO) { // Primary
        mStreamAttributeSource.audio_channel_mask = *channels;
        mStreamAttributeSource.num_channels = popcount(*channels);
    } else {
        ALOGE("%s(), wrong channels 0x%x, use 0x%x instead.", __FUNCTION__, *channels, kDefaultOutputSourceChannelMask);

        *channels = kDefaultOutputSourceChannelMask;
        *status = BAD_VALUE;
    }

    // check sample rate
    if (SampleRateSupport(*sampleRate) == true) {
        if ((mStreamAttributeSource.num_channels == 2) && (mStreamAttributeSource.output_devices == AUDIO_DEVICE_OUT_AUX_DIGITAL)) {
            mStreamAttributeSource.sample_rate = 48000;
        } else {
            mStreamAttributeSource.sample_rate = *sampleRate;
        }
        if ((mStreamOutType == STREAM_OUT_PRIMARY || mStreamOutType == STREAM_OUT_VOICE_DL) && ((flags & AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD) == 0)) {
            AudioALSASampleRateController::getInstance()->setPrimaryStreamOutSampleRate(*sampleRate);
        }
    } else {
        ALOGE("%s(), wrong sampleRate %d, use %d instead.", __FUNCTION__, *sampleRate, kDefaultOutputSourceSampleRate);

        *sampleRate = kDefaultOutputSourceSampleRate;
        *status = BAD_VALUE;
    }

    mStreamAttributeSource.mAudioOutputFlags = (audio_output_flags_t)flags;
    collectPlatformOutputFlags(mStreamAttributeSource.mAudioOutputFlags);

    if (mStreamAttributeSource.mAudioOutputFlags & AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD) {
        mStreamAttributeSource.usePolicyDevice = true;
    }

#ifdef MTK_HYBRID_NLE_SUPPORT
    if (isIsolatedDeepBuffer(mStreamAttributeSource.mAudioOutputFlags)) {
        AudioALSAHyBridNLEManager::setSupportRunNLEHandler(PLAYBACK_HANDLER_DEEP_BUFFER);
    }
#endif

    // debug for PowerHAL
    char value[PROPERTY_VALUE_MAX];
    if (mStreamAttributeSource.mAudioOutputFlags & AUDIO_OUTPUT_FLAG_FAST) {
        (void) property_get("vendor.audio.powerhal.latency.dl", value, "1");
    } else {
        (void) property_get("vendor.audio.powerhal.power.dl", value, "1");
    }
    int debuggable = atoi(value);
    mStreamAttributeSource.mPowerHalEnable = debuggable ? true : false;


    // audio low latency param - playback - hal buffer size
    setBufferSize();

    ALOGD("%s(), mStreamAttributeSource.latency %d, mStreamAttributeSource.buffer_size =%d, channels %d.", __FUNCTION__, mStreamAttributeSource.latency, 
mStreamAttributeSource.buffer_size, mStreamAttributeSource.num_channels);

    return *status;
}

uint32_t AudioALSAStreamOut::sampleRate() const
{
    TYPE_LOGVV("%s(), return %d", __FUNCTION__, mStreamAttributeSource.sample_rate);
    return mStreamAttributeSource.sample_rate;
}

size_t AudioALSAStreamOut::bufferSize() const
{
    TYPE_LOGVV("%s(), return %zu", __FUNCTION__, mBufferSizePerAccess);
    return mBufferSizePerAccess;
}

uint32_t AudioALSAStreamOut::channels() const
{
    TYPE_LOGVV("%s(), return 0x%x", __FUNCTION__, mStreamAttributeSource.audio_channel_mask);
    return mStreamAttributeSource.audio_channel_mask;
}

int AudioALSAStreamOut::format() const
{
    TYPE_LOGVV("%s(), return 0x%x", __FUNCTION__, mStreamAttributeSource.audio_format);
    return mStreamAttributeSource.audio_format;
}

uint32_t AudioALSAStreamOut::frameSize() const
{
    if (isHdmiOutNeedIECWrap())
        return popcount(AUDIO_CHANNEL_OUT_STEREO) * sizeof(int16_t);
    else
        return popcount(channels()) * (audio_bytes_per_sample((audio_format_t)format()));
}

uint32_t AudioALSAStreamOut::latency()
{
    uint32_t latency = 0;

    int oldCount;
    oldCount = android_atomic_inc(&mLockCount);
    AL_AUTOLOCK(mLock);
    oldCount = android_atomic_dec(&mLockCount);
    if (mPlaybackHandler == NULL || mStandby)
    {
        latency = mStreamAttributeSource.latency;
    }
    else
    {
        const stream_attribute_t *pStreamAttributeTarget = mPlaybackHandler->getStreamAttributeTarget();
        const uint8_t size_per_frame = pStreamAttributeTarget->num_channels *
                                       audio_bytes_per_sample(pStreamAttributeTarget->audio_format);

        latency = (pStreamAttributeTarget->buffer_size * 1000) / (pStreamAttributeTarget->sample_rate * size_per_frame);

        if(latency==0)
        {
            int latencyPlayHandler = mPlaybackHandler->getLatency();
            if (latencyPlayHandler > 0)
                latency = (uint32_t)latencyPlayHandler;
            else
                latency = mStreamAttributeSource.latency;
        }
    }

    TYPE_LOGVV("%s() return %d", __FUNCTION__, latency);
    return latency;
}

status_t AudioALSAStreamOut::setVolume(float left __unused, float right __unused)
{
    return INVALID_OPERATION;
}

ssize_t AudioALSAStreamOut::write(const void *buffer, size_t bytes)
{
    TYPE_LOGV("%s(), buffer = %p, bytes = %zu", __FUNCTION__, buffer, bytes);

    if (mStandby && mSuspendCount == 0)
    {
        // set volume before open device to avoid pop, and without holding lock.
        mStreamManager->setMasterVolume(mStreamManager->getMasterVolume(), getIdentity());
    }

    /* fast output is RT thread and keep streamout lock for write kernel.
       so other thread can't get streamout lock. if necessary, output will active release CPU. */
    int tryCount = 10;
    while(mLockCount && tryCount--) {
        TYPE_LOGD("%s, free CPU, mLockCount %d, tryCount %d", __FUNCTION__, mLockCount, tryCount);
        usleep(300);
    }

    AL_AUTOLOCK(mSuspendLock);

    size_t outputSize = 0;
    if ((mSuspendCount > 0) ||
        (mStreamManager->isModeInPhoneCall() == true && mStreamOutType != STREAM_OUT_PRIMARY))
    {
        // here to sleep a buffer size latency and return.
        TYPE_LOGV("%s() mSuspendCount = %u", __FUNCTION__, mSuspendCount);
        usleep(latency() * 1000);
        mPresentedBytes += bytes;
        mRenderedBytes += bytes;
        mWriteCount++;
        return bytes;
    }

    /* multi-ch or raw HDMI output has higher priority to standby stereo HDMI output */
    if (isHdmiOut())
        AL_LOCK(mStreamOutsLock);

    AL_AUTOLOCK(mLock);

    status_t status = NO_ERROR;

    /// check open
    if (mStandby == true)
    {
        status = open();
        setScreenState_l();
        if (mPlaybackHandler)
            mPlaybackHandler->setFirstDataWriteFlag(true);
    }
    else
    {
        if (mPlaybackHandler)
            mPlaybackHandler->setFirstDataWriteFlag(false);
    }

    if (isHdmiOut())
        AL_UNLOCK(mStreamOutsLock);

    if (bytes == 0)
        return 0;

    WritePcmDumpData(buffer, bytes);

    if (!mInternalSuspend)
    {
        /// write pcm data
        ASSERT(mPlaybackHandler != NULL);
        if (mPlaybackHandler)
            outputSize = mPlaybackHandler->write(buffer, bytes);
    }
    else
    {
        TYPE_LOGV("%s() internal suspend", __FUNCTION__);
        usleep(dataToDurationUs(bytes));
        outputSize = bytes;
    }

    if (isHdmiOutNeedIECWrap())
    {
        // while invoke SPDIFEncoder, outputSize is the accumulation of raw data but we need the accumulation of IEC61937
        uint64_t bytesWriteKernel = mPlaybackHandler->getBytesWriteKernel();
        mPresentedBytes += bytesWriteKernel;
        mRenderedBytes += bytesWriteKernel;

        mPlaybackHandler->resetBytesWriteKernel();
    }
    else
    {
        mPresentedBytes += outputSize;
        mRenderedBytes += outputSize;
    }

    mWriteCount++;

    if (isHdmiOut()) {
        uint64_t frames;
        struct timespec timestamp;
        getPresentationPosition_l(&frames, &timestamp);
    }

    TYPE_LOGV("%s() outputSize = %zu mPresentedBytes = %" PRIu64,
              __FUNCTION__, outputSize, mPresentedBytes);
    return outputSize;
}

status_t AudioALSAStreamOut::standby(bool halRequest) {
    // standby is used by framework to close stream out.
    ALOGD("%s(), flag %d", __FUNCTION__, mStreamAttributeSource.mAudioOutputFlags);
    status_t status = NO_ERROR;

    mStreamAttributeSource.usePolicyDevice = true;
    status = standbyStreamOut(halRequest);

    return status;
}



status_t AudioALSAStreamOut::standbyStreamOut(bool halRequest) {
    ALOGD("%s(), halRequest %d, mDestroy %d, flag %d", __FUNCTION__, halRequest,
          mDestroy, mStreamAttributeSource.mAudioOutputFlags);
    int oldCount;
    oldCount = android_atomic_inc(&mLockCount);
    AL_AUTOLOCK(mSuspendLock);
    AL_AUTOLOCK(mLock);

    status_t status = NO_ERROR;

    // mmap don't support hal standby
    if ((halRequest == true) && (mDestroy == false) && (mStreamAttributeSource.mAudioOutputFlags &
                                                        AUDIO_OUTPUT_FLAG_MMAP_NOIRQ)) {
        return status;
    }

    /// check close
    if (mStandby == false) {
        if ((halRequest == true) && (mStreamAttributeSource.mAudioOutputFlags & AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD)) {
#if defined(MTK_AUDIO_DSP_RECOVERY_SUPPORT)
            int tryLockRet = LOCK_ALOCK_TRYLOCK(get_adsp_recovery_lock());
            if (get_audio_dsp_recovery_mode() == true) {
                mStreamCbk(STREAM_CBK_EVENT_ERROR, 0, mCbkCookie);
            } else
#endif
            {
                ALOGD("%s(), offload mAudioOutputFlags = %d, reopen by framework", __FUNCTION__, mStreamAttributeSource.mAudioOutputFlags);
                mPlaybackHandler->setOffloadRoutingFlag(true);
                status = close();
            }
#if defined(MTK_AUDIO_DSP_RECOVERY_SUPPORT)
            if (tryLockRet == 0) {
                UNLOCK_ALOCK(get_adsp_recovery_lock());
            }
#endif
        } else {
            status = close();
        }
    }

    oldCount = android_atomic_dec(&mLockCount);
    return status;
}


status_t AudioALSAStreamOut::dump(int fd __unused, const Vector<String16> &args __unused)
{
    TYPE_LOGD("%s()", __FUNCTION__);
    return NO_ERROR;
}

bool AudioALSAStreamOut::SampleRateSupport(uint32_t sampleRate)
{
    if (sampleRate == 8000 || sampleRate == 11025 || sampleRate == 12000 ||
        sampleRate == 16000 || sampleRate == 22050 || sampleRate == 24000 ||
        sampleRate == 32000 || sampleRate == 44100 || sampleRate == 48000 ||
        sampleRate == 88200 || sampleRate == 96000 || sampleRate == 176400 ||
        sampleRate == 192000)
    {
        return true;
    }
    else
    {
        return false;
    }
}

status_t AudioALSAStreamOut::UpdateSampleRate(int sampleRate)
{
    TYPE_LOGD("%s() sampleRate = %d", __FUNCTION__, sampleRate);
    // check sample rate
    if (SampleRateSupport(sampleRate) == true)
    {
        AudioALSASampleRateController::getInstance()->setPrimaryStreamOutSampleRate(sampleRate);
        mStreamAttributeSource.sample_rate = sampleRate;

        setBufferSize();
    }
    else
    {
        TYPE_LOGE("%s(), wrong sampleRate %d, use %d instead.", __FUNCTION__, sampleRate, kDefaultOutputSourceSampleRate);
        sampleRate = kDefaultOutputSourceSampleRate;
    }
    return NO_ERROR;
}

status_t AudioALSAStreamOut::setParameters(const String8 &keyValuePairs)
{
    ALOGD("+%s(): flag %d, %s", __FUNCTION__, mStreamAttributeSource.mAudioOutputFlags, keyValuePairs.string());
    AudioParameter param = AudioParameter(keyValuePairs);

    /// keys
    const String8 keyRouting = String8(AudioParameter::keyRouting);
    const String8 keySampleRate = String8(AudioParameter::keySamplingRate);
    const String8 keyDynamicSampleRate = String8("DynamicSampleRate");
    const String8 keyLowLatencyMode = String8("LowLatencyMode");
    const String8 keyFormat = String8(AudioParameter::keyFormat);

//forced to use basic
#if 1
    const String8 keyRoutingToNone = String8(AUDIO_PARAMETER_KEY_ROUTING_TO_NONE);
    const String8 keyFmDirectControl = String8(AUDIO_PARAMETER_KEY_FM_DIRECT_CONTROL);
#else
    const String8 keyRoutingToNone = String8(AudioParameter::keyRoutingToNone);
    const String8 keyFmDirectControl = String8(AudioParameter::keyFmDirectControl);
#endif
    /// parse key value pairs
    status_t status = NO_ERROR;
    int value = 0;
    String8 value_str;

    /// routing
    if (param.getInt(keyRouting, value) == NO_ERROR)
    {
        param.remove(keyRouting);

        android_atomic_inc(&mLockCount);
        if ((mStreamOutType == STREAM_OUT_PRIMARY) || (mStreamOutType == STREAM_OUT_LOW_LATENCY))
        {
            status = mStreamManager->routingOutputDevice(this, mStreamAttributeSource.output_devices,
                                                         static_cast<audio_devices_t>(value));
        }
        else if ((mStreamOutType == STREAM_OUT_HDMI_STEREO) || (mStreamOutType == STREAM_OUT_HDMI_MULTI_CHANNEL))
        {
            TYPE_LOGV("%s() routing '%d'", __FUNCTION__, value);
        }
        else
        {
            TYPE_LOGW("%s() invalid routing '%d'", __FUNCTION__, value);
            status = INVALID_OPERATION;
        }
        android_atomic_dec(&mLockCount);
    }

    if (param.getInt(keyFmDirectControl, value) == NO_ERROR)
    {
        param.remove(keyFmDirectControl);

        android_atomic_inc(&mLockCount);
        AL_AUTOLOCK(mLock);
        AudioALSAFMController::getInstance()->setUseFmDirectConnectionMode(value?true:false);
        android_atomic_dec(&mLockCount);
    }

    // routing none, for no stream but has device change. e.g. vow path change
    if (param.getInt(keyRoutingToNone, value) == NO_ERROR)
    {
        param.remove(keyRoutingToNone);

        android_atomic_inc(&mLockCount);
        AL_AUTOLOCK(mLock);
        status = mStreamManager->DeviceNoneUpdate();
        android_atomic_dec(&mLockCount);
    }

    if (param.getInt(keySampleRate, value) == NO_ERROR)
    {
        param.remove(keySampleRate);
        android_atomic_inc(&mLockCount);
        AL_AUTOLOCK(mLock);

        if ((mStreamOutType == STREAM_OUT_PRIMARY || mStreamOutType == STREAM_OUT_LOW_LATENCY) &&
            (mPlaybackHandler == NULL))
        {
            UpdateSampleRate(value);
        }
        else
        {
            status = INVALID_OPERATION;
        }
        android_atomic_dec(&mLockCount);
    }

    if (param.getInt(keyDynamicSampleRate, value) == NO_ERROR)
    {
        param.remove(keyDynamicSampleRate);
        android_atomic_inc(&mLockCount);
        AL_AUTOLOCK(mLock);

        if (mStreamOutType == STREAM_OUT_PRIMARY || mStreamOutType == STREAM_OUT_LOW_LATENCY)
        {
            status = NO_ERROR;
        }
        else
        {
            status = INVALID_OPERATION;
        }
        android_atomic_dec(&mLockCount);
    }
   //keyformat
    if (param.getInt(keyFormat, value) == NO_ERROR)
    {
        param.remove(keyFormat);
        android_atomic_inc(&mLockCount);
        AL_AUTOLOCK(mLock);
        ALOGV("%s() Format = %d", __FUNCTION__, value);
        mStreamAttributeSource.audio_format = static_cast<audio_format_t>(value);
        android_atomic_dec(&mLockCount);
    }

#if defined(MTK_AUDIO_GAIN_TABLE) || defined(MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT)
    static String8 keyVolumeStreamType    = String8("volumeStreamType");
    static String8 keyVolumeDevice        = String8("volumeDevice");
    static String8 keyVolumeIndex         = String8("volumeIndex");
    if (param.getInt(keyVolumeStreamType, value) == NO_ERROR)
    {
        int device = 0;
        int index = 0;
        if (param.getInt(keyVolumeDevice, device) == NO_ERROR)
        {
            if (param.getInt(keyVolumeIndex, index) == NO_ERROR)
            {
#ifdef MTK_AUDIO_GAIN_TABLE
                mStreamManager->setAnalogVolume(value, device, index, 0);
#endif
#if defined(MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT)
                mStreamManager->setVolumeIndex(value, device, index);
#endif
            }
        }
        param.remove(keyVolumeStreamType);
        param.remove(keyVolumeDevice);
        param.remove(keyVolumeIndex);
    }
#endif

    if (param.get(String8(AUDIO_PARAMETER_DEVICE_CONNECT), value_str) == NO_ERROR) {
        if (param.getInt(String8(AUDIO_PARAMETER_DEVICE_CONNECT), value) == NO_ERROR) {
            audio_devices_t device = (audio_devices_t)value;
            mStreamManager->updateDeviceConnectionState(device, true);
        }

        if (param.get(String8("card"), value_str) == NO_ERROR) {
            param.remove(String8("card"));
        }

        if (param.get(String8("device"), value_str) == NO_ERROR) {
            param.remove(String8("device"));
        }

        param.remove(String8(AUDIO_PARAMETER_DEVICE_CONNECT));
    }

    if (param.get(String8(AUDIO_PARAMETER_DEVICE_DISCONNECT), value_str) == NO_ERROR) {
        if (param.getInt(String8(AUDIO_PARAMETER_DEVICE_DISCONNECT), value) == NO_ERROR) {
            audio_devices_t device = (audio_devices_t)value;
            mStreamManager->updateDeviceConnectionState(device, false);
        }

        if (param.get(String8("card"), value_str) == NO_ERROR) {
            param.remove(String8("card"));
        }

        if (param.get(String8("device"), value_str) == NO_ERROR) {
            param.remove(String8("device"));
        }

        param.remove(String8(AUDIO_PARAMETER_DEVICE_DISCONNECT));
    }
    if (param.size())
    {
        TYPE_LOGW("%s() still have param.size() = %zu, remain param = '%s'",
              __FUNCTION__, param.size(), param.toString().string());
        status = BAD_VALUE;
    }

    ALOGV("-%s(): %s ", __FUNCTION__, keyValuePairs.string());
    return status;
}

String8 AudioALSAStreamOut::getParameters(const String8 &keys)
{
    ALOGD("%s, keyvalue %s", __FUNCTION__, keys.string());
    AudioParameter param = AudioParameter(keys);
    AudioParameter returnParam = AudioParameter();
    String8 value;
    String8 result;
    std::string literalFormat;
    const String8 keyStreamSupportedFormats = String8(AudioParameter::keyStreamSupportedFormats);

    if (isHdmiOut())
    {
        if (param.get(String8(AUDIO_PARAMETER_STREAM_SUP_CHANNELS), value) == NO_ERROR)
        {
            String8 support_channel;
            for (size_t i = 0; i < mSupportedChannelMasks.size(); i++)
            {
                support_channel.append(mSupportedChannelMasks[i]);
                if (i != (mSupportedChannelMasks.size() - 1))
                {
                    support_channel.append("|");
                }
            }
            param.remove(String8(AUDIO_PARAMETER_STREAM_SUP_CHANNELS));
            returnParam.add(String8(AUDIO_PARAMETER_STREAM_SUP_CHANNELS), support_channel);
        }
    }

    if (mStreamOutType == STREAM_OUT_HDMI_MULTI_CHANNEL)
    {
        if (param.get(String8(AUDIO_PARAMETER_STREAM_SUP_SAMPLING_RATES), value) == NO_ERROR)
        {
            String8 support_sample_rates;
            for (size_t i = 0; i < mSupportedSampleRates.size(); i++)
            {
                support_sample_rates.append(mSupportedSampleRates[i]);
                if (i != (mSupportedSampleRates.size() - 1))
                {
                    support_sample_rates.append("|");
                }
            }
            param.remove(String8(AUDIO_PARAMETER_STREAM_SUP_SAMPLING_RATES));
            returnParam.add(String8(AUDIO_PARAMETER_STREAM_SUP_SAMPLING_RATES), support_sample_rates);
        }

        if (param.get(String8(AUDIO_PARAMETER_STREAM_SUP_FORMATS), value) == NO_ERROR)
        {
            String8 support_format;
            for (size_t i = 0; i < mSupportedFormats.size(); i++)
            {
                support_format.append(mSupportedFormats[i]);
                if (i != (mSupportedFormats.size() - 1))
                {
                    support_format.append("|");
                }
            }
            param.remove(String8(AUDIO_PARAMETER_STREAM_SUP_FORMATS));
            returnParam.add(String8(AUDIO_PARAMETER_STREAM_SUP_FORMATS), support_format);
        }
    }

	if (param.get(keyStreamSupportedFormats, value) == NO_ERROR) {
        ALOGD(" getParameters(keys); = %s", keys.string());
        param.remove(keyStreamSupportedFormats);
        FormatConverter::toString(mStreamAttributeSource.audio_format, literalFormat);
        result.append(literalFormat.c_str());
        //return string
        returnParam.add(keyStreamSupportedFormats, result);
    }
    const String8 keyValuePairs = returnParam.toString();
    ALOGD("-%s(), return \"%s\"", __FUNCTION__, keyValuePairs.string());

    return keyValuePairs;
}

status_t AudioALSAStreamOut::getRenderPosition(uint32_t *dspFrames)
{
    status_t ret = INVALID_OPERATION;

    if (dspFrames == NULL)
        return ret;
    else
        *dspFrames = 0;

    if (isHdmiOutNeedIECWrap())
    {
        if (AL_TRYLOCK(mLock) != 0) // unable to get the lock
        {
            // use updated position based on write()
            return getRenderPosition_l(dspFrames, false);
        }

        ret = getRenderPosition_l(dspFrames, true);

        AL_UNLOCK(mLock);
    }

    return ret;
}

status_t AudioALSAStreamOut::getPresentationPosition(uint64_t *frames, struct timespec *timestamp)
{
    status_t ret = -ENODEV;

    if (isHdmiOut()) {
        // failed to get the lock. It is probably held by a blocked write().
        if (AL_TRYLOCK(mLock) != OK) {
            if (mLastPresentationValid) {
                // use caeched position
                Mutex::Autolock _l(mPresentationLock);
                *frames = mLastPresentationFrames;
                *timestamp = mLastPresentationTimestamp;
                ret = NO_ERROR;
            }
            return ret;
        }
    } else {
        AL_LOCK_MS(mLock, MAX_AUDIO_LOCK_TIMEOUT_MS);
    }

    ret = getPresentationPosition_l(frames, timestamp);

    AL_UNLOCK(mLock);

    return ret;
}

status_t AudioALSAStreamOut::getNextWriteTimestamp(int64_t *timestamp __unused)
{
    return INVALID_OPERATION;
}

status_t AudioALSAStreamOut::setCallBack(stream_callback_t callback __unused, void *cookie __unused)
{
    return INVALID_OPERATION;
}

void AudioALSAStreamOut::updateSourceMetadata(const struct source_metadata* source_metadata) {
    ALOGV("%s(), source_metadata = %p", __FUNCTION__, source_metadata);
}

status_t AudioALSAStreamOut::open()
{
    // call open() only when mLock is locked.
    ASSERT(AL_TRYLOCK(mLock) != 0);

    TYPE_LOGD("%s(), flags %d", __FUNCTION__, mStreamAttributeSource.mAudioOutputFlags);

    status_t status = NO_ERROR;

    if (mStandby == true)
    {
        checkSuspendOutput();

        if (mInternalSuspend == false)
        {
#if defined(DOWNLINK_SUPPORT_PERFSERVICE) && defined(DOWNLINK_LOW_LATENCY_CPU_SPEED)
            if (mStreamOutType == STREAM_OUT_LOW_LATENCY)
            {
                if (-1 == mPerfServiceHandle)
                    regPerfServiceScn(&mPerfServiceHandle, DOWNLINK_LOW_LATENCY_CPU_SPEED);

                if (mPerfServiceHandle >= 0)
                    enablePerfServiceScn(mPerfServiceHandle);
            }
#endif
#if defined(DOWNLINK_SUPPORT_PERFSERVICE) && defined(DOWNLINK_LOW_POWER_CPU_SPEED)
            if (mStreamOutType == STREAM_OUT_PRIMARY)
            {
                if (-1 == mPerfServiceHandle)
                {
                    regPerfServiceScn(&mPerfServiceHandle, 0);
                    setPerfServiceScn(mPerfServiceHandle, CMD_SET_CPU_FREQ_MIN, DOWNLINK_LOW_POWER_CPU_SPEED, 0);
                    setPerfServiceScn(mPerfServiceHandle, CMD_SET_SCREEN_OFF_STATE, SCREEN_OFF_ENABLE, 0);
                }
                if (mScreenOff && !mIsLowPowerEnabled)
                    enableLowPowerSetting();
            }
#endif
            // create playback handler
            ASSERT(mPlaybackHandler == NULL);
            AudioALSASampleRateController::getInstance()->setScenarioStatus(PLAYBACK_SCENARIO_STREAM_OUT);
            mPlaybackHandler = mStreamManager->createPlaybackHandler(&mStreamAttributeSource);

            // open audio hardware
            status = mPlaybackHandler->open();
            if (status != NO_ERROR)
            {
                TYPE_LOGE("%s(), open() fail!!", __FUNCTION__);
                mPlaybackHandler->close();
                mStreamManager->destroyPlaybackHandler(mPlaybackHandler);
                mPlaybackHandler = NULL;
            }
            else
            {
                mStandby = false;
            }
        }
        else
        {
            mStandby = false;
        }

        OpenPCMDump(LOG_TAG);

        mStandby = false;
    }

    TYPE_LOGD("-%s()",__FUNCTION__);

    return status;
}

status_t AudioALSAStreamOut::close()
{
    // call close() only when mLock is locked.
    ASSERT(AL_TRYLOCK(mLock) != 0);

    TYPE_LOGD("%s()", __FUNCTION__);

    status_t status = NO_ERROR;

    if (mStandby == false)
    {
        if (mInternalSuspend == false)
        {
            ASSERT(mPlaybackHandler != NULL);

            // close audio hardware
            status = mPlaybackHandler->close();
            if (status != NO_ERROR)
            {
                TYPE_LOGE("%s(), close() fail!!", __FUNCTION__);
            }

            // destroy playback handler
            mStreamManager->destroyPlaybackHandler(mPlaybackHandler);
            mPlaybackHandler = NULL;
            AudioALSASampleRateController::getInstance()->resetScenarioStatus(PLAYBACK_SCENARIO_STREAM_OUT);

#ifdef DOWNLINK_SUPPORT_PERFSERVICE
            if (mStreamOutType == STREAM_OUT_LOW_LATENCY)
            {
                if (mPerfServiceHandle >= 0)
                    disablePerfServiceScn(mPerfServiceHandle);
            }
            if (mStreamOutType == STREAM_OUT_PRIMARY && mIsLowPowerEnabled)
            {
                disableLowPowerSetting();
            }
#endif
        }

        if (mStreamOutType == STREAM_OUT_HDMI_MULTI_CHANNEL)
        {
             // let stereo stream have chance to leave internal suspend
             mStreamManager->putStreamOutIntoStandy(STREAM_OUT_HDMI_STEREO);
        }

        ClosePCMDump();

        mStandby = true;
        mWriteCount = 0;
    }

    ASSERT(mPlaybackHandler == NULL);
    return status;
}

status_t AudioALSAStreamOut::pause(){
    ALOGD("%s() %p", __FUNCTION__, mPlaybackHandler);
    if (mPlaybackHandler != NULL) {
        mPaused = true;
        return mPlaybackHandler->pause();
    }
    return -ENODATA;
}

status_t AudioALSAStreamOut::resume(){
    ALOGD("%s() %p", __FUNCTION__, mPlaybackHandler);
    if (mPlaybackHandler != NULL) {
        mPaused = false;
        return mPlaybackHandler->resume();
    }
    return -ENODATA;
}

status_t AudioALSAStreamOut::flush()
{
    status_t ret = INVALID_OPERATION;

    TYPE_LOGV("+%s()", __FUNCTION__);

    if (isHdmiOutNeedIECWrap())
    {
        if (mPlaybackHandler)
            mPlaybackHandler->flush();

        mPresentedBytes = 0;
        mRenderedBytes = 0;
        mLastDspFrames = 0;

        {
            Mutex::Autolock _l(mPresentationLock);
            mLastPresentationFrames = 0;
            mLastPresentationValid = false;
        }

        ret = NO_ERROR;
    }

    TYPE_LOGV("-%s()", __FUNCTION__);

    return ret;
}

int AudioALSAStreamOut::drain(audio_drain_type_t type __unused)
{
    return 0;
}

status_t AudioALSAStreamOut::routing(audio_devices_t output_devices)
{
    AL_AUTOLOCK(mLock);

    TYPE_LOGD("+%s(), output_devices = 0x%x", __FUNCTION__, output_devices);

    status_t status = NO_ERROR;

    if (output_devices == mStreamAttributeSource.output_devices)
    {
        TYPE_LOGW("%s(), warning, routing to same device is not necessary", __FUNCTION__);
        return status;
    }

    if (mStandby == false)
    {
        ASSERT(mPlaybackHandler != NULL);

        {
            status = close();
        }
    }

    mStreamAttributeSource.output_devices = output_devices;

    TYPE_LOGD("-%s()", __FUNCTION__);
    return status;
}

status_t AudioALSAStreamOut::setScreenState(bool mode)
{
    TYPE_LOGD("+%s(), mode %d", __FUNCTION__, mode);

    android_atomic_inc(&mLockCount);
    AL_AUTOLOCK(mLock);
    android_atomic_dec(&mLockCount);

#ifdef DOWNLINK_SUPPORT_PERFSERVICE
    mScreenOff = !mode;

    if (mStreamOutType == STREAM_OUT_PRIMARY && !mStandby)
    {
        if (mScreenOff && !mIsLowPowerEnabled)
            enableLowPowerSetting();
        else if (!mScreenOff && mIsLowPowerEnabled)
            disableLowPowerSetting();
    }
#endif

    TYPE_LOGD("-%s()", __FUNCTION__);

    return NO_ERROR;
}

status_t AudioALSAStreamOut::setSuspend(const bool suspend_on)
{
    TYPE_LOGD("+%s(), mSuspendCount = %u, suspend_on = %d, flags %d",
          __FUNCTION__, mSuspendCount, suspend_on, mStreamAttributeSource.mAudioOutputFlags);

    android_atomic_inc(&mLockCount);
    AL_AUTOLOCK(mSuspendLock);
    android_atomic_dec(&mLockCount);

    if (suspend_on == true)
    {
        mSuspendCount++;
    }
    else if (suspend_on == false)
    {
        ASSERT(mSuspendCount > 0);
        mSuspendCount--;
    }

    TYPE_LOGD("-%s(), mSuspendCount = %u", __FUNCTION__, mSuspendCount);
    return NO_ERROR;
}

status_t AudioALSAStreamOut::setSuspendStreamOutHDMIStereo(const bool suspend_on)
{
    ALOGD("+%s(), mSuspendStreamOutHDMIStereoCount = %u, suspend_on = %d",
          __FUNCTION__, mSuspendStreamOutHDMIStereoCount, suspend_on);

    if (suspend_on == true)
    {
        mSuspendStreamOutHDMIStereoCount++;
    }
    else if (suspend_on == false)
    {
        ASSERT(mSuspendStreamOutHDMIStereoCount > 0);
        mSuspendStreamOutHDMIStereoCount--;
    }

    ALOGD("-%s(), mSuspendStreamOutHDMIStereoCount = %u", __FUNCTION__, mSuspendStreamOutHDMIStereoCount);
    return NO_ERROR;
}

void AudioALSAStreamOut::OpenPCMDump(const char *class_name)
{
    String8 dump_file_name;
    dump_file_name.appendFormat("%s.%d.%s.%d_%dch_format(0x%x).pid%d.tid%d.pcm",
                                streamout, mDumpFileNum, class_name, mStreamAttributeSource.sample_rate,
                                mStreamAttributeSource.num_channels, mStreamAttributeSource.audio_format,
                                getpid(), gettid());

    mPCMDumpFile = NULL;
    mPCMDumpFile = AudioOpendumpPCMFile(dump_file_name.string(), streamout_propty);
    if (mPCMDumpFile != NULL)
    {
        TYPE_LOGD("%s DumpFileName = %s", __FUNCTION__, dump_file_name.string());

        mDumpFileNum++;
        mDumpFileNum %= MAX_DUMP_NUM;
    }
}

void AudioALSAStreamOut::ClosePCMDump()
{
    if (mPCMDumpFile)
    {
        AudioCloseDumpPCMFile(mPCMDumpFile);
    }
}

void AudioALSAStreamOut::WritePcmDumpData(const void *buffer, ssize_t bytes)
{
    if (mPCMDumpFile)
    {
        AudioDumpPCMData((void *)buffer , bytes, mPCMDumpFile);
    }
}

void AudioALSAStreamOut::setBufferSize()
{
    uint32_t bufferFrameCount = kDefaultOutputBufferFrameCOunt[mStreamOutType];

    // adjust buffer frame count
    if ((mStreamAttributeSource.audio_format == AUDIO_FORMAT_IEC61937) &&
        (mStreamAttributeSource.sample_rate > kDefaultOutputSourceSampleRate))
    {
        bufferFrameCount *= (mStreamAttributeSource.sample_rate / kDefaultOutputSourceSampleRate);
    }
    else if (audio_is_linear_pcm(mStreamAttributeSource.audio_format) &&
             (mStreamAttributeSource.sample_rate != kDefaultOutputSourceSampleRate) &&
             (mStreamOutType != STREAM_OUT_PRIMARY))
             // FIXME: compensation filter processes data in fixed block size (512 samples).
    {
        bufferFrameCount = (bufferFrameCount * mStreamAttributeSource.sample_rate) / kDefaultOutputSourceSampleRate;
        bufferFrameCount &= ~0xf; /* 16 frames alignment */
    }

    mStreamAttributeSource.buffer_size = bufferFrameCount * frameSize();
    mStreamAttributeSource.latency = (bufferFrameCount * 1000) / mStreamAttributeSource.sample_rate;

    if (mStreamOutType == STREAM_OUT_PRIMARY)
    {
        mBufferSizePerAccess = mStreamAttributeSource.buffer_size >> 2;
    }
    else if (isHdmiOutNeedIECWrap())
    {
        mBufferSizePerAccess = mStreamAttributeSource.buffer_size >> 4;
    }
    else
    {
        mBufferSizePerAccess = mStreamAttributeSource.buffer_size >> 1;
    }
}

void AudioALSAStreamOut::checkSuspendOutput(void)
{
    if (mStreamOutType == STREAM_OUT_HDMI_MULTI_CHANNEL)
    {
        mStreamManager->putStreamOutIntoStandy(STREAM_OUT_HDMI_STEREO);
        mInternalSuspend = false;
    }
    else if (mStreamOutType == STREAM_OUT_HDMI_STEREO &&
             mStreamManager->isStreamOutActive(STREAM_OUT_HDMI_MULTI_CHANNEL))
    {
        mInternalSuspend = true;
    }
    else
    {
        mInternalSuspend = false;
    }
}

uint32_t AudioALSAStreamOut::dataToDurationUs(size_t bytes)
{
    return (bytes * 1000 / (frameSize() * sampleRate() / 1000));
}

void AudioALSAStreamOut::checkRedundantAndAdd(Vector<String8>& strVector, String8 str)
{
    for (Vector<String8>::const_iterator nextStr = strVector.begin();
         nextStr != strVector.end(); ++nextStr) {
        if (0 == (*nextStr).compare(str)) {
            // string existed
            return;
        }
    }

    strVector.add(str);
}

status_t AudioALSAStreamOut::getPresentationPosition_l(uint64_t *frames, struct timespec *timestamp)
{
    status_t ret = INVALID_OPERATION;

    if (mPlaybackHandler != NULL)
    {
        time_info_struct_t HW_Buf_Time_Info;
        memset(&HW_Buf_Time_Info, 0, sizeof(HW_Buf_Time_Info));
        //query remaining hardware buffer size
        if (NO_ERROR == mPlaybackHandler->getHardwareBufferInfo(&HW_Buf_Time_Info))
        {
            uint64_t presentedFrames = mPresentedBytes / (uint64_t)frameSize();
            uint64_t remainingFrames = HW_Buf_Time_Info.buffer_per_time - HW_Buf_Time_Info.frameInfo_get;
            uint64_t retFrames = (presentedFrames > remainingFrames) ? (presentedFrames - remainingFrames) : 0;

            *frames = retFrames;
            *timestamp = HW_Buf_Time_Info.timestamp_get;

            ret = NO_ERROR;

            if (isHdmiOut())
            {
                Mutex::Autolock _l(mPresentationLock);
                if (mLastPresentationFrames < *frames)
                {
                    mLastPresentationFrames = *frames;
                    mLastPresentationTimestamp = *timestamp;
                }
                else
                {
                    if (mLastPresentationValid) {
                        *frames = mLastPresentationFrames;
                        *timestamp = mLastPresentationTimestamp;
                    }
                    else
                    {
                        ret = INVALID_OPERATION;
                    }
                }
            }

            TYPE_LOGVV("%s() buffer_per_time = %d frameInfo_get = %d presentedFrames = %llu retFrames = %llu",
                     __FUNCTION__, HW_Buf_Time_Info.buffer_per_time, HW_Buf_Time_Info.frameInfo_get,
                     presentedFrames, retFrames);
        }
        else if (mPlaybackHandler->getPlaybackHandlerType() != PLAYBACK_HANDLER_BT_CVSD)
        {
            if (mWriteCount > 4 && !(mWriteCount%100))
                TYPE_LOGW("%s() getHardwareBufferInfo[%" PRIu64 "] fail", __FUNCTION__, mWriteCount);
            else
                TYPE_LOGV("%s() getHardwareBufferInfo[%" PRIu64 "] initial fail", __FUNCTION__, mWriteCount);
        }
    }
    else if (mInternalSuspend)
    {
        uint64_t presentedFrames = mPresentedBytes / (uint64_t)frameSize();
        *frames = presentedFrames;
    }
    else
    {
        TYPE_LOGV("%s() no playback handler!? mSuspendCount = %u mStandby = %d",
                  __FUNCTION__, mSuspendCount, mStandby);
    }

    mLastPresentationValid = (ret == NO_ERROR);

    return ret;
}

status_t AudioALSAStreamOut::getRenderPosition_l(uint32_t *dspFrames, bool isGetFromHW)
{
    if (dspFrames == NULL)
        return -EINVAL;
    else
        *dspFrames = mLastDspFrames;

    status_t ret = NO_ERROR;
    uint64_t RenderedFrames = mRenderedBytes / (uint64_t)frameSize();
    uint64_t remainingFrames = 0;

    if (isGetFromHW)
    {
        if (mPlaybackHandler != NULL)
        {
            time_info_struct_t HW_Buf_Time_Info;
            memset(&HW_Buf_Time_Info, 0, sizeof(HW_Buf_Time_Info));
            //query remaining hardware buffer size
            if (NO_ERROR == mPlaybackHandler->getHardwareBufferInfo(&HW_Buf_Time_Info))
            {
                remainingFrames = (HW_Buf_Time_Info.buffer_per_time - HW_Buf_Time_Info.frameInfo_get);
            }
            else
            {
                if (dspFrames)
                    TYPE_LOGV("%s() *dspFrames = %u", __FUNCTION__, *dspFrames);
                return ret;
            }
        }
        else
        {
            if (dspFrames)
                TYPE_LOGV("%s() *dspFrames = %u", __FUNCTION__, *dspFrames);

            TYPE_LOGW("%s() playback handler was closed", __FUNCTION__);
            return ret;
        }
    }

    uint64_t retFrames = (RenderedFrames > remainingFrames) ? (RenderedFrames - remainingFrames) : 0;
    *dspFrames = retFrames;

    if (mLastDspFrames <= *dspFrames)
    {
        mLastDspFrames = *dspFrames;
    }
    else
    {
        *dspFrames = mLastDspFrames;
    }

    if (dspFrames)
        TYPE_LOGV("%s() *dspFrames = %u", __FUNCTION__, *dspFrames);

    return ret;
}

void AudioALSAStreamOut::enableLowPowerSetting(void)
{
#ifdef DOWNLINK_SUPPORT_PERFSERVICE
    if (mPerfServiceHandle >= 0)
    {
        enablePerfServiceScn(mPerfServiceHandle);
        mIsLowPowerEnabled = true;
    }
#endif
}

void AudioALSAStreamOut::disableLowPowerSetting(void)
{
#ifdef DOWNLINK_SUPPORT_PERFSERVICE
    if (mPerfServiceHandle >= 0)
    {
        disablePerfServiceScn(mPerfServiceHandle);
        mIsLowPowerEnabled = false;
    }
#endif
}

status_t AudioALSAStreamOut::start() {
    int ret = INVALID_OPERATION;
    ALOGD("+%s()", __FUNCTION__);

    AL_AUTOLOCK(mLock);

    if ((mStreamAttributeSource.mAudioOutputFlags & AUDIO_OUTPUT_FLAG_MMAP_NOIRQ) &&
        (mPlaybackHandler != NULL) && !mStandby && !mStart) {
        ret = mPlaybackHandler->start();
        if (ret == 0) {
            mStart = true;
        }
    } else {
        ALOGW("%s fail, flags %d, !mStandby %d, !mStart %d, !mPlaybackHandler %d", __func__,
              mStreamAttributeSource.mAudioOutputFlags, !mStandby, !mStart, mPlaybackHandler != NULL);
    }

    ALOGD("-%s()", __FUNCTION__);
    return ret;
}

status_t AudioALSAStreamOut::stop() {
    int ret = INVALID_OPERATION;
    ALOGD("+%s()", __FUNCTION__);

    AL_AUTOLOCK(mLock);

    if (mStreamAttributeSource.mAudioOutputFlags & AUDIO_OUTPUT_FLAG_MMAP_NOIRQ && mPlaybackHandler != NULL
        && !mStandby && mStart) {
        ret = mPlaybackHandler->stop();
        if (ret == 0) {
            mStart = false;
        }
    } else {
        ALOGW("%s fail, flags %d, !mStandby %d, !mStart %d, !mPlaybackHandler %d", __func__,
              mStreamAttributeSource.mAudioOutputFlags, !mStandby, !mStart, mPlaybackHandler != NULL);
    }

    ALOGD("-%s", __FUNCTION__);
    return ret;
}

status_t AudioALSAStreamOut::createMmapBuffer(int32_t min_size_frames,
                                              struct audio_mmap_buffer_info *info) {
    int ret = INVALID_OPERATION;
    ALOGD("+%s(), min_size_frames %d", __FUNCTION__, min_size_frames);

    AL_AUTOLOCK(mLock);

    if (info == NULL || min_size_frames == 0) {
        ALOGE("%s(): info = %p, min_size_frames = %d", __func__, info, min_size_frames);
        return BAD_VALUE;
    }

    if (mStreamAttributeSource.mAudioOutputFlags & AUDIO_OUTPUT_FLAG_MMAP_NOIRQ && mStandby) {
        if (mPlaybackHandler == NULL) {
            mPlaybackHandler = mStreamManager->createPlaybackHandler(&mStreamAttributeSource);
        }
        ret = mPlaybackHandler->createMmapBuffer(min_size_frames, info);
        mStandby = false;
    } else {
        ALOGW("%s() fail, flags %d, mStandby %d, !mPlaybackHandler %d", __func__,
              mStreamAttributeSource.mAudioOutputFlags, mStandby, mPlaybackHandler != NULL);
    }

    ALOGD("-%s()", __FUNCTION__);
    return ret;
}

status_t AudioALSAStreamOut::getMmapPosition(struct audio_mmap_position *position) {
    int ret = INVALID_OPERATION;
    //ALOGD("%s+", __FUNCTION__);

    AL_AUTOLOCK(mLock);

    if (position == NULL || mPlaybackHandler == NULL) {
        ALOGD("%s(), mPlaybackHandler or position == NULL!!", __func__);
        //return BAD_VALUE;
        return NO_ERROR;
    }
    if (!(mStreamAttributeSource.mAudioOutputFlags & AUDIO_OUTPUT_FLAG_MMAP_NOIRQ)) {
        return BAD_VALUE;
    }

    ret = mPlaybackHandler->getMmapPosition(position);

    //ALOGD("%s-", __FUNCTION__);
    return ret;
}

}
