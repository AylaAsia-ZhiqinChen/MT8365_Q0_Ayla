/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef __MTK_CONTROLS_H
#define __MTK_CONTROLS_H

#include <linux/videodev2.h>
#include <linux/v4l2-controls.h>

#define V4L2_CID_MPEG_MTK_BASE (V4L2_CTRL_CLASS_MPEG | 0x2000)
#define V4L2_CID_MPEG_MTK_FRAME_INTERVAL (V4L2_CID_MPEG_MTK_BASE + 0)
#define V4L2_CID_MPEG_MTK_ERRORMB_MAP (V4L2_CID_MPEG_MTK_BASE + 1)
#define V4L2_CID_MPEG_MTK_DECODE_MODE (V4L2_CID_MPEG_MTK_BASE + 2)
#define V4L2_CID_MPEG_MTK_FRAME_SIZE (V4L2_CID_MPEG_MTK_BASE + 3)
#define V4L2_CID_MPEG_MTK_FIXED_MAX_FRAME_BUFFER (V4L2_CID_MPEG_MTK_BASE + 4)
#define V4L2_CID_MPEG_MTK_CRC_PATH (V4L2_CID_MPEG_MTK_BASE + 5)
#define V4L2_CID_MPEG_MTK_GOLDEN_PATH (V4L2_CID_MPEG_MTK_BASE + 6)
#define V4L2_CID_MPEG_MTK_COLOR_DESC (V4L2_CID_MPEG_MTK_BASE + 7)
#define V4L2_CID_MPEG_MTK_ASPECT_RATIO (V4L2_CID_MPEG_MTK_BASE + 8)
#define V4L2_CID_MPEG_MTK_SET_WAIT_KEY_FRAME (V4L2_CID_MPEG_MTK_BASE + 9)
#define V4L2_CID_MPEG_MTK_SET_NAL_SIZE_LENGTH (V4L2_CID_MPEG_MTK_BASE + 10)
#define V4L2_CID_MPEG_MTK_SEC_DECODE (V4L2_CID_MPEG_MTK_BASE + 11)
#define V4L2_CID_MPEG_MTK_FIX_BUFFERS (V4L2_CID_MPEG_MTK_BASE + 12)
#define V4L2_CID_MPEG_MTK_FIX_BUFFERS_SVP (V4L2_CID_MPEG_MTK_BASE + 13)
#define V4L2_CID_MPEG_MTK_INTERLACING (V4L2_CID_MPEG_MTK_BASE + 14)
#define V4L2_CID_MPEG_MTK_CODEC_TYPE (V4L2_CID_MPEG_MTK_BASE + 15)
#define V4L2_CID_MPEG_MTK_OPERATING_RATE (V4L2_CID_MPEG_MTK_BASE + 16)
#define V4L2_CID_MPEG_MTK_QUEUED_FRAMEBUF_COUNT (V4L2_CID_MPEG_MTK_BASE + 17)
#define V4L2_CID_MPEG_MTK_SEC_ENCODE  (V4L2_CID_MPEG_MTK_BASE+18)
#define V4L2_CID_MPEG_VIDEO_ENABLE_TSVC  (V4L2_CID_MPEG_BASE+230)
#define V4L2_CID_MPEG_VIDEO_H264_SPS (V4L2_CID_MPEG_BASE + 383)
#define V4L2_CID_MPEG_VIDEO_H264_PPS (V4L2_CID_MPEG_BASE + 384)
#define V4L2_CID_MPEG_VIDEO_H264_SCALING_MATRIX (V4L2_CID_MPEG_BASE + 385)
#define V4L2_CID_MPEG_VIDEO_H264_SLICE_PARAM (V4L2_CID_MPEG_BASE + 386)
#define V4L2_CID_MPEG_VIDEO_H264_DECODE_PARAM (V4L2_CID_MPEG_BASE + 387)

