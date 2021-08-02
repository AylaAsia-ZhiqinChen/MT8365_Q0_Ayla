/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2015. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

/*=============================================================
 * Include files
 *=============================================================*/

#include "typedefs.h"
#include "sec_devinfo.h"
#include "mtk_ptp.h"
#include "pll.h"
#include "platform.h"

/* extern U32 upmu_get_reg_value(kal_uint32 reg); */
static void mt_ptp_reg_dump(void);
static int ptp_gpufreq_get_max_freq(void);
static int set_vcore_to_pmic(enum ptp_det_id det_id, unsigned int target_volt);

/*=============================================================
 * Macro definition
 *=============================================================*/

#define __stringify_1(x...)	#x
#define __stringify(x...)	__stringify_1(x)

#define ARRAY_SIZE(x) (sizeof((x))/(sizeof(x[0])))

#define PTP_LOG_EN		0
#define CONFIG_PTP_SHOWLOG	0
#define EN_ISR_LOG		0
#define NR_FREQ			8
#define TEST_FAKE_EFUSE		0

/*
 * 100 us, This is the PTP Detector sampling time as represented in
 * cycles of bclk_ck during INIT. 52 MHz
 */
#define DETWINDOW_VAL		0x514

/*
 * mili Volt to config value. voltage = 700mV + val * 6.25mV
 * val = (voltage - 700) / 6.25
 * @mV:	mili volt
 */
#define PTP_VOLT_TO_PMIC_VAL(volt)  ((((volt) / 10) - 70000 + 625 - 1) / 625) /* unit: pmic step */
#define PTP_PMIC_VAL_TO_VOLT(pmic)  ((((pmic) * 625) + 70000) * 10) /* unit: micro voltage */

/**
 * We need to add 0x10 * 6.25mV = 100mV after getting PTPOD voltage for matching PMIC equation.
 * pmic_step = ptp_step + 0x10
 * PTPOD equation is [(ptp_step * 6.25) + 800]
 * PMIC equation [(pmic_step * 6.25) + 700]
 */
/* offset 0x10(16 steps) for CPU/GPU DVFS */
#define PTPOD_PMIC_OFFSET (0x10)

#define DTHI_VAL		0x01		/* positive */
#define DTLO_VAL		0xfe		/* negative (2's compliment) */
#define DETMAX_VAL		0xffff		/* This timeout value is in cycles of bclk_ck. */
#define AGECONFIG_VAL		0x555555	/* FIXME */
#define AGEM_VAL		0x0		/* FIXME */
#define DVTFIXED_VAL		0x6		/* FIXME */
#define VCO_VAL			0x10		/* FIXME */
#define DCCONFIG_VAL		0x555555	/* FIXME */

/*
 * bit operation
 */
#undef  BIT
#define BIT(bit)	(1U << (bit))

#define MSB(range)	(1 ? range)
#define LSB(range)	(0 ? range)
/**
 * Genearte a mask wher MSB to LSB are all 0b1
 * @r:	Range in the form of MSB:LSB
 */
#define BITMASK(r)	(((unsigned) -1 >> (31 - MSB(r))) & ~((1U << LSB(r)) - 1))

/**
 * Set value at MSB:LSB. For example, BITS(7:3, 0x5A)
 * will return a value where bit 3 to bit 7 is 0x5A
 * @r:	Range in the form of MSB:LSB
 */
/* BITS(MSB:LSB, value) => Set value at MSB:LSB  */
#define BITS(r, val)	((val << LSB(r)) & BITMASK(r))

/**
 * iterate over list of detectors
 * @det:	the detector * to use as a loop cursor.
 */
#define for_each_det(det) for (det = &ptp_detectors[PTP_DET_GPUSYS]; det < (ptp_detectors + ARRAY_SIZE(ptp_detectors)); det++)
/**
 * Given a ptp_det * in ptp_detectors. Return the id.
 * @det:	pointer to a ptp_det in ptp_detectors
 */
#define det_to_id(det)	((det) - &ptp_detectors[0])

#define PERCENT(numerator, denominator) \
	(unsigned char)(((numerator) * 100 + (denominator) - 1) / (denominator))

/*
 * LOG
 */
#define ptp_emerg(fmt, args...)     print("[PTP] " fmt, ##args)
#define ptp_alert(fmt, args...)     print("[PTP] " fmt, ##args)
#define ptp_crit(fmt, args...)      print("[PTP] " fmt, ##args)
#define ptp_error(fmt, args...)     print("[PTP] " fmt, ##args)
#define ptp_warning(fmt, args...)   print("[PTP] " fmt, ##args)
#define ptp_notice(fmt, args...)    print("[PTP] " fmt, ##args)
#define ptp_info(fmt, args...)      print("[PTP] " fmt, ##args)
#define ptp_debug(fmt, args...)     print("[PTP] " fmt, ##args)

#if EN_ISR_LOG
#define ptp_isr_info(fmt, args...)  ptp_notice(fmt, ##args)
#else
#define ptp_isr_info(fmt, args...)  ptp_debug(fmt, ##args)
#endif

#define FUNC_LV_MODULE          BIT(0)  /* module, platform driver interface */
#define FUNC_LV_CPUFREQ         BIT(1)  /* cpufreq driver interface          */
#define FUNC_LV_API             BIT(2)  /* mt_cpufreq driver global function */
#define FUNC_LV_LOCAL           BIT(3)  /* mt_cpufreq driver lcaol function  */
#define FUNC_LV_HELP            BIT(4)  /* mt_cpufreq driver help function   */

static unsigned int func_lv_mask = (FUNC_LV_MODULE | FUNC_LV_CPUFREQ | FUNC_LV_API | FUNC_LV_LOCAL | FUNC_LV_HELP);

#if CONFIG_PTP_SHOWLOG
#define FUNC_ENTER(lv)          do { if ((lv) & func_lv_mask) ptp_debug(">> %s()\n", __func__); } while (0)
#define FUNC_EXIT(lv)           do { if ((lv) & func_lv_mask) ptp_debug("<< %s():%d\n", __func__, __LINE__); } while (0)
#else
#define FUNC_ENTER(lv)
#define FUNC_EXIT(lv)
#endif /* CONFIG_CPU_DVFS_SHOWLOG */

/*
 * REG ACCESS
 */
#define ptp_read(addr)				(*(volatile unsigned int *)(addr))
#define ptp_read_field(addr, range)		((ptp_read(addr) & BITMASK(range)) >> LSB(range))
#define ptp_write(addr, val)			do { (*(volatile unsigned int *) (addr) = (unsigned int) (val)); } while (0)
#define ptp_write_field(addr, range, val)	ptp_write(addr, (ptp_read(addr) & ~BITMASK(range)) | BITS(range, val))


/*=============================================================
 * Local type definition
 *=============================================================*/

typedef enum {
	PTP_PHASE_INIT01,
	PTP_PHASE_INIT02,
	PTP_PHASE_MON,

	NR_PTP_PHASE,
} ptp_phase;

