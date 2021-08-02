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
#include "platform.h"

#include "pll.h"
#include "timer.h"
#include "spm.h"
#include "spm_mtcmos.h"
#include "wdt.h"
#include "emi.h"

#include "../security/inc/sec_devinfo.h"

typedef volatile unsigned int *V_UINT32P;

#define ALL_CLK_ON      0
#define DEBUG_FQMTR     0

#if DEBUG_FQMTR

#define fqmtr_err(fmt, args...)     print(fmt, ##args)
#define fqmtr_warn(fmt, args...)    print(fmt, ##args)
#define fqmtr_info(fmt, args...)    print(fmt, ##args)
#define fqmtr_dbg(fmt, args...)     print(fmt, ##args)
#define fqmtr_ver(fmt, args...)     print(fmt, ##args)

#define fqmtr_read(addr)            DRV_Reg32(addr)
#define fqmtr_write(addr, val)      DRV_WriteReg32(val, addr)

#define FREQ_MTR_CTRL_REG       (CKSYS_BASE + 0x10)
#define FREQ_MTR_CTRL_RDATA     (CKSYS_BASE + 0x14)

#define RG_FQMTR_CKDIV_GET(x)           (((x) >> 28) & 0x3)
#define RG_FQMTR_CKDIV_SET(x)           (((x)& 0x3) << 28)
#define RG_FQMTR_FIXCLK_SEL_GET(x)      (((x) >> 24) & 0x3)
#define RG_FQMTR_FIXCLK_SEL_SET(x)      (((x)& 0x3) << 24)
#define RG_FQMTR_MONCLK_SEL_GET(x)      (((x) >> 16) & 0x7f)
#define RG_FQMTR_MONCLK_SEL_SET(x)      (((x)& 0x7f) << 16)
#define RG_FQMTR_MONCLK_EN_GET(x)       (((x) >> 15) & 0x1)
#define RG_FQMTR_MONCLK_EN_SET(x)       (((x)& 0x1) << 15)
#define RG_FQMTR_MONCLK_RST_GET(x)      (((x) >> 14) & 0x1)
#define RG_FQMTR_MONCLK_RST_SET(x)      (((x)& 0x1) << 14)
#define RG_FQMTR_MONCLK_WINDOW_GET(x)   (((x) >> 0) & 0xfff)
#define RG_FQMTR_MONCLK_WINDOW_SET(x)   (((x)& 0xfff) << 0)

#define RG_FQMTR_CKDIV_DIV_2    0
#define RG_FQMTR_CKDIV_DIV_4    1
#define RG_FQMTR_CKDIV_DIV_8    2
#define RG_FQMTR_CKDIV_DIV_16   3

#define RG_FQMTR_FIXCLK_26MHZ   0
#define RG_FQMTR_FIXCLK_32KHZ   2

