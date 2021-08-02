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
#include "boot_device.h"
#include "nand.h"
#include "mmc_common_inter.h"

#include "uart.h"
#include "nand_core.h"
#include "pll.h"
#include "i2c.h"
#include "rtc.h"
#include "emi.h"
#include "pmic.h"
#include "wdt.h"
#include "ram_console.h"
#include "cust_sec_ctrl.h"
#include "gpio.h"
#include "pmic_wrap_init.h"
#include "keypad.h"
#include "usbphy.h"
#include "timer.h"
#include "dram_buffer.h"
#include "spm.h"
#ifndef MTK_EMMC_SUPPORT
#include "nand_device_list.h"
#endif
#include "pl_version.h"
#include "tz_mem.h"

/*============================================================================*/
/* CONSTAND DEFINITIONS                                                       */
/*============================================================================*/
#define MOD "[PLFM]"

/*============================================================================*/
/* GLOBAL VARIABLES                                                           */
/*============================================================================*/
unsigned int sys_stack[CFG_SYS_STACK_SZ >> 2];
const unsigned int sys_stack_sz = CFG_SYS_STACK_SZ;
boot_mode_t g_boot_mode;
boot_dev_t g_boot_dev;
meta_com_t g_meta_com_type = META_UNKNOWN_COM;
u32 g_meta_com_id = 0;
boot_reason_t g_boot_reason;
ulong g_boot_time;
u32 g_ddr_reserve_enable = 0;
u32 g_ddr_reserve_success = 0;
u32 g_ddr_reserve_ready = 0;
u32 g_smc_boot_opt;
u32 g_lk_boot_opt;
u32 g_kernel_boot_opt;
u32 g_emmc_ocr;
u32 g_emmc_cid[4];
u32 g_emmc_raw_csd[4];

extern unsigned int part_num;
extern part_hdr_t   part_info[PART_MAX_NUM];
extern bool ram_console_is_abnormal_boot(void);

#if defined (MTK_KERNEL_POWER_OFF_CHARGING)
kal_bool kpoc_flag = false;
#endif

#if CFG_BOOT_ARGUMENT
#define bootarg g_dram_buf->bootarg
#endif

#if CFG_USB_AUTO_DETECT
bool g_usbdl_flag;
#endif

/*============================================================================*/
/* platform_core_handler                                                      */
/*============================================================================*/
#define NR_CPUS 8
void platform_core_handler(void)
{
#if CFG_LOAD_SLT_AARCH64_KERNEL
    if (0 == aarch64_slt_done())
    {

        print("\n%s in platform_core_handler\n", MOD);
	return ;
    }
#endif
}

/*============================================================================*/
/* EXTERNAL FUNCTIONS                                                         */
/*============================================================================*/
/*mmc_switch_pll is workaround function
  mt8168 should gate clock when switching emmc/sdcard pll.
  */
static void mmc_config_pll(void)
{
	u32 clk_sts, clk_sel;

	/*set msdc0 source clock gate off*/
	DRV_SetReg32(MODULE_SW_CG3_SET, MSDC0_SRC_CLK_CG);
	/* switch HCLK to SYSPLL1_D2@273Mhz */
	clk_sel = DRV_Reg32(CLK_CFG_2);
	clk_sel &= ~CLK_MSDC0_HCLK_SEL;
	clk_sel |= (0x1 << 16);
	DRV_WriteReg32(CLK_CFG_2, clk_sel);
	/* switch source clock to MSDCPLL@400Mhz */
	clk_sel = DRV_Reg32(CLK_CFG_3);
	clk_sel &= ~CLK_MSDC0_SRC_SEL;
	clk_sel |= (0x1 << 0);
	DRV_WriteReg32(CLK_CFG_3, clk_sel);
	clk_sts = DRV_Reg32(CLKSTA_REG);
	while (clk_sts & (CHG_MSDC50_0_HCLK | CHG_MSDC50_0)) {
		clk_sts = DRV_Reg32(CLKSTA_REG);
		print("%s MSDC0 HCLK/SRC are changing! clk_sts: 0x%x\n",
			__func__, clk_sts);
	}
	/*clr msdc0 source clock gate off*/
	DRV_SetReg32(MODULE_SW_CG3_CLR, MSDC0_SRC_CLK_CG);

	/*set msdc1 source clock gate off*/
	DRV_SetReg32(MODULE_SW_CG3_SET, MSDC1_SRC_CLK_CG);
	/* switch msdc1 source clock to MSDCPLL_D2@200Mhz */
	clk_sel = DRV_Reg32(CLK_CFG_3);
	clk_sel &= ~CLK_MSDC1_SRC_SEL;
	clk_sel |= (0x1 << 16);
	DRV_WriteReg32(CLK_CFG_3, clk_sel);
	clk_sts = DRV_Reg32(CLKSTA_REG);
	while (clk_sts & CHG_MSDC30_1) {
		clk_sts = DRV_Reg32(CLKSTA_REG);
		print("%s MSDC1 SRC are changing! clk_sts: 0x%x\n",
			__func__, clk_sts);
	}
	/*clr msdc1 source clock gate off*/
	DRV_SetReg32(MODULE_SW_CG3_CLR, MSDC1_SRC_CLK_CG);
}

static u32 boot_device_init(void)
{
#if !CFG_FPGA_PLATFORM
	/* FPGA platform cannot access top ckgen registers */
	mmc_config_pll();
#endif
    #if (CFG_BOOT_DEV == BOOTDEV_SDMMC)
    return (u32)mmc_init_device();
    #else
    return (u32)nand_init_device();
    #endif
}

void platform_set_chrg_cur(int ma)
{
    //remove empty function to save space hw_set_cc(ma);
}

int usb_accessory_in(void)
{
#if !CFG_FPGA_PLATFORM
    int exist = 0;

    if (PMIC_CHRDET_EXIST == pmic_IsUsbCableIn()) {
        exist = 1;
        #if !CFG_USBIF_COMPLIANCE
        /* enable charging current as early as possible to avoid can't enter
         * following battery charging flow when low battery
         */
        platform_set_chrg_cur(450);
        #endif
    }
    return exist;
#else
    return 1;
#endif
}

extern bool is_uart_cable_inserted(void);

int usb_cable_in(void)
{
#if !CFG_FPGA_PLATFORM
    int exist = 0;
    CHARGER_TYPE ret;

    if ((g_boot_reason == BR_USB) || usb_accessory_in()) {
        ret = mt_charger_type_detection();
        if (ret == STANDARD_HOST || ret == CHARGING_HOST) {
            print("\n%s USB cable in\n", MOD);
            mt_usb_phy_poweron();
            mt_usb_phy_savecurrent();

            exist = 1;
        } else if (ret == NONSTANDARD_CHARGER || ret == STANDARD_CHARGER) {
            #if CFG_USBIF_COMPLIANCE
            platform_set_chrg_cur(450);
            #endif
        }
    }

    return exist;
#else
    print("\n%s USB cable in\n", MOD);
    mt_usb_phy_poweron();
    mt_usb_phy_savecurrent();

    return 1;
#endif
}

#if CFG_FPGA_PLATFORM
void show_tx(void)
{
	UINT8 var;
	USBPHY_I2C_READ8(0x6E, &var);
	UINT8 var2 = (var >> 3) & ~0xFE;
	print("[USB]addr: 0x6E (TX), value: %x - %x\n", var, var2);
}

void store_tx(UINT8 value)
{
	UINT8 var;
	UINT8 var2;
	USBPHY_I2C_READ8(0x6E, &var);

	if (value == 0) {
		var2 = var & ~(1 << 3);
	} else {
		var2 = var | (1 << 3);
	}

	USBPHY_I2C_WRITE8(0x6E, var2);
	USBPHY_I2C_READ8(0x6E, &var);
	var2 = (var >> 3) & ~0xFE;

	print("[USB]addr: 0x6E TX [AFTER WRITE], value after: %x - %x\n", var, var2);
}

void show_rx(void)
{
	UINT8 var;
	USBPHY_I2C_READ8(0x77, &var);
	UINT8 var2 = (var >> 7) & ~0xFE;
	print("[USB]addr: 0x77 (RX) [AFTER WRITE], value after: %x - %x\n", var, var2);
}

void test_uart(void)
{
	int i=0;
	UINT8 val = 0;
	for (i=0; i<1000; i++)
	{
			show_tx();
            mdelay(300);
			if (val) {
				val = 0;
			}
			else {
				val = 1;
			}
			store_tx(val);
			show_rx();
            mdelay(1000);
	}
}
#endif

