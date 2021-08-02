#ifndef __DRAMC_CH0_NAO_REG_H__
#define __DRAMC_CH0_NAO_REG_H__

#define DRAMC_NAO_BASE Channel_A_DRAMC_NAO_BASE_VIRTUAL

#define DRAMC_REG_TESTMODE                             (DRAMC_NAO_BASE + 0x00000)
    #define TESTMODE_TESTM_PAT0                                Fld(8, 24) //[31:24]

#define DRAMC_REG_LBWDAT0                              (DRAMC_NAO_BASE + 0x00004)
    #define LBWDAT0_LBWDATA0                                   Fld(32, 0) //[31:0]

#define DRAMC_REG_LBWDAT1                              (DRAMC_NAO_BASE + 0x00008)
    #define LBWDAT1_LBWDATA1                                   Fld(32, 0) //[31:0]

#define DRAMC_REG_LBWDAT2                              (DRAMC_NAO_BASE + 0x0000C)
    #define LBWDAT2_LBWDATA2                                   Fld(32, 0) //[31:0]

#define DRAMC_REG_LBWDAT3                              (DRAMC_NAO_BASE + 0x00010)
    #define LBWDAT3_LBWDATA3                                   Fld(32, 0) //[31:0]

#define DRAMC_REG_CKPHCHK                              (DRAMC_NAO_BASE + 0x00020)
    #define CKPHCHK_CKPHCHKCYC                                 Fld(16, 0) //[15:0]
    #define CKPHCHK_CKPHCNTEN                                  Fld(1, 31) //[31:31]

#define DRAMC_REG_DMMONITOR                            (DRAMC_NAO_BASE + 0x00024)
    #define DMMONITOR_JMTR_EN                                  Fld(1, 0) //[0:0]
    #define DMMONITOR_MONPAUSE_SW                              Fld(1, 2) //[2:2]
    #define DMMONITOR_BUSMONEN_SW                              Fld(1, 3) //[3:3]
    #define DMMONITOR_WDQ_MON_OPT                              Fld(1, 4) //[4:4]
    #define DMMONITOR_REQQUE_MON_SREF_DIS                      Fld(1, 8) //[8:8]
    #define DMMONITOR_REQQUE_MON_SREF_REOR                     Fld(1, 9) //[9:9]
    #define DMMONITOR_REQQUE_MON_SREF_LLAT                     Fld(1, 10) //[10:10]
    #define DMMONITOR_REQQUE_MON_SREF_HPRI                     Fld(1, 11) //[11:11]
    #define DMMONITOR_REQQUE_MON_SREF_RW                       Fld(1, 12) //[12:12]
    #define DMMONITOR_JMTRCNT                                  Fld(16, 16) //[31:16]

#define DRAMC_REG_TESTCHIP_DMA1                        (DRAMC_NAO_BASE + 0x00030)
    #define TESTCHIP_DMA1_DMAEN                                Fld(1, 0) //[0:0]
    #define TESTCHIP_DMA1_DMAPUREWREN                          Fld(1, 1) //[1:1]
    #define TESTCHIP_DMA1_DMAPURERDEN                          Fld(1, 2) //[2:2]
    #define TESTCHIP_DMA1_DMA_MWR                              Fld(1, 3) //[3:3]
    #define TESTCHIP_DMA1_DMABURSTLEN                          Fld(3, 4) //[6:4]
    #define TESTCHIP_DMA1_DMAEN_LOOP                           Fld(1, 8) //[8:8]
    #define TESTCHIP_DMA1_DMAFIXPAT                            Fld(1, 9) //[9:9]
    #define TESTCHIP_DMA1_DMA_LP4MATAB_OPT                     Fld(1, 12) //[12:12]

#define DRAMC_REG_MISC_STATUSA                         (DRAMC_NAO_BASE + 0x00080)
    #define MISC_STATUSA_WAIT_DLE                              Fld(1, 0) //[0:0]
    #define MISC_STATUSA_WRITE_DATA_BUFFER_EMPTY               Fld(1, 1) //[1:1]
    #define MISC_STATUSA_REQQ_EMPTY                            Fld(1, 2) //[2:2]
    #define MISC_STATUSA_PG_VLD                                Fld(1, 3) //[3:3]
    #define MISC_STATUSA_REQQUE_DEPTH                          Fld(4, 4) //[7:4]
    #define MISC_STATUSA_REFRESH_RATE                          Fld(3, 8) //[10:8]
    #define MISC_STATUSA_DRAM_HWCFG                            Fld(1, 12) //[12:12]
    #define MISC_STATUSA_CKEO_PRE                              Fld(1, 13) //[13:13]
    #define MISC_STATUSA_CKE1O_PRE                             Fld(1, 14) //[14:14]
    #define MISC_STATUSA_SREF_STATE                            Fld(1, 16) //[16:16]
    #define MISC_STATUSA_SELFREF_SM                            Fld(3, 17) //[19:17]
    #define MISC_STATUSA_REFRESH_OVER_CNT                      Fld(4, 20) //[23:20]
    #define MISC_STATUSA_REFRESH_QUEUE_CNT                     Fld(4, 24) //[27:24]
    #define MISC_STATUSA_REQDEPTH_UPD_DONE                     Fld(1, 28) //[28:28]
    #define MISC_STATUSA_MANUTXUPD_DONE                        Fld(1, 29) //[29:29]
    #define MISC_STATUSA_DRAMC_IDLE_STATUS                     Fld(1, 30) //[30:30]
    #define MISC_STATUSA_DRAMC_IDLE_DCM                        Fld(1, 31) //[31:31]

#define DRAMC_REG_SPECIAL_STATUS                       (DRAMC_NAO_BASE + 0x00084)
    #define SPECIAL_STATUS_SPECIAL_COMMAND_ENABLE              Fld(1, 0) //[0:0]
    #define SPECIAL_STATUS_H_ZQLAT_REQ                         Fld(1, 1) //[1:1]
    #define SPECIAL_STATUS_H_ZQLCAL_REQ                        Fld(1, 2) //[2:2]
    #define SPECIAL_STATUS_TX_RETRY_PERIOD                     Fld(1, 3) //[3:3]
    #define SPECIAL_STATUS_H_DQSOSCEN_REQ                      Fld(1, 4) //[4:4]
    #define SPECIAL_STATUS_DQSOSCEN_PERIOD                     Fld(1, 5) //[5:5]
    #define SPECIAL_STATUS_H_ZQCS_REQ                          Fld(1, 6) //[6:6]
    #define SPECIAL_STATUS_H_REFR_REQ                          Fld(1, 7) //[7:7]
    #define SPECIAL_STATUS_STBUPD_STOP                         Fld(1, 8) //[8:8]
    #define SPECIAL_STATUS_HW_ZQLAT_REQ                        Fld(1, 9) //[9:9]
    #define SPECIAL_STATUS_HW_ZQCAL_REQ                        Fld(1, 10) //[10:10]
    #define SPECIAL_STATUS_SPECIAL_STATUS                      Fld(1, 11) //[11:11]
    #define SPECIAL_STATUS_SCSM                                Fld(5, 12) //[16:12]
    #define SPECIAL_STATUS_XSR_TX_RETRY_SM                     Fld(3, 17) //[19:17]
    #define SPECIAL_STATUS_SCARB_SM                            Fld(5, 20) //[24:20]
    #define SPECIAL_STATUS_TX_RETRY_PERIOD_WO_RX_RETRY         Fld(1, 25) //[25:25]
    #define SPECIAL_STATUS_SC_DRAMC_QUEUE_ACK                  Fld(1, 28) //[28:28]
    #define SPECIAL_STATUS_SREF_REQ_2Q                         Fld(1, 30) //[30:30]
    #define SPECIAL_STATUS_SREF_REQ                            Fld(1, 31) //[31:31]

#define DRAMC_REG_SPCMDRESP                            (DRAMC_NAO_BASE + 0x00088)
    #define SPCMDRESP_MRW_RESPONSE                             Fld(1, 0) //[0:0]
    #define SPCMDRESP_MRR_RESPONSE                             Fld(1, 1) //[1:1]
    #define SPCMDRESP_PREA_RESPONSE                            Fld(1, 2) //[2:2]
    #define SPCMDRESP_AREF_RESPONSE                            Fld(1, 3) //[3:3]
    #define SPCMDRESP_ZQC_RESPONSE                             Fld(1, 4) //[4:4]
    #define SPCMDRESP_TCMD_RESPONSE                            Fld(1, 5) //[5:5]
    #define SPCMDRESP_ZQLAT_RESPONSE                           Fld(1, 6) //[6:6]
    #define SPCMDRESP_RDDQC_RESPONSE                           Fld(1, 7) //[7:7]
    #define SPCMDRESP_STEST_RESPONSE                           Fld(1, 8) //[8:8]
    #define SPCMDRESP_MPCMAN_RESPONSE                          Fld(1, 9) //[9:9]
    #define SPCMDRESP_DQSOSCEN_RESPONSE                        Fld(1, 10) //[10:10]
    #define SPCMDRESP_DQSOSCDIS_RESPONSE                       Fld(1, 11) //[11:11]
    #define SPCMDRESP_ACT_RESPONSE                             Fld(1, 12) //[12:12]
    #define SPCMDRESP_MPRW_RESPONSE                            Fld(1, 13) //[13:13]
    #define SPCMDRESP_TX_RETRY_DONE_RESPONSE                   Fld(1, 15) //[15:15]
    #define SPCMDRESP_DVFS_RESPONSE                            Fld(1, 16) //[16:16]
    #define SPCMDRESP_HW_ZQLAT_POP                             Fld(1, 17) //[17:17]
    #define SPCMDRESP_HW_ZQCAL_POP                             Fld(1, 18) //[18:18]
    #define SPCMDRESP_RDFIFO_RESPONSE                          Fld(1, 30) //[30:30]
    #define SPCMDRESP_WRFIFO_RESPONSE                          Fld(1, 31) //[31:31]

#define DRAMC_REG_MRR_STATUS                           (DRAMC_NAO_BASE + 0x0008C)
    #define MRR_STATUS_MRR_REG                                 Fld(16, 0) //[15:0]
    #define MRR_STATUS_MRR_SW_REG                              Fld(16, 16) //[31:16]

#define DRAMC_REG_MRR_STATUS2                          (DRAMC_NAO_BASE + 0x00090)
    #define MRR_STATUS2_MR4_REG                                Fld(16, 0) //[15:0]
    #define MRR_STATUS2_SHUFFLE_MRW_VRCG_NORMAL_OK             Fld(1, 16) //[16:16]
    #define MRR_STATUS2_TFC_OK                                 Fld(1, 17) //[17:17]
    #define MRR_STATUS2_TCKFSPX_OK                             Fld(1, 18) //[18:18]
    #define MRR_STATUS2_TVRCG_EN_OK                            Fld(1, 19) //[19:19]
    #define MRR_STATUS2_TCKFSPE_OK                             Fld(1, 20) //[20:20]
    #define MRR_STATUS2_TVRCG_DIS_OK                           Fld(1, 21) //[21:21]
    #define MRR_STATUS2_PHY_SHUFFLE_PERIOD_GO_ZERO_OK          Fld(1, 22) //[22:22]
    #define MRR_STATUS2_DVFS_STATE                             Fld(8, 24) //[31:24]

#define DRAMC_REG_MRRDATA0                             (DRAMC_NAO_BASE + 0x00094)
    #define MRRDATA0_MRR_DATA0                                 Fld(32, 0) //[31:0]

#define DRAMC_REG_MRRDATA1                             (DRAMC_NAO_BASE + 0x00098)
    #define MRRDATA1_MRR_DATA1                                 Fld(32, 0) //[31:0]

#define DRAMC_REG_MRRDATA2                             (DRAMC_NAO_BASE + 0x0009C)
    #define MRRDATA2_MRR_DATA2                                 Fld(32, 0) //[31:0]

#define DRAMC_REG_MRRDATA3                             (DRAMC_NAO_BASE + 0x000A0)
    #define MRRDATA3_MRR_DATA3                                 Fld(32, 0) //[31:0]

#define DRAMC_REG_DRS_STATUS                           (DRAMC_NAO_BASE + 0x000A8)
    #define DRS_STATUS_DRS_MONERR_ACK                          Fld(1, 8) //[8:8]
    #define DRS_STATUS_DRS_MONERR_REQ                          Fld(1, 9) //[9:9]
    #define DRS_STATUS_RK1_DRS_REQ                             Fld(1, 16) //[16:16]
    #define DRS_STATUS_RK1_DRS_2Q                              Fld(1, 17) //[17:17]
    #define DRS_STATUS_RK1_DRSLY_REQ                           Fld(1, 18) //[18:18]
    #define DRS_STATUS_RK1_DRS_RDY                             Fld(1, 19) //[19:19]
    #define DRS_STATUS_RK1_DRS_ACK                             Fld(1, 20) //[20:20]
    #define DRS_STATUS_RK1_DRS_STATUS                          Fld(1, 21) //[21:21]
    #define DRS_STATUS_SELFREF_SM_RK1                          Fld(3, 24) //[26:24]

#define DRAMC_REG_JMETER_ST                            (DRAMC_NAO_BASE + 0x000BC)
    #define JMETER_ST_ZEROS_CNT                                Fld(15, 0) //[14:0]
    #define JMETER_ST_ONES_CNT                                 Fld(15, 16) //[30:16]
    #define JMETER_ST_JMTR_DONE                                Fld(1, 31) //[31:31]

#define DRAMC_REG_TCMDO1LAT                            (DRAMC_NAO_BASE + 0x000C0)
    #define TCMDO1LAT_TCMD_O1_LATCH_DATA0                      Fld(6, 0) //[5:0]
    #define TCMDO1LAT_TCMD_O1_LATCH_DATA1                      Fld(6, 8) //[13:8]
    #define TCMDO1LAT_CATRAIN_CMP_ERR0                         Fld(6, 16) //[21:16]
    #define TCMDO1LAT_CATRAIN_CMP_ERR                          Fld(6, 24) //[29:24]
    #define TCMDO1LAT_CATRAIN_CMP_CPT                          Fld(1, 31) //[31:31]

#define DRAMC_REG_RDQC_CMP                             (DRAMC_NAO_BASE + 0x000C4)
    #define RDQC_CMP_RDDQC_CMP0_ERR                            Fld(16, 0) //[15:0]
    #define RDQC_CMP_RDDQC_CMP1_ERR                            Fld(16, 16) //[31:16]

#define DRAMC_REG_CKPHCHK_STATUS                       (DRAMC_NAO_BASE + 0x000C8)
    #define CKPHCHK_STATUS_CKPHCHK_STATUS                      Fld(16, 0) //[15:0]

#define DRAMC_REG_HWMRR_PUSH2POP_CNT                   (DRAMC_NAO_BASE + 0x0010C)
    #define HWMRR_PUSH2POP_CNT_HWMRR_PUSH2POP_CNT              Fld(32, 0) //[31:0]

#define DRAMC_REG_HWMRR_STATUS                         (DRAMC_NAO_BASE + 0x00110)
    #define HWMRR_STATUS_OV_P2P_CNT                            Fld(8, 0) //[7:0]
    #define HWMRR_STATUS_MRR_CNT_UNDER_FULL                    Fld(1, 30) //[30:30]
    #define HWMRR_STATUS_MRR_CNT_OVER_FULL                     Fld(1, 31) //[31:31]

#define DRAMC_REG_HW_REFRATE_MON                       (DRAMC_NAO_BASE + 0x00114)
    #define HW_REFRATE_MON_REFRESH_RATE_MIN_MON                Fld(3, 0) //[2:0]
    #define HW_REFRATE_MON_REFRESH_RATE_MAX_MON                Fld(3, 4) //[6:4]
    #define HW_REFRATE_MON_REFRESH_RATE_RK0                    Fld(3, 8) //[10:8]
    #define HW_REFRATE_MON_REFRESH_RATE_RK1                    Fld(3, 12) //[14:12]

#define DRAMC_REG_TESTRPT                              (DRAMC_NAO_BASE + 0x00120)
    #define TESTRPT_DM_CMP_CPT_RK0                             Fld(1, 0) //[0:0]
    #define TESTRPT_DM_CMP_CPT_RK1                             Fld(1, 1) //[1:1]
    #define TESTRPT_DM_CMP_ERR_RK0                             Fld(1, 4) //[4:4]
    #define TESTRPT_DM_CMP_ERR_RK1                             Fld(1, 5) //[5:5]
    #define TESTRPT_DLE_CNT_OK_RK0                             Fld(1, 8) //[8:8]
    #define TESTRPT_DLE_CNT_OK_RK1                             Fld(1, 9) //[9:9]
    #define TESTRPT_TESTSTAT                                   Fld(3, 20) //[22:20]
    #define TESTRPT_LB_CMP_FAIL                                Fld(1, 24) //[24:24]
    #define TESTRPT_CALI_DONE_MON                              Fld(1, 28) //[28:28]

#define DRAMC_REG_CMP_ERR                              (DRAMC_NAO_BASE + 0x00124)
    #define CMP_ERR_CMP_ERR                                    Fld(32, 0) //[31:0]

#define DRAMC_REG_TEST_ABIT_STATUS1                    (DRAMC_NAO_BASE + 0x00128)
    #define TEST_ABIT_STATUS1_TEST_ABIT_ERR1                   Fld(32, 0) //[31:0]

