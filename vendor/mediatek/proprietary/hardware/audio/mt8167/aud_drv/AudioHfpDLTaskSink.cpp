#define LOG_TAG "AudioHfpDLTaskSink"

#include "AudioHfpDLTaskSink.h"
#include "AudioVolumeFactory.h"
#include "AudioALSADeviceParser.h"
#include "AudioALSAVolumeController.h"
#include "AudioALSADriverUtility.h"
#include "AudioALSAHardwareResourceManager.h"
//#include "MtkAudioBitConverter.h"

#define NEED_64BYTES_ALIGN

/* delete array and set to null*/
#define DELETE_ARRAY_AND_SET_NULL(x) \
do{                                  \
    if(x!=NULL){                     \
        delete[] x;                  \
        x = NULL;                    \
    }                                \
} while(0)


/* delete and set to null*/
#define DELETE_AND_SET_NULL(x) \
do{                            \
    if(x!=NULL){               \
        delete x;              \
        x = NULL;              \
    }                          \
} while(0)

static const char *PROPERTY_KEY_EXTDAC = "vendor.audiohal.resource.extdac.support";


namespace android
{
AudioHfpDLTaskSink::AudioHfpDLTaskSink()
    : mStarted(false)
    , mSampleRate(16000)
    , mChannels(2)
    , mFormat(AUDIO_FORMAT_PCM_16_BIT)
    , mInterruptSample(160)
    , mDevice(AUDIO_DEVICE_OUT_SPEAKER)
    , mHardwareResourceManager(AudioALSAHardwareResourceManager::getInstance())
    , mMixer(AudioALSADriverUtility::getInstance()->getMixer())
    , mPcm(NULL)
    , mBitConverter(NULL)
    , mBitConverterOutputBuffer(NULL)
    , mDataPendingOutputBuffer(NULL)
    , mDataPendingRemindBuffer(NULL)
    , mDataPendingRemindNumber(0)
{
}

AudioHfpDLTaskSink::~AudioHfpDLTaskSink()
{
    // deinit bit converter if need
    if (mBitConverter != NULL) {
        mBitConverter->close();
        deleteMtkAudioBitConverter(mBitConverter);
        mBitConverter = NULL;
    }

    DELETE_ARRAY_AND_SET_NULL(mBitConverterOutputBuffer);
    DELETE_ARRAY_AND_SET_NULL(mDataPendingOutputBuffer);
    DELETE_ARRAY_AND_SET_NULL(mDataPendingRemindBuffer);
}

status_t AudioHfpDLTaskSink::prepare(uint32_t device, int format, uint32_t channelCount,
                uint32_t sampleRate, uint32_t accessSize)
{
    mDevice = device;
    mSampleRate = sampleRate;
    mChannels = channelCount;
    mFormat = format;
    mInterruptSample = accessSize / (audio_bytes_per_sample((audio_format_t)format) * channelCount);
    return NO_ERROR;
}

ssize_t AudioHfpDLTaskSink::write(const void *buffer, size_t bytes)
{
    ssize_t write_size = bytes;

    if (!mStarted)
    {
        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

        int pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl1Meida);
        int cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl1Meida);

        setLowJitterMode(true, mSampleRate);

        mHardwareResourceManager->startOutputDevice(mDevice, mSampleRate);
#if !defined(MTK_CONSYS_MT8167)
        mHardwareResourceManager->setRoutePath(ROUTE_NORMAL_PLAYBACK, AUDIO_DEVICE_OUT_SPEAKER);
#endif

        // HW pcm config
        mConfig.channels = mChannels;
        mConfig.rate = mSampleRate;
        mConfig.period_count = 4;
        mConfig.period_size = mInterruptSample;
#ifdef PLAYBACK_USE_24BITS_ONLY
        mConfig.format = PCM_FORMAT_S32_LE;
#else
        mConfig.format = audioFormatToPcm(mFormat);
#endif
        mConfig.start_threshold = mInterruptSample;
        mConfig.stop_threshold = 0;
        mConfig.silence_threshold = 0;
        mConfig.silence_size = 0;
        mConfig.avail_min = 0;

        ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d, pcmindex = %d, cardindex = %d",
              __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format, pcmindex, cardindex);

        // handler for 24 bits
#ifdef PLAYBACK_USE_24BITS_ONLY
        if (mFormat != AUDIO_FORMAT_PCM_8_24_BIT)
        {
            initBitConverter(mFormat, AUDIO_FORMAT_PCM_8_24_BIT);
        }
#endif

