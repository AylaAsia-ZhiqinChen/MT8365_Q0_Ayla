/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
#include "typedefs.h"
#include "mt8168.h"
#include "pll.h"
#include "spm.h"

/* #define SET_ARMPLL_DIV_EN */
#define SET_ARMPLL_CLK_SRC
/* #define _FREQ_SCAN_ */
/* #define CLK_ALLON */

#ifdef _FREQ_SCAN_
unsigned int mt_get_abist_freq(unsigned int ID)
{
	int output = 0, i = 0;
	unsigned int temp, clk26cali_0, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;
	clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
	DRV_WriteReg32(CLK_DBG_CFG, (clk_dbg_cfg & 0xFFC0FFFC)|(ID << 16)); //sel abist_cksw and enable freq meter sel abist
	clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
	DRV_WriteReg32(CLK_MISC_CFG_0, (clk_misc_cfg_0 & 0x00FFFFFF) | (0x3 << 24)); // select divider, WAIT CONFIRM
	clk26cali_0 = DRV_Reg32(CLK26CALI_0);
	clk26cali_1 = DRV_Reg32(CLK26CALI_1);
	DRV_WriteReg32(CLK26CALI_0, 0x1000);
	DRV_WriteReg32(CLK26CALI_0, 0x1010);
	/* wait frequency meter finish */
	while (DRV_Reg32(CLK26CALI_0) & 0x10)
	{
		mdelay(10);
		i++;
		if(i > 10)
			break;
	}
	temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;
	output = ((temp * 26000) ) / 1024; // Khz
	DRV_WriteReg32(CLK_DBG_CFG, clk_dbg_cfg);
	DRV_WriteReg32(CLK_MISC_CFG_0, clk_misc_cfg_0);
	DRV_WriteReg32(CLK26CALI_0, clk26cali_0);
	DRV_WriteReg32(CLK26CALI_1, clk26cali_1);
	//print("abist meter[%d] = %d Khz\n", ID, output);
	return output * 4;
}
static unsigned int mt_get_ckgen_freq(unsigned int ID)
{
	int output = 0, i = 0;
	unsigned int temp, clk26cali_0, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;
	clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
	DRV_WriteReg32(CLK_DBG_CFG, (clk_dbg_cfg & 0xFFFFC0FC)|(ID << 8)|(0x1)); //sel ckgen_cksw[22] and enable freq meter sel ckgen[21:16], 01:hd_faxi_ck
	clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
	DRV_WriteReg32(CLK_MISC_CFG_0, (clk_misc_cfg_0 & 0x00FFFFFF)); // select divider?dvt set zero
	clk26cali_0 = DRV_Reg32(CLK26CALI_0);
	clk26cali_1 = DRV_Reg32(CLK26CALI_1);
	DRV_WriteReg32(CLK26CALI_0, 0x1000);
	DRV_WriteReg32(CLK26CALI_0, 0x1010);
	/* wait frequency meter finish */
	while (DRV_Reg32(CLK26CALI_0) & 0x10)
	{
		mdelay(10);
		i++;
		if(i > 10)
			break;
	}
	temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;
	output = ((temp * 26000) ) / 1024; // Khz
	DRV_WriteReg32(CLK_DBG_CFG, clk_dbg_cfg);
	DRV_WriteReg32(CLK_MISC_CFG_0, clk_misc_cfg_0);
	DRV_WriteReg32(CLK26CALI_0, clk26cali_0);
	DRV_WriteReg32(CLK26CALI_1, clk26cali_1);
	//print("ckgen meter[%d] = %d Khz\n", ID, output);
	return output;
}
#define Range 1000
const int ckgen_needCheck_array[] =
{
/* 01 - 05*/	1, 1, 1, 1, 1,
/* 06 - 10*/	1, 1, 1, 1, 1,
/* 11 - 15*/	1, 1, 1, 1, 1,
/* 16 - 20*/	1, 1, 1, 1, 1,
/* 21 - 25*/	1, 1, 1, 1, 1,
/* 26 - 30*/	1, 1, 1, 1, 1,
/* 31 - 35*/	1, 1, 1, 1, 1,
/* 36 - 40*/	1, 1, 1, 1, 1,
/* 41 - 45*/	1, 1, 1, 1, 1,
/* 46 - 50*/	0, 0, 1, 1, 0,
/* 51 - 55*/	0, 0, 0, 0, 0,
/* 56 - 58*/	0, 0, 0,
};
const int ckgen_golden_array[] =
{
/*add if need*/
/* 01 - 05*/	156000, 466000, 312000, 26000, 460000,
/* 06 - 10*/	273000, 24000, 24000, 24000, 24000,
/* 11 - 15*/	26000, 109200, 273000, 384000, 192000,
/* 16 - 20*/	26000, 136500, 196608, 24576, 26000,
/* 21 - 25*/	273000, 136500, 62400, 68250, 124800,
/* 26 - 30*/	68250, 208000, 384000, 15625, 208000,
/* 31 - 35*/	0, 0, 0, 0, 0,
/* 36 - 40*/	0, 0, 0, 0, 0,
/* 41 - 45*/	0, 0, 0, 0, 0,
/* 46 - 50*/	0, 0, 0, 0, 0,
/* 51 - 55*/	0, 0, 0, 0, 0,
};

