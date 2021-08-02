#ifndef __DRAMC_CH0_REG_H__
#define __DRAMC_CH0_REG_H__

#define DRAMC_AO_BASE Channel_A_DRAMC_AO_BASE_VIRTUAL

#define DRAMC_REG_DDRCONF0                                 (DRAMC_AO_BASE + 0x00000)
    #define DDRCONF0_RDATRST                                   Fld(1, 0) //[0:0]
    #define DDRCONF0_DMSW_RST                                  Fld(1, 1) //[1:1]
    #define DDRCONF0_WDT_DBG_RST                               Fld(1, 2) //[2:2]
    #define DDRCONF0_APBL2                                     Fld(1, 10) //[10:10]
    #define DDRCONF0_BG4EN                                     Fld(1, 11) //[11:11]
    #define DDRCONF0_BK8EN                                     Fld(1, 12) //[12:12]
    #define DDRCONF0_BC4OTF_OPT                                Fld(1, 13) //[13:13]
    #define DDRCONF0_AG0MWR                                    Fld(1, 14) //[14:14]
    #define DDRCONF0_BC4OTF                                    Fld(1, 15) //[15:15]
    #define DDRCONF0_GDDR3RST                                  Fld(1, 16) //[16:16]
    #define DDRCONF0_DDR34_2TCMDEN                             Fld(1, 17) //[17:17]
    #define DDRCONF0_DQ4BMUX                                   Fld(1, 19) //[19:19]
    #define DDRCONF0_DM16BITFULL                               Fld(1, 21) //[21:21]
    #define DDRCONF0_DM4TO1MODE                                Fld(1, 22) //[22:22]
    #define DDRCONF0_GDDR3EN                                   Fld(1, 23) //[23:23]
    #define DDRCONF0_LPDDR2EN                                  Fld(1, 24) //[24:24]
    #define DDRCONF0_LPDDR3EN                                  Fld(1, 25) //[25:25]
    #define DDRCONF0_LPDDR4EN                                  Fld(1, 26) //[26:26]
    #define DDRCONF0_LPDDR2_NO_INT                             Fld(1, 27) //[27:27]
    #define DDRCONF0_DDR2EN                                    Fld(1, 28) //[28:28]
    #define DDRCONF0_DDR3EN                                    Fld(1, 29) //[29:29]
    #define DDRCONF0_DDR4EN                                    Fld(1, 30) //[30:30]
    #define DDRCONF0_DRAMC_SW_RST                              Fld(1, 31) //[31:31]

#define DRAMC_REG_DRAMCTRL                                 (DRAMC_AO_BASE + 0x00004)
    #define DRAMCTRL_CTOREQ_HPRI_OPT                           Fld(1, 0) //[0:0]
    #define DRAMCTRL_ADRDECEN_TARKMODE                         Fld(1, 1) //[1:1]
    #define DRAMCTRL_ADRDECEN                                  Fld(1, 2) //[2:2]
    #define DRAMCTRL_ADRBIT3DEC                                Fld(1, 3) //[3:3]
    #define DRAMCTRL_TMRR2WDIS                                 Fld(1, 4) //[4:4]
    #define DRAMCTRL_ADRDEN_1TO4_OPT                           Fld(1, 5) //[5:5]
    #define DRAMCTRL_RANK_ASYM                                 Fld(1, 7) //[7:7]
    #define DRAMCTRL_CLKWITRFC                                 Fld(1, 9) //[9:9]
    #define DRAMCTRL_CHKFORPRE                                 Fld(1, 10) //[10:10]
    #define DRAMCTRL_ASYNCEN                                   Fld(1, 12) //[12:12]
    #define DRAMCTRL_DYNMWREN                                  Fld(1, 13) //[13:13]
    #define DRAMCTRL_ALEBLOCK                                  Fld(1, 14) //[14:14]
    #define DRAMCTRL_TMRRICHKDIS                               Fld(1, 15) //[15:15]
    #define DRAMCTRL_DMRCDRSV                                  Fld(1, 16) //[16:16]
    #define DRAMCTRL_TMRRIBYRK_DIS                             Fld(1, 17) //[17:17]
    #define DRAMCTRL_ZQCALL                                    Fld(1, 18) //[18:18]
    #define DRAMCTRL_PREALL_OPTION                             Fld(1, 19) //[19:19]
    #define DRAMCTRL_TCMD                                      Fld(3, 20) //[22:20]
    #define DRAMCTRL_MRRIOPT                                   Fld(1, 23) //[23:23]
    #define DRAMCTRL_FW2R                                      Fld(1, 24) //[24:24]
    #define DRAMCTRL_REQQUE_DEPTH_UPD                          Fld(1, 25) //[25:25]
    #define DRAMCTRL_REQQUE_THD_EN                             Fld(1, 26) //[26:26]
    #define DRAMCTRL_REQQUE_MAXCNT_CHG                         Fld(1, 27) //[27:27]
    #define DRAMCTRL_PREA_RK                                   Fld(2, 28) //[29:28]
    #define DRAMCTRL_IDLE_COND_OPT                             Fld(1, 30) //[30:30]

#define DRAMC_REG_MISCTL0                                  (DRAMC_AO_BASE + 0x00008)
    #define MISCTL0_R_DMCA_IDLE_EN                             Fld(1, 0) //[0:0]
    #define MISCTL0_DM32BIT_RDSEL_OPT                          Fld(1, 1) //[1:1]
    #define MISCTL0_DRAMC_LOOP_BAK_EN                          Fld(1, 2) //[2:2]
    #define MISCTL0_WPRE0T                                     Fld(1, 3) //[3:3]
    #define MISCTL0_LOOP_BAK_WDAT_SEL                          Fld(3, 4) //[6:4]
    #define MISCTL0_DRAMC_LOOP_BAK_CMP_EN                      Fld(1, 7) //[7:7]
    #define MISCTL0_DLE64EN                                    Fld(1, 8) //[8:8]
    #define MISCTL0_PRE_DLE_VLD_OPT                            Fld(1, 10) //[10:10]
    #define MISCTL0_DATLAT_PDLE_TH                             Fld(3, 11) //[13:11]
    #define MISCTL0_PG_WAKEUP_OPT                              Fld(2, 14) //[15:14]
    #define MISCTL0_IDLE_CNT_OPT                               Fld(1, 16) //[16:16]
    #define MISCTL0_PAGDIS                                     Fld(1, 17) //[17:17]
    #define MISCTL0_IDLEDCM_CNT_OPT                            Fld(1, 18) //[18:18]
    #define MISCTL0_REFA_ARB_EN2                               Fld(1, 19) //[19:19]
    #define MISCTL0_WRBYTE_CNT_OPT                             Fld(1, 20) //[20:20]
    #define MISCTL0_REFA_ARB_EN_OPTION                         Fld(1, 21) //[21:21]
    #define MISCTL0_REORDER_MASK_E1T                           Fld(1, 22) //[22:22]
    #define MISCTL0_PBC_ARB_E1T                                Fld(1, 23) //[23:23]
    #define MISCTL0_PBC_ARB_EN                                 Fld(1, 24) //[24:24]
    #define MISCTL0_REFA_ARB_EN                                Fld(1, 25) //[25:25]
    #define MISCTL0_REFP_ARB_EN                                Fld(1, 26) //[26:26]
    #define MISCTL0_EMIPREEN                                   Fld(1, 27) //[27:27]
    #define MISCTL0_REFP_ARB_EN2                               Fld(1, 31) //[31:31]

#define DRAMC_REG_PERFCTL0                                 (DRAMC_AO_BASE + 0x0000C)
    #define PERFCTL0_DISRDPHASE1                               Fld(1, 1) //[1:1]
    #define PERFCTL0_AIDCHKEN                                  Fld(1, 3) //[3:3]
    #define PERFCTL0_RWOFOEN                                   Fld(1, 4) //[4:4]
    #define PERFCTL0_RWOFOWNUM                                 Fld(3, 5) //[7:5]
    #define PERFCTL0_RWHPRIEN                                  Fld(1, 8) //[8:8]
    #define PERFCTL0_RWLLATEN                                  Fld(1, 9) //[9:9]
    #define PERFCTL0_RWAGEEN                                   Fld(1, 10) //[10:10]
    #define PERFCTL0_EMILLATEN                                 Fld(1, 11) //[11:11]
    #define PERFCTL0_LASTCMDOPT                                Fld(1, 12) //[12:12]
    #define PERFCTL0_RWHPRICTL                                 Fld(1, 13) //[13:13]
    #define PERFCTL0_WFLUSHEN                                  Fld(1, 14) //[14:14]
    #define PERFCTL0_RWSPLIT                                   Fld(1, 15) //[15:15]
    #define PERFCTL0_MWHPRIEN                                  Fld(1, 17) //[17:17]
    #define PERFCTL0_REORDER_MODE                              Fld(1, 18) //[18:18]
    #define PERFCTL0_REORDEREN                                 Fld(1, 19) //[19:19]
    #define PERFCTL0_SBR_MASK_OPT                              Fld(1, 20) //[20:20]
    #define PERFCTL0_SBR_MASK_OPT2                             Fld(1, 21) //[21:21]
    #define PERFCTL0_MAFIXHIGH                                 Fld(1, 22) //[22:22]
    #define PERFCTL0_TESTWRHIGH                                Fld(1, 23) //[23:23]
    #define PERFCTL0_RECORDER_MASK_OPT                         Fld(1, 24) //[24:24]
    #define PERFCTL0_MDMCU_MASK_EN                             Fld(1, 25) //[25:25]
    #define PERFCTL0_WRFIFO_OPT                                Fld(1, 26) //[26:26]
    #define PERFCTL0_WRFIO_MODE2                               Fld(1, 27) //[27:27]
    #define PERFCTL0_RDFIFOEN                                  Fld(1, 30) //[30:30]
    #define PERFCTL0_WRFIFOEN                                  Fld(1, 31) //[31:31]

#define DRAMC_REG_ARBCTL                                   (DRAMC_AO_BASE + 0x00010)
    #define ARBCTL_MAXPENDCNT                                  Fld(8, 0) //[7:0]
    #define ARBCTL_RDATACNTDIS                                 Fld(1, 8) //[8:8]
    #define ARBCTL_WDATACNTDIS                                 Fld(1, 9) //[9:9]
    #define ARBCTL_RSV_SA0                                     Fld(1, 10) //[10:10]
    #define ARBCTL_RSV_SA1                                     Fld(1, 11) //[11:11]
    #define ARBCTL_RSV_SA2                                     Fld(1, 12) //[12:12]
    #define ARBCTL_RSV_SA3                                     Fld(1, 13) //[13:13]
    #define ARBCTL_RSV_DRAM_CBT                                Fld(1,13)//[13:13]
    #define ARBCTL_RSV_DRAM_TYPE                               Fld(3,10)//[12:10]
    #define ARBCTL_DBIWR_IMP_EN                                Fld(1, 14) //[14:14]
    #define ARBCTL_DBIWR_PINMUX_EN                             Fld(1, 15) //[15:15]
    #define ARBCTL_DBIWR_OPT_B0                                Fld(8, 16) //[23:16]
    #define ARBCTL_DBIWR_OPT_B1                                Fld(8, 24) //[31:24]

#define DRAMC_REG_PHY_RX_INCTL                             (DRAMC_AO_BASE + 0x00018)
    #define PHY_RX_INCTL_RX_IN_GATE_EN_OPT                     Fld(1, 0) //[0:0]
    #define PHY_RX_INCTL_RX_IN_GATE_EN_4BYTE_EN                Fld(1, 1) //[1:1]
    #define PHY_RX_INCTL_FIX_IN_GATE_EN                        Fld(4, 8) //[11:8]
    #define PHY_RX_INCTL_DIS_IN_GATE_EN                        Fld(4, 12) //[15:12]
    #define PHY_RX_INCTL_RX_IN_BUFF_EN_OPT                     Fld(1, 16) //[16:16]
    #define PHY_RX_INCTL_RX_IN_BUFF_EN_4BYTE_EN                Fld(1, 17) //[17:17]
    #define PHY_RX_INCTL_FIX_IN_BUFF_EN                        Fld(4, 24) //[27:24]
    #define PHY_RX_INCTL_DIS_IN_BUFF_EN                        Fld(4, 28) //[31:28]

#define DRAMC_REG_RSTMASK                                  (DRAMC_AO_BASE + 0x0001C)
    #define RSTMASK_WDATKEY0                                   Fld(1, 0) //[0:0]
    #define RSTMASK_WDATKEY1                                   Fld(1, 1) //[1:1]
    #define RSTMASK_WDATKEY2                                   Fld(1, 2) //[2:2]
    #define RSTMASK_WDATKEY3                                   Fld(1, 3) //[3:3]
    #define RSTMASK_WDATKEY4                                   Fld(1, 4) //[4:4]
    #define RSTMASK_WDATKEY5                                   Fld(1, 5) //[5:5]
    #define RSTMASK_WDATKEY6                                   Fld(1, 6) //[6:6]
    #define RSTMASK_WDATKEY7                                   Fld(1, 7) //[7:7]
    #define RSTMASK_WDATITLV                                   Fld(1, 8) //[8:8]
    #define RSTMASK_RSV_DRAM_CBT_MIXED                         Fld(2, 13) /* cc add */
    #define RSTMASK_RETRY_DATRST_MASK                          Fld(1, 21) //[21:21]
    #define RSTMASK_DVFS_SYNC_MASK_FOR_PHY                     Fld(1, 24) //[24:24]
    #define RSTMASK_GT_SYNC_MASK_FOR_PHY                       Fld(1, 25) //[25:25]
    #define RSTMASK_DVFS_SYNC_MASK                             Fld(1, 26) //[26:26]
    #define RSTMASK_GTDMW_SYNC_MASK                            Fld(1, 27) //[27:27]
    #define RSTMASK_GT_SYNC_MASK                               Fld(1, 28) //[28:28]
    #define RSTMASK_DAT_SYNC_MASK                              Fld(1, 29) //[29:29]
    #define RSTMASK_PHY_SYNC_MASK                              Fld(1, 30) //[30:30]
    #define RSTMASK_R_DMSHU_RDATRST_MASK                       Fld(1, 31) //[31:31]

#define DRAMC_REG_PADCTRL                                  (DRAMC_AO_BASE + 0x00020)
    #define PADCTRL_DQIENQKEND                                 Fld(2, 0) //[1:0]
    #define PADCTRL_DQIENLATEBEGIN                             Fld(1, 3) //[3:3]
    #define PADCTRL_DISDMOEDIS                                 Fld(1, 8) //[8:8]
    #define PADCTRL_DRAMOEN                                    Fld(1, 12) //[12:12]
    #define PADCTRL_FIXDQIEN                                   Fld(4, 16) //[19:16]
    #define PADCTRL_DISDQIEN                                   Fld(4, 20) //[23:20]

#define DRAMC_REG_CKECTRL                                  (DRAMC_AO_BASE + 0x00024)
    #define CKECTRL_CKEBYCTL                                   Fld(1, 0) //[0:0]
    #define CKECTRL_CKE2RANK_OPT3                              Fld(1, 1) //[1:1]
    #define CKECTRL_CKE2FIXON                                  Fld(1, 2) //[2:2]
    #define CKECTRL_CKE2FIXOFF                                 Fld(1, 3) //[3:3]
    #define CKECTRL_CKE1FIXON                                  Fld(1, 4) //[4:4]
    #define CKECTRL_CKE1FIXOFF                                 Fld(1, 5) //[5:5]
    #define CKECTRL_CKEFIXON                                   Fld(1, 6) //[6:6]
    #define CKECTRL_CKEFIXOFF                                  Fld(1, 7) //[7:7]
    #define CKECTRL_CKE2RANK_OPT5                              Fld(1, 8) //[8:8]
    #define CKECTRL_CKE2RANK_OPT6                              Fld(1, 9) //[9:9]
    #define CKECTRL_CKE2RANK_OPT7                              Fld(1, 10) //[10:10]
    #define CKECTRL_CKE2RANK_OPT8                              Fld(1, 11) //[11:11]
    #define CKECTRL_CKEEXTEND                                  Fld(1, 12) //[12:12]
    #define CKECTRL_CKETIMER_SEL                               Fld(1, 13) //[13:13]
    #define CKECTRL_FASTWAKE_SEL                               Fld(1, 14) //[14:14]
    #define CKECTRL_CKEWAKE_SEL                                Fld(1, 15) //[15:15]
    #define CKECTRL_CKEWAKE_SEL2                               Fld(1, 16) //[16:16]
    #define CKECTRL_CKE2RANK_OPT9                              Fld(1, 17) //[17:17]
    #define CKECTRL_CKE2RANK_OPT10                             Fld(1, 18) //[18:18]
    #define CKECTRL_CKE2RANK_OPT11                             Fld(1, 19) //[19:19]
    #define CKECTRL_CKE2RANK_OPT12                             Fld(1, 20) //[20:20]
    #define CKECTRL_CKE2RANK_OPT13                             Fld(1, 21) //[21:21]
    #define CKECTRL_CKEPBDIS                                   Fld(1, 22) //[22:22]
    #define CKECTRL_CKELCKFIX                                  Fld(1, 23) //[23:23]
    #define CKECTRL_CKELCKCNT                                  Fld(3, 24) //[26:24]
    #define CKECTRL_RUNTIMEMRRCKEFIX                           Fld(1, 27) //[27:27]
    #define CKECTRL_RUNTIMEMRRMIODIS                           Fld(1, 28) //[28:28]
    #define CKECTRL_CKE_H2L_OPT                                Fld(1, 29) //[29:29]
    #define CKECTRL_CKEON                                      Fld(1, 31) //[31:31]

#define DRAMC_REG_DRSCTRL                                  (DRAMC_AO_BASE + 0x00028)
    #define DRSCTRL_DRSDIS                                     Fld(1, 0) //[0:0]
    #define DRSCTRL_DRSBLOCKOPT                                Fld(1, 1) //[1:1]
    #define DRSCTRL_DRSPB2AB_OPT                               Fld(1, 2) //[2:2]
    #define DRSCTRL_DRSRK1_SW                                  Fld(1, 3) //[3:3]
    #define DRSCTRL_DRSMON_CLR                                 Fld(1, 4) //[4:4]
    #define DRSCTRL_DRSCLR_EN                                  Fld(1, 5) //[5:5]
    #define DRSCTRL_DRSACKWAITREF                              Fld(1, 6) //[6:6]
    #define DRSCTRL_DRSCLR_RK0_EN                              Fld(1, 7) //[7:7]
    #define DRSCTRL_DRSDLY                                     Fld(4, 8) //[11:8]
    #define DRSCTRL_DRS_CNTX                                   Fld(7, 12) //[18:12]
    #define DRSCTRL_DRS_SELFWAKE_DMYRD_DIS                     Fld(1, 19) //[19:19]
    #define DRSCTRL_DRS_DMYRD_MIOCK_OPT                        Fld(1, 20) //[20:20]
    #define DRSCTRL_DRSOPT2                                    Fld(1, 21) //[21:21]
    #define DRSCTRL_DRS_MR4_OPT_B                              Fld(1, 24) //[24:24]
    #define DRSCTRL_RK_SCINPUT_OPT                             Fld(1, 29) //[29:29]

#define DRAMC_REG_RKCFG                                    (DRAMC_AO_BASE + 0x00034)
    #define RKCFG_TXRANK                                       Fld(2, 0) //[1:0]
    #define RKCFG_CKE2RANK_OPT2                                Fld(1, 2) //[2:2]
    #define RKCFG_TXRANKFIX                                    Fld(1, 3) //[3:3]
    #define RKCFG_RKMODE                                       Fld(3, 4) //[6:4]
    #define RKCFG_RKSWAP                                       Fld(1, 7) //[7:7]
    #define RKCFG_DM3RANK                                      Fld(1, 8) //[8:8]
    #define RKCFG_RANKRDY_OPT                                  Fld(1, 9) //[9:9]
    #define RKCFG_MRS2RK                                       Fld(1, 10) //[10:10]
    #define RKCFG_DQSOSC2RK                                    Fld(1, 11) //[11:11]
    #define RKCFG_CKE2RANK                                     Fld(1, 12) //[12:12]
    #define RKCFG_CS2RANK                                      Fld(1, 13) //[13:13]
    #define RKCFG_SHU2RKOPT                                    Fld(1, 14) //[14:14]
    #define RKCFG_CKE2RANK_OPT                                 Fld(1, 15) //[15:15]
    #define RKCFG_RKSIZE                                       Fld(3, 16) //[18:16]
    #define RKCFG_DMCKEWAKE                                    Fld(1, 19) //[19:19]
    #define RKCFG_RK0SRF                                       Fld(1, 20) //[20:20]
    #define RKCFG_RK1SRF                                       Fld(1, 21) //[21:21]
    #define RKCFG_RK2SRF                                       Fld(1, 22) //[22:22]
    #define RKCFG_SRF_ENTER_MASK_OPT                           Fld(1, 23) //[23:23]
    #define RKCFG_RK0DPD                                       Fld(1, 24) //[24:24]
    #define RKCFG_RK1DPD                                       Fld(1, 25) //[25:25]
    #define RKCFG_RK2DPD                                       Fld(1, 26) //[26:26]
    #define RKCFG_CS1FORCE0                                    Fld(1, 27) //[27:27]
    #define RKCFG_RK0DPDX                                      Fld(1, 28) //[28:28]
    #define RKCFG_RK1DPDX                                      Fld(1, 29) //[29:29]
    #define RKCFG_RK2DPDX                                      Fld(1, 30) //[30:30]
    #define RKCFG_CS0FORCE                                     Fld(1, 31) //[31:31]

#define DRAMC_REG_DRAMC_PD_CTRL                            (DRAMC_AO_BASE + 0x00038)
    #define DRAMC_PD_CTRL_DCMEN                                Fld(1, 0) //[0:0]
    #define DRAMC_PD_CTRL_DCMEN2                               Fld(1, 1) //[1:1]
    #define DRAMC_PD_CTRL_DCMENNOTRFC                          Fld(1, 2) //[2:2]
    #define DRAMC_PD_CTRL_PHYCLK_REFWKEN                       Fld(1, 4) //[4:4]
    #define DRAMC_PD_CTRL_COMBPHY_CLKENSAME                    Fld(1, 5) //[5:5]
    #define DRAMC_PD_CTRL_DCMREF_OPT                           Fld(1, 8) //[8:8]
    #define DRAMC_PD_CTRL_PG_DCM_OPT                           Fld(1, 9) //[9:9]
    #define DRAMC_PD_CTRL_COMB_DCM                             Fld(1, 10) //[10:10]
    #define DRAMC_PD_CTRL_RDPERIODON                           Fld(1, 19) //[19:19]
    #define DRAMC_PD_CTRL_DQIEN_BUFFEN_OPT                     Fld(2, 20) //[21:20]
    #define DRAMC_PD_CTRL_MIOCKCTRLOFF                         Fld(1, 26) //[26:26]
    #define DRAMC_PD_CTRL_DISSTOP26M                           Fld(1, 27) //[27:27]
    #define DRAMC_PD_CTRL_PHYCLKDYNGEN                         Fld(1, 30) //[30:30]
    #define DRAMC_PD_CTRL_COMBCLKCTRL                          Fld(1, 31) //[31:31]

#define DRAMC_REG_CLKAR                                    (DRAMC_AO_BASE + 0x0003C)
    #define CLKAR_REQQUE_PACG_DIS                              Fld(15, 0) //[14:0]
    #define CLKAR_SELPH_CMD_CG_DIS                             Fld(1, 15) //[15:15]
    #define CLKAR_RDATCKAR                                     Fld(1, 16) //[16:16]
    #define CLKAR_SRF_CLKRUN                                   Fld(1, 17) //[17:17]
    #define CLKAR_IDLE_OPT                                     Fld(1, 18) //[18:18]
    #define CLKAR_PSELAR                                       Fld(1, 19) //[19:19]
    #define CLKAR_BCLKAR                                       Fld(1, 20) //[20:20]
    #define CLKAR_SELPH_4LCG_DIS                               Fld(1, 21) //[21:21]
    #define CLKAR_SELPH_CG_DIS                                 Fld(1, 22) //[22:22]
    #define CLKAR_TESTCLKRUN                                   Fld(1, 23) //[23:23]
    #define CLKAR_PHYGLUECLKRUN                                Fld(1, 24) //[24:24]
    #define CLKAR_DWCLKRUN                                     Fld(1, 25) //[25:25]
    #define CLKAR_REFCLKRUN                                    Fld(1, 26) //[26:26]
    #define CLKAR_REQQUECLKRUN                                 Fld(1, 27) //[27:27]
    #define CLKAR_SEQCLKRUN                                    Fld(1, 28) //[28:28]
    #define CLKAR_CALCKAR                                      Fld(1, 29) //[29:29]
    #define CLKAR_CMDCKAR                                      Fld(1, 30) //[30:30]
    #define CLKAR_RDYCKAR                                      Fld(1, 31) //[31:31]

