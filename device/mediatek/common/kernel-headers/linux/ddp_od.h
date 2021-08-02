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
#ifndef __DDP_OD_H__
#define __DDP_OD_H__
#include <linux/ioctl.h>
typedef struct {
  unsigned int size;
  unsigned int type;
  unsigned int ret;
  unsigned int param0;
  unsigned int param1;
  unsigned int param2;
  unsigned int param3;
} DISP_OD_CMD;
typedef enum {
  OD_CTL_READ_REG,
  OD_CTL_WRITE_REG,
  OD_CTL_ENABLE_DEMO_MODE,
  OD_CTL_RUN_TEST,
  OD_CTL_WRITE_TABLE,
  OD_CTL_CMD_NUM,
  OD_CTL_ENABLE
} DISP_OD_CMD_TYPE;
typedef enum {
  OD_CTL_ENABLE_OFF,
  OD_CTL_ENABLE_ON
} DISP_OD_ENABLE_STAGE;
#define OD_CTL_ENABLE_DELAY 3
#define DISP_IOCTL_MAGIC 'x'
#define DISP_IOCTL_OD_CTL _IOWR(DISP_IOCTL_MAGIC, 80, DISP_OD_CMD)
#define DISP_IOCTL_OD_SET_ENABLED _IOWR(DISP_IOCTL_MAGIC, 81, int)
#endif
