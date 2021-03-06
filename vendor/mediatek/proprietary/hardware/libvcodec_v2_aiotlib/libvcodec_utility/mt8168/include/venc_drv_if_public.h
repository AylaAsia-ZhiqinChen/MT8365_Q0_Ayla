#ifndef _VENC_DRV_IF_PUBLIC_H_
#define _VENC_DRV_IF_PUBLIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "val_types_public.h"

#define VENC_ENC_QUEUE_SIZE             (32)

/**
 * @par Enumeration
 *   VENC_DRV_QUERY_TYPE_T
 * @par Description
 *   This is the item used for query driver
 */
typedef enum __VENC_DRV_QUERY_TYPE_T {
	VENC_DRV_QUERY_TYPE_NONE,                   /* /< Default value (not used) */
	VENC_DRV_QUERY_TYPE_VIDEO_FORMAT,           /* /< Query the driver capability */
	VENC_DRV_QUERY_TYPE_VIDEO_PROPERTY,         /* /< Query the video property */
	VENC_DRV_QUERY_TYPE_VIDEO_PROPERTY_LIST,    /* /< Query the video property list */
	VENC_DRV_QUERY_TYPE_PROPERTY,               /* /< Get the driver property */
	VENC_DRV_QUERY_TYPE_MCI_SUPPORTED,          /* /< Query if the codec support MCI */
	VENC_DRV_QUERY_TYPE_CHIP_NAME,              /* /< Query chip name */
	VENC_DRV_QUERY_TYPE_INPUT_BUF_LIMIT,        /* /< Query input buffer stride and sliceheight */
	VENC_DRV_QUERY_TYPE_NORMAL_PRIO,            /* /< Query if recorder scenario adjust to normal priority, for 6571. */
	VENC_DRV_QUERY_TYPE_VIDEO_CAMCORDER_CAP,    /* /< Query spec. for MediaProfile */
	VENC_DRV_QUERY_TYPE_CHIP_VARIANT,           /* /< Query chip variant */
	VENC_DRV_QUERY_TYPE_SUPPORTED_FORMATS,      /* /< Query driver supported format */
	VENC_DRV_QUERY_TYPE_FRAME_SIZES,            /* /< Query driver supported frame size */
	VENC_DRV_QUERY_TYPE_MAX = 0xFFFFFFFF        /* /< Max VENC_DRV_QUERY_TYPE_T value */
}
VENC_DRV_QUERY_TYPE_T;


/**
 * @par Enumeration
 *   VENC_DRV_YUV_FORMAT_T
 * @par Description
 *   This is the item used for input YUV buffer format
 */
typedef enum __VENC_DRV_YUV_FORMAT_T {
	VENC_DRV_YUV_FORMAT_NONE = 0,                   /* /< Default value (not used) */
	VENC_DRV_YUV_FORMAT_GRAY,                   /* /< GRAY YUV format */
	VENC_DRV_YUV_FORMAT_422,                    /* /< 422 YUV format */
	VENC_DRV_YUV_FORMAT_420,                    /* /< 420 YUV format */
	VENC_DRV_YUV_FORMAT_411,                    /* /< 411 YUV format */
	VENC_DRV_YUV_FORMAT_YV12,                   /* /< Android YV12 (16/16/16) YUV format */
	VENC_DRV_YUV_FORMAT_NV12,                   /* /< NV12 YUV format */
	VENC_DRV_YUV_FORMAT_NV21,                   /* /< NV21 YUV format */
	VENC_DRV_YUV_FORMAT_BLK16X32,               /* /< Block 16x32 YUV format */
	VENC_DRV_YUV_FORMAT_BLK64X32,               /* /< Block 64x32 YUV format */
	VENC_DRV_YUV_FORMAT_YV12_1688,              /* /< YV12 YUV format */
	VENC_DRV_YUV_FORMAT_24bitRGB888,            /* 11 /<24-bit RGB format */
	VENC_DRV_YUV_FORMAT_24bitBGR888,            /* 12 /<24-bit BGR format */
	VENC_DRV_YUV_FORMAT_32bitRGBA8888,          /* 13 /<32-bit RGBA format */
	VENC_DRV_YUV_FORMAT_32bitBGRA8888,          /* 14 /<32-bit BGRA format */
	VENC_DRV_YUV_FORMAT_32bitARGB8888,          /* 15 /<32-bit ARGB format */
	VENC_DRV_YUV_FORMAT_32bitABGR8888,          /* 16 /<32-bit ABGR format */
	VENC_DRV_YUV_FORMAT_MAX = 0xFFFFFFFF        /* /< Max VENC_DRV_YUV_FORMAT_T value */
} VENC_DRV_YUV_FORMAT_T;


/**
 * @par Enumeration
 *   VENC_DRV_VIDEO_FORMAT_T
 * @par Description
 *   This is the item used for encode video format
 */
typedef enum __VENC_DRV_VIDEO_FORMAT_T {
	VENC_DRV_VIDEO_FORMAT_NONE,                 /* /< Default value (not used) */
	VENC_DRV_VIDEO_FORMAT_MPEG4,                /* /< MPEG4 video format */
	VENC_DRV_VIDEO_FORMAT_MPEG4_1080P,          /* /< MPEG4 video format for 1080p */
	VENC_DRV_VIDEO_FORMAT_MPEG4_SHORT,          /* /< MPEG4_SHORT (H.263 baseline profile) video format */
	VENC_DRV_VIDEO_FORMAT_H263,                 /* /< H.263 video format */
	VENC_DRV_VIDEO_FORMAT_H264,                 /* /< H.264 video format */
	VENC_DRV_VIDEO_FORMAT_H264_VGA,             /* /< H.264 video format for VGA */
	VENC_DRV_VIDEO_FORMAT_WMV9,                 /* /< WMV9 video format */
	VENC_DRV_VIDEO_FORMAT_VC1,                  /* /< VC1 video format */
	VENC_DRV_VIDEO_FORMAT_VP8,                  /* /< VP8 video format */
	VENC_DRV_VIDEO_FORMAT_JPEG,                 /* /< JPEG picture format */
	VENC_DRV_VIDEO_FORMAT_HEVC,                 /* /< HEVC video format */
	VENC_DRV_VIDEO_FORMAT_H264SEC,              /* /<: Secure H.264 */
	VENC_DRV_VIDEO_FORMAT_INHOUSE_H264SEC,      /* /<: Inhouse Secure H.264 */
	VENC_DRV_VIDEO_FORMAT_H263VT,               /* /<: H.263 VT*/
	VENC_DRV_VIDEO_FORMAT_HEIF,                 /* /<: HEIF image format */
	VENC_DRV_VIDEO_FORMAT_MAX = 0xFFFFFFFF      /* /< Max VENC_DRV_VIDEO_FORMAT_T value */
} VENC_DRV_VIDEO_FORMAT_T;


/**
 * @par Enumeration
 *   VENC_DRV_FRAME_RATE_T
 * @par Description
 *   This is the item used for encode frame rate
 */
typedef enum __VENC_DRV_FRAME_RATE_T {
	VENC_DRV_FRAME_RATE_NONE    = 0,            /* /< Default value (not used) */
	VENC_DRV_FRAME_RATE_7_5     = 75,           /* /< 7.5 */
	VENC_DRV_FRAME_RATE_10      = 10,           /* /< 10 */
	VENC_DRV_FRAME_RATE_15      = 15,           /* /< 15 */
	VENC_DRV_FRAME_RATE_20      = 20,           /* /< 20 */
	VENC_DRV_FRAME_RATE_24      = 24,           /* /< 24 */
	VENC_DRV_FRAME_RATE_25      = 25,           /* /< 25 */
	VENC_DRV_FRAME_RATE_29_97   = 2997,         /* /< 29.97 */
	VENC_DRV_FRAME_RATE_30      = 30,           /* /< 30 */
	VENC_DRV_FRAME_RATE_60      = 60,           /* /< 60 */
	VENC_DRV_FRAME_RATE_120     = 120,          /* /< 120 */
	VENC_DRV_FRAME_RATE_180     = 180,          /* /< 180 */
	VENC_DRV_FRAME_RATE_240     = 240,          /* /< 240 */
	VENC_DRV_FRAME_RATE_480     = 480,          /* /< 480 */
	VENC_DRV_FRAME_RATE_MAX     = 0xFFFFFFFF    /* /< Max VENC_DRV_FRAME_RATE_T value */
} VENC_DRV_FRAME_RATE_T;


