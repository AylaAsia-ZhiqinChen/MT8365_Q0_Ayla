#define LOG_TAG "AudioHfpDLTaskSource"
//#define LOG_NDEBUG 0

#include "AudioHfpDLTaskSource.h"
#include "AudioALSADeviceParser.h"
#include "AudioALSADriverUtility.h"

#include "AudioBTCVSDDef.h"
#include "AudioBTCVSDControl.h"
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
static bool mBTMode_Open;

AudioHfpDLTaskSource::AudioHfpDLTaskSource()
    :mIsRunning(false),
      mStarted(false),
      mChannelCount(2),
      mSampleRate(16000),
      mInterruptSample(160),
      mPcm(NULL),
      mMixer(AudioALSADriverUtility::getInstance()->getMixer()),
      mReadSignal(Condition::SHARED)
{
    ALOGD("%s()", __func__);
}

AudioHfpDLTaskSource::~AudioHfpDLTaskSource()
{
    if (mIsRunning) //should not go in here, check this just for safe
    {
        ALOGW("%s(), but we should not be here.", __func__);
        stop();
    }
}

status_t AudioHfpDLTaskSource::prepare(int format, uint32_t channelCount, uint32_t sampleRate, uint32_t accessSize)
{
    ALOGD("%s(), format=%d channelCount=%d sampleRate=%d accessSize=%d", __FUNCTION__,
        format, channelCount, sampleRate, accessSize );

    mSampleRate = sampleRate;
    mChannelCount = channelCount;
    mFormat = format;
    mInterruptSample = accessSize / (audio_bytes_per_sample((audio_format_t)format) * channelCount);

    return NO_ERROR;
}

status_t AudioHfpDLTaskSource::start()
{
    if (!mStarted) {
        AL_AUTOLOCK(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

        int pcmindex, cardindex;

#ifdef MTK_BT_AUDIO_USING_BTCVSD
        pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmBTCVSDCapture);
        cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmBTCVSDCapture);
#else
        pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmPcmRxCapture);
        cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmPcmRxCapture);