#ifdef NEED_64BYTES_ALIGN
        //data pending init (64bytes)
        mDataPendingOutputBuffer = new char[mMaxPcmDriverBufferSize + mDataAlignedSize];
        mDataPendingRemindBuffer = new char[mDataAlignedSize];
        mDataPendingRemindNumber = 0;
#endif

        mPcm = pcm_open(cardindex, pcmindex, PCM_OUT | PCM_MONOTONIC, &mConfig);
        if (mPcm == NULL)
        {
            ALOGE("%s(), mPcm == NULL!!", __FUNCTION__);
            return -1;
        }
        else if (pcm_is_ready(mPcm) == false)
        {
            ALOGE("%s(), pcm_is_ready(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            return -1;
        }
        else if (pcm_prepare(mPcm) != 0)
        {
            ALOGE("%s(), pcm_prepare(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            return -1;
        }

        //gain setting
        AudioVolumeInterface *audioVolumeCtrl = AudioVolumeFactory::CreateAudioVolumeController();
        audioVolumeCtrl->setMasterVolume(audioVolumeCtrl->getMasterVolume(), AUDIO_MODE_IN_COMMUNICATION, mDevice);

        mStarted = true;
    }

    void* bufferMainData = const_cast<void *>(buffer); //for bitConverter
    uint32_t bufferMainDataSize = bytes;

    if (mBitConverter != NULL)
    {
        uint32_t bufferSizeAfterBitConvert = mMaxPcmDriverBufferSize;
        mBitConverter->process(bufferMainData, &bufferMainDataSize, (void *)mBitConverterOutputBuffer, &bufferSizeAfterBitConvert);
        bufferMainData = (void *)mBitConverterOutputBuffer;
        bufferMainDataSize = bufferSizeAfterBitConvert;
    }

#ifdef NEED_64BYTES_ALIGN
    {
        int reminder = mDataPendingRemindNumber;
        int inputSize = bufferMainDataSize;

        int reminderToNextWrite = (inputSize + reminder) % mDataAlignedSize;
        int outputSize = (inputSize + reminder) - reminderToNextWrite;

        if (reminder || reminderToNextWrite)
        {
            if (reminder)
            {
                memcpy(mDataPendingOutputBuffer, mDataPendingRemindBuffer, reminder);
            }

            memcpy(mDataPendingOutputBuffer + reminder, bufferMainData, inputSize - reminderToNextWrite);

            if (reminderToNextWrite)
            {
                memcpy(mDataPendingRemindBuffer, (char*)bufferMainData + inputSize - reminderToNextWrite,
                       reminderToNextWrite);
            }

            mDataPendingRemindNumber = reminderToNextWrite;
            bufferMainData = mDataPendingOutputBuffer;
            bufferMainDataSize = outputSize;
        }
    }
#endif

    int retval = pcm_write(mPcm, bufferMainData, bufferMainDataSize);
    ALOGE_IF(retval != 0, "%s(), pcm_write() error, retval = %d", __FUNCTION__, retval);

    return write_size;
}

status_t AudioHfpDLTaskSink::stop()
{
    if (mStarted) {
        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());


        if (mPcm != NULL)
        {
            pcm_stop(mPcm);
            pcm_close(mPcm);
            mPcm = NULL;
        }
		mHardwareResourceManager->stopOutputDevice();
        mHardwareResourceManager->resetRoutePath(ROUTE_NORMAL_PLAYBACK, AUDIO_DEVICE_OUT_SPEAKER);

        setLowJitterMode(false, mSampleRate);

        //deinit 64bytes alignment and 32 bits convert here
        if (mBitConverter != NULL) {
            mBitConverter->close();
            deleteMtkAudioBitConverter(mBitConverter);
            mBitConverter = NULL;
        }
        DELETE_ARRAY_AND_SET_NULL(mBitConverterOutputBuffer);
        DELETE_ARRAY_AND_SET_NULL(mDataPendingOutputBuffer);
        DELETE_ARRAY_AND_SET_NULL(mDataPendingRemindBuffer);
        mDataPendingRemindNumber = 0;
        mStarted = false;
    }
    return NO_ERROR;
}

int AudioHfpDLTaskSink::getRemainDataLatency()
{
    int latencyMs = 0;
    if (mPcm) {
        unsigned int avail;
        struct timespec tstamp;
        if (pcm_get_htimestamp(mPcm, &avail, &tstamp) == 0) {
            latencyMs = avail * 1000 / mConfig.rate;
        } else {
            ALOGW("%s pcm_get_htimestamp fail", __FUNCTION__);
        }
    }
    return latencyMs;
}

