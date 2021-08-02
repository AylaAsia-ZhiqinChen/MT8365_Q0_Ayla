/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkEncodedImageFormat_DEFINED
#define SkEncodedImageFormat_DEFINED

#include <stdint.h>

/**
 *  Enum describing format of encoded data.
 */
enum class SkEncodedImageFormat {
#ifdef SK_BUILD_FOR_GOOGLE3
    kUnknown,
#endif
    kBMP,
    kGIF,
    kICO,
    kJPEG,
    kPNG,
    kWBMP,
    kWEBP,
    kPKM,
    kKTX,
    kASTC,
    kDNG,
    kHEIF,
    kJPEG_MTK,
};

#endif  // SkEncodedImageFormat_DEFINED
