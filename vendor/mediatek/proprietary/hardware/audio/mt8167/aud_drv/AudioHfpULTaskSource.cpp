#define LOG_TAG "AudioHfpULTaskSource"

#include "AudioHfpULTaskSource.h"
#include "AudioVolumeFactory.h"
#include "AudioALSADeviceParser.h"
#include "AudioALSAVolumeController.h"
#include "AudioALSADriverUtility.h"
#include "AudioALSAHardwareResourceManager.h"


namespace android
{

AudioHfpULTaskSource::AudioHfpULTaskSource()
    : mStarted(false),
      mSampleRate(16000),
      mAudioCustParamClient(AudioCustParamClient::GetInstance()),
      mChannels(2),
      mInterruptSample(160),
      mInputDevice(AUDIO_DEVICE_IN_BUILTIN_MIC),
      mOutputDevice(AUDIO_DEVICE_OUT_SPEAKER),
      mHardwareResourceManager(AudioALSAHardwareResourceManager::getInstance()),
      mAudioALSAVolumeController(AudioVolumeFactory::CreateAudioVolumeController()),
      mPcm(NULL),
      mBitConverterReadBuffer(NULL)
{
}

AudioHfpULTaskSource::~AudioHfpULTaskSource()
{
#ifdef RECORD_INPUT_24BITS
    if (mBitConverterReadBuffer != NULL)
    {
        delete[] mBitConverterReadBuffer;
        mBitConverterReadBuffer = NULL;
    }
#endif
}

status_t AudioHfpULTaskSource::prepare(uint32_t inDevice, uint32_t outDevice, int format,
            uint32_t channelCount, uint32_t sampleRate, uint32_t accessSize)
{
    mInputDevice = inDevice;
    mOutputDevice = outDevice;
    mSampleRate = sampleRate;
    mChannels = channelCount;
    mFormat = format;
    mInterruptSample = accessSize / (audio_bytes_per_sample((audio_format_t)format) * channelCount);
    return NO_ERROR;
}

status_t AudioHfpULTaskSource::start()
{
    ALOGD("%s()",__FUNCTION__);
    if (!mStarted)
    {
        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());
        int pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmUl1Capture);
        int cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmUl1Capture);

        mConfig.channels = mChannels;
        mConfig.rate = mSampleRate;
        mConfig.period_count = 8;
        mConfig.period_size = mInterruptSample;

        mHardwareResourceManager->startInputDevice(mInputDevice);
#if !defined(MTK_CONSYS_MT8167)
        mHardwareResourceManager->setRoutePath(ROUTE_NORMAL_PLAYBACK, AUDIO_DEVICE_OUT_SPEAKER);
#endif

#ifdef RECORD_INPUT_24BITS // 24bit record
        mConfig.format = PCM_FORMAT_S32_LE;
        if (mFormat != AUDIO_FORMAT_PCM_16_BIT)
        {
            ALOGW("%s, now driver force to 32bits pcm and we expect 16bits pcm record",__FUNCTION__);
            ALOGW("%s, there will be some problem if you request other format",__FUNCTION__);
        }
        else
        {
            mBitConverterReadBuffer = new char[mMaxPcmDriverBufferSize];
        }
#else
        mConfig.format = audioFormatToPcm(mFormat);
#endif
        mConfig.start_threshold = 0;
        mConfig.stop_threshold = 0;
        mConfig.silence_threshold = 0;
        mConfig.silence_size = 0;
        mConfig.avail_min = 0;

        ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
              __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);

        if (mPcm != NULL)
            ALOGW("%s, mPcm != NULL before we request pcm interface! mPcm: %p", __FUNCTION__, mPcm);

        mPcm = pcm_open(cardindex, pcmindex, PCM_IN | PCM_MONOTONIC, &mConfig);
        if (mPcm == NULL)
        {
            ALOGE("%s(), mPcm == NULL!!", __FUNCTION__);
            return UNKNOWN_ERROR;
        }
        else if (pcm_is_ready(mPcm) == false)
        {
            ALOGE("%s(), pcm_is_ready(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            return UNKNOWN_ERROR;
        }

        if (pcm_start(mPcm) != 0)
        {
            ALOGE("%s(), pcm_start(%p) fail due to %s", __FUNCTION__, mPcm, pcm_get_error(mPcm));
        }

        // apply gain due to input device
        if (mOutputDevice == AUDIO_DEVICE_OUT_EARPIECE)
        {
            mAudioALSAVolumeController->ApplyMicGain(Normal_Mic, AUDIO_MODE_IN_COMMUNICATION);
        }
        else if (mOutputDevice == AUDIO_DEVICE_OUT_SPEAKER)
        {
            mAudioALSAVolumeController->ApplyMicGain(Handfree_Mic, AUDIO_MODE_IN_COMMUNICATION);
        }
        else
        {
            mAudioALSAVolumeController->ApplyMicGain(Headset_Mic, AUDIO_MODE_IN_COMMUNICATION);
        }

        mStarted = true;
    }
    return NO_ERROR;
}

