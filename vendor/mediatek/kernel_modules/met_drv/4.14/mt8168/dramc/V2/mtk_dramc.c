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

#include "mtk_dramc_reg.h"

#define BM_REQ_OK				(0)
#define BM_ERR_WRONG_REQ			(-1)
#define BM_ERR_OVERRUN				(-2)


void __iomem *BaseAddrDRAMC[MAX_DRAMC_CHANN];
void __iomem *BaseAddrDDRPHY_AO[2];
void __iomem *BaseAddrDRAMC0_AO;


unsigned int MET_DRAMC_GetPageHitCount(enum DRAMC_Cnt_Type CountType, int chann)
{
	unsigned int iCount;
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	switch (CountType) {
	case DRAMC_R2R:
		iCount = readl(IOMEM(addr_base + DRAMC_R2R_PAGE_HIT));
		break;
	case DRAMC_R2W:
		iCount = readl(IOMEM(addr_base + DRAMC_R2W_PAGE_HIT));
		break;
	case DRAMC_W2R:
		iCount = readl(IOMEM(addr_base + DRAMC_W2R_PAGE_HIT));
		break;
	case DRAMC_W2W:
		iCount = readl(IOMEM(addr_base + DRAMC_W2W_PAGE_HIT));
		break;
	case DRAMC_ALL:
		iCount = readl(IOMEM(addr_base + DRAMC_R2R_PAGE_HIT)) +
			 readl(IOMEM(addr_base + DRAMC_R2W_PAGE_HIT)) +
			 readl(IOMEM(addr_base + DRAMC_W2R_PAGE_HIT)) +
			 readl(IOMEM(addr_base + DRAMC_W2W_PAGE_HIT));
		break;
	default:
		return BM_ERR_WRONG_REQ;
	}

	return iCount;
}

unsigned int MET_DRAMC_GetPageMissCount(enum DRAMC_Cnt_Type CountType, int chann)
{
	unsigned int iCount;
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	switch (CountType) {
	case DRAMC_R2R:
		iCount = readl(IOMEM(addr_base + DRAMC_R2R_PAGE_MISS));
		break;
	case DRAMC_R2W:
		iCount = readl(IOMEM(addr_base + DRAMC_R2W_PAGE_MISS));
		break;
	case DRAMC_W2R:
		iCount = readl(IOMEM(addr_base + DRAMC_W2R_PAGE_MISS));
		break;
	case DRAMC_W2W:
		iCount = readl(IOMEM(addr_base + DRAMC_W2W_PAGE_MISS));
		break;
	case DRAMC_ALL:
		iCount = readl(IOMEM(addr_base + DRAMC_R2R_PAGE_MISS)) +
			 readl(IOMEM(addr_base + DRAMC_R2W_PAGE_MISS)) +
			 readl(IOMEM(addr_base + DRAMC_W2R_PAGE_MISS)) +
			 readl(IOMEM(addr_base + DRAMC_W2W_PAGE_MISS));
		break;
	default:
		return BM_ERR_WRONG_REQ;
	}

	return iCount;
}


unsigned int MET_DRAMC_GetInterbankCount(enum DRAMC_Cnt_Type CountType, int chann)
{
	unsigned int iCount;
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	switch (CountType) {
	case DRAMC_R2R:
		iCount = readl(IOMEM(addr_base + DRAMC_R2R_INTERBANK));
		break;
	case DRAMC_R2W:
		iCount = readl(IOMEM(addr_base + DRAMC_R2W_INTERBANK));
		break;
	case DRAMC_W2R:
		iCount = readl(IOMEM(addr_base + DRAMC_W2R_INTERBANK));
		break;
	case DRAMC_W2W:
		iCount = readl(IOMEM(addr_base + DRAMC_W2W_INTERBANK));
		break;
	case DRAMC_ALL:
		iCount = readl(IOMEM(addr_base + DRAMC_R2R_INTERBANK)) +
			 readl(IOMEM(addr_base + DRAMC_R2W_INTERBANK)) +
			 readl(IOMEM(addr_base + DRAMC_W2R_INTERBANK)) +
			 readl(IOMEM(addr_base + DRAMC_W2W_INTERBANK));
		break;
	default:
		return BM_ERR_WRONG_REQ;
	}

	return iCount;
}


unsigned int MET_DRAMC_GetIdleCount(int chann)
{
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	return readl(IOMEM(addr_base + DRAMC_IDLE_COUNT));
}


unsigned int MET_DRAMC_Misc_Status(int chann)
{
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	return readl(IOMEM(addr_base + DRAMC_MISC_STATUSA));
}


unsigned int MET_DRAMC_SPCMDRESP(int chann)
{
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	return readl(IOMEM(addr_base + DRAMC_MISC_SPCMDRESP));
}


