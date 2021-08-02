/*
 * Copyright 2010 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkJpegUtility_MTK.h"

#include "SkCodecPriv.h"

/*
 * Call longjmp to continue execution on an error
 */
void skjpeg_err_exit_MTK(j_common_ptr_ALPHA dinfo) {
    // Simply return to Skia client code
    // JpegDecoderMgr will take care of freeing memory
    skjpeg_error_mgr_MTK* error = (skjpeg_error_mgr_MTK*) dinfo->err;
    (*error->output_message) (dinfo);

    if (error->fJmpBufStack.empty()) {
        SK_ABORT("JPEG error with no jmp_buf set.");
    }

    /* Let the memory manager delete any temp files before we die */
    jpeg_destroy_ALPHA(dinfo);

    longjmp(*error->fJmpBufStack.back(), -1);
}

static void sk_init_source_MTK(j_decompress_ptr_ALPHA cinfo) {
    skjpeg_source_mgr_MTK*  src = (skjpeg_source_mgr_MTK*)cinfo->src;
    src->next_input_byte = (const JOCTET_ALPHA*)src->fBuffer;
    src->bytes_in_buffer = 0;
    src->current_offset = 0;
    if (!src->fStream->rewind()) {
        SkCodecPrintf("xxxxxxxxxxxxxx failure to rewind\n");
        cinfo->err->error_exit((j_common_ptr_ALPHA)cinfo);
    }
}

static boolean_ALPHA sk_seek_input_data_MTK(j_decompress_ptr_ALPHA cinfo, long byte_offset) {
    skjpeg_source_mgr_MTK* src = (skjpeg_source_mgr_MTK*)cinfo->src;
    size_t bo = (size_t) byte_offset;

    if (bo > src->current_offset) {
        (void)src->fStream->skip(bo - src->current_offset);
    } else {
        if (!src->fStream->rewind()) {
            SkCodecPrintf("xxxxxxxxxxxxxx failure to rewind\n");
            cinfo->err->error_exit((j_common_ptr_ALPHA)cinfo);
            return false;
        }
        (void)src->fStream->skip(bo);
    }

    src->current_offset = bo;
    src->next_input_byte = (const JOCTET_ALPHA*)src->fBuffer;
    src->bytes_in_buffer = 0;
    return true;
}

static boolean_ALPHA sk_fill_input_buffer_MTK(j_decompress_ptr_ALPHA cinfo) {
    skjpeg_source_mgr_MTK* src = (skjpeg_source_mgr_MTK*)cinfo->src;
    if (src->fDecoder != nullptr && src->fDecoder->shouldCancelDecode()) {
        return FALSE_ALPHA;
    }
    size_t bytes = src->fStream->read(src->fBuffer, skjpeg_source_mgr_MTK::kBufferSize);
    // note that JPEG is happy with less than the full read,
    // as long as the result is non-zero
    if (bytes == 0) {
        return FALSE_ALPHA;
    }

    src->current_offset += bytes;
    src->next_input_byte = (const JOCTET_ALPHA*)src->fBuffer;
    src->bytes_in_buffer = bytes;
    return TRUE_ALPHA;
}

static void sk_skip_input_data_MTK(j_decompress_ptr_ALPHA cinfo, long num_bytes) {
    skjpeg_source_mgr_MTK*  src = (skjpeg_source_mgr_MTK*)cinfo->src;

    if (num_bytes > (long)src->bytes_in_buffer) {
        size_t bytesToSkip = num_bytes - src->bytes_in_buffer;
        while (bytesToSkip > 0) {
            size_t bytes = src->fStream->skip(bytesToSkip);
            if (bytes <= 0 || bytes > bytesToSkip) {
                cinfo->err->error_exit((j_common_ptr_ALPHA)cinfo);
                return;
            }
            src->current_offset += bytes;
            bytesToSkip -= bytes;
        }
        src->next_input_byte = (const JOCTET_ALPHA*)src->fBuffer;
        src->bytes_in_buffer = 0;
    } else {
        src->next_input_byte += num_bytes;
        src->bytes_in_buffer -= num_bytes;
    }
}

// Functions for buffered sources //

/*
 * Initialize the buffered source manager
 */
static void sk_init_buffered_source_MTK(j_decompress_ptr_ALPHA dinfo) {
    skjpeg_source_mgr_MTK* src = (skjpeg_source_mgr_MTK*) dinfo->src;
    src->next_input_byte = (const JOCTET_ALPHA*) src->fBuffer;
    src->bytes_in_buffer = 0;
}

/*
 * Fill the input buffer from the stream
 */
static boolean_ALPHA sk_fill_buffered_input_buffer_MTK(j_decompress_ptr_ALPHA dinfo) {
    skjpeg_source_mgr_MTK* src = (skjpeg_source_mgr_MTK*) dinfo->src;
    size_t bytes = src->fStream->read(src->fBuffer, skjpeg_source_mgr_MTK::kBufferSize);

    // libjpeg is still happy with a less than full read, as long as the result is non-zero
    if (bytes == 0) {
        // Let libjpeg know that the buffer needs to be refilled
        src->next_input_byte = nullptr;
        src->bytes_in_buffer = 0;
        return FALSE_ALPHA;
    }

    src->next_input_byte = (const JOCTET_ALPHA*) src->fBuffer;
    src->bytes_in_buffer = bytes;
    return true;
}

