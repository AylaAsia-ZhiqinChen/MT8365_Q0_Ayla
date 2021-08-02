#ifndef ANDROID_AUDIO_ALSA_CAPTURE_DATA_PROVIDER_HW_CM_H
#define ANDROID_AUDIO_ALSA_CAPTURE_DATA_PROVIDER_HW_CM_H

#include "AudioALSACaptureDataProviderBase.h"

namespace android {

class AudioALSACaptureDataProviderHwCM : public AudioALSACaptureDataProviderBase {
public:
    virtual ~AudioALSACaptureDataProviderHwCM();

    static AudioALSACaptureDataProviderHwCM *getInstance();

    /**
     * open/close pcm interface when 1st attach & the last detach
     */
    status_t open();
    status_t close();



protected:
    AudioALSACaptureDataProviderHwCM();



private:
    /**
     * singleton pattern
     */
    static AudioALSACaptureDataProviderHwCM *mAudioALSACaptureDataProvider;


    /**
     * pcm read thread
     */
    static void *readThread(void *arg);
    pthread_t hReadThread;

};

} // end namespace android

#endif // end of ANDROID_AUDIO_ALSA_CAPTURE_DATA_PROVIDER_HW_CM_H