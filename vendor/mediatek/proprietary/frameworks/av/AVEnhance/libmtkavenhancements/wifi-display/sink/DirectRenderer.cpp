/*
 * Copyright 2012, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "DirectRenderer"
#include <utils/Log.h>

#include "DirectRenderer.h"

#include <gui/SurfaceComposerClient.h>
#include <gui/Surface.h>
#include <media/AudioTrack.h>
#include <media/ICrypto.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/hexdump.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/MediaErrors.h>
#ifdef MTK_AOSP_ENHANCEMENT
#include <utils/String8.h>
#include <binder/IPCThreadState.h>
#include "DataPathTrace.h"
#include <cutils/properties.h>
//#include <media/AudioTrackCenter.h>
#include <utils/threads.h>
#endif
#include <media/MediaCodecBuffer.h>

#define UNUSED(x) ((void)(x))
namespace android {

#ifdef WFD_HDCP_RX_SUPPORT
static sp<IHDCP> gHDCP=NULL;
#endif
/*
   Drives the decoding process using a MediaCodec instance. Input buffers
   queued by calls to "queueInputBuffer" are fed to the decoder as soon
   as the decoder is ready for them, the client is notified about output
   buffers as the decoder spits them out.
*/
struct DirectRenderer::DecoderContext : public AHandler {
    enum {
        kWhatOutputBufferReady,
        kWhatError,
    };
    DecoderContext(const sp<AMessage> &notify);

    status_t init(
            const sp<AMessage> &format,
            const sp<IGraphicBufferProducer> &surfaceTex);

#ifdef MTK_AOSP_ENHANCEMENT
    void pause();
    void resume(const sp<IGraphicBufferProducer> &surfaceTex);
    int64_t getBufferedDurationUs();
    int64_t getBufferedDurationUs_l();
    status_t releaseAccessunit();
    void releaseAccessunit_l();
#endif
    void queueInputBuffer(const sp<ABuffer> &accessUnit);

    status_t renderOutputBufferAndRelease(size_t index);
    status_t releaseOutputBuffer(size_t index);

protected:
    virtual ~DecoderContext();

    virtual void onMessageReceived(const sp<AMessage> &msg);

private:
    enum {
        kWhatDecoderNotify,
    };

    sp<AMessage> mNotify;
    sp<ALooper> mDecoderLooper;
    sp<MediaCodec> mDecoder;
    //Vector<sp<ABuffer> > mDecoderInputBuffers;
    //Vector<sp<ABuffer> > mDecoderOutputBuffers;
    Vector<sp<MediaCodecBuffer> > mDecoderInputBuffers;
    Vector<sp<MediaCodecBuffer> > mDecoderOutputBuffers;
    
    List<size_t> mDecoderInputBuffersAvailable;
#ifdef MTK_AOSP_ENHANCEMENT
    sp<AMessage> mFormat;
    Mutex mLock;
    bool mIsVideo;
#endif
#ifdef MTK_SEC_VIDEO_PATH_SUPPORT
    int mSecureBuffer;
#endif
    bool mDecoderNotificationPending;

    List<sp<ABuffer> > mAccessUnits;
    bool bErrorReported;

    void onDecoderNotify();
    void scheduleDecoderNotification();
    void queueDecoderInputBuffers();

    void queueOutputBuffer(
            size_t index, int64_t timeUs, const sp<ABuffer> &buffer);
    void notifyErr(void);

    DISALLOW_EVIL_CONSTRUCTORS(DecoderContext);
};

////////////////////////////////////////////////////////////////////////////////

/*
   A "push" audio renderer. The primary function of this renderer is to use
   an AudioTrack in push mode and making sure not to block the event loop
   be ensuring that calls to AudioTrack::write never block. This is done by
   estimating an upper bound of data that can be written to the AudioTrack
   buffer without delay.
*/
struct DirectRenderer::AudioRenderer : public AHandler {
    AudioRenderer(const sp<DecoderContext> &decoderContext);

    void queueInputBuffer(
            size_t index, int64_t timeUs, const sp<ABuffer> &buffer);

#ifdef MTK_AOSP_ENHANCEMENT
    void flush();
#endif
protected:
    virtual ~AudioRenderer();
    virtual void onMessageReceived(const sp<AMessage> &msg);

private:
    enum {
        kWhatPushAudio,
#ifdef MTK_AOSP_ENHANCEMENT
        kWhatFlush,
#endif
    };

    struct BufferInfo {
        size_t mIndex;
        int64_t mTimeUs;
        sp<ABuffer> mBuffer;
    };

    sp<DecoderContext> mDecoderContext;
    sp<AudioTrack> mAudioTrack;

    List<BufferInfo> mInputBuffers;
    bool mPushPending;

    size_t mNumFramesWritten;
#ifdef MTK_AOSP_ENHANCEMENT
    size_t mNumInputBuffer;
    size_t mInputBufferSize;
    size_t mcurrentInfosize;
    bool mNeedTimeStretch;
    //status_t getNumFramesPlayedByAudioTrackCenter(uint32_t *position);
#endif
    void schedulePushIfNecessary();
    void onPushAudio();

    ssize_t writeNonBlocking(const uint8_t *data, size_t size);

    DISALLOW_EVIL_CONSTRUCTORS(AudioRenderer);
};

////////////////////////////////////////////////////////////////////////////////

DirectRenderer::DecoderContext::DecoderContext(const sp<AMessage> &notify)
    : mNotify(notify),
#ifdef MTK_AOSP_ENHANCEMENT
      mFormat(NULL),
      mIsVideo(false),
#endif
      mDecoderNotificationPending(false) {
      bErrorReported = false;
}

