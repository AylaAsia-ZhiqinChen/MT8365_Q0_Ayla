#ifndef __DDRPHY_WO_PLL_REG_H__
#define __DDRPHY_WO_PLL_REG_H__

#define DDRPHY_BASE_ADDR Channel_A_PHY_AO_BASE_VIRTUAL

#define DDRPHY_B0_DLL_ARPI0                         (DDRPHY_BASE_ADDR + 0x00080)
    #define B0_DLL_ARPI0_RG_ARPI_ISO_EN_B0                     Fld(1, 0) //[0:0]
    #define B0_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_B0                Fld(1, 1) //[1:1]
    #define B0_DLL_ARPI0_RG_ARPI_RESETB_B0                     Fld(1, 3) //[3:3]
    #define B0_DLL_ARPI0_RG_ARPI_LS_EN_B0                      Fld(1, 4) //[4:4]
    #define B0_DLL_ARPI0_RG_ARPI_LS_SEL_B0                     Fld(1, 5) //[5:5]
    #define B0_DLL_ARPI0_RG_ARPI_MCK8X_SEL_B0                  Fld(1, 6) //[6:6]

#define DDRPHY_B0_DLL_ARPI1                         (DDRPHY_BASE_ADDR + 0x00084)
    #define B0_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B0             Fld(1, 11) //[11:11]
    #define B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0                 Fld(1, 13) //[13:13]
    #define B0_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B0                Fld(1, 14) //[14:14]
    #define B0_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B0                Fld(1, 15) //[15:15]
    #define B0_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B0                 Fld(1, 17) //[17:17]
    #define B0_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B0               Fld(1, 19) //[19:19]
    #define B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0_REG_OPT          Fld(1, 20) //[20:20]
    #define B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0                  Fld(1, 21) //[21:21]
    #define B0_DLL_ARPI1_RG_ARPI_SET_UPDN_B0                   Fld(3, 28) //[30:28]

#define DDRPHY_B0_DLL_ARPI2                         (DDRPHY_BASE_ADDR + 0x00088)
    #define B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0                  Fld(1, 0) //[0:0]
    #define B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0                   Fld(1, 10) //[10:10]
    #define B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0                  Fld(1, 11) //[11:11]
    #define B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0                      Fld(1, 13) //[13:13]
    #define B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0                     Fld(1, 14) //[14:14]
    #define B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0                     Fld(1, 15) //[15:15]
    #define B0_DLL_ARPI2_RG_ARPI_CG_FB_B0                      Fld(1, 17) //[17:17]
    #define B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0                    Fld(1, 19) //[19:19]
    #define B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0              Fld(1, 27) //[27:27]
    #define B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0                     Fld(1, 31) //[31:31]

#define DDRPHY_B0_DLL_ARPI3                         (DDRPHY_BASE_ADDR + 0x0008C)
    #define B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0                  Fld(1, 11) //[11:11]
    #define B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0                      Fld(1, 13) //[13:13]
    #define B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0                     Fld(1, 14) //[14:14]
    #define B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0                     Fld(1, 15) //[15:15]
    #define B0_DLL_ARPI3_RG_ARPI_FB_EN_B0                      Fld(1, 17) //[17:17]
    #define B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0                    Fld(1, 19) //[19:19]

#define DDRPHY_B0_DLL_ARPI4                         (DDRPHY_BASE_ADDR + 0x00090)
    #define B0_DLL_ARPI4_RG_ARPI_BYPASS_DQSIEN_B0              Fld(1, 11) //[11:11]
    #define B0_DLL_ARPI4_RG_ARPI_BYPASS_DQ_B0                  Fld(1, 13) //[13:13]
    #define B0_DLL_ARPI4_RG_ARPI_BYPASS_DQM_B0                 Fld(1, 14) //[14:14]
    #define B0_DLL_ARPI4_RG_ARPI_BYPASS_DQS_B0                 Fld(1, 15) //[15:15]
    #define B0_DLL_ARPI4_RG_ARPI_BYPASS_FB_B0                  Fld(1, 17) //[17:17]
    #define B0_DLL_ARPI4_RG_ARPI_BYPASS_MCTL_B0                Fld(1, 19) //[19:19]

#define DDRPHY_B0_DLL_ARPI5                         (DDRPHY_BASE_ADDR + 0x00094)
    #define B0_DLL_ARPI5_RG_ARDLL_MON_SEL_B0                   Fld(4, 4) //[7:4]
    #define B0_DLL_ARPI5_RG_ARDLL_DIV_DEC_B0                   Fld(1, 8) //[8:8]
    #define B0_DLL_ARPI5_B0_DLL_ARPI5_RFU                      Fld(12, 12) //[23:12]
    #define B0_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_SEL_B0           Fld(1, 25) //[25:25]
    #define B0_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_B0               Fld(1, 26) //[26:26]
    #define B0_DLL_ARPI5_B0_DLL_ARPI5_RFU1                     Fld(1, 31) //[31:31]

#define DDRPHY_B0_DQ0                               (DDRPHY_BASE_ADDR + 0x00098)
    #define B0_DQ0_RG_RX_ARDQ0_OFFC_B0                         Fld(4, 0) //[3:0]
    #define B0_DQ0_RG_RX_ARDQ1_OFFC_B0                         Fld(4, 4) //[7:4]
    #define B0_DQ0_RG_RX_ARDQ2_OFFC_B0                         Fld(4, 8) //[11:8]
    #define B0_DQ0_RG_RX_ARDQ3_OFFC_B0                         Fld(4, 12) //[15:12]
    #define B0_DQ0_RG_RX_ARDQ4_OFFC_B0                         Fld(4, 16) //[19:16]
    #define B0_DQ0_RG_RX_ARDQ5_OFFC_B0                         Fld(4, 20) //[23:20]
    #define B0_DQ0_RG_RX_ARDQ6_OFFC_B0                         Fld(4, 24) //[27:24]
    #define B0_DQ0_RG_RX_ARDQ7_OFFC_B0                         Fld(4, 28) //[31:28]

#define DDRPHY_B0_DQ1                               (DDRPHY_BASE_ADDR + 0x0009C)
    #define B0_DQ1_RG_RX_ARDQM0_OFFC_B0                        Fld(4, 0) //[3:0]

#define DDRPHY_B0_DQ2                               (DDRPHY_BASE_ADDR + 0x000A0)
    #define B0_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B0                   Fld(1, 16) //[16:16]
    #define B0_DQ2_RG_TX_ARDQS0_OE_DIS_B0                      Fld(1, 17) //[17:17]
    #define B0_DQ2_RG_TX_ARDQM0_ODTEN_DIS_B0                   Fld(1, 18) //[18:18]
    #define B0_DQ2_RG_TX_ARDQM0_OE_DIS_B0                      Fld(1, 19) //[19:19]
    #define B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0                     Fld(1, 20) //[20:20]
    #define B0_DQ2_RG_TX_ARDQ_OE_DIS_B0                        Fld(1, 21) //[21:21]

#define DDRPHY_B0_DQ3                               (DDRPHY_BASE_ADDR + 0x000A4)
    #define B0_DQ3_RG_ARDQ_ATPG_EN_B0                          Fld(1, 0) //[0:0]
    #define B0_DQ3_RG_RX_ARDQ_SMT_EN_B0                        Fld(1, 1) //[1:1]
    #define B0_DQ3_RG_TX_ARDQ_EN_B0                            Fld(1, 2) //[2:2]
    #define B0_DQ3_RG_ARDQ_RESETB_B0                           Fld(1, 3) //[3:3]
    #define B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0                  Fld(1, 5) //[5:5]
    #define B0_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B0                  Fld(1, 6) //[6:6]
    #define B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0                    Fld(1, 7) //[7:7]
    #define B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0                   Fld(1, 10) //[10:10]
    #define B0_DQ3_RG_RX_ARDQ_OFFC_EN_B0                       Fld(1, 11) //[11:11]
    #define B0_DQ3_RG_RX_ARDQS0_SWAP_EN_B0                     Fld(1, 15) //[15:15]

#define DDRPHY_B0_DQ4                               (DDRPHY_BASE_ADDR + 0x000A8)
    #define B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0                    Fld(7, 0) //[6:0]
    #define B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0                    Fld(7, 8) //[14:8]
    #define B0_DQ4_RG_RX_ARDQ_EYE_R_DLY_B0                     Fld(6, 16) //[21:16]
    #define B0_DQ4_RG_RX_ARDQ_EYE_F_DLY_B0                     Fld(6, 24) //[29:24]

#define DDRPHY_B0_DQ5                               (DDRPHY_BASE_ADDR + 0x000AC)
    #define B0_DQ5_B0_DQ5_RFU                                  Fld(8, 0) //[7:0]
    #define B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0                  Fld(6, 8) //[13:8]
    #define B0_DQ5_RG_RX_ARDQ_VREF_EN_B0                       Fld(1, 16) //[16:16]
    #define B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0                   Fld(1, 17) //[17:17]
    #define B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0                       Fld(4, 20) //[23:20]
    #define B0_DQ5_RG_RX_ARDQ_EYE_EN_B0                        Fld(1, 24) //[24:24]
    #define B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0              Fld(1, 25) //[25:25]
    #define B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0                      Fld(1, 31) //[31:31]

#define DDRPHY_B0_DQ6                               (DDRPHY_BASE_ADDR + 0x000B0)
    #define B0_DQ6_RG_RX_ARDQ_BIAS_PS_B0                       Fld(2, 0) //[1:0]
    #define B0_DQ6_RG_TX_ARDQ_OE_EXT_DIS_B0                    Fld(1, 2) //[2:2]
    #define B0_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B0                 Fld(1, 3) //[3:3]
    #define B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0                   Fld(1, 5) //[5:5]
    #define B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0                   Fld(1, 6) //[6:6]
    #define B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0                 Fld(1, 7) //[7:7]
    #define B0_DQ6_RG_RX_ARDQ_LPBK_EN_B0                       Fld(1, 8) //[8:8]
    #define B0_DQ6_RG_RX_ARDQ_O1_SEL_B0                        Fld(1, 9) //[9:9]
    #define B0_DQ6_RG_RX_ARDQ_JM_SEL_B0                        Fld(1, 11) //[11:11]
    #define B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0                       Fld(1, 12) //[12:12]
    #define B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0                 Fld(2, 14) //[15:14]
    #define B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0                      Fld(1, 16) //[16:16]
    #define B0_DQ6_RG_TX_ARDQ_DDR4_SEL_B0                      Fld(1, 17) //[17:17]
    #define B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0                      Fld(1, 18) //[18:18]
    #define B0_DQ6_RG_TX_ARDQ_DDR3_SEL_B0                      Fld(1, 19) //[19:19]
    #define B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0            Fld(1, 24) //[24:24]
    #define B0_DQ6_RG_RX_ARDQ_EYE_OE_GATE_EN_B0                Fld(1, 28) //[28:28]
    #define B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0                 Fld(1, 31) //[31:31]

#define DDRPHY_B0_DQ7                               (DDRPHY_BASE_ADDR + 0x000B4)
    #define B0_DQ7_RG_TX_ARDQS0B_PULL_DN_B0                    Fld(1, 0) //[0:0]
    #define B0_DQ7_RG_TX_ARDQS0B_PULL_UP_B0                    Fld(1, 1) //[1:1]
    #define B0_DQ7_RG_TX_ARDQS0_PULL_DN_B0                     Fld(1, 2) //[2:2]
    #define B0_DQ7_RG_TX_ARDQS0_PULL_UP_B0                     Fld(1, 3) //[3:3]
    #define B0_DQ7_RG_TX_ARDQM0_PULL_DN_B0                     Fld(1, 4) //[4:4]
    #define B0_DQ7_RG_TX_ARDQM0_PULL_UP_B0                     Fld(1, 5) //[5:5]
    #define B0_DQ7_RG_TX_ARDQ_PULL_DN_B0                       Fld(1, 6) //[6:6]
    #define B0_DQ7_RG_TX_ARDQ_PULL_UP_B0                       Fld(1, 7) //[7:7]
    #define B0_DQ7_RG_TX_ARDQS0B_PULL_DN_B0_LP4Y               Fld(1, 16) //[16:16]

#define DDRPHY_B0_DQ8                               (DDRPHY_BASE_ADDR + 0x000B8)
    #define B0_DQ8_RG_TX_ARDQ_EN_LP4P_B0                       Fld(1, 0) //[0:0]
    #define B0_DQ8_RG_TX_ARDQ_EN_CAP_LP4P_B0                   Fld(1, 1) //[1:1]
    #define B0_DQ8_RG_TX_ARDQ_CAP_DET_B0                       Fld(1, 2) //[2:2]
    #define B0_DQ8_RG_TX_ARDQ_CKE_MCK4X_SEL_B0                 Fld(2, 3) //[4:3]
    #define B0_DQ8_RG_ARPI_TX_CG_DQ_EN_B0                      Fld(1, 5) //[5:5]
    #define B0_DQ8_RG_ARPI_TX_CG_DQM_EN_B0                     Fld(1, 6) //[6:6]
    #define B0_DQ8_RG_ARPI_TX_CG_DQS_EN_B0                     Fld(1, 7) //[7:7]
    #define B0_DQ8_RG_RX_ARDQS_BURST_E1_EN_B0                  Fld(1, 8) //[8:8]
    #define B0_DQ8_RG_RX_ARDQS_BURST_E2_EN_B0                  Fld(1, 9) //[9:9]
    #define B0_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B0                 Fld(1, 10) //[10:10]
    #define B0_DQ8_RG_RX_ARDQS_GATE_EN_MODE_B0                 Fld(1, 12) //[12:12]
    #define B0_DQ8_RG_RX_ARDQS_SER_RST_MODE_B0                 Fld(1, 13) //[13:13]
    #define B0_DQ8_RG_ARDLL_RESETB_B0                          Fld(1, 15) //[15:15]

#define DDRPHY_B0_DQ9                               (DDRPHY_BASE_ADDR + 0x000BC)
    #define B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0                  Fld(1, 0) //[0:0]
    #define B0_DQ9_RG_DQOE_TIEH_B0                             Fld(1, 1) //[1:1]
    #define B0_DQ9_RG_DQMOE_TIEH_B0                            Fld(1, 2) //[2:2]
    #define B0_DQ9_RG_DQSOE_TIEH_B0                            Fld(1, 3) //[3:3]
    #define B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0                Fld(1, 4) //[4:4]
    #define B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0                  Fld(1, 5) //[5:5]
    #define B0_DQ9_R_DMARPIDQ_SW_B0                            Fld(1, 6) //[6:6]
    #define B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0                   Fld(1, 7) //[7:7]
    #define B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0                     Fld(8, 8) //[15:8]
    #define B0_DQ9_R_DMDQSIEN_VALID_LAT_B0                     Fld(3, 16) //[18:16]
    #define B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0                     Fld(3, 20) //[22:20]
    #define B0_DQ9_R_DMRXDVS_VALID_LAT_B0                      Fld(3, 24) //[26:24]
    #define B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0                      Fld(3, 28) //[30:28]

#define DDRPHY_B0_DQ10                              (DDRPHY_BASE_ADDR + 0x000C0)
    #define B0_DQ10_RG_RX_ARDQ0_DQS_SEL_B0                     Fld(1, 0) //[0:0]
    #define B0_DQ10_RG_RX_ARDQ1_DQS_SEL_B0                     Fld(1, 1) //[1:1]
    #define B0_DQ10_RG_RX_ARDQ2_DQS_SEL_B0                     Fld(1, 2) //[2:2]
    #define B0_DQ10_RG_RX_ARDQ3_DQS_SEL_B0                     Fld(1, 3) //[3:3]
    #define B0_DQ10_RG_RX_ARDQ4_DQS_SEL_B0                     Fld(1, 4) //[4:4]
    #define B0_DQ10_RG_RX_ARDQ5_DQS_SEL_B0                     Fld(1, 5) //[5:5]
    #define B0_DQ10_RG_RX_ARDQ6_DQS_SEL_B0                     Fld(1, 6) //[6:6]
    #define B0_DQ10_RG_RX_ARDQ7_DQS_SEL_B0                     Fld(1, 7) //[7:7]
    #define B0_DQ10_RG_RX_ARDQM_DQS_SEL_B0                     Fld(1, 8) //[8:8]
    #define B0_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B0                   Fld(1, 16) //[16:16]
    #define B0_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B0                   Fld(1, 17) //[17:17]
    #define B0_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B0                   Fld(1, 18) //[18:18]
    #define B0_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B0                   Fld(1, 19) //[19:19]
    #define B0_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B0                   Fld(1, 20) //[20:20]
    #define B0_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B0                   Fld(1, 21) //[21:21]
    #define B0_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B0                   Fld(1, 22) //[22:22]
    #define B0_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B0                   Fld(1, 23) //[23:23]
    #define B0_DQ10_RG_TX_ARDQM_MCK4X_SEL_B0                   Fld(1, 24) //[24:24]
    #define B0_DQ10_RG_TX_ARDQS0_MCK4X_SEL_B0                  Fld(1, 25) //[25:25]
    #define B0_DQ10_RG_TX_ARDQS0B_MCK4X_SEL_B0                 Fld(1, 26) //[26:26]

#define DDRPHY_RFU_0X0C4                            (DDRPHY_BASE_ADDR + 0x000C4)
    #define RFU_0X0C4_RESERVED_0X0C4                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X0C8                            (DDRPHY_BASE_ADDR + 0x000C8)
    #define RFU_0X0C8_RESERVED_0X0C8                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X0CC                            (DDRPHY_BASE_ADDR + 0x000CC)
    #define RFU_0X0CC_RESERVED_0X0CC                           Fld(32, 0) //[31:0]

#define DDRPHY_B0_TX_MCK                            (DDRPHY_BASE_ADDR + 0x000D0)
    #define B0_TX_MCK_R_DM_TX_MCK_FRUN_B0                      Fld(10, 0) //[9:0]

#define DDRPHY_RFU_0X0D4                            (DDRPHY_BASE_ADDR + 0x000D4)
    #define RFU_0X0D4_RESERVED_0X0D4                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X0D8                            (DDRPHY_BASE_ADDR + 0x000D8)
    #define RFU_0X0D8_RESERVED_0X0D8                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X0DC                            (DDRPHY_BASE_ADDR + 0x000DC)
    #define RFU_0X0DC_RESERVED_0X0DC                           Fld(32, 0) //[31:0]

#define DDRPHY_B1_DLL_ARPI0                         (DDRPHY_BASE_ADDR + 0x00100)
    #define B1_DLL_ARPI0_RG_ARPI_ISO_EN_B1                     Fld(1, 0) //[0:0]
    #define B1_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_B1                Fld(1, 1) //[1:1]
    #define B1_DLL_ARPI0_RG_ARPI_RESETB_B1                     Fld(1, 3) //[3:3]
    #define B1_DLL_ARPI0_RG_ARPI_LS_EN_B1                      Fld(1, 4) //[4:4]
    #define B1_DLL_ARPI0_RG_ARPI_LS_SEL_B1                     Fld(1, 5) //[5:5]
    #define B1_DLL_ARPI0_RG_ARPI_MCK8X_SEL_B1                  Fld(1, 6) //[6:6]

#define DDRPHY_B1_DLL_ARPI1                         (DDRPHY_BASE_ADDR + 0x00104)
    #define B1_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B1             Fld(1, 11) //[11:11]
    #define B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1                 Fld(1, 13) //[13:13]
    #define B1_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B1                Fld(1, 14) //[14:14]
    #define B1_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B1                Fld(1, 15) //[15:15]
    #define B1_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B1                 Fld(1, 17) //[17:17]
    #define B1_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B1               Fld(1, 19) //[19:19]
    #define B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1_REG_OPT          Fld(1, 20) //[20:20]
    #define B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1                  Fld(1, 21) //[21:21]
    #define B1_DLL_ARPI1_RG_ARPI_SET_UPDN_B1                   Fld(3, 28) //[30:28]

#define DDRPHY_B1_DLL_ARPI2                         (DDRPHY_BASE_ADDR + 0x00108)
    #define B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1                  Fld(1, 0) //[0:0]
    #define B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1                   Fld(1, 10) //[10:10]
    #define B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1                  Fld(1, 11) //[11:11]
    #define B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1                      Fld(1, 13) //[13:13]
    #define B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1                     Fld(1, 14) //[14:14]
    #define B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1                     Fld(1, 15) //[15:15]
    #define B1_DLL_ARPI2_RG_ARPI_CG_FB_B1                      Fld(1, 17) //[17:17]
    #define B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1                    Fld(1, 19) //[19:19]
    #define B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1              Fld(1, 27) //[27:27]
    #define B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1                     Fld(1, 31) //[31:31]

#define DDRPHY_B1_DLL_ARPI3                         (DDRPHY_BASE_ADDR + 0x0010C)
    #define B1_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B1                  Fld(1, 11) //[11:11]
    #define B1_DLL_ARPI3_RG_ARPI_DQ_EN_B1                      Fld(1, 13) //[13:13]
    #define B1_DLL_ARPI3_RG_ARPI_DQM_EN_B1                     Fld(1, 14) //[14:14]
    #define B1_DLL_ARPI3_RG_ARPI_DQS_EN_B1                     Fld(1, 15) //[15:15]
    #define B1_DLL_ARPI3_RG_ARPI_FB_EN_B1                      Fld(1, 17) //[17:17]
    #define B1_DLL_ARPI3_RG_ARPI_MCTL_EN_B1                    Fld(1, 19) //[19:19]

#define DDRPHY_B1_DLL_ARPI4                         (DDRPHY_BASE_ADDR + 0x00110)
    #define B1_DLL_ARPI4_RG_ARPI_BYPASS_DQSIEN_B1              Fld(1, 11) //[11:11]
    #define B1_DLL_ARPI4_RG_ARPI_BYPASS_DQ_B1                  Fld(1, 13) //[13:13]
    #define B1_DLL_ARPI4_RG_ARPI_BYPASS_DQM_B1                 Fld(1, 14) //[14:14]
    #define B1_DLL_ARPI4_RG_ARPI_BYPASS_DQS_B1                 Fld(1, 15) //[15:15]
    #define B1_DLL_ARPI4_RG_ARPI_BYPASS_FB_B1                  Fld(1, 17) //[17:17]
    #define B1_DLL_ARPI4_RG_ARPI_BYPASS_MCTL_B1                Fld(1, 19) //[19:19]

#define DDRPHY_B1_DLL_ARPI5                         (DDRPHY_BASE_ADDR + 0x00114)
    #define B1_DLL_ARPI5_RG_ARDLL_MON_SEL_B1                   Fld(4, 4) //[7:4]
    #define B1_DLL_ARPI5_RG_ARDLL_DIV_DEC_B1                   Fld(1, 8) //[8:8]
    #define B1_DLL_ARPI5_B1_DLL_ARPI5_RFU                      Fld(12, 12) //[23:12]
    #define B1_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_SEL_B1           Fld(1, 25) //[25:25]
    #define B1_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_B1               Fld(1, 26) //[26:26]
    #define B1_DLL_ARPI5_B1_DLL_ARPI5_RFU1                     Fld(1, 31) //[31:31]

#define DDRPHY_B1_DQ0                               (DDRPHY_BASE_ADDR + 0x00118)
    #define B1_DQ0_RG_RX_ARDQ0_OFFC_B1                         Fld(4, 0) //[3:0]
    #define B1_DQ0_RG_RX_ARDQ1_OFFC_B1                         Fld(4, 4) //[7:4]
    #define B1_DQ0_RG_RX_ARDQ2_OFFC_B1                         Fld(4, 8) //[11:8]
    #define B1_DQ0_RG_RX_ARDQ3_OFFC_B1                         Fld(4, 12) //[15:12]
    #define B1_DQ0_RG_RX_ARDQ4_OFFC_B1                         Fld(4, 16) //[19:16]
    #define B1_DQ0_RG_RX_ARDQ5_OFFC_B1                         Fld(4, 20) //[23:20]
    #define B1_DQ0_RG_RX_ARDQ6_OFFC_B1                         Fld(4, 24) //[27:24]
    #define B1_DQ0_RG_RX_ARDQ7_OFFC_B1                         Fld(4, 28) //[31:28]

#define DDRPHY_B1_DQ1                               (DDRPHY_BASE_ADDR + 0x0011C)
    #define B1_DQ1_RG_RX_ARDQM0_OFFC_B1                        Fld(4, 0) //[3:0]

#define DDRPHY_B1_DQ2                               (DDRPHY_BASE_ADDR + 0x00120)
    #define B1_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B1                   Fld(1, 16) //[16:16]
    #define B1_DQ2_RG_TX_ARDQS0_OE_DIS_B1                      Fld(1, 17) //[17:17]
    #define B1_DQ2_RG_TX_ARDQM0_ODTEN_DIS_B1                   Fld(1, 18) //[18:18]
    #define B1_DQ2_RG_TX_ARDQM0_OE_DIS_B1                      Fld(1, 19) //[19:19]
    #define B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1                     Fld(1, 20) //[20:20]
    #define B1_DQ2_RG_TX_ARDQ_OE_DIS_B1                        Fld(1, 21) //[21:21]

#define DDRPHY_B1_DQ3                               (DDRPHY_BASE_ADDR + 0x00124)
    #define B1_DQ3_RG_ARDQ_ATPG_EN_B1                          Fld(1, 0) //[0:0]
    #define B1_DQ3_RG_RX_ARDQ_SMT_EN_B1                        Fld(1, 1) //[1:1]
    #define B1_DQ3_RG_TX_ARDQ_EN_B1                            Fld(1, 2) //[2:2]
    #define B1_DQ3_RG_ARDQ_RESETB_B1                           Fld(1, 3) //[3:3]
    #define B1_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B1                  Fld(1, 5) //[5:5]
    #define B1_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B1                  Fld(1, 6) //[6:6]
    #define B1_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B1                    Fld(1, 7) //[7:7]
    #define B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1                   Fld(1, 10) //[10:10]
    #define B1_DQ3_RG_RX_ARDQ_OFFC_EN_B1                       Fld(1, 11) //[11:11]
    #define B1_DQ3_RG_RX_ARDQS0_SWAP_EN_B1                     Fld(1, 15) //[15:15]

#define DDRPHY_B1_DQ4                               (DDRPHY_BASE_ADDR + 0x00128)
    #define B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1                    Fld(7, 0) //[6:0]
    #define B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1                    Fld(7, 8) //[14:8]
    #define B1_DQ4_RG_RX_ARDQ_EYE_R_DLY_B1                     Fld(6, 16) //[21:16]
    #define B1_DQ4_RG_RX_ARDQ_EYE_F_DLY_B1                     Fld(6, 24) //[29:24]

#define DDRPHY_B1_DQ5                               (DDRPHY_BASE_ADDR + 0x0012C)
    #define B1_DQ5_B1_DQ5_RFU                                  Fld(8, 0) //[7:0]
    #define B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1                  Fld(6, 8) //[13:8]
    #define B1_DQ5_RG_RX_ARDQ_VREF_EN_B1                       Fld(1, 16) //[16:16]
    #define B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1                   Fld(1, 17) //[17:17]
    #define B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1                       Fld(4, 20) //[23:20]
    #define B1_DQ5_RG_RX_ARDQ_EYE_EN_B1                        Fld(1, 24) //[24:24]
    #define B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1              Fld(1, 25) //[25:25]
    #define B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1                      Fld(1, 31) //[31:31]

#define DDRPHY_B1_DQ6                               (DDRPHY_BASE_ADDR + 0x00130)
    #define B1_DQ6_RG_RX_ARDQ_BIAS_PS_B1                       Fld(2, 0) //[1:0]
    #define B1_DQ6_RG_TX_ARDQ_OE_EXT_DIS_B1                    Fld(1, 2) //[2:2]
    #define B1_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B1                 Fld(1, 3) //[3:3]
    #define B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1                   Fld(1, 5) //[5:5]
    #define B1_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B1                   Fld(1, 6) //[6:6]
    #define B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1                 Fld(1, 7) //[7:7]
    #define B1_DQ6_RG_RX_ARDQ_LPBK_EN_B1                       Fld(1, 8) //[8:8]
    #define B1_DQ6_RG_RX_ARDQ_O1_SEL_B1                        Fld(1, 9) //[9:9]
    #define B1_DQ6_RG_RX_ARDQ_JM_SEL_B1                        Fld(1, 11) //[11:11]
    #define B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1                       Fld(1, 12) //[12:12]
    #define B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1                 Fld(2, 14) //[15:14]
    #define B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1                      Fld(1, 16) //[16:16]
    #define B1_DQ6_RG_TX_ARDQ_DDR4_SEL_B1                      Fld(1, 17) //[17:17]
    #define B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1                      Fld(1, 18) //[18:18]
    #define B1_DQ6_RG_TX_ARDQ_DDR3_SEL_B1                      Fld(1, 19) //[19:19]
    #define B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1            Fld(1, 24) //[24:24]
    #define B1_DQ6_RG_RX_ARDQ_EYE_OE_GATE_EN_B1                Fld(1, 28) //[28:28]
    #define B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1                 Fld(1, 31) //[31:31]

#define DDRPHY_B1_DQ7                               (DDRPHY_BASE_ADDR + 0x00134)
    #define B1_DQ7_RG_TX_ARDQS0B_PULL_DN_B1                    Fld(1, 0) //[0:0]
    #define B1_DQ7_RG_TX_ARDQS0B_PULL_UP_B1                    Fld(1, 1) //[1:1]
    #define B1_DQ7_RG_TX_ARDQS0_PULL_DN_B1                     Fld(1, 2) //[2:2]
    #define B1_DQ7_RG_TX_ARDQS0_PULL_UP_B1                     Fld(1, 3) //[3:3]
    #define B1_DQ7_RG_TX_ARDQM0_PULL_DN_B1                     Fld(1, 4) //[4:4]
    #define B1_DQ7_RG_TX_ARDQM0_PULL_UP_B1                     Fld(1, 5) //[5:5]
    #define B1_DQ7_RG_TX_ARDQ_PULL_DN_B1                       Fld(1, 6) //[6:6]
    #define B1_DQ7_RG_TX_ARDQ_PULL_UP_B1                       Fld(1, 7) //[7:7]
    #define B1_DQ7_RG_TX_ARDQS0B_PULL_DN_B1_LP4Y               Fld(1, 16) //[16:16]

#define DDRPHY_B1_DQ8                               (DDRPHY_BASE_ADDR + 0x00138)
    #define B1_DQ8_RG_TX_ARDQ_EN_LP4P_B1                       Fld(1, 0) //[0:0]
    #define B1_DQ8_RG_TX_ARDQ_EN_CAP_LP4P_B1                   Fld(1, 1) //[1:1]
    #define B1_DQ8_RG_TX_ARDQ_CAP_DET_B1                       Fld(1, 2) //[2:2]
    #define B1_DQ8_RG_TX_ARDQ_CKE_MCK4X_SEL_B1                 Fld(2, 3) //[4:3]
    #define B1_DQ8_RG_ARPI_TX_CG_DQ_EN_B1                      Fld(1, 5) //[5:5]
    #define B1_DQ8_RG_ARPI_TX_CG_DQM_EN_B1                     Fld(1, 6) //[6:6]
    #define B1_DQ8_RG_ARPI_TX_CG_DQS_EN_B1                     Fld(1, 7) //[7:7]
    #define B1_DQ8_RG_RX_ARDQS_BURST_E1_EN_B1                  Fld(1, 8) //[8:8]
    #define B1_DQ8_RG_RX_ARDQS_BURST_E2_EN_B1                  Fld(1, 9) //[9:9]
    #define B1_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B1                 Fld(1, 10) //[10:10]
    #define B1_DQ8_RG_RX_ARDQS_GATE_EN_MODE_B1                 Fld(1, 12) //[12:12]
    #define B1_DQ8_RG_RX_ARDQS_SER_RST_MODE_B1                 Fld(1, 13) //[13:13]
    #define B1_DQ8_RG_ARDLL_RESETB_B1                          Fld(1, 15) //[15:15]

#define DDRPHY_B1_DQ9                               (DDRPHY_BASE_ADDR + 0x0013C)
    #define B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1                  Fld(1, 0) //[0:0]
    #define B1_DQ9_RG_DQOE_TIEH_B1                             Fld(1, 1) //[1:1]
    #define B1_DQ9_RG_DQMOE_TIEH_B1                            Fld(1, 2) //[2:2]
    #define B1_DQ9_RG_DQSOE_TIEH_B1                            Fld(1, 3) //[3:3]
    #define B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1                Fld(1, 4) //[4:4]
    #define B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1                  Fld(1, 5) //[5:5]
    #define B1_DQ9_R_DMARPIDQ_SW_B1                            Fld(1, 6) //[6:6]
    #define B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1                   Fld(1, 7) //[7:7]
    #define B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1                     Fld(8, 8) //[15:8]
    #define B1_DQ9_R_DMDQSIEN_VALID_LAT_B1                     Fld(3, 16) //[18:16]
    #define B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1                     Fld(3, 20) //[22:20]
    #define B1_DQ9_R_DMRXDVS_VALID_LAT_B1                      Fld(3, 24) //[26:24]
    #define B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1                      Fld(3, 28) //[30:28]

#define DDRPHY_B1_DQ10                              (DDRPHY_BASE_ADDR + 0x00140)
    #define B1_DQ10_RG_RX_ARDQ0_DQS_SEL_B1                     Fld(1, 0) //[0:0]
    #define B1_DQ10_RG_RX_ARDQ1_DQS_SEL_B1                     Fld(1, 1) //[1:1]
    #define B1_DQ10_RG_RX_ARDQ2_DQS_SEL_B1                     Fld(1, 2) //[2:2]
    #define B1_DQ10_RG_RX_ARDQ3_DQS_SEL_B1                     Fld(1, 3) //[3:3]
    #define B1_DQ10_RG_RX_ARDQ4_DQS_SEL_B1                     Fld(1, 4) //[4:4]
    #define B1_DQ10_RG_RX_ARDQ5_DQS_SEL_B1                     Fld(1, 5) //[5:5]
    #define B1_DQ10_RG_RX_ARDQ6_DQS_SEL_B1                     Fld(1, 6) //[6:6]
    #define B1_DQ10_RG_RX_ARDQ7_DQS_SEL_B1                     Fld(1, 7) //[7:7]
    #define B1_DQ10_RG_RX_ARDQM_DQS_SEL_B1                     Fld(1, 8) //[8:8]
    #define B1_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B1                   Fld(1, 16) //[16:16]
    #define B1_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B1                   Fld(1, 17) //[17:17]
    #define B1_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B1                   Fld(1, 18) //[18:18]
    #define B1_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B1                   Fld(1, 19) //[19:19]
    #define B1_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B1                   Fld(1, 20) //[20:20]
    #define B1_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B1                   Fld(1, 21) //[21:21]
    #define B1_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B1                   Fld(1, 22) //[22:22]
    #define B1_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B1                   Fld(1, 23) //[23:23]
    #define B1_DQ10_RG_TX_ARDQM_MCK4X_SEL_B1                   Fld(1, 24) //[24:24]
    #define B1_DQ10_RG_TX_ARDQS0_MCK4X_SEL_B1                  Fld(1, 25) //[25:25]
    #define B1_DQ10_RG_TX_ARDQS0B_MCK4X_SEL_B1                 Fld(1, 26) //[26:26]

#define DDRPHY_RFU_0X144                            (DDRPHY_BASE_ADDR + 0x00144)
    #define RFU_0X144_RESERVED_0X144                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X148                            (DDRPHY_BASE_ADDR + 0x00148)
    #define RFU_0X148_RESERVED_0X148                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X14C                            (DDRPHY_BASE_ADDR + 0x0014C)
    #define RFU_0X14C_RESERVED_0X14C                           Fld(32, 0) //[31:0]

#define DDRPHY_B1_TX_MCK                            (DDRPHY_BASE_ADDR + 0x00150)
    #define B1_TX_MCK_R_DM_TX_MCK_FRUN_B1                      Fld(10, 0) //[9:0]

#define DDRPHY_RFU_0X154                            (DDRPHY_BASE_ADDR + 0x00154)
    #define RFU_0X154_RESERVED_0X154                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X158                            (DDRPHY_BASE_ADDR + 0x00158)
    #define RFU_0X158_RESERVED_0X158                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X15C                            (DDRPHY_BASE_ADDR + 0x0015C)
    #define RFU_0X15C_RESERVED_0X15C                           Fld(32, 0) //[31:0]

#define DDRPHY_CA_DLL_ARPI0                         (DDRPHY_BASE_ADDR + 0x00180)
    #define CA_DLL_ARPI0_RG_ARPI_ISO_EN_CA                     Fld(1, 0) //[0:0]
    #define CA_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_CA                Fld(1, 1) //[1:1]
    #define CA_DLL_ARPI0_RG_ARPI_RESETB_CA                     Fld(1, 3) //[3:3]
    #define CA_DLL_ARPI0_RG_ARPI_LS_EN_CA                      Fld(1, 4) //[4:4]
    #define CA_DLL_ARPI0_RG_ARPI_LS_SEL_CA                     Fld(1, 5) //[5:5]
    #define CA_DLL_ARPI0_RG_ARPI_MCK8X_SEL_CA                  Fld(1, 6) //[6:6]

