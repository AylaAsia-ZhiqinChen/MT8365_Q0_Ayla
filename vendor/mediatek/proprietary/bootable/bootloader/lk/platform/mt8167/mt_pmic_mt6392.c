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

#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_pmic.h>
#include <platform/mt_gpt.h>
#include <platform/mt_pmic_wrap_init.h>
#include <printf.h>

//==============================================================================
// Global variable
//==============================================================================
int Enable_PMIC_LOG = 1;

CHARGER_TYPE g_ret = CHARGER_UNKNOWN;
int g_charger_in_flag = 0;
int g_first_check=0;
static int count_num = 100;

extern int g_R_BAT_SENSE;
extern int g_R_I_SENSE;
extern int g_R_CHARGER_1;
extern int g_R_CHARGER_2;

#ifdef MTK_MT6333_SUPPORT
extern void mt6333_init (void);
#endif

static const u32 mt6392_auxadc_regs[] = {
	MT6392_AUXADC_ADC0, MT6392_AUXADC_ADC1, MT6392_AUXADC_ADC2,
	MT6392_AUXADC_ADC3, MT6392_AUXADC_ADC4, MT6392_AUXADC_ADC5,
	MT6392_AUXADC_ADC6, MT6392_AUXADC_ADC7, MT6392_AUXADC_ADC8,
	MT6392_AUXADC_ADC9, MT6392_AUXADC_ADC10, MT6392_AUXADC_ADC11,
	MT6392_AUXADC_ADC12, MT6392_AUXADC_ADC12, MT6392_AUXADC_ADC12,
	MT6392_AUXADC_ADC12,
};

//==============================================================================
// PMIC access API
//==============================================================================
U32 pmic_read_interface (U32 RegNum, U32 *val, U32 MASK, U32 SHIFT)
{
	U32 return_value = 0;
	U32 pmic6323_reg = 0;
	U32 rdata = 0;

	//mt6323_read_byte(RegNum, &pmic6323_reg);
	return_value= pwrap_wacs2(0, (RegNum), 0, &rdata);
	pmic6323_reg=rdata;
	if (return_value!=0) {
		printf("[pmic_read_interface] Reg[%x]= pmic_wrap read data fail\n", RegNum);
		return return_value;
	}
	//printf("[pmic_read_interface] Reg[%x]=0x%x\n", RegNum, pmic6323_reg);

	pmic6323_reg &= (MASK << SHIFT);
	*val = (pmic6323_reg >> SHIFT);
	//printf("[pmic_read_interface] val=0x%x\n", *val);

	return return_value;
}

U32 pmic_config_interface (U32 RegNum, U32 val, U32 MASK, U32 SHIFT)
{
	U32 return_value = 0;
	U32 pmic6323_reg = 0;
	U32 rdata = 0;

	if (val > MASK) {
		printf("[pmic_config_interface] Invalid data, Reg[%x]: MASK = 0x%x, val = 0x%x\n",
			RegNum, MASK, val);
		return E_PWR_INVALID_DATA;
	}

	//1. mt6323_read_byte(RegNum, &pmic6323_reg);
	return_value= pwrap_wacs2(0, (RegNum), 0, &rdata);
	pmic6323_reg=rdata;
	if (return_value!=0) {
		printf("[pmic_config_interface] Reg[%x]= pmic_wrap read data fail\n", RegNum);
		return return_value;
	}
	//printf("[pmic_config_interface] Reg[%x]=0x%x\n", RegNum, pmic6323_reg);

	pmic6323_reg &= ~(MASK << SHIFT);
	pmic6323_reg |= (val << SHIFT);

	//2. mt6323_write_byte(RegNum, pmic6323_reg);
	return_value= pwrap_wacs2(1, (RegNum), pmic6323_reg, &rdata);
	if (return_value!=0) {
		printf("[pmic_config_interface] Reg[%x]= pmic_wrap read data fail\n", RegNum);
		return return_value;
	}
	//printf("[pmic_config_interface] write Reg[%x]=0x%x\n", RegNum, pmic6323_reg);

#if 0
	//3. Double Check
	//mt6323_read_byte(RegNum, &pmic6323_reg);
	return_value= pwrap_wacs2(0, (RegNum), 0, &rdata);
	pmic6323_reg=rdata;
	if (return_value!=0) {
		printf("[pmic_config_interface] Reg[%x]= pmic_wrap write data fail\n", RegNum);
		return return_value;
	}
	printf("[pmic_config_interface] Reg[%x]=0x%x\n", RegNum, pmic6323_reg);
#endif

	return return_value;
}

