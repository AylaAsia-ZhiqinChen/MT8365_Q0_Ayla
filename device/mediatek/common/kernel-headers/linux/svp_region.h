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
#ifndef __LINUX_SVP_REGION_H
#define __LINUX_SVP_REGION_H
#define SVP_REGION_IOC_MAGIC 'S'
#define SVP_REGION_IOC_ONLINE _IOR(SVP_REGION_IOC_MAGIC, 2, int)
#define SVP_REGION_IOC_OFFLINE _IOR(SVP_REGION_IOC_MAGIC, 4, int)
#define SVP_REGION_ACQUIRE _IOR(SVP_REGION_IOC_MAGIC, 6, int)
#define SVP_REGION_RELEASE _IOR(SVP_REGION_IOC_MAGIC, 8, int)
#endif
