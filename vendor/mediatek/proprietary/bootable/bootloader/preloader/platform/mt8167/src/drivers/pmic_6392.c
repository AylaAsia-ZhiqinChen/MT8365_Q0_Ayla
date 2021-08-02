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
 */

#include <typedefs.h>
#include <platform.h>
#include <pmic_wrap_init.h>
#include <pmic.h>
#include <dramc_pi_api.h>

#define VOLTAGE_FULL_RANGE     1800

static const u32 mt6392_auxadc_regs[] = {
	MT6392_AUXADC_ADC0, MT6392_AUXADC_ADC1, MT6392_AUXADC_ADC2,
	MT6392_AUXADC_ADC3, MT6392_AUXADC_ADC4, MT6392_AUXADC_ADC5,
	MT6392_AUXADC_ADC6, MT6392_AUXADC_ADC7, MT6392_AUXADC_ADC8,
	MT6392_AUXADC_ADC9, MT6392_AUXADC_ADC10, MT6392_AUXADC_ADC11,
	MT6392_AUXADC_ADC12, MT6392_AUXADC_ADC12, MT6392_AUXADC_ADC12,
	MT6392_AUXADC_ADC12,
};

//////////////////////////////////////////////////////////////////////////////////////////
// PMIC access API
//////////////////////////////////////////////////////////////////////////////////////////
U32 pmic_read_interface (U32 RegNum, U32 *val, U32 MASK, U32 SHIFT)
{
	U32 return_value = 0;
	U32 pmic6392_reg = 0;
	U32 rdata = 0;

	//mt6323_read_byte(RegNum, &pmic6323_reg);
	return_value= pwrap_wacs2(0, (RegNum), 0, &rdata);
	pmic6392_reg=rdata;
	if (return_value!=0) {
		print("[pmic_read_interface] Reg[%x]= pmic_wrap read data fail\n", RegNum);
		return return_value;
	}
	//print("[pmic_read_interface] Reg[%x]=0x%x\n", RegNum, pmic6323_reg);

	pmic6392_reg &= (MASK << SHIFT);
	*val = (pmic6392_reg >> SHIFT);
	//print("[pmic_read_interface] val=0x%x\n", *val);

	return return_value;
}

U32 pmic_config_interface (U32 RegNum, U32 val, U32 MASK, U32 SHIFT)
{
	U32 return_value = 0;
	U32 pmic6392_reg = 0;
	U32 rdata = 0;

	if (val > MASK) {
		print("[pmic_config_interface] Invalid data, Reg[%x]: MASK = 0x%x, val = 0x%x\n",
			RegNum, MASK, val);
		return E_PWR_INVALID_DATA;
	}

	//1. mt6323_read_byte(RegNum, &pmic6323_reg);
	return_value= pwrap_wacs2(0, (RegNum), 0, &rdata);
	pmic6392_reg=rdata;
	if (return_value!=0) {
		print("[pmic_config_interface] Reg[%x]= pmic_wrap read data fail\n", RegNum);
		return return_value;
	}
	//print("[pmic_config_interface] Reg[%x]=0x%x\n", RegNum, pmic6323_reg);

	pmic6392_reg &= ~(MASK << SHIFT);
	pmic6392_reg |= (val << SHIFT);

	//2. mt6323_write_byte(RegNum, pmic6323_reg);
	return_value= pwrap_wacs2(1, (RegNum), pmic6392_reg, &rdata);
	if (return_value!=0) {
		print("[pmic_config_interface] Reg[%x]= pmic_wrap read data fail\n", RegNum);
		return return_value;
	}
	//print("[pmic_config_interface] write Reg[%x]=0x%x\n", RegNum, pmic6323_reg);

#if 0
	//3. Double Check
	//mt6323_read_byte(RegNum, &pmic6323_reg);
	return_value= pwrap_wacs2(0, (RegNum), 0, &rdata);
	pmic6323_reg=rdata;
	if (return_value!=0) {
		print("[pmic_config_interface] Reg[%x]= pmic_wrap write data fail\n", RegNum);
		return return_value;
	}
	print("[pmic_config_interface] Reg[%x]=0x%x\n", RegNum, pmic6323_reg);
#endif

	return return_value;
}

//////////////////////////////////////////////////////////////////////////////////////////
// PMIC-Charger Type Detection
//////////////////////////////////////////////////////////////////////////////////////////
CHARGER_TYPE g_ret = CHARGER_UNKNOWN;
int g_charger_in_flag = 0;
int g_first_check=0;

extern void Charger_Detect_Init(void);
extern void Charger_Detect_Release(void);

void pmic_lock(void)
{
}

void pmic_unlock(void)
{
}

U32 upmu_get_reg_value(kal_uint32 reg)
{
	U32 ret=0;
	U32 reg_val=0;

	ret=pmic_read_interface(reg, &reg_val, 0xFFFF, 0x0);

	return reg_val;
}

void upmu_set_rg_bc11_bb_ctrl(kal_uint32 val)
{
	kal_uint32 ret=0;

	pmic_lock();
	ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
	                           (kal_uint32)(val),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_BB_CTRL_MASK),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_BB_CTRL_SHIFT)
	                         );
	pmic_unlock();
}

void upmu_set_rg_bc11_rst(kal_uint32 val)
{
	kal_uint32 ret=0;

	pmic_lock();
	ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
	                           (kal_uint32)(val),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_RST_MASK),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_RST_SHIFT)
	                         );
	pmic_unlock();
}

void upmu_set_rg_bc11_vsrc_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	pmic_lock();
	ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON18),
	                           (kal_uint32)(val),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_VSRC_EN_MASK),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_VSRC_EN_SHIFT)
	                         );
	pmic_unlock();
}

kal_uint32 upmu_get_rgs_bc11_cmp_out(void)
{
	kal_uint32 ret=0;
	kal_uint32 val=0;

	pmic_lock();
	ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON18),
	                         (&val),
	                         (kal_uint32)(MT6392_PMIC_RGS_BC11_CMP_OUT_MASK),
	                         (kal_uint32)(MT6392_PMIC_RGS_BC11_CMP_OUT_SHIFT)
	                       );
	pmic_unlock();

	return val;
}

void upmu_set_rg_bc11_vref_vth(kal_uint32 val)
{
	kal_uint32 ret=0;

	pmic_lock();
	ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
	                           (kal_uint32)(val),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_VREF_VTH_MASK),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_VREF_VTH_SHIFT)
	                         );
	pmic_unlock();
}

void upmu_set_rg_bc11_cmp_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	pmic_lock();
	ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
	                           (kal_uint32)(val),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_CMP_EN_MASK),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_CMP_EN_SHIFT)
	                         );
	pmic_unlock();
}

void upmu_set_rg_bc11_ipd_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	pmic_lock();
	ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
	                           (kal_uint32)(val),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_IPD_EN_MASK),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_IPD_EN_SHIFT)
	                         );
	pmic_unlock();
}

void upmu_set_rg_bc11_ipu_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	pmic_lock();
	ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
	                           (kal_uint32)(val),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_IPU_EN_MASK),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_IPU_EN_SHIFT)
	                         );
	pmic_unlock();
}

