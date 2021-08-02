#define LOG_TAG "AudioHfpDLTaskSource"
//#define LOG_NDEBUG 0

#include "AudioHfpDLTaskSource.h"
#include "AudioBTCVSDControl.h"
#include "WCNChipController.h"

#include "AudioALSADeviceString.h"
#include "AudioALSADeviceParser.h"
#include "AudioALSADriverUtility.h"
#include "AudioALSAHardwareResourceManager.h"

#define AUDIO_HFP_DL_SOURCE_BUFFER_SIZE (0x8000)

//#define TEST_WITHOUT_BT
#ifdef TEST_WITHOUT_BT
static char gZeroData[AUDIO_HFP_DL_SOURCE_BUFFER_SIZE];
FILE * fBtSource;
uint32_t dataSize, dataPos;
char * dataBuf;
#endif

namespace android
{
AudioHfpDLTaskSource::AudioHfpDLTaskSource()
    : mAudioBTCVSDControl(AudioBTCVSDControl::getInstance()),
      mIsUseMergeInterface(WCNChipController::GetInstance()->IsBTMergeInterfaceSupported()),
      mIsRunning(false),
      mPrepared(false),
      mChannelCount(0),
      mSampleRate(0),
      mStarted(false),
      mPcm(NULL),
      mReadSignal(Condition::SHARED)
{
    if (mIsUseMergeInterface)
    {
        ALOGW("%s() no merge interface support for HFP feature, we do nothing.", __func__);
        return;
    }
#if defined(MTK_CONSYS_MT8167)
    mAudioBTCVSDControl->BT_SCO_SET_RXState(BT_SCO_RXSTATE_IDLE);
#endif
}

AudioHfpDLTaskSource::~AudioHfpDLTaskSource()
{
    if (mIsRunning) //should not go in here, check this just for safe
    {
        ALOGW("%s(), but we should not be here.", __func__);
        stop();
    }
}

status_t AudioHfpDLTaskSource::prepare(int format, uint32_t channelCount,
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
        return INVALID_OPERATION;
    }

    //check parameter available or not & store
    status_t status = paramCheckAndUpdate(format, channelCount, sampleRate, accessSize);
    if (status)
    {
        ALOGE("prepare failed: status = %i", status);
        return status;
    }

    mInterruptSample = accessSize / (audio_bytes_per_sample((audio_format_t)format) * channelCount);

    //initial member data and library
    mPrepared = true;
    return NO_ERROR;
}

status_t AudioHfpDLTaskSource::start()
{
    if(mIsUseMergeInterface)
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
        ALOGW("%s() called but already running!", __func__);
        return INVALID_OPERATION;
    }

    if (!mStarted) {
        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());
