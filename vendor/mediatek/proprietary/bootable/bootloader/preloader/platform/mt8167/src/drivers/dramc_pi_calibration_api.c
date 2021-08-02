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
/*-----------------------------------------------------------------------------
 *
 * $Author: jc.wu $
 * $Date: 2012/6/5 $
 * $RCSfile: pi_calibration_api.c,v $
 * $Revision: #5 $
 *
 *---------------------------------------------------------------------------*/

/** @file pi_calibration_api.c
 *  Basic DRAMC calibration API implementation
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dramc_common.h"
#include "x_hal_io.h"
#include "dramc_pi_api.h"

#if 1//REG_ACCESS_PORTING_DGB
U8 RegLogEnable=0;
#endif

#define MAX_CA_PI_DELAY         63
#define MAX_CS_PI_DELAY         63
#define MAX_CLK_PI_DELAY        31


#define PASS_RANGE_NA   0x7fff

#define SIMULATION_LP3_CA_TRAINING 1
#define SIMULATION_WRITE_LEVELING  1
#define SIMULATION_GATING 1
#define SIMULATION_DATLAT 1
#define SIMULATION_SW_IMPED 1
#define SIMULATION_RX_PERBIT    1
#define SIMULATION_TX_PERBIT    1  // Please enable with write leveling

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
static U8 fgwrlevel_done = 0;

U8 u1MR2Value=0x1a;

PASS_WIN_DATA_T DlyPerBit[32];
PASS_WIN_DATA_T RxWinPerBit[2][DQ_DATA_WIDTH];
#if SIMULATION_LP3_CA_TRAINING || SIMULATION_WRITE_LEVELING
static S32 CATrain_ClkDelay;
#endif
static S32 wrlevel_dqs_final_delay[DQS_NUMBER];

U32 gDramcSwImpedanceResule[2];

#ifdef DRAM_CALIB_LOG
U32 gDRAM_CALIB_LOG_pointer=0;
#endif
#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
SAVE_TO_SRAM_FORMAT_CHANNEL_T gDRAM_CALIB_LOG;
#endif

U32 u4DRAMdebugLOgEnable = 0x01;
#ifdef TARGET_BUILD_VARIANT_ENG
U32 u4DRAMdebugLOgEnable2 = 1;
#else
U32 u4DRAMdebugLOgEnable2 = 0;
#endif

static void dle_factor_handler(DRAMC_CTX_T *p, U8 curr_val);

void vSetRankNumber(DRAMC_CTX_T *p)
{
    #if DUAL_RANK_ENABLE
    p->support_rank_num =RANK_DUAL;
    #else
    p->support_rank_num =RANK_SINGLE;
    #endif
}

void vSetRank(DRAMC_CTX_T *p, U8 ucRank)
{
    p->rank = ucRank;
}

U8 u1GetRank(DRAMC_CTX_T *p)
{
    return p->rank;
}

void vSetCalibrationResult(DRAMC_CTX_T *p, U8 ucCalType, U8 ucResult)
{
    p->aru4CalExecuteFlag[p->rank] |= (1<<ucCalType); // ececution done
    if (ucResult == DRAM_OK)  // Calibration OK
    {
        p->aru4CalResultFlag[p->rank] &= (~(1<<ucCalType));
    }
    else  //Calibration fail
    {
        p->aru4CalResultFlag[p->rank] |= (1<<ucCalType);
    }
}

void vGetCalibrationResult_All(DRAMC_CTX_T *p, U8 u1Rank, U32 *u4CalExecute, U32 *u4CalResult)
{
    *u4CalExecute = p->aru4CalExecuteFlag[u1Rank];
    *u4CalResult = p->aru4CalResultFlag[u1Rank];
}

#if 0  //no use now, disable for saving code size.
void vGetCalibrationResult(DRAMC_CTX_T *p, U8 ucCalType, U8 *ucCalExecute, U8 *ucCalResult)
{
    U32 ucCalResult_All, ucCalExecute_All;

    ucCalExecute_All = p->aru4CalExecuteFlag[p->rank];
    ucCalResult_All = p->aru4CalResultFlag[p->rank];

    *ucCalExecute = (U8)((ucCalExecute_All >>ucCalType) & 0x1);
    *ucCalResult =  (U8)((ucCalResult_All >>ucCalType) & 0x1);
}
#endif

const char *szCalibStatusName[DRAM_CALIBRATION_MAX]=
{
	"ZQ Calibration",
	"SW Impedance",
	"CA Training",
	"Write leveling",
	"RX DQS gating",
	"RX DATLAT",
	"RX DQ/DQS(RDDQC)",
	"RX DQ/DQS(Engine)",
	"TX DQ/DQS",
};

void vPrintCalibrationResult(DRAMC_CTX_T *p)
{
    U8  ucRankIdx, ucCalIdx;
    U32 ucCalResult_All, ucCalExecute_All;
    U8 ucCalResult, ucCalExecute;

    for(ucRankIdx=0; ucRankIdx<RANK_MAX; ucRankIdx++)
    {
        ucCalExecute_All = p->aru4CalExecuteFlag[ucRankIdx];
        ucCalResult_All = p->aru4CalResultFlag[ucRankIdx];
        mcSHOW_DBG_MSG(("[vPrintCalibrationResult] Rank= %d, (ucCalExecute_All 0x%x, ucCalResult_All 0x%x)\n", ucRankIdx, ucCalExecute_All, ucCalResult_All));

        for(ucCalIdx =0; ucCalIdx<DRAM_CALIBRATION_MAX; ucCalIdx++)
        {
            ucCalExecute = (U8)((ucCalExecute_All >>ucCalIdx) & 0x1);
            ucCalResult =  (U8)((ucCalResult_All >>ucCalIdx) & 0x1);
            mcSHOW_DBG_MSG(("    %s : Execute= %d, Result= %d (0: Ok, 1:Fail)\n", szCalibStatusName[ucCalIdx], ucCalExecute, ucCalResult));
        }

    }
}

void vPrintCalibrationBasicInfo(DRAMC_CTX_T *p)
{
    mcSHOW_DBG_MSG(("===============================================================================\n"));
    mcSHOW_DBG_MSG(("Dram Type= %d, Freqency= %d, rank %d\n",
                                        p->dram_type, p->frequency, p->rank));
    mcSHOW_DBG_MSG(("odt_onoff= %d, Byte mode= %d, Read_DBI= %d, Write DBI= %d \n",
                                        p->odt_onoff, p->dram_cbt_mode, p->DBI_R_onoff, p->DBI_W_onoff));
    mcSHOW_DBG_MSG(("===============================================================================\n"));
}


// for LP3 to control all PHY of single channel
void vIO32WriteFldAlign_Phy_All(U32 reg32, U32 val32, U32 fld)
{
    if(reg32<Channel_A_PHY_BASE_VIRTUAL)
    {
        mcSHOW_DBG_MSG(("\n[vIO32WriteFldAlign_Phy_All] wrong address %d\n", reg32));
        return;
    }

    reg32 &= 0xffff;

    vIO32WriteFldAlign(reg32+Channel_A_PHY_BASE_VIRTUAL, val32, fld);
    vIO32WriteFldAlign(reg32+Channel_B_PHY_BASE_VIRTUAL, val32, fld);

}

void vApplyConfigAfterCalibration(DRAMC_CTX_T *p)
{
    //DA mode
    vIO32WriteFldAlign_All(DDRPHY_B0_DQ6, 0x0, B0_DQ6_RG_RX_ARDQ_BIAS_PS_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DQ6, 0x0, B1_DQ6_RG_RX_ARDQ_BIAS_PS_B1);
    vIO32WriteFldAlign_All(DDRPHY_CA_CMD6, 0x0, CA_CMD6_RG_RX_ARCMD_BIAS_PS);

    vIO32WriteFldAlign_All(DDRPHY_B0_DQ6, 0x1, B0_DQ6_RG_TX_ARDQ_OE_EXT_DIS_B0);
    vIO32WriteFldAlign_All(DDRPHY_B1_DQ6, 0x1, B1_DQ6_RG_TX_ARDQ_OE_EXT_DIS_B1);
    vIO32WriteFldAlign_All(DDRPHY_CA_CMD6, 0x1, CA_CMD6_RG_TX_ARCMD_OE_EXT_DIS);

    //IMPCAL Settings
    vIO32WriteFldMulti_All(DRAMC_REG_IMPCAL, P_Fld(0, IMPCAL_IMPCAL_IMPPDP) | P_Fld(0, IMPCAL_IMPCAL_IMPPDN) |\
    							P_Fld(0, IMPCAL_IMPCAL_CALI_EN));    //RG_RIMP_BIAS_EN and RG_RIMP_VREF_EN move to IMPPDP and IMPPDN

    //Prevent M_CK OFF because of hardware auto-sync
    vIO32WriteFldAlign_All(DDRPHY_MISC_CG_CTRL0, 0, Fld(4,0,AC_MSKB0));

    //DFS- fix Gating Tracking settings
    vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL0, 0, MISC_CTRL0_R_STBENCMP_DIV4CK_EN);
    vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL1, 0, MISC_CTRL1_R_DMSTBENCMP_RK_OPT);

    vIO32WriteFldAlign_All(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_REFRDIS);    //MR4 Disable
    vIO32WriteFldMulti_All(DRAMC_REG_DQSOSCR, P_Fld(0x1, DQSOSCR_DQSOSCRDIS)|P_Fld(0x1, DQSOSCR_DQSOSCENDIS));  //MR18, MR19 Disable
    vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(0x0, DUMMY_RD_DUMMY_RD_EN)
                                            | P_Fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
                                            | P_Fld(0x0, DUMMY_RD_DQSG_DMYRD_EN)
                                            | P_Fld(0x0, DUMMY_RD_DMY_RD_DBG));

#if APPLY_POWER_INIT_SEQUENCE
    //CKE dynamic
    vIO32WriteFldMulti_All(DRAMC_REG_CKECTRL, P_Fld(0, CKECTRL_CKEFIXON) | P_Fld(0, CKECTRL_CKE1FIXON)
                                            | P_Fld(0, CKECTRL_CKEFIXOFF) | P_Fld(0, CKECTRL_CKE1FIXOFF));
    //// Enable  HW MIOCK control to make CLK dynamic
    vIO32WriteFldAlign_All(DRAMC_REG_DRAMC_PD_CTRL, 0, DRAMC_PD_CTRL_MIOCKCTRLOFF);
#endif

    //reset pad workaround
    vIO32WriteFldAlign_All(DDRPHY_CA_CMD8, 1, CA_CMD8_RG_TX_RRESETB_PULL_UP);
    mcDELAY_MS(2);
    vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL1, 0, MISC_CTRL1_R_DMDA_RRESETB_E);

}

void vApplyConfigBeforeCalibration(DRAMC_CTX_T *p)
{
    //Clk free run
    EnableDramcPhyDCM(p, 0);

    //---- ZQ CS init --------
    vIO32WriteFldAlign_All(DRAMC_REG_SHU_SCINTV, 0x1f, SHU_SCINTV_TZQLAT); //ZQ Calibration Time, unit: 38.46ns, tZQCAL min is 1 us. need to set larger than 0x1b
    vIO32WriteFldAlign_All(DRAMC_REG_SHU_CONF3, 0x1ff, SHU_CONF3_ZQCSCNT); //Every refresh number to issue ZQCS commands, only for DDR3/LPDDR2/LPDDR3/LPDDR4
    vIO32WriteFldAlign_All(DRAMC_REG_DRAMCTRL, 0, DRAMCTRL_ZQCALL);  // HW send ZQ command for both rank, disable it due to some dram only have 1 ZQ pin for two rank.

    //ZQCSDUAL=0, ZQCSMASK=0
    vIO32WriteFldMulti(DRAMC_REG_ZQCS, P_Fld(0, ZQCS_ZQCSDUAL)| P_Fld(0x0, ZQCS_ZQCSMASK));


    // ---- End of ZQ CS init -----
    vIO32WriteFldAlign_All(DRAMC_REG_SHU1_WODT, p->DBI_W_onoff, SHU1_WODT_DBIWR);
    vIO32WriteFldAlign_All(DDRPHY_SHU1_B0_DQ7, p->DBI_R_onoff, SHU1_B0_DQ7_R_DMDQMDBI_SHU_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU1_B1_DQ7, p->DBI_R_onoff, SHU1_B1_DQ7_R_DMDQMDBI_SHU_B1);

    //disable MR4 read, REFRDIS=1
    vIO32WriteFldAlign_All(DRAMC_REG_SPCMDCTRL, 1, SPCMDCTRL_REFRDIS);

    // Disable ZQ
    vIO32WriteFldAlign_All(DRAMC_REG_SPCMDCTRL, 0, SPCMDCTRL_ZQCSDISB);   //ZQCSDISB=0
    vIO32WriteFldAlign_All(DRAMC_REG_SPCMDCTRL, 0, SPCMDCTRL_ZQCALDISB);   //ZQCALDISB=0

    // Disable HW gating tracking first, 0x1c0[31], need to disable both UI and PI tracking or the gating delay reg won't be valid.
    DramcHWGatingOnOff(p, 0);

    // Disable gating debug
    vIO32WriteFldAlign_All(DRAMC_REG_EYESCAN, 0, EYESCAN_STB_GERRSTOP);

    // ARPI_DQ SW mode mux, TX DQ use 1: PHY Reg 0: DRAMC Reg
    vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL1, 1, MISC_CTRL1_R_DMARPIDQ_SW);

    // Set to all-bank refresh
    vIO32WriteFldAlign_All(DRAMC_REG_REFCTRL0,  0, REFCTRL0_PBREFEN);
}


//Reset PHY to prevent glitch when change DQS gating delay or RX DQS input delay
// [Lynx] Everest :  All DramC and All Phy have to reset together.
void DramPhyReset(DRAMC_CTX_T *p)
{
    U32 backupReg0x64, backupReg0xC8, backupReg0xD0;

    backupReg0x64 = u4IO32Read4B(DRAMC_REG_SPCMDCTRL);
    backupReg0xC8 = u4IO32Read4B(DRAMC_REG_DQSOSCR);
    backupReg0xD0 = u4IO32Read4B(DRAMC_REG_DUMMY_RD);

    //Disable MR4 MR18/MR19, TxHWTracking, Dummy RD before reset
    vIO32WriteFldAlign_All(DRAMC_REG_SPCMDCTRL, 0x1, SPCMDCTRL_REFRDIS);    //MR4 Disable
    vIO32WriteFldMulti_All(DRAMC_REG_DQSOSCR, P_Fld(0x1, DQSOSCR_DQSOSCRDIS)|P_Fld(0x1, DQSOSCR_DQSOSCENDIS));  //MR18, MR19 Disable
    vIO32WriteFldMulti_All(DRAMC_REG_DUMMY_RD, P_Fld(0x0, DUMMY_RD_DUMMY_RD_EN)
                                            | P_Fld(0x0, DUMMY_RD_SREF_DMYRD_EN)
                                            | P_Fld(0x0, DUMMY_RD_DQSG_DMYRD_EN)
                                            | P_Fld(0x0, DUMMY_RD_DMY_RD_DBG));
    mcDELAY_US(4);

    // Everest change reset order : reset DQS before DQ, move PHY reset to final.
    {
        // Everest change : must reset all dramC and PHY together.
        vIO32WriteFldAlign_All(DRAMC_REG_DDRCONF0, 1, DDRCONF0_DMSW_RST);
        vIO32WriteFldAlign_All(DRAMC_REG_DDRCONF0, 1, DDRCONF0_RDATRST);// read data counter reset
        vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL1, 1, MISC_CTRL1_R_DMPHYRST);

        //RG_ARCMD_RESETB & RG_ARDQ_RESETB_B0/1 only reset once at init, Justin Chan.
        vIO32WriteFldMulti_All(DDRPHY_B0_DQ3, P_Fld(0, B0_DQ3_RG_RX_ARDQS0_STBEN_RESETB) |P_Fld(0, B0_DQ3_RG_RX_ARDQ_STBEN_RESETB_B0));
        vIO32WriteFldMulti_All(DDRPHY_B1_DQ3, P_Fld(0, B1_DQ3_RG_RX_ARDQS1_STBEN_RESETB) |P_Fld(0, B1_DQ3_RG_RX_ARDQ_STBEN_RESETB_B1));
        mcDELAY_US(1);//delay 10ns
        vIO32WriteFldMulti_All(DDRPHY_B1_DQ3, P_Fld(1, B1_DQ3_RG_RX_ARDQS1_STBEN_RESETB) |P_Fld(1, B1_DQ3_RG_RX_ARDQ_STBEN_RESETB_B1));
        vIO32WriteFldMulti_All(DDRPHY_B0_DQ3, P_Fld(1, B0_DQ3_RG_RX_ARDQS0_STBEN_RESETB) |P_Fld(1, B0_DQ3_RG_RX_ARDQ_STBEN_RESETB_B0));

        vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL1, 0, MISC_CTRL1_R_DMPHYRST);
        vIO32WriteFldAlign_All(DRAMC_REG_DDRCONF0, 0, DDRCONF0_RDATRST);// read data counter reset
        vIO32WriteFldAlign_All(DRAMC_REG_DDRCONF0, 0, DDRCONF0_DMSW_RST);
    }

    //Restore backup regs
    vIO32Write4B(DRAMC_REG_SPCMDCTRL, backupReg0x64);
    vIO32Write4B(DRAMC_REG_DQSOSCR, backupReg0xC8);
    vIO32Write4B(DRAMC_REG_DUMMY_RD, backupReg0xD0);
}


void DramEyeStbenReset(DRAMC_CTX_T *p)
{
    vIO32WriteFldAlign_Phy_All(DDRPHY_B0_DQ5, 0, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0);
    vIO32WriteFldAlign_Phy_All(DDRPHY_B1_DQ5, 0, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1);
    vIO32WriteFldAlign_Phy_All(DDRPHY_B1_DQ5, 0, CA_CMD5_RG_RX_ARCMD_EYE_STBEN_RESETB); //only in LP3 due to DQ pinmux to CA

    mcDELAY_US(1);//delay 10ns

    vIO32WriteFldAlign_Phy_All(DDRPHY_B0_DQ5, 1, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0);
    vIO32WriteFldAlign_Phy_All(DDRPHY_B1_DQ5, 1, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1);
    vIO32WriteFldAlign_Phy_All(DDRPHY_B1_DQ5, 1, CA_CMD5_RG_RX_ARCMD_EYE_STBEN_RESETB);//only in LP3 due to DQ pinmux to CA
}
DRAM_STATUS_T DramcRankSwap(DRAMC_CTX_T *p, U8 u1Rank)
{
    U8 u1Multi;

    if (p->support_rank_num > 1)
        u1Multi = 1;
    else
        u1Multi = 0;

    mcSHOW_DBG_MSG2(("[DramcRankSwap] Rank number %d, (u1Multi %d), Rank %d\n", p->support_rank_num, u1Multi, u1Rank));

    //Set to non-zero for multi-rank
    vIO32WriteFldAlign(DRAMC_REG_RKCFG, (u1Rank<<3) | u1Multi, RKCFG_RKMODE);

    if (u1Rank == 0)
    {
        vIO32WriteFldAlign(DRAMC_REG_RKCFG, 0, RKCFG_TXRANKFIX);
    }
    else
    {
        vIO32WriteFldAlign(DRAMC_REG_RKCFG, 1, RKCFG_TXRANKFIX);
    }
    vIO32WriteFldAlign(DRAMC_REG_RKCFG, u1Rank, RKCFG_TXRANK); //use other rank's setting

    return DRAM_OK;
}

//-------------------------------------------------------------------------
/** DramcSwImpedanceCal
 *  start TX OCD impedance calibration.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @param  apply           (U8): 0 don't apply the register we set  1 apply the register we set ,default don't apply.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------
#if SIMULATION_SW_IMPED
void DramcSwImpedanceSaveRegister(DRAMC_CTX_T *p)
{
     U8 val = 2;

     //DQ
     vIO32WriteFldAlign(DRAMC_REG_SHU1_DRVING2, (gDramcSwImpedanceResule[0]<<5) | gDramcSwImpedanceResule[1], SHU1_DRVING2_DQDRV1);

     //DQS
     vIO32WriteFldAlign(DRAMC_REG_SHU1_DRVING1, (gDramcSwImpedanceResule[0]<<5) | gDramcSwImpedanceResule[1], SHU1_DRVING1_DQSDRV1);

     //CMD
     vIO32WriteFldAlign_All(DRAMC_REG_SHU1_DRVING2, (gDramcSwImpedanceResule[0]<<5) | gDramcSwImpedanceResule[1], SHU1_DRVING2_CMDDRV1);

     //CLK
     vIO32WriteFldAlign_All(DRAMC_REG_SHU1_DRVING2, (gDramcSwImpedanceResule[0]<<5) | gDramcSwImpedanceResule[1], SHU1_DRVING2_CMDDRV2);

    if (p->dram_type== TYPE_LPDDR2)
    {
        mcSHOW_DBG_MSG(("ODTP/N=%d\n", val));
        vIO32WriteFldAlign_All(DRAMC_REG_SHU1_DRVING3, (val<<5) | val, SHU1_DRVING3_DQODT2);
        vIO32WriteFldAlign_All(DRAMC_REG_SHU1_DRVING4, (val<<5) | val, SHU1_DRVING4_DQODT1);
        vIO32WriteFldMulti(DRAMC_REG_SHU_ODTCTRL, P_Fld(1, SHU_ODTCTRL_RODTE) | P_Fld(1, SHU_ODTCTRL_RODTE2) | \
			                                P_Fld(2, SHU_ODTCTRL_RODT) | P_Fld(1, SHU_ODTCTRL_ROEN));
    }
}

DRAM_STATUS_T DramcSwImpedanceCal(DRAMC_CTX_T *p)
{
	U32 u4ImpxDrv, u4ImpCalResult;
	U32 u4DRVP_Result =0xff, u4DRVN_Result =0xff;
	//U32 u4BaklReg_DDRPHY_MISC_IMP_CTRL0, u4BaklReg_DDRPHY_MISC_IMP_CTRL1, u4BaklReg_DRAMC_REG_IMPCAL;

	vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL1, 0x0, MISC_SPM_CTRL1_PHY_SPM_CTL1);
	vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL2, 0x0, MISC_SPM_CTRL2_PHY_SPM_CTL2);
	vIO32WriteFldAlign(DDRPHY_MISC_SPM_CTRL0, 0x0, MISC_SPM_CTRL0_PHY_SPM_CTL0);

	//Register backup
	//u4BaklReg_DDRPHY_MISC_IMP_CTRL0 = u4IO32Read4B((DDRPHY_MISC_IMP_CTRL0));
	//u4BaklReg_DDRPHY_MISC_IMP_CTRL1 = u4IO32Read4B((DDRPHY_MISC_IMP_CTRL1));
	//u4BaklReg_DRAMC_REG_IMPCAL = u4IO32Read4B((DRAMC_REG_IMPCAL));

	//Disable IMP HW Tracking
	vIO32WriteFldAlign_All(DRAMC_REG_IMPCAL, 0, IMPCAL_IMPCAL_HW);

	//RG_IMPCAL_VREF_SEL=6'h0f
	//RG_IMPCAL_LP3_EN=0, RG_IMPCAL_LP4_EN=1
	//RG_IMPCAL_ODT_EN=0

	vIO32WriteFldMulti(DDRPHY_MISC_IMP_CTRL1, P_Fld(0, MISC_IMP_CTRL1_RG_RIMP_PRE_EN));
	vIO32WriteFldMulti(DRAMC_REG_IMPCAL, P_Fld(0, IMPCAL_IMPCAL_CALI_ENN) | P_Fld(1, IMPCAL_IMPCAL_IMPPDP) | \
						P_Fld(1, IMPCAL_IMPCAL_IMPPDN));	//RG_RIMP_BIAS_EN and RG_RIMP_VREF_EN move to IMPPDP and IMPPDN, ODT_EN move to CALI_ENN

	vIO32WriteFldMulti(DDRPHY_MISC_IMP_CTRL0, P_Fld(1, MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL) | \
						P_Fld(0, MISC_IMP_CTRL0_RG_RIMP_DDR4_SEL) | \
						P_Fld(0x2e, MISC_IMP_CTRL0_RG_RIMP_VREF_SEL));

	mcSHOW_DBG_MSG2(("0x%X=0x%X\n", DDRPHY_MISC_IMP_CTRL1, u4IO32Read4B(DDRPHY_MISC_IMP_CTRL1)));
	mcSHOW_DBG_MSG2(("0x%X=0x%X\n", DDRPHY_MISC_IMP_CTRL0, u4IO32Read4B(DDRPHY_MISC_IMP_CTRL0)));

	mcDELAY_US(1);

	// K pull up
	mcSHOW_DBG_MSG2(("======= K DRVP=====================\n"));

	//PUCMP_EN=1
	//ODT_EN=0
	vIO32WriteFldAlign(DRAMC_REG_IMPCAL, 1, IMPCAL_IMPCAL_CALI_EN);

	vIO32WriteFldAlign(DRAMC_REG_IMPCAL, 1, IMPCAL_IMPCAL_CALI_ENP);  //PUCMP_EN move to CALI_ENP

	vIO32WriteFldAlign(DRAMC_REG_IMPCAL, 0, IMPCAL_IMPCAL_CALI_ENN);  //ODT_EN move to CALI_ENN

	//DRVP=0
	//DRV05=1
	vIO32WriteFldMulti(DRAMC_REG_SHU_IMPCAL1, P_Fld(0, SHU_IMPCAL1_IMPDRVN)|P_Fld(0, SHU_IMPCAL1_IMPDRVP));

	vIO32WriteFldMulti(DDRPHY_MISC_IMP_CTRL1, P_Fld(1, MISC_IMP_CTRL1_RG_RIMP_REV));  //DRV05=1

	//OCDP Flow
	//If RGS_TX_OCD_IMPCALOUTX=0
	//RG_IMPX_DRVP++;
	//Else save and keep RG_IMPX_DRVP value, and assign to DRVP
	for(u4ImpxDrv=0; u4ImpxDrv<16; u4ImpxDrv++)
	{
		vIO32WriteFldAlign(DRAMC_REG_SHU_IMPCAL1, u4ImpxDrv, SHU_IMPCAL1_IMPDRVP);
		mcDELAY_US(1);
		u4ImpCalResult = u4IO32ReadFldAlign((DDRPHY_MISC_PHY_RGS1), MISC_PHY_RGS1_RGS_RIMPCALOUT);
		mcSHOW_DBG_MSG2(("1. OCD DRVP=%d CALOUT=%d\n", u4ImpxDrv, u4ImpCalResult));

		if((u4ImpCalResult ==1) && (u4DRVP_Result == 0xff))//first found
		{
			u4DRVP_Result = u4ImpxDrv;
			mcSHOW_DBG_MSG2(("1. OCD DRVP calibration OK! DRVP=%d\n\n", u4DRVP_Result));
			break;
		}
	}

	//LP3: DRVN calibration
	mcSHOW_DBG_MSG2(("======= K DRVN=====================\n"));
	//PUCMP_EN=0
	vIO32WriteFldAlign(DRAMC_REG_IMPCAL, 0, IMPCAL_IMPCAL_CALI_ENP);  //PUCMP_EN move to CALI_ENP

	//DRVP=DRVP_FINAL
	//DRVN=0
	//DRV05=1
	vIO32WriteFldMulti(DRAMC_REG_SHU_IMPCAL1, P_Fld(u4DRVP_Result, SHU_IMPCAL1_IMPDRVP) | P_Fld(0, SHU_IMPCAL1_IMPDRVN));

	vIO32WriteFldMulti(DDRPHY_MISC_IMP_CTRL1, P_Fld(1, MISC_IMP_CTRL1_RG_RIMP_REV));  //DRV05=1

	//If RGS_TX_OCD_IMPCALOUTX=1
	//RG_IMPX_DRVN++;
	//Else save RG_IMPX_DRVN value and assign to DRVN
	for(u4ImpxDrv=0; u4ImpxDrv<16 ; u4ImpxDrv++)
	{
		vIO32WriteFldAlign(DRAMC_REG_SHU_IMPCAL1, u4ImpxDrv, SHU_IMPCAL1_IMPDRVN);
		mcDELAY_US(1);
		u4ImpCalResult = u4IO32ReadFldAlign((DDRPHY_MISC_PHY_RGS1), MISC_PHY_RGS1_RGS_RIMPCALOUT);
		mcSHOW_DBG_MSG2(("3. OCD DRVN=%d ,CALOUT=%d\n", u4ImpxDrv, u4ImpCalResult));

		if((u4ImpCalResult ==0) &&(u4DRVN_Result == 0xff))//first found
		{
			u4DRVN_Result = u4ImpxDrv;
			mcSHOW_DBG_MSG2(("3. OCD DRVN calibration OK! ODTN=%d\n\n", u4DRVN_Result));
			break;
		}
	}

	//Register Restore
	//vIO32Write4B((DRAMC_REG_IMPCAL), u4BaklReg_DRAMC_REG_IMPCAL);
	//vIO32Write4B((DDRPHY_MISC_IMP_CTRL0), u4BaklReg_DDRPHY_MISC_IMP_CTRL0);
	//vIO32Write4B((DDRPHY_MISC_IMP_CTRL1), u4BaklReg_DDRPHY_MISC_IMP_CTRL1);

	if(u4DRVN_Result==0xff || u4DRVP_Result==0xff)
	{
		//MT8167 default value??
		u4DRVP_Result = 0xb;
		u4DRVN_Result = 0xb;
	}

	gDramcSwImpedanceResule[0] = (u4DRVP_Result<=3) ? u4DRVP_Result*3 : u4DRVP_Result;
	gDramcSwImpedanceResule[1] = (u4DRVN_Result<=3) ? u4DRVN_Result*3 : u4DRVN_Result;

	DramcSwImpedanceSaveRegister(p);

	mcSHOW_DBG_MSG(("\n[DramcSwImpedanceCal] FINAL: DRVP=%d, DRVN=%d\n\n",gDramcSwImpedanceResule[0], gDramcSwImpedanceResule[1]));

	vSetCalibrationResult(p, DRAM_CALIBRATION_SW_IMPEDANCE, DRAM_OK);
	mcSHOW_DBG_MSG2(("[DramcSwImpedanceCal] Done \n\n"));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    gDRAM_CALIB_LOG.RANK[p->rank].SwImpedanceCal.DRVP = gDramcSwImpedanceResule[0];
    gDRAM_CALIB_LOG.RANK[p->rank].SwImpedanceCal.DRVN = gDramcSwImpedanceResule[1];
#endif

	return DRAM_OK;
}
#endif //SIMULATION_SW_IMPED

U32 Another_Rank_CKE_low_backup_another_reg0x24;
U8  Another_Rank_CKE_low_backup_rank;
U8  Another_Rank_CKE_low_another_rank;
void Another_Rank_CKE_low(DRAMC_CTX_T *p, U8 on_off)
{
#if DUAL_RANK_ENABLE
    if (on_off == 0)
    {
        //another rank CKE low
        Another_Rank_CKE_low_backup_rank = u1GetRank(p);
        Another_Rank_CKE_low_another_rank = (Another_Rank_CKE_low_backup_rank == RANK_0) ? RANK_1 : RANK_0;
        vSetRank(p, Another_Rank_CKE_low_another_rank);
        Another_Rank_CKE_low_backup_another_reg0x24 = u4IO32Read4B(DRAMC_REG_CKECTRL);
        vIO32WriteFldAlign(DRAMC_REG_CKECTRL, 1, CKECTRL_CKEFIXOFF);
        vSetRank(p, Another_Rank_CKE_low_backup_rank);
    }
    else
    {   //restore original value
        vSetRank(p, Another_Rank_CKE_low_another_rank);
        vIO32Write4B(DRAMC_REG_CKECTRL, Another_Rank_CKE_low_backup_another_reg0x24);
        vSetRank(p, Another_Rank_CKE_low_backup_rank);
    }
#endif

}

void O1PathOnOff(DRAMC_CTX_T *p, U8 u1OnOff)
{
    if(u1OnOff)
    {
        vIO32WriteFldAlign_Phy_All(DDRPHY_B0_DQ5, 1, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0);
        vIO32WriteFldAlign_Phy_All(DDRPHY_B1_DQ5, 1, B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1);
        vIO32WriteFldAlign_Phy_All(DDRPHY_CA_CMD5, 1, CA_CMD5_RG_RX_ARCMD_EYE_VREF_EN);
        vIO32WriteFldAlign_Phy_All(DDRPHY_B0_DQ3, 1, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
        vIO32WriteFldAlign_Phy_All(DDRPHY_B1_DQ3, 1, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);
        vIO32WriteFldAlign_Phy_All(DDRPHY_CA_CMD3, 1, CA_CMD3_RG_RX_ARCMD_SMT_EN);
        mcDELAY_US(1);
    }
    else
    {
        vIO32WriteFldAlign_Phy_All(DDRPHY_B0_DQ5, 0, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0);
        vIO32WriteFldAlign_Phy_All(DDRPHY_B1_DQ5, 0, B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1);
        vIO32WriteFldAlign_Phy_All(DDRPHY_CA_CMD5, 0, CA_CMD5_RG_RX_ARCMD_EYE_VREF_EN);
        vIO32WriteFldAlign_Phy_All(DDRPHY_B0_DQ3, 0, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
        vIO32WriteFldAlign_Phy_All(DDRPHY_B1_DQ3, 0, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);
        vIO32WriteFldAlign_Phy_All(DDRPHY_CA_CMD3, 0, CA_CMD3_RG_RX_ARCMD_SMT_EN);
    }
}

// LPDDR DQ -> PHY DQ mapping
// in A60501, it's 1-1 mapping
const U32 uiDDR_PHY_Mapping[PIN_MUX_TYPE_MAX][32] = {
    //uiPCDDR3_PHY_Mapping_X16X2
    {
        26, 31, 27, 30, 29, 25, 28, 24,
        20, 19, 22, 16, 21, 18, 23, 17,
        9, 15, 10, 12, 8, 14, 11, 13,
        7, 3, 6, 1, 4, 0, 5, 2
    },
    //uiPCDDR4_PHY_Mapping_X16X2
    {
        26, 31, 27, 29, 25, 30, 24, 28,
        18, 22, 19, 23, 16, 20, 17, 21,
        9, 15, 11, 12, 10, 13, 8, 14,
        2, 6, 1, 4, 3, 5, 0, 7
    },
    //uiLPDDR3_PHY_Mapping_178BALL
    {
        21, 23, 18, 17, 22, 20, 19, 16,
        6, 7, 3, 1, 5, 4, 2, 0,
        31, 29, 25, 27, 30, 28, 24, 26,
        15, 14, 8, 10, 12, 13, 11, 9
    },
    //uiLPDDR3_PHY_Mapping_168BALL
    {
        26, 25, 27, 24, 19, 16, 17, 18,
        14, 5, 7, 4, 13, 6, 12, 15,
        23, 22, 30, 20, 28, 29, 31, 21,
        0, 10, 9, 8, 3, 2, 11, 1
    },
    //uiPCDDR3_PHY_Mapping_X8
    {
        31, 23, 30, 20, 28, 22, 29, 21,
        17, 27, 18, 25, 16, 24, 19, 26,
        14, 5, 13, 7, 15, 6, 12, 4,
        3, 10, 1, 8, 0, 11, 2, 9
    },
    //uiLPDDR3_PHY_Mapping_211BALL  Tank2
    {
        25, 24, 27, 26, 17, 18, 19, 16,
        7, 6, 5, 4, 15, 14, 12, 13,
        21, 23, 20, 30, 31, 22, 28, 29,
        11, 8, 3, 9, 10, 1, 0, 2
    },
    //uiLPDDR3_PHY_Mapping_LPDDR3_eMCP_1   kry_v1
    {
        25, 24, 27, 18, 26, 16, 17, 19,
        7, 6, 4, 5, 15, 12, 14, 13,
        22, 20, 21, 31, 28, 30, 29, 23,
        3, 8, 1, 0, 11, 2, 9, 10
    },
        //uiLPDDR3_PHY_Mapping_eMCP_MMD
    {
        25, 24, 27, 26, 18, 17, 16, 19,
        7, 6, 5, 4, 15, 14, 12, 13,
        21, 23, 20, 30, 31, 22, 28, 29,
        11, 8, 3, 9, 10, 1, 0, 2
    },
    //uiPCDDR3_PHY_Mapping_X4X2
    {
        29, 27, 31, 25, 28, 30, 26, 24,
        11, 15, 13, 9, 8, 12, 10, 14,
        29, 27, 31, 25, 28, 30, 26, 24,
        11, 15, 13, 9, 8, 12, 10, 14
    },
};
//for 4bitMux
unsigned char Bit_DQ_Mapping[32] =
{
    0, 1, 2, 3, 8, 9, 10, 11,
    4, 5, 6, 7, 12, 13, 14, 15,
    16, 17, 18, 19, 24, 25, 26, 27,
    20, 21, 22, 23, 28, 29, 30, 31
};
unsigned char DQS_Mapping[4] = {0, 1, 2, 3};
unsigned char DQM_Mapping[4] = {0, 1, 2, 3};

#if SIMULATION_LP3_CA_TRAINING
//-------------------------------------------------------------------------
/** DramcCATraining
 *  start the calibrate the skew between Clk pin and CAx pins.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------
#define MAX_CLKO_DELAY         31
#define CATRAINING_NUM        10

DRAM_STATUS_T CATrainingEntry(DRAMC_CTX_T *p, U32 uiMR41, U32 u4GoldenPattern)
{
    //CA_TRAINING_BEGIN:

    // CS extent enable (need DRAM to support)
    // for testing
    vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 1, CATRAINING1_CATRAINCSEXT);

    // CKE high, CKE must be driven HIGH prior to issuance of the MRW41 and MRW48 command
    vIO32WriteFldMulti(DRAMC_REG_CKECTRL, P_Fld(0, CKECTRL_CKEFIXOFF)|P_Fld(1, CKECTRL_CKEFIXON));

    // Enter MR 41/MR48
    // Set MA & OP.
    if (uiMR41)
    {
    	 vIO32WriteFldMulti(DRAMC_REG_MRS, P_Fld(0xa4, MRS_MRSOP)|P_Fld(0,MRS_MRSBA)|P_Fld(41, MRS_MRSMA));
    }
    else
    {
        vIO32WriteFldMulti(DRAMC_REG_MRS, P_Fld(0xc0, MRS_MRSOP)|P_Fld(0,MRS_MRSBA)|P_Fld(48, MRS_MRSMA));
    }
    // Hold the CA bus stable for at least one cycle.
    vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 1, CATRAINING1_CATRAINMRS);

    // MRW
    vIO32WriteFldAlign(DRAMC_REG_SPCMD, 1, SPCMD_MRWEN);
    mcDELAY_US(1);
    vIO32WriteFldAlign(DRAMC_REG_SPCMD, 0, SPCMD_MRWEN);

    // Disable CA bus stable.
    vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 0, CATRAINING1_CATRAINMRS);

    // Wait tCACKEL(10 tck) before CKE low
    mcDELAY_US(1);

    // CKE low
    vIO32WriteFldMulti(DRAMC_REG_CKECTRL, P_Fld(1, CKECTRL_CKEFIXOFF)|P_Fld(0,CKECTRL_CKEFIXON));

    // Set CA0~CA3, CA5~CA8 rising/falling golden value.
    vIO32Write4B(DRAMC_REG_CATRAINING2, u4GoldenPattern);

    // Wait tCAENT(10 tck) before pattern output
    mcDELAY_US(1);

    return DRAM_OK;
}


static S32 CheckCATrainingTransition(U32 uiCA, U32 capattern, U32 uiRisingEdge, U32 uiFallingEdge)
{
    S32 iPass=0;
    U32 iii;

    if (capattern == 0x55555555)
    {
         if ((uiRisingEdge!=0) && (uiFallingEdge==0))
         {
             iPass = 1;
         }
         else
         {
             iPass = 0;
         }
    }
    else if (capattern == 0xaaaaaaaa)
    {
        if ((uiRisingEdge==0) && (uiFallingEdge!=0))
        {
            iPass = 1;
        }
        else
        {
            iPass = 0;
        }
    }
    else if (capattern == 0x99999999)
    {
        iii = uiCA;
        if (iii>=5) iii-=5;

        if ((iii & 1) == 0)
        {
            if ((uiRisingEdge!=0) && (uiFallingEdge==0))
            {
                iPass = 1;
            }
            else
            {
                iPass = 0;
            }
        }
        else
        {
            if ((uiRisingEdge==0) && (uiFallingEdge!=0))
            {
                iPass = 1;
            }
            else
            {
                iPass = 0;
            }
        }
    }
    else if (capattern == 0x66666666)
    {
        iii = uiCA;
        if (iii>=5) iii-=5;

        if ((iii & 1) == 0)
        {
            if ((uiRisingEdge==0) && (uiFallingEdge!=0))
            {
                iPass = 1;
            }
            else
            {
                iPass = 0;
            }
        }
        else
        {
            if ((uiRisingEdge!=0) && (uiFallingEdge==0))
            {
                iPass = 1;
            }
            else
            {
                iPass = 0;
            }
        }
    }
    return iPass;
}


void CATrainingExit(DRAMC_CTX_T *p)
{
    vIO32WriteFldMulti(DRAMC_REG_CKECTRL, P_Fld(0, CKECTRL_CKEFIXOFF)|P_Fld(1, CKECTRL_CKEFIXON));
    mcDELAY_US(1);

    // CS extent enable
    // for testing
    vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 1, CATRAINING1_CATRAINCSEXT);

    // MR42 to leave CA training.
    vIO32WriteFldMulti(DRAMC_REG_MRS, P_Fld(0xa8, MRS_MRSOP)|P_Fld(0,MRS_MRSBA)|P_Fld(0x2a, MRS_MRSMA));

    // Hold the CA bus stable for at least one cycle.
    vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 1, CATRAINING1_CATRAINMRS);

    // MRW
    vIO32WriteFldAlign(DRAMC_REG_SPCMD, 1, SPCMD_MRWEN);
    mcDELAY_US(1);
    vIO32WriteFldAlign(DRAMC_REG_SPCMD, 0, SPCMD_MRWEN);

    // Disable the hold the CA bus stable for at least one cycle.
    vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 0, CATRAINING1_CATRAINMRS);

    // CS extent disable
    // for testing
    vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 0, CATRAINING1_CATRAINCSEXT);
}

void CATrainingDelayCompare(DRAMC_CTX_T *p, U32 uiMR41, U32 u4GoldenPattern, S32 *iCenterSum, S32 *iFirstCAPass , S32 *iLastCAPass, S32 *iCenter, U32 delayinecompare_flag)
{
    U32 *uiLPDDR_PHY_Mapping;
//    U32 u4O1Data[DQS_NUMBER];
    U32 u4dq_o1_tmp[DQS_NUMBER];
    U32 uiTemp, uiTemp2, uiCA, uiRisingEdge, uiFallingEdge, uiFinishCount;
    S32 iPass, iDelay, iDelay_Start, iDelay_End;

    uiLPDDR_PHY_Mapping = (U32 *)uiDDR_PHY_Mapping[(U8)(p->pinmux)];

    // Calculate the middle range & max middle.
    mcSHOW_DBG_MSG2(("=========================================\n"));
    mcSHOW_DBG_MSG2(("[CA Training] Frequency=%d, Rank=%d\n", p->frequency, p->rank));
    mcSHOW_DBG_MSG2(("x=Pass window CA(max~min) Clk(min~max) center. \n"));
    if(uiMR41)
    {
        mcSHOW_DBG_MSG2(("y=CA0~CA3, CA5~CA8\n"));
    }
    else//MR48
    {
        mcSHOW_DBG_MSG2(("y=CA4 CA9\n"));
    }
    mcSHOW_DBG_MSG2(("=========================================\n"));

    uiFinishCount = 0;

    if ((delayinecompare_flag & 1) == 0)
    {
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, 0, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);
        iDelay_Start = -(MAX_CLK_PI_DELAY/2);
        iDelay_End = MAX_CA_PI_DELAY;
    }
    else
    {
        iDelay_Start = 0;
        iDelay_End = MAX_CLK_PI_DELAY;
    }

    // Delay clock output delay to do CA training in order to get the pass window.
    for (iDelay= iDelay_Start; iDelay <= iDelay_End; iDelay++)
    {
        if ((delayinecompare_flag & 1) == 0)
        {
            if(iDelay <=0)
            {    //Set CLK delay
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, -iDelay, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK); //clock_PI_Delay
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, (-iDelay+2) > 15 ? 15 : (-iDelay+2), SHU1_R0_CA_CMD9_RG_RK0_ARPI_CS);
            }
            else
            {    // Set CA output delay
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, iDelay, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);    //CA0~9 CA_PI_Delay
            }
        }
        else
        {
            // set CLK PI value
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, iDelay, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK);    //clock_PI_Delay
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, (iDelay+2) > 15 ? 15 : (iDelay+2), SHU1_R0_CA_CMD9_RG_RK0_ARPI_CS); //clock_PI_Delay
        }

        // CA training pattern output enable
        vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 1, CATRAINING1_CATRAINEN);
        // delay 2 DRAM clock cycle
        mcDELAY_US(1);
        vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 0, CATRAINING1_CATRAINEN);

        // Wait tADR(20ns) before CA sampled values available in DQ.
        mcDELAY_US(1);

        // Get DQ value.
        uiTemp = u4IO32Read4B(DDRPHY_MISC_DQO1) & 0xffff;
        uiTemp |= (u4IO32Read4B(DDRPHY_MISC_DQO1+(1<<POS_BANK_NUM)) & 0xffff0000);

        #ifdef FIRST_BRING_UP
        mcSHOW_DBG_MSG2(("delay %d, DQ_O1 0x%x| Pass ", iDelay, uiTemp));
        #else
        mcSHOW_DBG_MSG2(("%d, | ", iDelay));
        #endif

        // Compare with golden value.
        for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
        {
            if(uiMR41 && ((uiCA==4) || (uiCA==9)))  // MR41 is for CA0~3, CA5~8
            {
                continue;
            }
            else if((uiMR41==0) && ((uiCA!=4) && (uiCA!=9)))// MR48 is for CA4, CA8
            {
                continue;
            }

           // if ( (iFirstCAPass[uiCA]==PASS_RANGE_NA) || (iLastCAPass[uiCA]==PASS_RANGE_NA)) //marked fo debug
            {
                if (uiCA<4)   //CA0~3
                {
                    uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[uiCA<<1]);
                    uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA<<1)+1]);
                }
                else if((uiCA==4) || (uiCA==9))
                {
                    uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 0 : 8]);
                    uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 1 : 9]);
                }
                else//CA5~8
                {
                    uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA-1)<<1]);
                    uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[((uiCA-1)<<1)+1]);
                }

                iPass = CheckCATrainingTransition(uiCA, u4GoldenPattern, uiRisingEdge, uiFallingEdge);

                mcSHOW_DBG_MSG2(("%d ", iPass));

                if (iFirstCAPass[uiCA]==PASS_RANGE_NA)
                {
                    if (iPass == 1)
                    {
                        iFirstCAPass[uiCA] = iDelay;
                    }
                }
                else
                {
                    if (iLastCAPass[uiCA]==PASS_RANGE_NA)
                    {
                        if (iPass == 0)
                        {
                            if((iDelay-iFirstCAPass[uiCA]) <5)  // prevent glitch
                            {
                                iFirstCAPass[uiCA]=PASS_RANGE_NA;
                                continue;
                            }

                            uiFinishCount++;
                            iLastCAPass[uiCA] = iDelay-1;

                            iCenter[uiCA] = (iLastCAPass[uiCA] + iFirstCAPass[uiCA]) >>1;

                            *iCenterSum += iCenter[uiCA];

                        }
                        else
                        {
                            if (iDelay==iDelay_End)
                            {
                                uiFinishCount++;
                                iLastCAPass[uiCA] = iDelay;

                                iCenter[uiCA] = (iLastCAPass[uiCA] + iFirstCAPass[uiCA]) >>1;
                                *iCenterSum += iCenter[uiCA];


                            }
                        }
                    }
                }
            }
        }

        // Wait tCACD(22clk) before output CA pattern to DDR again..
        mcDELAY_US(1);

        mcSHOW_DBG_MSG2(("\n"));

        if((uiMR41 && (uiFinishCount==8)) || ((uiMR41==0) && (uiFinishCount==2)))
        {
            mcSHOW_DBG_MSG2(("[CATrainingDelayCompare] Early break, uiMR41=%d, uiFinishCount=%d\n", uiMR41, uiFinishCount));
            break;
        }
    }


    vSetCalibrationResult(p, DRAM_CALIBRATION_CA_TRAIN, DRAM_OK); // set default result OK, udpate status when per bit fail

    if ((delayinecompare_flag & 1) == 1)
    {
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, 0, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK); //clock_PI_Delay
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, 2, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CS);
    }

    for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
    {
        if(uiMR41 && ((uiCA==4) || (uiCA==9)))  // MR41 is for CA0~3, CA5~8
        {
            continue;
        }
        else if((uiMR41==0) && ((uiCA!=4) && (uiCA!=9)))// MR48 is for CA4, CA8
        {
            continue;
        }


        mcSHOW_DBG_MSG(("FINAL: CA%d  (%d~%d) %d\n", uiCA, iFirstCAPass[uiCA], iLastCAPass[uiCA], iCenter[uiCA]));

        if(iLastCAPass[uiCA]==PASS_RANGE_NA)  // no CA window found
        {
            vSetCalibrationResult(p, DRAM_CALIBRATION_CA_TRAIN, DRAM_FAIL);
        }
    }

    // CS extent disable
    // for testing
    vIO32WriteFldAlign(DRAMC_REG_CATRAINING1, 0, CATRAINING1_CATRAINCSEXT);

    // Wait tCACKEN (10ck)
    mcDELAY_US(1);

    if((uiMR41 && (uiFinishCount<8)) || ((uiMR41==0) && (uiFinishCount<2)))
    {
        mcSHOW_ERR_MSG(("[CATrainingDelayCompare] Error: some bits has abnormal window, uiMR41=%d, uiFinishCount=%d\n", uiMR41, uiFinishCount));
    }

}

DRAM_STATUS_T CATrainingLP3(DRAMC_CTX_T *p)
{
    U32 uiMR41;
    U32 uiReg024h;
    U32 uiReg038h;
    U32 uiReg04ch;
    U32 uiReg80ch;
    U32 uiReg810h;
    U32 uiReg064h;
    S32 iFinalCACLK;
    S32 iCenterSum = 0;
    U8 backup_rank,ii;
    S32 iFirstCAPass[CATRAINING_NUM], iLastCAPass[CATRAINING_NUM], iCenter[CATRAINING_NUM];

    //  01010101b -> 10101010b : Golden value = 1001100110011001b=0x9999
    //  11111111b -> 00000000b : Golden value = 0101010101010101b=0x5555
    U32 u4GoldenPattern =0x55555555;
    //U32 u4GoldenPattern =0xaaaaaaaa;
#if SUPPORT_CATRAININT_DELAY_LINE_CALI
    S32 iCA_PerBit_DelayLine[CATRAINING_NUM];
    S32 iCenterMin = 0xff;
#endif

    Another_Rank_CKE_low(p, 0);

    mcSHOW_DBG_MSG(("\n[CATrainingLP3]\n"));

    // Fix ODT off. A60501 disable ODT in the init code. So no need to do the following code.
    // Let MIO_CK always ON.
    uiReg038h=u4IO32Read4B(DRAMC_REG_DRAMC_PD_CTRL);
    uiReg04ch=u4IO32Read4B(DRAMC_REG_REFCTRL0);
    uiReg80ch=u4IO32Read4B(DRAMC_REG_SHU_ACTIM3);
    uiReg810h=u4IO32Read4B(DRAMC_REG_SHU_ACTIM4);
    uiReg064h=u4IO32Read4B(DRAMC_REG_SPCMDCTRL);
    uiReg024h=u4IO32Read4B(DRAMC_REG_CKECTRL);

    // Disable Refresh and save information
    vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 1, REFCTRL0_REFDIS);      //REFDIS=1, disable auto refresh
    vIO32WriteFldAlign(DRAMC_REG_SHU_ACTIM3, 0, SHU_ACTIM3_REFCNT);
    vIO32WriteFldAlign(DRAMC_REG_SHU_ACTIM4, 0, SHU_ACTIM4_REFCNT_FR_CLK);

    vIO32WriteFldAlign(DRAMC_REG_DRAMC_PD_CTRL, 1, DRAMC_PD_CTRL_MIOCKCTRLOFF);   //MIOCKCTRLOFF=1
    vIO32WriteFldAlign(DRAMC_REG_DRAMC_PD_CTRL, 0, DRAMC_PD_CTRL_PHYCLKDYNGEN); // set R_DMPHYCLKDYNGEN=0


    // Step 1.1 : let IO to O1 path valid
    O1PathOnOff(p, 1);

    for (ii=0; ii<CATRAINING_NUM; ii++)
    {
         iLastCAPass[ii] = PASS_RANGE_NA;
         iFirstCAPass[ii] = PASS_RANGE_NA;
         iCenter[ii] = 0;
    }

#if SUPPORT_CATRAININT_DELAY_LINE_CALI
    if (p->frequency >= DDR_DDR1333)
    {
        backup_rank = u1GetRank(p);
        for(ii=p->rank; ii<RANK_MAX; ii++)
        {
            vSetRank(p,ii);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD1, 0, SHU1_R0_CA_CMD1_RK0_TX_ARCMD1_DLY);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), 0, SHU1_R0_CA_CMD0_RK0_TX_ARCA6_DLY);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD1, 0, SHU1_R0_CA_CMD1_RK0_TX_ARCMD2_DLY);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0, 0, SHU1_R0_CA_CMD0_RK0_TX_ARCA0_DLY);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD1+(1<<POS_BANK_NUM), 0, SHU1_R0_CA_CMD1_RK0_TX_ARCMD2_DLY);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), 0, SHU1_R0_CA_CMD0_RK0_TX_ARCA7_DLY);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0, 0, SHU1_R0_CA_CMD0_RK0_TX_ARCA3_DLY);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), 0, SHU1_R0_CA_CMD0_RK0_TX_ARCA3_DLY);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0, 0, SHU1_R0_CA_CMD0_RK0_TX_ARCA4_DLY);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), 0, SHU1_R0_CA_CMD0_RK0_TX_ARCA0_DLY);
        }
        vSetRank(p,backup_rank);
    }
#endif

    // ----- MR41, CA0~3, CA5~8 -------
    uiMR41 = 1;
    CATrainingEntry(p, uiMR41, u4GoldenPattern);  //MR41
    CATrainingDelayCompare(p, uiMR41, u4GoldenPattern, &iCenterSum, iFirstCAPass, iLastCAPass, iCenter, 0);

    // ----- MR48, CA4 and 9 -------
    uiMR41 = 0;
    CATrainingEntry(p, uiMR41, u4GoldenPattern);  //MR48
    CATrainingDelayCompare(p, uiMR41, u4GoldenPattern, &iCenterSum, iFirstCAPass, iLastCAPass, iCenter, 0);

	iFinalCACLK =iCenterSum/10;

#if SUPPORT_CATRAININT_DELAY_LINE_CALI
	if (p->frequency >= DDR_DDR1333)
	{
		for(ii=0; ii<CATRAINING_NUM; ii++)
		{
			if (iCenterMin > iCenter[ii])
				iCenterMin = iCenter[ii];
		}

        iFinalCACLK = iCenterMin;

        mcSHOW_DBG_MSG(("CA per bit delay PI(DLY): 64(%d)\n", p->density));

        for (ii=0; ii<CATRAINING_NUM; ii++)
        {
            iCA_PerBit_DelayLine[ii] = (iCenter[ii] - iCenterMin);
            mcSHOW_DBG_MSG(("%d ", iCA_PerBit_DelayLine[ii]));
        }
        mcSHOW_DBG_MSG(("\n"));

        for (ii=0; ii<CATRAINING_NUM; ii++)
        {
            iCA_PerBit_DelayLine[ii] = (iCA_PerBit_DelayLine[ii] * p->density + 8)/64;
            mcSHOW_DBG_MSG(("%d ", iCA_PerBit_DelayLine[ii]));
        }
        mcSHOW_DBG_MSG(("\n"));

        for(ii=p->rank; ii<RANK_MAX; ii++)
        {
            vSetRank(p,ii);
            mcSHOW_DBG_MSG(("rank%d\n",p->rank));

            if (p->pinmux == PIN_MUX_TYPE_LPDDR3_178BALL)
            {
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD1, iCA_PerBit_DelayLine[0], SHU1_R0_CA_CMD1_RK0_TX_ARCMD1_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[1], SHU1_R0_CA_CMD0_RK0_TX_ARCA6_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD1, iCA_PerBit_DelayLine[2], SHU1_R0_CA_CMD1_RK0_TX_ARCMD2_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0, iCA_PerBit_DelayLine[3], SHU1_R0_CA_CMD0_RK0_TX_ARCA0_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD1+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[4], SHU1_R0_CA_CMD1_RK0_TX_ARCMD2_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[5], SHU1_R0_CA_CMD0_RK0_TX_ARCA7_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0, iCA_PerBit_DelayLine[6], SHU1_R0_CA_CMD0_RK0_TX_ARCA3_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[7], SHU1_R0_CA_CMD0_RK0_TX_ARCA3_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0, iCA_PerBit_DelayLine[8], SHU1_R0_CA_CMD0_RK0_TX_ARCA4_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[9], SHU1_R0_CA_CMD0_RK0_TX_ARCA0_DLY);
            }
            else
            {
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD1, iCA_PerBit_DelayLine[3], SHU1_R0_CA_CMD1_RK0_TX_ARCMD1_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[1], SHU1_R0_CA_CMD0_RK0_TX_ARCA6_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD1, iCA_PerBit_DelayLine[2], SHU1_R0_CA_CMD1_RK0_TX_ARCMD2_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0, iCA_PerBit_DelayLine[4], SHU1_R0_CA_CMD0_RK0_TX_ARCA0_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD1+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[0], SHU1_R0_CA_CMD1_RK0_TX_ARCMD2_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[5], SHU1_R0_CA_CMD0_RK0_TX_ARCA7_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0, iCA_PerBit_DelayLine[7], SHU1_R0_CA_CMD0_RK0_TX_ARCA3_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[6], SHU1_R0_CA_CMD0_RK0_TX_ARCA3_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0, iCA_PerBit_DelayLine[9], SHU1_R0_CA_CMD0_RK0_TX_ARCA4_DLY);
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD0+(1<<POS_BANK_NUM), iCA_PerBit_DelayLine[8], SHU1_R0_CA_CMD0_RK0_TX_ARCA0_DLY);
            }
        }
        vSetRank(p,backup_rank);
	}
#endif
    mcSHOW_DBG_MSG(("=========================================\n"));
    mcSHOW_DBG_MSG(("u4GoldenPattern 0x%X, iFinalCACLKMacro0 = %d\n", u4GoldenPattern, iFinalCACLK));


    backup_rank = u1GetRank(p);

    for(ii=p->rank; ii<RANK_MAX; ii++)
    {
        vSetRank(p,ii);
        mcSHOW_DBG_MSG(("rank%d:\n",p->rank));
        if(iFinalCACLK <0)
        {    //SEt CLK delay
#if CA_TRAIN_RESULT_DO_NOT_MOVE_CLK
              CATrain_ClkDelay =0;  /// should not happen.

              vIO32WriteFldAlign_All(DDRPHY_SHU1_R0_CA_CMD9, 0x10, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);

              vSetCalibrationResult(p, DRAM_CALIBRATION_CA_TRAIN, DRAM_FAIL);
              mcSHOW_ERR_MSG(("Error : CLK delay is not 0 (%d), Set as 0.  CA set as 0x10\n\n", -iFinalCACLK));
              ASSERT(0);
#else
              CATrain_ClkDelay = -iFinalCACLK;

              vIO32WriteFldAlign_All(DDRPHY_SHU1_R0_CA_CMD9, 0, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);

              mcSHOW_DBG_MSG(("Clk Dealy is = %d, CA delay is 0\n", -iFinalCACLK));
#endif
        }
        else
        {    // Set CA output delay
              CATrain_ClkDelay = 0;

              vIO32WriteFldAlign_All(DDRPHY_SHU1_R0_CA_CMD9, iFinalCACLK, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);
              mcSHOW_DBG_MSG(("Macro%d Clk Dealy is 0, CA delay is %d\n", 0, iFinalCACLK));
        }

        // no need to enter self refresh before setting CLK under CA training mode
        vIO32WriteFldAlign_All(DDRPHY_SHU1_R0_CA_CMD9, CATrain_ClkDelay , SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK);
        vIO32WriteFldAlign_All(DDRPHY_SHU1_R0_CA_CMD9, (CATrain_ClkDelay+2) > 15 ? 15 : (CATrain_ClkDelay+2), SHU1_R0_CA_CMD9_RG_RK0_ARPI_CS);
    }
    vSetRank(p,backup_rank);

    mcSHOW_DBG_MSG(("[CATrainingLP3] ====Done====\n\n"));

    CATrainingExit(p);

    // Disable fix DQ input enable.  Disable IO to O1 path
    O1PathOnOff(p, 0);

    // Disable CKE high, back to dynamic
    vIO32WriteFldMulti(DRAMC_REG_CKECTRL, P_Fld(0, CKECTRL_CKEFIXOFF)|P_Fld(0, CKECTRL_CKEFIXON));

    // Restore the registers' values.
    //vIO32WriteFldAlign(DRAMC_REG_WODT, uiReg54h, WODT_WODTFIXOFF);
    vIO32Write4B(DRAMC_REG_DRAMC_PD_CTRL, uiReg038h);
    vIO32Write4B(DRAMC_REG_REFCTRL0, uiReg04ch);
    vIO32Write4B(DRAMC_REG_SHU_ACTIM3, uiReg80ch);
    vIO32Write4B(DRAMC_REG_SHU_ACTIM4, uiReg810h);
    vIO32Write4B(DRAMC_REG_SPCMDCTRL, uiReg064h);
    vIO32Write4B(DRAMC_REG_CKECTRL, uiReg024h);

    Another_Rank_CKE_low(p, 1);

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    for (ii=0; ii<CATRAINING_NUM;ii++)
    {
        gDRAM_CALIB_LOG.RANK[p->rank].CaTraining.WinPerBit[ii].first_pass = iFirstCAPass[ii];
        gDRAM_CALIB_LOG.RANK[p->rank].CaTraining.WinPerBit[ii].last_pass = iLastCAPass[ii];
        gDRAM_CALIB_LOG.RANK[p->rank].CaTraining.WinPerBit[ii].win_center = iCenter[ii];
        gDRAM_CALIB_LOG.RANK[p->rank].CaTraining.WinPerBit[ii].win_size = iLastCAPass[ii] - iFirstCAPass[ii] + 1;
        gDRAM_CALIB_LOG.RANK[p->rank].CaTraining.WinPerBit[ii].left_margin = iFinalCACLK - iFirstCAPass[ii];
        gDRAM_CALIB_LOG.RANK[p->rank].CaTraining.WinPerBit[ii].right_margin = iLastCAPass[ii] - iFinalCACLK;
    }
    gDRAM_CALIB_LOG.RANK[p->rank].CaTraining.CA_delay = (iFinalCACLK<0) ? 0: iFinalCACLK;
    gDRAM_CALIB_LOG.RANK[p->rank].CaTraining.Clk_delay = CATrain_ClkDelay;
    gDRAM_CALIB_LOG.RANK[p->rank].CaTraining.CS_delay = CATrain_ClkDelay+2;
#endif

    return DRAM_OK;
}
#endif //SIMULATION_LP3_CA_TRAINING

//-------------------------------------------------------------------------
/** DramcWriteLeveling
 *  start Write Leveling Calibration.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @param  apply           (U8): 0 don't apply the register we set  1 apply the register we set ,default don't apply.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------
#define WRITE_LEVELING_MOVD_DQS 1//UI

typedef struct _REG_TRANSFER
{
    U32 u4Addr;
    U32 u4Fld;
} REG_TRANSFER_T;


// NOT suitable for Gating delay
static DRAM_STATUS_T ExecuteMoveDramCDelay(DRAMC_CTX_T *p, REG_TRANSFER_T regs[], S8 iShiftUI)
{
    S32 s4HighLevelDelay, s4DelaySum;
    U32 u4Tmp0p5T, u4Tmp2T;
    U8 ucDataRateDivShift;
    DRAM_STATUS_T MoveResult;

    ucDataRateDivShift= 2;

    u4Tmp0p5T = u4IO32ReadFldAlign(regs[0].u4Addr, regs[0].u4Fld) & (~(1<<ucDataRateDivShift));
    u4Tmp2T = u4IO32ReadFldAlign(regs[1].u4Addr, regs[1].u4Fld);
    //mcSHOW_DBG_MSG(("\n[MoveDramC_Orz]  u4Tmp2T:%d,  u4Tmp0p5T: %d,\n",  u4Tmp2T, u4Tmp0p5T));

    s4HighLevelDelay = (u4Tmp2T <<ucDataRateDivShift) + u4Tmp0p5T;
    s4DelaySum = (s4HighLevelDelay + iShiftUI);
    //mcSHOW_DBG_MSG(("\n[MoveDramC_Orz]  s4HighLevelDealy(%d) +  iShiftUI(%d) = %d\n",  s4HighLevelDelay, iShiftUI, s4DelaySum));

    if(s4DelaySum < 0)
    {
        u4Tmp0p5T =0;
        u4Tmp2T=0;
        MoveResult =  DRAM_FAIL;
        //mcSHOW_ERR_MSG(("\n[MoveDramC_Orz]  s4HighLevelDealy(%d) +  iShiftUI(%d) is small than 0!!\n",  s4HighLevelDelay, iShiftUI));
    }
    else
    {
        u4Tmp2T = s4DelaySum >> ucDataRateDivShift;
        u4Tmp0p5T = s4DelaySum - (u4Tmp2T <<ucDataRateDivShift);
        MoveResult = DRAM_OK;
    }

    vIO32WriteFldAlign(regs[0].u4Addr, u4Tmp0p5T, regs[0].u4Fld);
    vIO32WriteFldAlign(regs[1].u4Addr, u4Tmp2T, regs[1].u4Fld);
    //mcSHOW_DBG_MSG(("\n[MoveDramC_Orz]  Final ==> u4Tmp2T:%d,  u4Tmp0p5T: %d,\n",  u4Tmp2T, u4Tmp0p5T));

    return MoveResult;
}

#if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
static void MoveDramC_TX_DQS(DRAMC_CTX_T *p, U8 u1ByteIdx, S8 iShiftUI)
{
    REG_TRANSFER_T TransferReg[2];

    //mcSHOW_DBG_MSG(("\n[MoveDramC_TX_DQS] Byte %d, iShiftUI %d\n", u1ByteIdx, iShiftUI));

    switch(u1ByteIdx)
    {
        case 0:
            // DQS0
            TransferReg[0].u4Addr = DRAMC_REG_SHU_SELPH_DQS1;
            TransferReg[0].u4Fld =SHU_SELPH_DQS1_DLY_DQS0;
            TransferReg[1].u4Addr = DRAMC_REG_SHU_SELPH_DQS0;
            TransferReg[1].u4Fld =SHU_SELPH_DQS0_TXDLY_DQS0;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            break;

        case 1:
            // DQS1
            TransferReg[0].u4Addr = DRAMC_REG_SHU_SELPH_DQS1;
            TransferReg[0].u4Fld =SHU_SELPH_DQS1_DLY_DQS1;
            TransferReg[1].u4Addr = DRAMC_REG_SHU_SELPH_DQS0;
            TransferReg[1].u4Fld =SHU_SELPH_DQS0_TXDLY_DQS1;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            break;

        case 2:
            // DQS2
            TransferReg[0].u4Addr = DRAMC_REG_SHU_SELPH_DQS1;
            TransferReg[0].u4Fld =SHU_SELPH_DQS1_DLY_DQS2;
            TransferReg[1].u4Addr = DRAMC_REG_SHU_SELPH_DQS0;
            TransferReg[1].u4Fld =SHU_SELPH_DQS0_TXDLY_DQS2;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            break;

        case 3:
            // DQS3
            TransferReg[0].u4Addr = DRAMC_REG_SHU_SELPH_DQS1;
            TransferReg[0].u4Fld =SHU_SELPH_DQS1_DLY_DQS3;
            TransferReg[1].u4Addr = DRAMC_REG_SHU_SELPH_DQS0;
            TransferReg[1].u4Fld =SHU_SELPH_DQS0_TXDLY_DQS3;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            break;

            default:
                break;
    }
}

static void MoveDramC_TX_DQS_OEN(DRAMC_CTX_T *p, U8 u1ByteIdx, S8 iShiftUI)
{
    REG_TRANSFER_T TransferReg[2];

    //mcSHOW_DBG_MSG(("\n[MoveDramC_TX_DQS_OEN] Byte %d, iShiftUI %d\n", u1ByteIdx, iShiftUI));

    switch(u1ByteIdx)
    {
        case 0:
            // DQS_OEN_0
            TransferReg[0].u4Addr = DRAMC_REG_SHU_SELPH_DQS1;
            TransferReg[0].u4Fld =SHU_SELPH_DQS1_DLY_OEN_DQS0;
            TransferReg[1].u4Addr = DRAMC_REG_SHU_SELPH_DQS0;
            TransferReg[1].u4Fld =SHU_SELPH_DQS0_TXDLY_OEN_DQS0;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            break;

        case 1:
            // DQS_OEN_1
            TransferReg[0].u4Addr = DRAMC_REG_SHU_SELPH_DQS1;
            TransferReg[0].u4Fld =SHU_SELPH_DQS1_DLY_OEN_DQS1;
            TransferReg[1].u4Addr = DRAMC_REG_SHU_SELPH_DQS0;
            TransferReg[1].u4Fld =SHU_SELPH_DQS0_TXDLY_OEN_DQS1;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            break;

        case 2:
            // DQS_OEN_2
            TransferReg[0].u4Addr = DRAMC_REG_SHU_SELPH_DQS1;
            TransferReg[0].u4Fld =SHU_SELPH_DQS1_DLY_OEN_DQS2;
            TransferReg[1].u4Addr = DRAMC_REG_SHU_SELPH_DQS0;
            TransferReg[1].u4Fld =SHU_SELPH_DQS0_TXDLY_OEN_DQS2;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            break;

        case 3:
            // DQS_OEN_3
            TransferReg[0].u4Addr = DRAMC_REG_SHU_SELPH_DQS1;
            TransferReg[0].u4Fld =SHU_SELPH_DQS1_DLY_OEN_DQS3;
            TransferReg[1].u4Addr = DRAMC_REG_SHU_SELPH_DQS0;
            TransferReg[1].u4Fld =SHU_SELPH_DQS0_TXDLY_OEN_DQS3;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            break;

            default:
                break;
    }
}
#endif


static void MoveDramC_TX_DQ(DRAMC_CTX_T *p, U8 u1ByteIdx, S8 iShiftUI)
{
    REG_TRANSFER_T TransferReg[2];

    //mcSHOW_DBG_MSG(("\n[MoveDramC_TX_DQ] Byte %d, iShiftUI %d\n", u1ByteIdx, iShiftUI));

    switch(u1ByteIdx)
    {
        case 0:
            // DQM0
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ3;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ3_DLY_DQM0;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ1;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ1_TXDLY_DQM0;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQM_OEN_0
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ3;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ3_DLY_OEN_DQM0;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ1;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQ0
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ2;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ2_DLY_DQ0;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ0;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ0_TXDLY_DQ0;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQ_OEN_0
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ2;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ2_DLY_OEN_DQ0;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ0;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
        break;

        case 1:
            // DQM1
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ3;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ3_DLY_DQM1;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ1;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ1_TXDLY_DQM1;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQM_OEN_1
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ3;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ3_DLY_OEN_DQM1;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ1;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQ1
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ2;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ2_DLY_DQ1;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ0;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ0_TXDLY_DQ1;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
             // DQ_OEN_1
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ2;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ2_DLY_OEN_DQ1;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ0;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1;
             ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
        break;

        case 2:
                // DQM2
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ3;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ3_DLY_DQM2;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ1;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ1_TXDLY_DQM2;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQM_OEN_2
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ3;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ3_DLY_OEN_DQM2;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ1;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQ2
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ2;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ2_DLY_DQ2;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ0;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ0_TXDLY_DQ2;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQ_OEN_2
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ2;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ2_DLY_OEN_DQ2;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ0;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
        break;

        case 3:
            // DQM3
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ3;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ3_DLY_DQM3;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ1;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ1_TXDLY_DQM3;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQM_OEN_3
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ3;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ3_DLY_OEN_DQM3;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ1;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQ3
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ2;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ2_DLY_DQ3;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ0;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ0_TXDLY_DQ3;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
            // DQ_OEN_3
            TransferReg[0].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ2;
            TransferReg[0].u4Fld =SHURK0_SELPH_DQ2_DLY_OEN_DQ3;
            TransferReg[1].u4Addr = DRAMC_REG_SHURK0_SELPH_DQ0;
            TransferReg[1].u4Fld =SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3;
            ExecuteMoveDramCDelay(p, TransferReg, iShiftUI);
        break;
    }
}


//for LPDDR3 DQ delay line used
static void Set_RX_DQ_DelayLine_Phy_Byte(DRAMC_CTX_T *p, U8 u1ByteIdx, S8 value[8])
{
    switch(u1ByteIdx)
    {
        case 0: //BYTE0
            //DQ0  & DQ1
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ2, P_Fld(value[0], SHU1_R0_B0_DQ2_RK0_RX_ARDQ0_F_DLY_B0) | P_Fld(value[0], SHU1_R0_B0_DQ2_RK0_RX_ARDQ0_R_DLY_B0) |
                                                      P_Fld(value[1], SHU1_R0_B0_DQ2_RK0_RX_ARDQ1_F_DLY_B0) | P_Fld(value[1], SHU1_R0_B0_DQ2_RK0_RX_ARDQ1_R_DLY_B0) );
            //DQ2  & DQ3
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ3, P_Fld(value[2], SHU1_R0_B0_DQ3_RK0_RX_ARDQ2_F_DLY_B0) | P_Fld(value[2], SHU1_R0_B0_DQ3_RK0_RX_ARDQ2_R_DLY_B0) |
                                                      P_Fld(value[3], SHU1_R0_B0_DQ3_RK0_RX_ARDQ3_F_DLY_B0) | P_Fld(value[3], SHU1_R0_B0_DQ3_RK0_RX_ARDQ3_R_DLY_B0) );
            //DQ4  & DQ5
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ4, P_Fld(value[4], SHU1_R0_B0_DQ4_RK0_RX_ARDQ4_F_DLY_B0) | P_Fld(value[4], SHU1_R0_B0_DQ4_RK0_RX_ARDQ4_R_DLY_B0) |
                                                      P_Fld(value[5], SHU1_R0_B0_DQ4_RK0_RX_ARDQ5_F_DLY_B0) | P_Fld(value[5], SHU1_R0_B0_DQ4_RK0_RX_ARDQ5_R_DLY_B0) );
            //DQ6  & DQ7
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ5, P_Fld(value[6], SHU1_R0_B0_DQ5_RK0_RX_ARDQ6_F_DLY_B0) | P_Fld(value[6], SHU1_R0_B0_DQ5_RK0_RX_ARDQ6_R_DLY_B0) |
                                                      P_Fld(value[7], SHU1_R0_B0_DQ5_RK0_RX_ARDQ7_F_DLY_B0) | P_Fld(value[7], SHU1_R0_B0_DQ5_RK0_RX_ARDQ7_R_DLY_B0) );
        break;
        case 1: //BYTE1
            //DQ0  & DQ1
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ2, P_Fld(value[0], SHU1_R0_B1_DQ2_RK0_RX_ARDQ0_F_DLY_B1) | P_Fld(value[0], SHU1_R0_B1_DQ2_RK0_RX_ARDQ0_R_DLY_B1) |
                                                      P_Fld(value[1], SHU1_R0_B1_DQ2_RK0_RX_ARDQ1_F_DLY_B1) | P_Fld(value[1], SHU1_R0_B1_DQ2_RK0_RX_ARDQ1_R_DLY_B1) );
            //DQ2  & DQ3
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ3, P_Fld(value[2], SHU1_R0_B1_DQ3_RK0_RX_ARDQ2_F_DLY_B1) | P_Fld(value[2], SHU1_R0_B1_DQ3_RK0_RX_ARDQ2_R_DLY_B1) |
                                                      P_Fld(value[3], SHU1_R0_B1_DQ3_RK0_RX_ARDQ3_F_DLY_B1) | P_Fld(value[3], SHU1_R0_B1_DQ3_RK0_RX_ARDQ3_R_DLY_B1) );
            //DQ4  & DQ5
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ4, P_Fld(value[4], SHU1_R0_B1_DQ4_RK0_RX_ARDQ4_F_DLY_B1) | P_Fld(value[4], SHU1_R0_B1_DQ4_RK0_RX_ARDQ4_R_DLY_B1) |
                                                      P_Fld(value[5], SHU1_R0_B1_DQ4_RK0_RX_ARDQ5_F_DLY_B1) | P_Fld(value[5], SHU1_R0_B1_DQ4_RK0_RX_ARDQ5_R_DLY_B1) );
            //DQ6  & DQ7
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ5, P_Fld(value[6], SHU1_R0_B1_DQ5_RK0_RX_ARDQ6_F_DLY_B1) | P_Fld(value[6], SHU1_R0_B1_DQ5_RK0_RX_ARDQ6_R_DLY_B1) |
                                                      P_Fld(value[7], SHU1_R0_B1_DQ5_RK0_RX_ARDQ7_F_DLY_B1) | P_Fld(value[7], SHU1_R0_B1_DQ5_RK0_RX_ARDQ7_R_DLY_B1) );
        break;
        case 2: //BYTE2
            //DQ0  & DQ1
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ2+(1<<POS_BANK_NUM), P_Fld(value[0], SHU1_R0_B0_DQ2_RK0_RX_ARDQ0_F_DLY_B0) | P_Fld(value[0], SHU1_R0_B0_DQ2_RK0_RX_ARDQ0_R_DLY_B0) |
                                                      P_Fld(value[1], SHU1_R0_B0_DQ2_RK0_RX_ARDQ1_F_DLY_B0) | P_Fld(value[1], SHU1_R0_B0_DQ2_RK0_RX_ARDQ1_R_DLY_B0) );
            //DQ2  & DQ3
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ3+(1<<POS_BANK_NUM), P_Fld(value[2], SHU1_R0_B0_DQ3_RK0_RX_ARDQ2_F_DLY_B0) | P_Fld(value[2], SHU1_R0_B0_DQ3_RK0_RX_ARDQ2_R_DLY_B0) |
                                                      P_Fld(value[3], SHU1_R0_B0_DQ3_RK0_RX_ARDQ3_F_DLY_B0) | P_Fld(value[3], SHU1_R0_B0_DQ3_RK0_RX_ARDQ3_R_DLY_B0) );
            //DQ4  & DQ5
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ4+(1<<POS_BANK_NUM), P_Fld(value[4], SHU1_R0_B0_DQ4_RK0_RX_ARDQ4_F_DLY_B0) | P_Fld(value[4], SHU1_R0_B0_DQ4_RK0_RX_ARDQ4_R_DLY_B0) |
                                                      P_Fld(value[5], SHU1_R0_B0_DQ4_RK0_RX_ARDQ5_F_DLY_B0) | P_Fld(value[5], SHU1_R0_B0_DQ4_RK0_RX_ARDQ5_R_DLY_B0) );
            //DQ6  & DQ7
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ5+(1<<POS_BANK_NUM), P_Fld(value[6], SHU1_R0_B0_DQ5_RK0_RX_ARDQ6_F_DLY_B0) | P_Fld(value[6], SHU1_R0_B0_DQ5_RK0_RX_ARDQ6_R_DLY_B0) |
                                                      P_Fld(value[7], SHU1_R0_B0_DQ5_RK0_RX_ARDQ7_F_DLY_B0) | P_Fld(value[7], SHU1_R0_B0_DQ5_RK0_RX_ARDQ7_R_DLY_B0) );
        break;
        case 3: //BYTE3
            //DQ0  & DQ1
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ2+(1<<POS_BANK_NUM), P_Fld(value[0], SHU1_R0_B1_DQ2_RK0_RX_ARDQ0_F_DLY_B1) | P_Fld(value[0], SHU1_R0_B1_DQ2_RK0_RX_ARDQ0_R_DLY_B1) |
                                                      P_Fld(value[1], SHU1_R0_B1_DQ2_RK0_RX_ARDQ1_F_DLY_B1) | P_Fld(value[1], SHU1_R0_B1_DQ2_RK0_RX_ARDQ1_R_DLY_B1) );
            //DQ2  & DQ3
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ3+(1<<POS_BANK_NUM), P_Fld(value[2], SHU1_R0_B1_DQ3_RK0_RX_ARDQ2_F_DLY_B1) | P_Fld(value[2], SHU1_R0_B1_DQ3_RK0_RX_ARDQ2_R_DLY_B1) |
                                                      P_Fld(value[3], SHU1_R0_B1_DQ3_RK0_RX_ARDQ3_F_DLY_B1) | P_Fld(value[3], SHU1_R0_B1_DQ3_RK0_RX_ARDQ3_R_DLY_B1) );
            //DQ4  & DQ5
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ4+(1<<POS_BANK_NUM), P_Fld(value[4], SHU1_R0_B1_DQ4_RK0_RX_ARDQ4_F_DLY_B1) | P_Fld(value[4], SHU1_R0_B1_DQ4_RK0_RX_ARDQ4_R_DLY_B1) |
                                                      P_Fld(value[5], SHU1_R0_B1_DQ4_RK0_RX_ARDQ5_F_DLY_B1) | P_Fld(value[5], SHU1_R0_B1_DQ4_RK0_RX_ARDQ5_R_DLY_B1) );
            //DQ6  & DQ7
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ5+(1<<POS_BANK_NUM), P_Fld(value[6], SHU1_R0_B1_DQ5_RK0_RX_ARDQ6_F_DLY_B1) | P_Fld(value[6], SHU1_R0_B1_DQ5_RK0_RX_ARDQ6_R_DLY_B1) |
                                                      P_Fld(value[7], SHU1_R0_B1_DQ5_RK0_RX_ARDQ7_F_DLY_B1) | P_Fld(value[7], SHU1_R0_B1_DQ5_RK0_RX_ARDQ7_R_DLY_B1) );
        break;
    }
}


//for LPDDR3 DQM delay line used
static void Set_RX_DQM_DelayLine_Phy_Byte(DRAMC_CTX_T *p, U8 u1ByteIdx, S8 value)
{
    switch(u1ByteIdx)
    {
        case 0:
            //DQM0
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ6, P_Fld(value,SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0)
            				| P_Fld(value, SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_F_DLY_B0));
        break;
        case 1:
            //DQM1
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ6, P_Fld(value,SHU1_R0_B1_DQ6_RK0_RX_ARDQM0_R_DLY_B1)
            				| P_Fld(value, SHU1_R0_B1_DQ6_RK0_RX_ARDQM0_F_DLY_B1));
        break;
        case 2:
            //DQM2
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ6+(1<<POS_BANK_NUM), P_Fld(value,SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0)
            				| P_Fld(value, SHU1_R0_B0_DQ6_RK0_RX_ARDQM0_F_DLY_B0));
        break;
        case 3:
            //DQM3
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ6+(1<<POS_BANK_NUM), P_Fld(value,SHU1_R0_B1_DQ6_RK0_RX_ARDQM0_R_DLY_B1)
            				| P_Fld(value, SHU1_R0_B1_DQ6_RK0_RX_ARDQM0_F_DLY_B1));
        break;
    }
}


#if SIMULATION_WRITE_LEVELING
#if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
static void WriteLevelingMoveDQSInsteadOfCLK(DRAMC_CTX_T *p)
{
    U8 u1ByteIdx, ucbit_num;
    U8 backup_rank, ii;

    backup_rank = u1GetRank(p);

    if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
        ucbit_num = 4;
    else
        ucbit_num = DQS_BIT_NUMBER;
    for(u1ByteIdx =0 ; u1ByteIdx<(p->data_width/ucbit_num); u1ByteIdx++)
    {
        MoveDramC_TX_DQS(p, u1ByteIdx, -WRITE_LEVELING_MOVD_DQS);

        MoveDramC_TX_DQS_OEN(p, u1ByteIdx, -WRITE_LEVELING_MOVD_DQS);

        for(ii=RANK_0; ii<RANK_MAX; ii++)
        {
            vSetRank(p, ii);
            MoveDramC_TX_DQ(p, u1ByteIdx, -WRITE_LEVELING_MOVD_DQS);
        }
        vSetRank(p, backup_rank);
    }
}
#endif


static void vSetDramMRWriteLevelingOnOff(DRAMC_CTX_T *p, U8 u1OnOff)
{
    // MR2 OP[7] to enable/disable write leveling
    if(u1OnOff)
        u1MR2Value |= 0x80;  // OP[7] WR LEV =1
    else
        u1MR2Value &= 0x7f;  // OP[7] WR LEV =0

    if (p->dram_type == TYPE_PCDDR3)
    {
        DramcModeRegWrite_PC3(p, 1, u1OnOff<<7);
    }
    else if (p->dram_type == TYPE_PCDDR4)
    {
        DramcModeRegWrite_PC4(p, 0, 1, DDR_PC4_MR1 | (u1OnOff<<7));
        if(u1OnOff == DISABLE)
        {
            DramcModeRegWrite_PC4(p, 0, 0, 0x310);//BG0, MR0 = 0x310
            mcDELAY_US(100);

            #if 0
            vIO32WriteFldAlign(DRAMC_REG_SHU1_WODT, 0, SHU1_WODT_WODTFIXOFF);
            vIO32WriteFldAlign(DRAMC_REG_SHU1_WODT, 1, SHU1_WODT_WODTFIX);
            #else
            vIO32WriteFldAlign(DRAMC_REG_SHU1_WODT, 1, SHU1_WODT_WODTFIXOFF);
            vIO32WriteFldAlign(DRAMC_REG_SHU1_WODT, 0, SHU1_WODT_WODTFIX);
            #endif
        }
    }
    else //LPDDR3
    {
        DramcModeRegWrite(p, 2, u1MR2Value);
    }
}


DRAM_STATUS_T DramcWriteLeveling(DRAMC_CTX_T *p)
{
    // Note that below procedure is based on "ODT off"
    //U32 *uiLPDDR_PHY_Mapping;
    U32 u4value, u4value1=0, u4dq_o1=0, u4dq_o1_tmp[DQS_NUMBER];
    U32 u4Value2T, u4Value05T;
    U8 byte_i, ucsample_count;
    S32 ii, ClockDelayMax;
    U8 ucsample_status[DQS_NUMBER], ucdq_o1_perbyte[DQS_NUMBER], ucdq_o1_index[DQS_NUMBER];
    U32 u4prv_register_1dc, u4prv_register_044, u4prv_register_0e4, u4prv_register_13c, u4prv_register_008;
    U32 u4prv_register_04c, u4prv_register_064, u4prv_register_038, u4prv_register_0bc, u4prv_register_024;
    DRAM_RANK_T backup_rank;

    S32 wrlevel_dq_delay[DQS_NUMBER];
    S32 wrlevel_dqs_delay[4] = {0,0,0,0};

    #if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
    S32 i4PIBegin, i4PIEnd;
    #endif

    // error handling
    if (!p)
    {
        mcSHOW_ERR_MSG(("context is NULL\n"));
        return DRAM_FAIL;
    }

    Another_Rank_CKE_low(p, 0);

    fgwrlevel_done = 0;
    backup_rank = u1GetRank(p);

    DramcRankSwap(p, p->rank);

    //uiLPDDR_PHY_Mapping = (U32 *)uiDDR_PHY_Mapping[(U8)(p->pinmux)];

    // DQ mapping
    ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    /// Note : uiLPDDR_PHY_Mapping_POP, need to take care mapping in real chip, but not in test chip.
    /// Everest : there is bit swap inside single byte. PHY & DRAM is 1-1 byte mapping, no swap.
    for (byte_i=0; byte_i<(p->data_width/DQS_BIT_NUMBER); byte_i++)
    {
        ucdq_o1_index[byte_i] = byte_i*8;
    }

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =1;
    mcSHOW_DBG_MSG(("\n[REG_ACCESS_PORTING_FUNC]   DramcWriteLeveling\n"));
#endif

    // backup mode settings
    u4prv_register_04c = u4IO32Read4B(DRAMC_REG_REFCTRL0);
    u4prv_register_064 = u4IO32Read4B(DRAMC_REG_SPCMDCTRL);
    u4prv_register_038 = u4IO32Read4B(DRAMC_REG_DRAMC_PD_CTRL);
    u4prv_register_0bc = u4IO32Read4B(DRAMC_REG_WRITE_LEV);
    //CKEONªº­È
    u4prv_register_024 = u4IO32Read4B(DRAMC_REG_CKECTRL);

    //write leveling mode initialization
    //disable auto refresh, REFCNT_FR_CLK = 0 (0x1dc[23:16]), ADVREFEN = 0 (0x44[30]), (CONF2_REFCNT =0)
    vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 1, REFCTRL0_REFDIS);      //REFDIS=1, disable auto refresh
    vIO32WriteFldAlign(DRAMC_REG_DRAMC_PD_CTRL, 1, DRAMC_PD_CTRL_MIOCKCTRLOFF);   //MIOCKCTRLOFF=1
    vIO32WriteFldAlign(DRAMC_REG_DRAMC_PD_CTRL, 0, DRAMC_PD_CTRL_PHYCLKDYNGEN);   //PHYCLKDYNGEN=0

    //Make CKE fixed at 1 (Don't enter power down, Put this before issuing MRS): CKEFIXON = 1
    vIO32WriteFldAlign(DRAMC_REG_CKECTRL, 1, CKECTRL_CKEFIXON); //CKEFIXDON must be operated under MIOCKCTRLOFF = 1


    //ODT, DQIEN fixed at 1; FIXODT = 1 (0xd8[23]), FIXDQIEN = 1111 (0xd8[15:12])
    vIO32WriteFldAlign(DRAMC_REG_SHU1_WODT, 1, SHU1_WODT_WODTFIXOFF);

    //PHY RX Setting for Write Leveling
    //Let IO toO1 path valid, Enable SMT_EN
    O1PathOnOff(p, 1);

    // enable DDR write leveling mode:  issue MR2[7] to enable write leveling (refer to DEFAULT MR2 value)
    vSetDramMRWriteLevelingOnOff(p, ENABLE);

    //wait tWLDQSEN (25 nCK / 25ns) after enabling write leveling mode (DDR3 / LPDDDR3)
    mcDELAY_US(1);

    //Set {R_DQS_B3_G R_DQS_B2_G R_DQS_B1_G R_DQS_B0_G}=1010: 0x13c[4:1] (this depends on sel_ph setting)
    //Enable Write leveling: 0x13c[0]
    //vIO32WriteFldMulti(DRAMC_REG_WRITE_LEVELING, P_Fld(0xa, WRITE_LEVELING_DQSBX_G)|P_Fld(1, WRITE_LEVELING_WRITE_LEVEL_EN));
    vIO32WriteFldMulti(DRAMC_REG_WRITE_LEV, P_Fld(0x1, WRITE_LEV_DQS_B3_G)
                                                | P_Fld(0x0, WRITE_LEV_DQS_B2_G)
                                                | P_Fld(0x1, WRITE_LEV_DQS_B1_G)
                                                | P_Fld(0x0, WRITE_LEV_DQS_B0_G));
    vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, 1, WRITE_LEV_WRITE_LEVEL_EN);
    vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, 1, WRITE_LEV_CBTMASKDQSOE);

    // select DQS
    if (p->data_width == DATA_WIDTH_16BIT)
    {
        u4value = 0x3;//select byte 0.1
    }
    else
    {
        u4value = 0xf;//select byte 0.1.2.3
    }

    vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, u4value, WRITE_LEV_DQS_SEL);

    // wait tWLMRD (40 nCL / 40 ns) before DQS pulse (DDR3 / LPDDR3)
    mcDELAY_US(1);

    //Proceed write leveling...
    //Initilize sw parameters
    ClockDelayMax = MAX_TX_DQSDLY_TAPS;
    for (ii=0; ii < (S32)(p->data_width/DQS_BIT_NUMBER); ii++)
    {
        ucsample_status[ii] = 0;
        wrlevel_dqs_final_delay[ii] = 0;
    }

    //used for WL done status
    // each bit of sample_cnt represents one-byte WL status
    // 1: done or N/A. 0: NOK
    if ((p->data_width == DATA_WIDTH_16BIT))
    {
        ucsample_count = 0xfc;
    }
    else
    {

        ucsample_count = 0xf0;
    }

    mcSHOW_DBG_MSG(("\n[Write Leveling]\n"));
    mcSHOW_DBG_MSG2(("delay  byte0  byte1  byte2  byte3\n"));
    mcSHOW_DBG_MSG2(("-----------------------------\n"));

    #if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
    if(p->arfgWriteLevelingInitShif[p->rank] ==FALSE)
    {
        WriteLevelingMoveDQSInsteadOfCLK(p);
        //p->arfgWriteLevelingInitShif[p->rank] =TRUE;
        p->arfgWriteLevelingInitShif[RANK_0] =TRUE;
        p->arfgWriteLevelingInitShif[RANK_1] =TRUE;
        #if TX_PERBIT_INIT_FLOW_CONTROL
        // both 2 rank use one write leveling result, TX need to udpate.
        p->fgTXPerbifInit[RANK_0]= FALSE;
        p->fgTXPerbifInit[RANK_1]= FALSE;
        #endif

        mcSHOW_DBG_MSG(("WriteLevelingMoveDQSInsteadOfCLK\n"));
    }
    #endif

    // Set DQS output delay to 0
    //MT8167 TX DQS
    vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7, 0, SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte0, DQS delay
    vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7, 0, SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte1, DQS delay
    vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), 0, SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte2, DQS delay
    vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), 0, SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte3, DQS delay

    #if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
    i4PIBegin = WRITE_LEVELING_MOVD_DQS*32 -MAX_CLK_PI_DELAY-1;
    i4PIEnd = i4PIBegin + 64;
    #endif


    #if  WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
    for (ii=i4PIBegin; ii<i4PIEnd; ii++)
    #else
    for (ii=(-MAX_CLK_PI_DELAY); ii<=MAX_TX_DQSDLY_TAPS; ii++)
    #endif
    {
        if (ii <= 0)
        {
            // Adjust Clk output delay.
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, -ii, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK);
        }
        else
        {
            // Adjust DQS output delay.
            // PI (TX DQ/DQS adjust at the same time)
            //MT8167 TX DQS
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7, ii, SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte0, DQS delay
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7, ii, SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte1, DQS delay
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), ii, SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte2, DQS delay
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), ii, SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte3, DQS delay

        }
        //Trigger DQS pulse, R_DQS_WLEV: 0x13c[8] from 1 to 0
        vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, 1, WRITE_LEV_DQS_WLEV);
        vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, 0, WRITE_LEV_DQS_WLEV);


        //wait tWLO (7.5ns / 20ns) before output (DDR3 / LPDDR3)
        mcDELAY_US(1);

        //Read DQ_O1 from register

        // Get DQ value.
        u4dq_o1 = u4IO32Read4B(DDRPHY_MISC_DQO1) & 0xffff;
        u4dq_o1 |= (u4IO32Read4B(DDRPHY_MISC_DQO1+(1<<POS_BANK_NUM)) & 0xffff0000);
        //mcSHOW_DBG_MSG2(("DQ_O1: 0x%x\n", u4dq_o1));

        if (p->en_4bitMux == ENABLE)
        {
            u4dq_o1_tmp[0] = (u4dq_o1 >> 4) & 0xf;
            u4dq_o1_tmp[1] = (u4dq_o1 >> 8) & 0xf;
            u4dq_o1_tmp[2] = (u4dq_o1 >> 20) & 0xf;
            u4dq_o1_tmp[3] = (u4dq_o1 >> 24) & 0xf;
            u4dq_o1 &= (~(0xf << 4) & ~(0xf << 8)  & ~(0xf << 20) & ~(0xf << 24));
            u4dq_o1 |= (u4dq_o1_tmp[0] << 8) | (u4dq_o1_tmp[1] << 4) | (u4dq_o1_tmp[2] << 24) | (u4dq_o1_tmp[3] << 20);
        }

        mcSHOW_DBG_MSG2(("%d    ", ii));

        mcSHOW_DBG_MSG2(("0x%x    ", u4dq_o1));

        for (byte_i = 0; byte_i < (p->data_width/DQS_BIT_NUMBER);  byte_i++)
        {
            ucdq_o1_perbyte[byte_i] = (U8)((u4dq_o1>>ucdq_o1_index[byte_i]) & 0xff);

            mcSHOW_DBG_MSG2(("%x   ", ucdq_o1_perbyte[byte_i]));

            if ((ucsample_status[byte_i]==0) && (ucdq_o1_perbyte[byte_i]==0))
            {
                ucsample_status[byte_i] = 1;
            }
            else if ((ucsample_status[byte_i]>=1) && (ucdq_o1_perbyte[byte_i] ==0))
            {
                ucsample_status[byte_i] = 1;
            }
            else if ((ucsample_status[byte_i]>=1) && (ucdq_o1_perbyte[byte_i] !=0))
            {
                ucsample_status[byte_i]++;
            }
            //mcSHOW_DBG_MSG(("(%x) ", ucsample_status[byte_i]));

            if((ucsample_count &(0x01 << byte_i))==0)// result not found of byte yet
            {
                #if  WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
                if((ucsample_status[byte_i] ==8) || ((ii==i4PIEnd-1) && (ucsample_status[byte_i] >1)))
                #else
                if((ucsample_status[byte_i] ==8) || ((ii==MAX_TX_DQSDLY_TAPS)&& (ucsample_status[byte_i] >1)))
                #endif
                {
                    wrlevel_dqs_final_delay[byte_i] = ii -ucsample_status[byte_i] +2;
                    ucsample_count |= (0x01 << byte_i);
                    //mcSHOW_DBG_MSG(("(record %d) ", wrlevel_dqs_final_delay[byte_i]));
                }
            }
        }
        mcSHOW_DBG_MSG2(("\n"));

        if (ucsample_count == 0xff)
            break;  // all byte found, early break.
    }

    if (ucsample_count == 0xff)
    {
        // all bytes are done
        fgwrlevel_done= 1;
        vSetCalibrationResult(p, DRAM_CALIBRATION_WRITE_LEVEL, DRAM_OK);
    }
    else
    {
        vSetCalibrationResult(p, DRAM_CALIBRATION_WRITE_LEVEL, DRAM_FAIL);
    }

    mcSHOW_DBG_MSG2(("pass bytecount = 0x%x (0xff means all bytes pass) \n\n", ucsample_count));

    for (byte_i = 0; byte_i < (p->data_width/DQS_BIT_NUMBER);  byte_i++)
    {
        if (ClockDelayMax > wrlevel_dqs_final_delay[byte_i])
        {
            ClockDelayMax = wrlevel_dqs_final_delay[byte_i];
        }
    }

    if (ClockDelayMax > 0)
    {
        ClockDelayMax = 0;
    }
    else
    {
        ClockDelayMax = -ClockDelayMax;
    }

    vPrintCalibrationBasicInfo(p);

    mcSHOW_DBG_MSG(("WL Clk delay = %d, CA CLK delay = %d\n", ClockDelayMax, CATrain_ClkDelay));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.Clk_delay = ClockDelayMax;
    gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.CA_delay = CATrain_ClkDelay;
#endif

    // Adjust Clk & CA if needed
    if (CATrain_ClkDelay < ClockDelayMax)
    {
        S32 Diff = ClockDelayMax - CATrain_ClkDelay;
        mcSHOW_DBG_MSG(("CA adjust %d taps... \n", Diff));

        // Write shift value into CA output delay.

        u4value = u4IO32ReadFldAlign(DDRPHY_SHU1_R0_CA_CMD9, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);
        u4value += Diff;

        //default value in init() is 0x10 , vIO32Write4B(mcSET_DDRPHY_REG_ADDR_CHC(0x0458), 0x00100000);   // Partickl
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, u4value, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);

        mcSHOW_DBG_MSG(("[DramcWriteLeveling] Update Macro0 CA PI Delay = %d, Macro1 CA PI Delay = %d\n", u4value, u4value1));

        // Write shift value into CS output delay.

        vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, ClockDelayMax, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CS);
        mcSHOW_DBG_MSG(("[DramcWriteLeveling] Update CS Delay = %d\n", ClockDelayMax));

    }
    else
    {
    	mcSHOW_DBG_MSG(("No need to update CA/CS delay because the CLK delay is small than CA training.\n"));
    	ClockDelayMax = CATrain_ClkDelay;
    }

    //DramcEnterSelfRefresh(p, 1);  //enter self refresh mode when changing CLK
    // Write max center value into Clk output delay.
    vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, ClockDelayMax, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK);
    //DramcEnterSelfRefresh(p, 0);

    mcSHOW_DBG_MSG(("Final Clk output delay = %d\n", ClockDelayMax));
    //mcSHOW_DBG_MSG(("After adjustment...\n"));

    u4Value2T= u4IO32Read4B(DRAMC_REG_SHU_SELPH_DQS0);
    u4Value05T= u4IO32Read4B(DRAMC_REG_SHU_SELPH_DQS1);
    for (byte_i = 0; byte_i < (p->data_width/DQS_BIT_NUMBER);  byte_i++)
    {
        wrlevel_dqs_final_delay[byte_i] += (ClockDelayMax);

        mcSHOW_DBG_MSG(("R%d FINAL: WriteLeveling DQS:(%d, %d) OEN:(%d, %d) ", p->rank, (u4Value2T >> (byte_i*4)) & 0x7, (u4Value05T >> (byte_i*4)) & 0x3, \
			(u4Value2T >> (byte_i*4+16)) & 0x7, (u4Value05T >> (byte_i*4+16)) & 0x3));
        mcSHOW_DBG_MSG(("DQS%d delay =  %d\n", byte_i, wrlevel_dqs_final_delay[byte_i]));
    }

    // write leveling done, mode settings recovery if necessary
    // recover mode registers : issue MR2[7] to disable write leveling (refer to DEFAULT MR2 value)

    vSetDramMRWriteLevelingOnOff(p, DISABLE);

    // restore registers.
    vIO32Write4B(DRAMC_REG_CKECTRL, u4prv_register_024);    //restore CKEFIXON value before MIOCKCTRLOFF restore
    vIO32Write4B(DRAMC_REG_REFCTRL0, u4prv_register_04c);
    vIO32Write4B(DRAMC_REG_SPCMDCTRL, u4prv_register_064);
    vIO32Write4B(DRAMC_REG_DRAMC_PD_CTRL, u4prv_register_038);
    vIO32Write4B(DRAMC_REG_WRITE_LEV, u4prv_register_0bc);

    //Disable DQ_O1, SELO1ASO=0 for power saving
    O1PathOnOff(p, 0);

    for(byte_i=0; byte_i<(p->data_width/DQS_BIT_NUMBER); byte_i++)
    {
        if(wrlevel_dqs_final_delay [byte_i] >= 0x40) //ARPI_PBYTE_B* is 6 bits, max 0x40
        {
            wrlevel_dqs_delay[byte_i] = wrlevel_dqs_final_delay [byte_i] - 0x40;
            MoveDramC_TX_DQS(p, byte_i, 2);
            MoveDramC_TX_DQS_OEN(p, byte_i, 2);
        }
        else
        {
            wrlevel_dqs_delay[byte_i] = wrlevel_dqs_final_delay [byte_i];
        }
    }

    /* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
    for(ii=p->rank; ii<RANK_MAX; ii++)
    {
        vSetRank(p,ii);

        // set to best values for  DQS
        //MT8167 TX DQS
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7, wrlevel_dqs_delay[0], SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte0, DQS delay
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7, wrlevel_dqs_delay[1], SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte1, DQS delay
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), wrlevel_dqs_delay[2], SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);	//rank0, byte2, DQS delay
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), wrlevel_dqs_delay[3], SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);	//rank0, byte3, DQS delay
    }
    vSetRank(p,backup_rank);

    #if 1//EVEREST_CHANGE_OF_PHY_PBYTE
    //Evereest new change, ARPI_DQ_RK0_ARPI_PBYTE_B* only move DQS, not including of DQM&DQ anymore.
    //Add move DQ, DQ= DQS+0x10, after cali.  take care diff. UI. with DQS
    for(byte_i=0; byte_i<(p->data_width/DQS_BIT_NUMBER); byte_i++)
    {
        wrlevel_dq_delay[byte_i] = wrlevel_dqs_final_delay [byte_i] + 0x10;
        if(wrlevel_dq_delay[byte_i] >= 0x40) //ARPI_DQ_B* is 6 bits, max 0x40
        {
            wrlevel_dq_delay[byte_i] -= 0x40;
            MoveDramC_TX_DQ(p, byte_i, 2);
        }
    }

    /* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
    for(ii=p->rank; ii<RANK_MAX; ii++)
    {
        vSetRank(p,ii);
        #if 0
        mcSHOW_DBG_MSG(("rank%d\n", p->rank));
        u4Value2T= u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQ0);
        u4Value05T= u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQ2);
        for (byte_i = 0; byte_i < (p->data_width/DQS_BIT_NUMBER);  byte_i++)
        {
        	mcSHOW_DBG_MSG(("DQ:(%d, %d) OEN:(%d, %d)",(u4Value2T >> (byte_i*4)) & 0x7, (u4Value05T >> (byte_i*4)) & 0x3, \
        		(u4Value2T >> (byte_i*4+16)) & 0x7, (u4Value05T >> (byte_i*4+16)) & 0x3));
        	mcSHOW_DBG_MSG(("DQ%d delay =  %d\n", byte_i, wrlevel_dq_delay[byte_i]));
        }
        u4Value2T= u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQ1);
        u4Value05T= u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQ3);
        for (byte_i = 0; byte_i < (p->data_width/DQS_BIT_NUMBER);  byte_i++)
        {
        	mcSHOW_DBG_MSG(("DQM:(%d, %d) OEN:(%d, %d)", (u4Value2T >> (byte_i*4)) & 0x7, (u4Value05T >> (byte_i*4)) & 0x3, \
        		(u4Value2T >> (byte_i*4+16)) & 0x7, (u4Value05T >> (byte_i*4+16)) & 0x3));
        	mcSHOW_DBG_MSG(("DQM%d delay =  %d\n", byte_i, wrlevel_dq_delay[byte_i]));
        }
        #endif
        // set to best values for  DQ/DQM/DQ_OEN/DQM_OEN
        //MT8167 TX DQ/DQM
        // set to best values for  DQM, DQ
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(wrlevel_dq_delay[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
					P_Fld(wrlevel_dq_delay[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(wrlevel_dq_delay[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
					P_Fld(wrlevel_dq_delay[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(wrlevel_dq_delay[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
					P_Fld(wrlevel_dq_delay[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(wrlevel_dq_delay[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
					P_Fld(wrlevel_dq_delay[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
    }
    vSetRank(p,backup_rank);
    #endif


    DramcRankSwap(p, RANK_0);

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =0;
#endif
    mcSHOW_DBG_MSG(("[DramcWriteLeveling] ====Done====\n"));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.Final_Clk_delay =  ClockDelayMax;
    gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.DQS0_delay = wrlevel_dqs_final_delay[0];
    gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.DQS1_delay = wrlevel_dqs_final_delay[1];
    gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.DQS2_delay = wrlevel_dqs_final_delay[2];
    gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.DQS3_delay = wrlevel_dqs_final_delay[3];
#endif

    Another_Rank_CKE_low(p, 1);

    return DRAM_OK;
}

#if SUPPORT_TYPE_DDR3X4
DRAM_STATUS_T DramcWriteLeveling4Bit(DRAMC_CTX_T *p, U8 u1DlyLine)
{
    // Note that below procedure is based on "ODT off"
    //U32 *uiLPDDR_PHY_Mapping;
    U32 u4value, u4value1=0, u4dq_o1=0, u4dq_o1_tmp[DQS_NUMBER];
    U32 u4Value2T, u4Value05T;
    U8 byte_i, ucsample_count, ucbit_num;
    static U8 ucSkewPI, ucSkewBit;
    S32 ii, ClockDelayMax;
    U8 ucsample_status[DQS_NUMBER], ucdq_o1_perbyte[DQS_NUMBER], ucdq_o1_index[DQS_NUMBER];
    U32 u4prv_register_1dc, u4prv_register_044, u4prv_register_0e4, u4prv_register_13c, u4prv_register_008;
    U32 u4prv_register_04c, u4prv_register_064, u4prv_register_038, u4prv_register_0bc, u4prv_register_024;
    DRAM_RANK_T backup_rank;

    S32 wrlevel_dq_delay[DQS_NUMBER];
    S32 wrlevel_dqs_delay[4] = {0,0,0,0};

    #if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
    S32 i4PIBegin, i4PIEnd;
    #endif

    if (u1DlyLine && ucSkewPI == 0)
    {
        mcSHOW_DBG_MSG(("DDR3X4 DQS1/3 NO SKEW\n"));
        p->density = 0;
        return DRAM_OK;
    }

    Another_Rank_CKE_low(p, 0);

    fgwrlevel_done = 0;
    backup_rank = u1GetRank(p);

    // error handling
    if (!p)
    {
        mcSHOW_ERR_MSG(("context is NULL\n"));
        return DRAM_FAIL;
    }

    DramcRankSwap(p, p->rank);

    if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
        ucbit_num = 4;
    else
        ucbit_num = DQS_BIT_NUMBER;

    //uiLPDDR_PHY_Mapping = (U32 *)uiDDR_PHY_Mapping[(U8)(p->pinmux)];

    // DQ mapping
    ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    /// Note : uiLPDDR_PHY_Mapping_POP, need to take care mapping in real chip, but not in test chip.
    /// there is bit swap inside single byte. PHY & DRAM is 1-1 byte mapping, no swap.
    for (byte_i=0; byte_i<(p->data_width/ucbit_num); byte_i++)
    {
        ucdq_o1_index[byte_i] = byte_i*ucbit_num;
    }

    if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
    {
        //swap B12 & B13
        ucdq_o1_index[2] = 16;
        ucdq_o1_index[3] = 20;
    }

    //backup wrlevel_dqs_final_delay
    if (u1DlyLine)
    {
        memcpy(wrlevel_dqs_delay, wrlevel_dqs_final_delay, sizeof(wrlevel_dqs_delay));
    }

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =1;
    mcSHOW_DBG_MSG(("\n[REG_ACCESS_PORTING_FUNC]   DramcWriteLeveling\n"));
#endif

    // backup mode settings
    u4prv_register_04c = u4IO32Read4B(DRAMC_REG_REFCTRL0);
    u4prv_register_064 = u4IO32Read4B(DRAMC_REG_SPCMDCTRL);
    u4prv_register_038 = u4IO32Read4B(DRAMC_REG_DRAMC_PD_CTRL);
    u4prv_register_0bc = u4IO32Read4B(DRAMC_REG_WRITE_LEV);
    //CKEONªº­È
    u4prv_register_024 = u4IO32Read4B(DRAMC_REG_CKECTRL);

    //write leveling mode initialization
    //disable auto refresh, REFCNT_FR_CLK = 0 (0x1dc[23:16]), ADVREFEN = 0 (0x44[30]), (CONF2_REFCNT =0)
    vIO32WriteFldAlign(DRAMC_REG_REFCTRL0, 1, REFCTRL0_REFDIS);      //REFDIS=1, disable auto refresh
    vIO32WriteFldAlign(DRAMC_REG_DRAMC_PD_CTRL, 1, DRAMC_PD_CTRL_MIOCKCTRLOFF);   //MIOCKCTRLOFF=1
    vIO32WriteFldAlign(DRAMC_REG_DRAMC_PD_CTRL, 0, DRAMC_PD_CTRL_PHYCLKDYNGEN);   //PHYCLKDYNGEN=0

    //Make CKE fixed at 1 (Don't enter power down, Put this before issuing MRS): CKEFIXON = 1
    vIO32WriteFldAlign(DRAMC_REG_CKECTRL, 1, CKECTRL_CKEFIXON); //CKEFIXDON must be operated under MIOCKCTRLOFF = 1


    //ODT, DQIEN fixed at 1; FIXODT = 1 (0xd8[23]), FIXDQIEN = 1111 (0xd8[15:12])
    vIO32WriteFldAlign(DRAMC_REG_SHU1_WODT, 1, SHU1_WODT_WODTFIXOFF);

    //PHY RX Setting for Write Leveling
    //Let IO toO1 path valid, Enable SMT_EN
    O1PathOnOff(p, 1);

    // enable DDR write leveling mode:  issue MR2[7] to enable write leveling (refer to DEFAULT MR2 value)
    vSetDramMRWriteLevelingOnOff(p, ENABLE);

    //wait tWLDQSEN (25 nCK / 25ns) after enabling write leveling mode (DDR3 / LPDDDR3)
    mcDELAY_US(1);

    //Set {R_DQS_B3_G R_DQS_B2_G R_DQS_B1_G R_DQS_B0_G}=1010: 0x13c[4:1] (this depends on sel_ph setting)
    //Enable Write leveling: 0x13c[0]
    //vIO32WriteFldMulti(DRAMC_REG_WRITE_LEVELING, P_Fld(0xa, WRITE_LEVELING_DQSBX_G)|P_Fld(1, WRITE_LEVELING_WRITE_LEVEL_EN));
    vIO32WriteFldMulti(DRAMC_REG_WRITE_LEV, P_Fld(0x1, WRITE_LEV_DQS_B3_G)
                                                | P_Fld(0x0, WRITE_LEV_DQS_B2_G)
                                                | P_Fld(0x1, WRITE_LEV_DQS_B1_G)
                                                | P_Fld(0x0, WRITE_LEV_DQS_B0_G));
    vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, 1, WRITE_LEV_WRITE_LEVEL_EN);
    vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, 1, WRITE_LEV_CBTMASKDQSOE);

    // select DQS
    if (p->data_width == DATA_WIDTH_16BIT && p->pinmux != PIN_MUX_TYPE_DDR3X4)
    {
        u4value = 0x3;//select byte 0.1
    }
    else
    {
        u4value = 0xf;//select byte 0.1.2.3
    }

    vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, u4value, WRITE_LEV_DQS_SEL);

    // wait tWLMRD (40 nCL / 40 ns) before DQS pulse (DDR3 / LPDDR3)
    mcDELAY_US(1);

    //Proceed write leveling...
    //Initilize sw parameters
    ClockDelayMax = MAX_TX_DQSDLY_TAPS;
    for (ii=0; ii < (S32)(p->data_width/ucbit_num); ii++)
    {
        ucsample_status[ii] = 0;
        wrlevel_dqs_final_delay[ii] = 0;
    }

    //used for WL done status
    // each bit of sample_cnt represents one-byte WL status
    // 1: done or N/A. 0: NOK
    if (p->data_width == DATA_WIDTH_16BIT && p->pinmux != PIN_MUX_TYPE_DDR3X4)
    {
        ucsample_count = 0xfc;
    }
    else
    {
        ucsample_count = 0xf0;
    }

    mcSHOW_DBG_MSG(("\n[Write Leveling]\n"));
    mcSHOW_DBG_MSG2(("delay  byte0  byte1  byte2  byte3\n"));
    mcSHOW_DBG_MSG2(("-----------------------------\n"));

    if (!u1DlyLine)
    {
        #if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
        if(p->arfgWriteLevelingInitShif[p->rank] ==FALSE)
        {
            WriteLevelingMoveDQSInsteadOfCLK(p);
            //p->arfgWriteLevelingInitShif[p->rank] =TRUE;
            p->arfgWriteLevelingInitShif[RANK_0] =TRUE;
            p->arfgWriteLevelingInitShif[RANK_1] =TRUE;
            #if TX_PERBIT_INIT_FLOW_CONTROL
            // both 2 rank use one write leveling result, TX need to udpate.
            p->fgTXPerbifInit[RANK_0]= FALSE;
            p->fgTXPerbifInit[RANK_1]= FALSE;
            #endif

            mcSHOW_DBG_MSG(("WriteLevelingMoveDQSInsteadOfCLK\n"));
        }
        #endif

        // Set DQS output delay to 0
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7, 0, SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte0, DQS delay
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7, 0, SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte1, DQS delay
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), 0, SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte2, DQS delay
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), 0, SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte3, DQS delay

        #if WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
        i4PIBegin = WRITE_LEVELING_MOVD_DQS*32 -MAX_CLK_PI_DELAY-1;
        i4PIEnd = i4PIBegin + 64;
        #endif
    }
    else
    {
        i4PIBegin = 0;
        i4PIEnd = 15;
    }

    #if  WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
    for (ii=i4PIBegin; ii<i4PIEnd; ii++)
    #else
    for (ii=(-MAX_CLK_PI_DELAY); ii<=MAX_TX_DQSDLY_TAPS; ii++)
    #endif
    {
        if (!u1DlyLine)
        {
            if (ii <= 0)
            {
                // Adjust Clk output delay.
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, -ii, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK);
            }
            else
            {
                // Adjust DQS output delay.
                // PI (TX DQ/DQS adjust at the same time)
                //MT8167 TX DQS
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7, ii, SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte0, DQS delay
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7, ii, SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte1, DQS delay
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), ii, SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte2, DQS delay
                vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), ii, SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte3, DQS delay

            }
        }
        else
        {
            //Adjust DQS Delay Line
            vIO32WriteFldMulti_All(DDRPHY_SHU1_R0_B0_DQ1, P_Fld(ii, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0B_DLY_B0) | P_Fld(ii, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0_DLY_B0) \
                                                        | P_Fld(ii, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0B_DLYB_B0) | P_Fld(ii, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0_DLYB_B0));
            vIO32WriteFldMulti_All(DDRPHY_SHU1_R0_B1_DQ1, P_Fld(ii, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0B_DLY_B1) | P_Fld(ii, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0_DLY_B1) \
                                                        | P_Fld(ii, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0B_DLYB_B1) | P_Fld(ii, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0_DLYB_B1));
        }
        //Trigger DQS pulse, R_DQS_WLEV: 0x13c[8] from 1 to 0
        vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, 1, WRITE_LEV_DQS_WLEV);
        vIO32WriteFldAlign(DRAMC_REG_WRITE_LEV, 0, WRITE_LEV_DQS_WLEV);


        //wait tWLO (7.5ns / 20ns) before output (DDR3 / LPDDR3)
        mcDELAY_US(1);

        //Read DQ_O1 from register

        // Get DQ value.
        u4dq_o1 = u4IO32Read4B(DDRPHY_MISC_DQO1) & 0xffff;
        u4dq_o1 |= (u4IO32Read4B(DDRPHY_MISC_DQO1+(1<<POS_BANK_NUM)) & 0xffff0000);
        //mcSHOW_DBG_MSG2(("DQ_O1: 0x%x\n", u4dq_o1));

        if (p->en_4bitMux == ENABLE)
        {
            u4dq_o1_tmp[0] = (u4dq_o1 >> 4) & 0xf;
            u4dq_o1_tmp[1] = (u4dq_o1 >> 8) & 0xf;
            u4dq_o1_tmp[2] = (u4dq_o1 >> 20) & 0xf;
            u4dq_o1_tmp[3] = (u4dq_o1 >> 24) & 0xf;
            u4dq_o1 &= (~(0xf << 4) & ~(0xf << 8)  & ~(0xf << 20) & ~(0xf << 24));
            u4dq_o1 |= (u4dq_o1_tmp[0] << 8) | (u4dq_o1_tmp[1] << 4) | (u4dq_o1_tmp[2] << 24) | (u4dq_o1_tmp[3] << 20);
        }

        mcSHOW_DBG_MSG2(("%d    ", ii));

        mcSHOW_DBG_MSG2(("0x%x    ", u4dq_o1));

        for (byte_i = 0; byte_i < (p->data_width/ucbit_num);  byte_i++)
        {
            if (p->pinmux != PIN_MUX_TYPE_DDR3X4)
                ucdq_o1_perbyte[byte_i] = (U8)((u4dq_o1>>ucdq_o1_index[byte_i]) & 0xff);
            else
                ucdq_o1_perbyte[byte_i] = (U8)((u4dq_o1>>ucdq_o1_index[byte_i]) & 0x0f);
            mcSHOW_DBG_MSG2(("%x   ", ucdq_o1_perbyte[byte_i]));

            if ((ucsample_status[byte_i]==0) && ((ucdq_o1_perbyte[byte_i]==0) || (u1DlyLine && ucdq_o1_perbyte[byte_i]!=0)))
            {
                if (u1DlyLine && ucdq_o1_perbyte[byte_i]!=0)
                    ucsample_status[byte_i] = 2;
                else
                    ucsample_status[byte_i] = 1;
            }
            else if ((ucsample_status[byte_i]>=1) && (ucdq_o1_perbyte[byte_i] ==0))
            {
                ucsample_status[byte_i] = 1;
            }
            else if ((ucsample_status[byte_i]>=1) && (ucdq_o1_perbyte[byte_i] !=0))
            {
                ucsample_status[byte_i]++;
            }
            //mcSHOW_DBG_MSG(("(%x) ", ucsample_status[byte_i]));

            if((ucsample_count &(0x01 << byte_i))==0)// result not found of byte yet
            {
                #if  WRITE_LEVELING_MOVE_DQS_INSTEAD_OF_CLK
                if((ucsample_status[byte_i] ==8) || ((ii==i4PIEnd-1) && (ucsample_status[byte_i] >1)))
                #else
                if((ucsample_status[byte_i] ==8) || ((ii==MAX_TX_DQSDLY_TAPS)&& (ucsample_status[byte_i] >1)))
                #endif
                {
                    wrlevel_dqs_final_delay[byte_i] = ii -ucsample_status[byte_i] +2;
                    ucsample_count |= (0x01 << byte_i);
                    //mcSHOW_DBG_MSG(("(record %d) ", wrlevel_dqs_final_delay[byte_i]));
                }
            }
        }
        mcSHOW_DBG_MSG2(("\n"));

        if (ucsample_count == 0xff)
            break;  // all byte found, early break.
        else if (u1DlyLine && (ucsample_count>>ucSkewBit) & 1)
            break; //K Dly Line need only check one skew bit
    }

    if ((ucsample_count == 0xff) || (u1DlyLine && (ucsample_count>>ucSkewBit) & 1))
    {
        // all bytes are done
        fgwrlevel_done= 1;
        vSetCalibrationResult(p, DRAM_CALIBRATION_WRITE_LEVEL, DRAM_OK);
    }
    else
    {
        vSetCalibrationResult(p, DRAM_CALIBRATION_WRITE_LEVEL, DRAM_FAIL);
    }

    mcSHOW_DBG_MSG2(("pass bytecount = 0x%x (0xff means all bytes pass) \n\n", ucsample_count));

    if (!u1DlyLine)
    {
        if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
        {
            //first PI K, dqs will be set the smallest, the larger PI bit needs to K Dly Line
            if (wrlevel_dqs_final_delay[1] > wrlevel_dqs_final_delay[3])
            {
                ucSkewPI = wrlevel_dqs_final_delay[1] - wrlevel_dqs_final_delay[3];
                ucSkewBit = 1;
                wrlevel_dqs_final_delay[1] = wrlevel_dqs_final_delay[3];
            }
            else
            {
                ucSkewPI = wrlevel_dqs_final_delay[3] - wrlevel_dqs_final_delay[1];
                ucSkewBit = 3;
                wrlevel_dqs_final_delay[3] = wrlevel_dqs_final_delay[1];
            }

            mcSHOW_DBG_MSG(("DDR3X4 (ucSkewBit)DQS%d-DQS%d = (ucSkewPI)%d\n", ucSkewBit, 4-ucSkewBit, ucSkewPI));
        }

        for (byte_i = 0; byte_i < (p->data_width/ucbit_num);  byte_i++)
        {
            if (ClockDelayMax > wrlevel_dqs_final_delay[byte_i])
            {
                ClockDelayMax = wrlevel_dqs_final_delay[byte_i];
            }
        }

        if (ClockDelayMax > 0)
        {
            ClockDelayMax = 0;
        }
        else
        {
            ClockDelayMax = -ClockDelayMax;
        }

        vPrintCalibrationBasicInfo(p);

        mcSHOW_DBG_MSG(("WL Clk delay = %d, CA CLK delay = %d\n", ClockDelayMax, CATrain_ClkDelay));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
        gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.Clk_delay = ClockDelayMax;
        gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.CA_delay = CATrain_ClkDelay;
#endif

        // Adjust Clk & CA if needed
        if (CATrain_ClkDelay < ClockDelayMax)
        {
            S32 Diff = ClockDelayMax - CATrain_ClkDelay;
            mcSHOW_DBG_MSG(("CA adjust %d taps... \n", Diff));

            // Write shift value into CA output delay.

            u4value = u4IO32ReadFldAlign(DDRPHY_SHU1_R0_CA_CMD9, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);
            u4value += Diff;

            //default value in init() is 0x10 , vIO32Write4B(mcSET_DDRPHY_REG_ADDR_CHC(0x0458), 0x00100000);   // Partickl
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, u4value, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD);

            mcSHOW_DBG_MSG(("[DramcWriteLeveling] Update Macro0 CA PI Delay = %d, Macro1 CA PI Delay = %d\n", u4value, u4value1));

            // Write shift value into CS output delay.

            vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, ClockDelayMax, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CS);
            mcSHOW_DBG_MSG(("[DramcWriteLeveling] Update CS Delay = %d\n", ClockDelayMax));

        }
        else
        {
            mcSHOW_DBG_MSG(("No need to update CA/CS delay because the CLK delay is small than CA training.\n"));
            ClockDelayMax = CATrain_ClkDelay;
        }

        //DramcEnterSelfRefresh(p, 1);  //enter self refresh mode when changing CLK
        // Write max center value into Clk output delay.
        vIO32WriteFldAlign(DDRPHY_SHU1_R0_CA_CMD9, ClockDelayMax, SHU1_R0_CA_CMD9_RG_RK0_ARPI_CLK);
        //DramcEnterSelfRefresh(p, 0);

        mcSHOW_DBG_MSG(("Final Clk output delay = %d\n", ClockDelayMax));
        //mcSHOW_DBG_MSG(("After adjustment...\n"));

        u4Value2T= u4IO32Read4B(DRAMC_REG_SHU_SELPH_DQS0);
        u4Value05T= u4IO32Read4B(DRAMC_REG_SHU_SELPH_DQS1);
        for (byte_i = 0; byte_i < (p->data_width/ucbit_num);  byte_i++)
        {
            wrlevel_dqs_final_delay[byte_i] += (ClockDelayMax);

            mcSHOW_DBG_MSG(("R%d FINAL: WriteLeveling DQS:(%d, %d) OEN:(%d, %d) ", p->rank, (u4Value2T >> (byte_i*4)) & 0x7, (u4Value05T >> (byte_i*4)) & 0x3, \
                (u4Value2T >> (byte_i*4+16)) & 0x7, (u4Value05T >> (byte_i*4+16)) & 0x3));
            mcSHOW_DBG_MSG(("DQS%d delay =  %d\n", byte_i, wrlevel_dqs_final_delay[byte_i]));
        }
    }
    // write leveling done, mode settings recovery if necessary
    // recover mode registers : issue MR2[7] to disable write leveling (refer to DEFAULT MR2 value)

    vSetDramMRWriteLevelingOnOff(p, DISABLE);

    // restore registers.
    vIO32Write4B(DRAMC_REG_CKECTRL, u4prv_register_024);    //restore CKEFIXON value before MIOCKCTRLOFF restore
    vIO32Write4B(DRAMC_REG_REFCTRL0, u4prv_register_04c);
    vIO32Write4B(DRAMC_REG_SPCMDCTRL, u4prv_register_064);
    vIO32Write4B(DRAMC_REG_DRAMC_PD_CTRL, u4prv_register_038);
    vIO32Write4B(DRAMC_REG_WRITE_LEV, u4prv_register_0bc);

    //Disable DQ_O1, SELO1ASO=0 for power saving
    O1PathOnOff(p, 0);

    if (!u1DlyLine)
    {
        for(byte_i=0; byte_i<(p->data_width/ucbit_num); byte_i++)
        {
            if(wrlevel_dqs_final_delay [byte_i] >= 0x40) //ARPI_PBYTE_B* is 6 bits, max 0x40
            {
                wrlevel_dqs_delay[byte_i] = wrlevel_dqs_final_delay [byte_i] - 0x40;
                MoveDramC_TX_DQS(p, byte_i, 2);
                MoveDramC_TX_DQS_OEN(p, byte_i, 2);
            }
            else
            {
                wrlevel_dqs_delay[byte_i] = wrlevel_dqs_final_delay [byte_i];
            }
        }

        /* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
        for(ii=p->rank; ii<RANK_MAX; ii++)
        {
            vSetRank(p,ii);

            // set to best values for  DQS
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7, wrlevel_dqs_delay[0], SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);  //rank0, byte0, DQS delay
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7, wrlevel_dqs_delay[1], SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);  //rank0, byte1, DQS delay
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), wrlevel_dqs_delay[2], SHU1_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);	//rank0, byte2, DQS delay
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), wrlevel_dqs_delay[3], SHU1_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);	//rank0, byte3, DQS delay
        }
        vSetRank(p,backup_rank);

        #if 1//EVEREST_CHANGE_OF_PHY_PBYTE
        //Evereest new change, ARPI_DQ_RK0_ARPI_PBYTE_B* only move DQS, not including of DQM&DQ anymore.
        //Add move DQ, DQ= DQS+0x10, after cali.  take care diff. UI. with DQS
        for(byte_i=0; byte_i<(p->data_width/ucbit_num); byte_i++)
        {
            wrlevel_dq_delay[byte_i] = wrlevel_dqs_final_delay [byte_i] + 0x10;
            if(wrlevel_dq_delay[byte_i] >= 0x40) //ARPI_DQ_B* is 6 bits, max 0x40
            {
                wrlevel_dq_delay[byte_i] -= 0x40;
                MoveDramC_TX_DQ(p, byte_i, 2);
            }
        }

        /* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
        for(ii=p->rank; ii<RANK_MAX; ii++)
        {
            vSetRank(p,ii);
            #if 0
            mcSHOW_DBG_MSG(("rank%d\n", p->rank));
            u4Value2T= u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQ0);
            u4Value05T= u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQ2);
            for (byte_i = 0; byte_i < (p->data_width/DQS_BIT_NUMBER);  byte_i++)
            {
            	mcSHOW_DBG_MSG(("DQ:(%d, %d) OEN:(%d, %d)",(u4Value2T >> (byte_i*4)) & 0x7, (u4Value05T >> (byte_i*4)) & 0x3, \
            		(u4Value2T >> (byte_i*4+16)) & 0x7, (u4Value05T >> (byte_i*4+16)) & 0x3));
            	mcSHOW_DBG_MSG(("DQ%d delay =  %d\n", byte_i, wrlevel_dq_delay[byte_i]));
            }
            u4Value2T= u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQ1);
            u4Value05T= u4IO32Read4B(DRAMC_REG_SHURK0_SELPH_DQ3);
            for (byte_i = 0; byte_i < (p->data_width/DQS_BIT_NUMBER);  byte_i++)
            {
            	mcSHOW_DBG_MSG(("DQM:(%d, %d) OEN:(%d, %d)", (u4Value2T >> (byte_i*4)) & 0x7, (u4Value05T >> (byte_i*4)) & 0x3, \
            		(u4Value2T >> (byte_i*4+16)) & 0x7, (u4Value05T >> (byte_i*4+16)) & 0x3));
            	mcSHOW_DBG_MSG(("DQM%d delay =  %d\n", byte_i, wrlevel_dq_delay[byte_i]));
            }
            #endif
            // set to best values for  DQ/DQM/DQ_OEN/DQM_OEN
            //MT8167 TX DQ/DQM
            // set to best values for  DQM, DQ
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(wrlevel_dq_delay[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
    					P_Fld(wrlevel_dq_delay[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(wrlevel_dq_delay[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
    					P_Fld(wrlevel_dq_delay[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(wrlevel_dq_delay[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
    					P_Fld(wrlevel_dq_delay[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(wrlevel_dq_delay[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
    					P_Fld(wrlevel_dq_delay[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
        }
        vSetRank(p,backup_rank);
        #endif
    }
    else
    {
        for (byte_i = 0; byte_i < (p->data_width/ucbit_num);  byte_i++)
        {
            mcSHOW_DBG_MSG(("DQS%d delay =  %d\n", byte_i, wrlevel_dqs_final_delay[byte_i]));
        }

        if (wrlevel_dqs_final_delay[ucSkewBit] == 0)
            p->density = 0;
        else
            p->density = wrlevel_dqs_final_delay[ucSkewBit] * 1000 / ucSkewPI;

        mcSHOW_DBG_MSG(("PI:DLY = %d:%d (%d)\n", ucSkewPI, wrlevel_dqs_final_delay[ucSkewBit], p->density));
        //Adjust DQS Delay Line
        for(ii=p->rank; ii<RANK_MAX; ii++)
        {
            //Adjust DQS Delay Line
            vIO32WriteFldMulti_All(DDRPHY_SHU1_R0_B0_DQ1, P_Fld(0, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0B_DLY_B0) | P_Fld(0, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0_DLY_B0) \
                                                        | P_Fld(0, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0B_DLYB_B0) | P_Fld(0, SHU1_R0_B0_DQ1_RK0_TX_ARDQS0_DLYB_B0));
            vIO32WriteFldMulti_All(DDRPHY_SHU1_R0_B1_DQ1, P_Fld(wrlevel_dqs_final_delay[ucSkewBit]>>1, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0B_DLY_B1) | P_Fld(wrlevel_dqs_final_delay[ucSkewBit]>>1, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0_DLY_B1) \
                                                        | P_Fld(wrlevel_dqs_final_delay[ucSkewBit]>>1, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0B_DLYB_B1) | P_Fld(wrlevel_dqs_final_delay[ucSkewBit]>>1, SHU1_R0_B1_DQ1_RK0_TX_ARDQS0_DLYB_B1));
            vSetRank(p,ii);
        }
        vSetRank(p,backup_rank);

        //restore wrlevel_dqs_final_delay
        memcpy(wrlevel_dqs_final_delay, wrlevel_dqs_delay, sizeof(wrlevel_dqs_final_delay));
    }

    DramcRankSwap(p, RANK_0);

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =0;
#endif
    mcSHOW_DBG_MSG(("[DramcWriteLeveling] ====Done====\n"));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    if (!u1DlyLine)
    {
        gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.Final_Clk_delay =  ClockDelayMax;
        gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.DQS0_delay = wrlevel_dqs_final_delay[0];
        gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.DQS1_delay = wrlevel_dqs_final_delay[1];
        gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.DQS2_delay = wrlevel_dqs_final_delay[2];
        gDRAM_CALIB_LOG.RANK[p->rank].Write_Leveling.DQS3_delay = wrlevel_dqs_final_delay[3];
    }
#endif

    Another_Rank_CKE_low(p, 1);

    return DRAM_OK;
}
#endif
#endif //SIMULATION_WRITE_LEVELING


#if SIMULATION_GATING
#define GATING_RODT_LATANCY_EN 0  //Need to enable when RODT enable
#define GATING_PATTERN_NUM_LP3 0x46
#define GATING_GOLDEND_DQSCNT_LP3 0x2323

#if GATING_ADJUST_TXDLY_FOR_TRACKING
U8 u1TXDLY_Cal_min =0xff, u1TXDLY_Cal_max=0;
U8 ucbest_coarse_tune2T_backup[RANK_MAX][DQS_NUMBER];
U8 ucbest_coarse_tune0p5T_backup[RANK_MAX][DQS_NUMBER];
U8 ucbest_coarse_tune2T_P1_backup[RANK_MAX][DQS_NUMBER];
U8 ucbest_coarse_tune0p5T_P1_backup[RANK_MAX][DQS_NUMBER];
#endif

// Use gating old burst mode to find gating window boundary
// Set the begining of window as new burst mode gating window center.

DRAM_STATUS_T DramcRxdqsGatingCal(DRAMC_CTX_T *p)
{
        U8 ucRX_DLY_DQSIENSTB_LOOP,ucRX_DQS_CTL_LOOP;
        U32 u4value, u4err_value;
        U8 ucpass_begin[DQS_NUMBER], ucpass_count[DQS_NUMBER], ucCurrentPass;
        U8 ucmin_coarse_tune2T[DQS_NUMBER], ucmin_coarse_tune0p5T[DQS_NUMBER], ucmin_fine_tune[DQS_NUMBER];
        U8 ucpass_count_1[DQS_NUMBER], ucmin_coarse_tune2T_1[DQS_NUMBER], ucmin_coarse_tune0p5T_1[DQS_NUMBER], ucmin_fine_tune_1[DQS_NUMBER];
        U8 dqs_i,  ucdly_coarse_large, ucdly_coarse_0p5T, ucdly_fine_xT, ucDQS_GW_FINE_STEP;
        U8 uctmp_offset, uctmp_value;
        U8 ucbest_fine_tune[DQS_NUMBER], ucbest_coarse_tune0p5T[DQS_NUMBER], ucbest_coarse_tune2T[DQS_NUMBER];
        U8 ucbest_fine_tune_P1[DQS_NUMBER], ucbest_coarse_tune0p5T_P1[DQS_NUMBER], ucbest_coarse_tune2T_P1[DQS_NUMBER];

        U8 ucFreqDiv;
        U8 ucdly_coarse_large_P1, ucdly_coarse_0p5T_P1;

    #if GATING_ADJUST_TXDLY_FOR_TRACKING
        U8 u1TX_dly_DQSgated;
    #endif

    #if GATING_RODT_LATANCY_EN
        U8 ucdly_coarse_large_RODT, ucdly_coarse_0p5T_RODT;
        U8 ucdly_coarse_large_RODT_P1, ucdly_coarse_0p5T_RODT_P1;  //Elbrus new
        U8 ucbest_coarse_large_RODT[DQS_NUMBER], ucbest_coarse_0p5T_RODT[DQS_NUMBER];
        U8 ucbest_coarse_large_RODT_P1[DQS_NUMBER], ucbest_coarse_0p5T_RODT_P1[DQS_NUMBER];//Elbrus new
    #endif
        U8 ucCoarseTune, ucCoarseStart, ucCoarseEnd;
        U32 u4DebugCnt[DQS_NUMBER];
        U16 u2DebugCntPerByte;

        U32 u4BakReg_DRAMC_DQSCAL0, u4BakReg_DRAMC_STBCAL_F;
        U32 u4BakReg_DRAMC_WODT, u4BakReg_DRAMC_SPCMD, u4BakReg_DRAMC_REFCTRL0;
        U8 u1PassByteCount=0;

#ifdef ENABLE_CALIBRATION_WINDOW_LOG_FOR_FT
        U16 u2MinWinSize = 0xffff;
        U8 u1MinWinSizeByteidx;
#endif
        // error handling
        if (!p)
        {
            mcSHOW_ERR_MSG(("context is NULL\n"));
            return DRAM_FAIL;
        }

        //Register backup
        u4BakReg_DRAMC_DQSCAL0 = u4IO32Read4B(DRAMC_REG_STBCAL);
        u4BakReg_DRAMC_STBCAL_F = u4IO32Read4B(DRAMC_REG_STBCAL1);
        u4BakReg_DRAMC_WODT = u4IO32Read4B(DRAMC_REG_DDRCONF0);
        u4BakReg_DRAMC_SPCMD = u4IO32Read4B(DRAMC_REG_SPCMD);
        u4BakReg_DRAMC_REFCTRL0 = u4IO32Read4B(DRAMC_REG_REFCTRL0);

        //Disable perbank refresh, use all bank refresh, currently DUT problem
        vIO32WriteFldAlign(DRAMC_REG_REFCTRL0,  0, REFCTRL0_PBREFEN);

        // Disable HW gating first, 0x1c0[31], need to disable both UI and PI tracking or the gating delay reg won't be valid.
        DramcHWGatingOnOff(p, 0);

        //If DQS ring counter is different as our expectation, error flag is asserted and the status is in ddrphycfg 0xFC0 ~ 0xFCC
        //Enable this function by R_DMSTBENCMPEN=1 (0x348[18])
        //Set R_DMSTBCNT_LATCH_EN=1, 0x348[11]
        //Set R_DM4TO1MODE=0, 0x54[11]
        //Clear error flag by ddrphycfg 0x5c0[1] R_DMPHYRST
        vIO32WriteFldAlign(DRAMC_REG_STBCAL1, 1, STBCAL1_STBENCMPEN);
        vIO32WriteFldAlign(DRAMC_REG_STBCAL1, 1, STBCAL1_STBCNT_LATCH_EN);
        vIO32WriteFldAlign(DRAMC_REG_DDRCONF0, 0, DDRCONF0_DM4TO1MODE);

        //enable &reset DQS counter
        vIO32WriteFldAlign(DRAMC_REG_SPCMD, 1, SPCMD_DQSGCNTEN);
        mcDELAY_US(4);//wait 1 auto refresh after DQS Counter enable

        vIO32WriteFldAlign(DRAMC_REG_SPCMD, 1, SPCMD_DQSGCNTRST);
        mcDELAY_US(1);//delay 2T
        vIO32WriteFldAlign(DRAMC_REG_SPCMD, 0, SPCMD_DQSGCNTRST);

        vIO32WriteFldAlign_All(DDRPHY_MISC_CTRL1, u1GetRank(p), MISC_CTRL1_R_DMSTBENCMP_RK_OPT);

        //Initialize variables
        for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++)
        {
            ucpass_begin[dqs_i] = 0;
            ucpass_count[dqs_i] = 0;
        }

        ucRX_DLY_DQSIENSTB_LOOP= 32;// PI fine tune 0->31

        mcSHOW_DBG_MSG(("\n[Gating]\n"));
        vPrintCalibrationBasicInfo(p);

        ucFreqDiv= 2;
        ucDQS_GW_FINE_STEP = DQS_GW_FINE_STEP;

        if(p->frequency >= DDR_DDR1333)     //1333
            ucCoarseStart = 5;
        else //if(p->frequency >= DDR_DDR1066)  //1066
            ucCoarseStart = 2;

        ucCoarseEnd = ucCoarseStart+24;

        ucRX_DQS_CTL_LOOP = 8; // Since Everest, no matter LP3 or LP4. ucRX_DQS_CTL_LOOP is 8.

    #if GATING_RODT_LATANCY_EN  //LP3 RODT is not enable, don't need to set the RODT settings.
        // Fix build warning, initialize variables.
        ucdly_coarse_large_RODT = 0;
        ucdly_coarse_0p5T_RODT = 0;

        ucdly_coarse_large_RODT_P1 = 2;
        ucdly_coarse_0p5T_RODT_P1 = 2;

        // 1.   DQSG latency =
        // (1)   R_DMR*DQSINCTL[3:0] (MCK) +
        // (2)   selph_TX_DLY[2:0] (MCK) +
        // (3)   selph_dly[2:0] (UI)

        // 2.   RODT latency =
        // (1)   R_DMTRODT[3:0] (MCK) +
        // (2)   selph_TX_DLY[2:0] (MCK) +
        // (3)   selph_dly[2:0] (UI)

    #endif

        for (ucCoarseTune = ucCoarseStart; ucCoarseTune < ucCoarseEnd; ucCoarseTune += DQS_GW_COARSE_STEP)
        {
            ucdly_coarse_large      = ucCoarseTune / ucRX_DQS_CTL_LOOP;
            ucdly_coarse_0p5T      = ucCoarseTune % ucRX_DQS_CTL_LOOP;

            ucdly_coarse_large_P1 = (ucCoarseTune + ucFreqDiv) / ucRX_DQS_CTL_LOOP;
            ucdly_coarse_0p5T_P1 =(ucCoarseTune + ucFreqDiv) % ucRX_DQS_CTL_LOOP;

            //DramPhyCGReset(p, 1);// need to reset when UI update or PI change >=2

            // 4T or 2T coarse tune
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, \
                                        P_Fld((U32) ucdly_coarse_large, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED)| \
                                        P_Fld((U32) ucdly_coarse_large, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)| \
                                        P_Fld((U32) ucdly_coarse_large, SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED)| \
                                        P_Fld((U32) ucdly_coarse_large, SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED)| \
                                        P_Fld((U32) ucdly_coarse_large_P1, SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)| \
                                        P_Fld((U32) ucdly_coarse_large_P1, SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)| \
                                        P_Fld((U32) ucdly_coarse_large_P1, SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1)| \
                                        P_Fld((U32) ucdly_coarse_large_P1, SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1));

            // 0.5T coarse tune
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, \
                                        P_Fld((U32) ucdly_coarse_0p5T, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED)| \
                                        P_Fld((U32) ucdly_coarse_0p5T, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)| \
                                        P_Fld((U32) ucdly_coarse_0p5T, SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED)| \
                                        P_Fld((U32) ucdly_coarse_0p5T, SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED)| \
                                        P_Fld((U32) ucdly_coarse_0p5T_P1, SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)| \
                                        P_Fld((U32) ucdly_coarse_0p5T_P1, SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)| \
                                        P_Fld((U32) ucdly_coarse_0p5T_P1, SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1)| \
                                        P_Fld((U32) ucdly_coarse_0p5T_P1, SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1));

            for (ucdly_fine_xT=DQS_GW_FINE_START; ucdly_fine_xT<DQS_GW_FINE_END; ucdly_fine_xT+=ucDQS_GW_FINE_STEP)
            {
                //ok we set a coarse/fine tune value already
                u4value = ucdly_fine_xT | (ucdly_fine_xT<<8) | (ucdly_fine_xT<<16) | (ucdly_fine_xT<<24);
                vIO32Write4B(DRAMC_REG_SHURK0_DQSIEN, u4value);

                //reset phy, reset read data counter
                DramPhyReset(p);

                //reset DQS counter
                vIO32WriteFldAlign(DRAMC_REG_SPCMD, 1, SPCMD_DQSGCNTRST);
                mcDELAY_US(1);//delay 2T
                vIO32WriteFldAlign(DRAMC_REG_SPCMD, 0, SPCMD_DQSGCNTRST);

                // enable TE2, audio pattern
                DramcEngine2(p, TE_OP_READ_CHECK, 0x55000000, 0xaa000000 |GATING_PATTERN_NUM_LP3, 1, 0, 0, 0);


                 //read DQS counter
                u4DebugCnt[0] = u4IO32Read4B(DRAMC_REG_DQSGNWCNT0);
                u4DebugCnt[1] = (u4DebugCnt[0] >> 16) & 0xffff;
                u4DebugCnt[0] &= 0xffff;

                if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
                {
                    u4DebugCnt[1] = u4IO32Read4B(DRAMC_REG_DQSGNWCNT1) & 0xffff;
                }

                if(p->data_width == DATA_WIDTH_32BIT)
                {
                    u4DebugCnt[2] = u4IO32Read4B(DRAMC_REG_DQSGNWCNT1);
                    u4DebugCnt[3] = (u4DebugCnt[2] >> 16) & 0xffff;
                    u4DebugCnt[2] &= 0xffff;
                }

                 u4err_value =0;

                 /*TINFO="%2d  %2d  %2d |(B3->B0) 0x%4x, 0x%4x, 0x%4x, 0x%4x | 0x%8x\n", ucdly_coarse_large, ucdly_coarse_0p5T, ucdly_fine_xT, u4DebugCnt[3], u4DebugCnt[2], u4DebugCnt[1], u4DebugCnt[0], u4err_value*/

                mcSHOW_DBG_MSG2(("%d %d %d |", ucdly_coarse_large, ucdly_coarse_0p5T, ucdly_fine_xT ));

                for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++)
                {
                    mcSHOW_DBG_MSG2(("0x%x ",u4DebugCnt[dqs_i]));
                }

                //mcSHOW_DBG_MSG2(("| %X",u4err_value));
                mcSHOW_DBG_MSG2(("\n"));

                //find gating window pass range per DQS separately
                for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++)
                {
                    if(u1PassByteCount  & (1<<dqs_i))
                    {
                        // real window found, break to prevent finding fake window.
                        continue;
                    }
                    u2DebugCntPerByte =(U16) u4DebugCnt[dqs_i];

                    // check if current tap is pass
                    ucCurrentPass =0;

                    if ((p->data_width == DATA_WIDTH_16BIT) || (p->rank == RANK_1 && p->asymmetric))
                    {
                        if(u2DebugCntPerByte==(GATING_GOLDEND_DQSCNT_LP3 << 1))
                        {
                                ucCurrentPass =1;
                                ucDQS_GW_FINE_STEP = 1;
                        }

                    }
                    else
                    {
                       if(u2DebugCntPerByte==GATING_GOLDEND_DQSCNT_LP3)
	                    {
	                            ucCurrentPass =1;
	                            ucDQS_GW_FINE_STEP = 1;
	                    }
                    }
                    //if current tap is pass
                    if (ucCurrentPass)
                    {
                        if (ucpass_begin[dqs_i]==0)
                        {
                            //no pass tap before , so it is the begining of pass range
                            ucpass_begin[dqs_i] = 1;
                            ucpass_count_1[dqs_i] = 0;
                            ucmin_coarse_tune2T_1[dqs_i] = ucdly_coarse_large;
                            ucmin_coarse_tune0p5T_1[dqs_i] = ucdly_coarse_0p5T;
                            ucmin_fine_tune_1[dqs_i] = ucdly_fine_xT;

                            /*TINFO="[Byte %d]First pass (%d, %d, %d)\n", dqs_i,ucdly_coarse_large, ucdly_coarse_0p5T, ucdly_fine_xT*/
                            mcSHOW_DBG_MSG(("[Byte %d]First pass (%d, %d, %d)\n", dqs_i,ucdly_coarse_large, ucdly_coarse_0p5T, ucdly_fine_xT));
                        }

                        if (ucpass_begin[dqs_i]==1)
                        {
                            //incr pass tap number
                            ucpass_count_1[dqs_i]++;
                        }

                    }
                    else // current tap is fail
                    {
                        if (ucpass_begin[dqs_i]==1)
                        {
                            //at the end of pass range
                            ucpass_begin[dqs_i] = 0;

                            //save the max range settings, to avoid glitch
                            if (ucpass_count_1[dqs_i] > ucpass_count[dqs_i])
                            {
                                ucmin_coarse_tune2T[dqs_i] = ucmin_coarse_tune2T_1[dqs_i];
                                ucmin_coarse_tune0p5T[dqs_i] = ucmin_coarse_tune0p5T_1[dqs_i];
                                ucmin_fine_tune[dqs_i] = ucmin_fine_tune_1[dqs_i];
                                ucpass_count[dqs_i] = ucpass_count_1[dqs_i];

                                //didn't has lead/lag RG, use SW workaround

                                if (ucpass_count_1[dqs_i] >= 70) // if didn't check fail region and pass UI more than 2UI, then workaround back to 2UI
                                {
                                    ucpass_count_1[dqs_i] = 64; // 2UI
                                    ucpass_count[dqs_i] = ucpass_count_1[dqs_i];
                                }

                                /*TINFO="[Byte %d]Bigger pass win(%d, %d, %d)  Pass tap=%d\n", \
                                    dqs_i, ucmin_coarse_tune2T_1[dqs_i], ucmin_coarse_tune0p5T_1[dqs_i], ucmin_fine_tune_1[dqs_i], ucpass_count_1[dqs_i]*/
                                mcSHOW_DBG_MSG(("[Byte %d]Bigger pass win(%d, %d, %d)  Pass tap=%d\n", \
                                    dqs_i, ucdly_coarse_large, ucdly_coarse_0p5T, ucdly_fine_xT, ucpass_count_1[dqs_i]));

                                #if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
                                gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.Gating_Win[dqs_i] = ucpass_count_1[dqs_i]*ucDQS_GW_FINE_STEP;
                                #endif
                                // LP4 pass window around 6 UI(burst mode), set 1~3 UI is pass
                                // LP3 pass window around 2 UI(pause mode), set 1~3 UI is pass
                                if((ucpass_count_1[dqs_i]*ucDQS_GW_FINE_STEP > 32) && (ucpass_count_1[dqs_i]*ucDQS_GW_FINE_STEP < 96))
                                {
                                    u1PassByteCount  |= (1<<dqs_i);
                                }

                                if(((p->data_width == DATA_WIDTH_16BIT) && (u1PassByteCount==0x3)) || \
									((p->data_width == DATA_WIDTH_32BIT) && (u1PassByteCount==0xf)))
                                {
                                    mcSHOW_DBG_MSG2(("All bytes gating window pass, Done, Early break!\n"));
                                    ucdly_fine_xT = DQS_GW_FINE_END;//break loop
                                    ucCoarseTune = ucCoarseEnd;      //break loop
                                }
                            }
                        }
                    }
                }
            }
        }


        vSetCalibrationResult(p, DRAM_CALIBRATION_GATING, DRAM_OK);

        //check if there is no pass taps for each DQS
        for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++)
        {
            if (ucpass_count[dqs_i]==0)
            {
                /*TINFO="error, no pass taps in DQS_%d !!!\n", dqs_i*/
                mcSHOW_ERR_MSG(("error, no pass taps in DQS_%d !!!\n", dqs_i));
                vSetCalibrationResult(p, DRAM_CALIBRATION_GATING, DRAM_FAIL);
            }

            #ifdef ENABLE_CALIBRATION_WINDOW_LOG_FOR_FT
            if(ucpass_count[dqs_i] < u2MinWinSize)
            {
                u2MinWinSize = ucpass_count[dqs_i];
                u1MinWinSizeByteidx= dqs_i;
            }
            #endif
        }

        #ifdef ENABLE_CALIBRATION_WINDOW_LOG_FOR_FT
        mcSHOW_DBG_MSG(("FT log: Gating min window : byte %d, size %d\n", u1MinWinSizeByteidx, u2MinWinSize*ucDQS_GW_FINE_STEP));
        #endif

        //find center of each byte
        for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++)
        {
        // -- PI for Phase0 & Phase1 --
            uctmp_offset = ucpass_count[dqs_i]*ucDQS_GW_FINE_STEP/2;
            uctmp_value = ucmin_fine_tune[dqs_i]+uctmp_offset;
            ucbest_fine_tune[dqs_i] = uctmp_value% ucRX_DLY_DQSIENSTB_LOOP;
            ucbest_fine_tune_P1[dqs_i] = ucbest_fine_tune[dqs_i];

        // coarse tune 0.5T for Phase 0
            uctmp_offset = uctmp_value / ucRX_DLY_DQSIENSTB_LOOP;
            uctmp_value = ucmin_coarse_tune0p5T[dqs_i]+uctmp_offset;
            ucbest_coarse_tune0p5T[dqs_i] = uctmp_value% ucRX_DQS_CTL_LOOP;

        // coarse tune 2T for Phase 0
            uctmp_offset = uctmp_value/ucRX_DQS_CTL_LOOP;
            ucbest_coarse_tune2T[dqs_i] = ucmin_coarse_tune2T[dqs_i]+uctmp_offset;
        // coarse tune 0.5T for Phase 1
            uctmp_value = ucbest_coarse_tune0p5T[dqs_i]+ ucFreqDiv;
            ucbest_coarse_tune0p5T_P1[dqs_i] = uctmp_value% ucRX_DQS_CTL_LOOP;

        // coarse tune 2T for Phase 1
            uctmp_offset = uctmp_value/ucRX_DQS_CTL_LOOP;
            ucbest_coarse_tune2T_P1[dqs_i] = ucbest_coarse_tune2T[dqs_i]+uctmp_offset;
        }

        mcSHOW_DBG_MSG(("===============================================================================\n"));
        mcSHOW_DBG_MSG(("    dqs input gating widnow, final delay value\n    Frequency=%d  rank=%d\n", p->frequency, p->rank));
        mcSHOW_DBG_MSG(("===============================================================================\n"));
        //mcSHOW_DBG_MSG(("test2_1: 0x%x, test2_2: 0x%x, test pattern: %d\n",  p->test2_1,p->test2_2, p->test_pattern));

        for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++)
        {
            /*TINFO="best DQS%d delay(2T, 0.5T, PI) = (%d, %d, %d)\n", dqs_i, ucbest_coarse_tune2T[dqs_i], ucbest_coarse_tune0p5T[dqs_i], ucbest_fine_tune[dqs_i]*/
            mcSHOW_DBG_MSG(("R%d FINAL: GW best DQS%d P0 delay(2T, 0.5T, PI) = (%d, %d, %d) [tap = %d]\n", p->rank, dqs_i, ucbest_coarse_tune2T[dqs_i], ucbest_coarse_tune0p5T[dqs_i], ucbest_fine_tune[dqs_i], ucpass_count[dqs_i]));
            #if GATING_ADJUST_TXDLY_FOR_TRACKING
            // find min gating TXDLY (should be in P0)
            u1TX_dly_DQSgated  = ((ucbest_coarse_tune2T[dqs_i] <<1)|((ucbest_coarse_tune0p5T[dqs_i] >>2)&0x1));

            if(u1TX_dly_DQSgated < u1TXDLY_Cal_min)
                u1TXDLY_Cal_min = u1TX_dly_DQSgated;

            ucbest_coarse_tune0p5T_backup[p->rank][dqs_i] = ucbest_coarse_tune0p5T[dqs_i];
            ucbest_coarse_tune2T_backup[p->rank][dqs_i] = ucbest_coarse_tune2T[dqs_i];
            #endif
        }
        mcSHOW_DBG_MSG2(("===============================================================================\n"));

        for (dqs_i=0; dqs_i<(p->data_width/DQS_BIT_NUMBER); dqs_i++)
        {
            /*TINFO="best DQS%d P1 delay(2T, 0.5T, PI) = (%d, %d, %d)\n", dqs_i, ucbest_coarse_tune2T_P1[dqs_i], ucbest_coarse_tune0p5T_P1[dqs_i], ucbest_fine_tune[dqs_i]*/
            mcSHOW_DBG_MSG(("R%d FINAL: GW best DQS%d P1 delay(2T, 0.5T, PI) = (%d, %d, %d)\n", p->rank, dqs_i, ucbest_coarse_tune2T_P1[dqs_i], ucbest_coarse_tune0p5T_P1[dqs_i], ucbest_fine_tune[dqs_i]));

            #if GATING_ADJUST_TXDLY_FOR_TRACKING
            // find max gating TXDLY (should be in P1)
            u1TX_dly_DQSgated  = ((ucbest_coarse_tune2T_P1[dqs_i] <<1)|((ucbest_coarse_tune0p5T_P1[dqs_i] >>2)&0x1));

            if(u1TX_dly_DQSgated > u1TXDLY_Cal_max)
                u1TXDLY_Cal_max = u1TX_dly_DQSgated;

            ucbest_coarse_tune0p5T_P1_backup[p->rank][dqs_i] = ucbest_coarse_tune0p5T_P1[dqs_i];
            ucbest_coarse_tune2T_P1_backup[p->rank][dqs_i] = ucbest_coarse_tune2T_P1[dqs_i];
            #endif
        }

        mcSHOW_DBG_MSG2(("===============================================================================\n"));

        //Restore registers
        vIO32Write4B(DRAMC_REG_STBCAL, u4BakReg_DRAMC_DQSCAL0);
        vIO32Write4B(DRAMC_REG_STBCAL1, u4BakReg_DRAMC_STBCAL_F);
        vIO32Write4B(DRAMC_REG_DDRCONF0, u4BakReg_DRAMC_WODT);
        vIO32Write4B(DRAMC_REG_SPCMD, u4BakReg_DRAMC_SPCMD);
        vIO32Write4B(DRAMC_REG_REFCTRL0, u4BakReg_DRAMC_REFCTRL0);

        // Set Coarse Tune Value to registers
        //DramPhyCGReset(p, 1);// need to reset when UI update or PI change >=2

