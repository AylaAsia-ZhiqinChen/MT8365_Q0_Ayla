#ifndef ANDROID_GUI_GUIKPIDEBUG_H
#define ANDROID_GUI_GUIKPIDEBUG_H

#include <utils/Singleton.h>
#include <gedkpi/GedKpiWrap_def.h>
#include <binder/IBinder.h>
// ----------------------------------------------------------------------------
namespace android {
// ----------------------------------------------------------------------------

typedef int32_t (*createWrapPrototype)(uint64_t surfaceHandle, uint64_t BBQ_ID);
typedef void (*destroyWrapPrototype)(uint64_t surfaceHandle);
typedef int32_t (*dequeueBufferTagWrapPrototype)(uint64_t surfaceHandle, int32_t fence, intptr_t buffer_addr);
typedef int32_t (*queueBufferTagWrapPrototype)(uint64_t surfaceHandle, int32_t fence, int32_t QedBuffer_length, intptr_t buffer_addr);
typedef int32_t (*acquireBufferTagWrapPrototype)(uint64_t surfaceHandle, intptr_t buffer_addr);
typedef void (*bufferConnectWrapPrototype)(uint64_t surfaceHandle, int32_t BBQ_api_type, int32_t pid);
typedef void (*bufferDisconnectWrapPrototype)(uint64_t surfaceHandle);

class GedKpiDebug : public RefBase {
public:
    GedKpiDebug();
    ~GedKpiDebug();

    void onConstructor(const uint64_t surfaceHandle, const uint64_t bqId);
    void onDestructor(const uint64_t surfaceHandle);
    void onAcquire(const uint64_t surfaceHandle, const sp<GraphicBuffer>& gb);
    void onDequeue(const uint64_t surfaceHandle, sp<GraphicBuffer>& gb, sp<Fence>& fence);
    void onQueue(const uint64_t surfaceHandle, const sp<GraphicBuffer>& gb, const sp<Fence>& fence, const uint32_t numPendingBuffers);
    void onProducerConnect(const uint64_t surfaceHandle, const sp<IBinder>& token, const int api);
    void onProducerDisconnect(const uint64_t surfaceHandle);
private:
    int32_t (*mGedKpiCreateWrap)(uint64_t surfaceHandle, uint64_t BBQ_ID);
    void (*mGedKpiDestroyWrap)(uint64_t surfaceHandle);
    int32_t (*mGedKpiDequeueBufferTagWrap)(uint64_t surfaceHandle, int32_t fence, intptr_t buffer_addr);
    int32_t (*mGedKpiQueueBufferTagWrap)(uint64_t surfaceHandle, int32_t fence, int32_t QedBuffer_length, intptr_t buffer_addr);
    int32_t (*mGedKpiAcquireBufferTagWrap)(uint64_t surfaceHandle, intptr_t buffer_addr);
    void (*mGedKpiBufferConnectWrap)(uint64_t surfaceHandle, int32_t BBQ_api_type, int32_t pid);
    void (*mGedKpiBufferDisconnectWrap)(uint64_t surfaceHandle);
};

// -----------------------------------------------------------------------------
// GuiDebug loader for dl open libgui_debug
class GedKpiModuleLoader : public Singleton<GedKpiModuleLoader> {
public:
    GedKpiModuleLoader();
    ~GedKpiModuleLoader();

    createWrapPrototype GedKpiCreate();
    destroyWrapPrototype GedKpiDestroy();
    dequeueBufferTagWrapPrototype GedKpiDequeue();
    queueBufferTagWrapPrototype GedKpiQueue();
    acquireBufferTagWrapPrototype GedKpiAcquire();
    bufferConnectWrapPrototype GedKpiBufferConnect();
    bufferDisconnectWrapPrototype GedKpiBufferDisconnect();
private:
    //for Ged Kpi
    void* mGedKpiSoHandle;
    int32_t (*mGedKpiCreate)(uint64_t surfaceHandle, uint64_t BBQ_ID);
    void (*mGedKpiDestroy)(uint64_t surfaceHandle);
    int32_t (*mGedKpiDequeueBuffer)(uint64_t surfaceHandle, int32_t fence, intptr_t buffer_addr);
    int32_t (*mGedKpiQueueBuffer)(uint64_t surfaceHandle, int32_t fence, int32_t QedBuffer_length, intptr_t buffer_addr);
    int32_t (*mGedKpiAcquireBuffer)(uint64_t surfaceHandle, intptr_t buffer_addr);
    void (*mGedKpiBufferConnect)(uint64_t surfaceHandle, int32_t BBQ_api_type, int32_t pid);
    void (*mGedKpiBufferDisconnect)(uint64_t surfaceHandle);
};
// ----------------------------------------------------------------------------
}; // namespace android
// ----------------------------------------------------------------------------
#endif // ANDROID_GUI_GUIKPIDEBUG_H