void set_to_usb_mode(void)
{
		UINT8 var;
#if !CFG_FPGA_PLATFORM
		/* Turn on USB MCU Bus Clock */
		var = READ_REG(PERI_GLOBALCON_PDN0_SET);
		print("\n[USB]USB bus clock: 0x008, value: %x\n", var);
		USB_CLR_BIT(USB0_PDN, PERI_GLOBALCON_PDN0_SET);
		var = READ_REG(PERI_GLOBALCON_PDN0_SET);
		print("\n[USB]USB bus clock: 0x008, value after: %x\n", var);

		/* Switch from BC1.1 mode to USB mode */
		var = USBPHY_READ8(0x1A);
		print("\n[USB]addr: 0x1A, value: %x\n", var);
		USBPHY_WRITE8(0x1A, var & 0x7f);
		print("\n[USB]addr: 0x1A, value after: %x\n", USBPHY_READ8(0x1A));

		/* Set RG_UART_EN to 0 */
		var = USBPHY_READ8(0x6E);
		print("\n[USB]addr: 0x6E, value: %x\n", var);
		USBPHY_WRITE8(0x6E, var & ~0x01);
		print("\n[USB]addr: 0x6E, value after: %x\n", USBPHY_READ8(0x6E));

		/* Set RG_USB20_DM_100K_EN to 0 */
		var = USBPHY_READ8(0x22);
		print("\n[USB]addr: 0x22, value: %x\n", var);
		USBPHY_WRITE8(0x22, var & ~0x02);
		print("\n[USB]addr: 0x22, value after: %x\n", USBPHY_READ8(0x22));
#else
		/* Set RG_UART_EN to 0 */
		USBPHY_I2C_READ8(0x6E, &var);
		print("\n[USB]addr: 0x6E, value: %x\n", var);
		USBPHY_I2C_WRITE8(0x6E, var & ~0x01);
		USBPHY_I2C_READ8(0x6E, &var);
		print("\n[USB]addr: 0x6E, value after: %x\n", var);

		/* Set RG_USB20_DM_100K_EN to 0 */
		USBPHY_I2C_READ8(0x22, &var);
		print("\n[USB]addr: 0x22, value: %x\n", var);
		USBPHY_I2C_WRITE8(0x22, var & ~0x02);
		USBPHY_I2C_READ8(0x22, &var);
		print("\n[USB]addr: 0x22, value after: %x\n", var);
#endif
		var = READ_REG(UART1_BASE + 0x90);
		print("\n[USB]addr: 0x11002090 (UART1), value: %x\n", var);
		WRITE_REG(var & ~0x01, UART1_BASE + 0x90);
		print("\n[USB]addr: 0x11002090 (UART1), value after: %x\n", READ_REG(UART1_BASE + 0x90));
}

void set_to_uart_mode(void)
{
		UINT8 var;
#if !CFG_FPGA_PLATFORM
		/* Turn on USB MCU Bus Clock */
		#if 0
		var = READ_REG(PERI_GLOBALCON_PDN0_SET);
		print("\n[USB]USB bus clock: 0x008, value: %x\n", var);
		USB_CLR_BIT(USB0_PDN, PERI_GLOBALCON_PDN0_SET);
		var = READ_REG(PERI_GLOBALCON_PDN0_SET);
		print("\n[USB]USB bus clock: 0x008, value after: %x\n", var);
    #endif
		/* Switch from BC1.1 mode to USB mode */
		var = USBPHY_READ8(0x1A);
		print("\n[USB]addr: 0x1A, value: %x\n", var);
		USBPHY_WRITE8(0x1A, var & 0x7f);
		print("\n[USB]addr: 0x1A, value after: %x\n", USBPHY_READ8(0x1A));

		/* Set ru_uart_mode to 2'b01 */
		var = USBPHY_READ8(0x6B);
		print("\n[USB]addr: 0x6B, value: %x\n", var);
		USBPHY_WRITE8(0x6B, var | 0x5C);
		print("\n[USB]addr: 0x6B, value after: %x\n", USBPHY_READ8(0x6B));

		/* Set RG_UART_EN to 1 */
		var = USBPHY_READ8(0x6E);
		print("\n[USB]addr: 0x6E, value: %x\n", var);
		USBPHY_WRITE8(0x6E, var | 0x07);
		print("\n[USB]addr: 0x6E, value after: %x\n", USBPHY_READ8(0x6E));

		/* Set RG_USB20_DM_100K_EN to 1 */
		var = USBPHY_READ8(0x22);
		print("\n[USB]addr: 0x22, value: %x\n", var);
		USBPHY_WRITE8(0x22, var | 0x02);
		print("\n[USB]addr: 0x22, value after: %x\n", USBPHY_READ8(0x22));

		/* Set RG_SUSPENDM to 1 */
		var = USBPHY_READ8(0x68);
		print("\n[USB]addr: 0x68, value: %x\n", var);
		USBPHY_WRITE8(0x68, var | 0x08);
		print("\n[USB]addr: 0x68, value after: %x\n", USBPHY_READ8(0x68));

		/* force suspendm = 1 */
		var = USBPHY_READ8(0x6A);
		print("\n[USB]addr: 0x6A, value: %x\n", var);
		USBPHY_WRITE8(0x6A, var | 0x04);
		print("\n[USB]addr: 0x6A, value after: %x\n", USBPHY_READ8(0x6A));
#else
		/* Set ru_uart_mode to 2'b01 */
		USBPHY_I2C_READ8(0x6B, &var);
		print("\n[USB]addr: 0x6B, value: %x\n", var);
		USBPHY_I2C_WRITE8(0x6B, var | 0x7C);
		USBPHY_I2C_READ8(0x6B, &var);
		print("\n[USB]addr: 0x6B, value after: %x\n", var);

		/* Set RG_UART_EN to 1 */
		USBPHY_I2C_READ8(0x6E, &var);
		print("\n[USB]addr: 0x6E, value: %x\n", var);
		USBPHY_I2C_WRITE8(0x6E, var | 0x07);
		USBPHY_I2C_READ8(0x6E, &var);
		print("\n[USB]addr: 0x6E, value after: %x\n", var);

		/* Set RG_USB20_DM_100K_EN to 1 */
		USBPHY_I2C_READ8(0x22, &var);
		print("\n[USB]addr: 0x22, value: %x\n", var);
		USBPHY_I2C_WRITE8(0x22, var | 0x02);
		USBPHY_I2C_READ8(0x22, &var);
		print("\n[USB]addr: 0x22, value after: %x\n", var);
#endif
		mdelay(100);

        var = DRV_Reg32(UART0_BASE+0xB0);
		print("\n[USB]addr: 0x110030B0 (UART2), value: %x\n", var);
		DRV_WriteReg32(UART0_BASE+0xB0, 0x0001);
		print("\n[USB]addr: 0x110030B0 (UART2), value after: %x\n", DRV_Reg32(UART1_BASE+0xB0));
}

extern U32 PMIC_VUSB_EN(void);

void platform_vusb_on(void)
{
#if !CFG_FPGA_PLATFORM
    U32 ret=0;

	ret = PMIC_VUSB_EN();

    if (ret == 0)
		print("[platform_vusb_on] VUSB33 is on\n");
    else
		print("[platform_vusb_on] Failed to turn on VUSB33!\n");
#endif
    return;
}