#define DRAMC_REG_TEST_ABIT_STATUS2                    (DRAMC_NAO_BASE + 0x0012C)
    #define TEST_ABIT_STATUS2_TEST_ABIT_ERR2                   Fld(32, 0) //[31:0]

#define DRAMC_REG_TEST_ABIT_STATUS3                    (DRAMC_NAO_BASE + 0x00130)
    #define TEST_ABIT_STATUS3_TEST_ABIT_ERR3                   Fld(32, 0) //[31:0]

#define DRAMC_REG_TEST_ABIT_STATUS4                    (DRAMC_NAO_BASE + 0x00134)
    #define TEST_ABIT_STATUS4_TEST_ABIT_ERR4                   Fld(32, 0) //[31:0]

#define DRAMC_REG_DQSDLY0                              (DRAMC_NAO_BASE + 0x00150)
    #define DQSDLY0_DEL0DLY                                    Fld(7, 0) //[6:0]
    #define DQSDLY0_DEL1DLY                                    Fld(7, 8) //[14:8]
    #define DQSDLY0_DEL2DLY                                    Fld(7, 16) //[22:16]
    #define DQSDLY0_DEL3DLY                                    Fld(7, 24) //[30:24]

#define DRAMC_REG_DQ_CAL_MAX_0                         (DRAMC_NAO_BASE + 0x00154)
    #define DQ_CAL_MAX_0_DQ0_0_DLY_MAX                         Fld(8, 0) //[7:0]
    #define DQ_CAL_MAX_0_DQ0_1_DLY_MAX                         Fld(8, 8) //[15:8]
    #define DQ_CAL_MAX_0_DQ0_2_DLY_MAX                         Fld(8, 16) //[23:16]
    #define DQ_CAL_MAX_0_DQ0_3_DLY_MAX                         Fld(8, 24) //[31:24]

#define DRAMC_REG_DQ_CAL_MAX_1                         (DRAMC_NAO_BASE + 0x00158)
    #define DQ_CAL_MAX_1_DQ0_4_DLY_MAX                         Fld(8, 0) //[7:0]
    #define DQ_CAL_MAX_1_DQ0_5_DLY_MAX                         Fld(8, 8) //[15:8]
    #define DQ_CAL_MAX_1_DQ0_6_DLY_MAX                         Fld(8, 16) //[23:16]
    #define DQ_CAL_MAX_1_DQ0_7_DLY_MAX                         Fld(8, 24) //[31:24]

#define DRAMC_REG_DQ_CAL_MAX_2                         (DRAMC_NAO_BASE + 0x0015C)
    #define DQ_CAL_MAX_2_DQ1_0_DLY_MAX                         Fld(8, 0) //[7:0]
    #define DQ_CAL_MAX_2_DQ1_1_DLY_MAX                         Fld(8, 8) //[15:8]
    #define DQ_CAL_MAX_2_DQ1_2_DLY_MAX                         Fld(8, 16) //[23:16]
    #define DQ_CAL_MAX_2_DQ1_3_DLY_MAX                         Fld(8, 24) //[31:24]

#define DRAMC_REG_DQ_CAL_MAX_3                         (DRAMC_NAO_BASE + 0x00160)
    #define DQ_CAL_MAX_3_DQ1_4_DLY_MAX                         Fld(8, 0) //[7:0]
    #define DQ_CAL_MAX_3_DQ1_5_DLY_MAX                         Fld(8, 8) //[15:8]
    #define DQ_CAL_MAX_3_DQ1_6_DLY_MAX                         Fld(8, 16) //[23:16]
    #define DQ_CAL_MAX_3_DQ1_7_DLY_MAX                         Fld(8, 24) //[31:24]

#define DRAMC_REG_DQ_CAL_MAX_4                         (DRAMC_NAO_BASE + 0x00164)
    #define DQ_CAL_MAX_4_DQ2_0_DLY_MAX                         Fld(8, 0) //[7:0]
    #define DQ_CAL_MAX_4_DQ2_1_DLY_MAX                         Fld(8, 8) //[15:8]
    #define DQ_CAL_MAX_4_DQ2_2_DLY_MAX                         Fld(8, 16) //[23:16]
    #define DQ_CAL_MAX_4_DQ2_3_DLY_MAX                         Fld(8, 24) //[31:24]

#define DRAMC_REG_DQ_CAL_MAX_5                         (DRAMC_NAO_BASE + 0x00168)
    #define DQ_CAL_MAX_5_DQ2_4_DLY_MAX                         Fld(8, 0) //[7:0]
    #define DQ_CAL_MAX_5_DQ2_5_DLY_MAX                         Fld(8, 8) //[15:8]
    #define DQ_CAL_MAX_5_DQ2_6_DLY_MAX                         Fld(8, 16) //[23:16]
    #define DQ_CAL_MAX_5_DQ2_7_DLY_MAX                         Fld(8, 24) //[31:24]

#define DRAMC_REG_DQ_CAL_MAX_6                         (DRAMC_NAO_BASE + 0x0016C)
    #define DQ_CAL_MAX_6_DQ3_0_DLY_MAX                         Fld(8, 0) //[7:0]
    #define DQ_CAL_MAX_6_DQ3_1_DLY_MAX                         Fld(8, 8) //[15:8]
    #define DQ_CAL_MAX_6_DQ3_2_DLY_MAX                         Fld(8, 16) //[23:16]
    #define DQ_CAL_MAX_6_DQ3_3_DLY_MAX                         Fld(8, 24) //[31:24]

#define DRAMC_REG_DQ_CAL_MAX_7                         (DRAMC_NAO_BASE + 0x00170)
    #define DQ_CAL_MAX_7_DQ3_4_DLY_MAX                         Fld(8, 0) //[7:0]
    #define DQ_CAL_MAX_7_DQ3_5_DLY_MAX                         Fld(8, 8) //[15:8]
    #define DQ_CAL_MAX_7_DQ3_6_DLY_MAX                         Fld(8, 16) //[23:16]
    #define DQ_CAL_MAX_7_DQ3_7_DLY_MAX                         Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MIN_0                        (DRAMC_NAO_BASE + 0x00174)
    #define DQS_CAL_MIN_0_DQS0_0_DLY_MIN                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MIN_0_DQS0_1_DLY_MIN                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MIN_0_DQS0_2_DLY_MIN                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MIN_0_DQS0_3_DLY_MIN                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MIN_1                        (DRAMC_NAO_BASE + 0x00178)
    #define DQS_CAL_MIN_1_DQS0_4_DLY_MIN                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MIN_1_DQS0_5_DLY_MIN                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MIN_1_DQS0_6_DLY_MIN                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MIN_1_DQS0_7_DLY_MIN                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MIN_2                        (DRAMC_NAO_BASE + 0x0017C)
    #define DQS_CAL_MIN_2_DQS1_0_DLY_MIN                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MIN_2_DQS1_1_DLY_MIN                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MIN_2_DQS1_2_DLY_MIN                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MIN_2_DQS1_3_DLY_MIN                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MIN_3                        (DRAMC_NAO_BASE + 0x00180)
    #define DQS_CAL_MIN_3_DQS1_4_DLY_MIN                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MIN_3_DQS1_5_DLY_MIN                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MIN_3_DQS1_6_DLY_MIN                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MIN_3_DQS1_7_DLY_MIN                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MIN_4                        (DRAMC_NAO_BASE + 0x00184)
    #define DQS_CAL_MIN_4_DQS2_0_DLY_MIN                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MIN_4_DQS2_1_DLY_MIN                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MIN_4_DQS2_2_DLY_MIN                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MIN_4_DQS2_3_DLY_MIN                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MIN_5                        (DRAMC_NAO_BASE + 0x00188)
    #define DQS_CAL_MIN_5_DQS2_4_DLY_MIN                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MIN_5_DQS2_5_DLY_MIN                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MIN_5_DQS2_6_DLY_MIN                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MIN_5_DQS2_7_DLY_MIN                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MIN_6                        (DRAMC_NAO_BASE + 0x0018C)
    #define DQS_CAL_MIN_6_DQS3_0_DLY_MIN                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MIN_6_DQS3_1_DLY_MIN                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MIN_6_DQS3_2_DLY_MIN                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MIN_6_DQS3_3_DLY_MIN                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MIN_7                        (DRAMC_NAO_BASE + 0x00190)
    #define DQS_CAL_MIN_7_DQS3_4_DLY_MIN                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MIN_7_DQS3_5_DLY_MIN                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MIN_7_DQS3_6_DLY_MIN                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MIN_7_DQS3_7_DLY_MIN                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MAX_0                        (DRAMC_NAO_BASE + 0x00194)
    #define DQS_CAL_MAX_0_DQS0_0_DLY_MAX                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MAX_0_DQS0_1_DLY_MAX                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MAX_0_DQS0_2_DLY_MAX                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MAX_0_DQS0_3_DLY_MAX                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MAX_1                        (DRAMC_NAO_BASE + 0x00198)
    #define DQS_CAL_MAX_1_DQS0_4_DLY_MAX                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MAX_1_DQS0_5_DLY_MAX                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MAX_1_DQS0_6_DLY_MAX                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MAX_1_DQS0_7_DLY_MAX                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MAX_2                        (DRAMC_NAO_BASE + 0x0019C)
    #define DQS_CAL_MAX_2_DQS1_0_DLY_MAX                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MAX_2_DQS1_1_DLY_MAX                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MAX_2_DQS1_2_DLY_MAX                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MAX_2_DQS1_3_DLY_MAX                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MAX_3                        (DRAMC_NAO_BASE + 0x001A0)
    #define DQS_CAL_MAX_3_DQS1_4_DLY_MAX                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MAX_3_DQS1_5_DLY_MAX                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MAX_3_DQS1_6_DLY_MAX                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MAX_3_DQS1_7_DLY_MAX                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MAX_4                        (DRAMC_NAO_BASE + 0x001A4)
    #define DQS_CAL_MAX_4_DQS2_0_DLY_MAX                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MAX_4_DQS2_1_DLY_MAX                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MAX_4_DQS2_2_DLY_MAX                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MAX_4_DQS2_3_DLY_MAX                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MAX_5                        (DRAMC_NAO_BASE + 0x001A8)
    #define DQS_CAL_MAX_5_DQS2_4_DLY_MAX                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MAX_5_DQS2_5_DLY_MAX                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MAX_5_DQS2_6_DLY_MAX                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MAX_5_DQS2_7_DLY_MAX                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MAX_6                        (DRAMC_NAO_BASE + 0x001AC)
    #define DQS_CAL_MAX_6_DQS3_0_DLY_MAX                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MAX_6_DQS3_1_DLY_MAX                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MAX_6_DQS3_2_DLY_MAX                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MAX_6_DQS3_3_DLY_MAX                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQS_CAL_MAX_7                        (DRAMC_NAO_BASE + 0x001B0)
    #define DQS_CAL_MAX_7_DQS3_4_DLY_MAX                       Fld(8, 0) //[7:0]
    #define DQS_CAL_MAX_7_DQS3_5_DLY_MAX                       Fld(8, 8) //[15:8]
    #define DQS_CAL_MAX_7_DQS3_6_DLY_MAX                       Fld(8, 16) //[23:16]
    #define DQS_CAL_MAX_7_DQS3_7_DLY_MAX                       Fld(8, 24) //[31:24]

#define DRAMC_REG_DQICAL0                              (DRAMC_NAO_BASE + 0x001B4)
    #define DQICAL0_DQ0_DLY_MAX                                Fld(7, 0) //[6:0]
    #define DQICAL0_DQ1_DLY_MAX                                Fld(7, 8) //[14:8]
    #define DQICAL0_DQ2_DLY_MAX                                Fld(7, 16) //[22:16]
    #define DQICAL0_DQ3_DLY_MAX                                Fld(7, 24) //[30:24]

#define DRAMC_REG_DQICAL1                              (DRAMC_NAO_BASE + 0x001B8)
    #define DQICAL1_DQS0_DLY_MIN                               Fld(7, 0) //[6:0]
    #define DQICAL1_DQS1_DLY_MIN                               Fld(7, 8) //[14:8]
    #define DQICAL1_DQS2_DLY_MIN                               Fld(7, 16) //[22:16]
    #define DQICAL1_DQS3_DLY_MIN                               Fld(7, 24) //[30:24]

#define DRAMC_REG_DQICAL2                              (DRAMC_NAO_BASE + 0x001BC)
    #define DQICAL2_DQS0_DLY_MAX                               Fld(7, 0) //[6:0]
    #define DQICAL2_DQS1_DLY_MAX                               Fld(7, 8) //[14:8]
    #define DQICAL2_DQS2_DLY_MAX                               Fld(7, 16) //[22:16]
    #define DQICAL2_DQS3_DLY_MAX                               Fld(7, 24) //[30:24]

#define DRAMC_REG_DQICAL3                              (DRAMC_NAO_BASE + 0x001C0)
    #define DQICAL3_DQS0_DLY_AVG                               Fld(7, 0) //[6:0]
    #define DQICAL3_DQS1_DLY_AVG                               Fld(7, 8) //[14:8]
    #define DQICAL3_DQS2_DLY_AVG                               Fld(7, 16) //[22:16]
    #define DQICAL3_DQS3_DLY_AVG                               Fld(7, 24) //[30:24]

#define DRAMC_REG_TESTCHIP_DMA_STATUS1                 (DRAMC_NAO_BASE + 0x00200)
    #define TESTCHIP_DMA_STATUS1_DMASTATUS                     Fld(1, 0) //[0:0]
    #define TESTCHIP_DMA_STATUS1_DMA_BUF_AVAIL                 Fld(1, 2) //[2:2]
    #define TESTCHIP_DMA_STATUS1_DMACMPERR                     Fld(1, 3) //[3:3]
    #define TESTCHIP_DMA_STATUS1_DMA_STATE                     Fld(4, 4) //[7:4]

#define DRAMC_REG_TESTCHIP_DMA_STATUS2                 (DRAMC_NAO_BASE + 0x00204)
    #define TESTCHIP_DMA_STATUS2_DMACMPERR_BIT                 Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS3                 (DRAMC_NAO_BASE + 0x00208)
    #define TESTCHIP_DMA_STATUS3_DMA_DATA_BUFFER0_31_0_        Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS4                 (DRAMC_NAO_BASE + 0x0020C)
    #define TESTCHIP_DMA_STATUS4_DMA_DATA_BUFFER0_63_32_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS5                 (DRAMC_NAO_BASE + 0x00210)
    #define TESTCHIP_DMA_STATUS5_DMA_DATA_BUFFER0_95_64_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS6                 (DRAMC_NAO_BASE + 0x00214)
    #define TESTCHIP_DMA_STATUS6_DMA_DATA_BUFFER0_127_96_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS7                 (DRAMC_NAO_BASE + 0x00218)
    #define TESTCHIP_DMA_STATUS7_DMA_DATA_BUFFER1_31_0_        Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS8                 (DRAMC_NAO_BASE + 0x0021C)
    #define TESTCHIP_DMA_STATUS8_DMA_DATA_BUFFER1_63_32_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS9                 (DRAMC_NAO_BASE + 0x00220)
    #define TESTCHIP_DMA_STATUS9_DMA_DATA_BUFFER1_95_64_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS10                (DRAMC_NAO_BASE + 0x00224)
    #define TESTCHIP_DMA_STATUS10_DMA_DATA_BUFFE1_127_96_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS11                (DRAMC_NAO_BASE + 0x00228)
    #define TESTCHIP_DMA_STATUS11_DMA_DATA_BUFFER2_31_0_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS12                (DRAMC_NAO_BASE + 0x0022C)
    #define TESTCHIP_DMA_STATUS12_DMA_DATA_BUFFER2_63_32_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS13                (DRAMC_NAO_BASE + 0x00230)
    #define TESTCHIP_DMA_STATUS13_DMA_DATA_BUFFER2_95_64_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS14                (DRAMC_NAO_BASE + 0x00234)
    #define TESTCHIP_DMA_STATUS14_DMA_DATA_BUFFER2_127_96_     Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS15                (DRAMC_NAO_BASE + 0x00238)
    #define TESTCHIP_DMA_STATUS15_DMA_DATA_BUFFER3_31_0_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS16                (DRAMC_NAO_BASE + 0x0023C)
    #define TESTCHIP_DMA_STATUS16_DMA_DATA_BUFFER3_63_32_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS17                (DRAMC_NAO_BASE + 0x00240)
    #define TESTCHIP_DMA_STATUS17_DMA_DATA_BUFFER3_95_64_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS18                (DRAMC_NAO_BASE + 0x00244)
    #define TESTCHIP_DMA_STATUS18_DMA_DATA_BUFFER3_127_96_     Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS19                (DRAMC_NAO_BASE + 0x00248)
    #define TESTCHIP_DMA_STATUS19_DMA_DATA_BUFFER4_31_0_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS20                (DRAMC_NAO_BASE + 0x0024C)
    #define TESTCHIP_DMA_STATUS20_DMA_DATA_BUFFER4_63_32_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS21                (DRAMC_NAO_BASE + 0x00250)
    #define TESTCHIP_DMA_STATUS21_DMA_DATA_BUFFER4_95_64_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS22                (DRAMC_NAO_BASE + 0x00254)
    #define TESTCHIP_DMA_STATUS22_DMA_DATA_BUFFER4_127_96_     Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS23                (DRAMC_NAO_BASE + 0x00258)
    #define TESTCHIP_DMA_STATUS23_DMA_DATA_BUFFER5_31_0_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS24                (DRAMC_NAO_BASE + 0x0025C)
    #define TESTCHIP_DMA_STATUS24_DMA_DATA_BUFFER5_63_32_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS25                (DRAMC_NAO_BASE + 0x00260)
    #define TESTCHIP_DMA_STATUS25_DMA_DATA_BUFFER5_95_64_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS26                (DRAMC_NAO_BASE + 0x00264)
    #define TESTCHIP_DMA_STATUS26_DMA_DATA_BUFFER5_127_96_     Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS27                (DRAMC_NAO_BASE + 0x00268)
    #define TESTCHIP_DMA_STATUS27_DMA_DATA_BUFFER6_31_0_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS28                (DRAMC_NAO_BASE + 0x0026C)
    #define TESTCHIP_DMA_STATUS28_DMA_DATA_BUFFER6_63_32_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS29                (DRAMC_NAO_BASE + 0x00270)
    #define TESTCHIP_DMA_STATUS29_DMA_DATA_BUFFER6_95_64_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS30                (DRAMC_NAO_BASE + 0x00274)
    #define TESTCHIP_DMA_STATUS30_DMA_DATA_BUFFER6_127_96_     Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS31                (DRAMC_NAO_BASE + 0x00278)
    #define TESTCHIP_DMA_STATUS31_DMA_DATA_BUFFER7_31_0_       Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS32                (DRAMC_NAO_BASE + 0x0027C)
    #define TESTCHIP_DMA_STATUS32_DMA_DATA_BUFFER7_63_32_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS33                (DRAMC_NAO_BASE + 0x00280)
    #define TESTCHIP_DMA_STATUS33_DMA_DATA_BUFFER7_95_64_      Fld(32, 0) //[31:0]

