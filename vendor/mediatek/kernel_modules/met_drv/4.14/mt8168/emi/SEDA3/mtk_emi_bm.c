// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 */

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/io.h>
#include <mt-plat/sync_write.h>
#include <mt-plat/mtk_io.h>
#include "mtk_typedefs.h"
#include "core_plf_init.h"
#include "met_plf_chip_init.h"
#include "mtk_emi_bm.h"
#include "mtk_dramc_reg.h"
#include "met_drv.h"
#include "interface.h"

#undef	DEBUG
#undef	debug_reg
#ifdef	debug_reg
static inline unsigned int emi_readl(void __iomem *padr)
{
	unsigned int tmp;

	tmp = readl(padr);
	MET_TRACE("[MET_EMI] RD_Reg: %p: %08x\n", padr, tmp);
	return tmp;
}

static inline void __emi_reg_sync_writel(unsigned int data, void __iomem *padr)
{
	unsigned int tmp;

	mt_reg_sync_writel(data, padr);
	tmp = readl(padr);
	MET_TRACE("[MET_EMI] WR_Reg: %p: %08x, %08x\n", padr, data, tmp);
}

#define emi_reg_sync_writel(data, adr)  __emi_reg_sync_writel(data, IOMEM(adr))

#else
#define emi_readl               readl
#define emi_reg_sync_writel     mt_reg_sync_writel
#endif

#define MASK_MASTER     0xFF
#define MASK_TRANS_TYPE 0xFF

static int dram_chann_num;
static void __iomem *BaseAddrEMI;
static void __iomem *BaseAddrCHN_EMI[2];

static int dramc0_dcm_enable;
static int dramc1_dcm_enable;

#define CH0_MISC_CG_CTRL0 (((unsigned long) BaseAddrDDRPHY_AO[0]) + 0x284)
#define CH1_MISC_CG_CTRL0 (((unsigned long) BaseAddrDDRPHY_AO[1]) + 0x284)
const unsigned int emi_config[] = {
	EMI_BMEN,
	EMI_MSEL,
	EMI_MSEL2,
	EMI_MSEL3,
	EMI_MSEL4,
	EMI_MSEL5,
	EMI_MSEL6,
	EMI_MSEL7,
	EMI_MSEL8,
	EMI_MSEL9,
	EMI_MSEL10,
	EMI_BMID0,
	EMI_BMID1,
	EMI_BMID2,
	EMI_BMID3,
	EMI_BMID4,
	EMI_BMID5,
	EMI_BMID6,
	EMI_BMID7,
	EMI_BMID8,
	EMI_BMID9,
	EMI_BMID10,
	EMI_BMEN1,
	EMI_BMEN2,
	EMI_BMRW0,
	EMI_BMRW1
};
#define EMI_CONFIG_MX_NR (sizeof(emi_config)/sizeof(unsigned int))
static unsigned int emi_config_val[EMI_CONFIG_MX_NR];

/*
 *   MET_REG_BSET/MET_REG_BCLR:
 *   reading value before set and clear
 */
static inline void MET_REG_BSET(unsigned long reg, u32 shift)
{
	unsigned int read_val = 0;

	read_val = emi_readl(IOMEM(reg));
	emi_reg_sync_writel(read_val | (1 << shift), reg);
}


static inline void MET_REG_BCLR(unsigned long reg, u32 shift)
{
	unsigned int read_val = 0;

	read_val = emi_readl(IOMEM(reg));
	emi_reg_sync_writel(read_val & (~((1 << shift) & 0xFFFFFFFF)), reg);
}