enum rg_fqmtr_monclk
{
    FM_NULL                = 0,
    FM_MAINPLL_DIV8        = 1,
    FM_MAINPLL_DIV11       = 2,
    FM_MAINPLL_DIV12       = 3,
    FM_MAINPLL_DIV20       = 4,
    FM_MAINPLL_DIV7        = 5,
    FM_UNIVPLL_DIV16       = 6,
    FM_UNIVPLL_DIV24       = 7,
    FM_NFI2X               = 8,
    FM_WHPLL               = 9,
    FM_WPLL                = 10,
    FM_26MHZ               = 11,
    FM_USB_48MHZ           = 12,
    FM_EMI1X               = 13,
    FM_AP_INFRA_FAST_BUS   = 14,
    FM_SMI                 = 15,
    FM_UART0               = 16,
    FM_UART1               = 17,
    FM_GPU                 = 18,
    FM_MSDC0               = 19,
    FM_MSDC1               = 20,
    FM_AD_DSI0_LNTC_DSICLK = 21,
    FM_AD_MPPLL_TST_CK     = 22,
    FM_AP_PLLLGP_TST_CK    = 23,
    FM_52MHZ               = 24,
    FM_ARMPLL              = 25,
    FM_32KHZ               = 26,
    FM_AD_MEMPLL_MONCLK    = 27,
    FM_AD_MEMPLL2_MONCLK   = 28,
    FM_AD_MEMPLL3_MONCLK   = 29,
    FM_AD_MEMPLL4_MONCLK   = 30,
    FM_RESERVED            = 31,
    FM_CAM_SENINF          = 32,
    FM_SCAM                = 33,
    FM_PWM_OF_MMSYS        = 34,
    FM_DDRPHYCFG           = 35,
    FM_PMIC_SPI            = 36,
    FM_SPI                 = 37,
    FM_104MHZ              = 38,
    FM_USB_78MHZ           = 39,
    FM_SPINOR              = 40,
    FM_ETHERNET            = 41,
    FM_VDEC                = 42,
    FM_FDPI0               = 43,
    FM_FDPI1               = 44,
    FM_AXI_MFG             = 45,
    FM_SLOW_MFG            = 46,
    FM_AUD1                = 47,
    FM_AUD2                = 48,
    FM_AUD_ENGEN1          = 49,
    FM_AUD_ENGEN2          = 50,
    FM_I2C                 = 51,
    FM_PWM                 = 52,
    FM_AUD_SPDIF_IN        = 53,
    FM_UART2               = 54,
    FM_BSI                 = 55,
    FM_DBG_ATCLK           = 56,
    FM_NFIECC              = 57,
    FM_LVDSTX_MONITOR      = 58,
    FM_LVDS_MONITOR_REF    = 59,
    FM_LVDS_MONITOR_FB     = 60,
    FM_USB20_480M          = 61,
    FM_HDMITX_MONITOR      = 62,
    FM_HDMITX_CLKDIG_CTS   = 63,
    FM_ARMPLL_650M         = 64,
    FM_MAINPLL_1501P5M     = 65,
    FM_UNIVPLL_1248M       = 66,
    FM_MMPLL               = 67,
    FM_TVDPLL_594M         = 68,
    FM_AUD1PLL_180P6336M   = 69,
    FM_AUD2PLL_196P608M    = 70,
    FM_LVDSPLL_150M        = 71,
    FM_USB20_48M           = 72,
    FM_UNIVPLL_48M         = 73,
    FM_MMPLL_D3            = 74,
    FM_MIPI_26M            = 75,
    FM_SYS_26M             = 76,
    FM_MEM_26M             = 77,
    FM_MEMPLL5_MONITOR     = 78,
    FM_MEMPLL6_MONITOR     = 79,
    FM_END,
};

