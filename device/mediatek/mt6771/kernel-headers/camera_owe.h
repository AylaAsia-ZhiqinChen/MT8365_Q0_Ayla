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
#ifndef _MT_OWE_H
#define _MT_OWE_H
#include <linux/ioctl.h>
#define KERNEL_LOG
#define _SUPPORT_MAX_OWE_FRAME_REQUEST_ 6
#define _SUPPORT_MAX_OWE_REQUEST_RING_SIZE_ 4
#define SIG_ERESTARTSYS 512
#define OWE_DEV_MAJOR_NUMBER 251
#define OWE_MAGIC 'o'
#define OWE_REG_RANGE (0x1000)
#define OWE_BASE_HW 0x1502C000
#define OWE_OCC_INT_ST (1 << 1)
#define OWE_WMFE_INT_ST (1 << 2)
#define WMFE_CTRL_SIZE 5
struct OWE_REG_STRUCT {
  unsigned int module;
  unsigned int Addr;
  unsigned int Val;
};
struct OWE_REG_IO_STRUCT {
  struct OWE_REG_STRUCT * pData;
  unsigned int Count;
};
enum OWE_IRQ_CLEAR_ENUM {
  OWE_IRQ_CLEAR_NONE,
  OWE_IRQ_CLEAR_WAIT,
  OWE_IRQ_WAIT_CLEAR,
  OWE_IRQ_CLEAR_STATUS,
  OWE_IRQ_CLEAR_ALL
};
enum OWE_IRQ_TYPE_ENUM {
  OWE_IRQ_TYPE_INT_OWE_ST,
  OWE_IRQ_TYPE_AMOUNT
};
struct OWE_WAIT_IRQ_STRUCT {
  enum OWE_IRQ_CLEAR_ENUM Clear;
  enum OWE_IRQ_TYPE_ENUM Type;
  unsigned int Status;
  unsigned int Timeout;
  int UserKey;
  int ProcessID;
  unsigned int bDumpReg;
};
struct OWE_CLEAR_IRQ_STRUCT {
  enum OWE_IRQ_TYPE_ENUM Type;
  int UserKey;
  unsigned int Status;
};
enum OCC_DMA {
  OCC_DMA_REF_VEC = 0x0,
  OCC_DMA_REF_PXL = 0x1,
  OCC_DMA_MAJ_VEC = 0x2,
  OCC_DMA_MAJ_PXL = 0x3,
  OCC_DMA_WDMA = 0x4,
  OCC_DMA_NUM,
};
enum WMFE_DMA {
  WMFE_DMA_IMGI = 0x0,
  WMFE_DMA_DPI = 0x1,
  WMFE_DMA_TBLI = 0x2,
  WMFE_DMA_MASKI = 0x3,
  WMFE_DMA_DPO = 0x4,
  WMFE_DMA_NUM,
};
struct OWE_OCCConfig {
  unsigned int DPE_OCC_CTRL_0;
  unsigned int DPE_OCC_CTRL_1;
  unsigned int DPE_OCC_CTRL_2;
  unsigned int DPE_OCC_CTRL_3;
  unsigned int DPE_OCC_REF_VEC_BASE;
  unsigned int DPE_OCC_REF_VEC_STRIDE;
  unsigned int DPE_OCC_REF_PXL_BASE;
  unsigned int DPE_OCC_REF_PXL_STRIDE;
  unsigned int DPE_OCC_MAJ_VEC_BASE;
  unsigned int DPE_OCC_MAJ_VEC_STRIDE;
  unsigned int DPE_OCC_MAJ_PXL_BASE;
  unsigned int DPE_OCC_MAJ_PXL_STRIDE;
  unsigned int DPE_OCC_WDMA_BASE;
  unsigned int DPE_OCC_WDMA_STRIDE;
  unsigned int DPE_OCC_PQ_0;
  unsigned int DPE_OCC_PQ_1;
  unsigned int DPE_OCC_SPARE;
  unsigned int DPE_OCC_DFT;
  unsigned int eng_secured;
  unsigned int dma_sec_size[OCC_DMA_NUM];
};
struct OWE_WMFECtrl {
  unsigned int WMFE_CTRL;
  unsigned int WMFE_SIZE;
  unsigned int WMFE_IMGI_BASE_ADDR;
  unsigned int WMFE_IMGI_STRIDE;
  unsigned int WMFE_DPI_BASE_ADDR;
  unsigned int WMFE_DPI_STRIDE;
  unsigned int WMFE_TBLI_BASE_ADDR;
  unsigned int WMFE_TBLI_STRIDE;
  unsigned int WMFE_MASKI_BASE_ADDR;
  unsigned int WMFE_MASKI_STRIDE;
  unsigned int WMFE_DPO_BASE_ADDR;
  unsigned int WMFE_DPO_STRIDE;
  unsigned int eng_secured;
  unsigned int dma_sec_size[WMFE_DMA_NUM];
};
struct OWE_WMFEConfig {
  unsigned int WmfeCtrlSize;
  struct OWE_WMFECtrl WmfeCtrl[WMFE_CTRL_SIZE];
};
enum OWE_CMD_ENUM {
  OWE_CMD_RESET,
  OWE_CMD_DUMP_REG,
  OWE_CMD_DUMP_ISR_LOG,
  OWE_CMD_READ_REG,
  OWE_CMD_WRITE_REG,
  OWE_CMD_WAIT_IRQ,
  OWE_CMD_CLEAR_IRQ,
  OWE_CMD_OCC_ENQUE_REQ,
  OWE_CMD_OCC_DEQUE_REQ,
  OWE_CMD_WMFE_ENQUE_REQ,
  OWE_CMD_WMFE_DEQUE_REQ,
  OWE_CMD_TOTAL,
};
struct OWE_OCCRequest {
  unsigned int m_ReqNum;
  struct OWE_OCCConfig * m_pOweConfig;
};
struct OWE_WMFERequest {
  unsigned int m_ReqNum;
  struct OWE_WMFEConfig * m_pWmfeConfig;
};
#define OWE_RESET _IO(OWE_MAGIC, OWE_CMD_RESET)
#define OWE_DUMP_REG _IO(OWE_MAGIC, OWE_CMD_DUMP_REG)
#define OWE_DUMP_ISR_LOG _IO(OWE_MAGIC, OWE_CMD_DUMP_ISR_LOG)
#define OWE_READ_REGISTER _IOWR(OWE_MAGIC, OWE_CMD_READ_REG, struct OWE_REG_IO_STRUCT)
#define OWE_WRITE_REGISTER _IOWR(OWE_MAGIC, OWE_CMD_WRITE_REG, struct OWE_REG_IO_STRUCT)
#define OWE_WAIT_IRQ _IOW(OWE_MAGIC, OWE_CMD_WAIT_IRQ, struct OWE_WAIT_IRQ_STRUCT)
#define OWE_CLEAR_IRQ _IOW(OWE_MAGIC, OWE_CMD_CLEAR_IRQ, struct OWE_CLEAR_IRQ_STRUCT)
#define OWE_OCC_ENQUE_REQ _IOWR(OWE_MAGIC, OWE_CMD_OCC_ENQUE_REQ, struct OWE_OCCRequest)
#define OWE_OCC_DEQUE_REQ _IOWR(OWE_MAGIC, OWE_CMD_OCC_DEQUE_REQ, struct OWE_OCCRequest)
#define OWE_WMFE_ENQUE_REQ _IOWR(OWE_MAGIC, OWE_CMD_WMFE_ENQUE_REQ, struct OWE_WMFERequest)
#define OWE_WMFE_DEQUE_REQ _IOWR(OWE_MAGIC, OWE_CMD_WMFE_DEQUE_REQ, struct OWE_WMFERequest)
#endif
