#define LOG_TAG "AudioHfpController"
//#define LOG_NDEBUG 0

#include "AudioHfpController.h"
#include "AudioHfpConf.h"
#include "AudioHfpDLTaskSink.h"
#include "AudioHfpDLTaskSource.h"
#include "AudioHfpULTaskSink.h"
#include "AudioHfpULTaskSource.h"
#ifdef AUDIO_HFP_ECHO_REFERENCE_FROM_AWB
#include "AudioHfpDLEchoRefSource.h"
#endif
#include "AudioSpeechEnhLayer.h"
#include <sys/prctl.h>
//#include <powermanager/PowerManager.h>
#include <binder/IServiceManager.h>
#include <hardware_legacy/power.h>


//#define DEBUG_AUDIO_LATENCY
//#define GET_DL_DATA_WITH_SPEECH_ENHANCEMENT

#define AUDIO_HFP_PROCESS_FRAME_MS (20)
#define AUDIO_HFP_PROCESS_SAMPLE_RATE (16000)
#define AUDIO_HFP_PROCESS_FORMAT (AUDIO_FORMAT_PCM_16_BIT)
#define AUDIO_HFP_PROCESS_CHANNEL_COUNT (2)
#define AUDIO_HFP_VOLUME_MAX_INDEX (15)
#define AUDIO_HFP_VOLUME_VALID_BIT (12)
#define AUDIO_HFP_VOLUME_UNITY_GAIN (1 << AUDIO_HFP_VOLUME_VALID_BIT)

