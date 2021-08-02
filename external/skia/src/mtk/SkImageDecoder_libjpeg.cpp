/*
 * Copyright (C) 2019 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
 */
 /*
 * Copyright 2007 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkImageDecoder.h"
#include "SkImageEncoder.h"
#include "SkJpegUtility_MTK.h"
#include "SkJPEGWriteUtility.h"
#include "SkColorPriv.h"
#include "SkScaledBitmapSampler.h"
#include "SkStream.h"
#include "SkTemplates.h"
#include "SkTime.h"
#include "SkUtils.h"
#include "SkRect.h"
#include "SkCanvas.h"
#include "SkMath.h"
#include "SkJpegCodec_MTK.h"
#include "native_handle.h"

#include <sys/mman.h>
#include <cutils/ashmem.h>

#ifdef MTK_JPEG_HW_REGION_RESIZER
  #include "Trace.h"
  #include <linux/ion.h>
  #include <ion/ion.h>
  #include "DpColorFormat.h"
  #include <vendor/mediatek/hardware/mms/1.3/IMms.h>
  #include <vendor/mediatek/hardware/mms/1.2/IMms.h>
  using ::vendor::mediatek::hardware::mms::V1_3::IMms;
  using ::vendor::mediatek::hardware::mms::V1_3::MDPParamFD;
  using ::vendor::mediatek::hardware::mms::V1_2::MMS_PROFILE_ENUM;
  using ::vendor::mediatek::hardware::mms::V1_2::MMS_MEDIA_TYPE_ENUM;
  using namespace android;
  #define MTK_SKIA_USE_ION
  #define ION_HEAP_MULTIMEDIA_MASK (1 << 10)
  #define ION_HEAP_MULTIMEDIA_MAP_MVA_MASK (1 << 14)
#endif

#include <cutils/properties.h>
#include <cutils/log.h>
#include <stdlib.h>

#undef LOG_TAG
#define LOG_TAG "skia"

#include "SkMutex.h"

static SkMutex  gAutoTileInitMutex;
static SkMutex  gAutoTileResizeMutex;

#include <stdio.h>
extern "C" {
    #include "jpeglib_alpha.h"
    #include "jerror_alpha.h"
}

const uint32_t kExifMarker = JPEG_APP0 + 1;
static void initialize_info(jpeg_decompress_struct_ALPHA* cinfo, skjpeg_source_mgr_MTK* src_mgr) {
    SkASSERT(cinfo != nullptr);
    SkASSERT(src_mgr != nullptr);
    jpeg_create_decompress_ALPHA(cinfo);
    jpeg_save_markers_ALPHA(cinfo, kExifMarker, 0xFFFF);
    jpeg_save_markers_ALPHA(cinfo, kICCMarker, 0xFFFF);
    cinfo->src = src_mgr;
#ifdef MTK_JPEG_SW_OPTIMIZATION
    char value[PROPERTY_VALUE_MAX];
    int mOptOn, mOptSwitch;
    property_get("ro.vendor.jpeg_decode_sw_opt", value, "0");
    mOptOn = atoi(value);
    property_get("debug.jpegregion.opt.switch", value, "1");
    mOptSwitch = atoi(value);
    if(mOptOn == 1 && mOptSwitch == 1) {
        cinfo->async_index_builder = true; // Enable SW region decode opt.
    }
#endif
}

static bool is_icc_marker(jpeg_marker_struct_ALPHA* marker) {
    if (kICCMarker != marker->marker || marker->data_length < kICCMarkerHeaderSize) {
        return false;
    }

    return !memcmp(marker->data, kICCSig, sizeof(kICCSig));
}

/*
 * ICC profiles may be stored using a sequence of multiple markers.  We obtain the ICC profile
 * in two steps:
 *     (1) Discover all ICC profile markers and verify that they are numbered properly.
 *     (2) Copy the data from each marker into a contiguous ICC profile.
 */
static std::unique_ptr<SkEncodedInfo::ICCProfile> read_color_profile(jpeg_decompress_struct_ALPHA* dinfo)
{
    // Note that 256 will be enough storage space since each markerIndex is stored in 8-bits.
    jpeg_marker_struct_ALPHA* markerSequence[256];
    memset(markerSequence, 0, sizeof(markerSequence));
    uint8_t numMarkers = 0;
    size_t totalBytes = 0;

    // Discover any ICC markers and verify that they are numbered properly.
    for (jpeg_marker_struct_ALPHA* marker = dinfo->marker_list; marker; marker = marker->next) {
        if (is_icc_marker(marker)) {
            // Verify that numMarkers is valid and consistent.
            if (0 == numMarkers) {
                numMarkers = marker->data[13];
                if (0 == numMarkers) {
                    SkDebugf("ICC Profile Error: numMarkers must be greater than zero.\n");
                    return nullptr;
                }
            } else if (numMarkers != marker->data[13]) {
                SkDebugf("ICC Profile Error: numMarkers must be consistent.\n");
                return nullptr;
            }

            // Verify that the markerIndex is valid and unique.  Note that zero is not
            // a valid index.
            uint8_t markerIndex = marker->data[12];
            if (markerIndex == 0 || markerIndex > numMarkers) {
                SkDebugf("ICC Profile Error: markerIndex is invalid.\n");
                return nullptr;
            }
            if (markerSequence[markerIndex]) {
                SkDebugf("ICC Profile Error: Duplicate value of markerIndex.\n");
                return nullptr;
            }
            markerSequence[markerIndex] = marker;
            SkASSERT(marker->data_length >= kICCMarkerHeaderSize);
            totalBytes += marker->data_length - kICCMarkerHeaderSize;
        }
    }

    if (0 == totalBytes) {
        // No non-empty ICC profile markers were found.
        return nullptr;
    }

    // Combine the ICC marker data into a contiguous profile.
    sk_sp<SkData> iccData = SkData::MakeUninitialized(totalBytes);
    void* dst = iccData->writable_data();
    for (uint32_t i = 1; i <= numMarkers; i++) {
        jpeg_marker_struct_ALPHA* marker = markerSequence[i];
        if (!marker) {
            SkDebugf("ICC Profile Error: Missing marker %d of %d.\n", i, numMarkers);
            return nullptr;
        }

        void* src = SkTAddOffset<void>(marker->data, kICCMarkerHeaderSize);
        size_t bytes = marker->data_length - kICCMarkerHeaderSize;
        memcpy(dst, src, bytes);
        dst = SkTAddOffset<void>(dst, bytes);
    }

    return SkEncodedInfo::ICCProfile::Make(std::move(iccData));
}

