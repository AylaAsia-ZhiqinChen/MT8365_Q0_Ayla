/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#define LOG_TAG "AudioUtilmtk"
#include <media/AudioUtilmtk.h>
#include <media/AudioParameter.h>
#include "AudioParamParser.h"

#define MTK_LOG_ENABLE 1
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <cutils/log.h>
#include <cutils/properties.h>

#include <utils/Vector.h>
#include <utils/SortedVector.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>

namespace android {
KeyedVector<AudioDump::PROP_AUDIO_DUMP, int> mAudioDumpKeyEnableVector;
Mutex mAudioDumpKeyMutex;
static   const char * af_dump_log = "vendor.af.dumplog";

const char * const AudioDump::keyAudioDumpMixer = "vendor.af.mixer.pcm";
const char * const AudioDump::keyAudioDumpTrack = "vendor.af.track.pcm";
const char * const AudioDump::keyAudioDumpOffload = "vendor.af.offload.write.raw";
const char * const AudioDump::keyAudioDumpResampler = "vendor.af.resampler.pcm";
const char * const AudioDump::keyAudioDumpMixerEnd = "vendor.af.mixer.end.pcm";
const char * const AudioDump::keyAudioDumpRecord = "vendor.af.record.dump.pcm";
const char * const AudioDump::keyAudioDumpEffect = "vendor.af.effect.pcm";
const char * const AudioDump::keyAudioDumpDrc = "vendor.af.mixer.drc.pcm";
const char * const AudioDump::keyAudioDumpLog = "vendor.af.dumplog";
const char * const AudioDump::keyAudioDumpAAudio = "vendor.aaudio.pcm";

const char * AudioDump::audioDumpPropertyStr[] = {
    AudioDump::keyAudioDumpMixer,
    AudioDump::keyAudioDumpTrack,
    AudioDump::keyAudioDumpOffload,
    AudioDump::keyAudioDumpResampler,
    AudioDump::keyAudioDumpMixerEnd,
    AudioDump::keyAudioDumpRecord,
    AudioDump::keyAudioDumpEffect,
    AudioDump::keyAudioDumpDrc,
    AudioDump::keyAudioDumpLog,
    AudioDump::keyAudioDumpAAudio,
};

const char * const AudioDump::af_track_pcm = "/sdcard/debuglogger/audio_dump/af_track_pcm";
const char * const AudioDump::af_mixer_pcm = "/sdcard/debuglogger/audio_dump/af_mixer_pcm.pcm";
const char * const AudioDump::af_mixer_write_pcm = "/sdcard/debuglogger/audio_dump/af_mixer_write_pcm.pcm";
const char * const AudioDump::af_mixer_end_pcm = "/sdcard/debuglogger/audio_dump/mixer_end";
const char * const AudioDump::af_offload_write_raw = "/sdcard/debuglogger/audio_dump/af_offload_write_pcm.raw";
const char * const AudioDump::af_record_read_pcm = "/sdcard/debuglogger/audio_dump/af_record_read.pcm";
const char * const AudioDump::af_record_convert_pcm = "/sdcard/debuglogger/audio_dump/af_record_convert.pcm";
const char * const AudioDump::af_effect_pcm = "/sdcard/debuglogger/audio_dump/af_effect.pcm";
const char * const AudioDump::af_mixer_drc_pcm_before = "/sdcard/debuglogger/audio_dump/mixer_drc_before";
const char * const AudioDump::af_mixer_drc_pcm_after = "/sdcard/debuglogger/audio_dump/mixer_drc_after";
const char * const AudioDump::af_resampler_in_pcm = "/sdcard/debuglogger/audio_dump/af_mixer_resampler_in.pcm";
const char * const AudioDump::af_resampler_out_pcm = "/sdcard/debuglogger/audio_dump/af_mixer_resampler_out.pcm";

const char * const AudioDump::aaudio_share_dl = "/sdcard/debuglogger/audio_dump/aaudio_share_dl";
const char * const AudioDump::aaudio_share_ul = "/sdcard/debuglogger/audio_dump/aaudio_share_ul";
const char * const AudioDump::aaudio_exclusive_dl = "/sdcard/debuglogger/audio_dump/aaudio_exclusive_dl";
const char * const AudioDump::aaudio_exclusive_ul = "/sdcard/debuglogger/audio_dump/aaudio_exclusive_ul";


bool FeatureOption::MTK_AUDIOMIXER_ENABLE_DRC = false;
int FeatureOption:: MTK_ENABLE_STEREO_SPEAKER = 0;
bool FeatureOption::MTK_USB_PHONECALL = false;
bool FeatureOption::MTK_TTY_SUPPORT = false;
bool FeatureOption::MTK_HIFIAUDIO_SUPPORT = false;
bool FeatureOption::MTK_BESLOUDNESS_ENABLE = false;
bool FeatureOption::MTK_AUDIO_GAIN_TABLE = false;
bool FeatureOption::MTK_AUDIO_GAIN_NVRAM = false;
bool FeatureOption::MTK_FM_ENABLE = false;
bool FeatureOption::MTK_AURISYS_FRAMEWORK_SUPPORT = false;


const char * const FeatureOption::foName_MTK_BESLOUDNESS_RUN_WITH_HAL = "MTK_BESLOUDNESS_RUN_WITH_HAL";
const char * const FeatureOption::foName_MTK_BESLOUDNESS_SUPPORT = "MTK_BESLOUDNESS_SUPPORT";
const char * const FeatureOption::foName_MTK_AUDIO_NUMBER_OF_SPEAKER = "MTK_AUDIO_NUMBER_OF_SPEAKER";
const char * const FeatureOption::foName_MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT = "MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT";
const char * const FeatureOption::foName_MTK_HIFIAUDIO_SUPPORT = "MTK_HIFIAUDIO_SUPPORT";
const char * const FeatureOption::foName_MTK_AURISYS_FRAMEWORK_SUPPORT = "MTK_AURISYS_FRAMEWORK_SUPPORT";
const char * const FeatureOption::foName_MTK_USB_PHONECALL = "MTK_USB_PHONECALL";
const char * const FeatureOption::foName_MTK_TTY_SUPPORT = "MTK_TTY_SUPPORT";
const char * const FeatureOption::foName_MTK_FM_SUPPORT = "MTK_FM_SUPPORT";


struct WAVEFORMATEX {
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
    uint16_t cbSize;