#define DRAMC_REG_CLKCTRL                                  (DRAMC_AO_BASE + 0x00040)
    #define CLKCTRL_PSEL_CNT                                   Fld(6, 0) //[5:0]
    #define CLKCTRL_SEQCLKRUN3                                 Fld(1, 7) //[7:7]
    #define CLKCTRL_SEQCLKRUN2                                 Fld(1, 8) //[8:8]
    #define CLKCTRL_CLK_EN_0                                   Fld(1, 28) //[28:28]
    #define CLKCTRL_CLK_EN_1                                   Fld(1, 29) //[29:29]

#define DRAMC_REG_SELFREF_HWSAVE_FLAG                      (DRAMC_AO_BASE + 0x00044)
    #define SELFREF_HWSAVE_FLAG_SELFREF_HWSAVE_FLAG_FROM_AO    Fld(1, 0) //[0:0]

#define DRAMC_REG_SREFCTRL                                 (DRAMC_AO_BASE + 0x00048)
    #define SREFCTRL_HMRRSEL_CGAR                              Fld(1, 12) //[12:12]
    #define SREFCTRL_RDDQSOSC_CGAR                             Fld(1, 13) //[13:13]
    #define SREFCTRL_SCARB_SM_CGAR                             Fld(1, 14) //[14:14]
    #define SREFCTRL_SCSM_CGAR                                 Fld(1, 15) //[15:15]
    #define SREFCTRL_SRFPD_DIS                                 Fld(1, 16) //[16:16]
    #define SREFCTRL_DQSOSC_THRD_OPT                           Fld(1, 17) //[17:17]
    #define SREFCTRL_DQSOSC_C2R_OPT                            Fld(1, 18) //[18:18]
    #define SREFCTRL_SREF3_OPTION                              Fld(1, 20) //[20:20]
    #define SREFCTRL_SREF3_OPTION1                             Fld(1, 21) //[21:21]
    #define SREFCTRL_SREF2_OPTION                              Fld(1, 22) //[22:22]
    #define SREFCTRL_SREFDLY                                   Fld(4, 24) //[27:24]
    #define SREFCTRL_SREF_HW_EN                                Fld(1, 30) //[30:30]
    #define SREFCTRL_SELFREF                                   Fld(1, 31) //[31:31]

#define DRAMC_REG_REFCTRL0                                 (DRAMC_AO_BASE + 0x0004C)
    #define REFCTRL0_DLLFRZ                                    Fld(1, 0) //[0:0]
    #define REFCTRL0_UPDBYWR                                   Fld(1, 1) //[1:1]
    #define REFCTRL0_DRVCGWREF                                 Fld(1, 2) //[2:2]
    #define REFCTRL0_DQDRVSWUPD                                Fld(1, 3) //[3:3]
    #define REFCTRL0_RFRINTCTL                                 Fld(1, 5) //[5:5]
    #define REFCTRL0_RFRINTEN                                  Fld(1, 6) //[6:6]
    #define REFCTRL0_REFOVERCNT_RST                            Fld(1, 7) //[7:7]
    #define REFCTRL0_DMPGVLD_IG                                Fld(1, 8) //[8:8]
    #define REFCTRL0_REFMODE_MANUAL                            Fld(1, 10) //[10:10]
    #define REFCTRL0_REFMODE_MANUAL_TRIG                       Fld(1, 11) //[11:11]
    #define REFCTRL0_DISBYREFNUM                               Fld(3, 12) //[14:12]
    #define REFCTRL0_PBREF_DISBYREFNUM                         Fld(1, 16) //[16:16]
    #define REFCTRL0_PBREF_DISBYRATE                           Fld(1, 17) //[17:17]
    #define REFCTRL0_PBREFEN                                   Fld(1, 18) //[18:18]
    #define REFCTRL0_ADVREF_CNT                                Fld(4, 20) //[23:20]
    #define REFCTRL0_REF_PREGATE_CNT                           Fld(4, 24) //[27:24]
    #define REFCTRL0_REFNA_OPT                                 Fld(1, 28) //[28:28]
    #define REFCTRL0_REFDIS                                    Fld(1, 29) //[29:29]
    #define REFCTRL0_REFFRERUN                                 Fld(1, 30) //[30:30]
    #define REFCTRL0_REFBW_FREN                                Fld(1, 31) //[31:31]

#define DRAMC_REG_REFCTRL1                                 (DRAMC_AO_BASE + 0x00050)
    #define REFCTRL1_SLEFREF_AUTOSAVE_EN                       Fld(1, 0) //[0:0]
    #define REFCTRL1_SREF_PRD_OPT                              Fld(1, 1) //[1:1]
    #define REFCTRL1_PSEL_OPT2                                 Fld(1, 2) //[2:2]
    #define REFCTRL1_PSEL_OPT3                                 Fld(1, 3) //[3:3]
    #define REFCTRL1_PRE8REF                                   Fld(1, 4) //[4:4]
    #define REFCTRL1_REF_QUE_AUTOSAVE_EN                       Fld(1, 5) //[5:5]
    #define REFCTRL1_PSEL_OPT1                                 Fld(1, 6) //[6:6]
    #define REFCTRL1_SREF_CG_OPT                               Fld(1, 7) //[7:7]
    #define REFCTRL1_MPENDREF_CNT                              Fld(3, 8) //[10:8]
    #define REFCTRL1_REFRATE_MON_CLR                           Fld(1, 11) //[11:11]
    #define REFCTRL1_REF_OVERHEAD_RATE_REFAL_ENA               Fld(1, 14) //[14:14]
    #define REFCTRL1_REF_OVERHEAD_RATE_REFPB_ENA               Fld(1, 15) //[15:15]
    #define REFCTRL1_REF_OVERHEAD_RATE                         Fld(8, 16) //[23:16]
    #define REFCTRL1_REF_OVERHEAD_SLOW_REFAL_ENA               Fld(1, 24) //[24:24]
    #define REFCTRL1_REF_OVERHEAD_SLOW_REFPB_ENA               Fld(1, 25) //[25:25]
    #define REFCTRL1_REF_OVERHEAD_ALL_REFAL_ENA                Fld(1, 26) //[26:26]
    #define REFCTRL1_REF_OVERHEAD_ALL_REFPB_ENA                Fld(1, 27) //[27:27]
    #define REFCTRL1_REFRATE_MANUAL                            Fld(3, 28) //[30:28]
    #define REFCTRL1_REFRATE_MANUAL_RATE_TRIG                  Fld(1, 31) //[31:31]

#define DRAMC_REG_REFRATRE_FILTER                          (DRAMC_AO_BASE + 0x00054)
    #define REFRATRE_FILTER_REFRATE_FIL7                       Fld(3, 0) //[2:0]
    #define REFRATRE_FILTER_REFRATE_FIL6                       Fld(3, 4) //[6:4]
    #define REFRATRE_FILTER_REFRATE_FIL5                       Fld(3, 8) //[10:8]
    #define REFRATRE_FILTER_REFRATE_FIL4                       Fld(3, 12) //[14:12]
    #define REFRATRE_FILTER_PB2AB_OPT                          Fld(1, 15) //[15:15]
    #define REFRATRE_FILTER_REFRATE_FIL3                       Fld(3, 16) //[18:16]
    #define REFRATRE_FILTER_REFRATE_FIL2                       Fld(3, 20) //[22:20]
    #define REFRATRE_FILTER_PB2AB_OPT1                         Fld(1, 23) //[23:23]
    #define REFRATRE_FILTER_REFRATE_FIL1                       Fld(3, 24) //[26:24]
    #define REFRATRE_FILTER_REFRATE_FIL0                       Fld(3, 28) //[30:28]
    #define REFRATRE_FILTER_REFRATE_FILEN                      Fld(1, 31) //[31:31]

#define DRAMC_REG_ZQCS                                     (DRAMC_AO_BASE + 0x00058)
    #define ZQCS_ZQCSOP                                        Fld(8, 0) //[7:0]
    #define ZQCS_ZQCSAD                                        Fld(8, 8) //[15:8]
    #define ZQCS_ZQCS_MASK_SEL                                 Fld(3, 16) //[18:16]
    #define ZQCS_ZQCS_MASK_SEL_CGAR                            Fld(1, 19) //[19:19]
    #define ZQCS_ZQMASK_CGAR                                   Fld(1, 20) //[20:20]
    #define ZQCS_ZQCSMASK_OPT                                  Fld(1, 21) //[21:21]
    #define ZQCS_ZQ_SRF_OPT                                    Fld(1, 22) //[22:22]
    #define ZQCS_ZQCSMASK                                      Fld(1, 30) //[30:30]
    #define ZQCS_ZQCSDUAL                                      Fld(1, 31) //[31:31]

#define DRAMC_REG_MRS                                      (DRAMC_AO_BASE + 0x0005C)
    #define MRS_MRSOP                                          Fld(8, 0) //[7:0]
    #define MRS_MRSMA                                          Fld(13, 8) //[20:8]
    #define MRS_MRSBA                                          Fld(3, 21) //[23:21]
    #define MRS_MRSRK                                          Fld(2, 24) //[25:24]
    #define MRS_MRRRK                                          Fld(2, 26) //[27:26]
    #define MRS_MPCRK                                          Fld(2, 28) //[29:28]
    #define MRS_MRSBG                                          Fld(2, 30) //[31:30]

#define DRAMC_REG_SPCMD                                    (DRAMC_AO_BASE + 0x00060)
    #define SPCMD_MRWEN                                        Fld(1, 0) //[0:0]
    #define SPCMD_MRREN                                        Fld(1, 1) //[1:1]
    #define SPCMD_PREAEN                                       Fld(1, 2) //[2:2]
    #define SPCMD_AREFEN                                       Fld(1, 3) //[3:3]
    #define SPCMD_ZQCEN                                        Fld(1, 4) //[4:4]
    #define SPCMD_TCMDEN                                       Fld(1, 5) //[5:5]
    #define SPCMD_ZQLATEN                                      Fld(1, 6) //[6:6]
    #define SPCMD_RDDQCEN                                      Fld(1, 7) //[7:7]
    #define SPCMD_DQSGCNTEN                                    Fld(1, 8) //[8:8]
    #define SPCMD_DQSGCNTRST                                   Fld(1, 9) //[9:9]
    #define SPCMD_DQSOSCENEN                                   Fld(1, 10) //[10:10]
    #define SPCMD_DQSOSCDISEN                                  Fld(1, 11) //[11:11]
    #define SPCMD_ACTEN                                        Fld(1, 12) //[12:12]
    #define SPCMD_MPRWEN                                       Fld(1, 13) //[13:13]

#define DRAMC_REG_SPCMDCTRL                                (DRAMC_AO_BASE + 0x00064)
    #define SPCMDCTRL_SC_PG_UPD_OPT                            Fld(1, 0) //[0:0]
    #define SPCMDCTRL_SC_PG_MAN_DIS                            Fld(1, 1) //[1:1]
    #define SPCMDCTRL_SPREA_EN                                 Fld(1, 2) //[2:2]
    #define SPCMDCTRL_SCARB_PRI_OPT                            Fld(1, 4) //[4:4]
    #define SPCMDCTRL_MRRSWUPD                                 Fld(1, 5) //[5:5]
    #define SPCMDCTRL_R_DMDVFSMRW_EN                           Fld(1, 6) //[6:6]
    #define SPCMDCTRL_DPDWOSC                                  Fld(1, 7) //[7:7]
    #define SPCMDCTRL_SC_PG_OPT2_DIS                           Fld(1, 8) //[8:8]
    #define SPCMDCTRL_SC_PG_STCMD_AREF_DIS                     Fld(1, 9) //[9:9]
    #define SPCMDCTRL_SC_PG_MPRW_DIS                           Fld(1, 10) //[10:10]
    #define SPCMDCTRL_RDDQCDIS                                 Fld(1, 11) //[11:11]
    #define SPCMDCTRL_RDDQC_4TO1_OPT                           Fld(1, 12) //[12:12]
    #define SPCMDCTRL_HMR4_TOG_OPT                             Fld(1, 18) //[18:18]
    #define SPCMDCTRL_SCPRE                                    Fld(1, 19) //[19:19]
    #define SPCMDCTRL_ZQCS_NONMASK_CLR                         Fld(1, 20) //[20:20]
    #define SPCMDCTRL_ZQCS_MASK_FIX                            Fld(1, 21) //[21:21]
    #define SPCMDCTRL_ZQCS_MASK_VALUE                          Fld(1, 22) //[22:22]
    #define SPCMDCTRL_SPDR_MR4_OPT                             Fld(1, 23) //[23:23]
    #define SPCMDCTRL_SRFMR4_CNTKEEP_B                         Fld(1, 24) //[24:24]
    #define SPCMDCTRL_MRWWOPRA                                 Fld(1, 25) //[25:25]
    #define SPCMDCTRL_CLR_EN                                   Fld(1, 26) //[26:26]
    #define SPCMDCTRL_MRRREFUPD_B                              Fld(1, 27) //[27:27]
    #define SPCMDCTRL_REFR_BLOCKEN                             Fld(1, 28) //[28:28]
    #define SPCMDCTRL_REFRDIS                                  Fld(1, 29) //[29:29]
    #define SPCMDCTRL_ZQCALDISB                                Fld(1, 30) //[30:30]
    #define SPCMDCTRL_ZQCSDISB                                 Fld(1, 31) //[31:31]

#define DRAMC_REG_PPR_CTRL                                 (DRAMC_AO_BASE + 0x00068)
    #define PPR_CTRL_XSR_TX_RETRY_BLOCK_ALE_MASK               Fld(1, 0) //[0:0]
    #define PPR_CTRL_XSR_TX_RETRY_OPT                          Fld(1, 1) //[1:1]
    #define PPR_CTRL_XSR_TX_RETRY_EN                           Fld(1, 2) //[2:2]
    #define PPR_CTRL_XSR_TX_RETRY_SPM_MODE                     Fld(1, 3) //[3:3]
    #define PPR_CTRL_XSR_TX_RETRY_SW_EN                        Fld(1, 4) //[4:4]
    #define PPR_CTRL_TX_RETRY_UPDPI_CG_OPT                     Fld(1, 5) //[5:5]
    #define PPR_CTRL_ACTEN_BK                                  Fld(3, 12) //[14:12]
    #define PPR_CTRL_ACTEN_ROW                                 Fld(16, 16) //[31:16]

#define DRAMC_REG_MPC_OPTION                               (DRAMC_AO_BASE + 0x0006C)
    #define MPC_OPTION_MPC_BLOCKALE_OPT                        Fld(1, 0) //[0:0]
    #define MPC_OPTION_MPC_BLOCKALE_OPT1                       Fld(1, 1) //[1:1]
    #define MPC_OPTION_MPC_BLOCKALE_OPT2                       Fld(1, 2) //[2:2]
    #define MPC_OPTION_ZQ_BLOCKALE_OPT                         Fld(1, 3) //[3:3]
    #define MPC_OPTION_RW2ZQLAT_OPT                            Fld(1, 4) //[4:4]
    #define MPC_OPTION_MPCOP                                   Fld(7, 8) //[14:8]
    #define MPC_OPTION_MPCMANEN                                Fld(1, 15) //[15:15]
    #define MPC_OPTION_MPCMAN_CAS2EN                           Fld(1, 16) //[16:16]
    #define MPC_OPTION_MPCRKEN                                 Fld(1, 17) //[17:17]

#define DRAMC_REG_REFQUE_CNT                               (DRAMC_AO_BASE + 0x00070)
    #define REFQUE_CNT_REFRESH_QUEUE_CNT_FROM_AO               Fld(4, 0) //[3:0]

#define DRAMC_REG_HW_MRR_FUN                               (DRAMC_AO_BASE + 0x00074)
    #define HW_MRR_FUN_TMRR_ENA                                Fld(1, 0) //[0:0]
    #define HW_MRR_FUN_TRCDMRR_EN                              Fld(1, 1) //[1:1]
    #define HW_MRR_FUN_TRPMRR_EN                               Fld(1, 2) //[2:2]
    #define HW_MRR_FUN_MANTMRR_EN                              Fld(1, 3) //[3:3]
    #define HW_MRR_FUN_TR2MRR_ENA                              Fld(1, 4) //[4:4]
    #define HW_MRR_FUN_R2MRRHPRICTL                            Fld(1, 5) //[5:5]
    #define HW_MRR_FUN_BUFEN_RFC_OPT                           Fld(1, 8) //[8:8]
    #define HW_MRR_FUN_MRR_REQNOPUSH_DIS                       Fld(1, 9) //[9:9]
    #define HW_MRR_FUN_MRR_BLOCK_NOR_DIS                       Fld(1, 10) //[10:10]
    #define HW_MRR_FUN_MRR_HW_HIPRI                            Fld(1, 11) //[11:11]
    #define HW_MRR_FUN_MRR_SPCMD_WAKE_DIS                      Fld(1, 12) //[12:12]
    #define HW_MRR_FUN_TMRR_OE_OPT_DIS                         Fld(1, 13) //[13:13]
    #define HW_MRR_FUN_MRR_PUSH2POP_ENA                        Fld(1, 16) //[16:16]
    #define HW_MRR_FUN_MRR_PUSH2POP_CLR                        Fld(1, 17) //[17:17]
    #define HW_MRR_FUN_MRR_PUSH2POP_ST_CLR                     Fld(1, 18) //[18:18]
    #define HW_MRR_FUN_MRR_PUSH2POP_SEL                        Fld(3, 20) //[22:20]
    #define HW_MRR_FUN_MRR_SBR3_BKVA_DIS                       Fld(1, 23) //[23:23]
    #define HW_MRR_FUN_MRR_DDRCLKCOMB_DIS                      Fld(1, 24) //[24:24]
    #define HW_MRR_FUN_TRPRCD_DIS_OPT1                         Fld(1, 25) //[25:25]
    #define HW_MRR_FUN_TRPRCD_OPT2                             Fld(1, 26) //[26:26]
    #define HW_MRR_FUN_MRR_SBR2_QHIT_DIS                       Fld(1, 27) //[27:27]
    #define HW_MRR_FUN_MRR_INPUT_BANK                          Fld(3, 28) //[30:28]
    #define HW_MRR_FUN_MRR_TZQCS_DIS                           Fld(1, 31) //[31:31]

#define DRAMC_REG_MRR_BIT_MUX1                             (DRAMC_AO_BASE + 0x00078)
    #define MRR_BIT_MUX1_MRR_BIT0_SEL                          Fld(5, 0) //[4:0]
    #define MRR_BIT_MUX1_MRR_BIT1_SEL                          Fld(5, 8) //[12:8]
    #define MRR_BIT_MUX1_MRR_BIT2_SEL                          Fld(5, 16) //[20:16]
    #define MRR_BIT_MUX1_MRR_BIT3_SEL                          Fld(5, 24) //[28:24]

#define DRAMC_REG_MRR_BIT_MUX2                             (DRAMC_AO_BASE + 0x0007C)
    #define MRR_BIT_MUX2_MRR_BIT4_SEL                          Fld(5, 0) //[4:0]
    #define MRR_BIT_MUX2_MRR_BIT5_SEL                          Fld(5, 8) //[12:8]
    #define MRR_BIT_MUX2_MRR_BIT6_SEL                          Fld(5, 16) //[20:16]
    #define MRR_BIT_MUX2_MRR_BIT7_SEL                          Fld(5, 24) //[28:24]

#define DRAMC_REG_MRR_BIT_MUX3                             (DRAMC_AO_BASE + 0x00080)
    #define MRR_BIT_MUX3_MRR_BIT8_SEL                          Fld(5, 0) //[4:0]
    #define MRR_BIT_MUX3_MRR_BIT9_SEL                          Fld(5, 8) //[12:8]
    #define MRR_BIT_MUX3_MRR_BIT10_SEL                         Fld(5, 16) //[20:16]
    #define MRR_BIT_MUX3_MRR_BIT11_SEL                         Fld(5, 24) //[28:24]

#define DRAMC_REG_MRR_BIT_MUX4                             (DRAMC_AO_BASE + 0x00084)
    #define MRR_BIT_MUX4_MRR_BIT12_SEL                         Fld(5, 0) //[4:0]
    #define MRR_BIT_MUX4_MRR_BIT13_SEL                         Fld(5, 8) //[12:8]
    #define MRR_BIT_MUX4_MRR_BIT14_SEL                         Fld(5, 16) //[20:16]
    #define MRR_BIT_MUX4_MRR_BIT15_SEL                         Fld(5, 24) //[28:24]

#define DRAMC_REG_TX_PI_DBG                                (DRAMC_AO_BASE + 0x00088)
    #define TX_PI_DBG_RK0_B0_PI_CHANGE_DBG                     Fld(1, 0) //[0:0]
    #define TX_PI_DBG_RK0_B1_PI_CHANGE_DBG                     Fld(1, 1) //[1:1]
    #define TX_PI_DBG_RK1_B0_PI_CHANGE_DBG                     Fld(1, 2) //[2:2]
    #define TX_PI_DBG_RK1_B1_PI_CHANGE_DBG                     Fld(1, 3) //[3:3]

#define DRAMC_REG_TEST2_5                                  (DRAMC_AO_BASE + 0x0008C)
    #define TEST2_5_TEST2_BASE_2                               Fld(28, 4) //[31:4]

#define DRAMC_REG_TEST2_0                                  (DRAMC_AO_BASE + 0x00090)
    #define TEST2_0_TEST2_PAT1                                 Fld(8, 0) //[7:0]
    #define TEST2_0_TEST2_PAT0                                 Fld(8, 8) //[15:8]

#define DRAMC_REG_TEST2_1                                  (DRAMC_AO_BASE + 0x00094)
    #define TEST2_1_TEST2_BASE                                 Fld(28, 4) //[31:4]

#define DRAMC_REG_TEST2_2                                  (DRAMC_AO_BASE + 0x00098)
    #define TEST2_2_TEST2_OFF                                  Fld(28, 4) //[31:4]

#define DRAMC_REG_TEST2_3                                  (DRAMC_AO_BASE + 0x0009C)
    #define TEST2_3_TESTCNT                                    Fld(4, 0) //[3:0]
    #define TEST2_3_DQSICALEN                                  Fld(1, 4) //[4:4]
    #define TEST2_3_DQSICALUPD                                 Fld(1, 5) //[5:5]
    #define TEST2_3_PSTWR2                                     Fld(1, 6) //[6:6]
    #define TEST2_3_TESTAUDPAT                                 Fld(1, 7) //[7:7]
    #define TEST2_3_DQSICALSTP                                 Fld(3, 8) //[10:8]
    #define TEST2_3_DQDLYAUTO                                  Fld(1, 11) //[11:11]
    #define TEST2_3_MANUDLLFRZ                                 Fld(1, 12) //[12:12]
    #define TEST2_3_MANUDQSUPD                                 Fld(1, 13) //[13:13]
    #define TEST2_3_DQSUPDMODE                                 Fld(1, 14) //[14:14]
    #define TEST2_3_DRDELSWEN                                  Fld(1, 19) //[19:19]
    #define TEST2_3_DRDELSWSEL                                 Fld(3, 20) //[22:20]
    #define TEST2_3_MDQS                                       Fld(1, 23) //[23:23]
    #define TEST2_3_DMPAT32                                    Fld(1, 24) //[24:24]
    #define TEST2_3_TESTADR_SHIFT                              Fld(1, 25) //[25:25]
    #define TEST2_3_TAHPRI_B                                   Fld(1, 26) //[26:26]
    #define TEST2_3_TESTLP                                     Fld(1, 27) //[27:27]
    #define TEST2_3_TEST2WREN2_HW_EN                           Fld(1, 28) //[28:28]
    #define TEST2_3_TEST1                                      Fld(1, 29) //[29:29]
    #define TEST2_3_TEST2R                                     Fld(1, 30) //[30:30]
    #define TEST2_3_TEST2W                                     Fld(1, 31) //[31:31]

