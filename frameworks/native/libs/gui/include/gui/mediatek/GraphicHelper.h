#ifndef ANDROID_GUI_GRAPHICEXT_H
#define ANDROID_GUI_GRAPHICEXT_H

#include <gui/IGraphicBufferConsumer.h>
#include <utils/RefBase.h>
#include <utils/Singleton.h>
#include <gui_debug/BufferQueueDumpAPI.h>
#include <gui/Surface.h>
#include <gui/mediatek/GraphicBoostUtilAPI.h>

namespace android {
// ----------------------------------------------------------------------------
// GraphicExt loader for dl open libboost_ext_fwk
class GraphicExtModuleLoader : public Singleton<GraphicExtModuleLoader> {
public:
    GraphicExtModuleLoader();
    ~GraphicExtModuleLoader();
    status_t setGLEnv(const Surface* surface);
    status_t setBQEnv(const Surface* surface, bool status);
    status_t clearEnv(const Surface* surface, bool status);
    void CreateGraphicExtInstance();
private:
    void* mGraphicExtSOHandle;
    GraphicExtUtilAPI* (*mCreateGraphicExtInstancePtr)();
    GraphicExtUtilAPI* mAPI;
};

// ----------------------------------------------------------------------------
}; // namespace android
#endif // ANDROID_GUI_BUFFERQUEUEDEBUG_H
