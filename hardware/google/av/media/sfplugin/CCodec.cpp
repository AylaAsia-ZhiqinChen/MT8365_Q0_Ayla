/*
 * Copyright (C) 2017 The Android Open Source Project
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
#define LOG_TAG "CCodec"
#include <utils/Log.h>

#include <sstream>
#include <thread>

#include <C2Config.h>
#include <C2Debug.h>
#include <C2ParamInternal.h>
#include <C2PlatformSupport.h>

#include <android/IGraphicBufferSource.h>
#include <android/IOMXBufferSource.h>
#include <android/hardware/media/omx/1.0/IGraphicBufferSource.h>
#include <android/hardware/media/omx/1.0/IOmx.h>
#include <android-base/stringprintf.h>
#include <cutils/properties.h>
#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>
#include <gui/bufferqueue/1.0/H2BGraphicBufferProducer.h>
#include <media/omx/1.0/WGraphicBufferSource.h>
#include <media/openmax/OMX_IndexExt.h>
#include <media/stagefright/BufferProducerWrapper.h>
#include <media/stagefright/MediaCodecConstants.h>
#include <media/stagefright/PersistentSurface.h>
#include <media/stagefright/codec2/1.0/InputSurface.h>

#include "C2OMXNode.h"
#include "CCodec.h"
#include "CCodecBufferChannel.h"
#include "InputSurfaceWrapper.h"

extern "C" android::PersistentSurface *CreateInputSurface();

namespace android {

using namespace std::chrono_literals;
using ::android::hardware::graphics::bufferqueue::V1_0::utils::H2BGraphicBufferProducer;
using android::base::StringPrintf;
using BGraphicBufferSource = ::android::IGraphicBufferSource;
using ::hardware::google::media::c2::V1_0::IInputSurface;

namespace {

class CCodecWatchdog : public AHandler {
private:
    enum {
        kWhatWatch,
    };
    constexpr static int64_t kWatchIntervalUs = 3300000;  // 3.3 secs

public:
    static sp<CCodecWatchdog> getInstance() {
        static sp<CCodecWatchdog> instance(new CCodecWatchdog);
        static std::once_flag flag;
        // Call Init() only once.
        std::call_once(flag, Init, instance);
        return instance;
    }

    ~CCodecWatchdog() = default;

    void watch(sp<CCodec> codec) {
        bool shouldPost = false;
        {
            Mutexed<std::set<wp<CCodec>>>::Locked codecs(mCodecsToWatch);
            // If a watch message is in flight, piggy-back this instance as well.
            // Otherwise, post a new watch message.
            shouldPost = codecs->empty();
            codecs->emplace(codec);
        }
        if (shouldPost) {
            ALOGV("posting watch message");
            (new AMessage(kWhatWatch, this))->post(kWatchIntervalUs);
        }
    }

protected:
    void onMessageReceived(const sp<AMessage> &msg) {
        switch (msg->what()) {
            case kWhatWatch: {
                Mutexed<std::set<wp<CCodec>>>::Locked codecs(mCodecsToWatch);
                ALOGV("watch for %zu codecs", codecs->size());
                for (auto it = codecs->begin(); it != codecs->end(); ++it) {
                    sp<CCodec> codec = it->promote();
                    if (codec == nullptr) {
                        continue;
                    }
                    codec->initiateReleaseIfStuck();
                }
                codecs->clear();
                break;
            }

            default: {
                TRESPASS("CCodecWatchdog: unrecognized message");
            }
        }
    }

private:
    CCodecWatchdog() : mLooper(new ALooper) {}

    static void Init(const sp<CCodecWatchdog> &thiz) {
        ALOGV("Init");
        thiz->mLooper->setName("CCodecWatchdog");
        thiz->mLooper->registerHandler(thiz);
        thiz->mLooper->start();
    }

    sp<ALooper> mLooper;

    Mutexed<std::set<wp<CCodec>>> mCodecsToWatch;
};

class C2InputSurfaceWrapper : public InputSurfaceWrapper {
public:
    explicit C2InputSurfaceWrapper(
            const std::shared_ptr<Codec2Client::InputSurface> &surface) :
        mSurface(surface) {
    }

    ~C2InputSurfaceWrapper() override = default;

    status_t connect(const std::shared_ptr<Codec2Client::Component> &comp) override {
        if (mConnection != nullptr) {
            return ALREADY_EXISTS;
        }
        return toStatusT(mSurface->connectToComponent(comp, &mConnection),
                         C2_OPERATION_InputSurface_connectToComponent);
    }

    void disconnect() override {
        if (mConnection != nullptr) {
            mConnection->disconnect();
            mConnection = nullptr;
        }
    }

    status_t start() override {
        // InputSurface does not distinguish started state
        return OK;
    }

    status_t signalEndOfInputStream() override {
        C2InputSurfaceEosTuning eos(true);
        std::vector<std::unique_ptr<C2SettingResult>> failures;
        c2_status_t err = mSurface->getConfigurable()->config({&eos}, C2_MAY_BLOCK, &failures);
        if (err != C2_OK) {
            return UNKNOWN_ERROR;
        }
        return OK;
    }

    status_t configure(Config &config __unused) {
        // TODO
        return OK;
    }

private:
    std::shared_ptr<Codec2Client::InputSurface> mSurface;
    std::shared_ptr<Codec2Client::InputSurfaceConnection> mConnection;
};

class GraphicBufferSourceWrapper : public InputSurfaceWrapper {
public:
//    explicit GraphicBufferSourceWrapper(const sp<BGraphicBufferSource> &source) : mSource(source) {}
    GraphicBufferSourceWrapper(
            const sp<BGraphicBufferSource> &source,
            uint32_t width,
            uint32_t height)
        : mSource(source), mWidth(width), mHeight(height) {
        mDataSpace = HAL_DATASPACE_BT709;
    }
    ~GraphicBufferSourceWrapper() override = default;

    status_t connect(const std::shared_ptr<Codec2Client::Component> &comp) override {
        mNode = new C2OMXNode(comp);
        mNode->setFrameSize(mWidth, mHeight);

        // NOTE: we do not use/pass through color aspects from GraphicBufferSource as we
        // communicate that directly to the component.
        mSource->configure(mNode, mDataSpace);
        return OK;
    }

    void disconnect() override {
        if (mNode == nullptr) {
            return;
        }
        sp<IOMXBufferSource> source = mNode->getSource();
        if (source == nullptr) {
            ALOGD("GBSWrapper::disconnect: node is not configured with OMXBufferSource.");
            return;
        }
        source->onOmxIdle();
        source->onOmxLoaded();
        mNode.clear();
    }

    status_t GetStatus(const binder::Status &status) {
        status_t err = OK;
        if (!status.isOk()) {
            err = status.serviceSpecificErrorCode();
            if (err == OK) {
                err = status.transactionError();
                if (err == OK) {
                    // binder status failed, but there is no servie or transaction error
                    err = UNKNOWN_ERROR;
                }
            }
        }
        return err;
    }

    status_t start() override {
        sp<IOMXBufferSource> source = mNode->getSource();
        if (source == nullptr) {
            return NO_INIT;
        }
        constexpr size_t kNumSlots = 16;
        for (size_t i = 0; i < kNumSlots; ++i) {
            source->onInputBufferAdded(i);
        }

        source->onOmxExecuting();
        return OK;
    }

    status_t signalEndOfInputStream() override {
        return GetStatus(mSource->signalEndOfInputStream());
    }

    status_t configure(Config &config) {
        std::stringstream status;
        status_t err = OK;

        // handle each configuration granually, in case we need to handle part of the configuration
        // elsewhere

        // TRICKY: we do not unset frame delay repeating
        if (config.mMinFps > 0 && config.mMinFps != mConfig.mMinFps) {
            int64_t us = 1e6 / config.mMinFps + 0.5;
            status_t res = GetStatus(mSource->setRepeatPreviousFrameDelayUs(us));
            status << " minFps=" << config.mMinFps << " => repeatDelayUs=" << us;
            if (res != OK) {
                status << " (=> " << asString(res) << ")";
                err = res;
            }
            mConfig.mMinFps = config.mMinFps;
        }

        // pts gap
        if (config.mMinAdjustedFps > 0 || config.mFixedAdjustedFps > 0) {
            if (mNode != nullptr) {
                OMX_PARAM_U32TYPE ptrGapParam = {};
                ptrGapParam.nSize = sizeof(OMX_PARAM_U32TYPE);
                ptrGapParam.nU32 = (config.mMinAdjustedFps > 0)
                        ? c2_min(INT32_MAX + 0., 1e6 / config.mMinAdjustedFps + 0.5)
                        : c2_max(0. - INT32_MAX, -1e6 / config.mFixedAdjustedFps - 0.5);
                (void)mNode->setParameter(
                        (OMX_INDEXTYPE)OMX_IndexParamMaxFrameDurationForBitrateControl,
                        &ptrGapParam, sizeof(ptrGapParam));
            }
        }

        // max fps
        // TRICKY: we do not unset max fps to 0 unless using fixed fps
        if ((config.mMaxFps > 0 || (config.mFixedAdjustedFps > 0 && config.mMaxFps == 0))
                && config.mMaxFps != mConfig.mMaxFps) {
            status_t res = GetStatus(mSource->setMaxFps(config.mMaxFps));
            status << " maxFps=" << config.mMaxFps;
            if (res != OK) {
                status << " (=> " << asString(res) << ")";
                err = res;
            }
            mConfig.mMaxFps = config.mMaxFps;
        }

        if (config.mTimeOffsetUs != mConfig.mTimeOffsetUs) {
            status_t res = GetStatus(mSource->setTimeOffsetUs(config.mTimeOffsetUs));
            status << " timeOffset " << config.mTimeOffsetUs << "us";
            if (res != OK) {
                status << " (=> " << asString(res) << ")";
                err = res;
            }
            mConfig.mTimeOffsetUs = config.mTimeOffsetUs;
        }

        if (config.mCaptureFps != mConfig.mCaptureFps || config.mCodedFps != mConfig.mCodedFps) {
            status_t res =
                GetStatus(mSource->setTimeLapseConfig(config.mCodedFps, config.mCaptureFps));
            status << " timeLapse " << config.mCaptureFps << "fps as " << config.mCodedFps << "fps";
            if (res != OK) {
                status << " (=> " << asString(res) << ")";
                err = res;
            }
            mConfig.mCaptureFps = config.mCaptureFps;
            mConfig.mCodedFps = config.mCodedFps;
        }

        if (config.mStartAtUs != mConfig.mStartAtUs
                || (config.mStopped != mConfig.mStopped && !config.mStopped)) {
            status_t res = GetStatus(mSource->setStartTimeUs(config.mStartAtUs));
            status << " start at " << config.mStartAtUs << "us";
            if (res != OK) {
                status << " (=> " << asString(res) << ")";
                err = res;
            }
            mConfig.mStartAtUs = config.mStartAtUs;
            mConfig.mStopped = config.mStopped;
        }

        // suspend-resume
        if (config.mSuspended != mConfig.mSuspended) {
            status_t res = GetStatus(mSource->setSuspend(config.mSuspended, config.mSuspendAtUs));
            status << " " << (config.mSuspended ? "suspend" : "resume")
                    << " at " << config.mSuspendAtUs << "us";
            if (res != OK) {
                status << " (=> " << asString(res) << ")";
                err = res;
            }
            mConfig.mSuspended = config.mSuspended;
            mConfig.mSuspendAtUs = config.mSuspendAtUs;
        }

        if (config.mStopped != mConfig.mStopped && config.mStopped) {
            status_t res = GetStatus(mSource->setStopTimeUs(config.mStopAtUs));
            status << " stop at " << config.mStopAtUs << "us";
            if (res != OK) {
                status << " (=> " << asString(res) << ")";
                err = res;
            } else {
                status << " delayUs";
                res = GetStatus(mSource->getStopTimeOffsetUs(&config.mInputDelayUs));
                if (res != OK) {
                    status << " (=> " << asString(res) << ")";
                } else {
                    status << "=" << config.mInputDelayUs << "us";
                }
                mConfig.mInputDelayUs = config.mInputDelayUs;
            }
            mConfig.mStopAtUs = config.mStopAtUs;
            mConfig.mStopped = config.mStopped;
        }

        // color aspects (android._color-aspects)

        // consumer usage
        ALOGD("ISConfig%s", status.str().c_str());
        return err;
    }

private:
    sp<BGraphicBufferSource> mSource;
    sp<C2OMXNode> mNode;
    uint32_t mWidth;
    uint32_t mHeight;
    Config mConfig;
};

class Codec2ClientInterfaceWrapper : public C2ComponentStore {
    std::shared_ptr<Codec2Client> mClient;

public:
    Codec2ClientInterfaceWrapper(std::shared_ptr<Codec2Client> client)
        : mClient(client) { }

    virtual ~Codec2ClientInterfaceWrapper() = default;

    virtual c2_status_t config_sm(
            const std::vector<C2Param *> &params,
            std::vector<std::unique_ptr<C2SettingResult>> *const failures) {
        return mClient->config(params, C2_MAY_BLOCK, failures);
    };

    virtual c2_status_t copyBuffer(
            std::shared_ptr<C2GraphicBuffer>,
            std::shared_ptr<C2GraphicBuffer>) {
        return C2_OMITTED;
    }

    virtual c2_status_t createComponent(
            C2String, std::shared_ptr<C2Component> *const component) {
        component->reset();
        return C2_OMITTED;
    }

    virtual c2_status_t createInterface(
            C2String, std::shared_ptr<C2ComponentInterface> *const interface) {
        interface->reset();
        return C2_OMITTED;
    }

    virtual c2_status_t query_sm(
            const std::vector<C2Param *> &stackParams,
            const std::vector<C2Param::Index> &heapParamIndices,
            std::vector<std::unique_ptr<C2Param>> *const heapParams) const {
        return mClient->query(stackParams, heapParamIndices, C2_MAY_BLOCK, heapParams);
    }

    virtual c2_status_t querySupportedParams_nb(
            std::vector<std::shared_ptr<C2ParamDescriptor>> *const params) const {
        return mClient->querySupportedParams(params);
    }

    virtual c2_status_t querySupportedValues_sm(
            std::vector<C2FieldSupportedValuesQuery> &fields) const {
        return mClient->querySupportedValues(fields, C2_MAY_BLOCK);
    }

    virtual C2String getName() const {
        return mClient->getName();
    }

    virtual std::shared_ptr<C2ParamReflector> getParamReflector() const {
        return mClient->getParamReflector();
    }

    virtual std::vector<std::shared_ptr<const C2Component::Traits>> listComponents() {
        return std::vector<std::shared_ptr<const C2Component::Traits>>();
    }
};

}  // namespace

// CCodec::ClientListener

struct CCodec::ClientListener : public Codec2Client::Listener {

    explicit ClientListener(const wp<CCodec> &codec) : mCodec(codec) {}

    virtual void onWorkDone(
            const std::weak_ptr<Codec2Client::Component>& component,
            std::list<std::unique_ptr<C2Work>>& workItems,
            size_t numDiscardedInputBuffers) override {
        (void)component;
        sp<CCodec> codec(mCodec.promote());
        if (!codec) {
            return;
        }
        codec->onWorkDone(workItems, numDiscardedInputBuffers);
    }

    virtual void onTripped(
            const std::weak_ptr<Codec2Client::Component>& component,
            const std::vector<std::shared_ptr<C2SettingResult>>& settingResult
            ) override {
        // TODO
        (void)component;
        (void)settingResult;
    }

    virtual void onError(
            const std::weak_ptr<Codec2Client::Component>& component,
            uint32_t errorCode) override {
        // TODO
        (void)component;
        (void)errorCode;
    }

    virtual void onDeath(
            const std::weak_ptr<Codec2Client::Component>& component) override {
        { // Log the death of the component.
            std::shared_ptr<Codec2Client::Component> comp = component.lock();
            if (!comp) {
                ALOGE("Codec2 component died.");
            } else {
                ALOGE("Codec2 component \"%s\" died.", comp->getName().c_str());
            }
        }

        // Report to MediaCodec.
        sp<CCodec> codec(mCodec.promote());
        if (!codec || !codec->mCallback) {
            return;
        }
        codec->mCallback->onError(DEAD_OBJECT, ACTION_CODE_FATAL);
    }

    virtual void onFramesRendered(
            const std::vector<RenderedFrame>& renderedFrames) override {
        // TODO
        (void)renderedFrames;
    }

    virtual void onInputBufferDone(
            const std::shared_ptr<C2Buffer>& buffer) override {
        sp<CCodec> codec(mCodec.promote());
        if (codec) {
            codec->onInputBufferDone(buffer);
        }
    }

private:
    wp<CCodec> mCodec;
};

// CCodecCallbackImpl

class CCodecCallbackImpl : public CCodecCallback {
public:
    explicit CCodecCallbackImpl(CCodec *codec) : mCodec(codec) {}
    ~CCodecCallbackImpl() override = default;

    void onError(status_t err, enum ActionCode actionCode) override {
        mCodec->mCallback->onError(err, actionCode);
    }

    void onOutputFramesRendered(int64_t mediaTimeUs, nsecs_t renderTimeNs) override {
        mCodec->mCallback->onOutputFramesRendered(
                {RenderedFrameInfo(mediaTimeUs, renderTimeNs)});
    }

    void onWorkQueued(bool eos) override {
        mCodec->onWorkQueued(eos);
    }

    void onOutputBuffersChanged() override {
        mCodec->mCallback->onOutputBuffersChanged();
    }

private:
    CCodec *mCodec;
};

// CCodec

CCodec::CCodec()
    : mChannel(new CCodecBufferChannel(std::make_shared<CCodecCallbackImpl>(this))),
      mQueuedWorkCount(0) {
}

CCodec::~CCodec() {
}

std::shared_ptr<BufferChannelBase> CCodec::getBufferChannel() {
    return mChannel;
}

status_t CCodec::tryAndReportOnError(std::function<status_t()> job) {
    status_t err = job();
    if (err != C2_OK) {
        mCallback->onError(err, ACTION_CODE_FATAL);
    }
    return err;
}

void CCodec::initiateAllocateComponent(const sp<AMessage> &msg) {
    auto setAllocating = [this] {
        Mutexed<State>::Locked state(mState);
        if (state->get() != RELEASED) {
            return INVALID_OPERATION;
        }
        state->set(ALLOCATING);
        return OK;
    };
    if (tryAndReportOnError(setAllocating) != OK) {
        return;
    }

    sp<RefBase> codecInfo;
    CHECK(msg->findObject("codecInfo", &codecInfo));
    // For Codec 2.0 components, componentName == codecInfo->getCodecName().

    sp<AMessage> allocMsg(new AMessage(kWhatAllocate, this));
    allocMsg->setObject("codecInfo", codecInfo);
    allocMsg->post();
}

void CCodec::allocate(const sp<MediaCodecInfo> &codecInfo) {
    if (codecInfo == nullptr) {
        mCallback->onError(UNKNOWN_ERROR, ACTION_CODE_FATAL);
        return;
    }
    ALOGD("allocate(%s)", codecInfo->getCodecName());
    mClientListener.reset(new ClientListener(this));

    AString componentName = codecInfo->getCodecName();
    std::shared_ptr<Codec2Client> client;

    // set up preferred component store to access vendor store parameters
    client = Codec2Client::CreateFromService("default", false);
    if (client) {
        ALOGI("setting up '%s' as default (vendor) store", client->getInstanceName().c_str());
        SetPreferredCodec2ComponentStore(
                std::make_shared<Codec2ClientInterfaceWrapper>(client));
    }

    std::shared_ptr<Codec2Client::Component> comp =
            Codec2Client::CreateComponentByName(
            componentName.c_str(),
            mClientListener,
            &client);
    if (!comp) {
        ALOGE("Failed Create component: %s", componentName.c_str());
        Mutexed<State>::Locked state(mState);
        state->set(RELEASED);
        state.unlock();
        mCallback->onError(UNKNOWN_ERROR, ACTION_CODE_FATAL);
        state.lock();
        return;
    }
    ALOGI("Created component [%s]", componentName.c_str());
    mChannel->setComponent(comp);
    auto setAllocated = [this, comp, client] {
        Mutexed<State>::Locked state(mState);
        if (state->get() != ALLOCATING) {
            state->set(RELEASED);
            return UNKNOWN_ERROR;
        }
        state->set(ALLOCATED);
        state->comp = comp;
        mClient = client;
        return OK;
    };
    if (tryAndReportOnError(setAllocated) != OK) {
        return;
    }

    // initialize config here in case setParameters is called prior to configure
    Mutexed<Config>::Locked config(mConfig);
    status_t err = config->initialize(mClient, comp);
    if (err != OK) {
        ALOGW("Failed to initialize configuration support");
        // TODO: report error once we complete implementation.
    }
    config->queryConfiguration(comp);

    mCallback->onComponentAllocated(componentName.c_str());
}

void CCodec::initiateConfigureComponent(const sp<AMessage> &format) {
    auto checkAllocated = [this] {
        Mutexed<State>::Locked state(mState);
        return (state->get() != ALLOCATED) ? UNKNOWN_ERROR : OK;
    };
    if (tryAndReportOnError(checkAllocated) != OK) {
        return;
    }

    sp<AMessage> msg(new AMessage(kWhatConfigure, this));
    msg->setMessage("format", format);
    msg->post();
}

void CCodec::configure(const sp<AMessage> &msg) {
    std::shared_ptr<Codec2Client::Component> comp;
    auto checkAllocated = [this, &comp] {
        Mutexed<State>::Locked state(mState);
        if (state->get() != ALLOCATED) {
            state->set(RELEASED);
            return UNKNOWN_ERROR;
        }
        comp = state->comp;
        return OK;
    };
    if (tryAndReportOnError(checkAllocated) != OK) {
        return;
    }

    auto doConfig = [msg, comp, this]() -> status_t {
        AString mime;
        if (!msg->findString("mime", &mime)) {
            return BAD_VALUE;
        }

        int32_t encoder;
        if (!msg->findInt32("encoder", &encoder)) {
            encoder = false;
        }

        // TODO: read from intf()
        if ((!encoder) != (comp->getName().find("encoder") == std::string::npos)) {
            return UNKNOWN_ERROR;
        }

        int32_t storeMeta;
        if (encoder
                && msg->findInt32("android._input-metadata-buffer-type", &storeMeta)
                && storeMeta != kMetadataBufferTypeInvalid) {
            if (storeMeta != kMetadataBufferTypeANWBuffer) {
                ALOGD("Only ANW buffers are supported for legacy metadata mode");
                return BAD_VALUE;
            }
            mChannel->setMetaMode(CCodecBufferChannel::MODE_ANW);
        }

        sp<RefBase> obj;
        sp<Surface> surface;
        if (msg->findObject("native-window", &obj)) {
            surface = static_cast<Surface *>(obj.get());
            setSurface(surface);
        }

        Mutexed<Config>::Locked config(mConfig);
        config->mUsingSurface = surface != nullptr;

        /*
         * Handle input surface configuration
         */
        if ((config->mDomain & (Config::IS_VIDEO | Config::IS_IMAGE))
                && (config->mDomain & Config::IS_ENCODER)) {
            config->mISConfig.reset(new InputSurfaceWrapper::Config{});
            {
                config->mISConfig->mMinFps = 0;
                int64_t value;
                if (msg->findInt64("repeat-previous-frame-after", &value) && value > 0) {
                    config->mISConfig->mMinFps = 1e6 / value;
                }
                (void)msg->findFloat("max-fps-to-encoder", &config->mISConfig->mMaxFps);
                config->mISConfig->mMinAdjustedFps = 0;
                config->mISConfig->mFixedAdjustedFps = 0;
                if (msg->findInt64("max-pts-gap-to-encoder", &value)) {
                    if (value < 0 && value >= INT32_MIN) {
                        config->mISConfig->mFixedAdjustedFps = -1e6 / value;
                    } else if (value > 0 && value <= INT32_MAX) {
                        config->mISConfig->mMinAdjustedFps = 1e6 / value;
                    }
                }
            }

            {
                double value;
                if (msg->findDouble("time-lapse-fps", &value)) {
                    config->mISConfig->mCaptureFps = value;
                    (void)msg->findAsFloat(KEY_FRAME_RATE, &config->mISConfig->mCodedFps);
                }
            }

            {
                config->mISConfig->mSuspended = false;
                config->mISConfig->mSuspendAtUs = -1;
                int32_t value;
                if (msg->findInt32("create-input-buffers-suspended", &value) && value) {
                    config->mISConfig->mSuspended = true;
                }
            }
        }

        /*
         * Handle desired color format.
         */
        if ((config->mDomain & (Config::IS_VIDEO | Config::IS_IMAGE))) {
            int32_t format = -1;
            if (!msg->findInt32(KEY_COLOR_FORMAT, &format)) {
                /*
                 * Also handle default color format (encoders require color format, so this is only
                 * needed for decoders.
                 */
                if (!(config->mDomain & Config::IS_ENCODER)) {
                    format = (surface == nullptr) ? COLOR_FormatYUV420Planar : COLOR_FormatSurface;
                }
            }

            if (format >= 0) {
                msg->setInt32("android._color-format", format);
            }
        }

        std::vector<std::unique_ptr<C2Param>> configUpdate;
        status_t err = config->getConfigUpdateFromSdkParams(
                comp, msg, Config::IS_CONFIG, C2_DONT_BLOCK, &configUpdate);
        if (err != OK) {
            ALOGW("failed to convert configuration to c2 params");
        }
        err = config->setParameters(comp, configUpdate, C2_DONT_BLOCK);
        if (err != OK) {
            ALOGW("failed to configure c2 params");
            return err;
        }

        std::vector<std::unique_ptr<C2Param>> params;
        C2StreamUsageTuning::input usage(0u, 0u);
        C2StreamMaxBufferSizeInfo::input maxInputSize(0u, 0u);

        std::initializer_list<C2Param::Index> indices {
        };
        c2_status_t c2err = comp->query(
                { &usage, &maxInputSize },
                indices,
                C2_DONT_BLOCK,
                &params);
        if (c2err != C2_OK && c2err != C2_BAD_INDEX) {
            ALOGE("Failed to query component interface: %d", c2err);
            return UNKNOWN_ERROR;
        }
        if (params.size() != indices.size()) {
            ALOGE("Component returns wrong number of params: expected %zu actual %zu",
                    indices.size(), params.size());
            return UNKNOWN_ERROR;
        }
        if (usage && (usage.value & C2MemoryUsage::CPU_READ)) {
            config->mInputFormat->setInt32("using-sw-read-often", true);
        }

        // NOTE: we don't blindly use client specified input size if specified as clients
        // at times specify too small size. Instead, mimic the behavior from OMX, where the
        // client specified size is only used to ask for bigger buffers than component suggested
        // size.
        int32_t clientInputSize = 0;
        bool clientSpecifiedInputSize =
            msg->findInt32(KEY_MAX_INPUT_SIZE, &clientInputSize) && clientInputSize > 0;
        // TEMP: enforce minimum buffer size of 1MB for video decoders
        // and 16K / 4K for audio encoders/decoders
        if (maxInputSize.value == 0) {
            if (config->mDomain & Config::IS_AUDIO) {
                maxInputSize.value = encoder ? 16384 : 4096;
            } else if (!encoder) {
                maxInputSize.value = 1048576u;
            }
        }

        // verify that CSD fits into this size (if defined)
        if ((config->mDomain & Config::IS_DECODER) && maxInputSize.value > 0) {
            sp<ABuffer> csd;
            for (size_t ix = 0; msg->findBuffer(StringPrintf("csd-%zu", ix).c_str(), &csd); ++ix) {
                if (csd && csd->size() > maxInputSize.value) {
                    maxInputSize.value = csd->size();
                }
            }
        }

        // TODO: do this based on component requiring linear allocator for input
        if ((config->mDomain & Config::IS_DECODER) || (config->mDomain & Config::IS_AUDIO)) {
            if (clientSpecifiedInputSize) {
                // Warn that we're overriding client's max input size if necessary.
                if ((uint32_t)clientInputSize < maxInputSize.value) {
                    ALOGD("client requested max input size %d, which is smaller than "
                          "what component recommended (%u); overriding with component "
                          "recommendation.", clientInputSize, maxInputSize.value);
                    ALOGW("This behavior is subject to change. It is recommended that "
                          "app developers double check whether the requested "
                          "max input size is in reasonable range.");
                } else {
                    maxInputSize.value = clientInputSize;
                }
            }
            // Pass max input size on input format to the buffer channel (if supplied by the
            // component or by a default)
            if (maxInputSize.value) {
                config->mInputFormat->setInt32(
                        KEY_MAX_INPUT_SIZE,
                        (int32_t)(c2_min(maxInputSize.value, uint32_t(INT32_MAX))));
            }
        }

        if ((config->mDomain & (Config::IS_VIDEO | Config::IS_IMAGE))) {
            // propagate HDR static info to output format for both encoders and decoders
            // if component supports this info, we will update from component, but only the raw port,
            // so don't propagate if component already filled it in.
            sp<ABuffer> hdrInfo;
            if (msg->findBuffer(KEY_HDR_STATIC_INFO, &hdrInfo)
                    && !config->mOutputFormat->findBuffer(KEY_HDR_STATIC_INFO, &hdrInfo)) {
                config->mOutputFormat->setBuffer(KEY_HDR_STATIC_INFO, hdrInfo);
            }

            // Set desired color format from configuration parameter
            int32_t format;
            if (msg->findInt32("android._color-format", &format)) {
                if (config->mDomain & Config::IS_ENCODER) {
                    config->mInputFormat->setInt32(KEY_COLOR_FORMAT, format);
                } else {
                    config->mOutputFormat->setInt32(KEY_COLOR_FORMAT, format);
                }
            }
        }

        // propagate encoder delay and padding to output format
        if ((config->mDomain & Config::IS_DECODER) && (config->mDomain & Config::IS_AUDIO)) {
            int delay = 0;
            if (msg->findInt32("encoder-delay", &delay)) {
                config->mOutputFormat->setInt32("encoder-delay", delay);
            }
            int padding = 0;
            if (msg->findInt32("encoder-padding", &padding)) {
                config->mOutputFormat->setInt32("encoder-padding", padding);
            }
        }

        // set channel-mask
        if (config->mDomain & Config::IS_AUDIO) {
            int32_t mask;
            if (msg->findInt32(KEY_CHANNEL_MASK, &mask)) {
                if (config->mDomain & Config::IS_ENCODER) {
                    config->mInputFormat->setInt32(KEY_CHANNEL_MASK, mask);
                } else {
                    config->mOutputFormat->setInt32(KEY_CHANNEL_MASK, mask);
                }
            }
        }

        ALOGD("setup formats input: %s and output: %s",
                config->mInputFormat->debugString().c_str(),
                config->mOutputFormat->debugString().c_str());
        return OK;
    };
    if (tryAndReportOnError(doConfig) != OK) {
        return;
    }

    Mutexed<Config>::Locked config(mConfig);

    mCallback->onComponentConfigured(config->mInputFormat, config->mOutputFormat);
}