enum ptp_features {
	FEA_INIT01	= BIT(PTP_PHASE_INIT01),
	FEA_INIT02	= BIT(PTP_PHASE_INIT02),
	FEA_MON		= BIT(PTP_PHASE_MON),
};

struct ptp_devinfo {
	/* PTPOD0 */
	unsigned int CPU_BDES		: 8;
	unsigned int CPU_MDES		: 8;
	unsigned int CPU_DCBDET		: 8;
	unsigned int CPU_DCMDET		: 8;

	/* PTPOD1 */
	unsigned int CPU_SPEC		: 3;
	unsigned int CPU_Turbo		: 1;
	unsigned int CPU_DVFS_LOW	: 2;
	unsigned int PTPINITEN		: 1; /* CPU_INIT */
	unsigned int PTPMONEN		: 1; /* CPU_MON */
	unsigned int CPU_LEAKAGE	: 8;
	unsigned int CPU_MTDES		: 8;
	unsigned int CPU_AGEDELTA	: 8;

	/* PTPOD2 */
	unsigned int GPU_BDES		: 8;
	unsigned int GPU_MDES		: 8;
	unsigned int GPU_DCBDET		: 8;
	unsigned int GPU_DCMDET		: 8;

	/* PTPOD3 */
	unsigned int GPU_SPEC		: 3;
	unsigned int GPU_Turbo		: 1;
	unsigned int GPU_DVFS_LOW	: 2;
	unsigned int GPU_INIT		: 1;
	unsigned int GPU_MON		: 1;
	unsigned int GPU_LEAKAGE	: 8;
	unsigned int GPU_MTDES		: 8;
	unsigned int GPU_AGEDELTA	: 8;

	/* PTPOD4 */
	unsigned int Lot_ID6		: 2;
	unsigned int Lot_ID5		: 6;
	unsigned int Lot_ID4		: 6;
	unsigned int Lot_ID3		: 6;
	unsigned int Lot_ID2		: 6;
	unsigned int Lot_ID1		: 6;

	/* PTPOD5 */
	unsigned int PTPOD5_Y		: 8;
	unsigned int PTPOD5_X		: 8;
	unsigned int PTPOD5_Random	: 1;
	unsigned int PTPOD5_Wfr_ID	: 4;
	unsigned int PTPOD5_Lot_ID7	: 7;
	unsigned int PTPOD5_Lot_ID6	: 4;

	/* PTPOD6 */
	unsigned int PTPOD6_Y		: 8;
	unsigned int PTPOD6_X		: 8;
	unsigned int PTPOD6_Random	: 1;
	unsigned int PTPOD6_Wfr_ID	: 4;
	unsigned int PTPOD6_Lot_ID7	: 7;
	unsigned int PTPOD6_Lot_ID6	: 4;
};

struct ptp_det {
	const char *name;
	struct ptp_det_ops *ops;
	int status;		/* TODO: enable/disable */
	int features;		/* enum ptp_features */
	enum ptp_ctrl_id ctrl_id;

	/* devinfo */
	unsigned int PTPINITEN;
	unsigned int PTPMONEN;
	unsigned int MDES;
	unsigned int BDES;
	unsigned int DCMDET;
	unsigned int DCBDET;
	unsigned int AGEDELTA;
	unsigned int MTDES;

	/* constant */
	unsigned int DETWINDOW;
	unsigned int VMAX;
	unsigned int VMIN;
	unsigned int DTHI;
	unsigned int DTLO;
	unsigned int VBOOT;
	unsigned int DETMAX;
	unsigned int AGECONFIG;
	unsigned int AGEM;
	unsigned int DVTFIXED;
	unsigned int VCO;
	unsigned int DCCONFIG;

	/* Generated by PTP init01. Used in PTP init02 */
	unsigned int DCVOFFSETIN;
	unsigned int AGEVOFFSETIN;

	/* slope */
	unsigned int MTS;
	unsigned int BTS;

	/* dvfs */
	unsigned int num_freq_tbl; /* could be got @ the same time with freq_tbl[] */
	unsigned int freq_base; /* base frequency used to calculate percentage */
	unsigned int freq_max; /* DVFS max freq */
	unsigned char freq_tbl[NR_FREQ]; /* percentage to maximum freq */

	unsigned int volt_tbl[NR_FREQ];
	unsigned int volt_tbl_init2[NR_FREQ];
	unsigned int volt_tbl_pmic[NR_FREQ];
	int volt_offset;

	int disabled; /* Disabled by error or sysfs */
};

struct ptp_det_ops {
	void (*switch_bank)(struct ptp_det *det);
};

static void turn_on_mfg_power_domain(void)
{
	/* Enable MFG-top clock */
	//ptp_write_field(CLR_CLK_GATING_CTRL8, 7:6, 0x3);

	/* Enable MFG-related power-domain */
	//spm_mtcmos_ctrl_mfg_async(STA_POWER_ON);
	//spm_mtcmos_ctrl_mfg_2d(STA_POWER_ON);
	//spm_mtcmos_ctrl_mfg(STA_POWER_ON);
}

static void turn_off_mfg_power_domain(void)
{
	/* Disable MFG-related power-domain */
	//spm_mtcmos_ctrl_mfg_async(STA_POWER_DOWN);
	//spm_mtcmos_ctrl_mfg_2d(STA_POWER_DOWN);
	//spm_mtcmos_ctrl_mfg(STA_POWER_DOWN);

	/* Disable MFG-top clock */
	//ptp_write_field(SET_CLK_GATING_CTRL8, 7:6, 0x3);
}

/*=============================================================
 *Local variable definition
 *=============================================================*/
static void base_ops_switch_bank(struct ptp_det *det)
{
	FUNC_ENTER(FUNC_LV_HELP);

	/* Enable Top thermal clock */
	//ptp_write_field(CLR_CLK_GATING_CTRL1, 1:1, 0x1);
	//ptp_error("CLK_GATING_CTRL1 = 0x%X\n", ptp_read(CLK_GATING_CTRL1));

	turn_on_mfg_power_domain();

	/* SYSTEMCLK_CG_EN = 31:31 */
	/* PTPODCORE1EN = 17:17 */
	/* PTPODCORE0EN = 16:16 */

	ptp_write_field(PTP_PTPCORESEL, 31:31, 0x1);
	ptp_write_field(PTP_PTPCORESEL, 17:17, 0x1);
	ptp_write_field(PTP_PTPCORESEL, 16:16, 0x1);
	/* APBSEL = 3:0 */
	ptp_write_field(PTP_PTPCORESEL, 3:0, det->ctrl_id);

	ptp_error("PTP_PTPCORESEL = 0x%X\n", ptp_read(PTP_PTPCORESEL));
	FUNC_EXIT(FUNC_LV_HELP);
}

static struct ptp_devinfo ptp_devinfo;

static struct ptp_det_ops gpu_det_ops = {
	.switch_bank		= base_ops_switch_bank,
};

