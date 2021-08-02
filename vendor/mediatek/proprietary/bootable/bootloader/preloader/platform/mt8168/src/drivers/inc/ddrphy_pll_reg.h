#ifndef __DDRPHY_PLL_REG_H__
#define __DDRPHY_PLL_REG_H__

#define DDRPHY_BASE_ADDR Channel_A_PHY_AO_BASE_VIRTUAL

#define DDRPHY_PLL1                                    (DDRPHY_BASE_ADDR + 0x00000)
    #define PLL1_RG_RPHYPLL_SDM_SSC_EN                         Fld(1, 2) //[2:2]
    #define PLL1_RG_RPHYPLL_EN                                 Fld(1, 31) //[31:31]

#define DDRPHY_PLL2                                    (DDRPHY_BASE_ADDR + 0x00004)
    #define PLL2_RG_RCLRPLL_SDM_SSC_EN                         Fld(1, 2) //[2:2]
    #define PLL2_RG_RCLRPLL_EN                                 Fld(1, 31) //[31:31]

#define DDRPHY_PLL3                                    (DDRPHY_BASE_ADDR + 0x00008)
    #define PLL3_RG_RPHYPLL_TSTOP_EN                           Fld(1, 0) //[0:0]
    #define PLL3_RG_RPHYPLL_TSTOD_EN                           Fld(1, 1) //[1:1]
    #define PLL3_RG_RPHYPLL_TSTFM_EN                           Fld(1, 2) //[2:2]
    #define PLL3_RG_RPHYPLL_TSTCK_EN                           Fld(1, 3) //[3:3]
    #define PLL3_RG_RPHYPLL_TST_EN                             Fld(1, 4) //[4:4]
    #define PLL3_RG_RPHYPLL_TSTLVROD_EN                        Fld(1, 5) //[5:5]
    #define PLL3_RG_RPHYPLL_TST_SEL                            Fld(4, 8) //[11:8]

#define DDRPHY_PLL4                                    (DDRPHY_BASE_ADDR + 0x0000C)
    #define PLL4_RG_RPHYPLL_RESETB                             Fld(1, 16) //[16:16]
    #define PLL4_RG_RPHYPLL_ATPG_EN                            Fld(1, 17) //[17:17]
    #define PLL4_RG_RPHYPLL_MCK8X_SEL                          Fld(1, 18) //[18:18]
    #define PLL4_PLL4_RFU                                      Fld(1, 19) //[19:19]
    #define PLL4_RG_RPHYPLL_SER_MODE                           Fld(1, 20) //[20:20]
    #define PLL4_RG_RPHYPLL_AD_MCK8X_EN                        Fld(1, 21) //[21:21]
    #define PLL4_RG_RPHYPLL_ADA_MCK8X_EN                       Fld(1, 22) //[22:22]
    #define PLL4_RESERVED_0X0C                                 Fld(1, 24) //[24:24]

#define DDRPHY_PLL5                                    (DDRPHY_BASE_ADDR + 0x00010)
    #define PLL5_RESERVED_0X010                                Fld(32, 0) //[31:0]

#define DDRPHY_PLL6                                    (DDRPHY_BASE_ADDR + 0x00014)
    #define PLL6_RESERVED_0X014                                Fld(32, 0) //[31:0]

#define DDRPHY_PLL7                                    (DDRPHY_BASE_ADDR + 0x00018)
    #define PLL7_RESERVED_0X018                                Fld(32, 0) //[31:0]

#define DDRPHY_PLL8                                    (DDRPHY_BASE_ADDR + 0x0001C)
    #define PLL8_RESERVED_0X01C                                Fld(32, 0) //[31:0]

#define DDRPHY_PLL9                                    (DDRPHY_BASE_ADDR + 0x00020)
    #define PLL9_RESERVED_0X020                                Fld(32, 0) //[31:0]

#define DDRPHY_PLL10                                   (DDRPHY_BASE_ADDR + 0x00024)
    #define PLL10_RESERVED_0X024                               Fld(32, 0) //[31:0]

#define DDRPHY_PLL11                                   (DDRPHY_BASE_ADDR + 0x00028)
    #define PLL11_RESERVED_0X028                               Fld(32, 0) //[31:0]

#define DDRPHY_PLL12                                   (DDRPHY_BASE_ADDR + 0x0002C)
    #define PLL12_RESERVED_0X02C                               Fld(32, 0) //[31:0]

#define DDRPHY_PLL13                                   (DDRPHY_BASE_ADDR + 0x00030)
    #define PLL13_RESERVED_0X030                               Fld(32, 0) //[31:0]

#define DDRPHY_PLL14                                   (DDRPHY_BASE_ADDR + 0x00034)
    #define PLL14_RESERVED_0X034                               Fld(32, 0) //[31:0]

#define DDRPHY_PLL15                                   (DDRPHY_BASE_ADDR + 0x00038)
    #define PLL15_RESERVED_0X038                               Fld(32, 0) //[31:0]

