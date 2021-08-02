#ifndef __ANDROID_GUI_DISPDEJITTER_HELPER_H__
#define __ANDROID_GUI_DISPDEJITTER_HELPER_H__

#include <utils/RefBase.h>
#include <utils/Singleton.h>

namespace android {

class DispDeJitter;
class GraphicBuffer;
class String8;

class DispDeJitterHelper : public Singleton<DispDeJitterHelper> {
public:
    DispDeJitterHelper();
    virtual ~DispDeJitterHelper();

    DispDeJitter* createDispDeJitter();
    void destroyDispDeJitter(DispDeJitter* dispDeJitter);
    bool shouldPresentNow(DispDeJitter* dispDeJitter, const String8& name, const sp<GraphicBuffer>& gb,
                          const nsecs_t& expectedPresent, const bool isDue);
    void markTimestamp(const sp<GraphicBuffer>& gb);

protected:
    void* mSoHandle;
    DispDeJitter* (*mFnCreateDispDeJitter)();
    void (*mFnDestroyDispDeJitter)(DispDeJitter*);
    bool (*mFnShouldDelayPresent)(DispDeJitter*, const String8&, const sp<GraphicBuffer>&, const nsecs_t&);
    void (*mFnMarkTimestamp)(const sp<GraphicBuffer>&);
};

}   // namespace android
#endif  // __ANDROID_GUI_DISPDEJITTER_HELPER_H__

