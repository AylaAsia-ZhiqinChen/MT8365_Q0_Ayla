#ifndef _AUDIO_HFP_DL_TASK_SOURCE_H
#define _AUDIO_HFP_DL_TASK_SOURCE_H

#include "AudioUtility.h"
#include <tinyalsa/asoundlib.h>
#include "AudioALSACaptureHandlerBase.h"

namespace android
{

class AudioBTCVSDControl;

class AudioHfpDLTaskSource
{
    public:

        AudioHfpDLTaskSource();

        virtual ~AudioHfpDLTaskSource();

        status_t prepare(int format, uint32_t channelCount,
            uint32_t sampleRate, uint32_t accessSize);

        status_t start();

        ssize_t read(void *buffer, ssize_t bytes);

        status_t stop();

        int getRemainDataLatency();

    private:
        class RingBufWithLock;
        class AudioHfpDLTaskSourceThread;

        bool mIsRunning;
        bool mPrepared;
		bool mStarted;
        uint32_t mChannelCount;
		unsigned int mSampleRate;
        int mFormat;
        unsigned int mInterruptSample;

        struct pcm *mPcm;
        struct pcm_config mConfig;
		struct mixer *mMixer;

        Condition mReadSignal;

        sp<RingBufWithLock> mRingBufWithLock;
        sp<AudioHfpDLTaskSourceThread> mAudioHfpDLTaskSourceThread;

        static const int mAvailableFormat = AUDIO_FORMAT_PCM_16_BIT;
        static const uint32_t mBTSampleRate = 8000;
        static const uint32_t mBTWidebandSampleRate = 16000;


        class RingBufWithLock : public RefBase
        {
            public:
                RingBuf mRingBuf;
                Mutex   mLock;
                virtual ~RingBufWithLock() {}
        };

        //reading thread
        class AudioHfpDLTaskSourceThread : public Thread
        {
            public:
                //constructor: get RingBufWithLock*, no need to delete it in destructor (The man create it should delete it.)
                AudioHfpDLTaskSourceThread(sp<RingBufWithLock> ringBufferWithLock, Condition* bufFillSignal, uint32_t channelCount, struct pcm *mPcm, struct mixer *mMixer, struct pcm_config mConfig, uint32_t accessSize);
                virtual ~AudioHfpDLTaskSourceThread();

                // Good place to do one-time initializations
                virtual status_t    readyToRun();
                virtual void        onFirstRef();
            protected:
                uint32 readDataFromBTCVSD(void *linear_buffer);
            private:
                virtual bool threadLoop();
                void copyDataToBuffer(void *outbuf, uint32_t outsize);
                struct timespec getCurrentTimeStamp();

                sp<RingBufWithLock> mRingBufferWithLock;
                int mBufferSize;
                int mFd2;
                uint32_t mReadBufferSize;
                bool mBTIrqReceived;
                uint32_t mChannelCount;
                AudioBTCVSDControl *mAudioBTCVSDControl;
                struct pcm *mThreadPcm;
                struct mixer *mThreadMixer;
                struct pcm_config mThreadPcmConfig;
                uint32_t mThreadAccessSize;
                Condition* mBufFillSignal;
        };
};

}

#endif