#if SUPPORT_TYPE_DDR3X4
        if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
        {
            //swap B12 & B13
            mcSHOW_DBG_MSG(("DDR3X4 GW SWAP\n"));
            ucbest_coarse_tune2T[2] = ucbest_coarse_tune2T[1];
            ucbest_coarse_tune2T[3] = ucbest_coarse_tune2T[1];
            ucbest_coarse_tune2T_P1[2] = ucbest_coarse_tune2T_P1[1];
            ucbest_coarse_tune2T_P1[3] = ucbest_coarse_tune2T_P1[1];
            ucbest_coarse_tune0p5T[2] = ucbest_coarse_tune0p5T[1];
            ucbest_coarse_tune0p5T[3] = ucbest_coarse_tune0p5T[1];
            ucbest_coarse_tune0p5T_P1[2] = ucbest_coarse_tune0p5T_P1[1];
            ucbest_coarse_tune0p5T_P1[3] = ucbest_coarse_tune0p5T_P1[1];
            ucbest_fine_tune[2] = ucbest_fine_tune[1];
            ucbest_fine_tune[3] = ucbest_fine_tune[1];

            for (dqs_i=0; dqs_i<(p->data_width/4); dqs_i++)
            {
                mcSHOW_DBG_MSG(("DDR3X4 R%d FINAL: GW best DQS%d P0 delay(2T, 0.5T, PI) = (%d, %d, %d) [tap = %d]\n", p->rank, dqs_i, ucbest_coarse_tune2T[dqs_i], ucbest_coarse_tune0p5T[dqs_i], ucbest_fine_tune[dqs_i], ucpass_count[dqs_i]));
            } 
            for (dqs_i=0; dqs_i<(p->data_width/4); dqs_i++)
            {
                mcSHOW_DBG_MSG(("DDR3X4 R%d FINAL: GW best DQS%d P1 delay(2T, 0.5T, PI) = (%d, %d, %d)\n", p->rank, dqs_i, ucbest_coarse_tune2T_P1[dqs_i], ucbest_coarse_tune0p5T_P1[dqs_i], ucbest_fine_tune[dqs_i]));
            } 

            ucbest_coarse_tune0p5T_backup[p->rank][2] = ucbest_coarse_tune0p5T_backup[p->rank][1];
            ucbest_coarse_tune2T_backup[p->rank][2] = ucbest_coarse_tune2T_backup[p->rank][1];
            ucbest_coarse_tune0p5T_backup[p->rank][3] = ucbest_coarse_tune0p5T_backup[p->rank][1];
            ucbest_coarse_tune2T_backup[p->rank][3] = ucbest_coarse_tune2T_backup[p->rank][1];

            ucbest_coarse_tune0p5T_P1_backup[p->rank][2] = ucbest_coarse_tune0p5T_P1_backup[p->rank][1];
            ucbest_coarse_tune2T_P1_backup[p->rank][2] = ucbest_coarse_tune2T_P1_backup[p->rank][1];
            ucbest_coarse_tune0p5T_P1_backup[p->rank][3] = ucbest_coarse_tune0p5T_P1_backup[p->rank][1];
            ucbest_coarse_tune2T_P1_backup[p->rank][3] = ucbest_coarse_tune2T_P1_backup[p->rank][1];
        }
