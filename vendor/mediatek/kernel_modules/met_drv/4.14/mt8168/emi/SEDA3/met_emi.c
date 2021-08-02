// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/string.h>

#ifdef CONFIG_ARM
//#include <asm/dma-mapping.h> /* arm_coherent_dma_ops */
#endif /* CONFIG_ARM */

#include <linux/dma-mapping.h>

#define MET_USER_EVENT_SUPPORT
#include "met_drv.h"
#include "trace.h"

#include "mtk_typedefs.h"
#include "core_plf_init.h"
#include "core_plf_trace.h"
#include "met_plf_chip_init.h"
#include "met_plf_chip_trace.h"
#include "mtk_emi_bm.h"
#include "mtk_dramc_reg.h"
#include "interface.h"

#if defined(CONFIG_MTK_TINYSYS_SSPM_SUPPORT) && defined(ONDIEMET_SUPPORT)
#include "sspm/ondiemet_sspm.h"
#endif


/*======================================================================*/
/*	Global variable definitions					*/
/*======================================================================*/
/*ondiemet emi sampling interval in us */
int emi_tsct_enable;
int emi_mdct_enable;
int emi_TP_busfiltr_enable;

int emi_use_ondiemet;
int metemi_func_opt;

int met_emi_regdump;
/* Dynamic MonitorCounter selection !!!EXPERIMENT!!! */
static int msel_enable;
static unsigned int msel_group1 = BM_MASTER_ALL;
static unsigned int msel_group2 = BM_MASTER_ALL;
static unsigned int msel_group3 = BM_MASTER_ALL;

/* CVS Added changeable buffer for testing */
static int mdmcu_sel_enable;
static unsigned int rd_mdmcu_rsv_num = 0x5;

/* Global variables */
static struct kobject *kobj_emi;
static int rwtype = BM_BOTH_READ_WRITE;

/* BW Limiter */
/*#define CNT_COUNTDOWN	(1000-1)*/		/* 1000 * 1ms = 1sec */
#define CNT_COUNTDOWN   (0)                     /* 1ms */
static int countdown;
static int bw_limiter_enable = BM_BW_LIMITER_ENABLE;

/* TTYPE counter */
static int ttype1_16_en = BM_TTYPE1_16_DISABLE;
static int ttype17_21_en = BM_TTYPE17_21_DISABLE;

static int dramc_pdir_enable;
static int dram_chann_num = 1;

enum SSPM_Mode {
	CUSTOMER_MODE = 0x0,
	UNDEFINE_MODE = 0x1,
	INTERNAL_MODE = 0X2780
};


/*======================================================================*/
/*	EMI Test Operations						*/
/*======================================================================*/
static int times;

static ssize_t test_apmcu_store(struct kobject *kobj,
				struct kobj_attribute *attr,
				const char *buf,
				size_t n)
{
	int i;
	unsigned int    *src_addr_v = NULL;
	dma_addr_t src_addr_p;
#ifdef CONFIG_ARM
	struct dma_map_ops *ops = (struct dma_map_ops *)symbol_get(arm_dma_ops);
#endif /* CONFIG_ARM */

	if ((n == 0) || (buf == NULL))
		return -EINVAL;
	if (kstrtoint(buf, 10, &times) != 0)
		return -EINVAL;
	if (times < 0)
		return -EINVAL;

	if (times > 5000)       /* Less than 20MB */
		return -EINVAL;

#ifdef CONFIG_ARM
	if (ops && ops->alloc) {
		(met_device.this_device)->coherent_dma_mask = DMA_BIT_MASK(32);
		src_addr_v = ops->alloc(met_device.this_device,
						PAGE_SIZE,
						&src_addr_p,
						GFP_KERNEL,
						0);
	}
#endif /* CONFIG_ARM */

#ifdef CONFIG_ARM64
	/* dma_alloc */
 	src_addr_v = dma_alloc_coherent(met_device.this_device,
 					PAGE_SIZE,
 					&src_addr_p,
 					GFP_KERNEL);
#endif /* CONFIG_ARM64 */

	if (src_addr_v == NULL) {
#ifdef CONFIG_MET_MODULE
		met_tag_oneshot_real(0, "test_apmcu dma alloc fail", PAGE_SIZE);
#else
		met_tag_oneshot(0, "test_apmcu dma alloc fail", PAGE_SIZE);
#endif
		return -ENOMEM;
	}
	/* testing */
	preempt_disable();
#ifdef CONFIG_MET_MODULE
	met_tag_start_real(0, "TEST_EMI_APMCU");
#else
	met_tag_start(0, "TEST_EMI_APMCU");
#endif
	for (i = 0; i < times; i++) {
		memset(src_addr_v, 2 * i, PAGE_SIZE);
#ifdef CONFIG_MET_MODULE
		met_tag_oneshot_real(0, "TEST_EMI_APMCU", PAGE_SIZE);
#else
		met_tag_oneshot(0, "TEST_EMI_APMCU", PAGE_SIZE);
#endif
	}
#ifdef CONFIG_MET_MODULE
	met_tag_end_real(0, "TEST_EMI_APMCU");
#else
	met_tag_end(0, "TEST_EMI_APMCU");
#endif
	/* the following function has no defined if MET is built as module */
	/* preempt_enable_no_resched(); */
	/* use this one to replace it: see met_drv.h */
	my_preempt_enable();

#ifdef CONFIG_ARM
	/* dma_free */
	if (ops && ops->free) {
		ops->free(met_device.this_device,
				  PAGE_SIZE,
				  src_addr_v,
				  src_addr_p,
			0);
	}
#endif /* CONFIG_ARM */

#ifdef CONFIG_ARM64
	/* dma_free */
 	if (src_addr_v != NULL)
 		dma_free_coherent(met_device.this_device,
 				  PAGE_SIZE,
 				  src_addr_v,
				  src_addr_p);
#endif /* CONFIG_ARM64 */

	return n;
}

/*======================================================================*/
/*	KOBJ Declarations						*/
/*======================================================================*/
DECLARE_KOBJ_ATTR_INT(emi_tsct_enable, emi_tsct_enable);
DECLARE_KOBJ_ATTR_INT(emi_mdct_enable, emi_mdct_enable);
DECLARE_KOBJ_ATTR_INT(emi_TP_busfiltr_enable, emi_TP_busfiltr_enable);
DECLARE_KOBJ_ATTR_INT(metemi_func_opt, metemi_func_opt);
DECLARE_KOBJ_ATTR_INT(emi_regdump, met_emi_regdump);
DECLARE_KOBJ_ATTR_INT(msel_enable, msel_enable);
DECLARE_KOBJ_ATTR_HEX_CHECK(msel_group1, msel_group1, msel_group1 > 0 && msel_group1 <= BM_MASTER_ALL);
DECLARE_KOBJ_ATTR_HEX_CHECK(msel_group2, msel_group2, msel_group2 > 0 && msel_group2 <= BM_MASTER_ALL);
DECLARE_KOBJ_ATTR_HEX_CHECK(msel_group3, msel_group3, msel_group3 > 0 && msel_group3 <= BM_MASTER_ALL);
DECLARE_KOBJ_ATTR_INT(mdmcu_sel_enable, mdmcu_sel_enable);
DECLARE_KOBJ_ATTR_INT(rd_mdmcu_rsv_num, rd_mdmcu_rsv_num);


/* KOBJ: rwtype */
DECLARE_KOBJ_ATTR_INT_CHECK(rwtype, rwtype, rwtype >= 0 && rwtype <= BM_WRITE_ONLY);

static unsigned int get_emi_clock_rate(unsigned int dram_data_rate_MHz)
{
	/*
	 *	the ddr type define :
	 *	enum DDRTYPE {
	 *	TYPE_LPDDR3 = 1,
	 *	TYPE_LPDDR4,
	 *	TYPE_LPDDR4X,
	 *	TYPE_LPDDR2
	 *	};
	 */

	unsigned int DRAM_TYPE;

	if (get_ddr_type_symbol) {
		DRAM_TYPE = get_ddr_type_symbol();

		if ((DRAM_TYPE == 2) || (DRAM_TYPE == 3))
			return dram_data_rate_MHz / DRAM_EMI_BASECLOCK_RATE_LP4 / DRAM_DATARATE;
		else
			return dram_data_rate_MHz / DRAM_EMI_BASECLOCK_RATE_LP3 / DRAM_DATARATE;
	} else {
		METERROR("[%s][%d]get_ddr_type_symbol = NULL , use the TYPE_LPDDR3 setting\n", __func__, __LINE__);
		return dram_data_rate_MHz / DRAM_EMI_BASECLOCK_RATE_LP3 / DRAM_DATARATE;
	}
}

