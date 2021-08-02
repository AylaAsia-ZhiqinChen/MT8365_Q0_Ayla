/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "C2SoftAacEnc"
#include <utils/Log.h>

#include <inttypes.h>

#include <C2PlatformSupport.h>
#include <SimpleC2Interface.h>
#include <media/stagefright/foundation/MediaDefs.h>
#include <media/stagefright/foundation/hexdump.h>

#include "C2SoftAacEnc.h"

namespace android {

class C2SoftAacEnc::IntfImpl : public C2InterfaceHelper {
public:
    explicit IntfImpl(const std::shared_ptr<C2ReflectorHelper> &helper)
        : C2InterfaceHelper(helper) {

        setDerivedInstance(this);

        addParameter(
                DefineParam(mInputFormat, C2_NAME_INPUT_STREAM_FORMAT_SETTING)
                .withConstValue(new C2StreamFormatConfig::input(0u, C2FormatAudio))
                .build());

        addParameter(
                DefineParam(mOutputFormat, C2_NAME_OUTPUT_STREAM_FORMAT_SETTING)
                .withConstValue(new C2StreamFormatConfig::output(0u, C2FormatCompressed))
                .build());

        addParameter(
                DefineParam(mInputMediaType, C2_NAME_INPUT_PORT_MIME_SETTING)
                .withConstValue(AllocSharedString<C2PortMimeConfig::input>(
                        MEDIA_MIMETYPE_AUDIO_RAW))
                .build());

        addParameter(
                DefineParam(mOutputMediaType, C2_NAME_OUTPUT_PORT_MIME_SETTING)
                .withConstValue(AllocSharedString<C2PortMimeConfig::output>(
                        MEDIA_MIMETYPE_AUDIO_AAC))
                .build());

        addParameter(
                DefineParam(mSampleRate, C2_NAME_STREAM_SAMPLE_RATE_SETTING)
                .withDefault(new C2StreamSampleRateInfo::input(0u, 44100))
                .withFields({C2F(mSampleRate, value).oneOf({
                    8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000
                })})
                .withSetter((Setter<decltype(*mSampleRate)>::StrictValueWithNoDeps))
                .build());

        addParameter(
                DefineParam(mChannelCount, C2_NAME_STREAM_CHANNEL_COUNT_SETTING)
                .withDefault(new C2StreamChannelCountInfo::input(0u, 1))
                .withFields({C2F(mChannelCount, value).inRange(1, 6)})
                .withSetter(Setter<decltype(*mChannelCount)>::StrictValueWithNoDeps)
                .build());

        addParameter(
                DefineParam(mBitrate, C2_NAME_STREAM_BITRATE_SETTING)
                .withDefault(new C2BitrateTuning::output(0u, 64000))
                .withFields({C2F(mBitrate, value).inRange(8000, 960000)})
                .withSetter(Setter<decltype(*mBitrate)>::NonStrictValueWithNoDeps)
                .build());

        addParameter(
                DefineParam(mInputMaxBufSize, C2_PARAMKEY_INPUT_MAX_BUFFER_SIZE)
                .withDefault(new C2StreamMaxBufferSizeInfo::input(0u, 8192))
                .calculatedAs(MaxBufSizeCalculator, mChannelCount)
                .build());

        addParameter(
                DefineParam(mProfileLevel, C2_PARAMKEY_PROFILE_LEVEL)
                .withDefault(new C2StreamProfileLevelInfo::output(0u,
                        C2Config::PROFILE_AAC_LC, C2Config::LEVEL_UNUSED))
                .withFields({
                    C2F(mProfileLevel, profile).oneOf({
                            C2Config::PROFILE_AAC_LC,
                            C2Config::PROFILE_AAC_HE,
                            C2Config::PROFILE_AAC_HE_PS,
                            C2Config::PROFILE_AAC_LD,
                            C2Config::PROFILE_AAC_ELD}),
                    C2F(mProfileLevel, level).oneOf({
                            C2Config::LEVEL_UNUSED
                    })
                })
                .withSetter(ProfileLevelSetter)
                .build());
    }

    uint32_t getSampleRate() const { return mSampleRate->value; }
    uint32_t getChannelCount() const { return mChannelCount->value; }
    uint32_t getBitrate() const { return mBitrate->value; }
    static C2R ProfileLevelSetter(bool mayBlock, C2P<C2StreamProfileLevelInfo::output> &me) {
        (void)mayBlock;
        (void)me;  // TODO: validate
        return C2R::Ok();
    }