void upmu_set_rg_bc11_bias_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	pmic_lock();
	ret=pmic_config_interface( (kal_uint32)(MT6392_CHR_CON19),
	                           (kal_uint32)(val),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_BIAS_EN_MASK),
	                           (kal_uint32)(MT6392_PMIC_RG_BC11_BIAS_EN_SHIFT)
	                         );
	pmic_unlock();
}


void hw_bc11_init(void)
{
	Charger_Detect_Init();

	//RG_BC11_BIAS_EN=1
	upmu_set_rg_bc11_bias_en(0x1);
	//RG_BC11_VSRC_EN[1:0]=00
	upmu_set_rg_bc11_vsrc_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_IPU_EN[1.0] = 00
	upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_IPD_EN[1.0] = 00
	upmu_set_rg_bc11_ipd_en(0x0);
	//BC11_RST=1
	upmu_set_rg_bc11_rst(0x1);
	//BC11_BB_CTRL=1
	upmu_set_rg_bc11_bb_ctrl(0x1);

	mdelay(100);

}


U32 hw_bc11_DCD(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_IPU_EN[1.0] = 10
	upmu_set_rg_bc11_ipu_en(0x2);
	//RG_BC11_IPD_EN[1.0] = 01
	upmu_set_rg_bc11_ipd_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=01
	upmu_set_rg_bc11_vref_vth(0x1);
	//RG_BC11_CMP_EN[1.0] = 10
	upmu_set_rg_bc11_cmp_en(0x2);

	mdelay(400);

	wChargerAvail = upmu_get_rgs_bc11_cmp_out();

	//RG_BC11_IPU_EN[1.0] = 00
	upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_IPD_EN[1.0] = 00
	upmu_set_rg_bc11_ipd_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	upmu_set_rg_bc11_vref_vth(0x0);

	return wChargerAvail;
}


U32 hw_bc11_stepA1(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_IPU_EN[1.0] = 10
	upmu_set_rg_bc11_ipu_en(0x2);
	//RG_BC11_VREF_VTH = [1:0]=10
	upmu_set_rg_bc11_vref_vth(0x2);
	//RG_BC11_CMP_EN[1.0] = 10
	upmu_set_rg_bc11_cmp_en(0x2);

	mdelay(80);

	wChargerAvail = upmu_get_rgs_bc11_cmp_out();

	//RG_BC11_IPU_EN[1.0] = 00
	upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);

	return  wChargerAvail;
}


U32 hw_bc11_stepB1(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_IPU_EN[1.0] = 01
	//upmu_set_rg_bc11_ipu_en(0x1);
	upmu_set_rg_bc11_ipd_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=10
	//upmu_set_rg_bc11_vref_vth(0x2);
	upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 01
	upmu_set_rg_bc11_cmp_en(0x1);

	mdelay(80);

	wChargerAvail = upmu_get_rgs_bc11_cmp_out();

	//RG_BC11_IPU_EN[1.0] = 00
	upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	upmu_set_rg_bc11_vref_vth(0x0);

	return  wChargerAvail;
}


U32 hw_bc11_stepC1(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_IPU_EN[1.0] = 01
	upmu_set_rg_bc11_ipu_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=10
	upmu_set_rg_bc11_vref_vth(0x2);
	//RG_BC11_CMP_EN[1.0] = 01
	upmu_set_rg_bc11_cmp_en(0x1);

	mdelay(80);

	wChargerAvail = upmu_get_rgs_bc11_cmp_out();

	//RG_BC11_IPU_EN[1.0] = 00
	upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	upmu_set_rg_bc11_vref_vth(0x0);

	return  wChargerAvail;
}


U32 hw_bc11_stepA2(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_VSRC_EN[1.0] = 10
	upmu_set_rg_bc11_vsrc_en(0x2);
	//RG_BC11_IPD_EN[1:0] = 01
	upmu_set_rg_bc11_ipd_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=00
	upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 01
	upmu_set_rg_bc11_cmp_en(0x1);

	mdelay(80);

	wChargerAvail = upmu_get_rgs_bc11_cmp_out();

	//RG_BC11_VSRC_EN[1:0]=00
	upmu_set_rg_bc11_vsrc_en(0x0);
	//RG_BC11_IPD_EN[1.0] = 00
	upmu_set_rg_bc11_ipd_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);

	return  wChargerAvail;
}


U32 hw_bc11_stepB2(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_IPU_EN[1:0]=10
	upmu_set_rg_bc11_ipu_en(0x2);
	//RG_BC11_VREF_VTH = [1:0]=01
	upmu_set_rg_bc11_vref_vth(0x1);
	//RG_BC11_CMP_EN[1.0] = 01
	upmu_set_rg_bc11_cmp_en(0x1);

	mdelay(80);

	wChargerAvail = upmu_get_rgs_bc11_cmp_out();

	//RG_BC11_IPU_EN[1.0] = 00
	upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	upmu_set_rg_bc11_vref_vth(0x0);

	return  wChargerAvail;
}


void hw_bc11_done(void)
{
	//RG_BC11_VSRC_EN[1:0]=00
	upmu_set_rg_bc11_vsrc_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=0
	upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_IPU_EN[1.0] = 00
	upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_IPD_EN[1.0] = 00
	upmu_set_rg_bc11_ipd_en(0x0);
	//RG_BC11_BIAS_EN=0
	upmu_set_rg_bc11_bias_en(0x0);

	Charger_Detect_Release();
}

CHARGER_TYPE hw_charger_type_detection(void)
{
	CHARGER_TYPE charger_tye;
#if defined(CONFIG_POWER_EXT)
	charger_tye = STANDARD_HOST;
#else

	/********* Step initial  ***************/
	hw_bc11_init();

	/********* Step DCD ***************/
	if (1 == hw_bc11_DCD()) {
		/********* Step A1 ***************/
		if (1 == hw_bc11_stepA1()) {
			/********* Step B1 ***************/
			if (1 == hw_bc11_stepB1()) {
				//charger_tye = NONSTANDARD_CHARGER;
				//printf("step B1 : Non STANDARD CHARGER!\r\n");
				charger_tye = APPLE_2_1A_CHARGER;
				printf("step B1 : Apple 2.1A CHARGER!\r\n");
			} else {
				//charger_tye = APPLE_2_1A_CHARGER;
				//printf("step B1 : Apple 2.1A CHARGER!\r\n");
				charger_tye = NONSTANDARD_CHARGER;
				printf("step B1 : Non STANDARD CHARGER!\r\n");
			}
		} else {
			/********* Step C1 ***************/
			if (1 == hw_bc11_stepC1()) {
				charger_tye = APPLE_1_0A_CHARGER;
				printf("step C1 : Apple 1A CHARGER!\r\n");
			} else {
				charger_tye = APPLE_0_5A_CHARGER;
				printf("step C1 : Apple 0.5A CHARGER!\r\n");
			}
		}

	} else {
		/********* Step A2 ***************/
		if (1 == hw_bc11_stepA2()) {
			/********* Step B2 ***************/
			if (1 == hw_bc11_stepB2()) {
				charger_tye = STANDARD_CHARGER;
				printf("step B2 : STANDARD CHARGER!\r\n");
			} else {
				charger_tye = CHARGING_HOST;
				printf("step B2 :  Charging Host!\r\n");
			}
		} else {
			charger_tye = STANDARD_HOST;
			printf("step A2 : Standard USB Host!\r\n");
		}

	}

	/********* Finally setting *******************************/
	hw_bc11_done();
#endif
	return charger_tye;
}