//==============================================================================
// PMIC APIs
//==============================================================================


//==============================================================================
// PMIC6392 Usage APIs
//==============================================================================
kal_bool upmu_is_chr_det(void)
{
	U32 tmp32=0;
	tmp32=mt6392_upmu_get_rgs_chrdet();
	if (tmp32 == 0) {
		return KAL_FALSE;
	} else {
		return KAL_TRUE;
	}
}

kal_bool pmic_chrdet_status(void)
{
	if ( upmu_is_chr_det() == KAL_TRUE ) {
#ifndef USER_BUILD
		printf("[pmic_chrdet_status] Charger exist\r\n");
#endif

		return KAL_TRUE;
	} else {
#ifndef USER_BUILD
		printf("[pmic_chrdet_status] No charger\r\n");
#endif

		return KAL_FALSE;
	}
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

	if (Enable_PMIC_LOG>1)
		printf("%d", ret);

	if (val==1) {
#ifndef USER_BUILD
		printf("LK pmic powerkey Release\n");
#endif

		return 0;
	} else {
#ifndef USER_BUILD
		printf("LK pmic powerkey Press\n");
#endif

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

	if (Enable_PMIC_LOG>1)
		printf("%d", ret);

	if (val==1) {
#ifndef USER_BUILD
		printf("LK pmic FCHRKEY Release\n");
#endif

		return 0;
	} else {
#ifndef USER_BUILD
		printf("LK pmic FCHRKEY Press\n");
#endif

		return 1;
	}
}

kal_uint32 upmu_get_reg_value(kal_uint32 reg)
{
	U32 ret=0;
	U32 temp_val=0;

	ret=pmic_read_interface(reg, &temp_val, 0xFFFF, 0x0);

	if (Enable_PMIC_LOG>1)
		printf("%d", ret);

	return temp_val;
}

void PMIC_DUMP_ALL_Register(void)
{
	U32 i=0;
	U32 ret=0;
	U32 reg_val=0;

	for (i=0; i<0x800; i++) {
		ret=pmic_read_interface(i,&reg_val,0xFFFF,0);
		printf("Reg[0x%x]=0x%x, %d\n", i, reg_val, ret);
	}
}

//==============================================================================
// PMIC6392 Init Code
//==============================================================================
void PMIC_INIT_SETTING_V1(void)
{
	U32 chip_version = 0;

	chip_version = mt6392_upmu_get_cid();

	printf("[LK_PMIC_INIT_SETTING_V1] PMIC Chip = 0x%x\n",chip_version);
}

void PMIC_CUSTOM_SETTING_V1(void)
{
	//printf("[PMIC_CUSTOM_SETTING_V1] \n");
}

U32 pmic_init (void)
{
	U32 ret_code = PMIC_TEST_PASS;

	//printf("\n[LK_pmic6323_init] LK Start...................\n");

	mt6392_upmu_set_rg_chrind_on(0);
	//printf("[LK_PMIC_INIT] Turn Off chrind\n");

	PMIC_INIT_SETTING_V1();
	//printf("[LK_PMIC_INIT_SETTING_V1] Done\n");

	PMIC_CUSTOM_SETTING_V1();
	//printf("[LK_PMIC_CUSTOM_SETTING_V1] Done\n");

	pmic_detect_powerkey();
	printf("chr detection : %d \n",upmu_is_chr_det());

	//PMIC_DUMP_ALL_Register();

#ifdef MTK_MT6333_SUPPORT
	mt6333_init();
#endif

	return ret_code;
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
	int ret, adc_rdy, reg_val;
	int raw_data, adc_result;
	int raw_mask = 0, r_val_temp = 0;
	int adc_div = 4096;
	int count = 0;
	int raw_data_sum = 0, raw_data_avg = 0;
	int u4Sample_times = 0;

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
	do {
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
				dwChannel = 6;
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
				dwChannel = 8;
			} else {
				/*DM channel*/
				ret = pmic_config_interface(MT6392_CHR_CON18,0x0b00,0x0f00,0);
				if (ret < 0) {
					return ret;
				}
			}
		}

		/* AUXADC_RQST0 SET  */
		ret = pmic_config_interface(MT6392_AUXADC_RQST0_SET,0x1,0xffff,dwChannel);
		if (ret < 0) {
			return ret;
		}

		if ((dwChannel == 0) || (dwChannel == 1))
			udelay(1500);
		else
			udelay(100);

		/* check auxadc is ready */
		do {
			ret = pmic_read_interface(mt6392_auxadc_regs[dwChannel], &reg_val, 0xffff, 0);
			if (ret < 0) {
				return ret;
			}
			udelay(100);
			adc_rdy = (reg_val >> 15) & 1;
			if (adc_rdy != 0)
				break;
			count++;
		} while (count < count_num);

		count = 0;
		if (adc_rdy != 1) {
			printf("PMIC_GetOneChannelValue adc get ready Fail\n");
			return -KAL_FALSE;
		}

		/* get the raw data and calculate the adc result of adc */
		ret = pmic_read_interface(mt6392_auxadc_regs[dwChannel], &reg_val, 0xffff, 0);
		if (ret < 0) {
			return ret;
		}

		switch (dwChannel) {
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
		raw_data_sum += raw_data;
		u4Sample_times++;
	} while (u4Sample_times < deCount);

	/* get auxadc real result*/
	raw_data_avg = raw_data_sum / deCount;
	adc_result = (raw_data_avg * r_val_temp * VOLTAGE_FULL_RANGE) / adc_div;

	return adc_result;
}


