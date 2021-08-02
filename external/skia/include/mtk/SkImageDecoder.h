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

#ifndef SkImageDecoder_DEFINED
#define SkImageDecoder_DEFINED

#include "SkBitmap.h"
#include "SkBRDAllocator.h"
#include "SkImage.h"
#include "SkPngChunkReader.h"
#include "SkRect.h"
#include "SkRefCnt.h"
#include "SkTRegistry.h"
#include "SkNoncopyable.h"
#include "SkEncodedInfo.h"

#include "SkColorTable.h"

class SkStream;
class SkStreamRewindable;

/** \class SkAutoTDelete
  An SkAutoTDelete<T> is like a T*, except that the destructor of SkAutoTDelete<T>
  automatically deletes the pointer it holds (if any).  That is, SkAutoTDelete<T>
  owns the T object that it points to.  Like a T*, an SkAutoTDelete<T> may hold
  either NULL or a pointer to a T object.  Also like T*, SkAutoTDelete<T> is
  thread-compatible, and once you dereference it, you get the threadsafety
  guarantees of T.

  The size of a SkAutoTDelete is small: sizeof(SkAutoTDelete<T>) == sizeof(T*)
*/
template <typename T> class SkAutoTDelete : public std::unique_ptr<T> {
public:
    SkAutoTDelete(T* obj = NULL) : std::unique_ptr<T>(obj) {}

    operator T*() const { return this->get(); }
    void free() { this->reset(nullptr); }
    T* detach() { return this->release(); }
};

/** \class SkImageDecoder

    Base class for decoding compressed images into a SkBitmap
*/
class SkImageDecoder : SkNoncopyable {
public:
    virtual ~SkImageDecoder();

    // TODO (scroggo): Merge with SkEncodedFormat
    enum Format {
        kUnknown_Format,
        kBMP_Format,
        kGIF_Format,
        kICO_Format,
        kJPEG_Format,
        kPNG_Format,
        kWBMP_Format,
        kWEBP_Format,
        kPKM_Format,
        kKTX_Format,
        kASTC_Format,

        kLastKnownFormat = kKTX_Format,
    };

    /** Return the format of image this decoder can decode. If this decoder can decode multiple
        formats, kUnknown_Format will be returned.
    */
    virtual Format getFormat() const;

    virtual SkImageInfo getImageInfo(sk_sp<SkColorSpace>) const;

    /** Return the format of the SkStreamRewindable or kUnknown_Format if it cannot be determined.
        Rewinds the stream before returning.
    */
    static Format GetStreamFormat(SkStreamRewindable*);

    /** Return a readable string of the Format provided.
    */
    static const char* GetFormatName(Format);

    /** Return a readable string of the value returned by getFormat().
    */
    const char* getFormatName() const;

    /** Whether the decoder should skip writing zeroes to output if possible.
    */
    bool getSkipWritingZeroes() const { return fSkipWritingZeroes; }

    /** Set to true if the decoder should skip writing any zeroes when
        creating the output image.
        This is a hint that may not be respected by the decoder.
        It should only be used if it is known that the memory to write
        to has already been set to 0; otherwise the resulting image will
        have garbage.
        This is ideal for images that contain a lot of completely transparent
        pixels, but may be a performance hit for an image that has only a
        few transparent pixels.
        The default is false.
    */
    void setSkipWritingZeroes(bool skip) { fSkipWritingZeroes = skip; }

    /** Returns true if the decoder should try to dither the resulting image.
        The default setting is true.
    */
    bool getDitherImage() const { return fDitherImage; }

    /** Set to true if the the decoder should try to dither the resulting image.
        The default setting is true.
    */
    void setDitherImage(bool dither) { fDitherImage = dither; }

    /** Returns true if the decoder should try to decode the
        resulting image to a higher quality even at the expense of
        the decoding speed.
    */
    bool getPreferQualityOverSpeed() const { return fPreferQualityOverSpeed; }

    /** Set to true if the the decoder should try to decode the
        resulting image to a higher quality even at the expense of
        the decoding speed.
    */
    void setPreferQualityOverSpeed(bool qualityOverSpeed) {
        fPreferQualityOverSpeed = qualityOverSpeed;
    }

