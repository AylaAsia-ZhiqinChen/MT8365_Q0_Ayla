/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkImageDecoder.h"
#include "SkBitmap.h"
#include "SkImagePriv.h"
#include "SkPixelRef.h"
#include "SkStream.h"
#include "SkTemplates.h"
#include "SkCanvas.h"
#include "SkBitmapDevice.h"
#include "SkDraw.h"
#include "SkImageInfo.h"
#include <stdio.h>
#include <cutils/properties.h>
#include <cutils/log.h>

//#define ATRACE_TAG ATRACE_TAG_GRAPHICS
#include <utils/Trace.h>

SkImageDecoder::SkImageDecoder()
    : fPeeker(nullptr)
    , fAllocator(nullptr)
    , fSampleSize(1)
    , fDefaultPref(kUnknown_SkColorType)
    , fPreserveSrcDepth(false)
    , fDitherImage(false)
    , fSkipWritingZeroes(false)
    , fPreferQualityOverSpeed(false)
    , fRequireUnpremultipliedColors(false)
    , fISOSpeedRatings(0)
    , fEncodedInfo(SkEncodedInfo::Make(0,0,SkEncodedInfo::kYUV_Color, SkEncodedInfo::kOpaque_Alpha, 8))
    , fImageInfo(SkImageInfo())
    , fPreferSize(0)
    , fPostProc(0)
    , fdc(NULL) {
}

SkImageDecoder::~SkImageDecoder() {
    SkSafeUnref(fPeeker);
    SkSafeUnref(fAllocator);
}

void SkImageDecoder::copyFieldsToOther(SkImageDecoder* other) {
    if (nullptr == other) {
        return;
    }
    other->setPeeker(fPeeker);
    other->setAllocator(fAllocator);
    other->setSampleSize(fSampleSize);
    other->setPreserveSrcDepth(fPreserveSrcDepth);
    other->setDitherImage(fDitherImage);
    other->setSkipWritingZeroes(fSkipWritingZeroes);
    other->setPreferQualityOverSpeed(fPreferQualityOverSpeed);
    other->setRequireUnpremultipliedColors(fRequireUnpremultipliedColors);
}

SkImageDecoder::Format SkImageDecoder::getFormat() const {
    return kUnknown_Format;
}

SkImageInfo SkImageDecoder::getImageInfo(sk_sp<SkColorSpace>) const{
    return SkImageInfo();
}

const char* SkImageDecoder::getFormatName() const {
    return GetFormatName(this->getFormat());
}

const char* SkImageDecoder::GetFormatName(Format format) {
    switch (format) {
        case kUnknown_Format:
            return "Unknown Format";
        case kBMP_Format:
            return "BMP";
        case kGIF_Format:
            return "GIF";
        case kICO_Format:
            return "ICO";
        case kPKM_Format:
            return "PKM";
        case kKTX_Format:
            return "KTX";
        case kASTC_Format:
            return "ASTC";
        case kJPEG_Format:
            return "JPEG";
        case kPNG_Format:
            return "PNG";
        case kWBMP_Format:
            return "WBMP";
        case kWEBP_Format:
            return "WEBP";
        default:
            SkDEBUGFAIL("Invalid format type!");
    }
    return "Unknown Format";
}

SkPngChunkReader* SkImageDecoder::setPeeker(SkPngChunkReader* peeker) {
    fPeeker = peeker;
    return peeker;
}

SkBitmap::Allocator* SkImageDecoder::setAllocator(SkBitmap::Allocator* alloc) {
    fAllocator = alloc;
    return alloc;
}

void SkImageDecoder::setSampleSize(int size) {
    if (size < 1) {
        size = 1;
    }
    fSampleSize = size;
}

bool SkImageDecoder::allocPixelRef(SkBitmap* bitmap,
                                   SkColorTable* ctable, SkBRDAllocator* allocator) const {
    if(allocator)
        return bitmap->tryAllocPixels(allocator);
    else
        return bitmap->tryAllocPixels(fAllocator);
}

