/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/

//=============================================================================
//  Include Files
//=============================================================================
#include "dramc_common.h"
#include "dramc_pi_api.h"
#include "x_hal_io.h"
#include "emi_hw.h"
#if __FLASH_TOOL_DA__
#include "../pmic/upmu_hw.h"
#else
#include "upmu_hw.h"
#endif
#ifdef DDR_RESERVE_MODE
#include "platform.h"
#include "wdt.h"
#endif

//=============================================================================
//  Definition
//=============================================================================

//=============================================================================
//  Global Variables
//=============================================================================
#ifdef DDR_INIT_TIME_PROFILING
static U32 CPUCycleSum=0;
#endif

DRAMC_CTX_T *psCurrDramCtx;
DRAMC_CTX_T DramCtx =
{
  RANK_DUAL,        //DRAM_RANK_NUMBER_T
  RANK_0,               //DRAM_RANK_T
  TYPE_LPDDR3,        // DRAM_DRAM_TYPE_T
  ODT_OFF,
  CBT_NORMAL_MODE,
  DBI_OFF,
  DBI_OFF,
  PACKAGE_SBS,        // DRAM_PACKAGE_T
  DATA_WIDTH_32BIT,     // DRAM_DATA_WIDTH_T
  DEFAULT_TEST2_1_CAL,    // test2_1;
  DEFAULT_TEST2_2_CAL,    // test2_2;
  TEST_XTALK_PATTERN,     // test_pattern;
  DDR_DDR1600,
  0x88, //vendor_id initial value
  0, //density
  DISABLE,  // fglow_freq_write_en;
  DISABLE,  // ssc_en;
  DISABLE,   // en_4bitMux;
  DISABLE,   // scan_rx_vref;
  PIN_MUX_TYPE_LPDDR3_178BALL,//pinmux
  DISABLE, //asymmetric
  0,  // min dq bit win size;
  0,   // all dq bit win sum;
  //aru4CalResultFlag[RANK_MAX]
  {0,0},
  //aru4CalExecuteFlag[RANK_MAX]
  {0,0},

#if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
  {0,0}, //BOOL arfgWriteLevelingInitShif;
#endif
#if TX_PERBIT_INIT_FLOW_CONTROL
  {FALSE, FALSE},//BOOL fgTXPerbifInit;
#endif
};

//=============================================================================
//  External references
//=============================================================================
#ifdef DRAM_CALIB_LOG
extern U32 gDRAM_CALIB_LOG_pointer;
extern SAVE_TO_SRAM_FORMAT_T   gDRAM_CALIB_LOG;
#endif
extern void EMI_Init(EMI_SETTINGS *emi_set);

void Dramc_DDR_Reserved_Mode_setting(void)
{
	DRAMC_CTX_T DummyDramCtx;
	//select ddr phy clock
	*((volatile unsigned int *)0x10000000) |= (1 << 1);
	DramPhyReset(&DummyDramCtx);
	return;
}

#if CPU_RW_TEST_AFTER_K
void vDramCPUReadWriteTestAfterCalibration(DRAMC_CTX_T *p)
{
    U8 u1DumpInfo=0, u1RankIdx;
    U32 uiLen, count, uiFixedAddr, uiRankdAddr[RANK_MAX], dram_rank_size[2]={0,0};
    U32 pass_count, err_count;
    uiLen = 0xffff;

#if GATING_ONLY_FOR_DEBUG
    DramcGatingDebugInit(p);
#endif

    uiRankdAddr[0] = DDR_BASE;
    get_dram_rank_size(dram_rank_size);

#if DUAL_RANK_ENABLE
    uiRankdAddr[1] = uiRankdAddr[0] + dram_rank_size[0];
    for(u1RankIdx =0; u1RankIdx< p->support_rank_num; u1RankIdx++)
#endif
    {
        u1DumpInfo=0;
        err_count=0;
        pass_count=0;

        #if GATING_ONLY_FOR_DEBUG
        DramcGatingDebugRankSel(p, u1RankIdx);
        #endif

        uiFixedAddr = uiRankdAddr[u1RankIdx];

        for (count= 0; count<uiLen; count+=4)
        {
            *(volatile unsigned int   *)(count +uiFixedAddr) = count + (0x5a5a <<16);
        }

        for (count=0; count<uiLen; count+=4)
        {
            if (*(volatile unsigned int   *)(count +uiFixedAddr) != count + (0x5a5a <<16))
            {
                //mcSHOW_DBG_MSG(("[Fail] Addr %xh = %xh\n",count, *(volatile unsigned int   *)(count +uiFixedAddr)));
                err_count++;
            }
            else
                pass_count ++;
        }

        if(err_count)
        {
            mcSHOW_DBG_MSG(("[MEM_TEST] Rank %d Fail. ", u1RankIdx));
            u1DumpInfo =1;
        }
        else
        {
            mcSHOW_DBG_MSG(("[MEM_TEST] Rank %d OK. ", u1RankIdx));
        }
        mcSHOW_DBG_MSG(("(uiFixedAddr 0x%X, Pass count =%d, Fail count =%d)\n", uiFixedAddr, pass_count, err_count));
    }

    if(u1DumpInfo)
    {
        // Read gating error flag
        DramcDumpDebugInfo(p);
    }

    #if GATING_ONLY_FOR_DEBUG
    DramcGatingDebugExit(p);
    #endif
}
#endif