static struct ptp_det ptp_detectors[NR_PTP_DET] = {
	[PTP_DET_GPUSYS] = {
		.name		= __stringify(PTP_DET_GPUSYS),
		.ops		= &gpu_det_ops,
		.ctrl_id	= PTP_CTRL_GPUSYS,
		.features	= FEA_INIT01 | FEA_INIT02 | FEA_MON, /* <-@@@ */
		.freq_base	= 500, /* 500Mhz */
		.VBOOT		= 0x38, /* 1.15v */
		.VMAX		= 0x51, /* 1.30625v */
		.VMIN		= 0x38, /* 1.15v */
		.volt_offset	= 0, /* <-@@@ */
	},
};


/*=============================================================
 * Local function definition
 *=============================================================*/
const static struct devinfo {
	int sn;
	int M_HW_RES4;
	int M_HW_RES5;
	int M_HW_RES_3;
	int M_HW_RES0;
	int M_HW_RES1;
	int M_HW_RES7;
	int M_HW_RES8;
	int M_HW_RES9;
	int M_HW_RES6;
	int core;
	int gpu;
	int sram2;
	int sram1;
} devinfo[] = {
	{ 1410, 1401856, 637949991, 0x253013CF, 0x13131313, 0xCFCECFC9, 0x192C3C40, 0x43230A0C, 0x3B3C5A51, 0x76414681, 88.155296, 25.598700, 26.679930, 107.494598 },
};

static void get_devinfo(struct ptp_devinfo *p)
{
	int *PTPOD = (int *)p;
	int i;

	FUNC_ENTER(FUNC_LV_HELP);

#if TEST_FAKE_EFUSE
	PTPOD[0] = 0x10BD3C1B;
	PTPOD[1] = 0x0055C0FA;
	PTPOD[2] = 0x10BD3C1B;
	PTPOD[3] = 0x0055C0FA;
	PTPOD[4] = 0x00000000;
	PTPOD[5] = 0x00000000;
	PTPOD[6] = 0x00000000;
#else
	PTPOD[0] = seclib_get_devinfo_with_index(67);
	PTPOD[1] = seclib_get_devinfo_with_index(68);
	PTPOD[2] = seclib_get_devinfo_with_index(69);
	PTPOD[3] = seclib_get_devinfo_with_index(70);
	PTPOD[4] = seclib_get_devinfo_with_index(71);
	PTPOD[5] = seclib_get_devinfo_with_index(72);
	PTPOD[6] = seclib_get_devinfo_with_index(73);
#endif

	for (i = 0; i < sizeof(struct ptp_devinfo)/sizeof(unsigned int); i++)
		ptp_error("PTPOD%d\t= 0x%X\n", i, PTPOD[i]);

	FUNC_EXIT(FUNC_LV_HELP);
}

static int ptp_init_det(struct ptp_det *det, struct ptp_devinfo *devinfo)
{
	unsigned int vcore_pmic = 0, vcore_pmic_orig;
	enum ptp_det_id det_id = det_to_id(det);

	FUNC_ENTER(FUNC_LV_HELP);

	/* init with devinfo */
	det->PTPINITEN	= devinfo->PTPINITEN;
	det->PTPMONEN	= devinfo->PTPMONEN;

	/* init with constant */
	det->DETWINDOW	= DETWINDOW_VAL;

	det->DTHI	= DTHI_VAL;
	det->DTLO	= DTLO_VAL;
	det->DETMAX	= DETMAX_VAL;

	det->AGECONFIG	= AGECONFIG_VAL;
	det->AGEM	= AGEM_VAL;
	det->DVTFIXED	= DVTFIXED_VAL;
	det->VCO	= VCO_VAL;
	det->DCCONFIG	= DCCONFIG_VAL;

	switch (det_id) {
	case PTP_DET_GPUSYS:
		det->MDES	= devinfo->GPU_MDES;
		det->BDES	= devinfo->GPU_BDES;
		det->DCMDET	= devinfo->GPU_DCMDET;
		det->DCBDET	= devinfo->GPU_DCBDET;
		det->AGEDELTA	= devinfo->GPU_AGEDELTA;
		det->MTDES	= devinfo->GPU_MTDES;
		det->DVTFIXED	= 0x04;
		/* get DVFS frequency table */
		det->num_freq_tbl = 1;
		/* GPU reference base freq */
		det->freq_tbl[0] = PERCENT(ptp_detectors[PTP_DET_GPUSYS].freq_max, det->freq_base);

		/* Get original Vcore voltage */
		pmic_read_interface(0x318, &vcore_pmic_orig, 0xFFFF, 0);

		set_vcore_to_pmic(det_id,
			ptp_detectors[PTP_DET_GPUSYS].VBOOT + PTPOD_PMIC_OFFSET);

		pmic_read_interface(0x318, &vcore_pmic, 0xFFFF, 0);
		ptp_notice("Vcore is %duV now.\n", PTP_PMIC_VAL_TO_VOLT(vcore_pmic));
		if (det->VBOOT != (vcore_pmic - PTPOD_PMIC_OFFSET)) {
			ptp_error("VBOOT doesn't match. det->VBOOT = 0x%X, vcore_pmic = 0x%X\n",
				det->VBOOT, (vcore_pmic - PTPOD_PMIC_OFFSET));
			ptp_error("Restore early Vcore voltage\n");
			set_vcore_to_pmic(det_id, vcore_pmic_orig);
			pmic_read_interface(0x318, &vcore_pmic_orig, 0xFFFF, 0);
			ptp_notice("Vcore is %duV now.\n",PTP_PMIC_VAL_TO_VOLT(vcore_pmic_orig));
			return -1;
		}

		break;

	default:
		ptp_error("[%s]: Unknown det_id %d\n", __func__, det_id);
		break;
	}

	FUNC_EXIT(FUNC_LV_HELP);

	return 0;
}

