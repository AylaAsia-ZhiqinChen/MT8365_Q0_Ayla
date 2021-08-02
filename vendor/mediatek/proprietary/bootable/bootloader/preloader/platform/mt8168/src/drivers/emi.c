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
/* Include Files */

#include "dramc_common.h"
//#include "reg_emi_reg.h"
//#include "reg_chn_emi0.h"
#include "dramc_register.h"
#include "platform.h"
#include "pl_version.h"

#include "emi.h"
#ifdef LASTPC_READY
#include "lastpc.h"
#endif

#if CFG_BOOT_ARGUMENT
#define bootarg g_dram_buf->bootarg
#endif

static unsigned int get_dramc_addr(dram_addr_t *dram_addr,
	unsigned int offset);

/*  Global Variables */
int emi_setting_index = -1;
extern EMI_SETTINGS default_emi_setting;

/*  External references */
extern char *opt_dle_value;

#define CQ_DMA_G_DMA_CON 0x018
#define CQ_DMA_G_DMA_SRC_ADDR 0x01c
#define CQ_DMA_G_DMA_SRC_ADDR2 0x060
#define CQ_DMA_G_DMA_DST_ADDR 0x020
#define CQ_DMA_G_DMA_DST_ADDR2 0x064
#define CQ_DMA_G_DMA_LEN1 0x024
#define CQ_DMA_G_DMA_EN 0x008

//#define print printf

#define PHY_ADDR_OFFSET 0x40000000

void EMI_ESL_Setting1(void)
{
	dsb();
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);;
	dsb();
#if (CHANNEL_NUM==4)
	*((volatile unsigned *)(EMI_CONA)) = 0xf050f254;
#else
	*((volatile unsigned *)(EMI_CONA)) = 0xf050f154;
#endif
	*((volatile unsigned *)(EMI_CONB)) = 0x17283544;
	*((volatile unsigned *)(EMI_CONC)) = 0x0a1a0b1a;
	*((volatile unsigned *)(EMI_COND)) = 0x3657587a;
	*((volatile unsigned *)(EMI_CONE)) = 0x0000c042;
	*((volatile unsigned *)(EMI_CONG)) = 0x2b2b2a38;
#if (CHANNEL_NUM == 4)
	*((volatile unsigned *)(EMI_CONH)) = 0x44440003;
#else
	*((volatile unsigned *)(EMI_CONH)) = 0x44440003;
#endif
	*((volatile unsigned *)(EMI_CONM)) = 0x007811ff;
	*((volatile unsigned *)(EMI_CONN)) = 0x00000000;
	*((volatile unsigned *)(EMI_MDCT)) = 0x11120c1f;
	*((volatile unsigned *)(EMI_SHF0)) = 0x11120c1f;
	*((volatile unsigned *)(EMI_MDCT_2ND)) = 0x00001123;
	*((volatile unsigned *)(EMI_SHF1)) = 0x00001123;
	*((volatile unsigned *)(EMI_IOCL)) = 0xa8a8a8a8;
	*((volatile unsigned *)(EMI_IOCL_2ND)) = 0x25252525;
	*((volatile unsigned *)(EMI_IOCM)) = 0xa8a8a8a8;
	*((volatile unsigned *)(EMI_IOCM_2ND)) = 0x25252525;
	*((volatile unsigned *)(EMI_TESTB)) = 0x00060037;
	*((volatile unsigned *)(EMI_TESTC)) = 0x38460014;
	*((volatile unsigned *)(EMI_TESTD)) = 0xa0000000;
	*((volatile unsigned *)(EMI_ARBA)) = 0x08047250;
	*((volatile unsigned *)(EMI_ARBB)) = 0x10107050;
	*((volatile unsigned *)(EMI_ARBC)) = 0x0a0a50df;
	*((volatile unsigned *)(EMI_ARBD)) = 0x0000f0d0;
	*((volatile unsigned *)(EMI_ARBE)) = 0x08086050;
	*((volatile unsigned *)(EMI_ARBE_2ND)) = 0x00000034;
	*((volatile unsigned *)(EMI_ARBF)) = 0x0a0a50df;
	*((volatile unsigned *)(EMI_ARBG)) = 0x20207048;
	*((volatile unsigned *)(EMI_ARBH)) = 0x20207048;
	*((volatile unsigned *)(EMI_ARBI)) = 0x00007108;
	*((volatile unsigned *)(EMI_ARBI_2ND)) = 0x00007108;
	*((volatile unsigned *)(EMI_ARBK)) = 0x00000000;
	*((volatile unsigned *)(EMI_SLCT)) = 0xff01ff00;
	*((volatile unsigned *)(EMI_BMEN)) = 0x00ff0000;
	*((volatile unsigned *)(EMI_CLUA)) = 0x00000000;