SkColorType SkImageDecoder::getPrefColorType(SrcDepth srcDepth, bool srcHasAlpha) const {
    SkColorType ct = fDefaultPref;
    if (fPreserveSrcDepth) {
        switch (srcDepth) {
            case k8BitGray_SrcDepth:
                ct = kN32_SkColorType;
                break;
            case k32Bit_SrcDepth:
                ct = kN32_SkColorType;
                break;
            case kIndex_SrcDepth:
                break;
        }
    }
    return ct;
}

SkColorType SkImageDecoder::computeOutputColorType(SkColorType requestedColorType) {
    bool highPrecision = false;
    switch (requestedColorType) {
        case kARGB_4444_SkColorType:
            return kN32_SkColorType;
        case kN32_SkColorType:
            break;
        case kAlpha_8_SkColorType:
            // Fall through to kGray_8.  Before kGray_8_SkColorType existed,
            // we allowed clients to request kAlpha_8 when they wanted a
            // grayscale decode.
        case kGray_8_SkColorType:
                return kGray_8_SkColorType;
            break;
        case kRGB_565_SkColorType:
                return kRGB_565_SkColorType;
            break;
        case kRGBA_F16_SkColorType:
            return kRGBA_F16_SkColorType;
        default:
            break;
    }

    // F16 is the Android default for high precision images.
    return highPrecision ? kRGBA_F16_SkColorType : kN32_SkColorType;
}

/**
 *  Loads the gamut as a set of three points (triangle).
 */
static void load_gamut(SkPoint rgb[], const skcms_Matrix3x3& xyz) {
    // rx = rX / (rX + rY + rZ)
    // ry = rY / (rX + rY + rZ)
    // gx, gy, bx, and gy are calulcated similarly.
    for (int rgbIdx = 0; rgbIdx < 3; rgbIdx++) {
        float sum = xyz.vals[rgbIdx][0] + xyz.vals[rgbIdx][1] + xyz.vals[rgbIdx][2];
        rgb[rgbIdx].fX = xyz.vals[rgbIdx][0] / sum;
        rgb[rgbIdx].fY = xyz.vals[rgbIdx][1] / sum;
    }
}

/**
 *  Calculates the area of the triangular gamut.
 */
static float calculate_area(SkPoint abc[]) {
    SkPoint a = abc[0];
    SkPoint b = abc[1];
    SkPoint c = abc[2];
    return 0.5f * SkTAbs(a.fX*b.fY + b.fX*c.fY - a.fX*c.fY - c.fX*b.fY - b.fX*a.fY);
}

static const float kSRGB_D50_GamutArea = 0.084f;

static bool is_wide_gamut(const skcms_ICCProfile& profile) {
    // Determine if the source image has a gamut that is wider than sRGB.  If so, we
    // will use P3 as the output color space to avoid clipping the gamut.
    if (profile.has_toXYZD50) {
        SkPoint rgb[3];
        load_gamut(rgb, profile.toXYZD50);
        return calculate_area(rgb) > kSRGB_D50_GamutArea;
    }

    return false;
}


sk_sp<SkColorSpace> SkImageDecoder::computeOutputColorSpace(SkColorType outputColorType,
                                                            sk_sp<SkColorSpace> prefColorSpace) {

    switch (outputColorType) {
        case kRGBA_8888_SkColorType:
        case kBGRA_8888_SkColorType: {
            // If |prefColorSpace| is supported, choose it.
            if (prefColorSpace) {
                return prefColorSpace;
            }

            const skcms_ICCProfile* encodedProfile = fEncodedInfo.profile();
            if (encodedProfile) {
                if (auto encodedSpace = SkColorSpace::Make(*encodedProfile)) {
                    // Leave the pixels in the encoded color space.  Color space conversion
                    // will be handled after decode time.
                    return encodedSpace;
                }

                if (is_wide_gamut(*encodedProfile)) {
                    return SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, SkNamedGamut::kDCIP3);
                }
            }
            return SkColorSpace::MakeSRGB();
        }
        case kRGBA_F16_SkColorType:
            // Note that |prefColorSpace| is ignored, F16 is always linear sRGB.
            return SkColorSpace::MakeSRGBLinear();
        case kRGB_565_SkColorType:
            // Note that |prefColorSpace| is ignored, 565 is always sRGB.
            return SkColorSpace::MakeSRGB();
        default:
            // Color correction not supported for kGray.
            return nullptr;
    }
}