#define DRAMC_REG_TEST2_4                                  (DRAMC_AO_BASE + 0x000A0)
    #define TEST2_4_TESTAUDINC                                 Fld(5, 0) //[4:0]
    #define TEST2_4_TEST2DISSCRAM                              Fld(1, 5) //[5:5]
    #define TEST2_4_TESTSSOPAT                                 Fld(1, 6) //[6:6]
    #define TEST2_4_TESTSSOXTALKPAT                            Fld(1, 7) //[7:7]
    #define TEST2_4_TESTAUDINIT                                Fld(5, 8) //[12:8]
    #define TEST2_4_TEST2_EN1ARB_DIS                           Fld(1, 13) //[13:13]
    #define TEST2_4_TESTAUDBITINV                              Fld(1, 14) //[14:14]
    #define TEST2_4_TESTAUDMODE                                Fld(1, 15) //[15:15]
    #define TEST2_4_TESTXTALKPAT                               Fld(1, 16) //[16:16]
    #define TEST2_4_TEST_REQ_LEN1                              Fld(1, 17) //[17:17]
    #define TEST2_4_TEST2EN1_OPT2                              Fld(1, 18) //[18:18]
    #define TEST2_4_TEST2EN1_OPT1_DIS                          Fld(1, 19) //[19:19]
    #define TEST2_4_DISMASK                                    Fld(1, 20) //[20:20]
    #define TEST2_4_TEST2_DQMTGL                               Fld(1, 21) //[21:21]
    #define TEST2_4_DQCALDIS                                   Fld(1, 22) //[22:22]
    #define TEST2_4_NEGDQS                                     Fld(1, 23) //[23:23]
    #define TEST2_4_TESTAGENTRK                                Fld(2, 24) //[25:24]
    #define TEST2_4_TESTDMITGLPAT                              Fld(1, 26) //[26:26]
    #define TEST2_4_TEST1TO4LEN1_DIS                           Fld(1, 27) //[27:27]
    #define TEST2_4_TESTAGENTRKSEL                             Fld(3, 28) //[30:28]

#define DRAMC_REG_WDT_DBG_SIGNAL                           (DRAMC_AO_BASE + 0x000A4)
    #define WDT_DBG_SIGNAL_LATCH_RD_TEST_DM_CMP_CPT2_RK0       Fld(1, 0) //[0:0]
    #define WDT_DBG_SIGNAL_LATCH_RD_TEST_DM_CMP_CPT2_RK1       Fld(1, 1) //[1:1]
    #define WDT_DBG_SIGNAL_LATCH_RD_TEST_DM_CMP_ERR2_RK0       Fld(1, 2) //[2:2]
    #define WDT_DBG_SIGNAL_LATCH_RD_TEST_DM_CMP_ERR2_RK1       Fld(1, 3) //[3:3]
    #define WDT_DBG_SIGNAL_LATCH_RD_TEST_DLE_CNT_OK2_RK0       Fld(1, 4) //[4:4]
    #define WDT_DBG_SIGNAL_LATCH_RD_TEST_DLE_CNT_OK2_RK1       Fld(1, 5) //[5:5]
    #define WDT_DBG_SIGNAL_LATCH_RDWR_TEST_DM_CMP_CPT2_RK0     Fld(1, 8) //[8:8]
    #define WDT_DBG_SIGNAL_LATCH_RDWR_TEST_DM_CMP_CPT2_RK1     Fld(1, 9) //[9:9]
    #define WDT_DBG_SIGNAL_LATCH_RDWR_TEST_DM_CMP_ERR2_RK0     Fld(1, 10) //[10:10]
    #define WDT_DBG_SIGNAL_LATCH_RDWR_TEST_DM_CMP_ERR2_RK1     Fld(1, 11) //[11:11]
    #define WDT_DBG_SIGNAL_LATCH_RDWR_TEST_DLE_CNT_OK2_RK0     Fld(1, 12) //[12:12]
    #define WDT_DBG_SIGNAL_LATCH_RDWR_TEST_DLE_CNT_OK2_RK1     Fld(1, 13) //[13:13]
    #define WDT_DBG_SIGNAL_LATCH_DRAMC_GATING_ERROR            Fld(1, 14) //[14:14]

#define DRAMC_REG_LBTEST                                   (DRAMC_AO_BASE + 0x000AC)
    #define LBTEST_LBTEST_IGB0                                 Fld(1, 0) //[0:0]
    #define LBTEST_LBTEST_IGB1                                 Fld(1, 1) //[1:1]
    #define LBTEST_LBTEST_IGB2                                 Fld(1, 2) //[2:2]
    #define LBTEST_LBTEST_IGB3                                 Fld(1, 3) //[3:3]
    #define LBTEST_LBTEST                                      Fld(1, 4) //[4:4]
    #define LBTEST_LBTEST_MODE                                 Fld(1, 5) //[5:5]
    #define LBTEST_OCDPAT                                      Fld(8, 16) //[23:16]
    #define LBTEST_OCDADJ                                      Fld(1, 24) //[24:24]
    #define LBTEST_RDCOLADR                                    Fld(1, 29) //[29:29]
    #define LBTEST_RDWRDATA                                    Fld(1, 30) //[30:30]
    #define LBTEST_RDTGDATA                                    Fld(1, 31) //[31:31]

#define DRAMC_REG_CATRAINING1                              (DRAMC_AO_BASE + 0x000B0)
    #define CATRAINING1_CATRAINEN                              Fld(1, 1) //[1:1]
    #define CATRAINING1_CATRAINMRS                             Fld(1, 2) //[2:2]
    #define CATRAINING1_TESTCATRAIN                            Fld(1, 5) //[5:5]
    #define CATRAINING1_CSTRAIN_OPTION                         Fld(1, 6) //[6:6]
    #define CATRAINING1_CATRAINCSEXT                           Fld(1, 13) //[13:13]
    #define CATRAINING1_CATRAINLAT                             Fld(4, 20) //[23:20]
    #define CATRAINING1_CATRAIN_INTV                           Fld(8, 24) //[31:24]

#define DRAMC_REG_CATRAINING2                              (DRAMC_AO_BASE + 0x000B4)
    #define CATRAINING2_CATRAINCA                              Fld(16, 0) //[15:0]
    #define CATRAINING2_CATRAINCA_Y                            Fld(16, 16) //[31:16]

#define DRAMC_REG_WRITE_LEV                                (DRAMC_AO_BASE + 0x000BC)
    #define WRITE_LEV_WRITE_LEVEL_EN                           Fld(1, 0) //[0:0]
    #define WRITE_LEV_DDRPHY_COMB_CG_SEL                       Fld(1, 2) //[2:2]
    #define WRITE_LEV_BYTEMODECBTEN                            Fld(1, 3) //[3:3]
    #define WRITE_LEV_BTCBTFIXDQSOE                            Fld(1, 4) //[4:4]
    #define WRITE_LEV_CBTMASKDQSOE                             Fld(1, 5) //[5:5]
    #define WRITE_LEV_DQS_OE_WLEV_OP                           Fld(1, 6) //[6:6]
    #define WRITE_LEV_DQS_WLEV                                 Fld(1, 7) //[7:7]
    #define WRITE_LEV_DQSBX_G                                  Fld(4, 8) //[11:8]
    #define WRITE_LEV_DQSBY_G                                  Fld(4, 12) //[15:12]
    #define WRITE_LEV_DQS_SEL                                  Fld(4, 16) //[19:16]
    #define WRITE_LEV_DMVREFCA                                 Fld(8, 20) //[27:20]
    #define WRITE_LEV_DQS_OE_OP1_DIS                           Fld(1, 28) //[28:28]
    #define WRITE_LEV_DQS_OE_OP2_EN                            Fld(1, 29) //[29:29]

#define DRAMC_REG_MR_GOLDEN                                (DRAMC_AO_BASE + 0x000C0)
    #define MR_GOLDEN_MR20_GOLDEN                              Fld(8, 0) //[7:0]
    #define MR_GOLDEN_MR15_GOLDEN                              Fld(8, 8) //[15:8]
    #define MR_GOLDEN_MR40_GOLDEN                              Fld(8, 16) //[23:16]
    #define MR_GOLDEN_MR32_GOLDEN                              Fld(8, 24) //[31:24]

#define DRAMC_REG_SLP4_TESTMODE                            (DRAMC_AO_BASE + 0x000C4)
    #define SLP4_TESTMODE_CA0_TEST                             Fld(4, 0) //[3:0]
    #define SLP4_TESTMODE_CA1_TEST                             Fld(4, 4) //[7:4]
    #define SLP4_TESTMODE_CA2_TEST                             Fld(4, 8) //[11:8]
    #define SLP4_TESTMODE_CA3_TEST                             Fld(4, 12) //[15:12]
    #define SLP4_TESTMODE_CA4_TEST                             Fld(4, 16) //[19:16]
    #define SLP4_TESTMODE_CA5_TEST                             Fld(4, 20) //[23:20]
    #define SLP4_TESTMODE_STESTEN                              Fld(1, 24) //[24:24]
    #define SLP4_TESTMODE_SPEC_MODE                            Fld(1, 25) //[25:25]
    #define SLP4_TESTMODE_ARPI_CAL_E2OPT                       Fld(1, 26) //[26:26]
    #define SLP4_TESTMODE_TX_DLY_CAL_E2OPT                     Fld(1, 27) //[27:27]
    #define SLP4_TESTMODE_DQM_FIX_LOW                          Fld(1, 28) //[28:28]

#define DRAMC_REG_DQSOSCR                                  (DRAMC_AO_BASE + 0x000C8)
    #define DQSOSCR_DQSOSC_INTEN                               Fld(1, 0) //[0:0]
    #define DQSOSCR_RK2_BYTE_MODE                              Fld(1, 1) //[1:1]
    #define DQSOSCR_TXUPD_BLOCK_SEL                            Fld(2, 2) //[3:2]
    #define DQSOSCR_TXUPD_BLOCK_OPT                            Fld(1, 4) //[4:4]
    #define DQSOSCR_TXUPDMODE                                  Fld(1, 5) //[5:5]
    #define DQSOSCR_MANUTXUPD                                  Fld(1, 6) //[6:6]
    #define DQSOSCR_ARUIDQ_SW                                  Fld(1, 7) //[7:7]
    #define DQSOSCR_DQS2DQ_UPD_BLOCK_CNT                       Fld(5, 8) //[12:8]
    #define DQSOSCR_TDQS2DQ_UPD_BLOCKING                       Fld(1, 13) //[13:13]
    #define DQSOSCR_DQS2DQ_UPD_MON_OPT                         Fld(1, 14) //[14:14]
    #define DQSOSCR_DQS2DQ_UPD_MON_CNT_SEL                     Fld(2, 15) //[16:15]
    #define DQSOSCR_TXUPD_IDLE_SEL                             Fld(2, 17) //[18:17]
    #define DQSOSCR_TXUPD_ABREF_SEL                            Fld(2, 19) //[20:19]
    #define DQSOSCR_TXUPD_IDLE_OPT                             Fld(1, 21) //[21:21]
    #define DQSOSCR_DQS2DQ_SHU_HW_CAL_DIS                      Fld(1, 22) //[22:22]
    #define DQSOSCR_SREF_TXUI_RELOAD_OPT                       Fld(1, 23) //[23:23]
    #define DQSOSCR_DQSOSCRDIS                                 Fld(1, 24) //[24:24]
    #define DQSOSCR_RK1_BYTE_MODE                              Fld(1, 25) //[25:25]
    #define DQSOSCR_RK0_BYTE_MODE                              Fld(1, 26) //[26:26]
    #define DQSOSCR_SREF_TXPI_RELOAD_OPT                       Fld(1, 27) //[27:27]
    #define DQSOSCR_EMPTY_WRITE_OPT                            Fld(1, 28) //[28:28]
    #define DQSOSCR_TXUPD_ABREF_OPT                            Fld(1, 29) //[29:29]
    #define DQSOSCR_DQSOSCLOPAD                                Fld(1, 30) //[30:30]
    #define DQSOSCR_DQSOSC_CALEN                               Fld(1, 31) //[31:31]

#define DRAMC_REG_DUMMY_RD                                 (DRAMC_AO_BASE + 0x000D0)
    #define DUMMY_RD_SREF_DMYRD_MASK                           Fld(1, 0) //[0:0]
    #define DUMMY_RD_DMYRDOFOEN                                Fld(1, 1) //[1:1]
    #define DUMMY_RD_DUMMY_RD_SW                               Fld(1, 4) //[4:4]
    #define DUMMY_RD_DMYWR_LPRI_EN                             Fld(1, 5) //[5:5]
    #define DUMMY_RD_DMY_WR_DBG                                Fld(1, 6) //[6:6]
    #define DUMMY_RD_DMY_RD_DBG                                Fld(1, 7) //[7:7]
    #define DUMMY_RD_DUMMY_RD_CNT0                             Fld(1, 8) //[8:8]
    #define DUMMY_RD_DUMMY_RD_CNT1                             Fld(1, 9) //[9:9]
    #define DUMMY_RD_DUMMY_RD_CNT2                             Fld(1, 10) //[10:10]
    #define DUMMY_RD_DUMMY_RD_CNT3                             Fld(1, 11) //[11:11]
    #define DUMMY_RD_DUMMY_RD_CNT4                             Fld(1, 12) //[12:12]
    #define DUMMY_RD_DUMMY_RD_CNT5                             Fld(1, 13) //[13:13]
    #define DUMMY_RD_DUMMY_RD_CNT6                             Fld(1, 14) //[14:14]
    #define DUMMY_RD_DUMMY_RD_CNT7                             Fld(1, 15) //[15:15]
    #define DUMMY_RD_RANK_NUM                                  Fld(2, 16) //[17:16]
    #define DUMMY_RD_DUMMY_RD_EN                               Fld(1, 20) //[20:20]
    #define DUMMY_RD_SREF_DMYRD_EN                             Fld(1, 21) //[21:21]
    #define DUMMY_RD_DQSG_DMYRD_EN                             Fld(1, 22) //[22:22]
    #define DUMMY_RD_DQSG_DMYWR_EN                             Fld(1, 23) //[23:23]
    #define DUMMY_RD_DUMMY_RD_PA_OPT                           Fld(1, 24) //[24:24]
    #define DUMMY_RD_DMY_RD_RX_TRACK                           Fld(1, 25) //[25:25]
    #define DUMMY_RD_DMYRD_HPRI_DIS                            Fld(1, 26) //[26:26]
    #define DUMMY_RD_DMYRD_REORDER_DIS                         Fld(1, 27) //[27:27]

#define DRAMC_REG_SHUCTRL                                  (DRAMC_AO_BASE + 0x000D4)
    #define SHUCTRL_R_SHUFFLE_BLOCK_OPT                        Fld(2, 0) //[1:0]
    #define SHUCTRL_DVFS_CG_OPT                                Fld(1, 2) //[2:2]
    #define SHUCTRL_VRCG_EN                                    Fld(1, 4) //[4:4]
    #define SHUCTRL_SHU_PHYRST_SEL                             Fld(1, 5) //[5:5]
    #define SHUCTRL_R_DVFS_PICG_MARGIN2                        Fld(2, 6) //[7:6]
    #define SHUCTRL_DMSHU_CNT                                  Fld(6, 8) //[13:8]
    #define SHUCTRL_LPSM_BYPASS_B                              Fld(1, 16) //[16:16]
    #define SHUCTRL_R_DRAMC_CHA                                Fld(1, 17) //[17:17]
    #define SHUCTRL_R_NEW_SHU_MUX_SPM                          Fld(1, 19) //[19:19]
    #define SHUCTRL_R_MPDIV_SHU_GP                             Fld(3, 20) //[22:20]
    #define SHUCTRL_R_OTHER_SHU_GP                             Fld(2, 24) //[25:24]
    #define SHUCTRL_R_DVFS_PICG_MARGIN3                        Fld(2, 26) //[27:26]
    #define SHUCTRL_DMSHU_LOW                                  Fld(1, 29) //[29:29]
    #define SHUCTRL_DMSHU_DRAMC                                Fld(1, 31) //[31:31]

#define DRAMC_REG_SHUCTRL1                                 (DRAMC_AO_BASE + 0x000D8)
    #define SHUCTRL1_FC_PRDCNT                                 Fld(8, 0) //[7:0]
    #define SHUCTRL1_CKFSPE_PRDCNT                             Fld(8, 8) //[15:8]
    #define SHUCTRL1_CKFSPX_PRDCNT                             Fld(8, 16) //[23:16]
    #define SHUCTRL1_VRCGEN_PRDCNT                             Fld(8, 24) //[31:24]

#define DRAMC_REG_SHUCTRL2                                 (DRAMC_AO_BASE + 0x000DC)
    #define SHUCTRL2_R_DLL_IDLE                                Fld(6, 0) //[5:0]
    #define SHUCTRL2_SHUFFLE_CHANGE_FREQ_OPT                   Fld(1, 6) //[6:6]
    #define SHUCTRL2_R_DVFS_FSM_CLR                            Fld(1, 7) //[7:7]
    #define SHUCTRL2_R_DVFS_SREF_OPT                           Fld(1, 8) //[8:8]
    #define SHUCTRL2_R_DVFS_CDC_OPTION                         Fld(1, 9) //[9:9]
    #define SHUCTRL2_R_DVFS_PICG_MARGIN                        Fld(2, 10) //[11:10]
    #define SHUCTRL2_R_DVFS_DLL_CHA                            Fld(1, 12) //[12:12]
    #define SHUCTRL2_R_CDC_MUX_SEL_OPTION                      Fld(1, 13) //[13:13]
    #define SHUCTRL2_R_DVFS_PARK_N                             Fld(1, 14) //[14:14]
    #define SHUCTRL2_R_DVFS_OPTION                             Fld(1, 15) //[15:15]
    #define SHUCTRL2_SHU_PERIOD_GO_ZERO_CNT                    Fld(8, 16) //[23:16]
    #define SHUCTRL2_HWSET_WLRL                                Fld(1, 24) //[24:24]
    #define SHUCTRL2_MR13_SHU_EN                               Fld(1, 25) //[25:25]
    #define SHUCTRL2_R_DVFS_RG_CDC_TX_SEL                      Fld(1, 26) //[26:26]
    #define SHUCTRL2_R_DVFS_RG_CDC_SYNC_ENABLE                 Fld(1, 27) //[27:27]
    #define SHUCTRL2_R_SHU_RESTORE                             Fld(1, 28) //[28:28]
    #define SHUCTRL2_SHU_CLK_MASK                              Fld(1, 29) //[29:29]
    #define SHUCTRL2_DVFS_CKE_OPT                              Fld(1, 30) //[30:30]
    #define SHUCTRL2_SHORTQ_OPT                                Fld(1, 31) //[31:31]

#define DRAMC_REG_SHUCTRL3                                 (DRAMC_AO_BASE + 0x000E0)
    #define SHUCTRL3_VRCGDIS_MRSMA                             Fld(13, 0) //[12:0]
    #define SHUCTRL3_VRCGDISOP                                 Fld(8, 16) //[23:16]
    #define SHUCTRL3_VRCGDIS_PRDCNT                            Fld(8, 24) //[31:24]

#define DRAMC_REG_SHUSTATUS                                (DRAMC_AO_BASE + 0x000E4)
    #define SHUSTATUS_SHUFFLE_LEVEL                            Fld(2, 1) /* cc add */
    #define SHUSTATUS_SHUFFLE_END                              Fld(1, 0) //[0:0]
    #define SHUSTATUS_SHUFFLE_START_LOW                        Fld(1, 1) //[1:1]
    #define SHUSTATUS_SHUFFLE_START_LOW_THREE                  Fld(1, 2) //[2:2]
    #define SHUSTATUS_MPDIV_SHU_GP                             Fld(3, 4) //[6:4]

#define DRAMC_REG_SEDA_LOOP_BAK_ERR_PAT_B01                (DRAMC_AO_BASE + 0x000E8)
    #define SEDA_LOOP_BAK_ERR_PAT_B01_SEDA_LOOP_BAK_ERR_PAT0   Fld(32, 0) //[31:0]

#define DRAMC_REG_SEDA_LOOP_BAK_ERR_PAT_B23                (DRAMC_AO_BASE + 0x000EC)
    #define SEDA_LOOP_BAK_ERR_PAT_B23_SEDA_LOOP_BAK_ERR_PAT1   Fld(32, 0) //[31:0]

#define DRAMC_REG_SEDA_LOOP_BAK_ERR_PAT_B45                (DRAMC_AO_BASE + 0x000F0)
    #define SEDA_LOOP_BAK_ERR_PAT_B45_SEDA_LOOP_BAK_ERR_PAT2   Fld(32, 0) //[31:0]

#define DRAMC_REG_SEDA_LOOP_BAK_ERR_PAT_B67                (DRAMC_AO_BASE + 0x000F4)
    #define SEDA_LOOP_BAK_ERR_PAT_B67_SEDA_LOOP_BAK_ERR_PAT3   Fld(32, 0) //[31:0]

#define DRAMC_REG_BYPASS_FSPOP                             (DRAMC_AO_BASE + 0x00100)
    #define BYPASS_FSPOP_BPFSP_SET_SHU0                        Fld(1, 0) //[0:0]
    #define BYPASS_FSPOP_BPFSP_SET_SHU1                        Fld(1, 1) //[1:1]
    #define BYPASS_FSPOP_BPFSP_SET_SHU2                        Fld(1, 2) //[2:2]
    #define BYPASS_FSPOP_BPFSP_SET_SHU3                        Fld(1, 3) //[3:3]
    #define BYPASS_FSPOP_BPFSP_SET_SHU4                        Fld(1, 4) //[4:4]
    #define BYPASS_FSPOP_BPFSP_SET_SHU5                        Fld(1, 5) //[5:5]
    #define BYPASS_FSPOP_BPFSP_SET_SHU6                        Fld(1, 6) //[6:6]
    #define BYPASS_FSPOP_BPFSP_SET_SHU7                        Fld(1, 7) //[7:7]
    #define BYPASS_FSPOP_BPFSP_SET_SHU8                        Fld(1, 8) //[8:8]
    #define BYPASS_FSPOP_BPFSP_SET_SHU9                        Fld(1, 9) //[9:9]
    #define BYPASS_FSPOP_BPFSP_OPT                             Fld(1, 16) //[16:16]

#define DRAMC_REG_STBCAL                                   (DRAMC_AO_BASE + 0x00200)
    #define STBCAL_PIMASK_RKCHG_OPT                            Fld(1, 0) //[0:0]
    #define STBCAL_PIMASK_RKCHG_EXT                            Fld(3, 1) //[3:1]
    #define STBCAL_STBDLELAST_OPT                              Fld(1, 4) //[4:4]
    #define STBCAL_DLLFRZIDLE4XUPD                             Fld(1, 5) //[5:5]
    #define STBCAL_FASTDQSG2X                                  Fld(1, 6) //[6:6]
    #define STBCAL_FASTDQSGUPD                                 Fld(1, 7) //[7:7]
    #define STBCAL_STBDLELAST_PULSE                            Fld(4, 8) //[11:8]
    #define STBCAL_STBDLELAST_FILTER                           Fld(1, 12) //[12:12]
    #define STBCAL_STBUPDSTOP                                  Fld(1, 13) //[13:13]
    #define STBCAL_CG_RKEN                                     Fld(1, 14) //[14:14]
    #define STBCAL_STBSTATE_OPT                                Fld(1, 15) //[15:15]
    #define STBCAL_PHYVALID_IG                                 Fld(1, 16) //[16:16]
    #define STBCAL_SREF_DQSGUPD                                Fld(1, 17) //[17:17]
    #define STBCAL_STBCNTRST                                   Fld(1, 18) //[18:18]
    #define STBCAL_RKCHGMASKDIS                                Fld(1, 19) //[19:19]
    #define STBCAL_PICGEN                                      Fld(1, 20) //[20:20]
    #define STBCAL_REFUICHG                                    Fld(1, 21) //[21:21]
    #define STBCAL_STB_SELPHYCALEN                             Fld(1, 22) //[22:22]
    #define STBCAL_STBCAL2R                                    Fld(1, 23) //[23:23]
    #define STBCAL_STBCALEN                                    Fld(1, 24) //[24:24]
    #define STBCAL_STBDLYOUT_OPT                               Fld(1, 25) //[25:25]
    #define STBCAL_PICHGBLOCK_NORD                             Fld(1, 26) //[26:26]
    #define STBCAL_STB_DQIEN_IG                                Fld(1, 27) //[27:27]
    #define STBCAL_DQSIENCG_CHG_EN                             Fld(1, 28) //[28:28]
    #define STBCAL_DQSIENCG_NORMAL_EN                          Fld(1, 29) //[29:29]
    #define STBCAL_DQSIENMODE_SELPH                            Fld(1, 30) //[30:30]
    #define STBCAL_DQSIENMODE                                  Fld(1, 31) //[31:31]

