/*
 * Copyright (c) 2015-2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __BOARD_ARM_DEF_H__
#define __BOARD_ARM_DEF_H__

#include <v2m_def.h>


/*
 * Required platform porting definitions common to all ARM
 * development platforms
 */

/* Size of cacheable stacks */
#if defined(IMAGE_BL1)
#if TRUSTED_BOARD_BOOT
# define PLATFORM_STACK_SIZE 0x1000
#else
# define PLATFORM_STACK_SIZE 0x440
#endif
#elif defined(IMAGE_BL2)
# if TRUSTED_BOARD_BOOT
#  define PLATFORM_STACK_SIZE 0x1000
# else
#  define PLATFORM_STACK_SIZE 0x400
# endif
#elif defined(IMAGE_BL2U)
# define PLATFORM_STACK_SIZE 0x200
#elif defined(IMAGE_BL31)
# define PLATFORM_STACK_SIZE 0x400
#elif defined(IMAGE_BL32)
# define PLATFORM_STACK_SIZE 0x440
#endif

/*
 * The constants below are not optimised for memory usage. Platforms that wish
 * to optimise these constants should set `ARM_BOARD_OPTIMISE_MEM` to 1 and
 * provide there own values.
 */
#if !ARM_BOARD_OPTIMISE_MEM
/*
 * PLAT_ARM_MMAP_ENTRIES depends on the number of entries in the
 * plat_arm_mmap array defined for each BL stage.
 *
 * Provide relatively optimised values for the runtime images (BL31 and BL32).
 * Optimisation is less important for the other, transient boot images so a
 * common, maximum value is used across these images.
 *
 * They are also used for the dynamically mapped regions in the images that
 * enable dynamic memory mapping.
 */
#if defined(IMAGE_BL31) || defined(IMAGE_BL32)
# define PLAT_ARM_MMAP_ENTRIES		6
# define MAX_XLAT_TABLES		4
#else
# define PLAT_ARM_MMAP_ENTRIES		10
# define MAX_XLAT_TABLES		5
#endif

/*
 * PLAT_ARM_MAX_BL1_RW_SIZE is calculated using the current BL1 RW debug size
 * plus a little space for growth.
 */
#define PLAT_ARM_MAX_BL1_RW_SIZE	0xA000

/*
 * PLAT_ARM_MAX_BL2_SIZE is calculated using the current BL2 debug size plus a
 * little space for growth.
 */
#if TRUSTED_BOARD_BOOT
# define PLAT_ARM_MAX_BL2_SIZE		0x1D000
#else
# define PLAT_ARM_MAX_BL2_SIZE		0xF000
#endif

/*
 * PLAT_ARM_MAX_BL31_SIZE is calculated using the current BL31 debug size plus a
 * little space for growth.
 */
#define PLAT_ARM_MAX_BL31_SIZE		0x1D000

#endif /* ARM_BOARD_OPTIMISE_MEM */

#define MAX_IO_DEVICES			3
#define MAX_IO_HANDLES			4

#define PLAT_ARM_TRUSTED_SRAM_SIZE	0x00040000	/* 256 KB */

#define PLAT_ARM_FIP_BASE		V2M_FLASH0_BASE
#define PLAT_ARM_FIP_MAX_SIZE		V2M_FLASH0_SIZE

#define PLAT_ARM_NVM_BASE		V2M_FLASH0_BASE
#define PLAT_ARM_NVM_SIZE		V2M_FLASH0_SIZE


#endif /* __BOARD_ARM_DEF_H__ */
