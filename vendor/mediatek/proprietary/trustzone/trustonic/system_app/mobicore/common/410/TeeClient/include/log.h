/*
 * Copyright (c) 2013-2017 TRUSTONIC LIMITED
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the TRUSTONIC LIMITED nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** Log wrapper for Android.
 * Maps LOG_*() macros to __android_log_print() if ANDROID is defined.
 * Adds some extra info to log output like LOG_TAG, file name and line number.
 */
#ifndef TLCWRAPPERANDROIDLOG_H_
#define TLCWRAPPERANDROIDLOG_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifndef WIN32
#include <errno.h>
#include <unistd.h>
#define GETPID getpid
#else
#include <process.h>
#define GETPID _getpid
#endif

/** LOG_D(fmt, ...)
 * Debug information logging, only shown in debug version
 */

/** LOG_I(fmt, ...)
 * Important information logging
 */

/** LOG_W(fmt, ...)
 * Warnings logging
 */

/** LOG_E(fmt, ...)
 * Error logging
 */

/** LOG_D_BUF(message, blob, sizeOfBlob)
 * Binary logging, line-wise output to LOG_D
 */

#define DUMMY_FUNCTION()    do {} while(0)

#ifdef ANDROID
#include <android/log.h>
// log to adb logcat
#ifdef NDEBUG // no logging in debug version
    #define LOG_D(fmt, ...) DUMMY_FUNCTION()
#else
    // add LINE
    #define LOG_D(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt " [%s:%d]", ##__VA_ARGS__, __FILE__, __LINE__)
#endif
    #define LOG_I(fmt, ...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt " [%s:%d]", ##__VA_ARGS__, __FILE__, __LINE__)
    #define LOG_W(fmt, ...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, fmt " [%s:%d]", ##__VA_ARGS__, __FILE__, __LINE__)
    #define LOG_E(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt " [%s:%d]", ##__VA_ARGS__, __FILE__, __LINE__)
    #define _LOG_E(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##__VA_ARGS__)
#endif // defined(ANDROID)


#ifdef LOG_TIZEN
#include <dlog.h>
#ifdef NDEBUG
    #define LOG_D(...)  DUMMY_FUNCTION()
#else
    #define LOG_D(...)  SLOGD(__VA_ARGS__)
#endif
    #define LOG_I(...)  SLOGI(__VA_ARGS__)
    #define LOG_W(...)  SLOGW(__VA_ARGS__)
    #define LOG_E(...)  SLOGW(__VA_ARGS__)
    #define _LOG_E(...) SLOGE(__VA_ARGS__)
#endif // defined(LOG_TIZEN)


#if !defined(_LOG_E)
    // log using fprintf, by default to stdout
#ifdef LOG_FPRINTF
    extern FILE* mc_log_file_;
    #define _LOG_x(_x_,...) \
        do { \
            fprintf(mc_log_file_, "%s/%s(%d): ",_x_,LOG_TAG,GETPID()); \
            fprintf(mc_log_file_, __VA_ARGS__); \
            fprintf(mc_log_file_, "\n"); \
        } while(0)
#else // LOG_FPRINTF
    #define _LOG_x(_x_,...) \
        do { \
            printf("%s/%s(%d): ",_x_,LOG_TAG,GETPID()); \
            printf(__VA_ARGS__); \
            printf("\n"); \
        } while(0)
#endif // !LOG_FPRINTF

#ifdef NDEBUG // no logging in debug version
    #define LOG_D(fmt, ...) DUMMY_FUNCTION()
#else
    #define LOG_D(...)  _LOG_x("D",__VA_ARGS__)
#endif
    #define LOG_I(...)  _LOG_x("I",__VA_ARGS__)
    #define LOG_W(...)  _LOG_x("W",__VA_ARGS__)
    #define LOG_E(...)  _LOG_x("W",__VA_ARGS__)
    #define _LOG_E(...)  _LOG_x("E",__VA_ARGS__)