#define DRAMC_REG_TESTCHIP_DMA_STATUS34                (DRAMC_NAO_BASE + 0x00284)
    #define TESTCHIP_DMA_STATUS34_DMA_DATA_BUFFER7_127_96_     Fld(32, 0) //[31:0]

#define DRAMC_REG_REFRESH_POP_COUNTER                  (DRAMC_NAO_BASE + 0x00300)
    #define REFRESH_POP_COUNTER_REFRESH_POP_COUNTER            Fld(32, 0) //[31:0]

#define DRAMC_REG_FREERUN_26M_COUNTER                  (DRAMC_NAO_BASE + 0x00304)
    #define FREERUN_26M_COUNTER_FREERUN_26M_COUNTER            Fld(32, 0) //[31:0]

#define DRAMC_REG_DRAMC_IDLE_COUNTER                   (DRAMC_NAO_BASE + 0x00308)
    #define DRAMC_IDLE_COUNTER_DRAMC_IDLE_COUNTER              Fld(32, 0) //[31:0]

#define DRAMC_REG_R2R_PAGE_HIT_COUNTER                 (DRAMC_NAO_BASE + 0x0030C)
    #define R2R_PAGE_HIT_COUNTER_R2R_PAGE_HIT_COUNTER          Fld(32, 0) //[31:0]

#define DRAMC_REG_R2R_PAGE_MISS_COUNTER                (DRAMC_NAO_BASE + 0x00310)
    #define R2R_PAGE_MISS_COUNTER_R2R_PAGE_MISS_COUNTER        Fld(32, 0) //[31:0]

#define DRAMC_REG_R2R_INTERBANK_COUNTER                (DRAMC_NAO_BASE + 0x00314)
    #define R2R_INTERBANK_COUNTER_R2R_INTERBANK_COUNTER        Fld(32, 0) //[31:0]

#define DRAMC_REG_R2W_PAGE_HIT_COUNTER                 (DRAMC_NAO_BASE + 0x00318)
    #define R2W_PAGE_HIT_COUNTER_R2W_PAGE_HIT_COUNTER          Fld(32, 0) //[31:0]

#define DRAMC_REG_R2W_PAGE_MISS_COUNTER                (DRAMC_NAO_BASE + 0x0031C)
    #define R2W_PAGE_MISS_COUNTER_R2W_PAGE_MISS_COUNTER        Fld(32, 0) //[31:0]

#define DRAMC_REG_R2W_INTERBANK_COUNTER                (DRAMC_NAO_BASE + 0x00320)
    #define R2W_INTERBANK_COUNTER_R2W_INTERBANK_COUNTER        Fld(32, 0) //[31:0]

#define DRAMC_REG_W2R_PAGE_HIT_COUNTER                 (DRAMC_NAO_BASE + 0x00324)
    #define W2R_PAGE_HIT_COUNTER_W2R_PAGE_HIT_COUNTER          Fld(32, 0) //[31:0]

#define DRAMC_REG_W2R_PAGE_MISS_COUNTER                (DRAMC_NAO_BASE + 0x00328)
    #define W2R_PAGE_MISS_COUNTER_W2R_PAGE_MISS_COUNTER        Fld(32, 0) //[31:0]

#define DRAMC_REG_W2R_INTERBANK_COUNTER                (DRAMC_NAO_BASE + 0x0032C)
    #define W2R_INTERBANK_COUNTER_W2R_INTERBANK_COUNTER        Fld(32, 0) //[31:0]

#define DRAMC_REG_W2W_PAGE_HIT_COUNTER                 (DRAMC_NAO_BASE + 0x00330)
    #define W2W_PAGE_HIT_COUNTER_W2W_PAGE_HIT_COUNTER          Fld(32, 0) //[31:0]

#define DRAMC_REG_W2W_PAGE_MISS_COUNTER                (DRAMC_NAO_BASE + 0x00334)
    #define W2W_PAGE_MISS_COUNTER_W2W_PAGE_MISS_COUNTER        Fld(32, 0) //[31:0]

#define DRAMC_REG_W2W_INTERBANK_COUNTER                (DRAMC_NAO_BASE + 0x00338)
    #define W2W_INTERBANK_COUNTER_W2W_INTERBANK_COUNTER        Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_PRE_STANDBY_COUNTER              (DRAMC_NAO_BASE + 0x0033C)
    #define RK0_PRE_STANDBY_COUNTER_RK0_PRE_STANDBY_COUNTER    Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_PRE_POWERDOWN_COUNTER            (DRAMC_NAO_BASE + 0x00340)
    #define RK0_PRE_POWERDOWN_COUNTER_RK0_PRE_POWERDOWN_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_ACT_STANDBY_COUNTER              (DRAMC_NAO_BASE + 0x00344)
    #define RK0_ACT_STANDBY_COUNTER_RK0_ACT_STANDBY_COUNTER    Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_ACT_POWERDOWN_COUNTER            (DRAMC_NAO_BASE + 0x00348)
    #define RK0_ACT_POWERDOWN_COUNTER_RK0_ACT_POWERDOWN_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_PRE_STANDBY_COUNTER              (DRAMC_NAO_BASE + 0x0034C)
    #define RK1_PRE_STANDBY_COUNTER_RK1_PRE_STANDBY_COUNTER    Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_PRE_POWERDOWN_COUNTER            (DRAMC_NAO_BASE + 0x00350)
    #define RK1_PRE_POWERDOWN_COUNTER_RK1_PRE_POWERDOWN_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_ACT_STANDBY_COUNTER              (DRAMC_NAO_BASE + 0x00354)
    #define RK1_ACT_STANDBY_COUNTER_RK1_ACT_STANDBY_COUNTER    Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_ACT_POWERDOWN_COUNTER            (DRAMC_NAO_BASE + 0x00358)
    #define RK1_ACT_POWERDOWN_COUNTER_RK1_ACT_POWERDOWN_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_PRE_STANDBY_COUNTER              (DRAMC_NAO_BASE + 0x0035C)
    #define RK2_PRE_STANDBY_COUNTER_RK2_PRE_STANDBY_COUNTER    Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_PRE_POWERDOWN_COUNTER            (DRAMC_NAO_BASE + 0x00360)
    #define RK2_PRE_POWERDOWN_COUNTER_RK2_PRE_POWERDOWN_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_ACT_STANDBY_COUNTER              (DRAMC_NAO_BASE + 0x00364)
    #define RK2_ACT_STANDBY_COUNTER_RK2_ACT_STANDBY_COUNTER    Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_ACT_POWERDOWN_COUNTER            (DRAMC_NAO_BASE + 0x00368)
    #define RK2_ACT_POWERDOWN_COUNTER_RK2_ACT_POWERDOWN_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ0_TOGGLE_COUNTER                   (DRAMC_NAO_BASE + 0x0036C)
    #define DQ0_TOGGLE_COUNTER_DQ0_TOGGLE_COUNTER              Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ1_TOGGLE_COUNTER                   (DRAMC_NAO_BASE + 0x00370)
    #define DQ1_TOGGLE_COUNTER_DQ1_TOGGLE_COUNTER              Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ2_TOGGLE_COUNTER                   (DRAMC_NAO_BASE + 0x00374)
    #define DQ2_TOGGLE_COUNTER_DQ2_TOGGLE_COUNTER              Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ3_TOGGLE_COUNTER                   (DRAMC_NAO_BASE + 0x00378)
    #define DQ3_TOGGLE_COUNTER_DQ3_TOGGLE_COUNTER              Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ0_TOGGLE_COUNTER_R                 (DRAMC_NAO_BASE + 0x0037C)
    #define DQ0_TOGGLE_COUNTER_R_DQ0_TOGGLE_COUNTER_R          Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ1_TOGGLE_COUNTER_R                 (DRAMC_NAO_BASE + 0x00380)
    #define DQ1_TOGGLE_COUNTER_R_DQ1_TOGGLE_COUNTER_R          Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ2_TOGGLE_COUNTER_R                 (DRAMC_NAO_BASE + 0x00384)
    #define DQ2_TOGGLE_COUNTER_R_DQ2_TOGGLE_COUNTER_R          Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ3_TOGGLE_COUNTER_R                 (DRAMC_NAO_BASE + 0x00388)
    #define DQ3_TOGGLE_COUNTER_R_DQ3_TOGGLE_COUNTER_R          Fld(32, 0) //[31:0]

#define DRAMC_REG_READ_BYTES_COUNTER                   (DRAMC_NAO_BASE + 0x0038C)
    #define READ_BYTES_COUNTER_READ_BYTES_COUNTER              Fld(32, 0) //[31:0]

#define DRAMC_REG_WRITE_BYTES_COUNTER                  (DRAMC_NAO_BASE + 0x00390)
    #define WRITE_BYTES_COUNTER_WRITE_BYTES_COUNTER            Fld(32, 0) //[31:0]

#define DRAMC_REG_MAX_SREF_REQ_TO_ACK_LATENCY_COUNTER  (DRAMC_NAO_BASE + 0x00394)
    #define MAX_SREF_REQ_TO_ACK_LATENCY_COUNTER_SREF_REQTOACK_MAX_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_MAX_RK1_DRS_LONG_REQ_TO_ACK_LATENCY_COUNTER (DRAMC_NAO_BASE + 0x00398)
    #define MAX_RK1_DRS_LONG_REQ_TO_ACK_LATENCY_COUNTER_DRS_LONG_REQTOACK_MAX_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_MAX_RK1_DRS_REQ_TO_ACK_LATENCY_COUNTER (DRAMC_NAO_BASE + 0x0039C)
    #define MAX_RK1_DRS_REQ_TO_ACK_LATENCY_COUNTER_DRS_REQTOACK_MAX_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_DRAMC_IDLE_DCM_COUNTER               (DRAMC_NAO_BASE + 0x003A0)
    #define DRAMC_IDLE_DCM_COUNTER_DRAMC_IDLE_DCM_COUNTER      Fld(32, 0) //[31:0]

#define DRAMC_REG_DDRPHY_CLK_EN_COUNTER                (DRAMC_NAO_BASE + 0x003A4)
    #define DDRPHY_CLK_EN_COUNTER_DDRPHY_CLK_EN_COUNTER        Fld(32, 0) //[31:0]

#define DRAMC_REG_DDRPHY_CLK_EN_COMB_COUNTER           (DRAMC_NAO_BASE + 0x003A8)
    #define DDRPHY_CLK_EN_COMB_COUNTER_DDRPHY_CLK_EN_COMB_COUNTER Fld(32, 0) //[31:0]

#define DRAMC_REG_LAT_COUNTER_CMD0                     (DRAMC_NAO_BASE + 0x003C0)
    #define LAT_COUNTER_CMD0_LAT_CMD0_CNT_MAX                  Fld(16, 0) //[15:0]
    #define LAT_COUNTER_CMD0_LAT_CMD0_CNT_MAX_HPRI             Fld(1, 16) //[16:16]
    #define LAT_COUNTER_CMD0_LAT_CMD0_CNT_MAX_LLAT             Fld(1, 17) //[17:17]
    #define LAT_COUNTER_CMD0_LAT_CMD0_CNT_MAX_REORDER          Fld(1, 18) //[18:18]

#define DRAMC_REG_LAT_COUNTER_CMD1                     (DRAMC_NAO_BASE + 0x003C4)
    #define LAT_COUNTER_CMD1_LAT_CMD1_CNT_MAX                  Fld(16, 0) //[15:0]
    #define LAT_COUNTER_CMD1_LAT_CMD1_CNT_MAX_HPRI             Fld(1, 16) //[16:16]
    #define LAT_COUNTER_CMD1_LAT_CMD1_CNT_MAX_LLAT             Fld(1, 17) //[17:17]
    #define LAT_COUNTER_CMD1_LAT_CMD1_CNT_MAX_REORDER          Fld(1, 18) //[18:18]

#define DRAMC_REG_LAT_COUNTER_CMD2                     (DRAMC_NAO_BASE + 0x003C8)
    #define LAT_COUNTER_CMD2_LAT_CMD2_CNT_MAX                  Fld(16, 0) //[15:0]
    #define LAT_COUNTER_CMD2_LAT_CMD2_CNT_MAX_HPRI             Fld(1, 16) //[16:16]
    #define LAT_COUNTER_CMD2_LAT_CMD2_CNT_MAX_LLAT             Fld(1, 17) //[17:17]
    #define LAT_COUNTER_CMD2_LAT_CMD2_CNT_MAX_REORDER          Fld(1, 18) //[18:18]

#define DRAMC_REG_LAT_COUNTER_CMD3                     (DRAMC_NAO_BASE + 0x003CC)
    #define LAT_COUNTER_CMD3_LAT_CMD3_CNT_MAX                  Fld(16, 0) //[15:0]
    #define LAT_COUNTER_CMD3_LAT_CMD3_CNT_MAX_HPRI             Fld(1, 16) //[16:16]
    #define LAT_COUNTER_CMD3_LAT_CMD3_CNT_MAX_LLAT             Fld(1, 17) //[17:17]
    #define LAT_COUNTER_CMD3_LAT_CMD3_CNT_MAX_REORDER          Fld(1, 18) //[18:18]

#define DRAMC_REG_LAT_COUNTER_CMD4                     (DRAMC_NAO_BASE + 0x003D0)
    #define LAT_COUNTER_CMD4_LAT_CMD4_CNT_MAX                  Fld(16, 0) //[15:0]
    #define LAT_COUNTER_CMD4_LAT_CMD4_CNT_MAX_HPRI             Fld(1, 16) //[16:16]
    #define LAT_COUNTER_CMD4_LAT_CMD4_CNT_MAX_LLAT             Fld(1, 17) //[17:17]
    #define LAT_COUNTER_CMD4_LAT_CMD4_CNT_MAX_REORDER          Fld(1, 18) //[18:18]

#define DRAMC_REG_LAT_COUNTER_CMD5                     (DRAMC_NAO_BASE + 0x003D4)
    #define LAT_COUNTER_CMD5_LAT_CMD5_CNT_MAX                  Fld(16, 0) //[15:0]
    #define LAT_COUNTER_CMD5_LAT_CMD5_CNT_MAX_HPRI             Fld(1, 16) //[16:16]
    #define LAT_COUNTER_CMD5_LAT_CMD5_CNT_MAX_LLAT             Fld(1, 17) //[17:17]
    #define LAT_COUNTER_CMD5_LAT_CMD5_CNT_MAX_REORDER          Fld(1, 18) //[18:18]

#define DRAMC_REG_LAT_COUNTER_CMD6                     (DRAMC_NAO_BASE + 0x003D8)
    #define LAT_COUNTER_CMD6_LAT_CMD6_CNT_MAX                  Fld(16, 0) //[15:0]
    #define LAT_COUNTER_CMD6_LAT_CMD6_CNT_MAX_HPRI             Fld(1, 16) //[16:16]
    #define LAT_COUNTER_CMD6_LAT_CMD6_CNT_MAX_LLAT             Fld(1, 17) //[17:17]
    #define LAT_COUNTER_CMD6_LAT_CMD6_CNT_MAX_REORDER          Fld(1, 18) //[18:18]