const char *rg_fqmtr_monclk_name[] =
{
    [0]  = "Power down (no clock)",
    [1]  = "mainpll div8",
    [2]  = "mainpll div11",
    [3]  = "mainpll div12",
    [4]  = "mainpll div20",
    [5]  = "mainpll div7",
    [6]  = "univpll div16",
    [7]  = "univpll div24",
    [8]  = "nfi2x",
    [9]  = "WHPLL",
    [10] = "WPLL",
    [11] = "26MHz",
    [12] = "USB 48MHz",
    [13] = "emi1x",
    [14] = "AP infra fast bus",
    [15] = "SMI",
    [16] = "UART0",
    [17] = "UART1",
    [18] = "GPU",
    [19] = "MSDC0",
    [20] = "MSDC1",
    [21] = "AD_DSI0_LNTC_DSICLK (mipi)",
    [22] = "AD_MPPLL_TST_CK (mipi)",
    [23] = "AP_PLLLGP_TST_CK",
    [24] = "52MHz",
    [25] = "ARMPLL",
    [26] = "32kHz",
    [27] = "AD_MEMPLL_MONCLK",
    [28] = "AD_MEMPLL2_MONCLK",
    [29] = "AD_MEMPLL3_MONCLK",
    [30] = "AD_MEMPLL4_MONCLK",
    [31] = "Reserved",
    [32] = "CAM SENINF",
    [33] = "SCAM",
    [34] = "PWM of MMSYS",
    [35] = "ddrphycfg",
    [36] = "PMIC SPI",
    [37] = "SPI",
    [38] = "104MHz",
    [39] = "USB 78MHz",
    [40] = "spinor",
    [41] = "ethernet",
    [42] = "vdec",
    [43] = "fdpi0",
    [44] = "fdpi1",
    [45] = "axi mfg",
    [46] = "slow mfg",
    [47] = "aud1",
    [48] = "aud2",
    [49] = "aud engen1",
    [50] = "aud engen2",
    [51] = "i2c",
    [52] = "pwm",
    [53] = "aud spdif in",
    [54] = "uart2",
    [55] = "bsi",
    [56] = "dbg atclk",
    [57] = "nfiecc",
    [58] = "lvdstx monitor",
    [59] = "lvds monitor ref",
    [60] = "lvds monitor fb",
    [61] = "usb20 480M",
    [62] = "hdmitx monitor",
    [63] = "hdmitx clkdig cts",
    [64] = "armpll 650M",
    [65] = "mainpll 1501.5M",
    [66] = "univpll 1248M",
    [67] = "mmpll",
    [68] = "tvdpll 594M",
    [69] = "aud1pll 180.6336M",
    [70] = "aud2pll 196.608M",
    [71] = "lvdspll 150M",
    [72] = "usb20 48M",
    [73] = "univpll 48M",
    [74] = "mmpll/3",
    [75] = "mipi 26M",
    [76] = "sys 26M",
    [77] = "mem 26M",
    [78] = "MEMPLL5 monitor",
    [79] = "MEMPLL6 monitor",
    [FM_END] = "FM_END",
};

#define RG_FQMTR_EN     1
#define RG_FQMTR_RST    1

#define RG_FRMTR_WINDOW     0x100

unsigned int do_fqmtr_ctrl(int fixclk, int monclk_sel)
{
    u32 value = 0;

    BUG_ON(!((fixclk == RG_FQMTR_FIXCLK_26MHZ) | (fixclk == RG_FQMTR_FIXCLK_32KHZ)));
    // reset
    DRV_WriteReg32(FREQ_MTR_CTRL_REG, RG_FQMTR_MONCLK_RST_SET(RG_FQMTR_RST));
    // reset deassert
    DRV_WriteReg32(FREQ_MTR_CTRL_REG, RG_FQMTR_MONCLK_RST_SET(!RG_FQMTR_RST));
    // set window and target
    DRV_WriteReg32(FREQ_MTR_CTRL_REG, RG_FQMTR_MONCLK_WINDOW_SET(RG_FRMTR_WINDOW) |
                RG_FQMTR_MONCLK_SEL_SET(monclk_sel) |
                RG_FQMTR_FIXCLK_SEL_SET(fixclk) |
                RG_FQMTR_MONCLK_EN_SET(RG_FQMTR_EN));
    gpt_busy_wait_us(100);
    value = DRV_Reg32(FREQ_MTR_CTRL_RDATA);
    // reset
    DRV_WriteReg32(FREQ_MTR_CTRL_REG, RG_FQMTR_MONCLK_RST_SET(RG_FQMTR_RST));
    // reset deassert
    DRV_WriteReg32(FREQ_MTR_CTRL_REG, RG_FQMTR_MONCLK_RST_SET(!RG_FQMTR_RST));
    if (fixclk == RG_FQMTR_FIXCLK_26MHZ)
        return ((26 * value) / (RG_FRMTR_WINDOW + 1));
    else
        return ((32000 * value) / (RG_FRMTR_WINDOW + 1));

}


void dump_fqmtr(void)
{
    int i = 0;
    unsigned int ret;

    // fixclk = RG_FQMTR_FIXCLK_26MHZ
    for (i = 0; i < FM_END; i++)
    {
        if (i == FM_RESERVED)
            continue;

        ret = do_fqmtr_ctrl(RG_FQMTR_FIXCLK_26MHZ, i);
        fqmtr_dbg("%s - %d MHz\n", rg_fqmtr_monclk_name[i], ret);
    }
}

