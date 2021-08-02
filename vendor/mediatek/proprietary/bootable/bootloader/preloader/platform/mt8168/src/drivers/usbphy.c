/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
#include "typedefs.h"
#include "platform.h"
#include "usbphy.h"
#include "usbd.h"
#include "pll.h"
#include "u3phy-i2c.h"
#include "ssusb_sifslv_ippc_c_header.h"
#include "ssusb_usb3_mac_csr_c_header.h"
#include "mtk-phy-d60802.h"

#define USB11PHY_READ8(offset)		__raw_readb(USB11_PHY_BASE + offset)
#define USB11PHY_WRITE8(offset, value)	__raw_writeb(value, USB11_PHY_BASE+offset)
#define USB11PHY_SET8(offset, mask)	USB11PHY_WRITE8(offset, USB11PHY_READ8(offset) | mask)
#define USB11PHY_CLR8(offset, mask)	USB11PHY_WRITE8(offset, USB11PHY_READ8(offset) & ~mask)

#if CFG_FPGA_PLATFORM
#define U3_PHY_PAGE		0xff
#define I2C_CHIP		0xc0

static struct mt_i2c_t usb_i2c;

#define DEBUG 0
#if DEBUG
	#define PHY_LOG print
#else
	#define PHY_LOG
#endif


#define MD1122_I2C_ADDR 0x60
#define PHY_VERSION_BANK 0x20
#define PHY_VERSION_ADDR 0xe4

static void *g_ippc_port_addr;

static int usb_phy_writeb(unsigned char data, unsigned char addr)
{
	u3phy_write_reg(g_ippc_port_addr, MD1122_I2C_ADDR, addr, data);

	return 0;
}

static unsigned char usb_phy_readb(unsigned char addr)
{
	unsigned char data;

	data = u3phy_read_reg(g_ippc_port_addr, MD1122_I2C_ADDR, addr);

	return data;
}

unsigned int get_phy_verison(void)
{
	unsigned int version = 0;

	u3phy_write_reg8(g_ippc_port_addr, MD1122_I2C_ADDR, 0xff, PHY_VERSION_BANK);

	version = u3phy_read_reg32(g_ippc_port_addr, MD1122_I2C_ADDR, PHY_VERSION_ADDR);
	print("ssusb phy version: %x %p\n", version, g_ippc_port_addr);

	return version;
}

#endif

#define USBPHY_READ32(offset)		__raw_readl(SSUSB_PHY_BASE+offset)
#define USBPHY_WRITE32(offset, value)	__raw_writel(value, SSUSB_PHY_BASE+offset)
#define USBPHY_SET32(offset, mask)	USBPHY_WRITE32(offset, USBPHY_READ32(offset) | mask)
#define USBPHY_CLR32(offset, mask)	USBPHY_WRITE32(offset, USBPHY_READ32(offset) & ~mask)

#if CFG_FPGA_PLATFORM

void mu3d_hal_pdn_dis(void) {
	USB_CLRMASK(U3D_SSUSB_IP_PW_CTRL2, SSUSB_IP_DEV_PDN);
#ifdef SUPPORT_U3
	USB_CLRMASK(U3D_SSUSB_U3_CTRL_0P, (SSUSB_U3_PORT_DIS | SSUSB_U3_PORT_PDN | SSUSB_U3_PORT_U2_CG_EN));
#endif
	USB_CLRMASK(U3D_SSUSB_U2_CTRL_0P, (SSUSB_U2_PORT_DIS | SSUSB_U2_PORT_PDN | SSUSB_U2_PORT_U2_CG_EN));
}

#define PHY_DRV_SHIFT	3
#define PHY_PHASE_SHIFT	3
#define PHY_PHASE_DRV_SHIFT	1

#define os_writel(addr, value) __raw_writel(value, addr)


