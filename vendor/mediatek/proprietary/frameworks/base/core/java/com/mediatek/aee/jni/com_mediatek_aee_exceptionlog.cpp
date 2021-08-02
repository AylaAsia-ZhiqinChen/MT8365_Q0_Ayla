/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*
 * Copyright (C) 2007 The Android Open Source Project
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

#define LOG_TAG "AES"

#include <jni.h>
#include <nativehelper/JNIHelp.h>
#include <utils/misc.h>
#include <utils/Log.h>
#include <utils/threads.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <dirent.h>
#include <linux/fb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <cutils/sockets.h>
#include <utils/Log.h>
#include <cutils/properties.h>

#include <private/android_filesystem_config.h>

#include "aee.h"

namespace android {

// ----------------------------------------------------------------------------

static void com_mediatek_exceptionlog_report(JNIEnv* env, jobject /*clazz*/,
    jstring _process, jstring _module, jstring _traceback, jstring _detail,
    jstring _cause, jlong _pid) {
    const char* process = env->GetStringUTFChars(_process, NULL);
    const char* module = env->GetStringUTFChars(_module, NULL);
    const char* traceback = env->GetStringUTFChars(_traceback, NULL);
    const char* detail = env->GetStringUTFChars(_detail, NULL);
    const char* cause = env->GetStringUTFChars(_cause, NULL);
    char value[PROPERTY_VALUE_MAX];
    int level = 0;
    int mode = 0;

    memset(value, 0x0, sizeof(value));

    if (property_get("persist.vendor.mtk.aee.explevel", value, NULL) > 0) {
        level = atoi(value);
    } else {
        if (property_get("persist.vendor.mtk.aee.mode", value, NULL) > 0) {
            mode = atoi(value);
            if (mode == 1)
                level = 4;
            else if (mode == 2)
                level = 4;
            else if (mode == 3)
                level = 1;
            else if (mode ==4)
                level = 0;
            else
                level = 0;
       } else {
            level = 0;
       }
    }

    pid_t pid = (pid_t)AE_NOT_AVAILABLE; //TODO:pass crashed pid from caller

    if(_pid != 0) {
        pid = (pid_t) _pid;
    }

    ALOGD("ExceptionLog: notify aed, process:%s pid:%d cause:%s \n", process, pid, cause);

    #ifdef MSSI_HAVE_AEE_FEATURE
    pid_t tid = (pid_t)AE_NOT_AVAILABLE; //TODO:pass crashed tid from caller
    if (strstr(cause, "lowmem")) {
        ALOGW("skip AOSP lowmem case");
    } else if (strstr(cause, "native_crash")) {
        // onpurpose skip native_crash reporting to aee
        // because it should be processed by aee before running here
        ALOGW("native_crash should be processed by aee already");
    } else if (strstr(cause, "system_server_crash")) {
        aee_aed_raise_exception(AE_JE, pid, tid, cause, process, module, traceback, detail, 0);
    } else if (strstr(cause, "crash") && (level >= 1)) {
        aee_aed_raise_exception(AE_JE, pid, tid, cause, process, module, traceback, detail, 0);
    } else if (strstr(cause, "wtf")) {
        if (level >= 3)
            aee_aed_raise_exception(AE_WTF, pid, tid, cause, process, module, traceback, detail, 0);
        else
            ALOGW("skip for Exp level %d", level);
    } else if (strstr(cause, "anr") && (level >= 1)) {
        int anrcount = 0;
        if (property_get("vendor.debug.mtk.aee.anrcount", value, "0") > 0)
            anrcount = atoi(value);
        if (anrcount < 3)
            aee_aed_raise_exception(AE_ANR, pid, tid, cause, process, module, traceback, detail, 0);
        else
            ALOGW("skip anr process:%s pid:%d\n", process, pid);
    } else if (strstr(cause, "watchdog")) {
        aee_aed_raise_exception(AE_SWT, pid, tid, cause, process, module, traceback, detail, 0);
    } else if (level >= 1) {
        ALOGW("not correct type, but use JE as default");
        aee_aed_raise_exception(AE_JE, pid, tid, cause, process, module, traceback, detail, 0);
    } else {
        ALOGW("skip for Exp level %d", level);
    }
    #endif

    // free native string resources
      env->ReleaseStringUTFChars(_process, process);
      env->ReleaseStringUTFChars(_module, module);
      env->ReleaseStringUTFChars(_traceback, traceback);
      env->ReleaseStringUTFChars(_detail, detail);
      env->ReleaseStringUTFChars(_cause, cause);
}

static void com_mediatek_exceptionlog_systemreport(JNIEnv* env, jobject /*clazz*/,
    jbyte _type, jstring _module, jstring _backtrace,jstring _msg, jstring _path) {
    const char* module = env->GetStringUTFChars(_module, NULL);
    const char* backtrace = env->GetStringUTFChars(_backtrace, NULL);
    const char* msg = env->GetStringUTFChars(_msg, NULL);
    const char* path = env->GetStringUTFChars(_path, NULL);

#define AEE_WARNING_JNI 0
#define AEE_EXCEPTION_JNI 1

    #ifdef MSSI_HAVE_AEE_FEATURE
    const unsigned char type = _type;
    if (AEE_WARNING_JNI == type) {
        aee_system_report_JNI(AE_DEFECT_WARNING,module,backtrace,path,msg,DB_OPT_DEFAULT);
    } else if (AEE_EXCEPTION_JNI == type) {
        aee_system_report_JNI(AE_DEFECT_EXCEPTION,module,backtrace,path,msg,DB_OPT_DEFAULT);
    } else {
        ALOGW("not correct type, but use warning as default");
        aee_system_report_JNI(AE_DEFECT_WARNING,module,backtrace,path,msg,DB_OPT_DEFAULT);
    }
    #else
    _type = ' ';
    #endif

    // free native string resources
    env->ReleaseStringUTFChars(_module, module);
    env->ReleaseStringUTFChars(_backtrace, backtrace);
    env->ReleaseStringUTFChars(_msg, msg);
    env->ReleaseStringUTFChars(_path, path);
}

static jboolean com_mediatek_exceptionlog_getNativeExceptionPidListImpl(JNIEnv* env, jobject /*clazz*/, jintArray pidList)
{
    if (env->GetArrayLength(pidList) < AEE_WORKER_MAX) {
    ALOGE("%s: Array no big enough %d\n", __func__, env->GetArrayLength(pidList));
    return JNI_FALSE;
    }

    #ifdef MSSI_HAVE_AEE_FEATURE
    struct aee_exception_entry entries[AEE_WORKER_MAX];
    if (aee_exception_running(entries) >= 0) {
    jint fill[AEE_WORKER_MAX];
        for (int i = 0; i < AEE_WORKER_MAX; i++) {
            if ((entries[i].clasz == AE_NE)||(entries[i].clasz == AE_RESMON)) {
                fill[i] = entries[i].pid;
            }
            else {
                fill[i] = -1;
            }
    }
    env->SetIntArrayRegion(pidList, 0, AEE_WORKER_MAX, fill);
    return JNI_TRUE;
    }
    #endif
    return JNI_FALSE;
}

static void com_mediatek_exceptionlog_switchFtrace(JNIEnv* /*env*/, jobject /*clazz*/, jint config)
{
    #ifdef MSSI_HAVE_AEE_FEATURE
    aee_switch_ftrace(config);
    #else
    config = 0;
    #endif
    return;
}

// QHQ add rt thread monitor system server
// 0 for kick
// other for set, in second.
static void com_mediatek_exceptionlog_WDTMatter (JNIEnv* /*env*/, jobject /*clazz*/, jlong lParam)
{
    int fd = open(AE_WDT_DEVICE_PATH, O_RDONLY);
    int ret = 0 ;
    if (fd < 0)
    {
        ALOGD("RT Monitor ERROR: open %s failed.\n",AE_WDT_DEVICE_PATH);
        return ;
    }

    ALOGD("AEEIOCTL_RT_MON_Kick IOCTL,cmd= %lu, lParam=%ld. \n", (unsigned long)AEEIOCTL_RT_MON_Kick, (long)lParam);
    if ((ret = ioctl(fd, AEEIOCTL_RT_MON_Kick, (int)(lParam))))
    {
            ALOGD("AEEIOCTL_RT_MON_Kick IOCTL, ioctl ret =%d, errno=%d \n", ret, errno);
    } ;

    close (fd) ;
    return;
}

static jlong com_mediatek_exceptionlog_SFMatter (JNIEnv* /*env*/, jobject /*clazz*/, jlong setorget, jlong lParam)
{
    int ret = 0 ;
    int ioctlcode ;

    int fd = open(AE_WDT_DEVICE_PATH, O_RDONLY);
        if (fd < 0)
        {
            ALOGD("AEEIOCTL_GET/SET_SF_STATE IOCTL ERROR: open %s failed.\n",AE_WDT_DEVICE_PATH);
            return -1 ;
        }

    if (setorget==0 ) // get
    {
        ioctlcode = AEEIOCTL_GET_SF_STATE ;
    }
    else    // set
    {
        ioctlcode = AEEIOCTL_SET_SF_STATE ;
    }

    ALOGD("AEEIOCTL_GET/SET_SF_STATE IOCTL,cmd= %d, lParam=%ld. \n",ioctlcode, (long)lParam);

        if ((ret = ioctl(fd, ioctlcode, (long)(&lParam))))
        {
            ALOGD("AEEIOCTL_GET/SET_SF_STATE IOCTL, ioctl ret =%d, errno=%d \n", ret, errno);
            lParam = -1 ;
        } ;

    close (fd) ;
    return lParam;
}
// QHQ add rt thread monitor system server end.
// ----------------------------------------------------------------------------

/*
 * JNI registration.
 */
static JNINativeMethod gNotify[] = {
    /* name, signature, funcPtr */
    { "report", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;J)V",
      (void*) com_mediatek_exceptionlog_report },
    { "systemreportImpl", "(BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
      (void*) com_mediatek_exceptionlog_systemreport },
    {"getNativeExceptionPidListImpl", "([I)Z", (void *)com_mediatek_exceptionlog_getNativeExceptionPidListImpl } ,
    {"switchFtraceImpl", "(I)V", (void *)com_mediatek_exceptionlog_switchFtrace } ,
// QHQ RT Monitor
    {"WDTMatter", "(J)V", (void *)com_mediatek_exceptionlog_WDTMatter },
    {"SFMatter", "(JJ)J", (void *)com_mediatek_exceptionlog_SFMatter }
// QHQ RT Monitor end
};

int register_com_mediatek_aee_ExceptionLog(JNIEnv* env)
{
    int res = jniRegisterNativeMethods(env, "com/mediatek/aee/ExceptionLogImpl",
                                        gNotify, NELEM(gNotify));

    return res;
}

}; // namespace android