#define DDRPHY_CA_DLL_ARPI1                         (DDRPHY_BASE_ADDR + 0x00184)
    #define CA_DLL_ARPI1_RG_ARPI_CLKIEN_JUMP_EN                Fld(1, 11) //[11:11]
    #define CA_DLL_ARPI1_RG_ARPI_CMD_JUMP_EN                   Fld(1, 13) //[13:13]
    #define CA_DLL_ARPI1_RG_ARPI_CLK_JUMP_EN                   Fld(1, 15) //[15:15]
    #define CA_DLL_ARPI1_RG_ARPI_CS_JUMP_EN                    Fld(1, 16) //[16:16]
    #define CA_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_CA                 Fld(1, 17) //[17:17]
    #define CA_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_CA               Fld(1, 19) //[19:19]
    #define CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA_REG_OPT          Fld(1, 20) //[20:20]
    #define CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA                  Fld(1, 21) //[21:21]
    #define CA_DLL_ARPI1_RG_ARPI_SET_UPDN_CA                   Fld(3, 28) //[30:28]

#define DDRPHY_CA_DLL_ARPI2                         (DDRPHY_BASE_ADDR + 0x00188)
    #define CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA                  Fld(1, 0) //[0:0]
    #define CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA                   Fld(1, 10) //[10:10]
    #define CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN                     Fld(1, 11) //[11:11]
    #define CA_DLL_ARPI2_RG_ARPI_CG_CMD                        Fld(1, 13) //[13:13]
    #define CA_DLL_ARPI2_RG_ARPI_CG_CLK                        Fld(1, 15) //[15:15]
    #define CA_DLL_ARPI2_RG_ARPI_CG_CS                         Fld(1, 16) //[16:16]
    #define CA_DLL_ARPI2_RG_ARPI_CG_FB_CA                      Fld(1, 17) //[17:17]
    #define CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA                    Fld(1, 19) //[19:19]
    #define CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA              Fld(1, 27) //[27:27]
    #define CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA                     Fld(1, 31) //[31:31]

#define DDRPHY_CA_DLL_ARPI3                         (DDRPHY_BASE_ADDR + 0x0018C)
    #define CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN                     Fld(1, 11) //[11:11]
    #define CA_DLL_ARPI3_RG_ARPI_CMD_EN                        Fld(1, 13) //[13:13]
    #define CA_DLL_ARPI3_RG_ARPI_CLK_EN                        Fld(1, 15) //[15:15]
    #define CA_DLL_ARPI3_RG_ARPI_CS_EN                         Fld(1, 16) //[16:16]
    #define CA_DLL_ARPI3_RG_ARPI_FB_EN_CA                      Fld(1, 17) //[17:17]
    #define CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA                    Fld(1, 19) //[19:19]

#define DDRPHY_CA_DLL_ARPI4                         (DDRPHY_BASE_ADDR + 0x00190)
    #define CA_DLL_ARPI4_RG_ARPI_BYPASS_CLKIEN                 Fld(1, 11) //[11:11]
    #define CA_DLL_ARPI4_RG_ARPI_BYPASS_CMD                    Fld(1, 13) //[13:13]
    #define CA_DLL_ARPI4_RG_ARPI_BYPASS_CLK                    Fld(1, 15) //[15:15]
    #define CA_DLL_ARPI4_RG_ARPI_BYPASS_CS                     Fld(1, 16) //[16:16]
    #define CA_DLL_ARPI4_RG_ARPI_BYPASS_FB_CA                  Fld(1, 17) //[17:17]
    #define CA_DLL_ARPI4_RG_ARPI_BYPASS_MCTL_CA                Fld(1, 19) //[19:19]

#define DDRPHY_CA_DLL_ARPI5                         (DDRPHY_BASE_ADDR + 0x00194)
    #define CA_DLL_ARPI5_RG_ARDLL_MON_SEL_CA                   Fld(4, 4) //[7:4]
    #define CA_DLL_ARPI5_RG_ARDLL_DIV_DEC_CA                   Fld(1, 8) //[8:8]
    #define CA_DLL_ARPI5_CA_DLL_ARPI5_RFU                      Fld(12, 12) //[23:12]
    #define CA_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_SEL_CA           Fld(1, 25) //[25:25]
    #define CA_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_CA               Fld(1, 26) //[26:26]

#define DDRPHY_CA_CMD0                              (DDRPHY_BASE_ADDR + 0x00198)
    #define CA_CMD0_RG_RX_ARCA0_OFFC                           Fld(4, 0) //[3:0]
    #define CA_CMD0_RG_RX_ARCA1_OFFC                           Fld(4, 4) //[7:4]
    #define CA_CMD0_RG_RX_ARCA2_OFFC                           Fld(4, 8) //[11:8]
    #define CA_CMD0_RG_RX_ARCA3_OFFC                           Fld(4, 12) //[15:12]
    #define CA_CMD0_RG_RX_ARCA4_OFFC                           Fld(4, 16) //[19:16]
    #define CA_CMD0_RG_RX_ARCA5_OFFC                           Fld(4, 20) //[23:20]

#define DDRPHY_CA_CMD1                              (DDRPHY_BASE_ADDR + 0x0019C)
    #define CA_CMD1_RG_RX_ARCS0_OFFC                           Fld(4, 0) //[3:0]
    #define CA_CMD1_RG_RX_ARCS1_OFFC                           Fld(4, 4) //[7:4]
    #define CA_CMD1_RG_RX_ARCS2_OFFC                           Fld(4, 8) //[11:8]
    #define CA_CMD1_RG_RX_ARCKE0_OFFC                          Fld(4, 12) //[15:12]
    #define CA_CMD1_RG_RX_ARCKE1_OFFC                          Fld(4, 16) //[19:16]
    #define CA_CMD1_RG_RX_ARCKE2_OFFC                          Fld(4, 20) //[23:20]

#define DDRPHY_CA_CMD2                              (DDRPHY_BASE_ADDR + 0x001A0)
    #define CA_CMD2_RG_RX_ARCA6_OFFC                           Fld(4, 0) //[3:0]
    #define CA_CMD2_RG_RX_ARCA7_OFFC                           Fld(4, 4) //[7:4]
    #define CA_CMD2_RG_RX_ARCA8_OFFC                           Fld(4, 8) //[11:8]
    #define CA_CMD2_RG_RX_ARCA9_OFFC                           Fld(4, 12) //[15:12]
    #define CA_CMD2_RG_RX_ARCA10_OFFC                          Fld(4, 16) //[19:16]

#define DDRPHY_CA_CMD3                              (DDRPHY_BASE_ADDR + 0x001A4)
    #define CA_CMD3_RG_ARCMD_ATPG_EN                           Fld(1, 0) //[0:0]
    #define CA_CMD3_RG_RX_ARCMD_SMT_EN                         Fld(1, 1) //[1:1]
    #define CA_CMD3_RG_TX_ARCMD_EN                             Fld(1, 2) //[2:2]
    #define CA_CMD3_RG_ARCMD_RESETB                            Fld(1, 3) //[3:3]
    #define CA_CMD3_RG_RX_ARCLK_IN_BUFF_EN                     Fld(1, 5) //[5:5]
    #define CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN                     Fld(1, 7) //[7:7]
    #define CA_CMD3_RG_RX_ARCMD_STBENCMP_EN                    Fld(1, 10) //[10:10]
    #define CA_CMD3_RG_RX_ARCMD_OFFC_EN                        Fld(1, 11) //[11:11]
    #define CA_CMD3_RG_RX_ARCLK_SWAP_EN                        Fld(1, 15) //[15:15]
    #define CA_CMD3_RG_TX_ARCLK_ODTEN_DIS                      Fld(1, 16) //[16:16]
    #define CA_CMD3_RG_TX_ARCLK_OE_DIS                         Fld(1, 17) //[17:17]
    #define CA_CMD3_RG_TX_ARCMD_ODTEN_DIS                      Fld(1, 20) //[20:20]
    #define CA_CMD3_RG_TX_ARCMD_OE_DIS                         Fld(1, 21) //[21:21]

#define DDRPHY_CA_CMD4                              (DDRPHY_BASE_ADDR + 0x001A8)
    #define CA_CMD4_RG_RX_ARCLK_EYE_R_DLY                      Fld(7, 0) //[6:0]
    #define CA_CMD4_RG_RX_ARCLK_EYE_F_DLY                      Fld(7, 8) //[14:8]
    #define CA_CMD4_RG_RX_ARCMD_EYE_R_DLY                      Fld(6, 16) //[21:16]
    #define CA_CMD4_RG_RX_ARCMD_EYE_F_DLY                      Fld(6, 24) //[29:24]

#define DDRPHY_CA_CMD5                              (DDRPHY_BASE_ADDR + 0x001AC)
    #define CA_CMD5_CA_CMD5_RFU                                Fld(8, 0) //[7:0]
    #define CA_CMD5_RG_RX_ARCMD_EYE_VREF_SEL                   Fld(6, 8) //[13:8]
    #define CA_CMD5_RG_RX_ARCMD_VREF_EN                        Fld(1, 16) //[16:16]
    #define CA_CMD5_RG_RX_ARCMD_EYE_VREF_EN                    Fld(1, 17) //[17:17]
    #define CA_CMD5_RG_RX_ARCMD_EYE_SEL                        Fld(4, 20) //[23:20]
    #define CA_CMD5_RG_RX_ARCMD_EYE_EN                         Fld(1, 24) //[24:24]
    #define CA_CMD5_RG_RX_ARCMD_EYE_STBEN_RESETB               Fld(1, 25) //[25:25]
    #define CA_CMD5_RG_RX_ARCLK_DVS_EN                         Fld(1, 31) //[31:31]

#define DDRPHY_CA_CMD6                              (DDRPHY_BASE_ADDR + 0x001B0)
    #define CA_CMD6_RG_RX_ARCMD_BIAS_PS                        Fld(2, 0) //[1:0]
    #define CA_CMD6_RG_TX_ARCMD_OE_EXT_DIS                     Fld(1, 2) //[2:2]
    #define CA_CMD6_RG_TX_ARCMD_ODTEN_EXT_DIS                  Fld(1, 3) //[3:3]
    #define CA_CMD6_RG_RX_ARCMD_RPRE_TOG_EN                    Fld(1, 5) //[5:5]
    #define CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN                    Fld(1, 6) //[6:6]
    #define CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN                  Fld(1, 7) //[7:7]
    #define CA_CMD6_RG_RX_ARCMD_LPBK_EN                        Fld(1, 8) //[8:8]
    #define CA_CMD6_RG_RX_ARCMD_O1_SEL                         Fld(1, 9) //[9:9]
    #define CA_CMD6_RG_RX_ARCMD_JM_SEL                         Fld(1, 11) //[11:11]
    #define CA_CMD6_RG_RX_ARCMD_BIAS_EN                        Fld(1, 12) //[12:12]
    #define CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL                  Fld(2, 14) //[15:14]
    #define CA_CMD6_RG_RX_ARCMD_DDR4_SEL                       Fld(1, 16) //[16:16]
    #define CA_CMD6_RG_TX_ARCMD_DDR4_SEL                       Fld(1, 17) //[17:17]
    #define CA_CMD6_RG_RX_ARCMD_DDR3_SEL                       Fld(1, 18) //[18:18]
    #define CA_CMD6_RG_TX_ARCMD_DDR3_SEL                       Fld(1, 19) //[19:19]
    #define CA_CMD6_RG_RX_ARCMD_EYE_DLY_DQS_BYPASS             Fld(1, 24) //[24:24]
    #define CA_CMD6_RG_RX_ARCMD_EYE_OE_GATE_EN                 Fld(1, 28) //[28:28]
    #define CA_CMD6_RG_RX_ARCMD_DMRANK_OUTSEL                  Fld(1, 31) //[31:31]

#define DDRPHY_CA_CMD7                              (DDRPHY_BASE_ADDR + 0x001B4)
    #define CA_CMD7_RG_TX_ARCLKB_PULL_DN                       Fld(1, 0) //[0:0]
    #define CA_CMD7_RG_TX_ARCLKB_PULL_UP                       Fld(1, 1) //[1:1]
    #define CA_CMD7_RG_TX_ARCLK_PULL_DN                        Fld(1, 2) //[2:2]
    #define CA_CMD7_RG_TX_ARCLK_PULL_UP                        Fld(1, 3) //[3:3]
    #define CA_CMD7_RG_TX_ARCS_PULL_DN                         Fld(1, 4) //[4:4]
    #define CA_CMD7_RG_TX_ARCS_PULL_UP                         Fld(1, 5) //[5:5]
    #define CA_CMD7_RG_TX_ARCMD_PULL_DN                        Fld(1, 6) //[6:6]
    #define CA_CMD7_RG_TX_ARCMD_PULL_UP                        Fld(1, 7) //[7:7]
    #define CA_CMD7_RG_TX_ARCLKB_PULL_DN_LP4Y                  Fld(1, 16) //[16:16]

#define DDRPHY_CA_CMD8                              (DDRPHY_BASE_ADDR + 0x001B8)
    #define CA_CMD8_RG_RRESETB_DRVP                            Fld(5, 0) //[4:0]
    #define CA_CMD8_RG_RRESETB_DRVN                            Fld(5, 8) //[12:8]
    #define CA_CMD8_RG_RX_RRESETB_SMT_EN                       Fld(1, 16) //[16:16]
    #define CA_CMD8_RG_TX_RRESETB_SCAN_IN_EN                   Fld(1, 17) //[17:17]
    #define CA_CMD8_RG_TX_RRESETB_DDR4_SEL                     Fld(1, 18) //[18:18]
    #define CA_CMD8_RG_TX_RRESETB_DDR3_SEL                     Fld(1, 19) //[19:19]
    #define CA_CMD8_RG_TX_RRESETB_PULL_DN                      Fld(1, 20) //[20:20]
    #define CA_CMD8_RG_TX_RRESETB_PULL_UP                      Fld(1, 21) //[21:21]

#define DDRPHY_CA_CMD9                              (DDRPHY_BASE_ADDR + 0x001BC)
    #define CA_CMD9_RG_TX_ARCMD_EN_LP4P                        Fld(1, 0) //[0:0]
    #define CA_CMD9_RG_TX_ARCMD_EN_CAP_LP4P                    Fld(1, 1) //[1:1]
    #define CA_CMD9_RG_TX_ARCMD_CAP_DET                        Fld(1, 2) //[2:2]
    #define CA_CMD9_RG_TX_ARCMD_CKE_MCK4X_SEL                  Fld(2, 3) //[4:3]
    #define CA_CMD9_RG_ARPI_TX_CG_CS_EN                        Fld(1, 5) //[5:5]
    #define CA_CMD9_RG_ARPI_TX_CG_CA_EN                        Fld(1, 6) //[6:6]
    #define CA_CMD9_RG_ARPI_TX_CG_CLK_EN                       Fld(1, 7) //[7:7]
    #define CA_CMD9_RG_RX_ARCLK_DQSIEN_BURST_E1_EN             Fld(1, 8) //[8:8]
    #define CA_CMD9_RG_RX_ARCLK_DQSIEN_BURST_E2_EN             Fld(1, 9) //[9:9]
    #define CA_CMD9_RG_RX_ARCLK_DQSSTB_CG_EN                   Fld(1, 10) //[10:10]
    #define CA_CMD9_RG_RX_ARCLK_GATE_EN_MODE                   Fld(1, 12) //[12:12]
    #define CA_CMD9_RG_RX_ARCLK_SER_RST_MODE                   Fld(1, 13) //[13:13]
    #define CA_CMD9_RG_ARDLL_RESETB_CA                         Fld(1, 15) //[15:15]
    #define CA_CMD9_RG_TX_ARCMD_LP3_CKE_SEL                    Fld(1, 16) //[16:16]
    #define CA_CMD9_RG_TX_ARCMD_LP4_CKE_SEL                    Fld(1, 17) //[17:17]
    #define CA_CMD9_RG_TX_ARCMD_LP4X_CKE_SEL                   Fld(1, 18) //[18:18]
    #define CA_CMD9_RG_TX_ARCMD_LSH_DQM_CG_EN                  Fld(1, 20) //[20:20]
    #define CA_CMD9_RG_TX_ARCMD_LSH_DQS_CG_EN                  Fld(1, 21) //[21:21]
    #define CA_CMD9_RG_TX_ARCMD_LSH_DQ_CG_EN                   Fld(1, 22) //[22:22]
    #define CA_CMD9_RG_TX_ARCMD_OE_SUS_EN                      Fld(1, 24) //[24:24]
    #define CA_CMD9_RG_TX_ARCMD_ODTEN_OE_SUS_EN                Fld(1, 25) //[25:25]

#define DDRPHY_CA_CMD10                             (DDRPHY_BASE_ADDR + 0x001C0)
    #define CA_CMD10_RG_RX_ARCMD_STBEN_RESETB                  Fld(1, 0) //[0:0]
    #define CA_CMD10_RG_RX_ARCLK_STBEN_RESETB                  Fld(1, 4) //[4:4]
    #define CA_CMD10_RG_RX_ARCLK_DQSIENMODE                    Fld(1, 5) //[5:5]
    #define CA_CMD10_R_DMRXFIFO_STBENCMP_EN_CA                 Fld(1, 7) //[7:7]
    #define CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA                   Fld(8, 8) //[15:8]
    #define CA_CMD10_R_DMDQSIEN_VALID_LAT_CA                   Fld(3, 16) //[18:16]
    #define CA_CMD10_R_DMDQSIEN_RDSEL_LAT_CA                   Fld(3, 20) //[22:20]
    #define CA_CMD10_R_DMRXDVS_VALID_LAT_CA                    Fld(3, 24) //[26:24]
    #define CA_CMD10_R_DMRXDVS_RDSEL_LAT_CA                    Fld(3, 28) //[30:28]

#define DDRPHY_CA_CMD11                             (DDRPHY_BASE_ADDR + 0x001C4)
    #define CA_CMD11_RG_RX_ARCA0_DQS_SEL                       Fld(1, 0) //[0:0]
    #define CA_CMD11_RG_RX_ARCA1_DQS_SEL                       Fld(1, 1) //[1:1]
    #define CA_CMD11_RG_RX_ARCA2_DQS_SEL                       Fld(1, 2) //[2:2]
    #define CA_CMD11_RG_RX_ARCA3_DQS_SEL                       Fld(1, 3) //[3:3]
    #define CA_CMD11_RG_RX_ARCA4_DQS_SEL                       Fld(1, 4) //[4:4]
    #define CA_CMD11_RG_RX_ARCA5_DQS_SEL                       Fld(1, 5) //[5:5]
    #define CA_CMD11_RG_RX_ARCA6_DQS_SEL                       Fld(1, 6) //[6:6]
    #define CA_CMD11_RG_RX_ARCA7_DQS_SEL                       Fld(1, 7) //[7:7]
    #define CA_CMD11_RG_RX_ARCA8_DQS_SEL                       Fld(1, 8) //[8:8]
    #define CA_CMD11_RG_RX_ARCA9_DQS_SEL                       Fld(1, 9) //[9:9]
    #define CA_CMD11_RG_RX_ARCA10_DQS_SEL                      Fld(1, 10) //[10:10]
    #define CA_CMD11_RG_RX_ARCKE0_DQS_SEL                      Fld(1, 11) //[11:11]
    #define CA_CMD11_RG_RX_ARCKE1_DQS_SEL                      Fld(1, 12) //[12:12]
    #define CA_CMD11_RG_RX_ARCS0_DQS_SEL                       Fld(1, 13) //[13:13]
    #define CA_CMD11_RG_RX_ARCS1_DQS_SEL                       Fld(1, 14) //[14:14]
    #define CA_CMD11_RG_TX_ARCA0_MCK4X_SEL                     Fld(1, 15) //[15:15]
    #define CA_CMD11_RG_TX_ARCA1_MCK4X_SEL                     Fld(1, 16) //[16:16]
    #define CA_CMD11_RG_TX_ARCA2_MCK4X_SEL                     Fld(1, 17) //[17:17]
    #define CA_CMD11_RG_TX_ARCA3_MCK4X_SEL                     Fld(1, 18) //[18:18]
    #define CA_CMD11_RG_TX_ARCA4_MCK4X_SEL                     Fld(1, 19) //[19:19]
    #define CA_CMD11_RG_TX_ARCA5_MCK4X_SEL                     Fld(1, 20) //[20:20]
    #define CA_CMD11_RG_TX_ARCA6_MCK4X_SEL                     Fld(1, 21) //[21:21]
    #define CA_CMD11_RG_TX_ARCA7_MCK4X_SEL                     Fld(1, 22) //[22:22]
    #define CA_CMD11_RG_TX_ARCA8_MCK4X_SEL                     Fld(1, 23) //[23:23]
    #define CA_CMD11_RG_TX_ARCA9_MCK4X_SEL                     Fld(1, 24) //[24:24]
    #define CA_CMD11_RG_TX_ARCA10_MCK4X_SEL                    Fld(1, 25) //[25:25]
    #define CA_CMD11_RG_TX_ARCKE0_MCK4X_SEL                    Fld(1, 26) //[26:26]
    #define CA_CMD11_RG_TX_ARCKE1_MCK4X_SEL                    Fld(1, 27) //[27:27]
    #define CA_CMD11_RG_TX_ARCS0_MCK4X_SEL                     Fld(1, 28) //[28:28]
    #define CA_CMD11_RG_TX_ARCS1_MCK4X_SEL                     Fld(1, 29) //[29:29]
    #define CA_CMD11_RG_TX_ARCLK_MCK4X_SEL                     Fld(1, 30) //[30:30]
    #define CA_CMD11_RG_TX_ARCLKB_MCK4X_SEL                    Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X1C8                            (DDRPHY_BASE_ADDR + 0x001C8)
    #define RFU_0X1C8_RESERVED_0X1C8                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X1CC                            (DDRPHY_BASE_ADDR + 0x001CC)
    #define RFU_0X1CC_RESERVED_0X1CC                           Fld(32, 0) //[31:0]

#define DDRPHY_CA_TX_MCK                            (DDRPHY_BASE_ADDR + 0x001D0)
    #define CA_TX_MCK_R_DM_TX_MCK_FRUN_CA                      Fld(13, 0) //[12:0]
    #define CA_TX_MCK_R_DMRESETB_DRVP_FRPHY                    Fld(5, 21) //[25:21]
    #define CA_TX_MCK_R_DMRESETB_DRVN_FRPHY                    Fld(5, 26) //[30:26]
    #define CA_TX_MCK_R_DMRESET_FRPHY_OPT                      Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X1D4                            (DDRPHY_BASE_ADDR + 0x001D4)
    #define RFU_0X1D4_RESERVED_0X1D4                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X1D8                            (DDRPHY_BASE_ADDR + 0x001D8)
    #define RFU_0X1D8_RESERVED_0X1D8                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X1DC                            (DDRPHY_BASE_ADDR + 0x001DC)
    #define RFU_0X1DC_RESERVED_0X1DC                           Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CG_CTRL8                        (DDRPHY_BASE_ADDR + 0x001FC)
    #define MISC_CG_CTRL8_R_LBK_CG_CTRL_CA                     Fld(32, 0) //[31:0]

#define DDRPHY_MISC_EXTLB0                          (DDRPHY_BASE_ADDR + 0x00200)
    #define MISC_EXTLB0_R_EXTLB_LFSR_INI_1                     Fld(16, 16) //[31:16]
    #define MISC_EXTLB0_R_EXTLB_LFSR_INI_0                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB1                          (DDRPHY_BASE_ADDR + 0x00204)
    #define MISC_EXTLB1_R_EXTLB_LFSR_INI_3                     Fld(16, 16) //[31:16]
    #define MISC_EXTLB1_R_EXTLB_LFSR_INI_2                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB2                          (DDRPHY_BASE_ADDR + 0x00208)
    #define MISC_EXTLB2_R_EXTLB_LFSR_INI_5                     Fld(16, 16) //[31:16]
    #define MISC_EXTLB2_R_EXTLB_LFSR_INI_4                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB3                          (DDRPHY_BASE_ADDR + 0x0020C)
    #define MISC_EXTLB3_R_EXTLB_LFSR_INI_7                     Fld(16, 16) //[31:16]
    #define MISC_EXTLB3_R_EXTLB_LFSR_INI_6                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB4                          (DDRPHY_BASE_ADDR + 0x00210)
    #define MISC_EXTLB4_R_EXTLB_LFSR_INI_9                     Fld(16, 16) //[31:16]
    #define MISC_EXTLB4_R_EXTLB_LFSR_INI_8                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB5                          (DDRPHY_BASE_ADDR + 0x00214)
    #define MISC_EXTLB5_R_EXTLB_LFSR_INI_11                    Fld(16, 16) //[31:16]
    #define MISC_EXTLB5_R_EXTLB_LFSR_INI_10                    Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB6                          (DDRPHY_BASE_ADDR + 0x00218)
    #define MISC_EXTLB6_R_EXTLB_LFSR_INI_13                    Fld(16, 16) //[31:16]
    #define MISC_EXTLB6_R_EXTLB_LFSR_INI_12                    Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB7                          (DDRPHY_BASE_ADDR + 0x0021C)
    #define MISC_EXTLB7_R_EXTLB_LFSR_INI_15                    Fld(16, 16) //[31:16]
    #define MISC_EXTLB7_R_EXTLB_LFSR_INI_14                    Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB8                          (DDRPHY_BASE_ADDR + 0x00220)
    #define MISC_EXTLB8_R_EXTLB_LFSR_INI_17                    Fld(16, 16) //[31:16]
    #define MISC_EXTLB8_R_EXTLB_LFSR_INI_16                    Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB9                          (DDRPHY_BASE_ADDR + 0x00224)
    #define MISC_EXTLB9_R_EXTLB_LFSR_INI_19                    Fld(16, 16) //[31:16]
    #define MISC_EXTLB9_R_EXTLB_LFSR_INI_18                    Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB10                         (DDRPHY_BASE_ADDR + 0x00228)
    #define MISC_EXTLB10_R_EXTLB_LFSR_INI_21                   Fld(16, 16) //[31:16]
    #define MISC_EXTLB10_R_EXTLB_LFSR_INI_20                   Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB11                         (DDRPHY_BASE_ADDR + 0x0022C)
    #define MISC_EXTLB11_R_EXTLB_LFSR_INI_23                   Fld(16, 16) //[31:16]
    #define MISC_EXTLB11_R_EXTLB_LFSR_INI_22                   Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB12                         (DDRPHY_BASE_ADDR + 0x00230)
    #define MISC_EXTLB12_R_EXTLB_LFSR_INI_25                   Fld(16, 16) //[31:16]
    #define MISC_EXTLB12_R_EXTLB_LFSR_INI_24                   Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB13                         (DDRPHY_BASE_ADDR + 0x00234)
    #define MISC_EXTLB13_R_EXTLB_LFSR_INI_27                   Fld(16, 16) //[31:16]
    #define MISC_EXTLB13_R_EXTLB_LFSR_INI_26                   Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB14                         (DDRPHY_BASE_ADDR + 0x00238)
    #define MISC_EXTLB14_R_EXTLB_LFSR_INI_29                   Fld(16, 16) //[31:16]
    #define MISC_EXTLB14_R_EXTLB_LFSR_INI_28                   Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB15                         (DDRPHY_BASE_ADDR + 0x0023C)
    #define MISC_EXTLB15_R_EXTLB_LFSR_INI_30                   Fld(16, 0) //[15:0]
    #define MISC_EXTLB15_MISC_EXTLB15_RFU                      Fld(16, 16) //[31:16]

#define DDRPHY_MISC_EXTLB16                         (DDRPHY_BASE_ADDR + 0x00240)
    #define MISC_EXTLB16_R_EXTLB_ODTEN_DQS1_ON                 Fld(1, 27) //[27:27]
    #define MISC_EXTLB16_R_EXTLB_ODTEN_DQM1_ON                 Fld(1, 26) //[26:26]
    #define MISC_EXTLB16_R_EXTLB_ODTEN_DQB1_ON                 Fld(1, 25) //[25:25]
    #define MISC_EXTLB16_R_EXTLB_ODTEN_DQS0_ON                 Fld(1, 24) //[24:24]
    #define MISC_EXTLB16_R_EXTLB_ODTEN_DQM0_ON                 Fld(1, 23) //[23:23]
    #define MISC_EXTLB16_R_EXTLB_ODTEN_DQB0_ON                 Fld(1, 22) //[22:22]
    #define MISC_EXTLB16_R_EXTLB_OE_DQS1_ON                    Fld(1, 21) //[21:21]
    #define MISC_EXTLB16_R_EXTLB_OE_DQM1_ON                    Fld(1, 20) //[20:20]
    #define MISC_EXTLB16_R_EXTLB_OE_DQB1_ON                    Fld(1, 19) //[19:19]
    #define MISC_EXTLB16_R_EXTLB_OE_DQS0_ON                    Fld(1, 18) //[18:18]
    #define MISC_EXTLB16_R_EXTLB_OE_DQM0_ON                    Fld(1, 17) //[17:17]
    #define MISC_EXTLB16_R_EXTLB_OE_DQB0_ON                    Fld(1, 16) //[16:16]
    #define MISC_EXTLB16_R_EXTLB_LFSR_TAP                      Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB17                         (DDRPHY_BASE_ADDR + 0x00244)
    #define MISC_EXTLB17_R_EXTLB_RX_LENGTH_M1                  Fld(24, 8) //[31:8]
    #define MISC_EXTLB17_R_EXTLB_TX_PRE_ON                     Fld(1, 7) //[7:7]
    #define MISC_EXTLB17_R_INTLB_DRDF_CA_MUXSEL                Fld(1, 5) //[5:5]
    #define MISC_EXTLB17_R_INTLB_ARCLK_MUXSEL                  Fld(1, 4) //[4:4]
    #define MISC_EXTLB17_R_EXTLB_TX_EN_OTHERCH_SEL             Fld(1, 3) //[3:3]
    #define MISC_EXTLB17_R_EXTLB_TX_EN                         Fld(1, 2) //[2:2]
    #define MISC_EXTLB17_R_EXTLB_RX_SWRST                      Fld(1, 1) //[1:1]
    #define MISC_EXTLB17_R_EXTLB                               Fld(1, 0) //[0:0]

#define DDRPHY_MISC_EXTLB18                         (DDRPHY_BASE_ADDR + 0x00248)
    #define MISC_EXTLB18_R_TX_EN_SRC_SEL                       Fld(1, 0) //[0:0]
    #define MISC_EXTLB18_R_OTH_TX_EN_SRC_SEL                   Fld(1, 1) //[1:1]
    #define MISC_EXTLB18_R_LPBK_DQ_MODE_FOR_CA                 Fld(1, 3) //[3:3]
    #define MISC_EXTLB18_R_LPBK_DQ_TX_MODE                     Fld(1, 4) //[4:4]
    #define MISC_EXTLB18_R_LPBK_CA_TX_MODE                     Fld(1, 5) //[5:5]
    #define MISC_EXTLB18_R_LPBK_DQ_RX_MODE                     Fld(1, 8) //[8:8]
    #define MISC_EXTLB18_R_LPBK_CA_RX_MODE                     Fld(1, 9) //[9:9]
    #define MISC_EXTLB18_R_TX_TRIG_SRC_SEL                     Fld(4, 16) //[19:16]
    #define MISC_EXTLB18_R_OTH_TX_TRIG_SRC_SEL                 Fld(4, 20) //[23:20]
    #define MISC_EXTLB18_R_EXTLB_OE_DQB2_ON                    Fld(1, 24) //[24:24]
    #define MISC_EXTLB18_R_EXTLB_OE_DQM2_ON                    Fld(1, 25) //[25:25]
    #define MISC_EXTLB18_R_EXTLB_OE_DQS2_ON                    Fld(1, 26) //[26:26]
    #define MISC_EXTLB18_R_EXTLB_ODTEN_DQB2_ON                 Fld(1, 27) //[27:27]
    #define MISC_EXTLB18_R_EXTLB_ODTEN_DQM2_ON                 Fld(1, 28) //[28:28]
    #define MISC_EXTLB18_R_EXTLB_ODTEN_DQS2_ON                 Fld(1, 29) //[29:29]

#define DDRPHY_MISC_EXTLB19                         (DDRPHY_BASE_ADDR + 0x0024C)
    #define MISC_EXTLB19_R_EXTLB_LFSR_ENABLE                   Fld(1, 0) //[0:0]
    #define MISC_EXTLB19_R_EXTLB_SSO_ENABLE                    Fld(1, 1) //[1:1]
    #define MISC_EXTLB19_R_EXTLB_XTALK_ENABLE                  Fld(1, 2) //[2:2]
    #define MISC_EXTLB19_R_EXTLB_LEADLAG_DBG_ENABLE            Fld(1, 3) //[3:3]
    #define MISC_EXTLB19_R_EXTLB_DBG_SEL                       Fld(6, 16) //[21:16]
    #define MISC_EXTLB19_R_LPBK_DC_TOG_MODE                    Fld(1, 23) //[23:23]
    #define MISC_EXTLB19_R_LPBK_DC_TOG_TIMER                   Fld(8, 24) //[31:24]

#define DDRPHY_MISC_EXTLB20                         (DDRPHY_BASE_ADDR + 0x00250)
    #define MISC_EXTLB20_R_XTALK_TX_00_TOG_CYCLE               Fld(4, 0) //[3:0]
    #define MISC_EXTLB20_R_XTALK_TX_01_TOG_CYCLE               Fld(4, 4) //[7:4]
    #define MISC_EXTLB20_R_XTALK_TX_02_TOG_CYCLE               Fld(4, 8) //[11:8]
    #define MISC_EXTLB20_R_XTALK_TX_03_TOG_CYCLE               Fld(4, 12) //[15:12]
    #define MISC_EXTLB20_R_XTALK_TX_04_TOG_CYCLE               Fld(4, 16) //[19:16]
    #define MISC_EXTLB20_R_XTALK_TX_05_TOG_CYCLE               Fld(4, 20) //[23:20]
    #define MISC_EXTLB20_R_XTALK_TX_06_TOG_CYCLE               Fld(4, 24) //[27:24]
    #define MISC_EXTLB20_R_XTALK_TX_07_TOG_CYCLE               Fld(4, 28) //[31:28]

#define DDRPHY_MISC_EXTLB21                         (DDRPHY_BASE_ADDR + 0x00254)
    #define MISC_EXTLB21_R_XTALK_TX_08_TOG_CYCLE               Fld(4, 0) //[3:0]
    #define MISC_EXTLB21_R_XTALK_TX_09_TOG_CYCLE               Fld(4, 4) //[7:4]
    #define MISC_EXTLB21_R_XTALK_TX_10_TOG_CYCLE               Fld(4, 8) //[11:8]
    #define MISC_EXTLB21_R_XTALK_TX_11_TOG_CYCLE               Fld(4, 12) //[15:12]
    #define MISC_EXTLB21_R_XTALK_TX_12_TOG_CYCLE               Fld(4, 16) //[19:16]
    #define MISC_EXTLB21_R_XTALK_TX_13_TOG_CYCLE               Fld(4, 20) //[23:20]
    #define MISC_EXTLB21_R_XTALK_TX_14_TOG_CYCLE               Fld(4, 24) //[27:24]
    #define MISC_EXTLB21_R_XTALK_TX_15_TOG_CYCLE               Fld(4, 28) //[31:28]

#define DDRPHY_MISC_EXTLB22                         (DDRPHY_BASE_ADDR + 0x00258)
    #define MISC_EXTLB22_R_XTALK_TX_16_TOG_CYCLE               Fld(4, 0) //[3:0]
    #define MISC_EXTLB22_R_XTALK_TX_17_TOG_CYCLE               Fld(4, 4) //[7:4]
    #define MISC_EXTLB22_R_XTALK_TX_18_TOG_CYCLE               Fld(4, 8) //[11:8]
    #define MISC_EXTLB22_R_XTALK_TX_19_TOG_CYCLE               Fld(4, 12) //[15:12]
    #define MISC_EXTLB22_R_XTALK_TX_20_TOG_CYCLE               Fld(4, 16) //[19:16]
    #define MISC_EXTLB22_R_XTALK_TX_21_TOG_CYCLE               Fld(4, 20) //[23:20]
    #define MISC_EXTLB22_R_XTALK_TX_22_TOG_CYCLE               Fld(4, 24) //[27:24]
    #define MISC_EXTLB22_R_XTALK_TX_23_TOG_CYCLE               Fld(4, 28) //[31:28]