#if (CHANNEL_NUM == 4)
	*((volatile unsigned *)CHN_EMI_CONA(CHN0_EMI_BASE)) =
		0x0400f050;
	*((volatile unsigned *)CHN_EMI_CONA(CHN1_EMI_BASE)) =
		0x0400f050;
#else
	*((volatile unsigned *)CHN_EMI_CONA(CHN0_EMI_BASE)) =
		0x0400f050;
	*((volatile unsigned *)CHN_EMI_CONA(CHN1_EMI_BASE)) =
		0x0400f050;
#endif
	*((volatile unsigned *)CHN_EMI_CONB(CHN0_EMI_BASE)) =
		0xffff5048;
	*((volatile unsigned *)CHN_EMI_CONB(CHN1_EMI_BASE)) =
		0xffff5048;
	*((volatile unsigned *)CHN_EMI_CONC(CHN0_EMI_BASE)) =
		0x00000005;
	*((volatile unsigned *)CHN_EMI_CONC(CHN1_EMI_BASE)) =
		0x00000005;
	*((volatile unsigned *)CHN_EMI_MDCT(CHN0_EMI_BASE)) =
		0x11f08c03;
	*((volatile unsigned *)CHN_EMI_MDCT(CHN1_EMI_BASE)) =
		0x11f08c03;
	*((volatile unsigned *)CHN_EMI_SHF0(CHN0_EMI_BASE)) =
		0x12508C17;
	*((volatile unsigned *)CHN_EMI_SHF0(CHN1_EMI_BASE)) =
		0x12508C17;
	*((volatile unsigned *)CHN_EMI_TESTB(CHN0_EMI_BASE)) =
		0x00038037;
	*((volatile unsigned *)CHN_EMI_TESTB(CHN1_EMI_BASE)) =
		0x00038037;
	*((volatile unsigned *)CHN_EMI_TESTC(CHN0_EMI_BASE)) =
		0x38460002;
	*((volatile unsigned *)CHN_EMI_TESTC(CHN1_EMI_BASE)) =
		0x38460002;
	*((volatile unsigned *)CHN_EMI_TESTD(CHN0_EMI_BASE)) =
		0x00000000;
	*((volatile unsigned *)CHN_EMI_TESTD(CHN1_EMI_BASE)) =
		0x00000000;
	*((volatile unsigned *)CHN_EMI_MD_PRE_MASK(CHN0_EMI_BASE)) =
		0xaa0148ff;
	*((volatile unsigned *)CHN_EMI_MD_PRE_MASK(CHN1_EMI_BASE)) =
		0xaa0148ff;
	*((volatile unsigned *)CHN_EMI_MD_PRE_MASK_SHF(CHN0_EMI_BASE)) =
		0xaa516cff;
	*((volatile unsigned *)CHN_EMI_MD_PRE_MASK_SHF(CHN1_EMI_BASE)) =
		0xaa516cff;
	*((volatile unsigned *)CHN_EMI_AP_ERALY_CKE(CHN0_EMI_BASE)) =
		0x000002ff;
	*((volatile unsigned *)CHN_EMI_AP_ERALY_CKE(CHN1_EMI_BASE)) =
		0x000002ff;
	*((volatile unsigned *)CHN_EMI_DQFR(CHN0_EMI_BASE)) =
		0x00003101;
	*((volatile unsigned *)CHN_EMI_DQFR(CHN1_EMI_BASE)) =
		0x00003101;
	*((volatile unsigned *)CHN_EMI_ARBI(CHN0_EMI_BASE)) =
		0x20407188;
	*((volatile unsigned *)CHN_EMI_ARBI(CHN1_EMI_BASE)) =
		0x20407188;
	*((volatile unsigned *)CHN_EMI_ARBI_2ND(CHN0_EMI_BASE)) =
		0x20407188;
	*((volatile unsigned *)CHN_EMI_ARBI_2ND(CHN1_EMI_BASE)) =
		0x20407188;
	*((volatile unsigned *)CHN_EMI_ARBJ(CHN0_EMI_BASE)) =
		0x0719595e;
	*((volatile unsigned *)CHN_EMI_ARBJ(CHN1_EMI_BASE)) =
		0x0719595e;
	*((volatile unsigned *)CHN_EMI_ARBJ_2ND(CHN0_EMI_BASE)) =
		0x0719595e;
	*((volatile unsigned *)CHN_EMI_ARBJ_2ND(CHN1_EMI_BASE)) =
		0x0719595e;
	*((volatile unsigned *)CHN_EMI_ARBK(CHN0_EMI_BASE)) =
		0x64f3fc79;
	*((volatile unsigned *)CHN_EMI_ARBK(CHN1_EMI_BASE)) =
		0x64f3fc79;
	*((volatile unsigned *)CHN_EMI_ARBK_2ND(CHN0_EMI_BASE)) =
		0x64f3fc79;
	*((volatile unsigned *)CHN_EMI_ARBK_2ND(CHN1_EMI_BASE)) =
		0x64f3fc79;
	*((volatile unsigned *)CHN_EMI_SLCT(CHN0_EMI_BASE)) =
		0x00080868;
	*((volatile unsigned *)CHN_EMI_SLCT(CHN1_EMI_BASE)) =
		0x00080868;
	*((volatile unsigned *)CHN_EMI_ARB_REF(CHN0_EMI_BASE)) =
		0x82410222;
	*((volatile unsigned *)CHN_EMI_ARB_REF(CHN1_EMI_BASE)) =
		0x82410222;
	*((volatile unsigned *)CHN_EMI_DRS_MON0(CHN0_EMI_BASE)) =
		0x0000f801;
	*((volatile unsigned *)CHN_EMI_DRS_MON0(CHN1_EMI_BASE)) =
		0x0000f801;
	*((volatile unsigned *)CHN_EMI_DRS_MON1(CHN0_EMI_BASE)) =
		0x40000000;
	*((volatile unsigned *)CHN_EMI_DRS_MON1(CHN1_EMI_BASE)) =
		0x40000000;
	*((volatile unsigned *)CHN_EMI_RKARB0(CHN0_EMI_BASE)) =
		0x0006002f;
	*((volatile unsigned *)CHN_EMI_RKARB0(CHN1_EMI_BASE)) =
		0x0006002f;
	*((volatile unsigned *)CHN_EMI_RKARB1(CHN0_EMI_BASE)) =
		0x01010101;
	*((volatile unsigned *)CHN_EMI_RKARB1(CHN1_EMI_BASE)) =
		0x01010101;
	*((volatile unsigned *)CHN_EMI_RKARB2(CHN0_EMI_BASE)) =
		0x20201840;
	*((volatile unsigned *)CHN_EMI_RKARB2(CHN1_EMI_BASE)) =
		0x20201840;
	dsb();
}