/**
 * @par Enumeration
 *   VENC_DRV_START_OPT_T
 * @par Description
 *   This is the item used for encode frame type
 */
typedef enum __VENC_DRV_START_OPT_T {
	VENC_DRV_START_OPT_NONE,                                /* /< Default value (not used) */
	VENC_DRV_START_OPT_ENCODE_SEQUENCE_HEADER,              /* /< Encode a Sequence header */
	VENC_DRV_START_OPT_ENCODE_SEQUENCE_HEADER_H264_SPS,     /* /< Encode a Sequence header H264 SPS */
	VENC_DRV_START_OPT_ENCODE_SEQUENCE_HEADER_H264_PPS,     /* /< Encode a Sequence header H264 PPS */
	VENC_DRV_START_OPT_ENCODE_FRAME,                        /* /< Encode a frame */
	VENC_DRV_START_OPT_ENCODE_KEY_FRAME,                    /* /< Encode a key frame */
	VENC_DRV_START_OPT_ENCODE_FINAL,                        /* /< Final encode (Only use to encode final frame) */
	VENC_DRV_START_OPT_ENCODE_DUMMY_NAL,                    /* /< Encode a dummy NAL for WFD */
	VENC_DRV_START_OPT_MAX = 0xFFFFFFFF                     /* /< Max VENC_DRV_START_OPT_T value */
} VENC_DRV_START_OPT_T;


/**
 * @par Enumeration
 *   VENC_DRV_MESSAGE_T
 * @par Description
 *   This is the item used for encode frame status
 */
typedef enum __VENC_DRV_MESSAGE_T {
	VENC_DRV_MESSAGE_NONE,                      /* /< Default value (not used) */
	VENC_DRV_MESSAGE_OK,                        /* /< Encode ok */
	VENC_DRV_MESSAGE_ERR,                       /* /< Encode error */
	VENC_DRV_MESSAGE_TIMEOUT,                   /* /< Encode timeout */
	VENC_DRV_MESSAGE_PARTIAL,                   /* /< Encode partial frame (ok means EOF) */
	VENC_DRV_MESSAGE_MAX = 0xFFFFFFFF           /* /< Max VENC_DRV_MESSAGE_T value */
} VENC_DRV_MESSAGE_T;


/**
 * @par Enumeration
 *   VENC_DRV_H264_VIDEO_PROFILE_T
 * @par Description
 *   This is the item used for h.264 encoder profile capability
 */
typedef enum __VENC_DRV_H264_VIDEO_PROFILE_T {
	VENC_DRV_H264_VIDEO_PROFILE_UNKNOWN              = 0,           /* /< Default value (not used) */
	VENC_DRV_H264_VIDEO_PROFILE_BASELINE             = (1 << 0),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_BASELINE Baseline */
	VENC_DRV_H264_VIDEO_PROFILE_CONSTRAINED_BASELINE = (1 << 1),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_CONSTRAINED_BASELINE Constrained Baseline */
	VENC_DRV_H264_VIDEO_PROFILE_MAIN                 = (1 << 2),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_MAIN Main */
	VENC_DRV_H264_VIDEO_PROFILE_EXTENDED             = (1 << 3),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_EXTENDED Extended */
	VENC_DRV_H264_VIDEO_PROFILE_HIGH                 = (1 << 4),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_HIGH High */
	VENC_DRV_H264_VIDEO_PROFILE_HIGH_10              = (1 << 5),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_10 High 10 */
	VENC_DRV_H264_VIDEO_PROFILE_HIGH422              = (1 << 6),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_422 High 422 */
	VENC_DRV_H264_VIDEO_PROFILE_HIGH444              = (1 << 7),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_444_PREDICTIVE High 444 */
	VENC_DRV_H264_VIDEO_PROFILE_HIGH_10_INTRA        = (1 << 8),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_10_INTRA High 10 Intra (Amendment 2) */
	VENC_DRV_H264_VIDEO_PROFILE_HIGH422_INTRA        = (1 << 9),    /* /<V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_422_INTRA High 422 Intra (Amendment 2) */
	VENC_DRV_H264_VIDEO_PROFILE_HIGH444_INTRA        = (1 << 10),   /* /<V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_444_INTRA High 444 Intra (Amendment 2) */
	VENC_DRV_H264_VIDEO_PROFILE_CAVLC444_INTRA       = (1 << 11),   /* /<V4L2_MPEG_VIDEO_H264_PROFILE_CAVLC_444_INTRA CAVLC 444 Intra (Amendment 2) */
	VENC_DRV_H264_VIDEO_PROFILE_SCALABLE_BASELINE    = (1 << 12),   /* /<V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_BASELINE Scalable Baseline (Amendment 3) */
	VENC_DRV_H264_VIDEO_PROFILE_SCALABLE_HIGH        = (1 << 13),   /* /<V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_HIGH Scalable High (Amendment 3) */
	VENC_DRV_H264_VIDEO_PROFILE_SCALABLE_HIGH_INTRA  = (1 << 14),   /* /<V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_HIGH_INTRA Scalable High Intra (Amendment 3) */
	VENC_DRV_H264_VIDEO_PROFILE_STEREO_HIGH          = (1 << 15),   /* /<V4L2_MPEG_VIDEO_H264_PROFILE_STEREO_HIGH Stereo High */
	VENC_DRV_H264_VIDEO_PROFILE_MULTIVIEW_HIGH       = (1 << 16),   /* /<V4L2_MPEG_VIDEO_H264_PROFILE_MULTIVIEW_HIGH Multiview High (Corrigendum 1 (2009)) */
	VENC_DRV_H264_VIDEO_PROFILE_MAX                  = 0xFFFFFFFF   /* /< Max VENC_DRV_H264_VIDEO_PROFILE_T value */
} VENC_DRV_H264_VIDEO_PROFILE_T;


/**
 * @par Enumeration
 *   VENC_DRV_HEVC_VIDEO_PROFILE_T
 * @par Description
 *   This is the item used for hevc encoder profile capability
 */
typedef enum __VENC_DRV_HEVC_VIDEO_PROFILE_T {
	VENC_DRV_HEVC_VIDEO_PROFILE_UNKNOWN              = 0,           /* /< Default value (not used) */
	VENC_DRV_HEVC_VIDEO_PROFILE_MAIN                     = (1 << 0),    /* /<V4L2_MPEG_VIDEO_H265_PROFILE_MAIN Main */
	VENC_DRV_HEVC_VIDEO_PROFILE_MAIN10                   = (1 << 1),    /* /<V4L2_MPEG_VIDEO_H265_PROFILE_MAIN10 Main */
	VENC_DRV_HEVC_VIDEO_PROFILE_MAIN_HDR10               = (1 << 2),    /* /<V4L2_MPEG_VIDEO_H265_PROFILE_MAIN_HDR10 Main */
	VENC_DRV_HEVC_VIDEO_PROFILE_MAIN_STILL_PIC           = (1 << 3),    /* /<V4L2_MPEG_VIDEO_H265_PROFILE_MAIN_STILL_PIC Main */
	VENC_DRV_HEVC_VIDEO_PROFILE_MAX                      = 0xFFFFFFFF   /* /< Max VENC_DRV_HEVC_VIDEO_PROFILE_T value */
} VENC_DRV_HEVC_VIDEO_PROFILE_T;


/**
 * @par Enumeration
 *   VENC_DRV_MPEG_VIDEO_PROFILE_T
 * @par Description
 *   This is the item used for h.263, mpeg2, mpeg4 encoder profile capability
 */