#define DDRPHY_MISC_EXTLB23                         (DDRPHY_BASE_ADDR + 0x0025C)
    #define MISC_EXTLB23_R_XTALK_TX_24_TOG_CYCLE               Fld(4, 0) //[3:0]
    #define MISC_EXTLB23_R_XTALK_TX_25_TOG_CYCLE               Fld(4, 4) //[7:4]
    #define MISC_EXTLB23_R_XTALK_TX_26_TOG_CYCLE               Fld(4, 8) //[11:8]
    #define MISC_EXTLB23_R_XTALK_TX_27_TOG_CYCLE               Fld(4, 12) //[15:12]
    #define MISC_EXTLB23_R_XTALK_TX_28_TOG_CYCLE               Fld(4, 16) //[19:16]
    #define MISC_EXTLB23_R_XTALK_TX_29_TOG_CYCLE               Fld(4, 20) //[23:20]
    #define MISC_EXTLB23_R_XTALK_TX_30_TOG_CYCLE               Fld(4, 24) //[27:24]
    #define MISC_EXTLB23_R_XTALK_TX_31_TOG_CYCLE               Fld(4, 28) //[31:28]

#define DDRPHY_DVFS_EMI_CLK                         (DDRPHY_BASE_ADDR + 0x00260)
    #define DVFS_EMI_CLK_R_DDRPHY_SHUFFLE_MUX_ENABLE           Fld(1, 23) //[23:23]
    #define DVFS_EMI_CLK_RG_DLL_SHUFFLE                        Fld(1, 24) //[24:24]
    #define DVFS_EMI_CLK_RG_52M_104M_SEL                       Fld(1, 29) //[29:29]
    #define DVFS_EMI_CLK_RG_GATING_EMI_NEW                     Fld(2, 30) //[31:30]

#define DDRPHY_MISC_VREF_CTRL                       (DDRPHY_BASE_ADDR + 0x00264)
    #define MISC_VREF_CTRL_PHY_SPM_CTL4                        Fld(16, 0) //[15:0]
    #define MISC_VREF_CTRL_MISC_LP_8X_MUX                      Fld(8, 16) //[23:16]
    #define MISC_VREF_CTRL_MISC_LP_DDR400_MUX                  Fld(4, 24) //[27:24]
    #define MISC_VREF_CTRL_VREF_CTRL_RFU                       Fld(3, 28) //[30:28]
    #define MISC_VREF_CTRL_RG_RVREF_VREF_EN                    Fld(1, 31) //[31:31]

#define DDRPHY_MISC_IMP_CTRL0                       (DDRPHY_BASE_ADDR + 0x00268)
    #define MISC_IMP_CTRL0_RG_IMP_OCD_PUCMP_EN                 Fld(1, 3) //[3:3]
    #define MISC_IMP_CTRL0_RG_IMP_EN                           Fld(1, 4) //[4:4]
    #define MISC_IMP_CTRL0_RG_RIMP_DDR4_SEL                    Fld(1, 5) //[5:5]
    #define MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL                    Fld(1, 6) //[6:6]

#define DDRPHY_MISC_IMP_CTRL1                       (DDRPHY_BASE_ADDR + 0x0026C)
    #define MISC_IMP_CTRL1_RG_RIMP_BIAS_EN                     Fld(1, 4) //[4:4]
    #define MISC_IMP_CTRL1_RG_RIMP_ODT_EN                      Fld(1, 5) //[5:5]
    #define MISC_IMP_CTRL1_RG_RIMP_PRE_EN                      Fld(1, 6) //[6:6]
    #define MISC_IMP_CTRL1_RG_RIMP_VREF_EN                     Fld(1, 7) //[7:7]
    #define MISC_IMP_CTRL1_RG_RIMP_DRV05                       Fld(1, 16) //[16:16]
    #define MISC_IMP_CTRL1_RG_RIMP_SUS_ECO_OPT                 Fld(1, 31) //[31:31]

#define DDRPHY_MISC_SHU_OPT                         (DDRPHY_BASE_ADDR + 0x00270)
    #define MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN   Fld(1, 0) //[0:0]
    #define MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN               Fld(2, 2) //[3:2]
    #define MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN   Fld(1, 8) //[8:8]
    #define MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN               Fld(2, 10) //[11:10]
    #define MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN                 Fld(2, 18) //[19:18]
    #define MISC_SHU_OPT_R_DQB2_SHU_PHY_GATING_RESETB_SPM_EN   Fld(1, 14) //[14:14]
    #define MISC_SHU_OPT_R_DQB2_SHU_PHDET_SPM_EN               Fld(2, 16) //[17:16]

#define DDRPHY_MISC_SPM_CTRL0                       (DDRPHY_BASE_ADDR + 0x00274)
    #define MISC_SPM_CTRL0_PHY_SPM_CTL0                        Fld(32, 0) //[31:0]

#define DDRPHY_MISC_SPM_CTRL1                       (DDRPHY_BASE_ADDR + 0x00278)
    #define MISC_SPM_CTRL1_RG_ARDMSUS_10                       Fld(1, 0) //[0:0]
    #define MISC_SPM_CTRL1_RG_ARDMSUS_10_B0                    Fld(1, 1) //[1:1]
    #define MISC_SPM_CTRL1_RG_ARDMSUS_10_B1                    Fld(1, 2) //[2:2]
    #define MISC_SPM_CTRL1_RG_ARDMSUS_10_CA                    Fld(1, 3) //[3:3]
    #define MISC_SPM_CTRL1_RG_ARDMSUS_10_B2                    Fld(1, 4) //[4:4]
    #define MISC_SPM_CTRL1_SPM_DVFS_CONTROL_SEL                Fld(1, 16) //[16:16]
    #define MISC_SPM_CTRL1_RG_DR_SHU_LEVEL                     Fld(2, 17) //[18:17]
    #define MISC_SPM_CTRL1_RG_PHYPLL_SHU_EN                    Fld(1, 19) //[19:19]
    #define MISC_SPM_CTRL1_RG_PHYPLL2_SHU_EN                   Fld(1, 20) //[20:20]
    #define MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW                   Fld(1, 21) //[21:21]
    #define MISC_SPM_CTRL1_RG_PHYPLL2_MODE_SW                  Fld(1, 22) //[22:22]
    #define MISC_SPM_CTRL1_RG_DR_SHORT_QUEUE                   Fld(1, 23) //[23:23]
    #define MISC_SPM_CTRL1_RG_DR_SHU_EN                        Fld(1, 24) //[24:24]
    #define MISC_SPM_CTRL1_RG_DDRPHY_DB_CK_CH0_EN              Fld(1, 25) //[25:25]
    #define MISC_SPM_CTRL1_RG_DDRPHY_DB_CK_CH1_EN              Fld(1, 26) //[26:26]

#define DDRPHY_MISC_SPM_CTRL2                       (DDRPHY_BASE_ADDR + 0x0027C)
    #define MISC_SPM_CTRL2_PHY_SPM_CTL2                        Fld(32, 0) //[31:0]

#define DDRPHY_MISC_SPM_CTRL3                       (DDRPHY_BASE_ADDR + 0x00280)
    #define MISC_SPM_CTRL3_RG_TX_TRACKING_DIS_CHA              Fld(1, 0) //[0:0]
    #define MISC_SPM_CTRL3_RG_TX_TRACKING_DIS_CHB              Fld(1, 1) //[1:1]
    #define MISC_SPM_CTRL3_RG_DPY_RXDLY_TRACK_EN_CH0           Fld(1, 2) //[2:2]
    #define MISC_SPM_CTRL3_RG_DPY_RXDLY_TRACK_EN_CH1           Fld(1, 3) //[3:3]
    #define MISC_SPM_CTRL3_RG_DR_SHU_LEVEL_SRAM_CH0            Fld(4, 4) //[7:4]
    #define MISC_SPM_CTRL3_RG_DR_SHU_LEVEL_SRAM_CH1            Fld(4, 8) //[11:8]
    #define MISC_SPM_CTRL3_RG_DR_SRAM_RESTORE_CH0              Fld(1, 12) //[12:12]
    #define MISC_SPM_CTRL3_RG_DR_SRAM_RESTORE_CH1              Fld(1, 13) //[13:13]
    #define MISC_SPM_CTRL3_RG_DR_SHU_LEVEL_SRAM_LATCH_CH0      Fld(1, 14) //[14:14]
    #define MISC_SPM_CTRL3_RG_DR_SHU_LEVEL_SRAM_LATCH_CH1      Fld(1, 15) //[15:15]
    #define MISC_SPM_CTRL3_RG_DPHY_RESERVED                    Fld(4, 16) //[19:16]
    #define MISC_SPM_CTRL3_RG_DPY_PRECAL_UP_CH0                Fld(1, 20) //[20:20]
    #define MISC_SPM_CTRL3_RG_DPY_PRECAL_UP_CH1                Fld(1, 21) //[21:21]
    #define MISC_SPM_CTRL3_RG_DR_SRAM_LOAD_CH0                 Fld(1, 22) //[22:22]
    #define MISC_SPM_CTRL3_RG_DR_SRAM_LOAD_CH1                 Fld(1, 23) //[23:23]
    #define MISC_SPM_CTRL3_REG_CDC_BYPASS_DBG                  Fld(1, 31) //[31:31]

#define DDRPHY_MISC_CG_CTRL0                        (DDRPHY_BASE_ADDR + 0x00284)
    #define MISC_CG_CTRL0_W_CHG_MEM                            Fld(1, 0) //[0:0]
    #define MISC_CG_CTRL0_CLK_MEM_SEL                          Fld(2, 4) //[5:4]
    #define MISC_CG_CTRL0_CLK_MEM_INV                          Fld(1, 6) //[6:6]
    #define MISC_CG_CTRL0_RG_CG_EMI_OFF_DISABLE                Fld(1, 8) //[8:8]
    #define MISC_CG_CTRL0_RG_CG_DRAMC_OFF_DISABLE              Fld(1, 9) //[9:9]
    #define MISC_CG_CTRL0_RG_CG_PHY_OFF_DISABLE                 Fld(1, 10) //[10:10]
    #define MISC_CG_CTRL0_RG_CG_COMB_OFF_DISABLE               Fld(1, 11) //[11:11]
    #define MISC_CG_CTRL0_RG_CG_CMD_OFF_DISABLE                Fld(1, 12) //[12:12]
    #define MISC_CG_CTRL0_RG_CG_COMB0_OFF_DISABLE              Fld(1, 13) //[13:13]
    #define MISC_CG_CTRL0_RG_CG_COMB1_OFF_DISABLE              Fld(1, 14) //[14:14]
    #define MISC_CG_CTRL0_RG_CG_RX_CMD_OFF_DISABLE             Fld(1, 15) //[15:15]
    #define MISC_CG_CTRL0_RG_CG_RX_COMB0_OFF_DISABLE           Fld(1, 16) //[16:16]
    #define MISC_CG_CTRL0_RG_CG_RX_COMB1_OFF_DISABLE           Fld(1, 17) //[17:17]
    #define MISC_CG_CTRL0_RG_CG_IDLE_SYNC_EN                   Fld(1, 18) //[18:18]
    #define MISC_CG_CTRL0_RG_CG_INFRA_OFF_DISABLE              Fld(1, 19) //[19:19]
    #define MISC_CG_CTRL0_RG_CG_DRAMC_CHB_CK_OFF               Fld(1, 20) //[20:20]
    #define MISC_CG_CTRL0_RG_DBG_OUT_SEL                       Fld(1, 21) //[21:21]
    #define MISC_CG_CTRL0_RG_CG_NAO_FORCE_OFF                  Fld(1, 22) //[22:22]
    #define MISC_CG_CTRL0_RG_CG_COMB2_OFF_DISABLE              Fld(1, 24) //[24:24]
    #define MISC_CG_CTRL0_RG_CG_RX_COMB2_OFF_DISABLE           Fld(1, 25) //[25:25]
    #define MISC_CG_CTRL0_RG_DA_RREF_CK_SEL                    Fld(1, 28) //[28:28]
    #define MISC_CG_CTRL0_RG_FREERUN_MCK_CG                    Fld(1, 29) //[29:29]
    #define MISC_CG_CTRL0_RG_FREERUN_MCK_CHB_SEL               Fld(1, 30) //[30:30]
    #define MISC_CG_CTRL0_CLK_MEM_DFS_CFG                      Fld(32, 0)

#define DDRPHY_MISC_CG_CTRL1                        (DDRPHY_BASE_ADDR + 0x00288)
    #define MISC_CG_CTRL1_R_DVS_DIV4_CG_CTRL                   Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CG_CTRL2                        (DDRPHY_BASE_ADDR + 0x0028C)
    #define MISC_CG_CTRL2_RG_MEM_DCM_APB_TOG                   Fld(1, 0) //[0:0]
    #define MISC_CG_CTRL2_RG_MEM_DCM_APB_SEL                   Fld(5, 1) //[5:1]
    #define MISC_CG_CTRL2_RG_MEM_DCM_FORCE_ON                  Fld(1, 6) //[6:6]
    #define MISC_CG_CTRL2_RG_MEM_DCM_DCM_EN                    Fld(1, 7) //[7:7]
    #define MISC_CG_CTRL2_RG_MEM_DCM_DBC_EN                    Fld(1, 8) //[8:8]
    #define MISC_CG_CTRL2_RG_MEM_DCM_DBC_CNT                   Fld(7, 9) //[15:9]
    #define MISC_CG_CTRL2_RG_MEM_DCM_FSEL                      Fld(5, 16) //[20:16]
    #define MISC_CG_CTRL2_RG_MEM_DCM_IDLE_FSEL                 Fld(5, 21) //[25:21]
    #define MISC_CG_CTRL2_RG_MEM_DCM_FORCE_OFF                 Fld(1, 26) //[26:26]
    #define MISC_CG_CTRL2_RG_PHY_CG_OFF_DISABLE                Fld(1, 28) //[28:28]
    #define MISC_CG_CTRL2_RG_PIPE0_CG_OFF_DISABLE              Fld(1, 29) //[29:29]
    #define MISC_CG_CTRL2_RG_MEM_DCM_CG_OFF_DISABLE            Fld(1, 31) //[31:31]
    #define MISC_CG_CTRL2_RG_MEM_DCM_CTL                       Fld(32, 0)

#define DDRPHY_MISC_CG_CTRL3                        (DDRPHY_BASE_ADDR + 0x00290)
    #define MISC_CG_CTRL3_R_LBK_CG_CTRL                        Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CG_CTRL4                        (DDRPHY_BASE_ADDR + 0x00294)
    #define MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL                    Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CG_CTRL5                        (DDRPHY_BASE_ADDR + 0x00298)
    #define MISC_CG_CTRL5_PHYBCLK_DRAMC_CHA_ENA_MASK           Fld(1, 0) //[0:0]
    #define MISC_CG_CTRL5_R_PHY_BCLK_HWSAVE_MASK               Fld(1, 1) //[1:1]
    #define MISC_CG_CTRL5_R_PHY_BCLK_DRAMC_CHB_ENA             Fld(1, 2) //[2:2]
    #define MISC_CG_CTRL5_R_PHYBCLK_DRAMC_CHA_ENA_MASK         Fld(1, 3) //[3:3]
    #define MISC_CG_CTRL5_RESERVE                              Fld(3, 4) //[6:4]
    #define MISC_CG_CTRL5_R_DVS_DIV4_CG_CTRL_B2                Fld(9, 7) //[15:7]
    #define MISC_CG_CTRL5_R_DVS_DIV4_CG_CTRL_B2_13             Fld(1, 13) //[13:13]
    #define MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN                     Fld(1, 16) //[16:16]
    #define MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN                     Fld(1, 17) //[17:17]
    #define MISC_CG_CTRL5_R_CA_DLY_DCM_EN                      Fld(1, 18) //[18:18]
    #define MISC_CG_CTRL5_R_DQ2_DLY_DCM_EN                     Fld(1, 19) //[19:19]
    #define MISC_CG_CTRL5_R_DQ1_PI_DCM_EN                      Fld(1, 20) //[20:20]
    #define MISC_CG_CTRL5_R_DQ0_PI_DCM_EN                      Fld(1, 21) //[21:21]
    #define MISC_CG_CTRL5_R_CA_PI_DCM_EN                       Fld(1, 22) //[22:22]
    #define MISC_CG_CTRL5_R_DQ2_PI_DCM_EN                      Fld(1, 23) //[23:23]

#define DDRPHY_MISC_CTRL0                           (DDRPHY_BASE_ADDR + 0x0029C)
    #define MISC_CTRL0_R_DMDQSIEN_SYNCOPT                      Fld(4, 0) //[3:0]
    #define MISC_CTRL0_R_DMDQSIEN_OUTSEL                       Fld(4, 4) //[7:4]
    #define MISC_CTRL0_R_DMSTBEN_SYNCOPT                       Fld(1, 8) //[8:8]
    #define MISC_CTRL0_R_DMSTBEN_OUTSEL                        Fld(1, 9) //[9:9]
    #define MISC_CTRL0_IMPCAL_CHAB_EN                          Fld(1, 10) //[10:10]
    #define MISC_CTRL0_R_DMVALID_DLY_OPT                       Fld(1, 11) //[11:11]
    #define MISC_CTRL0_R_DMVALID_NARROW_IG                     Fld(1, 12) //[12:12]
    #define MISC_CTRL0_R_DMVALID_DLY                           Fld(3, 13) //[15:13]
    #define MISC_CTRL0_R_DMDQSIEN_DEPTH_HALF                   Fld(1, 16) //[16:16]
    #define MISC_CTRL0_IMPCAL_LP_ECO_OPT                       Fld(1, 18) //[18:18]
    #define MISC_CTRL0_IMPCAL_CDC_ECO_OPT                      Fld(1, 19) //[19:19]
    #define MISC_CTRL0_IDLE_DCM_CHB_CDC_ECO_OPT                Fld(1, 20)
    #define MISC_CTRL0_IMPCAL_CTL_CK_SEL                       Fld(1, 21) //[21:21]
    #define MISC_CTRL0_R_DMDQSG_B2_SRC_SEL                     Fld(1, 23) //[23:23]
    #define MISC_CTRL0_R_DMDQSIEN_FIFO_EN                      Fld(1, 24) //[24:24]
    #define MISC_CTRL0_R_DMSTBENCMP_FIFO_EN                    Fld(1, 25) //[25:25]
    #define MISC_CTRL0_R_DMSTBENCMP_RK_FIFO_EN                 Fld(1, 26) //[26:26]
    #define MISC_CTRL0_R_DMSHU_PHYDCM_FORCEOFF                 Fld(1, 27) //[27:27]
    #define MISC_CTRL0_R_DQS0IEN_DIV4_CK_CG_CTRL               Fld(1, 28) //[28:28]
    #define MISC_CTRL0_R_DQS1IEN_DIV4_CK_CG_CTRL               Fld(1, 29) //[29:29]
    #define MISC_CTRL0_R_CLKIEN_DIV4_CK_CG_CTRL                Fld(1, 30) //[30:30]
    #define MISC_CTRL0_R_STBENCMP_DIV4CK_EN                    Fld(1, 31) //[31:31]

#define DDRPHY_MISC_CTRL1                           (DDRPHY_BASE_ADDR + 0x002A0)
    #define MISC_CTRL1_R_DMPHYRST                              Fld(1, 1) //[1:1]
    #define MISC_CTRL1_R_DM_TX_ARCLK_OE                        Fld(1, 2) //[2:2]
    #define MISC_CTRL1_R_DM_TX_ARCMD_OE                        Fld(1, 3) //[3:3]
    #define MISC_CTRL1_R_DMMCTLPLL_CKSEL                       Fld(2, 4) //[5:4]
    #define MISC_CTRL1_R_DMMUXCA                               Fld(1, 6) //[6:6]
    #define MISC_CTRL1_R_DMARPIDQ_SW                           Fld(1, 7) //[7:7]
    #define MISC_CTRL1_R_DMPINMUX                              Fld(2, 8) //[9:8]
    #define MISC_CTRL1_R_DMARPICA_SW_UPDX                      Fld(1, 10) //[10:10]
    #define MISC_CTRL1_CK_BFE_DCM_EN                           Fld(1, 11) //[11:11]
    #define MISC_CTRL1_R_DMRRESETB_I_OPT                       Fld(1, 12) //[12:12]
    #define MISC_CTRL1_R_DMDA_RRESETB_I                        Fld(1, 13) //[13:13]
    #define MISC_CTRL1_R_DMMUXCA_SEC                           Fld(1, 14) //[14:14]
    #define MISC_CTRL1_R_DMPINMUX_BIT2                         Fld(1, 15) //[15:15]
    #define MISC_CTRL1_R_DMDRAMCLKEN0                          Fld(4, 16) //[19:16]
    #define MISC_CTRL1_R_DMDRAMCLKEN1                          Fld(4, 20) //[23:20]
    #define MISC_CTRL1_R_DMDQSIENCG_EN                         Fld(1, 24) //[24:24]
    #define MISC_CTRL1_R_DMSTBENCMP_RK_OPT                     Fld(1, 25) //[25:25]
    #define MISC_CTRL1_R_WL_DOWNSP                             Fld(1, 26) //[26:26]
    #define MISC_CTRL1_R_DMODTDISOE_A                          Fld(1, 27) //[27:27]
    #define MISC_CTRL1_R_DMODTDISOE_B                          Fld(1, 28) //[28:28]
    #define MISC_CTRL1_R_DMODTDISOE_C                          Fld(1, 29) //[29:29]
    #define MISC_CTRL1_R_DMODTDISOE_D                          Fld(1, 30) //[30:30]
    #define MISC_CTRL1_R_DMDA_RRESETB_E                        Fld(1, 31) //[31:31]

#define DDRPHY_MISC_CTRL2                           (DDRPHY_BASE_ADDR + 0x002A4)
    #define MISC_CTRL2_CLRPLL_SHU_GP                           Fld(2, 0) //[1:0]
    #define MISC_CTRL2_PHYPLL_SHU_GP                           Fld(2, 2) //[3:2]
    #define MISC_CTRL2_RG_CONFIG_DQ_CA                         Fld(1, 4) //[4:4]
    #define MISC_CTRL2_R_RX_DLY_FORCE_OPTION                   Fld(1, 8) //[8:8]
    #define MISC_CTRL2_R_SW_RXFIFO_RDSEL_BUS                   Fld(4, 20) //[23:20]
    #define MISC_CTRL2_R_SW_RXFIFO_RDSEL_EN                    Fld(1, 24) //[24:24]

#define DDRPHY_MISC_CTRL3                           (DDRPHY_BASE_ADDR + 0x002A8)
    #define MISC_CTRL3_ARPI_CG_CMD_OPT                         Fld(2, 0) //[1:0]
    #define MISC_CTRL3_ARPI_CG_CLK_OPT                         Fld(2, 2) //[3:2]
    #define MISC_CTRL3_ARPI_MPDIV_CG_CA_OPT                    Fld(1, 4) //[4:4]
    #define MISC_CTRL3_ARPI_CG_MCK_CA_OPT                      Fld(1, 5) //[5:5]
    #define MISC_CTRL3_ARPI_CG_MCTL_CA_OPT                     Fld(1, 6) //[6:6]
    #define MISC_CTRL3_DDRPHY_MCK_MPDIV_CG_CA_SEL              Fld(2, 8) //[9:8]
    #define MISC_CTRL3_DRAM_CLK_NEW_CA_EN_SEL                  Fld(4, 12) //[15:12]
    #define MISC_CTRL3_ARPI_CG_DQ_OPT                          Fld(2, 16) //[17:16]
    #define MISC_CTRL3_ARPI_CG_DQS_OPT                         Fld(2, 18) //[19:18]
    #define MISC_CTRL3_ARPI_MPDIV_CG_DQ_OPT                    Fld(1, 20) //[20:20]
    #define MISC_CTRL3_ARPI_CG_MCK_DQ_OPT                      Fld(1, 21) //[21:21]
    #define MISC_CTRL3_ARPI_CG_MCTL_DQ_OPT                     Fld(1, 22) //[22:22]
    #define MISC_CTRL3_DDRPHY_MCK_MPDIV_CG_DQ_SEL              Fld(2, 24) //[25:24]
    #define MISC_CTRL3_R_DDRPHY_COMB_CG_IG                     Fld(1, 26) //[26:26]
    #define MISC_CTRL3_R_DDRPHY_RX_PIPE_CG_IG                  Fld(1, 27) //[27:27]
    #define MISC_CTRL3_DRAM_CLK_NEW_DQ_EN_SEL                  Fld(4, 28) //[31:28]

#define DDRPHY_MISC_CTRL4                           (DDRPHY_BASE_ADDR + 0x002AC)
    #define MISC_CTRL4_R_OPT2_MPDIV_CG                         Fld(1, 0) //[0:0]
    #define MISC_CTRL4_R_OPT2_CG_MCK                           Fld(1, 1) //[1:1]
    #define MISC_CTRL4_R_OPT2_CG_DQM                           Fld(1, 2) //[2:2]
    #define MISC_CTRL4_R_OPT2_CG_DQS                           Fld(1, 3) //[3:3]
    #define MISC_CTRL4_R_OPT2_CG_DQ                            Fld(1, 4) //[4:4]
    #define MISC_CTRL4_R_OPT2_CG_DQSIEN                        Fld(1, 5) //[5:5]
    #define MISC_CTRL4_R_OPT2_CG_CMD                           Fld(1, 6) //[6:6]
    #define MISC_CTRL4_R_OPT2_CG_CLK                           Fld(1, 7) //[7:7]
    #define MISC_CTRL4_R_OPT2_CG_CS                            Fld(1, 8) //[8:8]
    #define MISC_CTRL4_RG_PW_CON_CHA_0                         Fld(23, 9) //[31:9]

#define DDRPHY_MISC_CTRL5                           (DDRPHY_BASE_ADDR + 0x002B0)
    #define MISC_CTRL5_RG_PW_CON_CHA_1                         Fld(32, 0) //[31:0]

#define DDRPHY_MISC_EXTLB_RX0                       (DDRPHY_BASE_ADDR + 0x002B4)
    #define MISC_EXTLB_RX0_R_EXTLB_LFSR_RX_INI_1               Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX0_R_EXTLB_LFSR_RX_INI_0               Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX1                       (DDRPHY_BASE_ADDR + 0x002B8)
    #define MISC_EXTLB_RX1_R_EXTLB_LFSR_RX_INI_3               Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX1_R_EXTLB_LFSR_RX_INI_2               Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX2                       (DDRPHY_BASE_ADDR + 0x002BC)
    #define MISC_EXTLB_RX2_R_EXTLB_LFSR_RX_INI_5               Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX2_R_EXTLB_LFSR_RX_INI_4               Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX3                       (DDRPHY_BASE_ADDR + 0x002C0)
    #define MISC_EXTLB_RX3_R_EXTLB_LFSR_RX_INI_7               Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX3_R_EXTLB_LFSR_RX_INI_6               Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX4                       (DDRPHY_BASE_ADDR + 0x002C4)
    #define MISC_EXTLB_RX4_R_EXTLB_LFSR_RX_INI_9               Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX4_R_EXTLB_LFSR_RX_INI_8               Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX5                       (DDRPHY_BASE_ADDR + 0x002C8)
    #define MISC_EXTLB_RX5_R_EXTLB_LFSR_RX_INI_11              Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX5_R_EXTLB_LFSR_RX_INI_10              Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX6                       (DDRPHY_BASE_ADDR + 0x002CC)
    #define MISC_EXTLB_RX6_R_EXTLB_LFSR_RX_INI_13              Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX6_R_EXTLB_LFSR_RX_INI_12              Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX7                       (DDRPHY_BASE_ADDR + 0x002D0)
    #define MISC_EXTLB_RX7_R_EXTLB_LFSR_RX_INI_15              Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX7_R_EXTLB_LFSR_RX_INI_14              Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX8                       (DDRPHY_BASE_ADDR + 0x002D4)
    #define MISC_EXTLB_RX8_R_EXTLB_LFSR_RX_INI_17              Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX8_R_EXTLB_LFSR_RX_INI_16              Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX9                       (DDRPHY_BASE_ADDR + 0x002D8)
    #define MISC_EXTLB_RX9_R_EXTLB_LFSR_RX_INI_19              Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX9_R_EXTLB_LFSR_RX_INI_18              Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX10                      (DDRPHY_BASE_ADDR + 0x002DC)
    #define MISC_EXTLB_RX10_R_EXTLB_LFSR_RX_INI_21             Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX10_R_EXTLB_LFSR_RX_INI_20             Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX11                      (DDRPHY_BASE_ADDR + 0x002E0)
    #define MISC_EXTLB_RX11_R_EXTLB_LFSR_RX_INI_23             Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX11_R_EXTLB_LFSR_RX_INI_22             Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX12                      (DDRPHY_BASE_ADDR + 0x002E4)
    #define MISC_EXTLB_RX12_R_EXTLB_LFSR_RX_INI_25             Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX12_R_EXTLB_LFSR_RX_INI_24             Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX13                      (DDRPHY_BASE_ADDR + 0x002E8)
    #define MISC_EXTLB_RX13_R_EXTLB_LFSR_RX_INI_27             Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX13_R_EXTLB_LFSR_RX_INI_26             Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX14                      (DDRPHY_BASE_ADDR + 0x002EC)
    #define MISC_EXTLB_RX14_R_EXTLB_LFSR_RX_INI_29             Fld(16, 16) //[31:16]
    #define MISC_EXTLB_RX14_R_EXTLB_LFSR_RX_INI_28             Fld(16, 0) //[15:0]

#define DDRPHY_MISC_EXTLB_RX15                      (DDRPHY_BASE_ADDR + 0x002F0)
    #define MISC_EXTLB_RX15_R_EXTLB_LFSR_RX_INI_30             Fld(16, 0) //[15:0]
    #define MISC_EXTLB_RX15_MISC_EXTLB_RX15_RFU                Fld(16, 16) //[31:16]

#define DDRPHY_MISC_EXTLB_RX16                      (DDRPHY_BASE_ADDR + 0x002F4)
    #define MISC_EXTLB_RX16_R_EXTLB_RX_GATE_DELSEL_DQB0        Fld(7, 0) //[6:0]
    #define MISC_EXTLB_RX16_R_EXTLB_RX_GATE_DELSEL_DQB1        Fld(7, 8) //[14:8]
    #define MISC_EXTLB_RX16_R_EXTLB_RX_GATE_DELSEL_CA          Fld(7, 16) //[22:16]
    #define MISC_EXTLB_RX16_R_EXTLB_RX_GATE_DELSEL_DQB2        Fld(7, 23) //[29:23]

#define DDRPHY_MISC_EXTLB_RX17                      (DDRPHY_BASE_ADDR + 0x002F8)
    #define MISC_EXTLB_RX17_R_XTALK_RX_00_TOG_CYCLE            Fld(4, 0) //[3:0]
    #define MISC_EXTLB_RX17_R_XTALK_RX_01_TOG_CYCLE            Fld(4, 4) //[7:4]
    #define MISC_EXTLB_RX17_R_XTALK_RX_02_TOG_CYCLE            Fld(4, 8) //[11:8]
    #define MISC_EXTLB_RX17_R_XTALK_RX_03_TOG_CYCLE            Fld(4, 12) //[15:12]
    #define MISC_EXTLB_RX17_R_XTALK_RX_04_TOG_CYCLE            Fld(4, 16) //[19:16]
    #define MISC_EXTLB_RX17_R_XTALK_RX_05_TOG_CYCLE            Fld(4, 20) //[23:20]
    #define MISC_EXTLB_RX17_R_XTALK_RX_06_TOG_CYCLE            Fld(4, 24) //[27:24]
    #define MISC_EXTLB_RX17_R_XTALK_RX_07_TOG_CYCLE            Fld(4, 28) //[31:28]

#define DDRPHY_MISC_EXTLB_RX18                      (DDRPHY_BASE_ADDR + 0x002FC)
    #define MISC_EXTLB_RX18_R_XTALK_RX_08_TOG_CYCLE            Fld(4, 0) //[3:0]
    #define MISC_EXTLB_RX18_R_XTALK_RX_09_TOG_CYCLE            Fld(4, 4) //[7:4]
    #define MISC_EXTLB_RX18_R_XTALK_RX_10_TOG_CYCLE            Fld(4, 8) //[11:8]
    #define MISC_EXTLB_RX18_R_XTALK_RX_11_TOG_CYCLE            Fld(4, 12) //[15:12]
    #define MISC_EXTLB_RX18_R_XTALK_RX_12_TOG_CYCLE            Fld(4, 16) //[19:16]
    #define MISC_EXTLB_RX18_R_XTALK_RX_13_TOG_CYCLE            Fld(4, 20) //[23:20]
    #define MISC_EXTLB_RX18_R_XTALK_RX_14_TOG_CYCLE            Fld(4, 24) //[27:24]
    #define MISC_EXTLB_RX18_R_XTALK_RX_15_TOG_CYCLE            Fld(4, 28) //[31:28]

#define DDRPHY_MISC_EXTLB_RX19                      (DDRPHY_BASE_ADDR + 0x00300)
    #define MISC_EXTLB_RX19_R_XTALK_RX_16_TOG_CYCLE            Fld(4, 0) //[3:0]
    #define MISC_EXTLB_RX19_R_XTALK_RX_17_TOG_CYCLE            Fld(4, 4) //[7:4]
    #define MISC_EXTLB_RX19_R_XTALK_RX_18_TOG_CYCLE            Fld(4, 8) //[11:8]
    #define MISC_EXTLB_RX19_R_XTALK_RX_19_TOG_CYCLE            Fld(4, 12) //[15:12]
    #define MISC_EXTLB_RX19_R_XTALK_RX_20_TOG_CYCLE            Fld(4, 16) //[19:16]
    #define MISC_EXTLB_RX19_R_XTALK_RX_21_TOG_CYCLE            Fld(4, 20) //[23:20]
    #define MISC_EXTLB_RX19_R_XTALK_RX_22_TOG_CYCLE            Fld(4, 24) //[27:24]
    #define MISC_EXTLB_RX19_R_XTALK_RX_23_TOG_CYCLE            Fld(4, 28) //[31:28]

#define DDRPHY_MISC_EXTLB_RX20                      (DDRPHY_BASE_ADDR + 0x00304)
    #define MISC_EXTLB_RX20_R_XTALK_RX_24_TOG_CYCLE            Fld(4, 0) //[3:0]
    #define MISC_EXTLB_RX20_R_XTALK_RX_25_TOG_CYCLE            Fld(4, 4) //[7:4]
    #define MISC_EXTLB_RX20_R_XTALK_RX_26_TOG_CYCLE            Fld(4, 8) //[11:8]
    #define MISC_EXTLB_RX20_R_XTALK_RX_27_TOG_CYCLE            Fld(4, 12) //[15:12]
    #define MISC_EXTLB_RX20_R_XTALK_RX_28_TOG_CYCLE            Fld(4, 16) //[19:16]
    #define MISC_EXTLB_RX20_R_XTALK_RX_29_TOG_CYCLE            Fld(4, 20) //[23:20]
    #define MISC_EXTLB_RX20_R_XTALK_RX_30_TOG_CYCLE            Fld(4, 24) //[27:24]
    #define MISC_EXTLB_RX20_R_XTALK_RX_31_TOG_CYCLE            Fld(4, 28) //[31:28]

#define DDRPHY_CKMUX_SEL                            (DDRPHY_BASE_ADDR + 0x00308)
    #define CKMUX_SEL_R_PHYCTRLMUX                             Fld(1, 0) //[0:0]
    #define CKMUX_SEL_R_PHYCTRLDCM                             Fld(1, 1) //[1:1]
    #define CKMUX_SEL_FB_CK_MUX                                Fld(2, 16) //[17:16]
    #define CKMUX_SEL_FMEM_CK_MUX                              Fld(2, 18) //[19:18]

#define DDRPHY_RFU_0X30C                            (DDRPHY_BASE_ADDR + 0x0030C)
    #define RFU_0X30C_RESERVED_0X30C                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X310                            (DDRPHY_BASE_ADDR + 0x00310)
    #define RFU_0X310_RESERVED_0X310                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X314                            (DDRPHY_BASE_ADDR + 0x00314)
    #define RFU_0X314_RESERVED_0X314                           Fld(32, 0) //[31:0]

#define DDRPHY_MISC_CG_CTRL6                        (DDRPHY_BASE_ADDR + 0x00318)
    #define MISC_CG_CTRL6_ARMCTL_CK_OUT_CG_SEL                 Fld(1, 0) //[0:0]
    #define MISC_CG_CTRL6_FMEM_CK_CG_PINMUX                    Fld(2, 4) //[5:4]
    #define MISC_CG_CTRL6_IDLE_CG_SEL                          Fld(2, 6) //[7:6]

