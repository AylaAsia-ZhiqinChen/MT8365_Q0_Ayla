#include "AudioALSACaptureDataClientVoiceRecProc.h"

#include <inttypes.h>
#include <pthread.h>
#include <sys/prctl.h>

#include <utils/threads.h> /*  for ANDROID_PRIORITY_AUDIO */


#include "AudioUtility.h"
#include "SpeechUtility.h"


#include "AudioType.h"
#include <AudioLock.h>

#include "AudioALSACaptureDataProviderBase.h"
#include "AudioALSAHardwareResourceManager.h"

#include <SpeechEnhancementController.h>

#include "AudioVolumeInterface.h"
#include "AudioVolumeFactory.h"


#include <audio_memory_control.h>
#include <audio_lock.h>
#include <audio_ringbuf.h>
#include <audio_time.h>


//data process +++
#include "IAudioALSAProcessBase.h"
//data process ---

//Android Native Preprocess effect +++
#include "AudioPreProcess.h"
//Android Native Preprocess effect ---

//#undef ALOGV
//#define ALOGV ALOGD


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "AudioALSACaptureDataClientVoiceRecProc"


#ifdef AURISYS_ENABLE_LATENCY_DEBUG
#ifdef LATENCY_LOG
#undef LATENCY_LOG
#endif
#define LATENCY_LOG(x...) ALOGW(x)
#else
#define LATENCY_LOG(x...)
#endif

#define UL_LOCK_MS(al, ms) \
    ({ \
        int __RET__ = 0; \
        if (getNeedAEETimeoutFlg()) { \
            __RET__ = AL_LOCK_MS(al, ms); \
        } else { \
            __RET__ = AL_LOCK_MS_NO_ASSERT(al, ms); \
        } \
        __RET__; \
    })


#ifdef AURISYS_DUMP_LOG_V
#undef  AUD_LOG_V
#define AUD_LOG_V(x...) AUD_LOG_D(x)
#endif



namespace android {

/*==============================================================================
 *                     Constant
 *============================================================================*/

static AudioLock mAurisysLibManagerLock;


#define MTK_STREAMIN_VOLUEM_MAX (0x1000)
#define MTK_STREAMIN_VOLUME_VALID_BIT (12)

#ifndef UPLINK_DROP_POP_MS
#define DROP_POP_MS (60)
#else
#define DROP_POP_MS (UPLINK_DROP_POP_MS)
#endif

#ifndef UPLINK_DROP_POP_MS_FOR_UNPROCESSED
#define DROP_POP_MS_FOR_UNPROCESSED (80)
#else
#define DROP_POP_MS_FOR_UNPROCESSED (UPLINK_DROP_POP_MS_FOR_UNPROCESSED)
#endif

#define MAX_LOCK_TIME_OUT_MS (500)
#define MAX_PROCESS_DATA_WAIT_TIME_OUT_MS (40)

#ifndef MAX_RAW_DATA_LOCK_TIME_OUT_MS
#ifdef CONFIG_MT_ENG_BUILD
#define MAX_RAW_DATA_LOCK_TIME_OUT_MS (500 * 3)
#else
#define MAX_RAW_DATA_LOCK_TIME_OUT_MS (500)
#endif
#endif

#ifndef MAX_PROCESS_DATA_LOCK_TIME_OUT_MS
#ifdef CONFIG_MT_ENG_BUILD
#define MAX_PROCESS_DATA_LOCK_TIME_OUT_MS (500 * 3)
#else
#define MAX_PROCESS_DATA_LOCK_TIME_OUT_MS (500)
#endif
#endif


/*==============================================================================
 *                     Utility
 *============================================================================*/

inline bool isTimeStampStarted(const struct timespec *time_stamp) {
    return (time_stamp->tv_sec != 0 || time_stamp->tv_nsec != 0);
}


static int calculateTotalTimeSpec(const time_info_struct_t *timeInfo,
                                  struct timespec *time_stamp) {
    long ret_ns = 0;

    if (timeInfo == NULL || time_stamp == NULL) {
        ASSERT(timeInfo != NULL);
        ASSERT(time_stamp != NULL);
        return -1;
    }

    if (isTimeStampStarted(&timeInfo->timestamp_get) == false) {
        ALOGE("%s() timestamp_get not start!!", __FUNCTION__);
        return -1;
    }

    time_stamp->tv_sec = timeInfo->timestamp_get.tv_sec;
    time_stamp->tv_nsec = timeInfo->timestamp_get.tv_nsec;

    ret_ns = timeInfo->kernelbuffer_ns;
    if (time_stamp->tv_nsec >= ret_ns) {
        time_stamp->tv_nsec -= ret_ns;
    } else {
        time_stamp->tv_sec -= 1;
        time_stamp->tv_nsec = 1000000000 + time_stamp->tv_nsec - ret_ns;
    }
    ALOGV("%s(), sec: %ld, nsec: %ld, ret_ns: %ld", __FUNCTION__,
          time_stamp->tv_sec, time_stamp->tv_nsec, ret_ns);

