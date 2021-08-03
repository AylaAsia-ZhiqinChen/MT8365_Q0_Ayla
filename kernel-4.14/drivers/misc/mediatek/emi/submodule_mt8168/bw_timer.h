/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _BW_TIMER_H_
#define _BW_TIMER_H_

#define MAX_CH		2
#define MAX_RK		2

struct emi_info_t {
	unsigned int dram_type;
	unsigned int ch_num;
	unsigned int rk_num;
	unsigned int rank_size[MAX_RK];
};

/*****************************************************************************
 *	Macro Definiations
 *****************************************************************************/
#define EMI_REG_BASE			(0x10219000)
#define EMI_REG_BASE_MAPPED		(CEN_EMI_BASE)

#define EMI_MDCT				(EMI_REG_BASE_MAPPED + 0x078)
#define EMI_MDCT_2ND			(EMI_REG_BASE_MAPPED + 0x07C)
#define EMI_CONM				(EMI_REG_BASE_MAPPED + 0x060)

#define EMI_ARBA				(EMI_REG_BASE_MAPPED + 0x100)
#define EMI_ARBB				(EMI_REG_BASE_MAPPED + 0x108)
#define EMI_ARBC				(EMI_REG_BASE_MAPPED + 0x110)
#define EMI_ARBD				(EMI_REG_BASE_MAPPED + 0x118)
#define EMI_ARBE				(EMI_REG_BASE_MAPPED + 0x120)
#define EMI_ARBF				(EMI_REG_BASE_MAPPED + 0x128)
#define EMI_ARBG				(EMI_REG_BASE_MAPPED + 0x130)
#define EMI_ARBH				(EMI_REG_BASE_MAPPED + 0x138)

#define EMI_BMEN				(EMI_REG_BASE_MAPPED + 0x400)
#define EMI_BCNT				(EMI_REG_BASE_MAPPED + 0x408)
#define EMI_TACT				(EMI_REG_BASE_MAPPED + 0x410)
#define EMI_TSCT				(EMI_REG_BASE_MAPPED + 0x418)
#define EMI_WACT				(EMI_REG_BASE_MAPPED + 0x420)
#define EMI_WSCT				(EMI_REG_BASE_MAPPED + 0x428)
#define EMI_BACT				(EMI_REG_BASE_MAPPED + 0x430)
#define EMI_BSCT				(EMI_REG_BASE_MAPPED + 0x438)
#define EMI_MSEL				(EMI_REG_BASE_MAPPED + 0x440)
#define EMI_TSCT2				(EMI_REG_BASE_MAPPED + 0x448)
#define EMI_TSCT3				(EMI_REG_BASE_MAPPED + 0x450)
#define EMI_WSCT2				(EMI_REG_BASE_MAPPED + 0x458)
#define EMI_WSCT3				(EMI_REG_BASE_MAPPED + 0x460)
#define EMI_WSCT4				(EMI_REG_BASE_MAPPED + 0x464)
#define EMI_MSEL2				(EMI_REG_BASE_MAPPED + 0x468)

#define EMI_BMEN2				(EMI_REG_BASE_MAPPED + 0x4E8)

#define EMI_BMRW0				(EMI_REG_BASE_MAPPED + 0x4F8)

#define EMI_TTYPE1				(EMI_REG_BASE_MAPPED + 0x500)
#define EMI_TTYPE17				(EMI_REG_BASE_MAPPED + 0x580)

#define EMI_BWVL				(EMI_REG_BASE_MAPPED + 0x7D0)
#define EMI_BWVL_2ND			(EMI_REG_BASE_MAPPED + 0x7D4)
#define EMI_BWVL_3RD			(EMI_REG_BASE_MAPPED + 0x7D8)
#define EMI_BWVL_4TH			(EMI_REG_BASE_MAPPED + 0x7DC)
#define EMI_BWVL_5TH			(EMI_REG_BASE_MAPPED + 0x7E0)

#define EMI_CH0_REG_BASE		(0x1022D000)
#define EMI_CH0_REG_BASE_MAPPED	(CHN_EMI_BASE[0])
#define EMI_CH0_DRS_ST2			(EMI_CH0_REG_BASE_MAPPED + 0x17C)
#define EMI_CH0_DRS_ST3			(EMI_CH0_REG_BASE_MAPPED + 0x180)
#define EMI_CH0_DRS_ST4			(EMI_CH0_REG_BASE_MAPPED + 0x184)

#define EMI_CH1_REG_BASE		(0x10235000)
#define EMI_CH1_REG_BASE_MAPPED	(CHN_EMI_BASE[1])
#define EMI_CH1_DRS_ST2			(EMI_CH1_REG_BASE_MAPPED + 0x17C)
#define EMI_CH1_DRS_ST3			(EMI_CH1_REG_BASE_MAPPED + 0x180)
#define EMI_CH1_DRS_ST4			(EMI_CH1_REG_BASE_MAPPED + 0x184)