#endif

        // 4T or 2T coarse tune
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, \
                                    P_Fld((U32) ucbest_coarse_tune2T[0], SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune2T[1], SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune2T[2], SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune2T[3], SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_P1[0], SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_P1[1], SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_P1[2], SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_P1[3], SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1));

        // 0.5T coarse tune
        vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, \
                                    P_Fld((U32) ucbest_coarse_tune0p5T[0], SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T[1], SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T[2], SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T[3], SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_P1[0], SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_P1[1], SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_P1[2], SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_P1[3], SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1));


        // Set Fine Tune Value to registers
        u4value = ucbest_fine_tune[0] | (ucbest_fine_tune[1]<<8) | (ucbest_fine_tune[2]<<16) | (ucbest_fine_tune[3]<<24);
        vIO32Write4B(DRAMC_REG_SHURK0_DQSIEN, u4value);

        //mcDELAY_US(1);//delay 2T
        //DramPhyCGReset(p, 0);
        DramPhyReset(p);   //reset phy, reset read data counter

        /*TINFO="[DramcRxdqsGatingCal] ====Done====\n"*/
        mcSHOW_DBG_MSG(("[DramcRxdqsGatingCal] ====Done====\n"));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS0_2T = ucbest_coarse_tune2T_P1[0];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS0_05T = ucbest_coarse_tune0p5T_P1[0];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS0_PI = ucbest_fine_tune[0];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS1_2T = ucbest_coarse_tune2T_P1[1];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS1_05T = ucbest_coarse_tune0p5T_P1[1];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS1_PI = ucbest_fine_tune[1];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS2_2T = ucbest_coarse_tune2T_P1[2];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS2_05T = ucbest_coarse_tune0p5T_P1[2];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS2_PI = ucbest_fine_tune[2];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS3_2T = ucbest_coarse_tune2T_P1[3];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS3_05T = ucbest_coarse_tune0p5T_P1[3];
        gDRAM_CALIB_LOG.RANK[p->rank].GatingWindow.DQS3_PI = ucbest_fine_tune[3];