const char *ckgen_array[] =
{
	"axi_ck",
	"mem_ck",
	"mm_ck",
	"scp_ck",
	"mfg_ck",
	"atb_ck",
	"camtg_ck",
	"camtg1_ck",
	"uart_ck",
	"f_fspi_ck",
	"msdc50_0_hclk_ck",
	"fmsdc2_2_hclk_ck",
	"msdc50_0_ck",
	"msdc50_2_ck",
	"msdc30_1_ck",
	"audio_ck",
	"aud_intbus_ck",
	"aud_1_ck",
	"aud_2_ck",
	"aud_engen1_ck",
	"aud_engen2_ck",
	"hf_faud_spdif_ck",
	"disp_pwm_ck",
	"sspm_ck",
	"dxcc_ck",
	"ssusb_sys_ck",
	"ssusb_xhci_ck",
	"spm_ck",
	"i2c_ck",
	"pwm_ck",
	"seninf_ck",
	"aes_fde_ck",
	"camtm_ck",
	"dpi0_ck",
	"dpi1_ck",
	"dsp_ck",
	"nfi2x_ck",
	"nfiecc_ck",
	"ecc_ck",
	"eth_ck",
	"gcpu_ck",
	"gcpu_cpm_ck",
	"apu_ck",
	"apu_if_ck",
	"mbist_diag_clk",
	"f_ufs_mp_sap_cfg_ck",
	"f_ufs_tick1us_ck",
};

const int abist_needCheck_array[] =
{
/* 01 - 05*/	1, 0, 1, 1, 1,
/* 06 - 10*/	0, 1, 1, 1, 1,
/* 11 - 15*/	1, 1, 1, 1, 1,
/* 16 - 20*/	1, 1, 1, 1, 1,
/* 21 - 25*/	0, 1, 1, 1, 1,
/* 26 - 30*/	1, 1, 1, 1, 1,
/* 31 - 35*/	1, 1, 1, 1, 1,
/* 36 - 40*/	1, 1, 1, 0, 1,
/* 41 - 45*/	1, 1, 1, 1, 1,
/* 46 - 50*/	1, 1, 1, 1, 1,
/* 51 - 55*/	1, 1, 1, 1, 1,
/* 56 - 58*/	1, 1, 1, 1,
};
/* if needCheck_array[x]=1 but golden_array[x]=0: means we only want to read the value*/
const int ckgen_abist_golden_array[] =
{
/* 01 - 05*/	0, 0, 0, 0, 0,
/* 06 - 10*/	0, 0, 546000, 364000, 218400,
/* 11 - 15*/	156000, 624000, 416000, 249600, 178285,
/* 16 - 20*/	26000, 0, 0, 0, 0,
/* 21 - 25*/	0, 1216000, 1092000, 1248000, 460000,
/* 26 - 30*/	384000, 457000, 196608, 0, 0,
/* 31 - 35*/	192000, 0, 0, 0, 0,
/* 36 - 40*/	0, 0, 0, 0, 0,
/* 41 - 45*/	0, 0, 0, 0, 0,
/* 46 - 50*/	0, 0, 0, 0, 208000,
/* 51 - 55*/	0, 0, 0, 0, 48000,
/* 56 - 58*/	104000, 52000,
};