CHARGER_TYPE mt_charger_type_detection(void)
{
	if ( g_first_check == 0 ) {
		g_first_check = 1;
		g_ret = hw_charger_type_detection();
	} else {
		printf("[mt_charger_type_detection] Got data !!, %d, %d\r\n", g_charger_in_flag, g_first_check);
	}

	return g_ret;
}

//==============================================================================
// PMIC6392 Usage APIs
//==============================================================================
U32 mt6392_upmu_get_cid (void)
{
	kal_uint32 ret=0;
	kal_uint32 val=0;

	ret=pmic_read_interface( (kal_uint32)(MT6392_CID),
	                         (&val),
	                         (kal_uint32)(MT6392_PMIC_CID_MASK),
	                         (kal_uint32)(MT6392_PMIC_CID_SHIFT)
	                       );

	return val;
}

int pmic_detect_powerkey(void)
{
	U32 ret=0;
	U32 val=0;

	ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
	                         (&val),
	                         (kal_uint32)(MT6392_PMIC_PWRKEY_DEB_MASK),
	                         (kal_uint32)(MT6392_PMIC_PWRKEY_DEB_SHIFT)
	                       );

	if (val==1) {
		printf("pl pmic powerkey Release\n");
		return 0;
	} else {
		printf("pl pmic powerkey Press\n");
		return 1;
	}
}

int pmic_detect_homekey(void)
{
	U32 ret=0;
	U32 val=0;

	ret=pmic_read_interface( (kal_uint32)(MT6392_CHRSTATUS),
	                         (&val),
	                         (kal_uint32)(MT6392_PMIC_FCHRKEY_DEB_MASK),
	                         (kal_uint32)(MT6392_PMIC_FCHRKEY_DEB_SHIFT)
	                       );

	if (val==1) {
		printf("pl pmic FCHRKEY Release\n");
		return 0;
	} else {
		printf("pl pmic FCHRKEY Press\n");
		return 1;
	}
}

U32 pmic_IsUsbCableIn (void)
{
	U32 ret=0;
	U32 val=0;

	ret=pmic_read_interface( (kal_uint32)(MT6392_CHR_CON0),
	                         (&val),
	                         (kal_uint32)(MT6392_PMIC_RGS_CHRDET_MASK),
	                         (kal_uint32)(MT6392_PMIC_RGS_CHRDET_SHIFT)
	                       );

	if (val)
		return PMIC_CHRDET_EXIST;
	else
		return PMIC_CHRDET_NOT_EXIST;
}

static int vbat_status = PMIC_VBAT_NOT_DROP;
int pmic_IsVbatDrop(void)
{
	return vbat_status;
}

void hw_set_cc(int cc_val)
{
	U32 ret_val=0;
	U32 reg_val=0;
	U32 i=0;
	U32 hw_charger_ov_flag=0;

	printf("hw_set_cc: %d\r\n", cc_val);

	//VCDT_HV_VTH, 7V
	ret_val=pmic_config_interface(MT6392_CHR_CON1, 0x0B, MT6392_PMIC_RG_VCDT_HV_VTH_MASK, MT6392_PMIC_RG_VCDT_HV_VTH_SHIFT);
	//VCDT_HV_EN=1
	ret_val=pmic_config_interface(MT6392_CHR_CON0, 0x01, MT6392_PMIC_RG_VCDT_HV_EN_MASK, MT6392_PMIC_RG_VCDT_HV_EN_SHIFT);
	//CS_EN=1
	ret_val=pmic_config_interface(MT6392_CHR_CON2, 0x01, MT6392_PMIC_RG_CS_EN_MASK, MT6392_PMIC_RG_CS_EN_SHIFT);
	//CSDAC_MODE=1
	ret_val=pmic_config_interface(MT6392_CHR_CON23, 0x01, MT6392_PMIC_RG_CSDAC_MODE_MASK, MT6392_PMIC_RG_CSDAC_MODE_SHIFT);

	ret_val=pmic_read_interface(MT6392_CHR_CON0, &hw_charger_ov_flag, MT6392_PMIC_RGS_VCDT_HV_DET_MASK, MT6392_PMIC_RGS_VCDT_HV_DET_SHIFT);
	if (hw_charger_ov_flag == 1) {
		ret_val=pmic_config_interface(MT6392_CHR_CON0, 0x00, MT6392_PMIC_RG_CHR_EN_MASK, MT6392_PMIC_RG_CHR_EN_SHIFT);
		printf("pl chargerov turn off charging \n");
		return;
	}

	// CS_VTH
	switch (cc_val) {
		case 1600:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x00, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 1500:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x01, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 1400:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x02, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 1300:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x03, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 1200:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x04, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 1100:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x05, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 1000:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x06, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 900:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x07, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 800:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x08, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 700:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x09, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 650:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x0A, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 550:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x0B, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 450:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x0C, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 300:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x0D, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 200:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x0E, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		case 70:
			ret_val=pmic_config_interface(MT6392_CHR_CON4, 0x0F, MT6392_PMIC_RG_CS_VTH_MASK, MT6392_PMIC_RG_CS_VTH_SHIFT);
			break;
		default:
			dbg_print("hw_set_cc: argument invalid!!\r\n");
			break;
	}

	//upmu_chr_csdac_dly(0x4);                // CSDAC_DLY
	ret_val=pmic_config_interface(MT6392_CHR_CON21, 0x04, MT6392_PMIC_RG_CSDAC_DLY_MASK, MT6392_PMIC_RG_CSDAC_DLY_SHIFT);
	//upmu_chr_csdac_stp(0x1);                // CSDAC_STP
	ret_val=pmic_config_interface(MT6392_CHR_CON21, 0x01, MT6392_PMIC_RG_CSDAC_STP_MASK, MT6392_PMIC_RG_CSDAC_STP_SHIFT);
	//upmu_chr_csdac_stp_inc(0x1);            // CSDAC_STP_INC
	ret_val=pmic_config_interface(MT6392_CHR_CON20, 0x01, MT6392_PMIC_RG_CSDAC_STP_INC_MASK, MT6392_PMIC_RG_CSDAC_STP_INC_SHIFT);
	//upmu_chr_csdac_stp_dec(0x2);            // CSDAC_STP_DEC
	ret_val=pmic_config_interface(MT6392_CHR_CON20, 0x02, MT6392_PMIC_RG_CSDAC_STP_DEC_MASK, MT6392_PMIC_RG_CSDAC_STP_DEC_SHIFT);
	//upmu_chr_chrwdt_td(0x0);                // CHRWDT_TD, 4s
	ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x00, MT6392_PMIC_RG_CHRWDT_TD_MASK, MT6392_PMIC_RG_CHRWDT_TD_SHIFT);
	//upmu_set_rg_chrwdt_wr(1);             // CHRWDT_FLAG_WR
	ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x01, MT6392_PMIC_RG_CHRWDT_WR_MASK, MT6392_PMIC_RG_CHRWDT_WR_SHIFT);
	//upmu_chr_chrwdt_int_en(1);              // CHRWDT_INT_EN
	ret_val=pmic_config_interface(MT6392_CHR_CON15, 0x01, MT6392_PMIC_RG_CHRWDT_INT_EN_MASK, MT6392_PMIC_RG_CHRWDT_INT_EN_SHIFT);
	//upmu_chr_chrwdt_en(1);                  // CHRWDT_EN
	ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x01, MT6392_PMIC_RG_CHRWDT_EN_MASK, MT6392_PMIC_RG_CHRWDT_EN_SHIFT);
	//upmu_chr_chrwdt_flag_wr(1);             // CHRWDT_FLAG_WR
	ret_val=pmic_config_interface(MT6392_CHR_CON15, 0x01, MT6392_PMIC_RG_CHRWDT_FLAG_WR_MASK, MT6392_PMIC_RG_CHRWDT_FLAG_WR_SHIFT);
	//upmu_chr_csdac_enable(1);               // CSDAC_EN
	ret_val=pmic_config_interface(MT6392_CHR_CON0, 0x01, MT6392_PMIC_RG_CSDAC_EN_MASK, MT6392_PMIC_RG_CSDAC_EN_SHIFT);
	//upmu_set_rg_hwcv_en(1);                 // HWCV_EN
	ret_val=pmic_config_interface(MT6392_CHR_CON23, 0x01, MT6392_PMIC_RG_HWCV_EN_MASK, MT6392_PMIC_RG_HWCV_EN_SHIFT);
	//upmu_chr_enable(1);                     // CHR_EN
	ret_val=pmic_config_interface(MT6392_CHR_CON0, 0x01, MT6392_PMIC_RG_CHR_EN_MASK, MT6392_PMIC_RG_CHR_EN_SHIFT);

	for (i=MT6392_CHR_CON0 ; i<=MT6392_CHR_CON29 ; i++) {
		ret_val=pmic_read_interface(i,&reg_val,0xFFFF,0x0);
		print("[0x%x]=0x%x\n", i, reg_val);
	}

	printf("hw_set_cc: done\r\n");
}