#define DDRPHY_PLL16                                   (DDRPHY_BASE_ADDR + 0x0003C)
    #define PLL16_RESERVED_0X03C                               Fld(32, 0) //[31:0]

#define DDRPHY_SHU_PLL0                                (DDRPHY_BASE_ADDR + 0x00D80)
    #define SHU_PLL0_RG_RPHYPLL_TOP_REV                        Fld(16, 0) //[15:0]
    #define SHU_PLL0_RG_RPHYPLL_LOAD_EN                        Fld(1, 19) //[19:19]

#define DDRPHY_SHU_PLL1                                (DDRPHY_BASE_ADDR + 0x00D84)
    #define SHU_PLL1_RG_RPHYPLLGP_CK_SEL                       Fld(1, 0) //[0:0]
    #define SHU_PLL1_SHU1_PLL1_RFU                             Fld(3, 1) //[3:1]
    #define SHU_PLL1_R_SHU_AUTO_PLL_MUX                        Fld(1, 4) //[4:4]
    #define SHU_PLL1_RESERVED_0XD84                            Fld(27, 5) //[31:5]

#define DDRPHY_SHU_PLL2                                (DDRPHY_BASE_ADDR + 0x00D88)
    #define SHU_PLL2_RG_RCLRPLL_LOAD_EN                        Fld(1, 19) //[19:19]

#define DDRPHY_SHU_PLL3                                (DDRPHY_BASE_ADDR + 0x00D8C)
    #define SHU_PLL3_RESERVED_0XD8C                            Fld(32, 0) //[31:0]

#define DDRPHY_SHU_PLL4                                (DDRPHY_BASE_ADDR + 0x00D90)
    #define SHU_PLL4_RG_RPHYPLL_RESERVED                       Fld(16, 0) //[15:0]
    #define SHU_PLL4_RG_RPHYPLL_FS                             Fld(2, 18) //[19:18]
    #define SHU_PLL4_RG_RPHYPLL_BW                             Fld(3, 20) //[22:20]
    #define SHU_PLL4_RG_RPHYPLL_ICHP                           Fld(2, 24) //[25:24]
    #define SHU_PLL4_RG_RPHYPLL_IBIAS                          Fld(2, 26) //[27:26]
    #define SHU_PLL4_RG_RPHYPLL_BLP                            Fld(1, 29) //[29:29]
    #define SHU_PLL4_RG_RPHYPLL_BR                             Fld(1, 30) //[30:30]
    #define SHU_PLL4_RG_RPHYPLL_BP                             Fld(1, 31) //[31:31]

#define DDRPHY_SHU_PLL5                                (DDRPHY_BASE_ADDR + 0x00D94)
    #define SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN                     Fld(1, 0) //[0:0]
    #define SHU_PLL5_RG_RPHYPLL_SDM_PCW_CHG                    Fld(1, 1) //[1:1]
    #define SHU_PLL5_RG_RPHYPLL_SDM_PCW                        Fld(16, 16) //[31:16]

#define DDRPHY_SHU_PLL6                                (DDRPHY_BASE_ADDR + 0x00D98)
    #define SHU_PLL6_RG_RCLRPLL_RESERVED                       Fld(16, 0) //[15:0]
    #define SHU_PLL6_RG_RCLRPLL_FS                             Fld(2, 18) //[19:18]
    #define SHU_PLL6_RG_RCLRPLL_BW                             Fld(3, 20) //[22:20]
    #define SHU_PLL6_RG_RCLRPLL_ICHP                           Fld(2, 24) //[25:24]
    #define SHU_PLL6_RG_RCLRPLL_IBIAS                          Fld(2, 26) //[27:26]
    #define SHU_PLL6_RG_RCLRPLL_BLP                            Fld(1, 29) //[29:29]
    #define SHU_PLL6_RG_RCLRPLL_BR                             Fld(1, 30) //[30:30]
    #define SHU_PLL6_RG_RCLRPLL_BP                             Fld(1, 31) //[31:31]

#define DDRPHY_SHU_PLL7                                (DDRPHY_BASE_ADDR + 0x00D9C)
    #define SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN                     Fld(1, 0) //[0:0]
    #define SHU_PLL7_RG_RCLRPLL_SDM_PCW_CHG                    Fld(1, 1) //[1:1]
    #define SHU_PLL7_RG_RCLRPLL_SDM_PCW                        Fld(16, 16) //[31:16]

#define DDRPHY_SHU_PLL8                                (DDRPHY_BASE_ADDR + 0x00DA0)
    #define SHU_PLL8_RG_RPHYPLL_POSDIV                         Fld(3, 0) //[2:0]
    #define SHU_PLL8_RG_RPHYPLL_PREDIV                         Fld(2, 18) //[19:18]

