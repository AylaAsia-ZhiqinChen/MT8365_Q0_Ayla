#define LOG_TAG "AudioHfpULTaskSink"

#include "AudioHfpULTaskSink.h"
#include "AudioALSADeviceParser.h"
#include "AudioALSADriverUtility.h"


//#define TEST_WITHOUT_BT
#ifdef TEST_WITHOUT_BT
static const int gFakeBTBufferSizeMs = 45;
static int gFakeRemainBTBufferSizeMs = 45;
static struct timespec gTstempLastBTWrite;
static const char *AudioHfpULTaskOutput = "/data/vendor/audiohal/audio_dump/AudioHfpULTaskOutput_Dump";
#endif


namespace android
{

static bool mBTMode_Open;

int AudioHfpULTaskSink::DumpFileNum = 0;

AudioHfpULTaskSink::AudioHfpULTaskSink()
    : mStarted(false),
      mSampleRate(16000),
      mFormat(AUDIO_FORMAT_PCM_16_BIT),
      mInterruptSample(160),
      mAudioBTCVSDControl(AudioBTCVSDControl::getInstance()),
      mFd2(-1),
#ifdef TEST_WITHOUT_BT
      mAudioHfpULTaskSinkdumpFile(NULL),
#endif
      mChannelCount(2)
{
    mMixer = AudioALSADriverUtility::getInstance()->getMixer();
    ALOGD("mMixer = %p", mMixer);
    ASSERT(mMixer != NULL);

}

AudioHfpULTaskSink::~AudioHfpULTaskSink()
{
}

status_t AudioHfpULTaskSink::prepare(int format, uint32_t channelCount,
            uint32_t sampleRate, uint32_t accessSize)
{
    ALOGD("%s() format=%d channelCount=%d sampleRate=%d accessSize=%d", __func__, format, channelCount, sampleRate, accessSize);

    mSampleRate = sampleRate;
    mChannelCount = channelCount;
    mFormat = format;
    mInterruptSample = accessSize / (audio_bytes_per_sample((audio_format_t)format) * channelCount);

#ifndef MTK_BT_AUDIO_USING_BTCVSD
    if (mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "O07 I07 Switch"), 0, 1)) {
        ALOGE("Error: O07 I07 Switch invalid value");
    }
    if (mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "O08 I08 Switch"), 0, 1)) {
        ALOGE("Error: O08 I08 Switch invalid value");
    }
	if (mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "O03 I07 Switch"), 0, 0)) {
        ALOGE("Error: O03 I07 Switch invalid value");
    }
    if (mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "O04 I08 Switch"), 0, 0)) {
        ALOGE("Error: O04 I08 Switch invalid value");
    }
#endif

    return NO_ERROR;
}

ssize_t AudioHfpULTaskSink::write(const void *buffer, size_t bytes)
{
    ssize_t write_size = bytes;

#ifndef TEST_WITHOUT_BT
    if (!mStarted)
    {
        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

        int pcmindex, cardindex;

#ifdef MTK_BT_AUDIO_USING_BTCVSD
        pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmBTCVSDPlayback);
        cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmBTCVSDPlayback);
#else
        pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl2Meida);
        cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl2Meida);
#endif

        // HW pcm config
        memset(&mConfig, 0, sizeof(mConfig));
        mConfig.channels = mChannelCount;
        mConfig.rate = mSampleRate;
        mConfig.period_count = 6;

#ifdef MTK_BT_AUDIO_USING_BTCVSD
        /* btcvsd tx driver require buffersize % 60 == 0 */
        mConfig.period_size = 45;

#else
        mConfig.period_size = mInterruptSample;
