#define LOG_TAG "AudioALSAPlaybackHandlerHDMI"

#include "AudioALSAPlaybackHandlerHDMI.h"
#include "AudioALSADriverUtility.h"
#include "HDMITxController.h"

#include "AudioTypeExt.h"


#define calc_time_diff(x,y) ((x.tv_sec - y.tv_sec )+ (double)( x.tv_nsec - y.tv_nsec ) / (double)1000000000)


namespace android
{
//fixme
AudioALSAPlaybackHandlerHDMI::AudioALSAPlaybackHandlerHDMI(const stream_attribute_t *stream_attribute_source)
    : AudioALSAPlaybackHandlerBase(stream_attribute_source)
    /*, mSpdifEncoder(NULL)*/
    , mWrapperNeeded(false)
{
    ALOGD("%s()", __FUNCTION__);
    mPlaybackHandlerType = PLAYBACK_HANDLER_HDMI;
}

AudioALSAPlaybackHandlerHDMI::~AudioALSAPlaybackHandlerHDMI()
{
    ALOGD("%s()", __FUNCTION__);
}

status_t AudioALSAPlaybackHandlerHDMI::open()
{
    ALOGD("+%s(), output_devices = 0x%x audio_format = 0x%x", __FUNCTION__,
          mStreamAttributeSource->output_devices, mStreamAttributeSource->audio_format);

    if ((mStreamAttributeSource->audio_format == AUDIO_FORMAT_IEC61937) ||
        (mStreamAttributeSource->mAudioOutputFlags & AUDIO_OUTPUT_FLAG_IEC958_NONAUDIO)) {
        mStreamAttributeTarget.audio_format = AUDIO_FORMAT_PCM_16_BIT;
        mStreamAttributeTarget.audio_channel_mask = mStreamAttributeSource->audio_channel_mask;
        mWrapperNeeded = false;
    } else if (!audio_has_proportional_frames(mStreamAttributeSource->audio_format)) {
        mStreamAttributeTarget.audio_format = AUDIO_FORMAT_PCM_16_BIT;
        mStreamAttributeTarget.audio_channel_mask = AUDIO_CHANNEL_OUT_STEREO;
        //fixme
        /*mSpdifEncoder = new IecSPDIFEncoder(this, mStreamAttributeSource->audio_format);*/
        mWrapperNeeded = true;
    } else { // PCM
        mStreamAttributeTarget.audio_format = (mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_32_BIT ||
                                               mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_8_24_BIT) ?
                                               AUDIO_FORMAT_PCM_8_24_BIT : AUDIO_FORMAT_PCM_16_BIT;
        mStreamAttributeTarget.audio_channel_mask = mStreamAttributeSource->audio_channel_mask;
        mWrapperNeeded = false;
    }

    mStreamAttributeTarget.num_channels = popcount(mStreamAttributeTarget.audio_channel_mask);

    if (mStreamAttributeSource->audio_format == AUDIO_FORMAT_E_AC3) {
        mStreamAttributeTarget.sample_rate = mStreamAttributeSource->sample_rate * 4;
        mStreamAttributeTarget.buffer_size = mStreamAttributeSource->buffer_size * 4;
    } else {
        mStreamAttributeTarget.sample_rate = mStreamAttributeSource->sample_rate; // same as source stream
        mStreamAttributeTarget.buffer_size = mStreamAttributeSource->buffer_size;
    }

    // HW pcm config
    memset(&mConfig, 0, sizeof(mConfig));
    mConfig.channels = mStreamAttributeTarget.num_channels;
    mConfig.rate = mStreamAttributeTarget.sample_rate;
    mConfig.period_count = 2;
    mConfig.period_size = mStreamAttributeTarget.buffer_size / (mConfig.channels * mConfig.period_count *
                          audio_bytes_per_sample(mStreamAttributeTarget.audio_format));
    mConfig.format = (mStreamAttributeTarget.audio_format == AUDIO_FORMAT_PCM_8_24_BIT) ?
                     PCM_FORMAT_S24_LE : transferAudioFormatToPcmFormat(mStreamAttributeTarget.audio_format);
    mConfig.start_threshold = 0;
    mConfig.stop_threshold = 0;
    mConfig.silence_threshold = 0;
    mConfig.avail_min = 0;

    ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
          __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);