void emi_esl_setting2(void)
{
  	dsb();
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);
	dsb();

	*((volatile unsigned *)(EMI_CONN)) = 0x00000000;
	*((volatile unsigned *)CHN_EMI_ARB_REF(CHN0_EMI_BASE)) =
		0x82410222;
	*((volatile unsigned *)(EMI_CONM)) = 0x007811ff;
	*((volatile unsigned *)CHN_EMI_CONC(CHN0_EMI_BASE)) =
		0x00000005;

	dsb();
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);;
	dsb();
}

void emi_patch(void)
{
	EMI_SETTINGS *emi_set;

	if (emi_setting_index == -1)
		emi_set = &default_emi_setting;
	else
		emi_set = &emi_settings[emi_setting_index];

	if((emi_set->type)==TYPE_LPDDR4)
	{
		*((volatile unsigned *)(EMI_BWCT0)) = 0x0a000705;
		*((volatile unsigned *)(EMI_BWCT0_3RD)) = 0x0;
	}
	if((emi_set->type)==TYPE_LPDDR3)
	{
		*((volatile unsigned *)(EMI_BWCT0)) = 0x07000505; //total BW setting for VcoreDVFS
		*((volatile unsigned *)(EMI_BWCT0_3RD)) = 0x0;
	}
}