ssize_t AudioHfpULTaskSource::read(void *buffer, ssize_t bytes)
{
    ssize_t readSize = bytes;

#ifdef RECORD_INPUT_24BITS // 8bits '0' and 24bits data
    if(mFormat == AUDIO_FORMAT_PCM_16_BIT)
    {
        int pcmReadSize = bytes * 2;
        int readCount = bytes >> 1;

        ASSERT(pcmReadSize <= mMaxPcmDriverBufferSize); //no memory corruption allowed
        ALOGW_IF((pcmReadSize % 64 != 0), "%s not 64 bytes aligned, pcmReadSize = %d", __FUNCTION__, pcmReadSize);

        int ret = pcm_read(mPcm, mBitConverterReadBuffer, pcmReadSize);
        if (ret != 0)
        {
            ALOGE("%s pcm_read fail ret = %d(%s)", __FUNCTION__, ret, pcm_get_error(mPcm));
            return 0;
        }

        uint32_t *dataIterFor32Bits = (uint32_t *)mBitConverterReadBuffer;
        int16_t *dataIterFor16Bits = (int16_t *)buffer;
        for (int i = 0; i < readCount; i++)
        {
            *(dataIterFor16Bits + i) = (int16_t)(*(dataIterFor32Bits + i) >> 8);
        }
    }
#else
    ALOGW_IF((bytes % 64 != 0), "%s not 64 bytes aligned, bytes = %d", __FUNCTION__, bytes);
    int ret = pcm_read(mPcm, buffer, bytes);
    if (ret != 0)
    {
        ALOGE("%s pcm_read fail ret = %d(%s)", __FUNCTION__, ret, pcm_get_error(mPcm));
        return 0;
    }
#endif

    //if dual mic not support
    if ((mAudioCustParamClient->QueryFeatureSupportInfo()& SUPPORT_DUAL_MIC) == 0)
    {
        int sampleCount = bytes >> 2; //16 bits two channels
        int16_t *dataIter = (int16_t *)buffer;

        while (sampleCount)
        {
            *(dataIter + 1) = *dataIter; // left -> right
            dataIter += 2;
            sampleCount--;
        }
    }

    return readSize;
}

status_t AudioHfpULTaskSource::stop()
{
    if (mStarted)
    {
        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

        if (mPcm) {
            pcm_stop(mPcm);
            pcm_close(mPcm);
            mPcm = NULL;
        }

        //close codec
        mHardwareResourceManager->stopInputDevice(mHardwareResourceManager->getInputDevice());
        mHardwareResourceManager->resetRoutePath(ROUTE_NORMAL_PLAYBACK, AUDIO_DEVICE_OUT_SPEAKER);

#ifdef RECORD_INPUT_24BITS
        delete[] mBitConverterReadBuffer;
        mBitConverterReadBuffer = NULL;
#endif
        mStarted = false;
    }
    return NO_ERROR;
}

int AudioHfpULTaskSource::getRemainDataLatency()
{
    int latencyMs = 0;
    if (mPcm) {
        unsigned int avail;
        struct timespec tstamp;
        if (pcm_get_htimestamp(mPcm, &avail, &tstamp) == 0) {
            latencyMs = avail * 1000 / mConfig.rate;
        } else {
            ALOGW("%s pcm_get_htimestamp fail due to %s.", __FUNCTION__, pcm_get_error(mPcm));
        }
    }
    return latencyMs;
}

uint32_t AudioHfpULTaskSource::getSwMicDigitalGain()
{
    return mAudioALSAVolumeController->GetSWMICGain();
}

uint32_t AudioHfpULTaskSource::getUplinkTotalGain()
{
    return mAudioALSAVolumeController->GetULTotalGain();
}

pcm_format AudioHfpULTaskSource::audioFormatToPcm(int format)
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

}
