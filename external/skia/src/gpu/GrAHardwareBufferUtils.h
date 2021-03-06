/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef GrAHardwareBufferUtils_DEFINED
#define GrAHardwareBufferUtils_DEFINED

#include "SkTypes.h"

#if defined(SK_BUILD_FOR_ANDROID) && __ANDROID_API__ >= 26

#include "GrBackendSurface.h"
#include "GrTypes.h"

class GrContext;

extern "C" {
    typedef struct AHardwareBuffer AHardwareBuffer;
}

namespace GrAHardwareBufferUtils {

SkColorType GetSkColorTypeFromBufferFormat(uint32_t bufferFormat);

GrBackendFormat GetBackendFormat(GrContext* context, AHardwareBuffer* hardwareBuffer,
                                 uint32_t bufferFormat, bool requireKnownFormat);

typedef void* DeleteImageCtx;
typedef void (*DeleteImageProc)(DeleteImageCtx);

GrBackendTexture MakeBackendTexture(GrContext* context, AHardwareBuffer* hardwareBuffer,
                                    int width, int height,
                                    DeleteImageProc* deleteProc,
                                    DeleteImageCtx* deleteCtx,
                                    bool isProtectedContent,
                                    const GrBackendFormat& backendFormat,
                                    bool isRenderable);

void bindTextureImage(
        const GrBackendTexture& backTexture,
        GrContext* context, AHardwareBuffer* hardwareBuffer,
        bool isProtectedContent,
        DeleteImageProc* deleteProc,
        DeleteImageCtx* deleteCtx);
} // GrAHardwareBufferUtils


#endif
#endif
