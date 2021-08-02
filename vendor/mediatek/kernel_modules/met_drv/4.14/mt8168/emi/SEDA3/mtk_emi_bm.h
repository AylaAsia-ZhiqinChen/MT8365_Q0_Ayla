/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 */

#ifndef __MT_MET_EMI_BM_H__
#define __MT_MET_EMI_BM_H__

#define EMI_VER_MAJOR  3
#define EMI_VER_MINOR  0

#define DEF_BM_RW_TYPE          (BM_BOTH_READ_WRITE)
#define NTS                     2
#define NWSCT                   4
#define NLATENCY                8
#define NTRANS                  8
#define NALL                    (3 + 2 + 1)
#define NTTYPE                  5
#define NIDX_EMI                (NTS + NWSCT + NLATENCY + NTRANS + NALL + NTTYPE)

#define NCNT                    9
#define NCH                     2
#define NIDX_DRAMC              (NCNT * NCH)
#define NIDX                            (NIDX_EMI + NIDX_DRAMC)

#define NCLK                    1
#define NARB                    8
#define NBW                     10
#define NIDX_BL                 (NCLK + NARB + NBW)

/* 1000 To Khz and 4x freq & 2x data rate for LPDDR4 */
/* 1000 To Khz and 2x freq & 2x data rate for LPDDR3*/
/* TBD: calculate emi clock rate from DRAM DATA RATE */

/*dram baseclock/EMI clock  :	LP4=4	LP3=2	*/
#define DRAM_EMI_BASECLOCK_RATE_LP4     4
#define DRAM_EMI_BASECLOCK_RATE_LP3     2
/*dram io width  :	LP4=x16		LP3=x32 */
#define DRAM_IO_BUS_WIDTH_LP4           16
#define DRAM_IO_BUS_WIDTH_LP3           32
/*dram datarate  :	DDR=double */
#define DRAM_DATARATE   2

#define ADDR_EMI        ((unsigned long)BaseAddrEMI)

static const char of_emi_desc[] = "mediatek,emi";
static const char of_chn_emi_desc[] = "mediatek,chn_emi";
static const char of_dramc_desc[] = "mediatek,dramc";

#define BM_MASTER_M0            (0x01)
#define BM_MASTER_M1            (0x02)
#define BM_MASTER_M2            (0x04)
#define BM_MASTER_M3            (0x08)
#define BM_MASTER_M4            (0x10)
#define BM_MASTER_M5            (0x20)
#define BM_MASTER_M6            (0x40)
#define BM_MASTER_M7            (0x80)
#define BM_MASTER_ALL           (0xFF)


enum BM_RW_Type {
	BM_BOTH_READ_WRITE,
	BM_READ_ONLY,
	BM_WRITE_ONLY
};

enum {
	BM_TRANS_TYPE_1BEAT = 0x0,
	BM_TRANS_TYPE_2BEAT,
	BM_TRANS_TYPE_3BEAT,
	BM_TRANS_TYPE_4BEAT,
	BM_TRANS_TYPE_5BEAT,
	BM_TRANS_TYPE_6BEAT,
	BM_TRANS_TYPE_7BEAT,
	BM_TRANS_TYPE_8BEAT,
	BM_TRANS_TYPE_9BEAT,
	BM_TRANS_TYPE_10BEAT,
	BM_TRANS_TYPE_11BEAT,
	BM_TRANS_TYPE_12BEAT,
	BM_TRANS_TYPE_13BEAT,
	BM_TRANS_TYPE_14BEAT,
	BM_TRANS_TYPE_15BEAT,
	BM_TRANS_TYPE_16BEAT,
	BM_TRANS_TYPE_1Byte = 0 << 4,
	BM_TRANS_TYPE_2Byte = 1 << 4,
	BM_TRANS_TYPE_4Byte = 2 << 4,
	BM_TRANS_TYPE_8Byte = 3 << 4,
	BM_TRANS_TYPE_16Byte = 4 << 4,
	BM_TRANS_TYPE_32Byte = 5 << 4,
	BM_TRANS_TYPE_BURST_WRAP = 0 << 7,
	BM_TRANS_TYPE_BURST_INCR = 1 << 7
};

enum {
	BM_TRANS_RW_DEFAULT = 0x0,
	BM_TRANS_RW_READONLY,
	BM_TRANS_RW_WRITEONLY,
	BM_TRANS_RW_RWBOTH
};