DirectRenderer::DecoderContext::~DecoderContext() {
    if (mDecoder != NULL) {
        mDecoder->release();
        mDecoder.clear();

        mDecoderLooper->stop();
        mDecoderLooper.clear();
    }
}

#ifdef MTK_SEC_VIDEO_PATH_SUPPORT
typedef void (*SVP_COPY2SEC_CB_T)(void *src, int length, void *dst);

static
void SVPCopy2SecCallback(void *src, int length, void *dst)
{
    ALOGD("+++SVPCopy2SecCallback");

    uint8_t *srcBuffer = (uint8_t*)src;
    uint8_t *dstBuffer = (uint8_t*)dst;
    const uint64_t dstHandle = (uint64_t)dstBuffer;

    ALOGD("queueCSDInputBuffer, srcBuffer->data()=%p size=%d", srcBuffer, length);
    ALOGD("queueCSDInputBuffer, dstBuffer->data()=%p ", dstBuffer);

    if (gHDCP != NULL)
    {
        gHDCP->svp_copy2sec(srcBuffer, length, dstHandle);
    }
}

#endif // WFD_HDCP_RX_SUPPORT
status_t DirectRenderer::DecoderContext::init(
        const sp<AMessage> &format,
        const sp<IGraphicBufferProducer> &surfaceTex) {
    CHECK(mDecoder == NULL);

    AString mime;
    CHECK(format->findString("mime", &mime));

    mDecoderLooper = new ALooper;
#ifdef MTK_AOSP_ENHANCEMENT
    if (!strncasecmp("video/", mime.c_str(), 6))
    {
         mDecoderLooper->setName("video codec looper");
         mIsVideo = true;
#if defined(MTK_SEC_VIDEO_PATH_SUPPORT) && defined(WFD_HDCP_RX_SUPPORT)
        char val[PROPERTY_VALUE_MAX] = {0};

        if (property_get("media.wfd.closesvp", val, NULL) && (!strcasecmp("true", val) || !strcmp("1", val)))
        {
            mSecureBuffer=0;
        }
        else{
            mSecureBuffer=1;
            mime.append(".secure");
            ALOGE("secure '%s' ", mime.c_str());

            extern void SVP_reg_callback(SVP_COPY2SEC_CB_T pFnCopy2Sec);
            SVP_reg_callback((SVP_COPY2SEC_CB_T)&SVPCopy2SecCallback);
        }
//#else
//        mSecureBuffer=0;
#endif
    }else{
         mDecoderLooper->setName("audio codec looper");
#ifdef MTK_SEC_VIDEO_PATH_SUPPORT
         mSecureBuffer=0;
#endif
    }
#else
    mDecoderLooper->setName("video codec looper");
#endif

#ifdef MTK_AOSP_ENHANCEMENT
    mDecoderLooper->start(
            false /* runOnCallingThread */,
            false /* canCallJava */,
            PRIORITY_AUDIO);
#else
    mDecoderLooper->start(
            false /* runOnCallingThread */,
            false /* canCallJava */,
            PRIORITY_DEFAULT);
#endif
    ALOGE("CreateByType '%s' ", mime.c_str());

    mDecoder = MediaCodec::CreateByType(
            mDecoderLooper, mime.c_str(), false /* encoder */);

    CHECK(mDecoder != NULL);

#ifdef MTK_AOSP_ENHANCEMENT
    if (!strncasecmp("video/", mime.c_str(), 6))
    {
        format->setInt32("vdec-no-record", 1);
     format->setInt32("vdec-lowlatency", 1);
        format->setInt32("wfd-sink", 1);
        ALOGD("add wfd-sink flag");
    }
#endif


    status_t err = mDecoder->configure(
            format,
            surfaceTex == NULL
                ? NULL : new Surface(surfaceTex),
            NULL /* crypto */,
            0 /* flags */);
    CHECK_EQ(err, (status_t)OK);

    err = mDecoder->start();
    CHECK_EQ(err, (status_t)OK);

    err = mDecoder->getInputBuffers(
            &mDecoderInputBuffers);
    CHECK_EQ(err, (status_t)OK);

    err = mDecoder->getOutputBuffers(
            &mDecoderOutputBuffers);
    CHECK_EQ(err, (status_t)OK);

    scheduleDecoderNotification();

#ifdef MTK_AOSP_ENHANCEMENT
    mFormat = format;
#endif
    return OK;
}

#ifdef MTK_AOSP_ENHANCEMENT
void DirectRenderer::DecoderContext::pause() {
    status_t err = mDecoder->flush();
    //CHECK_EQ(err, (status_t)OK);
    ALOGD("flush return %d", err);

    err = mDecoder->release();
//    CHECK_EQ(err, (status_t)OK);
    ALOGD("release return %d", err);
}