const char *abist_array[] =
{
	"AD_ARMPLL_CK",
	"0",
	"AD_MAINPLLCK",
	"AD_CSI0A_CDPHY_DELAYCAL_CK",
	"AD_CSI0B_CDPHY_DELAYCAL_CK",
	"0",
	"AD_USB20_CLK480M",
	"AD_USB20_CLK480M_1P",
	"AD_MADADC_26MCKO",
	"AD_MAINPLL_H546M_CK",
	"AD_MAINPLL_H364M_CK",
	"AD_MAINPLL_H218P4M_CK",
	"AD_MAINPLL_H156M_CK",
	"AD_UNIVPLL_1248M_CK",
	"AD_USB20_192M_CK",
	"AD_UNIVPLL_624M_CK",
	"AD_UNIVPLL_416M_CK",
	"AD_UNIVPLL_249P6M_CK",
	"AD_UNIVPLL_178P3M_CK",
	"AD_SYS_26M_CK",
	"AD_CSI1A_DPHY_DELAYCAL_CK",
	"AD_CSI1B_DPHY_DELAYCAL_CK",
	"AD_CSI2A_DPHY_DELAYCAL_CK",
	"AD_CSI2B_DPHY_DELAYCAL_CK",
	"RTC32K",
	"AD_MMPLL_CK",
	"AD_MFGPLL_CK",
	"AD_MSDCPLL_CK",
	"AD_DSI0_LNTC_DSICLK",
	"AD_DSI0_MPPLL_TST_CK",
	"AD_APPLLGP_TST_CK",
	"AD_APLL1_180P6336M_CK",
	"AD_APLL1_196P608M_CK",
	"AD_MADCKO_TEST",
	"AD_MPLL_208M_CK",
	"Armpll_ll_mon_ck",
	"vad_clk_i",
	"msdc01_in_ck",
	"0",
	"msdc11_in_ck",
	"msdc12_in_ck",
	"AD_PLLGP_TST_CK",
	"AD_LVDSTX_CLKDIG_CTS",
	"AD_LVDSTX_CLKDIG",
	"AD_VPLL_DPIX_CK",
	"DA_USB20_48M_DIV_CK",
	"DA_UNIV_48M_DIV_CK",
	"DA_MPLL_104M_DIV_CK",
	"DA_MPLL_52M_DIV_CK",
	"DA_PLLGP_CPU_CK_MON",
	"trng_freq_debug_out0",
	"trng_freq_debug_out1",
	"AD_LVDSTX_MONCLK",
	"AD_VPLL_MONREF_CK",
	"AD_VPLL_MONFBK_CK",
	"AD_LVDSPLL_300M_CK",
	"AD_DSPPLL_CK",
	"AD_APUPLL_CK",
};
#endif
#define CKGEN_CHANNEL_CNT 58
#define ABIST_CHANNEL_CNT 58
unsigned int ret_feq_store[CKGEN_CHANNEL_CNT+ABIST_CHANNEL_CNT];
unsigned int ret_feq_total=0;
unsigned int pll_chk_is_fail = 0;
//after pmic_init
void mt_pll_post_init(void)
{
	unsigned int temp,clk_misc_cfg_0,ret_feq,reg_temp;
	unsigned int isFail = 0;
	/* need dram porting code */
	/* mempll_init_main(); */
#ifdef _FREQ_SCAN_
	print("Pll post init start...\n");
	print("==============================\n");
	print("==      Parsing Start       ==\n");
	print("==============================\n");
	for(temp=1; temp<=CKGEN_CHANNEL_CNT; temp++)
	{
		if(!ckgen_needCheck_array[temp-1])
			continue;
		else
			print("%d:",temp);
		ret_feq = 0;
		ret_feq = mt_get_ckgen_freq(temp);
		ret_feq_store[ret_feq_total] = ret_feq;
		ret_feq_total++;
		print("%s:", ckgen_array[temp-1]);
		print("%d\n",ret_feq);
	}
	//abist
	for(temp=1; temp<=ABIST_CHANNEL_CNT; temp++)
	{
		if(!abist_needCheck_array[temp-1])
			continue;
		else
			print("%d:",temp);
		ret_feq = mt_get_abist_freq(temp);
		ret_feq_store[ret_feq_total] = ret_feq;
		ret_feq_total++;
		print("%s:", abist_array[temp-1]);
		print("%d\n", ret_feq);
	}

	print("Pll post init Done!\n");
#endif // _FREQ_SCAN_
}