#if CFG_BOOT_ARGUMENT_BY_ATAG
void platform_set_boot_args_by_atag(unsigned *ptr)
{
    int i=0, j=0;
    u32 dram_rank_size[4];
    boot_tag *tags;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_boot_reason);
    tags->hdr.tag = BOOT_TAG_BOOT_REASON;
    tags->u.boot_reason.boot_reason = g_boot_reason;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_boot_mode);
    tags->hdr.tag = BOOT_TAG_BOOT_MODE;
    tags->u.boot_mode.boot_mode = g_boot_mode;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_meta_com);
    tags->hdr.tag = BOOT_TAG_META_COM;
    tags->u.meta_com.meta_com_type = (u32)g_meta_com_type;
    tags->u.meta_com.meta_com_id = g_meta_com_id;
    tags->u.meta_com.meta_uart_port = CFG_UART_META;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_log_com);
    tags->hdr.tag = BOOT_TAG_LOG_COM;
    tags->u.log_com.log_port = CFG_UART_LOG;
    tags->u.log_com.log_baudrate = CFG_LOG_BAUDRATE;
    tags->u.log_com.log_enable = (u8)log_status();
    tags->u.log_com.log_dynamic_switch = (u8)get_log_switch();
    ptr += tags->hdr.size;

    //---- DRAM tags ----
	mblock_set_mem_tag(&ptr);
    //----------------
    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_md_info);
    tags->hdr.tag = BOOT_TAG_MD_INFO;
    for(i=0;i<4;i++)
    tags->u.md_info.md_type[i] = bootarg.md_type[i];
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_boot_time);
    tags->hdr.tag = BOOT_TAG_BOOT_TIME;
    tags->u.boot_time.boot_time = get_timer(g_boot_time);;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_da_info);
    tags->hdr.tag = BOOT_TAG_DA_INFO;
    tags->u.da_info.da_info.addr = bootarg.da_info.addr;
    tags->u.da_info.da_info.arg1 = bootarg.da_info.arg1;
    tags->u.da_info.da_info.arg2 = bootarg.da_info.arg2;
    tags->u.da_info.da_info.len  = bootarg.da_info.len;
    tags->u.da_info.da_info.sig_len = bootarg.da_info.sig_len;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_sec_info);
    tags->hdr.tag = BOOT_TAG_SEC_INFO;
    tags->u.sec_info.sec_limit.magic_num = bootarg.sec_limit.magic_num;
    tags->u.sec_info.sec_limit.forbid_mode = bootarg.sec_limit.forbid_mode;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_part_num);
    tags->hdr.tag = BOOT_TAG_PART_NUM;
    tags->u.part_num.part_num = g_dram_buf->part_num;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_part_info);
    tags->hdr.tag = BOOT_TAG_PART_INFO;
    tags->u.part_info.part_info = g_dram_buf->part_info;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_eflag);
    tags->hdr.tag = BOOT_TAG_EFLAG;
    tags->u.eflag.e_flag = sp_check_platform();
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_ddr_reserve);
    tags->hdr.tag = BOOT_TAG_DDR_RESERVE;
    tags->u.ddr_reserve.ddr_reserve_enable = g_ddr_reserve_enable;
    tags->u.ddr_reserve.ddr_reserve_success = g_ddr_reserve_success;
    tags->u.ddr_reserve.ddr_reserve_ready = g_ddr_reserve_ready;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size  = boot_tag_size(boot_tag_dram_buf);
    tags->hdr.tag   = BOOT_TAG_DRAM_BUF;
    tags->u.dram_buf.dram_buf_size = sizeof(dram_buf_t);
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size  = boot_tag_size(boot_tag_boot_opt);
    tags->hdr.tag   = BOOT_TAG_BOOT_OPT;
    tags->u.boot_opt.smc_boot_opt = g_smc_boot_opt;
    tags->u.boot_opt.lk_boot_opt= g_lk_boot_opt;
    tags->u.boot_opt.kernel_boot_opt = g_kernel_boot_opt;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_sram_info);
    tags->hdr.tag  = BOOT_TAG_SRAM_INFO;
    tags->u.sram_info.non_secure_sram_addr= CFG_NON_SECURE_SRAM_ADDR;
    tags->u.sram_info.non_secure_sram_size = CFG_NON_SECURE_SRAM_SIZE;
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_lastpc);
    tags->hdr.tag  = BOOT_TAG_LASTPC;
    memcpy(tags->u.lastpc.lastpc, &bootarg.lastpc, sizeof(tags->u.lastpc.lastpc));
    ptr += tags->hdr.size;

    tags = (boot_tag *)ptr;
    tags->hdr.size = boot_tag_size(boot_tag_plat_dbg_info);
    tags->hdr.tag  = BOOT_TAG_PLAT_DBG_INFO;
    tags->u.plat_dbg_info.info_max = INFO_TYPE_MAX;
    for (i = 0;i < INFO_TYPE_MAX; i++) {
#ifdef CFG_PLAT_DBG_INFO_LIB
        unsigned int key = get_dbg_info_key(i);
        tags->u.plat_dbg_info.info[i].key = key;
        tags->u.plat_dbg_info.info[i].base = get_dbg_info_base(key);
        tags->u.plat_dbg_info.info[i].size = get_dbg_info_size(key);
#else
        tags->u.plat_dbg_info.info[i].key = 0;
        tags->u.plat_dbg_info.info_base[i] = 0;
        tags->u.plat_dbg_info.info_size[i] = 0;
#endif
    }
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_pl_version);
    tags->hdr.tag  = BOOT_TAG_PL_VERSION;
    memcpy(tags->u.pl_version.pl_version, PL_VERSION, sizeof(tags->u.pl_version.pl_version));
    ptr += tags->hdr.size;
    print("PL_VERSION = %s \n", tags->u.pl_version.pl_version);

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_chr_info);
    tags->hdr.tag  = BOOT_TAG_CHR_INFO;
    tags->u.chr_info.charger_type = mt_charger_type_detection();
    ptr += tags->hdr.size;

    tags = ptr;
    tags->hdr.size = boot_tag_size(boot_tag_emmc_info);
    tags->hdr.tag  = BOOT_TAG_EMMC_INFO;
    tags->u.emmc_info.emmc_ocr = g_emmc_ocr;
    memcpy(tags->u.emmc_info.emmc_cid, g_emmc_cid, sizeof(tags->u.emmc_info.emmc_cid));
    memcpy(tags->u.emmc_info.emmc_raw_csd, g_emmc_raw_csd, sizeof(tags->u.emmc_info.emmc_raw_csd));
    ptr += tags->hdr.size;
    print("emmc ocr = 0x%x\n", tags->u.emmc_info.emmc_ocr);
    print("emmc cid: 0x%x 0x%x 0x%x 0x%x\n",
		    tags->u.emmc_info.emmc_cid[0],
		    tags->u.emmc_info.emmc_cid[1],
		    tags->u.emmc_info.emmc_cid[2],
		    tags->u.emmc_info.emmc_cid[3]);
    print("emmc csd: 0x%x 0x%x 0x%x 0x%x\n",
		    tags->u.emmc_info.emmc_raw_csd[0],
		    tags->u.emmc_info.emmc_raw_csd[1],
		    tags->u.emmc_info.emmc_raw_csd[2],
		    tags->u.emmc_info.emmc_raw_csd[3]);

    {
        u32 ram_console_sram_addr = 0;
        u32 ram_console_sram_size = 0;
        tags = (boot_tag *)ptr;
        tags->hdr.size = boot_tag_size(boot_tag_ram_console_info);
        tags->hdr.tag = BOOT_TAG_RAM_CONSOLE_INFO;
        ram_console_sram_addr_size(&ram_console_sram_addr, &ram_console_sram_size);
        tags->u.ram_console_info.sram_addr = ram_console_sram_addr;
        tags->u.ram_console_info.sram_size = ram_console_sram_size;
        tags->u.ram_console_info.def_type = ram_console_def_memory();
        tags->u.ram_console_info.memory_info_offset = ram_console_memory_info_offset();
        ptr += tags->hdr.size;
    }

    tags = (boot_tag *)ptr;
    tags->hdr.size = boot_tag_size(boot_tag_is_abnormal_boot);
    tags->hdr.tag = BOOT_TAG_IS_ABNORMAL_BOOT;
    tags->u.is_abnormal_boot.is_abnormal_boot = ram_console_is_abnormal_boot();
    ptr += tags->hdr.size;

    /* END */
    *ptr++ = 0;
    *ptr++ = 0;
}

