/*
 *   Copyright Statement:
 *
 *     This software/firmware and related documentation ("MediaTek Software") are
 *     protected under relevant copyright laws. The information contained herein is
 *     confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 *     the prior written permission of MediaTek inc. and/or its licensors, any
 *     reproduction, modification, use or disclosure of MediaTek Software, and
 *     information contained herein, in whole or in part, shall be strictly
 *     prohibited.
 *
 *     MediaTek Inc. (C) 2019. All rights reserved.
 *
 *     BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *    THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 *     RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 *     ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 *     WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 *     WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 *     NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 *     RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 *     TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 *     RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 *     OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 *     SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 *     RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 *     STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 *     ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 *     RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 *     MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 *     CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     The following software/firmware and/or related documentation ("MediaTek
 *     Software") have been modified by MediaTek Inc. All revisions are subject to
 *     any receiver's applicable license agreements with MediaTek Inc.
 */
#ifndef LOGUTILS_H
#define LOGUTILS_H

#include <log/log.h>
#include <android/log.h>
#include <cutils/properties.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif // LOG_TAG

#define LOG_LEVEL "debug.camerapostalgo.feature.loglevel"
static int gLogLevel = ::property_get_int32(LOG_LEVEL, 2);

#define MY_LOGI(fmt, arg...)  do{if (gLogLevel==4) ALOGI("[%s] " fmt, __FUNCTION__, ##arg);}while(0)
#define MY_LOGD(fmt, arg...)  do{if (gLogLevel>=3) ALOGD("[%s] " fmt, __FUNCTION__, ##arg);}while(0)
#define MY_LOGW(fmt, arg...)  ALOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)  ALOGE("[%s] " fmt, __FUNCTION__, ##arg)

#define LOG_DETAILS "debug.camerapostalgo.feature.debug.logdetails"
static int8_t gLogDetail = ::property_get_bool(LOG_DETAILS, 0);
#define FUNCTION_IN   long function_in_begin_time = LogUtils::getCurrentMillSeconds();
#define FUNCTION_OUT  do{if (gLogDetail) ALOGD("[%s] cost [%ld] ms", __FUNCTION__, LogUtils::getCurrentMillSeconds() - function_in_begin_time);}while(0)

class LogUtils {
    public:
        static void fps(const char* tag);
        static long getCurrentMillSeconds();
};

#define LOG_FPS "debug.camerapostalgo.feature.debug.logfps"
static int gLogFps = ::property_get_int32(LOG_FPS, 0);
#define FPS LogUtils::fps(__FUNCTION__)

#endif
