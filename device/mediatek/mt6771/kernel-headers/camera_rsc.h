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
#ifndef _MT_RSC_H
#define _MT_RSC_H
#include <linux/ioctl.h>
#define KERNEL_LOG
#define _SUPPORT_MAX_RSC_FRAME_REQUEST_ 32
#define _SUPPORT_MAX_RSC_REQUEST_RING_SIZE_ 32
#define SIG_ERESTARTSYS 512
#define RSC_DEV_MAJOR_NUMBER 251
#define RSC_MAGIC 'r'
#define RSC_REG_RANGE (0x1000)
#define RSC_BASE_HW 0x15029000
#define RSC_INT_ST ((unsigned int) 1 << 0)
struct RSC_REG_STRUCT {
  unsigned int module;
  unsigned int Addr;
  unsigned int Val;
};
struct RSC_REG_IO_STRUCT {
  struct RSC_REG_STRUCT * pData;
  unsigned int Count;
};
enum RSC_IRQ_CLEAR_ENUM {
  RSC_IRQ_CLEAR_NONE,
  RSC_IRQ_CLEAR_WAIT,
  RSC_IRQ_WAIT_CLEAR,
  RSC_IRQ_CLEAR_STATUS,
  RSC_IRQ_CLEAR_ALL
};
enum RSC_IRQ_TYPE_ENUM {
  RSC_IRQ_TYPE_INT_RSC_ST,
  RSC_IRQ_TYPE_AMOUNT
};
struct RSC_WAIT_IRQ_STRUCT {
  enum RSC_IRQ_CLEAR_ENUM Clear;
  enum RSC_IRQ_TYPE_ENUM Type;
  unsigned int Status;
  unsigned int Timeout;
  int UserKey;
  int ProcessID;
  unsigned int bDumpReg;
};
struct RSC_CLEAR_IRQ_STRUCT {
  enum RSC_IRQ_TYPE_ENUM Type;
  int UserKey;
  unsigned int Status;
};
struct RSC_Config {
  unsigned int RSC_CTRL;
  unsigned int RSC_SIZE;
  unsigned int RSC_IMGI_C_BASE_ADDR;
  unsigned int RSC_IMGI_C_STRIDE;
  unsigned int RSC_IMGI_P_BASE_ADDR;
  unsigned int RSC_IMGI_P_STRIDE;
  unsigned int RSC_MVI_BASE_ADDR;
  unsigned int RSC_MVI_STRIDE;
  unsigned int RSC_APLI_C_BASE_ADDR;
  unsigned int RSC_APLI_P_BASE_ADDR;
  unsigned int RSC_MVO_BASE_ADDR;
  unsigned int RSC_MVO_STRIDE;
  unsigned int RSC_BVO_BASE_ADDR;
  unsigned int RSC_BVO_STRIDE;
#define RSC_TUNABLE
#ifdef RSC_TUNABLE
  unsigned int RSC_MV_OFFSET;
  unsigned int RSC_GMV_OFFSET;
  unsigned int RSC_CAND_NUM;
  unsigned int RSC_RAND_HORZ_LUT;
  unsigned int RSC_RAND_VERT_LUT;
  unsigned int RSC_SAD_CTRL;
  unsigned int RSC_SAD_EDGE_GAIN_CTRL;
  unsigned int RSC_SAD_CRNR_GAIN_CTRL;
  unsigned int RSC_STILL_STRIP_CTRL0;
  unsigned int RSC_STILL_STRIP_CTRL1;
  unsigned int RSC_RAND_PNLTY_CTRL;
  unsigned int RSC_RAND_PNLTY_GAIN_CTRL0;
  unsigned int RSC_RAND_PNLTY_GAIN_CTRL1;
#endif
  unsigned int RSC_STA_0;
};
enum RSC_CMD_ENUM {
  RSC_CMD_RESET,
  RSC_CMD_DUMP_REG,
  RSC_CMD_DUMP_ISR_LOG,
  RSC_CMD_READ_REG,
  RSC_CMD_WRITE_REG,
  RSC_CMD_WAIT_IRQ,
  RSC_CMD_CLEAR_IRQ,
  RSC_CMD_ENQUE_NUM,
  RSC_CMD_ENQUE,
  RSC_CMD_ENQUE_REQ,
  RSC_CMD_DEQUE_NUM,
  RSC_CMD_DEQUE,
  RSC_CMD_DEQUE_REQ,
  RSC_CMD_TOTAL,
};
struct RSC_Request {
  unsigned int m_ReqNum;
  struct RSC_Config * m_pRscConfig;
};
#define RSC_RESET _IO(RSC_MAGIC, RSC_CMD_RESET)
#define RSC_DUMP_REG _IO(RSC_MAGIC, RSC_CMD_DUMP_REG)
#define RSC_DUMP_ISR_LOG _IO(RSC_MAGIC, RSC_CMD_DUMP_ISR_LOG)
#define RSC_READ_REGISTER _IOWR(RSC_MAGIC, RSC_CMD_READ_REG, struct RSC_REG_IO_STRUCT)
#define RSC_WRITE_REGISTER _IOWR(RSC_MAGIC, RSC_CMD_WRITE_REG, struct RSC_REG_IO_STRUCT)
#define RSC_WAIT_IRQ _IOW(RSC_MAGIC, RSC_CMD_WAIT_IRQ, struct RSC_WAIT_IRQ_STRUCT)
#define RSC_CLEAR_IRQ _IOW(RSC_MAGIC, RSC_CMD_CLEAR_IRQ, struct RSC_CLEAR_IRQ_STRUCT)
#define RSC_ENQNUE_NUM _IOW(RSC_MAGIC, RSC_CMD_ENQUE_NUM, int)
#define RSC_ENQUE _IOWR(RSC_MAGIC, RSC_CMD_ENQUE, struct RSC_Config)
#define RSC_ENQUE_REQ _IOWR(RSC_MAGIC, RSC_CMD_ENQUE_REQ, struct RSC_Request)
#define RSC_DEQUE_NUM _IOR(RSC_MAGIC, RSC_CMD_DEQUE_NUM, int)
#define RSC_DEQUE _IOWR(RSC_MAGIC, RSC_CMD_DEQUE, struct RSC_Config)
#define RSC_DEQUE_REQ _IOWR(RSC_MAGIC, RSC_CMD_DEQUE_REQ, struct RSC_Request)
#endif
