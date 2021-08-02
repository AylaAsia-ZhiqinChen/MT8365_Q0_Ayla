#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#define __STDC_FORMAT_MACROS 1

#include "Scheduler/DispSync.h"

#include <dlfcn.h>
#include <inttypes.h>

#include <log/log.h>
#include <utils/String8.h>

#ifdef MTK_VSYNC_ENHANCEMENT_SUPPORT
#include "../DispSyncEnhancementApiLoader.h"
#endif

namespace android {
namespace impl {

#ifdef MTK_VSYNC_ENHANCEMENT_SUPPORT
void DispSync::initDispVsyncEnhancement() {
    struct DispSyncEnhancementFunctionList list;
    getDispSyncEnhancementFunctionList(&list);
    DispSyncEnhancementApiLoader::getInstance().registerFunction(&list);
}

bool DispSync::obeyResync() {
    Mutex::Autolock lock(mMutex);
    return DispSyncEnhancementApiLoader::getInstance().obeyResync();
}

bool DispSync::addPresentFenceEnhancementLocked(bool* res) {
    return DispSyncEnhancementApiLoader::getInstance().addPresentFence(res);
}

bool DispSync::addResyncSampleEnhancementLocked(bool* res, nsecs_t timestamp) {
    return DispSyncEnhancementApiLoader::getInstance().addResyncSample(res, timestamp,
            &mPeriod, &mPhase, &mReferenceTime);
}

void DispSync::dumpEnhanceInfo(std::string& result) const {
    DispSyncEnhancementApiLoader::getInstance().dump(result);
}

status_t DispSync::setVSyncMode(int32_t mode, int32_t fps) {
    Mutex::Autolock lock(mMutex);
    return DispSyncEnhancementApiLoader::getInstance().setVSyncMode(mode, fps, &mPeriod,
            &mPhase, &mReferenceTime);;
}
#endif

}
}