#define DRAMC_REG_LAT_COUNTER_CMD7                     (DRAMC_NAO_BASE + 0x003DC)
    #define LAT_COUNTER_CMD7_LAT_CMD7_CNT_MAX                  Fld(16, 0) //[15:0]
    #define LAT_COUNTER_CMD7_LAT_CMD7_CNT_MAX_HPRI             Fld(1, 16) //[16:16]
    #define LAT_COUNTER_CMD7_LAT_CMD7_CNT_MAX_LLAT             Fld(1, 17) //[17:17]
    #define LAT_COUNTER_CMD7_LAT_CMD7_CNT_MAX_REORDER          Fld(1, 18) //[18:18]

#define DRAMC_REG_LAT_COUNTER_AVER                     (DRAMC_NAO_BASE + 0x003E0)
    #define LAT_COUNTER_AVER_LAT_CMD_AVER_CNT                  Fld(32, 0) //[31:0]

#define DRAMC_REG_LAT_COUNTER_NUM                      (DRAMC_NAO_BASE + 0x003E4)
    #define LAT_COUNTER_NUM_LAT_CMD_NUM                        Fld(16, 0) //[15:0]

#define DRAMC_REG_LAT_COUNTER_BLOCK_ALE                (DRAMC_NAO_BASE + 0x003E8)
    #define LAT_COUNTER_BLOCK_ALE_CTO_BLOCK_CNT_MAX            Fld(16, 0) //[15:0]

#define DRAMC_REG_DQSSAMPLEV                           (DRAMC_NAO_BASE + 0x00400)
    #define DQSSAMPLEV_SAMPLE_OUT1_DQS0                        Fld(1, 0) //[0:0]
    #define DQSSAMPLEV_SAMPLE_OUT1_DQS1                        Fld(1, 1) //[1:1]
    #define DQSSAMPLEV_SAMPLE_OUT1_DQS2                        Fld(1, 2) //[2:2]
    #define DQSSAMPLEV_SAMPLE_OUT1_DQS3                        Fld(1, 3) //[3:3]
    #define DQSSAMPLEV_PI_OVERFLOW                             Fld(4, 12) //[15:12]
    #define DQSSAMPLEV_STB_CNT_SHU_ST_ERR_FLAG_RK0             Fld(4, 16) //[19:16]
    #define DQSSAMPLEV_DQSIEN_PICG_HEAD_ERR_RK0                Fld(4, 20) //[23:20]
    #define DQSSAMPLEV_STB_CNT_SHU_ST_ERR_FLAG_RK1             Fld(4, 24) //[27:24]
    #define DQSSAMPLEV_DQSIEN_PICG_HEAD_ERR_RK1                Fld(4, 28) //[31:28]

#define DRAMC_REG_DQSGNWCNT0                           (DRAMC_NAO_BASE + 0x00408)
    #define DQSGNWCNT0_DQS0F_GATING_COUNTER                    Fld(8, 0) //[7:0]
    #define DQSGNWCNT0_DQS0R_GATING_COUNTER                    Fld(8, 8) //[15:8]
    #define DQSGNWCNT0_DQS1F_GATING_COUNTER                    Fld(8, 16) //[23:16]
    #define DQSGNWCNT0_DQS1R_GATING_COUNTER                    Fld(8, 24) //[31:24]

#define DRAMC_REG_DQSGNWCNT1                           (DRAMC_NAO_BASE + 0x0040C)
    #define DQSGNWCNT1_DQS2F_GATING_COUNTER                    Fld(8, 0) //[7:0]
    #define DQSGNWCNT1_DQS2R_GATING_COUNTER                    Fld(8, 8) //[15:8]
    #define DQSGNWCNT1_DQS3F_GATING_COUNTER                    Fld(8, 16) //[23:16]
    #define DQSGNWCNT1_DQS3R_GATING_COUNTER                    Fld(8, 24) //[31:24]

#define DRAMC_REG_DQSGNWCNT2                           (DRAMC_NAO_BASE + 0x00410)
    #define DQSGNWCNT2_DQS0F_POS_GATING_COUNTER                Fld(8, 0) //[7:0]
    #define DQSGNWCNT2_DQS0R_POS_GATING_COUNTER                Fld(8, 8) //[15:8]
    #define DQSGNWCNT2_DQS0F_PRE_GATING_COUNTER                Fld(8, 16) //[23:16]
    #define DQSGNWCNT2_DQS0R_PRE_GATING_COUNTER                Fld(8, 24) //[31:24]

#define DRAMC_REG_DQSGNWCNT3                           (DRAMC_NAO_BASE + 0x00414)
    #define DQSGNWCNT3_DQS1F_POS_GATING_COUNTER                Fld(8, 0) //[7:0]
    #define DQSGNWCNT3_DQS1R_POS_GATING_COUNTER                Fld(8, 8) //[15:8]
    #define DQSGNWCNT3_DQS1F_PRE_GATING_COUNTER                Fld(8, 16) //[23:16]
    #define DQSGNWCNT3_DQS1R_PRE_GATING_COUNTER                Fld(8, 24) //[31:24]

#define DRAMC_REG_DQSGNWCNT4                           (DRAMC_NAO_BASE + 0x00418)
    #define DQSGNWCNT4_DQS2F_POS_GATING_COUNTER                Fld(8, 0) //[7:0]
    #define DQSGNWCNT4_DQS2R_POS_GATING_COUNTER                Fld(8, 8) //[15:8]
    #define DQSGNWCNT4_DQS2F_PRE_GATING_COUNTER                Fld(8, 16) //[23:16]
    #define DQSGNWCNT4_DQS2R_PRE_GATING_COUNTER                Fld(8, 24) //[31:24]

#define DRAMC_REG_DQSGNWCNT5                           (DRAMC_NAO_BASE + 0x0041C)
    #define DQSGNWCNT5_DQS3F_POS_GATING_COUNTER                Fld(8, 0) //[7:0]
    #define DQSGNWCNT5_DQS3R_POS_GATING_COUNTER                Fld(8, 8) //[15:8]
    #define DQSGNWCNT5_DQS3F_PRE_GATING_COUNTER                Fld(8, 16) //[23:16]
    #define DQSGNWCNT5_DQS3R_PRE_GATING_COUNTER                Fld(8, 24) //[31:24]

#define DRAMC_REG_TOGGLE_CNT                           (DRAMC_NAO_BASE + 0x00420)
    #define TOGGLE_CNT_TOGGLE_CNT                              Fld(32, 0) //[31:0]

#define DRAMC_REG_DQS0_ERR_CNT                         (DRAMC_NAO_BASE + 0x00424)
    #define DQS0_ERR_CNT_DQS0_ERR_CNT                          Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ_ERR_CNT0                          (DRAMC_NAO_BASE + 0x00428)
    #define DQ_ERR_CNT0_DQ_ERR_CNT0                            Fld(32, 0) //[31:0]

#define DRAMC_REG_DQS1_ERR_CNT                         (DRAMC_NAO_BASE + 0x0042C)
    #define DQS1_ERR_CNT_DQS1_ERR_CNT                          Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ_ERR_CNT1                          (DRAMC_NAO_BASE + 0x00430)
    #define DQ_ERR_CNT1_DQ_ERR_CNT1                            Fld(32, 0) //[31:0]

#define DRAMC_REG_DQS2_ERR_CNT                         (DRAMC_NAO_BASE + 0x00434)
    #define DQS2_ERR_CNT_DQS2_ERR_CNT                          Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ_ERR_CNT2                          (DRAMC_NAO_BASE + 0x00438)
    #define DQ_ERR_CNT2_DQ_ERR_CNT2                            Fld(32, 0) //[31:0]

#define DRAMC_REG_DQS3_ERR_CNT                         (DRAMC_NAO_BASE + 0x0043C)
    #define DQS3_ERR_CNT_DQS3_ERR_CNT                          Fld(32, 0) //[31:0]

#define DRAMC_REG_DQ_ERR_CNT3                          (DRAMC_NAO_BASE + 0x00440)
    #define DQ_ERR_CNT3_DQ_ERR_CNT3                            Fld(32, 0) //[31:0]

#define DRAMC_REG_DEBUG_APHY_RX_CTL                    (DRAMC_NAO_BASE + 0x00444)
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_LAT_B0_RK0_USABLE  Fld(1, 0) //[0:0]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_LAT_B1_RK0_USABLE  Fld(1, 1) //[1:1]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_LAT_B2_RK0_USABLE  Fld(1, 2) //[2:2]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_LAT_B3_RK0_USABLE  Fld(1, 3) //[3:3]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_LAT_B0_RK1_USABLE  Fld(1, 4) //[4:4]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_LAT_B1_RK1_USABLE  Fld(1, 5) //[5:5]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_LAT_B2_RK1_USABLE  Fld(1, 6) //[6:6]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_LAT_B3_RK1_USABLE  Fld(1, 7) //[7:7]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_PRE_LAT_B0_RK0_USABLE Fld(1, 8) //[8:8]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_PRE_LAT_B1_RK0_USABLE Fld(1, 9) //[9:9]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_PRE_LAT_B2_RK0_USABLE Fld(1, 10) //[10:10]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_PRE_LAT_B3_RK0_USABLE Fld(1, 11) //[11:11]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_PRE_LAT_B0_RK1_USABLE Fld(1, 12) //[12:12]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_PRE_LAT_B1_RK1_USABLE Fld(1, 13) //[13:13]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_PRE_LAT_B2_RK1_USABLE Fld(1, 14) //[14:14]
    #define DEBUG_APHY_RX_CTL_RX_IN_GATE_EN_PRE_LAT_B3_RK1_USABLE Fld(1, 15) //[15:15]
    #define DEBUG_APHY_RX_CTL_RX_RANK_SEL_RXDLY_UPDLAT_B0_RK0_USABLE Fld(1, 16) //[16:16]
    #define DEBUG_APHY_RX_CTL_RX_RANK_SEL_RXDLY_UPDLAT_B1_RK0_USABLE Fld(1, 17) //[17:17]
    #define DEBUG_APHY_RX_CTL_RX_RANK_SEL_RXDLY_UPDLAT_B2_RK0_USABLE Fld(1, 18) //[18:18]
    #define DEBUG_APHY_RX_CTL_RX_RANK_SEL_RXDLY_UPDLAT_B3_RK0_USABLE Fld(1, 19) //[19:19]
    #define DEBUG_APHY_RX_CTL_RX_RANK_SEL_RXDLY_UPDLAT_B0_RK1_USABLE Fld(1, 20) //[20:20]
    #define DEBUG_APHY_RX_CTL_RX_RANK_SEL_RXDLY_UPDLAT_B1_RK1_USABLE Fld(1, 21) //[21:21]
    #define DEBUG_APHY_RX_CTL_RX_RANK_SEL_RXDLY_UPDLAT_B2_RK1_USABLE Fld(1, 22) //[22:22]
    #define DEBUG_APHY_RX_CTL_RX_RANK_SEL_RXDLY_UPDLAT_B3_RK1_USABLE Fld(1, 23) //[23:23]
    #define DEBUG_APHY_RX_CTL_RX_IN_BUFF_EN_LAT_B0_RK0_USABLE  Fld(1, 24) //[24:24]
    #define DEBUG_APHY_RX_CTL_RX_IN_BUFF_EN_LAT_B1_RK0_USABLE  Fld(1, 25) //[25:25]
    #define DEBUG_APHY_RX_CTL_RX_IN_BUFF_EN_LAT_B2_RK0_USABLE  Fld(1, 26) //[26:26]
    #define DEBUG_APHY_RX_CTL_RX_IN_BUFF_EN_LAT_B3_RK0_USABLE  Fld(1, 27) //[27:27]
    #define DEBUG_APHY_RX_CTL_RX_IN_BUFF_EN_LAT_B0_RK1_USABLE  Fld(1, 28) //[28:28]
    #define DEBUG_APHY_RX_CTL_RX_IN_BUFF_EN_LAT_B1_RK1_USABLE  Fld(1, 29) //[29:29]
    #define DEBUG_APHY_RX_CTL_RX_IN_BUFF_EN_LAT_B2_RK1_USABLE  Fld(1, 30) //[30:30]
    #define DEBUG_APHY_RX_CTL_RX_IN_BUFF_EN_LAT_B3_RK1_USABLE  Fld(1, 31) //[31:31]

#define DRAMC_REG_DEBUG_RODTCTL                        (DRAMC_NAO_BASE + 0x00448)
    #define DEBUG_RODTCTL_SELPH_RODTEN_USABLE_B0_RK0           Fld(1, 0) //[0:0]
    #define DEBUG_RODTCTL_SELPH_RODTEN_USABLE_B1_RK0           Fld(1, 1) //[1:1]
    #define DEBUG_RODTCTL_SELPH_RODTEN_USABLE_B2_RK0           Fld(1, 2) //[2:2]
    #define DEBUG_RODTCTL_SELPH_RODTEN_USABLE_B3_RK0           Fld(1, 3) //[3:3]
    #define DEBUG_RODTCTL_SELPH_RODTEN_USABLE_B0_RK1           Fld(1, 4) //[4:4]
    #define DEBUG_RODTCTL_SELPH_RODTEN_USABLE_B1_RK1           Fld(1, 5) //[5:5]
    #define DEBUG_RODTCTL_SELPH_RODTEN_USABLE_B2_RK1           Fld(1, 6) //[6:6]
    #define DEBUG_RODTCTL_SELPH_RODTEN_USABLE_B3_RK1           Fld(1, 7) //[7:7]
    #define DEBUG_RODTCTL_R0DQSINCTL_OFFSET_LAT                Fld(4, 8) //[11:8]
    #define DEBUG_RODTCTL_R1DQSINCTL_OFFSET_LAT                Fld(4, 12) //[15:12]
    #define DEBUG_RODTCTL_RODTENSTBINCTL_RK0                   Fld(4, 16) //[19:16]
    #define DEBUG_RODTCTL_RODTENSTBINCTL_RK1                   Fld(4, 20) //[23:20]
    #define DEBUG_RODTCTL_RODTENCGEN_LAT_RK0                   Fld(4, 24) //[27:24]
    #define DEBUG_RODTCTL_RODTENCGEN_LAT_RK1                   Fld(4, 28) //[31:28]

#define DRAMC_REG_IORGCNT                              (DRAMC_NAO_BASE + 0x00450)
    #define IORGCNT_IO_RING_COUNTER_K                          Fld(16, 0) //[15:0]
    #define IORGCNT_IO_RING_COUNTER                            Fld(16, 16) //[31:16]

#define DRAMC_REG_DQSG_RETRY_STATE                     (DRAMC_NAO_BASE + 0x00454)
    #define DQSG_RETRY_STATE_DQSG_RETRY_1ST_ST                 Fld(8, 0) //[7:0]
    #define DQSG_RETRY_STATE_DQSG_RETRY_2ND_ST                 Fld(8, 8) //[15:8]
    #define DQSG_RETRY_STATE_DQSG_RETRY_3RD_ST                 Fld(8, 16) //[23:16]
    #define DQSG_RETRY_STATE_DQSG_RETRY_4TH_ST                 Fld(8, 24) //[31:24]

#define DRAMC_REG_DQSG_RETRY_STATE1                    (DRAMC_NAO_BASE + 0x00458)
    #define DQSG_RETRY_STATE1_RETRY_DONE_ALL                   Fld(1, 0) //[0:0]
    #define DQSG_RETRY_STATE1_TDQSCK_DONE                      Fld(1, 4) //[4:4]
    #define DQSG_RETRY_STATE1_IMPCAL_N_ERROR                   Fld(1, 8) //[8:8]
    #define DQSG_RETRY_STATE1_IMPCAL_P_ERROR                   Fld(1, 9) //[9:9]
    #define DQSG_RETRY_STATE1_STB_GATING_ERR                   Fld(1, 16) //[16:16]
    #define DQSG_RETRY_STATE1_R_OTHER_SHU_GP_GATING_ERR        Fld(2, 17) //[18:17]
    #define DQSG_RETRY_STATE1_R_MPDIV_SHU_GP_GATING_ERR        Fld(3, 19) //[21:19]
    #define DQSG_RETRY_STATE1_DQSG_RETRY_5TH_ST                Fld(8, 24) //[31:24]

#define DRAMC_REG_IMPCAL_STATUS1                       (DRAMC_NAO_BASE + 0x00460)
    #define IMPCAL_STATUS1_DRVNDQ_SAVE2                        Fld(5, 0) //[4:0]
    #define IMPCAL_STATUS1_DRVPDQ_SAVE2                        Fld(5, 5) //[9:5]
    #define IMPCAL_STATUS1_DRVNDQS_SAVE1                       Fld(5, 10) //[14:10]
    #define IMPCAL_STATUS1_DRVPDQS_SAVE1                       Fld(5, 15) //[19:15]
    #define IMPCAL_STATUS1_DRVNDQS_SAVE2                       Fld(5, 20) //[24:20]
    #define IMPCAL_STATUS1_DRVPDQS_SAVE2                       Fld(5, 25) //[29:25]

#define DRAMC_REG_IMPCAL_STATUS2                       (DRAMC_NAO_BASE + 0x00464)
    #define IMPCAL_STATUS2_DRVNCMD_SAVE1                       Fld(5, 0) //[4:0]
    #define IMPCAL_STATUS2_DRVPCMD_SAVE1                       Fld(5, 5) //[9:5]
    #define IMPCAL_STATUS2_DRVNCMD_SAVE2                       Fld(5, 10) //[14:10]
    #define IMPCAL_STATUS2_DRVPCMD_SAVE2                       Fld(5, 15) //[19:15]
    #define IMPCAL_STATUS2_DRVNDQ_SAVE1                        Fld(5, 20) //[24:20]
    #define IMPCAL_STATUS2_DRVPDQ_SAVE1                        Fld(5, 25) //[29:25]