int get_bat_sense_volt(int times)
{
	return PMIC_IMM_GetOneChannelValue(VBAT_CHANNEL_NUMBER,times,1);
}

int get_i_sense_volt(int times)
{
	return PMIC_IMM_GetOneChannelValue(ISENSE_CHANNEL_NUMBER,times,1);
}

int get_charger_volt(int times)
{
	return PMIC_IMM_GetOneChannelValue(VCHARGER_CHANNEL_NUMBER,times,1);
}

int get_tbat_volt(int times)
{
	return PMIC_IMM_GetOneChannelValue(VBATTEMP_CHANNEL_NUMBER,times,1);
}

//////////////////////////////////////////////////////////////////////////////////////////
// PMIC-Charger Type Detection
//////////////////////////////////////////////////////////////////////////////////////////

#if defined(CONFIG_POWER_EXT)
#else

kal_uint32 Enable_Detection_Log = 0;

static void hw_bc11_dump_register(void)
{
	kal_uint32 reg_val = 0;
	kal_uint32 reg_num = MT6392_CHR_CON18;
	kal_uint32 i = 0;

	for (i=reg_num ; i<=MT6392_CHR_CON19 ; i+=2) {
		reg_val = upmu_get_reg_value(i);
		printf("Chr Reg[0x%x]=0x%x \r\n", i, reg_val);
	}
}

extern void Charger_Detect_Init(void);
extern void Charger_Detect_Release(void);

static void hw_bc11_init(void)
{
	mdelay(300);
	Charger_Detect_Init();

	//RG_BC11_BIAS_EN=1
	mt6392_upmu_set_rg_bc11_bias_en(0x1);
	//RG_BC11_VSRC_EN[1:0]=00
	mt6392_upmu_set_rg_bc11_vsrc_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	mt6392_upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_IPU_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_IPD_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipd_en(0x0);
	//BC11_RST=1
	mt6392_upmu_set_rg_bc11_rst(0x1);
	//BC11_BB_CTRL=1
	mt6392_upmu_set_rg_bc11_bb_ctrl(0x1);

	//msleep(10);
	mdelay(50);

	if (Enable_Detection_Log) {
		printf("hw_bc11_init() \r\n");
		hw_bc11_dump_register();
	}
}

static U32 hw_bc11_DCD(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_IPU_EN[1.0] = 10
	mt6392_upmu_set_rg_bc11_ipu_en(0x2);
	//RG_BC11_IPD_EN[1.0] = 01
	mt6392_upmu_set_rg_bc11_ipd_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=01
	mt6392_upmu_set_rg_bc11_vref_vth(0x1);
	//RG_BC11_CMP_EN[1.0] = 10
	mt6392_upmu_set_rg_bc11_cmp_en(0x2);

	//msleep(20);
	mdelay(80);

	wChargerAvail = mt6392_upmu_get_rgs_bc11_cmp_out();

	if (Enable_Detection_Log) {
		printf("hw_bc11_DCD() \r\n");
		hw_bc11_dump_register();
	}

	//RG_BC11_IPU_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_IPD_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipd_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	mt6392_upmu_set_rg_bc11_vref_vth(0x0);

	return wChargerAvail;
}

static U32 hw_bc11_stepA1(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_IPU_EN[1.0] = 10
	mt6392_upmu_set_rg_bc11_ipu_en(0x2);
	//RG_BC11_VREF_VTH = [1:0]=10
	mt6392_upmu_set_rg_bc11_vref_vth(0x2);
	//RG_BC11_CMP_EN[1.0] = 10
	mt6392_upmu_set_rg_bc11_cmp_en(0x2);

	//msleep(80);
	mdelay(80);

	wChargerAvail = mt6392_upmu_get_rgs_bc11_cmp_out();

	if (Enable_Detection_Log) {
		printf("hw_bc11_stepA1() \r\n");
		hw_bc11_dump_register();
	}

	//RG_BC11_IPU_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_cmp_en(0x0);

	return  wChargerAvail;
}