#define DRAMC_REG_STBCAL1                                  (DRAMC_AO_BASE + 0x00204)
    #define STBCAL1_STBCNT_SHU_RST_EN                          Fld(1, 0) //[0:0]
    #define STBCAL1_RKUICHG_EN                                 Fld(1, 1) //[1:1]
    #define STBCAL1_DIS_PI_TRACK_AS_NOT_RD                     Fld(1, 2) //[2:2]
    #define STBCAL1_STBCNT_MODESEL                             Fld(1, 4) //[4:4]
    #define STBCAL1_DQSIEN_7UI_EN                              Fld(1, 5) //[5:5]
    #define STBCAL1_STB_SHIFT_DTCOUT_IG                        Fld(1, 6) //[6:6]
    #define STBCAL1_INPUTRXTRACK_BLOCK                         Fld(1, 7) //[7:7]
    #define STBCAL1_STB_FLAGCLR                                Fld(1, 8) //[8:8]
    #define STBCAL1_STB_DLLFRZ_IG                              Fld(1, 9) //[9:9]
    #define STBCAL1_STBENCMPEN                                 Fld(1, 10) //[10:10]
    #define STBCAL1_STBCNT_LATCH_EN                            Fld(1, 11) //[11:11]
    #define STBCAL1_DLLFRZ_MON_PBREF_OPT                       Fld(1, 12) //[12:12]
    #define STBCAL1_DLLFRZ_BLOCKLONG                           Fld(1, 13) //[13:13]
    #define STBCAL1_DQSERRCNT_DIS                              Fld(1, 14) //[14:14]
    #define STBCAL1_STBCNT_SW_RST                              Fld(1, 15) //[15:15]
    #define STBCAL1_STBCAL_FILTER                              Fld(16, 16) //[31:16]

#define DRAMC_REG_STBCAL2                                  (DRAMC_AO_BASE + 0x00208)
    #define STBCAL2_STB_PIDLYCG_IG                             Fld(1, 0) //[0:0]
    #define STBCAL2_STB_UIDLYCG_IG                             Fld(1, 1) //[1:1]
    #define STBCAL2_STB_DBG_EN                                 Fld(4, 4) //[7:4]
    #define STBCAL2_STB_DBG_EN_B1                              Fld(1, 5)
    #define STBCAL2_STB_DBG_EN_B0                              Fld(1, 4)
    #define STBCAL2_STB_DBG_CG_AO                              Fld(1, 8) //[8:8]
    #define STBCAL2_STB_DBG_UIPI_UPD_OPT                       Fld(1, 9) //[9:9]
    #define STBCAL2_DQSGCNT_BYP_REF                            Fld(1, 10) //[10:10]
    #define STBCAL2_STB_DRS_MASK_HW_SAVE                       Fld(1, 12) //[12:12]
    #define STBCAL2_STB_DRS_RK1_FLAG_SYNC_RK0_EN               Fld(1, 13) //[13:13]
    #define STBCAL2_STB_PICG_EARLY_1T_EN                       Fld(1, 16) //[16:16]
    #define STBCAL2_STB_STBENRST_EARLY_1T_EN                   Fld(1, 17) //[17:17]
    #define STBCAL2_STB_IG_XRANK_CG_RST                        Fld(1, 18) //[18:18]
    #define STBCAL2_STB_RST_BY_RANK                            Fld(1, 19) //[19:19]
    #define STBCAL2_DQSIEN_SELPH_BY_RANK_EN                    Fld(1, 20) //[20:20]
    #define STBCAL2_DQSIEN_SELPH_1TO4_EN                       Fld(1, 24) //[24:24]
    #define STBCAL2_STB_GERRSTOP                               Fld(1, 28) //[28:28]
    #define STBCAL2_STB_GERR_RST                               Fld(1, 29) //[29:29]
    #define STBCAL2_STB_GERR_B01                               Fld(1, 30) //[30:30]
    #define STBCAL2_STB_GERR_B23                               Fld(1, 31) //[31:31]

#define DRAMC_REG_EYESCAN                                  (DRAMC_AO_BASE + 0x0020C)
    #define EYESCAN_REG_SW_RST                                 Fld(1, 0) //[0:0]
    #define EYESCAN_RG_RX_EYE_SCAN_EN                          Fld(1, 1) //[1:1]
    #define EYESCAN_RG_RX_MIOCK_JIT_EN                         Fld(1, 2) //[2:2]
    #define EYESCAN_EYESCAN_RD_SEL_OPT                         Fld(1, 4) //[4:4]
    #define EYESCAN_EYESCAN_CHK_OPT                            Fld(1, 6) //[6:6]
    #define EYESCAN_EYESCAN_TOG_OPT                            Fld(1, 7) //[7:7]
    #define EYESCAN_EYESCAN_DQ_SYNC_EN                         Fld(1, 8) //[8:8]
    #define EYESCAN_EYESCAN_NEW_DQ_SYNC_EN                     Fld(1, 9) //[9:9]
    #define EYESCAN_EYESCAN_DQS_SYNC_EN                        Fld(1, 10) //[10:10]
    #define EYESCAN_EYESCAN_DQS_OPT                            Fld(1, 11) //[11:11]
    #define EYESCAN_DCBLNCEN                                   Fld(1, 12) //[12:12]
    #define EYESCAN_DCBLNCINS                                  Fld(1, 13) //[13:13]
    #define EYESCAN_RX_DQ_EYE_SEL                              Fld(4, 16) //[19:16]
    #define EYESCAN_RX_DQ_EYE_SEL_B1                           Fld(4, 20) //[23:20]
    #define EYESCAN_RX_DQ_EYE_SEL_B2                           Fld(4, 24) //[27:24]
    #define EYESCAN_RX_DQ_EYE_SEL_B3                           Fld(4, 28) //[31:28]

#define DRAMC_REG_DVFSDLL                                  (DRAMC_AO_BASE + 0x00210)
    #define DVFSDLL_DLL_LOCK_SHU_EN                            Fld(1, 0) //[0:0]
    #define DVFSDLL_R_DVFS_PICG_POSTPONE                       Fld(1, 1) //[1:1]
    #define DVFSDLL_R_DVFS_MCK_CG_EN_FT_IN                     Fld(1, 2) //[2:2]
    #define DVFSDLL_R_DVFS_MCK_CG_EN_FT_EN                     Fld(1, 3) //[3:3]
    #define DVFSDLL_R_BYPASS_1ST_DLL_SHU4                      Fld(1, 4) //[4:4]
    #define DVFSDLL_R_DDRPHY_SHUFFLE_DEBUG_ENABLE              Fld(1, 5) //[5:5]
    #define DVFSDLL_R_RETRY_SAV_MSK                            Fld(1, 6) //[6:6]
    #define DVFSDLL_RG_DLL_SHUFFLE                             Fld(1, 7) //[7:7]
    #define DVFSDLL_DLL_IDLE_SHU2                              Fld(7, 8) //[14:8]
    #define DVFSDLL_DLL_IDLE_SHU3                              Fld(7, 16) //[22:16]
    #define DVFSDLL_DLL_IDLE_SHU4                              Fld(7, 24) //[30:24]
    #define DVFSDLL_R_DVFS_SYNC_MODULE_RST_SEL                 Fld(1, 31) //[31:31]

#define DRAMC_REG_DVFSDLL2                                 (DRAMC_AO_BASE + 0x00214)
    #define DVFSDLL2_R_DVFS_PICG_MARGIN_NEW                    Fld(4, 0) //[3:0]
    #define DVFSDLL2_R_DVFS_PICG_MARGIN2_NEW                   Fld(4, 4) //[7:4]
    #define DVFSDLL2_R_DVFS_PICG_MARGIN3_NEW                   Fld(4, 8) //[11:8]
    #define DVFSDLL2_R_DVFS_MCK8X_MARGIN                       Fld(4, 12) //[15:12]
    #define DVFSDLL2_R_SHUFFLE_PI_RESET_ENABLE                 Fld(1, 16) //[16:16]
    #define DVFSDLL2_R_DVFS_PICG_MARGIN4_NEW                   Fld(4, 20) //[23:20]

#define DRAMC_REG_PRE_TDQSCK1                              (DRAMC_AO_BASE + 0x00218)
    #define PRE_TDQSCK1_FREQ_RATIO_TX_9                        Fld(5, 0) //[4:0]
    #define PRE_TDQSCK1_FREQ_RATIO_TX_10                       Fld(5, 5) //[9:5]
    #define PRE_TDQSCK1_FREQ_RATIO_TX_11                       Fld(5, 10) //[14:10]
    #define PRE_TDQSCK1_TX_TRACKING_OPT                        Fld(1, 15) //[15:15]
    #define PRE_TDQSCK1_SW_UP_TX_NOW_CASE                      Fld(1, 16) //[16:16]
    #define PRE_TDQSCK1_TXUIPI_CAL_CGAR                        Fld(1, 17) //[17:17]
    #define PRE_TDQSCK1_SHU_PRELOAD_TX_START                   Fld(1, 18) //[18:18]
    #define PRE_TDQSCK1_SHU_PRELOAD_TX_HW                      Fld(1, 19) //[19:19]
    #define PRE_TDQSCK1_APHY_CG_OPT1                           Fld(1, 20) //[20:20]
    #define PRE_TDQSCK1_TDQSCK_HW_SW_UP_SEL                    Fld(1, 22) //[22:22]
    #define PRE_TDQSCK1_TDQSCK_SW_UP_CASE                      Fld(1, 23) //[23:23]
    #define PRE_TDQSCK1_TDQSCK_SW_SAVE                         Fld(1, 24) //[24:24]
    #define PRE_TDQSCK1_TDQSCK_REG_DVFS                        Fld(1, 25) //[25:25]
    #define PRE_TDQSCK1_TDQSCK_PRECAL_HW                       Fld(1, 26) //[26:26]
    #define PRE_TDQSCK1_TDQSCK_PRECAL_START                    Fld(1, 27) //[27:27]
    #define PRE_TDQSCK1_R_DEBUG_RANK_SEL                       Fld(2, 28) //[29:28]
    #define PRE_TDQSCK1_R_DEBUG_BYTE_SEL                       Fld(2, 30) //[31:30]

#define DRAMC_REG_PRE_TDQSCK2                              (DRAMC_AO_BASE + 0x0021C)
    #define PRE_TDQSCK2_TDDQSCK_JUMP_RATIO3                    Fld(8, 0) //[7:0]
    #define PRE_TDQSCK2_TDDQSCK_JUMP_RATIO2                    Fld(8, 8) //[15:8]
    #define PRE_TDQSCK2_TDDQSCK_JUMP_RATIO1                    Fld(8, 16) //[23:16]
    #define PRE_TDQSCK2_TDDQSCK_JUMP_RATIO0                    Fld(8, 24) //[31:24]

#define DRAMC_REG_PRE_TDQSCK3                              (DRAMC_AO_BASE + 0x00220)
    #define PRE_TDQSCK3_TDDQSCK_JUMP_RATIO7                    Fld(8, 0) //[7:0]
    #define PRE_TDQSCK3_TDDQSCK_JUMP_RATIO6                    Fld(8, 8) //[15:8]
    #define PRE_TDQSCK3_TDDQSCK_JUMP_RATIO5                    Fld(8, 16) //[23:16]
    #define PRE_TDQSCK3_TDDQSCK_JUMP_RATIO4                    Fld(8, 24) //[31:24]

#define DRAMC_REG_PRE_TDQSCK4                              (DRAMC_AO_BASE + 0x00224)
    #define PRE_TDQSCK4_TDDQSCK_JUMP_RATIO11                   Fld(8, 0) //[7:0]
    #define PRE_TDQSCK4_TDDQSCK_JUMP_RATIO10                   Fld(8, 8) //[15:8]
    #define PRE_TDQSCK4_TDDQSCK_JUMP_RATIO9                    Fld(8, 16) //[23:16]
    #define PRE_TDQSCK4_TDDQSCK_JUMP_RATIO8                    Fld(8, 24) //[31:24]

#define DRAMC_REG_MIOCK_JIT_MTR                            (DRAMC_AO_BASE + 0x00228)
    #define MIOCK_JIT_MTR_RX_MIOCK_JIT_LIMIT                   Fld(32, 0) //[31:0]

#define DRAMC_REG_IMPCAL                                   (DRAMC_AO_BASE + 0x0022C)
    #define IMPCAL_DRV_ECO_OPT                                 Fld(1, 10) //[10:10]
    #define IMPCAL_IMPCAL_CHGDRV_ECO_OPT                       Fld(1, 11) //[11:11]
    #define IMPCAL_IMPCAL_SM_ECO_OPT                           Fld(1, 12) //[12:12]
    #define IMPCAL_IMPCAL_ECO_OPT                              Fld(1, 13) //[13:13]
    #define IMPCAL_DIS_SUS_CH1_DRV                             Fld(1, 14) //[14:14]
    #define IMPCAL_DIS_SUS_CH0_DRV                             Fld(1, 15) //[15:15]
    #define IMPCAL_DIS_SHU_DRV                                 Fld(1, 16) //[16:16]
    #define IMPCAL_IMPCAL_DRVUPDOPT                            Fld(1, 17) //[17:17]
    #define IMPCAL_IMPCAL_USING_SYNC                           Fld(1, 18) //[18:18]
    #define IMPCAL_IMPCAL_BYPASS_UP_CA_DRV                     Fld(1, 19) //[19:19]
    #define IMPCAL_IMPCAL_HWSAVE_EN                            Fld(1, 20) //[20:20]
    #define IMPCAL_IMPCAL_CALI_ENN                             Fld(1, 21) //[21:21]
    #define IMPCAL_IMPCAL_CALI_ENP                             Fld(1, 22) //[22:22]
    #define IMPCAL_IMPCAL_CALI_EN                              Fld(1, 23) //[23:23]
    #define IMPCAL_IMPCAL_IMPPDN                               Fld(1, 24) //[24:24]
    #define IMPCAL_IMPCAL_IMPPDP                               Fld(1, 25) //[25:25]
    #define IMPCAL_IMPCAL_NEW_OLD_SL                           Fld(1, 26) //[26:26]
    #define IMPCAL_IMPCAL_CMP_DIREC                            Fld(2, 27) //[28:27]
    #define IMPCAL_IMPCAL_SWVALUE_EN                           Fld(1, 29) //[29:29]
    #define IMPCAL_IMPCAL_EN                                   Fld(1, 30) //[30:30]
    #define IMPCAL_IMPCAL_HW                                   Fld(1, 31) //[31:31]

#define DRAMC_REG_IMPEDAMCE_CTRL1                          (DRAMC_AO_BASE + 0x00230)
    #define IMPEDAMCE_CTRL1_DQS1_OFF                           Fld(10, 0) //[9:0]
    #define IMPEDAMCE_CTRL1_DOS2_OFF                           Fld(10, 10) //[19:10]
    #define IMPEDAMCE_CTRL1_DQS1_OFF_SUB                       Fld(2, 28) //[29:28]
    #define IMPEDAMCE_CTRL1_DQS2_OFF_SUB                       Fld(2, 30) //[31:30]

#define DRAMC_REG_IMPEDAMCE_CTRL2                          (DRAMC_AO_BASE + 0x00234)
    #define IMPEDAMCE_CTRL2_DQ1_OFF                            Fld(10, 0) //[9:0]
    #define IMPEDAMCE_CTRL2_DQ2_OFF                            Fld(10, 10) //[19:10]
    #define IMPEDAMCE_CTRL2_DQ1_OFF_SUB                        Fld(2, 28) //[29:28]
    #define IMPEDAMCE_CTRL2_DQ2_OFF_SUB                        Fld(2, 30) //[31:30]

#define DRAMC_REG_IMPEDAMCE_CTRL3                          (DRAMC_AO_BASE + 0x00238)
    #define IMPEDAMCE_CTRL3_CMD1_OFF                           Fld(10, 0) //[9:0]
    #define IMPEDAMCE_CTRL3_CMD2_OFF                           Fld(10, 10) //[19:10]
    #define IMPEDAMCE_CTRL3_CMD1_OFF_SUB                       Fld(2, 28) //[29:28]
    #define IMPEDAMCE_CTRL3_CMD2_OFF_SUB                       Fld(2, 30) //[31:30]

#define DRAMC_REG_IMPEDAMCE_CTRL4                          (DRAMC_AO_BASE + 0x0023C)
    #define IMPEDAMCE_CTRL4_DQC1_OFF                           Fld(10, 0) //[9:0]
    #define IMPEDAMCE_CTRL4_DQC2_OFF                           Fld(10, 10) //[19:10]
    #define IMPEDAMCE_CTRL4_DQC1_OFF_SUB                       Fld(2, 28) //[29:28]
    #define IMPEDAMCE_CTRL4_DQC2_OFF_SUB                       Fld(2, 30) //[31:30]

#define DRAMC_REG_DRAMC_DBG_SEL1                           (DRAMC_AO_BASE + 0x00240)
    #define DRAMC_DBG_SEL1_DEBUG_SEL_0                         Fld(16, 0) //[15:0]
    #define DRAMC_DBG_SEL1_DEBUG_SEL_1                         Fld(16, 16) //[31:16]

#define DRAMC_REG_DRAMC_DBG_SEL2                           (DRAMC_AO_BASE + 0x00244)
    #define DRAMC_DBG_SEL2_DEBUG_SEL_2                         Fld(16, 0) //[15:0]
    #define DRAMC_DBG_SEL2_DEBUG_SEL_3                         Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_DQSOSC                               (DRAMC_AO_BASE + 0x00300)
    #define RK0_DQSOSC_R_DMDQS2DQ_FILT_OPT                     Fld(1, 29) //[29:29]
    #define RK0_DQSOSC_DQSOSCR_RK0EN                           Fld(1, 30) //[30:30]
    #define RK0_DQSOSC_DQSOSC_RK0INTCLR                        Fld(1, 31) //[31:31]

#define DRAMC_REG_RK0_DUMMY_RD_WDATA0                      (DRAMC_AO_BASE + 0x00318)
    #define RK0_DUMMY_RD_WDATA0_DMY_RD_RK0_WDATA0              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_DUMMY_RD_WDATA1                      (DRAMC_AO_BASE + 0x0031C)
    #define RK0_DUMMY_RD_WDATA1_DMY_RD_RK0_WDATA1              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_DUMMY_RD_WDATA2                      (DRAMC_AO_BASE + 0x00320)
    #define RK0_DUMMY_RD_WDATA2_DMY_RD_RK0_WDATA2              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_DUMMY_RD_WDATA3                      (DRAMC_AO_BASE + 0x00324)
    #define RK0_DUMMY_RD_WDATA3_DMY_RD_RK0_WDATA3              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_DUMMY_RD_ADR                         (DRAMC_AO_BASE + 0x00328)
    #define RK0_DUMMY_RD_ADR_DMY_RD_RK0_ROW_ADR                Fld(17, 0) //[16:0]
    #define RK0_DUMMY_RD_ADR_DMY_RD_RK0_COL_ADR                Fld(11, 17) //[27:17]
    #define RK0_DUMMY_RD_ADR_DMY_RD_RK0_LEN                    Fld(4, 28) //[31:28]

#define DRAMC_REG_RK0_DUMMY_RD_BK                          (DRAMC_AO_BASE + 0x0032C)
    #define RK0_DUMMY_RD_BK_DMY_RD_RK0_BK                      Fld(3, 0) //[2:0]
    #define RK0_DUMMY_RD_BK_DUMMY_RD_1_CNT0                    Fld(1, 4) //[4:4]
    #define RK0_DUMMY_RD_BK_DUMMY_RD_1_CNT1                    Fld(1, 5) //[5:5]
    #define RK0_DUMMY_RD_BK_DUMMY_RD_1_CNT2                    Fld(1, 6) //[6:6]
    #define RK0_DUMMY_RD_BK_DUMMY_RD_1_CNT3                    Fld(1, 7) //[7:7]
    #define RK0_DUMMY_RD_BK_DUMMY_RD_1_CNT4                    Fld(1, 8) //[8:8]
    #define RK0_DUMMY_RD_BK_DUMMY_RD_1_CNT5                    Fld(1, 9) //[9:9]
    #define RK0_DUMMY_RD_BK_DUMMY_RD_1_CNT6                    Fld(1, 10) //[10:10]
    #define RK0_DUMMY_RD_BK_DUMMY_RD_1_CNT7                    Fld(1, 11) //[11:11]

#define DRAMC_REG_RK0_PRE_TDQSCK1                          (DRAMC_AO_BASE + 0x00330)
    #define RK0_PRE_TDQSCK1_TDQSCK_UIFREQ1_B0R0                Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK1_TDQSCK_PIFREQ1_B0R0                Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R0                Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R0                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK2                          (DRAMC_AO_BASE + 0x00334)
    #define RK0_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R0                Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R0                Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R0                Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R0                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK3                          (DRAMC_AO_BASE + 0x00338)
    #define RK0_PRE_TDQSCK3_TDQSCK_UIFREQ1_P1_B0R0             Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R0             Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R0             Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R0             Fld(6, 18) //[23:18]

#define DRAMC_REG_RK0_PRE_TDQSCK4                          (DRAMC_AO_BASE + 0x0033C)
    #define RK0_PRE_TDQSCK4_TDQSCK_UIFREQ1_B1R0                Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK4_TDQSCK_PIFREQ1_B1R0                Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R0                Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK4_TDQSCK_PIFREQ2_B1R0                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK5                          (DRAMC_AO_BASE + 0x00340)
    #define RK0_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R0                Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R0                Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R0                Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R0                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK6                          (DRAMC_AO_BASE + 0x00344)
    #define RK0_PRE_TDQSCK6_TDQSCK_UIFREQ1_P1_B1R0             Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R0             Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R0             Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R0             Fld(6, 18) //[23:18]

#define DRAMC_REG_RK0_PRE_TDQSCK7                          (DRAMC_AO_BASE + 0x00348)
    #define RK0_PRE_TDQSCK7_TDQSCK_UIFREQ1_B2R0                Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK7_TDQSCK_PIFREQ1_B2R0                Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK7_TDQSCK_UIFREQ2_B2R0                Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK7_TDQSCK_PIFREQ2_B2R0                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK8                          (DRAMC_AO_BASE + 0x0034C)
    #define RK0_PRE_TDQSCK8_TDQSCK_UIFREQ3_B2R0                Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK8_TDQSCK_PIFREQ3_B2R0                Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK8_TDQSCK_UIFREQ4_B2R0                Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK8_TDQSCK_PIFREQ4_B2R0                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK9                          (DRAMC_AO_BASE + 0x00350)
    #define RK0_PRE_TDQSCK9_TDQSCK_UIFREQ1_P1_B2R0             Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK9_TDQSCK_UIFREQ2_P1_B2R0             Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK9_TDQSCK_UIFREQ3_P1_B2R0             Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK9_TDQSCK_UIFREQ4_P1_B2R0             Fld(6, 18) //[23:18]