unsigned int mt_get_cpu_freq(void)
{
    unsigned output = 0;
#if CFG_FPGA_PLATFORM
    return output;
#else
    return do_fqmtr_ctrl(RG_FQMTR_FIXCLK_26MHZ, FM_ARMPLL);
#endif
}


unsigned int mt_get_mem_freq(void)
{
    unsigned output = 0;
#if CFG_FPGA_PLATFORM
    return output;
#else
    return do_fqmtr_ctrl(RG_FQMTR_FIXCLK_26MHZ, FM_EMI1X);
#endif
}

unsigned int mt_get_bus_freq(void)
{
    unsigned int bus_clk = 26000;
#if CFG_FPGA_PLATFORM
    return bus_clk; // Khz
#else
    return do_fqmtr_ctrl(RG_FQMTR_FIXCLK_26MHZ, FM_AP_INFRA_FAST_BUS);
#endif
}
#endif /* DEBUG_FQMTR */

// after pmic_init
void mt_pll_post_init(void)
{
    unsigned int temp;
    int ret;

/*****************
 * xPLL HW Control
 ******************/
    // TBD (mem init)
    DRV_WriteReg32(AP_PLL_CON1, (DRV_Reg32(AP_PLL_CON1) & 0xFCFCEFCC)); // Main, ARM PLL HW Control
    DRV_WriteReg32(AP_PLL_CON2, (DRV_Reg32(AP_PLL_CON2) & 0xFFFFFFFC)); // Main, ARM PLL HW Control

    temp = DRV_Reg32(TOP_CKMUXSEL) & ~0xC;
    DRV_WriteReg32(TOP_CKMUXSEL, temp | 0x4); // switch CPU clock to ARMPLL

#if DEBUG_FQMTR
    dump_fqmtr();

    print("AP_PLL_CON1= 0x%x\n", DRV_Reg32(AP_PLL_CON1));
    print("AP_PLL_CON2= 0x%x\n", DRV_Reg32(AP_PLL_CON2));
    print("CLKSQ_STB_CON0= 0x%x\n", DRV_Reg32(CLKSQ_STB_CON0));
    print("PLL_ISO_CON0= 0x%x\n", DRV_Reg32(PLL_ISO_CON0));
    print("ARMPLL_CON0= 0x%x\n", DRV_Reg32(ARMPLL_CON0));
    print("ARMPLL_CON1= 0x%x\n", DRV_Reg32(ARMPLL_CON1));
    print("ARMPLL_PWR_CON0= 0x%x\n", DRV_Reg32(ARMPLL_PWR_CON0));
    print("MPLL_CON0= 0x%x\n", DRV_Reg32(MAINPLL_CON0));
    print("MPLL_CON1= 0x%x\n", DRV_Reg32(MAINPLL_CON1));
    print("MPLL_PWR_CON0= 0x%x\n", DRV_Reg32(MAINPLL_PWR_CON0));
    print("UPLL_CON0= 0x%x\n", DRV_Reg32(UNIVPLL_CON0));
    print("UPLL_CON1= 0x%x\n", DRV_Reg32(UNIVPLL_CON1));
    print("UPLL_PWR_CON0= 0x%x", DRV_Reg32(UNIVPLL_PWR_CON0));
    print("DISP_CG_CON0= 0x%x, \n", DRV_Reg32(MMSYS_CG_CON0));
    print("DISP_CG_CON1= 0x%x, \n", DRV_Reg32(MMSYS_CG_CON1));
    print("cpu_freq = %dKHz\n", mt_get_cpu_freq());
    print("bus_freq = %dKHz\n", mt_get_bus_freq());
    print("mem_freq = %dKHz\n", mt_get_mem_freq());
#endif /* DEBUG_FQMTR */
}

