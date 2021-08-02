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
#ifndef _MT_TSF_H
#define _MT_TSF_H
#include <linux/ioctl.h>
#define KERNEL_LOG
#define SIG_ERESTARTSYS 512
#define TSF_DEV_MAJOR_NUMBER 251
#define TSF_MAGIC 't'
#define TSF_REG_RANGE (0x1000)
#define TSF_BASE_HW 0x1a0a1000
#define TSF_INT_ST (1 << 0)
struct TSF_REG_STRUCT {
  unsigned int module;
  unsigned int Addr;
  unsigned int Val;
};
struct TSF_REG_IO_STRUCT {
  struct TSF_REG_STRUCT * pData;
  unsigned int Count;
};
enum TSF_IRQ_CLEAR_ENUM {
  TSF_IRQ_CLEAR_NONE,
  TSF_IRQ_CLEAR_WAIT,
  TSF_IRQ_WAIT_CLEAR,
  TSF_IRQ_CLEAR_STATUS,
  TSF_IRQ_CLEAR_ALL
};
enum TSF_IRQ_TYPE_ENUM {
  TSF_IRQ_TYPE_INT_TSF_ST,
  TSF_IRQ_TYPE_AMOUNT
};
struct TSF_WAIT_IRQ_STRUCT {
  enum TSF_IRQ_CLEAR_ENUM Clear;
  enum TSF_IRQ_TYPE_ENUM Type;
  unsigned int Status;
  unsigned int Timeout;
  int UserKey;
  int ProcessID;
  unsigned int bDumpReg;
};
struct TSF_CLEAR_IRQ_STRUCT {
  enum TSF_IRQ_TYPE_ENUM Type;
  int UserKey;
  unsigned int Status;
};
enum TSF_CMD_ENUM {
  TSF_CMD_RESET,
  TSF_CMD_DUMP_REG,
  TSF_CMD_DUMP_ISR_LOG,
  TSF_CMD_READ_REG,
  TSF_CMD_WRITE_REG,
  TSF_CMD_WAIT_IRQ,
  TSF_CMD_CLEAR_IRQ,
  TSF_CMD_TOTAL,
};
#define TSF_RESET _IO(TSF_MAGIC, TSF_CMD_RESET)
#define TSF_DUMP_REG _IO(TSF_MAGIC, TSF_CMD_DUMP_REG)
#define TSF_DUMP_ISR_LOG _IO(TSF_MAGIC, TSF_CMD_DUMP_ISR_LOG)
#define TSF_READ_REGISTER _IOWR(TSF_MAGIC, TSF_CMD_READ_REG, struct TSF_REG_IO_STRUCT)
#define TSF_WRITE_REGISTER _IOWR(TSF_MAGIC, TSF_CMD_WRITE_REG, struct TSF_REG_IO_STRUCT)
#define TSF_WAIT_IRQ _IOW(TSF_MAGIC, TSF_CMD_WAIT_IRQ, struct TSF_WAIT_IRQ_STRUCT)
#define TSF_CLEAR_IRQ _IOW(TSF_MAGIC, TSF_CMD_CLEAR_IRQ, struct TSF_CLEAR_IRQ_STRUCT)
#endif
