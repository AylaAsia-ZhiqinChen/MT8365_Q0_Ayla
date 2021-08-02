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
#ifndef _MT_DIP_H
#define _MT_DIP_H
#include <linux/ioctl.h>
#define KERNEL_LOG
#define ISR_LOG_ON
#define SIG_ERESTARTSYS 512
#define DIP_DEV_MAJOR_NUMBER 251
#define DIP_MAGIC 'D'
#define DIP_A_BASE_HW 0x15022000
#define DIP_REG_RANGE (0x6000)
#define MAX_TILE_TOT_NO (256)
#define MAX_ISP_DUMP_HEX_PER_TILE (256)
#define MAX_ISP_TILE_TDR_HEX_NO (MAX_TILE_TOT_NO * MAX_ISP_DUMP_HEX_PER_TILE)
#define MAX_DIP_CMDQ_BUFFER_SIZE (0x1000)
#define DIP_TDRI_ADDR_MASK (0xffff0000)
#define DIP_IMBI_BASEADDR_OFFSET (0x500 >> 2)
#define DIP_DUMP_ADDR_MASK (0xffffffff)
#define DIP_CMDQ_BASEADDR_OFFSET (12)
enum DIP_DEV_NODE_ENUM {
  DIP_IMGSYS_CONFIG_IDX = 0,
  DIP_DIP_A_IDX,
  DIP_DEV_NODE_NUM
};
enum DIP_IRQ_CLEAR_ENUM {
  DIP_IRQ_CLEAR_NONE,
  DIP_IRQ_CLEAR_WAIT,
  DIP_IRQ_CLEAR_STATUS,
  DIP_IRQ_CLEAR_ALL
};
enum DIP_IRQ_TYPE_ENUM {
  DIP_IRQ_TYPE_INT_DIP_A_ST,
  DIP_IRQ_TYPE_AMOUNT
};
struct DIP_WAIT_IRQ_ST {
  enum DIP_IRQ_CLEAR_ENUM Clear;
  unsigned int Status;
  int UserKey;
  unsigned int Timeout;
};
struct DIP_WAIT_IRQ_STRUCT {
  enum DIP_IRQ_TYPE_ENUM Type;
  unsigned int bDumpReg;
  struct DIP_WAIT_IRQ_ST EventInfo;
};
struct DIP_REGISTER_USERKEY_STRUCT {
  int userKey;
  char userName[32];
};
struct DIP_CLEAR_IRQ_ST {
  int UserKey;
  unsigned int Status;
};
struct DIP_CLEAR_IRQ_STRUCT {
  enum DIP_IRQ_TYPE_ENUM Type;
  struct DIP_CLEAR_IRQ_ST EventInfo;
};
struct DIP_REG_STRUCT {
  unsigned int module;
  unsigned int Addr;
  unsigned int Val;
};
struct DIP_REG_IO_STRUCT {
  struct DIP_REG_STRUCT * pData;
  unsigned int Count;
};
enum DIP_DUMP_CMD {
  DIP_DUMP_TPIPEBUF_CMD = 0,
  DIP_DUMP_TUNINGBUF_CMD,
  DIP_DUMP_DIPVIRBUF_CMD,
  DIP_DUMP_CMDQVIRBUF_CMD
};
struct DIP_DUMP_BUFFER_STRUCT {
  unsigned int DumpCmd;
  unsigned int * pBuffer;
  unsigned int BytesofBufferSize;
};
struct DIP_GET_DUMP_INFO_STRUCT {
  unsigned int extracmd;
  unsigned int imgi_baseaddr;
  unsigned int tdri_baseaddr;
  unsigned int dmgi_baseaddr;
  unsigned int cmdq_baseaddr;
};
enum DIP_MEMORY_INFO_CMD {
  DIP_MEMORY_INFO_TPIPE_CMD = 1,
  DIP_MEMORY_INFO_CMDQ_CMD
};
struct DIP_MEM_INFO_STRUCT {
  unsigned int MemInfoCmd;
  unsigned int MemPa;
  unsigned int * MemVa;
  unsigned int MemSizeDiff;
};
enum DIP_P2_BUFQUE_CTRL_ENUM {
  DIP_P2_BUFQUE_CTRL_ENQUE_FRAME = 0,
  DIP_P2_BUFQUE_CTRL_WAIT_DEQUE,
  DIP_P2_BUFQUE_CTRL_DEQUE_SUCCESS,
  DIP_P2_BUFQUE_CTRL_DEQUE_FAIL,
  DIP_P2_BUFQUE_CTRL_WAIT_FRAME,
  DIP_P2_BUFQUE_CTRL_WAKE_WAITFRAME,
  DIP_P2_BUFQUE_CTRL_CLAER_ALL,
  DIP_P2_BUFQUE_CTRL_MAX
};
enum DIP_P2_BUFQUE_PROPERTY {
  DIP_P2_BUFQUE_PROPERTY_DIP = 0,
  DIP_P2_BUFQUE_PROPERTY_NUM = 1,
  DIP_P2_BUFQUE_PROPERTY_WARP
};
struct DIP_P2_BUFQUE_STRUCT {
  enum DIP_P2_BUFQUE_CTRL_ENUM ctrl;
  enum DIP_P2_BUFQUE_PROPERTY property;
  unsigned int processID;
  unsigned int callerID;
  int frameNum;
  int cQIdx;
  int dupCQIdx;
  int burstQIdx;
  unsigned int timeoutIns;
};
enum DIP_P2_BUF_STATE_ENUM {
  DIP_P2_BUF_STATE_NONE = - 1,
  DIP_P2_BUF_STATE_ENQUE = 0,
  DIP_P2_BUF_STATE_RUNNING,
  DIP_P2_BUF_STATE_WAIT_DEQUE_FAIL,
  DIP_P2_BUF_STATE_DEQUE_SUCCESS,
  DIP_P2_BUF_STATE_DEQUE_FAIL
};
enum DIP_P2_BUFQUE_LIST_TAG {
  DIP_P2_BUFQUE_LIST_TAG_PACKAGE = 0,
  DIP_P2_BUFQUE_LIST_TAG_UNIT
};
enum DIP_P2_BUFQUE_MATCH_TYPE {
  DIP_P2_BUFQUE_MATCH_TYPE_WAITDQ = 0,
  DIP_P2_BUFQUE_MATCH_TYPE_WAITFM,
  DIP_P2_BUFQUE_MATCH_TYPE_FRAMEOP,
  DIP_P2_BUFQUE_MATCH_TYPE_WAITFMEQD
};
enum DIP_CMD_ENUM {
  DIP_CMD_RESET_BY_HWMODULE,
  DIP_CMD_READ_REG,
  DIP_CMD_WRITE_REG,
  DIP_CMD_WAIT_IRQ,
  DIP_CMD_CLEAR_IRQ,
  DIP_CMD_DEBUG_FLAG,
  DIP_CMD_P2_BUFQUE_CTRL,
  DIP_CMD_WAKELOCK_CTRL,
  DIP_CMD_FLUSH_IRQ_REQUEST,
  DIP_CMD_ION_IMPORT,
  DIP_CMD_ION_FREE,
  DIP_CMD_ION_FREE_BY_HWMODULE,
  DIP_CMD_DUMP_BUFFER,
  DIP_CMD_GET_DUMP_INFO,
  DIP_CMD_SET_MEM_INFO
};
#define DIP_RESET_BY_HWMODULE _IOW(DIP_MAGIC, DIP_CMD_RESET_BY_HWMODULE, unsigned long)
#define DIP_READ_REGISTER _IOWR(DIP_MAGIC, DIP_CMD_READ_REG, struct DIP_REG_IO_STRUCT)
#define DIP_WRITE_REGISTER _IOWR(DIP_MAGIC, DIP_CMD_WRITE_REG, struct DIP_REG_IO_STRUCT)
#define DIP_WAIT_IRQ _IOW(DIP_MAGIC, DIP_CMD_WAIT_IRQ, struct DIP_WAIT_IRQ_STRUCT)
#define DIP_CLEAR_IRQ _IOW(DIP_MAGIC, DIP_CMD_CLEAR_IRQ, struct DIP_CLEAR_IRQ_STRUCT)
#define DIP_FLUSH_IRQ_REQUEST _IOW(DIP_MAGIC, DIP_CMD_FLUSH_IRQ_REQUEST, struct DIP_WAIT_IRQ_STRUCT)
#define DIP_DEBUG_FLAG _IOW(DIP_MAGIC, DIP_CMD_DEBUG_FLAG, unsigned char *)
#define DIP_P2_BUFQUE_CTRL _IOWR(DIP_MAGIC, DIP_CMD_P2_BUFQUE_CTRL, struct DIP_P2_BUFQUE_STRUCT)
#define DIP_WAKELOCK_CTRL _IOWR(DIP_MAGIC, DIP_CMD_WAKELOCK_CTRL, unsigned long)
#define DIP_DUMP_BUFFER _IOWR(DIP_MAGIC, DIP_CMD_DUMP_BUFFER, struct DIP_DUMP_BUFFER_STRUCT)
#define DIP_GET_DUMP_INFO _IOWR(DIP_MAGIC, DIP_CMD_GET_DUMP_INFO, struct DIP_GET_DUMP_INFO_STRUCT)
#define DIP_SET_MEM_INFO _IOWR(DIP_MAGIC, DIP_CMD_SET_MEM_INFO, struct DIP_MEM_INFO_STRUCT)
#endif
