#include "AudioALSACaptureDataProviderSoundTrigger.h"

#include <pthread.h>

#include <sys/prctl.h>

#include "AudioType.h"
#include "AudioALSADriverUtility.h"
#include "AudioALSAStreamManager.h"
#include "AudioALSAHardwareResourceManager.h"
#include "AudioALSAAdspVaController.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "AudioALSACaptureDataProviderSoundTrigger"

namespace android {


/*==============================================================================
 *                     Constant
 *============================================================================*/

static uint32_t kReadBufferSize = 0;

/*==============================================================================
 *                     Implementation
 *============================================================================*/

AudioALSACaptureDataProviderSoundTrigger *AudioALSACaptureDataProviderSoundTrigger::mAudioALSACaptureDataProvider = NULL;
AudioALSACaptureDataProviderSoundTrigger *AudioALSACaptureDataProviderSoundTrigger::getInstance() {
    static AudioLock mGetInstanceLock;
    AL_AUTOLOCK(mGetInstanceLock);

    if (mAudioALSACaptureDataProvider == NULL) {
        mAudioALSACaptureDataProvider = new AudioALSACaptureDataProviderSoundTrigger();
    }
    ASSERT(mAudioALSACaptureDataProvider != NULL);
    return mAudioALSACaptureDataProvider;
}

AudioALSACaptureDataProviderSoundTrigger::AudioALSACaptureDataProviderSoundTrigger()
{
    mCaptureDataProviderType = CAPTURE_PROVIDER_SOUNDTRIGGER;
    mStDev = AudioALSAAdspVaController::getInstance()->getSoundTriggerDevice(),
    mPcmStatus = NO_ERROR;
    ALOGD("%s()", __FUNCTION__);
}

AudioALSACaptureDataProviderSoundTrigger::~AudioALSACaptureDataProviderSoundTrigger() {
    ALOGD("%s()", __FUNCTION__);
}


status_t AudioALSACaptureDataProviderSoundTrigger::open() {

    ALOGD("%s(+)", __FUNCTION__);

    ASSERT(mEnable == false);

    mStreamAttributeSource.num_channels = 1;
    mStreamAttributeSource.audio_channel_mask = AUDIO_CHANNEL_IN_MONO;
    mStreamAttributeSource.audio_format = AUDIO_FORMAT_PCM_16_BIT;
    mStreamAttributeSource.sample_rate = 16000;
    // Reset frames readed counter
    mStreamAttributeSource.Time_Info.total_frames_readed = 0;

    mlatency = 16; //TODO temp set to 16ms acording with alorithm

    kReadBufferSize = getPeriodBufSize(&mStreamAttributeSource, mlatency);

    mStart = false;
    mReadThreadReady = false;
    if ((mStDev == NULL) ||
        (!mStDev->open_for_streaming) ||
        (!mStDev->close_for_streaming) ||
        (!mStDev->read_samples)) {
        mPcmStatus = BAD_VALUE;
        return BAD_VALUE;
    }

    mStDevHandle = mStDev->open_for_streaming();
    if (mStDevHandle <= 0) {
        mPcmStatus = BAD_VALUE;
        return BAD_VALUE;
    }

    mPcmStatus = NO_ERROR;

    // create reading thread
    mReadThreadReady = true;
    mEnable = true;
    int ret = pthread_create(&hReadThread, NULL, AudioALSACaptureDataProviderSoundTrigger::readThread, (void *)this);
    if (ret != 0) {
        ALOGE("%s() create hReadThread fail!!", __FUNCTION__);
        return UNKNOWN_ERROR;
    }

    ALOGD("%s(-)", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSACaptureDataProviderSoundTrigger::close() {
    ALOGD("%s()", __FUNCTION__);

    mEnable = false;

    pthread_join(hReadThread, NULL);
    mReadThreadReady = false;

    mStDev->close_for_streaming(mStDevHandle);
    mStDevHandle = 0;

    ALOGD("pthread_join hReadThread done");

    return NO_ERROR;
}


void *AudioALSACaptureDataProviderSoundTrigger::readThread(void *arg) {
    status_t retval = NO_ERROR;
    int ret = 0;
    uint32_t count = 0;
    sound_trigger_device_t *stdev;
    int stDevHandle;

    AudioALSACaptureDataProviderSoundTrigger *pDataProvider = static_cast<AudioALSACaptureDataProviderSoundTrigger *>(arg);

    uint32_t open_index = pDataProvider->mOpenIndex;
    stdev = pDataProvider->mStDev;
    stDevHandle = pDataProvider->mStDevHandle;

    char nameset[32];
    sprintf(nameset, "%s_soundtrigger", __FUNCTION__);
    prctl(PR_SET_NAME, (unsigned long)nameset, 0, 0, 0);


    // read raw data from alsa driver
    char linear_buffer[kReadBufferSize];
    uint32_t frames = kReadBufferSize / 2;

    //TODO now temp sleep 5 second, late need improve
    usleep(5000);
    while (pDataProvider->mEnable == true) {

        ret = stdev->read_samples(stDevHandle, linear_buffer, (size_t)frames);
        if (ret < (int)frames) {
            ALOGE("%s(), pcm_read() error, retval = %d", __FUNCTION__, retval);
            //need notify dataclient this error
            pDataProvider->mPcmReadBuf.pBufBase = linear_buffer;
            pDataProvider->mPcmReadBuf.bufLen = 0;
            pDataProvider->mPcmReadBuf.pRead = linear_buffer;
            pDataProvider->mPcmReadBuf.pWrite = linear_buffer;
            pDataProvider->provideCaptureDataToAllClients(open_index);
            mPcmStatus = BAD_VALUE;
            break;
        }

        // use ringbuf format to save buffer info
        pDataProvider->mPcmReadBuf.pBufBase = linear_buffer;
        pDataProvider->mPcmReadBuf.bufLen    = kReadBufferSize + 1; // +1: avoid pRead == pWrite
        pDataProvider->mPcmReadBuf.pRead    = linear_buffer;
        pDataProvider->mPcmReadBuf.pWrite    = linear_buffer + kReadBufferSize;
        pDataProvider->provideCaptureDataToAllClients(open_index);
    }

    ALOGD("-%s(), pid: %d, tid: %d", __FUNCTION__, getpid(), gettid());
    pthread_exit(NULL);
    return NULL;
}

} // end of namespace android