void emi_init(DRAMC_CTX_T *p)
{
	EMI_SETTINGS *emi_set;
	unsigned temp;

	if (emi_setting_index == -1)
		emi_set = &default_emi_setting;
	else
		emi_set = &emi_settings[emi_setting_index];

	EMI_ESL_Setting1();

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);;
	//The following is MDL settings
	if(p->support_channel_num == CHANNEL_SINGLE) {
		emi_set->EMI_CONA_VAL &= ~(3UL<<8); //EMI_CONA[9:8] = 0: single channel, 1 dual channel, 2 quad channel
		emi_set->EMI_CONF_VAL = 0x00421000;
	}
	*(volatile unsigned *)EMI_CONA = emi_set->EMI_CONA_VAL;
	*(volatile unsigned *)EMI_CONF = emi_set->EMI_CONF_VAL;
	*(volatile unsigned *)EMI_CONH = emi_set->EMI_CONH_VAL;

	// CHNA and CHNB uses the same CH0 setting
	*(volatile unsigned *)CHN_EMI_CONA(CHN0_EMI_BASE) = emi_set->CHN0_EMI_CONA_VAL;
    *(volatile unsigned *)CHN_EMI_CONA(CHN1_EMI_BASE) = emi_set->CHN1_EMI_CONA_VAL;

	if((emi_set->type)==TYPE_PCDDR4)
	{
	    //BG_EN only for DDR4
		temp = *((volatile unsigned *)( EMI_CONH_2ND));
		temp |= (0x1<<8);
		*((volatile unsigned *)( EMI_CONH_2ND))=temp;
		// CHNA and CHNB uses the same CH0 setting for bringup use signal channal
		temp = *(volatile unsigned *)CHN_EMI_CONA(CHN0_EMI_BASE);
		temp &= ~(0x1<<26);
		*(volatile unsigned *)CHN_EMI_CONA(CHN0_EMI_BASE)=temp;
		*(volatile unsigned *)CHN_EMI_CONA(CHN1_EMI_BASE)=temp;

		temp = *(volatile unsigned *)CHN_EMI_COND(CHN0_EMI_BASE);
		temp |= 0x3;
		*(volatile unsigned *)CHN_EMI_COND(CHN0_EMI_BASE)=temp;
		*(volatile unsigned *)CHN_EMI_COND(CHN1_EMI_BASE)=temp;
	}

	//CMD_EN
	temp = *((volatile unsigned *)(EMI_CONM));
	temp |= (0x1<<10);
	*((volatile unsigned *)( EMI_CONM))=temp;
	// CHNA and CHNB uses the same CH0 setting ,for bringup use signal channal
	*(volatile unsigned *)CHN_EMI_CONC(CHN0_EMI_BASE) = 0x5;
	if(p->support_channel_num == CHANNEL_SINGLE)
		*(volatile unsigned *)CHN_EMI_CONC(CHN1_EMI_BASE) = 0x4;
	else
		*(volatile unsigned *)CHN_EMI_CONC(CHN1_EMI_BASE) = 0x5;
	dsb();

	p->vendor_id = emi_set->iLPDDR3_MODE_REG_5;

	/* emi slicer setting.
	 * sync MCU to EMI mux setting to spmfw.
	 * If not doing this step, system suspend/resume
	 * would encounter random hang.
	 */
	*((volatile unsigned *)(0x1000665c)) = 0x0;
}

void emi_init2(void)
{
	//emi_esl_setting2();
	emi_patch();
}

int get_dram_channel_nr(void)
{
	int channel_nr;

	channel_nr = (*((volatile unsigned int *)(EMI_CONA)) >> 8) & 0x3;

	return (0x1 << channel_nr);
}

int get_dram_rank_nr(void)
{
	int emi_cona = *(volatile unsigned *)EMI_CONA;

	if ((emi_cona & (1 << 17)) != 0 || (emi_cona & (1 << 16)) != 0)
		return 2; /* 2 Ranks */
	else
		return 1; /* 1 Rank */
}