#endif

        return DRAM_OK;
}

#if GATING_ADJUST_TXDLY_FOR_TRACKING
void DramcRxdqsGatingPostProcess(DRAMC_CTX_T *p)
{
     U8 dqs_i, u1RankRxDVS, ucbit_num;
     U8 u1RankIdx, u1RankMax;
     S8 s1ChangeDQSINCTL;
     U32 backup_rank, u1EnRxODT = 0;
     U32 u4ReadDQSINCTL, u4ReadTXDLY[RANK_MAX][DQS_NUMBER], u4ReadTXDLY_P1[RANK_MAX][DQS_NUMBER], u4RankINCTL_ROOT, u4XRTR2R, reg_TX_dly_DQSgated_min;

     backup_rank = u1GetRank(p);

     if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
        ucbit_num = 4;
     else
        ucbit_num = DQS_BIT_NUMBER;

     // 1066 : reg_TX_dly_DQSgated (min) =2
     // 1600 : reg_TX_dly_DQSgated (min) =3
    if(p->frequency <= DDR_DDR1333)
         reg_TX_dly_DQSgated_min = 2;
    else
         reg_TX_dly_DQSgated_min= 3;


    // === Begin of DVS setting =====
    //RANKRXDVS = reg_TX_dly_DQSgated (min) -1 = Roundup(tDQSCKdiff/MCK)
    if(reg_TX_dly_DQSgated_min>1)
    {
         u1RankRxDVS = reg_TX_dly_DQSgated_min -1;
    }
     else
    {
        u1RankRxDVS=0;
        mcSHOW_ERR_MSG(("[DramcRxdqsGatingPostProcess] u1RankRxDVS <1,  Please check!\n"));
    }

    vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ7, u1RankRxDVS, SHU1_B0_DQ7_R_DMRANKRXDVS_B0);
    vIO32WriteFldAlign(DDRPHY_SHU1_B1_DQ7, u1RankRxDVS, SHU1_B1_DQ7_R_DMRANKRXDVS_B1);
    vIO32WriteFldAlign(DDRPHY_SHU1_B0_DQ7+(1<<POS_BANK_NUM), u1RankRxDVS, SHU1_B0_DQ7_R_DMRANKRXDVS_B0);
    vIO32WriteFldAlign(DDRPHY_SHU1_B1_DQ7+(1<<POS_BANK_NUM), u1RankRxDVS, SHU1_B1_DQ7_R_DMRANKRXDVS_B1);

     // === End of DVS setting =====

     s1ChangeDQSINCTL = reg_TX_dly_DQSgated_min- u1TXDLY_Cal_min;

     mcSHOW_DBG_MSG(("[DramcRxdqsGatingPostProcess] p->frequency %d\n", p->frequency));
     mcSHOW_DBG_MSG(("[DramcRxdqsGatingPostProcess] s1ChangeDQSINCTL %d, reg_TX_dly_DQSgated_min %d, u1TXDLY_Cal_min %d\n", s1ChangeDQSINCTL, reg_TX_dly_DQSgated_min, u1TXDLY_Cal_min));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
     gDRAM_CALIB_LOG.RxdqsGatingPostProcess.s1ChangeDQSINCTL = s1ChangeDQSINCTL;
     gDRAM_CALIB_LOG.RxdqsGatingPostProcess.reg_TX_dly_DQSgated_min = reg_TX_dly_DQSgated_min;
     gDRAM_CALIB_LOG.RxdqsGatingPostProcess.u1TXDLY_Cal_min = u1TXDLY_Cal_min;