#endif // !defined(_LOG_E): neither Android nor Tizen


/** LOG_C() (critical) needs to be more prominent:
 * Display "*********** ERROR ***********" before actual error message.
 */
#define LOG_C(...) \
            do { \
                _LOG_E("  *****************************"); \
                _LOG_E("  *** ERROR: " __VA_ARGS__); \
                _LOG_E("  *** Detected in %s:%u()", __FILE__, __LINE__); \
                _LOG_E("  *****************************"); \
            } while(0)

#define LOG_ERRNO(MESSAGE) \
    LOG_E("%s failed with \"%s\"(errno %i)", MESSAGE, strerror(errno), errno);

#ifdef NDEBUG
    #define LOG_D_BUF(...)      DUMMY_FUNCTION()
#else
    #define LOG_D_BUF(f, b, s)  LOG_x_BUF(__FILE__, __LINE__, f, b, s, true)
#endif
#define LOG_I_BUF(f, b, s)      LOG_x_BUF(__FILE__, __LINE__, f, b, s, false)

#ifndef WIN32
__attribute__ ((unused))
#endif
static void LOG_x_BUF(
        const char*     file,
        int             line,
        const char*     message,
        const void*     blob,
        size_t          sizeOfBlob,
        bool            debug
) {

#define CPL         0x10  // chars per line
#define OVERHEAD    20

    char buffer[CPL * 4 + OVERHEAD];

    int index = 0;

    uint32_t moreThanOneLine = (sizeOfBlob > CPL);
    size_t blockLen = CPL;
    uint32_t addr = 0;
    uint32_t i = 0;

    if (debug) {
        LOG_D("Log buffer from %s:%d", file, line);
    } else {
        LOG_I("Log buffer from %s:%d", file, line);
    }

    if (NULL != message)
    {
        index += sprintf(&buffer[index], "%s", message);
    }

    if (moreThanOneLine)
    {
        if (NULL == message)
        {
            index += sprintf(&buffer[index], "memory dump");
        }
        index += sprintf(&buffer[index], " (%p, %zu bytes)", blob,sizeOfBlob);
        if (debug) {
            LOG_D("%s", buffer);
        } else {
            LOG_I("%s", buffer);
        }
        index = 0;
    }
    else if (NULL == message)
    {
        index += sprintf(&buffer[index], "Data at %p: ", blob);
    }

    if ((sizeOfBlob == 0) || (NULL == blob))
    {
        if (index) {
            if (debug) {
                LOG_D("%s", buffer);
            } else {
                LOG_I("%s", buffer);
            }
        }
    }
    else
    {
        while (sizeOfBlob > 0)
        {
            if (sizeOfBlob < blockLen)
            {
                blockLen = sizeOfBlob;
            }

            // address
            if (moreThanOneLine)
            {
                index += sprintf(&buffer[index], "0x%08X | ",addr);
                addr += CPL;
            }
            // bytes as hex
            for (i=0; i<blockLen; ++i)
            {
                index += sprintf(&buffer[index], "%02x ", ((const unsigned char *)blob)[i] );
            }
            // spaces if necessary
            if ((blockLen < CPL) && (moreThanOneLine))
            {
                // add spaces
                for (i=0; i<(3*(CPL-blockLen)); ++i) {
                    index += sprintf(&buffer[index], " ");
                }
            }
            // bytes as ASCII
            index += sprintf(&buffer[index], "| ");
            for (i=0; i<blockLen; ++i)
            {
                char c = ((const char *)blob)[i];
                index += sprintf(&buffer[index], "%c",(c>32)?c:'.');
            }

            blob = &(((const char *)blob)[blockLen]);
            sizeOfBlob -= blockLen;

            // print line to logcat / stdout
            if (debug) {
                LOG_D("%s", buffer);
            } else {
                LOG_I("%s", buffer);
            }
            index = 0;
        }
    }
}

#endif /** TLCWRAPPERANDROIDLOG_H_ */
