#ifndef ANDROID_AUDIO_ALSA_STREAM_OUT_H
#define ANDROID_AUDIO_ALSA_STREAM_OUT_H

#include <utils/Mutex.h>

#include <hardware_legacy/AudioMTKHardwareInterface.h>
#include <hardware/audio.h>
#include <system/audio.h>
#include <media/AudioParameter.h>

#include "AudioType.h"
#include <AudioLock.h>

#include "AudioALSAPlaybackHandlerBase.h"

namespace android
{

class AudioALSAStreamManager;

enum stream_out_type
{
    STREAM_OUT_PRIMARY = 0,
    STREAM_OUT_HDMI_STEREO = 1,
    STREAM_OUT_HDMI_MULTI_CHANNEL = 2,
    STREAM_OUT_VOICE_DL,
    STREAM_OUT_LOW_LATENCY,
    NUM_STREAM_OUT_TYPE
};

class AudioALSAStreamOut : public AudioMTKStreamOutInterface
{
    public:
        AudioALSAStreamOut();
        virtual ~AudioALSAStreamOut();

        virtual status_t set(uint32_t devices,
                             int *format,
                             uint32_t *channels,
                             uint32_t *sampleRate,
                             status_t *status,
                             uint32_t flags);

        /** return audio sampling rate in hz - eg. 44100 */
        virtual uint32_t    sampleRate() const;

        /** returns size of output buffer - eg. 4800 */
        virtual size_t      bufferSize() const;

        /** returns the output channel mask */
        virtual uint32_t    channels() const;

        /**
         * return audio format in 8bit or 16bit PCM format -
         * eg. AUDIO_FORMAT_PCM_16_BIT
         */
        virtual int         format() const;

        /**
         * return the frame size (number of bytes per sample).
         */
        uint32_t    frameSize() const;

        /**
         * return the audio hardware driver latency in milli seconds.
         */
        virtual uint32_t    latency();

        /**
         * Use this method in situations where audio mixing is done in the
         * hardware. This method serves as a direct interface with hardware,
         * allowing you to directly set the volume as apposed to via the framework.
         * This method might produce multiple PCM outputs or hardware accelerated
         * codecs, such as MP3 or AAC.
         */
        virtual status_t    setVolume(float left, float right);

        /** write audio buffer to driver. Returns number of bytes written */
        virtual ssize_t     write(const void *buffer, size_t bytes);
        virtual int drain(audio_drain_type_t type);
        /**
         * Put the audio hardware output into standby mode. Returns
         * status based on include/utils/Errors.h
         */
          // "standby" is used by framework to close stream out.
        virtual status_t    standby(bool halRequest = true);
        //virtual status_t    standby();
        virtual status_t    standbyStreamOut(bool halRequest = true);

        /** dump the state of the audio output device */
        virtual status_t dump(int fd, const Vector<String16> &args);

        // set/get audio output parameters. The function accepts a list of parameters
        // key value pairs in the form: key1=value1;key2=value2;...
        // Some keys are reserved for standard parameters (See AudioParameter class).
        // If the implementation does not accept a parameter change while the output is
        // active but the parameter is acceptable otherwise, it must return INVALID_OPERATION.
        // The audio flinger will put the output in standby and then change the parameter value.
        virtual status_t    setParameters(const String8 &keyValuePairs);
        virtual String8     getParameters(const String8 &keys);

        // return the number of audio frames written by the audio dsp to DAC since
        // the output has exited standby
        virtual status_t    getRenderPosition(uint32_t *dspFrames);


        /**
         * set stream out index
         */
        inline void         setIdentity(const uint32_t identity) { mStreamAttributeSource.mStreamOutIndex = mIdentity = identity; }
        inline uint32_t     getIdentity() const { return mIdentity; }

        inline uint32_t     getStreamOutType() const { return mStreamOutType; }

        /**
         * open/close stream out related audio hardware
         */
        virtual status_t    open();
        virtual status_t    close();
        virtual status_t    pause();
        virtual status_t    resume();
        virtual status_t    flush();

        bool                isOutPutStreamActive() { return (mStandby == false) ? true : false; }


        /**
         * routing
         */
        status_t routing(audio_devices_t output_devices);
        void updatePolicyDevice(audio_devices_t outputDevice);


        /**
         * suspend/resume
         */
        status_t setSuspend(const bool suspend_on);
        static status_t setSuspendStreamOutHDMIStereo(const bool suspend_on);


        /**
         * get stream attribute
         */
        virtual const stream_attribute_t *getStreamAttribute() const { return &mStreamAttributeSource; }


        /**
         * low latency
         */
        status_t setScreenState(bool mode);

	/**
	 *
	 */
        status_t UpdateSampleRate(int samplerate);

