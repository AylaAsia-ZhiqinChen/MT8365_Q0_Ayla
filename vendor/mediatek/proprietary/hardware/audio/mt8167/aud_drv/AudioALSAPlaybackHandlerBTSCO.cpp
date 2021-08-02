#define LOG_TAG "AudioALSAPlaybackHandlerBTSCO"

#include "AudioALSAPlaybackHandlerBTSCO.h"

#include "AudioALSADriverUtility.h"
#include "AudioALSAHardwareResourceManager.h"
#include <AudioLock.h>

#include "WCNChipController.h"


#if defined(CONFIG_MT_ENG_BUILD)
#define DEBUG_LATENCY
#endif

#define calc_time_diff(x,y) ((x.tv_sec - y.tv_sec )+ (double)( x.tv_nsec - y.tv_nsec ) / (double)1000000000)

namespace android
{

AudioALSAPlaybackHandlerBTSCO::AudioALSAPlaybackHandlerBTSCO(const stream_attribute_t *stream_attribute_source) :
    AudioALSAPlaybackHandlerBase(stream_attribute_source),
    mWCNChipController(WCNChipController::GetInstance())
{
    ALOGD("%s()", __FUNCTION__);
    mPlaybackHandlerType = PLAYBACK_HANDLER_BT_SCO;
    memset(&mNewtime, 0, sizeof(mNewtime));
    memset(&mOldtime, 0, sizeof(mOldtime));
}


AudioALSAPlaybackHandlerBTSCO::~AudioALSAPlaybackHandlerBTSCO()
{
    ALOGD("%s()", __FUNCTION__);
}


status_t AudioALSAPlaybackHandlerBTSCO::open()
{
    ALOGD("+%s(), mDevice = 0x%x", __FUNCTION__, mStreamAttributeSource->output_devices);
    AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

    int pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl1Meida);
    int cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl1Meida);

#ifdef PLAYBACK_USE_24BITS_ONLY
    mStreamAttributeTarget.audio_format = AUDIO_FORMAT_PCM_8_24_BIT;
#else
    mStreamAttributeTarget.audio_format = (mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_32_BIT) ?
                                           AUDIO_FORMAT_PCM_8_24_BIT : AUDIO_FORMAT_PCM_16_BIT;
