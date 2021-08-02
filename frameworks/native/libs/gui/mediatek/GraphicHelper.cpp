#define LOG_TAG "GraphicExt"
//#define LOG_NDEBUG 0
//#define MTK_LOG_ENABLE 1
#include <cmath>
#include <dlfcn.h>

#include <gui/mediatek/GraphicHelper.h>

namespace android {
// -----------------------------------------------------------------------------
ANDROID_SINGLETON_STATIC_INSTANCE(GraphicExtModuleLoader);

GraphicExtModuleLoader::GraphicExtModuleLoader() :
    mGraphicExtSOHandle(NULL),
    mCreateGraphicExtInstancePtr(NULL),
    mAPI(NULL)
{
    typedef GraphicExtUtilAPI *(*createGraphicExtPrototype)();
    mGraphicExtSOHandle = dlopen("libboost_ext_fwk.so", RTLD_LAZY);
    if (mGraphicExtSOHandle) {
        mCreateGraphicExtInstancePtr = reinterpret_cast<createGraphicExtPrototype>(dlsym(mGraphicExtSOHandle, "createUtilInstance"));
        if (mCreateGraphicExtInstancePtr == NULL) {
            ALOGE("Can't load func mCreateBQdumpPtr");
        }
    } else {
        ALOGE("Can't load libboost_ext_fwk");
    }
}

GraphicExtModuleLoader::~GraphicExtModuleLoader() {
    if (mAPI != NULL){
        delete mAPI;
    }

    if (mGraphicExtSOHandle != NULL) {
        dlclose(mGraphicExtSOHandle);
    }
}

status_t GraphicExtModuleLoader::clearEnv(const Surface* surface, bool status) {
    status_t err = NO_ERROR;
    if (mAPI) {
       err = mAPI->clearEnv(surface, status);
    }
    return err;
}

status_t GraphicExtModuleLoader::setBQEnv(const Surface* surface, bool status) {
    status_t err = NO_ERROR;
    if (mAPI) {
       err = mAPI->setBQEnv(surface, status);
    }
    return err;
}

status_t GraphicExtModuleLoader::setGLEnv(const Surface* surface) {
    status_t err = NO_ERROR;
    if (mAPI) {
       err = mAPI->setGLEnv(surface);
    }
    return err;
}

void GraphicExtModuleLoader::CreateGraphicExtInstance() {
    if (mCreateGraphicExtInstancePtr) {
        mAPI = mCreateGraphicExtInstancePtr();
    } else {
        ALOGE("GraphicExtModuleLoader::CreateGraphicExtInstance false");
    }
}
}; // namespace android