    static C2R MaxBufSizeCalculator(
            bool mayBlock,
            C2P<C2StreamMaxBufferSizeInfo::input> &me,
            const C2P<C2StreamChannelCountInfo::input> &channelCount) {
        (void)mayBlock;
        me.set().value = 1024 * sizeof(short) * channelCount.v.value;
        return C2R::Ok();
    }

private:
    std::shared_ptr<C2StreamFormatConfig::input> mInputFormat;
    std::shared_ptr<C2StreamFormatConfig::output> mOutputFormat;
    std::shared_ptr<C2PortMimeConfig::input> mInputMediaType;
    std::shared_ptr<C2PortMimeConfig::output> mOutputMediaType;
    std::shared_ptr<C2StreamSampleRateInfo::input> mSampleRate;
    std::shared_ptr<C2StreamChannelCountInfo::input> mChannelCount;
    std::shared_ptr<C2BitrateTuning::output> mBitrate;
    std::shared_ptr<C2StreamMaxBufferSizeInfo::input> mInputMaxBufSize;
    std::shared_ptr<C2StreamProfileLevelInfo::output> mProfileLevel;
};

constexpr char COMPONENT_NAME[] = "c2.android.aac.encoder";

C2SoftAacEnc::C2SoftAacEnc(
        const char *name,
        c2_node_id_t id,
        const std::shared_ptr<IntfImpl> &intfImpl)
    : SimpleC2Component(std::make_shared<SimpleInterface<IntfImpl>>(name, id, intfImpl)),
      mIntf(intfImpl),
      mAACEncoder(nullptr),
      mSBRMode(-1),
      mSBRRatio(0),
      mAACProfile(AOT_AAC_LC),
      mNumBytesPerInputFrame(0u),
      mOutBufferSize(0u),
      mSentCodecSpecificData(false),
      mInputSize(0),
      mInputTimeUs(-1ll),
      mSignalledError(false),
      mOutIndex(0u) {
}

C2SoftAacEnc::~C2SoftAacEnc() {
    onReset();
}

c2_status_t C2SoftAacEnc::onInit() {
    status_t err = initEncoder();
    return err == OK ? C2_OK : C2_CORRUPTED;
}

status_t C2SoftAacEnc::initEncoder() {
    if (AACENC_OK != aacEncOpen(&mAACEncoder, 0, 0)) {
        ALOGE("Failed to init AAC encoder");
        return UNKNOWN_ERROR;
    }
    return setAudioParams();
}

c2_status_t C2SoftAacEnc::onStop() {
    mSentCodecSpecificData = false;
    mInputSize = 0u;
    mInputTimeUs = -1ll;
    mSignalledError = false;
    return C2_OK;
}

void C2SoftAacEnc::onReset() {
    (void)onStop();
    aacEncClose(&mAACEncoder);
}

void C2SoftAacEnc::onRelease() {
    // no-op
}

c2_status_t C2SoftAacEnc::onFlush_sm() {
    mSentCodecSpecificData = false;
    mInputSize = 0u;
    return C2_OK;
}

static CHANNEL_MODE getChannelMode(uint32_t nChannels) {
    CHANNEL_MODE chMode = MODE_INVALID;
    switch (nChannels) {
        case 1: chMode = MODE_1; break;
        case 2: chMode = MODE_2; break;
        case 3: chMode = MODE_1_2; break;
        case 4: chMode = MODE_1_2_1; break;
        case 5: chMode = MODE_1_2_2; break;
        case 6: chMode = MODE_1_2_2_1; break;
        default: chMode = MODE_INVALID;
    }
    return chMode;
}

//static AUDIO_OBJECT_TYPE getAOTFromProfile(OMX_U32 profile) {
//    if (profile == OMX_AUDIO_AACObjectLC) {
//        return AOT_AAC_LC;
//    } else if (profile == OMX_AUDIO_AACObjectHE) {
//        return AOT_SBR;
//    } else if (profile == OMX_AUDIO_AACObjectHE_PS) {
//        return AOT_PS;
//    } else if (profile == OMX_AUDIO_AACObjectLD) {
//        return AOT_ER_AAC_LD;
//    } else if (profile == OMX_AUDIO_AACObjectELD) {
//        return AOT_ER_AAC_ELD;
//    } else {
//        ALOGW("Unsupported AAC profile - defaulting to AAC-LC");
//        return AOT_AAC_LC;
//    }
//}

status_t C2SoftAacEnc::setAudioParams() {
    // We call this whenever sample rate, number of channels, bitrate or SBR mode change
    // in reponse to setParameter calls.

    ALOGV("setAudioParams: %u Hz, %u channels, %u bps, %i sbr mode, %i sbr ratio",
         mIntf->getSampleRate(), mIntf->getChannelCount(), mIntf->getBitrate(), mSBRMode, mSBRRatio);

    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_AOT, mAACProfile)) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }

    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_SAMPLERATE, mIntf->getSampleRate())) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }
    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_BITRATE, mIntf->getBitrate())) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }
    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_CHANNELMODE,
            getChannelMode(mIntf->getChannelCount()))) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }
    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_TRANSMUX, TT_MP4_RAW)) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }

    if (mSBRMode != -1 && mAACProfile == AOT_ER_AAC_ELD) {
        if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_SBR_MODE, mSBRMode)) {
            ALOGE("Failed to set AAC encoder parameters");
            return UNKNOWN_ERROR;
        }
    }

    /* SBR ratio parameter configurations:
       0: Default configuration wherein SBR ratio is configured depending on audio object type by
          the FDK.
       1: Downsampled SBR (default for ELD)
       2: Dualrate SBR (default for HE-AAC)
     */
    if (AACENC_OK != aacEncoder_SetParam(mAACEncoder, AACENC_SBR_RATIO, mSBRRatio)) {
        ALOGE("Failed to set AAC encoder parameters");
        return UNKNOWN_ERROR;
    }

    return OK;
}