pcm_format AudioHfpDLTaskSink::audioFormatToPcm(int format)
{
    pcm_format retval = PCM_FORMAT_S16_LE;
    switch (format)
    {
        case AUDIO_FORMAT_PCM_8_BIT:
        {
            retval = PCM_FORMAT_S8;
            break;
        }
        case AUDIO_FORMAT_PCM_16_BIT:
        {
            retval = PCM_FORMAT_S16_LE;
            break;
        }
        case AUDIO_FORMAT_PCM_8_24_BIT:
        case AUDIO_FORMAT_PCM_32_BIT:
        {
            retval = PCM_FORMAT_S32_LE;
            break;
        }
        default:
        {
            ALOGE("No such audio format(0x%x)!! Use AUDIO_FORMAT_PCM_16_BIT(0x%x) instead", format, PCM_FORMAT_S16_LE);
            retval = PCM_FORMAT_S16_LE;
            break;
        }
    }
    ALOGD("%s(), audio_format(0x%x) => pcm_format(0x%x)", __FUNCTION__, format, retval);
    return retval;
}

status_t AudioHfpDLTaskSink::initBitConverter(int source, int target)
{
    // init bit converter if need
    if (source == target)
    {
        ALOGD("%s, we don't need bit Converter",__FUNCTION__);
        return NO_ERROR;
    }

    BCV_PCM_FORMAT bcv_pcm_format = BCV_IN_Q1P31_OUT_Q1P15; //useless, suppress warning
    if ((source == AUDIO_FORMAT_PCM_32_BIT) || (source == AUDIO_FORMAT_PCM_8_24_BIT))
    {
        if (target == AUDIO_FORMAT_PCM_16_BIT)
        {
            bcv_pcm_format = BCV_IN_Q1P31_OUT_Q1P15;
        }
        else if (target == AUDIO_FORMAT_PCM_8_24_BIT)
        {
            bcv_pcm_format = BCV_IN_Q1P31_OUT_Q9P23;
        }
    }
    else if (source == AUDIO_FORMAT_PCM_16_BIT)
    {
        if (target == AUDIO_FORMAT_PCM_32_BIT)
        {
            bcv_pcm_format = BCV_IN_Q1P15_OUT_Q1P31;
        }
        else if (target == AUDIO_FORMAT_PCM_8_24_BIT)
        {
            bcv_pcm_format = BCV_IN_Q1P15_OUT_Q9P23;
        }
    }
    else
    {
        ALOGE("%s, the conversion is not supported",__FUNCTION__);
        return BAD_VALUE;
    }

    ALOGD("%s(), audio_format: 0x%x => 0x%x, bcv_pcm_format = 0x%x",
          __FUNCTION__, source, target, bcv_pcm_format);

    ASSERT(mBitConverter == NULL); //no memory leak allow

    if (mChannels > 2) {
        mBitConverter = newMtkAudioBitConverter(
                            mSampleRate,
                            2,
                            bcv_pcm_format);
    } else {
        mBitConverter = newMtkAudioBitConverter(
                            mSampleRate,
                            mChannels,
                            bcv_pcm_format);
    }

    ASSERT(mBitConverter != NULL);
    mBitConverter->open();
    mBitConverter->resetBuffer();

    ASSERT(mBitConverterOutputBuffer == NULL); //no memory leak allow
    mBitConverterOutputBuffer = new char[mMaxPcmDriverBufferSize];
    ASSERT(mBitConverterOutputBuffer != NULL);

    ALOGV("%s(), mBitConverter = %p, mBitConverterOutputBuffer = %p", __FUNCTION__, mBitConverter, mBitConverterOutputBuffer);
    return NO_ERROR;
}

bool AudioHfpDLTaskSink::setLowJitterMode(bool bEnable,uint32_t SampleRate)
{
    ALOGD("%s() bEanble = %d SampleRate = %u", __FUNCTION__, bEnable, SampleRate);

    enum mixer_ctl_type type;
    struct mixer_ctl *ctl;
    int retval = 0;

    // check need open low jitter mode
    if(SampleRate < 48000 && (AudioALSADriverUtility::getInstance()->GetPropertyValue(PROPERTY_KEY_EXTDAC)) == false)
    {
        ALOGD("%s(), not enable low jitter mode", __FUNCTION__);
        return false;
    }

    ctl = mixer_get_ctl_by_name(mMixer, "Audio_I2S0dl1_hd_Switch");

    if (ctl == NULL)
    {
        ALOGE("Audio_I2S0dl1_hd_Switch not support");
        return false;
    }

    if (bEnable == true)
    {
        retval = mixer_ctl_set_enum_by_string(ctl, "On");
        ASSERT(retval == 0);
    }
    else
    {
        retval = mixer_ctl_set_enum_by_string(ctl, "Off");
        ASSERT(retval == 0);
    }
    return true;
}

}