    /** Set to true to require the decoder to return a bitmap with unpremultiplied
        colors. The default is false, meaning the resulting bitmap will have its
        colors premultiplied.
        NOTE: Passing true to this function may result in a bitmap which cannot
        be properly used by Skia.
    */
    void setRequireUnpremultipliedColors(bool request) {
        fRequireUnpremultipliedColors = request;
    }

    /** Returns true if the decoder will only return bitmaps with unpremultiplied
        colors.
    */
    bool getRequireUnpremultipliedColors() const { return fRequireUnpremultipliedColors; }

    SkPngChunkReader* getPeeker() const { return fPeeker; }
    SkPngChunkReader* setPeeker(SkPngChunkReader*);

    /**
     *  By default, the codec will try to comply with the "pref" colortype
     *  that is passed to decode() or decodeSubset(). However, this can be called
     *  to override that, causing the codec to try to match the src depth instead
     *  (as shown below).
     *
     *      src_8Index  -> kIndex_8_SkColorType
     *      src_8Gray   -> kN32_SkColorType
     *      src_8bpc    -> kN32_SkColorType
     */
    void setPreserveSrcDepth(bool preserve) {
        fPreserveSrcDepth = preserve;
    }

    SkBitmap::Allocator* getAllocator() const { return fAllocator; }
    SkBitmap::Allocator* setAllocator(SkBitmap::Allocator*);

    // sample-size, if set to > 1, tells the decoder to return a smaller than
    // original bitmap, sampling 1 pixel for every size pixels. e.g. if sample
    // size is set to 3, then the returned bitmap will be 1/3 as wide and high,
    // and will contain 1/9 as many pixels as the original.
    // Note: this is a hint, and the codec may choose to ignore this, or only
    // approximate the sample size.
    int getSampleSize() const { return fSampleSize; }
    void setSampleSize(int size);

    /** Reset the sampleSize to its default of 1
     */
    void resetSampleSize() { this->setSampleSize(1); }

    /** Decoding is synchronous, but for long decodes, a different thread can
        call this method safely. This sets a state that the decoders will
        periodically check, and if they see it changed to cancel, they will
        cancel. This will result in decode() returning false. However, there is
        no guarantee that the decoder will see the state change in time, so
        it is possible that cancelDecode() will be called, but will be ignored
        and decode() will return true (assuming no other problems were
        encountered).

        This state is automatically reset at the beginning of decode().
     */
    void cancelDecode() {
        // now the subclass must query shouldCancelDecode() to be informed
        // of the request
        fShouldCancelDecode = true;
    }

    /** Passed to the decode method. If kDecodeBounds_Mode is passed, then
        only the bitmap's info need be set. If kDecodePixels_Mode
        is passed, then the bitmap must have pixels or a pixelRef.
    */
    enum Mode {
        kDecodeBounds_Mode, //!< only return info in bitmap
        kDecodePixels_Mode  //!< return entire bitmap (including pixels)
    };

    /** Result of a decode. If read as a boolean, a partial success is
        considered a success (true).
    */
    enum Result {
        kFailure        = 0,    //!< Image failed to decode. bitmap will be
                                //   unchanged.
        kPartialSuccess = 1,    //!< Part of the image decoded. The rest is
                                //   filled in automatically
        kSuccess        = 2     //!< The entire image was decoded, if Mode is
                                //   kDecodePixels_Mode, or the bounds were
                                //   decoded, in kDecodeBounds_Mode.
    };

    /** Given a stream, decode it into the specified bitmap.
        If the decoder can decompress the image, it calls bitmap.setInfo(),
        and then if the Mode is kDecodePixels_Mode, call allocPixelRef(),
        which will allocated a pixelRef. To access the pixel memory, the codec
        needs to call lockPixels/unlockPixels on the
        bitmap. It can then set the pixels with the decompressed image.
    *   If the image cannot be decompressed, return kFailure. After the
    *   decoding, the function converts the decoded colortype in bitmap
    *   to pref if possible. Whether a conversion is feasible is
    *   tested by Bitmap::canCopyTo(pref).

        If an SkBitmap::Allocator is installed via setAllocator, it will be
        used to allocate the pixel memory. A clever allocator can be used
        to allocate the memory from a cache, volatile memory, or even from
        an existing bitmap's memory.

        If a Peeker is installed via setPeeker, it may be used to peek into
        meta data during the decode.
    */