/* KOBJ: emi_clock_rate */
static ssize_t emi_clock_rate_show(struct kobject *kobj,
				   struct kobj_attribute *attr,
				   char *buf)
{
	unsigned int dram_data_rate_MHz;

	if (get_dram_data_rate_symbol) {
		dram_data_rate_MHz = get_dram_data_rate_symbol();
	} else {
		METERROR("get_dram_data_rate_symbol = NULL\n");
		dram_data_rate_MHz = 0;
	}

	return snprintf(buf, PAGE_SIZE, "%d\n",
			get_emi_clock_rate(dram_data_rate_MHz));
}

DECLARE_KOBJ_ATTR_RO(emi_clock_rate);

/* KOBJ: ttype1_16_en */
DECLARE_KOBJ_ATTR_STR_LIST_ITEM(
	ttype1_16_en,
	KOBJ_ITEM_LIST(
		{ BM_TTYPE1_16_ENABLE,   "ENABLE" },
		{ BM_TTYPE1_16_DISABLE,  "DISABLE" }
		)
	);
DECLARE_KOBJ_ATTR_STR_LIST(ttype1_16_en, ttype1_16_en, ttype1_16_en);

/* KOBJ: ttype17_21_en */
DECLARE_KOBJ_ATTR_STR_LIST_ITEM(
	ttype17_21_en,
	KOBJ_ITEM_LIST(
		{ BM_TTYPE17_21_ENABLE,  "ENABLE" },
		{ BM_TTYPE17_21_DISABLE, "DISABLE" }
		)
	);
DECLARE_KOBJ_ATTR_STR_LIST(ttype17_21_en, ttype17_21_en, ttype17_21_en);

/* KOBJ: bw_limiter_enable */
DECLARE_KOBJ_ATTR_STR_LIST_ITEM(
	bw_limiter_enable,
	KOBJ_ITEM_LIST(
		{ BM_BW_LIMITER_ENABLE,  "ENABLE" },
		{ BM_BW_LIMITER_DISABLE, "DISABLE" }
		)
	);

DECLARE_KOBJ_ATTR_STR_LIST(bw_limiter_enable, bw_limiter_enable, bw_limiter_enable);

/* KOBJ: ttype_master */
DECLARE_KOBJ_ATTR_STR_LIST_ITEM(
	ttype_master,
	KOBJ_ITEM_LIST(
		{ BM_MASTER_M0,  "M0" },
		{ BM_MASTER_M1,  "M1" },
		{ BM_MASTER_M2,  "M2" },
		{ BM_MASTER_M3,  "M3" },
		{ BM_MASTER_M4,  "M4" },
		{ BM_MASTER_M5,  "M5" },
		{ BM_MASTER_M6,  "M6" },
		{ BM_MASTER_M7,  "M7" }
		)
	);


/* KOBJ: ttypeX_nbeat, ttypeX_nbyte, ttypeX_burst */
DECLARE_KOBJ_ATTR_INT_LIST_ITEM(
	ttype_nbeat,
	KOBJ_ITEM_LIST(
		{ BM_TRANS_TYPE_1BEAT,   1 },
		{ BM_TRANS_TYPE_2BEAT,   2 },
		{ BM_TRANS_TYPE_3BEAT,   3 },
		{ BM_TRANS_TYPE_4BEAT,   4 },
		{ BM_TRANS_TYPE_5BEAT,   5 },
		{ BM_TRANS_TYPE_6BEAT,   6 },
		{ BM_TRANS_TYPE_7BEAT,   7 },
		{ BM_TRANS_TYPE_8BEAT,   8 },
		{ BM_TRANS_TYPE_9BEAT,   9 },
		{ BM_TRANS_TYPE_10BEAT,  10 },
		{ BM_TRANS_TYPE_11BEAT,  11 },
		{ BM_TRANS_TYPE_12BEAT,  12 },
		{ BM_TRANS_TYPE_13BEAT,  13 },
		{ BM_TRANS_TYPE_14BEAT,  14 },
		{ BM_TRANS_TYPE_15BEAT,  15 },
		{ BM_TRANS_TYPE_16BEAT,  16 }
		)
	);
DECLARE_KOBJ_ATTR_INT_LIST_ITEM(
	ttype_nbyte,
	KOBJ_ITEM_LIST(
		{ BM_TRANS_TYPE_1Byte,   1 },
		{ BM_TRANS_TYPE_2Byte,   2 },
		{ BM_TRANS_TYPE_4Byte,   4 },
		{ BM_TRANS_TYPE_8Byte,   8 },
		{ BM_TRANS_TYPE_16Byte,  16 },
		{ BM_TRANS_TYPE_32Byte,  32 }
		)
	);
DECLARE_KOBJ_ATTR_STR_LIST_ITEM(
	ttype_burst,
	KOBJ_ITEM_LIST(
		{ BM_TRANS_TYPE_BURST_INCR,      "INCR" },
		{ BM_TRANS_TYPE_BURST_WRAP,      "WRAP" }
		)
	);

DECLARE_KOBJ_ATTR_STR_LIST_ITEM(
	ttype_rw,
	KOBJ_ITEM_LIST(
		{ BM_TRANS_RW_DEFAULT,   "DEFAULT" },
		{ BM_TRANS_RW_READONLY,  "R" },
		{ BM_TRANS_RW_WRITEONLY, "W" },
		{ BM_TRANS_RW_RWBOTH,    "BOTH" }
		)
	);

/* KOBJ: test_apmcu */
DECLARE_KOBJ_ATTR_SHOW_INT(test_apmcu, times);
/* please refer to session: "EMI Test Operations" for store operation */
DECLARE_KOBJ_ATTR(test_apmcu);

DECLARE_KOBJ_ATTR_INT(dramc_pdir_enable, dramc_pdir_enable);

/*enable high priority filter*/
static int high_priority_filter;
DECLARE_KOBJ_ATTR_HEX(high_priority_filter, high_priority_filter);


/**/
static int ttype_master_val[21];
static int ttype_busid_val[21];
static int ttype_nbeat_val[21];
static int ttype_nbyte_val[21];
static int ttype_burst_val[21];
static int ttype_rw_val[21];