#define DDRPHY_MISC_CG_CTRL7                        (DDRPHY_BASE_ADDR + 0x0031C)
    #define MISC_CG_CTRL7_R_PHY_MCK_CG_CTRL_2                  Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X320                            (DDRPHY_BASE_ADDR + 0x00320)
    #define RFU_0X320_RESERVED_0X320                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X324                            (DDRPHY_BASE_ADDR + 0x00324)
    #define RFU_0X324_RESERVED_0X324                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X328                            (DDRPHY_BASE_ADDR + 0x00328)
    #define RFU_0X328_RESERVED_0X328                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X32C                            (DDRPHY_BASE_ADDR + 0x0032C)
    #define RFU_0X32C_RESERVED_0X32C                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X330                            (DDRPHY_BASE_ADDR + 0x00330)
    #define RFU_0X330_RESERVED_0X330                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X334                            (DDRPHY_BASE_ADDR + 0x00334)
    #define RFU_0X334_RESERVED_0X334                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X338                            (DDRPHY_BASE_ADDR + 0x00338)
    #define RFU_0X338_RESERVED_0X338                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X33C                            (DDRPHY_BASE_ADDR + 0x0033C)
    #define RFU_0X33C_RESERVED_0X33C                           Fld(32, 0) //[31:0]

#define DDRPHY_MISC_STBERR_RK0_R                    (DDRPHY_BASE_ADDR + 0x00510)
    #define MISC_STBERR_RK0_R_STBERR_RK0_R                     Fld(16, 0) //[15:0]
    #define MISC_STBERR_RK0_R_STBENERR_ALL                     Fld(1, 16) //[16:16]
    #define MISC_STBERR_RK0_R_RX_ARDQ0_FIFO_STBEN_ERR_B0       Fld(1, 24) //[24:24]
    #define MISC_STBERR_RK0_R_RX_ARDQ4_FIFO_STBEN_ERR_B0       Fld(1, 25) //[25:25]
    #define MISC_STBERR_RK0_R_RX_ARDQ0_FIFO_STBEN_ERR_B1       Fld(1, 26) //[26:26]
    #define MISC_STBERR_RK0_R_RX_ARDQ4_FIFO_STBEN_ERR_B1       Fld(1, 27) //[27:27]
    #define MISC_STBERR_RK0_R_RX_ARCA0_FIFO_STBEN_ERR          Fld(1, 28) //[28:28]
    #define MISC_STBERR_RK0_R_RX_ARCA4_FIFO_STBEN_ERR          Fld(1, 29) //[29:29]
    #define MISC_STBERR_RK0_R_DA_RPHYPLLGP_CK_SEL              Fld(1, 31) //[31:31]

#define DDRPHY_MISC_STBERR_RK0_F                    (DDRPHY_BASE_ADDR + 0x00514)
    #define MISC_STBERR_RK0_F_STBERR_RK0_F                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_STBERR_RK1_R                    (DDRPHY_BASE_ADDR + 0x00518)
    #define MISC_STBERR_RK1_R_STBERR_RK1_R                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_STBERR_RK1_F                    (DDRPHY_BASE_ADDR + 0x0051C)
    #define MISC_STBERR_RK1_F_STBERR_RK1_F                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_STBERR_RK2_R                    (DDRPHY_BASE_ADDR + 0x00520)
    #define MISC_STBERR_RK2_R_STBERR_RK2_R                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_STBERR_RK2_F                    (DDRPHY_BASE_ADDR + 0x00524)
    #define MISC_STBERR_RK2_F_STBERR_RK2_F                     Fld(16, 0) //[15:0]

#define DDRPHY_MISC_RXDVS0                          (DDRPHY_BASE_ADDR + 0x005E0)
    #define MISC_RXDVS0_R_RX_DLY_TRACK_RO_SEL                  Fld(3, 0) //[2:0]
    #define MISC_RXDVS0_R_DA_DQX_R_DLY_RO_SEL                  Fld(4, 8) //[11:8]
    #define MISC_RXDVS0_R_DA_CAX_R_DLY_RO_SEL                  Fld(4, 12) //[15:12]

#define DDRPHY_MISC_RXDVS2                          (DDRPHY_BASE_ADDR + 0x005E8)
    #define MISC_RXDVS2_R_DMRXDVS_DEPTH_HALF                   Fld(1, 0) //[0:0]
    #define MISC_RXDVS2_R_DMRXDVS_SHUFFLE_CTRL_CG_IG           Fld(1, 8) //[8:8]
    #define MISC_RXDVS2_R_DMRXDVS_DBG_MON_EN                   Fld(1, 16) //[16:16]
    #define MISC_RXDVS2_R_DMRXDVS_DBG_MON_CLR                  Fld(1, 17) //[17:17]
    #define MISC_RXDVS2_R_DMRXDVS_DBG_PAUSE_EN                 Fld(1, 18) //[18:18]

#define DDRPHY_RFU_0X5EC                            (DDRPHY_BASE_ADDR + 0x005EC)
    #define RFU_0X5EC_RESERVED_0X5EC                           Fld(32, 0) //[31:0]

#define DDRPHY_B0_RXDVS0                            (DDRPHY_BASE_ADDR + 0x005F0)
    #define B0_RXDVS0_R_RX_RANKINSEL_B0                        Fld(1, 0) //[0:0]
    #define B0_RXDVS0_B0_RXDVS0_RFU                            Fld(3, 1) //[3:1]
    #define B0_RXDVS0_R_RX_RANKINCTL_B0                        Fld(4, 4) //[7:4]
    #define B0_RXDVS0_R_DVS_SW_UP_B0                           Fld(1, 8) //[8:8]
    #define B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0                Fld(1, 9) //[9:9]
    #define B0_RXDVS0_R_DMRXDVS_PBYTESTUCK_RST_B0              Fld(1, 10) //[10:10]
    #define B0_RXDVS0_R_DMRXDVS_PBYTESTUCK_IG_B0               Fld(1, 11) //[11:11]
    #define B0_RXDVS0_R_DMRXDVS_DQIENPOST_OPT_B0               Fld(2, 12) //[13:12]
    #define B0_RXDVS0_R_RX_DLY_RANK_ERR_ST_CLR_B0              Fld(3, 16) //[18:16]
    #define B0_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B0                  Fld(1, 19) //[19:19]
    #define B0_RXDVS0_R_RX_DLY_RK_OPT_B0                       Fld(2, 20) //[21:20]
    #define B0_RXDVS0_R_HWRESTORE_ENA_B0                       Fld(1, 22) //[22:22]
    #define B0_RXDVS0_R_HWSAVE_MODE_ENA_B0                     Fld(1, 24) //[24:24]
    #define B0_RXDVS0_R_RX_DLY_DVS_MODE_SYNC_DIS_B0            Fld(1, 26) //[26:26]
    #define B0_RXDVS0_R_RX_DLY_TRACK_BYPASS_MODESYNC_B0        Fld(1, 27) //[27:27]
    #define B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0                  Fld(1, 28) //[28:28]
    #define B0_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B0               Fld(1, 29) //[29:29]
    #define B0_RXDVS0_R_RX_DLY_TRACK_CLR_B0                    Fld(1, 30) //[30:30]
    #define B0_RXDVS0_R_RX_DLY_TRACK_ENA_B0                    Fld(1, 31) //[31:31]

#define DDRPHY_B0_RXDVS1                            (DDRPHY_BASE_ADDR + 0x005F4)
    #define B0_RXDVS1_B0_RXDVS1_RFU                            Fld(16, 0) //[15:0]
    #define B0_RXDVS1_R_DMRXDVS_UPD_CLR_ACK_B0                 Fld(1, 16) //[16:16]
    #define B0_RXDVS1_R_DMRXDVS_UPD_CLR_NORD_B0                Fld(1, 17) //[17:17]

#define DDRPHY_RFU_0X5F8                            (DDRPHY_BASE_ADDR + 0x005F8)
    #define RFU_0X5F8_RESERVED_0X5F8                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X5FC                            (DDRPHY_BASE_ADDR + 0x005FC)
    #define RFU_0X5FC_RESERVED_0X5FC                           Fld(32, 0) //[31:0]

#define DDRPHY_R0_B0_RXDVS0                         (DDRPHY_BASE_ADDR + 0x00600)
    #define R0_B0_RXDVS0_R_RK0_B0_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R0_B0_RXDVS0_R_RK0_B0_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R0_B0_RXDVS0_R_RK0_B0_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R0_B0_RXDVS1                         (DDRPHY_BASE_ADDR + 0x00604)
    #define R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R0_B0_RXDVS2                         (DDRPHY_BASE_ADDR + 0x00608)
    #define R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_B0         Fld(2, 16) //[17:16]
    #define R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_B0          Fld(2, 18) //[19:18]
    #define R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0    Fld(1, 23) //[23:23]
    #define R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_B0         Fld(2, 24) //[25:24]
    #define R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_B0          Fld(2, 26) //[27:26]
    #define R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0    Fld(1, 28) //[28:28]
    #define R0_B0_RXDVS2_R_RK0_DVS_FDLY_MODE_B0                Fld(1, 29) //[29:29]
    #define R0_B0_RXDVS2_R_RK0_DVS_MODE_B0                     Fld(2, 30) //[31:30]

#define DDRPHY_R0_B0_RXDVS7                         (DDRPHY_BASE_ADDR + 0x0061C)
    #define R0_B0_RXDVS7_RG_RK0_ARDQ_MIN_DLY_B0                Fld(6, 0) //[5:0]
    #define R0_B0_RXDVS7_RG_RK0_ARDQ_MIN_DLY_B0_RFU            Fld(2, 6) //[7:6]
    #define R0_B0_RXDVS7_RG_RK0_ARDQ_MAX_DLY_B0                Fld(6, 8) //[13:8]
    #define R0_B0_RXDVS7_RG_RK0_ARDQ_MAX_DLY_B0_RFU            Fld(2, 14) //[15:14]
    #define R0_B0_RXDVS7_RG_RK0_ARDQS0_MIN_DLY_B0              Fld(7, 16) //[22:16]
    #define R0_B0_RXDVS7_RG_RK0_ARDQS0_MIN_DLY_B0_RFU          Fld(1, 23) //[23:23]
    #define R0_B0_RXDVS7_RG_RK0_ARDQS0_MAX_DLY_B0              Fld(7, 24) //[30:24]
    #define R0_B0_RXDVS7_RG_RK0_ARDQS0_MAX_DLY_B0_RFU          Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X620                            (DDRPHY_BASE_ADDR + 0x00620)
    #define RFU_0X620_RESERVED_0X620                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X624                            (DDRPHY_BASE_ADDR + 0x00624)
    #define RFU_0X624_RESERVED_0X624                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X628                            (DDRPHY_BASE_ADDR + 0x00628)
    #define RFU_0X628_RESERVED_0X628                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X62C                            (DDRPHY_BASE_ADDR + 0x0062C)
    #define RFU_0X62C_RESERVED_0X62C                           Fld(32, 0) //[31:0]

#define DDRPHY_B1_RXDVS0                            (DDRPHY_BASE_ADDR + 0x00670)
    #define B1_RXDVS0_R_RX_RANKINSEL_B1                        Fld(1, 0) //[0:0]
    #define B1_RXDVS0_B1_RXDVS0_RFU                            Fld(3, 1) //[3:1]
    #define B1_RXDVS0_R_RX_RANKINCTL_B1                        Fld(4, 4) //[7:4]
    #define B1_RXDVS0_R_DVS_SW_UP_B1                           Fld(1, 8) //[8:8]
    #define B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1                Fld(1, 9) //[9:9]
    #define B1_RXDVS0_R_DMRXDVS_PBYTESTUCK_RST_B1              Fld(1, 10) //[10:10]
    #define B1_RXDVS0_R_DMRXDVS_PBYTESTUCK_IG_B1               Fld(1, 11) //[11:11]
    #define B1_RXDVS0_R_DMRXDVS_DQIENPOST_OPT_B1               Fld(2, 12) //[13:12]
    #define B1_RXDVS0_R_RX_DLY_RANK_ERR_ST_CLR_B1              Fld(3, 16) //[18:16]
    #define B1_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B1                  Fld(1, 19) //[19:19]
    #define B1_RXDVS0_R_RX_DLY_RK_OPT_B1                       Fld(2, 20) //[21:20]
    #define B1_RXDVS0_R_HWRESTORE_ENA_B1                       Fld(1, 22) //[22:22]
    #define B1_RXDVS0_R_HWSAVE_MODE_ENA_B1                     Fld(1, 24) //[24:24]
    #define B1_RXDVS0_R_RX_DLY_DVS_MODE_SYNC_DIS_B1            Fld(1, 26) //[26:26]
    #define B1_RXDVS0_R_RX_DLY_TRACK_BYPASS_MODESYNC_B1        Fld(1, 27) //[27:27]
    #define B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1                  Fld(1, 28) //[28:28]
    #define B1_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B1               Fld(1, 29) //[29:29]
    #define B1_RXDVS0_R_RX_DLY_TRACK_CLR_B1                    Fld(1, 30) //[30:30]
    #define B1_RXDVS0_R_RX_DLY_TRACK_ENA_B1                    Fld(1, 31) //[31:31]

#define DDRPHY_B1_RXDVS1                            (DDRPHY_BASE_ADDR + 0x00674)
    #define B1_RXDVS1_B1_RXDVS1_RFU                            Fld(16, 0) //[15:0]
    #define B1_RXDVS1_R_DMRXDVS_UPD_CLR_ACK_B1                 Fld(1, 16) //[16:16]
    #define B1_RXDVS1_R_DMRXDVS_UPD_CLR_NORD_B1                Fld(1, 17) //[17:17]

#define DDRPHY_RFU_0X678                            (DDRPHY_BASE_ADDR + 0x00678)
    #define RFU_0X678_RESERVED_0X678                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X67C                            (DDRPHY_BASE_ADDR + 0x0067C)
    #define RFU_0X67C_RESERVED_0X67C                           Fld(32, 0) //[31:0]

#define DDRPHY_R0_B1_RXDVS0                         (DDRPHY_BASE_ADDR + 0x00680)
    #define R0_B1_RXDVS0_R_RK0_B1_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R0_B1_RXDVS0_R_RK0_B1_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R0_B1_RXDVS0_R_RK0_B1_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R0_B1_RXDVS1                         (DDRPHY_BASE_ADDR + 0x00684)
    #define R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R0_B1_RXDVS2                         (DDRPHY_BASE_ADDR + 0x00688)
    #define R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_B1         Fld(2, 16) //[17:16]
    #define R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_B1          Fld(2, 18) //[19:18]
    #define R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1    Fld(1, 23) //[23:23]
    #define R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_B1         Fld(2, 24) //[25:24]
    #define R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_B1          Fld(2, 26) //[27:26]
    #define R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1    Fld(1, 28) //[28:28]
    #define R0_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1                Fld(1, 29) //[29:29]
    #define R0_B1_RXDVS2_R_RK0_DVS_MODE_B1                     Fld(2, 30) //[31:30]

#define DDRPHY_R0_B1_RXDVS7                         (DDRPHY_BASE_ADDR + 0x0069C)
    #define R0_B1_RXDVS7_RG_RK0_ARDQ_MIN_DLY_B1                Fld(6, 0) //[5:0]
    #define R0_B1_RXDVS7_RG_RK0_ARDQ_MIN_DLY_B1_RFU            Fld(2, 6) //[7:6]
    #define R0_B1_RXDVS7_RG_RK0_ARDQ_MAX_DLY_B1                Fld(6, 8) //[13:8]
    #define R0_B1_RXDVS7_RG_RK0_ARDQ_MAX_DLY_B1_RFU            Fld(2, 14) //[15:14]
    #define R0_B1_RXDVS7_RG_RK0_ARDQS0_MIN_DLY_B1              Fld(7, 16) //[22:16]
    #define R0_B1_RXDVS7_RG_RK0_ARDQS0_MIN_DLY_B1_RFU          Fld(1, 23) //[23:23]
    #define R0_B1_RXDVS7_RG_RK0_ARDQS0_MAX_DLY_B1              Fld(7, 24) //[30:24]
    #define R0_B1_RXDVS7_RG_RK0_ARDQS0_MAX_DLY_B1_RFU          Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X6A0                            (DDRPHY_BASE_ADDR + 0x006A0)
    #define RFU_0X6A0_RESERVED_0X6A0                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X6A4                            (DDRPHY_BASE_ADDR + 0x006A4)
    #define RFU_0X6A4_RESERVED_0X6A4                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X6A8                            (DDRPHY_BASE_ADDR + 0x006A8)
    #define RFU_0X6A8_RESERVED_0X6A8                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X6AC                            (DDRPHY_BASE_ADDR + 0x006AC)
    #define RFU_0X6AC_RESERVED_0X6AC                           Fld(32, 0) //[31:0]

#define DDRPHY_CA_RXDVS0                            (DDRPHY_BASE_ADDR + 0x006F0)
    #define CA_RXDVS0_R_RX_RANKINSEL_CA                        Fld(1, 0) //[0:0]
    #define CA_RXDVS0_CA_RXDVS0_RFU                            Fld(3, 1) //[3:1]
    #define CA_RXDVS0_R_RX_RANKINCTL_CA                        Fld(4, 4) //[7:4]
    #define CA_RXDVS0_R_DVS_SW_UP_CA                           Fld(1, 8) //[8:8]
    #define CA_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_CA                Fld(1, 9) //[9:9]
    #define CA_RXDVS0_R_DMRXDVS_PBYTESTUCK_RST_CA              Fld(1, 10) //[10:10]
    #define CA_RXDVS0_R_DMRXDVS_PBYTESTUCK_IG_CA               Fld(1, 11) //[11:11]
    #define CA_RXDVS0_R_DMRXDVS_DQIENPOST_OPT_CA               Fld(2, 12) //[13:12]
    #define CA_RXDVS0_R_RX_DLY_RANK_ERR_ST_CLR_CA              Fld(3, 16) //[18:16]
    #define CA_RXDVS0_R_DMRXDVS_CNTCMP_OPT_CA                  Fld(1, 19) //[19:19]
    #define CA_RXDVS0_R_RX_DLY_RK_OPT_CA                       Fld(2, 20) //[21:20]
    #define CA_RXDVS0_R_HWRESTORE_ENA_CA                       Fld(1, 22) //[22:22]
    #define CA_RXDVS0_R_HWSAVE_MODE_ENA_CA                     Fld(1, 24) //[24:24]
    #define CA_RXDVS0_R_RX_DLY_DVS_MODE_SYNC_DIS_CA            Fld(1, 26) //[26:26]
    #define CA_RXDVS0_R_RX_DLY_TRACK_BYPASS_MODESYNC_CA        Fld(1, 27) //[27:27]
    #define CA_RXDVS0_R_RX_DLY_TRACK_CG_EN_CA                  Fld(1, 28) //[28:28]
    #define CA_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_CA               Fld(1, 29) //[29:29]
    #define CA_RXDVS0_R_RX_DLY_TRACK_CLR_CA                    Fld(1, 30) //[30:30]
    #define CA_RXDVS0_R_RX_DLY_TRACK_ENA_CA                    Fld(1, 31) //[31:31]

#define DDRPHY_CA_RXDVS1                            (DDRPHY_BASE_ADDR + 0x006F4)
    #define CA_RXDVS1_CA_RXDVS1_RFU                            Fld(16, 0) //[15:0]
    #define CA_RXDVS1_R_DMRXDVS_UPD_CLR_ACK_CA                 Fld(1, 16) //[16:16]
    #define CA_RXDVS1_R_DMRXDVS_UPD_CLR_NORD_CA                Fld(1, 17) //[17:17]

#define DDRPHY_RFU_0X6F8                            (DDRPHY_BASE_ADDR + 0x006F8)
    #define RFU_0X6F8_RESERVED_0X6F8                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X6FC                            (DDRPHY_BASE_ADDR + 0x006FC)
    #define RFU_0X6FC_RESERVED_0X6FC                           Fld(32, 0) //[31:0]

#define DDRPHY_R0_CA_RXDVS0                         (DDRPHY_BASE_ADDR + 0x00700)
    #define R0_CA_RXDVS0_R_RK0_CA_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R0_CA_RXDVS0_R_RK0_CA_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R0_CA_RXDVS0_R_RK0_CA_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R0_CA_RXDVS1                         (DDRPHY_BASE_ADDR + 0x00704)
    #define R0_CA_RXDVS1_R_RK0_CA_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R0_CA_RXDVS1_R_RK0_CA_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R0_CA_RXDVS2                         (DDRPHY_BASE_ADDR + 0x00708)
    #define R0_CA_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_CA         Fld(2, 16) //[17:16]
    #define R0_CA_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_CA          Fld(2, 18) //[19:18]
    #define R0_CA_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_CA    Fld(1, 23) //[23:23]
    #define R0_CA_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_CA         Fld(2, 24) //[25:24]
    #define R0_CA_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_CA          Fld(2, 26) //[27:26]
    #define R0_CA_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_CA    Fld(1, 28) //[28:28]
    #define R0_CA_RXDVS2_R_RK0_DVS_FDLY_MODE_CA                Fld(1, 29) //[29:29]
    #define R0_CA_RXDVS2_R_RK0_DVS_MODE_CA                     Fld(2, 30) //[31:30]

#define DDRPHY_R0_CA_RXDVS9                         (DDRPHY_BASE_ADDR + 0x00724)
    #define R0_CA_RXDVS9_RG_RK0_ARCMD_MIN_DLY                  Fld(6, 0) //[5:0]
    #define R0_CA_RXDVS9_RG_RK0_ARCMD_MIN_DLY_RFU              Fld(2, 6) //[7:6]
    #define R0_CA_RXDVS9_RG_RK0_ARCMD_MAX_DLY                  Fld(6, 8) //[13:8]
    #define R0_CA_RXDVS9_RG_RK0_ARCMD_MAX_DLY_RFU              Fld(2, 14) //[15:14]
    #define R0_CA_RXDVS9_RG_RK0_ARCLK_MIN_DLY                  Fld(7, 16) //[22:16]
    #define R0_CA_RXDVS9_RG_RK0_ARCLK_MIN_DLY_RFU              Fld(1, 23) //[23:23]
    #define R0_CA_RXDVS9_RG_RK0_ARCLK_MAX_DLY                  Fld(7, 24) //[30:24]
    #define R0_CA_RXDVS9_RG_RK0_ARCLK_MAX_DLY_RFU              Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X728                            (DDRPHY_BASE_ADDR + 0x00728)
    #define RFU_0X728_RESERVED_0X728                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X72C                            (DDRPHY_BASE_ADDR + 0x0072C)
    #define RFU_0X72C_RESERVED_0X72C                           Fld(32, 0) //[31:0]

#define DDRPHY_R1_B0_RXDVS0                         (DDRPHY_BASE_ADDR + 0x00800)
    #define R1_B0_RXDVS0_R_RK1_B0_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R1_B0_RXDVS0_R_RK1_B0_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R1_B0_RXDVS0_R_RK1_B0_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R1_B0_RXDVS1                         (DDRPHY_BASE_ADDR + 0x00804)
    #define R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R1_B0_RXDVS2                         (DDRPHY_BASE_ADDR + 0x00808)
    #define R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_DQS_SCALE_B0         Fld(2, 16) //[17:16]
    #define R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_DQ_SCALE_B0          Fld(2, 18) //[19:18]
    #define R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0    Fld(1, 23) //[23:23]
    #define R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_DQS_SCALE_B0         Fld(2, 24) //[25:24]
    #define R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_DQ_SCALE_B0          Fld(2, 26) //[27:26]
    #define R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0    Fld(1, 28) //[28:28]
    #define R1_B0_RXDVS2_R_RK1_DVS_FDLY_MODE_B0                Fld(1, 29) //[29:29]
    #define R1_B0_RXDVS2_R_RK1_DVS_MODE_B0                     Fld(2, 30) //[31:30]

#define DDRPHY_R1_B0_RXDVS7                         (DDRPHY_BASE_ADDR + 0x0081C)
    #define R1_B0_RXDVS7_RG_RK1_ARDQ_MIN_DLY_B0                Fld(6, 0) //[5:0]
    #define R1_B0_RXDVS7_RG_RK1_ARDQ_MIN_DLY_B0_RFU            Fld(2, 6) //[7:6]
    #define R1_B0_RXDVS7_RG_RK1_ARDQ_MAX_DLY_B0                Fld(6, 8) //[13:8]
    #define R1_B0_RXDVS7_RG_RK1_ARDQ_MAX_DLY_B0_RFU            Fld(2, 14) //[15:14]
    #define R1_B0_RXDVS7_RG_RK1_ARDQS0_MIN_DLY_B0              Fld(7, 16) //[22:16]
    #define R1_B0_RXDVS7_RG_RK1_ARDQS0_MIN_DLY_B0_RFU          Fld(1, 23) //[23:23]
    #define R1_B0_RXDVS7_RG_RK1_ARDQS0_MAX_DLY_B0              Fld(7, 24) //[30:24]
    #define R1_B0_RXDVS7_RG_RK1_ARDQS0_MAX_DLY_B0_RFU          Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X820                            (DDRPHY_BASE_ADDR + 0x00820)
    #define RFU_0X820_RESERVED_0X820                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X824                            (DDRPHY_BASE_ADDR + 0x00824)
    #define RFU_0X824_RESERVED_0X824                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X828                            (DDRPHY_BASE_ADDR + 0x00828)
    #define RFU_0X828_RESERVED_0X828                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X82C                            (DDRPHY_BASE_ADDR + 0x0082C)
    #define RFU_0X82C_RESERVED_0X82C                           Fld(32, 0) //[31:0]

#define DDRPHY_R1_B1_RXDVS0                         (DDRPHY_BASE_ADDR + 0x00880)
    #define R1_B1_RXDVS0_R_RK1_B1_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R1_B1_RXDVS0_R_RK1_B1_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R1_B1_RXDVS0_R_RK1_B1_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R1_B1_RXDVS1                         (DDRPHY_BASE_ADDR + 0x00884)
    #define R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R1_B1_RXDVS2                         (DDRPHY_BASE_ADDR + 0x00888)
    #define R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_DQS_SCALE_B1         Fld(2, 16) //[17:16]
    #define R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_DQ_SCALE_B1          Fld(2, 18) //[19:18]
    #define R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1    Fld(1, 23) //[23:23]
    #define R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_DQS_SCALE_B1         Fld(2, 24) //[25:24]
    #define R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_DQ_SCALE_B1          Fld(2, 26) //[27:26]
    #define R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1    Fld(1, 28) //[28:28]
    #define R1_B1_RXDVS2_R_RK1_DVS_FDLY_MODE_B1                Fld(1, 29) //[29:29]
    #define R1_B1_RXDVS2_R_RK1_DVS_MODE_B1                     Fld(2, 30) //[31:30]

#define DDRPHY_R1_B1_RXDVS7                         (DDRPHY_BASE_ADDR + 0x0089C)
    #define R1_B1_RXDVS7_RG_RK1_ARDQ_MIN_DLY_B1                Fld(6, 0) //[5:0]
    #define R1_B1_RXDVS7_RG_RK1_ARDQ_MIN_DLY_B1_RFU            Fld(2, 6) //[7:6]
    #define R1_B1_RXDVS7_RG_RK1_ARDQ_MAX_DLY_B1                Fld(6, 8) //[13:8]
    #define R1_B1_RXDVS7_RG_RK1_ARDQ_MAX_DLY_B1_RFU            Fld(2, 14) //[15:14]
    #define R1_B1_RXDVS7_RG_RK1_ARDQS0_MIN_DLY_B1              Fld(7, 16) //[22:16]
    #define R1_B1_RXDVS7_RG_RK1_ARDQS0_MIN_DLY_B1_RFU          Fld(1, 23) //[23:23]
    #define R1_B1_RXDVS7_RG_RK1_ARDQS0_MAX_DLY_B1              Fld(7, 24) //[30:24]
    #define R1_B1_RXDVS7_RG_RK1_ARDQS0_MAX_DLY_B1_RFU          Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X8A0                            (DDRPHY_BASE_ADDR + 0x008A0)
    #define RFU_0X8A0_RESERVED_0X8A0                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X8A4                            (DDRPHY_BASE_ADDR + 0x008A4)
    #define RFU_0X8A4_RESERVED_0X8A4                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X8A8                            (DDRPHY_BASE_ADDR + 0x008A8)
    #define RFU_0X8A8_RESERVED_0X8A8                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X8AC                            (DDRPHY_BASE_ADDR + 0x008AC)
    #define RFU_0X8AC_RESERVED_0X8AC                           Fld(32, 0) //[31:0]

#define DDRPHY_R1_CA_RXDVS0                         (DDRPHY_BASE_ADDR + 0x00900)
    #define R1_CA_RXDVS0_R_RK1_CA_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R1_CA_RXDVS0_R_RK1_CA_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R1_CA_RXDVS0_R_RK1_CA_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R1_CA_RXDVS1                         (DDRPHY_BASE_ADDR + 0x00904)
    #define R1_CA_RXDVS1_R_RK1_CA_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R1_CA_RXDVS1_R_RK1_CA_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R1_CA_RXDVS2                         (DDRPHY_BASE_ADDR + 0x00908)
    #define R1_CA_RXDVS2_R_RK1_RX_DLY_FAL_DQS_SCALE_CA         Fld(2, 16) //[17:16]
    #define R1_CA_RXDVS2_R_RK1_RX_DLY_FAL_DQ_SCALE_CA          Fld(2, 18) //[19:18]
    #define R1_CA_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_CA    Fld(1, 23) //[23:23]
    #define R1_CA_RXDVS2_R_RK1_RX_DLY_RIS_DQS_SCALE_CA         Fld(2, 24) //[25:24]
    #define R1_CA_RXDVS2_R_RK1_RX_DLY_RIS_DQ_SCALE_CA          Fld(2, 26) //[27:26]
    #define R1_CA_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_CA    Fld(1, 28) //[28:28]
    #define R1_CA_RXDVS2_R_RK1_DVS_FDLY_MODE_CA                Fld(1, 29) //[29:29]
    #define R1_CA_RXDVS2_R_RK1_DVS_MODE_CA                     Fld(2, 30) //[31:30]

#define DDRPHY_R1_CA_RXDVS9                         (DDRPHY_BASE_ADDR + 0x00924)
    #define R1_CA_RXDVS9_RG_RK1_ARCMD_MIN_DLY                  Fld(6, 0) //[5:0]
    #define R1_CA_RXDVS9_RG_RK1_ARCMD_MIN_DLY_RFU              Fld(2, 6) //[7:6]
    #define R1_CA_RXDVS9_RG_RK1_ARCMD_MAX_DLY                  Fld(6, 8) //[13:8]
    #define R1_CA_RXDVS9_RG_RK1_ARCMD_MAX_DLY_RFU              Fld(2, 14) //[15:14]
    #define R1_CA_RXDVS9_RG_RK1_ARCLK_MIN_DLY                  Fld(7, 16) //[22:16]
    #define R1_CA_RXDVS9_RG_RK1_ARCLK_MIN_DLY_RFU              Fld(1, 23) //[23:23]
    #define R1_CA_RXDVS9_RG_RK1_ARCLK_MAX_DLY                  Fld(7, 24) //[30:24]
    #define R1_CA_RXDVS9_RG_RK1_ARCLK_MAX_DLY_RFU              Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X928                            (DDRPHY_BASE_ADDR + 0x00928)
    #define RFU_0X928_RESERVED_0X928                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X92C                            (DDRPHY_BASE_ADDR + 0x0092C)
    #define RFU_0X92C_RESERVED_0X92C                           Fld(32, 0) //[31:0]

#define DDRPHY_R2_B0_RXDVS0                         (DDRPHY_BASE_ADDR + 0x00A00)
    #define R2_B0_RXDVS0_R_RK2_B0_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R2_B0_RXDVS0_R_RK2_B0_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R2_B0_RXDVS0_R_RK2_B0_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R2_B0_RXDVS1                         (DDRPHY_BASE_ADDR + 0x00A04)
    #define R2_B0_RXDVS1_R_RK2_B0_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R2_B0_RXDVS1_R_RK2_B0_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R2_B0_RXDVS2                         (DDRPHY_BASE_ADDR + 0x00A08)
    #define R2_B0_RXDVS2_R_RK2_RX_DLY_FAL_DQS_SCALE_B0         Fld(2, 16) //[17:16]
    #define R2_B0_RXDVS2_R_RK2_RX_DLY_FAL_DQ_SCALE_B0          Fld(2, 18) //[19:18]
    #define R2_B0_RXDVS2_R_RK2_RX_DLY_FAL_TRACK_GATE_ENA_B0    Fld(1, 23) //[23:23]
    #define R2_B0_RXDVS2_R_RK2_RX_DLY_RIS_DQS_SCALE_B0         Fld(2, 24) //[25:24]
    #define R2_B0_RXDVS2_R_RK2_RX_DLY_RIS_DQ_SCALE_B0          Fld(2, 26) //[27:26]
    #define R2_B0_RXDVS2_R_RK2_RX_DLY_RIS_TRACK_GATE_ENA_B0    Fld(1, 28) //[28:28]
    #define R2_B0_RXDVS2_R_RK2_DVS_FDLY_MODE_B0                Fld(1, 29) //[29:29]
    #define R2_B0_RXDVS2_R_RK2_DVS_MODE_B0                     Fld(2, 30) //[31:30]

#define DDRPHY_R2_B0_RXDVS7                         (DDRPHY_BASE_ADDR + 0x00A1C)
    #define R2_B0_RXDVS7_RG_RK2_ARDQ_MIN_DLY_B0                Fld(6, 0) //[5:0]
    #define R2_B0_RXDVS7_RG_RK2_ARDQ_MIN_DLY_B0_RFU            Fld(2, 6) //[7:6]
    #define R2_B0_RXDVS7_RG_RK2_ARDQ_MAX_DLY_B0                Fld(6, 8) //[13:8]
    #define R2_B0_RXDVS7_RG_RK2_ARDQ_MAX_DLY_B0_RFU            Fld(2, 14) //[15:14]
    #define R2_B0_RXDVS7_RG_RK2_ARDQS0_MIN_DLY_B0              Fld(7, 16) //[22:16]
    #define R2_B0_RXDVS7_RG_RK2_ARDQS0_MIN_DLY_B0_RFU          Fld(1, 23) //[23:23]
    #define R2_B0_RXDVS7_RG_RK2_ARDQS0_MAX_DLY_B0              Fld(7, 24) //[30:24]
    #define R2_B0_RXDVS7_RG_RK2_ARDQS0_MAX_DLY_B0_RFU          Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0XA20                            (DDRPHY_BASE_ADDR + 0x00A20)
    #define RFU_0XA20_RESERVED_0XA20                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XA24                            (DDRPHY_BASE_ADDR + 0x00A24)
    #define RFU_0XA24_RESERVED_0XA24                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XA28                            (DDRPHY_BASE_ADDR + 0x00A28)
    #define RFU_0XA28_RESERVED_0XA28                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XA2C                            (DDRPHY_BASE_ADDR + 0x00A2C)
    #define RFU_0XA2C_RESERVED_0XA2C                           Fld(32, 0) //[31:0]

#define DDRPHY_R2_B1_RXDVS0                         (DDRPHY_BASE_ADDR + 0x00A80)
    #define R2_B1_RXDVS0_R_RK2_B1_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R2_B1_RXDVS0_R_RK2_B1_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R2_B1_RXDVS0_R_RK2_B1_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R2_B1_RXDVS1                         (DDRPHY_BASE_ADDR + 0x00A84)
    #define R2_B1_RXDVS1_R_RK2_B1_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R2_B1_RXDVS1_R_RK2_B1_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R2_B1_RXDVS2                         (DDRPHY_BASE_ADDR + 0x00A88)
    #define R2_B1_RXDVS2_R_RK2_RX_DLY_FAL_DQS_SCALE_B1         Fld(2, 16) //[17:16]
    #define R2_B1_RXDVS2_R_RK2_RX_DLY_FAL_DQ_SCALE_B1          Fld(2, 18) //[19:18]
    #define R2_B1_RXDVS2_R_RK2_RX_DLY_FAL_TRACK_GATE_ENA_B1    Fld(1, 23) //[23:23]
    #define R2_B1_RXDVS2_R_RK2_RX_DLY_RIS_DQS_SCALE_B1         Fld(2, 24) //[25:24]
    #define R2_B1_RXDVS2_R_RK2_RX_DLY_RIS_DQ_SCALE_B1          Fld(2, 26) //[27:26]
    #define R2_B1_RXDVS2_R_RK2_RX_DLY_RIS_TRACK_GATE_ENA_B1    Fld(1, 28) //[28:28]
    #define R2_B1_RXDVS2_R_RK2_DVS_FDLY_MODE_B1                Fld(1, 29) //[29:29]
    #define R2_B1_RXDVS2_R_RK2_DVS_MODE_B1                     Fld(2, 30) //[31:30]