#endif

     if(s1ChangeDQSINCTL!=0)  // need to change DQSINCTL and TXDLY of each byte
     {
         u1TXDLY_Cal_min += s1ChangeDQSINCTL;
         u1TXDLY_Cal_max += s1ChangeDQSINCTL;

        #if DUAL_RANK_ENABLE
        if (p->support_rank_num==RANK_DUAL)
            u1RankMax = RANK_MAX;
        else
        #endif
             u1RankMax =RANK_1;

        for(u1RankIdx=0; u1RankIdx<u1RankMax; u1RankIdx++)
        {
             mcSHOW_DBG_MSG2(("====DramcRxdqsGatingPostProcess (Rank = %d) ========================================\n", u1RankIdx));

             for (dqs_i=0; dqs_i<(p->data_width/ucbit_num); dqs_i++)
             {
                 u4ReadTXDLY[u1RankIdx][dqs_i]= ((ucbest_coarse_tune2T_backup[u1RankIdx][dqs_i]<<1) + ((ucbest_coarse_tune0p5T_backup[u1RankIdx][dqs_i]>>2) & 0x1));
                 u4ReadTXDLY_P1[u1RankIdx][dqs_i]= ((ucbest_coarse_tune2T_P1_backup[u1RankIdx][dqs_i]<<1) + ((ucbest_coarse_tune0p5T_P1_backup[u1RankIdx][dqs_i]>>2) & 0x1));

                 u4ReadTXDLY[u1RankIdx][dqs_i] += s1ChangeDQSINCTL;
                 u4ReadTXDLY_P1[u1RankIdx][dqs_i] += s1ChangeDQSINCTL;

                 ucbest_coarse_tune2T_backup[u1RankIdx][dqs_i] = (u4ReadTXDLY[u1RankIdx][dqs_i] >>1);
                 ucbest_coarse_tune0p5T_backup[u1RankIdx][dqs_i] = ((u4ReadTXDLY[u1RankIdx][dqs_i] & 0x1) <<2)+(ucbest_coarse_tune0p5T_backup[u1RankIdx][dqs_i] & 0x3);

                 ucbest_coarse_tune2T_P1_backup[u1RankIdx][dqs_i] = (u4ReadTXDLY_P1[u1RankIdx][dqs_i] >>1);
                 ucbest_coarse_tune0p5T_P1_backup[u1RankIdx][dqs_i] = ((u4ReadTXDLY_P1[u1RankIdx][dqs_i] & 0x1)<<2) +(ucbest_coarse_tune0p5T_P1_backup[u1RankIdx][dqs_i] & 0x3);

                 mcSHOW_DBG_MSG(("best DQS%d P0 delay(2T, 0.5T) = (%d, %d)\n", dqs_i, ucbest_coarse_tune2T_backup[u1RankIdx][dqs_i], ucbest_coarse_tune0p5T_backup[u1RankIdx][dqs_i]));
             }

             for (dqs_i=0; dqs_i<(p->data_width/ucbit_num); dqs_i++)
             {
                mcSHOW_DBG_MSG(("best DQS%d P1 delay(2T, 0.5T) = (%d, %d)\n", dqs_i, ucbest_coarse_tune2T_P1_backup[u1RankIdx][dqs_i], ucbest_coarse_tune0p5T_P1_backup[u1RankIdx][dqs_i]));
             }
        }

        for(u1RankIdx=0; u1RankIdx<u1RankMax; u1RankIdx++)
        {
            vSetRank(p, u1RankIdx);
            // 4T or 2T coarse tune
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG0, \
                                    P_Fld((U32) ucbest_coarse_tune2T_backup[u1RankIdx][0], SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_backup[u1RankIdx][1], SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_backup[u1RankIdx][2], SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_backup[u1RankIdx][3], SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_P1_backup[u1RankIdx][0], SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_P1_backup[u1RankIdx][1], SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_P1_backup[u1RankIdx][2], SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune2T_P1_backup[u1RankIdx][3], SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1));

                // 0.5T coarse tune
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQSG1, \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_backup[u1RankIdx][0], SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_backup[u1RankIdx][1], SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_backup[u1RankIdx][2], SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_backup[u1RankIdx][3], SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_P1_backup[u1RankIdx][0], SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_P1_backup[u1RankIdx][1], SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_P1_backup[u1RankIdx][2], SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1)| \
                                    P_Fld((U32) ucbest_coarse_tune0p5T_P1_backup[u1RankIdx][3], SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1));
        }
    }
    vSetRank(p, backup_rank);
    u1EnRxODT = u4IO32ReadFldAlign(DRAMC_REG_SHU_ODTCTRL, SHU_ODTCTRL_ROEN);
    u4ReadDQSINCTL = u4IO32ReadFldAlign(DRAMC_REG_SHURK0_DQSCTL, SHURK0_DQSCTL_DQSINCTL);
    u4ReadDQSINCTL -= s1ChangeDQSINCTL;

    if(u4ReadDQSINCTL>=2)
        u4RankINCTL_ROOT = u4ReadDQSINCTL-2;
    else
    {
        u4RankINCTL_ROOT=0;
        if (u1EnRxODT)
            mcSHOW_ERR_MSG(("[DramcRxdqsGatingPostProcess] DQSINCTL <2,  Risk for supporting RX ODT\n"));
        else if (u4ReadDQSINCTL < 1)
            mcSHOW_ERR_MSG(("[DramcRxdqsGatingPostProcess] DQSINCTL <1,  Risk for supporting RX\n"));
    }

    if (u1TXDLY_Cal_min < 2)
    {
        mcSHOW_ERR_MSG(("[DramcRxdqsGatingPostProcess] MIO_CK <2,  Risk for supporting HW Gating\n"));
    }

    vSetRank(p, RANK_0);
    vIO32WriteFldAlign(DRAMC_REG_SHURK0_DQSCTL, u4ReadDQSINCTL, SHURK0_DQSCTL_DQSINCTL);  //Rank0 DQSINCTL
    vIO32WriteFldAlign(DRAMC_REG_SHURK1_DQSCTL, u4ReadDQSINCTL, SHURK1_DQSCTL_R1DQSINCTL); //Rank1 DQSINCTL, no use in A-PHY. Common DQSINCTL of both rank.
    vIO32WriteFldAlign(DRAMC_REG_SHU_RANKCTL, u4ReadDQSINCTL, SHU_RANKCTL_RANKINCTL_PHY);  //RANKINCTL_PHY = DQSINCTL
    vIO32WriteFldAlign(DRAMC_REG_SHU_RANKCTL, u4RankINCTL_ROOT, SHU_RANKCTL_RANKINCTL);  //RANKINCTL= DQSINCTL -2
    vIO32WriteFldAlign(DRAMC_REG_SHU_RANKCTL, u4RankINCTL_ROOT, SHU_RANKCTL_RANKINCTL_ROOT1);  //RANKINCTL_ROOT1= DQSINCTL -2

    //XRTR2R=A-phy forbidden margin(6T) + reg_TX_dly_DQSgated (max) +Roundup(tDQSCKdiff/MCK+0.25MCK)+1(05T sel_ph margin)-1(forbidden margin overlap part)
    //Roundup(tDQSCKdiff/MCK+1UI) =1~2 all LP3 and LP4 timing
    u4XRTR2R= 8 + u1TXDLY_Cal_max;  // 6+ u1TXDLY_Cal_max +2
    vIO32WriteFldAlign(DRAMC_REG_SHU_ACTIM_XRT, u4XRTR2R, SHU_ACTIM_XRT_XRTR2R);
    vSetRank(p, backup_rank);

    mcSHOW_DBG_MSG2(("TX_dly_DQSgated check: min %d  max %d,  s1ChangeDQSINCTL=%d\n", u1TXDLY_Cal_min, u1TXDLY_Cal_max, s1ChangeDQSINCTL));
    mcSHOW_DBG_MSG2(("DQSINCTL=%d, RANKINCTL=%d, u4XRTR2R=%d\n", u4ReadDQSINCTL, u4RankINCTL_ROOT, u4XRTR2R));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    gDRAM_CALIB_LOG.RxdqsGatingPostProcess.TX_dly_DQSgated_check_min = u1TXDLY_Cal_min;
    gDRAM_CALIB_LOG.RxdqsGatingPostProcess.TX_dly_DQSgated_check_max = u1TXDLY_Cal_max;
    gDRAM_CALIB_LOG.RxdqsGatingPostProcess.DQSINCTL = u4ReadDQSINCTL;
    gDRAM_CALIB_LOG.RxdqsGatingPostProcess.RANKINCTL = u4RankINCTL_ROOT;
    gDRAM_CALIB_LOG.RxdqsGatingPostProcess.u4XRTR2R = u4XRTR2R;