int md1122_u3phy_init(void *i2c_port_base)
{
	g_ippc_port_addr = i2c_port_base;

	if (get_phy_verison() != 0xa60810a) {
		print( "get phy version failed\n");
		return -1;
	}

	/* usb phy initial sequence */
	usb_phy_writeb(0x00, 0xFF);
	print("*********** before bank 0x00 ***********\n");
	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x05, value: %x\n", usb_phy_readb(0x05));
	print("[U2P]addr: 0x18, value: %x\n", usb_phy_readb(0x18));
	print("*********** after ***********\n");
	usb_phy_writeb(0x55, 0x05);
	usb_phy_writeb(0x84, 0x18);

	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x05, value: %x\n", usb_phy_readb(0x05));
	print("[U2P]addr: 0x18, value: %x\n", usb_phy_readb(0x18));
	print("*********** before bank 0x10 ***********\n");
	usb_phy_writeb(0x10, 0xFF);
	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x0A, value: %x\n", usb_phy_readb(0x0A));
	print("*********** after ***********\n");

	usb_phy_writeb(0x84, 0x0A);

	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x0A, value: %x\n", usb_phy_readb(0x0A));
	print("*********** before bank 0x40 ***********\n");
	usb_phy_writeb(0x40, 0xFF);
	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x38, value: %x\n", usb_phy_readb(0x38));
	print("[U2P]addr: 0x42, value: %x\n", usb_phy_readb(0x42));
	print("[U2P]addr: 0x08, value: %x\n", usb_phy_readb(0x08));
	print("[U2P]addr: 0x09, value: %x\n", usb_phy_readb(0x09));
	print("[U2P]addr: 0x0C, value: %x\n", usb_phy_readb(0x0C));
	print("[U2P]addr: 0x0E, value: %x\n", usb_phy_readb(0x0E));
	print("[U2P]addr: 0x10, value: %x\n", usb_phy_readb(0x10));
	print("[U2P]addr: 0x14, value: %x\n", usb_phy_readb(0x14));
	print("*********** after ***********\n");

	usb_phy_writeb(0x46, 0x38);
	usb_phy_writeb(0x40, 0x42);
	usb_phy_writeb(0xAB, 0x08);
	usb_phy_writeb(0x0C, 0x09);
	usb_phy_writeb(0x71, 0x0C);
	usb_phy_writeb(0x4F, 0x0E);
	usb_phy_writeb(0xE1, 0x10);
	usb_phy_writeb(0x5F, 0x14);
	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x38, value: %x\n", usb_phy_readb(0x38));
	print("[U2P]addr: 0x42, value: %x\n", usb_phy_readb(0x42));
	print("[U2P]addr: 0x08, value: %x\n", usb_phy_readb(0x08));
	print("[U2P]addr: 0x09, value: %x\n", usb_phy_readb(0x09));
	print("[U2P]addr: 0x0C, value: %x\n", usb_phy_readb(0x0C));
	print("[U2P]addr: 0x0E, value: %x\n", usb_phy_readb(0x0E));
	print("[U2P]addr: 0x10, value: %x\n", usb_phy_readb(0x10));
	print("[U2P]addr: 0x14, value: %x\n", usb_phy_readb(0x14));
	print("*********** before bank 0x60 ***********\n");
	usb_phy_writeb(0x60, 0xFF);
	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x10, value: %x\n", usb_phy_readb(0x14));
	print("*********** after ***********\n");

	usb_phy_writeb(0x03, 0x14);
	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x10, value: %x\n", usb_phy_readb(0x14));
	print("*********** before bank 0x00 ***********\n");
	usb_phy_writeb(0x00, 0xFF);
	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x6A, value: %x\n", usb_phy_readb(0x6A));
	print("[U2P]addr: 0x68, value: %x\n", usb_phy_readb(0x68));
	print("[U2P]addr: 0x6C, value: %x\n", usb_phy_readb(0x6C));
	print("[U2P]addr: 0x6D, value: %x\n", usb_phy_readb(0x6D));
	usb_phy_writeb(0x04, 0x6A);
	usb_phy_writeb(0x08, 0x68);
	usb_phy_writeb(0x26, 0x6C);
	usb_phy_writeb(0x36, 0x6D);
	print("*********** after ***********\n");
	print("[U2P]addr: 0xFF, value: %x\n", usb_phy_readb(0xFF));
	print("[U2P]addr: 0x6A, value: %x\n", usb_phy_readb(0x6A));
	print("[U2P]addr: 0x68, value: %x\n", usb_phy_readb(0x68));
	print("[U2P]addr: 0x6C, value: %x\n", usb_phy_readb(0x6C));
	print("[U2P]addr: 0x6D, value: %x\n", usb_phy_readb(0x6D));

	print("[U2P]%s, end\n", __func__);

	mu3d_hal_ssusb_en();
	mu3d_hal_rst_dev();

	return 0;
}