/*
 * DEFAULT_VALUE
 */
#define EMI_BMEN_DEFAULT_VALUE	  (0x00010000)
#define EMI_BMEN2_DEFAULT_VALUE	  (0x02000000)
#define EMI_BMRW0_DEFAULT_VALUE	  (0xFFFFFFFF)
#define EMI_MSEL_DEFAULT_VALUE	  (0x00030024)
#define EMI_MSEL2_DEFAULT_VALUE	  (0x000000C0)
#define BC_OVERRUN				  (0x00000100)

/* EMI_BMEN */
#define BUS_MON_EN			BIT(0)
#define BUS_MON_PAUSE		BIT(1)
#define BUS_MON_IDLE		BIT(3)

#define MAX_DRAM_CH_NUM		(2)
#define DRAM_RANK_NUM		(2)
#define DRAM_PDIR_NUM		(8)
#define EMI_TTYPE_NUM		(21)
#define EMI_TSCT_NUM		(3)
#define EMI_MDCT_NUM		(2)
#define EMI_DRS_ST_NUM		(3)
#define EMI_BW_LIMIT_NUM	(8)

#define DRAMC_CG_SHIFT		(9)

#define EMI_IDX_SIZE		(1024)

#define EMI_BWVL_UNIT		(271)

/*#define MBW_BUF_LEN		  (0x800000)*/
#define DATA_CNT_PER_BLK	(35)
#define BLK_CNT_PER_BUF		(0x800)

/*****************************************************************************
 *	Type Definiations
 *****************************************************************************/
enum emi_user {
	EMI_USER_PMQOS,
	EMI_USER_MET_EMI,

	NR_EMI_USER
};

enum emibm_type {
	EMIBM_TOTAL,
	EMIBM_TOTAL_W,
	EMIBM_CPU,
	EMIBM_GPU,

	NR_EMIBM_TYPE
};

struct dram_dbg_idx {
	unsigned int misc_statusa;
	unsigned int refresh_top;
	unsigned int freerun_26m;
	unsigned int idle_cnt;
	unsigned int page_hit;
	unsigned int page_miss;
	unsigned int inter_bank;
#if 0
	unsigned int read_bytes;
	unsigned int write_bytes;
#endif
	unsigned int pdir[DRAM_PDIR_NUM];
};

struct emi_idx {
	/* for QoS/power meter */
	unsigned int bw[NR_EMIBM_TYPE];
	/* for MET */
	unsigned int data_rate;
	unsigned int bw_raw[NR_EMIBM_TYPE];
	unsigned int bact;
	unsigned int bsct;
	unsigned int bcnt;
	unsigned int tact;
	unsigned int wact;
	unsigned int dcm_ctrl;
	unsigned int ttype[EMI_TTYPE_NUM];
	unsigned int tsct[EMI_TSCT_NUM];
	unsigned int mdct[EMI_MDCT_NUM];
	unsigned int drs_st[EMI_DRS_ST_NUM * 2];
	unsigned int bw_limit[EMI_BW_LIMIT_NUM];
	struct dram_dbg_idx dram_dbg_info[MAX_DRAM_CH_NUM];
};

struct LAST_EMI_INFO_T {
	unsigned int decs_magic;
	unsigned int decs_ctrl;
	unsigned int decs_dram_type;
	unsigned int decs_diff_us;
	unsigned int mbw_buf_l;
	unsigned int mbw_buf_h;
};

struct regs_per_period {
	unsigned int sys_time_head;
	unsigned int sys_time_h;
	unsigned int emi_wsct;
	unsigned int emi_wsct2;
	unsigned int emi_wsct3;
	unsigned int emi_wsct4;
	unsigned int emi_ttype[EMI_TTYPE_NUM];
	unsigned int data_rate;
	unsigned int sys_time_tail;
};

typedef void (*emi_update_cntr_cb) (struct emi_idx *data);

extern void emi_mon_start(void);
extern void emi_mon_restart(void);
extern void emi_mon_stop(void);
extern bool emi_is_bwmon_available(void);
extern unsigned int emi_get_data_rate(void);
extern void emi_register_cb(enum emi_user user, emi_update_cntr_cb fn);
extern void emi_unregister_cb(enum emi_user user);
extern void emi_counter_read(void);
extern void emi_init(unsigned int ch_num, unsigned int emi_idx_addr);
extern unsigned int get_emi_bw(unsigned int bw_type);

/* public apis */
unsigned long long dvfsrc_get_emi_max_bw(void);
unsigned long long dvfsrc_get_emi_bw(void);

#endif