    return 0;
}


static uint32_t getDiffBufSize(const stream_attribute_t *attribute, uint64_t time_diff_ns) {
    uint64_t diff_buf_size = 0;

    uint64_t size_per_sample = 0;
    uint64_t size_per_frame = 0;


    size_per_sample = audio_bytes_per_sample(attribute->audio_format);
    size_per_frame  = attribute->num_channels * size_per_sample;

    diff_buf_size = (size_per_frame *
                     attribute->sample_rate *
                     time_diff_ns) / 1000000000;
    if ((diff_buf_size % size_per_frame) != 0) { // alignment
        diff_buf_size = ((diff_buf_size / size_per_frame) + 1) * size_per_frame;
    }

    return (uint32_t)diff_buf_size;
}

static uint32_t getDropMs(stream_attribute_t *attribute_target) {
    if (attribute_target->input_source == AUDIO_SOURCE_UNPROCESSED) {
        return DROP_POP_MS_FOR_UNPROCESSED;
    } else {
        return DROP_POP_MS;
    }
}


/*==============================================================================
 *                     Implementation
 *============================================================================*/
AudioALSACaptureDataClientVoiceRecProc::AudioALSACaptureDataClientVoiceRecProc(
    AudioALSACaptureDataProviderBase *pCaptureDataProvider,
    stream_attribute_t *stream_attribute_target) :
    mStreamAttributeSource(NULL),
    mStreamAttributeTarget(stream_attribute_target),
    mCaptureDataProvider(pCaptureDataProvider),
    mMicMute(false),
    mMuteTransition(false),
    mAudioALSAVolumeController(AudioVolumeFactory::CreateAudioVolumeController()),
    mEnable(false),
    mStreamInReopen(false),
    mProcessThreadLaunched(false),
    mLatency(0),
    mRawDataPeriodBufSize(0),
    mProcessedDataPeriodBufSize(0),
    mDropPopSize(0),
    mBliSrcPre(NULL),
    mProcess(NULL),
    mBliSrcPost(NULL),
    mNeedFmtCvt(false),
    mNeedChannelCvt(false),
    mAudioPreProcessEffect(NULL) {

    // init var
    uint32_t drop_ms = 0;
    uint32_t size_per_sample = 0;
    uint32_t size_per_frame = 0;
    uint32_t proc_rates;
    int ret = 0;


    ALOGD("%s(+)", __FUNCTION__);


    //The process flow is as following, we need init the following path
    // Process flow: RAW=> Pre SRC => Process => Post SRC => ChRemix
    //                 => FormatCvt => Dedrop => Native Effect => Processed
    // Now think data process output channel number and format is same with target

    // init Buffer
    memset((void *)&mRawDataBuf, 0, sizeof(mRawDataBuf));
    memset((void *)&mRawLinearBuf, 0, sizeof(mRawLinearBuf));

    memset((void *)&mSrcPreOutBuf, 0, sizeof(mSrcPreOutBuf));
    memset((void *)&mSrcPreOutLinearBuf, 0, sizeof(mSrcPreOutLinearBuf));

    memset((void *)&mProcInLinearBuf, 0, sizeof(mProcInLinearBuf));
    memset((void *)&mProcOutLinearBuf, 0, sizeof(mProcOutLinearBuf));

    memset((void *)&mSrcPostOutLinearBuf, 0, sizeof(mSrcPostOutLinearBuf));
    memset((void *)&mProcessedDataBuf, 0, sizeof(mProcessedDataBuf));

    // config attribute for input device
    mCaptureDataProvider->configStreamAttribute(mStreamAttributeTarget);

    // get attribute before enable
    mStreamAttributeSource = mCaptureDataProvider->getStreamAttributeSource();

    // enable before attach
    mEnable = true;

    // attach client to capture data provider (after data buf ready)
    mCaptureDataProvider->attach(this);
    mCaptureDataProvider->signalPcmStart();

    if (((mStreamAttributeSource->input_device == AUDIO_DEVICE_IN_USB_DEVICE) ||
        (mCaptureDataProvider->getCaptureDataProviderType() == CAPTURE_PROVIDER_SOUNDTRIGGER))&&
        (getPcmStatus() != true)) {
        hProcessThread = 0;
        ALOGD("%s, PCM Open/Read Fail...Device is unplugged ?", __FUNCTION__);
        return;
    }

    // default process periods is 16ms, TODO may change
    mLatency = 16;
    proc_rates = mStreamAttributeSource->sample_rate;
    if (mProcess) { /* If has process, get process request period and sample rate */
        mProcess->getParameters(IAudioALSAProcessBase::CMD_GET_IN_PERIOD_MS, &mLatency);
        mProcess->getParameters(IAudioALSAProcessBase::CMD_GET_IN_RATES,     &proc_rates);
    }

    // Init: Raw data buffer init
    initRawBuffer();

    // Init: Pre-SRC init
    initBliSrcPre(mStreamAttributeSource->sample_rate, proc_rates);

    // Init: Data process init
    // Notice: this stage may change the mLatency, and decide if need Pre SRC
    initDataProcess();

    // Init: Post-SRC init
    initBliSrcPost(proc_rates, mStreamAttributeTarget->sample_rate);

    // Init: Channel Convert (now only support input 1ch/2ch/4ch => 1ch/2ch)
    if (mSrcPostOutLinearBuf.num_channels != mStreamAttributeTarget->num_channels)
        mNeedChannelCvt = true;

    // Init: format convert init
    if (mSrcPostOutLinearBuf.audio_format != mStreamAttributeTarget->audio_format)
        mNeedFmtCvt = true;

    // Init: Processed buffer init
    initProcessedBuffer();

    ALOGD("mLatency %u, mRawDataPeriodBufSize %u, mProcessedDataPeriodBufSize %u",
          mLatency, mRawDataPeriodBufSize, mProcessedDataPeriodBufSize);
 
    //Android Native Preprocess effect +++
    mAudioPreProcessEffect = new AudioPreProcess(mStreamAttributeTarget); // use the same attribute as stream in
    ASSERT(mAudioPreProcessEffect != NULL);
    CheckNativeEffect();
    //Android Native Preprocess effect ---


    // depop
    if (mCaptureDataProvider->getCaptureDataProviderType() != CAPTURE_PROVIDER_SOUNDTRIGGER) {
        drop_ms = getDropMs(mStreamAttributeTarget);
        if ((drop_ms % mLatency) != 0) { // drop data size need to align interrupt rate
            drop_ms = ((drop_ms / mLatency) + 1) * mLatency; // cell()
        }
        mDropPopSize = (audio_bytes_per_sample(mStreamAttributeTarget->audio_format) *
                        mStreamAttributeTarget->num_channels *
                        mStreamAttributeTarget->sample_rate *
                        drop_ms) / 1000;

        size_per_sample = audio_bytes_per_sample(mStreamAttributeTarget->audio_format);
        size_per_frame  = mStreamAttributeTarget->num_channels * size_per_sample;
        if ((mDropPopSize % size_per_frame) != 0) { // alignment
            mDropPopSize = ((mDropPopSize / size_per_frame) + 1) * size_per_frame;
        }
        ASSERT(mDropPopSize >= mProcessedDataPeriodBufSize);
    } else {
        drop_ms = 0;
        mDropPopSize = 0;
    }

    // processThread
    hProcessThread = 0;
    ret = pthread_create(&hProcessThread, NULL,
                         AudioALSACaptureDataClientVoiceRecProc::processThread,
                         (void *)this);
    ASSERT(ret == 0);
    ALOGD("%s(-), drop_ms = %d", __FUNCTION__, drop_ms);
}


AudioALSACaptureDataClientVoiceRecProc::~AudioALSACaptureDataClientVoiceRecProc() {
    // start to close
    mEnable = false;

    // terminate processThread thread
    pthread_join(hProcessThread, NULL);
    ALOGD("pthread_join hProcessThread done");

    // detach client to capture data provider
    mCaptureDataProvider->detach(this);
    ALOGV("mCaptureDataProvider detach done");

    // close
    AL_LOCK_MS(mRawDataBufLock, MAX_LOCK_TIME_OUT_MS);
    AL_LOCK_MS(mProcessedDataBufLock, MAX_LOCK_TIME_OUT_MS);


    //Android Native Preprocess effect +++
    if (mAudioPreProcessEffect != NULL) { delete mAudioPreProcessEffect; }
    //Android Native Preprocess effect ---

    deInitRawBuffer();
    deinitBliSrcPre();
    deInitDataProcess();
    deInitBliSrcPost();
    deInitProcessedBuffer();


    AUDIO_FREE_POINTER(mProcessedDataBuf.base);


    AL_UNLOCK(mProcessedDataBufLock);
    AL_UNLOCK(mRawDataBufLock);
}


uint32_t AudioALSACaptureDataClientVoiceRecProc::copyCaptureDataToClient(RingBuf pcm_read_buf) {
    audio_ringbuf_t pcm_read_buf_wrap;
    uint32_t data_size, free_size;
    int ret = 0;

    if (mProcessThreadLaunched == false) {
        ALOGD("%s(), mProcessThreadLaunched == false. return", __FUNCTION__);
        return 0;
    }

    if (((mStreamAttributeSource->input_device == AUDIO_DEVICE_IN_USB_DEVICE) ||
        (mCaptureDataProvider->getCaptureDataProviderType() == CAPTURE_PROVIDER_SOUNDTRIGGER))&&
        (getPcmStatus() != true)) {
        ALOGD("%s, PCM Open/Read Fail... Device is unplugged ?", __FUNCTION__);
        AL_SIGNAL(mRawDataBufLock);
        return 0;
    }
    pcm_read_buf_wrap.base = pcm_read_buf.pBufBase;
    pcm_read_buf_wrap.read = pcm_read_buf.pRead;
    pcm_read_buf_wrap.write = pcm_read_buf.pWrite;
    pcm_read_buf_wrap.size = pcm_read_buf.bufLen;

    /* TODO, need check the target buffer status to avoid overwrite */
    ret = UL_LOCK_MS(mRawDataBufLock, MAX_RAW_DATA_LOCK_TIME_OUT_MS);
    if (ret != 0) {
        handleLockTimeout();
    }

    if (mEnable == false) {
        ALOGV("%s(), mEnable == false. return", __FUNCTION__);
        AL_SIGNAL(mRawDataBufLock);
        AL_UNLOCK(mRawDataBufLock);
        return 0;
    }

    data_size = audio_ringbuf_count(&pcm_read_buf_wrap);
    free_size = audio_ringbuf_free_space(&mRawDataBuf);
    if (data_size > free_size) {
        ALOGW("%s() target buffer free size(%u) < data size(%u)\n", __FUNCTION__,  free_size, data_size);
        AL_WAIT_MS(mRawDataBufLock, MAX_RAW_DATA_LOCK_TIME_OUT_MS);
        data_size = audio_ringbuf_count(&pcm_read_buf_wrap);
        free_size = audio_ringbuf_free_space(&mRawDataBuf);
        if (data_size > free_size)
            ALOGW("%s() warning some data will be drop(%u, %u)", __FUNCTION__,  free_size, data_size);
    }

    audio_ringbuf_copy_from_ringbuf_all(&mRawDataBuf, &pcm_read_buf_wrap);
    AL_SIGNAL(mRawDataBufLock);

    AL_UNLOCK(mRawDataBufLock);
    return 0;
}
bool AudioALSACaptureDataClientVoiceRecProc::getPcmStatus(void) {
    return (mCaptureDataProvider->getPcmStatus() == NO_ERROR);
}

void *AudioALSACaptureDataClientVoiceRecProc::processThread(void *arg) {
    char thread_name[128] = {0};

    AudioALSACaptureDataClientVoiceRecProc *client = NULL;
    AudioALSACaptureDataProviderBase *provider = NULL;
    IAudioALSAProcessBase *data_process = NULL;
    MtkAudioSrcBase      *blisrc_pre = NULL;
    audio_devices_t input_device;

    audio_ringbuf_t    *raw_ring = NULL;
    audio_linear_buf_t *raw = NULL;

    audio_ringbuf_t    *pre_src_ring = NULL;
    audio_linear_buf_t *pre_src = NULL;

    audio_linear_buf_t *proc_in = NULL;
    audio_linear_buf_t *proc_out = NULL;

    audio_linear_buf_t *post_src = NULL;

    audio_ringbuf_t    *processed_ring = NULL;
    audio_linear_buf_t *processed = NULL;

    char *effect_buf = NULL;
    uint32_t frame_idx = 0;

    uint32_t in_data_count = 0;
    uint32_t data_count = 0;

    int wait_result = 0;
    int ret = 0;

    client = static_cast<AudioALSACaptureDataClientVoiceRecProc *>(arg);

    CONFIG_THREAD(thread_name, ANDROID_PRIORITY_AUDIO);
#ifdef UPLINK_LOW_LATENCY
    if (client->IsLowLatencyCapture()) {
        audio_sched_setschedule(0, SCHED_RR, sched_get_priority_min(SCHED_RR));
    }
#endif

    /* get handle */
    provider = client->mCaptureDataProvider;
    input_device = client->mStreamAttributeSource->input_device;
    data_process = client->mProcess;
    blisrc_pre = client->mBliSrcPre;

    /* process thread created */
    client->mProcessThreadLaunched = true;

    /* get process buffer */
    raw_ring = &client->mRawDataBuf;;
    raw = &client->mRawLinearBuf;

    pre_src_ring = &client->mSrcPreOutBuf;
    pre_src = &client->mSrcPreOutLinearBuf;

    proc_in = &client->mProcInLinearBuf;
    proc_out = &client->mProcOutLinearBuf;

    post_src = &client->mSrcPostOutLinearBuf;

    processed_ring = &client->mProcessedDataBuf;
    processed = &client->mProcessedLinearBuf;


    while (client->mEnable == true) {
        ALOGV("%s(+)", __FUNCTION__);

        if (((input_device == AUDIO_DEVICE_IN_USB_DEVICE) ||
            (provider->getCaptureDataProviderType() == CAPTURE_PROVIDER_SOUNDTRIGGER))&&
            (client->getPcmStatus() != true)) {
            ALOGD("%s, PCM Open/Read Fail... Device is unplugged ?", __FUNCTION__);
            AL_SIGNAL(client->mProcessedDataBufLock);
            usleep(100);
            continue;
        }

        // get data from raw buffer
        ret = UL_LOCK_MS(client->mRawDataBufLock, MAX_RAW_DATA_LOCK_TIME_OUT_MS);
        if (ret != 0) {
            client->handleLockTimeout();
        }

        in_data_count = audio_ringbuf_count(raw_ring);
        data_count = raw->buf_size;
        // data not reary, wait data
        if (in_data_count < data_count) {
            wait_result = AL_WAIT_MS(client->mRawDataBufLock, MAX_PROCESS_DATA_WAIT_TIME_OUT_MS);
            if (wait_result != 0) {
                in_data_count = audio_ringbuf_count(raw_ring);
                if (in_data_count < data_count) {
                    ALOGW("in_data_count %u, data_count %u", in_data_count, data_count);
                }

                AL_UNLOCK(client->mRawDataBufLock);
                usleep(100);
            }
            AL_UNLOCK(client->mRawDataBufLock);
            continue;
        }

        // Process flow: RAW=> Pre SRC => Process => Post SRC => ChRemix
        //               => FormatCvt => Dedrop => Native Effect => Processed
        // 1. Copy data to process input buffer
        //    Buffer: raw ring => raw linear
        audio_ringbuf_copy_to_linear(raw->buf, raw_ring, data_count);
        frame_idx++;
        AL_SIGNAL(client->mRawDataBufLock);
        AL_UNLOCK(client->mRawDataBufLock);

        // 2. Pre sample rate convert before process
        //    Bufffer: raw linear=> pre src linear => proc in linear
        in_data_count = data_count;
        data_count    = pre_src->buf_size;
        client->doBliSrcPre(raw->buf, in_data_count, pre_src->buf, &data_count);
        if (blisrc_pre) {
            // used to period reorder before process
            // because src may change the buffer data size not same with period
            audio_ringbuf_copy_from_linear(pre_src_ring, pre_src->buf, data_count);
            in_data_count = audio_ringbuf_count(pre_src_ring);
            data_count = proc_in->buf_size;
            if (in_data_count < data_count) {
                continue;
            }
            audio_ringbuf_copy_to_linear(proc_in->buf, pre_src_ring, data_count);
        }
        // 3. data process, it not change the sample rate, and keep const frame count
        //    Buffer: proc in linear  => proc out linear
        if (data_process) {
            in_data_count = proc_in->buf_size;
            data_count = data_process->process(proc_in->buf, proc_out->buf, in_data_count);
            if (data_count == 0) {
                ALOGD("%s(), frame #%d data_count == 0, %dL", __FUNCTION__, frame_idx, __LINE__);
                continue;
            }
        }

        // 4. post src
        //    Buffer: proc out linear => post src lienar
        in_data_count = proc_out->buf_size;
        data_count    = post_src->buf_size;
        client->doBliSrcPost(proc_out->buf, in_data_count, post_src->buf, &data_count);

        // 5. channel Convert, post src may cause data size not equal with period
        //    Buffer: post src linear => processed linear
        in_data_count = data_count;
        client->ApplyChannelRemix(post_src->buf, in_data_count, processed->buf, &data_count);

        // 6. Format Convert TODO not implement now
        //    Buffer: procssed linear => processed linear
        in_data_count = data_count;
        client->TransferFormat(processed->buf, in_data_count, processed->buf, &data_count);

        // 7. depop, check the procssed linear buffer
        if (client->mDropPopSize > 0) {
            ALOGV("data_count %u, mDropPopSize %u, %dL", data_count, client->mDropPopSize, __LINE__);
            if (data_count >= client->mDropPopSize) {
                effect_buf = processed->buf + client->mDropPopSize;
                data_count -= client->mDropPopSize;
                client->mDropPopSize = 0;
            } else {
                effect_buf = processed->buf + data_count;
                client->mDropPopSize -= data_count;
                data_count = 0;
            }
        }
        if (data_count == 0) {
            ALOGV("%s(), frame #%d data_count == 0, %dL", __FUNCTION__, frame_idx, __LINE__);
            continue;
        }

        // 8. Native Preprocess effect+++
        //    Buffer procssed linear => effect
        effect_buf = processed->buf;

        data_count = client->NativePreprocess(effect_buf, data_count);
        if (data_count == 0) {
            ALOGD("%s(), frame #%d data_count == 0, %dL", __FUNCTION__, frame_idx, __LINE__);
            continue;
        }
        //Native Preprocess effect---


        // 9. copy to processed buf and signal read()
        //    Buffer: effect => processed ring
        ret = UL_LOCK_MS(client->mProcessedDataBufLock, MAX_PROCESS_DATA_LOCK_TIME_OUT_MS);
        if (ret != 0) {
            client->handleLockTimeout();
        }

        in_data_count = audio_ringbuf_free_space(processed_ring);
        if (in_data_count < data_count) {
            ret = AL_WAIT_MS(client->mProcessedDataBufLock, MAX_PROCESS_DATA_LOCK_TIME_OUT_MS);
        }
        audio_ringbuf_copy_from_linear(processed_ring, effect_buf, data_count);

        AL_SIGNAL(client->mProcessedDataBufLock);
        AL_UNLOCK(client->mProcessedDataBufLock);
    }


    ALOGV("%s terminated", thread_name);
    pthread_exit(NULL);
    return NULL;
}

ssize_t AudioALSACaptureDataClientVoiceRecProc::read(void *buffer, ssize_t bytes) {
    ALOGV("%s(+), bytes: %u", __FUNCTION__, (uint32_t)bytes);

    char *write = (char *)buffer;
    uint32_t data_count = 0;
    uint32_t left_count_to_read = bytes;

    int try_count = 8;
    int wait_result = 0;
    int ret = 0;

    uint32_t wait_ms = 0;

    // clean buffer
    memset(buffer, 0, bytes);

    // wait time
    wait_ms = (IsLowLatencyCapture()) ? mLatency * 5 : MAX_PROCESS_DATA_WAIT_TIME_OUT_MS;

    // push clean up handlder for read thread termination
    CLEANUP_PUSH_ALOCK(mProcessedDataBufLock.getAlock());

    // copy processed data
    do {
        ret = UL_LOCK_MS(mProcessedDataBufLock, MAX_PROCESS_DATA_LOCK_TIME_OUT_MS);
        if (ret != 0) {
            handleLockTimeout();
        }
        data_count = audio_ringbuf_count(&mProcessedDataBuf);
        if (data_count == 0) {
            // wait for new data
            wait_result = AL_WAIT_MS(mProcessedDataBufLock, wait_ms);
            if (((mStreamAttributeSource->input_device == AUDIO_DEVICE_IN_USB_DEVICE) ||
                (mCaptureDataProvider->getCaptureDataProviderType() == CAPTURE_PROVIDER_SOUNDTRIGGER))&&
                (getPcmStatus() != true)) {
                ALOGD("%s, PCM Open/Read Fail... Device is unplugged ?", __FUNCTION__);
                AL_UNLOCK(mProcessedDataBufLock);
                left_count_to_read = bytes;
                break;
            }
            if (wait_result != 0) { // something error, exit
                AL_UNLOCK(mProcessedDataBufLock);
                try_count--;
                usleep(100);
                continue;
            }

            if (mEnable == false) {
                ALOGD("%s(), record stopped. return", __FUNCTION__);
                AL_UNLOCK(mProcessedDataBufLock);
                break;
            }

            data_count = audio_ringbuf_count(&mProcessedDataBuf);
        }

        if (data_count >= left_count_to_read) { // ring buffer is enough, copy & exit
            audio_ringbuf_copy_to_linear(write, &mProcessedDataBuf, left_count_to_read);
            AL_SIGNAL(mProcessedDataBufLock);
            AL_UNLOCK(mProcessedDataBufLock);
            left_count_to_read = 0;
            break;
        }

        audio_ringbuf_copy_to_linear((char *)write, &mProcessedDataBuf, data_count);
        AL_SIGNAL(mProcessedDataBufLock);
        AL_UNLOCK(mProcessedDataBufLock);
        left_count_to_read -= data_count;
        write += data_count;

        try_count--;
    } while (left_count_to_read > 0 && try_count > 0 && mEnable == true);

    // pop clean up handlder for read thread termination
    CLEANUP_POP_ALOCK(mProcessedDataBufLock.getAlock());


    if (left_count_to_read > 0) {
        ALOGW("left_count_to_read %d!!", left_count_to_read);
    }

    // apply volume if need
    bytes -= left_count_to_read;
    ApplyVolume(buffer, bytes);


    ALOGV("%s(-), bytes: %u", __FUNCTION__, (uint32_t)bytes);
    return bytes;
}


bool AudioALSACaptureDataClientVoiceRecProc::IsLowLatencyCapture(void) {
    bool low_latency_on = false;
    bool voip_on = IsVoIPEnable();

#ifdef UPLINK_LOW_LATENCY
    if (voip_on == false &&
        (mStreamAttributeTarget->mAudioInputFlags & AUDIO_INPUT_FLAG_FAST)) {
        low_latency_on = true;
    }
#endif

    ALOGV("%s(), low_latency_on: %d, voip_on: %d", __FUNCTION__, low_latency_on, voip_on);
    return low_latency_on;
}


int AudioALSACaptureDataClientVoiceRecProc::getCapturePosition(int64_t *frames, int64_t *time) {
    if (mCaptureDataProvider == NULL || frames == NULL || time == NULL) {
        return -EINVAL;
    }

    /* Convert provider sample rate to streamin sample rate*/
    int ret = mCaptureDataProvider->getCapturePosition(frames, time);
    *frames = (*frames) * mStreamAttributeTarget->sample_rate / mStreamAttributeSource->sample_rate;
    ALOGV("%s(), frames = %" PRIu64 ", tar sample = %d, src sample = %d", __FUNCTION__, *frames, mStreamAttributeTarget->sample_rate, mStreamAttributeSource->sample_rate);
    return ret;
}

bool AudioALSACaptureDataClientVoiceRecProc::IsNeedApplyVolume() {
    bool ret = false;

    /* Only real input CaptureDataprovider need to apply volume for mic mute */
    switch (mCaptureDataProvider->getCaptureDataProviderType()) {
    case CAPTURE_PROVIDER_NORMAL:
    case CAPTURE_PROVIDER_BT_SCO:
    case CAPTURE_PROVIDER_BT_CVSD:
    case CAPTURE_PROVIDER_TDM_RECORD:
    case CAPTURE_PROVIDER_EXTERNAL:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}


status_t AudioALSACaptureDataClientVoiceRecProc::ApplyVolume(void *Buffer, uint32_t BufferSize) {
    if (IsNeedApplyVolume() == false) {
        return NO_ERROR;
    }

    // check if need apply mute
    if (mMicMute != mStreamAttributeTarget->micmute) {
        mMicMute =  mStreamAttributeTarget->micmute ;
        mMuteTransition = false;
    }

    if (mMicMute == true) {
        // do ramp down
        if (mMuteTransition == false) {
            uint32_t count = BufferSize >> 1;
            float Volume_inverse = ((float)MTK_STREAMIN_VOLUEM_MAX / (float)count) * -1;
            short *pPcm = (short *)Buffer;
            int ConsumeSample = 0;
            int value = 0;
            while (count) {
                value = *pPcm * (MTK_STREAMIN_VOLUEM_MAX + (Volume_inverse * ConsumeSample));
                *pPcm = clamp16(value >> MTK_STREAMIN_VOLUME_VALID_BIT);
                pPcm++;
                count--;
                ConsumeSample ++;
                //ALOGD("ApplyVolume Volume_inverse = %f ConsumeSample = %d",Volume_inverse,ConsumeSample);
            }
            mMuteTransition = true;
        } else {
            memset(Buffer, 0, BufferSize);
        }
    } else if (mMicMute == false) {
        // do ramp up
        if (mMuteTransition == false) {
            uint32_t count = BufferSize >> 1;
            float Volume_inverse = ((float)MTK_STREAMIN_VOLUEM_MAX / (float)count);
            short *pPcm = (short *)Buffer;
            int ConsumeSample = 0;
            int value = 0;
            while (count) {
                value = *pPcm * (Volume_inverse * ConsumeSample);
                *pPcm = clamp16(value >> MTK_STREAMIN_VOLUME_VALID_BIT);
                pPcm++;
                count--;
                ConsumeSample ++;
                //ALOGD("ApplyVolume Volume_inverse = %f ConsumeSample = %d",Volume_inverse,ConsumeSample);
            }
            mMuteTransition = true;
        }
    }

    return NO_ERROR;
}

// Raw buffer +++
status_t AudioALSACaptureDataClientVoiceRecProc::initRawBuffer(void)
{
    mRawDataBuf.size = getPeriodBufSize(mStreamAttributeSource, mLatency * 4);
    AUDIO_ALLOC_CHAR_BUFFER(mRawDataBuf.base, mRawDataBuf.size);
    mRawDataBuf.read = mRawDataBuf.write = mRawDataBuf.base;

    mRawLinearBuf.buf_size    = getPeriodBufSize(mStreamAttributeSource, mLatency);
    mRawLinearBuf.frame_count = getPeriodBufSize(mStreamAttributeSource, mLatency) /
                                getSizePerFrame (mStreamAttributeSource->audio_format, mStreamAttributeSource->num_channels);
    mRawLinearBuf.num_channels = mStreamAttributeSource->num_channels;
    mRawLinearBuf.sample_rate  = mStreamAttributeSource->sample_rate;
    mRawLinearBuf.audio_format = mStreamAttributeSource->audio_format;
    AUDIO_ALLOC_CHAR_BUFFER(mRawLinearBuf.buf, mRawLinearBuf.buf_size);
    mRawDataPeriodBufSize = getPeriodBufSize(mStreamAttributeSource, mLatency);
    ALOGI("%s(), Raw Buf: rate(%d) ch(%d) fmt(%d) size(%d) buf(%p)\n",
                                                   __FUNCTION__,
                                           mRawLinearBuf.sample_rate,
                                           mRawLinearBuf.num_channels,
                                           mRawLinearBuf.audio_format,
                                           mRawLinearBuf.buf_size,
                                           mRawLinearBuf.buf);
    return NO_ERROR;
}

status_t AudioALSACaptureDataClientVoiceRecProc::deInitRawBuffer(void)
{
   AUDIO_FREE_POINTER(mRawLinearBuf.buf);
   AUDIO_FREE_POINTER(mRawDataBuf.base);
   return NO_ERROR;
}
// Raw buffer ---


//pre sample rate convert +++
status_t AudioALSACaptureDataClientVoiceRecProc::initBliSrcPre(int src_rates, int dst_rates)
{
    SRC_PCM_FORMAT src_pcm_format = SRC_IN_END;
    audio_linear_buf_t *source = &mRawLinearBuf;
    audio_linear_buf_t *target = &mSrcPreOutLinearBuf;

    // raise multi-ch BliSrc flag
    mBliSrcMultiPre = (source->num_channels > 2);

    //TODO not support AUDIO_FORMAT_PCM_8_24_BIT SRC
    ASSERT(source->audio_format != AUDIO_FORMAT_PCM_8_24_BIT);

    //If there is no pre-src, keep 2 buffer same
    memcpy(target, source, sizeof(audio_linear_buf_t));
    // init BLI SRC if need
    if (src_rates  != dst_rates) {
        switch (source->audio_format) {
        case AUDIO_FORMAT_PCM_16_BIT:
            src_pcm_format = SRC_IN_Q1P15_OUT_Q1P15;
            break;
        case AUDIO_FORMAT_PCM_8_24_BIT:
            if (target->audio_format == AUDIO_FORMAT_PCM_16_BIT) {
                AUD_LOG_V("SRC_IN_Q9P23_OUT_Q1P31");
                src_pcm_format = SRC_IN_Q9P23_OUT_Q1P31; /* NOTE: sync with BCV_IN_Q1P31_OUT_Q1P15 */
            } else {
                AUD_WARNING("SRC not support AUDIO_FORMAT_PCM_8_24_BIT!!");
            }
            break;
        case AUDIO_FORMAT_PCM_32_BIT:
            src_pcm_format = SRC_IN_Q1P31_OUT_Q1P31;
            break;
        default:
            AUD_LOG_W("%s(), SRC format not support (%d->%d)", __FUNCTION__, source->audio_format, target->audio_format);
            src_pcm_format = SRC_IN_END;
            AUD_WARNING("audio_format error!");
            break;
        }

        AUD_LOG_D("=>%s(), sample_rate: %d => %d, num_channels: %d => %d, audio_format: 0x%x, 0x%x, SRC_PCM_FORMAT = %d", __FUNCTION__,
                  src_rates,  dst_rates,
                  source->num_channels, source->num_channels,
                  source->audio_format, source->audio_format,
                  src_pcm_format);

        mBliSrcPre = newMtkAudioSrc(
                      src_rates, mStreamAttributeSource->num_channels,
                      dst_rates,  mStreamAttributeSource->num_channels,
                      src_pcm_format);
        ASSERT(mBliSrcPre != NULL);

        if (!mBliSrcMultiPre)
            mBliSrcPre->open();
        else
            mBliSrcPre->multiChannelOpen();

        target->sample_rate = dst_rates;
        target->frame_count = (mLatency * target->sample_rate) / 1000;
        target->buf_size    = target->frame_count * getSizePerFrame(target->audio_format, target->num_channels);
        target->buf = NULL;
        AUDIO_ALLOC_CHAR_BUFFER(target->buf, target->buf_size);
        //Ring Buffer Allocate
        mSrcPreOutBuf.size = target->buf_size * 2;
        AUDIO_ALLOC_CHAR_BUFFER(mSrcPreOutBuf.base, mSrcPreOutBuf.size);
        mSrcPreOutBuf.read = mSrcPreOutBuf.write = mSrcPreOutBuf.base;
    }

    ALOGI("%s(), Pre SRC(%d) Out Buf: rate(%d) ch(%d) fmt(%d) size(%d) buf(%p)\n",
                                                   __FUNCTION__,
                                           (mBliSrcPre != NULL),
                                           target->sample_rate,
                                           target->num_channels,
                                           target->audio_format,
                                           target->buf_size,
                                           target->buf);
    return NO_ERROR;
}


status_t AudioALSACaptureDataClientVoiceRecProc::deinitBliSrcPre()
{
    // deinit BLI SRC if need
    if (mBliSrcPre != NULL) {
        mBliSrcPre->close();
        deleteMtkAudioSrc(mBliSrcPre);
        mBliSrcPre = NULL;
        AUDIO_FREE_POINTER(mSrcPreOutLinearBuf.buf);
        AUDIO_FREE_POINTER(mSrcPreOutBuf.base);
    } else {
        mProcInLinearBuf.buf = NULL; //Temp
    }

    return NO_ERROR;
}


status_t AudioALSACaptureDataClientVoiceRecProc::doBliSrcPre(void *pInBuffer, uint32_t inBytes, void *pOutBuffer, uint32_t *pOutBytes)
{
    if (mBliSrcPre != NULL) {
        uint32_t sizeIn = inBytes;
        uint32_t sizeOut = *pOutBytes;
        uint32_t consumed;

        if (!mBliSrcMultiPre)
            mBliSrcPre->process((int16_t *)pInBuffer, &sizeIn,
                             (int16_t *)pOutBuffer, &sizeOut);
        else
            mBliSrcPre->multiChannelProcess((int16_t *)pInBuffer, &sizeIn,
                             (int16_t *)pOutBuffer, &sizeOut);

        consumed = inBytes - sizeIn;

        ALOGV("%s(), src input data left = %u,  out data = %u",
              __FUNCTION__, consumed, sizeOut);

        ASSERT(consumed == inBytes);
        *pOutBytes = sizeOut;
    } else {
        *pOutBytes = inBytes;
    }

    return NO_ERROR;
}


//pre sample rate convert ---

//User Process +++
status_t AudioALSACaptureDataClientVoiceRecProc::initDataProcess(void)
{
    int ret = 0;

    audio_linear_buf_t *source = &mProcInLinearBuf;
    audio_linear_buf_t *target = &mProcOutLinearBuf;

    //Because last process node is pre src, need connect it
    memcpy(source, &mSrcPreOutLinearBuf, sizeof(audio_linear_buf_t));

    memcpy(target, source, sizeof(audio_linear_buf_t));
    if (mProcess) {
        proc_fmt_desc_t in, out;

        in.format =   source->audio_format;
        in.channels = source->num_channels;
        in.rates =      source->sample_rate;

        ret = mProcess->init(in, &out);
        ASSERT(ret == 0);

        //If there is pre src, we need allocate in buffer again
        if (mBliSrcPre) {
            source->buf = NULL;
            AUDIO_ALLOC_CHAR_BUFFER(source->buf, source->buf_size);
            ASSERT(source->buf != NULL);
        }

        target->audio_format = out.format;
        target->num_channels = out.channels;
        target->buf_size     = target->frame_count * getSizePerFrame(target->audio_format, target->num_channels);
        target->buf = NULL;
        AUDIO_ALLOC_CHAR_BUFFER(target->buf, target->buf_size);
        ASSERT(target->buf != NULL);

    }

    ALOGI("%s(), Process(%d) In Buf: rate(%d) ch(%d) fmt(%d) size(%d) buf(%p)\n",
                                                   __FUNCTION__,
                                           (mProcess != NULL),
                                           source->sample_rate,
                                           source->num_channels,
                                           source->audio_format,
                                           source->buf_size,
                                           source->buf);

    ALOGI("%s(), Process(%d) Out Buf: rate(%d) ch(%d) fmt(%d) size(%d) buf(%p)\n",
                                                   __FUNCTION__,
                                           (mProcess != NULL),
                                           target->sample_rate,
                                           target->num_channels,
                                           target->audio_format,
                                           target->buf_size,
                                           target->buf);

    return NO_ERROR;
}

status_t AudioALSACaptureDataClientVoiceRecProc::deInitDataProcess(void)
{
    /* process uninit */

    if (mProcess) {
        mProcess->uninit();

        AUDIO_FREE_POINTER(mProcInLinearBuf.buf);
        AUDIO_FREE_POINTER(mProcOutLinearBuf.buf);
    }
    return NO_ERROR;
}

//User Process ---

//Post sample rate convert +++
status_t AudioALSACaptureDataClientVoiceRecProc::initBliSrcPost(int src_rates, int dst_rates)
{
    SRC_PCM_FORMAT src_pcm_format = SRC_IN_END;
    audio_linear_buf_t *source = &mProcOutLinearBuf;
    audio_linear_buf_t *target = &mSrcPostOutLinearBuf;

    // raise multi-ch BliSrc flag
    mBliSrcMultiPost = (source->num_channels > 2);

    //TODO not support AUDIO_FORMAT_PCM_8_24_BIT SRC
    ASSERT(source->audio_format != AUDIO_FORMAT_PCM_8_24_BIT);

    memcpy(target, source, sizeof(audio_linear_buf_t));

    // init BLI SRC if need
    if (src_rates  != dst_rates) {
        switch (source->audio_format) {
        case AUDIO_FORMAT_PCM_16_BIT:
            src_pcm_format = SRC_IN_Q1P15_OUT_Q1P15;
            break;
        case AUDIO_FORMAT_PCM_8_24_BIT:
            if (target->audio_format == AUDIO_FORMAT_PCM_16_BIT) {
                AUD_LOG_V("SRC_IN_Q9P23_OUT_Q1P31");
                src_pcm_format = SRC_IN_Q9P23_OUT_Q1P31; /* NOTE: sync with BCV_IN_Q1P31_OUT_Q1P15 */
            } else {
                AUD_WARNING("SRC not support AUDIO_FORMAT_PCM_8_24_BIT!!");
            }
            break;
        case AUDIO_FORMAT_PCM_32_BIT:
            src_pcm_format = SRC_IN_Q1P31_OUT_Q1P31;
            break;
        default:
            AUD_LOG_W("%s(), SRC format not support (%d->%d)", __FUNCTION__, source->audio_format, target->audio_format);
            src_pcm_format = SRC_IN_END;
            AUD_WARNING("audio_format error!");
            break;
        }

        AUD_LOG_D("=>%s(), sample_rate: %d => %d, num_channels: %d => %d, audio_format: 0x%x, 0x%x, SRC_PCM_FORMAT = %d", __FUNCTION__,
                  src_rates,  dst_rates,
                  source->num_channels, source->num_channels,
                  source->audio_format, source->audio_format,
                  src_pcm_format);

        mBliSrcPost = newMtkAudioSrc(
                      src_rates, source->num_channels,
                      dst_rates,  target->num_channels,
                      src_pcm_format);
        ASSERT(mBliSrcPost != NULL);

        if (!mBliSrcMultiPost)
            mBliSrcPost->open();
        else
            mBliSrcPost->multiChannelOpen();

        target->sample_rate = dst_rates;
        target->frame_count = (mLatency * target->sample_rate) / 1000;
        target->buf_size    = target->frame_count * getSizePerFrame(target->audio_format, target->num_channels);
        target->buf = NULL;
        AUDIO_ALLOC_CHAR_BUFFER(target->buf, target->buf_size);
    }

    ALOGI("%s(), Post SRC(%d) Out Buf: rate(%d) ch(%d) fmt(%d) size(%d) buf(%p)\n",
                                                   __FUNCTION__,
                                           (mBliSrcPost != NULL),
                                           target->sample_rate,
                                           target->num_channels,
                                           target->audio_format,
                                           target->buf_size,
                                           target->buf);

    return NO_ERROR;
}


status_t AudioALSACaptureDataClientVoiceRecProc::deInitBliSrcPost()
{
    // deinit BLI SRC if need
    if (mBliSrcPost != NULL) {
        mBliSrcPost->close();
        deleteMtkAudioSrc(mBliSrcPost);
        mBliSrcPost = NULL;
        AUDIO_FREE_POINTER(mSrcPostOutLinearBuf.buf);
    }

    return NO_ERROR;
}

status_t AudioALSACaptureDataClientVoiceRecProc::doBliSrcPost(void *pInBuffer, uint32_t inBytes, void *pOutBuffer, uint32_t *pOutBytes)
{
    if (mBliSrcPost != NULL) {
        uint32_t sizeIn = inBytes;
        uint32_t sizeOut = *pOutBytes;
        uint32_t consumed;

        if (!mBliSrcMultiPost)
            mBliSrcPost->process((int16_t *)pInBuffer, &sizeIn,
                             (int16_t *)pOutBuffer, &sizeOut);
        else
            mBliSrcPost->multiChannelProcess((int16_t *)pInBuffer, &sizeIn,
                             (int16_t *)pOutBuffer, &sizeOut);

        consumed = inBytes - sizeIn;

        ALOGV("%s(), src input data left = %u,  out data = %u",
              __FUNCTION__, consumed, sizeOut);

        ASSERT(consumed == inBytes);
        *pOutBytes = sizeOut;
    } else {
        *pOutBytes = inBytes;
    }

    return NO_ERROR;
}

//Post sample rate convert ---

// Processed buffer +++
status_t AudioALSACaptureDataClientVoiceRecProc::initProcessedBuffer(void)
{
    mProcessedDataBuf.size = getPeriodBufSize(mStreamAttributeTarget, mLatency * 4);
    AUDIO_ALLOC_CHAR_BUFFER(mProcessedDataBuf.base, mProcessedDataBuf.size);
    mProcessedDataBuf.read = mProcessedDataBuf.write = mProcessedDataBuf.base;

    mProcessedLinearBuf.buf_size    = getPeriodBufSize(mStreamAttributeTarget, mLatency);
    mProcessedLinearBuf.frame_count = getPeriodBufSize(mStreamAttributeTarget, mLatency) /
                                getSizePerFrame (mStreamAttributeTarget->audio_format, mStreamAttributeTarget->num_channels);
    mProcessedLinearBuf.num_channels = mStreamAttributeTarget->num_channels;
    mProcessedLinearBuf.sample_rate  = mStreamAttributeTarget->sample_rate;
    mProcessedLinearBuf.audio_format = mStreamAttributeTarget->audio_format;

    if (mNeedChannelCvt) {
        AUDIO_ALLOC_CHAR_BUFFER(mProcessedLinearBuf.buf, mProcessedLinearBuf.buf_size);
    } else {
        mProcessedLinearBuf.buf = mSrcPostOutLinearBuf.buf;
    }
    mProcessedDataPeriodBufSize = getPeriodBufSize(mStreamAttributeTarget, mLatency);
    ALOGI("%s(), Processed Buf: rate(%d) ch(%d) fmt(%d) size(%d) buf(%p)\n",
                                                   __FUNCTION__,
                                           mProcessedLinearBuf.sample_rate,
                                           mProcessedLinearBuf.num_channels,
                                           mProcessedLinearBuf.audio_format,
                                           mProcessedLinearBuf.buf_size,
                                           mProcessedLinearBuf.buf);
    return NO_ERROR;

}

status_t AudioALSACaptureDataClientVoiceRecProc::deInitProcessedBuffer(void)
{
    AUDIO_FREE_POINTER(mProcessedDataBuf.base);
    if (mNeedChannelCvt)
        AUDIO_FREE_POINTER(mProcessedLinearBuf.buf);
    return NO_ERROR;
}
// Processed buffer ---

//Android Native Preprocess effect +++
void AudioALSACaptureDataClientVoiceRecProc::CheckNativeEffect(void) {
    if (mStreamAttributeTarget->NativePreprocess_Info.PreProcessEffect_Update == false) {
        return; // nothing to update
    }

    ASSERT(mAudioPreProcessEffect != NULL);
    mAudioPreProcessEffect->CheckNativeEffect();
    mStreamAttributeTarget->NativePreprocess_Info.PreProcessEffect_Update = false;
}


uint32_t AudioALSACaptureDataClientVoiceRecProc::NativePreprocess(void *buffer, uint32_t bytes) {
    CheckNativeEffect(); // stream in might addAudioEffect/removeAudioEffect
    if (mAudioPreProcessEffect->num_preprocessors == 0 || IsVoIPEnable() == true) {
        return bytes;
    }

    return mAudioPreProcessEffect->NativePreprocess(buffer, bytes, &mStreamAttributeSource->Time_Info);
}
//Android Native Preprocess effect ---

#define LINEAR_DOWNMIX_SIMPLE(src_buf, src_size, type, dst_buf, src_ch, dst_ch) \
            ({ \
                uint32_t __channel_size = (src_size / src_ch); \
                uint32_t __num_sample = __channel_size / sizeof(type); \
                uint32_t __data_size_ret = __channel_size * dst_ch; \
                type    *__linear_src = (type *)(src_buf); \
                type    *__linear_dst = (type *)(dst_buf); \
                uint32_t __idx_sample = 0; \
                for (__idx_sample = 0; __idx_sample < __num_sample; __idx_sample++) { \
                    memcpy(__linear_dst, __linear_src, dst_ch * sizeof(type)); \
                    __linear_dst += dst_ch; \
                    __linear_src += src_ch; \
                } \
                __data_size_ret; \
            })

#define LINEAR_1CH_UPMIX_TO_2CH(src_buf, src_size, type, dst_buf) \
            ({ \
                uint32_t __channel_size = src_size; \
                uint32_t __num_sample = __channel_size / sizeof(type); \
                uint32_t __data_size_ret = __channel_size * 2; \
                type    *__linear_src = (type *)(src_buf); \
                type    *__linear_dst = (type *)(dst_buf); \
                uint32_t __idx_sample = 0; \
                for (__idx_sample = 0; __idx_sample < __num_sample; __idx_sample++) { \
                    *(__linear_dst + 0) = *__linear_src; \
                    *(__linear_dst + 1) = *__linear_src; \
                    __linear_dst += 2; \
                    __linear_src += 1; \
                } \
                __data_size_ret; \
            })

uint32_t AudioALSACaptureDataClientVoiceRecProc::ApplyChannelRemix(char *src_buf, uint32_t src_size,
           char *dst_buf, uint32_t *pdst_size)
{
    uint32_t src_ch, dst_ch;
    uint32_t dst_size;

    if (!mNeedChannelCvt) {
        *pdst_size = src_size;
        return 0;
    }

    src_ch = mSrcPostOutLinearBuf.num_channels;
    dst_ch = mProcessedLinearBuf.num_channels;

    if (src_ch > dst_ch) {
        if (mSrcPostOutLinearBuf.audio_format == AUDIO_FORMAT_PCM_16_BIT)
            dst_size = LINEAR_DOWNMIX_SIMPLE(src_buf, src_size, int16_t, dst_buf, src_ch, dst_ch);
        else
            dst_size = LINEAR_DOWNMIX_SIMPLE(src_buf, src_size, int32_t, dst_buf, src_ch, dst_ch);
    } else if (src_ch == 1 && dst_ch == 2) {
        if (mSrcPostOutLinearBuf.audio_format == AUDIO_FORMAT_PCM_16_BIT)
            dst_size = LINEAR_1CH_UPMIX_TO_2CH(src_buf, src_size, int16_t, dst_buf);
        else
            dst_size = LINEAR_1CH_UPMIX_TO_2CH(src_buf, src_size, int32_t, dst_buf);
    } else {
        //not support other format
        ASSERT(0);
    }

    *pdst_size = dst_size;
    return 0;
}

uint32_t AudioALSACaptureDataClientVoiceRecProc::TransferFormat(char *src_buf, uint32_t src_size, char *dst_buf, uint32_t *pdst_size)
{
    //TODO
    *pdst_size = src_size;
    return 0;
}


void AudioALSACaptureDataClientVoiceRecProc::handleLockTimeout() {
    ALOGE("%s(), Lock timeout. Reopen StreamIn", __FUNCTION__);
    setStreamInReopen(true);
}


} // end of namespace android