static U32 hw_bc11_stepB1(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_IPU_EN[1.0] = 01
	//upmu_set_rg_bc11_ipu_en(0x1);
	mt6392_upmu_set_rg_bc11_ipd_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=10
	//upmu_set_rg_bc11_vref_vth(0x2);
	mt6392_upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 01
	mt6392_upmu_set_rg_bc11_cmp_en(0x1);

	//msleep(80);
	mdelay(80);

	wChargerAvail = mt6392_upmu_get_rgs_bc11_cmp_out();

	if (Enable_Detection_Log) {
		printf("hw_bc11_stepB1() \r\n");
		hw_bc11_dump_register();
	}

	//RG_BC11_IPU_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	mt6392_upmu_set_rg_bc11_vref_vth(0x0);

	return  wChargerAvail;
}


static U32 hw_bc11_stepC1(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_IPU_EN[1.0] = 01
	mt6392_upmu_set_rg_bc11_ipu_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=10
	mt6392_upmu_set_rg_bc11_vref_vth(0x2);
	//RG_BC11_CMP_EN[1.0] = 01
	mt6392_upmu_set_rg_bc11_cmp_en(0x1);

	//msleep(80);
	mdelay(80);

	wChargerAvail = mt6392_upmu_get_rgs_bc11_cmp_out();

	if (Enable_Detection_Log) {
		printf("hw_bc11_stepC1() \r\n");
		hw_bc11_dump_register();
	}

	//RG_BC11_IPU_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	mt6392_upmu_set_rg_bc11_vref_vth(0x0);

	return  wChargerAvail;
}


static U32 hw_bc11_stepA2(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_VSRC_EN[1.0] = 10
	mt6392_upmu_set_rg_bc11_vsrc_en(0x2);
	//RG_BC11_IPD_EN[1:0] = 01
	mt6392_upmu_set_rg_bc11_ipd_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=00
	mt6392_upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 01
	mt6392_upmu_set_rg_bc11_cmp_en(0x1);

	//msleep(80);
	mdelay(80);

	wChargerAvail = mt6392_upmu_get_rgs_bc11_cmp_out();

	if (Enable_Detection_Log) {
		printf("hw_bc11_stepA2() \r\n");
		hw_bc11_dump_register();
	}

	//RG_BC11_VSRC_EN[1:0]=00
	mt6392_upmu_set_rg_bc11_vsrc_en(0x0);
	//RG_BC11_IPD_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipd_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_cmp_en(0x0);

	return  wChargerAvail;
}


static U32 hw_bc11_stepB2(void)
{
	U32 wChargerAvail = 0;

	//RG_BC11_VSRC_EN[1:0]= 01
	mt6392_upmu_set_rg_bc11_vsrc_en(0x1);
	//RG_BC11_IPD_EN[1:0]= 10
	mt6392_upmu_set_rg_bc11_ipd_en(0x2);
	//RG_BC11_VREF_VTH = [1:0]= 00
	mt6392_upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 10
	mt6392_upmu_set_rg_bc11_cmp_en(0x2);

	mdelay(80);

	wChargerAvail = mt6392_upmu_get_rgs_bc11_cmp_out();

        if (Enable_Detection_Log) {
                printf("hw_bc11_stepB2() \r\n");
                hw_bc11_dump_register();
        }

	//RG_BC11_VSRC_EN[1:0]= 00
	mt6392_upmu_set_rg_bc11_vsrc_en(0x0);
	//RG_BC11_IPD_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipd_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_cmp_en(0x0);

	return  wChargerAvail;
}


static void hw_bc11_done(void)
{
	//RG_BC11_VSRC_EN[1:0]=00
	mt6392_upmu_set_rg_bc11_vsrc_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=0
	mt6392_upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_IPU_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_IPD_EN[1.0] = 00
	mt6392_upmu_set_rg_bc11_ipd_en(0x0);
	//RG_BC11_BIAS_EN=0
	mt6392_upmu_set_rg_bc11_bias_en(0x0);

	Charger_Detect_Release();

	if (Enable_Detection_Log) {
		printf("hw_bc11_done() \r\n");
		hw_bc11_dump_register();
	}

}
#endif

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