void DirectRenderer::DecoderContext::resume(const sp<IGraphicBufferProducer> &surfaceTex) {
    CHECK(mFormat != NULL);

    status_t err = mDecoder->configure(
            mFormat,
            surfaceTex == NULL
            ? NULL : new Surface(surfaceTex),
            NULL /* crypto */,
            0 /* flags */);
    CHECK_EQ(err, (status_t)OK);

    err = mDecoder->start();
    CHECK_EQ(err, (status_t)OK);

    err = mDecoder->getInputBuffers(
            &mDecoderInputBuffers);
    CHECK_EQ(err, (status_t)OK);

    err = mDecoder->getOutputBuffers(
            &mDecoderOutputBuffers);
    CHECK_EQ(err, (status_t)OK);
}
int64_t DirectRenderer::DecoderContext::getBufferedDurationUs() {
    Mutex::Autolock autoLock(mLock);
    return getBufferedDurationUs_l();
}
int64_t DirectRenderer::DecoderContext::getBufferedDurationUs_l() {
    if (mAccessUnits.empty()) {
        return 0;
    }
    int64_t time1 = -1;
    int64_t time2 = -1;
    int64_t durationUs = 0;
    List<sp<ABuffer> >::iterator it = mAccessUnits.begin();
    while (it != mAccessUnits.end()) {
        const sp<ABuffer> &buffer = *it;

        int64_t timeUs;
        if (buffer->meta()->findInt64("timeUs", &timeUs)) {
            if (time1 < 0 || timeUs < time1) {
                time1 = timeUs;
            }

            if (time2 < 0 || timeUs > time2) {
                time2 = timeUs;
            }
        } else {
            // This is a discontinuity, reset everything.
            durationUs += time2 - time1;
            time1 = time2 = -1;
        }

        ++it;
    }

    return durationUs + (time2 - time1);
}
status_t DirectRenderer::DecoderContext::releaseAccessunit() {
    Mutex::Autolock autoLock(mLock);
     releaseAccessunit_l();
     return OK;
}
void DirectRenderer::DecoderContext::releaseAccessunit_l() {
    if(mAccessUnits.empty()){
        ALOGD("mAccessunits already empty");
    }else {
        while(!mAccessUnits.empty()){
            sp<ABuffer> srcBuffer = *mAccessUnits.begin();
            mAccessUnits.erase(mAccessUnits.begin());
        }
        ALOGD("mAccessunits release done");
    }


}

#endif

void DirectRenderer::DecoderContext::queueInputBuffer(
        const sp<ABuffer> &accessUnit) {
    CHECK(mDecoder != NULL);

    mAccessUnits.push_back(accessUnit);
    queueDecoderInputBuffers();
}

status_t DirectRenderer::DecoderContext::renderOutputBufferAndRelease(
        size_t index) {
    return mDecoder->renderOutputBufferAndRelease(index);
}

status_t DirectRenderer::DecoderContext::releaseOutputBuffer(size_t index) {
    return mDecoder->releaseOutputBuffer(index);
}

void DirectRenderer::DecoderContext::queueDecoderInputBuffers() {
    if (mDecoder == NULL) {
        return;
    }

    bool submittedMore = false;

    while (!mAccessUnits.empty()
            && !mDecoderInputBuffersAvailable.empty()) {
        size_t index = *mDecoderInputBuffersAvailable.begin();

        mDecoderInputBuffersAvailable.erase(
                mDecoderInputBuffersAvailable.begin());

        sp<ABuffer> srcBuffer = *mAccessUnits.begin();
        mAccessUnits.erase(mAccessUnits.begin());

#if 0//def WFD_HDCP_RX_SUPPORT
        /* SVP Support */
        if (gHDCP != NULL)
        {

            int32_t StreamCtr=0;
            int64_t InputCtr=0ll;
            int32_t bEnc = false;

            srcBuffer->meta()->findInt32("Encrypted", &bEnc);
            srcBuffer->meta()->findInt32("StreamCtr", &StreamCtr);
            srcBuffer->meta()->findInt64("InputCtr", &InputCtr);
#ifdef MTK_SEC_VIDEO_PATH_SUPPORT
            if (mIsVideo && mSecureBuffer)
            {
                /* Video and SVP enable */
                const sp<MediaCodecBuffer> &dstBuffer =
                    mDecoderInputBuffers.itemAt(index);

                const uint64_t dstHandle = (uint64_t)dstBuffer->data();

                ALOGD("video:SVP, bEnc=%d dstBuffer=%p dstBuffer->data()=%p ", bEnc, dstBuffer.get(), dstBuffer->data());

                if (bEnc)
                {
                    /* Encrypted data */
                    gHDCP->svp_decrypt(
                        srcBuffer->data(), srcBuffer->size(),
                        (uint32_t)StreamCtr, (uint64_t)InputCtr,
                        dstHandle);
                }
                else
                {
                    /* Clear data */
                    gHDCP->svp_copy2sec(
                        srcBuffer->data(), srcBuffer->size(), dstHandle);
                }
            }
            else
#endif
            {
                const sp<MediaCodecBuffer> &dstBuffer =
                    mDecoderInputBuffers.itemAt(index);

                if (mIsVideo){
                    ALOGD("video:Normal, bEnc=%d size=%zu dstBuffer=%p dstBuffer->data()=%p ", bEnc, srcBuffer->size(), dstBuffer.get(), dstBuffer->data());
                }
                if (bEnc)
                {
                    /* Encrypted data */
                    gHDCP->decrypt(
                        srcBuffer->data(), srcBuffer->size(),
                        (uint32_t)StreamCtr, (uint64_t)InputCtr,
                        dstBuffer->data());
                }
                else
                {
                    /* Clear data */
                    memcpy(dstBuffer->data(), srcBuffer->data(), srcBuffer->size());
                }
            }

        }
        else
#endif
        {
        const sp<MediaCodecBuffer> &dstBuffer =
            mDecoderInputBuffers.itemAt(index);


            memcpy(dstBuffer->data(), srcBuffer->data(), srcBuffer->size());
        }
        int64_t timeUs;
        CHECK(srcBuffer->meta()->findInt64("timeUs", &timeUs));

#ifdef MTK_AOSP_ENHANCEMENT
        sp<WfdDebugInfo> debugInfo= defaultWfdDebugInfo();
        int64_t now = ALooper::GetNowUs();
        size_t trackIndex;
        CHECK(mNotify->findSize("trackIndex", &trackIndex));
        debugInfo->addTimeInfoByKey(trackIndex == 0, timeUs, "queueDecIn1", now/1000);
#endif
        char val[PROPERTY_VALUE_MAX] = {0};
        property_get("media.wfd.log", val, 0);
        if(atoi(val)){
            ALOGD("queueInputBuffer %s timeus:%lld size:%d AU=%zu DecoderInBuf=%zu ",mIsVideo == true?"video":"audio", (long long)timeUs,(int)srcBuffer->size(),mAccessUnits.size(), mDecoderInputBuffersAvailable.size());
        }
        status_t err = mDecoder->queueInputBuffer(
                index,
                0 /* offset */,
                srcBuffer->size(),
                timeUs,
                0 /* flags */);
        //CHECK_EQ(err, (status_t)OK);
        if(err != (status_t)OK){
            notifyErr();
            return;
        }
#ifdef MTK_AOSP_ENHANCEMENT
        //now = ALooper::GetNowUs();
        //debugInfo->addTimeInfoByKey(trackIndex == 0, timeUs, "queueDecIn2", now/1000);
#endif

        submittedMore = true;
    }

    if (submittedMore) {
        scheduleDecoderNotification();
    }
}