#define DDRPHY_R2_B1_RXDVS7                         (DDRPHY_BASE_ADDR + 0x00A9C)
    #define R2_B1_RXDVS7_RG_RK2_ARDQ_MIN_DLY_B1                Fld(6, 0) //[5:0]
    #define R2_B1_RXDVS7_RG_RK2_ARDQ_MIN_DLY_B1_RFU            Fld(2, 6) //[7:6]
    #define R2_B1_RXDVS7_RG_RK2_ARDQ_MAX_DLY_B1                Fld(6, 8) //[13:8]
    #define R2_B1_RXDVS7_RG_RK2_ARDQ_MAX_DLY_B1_RFU            Fld(2, 14) //[15:14]
    #define R2_B1_RXDVS7_RG_RK2_ARDQS0_MIN_DLY_B1              Fld(7, 16) //[22:16]
    #define R2_B1_RXDVS7_RG_RK2_ARDQS0_MIN_DLY_B1_RFU          Fld(1, 23) //[23:23]
    #define R2_B1_RXDVS7_RG_RK2_ARDQS0_MAX_DLY_B1              Fld(7, 24) //[30:24]
    #define R2_B1_RXDVS7_RG_RK2_ARDQS0_MAX_DLY_B1_RFU          Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0XAA0                            (DDRPHY_BASE_ADDR + 0x00AA0)
    #define RFU_0XAA0_RESERVED_0XAA0                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XAA4                            (DDRPHY_BASE_ADDR + 0x00AA4)
    #define RFU_0XAA4_RESERVED_0XAA4                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XAA8                            (DDRPHY_BASE_ADDR + 0x00AA8)
    #define RFU_0XAA8_RESERVED_0XAA8                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XAAC                            (DDRPHY_BASE_ADDR + 0x00AAC)
    #define RFU_0XAAC_RESERVED_0XAAC                           Fld(32, 0) //[31:0]

#define DDRPHY_R2_CA_RXDVS0                         (DDRPHY_BASE_ADDR + 0x00B00)
    #define R2_CA_RXDVS0_R_RK2_CA_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R2_CA_RXDVS0_R_RK2_CA_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R2_CA_RXDVS0_R_RK2_CA_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R2_CA_RXDVS1                         (DDRPHY_BASE_ADDR + 0x00B04)
    #define R2_CA_RXDVS1_R_RK2_CA_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R2_CA_RXDVS1_R_RK2_CA_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R2_CA_RXDVS2                         (DDRPHY_BASE_ADDR + 0x00B08)
    #define R2_CA_RXDVS2_R_RK2_RX_DLY_FAL_DQS_SCALE_CA         Fld(2, 16) //[17:16]
    #define R2_CA_RXDVS2_R_RK2_RX_DLY_FAL_DQ_SCALE_CA          Fld(2, 18) //[19:18]
    #define R2_CA_RXDVS2_R_RK2_RX_DLY_FAL_TRACK_GATE_ENA_CA    Fld(1, 23) //[23:23]
    #define R2_CA_RXDVS2_R_RK2_RX_DLY_RIS_DQS_SCALE_CA         Fld(2, 24) //[25:24]
    #define R2_CA_RXDVS2_R_RK2_RX_DLY_RIS_DQ_SCALE_CA          Fld(2, 26) //[27:26]
    #define R2_CA_RXDVS2_R_RK2_RX_DLY_RIS_TRACK_GATE_ENA_CA    Fld(1, 28) //[28:28]
    #define R2_CA_RXDVS2_R_RK2_DVS_FDLY_MODE_CA                Fld(1, 29) //[29:29]
    #define R2_CA_RXDVS2_R_RK2_DVS_MODE_CA                     Fld(2, 30) //[31:30]

#define DDRPHY_R2_CA_RXDVS9                         (DDRPHY_BASE_ADDR + 0x00B24)
    #define R2_CA_RXDVS9_RG_RK2_ARCMD_MIN_DLY                  Fld(6, 0) //[5:0]
    #define R2_CA_RXDVS9_RG_RK2_ARCMD_MIN_DLY_RFU              Fld(2, 6) //[7:6]
    #define R2_CA_RXDVS9_RG_RK2_ARCMD_MAX_DLY                  Fld(6, 8) //[13:8]
    #define R2_CA_RXDVS9_RG_RK2_ARCMD_MAX_DLY_RFU              Fld(2, 14) //[15:14]
    #define R2_CA_RXDVS9_RG_RK2_ARCLK_MIN_DLY                  Fld(7, 16) //[22:16]
    #define R2_CA_RXDVS9_RG_RK2_ARCLK_MIN_DLY_RFU              Fld(1, 23) //[23:23]
    #define R2_CA_RXDVS9_RG_RK2_ARCLK_MAX_DLY                  Fld(7, 24) //[30:24]
    #define R2_CA_RXDVS9_RG_RK2_ARCLK_MAX_DLY_RFU              Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0XB28                            (DDRPHY_BASE_ADDR + 0x00B28)
    #define RFU_0XB28_RESERVED_0XB28                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XB2C                            (DDRPHY_BASE_ADDR + 0x00B2C)
    #define RFU_0XB2C_RESERVED_0XB2C                           Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B0_DQ0                           (DDRPHY_BASE_ADDR + 0x00C00)
    #define SHU_B0_DQ0_RG_TX_ARDQS0_PRE_EN_B0                  Fld(1, 4) //[4:4]
    #define SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0                Fld(3, 8) //[10:8]
    #define SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT0           Fld(1, 8) //[8:8] //Francis added
    #define SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT1           Fld(1, 9) //[9:9] //Francis added
    #define SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT2           Fld(1, 10)//[10:10] //Francis added
    #define SHU_B0_DQ0_RG_TX_ARDQS0_DRVN_PRE_B0                Fld(3, 12) //[14:12]
    #define SHU_B0_DQ0_RG_TX_ARDQ_PRE_EN_B0                    Fld(1, 20) //[20:20]
    #define SHU_B0_DQ0_RG_TX_ARDQ_DRVP_PRE_B0                  Fld(3, 24) //[26:24]
    #define SHU_B0_DQ0_RG_TX_ARDQ_DRVN_PRE_B0                  Fld(3, 28) //[30:28]
    #define SHU_B0_DQ0_R_LP4Y_WDN_MODE_DQS0                    Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B0_DQ1                           (DDRPHY_BASE_ADDR + 0x00C04)
    #define SHU_B0_DQ1_RG_TX_ARDQ_DRVP_B0                      Fld(5, 0) //[4:0]
    #define SHU_B0_DQ1_RG_TX_ARDQ_DRVN_B0                      Fld(5, 8) //[12:8]
    #define SHU_B0_DQ1_RG_TX_ARDQ_ODTP_B0                      Fld(5, 16) //[20:16]
    #define SHU_B0_DQ1_RG_TX_ARDQ_ODTN_B0                      Fld(5, 24) //[28:24]

#define DDRPHY_SHU_B0_DQ2                           (DDRPHY_BASE_ADDR + 0x00C08)
    #define SHU_B0_DQ2_RG_TX_ARDQS0_DRVP_B0                    Fld(5, 0) //[4:0]
    #define SHU_B0_DQ2_RG_TX_ARDQS0_DRVN_B0                    Fld(5, 8) //[12:8]
    #define SHU_B0_DQ2_RG_TX_ARDQS0_ODTP_B0                    Fld(5, 16) //[20:16]
    #define SHU_B0_DQ2_RG_TX_ARDQS0_ODTN_B0                    Fld(5, 24) //[28:24]

#define DDRPHY_SHU_B0_DQ3                           (DDRPHY_BASE_ADDR + 0x00C0C)
    #define SHU_B0_DQ3_RG_TX_ARDQS0_PU_B0                      Fld(2, 0) //[1:0]
    #define SHU_B0_DQ3_RG_TX_ARDQS0_PU_PRE_B0                  Fld(2, 2) //[3:2]
    #define SHU_B0_DQ3_RG_TX_ARDQS0_PDB_B0                     Fld(2, 4) //[5:4]
    #define SHU_B0_DQ3_RG_TX_ARDQS0_PDB_PRE_B0                 Fld(2, 6) //[7:6]
    #define SHU_B0_DQ3_RG_TX_ARDQ_PU_B0                        Fld(2, 8) //[9:8]
    #define SHU_B0_DQ3_RG_TX_ARDQ_PU_PRE_B0                    Fld(2, 10) //[11:10]
    #define SHU_B0_DQ3_RG_TX_ARDQ_PDB_B0                       Fld(2, 12) //[13:12]
    #define SHU_B0_DQ3_RG_TX_ARDQ_PDB_PRE_B0                   Fld(2, 14) //[15:14]

#define DDRPHY_SHU_B0_DQ4                           (DDRPHY_BASE_ADDR + 0x00C10)
    #define SHU_B0_DQ4_RG_ARPI_AA_MCK_DL_B0                    Fld(6, 0) //[5:0]
    #define SHU_B0_DQ4_RG_ARPI_AA_MCK_FB_DL_B0                 Fld(6, 8) //[13:8]
    #define SHU_B0_DQ4_RG_ARPI_DA_MCK_FB_DL_B0                 Fld(6, 16) //[21:16]

#define DDRPHY_SHU_B0_DQ5                           (DDRPHY_BASE_ADDR + 0x00C14)
    #define SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0                  Fld(6, 0) //[5:0]
    #define SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0               Fld(1, 6) //[6:6]
    #define SHU_B0_DQ5_RG_ARPI_FB_B0                           Fld(6, 8) //[13:8]
    #define SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0              Fld(3, 16) //[18:16]
    #define SHU_B0_DQ5_DA_RX_ARDQS_DQSIEN_RB_DLY_B0            Fld(1, 19) //[19:19]
    #define SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0                 Fld(3, 20) //[22:20]
    #define SHU_B0_DQ5_RG_ARPI_MCTL_B0                         Fld(6, 24) //[29:24]

#define DDRPHY_SHU_B0_DQ6                           (DDRPHY_BASE_ADDR + 0x00C18)
    #define SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0                Fld(6, 0) //[5:0]
    #define SHU_B0_DQ6_RG_ARPI_RESERVE_B0                      Fld(16, 6) //[21:6]
    #define SHU_B0_DQ6_RG_ARPI_MIDPI_CAP_SEL_B0                Fld(2, 22) //[23:22]
    #define SHU_B0_DQ6_RG_ARPI_MIDPI_VTH_SEL_B0                Fld(2, 24) //[25:24]
    #define SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0                     Fld(1, 26) //[26:26]
    #define SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0              Fld(1, 27) //[27:27]
    #define SHU_B0_DQ6_RG_ARPI_CAP_SEL_B0                      Fld(2, 28) //[29:28]
    #define SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0                  Fld(1, 30) //[30:30]
    #define SHU_B0_DQ6_RG_ARPI_MIDPI_BYPASS_EN_B0              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B0_DQ7                           (DDRPHY_BASE_ADDR + 0x00C1C)
    #define SHU_B0_DQ7_R_DMRANKRXDVS_B0                        Fld(4, 0) //[3:0]
    #define SHU_B0_DQ7_MIDPI_ENABLE                            Fld(1, 4) //[4:4]
    #define SHU_B0_DQ7_MIDPI_DIV4_ENABLE                       Fld(1, 5) //[5:5]
    #define SHU_B0_DQ7_R_DMDQMDBI_EYE_SHU_B0                   Fld(1, 6) //[6:6]
    #define SHU_B0_DQ7_R_DMDQMDBI_SHU_B0                       Fld(1, 7) //[7:7]
    #define SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0                Fld(4, 8) //[11:8]
    #define SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0             Fld(1, 12) //[12:12]
    #define SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0               Fld(1, 13) //[13:13]
    #define SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0                   Fld(1, 14) //[14:14]
    #define SHU_B0_DQ7_R_DMRODTEN_B0                           Fld(1, 15) //[15:15]
    #define SHU_B0_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B0            Fld(1, 16) //[16:16]
    #define SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0                Fld(1, 17) //[17:17]
    #define SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0               Fld(1, 18) //[18:18]
    #define SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0               Fld(1, 19) //[19:19]
    #define SHU_B0_DQ7_R_LP4Y_SDN_MODE_DQS0                    Fld(1, 20) //[20:20]
    #define SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT                      Fld(1, 21) //[21:21]
    #define SHU_B0_DQ7_R_DMRDSEL_LOBYTE_OPT                    Fld(1, 22) //[22:22]
    #define SHU_B0_DQ7_R_DMRDSEL_HIBYTE_OPT                    Fld(1, 23) //[23:23]
    #define SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0                     Fld(1, 24) //[24:24]
    #define SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0                    Fld(3, 25) //[27:25]
    #define SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0                    Fld(1, 28) //[28:28]
    #define SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0                   Fld(3, 29) //[31:29]

#define DDRPHY_SHU_B0_DQ8                           (DDRPHY_BASE_ADDR + 0x00C20)
    #define SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0              Fld(15, 0) //[14:0]
    #define SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0               Fld(1, 15) //[15:15]
    #define SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0            Fld(1, 19) //[19:19]
    #define SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0                     Fld(1, 20) //[20:20]
    #define SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0                   Fld(1, 21) //[21:21]
    #define SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0           Fld(1, 22) //[22:22]
    #define SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0       Fld(1, 23) //[23:23]
    #define SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0                      Fld(1, 24) //[24:24]
    #define SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0                 Fld(1, 25) //[25:25]
    #define SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0           Fld(1, 26) //[26:26]
    #define SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0           Fld(1, 27) //[27:27]
    #define SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0          Fld(1, 28) //[28:28]
    #define SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0      Fld(1, 29) //[29:29]
    #define SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0                  Fld(1, 30) //[30:30]
    #define SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B0_DQ9                           (DDRPHY_BASE_ADDR + 0x00C24)
    #define SHU_B0_DQ9_RESERVED_0XC24                          Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B0_DQ10                          (DDRPHY_BASE_ADDR + 0x00C28)
    #define SHU_B0_DQ10_RESERVED_0XC28                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B0_DQ11                          (DDRPHY_BASE_ADDR + 0x00C2C)
    #define SHU_B0_DQ11_RESERVED_0XC2C                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B0_DQ12                          (DDRPHY_BASE_ADDR + 0x00C30)
    #define SHU_B0_DQ12_RESERVED_0XC30                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B0_DLL0                          (DDRPHY_BASE_ADDR + 0x00C34)
    #define SHU_B0_DLL0_RG_ARPISM_MCK_SEL_B0_SHU               Fld(1, 0) //[0:0]
    #define SHU_B0_DLL0_RG_ARDLL_DIV_MCTL_B0                   Fld(2, 1) //[2:1]
    #define SHU_B0_DLL0_B0_DLL0_RFU                            Fld(1, 3) //[3:3]
    #define SHU_B0_DLL0_RG_ARDLL_FAST_PSJP_B0                  Fld(1, 4) //[4:4]
    #define SHU_B0_DLL0_RG_ARDLL_PHDIV_B0                      Fld(1, 9) //[9:9]
    #define SHU_B0_DLL0_RG_ARDLL_PHJUMP_EN_B0                  Fld(1, 10) //[10:10]
    #define SHU_B0_DLL0_RG_ARDLL_P_GAIN_B0                     Fld(4, 12) //[15:12]
    #define SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0                    Fld(4, 16) //[19:16]
    #define SHU_B0_DLL0_RG_ARDLL_GAIN_B0                       Fld(4, 20) //[23:20]
    #define SHU_B0_DLL0_RG_ARDLL_PHDET_IN_SWAP_B0              Fld(1, 30) //[30:30]
    #define SHU_B0_DLL0_RG_ARDLL_PHDET_OUT_SEL_B0              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B0_DLL1                          (DDRPHY_BASE_ADDR + 0x00C38)
    #define SHU_B0_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B0              Fld(1, 0) //[0:0]
    #define SHU_B0_DLL1_RG_ARDLL_PS_EN_B0                      Fld(1, 1) //[1:1]
    #define SHU_B0_DLL1_RG_ARDLL_PD_CK_SEL_B0                  Fld(1, 2) //[2:2]
    #define SHU_B0_DLL1_RG_ARDQ_REV_B0                         Fld(24, 8) //[31:8]
    #define SHU_B0_DLL1_RG_ARDQ_REV_B0_31                      Fld(1, 31) /* cc add */

#define DDRPHY_SHU_B0_DLL2                          (DDRPHY_BASE_ADDR + 0x00C3C)
    #define SHU_B0_DLL2_RG_ARDQ_REV_MSB_B0                     Fld(8, 0) //[7:0]

#define DDRPHY_SHU_B1_DQ0                           (DDRPHY_BASE_ADDR + 0x00C80)
    #define SHU_B1_DQ0_RG_TX_ARDQS0_PRE_EN_B1                  Fld(1, 4) //[4:4]
    #define SHU_B1_DQ0_RG_TX_ARDQS0_DRVP_PRE_B1                Fld(3, 8) //[10:8]
    #define SHU_B1_DQ0_RG_TX_ARDQS0_DRVP_PRE_B1_BIT0           Fld(1, 8) //[8:8]//Francis added
    #define SHU_B1_DQ0_RG_TX_ARDQS0_DRVP_PRE_B1_BIT1           Fld(1, 9) //[9:9]//Francis added
    #define SHU_B1_DQ0_RG_TX_ARDQS0_DRVP_PRE_B1_BIT2           Fld(1,10) //[10:10] //Francis added
    #define SHU_B1_DQ0_RG_TX_ARDQS0_DRVN_PRE_B1                Fld(3, 12) //[14:12]
    #define SHU_B1_DQ0_RG_TX_ARDQ_PRE_EN_B1                    Fld(1, 20) //[20:20]
    #define SHU_B1_DQ0_RG_TX_ARDQ_DRVP_PRE_B1                  Fld(3, 24) //[26:24]
    #define SHU_B1_DQ0_RG_TX_ARDQ_DRVN_PRE_B1                  Fld(3, 28) //[30:28]
    #define SHU_B1_DQ0_R_LP4Y_WDN_MODE_DQS1                    Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B1_DQ1                           (DDRPHY_BASE_ADDR + 0x00C84)
    #define SHU_B1_DQ1_RG_TX_ARDQ_DRVP_B1                      Fld(5, 0) //[4:0]
    #define SHU_B1_DQ1_RG_TX_ARDQ_DRVN_B1                      Fld(5, 8) //[12:8]
    #define SHU_B1_DQ1_RG_TX_ARDQ_ODTP_B1                      Fld(5, 16) //[20:16]
    #define SHU_B1_DQ1_RG_TX_ARDQ_ODTN_B1                      Fld(5, 24) //[28:24]

#define DDRPHY_SHU_B1_DQ2                           (DDRPHY_BASE_ADDR + 0x00C88)
    #define SHU_B1_DQ2_RG_TX_ARDQS0_DRVP_B1                    Fld(5, 0) //[4:0]
    #define SHU_B1_DQ2_RG_TX_ARDQS0_DRVN_B1                    Fld(5, 8) //[12:8]
    #define SHU_B1_DQ2_RG_TX_ARDQS0_ODTP_B1                    Fld(5, 16) //[20:16]
    #define SHU_B1_DQ2_RG_TX_ARDQS0_ODTN_B1                    Fld(5, 24) //[28:24]

#define DDRPHY_SHU_B1_DQ3                           (DDRPHY_BASE_ADDR + 0x00C8C)
    #define SHU_B1_DQ3_RG_TX_ARDQS0_PU_B1                      Fld(2, 0) //[1:0]
    #define SHU_B1_DQ3_RG_TX_ARDQS0_PU_PRE_B1                  Fld(2, 2) //[3:2]
    #define SHU_B1_DQ3_RG_TX_ARDQS0_PDB_B1                     Fld(2, 4) //[5:4]
    #define SHU_B1_DQ3_RG_TX_ARDQS0_PDB_PRE_B1                 Fld(2, 6) //[7:6]
    #define SHU_B1_DQ3_RG_TX_ARDQ_PU_B1                        Fld(2, 8) //[9:8]
    #define SHU_B1_DQ3_RG_TX_ARDQ_PU_PRE_B1                    Fld(2, 10) //[11:10]
    #define SHU_B1_DQ3_RG_TX_ARDQ_PDB_B1                       Fld(2, 12) //[13:12]
    #define SHU_B1_DQ3_RG_TX_ARDQ_PDB_PRE_B1                   Fld(2, 14) //[15:14]

#define DDRPHY_SHU_B1_DQ4                           (DDRPHY_BASE_ADDR + 0x00C90)
    #define SHU_B1_DQ4_RG_ARPI_AA_MCK_DL_B1                    Fld(6, 0) //[5:0]
    #define SHU_B1_DQ4_RG_ARPI_AA_MCK_FB_DL_B1                 Fld(6, 8) //[13:8]
    #define SHU_B1_DQ4_RG_ARPI_DA_MCK_FB_DL_B1                 Fld(6, 16) //[21:16]

#define DDRPHY_SHU_B1_DQ5                           (DDRPHY_BASE_ADDR + 0x00C94)
    #define SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1                  Fld(6, 0) //[5:0]
    #define SHU_B1_DQ5_RG_RX_ARDQ_VREF_BYPASS_B1               Fld(1, 6) //[6:6]
    #define SHU_B1_DQ5_RG_ARPI_FB_B1                           Fld(6, 8) //[13:8]
    #define SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1              Fld(3, 16) //[18:16]
    #define SHU_B1_DQ5_DA_RX_ARDQS_DQSIEN_RB_DLY_B1            Fld(1, 19) //[19:19]
    #define SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1                 Fld(3, 20) //[22:20]
    #define SHU_B1_DQ5_RG_ARPI_MCTL_B1                         Fld(6, 24) //[29:24]

#define DDRPHY_SHU_B1_DQ6                           (DDRPHY_BASE_ADDR + 0x00C98)
    #define SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1                Fld(6, 0) //[5:0]
    #define SHU_B1_DQ6_RG_ARPI_RESERVE_B1                      Fld(16, 6) //[21:6]
    #define SHU_B1_DQ6_RG_ARPI_MIDPI_CAP_SEL_B1                Fld(2, 22) //[23:22]
    #define SHU_B1_DQ6_RG_ARPI_MIDPI_VTH_SEL_B1                Fld(2, 24) //[25:24]
    #define SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1                     Fld(1, 26) //[26:26]
    #define SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1              Fld(1, 27) //[27:27]
    #define SHU_B1_DQ6_RG_ARPI_CAP_SEL_B1                      Fld(2, 28) //[29:28]
    #define SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1                  Fld(1, 30) //[30:30]
    #define SHU_B1_DQ6_RG_ARPI_MIDPI_BYPASS_EN_B1              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B1_DQ7                           (DDRPHY_BASE_ADDR + 0x00C9C)
    #define SHU_B1_DQ7_R_DMRANKRXDVS_B1                        Fld(4, 0) //[3:0]
    #define SHU_B1_DQ7_R_DMDQMDBI_EYE_SHU_B1                   Fld(1, 6) //[6:6]
    #define SHU_B1_DQ7_R_DMDQMDBI_SHU_B1                       Fld(1, 7) //[7:7]
    #define SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1                Fld(4, 8) //[11:8]
    #define SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1             Fld(1, 12) //[12:12]
    #define SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1               Fld(1, 13) //[13:13]
    #define SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1                   Fld(1, 14) //[14:14]
    #define SHU_B1_DQ7_R_DMRODTEN_B1                           Fld(1, 15) //[15:15]
    #define SHU_B1_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B1            Fld(1, 16) //[16:16]
    #define SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1                Fld(1, 17) //[17:17]
    #define SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1               Fld(1, 18) //[18:18]
    #define SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1               Fld(1, 19) //[19:19]
    #define SHU_B1_DQ7_R_LP4Y_SDN_MODE_DQS1                    Fld(1, 20) //[20:20]
    #define SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1                     Fld(1, 24) //[24:24]
    #define SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1                    Fld(3, 25) //[27:25]
    #define SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1                    Fld(1, 28) //[28:28]
    #define SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1                   Fld(3, 29) //[31:29]

#define DDRPHY_SHU_B1_DQ8                           (DDRPHY_BASE_ADDR + 0x00CA0)
    #define SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1              Fld(15, 0) //[14:0]
    #define SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1               Fld(1, 15) //[15:15]
    #define SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1            Fld(1, 19) //[19:19]
    #define SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1                     Fld(1, 20) //[20:20]
    #define SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1                   Fld(1, 21) //[21:21]
    #define SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1           Fld(1, 22) //[22:22]
    #define SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1       Fld(1, 23) //[23:23]
    #define SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1                      Fld(1, 24) //[24:24]
    #define SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1                 Fld(1, 25) //[25:25]
    #define SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1           Fld(1, 26) //[26:26]
    #define SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1           Fld(1, 27) //[27:27]
    #define SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1          Fld(1, 28) //[28:28]
    #define SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1      Fld(1, 29) //[29:29]
    #define SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1                  Fld(1, 30) //[30:30]
    #define SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B1_DQ9                           (DDRPHY_BASE_ADDR + 0x00CA4)
    #define SHU_B1_DQ9_RESERVED_0XCA4                          Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B1_DQ10                          (DDRPHY_BASE_ADDR + 0x00CA8)
    #define SHU_B1_DQ10_RESERVED_0XCA8                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B1_DQ11                          (DDRPHY_BASE_ADDR + 0x00CAC)
    #define SHU_B1_DQ11_RESERVED_0XCAC                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B1_DQ12                          (DDRPHY_BASE_ADDR + 0x00CB0)
    #define SHU_B1_DQ12_RESERVED_0XCB0                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B1_DLL0                          (DDRPHY_BASE_ADDR + 0x00CB4)
    #define SHU_B1_DLL0_RG_ARPISM_MCK_SEL_B1_SHU               Fld(1, 0) //[0:0]
    #define SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1                   Fld(2, 1) //[2:1]
    #define SHU_B1_DLL0_B1_DLL0_RFU                            Fld(1, 3) //[3:3]
    #define SHU_B1_DLL0_RG_ARDLL_FAST_PSJP_B1                  Fld(1, 4) //[4:4]
    #define SHU_B1_DLL0_RG_ARDLL_PHDIV_B1                      Fld(1, 9) //[9:9]
    #define SHU_B1_DLL0_RG_ARDLL_PHJUMP_EN_B1                  Fld(1, 10) //[10:10]
    #define SHU_B1_DLL0_RG_ARDLL_P_GAIN_B1                     Fld(4, 12) //[15:12]
    #define SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1                    Fld(4, 16) //[19:16]
    #define SHU_B1_DLL0_RG_ARDLL_GAIN_B1                       Fld(4, 20) //[23:20]
    #define SHU_B1_DLL0_RG_ARDLL_PHDET_IN_SWAP_B1              Fld(1, 30) //[30:30]
    #define SHU_B1_DLL0_RG_ARDLL_PHDET_OUT_SEL_B1              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B1_DLL1                          (DDRPHY_BASE_ADDR + 0x00CB8)
    #define SHU_B1_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B1              Fld(1, 0) //[0:0]
    #define SHU_B1_DLL1_RG_ARDLL_PS_EN_B1                      Fld(1, 1) //[1:1]
    #define SHU_B1_DLL1_RG_ARDLL_PD_CK_SEL_B1                  Fld(1, 2) //[2:2]
    #define SHU_B1_DLL1_RG_ARDQ_REV_B1                         Fld(24, 8) //[31:8]
    #define SHU_B1_DLL1_RG_ARDQ_REV_B1_31                      Fld(1, 31) /* cc add */

#define DDRPHY_SHU_B1_DLL2                          (DDRPHY_BASE_ADDR + 0x00CBC)
    #define SHU_B1_DLL2_RG_ARDQ_REV_MSB_B1                     Fld(8, 0) //[7:0]

#define DDRPHY_SHU_CA_CMD0                          (DDRPHY_BASE_ADDR + 0x00D00)
    #define SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN                     Fld(1, 4) //[4:4]
    #define SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE                   Fld(3, 8) //[10:8]
    #define SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE_BIT0              Fld(1, 8) //[8:8] //Francis added
    #define SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE_BIT1              Fld(1, 9) //[9:9] //Francis added
    #define SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE_BIT2              Fld(1, 10) //[10:10] //Francis added
    #define SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE                   Fld(3, 12) //[14:12]
    #define SHU_CA_CMD0_RG_CGEN_FMEM_CK_CG_DLL                 Fld(1, 17) //[17:17]
    #define SHU_CA_CMD0_RG_FB_CK_MUX                           Fld(2, 18) //[19:18]
    #define SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN                     Fld(1, 20) //[20:20]
    #define SHU_CA_CMD0_RG_TX_ARCMD_DRVP_PRE                   Fld(3, 24) //[26:24]
    #define SHU_CA_CMD0_RG_TX_ARCMD_DRVN_PRE                   Fld(3, 28) //[30:28]
    #define SHU_CA_CMD0_R_LP4Y_WDN_MODE_CLK                    Fld(1, 31) //[31:31]

#define DDRPHY_SHU_CA_CMD1                          (DDRPHY_BASE_ADDR + 0x00D04)
    #define SHU_CA_CMD1_RG_TX_ARCMD_DRVP                       Fld(5, 0) //[4:0]
    #define SHU_CA_CMD1_RG_TX_ARCMD_DRVN                       Fld(5, 8) //[12:8]
    #define SHU_CA_CMD1_RG_TX_ARCMD_ODTP                       Fld(5, 16) //[20:16]
    #define SHU_CA_CMD1_RG_TX_ARCMD_ODTN                       Fld(5, 24) //[28:24]

#define DDRPHY_SHU_CA_CMD2                          (DDRPHY_BASE_ADDR + 0x00D08)
    #define SHU_CA_CMD2_RG_TX_ARCLK_DRVP                       Fld(5, 0) //[4:0]
    #define SHU_CA_CMD2_RG_TX_ARCLK_DRVN                       Fld(5, 8) //[12:8]
    #define SHU_CA_CMD2_RG_TX_ARCLK_ODTP                       Fld(5, 16) //[20:16]
    #define SHU_CA_CMD2_RG_TX_ARCLK_ODTN                       Fld(5, 24) //[28:24]

#define DDRPHY_SHU_CA_CMD3                          (DDRPHY_BASE_ADDR + 0x00D0C)
    #define SHU_CA_CMD3_RG_TX_ARCLK_PU                         Fld(2, 0) //[1:0]
    #define SHU_CA_CMD3_RG_TX_ARCLK_PU_PRE                     Fld(2, 2) //[3:2]
    #define SHU_CA_CMD3_RG_TX_ARCLK_PDB                        Fld(2, 4) //[5:4]
    #define SHU_CA_CMD3_RG_TX_ARCLK_PDB_PRE                    Fld(2, 6) //[7:6]
    #define SHU_CA_CMD3_RG_TX_ARCMD_PU                         Fld(2, 8) //[9:8]
    #define SHU_CA_CMD3_RG_TX_ARCMD_PU_BIT0                    Fld(1, 8)  //[8:8 //Francis added
    #define SHU_CA_CMD3_RG_TX_ARCMD_PU_BIT1                    Fld(1, 9)  //[9:9] //Francis added
    #define SHU_CA_CMD3_RG_TX_ARCMD_PU_PRE                     Fld(2, 10) //[11:10]
    #define SHU_CA_CMD3_RG_TX_ARCMD_PDB                        Fld(2, 12) //[13:12]
    #define SHU_CA_CMD3_RG_TX_ARCMD_PDB_PRE                    Fld(2, 14) //[15:14]

#define DDRPHY_SHU_CA_CMD4                          (DDRPHY_BASE_ADDR + 0x00D10)
    #define SHU_CA_CMD4_RG_ARPI_AA_MCK_DL_CA                   Fld(6, 0) //[5:0]
    #define SHU_CA_CMD4_RG_ARPI_AA_MCK_FB_DL_CA                Fld(6, 8) //[13:8]
    #define SHU_CA_CMD4_RG_ARPI_DA_MCK_FB_DL_CA                Fld(6, 16) //[21:16]

#define DDRPHY_SHU_CA_CMD5                          (DDRPHY_BASE_ADDR + 0x00D14)
    #define SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL                   Fld(6, 0) //[5:0]
    #define SHU_CA_CMD5_RG_RX_ARCMD_VREF_BYPASS                Fld(1, 6) //[6:6]
    #define SHU_CA_CMD5_RG_ARPI_FB_CA                          Fld(6, 8) //[13:8]
    #define SHU_CA_CMD5_RG_RX_ARCLK_DQSIEN_DLY                 Fld(3, 16) //[18:16]
    #define SHU_CA_CMD5_DA_RX_ARCLK_DQSIEN_RB_DLY              Fld(1, 19) //[19:19]
    #define SHU_CA_CMD5_RG_RX_ARCLK_DVS_DLY                    Fld(3, 20) //[22:20]
    #define SHU_CA_CMD5_RG_ARPI_MCTL_CA                        Fld(6, 24) //[29:24]

#define DDRPHY_SHU_CA_CMD6                          (DDRPHY_BASE_ADDR + 0x00D18)
    #define SHU_CA_CMD6_RG_ARPI_OFFSET_CLKIEN                  Fld(6, 0) //[5:0]
    #define SHU_CA_CMD6_RG_ARPI_RESERVE_CA                     Fld(16, 6) //[21:6]
    #define SHU_CA_CMD6_RG_ARPI_MIDPI_CAP_SEL_CA               Fld(2, 22) //[23:22]
    #define SHU_CA_CMD6_RG_ARPI_MIDPI_VTH_SEL_CA               Fld(2, 24) //[25:24]
    #define SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA                    Fld(1, 26) //[26:26]
    #define SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA             Fld(1, 27) //[27:27]
    #define SHU_CA_CMD6_RG_ARPI_CAP_SEL_CA                     Fld(2, 28) //[29:28]
    #define SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE                   Fld(1, 30) //[30:30]
    #define SHU_CA_CMD6_RG_ARPI_MIDPI_BYPASS_EN_CA             Fld(1, 31) //[31:31]

#define DDRPHY_SHU_CA_CMD7                          (DDRPHY_BASE_ADDR + 0x00D1C)
    #define SHU_CA_CMD7_R_DMRANKRXDVS_CA                       Fld(4, 0) //[3:0]
    #define SHU_CA_CMD7_R_DMRXDVS_PBYTE_FLAG_OPT_CA            Fld(1, 12) //[12:12]
    #define SHU_CA_CMD7_R_DMRODTEN_CA                          Fld(1, 15) //[15:15]
    #define SHU_CA_CMD7_R_DMARPI_CG_FB2DLL_DCM_EN_CA           Fld(1, 16) //[16:16]
    #define SHU_CA_CMD7_R_DMTX_ARPI_CG_CMD_NEW                 Fld(1, 17) //[17:17]
    #define SHU_CA_CMD7_R_DMTX_ARPI_CG_CLK_NEW                 Fld(1, 18) //[18:18]
    #define SHU_CA_CMD7_R_DMTX_ARPI_CG_CS_NEW                  Fld(1, 19) //[19:19]
    #define SHU_CA_CMD7_R_LP4Y_SDN_MODE_CLK                    Fld(1, 20) //[20:20]
    #define SHU_CA_CMD7_R_DMRXRANK_CMD_EN                      Fld(1, 24) //[24:24]
    #define SHU_CA_CMD7_R_DMRXRANK_CMD_LAT                     Fld(3, 25) //[27:25]
    #define SHU_CA_CMD7_R_DMRXRANK_CLK_EN                      Fld(1, 28) //[28:28]
    #define SHU_CA_CMD7_R_DMRXRANK_CLK_LAT                     Fld(3, 29) //[31:29]

#define DDRPHY_SHU_CA_CMD8                          (DDRPHY_BASE_ADDR + 0x00D20)
    #define SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_CYC_CA             Fld(15, 0) //[14:0]
    #define SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_EN_CA              Fld(1, 15) //[15:15]
    #define SHU_CA_CMD8_R_DMRANK_RXDLY_PIPE_CG_IG_CA           Fld(1, 19) //[19:19]
    #define SHU_CA_CMD8_R_RMRODTEN_CG_IG_CA                    Fld(1, 20) //[20:20]
    #define SHU_CA_CMD8_R_RMRX_TOPHY_CG_IG_CA                  Fld(1, 21) //[21:21]
    #define SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA          Fld(1, 22) //[22:22]
    #define SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA      Fld(1, 23) //[23:23]
    #define SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA                     Fld(1, 24) //[24:24]
    #define SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA                Fld(1, 25) //[25:25]
    #define SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA          Fld(1, 26) //[26:26]
    #define SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA          Fld(1, 27) //[27:27]
    #define SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA         Fld(1, 28) //[28:28]
    #define SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA     Fld(1, 29) //[29:29]
    #define SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA                 Fld(1, 30) //[30:30]
    #define SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA             Fld(1, 31) //[31:31]

#define DDRPHY_SHU_CA_CMD9                          (DDRPHY_BASE_ADDR + 0x00D24)
    #define SHU_CA_CMD9_RESERVED_0XD24                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_CA_CMD10                         (DDRPHY_BASE_ADDR + 0x00D28)
    #define SHU_CA_CMD10_RESERVED_0XD28                        Fld(32, 0) //[31:0]