void pl_hw_ulc_det(void)
{
	U32 ret_val=0;

	//upmu_chr_ulc_det_en(1);            // RG_ULC_DET_EN=1
	ret_val=pmic_config_interface(MT6392_CHR_CON23, 0x01, MT6392_PMIC_RG_ULC_DET_EN_MASK, MT6392_PMIC_RG_ULC_DET_EN_SHIFT);
	//upmu_chr_low_ich_db(1);            // RG_LOW_ICH_DB=000001'b
	ret_val=pmic_config_interface(MT6392_CHR_CON22, 0x01, MT6392_PMIC_RG_LOW_ICH_DB_MASK, MT6392_PMIC_RG_LOW_ICH_DB_SHIFT);
}

int hw_check_battery(void)
{
#ifndef MTK_DISABLE_POWER_ON_OFF_VOLTAGE_LIMITATION
	U32 ret_val;
	U32 reg_val=0;

	ret_val=pmic_config_interface(MT6392_CHR_CON7,0x01, MT6392_PMIC_RG_BATON_EN_MASK, MT6392_PMIC_RG_BATON_EN_SHIFT);      //BATON_EN=1
	ret_val=pmic_config_interface(MT6392_CHR_CON7,    0x01, MT6392_PMIC_BATON_TDET_EN_MASK, MT6392_PMIC_BATON_TDET_EN_SHIFT);  //BATON_TDET_EN=1
	ret_val=pmic_read_interface(MT6392_CHR_CON7,&reg_val, MT6392_PMIC_RGS_BATON_UNDET_MASK, MT6392_PMIC_RGS_BATON_UNDET_SHIFT);

	if (reg_val == 1) {
		printf("No Battery\n");

		ret_val=pmic_read_interface(MT6392_CHR_CON7,&reg_val,0xFFFF,0x0);
		print("[0x%x]=0x%x\n",MT6392_CHR_CON7,reg_val);

		return 0;
	} else {
		printf("Battery exist\n");

		ret_val=pmic_read_interface(MT6392_CHR_CON7,&reg_val,0xFF,0x0);
		print("[0x%x]=0x%x\n",MT6392_CHR_CON7,reg_val);

		pl_hw_ulc_det();

		return 1;
	}
#else
	return 1;
#endif
}

void pl_charging(int en_chr)
{
	U32 ret_val=0;
	U32 reg_val=0;
	U32 i=0;

	if (en_chr == 1) {
		printf("pl charging en\n");

		hw_set_cc(450);

		//USBDL set 1
		ret_val=pmic_config_interface(MT6392_CHR_CON16, 0x01, MT6392_PMIC_RG_USBDL_SET_MASK, MT6392_PMIC_RG_USBDL_SET_SHIFT);
	} else {
		printf("pl charging dis\n");

		//USBDL set 0
		ret_val=pmic_config_interface(MT6392_CHR_CON16, 0x00, MT6392_PMIC_RG_USBDL_SET_MASK, MT6392_PMIC_RG_USBDL_SET_SHIFT);

		//upmu_set_rg_hwcv_en(0); // HWCV_EN
		ret_val=pmic_config_interface(MT6392_CHR_CON23, 0x00, MT6392_PMIC_RG_HWCV_EN_MASK, MT6392_PMIC_RG_HWCV_EN_SHIFT);
		//upmu_chr_enable(0); // CHR_EN
		ret_val=pmic_config_interface(MT6392_CHR_CON0, 0x00, MT6392_PMIC_RG_CHR_EN_MASK, MT6392_PMIC_RG_CHR_EN_SHIFT);
	}

	for (i=MT6392_CHR_CON0 ; i<=MT6392_CHR_CON29 ; i++) {
		ret_val=pmic_read_interface(i,&reg_val,0xFFFF,0x0);
		print("[0x%x]=0x%x\n", i, reg_val);
	}

	printf("pl charging done\n");
}

void pl_kick_chr_wdt(void)
{
	int ret_val=0;

	//upmu_chr_chrwdt_td(0x0);                // CHRWDT_TD
	ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x03, MT6392_PMIC_RG_CHRWDT_TD_MASK, MT6392_PMIC_RG_CHRWDT_TD_SHIFT);
	//upmu_set_rg_chrwdt_wr(1);            // CHRWDT_FLAG
	ret_val=pmic_config_interface(MT6392_CHR_CON15, 0x01, MT6392_PMIC_RG_CHRWDT_WR_MASK, MT6392_PMIC_RG_CHRWDT_WR_SHIFT);
	//upmu_chr_chrwdt_int_en(1);             // CHRWDT_INT_EN
	ret_val=pmic_config_interface(MT6392_CHR_CON15, 0x01, MT6392_PMIC_RG_CHRWDT_INT_EN_MASK, MT6392_PMIC_RG_CHRWDT_INT_EN_SHIFT);
	//upmu_chr_chrwdt_en(1);                   // CHRWDT_EN
	ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x01, MT6392_PMIC_RG_CHRWDT_EN_MASK, MT6392_PMIC_RG_CHRWDT_EN_SHIFT);
	//upmu_chr_chrwdt_flag_wr(1);            // CHRWDT_FLAG_WR
	ret_val=pmic_config_interface(MT6392_CHR_CON15, 0x01, MT6392_PMIC_RG_CHRWDT_FLAG_WR_MASK, MT6392_PMIC_RG_CHRWDT_FLAG_WR_SHIFT);

	//printf("[pl_kick_chr_wdt] done\n");
}