#define DRAMC_REG_RK0_PRE_TDQSCK10                         (DRAMC_AO_BASE + 0x00354)
    #define RK0_PRE_TDQSCK10_TDQSCK_UIFREQ1_B3R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK10_TDQSCK_PIFREQ1_B3R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK10_TDQSCK_UIFREQ2_B3R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK10_TDQSCK_PIFREQ2_B3R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK11                         (DRAMC_AO_BASE + 0x00358)
    #define RK0_PRE_TDQSCK11_TDQSCK_UIFREQ3_B3R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK11_TDQSCK_PIFREQ3_B3R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK11_TDQSCK_UIFREQ4_B3R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK11_TDQSCK_PIFREQ4_B3R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK12                         (DRAMC_AO_BASE + 0x0035C)
    #define RK0_PRE_TDQSCK12_TDQSCK_UIFREQ1_P1_B3R0            Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK12_TDQSCK_UIFREQ2_P1_B3R0            Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK12_TDQSCK_UIFREQ3_P1_B3R0            Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK12_TDQSCK_UIFREQ4_P1_B3R0            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK0_PRE_TDQSCK13                         (DRAMC_AO_BASE + 0x00360)
    #define RK0_PRE_TDQSCK13_TDQSCK_UIFREQ5_B0R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK13_TDQSCK_PIFREQ5_B0R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK13_TDQSCK_UIFREQ6_B0R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK13_TDQSCK_PIFREQ6_B0R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK14                         (DRAMC_AO_BASE + 0x00364)
    #define RK0_PRE_TDQSCK14_TDQSCK_UIFREQ7_B0R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK14_TDQSCK_PIFREQ7_B0R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK14_TDQSCK_UIFREQ8_B0R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK14_TDQSCK_PIFREQ8_B0R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK15                         (DRAMC_AO_BASE + 0x00368)
    #define RK0_PRE_TDQSCK15_TDQSCK_UIFREQ5_P1_B0R0            Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK15_TDQSCK_UIFREQ6_P1_B0R0            Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK15_TDQSCK_UIFREQ7_P1_B0R0            Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK15_TDQSCK_UIFREQ8_P1_B0R0            Fld(6, 18) //[23:18]
    #define RK0_PRE_TDQSCK15_SHUFFLE_LEVEL_MODE_SELECT         Fld(1, 24) //[24:24]

#define DRAMC_REG_RK0_PRE_TDQSCK16                         (DRAMC_AO_BASE + 0x0036C)
    #define RK0_PRE_TDQSCK16_TDQSCK_UIFREQ5_B1R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK16_TDQSCK_PIFREQ5_B1R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK16_TDQSCK_UIFREQ6_B1R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK16_TDQSCK_PIFREQ6_B1R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK17                         (DRAMC_AO_BASE + 0x00370)
    #define RK0_PRE_TDQSCK17_TDQSCK_UIFREQ7_B1R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK17_TDQSCK_PIFREQ7_B1R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK17_TDQSCK_UIFREQ8_B1R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK17_TDQSCK_PIFREQ8_B1R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK18                         (DRAMC_AO_BASE + 0x00374)
    #define RK0_PRE_TDQSCK18_TDQSCK_UIFREQ5_P1_B1R0            Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK18_TDQSCK_UIFREQ6_P1_B1R0            Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK18_TDQSCK_UIFREQ7_P1_B1R0            Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK18_TDQSCK_UIFREQ8_P1_B1R0            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK0_PRE_TDQSCK19                         (DRAMC_AO_BASE + 0x00378)
    #define RK0_PRE_TDQSCK19_TDQSCK_UIFREQ5_B2R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK19_TDQSCK_PIFREQ5_B2R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK19_TDQSCK_UIFREQ6_B2R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK19_TDQSCK_PIFREQ6_B2R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK20                         (DRAMC_AO_BASE + 0x0037C)
    #define RK0_PRE_TDQSCK20_TDQSCK_UIFREQ7_B2R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK20_TDQSCK_PIFREQ7_B2R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK20_TDQSCK_UIFREQ8_B2R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK20_TDQSCK_PIFREQ8_B2R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK21                         (DRAMC_AO_BASE + 0x00380)
    #define RK0_PRE_TDQSCK21_TDQSCK_UIFREQ5_P1_B2R0            Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK21_TDQSCK_UIFREQ6_P1_B2R0            Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK21_TDQSCK_UIFREQ7_P1_B2R0            Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK21_TDQSCK_UIFREQ8_P1_B2R0            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK0_PRE_TDQSCK22                         (DRAMC_AO_BASE + 0x00384)
    #define RK0_PRE_TDQSCK22_TDQSCK_UIFREQ5_B3R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK22_TDQSCK_PIFREQ5_B3R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK22_TDQSCK_UIFREQ6_B3R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK22_TDQSCK_PIFREQ6_B3R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK23                         (DRAMC_AO_BASE + 0x00388)
    #define RK0_PRE_TDQSCK23_TDQSCK_UIFREQ7_B3R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK23_TDQSCK_PIFREQ7_B3R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK23_TDQSCK_UIFREQ8_B3R0               Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK23_TDQSCK_PIFREQ8_B3R0               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK24                         (DRAMC_AO_BASE + 0x0038C)
    #define RK0_PRE_TDQSCK24_TDQSCK_UIFREQ5_P1_B3R0            Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK24_TDQSCK_UIFREQ6_P1_B3R0            Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK24_TDQSCK_UIFREQ7_P1_B3R0            Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK24_TDQSCK_UIFREQ8_P1_B3R0            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_DQSOSC                               (DRAMC_AO_BASE + 0x00400)
    #define RK1_DQSOSC_DQSOSCR_RK1EN                           Fld(1, 30) //[30:30]
    #define RK1_DQSOSC_DQSOSC_RK1INTCLR                        Fld(1, 31) //[31:31]

#define DRAMC_REG_RK1_DUMMY_RD_WDATA0                      (DRAMC_AO_BASE + 0x00418)
    #define RK1_DUMMY_RD_WDATA0_DMY_RD_RK1_WDATA0              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_DUMMY_RD_WDATA1                      (DRAMC_AO_BASE + 0x0041C)
    #define RK1_DUMMY_RD_WDATA1_DMY_RD_RK1_WDATA1              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_DUMMY_RD_WDATA2                      (DRAMC_AO_BASE + 0x00420)
    #define RK1_DUMMY_RD_WDATA2_DMY_RD_RK1_WDATA2              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_DUMMY_RD_WDATA3                      (DRAMC_AO_BASE + 0x00424)
    #define RK1_DUMMY_RD_WDATA3_DMY_RD_RK1_WDATA3              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_DUMMY_RD_ADR                         (DRAMC_AO_BASE + 0x00428)
    #define RK1_DUMMY_RD_ADR_DMY_RD_RK1_ROW_ADR                Fld(17, 0) //[16:0]
    #define RK1_DUMMY_RD_ADR_DMY_RD_RK1_COL_ADR                Fld(11, 17) //[27:17]
    #define RK1_DUMMY_RD_ADR_DMY_RD_RK1_LEN                    Fld(4, 28) //[31:28]

#define DRAMC_REG_RK1_DUMMY_RD_BK                          (DRAMC_AO_BASE + 0x0042C)
    #define RK1_DUMMY_RD_BK_DMY_RD_RK1_BK                      Fld(3, 0) //[2:0]

#define DRAMC_REG_RK1_PRE_TDQSCK1                          (DRAMC_AO_BASE + 0x00430)
    #define RK1_PRE_TDQSCK1_TDQSCK_UIFREQ1_B0R1                Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK1_TDQSCK_PIFREQ1_B0R1                Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R1                Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R1                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK2                          (DRAMC_AO_BASE + 0x00434)
    #define RK1_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R1                Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R1                Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R1                Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R1                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK3                          (DRAMC_AO_BASE + 0x00438)
    #define RK1_PRE_TDQSCK3_TDQSCK_UIFREQ1_P1_B0R1             Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R1             Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R1             Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R1             Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_PRE_TDQSCK4                          (DRAMC_AO_BASE + 0x0043C)
    #define RK1_PRE_TDQSCK4_TDQSCK_UIFREQ1_B1R1                Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK4_TDQSCK_PIFREQ1_B1R1                Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R1                Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK4_TDQSCK_PIFREQ2_B1R1                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK5                          (DRAMC_AO_BASE + 0x00440)
    #define RK1_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R1                Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R1                Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R1                Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R1                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK6                          (DRAMC_AO_BASE + 0x00444)
    #define RK1_PRE_TDQSCK6_TDQSCK_UIFREQ1_P1_B1R1             Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R1             Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R1             Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R1             Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_PRE_TDQSCK7                          (DRAMC_AO_BASE + 0x00448)
    #define RK1_PRE_TDQSCK7_TDQSCK_UIFREQ1_B2R1                Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK7_TDQSCK_PIFREQ1_B2R1                Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK7_TDQSCK_UIFREQ2_B2R1                Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK7_TDQSCK_PIFREQ2_B2R1                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK8                          (DRAMC_AO_BASE + 0x0044C)
    #define RK1_PRE_TDQSCK8_TDQSCK_UIFREQ3_B2R1                Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK8_TDQSCK_PIFREQ3_B2R1                Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK8_TDQSCK_UIFREQ4_B2R1                Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK8_TDQSCK_PIFREQ4_B2R1                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK9                          (DRAMC_AO_BASE + 0x00450)
    #define RK1_PRE_TDQSCK9_TDQSCK_UIFREQ1_P1_B2R1             Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK9_TDQSCK_UIFREQ2_P1_B2R1             Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK9_TDQSCK_UIFREQ3_P1_B2R1             Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK9_TDQSCK_UIFREQ4_P1_B2R1             Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_PRE_TDQSCK10                         (DRAMC_AO_BASE + 0x00454)
    #define RK1_PRE_TDQSCK10_TDQSCK_UIFREQ1_B3R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK10_TDQSCK_PIFREQ1_B3R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK10_TDQSCK_UIFREQ2_B3R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK10_TDQSCK_PIFREQ2_B3R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK11                         (DRAMC_AO_BASE + 0x00458)
    #define RK1_PRE_TDQSCK11_TDQSCK_UIFREQ3_B3R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK11_TDQSCK_PIFREQ3_B3R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK11_TDQSCK_UIFREQ4_B3R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK11_TDQSCK_PIFREQ4_B3R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK12                         (DRAMC_AO_BASE + 0x0045C)
    #define RK1_PRE_TDQSCK12_TDQSCK_UIFREQ1_P1_B3R1            Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK12_TDQSCK_UIFREQ2_P1_B3R1            Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK12_TDQSCK_UIFREQ3_P1_B3R1            Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK12_TDQSCK_UIFREQ4_P1_B3R1            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_PRE_TDQSCK13                         (DRAMC_AO_BASE + 0x00460)
    #define RK1_PRE_TDQSCK13_TDQSCK_UIFREQ5_B0R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK13_TDQSCK_PIFREQ5_B0R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK13_TDQSCK_UIFREQ6_B0R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK13_TDQSCK_PIFREQ6_B0R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK14                         (DRAMC_AO_BASE + 0x00464)
    #define RK1_PRE_TDQSCK14_TDQSCK_UIFREQ7_B0R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK14_TDQSCK_PIFREQ7_B0R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK14_TDQSCK_UIFREQ8_B0R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK14_TDQSCK_PIFREQ8_B0R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK15                         (DRAMC_AO_BASE + 0x00468)
    #define RK1_PRE_TDQSCK15_TDQSCK_UIFREQ5_P1_B0R1            Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK15_TDQSCK_UIFREQ6_P1_B0R1            Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK15_TDQSCK_UIFREQ7_P1_B0R1            Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK15_TDQSCK_UIFREQ8_P1_B0R1            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_PRE_TDQSCK16                         (DRAMC_AO_BASE + 0x0046C)
    #define RK1_PRE_TDQSCK16_TDQSCK_UIFREQ5_B1R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK16_TDQSCK_PIFREQ5_B1R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK16_TDQSCK_UIFREQ6_B1R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK16_TDQSCK_PIFREQ6_B1R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK17                         (DRAMC_AO_BASE + 0x00470)
    #define RK1_PRE_TDQSCK17_TDQSCK_UIFREQ7_B1R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK17_TDQSCK_PIFREQ7_B1R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK17_TDQSCK_UIFREQ8_B1R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK17_TDQSCK_PIFREQ8_B1R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK18                         (DRAMC_AO_BASE + 0x00474)
    #define RK1_PRE_TDQSCK18_TDQSCK_UIFREQ5_P1_B1R1            Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK18_TDQSCK_UIFREQ6_P1_B1R1            Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK18_TDQSCK_UIFREQ7_P1_B1R1            Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK18_TDQSCK_UIFREQ8_P1_B1R1            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_PRE_TDQSCK19                         (DRAMC_AO_BASE + 0x00478)
    #define RK1_PRE_TDQSCK19_TDQSCK_UIFREQ5_B2R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK19_TDQSCK_PIFREQ5_B2R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK19_TDQSCK_UIFREQ6_B2R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK19_TDQSCK_PIFREQ6_B2R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK20                         (DRAMC_AO_BASE + 0x0047C)
    #define RK1_PRE_TDQSCK20_TDQSCK_UIFREQ7_B2R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK20_TDQSCK_PIFREQ7_B2R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK20_TDQSCK_UIFREQ8_B2R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK20_TDQSCK_PIFREQ8_B2R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK21                         (DRAMC_AO_BASE + 0x00480)
    #define RK1_PRE_TDQSCK21_TDQSCK_UIFREQ5_P1_B2R1            Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK21_TDQSCK_UIFREQ6_P1_B2R1            Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK21_TDQSCK_UIFREQ7_P1_B2R1            Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK21_TDQSCK_UIFREQ8_P1_B2R1            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_PRE_TDQSCK22                         (DRAMC_AO_BASE + 0x00484)
    #define RK1_PRE_TDQSCK22_TDQSCK_UIFREQ5_B3R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK22_TDQSCK_PIFREQ5_B3R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK22_TDQSCK_UIFREQ6_B3R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK22_TDQSCK_PIFREQ6_B3R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK23                         (DRAMC_AO_BASE + 0x00488)
    #define RK1_PRE_TDQSCK23_TDQSCK_UIFREQ7_B3R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK23_TDQSCK_PIFREQ7_B3R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK23_TDQSCK_UIFREQ8_B3R1               Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK23_TDQSCK_PIFREQ8_B3R1               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK24                         (DRAMC_AO_BASE + 0x0048C)
    #define RK1_PRE_TDQSCK24_TDQSCK_UIFREQ5_P1_B3R1            Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK24_TDQSCK_UIFREQ6_P1_B3R1            Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK24_TDQSCK_UIFREQ7_P1_B3R1            Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK24_TDQSCK_UIFREQ8_P1_B3R1            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_DQSOSC                               (DRAMC_AO_BASE + 0x00500)
    #define RK2_DQSOSC_FREQ_RATIO_TX_0                         Fld(5, 0) //[4:0]
    #define RK2_DQSOSC_FREQ_RATIO_TX_1                         Fld(5, 5) //[9:5]
    #define RK2_DQSOSC_FREQ_RATIO_TX_2                         Fld(5, 10) //[14:10]
    #define RK2_DQSOSC_FREQ_RATIO_TX_3                         Fld(5, 15) //[19:15]
    #define RK2_DQSOSC_FREQ_RATIO_TX_4                         Fld(5, 20) //[24:20]
    #define RK2_DQSOSC_FREQ_RATIO_TX_5                         Fld(5, 25) //[29:25]
    #define RK2_DQSOSC_DQSOSCR_RK2EN                           Fld(1, 30) //[30:30]
    #define RK2_DQSOSC_DQSOSC_RK2INTCLR                        Fld(1, 31) //[31:31]

#define DRAMC_REG_RK2_DUMMY_RD_WDATA0                      (DRAMC_AO_BASE + 0x00518)
    #define RK2_DUMMY_RD_WDATA0_DMY_RD_RK2_WDATA0              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_DUMMY_RD_WDATA1                      (DRAMC_AO_BASE + 0x0051C)
    #define RK2_DUMMY_RD_WDATA1_DMY_RD_RK2_WDATA1              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_DUMMY_RD_WDATA2                      (DRAMC_AO_BASE + 0x00520)
    #define RK2_DUMMY_RD_WDATA2_DMY_RD_RK2_WDATA2              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_DUMMY_RD_WDATA3                      (DRAMC_AO_BASE + 0x00524)
    #define RK2_DUMMY_RD_WDATA3_DMY_RD_RK2_WDATA3              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_DUMMY_RD_ADR                         (DRAMC_AO_BASE + 0x00528)
    #define RK2_DUMMY_RD_ADR_DMY_RD_RK2_ROW_ADR                Fld(17, 0) //[16:0]
    #define RK2_DUMMY_RD_ADR_DMY_RD_RK2_COL_ADR                Fld(11, 17) //[27:17]
    #define RK2_DUMMY_RD_ADR_DMY_RD_RK2_LEN                    Fld(4, 28) //[31:28]

#define DRAMC_REG_RK2_DUMMY_RD_BK                          (DRAMC_AO_BASE + 0x0052C)
    #define RK2_DUMMY_RD_BK_DMY_RD_RK2_BK                      Fld(3, 0) //[2:0]
    #define RK2_DUMMY_RD_BK_FREQ_RATIO_TX_6                    Fld(5, 3) //[7:3]
    #define RK2_DUMMY_RD_BK_FREQ_RATIO_TX_7                    Fld(5, 8) //[12:8]
    #define RK2_DUMMY_RD_BK_FREQ_RATIO_TX_8                    Fld(5, 13) //[17:13]

#define DRAMC_REG_RK2_PRE_TDQSCK1                          (DRAMC_AO_BASE + 0x00530)
    #define RK2_PRE_TDQSCK1_TDQSCK_UIFREQ1_B0R2                Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK1_TDQSCK_PIFREQ1_B0R2                Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R2                Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R2                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK2                          (DRAMC_AO_BASE + 0x00534)
    #define RK2_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R2                Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R2                Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R2                Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R2                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK3                          (DRAMC_AO_BASE + 0x00538)
    #define RK2_PRE_TDQSCK3_TDQSCK_UIFREQ1_P1_B0R2             Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R2             Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R2             Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R2             Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_PRE_TDQSCK4                          (DRAMC_AO_BASE + 0x0053C)
    #define RK2_PRE_TDQSCK4_TDQSCK_UIFREQ1_B1R2                Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK4_TDQSCK_PIFREQ1_B1R2                Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R2                Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK4_TDQSCK_PIFREQ2_B1R2                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK5                          (DRAMC_AO_BASE + 0x00540)
    #define RK2_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R2                Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R2                Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R2                Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R2                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK6                          (DRAMC_AO_BASE + 0x00544)
    #define RK2_PRE_TDQSCK6_TDQSCK_UIFREQ1_P1_B1R2             Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R2             Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R2             Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R2             Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_PRE_TDQSCK7                          (DRAMC_AO_BASE + 0x00548)
    #define RK2_PRE_TDQSCK7_TDQSCK_UIFREQ1_B2R2                Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK7_TDQSCK_PIFREQ1_B2R2                Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK7_TDQSCK_UIFREQ2_B2R2                Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK7_TDQSCK_PIFREQ2_B2R2                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK8                          (DRAMC_AO_BASE + 0x0054C)
    #define RK2_PRE_TDQSCK8_TDQSCK_UIFREQ3_B2R2                Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK8_TDQSCK_PIFREQ3_B2R2                Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK8_TDQSCK_UIFREQ4_B2R2                Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK8_TDQSCK_PIFREQ4_B2R2                Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK9                          (DRAMC_AO_BASE + 0x00550)
    #define RK2_PRE_TDQSCK9_TDQSCK_UIFREQ1_P1_B2R2             Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK9_TDQSCK_UIFREQ2_P1_B2R2             Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK9_TDQSCK_UIFREQ3_P1_B2R2             Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK9_TDQSCK_UIFREQ4_P1_B2R2             Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_PRE_TDQSCK10                         (DRAMC_AO_BASE + 0x00554)
    #define RK2_PRE_TDQSCK10_TDQSCK_UIFREQ1_B3R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK10_TDQSCK_PIFREQ1_B3R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK10_TDQSCK_UIFREQ2_B3R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK10_TDQSCK_PIFREQ2_B3R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK11                         (DRAMC_AO_BASE + 0x00558)
    #define RK2_PRE_TDQSCK11_TDQSCK_UIFREQ3_B3R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK11_TDQSCK_PIFREQ3_B3R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK11_TDQSCK_UIFREQ4_B3R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK11_TDQSCK_PIFREQ4_B3R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK12                         (DRAMC_AO_BASE + 0x0055C)
    #define RK2_PRE_TDQSCK12_TDQSCK_UIFREQ1_P1_B3R2            Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK12_TDQSCK_UIFREQ2_P1_B3R2            Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK12_TDQSCK_UIFREQ3_P1_B3R2            Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK12_TDQSCK_UIFREQ4_P1_B3R2            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_PRE_TDQSCK13                         (DRAMC_AO_BASE + 0x00560)
    #define RK2_PRE_TDQSCK13_TDQSCK_UIFREQ5_B0R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK13_TDQSCK_PIFREQ5_B0R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK13_TDQSCK_UIFREQ6_B0R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK13_TDQSCK_PIFREQ6_B0R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK14                         (DRAMC_AO_BASE + 0x00564)
    #define RK2_PRE_TDQSCK14_TDQSCK_UIFREQ7_B0R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK14_TDQSCK_PIFREQ7_B0R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK14_TDQSCK_UIFREQ8_B0R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK14_TDQSCK_PIFREQ8_B0R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK15                         (DRAMC_AO_BASE + 0x00568)
    #define RK2_PRE_TDQSCK15_TDQSCK_UIFREQ5_P1_B0R2            Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK15_TDQSCK_UIFREQ6_P1_B0R2            Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK15_TDQSCK_UIFREQ7_P1_B0R2            Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK15_TDQSCK_UIFREQ8_P1_B0R2            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_PRE_TDQSCK16                         (DRAMC_AO_BASE + 0x0056C)
    #define RK2_PRE_TDQSCK16_TDQSCK_UIFREQ5_B1R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK16_TDQSCK_PIFREQ5_B1R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK16_TDQSCK_UIFREQ6_B1R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK16_TDQSCK_PIFREQ6_B1R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK17                         (DRAMC_AO_BASE + 0x00570)
    #define RK2_PRE_TDQSCK17_TDQSCK_UIFREQ7_B1R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK17_TDQSCK_PIFREQ7_B1R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK17_TDQSCK_UIFREQ8_B1R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK17_TDQSCK_PIFREQ8_B1R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK18                         (DRAMC_AO_BASE + 0x00574)
    #define RK2_PRE_TDQSCK18_TDQSCK_UIFREQ5_P1_B1R2            Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK18_TDQSCK_UIFREQ6_P1_B1R2            Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK18_TDQSCK_UIFREQ7_P1_B1R2            Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK18_TDQSCK_UIFREQ8_P1_B1R2            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_PRE_TDQSCK19                         (DRAMC_AO_BASE + 0x00578)
    #define RK2_PRE_TDQSCK19_TDQSCK_UIFREQ5_B2R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK19_TDQSCK_PIFREQ5_B2R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK19_TDQSCK_UIFREQ6_B2R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK19_TDQSCK_PIFREQ6_B2R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK20                         (DRAMC_AO_BASE + 0x0057C)
    #define RK2_PRE_TDQSCK20_TDQSCK_UIFREQ7_B2R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK20_TDQSCK_PIFREQ7_B2R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK20_TDQSCK_UIFREQ8_B2R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK20_TDQSCK_PIFREQ8_B2R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK21                         (DRAMC_AO_BASE + 0x00580)
    #define RK2_PRE_TDQSCK21_TDQSCK_UIFREQ5_P1_B2R2            Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK21_TDQSCK_UIFREQ6_P1_B2R2            Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK21_TDQSCK_UIFREQ7_P1_B2R2            Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK21_TDQSCK_UIFREQ8_P1_B2R2            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_PRE_TDQSCK22                         (DRAMC_AO_BASE + 0x00584)
    #define RK2_PRE_TDQSCK22_TDQSCK_UIFREQ5_B3R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK22_TDQSCK_PIFREQ5_B3R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK22_TDQSCK_UIFREQ6_B3R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK22_TDQSCK_PIFREQ6_B3R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK23                         (DRAMC_AO_BASE + 0x00588)
    #define RK2_PRE_TDQSCK23_TDQSCK_UIFREQ7_B3R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK23_TDQSCK_PIFREQ7_B3R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK23_TDQSCK_UIFREQ8_B3R2               Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK23_TDQSCK_PIFREQ8_B3R2               Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK24                         (DRAMC_AO_BASE + 0x0058C)
    #define RK2_PRE_TDQSCK24_TDQSCK_UIFREQ5_P1_B3R2            Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK24_TDQSCK_UIFREQ6_P1_B3R2            Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK24_TDQSCK_UIFREQ7_P1_B3R2            Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK24_TDQSCK_UIFREQ8_P1_B3R2            Fld(6, 18) //[23:18]

