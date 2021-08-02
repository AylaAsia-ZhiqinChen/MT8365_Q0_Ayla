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
#ifndef CAMERA_SYSRAM_H
#define CAMERA_SYSRAM_H
#define SYSRAM_DEV_NAME "camera-sysram"
#define SYSRAM_MAGIC_NO 'p'
typedef enum {
  SYSRAM_USER_VIDO,
  SYSRAM_USER_GDMA,
  SYSRAM_USER_SW_FD,
  SYSRAM_USER_AMOUNT,
  SYSRAM_USER_NONE
} SYSRAM_USER_ENUM;
typedef struct {
  unsigned long Alignment;
  unsigned long Size;
  SYSRAM_USER_ENUM User;
  unsigned long Addr;
  unsigned long TimeoutMS;
} SYSRAM_ALLOC_STRUCT;
typedef enum {
  SYSRAM_CMD_ALLOC,
  SYSRAM_CMD_FREE,
  SYSRAM_CMD_DUMP
} SYSRAM_CMD_ENUM;
#define SYSRAM_ALLOC _IOWR(SYSRAM_MAGIC_NO, SYSRAM_CMD_ALLOC, SYSRAM_ALLOC_STRUCT)
#define SYSRAM_FREE _IOWR(SYSRAM_MAGIC_NO, SYSRAM_CMD_FREE, SYSRAM_USER_ENUM)
#define SYSRAM_DUMP _IO(SYSRAM_MAGIC_NO, SYSRAM_CMD_DUMP)
#endif
