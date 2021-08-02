#ifndef ANDROID_AUDIO_ALSA_CAPTURE_DATA_PROVIDER_SOUND_TRIGGER_H
#define ANDROID_AUDIO_ALSA_CAPTURE_DATA_PROVIDER_SOUND_TRIGGER_H

#include "AudioALSACaptureDataProviderBase.h"

namespace android {

struct sound_trigger_device;

class AudioALSACaptureDataProviderSoundTrigger : public AudioALSACaptureDataProviderBase {
public:
    virtual ~AudioALSACaptureDataProviderSoundTrigger();

    static AudioALSACaptureDataProviderSoundTrigger *getInstance();

    /**
     * open/close pcm interface when 1st attach & the last detach
     */
    status_t open();
    status_t close();



protected:
    AudioALSACaptureDataProviderSoundTrigger();



private:
    /**
     * singleton pattern
     */
    static AudioALSACaptureDataProviderSoundTrigger *mAudioALSACaptureDataProvider;


    /**
     * pcm read thread
     */
    static void *readThread(void *arg);
    pthread_t hReadThread;
    struct sound_trigger_device *mStDev;
    int mStDevHandle;

};

} // end namespace android

#endif // end of ANDROID_AUDIO_ALSA_CAPTURE_DATA_PROVIDER_SOUND_TRIGGER_H