void DirectRenderer::DecoderContext::onMessageReceived(
        const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatDecoderNotify:
        {
            onDecoderNotify();
            break;
        }

        default:
            TRESPASS();
    }
}

void DirectRenderer::DecoderContext::onDecoderNotify() {
    mDecoderNotificationPending = false;

    for (;;) {
        size_t index;
        status_t err = mDecoder->dequeueInputBuffer(&index);

        if (err == OK) {
            mDecoderInputBuffersAvailable.push_back(index);
        } else if (err == -EAGAIN) {
            break;
        } else {
            //TRESPASS();
            bErrorReported = true;
            notifyErr();
            ALOGD("find error in onDecoderNotify");
            return;
        }
    }

    queueDecoderInputBuffers();

    for (;;) {
        size_t index;
        size_t offset;
        size_t size;
        int64_t timeUs;
        uint32_t flags;
#ifdef MTK_AOSP_ENHANCEMENT
        int64_t now1 = ALooper::GetNowUs();
        size_t trackIndex;
        CHECK(mNotify->findSize("trackIndex", &trackIndex));
#endif
        status_t err = mDecoder->dequeueOutputBuffer(
                &index,
                &offset,
                &size,
                &timeUs,
                &flags);

        if (err == OK) {
#ifdef MTK_AOSP_ENHANCEMENT
            sp<WfdDebugInfo> debugInfo= defaultWfdDebugInfo();
            //int64_t now2 = ALooper::GetNowUs();
            debugInfo->addTimeInfoByKey(trackIndex == 0, timeUs, "dequeueDecOut1", now1/1000);
            //debugInfo->addTimeInfoByKey(trackIndex == 0, timeUs, "dequeueDecOut2", now2/1000);
            // ALOGD("[%s][%lld] dequeueDecOut1", ((trackIndex == 0) ? "video":"audio"), (timeUs / 1000));
#endif
//Add for O ABuffer -> MediaCodecBuffer
            const sp<MediaCodecBuffer> &srcOutBuffer =
            mDecoderOutputBuffers.itemAt(index);
            sp<ABuffer> dstOutBuffer = new ABuffer(srcOutBuffer->capacity());
            memcpy(dstOutBuffer->data(), srcOutBuffer->data(), srcOutBuffer->size());
            dstOutBuffer->setRange(0,srcOutBuffer->size());
//add end for O Abuffer->mediacodecbuffer
            queueOutputBuffer(
                    index, timeUs, /*mDecoderOutputBuffers.itemAt(index)*/dstOutBuffer);
        } else if (err == INFO_OUTPUT_BUFFERS_CHANGED) {
            err = mDecoder->getOutputBuffers(
                    &mDecoderOutputBuffers);
            CHECK_EQ(err, (status_t)OK);
        } else if (err == INFO_FORMAT_CHANGED) {
            // We don't care.
        } else if (err == -EAGAIN) {
            break;
        } else {
            bErrorReported = true;
            notifyErr();
//            TRESPASS();
        }
    }

    scheduleDecoderNotification();
}

void DirectRenderer::DecoderContext::scheduleDecoderNotification() {
    if (mDecoderNotificationPending) {
        return;
    }

    sp<AMessage> notify =
        new AMessage(kWhatDecoderNotify, this);

    mDecoder->requestActivityNotification(notify);
    mDecoderNotificationPending = true;
}

void DirectRenderer::DecoderContext::queueOutputBuffer(
        size_t index, int64_t timeUs, const sp<ABuffer> &buffer) {
    sp<AMessage> msg = mNotify->dup();
    msg->setInt32("what", kWhatOutputBufferReady);
    msg->setSize("index", index);
    msg->setInt64("timeUs", timeUs);
    msg->setBuffer("buffer", buffer);
    msg->post();
}

void DirectRenderer::DecoderContext::notifyErr(void){
    sp<AMessage> msg = mNotify->dup();
    msg->setInt32("what", kWhatError);
    msg->post();
    ALOGW("decodercontext notify error");
}

////////////////////////////////////////////////////////////////////////////////

DirectRenderer::AudioRenderer::AudioRenderer(
        const sp<DecoderContext> &decoderContext)
    : mDecoderContext(decoderContext),
      mPushPending(false),
      mNumFramesWritten(0) {
#ifdef MTK_AOSP_ENHANCEMENT
    mNumInputBuffer = 0;
    mInputBufferSize = 0;
    mcurrentInfosize = 0;
    mNeedTimeStretch = false;
    size_t tempFrameCnt = 0;
    AudioTrack::getMinFrameCount( &tempFrameCnt, AUDIO_STREAM_DEFAULT, 48000.0f);
    ALOGV("AudioRenderer::AudioRenderer audiotrack framecount = %zu", tempFrameCnt);
    mAudioTrack = new AudioTrack(
            AUDIO_STREAM_DEFAULT,
            48000.0f,
            AUDIO_FORMAT_PCM,
            AUDIO_CHANNEL_OUT_STEREO,
            tempFrameCnt);
#else
    mAudioTrack = new AudioTrack(
            AUDIO_STREAM_DEFAULT,
            48000.0f,
            AUDIO_FORMAT_PCM,
            AUDIO_CHANNEL_OUT_STEREO,
            (int)0 /* frameCount */);
#endif

    CHECK_EQ((status_t)OK, mAudioTrack->initCheck());

    mAudioTrack->start();
}