void CCodec::initiateCreateInputSurface() {
    status_t err = [this] {
        Mutexed<State>::Locked state(mState);
        if (state->get() != ALLOCATED) {
            return UNKNOWN_ERROR;
        }
        // TODO: read it from intf() properly.
        if (state->comp->getName().find("encoder") == std::string::npos) {
            return INVALID_OPERATION;
        }
        return OK;
    }();
    if (err != OK) {
        mCallback->onInputSurfaceCreationFailed(err);
        return;
    }

    (new AMessage(kWhatCreateInputSurface, this))->post();
}

void CCodec::createInputSurface() {
    status_t err;
    sp<IGraphicBufferProducer> bufferProducer;

    sp<AMessage> inputFormat;
    sp<AMessage> outputFormat;
    {
        Mutexed<Config>::Locked config(mConfig);
        inputFormat = config->mInputFormat;
        outputFormat = config->mOutputFormat;
    }

    std::shared_ptr<PersistentSurface> persistentSurface(CreateInputSurface());

    if (persistentSurface->getHidlTarget()) {
        sp<IInputSurface> inputSurface = IInputSurface::castFrom(
                persistentSurface->getHidlTarget());
        if (!inputSurface) {
            ALOGE("Corrupted input surface");
            mCallback->onInputSurfaceCreationFailed(UNKNOWN_ERROR);
            return;
        }
        err = setupInputSurface(std::make_shared<C2InputSurfaceWrapper>(
                std::make_shared<Codec2Client::InputSurface>(inputSurface)));
        bufferProducer = new H2BGraphicBufferProducer(inputSurface);
    } else {
        int32_t width = 0;
        (void)outputFormat->findInt32("width", &width);
        int32_t height = 0;
        (void)outputFormat->findInt32("height", &height);
        err = setupInputSurface(std::make_shared<GraphicBufferSourceWrapper>(
                persistentSurface->getBufferSource(), width, height));
        bufferProducer = persistentSurface->getBufferProducer();
    }

    if (err != OK) {
        ALOGE("Failed to set up input surface: %d", err);
        mCallback->onInputSurfaceCreationFailed(err);
        return;
    }

    mCallback->onInputSurfaceCreated(
            inputFormat,
            outputFormat,
            new BufferProducerWrapper(bufferProducer));
}