#define DECLARE_KOBJ_TTYPE_MASTER(nr) \
	DECLARE_KOBJ_ATTR_STR_LIST(ttype ## nr ## _master, ttype_master_val[nr - 1], ttype_master)

#define DECLARE_KOBJ_TTYPE_NBEAT(nr) \
	DECLARE_KOBJ_ATTR_INT_LIST(ttype ## nr ## _nbeat, ttype_nbeat_val[nr - 1], ttype_nbeat)

#define DECLARE_KOBJ_TTYPE_NBYTE(nr) \
	DECLARE_KOBJ_ATTR_INT_LIST(ttype ## nr ## _nbyte, ttype_nbyte_val[nr - 1], ttype_nbyte)

#define DECLARE_KOBJ_TTYPE_BURST(nr) \
	DECLARE_KOBJ_ATTR_STR_LIST(ttype ## nr ## _burst, ttype_burst_val[nr - 1], ttype_burst)

#define DECLARE_KOBJ_TTYPE_RW(nr) \
	DECLARE_KOBJ_ATTR_STR_LIST(ttype ## nr ## _rw, ttype_rw_val[nr - 1], ttype_rw)

#define DECLARE_KOBJ_TTYPE_BUSID_VAL(nr) \
	DECLARE_KOBJ_ATTR_HEX(ttype ## nr ## _busid, ttype_busid_val[nr - 1])

DECLARE_KOBJ_TTYPE_MASTER(1);
DECLARE_KOBJ_TTYPE_NBEAT(1);
DECLARE_KOBJ_TTYPE_NBYTE(1);
DECLARE_KOBJ_TTYPE_BURST(1);
DECLARE_KOBJ_TTYPE_RW(1);
DECLARE_KOBJ_TTYPE_BUSID_VAL(1);

DECLARE_KOBJ_TTYPE_MASTER(2);
DECLARE_KOBJ_TTYPE_NBEAT(2);
DECLARE_KOBJ_TTYPE_NBYTE(2);
DECLARE_KOBJ_TTYPE_BURST(2);
DECLARE_KOBJ_TTYPE_RW(2);
DECLARE_KOBJ_TTYPE_BUSID_VAL(2);

DECLARE_KOBJ_TTYPE_MASTER(3);
DECLARE_KOBJ_TTYPE_NBEAT(3);
DECLARE_KOBJ_TTYPE_NBYTE(3);
DECLARE_KOBJ_TTYPE_BURST(3);
DECLARE_KOBJ_TTYPE_RW(3);
DECLARE_KOBJ_TTYPE_BUSID_VAL(3);

DECLARE_KOBJ_TTYPE_MASTER(4);
DECLARE_KOBJ_TTYPE_NBEAT(4);
DECLARE_KOBJ_TTYPE_NBYTE(4);
DECLARE_KOBJ_TTYPE_BURST(4);
DECLARE_KOBJ_TTYPE_RW(4);
DECLARE_KOBJ_TTYPE_BUSID_VAL(4);

DECLARE_KOBJ_TTYPE_MASTER(5);
DECLARE_KOBJ_TTYPE_NBEAT(5);
DECLARE_KOBJ_TTYPE_NBYTE(5);
DECLARE_KOBJ_TTYPE_BURST(5);
DECLARE_KOBJ_TTYPE_RW(5);
DECLARE_KOBJ_TTYPE_BUSID_VAL(5);

DECLARE_KOBJ_TTYPE_MASTER(6);
DECLARE_KOBJ_TTYPE_NBEAT(6);
DECLARE_KOBJ_TTYPE_NBYTE(6);
DECLARE_KOBJ_TTYPE_BURST(6);
DECLARE_KOBJ_TTYPE_RW(6);
DECLARE_KOBJ_TTYPE_BUSID_VAL(6);

DECLARE_KOBJ_TTYPE_MASTER(7);
DECLARE_KOBJ_TTYPE_NBEAT(7);
DECLARE_KOBJ_TTYPE_NBYTE(7);
DECLARE_KOBJ_TTYPE_BURST(7);
DECLARE_KOBJ_TTYPE_RW(7);
DECLARE_KOBJ_TTYPE_BUSID_VAL(7);

DECLARE_KOBJ_TTYPE_MASTER(8);
DECLARE_KOBJ_TTYPE_NBEAT(8);
DECLARE_KOBJ_TTYPE_NBYTE(8);
DECLARE_KOBJ_TTYPE_BURST(8);
DECLARE_KOBJ_TTYPE_RW(8);
DECLARE_KOBJ_TTYPE_BUSID_VAL(8);

DECLARE_KOBJ_TTYPE_MASTER(9);
DECLARE_KOBJ_TTYPE_NBEAT(9);
DECLARE_KOBJ_TTYPE_NBYTE(9);
DECLARE_KOBJ_TTYPE_BURST(9);
DECLARE_KOBJ_TTYPE_RW(9);
DECLARE_KOBJ_TTYPE_BUSID_VAL(9);

DECLARE_KOBJ_TTYPE_MASTER(10);
DECLARE_KOBJ_TTYPE_NBEAT(10);
DECLARE_KOBJ_TTYPE_NBYTE(10);
DECLARE_KOBJ_TTYPE_BURST(10);
DECLARE_KOBJ_TTYPE_RW(10);
DECLARE_KOBJ_TTYPE_BUSID_VAL(10);

DECLARE_KOBJ_TTYPE_MASTER(11);
DECLARE_KOBJ_TTYPE_NBEAT(11);
DECLARE_KOBJ_TTYPE_NBYTE(11);
DECLARE_KOBJ_TTYPE_BURST(11);
DECLARE_KOBJ_TTYPE_RW(11);
DECLARE_KOBJ_TTYPE_BUSID_VAL(11);

DECLARE_KOBJ_TTYPE_MASTER(12);
DECLARE_KOBJ_TTYPE_NBEAT(12);
DECLARE_KOBJ_TTYPE_NBYTE(12);
DECLARE_KOBJ_TTYPE_BURST(12);
DECLARE_KOBJ_TTYPE_RW(12);
DECLARE_KOBJ_TTYPE_BUSID_VAL(12);

DECLARE_KOBJ_TTYPE_MASTER(13);
DECLARE_KOBJ_TTYPE_NBEAT(13);
DECLARE_KOBJ_TTYPE_NBYTE(13);
DECLARE_KOBJ_TTYPE_BURST(13);
DECLARE_KOBJ_TTYPE_RW(13);
DECLARE_KOBJ_TTYPE_BUSID_VAL(13);

DECLARE_KOBJ_TTYPE_MASTER(14);
DECLARE_KOBJ_TTYPE_NBEAT(14);
DECLARE_KOBJ_TTYPE_NBYTE(14);
DECLARE_KOBJ_TTYPE_BURST(14);
DECLARE_KOBJ_TTYPE_RW(14);
DECLARE_KOBJ_TTYPE_BUSID_VAL(14);

DECLARE_KOBJ_TTYPE_MASTER(15);
DECLARE_KOBJ_TTYPE_NBEAT(15);
DECLARE_KOBJ_TTYPE_NBYTE(15);
DECLARE_KOBJ_TTYPE_BURST(15);
DECLARE_KOBJ_TTYPE_RW(15);
DECLARE_KOBJ_TTYPE_BUSID_VAL(15);

DECLARE_KOBJ_TTYPE_MASTER(16);
DECLARE_KOBJ_TTYPE_NBEAT(16);
DECLARE_KOBJ_TTYPE_NBYTE(16);
DECLARE_KOBJ_TTYPE_BURST(16);
DECLARE_KOBJ_TTYPE_RW(16);
DECLARE_KOBJ_TTYPE_BUSID_VAL(16);

DECLARE_KOBJ_TTYPE_MASTER(17);
DECLARE_KOBJ_TTYPE_NBEAT(17);
DECLARE_KOBJ_TTYPE_NBYTE(17);
DECLARE_KOBJ_TTYPE_BURST(17);
DECLARE_KOBJ_TTYPE_RW(17);
DECLARE_KOBJ_TTYPE_BUSID_VAL(17);

DECLARE_KOBJ_TTYPE_MASTER(18);
DECLARE_KOBJ_TTYPE_NBEAT(18);
DECLARE_KOBJ_TTYPE_NBYTE(18);
DECLARE_KOBJ_TTYPE_BURST(18);
DECLARE_KOBJ_TTYPE_RW(18);
DECLARE_KOBJ_TTYPE_BUSID_VAL(18);

DECLARE_KOBJ_TTYPE_MASTER(19);
DECLARE_KOBJ_TTYPE_NBEAT(19);
DECLARE_KOBJ_TTYPE_NBYTE(19);
DECLARE_KOBJ_TTYPE_BURST(19);
DECLARE_KOBJ_TTYPE_RW(19);
DECLARE_KOBJ_TTYPE_BUSID_VAL(19);

DECLARE_KOBJ_TTYPE_MASTER(20);
DECLARE_KOBJ_TTYPE_NBEAT(20);
DECLARE_KOBJ_TTYPE_NBYTE(20);
DECLARE_KOBJ_TTYPE_BURST(20);
DECLARE_KOBJ_TTYPE_RW(20);
DECLARE_KOBJ_TTYPE_BUSID_VAL(20);

DECLARE_KOBJ_TTYPE_MASTER(21);
DECLARE_KOBJ_TTYPE_NBEAT(21);
DECLARE_KOBJ_TTYPE_NBYTE(21);
DECLARE_KOBJ_TTYPE_BURST(21);
DECLARE_KOBJ_TTYPE_RW(21);
DECLARE_KOBJ_TTYPE_BUSID_VAL(21);

/**/
#define KOBJ_ATTR_ITEM_SERIAL_FNODE(nr) \
	do { \
		KOBJ_ATTR_ITEM(ttype ## nr ## _master); \
		KOBJ_ATTR_ITEM(ttype ## nr ## _nbeat); \
		KOBJ_ATTR_ITEM(ttype ## nr ## _nbyte); \
		KOBJ_ATTR_ITEM(ttype ## nr ## _burst); \
		KOBJ_ATTR_ITEM(ttype ## nr ## _busid); \
		KOBJ_ATTR_ITEM(ttype ## nr ## _rw); \
	} while (0)

#define KOBJ_ATTR_LIST \
	do { \
		KOBJ_ATTR_ITEM(high_priority_filter); \
		KOBJ_ATTR_ITEM(metemi_func_opt); \
		KOBJ_ATTR_ITEM(emi_tsct_enable); \
		KOBJ_ATTR_ITEM(emi_mdct_enable); \
		KOBJ_ATTR_ITEM(emi_TP_busfiltr_enable); \
		KOBJ_ATTR_ITEM(emi_regdump); \
		KOBJ_ATTR_ITEM(msel_enable); \
		KOBJ_ATTR_ITEM(msel_group1); \
		KOBJ_ATTR_ITEM(msel_group2); \
		KOBJ_ATTR_ITEM(msel_group3); \
		KOBJ_ATTR_ITEM(emi_clock_rate); \
		KOBJ_ATTR_ITEM(rwtype); \
		KOBJ_ATTR_ITEM(ttype17_21_en); \
		KOBJ_ATTR_ITEM(ttype1_16_en); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(1); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(2); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(3); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(4); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(5); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(6); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(7); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(8); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(9); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(10); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(11); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(12); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(13); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(14); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(15); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(16); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(17); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(18); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(19); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(20); \
		KOBJ_ATTR_ITEM_SERIAL_FNODE(21); \
		KOBJ_ATTR_ITEM(test_apmcu); \
		KOBJ_ATTR_ITEM(bw_limiter_enable); \
		KOBJ_ATTR_ITEM(dramc_pdir_enable); \
		KOBJ_ATTR_ITEM(mdmcu_sel_enable); \
		KOBJ_ATTR_ITEM(rd_mdmcu_rsv_num); \
	} while (0)


/*======================================================================*/
/*	EMI Operations							*/
/*======================================================================*/
static void emi_init(void)
{
	unsigned int bmrw0_val, bmrw1_val, i, enable;
	unsigned int msel_group_val[4];

	/*save origianl EMI config*/
	MET_BM_SaveCfg();

	/* get dram channel number */
	dram_chann_num = MET_EMI_GetDramChannNum();

	/* Init. EMI bus monitor */
	MET_BM_SetReadWriteType(rwtype);

	if ((ttype1_16_en != BM_TTYPE1_16_ENABLE) && (emi_TP_busfiltr_enable != 1)) {
		if (msel_enable) {
			msel_group_val[0] = BM_MASTER_ALL;
			msel_group_val[1] = msel_group1;
			msel_group_val[2] = msel_group2;
			msel_group_val[3] = msel_group3;
		} else {
			msel_group_val[0] = BM_MASTER_ALL;
			msel_group_val[1] = BM_MASTER_ALL;
			msel_group_val[2] = BM_MASTER_ALL;
			msel_group_val[3] = BM_MASTER_ALL;
		}

		MET_BM_SetLatencyCounter(1);    /*enable latency count*/

		for (i = 1; i <= 4; i++) {
			MET_BM_SetMonitorCounter(i,
						 msel_group_val[i - 1] & BM_MASTER_ALL,
						 BM_TRANS_TYPE_4BEAT |
						 BM_TRANS_TYPE_8Byte |
						 BM_TRANS_TYPE_BURST_WRAP);
			MET_BM_SetbusID(i, 0);
			MET_BM_SetbusID_En(i, 0);       /*disable ttype bus sel*/
		}
		for (i = 0; i < 4; i++)
			MET_BM_Set_WsctTsct_id_sel(i, 0);       /*disable tp filter*/

	} else if ((ttype1_16_en != BM_TTYPE1_16_ENABLE) && (emi_TP_busfiltr_enable == 1)) {
		MET_BM_SetLatencyCounter(1);    /*enable latency count*/

		for (i = 1; i <= 4; i++) {
			MET_BM_SetMonitorCounter(i,
						 ttype_master_val[i - 1],
						 ttype_nbeat_val[i - 1] |
						 ttype_nbyte_val[i - 1] |
						 ttype_burst_val[i - 1]);
			MET_BM_SetbusID(i, ttype_busid_val[i - 1]);
			MET_BM_SetbusID_En(i, 0);       /*disable ttype bus sel*/
		}
		for (i = 0; i < 4; i++)
			MET_BM_Set_WsctTsct_id_sel(i, 1);       /*enable tp filter*/

	} else if ((ttype1_16_en == BM_TTYPE1_16_ENABLE) && (emi_TP_busfiltr_enable != 1)) {
		MET_BM_SetLatencyCounter(0);    /*disable latency count*/

		for (i = 1; i <= 16; i++) {
			MET_BM_SetMonitorCounter(i,
						 ttype_master_val[i - 1],
						 ttype_nbeat_val[i - 1] |
						 ttype_nbyte_val[i - 1] |
						 ttype_burst_val[i - 1]);

			MET_BM_SetbusID(i, ttype_busid_val[i - 1]);
			/*disenable ttype bus sel if busid > 0xff_ff*/
			MET_BM_SetbusID_En(i, (ttype_busid_val[i - 1] > 0xffff) ? 0 : 1);
		}
		for (i = 0; i < 4; i++)
			MET_BM_Set_WsctTsct_id_sel(i, 0);       /*disable tp filter*/
	} else {	/* (ttype1_16_en == BM_TTYPE1_16_ENABLE)  &&  (emi_TP_busfiltr_enable == 1) */
		MET_BM_SetLatencyCounter(0);    /*disable latency count*/

		for (i = 1; i <= 16; i++) {
			MET_BM_SetMonitorCounter(i,
						 ttype_master_val[i - 1],
						 ttype_nbeat_val[i - 1] |
						 ttype_nbyte_val[i - 1] |
						 ttype_burst_val[i - 1]);

			MET_BM_SetbusID(i, ttype_busid_val[i - 1]);
			/*disable ttype bus sel if busid > 0xff_ff*/
			MET_BM_SetbusID_En(i, (ttype_busid_val[i - 1] > 0xffff) ? 0 : 1);
		}
		for (i = 0; i < 4; i++)
			MET_BM_Set_WsctTsct_id_sel(i, 1);       /*enable tp filter*/
	}

	if (ttype17_21_en == BM_TTYPE17_21_ENABLE) {
		for (i = 17; i <= 21; i++) {
			MET_BM_SetMonitorCounter(i,
						 ttype_master_val[i - 1],
						 ttype_nbeat_val[i - 1] |
						 ttype_nbyte_val[i - 1] |
						 ttype_burst_val[i - 1]);
			MET_BM_SetbusID(i, ttype_busid_val[i - 1]);
			/*disable ttype bus sel if busid > 0xff_ff*/
			MET_BM_SetbusID_En(i, (ttype_busid_val[i - 1] > 0xffff) ? 0 : 1);
		}
	}

	bmrw0_val = 0;
	for (i = 0; i < 16; i++)
		bmrw0_val |= (ttype_rw_val[i] << (i * 2));

	bmrw1_val = 0;
	for (i = 16; i < 21; i++)
		bmrw1_val |= (ttype_rw_val[i] << ((i-16) * 2));

	MET_BM_SetTtypeCounterRW(bmrw0_val, bmrw1_val);

	for (i = 0; i < BM_COUNTER_MAX; i++) {
		if ((high_priority_filter & (1 << i)) == 0)
			enable = 0;
		else
			enable = 1;

		MET_BM_SetUltraHighFilter(i + 1, enable);
	}

	met_record_dramc_dcm_enable_flag();

	if (met_emi_regdump == 1)
		emi_dump_reg();
}


static void emi_uninit(void)
{
	MET_BM_RestoreCfg();
}

static inline void emi_start(void)
{
	MET_BM_Enable(1);
}


static inline void emi_stop(void)
{
	MET_BM_Enable(0);
}


static inline int do_emi(void)
{
	return met_emi.mode;
}


noinline void DRAM_DVFS(unsigned int dram_data_rate_MHz)
{
	MET_TRACE("%u\n", dram_data_rate_MHz);
}


static unsigned int emi_bw_limiter(unsigned int *__restrict__ array)
{
	int idx = 0;
	unsigned int dram_data_rate_MHz;

	if (get_dram_data_rate_symbol) {
		dram_data_rate_MHz = get_dram_data_rate_symbol();
	} else {
		METERROR("get_dram_data_rate_symbol = NULL\n");
		dram_data_rate_MHz = 0;
	}

	/* print dram data rate */
	DRAM_DVFS(dram_data_rate_MHz);

	/* get correct dram_clock_rate */
	array[idx++] = dram_data_rate_MHz;

	/* get correct ARB A->LAST */
	array[idx++] = MET_EMI_GetARBA();
	array[idx++] = MET_EMI_GetARBB();
	array[idx++] = MET_EMI_GetARBC();
	array[idx++] = MET_EMI_GetARBD();
	array[idx++] = MET_EMI_GetARBE();
	array[idx++] = MET_EMI_GetARBF();
	array[idx++] = MET_EMI_GetARBG();
	array[idx++] = MET_EMI_GetARBH();
	/* EMI Total BW Thresholds */
	array[idx++] = MET_EMI_GetBWCT0();
	array[idx++] = MET_EMI_GetBWCT1();
	array[idx++] = MET_EMI_GetBWCT2();
	array[idx++] = MET_EMI_GetBWCT3();
	array[idx++] = MET_EMI_GetBWCT4();
	array[idx++] = MET_EMI_GetBWST0();
	array[idx++] = MET_EMI_GetBWST1();
	/* EMI C+G BW Thresholds */
	array[idx++] = MET_EMI_GetBWCT0_2ND();
	array[idx++] = MET_EMI_GetBWCT1_2ND();
	array[idx++] = MET_EMI_GetBWST_2ND();

	return idx;
}


static void _ms_dramc(unsigned int *__restrict__ dramc_pdir_value, int dram_chann_num)
{
	MET_DRAMC_GetDebugCounter(dramc_pdir_value, dram_chann_num);
}


static unsigned int emi_polling(unsigned int *__restrict__ emi_value, unsigned int *__restrict__ emi_tsct,
				unsigned int *__restrict__ emi_ttype_value, unsigned int *__restrict__ dramc_pdir_value,
				unsigned int *__restrict__ emi_mdct_value)
{
	int j = 4;              /* skip 4 WSCTs */
	int i = 0;              /* ttype start at 0 */
	int k = 0;              /* tsct start at 0 */
	int n;

	MET_BM_Pause();

	/* Get Word Count */

	emi_value[0] = MET_BM_GetWordCount(1);  /* All */
	emi_value[1] = MET_BM_GetWordCount(2);  /* Group 1 */
	emi_value[2] = MET_BM_GetWordCount(3);  /* Group 2 */
	emi_value[3] = MET_BM_GetWordCount(4);  /* Group 3 */

	if (ttype1_16_en != BM_TTYPE1_16_ENABLE) {      /*1~21 NOT for ttype*/
		/* Get Latency */
		j += MET_BM_GetLatencyCycle(emi_value + j, 1, 8);

		/* Get Trans. */
		j += MET_BM_GetLatencyCycle(emi_value + j, 9, 16);
	} else {
		for (n = 4; n < 20; n++)
			emi_value[n] = 0;
		j = 20;

		i += MET_BM_GetLatencyCycle(emi_ttype_value + i, 1, 8);

		/* Get Trans. */
		i += MET_BM_GetLatencyCycle(emi_ttype_value + i, 9, 16);
	}

	/* Get BACT/BSCT/BCNT/WACT/DCM_CTRL */
	emi_value[j++] = MET_BM_GetBandwidthWordCount(); /* 20 */
	emi_value[j++] = MET_BM_GetOverheadWordCount();
	emi_value[j++] = MET_BM_GetBusCycCount();
	emi_value[j++] = MET_BM_GetWordAllCount();
	emi_value[j++] = MET_DRAMC_DCM_CTRL(0);

	/* Get TACT */
	emi_value[j++] = MET_BM_GetTransAllCount();

	/* Get PageHist/PageMiss/InterBank/Idle */
	for (n = 0; n < dram_chann_num; n++) {
		emi_value[j++] = MET_DRAMC_GetPageHitCount(DRAMC_ALL, n);
		emi_value[j++] = MET_DRAMC_GetPageMissCount(DRAMC_ALL, n);
		emi_value[j++] = MET_DRAMC_GetInterbankCount(DRAMC_ALL, n);
		emi_value[j++] = MET_DRAMC_GetIdleCount(n);
		emi_value[j++] = ((MET_DRAMC_SPCMDRESP(n) >> 8) & 0x7); /* refresh rate */
		emi_value[j++] = MET_DRAMC_RefPop(n);
		emi_value[j++] = MET_DRAMC_Free26M(n);
		emi_value[j++] = MET_DRAMC_RByte(n);
		emi_value[j++] = MET_DRAMC_WByte(n);
	}
	/* TTYPE */
	if (ttype17_21_en == BM_TTYPE17_21_ENABLE)    /*17~21 for ttype*/
		MET_BM_GetLatencyCycle(emi_ttype_value + 16, 17, 21);

	/* Get tsct */
	if (emi_tsct_enable == 1) {
		emi_tsct[k++] = MET_BM_GetTransCount(1);
		emi_tsct[k++] = MET_BM_GetTransCount(2);
		emi_tsct[k++] = MET_BM_GetTransCount(3);
	}
	/*get mdct rsv buffer*/
	if (emi_mdct_enable == 1) {
		emi_mdct_value[0] = (MET_BM_GetMDCT() >> 16) & 0x7;
		emi_mdct_value[1] = (MET_BM_GetMDCT_2() & 0x7);
	}

	if (dramc_pdir_enable == 1 && DRAMC_VER >= 2 )
		_ms_dramc(dramc_pdir_value, dram_chann_num);

	MET_BM_Continue();
	MET_BM_Clear_Start();

	return j;
}


/*======================================================================*/
/*	MET Device Operations						*/
/*======================================================================*/
static int emi_inited;

static int met_emi_create(struct kobject *parent)
{
	int ret = 0;
	int i;

	for (i = 0; i < 21; i++) {
		ttype_master_val[i] = BM_MASTER_M0;
		ttype_nbeat_val[i] = BM_TRANS_TYPE_1BEAT;
		ttype_nbyte_val[i] = BM_TRANS_TYPE_8Byte;
		ttype_burst_val[i] = BM_TRANS_TYPE_BURST_INCR;
		ttype_busid_val[i] = 0xfffff;   /*default disable ttype bus sel if busid > 0xff_ff */
		ttype_rw_val[i] =  BM_TRANS_RW_DEFAULT;
	}

	ret = MET_BM_Init();
	if (ret != 0) {
		pr_notice("MET_BM_Init failed!!!\n");
		ret = 0;        /* will retry later */
	} else {
		emi_inited = 1;
	}

	kobj_emi = parent;

#define KOBJ_ATTR_ITEM(attr_name) \
	do { \
		ret = sysfs_create_file(kobj_emi, &attr_name ## _attr.attr); \
		if (ret != 0) { \
			pr_notice("Failed to create " #attr_name " in sysfs\n"); \
			return ret; \
		} \
	} while (0)
	KOBJ_ATTR_LIST;
#undef  KOBJ_ATTR_ITEM

	return ret;
}


static void met_emi_delete(void)
{
#define KOBJ_ATTR_ITEM(attr_name) \
	sysfs_remove_file(kobj_emi, &attr_name ## _attr.attr)
	if (kobj_emi != NULL) {
		KOBJ_ATTR_LIST;
		kobj_emi = NULL;
	}
#undef  KOBJ_ATTR_ITEM

	if (emi_inited)
		MET_BM_DeInit();
}


static void met_emi_start(void)
{
	unsigned int bw_limiter[NIDX_BL] = {0};

	if (!emi_inited) {
		if (MET_BM_Init() != 0) {
			met_emi.mode = 0;
			pr_notice("MET_BM_Init failed!!!\n");
			return;
		}
		emi_inited = 1;
	}

	if (do_emi()) {
		emi_init();
		MET_BM_Clear_Start();

		/* Draw the first BW Limiter point */
		if (bw_limiter_enable == BM_BW_LIMITER_ENABLE) {
			emi_bw_limiter(bw_limiter);
			ms_bw_limiter(NIDX_BL, bw_limiter);
			/* init countdown value */
			countdown = CNT_COUNTDOWN;
		}
	}
}


static void met_emi_stop(void)
{
	unsigned int bw_limiter[NIDX_BL] = {0};

	if (!emi_inited)
		return;

	if (met_emi_regdump == 1)
		emi_dump_reg();

	if (do_emi()) {
		/* Draw the last BW Limiter point */
		if (bw_limiter_enable == BM_BW_LIMITER_ENABLE) {
			/*
			 * Skip drawing when we just draw
			 * the point at last polling.
			 */
			if (countdown < CNT_COUNTDOWN) {
				emi_bw_limiter(bw_limiter);
				ms_bw_limiter(NIDX_BL, bw_limiter);
			}
		}

		emi_stop();
		emi_uninit();
	}
}


/* DRS output */
noinline void emi_drs(uint32_t *value)
{
	MET_TRACE("%u,%u,%u,%u,%u,%u\n", value[0], value[1], value[2], value[3], value[4], value[5]);
}


static void met_emi_polling(unsigned long long stamp, int cpu)
{
	unsigned int emi_value[NIDX];
	unsigned int emi_tsct[3] = {0};
	unsigned int emi_ttype_value[21] = {0};
	unsigned int dramc_pdir_value[DRAMC_Debug_MAX_CNT * NCH] = {0};
	unsigned int emi_mdct_value[2];
	unsigned int chn_emi_drs[6];

	if (!do_emi())
		return;

	/* get emi & dramc counters */
	emi_value[0] = 0;       /* 0: pure linux MET , 0xa5: OnDieMET*/
	emi_value[1] = 0;       /* EBM pause duration (ns)*/
	emi_polling(emi_value + 2, emi_tsct, emi_ttype_value, dramc_pdir_value, emi_mdct_value);

	/* get and output BW Limiter */
	if (bw_limiter_enable == BM_BW_LIMITER_ENABLE) {
		unsigned int bw_limiter[NIDX_BL] = {0};

		if (countdown > 0) {
			countdown--;
		} else {
			emi_bw_limiter(bw_limiter);
			ms_bw_limiter(NIDX_BL, bw_limiter);
			/* reload countdown value */
			countdown = CNT_COUNTDOWN;
		}
	}

	/* output emi */
	ms_emi(NIDX_EMI - NTTYPE + (NCNT * dram_chann_num), emi_value);

	/* output tsct*/
	if (emi_tsct_enable == 1)
		ms_emi_tsct(3, emi_tsct);

	/* output mdct*/
	if (emi_mdct_enable == 1)
		ms_emi_mdct(2, emi_mdct_value);

	/* output dramc*/
	if (dramc_pdir_enable == 1 && DRAMC_VER >= 2 )
		ms_dramc(DRAMC_Debug_MAX_CNT * dram_chann_num, dramc_pdir_value);

	/* output ms_ttype */
	if (ttype17_21_en == BM_TTYPE17_21_ENABLE)
		ms_ttype(21, emi_ttype_value);

	/* adjust MDMCU buffer */
	if (mdmcu_sel_enable == 1)
		MET_BM_SetMDCT_MDMCU(rd_mdmcu_rsv_num);

	/* DRS handling */
	met_get_drs_registers(chn_emi_drs);
	emi_drs(chn_emi_drs);
}


static void met_emi_resume(void)
{
	/* return directly when emi was closed */
	if (!do_emi())
		return;

	/* remap EMI_BM related reg*/
	emi_init();

	/* restarn counting */
	MET_BM_Clear_Start();
}


static const char help[] = "  --emi                                 monitor EMI banwidth\n";
static int emi_print_help(char *buf, int len)
{
	return snprintf(buf, PAGE_SIZE, help);
}


#define TTYPE_NAME_STR_LEN  64
/* static char ttype_name[21][TTYPE_NAME_STR_LEN]; */
static int emi_print_header(char *buf, int len)
{
	int ret = 0;
/*	int ret_m[21]; */
	int i = 0;

#if 1 /* move to AP side print header */
//#ifndef CONFIG_MTK_TINYSYS_SSPM_SUPPORT
	unsigned int dram_data_rate_MHz;
	unsigned int DRAM_TYPE;
#endif

#if 0
	/*ttype header info*/
	for (i = 0; i < 21; i++) {
		int k;

		/*busid > 0xffff    not specific bus id , show all on specificmaster*/
		if (ttype_busid_val[i] > 0xffff) {
			int j;

			for (j = 0; j < ARRAY_SIZE(ttype_master_list_item); j++) {
				if (ttype_master_val[i] == ttype_master_list_item[j].key) {
					ret_m[i] = snprintf(ttype_name[i], TTYPE_NAME_STR_LEN, "ttype%d_%s",
							    i + 1, ttype_master_list_item[j].val);/*master*/
					break;
				}
			}
			if (j == ARRAY_SIZE(ttype_master_list_item))
				ret_m[i] = snprintf(ttype_name[i], TTYPE_NAME_STR_LEN, "ttype%d_%s",
						    i + 1, "unknown");
		} else {
			ret_m[i] = snprintf(ttype_name[i], TTYPE_NAME_STR_LEN, "ttype%d_%x",
					    i + 1, ttype_busid_val[i]);/*busID*/
		}

		/*show beat type*/
		for (k = 0; k < ARRAY_SIZE(ttype_nbeat_list_item); k++) {

			if (ttype_nbeat_val[i] == ttype_nbeat_list_item[k].key)
				ret_m[i] += snprintf(ttype_name[i] + ret_m[i], TTYPE_NAME_STR_LEN - ret_m[i], "_%d",
						     ttype_nbeat_list_item[k].val); /*beat*/
		}

		/*show byte type*/
		for (k = 0; k < ARRAY_SIZE(ttype_nbyte_list_item); k++) {

			if (ttype_nbyte_val[i] == ttype_nbyte_list_item[k].key)
				ret_m[i] += snprintf(ttype_name[i] + ret_m[i], TTYPE_NAME_STR_LEN - ret_m[i], "x%d",
						     ttype_nbyte_list_item[k].val); /*byte*/
		}

		/*show burst type*/
		for (k = 0; k < ARRAY_SIZE(ttype_burst_list_item); k++) {

			if (ttype_burst_val[i] == ttype_burst_list_item[k].key)
				ret_m[i] += snprintf(ttype_name[i] + ret_m[i], TTYPE_NAME_STR_LEN - ret_m[i], "_%s",
						     ttype_burst_list_item[k].val); /*burst*/
		}

		/*show rw type*/
		for (k = 0; k < ARRAY_SIZE(ttype_rw_list_item); k++) {

			if (ttype_rw_val[i] == ttype_rw_list_item[k].key)
				ret_m[i] += snprintf(ttype_name[i] + ret_m[i], TTYPE_NAME_STR_LEN - ret_m[i], "_%s",
						     ttype_rw_list_item[k].val); /*rw*/
		}
	}
#endif

	if ((ttype1_16_en != BM_TTYPE1_16_ENABLE) && (emi_TP_busfiltr_enable != 1)) {
		/* master selection header */
		if (msel_enable) {
			ret += snprintf(buf + ret, PAGE_SIZE - ret,
					"met-info [000] 0.0: met_emi_msel: %x,%x,%x\n",
					msel_group1 & BM_MASTER_ALL,
					msel_group2 & BM_MASTER_ALL,
					msel_group3 & BM_MASTER_ALL);
		} else {
			ret += snprintf(buf + ret, PAGE_SIZE - ret,
					"met-info [000] 0.0: met_emi_msel: %x,%x,%x\n",
					BM_MASTER_ALL & BM_MASTER_ALL,
					BM_MASTER_ALL & BM_MASTER_ALL,
					BM_MASTER_ALL & BM_MASTER_ALL);
		}
	} else {
		/*ttype master if BM_TTYPE1_16_ENABLE or emi_TP_busfiltr_enable*/
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"met-info [000] 0.0: met_emi_ttype_master: %x,%x,%x,%x\n",
				ttype_master_val[0], ttype_master_val[1], ttype_master_val[2], ttype_master_val[3]);

		if (emi_TP_busfiltr_enable == 1) {
			/* busID if emi_TP_busfiltr_enable*/
			ret += snprintf(buf + ret, PAGE_SIZE - ret,
					"met-info [000] 0.0: met_emi_ttype_busid: %x,%x,%x,%x\n",
					ttype_busid_val[0], ttype_busid_val[1], ttype_busid_val[2], ttype_busid_val[3]);
		}
	}

	/*RW type header*/
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_emi_rw_cfg: ");
	if (rwtype == BM_READ_ONLY)
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "R");
	else if (rwtype == BM_WRITE_ONLY)
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "W");
	else
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "BOTH");

	for (i = 0; i < 21; i++) {
		if (ttype_rw_val[i] == BM_TRANS_RW_DEFAULT)
			ret += snprintf(buf + ret, PAGE_SIZE - ret, ",DEFAULT");
		else if (ttype_rw_val[i] == BM_TRANS_RW_READONLY)
			ret += snprintf(buf + ret, PAGE_SIZE - ret, ",R");
		else if (ttype_rw_val[i] == BM_TRANS_RW_WRITEONLY)
			ret += snprintf(buf + ret, PAGE_SIZE - ret, ",W");
		else    /*BM_TRANS_RW_RWBOTH*/
			ret += snprintf(buf + ret, PAGE_SIZE - ret, ",BOTH");
	}
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "\n");

	/*ultra header*/
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"met-info [000] 0.0: met_emi_ultra_filter: %x\n", high_priority_filter);

#if 0
	/*ttype header (user define format)*/
	if ((ttype1_16_en == BM_TTYPE1_16_ENABLE) && (ttype17_21_en == BM_TTYPE17_21_ENABLE)) {
		/*header = ttype1~21t*/
		int i;

		ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: ms_ud_sys_header: ms_ttype,");
		for (i = 0; i < 21; i++)
			ret += snprintf(buf + ret, PAGE_SIZE - ret, "%s,", ttype_name[i]);

		ret += snprintf(buf + ret, PAGE_SIZE - ret, "x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x\n");

	} else if (ttype17_21_en == BM_TTYPE17_21_ENABLE) {
		/*header = ttype17~21t*/
		int i;

		ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: ms_ud_sys_header: ms_ttype,");

		for (i = 16; i < 21; i++)
			ret += snprintf(buf + ret, PAGE_SIZE - ret, "%s,", ttype_name[i]);

		ret += snprintf(buf + ret, PAGE_SIZE - ret, "x,x,x,x,x\n");
	}
#else
#if 1
	/* ttype header */
	if (ttype17_21_en == BM_TTYPE17_21_ENABLE) {
		int i = 0;
		int j = 0;

		/* ttype master list */
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_emi_ttype_master_list: ");
		for (i = 0; i < 21; i++) {
			for (j = 0; j < ARRAY_SIZE(ttype_master_list_item); j++) {
				if (ttype_master_val[i] == ttype_master_list_item[j].key) {
					ret += snprintf(buf + ret, PAGE_SIZE - ret, "%s,", ttype_master_list_item[j].val);
				}
			}
		}
		/* remove the last comma */
		snprintf(buf + ret -1, PAGE_SIZE - ret + 1, "\n");

		/* ttype busid list */
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_emi_ttype_busid_list: ");
		for (i = 0; i < 21; i++)
			ret += snprintf(buf + ret, PAGE_SIZE - ret, "%x,", ttype_busid_val[i]);

		snprintf(buf + ret -1, PAGE_SIZE - ret + 1, "\n");

		/* ttype nbeat list */
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_emi_ttype_nbeat_list: ");
		for (i = 0; i < 21; i++) {
			for (j = 0; j < ARRAY_SIZE(ttype_nbeat_list_item); j++) {
				if (ttype_nbeat_val[i] == ttype_nbeat_list_item[j].key) {
					ret += snprintf(buf + ret, PAGE_SIZE - ret, "%d,", ttype_nbeat_list_item[j].val);
				}
			}
		}
		snprintf(buf + ret -1, PAGE_SIZE - ret + 1, "\n");

		/* ttype nbyte list */
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_emi_ttype_nbyte_list: ");
		for (i = 0; i < 21; i++) {
			for (j = 0; j < ARRAY_SIZE(ttype_nbyte_list_item); j++) {
				if (ttype_nbyte_val[i] == ttype_nbyte_list_item[j].key) {
					ret += snprintf(buf + ret, PAGE_SIZE - ret, "%d,", ttype_nbyte_list_item[j].val);
				}
			}
		}
		snprintf(buf + ret -1, PAGE_SIZE - ret + 1, "\n");

		/* ttype burst list */
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_emi_ttype_burst_list: ");
		for (i = 0; i < 21; i++) {
			for (j = 0; j < ARRAY_SIZE(ttype_burst_list_item); j++) {
				if (ttype_burst_val[i] == ttype_burst_list_item[j].key) {
					ret += snprintf(buf + ret, PAGE_SIZE - ret, "%s,", ttype_burst_list_item[j].val);
				}
			}
		}
		snprintf(buf + ret -1, PAGE_SIZE - ret + 1, "\n");

		/* ttype enable */
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_emi_ttype_enable: %d,%d\n",ttype1_16_en, ttype17_21_en);


	}
#endif
#endif
	/*IP version*/
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"met-info [000] 0.0: DRAMC_VER: %d\n", DRAMC_VER);

	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"met-info [000] 0.0: EMI_VER: %d.%d\n", EMI_VER_MAJOR, EMI_VER_MINOR);

#if 1 /* move to AP side print header */
//#ifndef CONFIG_MTK_TINYSYS_SSPM_SUPPORT
	dram_chann_num = MET_EMI_GetDramChannNum();
	/*	met_dram_chann_num_header
	 *	channel number
	 *	LP4: 2, LP3: 1
	 */

	/*
	 *	the ddr type define :
	 *	enum DDRTYPE {
	 *	TYPE_LPDDR3 = 1,
	 *	TYPE_LPDDR4,
	 *	TYPE_LPDDR4X,
	 *	TYPE_LPDDR2
	 *	};
	 */
	if (get_ddr_type_symbol) {
		DRAM_TYPE = get_ddr_type_symbol();
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_dram_type: %d\n", DRAM_TYPE);

		if ((DRAM_TYPE == 2) || (DRAM_TYPE == 3))
			ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_dram_chann_num_header: %d,%d,%d,%d\n",
					dram_chann_num, DRAM_EMI_BASECLOCK_RATE_LP4,
					DRAM_IO_BUS_WIDTH_LP4, DRAM_DATARATE);
		else
			ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: met_dram_chann_num_header: %d,%d,%d,%d\n",
					dram_chann_num, DRAM_EMI_BASECLOCK_RATE_LP3,
					DRAM_IO_BUS_WIDTH_LP3, DRAM_DATARATE);
	} else
		METERROR("[%s][%d]get_ddr_type_symbol = NULL , use the TYPE_LPDDR3 setting\n", __func__, __LINE__);

	/* metemi_func_opt for middleware */
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "met-info [000] 0.0: metemi_func_opt_header: %d\n",
			metemi_func_opt);

	/* met_emi_clockrate */
	if (get_dram_data_rate_symbol) {
		dram_data_rate_MHz = get_dram_data_rate_symbol();
	} else {
		METERROR("get_dram_data_rate_symbol = NULL\n");
		dram_data_rate_MHz = 0;
	}

	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"met-info [000] 0.0: met_dram_clockrate: %d\n",
			dram_data_rate_MHz);

	/* 1 : by ondiemet, 0: by pure linux */
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"met-info [000] 0.0: emi_use_ondiemet: %u\n",
			emi_use_ondiemet);

	/*dram bank num*/
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"met-info [000] 0.0: met_dram_rank_num_header: %u,%u\n", MET_EMI_GetDramRankNum(),
				MET_EMI_GetDramRankNum());

	/* ms_emi header */
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"# ms_emi: TS0,TS1,GP0_WSCT,GP1_WSCT,GP2_WSCT,GP3_WSCT,");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"M0_LATENCY,M1_LATENCY,M2_LATENCY,M3_LATENCY,M4_LATENCY,M5_LATENCY,M6_LATENCY,M7_LATENCY,");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"M0_TRANS,M1_TRANS,M2_TRANS,M3_TRANS,M4_TRANS,M5_TRANS,M6_TRANS,M7_TRANS,");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"BACT,BSCT,BCNT,WACT,DCM_CTRL,TACT,");

	for (i = 0; i < dram_chann_num; i++) {
		if (i != 0)
			ret += snprintf(buf + ret, PAGE_SIZE - ret,
					",");
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"PageHit_%d,PageMiss_%d,InterBank_%d,Idle_%d,", i, i, i, i);
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"mr4_%d,refresh_pop_%d,freerun_26m_%d,", i, i, i);
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"read_bytes_%d,write_bytes_%d", i, i);
	}
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "\n");

	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"met-info [000] 0.0: met_emi_header: TS0,TS1,GP0_WSCT,GP1_WSCT,GP2_WSCT,GP3_WSCT,");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"M0_LATENCY,M1_LATENCY,M2_LATENCY,M3_LATENCY,M4_LATENCY,M5_LATENCY,M6_LATENCY,M7_LATENCY,");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"M0_TRANS,M1_TRANS,M2_TRANS,M3_TRANS,M4_TRANS,M5_TRANS,M6_TRANS,M7_TRANS,");
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"BACT,BSCT,BCNT,WACT,DCM_CTRL,TACT,");

	for (i = 0; i < dram_chann_num; i++) {
		if (i != 0)
			ret += snprintf(buf + ret, PAGE_SIZE - ret,
					",");
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"PageHit_%d,PageMiss_%d,InterBank_%d,Idle_%d,", i, i, i, i);
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"mr4_%d,refresh_pop_%d,freerun_26m_%d,", i, i, i);
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"read_bytes_%d,write_bytes_%d", i, i);
	}
	ret += snprintf(buf + ret, PAGE_SIZE - ret, "\n");

	/*TSCT header*/
	if (emi_tsct_enable == 1) {
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"met-info [000] 0.0: ms_emi_tsct_header: ms_emi_tsct,");
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"tsct1,tsct2,tsct3\n");
	}

	/*MDCT header*/
	if (emi_mdct_enable == 1) {
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"met-info [000] 0.0: ms_emi_mdct_header: ms_emi_mdct,");
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"RD_ULTRA,RD_MDMCU\n");
	}

	/* met_bw_limiter_header */
	if (bw_limiter_enable == BM_BW_LIMITER_ENABLE) {
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"met-info [000] 0.0: met_bw_limiter_header: CLK,");
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"ARBA,ARBB,ARBC,ARBD,ARBE,ARBF,ARBG,ARBH,BWCT0,BWCT1,BWCT2,BWCT3,BWCT4,BWST0,BWST1,BWCT0_2ND,BWCT1_2ND,BWST_2ND\n");
	}

	/* DRAM DVFS header */
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"met-info [000] 0.0: DRAM_DVFS_header: datarate(MHz)\n");

	/*PDIR met_dramc_header*/
	if (dramc_pdir_enable == 1 && DRAMC_VER >= 2 ) {
		ret += snprintf(buf + ret, PAGE_SIZE - ret,
				"met-info [000] 0.0: met_dramc_header: ");
		for (i = 0; i < dram_chann_num; i++) {
			if (i != 0)
				ret += snprintf(buf + ret, PAGE_SIZE - ret,
						",");
			ret += snprintf(buf + ret, PAGE_SIZE - ret, "freerun_26m_%d,", i);
			ret += snprintf(buf + ret, PAGE_SIZE - ret,
					"rk0_pre_sb_%d,rk0_pre_pd_%d,rk0_act_sb_%d,rk0_act_pd_%d,", i, i, i, i);
			ret += snprintf(buf + ret, PAGE_SIZE - ret,
					"rk1_pre_sb_%d,rk1_pre_pd_%d,rk1_act_sb_%d,rk1_act_pd_%d,", i, i, i, i);
			ret += snprintf(buf + ret, PAGE_SIZE - ret,
					"rk2_pre_sb_%d,rk2_pre_pd_%d,rk2_act_sb_%d,rk2_act_pd_%d", i, i, i, i);
		}
		ret += snprintf(buf + ret, PAGE_SIZE - ret, "\n");
	}

	/* DRS header */
	ret += snprintf(buf + ret, PAGE_SIZE - ret,
			"met-info [000] 0.0: emi_drs_header: ch0_RANK1_GP(%%),ch0_RANK1_SF(%%),ch0_ALL_SF(%%),ch1_RANK1_GP(%%),ch1_RANK1_SF(%%),ch1_ALL_SF(%%)\n");