DirectRenderer::AudioRenderer::~AudioRenderer() {
}

void DirectRenderer::AudioRenderer::queueInputBuffer(
        size_t index, int64_t timeUs, const sp<ABuffer> &buffer) {
    BufferInfo info;
    info.mIndex = index;
    info.mTimeUs = timeUs;
    info.mBuffer = buffer;
#ifdef MTK_AOSP_ENHANCEMENT
    //ALOGD("queue input buffer: info index:%d, timeUs:%lld, bufq size:%d",
    //        index, timeUs, mInputBuffers.size());
    if(mInputBufferSize == 0){
        mInputBufferSize = info.mBuffer->size();
    }
#endif

    mInputBuffers.push_back(info);
    schedulePushIfNecessary();
}

#ifdef MTK_AOSP_ENHANCEMENT
void DirectRenderer::AudioRenderer::flush() {
    sp<AMessage> msg = new AMessage(kWhatFlush, this);
    msg->post();
}
#endif
#if 0
status_t DirectRenderer::AudioRenderer::getNumFramesPlayedByAudioTrackCenter(uint32_t *position){
      extern AudioTrackCenter gAudioTrackCenter;
      intptr_t trackId = 0;
      trackId = gAudioTrackCenter.getTrackId(mAudioTrack.get(),NULL);
      if (trackId) {
          int64_t framePlayed = 0;
          status_t CurrentStatus= false;
          CurrentStatus = gAudioTrackCenter.getRealTimePosition(trackId, &framePlayed);
          if (framePlayed > 0xffffffff)
              ALOGW("warning!!!, getRealTimePosition framePlayed = %lld", (long long)framePlayed);
          *position = (uint32_t)framePlayed;
           ALOGV("get played number from AudioTrackCenter.");
          if(CurrentStatus != (status_t)OK){
            return BAD_VALUE;
          }else{
            return (status_t)OK;
          }
      } else {
           ALOGW("trackId == 0,getpositopn from audio track");
           return mAudioTrack->getPosition(position);
      }

}
#endif

void DirectRenderer::AudioRenderer::onMessageReceived(
        const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatPushAudio:
        {
            onPushAudio();
            break;
        }
#ifdef MTK_AOSP_ENHANCEMENT
        case kWhatFlush:
        {
            while (!mInputBuffers.empty()) {
                const BufferInfo &info = *mInputBuffers.begin();

                mDecoderContext->releaseOutputBuffer(info.mIndex);

                mInputBuffers.erase(mInputBuffers.begin());
            }
            break;
        }
#endif
        default:
            break;
    }
}

void DirectRenderer::AudioRenderer::schedulePushIfNecessary() {
    if (mPushPending || mInputBuffers.empty()) {
#ifdef MTK_AOSP_ENHANCEMENT
        //ALOGD("push exit! mPushPending:%d, bufq size: %d",
        //        mPushPending, mInputBuffers.size());
#endif
        return;
    }

    mPushPending = true;

    uint32_t numFramesPlayed;
//#ifdef MTK_AOSP_ENHANCEMENT
#if 0
    CHECK_EQ(getNumFramesPlayedByAudioTrackCenter(&numFramesPlayed),(status_t)OK);
    if (numFramesPlayed > mNumFramesWritten) {
        ALOGW("numFramesPlayed(%d) > mNumFramesWritten(%zu), reset numFramesPlayed",numFramesPlayed, mNumFramesWritten);
        numFramesPlayed = mNumFramesWritten;
     }
#else
    CHECK_EQ(mAudioTrack->getPosition(&numFramesPlayed),
             (status_t)OK);
#endif

    uint32_t numFramesPendingPlayout = mNumFramesWritten - numFramesPlayed;

    // This is how long the audio sink will have data to
    // play back.
    uint32_t sampleRate = mAudioTrack->getSampleRate();
    if(sampleRate == 0){
        ALOGW("samplerate is 0");
        return;
    }
    const float msecsPerFrame = 1000.0f / sampleRate;

    int64_t delayUs =
        msecsPerFrame * numFramesPendingPlayout * 1000ll;

    // Let's give it more data after about half that time
    // has elapsed.
#ifdef MTK_AOSP_ENHANCEMENT
    int32_t numUnWritten = 0;
    int64_t AccessUnitDelayUs = 0;
    int64_t UnWrittenDelayUs = 0;
    int64_t timeperinputbuffer =0;
    AccessUnitDelayUs = mDecoderContext->getBufferedDurationUs();
    ALOGV("AccessUnitDelayUs = %lld us",(long long)AccessUnitDelayUs);
    if(!mInputBuffers.empty()){
        numUnWritten = (mInputBufferSize*(mInputBuffers.size()-1)+mcurrentInfosize)/ mAudioTrack->frameSize();
        UnWrittenDelayUs = msecsPerFrame * numUnWritten * 1000ll;
        timeperinputbuffer = msecsPerFrame*mInputBuffers.size()*1000/(mAudioTrack->frameSize());
        ALOGV("mInputBuffers.size() = %zu,numUnWritten = %d,timeperinputbuffer = %lld",mInputBuffers.size(),numUnWritten,(long long)timeperinputbuffer);
    }
    //numFramesPendingPlayout =numUnWritten +numFramesPendingPlayout;
    int64_t TotaldelayUs = msecsPerFrame * (numFramesPendingPlayout+numUnWritten) * 1000ll + AccessUnitDelayUs;
    ALOGV("numFramesPlayed = %d ,PengdinginAudiotrack %d frames delayUs %lld us,PendinginOutPutbuffer %d frames delayUs = %lld us,TotaldelayUs = %lld us,",
    numFramesPlayed,numFramesPendingPlayout,(long long)delayUs,numUnWritten,(long long)UnWrittenDelayUs,(long long)TotaldelayUs);
    if(AccessUnitDelayUs > 500000){
        ALOGV("audio Accessunit pending too much data,drop some frame for audio latency");
        mDecoderContext->releaseAccessunit();
    }else if (TotaldelayUs > 180000 && delayUs > 80000){
        ALOGV("drop some outputbuffer for latency");
        while (mInputBuffers.size()>1) {
            const BufferInfo &info = *mInputBuffers.begin();
            mDecoderContext->releaseOutputBuffer(info.mIndex);
            mInputBuffers.erase(mInputBuffers.begin());
        }
    }
#endif

    (new AMessage(kWhatPushAudio, this))->post(delayUs / 4);
}