#define MAX_LIBJPEG_AUTO_NUM 32
class JpgLibAutoClean {
public:
    JpgLibAutoClean(): idx(-1) {}
    ~JpgLibAutoClean() {
      int i;
        for(i = idx; i >= 0; i--) {
          if (ptr[i]) {
              free(ptr[i]);
          }
        }
    }
    void set(void* s) {
        idx ++;
        ptr[idx] = s;
    }

private:
    void* ptr[MAX_LIBJPEG_AUTO_NUM];
    int idx;
};

class JpgStreamAutoClean {
public:
    JpgStreamAutoClean(): ptr(nullptr) {}
    ~JpgStreamAutoClean() {
        if (ptr) {
            delete ptr;
        }
    }
    void set(SkStream* s) {
        ptr = s;
    }
private:
    SkStream* ptr;
};

/* Automatically clean up after throwing an exception */
class JPEGAutoClean {
public:
    JPEGAutoClean(): cinfo_ptr(nullptr) {}
    ~JPEGAutoClean() {
        if (cinfo_ptr) {
            jpeg_destroy_decompress_ALPHA(cinfo_ptr);
        }
    }
    void set(jpeg_decompress_struct_ALPHA* info) {
        cinfo_ptr = info;
    }
private:
    jpeg_decompress_struct_ALPHA* cinfo_ptr;
};

class SkJPEGImageIndex {
public:
    // Takes ownership of stream.
    SkJPEGImageIndex(SkStreamRewindable* stream, SkImageDecoder* decoder)
        :
          mtkStream(nullptr)
        , fSrcMgr(stream, decoder)
        , fStream(stream)
        , fInfoInitialized(false)
        , fHuffmanCreated(false)
        , fDecompressStarted(false)
        {
            SkDEBUGCODE(fReadHeaderSucceeded = false;)
        }

    ~SkJPEGImageIndex() {
        if (fHuffmanCreated) {
            // Set to false before calling the libjpeg function, in case
            // the libjpeg function calls longjmp. Our setjmp handler may
            // attempt to delete this SkJPEGImageIndex, thus entering this
            // destructor again. Setting fHuffmanCreated to false first
            // prevents an infinite loop.
            fHuffmanCreated = false;
            jpeg_destroy_huffman_index_ALPHA(&fHuffmanIndex);
        }
        if (fDecompressStarted) {
            // Like fHuffmanCreated, set to false before calling libjpeg
            // function to prevent potential infinite loop.
            fDecompressStarted = false;
            jpeg_finish_decompress_ALPHA(&fCInfo);
        }
        if (fInfoInitialized) {
            this->destroyInfo();
        }
        if(mtkStream) delete mtkStream;
    }

    /**
     *  Destroy the cinfo struct.
     *  After this call, if a huffman index was already built, it
     *  can be used after calling initializeInfoAndReadHeader
     *  again. Must not be called after startTileDecompress except
     *  in the destructor.
     */
    void destroyInfo() {
        SkASSERT(fInfoInitialized);
        SkASSERT(!fDecompressStarted);
        // Like fHuffmanCreated, set to false before calling libjpeg
        // function to prevent potential infinite loop.
        fInfoInitialized = false;
        jpeg_destroy_decompress_ALPHA(&fCInfo);
        SkDEBUGCODE(fReadHeaderSucceeded = false;)
    }

    /**
     *  Initialize the cinfo struct.
     *  Calls jpeg_create_decompress, makes customizations, and
     *  finally calls jpeg_read_header. Returns true if jpeg_read_header
     *  returns JPEG_HEADER_OK.
     *  If cinfo was already initialized, destroyInfo must be called to
     *  destroy the old one. Must not be called after startTileDecompress.
     */
    bool initializeInfoAndReadHeader() {
        SkASSERT(!fInfoInitialized && !fDecompressStarted);
        initialize_info(&fCInfo, &fSrcMgr);
        fInfoInitialized = true;
        const bool success = (JPEG_HEADER_OK == jpeg_read_header_ALPHA(&fCInfo, true));
        SkDEBUGCODE(fReadHeaderSucceeded = success;)
        return success;
    }

    /**
     *  reset stream offset in fSrcMgr
     */
    void resetStream(){
        fSrcMgr.bytes_in_buffer = fSrcMgr.current_offset;
        fSrcMgr.next_input_byte = fSrcMgr.start_input_byte;
    }

    jpeg_decompress_struct_ALPHA* cinfo() { return &fCInfo; }

    huffman_index_ALPHA* huffmanIndex() { return &fHuffmanIndex; }

    /**
     *  Build the index to be used for tile based decoding.
     *  Must only be called after a successful call to
     *  initializeInfoAndReadHeader and must not be called more
     *  than once.
     */
    bool buildHuffmanIndex() {
        SkASSERT(fReadHeaderSucceeded);
        SkASSERT(!fHuffmanCreated);
        jpeg_create_huffman_index_ALPHA(&fCInfo, &fHuffmanIndex);
        SkASSERT(1 == fCInfo.scale_num && 1 == fCInfo.scale_denom);
        fHuffmanCreated = jpeg_build_huffman_index_ALPHA(&fCInfo, &fHuffmanIndex);
        return fHuffmanCreated;
    }