static void vCalibration_Flow_DDR(DRAMC_CTX_T *p)
{
    U8 u1RankMax;
    S8 s1RankIdx;

#ifdef DDR_INIT_TIME_PROFILING
    UINT32 CPU_Cycle;
    //TimeProfileBegin();
#endif

#if ENABLE_CA_TRAINING
    if (p->dram_type == TYPE_LPDDR3) {
        vSetRank(p, RANK_0);
        CATrainingLP3(p);
        DramcModeRegInit_LP3(p);
    }
#endif

#ifdef DDR_INIT_TIME_PROFILING
    CPU_Cycle=TimeProfileEnd();
    mcSHOW_ERR_MSG(("DRAMC CA train takes %d ms\n\r", CPU_Cycle));
    TimeProfileBegin();
#endif

#if GATING_ADJUST_TXDLY_FOR_TRACKING
    DramcRxdqsGatingPreProcess(p);
#endif

#if DUAL_RANK_ENABLE
    if (p->support_rank_num==RANK_DUAL)
        u1RankMax = RANK_MAX;
    else
#endif
        u1RankMax = RANK_1;

    for(s1RankIdx=RANK_0; s1RankIdx<u1RankMax; s1RankIdx++)
    {
        vSetRank(p, s1RankIdx);

#if DUAL_RANK_ENABLE
        if((p->support_rank_num==RANK_SINGLE) || ((p->support_rank_num==RANK_DUAL) && (s1RankIdx == RANK_0)))
#endif
        {
            #if ENABLE_WRITE_LEVELING
            if (p->dram_type != TYPE_LPDDR2)
            {
                if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
                {
                    #if SUPPORT_TYPE_DDR3X4
                    DramcWriteLeveling4Bit((DRAMC_CTX_T *) p, 0);
                    DramcWriteLeveling4Bit((DRAMC_CTX_T *) p, 1);
                    #endif
                }
                else
                {
                    DramcWriteLeveling((DRAMC_CTX_T *) p);//Dram will be reset when finish write leveling
                }
            }
            #endif

            #ifdef DDR_INIT_TIME_PROFILING
                CPU_Cycle=TimeProfileEnd();
                mcSHOW_ERR_MSG(("Rank %d Write leveling takes %d ms\n\r", s1RankIdx, CPU_Cycle));
                TimeProfileBegin();
            #endif
        }

    #if LJPLL_FREQ_DEBUG_LOG
        DDRPhyFreqMeter(0, 0x1b);
    #endif

        DramcRxdqsGatingCal(p);

    #ifdef DDR_INIT_TIME_PROFILING
        CPU_Cycle=TimeProfileEnd();
        mcSHOW_ERR_MSG(("Rank %d Gating takes %d ms\n\r", s1RankIdx, CPU_Cycle));
        TimeProfileBegin();
    #endif

    #if LJPLL_FREQ_DEBUG_LOG
        DDRPhyFreqMeter(0, 0x1b);
    #endif
        DramcRxdatlatCal((DRAMC_CTX_T *) p);

    #ifdef DDR_INIT_TIME_PROFILING
        CPU_Cycle=TimeProfileEnd();
        mcSHOW_ERR_MSG(("Rank %d Datlat takes %d ms\n\r", s1RankIdx, CPU_Cycle));
        TimeProfileBegin();
    #endif

    #if LJPLL_FREQ_DEBUG_LOG
        DDRPhyFreqMeter(0, 0x1b);
    #endif

    #if DUAL_RANK_ENABLE
    #ifndef DUAL_RANK_RX_K
        if(s1RankIdx==RANK_0 || p->rx_eye_scan == ENABLE)
    #endif
    #endif
        {
        if (p->rx_eye_scan == ENABLE)
            DramcRxEyeScan((DRAMC_CTX_T *)p, 1);
        else
            DramcRxWindowPerbitCal((DRAMC_CTX_T *) p, 1);

        #ifdef DDR_INIT_TIME_PROFILING
            CPU_Cycle=TimeProfileEnd();
            mcSHOW_ERR_MSG(("Rnak %d RX takes %d ms\n\r", s1RankIdx, CPU_Cycle));
            TimeProfileBegin();
        #endif
        }

    #if DUAL_RANK_ENABLE
    #ifndef DUAL_RANK_TX_K
        if(s1RankIdx==RANK_0 || p->rx_eye_scan == ENABLE)
    #endif
    #endif
        {      
            if (p->dram_type == TYPE_PCDDR4)
            {
                mcSHOW_ERR_MSG(("PCDDR4 VrefDQ Calibration start:\n")); 
                DramcTxVrefDQCalibration((DRAMC_CTX_T *) p);
                mcSHOW_ERR_MSG(("PCDDR4 VrefDQ Calibration done!\n")); 
            }	
            DramcTxWindowPerbitCal((DRAMC_CTX_T *) p, TX_DQ_DQS_MOVE_DQ_DQM);
        }
    }

    vSetRank(p, RANK_0);

#if GATING_ADJUST_TXDLY_FOR_TRACKING
    DramcRxdqsGatingPostProcess(p);
#endif

#if DUAL_RANK_ENABLE
    if (p->support_rank_num==RANK_DUAL)
    {
        DramcDualRankRxdatlatCal(p);
    }
#endif

#if LJPLL_FREQ_DEBUG_LOG
    DDRPhyFreqMeter(0, 0x1b);
#endif
    v4bitPinmuxEnableVerify(p);
}