typedef enum __VENC_DRV_MPEG_VIDEO_PROFILE_T {
	VENC_DRV_MPEG_VIDEO_PROFILE_UNKNOWN               = 0,          /* /< Default value (not used) */
	VENC_DRV_MPEG_VIDEO_PROFILE_MPEG4_SIMPLE          = (1 << 0),   /* /<V4L2_MPEG_VIDEO_MPEG4_PROFILE_SIMPLE MPEG4 Simple */
	VENC_DRV_MPEG_VIDEO_PROFILE_MPEG4_ADVANCED_SIMPLE = (1 << 1),   /* /<V4L2_MPEG_VIDEO_MPEG4_PROFILE_ADVANCED_SIMPLE MPEG4 Advanced Simple */
	VENC_DRV_MPEG_VIDEO_PROFILE_H263_0                = (1 << 2),   /* /< H.263 0 */
	VENC_DRV_MPEG_VIDEO_PROFILE_H263_1                = (1 << 3),   /* /< H.263 1 */
	VENC_DRV_MPEG_VIDEO_PROFILE_H263_2                = (1 << 4),   /* /< H.263 2 */
	VENC_DRV_MPEG_VIDEO_PROFILE_H263_3                = (1 << 5),   /* /< H.263 3 */
	VENC_DRV_MPEG_VIDEO_PROFILE_H263_4                = (1 << 6),   /* /< H.263 4 */
	VENC_DRV_MPEG_VIDEO_PROFILE_H263_5                = (1 << 7),   /* /< H.263 5 */
	VENC_DRV_MPEG_VIDEO_PROFILE_H263_6                = (1 << 8),   /* /< H.263 6 */
	VENC_DRV_MPEG_VIDEO_PROFILE_H263_7                = (1 << 9),   /* /< H.263 7 */
	VENC_DRV_MPEG_VIDEO_PROFILE_H263_8                = (1 << 10),  /* /< H.263 8 */
	VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_SIMPLE          = (1 << 11),  /* /< MPEG2 Simple */
	VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_MAIN            = (1 << 12),  /* /< MPEG2 Main */
	VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_SNR             = (1 << 13),  /* /< MPEG2 SNR */
	VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_SPATIAL         = (1 << 14),  /* /< MPEG2 Spatial */
	VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_HIGH            = (1 << 15),  /* /< MPEG2 High */
	VENC_DRV_MPEG_VIDEO_PROFILE_MAX                   = 0xFFFFFFFF  /* /< Max VENC_DRV_MPEG_VIDEO_PROFILE_T value */
} VENC_DRV_MPEG_VIDEO_PROFILE_T;

typedef enum __VENC_DRV_MPEG4_VIDEO_PROFILE_T {
	VENC_DRV_MPEG4_VIDEO_PROFILE_SIMPLE				= 0,
	VENC_DRV_MPEG4_VIDEO_PROFILE_ADVANCED_SIMPLE			= 1,
	VENC_DRV_MPEG4_VIDEO_PROFILE_CORE				= 2,
	VENC_DRV_MPEG4_VIDEO_PROFILE_SIMPLE_SCALABLE			= 3,
	VENC_DRV_MPEG4_VIDEO_PROFILE_ADVANCED_CODING_EFFICIENCY	= 4,
} VENC_DRV_MPEG4_VIDEO_PROFILE_T;

/**
 * @par Enumeration
 *   VENC_DRV_MS_VIDEO_PROFILE_T
 * @par Description
 *   This is the item used for MS encoder profile capability
 */
typedef enum __VENC_DRV_MS_VIDEO_PROFILE_T {
	VENC_DRV_MS_VIDEO_PROFILE_UNKNOWN      = 0,             /* /< Default value (not used) */
	VENC_DRV_MS_VIDEO_PROFILE_VC1_SIMPLE   = (1 << 0),      /* /< VC1 Simple */
	VENC_DRV_MS_VIDEO_PROFILE_VC1_MAIN     = (1 << 1),      /* /< VC1 Main */
	VENC_DRV_MS_VIDEO_PROFILE_VC1_ADVANCED = (1 << 2),      /* /< VC1 Advanced */
	VENC_DRV_MS_VIDEO_PROFILE_WMV9_SIMPLE  = (1 << 3),      /* /< WMV9 Simple */
	VENC_DRV_MS_VIDEO_PROFILE_WMV9_MAIN    = (1 << 4),      /* /< WMV9 Main */
	VENC_DRV_MS_VIDEO_PROFILE_WMV9_COMPLEX = (1 << 5),      /* /< WMV9 Complex */
	VENC_DRV_MS_VIDEO_PROFILE_MAX          = 0xFFFFFFFF     /* /< Max VENC_DRV_MS_VIDEO_PROFILE_T value */
} VENC_DRV_MS_VIDEO_PROFILE_T;


/**
 * @par Enumeration
 *   VENC_DRV_VIDEO_LEVEL_T
 * @par Description
 *   This is the item used for encoder level capability
 */
typedef enum __VENC_DRV_VIDEO_LEVEL_T {
	VENC_DRV_VIDEO_LEVEL_UNKNOWN = 0,       /* /< Default value (not used) */
	VENC_DRV_VIDEO_LEVEL_0,                 /* /< VC1 */
	VENC_DRV_VIDEO_LEVEL_1,                 /* /< H264, HEVC, VC1, MPEG4 */
	VENC_DRV_VIDEO_LEVEL_1b,                /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_1_1,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_1_2,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_1_3,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_2,                 /* /< H264, HEVC, VC1, MPEG4 */
	VENC_DRV_VIDEO_LEVEL_2_1,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_2_2,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_3,                 /* /< H264, HEVC, VC1, MPEG4 */
	VENC_DRV_VIDEO_LEVEL_3_1,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_3_2,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_4,                 /* /< H264, HEVC, VC1 */
	VENC_DRV_VIDEO_LEVEL_4_1,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_4_2,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_5,                 /* /< H264, HEVC, HEVC */
	VENC_DRV_VIDEO_LEVEL_5_1,               /* /< H264, HEVC */
	VENC_DRV_VIDEO_LEVEL_LOW,               /* /< VC1, MPEG2 */
	VENC_DRV_VIDEO_LEVEL_MEDIUM,            /* /< VC1, MPEG2 */
	VENC_DRV_VIDEO_LEVEL_HIGH1440,          /* /< MPEG2 */
	VENC_DRV_VIDEO_LEVEL_HIGH,              /* /< VC1, MPEG2 */
	VENC_DRV_VIDEO_LEVEL_6,                 /* /< H263 */
	VENC_DRV_VIDEO_LEVEL_7,               /* /< H263 */
	VENC_DRV_VIDEO_LEVEL_MAX = 0xFFFFFFFF   /* /< Max VENC_DRV_VIDEO_LEVEL_T value */
} VENC_DRV_VIDEO_LEVEL_T;


/**
 * @par Enumeration
 *   VENC_DRV_RESOLUTION_T
 * @par Description
 *   This is the item used for encoder resolution capability
 */
typedef enum __VENC_DRV_RESOLUTION_T {
	VENC_DRV_RESOLUTION_UNKNOWN = 0,                /* /< Default value (not used) */
	VENC_DRV_RESOLUTION_SUPPORT_QCIF,               /* /< CIF */
	VENC_DRV_RESOLUTION_SUPPORT_QVGA,               /* /< QVGA */
	VENC_DRV_RESOLUTION_SUPPORT_CIF,                /* /< QCIF */
	VENC_DRV_RESOLUTION_SUPPORT_HVGA,               /* /< HVGA: 480x320 */
	VENC_DRV_RESOLUTION_SUPPORT_VGA,                /* /< VGA: 640x480 */
	VENC_DRV_RESOLUTION_SUPPORT_480I,               /* /< 480I */
	VENC_DRV_RESOLUTION_SUPPORT_480P,               /* /< 480P */
	VENC_DRV_RESOLUTION_SUPPORT_576I,               /* /< 576I */
	VENC_DRV_RESOLUTION_SUPPORT_576P,               /* /< 480P */
	VENC_DRV_RESOLUTION_SUPPORT_FWVGA,              /* /< FWVGA: 864x480 */
	VENC_DRV_RESOLUTION_SUPPORT_720I,               /* /< 720I */
	VENC_DRV_RESOLUTION_SUPPORT_720P,               /* /< 720P */
	VENC_DRV_RESOLUTION_SUPPORT_1080I,              /* /< 1080I */
	VENC_DRV_RESOLUTION_SUPPORT_1080P,              /* /< 1080P */
	VENC_DRV_RESOLUTION_SUPPORT_2160P,              /* /< 2160P */
	VENC_DRV_RESOLUTION_SUPPORT_MAX = 0xFFFFFFFF    /* /< Max VENC_DRV_RESOLUTION_T value */
} VENC_DRV_RESOLUTION_T;


/**
 * @par Enumeration
 *   VENC_DRV_PRIORITY_ADJUSTMENT_TYPE_T
 * @par Description
 *   This is the item used for encoder resolution capability
 */