status_t CCodec::setupInputSurface(const std::shared_ptr<InputSurfaceWrapper> &surface) {
    Mutexed<Config>::Locked config(mConfig);
    config->mUsingSurface = true;

    // we are now using surface - apply default color aspects to input format - as well as
    // get dataspace
    bool inputFormatChanged = config->updateFormats(config->IS_INPUT);
    ALOGD("input format %s to %s",
            inputFormatChanged ? "changed" : "unchanged",
            config->mInputFormat->debugString().c_str());

    // configure dataspace
    static_assert(sizeof(int32_t) == sizeof(android_dataspace), "dataspace size mismatch");
    android_dataspace dataSpace = HAL_DATASPACE_UNKNOWN;
    (void)config->mInputFormat->findInt32("android._dataspace", (int32_t*)&dataSpace);
    surface->setDataSpace(dataSpace);

    status_t err = mChannel->setInputSurface(surface);
    if (err != OK) {
        // undo input format update
        config->mUsingSurface = false;
        (void)config->updateFormats(config->IS_INPUT);
        return err;
    }
    config->mInputSurface = surface;

    if (config->mISConfig) {
        surface->configure(*config->mISConfig);
    } else {
        ALOGD("ISConfig: no configuration");
    }

    return OK;
}

void CCodec::initiateSetInputSurface(const sp<PersistentSurface> &surface) {
    sp<AMessage> msg = new AMessage(kWhatSetInputSurface, this);
    msg->setObject("surface", surface);
    msg->post();
}

