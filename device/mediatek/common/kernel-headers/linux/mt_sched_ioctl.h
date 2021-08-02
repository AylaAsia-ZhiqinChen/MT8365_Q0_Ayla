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
#ifndef _MT_SCHED_IOCTL_H
#define _MT_SCHED_IOCTL_H
struct ioctl_arg {
  pid_t pid;
  unsigned int len;
  unsigned long * mask;
  unsigned long * mt_mask;
};
#define IOC_MAGIC '\x66'
#define IOCTL_SETAFFINITY _IOW(IOC_MAGIC, 0, struct ioctl_arg)
#define IOCTL_EXITAFFINITY _IOW(IOC_MAGIC, 1, pid_t)
#define IOCTL_GETAFFINITY _IOR(IOC_MAGIC, 2, struct ioctl_arg)
#define IOCTL_SETVIP _IOW(IOC_MAGIC, 3, pid_t)
#define IOCTL_UNSETVIP _IOW(IOC_MAGIC, 4, pid_t)
#endif