/*coda busid 12bit, but HW support 16 bit*/
#define EMI_BMID_MASK				(0xFFFF)
#define BM_COUNTER_MAX				(21)

/*
*#define BUS_MON_EN		(0x00000001)
*#define BUS_MON_PAUSE		(0x00000002)
*#define BUS_MON_IDLE		(0x00000008)
*#define BC_OVERRUN		(0x00000100)
*/
enum {
	BUS_MON_EN_SHIFT = 0,
	BUS_MON_PAUSE_SHIFT = 1,
	BUS_MON_IDLE_SHIFT = 3,
	BC_OVERRUN_SHIFT = 8,
	DRAMC_CG_SHIFT = 9,
};

#define BM_REQ_OK				(0)
#define BM_ERR_WRONG_REQ			(-1)
#define BM_ERR_OVERRUN				(-2)

#define BM_WSCT_TSCT_IDSEL_ENABLE		(0)
#define BM_WSCT_TSCT_IDSEL_DISABLE		(-1)
#define BM_TTYPE1_16_ENABLE			(0)
#define BM_TTYPE1_16_DISABLE			(-1)
#define BM_TTYPE17_21_ENABLE			(0)
#define BM_TTYPE17_21_DISABLE			(-1)
#define BM_BW_LIMITER_ENABLE			(0)
#define BM_BW_LIMITER_DISABLE			(-1)

#define M0_DOUBLE_HALF_BW_1CH	(0x0)
#define M0_DOUBLE_HALF_BW_2CH	(0x1)
#define M0_DOUBLE_HALF_BW_4CH	(0x2)

/* EMI Rank configuration */
enum {
	DISABLE_DUAL_RANK_MODE = 0,
	ENABLE_DUAL_RANK_MODE,
};

#define RANK_MASK 0x1
#define ONE_RANK 1
#define DUAL_RANK 2


#if defined(CONFIG_MTK_TINYSYS_SSPM_SUPPORT) && defined(ONDIEMET_SUPPORT)
/*ondiemet emi ipi command*/
enum BM_EMI_IPI_Type {
	SET_BASE_EMI = 0x0,
	SET_BASE_DRAMC0,
	SET_BASE_DRAMC1,
	SET_BASE_DRAMC2,
	SET_BASE_DRAMC3,
	SET_BASE_DDRPHY0AO,
	SET_BASE_DRAMC0_AO,
	SET_EBM_CONFIGS1,
	SET_EBM_CONFIGS2,
	SET_REGISTER_CB,
};
#endif


#define	EMI_OFF			0x0000
#define EMI_CONA		(0x000-EMI_OFF)
#define EMI_CONH		(0x038-EMI_OFF)
#define EMI_CONH_2ND		(0x03C-EMI_OFF)
#define EMI_CONM		(0x060-EMI_OFF)
#define EMI_CONO		(0x070-EMI_OFF)

#define EMI_MDCT		(0x078 - EMI_OFF)
#define EMI_MDCT_2ND		(0x07C - EMI_OFF)

#define EMI_ARBA		(0x100 - EMI_OFF)
#define EMI_ARBB		(0x108 - EMI_OFF)
#define EMI_ARBC		(0x110 - EMI_OFF)
#define EMI_ARBD		(0x118 - EMI_OFF)
#define EMI_ARBE		(0x120 - EMI_OFF)
#define EMI_ARBF		(0x128 - EMI_OFF)
#define EMI_ARBG		(0x130 - EMI_OFF)
#define EMI_ARBG_2ND		(0x134 - EMI_OFF)
#define EMI_ARBH		(0x138 - EMI_OFF)

#define EMI_BMEN		(0x400 - EMI_OFF)
#define EMI_BCNT		(0x408 - EMI_OFF)
#define EMI_TACT		(0x410 - EMI_OFF)
#define EMI_TSCT		(0x418 - EMI_OFF)
#define EMI_WACT		(0x420 - EMI_OFF)
#define EMI_WSCT		(0x428 - EMI_OFF)
#define EMI_BACT		(0x430 - EMI_OFF)
#define EMI_BSCT		(0x438 - EMI_OFF)