typedef enum __VENC_DRV_PRIORITY_ADJUSTMENT_TYPE_T {
	VENC_DRV_PRIORITY_ADJUSTMENT_TYPE_UNKNOWN = 0,       /* /< Default value (not used) */
	VENC_DRV_PRIORITY_ADJUSTMENT_TYPE_ONE,               /* /< 8127 */
	VENC_DRV_PRIORITY_ADJUSTMENT_TYPE_TWO,               /* /< Denali-2 */
	VENC_DRV_PRIORITY_ADJUSTMENT_TYPE_MAX = 0xFFFFFFFF    /* /< Max VENC_DRV_PRIORITY_ADJUSTMENT_TYPE_T value */
} VENC_DRV_PRIORITY_ADJUSTMENT_TYPE_T;

/**
 * @par Enumeration
 *   VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_T
 * @par Description
 *   This is the item used for encoder resolution capability
 */
typedef enum __VENC_DRV_DEFAULT_AVC_PROFILE_TYPE_T {
	VENC_DRV_DEFAULT_AVC_PROFILE_TYPE_UNKNOWN = 0,       /* /< Default value (not used) */
	VENC_DRV_DEFAULT_AVC_PROFILE_TYPE_ONE,               /* /< baseline: D2/70/80/8167*/
	VENC_DRV_DEFAULT_AVC_PROFILE_TYPE_TWO,               /* /< high profile: */
	VENC_DRV_DEFAULT_AVC_PROFILE_TYPE_MAX = 0xFFFFFFFF    /* /< Max VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_T value */
} VENC_DRV_GET_TYPE_DEFAULT_AVC_PROFILE_TYPE_T;

/**
 * @par Enumeration
 *   VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_T
 * @par Description
 *   This is the item used for encoder resolution capability
 */
typedef enum __VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_T {
	VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_UNKNOWN = 0,       /* /< Default value (not used) */
	VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_ONE,               /* /< AVCLevel21: D2/70/80/8167*/
	VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_TWO,               /* /< AVCLevel31: */
    VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_THREE,               /* /< AVCLevel41: */
    VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_FOUR,               /* /< AVCLevel-need-check: */
	VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_MAX = 0xFFFFFFFF    /* /< Max VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_T value */
} VENC_DRV_GET_TYPE_DEFAULT_AVC_LEVEL_TYPE_T;

/**
 * @par Enumeration
 *   VENC_DRV_MDP_PIPELINE_TYPE_T
 * @par Description
 *   This is the item used for encoder resolution capability
 */
typedef enum __VENC_DRV_MDP_PIPELINE_TYPE_T {
	VENC_DRV_MDP_PIPELINE_TYPE_UNKNOWN = 0,       /* /< Default value (not used) */
	VENC_DRV_MDP_PIPELINE_TYPE_ONE,               /* /< 8135 */
	VENC_DRV_MDP_PIPELINE_TYPE_TWO,               /* /< 8173 */
	VENC_DRV_MDP_PIPELINE_TYPE_MAX = 0xFFFFFFFF    /* /< Max VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_T value */
} VENC_DRV_MDP_PIPELINE_TYPE_T;

/**
 * @par Enumeration
 *   VENC_DRV_MULTISLICE_TYPE_T
 * @par Description
 *   This is the item used for encoder resolution capability
 */
typedef enum __VENC_DRV_MULTISLICE_TYPE_T {
	VENC_DRV_MULTISLICE_TYPE_UNKNOWN = 0,       /* /< Default value (not used) */
	VENC_DRV_MULTISLICE_TYPE_ONE,               /* /< D2 */
	VENC_DRV_MULTISLICE_TYPE_TWO,               /* /< 80 */
    VENC_DRV_MULTISLICE_TYPE_THREE,               /* /< 70 */
	VENC_DRV_MULTISLICE_TYPE_MAX = 0xFFFFFFFF    /* /< Max VENC_DRV_DEFAULT_AVC_LEVEL_TYPE_T value */
} VENC_DRV_MULTISLICE_TYPE_T;

/**
 * @par Enumeration
 *  VDEC_DRV_DECODER_TYPE_T
 * @par Description
 *  decoder type
 */
typedef enum __VENC_DRV_ENCODER_TYPE_T {
    VENC_DRV_ENCODER_MTK_HARDWARE = 0,              /* /< MTK software */
    VENC_DRV_ENCODER_MTK_SOFTWARE,                  /* /< MTK hardware */
    VENC_DRV_ENCODER_MTK_HYBRID                     /* /< MTK hybrid   */
}   VENC_DRV_ENCODER_TYPE_T;

/**
 * @par Enumeration
 *   VENC_DRV_SET_TYPE_T
 * @par Description
 *   This is the input parameter for eVEncDrvSetParam()
 */
typedef enum __VENC_DRV_SET_TYPE_T {
	VENC_DRV_SET_TYPE_UNKONW = 0,           /* /< Default value (not used) */
	VENC_DRV_SET_TYPE_RST,                  /* /< Set reset */
	VENC_DRV_SET_TYPE_CB,                   /* /< Set callback function */
	VENC_DRV_SET_TYPE_PARAM_RC,             /* /< Set rate control parameter */
	VENC_DRV_SET_TYPE_PARAM_ME,             /* /< Set motion estimation parameter */
	VENC_DRV_SET_TYPE_PARAM_EIS,            /* /< Set EIS parameter */
	VENC_DRV_SET_TYPE_PARAM_ENC,            /* /< Set encoder parameters such as I-frame period, etc. */
	VENC_DRV_SET_TYPE_STATISTIC_ON,         /* /< Enable statistic function */
	VENC_DRV_SET_TYPE_STATISTIC_OFF,        /* /< Disable statistic function */
	VENC_DRV_SET_TYPE_SET_OMX_TIDS,         /* /< Set OMX thread IDs */
	VENC_DRV_SET_TYPE_MPEG4_SHORT,          /* /< Set MPEG4 short header mode */
	VENC_DRV_SET_TYPE_FORCE_INTRA_ON,       /* /< Set Force Intra Frame on */
	VENC_DRV_SET_TYPE_FORCE_INTRA_OFF,      /* /< Set Force Intra Frame off */
	VENC_DRV_SET_TYPE_TIME_LAPSE,           /* /< Set time lapse */
	VENC_DRV_SET_TYPE_ALLOC_WORK_BUF,       /* /< Set to alloc working buffer */
	VENC_DRV_SET_TYPE_DUMP_WORK_BUF,        /* /< Set to dump working buffer */
	VENC_DRV_SET_TYPE_FREE_WORK_BUF,        /* /< Set to free working buffer */
	VENC_DRV_SET_TYPE_ADJUST_BITRATE,       /* /< Set to adjust bitrate */
	VENC_DRV_SET_TYPE_I_FRAME_INTERVAL,     /* /< Set I Frame interval */
	VENC_DRV_SET_TYPE_WFD_MODE,             /* /< Set Wifi-Display Mode */
	VENC_DRV_SET_TYPE_RECORD_SIZE,          /* /< Ser record size */
	VENC_DRV_SET_TYPE_USE_MCI_BUF,          /* /< Set to use MCI buffer */
	VENC_DRV_SET_TYPE_ADJUST_FRAMERATE,     /* /< Set frame rate */
	VENC_DRV_SET_TYPE_INIT_QP,              /* /< Set init QP */
	VENC_DRV_SET_TYPE_SKIP_FRAME,           /* /< Set skip one frame */
	VENC_DRV_SET_TYPE_SCENARIO,             /* /< Set VENC Scenario */
	VENC_DRV_SET_TYPE_PREPEND_HEADER,       /* /< Set prepend SPS/PPS before IDR */
	VENC_DRV_SET_TYPE_SLOW_MOTION_ENCODE,       /* /< Set to Slow Motion Video Recording for header or frame */
	VENC_DRV_SET_TYPE_SLOW_MOTION_POST_PROC,    /* /< Set to Slow Motion Video Recording for encoded bs with post processing */
	VENC_DRV_SET_TYPE_SLOW_MOTION_LOCK_HW,      /* /< Set to Slow Motion Video Recording for Lock HW */
	VENC_DRV_SET_TYPE_SLOW_MOTION_UNLOCK_HW,    /* /< Set to Slow Motion Video Recording for UnLock HW */
	VENC_DRV_SET_TYPE_NONREFP,              /* /< Set Enable/Disable Non reference P frame */
	VENC_DRV_SET_TYPE_CONFIG_QP,            /* /< Set init QP */
	VENC_DRV_SET_TYPE_RFS_ON,
	VENC_DRV_SET_TYPE_IDR_FRAME_INTERVAL,
	VENC_DRV_SET_TYPE_SET_WAIT_ISR_CB,
	VENC_DRV_SET_TYPE_SET_HW_CTRL_CB,
	VENC_DRV_SET_TYPE_OPERATION_RATE,       /* /< Set Operation Rate*/
	VENC_DRV_SET_TYPE_SET_KERNEL_LOG_COUNT, /* /< Set kernel log count> */
	VENC_DRV_SET_TYPE_DETECTED_FRAMERATE,
	VENC_DRV_SET_SEC_MODE,
	VENC_DRV_SET_TSVC,
	VENC_DRV_SET_TYPE_MAX = 0xFFFFFFFF      /* /< Max VENC_DRV_SET_TYPE_T value */
} VENC_DRV_SET_TYPE_T;


