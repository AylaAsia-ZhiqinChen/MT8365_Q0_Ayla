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

/*
 * ==========================================================================
 *   Include Files
 * ==========================================================================
 */

#include "dramc_common.h"
#include "x_hal_io.h"
#if (FOR_DV_SIMULATION_USED == 0)
#include "emi.h"
#endif

#if (FOR_DV_SIMULATION_USED == 0)
#if USE_PMIC_CHIP_MT6357
#include <regulator/mtk_regulator.h>
#include <rt5738.h>
#include <fan53526.h>
#include <hl7593.h>
#include "mt6357.h"
#endif
#endif

/*
 * Voltage info  for diff dram type and frequency
 * according to DVFS table provided by DE
 */
#if DUAL_FREQ_K
#define	LP4_DIV8_DDR3200_VCORE	800000
#define	LP4_DIV8_DDR2666_VCORE	750000
#define	LP4_DIV8_DDR1600_VCORE	675000
#define	LP4_DIV4_DDR1600_VCORE	800000
#define	LP4_DIV4_DDR1333_VCORE	750000
#define	LP4_DIV4_DDR1200_VCORE	675000

#define	LP3_DDR1600_VCORE		800000
#define	LP3_DDR1333_VCORE		750000
#define	LP3_DDR1200_VCORE		675000

#define	PCDDR4_VCORE			750000
#define	PCDDR34_VCORE_H_FREQ		800000
#define	PCDDR3_VCORE			750000
#else
#define	LP4_DIV8_DDR3200_VCORE	800000
#define	LP4_DIV8_DDR2666_VCORE	700000
#define	LP4_DIV8_DDR1600_VCORE	650000
#define	LP4_DIV4_DDR1600_VCORE	800000
#define	LP4_DIV4_DDR1333_VCORE	700000
#define	LP4_DIV4_DDR1200_VCORE	650000

#define	LP3_DDR1600_VCORE		800000
#define	LP3_DDR1333_VCORE		700000
#define	LP3_DDR1200_VCORE		650000

#define	PCDDR4_VCORE			750000
#define	PCDDR34_VCORE_H_FREQ		800000
#define	PCDDR3_VCORE			750000
#endif

#define LP4_VDD2				1125000 // 1100000
#define LP4_VDD2_LV				1070000
#define LP4_VDD2_HV				1160000

#define LP4X_VDDQ				600000
#define PC4_VDD					1200000

static int volt_prefix_nv(int vol) {return vol;}
static int volt_prefix_hv(int vol) {return (vol==LP4_VDD2)?LP4_VDD2_HV:((vol*105)/100);}
static int volt_prefix_lv(int vol) {return (vol==LP4_VDD2)?LP4_VDD2_LV:((vol*95)/100);}

#ifdef NVCORE_NVDRAM
static int (*vcore_prefix)(int) = volt_prefix_nv;
static int (*vdram_prefix)(int) = volt_prefix_nv;
#elif defined(HVCORE_HVDRAM)
static int (*vcore_prefix)(int) = volt_prefix_hv;
static int (*vdram_prefix)(int) = volt_prefix_hv;
#elif defined(LVCORE_LVDRAM)
static int (*vcore_prefix)(int) = volt_prefix_lv;
static int (*vdram_prefix)(int) = volt_prefix_lv;
#elif defined(HVCORE_LVDRAM)
static int (*vcore_prefix)(int) = volt_prefix_hv;
static int (*vdram_prefix)(int) = volt_prefix_lv;
#elif defined(LVCORE_HVDRAM)
static int (*vcore_prefix)(int) = volt_prefix_lv;
static int (*vdram_prefix)(int) = volt_prefix_hv;
#elif defined(NVCORE_LVDRAM)
static int (*vcore_prefix)(int) = volt_prefix_nv;
static int (*vdram_prefix)(int) = volt_prefix_lv;
#elif defined(NVCORE_HVDRAM)
static int (*vcore_prefix)(int) = volt_prefix_nv;
static int (*vdram_prefix)(int) = volt_prefix_hv;
#else
int (*vcore_prefix)(int) = volt_prefix_nv;
int (*vdram_prefix)(int) = volt_prefix_nv;
#endif

