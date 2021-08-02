/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its
 * licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek
 * Software if you have agreed to and been bound by the applicable license
 * agreement with MediaTek ("License Agreement") and been granted explicit
 * permission to do so within the License Agreement ("Permitted User").
 * If you are not a Permitted User, please cease any access or use of MediaTek
 * Software immediately.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
 * DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE.
 * MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A
 * PARTICULAR STANDARD OR OPEN FORUM.
 * RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 * LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
 /** @file hal_io.cpp
 *  hal_io.cpp provides functions of register access
 */

#include "x_hal_io.h"
#include "dramc_common.h"
#include "dramc_api.h"

extern DRAMC_CTX_T *ps_curr_dram_ctx;

#define ADDR_OFFSET_100		0x100
#define ADDR_OFFSET_200		0x200
#define ADDR_OFFSET_8		0x8

/* IF set, when io_32_write_xxx_all() is invoked,
 * the value will be written to both PHY B01 and
 * PHY B23 even if the data width is 16BIT.
 *
 * Mainly used during initial stage since some
 * RGs (B0_DLL_ARPI0 for example) are defined
 * in both B01/B23, but their content are different,
 * and writting only to B23 may not work.
 * Other hands, it's difficult for SW to decide
 * which Byte shall be written for these RGs.
 * So a simple and safe way (but cause efficient downgrade)
 * is to tell the IO interface to write to both bytes.
 */
static unsigned int sw_broadcast = 0;

void io_set_sw_broadcast(unsigned int value)
{
	sw_broadcast = value;
}

unsigned int io_get_sw_broadcast(void)
{
	return sw_broadcast;
}

unsigned int io_get_set_sw_broadcast(unsigned int value)
{
	unsigned int ret;

	ret = sw_broadcast;
	sw_broadcast = value;

	return ret;
}

/* Although B23 may be used for LP4 and DDR3X16,
 * but some regs are located in B01 only, including
 * CA related registers, PLL related, some MISC
 * related registers.
 * ...
 */
static inline is_reg_in_b01_only(unsigned int addr)
{
	unsigned int is;

	is = 0;

	if (addr < 0x80) {
		is = 1; /* PLL0~PLL8 */
	} else if (addr >= 0x13c && addr < 0x284) {
		is = 1; /* SEL_MAX0 ~ MISC_SPM_CTRL2 */
	} else if (addr >= 0xd00 && addr < 0xe00) {
		is = 1; /* SHU1_CA_CMD0 ~ SHU1_PLL11 */
	} else if (addr >= 0xea0 && addr < 0xf00) {
		is = 1; /* SHU1_RK0_CA_CDM0 ~ SHU1_RK0_CA_CMD10 */
	}

	return is;
}

/* If sw_broadcast is not set (means SW does not want to touch
 * both channels) while B23 is used, the IO shall
 * be mapped to PHY B23.
 */
static inline unsigned int is_to_b23_only(DRAMC_CTX_T *p, unsigned int reg)
{
	unsigned int is;

	is = 0;

	if ((sw_broadcast == 0) &&
		(is_lp4_family(p) ||
		((p->dram_type == TYPE_PCDDR3) &&
		 (p->data_width == DATA_WIDTH_16BIT)))) {
		is = 1;
	}

	return is;
}

#define reg32_type(reg32)	\
	((reg32) & (0xf << POS_BANK_NUM))