void DirectRenderer::AudioRenderer::onPushAudio() {
    mPushPending = false;

    while (!mInputBuffers.empty()) {
        const BufferInfo &info = *mInputBuffers.begin();

        ssize_t n = writeNonBlocking(
                info.mBuffer->data(), info.mBuffer->size());

        if (n < (ssize_t)info.mBuffer->size()) {
            CHECK_GE(n, 0);

            info.mBuffer->setRange(
                    info.mBuffer->offset() + n, info.mBuffer->size() - n);
#ifdef MTK_AOSP_ENHANCEMENT
            mcurrentInfosize = info.mBuffer->size();
#endif
            break;
        }
#ifdef MTK_AOSP_ENHANCEMENT
        else{
            mcurrentInfosize = 0;
        }
#endif
#ifdef MTK_AOSP_ENHANCEMENT
        sp<WfdDebugInfo> debugInfo= defaultWfdDebugInfo();
        int64_t now = ALooper::GetNowUs();
        debugInfo->addTimeInfoByKey(false, info.mTimeUs, "queueRender1", now/1000);
#endif
        mDecoderContext->releaseOutputBuffer(info.mIndex);

        mInputBuffers.erase(mInputBuffers.begin());
    }

    schedulePushIfNecessary();
}

ssize_t DirectRenderer::AudioRenderer::writeNonBlocking(
        const uint8_t *data, size_t size) {
    uint32_t numFramesPlayed;
    status_t err = mAudioTrack->getPosition(&numFramesPlayed);
    if (err != OK) {
        return err;
    }

    ssize_t numFramesAvailableToWrite =
        mAudioTrack->frameCount() - (mNumFramesWritten - numFramesPlayed);

    size_t numBytesAvailableToWrite =
        numFramesAvailableToWrite * mAudioTrack->frameSize();

    if (size > numBytesAvailableToWrite) {
        size = numBytesAvailableToWrite;
    }

    CHECK_EQ(mAudioTrack->write(data, size), (ssize_t)size);

    size_t numFramesWritten = size / mAudioTrack->frameSize();
    mNumFramesWritten += numFramesWritten;

    return size;
}

////////////////////////////////////////////////////////////////////////////////

DirectRenderer::DirectRenderer(
        const sp<IGraphicBufferProducer> &bufferProducer,
        const sp<AMessage> &notify)
    : mSurfaceTex(bufferProducer),
      mVideoRenderPending(false),
      mNumFramesLate(0),
      mNumFrames(0),
      mNotify(notify){
      ALOGD("add notify");
}

DirectRenderer::~DirectRenderer() {
#ifdef WFD_HDCP_RX_SUPPORT
    if (gHDCP != NULL)
    {
        gHDCP = NULL;
        ALOGD("[HDCP2x Rx] gHDCP has been set NULL");
    }
#endif
}

void DirectRenderer::onMessageReceived(const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatDecoderNotify:
        {
            onDecoderNotify(msg);
            break;
        }

        case kWhatRenderVideo:
        {
            onRenderVideo();
            break;
        }

        case kWhatQueueAccessUnit:
            onQueueAccessUnit(msg);
            break;

        case kWhatSetFormat:
            onSetFormat(msg);
            break;

#ifdef MTK_AOSP_ENHANCEMENT
        case kWhatPause:
        {
            sp<AReplyToken> replyID;
            CHECK(msg->senderAwaitsResponse(&replyID));

            onPause();

            sp<AMessage> response = new AMessage;
            response->postReply(replyID);
            break;
        }
        case kWhatPlay:
        {
            sp<AReplyToken> replyID;
            CHECK(msg->senderAwaitsResponse(&replyID));

            onResume();
            sp<AMessage> response = new AMessage;
            response->postReply(replyID);
            break;
        }
#endif
        default:
            TRESPASS();
    }
}

void DirectRenderer::setHDCP(const sp<IHDCP> &hdcp)
{
#ifdef WFD_HDCP_RX_SUPPORT
    if (hdcp != NULL)
        gHDCP = hdcp;
#else
    UNUSED(hdcp);
#endif
}

void DirectRenderer::setFormat(size_t trackIndex, const sp<AMessage> &format) {
    sp<AMessage> msg = new AMessage(kWhatSetFormat, this);
    msg->setSize("trackIndex", trackIndex);
    msg->setMessage("format", format);
    msg->post();
}

void DirectRenderer::onSetFormat(const sp<AMessage> &msg) {
    size_t trackIndex;
    CHECK(msg->findSize("trackIndex", &trackIndex));

    sp<AMessage> format;
    CHECK(msg->findMessage("format", &format));

    internalSetFormat(trackIndex, format);
}