#define DDRPHY_SHU_PLL9                                (DDRPHY_BASE_ADDR + 0x00DA4)
    #define SHU_PLL9_RG_RPHYPLL_RST_DLY                        Fld(2, 8) //[9:8]
    #define SHU_PLL9_RG_RPHYPLL_LVROD_EN                       Fld(1, 12) //[12:12]
    #define SHU_PLL9_RG_RPHYPLL_MONREF_EN                      Fld(1, 13) //[13:13]
    #define SHU_PLL9_RG_RPHYPLL_MONVC_EN                       Fld(2, 14) //[15:14]
    #define SHU_PLL9_RG_RPHYPLL_MONCK_EN                       Fld(1, 16) //[16:16]

#define DDRPHY_SHU_PLL10                               (DDRPHY_BASE_ADDR + 0x00DA8)
    #define SHU_PLL10_RG_RCLRPLL_POSDIV                        Fld(3, 0) //[2:0]
    #define SHU_PLL10_RG_RCLRPLL_PREDIV                        Fld(2, 18) //[19:18]

#define DDRPHY_SHU_PLL11                               (DDRPHY_BASE_ADDR + 0x00DAC)
    #define SHU_PLL11_RG_RCLRPLL_RST_DLY                       Fld(2, 8) //[9:8]
    #define SHU_PLL11_RG_RCLRPLL_LVROD_EN                      Fld(1, 12) //[12:12]
    #define SHU_PLL11_RG_RCLRPLL_MONREF_EN                     Fld(1, 13) //[13:13]
    #define SHU_PLL11_RG_RCLRPLL_MONVC_EN                      Fld(2, 14) //[15:14]
    #define SHU_PLL11_RG_RCLRPLL_MONCK_EN                      Fld(1, 16) //[16:16]

#define DDRPHY_SHU_PLL12                               (DDRPHY_BASE_ADDR + 0x00DB0)
    #define SHU_PLL12_RG_RCLRPLL_EXT_PODIV                     Fld(6, 0) //[5:0]
    #define SHU_PLL12_RG_RCLRPLL_BYPASS                        Fld(1, 6) //[6:6]
    #define SHU_PLL12_RG_RCLRPLL_EXTPODIV_EN                   Fld(1, 7) //[7:7]
    #define SHU_PLL12_RG_RCLRPLL_EXT_FBDIV                     Fld(6, 8) //[13:8]
    #define SHU_PLL12_RG_RCLRPLL_EXTFBDIV_EN                   Fld(1, 15) //[15:15]
    #define SHU_PLL12_RG_RPHYPLL_EXT_FBDIV                     Fld(6, 16) //[21:16]
    #define SHU_PLL12_RG_RPHYPLL_EXTFBDIV_EN                   Fld(1, 22) //[22:22]

#define DDRPHY_SHU_PLL13                               (DDRPHY_BASE_ADDR + 0x00DB4)
    #define SHU_PLL13_RG_RCLRPLL_FB_DL                         Fld(6, 0) //[5:0]
    #define SHU_PLL13_RG_RCLRPLL_REF_DL                        Fld(6, 8) //[13:8]
    #define SHU_PLL13_RG_RPHYPLL_FB_DL                         Fld(6, 16) //[21:16]
    #define SHU_PLL13_RG_RPHYPLL_REF_DL                        Fld(6, 24) //[29:24]

#define DDRPHY_SHU_PLL14                               (DDRPHY_BASE_ADDR + 0x00DB8)
    #define SHU_PLL14_RG_RPHYPLL_SDM_HREN                      Fld(1, 0) //[0:0]
    #define SHU_PLL14_RG_RPHYPLL_SDM_SSC_PH_INIT               Fld(1, 1) //[1:1]
    #define SHU_PLL14_RG_RPHYPLL_SDM_SSC_PRD                   Fld(16, 16) //[31:16]

#define DDRPHY_SHU_PLL15                               (DDRPHY_BASE_ADDR + 0x00DBC)
    #define SHU_PLL15_RG_RPHYPLL_SDM_SSC_DELTA                 Fld(16, 0) //[15:0]
    #define SHU_PLL15_RG_RPHYPLL_SDM_SSC_DELTA1                Fld(16, 16) //[31:16]

#define DDRPHY_SHU_PLL20                               (DDRPHY_BASE_ADDR + 0x00DD0)
    #define SHU_PLL20_RG_RCLRPLL_SDM_HREN                      Fld(1, 0) //[0:0]
    #define SHU_PLL20_RG_RCLRPLL_SDM_SSC_PH_INIT               Fld(1, 1) //[1:1]
    #define SHU_PLL20_RG_RCLRPLL_SDM_SSC_PRD                   Fld(16, 16) //[31:16]

#define DDRPHY_SHU_PLL21                               (DDRPHY_BASE_ADDR + 0x00DD4)
    #define SHU_PLL21_RG_RCLRPLL_SDM_SSC_DELTA                 Fld(16, 0) //[15:0]
    #define SHU_PLL21_RG_RCLRPLL_SDM_SSC_DELTA1                Fld(16, 16) //[31:16]

#endif /*__DDRPHY_PLL_REG_H__*/