#define DRAMC_REG_RK0_PRE_TDQSCK25                         (DRAMC_AO_BASE + 0x00590)
    #define RK0_PRE_TDQSCK25_TDQSCK_UIFREQ9_B0R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK25_TDQSCK_PIFREQ9_B0R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK25_TDQSCK_UIFREQ10_B0R0              Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK25_TDQSCK_PIFREQ10_B0R0              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK26                         (DRAMC_AO_BASE + 0x00594)
    #define RK0_PRE_TDQSCK26_TDQSCK_UIFREQ9_B1R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK26_TDQSCK_PIFREQ9_B1R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK26_TDQSCK_UIFREQ10_B1R0              Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK26_TDQSCK_PIFREQ10_B1R0              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK27                         (DRAMC_AO_BASE + 0x00598)
    #define RK0_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B1R0            Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B1R0           Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B0R0            Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B0R0           Fld(6, 18) //[23:18]

#define DRAMC_REG_RK0_PRE_TDQSCK28                         (DRAMC_AO_BASE + 0x0059C)
    #define RK0_PRE_TDQSCK28_TDQSCK_UIFREQ9_B2R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK28_TDQSCK_PIFREQ9_B2R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK28_TDQSCK_UIFREQ10_B2R0              Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK28_TDQSCK_PIFREQ10_B2R0              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK29                         (DRAMC_AO_BASE + 0x005A0)
    #define RK0_PRE_TDQSCK29_TDQSCK_UIFREQ9_B3R0               Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK29_TDQSCK_PIFREQ9_B3R0               Fld(7, 6) //[12:6]
    #define RK0_PRE_TDQSCK29_TDQSCK_UIFREQ10_B3R0              Fld(6, 13) //[18:13]
    #define RK0_PRE_TDQSCK29_TDQSCK_PIFREQ10_B3R0              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK0_PRE_TDQSCK30                         (DRAMC_AO_BASE + 0x005A4)
    #define RK0_PRE_TDQSCK30_TDQSCK_UIFREQ9_P1_B3R0            Fld(6, 0) //[5:0]
    #define RK0_PRE_TDQSCK30_TDQSCK_UIFREQ10_P1_B3R0           Fld(6, 6) //[11:6]
    #define RK0_PRE_TDQSCK30_TDQSCK_UIFREQ9_P1_B2R0            Fld(6, 12) //[17:12]
    #define RK0_PRE_TDQSCK30_TDQSCK_UIFREQ10_P1_B2R0           Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_PRE_TDQSCK25                         (DRAMC_AO_BASE + 0x005A8)
    #define RK1_PRE_TDQSCK25_TDQSCK_UIFREQ9_B0R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK25_TDQSCK_PIFREQ9_B0R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK25_TDQSCK_UIFREQ10_B0R1              Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK25_TDQSCK_PIFREQ10_B0R1              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK26                         (DRAMC_AO_BASE + 0x005AC)
    #define RK1_PRE_TDQSCK26_TDQSCK_UIFREQ9_B1R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK26_TDQSCK_PIFREQ9_B1R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK26_TDQSCK_UIFREQ10_B1R1              Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK26_TDQSCK_PIFREQ10_B1R1              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK27                         (DRAMC_AO_BASE + 0x005B0)
    #define RK1_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B1R1            Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B1R1           Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B0R1            Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B0R1           Fld(6, 18) //[23:18]

#define DRAMC_REG_RK1_PRE_TDQSCK28                         (DRAMC_AO_BASE + 0x005B4)
    #define RK1_PRE_TDQSCK28_TDQSCK_UIFREQ9_B2R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK28_TDQSCK_PIFREQ9_B2R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK28_TDQSCK_UIFREQ10_B2R1              Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK28_TDQSCK_PIFREQ10_B2R1              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK29                         (DRAMC_AO_BASE + 0x005B8)
    #define RK1_PRE_TDQSCK29_TDQSCK_UIFREQ9_B3R1               Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK29_TDQSCK_PIFREQ9_B3R1               Fld(7, 6) //[12:6]
    #define RK1_PRE_TDQSCK29_TDQSCK_UIFREQ10_B3R1              Fld(6, 13) //[18:13]
    #define RK1_PRE_TDQSCK29_TDQSCK_PIFREQ10_B3R1              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK1_PRE_TDQSCK30                         (DRAMC_AO_BASE + 0x005BC)
    #define RK1_PRE_TDQSCK30_TDQSCK_UIFREQ9_P1_B3R1            Fld(6, 0) //[5:0]
    #define RK1_PRE_TDQSCK30_TDQSCK_UIFREQ10_P1_B3R1           Fld(6, 6) //[11:6]
    #define RK1_PRE_TDQSCK30_TDQSCK_UIFREQ9_P1_B2R1            Fld(6, 12) //[17:12]
    #define RK1_PRE_TDQSCK30_TDQSCK_UIFREQ10_P1_B2R1           Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_PRE_TDQSCK25                         (DRAMC_AO_BASE + 0x005C0)
    #define RK2_PRE_TDQSCK25_TDQSCK_UIFREQ9_B0R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK25_TDQSCK_PIFREQ9_B0R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK25_TDQSCK_UIFREQ10_B0R2              Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK25_TDQSCK_PIFREQ10_B0R2              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK26                         (DRAMC_AO_BASE + 0x005C4)
    #define RK2_PRE_TDQSCK26_TDQSCK_UIFREQ9_B1R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK26_TDQSCK_PIFREQ9_B1R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK26_TDQSCK_UIFREQ10_B1R2              Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK26_TDQSCK_PIFREQ10_B1R2              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK27                         (DRAMC_AO_BASE + 0x005C8)
    #define RK2_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B1R2            Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B1R2           Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK27_TDQSCK_UIFREQ9_P1_B0R2            Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK27_TDQSCK_UIFREQ10_P1_B0R2           Fld(6, 18) //[23:18]

#define DRAMC_REG_RK2_PRE_TDQSCK28                         (DRAMC_AO_BASE + 0x005CC)
    #define RK2_PRE_TDQSCK28_TDQSCK_UIFREQ9_B2R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK28_TDQSCK_PIFREQ9_B2R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK28_TDQSCK_UIFREQ10_B2R2              Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK28_TDQSCK_PIFREQ10_B2R2              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK29                         (DRAMC_AO_BASE + 0x005D0)
    #define RK2_PRE_TDQSCK29_TDQSCK_UIFREQ9_B3R2               Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK29_TDQSCK_PIFREQ9_B3R2               Fld(7, 6) //[12:6]
    #define RK2_PRE_TDQSCK29_TDQSCK_UIFREQ10_B3R2              Fld(6, 13) //[18:13]
    #define RK2_PRE_TDQSCK29_TDQSCK_PIFREQ10_B3R2              Fld(7, 19) //[25:19]

#define DRAMC_REG_RK2_PRE_TDQSCK30                         (DRAMC_AO_BASE + 0x005D4)
    #define RK2_PRE_TDQSCK30_TDQSCK_UIFREQ9_P1_B3R2            Fld(6, 0) //[5:0]
    #define RK2_PRE_TDQSCK30_TDQSCK_UIFREQ10_P1_B3R2           Fld(6, 6) //[11:6]
    #define RK2_PRE_TDQSCK30_TDQSCK_UIFREQ9_P1_B2R2            Fld(6, 12) //[17:12]
    #define RK2_PRE_TDQSCK30_TDQSCK_UIFREQ10_P1_B2R2           Fld(6, 18) //[23:18]

#define DRAMC_REG_SHU_ACTIM0                               (DRAMC_AO_BASE + 0x00800)
    #define SHU_ACTIM0_TWTR                                    Fld(4, 0) //[3:0]
    #define SHU_ACTIM0_TWR                                     Fld(5, 8) //[12:8]
    #define SHU_ACTIM0_TRRD                                    Fld(3, 16) //[18:16]
    #define SHU_ACTIM0_TRCD                                    Fld(4, 24) //[27:24]

#define DRAMC_REG_SHU_ACTIM1                               (DRAMC_AO_BASE + 0x00804)
    #define SHU_ACTIM1_TRPAB                                   Fld(3, 0) //[2:0]
    #define SHU_ACTIM1_TRP                                     Fld(4, 8) //[11:8]
    #define SHU_ACTIM1_TRAS                                    Fld(4, 16) //[19:16]
    #define SHU_ACTIM1_TRC                                     Fld(5, 24) //[28:24]

#define DRAMC_REG_SHU_ACTIM2                               (DRAMC_AO_BASE + 0x00808)
    #define SHU_ACTIM2_TXP                                     Fld(3, 0) //[2:0]
    #define SHU_ACTIM2_EARLYCKECNT                             Fld(3, 4) //[6:4]
    #define SHU_ACTIM2_TRTP                                    Fld(3, 8) //[10:8]
    #define SHU_ACTIM2_TR2W                                    Fld(4, 16) //[19:16]
    #define SHU_ACTIM2_TFAW                                    Fld(5, 24) //[28:24]

#define DRAMC_REG_SHU_ACTIM3                               (DRAMC_AO_BASE + 0x0080C)
    #define SHU_ACTIM3_TRFCPB                                  Fld(8, 0) //[7:0]
    #define SHU_ACTIM3_MANTMRR                                 Fld(4, 8) //[11:8]
    #define SHU_ACTIM3_TR2MRR                                  Fld(4, 12) //[15:12]
    #define SHU_ACTIM3_TRFC                                    Fld(8, 16) //[23:16]
    #define SHU_ACTIM3_REFCNT                                  Fld(8, 24) //[31:24]

#define DRAMC_REG_SHU_ACTIM4                               (DRAMC_AO_BASE + 0x00810)
    #define SHU_ACTIM4_TXREFCNT                                Fld(10, 0) //[9:0]
    #define SHU_ACTIM4_REFCNT_FR_CLK                           Fld(8, 16) //[23:16]
    #define SHU_ACTIM4_TZQCS                                   Fld(8, 24) //[31:24]

#define DRAMC_REG_SHU_ACTIM5                               (DRAMC_AO_BASE + 0x00814)
    #define SHU_ACTIM5_TR2PD                                   Fld(5, 0) //[4:0]
    #define SHU_ACTIM5_TWTPD                                   Fld(5, 8) //[12:8]
    #define SHU_ACTIM5_TMRR2W                                  Fld(4, 24) //[27:24]

#define DRAMC_REG_SHU_ACTIM6                               (DRAMC_AO_BASE + 0x00818)
    #define SHU_ACTIM6_BGTCCD                                  Fld(2, 0) //[1:0]
    #define SHU_ACTIM6_BGTWTR                                  Fld(4, 4) //[7:4]
    #define SHU_ACTIM6_TWRMPR                                  Fld(4, 8) //[11:8]
    #define SHU_ACTIM6_BGTRRD                                  Fld(3, 12) //[14:12]

#define DRAMC_REG_SHU_ACTIM_XRT                            (DRAMC_AO_BASE + 0x0081C)
    #define SHU_ACTIM_XRT_XRTR2R                               Fld(5, 0) //[4:0]
    #define SHU_ACTIM_XRT_XRTR2W                               Fld(4, 8) //[11:8]
    #define SHU_ACTIM_XRT_XRTW2R                               Fld(4, 16) //[19:16]
    #define SHU_ACTIM_XRT_XRTW2W                               Fld(5, 24) //[28:24]

#define DRAMC_REG_SHU_AC_TIME_05T                          (DRAMC_AO_BASE + 0x00820)
    #define SHU_AC_TIME_05T_TRC_05T                            Fld(1, 0) //[0:0]
    #define SHU_AC_TIME_05T_TRFCPB_05T                         Fld(1, 1) //[1:1]
    #define SHU_AC_TIME_05T_TRFC_05T                           Fld(1, 2) //[2:2]
    #define SHU_AC_TIME_05T_TXP_05T                            Fld(1, 4) //[4:4]
    #define SHU_AC_TIME_05T_TRTP_05T                           Fld(1, 5) //[5:5]
    #define SHU_AC_TIME_05T_TRCD_05T                           Fld(1, 6) //[6:6]
    #define SHU_AC_TIME_05T_TRP_05T                            Fld(1, 7) //[7:7]
    #define SHU_AC_TIME_05T_TRPAB_05T                          Fld(1, 8) //[8:8]
    #define SHU_AC_TIME_05T_TRAS_05T                           Fld(1, 9) //[9:9]
    #define SHU_AC_TIME_05T_TWR_M05T                           Fld(1, 10) //[10:10]
    #define SHU_AC_TIME_05T_TRRD_05T                           Fld(1, 12) //[12:12]
    #define SHU_AC_TIME_05T_TFAW_05T                           Fld(1, 13) //[13:13]
    #define SHU_AC_TIME_05T_TR2PD_05T                          Fld(1, 15) //[15:15]
    #define SHU_AC_TIME_05T_TWTPD_M05T                         Fld(1, 16) //[16:16]
    #define SHU_AC_TIME_05T_BGTRRD_05T                         Fld(1, 21) //[21:21]
    #define SHU_AC_TIME_05T_BGTCCD_05T                         Fld(1, 22) //[22:22]
    #define SHU_AC_TIME_05T_BGTWTR_05T                         Fld(1, 23) //[23:23]
    #define SHU_AC_TIME_05T_TR2W_05T                           Fld(1, 24) //[24:24]
    #define SHU_AC_TIME_05T_TWTR_M05T                          Fld(1, 25) //[25:25]
    #define SHU_AC_TIME_05T_XRTR2W_05T                         Fld(1, 26) //[26:26]
    #define SHU_AC_TIME_05T_XRTW2R_M05T                        Fld(1, 27) //[27:27]

#define DRAMC_REG_SHU_AC_DERATING0                         (DRAMC_AO_BASE + 0x00824)
    #define SHU_AC_DERATING0_ACDERATEEN                        Fld(1, 0) //[0:0]
    #define SHU_AC_DERATING0_TRRD_DERATE                       Fld(3, 16) //[18:16]
    #define SHU_AC_DERATING0_TRCD_DERATE                       Fld(4, 24) //[27:24]

#define DRAMC_REG_SHU_AC_DERATING1                         (DRAMC_AO_BASE + 0x00828)
    #define SHU_AC_DERATING1_TRPAB_DERATE                      Fld(3, 0) //[2:0]
    #define SHU_AC_DERATING1_TRP_DERATE                        Fld(4, 8) //[11:8]
    #define SHU_AC_DERATING1_TRAS_DERATE                       Fld(4, 16) //[19:16]
    #define SHU_AC_DERATING1_TRC_DERATE                        Fld(5, 24) //[28:24]

#define DRAMC_REG_SHU_AC_DERATING_05T                      (DRAMC_AO_BASE + 0x00830)
    #define SHU_AC_DERATING_05T_TRC_05T_DERATE                 Fld(1, 0) //[0:0]
    #define SHU_AC_DERATING_05T_TRCD_05T_DERATE                Fld(1, 6) //[6:6]
    #define SHU_AC_DERATING_05T_TRP_05T_DERATE                 Fld(1, 7) //[7:7]
    #define SHU_AC_DERATING_05T_TRPAB_05T_DERATE               Fld(1, 8) //[8:8]
    #define SHU_AC_DERATING_05T_TRAS_05T_DERATE                Fld(1, 9) //[9:9]
    #define SHU_AC_DERATING_05T_TRRD_05T_DERATE                Fld(1, 12) //[12:12]

#define DRAMC_REG_SHU_PRE_TDQSCK1                          (DRAMC_AO_BASE + 0x00834)
    #define SHU_PRE_TDQSCK1_TDQSCK_JUMP_RATIO3                 Fld(8, 0) //[7:0]
    #define SHU_PRE_TDQSCK1_TDQSCK_JUMP_RATIO2                 Fld(8, 8) //[15:8]
    #define SHU_PRE_TDQSCK1_TDQSCK_JUMP_RATIO1                 Fld(8, 16) //[23:16]
    #define SHU_PRE_TDQSCK1_TDQSCK_JUMP_RATIO0                 Fld(8, 24) //[31:24]

#define DRAMC_REG_SHU_PRE_TDQSCK2                          (DRAMC_AO_BASE + 0x00838)
    #define SHU_PRE_TDQSCK2_TDQSCK_JUMP_RATIO7                 Fld(8, 0) //[7:0]
    #define SHU_PRE_TDQSCK2_TDQSCK_JUMP_RATIO6                 Fld(8, 8) //[15:8]
    #define SHU_PRE_TDQSCK2_TDQSCK_JUMP_RATIO5                 Fld(8, 16) //[23:16]
    #define SHU_PRE_TDQSCK2_TDQSCK_JUMP_RATIO4                 Fld(8, 24) //[31:24]

#define DRAMC_REG_SHU_DVFSCTL                              (DRAMC_AO_BASE + 0x0083C)
    #define SHU_DVFSCTL_R_BYPASS_1ST_DLL                       Fld(1, 0) //[0:0]
    #define SHU_DVFSCTL_R_DLL_IDLE                             Fld(7, 4) //[10:4]
    #define SHU_DVFSCTL_TDQSCK_JUMP_RATIO9                     Fld(8, 16) //[23:16]
    #define SHU_DVFSCTL_TDQSCK_JUMP_RATIO8                     Fld(8, 24) //[31:24]

#define DRAMC_REG_SHU_CONF0                                (DRAMC_AO_BASE + 0x00840)
    #define SHU_CONF0_DMPGTIM                                  Fld(6, 0) //[5:0]
    #define SHU_CONF0_ADVREFEN                                 Fld(1, 6) //[6:6]
    #define SHU_CONF0_ADVPREEN                                 Fld(1, 7) //[7:7]
    #define SHU_CONF0_SREF_PD_SEL                              Fld(1, 8) //[8:8]
    #define SHU_CONF0_TRFCPBIG                                 Fld(1, 9) //[9:9]
    #define SHU_CONF0_RDATDIV2                                 Fld(1, 10) //[10:10]
    #define SHU_CONF0_RDATDIV4                                 Fld(1, 11) //[11:11]
    #define SHU_CONF0_REFTHD                                   Fld(4, 12) //[15:12]
    #define SHU_CONF0_REQQUE_DEPTH                             Fld(4, 16) //[19:16]
    #define SHU_CONF0_WDATRGO                                  Fld(1, 22) //[22:22]
    #define SHU_CONF0_DUALSCHEN                                Fld(1, 23) //[23:23]
    #define SHU_CONF0_FREQDIV4                                 Fld(1, 24) //[24:24]
    #define SHU_CONF0_FDIV2                                    Fld(1, 25) //[25:25]
    #define SHU_CONF0_DM64BITEN                                Fld(1, 26) //[26:26]
    #define SHU_CONF0_CL2                                      Fld(1, 27) //[27:27]
    #define SHU_CONF0_BL2                                      Fld(1, 28) //[28:28]
    #define SHU_CONF0_BL4                                      Fld(1, 29) //[29:29]
    #define SHU_CONF0_MATYPE                                   Fld(2, 30) //[31:30]

#define DRAMC_REG_SHU_CONF1                                (DRAMC_AO_BASE + 0x00844)
    #define SHU_CONF1_DATLAT                                   Fld(5, 0) //[4:0]
    #define SHU_CONF1_DATLAT_DSEL                              Fld(5, 8) //[12:8]
    #define SHU_CONF1_REFBW_FR                                 Fld(10, 16) //[25:16]
    #define SHU_CONF1_DATLAT_DSEL_PHY                          Fld(5, 26) //[30:26]
    #define SHU_CONF1_TREFBWIG                                 Fld(1, 31) //[31:31]

#define DRAMC_REG_SHU_CONF2                                (DRAMC_AO_BASE + 0x00848)
    #define SHU_CONF2_TCMDO1LAT                                Fld(8, 0) //[7:0]
    #define SHU_CONF2_FSPCHG_PRDCNT                            Fld(8, 8) //[15:8]
    #define SHU_CONF2_DCMDLYREF                                Fld(3, 16) //[18:16]
    #define SHU_CONF2_DQCMD                                    Fld(1, 25) //[25:25]
    #define SHU_CONF2_DQ16COM1                                 Fld(1, 26) //[26:26]
    #define SHU_CONF2_RA15TOCS1                                Fld(1, 27) //[27:27]
    #define SHU_CONF2_WPRE2T                                   Fld(1, 28) //[28:28]
    #define SHU_CONF2_FASTWAKE2                                Fld(1, 29) //[29:29]
    #define SHU_CONF2_DAREFEN                                  Fld(1, 30) //[30:30]
    #define SHU_CONF2_FASTWAKE                                 Fld(1, 31) //[31:31]

#define DRAMC_REG_SHU_CONF3                                (DRAMC_AO_BASE + 0x0084C)
    #define SHU_CONF3_ZQCSCNT                                  Fld(16, 0) //[15:0]
    #define SHU_CONF3_REFRCNT                                  Fld(9, 16) //[24:16]

#define DRAMC_REG_SHU_STBCAL                               (DRAMC_AO_BASE + 0x00850)
    #define SHU_STBCAL_DMSTBLAT                                Fld(4, 0) //[3:0]
    #define SHU_STBCAL_PICGLAT                                 Fld(3, 4) //[6:4]
    #define SHU_STBCAL_DQSG_MODE                               Fld(1, 8) //[8:8]
    #define SHU_STBCAL_DQSIEN_PICG_MODE                        Fld(1, 9) //[9:9]
    #define SHU_STBCAL_DQSIEN_RX_SELPH_OPT                     Fld(1, 10) //[10:10]
    #define SHU_STBCAL_R0_DQSIEN_PICG_HEAD_EXT_LAT             Fld(3, 16) //[18:16]
    #define SHU_STBCAL_R0_DQSIEN_PICG_TAIL_EXT_LAT             Fld(3, 20) //[22:20]
    #define SHU_STBCAL_R1_DQSIEN_PICG_HEAD_EXT_LAT             Fld(3, 24) //[26:24]
    #define SHU_STBCAL_R1_DQSIEN_PICG_TAIL_EXT_LAT             Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_DQSOSCTHRD                           (DRAMC_AO_BASE + 0x00854)
    #define SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK0                  Fld(12, 0) //[11:0]
    #define SHU_DQSOSCTHRD_DQSOSCTHRD_DEC_RK0                  Fld(12, 12) //[23:12]
    #define SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK1_7TO0             Fld(8, 24) //[31:24]

#define DRAMC_REG_SHU_RANKCTL                              (DRAMC_AO_BASE + 0x00858)
    #define SHU_RANKCTL_RANKINCTL_RXDLY                        Fld(4, 0) //[3:0]
    #define SHU_RANKCTL_RANK_RXDLY_OPT                         Fld(1, 4) //[4:4]
    #define SHU_RANKCTL_TXRANKINCTL_TXDLY                      Fld(4, 8) //[11:8]
    #define SHU_RANKCTL_TXRANKINCTL                            Fld(4, 12) //[15:12]
    #define SHU_RANKCTL_TXRANKINCTL_ROOT                       Fld(4, 16) //[19:16]
    #define SHU_RANKCTL_RANKINCTL                              Fld(4, 20) //[23:20]
    #define SHU_RANKCTL_RANKINCTL_ROOT1                        Fld(4, 24) //[27:24]
    #define SHU_RANKCTL_RANKINCTL_PHY                          Fld(4, 28) //[31:28]

#define DRAMC_REG_SHU_CKECTRL                              (DRAMC_AO_BASE + 0x0085C)
    #define SHU_CKECTRL_CMDCKE                                 Fld(3, 16) //[18:16]
    #define SHU_CKECTRL_CKEPRD                                 Fld(3, 20) //[22:20]
    #define SHU_CKECTRL_TCKESRX                                Fld(2, 24) //[25:24]
    #define SHU_CKECTRL_SREF_CK_DLY                            Fld(2, 28) //[29:28]

#define DRAMC_REG_SHU_ODTCTRL                              (DRAMC_AO_BASE + 0x00860)
    #define SHU_ODTCTRL_ROEN                                   Fld(1, 0) //[0:0]
    #define SHU_ODTCTRL_WOEN                                   Fld(1, 1) //[1:1]
    #define SHU_ODTCTRL_RODTEN_SELPH_CG_IG                     Fld(1, 2) //[2:2]
    #define SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG                  Fld(1, 3) //[3:3]
    #define SHU_ODTCTRL_RODT                                   Fld(4, 4) //[7:4]
    #define SHU_ODTCTRL_TWODT                                  Fld(7, 16) //[22:16]
    #define SHU_ODTCTRL_FIXRODT                                Fld(1, 27) //[27:27]
    #define SHU_ODTCTRL_RODTEN_OPT                             Fld(1, 29) //[29:29]
    #define SHU_ODTCTRL_RODTE2                                 Fld(1, 30) //[30:30]
    #define SHU_ODTCTRL_RODTE                                  Fld(1, 31) //[31:31]

