#ifndef _AUDIO_HFP_DL_TASK_SINK_H
#define _AUDIO_HFP_DL_TASK_SINK_H

#include "AudioType.h"
#include "AudioUtility.h"
#include <tinyalsa/asoundlib.h>


namespace android
{

class AudioALSAHardwareResourceManager;
//class MtkAudioBitConverter;

class AudioHfpDLTaskSink
{
    public:

        AudioHfpDLTaskSink();

        virtual ~AudioHfpDLTaskSink();

        status_t prepare(uint32_t device, int format, uint32_t channelCount,
                        uint32_t sampleRate, uint32_t accessSize);

        ssize_t write(const void *buffer, size_t bytes);

        status_t stop();

        int getRemainDataLatency();

    private:

        pcm_format audioFormatToPcm(int format);
        status_t initBitConverter(int source, int target);
        bool setLowJitterMode(bool bEnable,uint32_t SampleRate);

        bool mStarted;
        unsigned int mSampleRate;
        unsigned int mChannels;
        int mFormat;
        unsigned int mInterruptSample;
        unsigned int mDevice;
        AudioALSAHardwareResourceManager *mHardwareResourceManager;
        struct mixer *mMixer;
        struct pcm *mPcm;
        struct pcm_config mConfig;
        struct pcm *mHpImpeDancePcm;

        MtkAudioBitConverterBase *mBitConverter;
        char *mBitConverterOutputBuffer;
        char *mDataPendingOutputBuffer;
        char *mDataPendingRemindBuffer;
        unsigned int mDataPendingRemindNumber;
        static const uint32_t mDataAlignedSize = 64;
        static const uint32_t mMaxPcmDriverBufferSize = 0x20000;
};

}

#endif