void mt_usb_phy_poweron(void)
{
	md1122_u3phy_init(0x11280700 + 0x00D0);
}

void mt_usb_phy_savecurrent(void)
{
}
void mt_usb_phy_recover(void)
{
}

void Charger_Detect_Init(void)
{
}

void Charger_Detect_Release(void)
{
}

void mt_usb11_phy_savecurrent(void)
{
}
#else

void enable_ssusb_xtal_clock(bool enable)
{
	if (enable) {
		/* PLL power on sequence */
		USB_SETMASK(APMIXEDSYS_UNIVPLL_PWR_CON0, BV_UNIVPLLPWRCON0_DA_UNIVPLL_SDM_PWR_ON);
		/* Wait 100 usec */
		udelay(100);
		USB_CLRMASK(APMIXEDSYS_UNIVPLL_PWR_CON0, BV_UNIVPLLPWRCON0_DA_UNIVPLL_SDM_ISO_EN);

		/* setting that control freq as 1610612736*/
		USB_SETMASK(APMIXEDSYS_UNIVPLL_CON0, BV_UNIVPLLCON0_RG_UNIVPLL_SDM_FRA_EN);
		__raw_writel(0x60000000, APMIXEDSYS_UNIVPLL_CON1);
		USB_SETMASK(APMIXEDSYS_UNIVPLL_CON0, BV_UNIVPLLCON0_RG_UNIVPLL_SDM_PCW_CHG);
		USB_CLRMASK(APMIXEDSYS_UNIVPLL_CON0, BM_UNIVPLLCON0_RG_UNIVPLL_POSDIV);
		/* Wait 100 usec */
		udelay(100);

		/* PLL enable */
		USB_SETMASK(APMIXEDSYS_UNIVPLL_CON0, BV_UNIVPLLCON0_RG_UNIVPLL_EN);
		/* Wait 100 usec */
		udelay(200);
		USB_SETMASK(APMIXEDSYS_UNIVPLL_CON0, BV_UNIVPLLCON0_RG_DIV_RSTB);
		/* 192MHz enable */
		USB_SETMASK(APMIXEDSYS_PLL_TEST_CON2, BV_PLLTESTCON2_RG_UNIVPLL_192M_EN);

		/* 48MHz div select */
		USB_CLRMASK(APMIXEDSYS_PLL_TEST_CON0, BV_PLLTESTCON0_PLLDIV_TEST);

		/* f_usb30, setting 124.8MHz */
		USB_CLRMASK(TOPCKGEN_CLK_CFG_3, BM_CLKCFG3_CLK_USB30_SEL);
		USB_SETMASK(TOPCKGEN_CLK_CFG_3, SSUSB_CLOCK_MUX_SET(UNIVPLL3_D2));
	}
}


void switch_2_usb()
{
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_FORCE_UART_EN);
	USB_CLRMASK(U3D_U2PHYDTM1, D60802_RG_UART_EN);
	USB_CLRMASK(U3D_U2PHYACR4, D60802_RG_USB20_GPIO_CTL);
	USB_CLRMASK(U3D_U2PHYACR4, D60802_USB20_GPIO_MODE);
}

/* return 0: internal current(proto pcb), else u3 current */
static int magna_pcb_version_detect(void)
{
	int ret = 0, data[4], rawvalue;

	ret = IMM_GetOneChannelValue(0, data, &rawvalue);
	if (ret) {
		print("%s failed %d\n", __func__, ret);
		return -1;
	}
	print("%s raw value:%d\n", __func__, rawvalue);
	return !!(rawvalue < 410 || rawvalue > 683);
}

#ifdef MTK_USB_EXTCON_SUPPORT
#define USBPHYACR6					0x018
#define RG_USB20_BC11_SW_EN			( 1ul << 23)

#define U2PHYACR4					0x020
#define RG_USB20_DM_100K_EN			( 1ul << 17 )

#define U2PHYDTM0					0x68
#define FORCE_DM_PULLDOWN			( 1ul << 21 )
#define FORCE_DP_PULLDOWN			( 1ul << 20 )

#define RG_DMPULLDOWN				( 1ul <<  7 )
#define RG_DPPULLDOWN				( 1ul <<  6 )