/*
 * Skip a certain number of bytes in the stream
 */
static void sk_skip_buffered_input_data_MTK(j_decompress_ptr_ALPHA dinfo, long numBytes) {
    skjpeg_source_mgr_MTK* src = (skjpeg_source_mgr_MTK*) dinfo->src;
    size_t bytes = (size_t) numBytes;

    if (bytes > src->bytes_in_buffer) {
        size_t bytesToSkip = bytes - src->bytes_in_buffer;
        if (bytesToSkip != src->fStream->skip(bytesToSkip)) {
            SkCodecPrintf("Failure to skip.\n");
            dinfo->err->error_exit((j_common_ptr_ALPHA) dinfo);
            return;
        }

        src->next_input_byte = (const JOCTET_ALPHA*) src->fBuffer;
        src->bytes_in_buffer = 0;
    } else {
        src->next_input_byte += numBytes;
        src->bytes_in_buffer -= numBytes;
    }
}

/*
 * We do not need to do anything to terminate our stream
 */
static void sk_term_source_MTK(j_decompress_ptr_ALPHA dinfo)
{
    // The current implementation of SkJpegCodec does not call
    // jpeg_finish_decompress(), so this function is never called.
    // If we want to modify this function to do something, we also
    // need to modify SkJpegCodec to call jpeg_finish_decompress().
}

// Functions for memory backed sources //

/*
 * Initialize the mem backed source manager
 */
static void sk_init_mem_source_MTK(j_decompress_ptr_ALPHA dinfo) {
    /* no work necessary here, all things are done in constructor */
}

static void sk_skip_mem_input_data_MTK(j_decompress_ptr_ALPHA cinfo, long num_bytes) {
    jpeg_source_mgr_ALPHA* src = cinfo->src;
    size_t bytes = static_cast<size_t>(num_bytes);
    if(bytes > src->bytes_in_buffer) {
        src->next_input_byte = nullptr;
        src->bytes_in_buffer = 0;
    } else {
        src->next_input_byte += bytes;
        src->bytes_in_buffer -= bytes;
    }
}

static boolean_ALPHA sk_fill_mem_input_buffer_MTK(j_decompress_ptr_ALPHA cinfo) {
    /* The whole JPEG data is expected to reside in the supplied memory,
     * buffer, so any request for more data beyond the given buffer size
     * is treated as an error.
     */
    return FALSE_ALPHA;
}

skjpeg_source_mgr_MTK::skjpeg_source_mgr_MTK(SkStream* stream, SkImageDecoder* decoder)
    : fStream(stream)
    , fDecoder(decoder) {

    if (stream->hasLength() && stream->getMemoryBase()) {
        init_source = sk_init_mem_source_MTK;
        fill_input_buffer = sk_fill_mem_input_buffer_MTK;
        skip_input_data = sk_skip_mem_input_data_MTK;
        resync_to_restart = jpeg_resync_to_restart_ALPHA;
        term_source = sk_term_source_MTK;
        bytes_in_buffer = static_cast<size_t>(stream->getLength());
        next_input_byte = static_cast<const JOCTET_ALPHA*>(stream->getMemoryBase());
        seek_input_data = NULL;
        current_offset = static_cast<size_t>(stream->getLength());
        start_input_byte = static_cast<const JOCTET_ALPHA*>(stream->getMemoryBase());
    }else{
        init_source = sk_init_source_MTK;
        fill_input_buffer = sk_fill_input_buffer_MTK;
        skip_input_data = sk_skip_input_data_MTK;
        resync_to_restart = jpeg_resync_to_restart_ALPHA;
        term_source = sk_term_source_MTK;
        seek_input_data = sk_seek_input_data_MTK;
    }
}

/*
 * Constructor for the source manager that we provide to libjpeg
 * We provide skia implementations of all of the stream processing functions required by libjpeg
 */
skjpeg_source_mgr_MTK::skjpeg_source_mgr_MTK(SkStream* stream)
    : fStream(stream)
    , fDecoder(NULL)
{
    if (stream->hasLength() && stream->getMemoryBase()) {
        init_source = sk_init_mem_source_MTK;
        fill_input_buffer = sk_fill_mem_input_buffer_MTK;
        skip_input_data = sk_skip_mem_input_data_MTK;
        resync_to_restart = jpeg_resync_to_restart_ALPHA;
        term_source = sk_term_source_MTK;
        bytes_in_buffer = static_cast<size_t>(stream->getLength());
        next_input_byte = static_cast<const JOCTET_ALPHA*>(stream->getMemoryBase());
        seek_input_data = NULL;
        current_offset = static_cast<size_t>(stream->getLength());
        start_input_byte = static_cast<const JOCTET_ALPHA*>(stream->getMemoryBase());
    } else {
        init_source = sk_init_buffered_source_MTK;
        fill_input_buffer = sk_fill_buffered_input_buffer_MTK;
        skip_input_data = sk_skip_buffered_input_data_MTK;
        resync_to_restart = jpeg_resync_to_restart_ALPHA;
        term_source = sk_term_source_MTK;
        seek_input_data = sk_seek_input_data_MTK;
    }
}
