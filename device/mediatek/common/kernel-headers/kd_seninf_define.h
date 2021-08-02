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
#ifndef _KD_SENINF_DEFINE_H_
#define _KD_SENINF_DEFINE_H_
#ifndef _MEDIA_TYPES_H
typedef unsigned char MUINT8;
typedef unsigned short MUINT16;
typedef unsigned int MUINT32;
typedef signed char MINT8;
typedef signed short MINT16;
typedef signed int MINT32;
#endif
struct KD_SENINF_MMAP {
  MUINT32 map_addr;
  MUINT32 map_length;
};
struct KD_SENINF_REG {
  struct KD_SENINF_MMAP seninf;
  struct KD_SENINF_MMAP ana;
  struct KD_SENINF_MMAP gpio;
};
#endif
