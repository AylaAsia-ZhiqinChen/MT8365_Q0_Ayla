#ifndef _AUDIO_HPF_CONTROLLER_H
#define _AUDIO_HPF_CONTROLLER_H

#include "AudioHfpProcessing.h"
#include "AudioUtility.h"
#include "AudioType.h"
#include "AudioALSAStreamIn.h"
#include "AudioALSAStreamManager.h"
#include "AudioALSACaptureHandlerBase.h"
#include "AudioALSACaptureHandlerNormal.h"

#include <AudioLock.h>

//#include <powermanager/IPowerManager.h>

namespace android
{

class AudioHfpDLTaskSink;
class AudioHfpDLTaskSource;
class AudioHfpULTaskSink;
class AudioHfpULTaskSource;
class AudioHfpDLEchoRefSource;

class AudioHFPStreamAttribute
{
    public:
        int mFormat;
        unsigned int mSampleRate;
        unsigned int mChannels;
        unsigned int mBufferSize;
        unsigned int mInterruptSample;
        unsigned int mDevices;
        unsigned int mBytesPerSample;
};

class AudioHfpController
{
    public:
        static AudioHfpController *getInstance();

        /**
         * a function for ~AudioHfpController destructor
         */
        virtual ~AudioHfpController();

        /**
        * a function for tell AudioHfpController enable
        * @param bEnable
        */
        virtual status_t setHfpEnable(bool enable);

        /**
        * a function for get AudioHfpController enable
        */
        virtual bool getHfpEnable(void);

        virtual status_t enableHfpTask(void);
        virtual status_t disableHfpTask(void);
        virtual bool isHfpTaskRunning(void);
        virtual status_t setHfpSampleRate(int sampleRate);
        virtual status_t setHfpVolume(int volume);
        virtual status_t setHfpMicMute(bool state);

        virtual int DLTaskBgsDataWrite(void *buffer, int writtenSize); //would blocked
        virtual status_t DLTaskBgsDataStart(uint32_t sampleRate, uint32_t chNum, int32_t format);
        virtual status_t DLTaskBgsDataStop();

    protected:
        AudioHfpController();
        static AudioHfpController *UniqueHfpControllerInstance;
        AudioHfpController(const AudioHfpController &);             // intentionally undefined
        AudioHfpController &operator=(const AudioHfpController &);  // intentionally undefined

        class AudioHfpThread : public Thread
        {
            public:
                AudioHfpThread(AudioHfpController *HfpController);
                virtual ~AudioHfpThread();

                // Good place to do one-time initializations
                virtual status_t readyToRun();
                virtual void onFirstRef();

            private:
                virtual bool threadLoop();
                AudioHfpController *mtAudioHfpController;
        };

        status_t putUL1UL2DataBuffer();
        status_t putUL1UL2DataProcessing();
        status_t putDL1DL2DataBuffer();
        status_t putDL1DL2DataProcessing();
        status_t getUL1UL2DataBuffer();
        status_t getDL1DL2DataBuffer();

        status_t DL1DataAligned();
        void applyGainOnDL1Data();

        // echo reference
        status_t DLEchoRefSourceAllocate();
        status_t DLEchoRefSourceFree();
        status_t DLEchoRefSourceStart();
        status_t DLEchoRefSourceStop();
        int getDLEchoRefSource();
        status_t setDLEchoRefAttribute();
        status_t putDLEchoRefBuffer();
        status_t putDLEchoRefPrcoessing();

        status_t TaskStop();
        status_t TaskResourceFree();
        status_t TaskStart();
        status_t TaskResourceAllocate();

        // ULDL process function
        status_t ULDLTaskProcess();
        status_t ULDLTaskStart();
        status_t ULDLTaskStop();

        // UL Task related function
        status_t ULTaskProcess();
        status_t ULTaskStart();
        status_t ULTaskStop();

        // UL Task Source
        status_t setULTaskSourceAttribute();
        status_t ULTaskULSourceAllocate();
        status_t ULTaskULSourceStart();
        int ULTaskGetULDataSource();
        status_t ULTaskULSourceFree();

        // UL Task Sink
        status_t setULTaskSinkAttribute();
        status_t ULTaskDLSinkAllocate();
        status_t ULTaskDLSinkStart();
        int ULTaskPutDLDataSink();
        status_t ULTaskDLSinkFree();

        // DL data src to DL1
        status_t DLTaskSrctoDLProcess();
        status_t DLTaskSrctoDLConfig();
        status_t DLTaskSrctoDLClose();

        // UL data src to DL2
        status_t ULTaskSrctoDLProcess();
        status_t ULTaskSrctoDLConfig();
        status_t ULTaskSrctoDLClose();

        // DL Task related function
        status_t DLTaskProcess();
        status_t DLTaskStart();
        status_t DLTaskStop();

        // DL Task Source
        status_t setDLTaskSourceAttribute();
        status_t DLTaskULSourceAllocate();
        status_t DLTaskULSourceStart();
        int DLTaskGetULDataSource();
        status_t DLTaskULSourceFree();

        // DL Task Sink
        status_t setDLTaskSinkAttribute();
        status_t DLTaskDLSinkAllocate();
        status_t DLTaskDLSinkStart();
        int DLTaskPutDLDataSink();
        status_t DLTaskDLSinkFree();
        status_t DL1DataBgsMixing();

        status_t setThreadEnable(bool enable);
        bool getThreadEnable();

        // background sound mixing
        status_t setBgsSoundAttribute();
        status_t setBgsSoundAttribute(uint32_t sampleRate, uint32_t chNum, int32_t format);

