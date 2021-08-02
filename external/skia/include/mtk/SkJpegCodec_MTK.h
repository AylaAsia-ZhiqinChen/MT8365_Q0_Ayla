/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkJpegCodec_MTK_DEFINED
#define SkJpegCodec_MTK_DEFINED

#include "SkCodec.h"
#include "SkColorSpace.h"
#include "SkImageInfo.h"
#include "SkSwizzler.h"
#include "SkStream.h"
#include "SkTemplates.h"

#ifdef MTK_JPEG_HW_REGION_RESIZER
#include <sys/mman.h>
#include <linux/ion.h>
#include <ion/ion.h>


#define ION_HEAP_MULTIMEDIA_MASK (1 << 10)

class SkIonMalloc
{
public:
    SkIonMalloc(int ionClientHnd): fIonAllocHnd(-1), fAddr(NULL), fShareFD(-1), fSize(0), fStreamSize(0), fColorType(kRGBA_8888_SkColorType)
    {
        if (ionClientHnd < 0)
        {
            SkDebugf("invalid ionClientHnd(%d)\n", ionClientHnd);
            fIonClientHnd = -1;
        }
        else
            fIonClientHnd = ionClientHnd;
    }
    ~SkIonMalloc()
    {
        free();
    }
    void* reset(size_t size)
    {
        int ret;
        if (fIonClientHnd >= 0)
        {
            if(fAddr != NULL)
            {
                free();
            }
            fSize = size;
            ret = ion_alloc(fIonClientHnd, size, 0, ION_HEAP_MULTIMEDIA_MASK, ION_FLAG_CACHED | ION_FLAG_CACHED_NEEDS_SYNC, &fIonAllocHnd);
            if (ret)
            {
                SkDebugf("SkIonMalloc::ion_alloc failed (%d, %zu, %d)\n", fIonClientHnd, size, fIonAllocHnd);
                return NULL;
            }
            ret = ion_map(fIonClientHnd, fIonAllocHnd, size, PROT_READ | PROT_WRITE, MAP_SHARED, 0, &fAddr, &fShareFD);
            if (ret)
            {
                SkDebugf("SkIonMalloc::ion_map failed (%d, %zu, %d)\n", fIonClientHnd, size, fShareFD);
                if (ion_free(fIonClientHnd, fIonAllocHnd))
                {
                    SkDebugf("SkIonMalloc::ion_free failed (%d %d)\n", fIonClientHnd, fIonAllocHnd);
                }
                return NULL;
            }
            return fAddr;
        }
        else
            return 0;
    }
    void free()
    {
        if (fIonClientHnd >= 0)
        {
            if(fAddr != NULL)
            {
                int ret = munmap(fAddr, fSize);
                if (ret < 0)
                    SkDebugf("SkIonMalloc::munmap failed (%d, %p, %zu)\n", fIonClientHnd, fAddr, fSize);
                else
                    fAddr = NULL;
            }
            if (fShareFD != -1)
            {
                if (close(fShareFD))
                {
                    SkCodecPrintf("SkIonMallo::close failed (%d, %d)\n", fIonClientHnd, fShareFD);
                }
            }
            if (fIonAllocHnd != -1)
            {
                if (ion_free(fIonClientHnd, fIonAllocHnd))
                {
                    SkDebugf("SkIonMalloc::ion_free failed (%d %d)\n", fIonClientHnd, fIonAllocHnd);
                }
            }
        }
    }
    void setStreamSize(int streamSize) { fStreamSize = streamSize; }
    void setColor(SkColorType color) { fColorType = color; }
    void* getAddr() { return fAddr; }
    int getFD() { return fShareFD; }
    int getSize() { return fSize; }
    int getStreamSize() { return fStreamSize; }
    SkColorType getColor() { return fColorType; }
private:
    ion_user_handle_t fIonAllocHnd;
    int               fIonClientHnd;
    unsigned char*    fAddr;
    int               fShareFD;
    size_t            fSize;
    size_t            fStreamSize;
    SkColorType       fColorType;
};
#endif

class JpegDecoderMgr_MTK;

/*
 *
 * This class implements the decoding for jpeg images
 *
 */
class SkJpegCodec : public SkCodec {
public:
    static bool IsJpeg(const void*, size_t);

    /*
     * Assumes IsJpeg was called and returned true
     * Takes ownership of the stream
     */
    static std::unique_ptr<SkCodec> MakeFromStream(std::unique_ptr<SkStream>, Result*);

protected:

    /*
     * Recommend a set of destination dimensions given a requested scale
     */
    SkISize onGetScaledDimensions(float desiredScale) const override;