static void base_ops_set_phase(struct ptp_det *det, ptp_phase phase)
{
	unsigned int i, filter, val;
	/* unsigned long flags; // <-XXX */

	FUNC_ENTER(FUNC_LV_HELP);

	/* mt_ptp_lock(&flags); // <-XXX */
	det->ops->switch_bank(det);

	/* config PTP register */
	ptp_write(PTP_DESCHAR, ((det->BDES << 8) & 0xff00) | (det->MDES & 0xff));
	ptp_write(PTP_TEMPCHAR, (((det->VCO << 16) & 0xff0000) | ((det->MTDES << 8) & 0xff00) | (det->DVTFIXED & 0xff)));
	ptp_write(PTP_DETCHAR, ((det->DCBDET << 8) & 0xff00) | (det->DCMDET & 0xff));
	ptp_write(PTP_AGECHAR, ((det->AGEDELTA << 8) & 0xff00) | (det->AGEM & 0xff));
	ptp_write(PTP_DCCONFIG, det->DCCONFIG);
	ptp_write(PTP_AGECONFIG, det->AGECONFIG);

	if (PTP_PHASE_MON == phase)
		ptp_write(PTP_TSCALCS, ((det->BTS << 12) & 0xfff000) | (det->MTS & 0xfff));

	if (det->AGEM == 0x0)
		ptp_write(PTP_RUNCONFIG, 0x80000000);
	else {
		val = 0x0;

		for (i = 0; i < 24; i += 2) {
			filter = 0x3 << i;

			if (((det->AGECONFIG) & filter) == 0x0)
				val |= (0x1 << i);
			else
				val |= ((det->AGECONFIG) & filter);
		}

		ptp_write(PTP_RUNCONFIG, val);
	}

	ptp_write(PTP_FREQPCT30,
		  ((det->freq_tbl[3] << 24) & 0xff000000)	|
		  ((det->freq_tbl[2] << 16) & 0xff0000)	|
		  ((det->freq_tbl[1] << 8) & 0xff00)	|
		  (det->freq_tbl[0] & 0xff));
	ptp_write(PTP_FREQPCT74,
		  ((det->freq_tbl[7] << 24) & 0xff000000)	|
		  ((det->freq_tbl[6] << 16) & 0xff0000)	|
		  ((det->freq_tbl[5] << 8) & 0xff00)	|
		  ((det->freq_tbl[4]) & 0xff));
	ptp_write(PTP_LIMITVALS,
		  ((det->VMAX << 24) & 0xff000000)	|
		  ((det->VMIN << 16) & 0xff0000)		|
		  ((det->DTHI << 8) & 0xff00)		|
		  (det->DTLO & 0xff));
	ptp_write(PTP_VBOOT, (((det->VBOOT) & 0xff)));
	ptp_write(PTP_DETWINDOW, (((det->DETWINDOW) & 0xffff)));
	ptp_write(PTP_PTPCONFIG, (((det->DETMAX) & 0xffff)));

	/* clear all pending PTP interrupt & config PTPINTEN */
	ptp_write(PTP_PTPINTSTS, 0xffffffff);

	switch (phase) {
	case PTP_PHASE_INIT01:
		ptp_write(PTP_PTPINTEN, 0x00005f01);
		/* enable PTP INIT measurement */
		ptp_write(PTP_PTPEN, 0x00000001);
		break;

	case PTP_PHASE_INIT02:
		ptp_write(PTP_PTPINTEN, 0x00005f01);
		ptp_write(PTP_INIT2VALS,
			  ((det->AGEVOFFSETIN << 16) & 0xffff0000) |
			  (det->DCVOFFSETIN & 0xffff));
		/* enable PTP INIT measurement */
		ptp_write(PTP_PTPEN, 0x00000005);
		break;

	case PTP_PHASE_MON:
		ptp_write(PTP_PTPINTEN, 0x00FF0000);
		/* enable PTP monitor mode */
		ptp_write(PTP_PTPEN, 0x00000002);
		break;

	default:
		ptp_error("[%s]: Unknown phase %d\n", __func__, phase);
		break;
	}

	/* mt_ptp_unlock(&flags); // <-XXX */

	FUNC_EXIT(FUNC_LV_HELP);
}

static void mt_ptp_reg_dump_locked(void)
{
	unsigned int addr;

	for (addr = PTP_DESCHAR; addr <= PTP_SMSTATE1; addr += 4)
		ptp_isr_info("0x%x = 0x%x\n", addr, *(volatile unsigned int *)addr);

	addr = PTP_PTPCORESEL;
	ptp_isr_info("0x%x = 0x%x\n", addr, *(volatile unsigned int *)addr);
}

