#define LOG_TAG "AudioHfpULTaskSink"

#include "AudioHfpULTaskSink.h"
#include "AudioBTCVSDControl.h"
#include "WCNChipController.h"

#include "AudioALSADeviceString.h"
#include "AudioALSADeviceParser.h"
#include "AudioALSADriverUtility.h"
#include "AudioALSAHardwareResourceManager.h"

//#define TEST_WITHOUT_BT
#ifdef TEST_WITHOUT_BT
static const int gFakeBTBufferSizeMs = 45;
static int gFakeRemainBTBufferSizeMs = 45;
static struct timespec gTstempLastBTWrite;
static const char *AudioHfpULTaskOutput = "/sdcard/mtklog/audio_dump/AudioHfpULTaskOutput_Dump";
#endif

namespace android
{

int AudioHfpULTaskSink::DumpFileNum = 0;

AudioHfpULTaskSink::AudioHfpULTaskSink()
    : mFdBluetooth(0),
      mAudioBTCVSDControl(AudioBTCVSDControl::getInstance()),
      mIsUseMergeInterface(WCNChipController::GetInstance()->IsBTMergeInterfaceSupported()),
      mIsRunning(false),
      mPrepared(false),
      mInterruptSample(160),
      mStarted(false),
      mPcm(NULL),
#ifdef TEST_WITHOUT_BT
      mAudioHfpULTaskSinkdumpFile(NULL),
#endif
      mChannelCount(0),
      mMixer(AudioALSADriverUtility::getInstance()->getMixer())
{

    if (mIsUseMergeInterface)
    {
        ALOGW("%s() no merge interface support for HFP feature, we do nothing.", __func__);
        return;
    }
#ifndef MTK_SUPPORT_BTCVSD_ALSA
    //get driver instance
    mFdBluetooth = mAudioBTCVSDControl->getFd();
    if (mFdBluetooth <= 0)
    {
        ALOGE("%s() getFd fail", __func__);
    }
#endif
#if defined(MTK_CONSYS_MT8167)
    mAudioBTCVSDControl->BT_SCO_SET_TXState(BT_SCO_TXSTATE_IDLE);
#endif
}

AudioHfpULTaskSink::~AudioHfpULTaskSink()
{
}

status_t AudioHfpULTaskSink::prepare(int format, uint32_t channelCount,
            uint32_t sampleRate, uint32_t accessSize)
{
    if (mIsUseMergeInterface)
    {
        ALOGW("%s() no merge interface support for HFP feature, we do nothing.", __func__);
        return UNKNOWN_ERROR;
    }
    if (mPrepared)
    {
        ALOGW("%s() called but already prepared!", __func__);
        return NO_ERROR;
    }
    //parameter checking & store parameter
    status_t status = paramCheckAndUpdate(format,channelCount,sampleRate,accessSize);

    mInterruptSample = accessSize / (audio_bytes_per_sample((audio_format_t)format) * channelCount);

    if (status)
    {
        ALOGE("prepare failed: status = %i", status);
        return status;
    }

    //initial member data and library
    mPrepared = true;
    return NO_ERROR;
}

ssize_t AudioHfpULTaskSink::write(const void *buffer, size_t bytes)
{
    if (!mStarted){

        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());
#if defined(MTK_CONSYS_MT8167) //8167 6627
        //////copy from Capture
        memset(&mConfig, 0, sizeof(mConfig));
        mConfig.channels = mChannelCount;
        mConfig.rate = mSampleRate;
        mConfig.period_size = 1020; // must be multiple of SCO_TX_ENCODE_SIZE
        mConfig.period_count = 2;
        mConfig.format = PCM_FORMAT_S16_LE;
        mConfig.start_threshold = 1020;
        mConfig.stop_threshold = 0;
        mConfig.silence_threshold = 0;

        ASSERT(mPcm == NULL);
        int pcmIdx = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmBTCVSDPlayback);
        int cardIdx = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmBTCVSDPlayback);

        ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d, pcmIdx = %d, cardIdx = %d",
              __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format,pcmIdx,cardIdx);

        mPcm = pcm_open(cardIdx, pcmIdx, PCM_OUT, &mConfig);
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
        else if (pcm_prepare(mPcm) != 0)
        {
            ALOGE("%s(), pcm_prepare(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            return -1;
        }
#else // 8167 7668
        int pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl2Meida);
        int cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl2Meida);

        // HW pcm config
        memset(&mConfig, 0, sizeof(mConfig));
        mConfig.channels = mChannelCount;
        mConfig.rate = mSampleRate;
        mConfig.period_count = 4;
        mConfig.period_size = mInterruptSample;
        mConfig.format = PCM_FORMAT_S16_LE;

        mConfig.start_threshold = 0;
        mConfig.stop_threshold = 0;
        mConfig.silence_threshold = 0;
        mConfig.avail_min = 0;

        ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
              __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);

        mPcm = pcm_open(cardindex, pcmindex, PCM_OUT, &mConfig);
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
        else if (pcm_prepare(mPcm) != 0)
        {
            ALOGE("%s(), pcm_prepare(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            return -1;
        }
#endif
        mStarted = true;

    }


    if (mIsUseMergeInterface)
    {
        ALOGW("%s() no merge interface support for HFP feature, we do nothing.", __func__);
        return 0;
    }
    ssize_t writtenBytes = bytes;
    if (!mPrepared)
    {
         ALOGW("%s() called but not prepared!", __func__);
        return 0;
    }

    if (!mIsRunning)
    {
        mIsRunning = true;
        //init BTCVSD
#ifndef TEST_WITHOUT_BT
#if defined(MTK_CONSYS_MT8167) // 8167 6627
        mAudioBTCVSDControl->BTCVSD_Init(mFdBluetooth, getSampleRate(), mChannelCount);
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
#endif
    }