void CCodec::setInputSurface(const sp<PersistentSurface> &surface) {
    sp<AMessage> inputFormat;
    sp<AMessage> outputFormat;
    {
        Mutexed<Config>::Locked config(mConfig);
        inputFormat = config->mInputFormat;
        outputFormat = config->mOutputFormat;
    }
    auto hidlTarget = surface->getHidlTarget();
    if (hidlTarget) {
        sp<IInputSurface> inputSurface =
                IInputSurface::castFrom(hidlTarget);
        if (!inputSurface) {
            ALOGE("Failed to set input surface: Corrupted surface.");
            mCallback->onInputSurfaceDeclined(UNKNOWN_ERROR);
            return;
        }
        status_t err = setupInputSurface(std::make_shared<C2InputSurfaceWrapper>(
                std::make_shared<Codec2Client::InputSurface>(inputSurface)));
        if (err != OK) {
            ALOGE("Failed to set up input surface: %d", err);
            mCallback->onInputSurfaceDeclined(err);
            return;
        }
    } else {
        int32_t width = 0;
        (void)outputFormat->findInt32("width", &width);
        int32_t height = 0;
        (void)outputFormat->findInt32("height", &height);
        status_t err = setupInputSurface(std::make_shared<GraphicBufferSourceWrapper>(
                surface->getBufferSource(), width, height));
        if (err != OK) {
            ALOGE("Failed to set up input surface: %d", err);
            mCallback->onInputSurfaceDeclined(err);
            return;
        }
    }
    mCallback->onInputSurfaceAccepted(inputFormat, outputFormat);
}

