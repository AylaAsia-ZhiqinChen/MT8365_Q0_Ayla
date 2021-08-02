#ifndef ANDROID_HDMI_TX_CONTROLLER_H
#define ANDROID_HDMI_TX_CONTROLLER_H

#include <utils/KeyedVector.h>

namespace android
{

class HDMITxController
{
    public:
        virtual ~HDMITxController();

        static void queryEdidInfo(void);
        static void clearEdidInfo(void);
        static bool isSinkSupportedFormat(unsigned int format, unsigned int sample_rate = 0, unsigned int channel_count = 0);
        static void notifyAudioSetting(unsigned int format, unsigned int sample_rate, unsigned int channel_mask, bool to_i2s = true);

    protected:
        HDMITxController();

    private:
        static unsigned int toHdmiTxCodecType(unsigned int type);
        static unsigned int toHdmiTxSinkCodecFormat(unsigned int type);
        static unsigned int toHdmiTxSinkSampleRateType(unsigned int rate);
        static void dumpSinkCapability(void);

        static unsigned int mHdmiSinkSupportCodec;
        // <channel, samplerate>
        static KeyedVector<unsigned int, unsigned int> mHdmiSinkPcmSupportProfiles;
};

} // end namespace android

#endif // end of ANDROID_HDMI_TX_CONTROLLER_H
