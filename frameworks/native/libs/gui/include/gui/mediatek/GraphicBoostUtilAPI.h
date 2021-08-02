#ifndef ANDROID_MTK_GRAPHIC_EXT_UTILAPI_H
#define ANDROID_MTK_GRAPHIC_EXT_UTILAPI_H

namespace android {
// ---------------------------------------------------------------------------
class Surface;

class GraphicExtUtilAPI
{
public:
    GraphicExtUtilAPI() {}
    virtual ~GraphicExtUtilAPI() {}
    virtual int setGLEnv(const Surface* surface);
    virtual int setBQEnv(const Surface* surface, bool status);
    virtual int clearEnv(const Surface* surface, bool status);
    virtual void notifyOtherModule();

};

extern "C"
{
    GraphicExtUtilAPI *createUtilInstance();
}
// ---------------------------------------------------------------------------
}; // namespace android

#endif // ANDROID_MTK_GRAPHIC_EXT_UTIL_H