#define DDRPHY_SHU_CA_CMD11                         (DDRPHY_BASE_ADDR + 0x00D2C)
    #define SHU_CA_CMD11_RG_RIMP_REV                           Fld(8, 0) //[7:0]
    #define SHU_CA_CMD11_RG_RIMP_VREF_SEL                      Fld(6, 8) //[13:8]
    #define SHU_CA_CMD11_RG_TX_ARCKE_DRVP                      Fld(5, 17) //[21:17]
    #define SHU_CA_CMD11_RG_TX_ARCKE_DRVN                      Fld(5, 22) //[26:22]

#define DDRPHY_SHU_CA_CMD12                         (DDRPHY_BASE_ADDR + 0x00D30)
    #define SHU_CA_CMD12_RESERVED_0XD30                        Fld(32, 0) //[31:0]

#define DDRPHY_SHU_CA_DLL0                          (DDRPHY_BASE_ADDR + 0x00D34)
    #define SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU               Fld(1, 0) //[0:0]
    #define SHU_CA_DLL0_RG_ARDLL_DIV_MCTL_CA                   Fld(2, 1) //[2:1]
    #define SHU_CA_DLL0_CA_DLL0_RFU                            Fld(1, 3) //[3:3]
    #define SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA                  Fld(1, 4) //[4:4]
    #define SHU_CA_DLL0_RG_ARDLL_PHDIV_CA                      Fld(1, 9) //[9:9]
    #define SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA                  Fld(1, 10) //[10:10]
    #define SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA                     Fld(4, 12) //[15:12]
    #define SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA                    Fld(4, 16) //[19:16]
    #define SHU_CA_DLL0_RG_ARDLL_GAIN_CA                       Fld(4, 20) //[23:20]
    #define SHU_CA_DLL0_RG_ARDLL_PHDET_IN_SWAP_CA              Fld(1, 30) //[30:30]
    #define SHU_CA_DLL0_RG_ARDLL_PHDET_OUT_SEL_CA              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_CA_DLL1                          (DDRPHY_BASE_ADDR + 0x00D38)
    #define SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA              Fld(1, 0) //[0:0]
    #define SHU_CA_DLL1_RG_ARDLL_PS_EN_CA                      Fld(1, 1) //[1:1]
    #define SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA                  Fld(1, 2) //[2:2]
    #define SHU_CA_DLL1_RG_ARCMD_REV                           Fld(24, 8) //[31:8]
    #define SHU_CA_DLL1_RG_ARCMD_REV_31                        Fld(1, 31) /* cc add */

#define DDRPHY_SHU_CA_DLL2                          (DDRPHY_BASE_ADDR + 0x00D3C)
    #define SHU_CA_DLL2_RG_ARCMD_MSB_REV                       Fld(8, 0) //[7:0]

#define DDRPHY_SHU_MISC0                            (DDRPHY_BASE_ADDR + 0x00DF0)
    #define SHU_MISC0_R_RX_PIPE_BYPASS_EN                      Fld(1, 1) //[1:1]
    #define SHU_MISC0_RG_CMD_TXPIPE_BYPASS_EN                  Fld(1, 2) //[2:2]
    #define SHU_MISC0_RG_CK_TXPIPE_BYPASS_EN                   Fld(1, 3) //[3:3]
    #define SHU_MISC0_RG_RVREF_SEL_DQ                          Fld(6, 16) //[21:16]
    #define SHU_MISC0_RG_RVREF_DDR4_SEL                        Fld(1, 22) //[22:22]
    #define SHU_MISC0_RG_RVREF_DDR3_SEL                        Fld(1, 23) //[23:23]
    #define SHU_MISC0_RG_RVREF_SEL_CMD                         Fld(6, 24) //[29:24]

#define DDRPHY_SHU_MISC1                            (DDRPHY_BASE_ADDR + 0x00DF4)
    #define SHU_MISC1_DR_EMI_RESERVE                           Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R0_B0_DQ0                        (DDRPHY_BASE_ADDR + 0x00E00)
    #define SHU_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0                  Fld(4, 0) //[3:0]
    #define SHU_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0                  Fld(4, 4) //[7:4]
    #define SHU_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0                  Fld(4, 8) //[11:8]
    #define SHU_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0                  Fld(4, 12) //[15:12]
    #define SHU_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0                  Fld(4, 16) //[19:16]
    #define SHU_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0                  Fld(4, 20) //[23:20]
    #define SHU_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0                  Fld(4, 24) //[27:24]
    #define SHU_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R0_B0_DQ1                        (DDRPHY_BASE_ADDR + 0x00E04)
    #define SHU_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0                 Fld(4, 0) //[3:0]
    #define SHU_R0_B0_DQ1_RK0_TX_ARDQS0_DLYB_B0                Fld(4, 16) //[19:16]
    #define SHU_R0_B0_DQ1_RK0_TX_ARDQS0B_DLYB_B0               Fld(4, 20) //[23:20]
    #define SHU_R0_B0_DQ1_RK0_TX_ARDQS0_DLY_B0                 Fld(4, 24) //[27:24]
    #define SHU_R0_B0_DQ1_RK0_TX_ARDQS0B_DLY_B0                Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R0_B0_DQ2                        (DDRPHY_BASE_ADDR + 0x00E08)
    #define SHU_R0_B0_DQ2_RK0_RX_ARDQ0_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R0_B0_DQ2_RK0_RX_ARDQ0_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R0_B0_DQ2_RK0_RX_ARDQ1_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R0_B0_DQ2_RK0_RX_ARDQ1_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B0_DQ3                        (DDRPHY_BASE_ADDR + 0x00E0C)
    #define SHU_R0_B0_DQ3_RK0_RX_ARDQ2_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R0_B0_DQ3_RK0_RX_ARDQ2_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R0_B0_DQ3_RK0_RX_ARDQ3_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R0_B0_DQ3_RK0_RX_ARDQ3_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B0_DQ4                        (DDRPHY_BASE_ADDR + 0x00E10)
    #define SHU_R0_B0_DQ4_RK0_RX_ARDQ4_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R0_B0_DQ4_RK0_RX_ARDQ4_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R0_B0_DQ4_RK0_RX_ARDQ5_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R0_B0_DQ4_RK0_RX_ARDQ5_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B0_DQ5                        (DDRPHY_BASE_ADDR + 0x00E14)
    #define SHU_R0_B0_DQ5_RK0_RX_ARDQ6_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R0_B0_DQ5_RK0_RX_ARDQ6_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R0_B0_DQ5_RK0_RX_ARDQ7_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R0_B0_DQ5_RK0_RX_ARDQ7_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B0_DQ6                        (DDRPHY_BASE_ADDR + 0x00E18)
    #define SHU_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0               Fld(6, 0) //[5:0]
    #define SHU_R0_B0_DQ6_RK0_RX_ARDQM0_F_DLY_B0               Fld(6, 8) //[13:8]
    #define SHU_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0               Fld(7, 16) //[22:16]
    #define SHU_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R0_B0_DQ7                        (DDRPHY_BASE_ADDR + 0x00E1C)
    #define SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0                       Fld(6, 8) //[13:8]
    #define SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0                      Fld(6, 16) //[21:16]
    #define SHU_R0_B0_DQ7_RK0_ARPI_PBYTE_B0                    Fld(6, 24) //[29:24]

#define DDRPHY_RFU_0XE20                            (DDRPHY_BASE_ADDR + 0x00E20)
    #define RFU_0XE20_RESERVED_0XE20                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XE24                            (DDRPHY_BASE_ADDR + 0x00E24)
    #define RFU_0XE24_RESERVED_0XE24                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XE28                            (DDRPHY_BASE_ADDR + 0x00E28)
    #define RFU_0XE28_RESERVED_0XE28                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XE2C                            (DDRPHY_BASE_ADDR + 0x00E2C)
    #define RFU_0XE2C_RESERVED_0XE2C                           Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R0_B1_DQ0                        (DDRPHY_BASE_ADDR + 0x00E50)
    #define SHU_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1                  Fld(4, 0) //[3:0]
    #define SHU_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1                  Fld(4, 4) //[7:4]
    #define SHU_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1                  Fld(4, 8) //[11:8]
    #define SHU_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1                  Fld(4, 12) //[15:12]
    #define SHU_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1                  Fld(4, 16) //[19:16]
    #define SHU_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1                  Fld(4, 20) //[23:20]
    #define SHU_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1                  Fld(4, 24) //[27:24]
    #define SHU_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R0_B1_DQ1                        (DDRPHY_BASE_ADDR + 0x00E54)
    #define SHU_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1                 Fld(4, 0) //[3:0]
    #define SHU_R0_B1_DQ1_RK0_TX_ARDQS0_DLYB_B1                Fld(4, 16) //[19:16]
    #define SHU_R0_B1_DQ1_RK0_TX_ARDQS0B_DLYB_B1               Fld(4, 20) //[23:20]
    #define SHU_R0_B1_DQ1_RK0_TX_ARDQS0_DLY_B1                 Fld(4, 24) //[27:24]
    #define SHU_R0_B1_DQ1_RK0_TX_ARDQS0B_DLY_B1                Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R0_B1_DQ2                        (DDRPHY_BASE_ADDR + 0x00E58)
    #define SHU_R0_B1_DQ2_RK0_RX_ARDQ0_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R0_B1_DQ2_RK0_RX_ARDQ0_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R0_B1_DQ2_RK0_RX_ARDQ1_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R0_B1_DQ2_RK0_RX_ARDQ1_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B1_DQ3                        (DDRPHY_BASE_ADDR + 0x00E5C)
    #define SHU_R0_B1_DQ3_RK0_RX_ARDQ2_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R0_B1_DQ3_RK0_RX_ARDQ2_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R0_B1_DQ3_RK0_RX_ARDQ3_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R0_B1_DQ3_RK0_RX_ARDQ3_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B1_DQ4                        (DDRPHY_BASE_ADDR + 0x00E60)
    #define SHU_R0_B1_DQ4_RK0_RX_ARDQ4_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R0_B1_DQ4_RK0_RX_ARDQ4_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R0_B1_DQ4_RK0_RX_ARDQ5_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R0_B1_DQ4_RK0_RX_ARDQ5_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B1_DQ5                        (DDRPHY_BASE_ADDR + 0x00E64)
    #define SHU_R0_B1_DQ5_RK0_RX_ARDQ6_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R0_B1_DQ5_RK0_RX_ARDQ6_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R0_B1_DQ5_RK0_RX_ARDQ7_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R0_B1_DQ5_RK0_RX_ARDQ7_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B1_DQ6                        (DDRPHY_BASE_ADDR + 0x00E68)
    #define SHU_R0_B1_DQ6_RK0_RX_ARDQM0_R_DLY_B1               Fld(6, 0) //[5:0]
    #define SHU_R0_B1_DQ6_RK0_RX_ARDQM0_F_DLY_B1               Fld(6, 8) //[13:8]
    #define SHU_R0_B1_DQ6_RK0_RX_ARDQS0_R_DLY_B1               Fld(7, 16) //[22:16]
    #define SHU_R0_B1_DQ6_RK0_RX_ARDQS0_F_DLY_B1               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R0_B1_DQ7                        (DDRPHY_BASE_ADDR + 0x00E6C)
    #define SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1                       Fld(6, 8) //[13:8]
    #define SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1                      Fld(6, 16) //[21:16]
    #define SHU_R0_B1_DQ7_RK0_ARPI_PBYTE_B1                    Fld(6, 24) //[29:24]

#define DDRPHY_RFU_0XE70                            (DDRPHY_BASE_ADDR + 0x00E70)
    #define RFU_0XE70_RESERVED_0XE70                           Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R0_CA_CMD11                      (DDRPHY_BASE_ADDR + 0x00E74)
    #define SHU_R0_CA_CMD11_RG_RK0_RX_ARCA7_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R0_CA_CMD11_RG_RK0_RX_ARCA7_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R0_CA_CMD11_RG_RK0_RX_ARCA6_R_DLY              Fld(6, 16) //[21:16]
    #define SHU_R0_CA_CMD11_RG_RK0_RX_ARCA6_F_DLY              Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD12                      (DDRPHY_BASE_ADDR + 0x00E78)
    #define SHU_R0_CA_CMD12_RG_RK0_RX_ARCA9_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R0_CA_CMD12_RG_RK0_RX_ARCA9_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R0_CA_CMD12_RG_RK0_RX_ARCA8_R_DLY              Fld(6, 16) //[21:16]
    #define SHU_R0_CA_CMD12_RG_RK0_RX_ARCA8_F_DLY              Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD13                      (DDRPHY_BASE_ADDR + 0x00E7C)
    #define SHU_R0_CA_CMD13_RG_RK0_RX_ARCA10_R_DLY             Fld(6, 16) //[21:16]
    #define SHU_R0_CA_CMD13_RG_RK0_RX_ARCA10_F_DLY             Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD0                       (DDRPHY_BASE_ADDR + 0x00EA0)
    #define SHU_R0_CA_CMD0_RK0_TX_ARCA0_DLY                    Fld(4, 0) //[3:0]
    #define SHU_R0_CA_CMD0_RK0_TX_ARCA1_DLY                    Fld(4, 4) //[7:4]
    #define SHU_R0_CA_CMD0_RK0_TX_ARCA2_DLY                    Fld(4, 8) //[11:8]
    #define SHU_R0_CA_CMD0_RK0_TX_ARCA3_DLY                    Fld(4, 12) //[15:12]
    #define SHU_R0_CA_CMD0_RK0_TX_ARCA4_DLY                    Fld(4, 16) //[19:16]
    #define SHU_R0_CA_CMD0_RK0_TX_ARCA5_DLY                    Fld(4, 20) //[23:20]
    #define SHU_R0_CA_CMD0_RK0_TX_ARCLK_DLYB                   Fld(4, 24) //[27:24]
    #define SHU_R0_CA_CMD0_RK0_TX_ARCLKB_DLYB                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R0_CA_CMD1                       (DDRPHY_BASE_ADDR + 0x00EA4)
    #define SHU_R0_CA_CMD1_RK0_TX_ARCKE0_DLY                   Fld(4, 0) //[3:0]
    #define SHU_R0_CA_CMD1_RK0_TX_ARCKE1_DLY                   Fld(4, 4) //[7:4]
    #define SHU_R0_CA_CMD1_RK0_TX_ARCKE2_DLY                   Fld(4, 8) //[11:8]
    #define SHU_R0_CA_CMD1_RK0_TX_ARCS0_DLY                    Fld(4, 12) //[15:12]
    #define SHU_R0_CA_CMD1_RK0_TX_ARCS1_DLY                    Fld(4, 16) //[19:16]
    #define SHU_R0_CA_CMD1_RK0_TX_ARCS2_DLY                    Fld(4, 20) //[23:20]
    #define SHU_R0_CA_CMD1_RK0_TX_ARCLK_DLY                    Fld(4, 24) //[27:24]
    #define SHU_R0_CA_CMD1_RK0_TX_ARCLKB_DLY                   Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R0_CA_CMD2                       (DDRPHY_BASE_ADDR + 0x00EA8)
    #define SHU_R0_CA_CMD2_RG_RK0_RX_ARCA0_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R0_CA_CMD2_RG_RK0_RX_ARCA0_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R0_CA_CMD2_RG_RK0_RX_ARCA1_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R0_CA_CMD2_RG_RK0_RX_ARCA1_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD3                       (DDRPHY_BASE_ADDR + 0x00EAC)
    #define SHU_R0_CA_CMD3_RG_RK0_RX_ARCA2_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R0_CA_CMD3_RG_RK0_RX_ARCA2_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R0_CA_CMD3_RG_RK0_RX_ARCA3_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R0_CA_CMD3_RG_RK0_RX_ARCA3_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD4                       (DDRPHY_BASE_ADDR + 0x00EB0)
    #define SHU_R0_CA_CMD4_RG_RK0_RX_ARCA4_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R0_CA_CMD4_RG_RK0_RX_ARCA4_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R0_CA_CMD4_RG_RK0_RX_ARCA5_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R0_CA_CMD4_RG_RK0_RX_ARCA5_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD5                       (DDRPHY_BASE_ADDR + 0x00EB4)
    #define SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE0_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE0_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE1_R_DLY              Fld(6, 16) //[21:16]
    #define SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE1_F_DLY              Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD6                       (DDRPHY_BASE_ADDR + 0x00EB8)
    #define SHU_R0_CA_CMD6_RG_RK0_RX_ARCKE2_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R0_CA_CMD6_RG_RK0_RX_ARCKE2_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R0_CA_CMD6_RG_RK0_RX_ARCS0_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R0_CA_CMD6_RG_RK0_RX_ARCS0_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD7                       (DDRPHY_BASE_ADDR + 0x00EBC)
    #define SHU_R0_CA_CMD7_RG_RK0_RX_ARCS1_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R0_CA_CMD7_RG_RK0_RX_ARCS1_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R0_CA_CMD7_RG_RK0_RX_ARCS2_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R0_CA_CMD7_RG_RK0_RX_ARCS2_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD8                       (DDRPHY_BASE_ADDR + 0x00EC0)
    #define SHU_R0_CA_CMD8_RG_RK0_RX_ARCLK_R_DLY               Fld(7, 16) //[22:16]
    #define SHU_R0_CA_CMD8_RG_RK0_RX_ARCLK_F_DLY               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R0_CA_CMD9                       (DDRPHY_BASE_ADDR + 0x00EC4)
    #define SHU_R0_CA_CMD9_RG_RK0_ARPI_CS                      Fld(6, 0) //[5:0]
    #define SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD                     Fld(6, 8) //[13:8]
    #define SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK                     Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_CA_CMD10                      (DDRPHY_BASE_ADDR + 0x00EC8)
    #define SHU_R0_CA_CMD10_RK0_TX_ARCA6_DLY                   Fld(4, 0) //[3:0]
    #define SHU_R0_CA_CMD10_RK0_TX_ARCA7_DLY                   Fld(4, 4) //[7:4]
    #define SHU_R0_CA_CMD10_RK0_TX_ARCA8_DLY                   Fld(4, 8) //[11:8]
    #define SHU_R0_CA_CMD10_RK0_TX_ARCA9_DLY                   Fld(4, 12) //[15:12]
    #define SHU_R0_CA_CMD10_RK0_TX_ARCA10_DLY                  Fld(4, 16) //[19:16]

#define DDRPHY_RFU_0XECC                            (DDRPHY_BASE_ADDR + 0x00ECC)
    #define RFU_0XECC_RESERVED_0XECC                           Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R1_B0_DQ0                        (DDRPHY_BASE_ADDR + 0x00F00)
    #define SHU_R1_B0_DQ0_RK1_TX_ARDQ0_DLY_B0                  Fld(4, 0) //[3:0]
    #define SHU_R1_B0_DQ0_RK1_TX_ARDQ1_DLY_B0                  Fld(4, 4) //[7:4]
    #define SHU_R1_B0_DQ0_RK1_TX_ARDQ2_DLY_B0                  Fld(4, 8) //[11:8]
    #define SHU_R1_B0_DQ0_RK1_TX_ARDQ3_DLY_B0                  Fld(4, 12) //[15:12]
    #define SHU_R1_B0_DQ0_RK1_TX_ARDQ4_DLY_B0                  Fld(4, 16) //[19:16]
    #define SHU_R1_B0_DQ0_RK1_TX_ARDQ5_DLY_B0                  Fld(4, 20) //[23:20]
    #define SHU_R1_B0_DQ0_RK1_TX_ARDQ6_DLY_B0                  Fld(4, 24) //[27:24]
    #define SHU_R1_B0_DQ0_RK1_TX_ARDQ7_DLY_B0                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R1_B0_DQ1                        (DDRPHY_BASE_ADDR + 0x00F04)
    #define SHU_R1_B0_DQ1_RK1_TX_ARDQM0_DLY_B0                 Fld(4, 0) //[3:0]
    #define SHU_R1_B0_DQ1_RK1_TX_ARDQS0_DLYB_B0                Fld(4, 16) //[19:16]
    #define SHU_R1_B0_DQ1_RK1_TX_ARDQS0B_DLYB_B0               Fld(4, 20) //[23:20]
    #define SHU_R1_B0_DQ1_RK1_TX_ARDQS0_DLY_B0                 Fld(4, 24) //[27:24]
    #define SHU_R1_B0_DQ1_RK1_TX_ARDQS0B_DLY_B0                Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R1_B0_DQ2                        (DDRPHY_BASE_ADDR + 0x00F08)
    #define SHU_R1_B0_DQ2_RK1_RX_ARDQ0_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R1_B0_DQ2_RK1_RX_ARDQ0_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R1_B0_DQ2_RK1_RX_ARDQ1_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R1_B0_DQ2_RK1_RX_ARDQ1_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B0_DQ3                        (DDRPHY_BASE_ADDR + 0x00F0C)
    #define SHU_R1_B0_DQ3_RK1_RX_ARDQ2_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R1_B0_DQ3_RK1_RX_ARDQ2_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R1_B0_DQ3_RK1_RX_ARDQ3_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R1_B0_DQ3_RK1_RX_ARDQ3_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B0_DQ4                        (DDRPHY_BASE_ADDR + 0x00F10)
    #define SHU_R1_B0_DQ4_RK1_RX_ARDQ4_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R1_B0_DQ4_RK1_RX_ARDQ4_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R1_B0_DQ4_RK1_RX_ARDQ5_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R1_B0_DQ4_RK1_RX_ARDQ5_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B0_DQ5                        (DDRPHY_BASE_ADDR + 0x00F14)
    #define SHU_R1_B0_DQ5_RK1_RX_ARDQ6_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R1_B0_DQ5_RK1_RX_ARDQ6_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R1_B0_DQ5_RK1_RX_ARDQ7_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R1_B0_DQ5_RK1_RX_ARDQ7_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B0_DQ6                        (DDRPHY_BASE_ADDR + 0x00F18)
    #define SHU_R1_B0_DQ6_RK1_RX_ARDQM0_R_DLY_B0               Fld(6, 0) //[5:0]
    #define SHU_R1_B0_DQ6_RK1_RX_ARDQM0_F_DLY_B0               Fld(6, 8) //[13:8]
    #define SHU_R1_B0_DQ6_RK1_RX_ARDQS0_R_DLY_B0               Fld(7, 16) //[22:16]
    #define SHU_R1_B0_DQ6_RK1_RX_ARDQS0_F_DLY_B0               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R1_B0_DQ7                        (DDRPHY_BASE_ADDR + 0x00F1C)
    #define SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0                       Fld(6, 8) //[13:8]
    #define SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0                      Fld(6, 16) //[21:16]
    #define SHU_R1_B0_DQ7_RK1_ARPI_PBYTE_B0                    Fld(6, 24) //[29:24]

#define DDRPHY_RFU_0XF20                            (DDRPHY_BASE_ADDR + 0x00F20)
    #define RFU_0XF20_RESERVED_0XF20                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XF24                            (DDRPHY_BASE_ADDR + 0x00F24)
    #define RFU_0XF24_RESERVED_0XF24                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XF28                            (DDRPHY_BASE_ADDR + 0x00F28)
    #define RFU_0XF28_RESERVED_0XF28                           Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0XF2C                            (DDRPHY_BASE_ADDR + 0x00F2C)
    #define RFU_0XF2C_RESERVED_0XF2C                           Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R1_B1_DQ0                        (DDRPHY_BASE_ADDR + 0x00F50)
    #define SHU_R1_B1_DQ0_RK1_TX_ARDQ0_DLY_B1                  Fld(4, 0) //[3:0]
    #define SHU_R1_B1_DQ0_RK1_TX_ARDQ1_DLY_B1                  Fld(4, 4) //[7:4]
    #define SHU_R1_B1_DQ0_RK1_TX_ARDQ2_DLY_B1                  Fld(4, 8) //[11:8]
    #define SHU_R1_B1_DQ0_RK1_TX_ARDQ3_DLY_B1                  Fld(4, 12) //[15:12]
    #define SHU_R1_B1_DQ0_RK1_TX_ARDQ4_DLY_B1                  Fld(4, 16) //[19:16]
    #define SHU_R1_B1_DQ0_RK1_TX_ARDQ5_DLY_B1                  Fld(4, 20) //[23:20]
    #define SHU_R1_B1_DQ0_RK1_TX_ARDQ6_DLY_B1                  Fld(4, 24) //[27:24]
    #define SHU_R1_B1_DQ0_RK1_TX_ARDQ7_DLY_B1                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R1_B1_DQ1                        (DDRPHY_BASE_ADDR + 0x00F54)
    #define SHU_R1_B1_DQ1_RK1_TX_ARDQM0_DLY_B1                 Fld(4, 0) //[3:0]
    #define SHU_R1_B1_DQ1_RK1_TX_ARDQS0_DLYB_B1                Fld(4, 16) //[19:16]
    #define SHU_R1_B1_DQ1_RK1_TX_ARDQS0B_DLYB_B1               Fld(4, 20) //[23:20]
    #define SHU_R1_B1_DQ1_RK1_TX_ARDQS0_DLY_B1                 Fld(4, 24) //[27:24]
    #define SHU_R1_B1_DQ1_RK1_TX_ARDQS0B_DLY_B1                Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R1_B1_DQ2                        (DDRPHY_BASE_ADDR + 0x00F58)
    #define SHU_R1_B1_DQ2_RK1_RX_ARDQ0_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R1_B1_DQ2_RK1_RX_ARDQ0_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R1_B1_DQ2_RK1_RX_ARDQ1_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R1_B1_DQ2_RK1_RX_ARDQ1_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B1_DQ3                        (DDRPHY_BASE_ADDR + 0x00F5C)
    #define SHU_R1_B1_DQ3_RK1_RX_ARDQ2_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R1_B1_DQ3_RK1_RX_ARDQ2_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R1_B1_DQ3_RK1_RX_ARDQ3_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R1_B1_DQ3_RK1_RX_ARDQ3_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B1_DQ4                        (DDRPHY_BASE_ADDR + 0x00F60)
    #define SHU_R1_B1_DQ4_RK1_RX_ARDQ4_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R1_B1_DQ4_RK1_RX_ARDQ4_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R1_B1_DQ4_RK1_RX_ARDQ5_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R1_B1_DQ4_RK1_RX_ARDQ5_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B1_DQ5                        (DDRPHY_BASE_ADDR + 0x00F64)
    #define SHU_R1_B1_DQ5_RK1_RX_ARDQ6_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R1_B1_DQ5_RK1_RX_ARDQ6_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R1_B1_DQ5_RK1_RX_ARDQ7_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R1_B1_DQ5_RK1_RX_ARDQ7_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B1_DQ6                        (DDRPHY_BASE_ADDR + 0x00F68)
    #define SHU_R1_B1_DQ6_RK1_RX_ARDQM0_R_DLY_B1               Fld(6, 0) //[5:0]
    #define SHU_R1_B1_DQ6_RK1_RX_ARDQM0_F_DLY_B1               Fld(6, 8) //[13:8]
    #define SHU_R1_B1_DQ6_RK1_RX_ARDQS0_R_DLY_B1               Fld(7, 16) //[22:16]
    #define SHU_R1_B1_DQ6_RK1_RX_ARDQS0_F_DLY_B1               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R1_B1_DQ7                        (DDRPHY_BASE_ADDR + 0x00F6C)
    #define SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1                       Fld(6, 8) //[13:8]
    #define SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1                      Fld(6, 16) //[21:16]
    #define SHU_R1_B1_DQ7_RK1_ARPI_PBYTE_B1                    Fld(6, 24) //[29:24]

#define DDRPHY_RFU_0XF70                            (DDRPHY_BASE_ADDR + 0x00F70)
    #define RFU_0XF70_RESERVED_0XF70                           Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R1_CA_CMD11                      (DDRPHY_BASE_ADDR + 0x00F74)
    #define SHU_R1_CA_CMD11_RG_RK1_RX_ARCA7_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R1_CA_CMD11_RG_RK1_RX_ARCA7_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R1_CA_CMD11_RG_RK1_RX_ARCA6_R_DLY              Fld(6, 16) //[21:16]
    #define SHU_R1_CA_CMD11_RG_RK1_RX_ARCA6_F_DLY              Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD12                      (DDRPHY_BASE_ADDR + 0x00F78)
    #define SHU_R1_CA_CMD12_RG_RK1_RX_ARCA9_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R1_CA_CMD12_RG_RK1_RX_ARCA9_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R1_CA_CMD12_RG_RK1_RX_ARCA8_R_DLY              Fld(6, 16) //[21:16]
    #define SHU_R1_CA_CMD12_RG_RK1_RX_ARCA8_F_DLY              Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD13                      (DDRPHY_BASE_ADDR + 0x00F7C)
    #define SHU_R1_CA_CMD13_RG_RK1_RX_ARCA10_R_DLY             Fld(6, 16) //[21:16]
    #define SHU_R1_CA_CMD13_RG_RK1_RX_ARCA10_F_DLY             Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD0                       (DDRPHY_BASE_ADDR + 0x00FA0)
    #define SHU_R1_CA_CMD0_RK1_TX_ARCA0_DLY                    Fld(4, 0) //[3:0]
    #define SHU_R1_CA_CMD0_RK1_TX_ARCA1_DLY                    Fld(4, 4) //[7:4]
    #define SHU_R1_CA_CMD0_RK1_TX_ARCA2_DLY                    Fld(4, 8) //[11:8]
    #define SHU_R1_CA_CMD0_RK1_TX_ARCA3_DLY                    Fld(4, 12) //[15:12]
    #define SHU_R1_CA_CMD0_RK1_TX_ARCA4_DLY                    Fld(4, 16) //[19:16]
    #define SHU_R1_CA_CMD0_RK1_TX_ARCA5_DLY                    Fld(4, 20) //[23:20]
    #define SHU_R1_CA_CMD0_RK1_TX_ARCLK_DLYB                   Fld(4, 24) //[27:24]
    #define SHU_R1_CA_CMD0_RK1_TX_ARCLKB_DLYB                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R1_CA_CMD1                       (DDRPHY_BASE_ADDR + 0x00FA4)
    #define SHU_R1_CA_CMD1_RK1_TX_ARCKE0_DLY                   Fld(4, 0) //[3:0]
    #define SHU_R1_CA_CMD1_RK1_TX_ARCKE1_DLY                   Fld(4, 4) //[7:4]
    #define SHU_R1_CA_CMD1_RK1_TX_ARCKE2_DLY                   Fld(4, 8) //[11:8]
    #define SHU_R1_CA_CMD1_RK1_TX_ARCS0_DLY                    Fld(4, 12) //[15:12]
    #define SHU_R1_CA_CMD1_RK1_TX_ARCS1_DLY                    Fld(4, 16) //[19:16]
    #define SHU_R1_CA_CMD1_RK1_TX_ARCS2_DLY                    Fld(4, 20) //[23:20]
    #define SHU_R1_CA_CMD1_RK1_TX_ARCLK_DLY                    Fld(4, 24) //[27:24]
    #define SHU_R1_CA_CMD1_RK1_TX_ARCLKB_DLY                   Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R1_CA_CMD2                       (DDRPHY_BASE_ADDR + 0x00FA8)
    #define SHU_R1_CA_CMD2_RG_RK1_RX_ARCA0_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R1_CA_CMD2_RG_RK1_RX_ARCA0_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R1_CA_CMD2_RG_RK1_RX_ARCA1_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R1_CA_CMD2_RG_RK1_RX_ARCA1_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD3                       (DDRPHY_BASE_ADDR + 0x00FAC)
    #define SHU_R1_CA_CMD3_RG_RK1_RX_ARCA2_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R1_CA_CMD3_RG_RK1_RX_ARCA2_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R1_CA_CMD3_RG_RK1_RX_ARCA3_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R1_CA_CMD3_RG_RK1_RX_ARCA3_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD4                       (DDRPHY_BASE_ADDR + 0x00FB0)
    #define SHU_R1_CA_CMD4_RG_RK1_RX_ARCA4_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R1_CA_CMD4_RG_RK1_RX_ARCA4_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R1_CA_CMD4_RG_RK1_RX_ARCA5_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R1_CA_CMD4_RG_RK1_RX_ARCA5_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD5                       (DDRPHY_BASE_ADDR + 0x00FB4)
    #define SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE0_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE0_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE1_R_DLY              Fld(6, 16) //[21:16]
    #define SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE1_F_DLY              Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD6                       (DDRPHY_BASE_ADDR + 0x00FB8)
    #define SHU_R1_CA_CMD6_RG_RK1_RX_ARCKE2_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R1_CA_CMD6_RG_RK1_RX_ARCKE2_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R1_CA_CMD6_RG_RK1_RX_ARCS0_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R1_CA_CMD6_RG_RK1_RX_ARCS0_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD7                       (DDRPHY_BASE_ADDR + 0x00FBC)
    #define SHU_R1_CA_CMD7_RG_RK1_RX_ARCS1_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R1_CA_CMD7_RG_RK1_RX_ARCS1_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R1_CA_CMD7_RG_RK1_RX_ARCS2_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R1_CA_CMD7_RG_RK1_RX_ARCS2_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD8                       (DDRPHY_BASE_ADDR + 0x00FC0)
    #define SHU_R1_CA_CMD8_RG_RK1_RX_ARCLK_R_DLY               Fld(7, 16) //[22:16]
    #define SHU_R1_CA_CMD8_RG_RK1_RX_ARCLK_F_DLY               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R1_CA_CMD9                       (DDRPHY_BASE_ADDR + 0x00FC4)
    #define SHU_R1_CA_CMD9_RG_RK1_ARPI_CS                      Fld(6, 0) //[5:0]
    #define SHU_R1_CA_CMD9_RG_RK1_ARPI_CMD                     Fld(6, 8) //[13:8]
    #define SHU_R1_CA_CMD9_RG_RK1_ARPI_CLK                     Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_CA_CMD10                      (DDRPHY_BASE_ADDR + 0x00FC8)
    #define SHU_R1_CA_CMD10_RK1_TX_ARCA6_DLY                   Fld(4, 0) //[3:0]
    #define SHU_R1_CA_CMD10_RK1_TX_ARCA7_DLY                   Fld(4, 4) //[7:4]
    #define SHU_R1_CA_CMD10_RK1_TX_ARCA8_DLY                   Fld(4, 8) //[11:8]
    #define SHU_R1_CA_CMD10_RK1_TX_ARCA9_DLY                   Fld(4, 12) //[15:12]
    #define SHU_R1_CA_CMD10_RK1_TX_ARCA10_DLY                  Fld(4, 16) //[19:16]