static void mt_ptp_reg_dump(void)
{
	struct ptp_det *det;
	unsigned long flags;

	FUNC_ENTER(FUNC_LV_HELP);
#if PTP_LOG_EN
	ptp_isr_info("PTP_REVISIONID	= 0x%x\n", ptp_read(PTP_REVISIONID));
	ptp_isr_info("PTP_TEMPMONCTL0	= 0x%x\n", ptp_read(PTP_TEMPMONCTL0));
	ptp_isr_info("PTP_TEMPMONCTL1	= 0x%x\n", ptp_read(PTP_TEMPMONCTL1));
	ptp_isr_info("PTP_TEMPMONCTL2	= 0x%x\n", ptp_read(PTP_TEMPMONCTL2));
	ptp_isr_info("PTP_TEMPMONINT	= 0x%x\n", ptp_read(PTP_TEMPMONINT));
	ptp_isr_info("PTP_TEMPMONINTSTS	= 0x%x\n", ptp_read(PTP_TEMPMONINTSTS));
	ptp_isr_info("PTP_TEMPMONIDET0	= 0x%x\n", ptp_read(PTP_TEMPMONIDET0));
	ptp_isr_info("PTP_TEMPMONIDET1	= 0x%x\n", ptp_read(PTP_TEMPMONIDET1));
	ptp_isr_info("PTP_TEMPMONIDET2	= 0x%x\n", ptp_read(PTP_TEMPMONIDET2));
	ptp_isr_info("PTP_TEMPH2NTHRE	= 0x%x\n", ptp_read(PTP_TEMPH2NTHRE));
	ptp_isr_info("PTP_TEMPHTHRE	= 0x%x\n", ptp_read(PTP_TEMPHTHRE));
	ptp_isr_info("PTP_TEMPCTHRE	= 0x%x\n", ptp_read(PTP_TEMPCTHRE));
	ptp_isr_info("PTP_TEMPOFFSETH	= 0x%x\n", ptp_read(PTP_TEMPOFFSETH));
	ptp_isr_info("PTP_TEMPOFFSETL	= 0x%x\n", ptp_read(PTP_TEMPOFFSETL));
	ptp_isr_info("PTP_TEMPMSRCTL0	= 0x%x\n", ptp_read(PTP_TEMPMSRCTL0));
	ptp_isr_info("PTP_TEMPMSRCTL1	= 0x%x\n", ptp_read(PTP_TEMPMSRCTL1));
	ptp_isr_info("PTP_TEMPAHBPOLL	= 0x%x\n", ptp_read(PTP_TEMPAHBPOLL));
	ptp_isr_info("PTP_TEMPAHBTO	= 0x%x\n", ptp_read(PTP_TEMPAHBTO));
	ptp_isr_info("PTP_TEMPADCPNP0	= 0x%x\n", ptp_read(PTP_TEMPADCPNP0));
	ptp_isr_info("PTP_TEMPADCPNP1	= 0x%x\n", ptp_read(PTP_TEMPADCPNP1));
	ptp_isr_info("PTP_TEMPADCPNP2	= 0x%x\n", ptp_read(PTP_TEMPADCPNP2));
	ptp_isr_info("PTP_TEMPADCMUX	= 0x%x\n", ptp_read(PTP_TEMPADCMUX));
	ptp_isr_info("PTP_TEMPADCEXT	= 0x%x\n", ptp_read(PTP_TEMPADCEXT));
	ptp_isr_info("PTP_TEMPADCEXT1	= 0x%x\n", ptp_read(PTP_TEMPADCEXT1));
	ptp_isr_info("PTP_TEMPADCEN	= 0x%x\n", ptp_read(PTP_TEMPADCEN));
	ptp_isr_info("PTP_TEMPPNPMUXADDR	= 0x%x\n", ptp_read(PTP_TEMPPNPMUXADDR));
	ptp_isr_info("PTP_TEMPADCMUXADDR	= 0x%x\n", ptp_read(PTP_TEMPADCMUXADDR));
	ptp_isr_info("PTP_TEMPADCEXTADDR	= 0x%x\n", ptp_read(PTP_TEMPADCEXTADDR));
	ptp_isr_info("PTP_TEMPADCEXT1ADDR	= 0x%x\n", ptp_read(PTP_TEMPADCEXT1ADDR));
	ptp_isr_info("PTP_TEMPADCENADDR		= 0x%x\n", ptp_read(PTP_TEMPADCENADDR));
	ptp_isr_info("PTP_TEMPADCVALIDADDR	= 0x%x\n", ptp_read(PTP_TEMPADCVALIDADDR));
	ptp_isr_info("PTP_TEMPADCVOLTADDR	= 0x%x\n", ptp_read(PTP_TEMPADCVOLTADDR));
	ptp_isr_info("PTP_TEMPRDCTRL		= 0x%x\n", ptp_read(PTP_TEMPRDCTRL));
	ptp_isr_info("PTP_TEMPADCVALIDMASK	= 0x%x\n", ptp_read(PTP_TEMPADCVALIDMASK));
	ptp_isr_info("PTP_TEMPADCVOLTAGESHIFT	= 0x%x\n", ptp_read(PTP_TEMPADCVOLTAGESHIFT));
	ptp_isr_info("PTP_TEMPADCWRITECTRL	= 0x%x\n", ptp_read(PTP_TEMPADCWRITECTRL));
	ptp_isr_info("PTP_TEMPMSR0	= 0x%x\n", ptp_read(PTP_TEMPMSR0));
	ptp_isr_info("PTP_TEMPMSR1	= 0x%x\n", ptp_read(PTP_TEMPMSR1));
	ptp_isr_info("PTP_TEMPMSR2	= 0x%x\n", ptp_read(PTP_TEMPMSR2));
	ptp_isr_info("PTP_TEMPIMMD0	= 0x%x\n", ptp_read(PTP_TEMPIMMD0));
	ptp_isr_info("PTP_TEMPIMMD1	= 0x%x\n", ptp_read(PTP_TEMPIMMD1));
	ptp_isr_info("PTP_TEMPIMMD2	= 0x%x\n", ptp_read(PTP_TEMPIMMD2));
	ptp_isr_info("PTP_TEMPMONIDET3	= 0x%x\n", ptp_read(PTP_TEMPMONIDET3));
	ptp_isr_info("PTP_TEMPADCPNP3	= 0x%x\n", ptp_read(PTP_TEMPADCPNP3));
	ptp_isr_info("PTP_TEMPMSR3	= 0x%x\n", ptp_read(PTP_TEMPMSR3));
	ptp_isr_info("PTP_TEMPIMMD3	= 0x%x\n", ptp_read(PTP_TEMPIMMD3));
	ptp_isr_info("PTP_TEMPPROTCTL	= 0x%x\n", ptp_read(PTP_TEMPPROTCTL));
	ptp_isr_info("PTP_TEMPPROTTA	= 0x%x\n", ptp_read(PTP_TEMPPROTTA));
	ptp_isr_info("PTP_TEMPPROTTB	= 0x%x\n", ptp_read(PTP_TEMPPROTTB));
	ptp_isr_info("PTP_TEMPPROTTC	= 0x%x\n", ptp_read(PTP_TEMPPROTTC));
	ptp_isr_info("PTP_TEMPSPARE0	= 0x%x\n", ptp_read(PTP_TEMPSPARE0));
	ptp_isr_info("PTP_TEMPSPARE1	= 0x%x\n", ptp_read(PTP_TEMPSPARE1));
	ptp_isr_info("PTP_TEMPSPARE2	= 0x%x\n", ptp_read(PTP_TEMPSPARE2));
	ptp_isr_info("PTP_TEMPSPARE3	= 0x%x\n", ptp_read(PTP_TEMPSPARE3));
#endif
	for_each_det(det) {
		/* mt_ptp_lock(&flags); */
		det->ops->switch_bank(det);

#if PTP_LOG_EN
		ptp_isr_info("PTP_DESCHAR[%s]	= 0x%x\n", det->name, ptp_read(PTP_DESCHAR));
		ptp_isr_info("PTP_TEMPCHAR[%s]	= 0x%x\n", det->name, ptp_read(PTP_TEMPCHAR));
		ptp_isr_info("PTP_DETCHAR[%s]	= 0x%x\n", det->name, ptp_read(PTP_DETCHAR));
		ptp_isr_info("PTP_AGECHAR[%s]	= 0x%x\n", det->name, ptp_read(PTP_AGECHAR));
		ptp_isr_info("PTP_DCCONFIG[%s]	= 0x%x\n", det->name, ptp_read(PTP_DCCONFIG));
		ptp_isr_info("PTP_AGECONFIG[%s]	= 0x%x\n", det->name, ptp_read(PTP_AGECONFIG));
		ptp_isr_info("PTP_FREQPCT30[%s]	= 0x%x\n", det->name, ptp_read(PTP_FREQPCT30));
		ptp_isr_info("PTP_FREQPCT74[%s]	= 0x%x\n", det->name, ptp_read(PTP_FREQPCT74));
		ptp_isr_info("PTP_LIMITVALS[%s]	= 0x%x\n", det->name, ptp_read(PTP_LIMITVALS));
		ptp_isr_info("PTP_VBOOT[%s]	= 0x%x\n", det->name, ptp_read(PTP_VBOOT));
		ptp_isr_info("PTP_DETWINDOW[%s]	= 0x%x\n", det->name, ptp_read(PTP_DETWINDOW));
		ptp_isr_info("PTP_PTPCONFIG[%s]	= 0x%x\n", det->name, ptp_read(PTP_PTPCONFIG));
		ptp_isr_info("PTP_TSCALCS[%s]	= 0x%x\n", det->name, ptp_read(PTP_TSCALCS));
		ptp_isr_info("PTP_RUNCONFIG[%s]	= 0x%x\n", det->name, ptp_read(PTP_RUNCONFIG));
		ptp_isr_info("PTP_PTPEN[%s]	= 0x%x\n", det->name, ptp_read(PTP_PTPEN));
		ptp_isr_info("PTP_INIT2VALS[%s]	= 0x%x\n", det->name, ptp_read(PTP_INIT2VALS));
		ptp_isr_info("PTP_DCVALUES[%s]	= 0x%x\n", det->name, ptp_read(PTP_DCVALUES));
		ptp_isr_info("PTP_AGEVALUES[%s]	= 0x%x\n", det->name, ptp_read(PTP_AGEVALUES));
		ptp_isr_info("PTP_VOP30[%s]	= 0x%x\n", det->name, ptp_read(PTP_VOP30));
		ptp_isr_info("PTP_VOP74[%s]	= 0x%x\n", det->name, ptp_read(PTP_VOP74));
		ptp_isr_info("PTP_TEMP[%s]	= 0x%x\n", det->name, ptp_read(PTP_TEMP));
		ptp_isr_info("PTP_PTPINTSTS[%s]	= 0x%x\n", det->name, ptp_read(PTP_PTPINTSTS));
		ptp_isr_info("PTP_PTPINTSTSRAW[%s]	= 0x%x\n", det->name, ptp_read(PTP_PTPINTSTSRAW));
		ptp_isr_info("PTP_PTPINTEN[%s]	= 0x%x\n", det->name, ptp_read(PTP_PTPINTEN));
		ptp_isr_info("PTP_SMSTATE0[%s]	= 0x%x\n", det->name, ptp_read(PTP_SMSTATE0));
		ptp_isr_info("PTP_SMSTATE1[%s]	= 0x%x\n", det->name, ptp_read(PTP_SMSTATE1));
		ptp_isr_info("PTP_PTPCTL0[%s]	= 0x%x\n", det->name, ptp_read(PTP_PTPCTL0));
#else
		ptp_isr_info("PTP_DESCHAR[%s]   = 0x%x\n", det->name, ptp_read(PTP_DESCHAR));
		ptp_isr_info("PTP_TEMPCHAR[%s]	= 0x%x\n", det->name, ptp_read(PTP_TEMPCHAR));
		ptp_isr_info("PTP_DETCHAR[%s]   = 0x%x\n", det->name, ptp_read(PTP_DETCHAR));
		ptp_isr_info("PTP_VBOOT[%s]     = 0x%x\n", det->name, ptp_read(PTP_VBOOT));
		ptp_isr_info("PTP_INIT2VALS[%s]	= 0x%x\n", det->name, ptp_read(PTP_INIT2VALS));
		ptp_isr_info("PTP_DETWINDOW[%s] = 0x%x\n", det->name, ptp_read(PTP_DETWINDOW));
		ptp_isr_info("PTP_DCVALUES[%s]  = 0x%x\n", det->name, ptp_read(PTP_DCVALUES));
		ptp_isr_info("PTP_FREQPCT30[%s] = 0x%x\n", det->name, ptp_read(PTP_FREQPCT30));
		ptp_isr_info("PTP_FREQPCT74[%s] = 0x%x\n", det->name, ptp_read(PTP_FREQPCT74));
		ptp_isr_info("PTP_LIMITVALS[%s] = 0x%x\n", det->name, ptp_read(PTP_LIMITVALS));
		ptp_isr_info("PTP_PTPCHKSHIFT[%s] = 0x%x\n", det->name, ptp_read(PTP_PTPCHKSHIFT));
		ptp_isr_info("PTP_VDESIGN30[%s] = 0x%x\n", det->name, ptp_read(PTP_VDESIGN30));
		ptp_isr_info("PTP_VDESIGN74[%s] = 0x%x\n", det->name, ptp_read(PTP_VDESIGN74));
		ptp_isr_info("PTP_VOP30[%s]     = 0x%x\n", det->name, ptp_read(PTP_VOP30));
		ptp_isr_info("PTP_VOP74[%s]     = 0x%x\n", det->name, ptp_read(PTP_VOP74));
		ptp_isr_info("PTP_PTPEN[%s]     = 0x%x\n", det->name, ptp_read(PTP_PTPEN));
#endif
		/* mt_ptp_unlock(&flags); */
	}
#if PTP_LOG_EN
	ptp_isr_info("PTP_PTPCORESEL	= 0x%x\n", ptp_read(PTP_PTPCORESEL));
	ptp_isr_info("PTP_THERMINTST	= 0x%x\n", ptp_read(PTP_THERMINTST));
	ptp_isr_info("PTP_PTPODINTST	= 0x%x\n", ptp_read(PTP_PTPODINTST));
	ptp_isr_info("PTP_THSTAGE0ST	= 0x%x\n", ptp_read(PTP_THSTAGE0ST));
	ptp_isr_info("PTP_THSTAGE1ST	= 0x%x\n", ptp_read(PTP_THSTAGE1ST));
	ptp_isr_info("PTP_THSTAGE2ST	= 0x%x\n", ptp_read(PTP_THSTAGE2ST));
	ptp_isr_info("PTP_THAHBST0	= 0x%x\n", ptp_read(PTP_THAHBST0));
	ptp_isr_info("PTP_THAHBST1	= 0x%x\n", ptp_read(PTP_THAHBST1));
	ptp_isr_info("PTP_PTPSPARE0	= 0x%x\n", ptp_read(PTP_PTPSPARE0));
	ptp_isr_info("PTP_PTPSPARE1	= 0x%x\n", ptp_read(PTP_PTPSPARE1));
	ptp_isr_info("PTP_PTPSPARE2	= 0x%x\n", ptp_read(PTP_PTPSPARE2));
	ptp_isr_info("PTP_PTPSPARE3	= 0x%x\n", ptp_read(PTP_PTPSPARE3));
	ptp_isr_info("PTP_THSLPEVEB	= 0x%x\n", ptp_read(PTP_THSLPEVEB));
#endif
	FUNC_EXIT(FUNC_LV_HELP);
}

