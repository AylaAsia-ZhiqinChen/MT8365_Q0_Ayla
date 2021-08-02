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
#ifndef _MT_MFB_H
#define _MT_MFB_H
#include <linux/ioctl.h>
#define KERNEL_LOG
#define _SUPPORT_MAX_MFB_FRAME_REQUEST_ 32
#define _SUPPORT_MAX_MFB_REQUEST_RING_SIZE_ 32
#define SIG_ERESTARTSYS 512
#define MFB_DEV_MAJOR_NUMBER 258
#define MFB_MAGIC 'm'
#define MFB_REG_RANGE (0x1000)
#define MFB_BASE_HW 0x1502E000
#define MFB_INT_ST ((unsigned int) 1 << 0)
struct MFB_REG_STRUCT {
  unsigned int module;
  unsigned int Addr;
  unsigned int Val;
};
#define MFB_REG_STRUCT struct MFB_REG_STRUCT
struct MFB_REG_IO_STRUCT {
  MFB_REG_STRUCT * pData;
  unsigned int Count;
};
#define MFB_REG_IO_STRUCT struct MFB_REG_IO_STRUCT
enum MFB_IRQ_CLEAR_ENUM {
  MFB_IRQ_CLEAR_NONE,
  MFB_IRQ_CLEAR_WAIT,
  MFB_IRQ_WAIT_CLEAR,
  MFB_IRQ_CLEAR_STATUS,
  MFB_IRQ_CLEAR_ALL
};
#define MFB_IRQ_CLEAR_ENUM enum MFB_IRQ_CLEAR_ENUM
enum MFB_IRQ_TYPE_ENUM {
  MFB_IRQ_TYPE_INT_MFB_ST,
  MFB_IRQ_TYPE_AMOUNT
};
#define MFB_IRQ_TYPE_ENUM enum MFB_IRQ_TYPE_ENUM
struct MFB_WAIT_IRQ_STRUCT {
  MFB_IRQ_CLEAR_ENUM Clear;
  MFB_IRQ_TYPE_ENUM Type;
  unsigned int Status;
  unsigned int Timeout;
  int UserKey;
  int ProcessID;
  unsigned int bDumpReg;
};
#define MFB_WAIT_IRQ_STRUCT struct MFB_WAIT_IRQ_STRUCT
struct MFB_CLEAR_IRQ_STRUCT {
  MFB_IRQ_TYPE_ENUM Type;
  int UserKey;
  unsigned int Status;
};
#define MFB_CLEAR_IRQ_STRUCT struct MFB_CLEAR_IRQ_STRUCT
struct MFB_Config {
  unsigned int MFB_TOP_CFG0;
  unsigned int MFB_TOP_CFG2;
  bool MFB_BLDMODE;
  unsigned int MFB_MFBI_ADDR;
  unsigned int MFB_MFBI_STRIDE;
  unsigned int MFB_MFBI_YSIZE;
  unsigned int MFB_MFBI_B_ADDR;
  unsigned int MFB_MFBI_B_STRIDE;
  unsigned int MFB_MFBI_B_YSIZE;
  unsigned int MFB_MFB2I_ADDR;
  unsigned int MFB_MFB2I_STRIDE;
  unsigned int MFB_MFB2I_YSIZE;
  unsigned int MFB_MFB2I_B_ADDR;
  unsigned int MFB_MFB2I_B_STRIDE;
  unsigned int MFB_MFB2I_B_YSIZE;
  unsigned int MFB_MFB3I_ADDR;
  unsigned int MFB_MFB3I_STRIDE;
  unsigned int MFB_MFB3I_YSIZE;
  unsigned int MFB_MFB4I_ADDR;
  unsigned int MFB_MFB4I_STRIDE;
  unsigned int MFB_MFB4I_YSIZE;
  unsigned int MFB_MFBO_ADDR;
  unsigned int MFB_MFBO_STRIDE;
  unsigned int MFB_MFBO_YSIZE;
  unsigned int MFB_MFBO_B_ADDR;
  unsigned int MFB_MFBO_B_STRIDE;
  unsigned int MFB_MFBO_B_YSIZE;
  unsigned int MFB_MFB2O_ADDR;
  unsigned int MFB_MFB2O_STRIDE;
  unsigned int MFB_MFB2O_YSIZE;
  unsigned int MFB_TDRI_ADDR;
  unsigned int MFB_TDRI_XSIZE;
  unsigned int MFB_SRZ_CTRL;
  unsigned int MFB_SRZ_IN_IMG;
  unsigned int MFB_SRZ_OUT_IMG;
  unsigned int MFB_SRZ_HORI_STEP;
  unsigned int MFB_SRZ_VERT_STEP;
  unsigned int MFB_SRZ_HORI_INT_OFST;
  unsigned int MFB_SRZ_HORI_SUB_OFST;
  unsigned int MFB_SRZ_VERT_INT_OFST;
  unsigned int MFB_SRZ_VERT_SUB_OFST;
  unsigned int MFB_C02A_CON;
  unsigned int MFB_C02A_CROP_CON1;
  unsigned int MFB_C02A_CROP_CON2;
  unsigned int MFB_C02B_CON;
  unsigned int MFB_C02B_CROP_CON1;
  unsigned int MFB_C02B_CROP_CON2;
  unsigned int MFB_CRSP_CTRL;
  unsigned int MFB_CRSP_OUT_IMG;
  unsigned int MFB_CRSP_STEP_OFST;
  unsigned int MFB_CRSP_CROP_X;
  unsigned int MFB_CRSP_CROP_Y;
#define MFB_TUNABLE
#ifdef MFB_TUNABLE
  unsigned int MFB_CON;
  unsigned int MFB_LL_CON1;
  unsigned int MFB_LL_CON2;
  unsigned int MFB_LL_CON3;
  unsigned int MFB_LL_CON4;
  unsigned int MFB_EDGE;
  unsigned int MFB_LL_CON5;
  unsigned int MFB_LL_CON6;
  unsigned int MFB_LL_CON7;
  unsigned int MFB_LL_CON8;
  unsigned int MFB_LL_CON9;
  unsigned int MFB_LL_CON10;
  unsigned int MFB_MBD_CON0;
  unsigned int MFB_MBD_CON1;
  unsigned int MFB_MBD_CON2;
  unsigned int MFB_MBD_CON3;
  unsigned int MFB_MBD_CON4;
  unsigned int MFB_MBD_CON5;
  unsigned int MFB_MBD_CON6;
  unsigned int MFB_MBD_CON7;
  unsigned int MFB_MBD_CON8;
  unsigned int MFB_MBD_CON9;
  unsigned int MFB_MBD_CON10;
#endif
};
#define MFB_Config struct MFB_Config
enum MFB_CMD_ENUM {
  MFB_CMD_RESET,
  MFB_CMD_DUMP_REG,
  MFB_CMD_DUMP_ISR_LOG,
  MFB_CMD_READ_REG,
  MFB_CMD_WRITE_REG,
  MFB_CMD_WAIT_IRQ,
  MFB_CMD_CLEAR_IRQ,
  MFB_CMD_ENQUE_NUM,
  MFB_CMD_ENQUE,
  MFB_CMD_ENQUE_REQ,
  MFB_CMD_DEQUE_NUM,
  MFB_CMD_DEQUE,
  MFB_CMD_DEQUE_REQ,
  MFB_CMD_TOTAL,
};
struct MFB_Request {
  unsigned int m_ReqNum;
  MFB_Config * m_pMfbConfig;
};
#define MFB_Request struct MFB_Request
#define MFB_RESET _IO(MFB_MAGIC, MFB_CMD_RESET)
#define MFB_DUMP_REG _IO(MFB_MAGIC, MFB_CMD_DUMP_REG)
#define MFB_DUMP_ISR_LOG _IO(MFB_MAGIC, MFB_CMD_DUMP_ISR_LOG)
#define MFB_READ_REGISTER _IOWR(MFB_MAGIC, MFB_CMD_READ_REG, MFB_REG_IO_STRUCT)
#define MFB_WRITE_REGISTER _IOWR(MFB_MAGIC, MFB_CMD_WRITE_REG, MFB_REG_IO_STRUCT)
#define MFB_WAIT_IRQ _IOW(MFB_MAGIC, MFB_CMD_WAIT_IRQ, MFB_WAIT_IRQ_STRUCT)
#define MFB_CLEAR_IRQ _IOW(MFB_MAGIC, MFB_CMD_CLEAR_IRQ, MFB_CLEAR_IRQ_STRUCT)
#define MFB_ENQNUE_NUM _IOW(MFB_MAGIC, MFB_CMD_ENQUE_NUM, int)
#define MFB_ENQUE _IOWR(MFB_MAGIC, MFB_CMD_ENQUE, MFB_Config)
#define MFB_ENQUE_REQ _IOWR(MFB_MAGIC, MFB_CMD_ENQUE_REQ, MFB_Request)
#define MFB_DEQUE_NUM _IOR(MFB_MAGIC, MFB_CMD_DEQUE_NUM, int)
#define MFB_DEQUE _IOWR(MFB_MAGIC, MFB_CMD_DEQUE, MFB_Config)
#define MFB_DEQUE_REQ _IOWR(MFB_MAGIC, MFB_CMD_DEQUE_REQ, MFB_Request)
#endif
