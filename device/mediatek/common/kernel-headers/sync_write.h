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
#ifndef __MT65XX_SYNC_WRITE_H__
#define __MT65XX_SYNC_WRITE_H__
#define mt_reg_sync_writel(v,a) mt65xx_reg_sync_writel(v, a)
#define mt_reg_sync_writew(v,a) mt65xx_reg_sync_writew(v, a)
#define mt_reg_sync_writeb(v,a) mt65xx_reg_sync_writeb(v, a)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define dsb() do { __asm__ __volatile__("dsb sy" : : : "memory"); } while(0)
#define outer_sync() do { int fd; char buf[] = "1"; fd = open("/sys/bus/platform/drivers/outercache/outer_sync", O_WRONLY); if(fd != - 1) { write(fd, buf, strlen(buf)); close(fd); } } while(0)
#define mt65xx_reg_sync_writel(v,a) do { * (volatile unsigned int *) (a) = (v); dsb(); } while(0)
#define mt65xx_reg_sync_writew(v,a) do { * (volatile unsigned short *) (a) = (v); dsb(); } while(0)
#define mt65xx_reg_sync_writeb(v,a) do { * (volatile unsigned char *) (a) = (v); dsb(); } while(0)
#endif
