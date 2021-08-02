/*
 * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Created on: 2017-11-30
 * Author: Christ Sun
 */

#include "SkBlitterAdapterHandler.h"
#include "MtkCommonTools.h"
#include "SkBitmapProcShaderExt.h"


#define __ANDROID_DEBUG__
#if defined(__LOCAL_DEBUG__)
#define LOGD(fmt, ...) printf(fmt, __VA_ARGS__)
#elif defined(__ANDROID_DEBUG__)
#define TAG "skia_mt"
#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#else
#define LOGD(...)
#endif

void ConstInYBlitHandler::handleRequest(BlitRectHandlerParam *handlerParam,
                                                const int &x, int &y,
                                                const int &width, int &height) {
#ifdef __MTK_TRACE_MT_BLITTER__
    MtkCommonTools::AutoSystrace autoTrace("ConstInYBlitHandler");
#endif

    if (!handlerParam) {
        LOGD("---%s fatal error: blit handler param is nullptr\n", __PRETTY_FUNCTION__);
        return;
    }
    if (handlerParam->fConstInY) {
#if defined(__MTK_DUMP_DRAW_BITMAP__) || defined(__MTK_DUMP_DRAW_BITMAP_RECT__)
        LOGD("---%s\n", __PRETTY_FUNCTION__);
#endif
        if (handlerParam->fShadeDirectlyIntoDevice) {
            // shade the first row directly into the device
            handlerParam->fShaderContext->shadeSpan(x, y, handlerParam->fDevice, width);
            handlerParam->fSpan = handlerParam->fDevice;
            while (--height > 0) {
                handlerParam->fDevice = (uint32_t*)((char*)handlerParam->fDevice + handlerParam->fDeviceRB);
                memcpy(handlerParam->fDevice, handlerParam->fSpan, width << 2);
            }
        } else {
            handlerParam->fShaderContext->shadeSpan(x, y, handlerParam->fSpan, width);
            //SkXfermode* xfer = fXfermode;
            if (handlerParam->fXfer) {
                do {
                    handlerParam->fXfer->xfer32(handlerParam->fDevice, handlerParam->fSpan, width, nullptr);
                    y += 1;
                    handlerParam->fDevice = (uint32_t*)((char*)handlerParam->fDevice + handlerParam->fDeviceRB);
                } while (--height > 0);
            } else {
                //SkBlitRow::Proc32 proc = fProc32;
                do {
                    handlerParam->fProc(handlerParam->fDevice, handlerParam->fSpan, width, 255);
                    y += 1;
                    handlerParam->fDevice = (uint32_t*)((char*)handlerParam->fDevice + handlerParam->fDeviceRB);
                } while (--height > 0);
            }
        }
    } else {
#ifdef __MTK_TRACE_MT_BLITTER__
        autoTrace.end();
#endif
        BlitHandler *next = getNextHandler();
        if (next)
            next->handleRequest(handlerParam, x, y, width, height);
    }
}

