#include "AudioALSACaptureDataProviderHwCM.h"

#include <pthread.h>

#include <sys/prctl.h>

#include "AudioType.h"
#include "AudioALSADriverUtility.h"
#include "AudioSpeechEnhanceInfo.h"
#include "AudioALSAStreamManager.h"
#include "AudioALSAHardwareResourceManager.h"
#include "audio_custom_exp.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "AudioALSACaptureDataProviderHwCM"

namespace android {


/*==============================================================================
 *                     Constant
 *============================================================================*/

static uint32_t kReadBufferSize = 0;
#define HW_CM_CAPTURE_TOTAL_CH    4;

/*==============================================================================
 *                     Implementation
 *============================================================================*/

AudioALSACaptureDataProviderHwCM *AudioALSACaptureDataProviderHwCM::mAudioALSACaptureDataProvider = NULL;
AudioALSACaptureDataProviderHwCM *AudioALSACaptureDataProviderHwCM::getInstance() {
    static AudioLock mGetInstanceLock;
    AL_AUTOLOCK(mGetInstanceLock);

    if (mAudioALSACaptureDataProvider == NULL) {
        mAudioALSACaptureDataProvider = new AudioALSACaptureDataProviderHwCM();
    }
    ASSERT(mAudioALSACaptureDataProvider != NULL);
    return mAudioALSACaptureDataProvider;
}

AudioALSACaptureDataProviderHwCM::AudioALSACaptureDataProviderHwCM():
    hReadThread(0) {
    ALOGD("%s()", __FUNCTION__);

    mCaptureDataProviderType = CAPTURE_PROVIDER_HWCM;
}

AudioALSACaptureDataProviderHwCM::~AudioALSACaptureDataProviderHwCM() {
    ALOGD("%s()", __FUNCTION__);
}


status_t AudioALSACaptureDataProviderHwCM::open() {

    ALOGD("%s(+)", __FUNCTION__);

    ASSERT(mEnable == false);

    int pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmVUL2Capture);
    int cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmVUL2Capture);
    ALOGD("cardindex = %d, pcmindex = %d", cardindex, pcmindex);

    // Config pcm config
    mConfig.channels = HW_CM_CAPTURE_TOTAL_CH;
    mConfig.rate = 16000; //TODO consider 48Khz
    mConfig.format = PCM_FORMAT_S16_LE;
    mConfig.start_threshold = 0;
    mConfig.stop_threshold = 0;
    mConfig.silence_threshold = 0;

    // config mStreamAttributeSource
    mStreamAttributeSource.num_channels= mConfig.channels;   // customize channel by macro define
    mStreamAttributeSource.sample_rate = mConfig.rate;
    if (mConfig.format == PCM_FORMAT_S16_LE)
        mStreamAttributeSource.audio_format = AUDIO_FORMAT_PCM_16_BIT;
    else
        mStreamAttributeSource.audio_format = AUDIO_FORMAT_PCM_8_24_BIT;

    mStreamAttributeSource.Time_Info.total_frames_readed = 0;

    mlatency = 16;
    kReadBufferSize = getPeriodBufSize(&mStreamAttributeSource, mlatency);

    mConfig.period_size = (kReadBufferSize / mConfig.channels / (pcm_format_to_bits(mConfig.format) / 8));
    mConfig.period_count = 4;


    ALOGD("pcm format: %d, channels: %d, rate: %d, period_size: %d, period_count: %d",
          mConfig.format, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count);
    ALOGD("latency: %d, kReadBufferSize: %u", mlatency, kReadBufferSize);

    // enable pcm
    ASSERT(mPcm == NULL);
    OpenPCMDump(LOG_TAG);
    mPcm = pcm_open(cardindex, pcmindex, PCM_IN | PCM_MONOTONIC, &mConfig);
    ASSERT(mPcm != NULL && pcm_is_ready(mPcm) == true);
    ALOGV("%s(), mPcm = %p", __FUNCTION__, mPcm);

    mStart = false;
    mReadThreadReady = false;

    // create reading thread
    mEnable = true;
    int ret = pthread_create(&hReadThread, NULL, AudioALSACaptureDataProviderHwCM::readThread, (void *)this);
    if (ret != 0) {
        ALOGE("%s() create hReadThread fail!!", __FUNCTION__);
        return UNKNOWN_ERROR;
    }

    ALOGD("%s(-)", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSACaptureDataProviderHwCM::close() {
    ALOGD("%s()", __FUNCTION__);

    mEnable = false;
    pthread_join(hReadThread, NULL);
    ALOGD("pthread_join hReadThread done");

    AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

    pcm_stop(mPcm);
    pcm_close(mPcm);
    mPcm = NULL;

    return NO_ERROR;
}


void *AudioALSACaptureDataProviderHwCM::readThread(void *arg) {
    status_t retval = NO_ERROR;
    int ret = 0;
    uint32_t counter = 1;

    AudioALSACaptureDataProviderHwCM *pDataProvider = static_cast<AudioALSACaptureDataProviderHwCM *>(arg);

    uint32_t open_index = pDataProvider->mOpenIndex;

    char nameset[32];
    sprintf(nameset, "%s%d", __FUNCTION__, pDataProvider->mCaptureDataProviderType);
    prctl(PR_SET_NAME, (unsigned long)nameset, 0, 0, 0);

#ifdef MTK_AUDIO_ADJUST_PRIORITY
    // force to set priority
    struct sched_param sched_p;
    sched_getparam(0, &sched_p);
    sched_p.sched_priority = RTPM_PRIO_AUDIO_RECORD + 1;
    if (0 != sched_setscheduler(0, SCHED_RR, &sched_p)) {
        ALOGE("[%s] failed, errno: %d", __FUNCTION__, errno);
    } else {
        sched_p.sched_priority = RTPM_PRIO_AUDIO_RECORD + 1;
        sched_getparam(0, &sched_p);
        ALOGD("sched_setscheduler ok, priority: %d", sched_p.sched_priority);
    }
#endif
    ALOGD("+%s(), pid: %d, tid: %d, kReadBufferSize=0x%x, open_index=%d, counter=%d ", __FUNCTION__, getpid(), gettid(), kReadBufferSize, open_index, counter);

    pDataProvider->waitPcmStart();

    // read raw data from alsa driver
    char linear_buffer[kReadBufferSize];
    uint32_t Read_Size = kReadBufferSize;
    while (pDataProvider->mEnable == true) {
        ASSERT(open_index == pDataProvider->mOpenIndex);
        ASSERT(pDataProvider->mPcm != NULL);

        ret = pDataProvider->pcmRead(pDataProvider->mPcm, linear_buffer, Read_Size);
        if (ret != 0) {
            ALOGE("%s(), pcm_read() error, retval = %d", __FUNCTION__, retval);
            continue;
        }

        //struct timespec tempTimeStamp;
        retval = pDataProvider->GetCaptureTimeStamp(&pDataProvider->mStreamAttributeSource.Time_Info, Read_Size);

        // use ringbuf format to save buffer info
        pDataProvider->mPcmReadBuf.pBufBase = linear_buffer;
        pDataProvider->mPcmReadBuf.bufLen    = Read_Size + 1; // +1: avoid pRead == pWrite
        pDataProvider->mPcmReadBuf.pRead    = linear_buffer;
        pDataProvider->mPcmReadBuf.pWrite    = linear_buffer + Read_Size;
        pDataProvider->provideCaptureDataToAllClients(open_index);
    }

    ALOGD("-%s(), pid: %d, tid: %d", __FUNCTION__, getpid(), gettid());
    pthread_exit(NULL);
    return NULL;
}

} // end of namespace android