#endif
        mConfig.format = PCM_FORMAT_S16_LE;

        mConfig.start_threshold = 0;
        mConfig.stop_threshold = 0;
        mConfig.silence_threshold = 0;
        mConfig.avail_min = 0;

        ALOGD("%s(), pcmindex = %d, cardindex = %d", __FUNCTION__, pcmindex, cardindex);
        ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
              __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);

        mPcm = pcm_open(cardindex, pcmindex, PCM_OUT | PCM_MONOTONIC, &mConfig);
        if (mPcm == NULL)
        {
            ALOGE("%s(), mPcm == NULL!!", __FUNCTION__);
            return -1;
        }
        else if (pcm_is_ready(mPcm) == false)
        {
            ALOGE("%s(), pcm_is_ready(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            return -1;
        }
        else if (pcm_start(mPcm) != 0)
        {
            ALOGE("%s(), pcm_start(%p) fail due to %s", __FUNCTION__, mPcm, pcm_get_error(mPcm));
        }

#ifdef MTK_BT_AUDIO_USING_BTCVSD
        if (mSampleRate == 16000)
            mAudioBTCVSDControl->BT_SCO_SetMode(1);
        else if (mSampleRate == 8000)
            mAudioBTCVSDControl->BT_SCO_SetMode(0);
        else {
            ALOGE("SampleRate Error!");
        }

        mBTMode_Open = mAudioBTCVSDControl->BT_SCO_isWideBand();
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "BTCVSD Band"), mBTMode_Open ? "WB" : "NB")) {
	        ALOGE("Error: BTCVSD Band invalid value");
        }

        mAudioBTCVSDControl->BT_SCO_SET_TXState(BT_SCO_TXSTATE_IDLE);
        mAudioBTCVSDControl->BTCVSD_Init(mFd2, mSampleRate, mChannelCount);
#endif
        mStarted = true;
    }

#ifdef MTK_BT_AUDIO_USING_BTCVSD
    int retval=writeDataToBTCVSD(buffer,write_size);
    write_size = retval;
    ALOGV("%s(): %d bytes has been write to btcvsd!",__func__,retval);
#else
    int retval = pcm_write(mPcm, buffer, write_size);
    ALOGE_IF(retval != 0, "%s(), pcm_write() error, retval = %d", __FUNCTION__, retval);
#endif
#else
    ALOGD("%s: Test without BT write()", __FUNCTION__);

    ++DumpFileNum;
    char Buf[10];
    sprintf(Buf, "%d.pcm", DumpFileNum);
    // dump mechanism
    DumpFileName = String8(AudioHfpULTaskOutput);
    DumpFileName.append((const char *)Buf);
    ALOGD("%s DumpFileName = %s", __FUNCTION__, DumpFileName.string());
    if (mAudioHfpULTaskSinkdumpFile == NULL)
    {
        mAudioHfpULTaskSinkdumpFile = AudioOpendumpPCMFile(DumpFileName, streamhfp_propty);
        ALOGD("%s() ,mAudioHfpULTaskSinkdumpFile:%p ",__func__,mAudioHfpULTaskSinkdumpFile);
    }

    if (mAudioHfpULTaskSinkdumpFile != NULL)
    {
        void* bufferNonConst = (void*)malloc(bytes);
        memcpy(bufferNonConst,buffer,bytes);
        AudioDumpPCMData(bufferNonConst , bytes, mAudioHfpULTaskSinkdumpFile);
        free(bufferNonConst);
    }
#endif

    return write_size;
}

status_t AudioHfpULTaskSink::stop()
{
    if (mStarted)
    {
        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

        if (mPcm) {
            pcm_stop(mPcm);
            pcm_close(mPcm);
            mPcm = NULL;
        }
        mStarted = false;
    }

#ifndef MTK_BT_AUDIO_USING_BTCVSD
	if (mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "O07 I07 Switch"), 0, 0)) {
        ALOGE("Error: O07 I07 Switch invalid value");
    }
    if (mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "O08 I08 Switch"), 0, 0)) {
        ALOGE("Error: O08 I08 Switch invalid value");
    }
	if (mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "O03 I07 Switch"), 0, 1)) {
        ALOGE("Error: O03 I07 Switch invalid value");
    }
    if (mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "O04 I08 Switch"), 0, 1)) {
        ALOGE("Error: O04 I08 Switch invalid value");
    }
#endif

#ifdef TEST_WITHOUT_BT
    if (mAudioHfpULTaskSinkdumpFile)
    {
        AudioCloseDumpPCMFile(mAudioHfpULTaskSinkdumpFile);
        mAudioHfpULTaskSinkdumpFile = NULL;
        ALOGD("ClosePcmDumpFile mAudioHfpULTaskSinkdumpFile");
    }
#endif

    return NO_ERROR;
}

int AudioHfpULTaskSink::getRemainDataLatency()
{
    return INVALID_OPERATION;
}