static void vDramCalibrationAllChannel(DRAMC_CTX_T *p)
{
    vCalibration_Flow_DDR(p);

#ifdef DRAM_CALIB_LOG
    i4WriteSramLog(gDRAM_CALIB_LOG_pointer, (unsigned int*) &gDRAM_CALIB_LOG, sizeof(gDRAM_CALIB_LOG)/4);
    gDRAM_CALIB_LOG_pointer += sizeof(gDRAM_CALIB_LOG)/4;
#endif

}

int GetDramInforAfterCalByMRR(DRAMC_CTX_T *p, DRAM_INFO_BY_MRR_T *DramInfo)
{
    U8 u1RankIdx, u1DieNumber=0;
    U16 u2Density;
    U32 u4Size, u4SizeBak = 0;

    // Read MR5 for Vendor ID
    DramcModeRegReadByRank(p, RANK_0, 5, &(DramInfo->u2MR5VendorID));
    DramInfo->u2MR5VendorID &= 0xff; // for byte mode, don't show value of another die.
    p->vendor_id = DramInfo->u2MR5VendorID;
    mcSHOW_DBG_MSG(("[Get MRR] Vendor %x.\n", DramInfo->u2MR5VendorID));

     for(u1RankIdx =0; u1RankIdx<RANK_MAX; u1RankIdx++)
        DramInfo->u4MR8Density[u1RankIdx] =0;

     // Read MR8 for dram density
    for(u1RankIdx =0; u1RankIdx<(p->support_rank_num); u1RankIdx++)
    {
        if((p->aru4CalExecuteFlag[u1RankIdx] !=0) && (p->aru4CalResultFlag[u1RankIdx]==0))
        {
            DramcModeRegReadByRank(p, u1RankIdx, 8, &u2Density);
            mcSHOW_DBG_MSG(("[Get MRR] MR8 %x.\n", u2Density));

            u1DieNumber = 1;
            if(((u2Density >> 6) & 0x3)==1) //OP[7:6] =0, x16 (2 die)
              u1DieNumber = 2;

            u2Density = (u2Density>>2)&0xf;

            switch(u2Density)
            {
                case 0x6:
                    u4Size = 0x20000000;  //4Gb
                    //DBG_MSG("[EMI]DRAM density = 4Gb\n");
                    break;
                case 0xE:
                    u4Size = 0x30000000;  //6Gb
                    //DBG_MSG("[EMI]DRAM density = 6Gb\n");
                    break;
                case 0x7:
                    u4Size = 0x40000000;  //8Gb
                    //DBG_MSG("[EMI]DRAM density = 8Gb\n");
                    break;
                case 0xD:
                    u4Size = 0x60000000;  //12Gb
                    //DBG_MSG("[EMI]DRAM density = 12Gb\n");
                    break;
                case 0x8:
                    u4Size = 0x80000000;  //16Gb
                    //DBG_MSG("[EMI]DRAM density = 16Gb\n");
                    break;
                //case 0x9:
                    //u4Size = 0x100000000L; //32Gb
                    //DBG_MSG("[EMI]DRAM density = 32Gb\n");
                    //break;
                default:
                    u4Size = 0x40000000; //reserved
            }

            if (u4Size > u4SizeBak)
                p->density = u2Density;

            u4SizeBak = u4Size;

            u4Size *= u1DieNumber;
            DramInfo->u4MR8DieNumber[u1RankIdx] = u1DieNumber-1;
            DramInfo->u4MR8Density[u1RankIdx] = u4Size;
        }

        mcSHOW_DBG_MSG(("[Get MRR] Rank %d, u1DieNumber %d, Desity 0x%x.\n", u1RankIdx, u1DieNumber, DramInfo->u4MR8Density[u1RankIdx]));
    }

    //while(1);
    return 0;
}

