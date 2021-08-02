#define LOG_TAG "AudioALSAPlaybackHandlerNormal"

#include "AudioALSAPlaybackHandlerNormal.h"

#include "AudioALSAHardwareResourceManager.h"
//#include "AudioALSAVolumeController.h"
//#include "AudioVolumeInterface.h"
#include "AudioVolumeFactory.h"
#include "AudioALSASampleRateController.h"

#include "AudioMTKFilter.h"
#include "AudioALSADeviceParser.h"
#include "AudioALSADriverUtility.h"

#undef MTK_HDMI_SUPPORT

#if defined(MTK_HDMI_SUPPORT)
#include "AudioExtDisp.h"
#endif

#ifdef MTK_DYNAMIC_BUFFER_SIZE_SUPPORT
#include "AudioALSAStreamManager.h"
#endif

#ifdef DEBUG_LATENCY
// Latency Detect
//#define DEBUG_LATENCY
#define THRESHOLD_FRAMEWORK   0.010
#define THRESHOLD_HAL         0.010
#define THRESHOLD_KERNEL      0.010
#endif

#define calc_time_diff(x,y) ((x.tv_sec - y.tv_sec )+ (double)( x.tv_nsec - y.tv_nsec ) / (double)1000000000)
static const char* PROPERTY_KEY_EXTDAC = "vendor.af.resouce.extdac_support";

