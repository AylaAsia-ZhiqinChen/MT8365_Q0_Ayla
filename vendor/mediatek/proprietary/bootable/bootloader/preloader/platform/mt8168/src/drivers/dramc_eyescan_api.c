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

/** @file dramc_basic_api.c
 *  Basic DRAMC API implementation
 */

/* Include files */
#include "dramc_common.h"
#include "x_hal_io.h"
#include "pll.h"

/*-------------------------------------------------------------
 *-------------------------------------------------------------
 *----- HQA used interface. Porting for HQA/Eyescan test ----------
 *-------------------------------------------------------------
 *-------------------------------------------------------------
 */
#if (CONFIG_EYESCAN_LOG == 1)
#if defined(RELEASE)
unsigned char gEye_Scan_color_flag = 0;
unsigned char gCBT_EYE_Scan_flag = 0;
unsigned char gCBT_EYE_Scan_only_higheset_freq_flag = 1;
unsigned char gRX_EYE_Scan_flag = 0;
unsigned char gRX_EYE_Scan_only_higheset_freq_flag = 1;
unsigned char gTX_EYE_Scan_flag = 1;
unsigned char gTX_EYE_Scan_only_higheset_freq_flag = 1;
#else
unsigned char gEye_Scan_color_flag = 0;
unsigned char gCBT_EYE_Scan_flag = 1;
unsigned char gCBT_EYE_Scan_only_higheset_freq_flag = 1;
unsigned char gRX_EYE_Scan_flag = 1;
unsigned char gRX_EYE_Scan_only_higheset_freq_flag = 1;
unsigned char gTX_EYE_Scan_flag = 1;
unsigned char gTX_EYE_Scan_only_higheset_freq_flag = 1;
#endif
#else
/* normal w/o eye scan */
unsigned char gEye_Scan_color_flag = 0;
unsigned char gCBT_EYE_Scan_flag = 0;
unsigned char gCBT_EYE_Scan_only_higheset_freq_flag = 0;
unsigned char gRX_EYE_Scan_flag = 0;
unsigned char gRX_EYE_Scan_only_higheset_freq_flag = 0;
unsigned char gTX_EYE_Scan_flag = 0;
unsigned char gTX_EYE_Scan_only_higheset_freq_flag = 0;
#endif

extern unsigned short delay_cell_ps_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];

#if (CONFIG_FOR_HQA_TEST_USED == 1)
void HQA_measure_message_reset_all_data(DRAMC_CTX_T *p)
{
    U32 u1BitIdx, u1RankIdx, u1ChannelIdx;

    for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<CHANNEL_NUM; u1ChannelIdx++)
    {
        for(u1RankIdx=RANK_0; u1RankIdx<RANK_MAX; u1RankIdx++)
        {
            for (u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
            {
                gFinalRXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx] =0;
                gFinalTXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx] =0;
            }
        }
    }
}
#endif

#if (CONFIG_FOR_HQA_REPORT_USED == 1)
U8 gHQALog_flag = 0;

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

U32 DDRPhyFMeter(DRAMC_CTX_T *p)
{
	unsigned int reg, freq;

	/*
	 * fmem_ck_bfe_dcm_ch0	55
	 * fmem_ck_aft_dcm_ch0	56
	 * fmem_ck_bfe_dcm_ch1	57
	 * fmem_ck_aft_dcm_ch1	58
	 */
	// enable ck_bfe_dcm_en for freqmeter measure ddrphy clock, not needed for normal use
    reg = DRV_Reg32(Channel_A_PHY_AO_BASE_ADDRESS + 0x2a0);
    DRV_WriteReg32(Channel_A_PHY_AO_BASE_ADDRESS + 0x2a0, reg | (1 << 11));
	freq = mt_get_ckgen_freq(55);
	//print("fmem_ck_bfe_dcm_ch0: %d\n", freq);
	DRV_WriteReg32(Channel_A_PHY_AO_BASE_ADDRESS + 0x2a0, reg);

#if 0
    reg = DRV_Reg32(Channel_B_PHY_AO_BASE_ADDRESS + 0x2a0);
    DRV_WriteReg32(Channel_B_PHY_AO_BASE_ADDRESS + 0x2a0, reg | (1 << 11));
	freq = mt_get_ckgen_freq(57);
	print("fmem_ck_bfe_dcm_ch1: %d\n", freq);
	DRV_WriteReg32(Channel_B_PHY_AO_BASE_ADDRESS + 0x2a0, reg);
#endif

	return (get_div_mode(p) == DIV8_MODE) ? freq*4/1000 : freq*2/1000;
}
void print_EyeScanVcent_for_HQA_report_used(DRAMC_CTX_T *p, U8 print_type, U8 u1ChannelIdx, U8 u1RankIdx, U8 *EyeScanVcent, U8 EyeScanVcentUpperBound, U8 EyeScanVcentUpperBound_bit, U8 EyeScanVcentLowerBound, U8 EyeScanVcentLowerBound_bit)
{
    U32 uiCA, u1BitIdx;
    U16 *pCBTVref_Voltage_Table[VREF_VOLTAGE_TABLE_NUM];
    U16 *pTXVref_Voltage_Table[VREF_VOLTAGE_TABLE_NUM];
    U32 vddq;
    U8 shuffleIdx;
	//U8  CBTVrefRange;
   // U8  TXVrefRange;
    U8 u1CBTEyeScanEnable, u1RXEyeScanEnable, u1TXEyeScanEnable;

	mcSHOW_DBG_MSG(("\n"));


    u1CBTEyeScanEnable = (gCBT_EYE_Scan_flag==1 && ((gCBT_EYE_Scan_only_higheset_freq_flag==1 && p->frequency == LP4_HIGHEST_FREQ) || gCBT_EYE_Scan_only_higheset_freq_flag==0));
    u1RXEyeScanEnable = (gRX_EYE_Scan_flag==1 && ((gRX_EYE_Scan_only_higheset_freq_flag==1 && p->frequency == LP4_HIGHEST_FREQ) || gRX_EYE_Scan_only_higheset_freq_flag==0));
    u1TXEyeScanEnable = (gTX_EYE_Scan_flag==1 && ((gTX_EYE_Scan_only_higheset_freq_flag==1 && p->frequency == LP4_HIGHEST_FREQ) || gTX_EYE_Scan_only_higheset_freq_flag==0));

    shuffleIdx = DRAM_DFS_SHUFFLE_1;

	if (gHQALog_flag==1 && print_type==0)
	{
        if (u1CBTEyeScanEnable)
        {
            if (p->dram_type == TYPE_LPDDR4)
            {
                pCBTVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_0];
                pCBTVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_1];
            }
            if (p->dram_type == TYPE_LPDDR4X)
            {
                pCBTVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_0];
                pCBTVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_1];
            }

            //CBTVrefRange = (dram_mr.mr12_value[p->channel][p->rank][p->dram_fsp]>>6)&1;
            vddq=get_dram_voltage(p, DRAM_VOL_VDDQ); //mv

            mcSHOW_DBG_MSG(("\n\n\n[HQA] information for measurement, "));
            mcSHOW_DBG_MSG(("\tDram Data rate = %d\n",p->frequency*2));

            mcSHOW_DBG_MSG(("CBT Eye Scan Vcent Voltage\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"CBT_Final_Vref Vcent", 0,
				pCBTVref_Voltage_Table[EyeScanVcent[0]][EyeScanVcent[1]]*vddq/100, NULL);
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"CBT_VdlVWHigh_Upper Vcent", 0,
				pCBTVref_Voltage_Table[EyeScanVcent[2]][EyeScanVcent[3]]*vddq/100, NULL);
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"CBT_VdlVWHigh_Lower Vcent", 0,
				pCBTVref_Voltage_Table[EyeScanVcent[4]][EyeScanVcent[5]]*vddq/100, NULL);

            mcSHOW_DBG_MSG(("\n"));

            mcSHOW_DBG_MSG(("CBT Eye Scan Vcent_UpperBound window\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"CBT_Vcent_UpperBound_Window", 0, EyeScanVcentUpperBound, NULL);
            mcSHOW_DBG_MSG(("CBT Eye Scan Vcent_UpperBound_Window worse bit\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"CBT_Vcent_UpperBound_Window_bit", 0, EyeScanVcentUpperBound_bit, NULL);
            mcSHOW_DBG_MSG(("CBT Eye Scan Vcent_UpperBound Min Window(%%)\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"CBT_Vcent_UpperBound_Window(%)", 0,
				(EyeScanVcentUpperBound * 100 + (is_lp4_family(p)==1?63:31)) / (is_lp4_family(p)==1?64:32), NULL);
            mcSHOW_DBG_MSG(("CBT Eye Scan Vcent_UpperBound Min Window PASS/FAIL\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT4,
				"CBT_Vcent_UpperBound_Window_PF", 0, 0,
				(EyeScanVcentUpperBound * 100 + (is_lp4_family(p)==1?63:31)) / (is_lp4_family(p)==1?64:32) >= 25 ? "PASS" : "FAIL");

            mcSHOW_DBG_MSG(("\n"));


            mcSHOW_DBG_MSG(("CBT Eye Scan Vcent_LowerBound window\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"CBT_Vcent_LowerBound_Window", 0, EyeScanVcentLowerBound, NULL);
            mcSHOW_DBG_MSG(("CBT Eye Scan Vcent_LowerBound_Window worse bit\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"CBT_Vcent_LowerBound_Window_bit", 0, EyeScanVcentLowerBound_bit, NULL);
            mcSHOW_DBG_MSG(("CBT Eye Scan Vcent_UpperBound Min Window(%%)\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"CBT_Vcent_LowerBound_Window(%)", 0,
				(EyeScanVcentLowerBound * 100 + (is_lp4_family(p)==1?63:31)) / (is_lp4_family(p)==1?64:32), NULL);
            mcSHOW_DBG_MSG(("CBT Eye Scan Vcent_LowerBound Min Window PASS/FAIL\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT4,
				"CBT_Vcent_LowerBound_Window_PF", 0, 0, (EyeScanVcentLowerBound * 100 + (is_lp4_family(p)==1?63:31)) / (is_lp4_family(p)==1?64:32) >= 25 ? "PASS" : "FAIL");

            mcSHOW_DBG_MSG(("\n"));
            mcSHOW_DBG_MSG(("CA Eye Scan per_bit window(%%)\n"));
            for (uiCA=0; uiCA<CATRAINING_NUM_LP4; uiCA++)
            {
#if (CONFIG_EYESCAN_LOG == 1)
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_2,
	                "CA_Perbit_Window(%)", uiCA,
	                ((gEyeScan_WinSize[EyeScanVcent[0]*30+EyeScanVcent[1]][uiCA]) * 100 + 31) / 32, NULL);
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_2,
					"CA_Perbit_BestWindow(%)", uiCA,
					((gEyeScan_WinSize[EyeScanVcent[10+uiCA*2]*30+EyeScanVcent[10+uiCA*2+1]][uiCA]) * 100 + 31) / 32, NULL);
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_2,
					"CA_Perbit_Window_Upperbond(%)", uiCA,
					((gEyeScan_WinSize[EyeScanVcent[2]*30+EyeScanVcent[3]][uiCA]) * 100 + 31) / 32, NULL);
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_2,
					"CA_Perbit_Window_Lowerbond(%) ", uiCA,
					((gEyeScan_WinSize[EyeScanVcent[4]*30+EyeScanVcent[5]][uiCA]) * 100 + 31) / 32, NULL);
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_2,
					"CA_Perbit_Eye_Height", uiCA,
					(gEyeScan_ContinueVrefHeight[uiCA]-1)*6*vddq/1000, NULL);
//                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_2, "CA_Perbit_Eye_Area", uiCA, gEyeScan_TotalPassCount[uiCA]*1000*3*vddq/(32*DDRPhyFMeter(p)), NULL); //total count*1/32UI*(1/freq*10^6 ps)*(0.6%vddq)
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_2,
	                "CA_Perbit_Eye_Area", uiCA,
	                (gEyeScan_TotalPassCount[uiCA]*10*3*vddq/(32*DDRPhyFMeter(p)))*100, NULL); //total count*1/32UI*(1/freq*10^6 ps)*(0.6%vddq)
#endif
            }
        }
    }

	if (gHQALog_flag==1 && print_type==1)
	{
        if (u1RXEyeScanEnable)
        {
            mcSHOW_DBG_MSG(("\n\n\n[HQA] information for measurement, "));
            mcSHOW_DBG_MSG(("\tDram Data rate = %d\n",p->frequency*2));

            mcSHOW_DBG_MSG(("RX Eye Scan Vcent Voltage\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"RX_Final_Vref Vcent", 0, gRXVref_Voltage_Table_LP4[EyeScanVcent[0]], NULL);
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"RX_VdlVWHigh_Upper Vcent", 0, gRXVref_Voltage_Table_LP4[EyeScanVcent[1]], NULL);
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"RX_VdlVWHigh_Lower Vcent", 0, gRXVref_Voltage_Table_LP4[EyeScanVcent[2]], NULL);

            mcSHOW_DBG_MSG(("\n"));

            mcSHOW_DBG_MSG(("RX Eye Scan Vcent_UpperBound window\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"RX_Vcent_UpperBound_Window", 0, EyeScanVcentUpperBound, NULL);
            mcSHOW_DBG_MSG(("RX Eye Scan Vcent_UpperBound_Window worse bit\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"RX_Vcent_UpperBound_Window_bit", 0, EyeScanVcentUpperBound_bit, NULL);
            mcSHOW_DBG_MSG(("RX Eye Scan Vcent_UpperBound Min Window(%%)\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"RX_Vcent_UpperBound_Window(%)", 0,
				((EyeScanVcentUpperBound * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000, NULL);
            mcSHOW_DBG_MSG(("RX Eye Scan Vcent_UpperBound Min Window PASS/FAIL\n"));
	        hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT4,
				"RX_Vcent_UpperBound_Window_PF", 0, 0,
				((EyeScanVcentUpperBound * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000 >= 20 ? "PASS" : "FAIL");

            mcSHOW_DBG_MSG(("\n"));


            mcSHOW_DBG_MSG(("RX Eye Scan Vcent_LowerBound window\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"RX_Vcent_LowerBound_Window", 0, EyeScanVcentLowerBound, NULL);
            mcSHOW_DBG_MSG(("RX Eye Scan Vcent_LowerBound_Window worse bit\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"RX_Vcent_LowerBound_Window_bit", 0, EyeScanVcentLowerBound_bit, NULL);
            mcSHOW_DBG_MSG(("RX Eye Scan Vcent_UpperBound Min Window(%%)\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"RX_Vcent_LowerBound_Window(%)", 0,
				((EyeScanVcentLowerBound * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000, NULL);
            mcSHOW_DBG_MSG(("RX Eye Scan Vcent_LowerBound Min Window PASS/FAIL\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT4,
				"RX_Vcent_LowerBound_Window_PF", 0, 0,
				((EyeScanVcentLowerBound * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000 >= 20 ? "PASS" : "FAIL");


            mcSHOW_DBG_MSG(("\n"));
            mcSHOW_DBG_MSG(("RX Eye Scan per_bit window(%%)\n"));
            for (u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
            {
#if (CONFIG_EYESCAN_LOG == 1)
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
                	"RX_Perbit_Window(%)", u1BitIdx,
                	((gEyeScan_WinSize[EyeScanVcent[0]][u1BitIdx] * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000, NULL);
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
					"RX_Perbit_BestWindow(%)", u1BitIdx,
					((gEyeScan_WinSize[EyeScanVcent[10+u1BitIdx]][u1BitIdx] * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000, NULL);
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
					"RX_Perbit_Window_Upperbond(%)", u1BitIdx,
					((gEyeScan_WinSize[EyeScanVcent[1]][u1BitIdx] * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000, NULL);
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
					"RX_Perbit_Window_Lowerbond(%) ", u1BitIdx,
					((gEyeScan_WinSize[EyeScanVcent[2]][u1BitIdx] * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000, NULL);
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
					"RX_Perbit_Eye_Height", u1BitIdx,
					(gEyeScan_ContinueVrefHeight[u1BitIdx]-1)*1330/100, NULL); //RX vref height 1225 ~ 1440, use 1330mv
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
					"RX_Perbit_Eye_Area", u1BitIdx,
					gEyeScan_TotalPassCount[u1BitIdx]*delay_cell_ps_all[DRAM_DFS_SHUFFLE_1][CHANNEL_A]*133/1000, NULL); //total count*jitter metter delay cell*(1/freq*10^6 ps)*(1330mv)
#endif
            }
        }
    }

	if (gHQALog_flag==1 && print_type==2)
	{
        if (u1TXEyeScanEnable)
        {
            if (p->dram_type == TYPE_LPDDR4)
            {
                pTXVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_0];
                pTXVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_1];
            }
            if (p->dram_type == TYPE_LPDDR4X)
            {
                pTXVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_0];
                pTXVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_1];
            }
            if (p->dram_type == TYPE_PCDDR4)
            {
                pTXVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_DDR4[VREF_RANGE_0];
                pTXVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_DDR4[VREF_RANGE_1];
            }

            //TXVrefRange = (dram_mr.mr14_value[p->channel][p->rank][p->dram_fsp]>>6)&1;
            vddq=get_dram_voltage(p, DRAM_VOL_VDDQ); //mv

            mcSHOW_DBG_MSG(("\n\n\n[HQA] information for measurement, "));
            mcSHOW_DBG_MSG(("\tDram Data rate = %d\n",p->frequency*2));

            mcSHOW_DBG_MSG(("TX Eye Scan Vcent Voltage\n"));
	        hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"TX_Final_Vref Vcent", 0,
				pTXVref_Voltage_Table[EyeScanVcent[0]][EyeScanVcent[1]]*vddq/100, NULL);
	        hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"TX_VdlVWHigh_Upper Vcent", 0,
				pTXVref_Voltage_Table[EyeScanVcent[2]][EyeScanVcent[3]]*vddq/100, NULL);
	        hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"TX_VdlVWHigh_Lower Vcent", 0,
				pTXVref_Voltage_Table[EyeScanVcent[4]][EyeScanVcent[5]]*vddq/100, NULL);

            mcSHOW_DBG_MSG(("\n"));

            mcSHOW_DBG_MSG(("TX Eye Scan Vcent_UpperBound window\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"TX_Vcent_UpperBound_Window", 0, EyeScanVcentUpperBound, NULL);
            mcSHOW_DBG_MSG(("TX Eye Scan Vcent_UpperBound_Window worse bit\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"TX_Vcent_UpperBound_Window_bit", 0, EyeScanVcentUpperBound_bit, NULL);
            mcSHOW_DBG_MSG(("TX Eye Scan Vcent_UpperBound Min Window(%%)\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"TX_Vcent_UpperBound_Window(%)", 0, (EyeScanVcentUpperBound * 100 + 31) / 32, NULL);
            mcSHOW_DBG_MSG(("TX Eye Scan Vcent_UpperBound Min Window PASS/FAIL\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT4,
				"TX_Vcent_UpperBound_Window_PF", 0, 0,
				(EyeScanVcentUpperBound * 100 + 31) / 32 >= 20 ? "PASS" : "FAIL");

            mcSHOW_DBG_MSG(("\n"));

            mcSHOW_DBG_MSG(("TX Eye Scan Vcent_LowerBound window\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"TX_Vcent_LowerBound_Window", 0, EyeScanVcentLowerBound, NULL);
            mcSHOW_DBG_MSG(("TX Eye Scan Vcent_LowerBound_Window worse bit\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"TX_Vcent_LowerBound_Window_bit", 0, EyeScanVcentLowerBound_bit, NULL);
            mcSHOW_DBG_MSG(("TX Eye Scan Vcent_UpperBound Min Window(%%)\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
				"TX_Vcent_LowerBound_Window(%)", 0, (EyeScanVcentLowerBound * 100 + 31) / 32, NULL);
            mcSHOW_DBG_MSG(("TX Eye Scan Vcent_LowerBound Min Window PASS/FAIL\n"));
            hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT4,
				"TX_Vcent_LowerBound_Window_PF", 0, 0,
				(EyeScanVcentLowerBound * 100 + 31) / 32 >= 20 ? "PASS" : "FAIL");

            mcSHOW_DBG_MSG(("\n"));
            mcSHOW_DBG_MSG(("TX Eye Scan per_bit window(%%)\n"));
            for (u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
            {
#if (CONFIG_EYESCAN_LOG == 1)
		if (is_lp4_family(p)) {
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Window(%)", u1BitIdx,
				((gEyeScan_WinSize[EyeScanVcent[0]*30+EyeScanVcent[1]][u1BitIdx]) * 100 + 31) / 32, NULL);
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_BestWindow(%)", u1BitIdx,
				((gEyeScan_WinSize[EyeScanVcent[10+u1BitIdx*2]*30+EyeScanVcent[10+u1BitIdx*2+1]][u1BitIdx]) * 100 + 31) / 32, NULL);
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Window_Upperbond(%)", u1BitIdx,
				((gEyeScan_WinSize[EyeScanVcent[2]*30+EyeScanVcent[3]][u1BitIdx]) * 100 + 31) / 32, NULL);
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Window_Lowerbond(%) ", u1BitIdx,
				((gEyeScan_WinSize[EyeScanVcent[4]*30+EyeScanVcent[5]][u1BitIdx]) * 100 + 31) / 32, NULL);
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Eye_Height", u1BitIdx,
				(gEyeScan_ContinueVrefHeight[u1BitIdx]-1)*6*vddq/1000, NULL);
//				  hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1, "TX_Perbit_Eye_Area", u1BitIdx, gEyeScan_TotalPassCount[u1BitIdx]*1000*3*vddq/(32*DDRPhyFMeter()), NULL); //total count*1/32UI*(1/freq*10^6 ps)*(0.6%vddq)
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Eye_Area", u1BitIdx,
				(gEyeScan_TotalPassCount[u1BitIdx]*10*3*vddq/(32*DDRPhyFMeter(p)))*100, NULL); //total count*1/32UI*(1/freq*10^6 ps)*(0.6%vddq)
		}else if (p->dram_type == TYPE_PCDDR4) {
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Window(%)", u1BitIdx,
				((gEyeScan_WinSize[(1-EyeScanVcent[0])*23+EyeScanVcent[1]][u1BitIdx]) * 100 + 31) / 32, NULL);
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_BestWindow(%)", u1BitIdx,
				((gEyeScan_WinSize[(1-EyeScanVcent[10+u1BitIdx*2])*23+EyeScanVcent[10+u1BitIdx*2+1]][u1BitIdx]) * 100 + 31) / 32, NULL);
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Window_Upperbond(%)", u1BitIdx,
				((gEyeScan_WinSize[(1-EyeScanVcent[2])*23+EyeScanVcent[3]][u1BitIdx]) * 100 + 31) / 32, NULL);
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Window_Lowerbond(%) ", u1BitIdx,
				((gEyeScan_WinSize[(1-EyeScanVcent[4])*23+EyeScanVcent[5]][u1BitIdx]) * 100 + 31) / 32, NULL);
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Eye_Height", u1BitIdx,
				(gEyeScan_ContinueVrefHeight[u1BitIdx]-1)*6*vddq/1000, NULL);
//				  hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1, "TX_Perbit_Eye_Area", u1BitIdx, gEyeScan_TotalPassCount[u1BitIdx]*1000*3*vddq/(32*DDRPhyFMeter()), NULL); //total count*1/32UI*(1/freq*10^6 ps)*(0.6%vddq)
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0_1,
				"TX_Perbit_Eye_Area", u1BitIdx,
				(gEyeScan_TotalPassCount[u1BitIdx]*10*3*vddq/(32*DDRPhyFMeter(p)))*100, NULL); //total count*1/32UI*(1/freq*10^6 ps)*(0.6%vddq)
		}
#endif
            }
	}
    }

	mcSHOW_DBG_MSG(("\n"));
}
#endif



#if (CONFIG_FOR_HQA_REPORT_USED == 1)
/* Add for HQA log generater */
void hqa_log_message_for_report(DRAMC_CTX_T *p,
								U32 channel,
								U32 rank,
								enum hqa_report_format format,
								char *main_str,
								U32 byte_bit_idx,
								S32 value,
								char *ans_str)
{
	mcSHOW_DBG_MSG(("[HQALOG] %d %s", p->frequency * 2, main_str));

    if (format == HQA_REPORT_FORMAT1) {
		     mcSHOW_DBG_MSG(("%d", byte_bit_idx));
    }

    if (format == HQA_REPORT_FORMAT3) {
        mcSHOW_DBG_MSG((" Channel%d ", channel));
    } else if ((format != HQA_REPORT_FORMAT5) &&
    	(format != HQA_REPORT_FORMAT6)) {
        mcSHOW_DBG_MSG((" Channel%d Rank%d ", channel, rank));
    }

	switch (format) {
	case HQA_REPORT_FORMAT0:
		mcSHOW_DBG_MSG(("Byte%d %d\n", byte_bit_idx, value));
		break;
    case HQA_REPORT_FORMAT0_1:
		mcSHOW_DBG_MSG(("Bit%X %d\n", byte_bit_idx, value));
        break;
    case HQA_REPORT_FORMAT0_2:
		mcSHOW_DBG_MSG(("CA%x %d\n", byte_bit_idx, value));
        break;
    case HQA_REPORT_FORMAT1:
    case HQA_REPORT_FORMAT2:
    case HQA_REPORT_FORMAT3:
    case HQA_REPORT_FORMAT6:
		mcSHOW_DBG_MSG(("%d\n", value));
		break;
    case HQA_REPORT_FORMAT4:
    case HQA_REPORT_FORMAT5:
        mcSHOW_DBG_MSG((" %s\n", ans_str));
        break;
	}
}
#endif


#if (CONFIG_FOR_HQA_TEST_USED == 1)
U32 u4gVcore[4] = {0};
void print_HQA_measure_message(DRAMC_CTX_T *p)
{
    U32 uiCA, u1BitIdx, u1RankIdx, u1ChannelIdx;
    U32 min_ca_value[CHANNEL_NUM][RANK_MAX], min_ca_bit[CHANNEL_NUM][RANK_MAX];
    U32 min_rx_value[CHANNEL_NUM][RANK_MAX], min_tx_value[CHANNEL_NUM][RANK_MAX];
    U32 min_RX_DQ_bit[CHANNEL_NUM][RANK_MAX], min_TX_DQ_bit[CHANNEL_NUM][RANK_MAX];
    U8 shuffleIdx, local_channel_num=2;
    U8 print_imp_option[2]={0, 0};
    int i;

    mcSHOW_DBG_MSG(("\n\n\n[HQA] information for measurement, "));
    mcSHOW_DBG_MSG(("\tDram Data rate = %d\n",p->frequency*2));
    print_calibration_basic_info(p);
    mcSHOW_DBG_MSG(("[HQALOG] %d Frequency = %u\n", p->frequency*2, DDRPhyFMeter(p)));

    shuffleIdx = DRAM_DFS_SHUFFLE_1;

    if(is_lp4_family(p))
    {
        local_channel_num = p->support_channel_num;
    }
#if ENABLE_LP3_SW
    else
    {
        //LP3
        local_channel_num = 1;
    }
#endif

    for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
    {
        for(u1RankIdx=RANK_0; u1RankIdx<RANK_MAX; u1RankIdx++)
        {
            min_ca_value[u1ChannelIdx][u1RankIdx] = 0xffff;
            min_rx_value[u1ChannelIdx][u1RankIdx] = 0xffff;
            min_tx_value[u1ChannelIdx][u1RankIdx] = 0xffff;
            min_RX_DQ_bit[u1ChannelIdx][u1RankIdx] = 0xffff;
            min_TX_DQ_bit[u1ChannelIdx][u1RankIdx] = 0xffff;

            for (uiCA=0; uiCA< CATRAINING_NUM_LP4; uiCA++)
            {
                if (gFinalCBTCA[u1ChannelIdx][u1RankIdx][uiCA] < min_ca_value[u1ChannelIdx][u1RankIdx])
                {
                    min_ca_value[u1ChannelIdx][u1RankIdx] = gFinalCBTCA[u1ChannelIdx][u1RankIdx][uiCA];
                    min_ca_bit[u1ChannelIdx][u1RankIdx] = uiCA;
                }
            }

            for (u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
            {
                if (gFinalRXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx] < min_rx_value[u1ChannelIdx][u1RankIdx])
                {
                    min_rx_value[u1ChannelIdx][u1RankIdx] = gFinalRXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx];
                    min_RX_DQ_bit[u1ChannelIdx][u1RankIdx] = u1BitIdx;
                }
                if (gFinalTXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx] < min_tx_value[u1ChannelIdx][u1RankIdx])
                {
                    min_tx_value[u1ChannelIdx][u1RankIdx] = gFinalTXPerbitWin[u1ChannelIdx][u1RankIdx][u1BitIdx];
                    min_TX_DQ_bit[u1ChannelIdx][u1RankIdx] = u1BitIdx;
                }
            }
        }
    }


    if (p->support_rank_num==2)
    {
        //Preloader LP3 RX/TX only K Rank0, so Rank1 use Rank0's value
        if(!is_lp4_family(p))
        {
#ifndef LP3_DUAL_RANK_RX_K
            min_rx_value[0][1] = min_rx_value[0][0];
            min_RX_DQ_bit[0][1] = min_RX_DQ_bit[0][0];
#endif

#ifndef LP3_DUAL_RANK_TX_K
            min_tx_value[0][1] = min_tx_value[0][0];
            gFinalTXPerbitWin_min_max[0][1] = gFinalTXPerbitWin_min_max[0][0];
            min_TX_DQ_bit[0][1] = min_TX_DQ_bit[0][0];
#endif

            #if 0//(TX_PER_BIT_DELAY_CELL==0)
            gFinalTXPerbitWin_min_margin[0][1] = gFinalTXPerbitWin_min_margin[0][0];
            gFinalTXPerbitWin_min_margin_bit[0][1] = gFinalTXPerbitWin_min_margin_bit[0][0];
            #endif
        }
    }


#if defined(DRAM_HQA)
	mcSHOW_DBG_MSG(("[Read Voltage]\n"));
	mcSHOW_DBG_MSG(("[HQALOG] %d Vcore_HQA = %d\n", p->frequency*2, get_vcore_voltage(p)));

	if (is_lp4_family(p)) {
	    /* LPDDR4 */
	    mcSHOW_DBG_MSG(("[HQALOG] %d Vdram_HQA = %d\n", p->frequency*2, get_dram_voltage(p, DRAM_VOL_VDD2)));
	    mcSHOW_DBG_MSG(("[HQALOG] %d Vddq_HQA = %d\n", p->frequency*2, get_dram_voltage(p, DRAM_VOL_VDDQ)));
	    mcSHOW_DBG_MSG(("[HQALOG] %d Vdd1_HQA = %d\n", p->frequency*2, get_dram_voltage(p, DRAM_VOL_VDD1)));
	} else {
	    /* LPDDR3 */
		mcSHOW_DBG_MSG(("[HQALOG] %d Vdram_HQA = %d\n", p->frequency*2, get_dram_voltage(p, DRAM_VOL_VDDQ)));
	}
	mcSHOW_DBG_MSG(("\n"));
#endif

    /*
        [Impedance Calibration]

        term_option=0
        [HQALOG] Impedance term_option=0 DRVP 11
        [HQALOG] Impedance term_option=0 DRVN 7

        term_option=1
        [HQALOG] Impedance term_option=1 DRVP 13
        [HQALOG] Impedance term_option=1 ODTN 15
    */
    if (p->dram_type == TYPE_LPDDR4)
    {
        print_imp_option[1] = 1;
    }
    else if (p->dram_type == TYPE_LPDDR4X)
    {
        print_imp_option[0] = 1;
        print_imp_option[1] = 1;
    }
    else
    {
      //TYPE_LPDDR4P, TYPE_LPDDR3
      print_imp_option[0] = 1;
    }

#if (CONFIG_FOR_HQA_REPORT_USED == 1)
if (gHQALog_flag==1)
{
    mcSHOW_DBG_MSG(("[Impedance Calibration]\n"));
    for(i=0; i<2; i++)
    {
        mcSHOW_DBG_MSG(("term_option=%d\n", i));
        if (print_imp_option[i]==0 || (print_imp_option[i]==1 && p->odt_onoff==ODT_OFF && i==1) || (print_imp_option[i]==1 && p->odt_onoff==ODT_ON && i==0))
    {
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT5,
					i==0 ? "Impedance term_option=0 DRVP" : "Impedance term_option=1 DRVP",
					0, 0, "NA");
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT5,
					i==0 ? "Impedance term_option=0 DRVN" : "Impedance term_option=1 ODTN",
					0, 0, "NA");
    }
        else
    {
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT6,
					i==0 ? "Impedance term_option=0 DRVP" : "Impedance term_option=1 DRVP",
					0, dramc_imp_result[i][0], NULL);
                hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT6,
					i==0 ? "Impedance term_option=0 DRVN" : "Impedance term_option=1 ODTN",
					0, i==0 ? dramc_imp_result[i][1] :
					dramc_imp_result[i][3], NULL);
        }
    }
    mcSHOW_DBG_MSG(("\n"));
}
#endif





    mcSHOW_DBG_MSG(("\n[Cmd Bus Training window]\n"));
    if(is_lp4_family(p))
    {
        //mcSHOW_DBG_MSG(("VrefCA Range : %d\n", gCBT_VREF_RANGE_SEL));
        /*
         VrefCA
             [HQALOG] 1600 VrefCA Channel0 Rank0 32
             [HQALOG] 1600 VrefCA Channel0 Rank1 24
             [HQALOG] 1600 VrefCA Channel1 Rank0 26
             [HQALOG] 1600 VrefCA Channel1 Rank1 30
         */
        mcSHOW_DBG_MSG(("VrefCA\n"));
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d VrefCA Channel%d "
                                "Rank%d %d\n",
                                p->frequency*2,
                            u1ChannelIdx,
                                u1RankIdx,
                                gFinalCBTVrefCA[u1ChannelIdx][u1RankIdx]));
            }
        }
    }

#if 0//(SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_CBT)
    if(p->femmc_Ready==1 )
    {
        mcSHOW_DBG_MSG(("\n[Cmd Bus Training window bypass calibration]\n"));
    }
    else
#endif
    {
        /*
         CA_Window
             [HQALOG] 1600 CA_Window Channel0 Rank0 61(bit 2)
             [HQALOG] 1600 CA_Window Channel0 Rank1 62(bit 1)
             [HQALOG] 1600 CA_Window Channel1 Rank0 60(bit 5)
             [HQALOG] 1600 CA_Window Channel1 Rank1 60(bit 5)
         */
        mcSHOW_DBG_MSG(("CA_Window\n"));
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
if (gHQALog_flag==1)
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                    hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2,
						"CA_Window", 0, min_ca_value[u1ChannelIdx][u1RankIdx], NULL);
                    hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2,
						"CA_Window_bit", 0, min_ca_bit[u1ChannelIdx][u1RankIdx], NULL);
            }
        }
        mcSHOW_DBG_MSG(("\n"));
}
else
#endif
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d CA_Window Channel%d "
                                "Rank%d %d (bit %d)\n",
                                p->frequency*2,
                                u1ChannelIdx,
                                u1RankIdx,
                                min_ca_value[u1ChannelIdx][u1RankIdx], min_ca_bit[u1ChannelIdx][u1RankIdx]));
            }
        }
}

        /*
         CA Min Window(%)
             [HQALOG] 1600 CA_Window(%) Channel0 Rank0 96%(PASS)
             [HQALOG] 1600 CA_Window(%) Channel0 Rank1 97%(PASS)
             [HQALOG] 1600 CA_Window(%) Channel1 Rank0 94%(PASS)
             [HQALOG] 1600 CA_Window(%) Channel1 Rank1 94%(PASS)
         */
        mcSHOW_DBG_MSG(("CA Min Window(%%)\n"));
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
if (gHQALog_flag==1)
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "CA_Window(%)", 0, ((min_ca_value[u1ChannelIdx][u1RankIdx] * 100 + (is_lp4_family(p)==1?63:31)) / (is_lp4_family(p)==1?64:32)), NULL);
                hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT4, "CA_Window_PF", 0, 0, ((((min_ca_value[u1ChannelIdx][u1RankIdx] * 100 + (is_lp4_family(p)==1?63:31)) / (is_lp4_family(p)==1?64:32)) >= 30) ? "PASS" : "FAIL"));
            }
        }
        mcSHOW_DBG_MSG(("\n"));
}
else
#endif
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d CA_Window(%%) Channel%d "
                                "Rank%d %d%% (%s)\n",
                                p->frequency*2,
                                u1ChannelIdx,
                                u1RankIdx,
                                        ((min_ca_value[u1ChannelIdx][u1RankIdx] * 100 + (is_lp4_family(p)==1?63:31)) / (is_lp4_family(p)==1?64:32)),
                                    ((((min_ca_value[u1ChannelIdx][u1RankIdx] * 100 + (is_lp4_family(p)==1?63:31)) / (is_lp4_family(p)==1?64:32)) >= 30) ? "PASS" : "FAIL")));
            }
        }
}
    }
            mcSHOW_DBG_MSG(("\n"));




    /*
    [RX minimum per bit window]
    Delay cell measurement (/100ps)
    [HQALOG] 3200 delaycell 892
    */
    mcSHOW_DBG_MSG(("\n[RX minimum per bit window]\n"));
    mcSHOW_DBG_MSG(("Delaycell measurement(/100ps)\n"));
