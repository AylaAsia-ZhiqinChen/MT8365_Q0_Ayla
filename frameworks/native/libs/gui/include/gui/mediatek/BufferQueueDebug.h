#ifndef ANDROID_GUI_BUFFERQUEUEDEBUG_H
#define ANDROID_GUI_BUFFERQUEUEDEBUG_H

#include <gui/IGraphicBufferConsumer.h>
#include <utils/RefBase.h>
#include <gui_debug/BufferQueueDumpAPI.h>

namespace android {
// ----------------------------------------------------------------------------
class String8;
class BufferQueueCore;
struct BufferQueueDebug : public RefBase {
    // debug target BQ info
    wp<BufferQueueCore> mBq;
    int32_t mId;
    int mConnectedApi;
    String8 mConsumerName;
    String8 mMiniConusmerName;

    // process info
    int32_t mPid;
    int32_t mProducerPid;
    int32_t mConsumerPid;
    String8 mProducerProcName;
    String8 mConsumerProcName;

    // if debug line enabled
    bool mLine;
    // debug line count
    uint32_t mLineCnt;

    BufferQueueDumpAPI* mDump;

    // generate path for file dump
    static void getDumpFileName(String8& fileName, const String8& name);

    status_t drawDebugLineToGraphicBuffer(
        const sp<GraphicBuffer>& gb, uint32_t cnt, uint8_t val = 0xff);

    // whether dump mechanism of general buffer queue is enabled or not
    bool mGeneralDump;
    // used to notify ged about queue/acquire events for fast DVFS

    BufferQueueDebug();
    virtual ~BufferQueueDebug();
    // BufferQueueCore part
    void onConstructor(wp<BufferQueueCore> bq,
        const String8& consumerName,
        const uint64_t& bqId);
    void onDestructor();
    void onDump(String8 &result, const String8& prefix) const;
    void onFreeBufferLocked(const int slot);
    // BufferQueueConsumer part
    void onConsumerDisconnectHead();
    void onConsumerDisconnectTail();
    void onSetConsumerName(const String8& consumerName);
    void onAcquire(
            const int buf,
            const sp<GraphicBuffer>& gb,
            const sp<Fence>& fence,
            const int64_t& timestamp,
            const uint32_t& transform,
            const BufferItem* const buffer);
    void onRelease(const int buf);
    void onConsumerConnect(
            const sp<IConsumerListener>& consumerListener,
            const bool controlledByApp);
    // BufferQueueProducer part
    void setIonInfo(const sp<GraphicBuffer>& gb);
    void onDequeue(sp<GraphicBuffer>& gb, sp<Fence>& fence);
    void onQueue(const sp<GraphicBuffer>& gb, const sp<Fence>& fence);
    void onProducerConnect(
            const sp<IBinder>& token,
            const int api,
            bool producerControlledByApp);
    void onProducerDisconnect();
    mutable bool mDebugLog;
};
status_t getProcessName(int pid, String8& name);

// -----------------------------------------------------------------------------
// GuiDebug loader for dl open libgui_debug
class GuiDebugModuleLoader : public RefBase {
public:
    GuiDebugModuleLoader();
    ~GuiDebugModuleLoader();
    BufferQueueDumpAPI* CreateBQDumpInstance();
    static GuiDebugModuleLoader* getInstance() {
        static Mutex sLock;
        Mutex::Autolock _l(sLock);
        static GuiDebugModuleLoader sIntance;
        return &sIntance;
    }
private:
    // for buffer dump
    void* mBQDumpSoHandle;
    BufferQueueDumpAPI* (*mCreateBQDumpInstancePtr)();
};

// ----------------------------------------------------------------------------
}; // namespace android
#endif // ANDROID_GUI_BUFFERQUEUEDEBUG_H
