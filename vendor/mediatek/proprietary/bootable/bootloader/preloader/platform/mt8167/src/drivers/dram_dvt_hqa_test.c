#include "dramc_common.h"
#include "x_hal_io.h"
#include "dramc_pi_api.h"

#ifdef MT8167_HQA_TEST
U32 ScanRet[32][RET_LEN];	
S8 ScanIndex = 0, ScanPI, ScanTxBegin;
static S32 CATrain_ClkDelay[CHANNEL_MAX];
U32 pattern[2][10] = {
	//0->1
	{0xaaa9, 0xaaa6, 0xaa9a, 0xaa6a, 0xaaa9, 0xa9aa, 0xa6aa, 0x9aaa, 0x6aaa, 0xa9aa},
	//1->0
	{0x5556, 0x5559, 0x5565, 0x5595, 0x5556, 0x5655, 0x5955, 0x6555, 0x9555, 0x5655}		
};

S8 iPatternType = -1;
U8 u1PatternIdx = 0;

#define PASS_RANGE_NA   0x7fff

#if SUPPORT_TX_DELAY_LINE_CALI
DRAM_STATUS_T DramcTxWindowPerbitCalDelayLine(DRAMC_CTX_T *p, DRAM_TX_PER_BIT_CALIBRATION_TYTE_T calType)
{
    U8 u1BitIdx, u1ByteIdx, u1SmallestDQSByte=0;
    U8 ucindex, ucbit_first, ucbit_last, u1PI;
    U16 u2TmpDQMSum;
    U32 uiFinishCount;
    PASS_WIN_DATA_T WinPerBit[DQ_DATA_WIDTH], VrefWinPerBit[DQ_DATA_WIDTH], FinalWinPerBit[DQ_DATA_WIDTH];
    S16 uiDelay, u2DQDelayBegin=0, u2DQDelayEnd=15;
    S32 iDQSDlyPerbyte[DQS_NUMBER], iDQMDlyPerbyte[DQS_NUMBER];//, iFinalDQSDly[DQS_NUMBER];

    U32 u4err_value, u4fail_bit;
    U8 u1VrefScanEnable;
    U16 u2VrefRange, u2VrefLevel, u2FinalVref=0xd;
    U16 u2VrefBegin, u2FinalRange, u2VrefEnd, u2VrefStep;
    U16 u2TempWinSum, u2tx_window_sum;
    S16 iDqsMax[DQS_NUMBER];
#if ENABLE_CLAIBTAION_WINDOW_LOG_FOR_FT
    U16 u2MinWinSize = 0xffff;
    U8 u1MinWinSizeBitidx;
#endif
    U16 TXPerbitWin_min_max = 0;
    U32 min_bit, min_winsize;

    U8 ii, backup_rank, u1PrintWinData, DlyLineMoveDQSOnly = 0;

    if (!p)
    {
        mcSHOW_ERR_MSG(("context is NULL\n"));
        return DRAM_FAIL;
    }

    backup_rank = u1GetRank(p);

   u1VrefScanEnable =0;

   if(u1VrefScanEnable)
    {
        u2VrefBegin = TX_VREF_RANGE_BEGIN;
        u2VrefEnd = TX_VREF_RANGE_END;
        u2VrefStep = TX_VREF_RANGE_STEP;
    }
    else //LPDDR3, the for loop will only excute u2VrefLevel=TX_VREF_RANGE_END/2.
    {
        u2VrefBegin = 0;
        u2VrefEnd = 0;
        u2VrefStep =  1;
    }

    u2VrefRange = (!p->odt_onoff);

    u2tx_window_sum =0;

    vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_FAIL);

    mcSHOW_DBG_MSG(("[DramcTxWindowPerbitCalDelayLine] Frequency=%d, Channel=%d, Rank=%d, calType=%d\n", p->frequency, p->channel, p->rank, calType));
    // Elbrus new :
    //Use HW TX tracking value
    //R_DMARPIDQ_SW :drphy_conf (0x170[7])(default set 1)
    //   0: DQS2DQ PI setting controlled by HW
    //R_DMARUIDQ_SW : Dramc_conf(0x156[15])(default set 1)
    //    0: DQS2DQ UI setting controlled by HW
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), 1, MISC_CTRL1_R_DMARPIDQ_SW);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_ARUIDQ_SW);


    for(u2VrefLevel = u2VrefBegin; u2VrefLevel <= u2VrefEnd; u2VrefLevel += u2VrefStep)
    {
         //   SET tx Vref (DQ) here, LP3 no need to set this.
        if(u1VrefScanEnable)
        {
            #if (!REDUCE_LOG_FOR_PRELOADER)
            mcSHOW_DBG_MSG(("\n\n======  LP4 TX VrefRange %d, VrefLevel=%d  ========\n", u2VrefRange, u2VrefLevel));
            mcFPRINTF((fp_A60501, "\n\n====== LP4 TX VrefRange %d,VrefLevel=%d ====== \n", u2VrefRange, u2VrefLevel));
            #endif

            u1MR14Value[p->channel][p->dram_fsp] = (u2VrefLevel | (u2VrefRange<<6));
            DramcModeRegWriteByRank(p, p->rank, 14, u2VrefLevel | (u2VrefRange<<6));
        }
        else
        {
            mcSHOW_DBG_MSG(("\n\n======  TX Vref Scan disable ========\n"));
            mcFPRINTF((fp_A60501, "\n\n====== TX Vref Scan disable ====== \n"));
        }

        // initialize parameters
        uiFinishCount = 0;
        u2TempWinSum =0;

        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            WinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
            WinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
            WinPerBit[u1BitIdx].win_center = (S16)PASS_RANGE_NA;
            VrefWinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
            VrefWinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
            VrefWinPerBit[u1BitIdx].win_center = (S16)PASS_RANGE_NA;
        }

		if(DlyLineMoveDQSOnly)
		{
			u2DQDelayBegin = -15;
			u2DQDelayEnd = 0;
		}
#if 0
		u1PI = u4IO32ReadFldAlign(DDRPHY_SHU1_R0_B0_DQ7, SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0);
		mcSHOW_DBG_MSG(("u1PI:%d\n", u1PI));
		if (u1PI > 8)
			u1PI -= 8;
		else
			u1PI = 0;
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(u1PI, SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
							P_Fld(u1PI, SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(u1PI, SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
							P_Fld(u1PI, SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(u1PI, SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
							P_Fld(u1PI, SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(u1PI, SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
							P_Fld(u1PI, SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
#endif

		if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
		{
			vIO32Write4B(DDRPHY_SHU1_R0_B0_DQ0, 0);
			vIO32Write4B(DDRPHY_SHU1_R0_B1_DQ0, 0);
			vIO32Write4B(DDRPHY_SHU1_R0_B0_DQ0+(1<<POS_BANK_NUM), 0);
			vIO32Write4B(DDRPHY_SHU1_R0_B1_DQ0+(1<<POS_BANK_NUM), 0);
		}

		if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
		{
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1, 0, SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);  // rank0, B0
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1, 0, SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);  // rank0, B1
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1+(1<<POS_BANK_NUM), 0, SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);	// rank0, B0
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1+(1<<POS_BANK_NUM), 0, SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);	// rank0, B1
		}

        for (uiDelay = u2DQDelayBegin; uiDelay <=u2DQDelayEnd; uiDelay++)
        //    for (uiDelay = u2DQDelayBegin; uiDelay <=u2DQDelayEnd; uiDelay+=3)  //DBI test
        {
        	if (uiDelay <= 0)
        	{
	            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ1, \
	                                            P_Fld(-uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0B_DLY_B0) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0_DLY_B0) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0B_DLYB_B0) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0_DLYB_B0));
	            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ1, \
	                                            P_Fld(-uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0B_DLY_B1) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0_DLY_B1) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0B_DLYB_B1) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0_DLYB_B1));
	            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ1+(1<<POS_BANK_NUM), \
	                                            P_Fld(-uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0B_DLY_B0) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0_DLY_B0) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0B_DLYB_B0) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0_DLYB_B0));
	            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ1+(1<<POS_BANK_NUM), \
	                                            P_Fld(-uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0B_DLY_B1) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0_DLY_B1) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0B_DLYB_B1) | \
	                                            P_Fld(-uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0_DLYB_B1));

        	}
        	else
        	{
	            if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
	            {
					vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0, \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
					vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ0, \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));
					vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0+(1<<POS_BANK_NUM), \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
													P_Fld(uiDelay, SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
					vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ0+(1<<POS_BANK_NUM), \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) | \
													P_Fld(uiDelay, SHU1_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));

	            }

	            if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
	            {
					vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1, uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);  // rank0, B0
					vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1, uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);  // rank0, B1
					vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1+(1<<POS_BANK_NUM), uiDelay, SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);
					vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1+(1<<POS_BANK_NUM), uiDelay, SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);
	            }
        	}

            u4err_value= TestEngineCompare(p);

            #if VREF_SPEED_UP_LP4
            if(u1VrefScanEnable==0)
            #endif
            {
                //mcSHOW_DBG_MSG(("Delay=%3d |%2d %2d %3d| %2d %2d| 0x%8x [0]",uiDelay, ucdq_ui_large,ucdq_ui_small, ucdq_pi, ucdq_oen_ui_large,ucdq_oen_ui_small, u4err_value));
                #ifdef ETT_PRINT_FORMAT
                if(u4err_value != 0)
                	mcSHOW_DBG_MSG2(("%d [0]",uiDelay));
                #else
                mcSHOW_DBG_MSG2(("Delay=%3d | 0x%8x [0]",uiDelay,  u4err_value));
                #endif
                mcFPRINTF((fp_A60501, "Delay=%3d | 0x%8x [0]",uiDelay, u4err_value));
            }

            // check fail bit ,0 ok ,others fail
            for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
            {
                u4fail_bit = u4err_value&((U32)1<<u1BitIdx);
#ifdef MT8167_HQA_TEST
                if ((uiDelay-u2DQDelayBegin) < RET_LEN)
                    ScanRet[ScanIndex][uiDelay-u2DQDelayBegin] |= (u4fail_bit ^ (1<<u1BitIdx));
#endif
                #if VREF_SPEED_UP_LP4
                if(u1VrefScanEnable==0)
                #endif
                {
                	if(u4err_value != 0)
                	{
	                    if(u1BitIdx%DQS_BIT_NUMBER ==0)
	                    {
	                        mcSHOW_DBG_MSG2((" "));
	                        mcFPRINTF((fp_A60501, " "));
	                    }

	                    if (u4fail_bit == 0)
	                    {
	                        mcSHOW_DBG_MSG2(("o"));
	                        mcFPRINTF((fp_A60501, "o"));
	                    }
	                    else
	                    {
	                            mcSHOW_DBG_MSG2(("x"));
	                            mcFPRINTF((fp_A60501, "x"));
	                    }
                	}

                }

                if(WinPerBit[u1BitIdx].first_pass== PASS_RANGE_NA)
                {
                    if(u4fail_bit==0) //compare correct: pass
                    {
                        WinPerBit[u1BitIdx].first_pass = uiDelay;
                    }
                }
                else if(WinPerBit[u1BitIdx].last_pass == PASS_RANGE_NA)
                {
                    if(u4fail_bit !=0) //compare error : fail
                    {
                        WinPerBit[u1BitIdx].last_pass  = (uiDelay-1);
                    }
                    else if (uiDelay==u2DQDelayEnd)
                    //else if (uiDelay==MAX_TX_DQDLY_TAPS)
                    {
                        WinPerBit[u1BitIdx].last_pass  = uiDelay;
                    }

                    if(WinPerBit[u1BitIdx].last_pass  !=PASS_RANGE_NA)
                    {
                        if((WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass) >= (VrefWinPerBit[u1BitIdx].last_pass -VrefWinPerBit[u1BitIdx].first_pass))
                        {
                            #if 0 //for debug
                            if(VrefWinPerBit[u1BitIdx].last_pass != PASS_RANGE_NA)
                            {
                                mcSHOW_DBG_MSG2(("Bit[%d] Bigger window update %d > %d\n", u1BitIdx, \
                                    (WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass), (VrefWinPerBit[u1BitIdx].last_pass -VrefWinPerBit[u1BitIdx].first_pass)));
                                mcFPRINTF((fp_A60501,"Bit[%d] Bigger window update %d > %d\n", u1BitIdx, \
                                    (WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass), (VrefWinPerBit[u1BitIdx].last_pass -VrefWinPerBit[u1BitIdx].first_pass)));

                            }
                            #endif
                            uiFinishCount |= (1<<u1BitIdx);
                            //update bigger window size
                            VrefWinPerBit[u1BitIdx].first_pass = WinPerBit[u1BitIdx].first_pass;
                            VrefWinPerBit[u1BitIdx].last_pass = WinPerBit[u1BitIdx].last_pass;
                        }

                        //reset tmp window
                        WinPerBit[u1BitIdx].first_pass = PASS_RANGE_NA;
                        WinPerBit[u1BitIdx].last_pass = PASS_RANGE_NA;
                    }
                 }
               }

                #if VREF_SPEED_UP_LP4
                if(u1VrefScanEnable==0)
                #endif
                {
                	if(u4err_value != 0)
                	{
	                    mcSHOW_DBG_MSG2((" [MSB]\n"));
	                    mcFPRINTF((fp_A60501, " [MSB]\n"));
                	}
                }

                //if all bits widnow found and all bits turns to fail again, early break;
            	if(((p->data_width== DATA_WIDTH_16BIT) &&(uiFinishCount == 0xffff)) || \
					((p->data_width== DATA_WIDTH_32BIT) &&(uiFinishCount == 0xffffffff)) || \
					((p->swap_byte_16bit == ENABLE) &&(uiFinishCount == 0xffff0000)))
                    {
                        vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_OK);
                        #if !VREF_SPEED_UP_LP4
                        if(u1VrefScanEnable ==0)
                        #endif
                        {
                                if(((p->data_width== DATA_WIDTH_16BIT) &&((u4err_value&0xffff) == 0xffff)) || \
								((p->data_width== DATA_WIDTH_32BIT) &&(u4err_value == 0xffffffff)) || \
								((p->swap_byte_16bit == ENABLE) &&((u4err_value&0xffff0000) == 0xffff0000)))
                                {
                                    #if !REDUCE_LOG_FOR_PRELOADER
                                        #ifdef ETT_PRINT_FORMAT
                                        mcSHOW_DBG_MSG2(("TX calibration finding left boundary early break. PI DQ delay=0x%B\n", uiDelay));
                                        #else
                                        mcSHOW_DBG_MSG2(("TX calibration finding left boundary early break. PI DQ delay=0x%2x\n", uiDelay));
                                        #endif
                                    #endif
                                    break;  //early break
                                }
                        }
                    }
        }

        if (p->swap_byte_16bit == ENABLE) {

			for (u1BitIdx = 0; u1BitIdx < (p->data_width)/2; u1BitIdx++)
			{
				memcpy(&VrefWinPerBit[u1BitIdx], &VrefWinPerBit[u1BitIdx+16], sizeof(PASS_WIN_DATA_T));
			}
        }
        if (p->en_4bitMux == ENABLE)
        {
			mcSHOW_DBG_MSG(("Tx 4bitMux is enabled\n"));

			#if 1
			for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				mcSHOW_DBG_MSG(("%d:%d ", VrefWinPerBit[u1BitIdx].first_pass, VrefWinPerBit[u1BitIdx].last_pass));
			}
			mcSHOW_DBG_MSG(("\n"));
			#endif

			for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				memcpy(&WinPerBit[Bit_DQ_Mapping[u1BitIdx]], &VrefWinPerBit[u1BitIdx], sizeof(PASS_WIN_DATA_T));
			}
			memcpy(VrefWinPerBit, WinPerBit, sizeof(PASS_WIN_DATA_T) * DQ_DATA_WIDTH);

			#if 1
			for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				mcSHOW_DBG_MSG(("%d:%d ", VrefWinPerBit[u1BitIdx].first_pass, VrefWinPerBit[u1BitIdx].last_pass));
				mcSHOW_DBG_MSG(("\n"));
			}
			#endif

        }

        min_winsize = 0xffff;
        min_bit = 0xff;
        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            VrefWinPerBit[u1BitIdx].win_size = VrefWinPerBit[u1BitIdx].last_pass- VrefWinPerBit[u1BitIdx].first_pass +1;

            if (VrefWinPerBit[u1BitIdx].win_size < min_winsize)
            {
                min_bit = u1BitIdx;
                min_winsize = VrefWinPerBit[u1BitIdx].win_size;
            }

            u2TempWinSum += VrefWinPerBit[u1BitIdx].win_size;  //Sum of CA Windows for vref selection

            #if ENABLE_CLAIBTAION_WINDOW_LOG_FOR_FT
            if(VrefWinPerBit[u1BitIdx].win_size<u2MinWinSize)
            {
                u2MinWinSize = VrefWinPerBit[u1BitIdx].win_size;
                u1MinWinSizeBitidx = u1BitIdx;
            }
            #endif
        }
        mcSHOW_DBG_MSG3(("Fra Min Bit=%d, winsize=%d ===\n",min_bit, min_winsize));

        #if ENABLE_CLAIBTAION_WINDOW_LOG_FOR_FT
        mcSHOW_DBG_MSG2(("FT log: TX min window : bit %d, size %d\n", u1MinWinSizeBitidx, u2MinWinSize));
        #endif
        #if !REDUCE_LOG_FOR_PRELOADER
        mcSHOW_DBG_MSG2(("\nTX Vref %d, Window Sum %d > %d\n", u2VrefLevel, u2TempWinSum, u2tx_window_sum));
        mcFPRINTF((fp_A60501, "\nTX Vref %d, Window Sum %d > %d\n", u2VrefLevel, u2TempWinSum, u2tx_window_sum));
        #endif

        u1PrintWinData =0;
        if ((min_winsize > TXPerbitWin_min_max) || ((min_winsize == TXPerbitWin_min_max) && (u2TempWinSum >u2tx_window_sum)))
        {
            #if REDUCE_LOG_FOR_PRELOADER
            u1PrintWinData =0;
            #else
            u1PrintWinData =1;
            #endif

            {
                mcSHOW_DBG_MSG3(("\nBetter TX Vref found %d, Window Sum %d > %d\n", u2VrefLevel, u2TempWinSum, u2tx_window_sum));
                mcFPRINTF((fp_A60501, "\nBetter TX Vref found %d, Window Sum %d > %d\n", u2VrefLevel, u2TempWinSum, u2tx_window_sum));
            }

            TXPerbitWin_min_max = min_winsize;
            u2tx_window_sum =u2TempWinSum;
            u2FinalRange = u2VrefRange;
            u2FinalVref = u2VrefLevel;
            memcpy(FinalWinPerBit, VrefWinPerBit, sizeof(PASS_WIN_DATA_T) * DQ_DATA_WIDTH);
            for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
				FinalWinPerBit[u1BitIdx].win_center = (FinalWinPerBit[u1BitIdx].first_pass + FinalWinPerBit[u1BitIdx].last_pass) >> 1;
        }

        #if VREF_SPEED_UP_LP4
        else if(u2TempWinSum < (u2tx_window_sum*95/100))
        {
            mcSHOW_DBG_MSG(("\nTX Vref found, early break!\n"));
            break;//max vref found, early break;
        }
        #endif

    }

	memset(DlyPerBit, 0, sizeof(DlyPerBit));
	memcpy(DlyPerBit, FinalWinPerBit, sizeof(PASS_WIN_DATA_T) * DQ_DATA_WIDTH);
    mcSHOW_DBG_MSG(("[DramcTxWindowPerbitCalDelayLine] ====Done====\n"));
    mcFPRINTF((fp_A60501, "[DramcTxWindowPerbitCalDelayLine] ====Done====\n"));

    return DRAM_OK;

}
static DRAM_STATUS_T DramcTxWindowPerbitCalTest(DRAMC_CTX_T *p, DRAM_TX_PER_BIT_CALIBRATION_TYTE_T calType)
{
    U8 u1BitIdx, u1ByteIdx, u1SmallestDQSByte=0;
    U8 ucindex;
    U32 uiFinishCount;
    PASS_WIN_DATA_T WinPerBit[DQ_DATA_WIDTH], VrefWinPerBit[DQ_DATA_WIDTH], FinalWinPerBit[DQ_DATA_WIDTH];

    U16 uiDelay, u2DQDelayBegin=0, u2DQDelayEnd=0;
    U8 ucdq_pi, ucdq_ui_small, ucdq_ui_large,ucdq_oen_ui_small, ucdq_oen_ui_large;
    static U8 dq_ui_small_bak, dq_ui_large_bak,  dq_oen_ui_small_bak, dq_oen_ui_large_bak;
    U8 ucdq_final_pi[DQS_NUMBER], ucdq_final_ui_large[DQS_NUMBER], ucdq_final_ui_small[DQS_NUMBER];
    U8 ucdq_final_oen_ui_large[DQS_NUMBER], ucdq_final_oen_ui_small[DQS_NUMBER];

    S16 s1temp1, s1temp2;
#if SUPPORT_TX_DELAY_LINE_CALI
    S16 s1LastPassMin[4] = {0xff, 0xff, 0xff, 0xff};
    S32 s2Factor, s2FactorMean = 0, s2Cnt = 0;
    S32 iDQMDlyPerbyte[4] = {0,0,0,0};
    U32 iDQ_PerBit_DelayLine[32];
#endif
    #if TX_DQM_CALC_MAX_MIN_CENTER
    U16 u2Center_min[DQS_NUMBER],u2Center_max[DQS_NUMBER];
    #else
    S16 s2sum_dly[DQS_NUMBER];
    #endif
    U32 u4err_value, u4fail_bit;
    U8 u1VrefScanEnable;
    U16 u2VrefRange, u2VrefLevel, u2FinalVref=0xd;
    U16 u2VrefBegin, u2FinalRange, u2VrefEnd, u2VrefStep;
    U16 u2TempWinSum, u2tx_window_sum;
    U32 u4TempRegValue;
    U16 u2TmpValue;
#if ENABLE_CLAIBTAION_WINDOW_LOG_FOR_FT
    U16 u2MinWinSize = 0xffff;
    U8 u1MinWinSizeBitidx;
#endif
    U16 TXPerbitWin_min_max = 0;
    U32 min_bit, min_winsize;

    U8 ii, backup_rank, u1PrintWinData;

    if (!p)
    {
        mcSHOW_ERR_MSG(("context is NULL\n"));
        return DRAM_FAIL;
    }

    backup_rank = u1GetRank(p);

#if SUPPORT_TX_DELAY_LINE_CALI
	if (backup_rank == RANK_1)
	{
		memset(iDQ_PerBit_DelayLine, 0, sizeof(iDQ_PerBit_DelayLine));
		memset(iDQMDlyPerbyte, 0, sizeof(iDQMDlyPerbyte));
		if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
		{
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0, \
											P_Fld(iDQ_PerBit_DelayLine[7], SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[6], SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[5], SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[4], SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[3], SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[2], SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[1], SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[0], SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ0, \
											P_Fld(iDQ_PerBit_DelayLine[15], SHU1_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[14], SHU1_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[13], SHU1_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[12], SHU1_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[11], SHU1_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[10], SHU1_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[9], SHU1_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[8], SHU1_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0+(1<<POS_BANK_NUM), \
											P_Fld(iDQ_PerBit_DelayLine[23], SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[22], SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[21], SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[20], SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[19], SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[18], SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[17], SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[16], SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ0+(1<<POS_BANK_NUM), \
											P_Fld(iDQ_PerBit_DelayLine[31], SHU1_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[30], SHU1_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[29], SHU1_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[28], SHU1_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[27], SHU1_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[26], SHU1_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[25], SHU1_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[24], SHU1_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));

		}

		if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
		{
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1, iDQMDlyPerbyte[0], SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);	// rank0, B0
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1, iDQMDlyPerbyte[1], SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);	// rank0, B1
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1+(1<<POS_BANK_NUM), iDQMDlyPerbyte[2], SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);  // rank0, B0
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1+(1<<POS_BANK_NUM), iDQMDlyPerbyte[3], SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);  // rank0, B1
		}
	}