#endif

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

        ALOGD("%s(), pcmindex = %d, cardindex = %d", __FUNCTION__, pcmindex, cardindex);

        ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
            __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);

        if (mPcm != NULL)
            ALOGW("%s, mPcm != NULL before we request pcm interface! mPcm: %p", __FUNCTION__, mPcm);

        mPcm = pcm_open(cardindex, pcmindex, PCM_IN | PCM_MONOTONIC, &mConfig);
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

        mStarted = true;
    }

    if (mIsRunning)
    {
        ALOGW("%s() called but already running!", __func__);
        return INVALID_OPERATION;
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

    mAudioHfpDLTaskSourceThread = new AudioHfpDLTaskSourceThread(mRingBufWithLock,&mReadSignal,mChannelCount,mPcm,mMixer,mConfig,accessSize);
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

    do
    {
        {  //for Autolock
            Mutex::Autolock lock(mRingBufWithLock->mLock);
            RingBufferDataSize = RingBuf_getDataCount(&mRingBufWithLock->mRingBuf);
            ALOGV("%s, read RingBufferDataSize = %d TryCount = %d ReadDataBytes = %d mRingBuf = %p",
                __func__,RingBufferDataSize, TryCount, ReadDataBytes, &mRingBufWithLock->mRingBuf);
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
    ALOGD("+%s",__func__);
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
    return NO_ERROR;
}

int AudioHfpDLTaskSource::getRemainDataLatency()
{
    return INVALID_OPERATION;
}

//---------- implementation AudioHfpDLTaskSourceThread--------------

AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::AudioHfpDLTaskSourceThread
    (sp<RingBufWithLock> ringBufferWithLock, Condition* bufFillSignal,uint32_t channelCount,struct pcm *mPcm, struct mixer *mMixer, struct pcm_config mConfig, uint32_t accessSize)
    : mRingBufferWithLock(ringBufferWithLock),
      mBufferSize(ringBufferWithLock->mRingBuf.bufLen),
      mChannelCount(channelCount),
      mThreadPcm(mPcm),
      mThreadMixer(mMixer),
      mThreadPcmConfig(mConfig),
      mThreadAccessSize(accessSize),
      mBufFillSignal(bufFillSignal),
      mAudioBTCVSDControl(AudioBTCVSDControl::getInstance()),
      mBTIrqReceived(false),
      mReadBufferSize(0),
      mFd2(-1)
{
#ifdef MTK_BT_AUDIO_USING_BTCVSD
    if (mConfig.rate == 16000)
        mAudioBTCVSDControl->BT_SCO_SetMode(1);
    else if (mConfig.rate == 8000)
        mAudioBTCVSDControl->BT_SCO_SetMode(0);
    else
        ALOGE("%s() SampleRate ERROR", __FUNCTION__);

    if (mAudioBTCVSDControl->BT_SCO_isWideBand() == true) {
        mReadBufferSize = MSBC_PCM_FRAME_BYTE * 6 * 2; // 16k mono->48k stereo
    } else {
        mReadBufferSize = SCO_RX_PCM8K_BUF_SIZE * 12 * 2; // 8k mono->48k stereo
    }

    mAudioBTCVSDControl->BT_SCO_SET_RXState(BT_SCO_RXSTATE_IDLE);
    mAudioBTCVSDControl->BT_SCO_RX_Begin(mFd2);
#endif

#ifdef TEST_WITHOUT_BT
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
#ifdef TEST_WITHOUT_BT
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
        if(mChannelCount == 2)
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
#ifdef MTK_BT_AUDIO_USING_BTCVSD
        uint32_t Read_Size=0;
        char linear_buffer[ MSBC_PCM_FRAME_BYTE * 6 * 2];
        Read_Size = readDataFromBTCVSD((void *)linear_buffer);
        ALOGV("%s(),%u bytes has been read from BTCVSD!",__func__,Read_Size);
        copyDataToBuffer((void *)linear_buffer, Read_Size);
        mBTIrqReceived=false;
        return true;
#else
        uint32_t Read_Size = mThreadAccessSize;
        char linear_buffer[Read_Size];
        int ret = pcm_read(mThreadPcm, linear_buffer, Read_Size);
        if (ret != 0)
        {
	        ALOGE("%s pcm_read fail ret = %d(%s)", __FUNCTION__, ret, pcm_get_error(mThreadPcm));
	        return 0;
        }
        copyDataToBuffer((void *)linear_buffer, Read_Size);
        return true;
#endif
#endif
    }
    ALOGD("threadLoop exit");
    return false;
}

void AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::copyDataToBuffer(void *outbuf,uint32_t outsize)
{
    Mutex::Autolock lock(mRingBufferWithLock->mLock);
    uint32_t freeSpace = RingBuf_getFreeSpace(&(mRingBufferWithLock->mRingBuf));

    uint32_t outToBufferSize = outsize;
    char* outToBufferMemory = (char *)outbuf;
#ifdef MTK_BT_AUDIO_USING_BTCVSD
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
        ALOGV("%s() send signal", __FUNCTION__);
        mBufFillSignal->signal();
    }
    else
    {
        ALOGW("%s(), freeSpace(%u) < outToBufferSize(%u)", __FUNCTION__, freeSpace, outToBufferSize);
    }
#ifdef MTK_BT_AUDIO_USING_BTCVSD
    if (mChannelCount == 2)
    {
        delete[] outToBufferMemory;
    }
#endif
}

//---------- implementation AudioHfpDLTaskSourceThread end--------------