    WAVEFORMATEX() {
        wFormatTag      = 0;
        nChannels       = 0;
        nSamplesPerSec  = 0;
        nAvgBytesPerSec = 0;
        nBlockAlign     = 0;
        wBitsPerSample  = 0;
        cbSize          = 0;
    }
};

struct WavFormatHeader
{
    char        ckID[5];           // 4 : Chunk ID: "RIFF"
    uint32_t    cksize;            // 4: Chunk size: File length - 8
    char        WAVEID[5];         // 4: WAVE ID: "WAVE"
    // Format Chunk
    char        FormatckID[5];     // 4: "fmt "
    uint32_t    Formatcksize;      // 4: Chunk size: 16 or 18 or 40 ( We will use 18, no extensiable format. )
    char        DataID[5];         // 4: "data"
    uint32_t    Datacksize;        // 4: Chunk size: Data Size
    WAVEFORMATEX WaveFormatEx;

    WavFormatHeader()
    :   cksize(0),
        Formatcksize(18),
        Datacksize(0)
    {
        strncpy(ckID, "RIFF", sizeof(ckID));
        strncpy(WAVEID, "WAVE", sizeof(WAVEID));
        strncpy(FormatckID, "fmt ", sizeof(FormatckID));
        strncpy(DataID, "data", sizeof(DataID));
    }
};

// This structure will be written to the file header
struct AudioDumpFileInfo
{
    audio_format_t format;
    uint32_t sampleRate;
    uint32_t channelCount;
    int size;

    AudioDumpFileInfo()
    {
        format = AUDIO_FORMAT_INVALID;
        sampleRate = 0;
        channelCount = 0;
        size = 0;
    }
};

struct AudioDumpBuffer
{
    void *pBufBase;
    int changeCount;  // The serial number of the file
    int allocBufSize;
    AudioDumpFileInfo fileInfo;