void pl_close_pre_chr_led(void)
{
	U32 ret_val=0;

	ret_val=pmic_config_interface(MT6392_CHR_CON22, 0x00, MT6392_PMIC_RG_CHRIND_ON_MASK, MT6392_PMIC_RG_CHRIND_ON_SHIFT);

	printf("pl pmic close pre-chr LED\n");
}

void usbdl_wo_battery_forced(int en_chr)
{
	U32 ret_val=0;

	if (en_chr == 1) {
		printf("chr force en\n");

		//USBDL set 1
		ret_val=pmic_config_interface(MT6392_CHR_CON16, 0x01, MT6392_PMIC_RG_USBDL_SET_MASK, MT6392_PMIC_RG_USBDL_SET_SHIFT);
		// CHRWDT_TD, 4s
		ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x00, MT6392_PMIC_RG_CHRWDT_TD_MASK, MT6392_PMIC_RG_CHRWDT_TD_SHIFT);
		// CHRWDT_FLAG_WR
		ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x01, MT6392_PMIC_RG_CHRWDT_WR_MASK, MT6392_PMIC_RG_CHRWDT_WR_SHIFT);
		// CHRWDT_EN
		ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x01, MT6392_PMIC_RG_CHRWDT_EN_MASK, MT6392_PMIC_RG_CHRWDT_EN_SHIFT);
	} else {
		printf("chr force dis\n");

		//USBDL set 0
		ret_val=pmic_config_interface(MT6392_CHR_CON16, 0x00, MT6392_PMIC_RG_USBDL_SET_MASK, MT6392_PMIC_RG_USBDL_SET_SHIFT);
		ret_val=pmic_config_interface(MT6392_CHR_CON16, 0x01, MT6392_PMIC_RG_USBDL_RST_MASK, MT6392_PMIC_RG_USBDL_RST_SHIFT);
		// CHRWDT_FLAG_WR
		ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x01, MT6392_PMIC_RG_CHRWDT_WR_MASK, MT6392_PMIC_RG_CHRWDT_WR_SHIFT);
		// CHRWDT_EN
		ret_val=pmic_config_interface(MT6392_CHR_CON13, 0x00, MT6392_PMIC_RG_CHRWDT_EN_MASK, MT6392_PMIC_RG_CHRWDT_EN_SHIFT);
	}
}


/**
 * PMIC_IMM_GetOneChannelValue() - Get voltage from a auxadc channel
 *
 * @dwChannel: Auxadc channel to choose
 *             0 : BATSNS   2.5~4.5V    ~1.668ms data ready
 *             0 : ISENSE   2.5~4.5V     ~1.668ms data ready
 *             2 : VCDT     0~1.5V  ~0.108ms data ready
 *             3 : BATON    0.1~1.8V    ~0.108ms data ready
 *             4 : PMIC_TEMP        ~0.108ms data ready
 *             6 : TYPE-C   0~3.3V  ~0.108ms data ready
 *             8 : DP/DM    0~3.6V  ~0.108ms data ready
 *             9-11 :
 * @deCount: Count time to be averaged
 * @trimd: Auxadc value from trimmed register or not.
 *         mt6392 auxadc has no trimmed register, reserve it to align with mt6397
 *
 * This returns the current voltage of the specified channel in mV,
 * zero for not supported channel, a negative number on error.
 */
int PMIC_IMM_GetOneChannelValue(int dwChannel, int deCount, int trimd)
{
	int ret, adc_rdy;
	int raw_data, adc_result;
	int raw_mask, r_val_temp;
	int adc_div = 4096;
	int count = 0;
	int adc_channel;
	int reg_val = 0;

	adc_channel = dwChannel;
#if defined PMIC_DVT_TC_EN
	/* only used for PMIC_DVT */

	/*STRUP_AUXADC_START SW Control*/
	ret = pmic_config_interface(MT6392_STRUP_CON10,0x1,0x1,6);
	if (ret < 0)
		return ret;

	/* STRUP_AUXADC_RSTB SW path */
	ret = pmic_config_interface(MT6392_STRUP_CON10,0x1,0x1,5);
	if (ret < 0)
		return ret;

	/* STRUP_AUXADC_RSTB SW control */
	ret = pmic_config_interface(MT6392_STRUP_CON10,0x1,0x1,7);
	if (ret < 0)
		return ret;
#endif

	/*defined channel 5 for TYEPC_CC1, channel 6 for TYPEC_CC2*/
	/*defined channel 7 for CHG_DP, channel 8 for CHG_DM*/
	if ((dwChannel == 5) || (dwChannel == 6)) {
		ret = pmic_config_interface(MT6392_TYPE_C_CTRL,0x0,0xffff,0);
		if (ret < 0) {
			return ret;
		}

		ret = pmic_config_interface(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0,0x5B8,0xffff,0);
		if (ret < 0) {
			return ret;
		}

		ret = pmic_config_interface(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0,0x0100,0xffff,0);
		if (ret < 0) {
			return ret;
		}
		udelay(1000);

		if (dwChannel == 5) {
			ret = pmic_config_interface(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0,0x1f40,0xffff,0);
			if (ret < 0) {
				return ret;
			}
			/* AUXADC_RQST0_SET is bit 6*/
			adc_channel = 6;
		} else {
			ret = pmic_config_interface(MT6392_TYPE_C_CC_SW_FORCE_MODE_VAL_0,0x1fc0,0xffff,0);
			if (ret < 0) {
				return ret;
			}
		}
		ret = pmic_config_interface(MT6392_TYPE_C_CC_SW_FORCE_MODE_ENABLE_0,0x05fc,0xffff,0);
		if (ret < 0) {
			return ret;
		}
		mdelay(10);

	}

	if ((dwChannel == 7) || (dwChannel == 8)) {
		if (dwChannel == 7) {
			/*DP channel*/
			ret = pmic_config_interface(MT6392_CHR_CON18,0x0700,0x0f00,0);
			if (ret < 0) {
				return ret;
			}
			/* AUXADC_RQST0_SET is bit 8*/
			adc_channel = 8;
		} else {
			/*DM channel*/
			ret = pmic_config_interface(MT6392_CHR_CON18,0x0b00,0x0f00,0);
			if (ret < 0) {
				return ret;
			}
		}
	}

	/* AUXADC_RQST0 SET  */
	ret = pmic_config_interface(MT6392_AUXADC_RQST0_SET,0x1,0xffff,adc_channel);
	if (ret < 0) {
		return ret;
	}

	/* the spec is 10us */
	if ((adc_channel == 0) || (adc_channel == 1))
		udelay(1000);
	else
		udelay(100);

	/* check auxadc is ready */
	do {
		ret = pmic_read_interface(mt6392_auxadc_regs[adc_channel], &reg_val, 0xffff, 0);
		if (ret < 0) {
			return ret;
		}
		udelay(100);
		adc_rdy = (reg_val >> 15) & 1;
		if (adc_rdy != 0)
			break;
		count++;
	} while (count < deCount);

	if (adc_rdy != 1) {
		printf("PMIC_GetOneChannelValue adc get ready Fail\n");
		return -KAL_FALSE;
	}

	/* get the raw data and calculate the adc result of adc */
	ret = pmic_read_interface(mt6392_auxadc_regs[adc_channel], &reg_val, 0xffff, 0);
	if (ret < 0) {
		return ret;
	}
	printf("MT6392_AUXADC_CON1[14:0] = 0x%x \n", reg_val&0x7fff);

	switch (adc_channel) {
		case 0:
		case 1:
			r_val_temp = 3;
			raw_mask = 0x7fff;
			adc_div = 32768;
			break;

		case 2:
		case 3:
		case 4:
			r_val_temp = 1;
			raw_mask = 0xfff;
			adc_div = 4096;
			break;

		case 5:
		case 6:
		case 7:
		case 8:
			r_val_temp = 2;
			raw_mask = 0xfff;
			adc_div = 4096;
			break;

		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			r_val_temp = 1;
			raw_mask = 0xfff;
			adc_div = 4096;
			break;
	}

	/* get auxadc raw data */
	raw_data = reg_val & raw_mask;

	/* get auxadc real result*/
	adc_result = (raw_data * r_val_temp * VOLTAGE_FULL_RANGE) / adc_div;

	return adc_result;
}