#if defined(DRAM_HQA) || defined(DRAM_ETT)
void set_vcore_prefix(VOLT_PREFIX_T prefix)
{
	if(VOLT_PREFIX_HV == prefix)
		vcore_prefix = volt_prefix_hv;
	else if(VOLT_PREFIX_LV == prefix)
		vcore_prefix = volt_prefix_lv;
	else
		vcore_prefix = volt_prefix_nv;
}

void set_vdram_prefix(VOLT_PREFIX_T prefix)
{
	if(VOLT_PREFIX_HV == prefix)
		vdram_prefix = volt_prefix_hv;
	else if(VOLT_PREFIX_LV == prefix)
		vdram_prefix = volt_prefix_lv;
	else
		vdram_prefix = volt_prefix_nv;
}
#endif

void set_vcore_voltage(int vcore_vol)
{
#if USE_PMIC_CHIP_MT6357
	struct mtk_regulator reg_vcore;
	mtk_regulator_get("vcore", &reg_vcore);

	mtk_regulator_set_voltage(&reg_vcore, vcore_vol, vcore_max_uV);
	show_msg((INFO, "[dram] set vcore=%d\n", vcore_vol));
#endif
}

int get_vcore_voltage(DRAMC_CTX_T *p)
{
	int vcore_vol = 800000;

#if USE_PMIC_CHIP_MT6357
	struct mtk_regulator reg_vcore;
	mtk_regulator_get("vcore", &reg_vcore);

	vcore_vol = mtk_regulator_get_voltage(&reg_vcore);
#endif
	return vcore_vol;
}

void set_vcore_by_freq(DRAMC_CTX_T *p)
{
#if (FOR_DV_SIMULATION_USED == 0) /* cc notes: confirm for REAL CHIP */
#if USE_PMIC_CHIP_MT6357
	int vcore_vol = 800000;

	if(is_lp4_family(p)) {
		if(get_div_mode(p) == DIV8_MODE) {
			switch(p->frequency) {
			case DDR3200_FREQ:
				vcore_vol = LP4_DIV8_DDR3200_VCORE;
				break;
			case DDR2666_FREQ:
				vcore_vol = LP4_DIV8_DDR2666_VCORE;
				break;
			case DDR1600_FREQ:
				vcore_vol = LP4_DIV8_DDR1600_VCORE;
				break;
			default:
				break;
			}
		} else {
			switch(p->frequency) {
			case DDR1600_FREQ:
				vcore_vol = LP4_DIV4_DDR1600_VCORE;
				break;
			case DDR1333_FREQ:
				vcore_vol = LP4_DIV4_DDR1333_VCORE;
				break;
			case DDR1200_FREQ:
				vcore_vol = LP4_DIV4_DDR1200_VCORE;
				break;
			default:
				break;
			}
		}
	}else if(p->dram_type == TYPE_LPDDR3) {
		switch(p->frequency) {
		case DDR1600_FREQ:
			vcore_vol = LP3_DDR1600_VCORE;
			break;
		case DDR1333_FREQ:
			vcore_vol = LP3_DDR1333_VCORE;
			break;
		case DDR1200_FREQ:
			vcore_vol = LP3_DDR1200_VCORE;
			break;
		default:
			break;
		}
	} else if(p->dram_type == TYPE_PCDDR4) {
		if((p->frequency == DDR3200_FREQ) || (p->frequency == DDR1600_FREQ))
			vcore_vol = PCDDR34_VCORE_H_FREQ;
		else
			vcore_vol = PCDDR4_VCORE;
	} else if(p->dram_type == TYPE_PCDDR3) {
		if(p->frequency == DDR1600_FREQ)
			vcore_vol = PCDDR34_VCORE_H_FREQ;
		else
			vcore_vol = PCDDR3_VCORE;
	} else {
		print("[%s]unsuport ddr type!!!\n", __func__);
	}

	set_vcore_voltage(vcore_prefix(vcore_vol));
#endif
#endif
}