void mt_pll_init(void)
{
  int ret = 0;
  unsigned int temp;
#if !(CFG_FPGA_PLATFORM)
/*************
 * CLKSQ
 * ***********/
    DRV_WriteReg32(AP_PLL_CON0, (DRV_Reg32(AP_PLL_CON0) | 0x1)); // [0] CLKSQ_EN = 1
    gpt_busy_wait_us(100);  // wait 100us
    DRV_WriteReg32(AP_PLL_CON0, (DRV_Reg32(AP_PLL_CON0) | 0x2)); // [1] CLKSQ_LPF_EN =1

/*************
 * xPLL PWR ON
 **************/
    DRV_WriteReg32(ARMPLL_PWR_CON0, (DRV_Reg32(ARMPLL_PWR_CON0) | 0x1));    // [0]ARMPLL_PWR_ON = 1
    DRV_WriteReg32(MAINPLL_PWR_CON0, (DRV_Reg32(MAINPLL_PWR_CON0) | 0x1));  // [0]MAINPLL_PWR_ON = 1
    DRV_WriteReg32(UNIVPLL_PWR_CON0, (DRV_Reg32(UNIVPLL_PWR_CON0) | 0x1));  // [0]UNIVPLL_PWR_ON = 1
    DRV_WriteReg32(MMPLL_PWR_CON0, (DRV_Reg32(MMPLL_PWR_CON0) | 0x1));      // [0]MMPLL_PWR_ON = 1
    DRV_WriteReg32(APLL1_PWR_CON0, (DRV_Reg32(APLL1_PWR_CON0) | 0x1));      // [0]APLL1_PWR_ON = 1
    DRV_WriteReg32(APLL2_PWR_CON0, (DRV_Reg32(APLL2_PWR_CON0) | 0x1));      // [0]APLL2_PWR_ON = 1
    DRV_WriteReg32(TVDPLL_PWR_CON0, (DRV_Reg32(TVDPLL_PWR_CON0) | 0x1));    // [0]TVDPLL_PWR_ON = 1
    DRV_WriteReg32(LVDSPLL_PWR_CON0, (DRV_Reg32(LVDSPLL_PWR_CON0) | 0x1));  // [0]LVDSPLL_PWR_ON = 1

/*************
 * Wait PWR ready(30ns)
 **************/
    gpt_busy_wait_us(30);

/******************
* xPLL ISO Disable
*******************/
    DRV_WriteReg32(ARMPLL_PWR_CON0, (DRV_Reg32(ARMPLL_PWR_CON0) & 0xFFFFFFFD));   // [2]ARMPLL_ISO_EN = 0
    DRV_WriteReg32(MAINPLL_PWR_CON0, (DRV_Reg32(MAINPLL_PWR_CON0) & 0xFFFFFFFD)); // [2]MAINPLL_ISO_EN = 0
    DRV_WriteReg32(UNIVPLL_PWR_CON0, (DRV_Reg32(UNIVPLL_PWR_CON0) & 0xFFFFFFFD)); // [2]UNIVPLL_ISO_EN = 0
    DRV_WriteReg32(MMPLL_PWR_CON0, (DRV_Reg32(MMPLL_PWR_CON0) & 0xFFFFFFFD));     // [2]MMPLL_ISO_EN = 0
    DRV_WriteReg32(APLL1_PWR_CON0, (DRV_Reg32(APLL1_PWR_CON0) & 0xFFFFFFFD));     // [2]APLL1_ISO_EN = 0
    DRV_WriteReg32(APLL2_PWR_CON0, (DRV_Reg32(APLL2_PWR_CON0) & 0xFFFFFFFD));     // [2]APLL2_ISO_EN = 0
    DRV_WriteReg32(TVDPLL_PWR_CON0, (DRV_Reg32(TVDPLL_PWR_CON0) & 0xFFFFFFFD));   // [2]TVDPLL_ISO_EN = 0
    DRV_WriteReg32(LVDSPLL_PWR_CON0, (DRV_Reg32(LVDSPLL_PWR_CON0) & 0xFFFFFFFD)); // [2]LVDSPLL_ISO_EN = 0

/********************
 * xPLL Frequency Set
 *********************/
    // DRV_WriteReg32(ARMPLL_CON1, 0x810c8000);  // 650 MHz
    DRV_WriteReg32(ARMPLL_CON1, 0x8009a000);  // 1000 MHz

    DRV_WriteReg32(MAINPLL_CON1, 0x800e7000); // 1501 MHz

    DRV_WriteReg32(UNIVPLL_CON1, 0x81000060); // 1248 MHz

    DRV_WriteReg32(MMPLL_CON1, 0x820e9d8a);   // 380 MHz

    DRV_WriteReg32(APLL1_CON1, 0xb7945ea6);   // 180.6 MHz
    DRV_WriteReg32(APLL1_CON0, 0x16);
    DRV_WriteReg32(APLL1_CON_TUNER, 0x37945ea7); // 180.6MHz + 1

    DRV_WriteReg32(APLL2_CON1, 0xbc7ea932);   // 196.6 MHz
    DRV_WriteReg32(APLL2_CON0, 0x16);
    DRV_WriteReg32(APLL2_CON_TUNER, 0x3c7ea933); // 196.6 MHz + 1

    DRV_WriteReg32(TVDPLL_CON1, 0x8216d89e);  // 594 MHz

    DRV_WriteReg32(LVDSPLL_CON1, 0x830b89d9); // 150Mhz

/***********************
 * xPLL Frequency Enable
 ************************/
    DRV_WriteReg32(ARMPLL_CON0, (DRV_Reg32(ARMPLL_CON0) | 0x1));   // [0]ARMPLL_EN = 1
    DRV_WriteReg32(MAINPLL_CON0, (DRV_Reg32(MAINPLL_CON0) | 0x1)); // [0]MAINPLL_EN = 1
    DRV_WriteReg32(UNIVPLL_CON0, (DRV_Reg32(UNIVPLL_CON0) | 0x1)); // [0]UNIVPLL_EN = 1
    DRV_WriteReg32(MMPLL_CON0, (DRV_Reg32(MMPLL_CON0) | 0x1));     // [0]MMPLL_EN = 1
    DRV_WriteReg32(APLL1_CON0, (DRV_Reg32(APLL1_CON0) | 0x1));     // [0]APLL1_EN = 1
    DRV_WriteReg32(APLL2_CON0, (DRV_Reg32(APLL2_CON0) | 0x1));     // [0]APLL2_EN = 1
    DRV_WriteReg32(TVDPLL_CON0, (DRV_Reg32(TVDPLL_CON0) | 0x1D));  // [0]TVDPLL_EN = 1
    DRV_WriteReg32(LVDSPLL_CON0, (DRV_Reg32(LVDSPLL_CON0) | 0x1)); // [0]LVDSPLL_EN = 1

/*************
 * Wait PWR ready(20ns)
 **************/
    gpt_busy_wait_us(20); // wait for PLL stable (min delay is 20us)

/***************
 * xPLL DIV RSTB
 ****************/
    DRV_WriteReg32(MAINPLL_CON0, (DRV_Reg32(MAINPLL_CON0) | 0x08000000)); // [27]MAINPLL_DIV_RSTB = 1
    DRV_WriteReg32(UNIVPLL_CON0, (DRV_Reg32(UNIVPLL_CON0) | 0x08000000)); // ]27]UNIVPLL_DIV_RSTB = 1

/*****************
 * AXI BUS Init
 ******************/
    DRV_WriteReg32(INFRABUS_DCMCTL1, 0x80000000);
    DRV_WriteReg32(CLK_MUX_SEL0, 0x0B60b444); // TODO: update mux_init_setting

/**************
 * INFRA CLKMUX
 ***************/
    DRV_WriteReg32(ACLKEN_DIV, 0x12); // CPU BUS clock freq is divided by 2
    DRV_WriteReg32(PCLKEN_DIV, 0x15); // CPU debug clock freq is divided by 5

/************
 * TOP CLKMUX
 *************/
    DRV_WriteReg32(CLK_MUX_SEL0, (DRV_Reg32(CLK_MUX_SEL0) & ~0x3fffbff7) | 0x2700b724);
    DRV_WriteReg32(CLK_MUX_SEL1, (DRV_Reg32(CLK_MUX_SEL1) & ~0x03ffbfff) | 0x00a98852);
    DRV_WriteReg32(CLK_MUX_SEL8, (DRV_Reg32(CLK_MUX_SEL8) & ~0x3fffffff) | 0x0fdd2208);
    DRV_WriteReg32(CLK_SEL_9, (DRV_Reg32(CLK_SEL_9) & ~0x0007f000) | 0x00000000);
    DRV_WriteReg32(CLK_MUX_SEL13, (DRV_Reg32(CLK_MUX_SEL13) & ~0x0000ffff) | 0x00002528);

/*************
 * for MTCMOS
 *************/
    spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));