#define get_col_bit(value, shift)	(((value >> shift) & 0x03) + 9)
#define get_row_bit(value, shift1, shift2)	\
	(((((value >> shift1) & 0x01) << 2) + ((value >> shift2) & 0x03)) + 13)
#define get_row_bit1(value1, value2, shift1, shift2)	\
	(((((value1 >> shift1) & 0x01) << 2) + \
	((value2 >> shift2) & 0x03)) + 13)
void get_dram_rank_size_by_emi_cona(u64 dram_rank_size[])
{
	unsigned col_bit, row_bit;
	u64 ch0_rank0_size, ch0_rank1_size, ch1_rank0_size, ch1_rank1_size;
	unsigned emi_cona = *(volatile unsigned *)EMI_CONA;
	unsigned emi_conh = *(volatile unsigned *)EMI_CONH;
	unsigned nr_chan_enabled = 1;
	u64 per_chan_rank0_size = 0, per_chan_rank1_size = 0;
	unsigned shift_for_16bit = 1;

	if (emi_cona & 0x2)  /* bit 1 */
		shift_for_16bit = 0;

	dram_rank_size[0] = 0;
	dram_rank_size[1] = 0;

	ch0_rank0_size = (emi_conh >> 16) & 0xf;
	ch0_rank1_size = (emi_conh >> 20) & 0xf;
	ch1_rank0_size = (emi_conh >> 24) & 0xf;
	ch1_rank1_size = (emi_conh >> 28) & 0xf;

	switch ((emi_cona >> 8) & 0x3) { /* bit 8:9 */
	case 0:
		nr_chan_enabled = 1;  /* bit0 */
		break;
	case 1:
		nr_chan_enabled = 2;  /* bit1 */
		break;
	case 2:
		nr_chan_enabled = 4;  /* bit2 */
		break;
	case 3:
	default:
		show_msg((INFO, "invalid CHN_EN field in EMI_REG_CONA (0x%x)\n",
			emi_cona));
		nr_chan_enabled = 2;  /* bit1 */
		break;
	}

	/* CH0 EMI */
	if (ch0_rank0_size == 0) { /* rank 0 setting */
		col_bit = get_col_bit(emi_cona, 4);
		row_bit = get_row_bit(emi_cona, 24, 12);
		per_chan_rank0_size = ((u64) (1 << (row_bit + col_bit))) *
			((u64) (4 >> shift_for_16bit) * 8);
	} else {
		per_chan_rank0_size = (ch0_rank0_size * 256 << 20);
	}

	if (0 != (emi_cona & (1 << 17))) { /* rank 1 exist */
		if (ch0_rank1_size == 0) {
			col_bit = get_col_bit(emi_cona, 6);
			row_bit = get_row_bit(emi_cona, 25, 14);
			per_chan_rank1_size =
				((u64) (1 << (row_bit + col_bit))) *
				((u64) (4 >> shift_for_16bit) * 8);
		} else {
			per_chan_rank1_size = (ch0_rank1_size * 256 << 20);
		}
	}

	if (nr_chan_enabled > 2) { /* CH0 EMI have CHA+CHB */
		dram_rank_size[0] = per_chan_rank0_size * 2;
		dram_rank_size[1] = per_chan_rank1_size * 2;
	} else { /* CH0 EMI is CHA */
		dram_rank_size[0] = per_chan_rank0_size;
		dram_rank_size[1] = per_chan_rank1_size;
	}

	/* CH1 EMI */
	if (nr_chan_enabled >= 2) {
		if (ch1_rank0_size == 0) { /* rank 0 setting */
			col_bit = get_col_bit(emi_cona, 20);
			row_bit = get_row_bit1(emi_conh, emi_cona, 4, 28);
			per_chan_rank0_size =
				((u64) (1 << (row_bit + col_bit))) *
				((u64) (4 >> shift_for_16bit) * 8);
		} else {
			per_chan_rank0_size = (ch1_rank0_size * 256 << 20);
		}

		if (0 != (emi_cona & (1 << 16))) { /* rank 1 exist */
			if (ch1_rank1_size == 0) {
				col_bit = get_col_bit(emi_cona, 22);
				row_bit = get_row_bit1(emi_conh,
					emi_cona, 5, 30);
				per_chan_rank1_size =
					((u64) (1 << (row_bit + col_bit))) *
					((u64) (4 >> shift_for_16bit) * 8);
			} else {
				per_chan_rank1_size =
					(ch1_rank1_size * 256 << 20);
			}
		}

		if (nr_chan_enabled > 2) { /* CH1 EMI have CHC+CHD */
			dram_rank_size[0] += per_chan_rank0_size * 2;
			dram_rank_size[1] += per_chan_rank1_size * 2;
		} else { /* CH1 EMI is CHB */
			dram_rank_size[0] += per_chan_rank0_size;
			dram_rank_size[1] += per_chan_rank1_size;
		}
	}

	show_msg((INFO,
		"DRAM rank0 size:0x%llx,\nDRAM rank1 size=0x%llx\n",
		dram_rank_size[0], dram_rank_size[1]));
}