//#if defined(DRAM_HQA) || defined(DRAM_ETT)
void set_dram_voltage(DRAMC_CTX_T *p)
{
#if (FOR_DV_SIMULATION_USED == 0) /* cc notes: confirm for REAL CHIP */
#if USE_PMIC_CHIP_MT6357
	struct mtk_regulator reg_vfe28;
	struct mtk_regulator reg_vdram;

	mtk_regulator_get("vfe28", &reg_vfe28);
	mtk_regulator_get("vdram", &reg_vdram);

	if(p->dram_type != TYPE_PCDDR4) {
		mtk_regulator_enable(&reg_vfe28, 0);
	}

	if ((is_lp4_family(p)) || (p->dram_type == TYPE_PCDDR4)) {
		mtk_regulator_enable(&reg_vdram, 0);
	}

	switch(p->dram_type) {
	case TYPE_LPDDR4:
		//1.1V, VDDQ use the same power with VDD2
		if (g_rt5738_hw_exist[MT6691_VDD2])
			rt5738_set_voltage(MT6691_VDD2, vdram_prefix(LP4_VDD2));
		else if (g_hl7593_hw_exist[hl7593_VDD2])
			hl7593_set_voltage(hl7593_VDD2, vdram_prefix(LP4_VDD2));
		else if (g_fan53526_hw_exist[FAN53526_VDD2])
			fan53526_set_voltage(FAN53526_VDD2, vdram_prefix(LP4_VDD2));

		if (g_rt5738_hw_exist[MT6691_VDD2])
			rt5738_enable(MT6691_VDD2, 1);
		else if (g_hl7593_hw_exist[hl7593_VDD2])
			hl7593_enable(hl7593_VDD2, 1);
		else if (g_fan53526_hw_exist[FAN53526_VDD2])
			fan53526_enable(FAN53526_VDD2, 1);
		break;
	case TYPE_LPDDR4X:
		//1.1V
		if (g_rt5738_hw_exist[MT6691_VDD2])
			rt5738_set_voltage(MT6691_VDD2, vdram_prefix(LP4_VDD2));
		else if (g_hl7593_hw_exist[hl7593_VDD2])
			hl7593_set_voltage(hl7593_VDD2, vdram_prefix(LP4_VDD2));
		else if (g_fan53526_hw_exist[FAN53526_VDD2])
			fan53526_set_voltage(FAN53526_VDD2, vdram_prefix(LP4_VDD2));

		//0.6V
		if (g_rt5738_hw_exist[MT6691_VDDQ])
			rt5738_set_voltage(MT6691_VDDQ, vdram_prefix(LP4X_VDDQ));
		else if (g_hl7593_hw_exist[hl7593_VDDQ])
			hl7593_set_voltage(hl7593_VDDQ, vdram_prefix(LP4X_VDDQ));
		else if (g_fan53526_hw_exist[FAN53526_VDDQ])
			fan53526_set_voltage(FAN53526_VDDQ, vdram_prefix(LP4X_VDDQ));

		if (g_rt5738_hw_exist[MT6691_VDD2])
			rt5738_enable(MT6691_VDD2, 1);
		else if (g_hl7593_hw_exist[hl7593_VDD2])
			hl7593_enable(hl7593_VDD2, 1);
		else if (g_fan53526_hw_exist[FAN53526_VDD2])
			fan53526_enable(FAN53526_VDD2, 1);

		if (g_rt5738_hw_exist[MT6691_VDDQ])
			rt5738_enable(MT6691_VDDQ, 1);
		else if (g_hl7593_hw_exist[hl7593_VDDQ])
			hl7593_enable(hl7593_VDDQ, 1);
		else if (g_fan53526_hw_exist[FAN53526_VDDQ])
			fan53526_enable(FAN53526_VDDQ, 1);
		break;
	case TYPE_PCDDR4:
		rt5738_set_voltage(MA5748_VDD, vdram_prefix(PC4_VDD)); //1.2V
		rt5738_enable(MA5748_VDD, 1);
		break;
	case TYPE_LPDDR3:
		/*default vdam=1.24V*/
		break;
	case TYPE_PCDDR3:
		mtk_regulator_set_voltage(&reg_vdram, 1370*1000, vdram_max_uV);
		break;
	default:
		print("[%s] not support ddr_type=%d\n\n", __func__, p->dram_type);
		break;
	}
	print("[%s]set dram voltage done!!!\n\n", __func__);
#endif

#endif
}