int MET_BM_Init(void)
{
	int i;
	int idx;

	/*emi*/
	if (!mt_cen_emi_base_get_symbol) {
		METERROR("[%d]mt_cen_emi_base_get_symbol = NULL\n", __LINE__);
		PR_BOOTMSG_ONCE("[%d]mt_cen_emi_base_get_symbol = NULL\n", __LINE__);
		return -1;
	}

	BaseAddrEMI = mt_cen_emi_base_get_symbol();
	if (BaseAddrEMI == 0) {
		METERROR("BaseAddrEMI = 0\n");
		PR_BOOTMSG_ONCE("BaseAddrEMI = 0\n");
		return -1;
	}

	METINFO("MET EMI: map emi to %p\n", BaseAddrEMI);
	PR_BOOTMSG("MET EMI: map emi to %p\n", BaseAddrEMI);

	/* get dram channel number */
	dram_chann_num = MET_EMI_GetDramChannNum();
	METINFO("[%s][%d]dram_chann_num = %d\n", __func__, __LINE__, dram_chann_num);

	if (dram_chann_num > MAX_DRAMC_CHANN) {
		METERROR("dram_chann_num %d > %d\n", dram_chann_num, MAX_DRAMC_CHANN);
		PR_BOOTMSG("dram_chann_num %d > %d\n", dram_chann_num, MAX_DRAMC_CHANN);
		return -1;
	}

	if (!mt_dramc_nao_chn_base_get_symbol) {
		METERROR("mt_dramc_nao_cha_base_get = NULL\n");
		PR_BOOTMSG_ONCE("mt_dramc_nao_cha_base_get = NULL\n");
		return -1;
	}

	for (i = 0; i < dram_chann_num; i++) {
		BaseAddrDRAMC[i] = mt_dramc_nao_chn_base_get_symbol(i);
		if (BaseAddrDRAMC[i] == 0) {
			METERROR("BaseAddrDRAMC%d = 0\n", i);
			PR_BOOTMSG_ONCE("BaseAddrDRAMC%d = 0\n", i);
			return -1;
		}

		METINFO("MET EMI: map nao dramc%c to %p\n",'A'+i, BaseAddrDRAMC[i]);
		PR_BOOTMSG("MET EMI: map nao dramc%c to %p\n", 'A'+i, BaseAddrDRAMC[i]);
	}

	/*dram DRAMC_DTS_DDRPHY_AO*/
	/* get DRS base address */
	if (!mt_ddrphy_chn_base_get_symbol) {
		METERROR("mt_ddrphy_chn_base_get = NULL\n");
		PR_BOOTMSG_ONCE("mt_ddrphy_chn_base_get = NULL\n");
		return -1;
	}

	if (!mt_chn_emi_base_get_symbol) {
		METERROR("mt_chn_emi_base_get_symbol = NULL\n");
		PR_BOOTMSG_ONCE("mt_chn_emi_base_get_symbol = NULL\n");
		return -1;
	}

	for (i = 1; i <= dram_chann_num && i < 3; i++) {
		idx = i - 1;
		BaseAddrDDRPHY_AO[idx] = mt_ddrphy_chn_base_get_symbol(idx);
		if (BaseAddrDDRPHY_AO[idx] == 0) {
			METERROR("BaseAddrDDRPHY_AO[%d] = 0\n", idx);
			PR_BOOTMSG_ONCE("BaseAddrDDRPHY_AO[%d] = 0\n", idx);
			return -1;
		}

		METINFO("MET EMI: map ddrphy%d AO to %p\n", idx, BaseAddrDDRPHY_AO[idx]);
		PR_BOOTMSG("MET EMI: map ddrphy%d AO to %p\n", idx, BaseAddrDDRPHY_AO[idx]);

		BaseAddrCHN_EMI[idx] = mt_chn_emi_base_get_symbol(idx);
		if (BaseAddrCHN_EMI[idx] == 0) {
			METERROR("BaseAddrCHN_EMI[%d] = 0\n", idx);
			PR_BOOTMSG_ONCE("BaseAddrCHN_EMI[%d] = 0\n", idx);
			return -1;
		}

		METINFO("MET EMI: map BaseAddrCHN_EMI[%d] to %p\n", idx, BaseAddrCHN_EMI[idx]);
		PR_BOOTMSG("MET EMI: map BaseAddrCHN_EMI[%d] to %p\n", idx, BaseAddrCHN_EMI[idx]);
	}

	/*dram DRAMC_DTS_DRAMC0_AO*/
	if (!mt_dramc_chn_base_get_symbol) {
		METERROR("mt_dramc_chn_base_get = NULL\n");
		PR_BOOTMSG_ONCE("mt_dramc_chn_base_get = NULL\n");
		return -1;
	}

	BaseAddrDRAMC0_AO = mt_dramc_chn_base_get_symbol(0);
	if (BaseAddrDRAMC0_AO == 0) {
		METERROR("BaseAddrDRAMC0_AO = 0\n");
		PR_BOOTMSG_ONCE("BaseAddrDRAMC0_AO = 0\n");
		return -1;
	}

	METINFO("MET EMI: map AO dramcA to %p\n", BaseAddrDRAMC0_AO);
	PR_BOOTMSG("MET EMI: map AO dramcA to %p\n", BaseAddrDRAMC0_AO);

	return 0;
}