    AudioDumpBuffer()
    {
        pBufBase = NULL;
        changeCount = 0;
        allocBufSize = 0;
    }
};

#define AUDIO_DUMP_BUFFER_COUNT_MAX 32 // How many buffer will combine

pthread_t hAudioDumpThread = 0;
pthread_cond_t  AudioDataNotifyEvent;
pthread_mutex_t AudioDataNotifyMutex;

Mutex mAudioDumpMutex;
Mutex mAudioDumpFileMutex;

KeyedVector<String8, Vector<AudioDumpBuffer *>* > mAudioDumpFileVector; ///< The queue buffer waiting for write
///< The first element of Vector<AudioDumpBuffer *> is the previous buffer info.

KeyedVector<String8, Vector<AudioDumpBuffer *>* > mAudioDumpQueueVector;

#define AUDIO_DUMP_FILE_DELAY_TIME_KEY "audiodump.filedelaytime"
int mAudioDumpSleepTime = 2;
uint32_t mAudioDumpFileIoDelayTime = 0;

FILE* fopen_rb(String8 filePath)
{
    FILE * fp = fopen(filePath.string(), "rb+");
    if (mAudioDumpFileIoDelayTime > 0) {
        usleep(mAudioDumpFileIoDelayTime * 1000);
    }

    if (fp == NULL) {
        ALOGV("fopen_rb() file(%s) rb+ fail, open with ab+", filePath.string());
        fp= fopen(filePath.string(), "ab+");
        if (mAudioDumpFileIoDelayTime > 0) {
            usleep(mAudioDumpFileIoDelayTime * 1000);
        }
    }

    if (fp == NULL) {
        // ALOGE("fopen_rb() file(%s) fail", filePath.string());
        return NULL;
    }

    return fp;
}

size_t fwrite_s(const void * ptr, size_t size, size_t count, FILE * stream)
{
    size_t ret = fwrite(ptr, size, count, stream);
    if (mAudioDumpFileIoDelayTime > 0) {
        usleep(mAudioDumpFileIoDelayTime * 1000);
    }
    return ret;
}

void fseek_s(FILE * stream, long int offset, int origin)
{
    int ret = fseek(stream, offset, origin);
    if (ret == 0 && mAudioDumpFileIoDelayTime > 0) {
        usleep(mAudioDumpFileIoDelayTime * 1000);
    }
}

size_t fread_s (void * ptr, size_t size, size_t count, FILE * stream)
{
    size_t ret = fread(ptr, size, count, stream);
    if (ret > 0 && mAudioDumpFileIoDelayTime > 0) {
        usleep(mAudioDumpFileIoDelayTime * 1000);
    }
    return ret;
}

void WriteNewWaveHeader(FILE *fp, AudioDumpBuffer audioBuffer)
{
    WavFormatHeader wavHeader;
    void* tmpBuffer = malloc(46);
    if(!tmpBuffer) {
        ALOGE("%s(): malloc fail!", __FUNCTION__);
        return;
    }

    if (audioBuffer.fileInfo.format == AUDIO_FORMAT_PCM_FLOAT) {
        wavHeader.WaveFormatEx.wFormatTag = 3; // IEEE Float
        wavHeader.WaveFormatEx.wBitsPerSample = 32;
    } else {
        wavHeader.WaveFormatEx.wFormatTag = 1; // PCM

        if (audioBuffer.fileInfo.format == AUDIO_FORMAT_PCM_8_BIT) {
            wavHeader.WaveFormatEx.wBitsPerSample = 8;
        } else if (audioBuffer.fileInfo.format == AUDIO_FORMAT_PCM_16_BIT) {
            wavHeader.WaveFormatEx.wBitsPerSample = 16;
        } else if (audioBuffer.fileInfo.format == AUDIO_FORMAT_PCM_24_BIT_PACKED) {
            wavHeader.WaveFormatEx.wBitsPerSample = 24;
        } else if (audioBuffer.fileInfo.format == AUDIO_FORMAT_PCM_8_24_BIT ||
                   audioBuffer.fileInfo.format == AUDIO_FORMAT_PCM_32_BIT) {
            wavHeader.WaveFormatEx.wBitsPerSample = 32;
        }
    }
    wavHeader.cksize                       = audioBuffer.fileInfo.size + 38; // 46 - 8
    wavHeader.Datacksize                   = audioBuffer.fileInfo.size;
    wavHeader.WaveFormatEx.nChannels       = audioBuffer.fileInfo.channelCount;
    wavHeader.WaveFormatEx.nSamplesPerSec  = audioBuffer.fileInfo.sampleRate;
    wavHeader.WaveFormatEx.nAvgBytesPerSec = wavHeader.WaveFormatEx.nSamplesPerSec * wavHeader.WaveFormatEx.nChannels
                                               * wavHeader.WaveFormatEx.wBitsPerSample / 8;
    wavHeader.WaveFormatEx.nBlockAlign     = wavHeader.WaveFormatEx.nChannels * wavHeader.WaveFormatEx.wBitsPerSample / 8;
    wavHeader.WaveFormatEx.cbSize          = 0;

    int pos = 0;
    memcpy((char*)tmpBuffer + pos, &wavHeader.ckID,            4); pos += 4;
    memcpy((char*)tmpBuffer + pos, &wavHeader.cksize,          4); pos += 4;
    memcpy((char*)tmpBuffer + pos, &wavHeader.WAVEID,          4); pos += 4;
    memcpy((char*)tmpBuffer + pos, &wavHeader.FormatckID,      4); pos += 4;
    memcpy((char*)tmpBuffer + pos, &wavHeader.Formatcksize,    4); pos += 4;
    memcpy((char*)tmpBuffer + pos, &wavHeader.WaveFormatEx,   18); pos += 18;
    memcpy((char*)tmpBuffer + pos, &wavHeader.DataID,          4); pos += 4;
    memcpy((char*)tmpBuffer + pos, &wavHeader.Datacksize,      4); pos += 4;

    fwrite_s(tmpBuffer, 46, 1, fp);
    free(tmpBuffer);
    tmpBuffer = NULL;
}

void UpdateWaveHeader(FILE *fp, AudioDumpBuffer audioBuffer)
{
    fseek_s(fp, 0, SEEK_END);
    if (ftell(fp) == 0) {
        WriteNewWaveHeader(fp, audioBuffer);
    } else {
        WavFormatHeader wavHeader;
        void* tmpBuffer = malloc(46);
        if(!tmpBuffer) {
            ALOGE("%s(): malloc fail!", __FUNCTION__);
            return;
        }

        fseek_s(fp, 0, SEEK_SET);
        if (fread_s((char*)tmpBuffer, 46, 1, fp) <= 0) {
            free(tmpBuffer);
            tmpBuffer = NULL;
            return;
        }

        memcpy((char*)(&wavHeader.cksize), (char*)tmpBuffer + 4, 4);
        wavHeader.cksize += audioBuffer.fileInfo.size;
        memcpy((char*)tmpBuffer + 4, &wavHeader.cksize, 4);

        memcpy((char*)(&wavHeader.Datacksize), (char*)tmpBuffer + 42, 4);
        wavHeader.Datacksize += audioBuffer.fileInfo.size;
        memcpy((char*)tmpBuffer + 42, &wavHeader.Datacksize, 4);

        fseek_s(fp, 0, SEEK_SET);
        fwrite_s((char*)tmpBuffer, 46, 1, fp);

        free(tmpBuffer);
        tmpBuffer = NULL;
    }
}

void UpdateWaveHeader_f(String8 filePath, AudioDumpBuffer audioBuffer)
{
    FILE *fp = fopen_rb(filePath);
    if (fp != NULL) {
        //ALOGD("UpdateWaveHeader_f START filePath(%s)", filePath.string());
        UpdateWaveHeader(fp, audioBuffer);
        fclose(fp);
    }
}

void WriteAudioBuffer(FILE *fp, AudioDumpBuffer audioBuffer)
{
    fwrite_s(audioBuffer.pBufBase, audioBuffer.fileInfo.size, 1, fp);
    //ALOGD("free %p, %d", audioBuffer.pBufBase, audioBuffer.fileInfo.size);
    free(audioBuffer.pBufBase);
    audioBuffer.pBufBase = NULL;
}

void UpdateAllWaveHeader()
{
    //int FileVectorSize = mAudioDumpFileVector.size();
    for (size_t i = 0; i < mAudioDumpFileVector.size(); i++) {
        String8 filePathPCM = mAudioDumpFileVector.keyAt(i);
        Vector<AudioDumpBuffer *>*pvector = (mAudioDumpFileVector.valueAt(i));
        int BufferVectorSize = (*pvector).size();

        if (BufferVectorSize == 1) { // Only Header info.
            String8 filePathWav = filePathPCM;

            AudioDumpBuffer *pLastBufferInfo = (*pvector)[0];
            if (pLastBufferInfo->changeCount == 0) {
                filePathWav = String8::format("%s.wav", filePathPCM.string());
            } else {
                filePathWav = String8::format("%s.%d.wav", filePathPCM.string(), pLastBufferInfo->changeCount);
            }

            UpdateWaveHeader_f(filePathWav, *pLastBufferInfo);

            bool bDeleted = false;
            if ((*pvector).size() == 1) {
                (*pvector).removeAt(0);
                delete pLastBufferInfo;
                pLastBufferInfo = NULL;

                mAudioDumpFileVector.removeItem(filePathPCM);
                delete pvector;
                pvector = NULL;

                bDeleted = true;

                i--;
            }

            if (bDeleted) {
                //break; // Break cause the vector size is changed.
            }
        }
    }
}

void PushBufferFromeQueueToVector(String8 filePath, AudioDumpBuffer *newDumpBuffer)
{
    AudioDumpBuffer *newInBuffer = newDumpBuffer;
    Vector<AudioDumpBuffer *> *pDumpBufferVector = NULL;

    ssize_t index = mAudioDumpFileVector.indexOfKey(filePath);
    if (index < 0) {
        pDumpBufferVector = new Vector<AudioDumpBuffer *>;
        mAudioDumpFileVector.add(filePath, pDumpBufferVector);
    } else {
        pDumpBufferVector = mAudioDumpFileVector.valueAt(index);
    }

    if (pDumpBufferVector->size() == 0) { // No previous buffer info, create a new one.
        AudioDumpBuffer *lastInfoBuffer = new AudioDumpBuffer();
        memcpy(lastInfoBuffer, newInBuffer, sizeof(AudioDumpBuffer));
        lastInfoBuffer->pBufBase = NULL;   // We don't care this parameter in last info.
        lastInfoBuffer->fileInfo.size = 0; // The size that had writtn to file. Set to 0 here since we havn't write anything.
        pDumpBufferVector->add(lastInfoBuffer);
    }
    pDumpBufferVector->add(newInBuffer);
}

void ProcessBufferQueue(bool bFlush)
{
    if (AUDIO_DUMP_BUFFER_COUNT_MAX == 1) {
        bFlush = true;
    }
    Vector<AudioDumpBuffer *> *pQueueBufferVector = NULL;
    Vector<String8> keyVector;
    mAudioDumpMutex.lock();
    int size = mAudioDumpQueueVector.size();
    for (int i = 0; i < size; i++) {
        String8 key = mAudioDumpQueueVector.keyAt(i);
        keyVector.add(key);
    }
    mAudioDumpMutex.unlock();

    for (size_t i = 0; i < keyVector.size(); i++) {
        int allocBufSize = 0;
        int currBufSize = 0;
        int mergeBufSize = 0;

        mAudioDumpMutex.lock();
        pQueueBufferVector = mAudioDumpQueueVector.valueFor(keyVector[i]);
        int queueSize = (*pQueueBufferVector).size();
        mAudioDumpMutex.unlock();

        while (queueSize >= 2) {
            mAudioDumpMutex.lock();
            AudioDumpBuffer *newQueueBuffer = (*pQueueBufferVector)[0];
            AudioDumpBuffer *newQueueBuffer2 = (*pQueueBufferVector)[1];
            mAudioDumpMutex.unlock();

            if (memcmp(&(newQueueBuffer->fileInfo),
                &(newQueueBuffer2->fileInfo),
                sizeof(AudioDumpFileInfo) - sizeof(int)) != 0) { // Compare format/sample rate/channel, excluding size
                PushBufferFromeQueueToVector(keyVector[i], newQueueBuffer);

                mAudioDumpMutex.lock();
                pQueueBufferVector->removeAt(0);
                mAudioDumpMutex.unlock();


            } else {
                currBufSize = newQueueBuffer->fileInfo.size;
                mergeBufSize = newQueueBuffer2->fileInfo.size;

                // allocate a big buffer to merge data
                if (newQueueBuffer->allocBufSize == 0) {
                    void *pBufTemp = newQueueBuffer->pBufBase;

                    allocBufSize = currBufSize * AUDIO_DUMP_BUFFER_COUNT_MAX;
                    newQueueBuffer->allocBufSize = allocBufSize;
                    newQueueBuffer->pBufBase = (char*)malloc(allocBufSize);

                    if (newQueueBuffer->pBufBase != NULL) {
                        memcpy((char*)newQueueBuffer->pBufBase, pBufTemp, currBufSize);
                    } else {
                        //error handle?
                        ALOGE("%s(): Error! Not enough memory for audio dump buffer size=%d", __FUNCTION__, allocBufSize);
                    }
                    free(pBufTemp);
                    pBufTemp = NULL;
                }

                if (newQueueBuffer->allocBufSize >= currBufSize + mergeBufSize) {
                    memcpy((char*)(newQueueBuffer->pBufBase) + currBufSize,
                           (char*)(newQueueBuffer2->pBufBase), mergeBufSize);
                    newQueueBuffer->fileInfo.size += mergeBufSize;

                    mAudioDumpMutex.lock();
                    pQueueBufferVector->removeAt(1);
                    mAudioDumpMutex.unlock();

                    free(newQueueBuffer2->pBufBase);
                    newQueueBuffer2->pBufBase = NULL;

                    delete newQueueBuffer2;
                    newQueueBuffer2 = NULL;
                } else {
                    PushBufferFromeQueueToVector(keyVector[i], newQueueBuffer);

                    mAudioDumpMutex.lock();
                    pQueueBufferVector->removeAt(0);
                    mAudioDumpMutex.unlock();



                }
            }
            queueSize--;
        }

        if (bFlush && queueSize == 1) {
            //ALOGD("Flush data");
            mAudioDumpMutex.lock();
            AudioDumpBuffer *newQueueBuffer = (*pQueueBufferVector)[0];
            mAudioDumpMutex.unlock();

            PushBufferFromeQueueToVector(keyVector[i], newQueueBuffer);

            mAudioDumpMutex.lock();
            pQueueBufferVector->removeItemsAt(0);
            mAudioDumpMutex.unlock();

            queueSize--;
        }
    }
}


void *AudioDumpThread(void *arg __attribute__((unused)))
{
    pthread_detach(pthread_self());
    bool bHasdata = false;
    bool bFlush = false;
    int iNoDataCount = 0;
    int iLoopCount = 0;
    ALOGV("AudioDumpThread(%ld) start", hAudioDumpThread);

    id_t tid = gettid();
    int priority = -10;
    int ret;

    //ret = getpriority(PRIO_PROCESS, tid);
    //ALOGD("Thread(%d) priority = %d", tid, ret);

    ret = setpriority(PRIO_PROCESS, tid, priority);
    if (ret != 0) {
        ALOGE("AudioDumpThread setpriority fail. tid(%d), priority(%d) ",
            tid, priority);
    }
    //ret = getpriority(PRIO_PROCESS, tid);
    //ALOGD("Thread(%d) priority = %d", tid, ret);

    while (1) {
        ProcessBufferQueue(bFlush);
        bFlush = false;
        bHasdata = false;
        size_t FileVectorSize = mAudioDumpFileVector.size();

        for (size_t i = 0; i < FileVectorSize; i++) {
            String8 filePathPCM = mAudioDumpFileVector.keyAt(i);
            Vector<AudioDumpBuffer *>* pvector = (mAudioDumpFileVector.valueAt(i));
            int BufferVectorSize = (*pvector).size();

            if (BufferVectorSize > 1) {
                bHasdata = true;
                FILE * fpWav = NULL;
                String8 filePathWav = filePathPCM;

                AudioDumpBuffer *pLastBufferInfo = (*pvector)[0];
                if (pLastBufferInfo == NULL) {
                    ALOGE("Array index error!!![%s]", filePathPCM.string());
                    continue;
                }

                //get the dump file name
                if (pLastBufferInfo->changeCount == 0) {
                    filePathWav = String8::format("%s.wav", filePathPCM.string());
                } else {
                    filePathWav = String8::format("%s.%d.wav", filePathPCM.string(), pLastBufferInfo->changeCount);
                }

                // create dump folder if need
                int ret = AudioDump::checkPath(filePathWav.string());
                if (ret < 0) {
                    ALOGE("dump %s fail!!!", filePathWav.string());
                    continue;
                }

                while (BufferVectorSize > 1) {
                    AudioDumpBuffer *pAudioBuffer = (*pvector)[1];
                    if (pAudioBuffer == NULL || pLastBufferInfo == NULL) {
                        //ALOGE("AudioDumpThread null buffer error!!!!");
                        break;
                    }

                    // if the format is changed, we use a new file name
                    if (memcmp(&(pAudioBuffer->fileInfo), &(pLastBufferInfo->fileInfo),
                        sizeof(AudioDumpFileInfo) - sizeof(int)) != 0) {
                        if (fpWav != NULL) {
                            fclose(fpWav);
                            fpWav = NULL;
                        }

                        UpdateWaveHeader_f(filePathWav, *pLastBufferInfo);

                        int changeCount = pLastBufferInfo->changeCount + 1;
                        memcpy(pLastBufferInfo, pAudioBuffer, sizeof(AudioDumpBuffer));
                        pLastBufferInfo->changeCount = changeCount;
                        pLastBufferInfo->fileInfo.size = 0;

                        filePathWav = String8::format("%s.%d.wav", filePathPCM.string(), pLastBufferInfo->changeCount);
                    }

                    // open the dump file, and write new header if need
                    if (fpWav == NULL) {
                        fpWav = fopen(filePathWav.string(), "ab+");
                        if (fpWav != NULL) {
                            fseek_s(fpWav, 0, SEEK_END);
                            if (ftell(fpWav) == 0) {
                                // Write Header
                                WriteNewWaveHeader(fpWav, *pLastBufferInfo);
                            }
                        }
                    }

                    // write audio buffer
                    if (fpWav != NULL) {
                        WriteAudioBuffer(fpWav, *pAudioBuffer);
                        pLastBufferInfo->fileInfo.size += pAudioBuffer->fileInfo.size;
                        (*pvector).removeAt(1);
                        delete pAudioBuffer;
                        pAudioBuffer = NULL;
                        BufferVectorSize--;
                    }
                }

                if (fpWav != NULL) {
                    fclose(fpWav);
                    fpWav = NULL;
                }
            }
        }

        if (!bHasdata) {
            iNoDataCount++;
            if (iNoDataCount >= 2 * AUDIO_DUMP_BUFFER_COUNT_MAX) { // wait 640ms
                bFlush = true;
                iLoopCount = 0;
            }
            if (iNoDataCount >= 2 * AUDIO_DUMP_BUFFER_COUNT_MAX + 1) { // wait 641ms
                UpdateAllWaveHeader();
            }
            if (iNoDataCount >= 300) { // 3s
                mAudioDumpMutex.lock();
                mAudioDumpSleepTime = -1;
                mAudioDumpMutex.unlock();
                //ALOGD("AudioDumpThread, wait for new data dump\n");
                pthread_mutex_lock(&AudioDataNotifyMutex);
                pthread_cond_wait(&AudioDataNotifyEvent, &AudioDataNotifyMutex);
                pthread_mutex_unlock(&AudioDataNotifyMutex);
                //ALOGD("AudioDumpThread, PCM data dump again\n");
            } else {
                mAudioDumpMutex.lock();
                mAudioDumpSleepTime = 10;
                mAudioDumpMutex.unlock();
                usleep(mAudioDumpSleepTime * 1000);
            }
        } else {
            iNoDataCount = 0;
            mAudioDumpMutex.lock();
            mAudioDumpSleepTime = 2;
            mAudioDumpMutex.unlock();
            usleep(mAudioDumpSleepTime * 1000);
        }

        if (++iLoopCount >= 4 * AUDIO_DUMP_BUFFER_COUNT_MAX) {
            bFlush = true;
            iLoopCount = 0;
        }
    }

    ALOGD("AudioDumpThread exit hAudioDumpThread=%ld", hAudioDumpThread);
    hAudioDumpThread = 0;
    pthread_exit(NULL);
    return 0;
}
// Audio Dump Thread E

//class  AudioDump
void AudioDump::dump(const char * filepath, void * buffer, int count)
{
    { // mAudioDumpKeyMutex region
        Mutex::Autolock _l(mAudioDumpKeyMutex);
        ssize_t index = mAudioDumpKeyEnableVector.indexOfKey(PROP_AUDIO_DUMP_LOG);
        if (index < 0) { // new add
            int bflag = (int)property_get_bool(af_dump_log, false);
            mAudioDumpKeyEnableVector.add(PROP_AUDIO_DUMP_LOG, bflag);
        }
    }
    int ret = checkPath(filepath);
    if (ret < 0) {
        // ALOGE("dump fail!!!");
    } else {
        FILE * fp= fopen(filepath, "ab+");
        if (fp != NULL) {
            fwrite_s(buffer, 1, count, fp);
            fclose(fp);
            if (mAudioDumpKeyEnableVector.valueFor(PROP_AUDIO_DUMP_LOG) > 0) {
                ALOGD("dump() %s(%d)", filepath, count); // For dump analysis
            } else {
                ALOGV("dump() %s(%d)", filepath, count); // For dump analysis
            }
        } else {
            //ALOGE("dump %s fail",property);
        }
    }
}

void AudioDump::threadDump(const char * path, void * buffer, int count, audio_format_t format,
                                  uint32_t sampleRate, uint32_t channelCount)
{
    { // mAudioDumpKeyMutex region
        Mutex::Autolock _l(mAudioDumpKeyMutex);
        ssize_t index = mAudioDumpKeyEnableVector.indexOfKey(PROP_AUDIO_DUMP_LOG);
        if (index < 0) { // new add
            int bflag = (int)property_get_bool(af_dump_log, false);
            mAudioDumpKeyEnableVector.add(PROP_AUDIO_DUMP_LOG, bflag);
        }
    }

    {
        Mutex::Autolock _l(mAudioDumpMutex);
        if (hAudioDumpThread == 0) {
            char value[PROPERTY_VALUE_MAX];
            property_get(AUDIO_DUMP_FILE_DELAY_TIME_KEY, value, "2");
            mAudioDumpFileIoDelayTime = atoi(value);

            //create PCM data dump thread here
            int ret;
            ret = pthread_create(&hAudioDumpThread, NULL, AudioDumpThread, NULL);
            if (ret != 0) {
                // ALOGE("hAudioDumpThread create fail!!!");
            } else {
                // ALOGD("hAudioDumpThread=%p created", hAudioDumpThread);
            }

            /*struct sched_param sched;
            sched.sched_priority = 0;
            ret = pthread_setschedparam( hAudioDumpThread, SCHED_OTHER, &sched );
            if (ret != 0)
            {
                ALOGE("pthread_setschedparam() fail!!!");
            }*/

            ret = pthread_cond_init(&AudioDataNotifyEvent, NULL);
            if (ret != 0) {
                ALOGE("AudioDataNotifyEvent create fail!!!");
            }

            ret = pthread_mutex_init(&AudioDataNotifyMutex, NULL);
            if (ret != 0) {
                ALOGE("AudioDataNotifyMutex create fail!!!");
            }
        }
    }

    pushBufferInfo(path, buffer, count, format, sampleRate, channelCount);

    {
        Mutex::Autolock _l(mAudioDumpKeyMutex);
        if (mAudioDumpKeyEnableVector.valueFor(PROP_AUDIO_DUMP_LOG) > 0) {
            ALOGD("threadDump() %s(%d)", path, count); // For dump analysis
        } else {
            ALOGV("threadDump() %s(%d)", path, count); // For dump analysis
        }
    }
}

bool AudioDump::getProperty(AudioDump::PROP_AUDIO_DUMP key)
{
    // get property from cache
    ssize_t index = mAudioDumpKeyEnableVector.indexOfKey(key);
    if (index < 0) { // new add
        int bflag = (int)property_get_bool(AudioDump::audioDumpPropertyStr[key], false);
        mAudioDumpKeyEnableVector.add(key, bflag);
        return bflag;
    }
    return mAudioDumpKeyEnableVector[index];
}

int AudioDump::checkPath(const char * path)
{
    char tmp[PATH_MAX];
    int i = 0;

    while (*path) {
        tmp[i] = *path;

        if (*path == '/' && i) {
            tmp[i] = '\0';
            Mutex::Autolock _l(mAudioDumpFileMutex);
            if (access(tmp, F_OK) != 0) {
                if (mkdir(tmp, 0770) == -1) {
                    // ALOGE("mkdir error! %s",(char*)strerror(errno));
                    return -1;
                }
            }
            tmp[i] = '/';
        }
        i++;
        path++;
    }
    return 0;
}

void AudioDump::updateKeys(int key)
{
    Mutex::Autolock _l(mAudioDumpKeyMutex);

    if (key < 0 || key >= AudioDump::PROP_AUDIO_DUMP_MAXNUM) {
        mAudioDumpKeyEnableVector.clear();
    } else {
        mAudioDumpKeyEnableVector.removeItem((AudioDump::PROP_AUDIO_DUMP)key);
    }
}

void AudioDump::pushBufferInfo(const char * path, void * buffer, int count,
                                    audio_format_t format, uint32_t sampleRate, uint32_t channelCount)
{
    if (buffer != NULL && count > 0) {
        AudioDumpBuffer *newQueueBuffer = new AudioDumpBuffer();

        newQueueBuffer->pBufBase = (char*)malloc(count);
        if (newQueueBuffer->pBufBase != NULL) {
            memcpy(newQueueBuffer->pBufBase, buffer, count);
        } else {
            //error handle?
            ALOGE("%s(): Error! Not enough memory for audio dump buffer size=%d", __FUNCTION__, count);
        }
        newQueueBuffer->fileInfo.format = format;
        newQueueBuffer->fileInfo.sampleRate = sampleRate;
        newQueueBuffer->fileInfo.channelCount = channelCount;
        newQueueBuffer->fileInfo.size = count;

        Vector<AudioDumpBuffer *> *pQueueBufferVector = NULL;

        mAudioDumpMutex.lock();
        ssize_t index = mAudioDumpQueueVector.indexOfKey(String8(path));
        if (index < 0) { // new add
            pQueueBufferVector = new Vector<AudioDumpBuffer *>;
            mAudioDumpQueueVector.add( String8(path), pQueueBufferVector);
            ALOGD("new threadDump() %s(%d)", path, count); // For dump analysis
        } else {
            pQueueBufferVector = mAudioDumpQueueVector.valueAt(index);
        }
        if (pQueueBufferVector) {
            pQueueBufferVector->add(newQueueBuffer);
        }
        bool needWakeup = (mAudioDumpSleepTime == -1);
        mAudioDumpMutex.unlock();
        if (needWakeup) { //need to send event
            pthread_mutex_lock(&AudioDataNotifyMutex);
            pthread_cond_signal(&AudioDataNotifyEvent);
            pthread_mutex_unlock(&AudioDataNotifyMutex);
        }
    }
}
// class hw

bool HwFSync::mUnderflow =false;

HwFSync::HwFSync()
    :   mFd(-1)
{
    memset(&action, 0, sizeof(action));
}

HwFSync::~HwFSync()
{
    if (mFd != -1) {
        ::close(mFd);
        mFd = -1;
    }
}

void HwFSync::setFsync()
{
    if (mFd == -1) {
        mFd = ::open("/dev/eac", O_RDWR);
        ALOGW("mfd =%d",mFd);
        if (mFd < 0) {
           ALOGE("setFsync fail to open eac");
        }
    }
    if (mFd >= 0) {
        ALOGD("callback hw setFSync");
        memset(&action, 0, sizeof(action));
        action.sa_handler = callback;
        action.sa_flags = 0;
        int err = sigaction(SIGIO, &action, NULL); //set up async handler
        if (err == 0) {
            if (fcntl(mFd, F_SETOWN, gettid()) == -1) { //enable async notification
                ALOGE("setFsync(), F_SETOWN error! %s",(char*)strerror(errno));
            }
            if (fcntl(mFd, F_SETFL, fcntl(mFd, F_GETFL) | FASYNC | FD_CLOEXEC) == -1) {
                ALOGE("setFsync(), F_SETFL error! %s", (char*)strerror(errno));
            }
        }
    }
}

//do  not use mutex to protect this value, use atomic if needed.
bool HwFSync::underflow()
{
   return mUnderflow;
}

void HwFSync::callback(int signal)
{
    ALOGD("callback");
    if (signal == SIGIO) {
        mUnderflow = true;
        ALOGD("callback hw is under flow");
    }
}

void HwFSync::reset()
{
   mUnderflow = false;
}

void setCPU_MIN_Freq(const char *pfreq)
{
    FILE *fp= fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq", "w");
    if (fp != NULL) {
        fputs(pfreq, fp);
        fclose(fp);
    } else {
        ALOGE("Can't open /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq");
    }
}

void FeatureOption::getValues()
{
    // FO values are cached in the system parser.
    bool foValue_MTK_BESLOUDNESS_RUN_WITH_HAL = appIsFeatureOptionEnabled(foName_MTK_BESLOUDNESS_RUN_WITH_HAL);
    bool foValue_MTK_BESLOUDNESS_SUPPORT = appIsFeatureOptionEnabled(foName_MTK_BESLOUDNESS_SUPPORT);
    const char * foValue_MTK_USB_PHONECALL = appGetFeatureOptionValue(foName_MTK_USB_PHONECALL);
    int foValue_MTK_AUDIO_NUMBER_OF_SPEAKER = atoi(appGetFeatureOptionValue(foName_MTK_AUDIO_NUMBER_OF_SPEAKER));
    bool foValue_MTK_TTY_SUPPORT = appIsFeatureOptionEnabled(foName_MTK_TTY_SUPPORT);
    bool foValue_MTK_HIFIAUDIO_SUPPORT = appIsFeatureOptionEnabled(foName_MTK_HIFIAUDIO_SUPPORT);
    bool foValue_MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT = appIsFeatureOptionEnabled(foName_MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT);
    bool foValue_MTK_FM_SUPPORT = appIsFeatureOptionEnabled(foName_MTK_FM_SUPPORT);
    bool foValue_MTK_AURISYS_FRAMEWORK_SUPPORT = appIsFeatureOptionEnabled(foName_MTK_AURISYS_FRAMEWORK_SUPPORT);

    MTK_AUDIOMIXER_ENABLE_DRC = !foValue_MTK_BESLOUDNESS_RUN_WITH_HAL && foValue_MTK_BESLOUDNESS_SUPPORT;
    ALOGD("MTK_AUDIOMIXER_ENABLE_DRC: %d", MTK_AUDIOMIXER_ENABLE_DRC);

    MTK_ENABLE_STEREO_SPEAKER = (foValue_MTK_AUDIO_NUMBER_OF_SPEAKER >= 2) ? 1 : 0;
    ALOGD("MTK_ENABLE_STEREO_SPEAKER: %d", MTK_ENABLE_STEREO_SPEAKER);

    MTK_USB_PHONECALL = (!strcmp(foValue_MTK_USB_PHONECALL, "AP") || !strcmp(foValue_MTK_USB_PHONECALL, "DSP"));
    ALOGD("foValue_MTK_USB_PHONECALL %s, MTK_USB_PHONECALL: %d", foValue_MTK_USB_PHONECALL, MTK_USB_PHONECALL);

    MTK_TTY_SUPPORT = foValue_MTK_TTY_SUPPORT;
    ALOGD("MTK_TTY_SUPPORT %d", MTK_TTY_SUPPORT);
    MTK_HIFIAUDIO_SUPPORT = foValue_MTK_HIFIAUDIO_SUPPORT;
    ALOGD("MTK_HIFIAUDIO_SUPPORT: %d", MTK_HIFIAUDIO_SUPPORT);

    MTK_BESLOUDNESS_ENABLE = foValue_MTK_BESLOUDNESS_SUPPORT;
    ALOGD("MTK_BESLOUDNESS_ENABLE : %d", MTK_BESLOUDNESS_ENABLE);

    MTK_AUDIO_GAIN_TABLE = foValue_MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT;
    MTK_AUDIO_GAIN_NVRAM = !MTK_AUDIO_GAIN_TABLE;
    ALOGD("MTK_AUDIO_GAIN_TABLE: %d", MTK_AUDIO_GAIN_TABLE);

    MTK_FM_ENABLE = foValue_MTK_FM_SUPPORT;
    ALOGD("MTK_FM_ENABLE : %d", MTK_FM_ENABLE);

    MTK_AURISYS_FRAMEWORK_SUPPORT = foValue_MTK_AURISYS_FRAMEWORK_SUPPORT;
    ALOGD("MTK_AURISYS_FRAMEWORK_SUPPORT : %d", MTK_AURISYS_FRAMEWORK_SUPPORT);
}

}