#define DDRPHY_RFU_0XFCC                            (DDRPHY_BASE_ADDR + 0x00FCC)
    #define RFU_0XFCC_RESERVED_0XFCC                           Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R2_B0_DQ0                        (DDRPHY_BASE_ADDR + 0x01000)
    #define SHU_R2_B0_DQ0_RK2_TX_ARDQ0_DLY_B0                  Fld(4, 0) //[3:0]
    #define SHU_R2_B0_DQ0_RK2_TX_ARDQ1_DLY_B0                  Fld(4, 4) //[7:4]
    #define SHU_R2_B0_DQ0_RK2_TX_ARDQ2_DLY_B0                  Fld(4, 8) //[11:8]
    #define SHU_R2_B0_DQ0_RK2_TX_ARDQ3_DLY_B0                  Fld(4, 12) //[15:12]
    #define SHU_R2_B0_DQ0_RK2_TX_ARDQ4_DLY_B0                  Fld(4, 16) //[19:16]
    #define SHU_R2_B0_DQ0_RK2_TX_ARDQ5_DLY_B0                  Fld(4, 20) //[23:20]
    #define SHU_R2_B0_DQ0_RK2_TX_ARDQ6_DLY_B0                  Fld(4, 24) //[27:24]
    #define SHU_R2_B0_DQ0_RK2_TX_ARDQ7_DLY_B0                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R2_B0_DQ1                        (DDRPHY_BASE_ADDR + 0x01004)
    #define SHU_R2_B0_DQ1_RK2_TX_ARDQM0_DLY_B0                 Fld(4, 0) //[3:0]
    #define SHU_R2_B0_DQ1_RK2_TX_ARDQS0_DLYB_B0                Fld(4, 16) //[19:16]
    #define SHU_R2_B0_DQ1_RK2_TX_ARDQS0B_DLYB_B0               Fld(4, 20) //[23:20]
    #define SHU_R2_B0_DQ1_RK2_TX_ARDQS0_DLY_B0                 Fld(4, 24) //[27:24]
    #define SHU_R2_B0_DQ1_RK2_TX_ARDQS0B_DLY_B0                Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R2_B0_DQ2                        (DDRPHY_BASE_ADDR + 0x01008)
    #define SHU_R2_B0_DQ2_RK2_RX_ARDQ0_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R2_B0_DQ2_RK2_RX_ARDQ0_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R2_B0_DQ2_RK2_RX_ARDQ1_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R2_B0_DQ2_RK2_RX_ARDQ1_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_B0_DQ3                        (DDRPHY_BASE_ADDR + 0x0100C)
    #define SHU_R2_B0_DQ3_RK2_RX_ARDQ2_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R2_B0_DQ3_RK2_RX_ARDQ2_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R2_B0_DQ3_RK2_RX_ARDQ3_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R2_B0_DQ3_RK2_RX_ARDQ3_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_B0_DQ4                        (DDRPHY_BASE_ADDR + 0x01010)
    #define SHU_R2_B0_DQ4_RK2_RX_ARDQ4_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R2_B0_DQ4_RK2_RX_ARDQ4_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R2_B0_DQ4_RK2_RX_ARDQ5_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R2_B0_DQ4_RK2_RX_ARDQ5_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_B0_DQ5                        (DDRPHY_BASE_ADDR + 0x01014)
    #define SHU_R2_B0_DQ5_RK2_RX_ARDQ6_R_DLY_B0                Fld(6, 0) //[5:0]
    #define SHU_R2_B0_DQ5_RK2_RX_ARDQ6_F_DLY_B0                Fld(6, 8) //[13:8]
    #define SHU_R2_B0_DQ5_RK2_RX_ARDQ7_R_DLY_B0                Fld(6, 16) //[21:16]
    #define SHU_R2_B0_DQ5_RK2_RX_ARDQ7_F_DLY_B0                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_B0_DQ6                        (DDRPHY_BASE_ADDR + 0x01018)
    #define SHU_R2_B0_DQ6_RK2_RX_ARDQM0_R_DLY_B0               Fld(6, 0) //[5:0]
    #define SHU_R2_B0_DQ6_RK2_RX_ARDQM0_F_DLY_B0               Fld(6, 8) //[13:8]
    #define SHU_R2_B0_DQ6_RK2_RX_ARDQS0_R_DLY_B0               Fld(7, 16) //[22:16]
    #define SHU_R2_B0_DQ6_RK2_RX_ARDQS0_F_DLY_B0               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R2_B0_DQ7                        (DDRPHY_BASE_ADDR + 0x0101C)
    #define SHU_R2_B0_DQ7_RK2_ARPI_DQ_B0                       Fld(6, 8) //[13:8]
    #define SHU_R2_B0_DQ7_RK2_ARPI_DQM_B0                      Fld(6, 16) //[21:16]
    #define SHU_R2_B0_DQ7_RK2_ARPI_PBYTE_B0                    Fld(6, 24) //[29:24]

#define DDRPHY_RFU_0X1020                           (DDRPHY_BASE_ADDR + 0x01020)
    #define RFU_0X1020_RESERVED_0X1020                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X1024                           (DDRPHY_BASE_ADDR + 0x01024)
    #define RFU_0X1024_RESERVED_0X1024                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X1028                           (DDRPHY_BASE_ADDR + 0x01028)
    #define RFU_0X1028_RESERVED_0X1028                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X102C                           (DDRPHY_BASE_ADDR + 0x0102C)
    #define RFU_0X102C_RESERVED_0X102C                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R2_B1_DQ0                        (DDRPHY_BASE_ADDR + 0x01050)
    #define SHU_R2_B1_DQ0_RK2_TX_ARDQ0_DLY_B1                  Fld(4, 0) //[3:0]
    #define SHU_R2_B1_DQ0_RK2_TX_ARDQ1_DLY_B1                  Fld(4, 4) //[7:4]
    #define SHU_R2_B1_DQ0_RK2_TX_ARDQ2_DLY_B1                  Fld(4, 8) //[11:8]
    #define SHU_R2_B1_DQ0_RK2_TX_ARDQ3_DLY_B1                  Fld(4, 12) //[15:12]
    #define SHU_R2_B1_DQ0_RK2_TX_ARDQ4_DLY_B1                  Fld(4, 16) //[19:16]
    #define SHU_R2_B1_DQ0_RK2_TX_ARDQ5_DLY_B1                  Fld(4, 20) //[23:20]
    #define SHU_R2_B1_DQ0_RK2_TX_ARDQ6_DLY_B1                  Fld(4, 24) //[27:24]
    #define SHU_R2_B1_DQ0_RK2_TX_ARDQ7_DLY_B1                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R2_B1_DQ1                        (DDRPHY_BASE_ADDR + 0x01054)
    #define SHU_R2_B1_DQ1_RK2_TX_ARDQM0_DLY_B1                 Fld(4, 0) //[3:0]
    #define SHU_R2_B1_DQ1_RK2_TX_ARDQS0_DLY_B1                 Fld(4, 24) //[27:24]
    #define SHU_R2_B1_DQ1_RK2_TX_ARDQS0B_DLY_B1                Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R2_B1_DQ2                        (DDRPHY_BASE_ADDR + 0x01058)
    #define SHU_R2_B1_DQ2_RK2_RX_ARDQ0_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R2_B1_DQ2_RK2_RX_ARDQ0_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R2_B1_DQ2_RK2_RX_ARDQ1_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R2_B1_DQ2_RK2_RX_ARDQ1_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_B1_DQ3                        (DDRPHY_BASE_ADDR + 0x0105C)
    #define SHU_R2_B1_DQ3_RK2_RX_ARDQ2_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R2_B1_DQ3_RK2_RX_ARDQ2_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R2_B1_DQ3_RK2_RX_ARDQ3_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R2_B1_DQ3_RK2_RX_ARDQ3_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_B1_DQ4                        (DDRPHY_BASE_ADDR + 0x01060)
    #define SHU_R2_B1_DQ4_RK2_RX_ARDQ4_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R2_B1_DQ4_RK2_RX_ARDQ4_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R2_B1_DQ4_RK2_RX_ARDQ5_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R2_B1_DQ4_RK2_RX_ARDQ5_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_B1_DQ5                        (DDRPHY_BASE_ADDR + 0x01064)
    #define SHU_R2_B1_DQ5_RK2_RX_ARDQ6_R_DLY_B1                Fld(6, 0) //[5:0]
    #define SHU_R2_B1_DQ5_RK2_RX_ARDQ6_F_DLY_B1                Fld(6, 8) //[13:8]
    #define SHU_R2_B1_DQ5_RK2_RX_ARDQ7_R_DLY_B1                Fld(6, 16) //[21:16]
    #define SHU_R2_B1_DQ5_RK2_RX_ARDQ7_F_DLY_B1                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_B1_DQ6                        (DDRPHY_BASE_ADDR + 0x01068)
    #define SHU_R2_B1_DQ6_RK2_RX_ARDQM0_R_DLY_B1               Fld(6, 0) //[5:0]
    #define SHU_R2_B1_DQ6_RK2_RX_ARDQM0_F_DLY_B1               Fld(6, 8) //[13:8]
    #define SHU_R2_B1_DQ6_RK2_RX_ARDQS0_R_DLY_B1               Fld(7, 16) //[22:16]
    #define SHU_R2_B1_DQ6_RK2_RX_ARDQS0_F_DLY_B1               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R2_B1_DQ7                        (DDRPHY_BASE_ADDR + 0x0106C)
    #define SHU_R2_B1_DQ7_RK2_ARPI_DQ_B1                       Fld(6, 8) //[13:8]
    #define SHU_R2_B1_DQ7_RK2_ARPI_DQM_B1                      Fld(6, 16) //[21:16]
    #define SHU_R2_B1_DQ7_RK2_ARPI_PBYTE_B1                    Fld(6, 24) //[29:24]

#define DDRPHY_RFU_0X1070                           (DDRPHY_BASE_ADDR + 0x01070)
    #define RFU_0X1070_RESERVED_0X1070                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X1074                           (DDRPHY_BASE_ADDR + 0x01074)
    #define RFU_0X1074_RESERVED_0X1074                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X1078                           (DDRPHY_BASE_ADDR + 0x01078)
    #define RFU_0X1078_RESERVED_0X1078                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X107C                           (DDRPHY_BASE_ADDR + 0x0107C)
    #define RFU_0X107C_RESERVED_0X107C                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R2_CA_CMD0                       (DDRPHY_BASE_ADDR + 0x010A0)
    #define SHU_R2_CA_CMD0_RK2_TX_ARCA0_DLY                    Fld(4, 0) //[3:0]
    #define SHU_R2_CA_CMD0_RK2_TX_ARCA1_DLY                    Fld(4, 4) //[7:4]
    #define SHU_R2_CA_CMD0_RK2_TX_ARCA2_DLY                    Fld(4, 8) //[11:8]
    #define SHU_R2_CA_CMD0_RK2_TX_ARCA3_DLY                    Fld(4, 12) //[15:12]
    #define SHU_R2_CA_CMD0_RK2_TX_ARCA4_DLY                    Fld(4, 16) //[19:16]
    #define SHU_R2_CA_CMD0_RK2_TX_ARCA5_DLY                    Fld(4, 20) //[23:20]
    #define SHU_R2_CA_CMD0_RK2_TX_ARCLK_DLYB                   Fld(4, 24) //[27:24]
    #define SHU_R2_CA_CMD0_RK2_TX_ARCLKB_DLYB                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R2_CA_CMD1                       (DDRPHY_BASE_ADDR + 0x010A4)
    #define SHU_R2_CA_CMD1_RK2_TX_ARCKE0_DLY                   Fld(4, 0) //[3:0]
    #define SHU_R2_CA_CMD1_RK2_TX_ARCKE1_DLY                   Fld(4, 4) //[7:4]
    #define SHU_R2_CA_CMD1_RK2_TX_ARCKE2_DLY                   Fld(4, 8) //[11:8]
    #define SHU_R2_CA_CMD1_RK2_TX_ARCS0_DLY                    Fld(4, 12) //[15:12]
    #define SHU_R2_CA_CMD1_RK2_TX_ARCS1_DLY                    Fld(4, 16) //[19:16]
    #define SHU_R2_CA_CMD1_RK2_TX_ARCS2_DLY                    Fld(4, 20) //[23:20]
    #define SHU_R2_CA_CMD1_RK2_TX_ARCLK_DLY                    Fld(4, 24) //[27:24]
    #define SHU_R2_CA_CMD1_RK2_TX_ARCLKB_DLY                   Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R2_CA_CMD2                       (DDRPHY_BASE_ADDR + 0x010A8)
    #define SHU_R2_CA_CMD2_RG_RK2_RX_ARCA0_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R2_CA_CMD2_RG_RK2_RX_ARCA0_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R2_CA_CMD2_RG_RK2_RX_ARCA1_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R2_CA_CMD2_RG_RK2_RX_ARCA1_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_CA_CMD3                       (DDRPHY_BASE_ADDR + 0x010AC)
    #define SHU_R2_CA_CMD3_RG_RK2_RX_ARCA2_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R2_CA_CMD3_RG_RK2_RX_ARCA2_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R2_CA_CMD3_RG_RK2_RX_ARCA3_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R2_CA_CMD3_RG_RK2_RX_ARCA3_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_CA_CMD4                       (DDRPHY_BASE_ADDR + 0x010B0)
    #define SHU_R2_CA_CMD4_RG_RK2_RX_ARCA4_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R2_CA_CMD4_RG_RK2_RX_ARCA4_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R2_CA_CMD4_RG_RK2_RX_ARCA5_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R2_CA_CMD4_RG_RK2_RX_ARCA5_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_CA_CMD5                       (DDRPHY_BASE_ADDR + 0x010B4)
    #define SHU_R2_CA_CMD5_RG_RK2_RX_ARCKE0_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R2_CA_CMD5_RG_RK2_RX_ARCKE0_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R2_CA_CMD5_RG_RK2_RX_ARCKE1_R_DLY              Fld(6, 16) //[21:16]
    #define SHU_R2_CA_CMD5_RG_RK2_RX_ARCKE1_F_DLY              Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_CA_CMD6                       (DDRPHY_BASE_ADDR + 0x010B8)
    #define SHU_R2_CA_CMD6_RG_RK2_RX_ARCKE2_R_DLY              Fld(6, 0) //[5:0]
    #define SHU_R2_CA_CMD6_RG_RK2_RX_ARCKE2_F_DLY              Fld(6, 8) //[13:8]
    #define SHU_R2_CA_CMD6_RG_RK2_RX_ARCS0_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R2_CA_CMD6_RG_RK2_RX_ARCS0_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_CA_CMD7                       (DDRPHY_BASE_ADDR + 0x010BC)
    #define SHU_R2_CA_CMD7_RG_RK2_RX_ARCS1_R_DLY               Fld(6, 0) //[5:0]
    #define SHU_R2_CA_CMD7_RG_RK2_RX_ARCS1_F_DLY               Fld(6, 8) //[13:8]
    #define SHU_R2_CA_CMD7_RG_RK2_RX_ARCS2_R_DLY               Fld(6, 16) //[21:16]
    #define SHU_R2_CA_CMD7_RG_RK2_RX_ARCS2_F_DLY               Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_CA_CMD8                       (DDRPHY_BASE_ADDR + 0x010C0)
    #define SHU_R2_CA_CMD8_RG_RK2_RX_ARCLK_R_DLY               Fld(7, 16) //[22:16]
    #define SHU_R2_CA_CMD8_RG_RK2_RX_ARCLK_F_DLY               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R2_CA_CMD9                       (DDRPHY_BASE_ADDR + 0x010C4)
    #define SHU_R2_CA_CMD9_RG_RK2_ARPI_CS                      Fld(6, 0) //[5:0]
    #define SHU_R2_CA_CMD9_RG_RK2_ARPI_CMD                     Fld(6, 8) //[13:8]
    #define SHU_R2_CA_CMD9_RG_RK2_ARPI_CLK                     Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R2_CA_CMD10                      (DDRPHY_BASE_ADDR + 0x010C8)
    #define SHU_R2_CA_CMD10_RK2_TX_ARCA6_DLY                   Fld(4, 0) //[3:0]
    #define SHU_R2_CA_CMD10_RK2_TX_ARCA7_DLY                   Fld(4, 4) //[7:4]
    #define SHU_R2_CA_CMD10_RK2_TX_ARCA8_DLY                   Fld(4, 8) //[11:8]
    #define SHU_R2_CA_CMD10_RK2_TX_ARCA9_DLY                   Fld(4, 12) //[15:12]
    #define SHU_R2_CA_CMD10_RK2_TX_ARCA10_DLY                  Fld(4, 16) //[19:16]

#define DDRPHY_B2_DLL_ARPI0                         (DDRPHY_BASE_ADDR + 0x02080)
    #define B2_DLL_ARPI0_RG_ARPI_ISO_EN_B2                     Fld(1, 0) //[0:0]
    #define B2_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_B2                Fld(1, 1) //[1:1]
    #define B2_DLL_ARPI0_RG_ARPI_RESETB_B2                     Fld(1, 3) //[3:3]
    #define B2_DLL_ARPI0_RG_ARPI_LS_EN_B2                      Fld(1, 4) //[4:4]
    #define B2_DLL_ARPI0_RG_ARPI_LS_SEL_B2                     Fld(1, 5) //[5:5]
    #define B2_DLL_ARPI0_RG_ARPI_MCK8X_SEL_B2                  Fld(1, 6) //[6:6]

#define DDRPHY_B2_DLL_ARPI1                         (DDRPHY_BASE_ADDR + 0x02084)
    #define B2_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B2             Fld(1, 11) //[11:11]
    #define B2_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B2                 Fld(1, 13) //[13:13]
    #define B2_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B2                Fld(1, 14) //[14:14]
    #define B2_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B2                Fld(1, 15) //[15:15]
    #define B2_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B2                 Fld(1, 17) //[17:17]
    #define B2_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B2               Fld(1, 19) //[19:19]
    #define B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2_REG_OPT          Fld(1, 20) //[20:20]
    #define B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2                  Fld(1, 21) //[21:21]
    #define B2_DLL_ARPI1_RG_ARPI_SET_UPDN_B2                   Fld(3, 28) //[30:28]

#define DDRPHY_B2_DLL_ARPI2                         (DDRPHY_BASE_ADDR + 0x02088)
    #define B2_DLL_ARPI2_RG_ARDLL_PHDET_EN_B2                  Fld(1, 0) //[0:0]
    #define B2_DLL_ARPI2_RG_ARPI_MPDIV_CG_B2                   Fld(1, 10) //[10:10]
    #define B2_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B2                  Fld(1, 11) //[11:11]
    #define B2_DLL_ARPI2_RG_ARPI_CG_DQ_B2                      Fld(1, 13) //[13:13]
    #define B2_DLL_ARPI2_RG_ARPI_CG_DQM_B2                     Fld(1, 14) //[14:14]
    #define B2_DLL_ARPI2_RG_ARPI_CG_DQS_B2                     Fld(1, 15) //[15:15]
    #define B2_DLL_ARPI2_RG_ARPI_CG_FB_B2                      Fld(1, 17) //[17:17]
    #define B2_DLL_ARPI2_RG_ARPI_CG_MCTL_B2                    Fld(1, 19) //[19:19]
    #define B2_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B2              Fld(1, 27) //[27:27]
    #define B2_DLL_ARPI2_RG_ARPI_CG_MCK_B2                     Fld(1, 31) //[31:31]

#define DDRPHY_B2_DLL_ARPI3                         (DDRPHY_BASE_ADDR + 0x0208C)
    #define B2_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B2                  Fld(1, 11) //[11:11]
    #define B2_DLL_ARPI3_RG_ARPI_DQ_EN_B2                      Fld(1, 13) //[13:13]
    #define B2_DLL_ARPI3_RG_ARPI_DQM_EN_B2                     Fld(1, 14) //[14:14]
    #define B2_DLL_ARPI3_RG_ARPI_DQS_EN_B2                     Fld(1, 15) //[15:15]
    #define B2_DLL_ARPI3_RG_ARPI_FB_EN_B2                      Fld(1, 17) //[17:17]
    #define B2_DLL_ARPI3_RG_ARPI_MCTL_EN_B2                    Fld(1, 19) //[19:19]

#define DDRPHY_B2_DLL_ARPI4                         (DDRPHY_BASE_ADDR + 0x02090)
    #define B2_DLL_ARPI4_RG_ARPI_BYPASS_DQSIEN_B2              Fld(1, 11) //[11:11]
    #define B2_DLL_ARPI4_RG_ARPI_BYPASS_DQ_B2                  Fld(1, 13) //[13:13]
    #define B2_DLL_ARPI4_RG_ARPI_BYPASS_DQM_B2                 Fld(1, 14) //[14:14]
    #define B2_DLL_ARPI4_RG_ARPI_BYPASS_DQS_B2                 Fld(1, 15) //[15:15]
    #define B2_DLL_ARPI4_RG_ARPI_BYPASS_FB_B2                  Fld(1, 17) //[17:17]
    #define B2_DLL_ARPI4_RG_ARPI_BYPASS_MCTL_B2                Fld(1, 19) //[19:19]

#define DDRPHY_B2_DLL_ARPI5                         (DDRPHY_BASE_ADDR + 0x02094)
    #define B2_DLL_ARPI5_RG_ARDLL_MON_SEL_B2                   Fld(4, 4) //[7:4]
    #define B2_DLL_ARPI5_RG_ARDLL_DIV_DEC_B2                   Fld(1, 8) //[8:8]
    #define B2_DLL_ARPI5_B2_DLL_ARPI5_RFU                      Fld(12, 12) //[23:12]
    #define B2_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_SEL_B2           Fld(1, 25) //[25:25]
    #define B2_DLL_ARPI5_RG_ARDLL_FJ_OUT_MODE_B2               Fld(1, 26) //[26:26]
    #define B2_DLL_ARPI5_B2_DLL_ARPI5_RFU1                     Fld(1, 31) //[31:31]

#define DDRPHY_B2_DQ0                               (DDRPHY_BASE_ADDR + 0x02098)
    #define B2_DQ0_RG_RX_ARDQ0_OFFC_B2                         Fld(4, 0) //[3:0]
    #define B2_DQ0_RG_RX_ARDQ1_OFFC_B2                         Fld(4, 4) //[7:4]
    #define B2_DQ0_RG_RX_ARDQ2_OFFC_B2                         Fld(4, 8) //[11:8]
    #define B2_DQ0_RG_RX_ARDQ3_OFFC_B2                         Fld(4, 12) //[15:12]
    #define B2_DQ0_RG_RX_ARDQ4_OFFC_B2                         Fld(4, 16) //[19:16]
    #define B2_DQ0_RG_RX_ARDQ5_OFFC_B2                         Fld(4, 20) //[23:20]
    #define B2_DQ0_RG_RX_ARDQ6_OFFC_B2                         Fld(4, 24) //[27:24]
    #define B2_DQ0_RG_RX_ARDQ7_OFFC_B2                         Fld(4, 28) //[31:28]

#define DDRPHY_B2_DQ1                               (DDRPHY_BASE_ADDR + 0x0209C)
    #define B2_DQ1_RG_RX_ARDQM0_OFFC_B2                        Fld(4, 0) //[3:0]

#define DDRPHY_B2_DQ2                               (DDRPHY_BASE_ADDR + 0x020A0)
    #define B2_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B2                   Fld(1, 16) //[16:16]
    #define B2_DQ2_RG_TX_ARDQS0_OE_DIS_B2                      Fld(1, 17) //[17:17]
    #define B2_DQ2_RG_TX_ARDQM0_ODTEN_DIS_B2                   Fld(1, 18) //[18:18]
    #define B2_DQ2_RG_TX_ARDQM0_OE_DIS_B2                      Fld(1, 19) //[19:19]
    #define B2_DQ2_RG_TX_ARDQ_ODTEN_DIS_B2                     Fld(1, 20) //[20:20]
    #define B2_DQ2_RG_TX_ARDQ_OE_DIS_B2                        Fld(1, 21) //[21:21]

#define DDRPHY_B2_DQ3                               (DDRPHY_BASE_ADDR + 0x020A4)
    #define B2_DQ3_RG_ARDQ_ATPG_EN_B2                          Fld(1, 0) //[0:0]
    #define B2_DQ3_RG_RX_ARDQ_SMT_EN_B2                        Fld(1, 1) //[1:1]
    #define B2_DQ3_RG_TX_ARDQ_EN_B2                            Fld(1, 2) //[2:2]
    #define B2_DQ3_RG_ARDQ_RESETB_B2                           Fld(1, 3) //[3:3]
    #define B2_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B2                  Fld(1, 5) //[5:5]
    #define B2_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B2                  Fld(1, 6) //[6:6]
    #define B2_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B2                    Fld(1, 7) //[7:7]
    #define B2_DQ3_RG_RX_ARDQ_STBENCMP_EN_B2                   Fld(1, 10) //[10:10]
    #define B2_DQ3_RG_RX_ARDQ_OFFC_EN_B2                       Fld(1, 11) //[11:11]
    #define B2_DQ3_RG_RX_ARDQS0_SWAP_EN_B2                     Fld(1, 15) //[15:15]

#define DDRPHY_B2_DQ4                               (DDRPHY_BASE_ADDR + 0x020A8)
    #define B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2                    Fld(7, 0) //[6:0]
    #define B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2                    Fld(7, 8) //[14:8]
    #define B2_DQ4_RG_RX_ARDQ_EYE_R_DLY_B2                     Fld(6, 16) //[21:16]
    #define B2_DQ4_RG_RX_ARDQ_EYE_F_DLY_B2                     Fld(6, 24) //[29:24]

#define DDRPHY_B2_DQ5                               (DDRPHY_BASE_ADDR + 0x020AC)
    #define B2_DQ5_B2_DQ5_RFU                                  Fld(8, 0) //[7:0]
    #define B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2                  Fld(6, 8) //[13:8]
    #define B2_DQ5_RG_RX_ARDQ_VREF_EN_B2                       Fld(1, 16) //[16:16]
    #define B2_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B2                   Fld(1, 17) //[17:17]
    #define B2_DQ5_RG_RX_ARDQ_EYE_SEL_B2                       Fld(4, 20) //[23:20]
    #define B2_DQ5_RG_RX_ARDQ_EYE_EN_B2                        Fld(1, 24) //[24:24]
    #define B2_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B2              Fld(1, 25) //[25:25]
    #define B2_DQ5_RG_RX_ARDQS0_DVS_EN_B2                      Fld(1, 31) //[31:31]

#define DDRPHY_B2_DQ6                               (DDRPHY_BASE_ADDR + 0x020B0)
    #define B2_DQ6_RG_RX_ARDQ_BIAS_PS_B2                       Fld(2, 0) //[1:0]
    #define B2_DQ6_RG_TX_ARDQ_OE_EXT_DIS_B2                    Fld(1, 2) //[2:2]
    #define B2_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B2                 Fld(1, 3) //[3:3]
    #define B2_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B2                   Fld(1, 5) //[5:5]
    #define B2_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B2                   Fld(1, 6) //[6:6]
    #define B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2                 Fld(1, 7) //[7:7]
    #define B2_DQ6_RG_RX_ARDQ_LPBK_EN_B2                       Fld(1, 8) //[8:8]
    #define B2_DQ6_RG_RX_ARDQ_O1_SEL_B2                        Fld(1, 9) //[9:9]
    #define B2_DQ6_RG_RX_ARDQ_JM_SEL_B2                        Fld(1, 11) //[11:11]
    #define B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2                       Fld(1, 12) //[12:12]
    #define B2_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B2                 Fld(2, 14) //[15:14]
    #define B2_DQ6_RG_RX_ARDQ_DDR4_SEL_B2                      Fld(1, 16) //[16:16]
    #define B2_DQ6_RG_TX_ARDQ_DDR4_SEL_B2                      Fld(1, 17) //[17:17]
    #define B2_DQ6_RG_RX_ARDQ_DDR3_SEL_B2                      Fld(1, 18) //[18:18]
    #define B2_DQ6_RG_TX_ARDQ_DDR3_SEL_B2                      Fld(1, 19) //[19:19]
    #define B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2            Fld(1, 24) //[24:24]
    #define B2_DQ6_RG_RX_ARDQ_EYE_OE_GATE_EN_B2                Fld(1, 28) //[28:28]
    #define B2_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B2                 Fld(1, 31) //[31:31]

#define DDRPHY_B2_DQ7                               (DDRPHY_BASE_ADDR + 0x020B4)
    #define B2_DQ7_RG_TX_ARDQS0B_PULL_DN_B2                    Fld(1, 0) //[0:0]
    #define B2_DQ7_RG_TX_ARDQS0B_PULL_UP_B2                    Fld(1, 1) //[1:1]
    #define B2_DQ7_RG_TX_ARDQS0_PULL_DN_B2                     Fld(1, 2) //[2:2]
    #define B2_DQ7_RG_TX_ARDQS0_PULL_UP_B2                     Fld(1, 3) //[3:3]
    #define B2_DQ7_RG_TX_ARDQM0_PULL_DN_B2                     Fld(1, 4) //[4:4]
    #define B2_DQ7_RG_TX_ARDQM0_PULL_UP_B2                     Fld(1, 5) //[5:5]
    #define B2_DQ7_RG_TX_ARDQ_PULL_DN_B2                       Fld(1, 6) //[6:6]
    #define B2_DQ7_RG_TX_ARDQ_PULL_UP_B2                       Fld(1, 7) //[7:7]
    #define B2_DQ7_RG_TX_ARDQS0B_PULL_DN_B2_LP4Y               Fld(1, 16) //[16:16]

#define DDRPHY_B2_DQ8                               (DDRPHY_BASE_ADDR + 0x020B8)
    #define B2_DQ8_RG_TX_ARDQ_EN_LP4P_B2                       Fld(1, 0) //[0:0]
    #define B2_DQ8_RG_TX_ARDQ_EN_CAP_LP4P_B2                   Fld(1, 1) //[1:1]
    #define B2_DQ8_RG_TX_ARDQ_CAP_DET_B2                       Fld(1, 2) //[2:2]
    #define B2_DQ8_RG_TX_ARDQ_CKE_MCK4X_SEL_B2                 Fld(2, 3) //[4:3]
    #define B2_DQ8_RG_ARPI_TX_CG_DQ_EN_B2                      Fld(1, 5) //[5:5]
    #define B2_DQ8_RG_ARPI_TX_CG_DQM_EN_B2                     Fld(1, 6) //[6:6]
    #define B2_DQ8_RG_ARPI_TX_CG_DQS_EN_B2                     Fld(1, 7) //[7:7]
    #define B2_DQ8_RG_RX_ARDQS_BURST_E1_EN_B2                  Fld(1, 8) //[8:8]
    #define B2_DQ8_RG_RX_ARDQS_BURST_E2_EN_B2                  Fld(1, 9) //[9:9]
    #define B2_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B2                 Fld(1, 10) //[10:10]
    #define B2_DQ8_RG_RX_ARDQS_GATE_EN_MODE_B2                 Fld(1, 12) //[12:12]
    #define B2_DQ8_RG_RX_ARDQS_SER_RST_MODE_B2                 Fld(1, 13) //[13:13]
    #define B2_DQ8_RG_ARDLL_RESETB_B2                          Fld(1, 15) //[15:15]

#define DDRPHY_B2_DQ9                               (DDRPHY_BASE_ADDR + 0x020BC)
    #define B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2                  Fld(1, 0) //[0:0]
    #define B2_DQ9_RG_DQOE_TIEH_B2                             Fld(1, 1) //[1:1]
    #define B2_DQ9_RG_DQMOE_TIEH_B2                            Fld(1, 2) //[2:2]
    #define B2_DQ9_RG_DQSOE_TIEH_B2                            Fld(1, 3) //[3:3]
    #define B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2                Fld(1, 4) //[4:4]
    #define B2_DQ9_RG_RX_ARDQS0_DQSIENMODE_B2                  Fld(1, 5) //[5:5]
    #define B2_DQ9_R_DMARPIDQ_SW_B2                            Fld(1, 6) //[6:6]
    #define B2_DQ9_R_DMRXFIFO_STBENCMP_EN_B2                   Fld(1, 7) //[7:7]
    #define B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2                     Fld(8, 8) //[15:8]
    #define B2_DQ9_R_DMDQSIEN_VALID_LAT_B2                     Fld(3, 16) //[18:16]
    #define B2_DQ9_R_DMDQSIEN_RDSEL_LAT_B2                     Fld(3, 20) //[22:20]
    #define B2_DQ9_R_DMRXDVS_VALID_LAT_B2                      Fld(3, 24) //[26:24]
    #define B2_DQ9_R_DMRXDVS_RDSEL_LAT_B2                      Fld(3, 28) //[30:28]

#define DDRPHY_B2_DQ10                              (DDRPHY_BASE_ADDR + 0x020C0)
    #define B2_DQ10_RG_RX_ARDQ0_DQS_SEL_B2                     Fld(1, 0) //[0:0]
    #define B2_DQ10_RG_RX_ARDQ1_DQS_SEL_B2                     Fld(1, 1) //[1:1]
    #define B2_DQ10_RG_RX_ARDQ2_DQS_SEL_B2                     Fld(1, 2) //[2:2]
    #define B2_DQ10_RG_RX_ARDQ3_DQS_SEL_B2                     Fld(1, 3) //[3:3]
    #define B2_DQ10_RG_RX_ARDQ4_DQS_SEL_B2                     Fld(1, 4) //[4:4]
    #define B2_DQ10_RG_RX_ARDQ5_DQS_SEL_B2                     Fld(1, 5) //[5:5]
    #define B2_DQ10_RG_RX_ARDQ6_DQS_SEL_B2                     Fld(1, 6) //[6:6]
    #define B2_DQ10_RG_RX_ARDQ7_DQS_SEL_B2                     Fld(1, 7) //[7:7]
    #define B2_DQ10_RG_RX_ARDQM_DQS_SEL_B2                     Fld(1, 8) //[8:8]
    #define B2_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B2                   Fld(1, 16) //[16:16]
    #define B2_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B2                   Fld(1, 17) //[17:17]
    #define B2_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B2                   Fld(1, 18) //[18:18]
    #define B2_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B2                   Fld(1, 19) //[19:19]
    #define B2_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B2                   Fld(1, 20) //[20:20]
    #define B2_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B2                   Fld(1, 21) //[21:21]
    #define B2_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B2                   Fld(1, 22) //[22:22]
    #define B2_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B2                   Fld(1, 23) //[23:23]
    #define B2_DQ10_RG_TX_ARDQM_MCK4X_SEL_B2                   Fld(1, 24) //[24:24]
    #define B2_DQ10_RG_TX_ARDQS0_MCK4X_SEL_B2                  Fld(1, 25) //[25:25]
    #define B2_DQ10_RG_TX_ARDQS0B_MCK4X_SEL_B2                 Fld(1, 26) //[26:26]

#define DDRPHY_RFU_0X20C4                           (DDRPHY_BASE_ADDR + 0x020C4)
    #define RFU_0X20C4_RESERVED_0X20C4                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X20C8                           (DDRPHY_BASE_ADDR + 0x020C8)
    #define RFU_0X20C8_RESERVED_0X20C8                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X20CC                           (DDRPHY_BASE_ADDR + 0x020CC)
    #define RFU_0X20CC_RESERVED_0X20CC                         Fld(32, 0) //[31:0]

#define DDRPHY_B2_TX_MCK                            (DDRPHY_BASE_ADDR + 0x020D0)
    #define B2_TX_MCK_R_DM_TX_MCK_FRUN_B2                      Fld(10, 0) //[9:0]

#define DDRPHY_RFU_0X20D4                           (DDRPHY_BASE_ADDR + 0x020D4)
    #define RFU_0X20D4_RESERVED_0X20D4                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X20D8                           (DDRPHY_BASE_ADDR + 0x020D8)
    #define RFU_0X20D8_RESERVED_0X20D8                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X20DC                           (DDRPHY_BASE_ADDR + 0x020DC)
    #define RFU_0X20DC_RESERVED_0X20DC                         Fld(32, 0) //[31:0]

#define DDRPHY_B2_RXDVS0                            (DDRPHY_BASE_ADDR + 0x025F0)
    #define B2_RXDVS0_R_RX_RANKINSEL_B2                        Fld(1, 0) //[0:0]
    #define B2_RXDVS0_B2_RXDVS0_RFU                            Fld(3, 1) //[3:1]
    #define B2_RXDVS0_R_RX_RANKINCTL_B2                        Fld(4, 4) //[7:4]
    #define B2_RXDVS0_R_DVS_SW_UP_B2                           Fld(1, 8) //[8:8]
    #define B2_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B2                Fld(1, 9) //[9:9]
    #define B2_RXDVS0_R_DMRXDVS_PBYTESTUCK_RST_B2              Fld(1, 10) //[10:10]
    #define B2_RXDVS0_R_DMRXDVS_PBYTESTUCK_IG_B2               Fld(1, 11) //[11:11]
    #define B2_RXDVS0_R_DMRXDVS_DQIENPOST_OPT_B2               Fld(2, 12) //[13:12]
    #define B2_RXDVS0_R_RX_DLY_RANK_ERR_ST_CLR_B2              Fld(3, 16) //[18:16]
    #define B2_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B2                  Fld(1, 19) //[19:19]
    #define B2_RXDVS0_R_RX_DLY_RK_OPT_B2                       Fld(2, 20) //[21:20]
    #define B2_RXDVS0_R_HWRESTORE_ENA_B2                       Fld(1, 22) //[22:22]
    #define B2_RXDVS0_R_HWSAVE_MODE_ENA_B2                     Fld(1, 24) //[24:24]
    #define B2_RXDVS0_R_RX_DLY_DVS_MODE_SYNC_DIS_B2            Fld(1, 26) //[26:26]
    #define B2_RXDVS0_R_RX_DLY_TRACK_BYPASS_MODESYNC_B2        Fld(1, 27) //[27:27]
    #define B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2                  Fld(1, 28) //[28:28]
    #define B2_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B2               Fld(1, 29) //[29:29]
    #define B2_RXDVS0_R_RX_DLY_TRACK_CLR_B2                    Fld(1, 30) //[30:30]
    #define B2_RXDVS0_R_RX_DLY_TRACK_ENA_B2                    Fld(1, 31) //[31:31]

#define DDRPHY_B2_RXDVS1                            (DDRPHY_BASE_ADDR + 0x025F4)
    #define B2_RXDVS1_B2_RXDVS1_RFU                            Fld(16, 0) //[15:0]
    #define B2_RXDVS1_R_DMRXDVS_UPD_CLR_ACK_B2                 Fld(1, 16) //[16:16]
    #define B2_RXDVS1_R_DMRXDVS_UPD_CLR_NORD_B2                Fld(1, 17) //[17:17]

#define DDRPHY_RFU_0X25F8                           (DDRPHY_BASE_ADDR + 0x025F8)
    #define RFU_0X25F8_RESERVED_0X25F8                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X25FC                           (DDRPHY_BASE_ADDR + 0x025FC)
    #define RFU_0X25FC_RESERVED_0X25FC                         Fld(32, 0) //[31:0]