#endif
    mStreamAttributeTarget.audio_channel_mask = AUDIO_CHANNEL_OUT_STEREO;
    mStreamAttributeTarget.num_channels = popcount(mStreamAttributeTarget.audio_channel_mask);
    mStreamAttributeTarget.sample_rate = mWCNChipController->GetBTCurrentSamplingRateNumber();

    if (mStreamAttributeTarget.sample_rate == mStreamAttributeSource->sample_rate) {
        mStreamAttributeTarget.buffer_size = mStreamAttributeSource->buffer_size;
    } else {
        mStreamAttributeTarget.buffer_size = mStreamAttributeSource->buffer_size * mStreamAttributeTarget.sample_rate /
                                             mStreamAttributeSource->sample_rate;
        /* 64 bytes alignedment */
        mStreamAttributeTarget.buffer_size += 0x3f;
        mStreamAttributeTarget.buffer_size &= ~0x3f;
    }

    if ((audio_bytes_per_sample(mStreamAttributeTarget.audio_format) !=
         audio_bytes_per_sample(mStreamAttributeSource->audio_format)) &&
        (audio_bytes_per_sample(mStreamAttributeSource->audio_format) > 0))
    {
        mStreamAttributeTarget.buffer_size = mStreamAttributeTarget.buffer_size * audio_bytes_per_sample(mStreamAttributeTarget.audio_format) /
                                             audio_bytes_per_sample(mStreamAttributeSource->audio_format);
        /* 64 bytes alignedment */
        mStreamAttributeTarget.buffer_size += 0x3f;
        mStreamAttributeTarget.buffer_size &= ~0x3f;
    }

    // HW pcm config
    memset(&mConfig, 0, sizeof(mConfig));
    mConfig.channels = mStreamAttributeTarget.num_channels;
    mConfig.rate = mStreamAttributeTarget.sample_rate;
    mConfig.period_count = 2;
    mConfig.period_size = (mStreamAttributeTarget.buffer_size / (mConfig.channels * mConfig.period_count)) /
                           audio_bytes_per_sample(mStreamAttributeTarget.audio_format);
    mConfig.format = (mStreamAttributeTarget.audio_format == AUDIO_FORMAT_PCM_8_24_BIT) ?
                     PCM_FORMAT_S24_LE : transferAudioFormatToPcmFormat(mStreamAttributeTarget.audio_format);
    mConfig.start_threshold = 0;
    mConfig.stop_threshold = 0;
    mConfig.silence_threshold = 0;
    mConfig.avail_min = 0;

    ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
          __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);

    // SRC
    initBliSrc();

    // bit conversion
    initBitConverter();

    initDataPending();

    // configure route path
    mHardwareResourceManager->setRoutePath(ROUTE_NORMAL_PLAYBACK, mStreamAttributeSource->output_devices);

    // open pcm driver
    openPcmDriver(pcmindex);

    // debug pcm dump
    OpenPCMDump(LOG_TAG);

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAPlaybackHandlerBTSCO::close()
{
    ALOGD("+%s()", __FUNCTION__);
    AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

    // close pcm driver
    closePcmDriver();

    // reset route path
    mHardwareResourceManager->resetRoutePath(ROUTE_NORMAL_PLAYBACK, mStreamAttributeSource->output_devices);

    DeinitDataPending();


    // bit conversion
    deinitBitConverter();


    // SRC
    deinitBliSrc();


    // debug pcm dump
    ClosePCMDump();


    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAPlaybackHandlerBTSCO::routing(const audio_devices_t output_devices __unused)
{
    return INVALID_OPERATION;
}

ssize_t AudioALSAPlaybackHandlerBTSCO::write(const void *buffer, size_t bytes)
{
    ALOGV("%s(), buffer = %p, bytes = %zu", __FUNCTION__, buffer, bytes);

    if (mPcm == NULL)
    {
        ALOGE("%s(), mPcm == NULL, return", __FUNCTION__);
        return bytes;
    }

    // const -> to non const
    void *pBuffer = const_cast<void *>(buffer);
    ASSERT(pBuffer != NULL);

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[0] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

    // SRC
    void *pBufferAfterBliSrc = NULL;
    uint32_t bytesAfterBliSrc = 0;
    doBliSrc(pBuffer, bytes, &pBufferAfterBliSrc, &bytesAfterBliSrc);


    // bit conversion
    void *pBufferAfterBitConvertion = NULL;
    uint32_t bytesAfterBitConvertion = 0;
    doBitConversion(pBufferAfterBliSrc, bytesAfterBliSrc, &pBufferAfterBitConvertion, &bytesAfterBitConvertion);


    // data pending
    void *pBufferAfterPending = NULL;
    uint32_t bytesAfterpending = 0;
    dodataPending(pBufferAfterBitConvertion, bytesAfterBitConvertion, &pBufferAfterPending, &bytesAfterpending);

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[1] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

    // write data to pcm driver
    WritePcmDumpData(pBufferAfterPending, bytesAfterpending);
    int retval = pcm_write(mPcm, pBufferAfterPending, bytesAfterpending);


#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[2] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif


    if (retval != 0)
    {
        ALOGE("%s(), pcm_write() error, retval = %d", __FUNCTION__, retval);
    }

#ifdef DEBUG_LATENCY
    ALOGD("AudioALSAPlaybackHandlerNormal::write (-) latency_in_us,%1.6lf,%1.6lf,%1.6lf", latencyTime[0], latencyTime[1], latencyTime[2]);
#endif


    return bytes;
}


} // end of namespace android