void get_dram_rank_size(unsigned long long dram_rank_size[])
{
	get_dram_rank_size_by_emi_cona(dram_rank_size);
}

unsigned long long phy_addr_to_dram_addr1(dram_addr_t *dram_addr,
	unsigned long long phy_addr)
{
	unsigned long long rank_size[4];
	unsigned int emi_conf, bit_xor, rank_num;
	unsigned int index, bit_shift;

	get_dram_rank_size_by_emi_cona(rank_size);
	emi_conf = *((volatile unsigned int *)EMI_CONF) >> 8;
	rank_num = (unsigned int)get_dram_rank_nr();

	phy_addr -= PHY_ADDR_OFFSET;
	for (index = 0; index < rank_num; index++) {
		if (phy_addr >= rank_size[index])
			phy_addr -= rank_size[index];
		else
			break;
	}

	for (index = 11; index < 17; index++) {
		bit_xor = (emi_conf >> (4 * (index - 11))) & 0xf;
		bit_xor &= phy_addr >> 16;
		for (bit_shift = 0; bit_shift < 4; bit_shift++)
			phy_addr ^= ((bit_xor >> bit_shift) & 0x1) << index;
	}
	return phy_addr;
}

void phy_addr_to_dram_addr(dram_addr_t *dram_addr, unsigned long long phy_addr)
{
	unsigned int emi_cona;
	unsigned int ch_num;
	unsigned int bit_shift, ch_pos, ch_width;
	unsigned int temp;

	emi_cona = *((volatile unsigned int *)EMI_CONA);
	ch_num = (unsigned int)get_dram_channel_nr();

	phy_addr = phy_addr_to_dram_addr1(dram_addr, phy_addr);

	if (ch_num > 1) {
		ch_pos = ((emi_cona >> 2) & 0x3) + 7;

		for (ch_width = bit_shift = 0; bit_shift < 4; bit_shift++) {
			if ((unsigned int)(1 << bit_shift) >= ch_num)
				break;
			ch_width++;
		}

		switch (ch_width) {
		case 2:
			dram_addr->addr = ((phy_addr
				& ~(((0x1 << 2) << ch_pos) - 1)) >> 2);
			break;
		default:
			dram_addr->addr = ((phy_addr
				& ~(((0x1 << 1) << ch_pos) - 1)) >> 1);
			break;
		}

		dram_addr->addr |= (phy_addr & ((0x1 << ch_pos) - 1));
	}

	temp = dram_addr->addr >> 1;
	switch ((emi_cona >> 4) & 0x3) {
	case 0:
		dram_addr->col = temp & 0x1FF;
		temp = temp >> 9;
		break;
	case 1:
		dram_addr->col = temp & 0x3FF;
		temp = temp >> 10;
		break;
	case 2:
	default:
		dram_addr->col = temp & 0x7FF;
		temp = temp >> 11;
		break;
	}
	dram_addr->bk = temp & 0x7;
	temp = temp >> 3;

	dram_addr->row = temp;
}

#define CQ_DMA_BASE			0x10212000