#endif
    //A.set RX DQ/DQS in the middle of the pass region from read DQ/DQS calibration
    //B.Fix DQS (RG_PI_**_PBYTE*) at degree from write leveling.
    //   Move DQ (per byte) gradually from 90 to -45 degree to find the left boundary
    //   Move DQ (per byte) gradually from 90 to 225 degree to find the right boundary
    //C.For each DQ delay in step B, start engine test
    //D.After engine test, read per bit results from registers.
    //E.Set RG_PI_**_DQ* to lie in the average of the middle of the pass region in the same byte
    if(fgwrlevel_done)
    {
        // Find smallest DQS delay after write leveling. DQ PI scan from smallest DQS PI.
        u2DQDelayBegin =0xff;
        for(u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
        {
            if(wrlevel_dqs_final_delay[p->channel][u1ByteIdx] < u2DQDelayBegin)
            {
                u2DQDelayBegin = wrlevel_dqs_final_delay[p->channel][u1ByteIdx];
                u1SmallestDQSByte = u1ByteIdx;
            }
        }
    }
    else
        u1SmallestDQSByte =0;

        if(p->fgTXPerbifInit[p->channel][p->rank]== FALSE)
        {
            // Scan from DQ delay = DQS delay
            // For everest, choose DQS 1 which is smaller and won't over 64.
            u4TempRegValue= u4IO32Read4B(DRAMC_REG_SHU_SELPH_DQS0);
            dq_ui_large_bak = (u4TempRegValue >> (u1SmallestDQSByte*4)) & 0x7;
            dq_oen_ui_large_bak= (u4TempRegValue >> (u1SmallestDQSByte*4+16)) & 0x7;

            u4TempRegValue= u4IO32Read4B(DRAMC_REG_SHU_SELPH_DQS1);
            //dq_ui_small_bak = (u4TempRegValue >> (u1SmallestDQSByte*4)) & 0x7;
            dq_ui_small_bak = (u4TempRegValue >> (u1SmallestDQSByte*4)) & 0x3;
            //dq_oen_ui_small_bak= (u4TempRegValue >> (u1SmallestDQSByte*4+16)) & 0x7;;
            dq_oen_ui_small_bak= (u4TempRegValue >> (u1SmallestDQSByte*4+16)) & 0x3;;

            p->fgTXPerbifInit[p->channel][p->rank]= TRUE;
        }

        u2DQDelayBegin = wrlevel_dqs_final_delay[p->channel][u1SmallestDQSByte];
        u2DQDelayEnd = u2DQDelayBegin +96; //Scan at least 1UI. Scan 64 to cover byte differece. if window found, early break.


#ifdef MT8167_HQA_TEST
       ScanTxBegin = u2DQDelayBegin;
#endif

   u1VrefScanEnable =0;

if(u1VrefScanEnable)
    {
        u2VrefBegin = TX_VREF_RANGE_BEGIN;
        u2VrefEnd = TX_VREF_RANGE_END;
        u2VrefStep = TX_VREF_RANGE_STEP;
    }
    else //LPDDR3, the for loop will only excute u2VrefLevel=TX_VREF_RANGE_END/2.
    {
        u2VrefBegin = 0;
        u2VrefEnd = 0;
        u2VrefStep =  1;
    }


    u2VrefRange = (!p->odt_onoff);

    u2tx_window_sum =0;

    vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_FAIL);

    mcSHOW_DBG_MSG(("[DramcTxWindowPerbitCal] Frequency=%d, Channel=%d, Rank=%d, calType=%d\n", p->frequency, p->channel, p->rank, calType));
    mcSHOW_DBG_MSG(("\n[DramcTxWindowPerbitCal] Begin, TX DQ(%d, %d),  DQ OEN(%d, %d)\n", dq_ui_large_bak, dq_ui_small_bak,dq_oen_ui_large_bak,  dq_oen_ui_small_bak));
    mcFPRINTF((fp_A60501, "\n[DramcTxWindowPerbitCal] Begin, TX DQ(%d, %d),  DQ OEN(%d, %d)\n", dq_ui_large_bak, dq_ui_small_bak,dq_oen_ui_large_bak,  dq_oen_ui_small_bak));
    // Elbrus new :
    //Use HW TX tracking value
    //R_DMARPIDQ_SW :drphy_conf (0x170[7])(default set 1)
    //   0: DQS2DQ PI setting controlled by HW
    //R_DMARUIDQ_SW : Dramc_conf(0x156[15])(default set 1)
    //    0: DQS2DQ UI setting controlled by HW
    ///TODO: need backup original setting?
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), 1, MISC_CTRL1_R_DMARPIDQ_SW);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), 1, DQSOSCR_ARUIDQ_SW);


    for(u2VrefLevel = u2VrefBegin; u2VrefLevel <= u2VrefEnd; u2VrefLevel += u2VrefStep)
    {
         //   SET tx Vref (DQ) here, LP3 no need to set this.
        if(u1VrefScanEnable)
        {
            #if (!REDUCE_LOG_FOR_PRELOADER)
            mcSHOW_DBG_MSG(("\n\n======  LP4 TX VrefRange %d, VrefLevel=%d  ========\n", u2VrefRange, u2VrefLevel));
            mcFPRINTF((fp_A60501, "\n\n====== LP4 TX VrefRange %d,VrefLevel=%d ====== \n", u2VrefRange, u2VrefLevel));
            #endif

            u1MR14Value[p->channel][p->dram_fsp] = (u2VrefLevel | (u2VrefRange<<6));
            DramcModeRegWriteByRank(p, p->rank, 14, u2VrefLevel | (u2VrefRange<<6));
        }
        else
        {
            mcSHOW_DBG_MSG(("\n\n======  TX Vref Scan disable ========\n"));
            mcFPRINTF((fp_A60501, "\n\n====== TX Vref Scan disable ====== \n"));
        }

        // initialize parameters
        uiFinishCount = 0;
        u2TempWinSum =0;

        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            WinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
            WinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
            VrefWinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
            VrefWinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
        }

        //Move DQ delay ,  1 PI = tCK/64, total 128 PI, 1UI = 32 PI
        //For data rate 3200, max tDQS2DQ is 2.56UI (82 PI)
        //For data rate 4266, max tDQS2DQ is 3.41UI (109 PI)
        for (uiDelay = u2DQDelayBegin; uiDelay <=u2DQDelayEnd; uiDelay++)
        //    for (uiDelay = u2DQDelayBegin; uiDelay <=u2DQDelayEnd; uiDelay+=3)  //DBI test
        {
            TxWinTransferDelayToUIPI(p, uiDelay, dq_ui_small_bak, dq_ui_large_bak, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi);
            TxWinTransferDelayToUIPI(p, uiDelay, dq_oen_ui_small_bak, dq_oen_ui_large_bak, &ucdq_oen_ui_large, &ucdq_oen_ui_small, &ucdq_pi);

            if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
            {
            //TXDLY_DQ , TXDLY_OEN_DQ
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0), \
                                            P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ0) | \
                                            P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ1) | \
                                            P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ2) | \
                                            P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ3) | \
                                            P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) | \
                                            P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) | \
                                            P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) | \
                                            P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3));

            // DLY_DQ[2:0]
           vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2), \
                                            P_Fld(ucdq_ui_small, SHURK0_SELPH_DQ2_DLY_DQ0) | \
                                            P_Fld(ucdq_ui_small, SHURK0_SELPH_DQ2_DLY_DQ1) | \
                                            P_Fld(ucdq_ui_small, SHURK0_SELPH_DQ2_DLY_DQ2) | \
                                            P_Fld(ucdq_ui_small, SHURK0_SELPH_DQ2_DLY_DQ3) | \
                                            P_Fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ2_DLY_OEN_DQ0) | \
                                            P_Fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ2_DLY_OEN_DQ1) | \
                                            P_Fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ2_DLY_OEN_DQ2) | \
                                            P_Fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ2_DLY_OEN_DQ3));
            }

            if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
            {
                //TXDLY_DQM , TXDLY_OEN_DQM
                vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1), \
                                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM0) | \
                                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM1) | \
                                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM2) | \
                                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM3) | \
                                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) | \
                                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) | \
                                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) | \
                                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3));

                // DLY_DQM[2:0]
               vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3), \
                                                P_Fld(ucdq_ui_small, SHURK0_SELPH_DQ3_DLY_DQM0) | \
                                                P_Fld(ucdq_ui_small, SHURK0_SELPH_DQ3_DLY_DQM1) | \
                                                P_Fld(ucdq_ui_small, SHURK0_SELPH_DQ3_DLY_DQM2) | \
                                                P_Fld(ucdq_ui_small, SHURK0_SELPH_DQ3_DLY_DQM3) | \
                                                P_Fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ3_DLY_OEN_DQM0) | \
                                                P_Fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ3_DLY_OEN_DQM1) | \
                                                P_Fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ3_DLY_OEN_DQM2) | \
                                                P_Fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ3_DLY_OEN_DQM3));
            }

            //set to registers, PI DQ (per byte)

		//MT8167 TX DQ/DQM
		// set to best values for  DQM, DQ
		#if 0
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_pi, SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
							P_Fld(ucdq_pi, SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_pi, SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
							P_Fld(ucdq_pi, SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_pi, SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
							P_Fld(ucdq_pi, SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_pi, SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
							P_Fld(ucdq_pi, SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
		#endif

		if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
		{
			// set to best values for  DQ
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_pi, SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_pi, SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_pi, SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_pi, SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
		}
		if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
		{
			// set to best values for  DQM
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_pi, SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_pi, SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_pi, SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_pi, SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1));
		}

            u4err_value= TestEngineCompare(p);

            #if VREF_SPEED_UP_LP4
            if(u1VrefScanEnable==0)
            #endif
            {
                //mcSHOW_DBG_MSG(("Delay=%3d |%2d %2d %3d| %2d %2d| 0x%8x [0]",uiDelay, ucdq_ui_large,ucdq_ui_small, ucdq_pi, ucdq_oen_ui_large,ucdq_oen_ui_small, u4err_value));
                #ifdef ETT_PRINT_FORMAT
                if(u4err_value !=0)
                {
                    mcSHOW_DBG_MSG2(("%d |%d %d %d|[0]",uiDelay, ucdq_ui_large,ucdq_ui_small, ucdq_pi));
                }
                #else
                mcSHOW_DBG_MSG2(("Delay=%3d |%2d %2d %3d| 0x%8x [0]",uiDelay, ucdq_ui_large,ucdq_ui_small, ucdq_pi, u4err_value));
                #endif
                mcFPRINTF((fp_A60501, "Delay=%3d | %2d %2d %3d| 0x%8x [0]",uiDelay, ucdq_ui_large,ucdq_ui_small, ucdq_pi, u4err_value));
            }

            // check fail bit ,0 ok ,others fail
            for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
            {
                u4fail_bit = u4err_value&((U32)1<<u1BitIdx);
#ifdef MT8167_HQA_TEST
                if ((uiDelay-u2DQDelayBegin) < RET_LEN)
                    ScanRet[ScanIndex][uiDelay-u2DQDelayBegin] |= (u4fail_bit ^ (1<<u1BitIdx));
#endif
                #if VREF_SPEED_UP_LP4
                if(u1VrefScanEnable==0)
                #endif
                {
                    if(u4err_value != 0)
                    {
                        if(u1BitIdx%DQS_BIT_NUMBER ==0)
                        {
                            mcSHOW_DBG_MSG2((" "));
                            mcFPRINTF((fp_A60501, " "));
                        }

                        if (u4fail_bit == 0)
                        {
                            mcSHOW_DBG_MSG2(("o"));
                            mcFPRINTF((fp_A60501, "o"));
                        }
                        else
                        {
                                mcSHOW_DBG_MSG2(("x"));
                                mcFPRINTF((fp_A60501, "x"));
                        }
                    }
                }

                if(WinPerBit[u1BitIdx].first_pass== PASS_RANGE_NA)
                {
                    if(u4fail_bit==0) //compare correct: pass
                    {
                        WinPerBit[u1BitIdx].first_pass = uiDelay;
                    }
                }
                else if(WinPerBit[u1BitIdx].last_pass == PASS_RANGE_NA)
                {
                    if(u4fail_bit !=0) //compare error : fail
                    {
                        WinPerBit[u1BitIdx].last_pass  = (uiDelay-1);
                    }
                    else if (uiDelay==u2DQDelayEnd)
                    //else if (uiDelay==MAX_TX_DQDLY_TAPS)
                    {
                        WinPerBit[u1BitIdx].last_pass  = uiDelay;
                    }

                    if(WinPerBit[u1BitIdx].last_pass  !=PASS_RANGE_NA)
                    {
                        if((WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass) >= (VrefWinPerBit[u1BitIdx].last_pass -VrefWinPerBit[u1BitIdx].first_pass))
                        {
                            #if 0 //for debug
                            if(VrefWinPerBit[u1BitIdx].last_pass != PASS_RANGE_NA)
                            {
                                mcSHOW_DBG_MSG2(("Bit[%d] Bigger window update %d > %d\n", u1BitIdx, \
                                    (WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass), (VrefWinPerBit[u1BitIdx].last_pass -VrefWinPerBit[u1BitIdx].first_pass)));
                                mcFPRINTF((fp_A60501,"Bit[%d] Bigger window update %d > %d\n", u1BitIdx, \
                                    (WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass), (VrefWinPerBit[u1BitIdx].last_pass -VrefWinPerBit[u1BitIdx].first_pass)));

                            }
                            #endif
                            uiFinishCount |= (1<<u1BitIdx);
                            //update bigger window size
                            VrefWinPerBit[u1BitIdx].first_pass = WinPerBit[u1BitIdx].first_pass;
                            VrefWinPerBit[u1BitIdx].last_pass = WinPerBit[u1BitIdx].last_pass;
                        }

                        //reset tmp window
                        WinPerBit[u1BitIdx].first_pass = PASS_RANGE_NA;
                        WinPerBit[u1BitIdx].last_pass = PASS_RANGE_NA;
                    }
                 }
               }

                #if VREF_SPEED_UP_LP4
                if(u1VrefScanEnable==0)
                #endif
                {
                    if(u4err_value != 0)
                    {
                        mcSHOW_DBG_MSG2((" [MSB]\n"));
                        mcFPRINTF((fp_A60501, " [MSB]\n"));
                    }
                }

                //if all bits widnow found and all bits turns to fail again, early break;
            	if(((p->data_width== DATA_WIDTH_16BIT) &&(uiFinishCount == 0xffff)) || \
					((p->data_width== DATA_WIDTH_32BIT) &&(uiFinishCount == 0xffffffff)) || \
					((p->swap_byte_16bit == ENABLE) &&(uiFinishCount == 0xffff0000)))
                    {
                        vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_OK);
                        #if !VREF_SPEED_UP_LP4
                        if(u1VrefScanEnable ==0)
                        #endif
                        {
                                if(((p->data_width== DATA_WIDTH_16BIT) &&((u4err_value&0xffff) == 0xffff)) || \
								((p->data_width== DATA_WIDTH_32BIT) &&(u4err_value == 0xffffffff)) || \
								((p->swap_byte_16bit == ENABLE) &&((u4err_value&0xffff0000) == 0xffff0000)))
                                {
                                    #if !REDUCE_LOG_FOR_PRELOADER
                                        #ifdef ETT_PRINT_FORMAT
                                        mcSHOW_DBG_MSG2(("TX calibration finding left boundary early break. PI DQ delay=0x%B\n", uiDelay));
                                        #else
                                        mcSHOW_DBG_MSG2(("TX calibration finding left boundary early break. PI DQ delay=0x%2x\n", uiDelay));
                                        #endif
                                    #endif
                                    break;  //early break
                                }
                        }
                    }
        }

		if (p->swap_byte_16bit == ENABLE) {

			for (u1BitIdx = 0; u1BitIdx < (p->data_width)/2; u1BitIdx++)
			{
				memcpy(&VrefWinPerBit[u1BitIdx], &VrefWinPerBit[u1BitIdx+16], sizeof(PASS_WIN_DATA_T));
			}
		}
        if (p->en_4bitMux == ENABLE)
        {
            mcSHOW_DBG_MSG(("Tx 4bitMux is enabled\n"));

			#if 1
			for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				mcSHOW_DBG_MSG(("%d:%d ", VrefWinPerBit[u1BitIdx].first_pass, VrefWinPerBit[u1BitIdx].last_pass));
			}
			mcSHOW_DBG_MSG(("\n"));
			#endif

			for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				memcpy(&WinPerBit[Bit_DQ_Mapping[u1BitIdx]], &VrefWinPerBit[u1BitIdx], sizeof(PASS_WIN_DATA_T));
			}
			memcpy(VrefWinPerBit, WinPerBit, sizeof(PASS_WIN_DATA_T) * DQ_DATA_WIDTH);

			#if 1
			for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
			{
				mcSHOW_DBG_MSG(("%d:%d ", VrefWinPerBit[u1BitIdx].first_pass, VrefWinPerBit[u1BitIdx].last_pass));
				mcSHOW_DBG_MSG(("\n"));
			}
			#endif

        }

        min_winsize = 0xffff;
        min_bit = 0xff;
        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            VrefWinPerBit[u1BitIdx].win_size = VrefWinPerBit[u1BitIdx].last_pass- VrefWinPerBit[u1BitIdx].first_pass +1;

            if (VrefWinPerBit[u1BitIdx].win_size < min_winsize)
            {
                min_bit = u1BitIdx;
                min_winsize = VrefWinPerBit[u1BitIdx].win_size;
            }

            u2TempWinSum += VrefWinPerBit[u1BitIdx].win_size;  //Sum of CA Windows for vref selection

            #if ENABLE_CLAIBTAION_WINDOW_LOG_FOR_FT
            if(VrefWinPerBit[u1BitIdx].win_size<u2MinWinSize)
            {
                u2MinWinSize = VrefWinPerBit[u1BitIdx].win_size;
                u1MinWinSizeBitidx = u1BitIdx;
            }
            #endif
        }
        mcSHOW_DBG_MSG3(("Fra Min Bit=%d, winsize=%d ===\n",min_bit, min_winsize));

        #if ENABLE_CLAIBTAION_WINDOW_LOG_FOR_FT
        mcSHOW_DBG_MSG2(("FT log: TX min window : bit %d, size %d\n", u1MinWinSizeBitidx, u2MinWinSize));
        #endif
        #if !REDUCE_LOG_FOR_PRELOADER
        mcSHOW_DBG_MSG2(("\nTX Vref %d, Window Sum %d > %d\n", u2VrefLevel, u2TempWinSum, u2tx_window_sum));
        mcFPRINTF((fp_A60501, "\nTX Vref %d, Window Sum %d > %d\n", u2VrefLevel, u2TempWinSum, u2tx_window_sum));
        #endif

        u1PrintWinData =0;
        if ((min_winsize > TXPerbitWin_min_max) || ((min_winsize == TXPerbitWin_min_max) && (u2TempWinSum >u2tx_window_sum)))
        {
            #if REDUCE_LOG_FOR_PRELOADER
            u1PrintWinData =0;
            #else
            u1PrintWinData =1;
            #endif

            {
                mcSHOW_DBG_MSG3(("\nBetter TX Vref found %d, Window Sum %d > %d\n", u2VrefLevel, u2TempWinSum, u2tx_window_sum));
                mcFPRINTF((fp_A60501, "\nBetter TX Vref found %d, Window Sum %d > %d\n", u2VrefLevel, u2TempWinSum, u2tx_window_sum));
            }

            TXPerbitWin_min_max = min_winsize;
            u2tx_window_sum =u2TempWinSum;
            u2FinalRange = u2VrefRange;
            u2FinalVref = u2VrefLevel;

            //Calculate the center of DQ pass window
            // Record center sum of each byte
            for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
            {
                #if TX_DQM_CALC_MAX_MIN_CENTER
                u2Center_min[u1ByteIdx] = 0xffff;
                u2Center_max[u1ByteIdx] = 0;
                #else
                s2sum_dly[u1ByteIdx] = 0;
                #endif

                for (u1BitIdx=0; u1BitIdx<DQS_BIT_NUMBER; u1BitIdx++)
                {
                    ucindex = u1ByteIdx * DQS_BIT_NUMBER + u1BitIdx;
                    FinalWinPerBit[ucindex].first_pass = VrefWinPerBit[ucindex].first_pass;
                    FinalWinPerBit[ucindex].last_pass =  VrefWinPerBit[ucindex].last_pass;
                    FinalWinPerBit[ucindex].win_size = VrefWinPerBit[ucindex].win_size;
                    FinalWinPerBit[ucindex].win_center = (FinalWinPerBit[ucindex].first_pass + FinalWinPerBit[ucindex].last_pass) >> 1;

                    #if TX_DQM_CALC_MAX_MIN_CENTER
                    if(FinalWinPerBit[ucindex].win_center < u2Center_min[u1ByteIdx])
                        u2Center_min[u1ByteIdx] = FinalWinPerBit[ucindex].win_center;

                    if(FinalWinPerBit[ucindex].win_center > u2Center_max[u1ByteIdx])
                        u2Center_max[u1ByteIdx] = FinalWinPerBit[ucindex].win_center;
                    #else
                        s2sum_dly[u1ByteIdx] += FinalWinPerBit[ucindex].win_center;
                    #endif

                    #ifdef FOR_HQA_TEST_USED
                    gFinalTXPerbitWin[p->channel][p->rank][ucindex] = FinalWinPerBit[ucindex].win_size;
                    #endif
                    #if SUPPORT_TX_DELAY_LINE_CALI
                        if (s1LastPassMin[u1ByteIdx] > FinalWinPerBit[ucindex].last_pass)
	                        s1LastPassMin[u1ByteIdx] = FinalWinPerBit[ucindex].last_pass;
                    #endif
                }
            }
        }


        if(u1PrintWinData)
        {
            //mcSHOW_DBG_MSG(("==================================================================\n"));
            mcSHOW_DBG_MSG(("channel=%d, VrefRange= %d, u2VrefLevel = %d\n", p->channel, u2VrefRange, u2VrefLevel));
            //mcSHOW_DBG_MSG(("PI DQ (per byte) window\nx=pass dq delay value (min~max)center \ny=0-7bit DQ of every group\n"));
            //mcSHOW_DBG_MSG(("==================================================================\n"));
            //mcSHOW_DBG_MSG(("bit    Byte0    bit      Byte1     bit     Byte2     bit     Byte3\n"));
            vPrintCalibrationBasicInfo(p);

            mcFPRINTF((fp_A60501,"==================================================================\n"));
            mcFPRINTF((fp_A60501,"    channel=%d(2:cha, 3:chb)  u2VrefLevel = %d\n", p->channel, u2VrefLevel));
            mcFPRINTF((fp_A60501,"PI DQ (per byte) window\nx=pass dq delay value (min~max)center \ny=0-7bit DQ of every group\n"));
            mcFPRINTF((fp_A60501,"==================================================================\n"));
            mcFPRINTF((fp_A60501,"bit   Byte0    bit      Byte1     bit     Byte2     bit     Byte3\n"));

            for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx++)
            {
                #ifdef ETT_PRINT_FORMAT
                mcSHOW_DBG_MSG(("TX Bit%d (%d~%d) %d %d,   Bit%d (%d~%d) %d %d,", \
                    u1BitIdx, FinalWinPerBit[u1BitIdx].first_pass, FinalWinPerBit[u1BitIdx].last_pass, FinalWinPerBit[u1BitIdx].win_size, FinalWinPerBit[u1BitIdx].win_center, \
                    u1BitIdx+8, FinalWinPerBit[u1BitIdx+8].first_pass, FinalWinPerBit[u1BitIdx+8].last_pass, FinalWinPerBit[u1BitIdx+8].win_size, FinalWinPerBit[u1BitIdx+8].win_center));
                #else
                mcSHOW_DBG_MSG(("TX Bit%2d (%2d~%2d) %2d %2d,   Bit%2d (%2d~%2d) %2d %2d,", \
                    u1BitIdx, FinalWinPerBit[u1BitIdx].first_pass, FinalWinPerBit[u1BitIdx].last_pass, FinalWinPerBit[u1BitIdx].win_size, FinalWinPerBit[u1BitIdx].win_center, \
                    u1BitIdx+8, FinalWinPerBit[u1BitIdx+8].first_pass, FinalWinPerBit[u1BitIdx+8].last_pass, FinalWinPerBit[u1BitIdx+8].win_sizes, FinalWinPerBit[u1BitIdx+8].win_center));
                #endif
                mcFPRINTF((fp_A60501,"TX Bit%2d (%2d~%2d) %2d %2d,   Bit%2d (%2d~%2d) %2d %2d,", \
                    u1BitIdx, FinalWinPerBit[u1BitIdx].first_pass, FinalWinPerBit[u1BitIdx].last_pass, FinalWinPerBit[u1BitIdx].win_size, FinalWinPerBit[u1BitIdx].win_center, \
                    u1BitIdx+8, FinalWinPerBit[u1BitIdx+8].first_pass, FinalWinPerBit[u1BitIdx+8].last_pass, FinalWinPerBit[u1BitIdx+8].win_size, FinalWinPerBit[u1BitIdx+8].win_center));
                    #ifdef ETT_PRINT_FORMAT
                    mcSHOW_DBG_MSG(("  %d (%d~%d) %d %d,   %d (%d~%d) %d %d\n", \
                        u1BitIdx+16, FinalWinPerBit[u1BitIdx+16].first_pass, FinalWinPerBit[u1BitIdx+16].last_pass, FinalWinPerBit[u1BitIdx+16].win_size, FinalWinPerBit[u1BitIdx+16].win_center, \
                        u1BitIdx+24, FinalWinPerBit[u1BitIdx+24].first_pass, FinalWinPerBit[u1BitIdx+24].last_pass, FinalWinPerBit[u1BitIdx+24].win_size, FinalWinPerBit[u1BitIdx+24].win_center ));
                    #else
                    mcSHOW_DBG_MSG(("  %2d (%2d~%2d) %2d %2d,   %2d (%2d~%2d) %2d %2d\n", \
                        u1BitIdx+16, FinalWinPerBit[u1BitIdx+16].first_pass, FinalWinPerBit[u1BitIdx+16].last_pass, FinalWinPerBit[u1BitIdx+16].win_size, FinalWinPerBit[u1BitIdx+16].win_center, \
                        u1BitIdx+24, FinalWinPerBit[u1BitIdx+24].first_pass, FinalWinPerBit[u1BitIdx+24].last_pass, FinalWinPerBit[u1BitIdx+24].win_size, FinalWinPerBit[u1BitIdx+24].win_center ));
                    #endif

                    mcFPRINTF((fp_A60501,"  %2d (%2d~%2d) %2d %2d,   %2d (%2d~%2d) %2d %2d\n", \
                        u1BitIdx+16, FinalWinPerBit[u1BitIdx+16].first_pass, FinalWinPerBit[u1BitIdx+16].last_pass, FinalWinPerBit[u1BitIdx+16].win_size, FinalWinPerBit[u1BitIdx+16].win_center, \
                        u1BitIdx+24, FinalWinPerBit[u1BitIdx+24].first_pass, FinalWinPerBit[u1BitIdx+24].last_pass, FinalWinPerBit[u1BitIdx+24].win_size, FinalWinPerBit[u1BitIdx+24].win_center ));

            }
            mcSHOW_DBG_MSG(("\n==================================================================\n"));
            mcFPRINTF((fp_A60501,"\n==================================================================\n"));
        }
        #if VREF_SPEED_UP_LP4
        else if(u2TempWinSum < (u2tx_window_sum*95/100))
        {
            mcSHOW_DBG_MSG(("\nTX Vref found, early break!\n"));
            break;//max vref found, early break;
        }
        #endif
    }