unsigned int reg_base_addr_traslate(unsigned char rank, unsigned int reg_addr)
{
	unsigned int offset = reg_addr & 0xffff;
	unsigned int regtype = ((reg_addr - Channel_A_DRAMC_NAO_BASE_VIRTUAL) >>
		POS_BANK_NUM) & 0xf;
	unsigned int baseaddr = 0;

	if (reg_addr < Channel_A_DRAMC_NAO_BASE_VIRTUAL || reg_addr >= MAX_BASE_VIRTUAL)
		return reg_addr;

	if (rank == RANK_1) {
		if (regtype <= 1) {
			/* DRAMC NAO */
			if (offset >= (DRAMC_REG_RK0_DQSOSC_STATUS - DRAMC_NAO_BASE) &&
				offset < (DRAMC_REG_RK1_DQSOSC_STATUS - DRAMC_NAO_BASE)) {
				offset += 0x100;
			}
		} else if (regtype >= 2 && regtype <= 3) {
			/* DRAMC AO */
			if (offset >= (DRAMC_REG_RK0_DQSOSC - DRAMC_AO_BASE) &&
				offset < (DRAMC_REG_RK1_DQSOSC - DRAMC_AO_BASE))
				offset += 0x100;
			else if (offset >= (DRAMC_REG_SHURK0_DQSCTL - DRAMC_AO_BASE) &&
				offset < (DRAMC_REG_SHURK1_DQSCTL - DRAMC_AO_BASE))
				offset += 0x100;
			else if (offset >= (DRAMC_REG_SHU2RK0_DQSCTL - DRAMC_AO_BASE) &&
				offset < (DRAMC_REG_SHU2RK1_DQSCTL - DRAMC_AO_BASE))
				offset += 0x100;
			else if (offset >= (DRAMC_REG_SHU3RK0_DQSCTL - DRAMC_AO_BASE) &&
				offset < (DRAMC_REG_SHU3RK1_DQSCTL - DRAMC_AO_BASE))
				offset += 0x100;
		} else if (regtype >= 4 && regtype <= 5) {
			/* PHY NAO */
		} else {
			/* PHY AO */
			if (offset >= (DDRPHY_R0_B0_RXDVS0 - DDRPHY_BASE_ADDR) &&
				offset < (DDRPHY_R1_B0_RXDVS0 - DDRPHY_BASE_ADDR)) {
				if ((offset >= (DDRPHY_RFU_0X620 - DDRPHY_BASE_ADDR) &&
					offset < (DDRPHY_R0_B1_RXDVS0 - DDRPHY_BASE_ADDR))
					|| (offset >= (DDRPHY_RFU_0X6A0 - DDRPHY_BASE_ADDR) &&
					offset < (DDRPHY_R1_B0_RXDVS0 - DDRPHY_BASE_ADDR))) {
				} else {
					offset += 0x200;
				}
			} else if (offset >= (DDRPHY_SHU_R0_B0_DQ0 - DDRPHY_BASE_ADDR) &&
				offset < (DDRPHY_SHU_R1_B0_DQ0 - DDRPHY_BASE_ADDR)) {
				offset += 0x100;
			} else if (offset >= (DDRPHY_MISC_STBERR_RK0_R - DDRPHY_BASE_ADDR) &&
				offset < (DDRPHY_MISC_STBERR_RK1_R - DDRPHY_BASE_ADDR)) {
				offset += 0x8;
			} else if (offset >= (DDRPHY_R0_B2_RXDVS0 - DDRPHY_BASE_ADDR) &&
				offset < (DDRPHY_R1_B2_RXDVS0 - DDRPHY_BASE_ADDR)) {
					offset += 0x200; /* cc update for B2 */
			} else if (offset >= (DDRPHY_SHU_R0_B2_DQ0 - DDRPHY_BASE_ADDR) &&
				offset < (DDRPHY_SHU_R1_B2_DQ0 - DDRPHY_BASE_ADDR)) {
				offset += 0x100; /* cc update for B2 */
			}
		}
	}

	switch (regtype)
	{
		case 0:
			baseaddr = Channel_A_DRAMC_NAO_BASE_ADDRESS;
			break;
		case 1:
			baseaddr = Channel_B_DRAMC_NAO_BASE_ADDRESS;
			break;
		case 2:
			baseaddr = Channel_A_DRAMC_AO_BASE_ADDRESS;
			break;
		case 3:
			baseaddr = Channel_B_DRAMC_AO_BASE_ADDRESS;
			break;
		case 4:
			baseaddr = Channel_A_PHY_NAO_BASE_ADDRESS;
			break;
		case 5:
			baseaddr = Channel_B_PHY_NAO_BASE_ADDRESS;
			break;
		case 6:
			baseaddr = Channel_A_PHY_AO_BASE_ADDRESS;
			break;
		case 7:
			baseaddr = Channel_B_PHY_AO_BASE_ADDRESS;
			break;
	}

	return (baseaddr + offset);
	/*
	 * show_msg2((INFO, "\n[regbaseaddrtraslate]  0x%x => 0x%x(0x%x, 0x%x)", reg_addr , (arRegBaseAddrList[regtype] +offset), arRegBaseAddrList[regtype], offset));
	 * return (arRegBaseAddrList[regtype] +offset);
	 */
}

unsigned int dram_register_read(unsigned int reg_addr)
{
	unsigned int reg_value;

	reg_addr = reg_base_addr_traslate(ps_curr_dram_ctx->rank, reg_addr);

#if (FOR_DV_SIMULATION_USED == 1)
	reg_value = register_read_c(reg_addr);
#else
	reg_value = (*(volatile unsigned int *)(reg_addr));
#endif

	return reg_value;
}

unsigned char dram_register_write(unsigned int reg_addr,
unsigned int reg_value)
{
	unsigned char ucstatus;

	ucstatus = 0;

	reg_addr = reg_base_addr_traslate(ps_curr_dram_ctx->rank, reg_addr);

#if (FOR_DV_SIMULATION_USED == 1)
	register_write_c(reg_addr, reg_value);
#else
	(*(volatile unsigned int *)reg_addr) = reg_value;
#endif
	dsb();

	return ucstatus;
}