void mt_pll_prepare(void)
{
	unsigned int mainpll_en = DRV_Reg32(MAINPLL_CON0) & 1;
	unsigned int univpll_en = DRV_Reg32(UNIVPLL_CON0) & 1;
	unsigned int armpll_en = DRV_Reg32(ARMPLL_CON0) & 1;
	unsigned int pllgp_en = DRV_Reg32(AP_PLLGP_CON1) & 3;

	if (!pllgp_en) {
		DRV_WriteReg32(AP_PLLGP_CON1, DRV_Reg32(AP_PLLGP_CON1) | 0x3);
		gpt_busy_wait_us(1);
	}

	if (!mainpll_en && !univpll_en && !armpll_en)
		return;

	/* Before PLL and div_macro disable, switch source clock to 26M */
	DRV_WriteReg32(CLK_CFG_0, (DRV_Reg32(CLK_CFG_0) & 0xFFFFFFF0));
	DRV_WriteReg32(CLK_CFG_UPDATE, 0x00000001);

	DRV_WriteReg32(INFRA_BUS_DCM_CTRL,
		DRV_Reg32(INFRA_BUS_DCM_CTRL) & 0xFFFFFFFC);
	DRV_WriteReg32(PERI_BUS_DCM_CTRL,
		DRV_Reg32(PERI_BUS_DCM_CTRL) & 0xFFFFFFFC);
	/* [22]=1'b0: disable the pllck_sel bit in infrasys DCM */
	DRV_WriteReg32(INFRA_BUS_DCM_CTRL,
		DRV_Reg32(INFRA_BUS_DCM_CTRL) & (~(0x1 << 22)));

	/* switch back to 26m */
	DRV_WriteReg32(BUS_PLL_DIV_CFG,
		DRV_Reg32(BUS_PLL_DIV_CFG) & (~(0x1 << 9)));
	DRV_WriteReg32(ACLKEN_DIV, 0x11);

	DRV_WriteReg32(UNIVPLL_CON0, DRV_Reg32(UNIVPLL_CON0) & 0xFF7FFFFF);
	DRV_WriteReg32(UNIVPLL_CON0, DRV_Reg32(UNIVPLL_CON0) & 0xFFFFFFFE);
	DRV_WriteReg32(MAINPLL_CON0, DRV_Reg32(MAINPLL_CON0) & 0xFF7FFFFF);
	DRV_WriteReg32(MAINPLL_CON0, DRV_Reg32(MAINPLL_CON0) & 0xFFFFFFFE);
	DRV_WriteReg32(ARMPLL_CON0, DRV_Reg32(ARMPLL_CON0) & 0xFFFFFFFE);

	gpt_busy_wait_us(1);

	DRV_WriteReg32(UNIVPLL_CON3, DRV_Reg32(UNIVPLL_CON3) | 0x2);
	DRV_WriteReg32(MAINPLL_CON3, DRV_Reg32(MAINPLL_CON3) | 0x2);
	DRV_WriteReg32(ARMPLL_CON3, DRV_Reg32(ARMPLL_CON3) | 0x2);

	DRV_WriteReg32(UNIVPLL_CON3, DRV_Reg32(UNIVPLL_CON3) & 0xFFFFFFFE);
	DRV_WriteReg32(MAINPLL_CON3, DRV_Reg32(MAINPLL_CON3) & 0xFFFFFFFE);
	DRV_WriteReg32(ARMPLL_CON3, DRV_Reg32(ARMPLL_CON3) & 0xFFFFFFFE);
}