#ifdef FOR_HQA_TEST_USED
    gFinalTXPerbitWin_min_max[p->channel][p->rank] = TXPerbitWin_min_max;
#endif
#if SUPPORT_TX_DELAY_LINE_CALI
	for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
	{
		s1LastPassMin[u1ByteIdx] -= (s1LastPassMin[u1ByteIdx] - u2Center_min[u1ByteIdx]) >> 1;
		if(s1LastPassMin[u1ByteIdx] < 0)
			s1LastPassMin[u1ByteIdx] = 0;	
		TxWinTransferDelayToUIPI(p, s1LastPassMin[u1ByteIdx], dq_ui_small_bak, dq_ui_large_bak, &ucdq_final_ui_large[u1ByteIdx], &ucdq_final_ui_small[u1ByteIdx], &ucdq_final_pi[u1ByteIdx]);
		TxWinTransferDelayToUIPI(p, s1LastPassMin[u1ByteIdx], dq_oen_ui_small_bak, dq_oen_ui_large_bak, &ucdq_final_oen_ui_large[u1ByteIdx], &ucdq_final_oen_ui_small[u1ByteIdx], &ucdq_final_pi[u1ByteIdx]);
		/* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
		for(ii=p->rank; ii<RANK_MAX; ii++)
		{
		    vSetRank(p,ii);
		    if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
		    {
		    //TXDLY_DQ , TXDLY_OEN_DQ
		     vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0), \
		                                     P_Fld(ucdq_final_ui_large[0], SHURK0_SELPH_DQ0_TXDLY_DQ0) | \
		                                     P_Fld(ucdq_final_ui_large[1], SHURK0_SELPH_DQ0_TXDLY_DQ1) | \
		                                     P_Fld(ucdq_final_ui_large[2], SHURK0_SELPH_DQ0_TXDLY_DQ2) | \
		                                     P_Fld(ucdq_final_ui_large[3], SHURK0_SELPH_DQ0_TXDLY_DQ3) | \
		                                     P_Fld(ucdq_final_oen_ui_large[0], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) | \
		                                     P_Fld(ucdq_final_oen_ui_large[1], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) | \
		                                     P_Fld(ucdq_final_oen_ui_large[2], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) | \
		                                     P_Fld(ucdq_final_oen_ui_large[3], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3));

		     // DLY_DQ[2:0]
		    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2), \
		                                     P_Fld(ucdq_final_ui_small[0], SHURK0_SELPH_DQ2_DLY_DQ0) | \
		                                     P_Fld(ucdq_final_ui_small[1], SHURK0_SELPH_DQ2_DLY_DQ1) | \
		                                     P_Fld(ucdq_final_ui_small[2], SHURK0_SELPH_DQ2_DLY_DQ2) | \
		                                     P_Fld(ucdq_final_ui_small[3], SHURK0_SELPH_DQ2_DLY_DQ3) | \
		                                     P_Fld(ucdq_final_oen_ui_small[0], SHURK0_SELPH_DQ2_DLY_OEN_DQ0) | \
		                                     P_Fld(ucdq_final_oen_ui_small[1], SHURK0_SELPH_DQ2_DLY_OEN_DQ1) | \
		                                     P_Fld(ucdq_final_oen_ui_small[2], SHURK0_SELPH_DQ2_DLY_OEN_DQ2) | \
		                                     P_Fld(ucdq_final_oen_ui_small[3], SHURK0_SELPH_DQ2_DLY_OEN_DQ3));
		    }
		    if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
		    {
		        //TXDLY_DQM , TXDLY_OEN_DQM
		     vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1), \
		                                     P_Fld(ucdq_final_ui_large[0], SHURK0_SELPH_DQ1_TXDLY_DQM0) | \
		                                     P_Fld(ucdq_final_ui_large[1], SHURK0_SELPH_DQ1_TXDLY_DQM1) | \
		                                     P_Fld(ucdq_final_ui_large[2], SHURK0_SELPH_DQ1_TXDLY_DQM2) | \
		                                     P_Fld(ucdq_final_ui_large[3], SHURK0_SELPH_DQ1_TXDLY_DQM3) | \
		                                     P_Fld(ucdq_final_oen_ui_large[0], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) | \
		                                     P_Fld(ucdq_final_oen_ui_large[1], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) | \
		                                     P_Fld(ucdq_final_oen_ui_large[2], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) | \
		                                     P_Fld(ucdq_final_oen_ui_large[3], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3));

		     // DLY_DQM[2:0]
		     vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3), \
		                                     P_Fld(ucdq_final_ui_small[0], SHURK0_SELPH_DQ3_DLY_DQM0) | \
		                                     P_Fld(ucdq_final_ui_small[1], SHURK0_SELPH_DQ3_DLY_DQM1) | \
		                                     P_Fld(ucdq_final_ui_small[2], SHURK0_SELPH_DQ3_DLY_DQM2) | \
		                                     P_Fld(ucdq_final_ui_small[3], SHURK0_SELPH_DQ3_DLY_DQM3) | \
		                                     P_Fld(ucdq_final_oen_ui_small[0], SHURK0_SELPH_DQ3_DLY_OEN_DQM0) | \
		                                     P_Fld(ucdq_final_oen_ui_small[1], SHURK0_SELPH_DQ3_DLY_OEN_DQM1) | \
		                                     P_Fld(ucdq_final_oen_ui_small[2], SHURK0_SELPH_DQ3_DLY_OEN_DQM2) | \
		                                     P_Fld(ucdq_final_oen_ui_small[3], SHURK0_SELPH_DQ3_DLY_OEN_DQM3));
		   }
		}
		vSetRank(p, backup_rank);

		//set to registers, PI DQ (per byte)
		/* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
		for(ii=p->rank; ii<RANK_MAX; ii++)
		{
			vSetRank(p,ii);

			//MT8167 TX DQ/DQM
			// set to best values for  DQM, DQ
#if 0
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
								P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
								P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
								P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
								P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
#endif
			if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
			{

				//MT8167 TX DQ/DQM
				// set to best values for  DQM, DQ
				vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
				vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
				vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
				vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
			}
			if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
			{
				//MT8167 TX DQ/DQM
				// set to best values for  DQM, DQ
				vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0));
				vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1));
				vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0));
				vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1));

			}
		}
		vSetRank(p, backup_rank);
	}

	DramcTxWindowPerbitCalDelayLine((DRAMC_CTX_T *) p, TX_DQ_DQS_MOVE_DQ_DQM);
	mcSHOW_DBG_MSG(("	PIEND	OFFSET	PISTEP	DLYSTEP FACTOR\n"));
	for(ii=0; ii<p->data_width; ii++) {
		if (FinalWinPerBit[ii].last_pass-s1LastPassMin[ii>>3] == 0)
			continue;
		s2Factor = (DlyPerBit[ii].win_size - 1) * 1000 / (FinalWinPerBit[ii].last_pass-s1LastPassMin[ii>>3]);
		s2FactorMean += s2Factor;
		mcSHOW_DBG_MSG(("BIT%d	%d	%d	%d	%d	%d\n", ii, FinalWinPerBit[ii].last_pass, s1LastPassMin[ii>>3], \
			FinalWinPerBit[ii].last_pass-s1LastPassMin[ii>>3], DlyPerBit[ii].win_size - 1, s2Factor));
		s2Cnt ++;
	}
	if (s2Cnt > 0)
		s2FactorMean /= s2Cnt;
	mcSHOW_DBG_MSG(("factor_mean	%d\n", s2FactorMean));
	for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++) {
		for (u1BitIdx=0; u1BitIdx<DQS_BIT_NUMBER; u1BitIdx++)
		{
			ucindex = u1ByteIdx * DQS_BIT_NUMBER + u1BitIdx;		
			iDQ_PerBit_DelayLine[ucindex] = ((FinalWinPerBit[ucindex].win_center - u2Center_min[u1ByteIdx]) * s2FactorMean + 300) / 1000;
			iDQMDlyPerbyte[u1ByteIdx] += iDQ_PerBit_DelayLine[ucindex];			
			mcSHOW_DBG_MSG(("BIT%d perbit delay line = %d\n", ucindex, iDQ_PerBit_DelayLine[ucindex]));
		}
		iDQMDlyPerbyte[u1ByteIdx] /= DQS_BIT_NUMBER;	
		mcSHOW_DBG_MSG(("DQM%d perbit delay line = %d\n", u1ByteIdx, iDQMDlyPerbyte[u1ByteIdx]));
	}
	/* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
	for(ii=p->rank; ii<RANK_MAX; ii++)
	{
        vSetRank(p,ii);
        if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
        {
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0, \
											P_Fld(iDQ_PerBit_DelayLine[7], SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[6], SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[5], SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[4], SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[3], SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[2], SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[1], SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[0], SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ0, \
											P_Fld(iDQ_PerBit_DelayLine[15], SHU1_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[14], SHU1_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[13], SHU1_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[12], SHU1_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[11], SHU1_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[10], SHU1_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[9], SHU1_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[8], SHU1_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0+(1<<POS_BANK_NUM), \
											P_Fld(iDQ_PerBit_DelayLine[23], SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[22], SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[21], SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[20], SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[19], SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[18], SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[17], SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
											P_Fld(iDQ_PerBit_DelayLine[16], SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
			vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ0+(1<<POS_BANK_NUM), \
											P_Fld(iDQ_PerBit_DelayLine[31], SHU1_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[30], SHU1_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[29], SHU1_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[28], SHU1_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[27], SHU1_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[26], SHU1_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[25], SHU1_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) | \
											P_Fld(iDQ_PerBit_DelayLine[24], SHU1_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));

        }

        if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
        {
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1, iDQMDlyPerbyte[0], SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);  // rank0, B0
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1, iDQMDlyPerbyte[1], SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);  // rank0, B1
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1+(1<<POS_BANK_NUM), iDQMDlyPerbyte[2], SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);  // rank0, B0
			vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1+(1<<POS_BANK_NUM), iDQMDlyPerbyte[3], SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);  // rank0, B1
        }
    }
    vSetRank(p, backup_rank);	
#endif

    //Calculate the center of DQ pass window
    //average the center delay
    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
    {
	#if SUPPORT_TX_DELAY_LINE_CALI
        uiDelay = u2Center_min[u1ByteIdx];
	#else
        #if TX_DQM_CALC_MAX_MIN_CENTER
        uiDelay = ((u2Center_min[u1ByteIdx] + u2Center_max[u1ByteIdx])>>1); //(max +min)/2
        #else
        s1temp1 = s2sum_dly[u1ByteIdx] /DQS_BIT_NUMBER;
        s1temp2 = s1temp1+1;

        if ((s2sum_dly[u1ByteIdx] -s1temp1*DQS_BIT_NUMBER) > (s1temp2*DQS_BIT_NUMBER-s2sum_dly[u1ByteIdx] ))
        {
            uiDelay = (U16)s1temp2;
        }
        else
        {
            uiDelay = (U16)s1temp1;
        }
        #endif
	#endif
        #ifdef FOR_HQA_TEST_USED
        U16 u2CenterDiff;

        gFinalTXPerbitWin_min_margin[p->channel][p->rank] = 0xffff;

        for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx++)
        {
            u2CenterDiff= uiDelay- FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].first_pass;  // left margin

            if(u2CenterDiff <gFinalTXPerbitWin_min_margin[p->channel][p->rank])
            {
                gFinalTXPerbitWin_min_margin[p->channel][p->rank] = u2CenterDiff;
                gFinalTXPerbitWin_min_margin_bit[p->channel][p->rank] =u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx;
                //mcSHOW_DBG_MSG(("Margin Bit%d, u2CenterDiff %d, Delay %d (%d~%d)\n",  gFinalTXPerbitWin_min_margin_bit[p->channel][p->rank], u2CenterDiff, uiDelay,
                //    FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].first_pass, FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].last_pass));
            }

            u2CenterDiff= FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].last_pass - uiDelay;// right mragin

            if(u2CenterDiff <gFinalTXPerbitWin_min_margin[p->channel][p->rank])
            {
                gFinalTXPerbitWin_min_margin[p->channel][p->rank] = u2CenterDiff;
                gFinalTXPerbitWin_min_margin_bit[p->channel][p->rank] =u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx;

                //mcSHOW_DBG_MSG(("margin Bit%d, u2CenterDiff %d, Delay %d (%d~%d)\n", gFinalTXPerbitWin_min_margin_bit[p->channel][p->rank], u2CenterDiff, uiDelay,
                //    FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].first_pass, FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].last_pass));
            }
        }

        #endif

        TxWinTransferDelayToUIPI(p, uiDelay, dq_ui_small_bak, dq_ui_large_bak, &ucdq_final_ui_large[u1ByteIdx], &ucdq_final_ui_small[u1ByteIdx], &ucdq_final_pi[u1ByteIdx]);
        TxWinTransferDelayToUIPI(p, uiDelay, dq_oen_ui_small_bak, dq_oen_ui_large_bak, &ucdq_final_oen_ui_large[u1ByteIdx], &ucdq_final_oen_ui_small[u1ByteIdx], &ucdq_final_pi[u1ByteIdx]);

        mcSHOW_DBG_MSG(("Byte%d, PI DQ Delay %d\n",  u1ByteIdx, uiDelay));
        mcSHOW_DBG_MSG(("Final DQ PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n", ucdq_final_ui_large[u1ByteIdx], ucdq_final_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));
        mcSHOW_DBG_MSG(("OEN DQ PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n\n", ucdq_final_oen_ui_large[u1ByteIdx], ucdq_final_oen_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));

        mcFPRINTF((fp_A60501,"Byte%d, PI DQ Delay %d\n",  u1ByteIdx, uiDelay));
        mcFPRINTF((fp_A60501,"Final DQ PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n", ucdq_final_ui_large[u1ByteIdx], ucdq_final_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));
        mcFPRINTF((fp_A60501,"OEN DQ PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n\n", ucdq_final_oen_ui_large[u1ByteIdx], ucdq_final_oen_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));

#ifdef DRAM_CALIB_LOG
        gDRAM_CALIB_LOG.CHANNEL[p->channel].RANK[p->rank].TxWindowPerbitCal.PI_DQ_elay[u1ByteIdx] = uiDelay;
        gDRAM_CALIB_LOG.CHANNEL[p->channel].RANK[p->rank].TxWindowPerbitCal.Large_UI[u1ByteIdx] = ucdq_final_ui_large[u1ByteIdx];
        gDRAM_CALIB_LOG.CHANNEL[p->channel].RANK[p->rank].TxWindowPerbitCal.Small_UI[u1ByteIdx] = ucdq_final_ui_small[u1ByteIdx];
        gDRAM_CALIB_LOG.CHANNEL[p->channel].RANK[p->rank].TxWindowPerbitCal.PI[u1ByteIdx] = ucdq_final_pi[u1ByteIdx];
#endif
    }

     // SET tx Vref (DQ) = u2FinalVref, LP3 no need to set this.
    if(u1VrefScanEnable)
    {
        u1MR14Value[p->channel][p->dram_fsp] = (u2FinalVref | (u2FinalRange<<6));
        DramcModeRegWriteByRank(p, p->rank, 14, u2FinalVref | (u2FinalRange<<6));

        mcSHOW_DBG_MSG(("\nFinal TX Range %d Vref %d\n\n", u2FinalRange, u2FinalVref));
        mcFPRINTF((fp_A60501, "\nFinal TX Range %d Vref %d\n\n", u2FinalRange, u2FinalVref));

#ifdef FOR_HQA_TEST_USED
        gFinalTXVrefDQ[p->channel][p->rank] = u2FinalVref;
#endif
    }

    // BU request RX & TX window size log.
    #if 0//def RELEASE  // for parsing tool
    if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY)
    {
        mcSHOW_DBG_MSG4(("TX CH%d R%d ,Freq %d\n", p->channel, p->rank, p->frequency));
        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            mcSHOW_DBG_MSG4(("%d: %d\n", u1BitIdx, gFinalTXPerbitWin[p->channel][p->rank][u1BitIdx]));
        }
    }
    #endif

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =1;
#endif

    /* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
    for(ii=p->rank; ii<RANK_MAX; ii++)
    {
            vSetRank(p,ii);
            if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
            {
            //TXDLY_DQ , TXDLY_OEN_DQ
             vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0), \
                                             P_Fld(ucdq_final_ui_large[0], SHURK0_SELPH_DQ0_TXDLY_DQ0) | \
                                             P_Fld(ucdq_final_ui_large[1], SHURK0_SELPH_DQ0_TXDLY_DQ1) | \
                                             P_Fld(ucdq_final_ui_large[2], SHURK0_SELPH_DQ0_TXDLY_DQ2) | \
                                             P_Fld(ucdq_final_ui_large[3], SHURK0_SELPH_DQ0_TXDLY_DQ3) | \
                                             P_Fld(ucdq_final_oen_ui_large[0], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) | \
                                             P_Fld(ucdq_final_oen_ui_large[1], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) | \
                                             P_Fld(ucdq_final_oen_ui_large[2], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) | \
                                             P_Fld(ucdq_final_oen_ui_large[3], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3));

             // DLY_DQ[2:0]
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2), \
                                             P_Fld(ucdq_final_ui_small[0], SHURK0_SELPH_DQ2_DLY_DQ0) | \
                                             P_Fld(ucdq_final_ui_small[1], SHURK0_SELPH_DQ2_DLY_DQ1) | \
                                             P_Fld(ucdq_final_ui_small[2], SHURK0_SELPH_DQ2_DLY_DQ2) | \
                                             P_Fld(ucdq_final_ui_small[3], SHURK0_SELPH_DQ2_DLY_DQ3) | \
                                             P_Fld(ucdq_final_oen_ui_small[0], SHURK0_SELPH_DQ2_DLY_OEN_DQ0) | \
                                             P_Fld(ucdq_final_oen_ui_small[1], SHURK0_SELPH_DQ2_DLY_OEN_DQ1) | \
                                             P_Fld(ucdq_final_oen_ui_small[2], SHURK0_SELPH_DQ2_DLY_OEN_DQ2) | \
                                             P_Fld(ucdq_final_oen_ui_small[3], SHURK0_SELPH_DQ2_DLY_OEN_DQ3));
            }
            if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
            {
                //TXDLY_DQM , TXDLY_OEN_DQM
             vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1), \
                                             P_Fld(ucdq_final_ui_large[0], SHURK0_SELPH_DQ1_TXDLY_DQM0) | \
                                             P_Fld(ucdq_final_ui_large[1], SHURK0_SELPH_DQ1_TXDLY_DQM1) | \
                                             P_Fld(ucdq_final_ui_large[2], SHURK0_SELPH_DQ1_TXDLY_DQM2) | \
                                             P_Fld(ucdq_final_ui_large[3], SHURK0_SELPH_DQ1_TXDLY_DQM3) | \
                                             P_Fld(ucdq_final_oen_ui_large[0], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) | \
                                             P_Fld(ucdq_final_oen_ui_large[1], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) | \
                                             P_Fld(ucdq_final_oen_ui_large[2], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) | \
                                             P_Fld(ucdq_final_oen_ui_large[3], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3));

             // DLY_DQM[2:0]
            vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3), \
                                             P_Fld(ucdq_final_ui_small[0], SHURK0_SELPH_DQ3_DLY_DQM0) | \
                                             P_Fld(ucdq_final_ui_small[1], SHURK0_SELPH_DQ3_DLY_DQM1) | \
                                             P_Fld(ucdq_final_ui_small[2], SHURK0_SELPH_DQ3_DLY_DQM2) | \
                                             P_Fld(ucdq_final_ui_small[3], SHURK0_SELPH_DQ3_DLY_DQM3) | \
                                             P_Fld(ucdq_final_oen_ui_small[0], SHURK0_SELPH_DQ3_DLY_OEN_DQM0) | \
                                             P_Fld(ucdq_final_oen_ui_small[1], SHURK0_SELPH_DQ3_DLY_OEN_DQM1) | \
                                             P_Fld(ucdq_final_oen_ui_small[2], SHURK0_SELPH_DQ3_DLY_OEN_DQM2) | \
                                             P_Fld(ucdq_final_oen_ui_small[3], SHURK0_SELPH_DQ3_DLY_OEN_DQM3));
    		}
    }
    vSetRank(p, backup_rank);

     //set to registers, PI DQ (per byte)
    /* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
    for(ii=p->rank; ii<RANK_MAX; ii++)
    {
        vSetRank(p,ii);

	//MT8167 TX DQ/DQM
	// set to best values for  DQM, DQ
	#if 0
	vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
						P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
						P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
						P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
						P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	#endif
	if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
	{

		//MT8167 TX DQ/DQM
		// set to best values for  DQM, DQ
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	}
	if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
	{
		//MT8167 TX DQ/DQM
		// set to best values for  DQM, DQ
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0));
		vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1));

	}

    }
    vSetRank(p, backup_rank);

     #if 0// for LP3 , TX tracking will be disable, don't need to set DQ delay in DramC.
     ///TODO: check LP3 byte mapping of dramC
     vIO32WriteFldMulti(DRAMC_REG_SHU1RK0_PI+(CHANNEL_A<< POS_BANK_NUM), \
                                     P_Fld(ucdq_final_pi[0], SHU1RK0_PI_RK0_ARPI_DQ_B0) | P_Fld(ucdq_final_pi[1], SHU1RK0_PI_RK0_ARPI_DQ_B1));

     vIO32WriteFldMulti(DRAMC_REG_SHU1RK0_PI+(CHANNEL_B<< POS_BANK_NUM), \
                                     P_Fld(ucdq_final_pi[2], SHU1RK0_PI_RK0_ARPI_DQ_B0) | P_Fld(ucdq_final_pi[3], SHU1RK0_PI_RK0_ARPI_DQ_B1));
     #endif

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =0;
#endif

    mcSHOW_DBG_MSG(("[DramcTxWindowPerbitCal] ====Done====\n"));
    mcFPRINTF((fp_A60501, "[DramcTxWindowPerbitCal] ====Done====\n"));

    #if 0
    vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DRAMC_REG_PADCTL4), 1, PADCTL4_CKEFIXON);  // test only
    #endif

    return DRAM_OK;

    // log example
    /*
TX calibration finding left boundary early break. PI DQ delay=0x3e
TX calibration finding right boundary early break. PI DQ delay=0x1c
==================================================================
    TX DQS perbit delay software calibration v3
    channel=2(2:cha, 3:chb)  apply = 1
==================================================================
PI DQ (per byte) window
x=pass dq delay value (min~max)center
y=0-7bit DQ of every group
input delay:Byte0 = 13 Byte1 = 13 Byte2 = 12 Byte3 = 13
==================================================================
bit    Byte0    bit    Byte1    bit    Byte2    bit    Byte3
 0   ( 1~26)13,  8   ( 3~26)14, 16   ( 2~27)14, 24   ( 3~26)14
 1   ( 2~26)14,  9   ( 1~24)12, 17   ( 0~25)12, 25   ( 4~26)15
 2   ( 3~25)14, 10   ( 2~26)14, 18   ( 2~25)13, 26   ( 3~27)15
 3   ( 2~24)13, 11   ( 1~23)12, 19   ( 3~25)14, 27   ( 1~23)12
 4   ( 3~26)14, 12   ( 2~26)14, 20   ( 0~24)12, 28   ( 1~25)13
 5   ( 3~25)14, 13   ( 2~25)13, 21   (-1~25)12, 29   ( 2~24)13
 6   ( 2~26)14, 14   ( 2~24)13, 22   (-1~26)12, 30   ( 3~27)15
 7   ( 1~25)13, 15   ( 2~26)14, 23   (-1~22)10, 31   ( 2~26)14

==================================================================
   */
}
#endif
void CATrainingLP3_ToggleOne(DRAMC_CTX_T *p) 
{
    U32 uiMR41;
    U32 uiReg024h;
    U32 uiReg038h;
    U32 uiReg04ch;
    U32 uiReg810h;
    U32 uiReg064h;
    S32 iFinalCACLK;
    S32 iCenterSum = 0;
    U8 ii, jj;
	U32 pattern_table[32];
    S32 iFirstCAPass[10], iLastCAPass[10], iCenter[10];    
    
    //  01010101b -> 10101010b : Golden value = 1001100110011001b=0x9999
    //  11111111b -> 00000000b : Golden value = 0101010101010101b=0x5555
    U32 u4GoldenPattern =0x55555555;
	
    //U32 u4GoldenPattern =0xaaaaaaaa;

    Another_Rank_CKE_low(p, 0);   

    mcSHOW_DBG_MSG(("\n[CATrainingLP3_ToggleOne]   Begin\n"));

    // Fix ODT off. A60501 disable ODT in the init code. So no need to do the following code.
    //uiReg54h=u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_WODT), WODT_WODTFIXOFF);
    //vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_WODT), 1, WODT_WODTFIXOFF);// According to Derping, should set to 1 to disable ODT.

    // Let MIO_CK always ON.
    uiReg038h=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL));
    uiReg04ch=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0));
    uiReg810h=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM4));
    uiReg064h=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL));  
    uiReg024h=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));  

    // Disable Refresh and save information
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), 1, REFCTRL0_REFDIS);      //REFDIS=1, disable auto refresh
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM3), 0, SHU_ACTIM3_REFCNT);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM4), 0, SHU_ACTIM4_REFCNT_FR_CLK);
    
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_MIOCKCTRLOFF);   //MIOCKCTRLOFF=1
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_PHYCLKDYNGEN); // set R_DMPHYCLKDYNGEN=0


    // Step 1.1 : let IO to O1 path valid
    O1PathOnOff(p, 1);

	for (ii=0; ii<32; ii++)
	{
		pattern_table[ii] = 1 << ii;
	}
	for (u1PatternIdx=0; u1PatternIdx<16; u1PatternIdx++) 
	{
		for (ii=0; ii<10; ii++) 
		{
			 iLastCAPass[ii] = PASS_RANGE_NA;
			 iFirstCAPass[ii] = PASS_RANGE_NA;
		}

		u4GoldenPattern = pattern_table[u1PatternIdx] | (pattern_table[u1PatternIdx] << 16);

		// ----- MR41, CA0~3, CA5~8 ------- 
		uiMR41 = 1;
		CATrainingEntry(p, uiMR41, u4GoldenPattern);  //MR41
		CATrainingDelayCompare(p, uiMR41, u4GoldenPattern, &iCenterSum, iFirstCAPass, iLastCAPass, iCenter, 0); 

		// ----- MR48, CA4 and 9 ------- 
		uiMR41 = 0;
		CATrainingEntry(p, uiMR41, u4GoldenPattern);  //MR48
		CATrainingDelayCompare(p, uiMR41, u4GoldenPattern, &iCenterSum, iFirstCAPass, iLastCAPass, iCenter, 0);	
		iFinalCACLK =iCenterSum/10;
		
		mcSHOW_DBG_MSG(("=========================================\n"));
#ifdef ETT_PRINT_FORMAT    
		mcSHOW_DBG_MSG(("u4GoldenPattern 0x%X, iFinalCACLKMacro0 = %d\n", u4GoldenPattern, iFinalCACLK));
#else    
		mcSHOW_DBG_MSG(("u4GoldenPattern 0x%x, iFinalCACLKMacro0 = %d\n", u4GoldenPattern, iFinalCACLK));
#endif

		 // Set CA output delay
		CATrain_ClkDelay[p->channel] = 0;
		// no need to enter self refresh before setting CLK under CA training mode
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_CA_CMD9), CATrain_ClkDelay[p->channel] , SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK);
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_CA_CMD9), CATrain_ClkDelay[p->channel]+2, SHU1_R1_CA_CMD9_RG_RK1_ARPI_CS); //clock_PI_Delay			  
		vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_CA_CMD9), 0xf, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);	 

		CATrainingExit(p);
	}
	// Disable fix DQ input enable.  Disable IO to O1 path
	O1PathOnOff(p, 0);

	// Disable CKE high, back to dynamic
	vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), P_Fld(0, CKECTRL_CKEFIXOFF)|P_Fld(0, CKECTRL_CKEFIXON));

	// Restore the registers' values.
	//vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_WODT), uiReg54h, WODT_WODTFIXOFF);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), uiReg038h);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), uiReg04ch);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM4), uiReg810h);
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL), uiReg064h);	 
	vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), uiReg024h);  
				 
	Another_Rank_CKE_low(p, 1);

	for (ii=0; ii<2; ii++) {
		for (jj=0; jj<8; jj++)
			mcSHOW_DBG_MSG(("0x%x ", pattern[ii][jj]));
	}

	mcSHOW_DBG_MSG(("\n"));

