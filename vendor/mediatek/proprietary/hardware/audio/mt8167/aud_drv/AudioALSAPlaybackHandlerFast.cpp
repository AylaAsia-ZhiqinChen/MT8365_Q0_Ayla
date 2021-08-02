#define LOG_TAG "AudioALSAPlaybackHandlerFast"

#include "AudioALSAPlaybackHandlerFast.h"

#include "AudioALSAHardwareResourceManager.h"
//#include "AudioALSAVolumeController.h"
//#include "AudioVolumeInterface.h"
#include "AudioVolumeFactory.h"
#include "AudioALSASampleRateController.h"

#include "AudioMTKFilter.h"

#include "AudioALSADeviceParser.h"
#include "AudioALSADriverUtility.h"



// Latency Detect
//#define DEBUG_LATENCY
#define THRESHOLD_FRAMEWORK   0.002
#define THRESHOLD_HAL         0.002
#define THRESHOLD_KERNEL      0.002

#define calc_time_diff(x,y) ((x.tv_sec - y.tv_sec )+ (double)( x.tv_nsec - y.tv_nsec ) / (double)1000000000)
static const char* PROPERTY_KEY_EXTDAC = "vendor.af.resouce.extdac_support";

namespace android
{

AudioALSAPlaybackHandlerFast::AudioALSAPlaybackHandlerFast(const stream_attribute_t *stream_attribute_source) :
    AudioALSAPlaybackHandlerBase(stream_attribute_source)
{
    ALOGD("%s()", __FUNCTION__);
    mPlaybackHandlerType = PLAYBACK_HANDLER_FAST;

    memset(&mNewtime, 0, sizeof(mNewtime));
    memset(&mOldtime, 0, sizeof(mOldtime));
}


AudioALSAPlaybackHandlerFast::~AudioALSAPlaybackHandlerFast()
{
    ALOGD("%s()", __FUNCTION__);
}

uint32_t AudioALSAPlaybackHandlerFast::GetLowJitterModeSampleRate()
{
    return 48000;
}

bool AudioALSAPlaybackHandlerFast::SetLowJitterMode(bool bEnable,uint32_t SampleRate)
{
    ALOGD("%s() bEanble = %d SampleRate = %u", __FUNCTION__, bEnable,SampleRate);

    enum mixer_ctl_type type;
    struct mixer_ctl *ctl;
    int retval = 0;

    // check need open low jitter mode
    if(SampleRate <= GetLowJitterModeSampleRate() && (AudioALSADriverUtility::getInstance()->GetPropertyValue(PROPERTY_KEY_EXTDAC)) == false)
    {
        ALOGD("%s(), bEanble = %d", __FUNCTION__, bEnable);
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

bool AudioALSAPlaybackHandlerFast::DeviceSupportHifi(audio_devices_t outputdevice)
{
    // modify this to let output device support hifi audio
    if (outputdevice == AUDIO_DEVICE_OUT_WIRED_HEADSET || outputdevice == AUDIO_DEVICE_OUT_WIRED_HEADPHONE)
    {
        return true;
    }
    else if (outputdevice & AUDIO_DEVICE_OUT_SPEAKER)
    {
#ifdef SMART_PA_SUPPORT
        return AudioSmartPaController::getInstance()->getMaxSupportedRate() > 48000;
#else
        return true;
#endif
    }
    return false;
}


uint32_t AudioALSAPlaybackHandlerFast::ChooseTargetSampleRate(uint32_t SampleRate, audio_devices_t outputdevice)
{
    ALOGD("ChooseTargetSampleRate SampleRate = %d outputdevice = %d",SampleRate,outputdevice);
    uint32_t TargetSampleRate = 44100;
    if (SampleRate <=  192000 && SampleRate > 96000 && DeviceSupportHifi(outputdevice))
    {
        TargetSampleRate = 192000;
    }
    else if (SampleRate <= 96000 && SampleRate > 48000 && DeviceSupportHifi(outputdevice))
    {
        TargetSampleRate = 96000;
    }
    else if (SampleRate <= 48000 && SampleRate >= 32000)
    {
        TargetSampleRate = SampleRate;
    }
    return TargetSampleRate;
}

status_t AudioALSAPlaybackHandlerFast::open()
{
    ALOGD("+%s(), mDevice = 0x%x", __FUNCTION__, mStreamAttributeSource->output_devices);
    AL_LOCK_MS(AudioALSADriverUtility::getInstance()->getStreamSramDramLock(), 3000);

    // acquire pmic clk
    mHardwareResourceManager->EnableAudBufClk(true);

    int pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl2Meida);
    int cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl2Meida);

#ifdef PLAYBACK_USE_24BITS_ONLY
    mStreamAttributeTarget.audio_format = AUDIO_FORMAT_PCM_8_24_BIT;
#else
    mStreamAttributeTarget.audio_format = (mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_32_BIT) ?
                                          AUDIO_FORMAT_PCM_8_24_BIT : AUDIO_FORMAT_PCM_16_BIT;
#endif
    mStreamAttributeTarget.audio_channel_mask = AUDIO_CHANNEL_OUT_STEREO;
    mStreamAttributeTarget.num_channels = popcount(mStreamAttributeTarget.audio_channel_mask);

    mStreamAttributeTarget.sample_rate = ChooseTargetSampleRate(AudioALSASampleRateController::getInstance()->getPrimaryStreamOutSampleRate(),
                                                                mStreamAttributeSource->output_devices);

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

    ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d, buffer size %d %d",
          __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format,
          mStreamAttributeTarget.buffer_size, mStreamAttributeSource->buffer_size);