#if ALL_CLK_ON
    DRV_WriteReg32(SET_CLK_GATING_CTRL0, 0x1c0);
    DRV_WriteReg32(CLR_CLK_GATING_CTRL0, 0x0000022f);
    DRV_WriteReg32(CLR_CLK_GATING_CTRL1, 0xfbfffffe);
    DRV_WriteReg32(CLR_CLK_GATING_CTRL2, 0x8ffffff7);
    DRV_WriteReg32(CLK_SEL_9, (DRV_Reg32(CLK_SEL_9) & ~0x000001ff) | 0x00000000);
    DRV_WriteReg32(CLR_CLK_GATING_CTRL8, 0x0007ffff);
    DRV_WriteReg32(CLR_CLK_GATING_CTRL9, 0x00003f00);
#else /* !ALL_CLK_ON */
    DRV_WriteReg32(CLK_GATING_CTRL0, 0xffcffdfa);
    DRV_WriteReg32(SET_CLK_GATING_CTRL1, 0x00800000);
    DRV_WriteReg32(SET_CLK_GATING_CTRL2, 0x00000010);

    DRV_WriteReg32(CLR_CLK_GATING_CTRL1, 0x00002800);
    DRV_WriteReg32(CLR_CLK_GATING_CTRL2, 0x85024000);
    DRV_WriteReg32(CLR_CLK_GATING_CTRL8, 0x00018000);