#if !defined(RELEASE) && (VENDER_JV_LOG==0)
    mcSHOW_DBG_MSG(("[HQALOG] %d delaycell %d\n",
                    p->frequency*2,
                    delay_cell_ps_all[shuffleIdx][CHANNEL_A]));
#endif
    /*
     VrefDQ
         [HQALOG] 1600 VrefRX Channel0 24
         [HQALOG] 1600 VrefRX Channel1 24
     */

    if(is_lp4_family(p))
    {
        if (p->enable_rx_scan_vref == ENABLE)
        {
            mcSHOW_DBG_MSG(("VrefRX\n"));
            if (gRX_EYE_Scan_flag==1)
            {
                for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
                {
                    for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
                    {
                        mcSHOW_DBG_MSG(("[HQALOG] %d VrefRX Channel%d Rank%d %d\n",
                                        p->frequency*2,
                                        u1ChannelIdx,
                                        u1RankIdx,
                                        final_rx_vref_dq[u1ChannelIdx][u1RankIdx]));
                    }
                }
            }
            else
            {
                for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
                {
                    mcSHOW_DBG_MSG(("[HQALOG] %d VrefRX Channel%d %d\n",
                                    p->frequency*2,
                                    u1ChannelIdx,
                                    final_rx_vref_dq[u1ChannelIdx][RANK_0]));
                }
            }
        }
        else
        {
            mcSHOW_DBG_MSG(("RX DQ Vref Scan : Disable\n"));
        }
    }

#if 0//(SUPPORT_SAVE_TIME_FOR_CALIBRATION )
    if(p->femmc_Ready==1 && ( p->Bypass_RXWINDOW))
 	{
        mcSHOW_DBG_MSG(("\n[RX minimum per bit window bypass calibration]\n"));
 	}
    else
#endif
    {
        /*
         RX_Window
             [HQALOG] 1600 RX_Window Channel0 Rank0 52(bit 2)
             [HQALOG] 1600 RX_Window Channel0 Rank1 52(bit 2)
             [HQALOG] 1600 RX_Window Channel1 Rank0 60(bit 12)
             [HQALOG] 1600 RX_Window Channel1 Rank1 62(bit 9)
         */
        mcSHOW_DBG_MSG(("RX_Window\n"));
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
if (gHQALog_flag==1)
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                    hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "RX_Window", 0, min_rx_value[u1ChannelIdx][u1RankIdx], NULL);
                    hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "RX_Window_bit", 0, min_RX_DQ_bit[u1ChannelIdx][u1RankIdx], NULL);
            }
        }
}
else
#endif
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d RX_Window Channel%d "
                                "Rank%d %d (bit %d)\n",
                                p->frequency*2,
                            u1ChannelIdx,
                                u1RankIdx,
                                min_rx_value[u1ChannelIdx][u1RankIdx], min_RX_DQ_bit[u1ChannelIdx][u1RankIdx]));
            }
        }
}

        /*
         RX Min Window(%)
             [HQALOG] 1600 RX_Window(%) Channel0 Rank0 43316/100ps(70%)(PASS)
             [HQALOG] 1600 RX_Window(%) Channel0 Rank1 43316/100ps(70%)(PASS)
             [HQALOG] 1600 RX_Window(%) Channel1 Rank0 49980/100ps(80%)(PASS)
             [HQALOG] 1600 RX_Window(%) Channel1 Rank1 51646/100ps(83%)(PASS)
         */
        mcSHOW_DBG_MSG(("RX Window(%%)\n"));
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
if (gHQALog_flag==1)
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                    hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "RX_Window(%)", 0, ((min_rx_value[u1ChannelIdx][u1RankIdx] * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000, NULL);
                    hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT4, "RX_Window_PF", 0, 0, (min_rx_value[u1ChannelIdx][u1RankIdx] * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) / 1000000 >= 40 ? "PASS" : "FAIL");
            }
        }
}
else
#endif
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d RX_Window(%%) Channel%d "
                                "Rank%d "
                                "%d/100ps (%d%%) (%s)\n",
                                p->frequency*2,
                                u1ChannelIdx,
                                u1RankIdx,
                                min_rx_value[u1ChannelIdx][u1RankIdx] * delay_cell_ps_all[shuffleIdx][u1ChannelIdx],
                                ((min_rx_value[u1ChannelIdx][u1RankIdx] * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000,
                                ((min_rx_value[u1ChannelIdx][u1RankIdx] * delay_cell_ps_all[shuffleIdx][u1ChannelIdx] * p->frequency * 2) + (1000000 - 1)) / 1000000 >= 40 ? "PASS" : "FAIL"));
            }
        }
}

        mcSHOW_DBG_MSG(("\n"));
    }






    /* [TX minimum per bit window]
     VrefDQ Range : 1
     VrefDQ
         [HQALOG] 1600 VrefTX Channel0 Rank0 30
         [HQALOG] 1600 VrefTX Channel0 Rank1 25
         [HQALOG] 1600 VrefTX Channel1 Rank0 24
         [HQALOG] 1600 VrefTX Channel1 Rank1 23
     */
    mcSHOW_DBG_MSG(("\n[TX minimum per bit window]\n"));
    if(is_lp4_family(p))
    {
        if (p->enable_tx_scan_vref == ENABLE)
        {
            mcSHOW_DBG_MSG(("VrefDQ Range : %d\n",(dram_mr.mr14_value[p->channel][p->rank][p->dram_fsp]>>6)&1));
            mcSHOW_DBG_MSG(("VrefDQ\n"));
            for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
            {
                for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
                {
                    mcSHOW_DBG_MSG(("[HQALOG] %d VrefDQ Channel%d "
                                    "Rank%d %d\n",
                                    p->frequency*2,
                                    u1ChannelIdx,
                                    u1RankIdx,
                                    gFinalTXVrefDQ[u1ChannelIdx][u1RankIdx]));
                }
            }
        }
        else
        {
            mcSHOW_DBG_MSG(("TX DQ Vref Scan : Disable\n"));
        }
    }