void mt_pll_init(void)
{
	unsigned int temp;

	print("Pll init start...\n");

	mt_pll_prepare();

	temp = DRV_Reg32(AP_PLLGP_CON1);
	temp |= 0x1;
	DRV_WriteReg32(AP_PLLGP_CON1, temp);
	gpt_busy_wait_us(1);

	//step 0
	DRV_WriteReg32(ACLKEN_DIV, 0x12); // MCU Bus DIV2
	//step 1
	temp = DRV_Reg32(AP_PLL_CON0);
	DRV_WriteReg32(AP_PLL_CON0, temp | 0x01);// [0]=1 (CLKSQ_EN)
	//step 2
	gpt_busy_wait_us(100);
	//step 3
	temp = DRV_Reg32(AP_PLL_CON0);
	DRV_WriteReg32(AP_PLL_CON0, temp | 0x2); // [1]=1 (CLKSQ_LPF_EN)
	temp = DRV_Reg32(AP_PLL_CON3);
	DRV_WriteReg32(AP_PLL_CON3, temp & ~0x2); // [1]=0 (LTECLKSQ_EN_SEL)
	temp = DRV_Reg32(AP_PLL_CON4);
	DRV_WriteReg32(AP_PLL_CON4, temp & ~0x10); // [4]=0 (LTECLKSQ_BY_DLY)
	/*************
	* xPLL PWR ON
	**************/
	//step 4
	temp = DRV_Reg32(ARMPLL_CON3);
	DRV_WriteReg32(ARMPLL_CON3, temp | 0x1); // [0]=1 (ARMPLL_PWR_ON)
	//step 7
	temp = DRV_Reg32(MFGPLL_CON3);
	DRV_WriteReg32(MFGPLL_CON3, temp | 0x1); // [0]=1 (GPUPLL_PWR_ON)
	//step 8
	temp = DRV_Reg32(MPLL_CON3);
	DRV_WriteReg32(MPLL_CON3, temp | 0x1); // [0]=1 (MPLL_PWR_ON)
	//step 9
	temp = DRV_Reg32(MAINPLL_CON3);
	DRV_WriteReg32(MAINPLL_CON3, temp | 0x1); // [0]=1 (MAINPLL_PWR_ON)
	//step 10
	temp = DRV_Reg32(UNIVPLL_CON3);
	DRV_WriteReg32(UNIVPLL_CON3, temp | 0x1); // [0]=1 (UNIVPLL_PWR_ON)
	//step 11
	temp = DRV_Reg32(MSDCPLL_CON3);
	DRV_WriteReg32(MSDCPLL_CON3, temp | 0x1); // [0]=1 (MSDCPLL_PWR_ON)

	//step 12
	temp = DRV_Reg32(MMPLL_CON3);
	DRV_WriteReg32(MMPLL_CON3, temp | 0x1); // [0]=1 (MMPLL_PWR_ON)
	//step 13
	temp = DRV_Reg32(APLL1_CON4);
	DRV_WriteReg32(APLL1_CON4, temp | 0x1); // [0]=1 (APLL1_PWR_ON)

	/* mt8168 */
	temp = DRV_Reg32(APLL2_CON4);
	DRV_WriteReg32(APLL2_CON4, temp | 0x1); // [0]=1 (APLL2_PWR_ON)

	temp = DRV_Reg32(LVDSPLL_CON3);
	DRV_WriteReg32(LVDSPLL_CON3, temp | 0x1); // [0]=1 (LVDSPLL_PWR_ON)

	temp = DRV_Reg32(DSPPLL_CON3);
	DRV_WriteReg32(DSPPLL_CON3, temp | 0x1); // [0]=1 (DSPPLL_PWR_ON)

	temp = DRV_Reg32(APUPLL_CON3);
	DRV_WriteReg32(APUPLL_CON3, temp | 0x1); // [0]=1 (APUPLL_PWR_ON)

	//step 14
	gpt_busy_wait_us(30); // Wait 30us
	/******************
	* xPLL ISO Eisable
	*******************/
	//step 15
	temp = DRV_Reg32(ARMPLL_CON3);
	DRV_WriteReg32(ARMPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (ARMPLL_ISO_EN)
	//step 18
	temp = DRV_Reg32(MFGPLL_CON3);
	DRV_WriteReg32(MFGPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (MFGPLL_ISO_EN)
	//step 19
	temp = DRV_Reg32(MPLL_CON3);
	DRV_WriteReg32(MPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (MPLL_ISO_EN)
	//step 20
	temp = DRV_Reg32(MAINPLL_CON3);
	DRV_WriteReg32(MAINPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (MAINPLL_ISO_EN)
	//step 21
	temp = DRV_Reg32(UNIVPLL_CON3);
	DRV_WriteReg32(UNIVPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (UNIVPLL_ISO_EN)
	//step 22
	temp = DRV_Reg32(MSDCPLL_CON3);
	DRV_WriteReg32(MSDCPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (MSDCPLL_ISO_EN)
	//step 23
	temp = DRV_Reg32(MMPLL_CON3);
	DRV_WriteReg32(MMPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (MMPLL_ISO_EN)
	//step 24
	temp = DRV_Reg32(APLL1_CON4);
	DRV_WriteReg32(APLL1_CON4, temp & 0xFFFFFFFD); // [1]=0 (APLL1_ISO_EN)

	/* mt8168 */
	//step 25
	temp = DRV_Reg32(APLL2_CON4);
	DRV_WriteReg32(APLL2_CON4, temp & 0xFFFFFFFD); // [1]=0 (APLL2_ISO_EN)

	temp = DRV_Reg32(LVDSPLL_CON3);
	DRV_WriteReg32(LVDSPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (LVDSPLL_ISO_EN)

	temp = DRV_Reg32(DSPPLL_CON3);
	DRV_WriteReg32(DSPPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (DSPPLL_ISO_EN)

	temp = DRV_Reg32(APUPLL_CON3);
	DRV_WriteReg32(APUPLL_CON3, temp & 0xFFFFFFFD); // [1]=0 (APUPLL_ISO_EN)

	gpt_busy_wait_us(1); // Wait 1us
	/********************
	* xPLL Frequency Set
	*********************/
	//step 26
	DRV_WriteReg32(ARMPLL_CON1, 0x811aec4e); // [25:0] (ARMPLL_N_INFO, 1400MHz)
	//step 27
	DRV_WriteReg32(MFGPLL_CON1, 0x810f6276); // [21:0] (GPUPLL_N_INFO, 800MHz)
	//step 28
	/* Keep MPLL as default 208Mhz */
	//step 29
	DRV_WriteReg32(MMPLL_CON1, 0x821193b1); // [21:0] (MMPLL_N_INFO, 457MHz)
	//step 30
	DRV_WriteReg32(APUPLL_CON1, 0x82180000); // [21:0] (APUPLL_N_INFO, 624MHz)
	DRV_WriteReg32(DSPPLL_CON1, 0x821713b1); // [21:0] (DSPPLL_N_INFO, 600MHz)

	//step 31
	temp = DRV_Reg32(CLK_MSDCCG_REG);
	DRV_WriteReg32(CLK_MSDCCG_REG, (temp | 0x00000e00)); // gate msdc module src clk
	DRV_WriteReg32(MSDCPLL_CON1, 0x820F0000); // [21:0] (MSDCPLL_N_INFO, 390MHz)
	DRV_WriteReg32(CLK_MSDCCG_REG, (temp & 0xfffff1ff)); // ungate msdc module src clk

	/***********************
	* xPLL Frequency Enable
	************************/
	//step 36
	temp = DRV_Reg32(ARMPLL_CON0);
	DRV_WriteReg32(ARMPLL_CON0, temp | 0x1); // [0]=1 (ARMPLL_EN)
	//step 39
	temp = DRV_Reg32(MFGPLL_CON0);
	DRV_WriteReg32(MFGPLL_CON0, temp | 0x1); // [0]=1 (GPUPLL_EN)
	//step 40
	temp = DRV_Reg32(MPLL_CON0);
	DRV_WriteReg32(MPLL_CON0, temp | 0x1); // [0]=1 (MPLL_EN)
	//step 41
	temp = DRV_Reg32(MAINPLL_CON0);
	DRV_WriteReg32(MAINPLL_CON0, temp | 0x1); // [0]=1 (MAINPLL_EN)
	//step 42
	temp = DRV_Reg32(UNIVPLL_CON0);
	DRV_WriteReg32(UNIVPLL_CON0, temp | 0x1); // [0]=1 (UNIVPLL_EN)
	//step 43
	temp = DRV_Reg32(MSDCPLL_CON0);
	DRV_WriteReg32(MSDCPLL_CON0, temp | 0x1); // [0]=1 (MSDCPLL_EN)
	//step 44
	temp = DRV_Reg32(MMPLL_CON0);
	DRV_WriteReg32(MMPLL_CON0, temp | 0x1); // [0]=1 (MMPLL_EN)
	//step 45
	temp = DRV_Reg32(APLL1_CON0);
	DRV_WriteReg32(APLL1_CON0, temp | 0x1); // [0]=1 (APLL1_EN)

	/* mt8168 */
	temp = DRV_Reg32(APLL2_CON0);
	DRV_WriteReg32(APLL2_CON0, temp | 0x1); // [1]=0 (APLL2_EN)

	temp = DRV_Reg32(LVDSPLL_CON0);
	DRV_WriteReg32(LVDSPLL_CON0, temp | 0x1); // [1]=0 (LVDSPLL_EN)

	temp = DRV_Reg32(DSPPLL_CON0);
	DRV_WriteReg32(DSPPLL_CON0, temp | 0x1); // [1]=0 (DSPPLL_EN)

	temp = DRV_Reg32(APUPLL_CON0);
	DRV_WriteReg32(APUPLL_CON0, temp | 0x1); // [1]=0 (APUPLL_EN)

	temp = DRV_Reg32(ULPLL_CON1);
	DRV_WriteReg32(ULPLL_CON1, temp | 0x4); // [2]=1 (ULPLL_HPM_EN)

	/***************
	* xPLL DIV Enable
	****************/
	//step 46
	temp = DRV_Reg32(MAINPLL_CON0);
	DRV_WriteReg32(MAINPLL_CON0, temp | 0xFF000180); // [31:24] (MAINPLL_DIV_EN)
	//step 47
	temp = DRV_Reg32(UNIVPLL_CON0);
	DRV_WriteReg32(UNIVPLL_CON0, temp | 0xFF000080); // [31:24] (UNIVPLL_DIV_EN)
	// step 48
	gpt_busy_wait_us(20); // Wait PLL stable (20us)
	/***************
	* xPLL DIV RSTB
	****************/
	//step 49
	temp = DRV_Reg32(MAINPLL_CON0);
	DRV_WriteReg32(MAINPLL_CON0, temp | 0x00800000); // [23]=1 (MAINPLL_DIV_RSTB)
	//step 50
	temp = DRV_Reg32(UNIVPLL_CON0);
	DRV_WriteReg32(UNIVPLL_CON0, temp | 0x00800000); // [23]=1 (UNIVPLL_DIV_RSTB)
	// step 51
	gpt_busy_wait_us(20); // Wait PLL stable (20us)
	/*****************
	* xPLL HW Control
	******************/
	//step 52
	temp = DRV_Reg32(PLLON_CON0);
	// [6]=0  use HW delay mode (by_maindiv_dly),
	// [29]= 0 MPLL_RSTB_SEL, [30]=0 MAINPLL_RSTB_SEL(spm), [31]=0 ARMPLL_RSTB_SEL(spm)
	DRV_WriteReg32(PLLON_CON0, temp & 0x1111ffbf);
	/**************
	* INFRA CPU CLKMUX, CLK Div
	***************/
#ifdef SET_ARMPLL_DIV_EN
	/* 8: div1, A: div2, B: div4, 1D: div6 */
	temp = DRV_Reg32(MP0_PLL_DIV_CFG) ;
	DRV_WriteReg32(MP0_PLL_DIV_CFG, (temp & 0xFFC1FFFF) | (0x08 << 17)); // [21:17] divsel: CPU clock divide by 1
	temp = DRV_Reg32(MP1_PLL_DIV_CFG) ;
	DRV_WriteReg32(MP1_PLL_DIV_CFG, (temp & 0xFFC1FFFF) | (0x08 << 17)); // [21:17] divsel: CPU clock divide by 1
	temp = DRV_Reg32(BUS_PLL_DIV_CFG) ;
	DRV_WriteReg32(BUS_PLL_DIV_CFG, (temp & 0xFFC1FFFF) | (0x0A << 17)); // [21:17] divsel: CPU clock divide by 1 ?
#endif
#ifdef SET_ARMPLL_CLK_SRC
	/* 0: 26M,  1: armpll, 2:  Mainpll, 3:  Unipll */
	/*
	* if (MP_SYNC_DCM_CONFIG.mp0_sync_dcm_div_en==1) {
	*	x==1;
	* } else {
	*  	x inside {[0:1]};
	*}
	*/
	temp = DRV_Reg32(MP0_PLL_DIV_CFG);
	DRV_WriteReg32(MP0_PLL_DIV_CFG, (temp & 0xFFFFF9FF) | (0x01<<9)); // [10:9] muxsel: switch to PLL speed
	/*
	* if (MP_SYNC_DCM_CONFIG.mp1_sync_dcm_div_en==1) {
	*	x==1;
	* } else {
	*  	x inside {[0:1]};
	*}
	*/
	temp = DRV_Reg32(MP1_PLL_DIV_CFG);
	DRV_WriteReg32(MP1_PLL_DIV_CFG, (temp & 0xFFFFF9FF) | (0x01<<9)); // [10:9] muxsel: switch to PLL speed
	/*
	* if (MP_SYNC_DCM_CONFIG.cci_sync_dcm_div_en==1) {
	*	x==1;
	* } else if (gic_sync_dcm.gic_sync_dcm_en==1) {
	* 	x==1;
	* } else {
	*  	x inside {[0:1]};
	*}
	*/
	temp = DRV_Reg32(BUS_PLL_DIV_CFG);
	/* [10:9] muxsel: switch to PLL speed */
	DRV_WriteReg32(BUS_PLL_DIV_CFG, (temp & 0xFFFFF9FF) | (0x01 << 9));
#endif
	/************
	* TOP CLKMUX
	*************/
	/* config AXI clock first, axi=syspll_d7 */
	DRV_WriteReg32(CLK_CFG_0, 0x00000001);
	DRV_WriteReg32(CLK_CFG_UPDATE, 0x00000001);

	/* CLK_SCP_CFG_0: [0] = sc_26ck_off_en, [1] = sc_memck_off_en,
	 * [2] = sc_axick_off_en, [9] = sc_mac_26m_off_en
	 */
	temp = DRV_Reg32(CLK_SCP_CFG_0);
	DRV_WriteReg32(CLK_SCP_CFG_0, (temp | 0x207));

	/* CLK_SCP_CFG_1[22:20] => 1 */
	/* CLK_SCP_CFG_1[16] => 0 */
	temp = DRV_Reg32(CLK_SCP_CFG_1);
	DRV_WriteReg32(CLK_SCP_CFG_1, (temp & 0xFF8EFFFF) | (0x01 << 20) | (0x1));
	/* config other clocks */
	DRV_WriteReg32(CLK_CFG_0, 0x06010101);
	DRV_WriteReg32(CLK_CFG_1, 0x02020201);
	DRV_WriteReg32(CLK_CFG_2, 0x01010100);
	DRV_WriteReg32(CLK_CFG_3, 0x01020101);
	DRV_WriteReg32(CLK_CFG_4, 0x03010101);
	DRV_WriteReg32(CLK_CFG_5, 0x02010103);
	DRV_WriteReg32(CLK_CFG_6, 0x01030301);
	DRV_WriteReg32(CLK_CFG_7, 0x01020202);
	DRV_WriteReg32(CLK_CFG_8, 0x02010003); /* clk_dpi0_sel -> clk26m_ck */
	DRV_WriteReg32(CLK_CFG_9, 0x04020403);
	DRV_WriteReg32(CLK_CFG_10, 0x01010103); /* clk_gcpu_sel -> syspll_d3 */
	DRV_WriteReg32(CLK_CFG_11, 0x00000000);
	/* update all clocks except "fmem & axi"(dram) */
	DRV_WriteReg32(CLK_CFG_UPDATE, 0xFFFFFFFC);
	DRV_WriteReg32(CLK_CFG_UPDATE1, 0x00000FFF);

	/* TOPCKGEN */
	temp = DRV_Reg32(CLK_MISC_CFG_0);
	DRV_WriteReg32(CLK_MISC_CFG_0, temp | 0x00f13f78);
	temp = DRV_Reg32(CLK_AUDDIV_0);
	DRV_WriteReg32(CLK_AUDDIV_0, temp & ~(0x3F9FF));
	mt_pll_post_init();
	/* INFRA CG */
	DRV_WriteReg32(INFRA_TOPAXI_SI0_CTL,
		DRV_Reg32(INFRA_TOPAXI_SI0_CTL) | (0x80000000));
	DRV_WriteReg32(PERI_BUS_DCM_CTRL,
		DRV_Reg32(PERI_BUS_DCM_CTRL) & ~(0x00000004));

	DRV_WriteReg32(INFRA_BUS_DCM_CTRL,
		DRV_Reg32(INFRA_BUS_DCM_CTRL) | 0x00400000);
	print("INFRA_BUS_DCM_CTRL %x\n", DRV_Reg32(INFRA_BUS_DCM_CTRL));

	DRV_WriteReg32(MODULE_SW_CG_0_CLR, 0x9DFF877F);
	DRV_WriteReg32(MODULE_SW_CG_1_CLR, 0x879C7796);
	DRV_WriteReg32(MODULE_SW_CG_2_CLR, 0x3ffc87dd);
	DRV_WriteReg32(MODULE_SW_CG_3_CLR, 0x7FFFFFBF);
	DRV_WriteReg32(MODULE_SW_CG_4_CLR, 0x00000FFF);

	DRV_WriteReg32(MODULE_SW_CG_1_SET, 0x800);

	print("mtcmos Start..\n");
	spm_mtcmos_ctrl_dis(STA_POWER_ON);
#ifdef CLK_ALLON
	spm_mtcmos_ctrl_cam(STA_POWER_ON);
	spm_mtcmos_ctrl_ven(STA_POWER_ON);
	spm_mtcmos_ctrl_vde(STA_POWER_ON);
	spm_mtcmos_ctrl_mfg(STA_POWER_ON);
	spm_mtcmos_ctrl_audio(STA_POWER_ON);
	spm_mtcmos_ctrl_dsp_shut_down(STA_POWER_ON);
	spm_mtcmos_ctrl_apu_shut_down(STA_POWER_ON);
	spm_mtcmos_ctrl_conn(STA_POWER_ON);
#endif
	/* PERICFG */
	temp = DRV_Reg32(PERIAXI_SI0_CTL);
	DRV_WriteReg32(PERIAXI_SI0_CTL, temp | 0x80000000);

	/* MMSYS */
	DRV_WriteReg32(MMSYS_CG_CLR0, 0xFFFFFFFF);
	DRV_WriteReg32(MMSYS_CG_CLR1, 0x0000000F);
#ifdef CLK_ALLON
	/* MFGCFG */
	temp = DRV_Reg32(MFG_CG_CLR);
	DRV_WriteReg32(MFG_CG_CLR, temp & ~(0x1));
	temp = DRV_Reg32(MFG_MBIST_CFG);
	DRV_WriteReg32(MFG_MBIST_CFG, temp & ~(0x1000000));

	/* CAMSYS */
	DRV_WriteReg32(CAMSYS_CG_CLR, 0x00001fc1);

	/* VDEC */
	temp = DRV_Reg32(VDEC_CKEN_SET);
	DRV_WriteReg32(VDEC_CKEN_SET, temp | 0x1);
	temp = DRV_Reg32(VDEC_LARB3_CKEN_SET);
	DRV_WriteReg32(VDEC_LARB3_CKEN_SET, temp | 0x1);

	/* VENC */
	temp = DRV_Reg32(VENC_CG_CON);
	DRV_WriteReg32(VENC_CG_CON, temp | 0x110);
#endif
	print("Pll init Done!\n");
}
