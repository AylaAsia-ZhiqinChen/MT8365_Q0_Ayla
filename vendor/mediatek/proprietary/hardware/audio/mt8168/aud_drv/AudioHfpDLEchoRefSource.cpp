#define LOG_TAG "AudioHfpDLEchoRefSource"

#include "AudioHfpDLEchoRefSource.h"
#include "AudioALSADriverUtility.h"
#include "AudioALSADeviceParser.h"

#include "AudioUtility.h"
#include "MtkAudioComponent.h"

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
        deleteMtkAudioSrc(mAudioSrc);
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

    mConfig.channels = 2;
    mConfig.rate = sampleRate;
    mConfig.period_size = accessSize / (mReqChannels * audio_bytes_per_sample((audio_format_t)format));
    mConfig.period_count = 8;
    mConfig.format = PCM_FORMAT_S16_LE;
    mConfig.start_threshold = 0;
    mConfig.stop_threshold = 0;
    mConfig.silence_threshold = 0;
    mConfig.avail_min = 0;

    if (mReqChannels != mConfig.channels) {
        mReadBytes = mConfig.period_size * mConfig.channels * 2;
        mReadBuffer = new char[mReadBytes];

        mAudioSrc = newMtkAudioSrc(mConfig.rate, mConfig.channels,
                                    mConfig.rate, mReqChannels,
                                    SRC_IN_Q1P15_OUT_Q1P15);

		ASSERT(mAudioSrc != NULL);
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

        mPcm = pcm_open(card, device, PCM_IN | PCM_MONOTONIC, &mConfig);
        if (!mPcm) {
            ALOGE("%s pcm_open %u-%u fail", __FUNCTION__, card, device);
            return UNKNOWN_ERROR;
        } else if (!pcm_is_ready(mPcm)) {
            pcm_close(mPcm);
            mPcm = NULL;
            ALOGE("%s pcm_is_ready fail %s", __FUNCTION__, pcm_get_error(mPcm));
            return UNKNOWN_ERROR;
        }

        pcm_start(mPcm);

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