namespace android
{

AudioALSAPlaybackHandlerNormal::AudioALSAPlaybackHandlerNormal(const stream_attribute_t *stream_attribute_source) :
    AudioALSAPlaybackHandlerBase(stream_attribute_source),
    mHpImpeDancePcm(NULL),
    mForceMute(false),
    mCurMuteBytes(0),
    mStartMuteBytes(0),
    mAllZeroBlock(NULL)
{
    ALOGD("%s()", __FUNCTION__);
    mPlaybackHandlerType = PLAYBACK_HANDLER_NORMAL;

    memset((void *)&mNewtime, 0, sizeof(mNewtime));
    memset((void *)&mOldtime, 0, sizeof(mOldtime));
    memset((void *)&mHpImpedanceConfig, 0, sizeof(mHpImpedanceConfig));
}

AudioALSAPlaybackHandlerNormal::~AudioALSAPlaybackHandlerNormal()
{
    ALOGD("%s()", __FUNCTION__);
}

uint32_t AudioALSAPlaybackHandlerNormal::GetLowJitterModeSampleRate()
{
    return 48000;
}

bool AudioALSAPlaybackHandlerNormal::SetLowJitterMode(bool bEnable,uint32_t SampleRate)
{
    ALOGD("%s() bEanble = %d SampleRate = %u", __FUNCTION__, bEnable,SampleRate);

    enum mixer_ctl_type type;
    struct mixer_ctl *ctl;
    int retval = 0;

    // check need open low jitter mode
    if(SampleRate <= GetLowJitterModeSampleRate() && (AudioALSADriverUtility::getInstance()->GetPropertyValue(PROPERTY_KEY_EXTDAC)) == false)
    {
#ifdef NXP_SMARTPA_SUPPORT
        if (mStreamAttributeSource->output_devices & AUDIO_DEVICE_OUT_SPEAKER)
        {
            ALOGD("%s(), force enable low jitter mode, bEnable = %d, device = 0x%x", __FUNCTION__, bEnable, mStreamAttributeSource->output_devices);
        }
        else
        {
            ALOGD("%s(), bypass low jitter mode, bEnable = %d, device = 0x%x", __FUNCTION__, bEnable, mStreamAttributeSource->output_devices);
            return false;
        }
#else
        ALOGD("%s(), bypass low jitter mode = %d", __FUNCTION__, bEnable);
        return false;
#endif
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

bool AudioALSAPlaybackHandlerNormal::DeviceSupportHifi(audio_devices_t outputdevice)
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

uint32_t AudioALSAPlaybackHandlerNormal::ChooseTargetSampleRate(uint32_t SampleRate, audio_devices_t outputdevice)
{
    ALOGD("ChooseTargetSampleRate SampleRate = %d outputdevice = %d",SampleRate,outputdevice);
    uint32_t TargetSampleRate = 44100;
    if (SampleRate <= 192000 && SampleRate > 96000 && DeviceSupportHifi(outputdevice))
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

status_t SetMHLChipEnable(int enable __unused)
{
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerNormal::open()
{
    ALOGD("+%s(), mDevice = 0x%x", __FUNCTION__, mStreamAttributeSource->output_devices);
    AL_LOCK_MS(AudioALSADriverUtility::getInstance()->getStreamSramDramLock(), 3000);
    unsigned int pcmindex = 0;
    unsigned int cardindex = 0;

    // acquire pmic clk
    mHardwareResourceManager->EnableAudBufClk(true);

    pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl1Meida);
    cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl1Meida);

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

    ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
          __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);

#if defined(MTK_AUDIO_SW_DRE) && defined(MTK_NEW_VOL_CONTROL)
    mStartMuteBytes = mConfig.period_size *
                      mConfig.period_count *
                      mConfig.channels *
                      (pcm_format_to_bits(mConfig.format) / 8);

    mAllZeroBlock = new char[mStreamAttributeSource->buffer_size];
    memset(mAllZeroBlock, 0, mStreamAttributeSource->buffer_size);
#endif

    // post processing
    initPostProcessing();

    // SRC
    initBliSrc();

    // bit conversion
    initBitConverter();

    // poweraq processing
#ifdef MTK_POWERAQ_SUPPORT
    initPowerAQProcessing(0/*PPC_HANDLER_NORMAL*/);
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
    mHardwareResourceManager->setRoutePath(ROUTE_NORMAL_PLAYBACK, mStreamAttributeSource->output_devices);

    // open pcm driver
    openPcmDriver(pcmindex);
    AL_UNLOCK(AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

   

    // debug pcm dump
    OpenPCMDump(LOG_TAG);

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerNormal::close()
{
    ALOGD("+%s()", __FUNCTION__);
    AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

  

#if defined(MTK_AUDIO_SW_DRE) && defined(MTK_NEW_VOL_CONTROL)
    delete [] mAllZeroBlock;
    if (mForceMute)
    {
        mForceMute = false;
        ALOGD("%s(), SWDRE swdre unmute", __FUNCTION__);
        AudioMTKGainController::getInstance()->requestMute(getIdentity(), false);
    }
#endif

#if defined(MTK_HYBRID_NLE_SUPPORT)
    // Must do this before close analog path
    deinitNLEProcessing();
#endif

    // close pcm driver
    closePcmDriver();

    // reset route path
    mHardwareResourceManager->resetRoutePath(ROUTE_NORMAL_PLAYBACK, mStreamAttributeSource->output_devices);

    // reset codec path
    mHardwareResourceManager->stopOutputDevice();

    // disable lowjitter mode
    //SetLowJitterMode(false, mStreamAttributeTarget.sample_rate);

    DeinitDataPending();

    // poweraq processing
#ifdef MTK_POWERAQ_SUPPORT
    deinitPowerAQProcessing(0/*PPC_HANDLER_NORMAL*/);
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


status_t AudioALSAPlaybackHandlerNormal::routing(const audio_devices_t output_devices)
{
    mHardwareResourceManager->changeOutputDevice(output_devices);
    if (mAudioFilterManagerHandler) { mAudioFilterManagerHandler->setDevice(output_devices); }
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerNormal::setScreenState(bool mode, size_t buffer_size, size_t reduceInterruptSize, bool bforce __unused)
{
    // don't increase irq period when play hifi
    if (mode == 0 && mStreamAttributeSource->sample_rate > 48000)
        return NO_ERROR;

    if (0 == buffer_size)
    {
        buffer_size = mStreamAttributeSource->buffer_size;
    }

    int rate = mode ? (buffer_size / mStreamAttributeSource->num_channels) / ((mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_16_BIT) ? 2 : 4) :
                   ((mStreamAttributeTarget.buffer_size - reduceInterruptSize) / mConfig.channels) / ((mStreamAttributeTarget.audio_format == AUDIO_FORMAT_PCM_16_BIT) ? 2 : 4);

    mStreamAttributeTarget.mInterrupt = (rate+0.0) / mStreamAttributeTarget.sample_rate;

    ALOGD("%s, rate %d %f, mode = %d , buffer_size = %zu, channel %d, format%d", __FUNCTION__, rate, mStreamAttributeTarget.mInterrupt, mode, buffer_size, mConfig.channels, mStreamAttributeTarget.audio_format);

    mHardwareResourceManager->setInterruptRate(rate);
    return NO_ERROR;
}

ssize_t AudioALSAPlaybackHandlerNormal::write(const void *buffer, size_t bytes)
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

#if defined(MTK_AUDIO_SW_DRE) && defined(MTK_NEW_VOL_CONTROL)
    if (mStreamAttributeSource->output_devices == AUDIO_DEVICE_OUT_WIRED_HEADSET ||
        mStreamAttributeSource->output_devices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE)
    {
        bool isAllMute = false;

        /* check if contents is mute */
        if (!memcmp(mAllZeroBlock, buffer, bytes))
        {
            isAllMute = true;
        }
        else
        {
            isAllMute = true;
            size_t tmp_bytes = bytes;
            int32_t *sample = (int32_t *)buffer;
            while (tmp_bytes > 0)
            {
                if ((*sample) >> 8 != 0 && ((*sample) & 0xffffff00) != 0xffffff00)
                {
                    isAllMute = false;
                    break;
                }
                tmp_bytes -= 4;
                sample++;
            }
        }

        /* calculate delay and apply mute */
        ALOGV("%s(), isAllMute = %d, mForceMute = %d, mCurMuteBytes = %d, mStartMuteBytes = %d",
              __FUNCTION__,
              isAllMute,
              mForceMute,
              mCurMuteBytes,
              mStartMuteBytes);

        if (isAllMute)
        {
            if (!mForceMute) /* not mute yet */
            {
                mCurMuteBytes += bytes;
                if (mCurMuteBytes >= mStartMuteBytes)
                {
                    mForceMute = true;
                    ALOGD("%s(), SWDRE swdre mute", __FUNCTION__);
                    AudioMTKGainController::getInstance()->requestMute(getIdentity(), true);
                }
            }
        }
        else
        {
            mCurMuteBytes = 0;

            if (mForceMute)
            {
                mForceMute = false;
                ALOGD("%s(), SWDRE swdre unmute", __FUNCTION__);
                AudioMTKGainController::getInstance()->requestMute(getIdentity(), false);
            }
        }
    }
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


    // write data to pcm driver
    int retval = pcm_write(mPcm, pBufferAfterPending, bytesAfterpending);

#if defined(MTK_HYBRID_NLE_SUPPORT)
    doNLEProcessing(pBufferAfterPending, bytesAfterpending);
#endif

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

status_t AudioALSAPlaybackHandlerNormal::setFilterMng(AudioMTKFilterManager *pFilterMng)
{
    ALOGD("+%s() mAudioFilterManagerHandler [%p]", __FUNCTION__, pFilterMng);
    mAudioFilterManagerHandler = pFilterMng;
    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


} // end of namespace android
