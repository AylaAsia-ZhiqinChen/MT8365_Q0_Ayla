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

#include "SkBlitterMTAdapter.h"
#include "SkBlitterMTAdapterHandler.h"
//#include "SkTraceEvent.h"


#include "stdio.h"
#ifdef __MTK_TRACE_MT_BLITTER__
#include "MtkCommonTools.h"
#endif

//#define __ANDROID_DEBUG__
#if defined(__LOCAL_DEBUG__)
#define LOGD(fmt, ...) printf(fmt, __VA_ARGS__)
#elif defined(__ANDROID_DEBUG__)
#define TAG "skia_mt"
#include <android/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#else
#define LOGD(...)
#endif

SkBlitterMTAdapter::SkBlitterMTAdapter(const SkPixmap& device, const SkPaint& paint,
    	                            SkShaderBase::Context* shaderContext)
									: INHERITED(device, paint, shaderContext)
{
	fMtAdaptee = new SkBlitterMTAdaptee();
    /*create chain of responsiblity.*/
    ConstInYBlitHandler *constInYHandler = new ConstInYBlitHandler();

    BlitDirectlyWithSPHandler *nomalShadeWithSPHandler = new BlitDirectlyWithSPHandler();
    BlitDirectlyNoSPHandlerMT *nomalShadeNoSPHandlerMT = new BlitDirectlyNoSPHandlerMT();
    BlitDirectlyNoSPHandler *nomalShadeNoSPHandler = new BlitDirectlyNoSPHandler();
    BlitSpecial2DHandlerMT *nomalShade2DHandlerMT = new BlitSpecial2DHandlerMT();
    BlitSpecial2DHandler *nomalShade2DHandler = new BlitSpecial2DHandler();
    BlitIndirectlyWithXferHandler *blitIndirectlyWithXferHandler =
                                            new BlitIndirectlyWithXferHandler();
    BlitIndirectlyNoXferHandler *blitIndirectlyNoXferHandler =
                                            new BlitIndirectlyNoXferHandler();
    BlitIndirectlyNoXferHandlerMT *blitIndirectlyNoXferHandlerMT =
                                            new BlitIndirectlyNoXferHandlerMT();

    constInYHandler->setNextHandler(nomalShadeWithSPHandler);
    nomalShadeWithSPHandler->setNextHandler(nomalShadeNoSPHandlerMT);
    nomalShadeNoSPHandlerMT->setNextHandler(nomalShadeNoSPHandler);
    nomalShadeNoSPHandler->setNextHandler(nomalShade2DHandlerMT);
    nomalShade2DHandlerMT->setNextHandler(nomalShade2DHandler);
    nomalShade2DHandler->setNextHandler(blitIndirectlyWithXferHandler);
    blitIndirectlyWithXferHandler->setNextHandler(blitIndirectlyNoXferHandlerMT);
    blitIndirectlyNoXferHandlerMT->setNextHandler(blitIndirectlyNoXferHandler);
    blitIndirectlyNoXferHandler->setNextHandler(nullptr);

    fChain = constInYHandler;
}

SkBlitterMTAdapter::~SkBlitterMTAdapter() {
    BlitHandler *cur, *next;
    cur = fChain;
    do {
        next = cur->getNextHandler();
        delete cur;
        cur = next;
    } while(next);
	delete fMtAdaptee;
}
void SkBlitterMTAdapter::blitRect(int x, int y, int width, int height) {
#ifdef __MTK_TRACE_MT_BLITTER__
    MtkCommonTools::AutoSystrace autoTrace("SkBlitterMTAdapter::blitRect");
#endif
    /*if target blit rect is bigger than dst device, assert*/
    SkASSERT(x >= 0 && y >= 0 &&
                 x + width <= fDevice.width() && y + height <= fDevice.height());
    /*get source bitmap original width and height from pixelref*/
    int srcWidth = 0;//init to zero
    int srcHeight = 0;
#ifdef __MTK_AFFINE_TRANSFORM_EXT__

    if (!fShaderContext->fShader.getOrigBitmapInfo(srcWidth, srcHeight)) {
        LOGD("---cannot get src original bitmap w & h\n");
    }
#endif
    /*pack parameter*/
    BlitRectHandlerParam param(fDevice.writable_addr32(x, y), //dst pixel buffer start address
                                    fDevice.rowBytes(), //dst rb
                                    fShaderContext,
                                    fBuffer,
                                    fConstInY,
                                    fShadeDirectlyIntoDevice,
                                    fXfermode,
                                    fProc32,
                                    fMtAdaptee,// for mt optimization
                                    fDevice,//dst pixmap
                                    srcWidth,
                                    srcHeight);

    /*handle request*/
    fChain->handleRequest(&param, x, y, width, height);
}