void platform_dump_boot_atag()
{
    int i=0, j=0;
    boot_tag *tags;
    for (tags = &(g_dram_buf->boottag); tags->hdr.size; tags = boot_tag_next(tags))
    {
        switch(tags->hdr.tag)
        {
            case BOOT_TAG_BOOT_REASON:
                print("BOOT_REASON: %d\n", tags->u.boot_reason.boot_reason);
                break;
            case BOOT_TAG_BOOT_MODE:
                print("BOOT_MODE: %d\n", tags->u.boot_mode.boot_mode);
                break;
            case BOOT_TAG_META_COM:
                print("META_COM TYPE: %d\n", tags->u.meta_com.meta_com_type);
                print("META_COM ID: %d\n", tags->u.meta_com.meta_com_id);
                print("META_COM PORT: %d\n", tags->u.meta_com.meta_uart_port);
                break;
            case BOOT_TAG_LOG_COM:
                print("LOG_COM PORT: %d\n", tags->u.log_com.log_port);
                print("LOG_COM BAUD: %d\n", tags->u.log_com.log_baudrate);
                print("LOG_COM EN: %d\n", tags->u.log_com.log_enable);
                print("LOG_COM SWITCH: %d\n", tags->u.log_com.log_dynamic_switch);
                break;
            case BOOT_TAG_MEM:
                print("MEM_NUM: %d\n", tags->u.mem.dram_rank_num);
                for (i = 0; i < tags->u.mem.dram_rank_num; i++)
                    print("MEM_SIZE: 0x%x\n", tags->u.mem.dram_rank_size[i]);

                print("mblock num: 0x%x\n", tags->u.mem.mblock_info.mblock_num);
                for(i=0;i<4;i++) {
                    print("mblock start: 0x%llx\n", tags->u.mem.mblock_info.mblock[i].start);
                    print("mblock size: 0x%llx\n", tags->u.mem.mblock_info.mblock[i].size);
                    print("mblock rank: 0x%x\n", tags->u.mem.mblock_info.mblock[i].rank);
                }

                print("orig_dram num: 0x%x\n", tags->u.mem.orig_dram_info.rank_num);
                for(i=0;i<4;i++) {
                    print("orig_dram start: 0x%llx\n", tags->u.mem.orig_dram_info.rank_info[i].start);
                    print("orig_dram size: 0x%llx\n", tags->u.mem.orig_dram_info.rank_info[i].size);
                }

                print("lca start: 0x%llx\n", tags->u.mem.lca_reserved_mem.start);
                print("lca size: 0x%llx\n", tags->u.mem.lca_reserved_mem.size);
                print("tee start: 0x%llx\n", tags->u.mem.tee_reserved_mem.start);
                print("tee size: 0x%llx\n", tags->u.mem.tee_reserved_mem.size);
                break;

            case BOOT_TAG_MD_INFO:
                for(i=0;i<4;i++)
                    print("MD_INFO: 0x%x\n", tags->u.md_info.md_type[i]);
                break;
            case BOOT_TAG_BOOT_TIME:
                print("BOOT_TIME: %d\n", tags->u.boot_time.boot_time);
                break;
            case BOOT_TAG_DA_INFO:
                print("DA_INFO: 0x%x\n", tags->u.da_info.da_info.addr);
                print("DA_INFO: 0x%x\n", tags->u.da_info.da_info.arg1);
                print("DA_INFO: 0x%x\n", tags->u.da_info.da_info.arg2);
                print("DA_INFO: 0x%x\n", tags->u.da_info.da_info.len);
                print("DA_INFO: 0x%x\n", tags->u.da_info.da_info.sig_len);
                break;
            case BOOT_TAG_SEC_INFO:
                print("SEC_INFO: 0x%x\n", tags->u.sec_info.sec_limit.magic_num);
                print("SEC_INFO: 0x%x\n", tags->u.sec_info.sec_limit.forbid_mode);
                break;
            case BOOT_TAG_PART_NUM:
                print("PART_NUM: %d\n", tags->u.part_num.part_num);
                break;
            case BOOT_TAG_PART_INFO:
                print("PART_INFO: 0x%x\n", tags->u.part_info.part_info);
                break;
            case BOOT_TAG_EFLAG:
                print("EFLAG: %d\n", tags->u.eflag.e_flag);
                break;
            case BOOT_TAG_DDR_RESERVE:
                print("DDR_RESERVE: %d\n", tags->u.ddr_reserve.ddr_reserve_enable);
                print("DDR_RESERVE: %d\n", tags->u.ddr_reserve.ddr_reserve_success);
                print("DDR_RESERVE: %d\n", tags->u.ddr_reserve.ddr_reserve_ready);
                break;
            case BOOT_TAG_DRAM_BUF:
                print("DRAM_BUF: %d\n", tags->u.dram_buf.dram_buf_size);
                break;
            case BOOT_TAG_VCORE_DVFS:
                print("DVFS: %d\n", tags->u.vcore_dvfs.vcore_dvfs_info.pll_setting_num);
                print("DVFS: %d\n", tags->u.vcore_dvfs.vcore_dvfs_info.freq_setting_num);
                print("DVFS: 0x%x\n", tags->u.vcore_dvfs.vcore_dvfs_info.low_freq_pll_setting_addr);
                print("DVFS: 0x%x\n", tags->u.vcore_dvfs.vcore_dvfs_info.low_freq_cha_setting_addr);
                print("DVFS: 0x%x\n", tags->u.vcore_dvfs.vcore_dvfs_info.low_freq_chb_setting_addr);
                print("DVFS: 0x%x\n", tags->u.vcore_dvfs.vcore_dvfs_info.high_freq_pll_setting_addr);
                print("DVFS: 0x%x\n", tags->u.vcore_dvfs.vcore_dvfs_info.high_freq_cha_setting_addr);
                print("DVFS: 0x%x\n", tags->u.vcore_dvfs.vcore_dvfs_info.high_freq_chb_setting_addr);
                break;
            case BOOT_TAG_BOOT_OPT:
                print("SMC: 0x%x\n", tags->u.boot_opt.smc_boot_opt);
                print("SMC: 0x%x\n", tags->u.boot_opt.lk_boot_opt);
                print("SMC: 0x%x\n", tags->u.boot_opt.kernel_boot_opt);
                break;
            case BOOT_TAG_SRAM_INFO:
                print("SRAM satrt: 0x%x\n", tags->u.sram_info.non_secure_sram_addr);
                print("SRAM size: 0x%x\n", tags->u.sram_info.non_secure_sram_size);
                break;
            case BOOT_TAG_LASTPC:
                break;
            case BOOT_TAG_PLAT_DBG_INFO:
                for (i = 0; i < INFO_TYPE_MAX; i++) {
                    print("PLAT_DBG_INFO key: 0x%x\n", tags->u.plat_dbg_info.info[i].key);
                    print("PLAT_DBG_INFO base: 0x%x\n", tags->u.plat_dbg_info.info[i].base);
                    print("PLAT_DBG_INFO size: 0x%x\n", tags->u.plat_dbg_info.info[i].size);
                }
            break;
        }
    }
}
#endif

void platform_parse_bootopt(void)
{
    u8 *bootimg_opt;
    u8 bootimg_opt_buf[10];

    u8 bootimg_opt_str_array[10][5];
    u32 i;

    i = 0;
    /* offset of boot option in boot image header 0x40 */
    bootimg_opt = CFG_BOOTIMG_HEADER_MEMADDR + 0x40;
    memset(bootimg_opt_str_array,'\0',50);
    memcpy(bootimg_opt_str_array[BOOT_OPT_64S3],STR_BOOT_OPT_64S3,4);
    memcpy(bootimg_opt_str_array[BOOT_OPT_64S1],STR_BOOT_OPT_64S1,4);
    memcpy(bootimg_opt_str_array[BOOT_OPT_32S3],STR_BOOT_OPT_32S3,4);
    memcpy(bootimg_opt_str_array[BOOT_OPT_32S1],STR_BOOT_OPT_32S1,4);
    memcpy(bootimg_opt_str_array[BOOT_OPT_64N2],STR_BOOT_OPT_64N2,4);
    memcpy(bootimg_opt_str_array[BOOT_OPT_64N1],STR_BOOT_OPT_64N1,4);
    memcpy(bootimg_opt_str_array[BOOT_OPT_32N2],STR_BOOT_OPT_32N2,4);
    memcpy(bootimg_opt_str_array[BOOT_OPT_32N1],STR_BOOT_OPT_32N1,4);
/*
#define  STR_BOOT_OPT_64S3 "64S3"
#define  STR_BOOT_OPT_64S1 "64S1"
#define  STR_BOOT_OPT_32S3 "32S3"
#define  STR_BOOT_OPT_32S1 "32S1"
#define  STR_BOOT_OPT_64N2 "64N2"
#define  STR_BOOT_OPT_64N1 "64N1"
#define  STR_BOOT_OPT_32N2 "32N2"
#define  STR_BOOT_OPT_32N1 "32N1"

typedef enum {
    BOOT_OPT_64S3 = 0,
    BOOT_OPT_64S1,
    BOOT_OPT_32S3,
    BOOT_OPT_32S1,
    BOOT_OPT_64N2,
    BOOT_OPT_64N1,
    BOOT_OPT_32N2,
    BOOT_OPT_32N1,
    BOOT_OPT_UNKNOWN
} boot_option_t;

u32 g_smc_boot_opt;
u32 g_lk_boot_opt;
u32 g_kernel_boot_opt;

*/
    //bootopt=64S3,32S1,32S1
    //"bootopt="
    memset(bootimg_opt_buf,'\0',10);
    memcpy(bootimg_opt_buf, bootimg_opt, 8);
    bootimg_opt = bootimg_opt + 8;
    if ( 0 == strcmp(bootimg_opt_buf, "bootopt="))
    {
        //"SMC option"
        memset(bootimg_opt_buf,'\0',10);
        memcpy(bootimg_opt_buf, bootimg_opt, 4);

        for(i = 0; i < BOOT_OPT_UNKNOWN; i++)
        {
            if ( 0 == strcmp(bootimg_opt_str_array[i], bootimg_opt_buf))
            {
                g_smc_boot_opt = i;
                print("%s,%s,boot_opt=0x%x\n", MOD, bootimg_opt_str_array[g_smc_boot_opt], g_smc_boot_opt);
                break;
            }
        }
        //"LK option"
        bootimg_opt = bootimg_opt + 5;
        memset(bootimg_opt_buf,'\0',10);
        memcpy(bootimg_opt_buf, bootimg_opt, 4);

        for(i = 0; i < BOOT_OPT_UNKNOWN; i++)
        {
            if ( 0 == strcmp(bootimg_opt_str_array[i], bootimg_opt_buf))
            {
                g_lk_boot_opt = i;
                print("%s,%s,boot_opt=0x%x\n", MOD, bootimg_opt_str_array[g_lk_boot_opt], g_lk_boot_opt);
                break;
            }
        }
        //"Kernel option"
        bootimg_opt = bootimg_opt + 5;
        memset(bootimg_opt_buf,'\0',10);
        memcpy(bootimg_opt_buf, bootimg_opt, 4);

        for(i = 0; i < BOOT_OPT_UNKNOWN; i++)
        {
            if ( 0 == strcmp(bootimg_opt_str_array[i], bootimg_opt_buf))
            {
                g_kernel_boot_opt = i;
                print("%s,%s,boot_opt=0x%x\n", MOD, bootimg_opt_str_array[g_kernel_boot_opt], g_kernel_boot_opt);
                break;
            }
        }
    }
    else    // if no specific bootopt
    {
        //bootopt=64S3,32S1,32S1
        g_smc_boot_opt = BOOT_OPT_64S3;
        g_lk_boot_opt = BOOT_OPT_32N2;
        g_kernel_boot_opt = BOOT_OPT_32N2;
    }
}

#define MP0_DBG_CTRL 0x10200404
#define MCUSYS_DBG_MON_SEL_A 0x10200590
#define MCUSYS_DBG_MON 0x10200594
#define MCU_ALL_PWR_ON_CTRL 0x10200b58