#endif
}
#endif


#if GATING_ADJUST_TXDLY_FOR_TRACKING
void DramcRxdqsGatingPreProcess(DRAMC_CTX_T *p)
{
    u1TXDLY_Cal_min =0xff;
    u1TXDLY_Cal_max=0;
}
#endif
#endif //SIMULATION_GATING

//-------------------------------------------------------------------------
/** DramcRxWindowPerbitCal (v2 version)
 *  start the rx dqs perbit sw calibration.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------

static void SetRxDqDqsDelay(DRAMC_CTX_T *p, S16 iDelay)
{
    U8 u1ByteIdx;
    //U32 u4value;
    U8 dl_value[8];
    U8 ucbit_num;

    if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
        ucbit_num = 4;
    else
        ucbit_num = DQS_BIT_NUMBER;

    if (iDelay <=0)
    {
        // Set DQS delay
        //B0 DQS
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ6, P_Fld(-iDelay, SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0)
        			| P_Fld(-iDelay, SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0));
        //B1 DQS
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ6, P_Fld(-iDelay, SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_F_DLY_B1)
        			| P_Fld(-iDelay, SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1));
        //B2 DQS
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ6+(1<<POS_BANK_NUM), P_Fld(-iDelay, SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0)
        			| P_Fld(-iDelay, SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0));
        //B3 DQS
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ6+(1<<POS_BANK_NUM), P_Fld(-iDelay, SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_F_DLY_B1)
        			| P_Fld(-iDelay, SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1));


        DramPhyReset(p);
    }
    else
    {
        // Adjust DQM output delay.

        for (u1ByteIdx=0; u1ByteIdx<(p->data_width/ucbit_num); u1ByteIdx++)
        {
            Set_RX_DQM_DelayLine_Phy_Byte(p, u1ByteIdx, iDelay);
        }

        // Adjust DQ output delay.
        //u4value = ((U32) iDelay) | (((U32)iDelay)<<8) | (((U32)iDelay)<<16) | (((U32)iDelay)<<24);

        //every 2bit dq have the same delay register address
        for (u1ByteIdx=0; u1ByteIdx<(p->data_width/ucbit_num); u1ByteIdx++)
        {
            dl_value[0] = iDelay;
            dl_value[1] = iDelay;
            dl_value[2] = iDelay;
            dl_value[3] = iDelay;
            dl_value[4] = iDelay;
            dl_value[5] = iDelay;
            dl_value[6] = iDelay;
            dl_value[7] = iDelay;

            Set_RX_DQ_DelayLine_Phy_Byte(p, u1ByteIdx, dl_value);
        }
    }
}

#if SIMULATION_RX_PERBIT
DRAM_STATUS_T DramcRxEyeScan(DRAMC_CTX_T *p, U8 u1UseTestEngine)
{
    U8 ii, u1BitIdx, u1ByteIdx;
    U8 dl_value[DQS_BIT_NUMBER]={0,0,0,0,0,0,0,0};
    U16 iWin_max = 0, iWin_min = 0xfff;
    S32 iDQSDlyPerbyte[DQS_NUMBER] = {0,0,0,0};
    S16 iFirst_max = -0xfff, iLast_min = 0xfff;
    PASS_WIN_DATA_T WinPerBit[DQ_DATA_WIDTH], FinalWinPerBit[DQ_DATA_WIDTH];

    mcSHOW_DBG_MSG(("DramcScanRxVref 21\n"));
    vIO32WriteFldAlign_All(DDRPHY_SHU1_B0_DQ5, 21, SHU1_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU1_B1_DQ5, 21, SHU1_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1);
    udelay(1);
    DramcRxWindowPerbitCal((DRAMC_CTX_T *) p, u1UseTestEngine);

    mcSHOW_DBG_MSG(("DramcScanRxVref 3\n"));
    vIO32WriteFldAlign_All(DDRPHY_SHU1_B0_DQ5, 3, SHU1_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU1_B1_DQ5, 3, SHU1_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1);
    udelay(2);
    DramcRxWindowPerbitCal((DRAMC_CTX_T *) p, u1UseTestEngine);

    vIO32WriteFldAlign_All(DDRPHY_SHU1_B0_DQ5, 14, SHU1_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
    vIO32WriteFldAlign_All(DDRPHY_SHU1_B1_DQ5, 14, SHU1_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1);
    udelay(1);

    mcSHOW_DBG_MSG(("DramcScanRxVref process begin\n"));

    //iWin_max = 0;
    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
    {
        for (u1BitIdx=u1ByteIdx*DQS_BIT_NUMBER; u1BitIdx<(u1ByteIdx+1)*DQS_BIT_NUMBER; u1BitIdx++)
        {
            iFirst_max = -0xfff;
            iLast_min = 0xfff;

            //VREF:21, VREF:3
            for (ii=0; ii<2; ii++)
            {
                if (RxWinPerBit[ii][u1BitIdx].first_pass > iFirst_max)
                    iFirst_max = RxWinPerBit[ii][u1BitIdx].first_pass;

                if (RxWinPerBit[ii][u1BitIdx].last_pass < iLast_min)
                    iLast_min = RxWinPerBit[ii][u1BitIdx].last_pass;
             }

            //the windows are distributed in the diffrent win half
            if (iLast_min < (iFirst_max+3))
            {   //choose the left half as the final win
                if (RxWinPerBit[0][u1BitIdx].last_pass < RxWinPerBit[1][u1BitIdx].last_pass)
                    ii = 0;
                else
                    ii = 1;
                iFirst_max = RxWinPerBit[ii][u1BitIdx].first_pass;
                iLast_min = RxWinPerBit[ii][u1BitIdx].last_pass;
            }
            FinalWinPerBit[u1BitIdx].first_pass = iFirst_max;
            FinalWinPerBit[u1BitIdx].last_pass = iLast_min;
            FinalWinPerBit[u1BitIdx].win_center = (iFirst_max + iLast_min) >> 1;
            FinalWinPerBit[u1BitIdx].win_size = iLast_min - iFirst_max + 1;

            if ((FinalWinPerBit[u1BitIdx].win_size > iWin_max) && (iLast_min < 0))
                iWin_max = FinalWinPerBit[u1BitIdx].win_size;

            if ((FinalWinPerBit[u1BitIdx].win_size < iWin_min) && (iLast_min < 0))
                iWin_min = FinalWinPerBit[u1BitIdx].win_size;
        }

    }

    mcSHOW_DBG_MSG(("MAX:%d MIN:%d ", iWin_max, iWin_min));
    if (iWin_max > 0)
    {
        if (iWin_min < 10)
            iWin_min = 10;
        iWin_max = (iWin_max + iWin_min + 1) >> 1;
    }
    mcSHOW_DBG_MSG(("EVERAGE:%d\n", iWin_max));

    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
    {
        iDQSDlyPerbyte[u1ByteIdx] = MAX_RX_DQSDLY_TAPS;
        for (u1BitIdx=u1ByteIdx*DQS_BIT_NUMBER; u1BitIdx<(u1ByteIdx+1)*DQS_BIT_NUMBER; u1BitIdx++)
        {
            if ((iWin_max > 0) && (FinalWinPerBit[u1BitIdx].last_pass >= 0) && (FinalWinPerBit[u1BitIdx].win_size > iWin_max))
            {
                mcSHOW_DBG_MSG(("BIT%d WIN:%d CHANGE TO WIN:%d\n", u1BitIdx, FinalWinPerBit[u1BitIdx].win_size, iWin_max));
                FinalWinPerBit[u1BitIdx].win_size = iWin_max;
                FinalWinPerBit[u1BitIdx].last_pass = iWin_max + FinalWinPerBit[u1BitIdx].first_pass - 1;
                FinalWinPerBit[u1BitIdx].win_center = (FinalWinPerBit[u1BitIdx].first_pass + FinalWinPerBit[u1BitIdx].last_pass) >> 1;
            }

            //find the min center for per byte
            if (FinalWinPerBit[u1BitIdx].win_center < iDQSDlyPerbyte[u1ByteIdx])
                iDQSDlyPerbyte[u1ByteIdx] = FinalWinPerBit[u1BitIdx].win_center;
        }

        if (iDQSDlyPerbyte[u1ByteIdx] > 0)  // Delay DQS=0, Delay DQ only
        {
            iDQSDlyPerbyte[u1ByteIdx] = 0;
        }
        else  //Need to delay DQS
        {
            iDQSDlyPerbyte[u1ByteIdx]  = -iDQSDlyPerbyte[u1ByteIdx] ;
        }

        // we delay DQ or DQS to let DQS sample the middle of rx pass window for all the 8 bits,
        for (u1BitIdx=u1ByteIdx*DQS_BIT_NUMBER; u1BitIdx<(u1ByteIdx+1)*DQS_BIT_NUMBER; u1BitIdx++)
        {
            FinalWinPerBit[u1BitIdx].best_dqdly = iDQSDlyPerbyte[u1ByteIdx] + FinalWinPerBit[u1BitIdx].win_center;
        }
    }

    for (u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
    {
        FinalWinPerBit[u1BitIdx].win_center = (FinalWinPerBit[u1BitIdx].last_pass + FinalWinPerBit[u1BitIdx].first_pass)>>1;     // window center of each DQ bit
        FinalWinPerBit[u1BitIdx].win_size = FinalWinPerBit[u1BitIdx].last_pass - FinalWinPerBit[u1BitIdx].first_pass + 1;     // window size of each DQ bit

        mcSHOW_DBG_MSG(("R%d FINAL: RX Bit %d, %d (%d ~ %d) %d\n", p->rank, u1BitIdx, FinalWinPerBit[u1BitIdx].win_center, FinalWinPerBit[u1BitIdx].first_pass, FinalWinPerBit[u1BitIdx].last_pass, FinalWinPerBit[u1BitIdx].win_size));
    }

    mcSHOW_DBG_MSG(("ScanRxVref DQS Delay :\nDQS0 = %d, DQS1 = %d, DQS2 = %d, DQS3 = %d\n", iDQSDlyPerbyte[0], iDQSDlyPerbyte[1], iDQSDlyPerbyte[2], iDQSDlyPerbyte[3]));
    mcSHOW_DBG_MSG(("ScanRxVref DQ Delay :\n"));

    for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx=u1BitIdx+4)
    {
        mcSHOW_DBG_MSG(("DQ%d =%d, DQ%d =%d, DQ%d =%d, DQ%d =%d \n", u1BitIdx, FinalWinPerBit[u1BitIdx].best_dqdly, u1BitIdx+1, FinalWinPerBit[u1BitIdx+1].best_dqdly, u1BitIdx+2, FinalWinPerBit[u1BitIdx+2].best_dqdly, u1BitIdx+3, FinalWinPerBit[u1BitIdx+3].best_dqdly));
    }


    //B0 DQS
    vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ6, P_Fld(iDQSDlyPerbyte[0], SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0)
                | P_Fld(iDQSDlyPerbyte[0], SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0));
    //B1 DQS
    vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ6, P_Fld(iDQSDlyPerbyte[1], SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_F_DLY_B1)
                | P_Fld(iDQSDlyPerbyte[1], SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1));
    //B2 DQS
    vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ6+(1<<POS_BANK_NUM), P_Fld(iDQSDlyPerbyte[2], SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0)
                | P_Fld(iDQSDlyPerbyte[2], SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0));
    //B3 DQS
    vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ6+(1<<POS_BANK_NUM), P_Fld(iDQSDlyPerbyte[3], SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_F_DLY_B1)
                | P_Fld(iDQSDlyPerbyte[3], SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1));

    DramPhyReset(p);

    if (p->en_4bitMux == ENABLE)
    {
        mcSHOW_DBG_MSG(("Rx 4bitMux is enabled\n"));

        for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
        mcSHOW_DBG_MSG(("%d ", FinalWinPerBit[u1BitIdx].best_dqdly));
        }
        mcSHOW_DBG_MSG(("\n"));


        for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            memcpy(&WinPerBit[Bit_DQ_Mapping[u1BitIdx]], &FinalWinPerBit[u1BitIdx], sizeof(PASS_WIN_DATA_T));
        }
        memcpy(FinalWinPerBit, WinPerBit, sizeof(PASS_WIN_DATA_T) * DQ_DATA_WIDTH);

        for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            mcSHOW_DBG_MSG(("%d ", FinalWinPerBit[u1BitIdx].best_dqdly));
        }
        mcSHOW_DBG_MSG(("\n"));

    }

    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
    {
        u1BitIdx = u1ByteIdx *DQS_BIT_NUMBER;

        dl_value[0] = FinalWinPerBit[u1BitIdx].best_dqdly;
        dl_value[1] = FinalWinPerBit[u1BitIdx+1].best_dqdly;
        dl_value[2] = FinalWinPerBit[u1BitIdx+2].best_dqdly;
        dl_value[3] = FinalWinPerBit[u1BitIdx+3].best_dqdly;
        dl_value[4] = FinalWinPerBit[u1BitIdx+4].best_dqdly;
        dl_value[5] = FinalWinPerBit[u1BitIdx+5].best_dqdly;
        dl_value[6] = FinalWinPerBit[u1BitIdx+6].best_dqdly;
        dl_value[7] = FinalWinPerBit[u1BitIdx+7].best_dqdly;

        Set_RX_DQ_DelayLine_Phy_Byte(p, u1ByteIdx, dl_value);
    }
    mcSHOW_DBG_MSG(("DramcScanRxVref process done\n"));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQS0_delay = iDQSDlyPerbyte[0];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQS1_delay = iDQSDlyPerbyte[1];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQS2_delay = iDQSDlyPerbyte[2];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQS3_delay = iDQSDlyPerbyte[3];

    for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
    {
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].first_pass = FinalWinPerBit[u1BitIdx].first_pass;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].last_pass = FinalWinPerBit[u1BitIdx].last_pass;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].win_size = FinalWinPerBit[u1BitIdx].win_size;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].win_center = FinalWinPerBit[u1BitIdx].win_center;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].left_margin= FinalWinPerBit[u1BitIdx].win_center - FinalWinPerBit[u1BitIdx].first_pass;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].right_margin= FinalWinPerBit[u1BitIdx].last_pass - FinalWinPerBit[u1BitIdx].win_center;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQ_delay[u1BitIdx] = FinalWinPerBit[u1BitIdx].best_dqdly;
    }
#endif

    return DRAM_OK;
}


DRAM_STATUS_T DramcRxWindowPerbitCal(DRAMC_CTX_T *p, U8 u1UseTestEngine)
{
    U8 ii, u1BitIdx, u1ByteIdx;
    U8 ucbit_first, ucbit_last, ucbit_num;
    S16 iDelay, u4DelayBegin, u4DelayEnd, u4DelayStep=1;
    U32 uiFinishCount;
    U32 u4err_value = 0, u4fail_bit;
    PASS_WIN_DATA_T WinPerBit[DQ_DATA_WIDTH], FinalWinPerBit[DQ_DATA_WIDTH];
    S32 iDQSDlyPerbyte[4] = {0,0,0,0}, iDQMDlyPerbyte[4] = {0,0,0,0};//, iFinalDQSDly[DQS_NUMBER];
    U16 u2TempWinSum, u2TmpDQMSum;

    U16 u2WinSize;
    U16 u2MinWinSize = 0xffff;
    U8 u1MinWinSizeBitidx = 0;

    U8 dl_value[8]={0,0,0,0,0,0,0,0};
    U8 backup_rank;

    #if REG_ACCESS_PORTING_DGB
    RegLogEnable =1;
    mcSHOW_DBG_MSG(("\n[REG_ACCESS_PORTING_FUNC]   DramcRxWindowPerbitCal\n"));
    #endif

    // error handling
    if (!p)
    {
        mcSHOW_ERR_MSG(("context is NULL\n"));
        return DRAM_FAIL;
    }

    backup_rank = u1GetRank(p);

    // 1.delay DQ ,find the pass widnow (left boundary).
    // 2.delay DQS find the pass window (right boundary).
    // 3.Find the best DQ / DQS to satify the middle value of the overall pass window per bit
    // 4.Set DQS delay to the max per byte, delay DQ to de-skew
    mcSHOW_DBG_MSG(("\n[RX]\n"));
    vPrintCalibrationBasicInfo(p);
    mcSHOW_DBG_MSG2(("Start DQ delay to find pass range, UseTestEngine =%d\n", u1UseTestEngine));
    mcSHOW_DBG_MSG2(("x-axis is bit #; y-axis is DQ delay (%d~%d)\n", (-MAX_RX_DQSDLY_TAPS), MAX_RX_DQDLY_TAPS));

    //defult set result fail. When window found, update the result as oK
    if(u1UseTestEngine)
        vSetCalibrationResult(p, DRAM_CALIBRATION_RX_PERBIT, DRAM_FAIL);

    // initialize parameters
    u2TempWinSum =0;
    uiFinishCount =0;

    for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
    {
        WinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
        WinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
        WinPerBit[u1BitIdx].win_center = (S16)PASS_RANGE_NA;
        FinalWinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
        FinalWinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
        FinalWinPerBit[u1BitIdx].win_center = (S16)PASS_RANGE_NA;
    }

    if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
        ucbit_num = 4;
    else
        ucbit_num = DQS_BIT_NUMBER;

    // Adjust DQM output delay to 0
    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/ucbit_num); u1ByteIdx++)
    {
        Set_RX_DQM_DelayLine_Phy_Byte(p, u1ByteIdx, 0);
    }

    // Adjust DQ output delay to 0
    //every 2bit dq have the same delay register address


    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/ucbit_num); u1ByteIdx++)
    {
        Set_RX_DQ_DelayLine_Phy_Byte(p, u1ByteIdx, dl_value);
    }

    if(p->frequency >= DDR_DDR1600)
    {
        u4DelayBegin= -48;
    }
    else if(p->frequency >= DDR_DDR1333)
    {
        u4DelayBegin= -70;
    }
    else if(p->frequency >= DDR_DDR1066)
    {
        u4DelayBegin= -96;
    }
    else
    {
        u4DelayBegin= -MAX_RX_DQSDLY_TAPS;
    }

    if (p->rx_eye_scan == ENABLE)
        u4DelayEnd = 0;
    else
        u4DelayEnd = MAX_RX_DQDLY_TAPS;

    u4DelayStep =1;

    for (iDelay=u4DelayBegin; iDelay<=u4DelayEnd; iDelay+= u4DelayStep)
    {
        SetRxDqDqsDelay(p, iDelay);

        if(u1UseTestEngine)
        {
            u4err_value = TestEngineCompare(p);
        }

        // if(u4err_value != 0  || iDelay == u4DelayBegin)
        if(iDelay <= -10)
        {
   	       //-10
            mcSHOW_DBG_MSG2(("%d, [0]", iDelay));
        }
        else if((iDelay > -10) && (iDelay < 0))
        {
            //-9
            mcSHOW_DBG_MSG2(("%d , [0]", iDelay));
        }
        else if((iDelay >= 0) && (iDelay < 10))
        {
            //9
            mcSHOW_DBG_MSG2((" %d , [0]", iDelay));
        }
        else
        {//10
            mcSHOW_DBG_MSG2(("%d , [0]", iDelay));
        }

        //mcSHOW_DBG_MSG2(("u4err_value %x, u1MRRValue %x\n", u4err_value, u1MRRValue));

        // check fail bit ,0 ok ,others fail
        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            u4fail_bit = u4err_value&((U32)1<<u1BitIdx);

            if(WinPerBit[u1BitIdx].first_pass== PASS_RANGE_NA)
            {
                if(u4fail_bit==0) //compare correct: pass
                {
                    WinPerBit[u1BitIdx].first_pass = iDelay;
                }
            }
            else if(WinPerBit[u1BitIdx].last_pass == PASS_RANGE_NA)
            {
                //mcSHOW_DBG_MSG(("fb%d \n", u4fail_bit));

                if(u4fail_bit !=0) //compare error : fail
                {
                    WinPerBit[u1BitIdx].last_pass  = (iDelay-1);
                }
                else if (iDelay==u4DelayEnd)
                {
                    WinPerBit[u1BitIdx].last_pass  = iDelay;
                }

                if(WinPerBit[u1BitIdx].last_pass  !=PASS_RANGE_NA)
                {
                    if((WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass) >= (FinalWinPerBit[u1BitIdx].last_pass -FinalWinPerBit[u1BitIdx].first_pass))
                    {
                        #if 0 //for debug
                        if(FinalWinPerBit[u1BitIdx].last_pass != PASS_RANGE_NA)
                        {
                            mcSHOW_DBG_MSG2(("Bit[%d] Bigger window update %d > %d\n", u1BitIdx, \
                                (WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass), (FinalWinPerBit[u1BitIdx].last_pass -FinalWinPerBit[u1BitIdx].first_pass)));
                        }
                        #endif
                        uiFinishCount |= (1<<u1BitIdx);
                        //update bigger window size
                        FinalWinPerBit[u1BitIdx].first_pass = WinPerBit[u1BitIdx].first_pass;
                        FinalWinPerBit[u1BitIdx].last_pass = WinPerBit[u1BitIdx].last_pass;
                    }
                    //reset tmp window
                    WinPerBit[u1BitIdx].first_pass = PASS_RANGE_NA;
                    WinPerBit[u1BitIdx].last_pass = PASS_RANGE_NA;
                }
            }

          //  if(u4err_value != 0 || iDelay == u4DelayBegin)
            {
                if(u1BitIdx%DQS_BIT_NUMBER ==0)
                {
                    mcSHOW_DBG_MSG2((" "));
                }

                if (u4fail_bit == 0)
                {
                    mcSHOW_DBG_MSG2(("o"));
                }
                else
                {
                    mcSHOW_DBG_MSG2(("x"));
                }
            }
        }

       // if(u4err_value != 0 || iDelay == u4DelayBegin)
        {
            mcSHOW_DBG_MSG2((" [MSB]\n"));
        }

    //if all bits widnow found and all bits turns to fail again, early break;
    	if(((p->data_width== DATA_WIDTH_16BIT) &&(uiFinishCount == 0xffff)) || \
			((p->data_width== DATA_WIDTH_32BIT) &&(uiFinishCount == 0xffffffff)))
        {
            if(u1UseTestEngine)
                vSetCalibrationResult(p, DRAM_CALIBRATION_RX_PERBIT, DRAM_OK);
            {
       		 	if(((p->data_width== DATA_WIDTH_16BIT) &&((u4err_value&0xffff) == 0xffff)) || \
					((p->data_width== DATA_WIDTH_32BIT) &&(u4err_value == 0xffffffff)))
                {
                        mcSHOW_DBG_MSG2(("\nRX all bits window found, early break!\n"));
                        break;  //early break
                }
             }
        }
    }

    for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
    {
        u2WinSize = FinalWinPerBit[u1BitIdx].last_pass-FinalWinPerBit[u1BitIdx].first_pass;

        u2TempWinSum += u2WinSize;  //Sum of CA Windows for vref selection

        if(u2WinSize<u2MinWinSize)
        {
            u2MinWinSize = u2WinSize;
            u1MinWinSizeBitidx = u1BitIdx;
        }
    }

    #ifdef ENABLE_CALIBRATION_WINDOW_LOG_FOR_FT
    mcSHOW_DBG_MSG(("FT log: RX min window : bit %d, size %d\n", u1MinWinSizeBitidx, u2MinWinSize));
    if (u2MinWinSize < 15)
    {
        mcSHOW_DBG_MSG(("FT log: RX error bit%d window:%d is too small!!\n", u1MinWinSizeBitidx, u2MinWinSize));
        ASSERT(0);
    }
    #endif

    mcSHOW_DBG_MSG(("RX Window Sum %d\n", u2TempWinSum));

    for (u1BitIdx=0; u1BitIdx<p->data_width; u1BitIdx++)
    {
        FinalWinPerBit[u1BitIdx].win_center = (FinalWinPerBit[u1BitIdx].last_pass + FinalWinPerBit[u1BitIdx].first_pass)>>1;     // window center of each DQ bit
        FinalWinPerBit[u1BitIdx].win_size = FinalWinPerBit[u1BitIdx].last_pass - FinalWinPerBit[u1BitIdx].first_pass + 1;     // window size of each DQ bit

        mcSHOW_DBG_MSG(("R%d FINAL: RX Bit %d, %d (%d ~ %d) %d\n", p->rank, u1BitIdx, FinalWinPerBit[u1BitIdx].win_center, FinalWinPerBit[u1BitIdx].first_pass, FinalWinPerBit[u1BitIdx].last_pass, FinalWinPerBit[u1BitIdx].win_size));

        #ifdef ENABLE_CALIBRATION_WINDOW_LOG_FOR_FT
        if (FinalWinPerBit[u1BitIdx].win_center < 0)
        {
            mcSHOW_DBG_MSG(("FT log: RX error bit%d center:%d < 0!!\n", u1BitIdx, FinalWinPerBit[u1BitIdx].win_center));
            ASSERT(0);
        }
        #endif
    }

    if (p->rx_eye_scan == ENABLE)
    {
        if (u4IO32ReadFldAlign(DDRPHY_SHU1_B0_DQ5, SHU1_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0) > 0xE)
        {
            memcpy(RxWinPerBit[0], FinalWinPerBit, sizeof(PASS_WIN_DATA_T) * DQ_DATA_WIDTH);
        }
        else
        {
            memcpy(RxWinPerBit[1], FinalWinPerBit, sizeof(PASS_WIN_DATA_T) * DQ_DATA_WIDTH);
        }

        return DRAM_OK;
    }

    // 3
    //As per byte, check max DQS delay in 8-bit. Except for the bit of max DQS delay, delay DQ to fulfill setup time = hold time
    for (u1ByteIdx = 0; u1ByteIdx < (p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
    {
        u2TmpDQMSum =0;

        ucbit_first =DQS_BIT_NUMBER*u1ByteIdx;
        ucbit_last = DQS_BIT_NUMBER*u1ByteIdx+DQS_BIT_NUMBER-1;
        iDQSDlyPerbyte[u1ByteIdx] = MAX_RX_DQSDLY_TAPS;

        for (u1BitIdx = ucbit_first; u1BitIdx <= ucbit_last; u1BitIdx++)
        {
            // find out max Center value
            if(FinalWinPerBit[u1BitIdx].win_center < iDQSDlyPerbyte[u1ByteIdx])
                iDQSDlyPerbyte[u1ByteIdx] = FinalWinPerBit[u1BitIdx].win_center;

            //mcSHOW_DBG_MSG(("bit#%2d : center=(%2d)\n", u1BitIdx, FinalWinPerBit[u1BitIdx].win_center));
        }

        //mcSHOW_DBG_MSG(("----seperate line----\n"));

        if (iDQSDlyPerbyte[u1ByteIdx]  > 0)  // Delay DQS=0, Delay DQ only
        {
            iDQSDlyPerbyte[u1ByteIdx]  = 0;
        }
        else  //Need to delay DQS
        {
            iDQSDlyPerbyte[u1ByteIdx]  = -iDQSDlyPerbyte[u1ByteIdx] ;
        }

        // we delay DQ or DQS to let DQS sample the middle of rx pass window for all the 8 bits,
        for (u1BitIdx = ucbit_first; u1BitIdx <= ucbit_last; u1BitIdx++)
        {
            FinalWinPerBit[u1BitIdx].best_dqdly = iDQSDlyPerbyte[u1ByteIdx] + FinalWinPerBit[u1BitIdx].win_center;
            u2TmpDQMSum += FinalWinPerBit[u1BitIdx].best_dqdly;
        }

        // calculate DQM as average of 8 DQ delay
        iDQMDlyPerbyte[u1ByteIdx] = u2TmpDQMSum/DQS_BIT_NUMBER;
    }


    vPrintCalibrationBasicInfo(p);
    mcSHOW_DBG_MSG(("DQS Delay :\nDQS0 = %d, DQS1 = %d, DQS2 = %d, DQS3 = %d\n", iDQSDlyPerbyte[0], iDQSDlyPerbyte[1], iDQSDlyPerbyte[2], iDQSDlyPerbyte[3]));
    mcSHOW_DBG_MSG(("DQM Delay :\nDQM0 = %d, DQM1 = %d, DQM2 = %d, DQM3 = %d\n", iDQMDlyPerbyte[0], iDQMDlyPerbyte[1], iDQMDlyPerbyte[2], iDQMDlyPerbyte[3]));
    mcSHOW_DBG_MSG(("DQ Delay :\n"));

    for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx=u1BitIdx+4)
    {
        mcSHOW_DBG_MSG(("DQ%d =%d, DQ%d =%d, DQ%d =%d, DQ%d =%d \n", u1BitIdx, FinalWinPerBit[u1BitIdx].best_dqdly, u1BitIdx+1, FinalWinPerBit[u1BitIdx+1].best_dqdly, u1BitIdx+2, FinalWinPerBit[u1BitIdx+2].best_dqdly, u1BitIdx+3, FinalWinPerBit[u1BitIdx+3].best_dqdly));
    }
    mcSHOW_DBG_MSG(("________________________________________________________________________\n"));

    // Set DQS & DQM delay
    /* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
#if SUPPORT_TYPE_DDR3X4
    if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
    {
        //swap B12 & B13
        mcSHOW_DBG_MSG(("DDR3X4 RX SWAP\n"));
        iDQSDlyPerbyte[2] = iDQSDlyPerbyte[1];
        iDQSDlyPerbyte[3] = iDQSDlyPerbyte[1];
        iDQMDlyPerbyte[2] = iDQMDlyPerbyte[1];
        iDQMDlyPerbyte[3] = iDQMDlyPerbyte[1];
        memcpy(FinalWinPerBit+16, FinalWinPerBit+8, sizeof(PASS_WIN_DATA_T) * DQS_BIT_NUMBER);
        memcpy(FinalWinPerBit+24, FinalWinPerBit+8, sizeof(PASS_WIN_DATA_T) * DQS_BIT_NUMBER);

        mcSHOW_DBG_MSG(("DDR3X4 DQS Delay :\nDQS0 = %d, DQS1 = %d, DQS2 = %d, DQS3 = %d\n", iDQSDlyPerbyte[0], iDQSDlyPerbyte[1], iDQSDlyPerbyte[2], iDQSDlyPerbyte[3]));
        mcSHOW_DBG_MSG(("DDR3X4 DQM Delay :\nDQM0 = %d, DQM1 = %d, DQM2 = %d, DQM3 = %d\n", iDQMDlyPerbyte[0], iDQMDlyPerbyte[1], iDQMDlyPerbyte[2], iDQMDlyPerbyte[3]));
        mcSHOW_DBG_MSG(("DDR3X4 DQ Delay :\n"));

        for (u1BitIdx = 16; u1BitIdx < DQ_DATA_WIDTH; u1BitIdx=u1BitIdx+4)
        {
            mcSHOW_DBG_MSG(("DDR3X4 DQ%d =%d, DQ%d =%d, DQ%d =%d, DQ%d =%d \n", u1BitIdx, FinalWinPerBit[u1BitIdx].best_dqdly, u1BitIdx+1, FinalWinPerBit[u1BitIdx+1].best_dqdly, u1BitIdx+2, FinalWinPerBit[u1BitIdx+2].best_dqdly, u1BitIdx+3, FinalWinPerBit[u1BitIdx+3].best_dqdly));
        }
    }
#endif

    for(ii=p->rank; ii<RANK_MAX; ii++)
    {
        vSetRank(p,ii);

        for (u1ByteIdx = 0; u1ByteIdx < (p->data_width/ucbit_num); u1ByteIdx++)
        {

        	Set_RX_DQM_DelayLine_Phy_Byte(p, u1ByteIdx,(U32)iDQMDlyPerbyte[u1ByteIdx]);
        }

        //B0 DQS
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ6, P_Fld(iDQSDlyPerbyte[0], SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0)
        			| P_Fld(iDQSDlyPerbyte[0], SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0));
        //B1 DQS
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ6, P_Fld(iDQSDlyPerbyte[1], SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_F_DLY_B1)
        			| P_Fld(iDQSDlyPerbyte[1], SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1));
        //B2 DQS
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ6+(1<<POS_BANK_NUM), P_Fld(iDQSDlyPerbyte[2], SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0)
        			| P_Fld(iDQSDlyPerbyte[2], SHU1_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0));
        //B3 DQS
        vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ6+(1<<POS_BANK_NUM), P_Fld(iDQSDlyPerbyte[3], SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_F_DLY_B1)
        			| P_Fld(iDQSDlyPerbyte[3], SHU1_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1));
    }
    vSetRank(p, backup_rank);

    DramPhyReset(p);

    // set dq delay

    if (p->en_4bitMux == ENABLE)
    {
         mcSHOW_DBG_MSG(("Rx 4bitMux is enabled\n"));

#if 1
        for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            mcSHOW_DBG_MSG(("%d ", FinalWinPerBit[u1BitIdx].best_dqdly));
        }
        mcSHOW_DBG_MSG(("\n"));
#endif

        for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            memcpy(&WinPerBit[Bit_DQ_Mapping[u1BitIdx]], &FinalWinPerBit[u1BitIdx], sizeof(PASS_WIN_DATA_T));
        }
        memcpy(FinalWinPerBit, WinPerBit, sizeof(PASS_WIN_DATA_T) * DQ_DATA_WIDTH);

#if 1
        for(u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            mcSHOW_DBG_MSG(("%d ", FinalWinPerBit[u1BitIdx].best_dqdly));
        }
        mcSHOW_DBG_MSG(("\n"));
#endif
    }
    /* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
    for(ii=p->rank; ii<RANK_MAX; ii++)
    {
        vSetRank(p,ii);

        //every 2bit dq have the same delay register address
        for (u1ByteIdx=0; u1ByteIdx<(p->data_width/ucbit_num); u1ByteIdx++)
        {
                u1BitIdx = u1ByteIdx *DQS_BIT_NUMBER;

                dl_value[0] = FinalWinPerBit[u1BitIdx].best_dqdly;
                dl_value[1] = FinalWinPerBit[u1BitIdx+1].best_dqdly;
                dl_value[2] = FinalWinPerBit[u1BitIdx+2].best_dqdly;
                dl_value[3] = FinalWinPerBit[u1BitIdx+3].best_dqdly;
                dl_value[4] = FinalWinPerBit[u1BitIdx+4].best_dqdly;
                dl_value[5] = FinalWinPerBit[u1BitIdx+5].best_dqdly;
                dl_value[6] = FinalWinPerBit[u1BitIdx+6].best_dqdly;
                dl_value[7] = FinalWinPerBit[u1BitIdx+7].best_dqdly;

             Set_RX_DQ_DelayLine_Phy_Byte(p, u1ByteIdx, dl_value);
        }
    }
    vSetRank(p, backup_rank);


    mcSHOW_DBG_MSG(("[DramcRxWindowPerbitCal] ====Done====\n"));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQS0_delay = iDQSDlyPerbyte[0];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQS1_delay = iDQSDlyPerbyte[1];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQS2_delay = iDQSDlyPerbyte[2];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQS3_delay = iDQSDlyPerbyte[3];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQM0_delay = iDQMDlyPerbyte[0];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQM1_delay = iDQMDlyPerbyte[1];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQM2_delay = iDQMDlyPerbyte[2];
    gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQM3_delay = iDQMDlyPerbyte[3];

    for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
    {
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].first_pass = FinalWinPerBit[u1BitIdx].first_pass;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].last_pass = FinalWinPerBit[u1BitIdx].last_pass;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].win_size = FinalWinPerBit[u1BitIdx].win_size;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].win_center = FinalWinPerBit[u1BitIdx].win_center;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].left_margin= FinalWinPerBit[u1BitIdx].win_center - FinalWinPerBit[u1BitIdx].first_pass;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.WinPerBit[u1BitIdx].right_margin= FinalWinPerBit[u1BitIdx].last_pass - FinalWinPerBit[u1BitIdx].win_center;
        gDRAM_CALIB_LOG.RANK[p->rank].RxWindowPerbitCal.DQ_delay[u1BitIdx] = FinalWinPerBit[u1BitIdx].best_dqdly;
    }
#endif

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =0;
#endif
    p->min_winsize = u2MinWinSize;
    p->sum_winsize = u2TempWinSum;

    return DRAM_OK;
}
#endif //SIMULATION_RX_PERBIT

#if SIMULATION_DATLAT
static void dle_factor_handler(DRAMC_CTX_T *p, U8 curr_val)
{
#if REG_ACCESS_PORTING_DGB
    RegLogEnable =1;
    mcSHOW_DBG_MSG(("\n[REG_ACCESS_PORTING_FUNC]   dle_factor_handler\n"));
#endif

    if(curr_val<4)
        curr_val =4;

    // Ohympus/Elbrus: Datlat_dsel = datlat -1, only 1 TX pipe
    //MT8167  Datlat_dsel = datlat -4
    vIO32WriteFldMulti(DRAMC_REG_SHU_CONF1,
                                        P_Fld(curr_val, SHU_CONF1_DATLAT) |
                                        P_Fld(curr_val -4, SHU_CONF1_DATLAT_DSEL) |
                                        P_Fld(curr_val -4, SHU_CONF1_DATLAT_DSEL_PHY));

    DramPhyReset(p);

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =0;
#endif
}

//-------------------------------------------------------------------------
/** Dramc_ta2_rx_scans
 */