#if defined(MTK_CONSYS_MT8167) // 8167 6627
    //write to BTCVSD
    writtenBytes = writeToBTCVSDController(buffer,bytes);
#else // 8167 7668
    writtenBytes = pcm_write(mPcm, buffer, bytes);
    ALOGE_IF(writtenBytes != 0, "%s(), pcm_write() error, writtenBytes = %d", __FUNCTION__, writtenBytes);
#endif
    return writtenBytes;
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

    if (mIsUseMergeInterface)
    {
        ALOGW("%s() no merge interface support for HFP feature, we do nothing.", __func__);
        return UNKNOWN_ERROR;
    }
    if (!mPrepared)
    {
        ALOGW("%s() called but not prepared!", __func__);
        return NO_INIT;
    }

    if (mIsRunning)
    {
        mIsRunning = false;
#ifndef TEST_WITHOUT_BT
#if defined(MTK_CONSYS_MT8167) //8167 6627
        mAudioBTCVSDControl->BTCVSD_StandbyProcess(mFdBluetooth);
#endif
#endif
    }

    else
    {
        ALOGW("%s() called but not running!", __func__);
    }

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

size_t AudioHfpULTaskSink::writeToBTCVSDController(const void *buffer,size_t bytes)
{

    ssize_t writtenBytes = 0;
    size_t outputSize = 0;
    uint8_t *outbuffer, *inbuf, *workbuf, i;
    uint32_t insize, outsize, workbufsize, total_outsize, src_fs_s, original_insize;
    original_insize = bytes;

    inbuf = (uint8_t *)buffer;
    struct timespec TimeInfo = GetSystemTime(false);

#ifdef TEST_WITHOUT_BT
    if (mAudioHfpULTaskSinkdumpFile != NULL)
    {
        void* bufferNonConst = (void*)malloc(bytes);
        memcpy(bufferNonConst,buffer,bytes);
        AudioDumpPCMData(bufferNonConst , bytes, mAudioHfpULTaskSinkdumpFile);
        free(bufferNonConst);
    }

    return original_insize;

#endif //#ifdef TEST_WITHOUT_BT

    do
    {
        outbuffer = mAudioBTCVSDControl->BT_SCO_TX_GetCVSDOutBuf();
        outsize = SCO_TX_ENCODE_SIZE;
        insize = bytes;
        workbuf = mAudioBTCVSDControl->BT_SCO_TX_GetCVSDWorkBuf();
        workbufsize = SCO_TX_PCM64K_BUF_SIZE;
        src_fs_s = getSampleRate();//sample rate stored
        total_outsize = 0;

        do
        {
#if defined(__MSBC_CODEC_SUPPORT__)
            if (mAudioBTCVSDControl->BT_SCO_isWideBand())
            {
                mAudioBTCVSDControl->btsco_process_TX_MSBC(inbuf, &insize, outbuffer, &outsize, workbuf); //return insize is consumed size
                ALOGV("%s, do mSBC encode outsize=%d, consumed size=%d, bytes=%d", __func__, outsize, insize, (uint32_t)bytes);
            }
            else
#endif
            {
                mAudioBTCVSDControl->btsco_process_TX_CVSD(inbuf, &insize, outbuffer, &outsize, workbuf, workbufsize); //return insize is consumed size
                ALOGV("%s, do CVSD encode outsize=%d, consumed size=%d, bytes=%d", __func__, outsize, insize, (uint32_t)bytes);
            }

            outbuffer += outsize;
            inbuf += insize;
            bytes -= insize;
            insize = bytes;
            total_outsize += outsize;
        }
        while ((total_outsize < BTSCO_CVSD_TX_OUTBUF_SIZE) && (outsize != 0));

        if (total_outsize)
        {
#ifdef MTK_SUPPORT_BTCVSD_ALSA

            // check if timeout during write bt data
            bool skip_write = false;

            struct mixer_ctl *ctl = mixer_get_ctl_by_name(mMixer, "btcvsd_tx_timeout");
            int index = mixer_ctl_get_value(ctl, 0);
            bool tx_timeout = index != 0;
            ALOGV("%s(), btcvsd_tx_timeout, tx_timeout %d, index %d", __FUNCTION__, tx_timeout, index);
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
                }
            }
#else
            writtenBytes = ::write(mFdBluetooth, mAudioBTCVSDControl->BT_SCO_TX_GetCVSDOutBuf(), total_outsize);
            ALOGV("%s, write total_outsize: %u, BTSCO_CVSD_TX_OUTBUF_SIZE: %d writtenBytes: %d"
                , __func__, total_outsize, BTSCO_CVSD_TX_OUTBUF_SIZE, writtenBytes);
#endif

            //writtenBytes = ::write(mFdBluetooth, mAudioBTCVSDControl->BT_SCO_TX_GetCVSDOutBuf(), total_outsize);  //total_outsize should be BTSCO_CVSD_TX_OUTBUF_SIZE!!!
            //ALOGV("%s, write total_outsize: %u, BTSCO_CVSD_TX_OUTBUF_SIZE: %d writtenBytes: %d"
            //    , __func__, total_outsize, BTSCO_CVSD_TX_OUTBUF_SIZE, writtenBytes);
        }
    }
    while (bytes > 0);

    return original_insize;
}