bool SkImageDecoder::buildTileIndex(SkStreamRewindable* stream, int *width, int *height) {
    // we reset this to false before calling onBuildTileIndex
    fShouldCancelDecode = false;

    return this->onBuildTileIndex(stream, width, height);
}

bool SkImageDecoder::onBuildTileIndex(SkStreamRewindable* stream, int* /*width*/,
                                      int* /*height*/) {
    delete stream;
    return false;
}


bool SkImageDecoder::cropBitmap(SkBitmap *dst, SkBitmap *src, int sampleSize,
                                int dstX, int dstY, int width, int height,
                                int srcX, int srcY, SkBRDAllocator* allocator) {
    int w = width / sampleSize;
    int h = height / sampleSize;

    // if the destination has no pixels then we must allocate them.
    if(sampleSize > 1 && width > 0 && w == 0) {
        ALOGW("Skia::cropBitmap W/H %d %d->%d %d, Sample %d, force width != 0 !!!!!!\n", width, height,w, h, sampleSize );
        w = 1;
    }
    if(sampleSize > 1 && height > 0 && h == 0) {
        ALOGW("Skia::cropBitmap W/H %d %d->%d %d, Sample %d, force height != 0 !!!!!!\n", width, height,w, h, sampleSize );
        h = 1;
    }

    // if the destination has no pixels then we must allocate them.
    if (dst->isNull()) {
        dst->setInfo(src->info().makeWH(w, h));

        if (!this->allocPixelRef(dst, NULL, allocator)) {
            SkDebugf(("failed to allocate pixels needed to crop the bitmap"));
            return false;
        }
    }
    // check to see if the destination is large enough to decode the desired
    // region. If this assert fails we will just draw as much of the source
    // into the destination that we can.
    if (dst->width() < w || dst->height() < h) {
        SkDebugf(("SkImageDecoder::cropBitmap does not have a large enough bitmap.\n"));
    }

    // Set the Src_Mode for the paint to prevent transparency issue in the
    // dest in the event that the dest was being re-used.
    SkPaint paint;
    SkCanvas canvas(*dst);
    canvas.drawBitmap(*src, (srcX - dstX) / sampleSize,
                            (srcY - dstY) / sampleSize,
                            &paint);
    return true;
}

void SkImageDecoder::setPreferSize(int size) {
    if (size < 0) {
        size = 0;
    }
    fPreferSize = size;
}

void SkImageDecoder::setPostProcFlag(int flag) {
    fPostProc = flag;
}

bool SkImageDecoder::decodeSubset(SkBitmap* bm, SkBRDAllocator* allocator, const SkIRect& rect, SkColorType pref,
                                  int sampleSize, sk_sp<SkColorSpace> dstColorSpace) {
    // we reset this to false before calling onDecodeSubset
    fShouldCancelDecode = false;
    // assign this, for use by getPrefColorType(), in case fUsePrefTable is false
    fDefaultPref = pref;
    SkDebugf("multi onDecodeSubset, bm=%p, pref=%d, sample=%d, format=%s, rect=(%d, %d ,%d ,%d)\n",
        bm, pref, sampleSize, this->getFormatName(), rect.left(), rect.top(), rect.width(), rect.height());
    if (! this->onDecodeSubset(bm, allocator, rect, sampleSize, dstColorSpace)) {
        return false;
    }
    SkDebugf("multi decodeSubset %s End,return true",this->getFormatName());
    return true;
}
