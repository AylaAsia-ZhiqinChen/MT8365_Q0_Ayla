#include "AudioALSACaptureDataProviderVoiceMix.h"

#include <pthread.h>

#include <sys/prctl.h>

#include "AudioType.h"

#include "SpeechDataProcessingHandler.h"
#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "AudioALSACaptureDataProviderMix"

namespace android {


/*==============================================================================
 *                     Constant
 *============================================================================*/


/*==============================================================================
 *                     Implementation
 *============================================================================*/
AudioLock mCaptureDataProviderVoiceMixLock;
AudioALSACaptureDataProviderVoiceMix *AudioALSACaptureDataProviderVoiceMix::mAudioALSACaptureDataProviderVoiceMix = NULL;
AudioALSACaptureDataProviderVoiceMix *AudioALSACaptureDataProviderVoiceMix::getInstance() {
    AL_AUTOLOCK(mCaptureDataProviderVoiceMixLock);

    if (mAudioALSACaptureDataProviderVoiceMix == NULL) {
        mAudioALSACaptureDataProviderVoiceMix = new AudioALSACaptureDataProviderVoiceMix();
    }
    ASSERT(mAudioALSACaptureDataProviderVoiceMix != NULL);
    return mAudioALSACaptureDataProviderVoiceMix;
}

bool AudioALSACaptureDataProviderVoiceMix::hasInstance() {
    return mAudioALSACaptureDataProviderVoiceMix != NULL ? true : false;
}

AudioALSACaptureDataProviderVoiceMix::AudioALSACaptureDataProviderVoiceMix() {
    ALOGD("%s()", __FUNCTION__);
    mCaptureDataProviderType = CAPTURE_PROVIDER_VOICE;
    memset(&mRawDataRingBuf, 0, sizeof(mRawDataRingBuf));
    mPeriodBufferSize = 0;
}

AudioALSACaptureDataProviderVoiceMix::~AudioALSACaptureDataProviderVoiceMix() {
    ALOGD("+%s()\n", __FUNCTION__);
    AL_AUTOLOCK(mCaptureDataProviderVoiceMixLock);

    mAudioALSACaptureDataProviderVoiceMix = NULL;
    SpeechDataProcessingHandler::destoryInstanceSafely();
    ALOGD("-%s()\n", __FUNCTION__);
}


status_t AudioALSACaptureDataProviderVoiceMix::open() {
    ALOGD("%s()", __FUNCTION__);

    ASSERT(mEnable == false);

    SpeechDataProcessingHandler::getInstance()->getStreamAttributeSource(SpeechDataProcessingHandler::VOICE_MIX_CALLER, &mStreamAttributeSource);

    mPeriodBufferSize = getPeriodBufSize(&mStreamAttributeSource, UPLINK_NORMAL_LATENCY_MS);
    mRawDataRingBuf.bufLen = mPeriodBufferSize * 4;

    /* Setup ringbuf */
    mRawDataRingBuf.pBufBase = new char[mRawDataRingBuf.bufLen];
    mRawDataRingBuf.pRead    = mRawDataRingBuf.pBufBase;
    mRawDataRingBuf.pWrite   = mRawDataRingBuf.pBufBase;
    mRawDataRingBuf.pBufEnd  = mRawDataRingBuf.pBufBase + mRawDataRingBuf.bufLen;

    ALOGD("%s(), mStreamAttributeSource: audio_format = %d, num_channels = %d, audio_channel_mask = %x, sample_rate = %d, periodBufferSize = %d\n",
          __FUNCTION__, mStreamAttributeSource.audio_format, mStreamAttributeSource.num_channels, mStreamAttributeSource.audio_channel_mask, mStreamAttributeSource.sample_rate, mPeriodBufferSize);

    mEnable = true;

    OpenPCMDump(LOG_TAG);

    return SpeechDataProcessingHandler::getInstance()->recordOn(RECORD_TYPE_MIX);
}

status_t AudioALSACaptureDataProviderVoiceMix::close() {
    ALOGD("%s()", __FUNCTION__);

    mEnable = false;

    delete[] mRawDataRingBuf.pBufBase;
    memset(&mRawDataRingBuf, 0, sizeof(mRawDataRingBuf));

    ClosePCMDump();

    return SpeechDataProcessingHandler::getInstance()->recordOff(RECORD_TYPE_MIX);
}

status_t AudioALSACaptureDataProviderVoiceMix::provideModemRecordDataToProvider(RingBuf modem_record_buf) {
    if (mEnable == false) {
        //ALOGW("%s(), already closed, return", __FUNCTION__);
        return NO_INIT;
    }

    int cntInRingBufs = RingBuf_getDataCount(&modem_record_buf);
    int freeSpaceInRingBuft = RingBuf_getFreeSpace(&mRawDataRingBuf);

    RingBuf_copyFromRingBuf(&mRawDataRingBuf, &modem_record_buf, RingBuf_getDataCount(&modem_record_buf));

    if ((uint32_t)RingBuf_getDataCount(&mRawDataRingBuf) >= mPeriodBufferSize) {
        mPcmReadBuf = mRawDataRingBuf;
        ALOGD("%s(), Data enough(%d), pBufBase(%p), bufLen(%d), pRead(%p), pWrite(%p)",
              __FUNCTION__,
              RingBuf_getDataCount(&mPcmReadBuf),
              mPcmReadBuf.pBufBase,
              mPcmReadBuf.bufLen,
              mPcmReadBuf.pRead,
              mPcmReadBuf.pWrite);

        provideCaptureDataToAllClients(mOpenIndex);

        // All data consumed, reset ring buf
        mRawDataRingBuf.pRead = mRawDataRingBuf.pWrite;
    } else {
        ALOGD("%s(), Data is not enough(%d), pBufBase(%p), bufLen(%d), pRead(%p), pWrite(%p)",
              __FUNCTION__,
              RingBuf_getDataCount(&mRawDataRingBuf),
              mRawDataRingBuf.pBufBase,
              mRawDataRingBuf.bufLen,
              mRawDataRingBuf.pRead,
              mRawDataRingBuf.pWrite);
    }

    return NO_ERROR;
}


} // end of namespace android