#if 0
	for (ii=0; ii<8; ii++) {
		for (jj=0; jj<32; jj++)
			if (pattern[0][ii] == pattern_table[jj]) {
				mcSHOW_DBG_MSG(("%d ", jj));
				break;
			}
	}
	mcSHOW_DBG_MSG(("\n"));
#endif
}
static DRAM_STATUS_T CATrainingLP3_HQA(DRAMC_CTX_T *p)
{
    U32 uiMR41;
    U32 uiReg024h;
    U32 uiReg038h;
    U32 uiReg04ch;
    U32 uiReg810h;
    U32 uiReg064h;
    S32 iFinalCACLK;
    S32 iCenterSum = 0;
    U8 backup_rank,ii;
    S32 iFirstCAPass[10], iLastCAPass[10], iCenter[10];    
    
    //  01010101b -> 10101010b : Golden value = 1001100110011001b=0x9999
    //  11111111b -> 00000000b : Golden value = 0101010101010101b=0x5555
    U32 u4GoldenPattern =0x55555555;
	U32 u4GoldenPattern2 =0x55555555;
	
    //U32 u4GoldenPattern =0xaaaaaaaa;

    Another_Rank_CKE_low(p, 0);   

    mcSHOW_DBG_MSG(("\n[CATrainingLP3]   Begin\n"));
    mcFPRINTF((fp_A60501, "\n[CATrainingLP3]   Begin\n"));

    // Fix ODT off. A60501 disable ODT in the init code. So no need to do the following code.
    //uiReg54h=u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_WODT), WODT_WODTFIXOFF);
    //vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_WODT), 1, WODT_WODTFIXOFF);// According to Derping, should set to 1 to disable ODT.

    // Let MIO_CK always ON.
    uiReg038h=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL));
    uiReg04ch=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0));
    uiReg810h=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM4));
    uiReg064h=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL));  
    uiReg024h=u4IO32Read4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));  

    // Disable Refresh and save information
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), 1, REFCTRL0_REFDIS);      //REFDIS=1, disable auto refresh
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM3), 0, SHU_ACTIM3_REFCNT);
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM4), 0, SHU_ACTIM4_REFCNT_FR_CLK);
    
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_MIOCKCTRLOFF);   //MIOCKCTRLOFF=1
    vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_PHYCLKDYNGEN); // set R_DMPHYCLKDYNGEN=0


    // Step 1.1 : let IO to O1 path valid
    O1PathOnOff(p, 1);

	for (iPatternType=0; iPatternType<2; iPatternType++)
		for (u1PatternIdx=0; u1PatternIdx<10; u1PatternIdx++)
		{
			mcSHOW_DBG_MSG(("iPatternType=%d, u1PatternIdx=%d\n", iPatternType, u1PatternIdx));
			if ((u1PatternIdx != 4) && (u1PatternIdx != 9))
			{
				u4GoldenPattern = pattern[iPatternType][u1PatternIdx] | (pattern[iPatternType][u1PatternIdx] << 16);
				if (iPatternType == 0)
					u4GoldenPattern2 = 0xaaaaaaaa;
				else
					u4GoldenPattern2 = 0x55555555;
			}
			else //CA4,CA9
			{
				u4GoldenPattern2 = pattern[iPatternType][u1PatternIdx] | (pattern[iPatternType][u1PatternIdx] << 16);
				if (iPatternType == 0)
					u4GoldenPattern = 0xaaaaaaaa;
				else
					u4GoldenPattern = 0x55555555;
			}
		    for (ii=0; ii<10; ii++) 
		    {
		         iLastCAPass[ii] = PASS_RANGE_NA;
		         iFirstCAPass[ii] = PASS_RANGE_NA;
		    }


		    // ----- MR41, CA0~3, CA5~8 ------- 
		    uiMR41 = 1;
		    CATrainingEntry(p, uiMR41, u4GoldenPattern);  //MR41
		    CATrainingDelayCompare(p, uiMR41, u4GoldenPattern, &iCenterSum, iFirstCAPass, iLastCAPass, iCenter, 0); 

		    // ----- MR48, CA4 and 9 ------- 
		    uiMR41 = 0;
		    CATrainingEntry(p, uiMR41, u4GoldenPattern2);  //MR48
		    CATrainingDelayCompare(p, uiMR41, u4GoldenPattern2, &iCenterSum, iFirstCAPass, iLastCAPass, iCenter, 0);    
		    
			iFinalCACLK =iCenterSum/10;
		    
		    mcSHOW_DBG_MSG(("=========================================\n"));
		    #ifdef ETT_PRINT_FORMAT    
		    mcSHOW_DBG_MSG(("u4GoldenPattern 0x%X, u4GoldenPattern2 0x%X, iFinalCACLKMacro0 = %d\n", u4GoldenPattern, u4GoldenPattern2, iFinalCACLK));
		    #else    
		    mcSHOW_DBG_MSG(("u4GoldenPattern 0x%x, u4GoldenPattern2 0x%X, iFinalCACLKMacro0 = %d\n", u4GoldenPattern, u4GoldenPattern2, iFinalCACLK));
		    #endif
		    
		    mcFPRINTF((fp_A60501, "=========================================\n"));
		    mcFPRINTF((fp_A60501, "iFinalCACLKMacro0 = %d\n", iFinalCACLK));


		    backup_rank = u1GetRank(p);

		    for(ii=p->rank; ii<RANK_MAX; ii++)
		    {
		        vSetRank(p,ii);		
		        mcSHOW_DBG_MSG(("rank%d:\n",p->rank));
		        if(iFinalCACLK <0) 
		        {    //SEt CLK delay
#if CA_TRAIN_RESULT_DO_NOT_MOVE_CLK
		              CATrain_ClkDelay[p->channel] =0;  /// should not happen.

		              vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_CA_CMD9), 0x10, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);
		              
		              vSetCalibrationResult(p, DRAM_CALIBRATION_CA_TRAIN, DRAM_FAIL);  
		              mcSHOW_ERR_MSG(("Error : CLK delay is not 0 (%d), Set as 0.  CA set as 0x10\n\n", -iFinalCACLK));
		              mcFPRINTF((fp_A60501, "Error : CLK delay is not 0 (%d), Set as 0.  CA set as 0x10\n\n", -iFinalCACLK));
		              while(1){};
#else
		              CATrain_ClkDelay[p->channel] = -iFinalCACLK;
		 
		                vIO32WriteFldAlign(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_CA_CMD9), 0, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);

		              mcSHOW_DBG_MSG(("Clk Dealy is = %d, CA delay is 0\n\n", -iFinalCACLK));
		              mcFPRINTF((fp_A60501, "Clk Dealy is = %d, CA delay is 0\n\n", -iFinalCACLK));