void io32_write_4b_msk2(unsigned int reg32,
	unsigned int val32, unsigned int msk32)
{
	unsigned int u4Val;

	val32 &= msk32;

	u4Val = dram_register_read(reg32);
	u4Val = ((u4Val & ~msk32) | val32);
	dram_register_write(reg32, u4Val);
}

void io32_write_4b_all2(unsigned int reg32, unsigned int val32)
{
	unsigned char ii, all_count;
	unsigned int reg_type = reg32_type(reg32);

	reg32 &= 0xffff;
	all_count = CHANNEL_NUM;

	if(reg_type >=Channel_A_PHY_AO_BASE_VIRTUAL)
		reg32 += Channel_A_PHY_AO_BASE_VIRTUAL;
	else if(reg_type >=Channel_A_DRAMC_AO_BASE_VIRTUAL)
		reg32 += Channel_A_DRAMC_AO_BASE_VIRTUAL;
	else
		reg32 += Channel_A_DRAMC_NAO_BASE_VIRTUAL;

	for (ii = 0; ii < all_count; ii++) {
		io32_write_4b(reg32 + ((unsigned int) ii << POS_BANK_NUM), val32);
	}

}

void io32_write_4b_msk_all2(unsigned int reg32,
	unsigned int val32, unsigned int msk32)
{
	unsigned int u4Val;
	unsigned char ii, all_count;
	unsigned int reg_type = reg32_type(reg32);

	reg32 &= 0xffff;
	all_count = CHANNEL_NUM;

	if(reg_type >= Channel_A_PHY_AO_BASE_VIRTUAL)
		reg32 += Channel_A_PHY_AO_BASE_VIRTUAL;
	else if(reg_type >=Channel_A_DRAMC_AO_BASE_VIRTUAL)
		reg32 += Channel_A_DRAMC_AO_BASE_VIRTUAL;
	else
		reg32 += Channel_A_DRAMC_NAO_BASE_VIRTUAL;

	for (ii = 0; ii < all_count; ii++) {
		u4Val = dram_register_read(reg32 +
			((unsigned int) ii << POS_BANK_NUM));
		u4Val = ((u4Val & ~msk32) | val32);
		dram_register_write(reg32 +
			((unsigned int) ii << POS_BANK_NUM), u4Val);
	}
}

void io_32_read_fld_align_phy_byte_2(unsigned char byte_idx,
	unsigned int reg32, unsigned int fld)
{
	unsigned int offset = 0;
	unsigned int except_offset = 0;

	if (reg32 < Channel_A_PHY_AO_BASE_VIRTUAL) {
		show_msg2((INFO, "\n[write_fld_align_phy_byte_2] wrong addr 0x%x\n", reg32));
		return;
	}

	reg32 &= 0xffff;
	if ((reg32 >= (DDRPHY_SHU_R0_B0_DQ0 - DDRPHY_BASE_ADDR)) &&
		(reg32 < (DDRPHY_SHU_R0_B1_DQ0 - DDRPHY_BASE_ADDR))) {
		offset = 0x50;

		if ((reg32 >= (DDRPHY_SHU_R0_B0_DQ0 - DDRPHY_BASE_ADDR)) &&
			(reg32 < (DDRPHY_SHU_R0_B0_DQ6-DDRPHY_BASE_ADDR))) {
			except_offset = 0x0;
		} else {
			except_offset = 0x8;
		}
	} else if ((reg32 >= (DDRPHY_R0_B0_RXDVS0 - DDRPHY_BASE_ADDR)) &&
		(reg32 < (DDRPHY_R0_B1_RXDVS0 - DDRPHY_BASE_ADDR))) {
		offset = 0x80;
		except_offset = 0x0;
	} else if ((reg32 >= (DDRPHY_SHU_B0_DQ0-DDRPHY_BASE_ADDR)) &&
		(reg32 < (DDRPHY_SHU_B1_DQ0-DDRPHY_BASE_ADDR))) {
		offset = 0x80;
		except_offset = 0;
	} else if ((reg32 >= (DDRPHY_B0_DLL_ARPI0 - DDRPHY_BASE_ADDR)) &&
		(reg32 < (DDRPHY_B1_DLL_ARPI0-DDRPHY_BASE_ADDR))) {
		offset = 0x80;
		except_offset = 0;
	} else {
		offset = 0x0;
		except_offset = 0;
	}

	switch (byte_idx) {
		case 0:
			return io_32_read_fld_align(reg32 +
				Channel_B_PHY_AO_BASE_VIRTUAL + offset, fld); //CHB_B1
			break;
		case 1:
			if ((reg32 == (DDRPHY_SHU_R0_B0_DQ7 & 0xffff)) &&
				(fld == SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0)) {
				fld = SHU_R0_CA_CMD9_RG_RK0_ARPI_CS;
			}
			return io_32_read_fld_align(reg32 + Channel_B_PHY_AO_BASE_VIRTUAL +
				(offset << 1) + except_offset, fld); //CHB_CA
			break;
		case 2:
			return io_32_read_fld_align(reg32 +
				Channel_A_PHY_AO_BASE_VIRTUAL + offset, fld); //CHA_B1
			break;
		case 3:
			return io_32_read_fld_align(reg32 +
				Channel_B_PHY_AO_BASE_VIRTUAL, fld); //CHB_B0
			break;
		default:
			show_msg2((INFO, "\n[WriteFldAlign_Phy_Byte] wrong index of ucByteIdx !!!\n"));
			break;
	}
}

