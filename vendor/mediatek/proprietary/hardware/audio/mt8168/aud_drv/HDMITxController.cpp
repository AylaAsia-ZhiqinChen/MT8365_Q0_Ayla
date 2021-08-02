#define LOG_TAG "HDMITxController"

#include "HDMITxController.h"
#include <utils/Log.h>
#include <system/audio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "audio_custom_exp.h"
#include <unistd.h>

#if defined(MTK_INTERNAL_HDMI_SUPPORT) || defined(MTK_INTERNAL_MHL_SUPPORT)
#include <linux/hdmitx.h>
#define HDMI_DRV "/dev/hdmitx"
#endif

#define CAP_TO_STR(cap) (((cap)!=0)?"Y":"N")


namespace android
{

unsigned int HDMITxController::mHdmiSinkSupportCodec = 0;
KeyedVector<unsigned int, unsigned int> HDMITxController::mHdmiSinkPcmSupportProfiles;

HDMITxController::HDMITxController()
{
}

HDMITxController::~HDMITxController()
{
}

void HDMITxController::queryEdidInfo(void)
{
    ALOGD("+%s\n", __FUNCTION__);
    clearEdidInfo();

#if defined(MTK_INTERNAL_HDMI_SUPPORT) || defined(MTK_INTERNAL_MHL_SUPPORT)
    int fd = open(HDMI_DRV, O_RDONLY);
    if (fd == -1)
    {
        ALOGE("%s open %s fail errno = %d\n", __FUNCTION__, HDMI_DRV, errno);
        return;
    }

    HDMI_EDID_T hdmi_edid;
    memset(&hdmi_edid, 0, sizeof(HDMI_EDID_T));

    int ret = ioctl(fd, MTK_HDMI_GET_EDID, &hdmi_edid);
    if (ret != -1)
    {
        mHdmiSinkSupportCodec = hdmi_edid.ui2_sink_aud_dec;
        // PCM 2ch capability
        unsigned int pcm_cap = hdmi_edid.ui4_hdmi_pcm_ch_type & 0XFF;
        if (pcm_cap)
        {
            mHdmiSinkPcmSupportProfiles.add(2, pcm_cap);
        }
        // PCM 6ch capability
        pcm_cap = (hdmi_edid.ui4_hdmi_pcm_ch_type >> 8) & 0XFF;
        if (pcm_cap)
        {
            mHdmiSinkPcmSupportProfiles.add(6, pcm_cap);
        }
        // PCM 8ch capability
        pcm_cap = (hdmi_edid.ui4_hdmi_pcm_ch_type >> 16) & 0XFF;
        if (pcm_cap)
        {
            mHdmiSinkPcmSupportProfiles.add(8, pcm_cap);
        }
        // PCM 3ch capability
        pcm_cap = hdmi_edid.ui4_hdmi_pcm_ch3ch4ch5ch7_type & 0XFF;
        if (pcm_cap)
        {
            mHdmiSinkPcmSupportProfiles.add(3, pcm_cap);
        }
        // PCM 4ch capability
        pcm_cap = (hdmi_edid.ui4_hdmi_pcm_ch3ch4ch5ch7_type >> 8) & 0XFF;
        if (pcm_cap)
        {
            mHdmiSinkPcmSupportProfiles.add(4, pcm_cap);
        }
        // PCM 5ch capability
        pcm_cap = (hdmi_edid.ui4_hdmi_pcm_ch3ch4ch5ch7_type >> 16) & 0XFF;
        if (pcm_cap)
        {
            mHdmiSinkPcmSupportProfiles.add(5, pcm_cap);
        }
        // PCM 7ch capability
        pcm_cap = (hdmi_edid.ui4_hdmi_pcm_ch3ch4ch5ch7_type >> 24) & 0XFF;
        if (pcm_cap)
        {
            mHdmiSinkPcmSupportProfiles.add(7, pcm_cap);
        }
        dumpSinkCapability();
    }
    else
    {
        ALOGE("%s ioctl MTK_HDMI_GET_EDID fail errno = %d\n", __FUNCTION__, errno);
    }

    close(fd);
#endif

    ALOGD("-%s\n", __FUNCTION__);
}

void HDMITxController::clearEdidInfo(void)
{
    mHdmiSinkSupportCodec = 0;
    mHdmiSinkPcmSupportProfiles.clear();
}

bool HDMITxController::isSinkSupportedFormat(unsigned int format, unsigned int sample_rate, unsigned int channel_count)
{
#if defined(MTK_INTERNAL_HDMI_SUPPORT) || defined(MTK_INTERNAL_MHL_SUPPORT)
    unsigned int sink_codec_format = toHdmiTxSinkCodecFormat(format);

    if (!(sink_codec_format & mHdmiSinkSupportCodec))
    {
        ALOGW("%s unsupported format %u\n", __FUNCTION__, format);
        return false;
    }
    if (sink_codec_format == HDMI_SINK_AUDIO_DEC_LPCM)
    {
        ssize_t index = mHdmiSinkPcmSupportProfiles.indexOfKey(channel_count);
        if (index >= 0)
        {
            uint32_t supported_sample_rates = mHdmiSinkPcmSupportProfiles.valueAt(index);
            if (!(supported_sample_rates & toHdmiTxSinkSampleRateType(sample_rate)))
            {
                ALOGW("%s unsupported sample rate %d\n", __FUNCTION__, sample_rate);
                return false;
            }
        }
        else
        {
            ALOGW("%s unsupported channel %d\n", __FUNCTION__, channel_count);
            return false;
        }
    }
    return true;
#else
    (void)format;
    (void)sample_rate;
    (void)channel_count;
    return false;
#endif
}

void HDMITxController::notifyAudioSetting(unsigned int format, unsigned int sample_rate, unsigned int channel_mask, bool to_i2s)
{
#if defined(MTK_INTERNAL_HDMI_SUPPORT) || defined(MTK_INTERNAL_MHL_SUPPORT)
    int fd = open(HDMI_DRV, O_RDONLY);
    if (fd == -1)
    {
        ALOGE("%s open %s fail errno = %d\n", __FUNCTION__, HDMI_DRV, errno);
        return;
    }

    HDMITX_AUDIO_PARA HdmiAudioParam;

    HdmiAudioParam.e_hdmi_aud_in = to_i2s ? SV_I2S : SV_SPDIF;
    HdmiAudioParam.e_aud_code = toHdmiTxCodecType(format);
    HdmiAudioParam.e_I2sFmt = HDMI_I2S_24BIT;
    HdmiAudioParam.u1HdmiI2sMclk = MCLK_128FS;

    switch (sample_rate)
    {
        case 192000:
            HdmiAudioParam.e_iec_frame = IEC_192K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_192K;
            // channel status byte 3: sampling frequency and clock accuracy
            HdmiAudioParam.bhdmi_LCh_status[3] = 0xE;
            break;
        case 176400:
            HdmiAudioParam.e_iec_frame = IEC_176K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_176K;
            // channel status byte 3: sampling frequency and clock accuracy
            HdmiAudioParam.bhdmi_LCh_status[3] = 0xC;
            break;
        case 96000:
            HdmiAudioParam.e_iec_frame = IEC_96K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_96K;
            // channel status byte 3: sampling frequency and clock accuracy
            HdmiAudioParam.bhdmi_LCh_status[3] = 0xA;
            break;
        case 88200:
            HdmiAudioParam.e_iec_frame = IEC_88K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_88K;
            // channel status byte 3: sampling frequency and clock accuracy
            HdmiAudioParam.bhdmi_LCh_status[3] = 0x8;
            break;
        case 48000:
            HdmiAudioParam.e_iec_frame = IEC_48K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_48K;
            // channel status byte 3: sampling frequency and clock accuracy
            HdmiAudioParam.bhdmi_LCh_status[3] = 0x2;
            break;
        case 44100:
            HdmiAudioParam.e_iec_frame = IEC_44K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_44K;
            // channel status byte 3: sampling frequency and clock accuracy
            HdmiAudioParam.bhdmi_LCh_status[3] = 0x0;
            break;
        case 32000:
            HdmiAudioParam.e_iec_frame = IEC_32K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_32K;
            // channel status byte 3: sampling frequency and clock accuracy
            HdmiAudioParam.bhdmi_LCh_status[3] = 0x3;
            break;
        default:
            ALOGW("%s unsupported sample rate %d\n", __FUNCTION__, sample_rate);
            break;
    }

    switch (channel_mask)
    {
        case AUDIO_CHANNEL_OUT_MONO:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_1_0;
            break;
        case AUDIO_CHANNEL_OUT_STEREO:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_2_0;
            break;
        case AUDIO_CHANNEL_OUT_QUAD_SIDE:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_4_0_CLRS;
            break;
        case AUDIO_CHANNEL_OUT_QUAD:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_4_0;
            break;
        case AUDIO_CHANNEL_OUT_5POINT1:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_5_1;
            break;
        case AUDIO_CHANNEL_OUT_7POINT1:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_7_1;
            break;
        default:
            ALOGW("%s unknown channel mask %x\n", __FUNCTION__, channel_mask);
            break;
    }

    // channel status byte 0: general control ande mode information
    HdmiAudioParam.bhdmi_LCh_status[0] = CHANNEL_STATUS_COPY_BIT << 2;

    // channel status byte 0, bit 1: Audio sample word used for other purposes than linear PCM
    if (!audio_is_linear_pcm((audio_format_t)format)) {
        HdmiAudioParam.bhdmi_LCh_status[0] |= (1 << 1);
    }

    // channel status byte 1: category code
    HdmiAudioParam.bhdmi_LCh_status[1] = CHANNEL_STATUS_CATEGORY_CODE;
    // channel status byte 2: source and channel number
    HdmiAudioParam.bhdmi_LCh_status[2] = 0;
    // channel status byte 4: word length and original sampling frequency
    HdmiAudioParam.bhdmi_LCh_status[4] = 0x2;
    memcpy(HdmiAudioParam.bhdmi_RCh_status, HdmiAudioParam.bhdmi_LCh_status, sizeof(HdmiAudioParam.bhdmi_LCh_status));

    int ret = ioctl(fd, MTK_HDMI_AUDIO_SETTING, &HdmiAudioParam);
    if (ret == -1)
    {
        ALOGE("%s ioctl MTK_HDMI_AUDIO_SETTING fail errno = %d\n", __FUNCTION__, errno);
    }

    close(fd);
#else
    (void)format;
    (void)sample_rate;
    (void)channel_mask;
    (void)to_i2s;
#endif
}

unsigned int HDMITxController::toHdmiTxCodecType(unsigned int type)
{
#if defined(MTK_INTERNAL_HDMI_SUPPORT) || defined(MTK_INTERNAL_MHL_SUPPORT)
    switch (type)
    {
    case AUDIO_FORMAT_AC3: return AVD_AC3;
    case AUDIO_FORMAT_E_AC3: return AVD_DOLBY_PLUS;
    case AUDIO_FORMAT_PCM_16_BIT: return AVD_LPCM;
    case AUDIO_FORMAT_PCM_8_24_BIT: return AVD_LPCM;
    case AUDIO_FORMAT_PCM_32_BIT: return AVD_LPCM;
    case AUDIO_FORMAT_IEC61937: return HDMI_SINK_AUDIO_DEC_LPCM; // TODO: need further test
    default: return AVD_BITS_NONE;
    }
#else
    (void)type;
    return 0;
#endif
}

unsigned int HDMITxController::toHdmiTxSinkCodecFormat(unsigned int type)
{
#if defined(MTK_INTERNAL_HDMI_SUPPORT) || defined(MTK_INTERNAL_MHL_SUPPORT)
    switch (type)
    {
    case AUDIO_FORMAT_AC3: return HDMI_SINK_AUDIO_DEC_AC3;
    case AUDIO_FORMAT_E_AC3: return HDMI_SINK_AUDIO_DEC_DOLBY_PLUS;
    case AUDIO_FORMAT_PCM_16_BIT: return HDMI_SINK_AUDIO_DEC_LPCM;
    case AUDIO_FORMAT_PCM_8_24_BIT: return HDMI_SINK_AUDIO_DEC_LPCM;
    case AUDIO_FORMAT_PCM_32_BIT: return HDMI_SINK_AUDIO_DEC_LPCM;
    case AUDIO_FORMAT_IEC61937: return HDMI_SINK_AUDIO_DEC_LPCM; // TODO: need further test
    default: return 0;
    }
#else
    (void)type;
    return 0;
#endif
}

unsigned int HDMITxController::toHdmiTxSinkSampleRateType(unsigned int rate)
{
#if defined(MTK_INTERNAL_HDMI_SUPPORT) || defined(MTK_INTERNAL_MHL_SUPPORT)
    switch (rate)
    {
    case 192000: return SINK_AUDIO_192k;
    case 176400: return SINK_AUDIO_176k;
    case 96000: return SINK_AUDIO_96k;
    case 88200: return SINK_AUDIO_88k;
    case 48000: return SINK_AUDIO_48k;
    case 44100: return SINK_AUDIO_44k;
    case 32000: return SINK_AUDIO_32k;
    default: return 0;
    }
#else
    (void)rate;
    return 0;
#endif
}

void HDMITxController::dumpSinkCapability(void)
{
#if defined(MTK_INTERNAL_HDMI_SUPPORT) || defined(MTK_INTERNAL_MHL_SUPPORT)
    // dump codec capability
    ALOGD("Sink LPCM(%s) AC3(%s) AAC(%s) DTS(%s) DDPlus(%s) DTS-HD(%s) DOLBY TRUEHD(%s)\n",
          CAP_TO_STR(HDMI_SINK_AUDIO_DEC_LPCM & mHdmiSinkSupportCodec),
          CAP_TO_STR(HDMI_SINK_AUDIO_DEC_AC3 & mHdmiSinkSupportCodec),
          CAP_TO_STR(HDMI_SINK_AUDIO_DEC_AAC & mHdmiSinkSupportCodec),
          CAP_TO_STR(HDMI_SINK_AUDIO_DEC_DTS & mHdmiSinkSupportCodec),
          CAP_TO_STR(HDMI_SINK_AUDIO_DEC_DOLBY_PLUS & mHdmiSinkSupportCodec),
          CAP_TO_STR(HDMI_SINK_AUDIO_DEC_DTS_HD & mHdmiSinkSupportCodec),
          CAP_TO_STR(HDMI_SINK_AUDIO_DEC_MAT_MLP & mHdmiSinkSupportCodec));

    // dump PCM capability
    for (int i = 2; i <= 8; i++)
    {
        uint32_t SupportedSampleRates = 0;
        ssize_t index = mHdmiSinkPcmSupportProfiles.indexOfKey(i);
        if (index >= 0)
        {
            SupportedSampleRates = mHdmiSinkPcmSupportProfiles.valueAt(index);
        }
        ALOGD("Sink PCM %dch: 192k(%s) 176k(%s) 96k(%s) 88k(%s) 48k(%s) 44k(%s) 32k(%s)\n", i,
              CAP_TO_STR(SINK_AUDIO_192k & SupportedSampleRates),
              CAP_TO_STR(SINK_AUDIO_176k & SupportedSampleRates),
              CAP_TO_STR(SINK_AUDIO_96k & SupportedSampleRates),
              CAP_TO_STR(SINK_AUDIO_88k & SupportedSampleRates),
              CAP_TO_STR(SINK_AUDIO_48k & SupportedSampleRates),
              CAP_TO_STR(SINK_AUDIO_44k & SupportedSampleRates),
              CAP_TO_STR(SINK_AUDIO_32k & SupportedSampleRates));
    }
#endif
}

} // end of namespace android