#if ENABLE_AUTO_DETECTION
static U8 DramRankNumberDetection(DRAMC_CTX_T *p)
{
    U8 u1RankBak;

    u1RankBak = u1GetRank(p);  // backup current rank setting

#if ENABLE_CA_TRAINING
    if (p->dram_type == TYPE_LPDDR3)
    {
        vSetRank(p, RANK_0);
        CATrainingLP3(p);
        DramcModeRegInit_LP3(p);
    }
#endif

#if ENABLE_WRITE_LEVELING
    vSetRank(p, RANK_1);
    DramcWriteLeveling((DRAMC_CTX_T *) p);//Dram will be reset when finish write leveling
#endif

    if(((p->aru4CalResultFlag[p->rank] >>DRAM_CALIBRATION_WRITE_LEVEL) &0x1) ==0)
    {
        p->support_rank_num = 2;
    }
    else
    {
        p->support_rank_num = 1;
    }
    mcSHOW_DBG_MSG(("[DramRankNumberDetection] %d, 0x%x\n", p->support_rank_num, p->aru4CalResultFlag[p->rank]));

    vSetRank(p, u1RankBak);  // restore rank setting

    return (p->support_rank_num-1);
}
#endif

int Before_Init_DRAM_While_Reserve_Mode_fail(DRAM_DRAM_TYPE_T dram_type)
{
    DRAMC_CTX_T * p;

    p = &DramCtx;
    mcSHOW_DBG_MSG(("========Reserve mode fall --> Before_Init_DRAM_While_Reserve_Mode_fail=======\n"));
    EnableDramcPhyDCM(p, 0);
    Switch26MHzDisableDummyReadRefreshAllBank(p);
    return 0;
}