static int auxadc_hw_init(void)
{
	int ret;

	print("[auxadc_hw_init] Preloader Start..................\n");

	/* disable CK_AON */
	ret = pmic_config_interface(MT6392_AUXADC_CON0,0x0,0x1,15);
	if (ret < 0)
		return ret;

	/* disable 12M_CK_AON */
	ret = pmic_config_interface(MT6392_AUXADC_CON0,0x0,0x1,14);
	if (ret < 0)
		return ret;

	/* STRUP_AUXADC_RSTB SW path */
	ret = pmic_config_interface(MT6392_STRUP_CON10,0x1,0x1,5);
	if (ret < 0)
		return ret;

	/* STRUP_AUXADC_RSTB SW control */
	ret = pmic_config_interface(MT6392_STRUP_CON10,0x1,0x1,7);
	if (ret < 0)
		return ret;

	/* AVG_NUM_LARGE = 128 samples  */
	ret = pmic_config_interface(MT6392_AUXADC_CON1,0x6,0x7,3);
	if (ret < 0)
		return ret;

	/* AVG_NUM_SMALL= 8 samples */
	ret = pmic_config_interface(MT6392_AUXADC_CON1,0x2,0x7,0);
	if (ret < 0)
		return ret;

	/* CH0-CH1 is 1, other is 0 */
	ret = pmic_config_interface(MT6392_AUXADC_CON2,0x3,0xfff,0);
	if (ret < 0)
		return ret;

	/* VBUS_EN enable */
	ret = pmic_config_interface(MT6392_AUXADC_CON10,0x1,0x1,9);
	if (ret < 0)
		return ret;

	/* disable date resue */
	ret = pmic_config_interface(MT6392_AUXADC_CON10, 0x3, 0x3, 1);
	if (ret < 0)
		return ret;

	return 0;
}