static inline void handle_init01_isr(struct ptp_det *det)
{
	FUNC_ENTER(FUNC_LV_LOCAL);

	ptp_isr_info("@ %s(%s)\n", __func__, det->name);

	/*
	 * Read & store 16 bit values DCVALUES.DCVOFFSET and
	 * AGEVALUES.AGEVOFFSET for later use in INIT2 procedure
	 */
	/* det->DCVOFFSETIN = ~(ptp_read(PTP_DCVALUES) & 0xffff) + 1; */ /* hw bug, workaround */
	det->DCVOFFSETIN = 0; /* for MT8168 only */
	det->AGEVOFFSETIN = ptp_read(PTP_AGEVALUES) & 0xffff;

	/*
	 * Set PTPEN.PTPINITEN/PTPEN.PTPINIT2EN = 0x0 &
	 * Clear PTP INIT interrupt PTPINTSTS = 0x00000001
	 */
	ptp_write(PTP_PTPEN, 0x0);
	ptp_write(PTP_PTPINTSTS, 0x1);
	/* ptp_init01_finish(det); */
	/* base_ops_set_phase(det, PTP_PHASE_INIT02); */

	FUNC_EXIT(FUNC_LV_LOCAL);
}

static inline void handle_init02_isr(struct ptp_det *det)
{
	unsigned int temp;
	int i;
	/* struct ptp_ctrl *ctrl = id_to_ptp_ctrl(det->ctrl_id); */

	FUNC_ENTER(FUNC_LV_LOCAL);

	ptp_isr_info("@ %s(%s)\n", __func__, det->name);

	temp = ptp_read(PTP_VOP30);
	det->volt_tbl[0] = temp & 0xff;
	det->volt_tbl[1] = (temp >> 8) & 0xff;
	det->volt_tbl[2] = (temp >> 16) & 0xff;
	det->volt_tbl[3] = (temp >> 24) & 0xff;

	temp = ptp_read(PTP_VOP74);
	det->volt_tbl[4] = temp & 0xff;
	det->volt_tbl[5] = (temp >> 8) & 0xff;
	det->volt_tbl[6] = (temp >> 16) & 0xff;
	det->volt_tbl[7] = (temp >> 24) & 0xff;

	for (i = 0; i < NR_FREQ; i++) {
		det->volt_tbl[i] = det->volt_tbl[i] + PTPOD_PMIC_OFFSET;
		ptp_isr_info("ptp_detectors[%s].volt_tbl[%d] = 0x%x\n", det->name, i, det->volt_tbl[i]);
	}

	memcpy(det->volt_tbl_init2, det->volt_tbl, sizeof(det->volt_tbl_init2));
	/* ptp_write(PTP_VBOOT, ((det->volt_tbl_init2[1]) & 0xff)+det_to_id(det)); */

	/* ptp_isr_info("ptp_level = 0x%x\n", ptp_level); */

	/* ptp_set_ptp_volt(det); */

	/*
	 * Set PTPEN.PTPINITEN/PTPEN.PTPINIT2EN = 0x0 &
	 * Clear PTP INIT interrupt PTPINTSTS = 0x00000001
	 */
	ptp_write(PTP_PTPEN, 0x0);
	ptp_write(PTP_PTPINTSTS, 0x1);

	/* atomic_dec(&ctrl->in_init); */
	/*  complete(&ctrl->init_done); */
	/* det->ops->mon_mode(det); */

	FUNC_EXIT(FUNC_LV_LOCAL);
}

