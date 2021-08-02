/** @file
*
*  Copyright (c) 2016-2017, Linaro Ltd. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#ifndef __HI3660_H__
#define __HI3660_H__

#define HKADC_SSI_REG_BASE                      0xE82B8000

#define PCTRL_REG_BASE                          0xE8A09000

#define PCTRL_CTRL3                             (PCTRL_REG_BASE + 0x010)
#define PCTRL_CTRL24                            (PCTRL_REG_BASE + 0x064)

#define PCTRL_CTRL3_USB_TXCO_EN                 (1 << 1)
#define PCTRL_CTRL24_USB3PHY_3MUX1_SEL          (1 << 25)

#define SCTRL_REG_BASE                          0xFFF0A000

#define SCTRL_SCFPLLCTRL0                       (SCTRL_REG_BASE + 0x120)
#define SCTRL_SCFPLLCTRL0_FPLL0_EN              (1 << 0)

#define SCTRL_BAK_DATA0                         (SCTRL_REG_BASE + 0x40C)

#define USB3OTG_BC_REG_BASE                     0xFF200000

#define USB3OTG_CTRL0                           (USB3OTG_BC_REG_BASE + 0x000)
#define USB3OTG_CTRL2                           (USB3OTG_BC_REG_BASE + 0x008)
#define USB3OTG_CTRL3                           (USB3OTG_BC_REG_BASE + 0x00C)
#define USB3OTG_CTRL4                           (USB3OTG_BC_REG_BASE + 0x010)
#define USB3OTG_CTRL6                           (USB3OTG_BC_REG_BASE + 0x018)
#define USB3OTG_CTRL7                           (USB3OTG_BC_REG_BASE + 0x01C)
#define USB3OTG_PHY_CR_STS                      (USB3OTG_BC_REG_BASE + 0x050)
#define USB3OTG_PHY_CR_CTRL                     (USB3OTG_BC_REG_BASE + 0x054)

#define USB3OTG_CTRL0_SC_USB3PHY_ABB_GT_EN      (1 << 15)
#define USB3OTG_CTRL2_TEST_POWERDOWN_SSP        (1 << 1)
#define USB3OTG_CTRL2_TEST_POWERDOWN_HSP        (1 << 0)
#define USB3OTG_CTRL3_VBUSVLDEXT                (1 << 6)
#define USB3OTG_CTRL3_VBUSVLDEXTSEL             (1 << 5)
#define USB3OTG_CTRL7_REF_SSP_EN                (1 << 16)
#define USB3OTG_PHY_CR_DATA_OUT(x)              (((x) & 0xFFFF) << 1)
#define USB3OTG_PHY_CR_ACK                      (1 << 0)
#define USB3OTG_PHY_CR_DATA_IN(x)               (((x) & 0xFFFF) << 4)
#define USB3OTG_PHY_CR_WRITE                    (1 << 3)
#define USB3OTG_PHY_CR_READ                     (1 << 2)
#define USB3OTG_PHY_CR_CAP_DATA                 (1 << 1)
#define USB3OTG_PHY_CR_CAP_ADDR                 (1 << 0)

#define PMU_REG_BASE                            0xFFF34000
#define PMIC_HARDWARE_CTRL0                     (PMU_REG_BASE + (0x0C5 << 2))
#define PMIC_OSC32K_ONOFF_CTRL                  (PMU_REG_BASE + (0x0CC << 2))

#define PMIC_HARDWARE_CTRL0_WIFI_CLK            (1 << 5)
#define PMIC_OSC32K_ONOFF_CTRL_EN_32K           (1 << 1)


#define CRG_REG_BASE                            0xFFF35000

#define CRG_PEREN2                              (CRG_REG_BASE + 0x020)
#define CRG_PERDIS2                             (CRG_REG_BASE + 0x024)
#define CRG_PERCLKEN2                           (CRG_REG_BASE + 0x028)
#define CRG_PERSTAT2                            (CRG_REG_BASE + 0x02C)
#define CRG_PEREN4                              (CRG_REG_BASE + 0x040)
#define CRG_PERDIS4                             (CRG_REG_BASE + 0x044)
#define CRG_PERCLKEN4                           (CRG_REG_BASE + 0x048)
#define CRG_PERSTAT4                            (CRG_REG_BASE + 0x04C)
#define CRG_PERRSTEN2                           (CRG_REG_BASE + 0x078)
#define CRG_PERRSTDIS2                          (CRG_REG_BASE + 0x07C)
#define CRG_PERRSTSTAT2                         (CRG_REG_BASE + 0x080)
#define CRG_PERRSTEN3                           (CRG_REG_BASE + 0x084)
#define CRG_PERRSTDIS3                          (CRG_REG_BASE + 0x088)
#define CRG_PERRSTSTAT3                         (CRG_REG_BASE + 0x08C)
#define CRG_PERRSTEN4                           (CRG_REG_BASE + 0x090)
#define CRG_PERRSTDIS4                          (CRG_REG_BASE + 0x094)
#define CRG_PERRSTSTAT4                         (CRG_REG_BASE + 0x098)
#define CRG_ISOEN                               (CRG_REG_BASE + 0x144)
#define CRG_ISODIS                              (CRG_REG_BASE + 0x148)
#define CRG_ISOSTAT                             (CRG_REG_BASE + 0x14C)

#define PERI_UFS_BIT                            (1 << 12)
#define PERI_ARST_UFS_BIT                       (1 << 7)

#define PEREN2_HKADCSSI                         BIT24

#define PEREN4_GT_ACLK_USB3OTG                  (1 << 1)
#define PEREN4_GT_CLK_USB3OTG_REF               (1 << 0)

#define PERRSTEN2_HKADCSSI                      BIT24

#define PERRSTEN4_USB3OTG_MUX                   (1 << 8)
#define PERRSTEN4_USB3OTG_AHBIF                 (1 << 7)
#define PERRSTEN4_USB3OTG_32K                   (1 << 6)
#define PERRSTEN4_USB3OTG                       (1 << 5)
#define PERRSTEN4_USB3OTGPHY_POR                (1 << 3)

#define PERISOEN_USB_REFCLK_ISO_EN              (1 << 25)

#define CRG_CLKDIV16_OFFSET                     0x0E8
#define SC_DIV_UFSPHY_CFG_MASK                  (0x3 << 9)
#define SC_DIV_UFSPHY_CFG(x)                    (((x) & 0x3) << 9)

#define CRG_CLKDIV17_OFFSET                     0x0EC
#define SC_DIV_UFS_PERIBUS                      (1 << 14)

#define UFS_SYS_REG_BASE                        0xFF3B1000

#define UFS_SYS_PSW_POWER_CTRL_OFFSET           0x004
#define UFS_SYS_PHY_ISO_EN_OFFSET               0x008
#define UFS_SYS_HC_LP_CTRL_OFFSET               0x00C
#define UFS_SYS_PHY_CLK_CTRL_OFFSET             0x010
#define UFS_SYS_PSW_CLK_CTRL_OFFSET             0x014
#define UFS_SYS_CLOCK_GATE_BYPASS_OFFSET        0x018
#define UFS_SYS_RESET_CTRL_EN_OFFSET            0x01C
#define UFS_SYS_MONITOR_HH_OFFSET               0x03C
#define UFS_SYS_UFS_SYSCTRL_OFFSET              0x05C
#define UFS_SYS_UFS_DEVICE_RESET_CTRL_OFFSET    0x060
#define UFS_SYS_UFS_APB_ADDR_MASK_OFFSET        0x064

#define BIT_UFS_PSW_ISO_CTRL                    (1 << 16)
#define BIT_UFS_PSW_MTCMOS_EN                   (1 << 0)
#define BIT_UFS_REFCLK_ISO_EN                   (1 << 16)
#define BIT_UFS_PHY_ISO_CTRL                    (1 << 0)
#define BIT_SYSCTRL_LP_ISOL_EN                  (1 << 16)
#define BIT_SYSCTRL_PWR_READY                   (1 << 8)
#define BIT_SYSCTRL_REF_CLOCK_EN                (1 << 24)
#define MASK_SYSCTRL_REF_CLOCK_SEL              (3 << 8)
#define MASK_SYSCTRL_CFG_CLOCK_FREQ             (0xFF)
#define BIT_SYSCTRL_PSW_CLK_EN                  (1 << 4)
#define MASK_UFS_CLK_GATE_BYPASS                (0x3F)
#define BIT_SYSCTRL_LP_RESET_N                  (1 << 0)
#define BIT_UFS_REFCLK_SRC_SE1                  (1 << 0)
#define MASK_UFS_SYSCTRL_BYPASS                 (0x3F << 16)
#define MASK_UFS_DEVICE_RESET                   (1 << 16)
#define BIT_UFS_DEVICE_RESET                    (1 << 0)

#define SCTRL_SCBAKDATA7                        (SCTRL_REG_BASE + 0x428)
#define HIKEY_REGION_SIZE(x)                    ((((x) >> 0x8) & 0xF) << 30)

#endif /* __HI3660_H__ */
