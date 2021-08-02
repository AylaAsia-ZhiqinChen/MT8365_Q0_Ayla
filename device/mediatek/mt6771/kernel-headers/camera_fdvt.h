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
#ifndef __CAMERA_FDVT_H__
#define __CAMERA_FDVT_H__
#include <linux/ioctl.h>
#define FDVT_IOC_MAGIC 'N'
#define SIG_ERESTARTSYS 512
struct FDVTRegIO {
  unsigned int * pAddr;
  unsigned int * pData;
  unsigned int u4Count;
};
#define FDVTRegIO struct FDVTRegIO
#define FDVT_IOC_INIT_SETPARA_CMD _IO(FDVT_IOC_MAGIC, 0x00)
#define FDVT_IOC_STARTFD_CMD _IO(FDVT_IOC_MAGIC, 0x01)
#define FDVT_IOC_G_WAITIRQ _IOR(FDVT_IOC_MAGIC, 0x02, unsigned int)
#define FDVT_IOC_T_SET_FDCONF_CMD _IOW(FDVT_IOC_MAGIC, 0x03, FDVTRegIO)
#define FDVT_IOC_G_READ_FDREG_CMD _IOWR(FDVT_IOC_MAGIC, 0x04, FDVTRegIO)
#define FDVT_IOC_T_SET_SDCONF_CMD _IOW(FDVT_IOC_MAGIC, 0x05, FDVTRegIO)
#define FDVT_IOC_T_DUMPREG _IO(FDVT_IOC_MAGIC, 0x80)
#endif
