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
#ifndef __CCCI_IPC_TASK_ID_H__
#define __CCCI_IPC_TASK_ID_H__
#ifdef __IPC_ID_TABLE
#define X_IPC_MODULE_CONF(a,b,c,d) { c, b },
#else
#define X_IPC_MODULE_CONF(a,b,c,d)
#endif
#define AP_UNIFY_ID_FLAG (1 << 31)
#define MD_UNIFY_ID_FLAG (0 << 31)
#define AGPS_MD_MOD_L4C 0
#define AGPS_MD_MOD_L4C_2 1
#define AGPS_MD_MOD_L4C_3 2
#define AGPS_MD_MOD_L4C_4 3
#define AGPS_AP_MOD_MMI 0
#define GPS_AP_MOD 2
#endif
