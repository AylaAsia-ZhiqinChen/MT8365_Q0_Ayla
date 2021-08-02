#include "AudioALSAPlaybackHandlerHfpClient.h"
#include "AudioHfpController.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "AudioALSAPlaybackHandlerHfpClient"


#if defined(CONFIG_MT_ENG_BUILD)
#define DEBUG_LATENCY
#endif

#define calc_time_diff(x,y) ((x.tv_sec - y.tv_sec )+ (double)( x.tv_nsec - y.tv_nsec ) / (double)1000000000)

namespace android
{

AudioALSAPlaybackHandlerHfpClient::AudioALSAPlaybackHandlerHfpClient(const stream_attribute_t *stream_attribute_source)
    : AudioALSAPlaybackHandlerBase(stream_attribute_source),
      mAudioHfpController(AudioHfpController::getInstance())
{
    ALOGD("%s()", __FUNCTION__);
    mPlaybackHandlerType = PLAYBACK_HANDLER_HFP_CLIENT;
}

AudioALSAPlaybackHandlerHfpClient::~AudioALSAPlaybackHandlerHfpClient()
{
    ALOGD("%s()", __FUNCTION__);
}

status_t AudioALSAPlaybackHandlerHfpClient::open()
{
    ALOGD("+%s() mDevice = 0x%x", __FUNCTION__, mStreamAttributeSource->output_devices);

    OpenPCMDump(LOG_TAG);

    mStreamAttributeTarget.audio_format = AUDIO_FORMAT_PCM_16_BIT;
    mStreamAttributeTarget.audio_channel_mask = AUDIO_CHANNEL_OUT_STEREO;
    mStreamAttributeTarget.num_channels = popcount(mStreamAttributeTarget.audio_channel_mask);
    mStreamAttributeTarget.sample_rate = 48000;//(mStreamAttributeSource->sample_rate > 48000) ? 48000 : mStreamAttributeSource->sample_rate;

    initBliSrc();

    initBitConverter();

    mAudioHfpController->DLTaskBgsDataStart(mStreamAttributeTarget.sample_rate,
                                                          mStreamAttributeTarget.num_channels,
                                                          mStreamAttributeTarget.audio_format);

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerHfpClient::close()
{
    ALOGD("+%s()", __FUNCTION__);

    mAudioHfpController->DLTaskBgsDataStop();

    deinitBitConverter();

    deinitBliSrc();

    ClosePCMDump();

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerHfpClient::routing(const audio_devices_t output_devices __unused)
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHfpClient::pause()
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHfpClient::resume()
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHfpClient::flush()
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHfpClient::setVolume(uint32_t vol __unused)
{
    return INVALID_OPERATION;
}

int AudioALSAPlaybackHandlerHfpClient::drain(audio_drain_type_t type __unused)
{
    return 0;
}

ssize_t AudioALSAPlaybackHandlerHfpClient::write(const void *buffer, size_t bytes)
{
    ALOGV("+%s(), buffer = %p, bytes = %d", __FUNCTION__, buffer, bytes);

    // const -> to non const
    void *pBuffer = const_cast<void *>(buffer);
    ASSERT(pBuffer != NULL);

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[0] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

    // stereo to mono for speaker
    // AudioMixer will perform stereo to mono when 32-bit
    if (mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_16_BIT) {
        doStereoToMonoConversionIfNeed(pBuffer, bytes);
    }

    // SRC
    void *pBufferAfterBliSrc = NULL;
    uint32_t bytesAfterBliSrc = 0;
    doBliSrc(pBuffer, bytes, &pBufferAfterBliSrc, &bytesAfterBliSrc);

    // bit conversion
    void *pBufferAfterBitConvertion = NULL;
    uint32_t bytesAfterBitConvertion = 0;
    doBitConversion(pBufferAfterBliSrc, bytesAfterBliSrc, &pBufferAfterBitConvertion, &bytesAfterBitConvertion);

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[1] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

    WritePcmDumpData(pBufferAfterBitConvertion, bytesAfterBitConvertion);

    int retval = mAudioHfpController->DLTaskBgsDataWrite(pBufferAfterBitConvertion, bytesAfterBitConvertion);

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[2] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

    if (retval < 0) {
        ALOGE("%s() error retval = %d", __FUNCTION__, retval);
    }

#ifdef DEBUG_LATENCY
    ALOGD("-%s latency_in_us,%1.6lf,%1.6lf,%1.6lf", __FUNCTION__, latencyTime[0], latencyTime[1], latencyTime[2]);
#endif

    return bytes;
}

} // end of namespace android