    /**
     *  Start tile based decoding. Must only be called after a
     *  successful call to buildHuffmanIndex, and must only be
     *  called once.
     */
    bool startTileDecompress() {
        SkASSERT(fHuffmanCreated);
        SkASSERT(fReadHeaderSucceeded);
        SkASSERT(!fDecompressStarted);
        if (jpeg_start_tile_decompress_ALPHA(&fCInfo)) {
            fDecompressStarted = true;
            return true;
        }
        return false;
    }

    SkMemoryStream *mtkStream ;

private:
    skjpeg_source_mgr_MTK  fSrcMgr;
    SkAutoTDelete<SkStream> fStream;
    jpeg_decompress_struct_ALPHA fCInfo;
    huffman_index_ALPHA fHuffmanIndex;
    bool fInfoInitialized;
    bool fHuffmanCreated;
    bool fDecompressStarted;
    SkDEBUGCODE(bool fReadHeaderSucceeded;)
};

class SkJPEGImageDecoder : public SkImageDecoder {
public:
    SkJPEGImageDecoder() {
        fImageIndex = nullptr;
        fImageWidth = 0;
        fImageHeight = 0;

    #ifdef MTK_SKIA_USE_ION
        fIonClientHnd = ion_open();
        if (fIonClientHnd < 0)
        {
            SkDebugf("ion_open failed\n");
            fIonClientHnd = -1;
        }
    #endif
    }

    virtual ~SkJPEGImageDecoder() override {
        delete fImageIndex;

    #ifdef MTK_SKIA_USE_ION
        if (fIonClientHnd != -1)
            ion_close(fIonClientHnd);
    #endif
    }

    Format getFormat() const override {
        return kJPEG_Format;
    }

    SkImageInfo getImageInfo(sk_sp<SkColorSpace> prefColorSpace) const override {
        return fEncodedInfo.makeImageInfo();
    }

protected:
    bool onBuildTileIndex(SkStreamRewindable *stream, int *width, int *height) override;

private:
    SkJPEGImageIndex* fImageIndex;
    int fImageWidth;
    int fImageHeight;

    /**
     *  Determine the appropriate bitmap colortype and out_color_space based on
     *  both the preference of the caller and the jpeg_color_space on the
     *  jpeg_decompress_struct passed in.
     *  Must be called after jpeg_read_header.
     */
    SkColorType getBitmapColorType(jpeg_decompress_struct_ALPHA*);

    typedef SkImageDecoder INHERITED;

#ifdef MTK_JPEG_HW_REGION_RESIZER
    bool fFirstTileDone = false;
    bool fUseHWResizer = false;
    int fIonClientHnd = 0;
#endif

protected:
    virtual bool onDecodeSubset(SkBitmap* bitmap, SkBRDAllocator* allocator, const SkIRect& rect, int isampleSize, sk_sp<SkColorSpace> dstColorSpace) override;
};

/*  If we need to better match the request, we might examine the image and
     output dimensions, and determine if the downsampling jpeg provided is
     not sufficient. If so, we can recompute a modified sampleSize value to
     make up the difference.

     To skip this additional scaling, just set sampleSize = 1; below.
 */
static int recompute_sampleSize(int sampleSize,
                                const jpeg_decompress_struct_ALPHA& cinfo) {
    return sampleSize * cinfo.output_width / cinfo.image_width;
}

static bool skip_src_rows_tile(jpeg_decompress_struct_ALPHA* cinfo,
                               huffman_index_ALPHA *index, void* buffer, int count) {
    for (int i = 0; i < count; i++) {
        JSAMPLE_ALPHA* rowptr = (JSAMPLE_ALPHA*)buffer;
        int row_count = jpeg_read_tile_scanline_ALPHA(cinfo, index, &rowptr);
        if (1 != row_count) {
            return false;
        }
    }
    return true;
}

extern unsigned int getISOSpeedRatings(void *buffer, unsigned int size);

// This guy exists just to aid in debugging, as it allows debuggers to just
// set a break-point in one place to see all error exists.
static void print_jpeg_decoder_errors(const jpeg_decompress_struct_ALPHA& cinfo,
                         int width, int height, const char caller[]) {
    char buffer[JMSG_LENGTH_MAX];
    cinfo.err->format_message((const j_common_ptr_ALPHA)&cinfo, buffer);
    SkDebugf("libjpeg error %d <%s> from %s [%d %d]\n",
             cinfo.err->msg_code, buffer, caller, width, height);
}

static bool return_false(const jpeg_decompress_struct_ALPHA& cinfo,
                         const SkBitmap& bm, const char caller[]) {
    print_jpeg_decoder_errors(cinfo, bm.width(), bm.height(), caller);
    return false;
}

// Convert a scanline of CMYK samples to RGBX in place. Note that this
// method moves the "scanline" pointer in its processing
static void convert_CMYK_to_RGB(uint8_t* scanline, unsigned int width) {
    // At this point we've received CMYK pixels from libjpeg. We
    // perform a crude conversion to RGB (based on the formulae
    // from easyrgb.com):
    //  CMYK -> CMY
    //    C = ( C * (1 - K) + K )      // for each CMY component
    //  CMY -> RGB
    //    R = ( 1 - C ) * 255          // for each RGB component
    // Unfortunately we are seeing inverted CMYK so all the original terms
    // are 1-. This yields:
    //  CMYK -> CMY
    //    C = ( (1-C) * (1 - (1-K) + (1-K) ) -> C = 1 - C*K
    // The conversion from CMY->RGB remains the same
    for (unsigned int x = 0; x < width; ++x, scanline += 4) {
        scanline[0] = SkMulDiv255Round(scanline[0], scanline[3]);
        scanline[1] = SkMulDiv255Round(scanline[1], scanline[3]);
        scanline[2] = SkMulDiv255Round(scanline[2], scanline[3]);
        scanline[3] = 255;
    }
}

/**
 *  Common code for setting the error manager.
 */
static void set_error_mgr(jpeg_decompress_struct_ALPHA* cinfo, skjpeg_error_mgr_MTK* errorManager) {
    SkASSERT(cinfo != nullptr);
    SkASSERT(errorManager != nullptr);
    cinfo->err = jpeg_std_error_ALPHA(errorManager);
    errorManager->error_exit = skjpeg_err_exit_MTK;
}