void C2SoftAacEnc::process(
        const std::unique_ptr<C2Work> &work,
        const std::shared_ptr<C2BlockPool> &pool) {
    // Initialize output work
    work->result = C2_OK;
    work->workletsProcessed = 1u;
    work->worklets.front()->output.flags = work->input.flags;

    if (mSignalledError) {
        return;
    }
    bool eos = (work->input.flags & C2FrameData::FLAG_END_OF_STREAM) != 0;

    uint32_t sampleRate = mIntf->getSampleRate();
    uint32_t channelCount = mIntf->getChannelCount();

    if (!mSentCodecSpecificData) {
        // The very first thing we want to output is the codec specific
        // data.

        if (AACENC_OK != aacEncEncode(mAACEncoder, nullptr, nullptr, nullptr, nullptr)) {
            ALOGE("Unable to initialize encoder for profile / sample-rate / bit-rate / channels");
            mSignalledError = true;
            work->result = C2_CORRUPTED;
            return;
        }

        uint32_t bitrate = mIntf->getBitrate();
        uint32_t actualBitRate = aacEncoder_GetParam(mAACEncoder, AACENC_BITRATE);
        if (bitrate != actualBitRate) {
            ALOGW("Requested bitrate %u unsupported, using %u", bitrate, actualBitRate);
        }

        AACENC_InfoStruct encInfo;
        if (AACENC_OK != aacEncInfo(mAACEncoder, &encInfo)) {
            ALOGE("Failed to get AAC encoder info");
            mSignalledError = true;
            work->result = C2_CORRUPTED;
            return;
        }

        std::unique_ptr<C2StreamCsdInfo::output> csd =
            C2StreamCsdInfo::output::AllocUnique(encInfo.confSize, 0u);
        if (!csd) {
            ALOGE("CSD allocation failed");
            mSignalledError = true;
            work->result = C2_NO_MEMORY;
            return;
        }
        memcpy(csd->m.value, encInfo.confBuf, encInfo.confSize);
        ALOGV("put csd");
#if defined(LOG_NDEBUG) && !LOG_NDEBUG
        hexdump(csd->m.value, csd->flexCount());
#endif
        work->worklets.front()->output.configUpdate.push_back(std::move(csd));

        mOutBufferSize = encInfo.maxOutBufBytes;
        mNumBytesPerInputFrame = encInfo.frameLength * channelCount * sizeof(int16_t);
        mInputTimeUs = work->input.ordinal.timestamp;

        mSentCodecSpecificData = true;
    }

    uint8_t temp[1];
    C2ReadView view = mDummyReadView;
    const uint8_t *data = temp;
    size_t capacity = 0u;
    if (!work->input.buffers.empty()) {
        view = work->input.buffers[0]->data().linearBlocks().front().map().get();
        data = view.data();
        capacity = view.capacity();
    }

    size_t numFrames = (capacity + mInputSize + (eos ? mNumBytesPerInputFrame - 1 : 0))
            / mNumBytesPerInputFrame;
    ALOGV("capacity = %zu; mInputSize = %zu; numFrames = %zu mNumBytesPerInputFrame = %u",
          capacity, mInputSize, numFrames, mNumBytesPerInputFrame);

    std::shared_ptr<C2LinearBlock> block;
    std::shared_ptr<C2Buffer> buffer;
    std::unique_ptr<C2WriteView> wView;
    uint8_t *outPtr = temp;
    size_t outAvailable = 0u;
    uint64_t inputIndex = work->input.ordinal.frameIndex.peeku();

    AACENC_InArgs inargs;
    AACENC_OutArgs outargs;
    memset(&inargs, 0, sizeof(inargs));
    memset(&outargs, 0, sizeof(outargs));
    inargs.numInSamples = capacity / sizeof(int16_t);

    void* inBuffer[]        = { (unsigned char *)data };
    INT   inBufferIds[]     = { IN_AUDIO_DATA };
    INT   inBufferSize[]    = { (INT)capacity };
    INT   inBufferElSize[]  = { sizeof(int16_t) };

    AACENC_BufDesc inBufDesc;
    inBufDesc.numBufs           = sizeof(inBuffer) / sizeof(void*);
    inBufDesc.bufs              = (void**)&inBuffer;
    inBufDesc.bufferIdentifiers = inBufferIds;
    inBufDesc.bufSizes          = inBufferSize;
    inBufDesc.bufElSizes        = inBufferElSize;

    void* outBuffer[]       = { outPtr };
    INT   outBufferIds[]    = { OUT_BITSTREAM_DATA };
    INT   outBufferSize[]   = { 0 };
    INT   outBufferElSize[] = { sizeof(UCHAR) };

    AACENC_BufDesc outBufDesc;
    outBufDesc.numBufs           = sizeof(outBuffer) / sizeof(void*);
    outBufDesc.bufs              = (void**)&outBuffer;
    outBufDesc.bufferIdentifiers = outBufferIds;
    outBufDesc.bufSizes          = outBufferSize;
    outBufDesc.bufElSizes        = outBufferElSize;

    AACENC_ERROR encoderErr = AACENC_OK;

    class FillWork {
    public:
        FillWork(uint32_t flags, C2WorkOrdinalStruct ordinal,
                 const std::shared_ptr<C2Buffer> &buffer)
            : mFlags(flags), mOrdinal(ordinal), mBuffer(buffer) {
        }
        ~FillWork() = default;

        void operator()(const std::unique_ptr<C2Work> &work) {
            work->worklets.front()->output.flags = (C2FrameData::flags_t)mFlags;
            work->worklets.front()->output.buffers.clear();
            work->worklets.front()->output.ordinal = mOrdinal;
            work->workletsProcessed = 1u;
            work->result = C2_OK;
            if (mBuffer) {
                work->worklets.front()->output.buffers.push_back(mBuffer);
            }
            ALOGV("timestamp = %lld, index = %lld, w/%s buffer",
                  mOrdinal.timestamp.peekll(),
                  mOrdinal.frameIndex.peekll(),
                  mBuffer ? "" : "o");
        }

    private:
        const uint32_t mFlags;
        const C2WorkOrdinalStruct mOrdinal;
        const std::shared_ptr<C2Buffer> mBuffer;
    };

    C2WorkOrdinalStruct outOrdinal = work->input.ordinal;

    while (encoderErr == AACENC_OK && inargs.numInSamples > 0) {
        if (numFrames && !block) {
            C2MemoryUsage usage = { C2MemoryUsage::CPU_READ, C2MemoryUsage::CPU_WRITE };
            // TODO: error handling, proper usage, etc.
            c2_status_t err = pool->fetchLinearBlock(mOutBufferSize, usage, &block);
            if (err != C2_OK) {
                ALOGE("fetchLinearBlock failed : err = %d", err);
                work->result = C2_NO_MEMORY;
                return;
            }

            wView.reset(new C2WriteView(block->map().get()));
            outPtr = wView->data();
            outAvailable = wView->size();
            --numFrames;
        }

        memset(&outargs, 0, sizeof(outargs));

        outBuffer[0] = outPtr;
        outBufferSize[0] = outAvailable;

        encoderErr = aacEncEncode(mAACEncoder,
                                  &inBufDesc,
                                  &outBufDesc,
                                  &inargs,
                                  &outargs);

        if (encoderErr == AACENC_OK) {
            if (buffer) {
                outOrdinal.frameIndex = mOutIndex++;
                outOrdinal.timestamp = mInputTimeUs;
                cloneAndSend(
                        inputIndex,
                        work,
                        FillWork(C2FrameData::FLAG_INCOMPLETE, outOrdinal, buffer));
                buffer.reset();
            }

            if (outargs.numOutBytes > 0) {
                mInputSize = 0;
                int consumed = (capacity / sizeof(int16_t)) - inargs.numInSamples
                        + outargs.numInSamples;
                mInputTimeUs = work->input.ordinal.timestamp
                        + (consumed * 1000000ll / channelCount / sampleRate);
                buffer = createLinearBuffer(block, 0, outargs.numOutBytes);
#if defined(LOG_NDEBUG) && !LOG_NDEBUG
                hexdump(outPtr, std::min(outargs.numOutBytes, 256));
#endif
                outPtr = temp;
                outAvailable = 0;
                block.reset();
            } else {
                mInputSize += outargs.numInSamples * sizeof(int16_t);
            }

            if (outargs.numInSamples > 0) {
                inBuffer[0] = (int16_t *)inBuffer[0] + outargs.numInSamples;
                inBufferSize[0] -= outargs.numInSamples * sizeof(int16_t);
                inargs.numInSamples -= outargs.numInSamples;
            }
        }
        ALOGV("encoderErr = %d mInputSize = %zu inargs.numInSamples = %d, mInputTimeUs = %lld",
              encoderErr, mInputSize, inargs.numInSamples, mInputTimeUs.peekll());
    }

    if (eos && inBufferSize[0] > 0) {
        if (numFrames && !block) {
            C2MemoryUsage usage = { C2MemoryUsage::CPU_READ, C2MemoryUsage::CPU_WRITE };
            // TODO: error handling, proper usage, etc.
            c2_status_t err = pool->fetchLinearBlock(mOutBufferSize, usage, &block);
            if (err != C2_OK) {
                ALOGE("fetchLinearBlock failed : err = %d", err);
                work->result = C2_NO_MEMORY;
                return;
            }

            wView.reset(new C2WriteView(block->map().get()));
            outPtr = wView->data();
            outAvailable = wView->size();
            --numFrames;
        }

        memset(&outargs, 0, sizeof(outargs));

        outBuffer[0] = outPtr;
        outBufferSize[0] = outAvailable;

        // Flush
        inargs.numInSamples = -1;

        (void)aacEncEncode(mAACEncoder,
                           &inBufDesc,
                           &outBufDesc,
                           &inargs,
                           &outargs);
    }

    outOrdinal.frameIndex = mOutIndex++;
    outOrdinal.timestamp = mInputTimeUs;
    FillWork((C2FrameData::flags_t)(eos ? C2FrameData::FLAG_END_OF_STREAM : 0),
             outOrdinal, buffer)(work);
}