void io_32_write_fld_align_phy_byte_2(unsigned char byte_idx,
	unsigned int reg32, unsigned int val, unsigned int fld)
{
	unsigned int offset = 0;
	unsigned int except_offset = 0;

	if (reg32 < Channel_A_PHY_AO_BASE_VIRTUAL) {
		show_msg2((INFO, "\n[write_fld_align_phy_byte_2] wrong addr 0x%x\n", reg32));
		return;
	}

	reg32 &= 0xffff;
	if ((reg32 >= (DDRPHY_SHU_R0_B0_DQ0 - DDRPHY_BASE_ADDR)) &&
		(reg32 < (DDRPHY_SHU_R0_B1_DQ0 - DDRPHY_BASE_ADDR))) {
		offset = 0x50;

		if ((reg32 >= (DDRPHY_SHU_R0_B0_DQ0 - DDRPHY_BASE_ADDR)) &&
			(reg32 < (DDRPHY_SHU_R0_B0_DQ6-DDRPHY_BASE_ADDR))) {
			except_offset = 0x0;
		} else {
			except_offset = 0x8;
		}
	} else if ((reg32 >= (DDRPHY_R0_B0_RXDVS0 - DDRPHY_BASE_ADDR)) &&
		(reg32 < (DDRPHY_R0_B1_RXDVS0 - DDRPHY_BASE_ADDR))) {
		offset = 0x80;
		except_offset = 0x0;
	} else if ((reg32 >= (DDRPHY_SHU_B0_DQ0-DDRPHY_BASE_ADDR)) &&
		(reg32 < (DDRPHY_SHU_B1_DQ0-DDRPHY_BASE_ADDR))) {
		offset = 0x80;
		except_offset = 0;
	} else if ((reg32 >= (DDRPHY_B0_DLL_ARPI0 - DDRPHY_BASE_ADDR)) &&
		(reg32 < (DDRPHY_B1_DLL_ARPI0-DDRPHY_BASE_ADDR))) {
		offset = 0x80;
		except_offset = 0;
	} else {
		offset = 0x0;
		except_offset = 0;
	}

	switch (byte_idx) {
		case 0:
			io_32_write_fld_align(reg32 + Channel_B_PHY_AO_BASE_VIRTUAL + offset,
				val, fld); //CHB_B1
			break;
		case 1:
			if ((reg32 == (DDRPHY_SHU_R0_B0_DQ7 & 0xffff)) &&
				(fld == SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0)) {
				fld = SHU_R0_CA_CMD9_RG_RK0_ARPI_CS;
			}
			io_32_write_fld_align(reg32 + Channel_B_PHY_AO_BASE_VIRTUAL +
				(offset << 1) + except_offset, val, fld); //CHB_CA
			break;
		case 2:
			io_32_write_fld_align(reg32 + Channel_A_PHY_AO_BASE_VIRTUAL + offset,
				val, fld); //CHA_B1
			break;
		case 3:
			io_32_write_fld_align(reg32 + Channel_B_PHY_AO_BASE_VIRTUAL,
				val, fld); //CHB_B0
			break;
		default:
			show_msg2((INFO, "\n[WriteFldAlign_Phy_Byte] wrong index of ucByteIdx !!!\n"));
			break;
	}
}

void io_32_write_fld_align_phy_all_2(unsigned int reg32,
	unsigned int val, unsigned int fld)
{
	unsigned char ch_idx;

	if (reg32 < Channel_A_PHY_AO_BASE_VIRTUAL) {
		show_msg2((INFO, "\n[write_fld_align_phy_all_2] wrong addr 0x%x\n", reg32));
		return;
	}

	reg32 &= 0xffff;

	for (ch_idx = 0; ch_idx < CHANNEL_NUM; ch_idx++) {
		io_32_write_fld_align(reg32 + Channel_A_PHY_AO_BASE_VIRTUAL +
			(ch_idx << POS_BANK_NUM), val, fld);
	}
}