#if 0//(SUPPORT_SAVE_TIME_FOR_CALIBRATION )
    if(p->femmc_Ready==1 && (p->Bypass_TXWINDOW))
 	{
        mcSHOW_DBG_MSG(("\n[TX minimum per bit window bypass calibration]\n"));
 	}
    else
#endif
    {
        /*
         TX_Window
             [HQALOG] 1600 TX_Window Channel0 Rank0 25(bit 2)
             [HQALOG] 1600 TX_Window Channel0 Rank1 25(bit 2)
             [HQALOG] 1600 TX_Window Channel1 Rank0 22(bit 9)
             [HQALOG] 1600 TX_Window Channel1 Rank1 23(bit 9)
         */
        mcSHOW_DBG_MSG(("TX_Window\n"));
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
if (gHQALog_flag==1)
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                    hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "TX_Window", 0, gFinalTXPerbitWin_min_max[u1ChannelIdx][u1RankIdx], NULL);
                    hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "TX_Window_bit", 0, min_TX_DQ_bit[u1ChannelIdx][u1RankIdx], NULL);
            }
        }
}
else
#endif
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d TX_Window Channel%d "
                                "Rank%d %d (bit %d)\n",
                                p->frequency*2,
                                u1ChannelIdx,
                                u1RankIdx,
                                gFinalTXPerbitWin_min_max[u1ChannelIdx][u1RankIdx], min_TX_DQ_bit[u1ChannelIdx][u1RankIdx]));
            }
        }
}
#if 0//(TX_PER_BIT_DELAY_CELL==0)
        mcSHOW_DBG_MSG(("min DQ margin\n"));
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d min_DQ_margin Channel%d "
                                "Rank%d %d (bit %d)\n",
                                p->frequency*2,
                                u1ChannelIdx,
                                u1RankIdx,
                                gFinalTXPerbitWin_min_margin[u1ChannelIdx][u1RankIdx], gFinalTXPerbitWin_min_margin_bit[u1ChannelIdx][u1RankIdx]));
            }
        }
#endif


        /*
         TX Min Window(%)
             [HQALOG] 1600 TX_Window(%) Channel0 Rank0 79%(PASS)
             [HQALOG] 1600 TX_Window(%) Channel0 Rank1 79%(PASS)
             [HQALOG] 1600 TX_Window(%) Channel1 Rank0 69%(PASS)
             [HQALOG] 1600 TX_Window(%) Channel1 Rank1 72%(PASS)
         */
        mcSHOW_DBG_MSG(("TX Min Window(%%)\n"));
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
if (gHQALog_flag==1)
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT2, "TX_Window(%)", 0, (min_tx_value[u1ChannelIdx][u1RankIdx] * 100 + 31) / 32, NULL);
                hqa_log_message_for_report(p, u1ChannelIdx, u1RankIdx, HQA_REPORT_FORMAT4, "TX_Window_PF", 0, 0, (min_tx_value[u1ChannelIdx][u1RankIdx] * 100 + 31) / 32 >= 45 ? "PASS" : "FAIL");
            }
        }
}
else
#endif
{
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d TX_Window(%%) Channel%d "
                                "Rank%d %d%% (%s)\n",
                                p->frequency*2,
                                u1ChannelIdx,
                                u1RankIdx,
                               (min_tx_value[u1ChannelIdx][u1RankIdx] * 100 + 31) / 32,
                               (min_tx_value[u1ChannelIdx][u1RankIdx] * 100 + 31) / 32 >= 45 ? "PASS" : "FAIL"));
            }
        }
}

        mcSHOW_DBG_MSG(("\n"));
    }



        /*
            [Duty Calibration]
            CLK Duty Final Delay Cell
            [HQALOG] DUTY CLK_Final_Delay Channel0 0
            [HQALOG] DUTY CLK_Final_Delay Channel1 -2
        */
#if !defined(RELEASE) && (VENDER_JV_LOG==0)
    if (is_lp4_family(p))
    {
        mcSHOW_DBG_MSG(("[duty Calibration]\n"));
        mcSHOW_DBG_MSG(("CLK Duty Final Delay Cell\n"));
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
                mcSHOW_DBG_MSG(("[HQALOG] %d DUTY CLK_Final_Delay Channel%d %d\n", p->frequency*2, u1ChannelIdx, gFinalClkDuty[u1ChannelIdx]));
        }

        /*
            CLK Duty MAX
            [HQALOG] DUTY CLK_MAX Channel0 4765%(X100)
            [HQALOG] DUTY CLK_MAX Channel1 5212%(X100)
        */
        mcSHOW_DBG_MSG(("CLK Duty MAX\n"));
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
                if (gHQALog_flag==1)
                {
                    hqa_log_message_for_report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT3, "DUTY CLK_MAX", 0, gFinalClkDutyMinMax[u1ChannelIdx][1], NULL);
                }
                else
#endif
                {
                    mcSHOW_DBG_MSG(("[HQALOG] %d DUTY CLK_MAX Channel%d %d%%(X100)\n", p->frequency*2, u1ChannelIdx, gFinalClkDutyMinMax[u1ChannelIdx][1]));
                }
        }

        /*
            CLK Duty MIN
            [HQALOG] DUTY CLK_MIN Channel0 4565%(X100)
            [HQALOG] DUTY CLK_MIN Channel1 5012%(X100)
        */
        mcSHOW_DBG_MSG(("CLK Duty MIN\n"));
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
            if (gHQALog_flag==1)
            {
                hqa_log_message_for_report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT3, "DUTY CLK_MIN", 0, gFinalClkDutyMinMax[u1ChannelIdx][0], NULL);
                hqa_log_message_for_report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT3, "DUTY CLK_MAX-MIN", 0, gFinalClkDutyMinMax[u1ChannelIdx][1]-gFinalClkDutyMinMax[u1ChannelIdx][0], NULL);
            }
            else
#endif
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d DUTY CLK_MIN Channel%d %d%%(X100)\n", p->frequency*2, u1ChannelIdx, gFinalClkDutyMinMax[u1ChannelIdx][0]));
            }
        }

        mcSHOW_DBG_MSG(("\n"));
    }


    /*
        DQS Duty Final Delay Cell
        [HQALOG] DUTY DQS_Final_Delay Channel0 DQS0 0
        [HQALOG] DUTY DQS_Final_Delay Channel0 DQS1 1
        [HQALOG] DUTY DQS_Final_Delay Channel1 DQS0 -2
        [HQALOG] DUTY DQS_Final_Delay Channel1 DQS1 -1
    */
    if (is_lp4_family(p))
    {
        mcSHOW_DBG_MSG(("DQS Duty Final Delay Cell\n"));
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
            if (p->frequency == LP4_HIGHEST_FREQ)
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d DUTY DQS_Final_Delay Channel%d DQS0 %d\n", p->frequency*2, u1ChannelIdx, gFinalDQSDuty[u1ChannelIdx][0]));
                mcSHOW_DBG_MSG(("[HQALOG] %d DUTY DQS_Final_Delay Channel%d DQS1 %d\n", p->frequency*2, u1ChannelIdx, gFinalDQSDuty[u1ChannelIdx][1]));
            }
        }

        /*
            DQS Duty MAX
            [HQALOG] DUTY DQS_MAX Channel0 DQS0 4765%(X100)
            [HQALOG] DUTY DQS_MAX Channel0 DQS1 5212%(X100)
            [HQALOG] DUTY DQS_MAX Channel1 DQS0 4765%(X100)
            [HQALOG] DUTY DQS_MAX Channel1 DQS1 5212%(X100)
        */
        mcSHOW_DBG_MSG(("DQS Duty MAX\n"));
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
                if (gHQALog_flag==1)
                {
                    hqa_log_message_for_report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_MAX", 0, gFinalDQSDutyMinMax[u1ChannelIdx][0][1], NULL);
                    hqa_log_message_for_report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_MAX", 1, gFinalDQSDutyMinMax[u1ChannelIdx][1][1], NULL);
                }
                else
#endif
                {
                    mcSHOW_DBG_MSG(("[HQALOG] %d DUTY DQS_MAX Channel%d DQS0 %d%%(X100)\n", p->frequency*2, u1ChannelIdx, gFinalDQSDutyMinMax[u1ChannelIdx][0][1]));
                    mcSHOW_DBG_MSG(("[HQALOG] %d DUTY DQS_MAX Channel%d DQS1 %d%%(X100)\n", p->frequency*2, u1ChannelIdx, gFinalDQSDutyMinMax[u1ChannelIdx][1][1]));
                }
        }

        /*
            DQS Duty MIN
            [HQALOG] DUTY DQS_MIN Channel0 DQS0 4765%(X100)
            [HQALOG] DUTY DQS_MIN Channel0 DQS1 5212%(X100)
            [HQALOG] DUTY DQS_MIN Channel1 DQS0 4765%(X100)
            [HQALOG] DUTY DQS_MIN Channel1 DQS1 5212%(X100)
        */
        mcSHOW_DBG_MSG(("DQS Duty MIN\n"));
        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
            if (gHQALog_flag==1)
            {
                hqa_log_message_for_report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_MIN", 0, gFinalDQSDutyMinMax[u1ChannelIdx][0][0], NULL);
                hqa_log_message_for_report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_MIN", 1, gFinalDQSDutyMinMax[u1ChannelIdx][1][0], NULL);
                hqa_log_message_for_report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_MAX-MIN", 0, gFinalDQSDutyMinMax[u1ChannelIdx][0][1]-gFinalDQSDutyMinMax[u1ChannelIdx][0][0], NULL);
                hqa_log_message_for_report(p, u1ChannelIdx, 0, HQA_REPORT_FORMAT0, "DUTY DQS_MAX-MIN", 1, gFinalDQSDutyMinMax[u1ChannelIdx][1][1]-gFinalDQSDutyMinMax[u1ChannelIdx][1][0], NULL);
            }
            else
#endif
            {
                mcSHOW_DBG_MSG(("[HQALOG] %d DUTY DQS_MIN Channel%d DQS0 %d%%(X100)\n", p->frequency*2, u1ChannelIdx, gFinalDQSDutyMinMax[u1ChannelIdx][0][0]));
                mcSHOW_DBG_MSG(("[HQALOG] %d DUTY DQS_MIN Channel%d DQS1 %d%%(X100)\n", p->frequency*2, u1ChannelIdx, gFinalDQSDutyMinMax[u1ChannelIdx][1][0]));
            }
        }

        mcSHOW_DBG_MSG(("\n"));
    }
#endif

    #if (CONFIG_ENABLE_MIOCK_JMETER == 1)
    //if(p->frequency == u2DFSGetHighestFreq(p))
    {
        if(is_lp4_family(p))	//LP4 Series
        {
            mcSHOW_DBG_MSG(("\n[DramcMiockJmeter]\n"
                        "Channel\tVCORE\t\t1 delay cell\n"));

            for(shuffle_index=DRAM_DFS_SHUFFLE_1; shuffle_index<DRAM_DFS_SHUFFLE_MAX; shuffle_index++)
            {
                mcSHOW_DBG_MSG(("\nSHUFFLE %d\n", shuffle_index+1));

                for (u1ChannelIdx = 0; u1ChannelIdx < local_channel_num; u1ChannelIdx++)
                {
                    mcSHOW_DBG_MSG(("CH%d\t%d\t\t%d/100 ps\n", u1ChannelIdx, u4gVcore[shuffle_index], delay_cell_ps_all[shuffle_index][u1ChannelIdx]));
                }
            }
        }
    }
    #endif

    mcSHOW_DBG_MSG(("\n\n\n"));