/**
 * @par Enumeration
 *   VENC_DRV_GET_TYPE_T
 * @par Description
 *   This is the input parameter for eVEncDrvGetParam()
 */
typedef enum __VENC_DRV_GET_TYPE_T {
	VENC_DRV_GET_TYPE_UNKONW = 0,               /* /< Default value (not used) */
	VENC_DRV_GET_TYPE_PARAM_RC,                 /* /< Get rate control parameter */
	VENC_DRV_GET_TYPE_PARAM_ME,                 /* /< Get motion estimation parameter */
	VENC_DRV_GET_TYPE_PARAM_EIS,                /* /< Get EIS parameter */
	VENC_DRV_GET_TYPE_PARAM_ENC,                /* /< Get encoder parameters such as I-frame period, etc. */
	VENC_DRV_GET_TYPE_STATISTIC,                /* /< Get statistic. */
	VENC_DRV_GET_TYPE_GET_CPU_LOADING_INFO,     /* /< query the cpu loading info from kernel driver */
	VENC_DRV_GET_TYPE_GET_YUV_FORMAT,           /* /< Get YUV format */
	VENC_DRV_GET_TYPE_GET_CODEC_TIDS,
	/* for DirectLink Meta Mode + */
	VENC_DRV_GET_TYPE_ALLOC_META_HANDLE_LIST,           /* /< Alloc a handle to store meta handle list */
	VENC_DRV_GET_TYPE_GET_BUF_INFO_FROM_META_HANDLE,    /* /< Get buffer virtual address from meta buffer handle */
	VENC_DRV_GET_TYPE_FREE_META_HANDLE_LIST,            /* /< free a handle allocated from VENC_DRV_GET_TYPE_ALLOC_META_HANDLE_LIST */
	/* for DirectLink Meta Mode - */
	/* query driver capability*/
	VENC_DRV_GET_TYPE_QUERY_RESOLUTION_CHANGE_ABILITY,
	VENC_DRV_GET_TYPE_PRIORITY_ADJUSTMENT_TYPE,
	VENC_DRV_GET_TYPE_DEFAULT_AVC_PROFILE_TYPE,
	VENC_DRV_GET_TYPE_DEFAULT_AVC_LEVEL_TYPE,
	VENC_DRV_GET_TYPE_MPEG4_SW_PLATFORM,
	VENC_DRV_GET_TYPE_AVC_HYBRID_PLATFORM,
	VENC_DRV_GET_TYPE_NON_REF_P_FREQUENCY,
	VENC_DRV_GET_TYPE_MDPPIPELINE_ENABLE_TYPE,
	VENC_DRV_GET_TYPE_SW_SOLUTION,
	VENC_DRV_GET_TYPE_RESET_BWC_ENABLE,
	VENC_DRV_GET_TYPE_IS_HEVC_HW_PLATFORM,
	VENC_DRV_GET_TYPE_IS_HEVC_SW_SPECIFIC_PLATFORM,
	VENC_DRV_GET_TYPE_IS_INHOUSE_TEE,
	VENC_DRV_GET_TYPE_MULTISLICE_TYPE,
	VENC_DRV_GET_TYPE_IS_DENALI_PLATFORM,
	VENC_DRV_GET_TYPE_QUERY_ADAPTIVE_DROPFRAME_ABILITY,
	VENC_DRV_GET_TYPE_DONE_BUFFERS,
	VENC_DRV_SET_TYPE_QUERY_ROI_RC_QP,
	VENC_DRV_GET_TYPE_RESOLUTION_CHANGE,
	VENC_DRV_GET_TYPE_MAX = 0xFFFFFFFF          /* /< Max VENC_DRV_GET_TYPE_MAX value */
} VENC_DRV_GET_TYPE_T;


/**
 * @par Enumeration
 *   VENC_DRV_MRESULT_T
 * @par Description
 *   This is the return value for eVEncDrvXXX()
 */
typedef enum __VENC_DRV_MRESULT_T {
	VENC_DRV_MRESULT_OK = 0,                    /* /< Return Success */
	VENC_DRV_MRESULT_FAIL,                      /* /< Return Fail */
	VENC_DRV_MRESULT_MAX = 0x0FFFFFFF           /* /< Max VENC_DRV_MRESULT_T value */
} VENC_DRV_MRESULT_T;


/**
 * @par Enumeration
 *   VENC_DRV_SCENARIO_T
 * @par Description
 *   This is the scenario for VENC scenario
 */
typedef enum __VENC_DRV_SCENARIO_T {
	VENC_DRV_SCENARIO_CAMERA_REC             = 1,        /* /< Camera recording */
	VENC_DRV_SCENARIO_LIVEPHOTO_CAPTURE      = (1 << 1), /* /< LivePhoto recording */
	VENC_DRV_SCENARIO_LIVEPHOTO_EFFECT       = (1 << 2), /* /< LivePhoto effect transcoding */
	VENC_DRV_SCENARIO_CAMERA_REC_SLOW_MOTION = (1 << 3), /* /< Camera recording with slow motion */
	VENC_DRV_SCENARIO_SCREEN_REC             = (1 << 4), /* /< Screen recording */
	VENC_DRV_SCENARIO_VILTE_REC              = (1 << 5), /* /< VILTE recording */
	VENC_DRV_SCENARIO_WECHAT_REC             = (1 << 6), /* /< WeChat recording */
	VENC_DRV_SCENARIO_LBHQ_REC               = (1 << 7), /* /< low bitrate high quality recording */
} VENC_DRV_SCENARIO_T;

/**
 * @par Enumeration
 *  VENC_DRV_INPUTBUF_T
 * @par Description
 *  Description the input buffer property
 */

typedef enum _VENC_DRV_INPUTBUF_T {
	VENC_DRV_INPUT_BUF_CAMERASWITCH  = (1 << 0), /*/ < init data, most case this is video header only*/
} VENC_DRV_INPUTBUF_T;

/**
 * @par Enumeration
 *  VENC_DRV_BITRATE_MODE_T
 * @par Description
 *  Description the bitratemode type; refer to v4l2_mpeg_video_bitrate_mode in v4l2-controls.h
 */
typedef enum _VENC_DRV_BITRATE_MODE_T {
    VENC_DRV_BITRATE_MODE_VBR  = 0, /*/ < variable bitrate mode */
    VENC_DRV_BITRATE_MODE_CBR  = 1, /*/ < constant bitrate mode */
    VENC_DRV_BITRATE_MODE_CQ   = 2, /*/ < constant quality bitrate mode */
} VENC_DRV_BITRATE_MODE_T;

/**
 * @par Structure
 *   VENC_DRV_QUERY_VIDEO_FORMAT_T
 * @par Description
 *   This is a input parameter for eVEncDrvQueryCapability()
 */
typedef struct __VENC_DRV_QUERY_VIDEO_FORMAT_T {
	VENC_DRV_VIDEO_FORMAT_T eVideoFormat;       /* /< [OUT] video format capability */
	VAL_UINT32_T            u4Profile;          /* /< [OUT] video profile capability (VENC_DRV_H264_VIDEO_PROFILE_T, VENC_DRV_MPEG_VIDEO_PROFILE_T, VENC_DRV_MS_VIDEO_PROFILE_T) */
	VENC_DRV_VIDEO_LEVEL_T  eLevel;             /* /< [OUT] video level capability */
	VENC_DRV_RESOLUTION_T   eResolution;        /* /< [OUT] video resolution capability */
	VAL_UINT32_T            u4Width;            /* /< [OUT] video width capability */
	VAL_UINT32_T            u4Height;           /* /< [OUT] video height capability */
	VAL_UINT32_T            u4Bitrate;          /* /< [OUT] video bitrate capability */
	VAL_UINT32_T            u4FrameRate;        /* /< [OUT] video FrameRate capability, 15, 30,... */
	VENC_DRV_ENCODER_TYPE_T eEncoderType;       /* /< [OUT] SW/HW Encoder */
} VENC_DRV_QUERY_VIDEO_FORMAT_T;

