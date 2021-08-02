#ifndef ANDROID_AUDIO_ALSA_CAPTURE_HANDLER_NULL_H
#define ANDROID_AUDIO_ALSA_CAPTURE_HANDLER_NULL_H

#include "AudioALSACaptureHandlerBase.h"

namespace android
{

class AudioALSACaptureHandlerNull : public AudioALSACaptureHandlerBase
{
    public:
        AudioALSACaptureHandlerNull(stream_attribute_t *stream_attribute_target);
        virtual ~AudioALSACaptureHandlerNull();

        /**
         * open/close speech driver
         */
        virtual status_t open();
        virtual status_t close();
        virtual status_t routing(const audio_devices_t input_device);

        /**
         * read data from speech driver
         */
        virtual ssize_t  read(void *buffer, ssize_t bytes);

    protected:
        /**
         * init audio speech driver
         */
        virtual status_t init();

        size_t mBytesPerSecond;
};

} // end namespace android

#endif // end of ANDROID_AUDIO_ALSA_CAPTURE_HANDLER_NULL_H
