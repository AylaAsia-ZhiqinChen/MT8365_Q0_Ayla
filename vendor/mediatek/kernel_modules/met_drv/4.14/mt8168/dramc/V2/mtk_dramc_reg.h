/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 */
#ifndef __MTK_DRAMC_REG_H__
#define __MTK_DRAMC_REG_H__
#include <linux/compiler.h>

#define DRAMC_VER 2

#define MAX_DRAMC_CHANN 4
extern void __iomem *BaseAddrDRAMC[MAX_DRAMC_CHANN];
extern void __iomem *BaseAddrDDRPHY_AO[2];
extern void __iomem *BaseAddrDRAMC0_AO;

enum BM_DRAMC_DTS_INDEX {
	DRAMC_DTS_DRAMC0_AO = 0x0,
	DRAMC_DTS_DRAMC0_NAO = 0x4,
	DRAMC_DTS_DRAMC1_NAO = 0x5,
	DRAMC_DTS_DRAMC2_NAO = 0x6,
	DRAMC_DTS_DRAMC3_NAO = 0x7,
	DRAMC_DTS_DDRPHY0_AO = 0x8,
};

enum DRAMC_Debug_Type {
	BM_FREERUN_26M = 0x0,
	BM_RK0_PRE_STANDBY,
	BM_RK0_PRE_POWERDOWN,
	BM_RK0_ACT_STANDBY,
	BM_RK0_ACT_POWERDOWN,
	BM_RK1_PRE_STANDBY,
	BM_RK1_PRE_POWERDOWN,
	BM_RK1_ACT_STANDBY,
	BM_RK1_ACT_POWERDOWN,
	BM_RK2_PRE_STANDBY,
	BM_RK2_PRE_POWERDOWN,
	BM_RK2_ACT_STANDBY,
	BM_RK2_ACT_POWERDOWN,
	DRAMC_Debug_MAX_CNT
};

enum DRAMC_Cnt_Type {
	DRAMC_R2R,
	DRAMC_R2W,
	DRAMC_W2R,
	DRAMC_W2W,
	DRAMC_ALL
};

#define ADDR_DRAMC0     ((unsigned long)BaseAddrDRAMC[0])
#define ADDR_DRAMC1     ((unsigned long)BaseAddrDRAMC[1])
#define ADDR_DRAMC2     ((unsigned long)BaseAddrDRAMC[2])
#define ADDR_DRAMC3     ((unsigned long)BaseAddrDRAMC[3])

#define DRAMC_MISC_STATUSA      0x80
#define DRAMC_DCM_CTRL		0x28C
#define DRAMC_REFRESH_POP       0x300
#define DRAMC_FREERUN_26M       0x304
#define DRAMC_R2R_PAGE_HIT      0x30C
#define DRAMC_R2R_PAGE_MISS     0x310
#define DRAMC_R2R_INTERBANK     0x314
#define DRAMC_R2W_PAGE_HIT      0x318
#define DRAMC_R2W_PAGE_MISS     0x31C
#define DRAMC_R2W_INTERBANK     0x320
#define DRAMC_W2R_PAGE_HIT      0x324
#define DRAMC_W2R_PAGE_MISS     0x328
#define DRAMC_W2R_INTERBANK     0x32C
#define DRAMC_W2W_PAGE_HIT      0x330
#define DRAMC_W2W_PAGE_MISS     0x334
#define DRAMC_W2W_INTERBANK     0x338
#define DRAMC_IDLE_COUNT        0x308
#define DRAMC_RK0_PRE_STANDBY   0x33c
#define DRAMC_RK0_PRE_POWERDOWN 0x340
#define DRAMC_RK0_ACT_STANDBY   0x344
#define DRAMC_RK0_ACT_POWERDOWN 0x348
#define DRAMC_RK1_PRE_STANDBY   0x34c
#define DRAMC_RK1_PRE_POWERDOWN 0x350
#define DRAMC_RK1_ACT_STANDBY   0x354
#define DRAMC_RK1_ACT_POWERDOWN 0x358
#define DRAMC_RK2_PRE_STANDBY   0x35c
#define DRAMC_RK2_PRE_POWERDOWN 0x360
#define DRAMC_RK2_ACT_STANDBY   0x364
#define DRAMC_RK2_ACT_POWERDOWN 0x368
#define DRAMC_READ_BYTES        0x38c
#define DRAMC_WRITE_BYTES       0x390
#define DRAMC_MISC_SPCMDRESP    0x3b8

extern unsigned int MET_DRAMC_GetPageHitCount(enum DRAMC_Cnt_Type CountType, int chann);
extern unsigned int MET_DRAMC_GetPageMissCount(enum DRAMC_Cnt_Type CountType, int chann);
extern unsigned int MET_DRAMC_GetInterbankCount(enum DRAMC_Cnt_Type CountType, int chann);
extern unsigned int MET_DRAMC_GetIdleCount(int chann);
extern unsigned int MET_DRAMC_Misc_Status(int chann);
extern unsigned int MET_DRAMC_SPCMDRESP(int chann);
extern unsigned int MET_DRAMC_RefPop(int chann);
extern unsigned int MET_DRAMC_Free26M(int chann);
extern unsigned int MET_DRAMC_RByte(int chann);
extern unsigned int MET_DRAMC_WByte(int chann);
extern unsigned int MET_DRAMC_DCM_CTRL(int chann);

/* Debug Counter status */
extern void MET_DRAMC_GetDebugCounter(int *value, int chann);


#endif /* __MTK_DRAMC_REG_H__ */