#endif

	return ret;
}


#if defined(CONFIG_MTK_TINYSYS_SSPM_SUPPORT) && defined(ONDIEMET_SUPPORT)
static int ondiemet_emi_print_header(char *buf, int len)
{
	emi_use_ondiemet = 1;
	/* return emi_print_ondiemet_header(buf, len); */
	return emi_print_header(buf, len);
}


static void MET_BM_IPI_REGISTER_CB(void)
{
	int ret, i;
	unsigned int rdata;
	unsigned int ipi_buf[4];

	for (i = 0; i < 4; i++)
		ipi_buf[i] = 0;

	if (sspm_buf_available == 1) {
		ipi_buf[0] = MET_MAIN_ID | (MID_EMI << MID_BIT_SHIFT) | MET_ARGU | SET_REGISTER_CB;
		ret = sspm_ipi_send_sync(IPI_ID_MET, IPI_OPT_WAIT, (void *)ipi_buf, 0, &rdata, 1);
	}
}


static void MET_BM_IPI_configs(void)
{
	int ret, i;
	unsigned int rdata;
	unsigned int ipi_buf[4];

	for (i = 0; i < 4; i++)
		ipi_buf[i] = 0;

	if (sspm_buf_available == 1) {
		ipi_buf[0] = MET_MAIN_ID | (MID_EMI << MID_BIT_SHIFT) | MET_ARGU | SET_EBM_CONFIGS1;
		ipi_buf[1] = INTERNAL_MODE << 16;
		ipi_buf[2] = EMI_VER_MAJOR << 24 | EMI_VER_MINOR << 16 | DRAMC_VER << 8 | 0;
		ret = sspm_ipi_send_sync(IPI_ID_MET, IPI_OPT_WAIT, (void *)ipi_buf, 0, &rdata, 1);
	}
}