#endif  
		        }
		        else 
		        {    // Set CA output delay
		              CATrain_ClkDelay[p->channel] = 0;

		              vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_CA_CMD9), iFinalCACLK, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);
		              mcSHOW_DBG_MSG(("Macro%d Clk Dealy is 0, CA delay is %d\n\n", 0, iFinalCACLK));
		              mcFPRINTF((fp_A60501, "Macro%d Clk Dealy is 0, CA delay is %d\n\n", 0, iFinalCACLK));
		    }    
				
		    // no need to enter self refresh before setting CLK under CA training mode
			vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_CA_CMD9), CATrain_ClkDelay[p->channel] , SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK);
			vIO32WriteFldAlign_All(DRAMC_REG_ADDR(DDRPHY_SHU1_R0_CA_CMD9), CATrain_ClkDelay[p->channel]+2, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CS); //clock_PI_Delay 			  
		    }
		    vSetRank(p,backup_rank);

		    mcSHOW_DBG_MSG(("=========================================\n"));    
		    mcFPRINTF((fp_A60501, "=========================================\n"));   


#if SUPPORT_CATRAININT_DELAY_LINE_CALI
			//U8 u1ClkPiBak = u4IO32ReadFldAlign(DDRPHY_SHU1_R0_CA_CMD9, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK);
			//vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, u1ClkPiBak+8, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK); //clock_PI_Delay
		    CATrainingDelayLineCali(p);
			//vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, u1ClkPiBak, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK); //clock_PI_Delay