void PMIC_INIT_SETTING_V1(void)
{
	unsigned int ret = 0;

	/* put init setting from DE/SA */
	ret = pmic_config_interface(0xE,0x1,0x1,2); // [2:2]: BATON_TDET_EN; 7/18:Jyun-Jia
	ret = pmic_config_interface(0x3C,0x1,0x1,5); // [5:5]: THR_HWPDN_EN; 6/30:kim
	ret = pmic_config_interface(0x40,0x1,0x1,2); // [2:2]: RG_FCHR_PU_EN; 7/6:kim
	ret = pmic_config_interface(0x40,0x1,0x1,4); // [4:4]: RG_EN_DRVSEL; 6/30:kim
	ret = pmic_config_interface(0x40,0x1,0x1,5); // [5:5]: RG_RST_DRVSEL; 6/30:kim
	ret = pmic_config_interface(0x50,0x1,0x1,1); // [1:1]: STRUP_PWROFF_PREOFF_EN;
	ret = pmic_config_interface(0x50,0x1,0x1,0); // [0:0]: STRUP_PWROFF_SEQ_EN;
	ret = pmic_config_interface(0x6E,0x1,0x1,0); // [0:0]: VADC18_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,1); // [1:1]: VCORE_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,2); // [2:2]: VPROC_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,3); // [3:3]: VSYS_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,4); // [4:4]: VIO18_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,5); // [5:5]: VIO28_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,6); // [6:6]: VGP2_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,7); // [7:7]: VEMC33_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,8); // [8:8]: VM25_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,9); // [9:9]: VM_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,10); // [10:10]: VUSB_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,11); // [11:11]: VMC_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,12); // [12:12]: VMCH_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x6E,0x1,0x1,13); // [13:13]: VXO22_PG_H2L_EN; 6/30:kim
	ret = pmic_config_interface(0x102,0x1,0x1,1); // [1:1]: RG_CLKSQ_EN; 8/9 disable clk26m in low power scenario
	ret = pmic_config_interface(0x102,0x1,0x1,6); // [6:6]: RG_RTC_75K_CK_PDN; 7/6 Huiyan
	ret = pmic_config_interface(0x102,0x1,0x1,11); // [11:11]: RG_TRIM_75K_CK_PDN; 7/6 Huiyan
	ret = pmic_config_interface(0x102,0x1,0x1,15); // [15:15]: RG_BUCK32K_PDN; 7/6 Huiyan
	ret = pmic_config_interface(0x108,0x1,0x1,9); // [9:9]: RG_RTCDET_CK_PDN; 7/6 Huiyan
	ret = pmic_config_interface(0x108,0x1,0x1,12); // [12:12]: RG_EFUSE_CK_PDN; 7/6 Huiyan
	ret = pmic_config_interface(0x11A,0x1,0x1,1); // [1:1]: RG_SYSRSTB_EN; 7/15:KIM
	ret = pmic_config_interface(0x11A,0x1,0x1,5); // [5:5]: RG_HOMEKEY_RST_EN; 7/6:kim
	ret = pmic_config_interface(0x120,0x1,0x1,4); // [4:4]: RG_SRCLKEN_HW_MODE; 7/6 Huiyan
	ret = pmic_config_interface(0x120,0x1,0x1,5); // [5:5]: RG_OSC_HW_MODE; 7/6 Huiyan
	ret = pmic_config_interface(0x136,0x1,0x1,0); // [0:0]: RG_WDTRSTB_DEB; 7/15:KIM
	ret = pmic_config_interface(0x148,0x1,0x1,0); // [0:0]: RG_SMT_SYSRSTB; 6/30: Kim
	ret = pmic_config_interface(0x148,0x1,0x1,1); // [1:1]: RG_SMT_INT;
	ret = pmic_config_interface(0x148,0x1,0x1,2); // [2:2]: RG_SMT_SRCLKEN;
	ret = pmic_config_interface(0x148,0x1,0x1,3); // [3:3]: RG_SMT_RTC_32K1V8;
	ret = pmic_config_interface(0x14A,0x1,0x1,0); // [0:0]: RG_SMT_SPI_CLK; 7/26 chihao SPI SMT to anti noise
	ret = pmic_config_interface(0x14A,0x1,0x1,1); // [1:1]: RG_SMT_SPI_CSN; 7/26 chihao SPI SMT to anti noise
	ret = pmic_config_interface(0x14A,0x1,0x1,2); // [2:2]: RG_SMT_SPI_MOSI; 7/26 chihao SPI SMT to anti noise
	ret = pmic_config_interface(0x14A,0x1,0x1,3); // [3:3]: RG_SMT_SPI_MISO; 7/26 chihao SPI SMT to anti noise
	ret = pmic_config_interface(0x154,0xE,0xF,0); // [3:0]: RG_OCTL_SPI_CLK; 7/19: Chihao SPI test
	ret = pmic_config_interface(0x154,0xE,0xF,4); // [7:4]: RG_OCTL_SPI_CSN; 7/19: Chihao SPI test
	ret = pmic_config_interface(0x154,0xE,0xF,8); // [11:8]: RG_OCTL_SPI_MOSI; 7/19: Chihao SPI test
	ret = pmic_config_interface(0x154,0xE,0xF,12); // [15:12]: RG_OCTL_SPI_MISO; 7/19: Chihao SPI test
	ret = pmic_config_interface(0x20E,0x0,0x3,0); // [1:0]: RG_VPROC_RZSEL; 8/8: Lan, compensation change for load transient improvement
	ret = pmic_config_interface(0x20E,0x3,0x3,6); // [7:6]: RG_VPROC_CSR; 8/8: Lan, compensation change for load transient improvement
	ret = pmic_config_interface(0x212,0x2,0x3,0); // [1:0]: RG_VPROC_SLP; 8/8: Lan, compensation change for load transient improvement
	ret = pmic_config_interface(0x212,0x2,0x3,4); // [5:4]: QI_VPROC_VSLEEP; 8/8 Lan, Sleep
	ret = pmic_config_interface(0x216,0x1,0x1,1); // [1:1]: VPROC_VOSEL_CTRL; 7/11:Lan
	ret = pmic_config_interface(0x21C,0x1,0x1,7); // [7:7]: VPROC_SFCHG_FEN; 8/8 Lan, DVFS slew rate control
	ret = pmic_config_interface(0x21C,0x1,0x1,15); // [15:15]: VPROC_SFCHG_REN; 8/8 Lan, DVFS slew rate control
	ret = pmic_config_interface(0x222,0x18,0x7F,0); // [6:0]: VPROC_VOSEL_SLEEP; 7/11:Lan
	ret = pmic_config_interface(0x226,0x3,0x3,0); // [1:0]: VPROC_BURST; 8/8: Lan, compensation change for load transient improvement
	ret = pmic_config_interface(0x230,0x3,0x3,0); // [1:0]: VPROC_TRANSTD; 8/8: Lan, DVFS FPWM period
	ret = pmic_config_interface(0x230,0x3,0x3,4); // [5:4]: VPROC_VOSEL_TRANS_EN; 8/8: Lan, DVFS FPWM edge
	ret = pmic_config_interface(0x230,0x1,0x1,8); // [8:8]: VPROC_VSLEEP_EN; 7/11:Lan
	ret = pmic_config_interface(0x23C,0x1,0x1,1); // [1:1]: VSYS_VOSEL_CTRL; 7/11: Lan
	ret = pmic_config_interface(0x256,0x1,0x1,8); // [8:8]: VSYS_VSLEEP_EN; 7/11:Lan
	ret = pmic_config_interface(0x302,0x0,0x3,0); // [1:0]: RG_VCORE_RZSEL; 8/8: Lan, compensation change for load transient improvement
	ret = pmic_config_interface(0x302,0x3,0x3,6); // [7:6]: RG_VCORE_CSR; 8/8: Lan, compensation change for load transient improvement
	ret = pmic_config_interface(0x306,0x2,0x3,0); // [1:0]: RG_VCORE_SLP; 8/8: Lan, compensation change for load transient improvement
	ret = pmic_config_interface(0x306,0x2,0x3,4); // [5:4]: QI_VCORE_VSLEEP; 8/8 Lan, Sleep
	ret = pmic_config_interface(0x30A,0x1,0x1,1); // [1:1]: VCORE_VOSEL_CTRL; 7/11:Lan
	ret = pmic_config_interface(0x310,0x1,0x1,7); // [7:7]: VCORE_SFCHG_FEN; 8/8 Lan, DVFS slew rate control
	ret = pmic_config_interface(0x310,0x1,0x1,15); // [15:15]: VCORE_SFCHG_REN; 8/8 Lan, DVFS slew rate control
	ret = pmic_config_interface(0x316,0x18,0x7F,0); // [6:0]: VCORE_VOSEL_SLEEP; 7/11:Lan
	ret = pmic_config_interface(0x31A,0x3,0x3,0); // [1:0]: VCORE_BURST; 8/8: Lan, compensation change for load transient improvement
	ret = pmic_config_interface(0x324,0x3,0x3,0); // [1:0]: VCORE_TRANSTD; 8/8: Lan, DVFS FPWM period
	ret = pmic_config_interface(0x324,0x3,0x3,4); // [5:4]: VCORE_VOSEL_TRANS_EN; 8/8: Lan, DVFS FPWM edge
	ret = pmic_config_interface(0x324,0x1,0x1,8); // [8:8]: VCORE_VSLEEP_EN; 7/11:Lan
	ret = pmic_config_interface(0x738,0x0,0x1,15); // [15:15]: AUXADC_CK_AON; Huiyan.Tang
	ret = pmic_config_interface(0x75C,0x0,0x1,14); // [14:14]: AUXADC_START_SHADE_EN; 7/11:Jyun-Jia comment keep default value

	if (platform_chip_ver() > CHIP_VER_E1)
		ret = pmic_config_interface(0x216,0x1,0x1,0); // [1:1]: VPROC_EN_CTRL; //only for suspend mode

	pmic_config_interface(0x21E, 0x50, 0x7F, 0); // set Vproc to 1.2 for cpu freq 1.0GHz
	pmic_config_interface(0x220, 0x50, 0x7F, 0); // set Vproc to 1.2 for cpu freq 1.0GHz
}