void MET_BM_DeInit(void)
{
}


void MET_BM_SaveCfg(void)
{
	int i;

	for (i = 0; i < EMI_CONFIG_MX_NR; i++)
		emi_config_val[i] = emi_readl(IOMEM(ADDR_EMI + emi_config[i]));
}


void MET_BM_RestoreCfg(void)
{
	int i;

	for (i = 0; i < EMI_CONFIG_MX_NR; i++)
		emi_reg_sync_writel(emi_config_val[i], ADDR_EMI + emi_config[i]);
}


void MET_BM_Clear_Start(void)
{
	/* Force EMI idle low */
	MET_REG_BSET(ADDR_EMI + EMI_BMEN, BUS_MON_IDLE_SHIFT);

	/* Disable dramc dcm */
	switch (dram_chann_num) {
	case 1:
		MET_REG_BSET(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
		break;
	case 2:
		MET_REG_BSET(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
		MET_REG_BSET(CH1_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
		break;
	default:
		METERROR("Error: dram_chann_num = %d\n", dram_chann_num);
	}

	/* Disable EBM */
	MET_REG_BCLR(ADDR_EMI + EMI_BMEN, BUS_MON_EN_SHIFT);

	/* Enable EBM */
	MET_REG_BSET(ADDR_EMI + EMI_BMEN, BUS_MON_EN_SHIFT);

	/* Enable EMI dcm */
	MET_REG_BCLR(ADDR_EMI + EMI_BMEN, BUS_MON_IDLE_SHIFT);

	/* restore dramc dcm */
	switch (dram_chann_num) {
	case 1:
		if (dramc0_dcm_enable)
			MET_REG_BCLR(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
		else
			MET_REG_BSET(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
		break;
	case 2:
		if (dramc0_dcm_enable)
			MET_REG_BCLR(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
		else
			MET_REG_BSET(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);

		if (dramc1_dcm_enable)
			MET_REG_BCLR(CH1_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
		else
			MET_REG_BSET(CH1_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
		break;
	default:
		METERROR("Error: dram_chann_num = %d\n", dram_chann_num);
	}
}


void MET_BM_Enable(const unsigned int enable)
{
	unsigned long int value_check;
	int i = 0;

	while (i < 100) {
		/* Force EMI idle low */
		MET_REG_BSET(ADDR_EMI + EMI_BMEN, BUS_MON_IDLE_SHIFT);

		/* disable dramc dcm */
		switch (dram_chann_num) {
		case 1:
			MET_REG_BSET(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
			break;
		case 2:
			MET_REG_BSET(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
			MET_REG_BSET(CH1_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
			break;
		default:
			METERROR("Error: dram_chann_num = %d\n", dram_chann_num);
		}

		if (enable == 0)
			/* Disable EBM */
			MET_REG_BCLR(ADDR_EMI + EMI_BMEN, BUS_MON_EN_SHIFT);
		else
			/* Enable EBM */
			MET_REG_BSET(ADDR_EMI + EMI_BMEN, BUS_MON_EN_SHIFT);

		/* Enable EMI dcm */
		MET_REG_BCLR(ADDR_EMI + EMI_BMEN, BUS_MON_IDLE_SHIFT);

		/* restore dramc dcm */
		switch (dram_chann_num) {
		case 1:
			if (dramc0_dcm_enable)
				MET_REG_BCLR(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
			else
				MET_REG_BSET(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
			break;
		case 2:
			if (dramc0_dcm_enable)
				MET_REG_BCLR(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
			else
				MET_REG_BSET(CH0_MISC_CG_CTRL0, DRAMC_CG_SHIFT);

			if (dramc1_dcm_enable)
				MET_REG_BCLR(CH1_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
			else
				MET_REG_BSET(CH1_MISC_CG_CTRL0, DRAMC_CG_SHIFT);
			break;
		default:
			METERROR("Error: dram_chann_num = %d\n", dram_chann_num);
		}

		value_check = emi_readl(IOMEM(ADDR_EMI + EMI_BMEN));

		if (enable == 0) {
			/* EN == 0, IDLE == 0 when EMI RESET */
			if (!test_bit(BUS_MON_EN_SHIFT, &value_check)
			    && !test_bit(BUS_MON_IDLE_SHIFT, &value_check)) {
				break;
			}
		} else {
			/* EN == 1, IDLE == 0 when EMI START */
			if (test_bit(BUS_MON_EN_SHIFT, &value_check)
			    && !test_bit(BUS_MON_IDLE_SHIFT, &value_check)) {
				break;
			}
		}
		i++;
	}

	/*MET_TRACE("[MET_BM_ENABLE] value_check: %lx, enable = %d\n", value_check, enable); */

}


#if 0
void BM_Disable(void)
{
	const unsigned int value = emi_readl(IOMEM(ADDR_EMI + EMI_BMEN));

	emi_reg_sync_writel(value & (~BUS_MON_EN), ADDR_EMI + EMI_BMEN);
}
#endif


void MET_BM_Pause(void)
{
	const unsigned int value = emi_readl(IOMEM(ADDR_EMI + EMI_BMEN));

	emi_reg_sync_writel(value | (1 << BUS_MON_PAUSE_SHIFT), ADDR_EMI + EMI_BMEN);
}


void MET_BM_Continue(void)
{
	const unsigned int value = emi_readl(IOMEM(ADDR_EMI + EMI_BMEN));

	emi_reg_sync_writel(value & (~(1 << BUS_MON_PAUSE_SHIFT)), ADDR_EMI + EMI_BMEN);
}


unsigned int MET_BM_IsOverrun(void)
{
	/*
	 * return 0 if EMI_BCNT(bus cycle counts) or
	 * EMI_WACT(total word counts) is overrun,
	 * otherwise return an !0 value
	 */
	const unsigned int value = emi_readl(IOMEM(ADDR_EMI + EMI_BMEN));

	return (value & (1 << BC_OVERRUN_SHIFT));
}


unsigned int MET_BM_GetReadWriteType(void)
{
	const unsigned int value = emi_readl(IOMEM(ADDR_EMI + EMI_BMEN));

	return ((value & 0xFFFFFFCF) >> 4);
}


void MET_BM_SetReadWriteType(const unsigned int ReadWriteType)
{
	const unsigned int value = emi_readl(IOMEM(ADDR_EMI + EMI_BMEN));

	/*
	 * ReadWriteType: 00/11 --> both R/W
	 *                   01 --> only R
	 *                   10 --> only W
	 */
	emi_reg_sync_writel((value & 0xFFFFFFCF) | (ReadWriteType << 4), ADDR_EMI + EMI_BMEN);
}


int MET_BM_GetBusCycCount(void)
{
	return MET_BM_IsOverrun() ? BM_ERR_OVERRUN : emi_readl(IOMEM(ADDR_EMI + EMI_BCNT));	/*Bus cycle counter */
}


unsigned int MET_BM_GetTransAllCount(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_TACT));
}


int MET_BM_GetTransCount(const unsigned int counter_num)
{
	unsigned int iCount;

	switch (counter_num) {
	case 1:
		iCount = emi_readl(IOMEM(ADDR_EMI + EMI_TSCT));
		break;

	case 2:
		iCount = emi_readl(IOMEM(ADDR_EMI + EMI_TSCT2));
		break;

	case 3:
		iCount = emi_readl(IOMEM(ADDR_EMI + EMI_TSCT3));
		break;

	default:
		return BM_ERR_WRONG_REQ;
	}

	return iCount;
}


int MET_BM_GetWordAllCount(void)
{
	return MET_BM_IsOverrun() ? BM_ERR_OVERRUN : emi_readl(IOMEM(ADDR_EMI + EMI_WACT));
}


int MET_BM_GetWordCount(const unsigned int counter_num)
{
	unsigned int iCount;

	switch (counter_num) {
	case 1:
		iCount = emi_readl(IOMEM(ADDR_EMI + EMI_WSCT));
		break;

	case 2:
		iCount = emi_readl(IOMEM(ADDR_EMI + EMI_WSCT2));
		break;

	case 3:
		iCount = emi_readl(IOMEM(ADDR_EMI + EMI_WSCT3));
		break;

	case 4:
		iCount = emi_readl(IOMEM(ADDR_EMI + EMI_WSCT4));
		break;

	default:
		return BM_ERR_WRONG_REQ;
	}

	return iCount;
}


unsigned int MET_BM_GetBandwidthWordCount(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BACT));	/*Bandwidth counter for access */
}


unsigned int MET_BM_GetOverheadWordCount(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BSCT));	/*Overhead counter */
}


int MET_BM_GetTransTypeCount(const unsigned int counter_num)
{
	return (counter_num < 1 || counter_num > BM_COUNTER_MAX)
	    ? BM_ERR_WRONG_REQ : emi_readl(IOMEM(ADDR_EMI + EMI_TTYPE1 + (counter_num - 1) * 8));
}


int MET_BM_GetMDCT(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_MDCT));
}


int MET_BM_GetMDCT_2(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_MDCT_2ND));
}


int MET_BM_SetMDCT_MDMCU(unsigned int mdmcu_rd_buf)
{
	unsigned int value_origin;

	value_origin = emi_readl(IOMEM(ADDR_EMI + EMI_MDCT_2ND));
	MET_TRACE("[MET_BM_SetMDCT_MDMCU] value_origin: %x\n", value_origin);

	value_origin = value_origin & ~(0x7);
	value_origin = value_origin | ((mdmcu_rd_buf) & 0x7);

	emi_reg_sync_writel(value_origin, ADDR_EMI + EMI_MDCT_2ND);

	return BM_REQ_OK;
}


int MET_BM_GetMonitorCounter(const unsigned int counter_num,
			     unsigned int *master, unsigned int *trans_type)
{
	unsigned int value, addr;

	if (counter_num < 1 || counter_num > BM_COUNTER_MAX)
		return BM_ERR_WRONG_REQ;


	if (counter_num == 1) {
		addr = EMI_BMEN;
		value = emi_readl(IOMEM(ADDR_EMI + addr));
		*master = (value >> 16) & MASK_MASTER;
		*trans_type = (value >> 24) & MASK_TRANS_TYPE;
	} else {
		addr = (counter_num <= 3) ? EMI_MSEL : (EMI_MSEL2 + (counter_num / 2 - 2) * 8);
		value = emi_readl(IOMEM(ADDR_EMI + addr)) >> ((counter_num % 2) * 16);
		*master = value & MASK_MASTER;
		*trans_type = (value >> 8) & MASK_TRANS_TYPE;
	}

	return BM_REQ_OK;
}


int MET_BM_SetMonitorCounter(const unsigned int counter_num,
			     const unsigned int master, const unsigned int trans_type)
{
	unsigned int value, addr;
	const unsigned int iMask = (MASK_TRANS_TYPE << 8) | MASK_MASTER;

	if (counter_num < 1 || counter_num > BM_COUNTER_MAX)
		return BM_ERR_WRONG_REQ;


	if (counter_num == 1) {
		addr = EMI_BMEN;
		value = (emi_readl(IOMEM(ADDR_EMI + addr)) & ~(iMask << 16)) |
		    ((trans_type & MASK_TRANS_TYPE) << 24) | ((master & MASK_MASTER) << 16);
	} else {
		addr = (counter_num <= 3) ? EMI_MSEL : (EMI_MSEL2 + (counter_num / 2 - 2) * 8);

		/* clear master and transaction type fields */
		value = emi_readl(IOMEM(ADDR_EMI + addr)) & ~(iMask << ((counter_num % 2) * 16));

		/* set master and transaction type fields */
		value |= (((trans_type & MASK_TRANS_TYPE) << 8) |
			  (master & MASK_MASTER)) << ((counter_num % 2) * 16);
	}

	emi_reg_sync_writel(value, ADDR_EMI + addr);

	return BM_REQ_OK;
}


int MET_BM_SetTtypeCounterRW(unsigned int bmrw0_val, unsigned int bmrw1_val)
{

	unsigned int value_origin;

	value_origin = emi_readl(IOMEM(ADDR_EMI + EMI_BMRW0));
	MET_TRACE("[MET_EMI_settype1] value_origin: %x\n", value_origin);
	if (value_origin != bmrw0_val) {
		emi_reg_sync_writel(bmrw0_val, ADDR_EMI + EMI_BMRW0);
		MET_TRACE("[MET_EMI_settype1] bmrw0_val: %x, value_origin: %x\n", bmrw0_val,
			   value_origin);
	}


	value_origin = emi_readl(IOMEM(ADDR_EMI + EMI_BMRW1));
	MET_TRACE("[MET_EMI_settype2] value_origin: %x\n", value_origin);
	if (value_origin != bmrw1_val) {
		emi_reg_sync_writel(bmrw1_val, ADDR_EMI + EMI_BMRW1);
		MET_TRACE("[MET_EMI_settype2] bmrw0_val: %x, value_origin: %x\n", bmrw1_val,
			   value_origin);

	}
	return BM_REQ_OK;
}


int MET_BM_Set_WsctTsct_id_sel(unsigned int counter_num, unsigned int enable)
{
	unsigned int value;

	if (counter_num > 3)
		return BM_ERR_WRONG_REQ;

	value =
	    ((emi_readl(IOMEM(ADDR_EMI + EMI_BMEN2)) & (~(1 << (28 + counter_num)))) |
	     (enable << (28 + counter_num)));
	emi_reg_sync_writel(value, ADDR_EMI + EMI_BMEN2);

	return BM_REQ_OK;
}


int MET_BM_SetMaster(const unsigned int counter_num, const unsigned int master)
{
	unsigned int value, addr;
	const unsigned int iMask = 0x7F;

	if (counter_num < 1 || counter_num > BM_COUNTER_MAX)
		return BM_ERR_WRONG_REQ;


	if (counter_num == 1) {
		addr = EMI_BMEN;
		value =
		    (emi_readl(IOMEM(ADDR_EMI + addr)) & ~(iMask << 16)) | ((master & iMask) << 16);
	} else {
		addr = (counter_num <= 3) ? EMI_MSEL : (EMI_MSEL2 + (counter_num / 2 - 2) * 8);

		/* clear master and transaction type fields */
		value = emi_readl(IOMEM(ADDR_EMI + addr)) & ~(iMask << ((counter_num % 2) * 16));

		/* set master and transaction type fields */
		value |= ((master & iMask) << ((counter_num % 2) * 16));
	}

	emi_reg_sync_writel(value, ADDR_EMI + addr);

	return BM_REQ_OK;
}


int MET_BM_SetbusID_En(const unsigned int counter_num,
		       const unsigned int enable)
{
	unsigned int value;

	if ((counter_num < 1 || counter_num > BM_COUNTER_MAX) || (enable > 1))
		return BM_ERR_WRONG_REQ;

	if (enable == 0) {
		/* clear  EMI ID selection Enabling   SEL_ID_EN */
		value = (emi_readl(IOMEM(ADDR_EMI + EMI_BMEN2))
			 & ~(1 << (counter_num - 1)));
	} else {
		/* enable  EMI ID selection Enabling   SEL_ID_EN */
		value = (emi_readl(IOMEM(ADDR_EMI + EMI_BMEN2))
			 | (1 << (counter_num - 1)));
	}
	emi_reg_sync_writel(value, ADDR_EMI + EMI_BMEN2);

	return BM_REQ_OK;
}


int MET_BM_SetbusID(const unsigned int counter_num,
		    const unsigned int id)
{
	unsigned int value, addr, shift_num;

	if ((counter_num < 1 || counter_num > BM_COUNTER_MAX))
		return BM_ERR_WRONG_REQ;

	/* offset of EMI_BMIDx register */
	addr = EMI_BMID0 + (counter_num - 1) / 2 * 4;
	shift_num = ((counter_num - 1) % 2) * 16;
	/* clear SELx_ID field */
	value = emi_readl(IOMEM(ADDR_EMI + addr)) & ~(EMI_BMID_MASK << shift_num);

	/* set SELx_ID field */
	if (id <= 0xffff)       /*bigger then 0xff_ff : no select busid in master, reset busid as 0*/
		value |= id << shift_num;

	emi_reg_sync_writel(value, ADDR_EMI + addr);

	return BM_REQ_OK;
}


int MET_BM_SetUltraHighFilter(const unsigned int counter_num, const unsigned int enable)
{
	unsigned int value;

	if ((counter_num < 1 || counter_num > BM_COUNTER_MAX) || (enable > 1))
		return BM_ERR_WRONG_REQ;


	value = (emi_readl(IOMEM(ADDR_EMI + EMI_BMEN1))
		 & ~(1 << (counter_num - 1)))
		| (enable << (counter_num - 1));

	emi_reg_sync_writel(value, ADDR_EMI + EMI_BMEN1);

	return BM_REQ_OK;
}


int MET_BM_SetLatencyCounter(unsigned int enable)
{
	unsigned int value;

	value = emi_readl(IOMEM(ADDR_EMI + EMI_BMEN2)) & ~(0x3 << 24);
	/*
	 * emi_ttype1 -- emi_ttype8 change as total latencies
	 * for m0 -- m7,
	 * and emi_ttype9 -- emi_ttype16 change as total transaction counts
	 * for m0 -- m7
	 */
	if (enable == 1)
		value |= (0x2 << 24);

	emi_reg_sync_writel(value, ADDR_EMI + EMI_BMEN2);

	return BM_REQ_OK;
}


int MET_BM_GetLatencyCycle(unsigned int *__restrict__ emi_value,
				const unsigned int begin, const unsigned int end)
{
	int i, j = 0;

	for (i = begin - 1; i < end; i++)
		emi_value[j++] = emi_readl(IOMEM(ADDR_EMI + (EMI_TTYPE1 + i * 8)));

	return j;
}


unsigned int MET_BM_GetEmiDcm(void)
{
	return (emi_readl(IOMEM(ADDR_EMI + EMI_CONM)) >> 24);
}


int MET_BM_SetEmiDcm(const unsigned int setting)
{
	unsigned int value;

	value = emi_readl(IOMEM(ADDR_EMI + EMI_CONM));
	emi_reg_sync_writel((value & 0x00FFFFFF) | (setting << 24), ADDR_EMI + EMI_CONM);

	return BM_REQ_OK;
}


unsigned int MET_EMI_GetMDCT(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_MDCT));
}


unsigned int MET_EMI_GetMDCT_2ND(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_MDCT_2ND));
}


unsigned int MET_EMI_GetARBA(void)
{
	/* EMI_ARBA EMI Bandwidth Filter Control M0/1 */
	return emi_readl(IOMEM(ADDR_EMI + EMI_ARBA));
}


unsigned int MET_EMI_GetARBB(void)
{
	/* need the ATBB for mt6739 */
	return emi_readl(IOMEM(ADDR_EMI+EMI_ARBB));
}


unsigned int MET_EMI_GetARBC(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_ARBC));
}