#if 0
void BlitShadeDirectlyHandler::handleRequest(BlitRectHandlerParam *handlerParam,
                                                            const int &x, int &y,
                                                            const int &width, int &height) {
#ifdef __MTK_TRACE_MT_BLITTER__
    MtkCommonTools::AutoSystrace autoTrace("BlitShadeDirectlyHandler");
#endif

    if (!handlerParam) {
        LOGD("---%s fatal error: blit handler param is nullptr\n", __PRETTY_FUNCTION__);
        return;
    }

    if (!handlerParam->fConstInY && handlerParam->fShadeDirectlyIntoDevice) {
#if defined(__MTK_DUMP_DRAW_BITMAP__) || defined(__MTK_DUMP_DRAW_BITMAP_RECT__)
        LOGD("---%s\n", __PRETTY_FUNCTION__);
#endif
        void* ctx;
        auto shadeProc = handlerParam->fShaderContext->asAShadeProc(&ctx);
        if (shadeProc) {
            do {
                shadeProc(ctx, x, y, handlerParam->fDevice, width);
                y += 1;
                handlerParam->fDevice = (uint32_t*)((char*)handlerParam->fDevice + handlerParam->fDeviceRB);
            } while (--height > 0);
        } else {
        	if (!handlerParam->fShaderContext->shadeSpanRect(x, y, handlerParam->fDevice,
                                            handlerParam->fDeviceRB, width, height, handlerParam->fDevPixmap)) {
	            do {
	                handlerParam->fShaderContext->shadeSpan(x, y, handlerParam->fDevice, width);
	                y += 1;
	                handlerParam->fDevice = (uint32_t*)((char*)handlerParam->fDevice + handlerParam->fDeviceRB);
	            } while (--height > 0);
        	}
        }
    } else {
#ifdef __MTK_TRACE_MT_BLITTER__
        autoTrace.end();
#endif
        BlitHandler *next = getNextHandler();
        if (next)
            next->handleRequest(handlerParam, x, y, width, height);
    }
}
#endif
void BlitIndirectlyWithXferHandler::handleRequest(BlitRectHandlerParam *handlerParam,
                                            const int &x, int &y,
                                            const int &width, int &height) {
#ifdef __MTK_TRACE_MT_BLITTER__
    MtkCommonTools::AutoSystrace autoTrace("BlitIndirectlyWithXferHandler");
#endif

    if (!handlerParam) {
        LOGD("---%s fatal error: blit handler param is nullptr\n", __PRETTY_FUNCTION__);
        return;
    }

    if (!handlerParam->fConstInY && !handlerParam->fShadeDirectlyIntoDevice
        && handlerParam->fXfer) {

#if defined(__MTK_DUMP_DRAW_BITMAP__) || defined(__MTK_DUMP_DRAW_BITMAP_RECT__)
        LOGD("---%s\n", __PRETTY_FUNCTION__);
#endif
        do {
            handlerParam->fShaderContext->shadeSpan(x, y, handlerParam->fSpan, width);
            handlerParam->fXfer->xfer32(handlerParam->fDevice, handlerParam->fSpan, width, nullptr);
            y += 1;
            handlerParam->fDevice = (uint32_t*)((char*)handlerParam->fDevice + handlerParam->fDeviceRB);
        } while (--height > 0);
    } else {
#ifdef __MTK_TRACE_MT_BLITTER__
        autoTrace.end();
#endif
        BlitHandler *next = getNextHandler();
        if (next)
            next->handleRequest(handlerParam, x, y, width, height);
    }
}

void BlitIndirectlyNoXferHandler::handleRequest(BlitRectHandlerParam *handlerParam,
                                            const int &x, int &y,
                                            const int &width, int &height) {
#ifdef __MTK_TRACE_MT_BLITTER__
    MtkCommonTools::AutoSystrace autoTrace("BlitIndirectlyNoXferHandler");
#endif

    if (!handlerParam) {
        LOGD("---%s fatal error: blit handler param is nullptr\n", __PRETTY_FUNCTION__);
        return;
    }

    if (!handlerParam->fConstInY && !handlerParam->fShadeDirectlyIntoDevice
        && !handlerParam->fXfer) {
#if defined(__MTK_DUMP_DRAW_BITMAP__) || defined(__MTK_DUMP_DRAW_BITMAP_RECT__)
        LOGD("---%s\n", __PRETTY_FUNCTION__);
#endif
        do {
            handlerParam->fShaderContext->shadeSpan(x, y, handlerParam->fSpan, width);
            handlerParam->fProc(handlerParam->fDevice, handlerParam->fSpan, width, 255);
            y += 1;
            handlerParam->fDevice = (uint32_t*)((char*)handlerParam->fDevice + handlerParam->fDeviceRB);
        } while (--height > 0);
    } else {
#ifdef __MTK_TRACE_MT_BLITTER__
        autoTrace.end();
#endif
        BlitHandler *next = getNextHandler();
        if (next)
            next->handleRequest(handlerParam, x, y, width, height);
    }
}