#define U2PHYDMON1					0x74
#define B_USB20_LINE_STATE 			22
#define USB20_LINE_STATE_MASK		( 3ul << B_USB20_LINE_STATE )

#define USB20_PHY_BASE				SSUSB_SIFSLV_U2PHY_COM_SIV_B_BASE


#define USBPHY_READ32(offset)				__raw_readl(USB20_PHY_BASE+(offset))
#define USBPHY_WRITE32(value, offset)		__raw_writel(value, USB20_PHY_BASE+(offset))

#define SSUSBIPPC_READ32(offset)			__raw_readl(USB20_IPPC_BASE+(offset))
#define SSUSBIPPC_WRITE32(value, offset)	__raw_writel(value, USB20_IPPC_BASE+(offset))

#define MOD "[PLFM]"
#if 0
#define LS_PRINT(fmt, ...)			print(fmt, ##__VA_ARGS__)
#else
#define LS_PRINT(fmt, ...)
#endif

int mt_usb_phychk_extconn(void)
{
	U32 val = 0;
	U32 line_state = 0;
	int usb_extconn = MT_USB_EXTCONN_UNKOWN;

	const char *string_usb_extconn_type[] = {
		 "UNKNOWN LINE TYPE",
		 "STANDARD_HOST",
		 "CHARGING_HOST",
		 "NONSTANDARD_CHARGER",
		 "STANDARD_CHARGER",
		 "INVALID PARAMETER",
	 };

	/* set PHY 0x18[23] = 1'b0 */
	val  = USBPHY_READ32(USBPHYACR6);
	val &= ~(RG_USB20_BC11_SW_EN);
	USBPHY_WRITE32(val, USBPHYACR6);
	LS_PRINT("usbphy addr 0x%x = 0x%x but 0x%x\n",
			USB20_PHY_BASE+USBPHYACR6, val, USBPHY_READ32(USBPHYACR6));

	/* Device side does NOT apply 15K pull-down on DP, and apply 100K pull up
	 * on DM.
	 * 1. For USB Host, 15K pull-down will cause linestate as 2'b00.
	 * 2. For Charger since no pull-down exist on D+/-, the linesate will be 2'b1x.
	 *
	 * stage 1
	 * set PHY 0x68[21:20] = 2'b11
	 * set PHY 0x68[ 7: 6] = 2'b00
	 * set PHY 0x20[   17] = 1'b1
	 */

	val  = USBPHY_READ32(U2PHYDTM0);
	val |=  (FORCE_DP_PULLDOWN | FORCE_DM_PULLDOWN);
	val &= ~(RG_DPPULLDOWN | RG_DMPULLDOWN);
	USBPHY_WRITE32(val, U2PHYDTM0);
	LS_PRINT("usbphy addr 0x%x = 0x%x but 0x%x\n",
			USB20_PHY_BASE+U2PHYDTM0, val, USBPHY_READ32(U2PHYDTM0));


	val  = USBPHY_READ32(U2PHYACR4);
	val |=  RG_USB20_DM_100K_EN;
	USBPHY_WRITE32(val, U2PHYACR4);
	LS_PRINT("usbphy addr 0x%x = 0x%x but 0x%x\n",
			USB20_PHY_BASE+U2PHYACR4, val, USBPHY_READ32(U2PHYACR4));

	mdelay(10);

	/* Read linestate
	  * Read PHY 0x74[23:22] 2'bxx
	  */
	line_state  = USBPHY_READ32(U2PHYDMON1);
	LS_PRINT("usbphy addr 0x%x = 0x%x\n",
			USB20_PHY_BASE+U2PHYDMON1, USBPHY_READ32(U2PHYDMON1));

	line_state  &=  USB20_LINE_STATE_MASK;
	line_state >>= B_USB20_LINE_STATE;
	if ((line_state & 0x02) == 0)
		usb_extconn = MT_USB_EXTCONN_STANDARDHOST;
	else if ((line_state & 0x02) != 0) {
		/* Device side does apply 15K pul-down on DP, and apply 100K pull up
		 * on DM.
		 * 1. For standard charger, D+/- are shorted, so the D+ pulling down
		 *     will drive both D+/- to low. therefore  linestate as 2'b00.
		 * 2. For non-standard charger, since no pull-down exist on D-,
		 *     the linesate will be 2'b1x.
		 *
		 * stage 2
		 * set PHY 0x68[21:20] = 2'b11
		 * set PHY 0x68[ 7: 6] = 2'b01
		 * set PHY 0x20[   17] = 1'b1
		 */

		val  = USBPHY_READ32(U2PHYDTM0);
		val |=  (FORCE_DP_PULLDOWN | FORCE_DM_PULLDOWN);
		val &= ~(RG_DPPULLDOWN | RG_DMPULLDOWN);
		val |=   RG_DPPULLDOWN;
		USBPHY_WRITE32(val, U2PHYDTM0);
		LS_PRINT("usbphy addr 0x%x = 0x%x but 0x%x\n",
				USB20_PHY_BASE+U2PHYDTM0, val, USBPHY_READ32(U2PHYDTM0));

		val  = USBPHY_READ32(U2PHYACR4);
		val |=  RG_USB20_DM_100K_EN;
		USBPHY_WRITE32(val, U2PHYACR4);
		LS_PRINT("usbphy addr 0x%x = 0x%x but 0x%x\n",
				USB20_PHY_BASE+U2PHYACR4, val, USBPHY_READ32(U2PHYACR4));

		mdelay(10);

		 /* Read linestate
		   * Read PHY 0x74[23:22] 2'bxx
		   */
		line_state  = USBPHY_READ32(U2PHYDMON1);
		LS_PRINT("usbphy addr 0x%x = 0x%x\n",
				USB20_PHY_BASE+U2PHYDMON1, USBPHY_READ32(U2PHYDMON1));

		line_state	&=	USB20_LINE_STATE_MASK;
		line_state >>= B_USB20_LINE_STATE;

		switch(line_state) {
		case 0x00:
			usb_extconn = MT_USB_EXTCONN_STANDARDCHARGER;
			 break;
		case 0x02:
		case 0x03:
			usb_extconn = MT_USB_EXTCONN_NONSTANDARDCHARGER;
			 break;
		default:
			usb_extconn = MT_USB_EXTCONN_UNKOWN;
			break;
		}
	}

	val  = USBPHY_READ32(U2PHYDTM0);
	val &=  ~(FORCE_DP_PULLDOWN | FORCE_DM_PULLDOWN);
	USBPHY_WRITE32(val, U2PHYDTM0);
	LS_PRINT("usbphy addr 0x%x = 0x%x but 0x%x\n",
				USB20_PHY_BASE+U2PHYDTM0, val, USBPHY_READ32(U2PHYDTM0));

	val  = USBPHY_READ32(U2PHYACR4);
	val &= ~RG_USB20_DM_100K_EN;
	USBPHY_WRITE32(val, U2PHYACR4);
	LS_PRINT("usbphy addr 0x%x = 0x%x but 0x%x\n",
				USB20_PHY_BASE+U2PHYACR4, val, USBPHY_READ32(U2PHYACR4));

	usb_extconn = (usb_extconn > MT_USB_EXTCONN_STANDARDCHARGER)
					? MT_USB_EXTCONN_MAXIMUM : usb_extconn;

	print("\n%s Final extened connector type: %s since line state: 0x%x\n",
				MOD, string_usb_extconn_type[usb_extconn], line_state);

	return usb_extconn;
}
#endif /*  MTK_USB_EXTCON_SUPPORT */