#if defined(MTK_CONSYS_MT8167)
#ifdef MTK_SUPPORT_BTCVSD_ALSA
        memset(&mConfig, 0, sizeof(mConfig));
        mConfig.channels = mChannelCount;
        mConfig.rate = mSampleRate;
        mConfig.period_size = 1024;
        mConfig.period_count = 2;
        mConfig.format = PCM_FORMAT_S16_LE;
        mConfig.start_threshold = 1024;
        mConfig.stop_threshold = 0;
        mConfig.silence_threshold = 0;

        int pcmIdx = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmBTCVSDCapture);
        int cardIdx = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmBTCVSDCapture);

        ALOGD("%s(), mConfig: channels = %d, rate = %d, period_count = %d, period_size = %d, format = %d, pcmIdx = %d, cardIdx = %d",
                __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_count, mConfig.period_size, mConfig.format, pcmIdx, cardIdx);

        mPcm = pcm_open(cardIdx, pcmIdx, PCM_IN, &mConfig);
        if (mPcm == NULL)
        {
            ALOGE("%s(), mPcm == NULL!!", __FUNCTION__);
            ASSERT(mPcm != NULL);
            return UNKNOWN_ERROR;
        }
        else if (pcm_is_ready(mPcm) == false)
        {
            ALOGE("%s(), pcm_is_ready(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            ASSERT(pcm_is_ready(mPcm) == true);
            return UNKNOWN_ERROR;
        }

        mAudioBTCVSDControl->BT_SCO_RX_Begin(0);

        if (pcm_start(mPcm) != 0)
        {
            ALOGE("%s(), pcm_start(%p) fail due to %s", __FUNCTION__, mPcm, pcm_get_error(mPcm));
        }
#else
        mAudioBTCVSDControl->BT_SCO_RX_Begin(mAudioBTCVSDControl->getFd());
#endif
#else  // 8167 7668
        int pcmIdx = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl1AwbCapture);
        int cardIdx = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl1AwbCapture);

        //enable4PinMode(true);

        memset(&mConfig, 0, sizeof(mConfig));
        mConfig.channels = mChannelCount;
        mConfig.rate = mSampleRate;
        mConfig.period_count = 4;
        mConfig.period_size = mInterruptSample;

        mConfig.format = PCM_FORMAT_S16_LE;

        mConfig.start_threshold = 0;
        mConfig.stop_threshold = 0;
        mConfig.silence_threshold = 0;
        mConfig.silence_size = 0;
        mConfig.avail_min = 0;

        ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
            __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);

        if (mPcm != NULL)
            ALOGW("%s, mPcm != NULL before we request pcm interface! mPcm: %p", __FUNCTION__, mPcm);

        mPcm = pcm_open(cardIdx, pcmIdx, PCM_IN | PCM_MONOTONIC, &mConfig);
        if (mPcm == NULL)
        {
            ALOGE("%s(), mPcm == NULL!!", __FUNCTION__);
            return UNKNOWN_ERROR;
        }
        else if (pcm_is_ready(mPcm) == false)
        {
            ALOGE("%s(), pcm_is_ready(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
            pcm_close(mPcm);
            mPcm = NULL;
            return UNKNOWN_ERROR;
        }

        if (pcm_start(mPcm) != 0)
        {
            ALOGE("%s(), pcm_start(%p) fail due to %s", __FUNCTION__, mPcm, pcm_get_error(mPcm));
        }

#endif
        mStarted = true;
    }

    //create buffer
    mRingBufWithLock = new RingBufWithLock();
    memset((void *)&(mRingBufWithLock->mRingBuf), 0, sizeof(RingBuf));
    RingBuf* ringBuf = &(mRingBufWithLock->mRingBuf);

    ringBuf->pBufBase = new char[AUDIO_HFP_DL_SOURCE_BUFFER_SIZE];
    if (ringBuf->pBufBase  == NULL)
    {
        ALOGE("%s() ringBuf->pBufBase allocate fail", __func__);
    }
    ringBuf->bufLen = AUDIO_HFP_DL_SOURCE_BUFFER_SIZE;
    ringBuf->pRead = ringBuf->pBufBase;
    ringBuf->pWrite = ringBuf->pBufBase;

    uint32_t accessSize = audio_bytes_per_sample((audio_format_t)mFormat) * mChannelCount * mInterruptSample;

    mAudioHfpDLTaskSourceThread =
        new AudioHfpDLTaskSourceThread(mRingBufWithLock,&mReadSignal,mChannelCount,mPcm,mAudioBTCVSDControl,accessSize);
    if (!mAudioHfpDLTaskSourceThread.get())
    {
        ALOGE("%s() create AudioHfpDLTaskSourceThread failed!",__func__);
        return NO_MEMORY;
    }

    mAudioHfpDLTaskSourceThread->run("mAudioHfpDLTaskSourceThread");
    mIsRunning = true;
    return NO_ERROR;
}