void BlitDirectlyWithSPHandler::handleRequest(BlitRectHandlerParam *handlerParam,
                                                const int &x, int &y,
                                                const int &width, int &height) {
#ifdef __MTK_TRACE_MT_BLITTER__
    MtkCommonTools::AutoSystrace autoTrace("BlitDirectlyWithSPHandler");
#endif

    if (!handlerParam) {
        LOGD("---%s fatal error: blit handler param is nullptr\n", __PRETTY_FUNCTION__);
        return;
    }

    //void* ctx;
    //auto shadeProc = handlerParam->fShaderContext->asAShadeProc(&ctx);


#ifdef __MTK_TRACE_MT_BLITTER__
        autoTrace.end();
#endif
        BlitHandler *next = getNextHandler();
        if (next)
            next->handleRequest(handlerParam, x, y, width, height);
}

void BlitDirectlyNoSPHandler::handleRequest(BlitRectHandlerParam *handlerParam,
                                                const int &x, int &y,
                                                const int &width, int &height) {
#ifdef __MTK_TRACE_MT_BLITTER__
    MtkCommonTools::AutoSystrace autoTrace("BlitDirectlyNoSPHandler");
#endif
    if (!handlerParam) {
        LOGD("---%s fatal error: blit handler param is nullptr\n", __PRETTY_FUNCTION__);
        return;
    }

    //void* ctx;
    //auto shadeProc = handlerParam->fShaderContext->asAShadeProc(&ctx);
    SkBitmapProcShaderExt ext(handlerParam->fShaderContext,
                                handlerParam->fSrcWidthInPixref,
                                handlerParam->fSrcHeightInPixref);
    if (!handlerParam->fConstInY && handlerParam->fShadeDirectlyIntoDevice
       && !ext.meetMtkExtCondition()) {
#if defined(__MTK_DUMP_DRAW_BITMAP__) || defined(__MTK_DUMP_DRAW_BITMAP_RECT__)
        LOGD("-----%s \n", __PRETTY_FUNCTION__);
#endif
        do {
            handlerParam->fShaderContext->shadeSpan(x, y, handlerParam->fDevice, width);
            y += 1;
            handlerParam->fDevice = (uint32_t*)((char*)handlerParam->fDevice + handlerParam->fDeviceRB);
        } while (--height > 0);
    } else {
#ifdef __MTK_TRACE_MT_BLITTER__
        autoTrace.end();
#endif
        BlitHandler *next = getNextHandler();
        if (next)
            next->handleRequest(handlerParam, x, y, width, height);
    }
}

void BlitSpecial2DHandler::handleRequest(BlitRectHandlerParam *handlerParam,
                                                    const int &x, int &y,
                                                    const int &width, int &height) {
#ifdef __MTK_TRACE_MT_BLITTER__
    MtkCommonTools::AutoSystrace autoTrace("BlitSpecial2DHandler");
#endif

    if (!handlerParam) {
        LOGD("---%s fatal error: blit handler param is nullptr\n", __PRETTY_FUNCTION__);
        return;
    }

    //void* ctx;
    //auto shadeProc = handlerParam->fShaderContext->asAShadeProc(&ctx);
    SkBitmapProcShaderExt ext(handlerParam->fShaderContext,
                                handlerParam->fSrcWidthInPixref,
                                handlerParam->fSrcHeightInPixref);

    if (!handlerParam->fConstInY && handlerParam->fShadeDirectlyIntoDevice &&
        ext.meetMtkExtCondition()) {
#if defined(__MTK_DUMP_DRAW_BITMAP__) || defined(__MTK_DUMP_DRAW_BITMAP_RECT__)
        LOGD("---%s\n", __PRETTY_FUNCTION__);
#endif

	    ext.shadeSpanRect(x, y, handlerParam->fDevice,
                            handlerParam->fDeviceRB, width, height,
                            handlerParam->fDevPixmap);
    } else {
#ifdef __MTK_TRACE_MT_BLITTER__
        autoTrace.end();
#endif
        BlitHandler *next = getNextHandler();
        if (next)
            next->handleRequest(handlerParam, x, y, width, height);
    }
}