#define DRAMC_REG_DQDRV_STATUS                         (DRAMC_NAO_BASE + 0x00468)
    #define DQDRV_STATUS_DRVNDQ_2                              Fld(5, 0) //[4:0]
    #define DQDRV_STATUS_DRVPDQ_2                              Fld(5, 5) //[9:5]
    #define DQDRV_STATUS_DRVNDQS_1                             Fld(5, 10) //[14:10]
    #define DQDRV_STATUS_DRVPDQS_1                             Fld(5, 15) //[19:15]
    #define DQDRV_STATUS_DRVNDQS_2                             Fld(5, 20) //[24:20]
    #define DQDRV_STATUS_DRVPDQS_2                             Fld(5, 25) //[29:25]

#define DRAMC_REG_CMDDRV_STATUS                        (DRAMC_NAO_BASE + 0x0046C)
    #define CMDDRV_STATUS_DRVNCMD_1                            Fld(5, 0) //[4:0]
    #define CMDDRV_STATUS_DRVPCMD_1                            Fld(5, 5) //[9:5]
    #define CMDDRV_STATUS_DRVNCMD_2                            Fld(5, 10) //[14:10]
    #define CMDDRV_STATUS_DRVPCMD_2                            Fld(5, 15) //[19:15]
    #define CMDDRV_STATUS_DRVNDQ_1                             Fld(5, 20) //[24:20]
    #define CMDDRV_STATUS_DRVPDQ_1                             Fld(5, 25) //[29:25]

#define DRAMC_REG_CMDDRV1                              (DRAMC_NAO_BASE + 0x00470)
    #define CMDDRV1_CMDDRV1                                    Fld(32, 0) //[31:0]

#define DRAMC_REG_CMDDRV2                              (DRAMC_NAO_BASE + 0x00474)
    #define CMDDRV2_CMDDRV2                                    Fld(32, 0) //[31:0]

#define DRAMC_REG_DRAMC_LOOP_BAK_ADR                   (DRAMC_NAO_BASE + 0x00504)
    #define DRAMC_LOOP_BAK_ADR_LOOP_BAK_ADR_CMP_FAIL           Fld(1, 0) //[0:0]
    #define DRAMC_LOOP_BAK_ADR_TEST_WR_BK_ADR                  Fld(3, 1) //[3:1]
    #define DRAMC_LOOP_BAK_ADR_TEST_WR_COL_ADR                 Fld(11, 4) //[14:4]
    #define DRAMC_LOOP_BAK_ADR_TEST_WR_ROW_ADR                 Fld(17, 15) //[31:15]

#define DRAMC_REG_DRAMC_LOOP_BAK_RK                    (DRAMC_NAO_BASE + 0x00508)
    #define DRAMC_LOOP_BAK_RK_TEST_WR_RK                       Fld(2, 0) //[1:0]

#define DRAMC_REG_DRAMC_LOOP_BAK_WDAT0                 (DRAMC_NAO_BASE + 0x00510)
    #define DRAMC_LOOP_BAK_WDAT0_LOOP_BACK_WDAT0               Fld(32, 0) //[31:0]

#define DRAMC_REG_DRAMC_LOOP_BAK_WDAT1                 (DRAMC_NAO_BASE + 0x00514)
    #define DRAMC_LOOP_BAK_WDAT1_LOOP_BACK_WDAT1               Fld(32, 0) //[31:0]

#define DRAMC_REG_DRAMC_LOOP_BAK_WDAT2                 (DRAMC_NAO_BASE + 0x00518)
    #define DRAMC_LOOP_BAK_WDAT2_LOOP_BACK_WDAT2               Fld(32, 0) //[31:0]

#define DRAMC_REG_DRAMC_LOOP_BAK_WDAT3                 (DRAMC_NAO_BASE + 0x0051C)
    #define DRAMC_LOOP_BAK_WDAT3_LOOP_BACK_WDAT3               Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_DQSOSC_STATUS                    (DRAMC_NAO_BASE + 0x00600)
    #define RK0_DQSOSC_STATUS_MR18_REG                         Fld(16, 0) //[15:0]
    #define RK0_DQSOSC_STATUS_MR19_REG                         Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_DQSOSC_DELTA                     (DRAMC_NAO_BASE + 0x00604)
    #define RK0_DQSOSC_DELTA_ABS_RK0_DQSOSC_DELTA              Fld(16, 0) //[15:0]
    #define RK0_DQSOSC_DELTA_SIGN_RK0_DQSOSC_DELTA             Fld(1, 16) //[16:16]
    #define RK0_DQSOSC_DELTA_DQSOCSR_RESPONSE                  Fld(1, 17) //[17:17]
    #define RK0_DQSOSC_DELTA_H_DQSOSCLSBR_REQ                  Fld(1, 18) //[18:18]
    #define RK0_DQSOSC_DELTA_DQSOSC_INT_RK0                    Fld(1, 19) //[19:19]

#define DRAMC_REG_RK0_DQSOSC_DELTA2                    (DRAMC_NAO_BASE + 0x00608)
    #define RK0_DQSOSC_DELTA2_ABS_RK0_DQSOSC_B1_DELTA          Fld(16, 0) //[15:0]
    #define RK0_DQSOSC_DELTA2_SIGN_RK0_DQSOSC_B1_DELTA         Fld(1, 16) //[16:16]

#define DRAMC_REG_RK0_CURRENT_TX_SETTING1              (DRAMC_NAO_BASE + 0x00610)
    #define RK0_CURRENT_TX_SETTING1_REG_TX_DLY_DQ0_MOD         Fld(3, 0) //[2:0]
    #define RK0_CURRENT_TX_SETTING1_REG_TX_DLY_DQ1_MOD         Fld(3, 4) //[6:4]
    #define RK0_CURRENT_TX_SETTING1_REG_TX_DLY_DQ2_MOD         Fld(3, 8) //[10:8]
    #define RK0_CURRENT_TX_SETTING1_REG_TX_DLY_DQ3_MOD         Fld(3, 12) //[14:12]
    #define RK0_CURRENT_TX_SETTING1_REG_TX_DLY_DQM0_MOD        Fld(3, 16) //[18:16]
    #define RK0_CURRENT_TX_SETTING1_REG_TX_DLY_DQM1_MOD        Fld(3, 20) //[22:20]
    #define RK0_CURRENT_TX_SETTING1_REG_TX_DLY_DQM2_MOD        Fld(3, 24) //[26:24]
    #define RK0_CURRENT_TX_SETTING1_REG_TX_DLY_DQM3_MOD        Fld(3, 28) //[30:28]

#define DRAMC_REG_RK0_CURRENT_TX_SETTING2              (DRAMC_NAO_BASE + 0x00614)
    #define RK0_CURRENT_TX_SETTING2_REG_DLY_DQ0_MOD            Fld(3, 0) //[2:0]
    #define RK0_CURRENT_TX_SETTING2_REG_DLY_DQ1_MOD            Fld(3, 4) //[6:4]
    #define RK0_CURRENT_TX_SETTING2_REG_DLY_DQ2_MOD            Fld(3, 8) //[10:8]
    #define RK0_CURRENT_TX_SETTING2_REG_DLY_DQ3_MOD            Fld(3, 12) //[14:12]
    #define RK0_CURRENT_TX_SETTING2_REG_DLY_DQM0_MOD           Fld(3, 16) //[18:16]
    #define RK0_CURRENT_TX_SETTING2_REG_DLY_DQM1_MOD           Fld(3, 20) //[22:20]
    #define RK0_CURRENT_TX_SETTING2_REG_DLY_DQM2_MOD           Fld(3, 24) //[26:24]
    #define RK0_CURRENT_TX_SETTING2_REG_DLY_DQM3_MOD           Fld(3, 28) //[30:28]

#define DRAMC_REG_RK0_CURRENT_TX_SETTING3              (DRAMC_NAO_BASE + 0x00618)
    #define RK0_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_DQ0_MOD     Fld(3, 0) //[2:0]
    #define RK0_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_DQ1_MOD     Fld(3, 4) //[6:4]
    #define RK0_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_DQ2_MOD     Fld(3, 8) //[10:8]
    #define RK0_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_DQ3_MOD     Fld(3, 12) //[14:12]
    #define RK0_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_DQM0_MOD    Fld(3, 16) //[18:16]
    #define RK0_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_DQM1_MOD    Fld(3, 20) //[22:20]
    #define RK0_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_DQM2_MOD    Fld(3, 24) //[26:24]
    #define RK0_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_DQM3_MOD    Fld(3, 28) //[30:28]

#define DRAMC_REG_RK0_CURRENT_TX_SETTING4              (DRAMC_NAO_BASE + 0x0061C)
    #define RK0_CURRENT_TX_SETTING4_REG_DLY_OEN_DQ0_MOD        Fld(3, 0) //[2:0]
    #define RK0_CURRENT_TX_SETTING4_REG_DLY_OEN_DQ1_MOD        Fld(3, 4) //[6:4]
    #define RK0_CURRENT_TX_SETTING4_REG_DLY_OEN_DQ2_MOD        Fld(3, 8) //[10:8]
    #define RK0_CURRENT_TX_SETTING4_REG_DLY_OEN_DQ3_MOD        Fld(3, 12) //[14:12]
    #define RK0_CURRENT_TX_SETTING4_REG_DLY_OEN_DQM0_MOD       Fld(3, 16) //[18:16]
    #define RK0_CURRENT_TX_SETTING4_REG_DLY_OEN_DQM1_MOD       Fld(3, 20) //[22:20]
    #define RK0_CURRENT_TX_SETTING4_REG_DLY_OEN_DQM2_MOD       Fld(3, 24) //[26:24]
    #define RK0_CURRENT_TX_SETTING4_REG_DLY_OEN_DQM3_MOD       Fld(3, 28) //[30:28]

#define DRAMC_REG_RK0_DUMMY_RD_DATA0                   (DRAMC_NAO_BASE + 0x00620)
    #define RK0_DUMMY_RD_DATA0_DUMMY_RD_RK0_DATA0              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_DUMMY_RD_DATA1                   (DRAMC_NAO_BASE + 0x00624)
    #define RK0_DUMMY_RD_DATA1_DUMMY_RD_RK0_DATA1              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_DUMMY_RD_DATA2                   (DRAMC_NAO_BASE + 0x00628)
    #define RK0_DUMMY_RD_DATA2_DUMMY_RD_RK0_DATA2              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_DUMMY_RD_DATA3                   (DRAMC_NAO_BASE + 0x0062C)
    #define RK0_DUMMY_RD_DATA3_DUMMY_RD_RK0_DATA3              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK0_B0_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x00630)
    #define RK0_B0_STB_MAX_MIN_DLY_RK0_B0_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK0_B0_STB_MAX_MIN_DLY_RK0_B0_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK0_B1_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x00634)
    #define RK0_B1_STB_MAX_MIN_DLY_RK0_B1_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK0_B1_STB_MAX_MIN_DLY_RK0_B1_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK0_B2_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x00638)
    #define RK0_B2_STB_MAX_MIN_DLY_RK0_B2_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK0_B2_STB_MAX_MIN_DLY_RK0_B2_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK0_B3_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x0063C)
    #define RK0_B3_STB_MAX_MIN_DLY_RK0_B3_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK0_B3_STB_MAX_MIN_DLY_RK0_B3_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK0_DQSIENDLY                        (DRAMC_NAO_BASE + 0x00640)
    #define RK0_DQSIENDLY_R0DQS0IENDLY                         Fld(7, 0) //[6:0]
    #define RK0_DQSIENDLY_R0DQS1IENDLY                         Fld(7, 8) //[14:8]
    #define RK0_DQSIENDLY_R0DQS2IENDLY                         Fld(7, 16) //[22:16]
    #define RK0_DQSIENDLY_R0DQS3IENDLY                         Fld(7, 24) //[30:24]

#define DRAMC_REG_RK0_DQSIENUIDLY                      (DRAMC_NAO_BASE + 0x00644)
    #define RK0_DQSIENUIDLY_R0DQS0IENUIDLY                     Fld(6, 0) //[5:0]
    #define RK0_DQSIENUIDLY_R0DQS1IENUIDLY                     Fld(6, 8) //[13:8]
    #define RK0_DQSIENUIDLY_R0DQS2IENUIDLY                     Fld(6, 16) //[21:16]
    #define RK0_DQSIENUIDLY_R0DQS3IENUIDLY                     Fld(6, 24) //[29:24]

#define DRAMC_REG_RK0_DQSIENUIDLY_P1                   (DRAMC_NAO_BASE + 0x00648)
    #define RK0_DQSIENUIDLY_P1_R0DQS0IENUIDLY_P1               Fld(6, 0) //[5:0]
    #define RK0_DQSIENUIDLY_P1_R0DQS1IENUIDLY_P1               Fld(6, 8) //[13:8]
    #define RK0_DQSIENUIDLY_P1_R0DQS2IENUIDLY_P1               Fld(6, 16) //[21:16]
    #define RK0_DQSIENUIDLY_P1_R0DQS3IENUIDLY_P1               Fld(6, 24) //[29:24]

#define DRAMC_REG_RK0_DQS_STBCALDEC_CNT1               (DRAMC_NAO_BASE + 0x00650)
    #define RK0_DQS_STBCALDEC_CNT1_RK0_DQS0_STBCALDEC_CNT      Fld(16, 0) //[15:0]
    #define RK0_DQS_STBCALDEC_CNT1_RK0_DQS1_STBCALDEC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_DQS_STBCALDEC_CNT2               (DRAMC_NAO_BASE + 0x00654)
    #define RK0_DQS_STBCALDEC_CNT2_RK0_DQS2_STBCALDEC_CNT      Fld(16, 0) //[15:0]
    #define RK0_DQS_STBCALDEC_CNT2_RK0_DQS3_STBCALDEC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_DQS_STBCALINC_CNT1               (DRAMC_NAO_BASE + 0x00658)
    #define RK0_DQS_STBCALINC_CNT1_RK0_DQS0_STBCALINC_CNT      Fld(16, 0) //[15:0]
    #define RK0_DQS_STBCALINC_CNT1_RK0_DQS1_STBCALINC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_DQS_STBCALINC_CNT2               (DRAMC_NAO_BASE + 0x0065C)
    #define RK0_DQS_STBCALINC_CNT2_RK0_DQS2_STBCALINC_CNT      Fld(16, 0) //[15:0]
    #define RK0_DQS_STBCALINC_CNT2_RK0_DQS3_STBCALINC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_PI_DQ_CAL                        (DRAMC_NAO_BASE + 0x00660)
    #define RK0_PI_DQ_CAL_RK0_ARPI_DQ_B0_CAL                   Fld(6, 0) //[5:0]
    #define RK0_PI_DQ_CAL_RK0_ARPI_DQ_B1_CAL                   Fld(6, 8) //[13:8]
    #define RK0_PI_DQ_CAL_PI_DQ_ADJ_RK0                        Fld(6, 16) //[21:16]
    #define RK0_PI_DQ_CAL_PI_DQ_ADJ_RK0_OVERFLOW               Fld(1, 22) //[22:22]
    #define RK0_PI_DQ_CAL_RK0_B0_PI_CHANGE_DBG                 Fld(1, 23) //[23:23]
    #define RK0_PI_DQ_CAL_PI_DQ_ADJ_RK0_B1                     Fld(6, 24) //[29:24]
    #define RK0_PI_DQ_CAL_PI_DQ_ADJ_RK0_B1_OVERFLOW            Fld(1, 30) //[30:30]
    #define RK0_PI_DQ_CAL_RK0_B1_PI_CHANGE_DBG                 Fld(1, 31) //[31:31]

#define DRAMC_REG_RK0_DQSG_RETRY_FLAG                  (DRAMC_NAO_BASE + 0x00664)
    #define RK0_DQSG_RETRY_FLAG_RK0_RETRY_DONE0                Fld(1, 0) //[0:0]
    #define RK0_DQSG_RETRY_FLAG_RK0_RETRY_DONE1                Fld(1, 1) //[1:1]
    #define RK0_DQSG_RETRY_FLAG_RK0_RETRY_DONE2                Fld(1, 2) //[2:2]
    #define RK0_DQSG_RETRY_FLAG_RK0_RETRY_DONE3                Fld(1, 3) //[3:3]
    #define RK0_DQSG_RETRY_FLAG_RK0_RETRY_FAIL0                Fld(1, 16) //[16:16]
    #define RK0_DQSG_RETRY_FLAG_RK0_RETRY_FAIL1                Fld(1, 17) //[17:17]
    #define RK0_DQSG_RETRY_FLAG_RK0_RETRY_FAIL2                Fld(1, 18) //[18:18]
    #define RK0_DQSG_RETRY_FLAG_RK0_RETRY_FAIL3                Fld(1, 19) //[19:19]

#define DRAMC_REG_RK0_PI_DQM_CAL                       (DRAMC_NAO_BASE + 0x00668)
    #define RK0_PI_DQM_CAL_RK0_ARPI_DQM_B0_CAL                 Fld(6, 0) //[5:0]
    #define RK0_PI_DQM_CAL_RK0_ARPI_DQM_B1_CAL                 Fld(6, 8) //[13:8]