void CCodec::initiateStart() {
    auto setStarting = [this] {
        Mutexed<State>::Locked state(mState);
        if (state->get() != ALLOCATED) {
            return UNKNOWN_ERROR;
        }
        state->set(STARTING);
        return OK;
    };
    if (tryAndReportOnError(setStarting) != OK) {
        return;
    }

    (new AMessage(kWhatStart, this))->post();
}

void CCodec::start() {
    std::shared_ptr<Codec2Client::Component> comp;
    auto checkStarting = [this, &comp] {
        Mutexed<State>::Locked state(mState);
        if (state->get() != STARTING) {
            return UNKNOWN_ERROR;
        }
        comp = state->comp;
        return OK;
    };
    if (tryAndReportOnError(checkStarting) != OK) {
        return;
    }

    c2_status_t err = comp->start();
    if (err != C2_OK) {
        mCallback->onError(toStatusT(err, C2_OPERATION_Component_start),
                           ACTION_CODE_FATAL);
        return;
    }
    sp<AMessage> inputFormat;
    sp<AMessage> outputFormat;
    status_t err2 = OK;
    {
        Mutexed<Config>::Locked config(mConfig);
        inputFormat = config->mInputFormat;
        outputFormat = config->mOutputFormat;
        if (config->mInputSurface) {
            err2 = config->mInputSurface->start();
        }
    }
    if (err2 != OK) {
        mCallback->onError(err2, ACTION_CODE_FATAL);
        return;
    }
    err2 = mChannel->start(inputFormat, outputFormat);
    if (err2 != OK) {
        mCallback->onError(err2, ACTION_CODE_FATAL);
        return;
    }

    auto setRunning = [this] {
        Mutexed<State>::Locked state(mState);
        if (state->get() != STARTING) {
            return UNKNOWN_ERROR;
        }
        state->set(RUNNING);
        return OK;
    };
    if (tryAndReportOnError(setRunning) != OK) {
        return;
    }
    mCallback->onStartCompleted();

    (void)mChannel->requestInitialInputBuffers();
}