static inline void handle_init_err_isr(struct ptp_det *det)
{
	FUNC_ENTER(FUNC_LV_LOCAL);

	ptp_isr_info("====================================================\n");
	ptp_isr_info("@ %s(%s)\n", __func__, det->name);
	ptp_isr_info("PTP init err: PTPEN(0x%x) = 0x%x, PTPINTSTS(0x%x) = 0x%x\n",
		     PTP_PTPEN, ptp_read(PTP_PTPEN),
		     PTP_PTPINTSTS, ptp_read(PTP_PTPINTSTS));
	ptp_isr_info("PTP_SMSTATE0 (0x%x) = 0x%x\n",
		     PTP_SMSTATE0, ptp_read(PTP_SMSTATE0));
	ptp_isr_info("PTP_SMSTATE1 (0x%x) = 0x%x\n",
		     PTP_SMSTATE1, ptp_read(PTP_SMSTATE1));
	ptp_isr_info("====================================================\n");
#if 0
	/* TODO: FIXME */
	{
		struct ptp_ctrl *ctrl = id_to_ptp_ctrl(det->ctrl_id);
		atomic_dec(&ctrl->in_init);
		complete(&ctrl->init_done);
	}
	/* TODO: FIXME */

	det->ops->disable_locked(det, BY_INIT_ERROR);
#endif
	FUNC_EXIT(FUNC_LV_LOCAL);
}

static inline void handle_mon_mode_isr(struct ptp_det *det)
{
	unsigned int temp;
	int i;

	FUNC_ENTER(FUNC_LV_LOCAL);

	ptp_isr_info("@ %s(%s)\n", __func__, det->name);

	/* check if thermal sensor init completed? */
	temp = (ptp_read(PTP_TEMP) & 0xff);

	if ((temp > 0x4b) && (temp < 0xd3)) {
		ptp_isr_info("thermal sensor init has not been completed. "
			     "(temp = 0x%x)\n", temp);
		goto out;
	}

	temp = ptp_read(PTP_VOP30);
	det->volt_tbl[0] = temp & 0xff;
	det->volt_tbl[1] = (temp >> 8) & 0xff;
	det->volt_tbl[2] = (temp >> 16) & 0xff;
	det->volt_tbl[3] = (temp >> 24) & 0xff;

	temp = ptp_read(PTP_VOP74);
	det->volt_tbl[4] = temp & 0xff;
	det->volt_tbl[5] = (temp >> 8) & 0xff;
	det->volt_tbl[6] = (temp >> 16) & 0xff;
	det->volt_tbl[7] = (temp >> 24) & 0xff;

	for (i = 0; i < NR_FREQ; i++)
		ptp_isr_info("ptp_detectors[%s].volt_tbl[%d] = 0x%x\n", det->name, i, det->volt_tbl[i]);

	/* ptp_isr_info("ptp_level = 0x%x\n", ptp_level); */
	ptp_isr_info("ISR : TEMPSPARE1 = 0x%x\n", ptp_read(PTP_TEMPSPARE1));
	/* ptp_set_ptp_volt(det); */

out:
	/* Clear PTP INIT interrupt PTPINTSTS = 0x00ff0000 */
	ptp_write(PTP_PTPINTSTS, 0x00ff0000);

	FUNC_EXIT(FUNC_LV_LOCAL);
}

static inline void handle_mon_err_isr(struct ptp_det *det)
{
	FUNC_ENTER(FUNC_LV_LOCAL);

	/* PTP Monitor mode error handler */
	ptp_isr_info("====================================================\n");
	ptp_isr_info("@ %s(%s)\n", __func__, det->name);
	ptp_isr_info("PTP mon err: PTPEN(0x%x) = 0x%x, PTPINTSTS(0x%x) = 0x%x\n",
		     PTP_PTPEN, ptp_read(PTP_PTPEN),
		     PTP_PTPINTSTS, ptp_read(PTP_PTPINTSTS));
	ptp_isr_info("PTP_SMSTATE0 (0x%x) = 0x%x\n",
		     PTP_SMSTATE0, ptp_read(PTP_SMSTATE0));
	ptp_isr_info("PTP_SMSTATE1 (0x%x) = 0x%x\n",
		     PTP_SMSTATE1, ptp_read(PTP_SMSTATE1));
	ptp_isr_info("PTP_TEMP (0x%x) = 0x%x\n",
		     PTP_TEMP, ptp_read(PTP_TEMP));
	ptp_isr_info("PTP_TEMPMSR0 (0x%x) = 0x%x\n",
		     PTP_TEMPMSR0, ptp_read(PTP_TEMPMSR0));
	ptp_isr_info("PTP_TEMPMSR1 (0x%x) = 0x%x\n",
		     PTP_TEMPMSR1, ptp_read(PTP_TEMPMSR1));
	ptp_isr_info("PTP_TEMPMSR2 (0x%x) = 0x%x\n",
		     PTP_TEMPMSR2, ptp_read(PTP_TEMPMSR2));
	ptp_isr_info("PTP_TEMPMONCTL0 (0x%x) = 0x%x\n",
		     PTP_TEMPMONCTL0, ptp_read(PTP_TEMPMONCTL0));
	ptp_isr_info("PTP_TEMPMSRCTL1 (0x%x) = 0x%x\n",
		     PTP_TEMPMSRCTL1, ptp_read(PTP_TEMPMSRCTL1));
	ptp_isr_info("====================================================\n");

	/* det->ops->disable_locked(det, BY_MON_ERROR); */

	FUNC_EXIT(FUNC_LV_LOCAL);
}