#endif /* ALL_CLK_ON */

    spm_mtcmos_ctrl_disp(STA_POWER_ON);

#if ALL_CLK_ON
    spm_mtcmos_ctrl_isp(STA_POWER_ON);
    spm_mtcmos_ctrl_mfg_async(STA_POWER_ON);
    spm_mtcmos_ctrl_mfg_2d(STA_POWER_ON);
    spm_mtcmos_ctrl_mfg(STA_POWER_ON);
    spm_mtcmos_ctrl_connsys(STA_POWER_ON);
    spm_mtcmos_ctrl_vdec(STA_POWER_ON);
#endif /* ALL_CLK_ON */

/*************
 * for CG
 *************/

#if ALL_CLK_ON
    DRV_WriteReg32(AUDIO_TOP_CON0, (DRV_Reg32(AUDIO_TOP_CON0) & ~0x0f3c8344) | 0x00000000);
    DRV_WriteReg32(MFG_CG_CLR, 0x0000000f);
    DRV_WriteReg32(MMSYS_CG_CLR0, 0x000fffff);
    DRV_WriteReg32(MMSYS_CG_CLR1, 0x003fc03f);
    DRV_WriteReg32(IMG_CG_CLR, 0x000003e1);
    DRV_WriteReg32(VDEC_CKEN_SET, 0x00000001);
    DRV_WriteReg32(VDEC_LARB1_CKEN_SET, 0x00000001);
#else /* !ALL_CLK_ON */
    DRV_WriteReg32(MMSYS_CG_CLR0, 0x3);
    DRV_WriteReg32(MMSYS_CG_CON1, 0x0);
    DRV_WriteReg32(AUDIO_TOP_CON0, (DRV_Reg32(AUDIO_TOP_CON0) | 0x0f3c8344));
#endif /* ALL_CLK_ON */

#endif /* !(CFG_FPGA_PLATFORM) */
}