        int calRemainTime(int bufferSize, AudioHFPStreamAttribute attr);
        status_t addAndClamp16(short *dst, short *src, uint32_t bufferSize, uint32_t frameSize);
        int compareTime(struct timespec time1, struct timespec time2);
        void setWakeLockAcquire(bool acquire);
        void applyMicMute();
        void updateParameters();
/*
        // For PM wake lock
        status_t acquireWakeLock(int uid = -1);
        void     releaseWakeLock();
        void     getPowerManager();

        // For PM wake lock
        sp<IPowerManager>   mPowerManager;
        sp<IBinder>         mWakeLockToken;
*/
        AudioHfpProcessing *mAudioHfpProcessing;
        AudioHfpDLTaskSink *mAudioHfpDLTaskSink;
        AudioHfpDLTaskSource *mAudioHfpDLTaskSource;
        AudioHfpULTaskSink *mAudioHfpULTaskSink;
        AudioHfpULTaskSource *mAudioHfpULTaskSource;
        AudioHfpDLEchoRefSource *mAudioHfpDLEchoRefSource;

        AudioHFPStreamAttribute mAudioULTaskSourceAttribute;
        char *mAudioULTaskInputBuffer;
        uint32_t mAudioULTaskUL2ProcessingBufferSize;
        uint32_t mAudioULTaskInputBuffeSize;
        char *mAudioULTaskUL1ProcessingBuffer;
        uint32_t mAudioULTaskUL1ProcessingBufferSize;
        char *mAudioULTaskUL2ProcessingBuffer;
        AudioHFPStreamAttribute mAudioULTaskSinkAttribute;
        char *mAudioULTaskOutputBuffer;
        uint32_t mAudioULTaskOutputBufferSize;
        AudioHFPStreamAttribute mAudioDLTaskSourceAttribute;
        char *mAudioDLTaskInputBuffer;
        uint32_t mAudioDLTaskInputBufferSize;
        AudioHFPStreamAttribute mAudioDLTaskSinkAttribute;
        char *mAudioDLTaskOutputBuffer;
        uint32_t mAudioDLTaskOutputBufferSize;
        char *mAudioDLTaskDL1ProcessingBuffer;
        uint32_t mAudioDLTaskDL1ProcessingBufferSize;
        char *mAudioDLTaskDL2ProcessingBuffer;
        uint32_t mAudioULTaskDL2ProcessingBufferSize;

        // echo reference data processing
        AudioHFPStreamAttribute mAudioDLEchoRefAttribute;
        RingBuf mEchoRefBuffer;
        char *mEchoRefTempBuffer;
        uint32_t mEchoRefTempBufferSize;
        char *mEchoRefReadBuffer;
        bool mEchoRefBufferAligned;

        // background sound processing
        AudioHFPStreamAttribute mAudioBgsSoundAttribute;
        RingBuf mBgsSoundBuffer;
        MtkAudioSrcBase *mBliHandlerBgsSoundSrc;
        char *mAudioBgsSoundSrcWorkingBuffer;
        uint32_t mAudioBgsSoundSrcWorkingBufferSize;
        char *mAudioBgsSoundSrcBuffer;
        pthread_mutex_t mAudioBgsLock;
        pthread_cond_t mAudioBgsWaitWorkCV;
        uint32_t mBgsSoundCount;
        AudioLock mBgsSoundCountLock;

        // DL Task SRC
        MtkAudioSrcBase *mBliHandlerAudioDLTaskSrc;
        char *mAudioDLTaskSrcBuffer;
        uint32_t mAudioDLTaskSrcBufferSize;
        char *mAudioDLTaskSrcWorkingBuffer;
        uint32_t mAudioDLTaskSrcWorkingBufferSize;

        //UL Task SRC
        MtkAudioSrcBase *mBliHandlerAudioULTaskSrc;
        char *mAudioULTaskSrcBuffer;
        uint32_t mAudioULTaskSrcBufferSize;
        char *mAudioULTaskSrcWorkingBuffer;
        uint32_t mAudioULTaskSrcWorkingBufferSize;

        AudioLock mEnableLock;
        bool mHfpEnable;
        bool mWakeLockAcquired;

        // default value
        const uint32_t mDLTaskSinkSampleRate;
        const uint32_t mDLTaskSinkChannelCount;
        const uint32_t mULTaskSourceSampleRate;
        const uint32_t mULTaskSourceChannelCount;
        uint32_t mULTaskSinkSampleRate;
        const uint32_t mULTaskSinkChannelCount;
        uint32_t mDLTaskSourceSampleRate;
        const uint32_t mDLTaskSourceChannelCount;
        const uint32_t mDLEchoRefChannelCount;

        // debug and dump usage
        int mDumpFileNum;
        FILE *mAudioHfpULTaskInputDumpFile;
        FILE *mAudioHfpULTaskOutputDumpFile;
        FILE *mAudioHfpDLTaskInputDumpFile;
        FILE *mAudioHfpDLTaskOutputDumpFile;
        FILE *mAudioHfpDLEchoRefDumpFile;
        FILE *mAudioBgsSoundSrcBufferDumpFile;

        bool mThreadEnable;
        sp<AudioHfpThread>  mAudioHfpThread;

        short mHfpVolumeGain;
        bool mMicMute;

        bool mFirstEchoRefDataArrived;
        struct timespec mFirstEchoRefDataPlaybackTime;
        struct timespec mHfpULTaskCaptureTime;

        uint32_t mDLTaskOutputDevice;
        uint32_t mULTaskInputDevice;

        bool mThreadAlive;
};

}

#endif
