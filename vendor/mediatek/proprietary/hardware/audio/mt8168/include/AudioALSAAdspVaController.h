#ifndef ANDROID_ALSA_AUDIO_ADSP_VA_CONTROLLER_H
#define ANDROID_ALSA_AUDIO_ADSP_VA_CONTROLLER_H

#include <tinyalsa/asoundlib.h>

#include "AudioType.h"
#include "AudioParamParser.h"
#include <AudioLock.h>

namespace android {

//class AudioALSAStreamManager;

//ADSP_VA_TODO how to improve the path search
typedef struct sound_trigger_device {
    void *sound_trigger_lib;
    int (*open_for_streaming)();
    int (*close_for_streaming)(int);
    int (*read_samples)(int, void *, size_t);
    int (*get_parameters)(int, void *);
} sound_trigger_device_t;

class AudioALSAAdspVaController {
public:
    virtual ~AudioALSAAdspVaController();

    static AudioALSAAdspVaController *getInstance();

    virtual status_t setVoiceDetectEnable(bool enable);
    virtual bool     getVoiceDetectEnable();

    virtual status_t setAdspEnable(int enable);
    virtual bool     getAdspEnable();

    sound_trigger_device_t *getSoundTriggerDevice(void)
    {
        return &mStDev;
    }

    virtual status_t updateDeviceInfoForVoiceWakeUp();

protected:
    AudioALSAAdspVaController();

private:
    /**
     * singleton pattern
     */
    static AudioALSAAdspVaController *mAudioALSAAdspVaController;

    stream_attribute_t *stream_attribute_target;

    struct mixer *mMixer;

    AudioLock mLock;

    bool mDetectEnable;
    int  mAdspEnable;

    sound_trigger_device_t mStDev;

    bool mIsUseHeadsetMic;
};

} // end namespace android

#endif // end of ANDROID_ALSA_AUDIO_ADSP_VA_CONTROLLER_H