#endif
		    

		    CATrainingExit(p);
		}
    // Disable fix DQ input enable.  Disable IO to O1 path
    O1PathOnOff(p, 0);
    
    // Disable CKE high, back to dynamic
    vIO32WriteFldMulti(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), P_Fld(0, CKECTRL_CKEFIXOFF)|P_Fld(0, CKECTRL_CKEFIXON));

    // Restore the registers' values.
    //vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_WODT), uiReg54h, WODT_WODTFIXOFF);
    vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), uiReg038h);
    vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), uiReg04ch);
    vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM4), uiReg810h);
    vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL), uiReg064h);    
    vIO32Write4B(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), uiReg024h);  
                 
    Another_Rank_CKE_low(p, 1);
    iPatternType = -1;      
    return DRAM_OK;
}

static void SetDqsDqDrv(DRAMC_CTX_T *p, U8 *drv)
{
	U8 u1Offest_code_b01_p = 0;
	U8 u1Offest_code_b01_n = 0;
	U8 u1Offest_code_b23_p = 0;
	U8 u1Offest_code_b23_n = 0;
	U8 u1Offest_code_cmd_p = 0;
	U8 u1Offest_code_cmd_n = 0;
	
     //DQ
     vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ1, drv[0]+u1Offest_code_b01_p, SHU1_B0_DQ1_RG_TX_ARDQ_DRVP_B_B0);
     vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ1, drv[1]+u1Offest_code_b01_n, SHU1_B0_DQ1_RG_TX_ARDQ_DRVN_B_B0);	 
     vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ1+(1<<POS_BANK_NUM), drv[0]+u1Offest_code_b23_p, SHU1_B0_DQ1_RG_TX_ARDQ_DRVP_B_B0);
     vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ1+(1<<POS_BANK_NUM), drv[1]+u1Offest_code_b23_n, SHU1_B0_DQ1_RG_TX_ARDQ_DRVN_B_B0);
     vIO32WriteFldAlign(DRAMC_REG_SHU1_DRVING2, (drv[0]<<5) | drv[1], SHU1_DRVING2_DQDRV1);         
     
     //DQS
     vIO32WriteFldAlign(DRAMC_REG_SHU1_DRVING1, (drv[0]<<5) | drv[1], SHU1_DRVING1_DQSDRV1); 

	 mcSHOW_DBG_MSG(("SetDqsDqDrv:%d %d, DQPN:0x%x, DQ01PNB:0x%x DQ23PNB:0x%x, DQS:0x%x\n", drv[0], drv[1], \
	 		u4IO32Read4B(DRAMC_REG_SHU1_DRVING2), u4IO32Read4B(DDRPHY_SHU1_B0_DQ1), u4IO32Read4B(DDRPHY_SHU1_B0_DQ1+(1<<POS_BANK_NUM)), u4IO32Read4B(DRAMC_REG_SHU1_DRVING1)));
                               
}

static void SetCmdClkDrv(DRAMC_CTX_T *p, U8 *drv)
{
	U8 u1Offest_code_b01_p = 0;
	U8 u1Offest_code_b01_n = 0;
	U8 u1Offest_code_b23_p = 0;
	U8 u1Offest_code_b23_n = 0;
	U8 u1Offest_code_cmd_p = 0;
	U8 u1Offest_code_cmd_n = 0;
 
     //CMD 
     vIO32WriteFldAlign(DDRPHY_SHU1_CA_CMD1, drv[0]+u1Offest_code_cmd_p, SHU1_CA_CMD1_RG_TX_ARCMD_DRVP_B);
     vIO32WriteFldAlign(DDRPHY_SHU1_CA_CMD1, drv[1]+u1Offest_code_cmd_n, SHU1_CA_CMD1_RG_TX_ARCMD_DRVN_B);
    
     vIO32WriteFldAlign_All(DRAMC_REG_SHU1_DRVING2, (drv[0]<<5) | drv[1], SHU1_DRVING2_CMDDRV1);                           
    
     //CLK
     vIO32WriteFldAlign_All(DRAMC_REG_SHU1_DRVING2, (drv[0]<<5) | drv[1], SHU1_DRVING2_CMDDRV2); 

     mcSHOW_DBG_MSG(("SetCmdClkDrv:%d %d, CLKPN:0x%x, CMDPN:0x%x CMDPNB:0x%x\n", drv[0], drv[1], \
		u4IO32Read4B(DRAMC_REG_SHU1_DRVING2), u4IO32Read4B(DRAMC_REG_SHU1_DRVING2), u4IO32Read4B(DDRPHY_SHU1_CA_CMD1)));
    
}

static void SetDqDrvB(DRAMC_CTX_T *p, U8 *drv)
{
	U8 u1Offest_code_b01_p = 0;
	U8 u1Offest_code_b01_n = 0;
	U8 u1Offest_code_b23_p = 0;
	U8 u1Offest_code_b23_n = 0;
	U8 u1Offest_code_cmd_p = 0;
	U8 u1Offest_code_cmd_n = 0;
	
     //DQ
     vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ1, drv[0]+u1Offest_code_b01_p, SHU1_B0_DQ1_RG_TX_ARDQ_DRVP_B_B0);
     vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ1, drv[1]+u1Offest_code_b01_n, SHU1_B0_DQ1_RG_TX_ARDQ_DRVN_B_B0);	 
     vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ1+(1<<POS_BANK_NUM), drv[0]+u1Offest_code_b23_p, SHU1_B0_DQ1_RG_TX_ARDQ_DRVP_B_B0);
     vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ1+(1<<POS_BANK_NUM), drv[1]+u1Offest_code_b23_n, SHU1_B0_DQ1_RG_TX_ARDQ_DRVN_B_B0);         
     
	 mcSHOW_DBG_MSG(("SetDqDrvB:%d %d, DQ01PNB:0x%x DQ23PNB:0x%x\n", drv[0], drv[1], u4IO32Read4B(DDRPHY_SHU1_B0_DQ1), u4IO32Read4B(DDRPHY_SHU1_B0_DQ1+(1<<POS_BANK_NUM))));                             
}

static void SetCmdDrvB(DRAMC_CTX_T *p, U8 *drv)
{
	U8 u1Offest_code_b01_p = 0;
	U8 u1Offest_code_b01_n = 0;
	U8 u1Offest_code_b23_p = 0;
	U8 u1Offest_code_b23_n = 0;
	U8 u1Offest_code_cmd_p = 0;
	U8 u1Offest_code_cmd_n = 0;
 
     //CMD 
     vIO32WriteFldAlign(DDRPHY_SHU1_CA_CMD1, drv[0]+u1Offest_code_cmd_p, SHU1_CA_CMD1_RG_TX_ARCMD_DRVP_B);
     vIO32WriteFldAlign(DDRPHY_SHU1_CA_CMD1, drv[1]+u1Offest_code_cmd_n, SHU1_CA_CMD1_RG_TX_ARCMD_DRVN_B);
	 
     mcSHOW_DBG_MSG(("SetCmdDrvB:%d %d, CMDPNB:0x%x\n", drv[0], drv[1], u4IO32Read4B(DDRPHY_SHU1_CA_CMD1)));    
}

static void SetDqVref(DRAMC_CTX_T *p, U8 vref)
{	
	vIO32WriteFldAlign(DDRPHY_MISC_VREF_CTRL, vref, MISC_VREF_CTRL_RG_RVREF_SEL_DQ);	
	mcSHOW_DBG_MSG(("SetDqVref:%d 0x%x\n", vref, u4IO32Read4B(DDRPHY_MISC_VREF_CTRL)));
}

static void SetDqRxVref(DRAMC_CTX_T *p, U8 vref)
{
    vIO32WriteFldAlign_All(DDRPHY_SHU1_B0_DQ5, vref, SHU1_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU1_B1_DQ5, vref, SHU1_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1);
    mcSHOW_DBG_MSG(("SetDqVref:%d 0x%x\n", vref, u4IO32Read4B(DDRPHY_SHU1_B0_DQ5)));
}

static void SetCmdVref(DRAMC_CTX_T *p, U8 vref)
{
	vIO32WriteFldAlign(DDRPHY_MISC_VREF_CTRL, vref, MISC_VREF_CTRL_RG_RVREF_SEL_CMD);	
	mcSHOW_DBG_MSG(("SetCmdVref:%d 0x%x\n", vref, u4IO32Read4B(DDRPHY_MISC_VREF_CTRL)));	
}
static void SetDrvSel(DRAMC_CTX_T *p, U32 drv_sel)
{
	vIO32WriteFldAlign(DDRPHY_CA_CMD9, drv_sel, CA_CMD9_RG_TX_ARCMD_DRV_SELECT);
	mcSHOW_DBG_MSG(("SetDrvSel:0x%x 0x%x\n", drv_sel, u4IO32Read4B(DDRPHY_CA_CMD9)));	
}

void DqEyeScanDrv(DRAMC_CTX_T *p)
{
	U8 BitIndex, i = 0xe;
	U8 drv[2];
	mcSHOW_DBG_MSG(("\nDqEyeScanDrv print begin\n"));
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1, DRAMC_PD_CTRL_MIOCKCTRLOFF);   //MIOCKCTRLOFF=1
	vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0, DRAMC_PD_CTRL_PHYCLKDYNGEN); // set R_DMPHYCLKDYNGEN=0

	for (i=3; i<=21; i++) 
	{
		if (i!=3 && i!=14 && i!=21)
			continue;
		SetDqVref(p, i);
		mcDELAY_US(5000000);
		memset(ScanRet, 0, sizeof(ScanRet));
		for (ScanIndex = 0; ScanIndex < 16; ScanIndex++) 
		{	
		 	drv[0] = ScanIndex;
			drv[1] = ScanIndex;
			SetDqsDqDrv(p, drv);
			mcDELAY_US(500000);
			DramcTxWindowPerbitCal(p, TX_DQ_DQS_MOVE_DQ_DQM);
		}

		mcSHOW_DBG_MSG(("vref:%d, drv:0~15\n", i));

		for (BitIndex = 0; BitIndex < 32; BitIndex++) {
			mcDELAY_US(1000);
			mcSHOW_DBG_MSG(("\nBitIndex:%d\n", BitIndex));
			for (ScanIndex = 15; ScanIndex >= 0; ScanIndex--) {
				mcSHOW_DBG_MSG(("%d	", ScanIndex));
				for (ScanPI = 0; ScanPI < RET_LEN; ScanPI++) {
					mcDELAY_US(100);
					if ((ScanRet[ScanIndex][ScanPI] >> BitIndex) & 1) {
						mcSHOW_DBG_MSG(("o	"));
					}
					else {
						mcSHOW_DBG_MSG(("x	"));
					}
				}
				mcSHOW_DBG_MSG(("\n"));
			}

			mcSHOW_DBG_MSG((" 	"));	
			for (ScanPI = 0; ScanPI < RET_LEN; ScanPI++) {
				mcSHOW_DBG_MSG(("%d	", ScanPI));	
			}
			mcSHOW_DBG_MSG(("\n"));
			
			mcSHOW_DBG_MSG((" 	"));	
			for (ScanPI = ScanTxBegin; ScanPI < ScanTxBegin+RET_LEN; ScanPI++) {
				mcSHOW_DBG_MSG(("%d	", ScanPI));	
			}
			mcSHOW_DBG_MSG(("\n"));
		}	
	}
	mcSHOW_DBG_MSG(("DqEyeScanDrv print done\n"));
	
}

