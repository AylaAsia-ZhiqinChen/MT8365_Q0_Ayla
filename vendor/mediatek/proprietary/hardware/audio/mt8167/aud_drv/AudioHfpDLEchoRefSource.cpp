#define LOG_TAG "AudioHfpDLEchoRefSource"

#include "AudioHfpDLEchoRefSource.h"
#include "AudioALSADriverUtility.h"
#include "AudioALSADeviceParser.h"
#ifdef NXP_SMARTPA_SUPPORT
#include <mtk_tfa98xx_interface.h>
#endif

namespace android
{

AudioHfpDLEchoRefSource::AudioHfpDLEchoRefSource()
    : mStarted(false),
      mReqChannels(1),
      mOutputDevice(AUDIO_DEVICE_OUT_SPEAKER),
      mPcm(NULL),
      mReadBuffer(NULL),
      mAudioSrc(NULL)
{
    memset(&mConfig, 0, sizeof(mConfig));
}

AudioHfpDLEchoRefSource::~AudioHfpDLEchoRefSource()
{
    if (mPcm) {
        pcm_close(mPcm);
        mPcm = NULL;
    }

    if (mReadBuffer) {
        delete [] mReadBuffer;
        mReadBuffer = NULL;
    }

    if (mAudioSrc != NULL) {
        mAudioSrc->close();
        delete mAudioSrc;
        mAudioSrc = NULL;
    }
}

status_t AudioHfpDLEchoRefSource::prepare(uint32_t inDevice, uint32_t outDevice, int format,
            uint32_t channelCount, uint32_t sampleRate, uint32_t accessSize)
{
    ALOGW_IF((format != AUDIO_FORMAT_PCM_16_BIT), "%s unexpected format %d", __FUNCTION__, format);

    mReqChannels = channelCount;
    mOutputDevice = outDevice;
    (void)inDevice;
#if defined(MTK_CONSYS_MT8167) // 8167 6627
    mConfig.channels = 2;
#else // 8167 7668
    mConfig.channels = 1;
#endif
    mConfig.rate = sampleRate;
    mConfig.period_size = accessSize / (mReqChannels * audio_bytes_per_sample((audio_format_t)format));
#if defined(MTK_CONSYS_MT8167)// 8167 6627
    mConfig.period_count = 8;
#else // 8167 7668
    mConfig.period_count = 4;
#endif
    mConfig.format = PCM_FORMAT_S16_LE;
    mConfig.start_threshold = 0;
    mConfig.stop_threshold = 0;
    mConfig.silence_threshold = 0;
    mConfig.silence_size = 0;
    mConfig.avail_min = 0;

    mReadBytes = mConfig.period_size * mConfig.channels * 2;
    mReadBuffer = new char[mReadBytes];
    if (mReqChannels != mConfig.channels) {
        mAudioSrc = newMtkAudioSrc(mConfig.rate, mConfig.channels,
                                    mConfig.rate, mReqChannels,
                                    SRC_IN_Q1P15_OUT_Q1P15);
        mAudioSrc->open();
    }
    return NO_ERROR;
}

status_t AudioHfpDLEchoRefSource::start()
{
    if (!mStarted)
    {
        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

        unsigned int card = 0;
        unsigned int device = 0;
#if defined(MTK_CONSYS_MT8167) // 8167 6627
#if defined(NXP_SMARTPA_SUPPORT) && defined(EXTCODEC_ECHO_REFERENCE_SUPPORT)
        if (mOutputDevice == AUDIO_DEVICE_OUT_SPEAKER)
        {
            MTK_Tfa98xx_EchoReferenceConfigure(1);

            card = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmI2SAwbCapture);
            device = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmI2SAwbCapture);
        }
        else
#endif
        {
            card = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl1AwbCapture);
            device = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl1AwbCapture);
        }
#else // 8167 7668
        card = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmVOIPCallBTCapture);
        device = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmVOIPCallBTCapture);
#endif
        ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
              __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);
        mPcm = pcm_open(card, device, PCM_IN, &mConfig);
        if (!mPcm) {
            ALOGE("%s pcm_open %u-%u fail", __FUNCTION__, card, device);
            return UNKNOWN_ERROR;
        } else if (!pcm_is_ready(mPcm)) {
            ALOGE("%s(), pcm_is_ready(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            return UNKNOWN_ERROR;
        } else if (pcm_prepare(mPcm) != 0) {
            ALOGE("%s(), pcm_prepare(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            return -1;
        }

        mStarted = true;
    }
    return NO_ERROR;
}

ssize_t AudioHfpDLEchoRefSource::read(void *buffer, ssize_t bytes)
{
    int ret;

    if (!mPcm) {
        ALOGE("%s invalid mPcm", __FUNCTION__);
        return 0;
    }

    if (mAudioSrc) {
        ret = pcm_read(mPcm, mReadBuffer, mReadBytes);
        ALOGE_IF((ret != 0), "%s pcm_read fail ret = %d(%s)", __FUNCTION__, ret, pcm_get_error(mPcm));

        unsigned int inputBytes = mReadBytes;
        unsigned int outputBytes = bytes;
        mAudioSrc->process((int16_t *)mReadBuffer, &inputBytes, (int16_t *)buffer, &outputBytes);

        ALOGE_IF((bytes != (ssize_t)outputBytes), "%s bytes(%d) != outputBytes(%d)",
                 __FUNCTION__, bytes, outputBytes);
        ALOGE_IF((inputBytes != 0), "%s inputBytes(%d) != 0", __FUNCTION__, inputBytes);
    } else {
        ret = pcm_read(mPcm, buffer, bytes);
        ALOGE_IF((ret != 0), "%s pcm_read fail ret = %d(%s)", __FUNCTION__, ret, pcm_get_error(mPcm));
    }

    return bytes;
}

status_t AudioHfpDLEchoRefSource::stop()
{
    if (mStarted)
    {
        if (mPcm) {
            AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

            pcm_stop(mPcm);
            pcm_close(mPcm);
            mPcm = NULL;
        }

        mStarted = false;
    }
    return NO_ERROR;
}

int AudioHfpDLEchoRefSource::getRemainDataLatency()
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

uint32_t AudioHfpDLEchoRefSource::getSwMicDigitalGain()
{
    return 0;
}

uint32_t AudioHfpDLEchoRefSource::getUplinkTotalGain()
{
    return 0;
}

}