void put_dummy_read_pattern(unsigned long long dst_pa, unsigned int src_pa,
	unsigned int len)
{
#if (CQ_DMA_BASE == 0) /* cc add in case Unexpected result */
	show_err2("%s while CQ_DMA_BASE is not configured\n",
		"[Error] put_dummy_read_pattern() invoked");
	while(1);
#endif

	*((volatile unsigned int *)(CQ_DMA_BASE + CQ_DMA_G_DMA_CON)) = 7 << 16;

	*((volatile unsigned int *)(CQ_DMA_BASE + CQ_DMA_G_DMA_SRC_ADDR))
		= src_pa;
	*((volatile unsigned int *)(CQ_DMA_BASE + CQ_DMA_G_DMA_SRC_ADDR2)) = 0;

	*((volatile unsigned int *)(CQ_DMA_BASE + CQ_DMA_G_DMA_DST_ADDR))
		= dst_pa & 0xffffffff;
	*((volatile unsigned int *)(CQ_DMA_BASE + CQ_DMA_G_DMA_DST_ADDR2))
		= dst_pa >> 32;

	*((volatile unsigned int *)(CQ_DMA_BASE + CQ_DMA_G_DMA_LEN1)) = len;
	dsb();
	*((volatile unsigned int *)(CQ_DMA_BASE + CQ_DMA_G_DMA_EN)) = 0x1;

	while
		(*((volatile unsigned int *)(CQ_DMA_BASE + CQ_DMA_G_DMA_EN)));
}

static unsigned int get_dramc_addr(dram_addr_t *dram_addr, unsigned int offset)
{
	unsigned int ch_num, rank_num;
	unsigned long long dummy_read_addr;
	unsigned long long rank_size[4];
	unsigned int index;
	unsigned int *src_addr;

	ch_num = (unsigned int)get_dram_channel_nr();
	rank_num = (unsigned int)get_dram_rank_nr();
	get_dram_rank_size_by_emi_cona(rank_size);
	dummy_read_addr = PHY_ADDR_OFFSET;
	src_addr = (unsigned int *)PHY_ADDR_OFFSET;

	if (dram_addr->ch >= ch_num) {
		show_msg((INFO, "[DRAMC] invalid channel: %d\n",
			dram_addr->ch));
		return 0;
	}

	if (dram_addr->rk >= rank_num) {
		show_msg((INFO, "[DRAMC] invalid rank: %d\n",
			dram_addr->rk));
		return 0;
	}

	for (index = 0; index <= dram_addr->rk; index++)
		dummy_read_addr += rank_size[index];
	dummy_read_addr -= offset;

	/* cc notes: only consider CH0?? */
	if (dram_addr->ch == 0)
		dummy_read_addr &= ~(0x100);

	if (offset == 0x20) { /* 32bit */
		for (index = 0; index < 4; index++)
			*(src_addr + index) = PATTERN3;
		put_dummy_read_pattern
			(dummy_read_addr, (unsigned int)src_addr, 16);
	}

	dram_addr->full_sys_addr = dummy_read_addr;
	phy_addr_to_dram_addr(dram_addr, dummy_read_addr);

	return dram_addr->addr;
}

unsigned int get_dummy_read_addr(dram_addr_t *dram_addr)
{
	return get_dramc_addr(dram_addr, 0x20);
}

void get_orig_dram_rank_info(dram_info_t *orig_dram_info)
{
    int i, j;
    u64 base = DRAM_BASE;
    unsigned long long rank_size[4];

    orig_dram_info->rank_num = get_dram_rank_nr();
    get_dram_rank_size(rank_size);

    orig_dram_info->rank_info[0].start = base;
    for (i = 0; i < orig_dram_info->rank_num; i++) {
        orig_dram_info->rank_info[i].size = (u64)rank_size[i];
        if (i > 0) {
            orig_dram_info->rank_info[i].start =
                orig_dram_info->rank_info[i - 1].start +
                orig_dram_info->rank_info[i - 1].size;
        }
        print("orig_dram_info[%d] start: 0x%llx, size: 0x%llx\n",
               i, orig_dram_info->rank_info[i].start,
               orig_dram_info->rank_info[i].size);
    }

    for (j=i; j<4; j++)
    {
        orig_dram_info->rank_info[j].start = 0;
        orig_dram_info->rank_info[j].size = 0;
    }
}
