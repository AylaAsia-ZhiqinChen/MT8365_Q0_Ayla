#ifndef ANDROID_AUDIO_ALSA_PLAYBACK_HANDLER_MIXER_H
#define ANDROID_AUDIO_ALSA_PLAYBACK_HANDLER_MIXER_H

#include "AudioALSAPlaybackHandlerBase.h"

namespace android {

class AudioMixerOut;
class AudioALSAPlaybackHandlerMixer : public AudioALSAPlaybackHandlerBase {
public:
    AudioALSAPlaybackHandlerMixer(const stream_attribute_t *stream_attribute_source);
    virtual ~AudioALSAPlaybackHandlerMixer();

    /**
     * open/close audio hardware
     */
    virtual status_t open();
    virtual status_t close();
    virtual status_t routing(const audio_devices_t output_devices);

    virtual int getLatency();

    virtual int setSuspend(bool suspend);

    /**
     * write data to audio hardware
     */
    virtual ssize_t  write(const void *buffer, size_t bytes);
    virtual int preWriteOperation(const void *buffer __unused, size_t bytes __unused) { return 0; }
    virtual int updateAudioMode(audio_mode_t mode __unused) { return 0; }

    /**
     * get hardware buffer info (framecount)
     */
    virtual status_t getHardwareBufferInfo(time_info_struct_t *HWBuffer_Time_Info);

    /**
     * low latency
     */
    virtual status_t setScreenState(bool mode, size_t buffer_size, size_t reduceInterruptSize, bool bforce = false);

private:
    AudioMixerOut *mMixerOut;

    struct timespec mNewtime, mOldtime;
    double latencyTime[3];
    int mIsForceDumpLatency;
};

} // end namespace android

#endif // end of ANDROID_AUDIO_ALSA_PLAYBACK_HANDLER_MIXER_H