namespace android
{
///data/vendor/audiohal/audio_dump/
static const char *AudioHfpULTaskInput = "/data/vendor/audiohal/audio_dump/AudioHfpULTaskInput_Dump";
static const char *AudioHfpULTaskOutput = "/data/vendor/audiohal/audio_dump/AudioHfpULTaskOutput_Dump";
static const char *AudioHfpDLTaskInput = "/data/vendor/audiohal/audio_dump/AudioHfpDLTaskInput_Dump";
static const char *AudioHfpDLTaskOutput = "/data/vendor/audiohal/audio_dump/AudioHfpDLTaskOutput_Dump";
static const char *AudioHfpDLEchoRef = "/data/vendor/audiohal/audio_dump/AudioHfpDLEchoRef_Dump";
static const char *AudioBgsSoundSrcBuffer ="/data/vendor/audiohal/audio_dump/AudioBgsSoundSrcBuffer_Dump";
static const char *AUDIO_HFP_WAKE_LOCK_NAME = "AudioHfpController";


AudioHfpController *AudioHfpController::UniqueHfpControllerInstance = 0;

AudioHfpController *AudioHfpController::getInstance()
{
    if (UniqueHfpControllerInstance == 0)
    {
        ALOGD("+UniqueHfpControllerInstance\n");
        UniqueHfpControllerInstance = new AudioHfpController();
        ALOGD("-UniqueHfpControllerInstance\n");
    }
    return UniqueHfpControllerInstance;
}

AudioHfpController::AudioHfpController()
    : mAudioHfpProcessing(new AudioHfpProcessing()),
      mAudioHfpDLTaskSink(NULL),
      mAudioHfpDLTaskSource(NULL),
      mAudioHfpULTaskSink(NULL),
      mAudioHfpULTaskSource(NULL),
      mAudioHfpDLEchoRefSource(NULL),
      mAudioULTaskInputBuffer(NULL),
      mAudioULTaskUL1ProcessingBuffer(NULL),
      mAudioULTaskUL2ProcessingBuffer(NULL),
      mAudioULTaskOutputBuffer(NULL),
      mAudioDLTaskInputBuffer(NULL),
      mAudioDLTaskOutputBuffer(NULL),
      mAudioDLTaskDL1ProcessingBuffer(NULL),
      mAudioDLTaskDL2ProcessingBuffer(NULL),
      mEchoRefReadBuffer(NULL),
      mBliHandlerBgsSoundSrc(NULL),
      mAudioBgsSoundSrcWorkingBuffer(NULL),
      mAudioBgsSoundSrcWorkingBufferSize(0),
      mAudioBgsSoundSrcBuffer(NULL),
      mBliHandlerAudioDLTaskSrc(NULL),
      mAudioDLTaskSrcBuffer(NULL),
      mAudioDLTaskSrcWorkingBuffer(NULL),
      mBliHandlerAudioULTaskSrc(NULL),
      mAudioULTaskSrcBuffer(NULL),
      mAudioULTaskSrcWorkingBuffer(NULL),
      mHfpEnable(false),
      mWakeLockAcquired(false),
      mDLTaskSinkSampleRate(AUDIO_HFP_PROCESS_SAMPLE_RATE),
      mDLTaskSinkChannelCount(AUDIO_HFP_PROCESS_CHANNEL_COUNT),
      mULTaskSourceSampleRate(AUDIO_HFP_PROCESS_SAMPLE_RATE),
      mULTaskSourceChannelCount(AUDIO_HFP_PROCESS_CHANNEL_COUNT),
      mULTaskSinkSampleRate(8000),
      mULTaskSinkChannelCount(AUDIO_HFP_PROCESS_CHANNEL_COUNT),
      mDLTaskSourceSampleRate(8000),
      mDLTaskSourceChannelCount(AUDIO_HFP_PROCESS_CHANNEL_COUNT),
      mDLEchoRefChannelCount(1),
      mDumpFileNum(0),
      mAudioHfpULTaskInputDumpFile(NULL),
      mAudioHfpULTaskOutputDumpFile(NULL),
      mAudioHfpDLTaskInputDumpFile(NULL),
      mAudioHfpDLTaskOutputDumpFile(NULL),
      mAudioHfpDLEchoRefDumpFile(NULL),
      mAudioBgsSoundSrcBufferDumpFile(NULL),
      mThreadEnable(false),
      mAudioHfpThread(NULL),
      mHfpVolumeGain(AUDIO_HFP_VOLUME_UNITY_GAIN),
      mMicMute(false),
      mFirstEchoRefDataArrived(false),
      mDLTaskOutputDevice(AUDIO_HFP_DEFAULT_OUTPUT_DEVICE),
      mULTaskInputDevice(AUDIO_HFP_DEFAULT_INPUT_DEVICE),
      mThreadAlive(false)
{
    ALOGD("%s", __FUNCTION__);

    memset(&mBgsSoundBuffer, 0, sizeof(mBgsSoundBuffer));

    int ret = pthread_mutex_init(&mAudioBgsLock, NULL);
    if (ret != 0) {
        ALOGE("%s failed to initialize mAudioBgsLock!", __FUNCTION__);
    }

    ret = pthread_cond_init(&mAudioBgsWaitWorkCV, NULL);
    if (ret != 0) {
        ALOGE("%s failed to initialize mAudioBgsWaitWorkCV!", __FUNCTION__);
    }

    if (mAudioHfpProcessing)
        mAudioHfpProcessing->InitCheck();

    setDLTaskSinkAttribute();
    setDLTaskSourceAttribute();
    setULTaskSinkAttribute();
    setULTaskSourceAttribute();
    setDLEchoRefAttribute();
    setBgsSoundAttribute();
}

AudioHfpController::~AudioHfpController()
{
    ALOGD("%s", __FUNCTION__);

    if (mAudioHfpProcessing)
        delete mAudioHfpProcessing;

    if (mAudioHfpDLTaskSink)
        delete mAudioHfpDLTaskSink;

    if (mAudioHfpDLTaskSource)
        delete mAudioHfpDLTaskSource;

    if (mAudioHfpULTaskSink)
        delete mAudioHfpULTaskSink;

    if (mAudioHfpULTaskSource)
        delete mAudioHfpULTaskSource;
}

status_t AudioHfpController::setHfpEnable(bool enable)
{
    ALOGD("%s", __FUNCTION__);
    mHfpEnable = enable;
    return NO_ERROR;
}

bool AudioHfpController::getHfpEnable(void)
{
    return mHfpEnable || mThreadAlive;
}

status_t AudioHfpController::enableHfpTask(void)
{
    ALOGD("%s", __FUNCTION__);

    if (mAudioHfpThread.get()) {
        ALOGW("%s AudioHfpThread existed already", __FUNCTION__);
        return NO_ERROR;
    }

    setWakeLockAcquire(true);

    mAudioHfpThread = new AudioHfpThread(this);
    if (!mAudioHfpThread.get()) {
        ALOGE("%s create thread fail!!", __FUNCTION__);
        return UNKNOWN_ERROR;
    }

    mAudioHfpThread->run("mAudioHfpThread");

    return NO_ERROR;
}

status_t AudioHfpController::disableHfpTask(void)
{
    ALOGD("+%s", __FUNCTION__);

    if (mAudioHfpThread.get()) {
        setThreadEnable(false);

        int ret = mAudioHfpThread->requestExitAndWait();
        if (ret == WOULD_BLOCK) {
            mAudioHfpThread->requestExit();
        }
        mAudioHfpThread.clear();
    }

    setWakeLockAcquire(false);

    ALOGD("-%s", __FUNCTION__);
    return NO_ERROR;
}

bool AudioHfpController::isHfpTaskRunning(void)
{
    return (mAudioHfpThread.get() != NULL);
}

status_t AudioHfpController::setHfpSampleRate(int sampleRate)
{
    ALOGD("%s sampleRate = %d", __FUNCTION__, sampleRate);

    if (sampleRate != 8000 && sampleRate != 16000) {
        ALOGW("%s invalid rate %d", __FUNCTION__, sampleRate);
        return BAD_VALUE;
    }

    mULTaskSinkSampleRate = mDLTaskSourceSampleRate = sampleRate;
    return NO_ERROR;
}

status_t AudioHfpController::setHfpVolume(int volume)
{
    ALOGD("%s volume = %d", __FUNCTION__, volume);

    if (volume > AUDIO_HFP_VOLUME_MAX_INDEX)
        volume = AUDIO_HFP_VOLUME_MAX_INDEX;
    else if (volume < 0)
        volume = 0;

    mHfpVolumeGain = (short)(((float) volume / (float)AUDIO_HFP_VOLUME_MAX_INDEX) *
                            (float)AUDIO_HFP_VOLUME_UNITY_GAIN);

    return NO_ERROR;
}

status_t AudioHfpController::setHfpMicMute(bool state)
{
#ifdef AUDIO_HFP_MIC_MUTE_SUPPORT
    ALOGD("%s state = %d", __FUNCTION__, state);
    mMicMute = state;
#endif
    return NO_ERROR;
}

status_t AudioHfpController::ULTaskStart()
{
    ULTaskSrctoDLConfig();
    return NO_ERROR;
}

status_t AudioHfpController::ULTaskStop()
{
    if (mAudioHfpULTaskSink)
        mAudioHfpULTaskSink->stop();

    if (mAudioHfpULTaskSource)
        mAudioHfpULTaskSource->stop();

    ULTaskSrctoDLClose();
    return NO_ERROR;
}

status_t AudioHfpController::ULTaskProcess()
{
    ALOGV("%s", __FUNCTION__);
    ULTaskSrctoDLProcess();
    applyMicMute();
    return NO_ERROR;
}

status_t AudioHfpController::setULTaskSourceAttribute()
{
    mAudioULTaskSourceAttribute.mFormat = AUDIO_HFP_PROCESS_FORMAT;
    mAudioULTaskSourceAttribute.mBytesPerSample = audio_bytes_per_sample((audio_format_t)mAudioULTaskSourceAttribute.mFormat);
    mAudioULTaskSourceAttribute.mSampleRate = mULTaskSourceSampleRate;
    mAudioULTaskSourceAttribute.mInterruptSample = mAudioULTaskSourceAttribute.mSampleRate * AUDIO_HFP_PROCESS_FRAME_MS / 1000;
    mAudioULTaskSourceAttribute.mChannels = mULTaskSourceChannelCount;
    mAudioULTaskSourceAttribute.mDevices = mULTaskInputDevice;
    mAudioULTaskSourceAttribute.mBufferSize = mAudioULTaskSourceAttribute.mInterruptSample *
                                              mAudioULTaskSourceAttribute.mChannels *
                                              mAudioULTaskSourceAttribute.mBytesPerSample;
    return NO_ERROR;
}

status_t AudioHfpController::ULTaskULSourceAllocate()
{
    ALOGD("%s mBufferSize = %d", __FUNCTION__, mAudioULTaskSourceAttribute.mBufferSize);

    if (mAudioULTaskInputBuffer == NULL) {
        mAudioULTaskInputBuffer = new char[mAudioULTaskSourceAttribute.mBufferSize];
        memset((void *)mAudioULTaskInputBuffer, 0, mAudioULTaskSourceAttribute.mBufferSize);
    } else {
        ALOGE("%s mAudioULTaskInputBuffer allocated already", __FUNCTION__);
    }

    mAudioULTaskUL1ProcessingBufferSize = mAudioHfpProcessing->GetFrameSize() * mAudioHfpProcessing->GetPcmFormatBytes();
    mAudioULTaskUL2ProcessingBufferSize = mAudioHfpProcessing->GetFrameSize() * mAudioHfpProcessing->GetPcmFormatBytes();

    if (mAudioULTaskUL1ProcessingBuffer == NULL) {
        mAudioULTaskUL1ProcessingBuffer = new char[mAudioULTaskUL1ProcessingBufferSize];
    }
    if (mAudioULTaskUL2ProcessingBuffer == NULL) {
        mAudioULTaskUL2ProcessingBuffer = new char[mAudioULTaskUL2ProcessingBufferSize];
    }

    if (!mAudioHfpULTaskSource) {
        mAudioHfpULTaskSource = new AudioHfpULTaskSource();
        mAudioHfpULTaskSource->prepare(mAudioULTaskSourceAttribute.mDevices,
                                       mAudioDLTaskSinkAttribute.mDevices,
                                       mAudioULTaskSourceAttribute.mFormat,
                                       mAudioULTaskSourceAttribute.mChannels,
                                       mAudioULTaskSourceAttribute.mSampleRate,
                                       mAudioULTaskSourceAttribute.mBufferSize);
    } else {
        ALOGE("%s mAudioHfpULTaskSource existed already", __FUNCTION__);
    }

	ALOGD("%s mAudioHfpULTaskInputDumpFile. ", __FUNCTION__);

    if (mAudioHfpULTaskInputDumpFile == NULL) {
        String8 dumpFileName;
        dumpFileName.appendFormat("%s.%d.%d_%dch.pcm", AudioHfpULTaskInput, mDumpFileNum,
                mAudioULTaskSourceAttribute.mSampleRate, mAudioULTaskSourceAttribute.mChannels);

		ALOGD("%s AudioHfpULTaskInput = %s", __FUNCTION__, AudioHfpULTaskInput);

        mAudioHfpULTaskInputDumpFile = AudioOpendumpPCMFile(dumpFileName.string(), streamhfp_propty);
    }

    return NO_ERROR;
}

status_t AudioHfpController::ULTaskULSourceStart()
{
    status_t ret;
    if (mAudioHfpULTaskSource){
        ret = mAudioHfpULTaskSource->start();
        if (ret != NO_ERROR)
            ALOGE("%s start fail", __FUNCTION__);
    }

    return NO_ERROR;
}

int AudioHfpController::ULTaskGetULDataSource()
{
    int ret = 0;
#ifdef DEBUG_AUDIO_LATENCY
    unsigned long long diffns = 0;
    struct timespec tstemp1, tstemp2;
    tstemp1 = GetSystemTime(false);
#endif

    // here to do read data from UL task UL data
    ret = mAudioHfpULTaskSource->read(mAudioULTaskInputBuffer, mAudioULTaskSourceAttribute.mBufferSize);

#ifdef DEBUG_AUDIO_LATENCY
    tstemp2 = GetSystemTime(false);
    diffns = TimeDifference(tstemp1, tstemp2);
    ALOGD("%s TimeDifference = %llu in ns %llu in us", __FUNCTION__, diffns, diffns / 1000);
#endif

    if (mAudioHfpULTaskInputDumpFile) {
        AudioDumpPCMData((void *)mAudioULTaskInputBuffer, mAudioULTaskSourceAttribute.mBufferSize, mAudioHfpULTaskInputDumpFile);
    }

    if (ret < 0) {
        ALOGW("%s read fail %d", __FUNCTION__, ret);
        mAudioULTaskInputBuffeSize = mAudioULTaskSourceAttribute.mBufferSize;
    } else if (ret != (int)mAudioULTaskSourceAttribute.mBufferSize) {
        mAudioULTaskInputBuffeSize = ret;
        ALOGW("%s read size %u != %u", __FUNCTION__, mAudioULTaskInputBuffeSize, mAudioULTaskSourceAttribute.mBufferSize);
    } else {
        mAudioULTaskInputBuffeSize = mAudioULTaskSourceAttribute.mBufferSize;
    }

    // estimate capture time
    mHfpULTaskCaptureTime = GetSystemTime(false);
    long dataLatencyNs = 1000000 * (mAudioHfpULTaskSource->getRemainDataLatency() +
                            calRemainTime(mAudioULTaskInputBuffeSize, mAudioULTaskSourceAttribute));

    if (mHfpULTaskCaptureTime.tv_nsec >= dataLatencyNs) {
        mHfpULTaskCaptureTime.tv_nsec -= dataLatencyNs;
    } else {
        mHfpULTaskCaptureTime.tv_sec -= 1;
        mHfpULTaskCaptureTime.tv_nsec = 1000000000 + mHfpULTaskCaptureTime.tv_nsec - dataLatencyNs;
    }

    return ret;
}

status_t AudioHfpController::ULTaskULSourceFree()
{
    ALOGD("%s", __FUNCTION__);

    if (mAudioHfpULTaskSource) {
        delete mAudioHfpULTaskSource;
        mAudioHfpULTaskSource = NULL;
    }

    if (mAudioULTaskInputBuffer) {
        delete [] mAudioULTaskInputBuffer;
        mAudioULTaskInputBuffer = NULL;
    }

    if (mAudioULTaskUL1ProcessingBuffer) {
        delete [] mAudioULTaskUL1ProcessingBuffer;
        mAudioULTaskUL1ProcessingBuffer = NULL;
    }

    if (mAudioULTaskUL2ProcessingBuffer) {
        delete [] mAudioULTaskUL2ProcessingBuffer;
        mAudioULTaskUL2ProcessingBuffer = NULL;
    }

    if (mAudioHfpULTaskInputDumpFile) {
        AudioCloseDumpPCMFile(mAudioHfpULTaskInputDumpFile);
        mAudioHfpULTaskInputDumpFile = NULL;
    }

    return NO_ERROR;
}

status_t AudioHfpController::setULTaskSinkAttribute()
{
    mAudioULTaskSinkAttribute.mFormat = AUDIO_HFP_PROCESS_FORMAT;
    mAudioULTaskSinkAttribute.mBytesPerSample = audio_bytes_per_sample((audio_format_t)mAudioULTaskSinkAttribute.mFormat);
    mAudioULTaskSinkAttribute.mSampleRate = mULTaskSinkSampleRate;
    mAudioULTaskSinkAttribute.mInterruptSample = mULTaskSinkSampleRate * AUDIO_HFP_PROCESS_FRAME_MS / 1000;
    mAudioULTaskSinkAttribute.mChannels = mULTaskSinkChannelCount;
    mAudioULTaskSinkAttribute.mBufferSize = mAudioULTaskSinkAttribute.mInterruptSample * mAudioULTaskSinkAttribute.mChannels *
                                            mAudioULTaskSinkAttribute.mBytesPerSample;
    return NO_ERROR;
}

status_t AudioHfpController::setBgsSoundAttribute()
{
    return setBgsSoundAttribute(44100, 2, AUDIO_HFP_PROCESS_FORMAT);
}

status_t AudioHfpController::setBgsSoundAttribute(uint32_t sampleRate, uint32_t chNum, int32_t format)
{
    ALOGD("%s sampleRate = %u chNum = %u format = %d", __FUNCTION__, sampleRate, chNum, format);
    mAudioBgsSoundAttribute.mFormat = format;
    mAudioBgsSoundAttribute.mBytesPerSample = audio_bytes_per_sample((audio_format_t)mAudioBgsSoundAttribute.mFormat);
    mAudioBgsSoundAttribute.mSampleRate = sampleRate;
    mAudioBgsSoundAttribute.mChannels = chNum;
    mAudioBgsSoundAttribute.mBufferSize = AUDIO_HFP_BGS_BUFFER_FRAME_COUNT * mAudioBgsSoundAttribute.mChannels *
                                          mAudioBgsSoundAttribute.mBytesPerSample;

    return NO_ERROR;
}

status_t AudioHfpController::ULTaskDLSinkAllocate()
{
    ALOGD("%s mBufferSize = %u", __FUNCTION__, mAudioULTaskSinkAttribute.mBufferSize);

    if (!mAudioHfpULTaskSink) {
        mAudioHfpULTaskSink = new AudioHfpULTaskSink();
        mAudioHfpULTaskSink->prepare(mAudioULTaskSinkAttribute.mFormat,
                                     mAudioULTaskSinkAttribute.mChannels,
                                     mAudioULTaskSinkAttribute.mSampleRate,
                                     mAudioULTaskSinkAttribute.mBufferSize);
    } else {
        ALOGE("%s mAudioHfpULTaskSink existed already", __FUNCTION__);
    }

    if (mAudioHfpULTaskOutputDumpFile == NULL) {
        String8 dumpFileName;
        dumpFileName.appendFormat("%s.%d.%d_%dch.pcm", AudioHfpULTaskOutput, mDumpFileNum,
                    mAudioULTaskSinkAttribute.mSampleRate, mAudioULTaskSinkAttribute.mChannels);
        mAudioHfpULTaskOutputDumpFile = AudioOpendumpPCMFile(dumpFileName.string(), streamhfp_propty);
    }

    return NO_ERROR;
}

status_t AudioHfpController::ULTaskDLSinkStart()
{
    // add pending data for first start
    if (mAudioHfpULTaskSink) {
        char *pendingData = new char[mAudioULTaskSinkAttribute.mBufferSize];
        memset(pendingData, 0, mAudioULTaskSinkAttribute.mBufferSize);

        for (int i = 0; i < AUDIO_HFP_SINK_PREFILL_BUFFER_COUNT; i++)
            mAudioHfpULTaskSink->write(pendingData, mAudioULTaskSinkAttribute.mBufferSize);

        if (pendingData) delete [] pendingData;
    }
    return NO_ERROR;
}

int AudioHfpController::ULTaskPutDLDataSink()
{
    int outputSize = 0;

#ifdef DEBUG_AUDIO_LATENCY
    unsigned long long diffns = 0;
    struct timespec tstemp1, tstemp2;
    tstemp1 = GetSystemTime(false);
#endif

    outputSize = mAudioHfpULTaskSink->write(mAudioULTaskOutputBuffer, mAudioULTaskOutputBufferSize);

#ifdef DEBUG_AUDIO_LATENCY
    tstemp2 = GetSystemTime(false);
    diffns = TimeDifference(tstemp1, tstemp2);
    ALOGD("%s TimeDifference = %llu in ns %llu in us", __FUNCTION__, diffns, diffns / 1000);
#endif

    if (mAudioHfpULTaskOutputDumpFile) {
        AudioDumpPCMData((void *)mAudioULTaskOutputBuffer , mAudioULTaskOutputBufferSize, mAudioHfpULTaskOutputDumpFile);
    }

    return outputSize;
}

status_t AudioHfpController::ULTaskDLSinkFree()
{
    ALOGD("%s", __FUNCTION__);

    if (mAudioHfpULTaskSink) {
        delete mAudioHfpULTaskSink;
        mAudioHfpULTaskSink = NULL;
    }

    if (mAudioHfpULTaskOutputDumpFile) {
        AudioCloseDumpPCMFile(mAudioHfpULTaskOutputDumpFile);
        mAudioHfpULTaskOutputDumpFile = NULL;
    }

    return NO_ERROR;
}

// UL data src to UL1 sampleRate
status_t AudioHfpController::ULTaskSrctoDLProcess()
{
    unsigned int consumeBytes = 0;
    unsigned int inLength = mAudioULTaskSourceAttribute.mBufferSize;
    unsigned int outLength = mAudioULTaskSrcBufferSize;

    if (mBliHandlerAudioULTaskSrc != NULL) {
        char *p_read = mAudioULTaskInputBuffer;
        uint32_t num_raw_data_left = inLength;
        uint32_t num_converted_data = outLength; // max convert num_free_space

        uint32_t consumed = num_raw_data_left;
        mBliHandlerAudioULTaskSrc->process((int16_t *)p_read, &num_raw_data_left, (int16_t *)mAudioULTaskSrcBuffer, &num_converted_data);
        consumed -= num_raw_data_left;
        p_read += consumed;

        if (num_raw_data_left > 0) {
            ALOGW("%s(), num_raw_data_left(%u) > 0", __FUNCTION__, num_raw_data_left);
            ASSERT(num_raw_data_left == 0);
        }

        mAudioULTaskOutputBuffer = mAudioULTaskSrcBuffer;
        mAudioULTaskOutputBufferSize = num_converted_data;
        ALOGV("%s , BLI_SRC num_raw_data_left = %u, num_converted_data = %u",
              __FUNCTION__, num_raw_data_left, num_converted_data);
    } else {
        mAudioULTaskOutputBuffer = mAudioULTaskInputBuffer;
        mAudioULTaskOutputBufferSize = inLength;
        ALOGV("%s no BLI_SRC mAudioULTaskOutputBufferSize = %u", __FUNCTION__, mAudioULTaskOutputBufferSize);
    }

    return NO_ERROR;
}

status_t AudioHfpController::ULTaskSrctoDLConfig()
{
    ALOGD("%s", __FUNCTION__);
    if (mAudioULTaskSourceAttribute.mSampleRate == mAudioULTaskSinkAttribute.mSampleRate) {
        ALOGD("%s no need to SRC mSampleRate = %d", __FUNCTION__, mAudioULTaskSourceAttribute.mSampleRate);
    } else {
        SRC_PCM_FORMAT src_pcm_format = SRC_IN_Q1P15_OUT_Q1P15;
        if (mAudioULTaskSourceAttribute.mFormat == AUDIO_FORMAT_PCM_32_BIT)
            src_pcm_format = SRC_IN_Q1P31_OUT_Q1P31;
        else if (mAudioULTaskSourceAttribute.mFormat == AUDIO_FORMAT_PCM_16_BIT)
            src_pcm_format = SRC_IN_Q1P15_OUT_Q1P15;
        else {
            ALOGE("%s(), not support mStreamAttributeSource->audio_format(0x%x) SRC!!", __FUNCTION__, mAudioULTaskSourceAttribute.mFormat);
        }

        mBliHandlerAudioULTaskSrc = newMtkAudioSrc(
                mAudioULTaskSourceAttribute.mSampleRate, mAudioULTaskSourceAttribute.mChannels,
                mAudioULTaskSinkAttribute.mSampleRate, mAudioULTaskSinkAttribute.mChannels,
                src_pcm_format);

        ASSERT(mBliHandlerAudioULTaskSrc != NULL);
        mBliHandlerAudioULTaskSrc->open();

        if (mAudioULTaskSourceAttribute.mBufferSize > mAudioULTaskSinkAttribute.mBufferSize)
            mAudioULTaskSrcBufferSize = mAudioULTaskSourceAttribute.mBufferSize * 2;
        else
            mAudioULTaskSrcBufferSize = mAudioULTaskSinkAttribute.mBufferSize * 2;

        mAudioULTaskSrcBuffer = new char[mAudioULTaskSrcBufferSize];
    }

    return NO_ERROR;
}

status_t AudioHfpController::ULTaskSrctoDLClose()
{
    ALOGD("%s", __FUNCTION__);
    if (mBliHandlerAudioULTaskSrc != NULL) {
        deleteMtkAudioSrc(mBliHandlerAudioULTaskSrc);
        mBliHandlerAudioULTaskSrc = NULL;
    }
    if (mAudioULTaskSrcBuffer != NULL) {
        delete [] mAudioULTaskSrcBuffer;
        mAudioULTaskSrcBuffer = NULL;
    }
    if (mAudioULTaskSrcWorkingBuffer != NULL) {
        delete [] mAudioULTaskSrcWorkingBuffer;
        mAudioULTaskSrcWorkingBuffer = NULL;
    }
    return NO_ERROR;
}

int AudioHfpController::calRemainTime(int bufferSize, AudioHFPStreamAttribute attr)
{
    int timeMs = 0;
    if (bufferSize >= 0) {
        timeMs = (bufferSize * 1000) / (attr.mSampleRate * attr.mChannels * attr.mBytesPerSample);
    }
    return timeMs;
}

/*
short clamp16(int sample)
{
    if ((sample >> 15) ^ (sample >> 31)) {
        sample = 0x7FFF ^ (sample >> 31);
    }
    return sample;
}
*/

status_t AudioHfpController::addAndClamp16(short *dst, short *src, uint32_t bufferSize, uint32_t frameSize)
{
    unsigned int frameNum = bufferSize / frameSize;
    unsigned int temp = 0;

    while (frameNum)
    {
        temp = (*dst) + (*src);
        *dst = clamp16(temp);
        src++;
        dst++;
        frameNum--;
    }
    return NO_ERROR;
}

int AudioHfpController::compareTime(struct timespec time1, struct timespec time2)
{
    int ret = 0;

    if (time1.tv_sec > time2.tv_sec) {
        ret = 1;
    } else if (time1.tv_sec == time2.tv_sec) {
        if (time1.tv_nsec > time2.tv_nsec)
            ret = 1;
        else if (time1.tv_nsec < time2.tv_nsec)
            ret = -1;
        else
            ret = 0;
    } else {
        ret = -1;
    }
    return ret;
}

void AudioHfpController::setWakeLockAcquire(bool acquire)
{
    if (acquire && !mWakeLockAcquired) {
        acquire_wake_lock(PARTIAL_WAKE_LOCK, AUDIO_HFP_WAKE_LOCK_NAME);
        mWakeLockAcquired = true;
    } else if (!acquire && mWakeLockAcquired) {
        release_wake_lock(AUDIO_HFP_WAKE_LOCK_NAME);
        mWakeLockAcquired = false;
    }
}

void AudioHfpController::applyMicMute()
{
    if (mMicMute && mAudioULTaskOutputBuffer)
        memset(mAudioULTaskOutputBuffer, 0, mAudioULTaskOutputBufferSize);
}

void AudioHfpController::updateParameters()
{
    if (mAudioHfpProcessing)
        mAudioHfpProcessing->UpdateParameters();
}

status_t AudioHfpController::DL1DataBgsMixing()
{
    pthread_mutex_lock(&mAudioBgsLock);
    unsigned int bgsDataSize = RingBuf_getDataCount(&mBgsSoundBuffer);
    unsigned int mixSize = (bgsDataSize > mAudioDLTaskOutputBufferSize) ? mAudioDLTaskOutputBufferSize : bgsDataSize;

    if (mixSize > 0) {
        ALOGV("%s mixSize = %u mAudioDLTaskOutputBufferSize = %u", __FUNCTION__,
              mixSize, mAudioDLTaskOutputBufferSize);
        // copy data from ring buffer
        RingBuf_copyToLinear(mAudioBgsSoundSrcBuffer, &mBgsSoundBuffer, mixSize);
        addAndClamp16((short*)mAudioDLTaskOutputBuffer, (short*)mAudioBgsSoundSrcBuffer, mixSize, mAudioDLTaskSinkAttribute.mBytesPerSample);
    }

    // signal for normal playback
    pthread_cond_signal(&mAudioBgsWaitWorkCV);
    pthread_mutex_unlock(&mAudioBgsLock);
    return NO_ERROR;
}

// DL data src to DL1 sampleRate
status_t AudioHfpController::DLTaskSrctoDLProcess()
{
    unsigned int inLength = mAudioDLTaskSourceAttribute.mBufferSize;
    unsigned int outLength = mAudioDLTaskSrcBufferSize;

    if (mBliHandlerAudioDLTaskSrc != NULL) {
        char *p_read = mAudioDLTaskInputBuffer;
        uint32_t num_raw_data_left = inLength;
        uint32_t num_converted_data = outLength; // max convert num_free_space

        uint32_t consumed = num_raw_data_left;
        mBliHandlerAudioDLTaskSrc->process((int16_t *)p_read, &num_raw_data_left,
               (int16_t *)mAudioDLTaskSrcBuffer, &num_converted_data);
        consumed -= num_raw_data_left;
        p_read += consumed;

        if (num_raw_data_left > 0) {
            ALOGW("%s(), num_raw_data_left(%u) > 0", __FUNCTION__, num_raw_data_left);
            ASSERT(num_raw_data_left == 0);
        }

        mAudioDLTaskOutputBuffer = mAudioDLTaskSrcBuffer;
        mAudioDLTaskOutputBufferSize = num_converted_data;
        ALOGV("%s , BLI_SRC num_raw_data_left = %u, num_converted_data = %u",
              __FUNCTION__, num_raw_data_left, num_converted_data);
    } else {
        ALOGV("%s no BLI_SRC", __FUNCTION__);
        mAudioDLTaskOutputBuffer = mAudioDLTaskInputBuffer;
        mAudioDLTaskOutputBufferSize = inLength;
    }

    return NO_ERROR;
}

status_t AudioHfpController::setThreadEnable(bool enable)
{
    ALOGD("%s enable = %d", __FUNCTION__, enable);
    mThreadEnable = enable;
    return NO_ERROR;
}

bool AudioHfpController::getThreadEnable()
{
    return mThreadEnable;
}

status_t AudioHfpController::DLTaskSrctoDLConfig()
{
    ALOGD("%s", __FUNCTION__);
    if (mAudioDLTaskSourceAttribute.mSampleRate == mAudioDLTaskSinkAttribute.mSampleRate) {
        ALOGD("%s no need to SRC mSampleRate = %d", __FUNCTION__, mAudioDLTaskSourceAttribute.mSampleRate);
    } else {
        SRC_PCM_FORMAT src_pcm_format = SRC_IN_Q1P15_OUT_Q1P15;
        if (mAudioDLTaskSourceAttribute.mFormat == AUDIO_FORMAT_PCM_32_BIT)
            src_pcm_format = SRC_IN_Q1P31_OUT_Q1P31;
        else if (mAudioDLTaskSourceAttribute.mFormat == AUDIO_FORMAT_PCM_16_BIT)
            src_pcm_format = SRC_IN_Q1P15_OUT_Q1P15;
        else {
            ALOGE("%s(), not support mStreamAttributeSource->audio_format(0x%x) SRC!!",
              __FUNCTION__, mAudioDLTaskSourceAttribute.mFormat);
        }

        mBliHandlerAudioDLTaskSrc = newMtkAudioSrc(
            mAudioDLTaskSourceAttribute.mSampleRate, mAudioDLTaskSourceAttribute.mChannels,
            mAudioDLTaskSinkAttribute.mSampleRate, mAudioDLTaskSinkAttribute.mChannels,
            src_pcm_format);

        ASSERT(mBliHandlerAudioDLTaskSrc != NULL);
        mBliHandlerAudioDLTaskSrc->open();

        if (mAudioDLTaskSourceAttribute.mBufferSize > mAudioDLTaskSinkAttribute.mBufferSize)
            mAudioDLTaskSrcBufferSize = mAudioDLTaskSourceAttribute.mBufferSize * 2;
        else
            mAudioDLTaskSrcBufferSize = mAudioDLTaskSinkAttribute.mBufferSize * 2;
        mAudioDLTaskSrcBuffer = new char[mAudioDLTaskSrcBufferSize];
    }

    return NO_ERROR;
}

status_t AudioHfpController::DLTaskSrctoDLClose()
{
    ALOGD("%s", __FUNCTION__);
    if (mBliHandlerAudioDLTaskSrc != NULL) {
        deleteMtkAudioSrc(mBliHandlerAudioDLTaskSrc);
        mBliHandlerAudioDLTaskSrc = NULL;
    }
    if (mAudioDLTaskSrcBuffer != NULL) {
        delete [] mAudioDLTaskSrcBuffer;
        mAudioDLTaskSrcBuffer = NULL;
    }
    if (mAudioDLTaskSrcWorkingBuffer != NULL) {
        delete [] mAudioDLTaskSrcWorkingBuffer;
        mAudioDLTaskSrcWorkingBuffer = NULL;
    }
    return NO_ERROR;
}

status_t AudioHfpController::DLTaskStart()
{
    DLTaskSrctoDLConfig();
    return NO_ERROR;
}

status_t AudioHfpController::DLTaskStop()
{
    if (mAudioHfpDLTaskSink)
        mAudioHfpDLTaskSink->stop();

    if (mAudioHfpDLTaskSource)
        mAudioHfpDLTaskSource->stop();

    DLTaskSrctoDLClose();
    return NO_ERROR;
}

status_t AudioHfpController::DLTaskProcess()
{
    ALOGV("%s", __FUNCTION__);
    DLTaskSrctoDLProcess();

    DL1DataAligned();

    applyGainOnDL1Data();

    DL1DataBgsMixing();
    return NO_ERROR;
}

status_t AudioHfpController::setDLTaskSourceAttribute()
{
    mAudioDLTaskSourceAttribute.mFormat = AUDIO_HFP_PROCESS_FORMAT;
    mAudioDLTaskSourceAttribute.mBytesPerSample = audio_bytes_per_sample((audio_format_t)mAudioDLTaskSourceAttribute.mFormat);
    mAudioDLTaskSourceAttribute.mSampleRate = mDLTaskSourceSampleRate;
    mAudioDLTaskSourceAttribute.mInterruptSample = mAudioDLTaskSourceAttribute.mSampleRate * AUDIO_HFP_PROCESS_FRAME_MS / 1000;
    mAudioDLTaskSourceAttribute.mChannels = mDLTaskSourceChannelCount;
    mAudioDLTaskSourceAttribute.mBufferSize = mAudioDLTaskSourceAttribute.mInterruptSample * mAudioDLTaskSourceAttribute.mChannels *
                                              mAudioDLTaskSourceAttribute.mBytesPerSample;
    return NO_ERROR;
}

status_t AudioHfpController::DLTaskULSourceAllocate()
{
    ALOGD("%s", __FUNCTION__);

    if (mAudioDLTaskInputBuffer == NULL) {
        mAudioDLTaskInputBuffer = new char[mAudioDLTaskSourceAttribute.mBufferSize];
    } else {
        ALOGE("%s mAudioDLTaskInputBuffer allocated already", __FUNCTION__);
    }

    if (!mAudioHfpDLTaskSource) {
        mAudioHfpDLTaskSource = new AudioHfpDLTaskSource();
        mAudioHfpDLTaskSource->prepare(mAudioDLTaskSourceAttribute.mFormat,
                                       mAudioDLTaskSourceAttribute.mChannels,
                                       mAudioDLTaskSourceAttribute.mSampleRate,
                                       mAudioDLTaskSourceAttribute.mBufferSize);
    } else {
        ALOGE("%s mAudioHfpDLTaskSource existed already", __FUNCTION__);
    }

	 ALOGD("%s , mAudioHfpDLTaskInputDumpFile .", __FUNCTION__);

    if (mAudioHfpDLTaskInputDumpFile == NULL) {
        String8 dumpFileName;
        dumpFileName.appendFormat("%s.%d.%d_%dch.pcm", AudioHfpDLTaskInput, mDumpFileNum,
                    mAudioDLTaskSourceAttribute.mSampleRate, mAudioDLTaskSourceAttribute.mChannels);
        mAudioHfpDLTaskInputDumpFile = AudioOpendumpPCMFile(dumpFileName.string(), streamhfp_propty);
    }

    return NO_ERROR;
}

status_t AudioHfpController::DLTaskULSourceStart()
{
    status_t ret;
    if (mAudioHfpDLTaskSource){
        ret = mAudioHfpDLTaskSource->start();
        if (ret != NO_ERROR)
            ALOGE("%s start fail", __FUNCTION__);
    }

    return NO_ERROR;
}

int AudioHfpController::DLTaskGetULDataSource()
{
    int ret = 0;
#ifdef DEBUG_AUDIO_LATENCY
    unsigned long long diffns = 0;
    struct timespec tstemp1, tstemp2;
    tstemp1 = GetSystemTime(false);
#endif

    // here to do read data from DL task UL data
    ret = mAudioHfpDLTaskSource->read(mAudioDLTaskInputBuffer, mAudioDLTaskSourceAttribute.mBufferSize);

    ALOGV("%s read ret= %d", __FUNCTION__, ret);

#ifdef DEBUG_AUDIO_LATENCY
    tstemp2 = GetSystemTime(false);
    diffns = TimeDifference(tstemp1, tstemp2);
    ALOGD("%s TimeDifference = %llu in ns %llu in us", __FUNCTION__, diffns, diffns / 1000);
#endif

    if (mAudioHfpDLTaskInputDumpFile) {
        AudioDumpPCMData((void *)mAudioDLTaskInputBuffer, mAudioDLTaskSourceAttribute.mBufferSize, mAudioHfpDLTaskInputDumpFile);
    }

    if (ret < 0)
        ALOGW("%s read fail %d", __FUNCTION__, ret);
    else if (ret != (int)mAudioDLTaskSourceAttribute.mBufferSize)
        ALOGW("%s read size %d != %u", __FUNCTION__, ret, mAudioDLTaskSourceAttribute.mBufferSize);

    return ret;
}

status_t AudioHfpController::DLTaskULSourceFree()
{
    ALOGD("%s", __FUNCTION__);

    if (mAudioHfpDLTaskSource) {
        delete mAudioHfpDLTaskSource;
        mAudioHfpDLTaskSource = NULL;
    }

    if (mAudioDLTaskInputBuffer) {
        delete [] mAudioDLTaskInputBuffer;
        mAudioDLTaskInputBuffer = NULL;
    }

    if (mAudioHfpDLTaskInputDumpFile) {
        AudioCloseDumpPCMFile(mAudioHfpDLTaskInputDumpFile);
        mAudioHfpDLTaskInputDumpFile = NULL;
    }

    return NO_ERROR;
}

status_t AudioHfpController::setDLTaskSinkAttribute()
{
    mAudioDLTaskSinkAttribute.mFormat = AUDIO_HFP_PROCESS_FORMAT;
    mAudioDLTaskSinkAttribute.mBytesPerSample = audio_bytes_per_sample((audio_format_t)mAudioDLTaskSinkAttribute.mFormat);
    mAudioDLTaskSinkAttribute.mSampleRate = mDLTaskSinkSampleRate;
    mAudioDLTaskSinkAttribute.mInterruptSample = mDLTaskSinkSampleRate * AUDIO_HFP_PROCESS_FRAME_MS / 1000;
    mAudioDLTaskSinkAttribute.mChannels = mDLTaskSinkChannelCount;
    mAudioDLTaskSinkAttribute.mDevices = mDLTaskOutputDevice;
    mAudioDLTaskSinkAttribute.mBufferSize = mAudioDLTaskSinkAttribute.mInterruptSample * mAudioDLTaskSinkAttribute.mChannels *
                                            mAudioDLTaskSinkAttribute.mBytesPerSample;
    return NO_ERROR;
}

status_t AudioHfpController::setDLEchoRefAttribute()
{
    mAudioDLEchoRefAttribute.mFormat = AUDIO_HFP_PROCESS_FORMAT;
    mAudioDLEchoRefAttribute.mBytesPerSample = audio_bytes_per_sample((audio_format_t)mAudioDLEchoRefAttribute.mFormat);
    mAudioDLEchoRefAttribute.mSampleRate = mDLTaskSinkSampleRate;
    mAudioDLEchoRefAttribute.mInterruptSample = mDLTaskSinkSampleRate * AUDIO_HFP_PROCESS_FRAME_MS / 1000;
    mAudioDLEchoRefAttribute.mChannels = mDLEchoRefChannelCount;
    mAudioDLEchoRefAttribute.mBufferSize = mAudioDLEchoRefAttribute.mInterruptSample * mAudioDLEchoRefAttribute.mChannels *
                                           mAudioDLEchoRefAttribute.mBytesPerSample;
    return NO_ERROR;
}

status_t AudioHfpController::DLTaskDLSinkAllocate()
{
    mAudioDLTaskDL1ProcessingBufferSize = mAudioHfpProcessing->GetFrameSize() * mAudioHfpProcessing->GetPcmFormatBytes();
    mAudioULTaskDL2ProcessingBufferSize = mAudioHfpProcessing->GetFrameSize() * mAudioHfpProcessing->GetPcmFormatBytes();

    ALOGD("%s mAudioDLTaskDL1ProcessingBufferSize = %d mAudioULTaskDL2ProcessingBufferSize = %d",
          __FUNCTION__ , mAudioDLTaskDL1ProcessingBufferSize, mAudioULTaskDL2ProcessingBufferSize);

    if (mAudioDLTaskDL1ProcessingBuffer == NULL) {
        mAudioDLTaskDL1ProcessingBuffer = new char[mAudioDLTaskDL1ProcessingBufferSize];
    }

    if (mAudioDLTaskDL2ProcessingBuffer == NULL) {
        mAudioDLTaskDL2ProcessingBuffer = new char[mAudioULTaskDL2ProcessingBufferSize];
    }

    if (!mAudioHfpDLTaskSink) {
        mAudioHfpDLTaskSink = new AudioHfpDLTaskSink();
        mAudioHfpDLTaskSink->prepare(mAudioDLTaskSinkAttribute.mDevices,
                                     mAudioDLTaskSinkAttribute.mFormat,
                                     mAudioDLTaskSinkAttribute.mChannels,
                                     mAudioDLTaskSinkAttribute.mSampleRate,
                                     mAudioDLTaskSinkAttribute.mBufferSize);
    } else {
        ALOGE("%s mAudioHfpDLTaskSink existed already", __FUNCTION__);
    }

    if (mAudioHfpDLTaskOutputDumpFile == NULL) {
        String8 dumpFileName;
        dumpFileName.appendFormat("%s.%d.%d_%dch.pcm", AudioHfpDLTaskOutput, mDumpFileNum,
                    mAudioDLTaskSinkAttribute.mSampleRate, mAudioDLTaskSinkAttribute.mChannels);

		ALOGD("%s AudioHfpDLTaskOutput = %s", __FUNCTION__, AudioHfpDLTaskOutput);

        mAudioHfpDLTaskOutputDumpFile = AudioOpendumpPCMFile(dumpFileName.string(), streamhfp_propty);
    }

    return NO_ERROR;
}

status_t AudioHfpController::DLTaskDLSinkStart()
{
    // add pending data for first start
    if (mAudioHfpDLTaskSink) {
        char *pendingData = new char[mAudioDLTaskSinkAttribute.mBufferSize];
        memset(pendingData, 0, mAudioDLTaskSinkAttribute.mBufferSize);

        for (int i = 0; i < AUDIO_HFP_SINK_PREFILL_BUFFER_COUNT; i++)
            mAudioHfpDLTaskSink->write(pendingData, mAudioDLTaskSinkAttribute.mBufferSize);

        if (pendingData)
            delete [] pendingData;
    }

    return NO_ERROR;
}

int AudioHfpController::DLTaskPutDLDataSink()
{
    int outputSize = 0;
#ifdef DEBUG_AUDIO_LATENCY
    unsigned long long diffns = 0;
    struct timespec tstemp1, tstemp2;
    tstemp1 = GetSystemTime(false);
#endif

    outputSize = mAudioHfpDLTaskSink->write(mAudioDLTaskOutputBuffer, mAudioDLTaskOutputBufferSize);

#ifdef DEBUG_AUDIO_LATENCY
    tstemp2 = GetSystemTime(false);
    diffns = TimeDifference(tstemp1, tstemp2);
    ALOGD("%s TimeDifference = %llu in ns %llu in us", __FUNCTION__, diffns, diffns / 1000);
#endif

    if (mAudioHfpDLTaskOutputDumpFile) {
        AudioDumpPCMData((void *)mAudioDLTaskOutputBuffer , mAudioDLTaskOutputBufferSize, mAudioHfpDLTaskOutputDumpFile);
    }

    return outputSize;
}

status_t AudioHfpController::DLTaskDLSinkFree()
{
    ALOGD("%s", __FUNCTION__);

    if (mAudioHfpDLTaskSink) {
        delete mAudioHfpDLTaskSink;
        mAudioHfpDLTaskSink = NULL;
    }

    if (mAudioDLTaskDL1ProcessingBuffer) {
        delete [] mAudioDLTaskDL1ProcessingBuffer;
        mAudioDLTaskDL1ProcessingBuffer = NULL;
    }

    if (mAudioDLTaskDL2ProcessingBuffer) {
        delete [] mAudioDLTaskDL2ProcessingBuffer;
        mAudioDLTaskDL2ProcessingBuffer = NULL;
    }

    if (mAudioHfpDLTaskOutputDumpFile) {
        AudioCloseDumpPCMFile(mAudioHfpDLTaskOutputDumpFile);
        mAudioHfpDLTaskOutputDumpFile = NULL;
    }

    return NO_ERROR;
}


status_t AudioHfpController::putUL1UL2DataBuffer()
{
    uint32_t frameNumber = mAudioULTaskInputBuffeSize / mAudioHfpProcessing->GetPcmFormatBytes();
    short *processingBuffer = (short *)mAudioULTaskInputBuffer;
    short *processingUL1Buffer = (short *)mAudioULTaskUL1ProcessingBuffer;
    short *processingUL2Buffer = (short *)mAudioULTaskUL2ProcessingBuffer;

    // copy UL1 and UL2 data to temp buffer
    while (frameNumber)
    {
        *processingUL1Buffer = *processingBuffer;
        processingUL1Buffer++;
        processingBuffer++;
        frameNumber--;
        *processingUL2Buffer = *processingBuffer;
        processingUL2Buffer++;
        processingBuffer++;
        frameNumber--;
    }
    return NO_ERROR;
}

status_t AudioHfpController::putUL1UL2DataProcessing()
{
    mAudioHfpProcessing->PutUl1Data(mAudioULTaskUL1ProcessingBuffer, mAudioULTaskUL1ProcessingBufferSize);
    mAudioHfpProcessing->PutUl2Data(mAudioULTaskUL2ProcessingBuffer, mAudioULTaskUL2ProcessingBufferSize);
    return NO_ERROR;
}

status_t AudioHfpController::putDL1DL2DataProcessing()
{
    // here we only put DL1 because of only support mono
    mAudioHfpProcessing->PutDLData(mAudioDLTaskDL1ProcessingBuffer, mAudioDLTaskDL1ProcessingBufferSize);
    return NO_ERROR;
}

status_t AudioHfpController::getUL1UL2DataBuffer()
{
    unsigned int UL1BufferFrameSize;
    short *UL1Buffer = (short *)mAudioHfpProcessing->GutUl1Data(&UL1BufferFrameSize);
    short *ULTaskInputBuffer = (short *)mAudioULTaskInputBuffer;

    while (UL1BufferFrameSize)
    {
        *ULTaskInputBuffer++ = *UL1Buffer;
        *ULTaskInputBuffer++ = *UL1Buffer;
        UL1Buffer++;
        UL1BufferFrameSize--;
    }
    return NO_ERROR;
}

status_t AudioHfpController::getDL1DL2DataBuffer()
{
    short *DL1Buffer, *DLTaskOutputBuffer;
    unsigned int DL1BufferFrameSize;
    DL1Buffer = (short *)mAudioHfpProcessing->GutDLData(&DL1BufferFrameSize);
    DLTaskOutputBuffer = (short *)mAudioDLTaskOutputBuffer;

    while (DL1BufferFrameSize)
    {
        *DLTaskOutputBuffer++ = *DL1Buffer;
        *DLTaskOutputBuffer++ = *DL1Buffer++;
        DL1BufferFrameSize--;
    }
    return NO_ERROR;
}

status_t AudioHfpController::DLEchoRefSourceAllocate()
{
    mEchoRefBuffer.bufLen = mAudioDLEchoRefAttribute.mBufferSize * AUDIO_HFP_ECHO_REFERENCE_BUFFER_COUNT;
    mEchoRefBuffer.pBufBase = new char[mEchoRefBuffer.bufLen];
    memset(mEchoRefBuffer.pBufBase, 0, mEchoRefBuffer.bufLen);
    mEchoRefBuffer.pWrite = mEchoRefBuffer.pBufBase;
    mEchoRefBuffer.pRead = mEchoRefBuffer.pBufBase;

    mEchoRefTempBufferSize = mAudioDLEchoRefAttribute.mBufferSize * 4;
    mEchoRefTempBuffer = new char[mEchoRefTempBufferSize];
    mEchoRefBufferAligned = false;
    mFirstEchoRefDataArrived = false;

#ifdef AUDIO_HFP_ECHO_REFERENCE_FROM_AWB
    mEchoRefReadBuffer = new char[mAudioDLEchoRefAttribute.mBufferSize];

    if (!mAudioHfpDLEchoRefSource) {
        mAudioHfpDLEchoRefSource = new AudioHfpDLEchoRefSource();
        mAudioHfpDLEchoRefSource->prepare(mAudioULTaskSourceAttribute.mDevices,
                                       mAudioDLTaskSinkAttribute.mDevices,
                                       mAudioDLEchoRefAttribute.mFormat,
                                       mAudioDLEchoRefAttribute.mChannels,
                                       mAudioDLEchoRefAttribute.mSampleRate,
                                       mAudioDLEchoRefAttribute.mBufferSize);
    } else {
        ALOGE("%s mAudioHfpDLEchoRefSource existed already", __FUNCTION__);
    }
#endif

    if (mAudioHfpDLEchoRefDumpFile == NULL) {
        String8 dumpFileName;
        // echo ref only have mono data
        dumpFileName.appendFormat("%s.%d.%d_%dch.pcm", AudioHfpDLEchoRef, mDumpFileNum,
                mAudioDLEchoRefAttribute.mSampleRate, mAudioDLEchoRefAttribute.mChannels);
        mAudioHfpDLEchoRefDumpFile = AudioOpendumpPCMFile(dumpFileName.string(), streamhfp_propty);
    }

    return NO_ERROR;
}

status_t AudioHfpController::DLEchoRefSourceFree()
{
#ifdef AUDIO_HFP_ECHO_REFERENCE_FROM_AWB
    if (mAudioHfpDLEchoRefSource) {
        delete mAudioHfpDLEchoRefSource;
        mAudioHfpDLEchoRefSource = NULL;
    }

    if (mEchoRefReadBuffer) {
        delete [] mEchoRefReadBuffer;
        mEchoRefReadBuffer = NULL;
    }
#endif

    if (mEchoRefBuffer.pBufBase) {
        delete [] mEchoRefBuffer.pBufBase;
        memset(&mEchoRefBuffer, 0, sizeof(mEchoRefBuffer));
    }

    if (mEchoRefTempBuffer) {
        delete [] mEchoRefTempBuffer;
        mEchoRefTempBuffer = NULL;
    }

    if (mAudioHfpDLEchoRefDumpFile) {
        AudioCloseDumpPCMFile(mAudioHfpDLEchoRefDumpFile);
        mAudioHfpDLEchoRefDumpFile = NULL;
    }

    return NO_ERROR;
}

status_t AudioHfpController::DLEchoRefSourceStart()
{
#ifdef AUDIO_HFP_ECHO_REFERENCE_FROM_AWB
    status_t ret;
    if (mAudioHfpDLEchoRefSource){
        ret = mAudioHfpDLEchoRefSource->start();
        if (ret != NO_ERROR)
            ALOGE("%s start fail", __FUNCTION__);
    }
#endif
    return NO_ERROR;
}

status_t AudioHfpController::DLEchoRefSourceStop()
{
#ifdef AUDIO_HFP_ECHO_REFERENCE_FROM_AWB
    if (mAudioHfpDLEchoRefSource)
        mAudioHfpDLEchoRefSource->stop();
#endif
    return NO_ERROR;
}

int AudioHfpController::getDLEchoRefSource()
{
    int ret = 0;
#ifdef AUDIO_HFP_ECHO_REFERENCE_FROM_AWB

#ifdef DEBUG_AUDIO_LATENCY
    unsigned long long diffns = 0;
    struct timespec tstemp1, tstemp2;
    tstemp1 = GetSystemTime(false);
#endif

    ret = mAudioHfpDLEchoRefSource->read(mEchoRefReadBuffer, mAudioDLEchoRefAttribute.mBufferSize);

	ALOGV("%s read ret= %d", __FUNCTION__, ret);

#ifdef DEBUG_AUDIO_LATENCY
    tstemp2 = GetSystemTime(false);
    diffns = TimeDifference(tstemp1, tstemp2);
    ALOGD("%s TimeDifference = %llu in ns %llu in us", __FUNCTION__, diffns, diffns / 1000);
#endif

    if (ret < 0)
        ALOGW("%s read fail %d", __FUNCTION__, ret);
    else if (ret != (int)mAudioDLEchoRefAttribute.mBufferSize)
        ALOGW("%s read size %d != %u", __FUNCTION__, ret, mAudioDLEchoRefAttribute.mBufferSize);

#endif
    return ret;
}

status_t AudioHfpController::putDLEchoRefBuffer()
{
    if (!mFirstEchoRefDataArrived) {
        mFirstEchoRefDataPlaybackTime = GetSystemTime(false);

#ifdef AUDIO_HFP_ECHO_REFERENCE_FROM_AWB
        long dataLatencyNs = -1000000 * (long)(mAudioHfpDLEchoRefSource->getRemainDataLatency());
#else
        long dataLatencyNs = 1000000 * (long)(mAudioHfpDLTaskSink->getRemainDataLatency());
#endif

        // consider platform jitter
        long totalLatencyNs = dataLatencyNs + AUDIO_HFP_PLATFORM_JITTER_NS;

        ALOGD("%s totalLatencyNs = %ld ns dataLatencyNs = %ld ns jitter = %d ns",
              __FUNCTION__, totalLatencyNs, dataLatencyNs, AUDIO_HFP_PLATFORM_JITTER_NS);

        if (totalLatencyNs > 0) {
            if ((mFirstEchoRefDataPlaybackTime.tv_nsec + totalLatencyNs) >= 1000000000) {
                mFirstEchoRefDataPlaybackTime.tv_sec++;
                mFirstEchoRefDataPlaybackTime.tv_nsec = mFirstEchoRefDataPlaybackTime.tv_nsec + totalLatencyNs - 1000000000;
            } else {
                mFirstEchoRefDataPlaybackTime.tv_nsec += totalLatencyNs;
            }
        } else if (totalLatencyNs < 0) {
            if (mFirstEchoRefDataPlaybackTime.tv_nsec > totalLatencyNs * -1) {
                mFirstEchoRefDataPlaybackTime.tv_nsec += totalLatencyNs;
            } else {
                mFirstEchoRefDataPlaybackTime.tv_sec--;
                mFirstEchoRefDataPlaybackTime.tv_nsec = mFirstEchoRefDataPlaybackTime.tv_nsec + 1000000000 + totalLatencyNs;
            }
        }

        mFirstEchoRefDataArrived = true;
    }

    // copy data to mEchoRefBuffer
    uint32_t freeSize = RingBuf_getFreeSpace(&mEchoRefBuffer);
#ifdef AUDIO_HFP_ECHO_REFERENCE_FROM_AWB
    uint32_t dataSize = mAudioDLEchoRefAttribute.mBufferSize;
    char *srcBuf = mEchoRefReadBuffer;
#else
    uint32_t dataSize = mAudioDLTaskDL1ProcessingBufferSize;
    char *srcBuf = mAudioDLTaskDL1ProcessingBuffer;
#endif

    ALOGV("%s dataSize = %u freeSize = %u", __FUNCTION__, dataSize, freeSize);

    if (freeSize >= dataSize) {
        RingBuf_copyFromLinear(&mEchoRefBuffer, srcBuf, dataSize);
    } else {
        ALOGW("%s no enough data (%u < %u)", __FUNCTION__, freeSize, dataSize);
    }
    return NO_ERROR;
}

status_t AudioHfpController::putDLEchoRefPrcoessing()
{
    uint32_t processSize = mAudioDLEchoRefAttribute.mBufferSize;

    if (mEchoRefBufferAligned == false)
    {
        if (compareTime(mFirstEchoRefDataPlaybackTime, mHfpULTaskCaptureTime) >= 0)
        {
            ALOGV("%s capture time earlier than echo ref time %lu:%lu -> %lu:%lu",
                  __FUNCTION__, mHfpULTaskCaptureTime.tv_sec, mHfpULTaskCaptureTime.tv_nsec,
                  mFirstEchoRefDataPlaybackTime.tv_sec, mFirstEchoRefDataPlaybackTime.tv_nsec);

            unsigned long long timeDiffMs = TimeDifference(mFirstEchoRefDataPlaybackTime, mHfpULTaskCaptureTime) / 1000000ULL;
            if (timeDiffMs >= AUDIO_HFP_PROCESS_FRAME_MS) {
                ALOGV("%s fill zero data and wait %llu ms to align", __FUNCTION__, timeDiffMs);
                memset(mEchoRefTempBuffer, 0, processSize);
            } else {
                int paddingZeroSize = timeDiffMs * mAudioDLEchoRefAttribute.mSampleRate *
                                      mAudioDLEchoRefAttribute.mChannels *
                                      mAudioDLEchoRefAttribute.mBytesPerSample / 1000;
                int fillDataSize = processSize - paddingZeroSize;
                int availDataSize = RingBuf_getDataCount(&mEchoRefBuffer);

                ALOGV("%s align within %llu ms", __FUNCTION__, timeDiffMs);

                if (paddingZeroSize > 0)
                    memset(mEchoRefTempBuffer, 0, paddingZeroSize);

                if (availDataSize >= fillDataSize) {
                    RingBuf_copyToLinear(mEchoRefTempBuffer + paddingZeroSize, &mEchoRefBuffer, fillDataSize);
                } else {
                    RingBuf_copyToLinear(mEchoRefTempBuffer + paddingZeroSize, &mEchoRefBuffer, availDataSize);
                    memset((mEchoRefTempBuffer + paddingZeroSize + availDataSize), 0,
                           (processSize - paddingZeroSize - availDataSize));
                    ALOGW("%s no enough data to align (%d < %d) timeDiffMs = %llu",
                          __FUNCTION__, fillDataSize, availDataSize, timeDiffMs);
                }
                mEchoRefBufferAligned = true;
            }
        }
        else
        {
            // TODO: better way to handle the situation ?
            ALOGW("%s capture time later than echo ref time %lu:%lu -> %lu:%lu",
                  __FUNCTION__, mHfpULTaskCaptureTime.tv_sec, mHfpULTaskCaptureTime.tv_nsec,
                  mFirstEchoRefDataPlaybackTime.tv_sec, mFirstEchoRefDataPlaybackTime.tv_nsec);
            mFirstEchoRefDataArrived = false;
            mEchoRefBuffer.pRead = mEchoRefBuffer.pWrite = mEchoRefBuffer.pBufBase;
            memset(mEchoRefTempBuffer, 0, processSize);
        }
    }
    else  // just keep filling data
    {
        uint32_t dataSize = RingBuf_getDataCount(&mEchoRefBuffer);
        if (dataSize >= processSize) {
            RingBuf_copyToLinear(mEchoRefTempBuffer, &mEchoRefBuffer, processSize);
        } else {
            ALOGW("%s no enough data (%u < %u)", __FUNCTION__, dataSize, processSize);
            RingBuf_copyToLinear(mEchoRefTempBuffer, &mEchoRefBuffer, dataSize);
            memset((mEchoRefTempBuffer + dataSize), 0, (processSize - dataSize));
        }
    }

    mAudioHfpProcessing->PutDL_DelayData(mEchoRefTempBuffer, processSize);

    if (mAudioHfpDLEchoRefDumpFile) {
        AudioDumpPCMData((void *)mEchoRefTempBuffer, processSize, mAudioHfpDLEchoRefDumpFile);
    }

    return NO_ERROR;
}

/*
/ after this processing ,
/ mAudioDLTaskDL1ProcessingBuffer has channels 1 data
/ mAudioDLTaskDL2ProcessingBuffer has cahnnels 2 data ,
/ not interleave data
*/
status_t AudioHfpController::putDL1DL2DataBuffer()
{
    uint32_t frameNumber = mAudioDLTaskOutputBufferSize / mAudioHfpProcessing->GetPcmFormatBytes();
    short *processingBuffer = (short *)mAudioDLTaskOutputBuffer;
    short *processingDL1Buffer = (short *)mAudioDLTaskDL1ProcessingBuffer;
    short *processingDL2Buffer = (short *)mAudioDLTaskDL2ProcessingBuffer;

    // copy UL1 and UL2 data to temp buffer
    while (frameNumber)
    {
        *processingDL1Buffer = *processingBuffer;
        processingDL1Buffer++;
        processingBuffer++;
        frameNumber--;
        *processingDL2Buffer = *processingBuffer;
        processingDL2Buffer++;
        processingBuffer++;
        frameNumber--;
    }
    return NO_ERROR;
}

status_t AudioHfpController::DL1DataAligned()
{
    if (mAudioDLTaskSinkAttribute.mBufferSize > mAudioDLTaskOutputBufferSize) {
        ALOGW("%s mBufferSize(%u) > mAudioDLTaskOutputBufferSize(%u)", __FUNCTION__,
              mAudioDLTaskSinkAttribute.mBufferSize, mAudioDLTaskOutputBufferSize);
        unsigned int paddingZeroSize = mAudioDLTaskSinkAttribute.mBufferSize - mAudioDLTaskOutputBufferSize;
        char *mdest = mAudioDLTaskOutputBuffer + paddingZeroSize;
        memcpy((void *)mdest, mAudioDLTaskOutputBuffer, mAudioDLTaskOutputBufferSize);
        memset(mAudioDLTaskOutputBuffer, 0, paddingZeroSize);
        mAudioDLTaskOutputBufferSize = mAudioDLTaskSinkAttribute.mBufferSize;
    } else if (mAudioDLTaskSinkAttribute.mBufferSize < mAudioDLTaskOutputBufferSize) {
        ALOGW("%s mBufferSize(%u) < mAudioDLTaskOutputBufferSize(%u)", __FUNCTION__,
              mAudioDLTaskSinkAttribute.mBufferSize, mAudioDLTaskOutputBufferSize);
        mAudioDLTaskOutputBufferSize = mAudioDLTaskSinkAttribute.mBufferSize;
    }
    return NO_ERROR;
}

void AudioHfpController::applyGainOnDL1Data()
{
    short gain = mHfpVolumeGain;
    size_t sampleCount = mAudioDLTaskOutputBufferSize / mAudioDLTaskSinkAttribute.mBytesPerSample;
    short *buffer = (short *)mAudioDLTaskOutputBuffer;

    for (size_t i = 0; i < sampleCount; i++)
    {
        *buffer = clamp16(((int)*buffer * gain) >> AUDIO_HFP_VOLUME_VALID_BIT);
        buffer++;
    }
}

status_t AudioHfpController::TaskStart()
{
    ALOGD("%s", __FUNCTION__);
    mDumpFileNum++;

    ULTaskStart();
    DLTaskStart();
    ULTaskULSourceStart();
    DLEchoRefSourceStart();
    DLTaskULSourceStart();

    ULDLTaskStart();

    ULTaskDLSinkStart();
    DLTaskDLSinkStart();

    return NO_ERROR;
}

status_t AudioHfpController::TaskResourceAllocate()
{
    ALOGD("%s", __FUNCTION__);
    ULTaskULSourceAllocate();
    DLTaskULSourceAllocate();
    ULTaskDLSinkAllocate();
    DLTaskDLSinkAllocate();
    DLEchoRefSourceAllocate();
    return NO_ERROR;
}

status_t AudioHfpController::TaskStop()
{
    ALOGD("%s", __FUNCTION__);
    ULDLTaskStop();
    DLTaskStop();
    ULTaskStop();
    DLEchoRefSourceStop();
    return NO_ERROR;
}

status_t AudioHfpController::TaskResourceFree()
{
    ALOGD("%s", __FUNCTION__);
    ULTaskULSourceFree();
    DLTaskULSourceFree();
    ULTaskDLSinkFree();
    DLTaskDLSinkFree();
    DLEchoRefSourceFree();
    return NO_ERROR;
}

// here do AEC process
status_t AudioHfpController::ULDLTaskProcess()
{
    ALOGV("%s",__FUNCTION__);
    // put UL data to AEC lib
    putUL1UL2DataBuffer();
    putUL1UL2DataProcessing();

    // put dl data to AEC lib
    putDL1DL2DataBuffer();
    putDL1DL2DataProcessing();

    // put dl ref data to AEC lib
    putDLEchoRefBuffer();
    putDLEchoRefPrcoessing();

    //aec processing
    mAudioHfpProcessing->ProCessing();

    // get processed data
    getUL1UL2DataBuffer();
#ifdef GET_DL_DATA_WITH_SPEECH_ENHANCEMENT
    getDL1DL2DataBuffer();
#endif
    return NO_ERROR;
}

status_t AudioHfpController::ULDLTaskStart()
{
    // set device mode to hfp processing
    if (mULTaskInputDevice == AUDIO_DEVICE_IN_WIRED_HEADSET)
        mAudioHfpProcessing->SetDeviceMode(0, ROUTE_HEADSET);
    else if (mDLTaskOutputDevice == AUDIO_DEVICE_OUT_SPEAKER)
        mAudioHfpProcessing->SetDeviceMode(0, ROUTE_SPEAKER);
    else
        mAudioHfpProcessing->SetDeviceMode(0, ROUTE_NORMAL);

    // mic gain is set while UL task source starts
    // query mic gain and set to hfp processing
    mAudioHfpProcessing->SetMICDigitalGain(0, mAudioHfpULTaskSource->getSwMicDigitalGain());
    mAudioHfpProcessing->SetUpLinkTotalGain(0, mAudioHfpULTaskSource->getUplinkTotalGain());

    // start for ACE lib
    mAudioHfpProcessing->Start();
    return NO_ERROR;
}

status_t AudioHfpController::ULDLTaskStop()
{
    // stop for ACE lib
    mAudioHfpProcessing->Stop();
    return NO_ERROR;
}

int AudioHfpController::DLTaskBgsDataWrite(void *buffer, int writtenSize)
{
    ALOGV("+%s WrittenSize = %d", __FUNCTION__, writtenSize);

    if (getThreadEnable() == false)
    {
        ALOGD("%s getThreadEnable == false", __FUNCTION__);
        usleep(25*1000);
        return writtenSize;
    }

    char *pBuffer = (char *)buffer;
    unsigned int consumeBytes = 0, inLength = 0, outLength = 0;
    int returnSize = writtenSize;

    // 1 second for timeout
    struct timeval now;
    struct timespec timeout;
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + 1;
    timeout.tv_nsec = now.tv_usec * 1000;

    while ((writtenSize != 0) && getHfpEnable() == true) {
        pthread_mutex_lock(&mAudioBgsLock);
        if (mBliHandlerBgsSoundSrc != NULL) {
            inLength = writtenSize;
            consumeBytes = inLength;
            outLength = RingBuf_getFreeSpace(&mBgsSoundBuffer);
            mBliHandlerBgsSoundSrc->process((int16_t *)pBuffer, &inLength,
                (int16_t *)mAudioBgsSoundSrcBuffer, &outLength);
            consumeBytes -= inLength;
            pBuffer+= consumeBytes;
            writtenSize -= consumeBytes;
            ALOGV("-%s BLI_Process inLength = %d outLength = %d consumeBytes = %d",
                  __FUNCTION__, inLength, outLength, consumeBytes);
            RingBuf_copyFromLinear(&mBgsSoundBuffer, mAudioBgsSoundSrcBuffer, outLength);

            if (mAudioBgsSoundSrcBufferDumpFile)
                AudioDumpPCMData((void *)mAudioBgsSoundSrcBuffer, outLength,
                     mAudioBgsSoundSrcBufferDumpFile);
        } else {
            int availableSize = RingBuf_getFreeSpace(&mBgsSoundBuffer) ;
            if(availableSize > writtenSize) {
                availableSize = writtenSize;
            }
            RingBuf_copyFromLinear(&mBgsSoundBuffer, (char*)buffer, availableSize);
            pBuffer += availableSize;
            writtenSize -= availableSize;
            ALOGV("-%s no LI_Convert availableSize = %d", __FUNCTION__, availableSize);
            if (mAudioBgsSoundSrcBufferDumpFile) {
                AudioDumpPCMData((void *)buffer, availableSize, mAudioBgsSoundSrcBufferDumpFile);
            }
        }

        if (writtenSize != 0) {
            ALOGV("%s pthread_cond_timedwait WrittenSize = %d >>", __FUNCTION__, writtenSize);

            int ret = pthread_cond_timedwait(&mAudioBgsWaitWorkCV ,&mAudioBgsLock, &timeout);
            if (ret == ETIMEDOUT)
                ALOGW("%s pthread_cond_timedwait timeout WrittenSize = %d <<", __FUNCTION__, writtenSize);
            else
                ALOGV("%s pthread_cond_timedwait WrittenSize = %d <<", __FUNCTION__, writtenSize);

            pthread_mutex_unlock(&mAudioBgsLock);
        } else {
            pthread_mutex_unlock(&mAudioBgsLock);
        }
    }

    return returnSize;
}

status_t AudioHfpController::DLTaskBgsDataStart(uint32_t sampleRate, uint32_t chNum, int32_t format)
{
	AL_AUTOLOCK(mBgsSoundCountLock);
	ALOGD("%s mBgsSoundCount %d",__FUNCTION__, mBgsSoundCount);
    setBgsSoundAttribute(sampleRate, chNum, format);
	mBgsSoundCount++;
	if (mBgsSoundCount == 1) {
		mBgsSoundBuffer.bufLen = (mAudioBgsSoundAttribute.mBufferSize *
					  mAudioDLTaskSinkAttribute.mSampleRate) /
                              mAudioBgsSoundAttribute.mSampleRate * AUDIO_HFP_BGS_BUFFER_COUNT;
    mAudioBgsSoundSrcBuffer = new char[mAudioBgsSoundAttribute.mBufferSize*2];
    mBgsSoundBuffer.pBufBase = new char[mBgsSoundBuffer.bufLen];
    mBgsSoundBuffer.pRead = mBgsSoundBuffer.pBufBase;
    mBgsSoundBuffer.pWrite = mBgsSoundBuffer.pBufBase;

    ALOGD("%s mBgsSoundBuffer.bufLen = %d", __FUNCTION__, mBgsSoundBuffer.bufLen);

    if (mAudioDLTaskSinkAttribute.mSampleRate !=
        mAudioBgsSoundAttribute.mSampleRate) {
        ALOGD("%s BLI_Open insr = %d out sr = %d", __FUNCTION__,
              mAudioBgsSoundAttribute.mSampleRate,
              mAudioDLTaskSinkAttribute.mSampleRate);

        SRC_PCM_FORMAT src_pcm_format = SRC_IN_Q1P15_OUT_Q1P15;
        if (mAudioBgsSoundAttribute.mFormat == AUDIO_FORMAT_PCM_32_BIT)
            src_pcm_format = SRC_IN_Q1P31_OUT_Q1P31;
        else if (mAudioBgsSoundAttribute.mFormat == AUDIO_FORMAT_PCM_16_BIT)
            src_pcm_format = SRC_IN_Q1P15_OUT_Q1P15;
        else {
            ALOGE("%s(), not support mStreamAttributeSource->audio_format(0x%x) SRC!!",
                  __FUNCTION__,
                  mAudioBgsSoundAttribute.mFormat);
        }

        mBliHandlerBgsSoundSrc = newMtkAudioSrc(
                         mAudioBgsSoundAttribute.mSampleRate, mAudioBgsSoundAttribute.mChannels,
                         mAudioDLTaskSinkAttribute.mSampleRate, mAudioDLTaskSinkAttribute.mChannels,
                         src_pcm_format);

        ASSERT(mBliHandlerBgsSoundSrc != NULL);
        mBliHandlerBgsSoundSrc->open();
    }

    if (mAudioBgsSoundSrcBufferDumpFile == NULL) {
        String8 dumpFileName;
        dumpFileName.appendFormat("%s.%d.%d_%dch.pcm", AudioBgsSoundSrcBuffer, mDumpFileNum,
                    mAudioDLTaskSinkAttribute.mSampleRate, mAudioDLTaskSinkAttribute.mChannels);
        mAudioBgsSoundSrcBufferDumpFile = AudioOpendumpPCMFile(dumpFileName.string(), streamhfp_propty);
    }
	}

    return NO_ERROR;
}

status_t AudioHfpController::DLTaskBgsDataStop()
{
	AL_AUTOLOCK(mBgsSoundCountLock);
	ALOGD("%s mBgsSoundCount %d",__FUNCTION__, mBgsSoundCount);

	mBgsSoundCount--;

	if(mBgsSoundCount == 0) {
    if (mBgsSoundBuffer.pBufBase != NULL) {
        delete [] mBgsSoundBuffer.pBufBase;
        mBgsSoundBuffer.pBufBase = NULL;
        mBgsSoundBuffer.pRead = NULL;
        mBgsSoundBuffer.pWrite = NULL;
    }
    if (mAudioBgsSoundSrcBuffer != NULL) {
        delete [] mAudioBgsSoundSrcBuffer;
        mAudioBgsSoundSrcBuffer = NULL;
    }
    if (mBliHandlerBgsSoundSrc != NULL) {
        deleteMtkAudioSrc(mBliHandlerBgsSoundSrc);
        mBliHandlerBgsSoundSrc = NULL;
    }
    if (mAudioBgsSoundSrcBufferDumpFile != NULL) {
        AudioCloseDumpPCMFile(mAudioBgsSoundSrcBufferDumpFile);
        mAudioBgsSoundSrcBufferDumpFile = NULL;
    }
	}
    return NO_ERROR;
}


AudioHfpController::AudioHfpThread::AudioHfpThread(AudioHfpController *HfpController)
{
    ALOGD("%s", __FUNCTION__);
    if (HfpController != NULL) {
        mtAudioHfpController = HfpController;
        mtAudioHfpController->mThreadAlive = true;
    }
}

AudioHfpController::AudioHfpThread::~AudioHfpThread()
{
    ALOGD("%s", __FUNCTION__);

    mtAudioHfpController->TaskStop();
    mtAudioHfpController->TaskResourceFree();
    mtAudioHfpController->mThreadAlive = false;
    mtAudioHfpController = NULL;
}

void AudioHfpController::AudioHfpThread::onFirstRef()
{
    ALOGV("%s", __FUNCTION__);
}


// Good place to do one-time initializations
status_t AudioHfpController::AudioHfpThread::readyToRun()
{
    ALOGD("%s", __FUNCTION__);

#ifdef MTK_AUDIO_ADJUST_PRIORITY
#define RTPM_PRIO_AUDIO_RECORD (77)
    // force to set priority
    struct sched_param sched_p;
    sched_getparam(0, &sched_p);
    sched_p.sched_priority = RTPM_PRIO_AUDIO_RECORD + 1; //AUDIO_HFP_THREAD_PRIORITY
    if (0 != sched_setscheduler(0, SCHED_RR, &sched_p)) {
        ALOGE("[%s] failed, errno: %d", __FUNCTION__, errno);
    } else {
        sched_p.sched_priority = RTPM_PRIO_AUDIO_CCCI_THREAD;
        sched_getparam(0, &sched_p);
        ALOGD("sched_setscheduler ok, priority: %d", sched_p.sched_priority);
    }
#endif

    prctl(PR_SET_NAME, (unsigned long)"AudioHfpThread", 0, 0, 0);

    mtAudioHfpController->setDLTaskSinkAttribute();
    mtAudioHfpController->setDLTaskSourceAttribute();
    mtAudioHfpController->setULTaskSinkAttribute();
    mtAudioHfpController->setULTaskSourceAttribute();
    mtAudioHfpController->setDLEchoRefAttribute();
    mtAudioHfpController->setBgsSoundAttribute();

    mtAudioHfpController->TaskResourceAllocate();
    mtAudioHfpController->updateParameters();
    mtAudioHfpController->TaskStart();

    mtAudioHfpController->setThreadEnable(true);

    return NO_ERROR;
}

bool AudioHfpController::AudioHfpThread::threadLoop()
{
    unsigned long long timeDiffNs, timeDiffUs;
    struct timespec tstemp1, tstemp2;

    while (!(exitPending() == true))
    {
        tstemp1 = GetSystemTime(false);

        // get data from input
        mtAudioHfpController->ULTaskGetULDataSource();   //mAudioHfpULTaskSource->read() UL source: MultiMedia1_Capture
        mtAudioHfpController->getDLEchoRefSource();      //mAudioHfpDLEchoRefSource->read() echo ref: DL1_AWB_Record -> VOIP_Call_BT_Capture
        mtAudioHfpController->DLTaskGetULDataSource();   //mAudioHfpDLTaskSource->read() DL source: BTCVSD_Capture -> DL1_AWB_Record

        // DL process
        mtAudioHfpController->DLTaskProcess();

        // AEC process
        mtAudioHfpController->ULDLTaskProcess();

        // UL process
        mtAudioHfpController->ULTaskProcess();

        // write data to output
        mtAudioHfpController->DLTaskPutDLDataSink();     //mAudioHfpDLTaskSink->write() DL sink: MultiMedia1_PLayback
        mtAudioHfpController->ULTaskPutDLDataSink();     //mAudioHfpULTaskSink->write() UL sink: BTCVSD_Playback -> MultiMedia2_PLayback

        tstemp2 = GetSystemTime(false);
        timeDiffNs = TimeDifference(tstemp1, tstemp2);
        timeDiffUs = timeDiffNs / 1000;
        ALOGV("%s TimeDifference = %llu in ns %llu in us", __FUNCTION__, timeDiffNs, timeDiffUs);

        if (!exitPending() && timeDiffUs > AUDIO_HFP_RESET_TIME_US) {
            ALOGD("%s reset due to timeout (%llu > %d) >>", __FUNCTION__, timeDiffUs, AUDIO_HFP_RESET_TIME_US);
            mtAudioHfpController->TaskStop();
            mtAudioHfpController->TaskResourceFree();
            mtAudioHfpController->TaskResourceAllocate();
            mtAudioHfpController->TaskStart();
            ALOGD("%s reset due to timeout (%llu > %d) <<", __FUNCTION__, timeDiffUs, AUDIO_HFP_RESET_TIME_US);
        }

        return true;
    }

    ALOGD("%s exit", __FUNCTION__);
    return false;
}

}