void DirectRenderer::internalSetFormat(
        size_t trackIndex, const sp<AMessage> &format) {
    CHECK_LT(trackIndex, 2u);

    CHECK(mDecoderContext[trackIndex] == NULL);

    sp<AMessage> notify = new AMessage(kWhatDecoderNotify, this);
    notify->setSize("trackIndex", trackIndex);

    mDecoderContext[trackIndex] = new DecoderContext(notify);
    looper()->registerHandler(mDecoderContext[trackIndex]);

    CHECK_EQ((status_t)OK,
             mDecoderContext[trackIndex]->init(
                 format, trackIndex == 0 ? mSurfaceTex : NULL));

    if (trackIndex == 1) {
        // Audio
        mAudioRenderer = new AudioRenderer(mDecoderContext[1]);
        looper()->registerHandler(mAudioRenderer);
    }
}

void DirectRenderer::queueAccessUnit(
        size_t trackIndex, const sp<ABuffer> &accessUnit) {
//#ifdef MTK_AOSP_ENHANCEMENT
#if 0
    sp<WfdDebugInfo> debugInfo= defaultWfdDebugInfo();
    int64_t now = ALooper::GetNowUs();
    int64_t timeUs;
    CHECK(accessUnit->meta()->findInt64("timeUs", &timeUs));
    debugInfo->addTimeInfoByKey(trackIndex == 0, timeUs, "queueAccu", now/1000);
#endif
    sp<AMessage> msg = new AMessage(kWhatQueueAccessUnit, this);
    msg->setSize("trackIndex", trackIndex);
    msg->setBuffer("accessUnit", accessUnit);
    msg->post();
}

void DirectRenderer::onQueueAccessUnit(const sp<AMessage> &msg) {
    size_t trackIndex;
    CHECK(msg->findSize("trackIndex", &trackIndex));

    sp<ABuffer> accessUnit;
    CHECK(msg->findBuffer("accessUnit", &accessUnit));

    CHECK_LT(trackIndex, 2u);
    CHECK(mDecoderContext[trackIndex] != NULL);

    mDecoderContext[trackIndex]->queueInputBuffer(accessUnit);
}

void DirectRenderer::onDecoderNotify(const sp<AMessage> &msg) {
    size_t trackIndex;
    CHECK(msg->findSize("trackIndex", &trackIndex));

    int32_t what;
    CHECK(msg->findInt32("what", &what));

    switch (what) {
        case DecoderContext::kWhatOutputBufferReady:
        {
            size_t index;
            CHECK(msg->findSize("index", &index));

            int64_t timeUs;
            CHECK(msg->findInt64("timeUs", &timeUs));

            sp<ABuffer> buffer;
            CHECK(msg->findBuffer("buffer", &buffer));

            queueOutputBuffer(trackIndex, index, timeUs, buffer);
            break;
        }
        case DecoderContext::kWhatError:
        {
            notifyErr();
            break;
        }

        default:
            TRESPASS();
    }
}

void DirectRenderer::notifyErr(void)
{
    ALOGD("get error");
    sp<AMessage> msg = mNotify->dup();
    msg->setInt32("what", kWhatError);
    msg->post();
    ALOGW("notify wifidisplaysink error");
}

void DirectRenderer::queueOutputBuffer(
        size_t trackIndex,
        size_t index, int64_t timeUs, const sp<ABuffer> &buffer) {
#ifdef MTK_AOSP_ENHANCEMENT
    sp<WfdDebugInfo> debugInfo= defaultWfdDebugInfo();
    int64_t now = ALooper::GetNowUs();
    debugInfo->addTimeInfoByKey(trackIndex == 0, timeUs, "queueOutput", now/1000);
#endif
    if (trackIndex == 1) {
        // Audio
        mAudioRenderer->queueInputBuffer(index, timeUs, buffer);
        debugLatency(false,timeUs);
        return;
    }

    OutputInfo info;
    info.mIndex = index;
    info.mTimeUs = timeUs;
    info.mBuffer = buffer;
    mVideoOutputBuffers.push_back(info);

    scheduleVideoRenderIfNecessary();
}

void DirectRenderer::scheduleVideoRenderIfNecessary() {
    if (mVideoRenderPending || mVideoOutputBuffers.empty()) {
        return;
    }

    mVideoRenderPending = true;

    int64_t timeUs = (*mVideoOutputBuffers.begin()).mTimeUs;
    int64_t nowUs = ALooper::GetNowUs();

    int64_t delayUs = timeUs - nowUs;

     //video frame display as soon as possible.
     delayUs = 0;

    (new AMessage(kWhatRenderVideo, this))->post(delayUs);
}

void DirectRenderer::onRenderVideo() {
    mVideoRenderPending = false;

    int64_t nowUs = ALooper::GetNowUs();

    while (!mVideoOutputBuffers.empty()) {
        const OutputInfo &info = *mVideoOutputBuffers.begin();

        //video frame display as soon as possible.
        /*if (info.mTimeUs > nowUs) {
            break;
        }*/

        if (info.mTimeUs + 15000ll < nowUs) {
            ++mNumFramesLate;
        }
        ++mNumFrames;

#ifdef MTK_AOSP_ENHANCEMENT
        sp<WfdDebugInfo> debugInfo= defaultWfdDebugInfo();
        int64_t now = ALooper::GetNowUs();
        int64_t timeUs = info.mTimeUs;
        debugInfo->addTimeInfoByKey(true, info.mTimeUs, "queueRender1", now/1000);
#endif
        char val[PROPERTY_VALUE_MAX] = {0};
        property_get("media.wfd.log", val, 0);
        if(atoi(val)){
		    ALOGD("Render, renderOutputBufferAndRelease timeUs=%lld mVideoOutputBuffers=%zu latency_path \n",
				(long long)timeUs, mVideoOutputBuffers.size());
        }
        status_t err =
            mDecoderContext[0]->renderOutputBufferAndRelease(info.mIndex);
        if(err != OK){
            notifyErr();
            return;
        }
        //CHECK_EQ(err, (status_t)OK);

        mVideoOutputBuffers.erase(mVideoOutputBuffers.begin());
#ifdef MTK_AOSP_ENHANCEMENT
        debugLatency(true, timeUs);
#endif
    }

    scheduleVideoRenderIfNecessary();
}