static inline void ptp_isr_handler(struct ptp_det *det)
{
	unsigned int PTPINTSTS, PTPEN;

	FUNC_ENTER(FUNC_LV_LOCAL);

	PTPINTSTS = ptp_read(PTP_PTPINTSTS);
	PTPEN = ptp_read(PTP_PTPEN);

	ptp_isr_info("[%s]\n", det->name);

	if (PTPINTSTS == 0x1) { /* PTP init1 or init2 */
		if ((PTPEN & 0x7) == 0x1)   /* PTP init1 */
			handle_init01_isr(det);
		else if ((PTPEN & 0x7) == 0x5)   /* PTP init2 */
			handle_init02_isr(det);
		else {
			/*
			 * error : init1 or init2,
			 * but enable setting is wrong.
			 */
			handle_init_err_isr(det);
		}
	} else if ((PTPINTSTS & 0x00ff0000) != 0x0)
		handle_mon_mode_isr(det);
	else { /* PTP error handler */
		/* init 1  || init 2 error handler */
		if (((PTPEN & 0x7) == 0x1) || ((PTPEN & 0x7) == 0x5))
			handle_init_err_isr(det);
		else /* PTP Monitor mode error handler */
			handle_mon_err_isr(det);
	}

	FUNC_EXIT(FUNC_LV_LOCAL);
}

static void ptp_isr(void)
{
	struct ptp_det *det;

	FUNC_ENTER(FUNC_LV_MODULE);

	ptp_error("%s()\n", __func__);
	ptp_error("PTP_PTPCORESEL = 0x%X\n", ptp_read(PTP_PTPCORESEL));

	while (BIT(PTP_CTRL_GPUSYS) & ptp_read(PTP_PTPODINTST))
		;

	ptp_error("PTP_PTPODINTST = 0X%X\n", ptp_read(PTP_PTPODINTST));

	det = &ptp_detectors[PTP_DET_GPUSYS];

	det->ops->switch_bank(det);
	/* mt_ptp_reg_dump_locked(); // <-XXX */
	ptp_isr_handler(det);

	FUNC_EXIT(FUNC_LV_MODULE);
}

static int set_vcore_to_pmic(enum ptp_det_id det_id, unsigned int target_volt)
{
	unsigned int vcore_pmic;

	ptp_notice("%s()\n", __func__);

	if (det_id == PTP_CTRL_GPUSYS) {
		pmic_config_interface(0x312, target_volt, 0x7F, 0);
		pmic_config_interface(0x314, target_volt, 0x7F, 0);

		/* Read Vcore Voltage */
		pmic_read_interface(0x318, &vcore_pmic, 0xFFFF, 0);
		ptp_debug("Vcore pmic = 0x%X\n", vcore_pmic);
	}

	return 0;
}

/*=============================================================
 * Global function definition
 *=============================================================*/

void ptp_init(void)
{
	int i, reg_val, ret;
	unsigned int vcore_pmic, vcore_ptp_volt = 0, gpu_freq_min;

	FUNC_ENTER(FUNC_LV_MODULE);

	get_devinfo(&ptp_devinfo);
	ptp_detectors[PTP_DET_GPUSYS].freq_max = mt_gpufreq_get_max_freq();
	gpu_freq_min = mt_gpufreq_get_min_freq();
#if TEST_FAKE_EFUSE
	ptp_detectors[PTP_DET_GPUSYS].freq_max = 574;
#else
	if (ptp_devinfo.PTPINITEN == 0) {
		ptp_notice("PTPINITEN = 0x%x\n", ptp_devinfo.PTPINITEN);
		return;
	} else if (ptp_detectors[PTP_DET_GPUSYS].freq_max <= gpu_freq_min) {
		ptp_notice("GPU's max freq is %dMHz <= %dMHz\n", ptp_detectors[PTP_DET_GPUSYS].freq_max, gpu_freq_min);
		ptp_notice("PTPOD doesn't need to tune down Vcore voltage\n");
		return;
	}
#endif
	ptp_notice("GPU's max freq = %dMHz\n", ptp_detectors[PTP_DET_GPUSYS].freq_max);

	ret = ptp_init_det(&ptp_detectors[PTP_DET_GPUSYS], &ptp_devinfo);
	if (ret != 0)
		return;

	/* pmic PWM */
	pmic_config_interface(0x304, 0x1, 0x1, 8);

	/* Check Vcore mode */
	pmic_read_interface(0x304, &reg_val, 0xFFFF, 0);
	ptp_notice("Vcore(0x304) mode = 0x%X\n", reg_val);

	/* PTPOD init01 */
	base_ops_set_phase(&ptp_detectors[PTP_CTRL_GPUSYS], PTP_PHASE_INIT01);
	ptp_isr();
	mt_ptp_reg_dump();

	/* PTPOD init02 */
	base_ops_set_phase(&ptp_detectors[PTP_CTRL_GPUSYS], PTP_PHASE_INIT02);
	ptp_isr();
	mt_ptp_reg_dump();

	vcore_pmic = ptp_detectors[PTP_CTRL_GPUSYS].volt_tbl_init2[0];

	/* vcore_pmic = (vcore_pmic == 0) ? ptp_detectors[PTP_CTRL_GPUSYS].VMAX : vcore_pmic; */
	ptp_debug("Set pmic volt: 0x%X\n", vcore_pmic);

	/* Set PTPOD Vcore voltage */
	pmic_config_interface(0x312, vcore_pmic, 0x7F, 0);
	pmic_config_interface(0x314, vcore_pmic, 0x7F, 0);

	/* pmic auto */
	pmic_config_interface(0x304, 0x0, 0x1, 8);

	/* Check Vcore mode */
	pmic_read_interface(0x304, &reg_val, 0xFFFF, 0);
	ptp_notice("Vcore(0x304) mode = 0x%X\n", reg_val);

	/* Read PMIC Vcore step */
	pmic_read_interface(0x318, &vcore_pmic, 0xFFFF, 0);
	vcore_ptp_volt = PTP_PMIC_VAL_TO_VOLT(vcore_pmic);

	ptp_notice("Right now\n");
	ptp_notice("Vcore is %duV\n", vcore_ptp_volt);

	/* Disable Top thermal clock */
	//ptp_write_field(SET_CLK_GATING_CTRL1, 1:1, 0x1);
	//ptp_error("CLK_GATING_CTRL1 = 0x%X\n", ptp_read(CLK_GATING_CTRL1));

	turn_off_mfg_power_domain();

	FUNC_EXIT(FUNC_LV_MODULE);
}
