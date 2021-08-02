/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkJpegDecoderMgr_MTK_DEFINED
#define SkJpegDecoderMgr_MTK_DEFINED

#include "SkCodec.h"
#include "SkCodecPriv.h"
#include <stdio.h>
#include "SkJpegUtility_MTK.h"

extern "C" {
    #include "jpeglib_alpha.h"
}

class JpegDecoderMgr_MTK : SkNoncopyable {
public:

    /*
     * Print a useful error message and return false
     */
    bool returnFalse_MTK(const char caller[]);

    /*
     * Print a useful error message and return a decode failure
     */
    SkCodec::Result returnFailure_MTK(const char caller[], SkCodec::Result result);

    /*
     * Create the decode manager
     * Does not take ownership of stream
     */
    JpegDecoderMgr_MTK(SkStream* stream);

    /*
     * Initialize decompress struct
     * Initialize the source manager
     */
    void  init_MTK();

    /*
     * Returns true if it successfully sets outColor to the encoded color,
     * and false otherwise.
     */
    bool getEncodedColor_MTK(SkEncodedInfo::Color* outColor);

    /*
     * Free memory used by the decode manager
     */
    ~JpegDecoderMgr_MTK();

    /*
     * Get the skjpeg_error_mgr in order to set an error return jmp_buf
     */
    skjpeg_error_mgr_MTK* errorMgr_MTK() { return &fErrorMgr; }

    /*
     * Get function for the decompress info struct
     */
    jpeg_decompress_struct_ALPHA* dinfo_MTK() { return &fDInfo; }

private:

    jpeg_decompress_struct_ALPHA fDInfo;
    skjpeg_source_mgr_MTK        fSrcMgr;
    skjpeg_error_mgr_MTK         fErrorMgr;
    jpeg_progress_mgr_ALPHA      fProgressMgr;
    bool                         fInit;
};

#endif