unsigned int MET_EMI_GetARBD(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_ARBD));
}


unsigned int MET_EMI_GetARBE(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_ARBE));
}


unsigned int MET_EMI_GetARBF(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_ARBF));
}


unsigned int MET_EMI_GetARBG(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_ARBG));
}


unsigned int MET_EMI_GetARBH(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_ARBH));
}


/* Total BW status*/
unsigned int MET_EMI_GetBWCT0(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWCT0));
}


/* Total BW status*/
unsigned int MET_EMI_GetBWCT1(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWCT1));
}


/* Total BW status*/
unsigned int MET_EMI_GetBWCT2(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWCT2));
}


/* Total BW status*/
unsigned int MET_EMI_GetBWCT3(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWCT3));
}


/* Total BW status*/
unsigned int MET_EMI_GetBWCT4(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWCT4));
}


/* Total BW status*/
unsigned int MET_EMI_GetBWST0(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWST0));
}

/* Total BW status*/
unsigned int MET_EMI_GetBWST1(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWST1));
}


/* C+G BW status*/
unsigned int MET_EMI_GetBWCT0_2ND(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWCT0_2ND));
}

/* C+G BW status*/
unsigned int MET_EMI_GetBWCT1_2ND(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWCT1_2ND));
}


/* C+G BW status*/
unsigned int MET_EMI_GetBWST_2ND(void)
{
	return emi_readl(IOMEM(ADDR_EMI + EMI_BWST_2ND));
}