void platform_set_boot_args(void)
{
    unsigned int i, j, val;

    *(volatile unsigned int *)MCU_ALL_PWR_ON_CTRL |= 1;
    *(volatile unsigned int *)MCUSYS_DBG_MON_SEL_A = 1;
    val = *(volatile unsigned int *)MP0_DBG_CTRL;

    for (i=0; i<4; i++)
        for (j=0; j<8; j++) {
            val &= ~0xff;
            val |= i << 4 | j;
            *(volatile unsigned int *)MP0_DBG_CTRL = val;
            bootarg.lastpc[i][j] = *(volatile unsigned int *)MCUSYS_DBG_MON;
            print("lastpc[%d][%d] = %x\n", i, j, bootarg.lastpc[i][j]);
        }

    *(volatile unsigned int *)MCU_ALL_PWR_ON_CTRL = 0x7f;
    *(volatile unsigned int *)MCU_ALL_PWR_ON_CTRL = 1;
    *(volatile unsigned int *)MCU_ALL_PWR_ON_CTRL = 0;

#if CFG_ATF_SUPPORT
    {
    u64 sec_addr;
    tee_get_secmem_start(&sec_addr);
    bootarg.tee_reserved_mem.start = sec_addr;
    bootarg.tee_reserved_mem.size = ATF_LOG_BUFFER_SIZE;
    }
#else
    bootarg.tee_reserved_mem.start = 0;
    bootarg.tee_reserved_mem.size = 0;
#endif

#if CFG_BOOT_ARGUMENT_BY_ATAG
    print("\n%s boot to LK by ATAG.\n", MOD, g_boot_reason);
    platform_set_boot_args_by_atag(&(g_dram_buf->boottag)); // set jump addr
    platform_dump_boot_atag();

#elif CFG_BOOT_ARGUMENT && !CFG_BOOT_ARGUMENT_BY_ATAG
    bootarg.magic = BOOT_ARGUMENT_MAGIC;
    bootarg.mode  = g_boot_mode;
    bootarg.e_flag = sp_check_platform();
    bootarg.log_port = CFG_UART_LOG;
    bootarg.log_baudrate = CFG_LOG_BAUDRATE;
    bootarg.log_enable = (u8)log_status();
    bootarg.log_dynamic_switch = (u8)get_log_switch();
#if !CFG_FPGA_PLATFORM
/*In FPGA phase, dram related function should by pass*/
    bootarg.dram_rank_num = bootarg.mblock_info.mblock_num;
    for (i = 0; i < bootarg.mblock_info.mblock_num; i++) {
	    bootarg.dram_rank_size[i] = bootarg.mblock_info.mblock[i].size;
        print("%s, rank[%d].size = 0x%llx\n", __func__, i,
			(unsigned long long) bootarg.dram_rank_size[i]);
    }
#endif
    bootarg.boot_reason = g_boot_reason;
    bootarg.meta_com_type = (u32)g_meta_com_type;
    bootarg.meta_com_id = g_meta_com_id;
    bootarg.meta_uart_port = CFG_UART_META;
    bootarg.boot_time = get_timer(g_boot_time);

    #if 0//#if (CFG_BOOT_DEV == BOOTDEV_NAND)
    bootarg.flash_number = sizeof(gen_FlashTable)/sizeof(flashdev_info);
	  if(bootarg.flash_number > MAX_FLASH)
	  {
	      print("[BOOTDEV NAND]warning !! bootarg.flash_number > MAX_FLASH");
	  }
	  print("\n%s flash number: %d\n", MOD, bootarg.flash_number);
	  memcpy(bootarg.gen_FlashTable,gen_FlashTable,bootarg.flash_number*sizeof(flashdev_info));
	  //memcpy(gen_FlashTable_t,gen_FlashTable,bootarg.flash_number*sizeof(flashdev_info));
    //bootarg.gen_FlashTable = gen_FlashTable_t;//gen_FlashTable_t;

	  //	print("\n%s bootarg->flash_number: %d CHIP_CNT %d size %d sizeof(gen_FlashTable) %d\n", MOD, bootarg->flash_number,CHIP_CNT,sizeof(flashdev_info),sizeof(gen_FlashTable));
	  //	print("\n%s %s %s 0x%x\n", MOD, gen_FlashTable_t[0].devciename, gen_FlashTable_t[1].devciename,bootarg->gen_FlashTable);
	  //	q = (unsigned int* )bootarg->gen_FlashTable;
		//for(k = 0 ; k<(bootarg->flash_number*sizeof(flashdev_info))/4;k++)
		//	print("0x%x, ",*q++);
    #endif

    bootarg.part_num =  g_dram_buf->part_num;
    bootarg.part_info = g_dram_buf->part_info;

    bootarg.ddr_reserve_enable = g_ddr_reserve_enable;
    bootarg.ddr_reserve_success= g_ddr_reserve_success;
    bootarg.ddr_reserve_ready = g_ddr_reserve_ready;
    bootarg.dram_buf_size =  sizeof(dram_buf_t);

    bootarg.smc_boot_opt = g_smc_boot_opt;
    bootarg.lk_boot_opt = g_lk_boot_opt;
    bootarg.kernel_boot_opt = g_kernel_boot_opt;

    bootarg.non_secure_sram_addr = CFG_NON_SECURE_SRAM_ADDR;
    bootarg.non_secure_sram_size = CFG_NON_SECURE_SRAM_SIZE;

    print("%s NON SECURE SRAM ADDR: 0x%x\n", MOD, bootarg.non_secure_sram_addr);
    print("%s NON SECURE SRAM SIZE: 0x%x\n", MOD, bootarg.non_secure_sram_size);
    memcpy(bootarg.pl_version, PL_VERSION, sizeof(bootarg.pl_version));
    print("%s PL_VERSION = %s \n", MOD, bootarg.pl_version);

    bootarg.emmc_ocr = g_emmc_ocr;
    memcpy(bootarg.emmc_cid, g_emmc_cid, sizeof(g_emmc_cid));
    memcpy(bootarg.emmc_raw_csd, g_emmc_raw_csd, sizeof(g_emmc_raw_csd));
#if CFG_WORLD_PHONE_SUPPORT
    print("%s md_type[0] = %d \n", MOD, bootarg.md_type[0]);
    print("%s md_type[1] = %d \n", MOD, bootarg.md_type[1]);
#endif

    print("\n%s boot reason: %d\n", MOD, g_boot_reason);
    print("%s boot mode: %d\n", MOD, g_boot_mode);
    print("%s META COM%d: %d\n", MOD, bootarg.meta_com_id, bootarg.meta_com_type);
    print("%s <0x%x>: 0x%x\n", MOD, &bootarg.e_flag, bootarg.e_flag);
    print("%s boot time: %dms\n", MOD, bootarg.boot_time);
    print("%s DDR reserve mode: enable = %d, success = %d, ready = %d\n", MOD, bootarg.ddr_reserve_enable, bootarg.ddr_reserve_success, bootarg.ddr_reserve_ready);
    print("%s dram_buf_size: 0x%x\n", MOD, bootarg.dram_buf_size);
    print("%s smc_boot_opt: 0x%x\n", MOD, bootarg.smc_boot_opt);
    print("%s lk_boot_opt: 0x%x\n", MOD, bootarg.lk_boot_opt);
    print("%s kernel_boot_opt: 0x%x\n", MOD, bootarg.kernel_boot_opt);
    print("%s tee_reserved_mem: 0x%llx, 0x%llx\n", MOD, bootarg.tee_reserved_mem.start, bootarg.tee_reserved_mem.size);
#endif

}

void platform_set_dl_boot_args(da_info_t *da_info)
{
#if CFG_BOOT_ARGUMENT
    if (da_info->addr != BA_FIELD_BYPASS_MAGIC)
	bootarg.da_info.addr = da_info->addr;

    if (da_info->arg1 != BA_FIELD_BYPASS_MAGIC)
	bootarg.da_info.arg1 = da_info->arg1;

    if (da_info->arg2 != BA_FIELD_BYPASS_MAGIC)
	bootarg.da_info.arg2 = da_info->arg2;

    if (da_info->len != BA_FIELD_BYPASS_MAGIC)
	bootarg.da_info.len = da_info->len;

    if (da_info->sig_len != BA_FIELD_BYPASS_MAGIC)
	bootarg.da_info.sig_len = da_info->sig_len;
#endif

    return;
}

void platform_wdt_all_kick(void)
{
    /* kick watchdog to avoid cpu reset */
    mtk_wdt_restart();
}

void platform_wdt_kick(void)
{
    /* kick hardware watchdog */
    mtk_wdt_restart();
}

