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
#ifndef __DISP_SESSION_H
#define __DISP_SESSION_H
#include <linux/types.h>
#define DISP_SESSION_DEVICE "mtk_disp_mgr"
#define DISP_NO_ION_FD ((int) (~0U >> 1))
#define DISP_NO_USE_LAEYR_ID ((int) (~0U >> 1))
#define OVL_CASCADE_SUPPORT
#ifdef OVL_CASCADE_SUPPORT
#define MAX_INPUT_CONFIG 8
#else
#define MAX_INPUT_CONFIG 4
#endif
#define MAKE_DISP_FORMAT_ID(id,bpp) (((id) << 8) | (bpp))
#define DISP_SESSION_MODE(id) (((id) >> 24) & 0xff)
#define DISP_SESSION_TYPE(id) (((id) >> 16) & 0xff)
#define DISP_SESSION_DEV(id) ((id) & 0xff)
#define MAKE_DISP_SESSION(type,dev) (unsigned int) ((type) << 16 | (dev))
#define RSZ_RES_LIST_NUM 8
typedef enum {
  DISP_IF_TYPE_DBI = 0,
  DISP_IF_TYPE_DPI,
  DISP_IF_TYPE_DSI0,
  DISP_IF_TYPE_DSI1,
  DISP_IF_TYPE_DSIDUAL,
  DISP_IF_HDMI = 7,
  DISP_IF_HDMI_SMARTBOOK,
  DISP_IF_MHL,
  DISP_IF_EPD,
  DISP_IF_SLIMPORT
} DISP_IF_TYPE;
typedef enum {
  DISP_IF_FORMAT_RGB565 = 0,
  DISP_IF_FORMAT_RGB666,
  DISP_IF_FORMAT_RGB888
} DISP_IF_FORMAT;
typedef enum {
  DISP_IF_MODE_VIDEO = 0,
  DISP_IF_MODE_COMMAND
} DISP_IF_MODE;
typedef enum {
  DISP_ORIENTATION_0 = 0,
  DISP_ORIENTATION_90 = 1,
  DISP_ORIENTATION_180 = 2,
  DISP_ORIENTATION_270 = 3,
} DISP_ORIENTATION;
typedef enum {
  DISP_FORMAT_UNKNOWN = 0,
  DISP_FORMAT_RGB565 = MAKE_DISP_FORMAT_ID(1, 2),
  DISP_FORMAT_RGB888 = MAKE_DISP_FORMAT_ID(2, 3),
  DISP_FORMAT_BGR888 = MAKE_DISP_FORMAT_ID(3, 3),
  DISP_FORMAT_ARGB8888 = MAKE_DISP_FORMAT_ID(4, 4),
  DISP_FORMAT_ABGR8888 = MAKE_DISP_FORMAT_ID(5, 4),
  DISP_FORMAT_RGBA8888 = MAKE_DISP_FORMAT_ID(6, 4),
  DISP_FORMAT_BGRA8888 = MAKE_DISP_FORMAT_ID(7, 4),
  DISP_FORMAT_YUV422 = MAKE_DISP_FORMAT_ID(8, 2),
  DISP_FORMAT_XRGB8888 = MAKE_DISP_FORMAT_ID(9, 4),
  DISP_FORMAT_XBGR8888 = MAKE_DISP_FORMAT_ID(10, 4),
  DISP_FORMAT_RGBX8888 = MAKE_DISP_FORMAT_ID(11, 4),
  DISP_FORMAT_BGRX8888 = MAKE_DISP_FORMAT_ID(12, 4),
  DISP_FORMAT_UYVY = MAKE_DISP_FORMAT_ID(13, 2),
  DISP_FORMAT_YUV420_P = MAKE_DISP_FORMAT_ID(14, 2),
  DISP_FORMAT_YV12 = MAKE_DISP_FORMAT_ID(16, 1),
  DISP_FORMAT_PARGB8888 = MAKE_DISP_FORMAT_ID(17, 4),
  DISP_FORMAT_PABGR8888 = MAKE_DISP_FORMAT_ID(18, 4),
  DISP_FORMAT_PRGBA8888 = MAKE_DISP_FORMAT_ID(19, 4),
  DISP_FORMAT_PBGRA8888 = MAKE_DISP_FORMAT_ID(20, 4),
  DISP_FORMAT_DIM = MAKE_DISP_FORMAT_ID(21, 0),
  DISP_FORMAT_RGBA1010102 = MAKE_DISP_FORMAT_ID(22, 4),
  DISP_FORMAT_PRGBA1010102 = MAKE_DISP_FORMAT_ID(23, 4),
  DISP_FORMAT_RGBA_FP16 = MAKE_DISP_FORMAT_ID(24, 8),
  DISP_FORMAT_PRGBA_FP16 = MAKE_DISP_FORMAT_ID(25, 8),
  DISP_FORMAT_NUM = MAKE_DISP_FORMAT_ID(26, 0),
  DISP_FORMAT_BPP_MASK = 0xFF,
} DISP_FORMAT;
typedef enum {
  DISP_LAYER_2D = 0,
  DISP_LAYER_3D_SBS_0 = 0x1,
  DISP_LAYER_3D_SBS_90 = 0x2,
  DISP_LAYER_3D_SBS_180 = 0x3,
  DISP_LAYER_3D_SBS_270 = 0x4,
  DISP_LAYER_3D_TAB_0 = 0x10,
  DISP_LAYER_3D_TAB_90 = 0x20,
  DISP_LAYER_3D_TAB_180 = 0x30,
  DISP_LAYER_3D_TAB_270 = 0x40,
} DISP_LAYER_TYPE;
typedef enum {
  DISP_NORMAL_BUFFER = 0,
  DISP_PROTECT_BUFFER = 1,
  DISP_SECURE_BUFFER = 2,
  DISP_SECURE_BUFFER_SHIFT = 0x10002
} DISP_BUFFER_TYPE;
typedef enum {
  DISP_BUFFER_ION = 0,
  DISP_BUFFER_ALPHA = 1,
  DISP_BUFFER_MVA = 2,
} DISP_BUFFER_SOURCE;
typedef enum {
  DISP_ALPHA_ONE = 0,
  DISP_ALPHA_SRC = 1,
  DISP_ALPHA_SRC_INVERT = 2,
  DISP_ALPHA_INVALID = 3,
} DISP_ALPHA_TYPE;
typedef enum {
  DISP_SESSION_PRIMARY = 1,
  DISP_SESSION_EXTERNAL = 2,
  DISP_SESSION_MEMORY = 3
} DISP_SESSION_TYPE;
typedef enum {
  DISP_YUV_BT601_FULL = 0,
  DISP_YUV_BT601 = 1,
  DISP_YUV_BT709 = 2
} DISP_YUV_RANGE_ENUM;
typedef enum {
  DISP_INVALID_SESSION_MODE = 0,
  DISP_SESSION_DIRECT_LINK_MODE = 1,
  DISP_SESSION_DECOUPLE_MODE = 2,
  DISP_SESSION_DIRECT_LINK_MIRROR_MODE = 3,
  DISP_SESSION_DECOUPLE_MIRROR_MODE = 4,
  DISP_SESSION_RDMA_MODE,
  DISP_SESSION_DUAL_DIRECT_LINK_MODE,
  DISP_SESSION_DUAL_DECOUPLE_MODE,
  DISP_SESSION_DUAL_RDMA_MODE,
  DISP_SESSION_TRIPLE_DIRECT_LINK_MODE,
  DISP_SESSION_MODE_NUM,
} DISP_MODE;
typedef enum {
  SESSION_USER_INVALID = - 1,
  SESSION_USER_HWC = 0,
  SESSION_USER_GUIEXT = 1,
  SESSION_USER_AEE = 2,
  SESSION_USER_PANDISP = 3,
  SESSION_USER_CNT,
} DISP_SESSION_USER;
typedef enum {
  DISP_OUTPUT_UNKNOWN = 0,
  DISP_OUTPUT_MEMORY = 1,
  DISP_OUTPUT_DECOUPLE = 2,
} DISP_DC_TYPE;
typedef enum {
  TRIGGER_NORMAL,
  TRIGGER_SUSPEND,
  TRIGGER_RESUME,
  TRIGGER_MODE_MAX_NUM
} EXTD_TRIGGER_MODE;
typedef struct disp_session_config_t {
  DISP_SESSION_TYPE type;
  unsigned int device_id;
  DISP_MODE mode;
  unsigned int session_id;
  DISP_SESSION_USER user;
  unsigned int present_fence_idx;
  DISP_DC_TYPE dc_type;
  int need_merge;
  EXTD_TRIGGER_MODE tigger_mode;
} disp_session_config;
typedef struct {
  unsigned int session_id;
  unsigned int vsync_cnt;
  unsigned long long vsync_ts;
  int lcm_fps;
} disp_session_vsync_config;
struct layer_dirty_roi {
  __u16 dirty_x;
  __u16 dirty_y;
  __u16 dirty_w;
  __u16 dirty_h;
};
typedef struct disp_input_config_t {
  void * src_base_addr;
  void * src_phy_addr;
  DISP_BUFFER_SOURCE buffer_source;
  DISP_BUFFER_TYPE security;
  DISP_FORMAT src_fmt;
  DISP_ALPHA_TYPE src_alpha;
  DISP_ALPHA_TYPE dst_alpha;
  DISP_YUV_RANGE_ENUM yuv_range;
  int dataspace;
  DISP_ORIENTATION layer_rotation;
  DISP_LAYER_TYPE layer_type;
  DISP_ORIENTATION video_rotation;
  __u32 next_buff_idx;
  int src_fence_fd;
  void * src_fence_struct;
  __u32 src_color_key;
  __u32 frm_sequence;
  __u32 dim_color;
  void * dirty_roi_addr;
  __u16 dirty_roi_num;
  __u16 src_v_pitch;
  __u16 src_pitch;
  __u16 src_offset_x, src_offset_y;
  __u16 src_width, src_height;
  __u16 tgt_offset_x, tgt_offset_y;
  __u16 tgt_width, tgt_height;
  __u8 alpha_enable;
  __u8 alpha;
  __u8 sur_aen;
  __u8 src_use_color_key;
  __u8 layer_id;
  __u8 layer_enable;
  __u8 src_direct_link;
  __u8 isTdshp;
  __u8 identity;
  __u8 connected_type;
  __s8 ext_sel_layer;
  __u8 compress;
} disp_input_config;
typedef struct disp_output_config_t {
  void * va;
  void * pa;
  DISP_FORMAT fmt;
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
  unsigned int pitch;
  unsigned int pitchUV;
  DISP_BUFFER_TYPE security;
  unsigned int buff_idx;
  unsigned int interface_idx;
  int src_fence_fd;
  void * src_fence_struct;
  unsigned int frm_sequence;
} disp_output_config;
struct disp_ccorr_config {
  bool is_dirty;
  int mode;
  int color_matrix[16];
};
typedef struct disp_session_input_config_t {
  DISP_SESSION_USER setter;
  unsigned int session_id;
  unsigned int config_layer_num;
  disp_input_config config[12];
  struct disp_ccorr_config ccorr_config;
} disp_session_input_config;
typedef struct disp_session_output_config_t {
  unsigned int session_id;
  disp_output_config config;
} disp_session_output_config;
typedef struct disp_session_layer_num_config_t {
  unsigned int session_id;
  unsigned int max_layer_num;
} disp_session_layer_num_config;
struct disp_frame_cfg_t {
  DISP_SESSION_USER setter;
  unsigned int session_id;
  unsigned int input_layer_num;
  disp_input_config input_cfg[12];
  unsigned int overlap_layer_num;
  unsigned int const_layer_num;
  disp_input_config const_layer[1];
  int output_en;
  disp_output_config output_cfg;
  DISP_MODE mode;
  unsigned int present_fence_idx;
  int prev_present_fence_fd;
  void * prev_present_fence_struct;
  EXTD_TRIGGER_MODE tigger_mode;
  DISP_SESSION_USER user;
  struct disp_ccorr_config ccorr_config;
  int res_idx;
  unsigned int hrt_weight;
  unsigned int hrt_idx;
};
typedef struct disp_session_info_t {
  unsigned int session_id;
  unsigned int maxLayerNum;
  unsigned int isHwVsyncAvailable;
  DISP_IF_TYPE displayType;
  unsigned int displayWidth;
  unsigned int displayHeight;
  unsigned int displayFormat;
  DISP_IF_MODE displayMode;
  unsigned int vsyncFPS;
  unsigned int physicalWidth;
  unsigned int physicalHeight;
  unsigned int physicalWidthUm;
  unsigned int physicalHeightUm;
  unsigned int density;
  unsigned int isConnected;
  unsigned int isHDCPSupported;
  unsigned int isOVLDisabled;
  unsigned int is3DSupport;
  unsigned int const_layer_num;
  unsigned int updateFPS;
  unsigned int is_updateFPS_stable;
} disp_session_info;
typedef struct disp_buffer_info_t {
  unsigned int session_id;
  unsigned int layer_id;
  unsigned int layer_en;
  int ion_fd;
  unsigned int cache_sync;
  unsigned int index;
  int fence_fd;
  unsigned int interface_index;
  int interface_fence_fd;
} disp_buffer_info;
typedef struct disp_present_fence_info_t {
  unsigned int session_id;
  int present_fence_fd;
  unsigned int present_fence_index;
} disp_present_fence;
typedef struct disp_present_fence_t {
  unsigned int session_id;
  unsigned int index;
  int fence_fd;
} disp_present_fence_info;
typedef enum {
  DISP_OUTPUT_CAP_DIRECT_LINK = 0,
  DISP_OUTPUT_CAP_DECOUPLE,
  DISP_OUTPUT_CAP_SWITCHABLE,
} DISP_CAP_OUTPUT_MODE;
typedef enum {
  DISP_OUTPUT_CAP_SINGLE_PASS = 0,
  DISP_OUTPUT_CAP_MULTI_PASS,
} DISP_CAP_OUTPUT_PASS;
typedef enum {
  DISP_FEATURE_TIME_SHARING = 0x00000001,
  DISP_FEATURE_HRT = 0x00000002,
  DISP_FEATURE_PARTIAL = 0x00000004,
  DISP_FEATURE_FENCE_WAIT = 0x00000008,
  DISP_FEATURE_RSZ = 0x00000010,
  DISP_FEATURE_NO_PARGB = 0x00000020,
  DISP_FEATURE_DISP_SELF_REFRESH = 0x00000040,
  DISP_FEATURE_RPO = 0x00000080,
  DISP_FEATURE_FBDC = 0x00000100,
  DISP_FEATURE_FORCE_DISABLE_AOD = 0x00000200,
} DISP_FEATURE;
typedef struct disp_caps_t {
  DISP_CAP_OUTPUT_MODE output_mode;
  DISP_CAP_OUTPUT_PASS output_pass;
  unsigned int max_layer_num;
  unsigned int disp_feature;
  int is_support_frame_cfg_ioctl;
  int is_output_rotated;
  int lcm_degree;
  unsigned int rsz_in_res_list[RSZ_RES_LIST_NUM][3];
  unsigned int rsz_list_length;
  unsigned int rsz_in_max[2];
  int is_support_three_session;
  int lcm_color_mode;
  unsigned int max_luminance;
  unsigned int average_luminance;
  unsigned int min_luminance;
} disp_caps_info;
typedef struct disp_session_buf_t {
  unsigned int session_id;
  unsigned int buf_hnd[3];
} disp_session_buf_info;
enum LAYERING_CAPS {
  LAYERING_OVL_ONLY = 0x00000001,
  MDP_RSZ_LAYER = 0x00000002,
  DISP_RSZ_LAYER = 0x00000004,
  MDP_ROT_LAYER = 0x00000008,
  MDP_HDR_LAYER = 0x00000010,
  NO_FBDC       = 0x00000020,
};
typedef struct layer_config_t {
  unsigned int ovl_id;
  DISP_FORMAT src_fmt;
  int dataspace;
  unsigned int dst_offset_x, dst_offset_y;
  unsigned int dst_width, dst_height;
  int ext_sel_layer;
  unsigned int src_offset_x, src_offset_y;
  unsigned int src_width, src_height;
  unsigned int layer_caps;
  unsigned int clip; /* drv internal use */
  __u8 compress;
} layer_config;
typedef struct disp_layer_info_t {
  layer_config * input_config[2];
  int disp_mode[2];
  int layer_num[2];
  int gles_head[2];
  int gles_tail[2];
  int hrt_num;
  int res_idx;
  unsigned int hrt_weight;
  unsigned int hrt_idx;
} disp_layer_info;
enum DISP_SCENARIO {
  DISP_SCENARIO_NORMAL,
  DISP_SCENARIO_SELF_REFRESH,
  DISP_SCENARIO_FORCE_DC,
  DISP_SCENARIO_NUM,
};
struct disp_scenario_config_t {
  unsigned int session_id;
  unsigned int scenario;
};
enum DISP_UT_ERROR {
  DISP_UT_ERROR_OVL = 0x00000001,
  DISP_UT_ERROR_WDMA = 0x00000002,
  DISP_UT_ERROR_RDMA = 0x00000004,
  DISP_UT_ERROR_CMDQ_TIMEOUT = 0x00000008,
};
enum DISP_SELF_REFRESH_TYPE {
  WAIT_FOR_REFRESH,
  REFRESH_FOR_ANTI_LATENCY2,
  REFRESH_FOR_SWITCH_DECOUPLE,
  REFRESH_FOR_SWITCH_DECOUPLE_MIRROR,
  REFRESH_FOR_IDLE,
  REFRESH_TYPE_NUM,
};
#define DISP_IOW(num,dtype) _IOW('O', num, dtype)
#define DISP_IOR(num,dtype) _IOR('O', num, dtype)
#define DISP_IOWR(num,dtype) _IOWR('O', num, dtype)
#define DISP_IO(num) _IO('O', num)
#define DISP_IOCTL_CREATE_SESSION DISP_IOW(201, disp_session_config)
#define DISP_IOCTL_DESTROY_SESSION DISP_IOW(202, disp_session_config)
#define DISP_IOCTL_TRIGGER_SESSION DISP_IOW(203, disp_session_config)
#define DISP_IOCTL_PREPARE_INPUT_BUFFER DISP_IOW(204, disp_buffer_info)
#define DISP_IOCTL_PREPARE_OUTPUT_BUFFER DISP_IOW(205, disp_buffer_info)
#define DISP_IOCTL_SET_INPUT_BUFFER DISP_IOW(206, disp_session_input_config)
#define DISP_IOCTL_SET_OUTPUT_BUFFER DISP_IOW(207, disp_session_output_config)
#define DISP_IOCTL_GET_SESSION_INFO DISP_IOW(208, disp_session_info)
#define DISP_IOCTL_SET_SESSION_MODE DISP_IOW(209, disp_session_config)
#define DISP_IOCTL_GET_SESSION_MODE DISP_IOW(210, disp_session_config)
#define DISP_IOCTL_SET_SESSION_TYPE DISP_IOW(211, disp_session_config)
#define DISP_IOCTL_GET_SESSION_TYPE DISP_IOW(212, disp_session_config)
#define DISP_IOCTL_WAIT_FOR_VSYNC DISP_IOW(213, disp_session_vsync_config)
#define DISP_IOCTL_SET_MAX_LAYER_NUM DISP_IOW(214, disp_session_layer_num_config)
#define DISP_IOCTL_GET_VSYNC_FPS DISP_IOW(215, unsigned int)
#define DISP_IOCTL_SET_VSYNC_FPS DISP_IOW(216, unsigned int)
#define DISP_IOCTL_GET_PRESENT_FENCE DISP_IOW(217, disp_present_fence)
#define DISP_IOCTL_GET_IS_DRIVER_SUSPEND DISP_IOW(218, unsigned int)
#define DISP_IOCTL_GET_DISPLAY_CAPS DISP_IOW(219, disp_caps_info)
#define DISP_IOCTL_INSERT_SESSION_BUFFERS DISP_IOW(220, disp_session_buf_info)
#define DISP_IOCTL_FRAME_CONFIG DISP_IOW(221, disp_session_output_config)
#define DISP_IOCTL_QUERY_VALID_LAYER DISP_IOW(222, disp_layer_info)
#define DISP_IOCTL_SET_SCENARIO DISP_IOW(223, struct disp_scenario_config_t)
#define DISP_IOCTL_WAIT_ALL_JOBS_DONE DISP_IOW(224, unsigned int)
#define DISP_IOCTL_SCREEN_FREEZE DISP_IOW(225, unsigned int)
#define DISP_IOCTL_GET_UT_RESULT DISP_IOW(226, unsigned int)
#define DISP_IOCTL_WAIT_DISP_SELF_REFRESH DISP_IOW(227, unsigned int)
#endif