#if VENDER_JV_LOG
    mcSHOW_DBG_MSG5(("\n\n\n[Summary] information for measurement\n"));
    //mcSHOW_DBG_MSG5(("\tDram Data rate = %d\n",p->frequency*2));
    vPrintCalibrationBasicInfo_ForJV(p);

    if(is_lp4_family(p))
     {
         mcSHOW_DBG_MSG5(("[Cmd Bus Training window]\n"));
         mcSHOW_DBG_MSG5(("VrefCA Range : %d\n", gCBT_VREF_RANGE_SEL));
#if CHANNEL_NUM==4
        mcSHOW_DBG_MSG5(("CHA_VrefCA_Rank0   CHB_VrefCA_Rank0   CHC_VrefCA_Rank0    CHD_VrefCA_Rank0\n"));
        mcSHOW_DBG_MSG5(("%d                 %d                 %d                  %d\n", gFinalCBTVrefCA[0][0], gFinalCBTVrefCA[1][0], gFinalCBTVrefCA[2][0], gFinalCBTVrefCA[3][0]));
#else
         mcSHOW_DBG_MSG5(("CHA_VrefCA_Rank0   CHB_VrefCA_Rank0\n"));
         mcSHOW_DBG_MSG5(("%d                 %d\n", gFinalCBTVrefCA[0][0], gFinalCBTVrefCA[1][0]));
#endif
         mcSHOW_DBG_MSG5(("CHA_CA_window_Rank0   CHB_CA_winow_Rank0\n"));
         mcSHOW_DBG_MSG5(("%d%%(bit %d)              %d%%(bit %d) \n\n",(min_ca_value[0][0]*100+63)/64, min_ca_bit[0][0],
                                                                     (min_ca_value[1][0]*100+63)/64, min_ca_bit[1][0]));
     }
     else
     {
         mcSHOW_DBG_MSG5(("[CA Training window]\n"));
         mcSHOW_DBG_MSG5(("CHA_CA_win_Rank0\n"));
         mcSHOW_DBG_MSG5(("%d%%(bit %d)\n\n",(min_ca_value[0][0]*100+63)/64, min_ca_bit[0][0]));
     }

     mcSHOW_DBG_MSG5(("[RX minimum per bit window]\n"));
     if (p->enable_rx_scan_vref == ENABLE)
     {
#if CHANNEL_NUM==4
        mcSHOW_DBG_MSG5(("CHA_VrefDQ    CHB_VrefDQ      CHC_VrefDQ      CHD_VrefDQ\n"));
        mcSHOW_DBG_MSG5(("%d            %d              %d              %d \n", final_rx_vref_dq[CHANNEL_A][RANK_0], final_rx_vref_dq[CHANNEL_B][RANK_0], final_rx_vref_dq[CHANNEL_C][RANK_0], final_rx_vref_dq[CHANNEL_D][RANK_0]));
#else
         mcSHOW_DBG_MSG5(("CHA_VrefDQ   CHB_VrefDQ\n"));
         mcSHOW_DBG_MSG5(("%d                 %d \n", final_rx_vref_dq[CHANNEL_A][RANK_0], final_rx_vref_dq[CHANNEL_B][RANK_0]));
#endif
     }
     else
     {
         mcSHOW_DBG_MSG5(("RX DQ Vref Scan : Disable\n"));

     }

    if(is_lp4_family(p))
    {
#if CHANNEL_NUM==4
    mcSHOW_DBG_MSG5(("CHA_Rank0           CHA_Rank1           CHB_Rank0           CHB_Rank1         CHC_Rank0       CHC_Rank1       CHD_Rank0       CHD_Rank1\n"));
    mcSHOW_DBG_MSG5(("%d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)\n\n",
                                    ((min_rx_value[0][0]*delay_cell_ps_all[shuffleIdx][0]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[0][0],
                                    ((min_rx_value[0][1]*delay_cell_ps_all[shuffleIdx][0]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[0][1],
                                    ((min_rx_value[1][0]*delay_cell_ps_all[shuffleIdx][1]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[1][0],
                                    ((min_rx_value[1][1]*delay_cell_ps_all[shuffleIdx][1]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[1][1],
                                    ((min_rx_value[2][0]*delay_cell_ps_all[shuffleIdx][2]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[2][0],
                                    ((min_rx_value[2][1]*delay_cell_ps_all[shuffleIdx][2]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[2][1],
                                    ((min_rx_value[3][0]*delay_cell_ps_all[shuffleIdx][3]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[3][0],
                                    ((min_rx_value[3][1]*delay_cell_ps_all[shuffleIdx][3]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[3][1]));
#else
     mcSHOW_DBG_MSG5(("CHA_Rank0           CHA_Rank1           CHB_Rank0           CHB_Rank1\n"));
     mcSHOW_DBG_MSG5(("%d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)         %d%%(bit %d)\n\n",
                                     ((min_rx_value[0][0]*delay_cell_ps_all[shuffleIdx][0]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[0][0],
                                     ((min_rx_value[0][1]*delay_cell_ps_all[shuffleIdx][0]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[0][1],
                                     ((min_rx_value[1][0]*delay_cell_ps_all[shuffleIdx][1]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[1][0],
                                     ((min_rx_value[1][1]*delay_cell_ps_all[shuffleIdx][1]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[1][1]));
#endif
    }
    else
    {
        mcSHOW_DBG_MSG5(("CHA_Rank0           CHA_Rank1\n"));
        mcSHOW_DBG_MSG5(("%d%%(bit %d)         %d%%(bit %d)\n\n",
                                        ((min_rx_value[0][0]*delay_cell_ps_all[shuffleIdx][0]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[0][0],
                                        ((min_rx_value[0][1]*delay_cell_ps_all[shuffleIdx][0]*p->frequency*2)+(1000000-1))/1000000, min_RX_DQ_bit[0][1]));
    }


     mcSHOW_DBG_MSG5(("[TX minimum per bit window]\n"));
     if (p->enable_tx_scan_vref == ENABLE)
     {
         mcSHOW_DBG_MSG5(("VrefDQ Range : %d\n",(u1MR14Value[p->channel][p->rank][p->dram_fsp]>>6)&1));
#if CHANNEL_NUM==4
        mcSHOW_DBG_MSG5(("CHA_VrefDQ_Rank0   CHA_VrefDQ_Rank1    CHB_VrefDQ_Rank0    CHB_VrefDQ_Rank1   CHC_VrefDQ_Rank0    CHC_VrefDQ_Rank1    CHD_VrefDQ_Rank0    CHD_VrefDQ_Rank1\n"));
        mcSHOW_DBG_MSG5(("%d                 %d                  %d                  %d                 %d                  %d                  %d                  %d\n"
                    , gFinalTXVrefDQ[0][0], gFinalTXVrefDQ[0][1], gFinalTXVrefDQ[1][0], gFinalTXVrefDQ[1][1]
                    , gFinalTXVrefDQ[2][0], gFinalTXVrefDQ[2][1], gFinalTXVrefDQ[3][0], gFinalTXVrefDQ[3][1]
                    ));
#else
         mcSHOW_DBG_MSG5(("CHA_VrefDQ_Rank0   CHA_VrefDQ_Rank1    CHB_VrefDQ_Rank0    CHB_VrefDQ_Rank1\n"));
         mcSHOW_DBG_MSG5(("%d                  %d                   %d                   %d\n", gFinalTXVrefDQ[0][0], gFinalTXVrefDQ[0][1], gFinalTXVrefDQ[1][0], gFinalTXVrefDQ[1][1]));
#endif
     }
     else
     {
         mcSHOW_DBG_MSG5(("TX DQ Vref Scan : Disable\n"));
     }

    if(is_lp4_family(p))
    {
#if CHANNEL_NUM==4
    mcSHOW_DBG_MSG5(("CHA_Rank0         CHA_Rank1           CHB_Rank0           CHB_Rank1       CHC_Rank0       CHC_Rank1       CHD_Rank0   CHD_Rank1\n"));
    mcSHOW_DBG_MSG5(("%d%%               %d%%                 %d%%                 %d%%                 %d%%                 %d%%                 %d%%                 %d%%\n",
                                        (min_tx_value[0][0]*100+31)/32,
                                        (min_tx_value[0][1]*100+31)/32,
                                        (min_tx_value[1][0]*100+31)/32,
                                        (min_tx_value[1][1]*100+31)/32,
                                        (min_tx_value[2][0]*100+31)/32,
                                        (min_tx_value[2][1]*100+31)/32,
                                        (min_tx_value[3][0]*100+31)/32,
                                        (min_tx_value[3][1]*100+31)/32
                                        ));
#else
     mcSHOW_DBG_MSG5(("CHA_Rank0           CHA_Rank1           CHB_Rank0           CHB_Rank1\n"));
     mcSHOW_DBG_MSG5(("%d%%                %d%%                %d%%                %d%%\n",
                                         (min_tx_value[0][0]*100+31)/32,
                                         (min_tx_value[0][1]*100+31)/32,
                                         (min_tx_value[1][0]*100+31)/32,
                                         (min_tx_value[1][1]*100+31)/32));
#endif
    }
    else
    {
     mcSHOW_DBG_MSG5(("CHA_Rank0           CHA_Rank1\n"));
     mcSHOW_DBG_MSG5(("%d%%                %d%%\n",
                                         (min_tx_value[0][0]*100+31)/32,
                                         (min_tx_value[0][1]*100+31)/32));
    }
#endif


    // reset all data
    HQA_measure_message_reset_all_data(p);
}
#endif

#if (CONFIG_FOR_HQA_TEST_USED == 1)
unsigned short gFinalCBTVrefCA[CHANNEL_NUM][RANK_MAX];
unsigned short gFinalCBTCA[CHANNEL_NUM][RANK_MAX][10];
unsigned short gFinalRXPerbitWin[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH];
unsigned short gFinalTXPerbitWin[CHANNEL_NUM][RANK_MAX][DQ_DATA_WIDTH];
unsigned short gFinalTXPerbitWin_min_max[CHANNEL_NUM][RANK_MAX];
unsigned short gFinalTXPerbitWin_min_margin[CHANNEL_NUM][RANK_MAX];
unsigned short gFinalTXPerbitWin_min_margin_bit[CHANNEL_NUM][RANK_MAX];
signed char gFinalClkDuty[CHANNEL_NUM];
unsigned int gFinalClkDutyMinMax[CHANNEL_NUM][2];
signed char gFinalDQSDuty[CHANNEL_NUM][DQS_NUMBER];
unsigned int gFinalDQSDutyMinMax[CHANNEL_NUM][DQS_NUMBER][2];
U8 final_vref_eye_margin[CHANNEL_NUM][DQ_DATA_WIDTH];
U16 final_vref_value[CHANNEL_NUM];
#endif
unsigned char gFinalCBTVrefDQ[CHANNEL_NUM][RANK_MAX];
unsigned char gFinalTXVrefDQ[CHANNEL_NUM][RANK_MAX];
unsigned char gFinalTXVrefDQRange[CHANNEL_NUM][RANK_MAX];

#if (CONFIG_EYESCAN_LOG == 1)
S16  gEyeScan_Min[VREF_TOTAL_NUM_WITH_RANGE][DQ_DATA_WIDTH_LP4][EYESCAN_BROKEN_NUM];
S16  gEyeScan_Max[VREF_TOTAL_NUM_WITH_RANGE][DQ_DATA_WIDTH_LP4][EYESCAN_BROKEN_NUM];
U16  gEyeScan_CaliDelay[DQS_NUMBER];
U8  gEyeScan_WinSize[VREF_TOTAL_NUM_WITH_RANGE][DQ_DATA_WIDTH];
S8  gEyeScan_DelayCellPI[DQ_DATA_WIDTH];
U8  gEyeScan_ContinueVrefHeight[DQ_DATA_WIDTH];
U16  gEyeScan_TotalPassCount[DQ_DATA_WIDTH];
U8 gEyeScan_index[DQ_DATA_WIDTH];
U8 gu1pass_in_this_vref_flag[DQ_DATA_WIDTH];

const U16 gRXVref_Voltage_Table_LP4[RX_VREF_RANGE_END+1]={
        1363,
        2590,
        3815,
        5040,
        6264,
        7489,
        8714,
        9938,
        11160,
        12390,
        13610,
        14840,
        16060,
        17290,
        18510,
        19740,
        20670,
        22100,
        23530,
        24970,
        26400,
        27830,
        29260,
        30700,
        32130,
        33560,
        34990,
        36430,
        37860,
        39290,
        40720,
        42160
};

const U16 gVref_Voltage_Table_LP4X[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM]={
    {1500,1560,1620,1680,1740,1800,1860,1920,1980,2040,2100,2160,2220,2280,2340,2400,2460,2510,2570,2630,2690,2750,2810,2870,2930,2990,3050,3110,3170,3230,3290,3350,3410,3470,3530,3590,3650,3710,3770,3830,3890,3950,4010,4070,4130,4190,4250,4310,4370,4430,4490},
    {3290,3350,3410,3470,3530,3590,3650,3710,3770,3830,3890,3950,4010,4070,4130,4190,4250,4310,4370,4430,4490,4550,4610,4670,4730,4790,4850,4910,4970,5030,5090,5150,5210,5270,5330,5390,5450,5510,5570,5630,5690,5750,5810,5870,5930,5990,6050,6110,6170,6230,6290}
};

const U16 gVref_Voltage_Table_LP4[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM]={
    {1000,1040,1080,1120,1160,1200,1240,1280,1320,1360,1400,1440,1480,1520,1560,1600,1640,1680,1720,1760,1800,1840,1880,1920,1960,2000,2040,2080,2120,2160,2200,2240,2280,2320,2360,2400,2440,2480,2520,2560,2600,2640,2680,2720,2760,2800,2840,2880,2920,2960,3000},
    {2200,2240,2280,2320,2360,2400,2440,2480,2520,2560,2600,2640,2680,2720,2760,2800,2840,2880,2920,2960,3000,3040,3080,3120,3160,3200,3240,3280,3320,3360,3400,3440,3480,3520,3560,3600,3640,3680,3720,3760,3880,3840,3880,3920,3960,4000,4040,4080,4120,4160,4200}
};

const U16 gVref_Voltage_Table_DDR4[VREF_RANGE_MAX][VREF_VOLTAGE_TABLE_NUM]={
    {6000,6065,6130,6195,6260,6325,6390,6455,6520,6585,6650,6715,6780,6845,6910,6975,7040,7105,7170,7235,7300,7365,7430,7495,7560,7625,7690,7755,7820,7885,7950,8015,8080,8145,8210,8275,8340,8405,8470,8535,8600,8665,8730,8795,8860,8925,8990,9055,9120,9185,9250},
    {4500,4565,4630,4695,4760,4825,4890,4955,5020,5085,5150,5215,5280,5345,5410,5475,5540,5605,5670,5735,5800,5865,5930,5995,6060,6125,6190,6255,6320,6385,6450,6515,6580,6645,6710,6775,6840,6905,6970,7035,7100,7165,7230,7295,7360,7425,7490,7555,7620,7685,7750}
};

#define EyeScan_Pic_draw_line_Mirror 1
#define EysScan_Pic_draw_1UI_line 1

void EyeScan_Pic_draw_line(DRAMC_CTX_T *p, U8 draw_type, U8 u1VrefRange,
	U8 u1VrefIdx, U8 u1BitIdx, S16 u2DQDelayBegin, S16 u2DQDelayEnd,
	U8 u1FinalVrefRange, U16 Final_Vref_val, U8 VdlVWHigh_Upper_Vcent_Range,
	U32 VdlVWHigh_Upper_Vcent, U8 VdlVWHigh_Lower_Vcent_Range, U32 VdlVWHigh_Lower_Vcent,
	U16 FinalDQCaliDelay, S8 EyeScan_DelayCellPI_value, U16 delay_cell_ps,
	U16 Max_EyeScan_Min_val)
{
    int i;
    int local_VrefIdx, local_Upper_Vcent, local_Lower_Vcent, local_Final_VrefIdx;
    S8 EyeScan_Index;
    S16 EyeScan_Min_val, EyeScan_Max_val, Final_EyeScan_Min_val=EYESCAN_DATA_INVALID, Final_EyeScan_Max_val=EYESCAN_DATA_INVALID, Final_EyeScan_winsize=1;
    U16 *pVref_Voltage_Table[VREF_VOLTAGE_TABLE_NUM];
    U32 PI_of_1_UI;

    if (draw_type == 1)
    {
        pVref_Voltage_Table[VREF_RANGE_0]= (U16 *)gRXVref_Voltage_Table_LP4;
        if(p->delay_cell_timex100!=0)
        {
            PI_of_1_UI = (50000000/(p->frequency*p->delay_cell_timex100));

            FinalDQCaliDelay = (U16)EyeScan_DelayCellPI_value;
            EyeScan_DelayCellPI_value = 0;
        }
        else
        {
            PI_of_1_UI = 0;
            mcSHOW_ERR_MSG(("DelayCell is 0\n"));
        }
    }
    else
    {
        if (p->dram_type == TYPE_LPDDR4)
        {
            pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_0];
            pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_1];
        }
        if (p->dram_type == TYPE_LPDDR4X)
        {
            pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_0];
            pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_1];
        }
        if (p->dram_type == TYPE_PCDDR4)
        {
            pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_DDR4[VREF_RANGE_0];
            pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_DDR4[VREF_RANGE_1];
        }

        PI_of_1_UI = 32;
    }

    if (u1VrefRange==1 && u1VrefIdx <=20 && is_lp4_family(p))
    {
        u1VrefRange=0;
        u1VrefIdx += 30;
    } else if (u1VrefRange==0 && u1VrefIdx <=27 && p->dram_type==TYPE_PCDDR4 && (draw_type == 2)){
        u1VrefRange=1;
        u1VrefIdx += 23;
    }
    if (u1FinalVrefRange==1 && Final_Vref_val <=20 && is_lp4_family(p))
    {
        u1FinalVrefRange=0;
        Final_Vref_val += 30;
    } else if (u1FinalVrefRange==0 && Final_Vref_val <=27 && p->dram_type==TYPE_PCDDR4 && (draw_type == 2)){
        u1FinalVrefRange=1;
        Final_Vref_val += 23;
    }

    if (u1VrefRange != u1FinalVrefRange)
    {
        Final_Vref_val = 0xff;
    }
    local_Upper_Vcent = VdlVWHigh_Upper_Vcent_Range*VREF_VOLTAGE_TABLE_NUM+VdlVWHigh_Upper_Vcent;
    local_Lower_Vcent = VdlVWHigh_Lower_Vcent_Range*VREF_VOLTAGE_TABLE_NUM+VdlVWHigh_Lower_Vcent;
    local_VrefIdx = u1VrefRange*VREF_VOLTAGE_TABLE_NUM+u1VrefIdx;
    local_Final_VrefIdx = u1FinalVrefRange*VREF_VOLTAGE_TABLE_NUM+Final_Vref_val;

    if (VdlVWHigh_Upper_Vcent_Range==VREF_RANGE_1 && VdlVWHigh_Upper_Vcent<=20 && is_lp4_family(p)) local_Upper_Vcent = VdlVWHigh_Upper_Vcent_Range*VREF_VOLTAGE_TABLE_NUM+VdlVWHigh_Upper_Vcent-20;
    if (VdlVWHigh_Lower_Vcent_Range==VREF_RANGE_1 && VdlVWHigh_Lower_Vcent<=20 && is_lp4_family(p)) local_Lower_Vcent = VdlVWHigh_Lower_Vcent_Range*VREF_VOLTAGE_TABLE_NUM+VdlVWHigh_Lower_Vcent-20;
	if (VdlVWHigh_Upper_Vcent_Range==VREF_RANGE_0 && VdlVWHigh_Upper_Vcent<=27 && p->dram_type == TYPE_PCDDR4 && draw_type == 2) local_Upper_Vcent = VREF_VOLTAGE_TABLE_NUM+VdlVWHigh_Upper_Vcent+23;
    if (VdlVWHigh_Lower_Vcent_Range==VREF_RANGE_0 && VdlVWHigh_Lower_Vcent<=27 && p->dram_type == TYPE_PCDDR4 && draw_type == 2) local_Lower_Vcent = VREF_VOLTAGE_TABLE_NUM+VdlVWHigh_Lower_Vcent+23;

    mcSHOW_EYESCAN_MSG(("Vref-"));

    if (draw_type == 1 && u1VrefIdx <= 7)
    {
        mcSHOW_EYESCAN_MSG((" "));
    }

    mcSHOW_EYESCAN_MSG(("%d.%d%d",pVref_Voltage_Table[u1VrefRange][u1VrefIdx]/100, ((pVref_Voltage_Table[u1VrefRange][u1VrefIdx]%100)/10), pVref_Voltage_Table[u1VrefRange][u1VrefIdx]%10));

    if (draw_type == 1)
    {
        mcSHOW_EYESCAN_MSG(("m|"));
    }
    else
    {
        mcSHOW_EYESCAN_MSG(("%%|"));
    }




#if VENDER_JV_LOG || defined(RELEASE)
#if EyeScan_Pic_draw_line_Mirror
    EyeScan_DelayCellPI_value = 0-EyeScan_DelayCellPI_value;
#endif
#endif

#if EyeScan_Pic_draw_line_Mirror
    EyeScan_Index=EYESCAN_BROKEN_NUM-1;
    if (is_lp4_family(p) || ((p->dram_type == TYPE_PCDDR4) && (draw_type == 1))) {
	EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
	EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
    } else if (p->dram_type == TYPE_PCDDR4) {
	EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
	EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
    }
    while(EyeScan_Min_val==EYESCAN_DATA_INVALID && EyeScan_Index>0)
    {
        EyeScan_Index--;
        if (is_lp4_family(p) || ((p->dram_type == TYPE_PCDDR4) && (draw_type == 1))) {
		EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
		EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
	} else if (p->dram_type == TYPE_PCDDR4) {
		EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
		EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
	}
    }
#else
    EyeScan_Index=0;
    if (is_lp4_family(p) || ((p->dram_type == TYPE_PCDDR4) && (draw_type == 1))) {
	EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
	EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
    } else if (p->dram_type == TYPE_PCDDR4) {
	EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
	EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
    }
#endif

    if ((EyeScan_Max_val - EyeScan_Min_val + 1) > Final_EyeScan_winsize)
    {
#if EyeScan_Pic_draw_line_Mirror
        Final_EyeScan_Max_val = EyeScan_Max_val;
        Final_EyeScan_Min_val = EyeScan_Min_val;
#else
        Final_EyeScan_Max_val = EyeScan_Max_val;
        Final_EyeScan_Min_val = EyeScan_Min_val;
#endif
        Final_EyeScan_winsize =  (EyeScan_Max_val - EyeScan_Min_val + 1);
    }

#if VENDER_JV_LOG || defined(RELEASE)
#if EyeScan_Pic_draw_line_Mirror
    for(i=(Max_EyeScan_Min_val+PI_of_1_UI+EyeScan_DelayCellPI_value)*delay_cell_ps/100; i>(Max_EyeScan_Min_val+EyeScan_DelayCellPI_value)*delay_cell_ps/100; i-=10)
#else
    for(i=(Max_EyeScan_Min_val+EyeScan_DelayCellPI_value)*delay_cell_ps/100; i<(Max_EyeScan_Min_val+PI_of_1_UI+EyeScan_DelayCellPI_value)*delay_cell_ps/100; i+=10)
#endif
#else
#if EyeScan_Pic_draw_line_Mirror
    for(i=u2DQDelayEnd; i>=u2DQDelayBegin; i--)
#else
    for(i=u2DQDelayBegin; i<=u2DQDelayEnd; i++)
#endif
#endif
    {

#if VENDER_JV_LOG || defined(RELEASE)
#if EyeScan_Pic_draw_line_Mirror
        if (i<=((EyeScan_Min_val+EyeScan_DelayCellPI_value)*delay_cell_ps/100) && EyeScan_Index!= 0)
        {
            EyeScan_Index--;
            if (is_lp4_family(p) || ((p->dram_type == TYPE_PCDDR4) && (draw_type == 1))) {
		EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
		EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
            } else if (p->dram_type == TYPE_PCDDR4) {
		EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
		EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
	    }
            if ((EyeScan_Max_val - EyeScan_Min_val + 1) > Final_EyeScan_winsize)
            {
                Final_EyeScan_Max_val = EyeScan_Max_val;
                Final_EyeScan_Min_val = EyeScan_Min_val;
                Final_EyeScan_winsize =  (EyeScan_Max_val - EyeScan_Min_val + 1);
            }
        }
#endif
#else
#if EyeScan_Pic_draw_line_Mirror
        if (i==(EyeScan_Min_val) && EyeScan_Index!= 0)
        {
            EyeScan_Index--;
            if (is_lp4_family(p) || ((p->dram_type == TYPE_PCDDR4) && (draw_type == 1))) {
		EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
		EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+u1VrefRange*30][u1BitIdx][EyeScan_Index];
	    } else if (p->dram_type == TYPE_PCDDR4) {
		EyeScan_Min_val = gEyeScan_Min[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
		EyeScan_Max_val = gEyeScan_Max[u1VrefIdx+(1-u1VrefRange)*23][u1BitIdx][EyeScan_Index];
            }

            if ((EyeScan_Max_val - EyeScan_Min_val + 1) > Final_EyeScan_winsize)
            {
                Final_EyeScan_Max_val = EyeScan_Max_val;
                Final_EyeScan_Min_val = EyeScan_Min_val;
                Final_EyeScan_winsize =  (EyeScan_Max_val - EyeScan_Min_val + 1);
            }

        }
#endif
#endif

#if VENDER_JV_LOG || defined(RELEASE)
        if (i>=((EyeScan_Min_val+EyeScan_DelayCellPI_value)*delay_cell_ps/100) && i<=((EyeScan_Max_val+EyeScan_DelayCellPI_value)*delay_cell_ps/100))
#else
        if (i>=(EyeScan_Min_val) && i<=(EyeScan_Max_val))
#endif
        {
#if !VENDER_JV_LOG && !defined(RELEASE)
            if (i==FinalDQCaliDelay+EyeScan_DelayCellPI_value) //Final DQ delay
            {
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;105mH\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("H"));
                }
            }
            else
            if (local_VrefIdx==local_Final_VrefIdx) //Final Vref
            {
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;105mV\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("V"));
                }
#if (CONFIG_FOR_HQA_TEST_USED == 1)
                final_vref_eye_margin[p->channel][u1BitIdx] = EyeScan_Max_val - EyeScan_Min_val + 1;
                final_vref_value[p->channel] = pVref_Voltage_Table[u1VrefRange][u1VrefIdx];
#endif
            }
            else //spec in margin
            if (local_VrefIdx<=local_Upper_Vcent && local_VrefIdx>=local_Lower_Vcent && i>=(FinalDQCaliDelay+EyeScan_DelayCellPI_value-3) && i<=(FinalDQCaliDelay+EyeScan_DelayCellPI_value+3))
            {
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;103mQ\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("Q"));
                }
            }
            else //pass margin
#endif
            {
#if VENDER_JV_LOG || defined(RELEASE)
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;102mO\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("O"));
                }
#else
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;102mO\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("O"));
                }
#endif
            }
        }
        else
        {
#if !VENDER_JV_LOG && !defined(RELEASE)
#if EysScan_Pic_draw_1UI_line
            if (i==(int)(Max_EyeScan_Min_val) || i==(int)(Max_EyeScan_Min_val+PI_of_1_UI))
            {
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;107m.\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("."));
                }
            }
            else
#endif
#endif
            {
                //not valid
#if VENDER_JV_LOG || defined(RELEASE)
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;100m.\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("."));
                }
#else
                if (gEye_Scan_color_flag)
                {
                    mcSHOW_EYESCAN_MSG(("\033[0;100m.\033[m"));
                }
                else
                {
                    mcSHOW_EYESCAN_MSG(("."));
                }
#endif
            }
        }
    }


#if EyeScan_Pic_draw_line_Mirror
    if (Final_EyeScan_Min_val!=EYESCAN_DATA_INVALID && Final_EyeScan_Max_val!=EYESCAN_DATA_INVALID)
    {
#if !VENDER_JV_LOG && !defined(RELEASE)
        if (Final_EyeScan_Max_val>(FinalDQCaliDelay+EyeScan_DelayCellPI_value) && (FinalDQCaliDelay+EyeScan_DelayCellPI_value)>Final_EyeScan_Min_val)
        {
            mcSHOW_EYESCAN_MSG((" -%d ",(Final_EyeScan_Max_val-(FinalDQCaliDelay+EyeScan_DelayCellPI_value))));
            mcSHOW_EYESCAN_MSG(("%d ", ((FinalDQCaliDelay+EyeScan_DelayCellPI_value)-Final_EyeScan_Min_val)));
        }
        else if (Final_EyeScan_Max_val>(FinalDQCaliDelay+EyeScan_DelayCellPI_value) && Final_EyeScan_Min_val>(FinalDQCaliDelay+EyeScan_DelayCellPI_value))
        {
            mcSHOW_EYESCAN_MSG((" -%d ",(Final_EyeScan_Max_val-Final_EyeScan_Min_val)));
            mcSHOW_EYESCAN_MSG((" --- "));
        }
        else if ((FinalDQCaliDelay+EyeScan_DelayCellPI_value)>Final_EyeScan_Max_val && (FinalDQCaliDelay+EyeScan_DelayCellPI_value)>Final_EyeScan_Min_val)
        {
            mcSHOW_EYESCAN_MSG((" --- "));
            mcSHOW_EYESCAN_MSG(("%d ", (Final_EyeScan_Max_val-Final_EyeScan_Min_val)));
        }
        else
        {
            mcSHOW_EYESCAN_MSG((" --- "));
            mcSHOW_EYESCAN_MSG((" --- "));
        }
#endif
        //window
#if VENDER_JV_LOG || defined(RELEASE)
        mcSHOW_EYESCAN_MSG(("%dps", Final_EyeScan_winsize*delay_cell_ps/100));
#else
        mcSHOW_EYESCAN_MSG(("%d", Final_EyeScan_winsize));
#endif
    }
#else
    if (Final_EyeScan_Max_val != Final_EyeScan_Min_val && Final_EyeScan_Max_val!=EYESCAN_DATA_INVALID)
    {
#if !VENDER_JV_LOG && !defined(RELEASE)
        if (Final_EyeScan_Max_val>(FinalDQCaliDelay+EyeScan_DelayCellPI_value) && (FinalDQCaliDelay+EyeScan_DelayCellPI_value)>Final_EyeScan_Min_val)
        {
            mcSHOW_EYESCAN_MSG((" -%d ", ((FinalDQCaliDelay+EyeScan_DelayCellPI_value)-Final_EyeScan_Min_val)));
            mcSHOW_EYESCAN_MSG(("%d ",(Final_EyeScan_Max_val-(FinalDQCaliDelay+EyeScan_DelayCellPI_value))));
        }
        else if (Final_EyeScan_Max_val>(FinalDQCaliDelay+EyeScan_DelayCellPI_value) && Final_EyeScan_Min_val>(FinalDQCaliDelay+EyeScan_DelayCellPI_value))
        {
            mcSHOW_EYESCAN_MSG((" --- "));
            mcSHOW_EYESCAN_MSG(("%d ",(Final_EyeScan_Max_val-Final_EyeScan_Min_val)));
        }
        else if ((FinalDQCaliDelay+EyeScan_DelayCellPI_value)>Final_EyeScan_Max_val && (FinalDQCaliDelay+EyeScan_DelayCellPI_value)>Final_EyeScan_Min_val)
        {
            mcSHOW_EYESCAN_MSG((" -%d ", (Final_EyeScan_Max_val-Final_EyeScan_Min_val)));
            mcSHOW_EYESCAN_MSG((" --- "));
        }
        else
        {
            mcSHOW_EYESCAN_MSG((" --- "));
            mcSHOW_EYESCAN_MSG((" --- "));
        }
#endif
        //window
#if VENDER_JV_LOG || defined(RELEASE)
        mcSHOW_EYESCAN_MSG(("%dps", Final_EyeScan_winsize*delay_cell_ps/100));
#else
        mcSHOW_EYESCAN_MSG(("%d", Final_EyeScan_winsize));
#endif
    }
#endif

    mcSHOW_EYESCAN_MSG(("\n"));


}

void print_EYESCAN_LOG_message(DRAMC_CTX_T *p, U8 print_type)
{
    U32 u1ChannelIdx=p->channel, u1RankIdx=p->rank;
    S8 u1VrefIdx;
    U8 u1VrefRange;
    U8 u1BitIdx, u1CA;
    U32 VdlVWTotal, /* VdlVWLow, VdlVWHigh, */Vcent_DQ;
    U32 VdlVWHigh_Upper_Vcent=VREF_VOLTAGE_TABLE_NUM-1, VdlVWHigh_Lower_Vcent=0, VdlVWBest_Vcent=0;
    U32 VdlVWHigh_Upper_Vcent_Range=1, VdlVWHigh_Lower_Vcent_Range=0, VdlVWBest_Vcent_Range=1;;
    U8 Upper_Vcent_pass_flag=0, Lower_Vcent_pass_flag=0;
    S32 i, vrefrange_i;
    //U8 local_channel_num=2;
    U8 shuffleIdx, shuffleIdx2 = get_hqa_shuffle_idx(p);
    U8 TXVrefRange, CBTVrefRange;
    U32 vddq;
    U8 Min_Value_1UI_Line;
    S8 EyeScan_Index;
    U16 *pVref_Voltage_Table[VREF_VOLTAGE_TABLE_NUM];
    S8 EyeScan_DelayCellPI_value;
    U8 EyeScanVcent[10+DQ_DATA_WIDTH*2], max_winsize;
    U8 minCBTEyeScanVcentUpperBound=0xff, minCBTEyeScanVcentUpperBound_bit=0;
    U8 minCBTEyeScanVcentLowerBound=0xff, minCBTEyeScanVcentLowerBound_bit=0;
    U8 minRXEyeScanVcentUpperBound=0xff, minRXEyeScanVcentUpperBound_bit=0;
    U8 minRXEyeScanVcentLowerBound=0xff, minRXEyeScanVcentLowerBound_bit=0;
    U8 minTXEyeScanVcentUpperBound=0xff, minTXEyeScanVcentUpperBound_bit=0;
    U8 minTXEyeScanVcentLowerBound=0xff, minTXEyeScanVcentLowerBound_bit=0;
    U16 one_pi_ps=100000000/(p->frequency*2*32);
    U8 u1CBTEyeScanEnable, u1RXEyeScanEnable, u1TXEyeScanEnable;

    U16 u2DQDelayBegin, u2DQDelayEnd;//, u2TX_DQ_PreCal_LP4_Samll;

    if(is_lp4_family(p))
    {
    //    local_channel_num = p->support_channel_num;
    }
    else
    {
        //LP3
    //    local_channel_num = 1;
    }

    if (p->dram_type == TYPE_LPDDR4)
    {
        pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_0];
        pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4[VREF_RANGE_1];
    }
    if (p->dram_type == TYPE_LPDDR4X)
    {
        pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_0];
        pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_LP4X[VREF_RANGE_1];
    }

	if (p->dram_type == TYPE_PCDDR4)
	{
		pVref_Voltage_Table[VREF_RANGE_0] = (U16 *)gVref_Voltage_Table_DDR4[VREF_RANGE_0];
		pVref_Voltage_Table[VREF_RANGE_1] = (U16 *)gVref_Voltage_Table_DDR4[VREF_RANGE_1];
	}
	if (is_lp4_family(p)) {
		u1CBTEyeScanEnable = (gCBT_EYE_Scan_flag==1 && ((gCBT_EYE_Scan_only_higheset_freq_flag==1 && p->frequency == LP4_HIGHEST_FREQ) || gCBT_EYE_Scan_only_higheset_freq_flag==0));
		u1RXEyeScanEnable = (gRX_EYE_Scan_flag==1 && ((gRX_EYE_Scan_only_higheset_freq_flag==1 && p->frequency == LP4_HIGHEST_FREQ) || gRX_EYE_Scan_only_higheset_freq_flag==0));
		u1TXEyeScanEnable = (gTX_EYE_Scan_flag==1 && ((gTX_EYE_Scan_only_higheset_freq_flag==1 && p->frequency == LP4_HIGHEST_FREQ) || gTX_EYE_Scan_only_higheset_freq_flag==0));
	} else if (p->dram_type == TYPE_PCDDR4) {
		u1CBTEyeScanEnable = 0;
		u1RXEyeScanEnable = (gRX_EYE_Scan_flag==1 && ((gRX_EYE_Scan_only_higheset_freq_flag==1 && p->frequency == DDR4_HIGHEST_FREQ) || gRX_EYE_Scan_only_higheset_freq_flag==0));
		u1TXEyeScanEnable = (gTX_EYE_Scan_flag==1 && ((gTX_EYE_Scan_only_higheset_freq_flag==1 && p->frequency == DDR4_HIGHEST_FREQ) || gTX_EYE_Scan_only_higheset_freq_flag==0));
	}




/**************************************************************************************
    CBT EYESCAN log
***************************************************************************************/
    if (p->frequency <=934) VdlVWTotal = 17500; //VcIVW 175mv
    else if (p->frequency <= 1600) VdlVWTotal = 15500; //VcIVW 155mv
    else VdlVWTotal = 14500; //VcIVW 145mv

    CBTVrefRange = (dram_mr.mr12_value[p->channel][p->rank][p->dram_fsp]>>6)&1;

#if !VENDER_JV_LOG && !defined(RELEASE)
    if (print_type==0)
    if (u1CBTEyeScanEnable)
    {
        mcSHOW_DBG_MSG(("[EYESCAN_LOG] CBT Window\n"));
        vddq=get_dram_voltage(p, DRAM_VOL_VDDQ); //mv
        mcSHOW_DBG_MSG(("[EYESCAN_LOG] VDDQ=%dmV\n",vddq));
//        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
//            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                mcSHOW_DBG_MSG(("[EYESCAN_LOG] CBT Channel%d Range %d Final_Vref Vcent=%d(%dmV(X100))\n",
                                                        u1ChannelIdx,
                                                        CBTVrefRange,
                                                        gFinalCBTVrefDQ[u1ChannelIdx][u1RankIdx],
                                                        pVref_Voltage_Table[CBTVrefRange][gFinalCBTVrefDQ[u1ChannelIdx][u1RankIdx]]*vddq/100));

                Vcent_DQ = pVref_Voltage_Table[CBTVrefRange][gFinalCBTVrefDQ[u1ChannelIdx][u1RankIdx]]*vddq/100;

                //find VdlVWHigh first
                VdlVWHigh_Upper_Vcent_Range = 1;
                VdlVWHigh_Upper_Vcent = VREF_VOLTAGE_TABLE_NUM-1;
                vrefrange_i = CBTVrefRange;
                for(i=(gFinalCBTVrefDQ[u1ChannelIdx][u1RankIdx]); i<VREF_VOLTAGE_TABLE_NUM; i++)
                {
                    if (((pVref_Voltage_Table[vrefrange_i][i]*vddq/100 - Vcent_DQ)) >= VdlVWTotal/2)
                    {
                        /* find VdlVWHigh upper bound */
                        VdlVWHigh_Upper_Vcent = i;
                        VdlVWHigh_Upper_Vcent_Range = vrefrange_i;
                        break;
                    }
                    if (i==(VREF_VOLTAGE_TABLE_NUM-1) && vrefrange_i==0)
                    {
                        vrefrange_i=1;
                        i=20;
                    }
    }
                mcSHOW_DBG_MSG(("[EYESCAN_LOG] CBT VdlVWHigh_Upper Range=%d Vcent=%d(%dmV(X100))\n",
                    VdlVWHigh_Upper_Vcent_Range,
                    VdlVWHigh_Upper_Vcent,
                    pVref_Voltage_Table[VdlVWHigh_Upper_Vcent_Range][VdlVWHigh_Upper_Vcent]*vddq/100));

                //find VldVWLow first
                VdlVWHigh_Lower_Vcent_Range = 0;
                VdlVWHigh_Lower_Vcent = 0;
                vrefrange_i = CBTVrefRange;
                for(i=(gFinalCBTVrefDQ[u1ChannelIdx][u1RankIdx]); i>=0; i--)
                {
                    if (((Vcent_DQ - pVref_Voltage_Table[vrefrange_i][i]*vddq/100)) >= VdlVWTotal/2)
                    {
                        /* find VdlVWHigh lower bound */
                        VdlVWHigh_Lower_Vcent = i;
                        VdlVWHigh_Lower_Vcent_Range = vrefrange_i;
                        break;
                    }
                    if (i<=21 && vrefrange_i==1)
                    {
                        vrefrange_i=0;
                        i=VREF_VOLTAGE_TABLE_NUM-(21-i);
                    }
                }
                mcSHOW_DBG_MSG(("[EYESCAN_LOG] CBT VdlVWHigh_Lower Range=%d Vcent=%d(%dmV(X100))\n",
                    VdlVWHigh_Lower_Vcent_Range,
                    VdlVWHigh_Lower_Vcent,
                    pVref_Voltage_Table[VdlVWHigh_Lower_Vcent_Range][VdlVWHigh_Lower_Vcent]*vddq/100));

#ifdef FOR_HQA_TEST_USED
                EyeScanVcent[0] = CBTVrefRange;
                EyeScanVcent[1] = gFinalCBTVrefDQ[u1ChannelIdx][u1RankIdx];
                EyeScanVcent[2] = VdlVWHigh_Upper_Vcent_Range;
                EyeScanVcent[3] = VdlVWHigh_Upper_Vcent;
                EyeScanVcent[4] = VdlVWHigh_Lower_Vcent_Range;
                EyeScanVcent[5] = VdlVWHigh_Lower_Vcent;
#endif

                shuffleIdx = DRAM_DFS_SHUFFLE_1;

    //            mcSHOW_DBG_MSG(("[EYESCAN_LOG] delay cell %d/100ps\n", delay_cell_ps_all[shuffleIdx][u1ChannelIdx]));

                for (u1CA=0; u1CA<CATRAINING_NUM_LP4; u1CA++)
                {

                    // compare Upper/Lower Vcent pass criterion is pass or fail?
                    for(u1VrefIdx=gFinalCBTVrefDQ[u1ChannelIdx][u1RankIdx]+CBTVrefRange*30; u1VrefIdx<=(S8)(VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30); u1VrefIdx++)
                    {
                        Upper_Vcent_pass_flag = 0;
                        for (EyeScan_Index=0; EyeScan_Index<EYESCAN_BROKEN_NUM; EyeScan_Index++)
                        {
                            if ((((gEyeScan_CaliDelay[0]+gEyeScan_DelayCellPI[u1CA]) - gEyeScan_Min[u1VrefIdx][u1CA][EyeScan_Index]) >=4 ) && ((gEyeScan_Max[u1VrefIdx][u1CA][EyeScan_Index] - (gEyeScan_CaliDelay[0]+gEyeScan_DelayCellPI[u1CA])) >=4 ))
                            {
                                Upper_Vcent_pass_flag = 1;
                            }
                        }
                        if (Upper_Vcent_pass_flag == 0) break; // fail!!
                    }
                    for(u1VrefIdx=VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30; u1VrefIdx<=(S8)(gFinalCBTVrefDQ[u1ChannelIdx][u1RankIdx]+CBTVrefRange*30); u1VrefIdx++)
                    {
                        Lower_Vcent_pass_flag = 0;
                        for (EyeScan_Index=0; EyeScan_Index<EYESCAN_BROKEN_NUM; EyeScan_Index++)
                        {
                            if ((((gEyeScan_CaliDelay[0]+gEyeScan_DelayCellPI[u1CA]) - gEyeScan_Min[u1VrefIdx][u1CA][EyeScan_Index]) >=4 ) && ((gEyeScan_Max[u1VrefIdx][u1CA][EyeScan_Index] - (gEyeScan_CaliDelay[0]+gEyeScan_DelayCellPI[u1CA])) >=4 ))
                             {
                                Lower_Vcent_pass_flag = 1;
                            }
                        }
                        if (Lower_Vcent_pass_flag == 0) break; //fail!!
                    }

                    mcSHOW_DBG_MSG(("[EYESCAN_LOG] %d Channel%d Rank%d CA%d\tHigher VdlTW=%dPI(%d/100ps)(%s)\tLower VdlTW=%dpi(%d/100ps)(%s)\n",
                        p->frequency*2,
                        u1ChannelIdx,
                        u1RankIdx,
                        u1CA,
                        gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1CA],
                        gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1CA]*one_pi_ps,
                        Upper_Vcent_pass_flag==1 ? "PASS" : "FAIL",
                        gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1CA],
                        gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1CA]*one_pi_ps,
                        Lower_Vcent_pass_flag==1 ? "PASS" : "FAIL"
                        ));

#ifdef FOR_HQA_TEST_USED
                    //find VdlVWBest Vref Range and Vref
                    VdlVWBest_Vcent_Range = 1;
                    VdlVWBest_Vcent = VREF_VOLTAGE_TABLE_NUM-1;
                    vrefrange_i = 1;
                    max_winsize = 0;
                    for(i=VREF_VOLTAGE_TABLE_NUM-1; i>=0; i--)
                    {
                        if (gEyeScan_WinSize[i+vrefrange_i*30][u1CA] > max_winsize)
                        {
                            max_winsize = gEyeScan_WinSize[i+vrefrange_i*30][u1CA];
                            VdlVWBest_Vcent_Range = vrefrange_i;
                            VdlVWBest_Vcent = i;
                        }
                        if (i==21 && vrefrange_i==1)
                        {
                            vrefrange_i=0;
                            i=VREF_VOLTAGE_TABLE_NUM;
                        }
                    }

                    EyeScanVcent[10+u1CA*2] = VdlVWBest_Vcent_Range;
                    EyeScanVcent[10+u1CA*2+1] = VdlVWBest_Vcent;
#endif

                    if (gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1CA] < minCBTEyeScanVcentUpperBound)
                    {
                        minCBTEyeScanVcentUpperBound = gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1CA];
                        minCBTEyeScanVcentUpperBound_bit = u1CA;
                    }
                    if (gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1CA] < minCBTEyeScanVcentLowerBound)
                    {
                        minCBTEyeScanVcentLowerBound = gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1CA];
                        minCBTEyeScanVcentLowerBound_bit = u1CA;
                    }
                }