#define DRAMC_REG_RK0_DQS0_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x00670)
    #define RK0_DQS0_STBCAL_CNT_R0_DQS0_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK0_DQS1_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x00674)
    #define RK0_DQS1_STBCAL_CNT_R0_DQS1_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK0_DQS2_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x00678)
    #define RK0_DQS2_STBCAL_CNT_R0_DQS2_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK0_DQS3_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x0067C)
    #define RK0_DQS3_STBCAL_CNT_R0_DQS3_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_00              (DRAMC_NAO_BASE + 0x00680)
    #define RK0_B01_STB_DBG_INFO_00_RK0_B0_STB_DBG_INFO_00     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_00_RK0_B1_STB_DBG_INFO_00     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_01              (DRAMC_NAO_BASE + 0x00684)
    #define RK0_B01_STB_DBG_INFO_01_RK0_B0_STB_DBG_INFO_01     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_01_RK0_B1_STB_DBG_INFO_01     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_02              (DRAMC_NAO_BASE + 0x00688)
    #define RK0_B01_STB_DBG_INFO_02_RK0_B0_STB_DBG_INFO_02     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_02_RK0_B1_STB_DBG_INFO_02     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_03              (DRAMC_NAO_BASE + 0x0068C)
    #define RK0_B01_STB_DBG_INFO_03_RK0_B0_STB_DBG_INFO_03     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_03_RK0_B1_STB_DBG_INFO_03     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_04              (DRAMC_NAO_BASE + 0x00690)
    #define RK0_B01_STB_DBG_INFO_04_RK0_B0_STB_DBG_INFO_04     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_04_RK0_B1_STB_DBG_INFO_04     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_05              (DRAMC_NAO_BASE + 0x00694)
    #define RK0_B01_STB_DBG_INFO_05_RK0_B0_STB_DBG_INFO_05     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_05_RK0_B1_STB_DBG_INFO_05     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_06              (DRAMC_NAO_BASE + 0x00698)
    #define RK0_B01_STB_DBG_INFO_06_RK0_B0_STB_DBG_INFO_06     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_06_RK0_B1_STB_DBG_INFO_06     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_07              (DRAMC_NAO_BASE + 0x0069C)
    #define RK0_B01_STB_DBG_INFO_07_RK0_B0_STB_DBG_INFO_07     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_07_RK0_B1_STB_DBG_INFO_07     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_08              (DRAMC_NAO_BASE + 0x006A0)
    #define RK0_B01_STB_DBG_INFO_08_RK0_B0_STB_DBG_INFO_08     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_08_RK0_B1_STB_DBG_INFO_08     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_09              (DRAMC_NAO_BASE + 0x006A4)
    #define RK0_B01_STB_DBG_INFO_09_RK0_B0_STB_DBG_INFO_09     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_09_RK0_B1_STB_DBG_INFO_09     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_10              (DRAMC_NAO_BASE + 0x006A8)
    #define RK0_B01_STB_DBG_INFO_10_RK0_B0_STB_DBG_INFO_10     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_10_RK0_B1_STB_DBG_INFO_10     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_11              (DRAMC_NAO_BASE + 0x006AC)
    #define RK0_B01_STB_DBG_INFO_11_RK0_B0_STB_DBG_INFO_11     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_11_RK0_B1_STB_DBG_INFO_11     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_12              (DRAMC_NAO_BASE + 0x006B0)
    #define RK0_B01_STB_DBG_INFO_12_RK0_B0_STB_DBG_INFO_12     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_12_RK0_B1_STB_DBG_INFO_12     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_13              (DRAMC_NAO_BASE + 0x006B4)
    #define RK0_B01_STB_DBG_INFO_13_RK0_B0_STB_DBG_INFO_13     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_13_RK0_B1_STB_DBG_INFO_13     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_14              (DRAMC_NAO_BASE + 0x006B8)
    #define RK0_B01_STB_DBG_INFO_14_RK0_B0_STB_DBG_INFO_14     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_14_RK0_B1_STB_DBG_INFO_14     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B01_STB_DBG_INFO_15              (DRAMC_NAO_BASE + 0x006BC)
    #define RK0_B01_STB_DBG_INFO_15_RK0_B0_STB_DBG_INFO_15     Fld(16, 0) //[15:0]
    #define RK0_B01_STB_DBG_INFO_15_RK0_B1_STB_DBG_INFO_15     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_00              (DRAMC_NAO_BASE + 0x006C0)
    #define RK0_B23_STB_DBG_INFO_00_RK0_B2_STB_DBG_INFO_00     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_00_RK0_B3_STB_DBG_INFO_00     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_01              (DRAMC_NAO_BASE + 0x006C4)
    #define RK0_B23_STB_DBG_INFO_01_RK0_B2_STB_DBG_INFO_01     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_01_RK0_B3_STB_DBG_INFO_01     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_02              (DRAMC_NAO_BASE + 0x006C8)
    #define RK0_B23_STB_DBG_INFO_02_RK0_B2_STB_DBG_INFO_02     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_02_RK0_B3_STB_DBG_INFO_02     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_03              (DRAMC_NAO_BASE + 0x006CC)
    #define RK0_B23_STB_DBG_INFO_03_RK0_B2_STB_DBG_INFO_03     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_03_RK0_B3_STB_DBG_INFO_03     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_04              (DRAMC_NAO_BASE + 0x006D0)
    #define RK0_B23_STB_DBG_INFO_04_RK0_B2_STB_DBG_INFO_04     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_04_RK0_B3_STB_DBG_INFO_04     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_05              (DRAMC_NAO_BASE + 0x006D4)
    #define RK0_B23_STB_DBG_INFO_05_RK0_B2_STB_DBG_INFO_05     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_05_RK0_B3_STB_DBG_INFO_05     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_06              (DRAMC_NAO_BASE + 0x006D8)
    #define RK0_B23_STB_DBG_INFO_06_RK0_B2_STB_DBG_INFO_06     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_06_RK0_B3_STB_DBG_INFO_06     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_07              (DRAMC_NAO_BASE + 0x006DC)
    #define RK0_B23_STB_DBG_INFO_07_RK0_B2_STB_DBG_INFO_07     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_07_RK0_B3_STB_DBG_INFO_07     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_08              (DRAMC_NAO_BASE + 0x006E0)
    #define RK0_B23_STB_DBG_INFO_08_RK0_B2_STB_DBG_INFO_08     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_08_RK0_B3_STB_DBG_INFO_08     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_09              (DRAMC_NAO_BASE + 0x006E4)
    #define RK0_B23_STB_DBG_INFO_09_RK0_B2_STB_DBG_INFO_09     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_09_RK0_B3_STB_DBG_INFO_09     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_10              (DRAMC_NAO_BASE + 0x006E8)
    #define RK0_B23_STB_DBG_INFO_10_RK0_B2_STB_DBG_INFO_10     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_10_RK0_B3_STB_DBG_INFO_10     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_11              (DRAMC_NAO_BASE + 0x006EC)
    #define RK0_B23_STB_DBG_INFO_11_RK0_B2_STB_DBG_INFO_11     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_11_RK0_B3_STB_DBG_INFO_11     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_12              (DRAMC_NAO_BASE + 0x006F0)
    #define RK0_B23_STB_DBG_INFO_12_RK0_B2_STB_DBG_INFO_12     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_12_RK0_B3_STB_DBG_INFO_12     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_13              (DRAMC_NAO_BASE + 0x006F4)
    #define RK0_B23_STB_DBG_INFO_13_RK0_B2_STB_DBG_INFO_13     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_13_RK0_B3_STB_DBG_INFO_13     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_14              (DRAMC_NAO_BASE + 0x006F8)
    #define RK0_B23_STB_DBG_INFO_14_RK0_B2_STB_DBG_INFO_14     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_14_RK0_B3_STB_DBG_INFO_14     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK0_B23_STB_DBG_INFO_15              (DRAMC_NAO_BASE + 0x006FC)
    #define RK0_B23_STB_DBG_INFO_15_RK0_B2_STB_DBG_INFO_15     Fld(16, 0) //[15:0]
    #define RK0_B23_STB_DBG_INFO_15_RK0_B3_STB_DBG_INFO_15     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_DQSOSC_STATUS                    (DRAMC_NAO_BASE + 0x00700)
    #define RK1_DQSOSC_STATUS_MR18_REG_RK1                     Fld(16, 0) //[15:0]
    #define RK1_DQSOSC_STATUS_MR19_REG_RK1                     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_DQSOSC_DELTA                     (DRAMC_NAO_BASE + 0x00704)
    #define RK1_DQSOSC_DELTA_ABS_RK1_DQSOSC_DELTA              Fld(16, 0) //[15:0]
    #define RK1_DQSOSC_DELTA_SIGN_RK1_DQSOSC_DELTA             Fld(1, 16) //[16:16]
    #define RK1_DQSOSC_DELTA_DQSOSCR_RK1_RESPONSE              Fld(1, 17) //[17:17]
    #define RK1_DQSOSC_DELTA_H_DQSOSCLSBR_RK1_REQ              Fld(1, 18) //[18:18]
    #define RK1_DQSOSC_DELTA_DQSOSC_INT_RK1                    Fld(1, 19) //[19:19]

#define DRAMC_REG_RK1_DQSOSC_DELTA2                    (DRAMC_NAO_BASE + 0x00708)
    #define RK1_DQSOSC_DELTA2_ABS_RK1_DQSOSC_B1_DELTA          Fld(16, 0) //[15:0]
    #define RK1_DQSOSC_DELTA2_SIGN_RK1_DQSOSC_B1_DELTA         Fld(1, 16) //[16:16]

#define DRAMC_REG_RK1_CURRENT_TX_SETTING1              (DRAMC_NAO_BASE + 0x00710)
    #define RK1_CURRENT_TX_SETTING1_REG_TX_DLY_R1DQ0_MOD       Fld(3, 0) //[2:0]
    #define RK1_CURRENT_TX_SETTING1_REG_TX_DLY_R1DQ1_MOD       Fld(3, 4) //[6:4]
    #define RK1_CURRENT_TX_SETTING1_REG_TX_DLY_R1DQ2_MOD       Fld(3, 8) //[10:8]
    #define RK1_CURRENT_TX_SETTING1_REG_TX_DLY_R1DQ3_MOD       Fld(3, 12) //[14:12]
    #define RK1_CURRENT_TX_SETTING1_REG_TX_DLY_R1DQM0_MOD      Fld(3, 16) //[18:16]
    #define RK1_CURRENT_TX_SETTING1_REG_TX_DLY_R1DQM1_MOD      Fld(3, 20) //[22:20]
    #define RK1_CURRENT_TX_SETTING1_REG_TX_DLY_R1DQM2_MOD      Fld(3, 24) //[26:24]
    #define RK1_CURRENT_TX_SETTING1_REG_TX_DLY_R1DQM3_MOD      Fld(3, 28) //[30:28]

#define DRAMC_REG_RK1_CURRENT_TX_SETTING2              (DRAMC_NAO_BASE + 0x00714)
    #define RK1_CURRENT_TX_SETTING2_REG_DLY_R1DQ0_MOD          Fld(3, 0) //[2:0]
    #define RK1_CURRENT_TX_SETTING2_REG_DLY_R1DQ1_MOD          Fld(3, 4) //[6:4]
    #define RK1_CURRENT_TX_SETTING2_REG_DLY_R1DQ2_MOD          Fld(3, 8) //[10:8]
    #define RK1_CURRENT_TX_SETTING2_REG_DLY_R1DQ3_MOD          Fld(3, 12) //[14:12]
    #define RK1_CURRENT_TX_SETTING2_REG_DLY_R1DQM0_MOD         Fld(3, 16) //[18:16]
    #define RK1_CURRENT_TX_SETTING2_REG_DLY_R1DQM1_MOD         Fld(3, 20) //[22:20]
    #define RK1_CURRENT_TX_SETTING2_REG_DLY_R1DQM2_MOD         Fld(3, 24) //[26:24]
    #define RK1_CURRENT_TX_SETTING2_REG_DLY_R1DQM3_MOD         Fld(3, 28) //[30:28]

#define DRAMC_REG_RK1_CURRENT_TX_SETTING3              (DRAMC_NAO_BASE + 0x00718)
    #define RK1_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R1DQ0_MOD   Fld(3, 0) //[2:0]
    #define RK1_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R1DQ1_MOD   Fld(3, 4) //[6:4]
    #define RK1_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R1DQ2_MOD   Fld(3, 8) //[10:8]
    #define RK1_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R1DQ3_MOD   Fld(3, 12) //[14:12]
    #define RK1_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R1DQM0_MOD  Fld(3, 16) //[18:16]
    #define RK1_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R1DQM1_MOD  Fld(3, 20) //[22:20]
    #define RK1_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R1DQM2_MOD  Fld(3, 24) //[26:24]
    #define RK1_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R1DQM3_MOD  Fld(3, 28) //[30:28]

#define DRAMC_REG_RK1_CURRENT_TX_SETTING4              (DRAMC_NAO_BASE + 0x0071C)
    #define RK1_CURRENT_TX_SETTING4_REG_DLY_OEN_R1DQ0_MOD      Fld(3, 0) //[2:0]
    #define RK1_CURRENT_TX_SETTING4_REG_DLY_OEN_R1DQ1_MOD      Fld(3, 4) //[6:4]
    #define RK1_CURRENT_TX_SETTING4_REG_DLY_OEN_R1DQ2_MOD      Fld(3, 8) //[10:8]
    #define RK1_CURRENT_TX_SETTING4_REG_DLY_OEN_R1DQ3_MOD      Fld(3, 12) //[14:12]
    #define RK1_CURRENT_TX_SETTING4_REG_DLY_OEN_R1DQM0_MOD     Fld(3, 16) //[18:16]
    #define RK1_CURRENT_TX_SETTING4_REG_DLY_OEN_R1DQM1_MOD     Fld(3, 20) //[22:20]
    #define RK1_CURRENT_TX_SETTING4_REG_DLY_OEN_R1DQM2_MOD     Fld(3, 24) //[26:24]
    #define RK1_CURRENT_TX_SETTING4_REG_DLY_OEN_R1DQM3_MOD     Fld(3, 28) //[30:28]

#define DRAMC_REG_RK1_DUMMY_RD_DATA0                   (DRAMC_NAO_BASE + 0x00720)
    #define RK1_DUMMY_RD_DATA0_DUMMY_RD_RK1_DATA0              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_DUMMY_RD_DATA1                   (DRAMC_NAO_BASE + 0x00724)
    #define RK1_DUMMY_RD_DATA1_DUMMY_RD_RK1_DATA1              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_DUMMY_RD_DATA2                   (DRAMC_NAO_BASE + 0x00728)
    #define RK1_DUMMY_RD_DATA2_DUMMY_RD_RK1_DATA2              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_DUMMY_RD_DATA3                   (DRAMC_NAO_BASE + 0x0072C)
    #define RK1_DUMMY_RD_DATA3_DUMMY_RD_RK1_DATA3              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK1_B0_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x00730)
    #define RK1_B0_STB_MAX_MIN_DLY_RK1_B0_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK1_B0_STB_MAX_MIN_DLY_RK1_B0_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK1_B1_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x00734)
    #define RK1_B1_STB_MAX_MIN_DLY_RK1_B1_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK1_B1_STB_MAX_MIN_DLY_RK1_B1_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK1_B2_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x00738)
    #define RK1_B2_STB_MAX_MIN_DLY_RK1_B2_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK1_B2_STB_MAX_MIN_DLY_RK1_B2_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK1_B3_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x0073C)
    #define RK1_B3_STB_MAX_MIN_DLY_RK1_B3_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK1_B3_STB_MAX_MIN_DLY_RK1_B3_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK1_DQSIENDLY                        (DRAMC_NAO_BASE + 0x00740)
    #define RK1_DQSIENDLY_R1DQS0IENDLY                         Fld(7, 0) //[6:0]
    #define RK1_DQSIENDLY_R1DQS1IENDLY                         Fld(7, 8) //[14:8]
    #define RK1_DQSIENDLY_R1DQS2IENDLY                         Fld(7, 16) //[22:16]
    #define RK1_DQSIENDLY_R1DQS3IENDLY                         Fld(7, 24) //[30:24]

#define DRAMC_REG_RK1_DQSIENUIDLY                      (DRAMC_NAO_BASE + 0x00744)
    #define RK1_DQSIENUIDLY_R1DQS0IENUIDLY                     Fld(6, 0) //[5:0]
    #define RK1_DQSIENUIDLY_R1DQS1IENUIDLY                     Fld(6, 8) //[13:8]
    #define RK1_DQSIENUIDLY_R1DQS2IENUIDLY                     Fld(6, 16) //[21:16]
    #define RK1_DQSIENUIDLY_R1DQS3IENUIDLY                     Fld(6, 24) //[29:24]

#define DRAMC_REG_RK1_DQSIENUIDLY_P1                   (DRAMC_NAO_BASE + 0x00748)
    #define RK1_DQSIENUIDLY_P1_R1DQS0IENUIDLY_P1               Fld(6, 0) //[5:0]
    #define RK1_DQSIENUIDLY_P1_R1DQS1IENUIDLY_P1               Fld(6, 8) //[13:8]
    #define RK1_DQSIENUIDLY_P1_R1DQS2IENUIDLY_P1               Fld(6, 16) //[21:16]
    #define RK1_DQSIENUIDLY_P1_R1DQS3IENUIDLY_P1               Fld(6, 24) //[29:24]

