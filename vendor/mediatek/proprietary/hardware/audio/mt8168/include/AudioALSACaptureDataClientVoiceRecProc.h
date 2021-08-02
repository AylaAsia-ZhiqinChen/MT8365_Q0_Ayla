#ifndef ANDROID_AUDIO_ALSA_CAPTURE_DATA_CLIENT_VOICE_REC_CUST_H
#define ANDROID_AUDIO_ALSA_CAPTURE_DATA_CLIENT_VOICE_REC_CUST_H

#include "IAudioALSACaptureDataClient.h"

#include <pthread.h>


#include "AudioType.h"
#include "AudioUtility.h"

#include <audio_ringbuf.h>

#include <AudioLock.h>


class AudioVolumeInterface;

namespace android {

class AudioALSACaptureDataProviderBase;
class MtkAudioSrcBase;

//Data Process +++
class IAudioALSAProcessBase;
//Data Process ---

//Sample Rate Convert +++
class MtkAudioSrcBase;
//Sample Rate Convert ---

//Android Native Preprocess effect +++
class AudioPreProcess;
//Android Native Preprocess effect ---


/// Observer pattern: Observer
class AudioALSACaptureDataClientVoiceRecProc : public IAudioALSACaptureDataClient {
public:

    typedef struct audio_linear_buf {
        char                 *buf;
        uint32_t             buf_size;
        uint32_t             frame_count;
        uint32_t             num_channels;
        uint32_t             sample_rate;
        audio_format_t       audio_format;
    } audio_linear_buf_t;

    AudioALSACaptureDataClientVoiceRecProc(
        AudioALSACaptureDataProviderBase *pCaptureDataProvider,
        stream_attribute_t *stream_attribute_target);

    virtual ~AudioALSACaptureDataClientVoiceRecProc();


    /**
     * set client index
     */
    inline void        *getIdentity() const { return (void *)this; }

    /**
     * set/get raw frame count from hardware
     */
    inline void setRawStartFrameCount(int64_t frameCount) { mRawStartFrameCount = frameCount * mStreamAttributeTarget->sample_rate / mStreamAttributeSource->sample_rate; }
    virtual inline int64_t getRawStartFrameCount() { return mRawStartFrameCount; }

    /**
     * let provider copy raw data to client
     */
    virtual uint32_t    copyCaptureDataToClient(RingBuf pcm_read_buf); // called by capture data provider


    /**
     * let handler read processed data from client
     */
    virtual ssize_t     read(void *buffer, ssize_t bytes); // called by capture handler


    /**
     * check if the attached client has low latency requirement
     */
    bool    IsLowLatencyCapture(void);

    /**
     * Just implement Echo reference for dummy, not use
     */
    void AddEchoRefDataProvider(AudioALSACaptureDataProviderBase *pCaptureDataProvider, stream_attribute_t *stream_attribute_target) { }
    uint32_t    copyEchoRefCaptureDataToClient(RingBuf pcm_read_buf) { return 0; }

    /**
     * Query captured frames & time stamp
     */
    int getCapturePosition(int64_t *frames, int64_t *time);

    /**
     * Update BesRecord Parameters
     */
    status_t UpdateBesRecParam() { return INVALID_OPERATION; } /* TODO: remove it */


    /**
     * Attribute
     */
    const stream_attribute_t *getStreamAttributeSource() { return mStreamAttributeSource; }


    /**
     * StreamIn reopen
     */
    virtual bool getStreamInReopen() { return mStreamInReopen; }
    virtual void setStreamInReopen(bool state) { mStreamInReopen = state; }


private:
    AudioALSACaptureDataClientVoiceRecProc() {}


    /**
     * stream attribute
     */
    const stream_attribute_t *mStreamAttributeSource; // from audio hw
    stream_attribute_t       *mStreamAttributeTarget; // to stream in
    bool IsVoIPEnable(void) { return mStreamAttributeTarget->mVoIPEnable; }


    /**
     * for data provider
     */
    AudioALSACaptureDataProviderBase *mCaptureDataProvider;
    bool getPcmStatus(void);


    /**
     * for StreamIn reopen
     */
    void handleLockTimeout();


    /**
     * gain control
     */
    bool IsNeedApplyVolume();
    status_t ApplyVolume(void *Buffer, uint32_t BufferSize);
    bool mMicMute;
    bool mMuteTransition;
    AudioVolumeInterface *mAudioALSAVolumeController;


    /**
     * process raw data to processed data
     */
    bool            mEnable;

    bool            mStreamInReopen;

    static void    *processThread(void *arg);
    pthread_t       hProcessThread;
    bool            mProcessThreadLaunched;

    uint32_t        mLatency;
    int64_t         mRawStartFrameCount;

    /**
     * Raw Data Buffer
     */
    status_t initRawBuffer(void);
    status_t deInitRawBuffer(void);
    audio_ringbuf_t mRawDataBuf;
    AudioLock       mRawDataBufLock;
    audio_linear_buf_t mRawLinearBuf;
    uint32_t        mRawDataPeriodBufSize;

    /**
     * Processed Data Buffer
     */
    status_t initProcessedBuffer(void);
    status_t deInitProcessedBuffer(void);
    audio_ringbuf_t mProcessedDataBuf;
    AudioLock       mProcessedDataBufLock;
    audio_linear_buf_t mProcessedLinearBuf;
    uint32_t        mProcessedDataPeriodBufSize;

    /**
     * Sample Rate Convert Pre Process
     */
    status_t        initBliSrcPre(int src_rates, int dst_rates);
    status_t        deinitBliSrcPre();
    status_t        doBliSrcPre(void *pInBuffer, uint32_t inBytes, void *pOutBuffer, uint32_t *pOutBytes);
    MtkAudioSrcBase    *mBliSrcPre;
    audio_linear_buf_t mSrcPreOutLinearBuf;
    audio_ringbuf_t    mSrcPreOutBuf;
    bool               mBliSrcMultiPre;

    /**
     * DataProcess
     */
    status_t initDataProcess(void);
    status_t deInitDataProcess(void);
    IAudioALSAProcessBase *mProcess;
    audio_linear_buf_t   mProcInLinearBuf;
    audio_linear_buf_t   mProcOutLinearBuf;

    /**
     * Sample Rate Convert Post Process
     */
    status_t        initBliSrcPost(int src_rates, int dst_rates);
    status_t        deInitBliSrcPost();
    status_t        doBliSrcPost(void *pInBuffer, uint32_t inBytes, void *pOutBuffer, uint32_t *pOutBytes);
    MtkAudioSrcBase    *mBliSrcPost;
    audio_linear_buf_t mSrcPostOutLinearBuf;
    bool               mBliSrcMultiPost;

    /**
     * Format & ReMix
     */
    uint32_t        TransferFormat(char *src_buf, uint32_t src_size, char *dst_buf, uint32_t *pdst_size);
    uint32_t        ApplyChannelRemix(char *src_buf, uint32_t src_size, char *dst_buf, uint32_t *pdst_size);
    bool mNeedFmtCvt;
    bool mNeedChannelCvt;

    /**
     * Depop
     */
    uint32_t mDropPopSize;


    //Android Native Preprocess effect +++
    AudioPreProcess *mAudioPreProcessEffect;
    uint32_t NativePreprocess(void *buffer, uint32_t bytes);
    void CheckNativeEffect(void);
    //Android Native Preprocess effect ---

};

} // end namespace android

#endif // end of ANDROID_AUDIO_ALSA_CAPTURE_DATA_CLIENT_VOICE_REC_CUST_H