/**
 * @par Structure
 *   P_VENC_DRV_QUERY_VIDEO_FORMAT_T
 * @par Description
 *   This is the pointer of VENC_DRV_QUERY_VIDEO_FORMAT_T
 */
typedef VENC_DRV_QUERY_VIDEO_FORMAT_T * P_VENC_DRV_QUERY_VIDEO_FORMAT_T;


/**
 * @par Structure
 *   VENC_DRV_QUERY_INPUT_BUF_LIMIT
 * @par Description
 *   This is a input parameter for eVEncDrvQueryCapability()
 */
typedef struct __VENC_DRV_QUERY_INPUT_BUF_LIMIT {
	VENC_DRV_VIDEO_FORMAT_T eVideoFormat;       /* /< [IN]  video format */
	VAL_UINT32_T            u4Width;            /* /< [IN]  video width */
	VAL_UINT32_T            u4Height;           /* /< [IN]  video height */
	VAL_UINT32_T            u4Stride;           /* /< [OUT] video stride */
	VAL_UINT32_T            u4SliceHeight;      /* /< [OUT] video sliceheight */
	VENC_DRV_SCENARIO_T     eScenario;          /* /< [IN]  venc scenario */
} VENC_DRV_QUERY_INPUT_BUF_LIMIT;


/**
 * @par Structure
 *   VENC_DRV_PARAM_ENC_T
 * @par Description
 *   This is the encoder settings and used as input or output parameter for eVEncDrvSetParam() or eVEncDrvGetParam()
 */
typedef struct __VENC_DRV_PARAM_ENC_T {      /*union extend 64bits for TEE*/
	VENC_DRV_YUV_FORMAT_T   eVEncFormat;        /* /< [IN/OUT] YUV format */
	VAL_UINT32_T            u4Profile;          /* /< [IN/OUT] Profile */
	VAL_UINT32_T            u4Level;            /* /< [IN/OUT] Level */
	VAL_UINT32_T            u4Width;            /* /< [IN/OUT] Image Width */
	VAL_UINT32_T            u4Height;           /* /< [IN/OUT] Image Height */
	VAL_UINT32_T            u4BufWidth;         /* /< [IN/OUT] Buffer Width */
	VAL_UINT32_T            u4BufHeight;        /* /< [IN/OUT] Buffer Heigh */
	VAL_UINT32_T            u4NumPFrm;          /* /< [IN/OUT] The number of P frame between two I frame. */
	VAL_UINT32_T            u4NumBFrm;          /* /< [IN/OUT] The number of B frame between two reference frame. */
	VENC_DRV_FRAME_RATE_T   eFrameRate;         /* /< [IN/OUT] Frame rate */
	VAL_BOOL_T              fgInterlace;        /* /< [IN/OUT] Interlace coding. */
	union {
		VAL_VOID_T          *pvExtraEnc;
		VAL_UINT64_T        pvExtraEnc_ext64;
	};
	VAL_MEMORY_T            rExtraEncMem;       /* /< [IN/OUT] Extra Encoder Memory Info */
	VAL_BOOL_T              fgUseMCI;           /* /< [IN/OUT] Use MCI */
	VAL_BOOL_T              fgMultiSlice;       /* /< [IN/OUT] Is multi-slice bitstream ? */
	VAL_BOOL_T              fgMBAFF;
	VAL_UINT32_T            u4OperationRate;    /* /< [IN/OUT] operation rate */
	VAL_UINT32_T            u4Scenario;         /* /< [IN/OUT] scenario */
	VAL_UINT32_T            u4PrependHeader;    /* /< [IN/OUT] scenario */
	VAL_UINT32_T            u4MaxWith;
	VAL_UINT32_T            u4MaxHeight;
} VENC_DRV_PARAM_ENC_T;

/**
 * @par Structure
 *   P_VENC_DRV_PARAM_ENC_T
 * @par Description
 *   This is the pointer of VENC_DRV_PARAM_ENC_T
 */
typedef VENC_DRV_PARAM_ENC_T * P_VENC_DRV_PARAM_ENC_T;


/**
 * @par Structure
 *   VENC_DRV_PARAM_ENC_EXTRA_T
 * @par Description
 *   This is the encoder settings and used as input or output parameter for eVEncDrvSetParam() or eVEncDrvGetParam()
 */
typedef struct __VENC_DRV_PARAM_ENC_EXTRA_T {
	VAL_UINT32_T            u4IntraFrameRate;   /* /< [IN/OUT] Intra frame rate */
	VAL_UINT32_T            u4BitRate;          /* /< [IN/OUT] BitRate kbps */
	VAL_UINT32_T            u4FrameRateQ16;     /* /< [IN/OUT] Frame rate in Q16 format */
	VAL_UINT32_T            u4UseMBAFF;         /* /< [IN/OUT] Use MBAFF */
	VAL_UINT32_T            u4BitrateMode;
	VAL_UINT32_T            u4RoiOn;
	VAL_UINT32_T            u4HeifGridSize;     /* /< [31:16] grid_w [15:0] grid_h */
} VENC_DRV_PARAM_ENC_EXTRA_T;

/**
 * @par Structure
 *   P_VENC_DRV_PARAM_ENC_EXTRA_T
 * @par Description
 *   This is the pointer of VENC_DRV_PARAM_ENC_EXTRA_T
 */
typedef VENC_DRV_PARAM_ENC_EXTRA_T * pVENC_DRV_PARAM_ENC_EXTRA_T;


#define VENC_DRV_VDO_PROP_LIST_MAX      (64)

/**
 * @par Structure
 *   VENC_DRV_VIDEO_PROPERTY_T
 * @par Description
 *   This is used to get the "target bitrate" according to "resolution and frame rate"
 */
typedef struct __VENC_DRV_VIDEO_PROPERTY_T {
	VENC_DRV_VIDEO_FORMAT_T     eVideoFormat;
	VAL_UINT32_T    u4Width;
	VAL_UINT32_T    u4Height;
	VAL_UINT32_T    u4FrameRate;
	VAL_UINT32_T    u4BitRate;    /* used for query table */
	VAL_BOOL_T      fgPropIsValid;
} VENC_DRV_VIDEO_PROPERTY_T;

/**
 * @par Structure
 *   P_VENC_DRV_VIDEO_PROPERTY_T
 * @par Description
 *   This is the pointer of VENC_DRV_VIDEO_PROPERTY_T
 */
typedef VENC_DRV_VIDEO_PROPERTY_T * P_VENC_DRV_VIDEO_PROPERTY_T;


/**
 * @par Structure
 *   VENC_DRV_TIMESTAMP_T
 * @par Description
 *   This is timestamp information and used as items for VENC_DRV_PARAM_FRM_BUF_T and VENC_DRV_PARAM_BS_BUF_T
 */
typedef struct __VENC_DRV_TIMESTAMP_T {
	VAL_UINT32_T    u4TimeStamp[2];     /* /< [IN] Timestamp information */
} VENC_DRV_TIMESTAMP_T;

/**
 * @par Structure
 *   P_VENC_DRV_TIMESTAMP_T
 * @par Description
 *   This is the pointer of VENC_DRV_TIMESTAMP_T
 */
typedef VENC_DRV_TIMESTAMP_T * P_VENC_DRV_TIMESTAMP_T;


/**
 * @par Structure
 *   VENC_DRV_EIS_INPUT_T
 * @par Description
 *   This is EIS information and used as items for VENC_DRV_PARAM_FRM_BUF_T
 */
typedef struct __VENC_DRV_EIS_INPUT_T {
	VAL_UINT32_T    u4X;    /* /< [IN] Start coordination X */
	VAL_UINT32_T    u4Y;    /* /< [IN] Start coordination Y */
} VENC_DRV_EIS_INPUT_T;

/**
 * @par Structure
 *   P_VENC_DRV_EIS_INPUT_T
 * @par Description
 *   This is the pointer of VENC_DRV_EIS_INPUT_T
 */