/**
 *  Common code for turning off upsampling and smoothing. Turning these
 *  off helps performance without showing noticable differences in the
 *  resulting bitmap.
 */
static void turn_off_visual_optimizations(jpeg_decompress_struct_ALPHA* cinfo) {
    SkASSERT(cinfo != nullptr);
    /* this gives about 30% performance improvement. In theory it may
       reduce the visual quality, in practice I'm not seeing a difference
     */
    cinfo->do_fancy_upsampling = 0;

    /* this gives another few percents */
    cinfo->do_block_smoothing = 0;
}

SkColorType SkJPEGImageDecoder::getBitmapColorType(jpeg_decompress_struct_ALPHA* cinfo) {
    SkASSERT(cinfo != nullptr);

    SrcDepth srcDepth = k32Bit_SrcDepth;
    if (JCS_GRAYSCALE_ALPHA == cinfo->jpeg_color_space) {
        srcDepth = k8BitGray_SrcDepth;
    }

    SkColorType colorType = this->getPrefColorType(srcDepth, /*hasAlpha*/ false);
    switch (colorType) {
        case kAlpha_8_SkColorType:
            // Only respect A8 colortype if the original is grayscale,
            // in which case we will treat the grayscale as alpha
            // values.
            if (cinfo->jpeg_color_space != JCS_GRAYSCALE_ALPHA) {
                colorType = kN32_SkColorType;
            }
            break;
        case kN32_SkColorType:
            // Fall through.
        case kARGB_4444_SkColorType:
            // Fall through.
        case kRGB_565_SkColorType:
            // These are acceptable destination colortypes.
            break;
        default:
            // Force all other colortypes to 8888.
            colorType = kN32_SkColorType;
            break;
    }

    switch (cinfo->jpeg_color_space) {
        case JCS_CMYK_ALPHA:
            // Fall through.
        case JCS_YCCK_ALPHA:
            // libjpeg cannot convert from CMYK or YCCK to RGB - here we set up
            // so libjpeg will give us CMYK samples back and we will later
            // manually convert them to RGB
            cinfo->out_color_space = JCS_CMYK_ALPHA;
            break;
        case JCS_GRAYSCALE_ALPHA:
            if (kAlpha_8_SkColorType == colorType) {
                cinfo->out_color_space = JCS_GRAYSCALE_ALPHA;
                break;
            }
            // The data is JCS_GRAYSCALE, but the caller wants some sort of RGB
            // colortype. Fall through to set to the default.
        default:
            cinfo->out_color_space = JCS_RGB_ALPHA;
            break;
    }
    return colorType;
}

/**
 *  Based on the colortype and dither mode, adjust out_color_space and
 *  dither_mode of cinfo. Only does work in ANDROID_RGB
 */
static void adjust_out_color_space_and_dither(jpeg_decompress_struct_ALPHA* cinfo,
                                              SkColorType colorType,
                                              const SkImageDecoder& decoder) {
    SkASSERT(cinfo != nullptr);
    cinfo->dither_mode = JDITHER_NONE_ALPHA;
    if (JCS_CMYK_ALPHA == cinfo->out_color_space) {
        return;
    }
    switch (colorType) {
        case kN32_SkColorType:
            cinfo->out_color_space = JCS_EXT_RGBA_ALPHA;
            break;
        case kRGB_565_SkColorType:
            cinfo->out_color_space = JCS_RGB565_ALPHA;
            if (decoder.getDitherImage()) {
                cinfo->dither_mode = JDITHER_ORDERED_ALPHA;
            }
            break;
        default:
            break;
    }
}

/**
 *  Get the config and bytes per pixel of the source data. Return
 *  whether the data is supported.
 */
static bool get_src_config(const jpeg_decompress_struct_ALPHA& cinfo,
                           SkScaledBitmapSampler::SrcConfig* sc,
                           int* srcBytesPerPixel) {
    SkASSERT(sc != nullptr && srcBytesPerPixel != nullptr);
    if (JCS_CMYK_ALPHA == cinfo.out_color_space) {
        // In this case we will manually convert the CMYK values to RGB
        *sc = SkScaledBitmapSampler::kRGBX;
        // The CMYK work-around relies on 4 components per pixel here
        *srcBytesPerPixel = 4;
    } else if (3 == cinfo.out_color_components && JCS_RGB_ALPHA == cinfo.out_color_space) {
        *sc = SkScaledBitmapSampler::kRGB;
        *srcBytesPerPixel = 3;
    } else if (JCS_EXT_RGBA_ALPHA == cinfo.out_color_space) {
        *sc = SkScaledBitmapSampler::kRGBX;
        *srcBytesPerPixel = 4;
    } else if (JCS_RGB565_ALPHA == cinfo.out_color_space) {
        *sc = SkScaledBitmapSampler::kRGB_565;
        *srcBytesPerPixel = 2;
    } else if (1 == cinfo.out_color_components &&
               JCS_GRAYSCALE_ALPHA == cinfo.out_color_space) {
        *sc = SkScaledBitmapSampler::kGray;
        *srcBytesPerPixel = 1;
    } else {
        return false;
    }
    return true;
}