uint32_t AudioHfpULTaskSink::getSampleRate()
{
    if (mAudioBTCVSDControl->BT_SCO_isWideBand())
    {
        return mBTWidebandSampleRate;
    }
    else
    {
        return mBTSampleRate;
    }
}

status_t AudioHfpULTaskSink::paramCheckAndUpdate(int format, uint32_t channelCount, uint32_t sampleRate, uint32_t accessSize)
{
    if (format != mAvailableFormat)
    {
        ALOGE("format not capatible, %i", format);
        return BAD_VALUE;
    }

    if (channelCount != 1 && channelCount != 2)
    {
        ALOGE("channelCount not capatible, %i", channelCount);
        return BAD_VALUE;
    }
    else
    {
        mChannelCount = channelCount;
    }

    mSampleRate = sampleRate;

    if (sampleRate != mBTSampleRate && sampleRate != mBTWidebandSampleRate)
    {
        ALOGE("sampleRate not capatible, %i", sampleRate);
        return BAD_VALUE;
    }

    if (sampleRate == mBTWidebandSampleRate)
    {
        mAudioBTCVSDControl->BT_SCO_SetMode(1);
    }
    else
    {
        mAudioBTCVSDControl->BT_SCO_SetMode(0);
    }

    (void)accessSize;

    return NO_ERROR;
}

}
