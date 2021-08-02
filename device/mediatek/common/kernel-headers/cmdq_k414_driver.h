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
#ifndef __CMDQ_DRIVER_H__
#define __CMDQ_DRIVER_H__
#include <linux/kernel.h>
#include "cmdq_k414_def.h"
struct cmdqUsageInfoStruct {
  uint32_t count[CMDQ_MAX_ENGINE_COUNT];
};
struct cmdqJobStruct {
  struct cmdqCommandStruct command;
  cmdqJobHandle_t hJob;
};
struct cmdqJobResultStruct {
  cmdqJobHandle_t hJob;
  uint64_t engineFlag;
  struct cmdqRegValueStruct regValue;
  struct cmdqReadAddressStruct readAddress;
};
struct cmdqWriteAddressStruct {
  uint32_t count;
  uint32_t startPA;
};
#define CMDQ_IOCTL_MAGIC_NUMBER 'x'
#define CMDQ_IOCTL_LOCK_MUTEX _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 1, int)
#define CMDQ_IOCTL_UNLOCK_MUTEX _IOR(CMDQ_IOCTL_MAGIC_NUMBER, 2, int)
#define CMDQ_IOCTL_EXEC_COMMAND _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 3, struct cmdqCommandStruct)
#define CMDQ_IOCTL_QUERY_USAGE _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 4, struct cmdqUsageInfoStruct)
#define CMDQ_IOCTL_ASYNC_JOB_EXEC _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 5, struct cmdqJobStruct)
#define CMDQ_IOCTL_ASYNC_JOB_WAIT_AND_CLOSE _IOR(CMDQ_IOCTL_MAGIC_NUMBER, 6, struct cmdqJobResultStruct)
#define CMDQ_IOCTL_ALLOC_WRITE_ADDRESS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 7, struct cmdqWriteAddressStruct)
#define CMDQ_IOCTL_FREE_WRITE_ADDRESS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 8, struct cmdqWriteAddressStruct)
#define CMDQ_IOCTL_READ_ADDRESS_VALUE _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 9, struct cmdqReadAddressStruct)
#define CMDQ_IOCTL_QUERY_CAP_BITS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 10, int)
#define CMDQ_IOCTL_QUERY_DTS _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 11, struct cmdqDTSDataStruct)
#define CMDQ_IOCTL_NOTIFY_ENGINE _IOW(CMDQ_IOCTL_MAGIC_NUMBER, 12, uint64_t)
#endif
