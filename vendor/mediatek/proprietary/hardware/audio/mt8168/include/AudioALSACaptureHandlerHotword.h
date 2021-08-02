#ifndef ANDROID_AUDIO_ALSA_CAPTURE_HANDLER_HOTWORD_H
#define ANDROID_AUDIO_ALSA_CAPTURE_HANDLER_HOTWORD_H

#include "AudioALSACaptureHandlerBase.h"

namespace android
{
class AudioALSACaptureHandlerHotword : public AudioALSACaptureHandlerBase
{
    public:
        AudioALSACaptureHandlerHotword(stream_attribute_t *stream_attribute_target);
        virtual ~AudioALSACaptureHandlerHotword();

        /**
         * open/close audio hardware
         */
        virtual status_t open();
        virtual status_t close();
        virtual status_t routing(const audio_devices_t input_device);


        /**
         * read data from audio hardware
         */
        virtual ssize_t  read(void *buffer, ssize_t bytes);

    protected:
        /**
         * init audio hardware
         */
        virtual status_t init();

        int mStDevSupport;
        bool mStDevInUse;


    private:

};

} // end namespace android

#endif // end of ANDROID_AUDIO_ALSA_CAPTURE_HANDLER_HOTWORD_H