void CCodec::initiateShutdown(bool keepComponentAllocated) {
    if (keepComponentAllocated) {
        initiateStop();
    } else {
        initiateRelease();
    }
}

void CCodec::initiateStop() {
    {
        Mutexed<State>::Locked state(mState);
        if (state->get() == ALLOCATED
                || state->get()  == RELEASED
                || state->get() == STOPPING
                || state->get() == RELEASING) {
            // We're already stopped, released, or doing it right now.
            state.unlock();
            mCallback->onStopCompleted();
            state.lock();
            return;
        }
        state->set(STOPPING);
    }

    mChannel->stop();
    (new AMessage(kWhatStop, this))->post();
}

void CCodec::stop() {
    std::shared_ptr<Codec2Client::Component> comp;
    {
        Mutexed<State>::Locked state(mState);
        if (state->get() == RELEASING) {
            state.unlock();
            // We're already stopped or release is in progress.
            mCallback->onStopCompleted();
            state.lock();
            return;
        } else if (state->get() != STOPPING) {
            state.unlock();
            mCallback->onError(UNKNOWN_ERROR, ACTION_CODE_FATAL);
            state.lock();
            return;
        }
        comp = state->comp;
    }
    status_t err = comp->stop();
    if (err != C2_OK) {
        // TODO: convert err into status_t
        mCallback->onError(UNKNOWN_ERROR, ACTION_CODE_FATAL);
    }

    {
        Mutexed<Config>::Locked config(mConfig);
        if (config->mInputSurface) {
            config->mInputSurface->disconnect();
            config->mInputSurface = nullptr;
        }
    }
    {
        Mutexed<State>::Locked state(mState);
        if (state->get() == STOPPING) {
            state->set(ALLOCATED);
        }
    }
    mCallback->onStopCompleted();
}

void CCodec::initiateRelease(bool sendCallback /* = true */) {
    bool clearInputSurfaceIfNeeded = false;
    {
        Mutexed<State>::Locked state(mState);
        if (state->get() == RELEASED || state->get() == RELEASING) {
            // We're already released or doing it right now.
            if (sendCallback) {
                state.unlock();
                mCallback->onReleaseCompleted();
                state.lock();
            }
            return;
        }
        if (state->get() == ALLOCATING) {
            state->set(RELEASING);
            // With the altered state allocate() would fail and clean up.
            if (sendCallback) {
                state.unlock();
                mCallback->onReleaseCompleted();
                state.lock();
            }
            return;
        }
        if (state->get() == STARTING
                || state->get() == RUNNING
                || state->get() == STOPPING) {
            // Input surface may have been started, so clean up is needed.
            clearInputSurfaceIfNeeded = true;
        }
        state->set(RELEASING);
    }

    if (clearInputSurfaceIfNeeded) {
        Mutexed<Config>::Locked config(mConfig);
        if (config->mInputSurface) {
            config->mInputSurface->disconnect();
            config->mInputSurface = nullptr;
        }
    }

    mChannel->stop();
    // thiz holds strong ref to this while the thread is running.
    sp<CCodec> thiz(this);
    std::thread([thiz, sendCallback] { thiz->release(sendCallback); }).detach();
}

void CCodec::release(bool sendCallback) {
    std::shared_ptr<Codec2Client::Component> comp;
    {
        Mutexed<State>::Locked state(mState);
        if (state->get() == RELEASED) {
            if (sendCallback) {
                state.unlock();
                mCallback->onReleaseCompleted();
                state.lock();
            }
            return;
        }
        comp = state->comp;
    }
    comp->release();

    {
        Mutexed<State>::Locked state(mState);
        state->set(RELEASED);
        state->comp.reset();
    }
    if (sendCallback) {
        mCallback->onReleaseCompleted();
    }
}

status_t CCodec::setSurface(const sp<Surface> &surface) {
    return mChannel->setSurface(surface);
}

void CCodec::signalFlush() {
    status_t err = [this] {
        Mutexed<State>::Locked state(mState);
        if (state->get() == FLUSHED) {
            return ALREADY_EXISTS;
        }
        if (state->get() != RUNNING) {
            return UNKNOWN_ERROR;
        }
        state->set(FLUSHING);
        return OK;
    }();
    switch (err) {
        case ALREADY_EXISTS:
            mCallback->onFlushCompleted();
            return;
        case OK:
            break;
        default:
            mCallback->onError(err, ACTION_CODE_FATAL);
            return;
    }

    mChannel->stop();
    (new AMessage(kWhatFlush, this))->post();
}

void CCodec::flush() {
    std::shared_ptr<Codec2Client::Component> comp;
    auto checkFlushing = [this, &comp] {
        Mutexed<State>::Locked state(mState);
        if (state->get() != FLUSHING) {
            return UNKNOWN_ERROR;
        }
        comp = state->comp;
        return OK;
    };
    if (tryAndReportOnError(checkFlushing) != OK) {
        return;
    }

    std::list<std::unique_ptr<C2Work>> flushedWork;
    c2_status_t err = comp->flush(C2Component::FLUSH_COMPONENT, &flushedWork);
    {
        Mutexed<std::list<std::unique_ptr<C2Work>>>::Locked queue(mWorkDoneQueue);
        flushedWork.splice(flushedWork.end(), *queue);
    }
    if (err != C2_OK) {
        // TODO: convert err into status_t
        mCallback->onError(UNKNOWN_ERROR, ACTION_CODE_FATAL);
    }

    mChannel->flush(flushedWork);
    subQueuedWorkCount(flushedWork.size());

    {
        Mutexed<State>::Locked state(mState);
        state->set(FLUSHED);
    }
    mCallback->onFlushCompleted();
}

void CCodec::signalResume() {
    auto setResuming = [this] {
        Mutexed<State>::Locked state(mState);
        if (state->get() != FLUSHED) {
            return UNKNOWN_ERROR;
        }
        state->set(RESUMING);
        return OK;
    };
    if (tryAndReportOnError(setResuming) != OK) {
        return;
    }

    (void)mChannel->start(nullptr, nullptr);

    {
        Mutexed<State>::Locked state(mState);
        if (state->get() != RESUMING) {
            state.unlock();
            mCallback->onError(UNKNOWN_ERROR, ACTION_CODE_FATAL);
            state.lock();
            return;
        }
        state->set(RUNNING);
    }

    (void)mChannel->requestInitialInputBuffers();
}

void CCodec::signalSetParameters(const sp<AMessage> &params) {
    setParameters(params);
}