#if (CONFIG_FOR_HQA_TEST_USED == 1)
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
                print_EyeScanVcent_for_HQA_report_used(p, print_type, u1ChannelIdx, u1RankIdx, EyeScanVcent, minCBTEyeScanVcentUpperBound, minCBTEyeScanVcentUpperBound_bit, minCBTEyeScanVcentLowerBound, minCBTEyeScanVcentLowerBound_bit);
#endif
#endif
            }
        }
    }
#endif





    if (print_type==0)
    if (u1CBTEyeScanEnable)
    {
        mcSHOW_DBG_MSG(("\n\n"));

        for (u1CA=0; u1CA<CATRAINING_NUM_LP4; u1CA++)
        {
            EyeScan_Index = 0;

#if EyeScan_Pic_draw_line_Mirror
            EyeScan_DelayCellPI_value = 0-gEyeScan_DelayCellPI[u1CA];
#else
            EyeScan_DelayCellPI_value = gEyeScan_DelayCellPI[u1CA];
#endif
            Min_Value_1UI_Line = gEyeScan_CaliDelay[0]-16-EyeScan_DelayCellPI_value;


            mcSHOW_EYESCAN_MSG(("[EYESCAN_LOG] CBT EYESCAN Channel%d, Rank%d, CA%d ===\n",p->channel, p->rank, u1CA));

#if VENDER_JV_LOG || defined(RELEASE)
            for(i=0; i<8+one_pi_ps*32/1000; i++) mcSHOW_EYESCAN_MSG((" "));
            mcSHOW_EYESCAN_MSG(("window\n"));
#else
            for(i=0; i<8+one_pi_ps*32/1000; i++) mcSHOW_EYESCAN_MSG((" "));
            mcSHOW_EYESCAN_MSG(("first last window\n"));
#endif

            u1VrefRange=1;
            for (u1VrefIdx=VREF_VOLTAGE_TABLE_NUM-1; u1VrefIdx>=0; u1VrefIdx--)
            {

                    EyeScan_Pic_draw_line(p, 0, u1VrefRange, u1VrefIdx, u1CA, 0, 96, CBTVrefRange, gFinalCBTVrefDQ[u1ChannelIdx][u1RankIdx], VdlVWHigh_Upper_Vcent_Range, VdlVWHigh_Upper_Vcent, VdlVWHigh_Lower_Vcent_Range, VdlVWHigh_Lower_Vcent, gEyeScan_CaliDelay[0], gEyeScan_DelayCellPI[u1CA], one_pi_ps, Min_Value_1UI_Line);

                    if (u1VrefRange==VREF_RANGE_1 && u1VrefIdx==21)
                    {
                        u1VrefRange=VREF_RANGE_0;
                        u1VrefIdx=VREF_VOLTAGE_TABLE_NUM;
                    }
            }
            mcSHOW_EYESCAN_MSG(("\n\n"));

        }
    }





/**************************************************************************************
    RX EYESCAN log
***************************************************************************************/
    if (p->frequency <=1600) VdlVWTotal = 10000; //14000; //140mv
    else VdlVWTotal = 10000; //12000; //120mv