uint32 AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::readDataFromBTCVSD(void *linear_buffer)
{
    uint8_t packetvalid=0, *outbuf, *workbuf, *tempbuf, *inbuf;
    uint32_t i=0, Read_Size=0, outsize, workbufsize, insize, bytes=0,total_read_size = 0,offset=0;
    char isMuteData = false;
    struct timespec bufferTimeStamp;
    uint8_t *cvsd_raw_data = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf();
    ALOGV("+%s(), define uint8_t *cvsd_raw_data as %p.", __FUNCTION__,cvsd_raw_data);
    // make sure receiving bt irq before start reading
    if (!mBTIrqReceived) {
        if (pcm_prepare(mThreadPcm)) {
            ALOGE("%s(), pcm_prepare(%p) == false due to %s", __FUNCTION__, mThreadPcm, pcm_get_error(mThreadPcm));
        }

        struct mixer_ctl *ctl;
        unsigned int num_values, i;
        int index = 0;
        ctl = mixer_get_ctl_by_name(mThreadMixer, "BTCVSD Rx Irq Received Switch");
        num_values = mixer_ctl_get_num_values(ctl);
        for (i = 0; i < num_values; i++) {
            index = mixer_ctl_get_value(ctl, i);
            ALOGV("%s(), BTCVSD Rx Irq Received Switch, i = %d, index = %d", __FUNCTION__, i, index);
        }
        mBTIrqReceived = index != 0;
        ALOGV("%s(), +mBTIrqReceived=false,-mBTIrqReceived=%d,index=%d.",__func__,mBTIrqReceived,index);
    }

    float returnMuteDataMs = 0.0f;  /* If we cannot read data from kernel, return mute data first */
    // check if enough data in kernel
    struct timespec timeStamp;
    unsigned int avail;
    if (pcm_get_htimestamp(mThreadPcm, &avail, &timeStamp) != 0) {
        ALOGV("%s(), pcm_get_htimestamp fail %s\n", __FUNCTION__, pcm_get_error(mThreadPcm));
    } else {
        ALOGV("%s(), pcm_get_htimestamp succeed!\n", __FUNCTION__);
        if ((BTSCO_CVSD_RX_TEMPINPUTBUF_SIZE / (mThreadPcmConfig.channels * pcm_format_to_bits(mThreadPcmConfig.format) / 8)) > avail) {
            ALOGV("%s(), read size %u, avail %u, skip write",
                  __FUNCTION__, BTSCO_CVSD_RX_TEMPINPUTBUF_SIZE, avail);
            usleep(22500);
            ALOGV("-%s(),return for [avail] too few.", __FUNCTION__);
            return 0;
        }
    }
    if (mBTIrqReceived) {
        ALOGV("%s(), when mBTIrqReceived  is true:",__func__);
        int retval = pcm_read(mThreadPcm, cvsd_raw_data, BTSCO_CVSD_RX_TEMPINPUTBUF_SIZE);
        if (retval != 0) {
            // read fail, return mute data
            ALOGE("%s(), pcm_read() error, retval = %d, fail due to %s", __FUNCTION__, retval, pcm_get_error(mThreadPcm));
            returnMuteDataMs = 60.0f;
        } else {
            ALOGV("%s(), pcm_read() succeed.",__func__);
            // check if timeout during read bt data
            struct mixer_ctl *ctl = mixer_get_ctl_by_name(mThreadMixer, "BTCVSD Rx Timeout Switch");
            int index = mixer_ctl_get_value(ctl, 0);
            bool rx_timeout = index != 0;
            ALOGV("%s(), BTCVSD Rx Timeout Switch, rx_timeout %d, index %d", __FUNCTION__, rx_timeout, index);
            if(rx_timeout){
                // read timeout, return mute data
                ALOGE("%s(), rx_timeout %d, index %d, return mute data", __FUNCTION__, rx_timeout, index);
                returnMuteDataMs = 60.0f;
            }
        }
    }else{//IRQ not received,return mute data
        ALOGW("%s(), mBTIrqReceived = %d", __FUNCTION__, mBTIrqReceived);
        ALOGV("%s(), when mBTIrqReceived  is false:[ returnMuteDataMs = 22.5f].",__func__);
        returnMuteDataMs = 22.5f;

    }
    if(returnMuteDataMs != 0.0f){

        isMuteData = true;
        bufferTimeStamp = getCurrentTimeStamp();
        uint32_t muteBytes = mAudioBTCVSDControl->BT_SCO_isWideBand() ? returnMuteDataMs * 16 * 2 : returnMuteDataMs * 8 * 2;
        usleep(returnMuteDataMs * 1000);    // bt irq interval is 22.5ms
        memset(linear_buffer, 0, MSBC_PCM_FRAME_BYTE * 6 * 2);
        ALOGV("-%s(),return for [returnMuteDataMs=%f] not 0.0f.", __FUNCTION__, returnMuteDataMs);
        return muteBytes;
    }

    ALOGV("%s() isMuteData = %d, bufferTimeStamp = %ld.%09ld", __FUNCTION__, isMuteData, bufferTimeStamp.tv_sec, bufferTimeStamp.tv_nsec);

    tempbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDTempInBuf();
    workbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDWorkBuf();
    workbufsize = SCO_RX_PCM64K_BUF_SIZE;//480

    inbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDInBuf();
    insize = SCO_RX_PLC_SIZE;//30
    if (mAudioBTCVSDControl->BT_SCO_isWideBand() == true) {
        outbuf = mAudioBTCVSDControl->BT_SCO_RX_GetMSBCOutBuf();
        outsize = MSBC_PCM_FRAME_BYTE;//240
    } else {
        outbuf = mAudioBTCVSDControl->BT_SCO_RX_GetCVSDOutBuf();
        outsize = SCO_RX_PCM8K_BUF_SIZE;//60
    }
    ALOGV("inbuf=%p,insize=%u,outsize=%d,outbuf=%p,then we can infere BT_SCO_isWideBand() as [?].",inbuf,insize,outsize,outbuf);
    bytes = BTSCO_CVSD_RX_INBUF_SIZE;//480
    ALOGV("%s(),+ do_while loop:",__func__);
    do
    {
        memcpy(inbuf, cvsd_raw_data, SCO_RX_PLC_SIZE);
        cvsd_raw_data += SCO_RX_PLC_SIZE;

        packetvalid = *cvsd_raw_data; // parser packet valid info for each 30-byte packet
        //packetvalid = 1; // force packvalid to 1 for test
        cvsd_raw_data += BTSCO_CVSD_PACKET_VALID_SIZE;
        insize = SCO_RX_PLC_SIZE;
        outsize = (mAudioBTCVSDControl->BT_SCO_isWideBand() == true) ? MSBC_PCM_FRAME_BYTE : SCO_RX_PCM8K_BUF_SIZE;
        ALOGV("btsco_process_RX_CVSD/MSBC(+), insize = %d, outsize = %d, packetvalid = %u", insize, outsize, packetvalid);

        if (mAudioBTCVSDControl->BT_SCO_isWideBand() == true) {
            mAudioBTCVSDControl->btsco_process_RX_MSBC(inbuf, &insize, outbuf, &outsize, workbuf, packetvalid);
        } else {
            mAudioBTCVSDControl->btsco_process_RX_CVSD(inbuf, &insize, outbuf, &outsize, workbuf, workbufsize, packetvalid);
        }
        inbuf += SCO_RX_PLC_SIZE;
        bytes -= insize;
        ALOGV("btsco_process_RX_CVSD/MSBC(-), insize = %d, outsize = %d, bytes = %d", insize, outsize, bytes);
        if (outsize > 0) {
            if (total_read_size + outsize <= mReadBufferSize) {
                ALOGV(" outsize>0 &&total_read_size %u+outsize %u<=mReadBufferSize %u,bytes:%u,so copy linear_buffer to outbuf.", total_read_size, outsize, mReadBufferSize, bytes);
                memcpy(linear_buffer, outbuf, outsize);
        } else {
           ALOGE("%s(), total_read_size %u + outsize %u > mReadBufferSize %u, bytes %u", __FUNCTION__, total_read_size, outsize, mReadBufferSize, bytes);
           ASSERT(total_read_size + outsize <= mReadBufferSize);
           if (total_read_size < mReadBufferSize) {
              outsize = mReadBufferSize - total_read_size;
              memcpy(linear_buffer, outbuf, outsize);
              ALOGV("total_read_size < mReadBufferSize,so copy  linear_buffer to outbuf.");
           } else {
             ALOGV("total_read_size >= mReadBufferSize,so  break!! stop do-while loop.");
               break;
           }
        }
        linear_buffer = ((char *)linear_buffer) + outsize;
        total_read_size += outsize;
      }
    }while (bytes > 0 && total_read_size < mReadBufferSize);
    ALOGV("%s(),- do_while loop:",__func__);
    return total_read_size;
}

struct timespec AudioHfpDLTaskSource::AudioHfpDLTaskSourceThread::getCurrentTimeStamp() {
    struct timespec currentTime;

    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    ALOGV("%s(), timestamp = %ld.%09ld", __FUNCTION__, currentTime.tv_sec, currentTime.tv_nsec);

    return currentTime;
}

}