int get_dram_voltage(DRAMC_CTX_T *p, DRAM_VOL_T vol_type)
{
	unsigned long vol = 0;
	struct mtk_regulator reg_vdram;

	mtk_regulator_get("vdram", &reg_vdram);

#if (FOR_DV_SIMULATION_USED == 0) /* cc notes: confirm for REAL CHIP */
#if USE_PMIC_CHIP_MT6357
	switch(p->dram_type) {
	case TYPE_LPDDR4:
		if ((vol_type == DRAM_VOL_VDD2) || (vol_type == DRAM_VOL_VDDQ) || ((vol_type == DRAM_VOL_VMEM))) {
			//1.1V
			if (g_rt5738_hw_exist[MT6691_VDD2])
				vol = rt5738_get_voltage(MT6691_VDD2);
			else if (g_hl7593_hw_exist[hl7593_VDD2])
				vol = hl7593_get_voltage(hl7593_VDD2);
			else if (g_fan53526_hw_exist[FAN53526_VDD2])
				vol = fan53526_get_voltage(FAN53526_VDD2);
		}

		break;
	case TYPE_LPDDR4X:
		if(vol_type == DRAM_VOL_VDD2) {
			//1.1V
			if (g_rt5738_hw_exist[MT6691_VDD2])
				vol = rt5738_get_voltage(MT6691_VDD2);
			else if (g_hl7593_hw_exist[hl7593_VDD2])
				vol = hl7593_get_voltage(hl7593_VDD2);
			else if (g_fan53526_hw_exist[FAN53526_VDD2])
				vol = fan53526_get_voltage(FAN53526_VDD2);
		} else if((vol_type == DRAM_VOL_VDDQ) || (vol_type == DRAM_VOL_VMEM)) {
			//1.1V or 0.6V
			if (g_rt5738_hw_exist[MT6691_VDDQ])
				vol = rt5738_get_voltage(MT6691_VDDQ);
			else if (g_hl7593_hw_exist[hl7593_VDDQ])
				vol = hl7593_get_voltage(hl7593_VDDQ);
			else if (g_fan53526_hw_exist[FAN53526_VDDQ])
				vol = fan53526_get_voltage(FAN53526_VDDQ);
		}

		break;
	case TYPE_PCDDR4:
		if ((vol_type == DRAM_VOL_VDD) || (vol_type == DRAM_VOL_VDDQ) || (vol_type == DRAM_VOL_VMEM))
			vol = rt5738_get_voltage(MA5748_VDD); //1.2V

		break;
	case TYPE_LPDDR3:
	case TYPE_PCDDR3:
		vol = mtk_regulator_get_voltage(&reg_vdram);
		break;
	default:
		print("[%s] not support ddr_type=%d\n", __func__, p->dram_type);
		break;
	}

#endif
#endif

	show_msg((INFO, "[%s]ddr_type=%d, vol_type=%d, vol=%d\n", __func__, p->dram_type, vol_type, vol));
	return vol/1000;
}

//#endif

void switch_dramc_voltage_to_auto_mode(DRAMC_CTX_T *p)
{
#if (FOR_DV_SIMULATION_USED == 0) /* cc notes: confirm for REAL CHIP */
#if USE_PMIC_CHIP_MT6357
    switch(p->dram_type) {
    case TYPE_LPDDR4:
        if (g_rt5738_hw_exist[MT6691_VDD2])
            rt5738_set_mode(MT6691_VDD2, 0);
        else if (g_hl7593_hw_exist[hl7593_VDD2])
            hl7593_set_mode(hl7593_VDD2, 0);
        else if (g_fan53526_hw_exist[FAN53526_VDD2])
            fan53526_set_mode(FAN53526_VDD2, 0);
        break;
    case TYPE_LPDDR4X:
        if (g_rt5738_hw_exist[MT6691_VDD2])
            rt5738_set_mode(MT6691_VDD2, 0);
        else if (g_hl7593_hw_exist[hl7593_VDD2])
            hl7593_set_mode(hl7593_VDD2, 0);
        else if (g_fan53526_hw_exist[FAN53526_VDD2])
            fan53526_set_mode(FAN53526_VDD2, 0);

        if (g_rt5738_hw_exist[MT6691_VDDQ])
            rt5738_set_mode(MT6691_VDDQ, 0);
        else if (g_hl7593_hw_exist[hl7593_VDDQ])
            hl7593_set_mode(hl7593_VDDQ, 0);
        else if (g_fan53526_hw_exist[FAN53526_VDDQ])
            fan53526_set_mode(FAN53526_VDDQ, 0);
        break;
    case TYPE_PCDDR4:
    case TYPE_LPDDR3:
    case TYPE_PCDDR3:
        break;
    default:
        print("[%s] not support ddr_type=%d\n\n", __func__, p->dram_type);
        break;
    }
    print("[%s]switch dram voltage to auto mode done!!!\n\n", __func__);
#endif
#endif
}