static void ondiemet_emi_start(void)
{
	MET_BM_IPI_REGISTER_CB();
	if (!emi_inited) {
		if (MET_BM_Init() != 0) {
			met_emi.mode = 0;
			pr_notice("MET_BM_Init failed!!!\n");
			return;
		}
		emi_inited = 1;
	}
	MET_BM_IPI_configs();

	if (do_emi())
		emi_init();

	ondiemet_module[ONDIEMET_SSPM] |= ID_EMI;
}


static void ondiemet_emi_stop(void)
{
	if (!emi_inited)
		return;

	if (do_emi())
		emi_uninit();
}
#endif


struct metdevice met_emi = {
	.name			= "emi",
	.owner			= THIS_MODULE,
	.type			= MET_TYPE_BUS,
	.create_subfs		= met_emi_create,
	.delete_subfs		= met_emi_delete,
	.cpu_related		= 0,
	.start			= met_emi_start,
	.stop			= met_emi_stop,
	.resume			= met_emi_resume,
	.timed_polling		= met_emi_polling,
	.print_help		= emi_print_help,
	.print_header		= emi_print_header,
#if defined(CONFIG_MTK_TINYSYS_SSPM_SUPPORT) && defined(ONDIEMET_SUPPORT)
	.ondiemet_mode		= 1,
	.ondiemet_start		= ondiemet_emi_start,
	.ondiemet_stop		= ondiemet_emi_stop,
	.ondiemet_print_help	= emi_print_help,
	.ondiemet_print_header	= ondiemet_emi_print_header,
#else
	.ondiemet_mode		= 0,
#endif
};
