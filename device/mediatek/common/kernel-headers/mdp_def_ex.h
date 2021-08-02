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
#ifndef __MDP_DEF_EX_H__
#define __MDP_DEF_EX_H__
#include <linux/kernel.h>
#define MDP_LIMIT_DRIVER_DEVICE_NAME "mtk_mdp_limit"
enum CMDQ_META_OP {
  CMDQ_MOP_WRITE = 0,
  CMDQ_MOP_READ,
  CMDQ_MOP_POLL,
  CMDQ_MOP_WAIT,
  CMDQ_MOP_WAIT_NO_CLEAR,
  CMDQ_MOP_CLEAR,
  CMDQ_MOP_SET,
  CMDQ_MOP_ACQUIRE,
  CMDQ_MOP_WRITE_FD,
  CMDQ_MOP_WRITE_FROM_REG,
  CMDQ_MOP_WRITE_SEC,
  CMDQ_MOP_NOP,
};
#define CMDQ_EVENT_WAIT 0x80008001
#define CMDQ_EVENT_WAIT_NO_CLEAR 0x00008001
#define CMDQ_EVENT_CLEAR 0x80000000
#define CMDQ_EVENT_SET 0x80010000
#define CMDQ_EVENT_ACQUIRE 0x80018000
struct op_meta {
  uint8_t op;
  uint16_t engine;
  union {
    uint16_t offset;
    uint16_t event;
  };
  union {
    uint32_t value;
    uint32_t readback_id;
    uint32_t fd;
    uint32_t sec_handle;
    struct {
      uint16_t from_offset;
      uint16_t from_engine;
    };
  };
  union {
    uint32_t mask;
    uint32_t fd_offset;
    uint32_t sec_index;
  };
};
struct hw_meta {
  uint16_t offset;
  uint16_t engine;
};
struct mdp_submit {
  cmdqU32Ptr_t metas;
  uint32_t meta_count;
  uint32_t priority;
  uint64_t engine_flag;
  uint32_t prop_size;
  uint64_t prop_addr;
  uint64_t readback_ext;
  uint32_t read_count_v1;
  cmdqU32Ptr_t hw_metas_read_v1;
  uint64_t job_id;
  struct cmdqSecDataStruct secData;
};
struct mdp_read_v1 {
  uint32_t count;
  cmdqU32Ptr_t ret_values;
};
struct mdp_read_readback {
  uint32_t count;
  cmdqU32Ptr_t ids;
  cmdqU32Ptr_t ret_values;
};
struct mdp_wait {
  uint64_t job_id;
  struct mdp_read_v1 read_v1_result;
  struct mdp_read_readback read_result;
};
struct mdp_readback {
  uint32_t count;
  uint32_t start_id;
};
#define CMDQ_IOCTL_MAGIC_NUMBER 'x'
#define CMDQ_IOCTL_ASYNC_EXEC _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 20, struct mdp_submit)
#define CMDQ_IOCTL_ASYNC_WAIT _IOR(CMDQ_IOCTL_MAGIC_NUMBER, 21, struct mdp_wait)
#define CMDQ_IOCTL_ALLOC_READBACK_SLOTS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 22, struct mdp_readback)
#define CMDQ_IOCTL_FREE_READBACK_SLOTS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 23, struct mdp_readback)
#define CMDQ_IOCTL_READ_READBACK_SLOTS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 24, struct mdp_read_readback)
#endif
