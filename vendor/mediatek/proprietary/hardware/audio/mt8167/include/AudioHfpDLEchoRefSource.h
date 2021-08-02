#ifndef _AUDIO_HFP_DL_ECHO_REF_SOURCE_H
#define _AUDIO_HFP_DL_ECHO_REF_SOURCE_H

#include "AudioType.h"
#include <tinyalsa/asoundlib.h>

#include "AudioUtility.h"

namespace android
{

class AudioHfpDLEchoRefSource
{
    public:

        AudioHfpDLEchoRefSource();

        virtual ~AudioHfpDLEchoRefSource();

        status_t prepare(uint32_t inDevice, uint32_t outDevice, int format,
                        uint32_t channelCount, uint32_t sampleRate, uint32_t accessSize);

        status_t start();

        ssize_t read(void *buffer, ssize_t bytes);

        status_t stop();

        int getRemainDataLatency();

        uint32_t getSwMicDigitalGain();

        uint32_t getUplinkTotalGain();

    private:

        bool mStarted;
        unsigned int mReqChannels;
        unsigned int mOutputDevice;
        struct pcm *mPcm;
        struct pcm_config mConfig;
        char *mReadBuffer;
        unsigned int mReadBytes;
        MtkAudioSrcBase *mAudioSrc;
};

}

#endif
