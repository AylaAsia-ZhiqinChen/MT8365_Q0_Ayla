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
#ifndef __CMDQ_ENGINE_COMMON_H__
#define __CMDQ_ENGINE_COMMON_H__
typedef enum CMDQ_ENG_ENUM {
  CMDQ_ENG_WPEI = 0,
  CMDQ_ENG_WPEO,
  CMDQ_ENG_ISP_IMGI,
  CMDQ_ENG_ISP_IMGO,
  CMDQ_ENG_ISP_IMG2O,
  CMDQ_ENG_IPUI,
  CMDQ_ENG_IPUO,
  CMDQ_ENG_MDP_CAMIN,
  CMDQ_ENG_MDP_RDMA0,
  CMDQ_ENG_MDP_RDMA1,
  CMDQ_ENG_MDP_IPUIN,
  CMDQ_ENG_MDP_RSZ0,
  CMDQ_ENG_MDP_RSZ1,
  CMDQ_ENG_MDP_RSZ2,
  CMDQ_ENG_MDP_TDSHP0,
  CMDQ_ENG_MDP_TDSHP1,
  CMDQ_ENG_MDP_COLOR0,
  CMDQ_ENG_MDP_MOUT0,
  CMDQ_ENG_MDP_MOUT1,
  CMDQ_ENG_MDP_PATH0_SOUT,
  CMDQ_ENG_MDP_PATH1_SOUT,
  CMDQ_ENG_MDP_WROT0,
  CMDQ_ENG_MDP_WROT1,
  CMDQ_ENG_MDP_WDMA,
  CMDQ_ENG_JPEG_ENC,
  CMDQ_ENG_VIDEO_ENC,
  CMDQ_ENG_JPEG_DEC,
  CMDQ_ENG_JPEG_REMDC,
  CMDQ_ENG_DISP_UFOE,
  CMDQ_ENG_DISP_AAL,
  CMDQ_ENG_DISP_COLOR0,
  CMDQ_ENG_DISP_COLOR1,
  CMDQ_ENG_DISP_RDMA0,
  CMDQ_ENG_DISP_RDMA1,
  CMDQ_ENG_DISP_RDMA2,
  CMDQ_ENG_DISP_WDMA0,
  CMDQ_ENG_DISP_WDMA1,
  CMDQ_ENG_DISP_OVL0,
  CMDQ_ENG_DISP_OVL1,
  CMDQ_ENG_DISP_OVL2,
  CMDQ_ENG_DISP_2L_OVL0,
  CMDQ_ENG_DISP_2L_OVL1,
  CMDQ_ENG_DISP_2L_OVL2,
  CMDQ_ENG_DISP_GAMMA,
  CMDQ_ENG_DISP_MERGE,
  CMDQ_ENG_DISP_SPLIT0,
  CMDQ_ENG_DISP_SPLIT1,
  CMDQ_ENG_DISP_DSI0_VDO,
  CMDQ_ENG_DISP_DSI1_VDO,
  CMDQ_ENG_DISP_DSI0_CMD,
  CMDQ_ENG_DISP_DSI1_CMD,
  CMDQ_ENG_DISP_DSI0,
  CMDQ_ENG_DISP_DSI1,
  CMDQ_ENG_DISP_DPI,
  CMDQ_ENG_DPE,
  CMDQ_ENG_CMDQ,
  CMDQ_ENG_DISP_MUTEX,
  CMDQ_ENG_MMSYS_CONFIG,
  CMDQ_MAX_ENGINE_COUNT
} CMDQ_ENG_ENUM;
#endif