#define DDRPHY_R0_B2_RXDVS0                         (DDRPHY_BASE_ADDR + 0x02600)
    #define R0_B2_RXDVS0_R_RK0_B2_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R0_B2_RXDVS0_R_RK0_B2_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R0_B2_RXDVS0_R_RK0_B2_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R0_B2_RXDVS1                         (DDRPHY_BASE_ADDR + 0x02604)
    #define R0_B2_RXDVS1_R_RK0_B2_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R0_B2_RXDVS1_R_RK0_B2_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R0_B2_RXDVS2                         (DDRPHY_BASE_ADDR + 0x02608)
    #define R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_B2         Fld(2, 16) //[17:16]
    #define R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_B2          Fld(2, 18) //[19:18]
    #define R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2    Fld(1, 23) //[23:23]
    #define R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_B2         Fld(2, 24) //[25:24]
    #define R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_B2          Fld(2, 26) //[27:26]
    #define R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2    Fld(1, 28) //[28:28]
    #define R0_B2_RXDVS2_R_RK0_DVS_FDLY_MODE_B2                Fld(1, 29) //[29:29]
    #define R0_B2_RXDVS2_R_RK0_DVS_MODE_B2                     Fld(2, 30) //[31:30]

#define DDRPHY_R0_B2_RXDVS7                         (DDRPHY_BASE_ADDR + 0x0261C)
    #define R0_B2_RXDVS7_RG_RK0_ARDQ_MIN_DLY_B2                Fld(6, 0) //[5:0]
    #define R0_B2_RXDVS7_RG_RK0_ARDQ_MIN_DLY_B2_RFU            Fld(2, 6) //[7:6]
    #define R0_B2_RXDVS7_RG_RK0_ARDQ_MAX_DLY_B2                Fld(6, 8) //[13:8]
    #define R0_B2_RXDVS7_RG_RK0_ARDQ_MAX_DLY_B2_RFU            Fld(2, 14) //[15:14]
    #define R0_B2_RXDVS7_RG_RK0_ARDQS0_MIN_DLY_B2              Fld(7, 16) //[22:16]
    #define R0_B2_RXDVS7_RG_RK0_ARDQS0_MIN_DLY_B2_RFU          Fld(1, 23) //[23:23]
    #define R0_B2_RXDVS7_RG_RK0_ARDQS0_MAX_DLY_B2              Fld(7, 24) //[30:24]
    #define R0_B2_RXDVS7_RG_RK0_ARDQS0_MAX_DLY_B2_RFU          Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X2620                           (DDRPHY_BASE_ADDR + 0x02620)
    #define RFU_0X2620_RESERVED_0X2620                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X2624                           (DDRPHY_BASE_ADDR + 0x02624)
    #define RFU_0X2624_RESERVED_0X2624                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X2628                           (DDRPHY_BASE_ADDR + 0x02628)
    #define RFU_0X2628_RESERVED_0X2628                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X262C                           (DDRPHY_BASE_ADDR + 0x0262C)
    #define RFU_0X262C_RESERVED_0X262C                         Fld(32, 0) //[31:0]

#define DDRPHY_R1_B2_RXDVS0                         (DDRPHY_BASE_ADDR + 0x02800)
    #define R1_B2_RXDVS0_R_RK1_B2_DVS_LEAD_LAG_CNT_CLR         Fld(1, 26) //[26:26]
    #define R1_B2_RXDVS0_R_RK1_B2_DVS_SW_CNT_CLR               Fld(1, 27) //[27:27]
    #define R1_B2_RXDVS0_R_RK1_B2_DVS_SW_CNT_ENA               Fld(1, 31) //[31:31]

#define DDRPHY_R1_B2_RXDVS1                         (DDRPHY_BASE_ADDR + 0x02804)
    #define R1_B2_RXDVS1_R_RK1_B2_DVS_TH_LAG                   Fld(16, 0) //[15:0]
    #define R1_B2_RXDVS1_R_RK1_B2_DVS_TH_LEAD                  Fld(16, 16) //[31:16]

#define DDRPHY_R1_B2_RXDVS2                         (DDRPHY_BASE_ADDR + 0x02808)
    #define R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_DQS_SCALE_B2         Fld(2, 16) //[17:16]
    #define R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_DQ_SCALE_B2          Fld(2, 18) //[19:18]
    #define R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B2    Fld(1, 23) //[23:23]
    #define R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_DQS_SCALE_B2         Fld(2, 24) //[25:24]
    #define R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_DQ_SCALE_B2          Fld(2, 26) //[27:26]
    #define R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B2    Fld(1, 28) //[28:28]
    #define R1_B2_RXDVS2_R_RK1_DVS_FDLY_MODE_B2                Fld(1, 29) //[29:29]
    #define R1_B2_RXDVS2_R_RK1_DVS_MODE_B2                     Fld(2, 30) //[31:30]

#define DDRPHY_R1_B2_RXDVS7                         (DDRPHY_BASE_ADDR + 0x0281C)
    #define R1_B2_RXDVS7_RG_RK1_ARDQ_MIN_DLY_B2                Fld(6, 0) //[5:0]
    #define R1_B2_RXDVS7_RG_RK1_ARDQ_MIN_DLY_B2_RFU            Fld(2, 6) //[7:6]
    #define R1_B2_RXDVS7_RG_RK1_ARDQ_MAX_DLY_B2                Fld(6, 8) //[13:8]
    #define R1_B2_RXDVS7_RG_RK1_ARDQ_MAX_DLY_B2_RFU            Fld(2, 14) //[15:14]
    #define R1_B2_RXDVS7_RG_RK1_ARDQS0_MIN_DLY_B2              Fld(7, 16) //[22:16]
    #define R1_B2_RXDVS7_RG_RK1_ARDQS0_MIN_DLY_B2_RFU          Fld(1, 23) //[23:23]
    #define R1_B2_RXDVS7_RG_RK1_ARDQS0_MAX_DLY_B2              Fld(7, 24) //[30:24]
    #define R1_B2_RXDVS7_RG_RK1_ARDQS0_MAX_DLY_B2_RFU          Fld(1, 31) //[31:31]

#define DDRPHY_RFU_0X2820                           (DDRPHY_BASE_ADDR + 0x02820)
    #define RFU_0X2820_RESERVED_0X2820                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X2824                           (DDRPHY_BASE_ADDR + 0x02824)
    #define RFU_0X2824_RESERVED_0X2824                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X2828                           (DDRPHY_BASE_ADDR + 0x02828)
    #define RFU_0X2828_RESERVED_0X2828                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X282C                           (DDRPHY_BASE_ADDR + 0x0282C)
    #define RFU_0X282C_RESERVED_0X282C                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B2_DQ0                           (DDRPHY_BASE_ADDR + 0x02C00)
    #define SHU_B2_DQ0_RG_TX_ARDQS0_PRE_EN_B2                  Fld(1, 4) //[4:4]
    #define SHU_B2_DQ0_RG_TX_ARDQS0_DRVP_PRE_B2                Fld(3, 8) //[10:8]
    #define SHU_B2_DQ0_RG_TX_ARDQS0_DRVP_PRE_B2_BIT0           Fld(1,8) //[8:8]//Francis added
    #define SHU_B2_DQ0_RG_TX_ARDQS0_DRVP_PRE_B2_BIT1           Fld(1,9) //[9:9]//Francis added
    #define SHU_B2_DQ0_RG_TX_ARDQS0_DRVP_PRE_B2_BIT2           Fld(1,10) //[10:10] //Francis added
    #define SHU_B2_DQ0_RG_TX_ARDQS0_DRVN_PRE_B2                Fld(3, 12) //[14:12]
    #define SHU_B2_DQ0_RG_TX_ARDQ_PRE_EN_B2                    Fld(1, 20) //[20:20]
    #define SHU_B2_DQ0_RG_TX_ARDQ_DRVP_PRE_B2                  Fld(3, 24) //[26:24]
    #define SHU_B2_DQ0_RG_TX_ARDQ_DRVN_PRE_B2                  Fld(3, 28) //[30:28]
    #define SHU_B2_DQ0_R_LP4Y_WDN_MODE_DQS0                    Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B2_DQ1                           (DDRPHY_BASE_ADDR + 0x02C04)
    #define SHU_B2_DQ1_RG_TX_ARDQ_DRVP_B2                      Fld(5, 0) //[4:0]
    #define SHU_B2_DQ1_RG_TX_ARDQ_DRVN_B2                      Fld(5, 8) //[12:8]
    #define SHU_B2_DQ1_RG_TX_ARDQ_ODTP_B2                      Fld(5, 16) //[20:16]
    #define SHU_B2_DQ1_RG_TX_ARDQ_ODTN_B2                      Fld(5, 24) //[28:24]

#define DDRPHY_SHU_B2_DQ2                           (DDRPHY_BASE_ADDR + 0x02C08)
    #define SHU_B2_DQ2_RG_TX_ARDQS0_DRVP_B2                    Fld(5, 0) //[4:0]
    #define SHU_B2_DQ2_RG_TX_ARDQS0_DRVN_B2                    Fld(5, 8) //[12:8]
    #define SHU_B2_DQ2_RG_TX_ARDQS0_ODTP_B2                    Fld(5, 16) //[20:16]
    #define SHU_B2_DQ2_RG_TX_ARDQS0_ODTN_B2                    Fld(5, 24) //[28:24]

#define DDRPHY_SHU_B2_DQ3                           (DDRPHY_BASE_ADDR + 0x02C0C)
    #define SHU_B2_DQ3_RG_TX_ARDQS0_PU_B2                      Fld(2, 0) //[1:0]
    #define SHU_B2_DQ3_RG_TX_ARDQS0_PU_PRE_B2                  Fld(2, 2) //[3:2]
    #define SHU_B2_DQ3_RG_TX_ARDQS0_PDB_B2                     Fld(2, 4) //[5:4]
    #define SHU_B2_DQ3_RG_TX_ARDQS0_PDB_PRE_B2                 Fld(2, 6) //[7:6]
    #define SHU_B2_DQ3_RG_TX_ARDQ_PU_B2                        Fld(2, 8) //[9:8]
    #define SHU_B2_DQ3_RG_TX_ARDQ_PU_PRE_B2                    Fld(2, 10) //[11:10]
    #define SHU_B2_DQ3_RG_TX_ARDQ_PDB_B2                       Fld(2, 12) //[13:12]
    #define SHU_B2_DQ3_RG_TX_ARDQ_PDB_PRE_B2                   Fld(2, 14) //[15:14]

#define DDRPHY_SHU_B2_DQ4                           (DDRPHY_BASE_ADDR + 0x02C10)
    #define SHU_B2_DQ4_RG_ARPI_AA_MCK_DL_B2                    Fld(6, 0) //[5:0]
    #define SHU_B2_DQ4_RG_ARPI_AA_MCK_FB_DL_B2                 Fld(6, 8) //[13:8]
    #define SHU_B2_DQ4_RG_ARPI_DA_MCK_FB_DL_B2                 Fld(6, 16) //[21:16]

#define DDRPHY_SHU_B2_DQ5                           (DDRPHY_BASE_ADDR + 0x02C14)
    #define SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2                  Fld(6, 0) //[5:0]
    #define SHU_B2_DQ5_RG_RX_ARDQ_VREF_BYPASS_B2               Fld(1, 6) //[6:6]
    #define SHU_B2_DQ5_RG_ARPI_FB_B2                           Fld(6, 8) //[13:8]
    #define SHU_B2_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B2              Fld(3, 16) //[18:16]
    #define SHU_B2_DQ5_DA_RX_ARDQS_DQSIEN_RB_DLY_B2            Fld(1, 19) //[19:19]
    #define SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2                 Fld(3, 20) //[22:20]
    #define SHU_B2_DQ5_RG_ARPI_MCTL_B2                         Fld(6, 24) //[29:24]

#define DDRPHY_SHU_B2_DQ6                           (DDRPHY_BASE_ADDR + 0x02C18)
    #define SHU_B2_DQ6_RG_ARPI_OFFSET_DQSIEN_B2                Fld(6, 0) //[5:0]
    #define SHU_B2_DQ6_RG_ARPI_RESERVE_B2                      Fld(16, 6) //[21:6]
    #define SHU_B2_DQ6_RG_ARPI_MIDPI_CAP_SEL_B2                Fld(2, 22) //[23:22]
    #define SHU_B2_DQ6_RG_ARPI_MIDPI_VTH_SEL_B2                Fld(2, 24) //[25:24]
    #define SHU_B2_DQ6_RG_ARPI_MIDPI_EN_B2                     Fld(1, 26) //[26:26]
    #define SHU_B2_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B2              Fld(1, 27) //[27:27]
    #define SHU_B2_DQ6_RG_ARPI_CAP_SEL_B2                      Fld(2, 28) //[29:28]
    #define SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2                  Fld(1, 30) //[30:30]
    #define SHU_B2_DQ6_RG_ARPI_MIDPI_BYPASS_EN_B2              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B2_DQ7                           (DDRPHY_BASE_ADDR + 0x02C1C)
    #define SHU_B2_DQ7_R_DMRANKRXDVS_B2                        Fld(4, 0) //[3:0]
    #define SHU_B2_DQ7_MIDPI_ENABLE                            Fld(1, 4) //[4:4]
    #define SHU_B2_DQ7_MIDPI_DIV4_ENABLE                       Fld(1, 5) //[5:5]
    #define SHU_B2_DQ7_R_DMDQMDBI_EYE_SHU_B2                   Fld(1, 6) //[6:6]
    #define SHU_B2_DQ7_R_DMDQMDBI_SHU_B2                       Fld(1, 7) //[7:7]
    #define SHU_B2_DQ7_R_DMRXDVS_DQM_FLAGSEL_B2                Fld(4, 8) //[11:8]
    #define SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2             Fld(1, 12) //[12:12]
    #define SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2               Fld(1, 13) //[13:13]
    #define SHU_B2_DQ7_R_DMRXTRACK_DQM_EN_B2                   Fld(1, 14) //[14:14]
    #define SHU_B2_DQ7_R_DMRODTEN_B2                           Fld(1, 15) //[15:15]
    #define SHU_B2_DQ7_R_DMARPI_CG_FB2DLL_DCM_EN_B2            Fld(1, 16) //[16:16]
    #define SHU_B2_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B2                Fld(1, 17) //[17:17]
    #define SHU_B2_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B2               Fld(1, 18) //[18:18]
    #define SHU_B2_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B2               Fld(1, 19) //[19:19]
    #define SHU_B2_DQ7_R_LP4Y_SDN_MODE_DQS0                    Fld(1, 20) //[20:20]
    #define SHU_B2_DQ7_R_DMRXRANK_DQ_EN_B2                     Fld(1, 24) //[24:24]
    #define SHU_B2_DQ7_R_DMRXRANK_DQ_LAT_B2                    Fld(3, 25) //[27:25]
    #define SHU_B2_DQ7_R_DMRXRANK_DQS_EN_B2                    Fld(1, 28) //[28:28]
    #define SHU_B2_DQ7_R_DMRXRANK_DQS_LAT_B2                   Fld(3, 29) //[31:29]

#define DDRPHY_SHU_B2_DQ8                           (DDRPHY_BASE_ADDR + 0x02C20)
    #define SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B2              Fld(15, 0) //[14:0]
    #define SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_EN_B2               Fld(1, 15) //[15:15]
    #define SHU_B2_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B2            Fld(1, 19) //[19:19]
    #define SHU_B2_DQ8_R_RMRODTEN_CG_IG_B2                     Fld(1, 20) //[20:20]
    #define SHU_B2_DQ8_R_RMRX_TOPHY_CG_IG_B2                   Fld(1, 21) //[21:21]
    #define SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2           Fld(1, 22) //[22:22]
    #define SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2       Fld(1, 23) //[23:23]
    #define SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2                      Fld(1, 24) //[24:24]
    #define SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2                 Fld(1, 25) //[25:25]
    #define SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2           Fld(1, 26) //[26:26]
    #define SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2           Fld(1, 27) //[27:27]
    #define SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2          Fld(1, 28) //[28:28]
    #define SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2      Fld(1, 29) //[29:29]
    #define SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2                  Fld(1, 30) //[30:30]
    #define SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B2_DQ9                           (DDRPHY_BASE_ADDR + 0x02C24)
    #define SHU_B2_DQ9_RESERVED_0X2C24                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B2_DQ10                          (DDRPHY_BASE_ADDR + 0x02C28)
    #define SHU_B2_DQ10_RESERVED_0X2C28                        Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B2_DQ11                          (DDRPHY_BASE_ADDR + 0x02C2C)
    #define SHU_B2_DQ11_RESERVED_0X2C2C                        Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B2_DQ12                          (DDRPHY_BASE_ADDR + 0x02C30)
    #define SHU_B2_DQ12_RESERVED_0X2C30                        Fld(32, 0) //[31:0]

#define DDRPHY_SHU_B2_DLL0                          (DDRPHY_BASE_ADDR + 0x02C34)
    #define SHU_B2_DLL0_RG_ARPISM_MCK_SEL_B2_SHU               Fld(1, 0) //[0:0]
    #define SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2                   Fld(2, 1) //[2:1]
    #define SHU_B2_DLL0_B2_DLL0_RFU                            Fld(1, 3) //[3:3]
    #define SHU_B2_DLL0_RG_ARDLL_FAST_PSJP_B2                  Fld(1, 4) //[4:4]
    #define SHU_B2_DLL0_RG_ARDLL_PHDIV_B2                      Fld(1, 9) //[9:9]
    #define SHU_B2_DLL0_RG_ARDLL_PHJUMP_EN_B2                  Fld(1, 10) //[10:10]
    #define SHU_B2_DLL0_RG_ARDLL_P_GAIN_B2                     Fld(4, 12) //[15:12]
    #define SHU_B2_DLL0_RG_ARDLL_IDLECNT_B2                    Fld(4, 16) //[19:16]
    #define SHU_B2_DLL0_RG_ARDLL_GAIN_B2                       Fld(4, 20) //[23:20]
    #define SHU_B2_DLL0_RG_ARDLL_PHDET_IN_SWAP_B2              Fld(1, 30) //[30:30]
    #define SHU_B2_DLL0_RG_ARDLL_PHDET_OUT_SEL_B2              Fld(1, 31) //[31:31]

#define DDRPHY_SHU_B2_DLL1                          (DDRPHY_BASE_ADDR + 0x02C38)
    #define SHU_B2_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B2              Fld(1, 0) //[0:0]
    #define SHU_B2_DLL1_RG_ARDLL_PS_EN_B2                      Fld(1, 1) //[1:1]
    #define SHU_B2_DLL1_RG_ARDLL_PD_CK_SEL_B2                  Fld(1, 2) //[2:2]
    #define SHU_B2_DLL1_RG_ARDQ_REV_B2                         Fld(24, 8) //[31:8]
    #define SHU_B2_DLL1_RG_ARDQ_REV_B2_31                      Fld(1, 31) /* cc add */

#define DDRPHY_SHU_B2_DLL2                          (DDRPHY_BASE_ADDR + 0x02C3C)
    #define SHU_B2_DLL2_RG_ARDQ_REV_MSB_B2                     Fld(8, 0) //[7:0]

#define DDRPHY_SHU_R0_B2_DQ0                        (DDRPHY_BASE_ADDR + 0x02E00)
    #define SHU_R0_B2_DQ0_RK0_TX_ARDQ0_DLY_B2                  Fld(4, 0) //[3:0]
    #define SHU_R0_B2_DQ0_RK0_TX_ARDQ1_DLY_B2                  Fld(4, 4) //[7:4]
    #define SHU_R0_B2_DQ0_RK0_TX_ARDQ2_DLY_B2                  Fld(4, 8) //[11:8]
    #define SHU_R0_B2_DQ0_RK0_TX_ARDQ3_DLY_B2                  Fld(4, 12) //[15:12]
    #define SHU_R0_B2_DQ0_RK0_TX_ARDQ4_DLY_B2                  Fld(4, 16) //[19:16]
    #define SHU_R0_B2_DQ0_RK0_TX_ARDQ5_DLY_B2                  Fld(4, 20) //[23:20]
    #define SHU_R0_B2_DQ0_RK0_TX_ARDQ6_DLY_B2                  Fld(4, 24) //[27:24]
    #define SHU_R0_B2_DQ0_RK0_TX_ARDQ7_DLY_B2                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R0_B2_DQ1                        (DDRPHY_BASE_ADDR + 0x02E04)
    #define SHU_R0_B2_DQ1_RK0_TX_ARDQM0_DLY_B2                 Fld(4, 0) //[3:0]
    #define SHU_R0_B2_DQ1_RK0_TX_ARDQS0_DLYB_B2                Fld(4, 16) //[19:16]
    #define SHU_R0_B2_DQ1_RK0_TX_ARDQS0B_DLYB_B2               Fld(4, 20) //[23:20]
    #define SHU_R0_B2_DQ1_RK0_TX_ARDQS0_DLY_B2                 Fld(4, 24) //[27:24]
    #define SHU_R0_B2_DQ1_RK0_TX_ARDQS0B_DLY_B2                Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R0_B2_DQ2                        (DDRPHY_BASE_ADDR + 0x02E08)
    #define SHU_R0_B2_DQ2_RK0_RX_ARDQ0_R_DLY_B2                Fld(6, 0) //[5:0]
    #define SHU_R0_B2_DQ2_RK0_RX_ARDQ0_F_DLY_B2                Fld(6, 8) //[13:8]
    #define SHU_R0_B2_DQ2_RK0_RX_ARDQ1_R_DLY_B2                Fld(6, 16) //[21:16]
    #define SHU_R0_B2_DQ2_RK0_RX_ARDQ1_F_DLY_B2                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B2_DQ3                        (DDRPHY_BASE_ADDR + 0x02E0C)
    #define SHU_R0_B2_DQ3_RK0_RX_ARDQ2_R_DLY_B2                Fld(6, 0) //[5:0]
    #define SHU_R0_B2_DQ3_RK0_RX_ARDQ2_F_DLY_B2                Fld(6, 8) //[13:8]
    #define SHU_R0_B2_DQ3_RK0_RX_ARDQ3_R_DLY_B2                Fld(6, 16) //[21:16]
    #define SHU_R0_B2_DQ3_RK0_RX_ARDQ3_F_DLY_B2                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B2_DQ4                        (DDRPHY_BASE_ADDR + 0x02E10)
    #define SHU_R0_B2_DQ4_RK0_RX_ARDQ4_R_DLY_B2                Fld(6, 0) //[5:0]
    #define SHU_R0_B2_DQ4_RK0_RX_ARDQ4_F_DLY_B2                Fld(6, 8) //[13:8]
    #define SHU_R0_B2_DQ4_RK0_RX_ARDQ5_R_DLY_B2                Fld(6, 16) //[21:16]
    #define SHU_R0_B2_DQ4_RK0_RX_ARDQ5_F_DLY_B2                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B2_DQ5                        (DDRPHY_BASE_ADDR + 0x02E14)
    #define SHU_R0_B2_DQ5_RK0_RX_ARDQ6_R_DLY_B2                Fld(6, 0) //[5:0]
    #define SHU_R0_B2_DQ5_RK0_RX_ARDQ6_F_DLY_B2                Fld(6, 8) //[13:8]
    #define SHU_R0_B2_DQ5_RK0_RX_ARDQ7_R_DLY_B2                Fld(6, 16) //[21:16]
    #define SHU_R0_B2_DQ5_RK0_RX_ARDQ7_F_DLY_B2                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R0_B2_DQ6                        (DDRPHY_BASE_ADDR + 0x02E18)
    #define SHU_R0_B2_DQ6_RK0_RX_ARDQM0_R_DLY_B2               Fld(6, 0) //[5:0]
    #define SHU_R0_B2_DQ6_RK0_RX_ARDQM0_F_DLY_B2               Fld(6, 8) //[13:8]
    #define SHU_R0_B2_DQ6_RK0_RX_ARDQS0_R_DLY_B2               Fld(7, 16) //[22:16]
    #define SHU_R0_B2_DQ6_RK0_RX_ARDQS0_F_DLY_B2               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R0_B2_DQ7                        (DDRPHY_BASE_ADDR + 0x02E1C)
    #define SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2                       Fld(6, 8) //[13:8]
    #define SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2                      Fld(6, 16) //[21:16]
    #define SHU_R0_B2_DQ7_RK0_ARPI_PBYTE_B2                    Fld(6, 24) //[29:24]

#define DDRPHY_RFU_0X2E20                           (DDRPHY_BASE_ADDR + 0x02E20)
    #define RFU_0X2E20_RESERVED_0X2E20                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X2E24                           (DDRPHY_BASE_ADDR + 0x02E24)
    #define RFU_0X2E24_RESERVED_0X2E24                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X2E28                           (DDRPHY_BASE_ADDR + 0x02E28)
    #define RFU_0X2E28_RESERVED_0X2E28                         Fld(32, 0) //[31:0]

#define DDRPHY_RFU_0X2E2C                           (DDRPHY_BASE_ADDR + 0x02E2C)
    #define RFU_0X2E2C_RESERVED_0X2E2C                         Fld(32, 0) //[31:0]

#define DDRPHY_SHU_R1_B2_DQ0                        (DDRPHY_BASE_ADDR + 0x02F00)
    #define SHU_R1_B2_DQ0_RK1_TX_ARDQ0_DLY_B2                  Fld(4, 0) //[3:0]
    #define SHU_R1_B2_DQ0_RK1_TX_ARDQ1_DLY_B2                  Fld(4, 4) //[7:4]
    #define SHU_R1_B2_DQ0_RK1_TX_ARDQ2_DLY_B2                  Fld(4, 8) //[11:8]
    #define SHU_R1_B2_DQ0_RK1_TX_ARDQ3_DLY_B2                  Fld(4, 12) //[15:12]
    #define SHU_R1_B2_DQ0_RK1_TX_ARDQ4_DLY_B2                  Fld(4, 16) //[19:16]
    #define SHU_R1_B2_DQ0_RK1_TX_ARDQ5_DLY_B2                  Fld(4, 20) //[23:20]
    #define SHU_R1_B2_DQ0_RK1_TX_ARDQ6_DLY_B2                  Fld(4, 24) //[27:24]
    #define SHU_R1_B2_DQ0_RK1_TX_ARDQ7_DLY_B2                  Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R1_B2_DQ1                        (DDRPHY_BASE_ADDR + 0x02F04)
    #define SHU_R1_B2_DQ1_RK1_TX_ARDQM0_DLY_B2                 Fld(4, 0) //[3:0]
    #define SHU_R1_B2_DQ1_RK1_TX_ARDQS0_DLYB_B2                Fld(4, 16) //[19:16]
    #define SHU_R1_B2_DQ1_RK1_TX_ARDQS0B_DLYB_B2               Fld(4, 20) //[23:20]
    #define SHU_R1_B2_DQ1_RK1_TX_ARDQS0_DLY_B2                 Fld(4, 24) //[27:24]
    #define SHU_R1_B2_DQ1_RK1_TX_ARDQS0B_DLY_B2                Fld(4, 28) //[31:28]

#define DDRPHY_SHU_R1_B2_DQ2                        (DDRPHY_BASE_ADDR + 0x02F08)
    #define SHU_R1_B2_DQ2_RK1_RX_ARDQ0_R_DLY_B2                Fld(6, 0) //[5:0]
    #define SHU_R1_B2_DQ2_RK1_RX_ARDQ0_F_DLY_B2                Fld(6, 8) //[13:8]
    #define SHU_R1_B2_DQ2_RK1_RX_ARDQ1_R_DLY_B2                Fld(6, 16) //[21:16]
    #define SHU_R1_B2_DQ2_RK1_RX_ARDQ1_F_DLY_B2                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B2_DQ3                        (DDRPHY_BASE_ADDR + 0x02F0C)
    #define SHU_R1_B2_DQ3_RK1_RX_ARDQ2_R_DLY_B2                Fld(6, 0) //[5:0]
    #define SHU_R1_B2_DQ3_RK1_RX_ARDQ2_F_DLY_B2                Fld(6, 8) //[13:8]
    #define SHU_R1_B2_DQ3_RK1_RX_ARDQ3_R_DLY_B2                Fld(6, 16) //[21:16]
    #define SHU_R1_B2_DQ3_RK1_RX_ARDQ3_F_DLY_B2                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B2_DQ4                        (DDRPHY_BASE_ADDR + 0x02F10)
    #define SHU_R1_B2_DQ4_RK1_RX_ARDQ4_R_DLY_B2                Fld(6, 0) //[5:0]
    #define SHU_R1_B2_DQ4_RK1_RX_ARDQ4_F_DLY_B2                Fld(6, 8) //[13:8]
    #define SHU_R1_B2_DQ4_RK1_RX_ARDQ5_R_DLY_B2                Fld(6, 16) //[21:16]
    #define SHU_R1_B2_DQ4_RK1_RX_ARDQ5_F_DLY_B2                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B2_DQ5                        (DDRPHY_BASE_ADDR + 0x02F14)
    #define SHU_R1_B2_DQ5_RK1_RX_ARDQ6_R_DLY_B2                Fld(6, 0) //[5:0]
    #define SHU_R1_B2_DQ5_RK1_RX_ARDQ6_F_DLY_B2                Fld(6, 8) //[13:8]
    #define SHU_R1_B2_DQ5_RK1_RX_ARDQ7_R_DLY_B2                Fld(6, 16) //[21:16]
    #define SHU_R1_B2_DQ5_RK1_RX_ARDQ7_F_DLY_B2                Fld(6, 24) //[29:24]

#define DDRPHY_SHU_R1_B2_DQ6                        (DDRPHY_BASE_ADDR + 0x02F18)
    #define SHU_R1_B2_DQ6_RK1_RX_ARDQM0_R_DLY_B2               Fld(6, 0) //[5:0]
    #define SHU_R1_B2_DQ6_RK1_RX_ARDQM0_F_DLY_B2               Fld(6, 8) //[13:8]
    #define SHU_R1_B2_DQ6_RK1_RX_ARDQS0_R_DLY_B2               Fld(7, 16) //[22:16]
    #define SHU_R1_B2_DQ6_RK1_RX_ARDQS0_F_DLY_B2               Fld(7, 24) //[30:24]

#define DDRPHY_SHU_R1_B2_DQ7                        (DDRPHY_BASE_ADDR + 0x02F1C)
    #define SHU_R1_B2_DQ7_RK1_ARPI_DQ_B2                       Fld(6, 8) //[13:8]
    #define SHU_R1_B2_DQ7_RK1_ARPI_DQM_B2                      Fld(6, 16) //[21:16]
    #define SHU_R1_B2_DQ7_RK1_ARPI_PBYTE_B2                    Fld(6, 24) //[29:24]


/* User Define, RSV bits. From M, need review */

#define RG_ARDQ_REV_BIT_23_NA				Fld(1, 31)
#define RG_ARDQ_REV_BIT_2221_DATA_SWAP		Fld(2, 29)
#define RG_ARDQ_REV_BIT_20_DATA_SWAP_EN		Fld(1, 28)
#define RG_ARDQ_REV_BIT_19_RX_DQSIEN_FORCE_ON_EN	Fld(1, 27)
#define RG_ARDQ_REV_BIT_18_RX_LP4Y_EN		Fld(1, 26)
#define RG_ARDQ_REV_BIT_1716_TX_LP4Y_SEL	Fld(2, 24)
#define RG_ARDQ_REV_BIT_15_TX_PRE_EN		Fld(1, 23)
#define RG_ARDQ_REV_BIT_14_TX_PRE_DATA_SEL	Fld(1, 22)
#define RG_ARDQ_REV_BIT_1312_DRVP_PRE		Fld(2, 20)
#define RG_ARDQ_REV_BIT_1110_DRVN_PRE		Fld(2, 18)
#define RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN	Fld(1, 17)
#define RG_ARDQ_REV_BIT_08_TX_ODT_DISABLE	Fld(1, 16)
#define RG_ARDQ_REV_BIT_07_MCK4X_SEL_DQ5	Fld(1, 15)
#define RG_ARDQ_REV_BIT_06_MCK4X_SEL_DQ1	Fld(1, 14)
#define RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE 	Fld(1, 13)
#define RG_ARDQ_REV_BIT_04_RX_DQSIEN_RB_DLY 	Fld(1, 12)
#define RG_ARDQ_REV_BIT_03_RX_DQS_GATE_EN_MODE	Fld(1, 11)
#define RG_ARDQ_REV_BIT_02_TX_READ_BASE_EN_DQSB Fld(1, 10)
#define RG_ARDQ_REV_BIT_01_DQS_MCK4XB_DLY_EN 	Fld(1, 9)
#define RG_ARDQ_REV_BIT_00_DQS_MCK4X_DLY_EN 	Fld(1, 8)


//DDRPHY_SHU1_CA_DLL1
#define RG_ARCMD_REV_BIT_23_NA 				Fld(1, 31)
#define RG_ARCMD_REV_BIT_2221_DATA_SWAP 		Fld(2, 29)
#define RG_ARCMD_REV_BIT_20_DATA_SWAP_EN 		Fld(1, 28)
#define RG_ARCMD_REV_BIT_19_RX_DQSIEN_FORCE_ON_EN	Fld(1, 27)
#define RG_ARCMD_REV_BIT_18_RX_LP4Y_EN 			Fld(1, 26)
#define RG_ARCMD_REV_BIT_1716_TX_LP4Y_SEL		Fld(2, 24)
#define RG_ARCMD_REV_BIT_15_TX_DDR4P_CKE_SEL		Fld(1, 23)
#define RG_ARCMD_REV_BIT_14_TX_DDR4_CKE_SEL		Fld(1, 22)
#define RG_ARCMD_REV_BIT_13_TX_DDR3_CKE_SEL		Fld(1, 21)
#define RG_ARCMD_REV_BIT_1208_TX_CKE_DRVN		Fld(5, 16)
#define RG_ARCMD_REV_BIT_07_MCK4X_SEL_CKE1		Fld(1, 15)
#define RG_ARCMD_REV_BIT_06_MCK4X_SEL_CKE0		Fld(1, 14)
#define RG_ARCMD_REV_BIT_05_RX_SER_RST_MODE		Fld(1, 13)
#define RG_ARCMD_REV_BIT_04_RX_DQSIEN_RB_DLY		Fld(1, 12)
#define RG_ARCMD_REV_BIT_03_RX_DQS_GATE_EN_MODE		Fld(1, 11)
#define RG_ARCMD_REV_BIT_02_TX_LSH_DQM_CG_EN		Fld(1, 10)
#define RG_ARCMD_REV_BIT_01_TX_LSH_DQS_CG_EN		Fld(1, 9)
#define RG_ARCMD_REV_BIT_00_TX_LSH_DQ_CG_EN		Fld(1, 8)


//DDRPHY_SHU1_CA_CMD6
//DDRPHY_SHU1_B0_DQ6
//DDRPHY_SHU1_B1_DQ6
#define RG_ARPI_RESERVE_BIT_15_PSMUX_DRV_SEL		Fld(1, 21)
#define RG_ARPI_RESERVE_BIT_14_LP3_SEL			Fld(1, 20)
#define RG_ARPI_RESERVE_BIT_13_CG_SYNC_ENB		Fld(1, 19)
#define RG_ARPI_RESERVE_BIT_12_BYPASS_SR_DQS		Fld(1, 18)
#define RG_ARPI_RESERVE_BIT_11_BYPASS_SR		Fld(1, 17)
#define RG_ARPI_RESERVE_BIT_10_BUFGP_XLATCH_FORCE_DQS	Fld(1, 16)
#define RG_ARPI_RESERVE_BIT_09_BUFGP_XLATCH_FORCE	Fld(1, 15)
#define RG_ARPI_RESERVE_BIT_08_SMT_XLATCH_FORCE_DQS	Fld(1, 14)
#define RG_ARPI_RESERVE_BIT_07_SMT_XLATCH_FORCE		Fld(1, 13)
#define RG_ARPI_RESERVE_BIT_06_PSMUX_XLATCH_FORCEDQS	Fld(1, 12)
#define RG_ARPI_RESERVE_BIT_05_PSMUX_XLATCH_FORCE	Fld(1, 11)
#define RG_ARPI_RESERVE_BIT_04_8PHASE_XLATCH_FORCE	Fld(1, 10)
#define RG_ARPI_RESERVE_BIT_03_MIDPI_CAP_SEL		Fld(1, 9)
#define RG_ARPI_RESERVE_BIT_02_HYST_SEL			Fld(1, 8)
#define RG_ARPI_RESERVE_BIT_01_DLL_FAST_PSJP		Fld(1, 7)
#define RG_ARPI_RESERVE_BIT_00_TX_CG_EN			Fld(1, 6)


//DDRPHY_SHU1_PLL6
//DDRPHY_SHU1_PLL4
#define RG_PLL_RESERVE_BIT_13_PLL_FS_EN			Fld(1, 13)

#endif /*__DDRPHY_WO_PLL_REG_H__*/