#if !VENDER_JV_LOG && !defined(RELEASE)
    if (print_type==1)
    if (u1RXEyeScanEnable)
    {
        mcSHOW_DBG_MSG(("[EYESCAN_LOG] RX Window\n"));
        {
            mcSHOW_DBG_MSG(("[EYESCAN_LOG] RX Final_Vref Vcent Channel%d %d(%dmV(X100))\n",
                                                    u1ChannelIdx,
                                                    final_rx_vref_dq[u1ChannelIdx][u1RankIdx],
                                                    gRXVref_Voltage_Table_LP4[final_rx_vref_dq[u1ChannelIdx][u1RankIdx]]));

            Vcent_DQ = gRXVref_Voltage_Table_LP4[final_rx_vref_dq[u1ChannelIdx][u1RankIdx]];

            //find VdlVWHigh first
            VdlVWHigh_Upper_Vcent_Range = 0;
            VdlVWHigh_Upper_Vcent = RX_VREF_RANGE_END;
            for(i=final_rx_vref_dq[u1ChannelIdx][u1RankIdx]; i<=RX_VREF_RANGE_END; i++)
            {
                if (gRXVref_Voltage_Table_LP4[i] - Vcent_DQ >= VdlVWTotal/2)
                {
                    /* find VdlVWHigh upper bound */
                    VdlVWHigh_Upper_Vcent = i;
                    break;
                }
            }
            mcSHOW_DBG_MSG(("[EYESCAN_LOG] RX VdlVWHigh_Upper Vcent=%d(%dmV(X100))\n", VdlVWHigh_Upper_Vcent, gRXVref_Voltage_Table_LP4[VdlVWHigh_Upper_Vcent]));

            //find VldVWLow first
            VdlVWHigh_Lower_Vcent_Range = 0;
            VdlVWHigh_Lower_Vcent = 0;
            for(i=final_rx_vref_dq[u1ChannelIdx][u1RankIdx]; i>=0; i--)
            {
                if (Vcent_DQ - gRXVref_Voltage_Table_LP4[i] >= VdlVWTotal/2)
                {
                    /* find VdlVWHigh lower bound */
                    VdlVWHigh_Lower_Vcent = i;
                    break;
                }
            }
            mcSHOW_DBG_MSG(("[EYESCAN_LOG] RX VdlVWHigh_Lower Vcent=%d(%dmV(X100))\n", VdlVWHigh_Lower_Vcent, gRXVref_Voltage_Table_LP4[VdlVWHigh_Lower_Vcent]));

#if (CONFIG_FOR_HQA_TEST_USED == 1)
            EyeScanVcent[0] = final_rx_vref_dq[u1ChannelIdx][u1RankIdx];
            EyeScanVcent[1] = VdlVWHigh_Upper_Vcent;
            EyeScanVcent[2] = VdlVWHigh_Lower_Vcent;
#endif

            shuffleIdx = DRAM_DFS_SHUFFLE_1;

            mcSHOW_DBG_MSG(("[EYESCAN_LOG] delay cell %d/100ps\n", delay_cell_ps_all[shuffleIdx2][u1ChannelIdx]));

            for (u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
            {
                mcSHOW_DBG_MSG(("[EYESCAN_LOG] %d Channel%d Bit%d(DRAM DQ%d)\tHigher VdlTW=%d/100ps\tLower VdlTW=%d/100ps\n",
                    p->frequency*2,
                    u1ChannelIdx,
                    u1BitIdx,
                    u1BitIdx,
                    gEyeScan_WinSize[VdlVWHigh_Upper_Vcent][u1BitIdx]*delay_cell_ps_all[shuffleIdx2][u1ChannelIdx],
                    gEyeScan_WinSize[VdlVWHigh_Lower_Vcent][u1BitIdx]*delay_cell_ps_all[shuffleIdx2][u1ChannelIdx]
                    ));

#if (CONFIG_FOR_HQA_TEST_USED == 1)
                    //find VdlVWBest Vref Range and Vref
                    VdlVWBest_Vcent = VREF_VOLTAGE_TABLE_NUM-1;
                    max_winsize = 0;
                    for(i=RX_VREF_RANGE_END; i>=0; i--)
                    {
                        if (gEyeScan_WinSize[i][u1BitIdx] > max_winsize)
                        {
                            max_winsize = gEyeScan_WinSize[i][u1BitIdx];
                            VdlVWBest_Vcent = i;
                        }
                    }

                    EyeScanVcent[10+u1BitIdx] = VdlVWBest_Vcent;
#endif

                if (gEyeScan_WinSize[VdlVWHigh_Upper_Vcent][u1BitIdx] < minRXEyeScanVcentUpperBound)
                {
                    minRXEyeScanVcentUpperBound = gEyeScan_WinSize[VdlVWHigh_Upper_Vcent][u1BitIdx];
                    minRXEyeScanVcentUpperBound_bit = u1BitIdx;
                }
                if (gEyeScan_WinSize[VdlVWHigh_Lower_Vcent][u1BitIdx] < minRXEyeScanVcentLowerBound)
                {
                    minRXEyeScanVcentLowerBound = gEyeScan_WinSize[VdlVWHigh_Lower_Vcent][u1BitIdx];
                    minRXEyeScanVcentLowerBound_bit = u1BitIdx;
                }
            }

#if (CONFIG_FOR_HQA_TEST_USED == 1)
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
            print_EyeScanVcent_for_HQA_report_used(p, print_type, u1ChannelIdx, u1RankIdx, EyeScanVcent, minRXEyeScanVcentUpperBound, minRXEyeScanVcentUpperBound_bit, minRXEyeScanVcentLowerBound, minRXEyeScanVcentLowerBound_bit);
#endif
#endif
        }
    }
#endif

    if (print_type==1)
    if (u1RXEyeScanEnable)
    {
        int drawend, drawbegin;

        mcSHOW_DBG_MSG(("\n\n"));

        for (u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
        {
            EyeScan_Index = 0;

            Min_Value_1UI_Line = gEyeScan_DelayCellPI[u1BitIdx]-16;

            mcSHOW_EYESCAN_MSG(("[EYESCAN_LOG] RX EYESCAN Channel%d, Rank%d, DQ%d ===\n",p->channel, p->rank, u1BitIdx));

#if VENDER_JV_LOG || defined(RELEASE)
            for(i=0; i<8+one_pi_ps*32/1000; i++) mcSHOW_EYESCAN_MSG((" "));
            mcSHOW_EYESCAN_MSG(("window\n"));
#else
            for(i=0; i<8+one_pi_ps*32/1000; i++) mcSHOW_EYESCAN_MSG((" "));
            mcSHOW_EYESCAN_MSG(("first last window\n"));
#endif

            drawbegin = -32 ;//gEyeScan_Min[final_rx_vref_dq[u1ChannelIdx][u1RankIdx]][u1BitIdx][EyeScan_Index]-5;
            drawend = 64;

            u1VrefRange=0;
            for (u1VrefIdx=RX_VREF_RANGE_END; u1VrefIdx>=0; u1VrefIdx--)
            {
//fra                    EyeScan_Pic_draw_line(p, 1, u1VrefRange, u1VrefIdx, u1BitIdx, p->odt_onoff==ODT_ON ? 0 : -32, 64, u1VrefRange, final_rx_vref_dq[u1ChannelIdx][u1RankIdx], VdlVWHigh_Upper_Vcent_Range, VdlVWHigh_Upper_Vcent, VdlVWHigh_Lower_Vcent_Range, VdlVWHigh_Lower_Vcent, gEyeScan_CaliDelay[u1BitIdx/8], gEyeScan_DelayCellPI[u1BitIdx], one_pi_ps, Min_Value_1UI_Line);
                    EyeScan_Pic_draw_line(p, 1, u1VrefRange, u1VrefIdx, u1BitIdx, drawbegin, drawend, u1VrefRange, final_rx_vref_dq[u1ChannelIdx][u1RankIdx], VdlVWHigh_Upper_Vcent_Range, VdlVWHigh_Upper_Vcent, VdlVWHigh_Lower_Vcent_Range, VdlVWHigh_Lower_Vcent, gEyeScan_CaliDelay[u1BitIdx/8], gEyeScan_DelayCellPI[u1BitIdx], one_pi_ps, Min_Value_1UI_Line);
            }
            mcSHOW_EYESCAN_MSG(("\n\n"));

        }
//fra while(1);
    }
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
    if (print_type==1) {
        mcSHOW_EYESCAN_MSG(("[HQALOG]RX EYESCAN Margin Channel%d ", p->channel));
        for(u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
            mcSHOW_EYESCAN_MSG(("%d ",final_vref_eye_margin[p->channel][u1BitIdx]));
        mcSHOW_EYESCAN_MSG(("\n[HQALOG]RX EYESCAN vref value Channel%d %d\n", p->channel, final_vref_value[p->channel]));
    }
#endif




/**************************************************************************************
    TX EYESCAN log
***************************************************************************************/
	if (is_lp4_family(p)){
            TXVrefRange = (dram_mr.mr14_value[p->channel][p->rank][p->dram_fsp]>>6)&1;
	} else if (p->dram_type == TYPE_PCDDR4) {
	    TXVrefRange = gFinalTXVrefDQRange[p->channel][p->rank];//(dram_mr.mr06_value[p->dram_fsp]>>6)&1;
	}

#if !VENDER_JV_LOG && !defined(RELEASE)
    if (print_type==2)
    if (u1TXEyeScanEnable)
    {   U8 cal_length;
        U16 finalTXVref;

    if (print_type==2)
    {
            mcSHOW_DBG_MSG(("[EYESCAN_LOG] TX DQ Window\n"));
            cal_length = p->data_width;
            finalTXVref = gFinalTXVrefDQ[u1ChannelIdx][u1RankIdx];
        }

        vddq=get_dram_voltage(p, DRAM_VOL_VDDQ); //mv
        mcSHOW_DBG_MSG(("[EYESCAN_LOG] VDDQ=%dmV\n",vddq));
//        for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<local_channel_num; u1ChannelIdx++)
        {
//            for(u1RankIdx = RANK_0; u1RankIdx < 2; u1RankIdx++)
            {
                if (print_type==2)
                {
                    mcSHOW_DBG_MSG(("[EYESCAN_LOG] TX Channel%d Range %d Final_Vref Vcent=%d(%dmV(X100))\n",
                                                        u1ChannelIdx,
                                                        TXVrefRange,
                                                        gFinalTXVrefDQ[u1ChannelIdx][u1RankIdx],
                                                        pVref_Voltage_Table[TXVrefRange][gFinalTXVrefDQ[u1ChannelIdx][u1RankIdx]]*vddq/100));

                    Vcent_DQ = pVref_Voltage_Table[TXVrefRange][gFinalTXVrefDQ[u1ChannelIdx][u1RankIdx]]*vddq/100;
                }

                //find VdlVWHigh first
                VdlVWHigh_Upper_Vcent_Range = 1;
                VdlVWHigh_Upper_Vcent = VREF_VOLTAGE_TABLE_NUM-1;
                vrefrange_i = TXVrefRange;
                for(i=(finalTXVref); i<VREF_VOLTAGE_TABLE_NUM; i++)
                {
                    if (((pVref_Voltage_Table[vrefrange_i][i]*vddq/100 - Vcent_DQ)) >= VdlVWTotal/2)
                    {
                        /* find VdlVWHigh upper bound */
                        VdlVWHigh_Upper_Vcent = i;
                        VdlVWHigh_Upper_Vcent_Range = vrefrange_i;
                        break;
                    }
                    if (i==(VREF_VOLTAGE_TABLE_NUM-1) && vrefrange_i==0 && is_lp4_family(p))
                    {
                        vrefrange_i=1;
                        i=20;
                    }
                    if (i==(VREF_VOLTAGE_TABLE_NUM-1) && vrefrange_i==1 && p->dram_type == TYPE_PCDDR4)
                    {
			vrefrange_i=0;
                        i=27;
                    }
                }
                mcSHOW_DBG_MSG(("[EYESCAN_LOG] TX VdlVWHigh_Upper Range=%d Vcent=%d(%dmV(X100))\n",
                    VdlVWHigh_Upper_Vcent_Range,
                    VdlVWHigh_Upper_Vcent,
                    pVref_Voltage_Table[VdlVWHigh_Upper_Vcent_Range][VdlVWHigh_Upper_Vcent]*vddq/100));

                //find VldVWLow first
                VdlVWHigh_Lower_Vcent_Range = 0;
                VdlVWHigh_Lower_Vcent = 0;
                vrefrange_i = TXVrefRange;
                for(i=(finalTXVref); i>=0; i--)
                {
                    if (((Vcent_DQ - pVref_Voltage_Table[vrefrange_i][i]*vddq/100)) >= VdlVWTotal/2)
                    {
                        /* find VdlVWHigh lower bound */
                        VdlVWHigh_Lower_Vcent = i;
                        VdlVWHigh_Lower_Vcent_Range = vrefrange_i;
                        break;
                    }
                    if (i<=21 && vrefrange_i==1 && is_lp4_family(p))
                    {
                        vrefrange_i=0;
                        i=VREF_VOLTAGE_TABLE_NUM-(21-i);
                    }
                    if (i<=28 && vrefrange_i==0 && p->dram_type == TYPE_PCDDR4)
                    {
                        vrefrange_i=1;
                        i=VREF_VOLTAGE_TABLE_NUM-(28-i);
                    }
                }
                mcSHOW_DBG_MSG(("[EYESCAN_LOG] TX VdlVWHigh_Lower Range=%d Vcent=%d(%dmV(X100))\n",
                    VdlVWHigh_Lower_Vcent_Range,
                    VdlVWHigh_Lower_Vcent,
                    pVref_Voltage_Table[VdlVWHigh_Lower_Vcent_Range][VdlVWHigh_Lower_Vcent]*vddq/100));

#if (CONFIG_FOR_HQA_TEST_USED == 1)
                EyeScanVcent[0] = TXVrefRange;
                EyeScanVcent[1] = gFinalTXVrefDQ[u1ChannelIdx][u1RankIdx];
                EyeScanVcent[2] = VdlVWHigh_Upper_Vcent_Range;
                EyeScanVcent[3] = VdlVWHigh_Upper_Vcent;
                EyeScanVcent[4] = VdlVWHigh_Lower_Vcent_Range;
                EyeScanVcent[5] = VdlVWHigh_Lower_Vcent;
#endif

                shuffleIdx = DRAM_DFS_SHUFFLE_1;

    //            mcSHOW_DBG_MSG(("[EYESCAN_LOG] delay cell %d/100ps\n", delay_cell_ps_all[shuffleIdx][u1ChannelIdx]));

                for (u1BitIdx=0; u1BitIdx<cal_length; u1BitIdx++)
                {

                    // compare Upper/Lower Vcent pass criterion is pass or fail?
#if 1
			if (is_lp4_family(p))
			{
				for(u1VrefIdx=finalTXVref+TXVrefRange*30; u1VrefIdx<=(S8)(VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30); u1VrefIdx++)
				{
					Upper_Vcent_pass_flag = 0;
					for (EyeScan_Index=0; EyeScan_Index<EYESCAN_BROKEN_NUM; EyeScan_Index++)
					{
						if (print_type==2)
							if ((((gEyeScan_CaliDelay[u1BitIdx/8]+gEyeScan_DelayCellPI[u1BitIdx]) - gEyeScan_Min[u1VrefIdx][u1BitIdx][EyeScan_Index]) >=4 ) && ((gEyeScan_Max[u1VrefIdx][u1BitIdx][EyeScan_Index] - (gEyeScan_CaliDelay[u1BitIdx/8]+gEyeScan_DelayCellPI[u1BitIdx])) >=4 ))
							{
								Upper_Vcent_pass_flag = 1;
							}
					}
						if (Upper_Vcent_pass_flag == 0) break; // fail!!
				}
				for(u1VrefIdx=VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30; u1VrefIdx<=(S8)(finalTXVref+TXVrefRange*30); u1VrefIdx++)
				{
					Lower_Vcent_pass_flag = 0;
					for (EyeScan_Index=0; EyeScan_Index<EYESCAN_BROKEN_NUM; EyeScan_Index++)
					{
						if (print_type==2)
							if ((((gEyeScan_CaliDelay[u1BitIdx/8]+gEyeScan_DelayCellPI[u1BitIdx]) - gEyeScan_Min[u1VrefIdx][u1BitIdx][EyeScan_Index]) >=4 ) && ((gEyeScan_Max[u1VrefIdx][u1BitIdx][EyeScan_Index] - (gEyeScan_CaliDelay[u1BitIdx/8]+gEyeScan_DelayCellPI[u1BitIdx])) >=4 ))
							{
								Lower_Vcent_pass_flag = 1;
							}
					}
					if (Lower_Vcent_pass_flag == 0) break; //fail!!
				}
			}
			if (p->dram_type == TYPE_PCDDR4)
			{
				for(u1VrefIdx=finalTXVref+(1-TXVrefRange)*23; u1VrefIdx<=(S8)(VdlVWHigh_Upper_Vcent+(1-VdlVWHigh_Upper_Vcent_Range)*23); u1VrefIdx++)
				{
					Upper_Vcent_pass_flag = 0;
					for (EyeScan_Index=0; EyeScan_Index<EYESCAN_BROKEN_NUM; EyeScan_Index++)
					{
						if (print_type==2)
							if ((((gEyeScan_CaliDelay[u1BitIdx/8]+gEyeScan_DelayCellPI[u1BitIdx]) - gEyeScan_Min[u1VrefIdx][u1BitIdx][EyeScan_Index]) >=4 ) && ((gEyeScan_Max[u1VrefIdx][u1BitIdx][EyeScan_Index] - (gEyeScan_CaliDelay[u1BitIdx/8]+gEyeScan_DelayCellPI[u1BitIdx])) >=4 ))
							{
								Upper_Vcent_pass_flag = 1;
							}
					}
					if (Upper_Vcent_pass_flag == 0) break; // fail!!
				}
				for(u1VrefIdx=VdlVWHigh_Lower_Vcent+(1-VdlVWHigh_Lower_Vcent_Range)*23; u1VrefIdx<=(S8)(finalTXVref+(1-TXVrefRange)*23); u1VrefIdx++)
				{
					Lower_Vcent_pass_flag = 0;
					for (EyeScan_Index=0; EyeScan_Index<EYESCAN_BROKEN_NUM; EyeScan_Index++)
					{
						if (print_type==2)
							if ((((gEyeScan_CaliDelay[u1BitIdx/8]+gEyeScan_DelayCellPI[u1BitIdx]) - gEyeScan_Min[u1VrefIdx][u1BitIdx][EyeScan_Index]) >=4 ) && ((gEyeScan_Max[u1VrefIdx][u1BitIdx][EyeScan_Index] - (gEyeScan_CaliDelay[u1BitIdx/8]+gEyeScan_DelayCellPI[u1BitIdx])) >=4 ))
							{
								Lower_Vcent_pass_flag = 1;
							}
					}
					if (Lower_Vcent_pass_flag == 0) break; //fail!!
				}
			}

#else
					Upper_Vcent_pass_flag = 0;
					Lower_Vcent_pass_flag = 0;
					for (EyeScan_Index=0; EyeScan_Index<EYESCAN_BROKEN_NUM; EyeScan_Index++)
					{
						if ((EyeScan_Min[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1BitIdx][EyeScan_Index] <= (EyeScan_CaliDelay[u1BitIdx/8]-4+EyeScan_DelayCellPI[u1BitIdx])) && ((EyeScan_CaliDelay[u1BitIdx/8]+4+EyeScan_DelayCellPI[u1BitIdx]) <= EyeScan_Max[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1BitIdx][EyeScan_Index]))
						{
							Upper_Vcent_pass_flag = 1;
						}

						if ((EyeScan_Min[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1BitIdx][EyeScan_Index] <= (EyeScan_CaliDelay[u1BitIdx/8]-4+EyeScan_DelayCellPI[u1BitIdx])) && ((EyeScan_CaliDelay[u1BitIdx/8]+4+EyeScan_DelayCellPI[u1BitIdx]) <= EyeScan_Max[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1BitIdx][EyeScan_Index]))
						{
							Lower_Vcent_pass_flag = 1;
						}
					}
#endif

#if (CONFIG_FOR_HQA_TEST_USED == 1)
                    //find VdlVWBest Vref Range and Vref
                    VdlVWBest_Vcent_Range = 1;
                    VdlVWBest_Vcent = VREF_VOLTAGE_TABLE_NUM-1;
                    vrefrange_i = 1;
                    max_winsize = 0;
                    for(i=VREF_VOLTAGE_TABLE_NUM-1; i>=0; i--)
                    {
                        if (gEyeScan_WinSize[i+vrefrange_i*30][u1BitIdx] > max_winsize && is_lp4_family(p))
                        {
                            max_winsize = gEyeScan_WinSize[i+vrefrange_i*30][u1BitIdx];
                            VdlVWBest_Vcent_Range = vrefrange_i;
                            VdlVWBest_Vcent = i;
                        }
                        if (i==21 && vrefrange_i==1 && is_lp4_family(p))
                        {
                            vrefrange_i=0;
                            i=VREF_VOLTAGE_TABLE_NUM;
                        }
                        if (gEyeScan_WinSize[i+(1-vrefrange_i)*23][u1BitIdx] > max_winsize && p->dram_type == TYPE_PCDDR4)
                        {
                            max_winsize = gEyeScan_WinSize[i+(1-vrefrange_i)*23][u1BitIdx];
                            VdlVWBest_Vcent_Range = vrefrange_i;
                            VdlVWBest_Vcent = i;
                        }
                        if (i==28 && vrefrange_i==0 && p->dram_type == TYPE_PCDDR4)
                        {
                            vrefrange_i=1;
                            i=VREF_VOLTAGE_TABLE_NUM;
                        }
                    }

                    EyeScanVcent[10+u1BitIdx*2] = VdlVWBest_Vcent_Range;
                    EyeScanVcent[10+u1BitIdx*2+1] = VdlVWBest_Vcent;
#endif

		    if (print_type==2)
		    {
			if (is_lp4_family(p)){
				mcSHOW_DBG_MSG(("[EYESCAN_LOG] %d Channel%d Rank%d Bit%d(DRAM DQ%d)\tHigher VdlTW=%dPI(%d/100ps)(%s)\tLower VdlTW=%dpi(%d/100ps)(%s)\n",
					p->frequency*2, u1ChannelIdx, u1RankIdx, u1BitIdx, u1BitIdx,
					gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1BitIdx],
					gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1BitIdx]*one_pi_ps,
					Upper_Vcent_pass_flag==1 ? "PASS" : "FAIL",
					gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1BitIdx],
					gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1BitIdx]*one_pi_ps,
					Lower_Vcent_pass_flag==1 ? "PASS" : "FAIL"));
			} else if (p->dram_type == TYPE_PCDDR4) {
				mcSHOW_DBG_MSG(("[EYESCAN_LOG] %d Channel%d Rank%d Bit%d(DRAM DQ%d)\tHigher VdlTW=%dPI(%d/100ps)(%s)\tLower VdlTW=%dpi(%d/100ps)(%s)\n",
					p->frequency*2, u1ChannelIdx, u1RankIdx, u1BitIdx, u1BitIdx,
					gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+(1-VdlVWHigh_Upper_Vcent_Range)*23][u1BitIdx],
					gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+(1-VdlVWHigh_Upper_Vcent_Range)*23][u1BitIdx]*one_pi_ps,
					Upper_Vcent_pass_flag==1 ? "PASS" : "FAIL",
					gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+(1-VdlVWHigh_Lower_Vcent_Range)*23][u1BitIdx],
					gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+(1-VdlVWHigh_Lower_Vcent_Range)*23][u1BitIdx]*one_pi_ps,
					Lower_Vcent_pass_flag==1 ? "PASS" : "FAIL"));
			}

                    if (is_lp4_family(p) && (gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1BitIdx] < minTXEyeScanVcentUpperBound))
                    {
                        minTXEyeScanVcentUpperBound = gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+VdlVWHigh_Upper_Vcent_Range*30][u1BitIdx];
                        minTXEyeScanVcentUpperBound_bit = u1BitIdx;
                    }
                    if (is_lp4_family(p) && (gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1BitIdx] < minTXEyeScanVcentLowerBound))
                    {
                        minTXEyeScanVcentLowerBound = gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+VdlVWHigh_Lower_Vcent_Range*30][u1BitIdx];
                        minTXEyeScanVcentLowerBound_bit = u1BitIdx;
                    }
                    if ((p->dram_type == TYPE_PCDDR4) && (gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+(1-VdlVWHigh_Upper_Vcent_Range)*23][u1BitIdx] < minTXEyeScanVcentUpperBound))
                    {
                        minTXEyeScanVcentUpperBound = gEyeScan_WinSize[VdlVWHigh_Upper_Vcent+(1-VdlVWHigh_Upper_Vcent_Range)*23][u1BitIdx];
                        minTXEyeScanVcentUpperBound_bit = u1BitIdx;
                    }
                    if ((p->dram_type == TYPE_PCDDR4) && (gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+(1-VdlVWHigh_Lower_Vcent_Range)*23][u1BitIdx] < minTXEyeScanVcentLowerBound))
                    {
                        minTXEyeScanVcentLowerBound = gEyeScan_WinSize[VdlVWHigh_Lower_Vcent+(1-VdlVWHigh_Lower_Vcent_Range)*30][u1BitIdx];
                        minTXEyeScanVcentLowerBound_bit = u1BitIdx;
                    }
                }
                }
#if (CONFIG_FOR_HQA_TEST_USED == 1)
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
                print_EyeScanVcent_for_HQA_report_used(p, print_type, u1ChannelIdx, u1RankIdx, EyeScanVcent, minTXEyeScanVcentUpperBound, minTXEyeScanVcentUpperBound_bit, minTXEyeScanVcentLowerBound, minTXEyeScanVcentLowerBound_bit);
#endif
#endif
            }
        }
    }