    mStreamAttributeTarget.mInterrupt = (mConfig.period_size+0.0) / mStreamAttributeTarget.sample_rate;

    // post processing
    initPostProcessing();

    // SRC
    initBliSrc();

    // bit conversion
    initBitConverter();

    // poweraq processing
#ifdef MTK_POWERAQ_SUPPORT
    initPowerAQProcessing(1/*PPC_HANDLER_FAST*/);
#endif

    initDataPending();

#if defined(MTK_HYBRID_NLE_SUPPORT)
    mStreamAttributeTarget.output_devices = mStreamAttributeSource->output_devices;
    initNLEProcessing();
#endif

    // disable lowjitter mode
    //SetLowJitterMode(true, mStreamAttributeTarget.sample_rate);

    // configure codec path
    mHardwareResourceManager->startOutputDevice(mStreamAttributeSource->output_devices, mStreamAttributeTarget.sample_rate);

    // configure route path
    mHardwareResourceManager->setRoutePath(ROUTE_LOW_LATENCY_PLAYBACK, mStreamAttributeSource->output_devices);

    // open pcm driver
    openPcmDriver(pcmindex);
    AL_UNLOCK(AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

 

    // debug pcm dump
    OpenPCMDump(LOG_TAG);

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAPlaybackHandlerFast::close()
{
    ALOGD("+%s()", __FUNCTION__);
    AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

 

#if defined(MTK_HYBRID_NLE_SUPPORT)
    // Must do this before close analog path
    deinitNLEProcessing();
#endif

    // close pcm driver
    closePcmDriver();

    // reset route path
    mHardwareResourceManager->resetRoutePath(ROUTE_LOW_LATENCY_PLAYBACK, mStreamAttributeSource->output_devices);

    // reset codec path
    mHardwareResourceManager->stopOutputDevice();

    // disable lowjitter mode
    //SetLowJitterMode(false, mStreamAttributeTarget.sample_rate);

    DeinitDataPending();

    // poweraq processing
#ifdef MTK_POWERAQ_SUPPORT
    deinitPowerAQProcessing(1/*PPC_HANDLER_FAST*/);
#endif

    // bit conversion
    deinitBitConverter();

    // SRC
    deinitBliSrc();

    // post processing
    deinitPostProcessing();

    // debug pcm dump
    ClosePCMDump();

    //release pmic clk
    mHardwareResourceManager->EnableAudBufClk(false);

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAPlaybackHandlerFast::routing(const audio_devices_t output_devices)
{
    mHardwareResourceManager->changeOutputDevice(output_devices);
    if (mAudioFilterManagerHandler) { mAudioFilterManagerHandler->setDevice(output_devices); }
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerFast::setScreenState(bool mode __unused, size_t buffer_size __unused,
    size_t reduceInterruptSize __unused, bool bforce __unused)
{
    return NO_ERROR;
}

ssize_t AudioALSAPlaybackHandlerFast::write(const void *buffer, size_t bytes)
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

    // stereo to mono for speaker
    doStereoToMonoConversionIfNeed(pBuffer, bytes);


    // post processing (can handle both Q1P16 and Q1P31 by audio_format_t)
    void *pBufferAfterPostProcessing = NULL;
    uint32_t bytesAfterPostProcessing = 0;
    doPostProcessing(pBuffer, bytes, &pBufferAfterPostProcessing, &bytesAfterPostProcessing);


    // SRC
    void *pBufferAfterBliSrc = NULL;
    uint32_t bytesAfterBliSrc = 0;
    doBliSrc(pBufferAfterPostProcessing, bytesAfterPostProcessing, &pBufferAfterBliSrc, &bytesAfterBliSrc);


    // bit conversion
    void *pBufferAfterBitConvertion = NULL;
    uint32_t bytesAfterBitConvertion = 0;
    doBitConversion(pBufferAfterBliSrc, bytesAfterBliSrc, &pBufferAfterBitConvertion, &bytesAfterBitConvertion);

    // poweraq processing (can handle both Q1P16 and Q1P31 by audio_format_t)
#ifdef MTK_POWERAQ_SUPPORT
    void *pBufferAfterPowerAQProcessing = NULL;
    uint32_t bytesAfterPowerAQProcessing = 0;
    int latency = 0;
    doPowerAQProcessing(pBufferAfterBitConvertion, bytesAfterBitConvertion, &pBufferAfterPowerAQProcessing, &bytesAfterPowerAQProcessing, &latency);
    // update powerAQ render delay no use now
    //updatePowerAQRenderDelay(latency);

    // data pending
    void *pBufferAfterPending = NULL;
    uint32_t bytesAfterpending = 0;
    dodataPending(pBufferAfterPowerAQProcessing, bytesAfterPowerAQProcessing, &pBufferAfterPending, &bytesAfterpending);
#else
    // data pending
    void *pBufferAfterPending = NULL;
    uint32_t bytesAfterpending = 0;
    dodataPending(pBufferAfterBitConvertion, bytesAfterBitConvertion, &pBufferAfterPending, &bytesAfterpending);
#endif

    // pcm dump
    WritePcmDumpData(pBufferAfterPending, bytesAfterpending);

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[1] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

#ifdef MTK_LATENCY_DETECT_PULSE
    detectPulse(5, 800, 0, (void *)pBufferAfterPending, bytesAfterpending/mStreamAttributeTarget.num_channels/((mStreamAttributeTarget.audio_format == AUDIO_FORMAT_PCM_16_BIT) ? 2 : 4),
                 mStreamAttributeTarget.audio_format, mStreamAttributeTarget.num_channels, mStreamAttributeTarget.sample_rate);
#endif

    //dataTransferBeforeWrite(pBufferAfterPending, bytesAfterpending);
    // write data to pcm driver
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
    if(latencyTime[0]>THRESHOLD_FRAMEWORK || latencyTime[1]>THRESHOLD_HAL || latencyTime[2]>(mStreamAttributeTarget.mInterrupt-latencyTime[0]-latencyTime[1]+THRESHOLD_KERNEL))
    {
        ALOGD("latency_in_s,%1.3lf,%1.3lf,%1.3lf, interrupt,%1.3lf", latencyTime[0], latencyTime[1], latencyTime[2], mStreamAttributeTarget.mInterrupt);
    }
#endif

    return bytes;
}


status_t AudioALSAPlaybackHandlerFast::setFilterMng(AudioMTKFilterManager *pFilterMng)
{
    ALOGD("+%s() mAudioFilterManagerHandler [%p]", __FUNCTION__, pFilterMng);
    mAudioFilterManagerHandler = pFilterMng;
    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerFast::dataTransferBeforeWrite(void *addr, uint32_t size)
{
    static bool bSupport = true;

    //ALOGD("+%s() addr 0x%x", __FUNCTION__, (long long) addr);

    if (bSupport)
    {
        struct mixer_ctl *ctl = mixer_get_ctl_by_name(mMixer, "Audio_DL2_DataTransfer");
        if (ctl == NULL)
        {
            bSupport = false;
            return NO_ERROR;
        }

        int array[2] = {(int)((long long) addr), (int)size};
        int retval = mixer_ctl_set_array(ctl, array, 2);
        ASSERT(retval == 0);
    }
    return NO_ERROR;
}

} // end of namespace android
