#ifndef _AUDIO_HFP_UL_TASK_SOURCE_H
#define _AUDIO_HFP_UL_TASK_SOURCE_H

#include "AudioType.h"
#include <tinyalsa/asoundlib.h>
#include "AudioVolumeInterface.h"
#include "AudioCustParamClient.h"

namespace android
{

class AudioALSAHardwareResourceManager;

class AudioHfpULTaskSource
{
    public:

        AudioHfpULTaskSource();

        virtual ~AudioHfpULTaskSource();

        status_t prepare(uint32_t inDevice, uint32_t outDevice, int format,
                        uint32_t channelCount, uint32_t sampleRate, uint32_t accessSize);

        status_t start();

        ssize_t read(void *buffer, ssize_t bytes);

        status_t stop();

        int getRemainDataLatency();

        uint32_t getSwMicDigitalGain();

        uint32_t getUplinkTotalGain();

    private:

        pcm_format audioFormatToPcm(int format);

        bool mStarted;
        unsigned int mSampleRate;
        unsigned int mChannels;
        int mFormat;
        unsigned int mInterruptSample;
        unsigned int mInputDevice;
        unsigned int mOutputDevice;

        AudioALSAHardwareResourceManager *mHardwareResourceManager;
        AudioVolumeInterface *mAudioALSAVolumeController;
        AudioCustParamClient *mAudioCustParamClient;

        struct pcm *mPcm;
        struct pcm_config mConfig;

        static const uint32_t mMaxPcmDriverBufferSize = 0x20000;
        char *mBitConverterReadBuffer;
};

}

#endif