unsigned int MET_EMI_GetBMRW0(void)
{
	return readl(IOMEM(ADDR_EMI + EMI_BMRW0));
}


void emi_dump_reg(void)
{
	int i;

	MET_TRACE("[emi_regdump]\n");
	for (i = 0x400; i < 0x500; i = i + 16)
		MET_TRACE("%4x__ %8x %8x %8x %8x\n", i, readl(IOMEM(ADDR_EMI + i)),
			   readl(IOMEM(ADDR_EMI + i + 4)), readl(IOMEM(ADDR_EMI + i + 8)),
			   readl(IOMEM(ADDR_EMI + i + 12)));
}


unsigned int MET_EMI_GetDramChannNum(void)
{
	int num = -1;

	if (BaseAddrEMI) {
		num = emi_readl(IOMEM(ADDR_EMI + EMI_CONA));
		num = ((num >> 8) & 0x0000003);
	} else {
		return 1;
	}

	if (num == M0_DOUBLE_HALF_BW_1CH)
		return 1;
	else if (num == M0_DOUBLE_HALF_BW_2CH)
		return 2;
	else if (num == M0_DOUBLE_HALF_BW_4CH)
		return 4;
	else                    /* default return single channel */
		return 1;
}


unsigned int MET_EMI_GetDramRankNum(void)
{
	int dual_rank = 0;

	if (BaseAddrEMI) {
		dual_rank = emi_readl(IOMEM(ADDR_EMI + EMI_CONA));
		dual_rank = ((dual_rank >> 17) & RANK_MASK);
	} else {
		return DUAL_RANK;
	}

	if (dual_rank == DISABLE_DUAL_RANK_MODE)
		return ONE_RANK;
	else			/* default return dual rank */
		return DUAL_RANK;
}