#endif


    if (print_type==2)
    if (u1TXEyeScanEnable)
    {   U8 cal_length;

        mcSHOW_DBG_MSG(("\n\n"));

        if (print_type==2) cal_length = p->data_width;

        if(gEyeScan_CaliDelay[0] <gEyeScan_CaliDelay[1])
            u2DQDelayBegin = gEyeScan_CaliDelay[0]-24;
        else
            u2DQDelayBegin = gEyeScan_CaliDelay[1]-24;


        u2DQDelayEnd = u2DQDelayBegin + 64;


        for (u1BitIdx=0; u1BitIdx<cal_length; u1BitIdx++)
        {
            EyeScan_Index = 0;

#if EyeScan_Pic_draw_line_Mirror
            EyeScan_DelayCellPI_value = 0-gEyeScan_DelayCellPI[u1BitIdx];
#else
            EyeScan_DelayCellPI_value = gEyeScan_DelayCellPI[u1BitIdx];
#endif
            if (print_type==2) Min_Value_1UI_Line = gEyeScan_CaliDelay[u1BitIdx/8]-16-EyeScan_DelayCellPI_value;

            if (print_type==2)
            {
                mcSHOW_EYESCAN_MSG(("[EYESCAN_LOG] TX DQ EYESCAN Channel%d, Rank%d, Bit%d(DRAM DQ%d) ===\n",p->channel, p->rank, u1BitIdx, u1BitIdx));
            }

#if VENDER_JV_LOG
            for(i=0; i<8+one_pi_ps*32/1000; i++) mcSHOW_DBG_MSG5((" "));
            mcSHOW_EYESCAN_MSG(("window\n"));
#else
            for(i=0; i<15+u2DQDelayEnd-u2DQDelayBegin+2; i++) mcSHOW_DBG_MSG((" "));
            mcSHOW_EYESCAN_MSG(("first last window\n"));
#endif

           if (is_lp4_family(p)) {
		u1VrefRange=VREF_RANGE_1;
	   } else if (p->dram_type == TYPE_PCDDR4) {
		u1VrefRange=VREF_RANGE_0;
	   }
            for (u1VrefIdx=VREF_VOLTAGE_TABLE_NUM-1; u1VrefIdx>=0; u1VrefIdx--)
            {
                EyeScan_Pic_draw_line(p, print_type, u1VrefRange, u1VrefIdx, u1BitIdx, u2DQDelayBegin, u2DQDelayEnd, TXVrefRange, gFinalTXVrefDQ[u1ChannelIdx][u1RankIdx], VdlVWHigh_Upper_Vcent_Range, VdlVWHigh_Upper_Vcent, VdlVWHigh_Lower_Vcent_Range, VdlVWHigh_Lower_Vcent, gEyeScan_CaliDelay[u1BitIdx/8], gEyeScan_DelayCellPI[u1BitIdx], one_pi_ps, Min_Value_1UI_Line);

	            if (u1VrefRange==VREF_RANGE_1 && u1VrefIdx==21 && is_lp4_family(p))
	            {
	                u1VrefRange=VREF_RANGE_0;
	                u1VrefIdx=VREF_VOLTAGE_TABLE_NUM;
	            }
	            if (u1VrefRange==VREF_RANGE_0 && u1VrefIdx==28 && p->dram_type == TYPE_PCDDR4)
	            {
	                u1VrefRange=VREF_RANGE_1;
	                u1VrefIdx=VREF_VOLTAGE_TABLE_NUM;
	            }
            }
            mcSHOW_EYESCAN_MSG(("\n\n"));

        }
    }
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
    if (print_type == 2) {
        mcSHOW_EYESCAN_MSG(("[HQALOG]TX EYESCAN Margin Channel%d ", p->channel));
        for(u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
            mcSHOW_EYESCAN_MSG(("%d ",final_vref_eye_margin[p->channel][u1BitIdx]));
        mcSHOW_EYESCAN_MSG(("\n[HQALOG]TX EYESCAN vref value Channel%d %d\n", p->channel, final_vref_value[p->channel]));
    }
#endif
}

#undef mcSHOW_DBG_MSG
#undef mcSHOW_EYESCAN_MSG

#endif

#ifdef DRAM_ETT
#define dbg_print print
#define must_print print
static char UserCommand[128];
static unsigned char command_index = 0;
static int global_which_test=0;
static int ett_needDelay = 0;

//SAVE_TO_SRAM_FORMAT_CHANNEL_T gDRAM_CALIB_LOG;

void TestMenu(void);
static void print_ETT_message(void);
static int complex_mem_test_ett(unsigned int start, unsigned int len);
DRAM_PLL_FREQ_SEL_T freqSel = DDR_DDR1600;
U8 mr3Val = 2;
U8 odtVal = 0;
U8 drvVal = 0;
U8 final_dram = (U8)ETT_DDR3X16X2, final_dram_bak;
U8 run_ett = 0;
CALI_TYPE_T caliType = CALI_MAX;
EYE_SCAN_TYPE_T eyeType = EYE_SCAN_TYPE_MAX;
extern DRAMC_CTX_T *ps_curr_dram_ctx;

static void toUpperString(char *str)
{
    char *temp = str;

    while (*temp) {
        if (*temp>='a' && *temp<='z') {
            *temp -= 'a'-'A';
        }
        temp++;
    }
}

static void pmic_voltage_read(void)
{
    print("[to-do]pmic_voltage_read");
}

static unsigned int UART_Get_Command(void)
{
    unsigned char buff;
    int result_count;

    result_count = GetUARTBytes(&buff, 1, 1);

	if (buff == '=')
		return 0;

    while (!result_count) {
        if ((buff == 0x0d) || (buff == 0x0a)) {
            UserCommand[command_index] = '\0';
            command_index = 0;
            buff = '\n';
            PutUARTByte(buff);

            buff = '\r';
            PutUARTByte(buff);
			toUpperString(UserCommand);
            return 1;
        }

        /* check if the input char is backspace */
        if (buff == '\b') {
            /* check if any data in the command buffer */
            if (command_index) {
                /* put "backspace" */
                /* clear the char in the command buffer */
			      PutUARTByte('\b');
		          PutUARTByte(' ');
		   	      PutUARTByte('\b');

                UserCommand[--command_index] = 0;
            }
        } else {
            if (!result_count) {
                PutUARTByte(buff);
            }

            /* store the char in the command buffer */
            UserCommand[command_index++] = buff;
        }

		result_count = GetUARTBytes(&buff, 1, 1);
    }

    return 0;
}

//nAdjust -    0:Vcore++  1:Vcore--
static void pmic_Vcore_adjust(int nAdjust)
{
	unsigned int OldVcore = 0;
	//[QW] to-do
	switch(nAdjust)
	{
		case ETT_VCORE_INC:        // Vcore++
		break;
		case ETT_VCORE_DEC:        // Vcore--
		break;
		default :
		break;
	}
}

// nAdjust -    0:Vdram++  1:Vdram--
static void pmic_Vdram_adjust(int nAdjust)
{
    unsigned int OldVdram = 0;
	//[QW] to-do
    switch(nAdjust) {
		case ETT_VDRAM_INC:        // Vdram++
			break;
		case ETT_VDRAM_DEC:        // Vdram--
			break;
		default :
			break;
	}
}


#define PATTERN1 0x5A5A5A5A
#define PATTERN2 0xA5A5A5A5
#define _errorExit(errcode) //return(errcode)
#define dbg_print print
static unsigned int u4Process = 0x00;
static void mem_test_show_process(unsigned int index, unsigned int len)
{
#if 1
	 unsigned int u4NewProcess, u4OldProcess;

     u4NewProcess = (unsigned long long)index*100/(unsigned long long)len;
	// dbg_print("0x%x : 0x%x : 0x%x : %d\n", index,len, len-1, u4NewProcess);

	 if(index == 0x00) dbg_print("0%%");
     if(u4NewProcess != u4Process)
     {

	// dbg_print("1: 0x%x : 0x%x : 0x%x : %d : %d\n", index,len, len-1, u4NewProcess, u4Process);
		u4Process = u4NewProcess;
		if((u4Process%10) == 0x00)
		{
			dbg_print("\n%d%%", u4Process);
		}
		else
		{
			dbg_print(". ");
		}
     }

	 if((index != 0x00) && (index == (unsigned int)(len - 1)))
	 {
		u4Process = 0x00;
		 dbg_print("\n100%% \n");
	 }
#endif
}

static int complex_mem_test_ett(unsigned int start, unsigned int len)
{
    volatile unsigned char *MEM8_BASE = (volatile unsigned char *) start;
    volatile unsigned short *MEM16_BASE = (volatile unsigned short *) start;
    volatile unsigned int *MEM32_BASE = (volatile unsigned int *) start;
    volatile unsigned int *MEM_BASE = (volatile unsigned int *) start;
	volatile unsigned long long *MEM64_BASE = (volatile unsigned long long *)start;
    unsigned char pattern8;
    unsigned short pattern16;
    unsigned long long pattern64;
    unsigned int i, j, size, pattern32;
    unsigned int value, temp;

    size = len >> 2;

	*(volatile unsigned int *)0x10007000 = 0x22000000;
    dbg_print("memory test start address = 0x%x, test length = 0x%x\n", start, len);

    /* === Verify the tied bits (tied low) === */
    for (i = 0; i < size; i++)
    {
        MEM32_BASE[i] = 0;
    }

    for (i = 0; i < size; i++)
    {
		mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0)
        {
            dbg_print("Tied Low Test: Address %x not all zero, %x!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....32bits all zero test: Fail!\n\r");
            _errorExit(1);
        }
        //else
        {
            MEM32_BASE[i] = 0xffffffff;
        }
    }
    if(i == size)	dbg_print("..32bits all zero test: Pass!\n\r");


    /* === Verify the tied bits (tied high) === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        temp = MEM32_BASE[i];
        if(temp != 0xffffffff)
        {
            dbg_print("Tied High Test: Address %x not equal 0xFFFFFFFF, %x!\n\r", &MEM32_BASE[i], temp);
            dbg_print("....32bits all one test: Fail!\n\r");
            _errorExit(2);
        }
        //else
        {
            MEM32_BASE[i] = 0x00;
    	}
    }
    if(i == size)	dbg_print("..32bits all one test: Pass!\n\r");

    /* === Verify pattern 1 (0x00~0xff) === */
    pattern8 = 0x00;
    for (i = 0; i < len; i++)
        MEM8_BASE[i] = pattern8++;

    pattern8 = 0x00;
    for (i = 0; i < len; i++)
    {
    	mem_test_show_process(i, len);
        if (MEM8_BASE[i] != pattern8++)
        {
            dbg_print("Address %x = %x, %x is expected!\n\r", &MEM8_BASE[i], MEM8_BASE[i], --pattern8);
            dbg_print("....8bits 0x00~0xff pattern test: Fail!\n\r");
            _errorExit(3);
        }
    }
    if(i == len)
        dbg_print("..8bits 0x00~0xff pattern test: Pass!\n\r");


    /* === Verify pattern 3 (0x0000, 0x0001, 0x0002, ... 0xFFFF) === */
    pattern16 = 0x00;
    for (i = 0; i < (len >> 1); i++)
        MEM16_BASE[i] = pattern16++;

    pattern16 = 0x00;
    for (i = 0; i < (len >> 1); i++)
    {
    	mem_test_show_process(i, (len >> 1));
        temp = MEM16_BASE[i];
        if(temp != pattern16++)
        {

            dbg_print("Address %x = %x, %x is expected!\n\r", &MEM16_BASE[i], temp, --pattern16);
            dbg_print("....16bits 0x00~0xffff pattern test: Fail!\n\r");
            _errorExit(4);
        }
    }
    if(i == (len >> 1))
        dbg_print("..16bits 0x00~0xffff pattern test: Pass!\n\r");

    /* === Verify pattern 4 (0x00000000, 0x00000001, 0x00000002, ... 0xFFFFFFFF) === */
    pattern32 = 0x00;
    for (i = 0; i < (len >> 2); i++)
        MEM32_BASE[i] = pattern32++;
    pattern32 = 0x00;
    for (i = 0; i < (len >> 2); i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != pattern32++)
        {
            dbg_print("Address %x = %x, %x is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i], --pattern32);
            dbg_print("....32bits 0x00~0xffffffff pattern test: Fail!\n\r");
            _errorExit(5);
        }
    }
    if(i == (len >> 2))
        dbg_print("..32bits 0x00~0xffffffff pattern test: Pass!\n\r");


    /* === Pattern 5: Filling memory range with 0xa5a5a5a5 === */
    for (i = 0; i < size; i++)
        MEM32_BASE[i] = 0xa5a5a5a5;

    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 0h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0xa5a5a5a5)
        {
            dbg_print("Address %x = %x, 0xa5a5a5a5 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0xa5a5a5a5 pattern test: Fail!\n\r");
            _errorExit(6);
        }
        //else
        {
            MEM8_BASE[i * 4] = 0x00;
        }
    }
    if(i == size)
        dbg_print("..0xa5a5a5a5 pattern test: Pass!\n\r");

    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 2h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0xa5a5a500)
        {
            dbg_print("Address %x = %x, 0xa5a5a500 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0xa5a5a500 pattern test: Fail!\n\r");
            _errorExit(7);
        }
        else
        {
            MEM8_BASE[i * 4 + 2] = 0x00;
        }
    }
    if(i == size)
        dbg_print("..0xa5a5a500 pattern test: Pass!\n\r");


    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 1h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0xa500a500)
        {
            dbg_print("Address %x = %x, 0xa500a500 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0xa500a500 pattern test: Fail!\n\r");
            _errorExit(8);
        }
        //else
        {
            MEM8_BASE[i * 4 + 1] = 0x00;
        }
    }
    if(i == size)
        dbg_print("..0xa500a500 pattern test: Pass!\n\r");


    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 3h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0xa5000000)
        {
            dbg_print("Address %x = %x, 0xa5000000 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0xa5000000 pattern test: Fail!\n\r");
            _errorExit(9);
        }
        //else
        {
            MEM8_BASE[i * 4 + 3] = 0x00;
        }
    }
    if(i == size)
        dbg_print("..0xa5000000 pattern test: Pass!\n\r");


    /* === Read Check then Fill Memory with ffff Word Pattern at offset 1h === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        if (MEM32_BASE[i] != 0x00000000)
        {
            dbg_print("Address %x = %x, 0x00000000 is expected!\n\r", &MEM32_BASE[i], MEM32_BASE[i]);
            dbg_print("....0x00000000 pattern test: Fail!\n\r");
            _errorExit(10);
        }
    }
    if(i == size)
        dbg_print("..0x00000000 pattern test: Pass!\n\r");

    /************************************************
    * Additional verification
    ************************************************/
    /* === stage 1 => write 0 === */
    for (i = 0; i < size; i++)
    {
        MEM_BASE[i] = PATTERN1;
    }

    /* === stage 2 => read 0, write 0xF === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        value = MEM_BASE[i];
        if (value != PATTERN1)
        {
            dbg_print("\nStage 2 error. Addr = %x, value = %x\n", &(MEM_BASE[i]), value);
            _errorExit(11);
        }
        MEM_BASE[i] = PATTERN2;
    }

    /* === stage 3 => read F === */
    for (i = 0; i < size; i++)
    {
    	mem_test_show_process(i, size);
        value = MEM_BASE[i];
        if (value != PATTERN2)
        {
            dbg_print("\nStage 3 error. Addr = %x, value = %x\n", &(MEM_BASE[i]), value);
            _errorExit(12);
        }
    }

    dbg_print("..%x and %x Interleaving test: Pass!\n\r", PATTERN1, PATTERN2);
	dbg_print("complex_mem_test_ett done\n");

    return 0;
}

void ett_eyescan_cmd()
{
	int cmd_loop = 1;

	while(cmd_loop) {
		must_print("[EYE Scan]\n");
		must_print("Draw Eye Scan : %s\n", gEye_Scan_color_flag==1 ? "COLOR" : "MONO");
		must_print("---------------------\n");
		must_print("CBT Eye Scan : %s %s Freq\n", gCBT_EYE_Scan_flag==1 ? "ON" : "OFF", gCBT_EYE_Scan_only_higheset_freq_flag==1 ? "Highest" : "All");
		must_print("RX	Eye Scan : %s %s Freq\n", gRX_EYE_Scan_flag==1 ? "ON" : "OFF", gRX_EYE_Scan_only_higheset_freq_flag==1 ? "Highest" : "All");
		must_print("TX	Eye Scan : %s %s Freq\n", gTX_EYE_Scan_flag==1 ? "ON" : "OFF", gTX_EYE_Scan_only_higheset_freq_flag==1 ? "Highest" : "All");
		must_print("\n");
		must_print("CM : Draw %s Eye Scan\n", gEye_Scan_color_flag==1 ? "MONO" : "COLOR");
		must_print("C  : CBT Eye Scan turn %s\n", gCBT_EYE_Scan_flag==1 ? "off" : "on");
		must_print("CF : CBT Eye Scan at %s Freq\n", gCBT_EYE_Scan_only_higheset_freq_flag==1 ? "All" : "Highest");
		must_print("R  : RX Eye Scan turn %s\n", gRX_EYE_Scan_flag==1 ? "off" : "on");
		must_print("RF : RX Eye Scan at %s Freq\n", gRX_EYE_Scan_only_higheset_freq_flag==1 ? "All" : "Highest");
		must_print("T  : TX Eye Scan turn %s\n", gTX_EYE_Scan_flag==1 ? "off" : "on");
		must_print("TF : TX Eye Scan at %s Freq\n", gTX_EYE_Scan_only_higheset_freq_flag==1 ? "All" : "Highest");
		must_print("Q  : Back to pre menu\n");
		print("Please enter pattern selection:");

		while(1) {
			if(UART_Get_Command()) {
				toUpperString(UserCommand);
				if ( !(strcmp((const char *)UserCommand, "C")) )
				{
					gCBT_EYE_Scan_flag = !gCBT_EYE_Scan_flag;
				}
				else if ( !(strcmp((const char *)UserCommand, "CF")) )
				{
					gCBT_EYE_Scan_only_higheset_freq_flag = !gCBT_EYE_Scan_only_higheset_freq_flag;
				}

				if ( !(strcmp((const char *)UserCommand, "R")) )
				{
					gRX_EYE_Scan_flag = !gRX_EYE_Scan_flag;
				}
				else if ( !(strcmp((const char *)UserCommand, "RF")) )
				{
					gRX_EYE_Scan_only_higheset_freq_flag = !gRX_EYE_Scan_only_higheset_freq_flag;
				}
				if ( !(strcmp((const char *)UserCommand, "T")) )
				{
					gTX_EYE_Scan_flag = !gTX_EYE_Scan_flag;
				}
				else if ( !(strcmp((const char *)UserCommand, "TF")) )
				{
					gTX_EYE_Scan_only_higheset_freq_flag = !gTX_EYE_Scan_only_higheset_freq_flag;
				}
				if ( !(strcmp((const char *)UserCommand, "CM")) )
				{
					gEye_Scan_color_flag = !gEye_Scan_color_flag;
				}
				if ( !(strcmp((const char *)UserCommand, "Q")))
					cmd_loop = 0;

				break;
			}
		}
	}
}

static void ett_start_cmd(void)
{
	int i;

	print("G : Start the ETT test. %s\n",(ett_needDelay)?"Delay 40s":"No Delay 40s");
	if (ett_needDelay == 1)
	{
		for (i = 0 ; i < 40; i++)
		{
			print(".");
			mdelay(500);
		}
	}

	run_ett = 1;
}
static void ett_cmd_dly(void)
{
	print("D : Delay 40s before start ETT...[%s]\n",(ett_needDelay)?"Disable":"Enable");
	while(1) {
		if (UART_Get_Command()) {
			toUpperString(UserCommand);
			if ( !(strcmp((const char *)UserCommand, "D")) )
			{
				ett_needDelay = !ett_needDelay;
				print("D : Delay 40s before start ETT...[%s]\n",(ett_needDelay)?"Disable":"Enable");
			}
			if ( !(strcmp((const char *)UserCommand, "Q")))
				return;
		}
	}
}

extern int (*vcore_prefix)(int);
extern int (*vdram_prefix)(int);
static void ett_voltage_tips(void)
{
		print("\n");
		print("======================================\n");
		print("%d (Vcore HV: +5%%	Vdram HV: +5%%)\n",  ETT_HVCORE_HVDRAM);
		print("%d (Vcore NV: +0%%	Vdram NV: +0%%)\n", ETT_NVCORE_NVDRAM);
		print("%d (Vcore LV: -5%%	Vdram LV: -5%%)\n", ETT_LVCORE_LVDRAM);
		print("%d (Vcore HV; +5%%	Vdram LV: -5%%)\n", ETT_HVCORE_LVDRAM);
		print("%d (Vcore LV: -5%%	Vdram HV: +5%%)\n", ETT_LVCORE_HVDRAM);
		print("%d (Vcore NV: +0%%	Vdram LV: -5%%)\n", ETT_NVCORE_LVDRAM);
		print("%d (Vcore NV: +0%%	Vdram HV: +5%%)\n", ETT_NVCORE_HVDRAM);
#if 0
		print("======================================\n");
		print("%d : (Vcore )  ++ ...\n", ETT_VCORE_INC);
		print("%d : (Vcore )  -- ...\n", ETT_VCORE_DEC);
		print("%d : (Vdram )  ++ ...\n", ETT_VDRAM_INC);
		print("%d : (Vdram )  -- ...\n", ETT_VDRAM_DEC);
#endif
		print("Q: Back to pre menu\n");

		print("[Current] Vcore:%d%%, Vmem:%d%%\n", vcore_prefix(100)-100, vdram_prefix(100)-100);

		print("Please enter pattern selection:(%d-%d)", ETT_HVCORE_HVDRAM, ETT_NVCORE_HVDRAM);

}

