#ifndef ANDROID_AUDIO_ALSA_PLAYBACK_HANDLER_HDMI_H
#define ANDROID_AUDIO_ALSA_PLAYBACK_HANDLER_HDMI_H

#include "AudioALSAPlaybackHandlerBase.h"
#include <audio_utils/spdif/SPDIFEncoder.h>


namespace android
{

class AudioALSAPlaybackHandlerHDMI : public AudioALSAPlaybackHandlerBase
{
public:
    AudioALSAPlaybackHandlerHDMI(const stream_attribute_t *stream_attribute_source);
    virtual ~AudioALSAPlaybackHandlerHDMI();

    /**
     * open/close audio hardware
     */
    virtual status_t open();
    virtual status_t close();
    virtual status_t pause();
    virtual status_t resume();
    virtual status_t flush();
    virtual int drain(audio_drain_type_t type);
    virtual status_t routing(const audio_devices_t output_devices);
    virtual status_t setVolume(uint32_t vol);


    /**
     * write data to audio hardware
     */
    virtual ssize_t  write(const void *buffer, size_t bytes);


    /**
    * get hardware buffer info (framecount)
    */
    virtual status_t getHardwareBufferInfo(time_info_struct_t *HWBuffer_Time_Info);

    virtual uint64_t getBytesWriteKernel();

    static void forceClkOn(const stream_attribute_t *stream_attribute_source);
    static void forceClkOff();

private:
/***************** fixme ***************/
    class IecSPDIFEncoder/* : public SPDIFEncoder*/
    {
    public:
        /*IecSPDIFEncoder(AudioALSAPlaybackHandlerHDMI *playbackHandler, audio_format_t format)
            : SPDIFEncoder(format)
            , mPlaybackHandler(playbackHandler)
        {
        }*/

        /*virtual */ssize_t writeOutput(const void* buffer, size_t bytes)
        {
            return 0;/*mPlaybackHandler->writeDataBurst(buffer, bytes);*/
        }

    protected:
        /*AudioALSAPlaybackHandlerHDMI * const mPlaybackHandler;*/
    };

    ssize_t writeDataBurst(const void* buffer, size_t bytes);
    ssize_t writeKernel(const void* buffer, size_t bytes);

    IecSPDIFEncoder *mSpdifEncoder;
    bool mWrapperNeeded;
};

} // end namespace android

#endif // end of ANDROID_AUDIO_ALSA_PLAYBACK_HANDLER_HDMI_H