#define DRAMC_REG_RK1_DQS_STBCALDEC_CNT1               (DRAMC_NAO_BASE + 0x00750)
    #define RK1_DQS_STBCALDEC_CNT1_RK1_DQS0_STBCALDEC_CNT      Fld(16, 0) //[15:0]
    #define RK1_DQS_STBCALDEC_CNT1_RK1_DQS1_STBCALDEC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_DQS_STBCALDEC_CNT2               (DRAMC_NAO_BASE + 0x00754)
    #define RK1_DQS_STBCALDEC_CNT2_RK1_DQS2_STBCALDEC_CNT      Fld(16, 0) //[15:0]
    #define RK1_DQS_STBCALDEC_CNT2_RK1_DQS3_STBCALDEC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_DQS_STBCALINC_CNT1               (DRAMC_NAO_BASE + 0x00758)
    #define RK1_DQS_STBCALINC_CNT1_RK1_DQS0_STBCALINC_CNT      Fld(16, 0) //[15:0]
    #define RK1_DQS_STBCALINC_CNT1_RK1_DQS1_STBCALINC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_DQS_STBCALINC_CNT2               (DRAMC_NAO_BASE + 0x0075C)
    #define RK1_DQS_STBCALINC_CNT2_RK1_DQS2_STBCALINC_CNT      Fld(16, 0) //[15:0]
    #define RK1_DQS_STBCALINC_CNT2_RK1_DQS3_STBCALINC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_PI_DQ_CAL                        (DRAMC_NAO_BASE + 0x00760)
    #define RK1_PI_DQ_CAL_RK1_ARPI_DQ_B0_CAL                   Fld(6, 0) //[5:0]
    #define RK1_PI_DQ_CAL_RK1_ARPI_DQ_B1_CAL                   Fld(6, 8) //[13:8]
    #define RK1_PI_DQ_CAL_PI_DQ_ADJ_RK1                        Fld(6, 16) //[21:16]
    #define RK1_PI_DQ_CAL_PI_DQ_ADJ_RK1_FLOW                   Fld(1, 22) //[22:22]
    #define RK1_PI_DQ_CAL_RK1_B0_PI_CHANGE_DBG                 Fld(1, 23) //[23:23]
    #define RK1_PI_DQ_CAL_PI_DQ_ADJ_RK1_B1                     Fld(6, 24) //[29:24]
    #define RK1_PI_DQ_CAL_PI_DQ_ADJ_RK1_B1_FLOW                Fld(1, 30) //[30:30]
    #define RK1_PI_DQ_CAL_RK1_B1_PI_CHANGE_DBG                 Fld(1, 31) //[31:31]

#define DRAMC_REG_RK1_DQSG_RETRY_FLAG                  (DRAMC_NAO_BASE + 0x00764)
    #define RK1_DQSG_RETRY_FLAG_RK1_RETRY_DONE0                Fld(1, 0) //[0:0]
    #define RK1_DQSG_RETRY_FLAG_RK1_RETRY_DONE1                Fld(1, 1) //[1:1]
    #define RK1_DQSG_RETRY_FLAG_RK1_RETRY_DONE2                Fld(1, 2) //[2:2]
    #define RK1_DQSG_RETRY_FLAG_RK1_RETRY_DONE3                Fld(1, 3) //[3:3]
    #define RK1_DQSG_RETRY_FLAG_RK1_RETRY_FAIL0                Fld(1, 16) //[16:16]
    #define RK1_DQSG_RETRY_FLAG_RK1_RETRY_FAIL1                Fld(1, 17) //[17:17]
    #define RK1_DQSG_RETRY_FLAG_RK1_RETRY_FAIL2                Fld(1, 18) //[18:18]
    #define RK1_DQSG_RETRY_FLAG_RK1_RETRY_FAIL3                Fld(1, 19) //[19:19]

#define DRAMC_REG_RK1_PI_DQM_CAL                       (DRAMC_NAO_BASE + 0x00768)
    #define RK1_PI_DQM_CAL_RK1_ARPI_DQM_B0_CAL                 Fld(6, 0) //[5:0]
    #define RK1_PI_DQM_CAL_RK1_ARPI_DQM_B1_CAL                 Fld(6, 8) //[13:8]

#define DRAMC_REG_RK1_DQS0_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x00770)
    #define RK1_DQS0_STBCAL_CNT_R1_DQS0_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK1_DQS1_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x00774)
    #define RK1_DQS1_STBCAL_CNT_R1_DQS1_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK1_DQS2_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x00778)
    #define RK1_DQS2_STBCAL_CNT_R1_DQS2_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK1_DQS3_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x0077C)
    #define RK1_DQS3_STBCAL_CNT_R1_DQS3_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_00              (DRAMC_NAO_BASE + 0x00780)
    #define RK1_B01_STB_DBG_INFO_00_RK1_B0_STB_DBG_INFO_00     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_00_RK1_B1_STB_DBG_INFO_00     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_01              (DRAMC_NAO_BASE + 0x00784)
    #define RK1_B01_STB_DBG_INFO_01_RK1_B0_STB_DBG_INFO_01     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_01_RK1_B1_STB_DBG_INFO_01     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_02              (DRAMC_NAO_BASE + 0x00788)
    #define RK1_B01_STB_DBG_INFO_02_RK1_B0_STB_DBG_INFO_02     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_02_RK1_B1_STB_DBG_INFO_02     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_03              (DRAMC_NAO_BASE + 0x0078C)
    #define RK1_B01_STB_DBG_INFO_03_RK1_B0_STB_DBG_INFO_03     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_03_RK1_B1_STB_DBG_INFO_03     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_04              (DRAMC_NAO_BASE + 0x00790)
    #define RK1_B01_STB_DBG_INFO_04_RK1_B0_STB_DBG_INFO_04     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_04_RK1_B1_STB_DBG_INFO_04     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_05              (DRAMC_NAO_BASE + 0x00794)
    #define RK1_B01_STB_DBG_INFO_05_RK1_B0_STB_DBG_INFO_05     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_05_RK1_B1_STB_DBG_INFO_05     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_06              (DRAMC_NAO_BASE + 0x00798)
    #define RK1_B01_STB_DBG_INFO_06_RK1_B0_STB_DBG_INFO_06     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_06_RK1_B1_STB_DBG_INFO_06     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_07              (DRAMC_NAO_BASE + 0x0079C)
    #define RK1_B01_STB_DBG_INFO_07_RK1_B0_STB_DBG_INFO_07     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_07_RK1_B1_STB_DBG_INFO_07     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_08              (DRAMC_NAO_BASE + 0x007A0)
    #define RK1_B01_STB_DBG_INFO_08_RK1_B0_STB_DBG_INFO_08     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_08_RK1_B1_STB_DBG_INFO_08     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_09              (DRAMC_NAO_BASE + 0x007A4)
    #define RK1_B01_STB_DBG_INFO_09_RK1_B0_STB_DBG_INFO_09     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_09_RK1_B1_STB_DBG_INFO_09     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_10              (DRAMC_NAO_BASE + 0x007A8)
    #define RK1_B01_STB_DBG_INFO_10_RK1_B0_STB_DBG_INFO_10     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_10_RK1_B1_STB_DBG_INFO_10     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_11              (DRAMC_NAO_BASE + 0x007AC)
    #define RK1_B01_STB_DBG_INFO_11_RK1_B0_STB_DBG_INFO_11     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_11_RK1_B1_STB_DBG_INFO_11     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_12              (DRAMC_NAO_BASE + 0x007B0)
    #define RK1_B01_STB_DBG_INFO_12_RK1_B0_STB_DBG_INFO_12     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_12_RK1_B1_STB_DBG_INFO_12     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_13              (DRAMC_NAO_BASE + 0x007B4)
    #define RK1_B01_STB_DBG_INFO_13_RK1_B0_STB_DBG_INFO_13     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_13_RK1_B1_STB_DBG_INFO_13     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_14              (DRAMC_NAO_BASE + 0x007B8)
    #define RK1_B01_STB_DBG_INFO_14_RK1_B0_STB_DBG_INFO_14     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_14_RK1_B1_STB_DBG_INFO_14     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B01_STB_DBG_INFO_15              (DRAMC_NAO_BASE + 0x007BC)
    #define RK1_B01_STB_DBG_INFO_15_RK1_B0_STB_DBG_INFO_15     Fld(16, 0) //[15:0]
    #define RK1_B01_STB_DBG_INFO_15_RK1_B1_STB_DBG_INFO_15     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_00              (DRAMC_NAO_BASE + 0x007C0)
    #define RK1_B23_STB_DBG_INFO_00_RK1_B2_STB_DBG_INFO_00     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_00_RK1_B3_STB_DBG_INFO_00     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_01              (DRAMC_NAO_BASE + 0x007C4)
    #define RK1_B23_STB_DBG_INFO_01_RK1_B2_STB_DBG_INFO_01     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_01_RK1_B3_STB_DBG_INFO_01     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_02              (DRAMC_NAO_BASE + 0x007C8)
    #define RK1_B23_STB_DBG_INFO_02_RK1_B2_STB_DBG_INFO_02     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_02_RK1_B3_STB_DBG_INFO_02     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_03              (DRAMC_NAO_BASE + 0x007CC)
    #define RK1_B23_STB_DBG_INFO_03_RK1_B2_STB_DBG_INFO_03     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_03_RK1_B3_STB_DBG_INFO_03     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_04              (DRAMC_NAO_BASE + 0x007D0)
    #define RK1_B23_STB_DBG_INFO_04_RK1_B2_STB_DBG_INFO_04     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_04_RK1_B3_STB_DBG_INFO_04     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_05              (DRAMC_NAO_BASE + 0x007D4)
    #define RK1_B23_STB_DBG_INFO_05_RK1_B2_STB_DBG_INFO_05     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_05_RK1_B3_STB_DBG_INFO_05     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_06              (DRAMC_NAO_BASE + 0x007D8)
    #define RK1_B23_STB_DBG_INFO_06_RK1_B2_STB_DBG_INFO_06     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_06_RK1_B3_STB_DBG_INFO_06     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_07              (DRAMC_NAO_BASE + 0x007DC)
    #define RK1_B23_STB_DBG_INFO_07_RK1_B2_STB_DBG_INFO_07     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_07_RK1_B3_STB_DBG_INFO_07     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_08              (DRAMC_NAO_BASE + 0x007E0)
    #define RK1_B23_STB_DBG_INFO_08_RK1_B2_STB_DBG_INFO_08     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_08_RK1_B3_STB_DBG_INFO_08     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_09              (DRAMC_NAO_BASE + 0x007E4)
    #define RK1_B23_STB_DBG_INFO_09_RK1_B2_STB_DBG_INFO_09     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_09_RK1_B3_STB_DBG_INFO_09     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_10              (DRAMC_NAO_BASE + 0x007E8)
    #define RK1_B23_STB_DBG_INFO_10_RK1_B2_STB_DBG_INFO_10     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_10_RK1_B3_STB_DBG_INFO_10     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_11              (DRAMC_NAO_BASE + 0x007EC)
    #define RK1_B23_STB_DBG_INFO_11_RK1_B2_STB_DBG_INFO_11     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_11_RK1_B3_STB_DBG_INFO_11     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_12              (DRAMC_NAO_BASE + 0x007F0)
    #define RK1_B23_STB_DBG_INFO_12_RK1_B2_STB_DBG_INFO_12     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_12_RK1_B3_STB_DBG_INFO_12     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_13              (DRAMC_NAO_BASE + 0x007F4)
    #define RK1_B23_STB_DBG_INFO_13_RK1_B2_STB_DBG_INFO_13     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_13_RK1_B3_STB_DBG_INFO_13     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_14              (DRAMC_NAO_BASE + 0x007F8)
    #define RK1_B23_STB_DBG_INFO_14_RK1_B2_STB_DBG_INFO_14     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_14_RK1_B3_STB_DBG_INFO_14     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK1_B23_STB_DBG_INFO_15              (DRAMC_NAO_BASE + 0x007FC)
    #define RK1_B23_STB_DBG_INFO_15_RK1_B2_STB_DBG_INFO_15     Fld(16, 0) //[15:0]
    #define RK1_B23_STB_DBG_INFO_15_RK1_B3_STB_DBG_INFO_15     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_DQSOSC_STATUS                    (DRAMC_NAO_BASE + 0x00800)
    #define RK2_DQSOSC_STATUS_MR18_REG_RK2                     Fld(16, 0) //[15:0]
    #define RK2_DQSOSC_STATUS_MR19_REG_RK2                     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_DQSOSC_DELTA                     (DRAMC_NAO_BASE + 0x00804)
    #define RK2_DQSOSC_DELTA_ABS_RK2_DQSOSC_DELTA              Fld(16, 0) //[15:0]
    #define RK2_DQSOSC_DELTA_SIGN_RK2_DQSOSC_DELTA             Fld(1, 16) //[16:16]
    #define RK2_DQSOSC_DELTA_DQSOSCR_RK2_RESPONSE              Fld(1, 17) //[17:17]
    #define RK2_DQSOSC_DELTA_H_DQSOSCLSBR_RK2_REQ              Fld(1, 18) //[18:18]
    #define RK2_DQSOSC_DELTA_DQSOSC_INT_RK2                    Fld(1, 19) //[19:19]

#define DRAMC_REG_RK2_DQSOSC_DELTA2                    (DRAMC_NAO_BASE + 0x00808)
    #define RK2_DQSOSC_DELTA2_ABS_RK2_DQSOSC_B1_DELTA          Fld(16, 0) //[15:0]
    #define RK2_DQSOSC_DELTA2_SIGN_RK2_DQSOSC_B1_DELTA         Fld(1, 16) //[16:16]

#define DRAMC_REG_RK2_CURRENT_TX_SETTING1              (DRAMC_NAO_BASE + 0x00810)
    #define RK2_CURRENT_TX_SETTING1_REG_TX_DLY_R2DQ0_MOD       Fld(3, 0) //[2:0]
    #define RK2_CURRENT_TX_SETTING1_REG_TX_DLY_R2DQ1_MOD       Fld(3, 4) //[6:4]
    #define RK2_CURRENT_TX_SETTING1_REG_TX_DLY_R2DQ2_MOD       Fld(3, 8) //[10:8]
    #define RK2_CURRENT_TX_SETTING1_REG_TX_DLY_R2DQ3_MOD       Fld(3, 12) //[14:12]
    #define RK2_CURRENT_TX_SETTING1_REG_TX_DLY_R2DQM0_MOD      Fld(3, 16) //[18:16]
    #define RK2_CURRENT_TX_SETTING1_REG_TX_DLY_R2DQM1_MOD      Fld(3, 20) //[22:20]
    #define RK2_CURRENT_TX_SETTING1_REG_TX_DLY_R2DQM2_MOD      Fld(3, 24) //[26:24]
    #define RK2_CURRENT_TX_SETTING1_REG_TX_DLY_R2DQM3_MOD      Fld(3, 28) //[30:28]

#define DRAMC_REG_RK2_CURRENT_TX_SETTING2              (DRAMC_NAO_BASE + 0x00814)
    #define RK2_CURRENT_TX_SETTING2_REG_DLY_R2DQ0_MOD          Fld(3, 0) //[2:0]
    #define RK2_CURRENT_TX_SETTING2_REG_DLY_R2DQ1_MOD          Fld(3, 4) //[6:4]
    #define RK2_CURRENT_TX_SETTING2_REG_DLY_R2DQ2_MOD          Fld(3, 8) //[10:8]
    #define RK2_CURRENT_TX_SETTING2_REG_DLY_R2DQ3_MOD          Fld(3, 12) //[14:12]
    #define RK2_CURRENT_TX_SETTING2_REG_DLY_R2DQM0_MOD         Fld(3, 16) //[18:16]
    #define RK2_CURRENT_TX_SETTING2_REG_DLY_R2DQM1_MOD         Fld(3, 20) //[22:20]
    #define RK2_CURRENT_TX_SETTING2_REG_DLY_R2DQM2_MOD         Fld(3, 24) //[26:24]
    #define RK2_CURRENT_TX_SETTING2_REG_DLY_R2DQM3_MOD         Fld(3, 28) //[30:28]

#define DRAMC_REG_RK2_CURRENT_TX_SETTING3              (DRAMC_NAO_BASE + 0x00818)
    #define RK2_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R2DQ0_MOD   Fld(3, 0) //[2:0]
    #define RK2_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R2DQ1_MOD   Fld(3, 4) //[6:4]
    #define RK2_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R2DQ2_MOD   Fld(3, 8) //[10:8]
    #define RK2_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R2DQ3_MOD   Fld(3, 12) //[14:12]
    #define RK2_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R2DQM0_MOD  Fld(3, 16) //[18:16]
    #define RK2_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R2DQM1_MOD  Fld(3, 20) //[22:20]
    #define RK2_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R2DQM2_MOD  Fld(3, 24) //[26:24]
    #define RK2_CURRENT_TX_SETTING3_REG_TX_DLY_OEN_R2DQM3_MOD  Fld(3, 28) //[30:28]

