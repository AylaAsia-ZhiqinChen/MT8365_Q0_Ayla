/*
 * Copyright 2010 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkJpegUtility_MTK_DEFINED
#define SkJpegUtility_MTK_DEFINED

//#include "SkJpegPriv.h"
#include "SkImageDecoder.h"
#include "SkStream.h"
#include "SkTArray.h"

#include <setjmp.h>
// stdio is needed for jpeglib
//#include <stdio.h>

extern "C" {
    #include "jpeglib_alpha.h"
    #include "jerror_alpha.h"
}

/*
 * MTK error-handling struct
*/
struct skjpeg_error_mgr_MTK : jpeg_error_mgr_ALPHA {
    class AutoPushJmpBuf {
    public:
        AutoPushJmpBuf(skjpeg_error_mgr_MTK* mgr) : fMgr(mgr) {
            fMgr->fJmpBufStack.push_back(&fJmpBuf);
        }
        ~AutoPushJmpBuf() {
            SkASSERT(fMgr->fJmpBufStack.back() == &fJmpBuf);
            fMgr->fJmpBufStack.pop_back();
        }
        operator jmp_buf&() { return fJmpBuf; }

    private:
        skjpeg_error_mgr_MTK* const fMgr;
        jmp_buf fJmpBuf;
    };

    SkSTArray<4, jmp_buf*> fJmpBufStack;
};

/*
 * Error handling function
 */
void skjpeg_err_exit_MTK(j_common_ptr_ALPHA cinfo);

/*
 * Source handling struct for that allows libjpeg to use our stream object
 */
struct skjpeg_source_mgr_MTK : jpeg_source_mgr_ALPHA {

    skjpeg_source_mgr_MTK(SkStream* stream, SkImageDecoder* decoder);
    skjpeg_source_mgr_MTK(SkStream* stream);

    // Unowned.
    SkStream* fStream;
    // Unowned pointer to the decoder, used to check if the decoding process
    // has been cancelled.
    SkImageDecoder* fDecoder;
    enum {
        // TODO (msarett): Experiment with different buffer sizes.
        // This size was chosen because it matches SkImageDecoder.
        kBufferSize = 1024
    };
    char fBuffer[kBufferSize];
};

#endif