/*
 * Dual Channel:
 * Enables two rank for CHB
 * Quad Channel:
 * Enables two rank  for CHC and CHD
 * 0: Disable dual rank mode
 * 1: Enable dual rank mode
 */
unsigned int MET_EMI_GetDramRankNum_CHN1(void)
{
	int dual_rank = 0;

	if (BaseAddrEMI) {
		dual_rank = emi_readl(IOMEM(ADDR_EMI + EMI_CONA));
		dual_rank = ((dual_rank >> 16) & RANK_MASK);
	} else {
		return DUAL_RANK;
	}

	if (dual_rank == DISABLE_DUAL_RANK_MODE)
		return ONE_RANK;
	else			/* default return dual rank */
		return DUAL_RANK;
}


void met_get_drs_registers(uint32_t *value)
{
	uint32_t tmp;

	memset(value, 0, sizeof(uint32_t) * 6);

	if (BaseAddrCHN_EMI[0] == NULL)
		return;
	if (dram_chann_num > 1 && BaseAddrCHN_EMI[1] == NULL)
		return;

	tmp = emi_readl(IOMEM(BaseAddrCHN_EMI[0] + CHN_EMI_DRS_ST2)) & ((1 << 23) - 1);
	/* working unit:38.5ns, the counter is update every 16ms */
	value[0] = (tmp * 3850) / 16000000;

	tmp = emi_readl(IOMEM(BaseAddrCHN_EMI[0] + CHN_EMI_DRS_ST3)) & ((1 << 23) - 1);
	value[1] = (tmp * 3850) / 16000000;

	tmp = emi_readl(IOMEM(BaseAddrCHN_EMI[0] + CHN_EMI_DRS_ST4)) & ((1 << 23) - 1);
	value[2] = (tmp * 3850) / 16000000;

	if (dram_chann_num > 1) {
		tmp = emi_readl(IOMEM(BaseAddrCHN_EMI[1] + CHN_EMI_DRS_ST2)) & ((1 << 23) - 1);
		value[3] = (tmp * 3850) / 16000000;

		tmp = emi_readl(IOMEM(BaseAddrCHN_EMI[1] + CHN_EMI_DRS_ST3)) & ((1 << 23) - 1);
		value[4] = (tmp * 3850) / 16000000;

		tmp = emi_readl(IOMEM(BaseAddrCHN_EMI[1] + CHN_EMI_DRS_ST4)) & ((1 << 23) - 1);
		value[5] = (tmp * 3850) / 16000000;
	}
}


void met_record_dramc_dcm_enable_flag(void)
{
	int reg_val;

	switch (dram_chann_num) {
	case 1:
		reg_val = emi_readl(IOMEM(CH0_MISC_CG_CTRL0));
		dramc0_dcm_enable = !((reg_val >> DRAMC_CG_SHIFT) & 0x1);
		break;
	case 2:
		reg_val = emi_readl(IOMEM(CH0_MISC_CG_CTRL0));
		dramc0_dcm_enable = !((reg_val >> DRAMC_CG_SHIFT) & 0x1);

		reg_val = emi_readl(IOMEM(CH1_MISC_CG_CTRL0));
		dramc1_dcm_enable = !((reg_val >> DRAMC_CG_SHIFT) & 0x1);
		break;
	default:
		METERROR("Error: dram_chann_num = %d\n", dram_chann_num);
	}
}