#define DRAMC_REG_RK2_CURRENT_TX_SETTING4              (DRAMC_NAO_BASE + 0x0081C)
    #define RK2_CURRENT_TX_SETTING4_REG_DLY_OEN_R2DQ0_MOD      Fld(3, 0) //[2:0]
    #define RK2_CURRENT_TX_SETTING4_REG_DLY_OEN_R2DQ1_MOD      Fld(3, 4) //[6:4]
    #define RK2_CURRENT_TX_SETTING4_REG_DLY_OEN_R2DQ2_MOD      Fld(3, 8) //[10:8]
    #define RK2_CURRENT_TX_SETTING4_REG_DLY_OEN_R2DQ3_MOD      Fld(3, 12) //[14:12]
    #define RK2_CURRENT_TX_SETTING4_REG_DLY_OEN_R2DQM0_MOD     Fld(3, 16) //[18:16]
    #define RK2_CURRENT_TX_SETTING4_REG_DLY_OEN_R2DQM1_MOD     Fld(3, 20) //[22:20]
    #define RK2_CURRENT_TX_SETTING4_REG_DLY_OEN_R2DQM2_MOD     Fld(3, 24) //[26:24]
    #define RK2_CURRENT_TX_SETTING4_REG_DLY_OEN_R2DQM3_MOD     Fld(3, 28) //[30:28]

#define DRAMC_REG_RK2_DUMMY_RD_DATA0                   (DRAMC_NAO_BASE + 0x00820)
    #define RK2_DUMMY_RD_DATA0_DUMMY_RD_RK2_DATA0              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_DUMMY_RD_DATA1                   (DRAMC_NAO_BASE + 0x00824)
    #define RK2_DUMMY_RD_DATA1_DUMMY_RD_RK2_DATA1              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_DUMMY_RD_DATA2                   (DRAMC_NAO_BASE + 0x00828)
    #define RK2_DUMMY_RD_DATA2_DUMMY_RD_RK2_DATA2              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_DUMMY_RD_DATA3                   (DRAMC_NAO_BASE + 0x0082C)
    #define RK2_DUMMY_RD_DATA3_DUMMY_RD_RK2_DATA3              Fld(32, 0) //[31:0]

#define DRAMC_REG_RK2_B0_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x00830)
    #define RK2_B0_STB_MAX_MIN_DLY_RK2_B0_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK2_B0_STB_MAX_MIN_DLY_RK2_B0_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK2_B1_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x00834)
    #define RK2_B1_STB_MAX_MIN_DLY_RK2_B1_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK2_B1_STB_MAX_MIN_DLY_RK2_B1_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK2_B2_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x00838)
    #define RK2_B2_STB_MAX_MIN_DLY_RK2_B2_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK2_B2_STB_MAX_MIN_DLY_RK2_B2_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK2_B3_STB_MAX_MIN_DLY               (DRAMC_NAO_BASE + 0x0083C)
    #define RK2_B3_STB_MAX_MIN_DLY_RK2_B3_STBEN_MIN_DLY        Fld(12, 0) //[11:0]
    #define RK2_B3_STB_MAX_MIN_DLY_RK2_B3_STBEN_MAX_DLY        Fld(12, 16) //[27:16]

#define DRAMC_REG_RK2_DQSIENDLY                        (DRAMC_NAO_BASE + 0x00840)
    #define RK2_DQSIENDLY_R2DQS0IENDLY                         Fld(7, 0) //[6:0]
    #define RK2_DQSIENDLY_R2DQS1IENDLY                         Fld(7, 8) //[14:8]
    #define RK2_DQSIENDLY_R2DQS2IENDLY                         Fld(7, 16) //[22:16]
    #define RK2_DQSIENDLY_R2DQS3IENDLY                         Fld(7, 24) //[30:24]

#define DRAMC_REG_RK2_DQSIENUIDLY                      (DRAMC_NAO_BASE + 0x00844)
    #define RK2_DQSIENUIDLY_R2DQS0IENUIDLY                     Fld(6, 0) //[5:0]
    #define RK2_DQSIENUIDLY_R2DQS1IENUIDLY                     Fld(6, 8) //[13:8]
    #define RK2_DQSIENUIDLY_R2DQS2IENUIDLY                     Fld(6, 16) //[21:16]
    #define RK2_DQSIENUIDLY_R2DQS3IENUIDLY                     Fld(6, 24) //[29:24]

#define DRAMC_REG_RK2_DQSIENUIDLY_P1                   (DRAMC_NAO_BASE + 0x00848)
    #define RK2_DQSIENUIDLY_P1_R2DQS0IENUIDLY_P1               Fld(6, 0) //[5:0]
    #define RK2_DQSIENUIDLY_P1_R2DQS1IENUIDLY_P1               Fld(6, 8) //[13:8]
    #define RK2_DQSIENUIDLY_P1_R2DQS2IENUIDLY_P1               Fld(6, 16) //[21:16]
    #define RK2_DQSIENUIDLY_P1_R2DQS3IENUIDLY_P1               Fld(6, 24) //[29:24]

#define DRAMC_REG_RK2_DQS_STBCALDEC_CNT1               (DRAMC_NAO_BASE + 0x00850)
    #define RK2_DQS_STBCALDEC_CNT1_RK2_DQS0_STBCALDEC_CNT      Fld(16, 0) //[15:0]
    #define RK2_DQS_STBCALDEC_CNT1_RK2_DQS1_STBCALDEC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_DQS_STBCALDEC_CNT2               (DRAMC_NAO_BASE + 0x00854)
    #define RK2_DQS_STBCALDEC_CNT2_RK2_DQS2_STBCALDEC_CNT      Fld(16, 0) //[15:0]
    #define RK2_DQS_STBCALDEC_CNT2_RK2_DQS3_STBCALDEC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_DQS_STBCALINC_CNT1               (DRAMC_NAO_BASE + 0x00858)
    #define RK2_DQS_STBCALINC_CNT1_RK2_DQS0_STBCALINC_CNT      Fld(16, 0) //[15:0]
    #define RK2_DQS_STBCALINC_CNT1_RK2_DQS1_STBCALINC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_DQS_STBCALINC_CNT2               (DRAMC_NAO_BASE + 0x0085C)
    #define RK2_DQS_STBCALINC_CNT2_RK2_DQS2_STBCALINC_CNT      Fld(16, 0) //[15:0]
    #define RK2_DQS_STBCALINC_CNT2_RK2_DQS3_STBCALINC_CNT      Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_PI_DQ_CAL                        (DRAMC_NAO_BASE + 0x00860)
    #define RK2_PI_DQ_CAL_RK2_ARPI_DQ_B0_CAL                   Fld(6, 0) //[5:0]
    #define RK2_PI_DQ_CAL_RK2_ARPI_DQ_B1_CAL                   Fld(6, 8) //[13:8]
    #define RK2_PI_DQ_CAL_PI_DQ_ADJ_RK2                        Fld(6, 16) //[21:16]
    #define RK2_PI_DQ_CAL_PI_DQ_ADJ_RK2_OVERFLOW               Fld(1, 22) //[22:22]
    #define RK2_PI_DQ_CAL_PI_DQ_ADJ_RK2_B1                     Fld(6, 24) //[29:24]
    #define RK2_PI_DQ_CAL_PI_DQ_ADJ_RK2_B1_OVERFLOW            Fld(1, 30) //[30:30]

#define DRAMC_REG_RK2_DQSG_RETRY_FLAG                  (DRAMC_NAO_BASE + 0x00864)
    #define RK2_DQSG_RETRY_FLAG_RK2_RETRY_DONE0                Fld(1, 0) //[0:0]
    #define RK2_DQSG_RETRY_FLAG_RK2_RETRY_DONE1                Fld(1, 1) //[1:1]
    #define RK2_DQSG_RETRY_FLAG_RK2_RETRY_DONE2                Fld(1, 2) //[2:2]
    #define RK2_DQSG_RETRY_FLAG_RK2_RETRY_DONE3                Fld(1, 3) //[3:3]
    #define RK2_DQSG_RETRY_FLAG_RK2_RETRY_FAIL0                Fld(1, 16) //[16:16]
    #define RK2_DQSG_RETRY_FLAG_RK2_RETRY_FAIL1                Fld(1, 17) //[17:17]
    #define RK2_DQSG_RETRY_FLAG_RK2_RETRY_FAIL2                Fld(1, 18) //[18:18]
    #define RK2_DQSG_RETRY_FLAG_RK2_RETRY_FAIL3                Fld(1, 19) //[19:19]

#define DRAMC_REG_RK2_PI_DQM_CAL                       (DRAMC_NAO_BASE + 0x00868)
    #define RK2_PI_DQM_CAL_RK2_ARPI_DQM_B0_CAL                 Fld(6, 0) //[5:0]
    #define RK2_PI_DQM_CAL_RK2_ARPI_DQM_B1_CAL                 Fld(6, 8) //[13:8]

#define DRAMC_REG_RK2_DQS0_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x00870)
    #define RK2_DQS0_STBCAL_CNT_R2_DQS0_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK2_DQS1_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x00874)
    #define RK2_DQS1_STBCAL_CNT_R2_DQS1_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK2_DQS2_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x00878)
    #define RK2_DQS2_STBCAL_CNT_R2_DQS2_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK2_DQS3_STBCAL_CNT                  (DRAMC_NAO_BASE + 0x0087C)
    #define RK2_DQS3_STBCAL_CNT_R2_DQS3_STBCAL_CNT             Fld(17, 0) //[16:0]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_00              (DRAMC_NAO_BASE + 0x00880)
    #define RK2_B01_STB_DBG_INFO_00_RK2_B0_STB_DBG_INFO_00     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_00_RK2_B1_STB_DBG_INFO_00     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_01              (DRAMC_NAO_BASE + 0x00884)
    #define RK2_B01_STB_DBG_INFO_01_RK2_B0_STB_DBG_INFO_01     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_01_RK2_B1_STB_DBG_INFO_01     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_02              (DRAMC_NAO_BASE + 0x00888)
    #define RK2_B01_STB_DBG_INFO_02_RK2_B0_STB_DBG_INFO_02     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_02_RK2_B1_STB_DBG_INFO_02     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_03              (DRAMC_NAO_BASE + 0x0088C)
    #define RK2_B01_STB_DBG_INFO_03_RK2_B0_STB_DBG_INFO_03     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_03_RK2_B1_STB_DBG_INFO_03     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_04              (DRAMC_NAO_BASE + 0x00890)
    #define RK2_B01_STB_DBG_INFO_04_RK2_B0_STB_DBG_INFO_04     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_04_RK2_B1_STB_DBG_INFO_04     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_05              (DRAMC_NAO_BASE + 0x00894)
    #define RK2_B01_STB_DBG_INFO_05_RK2_B0_STB_DBG_INFO_05     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_05_RK2_B1_STB_DBG_INFO_05     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_06              (DRAMC_NAO_BASE + 0x00898)
    #define RK2_B01_STB_DBG_INFO_06_RK2_B0_STB_DBG_INFO_06     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_06_RK2_B1_STB_DBG_INFO_06     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_07              (DRAMC_NAO_BASE + 0x0089C)
    #define RK2_B01_STB_DBG_INFO_07_RK2_B0_STB_DBG_INFO_07     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_07_RK2_B1_STB_DBG_INFO_07     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_08              (DRAMC_NAO_BASE + 0x008A0)
    #define RK2_B01_STB_DBG_INFO_08_RK2_B0_STB_DBG_INFO_08     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_08_RK2_B1_STB_DBG_INFO_08     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_09              (DRAMC_NAO_BASE + 0x008A4)
    #define RK2_B01_STB_DBG_INFO_09_RK2_B0_STB_DBG_INFO_09     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_09_RK2_B1_STB_DBG_INFO_09     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_10              (DRAMC_NAO_BASE + 0x008A8)
    #define RK2_B01_STB_DBG_INFO_10_RK2_B0_STB_DBG_INFO_10     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_10_RK2_B1_STB_DBG_INFO_10     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_11              (DRAMC_NAO_BASE + 0x008AC)
    #define RK2_B01_STB_DBG_INFO_11_RK2_B0_STB_DBG_INFO_11     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_11_RK2_B1_STB_DBG_INFO_11     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_12              (DRAMC_NAO_BASE + 0x008B0)
    #define RK2_B01_STB_DBG_INFO_12_RK2_B0_STB_DBG_INFO_12     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_12_RK2_B1_STB_DBG_INFO_12     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_13              (DRAMC_NAO_BASE + 0x008B4)
    #define RK2_B01_STB_DBG_INFO_13_RK2_B0_STB_DBG_INFO_13     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_13_RK2_B1_STB_DBG_INFO_13     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_14              (DRAMC_NAO_BASE + 0x008B8)
    #define RK2_B01_STB_DBG_INFO_14_RK2_B0_STB_DBG_INFO_14     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_14_RK2_B1_STB_DBG_INFO_14     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B01_STB_DBG_INFO_15              (DRAMC_NAO_BASE + 0x008BC)
    #define RK2_B01_STB_DBG_INFO_15_RK2_B0_STB_DBG_INFO_15     Fld(16, 0) //[15:0]
    #define RK2_B01_STB_DBG_INFO_15_RK2_B1_STB_DBG_INFO_15     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_00              (DRAMC_NAO_BASE + 0x008C0)
    #define RK2_B23_STB_DBG_INFO_00_RK2_B2_STB_DBG_INFO_00     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_00_RK2_B3_STB_DBG_INFO_00     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_01              (DRAMC_NAO_BASE + 0x008C4)
    #define RK2_B23_STB_DBG_INFO_01_RK2_B2_STB_DBG_INFO_01     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_01_RK2_B3_STB_DBG_INFO_01     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_02              (DRAMC_NAO_BASE + 0x008C8)
    #define RK2_B23_STB_DBG_INFO_02_RK2_B2_STB_DBG_INFO_02     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_02_RK2_B3_STB_DBG_INFO_02     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_03              (DRAMC_NAO_BASE + 0x008CC)
    #define RK2_B23_STB_DBG_INFO_03_RK2_B2_STB_DBG_INFO_03     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_03_RK2_B3_STB_DBG_INFO_03     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_04              (DRAMC_NAO_BASE + 0x008D0)
    #define RK2_B23_STB_DBG_INFO_04_RK2_B2_STB_DBG_INFO_04     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_04_RK2_B3_STB_DBG_INFO_04     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_05              (DRAMC_NAO_BASE + 0x008D4)
    #define RK2_B23_STB_DBG_INFO_05_RK2_B2_STB_DBG_INFO_05     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_05_RK2_B3_STB_DBG_INFO_05     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_06              (DRAMC_NAO_BASE + 0x008D8)
    #define RK2_B23_STB_DBG_INFO_06_RK2_B2_STB_DBG_INFO_06     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_06_RK2_B3_STB_DBG_INFO_06     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_07              (DRAMC_NAO_BASE + 0x008DC)
    #define RK2_B23_STB_DBG_INFO_07_RK2_B2_STB_DBG_INFO_07     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_07_RK2_B3_STB_DBG_INFO_07     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_08              (DRAMC_NAO_BASE + 0x008E0)
    #define RK2_B23_STB_DBG_INFO_08_RK2_B2_STB_DBG_INFO_08     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_08_RK2_B3_STB_DBG_INFO_08     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_09              (DRAMC_NAO_BASE + 0x008E4)
    #define RK2_B23_STB_DBG_INFO_09_RK2_B2_STB_DBG_INFO_09     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_09_RK2_B3_STB_DBG_INFO_09     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_10              (DRAMC_NAO_BASE + 0x008E8)
    #define RK2_B23_STB_DBG_INFO_10_RK2_B2_STB_DBG_INFO_10     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_10_RK2_B3_STB_DBG_INFO_10     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_11              (DRAMC_NAO_BASE + 0x008EC)
    #define RK2_B23_STB_DBG_INFO_11_RK2_B2_STB_DBG_INFO_11     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_11_RK2_B3_STB_DBG_INFO_11     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_12              (DRAMC_NAO_BASE + 0x008F0)
    #define RK2_B23_STB_DBG_INFO_12_RK2_B2_STB_DBG_INFO_12     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_12_RK2_B3_STB_DBG_INFO_12     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_13              (DRAMC_NAO_BASE + 0x008F4)
    #define RK2_B23_STB_DBG_INFO_13_RK2_B2_STB_DBG_INFO_13     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_13_RK2_B3_STB_DBG_INFO_13     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_14              (DRAMC_NAO_BASE + 0x008F8)
    #define RK2_B23_STB_DBG_INFO_14_RK2_B2_STB_DBG_INFO_14     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_14_RK2_B3_STB_DBG_INFO_14     Fld(16, 16) //[31:16]

#define DRAMC_REG_RK2_B23_STB_DBG_INFO_15              (DRAMC_NAO_BASE + 0x008FC)
    #define RK2_B23_STB_DBG_INFO_15_RK2_B2_STB_DBG_INFO_15     Fld(16, 0) //[15:0]
    #define RK2_B23_STB_DBG_INFO_15_RK2_B3_STB_DBG_INFO_15     Fld(16, 16) //[31:16]

#endif /*__DRAMC_CH0_NAO_REG_H__*/