ssize_t AudioHfpULTaskSink::writeDataToBTCVSD(const void *buffer, size_t bytes){
    const size_t bytes_tmp = bytes;
    if (mPcm == NULL) {
        ALOGE("%s(), mPcm == NULL, return", __FUNCTION__);
        return bytes_tmp;
    }
    uint8_t *outbuffer, *inbuf, *workbuf;
    uint32_t insize, outsize, workbufsize, total_outsize, src_fs_s;
    //inbuf = (uint8_t *)pBufferAfterBitConvertion;// in playbackHandlerBtcvsd
    inbuf = (uint8_t *)buffer;
    ALOGV("%s(),do-while start!",__func__);
    do{
        outbuffer = mAudioBTCVSDControl->BT_SCO_TX_GetCVSDOutBuf();
        outsize = SCO_TX_ENCODE_SIZE;
        insize = bytes;
        workbuf = mAudioBTCVSDControl->BT_SCO_TX_GetCVSDWorkBuf();
        workbufsize = SCO_TX_PCM64K_BUF_SIZE;
        total_outsize = 0;
        do{
           if (mBTMode_Open != mAudioBTCVSDControl->BT_SCO_isWideBand()) {
                ALOGV("BTSCO change mode after TX_Begin!!!");
                mAudioBTCVSDControl->BT_SCO_TX_End(mFd2);
                mAudioBTCVSDControl->BT_SCO_TX_Begin(mFd2, mSampleRate, mChannelCount);
                mBTMode_Open = mAudioBTCVSDControl->BT_SCO_isWideBand();
                if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "BTCVSD Band"), mBTMode_Open ? "WB" : "NB")) {
                    ALOGE("Error: BTCVSD Band invalid value");
                }
                return bytes_tmp;
            }

            if (mAudioBTCVSDControl->BT_SCO_isWideBand()) {
                mAudioBTCVSDControl->btsco_process_TX_MSBC(inbuf, &insize, outbuffer, &outsize, workbuf); // return insize is consumed size
                ALOGV("WriteDataToBTSCOHW, do mSBC encode outsize=%d, consumed size=%d, bytes=%zu", outsize, insize, bytes);
            } else {
                mAudioBTCVSDControl->btsco_process_TX_CVSD(inbuf, &insize, outbuffer, &outsize, workbuf, workbufsize); // return insize is consumed size
                ALOGV("WriteDataToBTSCOHW, do CVSD encode outsize=%d, consumed size=%d, bytes=%zu", outsize, insize, bytes);
            }
            outbuffer += outsize;
            inbuf += insize;
            ASSERT(bytes >= insize);  // bytes - insize >= 0
            bytes -= insize;

            insize = bytes;
            total_outsize += outsize;
        }while(total_outsize < BTSCO_CVSD_TX_OUTBUF_SIZE && outsize != 0);
            ALOGV("WriteDataToBTSCOHW write to kernel(+) total_outsize = %u", total_outsize);
        // check if timeout during write bt data
            bool skip_write = false;

            struct mixer_ctl *ctl = mixer_get_ctl_by_name(mMixer, "BTCVSD Tx Timeout Switch");
            int index = mixer_ctl_get_value(ctl, 0);
            bool tx_timeout = index != 0;
            ALOGV("%s(), BTCVSD Tx Timeout Switch, tx_timeout %d, index %d", __FUNCTION__, tx_timeout, index);
            if (tx_timeout) {
                // write timeout
                struct timespec timeStamp;
                unsigned int avail;
                if (pcm_get_htimestamp(mPcm, &avail, &timeStamp) != 0) {
                    ALOGV("%s(), pcm_get_htimestamp fail %s\n", __FUNCTION__, pcm_get_error(mPcm));
                } else {
                    if ((total_outsize / (mConfig.channels * pcm_format_to_bits(mConfig.format) / 8)) > avail) {
                        skip_write = true;
                        ALOGW("%s(), tx_timeout %d, total_out_size %u, avail %u, skip write", __FUNCTION__, tx_timeout, total_outsize, avail);
                        return 0;
                    }
                }
            }

            if (!skip_write) {
                int retval = pcm_write(mPcm, mAudioBTCVSDControl->BT_SCO_TX_GetCVSDOutBuf(), total_outsize);
                if (retval != 0) {
                    ALOGE("%s(), pcm_write() error, retval = %d", __FUNCTION__, retval);
                } else{
                    ALOGV("%s():pcm_write %d bytes to BT_SCO_TX_GetCVSDOutBuf!",__func__,total_outsize);
                }
            }
        ALOGV("WriteDataToBTSCOHW write to kernel(-) remaining bytes = %zu", bytes);
    }while(bytes > 0);
    ALOGV("%s(),do-while finish!",__func__);
    //ALOGV("-%s(), buffer = %p,bytes = %zu,total_out_size=%zu", __FUNCTION__, buffer, bytes,total_outsize);
    return bytes_tmp;
}//end writeDataToBTCVSD


}