void CCodec::setParameters(const sp<AMessage> &params) {
    std::shared_ptr<Codec2Client::Component> comp;
    auto checkState = [this, &comp] {
        Mutexed<State>::Locked state(mState);
        if (state->get() == RELEASED) {
            return INVALID_OPERATION;
        }
        comp = state->comp;
        return OK;
    };
    if (tryAndReportOnError(checkState) != OK) {
        return;
    }

    Mutexed<Config>::Locked config(mConfig);

    /**
     * Handle input surface parameters
     */
    if ((config->mDomain & (Config::IS_VIDEO | Config::IS_IMAGE))
            && (config->mDomain & Config::IS_ENCODER) && config->mInputSurface && config->mISConfig) {
        (void)params->findInt64("time-offset-us", &config->mISConfig->mTimeOffsetUs);

        if (params->findInt64("skip-frames-before", &config->mISConfig->mStartAtUs)) {
            config->mISConfig->mStopped = false;
        } else if (params->findInt64("stop-time-us", &config->mISConfig->mStopAtUs)) {
            config->mISConfig->mStopped = true;
        }

        int32_t value;
        if (params->findInt32("drop-input-frames", &value)) {
            config->mISConfig->mSuspended = value;
            config->mISConfig->mSuspendAtUs = -1;
            (void)params->findInt64("drop-start-time-us", &config->mISConfig->mSuspendAtUs);
        }

        (void)config->mInputSurface->configure(*config->mISConfig);
        if (config->mISConfig->mStopped) {
            config->mInputFormat->setInt64(
                    "android._stop-time-offset-us", config->mISConfig->mInputDelayUs);
        }
    }

    std::vector<std::unique_ptr<C2Param>> configUpdate;
    (void)config->getConfigUpdateFromSdkParams(
            comp, params, Config::IS_PARAM, C2_MAY_BLOCK, &configUpdate);
    // Prefer to pass parameters to the buffer channel, so they can be synchronized with the frames.
    // Parameter synchronization is not defined when using input surface. For now, route
    // these directly to the component.
    if (config->mInputSurface == nullptr
            && (property_get_bool("debug.stagefright.ccodec_delayed_params", false)
                    || comp->getName().find("c2.android.") == 0)) {
        mChannel->setParameters(configUpdate);
    } else {
        (void)config->setParameters(comp, configUpdate, C2_MAY_BLOCK);
    }
}

void CCodec::signalEndOfInputStream() {
    mCallback->onSignaledInputEOS(mChannel->signalEndOfInputStream());
}

void CCodec::signalRequestIDRFrame() {
    std::shared_ptr<Codec2Client::Component> comp;
    {
        Mutexed<State>::Locked state(mState);
        if (state->get() == RELEASED) {
            ALOGD("no IDR request sent since component is released");
            return;
        }
        comp = state->comp;
    }
    ALOGV("request IDR");
    Mutexed<Config>::Locked config(mConfig);
    std::vector<std::unique_ptr<C2Param>> params;
    params.push_back(
            std::make_unique<C2StreamRequestSyncFrameTuning::output>(0u, true));
    config->setParameters(comp, params, C2_MAY_BLOCK);
}

void CCodec::onWorkDone(std::list<std::unique_ptr<C2Work>> &workItems,
                        size_t numDiscardedInputBuffers) {
    if (!workItems.empty()) {
        {
            Mutexed<std::list<size_t>>::Locked numDiscardedInputBuffersQueue(
                    mNumDiscardedInputBuffersQueue);
            numDiscardedInputBuffersQueue->insert(
                    numDiscardedInputBuffersQueue->end(),
                    workItems.size() - 1, 0);
            numDiscardedInputBuffersQueue->emplace_back(
                    numDiscardedInputBuffers);
        }
        {
            Mutexed<std::list<std::unique_ptr<C2Work>>>::Locked queue(mWorkDoneQueue);
            queue->splice(queue->end(), workItems);
        }
    }
    (new AMessage(kWhatWorkDone, this))->post();
}

void CCodec::onInputBufferDone(const std::shared_ptr<C2Buffer>& buffer) {
    mChannel->onInputBufferDone(buffer);
}

void CCodec::onMessageReceived(const sp<AMessage> &msg) {
    TimePoint now = std::chrono::steady_clock::now();
    CCodecWatchdog::getInstance()->watch(this);
    switch (msg->what()) {
        case kWhatAllocate: {
            // C2ComponentStore::createComponent() should return within 100ms.
            setDeadline(now, 150ms, "allocate");
            sp<RefBase> obj;
            CHECK(msg->findObject("codecInfo", &obj));
            allocate((MediaCodecInfo *)obj.get());
            break;
        }
        case kWhatConfigure: {
            // C2Component::commit_sm() should return within 5ms.
            setDeadline(now, 250ms, "configure");
            sp<AMessage> format;
            CHECK(msg->findMessage("format", &format));
            configure(format);
            break;
        }
        case kWhatStart: {
            // C2Component::start() should return within 500ms.
            setDeadline(now, 550ms, "start");
            mQueuedWorkCount = 0;
            start();
            break;
        }
        case kWhatStop: {
            // C2Component::stop() should return within 500ms.
            setDeadline(now, 550ms, "stop");
            stop();

            mQueuedWorkCount = 0;
            Mutexed<NamedTimePoint>::Locked deadline(mQueueDeadline);
            deadline->set(TimePoint::max(), "none");
            break;
        }
        case kWhatFlush: {
            // C2Component::flush_sm() should return within 5ms.
            setDeadline(now, 50ms, "flush");
            flush();
            break;
        }
        case kWhatCreateInputSurface: {
            // Surface operations may be briefly blocking.
            setDeadline(now, 100ms, "createInputSurface");
            createInputSurface();
            break;
        }
        case kWhatSetInputSurface: {
            // Surface operations may be briefly blocking.
            setDeadline(now, 100ms, "setInputSurface");
            sp<RefBase> obj;
            CHECK(msg->findObject("surface", &obj));
            sp<PersistentSurface> surface(static_cast<PersistentSurface *>(obj.get()));
            setInputSurface(surface);
            break;
        }
        case kWhatWorkDone: {
            std::unique_ptr<C2Work> work;
            size_t numDiscardedInputBuffers;
            bool shouldPost = false;
            {
                Mutexed<std::list<std::unique_ptr<C2Work>>>::Locked queue(mWorkDoneQueue);
                if (queue->empty()) {
                    break;
                }
                work.swap(queue->front());
                queue->pop_front();
                shouldPost = !queue->empty();
            }
            {
                Mutexed<std::list<size_t>>::Locked numDiscardedInputBuffersQueue(
                        mNumDiscardedInputBuffersQueue);
                if (numDiscardedInputBuffersQueue->empty()) {
                    numDiscardedInputBuffers = 0;
                } else {
                    numDiscardedInputBuffers = numDiscardedInputBuffersQueue->front();
                    numDiscardedInputBuffersQueue->pop_front();
                }
            }
            if (shouldPost) {
                (new AMessage(kWhatWorkDone, this))->post();
            }

            if (work->worklets.empty()
                    || !(work->worklets.front()->output.flags & C2FrameData::FLAG_INCOMPLETE)) {
                subQueuedWorkCount(1);
            }
            // handle configuration changes in work done
            Mutexed<Config>::Locked config(mConfig);
            bool changed = false;
            Config::Watcher<C2StreamInitDataInfo::output> initData =
                config->watch<C2StreamInitDataInfo::output>();
            if (!work->worklets.empty()
                    && (work->worklets.front()->output.flags
                            & C2FrameData::FLAG_DISCARD_FRAME) == 0) {

                // copy buffer info to config
                std::vector<std::unique_ptr<C2Param>> updates =
                    std::move(work->worklets.front()->output.configUpdate);
                unsigned stream = 0;
                for (const std::shared_ptr<C2Buffer> &buf : work->worklets.front()->output.buffers) {
                    for (const std::shared_ptr<const C2Info> &info : buf->info()) {
                        // move all info into output-stream #0 domain
                        updates.emplace_back(C2Param::CopyAsStream(*info, true /* output */, stream));
                    }
                    for (const C2ConstGraphicBlock &block : buf->data().graphicBlocks()) {
                        // ALOGV("got output buffer with crop %u,%u+%u,%u and size %u,%u",
                        //      block.crop().left, block.crop().top,
                        //      block.crop().width, block.crop().height,
                        //      block.width(), block.height());
                        updates.emplace_back(new C2StreamCropRectInfo::output(stream, block.crop()));
                        updates.emplace_back(new C2StreamPictureSizeInfo::output(
                                stream, block.width(), block.height()));
                        break; // for now only do the first block
                    }
                    ++stream;
                }

                changed = config->updateConfiguration(updates, config->mOutputDomain);

                // copy standard infos to graphic buffers if not already present (otherwise, we
                // may overwrite the actual intermediate value with a final value)
                stream = 0;
                const static std::vector<C2Param::Index> stdGfxInfos = {
                    C2StreamRotationInfo::output::PARAM_TYPE,
                    C2StreamColorAspectsInfo::output::PARAM_TYPE,
                    C2StreamDataSpaceInfo::output::PARAM_TYPE,
                    C2StreamHdrStaticInfo::output::PARAM_TYPE,
                    C2StreamHdr10PlusInfo::output::PARAM_TYPE,
                    C2StreamPixelAspectRatioInfo::output::PARAM_TYPE,
                    C2StreamSurfaceScalingInfo::output::PARAM_TYPE
                };
                for (const std::shared_ptr<C2Buffer> &buf : work->worklets.front()->output.buffers) {
                    if (buf->data().graphicBlocks().size()) {
                        for (C2Param::Index ix : stdGfxInfos) {
                            if (!buf->hasInfo(ix)) {
                                const C2Param *param =
                                    config->getConfigParameterValue(ix.withStream(stream));
                                if (param) {
                                    std::shared_ptr<C2Param> info(C2Param::Copy(*param));
                                    buf->setInfo(std::static_pointer_cast<C2Info>(info));
                                }
                            }
                        }
                    }
                    ++stream;
                }
            }
            mChannel->onWorkDone(
                    std::move(work), changed ? config->mOutputFormat : nullptr,
                    initData.hasChanged() ? initData.update().get() : nullptr,
                    numDiscardedInputBuffers);
            break;
        }
        case kWhatWatch: {
            // watch message already posted; no-op.
            break;
        }
        default: {
            ALOGE("unrecognized message");
            break;
        }
    }
    setDeadline(TimePoint::max(), 0ms, "none");
}

