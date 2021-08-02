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

#ifndef __SKBlITTERADATER_HANDLER_H__
#define __SKBlITTERADATER_HANDLER_H__
#include "SkColor.h"
#include "SkCoreBlitters.h"
#include "SkBlitterMTAdaptee.h"
#include "SkShaderBase.h"

struct BlitRectHandlerParam {
public:
    BlitRectHandlerParam(uint32_t* device, size_t deviceRB,
                            SkShaderBase::Context* shaderContext, SkPMColor* span,
                            bool constInY, bool shadeDirectlyIntoDevice,
                            SkXfermode* xfer, SkBlitRow::Proc32 proc,
                            SkBlitterMTAdaptee *MTAdaptee, const SkPixmap &devPixmap,
                            int srcWidth, int srcHeight)
                            : fDevice(device)
                            , fDeviceRB(deviceRB)
                            , fShaderContext(shaderContext)
                            , fSpan(span)
                            , fConstInY(constInY)
                            , fShadeDirectlyIntoDevice(shadeDirectlyIntoDevice)
                            , fXfer(xfer)
                            , fProc(proc)
                            , fMTAdaptee(MTAdaptee)
                            , fDevPixmap(devPixmap)
                            , fSrcWidthInPixref(srcWidth)
                            , fSrcHeightInPixref(srcHeight) {}

    uint32_t  *fDevice; //dst buffer start addr
    size_t    fDeviceRB; // dst rb
    SkShaderBase::Context *fShaderContext;
    SkPMColor        *fSpan; //transition buffer start addr. maybe not used and can remove
    bool fConstInY;
    bool fShadeDirectlyIntoDevice;
    SkXfermode* fXfer; // xfermode
    SkBlitRow::Proc32 fProc; //blitrow proc
    SkBlitterMTAdaptee *fMTAdaptee;
    const SkPixmap &fDevPixmap;
    int fSrcWidthInPixref;// for extension use
    int fSrcHeightInPixref;
};

class BlitHandler {
public:
    virtual void handleRequest(BlitRectHandlerParam *handlerParam,
                                        const int &x, int &y,
                                        const int &width, int &height) = 0;
    void setNextHandler(BlitHandler *ch) {fNextHandler = ch;}
    BlitHandler *getNextHandler() {return fNextHandler;}
    virtual ~BlitHandler(){}
protected:
    BlitHandler() : fNextHandler(nullptr) {}
private:
    BlitHandler *fNextHandler;
};

/*blit const in y direction*/
class ConstInYBlitHandler: public BlitHandler {
public:
    void handleRequest(BlitRectHandlerParam *handlerParam,
                        const int &x, int &y,
                        const int &width, int &height) override;
};

/*aosp flow*/
/*blit directly*/
#if 0
class BlitShadeDirectlyHandler: public BlitHandler {
public:
    void handleRequest(BlitRectHandlerParam *handlerParam,
                        const int &x, int &y,
                        const int &width, int &height) override;
};
#endif
/*blit indirectly with xfer*/
class BlitIndirectlyWithXferHandler: public BlitHandler {
public:
    void handleRequest(BlitRectHandlerParam *handlerParam,
                        const int &x, int &y,
                        const int &width, int &height) override;
};

/*blit indirectly without xfer*/
class BlitIndirectlyNoXferHandler: public BlitHandler {
public:
    void handleRequest(BlitRectHandlerParam *handlerParam,
                        const int &x, int &y,
                        const int &width, int &height) override;
};

/*split BlitShadeDirectlyHandler into 3 sub handler as follows
 *blit directly with shader proc*/
class BlitDirectlyWithSPHandler: public BlitHandler {
public:
    void handleRequest(BlitRectHandlerParam *handlerParam,
                        const int &x, int &y,
                        const int &width, int &height) override;
};

/*blit directly without shader proc*/
class BlitDirectlyNoSPHandler: public BlitHandler {
public:
    void handleRequest(BlitRectHandlerParam *handlerParam,
                        const int &x, int &y,
                        const int &width, int &height) override;
};

/*blit special cases directly without shader proc*/
class BlitSpecial2DHandler: public BlitHandler {
public:
    void handleRequest(BlitRectHandlerParam *handlerParam,
                        const int &x, int &y,
                        const int &width, int &height) override;
};

#endif