#define DRAMC_REG_SHU_IMPCAL1                              (DRAMC_AO_BASE + 0x00864)
    #define SHU_IMPCAL1_IMPCAL_CHKCYCLE                        Fld(3, 0) //[2:0]
    #define SHU_IMPCAL1_IMPDRVP                                Fld(5, 4) //[8:4]
    #define SHU_IMPCAL1_IMPDRVN                                Fld(5, 11) //[15:11]
    #define SHU_IMPCAL1_IMPCAL_CALEN_CYCLE                     Fld(3, 17) //[19:17]
    #define SHU_IMPCAL1_IMPCALCNT                              Fld(8, 20) //[27:20]
    #define SHU_IMPCAL1_IMPCAL_CALICNT                         Fld(4, 28) //[31:28]

#define DRAMC_REG_SHU_DQSOSC_PRD                           (DRAMC_AO_BASE + 0x00868)
    #define SHU_DQSOSC_PRD_DQSOSC_PRDCNT                       Fld(10, 0) //[9:0]
    #define SHU_DQSOSC_PRD_DQSOSCTHRD_INC_RK1_11TO8            Fld(4, 16) //[19:16]
    #define SHU_DQSOSC_PRD_DQSOSCTHRD_DEC_RK1                  Fld(12, 20) //[31:20]

#define DRAMC_REG_SHU_DQSOSCR                              (DRAMC_AO_BASE + 0x0086C)
    #define SHU_DQSOSCR_DQSOSCRCNT                             Fld(8, 0) //[7:0]
    #define SHU_DQSOSCR_DQSOSC_DELTA                           Fld(16, 16) //[31:16]

#define DRAMC_REG_SHU_DQSOSCR2                             (DRAMC_AO_BASE + 0x00870)
    #define SHU_DQSOSCR2_DQSOSCENCNT                           Fld(16, 0) //[15:0]
    #define SHU_DQSOSCR2_DQSOSC_ADV_SEL                        Fld(2, 16) //[17:16]
    #define SHU_DQSOSCR2_DQSOSC_DRS_ADV_SEL                    Fld(2, 18) //[19:18]

#define DRAMC_REG_SHU_RODTENSTB                            (DRAMC_AO_BASE + 0x00874)
    #define SHU_RODTENSTB_RODTENSTB_TRACK_EN                   Fld(1, 0) //[0:0]
    #define SHU_RODTENSTB_RODTEN_P1_ENABLE                     Fld(1, 1) //[1:1]
    #define SHU_RODTENSTB_RODTENSTB_4BYTE_EN                   Fld(1, 2) //[2:2]
    #define SHU_RODTENSTB_RODTENSTB_SELPH_MODE                 Fld(1, 3) //[3:3]
    #define SHU_RODTENSTB_RODTENSTB_TRACK_UDFLWCTRL            Fld(1, 4) //[4:4]
    #define SHU_RODTENSTB_RODTENSTB_OFFSET                     Fld(8, 8) //[15:8]
    #define SHU_RODTENSTB_RODTENSTB_EXT                        Fld(16, 16) //[31:16]

#define DRAMC_REG_SHU_PIPE                                 (DRAMC_AO_BASE + 0x00878)
    #define SHU_PIPE_PHYRXPIPE1                                Fld(1, 0) //[0:0]
    #define SHU_PIPE_PHYRXPIPE2                                Fld(1, 1) //[1:1]
    #define SHU_PIPE_PHYRXPIPE3                                Fld(1, 2) //[2:2]
    #define SHU_PIPE_PHYRXRDSLPIPE1                            Fld(1, 4) //[4:4]
    #define SHU_PIPE_PHYRXRDSLPIPE2                            Fld(1, 5) //[5:5]
    #define SHU_PIPE_PHYRXRDSLPIPE3                            Fld(1, 6) //[6:6]
    #define SHU_PIPE_PHYPIPE1EN                                Fld(1, 8) //[8:8]
    #define SHU_PIPE_PHYPIPE2EN                                Fld(1, 9) //[9:9]
    #define SHU_PIPE_PHYPIPE3EN                                Fld(1, 10) //[10:10]
    #define SHU_PIPE_DLE_LAST_EXTEND3                          Fld(1, 26) //[26:26]
    #define SHU_PIPE_READ_START_EXTEND3                        Fld(1, 27) //[27:27]
    #define SHU_PIPE_DLE_LAST_EXTEND2                          Fld(1, 28) //[28:28]
    #define SHU_PIPE_READ_START_EXTEND2                        Fld(1, 29) //[29:29]
    #define SHU_PIPE_DLE_LAST_EXTEND1                          Fld(1, 30) //[30:30]
    #define SHU_PIPE_READ_START_EXTEND1                        Fld(1, 31) //[31:31]

#define DRAMC_REG_SHU_TEST1                                (DRAMC_AO_BASE + 0x0087C)
    #define SHU_TEST1_LATNORMPOP                               Fld(5, 8) //[12:8]
    #define SHU_TEST1_DQSICALBLCOK_CNT                         Fld(3, 20) //[22:20]
    #define SHU_TEST1_DQSICALI_NEW                             Fld(1, 23) //[23:23]

#define DRAMC_REG_SHU_SELPH_CA1                            (DRAMC_AO_BASE + 0x00880)
    #define SHU_SELPH_CA1_TXDLY_CS                             Fld(3, 0) //[2:0]
    #define SHU_SELPH_CA1_TXDLY_CKE                            Fld(3, 4) //[6:4]
    #define SHU_SELPH_CA1_TXDLY_ODT                            Fld(3, 8) //[10:8]
    #define SHU_SELPH_CA1_TXDLY_RESET                          Fld(3, 12) //[14:12]
    #define SHU_SELPH_CA1_TXDLY_WE                             Fld(3, 16) //[18:16]
    #define SHU_SELPH_CA1_TXDLY_CAS                            Fld(3, 20) //[22:20]
    #define SHU_SELPH_CA1_TXDLY_RAS                            Fld(3, 24) //[26:24]
    #define SHU_SELPH_CA1_TXDLY_CS1                            Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_SELPH_CA2                            (DRAMC_AO_BASE + 0x00884)
    #define SHU_SELPH_CA2_TXDLY_BA0                            Fld(3, 0) //[2:0]
    #define SHU_SELPH_CA2_TXDLY_BA1                            Fld(3, 4) //[6:4]
    #define SHU_SELPH_CA2_TXDLY_BA2                            Fld(3, 8) //[10:8]
    #define SHU_SELPH_CA2_TXDLY_CMD                            Fld(5, 16) //[20:16]
    #define SHU_SELPH_CA2_TXDLY_CKE1                           Fld(3, 24) //[26:24]

#define DRAMC_REG_SHU_SELPH_CA3                            (DRAMC_AO_BASE + 0x00888)
    #define SHU_SELPH_CA3_TXDLY_RA0                            Fld(3, 0) //[2:0]
    #define SHU_SELPH_CA3_TXDLY_RA1                            Fld(3, 4) //[6:4]
    #define SHU_SELPH_CA3_TXDLY_RA2                            Fld(3, 8) //[10:8]
    #define SHU_SELPH_CA3_TXDLY_RA3                            Fld(3, 12) //[14:12]
    #define SHU_SELPH_CA3_TXDLY_RA4                            Fld(3, 16) //[18:16]
    #define SHU_SELPH_CA3_TXDLY_RA5                            Fld(3, 20) //[22:20]
    #define SHU_SELPH_CA3_TXDLY_RA6                            Fld(3, 24) //[26:24]
    #define SHU_SELPH_CA3_TXDLY_RA7                            Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_SELPH_CA4                            (DRAMC_AO_BASE + 0x0088C)
    #define SHU_SELPH_CA4_TXDLY_RA8                            Fld(3, 0) //[2:0]
    #define SHU_SELPH_CA4_TXDLY_RA9                            Fld(3, 4) //[6:4]
    #define SHU_SELPH_CA4_TXDLY_RA10                           Fld(3, 8) //[10:8]
    #define SHU_SELPH_CA4_TXDLY_RA11                           Fld(3, 12) //[14:12]
    #define SHU_SELPH_CA4_TXDLY_RA12                           Fld(3, 16) //[18:16]
    #define SHU_SELPH_CA4_TXDLY_RA13                           Fld(3, 20) //[22:20]
    #define SHU_SELPH_CA4_TXDLY_RA14                           Fld(3, 24) //[26:24]
    #define SHU_SELPH_CA4_TXDLY_RA15                           Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_SELPH_CA5                            (DRAMC_AO_BASE + 0x00890)
    #define SHU_SELPH_CA5_DLY_CS                               Fld(3, 0) //[2:0]
    #define SHU_SELPH_CA5_DLY_CKE                              Fld(3, 4) //[6:4]
    #define SHU_SELPH_CA5_DLY_ODT                              Fld(3, 8) //[10:8]
    #define SHU_SELPH_CA5_DLY_RESET                            Fld(3, 12) //[14:12]
    #define SHU_SELPH_CA5_DLY_WE                               Fld(3, 16) //[18:16]
    #define SHU_SELPH_CA5_DLY_CAS                              Fld(3, 20) //[22:20]
    #define SHU_SELPH_CA5_DLY_RAS                              Fld(3, 24) //[26:24]
    #define SHU_SELPH_CA5_DLY_CS1                              Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_SELPH_CA6                            (DRAMC_AO_BASE + 0x00894)
    #define SHU_SELPH_CA6_DLY_BA0                              Fld(3, 0) //[2:0]
    #define SHU_SELPH_CA6_DLY_BA1                              Fld(3, 4) //[6:4]
    #define SHU_SELPH_CA6_DLY_BA2                              Fld(3, 8) //[10:8]
    #define SHU_SELPH_CA6_DLY_CKE1                             Fld(3, 24) //[26:24]

#define DRAMC_REG_SHU_SELPH_CA7                            (DRAMC_AO_BASE + 0x00898)
    #define SHU_SELPH_CA7_DLY_RA0                              Fld(3, 0) //[2:0]
    #define SHU_SELPH_CA7_DLY_RA1                              Fld(3, 4) //[6:4]
    #define SHU_SELPH_CA7_DLY_RA2                              Fld(3, 8) //[10:8]
    #define SHU_SELPH_CA7_DLY_RA3                              Fld(3, 12) //[14:12]
    #define SHU_SELPH_CA7_DLY_RA4                              Fld(3, 16) //[18:16]
    #define SHU_SELPH_CA7_DLY_RA5                              Fld(3, 20) //[22:20]
    #define SHU_SELPH_CA7_DLY_RA6                              Fld(3, 24) //[26:24]
    #define SHU_SELPH_CA7_DLY_RA7                              Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_SELPH_CA8                            (DRAMC_AO_BASE + 0x0089C)
    #define SHU_SELPH_CA8_DLY_RA8                              Fld(3, 0) //[2:0]
    #define SHU_SELPH_CA8_DLY_RA9                              Fld(3, 4) //[6:4]
    #define SHU_SELPH_CA8_DLY_RA10                             Fld(3, 8) //[10:8]
    #define SHU_SELPH_CA8_DLY_RA11                             Fld(3, 12) //[14:12]
    #define SHU_SELPH_CA8_DLY_RA12                             Fld(3, 16) //[18:16]
    #define SHU_SELPH_CA8_DLY_RA13                             Fld(3, 20) //[22:20]
    #define SHU_SELPH_CA8_DLY_RA14                             Fld(3, 24) //[26:24]
    #define SHU_SELPH_CA8_DLY_RA15                             Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_SELPH_DQS0                           (DRAMC_AO_BASE + 0x008A0)
    #define SHU_SELPH_DQS0_TXDLY_DQS0                          Fld(3, 0) //[2:0]
    #define SHU_SELPH_DQS0_TXDLY_DQS1                          Fld(3, 4) //[6:4]
    #define SHU_SELPH_DQS0_TXDLY_DQS2                          Fld(3, 8) //[10:8]
    #define SHU_SELPH_DQS0_TXDLY_DQS3                          Fld(3, 12) //[14:12]
    #define SHU_SELPH_DQS0_TXDLY_OEN_DQS0                      Fld(3, 16) //[18:16]
    #define SHU_SELPH_DQS0_TXDLY_OEN_DQS1                      Fld(3, 20) //[22:20]
    #define SHU_SELPH_DQS0_TXDLY_OEN_DQS2                      Fld(3, 24) //[26:24]
    #define SHU_SELPH_DQS0_TXDLY_OEN_DQS3                      Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_SELPH_DQS1                           (DRAMC_AO_BASE + 0x008A4)
    #define SHU_SELPH_DQS1_DLY_DQS0                            Fld(3, 0) //[2:0]
    #define SHU_SELPH_DQS1_DLY_DQS1                            Fld(3, 4) //[6:4]
    #define SHU_SELPH_DQS1_DLY_DQS2                            Fld(3, 8) //[10:8]
    #define SHU_SELPH_DQS1_DLY_DQS3                            Fld(3, 12) //[14:12]
    #define SHU_SELPH_DQS1_DLY_OEN_DQS0                        Fld(3, 16) //[18:16]
    #define SHU_SELPH_DQS1_DLY_OEN_DQS1                        Fld(3, 20) //[22:20]
    #define SHU_SELPH_DQS1_DLY_OEN_DQS2                        Fld(3, 24) //[26:24]
    #define SHU_SELPH_DQS1_DLY_OEN_DQS3                        Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_DRVING1                              (DRAMC_AO_BASE + 0x008A8)
    #define SHU_DRVING1_DQDRVN2                                Fld(5, 0) //[4:0]
    #define SHU_DRVING1_DQDRVP2                                Fld(5, 5) //[9:5]
    #define SHU_DRVING1_DQSDRVN1                               Fld(5, 10) //[14:10]
    #define SHU_DRVING1_DQSDRVP1                               Fld(5, 15) //[19:15]
    #define SHU_DRVING1_DQSDRVN2                               Fld(5, 20) //[24:20]
    #define SHU_DRVING1_DQSDRVP2                               Fld(5, 25) //[29:25]
    #define SHU_DRVING1_DIS_IMP_ODTN_TRACK                     Fld(1, 30) //[30:30]
    #define SHU_DRVING1_DIS_IMPCAL_HW                          Fld(1, 31) //[31:31]

#define DRAMC_REG_SHU_DRVING2                              (DRAMC_AO_BASE + 0x008AC)
    #define SHU_DRVING2_CMDDRVN1                               Fld(5, 0) //[4:0]
    #define SHU_DRVING2_CMDDRVP1                               Fld(5, 5) //[9:5]
    #define SHU_DRVING2_CMDDRVN2                               Fld(5, 10) //[14:10]
    #define SHU_DRVING2_CMDDRVP2                               Fld(5, 15) //[19:15]
    #define SHU_DRVING2_DQDRVN1                                Fld(5, 20) //[24:20]
    #define SHU_DRVING2_DQDRVP1                                Fld(5, 25) //[29:25]
    #define SHU_DRVING2_DIS_IMPCAL_ODT_EN                      Fld(1, 31) //[31:31]

#define DRAMC_REG_SHU_DRVING3                              (DRAMC_AO_BASE + 0x008B0)
    #define SHU_DRVING3_DQODTN2                                Fld(5, 0) //[4:0]
    #define SHU_DRVING3_DQODTP2                                Fld(5, 5) //[9:5]
    #define SHU_DRVING3_DQSODTN                                Fld(5, 10) //[14:10]
    #define SHU_DRVING3_DQSODTP                                Fld(5, 15) //[19:15]
    #define SHU_DRVING3_DQSODTN2                               Fld(5, 20) //[24:20]
    #define SHU_DRVING3_DQSODTP2                               Fld(5, 25) //[29:25]

#define DRAMC_REG_SHU_DRVING4                              (DRAMC_AO_BASE + 0x008B4)
    #define SHU_DRVING4_CMDODTN1                               Fld(5, 0) //[4:0]
    #define SHU_DRVING4_CMDODTP1                               Fld(5, 5) //[9:5]
    #define SHU_DRVING4_CMDODTN2                               Fld(5, 10) //[14:10]
    #define SHU_DRVING4_CMDODTP2                               Fld(5, 15) //[19:15]
    #define SHU_DRVING4_DQODTN1                                Fld(5, 20) //[24:20]
    #define SHU_DRVING4_DQODTP1                                Fld(5, 25) //[29:25]

#define DRAMC_REG_SHU_DRVING5                              (DRAMC_AO_BASE + 0x008B8)
    #define SHU_DRVING5_DQDRVN3                                Fld(5, 0) //[4:0]
    #define SHU_DRVING5_DQDRVP3                                Fld(5, 5) //[9:5]
    #define SHU_DRVING5_DQCDRVN1                               Fld(5, 10) //[14:10]
    #define SHU_DRVING5_DQCDRVP1                               Fld(5, 15) //[19:15]
    #define SHU_DRVING5_DQSDRVN3                               Fld(5, 20) //[24:20]
    #define SHU_DRVING5_DQSDRVP3                               Fld(5, 25) //[29:25]

#define DRAMC_REG_SHU_DRVING6                              (DRAMC_AO_BASE + 0x008BC)
    #define SHU_DRVING6_DQODTN3                                Fld(5, 0) //[4:0]
    #define SHU_DRVING6_DQODTP3                                Fld(5, 5) //[9:5]
    #define SHU_DRVING6_DQSODTN3                               Fld(5, 20) //[24:20]
    #define SHU_DRVING6_DQSODTP3                               Fld(5, 25) //[29:25]

#define DRAMC_REG_SHU_WODT                                 (DRAMC_AO_BASE + 0x008C0)
    #define SHU_WODT_DISWODT                                   Fld(3, 0) //[2:0]
    #define SHU_WODT_WODTFIX                                   Fld(1, 3) //[3:3]
    #define SHU_WODT_WODTFIXOFF                                Fld(1, 4) //[4:4]
    #define SHU_WODT_DISWODTE                                  Fld(1, 5) //[5:5]
    #define SHU_WODT_DISWODTE2                                 Fld(1, 6) //[6:6]
    #define SHU_WODT_WODTPDEN                                  Fld(1, 7) //[7:7]
    #define SHU_WODT_DQOE_CNT                                  Fld(3, 8) //[10:8]
    #define SHU_WODT_DQOE_OPT                                  Fld(1, 11) //[11:11]
    #define SHU_WODT_TXUPD_SEL                                 Fld(2, 12) //[13:12]
    #define SHU_WODT_TXUPD_W2R_SEL                             Fld(3, 14) //[16:14]
    #define SHU_WODT_DBIWR                                     Fld(1, 29) //[29:29]
    #define SHU_WODT_TWPSTEXT                                  Fld(1, 30) //[30:30]
    #define SHU_WODT_WPST1P5T                                  Fld(1, 31) //[31:31]

#define DRAMC_REG_SHU_DQSG                                 (DRAMC_AO_BASE + 0x008C4)
    #define SHU_DQSG_DLLFRZRFCOPT                              Fld(2, 0) //[1:0]
    #define SHU_DQSG_DLLFRZWROPT                               Fld(2, 4) //[5:4]
    #define SHU_DQSG_R_RSTBCNT_LATCH_OPT                       Fld(3, 8) //[10:8]
    #define SHU_DQSG_STB_UPDMASK_EN                            Fld(1, 11) //[11:11]
    #define SHU_DQSG_STB_UPDMASKCYC                            Fld(4, 12) //[15:12]
    #define SHU_DQSG_DQSINCTL_PRE_SEL                          Fld(1, 16) //[16:16]
    #define SHU_DQSG_SCINTV                                    Fld(6, 20) //[25:20]

#define DRAMC_REG_SHU_SCINTV                               (DRAMC_AO_BASE + 0x008C8)
    #define SHU_SCINTV_ODTREN                                  Fld(1, 0) //[0:0]
    #define SHU_SCINTV_TZQLAT                                  Fld(5, 1) //[5:1]
    #define SHU_SCINTV_TZQLAT2                                 Fld(5, 6) //[10:6]
    #define SHU_SCINTV_RDDQC_INTV                              Fld(2, 11) //[12:11]
    #define SHU_SCINTV_MRW_INTV                                Fld(5, 13) //[17:13]
    #define SHU_SCINTV_DQS2DQ_SHU_PITHRD                       Fld(6, 18) //[23:18]
    #define SHU_SCINTV_DQS2DQ_FILT_PITHRD                      Fld(6, 24) //[29:24]
    #define SHU_SCINTV_DQSOSCENDIS                             Fld(1, 30) //[30:30]

#define DRAMC_REG_SHU_MISC                                 (DRAMC_AO_BASE + 0x008CC)
    #define SHU_MISC_REQQUE_MAXCNT                             Fld(4, 0) //[3:0]
    #define SHU_MISC_CKEHCMD                                   Fld(2, 4) //[5:4]
    #define SHU_MISC_NORMPOP_LEN                               Fld(3, 8) //[10:8]
    #define SHU_MISC_PREA_INTV                                 Fld(5, 12) //[16:12]

#define DRAMC_REG_SHU_DQS2DQ_TX                            (DRAMC_AO_BASE + 0x008D0)
    #define SHU_DQS2DQ_TX_OE2DQ_OFFSET                         Fld(5, 0) //[4:0]

#define DRAMC_REG_SHU_HWSET_MR2                            (DRAMC_AO_BASE + 0x008D4)
    #define SHU_HWSET_MR2_HWSET_MR2_MRSMA                      Fld(13, 0) //[12:0]
    #define SHU_HWSET_MR2_HWSET_MR2_OP                         Fld(8, 16) //[23:16]

#define DRAMC_REG_SHU_HWSET_MR13                           (DRAMC_AO_BASE + 0x008D8)
    #define SHU_HWSET_MR13_HWSET_MR13_MRSMA                    Fld(13, 0) //[12:0]
    #define SHU_HWSET_MR13_HWSET_MR13_OP                       Fld(8, 16) //[23:16]

#define DRAMC_REG_SHU_HWSET_VRCG                           (DRAMC_AO_BASE + 0x008DC)
    #define SHU_HWSET_VRCG_HWSET_VRCG_MRSMA                    Fld(13, 0) //[12:0]
    #define SHU_HWSET_VRCG_HWSET_VRCG_OP                       Fld(8, 16) //[23:16]

#define DRAMC_REG_SHU_PHY_RX_CTRL                          (DRAMC_AO_BASE + 0x008E0)
    #define SHU_PHY_RX_CTRL_RODTENCGEN_HEAD                    Fld(2, 4) //[5:4]
    #define SHU_PHY_RX_CTRL_RODTENCGEN_TAIL                    Fld(2, 6) //[7:6]
    #define SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN               Fld(1, 8) //[8:8]
    #define SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET              Fld(3, 9) //[11:9]
    #define SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET           Fld(2, 14) //[15:14]
    #define SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD                 Fld(3, 16) //[18:16]
    #define SHU_PHY_RX_CTRL_RX_IN_GATE_EN_TAIL                 Fld(3, 20) //[22:20]
    #define SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD                 Fld(3, 24) //[26:24]
    #define SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_TAIL                 Fld(3, 28) //[30:28]

#define DRAMC_REG_SHU_APHY_TX_PICG_CTRL                    (DRAMC_AO_BASE + 0x008E4)
    #define SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT Fld(4, 0) //[3:0]
    #define SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_SEL_P1 Fld(3, 4) //[6:4]
    #define SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_SEL_P0 Fld(3, 8) //[10:8]
    #define SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT    Fld(1, 11) //[11:11]
    #define SHU_APHY_TX_PICG_CTRL_APHYPI_CG_CK_SEL             Fld(4, 20) //[23:20]
    #define SHU_APHY_TX_PICG_CTRL_APHYPI_CG_CK_OPT             Fld(1, 24) //[24:24]
    #define SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_DYN_GATING_SEL    Fld(4, 27) //[30:27]
    #define SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_OPT            Fld(1, 31) //[31:31]