void DirectRenderer::debugLatency(bool mediaType, int64_t timestamp){
//mediatype: true:video,false:audio;
    sp<WfdDebugInfo> debugInfo= defaultWfdDebugInfo();
    //int64_t offset = debugInfo->getTimeInfoByKey(true, timestamp, "Offset");
    int64_t now = ALooper::GetNowUs();
    //int64_t oldtimeUs = timestamp + offset;
    int64_t t1 = debugInfo->getTimeInfoByKey(true, timestamp, "rtprecvin");
    ALOGV("t1 %lld, ts %lld", (long long)t1, (long long)timestamp);
    //int64_t t2 = debugInfo->getTimeInfoByKey(true, oldtimeUs, "atsparserin");
    //int64_t t4 = debugInfo->getTimeInfoByKey(true, oldtimeUs, "apacksourceout");

    //int64_t q1 = debugInfo->getTimeInfoByKey(true, timestamp, "queueAccu");
    int64_t q2 = debugInfo->getTimeInfoByKey(mediaType, timestamp, "queueDecIn1");
    ALOGV("q2 %lld", (long long)q2);
    int64_t q4 = debugInfo->getTimeInfoByKey(mediaType, timestamp, "dequeueDecOut1");

    int64_t q6 = debugInfo->getTimeInfoByKey(mediaType, timestamp, "queueOutput");
    //int64_t q7 = debugInfo->getTimeInfoByKey(mediaType, timestamp, "queueRender1");
    char val[PROPERTY_VALUE_MAX] = {0};
    property_get("media.wfd.log", val, 0);
    if(atoi(val)){
        if(mediaType){
            ALOGD("[video] ts = %lld ms, latency front %lld ms, decoder %lld ms, render %lld ms, total %lldms",
                (long long)timestamp/1000ll,
                (long long)(((t1 > 0)&&(q2 > 0))?(q2-t1):-1),
                (long long)(((q4 > 0)&&(q2 > 0))?(q4-q2):-1),
                (long long)((q4 > 0)?(now/1000ll-q4):-1),
                (long long)((t1 > 0)?(now/1000ll-t1):-1));

        }else{
            ALOGD("[audio] ts = %lld ms, decoder %lld ms, total %lld ms",
                (long long)timestamp/1000ll,
                (long long)(((q4 > 0)&&(q2 > 0))?(q4-q2):-1),
                (long long)(((q6 > 0)&&(t1 > 0))?(q6-t1):-1));
        }
    }
    static int32_t printNum = 0;
    printNum++;
    if((printNum % 100) == 0){
        debugInfo->printDebugInfoByKey(mediaType, timestamp);
    }

    debugInfo->removeTimeInfoByKey(mediaType, timestamp);
}
void DirectRenderer::pause() {
    ALOGD("pause caller pid: %d, our pid: %d",
            IPCThreadState::self()->getCallingPid(), getpid());
    if (getpid() == IPCThreadState::self()->getCallingPid()) {
        onPause();
    } else {
        sp<AMessage> msg = new AMessage(kWhatPause, this);
        sp<AMessage> response;
        msg->postAndAwaitResponse(&response);
    }
}

void DirectRenderer::onPause() {
    ALOGD("kWhatPause >>");
    // flush video renderer
    while (!mVideoOutputBuffers.empty()) {
        const OutputInfo &info = *mVideoOutputBuffers.begin();
        status_t err =
            mDecoderContext[0]->releaseOutputBuffer(info.mIndex);
        //CHECK_EQ(err, (status_t)OK);
        if(err != OK){
            ALOGW("release report error");
        }
        mVideoOutputBuffers.erase(mVideoOutputBuffers.begin());
    }
    // flush audio renderer
    ALOGD("calling audio render %p's flush.", mAudioRenderer.get());
    if (mAudioRenderer != NULL)
        mAudioRenderer->flush();

    // flush decoder
    for (size_t trackIndex = 0; trackIndex < 2; trackIndex ++)
        if (mDecoderContext[trackIndex] != NULL)
            mDecoderContext[trackIndex]->pause();

    ALOGD("<< kWhatPause");
}

void DirectRenderer::resume(const sp<IGraphicBufferProducer> &bufferProducer) {
    mSurfaceTex = bufferProducer;
    ALOGD("resume caller pid: %d, our pid: %d",
            IPCThreadState::self()->getCallingPid(), getpid());
    if (getpid() == IPCThreadState::self()->getCallingPid()) {
        onResume();
    } else {
        sp<AMessage> msg = new AMessage(kWhatPlay, this);
        sp<AMessage> response;
        msg->postAndAwaitResponse(&response);
    }
}

void DirectRenderer::onResume() {
    ALOGD("kWhatPlay >>");
    for (size_t trackIndex = 0; trackIndex < 2; trackIndex ++)
        if (mDecoderContext[trackIndex] != NULL)
            mDecoderContext[trackIndex]->resume(trackIndex == 0 ? mSurfaceTex : NULL);
    ALOGD("<< kWhatPlay");
}

}  // namespace android