ssize_t AudioHfpDLTaskSource::read(void *buffer, ssize_t bytes)
{
    ALOGV("+%s ", __FUNCTION__);
    if (mIsUseMergeInterface)
    {
        ALOGW("%s() no merge interface support for HFP feature, we do nothing.", __func__);
        return 0;
    }

    if (!mPrepared)
    {
        ALOGW("%s() called but not prepared!",__func__);
        return NO_INIT;
    }

    if (!mIsRunning)
    {
        ALOGW("%s() called but not starting yet!",__func__);
        return INVALID_OPERATION;
    }

    char *pWrite = (char *)buffer;
    ssize_t ReadDataBytes = bytes;
    ssize_t RingBufferDataSize;
    int TryCount=20;
    uint32_t ProcessedDataBytes;
    bool isBusy = false;

    do
    {
        {  //for Autolock
            Mutex::Autolock lock(mRingBufWithLock->mLock);
            RingBufferDataSize = RingBuf_getDataCount(&mRingBufWithLock->mRingBuf);
            ALOGV("%s, read RingBufferDataSize = %d TryCount = %d ReadDataBytes = %d mRingBuf = %p",
                __func__,(int)RingBufferDataSize, TryCount, (int)ReadDataBytes, &mRingBufWithLock->mRingBuf);
            if (RingBufferDataSize >= ReadDataBytes) // ring buffer is enough, copy & exit
            {
                RingBuf_copyToLinear((char *)pWrite, &mRingBufWithLock->mRingBuf, ReadDataBytes);
                ReadDataBytes = 0;
                break;
            }
            else // ring buffer is not enough, copy all data & wait
            {
                RingBuf_copyToLinear((char *)pWrite, &mRingBufWithLock->mRingBuf, RingBufferDataSize);
                ReadDataBytes -= RingBufferDataSize;
                pWrite += RingBufferDataSize;
                ALOGV("%s, wait signal",__func__);
                mReadSignal.waitRelative(mRingBufWithLock->mLock,milliseconds(300));
                ALOGV("%s, wake up",__func__);
            }
        }

        TryCount--;
    }
    while (ReadDataBytes && TryCount);

    return (bytes - ReadDataBytes);
}

status_t AudioHfpDLTaskSource::stop()
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

    //stop reading data
    int ret = mAudioHfpDLTaskSourceThread->requestExitAndWait();
    if (ret == WOULD_BLOCK)
    {
        mAudioHfpDLTaskSourceThread->requestExit();
    }
    mAudioHfpDLTaskSourceThread.clear();

    //destory thread and buffer
    {
        Mutex::Autolock lock(mRingBufWithLock->mLock);
        RingBuf* ringBuf = &(mRingBufWithLock->mRingBuf);
        if (ringBuf->pBufBase)
        {
            delete[] ringBuf->pBufBase;
        }
        ringBuf->bufLen = 0;
        ringBuf->pRead = NULL;
        ringBuf->pWrite = NULL;
    }
    mIsRunning = false;
    return NO_ERROR;
}

int AudioHfpDLTaskSource::getRemainDataLatency()
{
    return INVALID_OPERATION;
}

status_t AudioHfpDLTaskSource::paramCheckAndUpdate(int format, uint32_t channelCount, uint32_t sampleRate, uint32_t accessSize)
{
    if (format != mAvailableFormat)
    {
        ALOGE("format not capatible, %i",format);
        return BAD_VALUE;
    }

    if (channelCount != 1 && channelCount != 2)
    {
        ALOGE("channelCount not capatible, %i",channelCount);
        return BAD_VALUE;
    }
    else
    {
        mChannelCount = channelCount;
    }
    mSampleRate = sampleRate;
    mFormat = format;

    if (sampleRate != mBTSampleRate && sampleRate != mBTWidebandSampleRate)
    {
        ALOGE("sampleRate not capatible, %i",sampleRate);
        return BAD_VALUE;
    }
#if defined(MTK_CONSYS_MT8167)
    if (sampleRate == mBTWidebandSampleRate)
    {
        mAudioBTCVSDControl->BT_SCO_SetMode(1);
    }
    else
    {
        mAudioBTCVSDControl->BT_SCO_SetMode(0);
    }
#endif
    (void)accessSize;

    return NO_ERROR;
}

//---------- implementation AudioHfpDLTaskSourceThread--------------

AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::AudioHfpDLTaskSourceThread
    (sp<RingBufWithLock> ringBufferWithLock, Condition* bufFillSignal,uint32_t channelCount,struct pcm *mPcm,AudioBTCVSDControl *mAudioBTCVSDCtrl, uint32_t accessSize)
    : mRingBufferWithLock(ringBufferWithLock),
      mBufferSize(ringBufferWithLock->mRingBuf.bufLen),
      mChannelCount(channelCount),
      mFdBluetooth(0),
      mThreadPcm(mPcm),
      mThreadAccessSize(accessSize),
      mAudioBTCVSDControl(mAudioBTCVSDCtrl),
      mBufFillSignal(bufFillSignal)
{
#ifndef TEST_WITHOUT_BT

#else
    ALOGD("HFP Test without BT.");

    // Put a fake BT source data in /sdcard/Music/HfpTest.wav
    fBtSource= fopen("/sdcard/Music/HfpTest.wav", "rb");

    if (fBtSource == NULL)
    {
        ALOGW("Fail to fopen test wav file! Use zero data as BT source.");
        dataBuf = gZeroData;
        dataSize = AUDIO_HFP_DL_SOURCE_BUFFER_SIZE;
    }
    else
    {
        char * header = (char *) malloc( sizeof(char) * 44);
        fread(header, sizeof(char) * 44, 1, fBtSource);
        uint32_t sum = 0;
        for (int i=0 ; i<4 ; i++ )
        {
            sum |= ( (uint32_t)header[40 + i] & 0xff ) << i * 8;
        }
        free(header);
        dataSize = sum;

        dataBuf = (char *) malloc(sizeof(char) * dataSize);
        fread(dataBuf, sizeof(char) * dataSize, 1, fBtSource);
    }

    dataPos = 0;
    //ALOGD("dataSize = %u", dataSize);
#endif
}

AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::~AudioHfpDLTaskSourceThread()
{
#ifndef TEST_WITHOUT_BT
#if defined(MTK_CONSYS_MT8167)
    mAudioBTCVSDControl->BT_SCO_RX_End(mFdBluetooth);
#endif
#else
    free(dataBuf);
    fclose(fBtSource);
#endif
}

status_t AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::readyToRun()
{
    //nothing to do
    return NO_ERROR;
}

void AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::onFirstRef()
{
    //nothing to do
}

bool AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::threadLoop()
{
    while (!(exitPending() == true))
    {
#ifdef TEST_WITHOUT_BT
        //ALOGV("%s() test without BT", __FUNCTION__);

        uint32_t writeMs = 20; //magic number for test purpose
        uint32_t sizeToWrite = writeMs * 8 * 2 * 2; // (ms / 1000) * 8000(Hz) * stereo * (16bits/2)
        if(mAudioBTCVSDControl->BT_SCO_isWideBand())
        {
            sizeToWrite *= 2;
        }

        if (dataPos + sizeToWrite <= dataSize)
        {
            copyDataToBuffer((void *)(dataBuf + dataPos), sizeToWrite);
            dataPos += sizeToWrite;
        }
        else
        {
            //ALOGD("Handle end of buffer: looping");
            copyDataToBuffer((void *)(dataBuf + dataPos), dataSize - dataPos);
            copyDataToBuffer((void *)dataBuf, sizeToWrite - dataSize + dataPos);
            dataPos = sizeToWrite - dataSize + dataPos;
        }
        usleep(writeMs*1000); // for simulate waiting BT
#else

#if defined(MTK_CONSYS_MT8167)
#if defined(__MSBC_CODEC_SUPPORT__)
        if (mAudioBTCVSDControl->BT_SCO_isWideBand())
        {
            btsco_mSBC_RX_main();
        }
        else
#endif
        {
            btsco_cvsd_RX_main();
        }
#else // 8167 7668
        uint32_t Read_Size = mThreadAccessSize;
        char linear_buffer[Read_Size];

        int ret = pcm_read(mThreadPcm, linear_buffer, Read_Size);

		ALOGV("%s AudioHfpDLTaskSource pcm_read ret = %d ,read size=%d .", __FUNCTION__, ret,Read_Size);

        if (ret != 0)
        {
            ALOGE("%s pcm_read fail ret = %d(%s)", __FUNCTION__, ret, pcm_get_error(mThreadPcm));
            return 0;
        }

        copyDataToBuffer((void *)linear_buffer, Read_Size);
#endif
        return true;
#endif //TEST_WITHOUT_BT
    }
    ALOGD("threadLoop exit");
    return false;
}

void AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::btsco_cvsd_RX_main()
{
    uint8_t packetvalid, *outbuf, *workbuf, *tempbuf, *inbuf;
    uint32_t i, Read_Size, outsize, workbufsize, insize, bytes, offset;

    ALOGV("btsco_cvsd_RX_main(+)");

#ifdef MTK_SUPPORT_BTCVSD_ALSA
    ASSERT(mThreadPcm != NULL);
    uint8_t *cvsd_raw_data = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf();
    int retval = pcm_read(mThreadPcm, cvsd_raw_data, BTSCO_CVSD_RX_TEMPINPUTBUF_SIZE);
    if (retval != 0) {
        ALOGE("%s(), pcm_read() error, retval = %d", __FUNCTION__, retval);
    }
#else
    Read_Size = ::read(mFdBluetooth, mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf(), BTSCO_CVSD_RX_TEMPINPUTBUF_SIZE);
    ALOGV("btsco_cvsd_RX_main ::read() done Read_Size=%d", Read_Size);
#endif

    //Read_Size = ::read(mFdBluetooth, mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf(), BTSCO_CVSD_RX_TEMPINPUTBUF_SIZE);
    //ALOGV("btsco_cvsd_RX_main ::read() done Read_Size=%d", Read_Size);

    outbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDOutBuf();
    outsize = SCO_RX_PCM8K_BUF_SIZE;
    workbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDWorkBuf();
    workbufsize = SCO_RX_PCM64K_BUF_SIZE;
    tempbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf();
    inbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDInBuf();
    insize = SCO_RX_PLC_SIZE;
    bytes = BTSCO_CVSD_RX_INBUF_SIZE;
    i = 0;
    offset = 0;
    do
    {
        packetvalid = *((char *)tempbuf + SCO_RX_PLC_SIZE + offset + i * BTSCO_CVSD_PACKET_VALID_SIZE); //parser packet valid info for each 30-byte packet
        memcpy(inbuf + offset, tempbuf + offset + i * BTSCO_CVSD_PACKET_VALID_SIZE, SCO_RX_PLC_SIZE);

        mAudioBTCVSDControl->btsco_process_RX_CVSD(inbuf + offset, &insize, outbuf, &outsize, workbuf, workbufsize, packetvalid);
        offset += SCO_RX_PLC_SIZE;
        bytes -= insize;
        copyDataToBuffer((void *)outbuf, outsize);

        outsize = SCO_RX_PCM8K_BUF_SIZE;
        insize = SCO_RX_PLC_SIZE;
        i++;
    }
    while (bytes > 0);


}

void AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::btsco_mSBC_RX_main(void)
{
    uint8_t packetvalid, *outbuf, *workbuf, *tempbuf, *inbuf;
    uint32_t i, Read_Size, outsize, workbufsize, insize, bytes, offset;

    ALOGV("btsco_mSBC_RX_main(+)");

#ifdef MTK_SUPPORT_BTCVSD_ALSA
    ASSERT(mThreadPcm != NULL);
    uint8_t *cvsd_raw_data = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf();
    int retval = pcm_read(mThreadPcm, cvsd_raw_data, BTSCO_CVSD_RX_TEMPINPUTBUF_SIZE);
    if (retval != 0) {
        ALOGE("%s(), pcm_read() error, retval = %d", __FUNCTION__, retval);
    }
#else
    Read_Size = ::read(mFdBluetooth, mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf(), BTSCO_CVSD_RX_TEMPINPUTBUF_SIZE);
    ALOGV("%s, Read_Size = %u",__func__,Read_Size);
#endif

    //Read_Size = ::read(mFdBluetooth, mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf(), BTSCO_CVSD_RX_TEMPINPUTBUF_SIZE);
    //ALOGV("%s, Read_Size = %u",__func__,Read_Size);
    outbuf = mAudioBTCVSDControl->BT_SCO_RX_GetMSBCOutBuf();
    outsize = MSBC_PCM_FRAME_BYTE;
    workbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDWorkBuf();
    workbufsize = SCO_RX_PCM64K_BUF_SIZE;
    tempbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf();
    inbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDInBuf();
    insize = SCO_RX_PLC_SIZE;
    bytes = BTSCO_CVSD_RX_INBUF_SIZE;
    i = 0;
    offset = 0;
    do
    {
        outsize = MSBC_PCM_FRAME_BYTE;
        insize = SCO_RX_PLC_SIZE;

        packetvalid = *((char *)tempbuf + SCO_RX_PLC_SIZE + offset + i * BTSCO_CVSD_PACKET_VALID_SIZE); //parser packet valid info for each 30-byte packet

        memcpy(inbuf + offset, tempbuf + offset + i * BTSCO_CVSD_PACKET_VALID_SIZE, SCO_RX_PLC_SIZE);

        ALOGV("btsco_process_RX_MSBC(+) insize=%d,outsize=%d,packetvalid=%d ", insize, outsize, packetvalid);
        mAudioBTCVSDControl->btsco_process_RX_MSBC(inbuf + offset, &insize, outbuf, &outsize, workbuf, packetvalid);
        offset += SCO_RX_PLC_SIZE;
        bytes -= insize;
        ALOGV("btsco_process_RX_MSBC(-) consumed=%d,outsize=%d, bytes=%d", insize, outsize, bytes);

        if (outsize != 0)
        {
            copyDataToBuffer((void *)outbuf, outsize);
        }

        i++;
    }
    while (bytes > 0);
    ALOGV("btsco_mSBC_RX_main(-)");
}

void AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::copyDataToBuffer(void *outbuf,uint32_t outsize)
{
    Mutex::Autolock lock(mRingBufferWithLock->mLock);
    uint32_t freeSpace = RingBuf_getFreeSpace(&(mRingBufferWithLock->mRingBuf));

    uint32_t outToBufferSize = outsize;
    char* outToBufferMemory = (char *)outbuf;
#if defined(MTK_CONSYS_MT8167)
    if (mChannelCount == 2) //outbuf is mono, need to copy to stereo
    {
        uint32_t outBufStereoSize = outsize * mChannelCount;
        char* outputBufStereo = new char[outBufStereoSize];

        uint32_t sampleCount = outsize / audio_bytes_per_sample((audio_format_t)mAvailableFormat);

        short* bufIter = (short*) outputBufStereo;
        short* oriBufIter = (short*) outbuf;

        for(uint32_t i = 0; i < sampleCount; i++)
        {
            *bufIter = *oriBufIter;
            ++bufIter;
            *bufIter = *oriBufIter;
            ++bufIter;
            ++oriBufIter;
        }

        outToBufferSize = outBufStereoSize;
        outToBufferMemory = outputBufStereo;
    }
#endif
    if (freeSpace >= outToBufferSize)
    {
        ALOGV("%s RingBuf_copyToLinear freeSpace = %u outToBufferSize = %u mRingBuf = %p",
            __func__, freeSpace, outToBufferSize, &mRingBufferWithLock->mRingBuf);
        RingBuf_copyFromLinear(&mRingBufferWithLock->mRingBuf, outToBufferMemory, outToBufferSize);
        ALOGV("%s send signal",__func__);
        mBufFillSignal->signal();
    }
    else
    {
        ALOGW("%s(), freeSpace(%u) < outToBufferSize(%u)", __FUNCTION__, freeSpace, outToBufferSize);
    }
#if defined(MTK_CONSYS_MT8167)
    if (mChannelCount == 2)
    {
        delete[] outToBufferMemory;
    }
#endif
}

//---------- implementation AudioHfpDLTaskSourceThread end--------------

}