void CCodec::setDeadline(
        const TimePoint &now,
        const std::chrono::milliseconds &timeout,
        const char *name) {
    int32_t mult = std::max(1, property_get_int32("debug.stagefright.ccodec_timeout_mult", 1));
    Mutexed<NamedTimePoint>::Locked deadline(mDeadline);
    deadline->set(now + (timeout * mult), name);
}

void CCodec::initiateReleaseIfStuck() {
    std::string name;
    bool pendingDeadline = false;
    for (Mutexed<NamedTimePoint> *deadlinePtr : { &mDeadline, &mQueueDeadline, &mEosDeadline }) {
        Mutexed<NamedTimePoint>::Locked deadline(*deadlinePtr);
        if (deadline->get() < std::chrono::steady_clock::now()) {
            name = deadline->getName();
            break;
        }
        if (deadline->get() != TimePoint::max()) {
            pendingDeadline = true;
        }
    }
    if (name.empty()) {
        // We're not stuck.
        if (pendingDeadline) {
            // If we are not stuck yet but still has deadline coming up,
            // post watch message to check back later.
            (new AMessage(kWhatWatch, this))->post();
        }
        return;
    }

    ALOGW("previous call to %s exceeded timeout", name.c_str());
    initiateRelease(false);
    mCallback->onError(UNKNOWN_ERROR, ACTION_CODE_FATAL);
}

void CCodec::onWorkQueued(bool eos) {
    ALOGV("queued work count +1 from %d", mQueuedWorkCount.load());
    int32_t count = ++mQueuedWorkCount;
    if (eos) {
        CCodecWatchdog::getInstance()->watch(this);
        Mutexed<NamedTimePoint>::Locked deadline(mEosDeadline);
        deadline->set(std::chrono::steady_clock::now() + 3s, "eos");
    }
    // TODO: query and use input/pipeline/output delay combined
    if (count >= 4) {
        CCodecWatchdog::getInstance()->watch(this);
        Mutexed<NamedTimePoint>::Locked deadline(mQueueDeadline);
        deadline->set(std::chrono::steady_clock::now() + 3s, "queue");
    }
}

void CCodec::subQueuedWorkCount(uint32_t count) {
    ALOGV("queued work count -%u from %d", count, mQueuedWorkCount.load());
    int32_t currentCount = (mQueuedWorkCount -= count);
    if (currentCount == 0) {
        Mutexed<NamedTimePoint>::Locked deadline(mEosDeadline);
        deadline->set(TimePoint::max(), "none");
    }
    Mutexed<NamedTimePoint>::Locked deadline(mQueueDeadline);
    deadline->set(TimePoint::max(), "none");
}

}  // namespace android

extern "C" android::CodecBase *CreateCodec() {
    return new android::CCodec;
}

extern "C" android::PersistentSurface *CreateInputSurface() {
    // Attempt to create a Codec2's input surface.
    std::shared_ptr<android::Codec2Client::InputSurface> inputSurface =
            android::Codec2Client::CreateInputSurface();
    if (inputSurface) {
        return new android::PersistentSurface(
                inputSurface->getGraphicBufferProducer(),
                static_cast<android::sp<android::hidl::base::V1_0::IBase>>(
                inputSurface->getHalInterface()));
    }

    // Fall back to OMX.
    using namespace android::hardware::media::omx::V1_0;
    using namespace android::hardware::media::omx::V1_0::utils;
    using namespace android::hardware::graphics::bufferqueue::V1_0::utils;
    typedef android::hardware::media::omx::V1_0::Status OmxStatus;
    android::sp<IOmx> omx = IOmx::getService();
    typedef android::hardware::graphics::bufferqueue::V1_0::
            IGraphicBufferProducer HGraphicBufferProducer;
    typedef android::hardware::media::omx::V1_0::
            IGraphicBufferSource HGraphicBufferSource;
    OmxStatus s;
    android::sp<HGraphicBufferProducer> gbp;
    android::sp<HGraphicBufferSource> gbs;
    android::Return<void> transStatus = omx->createInputSurface(
            [&s, &gbp, &gbs](
                    OmxStatus status,
                    const android::sp<HGraphicBufferProducer>& producer,
                    const android::sp<HGraphicBufferSource>& source) {
                s = status;
                gbp = producer;
                gbs = source;
            });
    if (transStatus.isOk() && s == OmxStatus::OK) {
        return new android::PersistentSurface(
                new H2BGraphicBufferProducer(gbp),
                sp<::android::IGraphicBufferSource>(
                    new LWGraphicBufferSource(gbs)));
    }

    return nullptr;
}