#define EMI_MSEL		(0x440 - EMI_OFF)
#define EMI_TSCT2		(0x448 - EMI_OFF)
#define EMI_TSCT3		(0x450 - EMI_OFF)
#define EMI_WSCT2		(0x458 - EMI_OFF)
#define EMI_WSCT3		(0x460 - EMI_OFF)
#define EMI_WSCT4		(0x464 - EMI_OFF)
#define EMI_MSEL2		(0x468 - EMI_OFF)
#define EMI_MSEL3		(0x470 - EMI_OFF)
#define EMI_MSEL4		(0x478 - EMI_OFF)
#define EMI_MSEL5		(0x480 - EMI_OFF)
#define EMI_MSEL6		(0x488 - EMI_OFF)
#define EMI_MSEL7		(0x490 - EMI_OFF)
#define EMI_MSEL8		(0x498 - EMI_OFF)
#define EMI_MSEL9		(0x4A0 - EMI_OFF)
#define EMI_MSEL10		(0x4A8 - EMI_OFF)

#define EMI_BMID0		(0x4B0 - EMI_OFF)
#define EMI_BMID1		(0x4B4 - EMI_OFF)
#define EMI_BMID2		(0x4B8 - EMI_OFF)
#define EMI_BMID3		(0x4BC - EMI_OFF)
#define EMI_BMID4		(0x4C0 - EMI_OFF)
#define EMI_BMID5		(0x4C4 - EMI_OFF)
#define EMI_BMID6		(0x4C8 - EMI_OFF)
#define EMI_BMID7		(0x4CC - EMI_OFF)
#define EMI_BMID8		(0x4D0 - EMI_OFF)
#define EMI_BMID9		(0x4D4 - EMI_OFF)
#define EMI_BMID10		(0x4D8 - EMI_OFF)

#define EMI_BMEN1		(0x4E0 - EMI_OFF)
#define EMI_BMEN2		(0x4E8 - EMI_OFF)
#define EMI_BMRW0		(0x4F8 - EMI_OFF)
#define EMI_BMRW1		(0x4FC - EMI_OFF)
#define EMI_TTYPE1		(0x500 - EMI_OFF)
#define EMI_TTYPE2		(0x508 - EMI_OFF)
#define EMI_TTYPE3		(0x510 - EMI_OFF)
#define EMI_TTYPE4		(0x518 - EMI_OFF)
#define EMI_TTYPE5		(0x520 - EMI_OFF)
#define EMI_TTYPE6		(0x528 - EMI_OFF)
#define EMI_TTYPE7		(0x530 - EMI_OFF)
#define EMI_TTYPE8		(0x538 - EMI_OFF)
#define EMI_TTYPE9		(0x540 - EMI_OFF)
#define EMI_TTYPE10		(0x548 - EMI_OFF)
#define EMI_TTYPE11		(0x550 - EMI_OFF)
#define EMI_TTYPE12		(0x558 - EMI_OFF)
#define EMI_TTYPE13		(0x560 - EMI_OFF)
#define EMI_TTYPE14		(0x568 - EMI_OFF)
#define EMI_TTYPE15		(0x570 - EMI_OFF)
#define EMI_TTYPE16		(0x578 - EMI_OFF)
#define EMI_TTYPE17		(0x580 - EMI_OFF)
#define EMI_TTYPE18		(0x588 - EMI_OFF)
#define EMI_TTYPE19		(0x590 - EMI_OFF)
#define EMI_TTYPE20		(0x598 - EMI_OFF)
#define EMI_TTYPE21		(0x5A0 - EMI_OFF)

#define EMI_BWCT0		(0x5B0 - EMI_OFF)
#define EMI_BWCT1		(0x5B4 - EMI_OFF)
#define EMI_BWCT2		(0x5B8 - EMI_OFF)
#define EMI_BWCT3		(0x5BC - EMI_OFF)
#define EMI_BWCT4		(0x5C0 - EMI_OFF)
#define EMI_BWST0		(0x5C4 - EMI_OFF)
#define EMI_BWST1		(0x5C8 - EMI_OFF)

#define EMI_BWCT0_2ND		(0x6A0 - EMI_OFF)
#define EMI_BWCT1_2ND		(0x6A4 - EMI_OFF)
#define EMI_BWST_2ND		(0x6A8 - EMI_OFF)


