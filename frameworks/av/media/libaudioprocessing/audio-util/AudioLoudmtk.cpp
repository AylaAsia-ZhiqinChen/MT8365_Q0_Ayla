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

#include "AudioLoudmtk.h"
#include <dlfcn.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "AudioLoudmtk"
#define UNUSED(x) ((void)(x))

namespace android {

#if defined(__LP64__)
const char *AUDIO_COMPONENT_ENGINE_LIB_PATH = "/system/lib64/libaudiocomponentengine.so";
#else
const char *AUDIO_COMPONENT_ENGINE_LIB_PATH = "/system/lib/libaudiocomponentengine.so";
#endif

typedef bool AudioLoudEnableXMLCallBack();

static void *g_AudioComponentEngineHandle = NULL;
static create_AudioLoud *fp_AudioLoud = NULL;
static create_AudioCustParamCache *fp_CustParamCache = NULL;
static AudioLoudEnableXMLCallBack *fp_MtkAudioLoudEnableXMLCallBack = NULL;
static destroy_AudioLoud *fp_DestroyAudioLoud = NULL;
static destroy_AudioCustParamCache *fp_DestroyCustParamCache  = NULL;


inline void *openAudioRelatedLib(const char *filepath) {
    void *handle = NULL;
    if (filepath == NULL) {
        ALOGE("%s null input parameter", __FUNCTION__);
        return NULL;
    } else {
        if (access(filepath, R_OK) == 0) {
            handle = dlopen(filepath, RTLD_NOW);
            if (handle == NULL) {
                ALOGE("%s(), dlopen fail! (%s)\n", __FUNCTION__, dlerror());
                return NULL;
            }
            return handle;
        } else {
            ALOGE("%s filepath %s doesn't exist", __FUNCTION__, filepath);
            return NULL;
        }
    }
}

inline bool openAudioComponentEngine(void) {
    if (g_AudioComponentEngineHandle == NULL) {
        fp_AudioLoud = NULL;
        fp_CustParamCache = NULL;
        fp_MtkAudioLoudEnableXMLCallBack = NULL;
        fp_DestroyAudioLoud = NULL;
        fp_DestroyCustParamCache = NULL;
        g_AudioComponentEngineHandle  = openAudioRelatedLib(AUDIO_COMPONENT_ENGINE_LIB_PATH);
        return (g_AudioComponentEngineHandle == NULL) ? false : true;
    }
    return true;
}

inline void closeAudioComponentEngine(void) {
    if (g_AudioComponentEngineHandle != NULL) {
        dlclose(g_AudioComponentEngineHandle);
        g_AudioComponentEngineHandle = NULL;
        fp_AudioLoud = NULL;
        fp_CustParamCache = NULL;
        fp_MtkAudioLoudEnableXMLCallBack = NULL;
        fp_DestroyAudioLoud = NULL;
        fp_DestroyCustParamCache = NULL;
    }
}

MtkAudioLoudBase *newMtkAudioLoud(uint32_t eFLTtype, bool bFastTrack) {
    if (!openAudioComponentEngine()) {
        return NULL;
    }
    const char *error;
    if (fp_AudioLoud == NULL) {
        fp_AudioLoud = (create_AudioLoud *)dlsym(g_AudioComponentEngineHandle, "createMtkAudioLoud");
        error = dlerror();
        if (error != NULL) {
            ALOGE("%s(), dlsym createMtkAudioLoud fail. (%s)\n", __FUNCTION__, error);
            closeAudioComponentEngine();
            return NULL;
        }
    }
    ALOGV("%p fp_AudioLoud %p", g_AudioComponentEngineHandle, fp_AudioLoud);
    return fp_AudioLoud(eFLTtype, bFastTrack);
}

void deleteMtkAudioLoud(MtkAudioLoudBase *pObject) {
    if (!openAudioComponentEngine()) {
        return;
    }
    const char *error;
    if (fp_DestroyAudioLoud == NULL) {
        fp_DestroyAudioLoud = (destroy_AudioLoud *)dlsym(g_AudioComponentEngineHandle, "destroyMtkAudioLoud");
        error = dlerror();
        if (error != NULL) {
            ALOGE("%s(), dlsym deleteMtkAudioLoud fail. (%s)\n", __FUNCTION__, error);
            closeAudioComponentEngine();
            return;
        }
    }
    ALOGV("%p fp_DestroyAudioLoud %p", g_AudioComponentEngineHandle, fp_DestroyAudioLoud);
    fp_DestroyAudioLoud(pObject);
    return;
}

MtkAudioCustParamCacheBase *newMtkAudioCustParamCache() {
    if (!openAudioComponentEngine()) {
        return NULL;
    }
    const char *error;
    if (fp_CustParamCache == NULL) {
        fp_CustParamCache = (create_AudioCustParamCache *)dlsym(g_AudioComponentEngineHandle, "createMtkAudioCustParamCache");
        error = dlerror();
        if (error != NULL) {
            ALOGE("%s(), dlsym createMtkAudioCustParamCache fail. (%s)\n", __FUNCTION__, error);
            closeAudioComponentEngine();
            return NULL;
        }
    }
    ALOGV("%p fp_CustParamCache %p", g_AudioComponentEngineHandle, fp_CustParamCache);
    return fp_CustParamCache();
}

void deleteMtkCustParamCache(MtkAudioCustParamCacheBase *pObject) {
    if (!openAudioComponentEngine()) {
        return;
    }
    const char *error;
    if (fp_DestroyCustParamCache == NULL) {
        fp_DestroyCustParamCache = (destroy_AudioCustParamCache *)dlsym(g_AudioComponentEngineHandle, "destroyMtkAudioCustParamCache");
        error = dlerror();
        if (error != NULL) {
            ALOGE("%s(), dlsym deleteMtkCustParamCache fail. (%s)\n", __FUNCTION__, error);
            closeAudioComponentEngine();
            return;
        }
    }
    ALOGV("%p fp_DestroyCustParamCache %p", g_AudioComponentEngineHandle, fp_DestroyCustParamCache);
    fp_DestroyCustParamCache(pObject);
    return;
}

bool MtkAudioLoud_InitParser(void) {
    if (!openAudioComponentEngine()) {
        return false;
    }
    const char *error;
    if (fp_MtkAudioLoudEnableXMLCallBack == NULL) {
        fp_MtkAudioLoudEnableXMLCallBack = (AudioLoudEnableXMLCallBack *)dlsym(g_AudioComponentEngineHandle, "MtkAudioLoudEnableXMLCallBack");
        error = dlerror();
        if (error != NULL) {
            ALOGE("%s(), dlsym MtkAudioLoudEnableXMLCallBack fail. (%s)\n", __FUNCTION__, error);
            closeAudioComponentEngine();
            return false;
        }
    }
    ALOGV("%p fp_MtkAudioLoudEnableXMLCallBack %p", g_AudioComponentEngineHandle, fp_MtkAudioLoudEnableXMLCallBack);
    return fp_MtkAudioLoudEnableXMLCallBack();
}

}