//-------------------------------------------------------------------------
static U32 Dramc_ta2_rx_scan(DRAMC_CTX_T *p)
{
    U32 u4err_value = 0xffffffff;
    S16 iDelay;
    U8 u1ByteIdx;

    U8 dl_value[8]={0,0,0,0,0,0,0,0};

    // Adjust DQM output delay to 0
    for(u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
    {
        Set_RX_DQM_DelayLine_Phy_Byte(p, u1ByteIdx, 0);
    }

    // Adjust DQ output delay to 0
    //every 2bit dq have the same delay register address
    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
    {
        Set_RX_DQ_DelayLine_Phy_Byte(p, u1ByteIdx, dl_value);
    }

   // quick rx dqs search
   //mcSHOW_DBG_MSG(("quick rx dqs search\n"));
   for (iDelay=-32; iDelay<=32; iDelay+=4)
   {
       //mcSHOW_DBG_MSG(("%2d, ", iDelay));

       SetRxDqDqsDelay(p, iDelay);
       u4err_value = TestEngineCompare(p);

       if(u4err_value ==0)// rx dqs found.
            break;
    }

    mcSHOW_DBG_MSG(("RX DQS delay = %d, ", iDelay));

    return u4err_value;
}


static U8 aru1RxDatlatResult[RANK_MAX];

U8 DramcRxdatlatScan(DRAMC_CTX_T *p, DRAM_DATLAT_CALIBRATION_TYTE_T use_rxtx_scan)
{
    U8 ii;
    U32 u4prv_register_080;
    U32 u4err_value= 0xffffffff;
    U8 ucfirst, ucbegin, ucsum, ucbest_step;

    // error handling
    if (!p)
    {
        mcSHOW_ERR_MSG(("context is NULL\n"));
        return DRAM_FAIL;
    }
#if REG_ACCESS_PORTING_DGB
    RegLogEnable =1;
    mcSHOW_DBG_MSG(("\n[REG_ACCESS_PORTING_FUNC]   DramcRxdatlatCal\n"));
#endif
	mcSHOW_DBG_MSG(("\n[DATLAT]\n"));
    mcSHOW_DBG_MSG2(("==============================================================\n"));
    mcSHOW_DBG_MSG2((" Frequency=%d, Rank=%d, use_rxtx_scan=%d\n", p->frequency, p->rank, use_rxtx_scan));
    mcSHOW_DBG_MSG2(("==============================================================\n"));

    // [11:10] DQIENQKEND 01 -> 00 for DATLAT calibration issue, DQS input enable will refer to DATLAT
    // if need to enable this (for power saving), do it after all calibration done
    //u4prv_register_0d8 = u4IO32Read4B(DRAMC_REG_MCKDLY);
    //vIO32WriteFldMulti(DRAMC_REG_PADCTRL, P_Fld(0, PADCTRL_DQIENQKEND) | P_Fld(0, PADCTRL_DQIENLATEBEGIN));

    // pre-save
    // 0x07c[6:4]   DATLAT bit2-bit0
    u4prv_register_080 = u4IO32Read4B(DRAMC_REG_SHU_CONF1);

    // init best_step to default
    ucbest_step = (U8) u4IO32ReadFldAlign(DRAMC_REG_SHU_CONF1, SHU_CONF1_DATLAT);
    mcSHOW_DBG_MSG(("DATLAT Default value = 0x%x\n", ucbest_step));

    // 1.set DATLAT 0-15 (0-21 for MT6595)
    // 2.enable engine1 or engine2
    // 3.check result  ,3~4 taps pass
    // 4.set DATLAT 2nd value for optimal

    // Initialize
    ucfirst = 0xff;
    ucbegin = 0;
    ucsum = 0;

    for (ii = 5; ii < DATLAT_TAP_NUMBER; ii++)
    {
        // 1
        dle_factor_handler(p, ii);

        // 2
        if(use_rxtx_scan == fcDATLAT_USE_DEFAULT)
        {
            u4err_value = TestEngineCompare(p);
        }
        else  //if(use_rxtx_scan == fcDATLAT_USE_RX_SCAN)//LPDDR3, LP4 Should not enter if datlat calibration is after RDDQC
        {
            u4err_value = Dramc_ta2_rx_scan(p);
        }

        // 3
        if(p->data_width == DATA_WIDTH_16BIT)
        {
            u4err_value = u4err_value & 0x0000FFFF;
        }

        if (u4err_value == 0)
        {
            if (ucbegin == 0)
            {
                // first tap which is pass
                ucfirst = ii;
                ucbegin = 1;
            }
            if (ucbegin == 1)
            {
                ucsum++;

                if(ucsum >5)
                    break;  //early break.
            }
        }
        else
        {
            if (ucbegin == 1)
            {
                // pass range end
                ucbegin = 0xff;
            }
        }

        mcSHOW_DBG_MSG(("%d, 0x%X, sum=%d\n", ii, u4err_value, ucsum));

    }

    // 4
    if (ucsum == 0)
    {
        mcSHOW_ERR_MSG(("no DATLAT taps pass, DATLAT calibration fail!!\n"));
    }
    else if (ucsum <= 3)
    {
        ucbest_step = ucfirst + (ucsum>>1);
    }
    else // window is larger htan 3
    {
        ucbest_step = ucfirst + 2;
    }

    aru1RxDatlatResult[p->rank] = ucbest_step;

    mcSHOW_DBG_MSG(("pattern=%d first_step=%d total pass=%d best_step=%d\n", p->test_pattern, ucfirst, ucsum, ucbest_step));
    mcSHOW_DBG_MSG(("R%d FINAL: DATLAT = %d [%d ~ %d]\n", p->rank, ucbest_step, ucfirst, ucfirst+ucsum-1));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    gDRAM_CALIB_LOG.RANK[p->rank].DATLAT.best_step = ucbest_step;
#endif

    if(ucsum <5)
    {
        mcSHOW_DBG_MSG2(("NOTICE]DatlatSum %d\n", ucsum));
    }

    if (ucsum == 0)
    {
        mcSHOW_ERR_MSG(("DATLAT calibration fail, write back to default values!\n"));
        vIO32Write4B(DRAMC_REG_SHU_CONF1, u4prv_register_080);
        vSetCalibrationResult(p, DRAM_CALIBRATION_DATLAT, DRAM_FAIL);
    }
    else
    {
        dle_factor_handler(p, ucbest_step);
        vSetCalibrationResult(p, DRAM_CALIBRATION_DATLAT, DRAM_OK);
    }

    // [11:10] DQIENQKEND 01 -> 00 for DATLAT calibration issue, DQS input enable will refer to DATLAT
    // if need to enable this (for power saving), do it after all calibration done
    vIO32WriteFldMulti(DRAMC_REG_PADCTRL, P_Fld(1, PADCTRL_DQIENQKEND) | P_Fld(1, PADCTRL_DQIENLATEBEGIN));

    mcSHOW_DBG_MSG(("[DramcRxdatlatCal] ====Done====\n"));

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =0;
#endif

    return ucsum;
}

void DramcRxdatlatCal(DRAMC_CTX_T *p)
{
    U8 u1DatlatWindowSum;

    u1DatlatWindowSum = DramcRxdatlatScan(p, fcDATLAT_USE_DEFAULT);

    if(u1DatlatWindowSum <5)
    {
        mcSHOW_DBG_MSG(("\nu1DatlatWindowSum %d is too small(<5), Start RX + Datlat scan\n", u1DatlatWindowSum));
        DramcRxdatlatScan(p, fcDATLAT_USE_RX_SCAN);
    }
}

DRAM_STATUS_T DramcDualRankRxdatlatCal(DRAMC_CTX_T *p)
{
    U8 u1FinalDatlat, u1Datlat0, u1Datlat1;

    u1Datlat0 = aru1RxDatlatResult[0];
    u1Datlat1 = aru1RxDatlatResult[1];

    if(u1Datlat0> u1Datlat1)
    {
        u1FinalDatlat= u1Datlat0;
    }
    else
    {
        u1FinalDatlat= u1Datlat1;
    }

    dle_factor_handler(p, u1FinalDatlat);
    mcSHOW_DBG_MSG(("[DramcDualRankRxdatlatCal] RANK0: %d, RANK1: %d, Final_Datlat %d\n", u1Datlat0, u1Datlat1, u1FinalDatlat));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
    gDRAM_CALIB_LOG.DualRankRxdatlatCal.Rank0_Datlat = u1Datlat0;
    gDRAM_CALIB_LOG.DualRankRxdatlatCal.Rank1_Datlat = u1Datlat1;
    gDRAM_CALIB_LOG.DualRankRxdatlatCal.Final_Datlat = u1FinalDatlat;
#endif

    return DRAM_OK;

}
#endif //SIMULATION_DATLAT

#if SIMULATION_TX_PERBIT
//-------------------------------------------------------------------------
/** DramcTxWindowPerbitCal (v2)
 *  TX DQS per bit SW calibration.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @param  apply           (U8): 0 don't apply the register we set  1 apply the register we set ,default don't apply.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------
#define ENABLE_64_PI_TO_UI 1
#if ENABLE_64_PI_TO_UI
#define TX_DQ_UI_TO_PI_TAP         64 // 1 PI = tCK/64, total 128 PI, 1UI = 32 PI
#else
#define TX_DQ_UI_TO_PI_TAP         32 // 1 PI = tCK/64, total 128 PI, 1UI = 32 PI
#endif

#define TX_VREF_RANGE_BEGIN       0
#define TX_VREF_RANGE_END           50 // binary 110010
#define TX_VREF_RANGE_STEP         2

static void TxWinTransferDelayToUIPI(DRAMC_CTX_T *p, U16 uiDelay, U8 u1UISmallBak, U8 u1UILargeBak, U8* pu1UILarge, U8* pu1UISmall, U8* pu1PI)
{
    U8 u1Small_ui_to_large;
    U16 u2TmpValue;

    //in LP4, 8 small UI =  1 large UI
    //in LP3, 4 small UI =  1 large UI
    //LPDDR3
        u1Small_ui_to_large =  2;

    #if 0
    *pu1PI = uiDelay% TX_DQ_UI_TO_PI_TAP;
    #else
    *pu1PI = uiDelay & (TX_DQ_UI_TO_PI_TAP-1);
    #endif

    #if ENABLE_64_PI_TO_UI
    u2TmpValue = (uiDelay /TX_DQ_UI_TO_PI_TAP)*2 +u1UISmallBak;
    #else
    u2TmpValue = uiDelay /TX_DQ_UI_TO_PI_TAP +u1UISmallBak;
    #endif

    #if 0
    *pu1UISmall = u2TmpValue % u1Small_ui_to_large;
    *pu1UILarge = u2TmpValue / u1Small_ui_to_large +u1UILargeBak;
    #else
    *pu1UISmall = u2TmpValue - ((u2TmpValue >> u1Small_ui_to_large) <<u1Small_ui_to_large);
    *pu1UILarge = (u2TmpValue >> u1Small_ui_to_large) +u1UILargeBak;
    #endif
}

#define TX_DQM_CALC_MAX_MIN_CENTER 1
#define TX_NEW_CENTER 1

DRAM_STATUS_T DramcTxWindowPerbitCal(DRAMC_CTX_T *p, DRAM_TX_PER_BIT_CALIBRATION_TYTE_T calType)
{
    U8 u1BitIdx, u1ByteIdx, u1SmallestDQSByte=0;
    U8 ucindex, ucbit_num;
    U32 uiFinishCount;
    PASS_WIN_DATA_T WinPerBit[DQ_DATA_WIDTH], VrefWinPerBit[DQ_DATA_WIDTH], FinalWinPerBit[DQ_DATA_WIDTH];

    U16 uiDelay, uiDelay2, u2DQDelayBegin=0, u2DQDelayEnd=0;
    U8 ucdq_pi, ucdq_ui_small, ucdq_ui_large,ucdq_oen_ui_small, ucdq_oen_ui_large;
    static U8 dq_ui_small_bak, dq_ui_large_bak,  dq_oen_ui_small_bak, dq_oen_ui_large_bak;
    U8 ucdq_final_pi[DQS_NUMBER], ucdq_final_ui_large[DQS_NUMBER], ucdq_final_ui_small[DQS_NUMBER];
    U8 ucdq_final_oen_ui_large[DQS_NUMBER], ucdq_final_oen_ui_small[DQS_NUMBER];

    U16 u2Center_min[DQS_NUMBER],u2Center_max[DQS_NUMBER];
    #if !TX_DQM_CALC_MAX_MIN_CENTER
    S16 s1temp1, s1temp2;
    S16 s2sum_dly[DQS_NUMBER];
    #endif
    U32 u4err_value, u4fail_bit;
    U16 u2TempWinSum;
    U32 u4TempRegValue;
    U16 u2MinWinSize = 0xffff;
    U8 u1MinWinSizeBitidx = 0;
    #if TX_NEW_CENTER
    U16 u2First_max[DQS_NUMBER] = {0,0,0,0};
    U16 u2Last_min[DQS_NUMBER] = {0xffff,0xffff,0xffff,0xffff};
    #endif
    U8 ii, backup_rank;
    #if SUPPORT_TX_DELAY_LINE_CALI
    U16 u2dqm_dly[DQS_NUMBER] = {0,0,0,0};
    #endif

    if (!p)
    {
        mcSHOW_ERR_MSG(("context is NULL\n"));
        return DRAM_FAIL;
    }

    for(u1ByteIdx = 0; u1ByteIdx < DQS_NUMBER; u1ByteIdx++)
        ucdq_final_pi[u1ByteIdx] = 0;

    backup_rank = u1GetRank(p);
	mcSHOW_DBG_MSG(("\n[TX]\n"));

    if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
        ucbit_num = 4;
    else
        ucbit_num = DQS_BIT_NUMBER;
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
        for(u1ByteIdx=0; u1ByteIdx<(p->data_width/ucbit_num); u1ByteIdx++)
        {
            if(wrlevel_dqs_final_delay[u1ByteIdx] < u2DQDelayBegin)
            {
                u2DQDelayBegin = wrlevel_dqs_final_delay[u1ByteIdx];
                u1SmallestDQSByte = u1ByteIdx;
            }
        }
    }
    else
    {
        u1SmallestDQSByte =0;
    }

    if(p->fgTXPerbifInit[p->rank]== FALSE)
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

        p->fgTXPerbifInit[p->rank]= TRUE;
    }

    u2DQDelayBegin = wrlevel_dqs_final_delay[u1SmallestDQSByte];
    if (u2DQDelayBegin >= 0x40)
        u2DQDelayBegin -= 0x40;
    u2DQDelayEnd = u2DQDelayBegin +96; //Scan at least 1UI. Scan 64 to cover byte differece. if window found, early break.

    vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_FAIL);

    mcSHOW_DBG_MSG(("[DramcTxWindowPerbitCal] Frequency=%d, Rank=%d, calType=%d\n", p->frequency, p->rank, calType));
    mcSHOW_DBG_MSG(("[DramcTxWindowPerbitCal] Begin, TX DQ(%d, %d),  DQ OEN(%d, %d)\n", dq_ui_large_bak, dq_ui_small_bak,dq_oen_ui_large_bak,  dq_oen_ui_small_bak));

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

#if SUPPORT_TX_DELAY_LINE_CALI
    if (p->frequency >= DDR_DDR1333)
    {
        for(ii=p->rank; ii<RANK_MAX; ii++)
        {
            vSetRank(p,ii);
            vIO32WriteFldAlign_All(DDRPHY_SHU1_R0_B0_DQ1, 0, SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);
            vIO32WriteFldAlign_All(DDRPHY_SHU1_R0_B1_DQ1, 0, SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);
            vIO32Write4B_All(DDRPHY_SHU1_R0_B0_DQ0, 0);
            vIO32Write4B_All(DDRPHY_SHU1_R0_B1_DQ0, 0);
        }
        vSetRank(p, backup_rank);
    }
#endif
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
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, \
                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ0) | \
                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ1) | \
                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ2) | \
                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ3) | \
                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) | \
                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) | \
                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) | \
                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3));

            // DLY_DQ[2:0]
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, \
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
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, \
                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM0) | \
                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM1) | \
                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM2) | \
                                P_Fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM3) | \
                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) | \
                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) | \
                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) | \
                                P_Fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3));

            // DLY_DQM[2:0]
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ3, \
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

        //mcSHOW_DBG_MSG(("Delay=%3d |%2d %2d %3d| %2d %2d| 0x%8x [0]",uiDelay, ucdq_ui_large,ucdq_ui_small, ucdq_pi, ucdq_oen_ui_large,ucdq_oen_ui_small, u4err_value));
        if(u4err_value != 0 || uiDelay == u2DQDelayBegin)
        {
            mcSHOW_DBG_MSG2(("%d |%d %d %d|[0]",uiDelay, ucdq_ui_large,ucdq_ui_small, ucdq_pi));
        }

        // check fail bit ,0 ok ,others fail
        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            u4fail_bit = u4err_value&((U32)1<<u1BitIdx);

            if(u4err_value != 0 || uiDelay == u2DQDelayBegin)
            {
                if(u1BitIdx%DQS_BIT_NUMBER ==0)
                {
                    mcSHOW_DBG_MSG2((" "));
                }

                if (u4fail_bit == 0)
                {
                    mcSHOW_DBG_MSG2(("o"));
                }
                else
                {
                    mcSHOW_DBG_MSG2(("x"));
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

        if(u4err_value != 0 || uiDelay == u2DQDelayBegin)
        {
            mcSHOW_DBG_MSG2((" [MSB]\n"));
        }

        //if all bits widnow found and all bits turns to fail again, early break;
        if(((p->data_width== DATA_WIDTH_16BIT) &&(uiFinishCount == 0xffff)) || \
        ((p->data_width== DATA_WIDTH_32BIT) &&(uiFinishCount == 0xffffffff)))
        {
            vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_OK);

            if(((p->data_width== DATA_WIDTH_16BIT) &&((u4err_value&0xffff) == 0xffff)) || \
			((p->data_width== DATA_WIDTH_32BIT) &&(u4err_value == 0xffffffff)))
            {
                    mcSHOW_DBG_MSG2(("TX calibration finding left boundary early break. PI DQ delay=0x%B\n", uiDelay));
                break;  //early break
            }
        }
    }

    for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
    {
        VrefWinPerBit[u1BitIdx].win_size = VrefWinPerBit[u1BitIdx].last_pass- VrefWinPerBit[u1BitIdx].first_pass +1;

        if(VrefWinPerBit[u1BitIdx].win_size<u2MinWinSize)
        {
            u2MinWinSize = VrefWinPerBit[u1BitIdx].win_size;
            u1MinWinSizeBitidx = u1BitIdx;
        }

        u2TempWinSum += VrefWinPerBit[u1BitIdx].win_size;  //Sum of CA Windows for vref selection
    }
    mcSHOW_DBG_MSG2(("Fra Min Bit=%d, winsize=%d ===\n",u1MinWinSizeBitidx, u2MinWinSize));

    #ifdef ENABLE_CALIBRATION_WINDOW_LOG_FOR_FT
    mcSHOW_DBG_MSG(("FT log: TX min window : bit %d, size %d\n", u1MinWinSizeBitidx, u2MinWinSize));
    #endif

    mcSHOW_DBG_MSG(("TX Window Sum %d\n", u2TempWinSum));

    //Calculate the center of DQ pass window
    // Record center sum of each byte
    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
    {
        u2Center_min[u1ByteIdx] = 0xffff;
        u2Center_max[u1ByteIdx] = 0;
        #if !TX_DQM_CALC_MAX_MIN_CENTER
        s2sum_dly[u1ByteIdx] = 0;
        #endif

        #if TX_NEW_CENTER
        u2Last_min[u1ByteIdx] = 0xffff;
        u2First_max[u1ByteIdx] = 0;
        #endif

        for (u1BitIdx=0; u1BitIdx<DQS_BIT_NUMBER; u1BitIdx++)
        {
            ucindex = u1ByteIdx * DQS_BIT_NUMBER + u1BitIdx;
            FinalWinPerBit[ucindex].first_pass = VrefWinPerBit[ucindex].first_pass;
            FinalWinPerBit[ucindex].last_pass =  VrefWinPerBit[ucindex].last_pass;
            FinalWinPerBit[ucindex].win_size = VrefWinPerBit[ucindex].win_size;
            FinalWinPerBit[ucindex].win_center = (FinalWinPerBit[ucindex].first_pass + FinalWinPerBit[ucindex].last_pass) >> 1;

            if(FinalWinPerBit[ucindex].win_center < u2Center_min[u1ByteIdx])
                u2Center_min[u1ByteIdx] = FinalWinPerBit[ucindex].win_center;

            if(FinalWinPerBit[ucindex].win_center > u2Center_max[u1ByteIdx])
                u2Center_max[u1ByteIdx] = FinalWinPerBit[ucindex].win_center;
            #if !TX_DQM_CALC_MAX_MIN_CENTER
            s2sum_dly[u1ByteIdx] += FinalWinPerBit[ucindex].win_center;
            #endif

            #if TX_NEW_CENTER
            if (FinalWinPerBit[ucindex].first_pass > u2First_max[u1ByteIdx])
                u2First_max[u1ByteIdx] = FinalWinPerBit[ucindex].first_pass;

            if (FinalWinPerBit[ucindex].last_pass < u2Last_min[u1ByteIdx])
                u2Last_min[u1ByteIdx] = FinalWinPerBit[ucindex].last_pass;
            #endif
        }
    }

    //mcSHOW_DBG_MSG(("==================================================================\n"));
    //mcSHOW_DBG_MSG(("PI DQ (per byte) window\nx=pass dq delay value (min~max)center \ny=0-7bit DQ of every group\n"));
    //mcSHOW_DBG_MSG(("==================================================================\n"));
    //mcSHOW_DBG_MSG(("bit    Byte0    bit      Byte1     bit     Byte2     bit     Byte3\n"));
    vPrintCalibrationBasicInfo(p);

    for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx++)
    {

        mcSHOW_DBG_MSG(("R%d FINAL: TX Bit%d (%d~%d) %d %d,   Bit%d (%d~%d) %d %d,", p->rank, \
            u1BitIdx, FinalWinPerBit[u1BitIdx].first_pass, FinalWinPerBit[u1BitIdx].last_pass, FinalWinPerBit[u1BitIdx].win_size, FinalWinPerBit[u1BitIdx].win_center, \
            u1BitIdx+8, FinalWinPerBit[u1BitIdx+8].first_pass, FinalWinPerBit[u1BitIdx+8].last_pass, FinalWinPerBit[u1BitIdx+8].win_size, FinalWinPerBit[u1BitIdx+8].win_center));

        if(p->data_width == DATA_WIDTH_32BIT)
        {

            mcSHOW_DBG_MSG(("  Bit%d (%d~%d) %d %d,   Bit%d (%d~%d) %d %d\n", \
                u1BitIdx+16, FinalWinPerBit[u1BitIdx+16].first_pass, FinalWinPerBit[u1BitIdx+16].last_pass, FinalWinPerBit[u1BitIdx+16].win_size, FinalWinPerBit[u1BitIdx+16].win_center, \
                u1BitIdx+24, FinalWinPerBit[u1BitIdx+24].first_pass, FinalWinPerBit[u1BitIdx+24].last_pass, FinalWinPerBit[u1BitIdx+24].win_size, FinalWinPerBit[u1BitIdx+24].win_center ));
        }
        else
        {
            mcSHOW_DBG_MSG(("\n"));
        }
    }
    mcSHOW_DBG_MSG(("\n==================================================================\n"));

    //Calculate the center of DQ pass window
    //average the center delay
    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
    {
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

        #if TX_NEW_CENTER
        uiDelay2 = (u2Last_min[u1ByteIdx] + u2First_max[u1ByteIdx]) >> 1;
        #else
        uiDelay2 = uiDelay;
        #endif

        #if SUPPORT_TX_DELAY_LINE_CALI
        if (p->frequency >= DDR_DDR1333)
        {
            uiDelay2 = u2Center_min[u1ByteIdx];
            u2dqm_dly[u1ByteIdx] = uiDelay;
        }
        #endif

        TxWinTransferDelayToUIPI(p, uiDelay2, dq_ui_small_bak, dq_ui_large_bak, &ucdq_final_ui_large[u1ByteIdx], &ucdq_final_ui_small[u1ByteIdx], &ucdq_final_pi[u1ByteIdx]);
        TxWinTransferDelayToUIPI(p, uiDelay2, dq_oen_ui_small_bak, dq_oen_ui_large_bak, &ucdq_final_oen_ui_large[u1ByteIdx], &ucdq_final_oen_ui_small[u1ByteIdx], &ucdq_final_pi[u1ByteIdx]);

        mcSHOW_DBG_MSG(("Byte%d, PI DQ Delay %d Delay2 %d\n", u1ByteIdx, uiDelay, uiDelay2));
        mcSHOW_DBG_MSG(("Final DQ PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n", ucdq_final_ui_large[u1ByteIdx], ucdq_final_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));
        mcSHOW_DBG_MSG(("OEN DQ PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n\n", ucdq_final_oen_ui_large[u1ByteIdx], ucdq_final_oen_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));

#if defined(DRAM_CALIB_LOG) || defined(DRAM_ETT)
        for (u1BitIdx = 0; u1BitIdx < DQS_BIT_NUMBER; u1BitIdx++)
        {
            gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.WinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].first_pass = FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].first_pass;
            gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.WinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].last_pass = FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].last_pass;
            gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.WinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].win_size = FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].win_size;
            gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.WinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].win_center = FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].win_center;
            gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.WinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].left_margin = uiDelay2 - FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].first_pass;  // left margin
            gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.WinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].right_margin = FinalWinPerBit[u1ByteIdx *DQS_BIT_NUMBER +u1BitIdx].last_pass - uiDelay2;  // right margin
        }
        gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.PI_DQ_delay[u1ByteIdx] = uiDelay2;
        gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.Large_UI[u1ByteIdx] = ucdq_final_ui_large[u1ByteIdx];
        gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.Small_UI[u1ByteIdx] = ucdq_final_ui_small[u1ByteIdx];
        gDRAM_CALIB_LOG.RANK[p->rank].TxWindowPerbitCal.PI[u1ByteIdx] = ucdq_final_pi[u1ByteIdx];
#endif
    }

#if REG_ACCESS_PORTING_DGB
    RegLogEnable =1;
#endif

#if SUPPORT_TYPE_DDR3X4
    if (p->pinmux == PIN_MUX_TYPE_DDR3X4)
    {
        //swap B12 & B13
        mcSHOW_DBG_MSG(("DDR3X4 TX SWAP\n"));
        ucdq_final_ui_large[2] = ucdq_final_ui_large[1];
        ucdq_final_ui_large[3] = ucdq_final_ui_large[1];
        ucdq_final_oen_ui_large[2] = ucdq_final_oen_ui_large[1];
        ucdq_final_oen_ui_large[3] = ucdq_final_oen_ui_large[1];
        ucdq_final_ui_small[2] = ucdq_final_ui_small[1];
        ucdq_final_ui_small[3] = ucdq_final_ui_small[1];
        ucdq_final_oen_ui_small[2] = ucdq_final_oen_ui_small[1];
        ucdq_final_oen_ui_small[3] = ucdq_final_oen_ui_small[1];
        ucdq_final_pi[2] = ucdq_final_pi[1];
        ucdq_final_pi[3] = ucdq_final_pi[1];

        for (u1ByteIdx=0; u1ByteIdx<(p->data_width/ucbit_num); u1ByteIdx++)
        {
            mcSHOW_DBG_MSG(("DDR3X4 Final DQM(DQ) PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n", ucdq_final_ui_large[u1ByteIdx], ucdq_final_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));
            mcSHOW_DBG_MSG(("DDR3X4 OEN DQM(DQ) PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n\n", ucdq_final_oen_ui_large[u1ByteIdx], ucdq_final_oen_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));
        }
    }
#endif

    /* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1, save to Reg Rank1 */
    for(ii=p->rank; ii<RANK_MAX; ii++)
    {
        vSetRank(p,ii);

        if(calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
        {
            //TXDLY_DQ , TXDLY_OEN_DQ
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, \
                                    P_Fld(ucdq_final_ui_large[0], SHURK0_SELPH_DQ0_TXDLY_DQ0) | \
                                    P_Fld(ucdq_final_ui_large[1], SHURK0_SELPH_DQ0_TXDLY_DQ1) | \
                                    P_Fld(ucdq_final_ui_large[2], SHURK0_SELPH_DQ0_TXDLY_DQ2) | \
                                    P_Fld(ucdq_final_ui_large[3], SHURK0_SELPH_DQ0_TXDLY_DQ3) | \
                                    P_Fld(ucdq_final_oen_ui_large[0], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) | \
                                    P_Fld(ucdq_final_oen_ui_large[1], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) | \
                                    P_Fld(ucdq_final_oen_ui_large[2], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) | \
                                    P_Fld(ucdq_final_oen_ui_large[3], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3));

             // DLY_DQ[2:0]
             vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, \
                                    P_Fld(ucdq_final_ui_small[0], SHURK0_SELPH_DQ2_DLY_DQ0) | \
                                    P_Fld(ucdq_final_ui_small[1], SHURK0_SELPH_DQ2_DLY_DQ1) | \
                                    P_Fld(ucdq_final_ui_small[2], SHURK0_SELPH_DQ2_DLY_DQ2) | \
                                    P_Fld(ucdq_final_ui_small[3], SHURK0_SELPH_DQ2_DLY_DQ3) | \
                                    P_Fld(ucdq_final_oen_ui_small[0], SHURK0_SELPH_DQ2_DLY_OEN_DQ0) | \
                                    P_Fld(ucdq_final_oen_ui_small[1], SHURK0_SELPH_DQ2_DLY_OEN_DQ1) | \
                                    P_Fld(ucdq_final_oen_ui_small[2], SHURK0_SELPH_DQ2_DLY_OEN_DQ2) | \
                                    P_Fld(ucdq_final_oen_ui_small[3], SHURK0_SELPH_DQ2_DLY_OEN_DQ3));
 
             // set to best values for  DQ
             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
        }

        if(calType ==TX_DQ_DQS_MOVE_DQM_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM)
        {
            //TXDLY_DQM , TXDLY_OEN_DQM
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ1, \
                                     P_Fld(ucdq_final_ui_large[0], SHURK0_SELPH_DQ1_TXDLY_DQM0) | \
                                     P_Fld(ucdq_final_ui_large[1], SHURK0_SELPH_DQ1_TXDLY_DQM1) | \
                                     P_Fld(ucdq_final_ui_large[2], SHURK0_SELPH_DQ1_TXDLY_DQM2) | \
                                     P_Fld(ucdq_final_ui_large[3], SHURK0_SELPH_DQ1_TXDLY_DQM3) | \
                                     P_Fld(ucdq_final_oen_ui_large[0], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) | \
                                     P_Fld(ucdq_final_oen_ui_large[1], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) | \
                                     P_Fld(ucdq_final_oen_ui_large[2], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) | \
                                     P_Fld(ucdq_final_oen_ui_large[3], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3));

            // DLY_DQM[2:0]
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ3, \
                                     P_Fld(ucdq_final_ui_small[0], SHURK0_SELPH_DQ3_DLY_DQM0) | \
                                     P_Fld(ucdq_final_ui_small[1], SHURK0_SELPH_DQ3_DLY_DQM1) | \
                                     P_Fld(ucdq_final_ui_small[2], SHURK0_SELPH_DQ3_DLY_DQM2) | \
                                     P_Fld(ucdq_final_ui_small[3], SHURK0_SELPH_DQ3_DLY_DQM3) | \
                                     P_Fld(ucdq_final_oen_ui_small[0], SHURK0_SELPH_DQ3_DLY_OEN_DQM0) | \
                                     P_Fld(ucdq_final_oen_ui_small[1], SHURK0_SELPH_DQ3_DLY_OEN_DQM1) | \
                                     P_Fld(ucdq_final_oen_ui_small[2], SHURK0_SELPH_DQ3_DLY_OEN_DQM2) | \
                                     P_Fld(ucdq_final_oen_ui_small[3], SHURK0_SELPH_DQ3_DLY_OEN_DQM3));

            // set to best values for  DQM
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0));
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1));
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQM_B0));
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQM_B1));
        }
    }

    vSetRank(p, backup_rank);

