/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2018. All rights reserved.
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <android/log.h>
#include <log/log.h>
#include <inttypes.h>

#include <mtk_log.h>

#define MAX_LOG_LEN 1024

void mtkLogD(const char *tag, const char *fmt, ...)
{
    char *buffer = NULL;
    buffer = calloc(1, MAX_LOG_LEN);
    if(buffer == NULL) {
        __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_DEBUG, tag, "mtk_logD : out of memory");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, MAX_LOG_LEN, fmt, args);
    __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_DEBUG, tag, "%s", buffer);
    va_end(args);
    free(buffer);
}

void mtkLogV(const char *tag, const char *fmt, ...)
{
    char *buffer = NULL;
    buffer = calloc(1, MAX_LOG_LEN);
    if(buffer == NULL) {
        __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_VERBOSE, tag, "mtk_logD : out of memory");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, MAX_LOG_LEN, fmt, args);
    __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_VERBOSE, tag, "%s", buffer);
    va_end(args);
    free(buffer);
}

void mtkLogI(const char *tag, const char *fmt, ...)
{
    char *buffer = NULL;
    buffer = calloc(1, MAX_LOG_LEN);
    if(buffer == NULL) {
        __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_INFO, tag, "mtk_logD : out of memory");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, MAX_LOG_LEN, fmt, args);
    __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_INFO, tag, "%s", buffer);
    va_end(args);
    free(buffer);
}

void mtkLogW(const char *tag, const char *fmt, ...)
{
    char *buffer = NULL;
    buffer = calloc(1, MAX_LOG_LEN);
    if(buffer == NULL) {
        __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_WARN, tag, "mtk_logD : out of memory");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, MAX_LOG_LEN, fmt, args);
    __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_WARN, tag, "%s", buffer);
    va_end(args);
    free(buffer);
}

void mtkLogE(const char *tag, const char *fmt, ...)
{
    char *buffer = NULL;
    buffer = calloc(1, MAX_LOG_LEN);
    if(buffer == NULL) {
        __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_ERROR, tag, "mtkLogW : out of memory");
        return;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, MAX_LOG_LEN, fmt, args);
    __android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_ERROR, tag, "%s", buffer);
    va_end(args);
    free(buffer);
}

void mtkAssert(char *pErrMsg) {
    if (pErrMsg) {
        LOG_ALWAYS_FATAL("%s", pErrMsg);
    } else {
        LOG_ALWAYS_FATAL();
    }
}