#define V4L2_CID_MPEG_VIDEO_VP8_FRAME_HDR (V4L2_CID_MPEG_BASE + 512)
#define V4L2_CID_MPEG_VIDEO_H265_PROFILE (V4L2_CID_MPEG_BASE + 513)
enum v4l2_mpeg_video_h265_profile {
  V4L2_MPEG_VIDEO_H265_PROFILE_MAIN = 0,
  V4L2_MPEG_VIDEO_H265_PROFILE_MAIN10 = 1,
  V4L2_MPEG_VIDEO_H265_PROFILE_MAIN_HDR10 = 2,
  V4L2_MPEG_VIDEO_H265_PROFILE_MAIN_STILL_PIC = 3
};
#define V4L2_CID_MPEG_VIDEO_H265_TIER_LEVEL (V4L2_CID_MPEG_BASE + 514)
enum v4l2_mpeg_video_h265_level {
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_1 = 0,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_1 = 1,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_2 = 2,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_2 = 3,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_2_1 = 4,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_2_1 = 5,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_3 = 6,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_3 = 7,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_3_1 = 8,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_3_1 = 9,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_4 = 10,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_4 = 11,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_4_1 = 12,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_4_1 = 13,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_5 = 14,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_5 = 15,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_5_1 = 16,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_5_1 = 17,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_5_2 = 18,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_5_2 = 19,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_6 = 20,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_6 = 21,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_6_1 = 22,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_6_1 = 23,
  V4L2_MPEG_VIDEO_H265_LEVEL_MAIN_TIER_LEVEL_6_2 = 24,
  V4L2_MPEG_VIDEO_H265_LEVEL_HIGH_TIER_LEVEL_6_2 = 25,
};
#define V4L2_CID_MPEG_VIDEO_PREPEND_SPSPPS_TO_IDR (V4L2_CID_MPEG_BASE + 644)

#define V4L2_CID_MPEG_MTK_BASE (V4L2_CTRL_CLASS_MPEG | 0x2000)
#define V4L2_CID_MPEG_MTK_FRAME_INTERVAL (V4L2_CID_MPEG_MTK_BASE + 0)
#define V4L2_CID_MPEG_MTK_ERRORMB_MAP (V4L2_CID_MPEG_MTK_BASE + 1)
#define V4L2_CID_MPEG_MTK_DECODE_MODE (V4L2_CID_MPEG_MTK_BASE + 2)
#define V4L2_CID_MPEG_MTK_FRAME_SIZE (V4L2_CID_MPEG_MTK_BASE + 3)
#define V4L2_CID_MPEG_MTK_FIXED_MAX_FRAME_BUFFER (V4L2_CID_MPEG_MTK_BASE + 4)
#define V4L2_CID_MPEG_MTK_UFO_MODE (V4L2_CID_MPEG_MTK_BASE + 5)
#define V4L2_CID_MPEG_MTK_ENCODE_SCENARIO (V4L2_CID_MPEG_MTK_BASE + 6)
#define V4L2_CID_MPEG_MTK_ENCODE_NONREFP (V4L2_CID_MPEG_MTK_BASE + 7)
#define V4L2_CID_MPEG_MTK_ENCODE_DETECTED_FRAMERATE (V4L2_CID_MPEG_MTK_BASE + 8)
#define V4L2_CID_MPEG_MTK_ENCODE_RFS_ON (V4L2_CID_MPEG_MTK_BASE + 9)
#define V4L2_CID_MPEG_MTK_ENCODE_OPERATION_RATE (V4L2_CID_MPEG_MTK_BASE + 10)
#define V4L2_CID_MPEG_MTK_SEC_DECODE (V4L2_CID_MPEG_MTK_BASE + 11)
#define V4L2_CID_MPEG_MTK_ENCODE_ROI_RC_QP (V4L2_CID_MPEG_MTK_BASE+12)
#define V4L2_CID_MPEG_MTK_ENCODE_ROI_ON (V4L2_CID_MPEG_MTK_BASE+13)
#define V4L2_CID_MPEG_MTK_ENCODE_GRID_SIZE (V4L2_CID_MPEG_MTK_BASE+14)
#define V4L2_CID_MPEG_MTK_RESOLUTION_CHANGE (V4L2_CID_MPEG_MTK_BASE+15)
#define V4L2_CID_MPEG_MTK_MAX_WIDTH (V4L2_CID_MPEG_MTK_BASE+16)
#define V4L2_CID_MPEG_MTK_MAX_HEIGHT (V4L2_CID_MPEG_MTK_BASE+17)

enum {
	V4L2_MPEG_VIDEO_BITRATE_MODE_CQ = V4L2_MPEG_VIDEO_BITRATE_MODE_CBR + 1,
};

#endif