static bool getEncodedColor(jpeg_decompress_struct_ALPHA* cinfo, SkEncodedInfo::Color* outColor) {
    switch (cinfo->jpeg_color_space) {
        case JCS_GRAYSCALE_ALPHA:
            *outColor = SkEncodedInfo::kGray_Color;
            return true;
        case JCS_YCbCr_ALPHA:
            *outColor = SkEncodedInfo::kYUV_Color;
            return true;
        case JCS_RGB_ALPHA:
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

bool SkJPEGImageDecoder::onBuildTileIndex(SkStreamRewindable* stream, int *width, int *height) {

    fFirstTileDone = false;
    fUseHWResizer = false;

    size_t length = stream->getLength();
    if (length <= 0 ) {
        return false;
    }

    SkAutoTMalloc<uint8_t> allocMemory(length);

    SkStreamRewindable* dupStream = stream->duplicate().release();

    dupStream->read(allocMemory.get(), length) ;
#ifdef MTK_JPEG_HW_REGION_RESIZER
    /* parsing and get ISOSpeedRatings */
    fISOSpeedRatings = getISOSpeedRatings(allocMemory.get(), length);
    SkDebugf("onBuildTileIndex parsed ISOSpeedRatings %d L:%d!! \n" ,fISOSpeedRatings, __LINE__);
#endif

    SkMemoryStream* mtkPxyStream = new SkMemoryStream(allocMemory ,  length,  true);

    SkAutoTDelete<SkJPEGImageIndex> imageIndex = new SkJPEGImageIndex(dupStream, this);

    if(mtkPxyStream){
        imageIndex->mtkStream = mtkPxyStream ;
    }

    jpeg_decompress_struct_ALPHA* cinfo = imageIndex->cinfo();

    skjpeg_error_mgr_MTK sk_err;
    set_error_mgr(imageIndex->cinfo(), &sk_err);

    // All objects need to be instantiated before this setjmp call so that
    // they will be cleaned up properly if an error occurs.
    skjpeg_error_mgr_MTK::AutoPushJmpBuf jmp(&sk_err);
    if (setjmp(jmp)) {
        return false;
    }

    // create the cinfo used to create/build the huffmanIndex
    if (!imageIndex->initializeInfoAndReadHeader()) {
        return false;
    }

    if (!imageIndex->buildHuffmanIndex()) {
        return false;
    }

    // destroy the cinfo used to create/build the huffman index
    imageIndex->destroyInfo();
    imageIndex->resetStream(); //add for reset stream offset

    // Init decoder to image decode mode
    if (!imageIndex->initializeInfoAndReadHeader()) {
        return false;
    }

    // FIXME: This sets cinfo->out_color_space, which we may change later
    // based on the config in onDecodeSubset. This should be fine, since
    // jpeg_init_read_tile_scanline will check out_color_space again after
    // that change (when it calls jinit_color_deconverter).
    (void) this->getBitmapColorType(cinfo);

    // Create image info object and the codec
    SkEncodedInfo::Color color;
    if (!getEncodedColor(cinfo, &color)) {
        return false;
    }
    //fEncodedInfo = SkEncodedInfo::Make(color, SkEncodedInfo::kOpaque_Alpha, 8);
    //SkDebugf("onBuildTileIndex SkEncodedInfo::Make color %d\n", color);

    turn_off_visual_optimizations(cinfo);

    // instead of jpeg_start_decompress() we start a tiled decompress
    if (!imageIndex->startTileDecompress()) {
        return false;
    }

    SkASSERT(1 == cinfo->scale_num);
    fImageWidth = cinfo->output_width;
    fImageHeight = cinfo->output_height;

    if (width) {
        *width = fImageWidth;
    }
    if (height) {
        *height = fImageHeight;
    }

    auto profile = read_color_profile(cinfo);

    delete fImageIndex;
    fImageIndex = imageIndex.detach();

    if ((cinfo->comps_in_scan < cinfo->num_components )&& !cinfo->progressive_mode){
      SkDebugf("buildTileIndex fail, region_decoder unsupported format : prog %d, comp %d, scan_comp %d!!\n"
      , cinfo->progressive_mode, cinfo->num_components, cinfo->comps_in_scan );
      return false;
    }

    if(profile)
    {
        SkDebugf("icc profile color space found");
        return false; // mtk region decode cannot handle icc profile
    }

    return true;
}

#ifdef MTK_JPEG_HW_REGION_RESIZER

extern void* allocateIONBuffer(int ionClientHnd, ion_user_handle_t *ionAllocHnd, int *bufferFD, size_t size);
extern void freeIONBuffer(int ionClientHnd, ion_user_handle_t ionAllocHnd, void* bufferAddr, int bufferFD, size_t size);
extern int IONVaToMva(int ionClientHnd, unsigned long va, unsigned int size, unsigned int *mva, int *handleToBeFree);

bool MDPResizer(void* src, int ionClientHnd, int srcFD, int width, int height, SkScaledBitmapSampler::SrcConfig sc, SkBitmap* bm, SkColorType colorType, int tdsp, void* pPPParam, unsigned int ISOSpeed)
{
    if((nullptr == bm))
    {
        ALOGW("MDPResizer : null bitmap");
        return false;
    }
    if(nullptr == bm->getPixels())
    {
        ALOGW("MDPResizer : null pixels");
        return false;
    }
    if((colorType == kRGBA_8888_SkColorType) ||
       (colorType == kRGB_565_SkColorType))
    {
        sp<IMms> IMms_service = IMms::tryGetService();
        if (IMms_service == nullptr)
        {
            SkDebugf("cannot find IMms_service!");
            return false;
        }
        MDPParamFD mdpParam;
        native_handle_t* srcHdl;
        native_handle_t* dstHdl;
        memset(&mdpParam, 0, sizeof(MDPParamFD));
        unsigned int src_size = 0;
        unsigned int src_pByte = 4;
        mdpParam.src_planeNumber = 1;

        if(colorType == kRGBA_8888_SkColorType)
        {
            mdpParam.dst_format = eRGBX8888; //eABGR8888;    //bltParam.dstFormat = MHAL_FORMAT_ABGR_8888;
        }
        else
        {
            mdpParam.dst_format = eRGB565;    //bltParam.dstFormat = MHAL_FORMAT_RGB_565;
        }
        switch(sc)
        {
            case SkScaledBitmapSampler::kRGB:
                mdpParam.src_format = eRGB888;         //bltParam.srcFormat = MHAL_FORMAT_BGR_888;
                src_pByte = 3;
                break;
            case SkScaledBitmapSampler::kRGBX:
                mdpParam.src_format = eRGBX8888;//eABGR8888;         //bltParam.srcFormat = MHAL_FORMAT_ABGR_8888;
                src_pByte = 4;
                break;
            case SkScaledBitmapSampler::kRGB_565:
                mdpParam.src_format = eRGB565;         //bltParam.srcFormat = MHAL_FORMAT_RGB_565;
                src_pByte = 2;
                break;
            case SkScaledBitmapSampler::kGray:
                mdpParam.src_format = eGREY;           //bltParam.srcFormat = MHAL_FORMAT_Y800;
                src_pByte = 1;
                break;
            default :
                ALOGW("MDPResizer : invalid src format %d", sc);
                return false;
            break;
        }

        src_size = width * height * src_pByte ;
        mdpParam.src_sizeList[0] = src_size;
        SkDebugf("MDPResizer: wh (%d %d)->(%d %d), fmt %d->%d, size %d->%zu, regionPQ %d!!\n", width, height, bm->width(), bm->height()
        ,sc, colorType, src_size, bm->rowBytes() * bm->height(), tdsp);

        {
            mdpParam.pq_param.enable = (tdsp == 0)? false:true;
            mdpParam.pq_param.scenario = MMS_MEDIA_TYPE_ENUM::MMS_MEDIA_PICTURE;
            mdpParam.pq_param.iso = ISOSpeed;

            if (pPPParam)
            {
                SkDebugf("MDPResizer: enable imgDc pParam %p", pPPParam);
            }
        }

        if (srcFD >= 0)
        {
            srcHdl = native_handle_create(1, 0);
            srcHdl->data[0] = srcFD;
            mdpParam.inputHandle = srcHdl;
        }
        else
            return false;

        mdpParam.src_rect.x = 0;
        mdpParam.src_rect.y = 0;
        mdpParam.src_rect.w = width;
        mdpParam.src_rect.h = height;
        mdpParam.src_width = width;
        mdpParam.src_height = height;
        mdpParam.src_yPitch = width * src_pByte;
        mdpParam.src_profile = MMS_PROFILE_ENUM::MMS_PROFILE_JPEG;

        // set dst buffer
        ion_user_handle_t ionAllocHnd = 0;
        int dstFD = -1;
        void* dstBuffer = nullptr;
        unsigned int dst_size = 0;
        unsigned int skBitmapSize_MTK = bm->height() * bm->rowBytes();
        mdpParam.dst_planeNumber = 1;

        dst_size = skBitmapSize_MTK;
        mdpParam.dst_sizeList[0] = dst_size;
        dstBuffer = allocateIONBuffer(ionClientHnd, &ionAllocHnd, &dstFD, dst_size);

        dstHdl = native_handle_create(1, 0);
        dstHdl->data[0] = dstFD;
        mdpParam.outputHandle = dstHdl;
        SkDebugf("MDPResizer allocateIONBuffer src:(%d), dst:(%d, %d, %d, %d, %p)",
                    srcFD, ionClientHnd, ionAllocHnd, dstFD, dst_size, dstBuffer);

        mdpParam.dst_rect.x = 0;
        mdpParam.dst_rect.y = 0;
        mdpParam.dst_rect.w = bm->width();
        mdpParam.dst_rect.h = bm->height();
        mdpParam.dst_width = bm->width();
        mdpParam.dst_height = bm->height();
        mdpParam.dst_yPitch = bm->rowBytes();
        mdpParam.dst_profile = MMS_PROFILE_ENUM::MMS_PROFILE_JPEG;

        auto ret = IMms_service->BlitStreamFD(mdpParam);
        if (!ret.isOk())
        {
            SkDebugf("IMms_service->BlitStreamFD failed");
            if (dstBuffer != nullptr)
            {
                freeIONBuffer(ionClientHnd, ionAllocHnd, dstBuffer, dstFD, skBitmapSize_MTK);
            }
            native_handle_delete(srcHdl);
            native_handle_delete(dstHdl);
            return false;
        }

        // if dstBuffer is not nullptr, need to copy pixels to bitmap and free ION buffer
        if (dstBuffer != nullptr)
        {
            memcpy(bm->getPixels(), dstBuffer, skBitmapSize_MTK);
            freeIONBuffer(ionClientHnd, ionAllocHnd, dstBuffer, dstFD, skBitmapSize_MTK);
        }

        native_handle_delete(srcHdl);
        native_handle_delete(dstHdl);

        return true;
    }
    return false;

}
#endif


bool SkJPEGImageDecoder::onDecodeSubset(SkBitmap* bm, SkBRDAllocator* allocator, const SkIRect& region, int isampleSize, sk_sp<SkColorSpace> dstColorSpace) {

#ifdef MTK_JPEG_HW_REGION_RESIZER
    unsigned int enTdshp = (this->getPostProcFlag()? 1 : 0);

    if (fFirstTileDone == false)
    {
        long u4PQOpt;
        char value[PROPERTY_VALUE_MAX];

        property_get("jpegRegion.forceEnable.PQ", value, "-1");
        u4PQOpt = atol(value);
        if(-1 == u4PQOpt)
            enTdshp = (this->getPostProcFlag()? 1 : 0);
        else if(0 == u4PQOpt)
            enTdshp = 0x0;
        else
            enTdshp = 0x1;
        if (!enTdshp)
        {
            fFirstTileDone = true;
            fUseHWResizer = false;
        }
    }
#endif

    JpgLibAutoClean auto_clean_cinfo ;
    JpgStreamAutoClean auto_clean_stream;
    jpeg_decompress_struct_ALPHA *cinfo = nullptr;
    SkStream *stream ;
    skjpeg_source_mgr_MTK *sk_stream = nullptr;
    SkColorType colorType;

    if(fImageIndex->mtkStream){
        stream = fImageIndex->mtkStream->duplicate().release();

        sk_stream = new skjpeg_source_mgr_MTK(stream, this);

        cinfo = (jpeg_decompress_struct_ALPHA *)malloc(sizeof(struct jpeg_decompress_struct_ALPHA));
        if(cinfo != nullptr)
            memset(cinfo, 0, sizeof(struct jpeg_decompress_struct_ALPHA));
        else
        {
            delete sk_stream;
            return false;
        }
        auto_clean_cinfo.set(cinfo);

        skjpeg_error_mgr_MTK sk_err;
        set_error_mgr(cinfo, &sk_err);

        // All objects need to be instantiated before this setjmp call so that
        // they will be cleaned up properly if an error occurs.
        skjpeg_error_mgr_MTK::AutoPushJmpBuf jmp(&sk_err);
        if (setjmp(jmp)) {
           SkDebugf("MTR_JPEG: setjmp L:%d!!\n ", __LINE__ );
           delete sk_stream;
           return false;
        }

        // Init decoder to image decode mode
        {
            initialize_info(cinfo, sk_stream);
            const bool success = (JPEG_HEADER_OK == jpeg_read_header_ALPHA(cinfo, true));
            if(!success){
               SkDebugf("MTR_JPEG: initializeInfoAndReadHeader error L:%d!!\n ", __LINE__ );
               return false;
            }
        }

        // FIXME: This sets cinfo->out_color_space, which we may change later
        // based on the config in onDecodeSubset. This should be fine, since
        // jpeg_init_read_tile_scanline will check out_color_space again after
        // that change (when it calls jinit_color_deconverter).
        colorType = this->getBitmapColorType(cinfo);
        adjust_out_color_space_and_dither(cinfo, colorType, *this);

        turn_off_visual_optimizations(cinfo);

        // instead of jpeg_start_decompress() we start a tiled decompress
        if (!jpeg_start_tile_decompress_ALPHA(cinfo)) {
           SkDebugf("MTR_JPEG: startTileDecompress error L:%d!!\n ", __LINE__ );
           return false;
        }

        auto_clean_stream.set(stream);
    }
    else return false;

    SkIRect rect = SkIRect::MakeWH(fImageWidth, fImageHeight);
    if (!rect.intersect(region)) {
        // If the requested region is entirely outside the image return false
        return false;
    }

    SkAutoTMalloc<uint8_t> srcStorage;
    skjpeg_error_mgr_MTK errorManager;
    if(cinfo != nullptr)
        set_error_mgr(cinfo, &errorManager);

    skjpeg_error_mgr_MTK::AutoPushJmpBuf jmp(&errorManager);
    if (setjmp(jmp)) {
        return false;
    }

    if(isampleSize == 0x0){
        isampleSize = this->getSampleSize();
        SkDebugf("JPEG: debug isampleSize = %d , L:%d!!\n",isampleSize ,__LINE__);
    }
    int requestedSampleSize = (isampleSize == 3)? 2: isampleSize;

#ifdef MTK_JPEG_HW_REGION_RESIZER
    if(!fFirstTileDone || fUseHWResizer)
    {
        SkIRect rectHWResz;
        // create new region which is padding 40 pixels for each boundary
        rectHWResz.set((region.left() >= 40)? region.left() - 40: region.left(),
                       (region.top() >= 40)? region.top() - 40: region.top(),
                       (region.right() + 40 <= fImageWidth)? region.right() + 40: fImageWidth,
                       (region.bottom() + 40 <= fImageHeight)? region.bottom() + 40: fImageHeight);
        // set rect to enlarged size to fit HW resizer constraint
        rect.set(0,0,fImageWidth, fImageHeight);
        if (!rect.intersect(rectHWResz))
        {
            return false;
        }
    }
#endif
    cinfo->scale_denom = requestedSampleSize;

    int startX = rect.fLeft;
    int startY = rect.fTop;
    int width = rect.width();
    int height = rect.height();

    jpeg_init_read_tile_scanline_ALPHA(cinfo, fImageIndex->huffmanIndex(),
                                 &startX, &startY, &width, &height);
    int skiaSampleSize = recompute_sampleSize(requestedSampleSize, *cinfo);
    int actualSampleSize = skiaSampleSize * (DCTSIZE_ALPHA / cinfo->min_DCT_scaled_size);

    SkScaledBitmapSampler sampler(width, height, skiaSampleSize);

    SkBitmap bitmap;
    // Assume an A8 bitmap is not opaque to avoid the check of each
    // individual pixel. It is very unlikely to be opaque, since
    // an opaque A8 bitmap would not be very interesting.
    // Otherwise, a jpeg image is opaque.
    bitmap.setInfo(SkImageInfo::Make(sampler.scaledWidth(), sampler.scaledHeight(), colorType,
                                     kAlpha_8_SkColorType == colorType ?
                                         kPremul_SkAlphaType : kOpaque_SkAlphaType, dstColorSpace));

    if (!bitmap.tryAllocPixels()) {
        return return_false(*cinfo, bitmap, "allocPixels");
    }

    // check for supported formats
    SkScaledBitmapSampler::SrcConfig sc;
    int srcBytesPerPixel;

    if (!get_src_config(*cinfo, &sc, &srcBytesPerPixel)) {
        return return_false(*cinfo, *bm, "jpeg colorspace");
    }

    if (!sampler.begin(&bitmap, sc, *this)) {
        return return_false(*cinfo, bitmap, "sampler.begin");
    }

    uint8_t* srcRow = (uint8_t*)srcStorage.reset(width * srcBytesPerPixel);

#ifdef MTK_JPEG_HW_REGION_RESIZER
if(!fFirstTileDone || fUseHWResizer)
{

    #ifdef MTK_SKIA_USE_ION
    SkIonMalloc srcAllocator(fIonClientHnd);
    uint8_t* hwBuffer = (uint8_t*)srcAllocator.reset(width * height * srcBytesPerPixel + 4);
    #else
    SkAutoTMalloc<uint8_t> hwStorage;
    uint8_t* hwBuffer = (uint8_t*)srcStorage.reset(width * height * srcBytesPerPixel + 4);
    #endif

    hwBuffer[width * height * srcBytesPerPixel + 4 - 1] = 0xF0;
    hwBuffer[width * height * srcBytesPerPixel + 4 - 2] = 0xF0;
    hwBuffer[width * height * srcBytesPerPixel + 4 - 3] = 0xF0;
    hwBuffer[width * height * srcBytesPerPixel + 4 - 4] = 0xF0;
    int row_total_count = 0;
    int bpr = width * srcBytesPerPixel;
    JSAMPLE_ALPHA* rowptr = (JSAMPLE_ALPHA*)hwBuffer;

    while (row_total_count < height) {
        int row_count = jpeg_read_tile_scanline_ALPHA(cinfo, fImageIndex->huffmanIndex(), &rowptr);
        // if row_count == 0, then we didn't get a scanline, so abort.
        // if we supported partial images, we might return true in this case
        if (0 == row_count) {
            return return_false(*cinfo, bitmap, "read_scanlines");
        }

        if (JCS_CMYK_ALPHA == cinfo->out_color_space) {
            convert_CMYK_to_RGB(rowptr, width);
        }
        row_total_count += row_count;
        rowptr += bpr;
    }

    int try_times = 5;
    bool result = false;
    do
    {
        #ifdef MTK_SKIA_USE_ION
        result = MDPResizer(hwBuffer, fIonClientHnd, srcAllocator.getFD(), width, height, sc, &bitmap, colorType, enTdshp, nullptr, fISOSpeedRatings);
        #else
        result = MDPResizer(hwBuffer, 0, -1, width, height, sc, &bitmap, colorType, enTdshp, nullptr, fISOSpeedRatings);
        #endif

        if(!result && ++try_times < 5)
        {
            SkDebugf("Hardware resize fail, sleep 100 us and then try again ");
            usleep(100*1000);
        }
    }while(!result && try_times < 5);

    if(!result)
    {
        {
            SkAutoMutexAcquire ac(gAutoTileResizeMutex);

            fFirstTileDone = true;
        }
        ALOGW("Hardware resize fail, use sw sampler");
        row_total_count = 0;
        rowptr = (JSAMPLE_ALPHA*)hwBuffer;
        rowptr += (bpr * sampler.srcY0());
        row_total_count += sampler.srcY0();
        for (int y = 0;; y++) {
            sampler.next(rowptr);
            if (bitmap.height() - 1 == y) {
                // we're done
                SkDebugf("total row count %d\n", row_total_count);
                break;
            }
            rowptr += bpr;
            row_total_count ++;

            rowptr += (bpr * (sampler.srcDY() - 1));
            row_total_count += (sampler.srcDY() - 1);
        }
    }
    else
    {
        {
            SkAutoMutexAcquire ac(gAutoTileResizeMutex);
            fUseHWResizer = true;
            fFirstTileDone = true;
        }
    }
} else {
#endif
    //  Possibly skip initial rows [sampler.srcY0]
    if (!skip_src_rows_tile(cinfo, fImageIndex->huffmanIndex(), srcRow, sampler.srcY0())) {
        return return_false(*cinfo, bitmap, "skip rows");
    }

    // now loop through scanlines until y == bitmap->height() - 1
    for (int y = 0;; y++) {
        JSAMPLE_ALPHA* rowptr = (JSAMPLE_ALPHA*)srcRow;
        int row_count = jpeg_read_tile_scanline_ALPHA(cinfo, fImageIndex->huffmanIndex(), &rowptr);
        // if row_count == 0, then we didn't get a scanline, so abort.
        // onDecodeSubset() relies on onBuildTileIndex(), which
        // needs a complete image to succeed.
        if (0 == row_count) {
            return return_false(*cinfo, bitmap, "read_scanlines");
        }

        if (JCS_CMYK_ALPHA == cinfo->out_color_space) {
            convert_CMYK_to_RGB(srcRow, width);
        }

        sampler.next(srcRow);
        if (bitmap.height() - 1 == y) {
            // we're done
            break;
        }

        if (!skip_src_rows_tile(cinfo, fImageIndex->huffmanIndex(), srcRow,
                                sampler.srcDY() - 1)) {
            return return_false(*cinfo, bitmap, "skip rows");
        }
    }

#ifdef MTK_JPEG_HW_REGION_RESIZER
}
#endif

    cropBitmap(bm, &bitmap, actualSampleSize, region.x(), region.y(),
               region.width(), region.height(), startX, startY, allocator);
    if (bm->pixelRef() == nullptr) {
        ALOGW("SkiaJPEG::cropBitmap allocPixelRef FAIL L:%d !!!!!!\n", __LINE__);
        return return_false(*cinfo, bitmap, "cropBitmap Allocate Pixel Fail!! ");
    }
    SkAutoTDelete<skjpeg_source_mgr_MTK> adjpg(sk_stream);
    jpeg_finish_decompress_ALPHA(cinfo);
    jpeg_destroy_decompress_ALPHA(cinfo);
    return true;
}

static bool is_jpeg(SkStreamRewindable* stream) {
    static const unsigned char gHeader[] = { 0xFF, 0xD8, 0xFF };
    static const size_t HEADER_SIZE = sizeof(gHeader);

    char buffer[HEADER_SIZE];
    size_t len = stream->read(buffer, HEADER_SIZE);

    if (len != HEADER_SIZE) {
        return false;   // can't read enough
    }
    if (memcmp(buffer, gHeader, HEADER_SIZE)) {
        return false;
    }
    return true;
}


static SkImageDecoder* sk_libjpeg_dfactory(SkStreamRewindable* stream) {
    if (is_jpeg(stream)) {
        return new SkJPEGImageDecoder;
    }
    return nullptr;
}

static SkImageDecoder::Format get_format_jpeg(SkStreamRewindable* stream) {
    if (is_jpeg(stream)) {
        return SkImageDecoder::kJPEG_Format;
    }
    return SkImageDecoder::kUnknown_Format;
}

static SkImageDecoder_DecodeReg gDReg(sk_libjpeg_dfactory);
static SkImageDecoder_FormatReg gFormatReg(get_format_jpeg);

