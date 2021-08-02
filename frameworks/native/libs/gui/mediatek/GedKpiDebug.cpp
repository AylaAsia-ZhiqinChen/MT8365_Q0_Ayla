//#define LOG_NDEBUG 0
//#define MTK_LOG_ENABLE 1
#if defined(MTK_GEDKPI_PRODUCER) || defined(MTK_GEDKPI_CONSUMER)
#include <cmath>
#include <dlfcn.h>

#include <cutils/properties.h>
#include <log/log.h>
#include <binder/IPCThreadState.h>

#include <gui/BufferQueueCore.h>

#include <gui/mediatek/GedKpiDebug.h>

#include <gedkpi/GedKpiWrap_def.h>

namespace android {

GedKpiDebug::GedKpiDebug() {
    mGedKpiCreateWrap = GedKpiModuleLoader::getInstance().GedKpiCreate();
    mGedKpiDestroyWrap = GedKpiModuleLoader::getInstance().GedKpiDestroy();
    mGedKpiDequeueBufferTagWrap = GedKpiModuleLoader::getInstance().GedKpiDequeue();
    mGedKpiQueueBufferTagWrap = GedKpiModuleLoader::getInstance().GedKpiQueue();
    mGedKpiAcquireBufferTagWrap = GedKpiModuleLoader::getInstance().GedKpiAcquire();
    mGedKpiBufferConnectWrap = GedKpiModuleLoader::getInstance().GedKpiBufferConnect();
    mGedKpiBufferDisconnectWrap = GedKpiModuleLoader::getInstance().GedKpiBufferDisconnect();
}

GedKpiDebug::~GedKpiDebug() {
}

void GedKpiDebug::onConstructor(const uint64_t surfaceHandle, const uint64_t bqId) {
    if (mGedKpiCreateWrap) {
        const int32_t err  = mGedKpiCreateWrap(surfaceHandle, bqId);
        if (err != GED_OK) {
            ALOGE("[%s]: an error ged handle", __func__);
        }
    } else {
        ALOGE("[%s]: finding createWrapPrototype() failed", __func__);
    }
}

void GedKpiDebug::onDestructor(const uint64_t surfaceHandle) {
    if (mGedKpiDestroyWrap) {
        mGedKpiDestroyWrap(surfaceHandle);
    } else {
        ALOGE("[%s]: finding destroyWrapPrototype() failed", __func__);
    }
}

void GedKpiDebug::onAcquire(const uint64_t surfaceHandle, const sp<GraphicBuffer>& gb) {
    if (gb != nullptr) {
        if (mGedKpiAcquireBufferTagWrap) {
            const int32_t err = mGedKpiAcquireBufferTagWrap(surfaceHandle, reinterpret_cast<intptr_t>(gb->handle));
            if (err != GED_OK) {
                ALOGE("[%s]: ged acquire fail", __func__);
            }
        } else {
            ALOGE("[%s]: finding acquireBufferTagWrapPrototype() failed", __func__);
        }
    } else {
        ALOGE("[%s]: GraphicBuffer is null", __func__);
    }
}

void GedKpiDebug::onDequeue(const uint64_t surfaceHandle, sp<GraphicBuffer>& gb, sp<Fence>& fence) {
    if (gb != nullptr) {
        const int32_t dupFenceFd = fence->isValid() ? fence->dup() : -1;
        if (mGedKpiDequeueBufferTagWrap) {
            const int32_t err = mGedKpiDequeueBufferTagWrap(
                surfaceHandle, dupFenceFd, reinterpret_cast<intptr_t>(gb->handle));
            if (err != GED_OK) {
                ALOGE("[%s]: ged dequeue fail", __func__);
            }
        } else {
            ALOGE("[%s]: finding dequeueBufferTagWrapPrototype() failed", __func__);
        }
        if (dupFenceFd >= 0) {
            ::close(dupFenceFd);
        }
    } else {
        ALOGE("[%s]: GraphicBuffer is null", __func__);
    }
}

void GedKpiDebug::onQueue(const uint64_t surfaceHandle, const sp<GraphicBuffer>& gb, const sp<Fence>& fence, const uint32_t numPendingBuffers) {
    if (gb != nullptr) {
        const int32_t dupFenceFd = fence->isValid() ? fence->dup() : -1;
        if (mGedKpiQueueBufferTagWrap) {
            const int32_t err = mGedKpiQueueBufferTagWrap(
                surfaceHandle, dupFenceFd, static_cast<int32_t>(numPendingBuffers), reinterpret_cast<intptr_t>(gb->handle));
            if (err != GED_OK) {
                ALOGE("[%s]: ged queue fail: size:%d" PRIu64 ,
                    __func__, static_cast<int>(numPendingBuffers));
            }
        } else {
            ALOGE("[%s]: finding queueBufferTagWrapPrototype() failed", __func__);
        }
        if (dupFenceFd >= 0) {
            ::close(dupFenceFd);
        }
    } else {
        ALOGE("[%s]: GraphicBuffer is null", __func__);
    }
}

void GedKpiDebug::onProducerConnect(const uint64_t surfaceHandle, const sp<IBinder>& token, const int api) {
    IPCThreadState* ipc = IPCThreadState::selfOrNull();
    int32_t pid = (token != NULL && NULL != token->localBinder())
        ? getpid()
        : (ipc != nullptr)?ipc->getCallingPid():-1;

    if (mGedKpiBufferConnectWrap) {
        mGedKpiBufferConnectWrap(surfaceHandle, api, pid);
    } else {
        ALOGE("[%s]: finding bufferConnectWrapPrototype() failed", __func__);
    }
}

void GedKpiDebug::onProducerDisconnect(const uint64_t surfaceHandle) {
    if (mGedKpiBufferDisconnectWrap) {
        mGedKpiBufferDisconnectWrap(surfaceHandle);
    } else {
        ALOGE("[%s]: finding bufferDisconnectWrapPrototype() failed", __func__);
    }
}
// -----------------------------------------------------------------------------
ANDROID_SINGLETON_STATIC_INSTANCE(GedKpiModuleLoader);

GedKpiModuleLoader::GedKpiModuleLoader() :
    mGedKpiSoHandle(NULL),
    mGedKpiCreate(NULL),
    mGedKpiDestroy(NULL),
    mGedKpiDequeueBuffer(NULL),
    mGedKpiQueueBuffer(NULL),
    mGedKpiAcquireBuffer(NULL),
    mGedKpiBufferConnect(NULL),
    mGedKpiBufferDisconnect(NULL)
{
    // used to notify ged about queue/acquire events for fast DVFS
    mGedKpiSoHandle = dlopen("libged_kpi.so", RTLD_LAZY);
    if (mGedKpiSoHandle) {
        mGedKpiCreate = reinterpret_cast<createWrapPrototype>(dlsym(mGedKpiSoHandle, "ged_kpi_create_wrap"));
        if (NULL == mGedKpiCreate) {
            ALOGE("finding createWrapPrototype() failed [%s]", dlerror());
        }

        mGedKpiDestroy = reinterpret_cast<destroyWrapPrototype>(dlsym(mGedKpiSoHandle, "ged_kpi_destroy_wrap"));
        if (NULL == mGedKpiDestroy) {
            ALOGE("finding destroyWrapPrototype() failed [%s]", dlerror());
        }

        mGedKpiDequeueBuffer = reinterpret_cast<dequeueBufferTagWrapPrototype>(dlsym(mGedKpiSoHandle, "ged_kpi_dequeue_buffer_tag_wrap"));
        if (NULL == mGedKpiDequeueBuffer) {
            ALOGE("finding dequeueBufferTagWrapPrototype() failed [%s]", dlerror());
        }

        mGedKpiQueueBuffer = reinterpret_cast<queueBufferTagWrapPrototype>(dlsym(mGedKpiSoHandle, "ged_kpi_queue_buffer_tag_wrap"));
        if (NULL == mGedKpiQueueBuffer) {
            ALOGE("finding queueBufferTagWrapPrototype() failed [%s]", dlerror());
        }

        mGedKpiAcquireBuffer = reinterpret_cast<acquireBufferTagWrapPrototype>(dlsym(mGedKpiSoHandle, "ged_kpi_acquire_buffer_tag_wrap"));
        if (NULL == mGedKpiAcquireBuffer) {
            ALOGE("finding acquireBufferTagWrapPrototype() failed [%s]", dlerror());
        }

        mGedKpiBufferConnect = reinterpret_cast<bufferConnectWrapPrototype>(dlsym(mGedKpiSoHandle, "ged_kpi_buffer_connect"));
        if (NULL == mGedKpiAcquireBuffer) {
            ALOGE("finding bufferConnectWrapPrototype() failed [%s]", dlerror());
        }

        mGedKpiBufferDisconnect = reinterpret_cast<bufferDisconnectWrapPrototype>(dlsym(mGedKpiSoHandle, "ged_kpi_buffer_disconnect"));
        if (NULL == mGedKpiAcquireBuffer) {
            ALOGE("finding bufferDisconnectWrapPrototype() failed [%s]", dlerror());
        }
    } else {
        ALOGE("open libged_kpi.so failed");
    }
}

GedKpiModuleLoader::~GedKpiModuleLoader() {
    if(mGedKpiSoHandle != NULL) {
        dlclose(mGedKpiSoHandle);
    }
}

createWrapPrototype GedKpiModuleLoader::GedKpiCreate() {
    if (mGedKpiCreate) {
        return mGedKpiCreate;
    } else {
        return NULL;
    }
}

destroyWrapPrototype GedKpiModuleLoader::GedKpiDestroy() {
    if (mGedKpiDestroy) {
        return mGedKpiDestroy;
    } else {
        return NULL;
    }
}

dequeueBufferTagWrapPrototype GedKpiModuleLoader::GedKpiDequeue() {
    if (mGedKpiDequeueBuffer) {
        return mGedKpiDequeueBuffer;
    } else {
        return NULL;
    }
}

queueBufferTagWrapPrototype GedKpiModuleLoader::GedKpiQueue() {
    if (mGedKpiQueueBuffer) {
        return mGedKpiQueueBuffer;
    } else {
        return NULL;
    }
}

acquireBufferTagWrapPrototype GedKpiModuleLoader::GedKpiAcquire() {
    if (mGedKpiAcquireBuffer) {
        return mGedKpiAcquireBuffer;
    } else {
        return NULL;
    }
}

bufferConnectWrapPrototype GedKpiModuleLoader::GedKpiBufferConnect() {
    if (mGedKpiBufferConnect) {
        return mGedKpiBufferConnect;
    } else {
        return NULL;
    }
}

bufferDisconnectWrapPrototype GedKpiModuleLoader::GedKpiBufferDisconnect() {
    if (mGedKpiBufferDisconnect) {
        return mGedKpiBufferDisconnect;
    } else {
        return NULL;
    }
}

}; // namespace android

#endif