void mt_usb_phy_poweron(void)
{
	enable_ssusb_xtal_clock(1);

	/* 6, switch to USB function */
	switch_2_usb();
	/* 7, DP/DM BC1.1 path Disable */
	USB_CLRMASK(U3D_USBPHYACR6, D60802_RG_USB20_BC11_SW_EN);
	/*Internal R bias enable*/
	USB_SETMASK(U3D_USBPHYACR0, D60802_RG_USB20_INTR_EN);
	/* 10, Change 100uA current switch to SSUSB */
	USB_CLRMASK(U3D_USBPHYACR5, D60802_RG_USB20_HS_100U_U3_EN);
	/* 8, dp_100k diable */
	USB_CLRMASK(U3D_U2PHYACR4, D60802_USB20_DP_100K_EN);
	/* 9, dm_100k disable */
	USB_CLRMASK(U3D_U2PHYACR4, D60802_RG_USB20_DM_100K_EN);
	/* 11, OTG enable */
	USB_CLRMASK(U3D_USBPHYACR6, D60802_RG_USB20_OTG_VBUSCMP_EN);
	/* 12, Release force suspendm */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_FORCE_SUSPENDM);

    mu3d_hal_ssusb_en();
}

void mt_usb_phy_savecurrent(void)
{
	/* 1, switch to usb function */
	switch_2_usb();
	/* 2, let syspendm=1 */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_RG_SUSPENDM);
	/* 3, force_suspendm */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_FORCE_SUSPENDM);
	/* 4 wait for USBPLL stable */
	mdelay(2);
	/* 5 */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_RG_DPPULLDOWN);
	/* 6 */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_RG_DMPULLDOWN);
	/* 7 */
	__raw_writel((__raw_readl(U3D_U2PHYDTM0) & ~D60802_RG_XCVRSEL) | (0x1 << D60802_RG_XCVRSEL_OFST), U3D_U2PHYDTM0);
	/* 8 */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_RG_TERMSEL);
	/* 9 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_RG_DATAIN);
	/* 10 */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_FORCE_DP_PULLDOWN);
	/* 11 */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_FORCE_DM_PULLDOWN);
	/* 12 */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_FORCE_XCVRSEL);
	/* 13 */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_FORCE_TERMSEL);
	/* 14 */
	USB_SETMASK(U3D_U2PHYDTM0, D60802_FORCE_DATAIN);
	/* 15, DP/DM BC1.1 path Disable */
	USB_CLRMASK(U3D_USBPHYACR6, D60802_RG_USB20_BC11_SW_EN);
	/* 16, OTG disable */
	USB_CLRMASK(U3D_USBPHYACR6, D60802_RG_USB20_OTG_VBUSCMP_EN);
	/* 18, wait 800us */
	mdelay(1);
	/* 19 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_RG_SUSPENDM);
	/* 20, wait 1us */
	mdelay(1);
}