static void pmic_buck_oc_enable(void)
{
	int reg_val = 0;

	pmic_read_interface(MT6392_BUCK_OC_CON0, &reg_val, 0xFFFF, 0);
	print("[pmic_buck_oc_enable] MT6392_BUCK_OC_CON0 0x%x = 0x%x\n", MT6392_BUCK_OC_CON0, reg_val);

	pmic_config_interface(MT6392_BUCK_OC_CON3, 0x7, 0x7, 0);
	pmic_config_interface(MT6392_BUCK_OC_CON4, 0x7, 0x7, 0);
	pmic_config_interface(MT6392_BUCK_OC_CON0, 0x7, 0x7, 0);

	pmic_read_interface(MT6392_BUCK_OC_CON0, &reg_val, 0xFFFF, 0);
	print("[pmic_buck_oc_enable] Clear Buck OC Flag, MT6392_BUCK_OC_CON0 0x%x = 0x%x\n",
		MT6392_BUCK_OC_CON0, reg_val);

	pmic_config_interface(MT6392_BUCK_OC_CON3, 0x0, 0x7, 0);
	pmic_config_interface(MT6392_BUCK_EFUSE_OC_CON0, 0x1, 0x1, 1);
	pmic_config_interface(MT6392_STRUP_CON16, 0x0, 0x7, 0);
	print("[pmic_buck_oc_enable] Enable Buck OC shutdown function...\n");
}

static void pmic_ldo_cali_sw_bonding(void)
{
	int reg_val = 0;
	u32 cid = mt6392_upmu_get_cid();

	/* RG_VAUD28_CAL: 244~247 */
	pmic_read_interface(MT6392_EFUSE_DOUT_240_255, &reg_val, 0xF, 4);
	pmic_config_interface(MT6392_ANALDO_CON22, reg_val,
		MT6392_PMIC_RG_VAUD28_CAL_MASK, MT6392_PMIC_RG_VAUD28_CAL_SHIFT);

	/* RG_VAUD22_CAL: 248~251 */
	pmic_read_interface(MT6392_EFUSE_DOUT_240_255, &reg_val, 0xF, 8);
	pmic_config_interface(MT6392_ANALDO_CON8, reg_val,
		MT6392_PMIC_RG_VAUD22_CAL_MASK, MT6392_PMIC_RG_VAUD22_CAL_SHIFT);

	/* RG_VCAMA_CAL: 252~255 */
	pmic_read_interface(MT6392_EFUSE_DOUT_240_255, &reg_val, 0xF, 12);
	pmic_config_interface(MT6392_ANALDO_CON10, reg_val,
		MT6392_PMIC_RG_VCAMA_CAL_MASK, MT6392_PMIC_RG_VCAMA_CAL_SHIFT);

	/* RG_VCAMD_CAL: 153~156 */
	pmic_read_interface(MT6392_EFUSE_DOUT_144_159, &reg_val, 0xF, 12);
	pmic_config_interface(MT6392_DIGLDO_CON52, reg_val,
		MT6392_PMIC_RG_VCAMD_CAL_MASK, MT6392_PMIC_RG_VCAMD_CAL_SHIFT);

	if (cid == 0x1092) {
		/* RG_VMC_CAL: 256~259 */
		pmic_read_interface(MT6392_EFUSE_DOUT_256_271, &reg_val, 0xF, 0);
		pmic_config_interface(MT6392_DIGLDO_CON24, reg_val,
			MT6392_PMIC_RG_VMC_CAL_MASK, MT6392_PMIC_RG_VMC_CAL_SHIFT);
	}

	print("[pmic_ldo_cali_sw_bonding] Done...................\n");
}

//==============================================================================
// PMIC6392 Init Code
//==============================================================================
U32 pmic_init (void)
{
	U32 ret_code = PMIC_TEST_PASS;
	int ret_val=0;
	int reg_val=0;

	print("[pmic6392_init] Preloader Start..................\n");

	print("[pmic6392_init] PMIC CHIP Code = 0x%x\n", mt6392_upmu_get_cid());

	ret_val=pmic_read_interface(MT6392_STRUP_CON18, (&reg_val),0xFFFF, 0);
	print("[pmic6392_init] Debug Status: Reg[0x%x]=0x%x\n", MT6392_STRUP_CON18, reg_val);
	ret_val=pmic_read_interface(MT6392_STRUP_CON19, (&reg_val),0xFFFF, 0);
	print("[pmic6392_init] Debug Status: Reg[0x%x]=0x%x\n", MT6392_STRUP_CON19, reg_val);

	/* clear power off status */
	ret_val=pmic_config_interface(MT6392_STRUP_CON17, 0x1, 0x1, 0);

	//put pmic hw initial setting here

	//USBDL reset 1
	//ret_val=pmic_config_interface(MT6392_CHR_CON16, 0x01, MT6392_PMIC_RG_USBDL_RST_MASK, MT6392_PMIC_RG_USBDL_RST_SHIFT);

	print("[pmic6392_init] powerKey = %d\n", pmic_detect_powerkey());
	print("[pmic6392_init] is USB in = 0x%x\n", pmic_IsUsbCableIn());

	/* pmic initial setting */
	PMIC_INIT_SETTING_V1();

	//Enable PMIC RST function (depends on main chip RST function)
	ret_val=pmic_config_interface(0x011A, 0x1, 0x1, 1);
#if DDR_AUTO_REBOOT
	ret_val=pmic_config_interface(0x011A, 0x1, 0x1, 2);
#endif
	ret_val=pmic_config_interface(0x011A, 0x1, 0x1, 3);
	ret_val=pmic_config_interface(0x0136, 0x1, 0x1, 0);
	ret_val=pmic_read_interface( 0x011A, (&reg_val),0xFFFF, 0);
	print("[pmic6392_init] Reg[0x%x]=0x%x\n", 0x011A, reg_val);
	ret_val=pmic_read_interface( 0x0136, (&reg_val),0xFFFF, 0);
	print("[pmic6392_init] Reg[0x%x]=0x%x\n", 0x0136, reg_val);

	/* Clear Buck OC flag and enable function */
	pmic_buck_oc_enable();

	/* LDO efuse bits SW bonding */
	pmic_ldo_cali_sw_bonding();

	/* turn on VAUD22/VAUD28 for audio */
	pmic_config_interface(0x404, 0x1, 0x1, 14);
	pmic_config_interface(0x424, 0x1, 0x1, 14);

	/* Configure long press pwrkey shutdown */
	ret_val = pmic_config_interface(MT6392_STRUP_CON13, 0x2, 0x3, 0);
	ret_val = pmic_config_interface(MT6392_STRUP_CON13, 0x1, 0x1, 4);
	ret_val = pmic_config_interface(MT6392_STRUP_CON13, 0x1, 0x1, 6);
	ret_val = pmic_read_interface(MT6392_STRUP_CON13, (&reg_val),0xFFFF, 0);
	print("[pmic6392_init] Reg[0x%x]=0x%x\n", MT6392_STRUP_CON13, reg_val);

	/*mt6392 auxadc init*/
	auxadc_hw_init();

	print("[pmic6392_init] Done...................\n");

	return ret_code;
}

