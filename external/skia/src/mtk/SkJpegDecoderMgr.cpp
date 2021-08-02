/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkJpegDecoderMgr_MTK.h"

#include "SkJpegUtility_MTK.h"

#ifdef SK_BUILD_FOR_ANDROID_FRAMEWORK
    #include "SkAndroidFrameworkUtils.h"
#endif

/*
 * Print information, warning, and error messages
 */
static void print_message_MTK(const j_common_ptr_ALPHA info, const char caller[]) {
    char buffer[JMSG_LENGTH_MAX];
    info->err->format_message(info, buffer);
    SkCodecPrintf("libjpeg error %d <%s> from %s\n", info->err->msg_code, buffer, caller);
}

/*
 * Reporting function for error and warning messages.
 */
static void output_message_MTK(j_common_ptr_ALPHA info) {
    print_message_MTK(info, "output_message");
}

static void progress_monitor_MTK(j_common_ptr_ALPHA info) {
  int scan = ((j_decompress_ptr_ALPHA)info)->input_scan_number;
  // Progressive images with a very large number of scans can cause the
  // decoder to hang.  Here we use the progress monitor to abort on
  // a very large number of scans.  100 is arbitrary, but much larger
  // than the number of scans we might expect in a normal image.
  if (scan >= 100) {
      skjpeg_err_exit_MTK(info);
  }
}

bool JpegDecoderMgr_MTK::returnFalse_MTK(const char caller[]) {
    print_message_MTK((j_common_ptr_ALPHA) &fDInfo, caller);
    return false;
}

SkCodec::Result JpegDecoderMgr_MTK::returnFailure_MTK(const char caller[], SkCodec::Result result) {
    print_message_MTK((j_common_ptr_ALPHA) &fDInfo, caller);
    return result;
}

bool JpegDecoderMgr_MTK::getEncodedColor_MTK(SkEncodedInfo::Color* outColor) {
    switch (fDInfo.jpeg_color_space) {
        case JCS_GRAYSCALE_ALPHA:
            *outColor = SkEncodedInfo::kGray_Color;
            return true;
        case JCS_YCbCr_ALPHA:
            *outColor = SkEncodedInfo::kYUV_Color;
            return true;
        case JCS_RGB_ALPHA:
#ifdef SK_BUILD_FOR_ANDROID_FRAMEWORK
            SkAndroidFrameworkUtils::SafetyNetLog("118372692");
#endif
            *outColor = SkEncodedInfo::kRGB_Color;
            return true;
        case JCS_YCCK_ALPHA:
            *outColor = SkEncodedInfo::kYCCK_Color;
            return true;
        case JCS_CMYK_ALPHA:
            *outColor = SkEncodedInfo::kInvertedCMYK_Color;
            return true;
        default:
            return false;
    }
}

JpegDecoderMgr_MTK::JpegDecoderMgr_MTK(SkStream* stream)
    : fSrcMgr(stream)
    , fInit(false)
{
    // Error manager must be set before any calls to libjeg in order to handle failures
    fDInfo.err = jpeg_std_error_ALPHA(&fErrorMgr);
    fErrorMgr.error_exit = skjpeg_err_exit_MTK;
}

void JpegDecoderMgr_MTK::init_MTK() {
    jpeg_create_decompress_ALPHA(&fDInfo);
    fInit = true;
    fDInfo.src = &fSrcMgr;
    fDInfo.err->output_message = &output_message_MTK;
    fDInfo.progress = &fProgressMgr;
    fProgressMgr.progress_monitor = &progress_monitor_MTK;
}

JpegDecoderMgr_MTK::~JpegDecoderMgr_MTK() {
    if (fInit) {
        jpeg_destroy_decompress_ALPHA(&fDInfo);
    }
}