c2_status_t C2SoftAacEnc::drain(
        uint32_t drainMode,
        const std::shared_ptr<C2BlockPool> &pool) {
    switch (drainMode) {
        case DRAIN_COMPONENT_NO_EOS:
            [[fallthrough]];
        case NO_DRAIN:
            // no-op
            return C2_OK;
        case DRAIN_CHAIN:
            return C2_OMITTED;
        case DRAIN_COMPONENT_WITH_EOS:
            break;
        default:
            return C2_BAD_VALUE;
    }

    (void)pool;
    mSentCodecSpecificData = false;
    mInputSize = 0u;

    // TODO: we don't have any pending work at this time to drain.
    return C2_OK;
}

class C2SoftAacEncFactory : public C2ComponentFactory {
public:
    C2SoftAacEncFactory() : mHelper(std::static_pointer_cast<C2ReflectorHelper>(
            GetCodec2PlatformComponentStore()->getParamReflector())) {
    }

    virtual c2_status_t createComponent(
            c2_node_id_t id,
            std::shared_ptr<C2Component>* const component,
            std::function<void(C2Component*)> deleter) override {
        *component = std::shared_ptr<C2Component>(
                new C2SoftAacEnc(COMPONENT_NAME,
                                 id,
                                 std::make_shared<C2SoftAacEnc::IntfImpl>(mHelper)),
                deleter);
        return C2_OK;
    }

    virtual c2_status_t createInterface(
            c2_node_id_t id, std::shared_ptr<C2ComponentInterface>* const interface,
            std::function<void(C2ComponentInterface*)> deleter) override {
        *interface = std::shared_ptr<C2ComponentInterface>(
                new SimpleInterface<C2SoftAacEnc::IntfImpl>(
                        COMPONENT_NAME, id, std::make_shared<C2SoftAacEnc::IntfImpl>(mHelper)),
                deleter);
        return C2_OK;
    }

    virtual ~C2SoftAacEncFactory() override = default;

private:
    std::shared_ptr<C2ReflectorHelper> mHelper;
};

}  // namespace android

extern "C" ::C2ComponentFactory* CreateCodec2Factory() {
    ALOGV("in %s", __func__);
    return new ::android::C2SoftAacEncFactory();
}

extern "C" void DestroyCodec2Factory(::C2ComponentFactory* factory) {
    ALOGV("in %s", __func__);
    delete factory;
}
