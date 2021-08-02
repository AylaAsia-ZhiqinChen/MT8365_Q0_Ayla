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
#ifndef _CAM_CAL_DATA_H
#define _CAM_CAL_DATA_H

#ifdef CONFIG_COMPAT
/* 64 bit */
#include <linux/fs.h>
#include <linux/compat.h>
#endif

struct stCAM_CAL_INFO_STRUCT {
  u32 u4Offset;
  u32 u4Length;
  u32 sensorID;
  /*
   * MAIN = 0x01,
   * SUB  = 0x02,
   * MAIN_2 = 0x04,
   * SUB_2 = 0x08,
   * MAIN_3 = 0x10,
   */
  u32 deviceID;
  u8 *pu1Params;
};

#ifdef CONFIG_COMPAT

struct COMPAT_stCAM_CAL_INFO_STRUCT {
  u32 u4Offset;
  u32 u4Length;
  u32 sensorID;
  u32 deviceID;
  compat_uptr_t pu1Params;
};
#endif

#endif/*_CAM_CAL_DATA_H*/
