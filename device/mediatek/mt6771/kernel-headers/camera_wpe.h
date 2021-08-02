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
#ifndef _MT_WPE_H
#define _MT_WPE_H
#include <linux/ioctl.h>
#define KERNEL_LOG
#define _SUPPORT_MAX_WPE_FRAME_REQUEST_ 32
#define _SUPPORT_MAX_WPE_REQUEST_RING_SIZE_ 32
#define SIG_ERESTARTSYS 512
#define WPE_DEV_MAJOR_NUMBER 251
#define WPE_MAGIC 'w'
#define WPE_REG_RANGE (0x1000)
#define WPE_BASE_HW 0x1502a000
#define WPE_B_BASE_HW 0x1502d000
#define WPE_INT_ST ((unsigned int) 1 << 0)
struct WPE_REG_STRUCT {
  unsigned int module;
  unsigned int Addr;
  unsigned int Val;
};
struct WPE_REG_IO_STRUCT {
  struct WPE_REG_STRUCT * pData;
  unsigned int Count;
};
enum WPE_IRQ_CLEAR_ENUM {
  WPE_IRQ_CLEAR_NONE,
  WPE_IRQ_CLEAR_WAIT,
  WPE_IRQ_WAIT_CLEAR,
  WPE_IRQ_CLEAR_STATUS,
  WPE_IRQ_CLEAR_ALL
};
enum WPE_IRQ_TYPE_ENUM {
  WPE_IRQ_TYPE_INT_WPE_ST,
  WPE_IRQ_TYPE_INT_WPEB_ST,
  WPE_IRQ_TYPE_AMOUNT
};
struct WPE_WAIT_IRQ_STRUCT {
  enum WPE_IRQ_CLEAR_ENUM Clear;
  enum WPE_IRQ_TYPE_ENUM Type;
  unsigned int Status;
  unsigned int Timeout;
  int UserKey;
  int ProcessID;
  unsigned int bDumpReg;
};
struct WPE_CLEAR_IRQ_STRUCT {
  enum WPE_IRQ_TYPE_ENUM Type;
  int UserKey;
  unsigned int Status;
};
enum ISP_WPE_BUFQUE_CTRL_ENUM {
  ISP_WPE_BUFQUE_CTRL_ENQUE_FRAME = 0,
  ISP_WPE_BUFQUE_CTRL_WAIT_DEQUE,
  ISP_WPE_BUFQUE_CTRL_DEQUE_SUCCESS,
  ISP_WPE_BUFQUE_CTRL_DEQUE_FAIL,
  ISP_WPE_BUFQUE_CTRL_WAIT_FRAME,
  ISP_WPE_BUFQUE_CTRL_WAKE_WAITFRAME,
  ISP_WPE_BUFQUE_CTRL_CLAER_ALL,
  ISP_WPE_BUFQUE_CTRL_MAX
};
enum ISP_WPE_BUFQUE_PROPERTY {
  ISP_WPE_BUFQUE_PROPERTY_DIP = 0,
  ISP_WPE_BUFQUE_PROPERTY_WARP,
  ISP_WPE_BUFQUE_PROPERTY_WARP2,
  ISP_WPE_BUFQUE_PROPERTY_NUM,
};
struct ISP_WPE_BUFQUE_STRUCT {
  enum ISP_WPE_BUFQUE_CTRL_ENUM ctrl;
  enum ISP_WPE_BUFQUE_PROPERTY property;
  unsigned int processID;
  unsigned int callerID;
  int frameNum;
  int cQIdx;
  int dupCQIdx;
  int burstQIdx;
  unsigned int timeoutIns;
};
struct WPE_Config {
  unsigned int WPE_CTL_MOD_EN;
  unsigned int WPE_CTL_DMA_EN;
  unsigned int WPE_CTL_CFG;
  unsigned int WPE_CTL_FMT_SEL;
  unsigned int WPE_CTL_INT_EN;
  unsigned int WPE_CTL_INT_STATUS;
  unsigned int WPE_CTL_INT_STATUSX;
  unsigned int WPE_CTL_TDR_TILE;
  unsigned int WPE_CTL_TDR_DBG_STATUS;
  unsigned int WPE_CTL_TDR_TCM_EN;
  unsigned int WPE_CTL_SW_CTL;
  unsigned int WPE_CTL_SPARE0;
  unsigned int WPE_CTL_SPARE1;
  unsigned int WPE_CTL_SPARE2;
  unsigned int WPE_CTL_DONE_SEL;
  unsigned int WPE_CTL_DBG_SET;
  unsigned int WPE_CTL_DBG_PORT;
  unsigned int WPE_CTL_DATE_CODE;
  unsigned int WPE_CTL_PROJ_CODE;
  unsigned int WPE_CTL_WPE_DCM_DIS;
  unsigned int WPE_CTL_DMA_DCM_DIS;
  unsigned int WPE_CTL_WPE_DCM_STATUS;
  unsigned int WPE_CTL_DMA_DCM_STATUS;
  unsigned int WPE_CTL_WPE_REQ_STATUS;
  unsigned int WPE_CTL_DMA_REQ_STATUS;
  unsigned int WPE_CTL_WPE_RDY_STATUS;
  unsigned int WPE_CTL_DMA_RDY_STATUS;
  unsigned int WPE_VGEN_CTL;
  unsigned int WPE_VGEN_IN_IMG;
  unsigned int WPE_VGEN_OUT_IMG;
  unsigned int WPE_VGEN_HORI_STEP;
  unsigned int WPE_VGEN_VERT_STEP;
  unsigned int WPE_VGEN_HORI_INT_OFST;
  unsigned int WPE_VGEN_HORI_SUB_OFST;
  unsigned int WPE_VGEN_VERT_INT_OFST;
  unsigned int WPE_VGEN_VERT_SUB_OFST;
  unsigned int WPE_VGEN_POST_CTL;
  unsigned int WPE_VGEN_POST_COMP_X;
  unsigned int WPE_VGEN_POST_COMP_Y;
  unsigned int WPE_VGEN_MAX_VEC;
  unsigned int WPE_VFIFO_CTL;
  unsigned int WPE_CFIFO_CTL;
  unsigned int WPE_RWCTL_CTL;
  unsigned int WPE_CACHI_SPECIAL_FUN_EN;
  unsigned int WPE_C24_TILE_EDGE;
  unsigned int WPE_MDP_CROP_X;
  unsigned int WPE_MDP_CROP_Y;
  unsigned int WPE_ISPCROP_CON1;
  unsigned int WPE_ISPCROP_CON2;
  unsigned int WPE_PSP_CTL;
  unsigned int WPE_PSP2_CTL;
  unsigned int WPE_ADDR_GEN_SOFT_RSTSTAT_0;
  unsigned int WPE_ADDR_GEN_BASE_ADDR_0;
  unsigned int WPE_ADDR_GEN_OFFSET_ADDR_0;
  unsigned int WPE_ADDR_GEN_STRIDE_0;
  unsigned int WPE_CACHI_CON_0;
  unsigned int WPE_CACHI_CON2_0;
  unsigned int WPE_CACHI_CON3_0;
  unsigned int WPE_ADDR_GEN_ERR_CTRL_0;
  unsigned int WPE_ADDR_GEN_ERR_STAT_0;
  unsigned int WPE_ADDR_GEN_RSV1_0;
  unsigned int WPE_ADDR_GEN_DEBUG_SEL_0;
  unsigned int WPE_ADDR_GEN_SOFT_RSTSTAT_1;
  unsigned int WPE_ADDR_GEN_BASE_ADDR_1;
  unsigned int WPE_ADDR_GEN_OFFSET_ADDR_1;
  unsigned int WPE_ADDR_GEN_STRIDE_1;
  unsigned int WPE_CACHI_CON_1;
  unsigned int WPE_CACHI_CON2_1;
  unsigned int WPE_CACHI_CON3_1;
  unsigned int WPE_ADDR_GEN_ERR_CTRL_1;
  unsigned int WPE_ADDR_GEN_ERR_STAT_1;
  unsigned int WPE_ADDR_GEN_RSV1_1;
  unsigned int WPE_ADDR_GEN_DEBUG_SEL_1;
  unsigned int WPE_ADDR_GEN_SOFT_RSTSTAT_2;
  unsigned int WPE_ADDR_GEN_BASE_ADDR_2;
  unsigned int WPE_ADDR_GEN_OFFSET_ADDR_2;
  unsigned int WPE_ADDR_GEN_STRIDE_2;
  unsigned int WPE_CACHI_CON_2;
  unsigned int WPE_CACHI_CON2_2;
  unsigned int WPE_CACHI_CON3_2;
  unsigned int WPE_ADDR_GEN_ERR_CTRL_2;
  unsigned int WPE_ADDR_GEN_ERR_STAT_2;
  unsigned int WPE_ADDR_GEN_RSV1_2;
  unsigned int WPE_ADDR_GEN_DEBUG_SEL_2;
  unsigned int WPE_ADDR_GEN_SOFT_RSTSTAT_3;
  unsigned int WPE_ADDR_GEN_BASE_ADDR_3;
  unsigned int WPE_ADDR_GEN_OFFSET_ADDR_3;
  unsigned int WPE_ADDR_GEN_STRIDE_3;
  unsigned int WPE_CACHI_CON_3;
  unsigned int WPE_CACHI_CON2_3;
  unsigned int WPE_CACHI_CON3_3;
  unsigned int WPE_ADDR_GEN_ERR_CTRL_3;
  unsigned int WPE_ADDR_GEN_ERR_STAT_3;
  unsigned int WPE_ADDR_GEN_RSV1_3;
  unsigned int WPE_ADDR_GEN_DEBUG_SEL_3;
  unsigned int WPE_DMA_SOFT_RSTSTAT;
  unsigned int WPE_TDRI_BASE_ADDR;
  unsigned int WPE_TDRI_OFST_ADDR;
  unsigned int WPE_TDRI_XSIZE;
  unsigned int WPE_VERTICAL_FLIP_EN;
  unsigned int WPE_DMA_SOFT_RESET;
  unsigned int WPE_LAST_ULTRA_EN;
  unsigned int WPE_SPECIAL_FUN_EN;
  unsigned int WPE_WPEO_BASE_ADDR;
  unsigned int WPE_WPEO_OFST_ADDR;
  unsigned int WPE_WPEO_XSIZE;
  unsigned int WPE_WPEO_YSIZE;
  unsigned int WPE_WPEO_STRIDE;
  unsigned int WPE_WPEO_CON;
  unsigned int WPE_WPEO_CON2;
  unsigned int WPE_WPEO_CON3;
  unsigned int WPE_WPEO_CROP;
  unsigned int WPE_MSKO_BASE_ADDR;
  unsigned int WPE_MSKO_OFST_ADDR;
  unsigned int WPE_MSKO_XSIZE;
  unsigned int WPE_MSKO_YSIZE;
  unsigned int WPE_MSKO_STRIDE;
  unsigned int WPE_MSKO_CON;
  unsigned int WPE_MSKO_CON2;
  unsigned int WPE_MSKO_CON3;
  unsigned int WPE_MSKO_CROP;
  unsigned int WPE_VECI_BASE_ADDR;
  unsigned int WPE_VECI_OFST_ADDR;
  unsigned int WPE_VECI_XSIZE;
  unsigned int WPE_VECI_YSIZE;
  unsigned int WPE_VECI_STRIDE;
  unsigned int WPE_VECI_CON;
  unsigned int WPE_VECI_CON2;
  unsigned int WPE_VECI_CON3;
  unsigned int WPE_VEC2I_BASE_ADDR;
  unsigned int WPE_VEC2I_OFST_ADDR;
  unsigned int WPE_VEC2I_XSIZE;
  unsigned int WPE_VEC2I_YSIZE;
  unsigned int WPE_VEC2I_STRIDE;
  unsigned int WPE_VEC2I_CON;
  unsigned int WPE_VEC2I_CON2;
  unsigned int WPE_VEC2I_CON3;
  unsigned int WPE_VEC3I_BASE_ADDR;
  unsigned int WPE_VEC3I_OFST_ADDR;
  unsigned int WPE_VEC3I_XSIZE;
  unsigned int WPE_VEC3I_YSIZE;
  unsigned int WPE_VEC3I_STRIDE;
  unsigned int WPE_VEC3I_CON;
  unsigned int WPE_VEC3I_CON2;
  unsigned int WPE_VEC3I_CON3;
  unsigned int WPE_DMA_ERR_CTRL;
  unsigned int WPE_WPEO_ERR_STAT;
  unsigned int WPE_MSKO_ERR_STAT;
  unsigned int WPE_VECI_ERR_STAT;
  unsigned int WPE_VEC2I_ERR_STAT;
  unsigned int WPE_VEC3I_ERR_STAT;
  unsigned int WPE_DMA_DEBUG_ADDR;
  unsigned int WPE_DMA_DEBUG_SEL;
};
enum WPE_CMD_ENUM {
  WPE_CMD_RESET,
  WPE_CMD_DUMP_REG,
  WPE_CMD_DUMP_ISR_LOG,
  WPE_CMD_READ_REG,
  WPE_CMD_WRITE_REG,
  WPE_CMD_WAIT_IRQ,
  WPE_CMD_CLEAR_IRQ,
  WPE_CMD_ENQUE_NUM,
  WPE_CMD_ENQUE,
  WPE_CMD_ENQUE_REQ,
  WPE_CMD_DEQUE_NUM,
  WPE_CMD_DEQUE,
  WPE_CMD_DEQUE_REQ,
  WPE_CMD_DEQUE_DONE,
  WPE_CMD_WAIT_DEQUE,
  WPE_CMD_BUFQUE_CTRL,
  WPE_CMD_TOTAL,
};
struct WPE_Request {
  unsigned int m_ReqNum;
  struct WPE_Config * m_pWpeConfig;
};
#define WPE_RESET _IO(WPE_MAGIC, WPE_CMD_RESET)
#define WPE_DUMP_REG _IO(WPE_MAGIC, WPE_CMD_DUMP_REG)
#define WPE_DUMP_ISR_LOG _IO(WPE_MAGIC, WPE_CMD_DUMP_ISR_LOG)
#define WPE_READ_REGISTER _IOWR(WPE_MAGIC, WPE_CMD_READ_REG, struct WPE_REG_IO_STRUCT)
#define WPE_WRITE_REGISTER _IOWR(WPE_MAGIC, WPE_CMD_WRITE_REG, struct WPE_REG_IO_STRUCT)
#define WPE_WAIT_IRQ _IOW(WPE_MAGIC, WPE_CMD_WAIT_IRQ, struct WPE_WAIT_IRQ_STRUCT)
#define WPE_CLEAR_IRQ _IOW(WPE_MAGIC, WPE_CMD_CLEAR_IRQ, struct WPE_CLEAR_IRQ_STRUCT)
#define WPE_ENQNUE_NUM _IOW(WPE_MAGIC, WPE_CMD_ENQUE_NUM, int)
#define WPE_ENQUE _IOWR(WPE_MAGIC, WPE_CMD_ENQUE, struct WPE_Config)
#define WPE_ENQUE_REQ _IOWR(WPE_MAGIC, WPE_CMD_ENQUE_REQ, struct WPE_Request)
#define WPE_DEQUE_NUM _IOR(WPE_MAGIC, WPE_CMD_DEQUE_NUM, int)
#define WPE_DEQUE _IOWR(WPE_MAGIC, WPE_CMD_DEQUE, struct WPE_Config)
#define WPE_DEQUE_REQ _IOWR(WPE_MAGIC, WPE_CMD_DEQUE_REQ, struct WPE_Request)
#define WPE_DEQUE_DONE _IOWR(WPE_MAGIC, WPE_CMD_DEQUE_DONE, struct WPE_Request)
#define WPE_WAIT_DEQUE _IO(WPE_MAGIC, WPE_CMD_WAIT_DEQUE)
#define WPE_BUFQUE_CTRL _IOWR(WPE_MAGIC, WPE_CMD_BUFQUE_CTRL, struct ISP_WPE_BUFQUE_STRUCT)
#endif