#if CFG_DT_MD_DOWNLOAD
void platform_modem_download(void)
{
    print("[%s] modem download...\n", MOD);

    /* Switch to MT6261 USB:
     * GPIO_USB_SW1(USB_SW1)=1		//GPIO115, GPIO48
     * GPIO_USB_SW2(USB_SW2)=0		//GPIO116, GPIO196
     * phone
    #define GPIO_UART_URTS0_PIN         (GPIO115 | 0x80000000)
    #define GPIO_UART_UCTS0_PIN         (GPIO116 | 0x80000000)
     * EVB
    #define GPIO_EXT_USB_SW1         (GPIO48 | 0x80000000)
    #define GPIO_EXT_USB_SW2         (GPIO196 | 0x80000000)
     */
#if defined(GPIO_EXT_USB_SW1)
    mt_set_gpio_mode(GPIO_EXT_USB_SW1, GPIO_EXT_USB_SW1_M_GPIO);
    mt_set_gpio_dir(GPIO_EXT_USB_SW1, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_EXT_USB_SW1, GPIO_OUT_ONE);
#endif

#if defined(GPIO_EXT_USB_SW2)
	mt_set_gpio_mode(GPIO_EXT_USB_SW2, GPIO_EXT_USB_SW2_M_GPIO);
    mt_set_gpio_dir(GPIO_EXT_USB_SW2, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_EXT_USB_SW2, GPIO_OUT_ZERO);
#endif

    /* Press MT6261 DL key to enter download mode
     * GPIO_KPD_KROW1_PIN(GPIO_KCOL0)=0 //GPIO120, GPIO105
     * phone
    #define GPIO_KPD_KROW1_PIN         (GPIO120 | 0x80000000)
     * evb
    #define GPIO_EXT_MD_DL_KEY         (GPIO105 | 0x80000000)
     *
     */
#if defined(GPIO_EXT_MD_DL_KEY)
    mt_set_gpio_mode(GPIO_EXT_MD_DL_KEY, GPIO_EXT_MD_DL_KEY_M_GPIO);
    mt_set_gpio_dir(GPIO_EXT_MD_DL_KEY, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_EXT_MD_DL_KEY, GPIO_OUT_ZERO);
#endif

    /* Bring-up MT6261:
     * GPIO_OTG_DRVVBUS_PIN(GPIO_USB_DRVVBUS)=0,
     * GPIO_52_RST(GPIO_RESETB)=INPUT/NOPULL, GPIO118, GPIO166
     #define GPIO_UART_UTXD3_PIN         (GPIO118 | 0x80000000)
     #define GPIO_EXT_MD_RST         (GPIO166 | 0x80000000)
     * GPIO_RST_KEY(GPIO_PWRKEY)=0->1->0
     #define GPIO_UART_URXD3_PIN         (GPIO117 | 0x80000000)
     * GPIO_PWR_KEY(GPIO_PWRKEY)=1
     #define GPIO_EXT_MD_PWR_KEY         (GPIO167 | 0x80000000)
     */
#if defined(GPIO_EXT_MD_RST)
//    mt_set_gpio_mode(GPIO_OTG_DRVVBUS_PIN, GPIO_OTG_DRVVBUS_PIN_M_GPIO);
    mt_set_gpio_mode(GPIO_EXT_MD_RST, GPIO_EXT_MD_RST_M_GPIO);
#endif

    /* MD DRVVBUS to low */
//    mt_set_gpio_dir(GPIO_OTG_DRVVBUS_PIN, GPIO_DIR_OUT);
//    mt_set_gpio_out(GPIO_OTG_DRVVBUS_PIN, GPIO_OUT_ZERO);
#if defined(GPIO_EXT_MD_PWR_KEY)
    mt_set_gpio_mode(GPIO_EXT_MD_PWR_KEY, GPIO_EXT_MD_PWR_KEY_M_GPIO);
    mt_set_gpio_dir(GPIO_EXT_MD_PWR_KEY, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_EXT_MD_PWR_KEY, GPIO_OUT_ONE); /* default @ reset state */
#endif

#if defined(GPIO_EXT_MD_RST)
    /* MD reset pin: hold to zero */
    mt_set_gpio_pull_enable(GPIO_EXT_MD_RST, GPIO_PULL_DISABLE);
    mt_set_gpio_dir(GPIO_EXT_MD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_EXT_MD_RST, GPIO_OUT_ZERO); /* default @ reset state */
#endif
    mdelay(200);


    /* MD reset pin: released */
#if defined(GPIO_EXT_MD_RST)
    mt_set_gpio_out(GPIO_EXT_MD_RST, GPIO_OUT_ONE);
    mdelay(200);
    mt_set_gpio_dir(GPIO_EXT_MD_RST, GPIO_DIR_IN);
#endif

    print("[%s] AP modem download done\n", MOD);
	//keep kick WDT to avoid HW WDT reset
    while (1) {
        platform_wdt_all_kick();
		mdelay(1000);
    }
}
#endif

#if CFG_USB_AUTO_DETECT
void platform_usbdl_flag_check()
{
    U32 usbdlreg = 0;
    usbdlreg = DRV_Reg32(SRAMROM_USBDL);
     /*Set global variable to record the usbdl flag*/
    if(usbdlreg & USBDL_BIT_EN)
        g_usbdl_flag = 1;
    else
        g_usbdl_flag = 0;
}

void platform_usb_auto_detect_flow()
{

    print("USB DL Flag is %d when enter preloader  \n",g_usbdl_flag);

    /*usb download flag haven't set */
	if(g_usbdl_flag == 0 && g_boot_reason != BR_RTC){
        /*set up usbdl flag*/
        platform_safe_mode(1,CFG_USB_AUTO_DETECT_TIMEOUT_MS);
        print("Preloader going reset and trigger BROM usb auto detectiton!!\n");

        /*WDT by pass powerkey reboot*/
        mtk_arch_reset(1);

	}else{
    /*usb download flag have been set*/
    }

}
#endif


void platform_safe_mode(int en, u32 timeout)
{
#if !CFG_FPGA_PLATFORM

    U32 usbdlreg = 0;

    /* if anything is wrong and caused wdt reset, enter bootrom download mode */
    timeout = !timeout ? USBDL_TIMEOUT_MAX : timeout / 1000;
    timeout <<= 2;
    timeout &= USBDL_TIMEOUT_MASK; /* usbdl timeout cannot exceed max value */

    usbdlreg |= timeout;
    if (en)
	    usbdlreg |= USBDL_BIT_EN;
    else
	    usbdlreg &= ~USBDL_BIT_EN;

    usbdlreg &= ~USBDL_BROM ;
    /*Add magic number for MT6582*/
    usbdlreg |= USBDL_MAGIC;

    DRV_WriteReg32(SRAMROM_USBDL, usbdlreg);

    return;
        return;
#endif

}

#if CFG_EMERGENCY_DL_SUPPORT
void platform_emergency_download(u32 timeout)
{
    /* enter download mode */
    print("%s emergency download mode(timeout: %ds).\n", MOD, timeout / 1000);
    platform_safe_mode(1, timeout);

#if !CFG_FPGA_PLATFORM
    mtk_arch_reset(0); /* don't bypass power key */
#endif

    while(1);
}
#endif



int platform_get_mcp_id(u8 *id, u32 len, u32 *fw_id_len)
{
    int ret = -1;

    memset(id, 0, len);

#if (CFG_BOOT_DEV == BOOTDEV_SDMMC)
    ret = mmc_get_device_id(id, len,fw_id_len);
#else
    ret = nand_get_device_id(id, len);
#endif

    return ret;
}

#if (CFG_PMIC_FULL_RESET)
bool platform_pmic_full_reset_check(void)
{
	pal_log_info("[Reg check 1][0x%x]=0x%x\n", 0x166, upmu_get_reg_value(0x166));
	pal_log_info("[Reg check 1][0x%x]=0x%x\n", 0x22, upmu_get_reg_value(0x22));
	if(upmu_get_reg_value(0x22)){
		pal_log_info("[Reg check][0x%x]=0x%x\n", 0x22, upmu_get_reg_value(0x22));
		pmic_config_interface(0x22, 0, 0xff, 0); //clear status at 0x22
		pal_log_info("[Reg check][0x%x]=0x%x\n", 0x22, upmu_get_reg_value(0x22));
		return true;
	}
	else
		return false;
}
#endif