unsigned int MET_DRAMC_RefPop(int chann)
{
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	return readl(IOMEM(addr_base + DRAMC_REFRESH_POP));
}


unsigned int MET_DRAMC_Free26M(int chann)
{
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	return readl(IOMEM(addr_base + DRAMC_FREERUN_26M));
}


unsigned int MET_DRAMC_RByte(int chann)
{
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	return readl(IOMEM(addr_base + DRAMC_READ_BYTES));
}


unsigned int MET_DRAMC_WByte(int chann)
{
	unsigned long addr_base = ADDR_DRAMC0;

	if (chann == 0)
		addr_base = ADDR_DRAMC0;
	else if (chann == 1)
		addr_base = ADDR_DRAMC1;
	else if (chann == 2)
		addr_base = ADDR_DRAMC2;
	else if (chann == 3)
		addr_base = ADDR_DRAMC3;
	else
		return BM_ERR_WRONG_REQ;

	return readl(IOMEM(addr_base + DRAMC_WRITE_BYTES));
}


unsigned int MET_DRAMC_DCM_CTRL(int chann)
{
	unsigned long addr_base;

	if (chann == 0)
		addr_base = (unsigned long)BaseAddrDDRPHY_AO[0];
	else if (chann == 1)
		addr_base = (unsigned long)BaseAddrDDRPHY_AO[1];
	else
		return BM_ERR_WRONG_REQ;

	/* CH0_MEM_DCM_CTRL[25:21] DCM slowdown factor */
	return ((readl(IOMEM(addr_base + DRAMC_DCM_CTRL)) & 0x03E00000) >> 21);
}


void MET_DRAMC_GetDebugCounter(int *value, int chann)
{
	int i;
	unsigned long addr_base = ADDR_DRAMC0;

	for (i = 0; i < chann; i++) {
		if (i == 0)
			addr_base = ADDR_DRAMC0;
		else if (i == 1)
			addr_base = ADDR_DRAMC1;
		else if (i == 2)
			addr_base = ADDR_DRAMC2;
		else if (i == 3)
			addr_base = ADDR_DRAMC3;
		else
			return;

		value[DRAMC_Debug_MAX_CNT * i + BM_FREERUN_26M] = MET_DRAMC_Free26M(i);

		value[DRAMC_Debug_MAX_CNT * i + BM_RK0_PRE_STANDBY] =
			readl(IOMEM(addr_base + DRAMC_RK0_PRE_STANDBY));
		value[DRAMC_Debug_MAX_CNT * i + BM_RK0_PRE_POWERDOWN] =
			readl(IOMEM(addr_base + DRAMC_RK0_PRE_POWERDOWN));
		value[DRAMC_Debug_MAX_CNT * i + BM_RK0_ACT_STANDBY] =
			readl(IOMEM(addr_base + DRAMC_RK0_ACT_STANDBY));
		value[DRAMC_Debug_MAX_CNT * i + BM_RK0_ACT_POWERDOWN] =
			readl(IOMEM(addr_base + DRAMC_RK0_ACT_POWERDOWN));

		value[DRAMC_Debug_MAX_CNT * i + BM_RK1_PRE_STANDBY] =
			readl(IOMEM(addr_base + DRAMC_RK1_PRE_STANDBY));
		value[DRAMC_Debug_MAX_CNT * i + BM_RK1_PRE_POWERDOWN] =
			readl(IOMEM(addr_base + DRAMC_RK1_PRE_POWERDOWN));
		value[DRAMC_Debug_MAX_CNT * i + BM_RK1_ACT_STANDBY] =
			readl(IOMEM(addr_base + DRAMC_RK1_ACT_STANDBY));
		value[DRAMC_Debug_MAX_CNT * i + BM_RK1_ACT_POWERDOWN] =
			readl(IOMEM(addr_base + DRAMC_RK1_ACT_POWERDOWN));

		value[DRAMC_Debug_MAX_CNT * i + BM_RK2_PRE_STANDBY] =
			readl(IOMEM(addr_base + DRAMC_RK2_PRE_STANDBY));
		value[DRAMC_Debug_MAX_CNT * i + BM_RK2_PRE_POWERDOWN] =
			readl(IOMEM(addr_base + DRAMC_RK2_PRE_POWERDOWN));
		value[DRAMC_Debug_MAX_CNT * i + BM_RK2_ACT_STANDBY] =
			readl(IOMEM(addr_base + DRAMC_RK2_ACT_STANDBY));
		value[DRAMC_Debug_MAX_CNT * i + BM_RK2_ACT_POWERDOWN] =
			readl(IOMEM(addr_base + DRAMC_RK2_ACT_POWERDOWN));
	}
}
