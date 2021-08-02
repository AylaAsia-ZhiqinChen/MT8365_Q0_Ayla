/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2003-2007 by Tensilica Inc.  ALL RIGHTS RESERVED.
 *
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any adapted
 * or modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the prior
 * written consent of Tensilica Inc.
 */

/*
 * This file provides Xtensa processor configuration
 * parameters that can be used in XTMP simulations.
 */

#ifndef __XTMP_CONFIG_included__
#define __XTMP_CONFIG_included__

#define XTMP_CONFIG_NAME		"hifi4_Aquila_E2_PROD"
#define XTMP_BYTE_ORDERING		LE
#define XTMP_IS_LITTLE_ENDIAN		1
#define XTMP_IS_BIG_ENDIAN		0
#define XTMP_PIF_BYTES			8
#define XTMP_PIF_BITS			64
#define XTMP_BIT_WIDTH			XTMP_PIF_BITS	/* DEPRECATED */
#define XTMP_LOAD_STORE_BITS		64
#define XTMP_IFETCH_BITS		128
#define XTMP_HAS_PIF			1
#define XTMP_HAS_PRID			1
#define XTMP_HAS_DCACHE			1
#define XTMP_DCACHE_SIZE		32768
#define XTMP_DCACHE_WAYS		4
#define XTMP_DCACHE_WRITE_BACK		1
#define XTMP_DCACHE_LINE_BYTES		128

#define XTMP_HAS_SYS_ROM		0
#define XTMP_SYS_ROM_PADDR		-1
#define XTMP_SYS_ROM_VADDR		-1
#define XTMP_SYS_ROM_SIZE		0

#define XTMP_HAS_SYS_RAM		1
#define XTMP_SYS_RAM_PADDR		0x40020000
#define XTMP_SYS_RAM_VADDR		0x40020000
#define XTMP_SYS_RAM_SIZE		0x80000000

#define XTMP_HAS_XLMI			0
#define XTMP_XLMI_PADDR			-1
#define XTMP_XLMI_VADDR			-1
#define XTMP_XLMI_SIZE			0
#define XTMP_XLMI_BUSY			0
#define XTMP_XLMI_INBOUND		0

#define XTMP_HAS_DATA_RAM		1
#define XTMP_NUM_DATA_RAMS		2
#define XTMP_DATA_RAM0_PADDR		0x1E000000
#define XTMP_DATA_RAM0_VADDR		0x1E000000
#define XTMP_DATA_RAM0_SIZE		0x00040000
#define XTMP_DATA_RAM0_BUSY		1
#define XTMP_DATA_RAM0_INBOUND		1
#define XTMP_DATA_RAM1_PADDR		0x1E040000
#define XTMP_DATA_RAM1_VADDR		0x1E040000
#define XTMP_DATA_RAM1_SIZE		0x00040000
#define XTMP_DATA_RAM1_BUSY		1
#define XTMP_DATA_RAM1_INBOUND		1

#define XTMP_HAS_DATA_ROM		0
#define XTMP_NUM_DATA_ROMS		0
#define XTMP_DATA_ROM0_PADDR		-1
#define XTMP_DATA_ROM0_VADDR		-1
#define XTMP_DATA_ROM0_SIZE		0
#define XTMP_DATA_ROM0_BUSY		0

#define XTMP_HAS_INST_RAM		1
#define XTMP_NUM_INST_RAMS		2
#define XTMP_INST_RAM0_PADDR		0x40000000
#define XTMP_INST_RAM0_VADDR		0x40000000
#define XTMP_INST_RAM0_SIZE		0x00010000
#define XTMP_INST_RAM0_BUSY		1
#define XTMP_INST_RAM0_INBOUND		1
#define XTMP_INST_RAM1_PADDR		0x40010000
#define XTMP_INST_RAM1_VADDR		0x40010000
#define XTMP_INST_RAM1_SIZE		0x00010000
#define XTMP_INST_RAM1_BUSY		1
#define XTMP_INST_RAM1_INBOUND		1

#define XTMP_HAS_INST_ROM		0
#define XTMP_NUM_INST_ROMS		0
#define XTMP_INST_ROM0_PADDR		-1
#define XTMP_INST_ROM0_VADDR		-1
#define XTMP_INST_ROM0_SIZE		0
#define XTMP_INST_ROM0_BUSY		0

#define XTMP_HAS_BYPASS_REGION		1
#define XTMP_BYPASS_PADDR		0x20000000
#define XTMP_BYPASS_VADDR		0x20000000
#define XTMP_BYPASS_PSIZE		0x20000000

#define XTMP_RESET_VECTOR_VADDR		0x40000640

#define XTMP_NUM_INTERRUPTS		25
#define XTMP_NUM_EXTERNAL_INTERRUPTS	18

#define XTMP_R_STAGE			0
#define XTMP_E_STAGE			1
#define XTMP_M_STAGE			3
#define XTMP_W_STAGE			4

#define _xim1(w,e,n)		XTMP_insert ## w ## e ## n
#define _xim0(w,e,n)		_xim1(w,e,n)
#define XTMP_PIF_INSERT(n)	_xim0(XTMP_PIF_BITS,XTMP_BYTE_ORDERING,n)
#define XTMP_INSERT(n)		XTMP_PIF_INSERT(n)	/* DEPRECATED */
#define XTMP_LS_INSERT(n)	_xim0(XTMP_LOAD_STORE_BITS,XTMP_BYTE_ORDERING,n)
#define XTMP_IF_INSERT(n)	_xim0(XTMP_IFETCH_BITS,XTMP_BYTE_ORDERING,n)
#define _xem1(w,e,n)		XTMP_extract ## w ## e ## n
#define _xem0(w,e,n)		_xem1(w,e,n)
#define XTMP_PIF_EXTRACT(n)	_xem0(XTMP_PIF_BITS,XTMP_BYTE_ORDERING,n)
#define XTMP_EXTRACT(n)		XTMP_PIF_EXTRACT(n)	/* DEPRECATED */
#define XTMP_LS_EXTRACT(n)	_xem0(XTMP_LOAD_STORE_BITS,XTMP_BYTE_ORDERING,n)
#define XTMP_IF_EXTRACT(n)	_xem0(XTMP_IFETCH_BITS,XTMP_BYTE_ORDERING,n)

#endif /* __XTMP_CONFIG_included */