#if (SUPPORT_TX_DELAY_LINE_CALI || SUPPORT_CATRAININT_DELAY_LINE_CALI)
DRAM_STATUS_T DramcMiockJmeter(DRAMC_CTX_T *p)
{
    U8 u8_1T = 0;
    U8 ucsearch_state, ucdqs_dly, fgcurrent_value, fginitial_value, ucstart_period, ucend_period, ucg_num_dlycell_perT;
    U32 u4sample_cnt, u4ones_cnt[DQS_NUMBER];
    U32 u4prv_register_B0_DQ6[2], u4prv_register_B1_DQ6[2];
    U32 u4prv_register_B0_DQ5[2], u4prv_register_B1_DQ5[2];
    U32 u4prv_register_B0_DQ4[2], u4prv_register_B1_DQ4[2];
    U32 u4prv_register_B0_DQ3[2], u4prv_register_B1_DQ3[2];
    U32 u4prv_register_CA_CMD4[2], u4prv_register_CA_CMD6[2];
    U32 u4prv_register_SHU1_B0_DQ7[2], u4prv_register_SHU1_B1_DQ7[2];
    U32 u4prv_register_MISC_CTRL1[2];
    U32 u4prv_register_EYESCAN, u4prv_register_STBCAL1;

    // error handling
    if (!p)
    {
        mcSHOW_ERR_MSG(("context is NULL\n"));
        return DRAM_FAIL;
    }

    //backup register value
    u4prv_register_EYESCAN = u4IO32Read4B(DRAMC_REG_EYESCAN);
    u4prv_register_STBCAL1 = u4IO32Read4B(DRAMC_REG_STBCAL1);

    U8 u1PhyIdx = 0;
    for(u1PhyIdx=0; u1PhyIdx<2; u1PhyIdx++)
    {
        u4prv_register_B0_DQ6[u1PhyIdx] = u4IO32Read4B((DDRPHY_B0_DQ6)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_B1_DQ6[u1PhyIdx] = u4IO32Read4B((DDRPHY_B1_DQ6)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_B0_DQ5[u1PhyIdx] = u4IO32Read4B((DDRPHY_B0_DQ5)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_B1_DQ5[u1PhyIdx] = u4IO32Read4B((DDRPHY_B1_DQ5)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_B0_DQ3[u1PhyIdx] = u4IO32Read4B((DDRPHY_B0_DQ3)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_B1_DQ3[u1PhyIdx] = u4IO32Read4B((DDRPHY_B1_DQ3)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_SHU1_B0_DQ7[u1PhyIdx] = u4IO32Read4B((DDRPHY_SHU1_B0_DQ7)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_SHU1_B1_DQ7[u1PhyIdx] = u4IO32Read4B((DDRPHY_SHU1_B1_DQ7)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_B0_DQ4[u1PhyIdx] = u4IO32Read4B((DDRPHY_B0_DQ4)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_B1_DQ4[u1PhyIdx] = u4IO32Read4B((DDRPHY_B1_DQ4)+(u1PhyIdx<<POS_BANK_NUM));
        u4prv_register_MISC_CTRL1[u1PhyIdx] = u4IO32Read4B((DDRPHY_MISC_CTRL1)+(u1PhyIdx<<POS_BANK_NUM));
    }

    //MCK4X CG
    vIO32WriteFldAlign_All((DDRPHY_MISC_CTRL1), 0, MISC_CTRL1_R_DMDQSIENCG_EN);

    // Bypass DQS glitch-free mode
    // RG_RX_*RDQ_EYE_DLY_DQS_BYPASS_B**
    vIO32WriteFldAlign_All((DDRPHY_B0_DQ6), 1, B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0);
    vIO32WriteFldAlign_All((DDRPHY_B1_DQ6), 1, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1);

    //Enable DQ eye scan
    //RG_??_RX_EYE_SCAN_EN
    //RG_??_RX_VREF_EN
    //RG_??_RX_SMT_EN
    vIO32WriteFldAlign_All((DDRPHY_B0_DQ5), 1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
    vIO32WriteFldAlign_All((DDRPHY_B1_DQ5), 1, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
    vIO32WriteFldAlign_All((DDRPHY_B0_DQ3), 1, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
    vIO32WriteFldAlign_All((DDRPHY_B1_DQ3), 1, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);

    //JM_SEL
    vIO32WriteFldAlign_All((DDRPHY_B0_DQ6), 1, B0_DQ6_RG_RX_ARDQ_JM_SEL_B0);
    vIO32WriteFldAlign_All((DDRPHY_B1_DQ6), 1, B1_DQ6_RG_RX_ARDQ_JM_SEL_B1);

    //Enable MIOCK jitter meter mode ( RG_RX_MIOCK_JIT_EN=1)
    vIO32WriteFldAlign_All((DDRPHY_PLL1), 1, PLL1_RG_RX_MIOCK_JIT_EN);

    //Disable DQ eye scan (b'1), for counter clear
    vIO32WriteFldAlign_All((DDRPHY_PLL1), 0, PLL1_R_DMDQSERRCNT_DIS);

    ucsearch_state = 0;
    for (ucdqs_dly=0; ucdqs_dly<128; ucdqs_dly++)
    {

        //Set DQS delay (RG_??_RX_DQS_EYE_DLY)
        vIO32WriteFldAlign_All((DDRPHY_B0_DQ4), ucdqs_dly, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0);
        vIO32WriteFldAlign_All((DDRPHY_B0_DQ4), ucdqs_dly, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0);
        vIO32WriteFldAlign_All((DDRPHY_B1_DQ4), ucdqs_dly, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1);
        vIO32WriteFldAlign_All((DDRPHY_B1_DQ4), ucdqs_dly, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1);

        DramPhyReset(p);

        //Reset eye scan counters (reg_sw_rst): 1 to 0
        vIO32WriteFldAlign_All(DDRPHY_PLL4, 1, PLL4_RG_RPHYPLL_PHYRESET);
		mcDELAY_US(1);
        vIO32WriteFldAlign_All(DDRPHY_PLL4, 0, PLL4_RG_RPHYPLL_PHYRESET);

        //Enable DQ eye scan (b'1)
        vIO32WriteFldAlign_All((DDRPHY_PLL1), 1, PLL1_RG_RX_EYE_SCAN_EN);

        // 2ns/sample, here we delay 1ms about 500 samples
		mcDELAY_US(1000);

        //Disable DQ eye scan (b'1), for counter latch
		vIO32WriteFldAlign_All((DDRPHY_PLL1), 0, PLL1_RG_RX_EYE_SCAN_EN);

		u4sample_cnt = u4IO32Read4B(DDRPHY_BASE_ADDR + 0xFDC);
		u4ones_cnt[0] = u4IO32Read4B(DDRPHY_BASE_ADDR + 0xFF0);
		u4ones_cnt[1] = u4IO32Read4B(DDRPHY_BASE_ADDR + 0xFF4);
		u4ones_cnt[2] = u4IO32Read4B(DDRPHY_BASE_ADDR + 0xFF8);
		u4ones_cnt[3] = u4IO32Read4B(DDRPHY_BASE_ADDR + 0xFFC);

        mcSHOW_DBG_MSG(("%d : %d, %d, %d, %d, %d\n", ucdqs_dly, u4sample_cnt, u4ones_cnt[0],u4ones_cnt[1],u4ones_cnt[2],u4ones_cnt[3]));

        //change to boolean value
        if (u4ones_cnt[0] < (u4sample_cnt/2))
        {
            fgcurrent_value = 0;
        }
        else
        {
            fgcurrent_value = 1;
        }

        if (u8_1T)
        {
            if (ucsearch_state==0)
            {
                //record initial value at the beginning
                fginitial_value = fgcurrent_value;
                ucsearch_state = 1;
            }
            else if (ucsearch_state==1)
            {
                // check if change value
                if (fgcurrent_value != fginitial_value)
                {
                    // start of the period
                    fginitial_value = fgcurrent_value;
                    ucstart_period = ucdqs_dly;
                    ucsearch_state = 2;
                }
            }
            else if (ucsearch_state==2)
            {
                // check if change value
                if (fgcurrent_value != fginitial_value)
                {
                    fginitial_value = fgcurrent_value;
                    ucsearch_state = 3;
                }
            }
            else if (ucsearch_state==3)
            {
                // check if change value
                if (fgcurrent_value != fginitial_value)
                {
                    // end of the period, break the loop
                    ucend_period = ucdqs_dly;
                    ucsearch_state = 4;
                    break;
                }
            }
            else
            {
                //nothing
            }
        }
        else //only 0.5T data
        {
            if (ucsearch_state==0)
            {
                //record initial value at the beginning
                fginitial_value = fgcurrent_value;
                ucsearch_state = 1;
            }
            else if (ucsearch_state==1)
            {
                // check if change value
                if (fgcurrent_value != fginitial_value)
                {
                    // start of the period
                    fginitial_value = fgcurrent_value;
                    ucstart_period = ucdqs_dly;
                    ucsearch_state = 2;
                }
            }
            else if (ucsearch_state==2)
            {
                // check if change value
                if (fgcurrent_value != fginitial_value)
                {
                    // end of the period, break the loop
                    ucend_period = ucdqs_dly;
                    ucsearch_state = 4;
                   break;
                }
            }
        }
    }

    //restore to orignal value
    vIO32Write4B((DRAMC_REG_EYESCAN), u4prv_register_EYESCAN);
    vIO32Write4B((DRAMC_REG_STBCAL1), u4prv_register_STBCAL1);

    for(u1PhyIdx=0; u1PhyIdx<2; u1PhyIdx++)
    {
        vIO32Write4B((DDRPHY_B0_DQ6)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_B0_DQ6[u1PhyIdx]);
        vIO32Write4B((DDRPHY_B1_DQ6)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_B1_DQ6[u1PhyIdx]);
        vIO32Write4B((DDRPHY_B0_DQ5)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_B0_DQ5[u1PhyIdx]);
        vIO32Write4B((DDRPHY_B1_DQ5)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_B1_DQ5[u1PhyIdx]);
        vIO32Write4B((DDRPHY_B0_DQ4)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_B0_DQ4[u1PhyIdx]);
        vIO32Write4B((DDRPHY_B1_DQ4)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_B1_DQ4[u1PhyIdx]);
        vIO32Write4B((DDRPHY_B0_DQ3)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_B0_DQ3[u1PhyIdx]);
        vIO32Write4B((DDRPHY_B1_DQ3)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_B1_DQ3[u1PhyIdx]);
        vIO32Write4B((DDRPHY_SHU1_B0_DQ7)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_SHU1_B0_DQ7[u1PhyIdx]);
        vIO32Write4B((DDRPHY_SHU1_B1_DQ7)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_SHU1_B1_DQ7[u1PhyIdx]);
        vIO32Write4B((DDRPHY_MISC_CTRL1)+(u1PhyIdx<<POS_BANK_NUM), u4prv_register_MISC_CTRL1[u1PhyIdx]);
    }


    if(ucsearch_state!=4)
    {
        mcSHOW_DBG_MSG2(("===============================================================================\n"));
        mcSHOW_DBG_MSG2(("    MIOCK jitter meter - channel=%d\n", 0));
        mcSHOW_DBG_MSG(("    Less than 0.5T data. Cannot calculate delay cell time\n"));
        mcSHOW_DBG_MSG2(("===============================================================================\n"));
        p->density = 64;
        return DRAM_FAIL;
    }

    //Calculate 1 delay cell = ? ps
    // 1T = ? delay cell
	ucg_num_dlycell_perT = (ucend_period - ucstart_period)*(u8_1T ? 1 : 2);

    mcSHOW_DBG_MSG2(("===============================================================================\n"));
    mcSHOW_DBG_MSG2(("    MIOCK jitter meter - channel=%d\n", 0));
    mcSHOW_DBG_MSG2(("===============================================================================\n"));
    mcSHOW_DBG_MSG(("1T = (%d-%d)*%d = %d delay cells\n", ucend_period, ucstart_period, (u8_1T ? 1 : 2), ucg_num_dlycell_perT));

    p->density = ucg_num_dlycell_perT;

    return DRAM_OK;

}
#endif

//#define TEST_MODE_MRS
int Init_DRAM(EMI_SETTINGS *emi_set)
{
    DRAM_DRAM_TYPE_T dram_type;
    DRAMC_CTX_T * p;
#if ENABLE_AUTO_DETECTION
    DRAM_INFO_BY_MRR_T DramInfo;
#endif
    AC_TIMING_EXTERNAL_T *ACFromEmi = &(emi_set->AcTimeEMI);
	//---------------------------------------------------------------------------------
	dram_type = mt_get_dram_type();

    p = &DramCtx;
    p->dram_type = dram_type;
    p->dram_cbt_mode = emi_set->dram_cbt_mode;
    p->frequency = ACFromEmi->AC_TIME_EMI_FREQUENCY;
    p->pinmux = (PIN_MUX_TYPE_T)(emi_set->PIN_MUX_TYPE);

    if (p->pinmux >= PIN_MUX_TYPE_MAX)
        p->pinmux = PIN_MUX_TYPE_LPDDR3_178BALL;

    if ((p->pinmux == PIN_MUX_TYPE_LPDDR3_168BALL) || (p->pinmux == PIN_MUX_TYPE_DDR3X8) || (p->pinmux == PIN_MUX_TYPE_LPDDR3_211BALL)||(p->pinmux == PIN_MUX_TYPE_LPDDR3_eMCP_1)||(p->pinmux == PIN_MUX_TYPE_LPDDR3_eMCP_MMD))
        p->en_4bitMux = ENABLE;

    if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
    {
        mcSHOW_DBG_MSG(("PIN MUX TYPE_DDR3X4 INIT\n"));
        //vIO32WriteFldAlign(DRAMC_REG_DDRCONF0, 0x0, DDRCONF0_DM64BITEN);
        vIO32WriteFldAlign(DDRPHY_PLL1, 0x1, PLL1_DDR3_2X8_OPEN);
        vIO32WriteFldMulti(DRAMC_REG_ZQCS, P_Fld(0x1, ZQCS_SWAP13) | P_Fld(0x1, ZQCS_SWAP12));
    }

    if (emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 30))
    {
        p->data_width = DATA_WIDTH_16BIT;
    }

    if ((dram_type == TYPE_PCDDR3) || (dram_type == TYPE_PCDDR4))
    {
        if (emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 28))
    	{
        	p->asymmetric = ENABLE;
    	}
    }
    else if ((dram_type == TYPE_LPDDR3) || (dram_type == TYPE_LPDDR2))
    {
        if (emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 28))
        {
            p->rx_eye_scan = ENABLE;
        }
    }

	//---------------------------------------------------------------------------------

    Global_Option_Init(p);

    DramcSwImpedanceCal(p);
	
    DramcInit((DRAMC_CTX_T *) p);
	
    vApplyConfigBeforeCalibration((DRAMC_CTX_T *) p);

#if (SUPPORT_TX_DELAY_LINE_CALI || SUPPORT_CATRAININT_DELAY_LINE_CALI)
    if (p->frequency >= DDR_DDR1333)
	    DramcMiockJmeter(p);
#endif

#if ENABLE_AUTO_DETECTION
    if (p->dram_type == TYPE_LPDDR3) {
        DramRankNumberDetection(p);
        DramReInitForCalibration(p);  // Restore setting after rank dection (especially DQ= DQS+16)
    }
    else 
    {
        p->support_rank_num = ((emi_set->EMI_CONA_VAL >> 17) & 1) + 1;
    }
#else
    p->support_rank_num = ((emi_set->EMI_CONA_VAL >> 17) & 1) + 1;
#endif

    if (p->support_rank_num == RANK_DUAL)
        vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0, RKCFG_CS2RANK);

    vDramCalibrationAllChannel(p);

    if (p->asymmetric == ENABLE)
    {
        vIO32WriteFldAlign(DDRPHY_MISC_CTRL1, 0x1, MISC_CTRL1_R_DMASYNNETRIC_DRAM_EN);
        vIO32WriteFldAlign(DDRPHY_MISC_CTRL1+(1<<POS_BANK_NUM), 0x1, MISC_CTRL1_R_DMASYNNETRIC_DRAM_EN);

        vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ7, 0x2, SHU1_B0_DQ7_RG_ARDQ_REV_B0);
        vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ7+(1<<POS_BANK_NUM), 0x2, SHU1_B0_DQ7_RG_ARDQ_REV_B0);

        vIO32WriteFldAlign(DRAMC_REG_DDRCONF0, 0x0, DDRCONF0_DM64BITEN);
        vIO32WriteFldAlign(DRAMC_REG_DRAMCTRL, 0x1, DRAMCTRL_RANK_ASYM);
        vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0, RKCFG_CS2RANK);
        vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0x9, RKCFG_RKMODE);
        //emi setting should already configure row/col bit for rank1
        *(volatile unsigned *)EMI_CONA |= 0x1 << 17;
        *(volatile unsigned *)EMI_CONA &= ~(0x1 << 1);

        p->rank = RANK_1;
        DramPhyReset(p);
        DramcRxdqsGatingCal(p);
        DramcRxdatlatCal(p);
        DramPhyReset(p);
    }

#if ENABLE_AUTO_DETECTION 
    if (p->dram_type == TYPE_LPDDR3)
    {
        memset(&DramInfo, 0, sizeof(DramInfo));
        GetDramInforAfterCalByMRR(p, &DramInfo);
        vDramcUpdateEmiSetting(p, &DramInfo);
        vDramcACTimingOptimize(p);
    }
#endif
    vApplyConfigAfterCalibration(p);

#if CPU_RW_TEST_AFTER_K
    mcSHOW_DBG_MSG(("\n[MEM_TEST] 01: Before run time config\n"));
    vDramCPUReadWriteTestAfterCalibration(p);
#endif

    mcSHOW_DBG_MSG(("\nSettings after calibration ...\n"));
    DramcRunTimeConfig(p);

#if CPU_RW_TEST_AFTER_K
    mcSHOW_DBG_MSG(("\n[MEM_TEST] 01: After run time config\n"));
    vDramCPUReadWriteTestAfterCalibration(p);
#endif
    return 0;
}


