#ifndef _AUDIO_HFP_UL_TASK_SINK_H
#define _AUDIO_HFP_UL_TASK_SINK_H

#include "AudioType.h"
#include "AudioUtility.h"
#include <tinyalsa/asoundlib.h>

namespace android
{

class AudioBTCVSDControl;

class AudioHfpULTaskSink
{
    public:

        AudioHfpULTaskSink();

        virtual ~AudioHfpULTaskSink();

        status_t prepare(int format, uint32_t channelCount,
                        uint32_t sampleRate, uint32_t accessSize);

        ssize_t write(const void *buffer, size_t bytes);

        status_t stop();

        int getRemainDataLatency();

    private:
        status_t paramCheckAndUpdate(int format, uint32_t channelCount, uint32_t sampleRate, uint32_t accessSize);

        size_t writeToBTCVSDController(const void *buffer,size_t bytes);

        uint32_t getSampleRate();

        int mFdBluetooth;
        AudioBTCVSDControl *mAudioBTCVSDControl;
        bool mIsUseMergeInterface; // BT spec check, no merge interface support.
        bool mIsRunning;
        bool mPrepared;
        bool mStarted;
        unsigned int mSampleRate;
        int mFormat;
        unsigned int mInterruptSample;
        uint32_t mChannelCount;
        struct pcm *mPcm;
        struct pcm_config mConfig;

        struct mixer *mMixer;

        static int DumpFileNum;
        String8 DumpFileName;
        FILE *mAudioHfpULTaskSinkdumpFile;

        static const int mAvailableFormat = AUDIO_FORMAT_PCM_16_BIT;
        static const uint32_t mBTSampleRate = 8000;
        static const uint32_t mBTWidebandSampleRate = 16000;
};

}

#endif