extern void emi_dump_reg(void);
extern int MET_BM_Init(void);
extern void MET_BM_DeInit(void);
extern void MET_BM_SaveCfg(void);
extern void MET_BM_RestoreCfg(void);
extern void MET_BM_Enable(const unsigned int enable);
extern void MET_BM_Pause(void);
extern void MET_BM_Continue(void);
extern unsigned int MET_BM_IsOverrun(void);
extern unsigned int MET_BM_GetReadWriteType(void);
extern void MET_BM_SetReadWriteType(const unsigned int ReadWriteType);
extern int MET_BM_GetBusCycCount(void);
extern unsigned int MET_BM_GetTransAllCount(void);
extern int MET_BM_GetTransCount(const unsigned int counter_num);
extern int MET_BM_GetWordAllCount(void);
extern int MET_BM_GetWordCount(const unsigned int counter_num);
extern unsigned int MET_BM_GetBandwidthWordCount(void);
extern unsigned int MET_BM_GetOverheadWordCount(void);
extern int MET_BM_GetTransTypeCount(const unsigned int counter_num);
extern int MET_BM_GetMDCT(void);
extern int MET_BM_GetMDCT_2(void);
extern int MET_BM_GetMonitorCounter(const unsigned int counter_num,
				    unsigned int *master, unsigned int *trans_type);
extern int MET_BM_SetMDCT_MDMCU(unsigned int mdmcu_rd_buf);
extern int MET_BM_SetMonitorCounter(const unsigned int counter_num,
				    const unsigned int master, const unsigned int trans_type);
extern int MET_BM_SetTtypeCounterRW(unsigned int bmrw0_val, unsigned int bmrw1_val);
extern int MET_BM_Set_WsctTsct_id_sel(unsigned int counter_num, unsigned int enable);
extern int MET_BM_SetMaster(const unsigned int counter_num, const unsigned int master);
extern int MET_BM_SetbusID_En(const unsigned int counter_num,
			      const unsigned int enable);
extern int MET_BM_SetbusID(const unsigned int counter_num,
			   const unsigned int id);
extern int MET_BM_SetUltraHighFilter(const unsigned int counter_num, const unsigned int enable);
extern int MET_BM_SetLatencyCounter(unsigned int enable);
int MET_BM_GetLatencyCycle(unsigned int *__restrict__ emi_value,
				const unsigned int begin, const unsigned int end);
extern unsigned int MET_BM_GetEmiDcm(void);
extern int MET_BM_SetEmiDcm(const unsigned int setting);
extern void MET_BM_Clear_Start(void);
extern unsigned int MET_EMI_GetDramRankNum(void);
extern unsigned int MET_EMI_GetDramRankNum_CHN1(void);

/* Config */
unsigned int MET_EMI_GetARBA(void);
unsigned int MET_EMI_GetARBB(void);
unsigned int MET_EMI_GetARBC(void);
unsigned int MET_EMI_GetARBD(void);
unsigned int MET_EMI_GetARBE(void);
unsigned int MET_EMI_GetARBF(void);
unsigned int MET_EMI_GetARBG(void);
unsigned int MET_EMI_GetARBH(void);

/* Total BW status */
extern unsigned int MET_EMI_GetBWCT0(void);
extern unsigned int MET_EMI_GetBWCT1(void);
extern unsigned int MET_EMI_GetBWCT2(void);
extern unsigned int MET_EMI_GetBWCT3(void);
extern unsigned int MET_EMI_GetBWCT4(void);
extern unsigned int MET_EMI_GetBWST0(void);
extern unsigned int MET_EMI_GetBWST1(void);
/* C+G BW */
extern unsigned int MET_EMI_GetBWCT0_2ND(void);
extern unsigned int MET_EMI_GetBWCT1_2ND(void);
extern unsigned int MET_EMI_GetBWST_2ND(void);

unsigned int MET_EMI_GetBMRW0(void);
unsigned int MET_EMI_GetDramChannNum(void);

/* ondiemet*/
void MET_BM_IPI_baseaddr(void);
void met_emi_phyaddr_debug(void);

extern void met_record_dramc_dcm_enable_flag(void);

/* get DRS registers content */
#define CHN_EMI_DRS_ST2 0x17C
#define CHN_EMI_DRS_ST3 0x180
#define CHN_EMI_DRS_ST4 0x184
extern void met_get_drs_registers(uint32_t *value);

#endif                          /* !__MT_MET_EMI_BM_H__ */