#define DRAMC_REG_SHU_DQSG_RETRY                           (DRAMC_AO_BASE + 0x008E8)
    #define SHU_DQSG_RETRY_R_DQSGRETRY_SW_RESET                Fld(1, 0) //[0:0]
    #define SHU_DQSG_RETRY_R_DQSG_RETRY_SW_EN                  Fld(1, 1) //[1:1]
    #define SHU_DQSG_RETRY_R_DDR1866_PLUS                      Fld(1, 2) //[2:2]
    #define SHU_DQSG_RETRY_R_RETRY_ONCE                        Fld(1, 3) //[3:3]
    #define SHU_DQSG_RETRY_R_RETRY_3TIMES                      Fld(1, 4) //[4:4]
    #define SHU_DQSG_RETRY_R_RETRY_1RANK                       Fld(1, 5) //[5:5]
    #define SHU_DQSG_RETRY_R_RETRY_SAV_MSK                     Fld(1, 6) //[6:6]
    #define SHU_DQSG_RETRY_R_DM4BYTE                           Fld(1, 7) //[7:7]
    #define SHU_DQSG_RETRY_R_DQSIENLAT                         Fld(4, 8) //[11:8]
    #define SHU_DQSG_RETRY_R_STBENCMP_ALLBYTE                  Fld(1, 12) //[12:12]
    #define SHU_DQSG_RETRY_R_XSR_DQSG_RETRY_EN                 Fld(1, 13) //[13:13]
    #define SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE                Fld(1, 14) //[14:14]
    #define SHU_DQSG_RETRY_R_RETRY_CMP_DATA                    Fld(1, 15) //[15:15]
    #define SHU_DQSG_RETRY_R_RETRY_ALE_BLOCK_MASK              Fld(1, 20) //[20:20]
    #define SHU_DQSG_RETRY_R_RDY_SEL_DLE                       Fld(1, 21) //[21:21]
    #define SHU_DQSG_RETRY_R_RETRY_ROUND_NUM                   Fld(2, 24) //[25:24]
    #define SHU_DQSG_RETRY_R_RETRY_RANKSEL_FROM_PHY            Fld(1, 28) //[28:28]
    #define SHU_DQSG_RETRY_R_RETRY_PA_DSIABLE                  Fld(1, 29) //[29:29]
    #define SHU_DQSG_RETRY_R_RETRY_STBEN_RESET_MSK             Fld(1, 30) //[30:30]
    #define SHU_DQSG_RETRY_R_RETRY_USE_BURST_MDOE              Fld(1, 31) //[31:31]

#define DRAMC_REG_SHURK0_DQSCTL                            (DRAMC_AO_BASE + 0x00A00)
    #define SHURK0_DQSCTL_DQSINCTL                             Fld(4, 0) //[3:0]

#define DRAMC_REG_SHURK0_DQSIEN                            (DRAMC_AO_BASE + 0x00A04)
    #define SHURK0_DQSIEN_R0DQS0IEN                            Fld(7, 0) //[6:0]
    #define SHURK0_DQSIEN_R0DQS1IEN                            Fld(7, 8) //[14:8]
    #define SHURK0_DQSIEN_R0DQS2IEN                            Fld(7, 16) //[22:16]
    #define SHURK0_DQSIEN_R0DQS3IEN                            Fld(7, 24) //[30:24]

#define DRAMC_REG_SHURK0_DQSCAL                            (DRAMC_AO_BASE + 0x00A08)
    #define SHURK0_DQSCAL_R0DQSIENLLMT                         Fld(7, 0) //[6:0]
    #define SHURK0_DQSCAL_R0DQSIENLLMTEN                       Fld(1, 7) //[7:7]
    #define SHURK0_DQSCAL_R0DQSIENHLMT                         Fld(7, 8) //[14:8]
    #define SHURK0_DQSCAL_R0DQSIENHLMTEN                       Fld(1, 15) //[15:15]

#define DRAMC_REG_SHURK0_PI                                (DRAMC_AO_BASE + 0x00A0C)
    #define SHURK0_PI_RK0_ARPI_DQ_B1                           Fld(6, 0) //[5:0]
    #define SHURK0_PI_RK0_ARPI_DQ_B0                           Fld(6, 8) //[13:8]
    #define SHURK0_PI_RK0_ARPI_DQM_B1                          Fld(6, 16) //[21:16]
    #define SHURK0_PI_RK0_ARPI_DQM_B0                          Fld(6, 24) //[29:24]

#define DRAMC_REG_SHURK0_DQSOSC                            (DRAMC_AO_BASE + 0x00A10)
    #define SHURK0_DQSOSC_DQSOSC_BASE_RK0                      Fld(16, 0) //[15:0]
    #define SHURK0_DQSOSC_DQSOSC_BASE_RK0_B1                   Fld(16, 16) //[31:16]

#define DRAMC_REG_SHURK0_SELPH_ODTEN0                      (DRAMC_AO_BASE + 0x00A1C)
    #define SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN                Fld(3, 0) //[2:0]
    #define SHURK0_SELPH_ODTEN0_TXDLY_B0_RODTEN_P1             Fld(3, 4) //[6:4]
    #define SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN                Fld(3, 8) //[10:8]
    #define SHURK0_SELPH_ODTEN0_TXDLY_B1_RODTEN_P1             Fld(3, 12) //[14:12]
    #define SHURK0_SELPH_ODTEN0_TXDLY_B2_RODTEN                Fld(3, 16) //[18:16]
    #define SHURK0_SELPH_ODTEN0_TXDLY_B2_RODTEN_P1             Fld(3, 20) //[22:20]
    #define SHURK0_SELPH_ODTEN0_TXDLY_B3_RODTEN                Fld(3, 24) //[26:24]
    #define SHURK0_SELPH_ODTEN0_TXDLY_B3_RODTEN_P1             Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK0_SELPH_ODTEN1                      (DRAMC_AO_BASE + 0x00A20)
    #define SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN                  Fld(3, 0) //[2:0]
    #define SHURK0_SELPH_ODTEN1_DLY_B0_RODTEN_P1               Fld(3, 4) //[6:4]
    #define SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN                  Fld(3, 8) //[10:8]
    #define SHURK0_SELPH_ODTEN1_DLY_B1_RODTEN_P1               Fld(3, 12) //[14:12]
    #define SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN                  Fld(3, 16) //[18:16]
    #define SHURK0_SELPH_ODTEN1_DLY_B2_RODTEN_P1               Fld(3, 20) //[22:20]
    #define SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN                  Fld(3, 24) //[26:24]
    #define SHURK0_SELPH_ODTEN1_DLY_B3_RODTEN_P1               Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK0_SELPH_DQSG0                       (DRAMC_AO_BASE + 0x00A24)
    #define SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED               Fld(3, 0) //[2:0]
    #define SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1            Fld(3, 4) //[6:4]
    #define SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED               Fld(3, 8) //[10:8]
    #define SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1            Fld(3, 12) //[14:12]
    #define SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED               Fld(3, 16) //[18:16]
    #define SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1            Fld(3, 20) //[22:20]
    #define SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED               Fld(3, 24) //[26:24]
    #define SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1            Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK0_SELPH_DQSG1                       (DRAMC_AO_BASE + 0x00A28)
    #define SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED              Fld(3, 0) //[2:0]
    #define SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1           Fld(3, 4) //[6:4]
    #define SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED              Fld(3, 8) //[10:8]
    #define SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1           Fld(3, 12) //[14:12]
    #define SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED              Fld(3, 16) //[18:16]
    #define SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1           Fld(3, 20) //[22:20]
    #define SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED              Fld(3, 24) //[26:24]
    #define SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1           Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK0_SELPH_DQ0                         (DRAMC_AO_BASE + 0x00A2C)
    #define SHURK0_SELPH_DQ0_TXDLY_DQ0                         Fld(3, 0) //[2:0]
    #define SHURK0_SELPH_DQ0_TXDLY_DQ1                         Fld(3, 4) //[6:4]
    #define SHURK0_SELPH_DQ0_TXDLY_DQ2                         Fld(3, 8) //[10:8]
    #define SHURK0_SELPH_DQ0_TXDLY_DQ3                         Fld(3, 12) //[14:12]
    #define SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0                     Fld(3, 16) //[18:16]
    #define SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1                     Fld(3, 20) //[22:20]
    #define SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2                     Fld(3, 24) //[26:24]
    #define SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3                     Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK0_SELPH_DQ1                         (DRAMC_AO_BASE + 0x00A30)
    #define SHURK0_SELPH_DQ1_TXDLY_DQM0                        Fld(3, 0) //[2:0]
    #define SHURK0_SELPH_DQ1_TXDLY_DQM1                        Fld(3, 4) //[6:4]
    #define SHURK0_SELPH_DQ1_TXDLY_DQM2                        Fld(3, 8) //[10:8]
    #define SHURK0_SELPH_DQ1_TXDLY_DQM3                        Fld(3, 12) //[14:12]
    #define SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0                    Fld(3, 16) //[18:16]
    #define SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1                    Fld(3, 20) //[22:20]
    #define SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2                    Fld(3, 24) //[26:24]
    #define SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3                    Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK0_SELPH_DQ2                         (DRAMC_AO_BASE + 0x00A34)
    #define SHURK0_SELPH_DQ2_DLY_DQ0                           Fld(3, 0) //[2:0]
    #define SHURK0_SELPH_DQ2_DLY_DQ1                           Fld(3, 4) //[6:4]
    #define SHURK0_SELPH_DQ2_DLY_DQ2                           Fld(3, 8) //[10:8]
    #define SHURK0_SELPH_DQ2_DLY_DQ3                           Fld(3, 12) //[14:12]
    #define SHURK0_SELPH_DQ2_DLY_OEN_DQ0                       Fld(3, 16) //[18:16]
    #define SHURK0_SELPH_DQ2_DLY_OEN_DQ1                       Fld(3, 20) //[22:20]
    #define SHURK0_SELPH_DQ2_DLY_OEN_DQ2                       Fld(3, 24) //[26:24]
    #define SHURK0_SELPH_DQ2_DLY_OEN_DQ3                       Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK0_SELPH_DQ3                         (DRAMC_AO_BASE + 0x00A38)
    #define SHURK0_SELPH_DQ3_DLY_DQM0                          Fld(3, 0) //[2:0]
    #define SHURK0_SELPH_DQ3_DLY_DQM1                          Fld(3, 4) //[6:4]
    #define SHURK0_SELPH_DQ3_DLY_DQM2                          Fld(3, 8) //[10:8]
    #define SHURK0_SELPH_DQ3_DLY_DQM3                          Fld(3, 12) //[14:12]
    #define SHURK0_SELPH_DQ3_DLY_OEN_DQM0                      Fld(3, 16) //[18:16]
    #define SHURK0_SELPH_DQ3_DLY_OEN_DQM1                      Fld(3, 20) //[22:20]
    #define SHURK0_SELPH_DQ3_DLY_OEN_DQM2                      Fld(3, 24) //[26:24]
    #define SHURK0_SELPH_DQ3_DLY_OEN_DQM3                      Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK0_DQS2DQ_CAL1                       (DRAMC_AO_BASE + 0x00A40)
    #define SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0            Fld(11, 0) //[10:0]
    #define SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1            Fld(11, 16) //[26:16]

#define DRAMC_REG_SHURK0_DQS2DQ_CAL2                       (DRAMC_AO_BASE + 0x00A44)
    #define SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0            Fld(11, 0) //[10:0]
    #define SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1            Fld(11, 16) //[26:16]

#define DRAMC_REG_SHURK0_DQS2DQ_CAL3                       (DRAMC_AO_BASE + 0x00A48)
    #define SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ0        Fld(6, 0) //[5:0]
    #define SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ1        Fld(6, 6) //[11:6]
    #define SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ0_B4TO0  Fld(5, 12) //[16:12]
    #define SHURK0_DQS2DQ_CAL3_BOOT_TARG_UI_RK0_OEN_DQ1_B4TO0  Fld(5, 17) //[21:17]

#define DRAMC_REG_SHURK0_DQS2DQ_CAL4                       (DRAMC_AO_BASE + 0x00A4C)
    #define SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM0       Fld(6, 0) //[5:0]
    #define SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM1       Fld(6, 6) //[11:6]
    #define SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM0_B4TO0 Fld(5, 12) //[16:12]
    #define SHURK0_DQS2DQ_CAL4_BOOT_TARG_UI_RK0_OEN_DQM1_B4TO0 Fld(5, 17) //[21:17]

#define DRAMC_REG_SHURK0_DQS2DQ_CAL5                       (DRAMC_AO_BASE + 0x00A50)
    #define SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0           Fld(11, 0) //[10:0]
    #define SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1           Fld(11, 16) //[26:16]

#define DRAMC_REG_SHURK1_DQSCTL                            (DRAMC_AO_BASE + 0x00B00)
    #define SHURK1_DQSCTL_R1DQSINCTL                           Fld(4, 0) //[3:0]

#define DRAMC_REG_SHURK1_DQSIEN                            (DRAMC_AO_BASE + 0x00B04)
    #define SHURK1_DQSIEN_R1DQS0IEN                            Fld(7, 0) //[6:0]
    #define SHURK1_DQSIEN_R1DQS1IEN                            Fld(7, 8) //[14:8]
    #define SHURK1_DQSIEN_R1DQS2IEN                            Fld(7, 16) //[22:16]
    #define SHURK1_DQSIEN_R1DQS3IEN                            Fld(7, 24) //[30:24]

#define DRAMC_REG_SHURK1_DQSCAL                            (DRAMC_AO_BASE + 0x00B08)
    #define SHURK1_DQSCAL_R1DQSIENLLMT                         Fld(7, 0) //[6:0]
    #define SHURK1_DQSCAL_R1DQSIENLLMTEN                       Fld(1, 7) //[7:7]
    #define SHURK1_DQSCAL_R1DQSIENHLMT                         Fld(7, 8) //[14:8]
    #define SHURK1_DQSCAL_R1DQSIENHLMTEN                       Fld(1, 15) //[15:15]

#define DRAMC_REG_SHURK1_PI                                (DRAMC_AO_BASE + 0x00B0C)
    #define SHURK1_PI_RK1_ARPI_DQ_B1                           Fld(6, 0) //[5:0]
    #define SHURK1_PI_RK1_ARPI_DQ_B0                           Fld(6, 8) //[13:8]
    #define SHURK1_PI_RK1_ARPI_DQM_B1                          Fld(6, 16) //[21:16]
    #define SHURK1_PI_RK1_ARPI_DQM_B0                          Fld(6, 24) //[29:24]

#define DRAMC_REG_SHURK1_DQSOSC                            (DRAMC_AO_BASE + 0x00B10)
    #define SHURK1_DQSOSC_DQSOSC_BASE_RK1                      Fld(16, 0) //[15:0]
    #define SHURK1_DQSOSC_DQSOSC_BASE_RK1_B1                   Fld(16, 16) //[31:16]

#define DRAMC_REG_SHURK1_SELPH_ODTEN0                      (DRAMC_AO_BASE + 0x00B1C)
    #define SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN              Fld(3, 0) //[2:0]
    #define SHURK1_SELPH_ODTEN0_TXDLY_B0_R1RODTEN_P1           Fld(3, 4) //[6:4]
    #define SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN              Fld(3, 8) //[10:8]
    #define SHURK1_SELPH_ODTEN0_TXDLY_B1_R1RODTEN_P1           Fld(3, 12) //[14:12]
    #define SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN              Fld(3, 16) //[18:16]
    #define SHURK1_SELPH_ODTEN0_TXDLY_B2_R1RODTEN_P1           Fld(3, 20) //[22:20]
    #define SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN              Fld(3, 24) //[26:24]
    #define SHURK1_SELPH_ODTEN0_TXDLY_B3_R1RODTEN_P1           Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK1_SELPH_ODTEN1                      (DRAMC_AO_BASE + 0x00B20)
    #define SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN                Fld(3, 0) //[2:0]
    #define SHURK1_SELPH_ODTEN1_DLY_B0_R1RODTEN_P1             Fld(3, 4) //[6:4]
    #define SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN                Fld(3, 8) //[10:8]
    #define SHURK1_SELPH_ODTEN1_DLY_B1_R1RODTEN_P1             Fld(3, 12) //[14:12]
    #define SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN                Fld(3, 16) //[18:16]
    #define SHURK1_SELPH_ODTEN1_DLY_B2_R1RODTEN_P1             Fld(3, 20) //[22:20]
    #define SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN                Fld(3, 24) //[26:24]
    #define SHURK1_SELPH_ODTEN1_DLY_B3_R1RODTEN_P1             Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK1_SELPH_DQSG0                       (DRAMC_AO_BASE + 0x00B24)
    #define SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED             Fld(3, 0) //[2:0]
    #define SHURK1_SELPH_DQSG0_TX_DLY_R1DQS0_GATED_P1          Fld(3, 4) //[6:4]
    #define SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED             Fld(3, 8) //[10:8]
    #define SHURK1_SELPH_DQSG0_TX_DLY_R1DQS1_GATED_P1          Fld(3, 12) //[14:12]
    #define SHURK1_SELPH_DQSG0_TX_DLY_R1DQS2_GATED             Fld(3, 16) //[18:16]
    #define SHURK1_SELPH_DQSG0_TX_DLY_R1DQS2_GATED_P1          Fld(3, 20) //[22:20]
    #define SHURK1_SELPH_DQSG0_TX_DLY_R1DQS3_GATED             Fld(3, 24) //[26:24]
    #define SHURK1_SELPH_DQSG0_TX_DLY_R1DQS3_GATED_P1          Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK1_SELPH_DQSG1                       (DRAMC_AO_BASE + 0x00B28)
    #define SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED            Fld(3, 0) //[2:0]
    #define SHURK1_SELPH_DQSG1_REG_DLY_R1DQS0_GATED_P1         Fld(3, 4) //[6:4]
    #define SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED            Fld(3, 8) //[10:8]
    #define SHURK1_SELPH_DQSG1_REG_DLY_R1DQS1_GATED_P1         Fld(3, 12) //[14:12]
    #define SHURK1_SELPH_DQSG1_REG_DLY_R1DQS2_GATED            Fld(3, 16) //[18:16]
    #define SHURK1_SELPH_DQSG1_REG_DLY_R1DQS2_GATED_P1         Fld(3, 20) //[22:20]
    #define SHURK1_SELPH_DQSG1_REG_DLY_R1DQS3_GATED            Fld(3, 24) //[26:24]
    #define SHURK1_SELPH_DQSG1_REG_DLY_R1DQS3_GATED_P1         Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK1_SELPH_DQ0                         (DRAMC_AO_BASE + 0x00B2C)
    #define SHURK1_SELPH_DQ0_TX_DLY_R1DQ0                      Fld(3, 0) //[2:0]
    #define SHURK1_SELPH_DQ0_TX_DLY_R1DQ1                      Fld(3, 4) //[6:4]
    #define SHURK1_SELPH_DQ0_TX_DLY_R1DQ2                      Fld(3, 8) //[10:8]
    #define SHURK1_SELPH_DQ0_TX_DLY_R1DQ3                      Fld(3, 12) //[14:12]
    #define SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0                  Fld(3, 16) //[18:16]
    #define SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1                  Fld(3, 20) //[22:20]
    #define SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2                  Fld(3, 24) //[26:24]
    #define SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3                  Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK1_SELPH_DQ1                         (DRAMC_AO_BASE + 0x00B30)
    #define SHURK1_SELPH_DQ1_TX_DLY_R1DQM0                     Fld(3, 0) //[2:0]
    #define SHURK1_SELPH_DQ1_TX_DLY_R1DQM1                     Fld(3, 4) //[6:4]
    #define SHURK1_SELPH_DQ1_TX_DLY_R1DQM2                     Fld(3, 8) //[10:8]
    #define SHURK1_SELPH_DQ1_TX_DLY_R1DQM3                     Fld(3, 12) //[14:12]
    #define SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0                 Fld(3, 16) //[18:16]
    #define SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1                 Fld(3, 20) //[22:20]
    #define SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2                 Fld(3, 24) //[26:24]
    #define SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3                 Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK1_SELPH_DQ2                         (DRAMC_AO_BASE + 0x00B34)
    #define SHURK1_SELPH_DQ2_DLY_R1DQ0                         Fld(3, 0) //[2:0]
    #define SHURK1_SELPH_DQ2_DLY_R1DQ1                         Fld(3, 4) //[6:4]
    #define SHURK1_SELPH_DQ2_DLY_R1DQ2                         Fld(3, 8) //[10:8]
    #define SHURK1_SELPH_DQ2_DLY_R1DQ3                         Fld(3, 12) //[14:12]
    #define SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0                     Fld(3, 16) //[18:16]
    #define SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1                     Fld(3, 20) //[22:20]
    #define SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2                     Fld(3, 24) //[26:24]
    #define SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3                     Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK1_SELPH_DQ3                         (DRAMC_AO_BASE + 0x00B38)
    #define SHURK1_SELPH_DQ3_DLY_R1DQM0                        Fld(3, 0) //[2:0]
    #define SHURK1_SELPH_DQ3_DLY_R1DQM1                        Fld(3, 4) //[6:4]
    #define SHURK1_SELPH_DQ3_DLY_R1DQM2                        Fld(3, 8) //[10:8]
    #define SHURK1_SELPH_DQ3_DLY_R1DQM3                        Fld(3, 12) //[14:12]
    #define SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0                    Fld(3, 16) //[18:16]
    #define SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1                    Fld(3, 20) //[22:20]
    #define SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2                    Fld(3, 24) //[26:24]
    #define SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3                    Fld(3, 28) //[30:28]

#define DRAMC_REG_SHURK1_DQS2DQ_CAL1                       (DRAMC_AO_BASE + 0x00B40)
    #define SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ0            Fld(11, 0) //[10:0]
    #define SHURK1_DQS2DQ_CAL1_BOOT_ORIG_UI_RK1_DQ1            Fld(11, 16) //[26:16]

#define DRAMC_REG_SHURK1_DQS2DQ_CAL2                       (DRAMC_AO_BASE + 0x00B44)
    #define SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ0            Fld(11, 0) //[10:0]
    #define SHURK1_DQS2DQ_CAL2_BOOT_TARG_UI_RK1_DQ1            Fld(11, 16) //[26:16]

#define DRAMC_REG_SHURK1_DQS2DQ_CAL3                       (DRAMC_AO_BASE + 0x00B48)
    #define SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ0        Fld(6, 0) //[5:0]
    #define SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ1        Fld(6, 6) //[11:6]
    #define SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ0_B4TO0  Fld(5, 12) //[16:12]
    #define SHURK1_DQS2DQ_CAL3_BOOT_TARG_UI_RK1_OEN_DQ1_B4TO0  Fld(5, 17) //[21:17]

#define DRAMC_REG_SHURK1_DQS2DQ_CAL4                       (DRAMC_AO_BASE + 0x00B4C)
    #define SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM0       Fld(6, 0) //[5:0]
    #define SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM1       Fld(6, 6) //[11:6]
    #define SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM0_B4TO0 Fld(5, 12) //[16:12]
    #define SHURK1_DQS2DQ_CAL4_BOOT_TARG_UI_RK1_OEN_DQM1_B4TO0 Fld(5, 17) //[21:17]

#define DRAMC_REG_SHURK1_DQS2DQ_CAL5                       (DRAMC_AO_BASE + 0x00B50)
    #define SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM0           Fld(11, 0) //[10:0]
    #define SHURK1_DQS2DQ_CAL5_BOOT_TARG_UI_RK1_DQM1           Fld(11, 16) //[26:16]

#define DRAMC_REG_SHU2RK0_DQSCTL (DRAMC_REG_SHURK0_DQSCTL + SHU_GRP_DRAMC_OFFSET)
#define DRAMC_REG_SHU3RK0_DQSCTL (DRAMC_REG_SHURK0_DQSCTL + (SHU_GRP_DRAMC_OFFSET << 1))
#define DRAMC_REG_SHU2RK1_DQSCTL (DRAMC_REG_SHURK1_DQSCTL + SHU_GRP_DRAMC_OFFSET)
#define DRAMC_REG_SHU3RK1_DQSCTL (DRAMC_REG_SHURK1_DQSCTL + (SHU_GRP_DRAMC_OFFSET << 1))

#endif /*__DRAMC_CH0_REG_H__*/
