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
#ifndef _KD_SENINF_H_
#define _KD_SENINF_H_
#include "kd_imgsensor_define.h"
#include "kd_seninf_define.h"
#define SENINFMAGIC 's'
#define KDSENINFIOC_X_GET_REG_ADDR _IOWR(SENINFMAGIC, 40, struct KD_SENINF_REG)
#define KDSENINFIOC_X_SET_MCLK_PLL _IOWR(SENINFMAGIC, 60, ACDK_SENSOR_MCLK_STRUCT)
#define KDSENINFIOC_X_GET_ISP_CLK _IOWR(SENINFMAGIC, 80, u32)
#define KDSENINFIOC_X_GET_CSI_CLK _IOWR(SENINFMAGIC, 85, u32)
#define KDSENINFIOC_X_SECURE_DUMP _IO(SENINFMAGIC, 90)

/* Get/Set ISP CLK via MMDVFS */
/* Get ISP CLK via MMDVFS*/
#define KDSENINFIOC_DFS_UPDATE               _IOWR(SENINFMAGIC, 95, unsigned int)
#define KDSENINFIOC_GET_SUPPORTED_ISP_CLOCKS _IOWR(SENINFMAGIC, 100, IMAGESENSOR_GET_SUPPORTED_ISP_CLK)
#define KDSENINFIOC_GET_CUR_ISP_CLOCK        _IOWR(SENINFMAGIC, 105, unsigned int)

#endif
