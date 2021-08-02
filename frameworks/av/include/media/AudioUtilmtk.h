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

#ifndef ANDROID_AUDIO_UTILITY_MTK_H
#define ANDROID_AUDIO_UTILITY_MTK_H

#include <utils/threads.h>
#include <hardware/audio.h>
//#include <media/AudioParameter.h>
//#include "../../../../protect-bsp/frameworks/av/media/libeffects/loudness/MtkEffectLoudness.h"
static const effect_uuid_t SL_IID_MTKLOUDNESS_ = { 0x1486d4c0, 0x8757, 0x11e0, 0xb091, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };
const effect_uuid_t * const SL_IID_MTKLOUDNESS = &SL_IID_MTKLOUDNESS_;

namespace android {

class AudioDump
{
public:
    enum PROP_AUDIO_DUMP {
        PROP_AUDIO_DUMP_MIXER = 0,
        PROP_AUDIO_DUMP_TRACK,
        PROP_AUDIO_DUMP_OFFLOAD,
        PROP_AUDIO_DUMP_RESAMPLER,
        PROP_AUDIO_DUMP_MIXEREND,
        PROP_AUDIO_DUMP_RECORD,
        PROP_AUDIO_DUMP_EFFECT,
        PROP_AUDIO_DUMP_DRC,
        PROP_AUDIO_DUMP_LOG,
        PROP_AUDIO_DUMP_AAUDIO,
        PROP_AUDIO_DUMP_MAXNUM
    };
    static void dump(const char * path, void * buffer, int count);
    static void threadDump(const char * path, void * buffer, int count, audio_format_t format,
                                  uint32_t sampleRate, uint32_t channelCount);
    static bool getProperty(AudioDump::PROP_AUDIO_DUMP index);
    static int checkPath(const char * path);
    static void updateKeys(int key);

    // For audio dump debug
    static const char * const keyAudioDumpMixer;
    static const char * const keyAudioDumpTrack;
    static const char * const keyAudioDumpOffload;
    static const char * const keyAudioDumpResampler;
    static const char * const keyAudioDumpMixerEnd;
    static const char * const keyAudioDumpRecord;
    static const char * const keyAudioDumpEffect;
    static const char * const keyAudioDumpDrc;
    static const char * const keyAudioDumpLog;
    static const char * const keyAudioDumpAAudio;


    static const char *audioDumpPropertyStr[];
    static const char * const af_track_pcm;
    static const char * const af_mixer_pcm;
    static const char * const af_mixer_write_pcm;
    static const char * const af_mixer_end_pcm;
    static const char * const af_offload_write_raw;
    static const char * const af_record_read_pcm;
    static const char * const af_record_convert_pcm;
    static const char * const af_effect_pcm;
    static const char * const af_mixer_drc_pcm_before;
    static const char * const af_mixer_drc_pcm_after;
    static const char * const af_resampler_in_pcm;
    static const char * const af_resampler_out_pcm;

    static const char * const aaudio_share_dl;
    static const char * const aaudio_share_ul;
    static const char * const aaudio_exclusive_dl;
    static const char * const aaudio_exclusive_ul;

private:
    static void pushBufferInfo(const char * path, void * buffer, int count,
        audio_format_t format, uint32_t sampleRate, uint32_t channelCount);

private:
    AudioDump(AudioDump &);
    AudioDump & operator =(AudioDump&);
};

class HwFSync{
public:
    HwFSync();
    ~HwFSync();
    void setFsync();
    bool underflow();
    void reset();
    static void callback(int signal);
private:
    HwFSync(HwFSync &);
    HwFSync & operator = (HwFSync&);
    int mFd;
    struct sigaction action;
    static bool mUnderflow;

};

class FeatureOption
{
public:
    static void getValues();

    static bool MTK_AUDIOMIXER_ENABLE_DRC;
    static int MTK_ENABLE_STEREO_SPEAKER;
    static bool MTK_USB_PHONECALL;
    static bool MTK_TTY_SUPPORT;
    static bool MTK_HIFIAUDIO_SUPPORT;
    static bool MTK_BESLOUDNESS_ENABLE;
    static bool MTK_AUDIO_GAIN_TABLE;
    static bool MTK_AUDIO_GAIN_NVRAM ;
    static bool MTK_FM_ENABLE ;
    static bool MTK_AURISYS_FRAMEWORK_SUPPORT;

private:
    static const char * const foName_MTK_BESLOUDNESS_RUN_WITH_HAL;
    static const char * const foName_MTK_BESLOUDNESS_SUPPORT;
    static const char * const foName_MTK_AUDIO_NUMBER_OF_SPEAKER;
    static const char * const foName_MTK_AUDIO_HIERARCHICAL_PARAM_SUPPORT;
    static const char * const foName_MTK_HIFIAUDIO_SUPPORT;
    static const char * const foName_MTK_AURISYS_FRAMEWORK_SUPPORT;
    static const char * const foName_MTK_USB_PHONECALL;
    static const char * const foName_MTK_TTY_SUPPORT;
    static const char * const foName_MTK_FM_SUPPORT;
};

//MTK80721 FIX CR: ALPS00355888
extern "C"{
void setCPU_MIN_Freq(const char *pfreq);
}

}

#endif