    /**
     * Given a stream, build an index for doing tile-based decode.
     * The built index will be saved in the decoder, and the image size will
     * be returned in width and height.
     *
     * Takes ownership of the SkStreamRewindable, on success or failure.
     *
     * Return true for success or false on failure.
     */
    bool buildTileIndex(SkStreamRewindable*, int *width, int *height);

    /**
     * Decode a rectangle subset in the image.
     * The method can only be called after buildTileIndex().
     *
     * Return true for success.
     * Return false if the index is never built or failing in decoding.
     */

    /**
     *  @param requestedColorType Color type requested by the client
     *
     *  |requestedColorType| may be overriden.  We will default to kF16
     *  for high precision images.
     *
     *  In the general case, if it is possible to decode to
     *  |requestedColorType|, this returns |requestedColorType|.
     *  Otherwise, this returns a color type that is an appropriate
     *  match for the the encoded data.
     */
    SkColorType computeOutputColorType(SkColorType requestedColorType);

    /**
     *  @param outputColorType Color type that the client will decode to.
     *  @param prefColorSpace  Preferred color space to decode to.
     *                         This may not return |prefColorSpace| for a couple reasons.
     *                         (1) Android Principles: 565 must be sRGB, F16 must be
     *                             linear sRGB, transfer function must be parametric.
     *                         (2) Codec Limitations: F16 requires a linear color space.
     *
     *  Returns the appropriate color space to decode to.
     */
    sk_sp<SkColorSpace> computeOutputColorSpace(SkColorType outputColorType,
                                                sk_sp<SkColorSpace> prefColorSpace = nullptr);

    /** Given a stream, this will try to find an appropriate decoder object.
        If none is found, the method returns NULL.
    */
    static SkImageDecoder* Factory(SkStreamRewindable*);

protected:
    // must be overridden in subclasses. This guy is called by decode(...)

    // If the decoder wants to support tiled based decoding, this method must be overridden.
    // This is called by buildTileIndex(...)
    virtual bool onBuildTileIndex(SkStreamRewindable*, int* /*width*/, int* /*height*/);

    // If the decoder wants to support tiled based decoding,
    // this method must be overridden. This guy is called by decodeRegion(...)

    /** If planes or rowBytes is NULL, decodes the header and computes componentSizes
        for memory allocation.
        Otherwise, decodes the YUV planes into the provided image planes and
        updates componentSizes to the final image size.
        Returns whether the decoding was successful.
    */

    /*
     * Crop a rectangle from the src Bitmap to the dest Bitmap. src and dst are
     * both sampled by sampleSize from an original Bitmap.
     *
     * @param dst the destination bitmap.
     * @param src the source bitmap that is sampled by sampleSize from the
     *            original bitmap.
     * @param sampleSize the sample size that src is sampled from the original bitmap.
     * @param (dstX, dstY) the upper-left point of the dest bitmap in terms of
     *                     the coordinate in the original bitmap.
     * @param (width, height) the width and height of the unsampled dst.
     * @param (srcX, srcY) the upper-left point of the src bitmap in terms of
     *                     the coordinate in the original bitmap.
     * @return bool Whether or not it succeeded.
     */
    bool cropBitmap(SkBitmap *dst, SkBitmap *src, int sampleSize,
                    int dstX, int dstY, int width, int height,
                    int srcX, int srcY, SkBRDAllocator* allocator);

    /**
     *  Copy all fields on this decoder to the other decoder. Used by subclasses
     *  to decode a subimage using a different decoder, but with the same settings.
     */
    void copyFieldsToOther(SkImageDecoder* other);