    /*
     * Initiates the jpeg decode
     */
    Result onGetPixels(const SkImageInfo& dstInfo, void* dst, size_t dstRowBytes, const Options&,
            int*) override;

    bool onQueryYUV8(SkYUVASizeInfo* sizeInfo, SkYUVColorSpace* colorSpace) const override;

    Result onGetYUV8Planes(const SkYUVASizeInfo& sizeInfo,
                           void* planes[SkYUVASizeInfo::kMaxCount]) override;

    SkEncodedImageFormat onGetEncodedFormat() const override {
        return SkEncodedImageFormat::kJPEG;
    }

    bool onRewind() override;

    bool onDimensionsSupported(const SkISize&) override;

    bool conversionSupported(const SkImageInfo&, bool, bool) override;

private:
    /*
     * Allows SkRawCodec to communicate the color profile from the exif data.
     */
    static std::unique_ptr<SkCodec> MakeFromStream(std::unique_ptr<SkStream>, Result*,
            std::unique_ptr<SkEncodedInfo::ICCProfile> defaultColorProfile);

    /*
     * Read enough of the stream to initialize the SkJpegCodec.
     * Returns a bool representing success or failure.
     *
     * @param codecOut
     * If this returns true, and codecOut was not nullptr,
     * codecOut will be set to a new SkJpegCodec.
     *
     * @param decoderMgrOut
     * If this returns true, and codecOut was nullptr,
     * decoderMgrOut must be non-nullptr and decoderMgrOut will be set to a new
     * JpegDecoderMgr pointer.
     *
     * @param stream
     * Deleted on failure.
     * codecOut will take ownership of it in the case where we created a codec.
     * Ownership is unchanged when we set decoderMgrOut.
     *
     * @param defaultColorProfile
     * If the jpeg does not have an embedded color profile, the image data should
     * be tagged with this color profile.
     */
    static Result ReadHeader(SkStream* stream, SkCodec** codecOut,
            JpegDecoderMgr_MTK** decoderMgrOut,
            std::unique_ptr<SkEncodedInfo::ICCProfile> defaultColorProfile);

    /*
     * Creates an instance of the decoder
     * Called only by NewFromStream
     *
     * @param info contains properties of the encoded data
     * @param stream the encoded image data
     * @param decoderMgr holds decompress struct, src manager, and error manager
     *                   takes ownership
     */
    SkJpegCodec(SkEncodedInfo&& info, std::unique_ptr<SkStream> stream,
            JpegDecoderMgr_MTK* decoderMgr, SkEncodedOrigin origin);

#ifdef MTK_JPEG_HW_REGION_RESIZER
    ~SkJpegCodec() override;
#endif
    void initializeSwizzler(const SkImageInfo& dstInfo, const Options& options,
                            bool needsCMYKToRGB);
    void allocateStorage(const SkImageInfo& dstInfo);
    int readRows(const SkImageInfo& dstInfo, void* dst, size_t rowBytes, int count, const Options&);

#ifdef MTK_JPEG_HW_REGION_RESIZER
    int readRows_MTK(const SkImageInfo& dstInfo, void* dst, size_t rowBytes, int count, const Options&);
#endif

    /*
     * Scanline decoding.
     */
    SkSampler* getSampler(bool createIfNecessary) override;
    Result onStartScanlineDecode(const SkImageInfo& dstInfo,
            const Options& options) override;
    int onGetScanlines(void* dst, int count, size_t rowBytes) override;
    bool onSkipScanlines(int count) override;

    std::unique_ptr<JpegDecoderMgr_MTK>    fDecoderMgr;

    // We will save the state of the decompress struct after reading the header.
    // This allows us to safely call onGetScaledDimensions() at any time.
    const int                          fReadyState;


    SkAutoTMalloc<uint8_t>             fStorage;
    uint8_t*                           fSwizzleSrcRow;
    uint32_t*                          fColorXformSrcRow;

    // libjpeg-turbo provides some subsetting.  In the case that libjpeg-turbo
    // cannot take the exact the subset that we need, we will use the swizzler
    // to further subset the output from libjpeg-turbo.
    SkIRect                            fSwizzlerSubset;

    std::unique_ptr<SkSwizzler>        fSwizzler;

    friend class SkRawCodec;

    typedef SkCodec INHERITED;
#ifdef MTK_JPEG_HW_REGION_RESIZER
    int                    fIonClientHnd;
    int                    fISOSpeedRatings;
    SkIonMalloc*           fIonBufferStorage;
    bool                   fIsSampleDecode;
    unsigned int           fSampleDecodeY;

    bool                   fFirstTileDone;
    bool                   fUseHWResizer;
    bool                   fEnTdshp;
    unsigned int           fRegionHeight;
#endif
};

#endif