#if SUPPORT_TX_DELAY_LINE_CALI
    if (p->frequency >= DDR_DDR1333)
    {
        //TX Delay Line DQM, average - min
        mcSHOW_DBG_MSG(("TX per bit delay PI(DLY): 64(%d)\n", p->density));
        mcSHOW_DBG_MSG(("TX DQM\n"));
        for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
        {
            uiDelay = u2dqm_dly[u1ByteIdx] - u2Center_min[u1ByteIdx];
            u2dqm_dly[u1ByteIdx] = (uiDelay * p->density + 8)/64;
            mcSHOW_DBG_MSG(("%d(%d) ", uiDelay, u2dqm_dly[u1ByteIdx]));
        }

        for(ii=p->rank; ii<RANK_MAX; ii++)
        {
            vSetRank(p,ii);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1, u2dqm_dly[0], SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1, u2dqm_dly[1], SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B0_DQ1+(1<<POS_BANK_NUM), u2dqm_dly[2], SHU1_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);
            vIO32WriteFldAlign(DDRPHY_SHU1_R0_B1_DQ1+(1<<POS_BANK_NUM), u2dqm_dly[3], SHU1_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);
        }
        vSetRank(p, backup_rank);

        mcSHOW_DBG_MSG(("\nTX DQ\n"));
        //TX Delay Line DQ de-skew
        for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
        {
            for (u1BitIdx=0; u1BitIdx<DQS_BIT_NUMBER; u1BitIdx++)
            {
                ucindex = u1ByteIdx * DQS_BIT_NUMBER + u1BitIdx;
                uiDelay = FinalWinPerBit[ucindex].win_center - u2Center_min[u1ByteIdx];
                mcSHOW_DBG_MSG(("%d ", uiDelay));
                ii = ucindex;
                if (p->en_4bitMux == ENABLE)
                {
                     if ((ucindex >= 4 && ucindex <= 7) || (ucindex >= 20 && ucindex <= 23))
                        ii = ucindex + 4;
                     else if ((ucindex >= 8 && ucindex <= 11) || (ucindex >= 24 && ucindex <= 27))
                        ii = ucindex - 4;
                }
                WinPerBit[ii].best_dqdly = (uiDelay * p->density + 8)/64;
            }
        }
		mcSHOW_DBG_MSG(("\n"));

		for (ucindex=0; ucindex<p->data_width; ucindex++)
		{
			mcSHOW_DBG_MSG(("%d ", WinPerBit[ucindex].best_dqdly));
		}
		mcSHOW_DBG_MSG(("\n"));

        for(ii=p->rank; ii<RANK_MAX; ii++)
        {
            vSetRank(p,ii);

            // set to de-skew values for DQ
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0, \
                               P_Fld(WinPerBit[0].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0) | \
                               P_Fld(WinPerBit[1].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
                               P_Fld(WinPerBit[2].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
                               P_Fld(WinPerBit[3].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
                               P_Fld(WinPerBit[4].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
                               P_Fld(WinPerBit[5].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
                               P_Fld(WinPerBit[6].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
                               P_Fld(WinPerBit[7].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0));
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ0, \
                               P_Fld(WinPerBit[8].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1) | \
                               P_Fld(WinPerBit[9].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) | \
                               P_Fld(WinPerBit[10].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) | \
                               P_Fld(WinPerBit[11].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) | \
                               P_Fld(WinPerBit[12].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) | \
                               P_Fld(WinPerBit[13].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) | \
                               P_Fld(WinPerBit[14].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) | \
                               P_Fld(WinPerBit[15].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1));
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0+(1<<POS_BANK_NUM), \
                               P_Fld(WinPerBit[16].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0) | \
                               P_Fld(WinPerBit[17].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
                               P_Fld(WinPerBit[18].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
                               P_Fld(WinPerBit[19].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
                               P_Fld(WinPerBit[20].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
                               P_Fld(WinPerBit[21].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
                               P_Fld(WinPerBit[22].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
                               P_Fld(WinPerBit[23].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0));
            vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ0+(1<<POS_BANK_NUM), \
                               P_Fld(WinPerBit[24].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1) | \
                               P_Fld(WinPerBit[25].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) | \
                               P_Fld(WinPerBit[26].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) | \
                               P_Fld(WinPerBit[27].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) | \
                               P_Fld(WinPerBit[28].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) | \
                               P_Fld(WinPerBit[29].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) | \
                               P_Fld(WinPerBit[30].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) | \
                               P_Fld(WinPerBit[31].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1));
        }
        vSetRank(p, backup_rank);
    }
#endif

#if SUPPORT_TYPE_DDR3X4
    if ((p->pinmux == PIN_MUX_TYPE_DDR3X4) && (p->density) && (calType ==TX_DQ_DQS_MOVE_DQ_ONLY || calType== TX_DQ_DQS_MOVE_DQ_DQM))//perbit dly line enable
    {
        for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
        {
            TxWinTransferDelayToUIPI(p, u2Center_min[u1ByteIdx], dq_ui_small_bak, dq_ui_large_bak, &ucdq_final_ui_large[u1ByteIdx], &ucdq_final_ui_small[u1ByteIdx], &ucdq_final_pi[u1ByteIdx]);
            TxWinTransferDelayToUIPI(p, u2Center_min[u1ByteIdx], dq_oen_ui_small_bak, dq_oen_ui_large_bak, &ucdq_final_oen_ui_large[u1ByteIdx], &ucdq_final_oen_ui_small[u1ByteIdx], &ucdq_final_pi[u1ByteIdx]);
            mcSHOW_DBG_MSG(("DDR3X4 Byte%d, PI DQ Delay %d \n", u1ByteIdx, u2Center_min[u1ByteIdx]));
        }

        //swap B12 & B13
        mcSHOW_DBG_MSG(("DDR3X4 TX Per Bit SWAP\n"));
        ucdq_final_ui_large[2] = ucdq_final_ui_large[1];
        ucdq_final_ui_large[3] = ucdq_final_ui_large[1];
        ucdq_final_oen_ui_large[2] = ucdq_final_oen_ui_large[1];
        ucdq_final_oen_ui_large[3] = ucdq_final_oen_ui_large[1];
        ucdq_final_ui_small[2] = ucdq_final_ui_small[1];
        ucdq_final_ui_small[3] = ucdq_final_ui_small[1];
        ucdq_final_oen_ui_small[2] = ucdq_final_oen_ui_small[1];
        ucdq_final_oen_ui_small[3] = ucdq_final_oen_ui_small[1];
        ucdq_final_pi[2] = ucdq_final_pi[1];
        ucdq_final_pi[3] = ucdq_final_pi[1];

        for (u1ByteIdx=0; u1ByteIdx<(p->data_width/ucbit_num); u1ByteIdx++)
        {
            mcSHOW_DBG_MSG(("DDR3X4 Final DQ PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n", ucdq_final_ui_large[u1ByteIdx], ucdq_final_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));
            mcSHOW_DBG_MSG(("DDR3X4 OEN DQ PI Delay(LargeUI, SmallUI, PI) =(%d ,%d, %d)\n\n", ucdq_final_oen_ui_large[u1ByteIdx], ucdq_final_oen_ui_small[u1ByteIdx], ucdq_final_pi[u1ByteIdx]));
        }

        //TX Delay Line DQ de-skew
        mcSHOW_DBG_MSG(("DDR3X4 de-skew(%d) PI(DLY): \n", p->density));
        for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUMBER); u1ByteIdx++)
        {
            for (u1BitIdx=0; u1BitIdx<DQS_BIT_NUMBER; u1BitIdx++)
            {
                ucindex = u1ByteIdx * DQS_BIT_NUMBER + u1BitIdx;
                WinPerBit[ucindex].best_dqdly = FinalWinPerBit[ucindex].win_center - u2Center_min[u1ByteIdx];
                mcSHOW_DBG_MSG(("%d", WinPerBit[ucindex].best_dqdly));
                WinPerBit[ucindex].best_dqdly = (WinPerBit[ucindex].best_dqdly*p->density + 200)/1000;
                mcSHOW_DBG_MSG(("(%d) ", WinPerBit[ucindex].best_dqdly)); 
            }

            mcSHOW_DBG_MSG(("\n"));
        }
        for(ii=p->rank; ii<RANK_MAX; ii++)
        {
            vSetRank(p,ii);

            //TXDLY_DQ , TXDLY_OEN_DQ
            vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ0, \
                                    P_Fld(ucdq_final_ui_large[0], SHURK0_SELPH_DQ0_TXDLY_DQ0) | \
                                    P_Fld(ucdq_final_ui_large[1], SHURK0_SELPH_DQ0_TXDLY_DQ1) | \
                                    P_Fld(ucdq_final_ui_large[2], SHURK0_SELPH_DQ0_TXDLY_DQ2) | \
                                    P_Fld(ucdq_final_ui_large[3], SHURK0_SELPH_DQ0_TXDLY_DQ3) | \
                                    P_Fld(ucdq_final_oen_ui_large[0], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) | \
                                    P_Fld(ucdq_final_oen_ui_large[1], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) | \
                                    P_Fld(ucdq_final_oen_ui_large[2], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) | \
                                    P_Fld(ucdq_final_oen_ui_large[3], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3));

             // DLY_DQ[2:0]
             vIO32WriteFldMulti(DRAMC_REG_SHURK0_SELPH_DQ2, \
                                    P_Fld(ucdq_final_ui_small[0], SHURK0_SELPH_DQ2_DLY_DQ0) | \
                                    P_Fld(ucdq_final_ui_small[1], SHURK0_SELPH_DQ2_DLY_DQ1) | \
                                    P_Fld(ucdq_final_ui_small[2], SHURK0_SELPH_DQ2_DLY_DQ2) | \
                                    P_Fld(ucdq_final_ui_small[3], SHURK0_SELPH_DQ2_DLY_DQ3) | \
                                    P_Fld(ucdq_final_oen_ui_small[0], SHURK0_SELPH_DQ2_DLY_OEN_DQ0) | \
                                    P_Fld(ucdq_final_oen_ui_small[1], SHURK0_SELPH_DQ2_DLY_OEN_DQ1) | \
                                    P_Fld(ucdq_final_oen_ui_small[2], SHURK0_SELPH_DQ2_DLY_OEN_DQ2) | \
                                    P_Fld(ucdq_final_oen_ui_small[3], SHURK0_SELPH_DQ2_DLY_OEN_DQ3));

             // set to best values for  DQ
             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7, P_Fld(ucdq_final_pi[0], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7, P_Fld(ucdq_final_pi[1], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));
             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[2], SHU1_R0_B0_DQ7_RK0_ARPI_DQ_B0));
             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B1_DQ7+(1<<POS_BANK_NUM), P_Fld(ucdq_final_pi[3], SHU1_R0_B1_DQ7_RK0_ARPI_DQ_B1));

             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0, \
                                   P_Fld(WinPerBit[0].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0) | \
                                   P_Fld(WinPerBit[1].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
                                   P_Fld(WinPerBit[2].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
                                   P_Fld(WinPerBit[3].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
                                   P_Fld(WinPerBit[4].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
                                   P_Fld(WinPerBit[5].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
                                   P_Fld(WinPerBit[6].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
                                   P_Fld(WinPerBit[7].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0));
             vIO32WriteFldMulti(DDRPHY_SHU1_R0_B0_DQ0+(1<<POS_BANK_NUM), \
                                   P_Fld(WinPerBit[8].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0) | \
                                   P_Fld(WinPerBit[9].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) | \
                                   P_Fld(WinPerBit[10].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) | \
                                   P_Fld(WinPerBit[11].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) | \
                                   P_Fld(WinPerBit[12].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) | \
                                   P_Fld(WinPerBit[13].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) | \
                                   P_Fld(WinPerBit[14].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) | \
                                   P_Fld(WinPerBit[15].best_dqdly, SHU1_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0));
             vIO32WriteFldMulti_All(DDRPHY_SHU1_R0_B1_DQ0, \
                                   P_Fld(WinPerBit[8].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1) | \
                                   P_Fld(WinPerBit[9].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) | \
                                   P_Fld(WinPerBit[10].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) | \
                                   P_Fld(WinPerBit[11].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) | \
                                   P_Fld(WinPerBit[12].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) | \
                                   P_Fld(WinPerBit[13].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) | \
                                   P_Fld(WinPerBit[14].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) | \
                                   P_Fld(WinPerBit[15].best_dqdly, SHU1_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1));
        } 
        vSetRank(p, backup_rank);
    }
#endif
#if REG_ACCESS_PORTING_DGB
    RegLogEnable =0;
#endif

	p->min_winsize = u2MinWinSize;
	p->sum_winsize = u2TempWinSum;

    mcSHOW_DBG_MSG(("[DramcTxWindowPerbitCal] ====Done====\n"));

    return DRAM_OK;
}

#define PC4_VREFDQ_START			0x00
#define PC4_VREFDQ_END				0x32
#define PC4_VREFDQ_STEP				0x01
#define PC4_VREFDQ_RANGE			0x01
#define PC4_VREFDQ_DAEFAULT_VALUE	0x08
#define PC4_VREFDQ_TYPE_WINSUM		0x00
#define PC4_VREFDQ_TYPE_WINMIN		0x01

DRAM_STATUS_T DramcTxVrefDQCalibration(DRAMC_CTX_T *p)
{
    U8 u1VrefScanEnable;
    U16 u2VrefRange, u2VrefLevel, u2VrefValue;
    U16 u2VrefBegin, u2VrefEnd, u2VrefStep;
    VrefDQ_WIN_T VrefDQWin[PC4_VREFDQ_END - PC4_VREFDQ_START + 1];
    U16 u2FinalVrefLevel, u2FinalVrefWin;
    U8 backup_rank;

    if (!p)
    {
        mcSHOW_ERR_MSG(("context is NULL\n"));
        return DRAM_FAIL;
    }

    backup_rank = u1GetRank(p);

    if (p->dram_type == TYPE_PCDDR4)		u1VrefScanEnable = 1;
    else									u1VrefScanEnable = 0;

    if(u1VrefScanEnable)
    {
        u2VrefBegin = PC4_VREFDQ_START;
        u2VrefEnd = PC4_VREFDQ_END;
        u2VrefStep = PC4_VREFDQ_STEP;
        u2VrefRange = PC4_VREFDQ_RANGE;
        u2FinalVrefLevel = u2VrefBegin;
        u2FinalVrefWin = 0x00;
        for(u2VrefLevel = u2VrefBegin; u2VrefLevel <= u2VrefEnd; u2VrefLevel += u2VrefStep)
        {
            u2VrefValue = (PC4_VREFDQ_RANGE<<6) | PC4_VREFDQ_DAEFAULT_VALUE;
            VrefDQWin[u2VrefLevel].VrefValue = u2VrefValue;
            VrefDQWin[u2VrefLevel].TxWin = 0x00;
        }
    }
    else //LPDDR3, the for loop will only excute u2VrefLevel=TX_VREF_RANGE_END/2.
    {
        u2VrefBegin = 0;
        u2VrefEnd = 0;
        u2VrefStep =  1;
        u2VrefRange = 0;
    }

    vSetCalibrationResult(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_FAIL);

    mcSHOW_DBG_MSG2(("[DramcTxVrefDQCalibration] Frequency=%d, Rank=%d\n", p->frequency, p->rank));

    DramcModeRegWrite_PC4(p, 1, 2, 0x080 | (DDR_PC4_TCCD << 10));//BG1, MR2 = 0x049  //VrefDQ Traing Enable
    mcDELAY_US(100);
    for(u2VrefLevel = u2VrefBegin; u2VrefLevel <= u2VrefEnd; u2VrefLevel += u2VrefStep)
    {
         //   SET tx Vref (DQ) here, LP3 no need to set this.
        if(u1VrefScanEnable)
        {
            //mcSHOW_DBG_MSG(("\n\n======  PC4 TX VrefDQ Range %d, VrefLevel=%d  ========\n", u2VrefRange, u2VrefLevel));
            u2VrefValue = (u2VrefRange<<6) | u2VrefLevel;
            DramcModeRegWrite_PC4(p, 1, 2, 0x080 | (DDR_PC4_TCCD << 10) | u2VrefValue); //set VrefDQ Traing Value
            mcDELAY_US(100);
        }
        else
        {
            //mcSHOW_DBG_MSG(("\n\n======  TX Vref Scan disable ========\n"));
            break;
        }

        //VrefDQWin[u2VrefLevel].TxWin = DramcTxWindowPerbitCal1((DRAMC_CTX_T *) p, TX_DQ_DQS_MOVE_DQ_DQM, PC4_VREFDQ_TYPE_WINSUM);
        p->fgTXPerbifInit[p->rank]= FALSE;
        u4DRAMdebugLOgEnable = 0;
        u4DRAMdebugLOgEnable2 = 0;
        DramcTxWindowPerbitCal((DRAMC_CTX_T *) p, TX_DQ_DQS_MOVE_DQ_DQM);
        VrefDQWin[u2VrefLevel].TxWin = p->sum_winsize;
        u4DRAMdebugLOgEnable = 1;
        #ifdef TARGET_BUILD_VARIANT_ENG
        u4DRAMdebugLOgEnable2 = 1;
        #endif
        if(u1VrefScanEnable)
        {
            if(VrefDQWin[u2VrefLevel].TxWin)
            {
                VrefDQWin[u2VrefLevel].VrefValue = u2VrefValue;
            }
        }
    }

    if(u1VrefScanEnable)
    {
        u2FinalVrefWin = VrefDQWin[u2VrefBegin].TxWin;
        mcSHOW_DBG_MSG2(("VrefDQ Level %d, Value 0x%x, Win %d\n", u2VrefBegin, VrefDQWin[u2VrefBegin].VrefValue, VrefDQWin[u2VrefBegin].TxWin));
        for(u2VrefLevel = u2VrefBegin + u2VrefStep; u2VrefLevel <= u2VrefEnd; u2VrefLevel += u2VrefStep)
        {
            if(u2VrefLevel < 10)
            {
                mcSHOW_DBG_MSG2(("VrefDQ Level  %d, Value 0x%x, Win %d\n", u2VrefLevel, VrefDQWin[u2VrefLevel].VrefValue, VrefDQWin[u2VrefLevel].TxWin));
            }
            else
            {
                mcSHOW_DBG_MSG2(("VrefDQ Level %d, Value 0x%x, Win %d\n", u2VrefLevel, VrefDQWin[u2VrefLevel].VrefValue, VrefDQWin[u2VrefLevel].TxWin));
            }
            if(u2FinalVrefWin < VrefDQWin[u2VrefLevel].TxWin)
            {
                u2FinalVrefLevel = u2VrefLevel;
                u2FinalVrefWin = VrefDQWin[u2VrefLevel].TxWin;
            }
        }
        DramcModeRegWrite_PC4(p, 1, 2, 0x080 | (DDR_PC4_TCCD << 10) | VrefDQWin[u2FinalVrefLevel].VrefValue); //set VrefDQ Traing Value
        mcDELAY_US(100);

        mcSHOW_DBG_MSG(("\nFinal VrefDQ Level %d VrefDQ Value 0x%x, TX Win %d \n", u2FinalVrefLevel, VrefDQWin[u2FinalVrefLevel].VrefValue, VrefDQWin[u2FinalVrefLevel].TxWin));
        DramcModeRegWrite_PC4(p, 1, 2, 0x000 | (DDR_PC4_TCCD << 10) | VrefDQWin[u2FinalVrefLevel].VrefValue); //disable VrefDQ Traing
        mcDELAY_US(100);

    }
    vSetRank(p, backup_rank);

    return DRAM_OK;

}

#endif //SIMULATION_TX_PERBIT