    unsigned int pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmHDMI);

    if (audio_is_linear_pcm(mStreamAttributeSource->audio_format) &&
        !(mStreamAttributeSource->mAudioOutputFlags & AUDIO_OUTPUT_FLAG_IEC958_NONAUDIO)) {
        // bit conversion
        initBitConverter();
    }

    // open pcm driver
    openPcmDriver(pcmindex);

    OpenPCMDump(LOG_TAG);

    HDMITxController::notifyAudioSetting(mStreamAttributeSource->audio_format,
                                         mStreamAttributeTarget.sample_rate,
                                         mStreamAttributeTarget.audio_channel_mask);

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAPlaybackHandlerHDMI::close()
{
    ALOGD("+%s()", __FUNCTION__);

    // close pcm driver
    closePcmDriver();

    // bit conversion
    deinitBitConverter();

    ClosePCMDump();
//fixme
/*
    if (mSpdifEncoder) {
        delete mSpdifEncoder;
        mSpdifEncoder = NULL;
    }
*/
    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAPlaybackHandlerHDMI::routing(const audio_devices_t output_devices __unused)
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHDMI::pause()
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHDMI::resume()
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHDMI::flush()
{
    return 0;
//fixme
/*
    if (mSpdifEncoder)
        mSpdifEncoder->reset();

    resetBytesWriteKernel();
    return NO_ERROR;
*/
}

status_t AudioALSAPlaybackHandlerHDMI::setVolume(uint32_t vol __unused)
{
    return INVALID_OPERATION;
}

int AudioALSAPlaybackHandlerHDMI::drain(audio_drain_type_t type __unused)
{
    return 0;
}

ssize_t AudioALSAPlaybackHandlerHDMI::write(const void *buffer, size_t bytes)
{
    ALOGV("%s(), buffer = %p, bytes = %zu", __FUNCTION__, buffer, bytes);

    if (mPcm == NULL)
    {
        ALOGE("%s(), mPcm == NULL, return", __FUNCTION__);
        return bytes;
    }

    // work around for incall mode HDMI is playing
    if (mStreamAttributeSource->audio_mode ==  AUDIO_MODE_IN_CALL)
    {
        usleep(30 * 1000);
        return bytes;
    }

    // const -> to non const
    void *pBuffer = const_cast<void *>(buffer);
    ASSERT(pBuffer != NULL);

    if (mWrapperNeeded)
    {
//fixme
        /*ASSERT(mSpdifEncoder != NULL);*/
        return 0;/*mSpdifEncoder->write(buffer, bytes);*/
    }

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[0] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

    // bit conversion
    void *pBufferAfterBitConvertion = NULL;
    uint32_t bytesAfterBitConvertion = 0;
    doBitConversion(pBuffer, bytes, &pBufferAfterBitConvertion, &bytesAfterBitConvertion);

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[1] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

    writeKernel(pBufferAfterBitConvertion, bytesAfterBitConvertion);

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[2] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

#ifdef DEBUG_LATENCY
    ALOGD("AudioALSAPlaybackHandlerHDMI::write (-) latency_in_us,%1.6lf,%1.6lf,%1.6lf",
          latencyTime[0], latencyTime[1], latencyTime[2]);
#endif

    return bytes;
}

status_t AudioALSAPlaybackHandlerHDMI::getHardwareBufferInfo(time_info_struct_t *HWBuffer_Time_Info)
{
    status_t ret = AudioALSAPlaybackHandlerBase::getHardwareBufferInfo(HWBuffer_Time_Info);

    // consider rate multipler for E-AC3
    if (mStreamAttributeSource->audio_format == AUDIO_FORMAT_E_AC3) {
        HWBuffer_Time_Info->buffer_per_time >>= 2;
        HWBuffer_Time_Info->frameInfo_get >>= 2;
    }

    return ret;
}

uint64_t AudioALSAPlaybackHandlerHDMI::getBytesWriteKernel()
{
    if (mStreamAttributeSource->audio_format == AUDIO_FORMAT_E_AC3)
        return mBytesWriteKernel >> 2;
    else
        return mBytesWriteKernel;
}

void AudioALSAPlaybackHandlerHDMI::forceClkOn(const stream_attribute_t *stream_attribute_source)
{
    ALOGV("+%s()", __FUNCTION__);

    struct mixer *mixerInstance = AudioALSADriverUtility::getInstance()->getMixer();
    if (mixer_ctl_set_value(mixer_get_ctl_by_name(mixerInstance, "HDMI_Force_Clk_Switch"), 0 ,1) < 0)
        ALOGW("%s mixer_ctl_set_enum_by_string HDMI_Force_Clk_Switch On failed", __FUNCTION__);

    // trigger clock output in advance
    AudioALSAPlaybackHandlerHDMI *handler = new AudioALSAPlaybackHandlerHDMI(stream_attribute_source);
    handler->open();
    handler->close();
    delete handler;

    ALOGV("-%s()", __FUNCTION__);
}

void AudioALSAPlaybackHandlerHDMI::forceClkOff()
{
    ALOGV("+%s()", __FUNCTION__);

    struct mixer *mixerInstance = AudioALSADriverUtility::getInstance()->getMixer();
    if (mixer_ctl_set_value(mixer_get_ctl_by_name(mixerInstance, "HDMI_Force_Clk_Switch"), 0 ,0) < 0)
        ALOGW("%s mixer_ctl_set_enum_by_string HDMI_Force_Clk_Switch Off failed", __FUNCTION__);

    ALOGV("-%s()", __FUNCTION__);
}

ssize_t AudioALSAPlaybackHandlerHDMI::writeDataBurst(const void* buffer, size_t bytes)
{
    return writeKernel(buffer, bytes);
}

ssize_t AudioALSAPlaybackHandlerHDMI::writeKernel(const void* buffer, size_t bytes)
{
    // write data to pcm driver
    int retval = pcm_write(mPcm, buffer, bytes);
    ALOGE_IF(retval != 0, "%s(), pcm_write() error, retval = %d", __FUNCTION__, retval);

    WritePcmDumpData(buffer, bytes);

    mBytesWriteKernel += bytes;

    return bytes;
}


} // end of namespace android