	/**
	 *
	 */
        bool  SampleRateSupport(uint32_t samplerate);

        virtual status_t    getPresentationPosition(uint64_t *frames, struct timespec *timestamp);//Add for KK

        virtual status_t    setCallBack(stream_callback_t callback, void *cookie);
        //Force below two functions to pure virtual
        virtual status_t    getNextWriteTimestamp(int64_t *timestamp);

        /**
         * Called when the metadata of the stream's source has been changed.
         * @param source_metadata Description of the audio that is played by the clients.
         */
        virtual void updateSourceMetadata(const struct source_metadata* source_metadata);
		
	 /**
     * AAudio MMAP
     */
    virtual status_t    start();
    virtual status_t    stop();
    virtual status_t    createMmapBuffer(int32_t min_size_frames,
                                  struct audio_mmap_buffer_info *info);
    virtual status_t    getMmapPosition(struct audio_mmap_position *position);

    protected:
        AudioALSAStreamManager         *mStreamManager;
        AudioALSAPlaybackHandlerBase   *mPlaybackHandler;

        /**
         * for debug PCM dump
         */
        void  OpenPCMDump(const char *class_name);
        void  ClosePCMDump(void);
        void  WritePcmDumpData(const void *buffer, ssize_t bytes);
        FILE *mPCMDumpFile;
        static uint32_t mDumpFileNum;

        /**
         * low latency
         */
        status_t setScreenState_l()
        {
            status_t ret = NO_ERROR;
            if (NULL != mPlaybackHandler)
            {
#if defined(MTK_HYBRID_NLE_SUPPORT)
                ret = mPlaybackHandler->setScreenState(mLowLatencyMode != 0, mStreamAttributeSource.buffer_size, (8192<<1));
#else
                ret = mPlaybackHandler->setScreenState(mLowLatencyMode != 0, mStreamAttributeSource.buffer_size, 8192);
#endif
            }
            return ret;
        }

    private:
        AudioLock           mLock;
        volatile int32_t    mLockCount;
        AudioLock           mSuspendLock;
        static AudioLock    mStreamOutsLock;

        uint32_t            mIdentity; // key for mStreamOutVector

        uint32_t     mSuspendCount;
        static uint32_t     mSuspendStreamOutHDMIStereoCount;

        bool                mStandby;
        stream_attribute_t  mStreamAttributeSource;

        stream_out_type     mStreamOutType;
        static AudioALSAStreamOut *mStreamOutHDMIStereo;
        uint64_t mPresentedBytes;
        timespec mPresentedTime;
        uint64_t mPresentFrames;

        bool                mLowLatencyMode;
        bool                mOffload;
        bool                mPaused;
        stream_callback_t   mStreamCbk;
        void                *mCbkCookie;
        uint32_t            mOffloadVol;

        void setBufferSize();
        void checkSuspendOutput(void);
        uint32_t dataToDurationUs(size_t bytes);
        bool isHdmiOut() const
        {
            return ((mStreamOutType == STREAM_OUT_HDMI_STEREO) ||
                    (mStreamOutType == STREAM_OUT_HDMI_MULTI_CHANNEL));
        }
        bool isHdmiOutNeedIECWrap() const
        {
            return ((mStreamOutType == STREAM_OUT_HDMI_MULTI_CHANNEL) &&
                     !audio_has_proportional_frames((audio_format_t)format()));
        }
        void checkRedundantAndAdd(Vector<String8>& strVector, String8 str);
        status_t getPresentationPosition_l(uint64_t *frames, struct timespec *timestamp);
        status_t getRenderPosition_l(uint32_t *dspFrames, bool isGetFromHW);

        /**
         * mPerfServiceHandle : PerfService's scenario handler
         */
#ifdef DOWNLINK_SUPPORT_PERFSERVICE
        int mPerfServiceHandle;
        bool mScreenOff;
        bool mIsLowPowerEnabled;
#endif

        void enableLowPowerSetting(void);
        void disableLowPowerSetting(void);

        size_t              mBufferSizePerAccess;
        bool                mInternalSuspend;
        uint64_t            mWriteCount;
        Vector<String8>     mSupportedFormats;
        Vector<String8>     mSupportedChannelMasks;
        Vector<String8>     mSupportedSampleRates;
        Mutex               mPresentationLock;
        uint32_t            mLastPresentationFrames;
        struct timespec     mLastPresentationTimestamp;
        bool                mLastPresentationValid;
        uint64_t            mRenderedBytes;
        uint32_t            mLastDspFrames;
		
    /**
     * AAudio MMAP
     */
    bool                mStart;
    bool                mDestroy;
};

} // end namespace android

#endif // end of ANDROID_SPEECH_DRIVER_INTERFACE_H