typedef VENC_DRV_EIS_INPUT_T * P_VENC_DRV_EIS_INPUT_T;


/**
 * @par Structure
 *   VENC_DRV_PARAM_FRM_BUF_T
 * @par Description
 *   This is frame buffer information and used as input parameter for eVEncDrvEncode()
 */
typedef struct __VENC_DRV_PARAM_FRM_BUF_T {
	VAL_MEM_ADDR_T          rFrmBufAddr;        /* /< [IN] Frame buffer address */
	VAL_MEM_ADDR_T          rCoarseAddr;        /* /< [IN] Coarse address */
	VENC_DRV_TIMESTAMP_T    rTimeStamp;         /* /< [IN] Timestamp information */
	VENC_DRV_EIS_INPUT_T    rEISInput;          /* /< [IN] EIS information */
	VAL_UINT32_T            rSecMemHandle;      /* /< [IN/OUT] security memory handle for SVP */
	VAL_MEM_ADDR_T          rBaseAddrPlanes[3]; /* /< [IN/OUT] Multi planes base address */
	VAL_UINT32_T            u4PlanesNum;        /* /< [IN] Number of planes */
#ifdef __ANDROID__
    union {
        ion_user_handle_t pIonBufhandle;        /* /< [IN/OUT] ion handle */
        VAL_UINT64_T pIonBufhandle_ext64;
    };
#else
    VAL_UINT64_T pIonBufhandle;
#endif
    int index;
    VAL_UINT32_T            u4IonShareFd;
    VAL_UINT32_T            u4IonDevFd;     /* /< [IN/OUT] ion device fd */
    VAL_UINT32_T            u4InputFlag;        /*/ < [IN]     the property of input buffer */
    VAL_MEM_ADDR_T          rRoiBufAddr;        /* /< [IN] Roi buffer address */
} VENC_DRV_PARAM_FRM_BUF_T;

/**
 * @par Structure
 *   P_VENC_DRV_PARAM_FRM_BUF_T
 * @par Description
 *   This is the pointer of VENC_DRV_PARAM_FRM_BUF_T
 */
typedef VENC_DRV_PARAM_FRM_BUF_T * P_VENC_DRV_PARAM_FRM_BUF_T;


/**
 * @par Structure
 *   VENC_DRV_PARAM_BS_BUF_T
 * @par Description
 *   This is bitstream buffer information and used as input parameter for\n
 *   eVEncDrvEncode()\n
 */
typedef struct __VENC_DRV_PARAM_BS_BUF_T {/*union extend 64bits for TEE */
	VAL_MEM_ADDR_T          rBSAddr;        /* /< [IN] Bitstream buffer address */
	union {
		VAL_ULONG_T         u4BSStartVA;    /* /< [IN] Bitstream fill start address */
		VAL_UINT64_T        u4BSStartVA_ext64;
	};
	union {
		VAL_ULONG_T         u4BSSize;       /* /< [IN] Bitstream size (filled bitstream in bytes) */
		VAL_UINT64_T        u4BSSize_ext64;
	};
	VENC_DRV_TIMESTAMP_T    rTimeStamp;     /* /< [IN] Time stamp information */
	VAL_UINT32_T            rSecMemHandle;  /* /< [IN/OUT] security memory handle for SVP */
#ifdef __ANDROID__
    union {
        ion_user_handle_t pIonBufhandle;    /* /< [IN/OUT] ion handle */
        VAL_UINT64_T pIonBufhandle_ext64;
    };
#else
    VAL_UINT64_T pIonBufhandle;
#endif
    int index;
    VAL_UINT32_T            u4IonShareFd;
    VAL_UINT32_T            u4IonDevFd;     /* /< [IN/OUT] ion device fd */
} VENC_DRV_PARAM_BS_BUF_T;

/**
 * @par Structure
 *   P_VENC_DRV_PARAM_BS_BUF_T
 * @par Description
 *   This is the pointer of VENC_DRV_PARAM_BS_BUF_T
 */
typedef VENC_DRV_PARAM_BS_BUF_T * P_VENC_DRV_PARAM_BS_BUF_T;


/**
 * @par Structure
 *   VENC_DRV_DONE_RESULT_T
 * @par Description
 *   This is callback and return information and used as output parameter for eVEncDrvEncode()
 */
typedef struct __VENC_DRV_DONE_RESULT_T {        /*union extend 64bits for TEE */
	VENC_DRV_MESSAGE_T          eMessage;           /* /< [OUT] Message, such as success or error code */
	union {
		P_VENC_DRV_PARAM_BS_BUF_T  prBSBuf;         /* /< [OUT] Bitstream information */
		VAL_UINT64_T               prBSBuf_ext64;
	};
	union {
		P_VENC_DRV_PARAM_FRM_BUF_T prFrmBuf;        /* /< [OUT] Input frame buffer information. if address is null, don't use this buffer, else reuse */
		VAL_UINT64_T               prFrmBuf_ext64;
	};
	VAL_BOOL_T                  fgIsKeyFrm;         /* /< [OUT] output is key frame or not */
	VAL_UINT32_T                u4HWEncodeTime;     /* /< [OUT] HW encode Time */
} VENC_DRV_DONE_RESULT_T;

/**
 * @par Structure
 *   P_VENC_DRV_DONE_RESULT_T
 * @par Description
 *   This is the pointer of VENC_DRV_DONE_RESULT_T
 */
typedef VENC_DRV_DONE_RESULT_T * P_VENC_DRV_DONE_RESULT_T;


/**
 * @par Structure
 *   VENC_DRV_PROPERTY_T
 * @par Description
 *   This is property information and used as output parameter for eVEncDrvQueryCapability()
 */
typedef struct __VENC_DRV_PROPERTY_T {
	VAL_UINT32_T    u4BufAlign;             /* /< [OUT] Buffer alignment requirement */
	VAL_UINT32_T    u4BufUnitSize;          /* /< [OUT] Buffer unit size is N bytes (e.g., 8, 16, or 64 bytes per unit.) */
	VAL_UINT32_T    u4ExtraBufSize;         /* /< [OUT] Extra buffer size in initial stage */
	VAL_BOOL_T      fgOutputRingBuf;        /* /< [OUT] Output is ring buffer */
	VAL_BOOL_T      fgCoarseMESupport;      /* /< [OUT] Support ME coarse search */
	VAL_BOOL_T      fgEISSupport;           /* /< [OUT] Support EIS */
} VENC_DRV_PROPERTY_T;

/**
 * @par Structure
 *   P_VENC_DRV_PROPERTY_T
 * @par Description
 *   This is the pointer of VENC_DRV_PROPERTY_T
 */
typedef VENC_DRV_PROPERTY_T * P_VENC_DRV_PROPERTY_T;

/**
 * @par Structure
 *   VENC_DRV_DONEBUFFER_T
 * @par Description
 *   This is the structure to keep paired frame, bitstream buffer
 */

typedef struct __VENC_DRV_DONEBUFFER_T
{
    union {
        VENC_DRV_PARAM_FRM_BUF_T *pFrameBuf;
        VAL_UINT64_T              pFrameBuf_ex;
    };
    union {
        VENC_DRV_PARAM_BS_BUF_T  *pBsBuf;
        VAL_UINT64_T              pBsBuf_ex;
    };
    VAL_UINT32_T             u4IsKeyFrame;
} VENC_DRV_DONEBUFFER_T, *pVENC_DRV_DONEBuffer_T;


/**
 * @par Structure
 *   VENC_DRV_DONE_LIST_T
 * @par Description
 *   This is the structure to keep done buffer pair,when buffer is done
     these buffer pair is move from internal list to done list
 */

typedef struct __VENC_DRV_DONE_LIST_T
{
#ifdef __ANDROID__
	union {
	    pthread_mutex_t             rlock;
		VAL_UINT64_T                rlock_ex;
	};
#else
	VAL_UINT64_T				rlock;
#endif
    VAL_UINT32_T                u4Count;
    VAL_UINT32_T                u4ReadIndex;
    VAL_UINT32_T                u4WriteIndex;
    VENC_DRV_DONEBUFFER_T       rDoneBuffer[VENC_ENC_QUEUE_SIZE];
} VENC_DRV_DONE_LIST_T, *pVENC_DRV_DONE_LIST_T;


/**
 * @par Structure
 *  SEC_VENC_INIT_CONFIG
 * @par Description
 *  This is the structure for initial Venc TLC
 */
