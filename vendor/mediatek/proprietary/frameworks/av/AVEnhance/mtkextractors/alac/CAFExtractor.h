/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CAF_EXTRACTOR_H_

#define CAF_EXTRACTOR_H_

#include <utils/Errors.h>
#include <media/MediaExtractorPluginApi.h>
#include <media/MediaExtractorPluginHelper.h>
#include <media/NdkMediaFormat.h>

namespace android {

struct AMessage;
class String8;

enum
{
    kALACCodecFormat        = 'alac',
    kALACVersion            = 0,
    kALACCompatibleVersion  = kALACVersion,
    kALACDefaultFrameSize   = 4096
};

typedef struct CAFSpecificConfig
{
    uint32_t                frameLength;
    uint8_t                 compatibleVersion;
    uint8_t                 bitDepth;                           // max 32
    uint8_t                 pb;                                 // 0 <= pb <= 255
    uint8_t                 mb;
    uint8_t                 kb;
    uint8_t                 numChannels;
    uint16_t                maxRun;
    uint32_t                maxFrameBytes;
    uint32_t                avgBitRate;
    uint32_t                sampleRate;

} CAFSpecificConfig;

typedef struct CAFAudioFormat
{
    double    nSampleRate;
    uint32_t  nFormatID;
    uint32_t  nFormatFlags;
    uint32_t  nBytesPerPacket;
    uint32_t  nFramesPerPacket;
    uint32_t  nChannelsPerFrame;
    uint32_t  nBitsPerChannel;
} CAFAudioFormat;

typedef struct CAFPacketTableHeader
{
    int64_t   nNumberPackets;
    int64_t   nNumberValidFrames;
    int32_t   nPrimingFrames;
    int32_t   nRemainderFrames;
} CAFPacketTableHeader;

class CAFExtractor : public MediaExtractorPluginHelper {
public:
    CAFExtractor(DataSourceHelper *source, off64_t offset);

    virtual size_t countTracks();
    virtual MediaTrackHelper *getTrack(size_t index);
    virtual media_status_t getTrackMetaData(AMediaFormat *meta, size_t index, uint32_t flags);

    virtual media_status_t getMetaData(AMediaFormat *meta);

    virtual const char* name() { return "CAFExtractor"; }

protected:
    virtual ~CAFExtractor();

private:
    DataSourceHelper *mDataSource;
    AMediaFormat *mMeta;
    status_t mInitCheck;

    //desc
    CAFAudioFormat       mCAFDescChunkInfo;

    //pakt
    CAFPacketTableHeader mCAFPaktChunkInfo;

    //Kuki
    CAFSpecificConfig    mCAFKukiChunkInfo;

    off64_t mPaktStartPos;
    off64_t mDataStartPos;

    CAFExtractor(const CAFExtractor &);
    CAFExtractor &operator=(const CAFExtractor &);
};

bool SniffCAF(
        DataSourceHelper *source, String8 *mimeType,
        float *confidence, off64_t *offset);

}  // namespace android

#endif  // CAF_EXTRACTOR_H_