void DqRxEyeScanVref(DRAMC_CTX_T *p)
{
	U8 BitIndex, i, IdxBegin, IdxEnd;
	mcSHOW_DBG_MSG(("\nDqEyeScanVref print begin\n"));
	//vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0x1, REFCTRL0_REFDIS);

	for (i=0; i<=1; i++)
	//for (i=0; i<=0; i++)
	{
		vIO32WriteFldMulti_All(DDRPHY_B0_DQ6, P_Fld(i^1, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0)
	            | P_Fld(i, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0));
		vIO32WriteFldMulti_All(DDRPHY_B1_DQ6, P_Fld(i^1, B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1)
	            | P_Fld(i, B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1));

		memset(ScanRet, 0, sizeof(ScanRet));
		if (i == 0) {
			IdxBegin = 31;
			IdxEnd = 0;
		}
		else {
			IdxBegin = 31;
			IdxEnd = 2;
		}
		for (ScanIndex = IdxBegin; ScanIndex >= IdxEnd; ScanIndex--)
		//for (ScanIndex = 0; ScanIndex < 32; ScanIndex++)
		{
			SetDqRxVref(p, ScanIndex);
			mcDELAY_US(3000000);
			DramPhyReset(p);
			DramcRxWindowPerbitCal(p, 1);
		}

		mcSHOW_DBG_MSG(("Vref:0~31 DDR3_SEL=%d, DDR4_SEL=%d begin\n", \
			u4IO32ReadFldAlign(DDRPHY_B0_DQ6, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0), \
			u4IO32ReadFldAlign(DDRPHY_B0_DQ6, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0)));

		for (BitIndex = 0; BitIndex < 32; BitIndex++) {
			mcDELAY_US(1000);
			mcSHOW_DBG_MSG(("\nBitIndex:%d\n", BitIndex));
			for (ScanIndex = 31; ScanIndex >= 0; ScanIndex--) {
				if (ScanIndex == 0) {
					mcSHOW_DBG_MSG(("%d:DDR3_SEL=%d,DDR4_SEL=%d	", ScanIndex, u4IO32ReadFldAlign(DDRPHY_B0_DQ6, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0), \
													u4IO32ReadFldAlign(DDRPHY_B0_DQ6, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0)));
				}
				else {
					mcSHOW_DBG_MSG(("%d	", ScanIndex));
				}
				for (ScanPI = 0; ScanPI < RET_LEN; ScanPI++) {
					mcDELAY_US(50);
					if ((ScanRet[ScanIndex][ScanPI] >> BitIndex) & 1) {
						mcSHOW_DBG_MSG(("o	"));
					}
					else {
						mcSHOW_DBG_MSG(("x	"));
					}
				}
				mcSHOW_DBG_MSG(("\n"));
			}

			mcSHOW_DBG_MSG((" 	"));
			for (ScanPI = 0; ScanPI < RET_LEN; ScanPI++) {
				mcSHOW_DBG_MSG(("%d	", ScanPI));	
			}
			mcSHOW_DBG_MSG(("\n"));
			mcSHOW_DBG_MSG((" 	"));
			for (ScanPI = ScanTxBegin; ScanPI < ScanTxBegin+RET_LEN; ScanPI++) {
				mcSHOW_DBG_MSG(("%d	", ScanPI));
			}
			mcSHOW_DBG_MSG(("\n"));

		}

		mcSHOW_DBG_MSG(("Vref:0~31 DDR3_SEL=%d, DDR4_SEL=%d done\n", \
			u4IO32ReadFldAlign(DDRPHY_B0_DQ6, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0), \
			u4IO32ReadFldAlign(DDRPHY_B0_DQ6, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0)));
	}

#if 0
	mcSHOW_DBG_MSG(("Vref:16~31\n"));
	memset(ScanRet, 0, sizeof(ScanRet));

	for (ScanIndex = 0; ScanIndex < 16; ScanIndex++) 
	{
		SetDqVref(p, ScanIndex+16);
		DramcTxWindowPerbitCal(p, TX_DQ_DQS_MOVE_DQ_DQM);
	}

	for (BitIndex = 0; BitIndex < 32; BitIndex++) {
		mcDELAY_US(20);
		mcSHOW_DBG_MSG(("\nBitIndex:%d\n", BitIndex));
		for (ScanIndex = 15; ScanIndex >= 0; ScanIndex--) {
			mcSHOW_DBG_MSG(("%d	", ScanIndex+16));
			for (ScanPI = 0; ScanPI <= 96; ScanPI++) {
				if ((ScanRet[ScanIndex][ScanPI] >> BitIndex) & 1)
					mcSHOW_DBG_MSG(("o	"));
				else
					mcSHOW_DBG_MSG(("x	"));
			}
			mcSHOW_DBG_MSG(("\n"));
		}

		mcSHOW_DBG_MSG((" 	"));
		for (ScanPI = 0; ScanPI <= 96; ScanPI++) {
			mcSHOW_DBG_MSG(("%d	", ScanPI));
		}
		mcSHOW_DBG_MSG(("\n"));
		mcSHOW_DBG_MSG((" 	"));
		for (ScanPI = ScanTxBegin; ScanPI <= ScanTxBegin+96; ScanPI++) {
			mcSHOW_DBG_MSG(("%d	", ScanPI));
		}
		mcSHOW_DBG_MSG(("\n"));
	}
#endif
	mcSHOW_DBG_MSG(("DqEyeScanVref print done\n"));

}

void DqEyeScanVref(DRAMC_CTX_T *p)
{
	U8 BitIndex, i, IdxBegin, IdxEnd;
	mcSHOW_DBG_MSG(("\nDqEyeScanVref print begin\n"));
	//vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0x1, REFCTRL0_REFDIS);

	for (i=0; i<=1; i++)
	//for (i=0; i<=0; i++)
	{			
		vIO32WriteFldMulti(DDRPHY_MISC_VREF_CTRL, P_Fld(i^1, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL)
	            | P_Fld(i, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL));
		memset(ScanRet, 0, sizeof(ScanRet));
		if (i == 0) {
			IdxBegin = 28;
			IdxEnd = 0;
		}
		else {
			IdxBegin = 31;
			IdxEnd = 17;
		}
		for (ScanIndex = IdxBegin; ScanIndex >= IdxEnd; ScanIndex--) 
		//for (ScanIndex = 0; ScanIndex < 32; ScanIndex++) 
		{
			SetDqVref(p, ScanIndex);
			mcDELAY_US(2000000);
			DramPhyReset(p);
			DramcTxWindowPerbitCal(p, TX_DQ_DQS_MOVE_DQ_DQM);
		}

		mcSHOW_DBG_MSG(("Vref:0~31 DDR3_SEL=%d, DDR4_SEL=%d begin\n", \
			u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL), \
			u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL)));

		for (BitIndex = 0; BitIndex < 32; BitIndex++) {
			mcDELAY_US(1000);
			mcSHOW_DBG_MSG(("\nBitIndex:%d\n", BitIndex));
			for (ScanIndex = 31; ScanIndex >= 0; ScanIndex--) {
				if (ScanIndex == 0) {
					mcSHOW_DBG_MSG(("%d:DDR3_SEL=%d,DDR4_SEL=%d	", ScanIndex, u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL), \
													u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL)));
				}
				else {
					mcSHOW_DBG_MSG(("%d	", ScanIndex));
				}
				for (ScanPI = 0; ScanPI < RET_LEN; ScanPI++) {
					mcDELAY_US(20);
					if ((ScanRet[ScanIndex][ScanPI] >> BitIndex) & 1) {
						mcSHOW_DBG_MSG(("o	"));
					}
					else {
						mcSHOW_DBG_MSG(("x	"));
					}
				}
				mcSHOW_DBG_MSG(("\n"));	
			}

			mcSHOW_DBG_MSG((" 	"));
			for (ScanPI = 0; ScanPI < RET_LEN; ScanPI++) {
				mcSHOW_DBG_MSG(("%d	", ScanPI));
			}
			mcSHOW_DBG_MSG(("\n")); 
			mcSHOW_DBG_MSG((" 	"));	
			for (ScanPI = ScanTxBegin; ScanPI < ScanTxBegin+RET_LEN; ScanPI++) {
				mcSHOW_DBG_MSG(("%d	", ScanPI));	
			}
			mcSHOW_DBG_MSG(("\n"));	

		}

		mcSHOW_DBG_MSG(("Vref:0~31 DDR3_SEL=%d, DDR4_SEL=%d done\n", \
			u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL), \
			u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL)));
	}

#if 0
	mcSHOW_DBG_MSG(("Vref:16~31\n"));
	memset(ScanRet, 0, sizeof(ScanRet));

	for (ScanIndex = 0; ScanIndex < 16; ScanIndex++) 
	{	
		SetDqVref(p, ScanIndex+16);
		DramcTxWindowPerbitCal(p, TX_DQ_DQS_MOVE_DQ_DQM);
	}

	for (BitIndex = 0; BitIndex < 32; BitIndex++) {
		mcDELAY_US(20);
		mcSHOW_DBG_MSG(("\nBitIndex:%d\n", BitIndex));
		for (ScanIndex = 15; ScanIndex >= 0; ScanIndex--) {
			mcSHOW_DBG_MSG(("%d	", ScanIndex+16));
			for (ScanPI = 0; ScanPI <= 96; ScanPI++) {
				if ((ScanRet[ScanIndex][ScanPI] >> BitIndex) & 1)
					mcSHOW_DBG_MSG(("o	"));
				else
					mcSHOW_DBG_MSG(("x	"));	
			}
			mcSHOW_DBG_MSG(("\n"));	
		}

		mcSHOW_DBG_MSG((" 	"));
		for (ScanPI = 0; ScanPI <= 96; ScanPI++) {
			mcSHOW_DBG_MSG(("%d	", ScanPI));	
		}
		mcSHOW_DBG_MSG(("\n"));
		mcSHOW_DBG_MSG((" 	"));	
		for (ScanPI = ScanTxBegin; ScanPI <= ScanTxBegin+96; ScanPI++) {
			mcSHOW_DBG_MSG(("%d	", ScanPI));	
		}
		mcSHOW_DBG_MSG(("\n"));	
	}
#endif
	mcSHOW_DBG_MSG(("DqEyeScanVref print done\n"));
	
}

void CaEyeScanDrv(DRAMC_CTX_T *p)
{
	U8 CaIndex, i;
	U8 drv[2];
	mcSHOW_DBG_MSG(("\nCaEyeScanDrv print begin\n"));

	for (i=3; i<=21; i++) 
	{
		SetDqVref(p, i);
		mcDELAY_US(1000000);

		memset(ScanRet, 0, sizeof(ScanRet));
		for (ScanIndex = 0; ScanIndex < 16; ScanIndex++) 
		{	
		 	drv[0] = ScanIndex;
			drv[1] = ScanIndex;
			SetCmdClkDrv(p, drv);
			CATrainingLP3(p);
			CATrainingLP3_HQA(p);
		}
		
		mcSHOW_DBG_MSG(("vref:%d, drv:0~15\n", i));

		for (CaIndex = 0; CaIndex < 10; CaIndex++) {
			mcDELAY_US(50);
			mcSHOW_DBG_MSG(("\nCaIndex:%d\n", CaIndex));
			for (ScanIndex = 15; ScanIndex >= 0; ScanIndex--) {
				mcSHOW_DBG_MSG(("%d	", ScanIndex));
				for (ScanPI = 0; ScanPI < RET_LEN; ScanPI++) {
					if ((ScanRet[ScanIndex][ScanPI] >> CaIndex) & 1) {
						mcSHOW_DBG_MSG(("o	"));
					}
					else {
						mcSHOW_DBG_MSG(("x	"));
					}
				}
				mcSHOW_DBG_MSG(("\n"));	
			}
			
			mcSHOW_DBG_MSG((" 	"));
			for (ScanPI = -15; ScanPI < -15+RET_LEN; ScanPI++) {
				mcSHOW_DBG_MSG(("%d	", ScanPI));	
			}
			mcSHOW_DBG_MSG(("\n")); 		
		}	
	}
	mcSHOW_DBG_MSG(("\nCaEyeScanDrv print done\n"));

}

void CaEyeScanVref(DRAMC_CTX_T *p)
{
	U8 CaIndex, i = 0, j;
	U32 drv_sel;
	U8 drv[2] = {9, 9};
	mcSHOW_DBG_MSG(("\nCaEyeScanVref print begin\n"));

	for (j=0; j<=1; j++)
	//for (j=1; j<=1; j++)
	{
		if (j == 0) 
		{
			drv_sel = 0;
		}
		else 
		{
			drv_sel = (1<<1) | (1<<2) | (1<<5) | (1<<6) | (1<<7) | (1<<9) |\
					  (1<<10) | (1<<13) | (1<<16) | (1<<19) | (1<<22);			
			drv[0] = 0;
			drv[1] = 0;
			SetCmdDrvB(p, drv);
		}
		
		SetDrvSel(p, drv_sel);

VREF_K:
		for (i=0; i<=1; i++)
		{			
			vIO32WriteFldMulti(DDRPHY_MISC_VREF_CTRL, P_Fld(i^1, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL)
		            | P_Fld(i, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL));
			memset(ScanRet, 0, sizeof(ScanRet));
			for (ScanIndex = 31; ScanIndex >= 0; ScanIndex--)
			//for (ScanIndex = 0; ScanIndex < 32; ScanIndex++) 
			{	
				SetDqVref(p, ScanIndex);
				mcDELAY_US(1000000);
				CATrainingLP3(p);
				CATrainingLP3_HQA(p);
			}

			mcSHOW_DBG_MSG(("drv_sel:0x%x, DRVB:0x%x, Vref:0~31 DDR3_SEL=%d, DDR4_SEL=%d begin\n", \
				drv_sel, drv[0], \
				u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL), \
				u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL)));

			for (CaIndex = 0; CaIndex < 10; CaIndex++) {
				mcDELAY_US(500);
				mcSHOW_DBG_MSG(("\nCaIndex:%d\n", CaIndex));
				for (ScanIndex = 31; ScanIndex >= 0; ScanIndex--) {
					if (ScanIndex == 0) {
						mcSHOW_DBG_MSG(("%d:DDR3_SEL=%d,DDR4_SEL=%d	", ScanIndex, u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL), \
														u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL)));
					}
					else {
						mcSHOW_DBG_MSG(("%d	", ScanIndex));
					}

					for (ScanPI = 0; ScanPI < RET_LEN; ScanPI++) {
						if ((ScanRet[ScanIndex][ScanPI] >> CaIndex) & 1) {
							mcSHOW_DBG_MSG(("o	"));
						}
						else {
							mcSHOW_DBG_MSG(("x	"));
						}
					}
					mcSHOW_DBG_MSG(("\n")); 
				}

				mcSHOW_DBG_MSG((" 	"));
				for (ScanPI = -15; ScanPI < -15+RET_LEN; ScanPI++) {
					mcSHOW_DBG_MSG(("%d	", ScanPI));	
				}
				mcSHOW_DBG_MSG(("\n")); 
			}
			mcSHOW_DBG_MSG(("drv_sel:0x%x, DRVB:0x%x, Vref:0~31 DDR3_SEL=%d, DDR4_SEL=%d done\n", \
				drv_sel, drv[0], \
				u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL), \
				u4IO32ReadFldAlign(DDRPHY_MISC_VREF_CTRL, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL)));
		}

		if ((drv[0] == 0) && (j == 1)) {
			drv[0] = 0xf;
			drv[1] = 0xf;
			SetCmdDrvB(p, drv);
			goto VREF_K;
		}
	}
#if 0
	mcSHOW_DBG_MSG(("Vref:16~31\n"));
	memset(ScanRet, 0, sizeof(ScanRet));

	for (ScanIndex = 0; ScanIndex < 16; ScanIndex++) 
	{	
		SetCmdVref(p, ScanIndex+16);
		CATrainingLP3(p);
	}

	for (CaIndex = 0; CaIndex < 10; CaIndex++) {
		mcSHOW_DBG_MSG(("\nCaIndex:%d\n", CaIndex));
		for (ScanIndex = 15; ScanIndex >= 0; ScanIndex--) {
			mcSHOW_DBG_MSG(("%d	", ScanIndex+16));
			for (ScanPI = 0; ScanPI <= 78; ScanPI++) {
				if ((ScanRet[ScanIndex][ScanPI] >> CaIndex) & 1)
					mcSHOW_DBG_MSG(("o	"));
				else
					mcSHOW_DBG_MSG(("x	")); 
			}
			mcSHOW_DBG_MSG(("\n")); 
		}

		for (ScanPI = -15; ScanPI <= 63; ScanPI++) {
			mcSHOW_DBG_MSG(("%d	", ScanPI));	
		}
		mcSHOW_DBG_MSG(("\n")); 
	}
#endif
	mcSHOW_DBG_MSG(("CaEyeScanVref print done\n"));
	
}
#endif

#ifdef MT8167_DVT_TEST
static void WriteData(U32 uiFixedAddr, U32 uiLen, U32 pattern)
{
    U32 count;

    for (count= 0; count<uiLen; count+=4)
    {
        *(volatile unsigned int *)(count +uiFixedAddr) = count + ( pattern << 16);
    }
}
extern u32 g_ddr_reserve_enable;
extern u32 g_ddr_reserve_success;

static void ReadData(U32 uiFixedAddr, U32 uiLen, U32 pattern)
{
    U32 count, pass_count=0, err_count=0;
    for (count=0; count<uiLen; count+=4)
    {
        if (*(volatile unsigned int *)(count +uiFixedAddr) != count + (pattern << 16))
        {      
         if((g_ddr_reserve_enable==1) && (g_ddr_reserve_success==1))
            mcSHOW_DBG_MSG(("[Fail] Addr[0x%x]0x%x != 0x%x\n",count +uiFixedAddr, *(volatile unsigned int	 *)(count +uiFixedAddr), count + (0x5a5a <<16)));
            err_count++;
        }
        else
            pass_count ++;
    }     
    
    if(err_count)
    {       
        mcSHOW_DBG_MSG(("[MEM_TEST] read data Fail. \n"));
    }
    else
    {
        mcSHOW_DBG_MSG(("[MEM_TEST] read data OK. \n"));
    }
    mcSHOW_DBG_MSG(("(uiFixedAddr 0x%X, Pass count =%d, Fail count =%d)\n", uiFixedAddr, pass_count, err_count));

}
void SrefEntryTest(DRAMC_CTX_T *p)
{
	U8 u1RankIdx;
	U32 dram_rank_base[2];

	mcSHOW_DBG_MSG(("\n[SrefEntryTest] begin\n"));
	get_dram_rank_size(dram_rank_base);
	dram_rank_base[1] = DDR_BASE + dram_rank_base[0];
	dram_rank_base[0] = DDR_BASE;
#if 0
	//enable auto refresh
	vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0, REFCTRL0_REFDIS);

	//write data
	mcSHOW_DBG_MSG(("write data\n"));
	WriteData(DDR_BASE, 0x100000, 0x5a5a);
	
	//disable auto refresh
	vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 1, REFCTRL0_REFDIS);

	//delay 120s
	mcSHOW_DBG_MSG(("delay 120s ......\n"));
	mdelay(120000);

	//enable auto refresh
	vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0, REFCTRL0_REFDIS);	
	
	//read data and compare it 
	ReadData(DDR_BASE, 0x100000, 0x5a5a);