static void ett_voltage_cmd(void)
{
	ett_voltage_tips();

	while (1)
	{
		if ( UART_Get_Command() )
		{
			int  nSel = atoi(UserCommand);

			toUpperString(UserCommand);
			if ( !(strcmp((const char *)UserCommand, "Q")) )
				return;

			if ( (nSel<ETT_HVCORE_HVDRAM) || (nSel>=ETT_VCORE_VDRAM_MAX))
			{
				nSel = ETT_NVCORE_NVDRAM;
			}

			switch(nSel)
			{
				case ETT_HVCORE_HVDRAM:
					set_vcore_prefix(VOLT_PREFIX_HV);
					set_vdram_prefix(VOLT_PREFIX_HV);
				break;
				case ETT_LVCORE_LVDRAM:
					set_vcore_prefix(VOLT_PREFIX_LV);
					set_vdram_prefix(VOLT_PREFIX_LV);
				break;
				case ETT_HVCORE_LVDRAM:
					set_vcore_prefix(VOLT_PREFIX_HV);
					set_vdram_prefix(VOLT_PREFIX_LV);
				break;
				case ETT_LVCORE_HVDRAM:
					set_vcore_prefix(VOLT_PREFIX_LV);
					set_vdram_prefix(VOLT_PREFIX_HV);
				break;
				case ETT_NVCORE_LVDRAM:
					set_vcore_prefix(VOLT_PREFIX_NV);
					set_vdram_prefix(VOLT_PREFIX_LV);
				break;
				case ETT_NVCORE_HVDRAM:
					set_vcore_prefix(VOLT_PREFIX_NV);
					set_vdram_prefix(VOLT_PREFIX_HV);
				break;
#if 0
				case ETT_VCORE_INC:
					pmic_Vcore_adjust(ETT_VCORE_INC);
				break;
				case ETT_VCORE_DEC:
					pmic_Vcore_adjust(ETT_VCORE_DEC);
				break;
				case ETT_VDRAM_INC:
					pmic_Vdram_adjust(ETT_VDRAM_INC);
				break;
				case ETT_VDRAM_DEC:
					pmic_Vdram_adjust(ETT_VDRAM_DEC);
				break;
#endif
				case ETT_NVCORE_NVDRAM:
				default:
					set_vcore_prefix(VOLT_PREFIX_NV);
					set_vdram_prefix(VOLT_PREFIX_NV);
				break;
			}
			ett_voltage_tips();
		}
	}
}

static ETT_CMD_T ett_cmd_table[] = {
	{"G", "Start the ETT test", ett_start_cmd},
	{"D", "Delay 40s before start ETT", ett_cmd_dly},
	{"E", "eye scan", ett_eyescan_cmd},
	{"V", "Voltage config", ett_voltage_cmd},
	{NULL, NULL, NULL},
};


void ett_help_print()
{
	ETT_CMD_T *pcmd = &ett_cmd_table[0];

	print("\n");
	while(pcmd->key != NULL && pcmd->cmd != NULL) {
		print("%s: %s\n", pcmd->key, pcmd->tip);
		pcmd++;
	}
	print("Please enter selection:");
}

void TestMenu(void)
{
	ETT_CMD_T *pcmd = NULL;

	mtk_wdt_disable();
	ett_help_print();
	while(1) {
		if(UART_Get_Command())
			toUpperString(UserCommand);
		else
			continue;

		pcmd = &ett_cmd_table[0];
		while(pcmd->key != NULL) {
			if(!strcmp((const char *)UserCommand, pcmd->key)) {
				pcmd->cmd();
				break;
			}
			pcmd++;
		}

		if(run_ett) break;

		ett_help_print();
	}
}

#if 0
static void get_criteria_info(CRITERIA_LIST_T *criteria_list)
{
    DRAM_DRAM_TYPE_T dram_type = mt_get_dram_type();

    if(dram_type == TYPE_LPDDR3)
    {
        criteria_list->HTLV.CA = 0;
        criteria_list->HTLV.RX = 0;
        criteria_list->HTLV.TX = 0;

        criteria_list->NTNV.CA = 0;
        criteria_list->NTNV.RX = 0;
        criteria_list->NTNV.TX = 0;

        criteria_list->LTHV.CA = 0;
        criteria_list->LTHV.RX = 0;
        criteria_list->LTHV.TX = 0;

        criteria_list->BUFF.CA = 0;
        criteria_list->BUFF.RX = 0;
        criteria_list->BUFF.TX = 0;
    }
    else if(dram_type == TYPE_PCDDR3)
    {
        criteria_list->HTLV.CA = 0;
        criteria_list->HTLV.RX = 0;
        criteria_list->HTLV.TX = 0;

        criteria_list->NTNV.CA = 0;
        criteria_list->NTNV.RX = 0;
        criteria_list->NTNV.TX = 0;

        criteria_list->LTHV.CA = 0;
        criteria_list->LTHV.RX = 0;
        criteria_list->LTHV.TX = 0;

        criteria_list->BUFF.CA = 0;
        criteria_list->BUFF.RX = 0;
        criteria_list->BUFF.TX = 0;
    }
    else //dram_type == TYPE_PCDDR4
    {
        criteria_list->HTLV.CA = 0;
        criteria_list->HTLV.RX = 0;
        criteria_list->HTLV.TX = 0;

        criteria_list->NTNV.CA = 0;
        criteria_list->NTNV.RX = 0;
        criteria_list->NTNV.TX = 0;

        criteria_list->LTHV.CA = 0;
        criteria_list->LTHV.RX = 0;
        criteria_list->LTHV.TX = 0;

        criteria_list->BUFF.CA = 0;
        criteria_list->BUFF.RX = 0;
        criteria_list->BUFF.TX = 0;
    }

}

static void print_ETT_message(void)
{
    U8 u1BitIdx = 0, u1RankIdx = 0, support_rank_num;
    U8 u1MinWinBit, u1MinMarginBit, u1MinWin, u1MinMargin;
    U32 ca_warn = 0, rx_warn = 0, tx_warn = 0;
    U32 ca_fail = 0, rx_fail = 0, tx_fail = 0;
    DRAM_DATA_WIDTH_T data_width = DATA_WIDTH_16BIT;
    SAVE_TO_SRAM_FORMAT_PASS_WIN_DATA_T *pWin;
    CRITERIA_SIGNAL_T *criteria_signal_list;
    CRITERIA_SIGNAL_T *buffer_signal_list;
    CRITERIA_LIST_T criteria;
    ETT_RESULT_T ett_ret_idx = ETT_FAIL;
    char *ett_ret[ETT_RET_MAX] = {"FAIL", "WARN", "PASS"};
    DRAM_DRAM_TYPE_T dram_type = mt_get_dram_type();

    mcSHOW_DBG_MSG(("\n\n=================[ETT] information Begin=================\n"));

    get_criteria_info(&criteria);

    mcSHOW_DBG_MSG(("PASS CRITERIA: CA RX TX, BUFFER: CA RX TX\n"));

    if (voltage_sel == ETT_HVCORE_HVDRAM)
    {
        criteria_signal_list = &criteria.LTHV;
        mcSHOW_DBG_MSG(("HV: "));

    }
    else if (voltage_sel == ETT_LVCORE_LVDRAM)
    {
        criteria_signal_list = &criteria.HTLV;
        mcSHOW_DBG_MSG(("LV: "));
    }
    else
    {
        criteria_signal_list = &criteria.NTNV;
        mcSHOW_DBG_MSG(("NV: "));
    }
    buffer_signal_list = &criteria.BUFF;
    mcSHOW_DBG_MSG(("%d %d %d, ", criteria_signal_list->CA, criteria_signal_list->RX, criteria_signal_list->TX));
    mcSHOW_DBG_MSG(("%d %d %d\n", buffer_signal_list->CA, buffer_signal_list->RX, buffer_signal_list->TX));

	/*
    if (io_32_read_fld_align(DRAMC_REG_DDRCONF0, DDRCONF0_DM64BITEN))
        data_width = DATA_WIDTH_32BIT;
	*/

    u4DRAMdebugLOgEnable2 = 0;

    mcSHOW_DBG_MSG2(("\n=================Impedance Begin=================\n"));
    mcSHOW_DBG_MSG2(("DRVP = %d, DRVN = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].SwImpedanceCal.DRVP, gDRAM_CALIB_LOG.RANK[u1RankIdx].SwImpedanceCal.DRVN));
    mcSHOW_DBG_MSG2(("=================Impedance Done=================\n"));

    if (dram_type == TYPE_LPDDR3)
    {
        mcSHOW_DBG_MSG(("\n=================CA Training Begin=================\n"));
        mcSHOW_DBG_MSG2(("CA_delay = %d, Clk_delay = %d, CS_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.CA_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.Clk_delay, \
                                                                            gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.CS_delay));

        if (criteria_signal_list->CA > buffer_signal_list->CA)
        {
            buffer_signal_list->CA = criteria_signal_list->CA - buffer_signal_list->CA;
        }
        else
        {
            buffer_signal_list->CA = 0;
        }

        mcSHOW_DBG_MSG(("CAx: [ETT RESULT] (first pass ~ last pass) center, size, best delay, (left margin, right margin)\n"));

        u1MinWin = 0xff;
        u1MinMargin = 0xff;
        for (u1BitIdx=0; u1BitIdx<10; u1BitIdx++)
        {
            pWin = &gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.WinPerBit[u1BitIdx];
            if (pWin->win_size < buffer_signal_list->CA)
            {
                ett_ret_idx = ETT_FAIL;
                ca_fail |= (1 << u1BitIdx);
            }
            else if (pWin->win_size < criteria_signal_list->CA)
            {
                ett_ret_idx = ETT_WARN;
                ca_warn |= (1 << u1BitIdx);
            }
            else
            {
                ett_ret_idx = ETT_PASS;
            }
            mcSHOW_DBG_MSG(("CA%d: [%s] (%d ~ %d) %d, %d, %d, (%d, %d)\n", u1BitIdx, ett_ret[ett_ret_idx], pWin->first_pass, pWin->last_pass, pWin->win_center, \
                                                                                    pWin->win_size, gDRAM_CALIB_LOG.RANK[u1RankIdx].CaTraining.CA_delay, pWin->left_margin, pWin->right_margin));

            if (pWin->win_size < u1MinWin)
            {
                u1MinWin = pWin->win_size;
                u1MinWinBit = u1BitIdx;
            }

            if (pWin->left_margin < u1MinMargin)
            {
                u1MinMargin = pWin->left_margin;
                u1MinMarginBit = u1BitIdx;
            }

            if (pWin->right_margin < u1MinMargin)
            {
                u1MinMargin = pWin->right_margin;
                u1MinMarginBit = u1BitIdx;
            }
        }

        mcSHOW_DBG_MSG(("CA%d Minimum Window %d\n", u1MinWinBit, u1MinWin));
        mcSHOW_DBG_MSG(("CA%d Minimum Margin %d\n", u1MinMarginBit, u1MinMargin));
        mcSHOW_DBG_MSG(("=================CA Training Done=================\n"));
    }

    mcSHOW_DBG_MSG2(("\n=================Write Leveling Begin=================\n"));
    mcSHOW_DBG_MSG2(("CA_Clk_delay = %d, Clk_delay = %d, Final_Clk_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.CA_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.Clk_delay, \
                                                                                gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.Final_Clk_delay));
    mcSHOW_DBG_MSG2(("DQS0_delay = %d, DQS1_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.DQS0_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.DQS1_delay));

    if (data_width == DATA_WIDTH_32BIT)
        mcSHOW_DBG_MSG2(("DQS2_delay = %d, DQS3_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.DQS2_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].Write_Leveling.DQS3_delay));

    mcSHOW_DBG_MSG2(("=================Write Leveling Done=================\n"));

    support_rank_num = (U8)get_dram_rank_nr();

    for (u1RankIdx=RANK_0; u1RankIdx<support_rank_num; u1RankIdx++)
    {
        mcSHOW_DBG_MSG(("\n=================[Rank%d] Begin=================\n", u1RankIdx));

        mcSHOW_DBG_MSG2(("=================Gating Begin=================\n"));
        mcSHOW_DBG_MSG2(("DQS0 P1 Delay(2T, 0.5T, PI) = (%d, %d, %d) Pass tag = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS0_2T, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS0_05T, \
                                                    gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS0_PI, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.Gating_Win[0]));
        mcSHOW_DBG_MSG2(("DQS1 P1 Delay(2T, 0.5T, PI) = (%d, %d, %d) Pass tag = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS1_2T, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS1_05T, \
                                                    gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS1_PI, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.Gating_Win[1]));
        if (data_width == DATA_WIDTH_32BIT)
        {
            mcSHOW_DBG_MSG2(("DQS2 P1 Delay(2T, 0.5T, PI) = (%d, %d, %d) Pass tag = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS2_2T, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS2_05T, \
                                                        gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS2_PI, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.Gating_Win[2]));
            mcSHOW_DBG_MSG2(("DQS3 P1 Delay(2T, 0.5T, PI) = (%d, %d, %d) Pass tag = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS3_2T, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS3_05T, \
                                                        gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.DQS3_PI, gDRAM_CALIB_LOG.RANK[u1RankIdx].GatingWindow.Gating_Win[3]));
        }

        mcSHOW_DBG_MSG2(("s1ChangeDQSINCTL = %d, reg_TX_dly_DQSgated_min = %d, u1TXDLY_Cal_min = %d\n", gDRAM_CALIB_LOG.RxdqsGatingPostProcess.s1ChangeDQSINCTL, \
                                                                                                  gDRAM_CALIB_LOG.RxdqsGatingPostProcess.reg_TX_dly_DQSgated_min, gDRAM_CALIB_LOG.RxdqsGatingPostProcess.u1TXDLY_Cal_min));
        mcSHOW_DBG_MSG2(("TX_dly_DQSgated_check_min = %d, TX_dly_DQSgated_check_max = %d\n", gDRAM_CALIB_LOG.RxdqsGatingPostProcess.TX_dly_DQSgated_check_min, gDRAM_CALIB_LOG.RxdqsGatingPostProcess.TX_dly_DQSgated_check_max));
        mcSHOW_DBG_MSG2(("DQSINCTL = %d, RANKINCTL = %d, u4XRTR2R = %d\n", gDRAM_CALIB_LOG.RxdqsGatingPostProcess.DQSINCTL, gDRAM_CALIB_LOG.RxdqsGatingPostProcess.RANKINCTL, gDRAM_CALIB_LOG.RxdqsGatingPostProcess.u4XRTR2R));

        mcSHOW_DBG_MSG2(("=================Gating Done=================\n"));

        mcSHOW_DBG_MSG2(("\n=================Datlat Begin=================\n"));
        mcSHOW_DBG_MSG2(("best_step = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].DATLAT.best_step));

        if (u1RankIdx == RANK_1)
        {
            mcSHOW_DBG_MSG2(("Rank0_Datlat = %d, Rank1_Datlat = %d, Final_Datlat = %d\n", gDRAM_CALIB_LOG.DualRankRxdatlatCal.Rank0_Datlat, gDRAM_CALIB_LOG.DualRankRxdatlatCal.Rank1_Datlat, \
                                                                                            gDRAM_CALIB_LOG.DualRankRxdatlatCal.Final_Datlat));
        }
        mcSHOW_DBG_MSG2(("=================Datlat Done=================\n"));

        mcSHOW_DBG_MSG(("\n=================RX Begin=================\n"));
        mcSHOW_DBG_MSG2(("DQS0_delay = %d, DQS1_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQS0_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQS1_delay));

        if (data_width == DATA_WIDTH_32BIT)
            mcSHOW_DBG_MSG2(("DQS2_delay = %d, DQS3_delay = %d\n", gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQS2_delay, gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQS3_delay));

        if (criteria_signal_list->RX > buffer_signal_list->RX)
        {
            buffer_signal_list->RX = criteria_signal_list->RX - buffer_signal_list->RX;
        }
        else
        {
            buffer_signal_list->RX = 0;
        }

        mcSHOW_DBG_MSG(("RX DQx: [ETT RESULT] (first pass ~ last pass) center, size, best delay, (left margin, right margin)\n"));

        u1MinWin = 0xff;
        u1MinMargin = 0xff;
        for (u1BitIdx=0; u1BitIdx<data_width; u1BitIdx++)
        {
            pWin = &gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.WinPerBit[u1BitIdx];
            if (pWin->win_size < buffer_signal_list->RX)
            {
                ett_ret_idx = ETT_FAIL;
                rx_fail |= (1 << u1BitIdx);
            }
            else if (pWin->win_size < criteria_signal_list->RX)
            {
                ett_ret_idx = ETT_WARN;
                rx_warn |= (1 << u1BitIdx);
            }
            else
            {
                ett_ret_idx = ETT_PASS;
            }

            mcSHOW_DBG_MSG(("RX DQ%d: [%s] (%d ~ %d) %d, %d, %d, (%d, %d)\n", u1BitIdx, ett_ret[ett_ret_idx], pWin->first_pass, pWin->last_pass, pWin->win_center, \
                                                                                    pWin->win_size, gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQ_delay[u1BitIdx], pWin->left_margin, pWin->right_margin));

            if (pWin->win_size < u1MinWin)
            {
                u1MinWin = pWin->win_size;
                u1MinWinBit = u1BitIdx;
            }

            if (pWin->left_margin < u1MinMargin)
            {
                u1MinMargin = pWin->left_margin;
                u1MinMarginBit = u1BitIdx;
            }

            if (pWin->right_margin < u1MinMargin)
            {
                u1MinMargin = pWin->right_margin;
                u1MinMarginBit = u1BitIdx;
            }
        }

        mcSHOW_DBG_MSG(("RX DQ%d Minimum Window %d\n", u1MinWinBit, u1MinWin));
        mcSHOW_DBG_MSG(("RX DQ%d Minimum Margin %d\n", u1MinMarginBit, u1MinMargin));
        mcSHOW_DBG_MSG(("=================RX Done=================\n"));

        mcSHOW_DBG_MSG(("\n=================TX Begin=================\n"));

        for (u1BitIdx=0; u1BitIdx<data_width/DQS_BIT_NUMBER; u1BitIdx++)
            mcSHOW_DBG_MSG2(("Byte%d Delay(LargeUI, SmallUI, PI) = (%d, %d, %d)\n", u1BitIdx, gDRAM_CALIB_LOG.RANK[u1RankIdx].TxWindowPerbitCal.Large_UI[u1BitIdx], \
                                                                                    gDRAM_CALIB_LOG.RANK[u1RankIdx].TxWindowPerbitCal.Small_UI[u1BitIdx], gDRAM_CALIB_LOG.RANK[u1RankIdx].TxWindowPerbitCal.PI[u1BitIdx]));

        if (criteria_signal_list->TX > buffer_signal_list->TX)
        {
            buffer_signal_list->TX = criteria_signal_list->TX - buffer_signal_list->TX;
        }
        else
        {
            buffer_signal_list->TX = 0;
        }

        mcSHOW_DBG_MSG(("TX DQx: [ETT RESULT] (first pass ~ last pass) center, size, best delay, (left margin, right margin)\n"));

        u1MinWin = 0xff;
        u1MinMargin = 0xff;
        for (u1BitIdx=0; u1BitIdx<data_width; u1BitIdx++)
        {
            pWin = &gDRAM_CALIB_LOG.RANK[u1RankIdx].TxWindowPerbitCal.WinPerBit[u1BitIdx];
            if (pWin->win_size < buffer_signal_list->TX)
            {
                ett_ret_idx = ETT_FAIL;
                tx_fail |= (1 << u1BitIdx);
            }
            else if (pWin->win_size < criteria_signal_list->TX)
            {
                ett_ret_idx = ETT_WARN;
                tx_warn |= (1 << u1BitIdx);
            }
            else
            {
                ett_ret_idx = ETT_PASS;
            }

            mcSHOW_DBG_MSG(("TX DQ%d: [%s] (%d ~ %d) %d, %d, %d, (%d, %d)\n", u1BitIdx, ett_ret[ett_ret_idx], pWin->first_pass, pWin->last_pass, pWin->win_center, \
                                                                                pWin->win_size, gDRAM_CALIB_LOG.RANK[u1RankIdx].RxWindowPerbitCal.DQ_delay[u1BitIdx], pWin->left_margin, pWin->right_margin));

            if (pWin->win_size < u1MinWin)
            {
                u1MinWin = pWin->win_size;
                u1MinWinBit = u1BitIdx;
            }

            if (pWin->left_margin < u1MinMargin)
            {
                u1MinMargin = pWin->left_margin;
                u1MinMarginBit = u1BitIdx;
            }

            if (pWin->right_margin < u1MinMargin)
            {
                u1MinMargin = pWin->right_margin;
                u1MinMarginBit = u1BitIdx;
            }
        }

        mcSHOW_DBG_MSG(("TX DQ%d Minimum Window %d\n", u1MinWinBit, u1MinWin));
        mcSHOW_DBG_MSG(("TX DQ%d Minimum Margin %d\n", u1MinMarginBit, u1MinMargin));
        mcSHOW_DBG_MSG(("=================TX Done=================\n"));

        mcSHOW_DBG_MSG(("\n=================[Rank%d] Done=================\n", u1RankIdx));
    }

    mcSHOW_DBG_MSG(("ca_warn = 0x%x, rx_warn = 0x%x, tx_warn = 0x%x\n", ca_warn, rx_warn, tx_warn));
    mcSHOW_DBG_MSG(("ca_fail = 0x%x, rx_fail = 0x%x, tx_fail = 0x%x\n", ca_fail, rx_fail, tx_fail));

    if (ca_fail || rx_fail || tx_fail)
    {
        mcSHOW_DBG_MSG(("\nFINAL ETT RESULT FAIL\n"));
    }

    else
    {
        mcSHOW_DBG_MSG(("\nFINAL ETT RESULT PASS\n"));
    }

    u4DRAMdebugLOgEnable2 = 1;
    mcSHOW_DBG_MSG(("\n=================[ETT] information Done=================\n"));
}
#endif
#endif //DRAM_ETT