static boot_reason_t platform_boot_status(void)
{
	int wdt_flag;
	int rtc_flag = -1;
#if !CFG_FPGA_PLATFORM
#if (CFG_PMIC_FULL_RESET)
    bool wdt_flag = platform_pmic_full_reset_check();
#endif

#if defined (MTK_KERNEL_POWER_OFF_CHARGING)
	ulong begin = get_timer(0);
	do  {
		rtc_flag = rtc_boot_check();
		if (rtc_flag == RTC_ALARM) {
			print("%s RTC boot!\n", MOD);
			return BR_RTC;
		} else if (rtc_flag == RTC_REBOOT_RECOVERY) {
			print("%s REBOOT RECOVERY !\n", MOD);
			return BR_REBOOT_RECOVERY;
		}
		if(!kpoc_flag)
			break;
	} while (get_timer(begin) < 1000 && kpoc_flag);
#else
	rtc_flag = rtc_boot_check();
	if (rtc_flag == RTC_ALARM) {
		print("%s RTC boot!\n", MOD);
		return BR_RTC;
	} else if (rtc_flag == RTC_REBOOT_RECOVERY) {
		print("%s REBOOT RECOVERY !\n", MOD);
		return BR_REBOOT_RECOVERY;
	}
#endif
	BOOTING_TIME_PROFILING_LOG("check Boot status-RTC");
#endif
	wdt_flag = mtk_wdt_boot_check();
	switch(wdt_flag) {
		case RE_BOOT_BY_WDT_HW:
			print("%s WDT HW reboot!\n", MOD);
			return BR_WDT_HW;
		case RE_BOOT_BY_THERMAL_DIRECT:
			print("%s WDT THERMAL reboot!\n", MOD);
			return BR_WDT_THERMAL;
		case RE_BOOT_BY_SPM_THERMAL:
			print("%s WDT THERMAL SPM reboot!\n", MOD);
			return BR_WDT_THERMAL_SPM;
		case RE_BOOT_BY_SPM:
			print("%s WDT SPM reboot!\n", MOD);
			return BR_WDT_SPM;
		case WDT_BY_PASS_PWK_REBOOT:
			if (rtc_flag == RTC_KERNEL_PANIC) {
				print("%s WDT SW reboot (KERNEL PANIC)!\n", MOD);
				return BR_KERNEL_PANIC;
			} else if (rtc_flag == RTC_KERNEL_RESTART) {
				print("%s WDT SW reboot (KERNEL RESTART)!\n", MOD);
				return BR_KERNEL_RESTART;
			} else if (rtc_flag == RTC_KERNEL_AEE) {
				print("%s WDT SW reboot (KERNEL AEE)!\n", MOD);
				return BR_WDT_AEE;
			} else {
				print("%s WDT reboot bypass power key!\n", MOD);
				return BR_WDT_BY_PASS_PWK;
			}
		case WDT_NORMAL_REBOOT:
			if (rtc_flag == RTC_REBOOT_BOOTLOADER) {
				print("%s WDT SW reboot (BOOTLOADER)!\n", MOD);
				return BR_REBOOT_BOOTLOADER;
			} else {
				/* ex: reboot charger/kpoc */
				print("%s WDT normal boot!\n", MOD);
				return BR_WDT;
			}
		default:
			/* do nothing, not WDT reboot */
			break;

	}
	BOOTING_TIME_PROFILING_LOG("check Boot status-WDT");

#if (CFG_PMIC_FULL_RESET)
 if(wdt_flag && g_rgu_status == RE_BOOT_REASON_UNKNOW){
	   print("%s WDT reboot bypass power key! PMIC Full Reset.\n", MOD);
	   g_rgu_status = RE_BOOT_BY_WDT_SW;
	   return BR_WDT_BY_PASS_PWK;
   }
#endif

#if !CFG_FPGA_PLATFORM
    /* check power key */
    if (mtk_detect_key(PL_PMIC_PWR_KEY)) {
        print("%s Power key boot!\n", MOD);
        rtc_mark_bypass_pwrkey();
        return BR_POWER_KEY;
    }
	BOOTING_TIME_PROFILING_LOG("check Boot status-PWR key");
#endif



#if !CFG_EVB_PLATFORM
    if (usb_accessory_in()) {
        print("%s USB/charger boot!\n", MOD);
		BOOTING_TIME_PROFILING_LOG("check Boot status-usb_accessory_in");
        return BR_USB;
    }
    //need to unlock rtc PROT
    //check after rtc_boot_check() initial finish.
	if (rtc_2sec_reboot_check()) {
		print("%s 2sec reboot!\n", MOD);
		BOOTING_TIME_PROFILING_LOG("check Boot status-rtc_2sec_reboot_check");
		return BR_2SEC_REBOOT;
	}

    print("%s Unknown boot!\n", MOD);
    pl_power_off();
    /* should nerver be reached */
#endif

    print("%s Power key boot!\n", MOD);

    return BR_POWER_KEY;
}

#if CFG_LOAD_DSP_ROM || CFG_LOAD_MD_ROM
int platform_is_three_g(void)
{
    u32 tmp = sp_check_platform();

    return (tmp & 0x1) ? 0 : 1;
}
#endif

chip_ver_t platform_chip_ver(void)
{
	chip_ver_t sw_ver;
	unsigned int ver;
	//    unsigned int hw_subcode = DRV_Reg32(APHW_SUBCODE);

	ver = DRV_Reg32(APSW_VER);
	if ( 0x0 == ver )
		sw_ver = CHIP_VER_E1;
	else
		sw_ver = CHIP_VER_E2;

	return sw_ver;
}

// ------------------------------------------------
// detect download mode
// ------------------------------------------------

bool platform_com_wait_forever_check(void)
{
#ifdef USBDL_DETECT_VIA_KEY
    /* check download key */
    if (TRUE == mtk_detect_key(COM_WAIT_KEY)) {
        print("%s COM handshake timeout force disable: Key\n", MOD);
        return TRUE;
    }
#endif

#ifdef USBDL_DETECT_VIA_AT_COMMAND
    print("platform_com_wait_forever_check\n");
    /* check SRAMROM_USBDL_TO_DIS */
    if (USBDL_TO_DIS == (INREG32(SRAMROM_USBDL_TO_DIS) & USBDL_TO_DIS)) {
	print("%s COM handshake timeout force disable: AT Cmd\n", MOD);
	CLRREG32(SRAMROM_USBDL_TO_DIS, USBDL_TO_DIS);
	return TRUE;
    }
#endif

    return FALSE;
}

/* NOTICE: need to revise if platform supports >4G memory size*/
u32 platform_memory_size(void)
{
    static u32 mem_size = 0;
    int nr_bank;
    int i;
    int rank_size[4], *rksize = &rank_size[0];
    u32 size = 0;

    if (!mem_size) {
        nr_bank = get_dram_rank_nr();

        get_dram_rank_size(rank_size);

        for (i = 0; i < nr_bank; i++)
            size += (u32)*rksize++;
        mem_size = size;
    }

    return mem_size;
}

void platform_pre_init(void)
{
    u32 pmic_ret=0,i=0;

    /* init timer */
    mtk_timer_init();

    /* init boot time */
    g_boot_time = get_timer(0);


#if !CFG_FPGA_PLATFORM
    /* init pll */
    mt_pll_init();
#endif

    /* init uart baudrate when pll on */
    mtk_uart_init(UART_SRC_CLK_FRQ, CFG_LOG_BAUDRATE);

	BOOTING_TIME_PROFILING_LOG("PLL");
    /*GPIO init*/
    mt_gpio_init();
	BOOTING_TIME_PROFILING_LOG("GPIO");

#if !CFG_FPGA_PLATFORM

    #if (CFG_USB_UART_SWITCH)
	if (is_uart_cable_inserted()) {
		print("\n%s Switch to UART Mode\n", MOD);
		set_to_uart_mode();
	} else {
		print("\n%s Keep stay in USB Mode\n", MOD);
	}
    BOOTING_TIME_PROFILING_LOG("USB SWITCH to UART");
    #endif
#endif

//#if !CFG_FPGA_PLATFORM
    /* init pll post */
//    mt_pll_post_init();
//	BOOTING_TIME_PROFILING_LOG("PLL post");
//#endif

    //retry 3 times for pmic wrapper init
    pwrap_init_preloader();
    BOOTING_TIME_PROFILING_LOG("PWRAP");

    //i2c hw init
    i2c_hw_init();
    BOOTING_TIME_PROFILING_LOG("I2C");

#if !CFG_FPGA_PLATFORM
    pmic_ret = pmic_init();
    BOOTING_TIME_PROFILING_LOG("PMIC");

    //mt_pll_post_init();
    //mt_arm_pll_sel();
    BOOTING_TIME_PROFILING_LOG("PLL POST Init");
#endif

    //enable long press reboot function
    PMIC_enable_long_press_reboot();
    BOOTING_TIME_PROFILING_LOG("Long Pressed Reboot");

    print("%s Init PMIC: %s(%d)\n", MOD, pmic_ret ? "FAIL" : "OK", pmic_ret);

    platform_core_handler();

    print("%s chip_ver[%x]\n", MOD, platform_chip_ver());
}


#ifdef MTK_MT8193_SUPPORT
extern int mt8193_init(void);
#endif