#endif	
	// write data
	mcSHOW_DBG_MSG(("write data\n"));
	for(u1RankIdx =0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
	{
		WriteData(dram_rank_base[u1RankIdx], 0x10000000, 0x5a5a);
	}
	
	while (1)
	{
		//enter sref and check sref state
		//DramcEnterSelfRefresh(p, 1);
		//0x39c r7[1] EMI_CLK_OFF_REG
		//0x3b4 r13[21] EMI_CLK_OFF_ACK
		*((volatile int *)0x10006014) |= (1 << 1);   
		while (((*((volatile int *)0x100063b4) >> 21) & 1) == 0)
		{
		}
		mcSHOW_DBG_MSG(("Self refresh state(%d)\n", u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MISC_STATUSA), MISC_STATUSA_SREF_STATE)));

		//disale auto refresh
		vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 1, REFCTRL0_REFDIS);

		//delay 120s
		mcSHOW_DBG_MSG(("delay 120s ......\n"));
		mdelay(120000);

		//enable auto refresh
		vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0, REFCTRL0_REFDIS);

		//exit sref 
		//DramcEnterSelfRefresh(p,0);
		*((volatile int *)0x10006014) &= ~(1<<1); 
		while (((*((volatile int *)0x100063b4) >> 21) & 1) == 1)
		{
		}
		mcSHOW_DBG_MSG(("Self refresh state(%d)\n", u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MISC_STATUSA), MISC_STATUSA_SREF_STATE)));

		//read data and compare it
		for(u1RankIdx =0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
		{
			ReadData(dram_rank_base[u1RankIdx], 0x10000000, 0x5a5a);
		}

	}	
	mcSHOW_DBG_MSG(("[SrefEntryTest] done\n"));
}

void PasrTest(DRAMC_CTX_T *p)
{
    U8 u1RankIdx, i;
    U32 dram_rank_base[2];

    mcSHOW_DBG_MSG(("\n[PasrTest] begin\n"));
    get_dram_rank_size(dram_rank_base);
	dram_rank_base[1] = DDR_BASE + dram_rank_base[0];
	dram_rank_base[0] = DDR_BASE;

	for (i = 0; i < 2; i++)
	{		
    	for(u1RankIdx =0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
		{
	        //enable auto refresh
	        vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0, REFCTRL0_REFDIS);
		    	
	        //write data
	        WriteData(dram_rank_base[u1RankIdx], 0x100000, 0x5a5a);
			WriteData(dram_rank_base[u1RankIdx] + 0x20000000, 0x100000, 0x5a5a);
			
	        mcSHOW_DBG_MSG(("\nDramcModeRegInit_LP3 for Rank%d\n", u1RankIdx));            
	        vIO32WriteFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MRS), u1RankIdx, MRS_MRSRK);
			if (i == 0)
	        	DramcModeRegWrite(p, 17, 0xf0); 
			else
				DramcModeRegWrite(p, 17, 0x0f); 
	        mcDELAY_US(2);
			
			//enter sref and check sref state
			*((volatile int *)0x10006014) |= (1 << 1);	 
			while (((*((volatile int *)0x100063b4) >> 21) & 1) == 0)
			{
			}
			mcSHOW_DBG_MSG(("Self refresh state(%d)\n", u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MISC_STATUSA), MISC_STATUSA_SREF_STATE)));
			
			//disale auto refresh
			vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 1, REFCTRL0_REFDIS);
			
			//delay 120s
			mcSHOW_DBG_MSG(("delay 120s ......\n"));
			mdelay(120000);
			
			//enable auto refresh
			vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 0, REFCTRL0_REFDIS);
			
			//exit sref 
			//DramcEnterSelfRefresh(p,0);
			*((volatile int *)0x10006014) &= ~(1<<1); 
			while (((*((volatile int *)0x100063b4) >> 21) & 1) == 1)
			{
			}
			mcSHOW_DBG_MSG(("Self refresh state(%d)\n", u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_MISC_STATUSA), MISC_STATUSA_SREF_STATE)));

	        DramcModeRegWrite(p, 17, 0x0); 
	        mcDELAY_US(2);
	        
	        //read data and compare it 
			ReadData(dram_rank_base[u1RankIdx], 0x100000, 0x5a5a);
			ReadData(dram_rank_base[u1RankIdx] + 0x20000000, 0x100000, 0x5a5a);
	    }
	}
    mcSHOW_DBG_MSG(("[PasrTest] done\n"));

}

void ReserveModeTest(DRAMC_CTX_T *p)
{
    U8 u1RankIdx, i;
    U32 dram_rank_base[2];
	
    mcSHOW_DBG_MSG(("\n[ReserveModeTest] begin\n"));
    get_dram_rank_size(dram_rank_base);
	dram_rank_base[1] = DDR_BASE + dram_rank_base[0];
	dram_rank_base[0] = DDR_BASE;
	

        //read data and compare it 
        if((g_ddr_reserve_enable==1) && (g_ddr_reserve_success==1))
        {
            for(u1RankIdx =0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
			ReadData(dram_rank_base[u1RankIdx], 0x40000000, 0x5a5a);
        }
        //write data
        else
        {
    
            vIO32WriteFldMulti(DRAMC_REG_TEST2_3, P_Fld(0, TEST2_3_TEST2W) | P_Fld(0, TEST2_3_TEST2R) | P_Fld(0, TEST2_3_TEST1));
            vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(0x0, DUMMY_RD_DUMMY_RD_EN)
                                        | P_Fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
                                        | P_Fld(0x0, DUMMY_RD_DQSG_DMYRD_EN)
                                        | P_Fld(0x0, DUMMY_RD_DMY_RD_DBG));	
            vIO32WriteFldAlign_All(DRAMC_REG_STBCAL, 0, STBCAL_STBCALEN);   // PI tracking off = HW gating tracking off
            vIO32WriteFldAlign_All(DRAMC_REG_STBCAL, 0, STBCAL_STB_SELPHYCALEN);
		for(u1RankIdx =0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
		{
			WriteData(dram_rank_base[u1RankIdx], 0x40000000, 0x5a5a);
			ReadData(dram_rank_base[u1RankIdx], 0x40000000, 0x5a5a);
		}
    }

	//if((g_ddr_reserve_enable==1) && (g_ddr_reserve_success==1))
		//while(1);
	mtk_wdt_sw_reset();
	mcSHOW_DBG_MSG(("\n[ReserveModeTest] done\n"));

}

void SscTest(DRAMC_CTX_T *p)
{
	U32 i;
	mcSHOW_DBG_MSG(("\n[SscTest] begin\n"));
	mcSHOW_DBG_MSG(("SSC %s\n", p->ssc_en ? "ON" : "OFF"));
	for(i=0; i<100; i++)
	{
		mcSHOW_DBG_MSG(("%d:", i));
		#if ENABLE_DDRPHY_FREQ_METER
		DDRPhyFreqMeter(0, 0x1b);
		#endif
		TestEngineCompare(p);
		mcDELAY_US(2);
	}
	mcSHOW_DBG_MSG(("\n[SscTest] done\n"));
}

void HwTracking(DRAMC_CTX_T *p)
{
	U8 u1RankIdx, i, j, vol_adjust = 3;//bit0 Vcore, bit Vmem
	U32 u4PI, u4UI, u4UI_P1, u42T;
	U32 u4PI_NAO, u4UI_NAO, u4UI_P1_NAO;
	U32 u4result, u4test_len = 0x100000;
	U32 dram_rank_base[2];
	
	mcSHOW_DBG_MSG(("\n[HwTracking] begin\n"));
	get_dram_rank_size(dram_rank_base);
	dram_rank_base[1] = DDR_BASE + dram_rank_base[0];
	dram_rank_base[0] = DDR_BASE;

	u4result = TestEngineCompare(p);
	vIO32WriteFldMulti(DRAMC_REG_TEST2_3, P_Fld(0, TEST2_3_TEST2W) | P_Fld(0, TEST2_3_TEST2R) | P_Fld(0, TEST2_3_TEST1));
	vIO32WriteFldAlign(DRAMC_REG_TEST2_2, u4test_len, TEST2_2_TEST2_OFF);
	if (p->support_rank_num == RANK_DUAL)
		vIO32WriteFldAlign(DRAMC_REG_TEST2_4, 3, TEST2_4_TESTAGENTRKSEL);

	mcSHOW_DBG_MSG(("set Vcore 1.15V\n"));
	pmic_config_interface(0x312, 0x48, 0x7f, 0);
	pmic_config_interface(0x314, 0x48, 0x7f, 0);
	mcSHOW_DBG_MSG(("set Vmem 1.20V\n"));
	pmic_config_interface(0x554, 0x0, 0xf, 8);

	for(u1RankIdx =0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
	{
		*(volatile unsigned int *)(dram_rank_base[u1RankIdx]) = 0x12345678;
		*(volatile unsigned int *)(dram_rank_base[u1RankIdx]+0x40) = 0x12345678;
		mcSHOW_DBG_MSG(("Addr[0x%x]= 0x%x, Addr[0x%x]= 0x%x\n", dram_rank_base[u1RankIdx], *(volatile unsigned int *)(dram_rank_base[u1RankIdx]), dram_rank_base[u1RankIdx]+0x40, *(volatile unsigned int *)(dram_rank_base[u1RankIdx]+0x40)));
	}
	for (vol_adjust=1; vol_adjust<4; vol_adjust++) 
	{
		for (i=0; i<100; i++)
		{
			mcSHOW_DBG_MSG(("vol:%d loop %d:\n", vol_adjust, i));
			for(j=0; j<4; j++)
			{
				for(u1RankIdx =0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
				{
					p->rank = u1RankIdx;
					//read DQS gating PI & UI
					u4PI = u4IO32Read4B(DRAMC_REG_SHURK0_DQSIEN);
					u4UI = u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQSG1);
					u42T = u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQSG0);
					//read NAO gating PI & UI
					u4PI_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENDLY);
					u4UI_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENUIDLY);
					u4UI_P1_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENUIDLY_P1);
					mcSHOW_DBG_MSG(("rank%d Vcore PI:0x%x, UI:0x%x, 2T:0x%x, PI_NAO:0x%x, UI_NAO:0x%x, UI_P1_NAO:0x%x\n", 
														u1RankIdx, u4PI, u4UI, u42T, u4PI_NAO, u4UI_NAO, u4UI_P1_NAO));
				}
				p->rank = 0;
				mcSHOW_DBG_MSG(("enable test engine2\n"));
				vIO32WriteFldMulti(DRAMC_REG_TEST2_3, P_Fld(1, TEST2_3_TEST2W) | P_Fld(1, TEST2_3_TEST2R) | P_Fld(0, TEST2_3_TEST1));

				switch (j) 
				{
					case 0://Vcore 1.15V->1.31V, Vmem 1.20V->1.30V
						if ((vol_adjust & 1) == 1) {
							mcSHOW_DBG_MSG(("set Vcore 1.31V\n"));
							pmic_config_interface(0x312, 0x62, 0x7f, 0); 
							pmic_config_interface(0x314, 0x62, 0x7f, 0);
						}
						if ((vol_adjust & 2) == 2) {
							mcSHOW_DBG_MSG(("set Vmem 1.30V\n"));
							pmic_config_interface(0x554, 0xb, 0xf, 8);
						}
					break;
					case 1://Vcore 1.31V->1.15V, Vmem 1.30V->1.20V
					case 3://Vcore 1.05V->1.15V, Vmem 1.14V->1.20V
						if ((vol_adjust & 1) == 1) {
							mcSHOW_DBG_MSG(("set Vcore 1.15V\n"));
							pmic_config_interface(0x312, 0x48, 0x7f, 0);
							pmic_config_interface(0x314, 0x48, 0x7f, 0);
						}
						if ((vol_adjust & 2) == 2) {
							mcSHOW_DBG_MSG(("set Vmem 1.20V\n"));
							pmic_config_interface(0x554, 0x0, 0xf, 8);
						}
					break;
					case 2://Vcore 1.15V->1.05V, Vmem 1.20V->1.14V
					default:
						if ((vol_adjust & 1) == 1) {
							mcSHOW_DBG_MSG(("set Vcore 1.05V\n"));
							pmic_config_interface(0x312, 0x38, 0x7f, 0);
							pmic_config_interface(0x314, 0x38, 0x7f, 0);
						}
						if ((vol_adjust & 2) == 2) {
							mcSHOW_DBG_MSG(("set Vmem 1.14V\n"));
							pmic_config_interface(0x554, 0x3, 0xf, 8);
						}
				}

		        while((u4IO32ReadFldAlign(DRAMC_REG_ADDR(DRAMC_REG_TESTRPT), TESTRPT_DM_CMP_CPT))==0)
		        {
					mcSHOW_DBG_MSG(("wait cmp ready\n"));
					mcDELAY_US(1000000);
		        }
				vIO32WriteFldMulti(DRAMC_REG_TEST2_3, P_Fld(0, TEST2_3_TEST2W) | P_Fld(0, TEST2_3_TEST2R) | P_Fld(0, TEST2_3_TEST1));
				u4result = (u4IO32Read4B(DRAMC_REG_CMP_ERR));

				mcSHOW_DBG_MSG(("CMP LEN 0x%x %s\n", u4test_len, u4result ? "FAIL" : "PASS"));
			}
		}
	}
	for(u1RankIdx =0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
	{
		mcSHOW_DBG_MSG(("Addr[0x%x]= 0x%x, Addr[0x%x]= 0x%x\n", dram_rank_base[u1RankIdx], *(volatile unsigned int *)(dram_rank_base[u1RankIdx]), dram_rank_base[u1RankIdx]+0x40, *(volatile unsigned int *)(dram_rank_base[u1RankIdx]+0x40)));		
	}

#if 0
    for(u1RankIdx =0; u1RankIdx < (U32)(p->support_rank_num); u1RankIdx++)
    {	
    	mcSHOW_DBG_MSG(("rank%d:\n", u1RankIdx));
		vSetRank(p, u1RankIdx);
		for(u1Vol=0; u1Vol<10; u1Vol++)
		{
			mcSHOW_DBG_MSG(("Vol%d:\n", u1Vol));
			//read DQS gating PI & UI
			u4PI = u4IO32Read4B(DRAMC_REG_SHURK0_DQSIEN);
			u4UI = u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQSG1);	
			u42T = u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQSG0);
			//read NAO gating PI & UI
			u4PI_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENDLY);
			u4UI_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENUIDLY);
			u4UI_P1_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENUIDLY_P1);
			mcSHOW_DBG_MSG(("Before changing Vcore PI:0x%x, UI:0x%x, 2T:0x%x, PI_NAO:0x%x, UI_NAO:0x%x, u4UI_P1_NAO:0x%x\n", 
												u4PI, u4UI, u42T, u4PI_NAO, u4UI_NAO, u4UI_P1_NAO));
			//change Vcore
			//Vcore 1.15V
			pmic_config_interface(0x312, 0x48+u1Vol, 0x7f, 0); 
			pmic_config_interface(0x314, 0x48+u1Vol, 0x7f, 0); 

			TestEngineCompare(p);

			//read DQS gating PI & UI
			u4PI = u4IO32Read4B(DRAMC_REG_SHURK0_DQSIEN);
			u4UI = u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQSG1);
			u42T = u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQSG0);
			//read NAO gating PI & UI
			u4PI_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENDLY);
			u4UI_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENUIDLY);
			u4UI_P1_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENUIDLY_P1);			
			mcSHOW_DBG_MSG(("After changing Vcore PI:0x%x, UI:0x%x, 2T:0x%x, PI_NAO:0x%x, UI_NAO:0x%x, u4UI_P1_NAO:0x%x\n", 
												u4PI, u4UI, u42T, u4PI_NAO, u4UI_NAO, u4UI_P1_NAO));
		
			//enter sref
			*((volatile int *)0x10006014) |= (1 << 1);	 
			while (((*((volatile int *)0x100063b4) >> 21) & 1) == 0)
			{
			}

			//exit sref	
			*((volatile int *)0x10006014) &= ~(1<<1); 
			while (((*((volatile int *)0x100063b4) >> 21) & 1) == 1)
			{
			}

			//read DQS gating PI & UI
			u4PI = u4IO32Read4B(DRAMC_REG_SHURK0_DQSIEN);
			u4UI = u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQSG1);
			u42T = u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQSG0);
			//read NAO gating PI & UI
			u4PI_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENDLY);
			u4UI_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENUIDLY);
			u4UI_P1_NAO = u4IO32Read4B(DRAMC_REG_RK0_DQSIENUIDLY_P1);
			mcSHOW_DBG_MSG(("After sref PI:0x%x, UI:0x%x, 2T:0x%x, PI_NAO:0x%x, UI_NAO:0x%x, u4UI_P1_NAO:0x%x\n", 
												u4PI, u4UI, u42T, u4PI_NAO, u4UI_NAO, u4UI_P1_NAO));
	
		}
	}
#endif
	mcSHOW_DBG_MSG(("\n[HwTracking] done\n"));

}
#endif