typedef struct sec_venc_init_config {
	int         width;
	int         height;
	void        *pVencHandle;
	uint32_t    uVencHandleLen;
	unsigned char *pRCCode;
} SEC_VENC_INIT_CONFIG;

/**
 * @par Structure
 *  SEC_VENC_INIT_CONFIG
 * @par Description
 *  This is the structure for setting Venc TLC
 */
typedef struct sec_venc_enc_parameter {
	uint32_t bitstreamSecHandle;
	uint32_t uBitstreamBufSize;
	uint32_t uBitstreamDataLen;

	uint32_t frameSecHandle;
	uint32_t uFrameBufSize;
	uint32_t uFrameDataLen;
} SEC_VENC_ENC_PARAM;


/**
 * @par Structure
 *   VENC_DRV_ADAPTIVE_INFO_FORMAT_T
 * @par Description
 *   This is a parameter for getAdaptiveEncodeInfo
 */
typedef struct venc_drv_adaptive_info_format_t {
	VENC_DRV_VIDEO_FORMAT_T eVideoFormat;       /* /< video format */
	VAL_UINT32_T            u4Width;            /* /< video width */
	VAL_UINT32_T            u4Height;           /* /< video height */
	VAL_UINT32_T            u4MaxWidth;         /* /< initial video width */
	VAL_UINT32_T            u4MaxHeight;        /* /< initial video height */
	VAL_UINT32_T            u4Bitrate;          /* /< video bitrate */
	VENC_DRV_FRAME_RATE_T   u4FrameRate;        /* /< video FrameRate, 15, 30,... */
} VENC_DRV_ADAPTIVE_INFO_FORMAT_T;


/*
 * struct venc_resolution_change - This is return information used in venc_if_encode()
 * @width: width resolution change to
 * @height: height resolution change to
 * @resolutionchange : if resolution change
 * NEED to sync with kernel structure
 */
typedef struct venc_resolution_change {
	VAL_UINT32_T width;
	VAL_UINT32_T height;
	VAL_UINT32_T framerate;
	VAL_UINT32_T resolutionchange;
}VENC_RESOLUTION_CHANGE_T;



/**
 * @par Function
 *   eVEncDrvQueryCapability
 * @par Description
 *   Query the driver capability
 * @param
 *   a_eType                [IN/OUT] The VENC_DRV_QUERY_TYPE_T structure
 * @param
 *   a_pvInParam            [IN]     The input parameter
 * @param
 *   a_pvOutParam           [OUT]    The output parameter
 * @par Returns
 *   VENC_DRV_MRESULT_T     [OUT]    VENC_DRV_MRESULT_OK for success, VENC_DRV_MRESULT_FAIL for fail
 */
VENC_DRV_MRESULT_T  eVEncDrvQueryCapability(
	VENC_DRV_QUERY_TYPE_T a_eType,
	VAL_VOID_T *a_pvInParam,
	VAL_VOID_T *a_pvOutParam
);


/**
 * @par Function
 *   eVEncDrvCreate
 * @par Description
 *   Create the driver handle
 * @param
 *   a_phHandle             [OUT] The driver handle
 * @param
 *   a_eVideoFormat         [IN]  The VENC_DRV_VIDEO_FORMAT_T structure
 * @par Returns
 *   VENC_DRV_MRESULT_T     [OUT] VENC_DRV_MRESULT_OK for success, VENC_DRV_MRESULT_FAIL for fail
 */
VENC_DRV_MRESULT_T  eVEncDrvCreate(VAL_HANDLE_T *a_phHandle, VENC_DRV_VIDEO_FORMAT_T a_eVideoFormat
				  );


/**
 * @par Function
 *   eVEncDrvRelease
 * @par Description
 *   Release the driver handle
 * @param
 *   a_hHandle              [IN]  The driver handle
 * @param
 *   a_eVideoFormat         [IN]  The VENC_DRV_VIDEO_FORMAT_T structure
 * @par Returns
 *   VENC_DRV_MRESULT_T     [OUT] VENC_DRV_MRESULT_OK for success, VENC_DRV_MRESULT_FAIL for fail
 */
VENC_DRV_MRESULT_T  eVEncDrvRelease(
	VAL_HANDLE_T a_hHandle,
	VENC_DRV_VIDEO_FORMAT_T a_eVideoFormat
);


/**
 * @par Function
 *   eVEncDrvInit
 * @par Description
 *   Init the driver setting, alloc working memory ... etc.
 * @param
 *   a_hHandle              [IN]  The driver handle
 * @par Returns
 *   VENC_DRV_MRESULT_T     [OUT] VENC_DRV_MRESULT_OK for success, VENC_DRV_MRESULT_FAIL for fail
 */
VENC_DRV_MRESULT_T  eVEncDrvInit(
	VAL_HANDLE_T a_hHandle
);

/**
 * @par Function
 *   eVEncDrvDeInit
 * @par Description
 *   DeInit the driver setting, free working memory ... etc.
 * @param
 *   a_hHandle              [IN]  The driver handle
 * @par Returns
 *   VENC_DRV_MRESULT_T     [OUT] VENC_DRV_MRESULT_OK for success, VENC_DRV_MRESULT_FAIL for fail
 */
VENC_DRV_MRESULT_T  eVEncDrvDeInit(
	VAL_HANDLE_T a_hHandle
);


/**
 * @par Function
 *   eVEncDrvSetParam
 * @par Description
 *   Set parameter to driver
 * @param
 *   a_hHandle              [IN]  The driver handle
 * @param
 *   a_eType                [IN]  The VENC_DRV_SET_TYPE_T structure
 * @param
 *   a_pvInParam            [IN]  The input parameter
 * @param
 *   a_pvOutParam           [OUT] The output parameter
 * @par Returns
 *   VENC_DRV_MRESULT_T     [OUT] VENC_DRV_MRESULT_OK for success, VENC_DRV_MRESULT_FAIL for fail
 */
VENC_DRV_MRESULT_T  eVEncDrvSetParam(
	VAL_HANDLE_T a_hHandle,
	VENC_DRV_SET_TYPE_T a_eType,
	VAL_VOID_T *a_pvInParam,
	VAL_VOID_T *a_pvOutParam
);


/**
 * @par Function
 *   eVEncDrvGetParam
 * @par Description
 *   Get parameter from driver
 * @param
 *   a_hHandle              [IN]  The driver handle
 * @param
 *   a_eType                [IN]  The VENC_DRV_SET_TYPE_T structure
 * @param
 *   a_pvInParam            [IN]  The input parameter
 * @param
 *   a_pvOutParam           [OUT] The output parameter
 * @par Returns
 *   VENC_DRV_MRESULT_T     [OUT] VENC_DRV_MRESULT_OK for success, VENC_DRV_MRESULT_FAIL for fail
 */
VENC_DRV_MRESULT_T  eVEncDrvGetParam(
	VAL_HANDLE_T a_hHandle,
	VENC_DRV_GET_TYPE_T a_eType,
	VAL_VOID_T *a_pvInParam,
	VAL_VOID_T *a_pvOutParam
);


/**
 * @par Function
 *   eVEncDrvEncode
 * @par Description
 *   Encode frame
 * @param
 *   a_hHandle              [IN]  The driver handle
 * @param
 *   a_eOpt                 [IN]  The VENC_DRV_START_OPT_T structure
 * @param
 *   a_prFrmBuf             [IN]  The input frame buffer with VENC_DRV_PARAM_FRM_BUF_T structure
 * @param
 *   a_prBSBuf              [IN]  The input bitstream buffer with VENC_DRV_PARAM_BS_BUF_T structure
 * @param
 *   a_prResult             [OUT] The output result with VENC_DRV_DONE_RESULT_T structure
 * @par Returns
 *   VENC_DRV_MRESULT_T     [OUT] VENC_DRV_MRESULT_OK for success, VENC_DRV_MRESULT_FAIL for fail
 */
VENC_DRV_MRESULT_T  eVEncDrvEncode(VAL_HANDLE_T a_hHandle, VENC_DRV_START_OPT_T a_eOpt, VENC_DRV_PARAM_FRM_BUF_T *a_prFrmBuf, VENC_DRV_PARAM_BS_BUF_T *a_prBSBuf, VENC_DRV_DONE_RESULT_T *a_prResult);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VENC_DRV_IF_PUBLIC_H_ */