void platform_init(void)
{
    u32 ret, tmp;
    boot_reason_t reason;

    if (clk_buf_init())
	    print("%s: clk_buf_init fail\n", __func__);
#if !CFG_FPGA_PLATFORM
    rtc_init();

    pmic_init_setting();
    pl_battery_init(false);
#endif
    /* check DDR-reserve mode */
    check_ddr_reserve_status();
	BOOTING_TIME_PROFILING_LOG("chk DDR Reserve status");

    /* init watch dog, will enable AP watch dog */
    mtk_wdt_init();
    /*init kpd PMIC mode support*/
    set_kpd_pmic_mode();


#if CFG_MDWDT_DISABLE
    /* no need to disable MD WDT, the code here is for backup reason */

    /* disable MD0 watch dog. */
    DRV_WriteReg32(0x20050000, 0x2200);

    /* disable MD1 watch dog. */
    DRV_WriteReg32(0x30050020, 0x2200);
#endif


#if !CFG_FPGA_PLATFORM/* FIXME */
    g_boot_reason = reason = platform_boot_status();
	BOOTING_TIME_PROFILING_LOG("check Boot status");

    rtc_bbpu_power_on();
    BOOTING_TIME_PROFILING_LOG("rtc_bbpu_power_on");
#else
    g_boot_reason = BR_POWER_KEY;
#endif

    enable_PMIC_kpd_clock();
    BOOTING_TIME_PROFILING_LOG("Enable PMIC Kpd clk");

    /* init device storeage */
    ret = boot_device_init();
	BOOTING_TIME_PROFILING_LOG("Boot dev init");
    print("%s Init Boot Device: %s(%d)\n", MOD, ret ? "FAIL" : "OK", ret);

#if !CFG_FPGA_PLATFORM
    /* init memory */
    mt_mem_init();
	BOOTING_TIME_PROFILING_LOG("mem_init");
#endif

    /*init dram buffer*/
    init_dram_buffer();
	BOOTING_TIME_PROFILING_LOG("Init Dram buf");

    /* switch log buffer to dram */
    //log_buf_ctrl(1);
#if 0 /* FIXME */
    /* enable CA9 share bits for USB(30)/NFI(29)/MSDC(28) modules to access ISRAM */
    tmp = DRV_Reg32(0xC1000200);
    tmp |= ((1<<30)|(1<<29)|(1<<28));
    DRV_WriteReg32 (0xC1000200, tmp);
#endif

#ifdef MTK_MT8193_SUPPORT
	mt8193_init();
#endif
	ram_console_init();
	ram_console_reboot_reason_save(g_rgu_status);
	BOOTING_TIME_PROFILING_LOG("Ram console");

#if CFG_EMERGENCY_DL_SUPPORT
    /* check if to enter emergency download mode */
    /* Move after dram_inital and boot_device_init.
      Use excetution time to remove delay time in mtk_kpd_gpio_set()*/
    if (mtk_detect_dl_keys()) {
        platform_emergency_download(CFG_EMERGENCY_DL_TIMEOUT_MS);
    }
#endif


#if CFG_REBOOT_TEST
    mtk_wdt_sw_reset();
    while(1);
#endif

#if !CFG_FPGA_PLATFORM
/*In FPGA phase, dram related function should by pass*/
    bootarg.dram_rank_num = get_dram_rank_nr();
    get_dram_rank_size(bootarg.dram_rank_size);
    get_orig_dram_rank_info(&bootarg.orig_dram_info);
    setup_mblock_info(&bootarg.mblock_info, &bootarg.orig_dram_info, &bootarg.lca_reserved_mem);
#else
    /*pass a defaut dram info to LK*/ //256 + 256MB
    bootarg.dram_rank_num = 0x00000001;
    bootarg.dram_rank_size[0] = 0x20000000;
    bootarg.dram_rank_size[1] = 0; //0x20000000;
    bootarg.dram_rank_size[2] = 0;
    bootarg.dram_rank_size[3] = 0;
    bootarg.mblock_info.mblock_num = 0x00000001;
    bootarg.mblock_info.mblock[0].start = 0x40000000;
    bootarg.mblock_info.mblock[0].size = 0x20000000;
    bootarg.mblock_info.mblock[0].rank = 0;
    bootarg.mblock_info.mblock[1].start = 0; //x60000000;
    bootarg.mblock_info.mblock[1].size = 0; //x1FE00000;
    bootarg.mblock_info.mblock[1].rank = 0;
    bootarg.mblock_info.mblock[2].start = 0;
    bootarg.mblock_info.mblock[2].size = 0;
    bootarg.mblock_info.mblock[2].rank = 0;
    bootarg.mblock_info.mblock[3].start = 0;
    bootarg.mblock_info.mblock[3].size = 0;
    bootarg.mblock_info.mblock[3].rank = 0;
    bootarg.mblock_info.reserved_num = 0;
    bootarg.mblock_info.reserved[0].start = 0;
    bootarg.orig_dram_info.rank_num = 1;
    bootarg.orig_dram_info.rank_info[0].start = 0x40000000;
    bootarg.orig_dram_info.rank_info[0].size = 0x20000000;
    bootarg.orig_dram_info.rank_info[1].start = 0; //x60000000;
    bootarg.orig_dram_info.rank_info[1].size = 0; //x1FE00000;
    bootarg.orig_dram_info.rank_info[2].start = 0;
    bootarg.orig_dram_info.rank_info[2].size = 0;
    bootarg.orig_dram_info.rank_info[3].start = 0;
    bootarg.orig_dram_info.rank_info[3].size = 0;
    bootarg.lca_reserved_mem.start = 0;
    bootarg.lca_reserved_mem.size = 0;
#endif
}

void platform_post_init(void)
{
#if CFG_FUNCTION_PICACHU_SUPPORT
    print("[PICACHU]start_picachu\n");
    extern void start_picachu();
    start_picachu();
#endif

#if CFG_BATTERY_DETECT
    /* normal boot to check battery exists or not */
    if (g_boot_mode == NORMAL_BOOT && !hw_check_battery() && usb_accessory_in()) {
        print("%s Wait for battery inserted...\n", MOD);
        /* disable pmic pre-charging led */
        pl_close_pre_chr_led();
        /* enable force charging mode */
        pl_charging(1);
        do {
            mdelay(300);
            /* check battery exists or not */
            if (hw_check_battery())
                break;
            /* kick all watchdogs */
            platform_wdt_all_kick();
        } while(1);
        /* disable force charging mode */
        pl_charging(0);
    }
#endif
#if !CFG_FPGA_PLATFORM
    pl_battery_init(true);
#endif
	BOOTING_TIME_PROFILING_LOG("Battery detect");


#if CFG_MDJTAG_SWITCH
    unsigned int md_pwr_con;

    /* md0 default power on and clock on */
    /* md1 default power on and clock off */

    /* ungate md1 */
    /* rst_b = 0 */
    md_pwr_con = DRV_Reg32(0x10006280);
    md_pwr_con &= ~0x1;
    DRV_WriteReg32(0x10006280, md_pwr_con);

    /* enable clksq2 for md1 */
    DRV_WriteReg32(0x10209000, 0x00001137);
    udelay(200);
    DRV_WriteReg32(0x10209000, 0x0000113f);

    /* rst_b = 1 */
    md_pwr_con = DRV_Reg32(0x10006280);
    md_pwr_con |= 0x1;
    DRV_WriteReg32(0x10006280, md_pwr_con);

    /* switch to MD legacy JTAG */
    /* this step is not essentially required */
#endif
	BOOTING_TIME_PROFILING_LOG("MTJTAG switch");

#if CFG_MDMETA_DETECT
    if (g_boot_mode == META_BOOT || g_boot_mode == ADVMETA_BOOT) {
	/* trigger md0 to enter meta mode */
        DRV_WriteReg32(0x20000010, 0x1);
	/* trigger md1 to enter meta mode */
        DRV_WriteReg32(0x30000010, 0x1);
    } else {
	/* md0 does not enter meta mode */
        DRV_WriteReg32(0x20000010, 0x0);
	/* md1 does not enter meta mode */
        DRV_WriteReg32(0x30000010, 0x0);
    }
#endif
	BOOTING_TIME_PROFILING_LOG("MTMETA Detect");

	DRV_WriteReg32(0x14000904, 0x0);

    platform_parse_bootopt();
    ram_console_mblock_reserve();
    platform_set_boot_args();
	BOOTING_TIME_PROFILING_LOG("Boot Argu");
#ifdef DRAM_ETT
	while(1);
#endif
}

void platform_error_handler(void)
{
    int i = 0;
    /* if log is disabled, re-init log port and enable it */
    if (log_status() == 0) {
        mtk_uart_init(UART_SRC_CLK_FRQ, CFG_LOG_BAUDRATE);
        log_ctrl(1);
    }
    print("PL fatal error...\n");

    #if !CFG_FPGA_PLATFORM
    sec_util_brom_download_recovery_check();
    #endif

#if defined(ONEKEY_REBOOT_NORMAL_MODE_PL) || defined(TWOKEY_REBOOT_NORMAL_MODE_PL)
    /* add delay for Long Preessed Reboot count down
       only pressed power key will have this delay */
    print("PL delay for Long Press Reboot\n");
    for ( i=3; i > 0;i-- ) {
        if (mtk_detect_key(PL_PMIC_PWR_KEY)) {
            platform_wdt_kick();
            mdelay(5000);   //delay 5s/per kick,
        } else {
            break; //Power Key Release,
        }
    }
#endif

    /* enter emergency download mode */
    #if CFG_EMERGENCY_DL_SUPPORT
    platform_emergency_download(CFG_EMERGENCY_DL_TIMEOUT_MS);
    #endif

    while(1);
}

void platform_assert(char *file, int line, char *expr)
{
    print("<ASSERT> %s:line %d %s\n", file, line, expr);
    platform_error_handler();
}

int aarch64_slt_done(void)
{
#if CFG_LOAD_SLT_AARCH64_KERNEL
    if ((*(unsigned int*) AARCH64_SLT_DONE_ADDRESS) == AARCH64_SLT_DONE_MAGIC)
    {
        return 1;
    }
    else
    {
        return 0;
    }
#endif
    return 1;
}


