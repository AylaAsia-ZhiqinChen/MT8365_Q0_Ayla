/*
 * Copyright 2017, The Android Open Source Project
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

#ifndef CCODEC_BUFFER_CHANNEL_H_

#define CCODEC_BUFFER_CHANNEL_H_

#include <map>
#include <memory>
#include <vector>

#include <C2Buffer.h>
#include <C2Component.h>
#include <Codec2Mapper.h>

#include <codec2/hidl/client.h>
#include <media/stagefright/bqhelper/GraphicBufferSource.h>
#include <media/stagefright/codec2/1.0/InputSurface.h>
#include <media/stagefright/foundation/Mutexed.h>
#include <media/stagefright/CodecBase.h>
#include <media/ICrypto.h>

#include "InputSurfaceWrapper.h"

namespace android {

class CCodecCallback {
public:
    virtual ~CCodecCallback() = default;
    virtual void onError(status_t err, enum ActionCode actionCode) = 0;
    virtual void onOutputFramesRendered(int64_t mediaTimeUs, nsecs_t renderTimeNs) = 0;
    virtual void onWorkQueued(bool eos) = 0;
    virtual void onOutputBuffersChanged() = 0;
};

/**
 * BufferChannelBase implementation for CCodec.
 */
class CCodecBufferChannel
    : public BufferChannelBase, public std::enable_shared_from_this<CCodecBufferChannel> {
public:
    explicit CCodecBufferChannel(const std::shared_ptr<CCodecCallback> &callback);
    virtual ~CCodecBufferChannel();

    // BufferChannelBase interface
    virtual status_t queueInputBuffer(const sp<MediaCodecBuffer> &buffer) override;
    virtual status_t queueSecureInputBuffer(
            const sp<MediaCodecBuffer> &buffer,
            bool secure,
            const uint8_t *key,
            const uint8_t *iv,
            CryptoPlugin::Mode mode,
            CryptoPlugin::Pattern pattern,
            const CryptoPlugin::SubSample *subSamples,
            size_t numSubSamples,
            AString *errorDetailMsg) override;
    virtual status_t renderOutputBuffer(
            const sp<MediaCodecBuffer> &buffer, int64_t timestampNs) override;
    virtual status_t discardBuffer(const sp<MediaCodecBuffer> &buffer) override;
    virtual void getInputBufferArray(Vector<sp<MediaCodecBuffer>> *array) override;
    virtual void getOutputBufferArray(Vector<sp<MediaCodecBuffer>> *array) override;

    // Methods below are interface for CCodec to use.

    /**
     * Set the component object for buffer processing.
     */
    void setComponent(const std::shared_ptr<Codec2Client::Component> &component);

    /**
     * Set output graphic surface for rendering.
     */
    status_t setSurface(const sp<Surface> &surface);

    /**
     * Set GraphicBufferSource object from which the component extracts input
     * buffers.
     */
    status_t setInputSurface(const std::shared_ptr<InputSurfaceWrapper> &surface);

    /**
     * Signal EOS to input surface.
     */
    status_t signalEndOfInputStream();

    /**
     * Set parameters.
     */
    status_t setParameters(std::vector<std::unique_ptr<C2Param>> &params);

    /**
     * Start queueing buffers to the component. This object should never queue
     * buffers before this call has completed.
     */
    status_t start(const sp<AMessage> &inputFormat, const sp<AMessage> &outputFormat);

    /**
     * Request initial input buffers to be filled by client.
     */
    status_t requestInitialInputBuffers();

    /**
     * Stop queueing buffers to the component. This object should never queue
     * buffers after this call, until start() is called.
     */
    void stop();

    void flush(const std::list<std::unique_ptr<C2Work>> &flushedWork);

    /**
     * Notify input client about work done.
     *
     * @param workItems   finished work item.
     * @param outputFormat new output format if it has changed, otherwise nullptr
     * @param initData    new init data (CSD) if it has changed, otherwise nullptr
     * @param numDiscardedInputBuffers the number of input buffers that are
     *                    returned for the first time (not previously returned by
     *                    onInputBufferDone()).
     */
    void onWorkDone(
            std::unique_ptr<C2Work> work, const sp<AMessage> &outputFormat,
            const C2StreamInitDataInfo::output *initData,
            size_t numDiscardedInputBuffers);

    /**
     * Make an input buffer available for the client as it is no longer needed
     * by the codec.
     *
     * @param buffer The buffer that becomes unused.
     */
    void onInputBufferDone(const std::shared_ptr<C2Buffer>& buffer);

    enum MetaMode {
        MODE_NONE,
        MODE_ANW,
    };

    void setMetaMode(MetaMode mode);

    // Internal classes
    class Buffers;
    class InputBuffers;
    class OutputBuffers;

private:
    class QueueGuard;

    /**
     * Special mutex-like object with the following properties:
     *
     * - At STOPPED state (initial, or after stop())
     *   - QueueGuard object gets created at STOPPED state, and the client is
     *     supposed to return immediately.
     * - At RUNNING state (after start())
     *   - Each QueueGuard object
     */
    class QueueSync {
    public:
        /**
         * At construction the sync object is in STOPPED state.
         */
        inline QueueSync() {}
        ~QueueSync() = default;

        /**
         * Transition to RUNNING state when stopped. No-op if already in RUNNING
         * state.
         */
        void start();

        /**
         * At RUNNING state, wait until all QueueGuard object created during
         * RUNNING state are destroyed, and then transition to STOPPED state.
         * No-op if already in STOPPED state.
         */
        void stop();

    private:
        Mutex mGuardLock;

        struct Counter {
            inline Counter() : value(-1) {}
            int32_t value;
            Condition cond;
        };
        Mutexed<Counter> mCount;

        friend class CCodecBufferChannel::QueueGuard;
    };

    class QueueGuard {
    public:
        QueueGuard(QueueSync &sync);
        ~QueueGuard();
        inline bool isRunning() { return mRunning; }

    private:
        QueueSync &mSync;
        bool mRunning;
    };

    void feedInputBufferIfAvailable();
    void feedInputBufferIfAvailableInternal();
    status_t queueInputBufferInternal(const sp<MediaCodecBuffer> &buffer);
    bool handleWork(
            std::unique_ptr<C2Work> work, const sp<AMessage> &outputFormat,
            const C2StreamInitDataInfo::output *initData);
    void sendOutputBuffers();

    QueueSync mSync;
    sp<MemoryDealer> mDealer;
    sp<IMemory> mDecryptDestination;
    int32_t mHeapSeqNum;

    std::shared_ptr<Codec2Client::Component> mComponent;
    std::string mComponentName; ///< component name for debugging
    const char *mName; ///< C-string version of component name
    std::shared_ptr<CCodecCallback> mCCodecCallback;
    std::shared_ptr<C2BlockPool> mInputAllocator;
    QueueSync mQueueSync;
    std::vector<std::unique_ptr<C2Param>> mParamsToBeSet;

    Mutexed<std::unique_ptr<InputBuffers>> mInputBuffers;
    Mutexed<std::list<sp<ABuffer>>> mFlushedConfigs;
    Mutexed<std::unique_ptr<OutputBuffers>> mOutputBuffers;

    std::atomic_uint64_t mFrameIndex;
    std::atomic_uint64_t mFirstValidFrameIndex;

    sp<MemoryDealer> makeMemoryDealer(size_t heapSize);

    struct OutputSurface {
        sp<Surface> surface;
        uint32_t generation;
    };
    Mutexed<OutputSurface> mOutputSurface;

    struct BlockPools {
        C2Allocator::id_t inputAllocatorId;
        std::shared_ptr<C2BlockPool> inputPool;
        C2Allocator::id_t outputAllocatorId;
        C2BlockPool::local_id_t outputPoolId;
        std::shared_ptr<Codec2Client::Configurable> outputPoolIntf;
    };
    Mutexed<BlockPools> mBlockPools;

    std::shared_ptr<InputSurfaceWrapper> mInputSurface;

    MetaMode mMetaMode;

    // PipelineCapacity is used in the input buffer gating logic.
    //
    // There are three criteria that need to be met before
    // onInputBufferAvailable() is called:
    // 1. The number of input buffers that have been received by
    //    CCodecBufferChannel but not returned via onWorkDone() or
    //    onInputBufferDone() does not exceed a certain limit. (Let us call this
    //    number the "input" capacity.)
    // 2. The number of work items that have been received by
    //    CCodecBufferChannel whose outputs have not been returned from the
    //    component (by calling onWorkDone()) does not exceed a certain limit.
    //    (Let us call this the "component" capacity.)
    //
    // These three criteria guarantee that a new input buffer that arrives from
    // the invocation of onInputBufferAvailable() will not
    // 1. overload CCodecBufferChannel's input buffers;
    // 2. overload the component; or
    //
    struct PipelineCapacity {
        // The number of available input capacity.
        std::atomic_int input;
        // The number of available component capacity.
        std::atomic_int component;

        PipelineCapacity();
        // Set the values of #input and #component.
        void initialize(int newInput, int newComponent,
                        const char* newName = "<UNKNOWN COMPONENT>",
                        const char* callerTag = nullptr);

        // Return true and decrease #input and #component by one if
        // they are all greater than zero; return false otherwise.
        //
        // callerTag is used for logging only.
        //
        // allocate() is called by CCodecBufferChannel to check whether it can
        // receive another input buffer. If the return value is true,
        // onInputBufferAvailable() and onOutputBufferAvailable() can be called
        // afterwards.
        bool allocate(const char* callerTag = nullptr);

        // Increase #input and #component by one.
        //
        // callerTag is used for logging only.
        //
        // free() is called by CCodecBufferChannel after allocate() returns true
        // but onInputBufferAvailable() cannot be called for any reasons. It
        // essentially undoes an allocate() call.
        void free(const char* callerTag = nullptr);

        // Increase #input by @p numDiscardedInputBuffers.
        //
        // callerTag is used for logging only.
        //
        // freeInputSlots() is called by CCodecBufferChannel when onWorkDone()
        // or onInputBufferDone() is called. @p numDiscardedInputBuffers is
        // provided in onWorkDone(), and is 1 in onInputBufferDone().
        int freeInputSlots(size_t numDiscardedInputBuffers,
                           const char* callerTag = nullptr);

        // Increase #component by one and return the updated value.
        //
        // callerTag is used for logging only.
        //
        // freeComponentSlot() is called by CCodecBufferChannel when
        // onWorkDone() is called.
        int freeComponentSlot(const char* callerTag = nullptr);

    private:
        // Component name. Used for logging.
        const char* mName;
    };
    PipelineCapacity mAvailablePipelineCapacity;

    class ReorderStash {
    public:
        struct Entry {
            inline Entry() : buffer(nullptr), timestamp(0), flags(0), ordinal({0, 0, 0}) {}
            inline Entry(
                    const std::shared_ptr<C2Buffer> &b,
                    int64_t t,
                    int32_t f,
                    const C2WorkOrdinalStruct &o)
                : buffer(b), timestamp(t), flags(f), ordinal(o) {}
            std::shared_ptr<C2Buffer> buffer;
            int64_t timestamp;
            int32_t flags;
            C2WorkOrdinalStruct ordinal;
        };

        ReorderStash();

        void clear();
        void setDepth(uint32_t depth);
        void setKey(C2Config::ordinal_key_t key);
        bool pop(Entry *entry);
        void emplace(
                const std::shared_ptr<C2Buffer> &buffer,
                int64_t timestamp,
                int32_t flags,
                const C2WorkOrdinalStruct &ordinal);
        void defer(const Entry &entry);
        bool hasPending() const;

    private:
        std::list<Entry> mPending;
        std::list<Entry> mStash;
        uint32_t mDepth;
        C2Config::ordinal_key_t mKey;

        bool less(const C2WorkOrdinalStruct &o1, const C2WorkOrdinalStruct &o2);
    };
    Mutexed<ReorderStash> mReorderStash;

    std::atomic_bool mInputMetEos;

    inline bool hasCryptoOrDescrambler() {
        return mCrypto != nullptr || mDescrambler != nullptr;
    }
};