    /** Can be queried from within onDecode, to see if the user (possibly in
        a different thread) has requested the decode to cancel. If this returns
        true, your onDecode() should stop and return false.
        Each subclass needs to decide how often it can query this, to balance
        responsiveness with performance.

        Calling this outside of onDecode() may return undefined values.
     */

public:
    bool shouldCancelDecode() const { return fShouldCancelDecode; }

protected:
    SkImageDecoder();

    /**
     *  Return the default preference being used by the current or latest call to decode.
     */
    SkColorType getDefaultPref() { return fDefaultPref; }

    /*  Helper for subclasses. Call this to allocate the pixel memory given the bitmap's info.
        Returns true on success. This method handles checking for an optional Allocator.
    */
    bool allocPixelRef(SkBitmap*, SkColorTable*, SkBRDAllocator* allocator) const;

    /**
     *  The raw data of the src image.
     */
    enum SrcDepth {
        // Color-indexed.
        kIndex_SrcDepth,
        // Grayscale in 8 bits.
        k8BitGray_SrcDepth,
        // 8 bits per component. Used for 24 bit if there is no alpha.
        k32Bit_SrcDepth,
    };
    /** The subclass, inside onDecode(), calls this to determine the colorType of
        the returned bitmap. SrcDepth and hasAlpha reflect the raw data of the
        src image. This routine returns the caller's preference given
        srcDepth and hasAlpha, or kUnknown_SkColorType if there is no preference.
     */
    SkColorType getPrefColorType(SrcDepth, bool hasAlpha) const;

private:
    SkPngChunkReader*       fPeeker;
    SkBitmap::Allocator*    fAllocator;
    int                     fSampleSize;
    SkColorType             fDefaultPref;   // use if fUsePrefTable is false
    bool                    fPreserveSrcDepth;
    bool                    fDitherImage;
    bool                    fSkipWritingZeroes;
    mutable bool            fShouldCancelDecode;
    bool                    fPreferQualityOverSpeed;
    bool                    fRequireUnpremultipliedColors;
public:
    // prefer size, if set to > 0, tells the decoder to returen a bitmap that
    // max dimension fit this value
    int getPreferSize() const { return fPreferSize; }
    void setPreferSize(int size);

    // post process flag, tell decoder to enhance color/sharpness.
    int getPostProcFlag() const { return fPostProc; }
    void setPostProcFlag(int flag);

    bool decodeSubset(SkBitmap* bm, SkBRDAllocator* allocator, const SkIRect& subset, SkColorType pref, int sampleSize, sk_sp<SkColorSpace> dstColorSpace);

protected:
    virtual bool onDecodeSubset(SkBitmap* bitmap, SkBRDAllocator* allocator, const SkIRect& rect, int sampleSize, sk_sp<SkColorSpace> dstColorSpace) {
        return false;
    }
    unsigned int fISOSpeedRatings;
    SkEncodedInfo fEncodedInfo;
    SkImageInfo fImageInfo;

private:
    int   fPreferSize;
    int   fPostProc;
    void* fdc;
    int   fsize;
};

/** Calling newDecoder with a stream returns a new matching imagedecoder
    instance, or NULL if none can be found. The caller must manage its ownership
    of the stream as usual, calling unref() when it is done, as the returned
    decoder may have called ref() (and if so, the decoder is responsible for
    balancing its ownership when it is destroyed).
 */
class SkImageDecoderFactory : public SkRefCnt {
public:
    virtual SkImageDecoder* newDecoder(SkStreamRewindable*) = 0;

private:
    typedef SkRefCnt INHERITED;
};

class SkDefaultImageDecoderFactory : SkImageDecoderFactory {
public:
    // calls SkImageDecoder::Factory(stream)
    virtual SkImageDecoder* newDecoder(SkStreamRewindable* stream) {
        return SkImageDecoder::Factory(stream);
    }
};

// Typedefs to make registering decoder and formatter callbacks easier.
// These have to be defined outside SkImageDecoder. :(
typedef SkTRegistry<SkImageDecoder*(*)(SkStreamRewindable*)>        SkImageDecoder_DecodeReg;
typedef SkTRegistry<SkImageDecoder::Format(*)(SkStreamRewindable*)> SkImageDecoder_FormatReg;

#endif