void mt_usb_phy_recover(void)
{

	/* 1, switch to usb function */
	switch_2_usb();
	/* 2, force_suspendm */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_FORCE_SUSPENDM);
	/* 3 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_RG_DPPULLDOWN);
	/* 4 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_RG_DMPULLDOWN);
	/* 5 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_RG_XCVRSEL);
	/* 6 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_RG_TERMSEL);
	/* 7 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_RG_DATAIN);
	/* 8 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_FORCE_DP_PULLDOWN);
	/* 9 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_FORCE_DM_PULLDOWN);
	/* 10 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_FORCE_XCVRSEL);
	/* 11 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_FORCE_TERMSEL);
	/* 12 */
	USB_CLRMASK(U3D_U2PHYDTM0, D60802_FORCE_DATAIN);
	/* 13, DP/DM BC1.1 path Disable */
	USB_CLRMASK(U3D_USBPHYACR6, D60802_RG_USB20_BC11_SW_EN);
	/* 14, OTG disable  */
	USB_CLRMASK(U3D_USBPHYACR6, D60802_RG_USB20_OTG_VBUSCMP_EN);
	/* 15, Change 100uA current switch to SSUSB */
	USB_CLRMASK(U3D_USBPHYACR5, D60802_RG_USB20_HS_100U_U3_EN);

	/* 16, wait 800us */
	mdelay(1);
}
/* BC1.2 */
void Charger_Detect_Init(void)
{
	/* turn on USB reference clock. */
	/* enable_clock(MT_CG_PERI_USB0, "USB30"); */

	/* wait 50 usec. */
	/* udelay(50); */

	/* RG_USB20_BC11_SW_EN = 1'b1 */
	USB_SETMASK(U3D_USBPHYACR6, D60802_RG_USB20_BC11_SW_EN);
	/* udelay(1); */

	/* 4 14. turn off internal 48Mhz PLL. */
	/*disable_clock(MT_CG_PERI_USB0, "USB30"); */
}

void Charger_Detect_Release(void)
{
	/* turn on USB reference clock. */
	/*enable_clock(MT_CG_PERI_USB0, "USB30"); */

	/* wait 50 usec. */
	/* udelay(50); */

	/* RG_USB20_BC11_SW_EN = 1'b0 */
	USB_CLRMASK(U3D_USBPHYACR6, D60802_RG_USB20_BC11_SW_EN);
	/* udelay(1); */

	/* 4 14. turn off internal 48Mhz PLL. */
	/* disable_clock(MT_CG_PERI_USB0, "USB30"); */
}

#endif