// Conversion of a c2_status_t value to a status_t value may depend on the
// operation that returns the c2_status_t value.
enum c2_operation_t {
    C2_OPERATION_NONE,
    C2_OPERATION_Component_connectToOmxInputSurface,
    C2_OPERATION_Component_createBlockPool,
    C2_OPERATION_Component_destroyBlockPool,
    C2_OPERATION_Component_disconnectFromInputSurface,
    C2_OPERATION_Component_drain,
    C2_OPERATION_Component_flush,
    C2_OPERATION_Component_queue,
    C2_OPERATION_Component_release,
    C2_OPERATION_Component_reset,
    C2_OPERATION_Component_setOutputSurface,
    C2_OPERATION_Component_start,
    C2_OPERATION_Component_stop,
    C2_OPERATION_ComponentStore_copyBuffer,
    C2_OPERATION_ComponentStore_createComponent,
    C2_OPERATION_ComponentStore_createInputSurface,
    C2_OPERATION_ComponentStore_createInterface,
    C2_OPERATION_Configurable_config,
    C2_OPERATION_Configurable_query,
    C2_OPERATION_Configurable_querySupportedParams,
    C2_OPERATION_Configurable_querySupportedValues,
    C2_OPERATION_InputSurface_connectToComponent,
    C2_OPERATION_InputSurfaceConnection_disconnect,
};

status_t toStatusT(c2_status_t c2s, c2_operation_t c2op = C2_OPERATION_NONE);

}  // namespace android

#endif  // CCODEC_BUFFER_CHANNEL_H_
