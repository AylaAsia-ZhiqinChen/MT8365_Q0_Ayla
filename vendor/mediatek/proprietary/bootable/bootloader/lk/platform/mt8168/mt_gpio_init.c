/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2016. All rights reserved.
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
 * THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
 * applicable license agreements with MediaTek Inc.
 */

#include <platform/mt_gpio.h>

#include <cust_power.h>
#include <cust_gpio_boot.h>
#include <platform.h>
#include <platform/mt_reg_base.h>
#include <debug.h>

//#include <debug.h>
#define GPIO_INIT_DEBUG 1
/*----------------------------------------------------------------------------*/
#define GPIOTAG "[GPIO] "
#define GPIODBG(fmt, arg...)    printf(GPIOTAG "%s: " fmt, __FUNCTION__ ,##arg)
#define GPIOERR(fmt, arg...)    printf(GPIOTAG "%s: " fmt, __FUNCTION__ ,##arg)
#define GPIOVER(fmt, arg...)    printf(GPIOTAG "%s: " fmt, __FUNCTION__ ,##arg)

#define GPIO_WR32(addr, data)   DRV_WriteReg32(addr,data)
#define GPIO_RD32(addr)         DRV_Reg32(addr)

u32 gpio_init_dir_data[] = {
	((GPIO0_DIR       <<  0) |(GPIO1_DIR       <<  1) |(GPIO2_DIR       <<  2) |(GPIO3_DIR       <<  3) |
	(GPIO4_DIR       <<  4) |(GPIO5_DIR       <<  5) |(GPIO6_DIR       <<  6) |(GPIO7_DIR       <<  7) |
	(GPIO8_DIR       <<  8) |(GPIO9_DIR       <<  9) |(GPIO10_DIR      << 10) |(GPIO11_DIR      << 11) |
	(GPIO12_DIR      << 12) |(GPIO13_DIR      << 13) |(GPIO14_DIR      << 14) |(GPIO15_DIR      << 15) |
	(GPIO16_DIR      <<  16) |(GPIO17_DIR      <<  17) |(GPIO18_DIR      <<  18) |(GPIO19_DIR      <<  19) |
	(GPIO20_DIR      <<  20) |(GPIO21_DIR      <<  21) |(GPIO22_DIR      <<  22) |(GPIO23_DIR      <<  23) |
	(GPIO24_DIR      <<  24) |(GPIO25_DIR      <<  25) |(GPIO26_DIR      << 26) |(GPIO27_DIR      << 27) |
	(GPIO28_DIR      << 28) |(GPIO29_DIR      << 29) |(GPIO30_DIR      << 30) |(GPIO31_DIR      << 31)),

	((GPIO32_DIR      <<  0) |(GPIO33_DIR      <<  1) |(GPIO34_DIR      <<  2) |(GPIO35_DIR      <<  3) |
	(GPIO36_DIR      <<  4) |(GPIO37_DIR      <<  5) |(GPIO38_DIR      <<  6) |(GPIO39_DIR      <<  7) |
	(GPIO40_DIR      <<  8) |(GPIO41_DIR      <<  9) |(GPIO42_DIR      << 10) |(GPIO43_DIR      << 11) |
	(GPIO44_DIR      << 12) |(GPIO45_DIR      << 13) |(GPIO46_DIR      << 14) |(GPIO47_DIR      << 15) |
	(GPIO48_DIR      << 16) |(GPIO49_DIR      << 17) |(GPIO50_DIR      << 18) |(GPIO51_DIR      << 19) |
	(GPIO52_DIR      << 20) |(GPIO53_DIR      << 21) |(GPIO54_DIR      << 22) |(GPIO55_DIR      << 23) |
	(GPIO56_DIR      << 24) |(GPIO57_DIR      << 25) |(GPIO58_DIR      << 26) |(GPIO59_DIR      << 27) |
	(GPIO60_DIR      << 28) |(GPIO61_DIR      << 29) |(GPIO62_DIR      << 30) |(GPIO63_DIR      << 31)),

	((GPIO64_DIR      <<  0) |(GPIO65_DIR      <<  1) |(GPIO66_DIR      <<  2) |(GPIO67_DIR      <<  3) |
	(GPIO68_DIR      <<  4) |(GPIO69_DIR      <<  5) |(GPIO70_DIR      <<  6) |(GPIO71_DIR      <<  7) |
	(GPIO72_DIR      <<  8) |(GPIO73_DIR      <<  9) |(GPIO74_DIR      << 10) |(GPIO75_DIR      << 11) |
	(GPIO76_DIR      << 12) |(GPIO77_DIR      << 13) |(GPIO78_DIR      << 14) |(GPIO79_DIR      << 15) |
	(GPIO80_DIR      << 16) |(GPIO81_DIR      << 17) |(GPIO82_DIR      << 18) |(GPIO83_DIR      << 19) |
	(GPIO84_DIR      << 20) |(GPIO85_DIR      << 21) |(GPIO86_DIR      << 22) |(GPIO87_DIR      << 23) |
	(GPIO88_DIR      << 24) |(GPIO89_DIR      << 25) |(GPIO90_DIR      << 26) |(GPIO91_DIR      << 27) |
	(GPIO92_DIR      << 28) |(GPIO93_DIR      << 29) |(GPIO94_DIR      << 30) |(GPIO95_DIR      << 31)),

	((GPIO96_DIR      <<  0) |(GPIO97_DIR      <<  1) |(GPIO98_DIR      <<  2) |(GPIO99_DIR      <<  3) |
	(GPIO100_DIR     <<  4) |(GPIO101_DIR     <<  5) |(GPIO102_DIR     <<  6) |(GPIO103_DIR     <<  7) |
	(GPIO104_DIR     <<  8) |(GPIO105_DIR     <<  9) |(GPIO106_DIR     << 10) |(GPIO107_DIR     << 11) |
	(GPIO108_DIR     << 12) |(GPIO109_DIR     << 13) |(GPIO110_DIR     << 14) |(GPIO111_DIR     << 15) |
	(GPIO112_DIR	  << 16) |(GPIO113_DIR	   << 17) |(GPIO114_DIR		<< 18) |(GPIO115_DIR 	 <<  19) |
	(GPIO116_DIR	 <<  20) |(GPIO117_DIR	  << 21) |(GPIO118_DIR	   <<  22) |(GPIO119_DIR 	<< 23) |
	(GPIO120_DIR	 <<  24) |(GPIO121_DIR	  <<  25) |(GPIO122_DIR	   << 26) |(GPIO123_DIR 	<< 27) |
	(GPIO124_DIR	 << 28) |(GPIO125_DIR	  << 29) |(GPIO126_DIR	   << 30) |(GPIO127_DIR 	<< 31)),

	((GPIO128_DIR	  <<  0) |(GPIO129_DIR	   <<  1) |(GPIO130_DIR		<<	2) |(GPIO131_DIR 	 <<  3) |
	(GPIO132_DIR	 <<  4) |(GPIO133_DIR	  <<  5) |(GPIO134_DIR	   <<  6) |(GPIO135_DIR 	<<	7) |
	(GPIO136_DIR	 <<  8) |(GPIO137_DIR	  <<  9) |(GPIO138_DIR	   << 10) |(GPIO139_DIR 	<< 11) |
	(GPIO140_DIR	 << 12) |(GPIO141_DIR	  << 13) |(GPIO142_DIR	   << 14) |(GPIO143_DIR 	<< 15) |
	(GPIO144_DIR	  << 16)),

}; /*end of gpio_init_dir_data*/

/*----------------------------------------------------------------------------*/
u32 gpio_init_pullen_data[] = {
	((GPIO0_PULLEN       <<  0) |(GPIO1_PULLEN       <<  1) |(GPIO2_PULLEN       <<  2) |(GPIO3_PULLEN       <<  3) |
	(GPIO4_PULLEN      <<  4) |(GPIO5_PULLEN       <<  5) |(GPIO6_PULLEN       <<  6) |(GPIO7_PULLEN       <<  7) |
	(GPIO8_PULLEN       <<  8) |(GPIO9_PULLEN       <<  9) |(GPIO10_PULLEN      << 10) |(GPIO11_PULLEN      << 11) |
	(GPIO12_PULLEN      << 12) |(GPIO13_PULLEN      << 13) |(GPIO14_PULLEN      << 14) |(GPIO15_PULLEN     << 15) |
	(GPIO16_PULLEN      <<  16) |(GPIO17_PULLEN      <<  17) |(GPIO18_DIR      <<  18) |(GPIO19_PULLEN      <<  19) |
	(GPIO20_PULLEN      <<  20) |(GPIO21_PULLEN      <<  21) |(GPIO22_PULLEN      <<  22) |(GPIO23_PULLEN      <<  23) |
	(GPIO24_PULLEN      <<  24) |(GPIO25_PULLEN      <<  25) |(GPIO26_PULLEN      << 26) |(GPIO27_PULLEN      << 27) |
	(GPIO28_PULLEN      << 28) |(GPIO29_PULLEN      << 29) |(GPIO30_PULLEN      << 30) |(GPIO31_PULLEN     << 31)),

	((GPIO32_PULLEN      <<  0) |(GPIO33_PULLEN      <<  1) |(GPIO34_PULLEN      <<  2) |(GPIO35_PULLEN      <<  3) |
	(GPIO36_PULLEN      <<  4) |(GPIO37_PULLEN      <<  5) |(GPIO38_PULLEN      <<  6) |(GPIO39_PULLEN      <<  7) |
	(GPIO40_PULLEN      <<  8) |(GPIO41_PULLEN      <<  9) |(GPIO42_PULLEN      << 10) |(GPIO43_PULLEN      << 11) |
	(GPIO44_PULLEN      << 12) |(GPIO45_PULLEN      << 13) |(GPIO46_PULLEN      << 14) |(GPIO47_PULLEN      << 15) |
	(GPIO48_PULLEN      << 16) |(GPIO49_PULLEN      << 17) |(GPIO50_PULLEN      << 18) |(GPIO51_PULLEN      << 19) |
	(GPIO52_PULLEN      << 20) |(GPIO53_PULLEN      << 21) |(GPIO54_PULLEN      << 22) |(GPIO55_PULLEN      << 23) |
	(GPIO56_PULLEN      << 24) |(GPIO57_PULLEN      << 25) |(GPIO58_PULLEN      << 26) |(GPIO59_PULLEN      << 27) |
	(GPIO60_PULLEN      << 28) |(GPIO61_PULLEN      << 29) |(GPIO62_PULLEN      << 30) |(GPIO63_PULLEN      << 31)),

	((GPIO64_PULLEN      <<  0) |(GPIO65_PULLEN      <<  1) |(GPIO66_PULLEN      <<  2) |(GPIO67_PULLEN      <<  3) |
	(GPIO68_PULLEN      <<  4) |(GPIO69_PULLEN      <<  5) |(GPIO70_PULLEN      <<  6) |(GPIO71_PULLEN      <<  7) |
	(GPIO72_PULLEN      <<  8) |(GPIO73_PULLEN      <<  9) |(GPIO74_PULLEN      << 10) |(GPIO75_PULLEN      << 11) |
	(GPIO76_PULLEN      << 12) |(GPIO77_PULLEN      << 13) |(GPIO78_PULLEN      << 14) |(GPIO79_PULLEN      << 15) |
	(GPIO80_PULLEN      << 16) |(GPIO81_PULLEN      << 17) |(GPIO82_PULLEN      << 18) |(GPIO83_PULLEN      << 19) |
	(GPIO84_PULLEN      << 20) |(GPIO85_PULLEN     << 21) |(GPIO86_PULLEN      << 22) |(GPIO87_PULLEN      << 23) |
	(GPIO88_PULLEN      << 24) |(GPIO89_PULLEN      << 25) |(GPIO90_PULLEN      << 26) |(GPIO91_PULLEN      << 27) |
	(GPIO92_PULLEN      << 28) |(GPIO93_PULLEN      << 29) |(GPIO94_PULLEN      << 30) |(GPIO95_PULLEN      << 31)),

	((GPIO96_PULLEN      <<  0) |(GPIO97_PULLEN      <<  1) |(GPIO98_PULLEN      <<  2) |(GPIO99_PULLEN      <<  3) |
	(GPIO100_PULLEN     <<  4) |(GPIO101_PULLEN     <<  5) |(GPIO102_PULLEN     <<  6) |(GPIO103_PULLEN     <<  7) |
	(GPIO104_PULLEN     <<  8) |(GPIO105_PULLEN     <<  9) |(GPIO106_PULLEN     << 10) |(GPIO107_PULLEN     << 11) |
	(GPIO108_PULLEN     << 12) |(GPIO109_PULLEN     << 13) |(GPIO110_PULLEN     << 14) |(GPIO111_PULLEN     << 15) |
	(GPIO112_PULLEN	  << 16) |(GPIO113_PULLEN	   << 17) |(GPIO114_PULLEN		<< 18) |(GPIO115_PULLEN 	 <<  19) |
	(GPIO116_PULLEN	 <<  20) |(GPIO117_PULLEN	  << 21) |(GPIO118_PULLEN	   <<  22) |(GPIO119_PULLEN 	<< 23) |
	(GPIO120_PULLEN	 <<  24) |(GPIO121_PULLEN	  <<  25) |(GPIO122_PULLEN	   << 26) |(GPIO123_PULLEN 	<< 27) |
	(GPIO124_PULLEN	 << 28) |(GPIO125_PULLEN	  << 29) |(GPIO126_PULLEN	   << 30) |(GPIO127_PULLEN 	<< 31)),

	((GPIO128_PULLEN	  <<  0) |(GPIO129_PULLEN	   <<  1) |(GPIO130_PULLEN		<<	2) |(GPIO131_PULLEN 	 <<  3) |
	(GPIO132_PULLEN	 <<  4) |(GPIO133_PULLEN	  <<  5) |(GPIO134_PULLEN	   <<  6) |(GPIO135_PULLEN 	<<	7) |
	(GPIO136_PULLEN	 <<  8) |(GPIO137_PULLEN	  <<  9) |(GPIO138_PULLEN	   << 10) |(GPIO139_PULLEN 	<< 11) |
	(GPIO140_PULLEN	 << 12) |(GPIO141_PULLEN	  << 13) |(GPIO142_PULLEN	   << 14) |(GPIO143_PULLEN 	<< 15) |
	(GPIO144_PULLEN	  << 16)),
}; /*end of gpio_init_pullen_data*/
/*----------------------------------------------------------------------------*/
u32 gpio_init_pullsel_data[] = {
	((GPIO0_PULL	   <<  0) |(GPIO1_PULL		 <<  1) |(GPIO2_PULL	   <<  2) |(GPIO3_PULL		 <<  3) |
	(GPIO4_PULL 	 <<  4) |(GPIO5_PULL	   <<  5) |(GPIO6_PULL		 <<  6) |(GPIO7_PULL	   <<  7) |
	(GPIO8_PULL 	  <<  8) |(GPIO9_PULL		<<	9) |(GPIO10_PULL	  << 10) |(GPIO11_PULL		<< 11) |
	(GPIO12_PULL	  << 12) |(GPIO13_PULL		<< 13) |(GPIO14_PULL	  << 14) |(GPIO15_PULL 	<< 15) |
	(GPIO16_PULL	  <<  16) |(GPIO17_PULL 	 <<  17) |(GPIO18_DIR	   <<  18) |(GPIO19_PULL	  <<  19) |
	(GPIO20_PULL	  <<  20) |(GPIO21_PULL 	 <<  21) |(GPIO22_PULL		<<	22) |(GPIO23_PULL	   <<  23) |
	(GPIO24_PULL	  <<  24) |(GPIO25_PULL 	 <<  25) |(GPIO26_PULL		<< 26) |(GPIO27_PULL	  << 27) |
	(GPIO28_PULL	  << 28) |(GPIO29_PULL		<< 29) |(GPIO30_PULL	  << 30) |(GPIO31_PULL	   << 31)),
		
	((GPIO32_PULL	   <<  0) |(GPIO33_PULL 	 <<  1) |(GPIO34_PULL	   <<  2) |(GPIO35_PULL 	 <<  3) |
	(GPIO36_PULL	  <<  4) |(GPIO37_PULL		<<	5) |(GPIO38_PULL	  <<  6) |(GPIO39_PULL		<<	7) |
	(GPIO40_PULL	  <<  8) |(GPIO41_PULL		<<	9) |(GPIO42_PULL	  << 10) |(GPIO43_PULL		<< 11) |
	(GPIO44_PULL	  << 12) |(GPIO45_PULL		<< 13) |(GPIO46_PULL	  << 14) |(GPIO47_PULL		<< 15) |
	(GPIO48_PULL	  << 16) |(GPIO49_PULL		<< 17) |(GPIO50_PULL	  << 18) |(GPIO51_PULL		<< 19) |
	(GPIO52_PULL	  << 20) |(GPIO53_PULL		<< 21) |(GPIO54_PULL	  << 22) |(GPIO55_PULL		<< 23) |
	(GPIO56_PULL	  << 24) |(GPIO57_PULL		<< 25) |(GPIO58_PULL	  << 26) |(GPIO59_PULL		<< 27) |
	(GPIO60_PULL	  << 28) |(GPIO61_PULL		<< 29) |(GPIO62_PULL	  << 30) |(GPIO63_PULL		<<31)),

	((GPIO64_PULL	   <<  0) |(GPIO65_PULL 	 <<  1) |(GPIO66_PULL	   <<  2) |(GPIO67_PULL 	 <<  3) |
	(GPIO68_PULL	  <<  4) |(GPIO69_PULL		<<	5) |(GPIO70_PULL	  <<  6) |(GPIO71_PULL		<<	7) |
	(GPIO72_PULL	  <<  8) |(GPIO73_PULL		<<	9) |(GPIO74_PULL	  << 10) |(GPIO75_PULL		<< 11) |
	(GPIO76_PULL	  << 12) |(GPIO77_PULL		<< 13) |(GPIO78_PULL	  << 14) |(GPIO79_PULL		<< 15) |
	(GPIO80_PULL	  << 16) |(GPIO81_PULL		<< 17) |(GPIO82_PULL	  << 18) |(GPIO83_PULL		<< 19) |
	(GPIO84_PULL	  << 20) |(GPIO85_PULL	   << 21) |(GPIO86_PULL 	 << 22) |(GPIO87_PULL	   << 23) |
	(GPIO88_PULL	  << 24) |(GPIO89_PULL		<< 25) |(GPIO90_PULL	  << 26) |(GPIO91_PULL		<< 27) |
	(GPIO92_PULL	  << 28) |(GPIO93_PULL		<< 29) |(GPIO94_PULL	  << 30) |(GPIO95_PULL		<< 31)),

	((GPIO96_PULL	   <<  0) |(GPIO97_PULL 	 <<  1) |(GPIO98_PULL	   <<  2) |(GPIO99_PULL 	 <<  3) |
	(GPIO100_PULL	  <<  4) |(GPIO101_PULL 	<<	5) |(GPIO102_PULL	  <<  6) |(GPIO103_PULL 	<<	7) |
	(GPIO104_PULL	  <<  8) |(GPIO105_PULL 	<<	9) |(GPIO106_PULL	  << 10) |(GPIO107_PULL 	<< 11) |
	(GPIO108_PULL	  << 12) |(GPIO109_PULL 	<< 13) |(GPIO110_PULL	  << 14) |(GPIO111_PULL 	<< 15) |
	(GPIO112_PULL	  << 16) |(GPIO113_PULL    << 17) |(GPIO114_PULL		<< 18) |(GPIO115_PULL	 <<  19) |
	(GPIO116_PULL	 <<  20) |(GPIO117_PULL   << 21) |(GPIO118_PULL    <<  22) |(GPIO119_PULL	<< 23) |
	(GPIO120_PULL	 <<  24) |(GPIO121_PULL   <<  25) |(GPIO122_PULL	   << 26) |(GPIO123_PULL	<< 27) |
	(GPIO124_PULL	 << 28) |(GPIO125_PULL	  << 29) |(GPIO126_PULL    << 30) |(GPIO127_PULL	<< 31)),

	((GPIO128_PULL	  <<  0) |(GPIO129_PULL    <<  1) |(GPIO130_PULL		<<	2) |(GPIO131_PULL	 <<  3) |
	(GPIO132_PULL	 <<  4) |(GPIO133_PULL	  <<  5) |(GPIO134_PULL    <<  6) |(GPIO135_PULL	<<	7) |
	(GPIO136_PULL	 <<  8) |(GPIO137_PULL	  <<  9) |(GPIO138_PULL    << 10) |(GPIO139_PULL	<< 11) |
	(GPIO140_PULL	 << 12) |(GPIO141_PULL	  << 13) |(GPIO142_PULL    << 14) |(GPIO143_PULL	<< 15) |
	(GPIO144_PULL	  << 16)),
}; /*end of gpio_init_pullsel_data*/

struct msdc_pull {
	u32 pin;
	u32 pullsel;
	u32 pullen;
};
struct msdc_pull msdc_pull_data[] = {
	{GPIO22, GPIO22_PULL, GPIO22_PULLEN},
	{GPIO23, GPIO23_PULL, GPIO23_PULLEN},
	{GPIO24, GPIO24_PULL, GPIO24_PULLEN},
	{GPIO25, GPIO25_PULL, GPIO25_PULLEN},
	{GPIO80, GPIO80_PULL, GPIO80_PULLEN},
	{GPIO81, GPIO81_PULL, GPIO81_PULLEN},
	{GPIO82, GPIO82_PULL, GPIO82_PULLEN},
	{GPIO83, GPIO83_PULL, GPIO83_PULLEN},
	{GPIO84, GPIO84_PULL, GPIO84_PULLEN},
	{GPIO85, GPIO85_PULL, GPIO85_PULLEN},
	{GPIO86, GPIO86_PULL, GPIO86_PULLEN},
	{GPIO87, GPIO87_PULL, GPIO87_PULLEN},
	{GPIO88, GPIO88_PULL, GPIO88_PULLEN},
	{GPIO89, GPIO89_PULL, GPIO89_PULLEN},
	{GPIO90, GPIO90_PULL, GPIO90_PULLEN},
	{GPIO91, GPIO91_PULL, GPIO91_PULLEN},
	{GPIO92, GPIO92_PULL, GPIO92_PULLEN},
	{GPIO93, GPIO93_PULL, GPIO93_PULLEN},
	{GPIO94, GPIO94_PULL, GPIO94_PULLEN},
	{GPIO95, GPIO95_PULL, GPIO95_PULLEN},
	{GPIO96, GPIO96_PULL, GPIO96_PULLEN},
	{GPIO97, GPIO97_PULL, GPIO97_PULLEN},
	{GPIO98, GPIO98_PULL, GPIO98_PULLEN},
	{GPIO99, GPIO99_PULL, GPIO99_PULLEN},
	{GPIO100, GPIO100_PULL, GPIO100_PULLEN},
	{GPIO101, GPIO101_PULL, GPIO101_PULLEN},
	{GPIO102, GPIO102_PULL, GPIO102_PULLEN},
	{GPIO103, GPIO103_PULL, GPIO103_PULLEN},
	{GPIO104, GPIO104_PULL, GPIO104_PULLEN},
	{GPIO105, GPIO105_PULL, GPIO105_PULLEN},
	{GPIO106, GPIO106_PULL, GPIO106_PULLEN},
	{GPIO107, GPIO107_PULL, GPIO107_PULLEN},
	{GPIO108, GPIO108_PULL, GPIO108_PULLEN},
	{GPIO109, GPIO109_PULL, GPIO109_PULLEN},
};

/*----------------------------------------------------------------------------*/
u32 gpio_init_dout_data[] = {
	((GPIO0_DATAOUT	   <<  0) |(GPIO1_DATAOUT		 <<  1) |(GPIO2_DATAOUT	   <<  2) |(GPIO3_DATAOUT		 <<  3) |
	(GPIO4_DATAOUT 	 <<  4) |(GPIO5_DATAOUT	   <<  5) |(GPIO6_DATAOUT		 <<  6) |(GPIO7_DATAOUT	   <<  7) |
	(GPIO8_DATAOUT 	  <<  8) |(GPIO9_DATAOUT		<<	9) |(GPIO10_DATAOUT	  << 10) |(GPIO11_DATAOUT		<< 11) |
	(GPIO12_DATAOUT	  << 12) |(GPIO13_DATAOUT		<< 13) |(GPIO14_DATAOUT	  << 14) |(GPIO15_DATAOUT 	<< 15) |
	(GPIO16_DATAOUT	  <<  16) |(GPIO17_DATAOUT 	 <<  17) |(GPIO18_DIR	   <<  18) |(GPIO19_DATAOUT	  <<  19) |
	(GPIO20_DATAOUT	  <<  20) |(GPIO21_DATAOUT 	 <<  21) |(GPIO22_DATAOUT		<<	22) |(GPIO23_DATAOUT	   <<  23) |
	(GPIO24_DATAOUT	  <<  24) |(GPIO25_DATAOUT 	 <<  25) |(GPIO26_DATAOUT		<< 26) |(GPIO27_DATAOUT	  << 27) |
	(GPIO28_DATAOUT	  << 28) |(GPIO29_DATAOUT		<< 29) |(GPIO30_DATAOUT	  << 30) |(GPIO31_DATAOUT	   << 31)),

	((GPIO32_DATAOUT	   <<  0) |(GPIO33_DATAOUT 	 <<  1) |(GPIO34_DATAOUT	   <<  2) |(GPIO35_DATAOUT 	 <<  3) |
	(GPIO36_DATAOUT	  <<  4) |(GPIO37_DATAOUT		<<	5) |(GPIO38_DATAOUT	  <<  6) |(GPIO39_DATAOUT		<<	7) |
	(GPIO40_DATAOUT	  <<  8) |(GPIO41_DATAOUT		<<	9) |(GPIO42_DATAOUT	  << 10) |(GPIO43_DATAOUT		<< 11) |
	(GPIO44_DATAOUT	  << 12) |(GPIO45_DATAOUT		<< 13) |(GPIO46_DATAOUT	  << 14) |(GPIO47_DATAOUT		<< 15) |
	(GPIO48_DATAOUT	  << 16) |(GPIO49_DATAOUT		<< 17) |(GPIO50_DATAOUT	  << 18) |(GPIO51_DATAOUT		<< 19) |
	(GPIO52_DATAOUT	  << 20) |(GPIO53_DATAOUT		<< 21) |(GPIO54_DATAOUT	  << 22) |(GPIO55_DATAOUT		<< 23) |
	(GPIO56_DATAOUT	  << 24) |(GPIO57_DATAOUT		<< 25) |(GPIO58_DATAOUT	  << 26) |(GPIO59_DATAOUT		<< 27) |
	(GPIO60_DATAOUT	  << 28) |(GPIO61_DATAOUT		<< 29) |(GPIO62_DATAOUT	  << 30) |(GPIO63_DATAOUT		<< 31)),

	((GPIO64_DATAOUT	   <<  0) |(GPIO65_DATAOUT 	 <<  1) |(GPIO66_DATAOUT	   <<  2) |(GPIO67_DATAOUT 	 <<  3) |
	(GPIO68_DATAOUT	  <<  4) |(GPIO69_DATAOUT		<<	5) |(GPIO70_DATAOUT	  <<  6) |(GPIO71_DATAOUT		<<	7) |
	(GPIO72_DATAOUT	  <<  8) |(GPIO73_DATAOUT		<<	9) |(GPIO74_DATAOUT	  << 10) |(GPIO75_DATAOUT		<< 11) |
	(GPIO76_DATAOUT	  << 12) |(GPIO77_DATAOUT		<< 13) |(GPIO78_DATAOUT	  << 14) |(GPIO79_DATAOUT		<< 15) |
	(GPIO80_DATAOUT	  << 16) |(GPIO81_DATAOUT		<< 17) |(GPIO82_DATAOUT	  << 18) |(GPIO83_DATAOUT		<< 19) |
	(GPIO84_DATAOUT	  << 20) |(GPIO85_DATAOUT	   << 21) |(GPIO86_DATAOUT 	 << 22) |(GPIO87_DATAOUT	   << 23) |
	(GPIO88_DATAOUT	  << 24) |(GPIO89_DATAOUT		<< 25) |(GPIO90_DATAOUT	  << 26) |(GPIO91_DATAOUT		<< 27) |
	(GPIO92_DATAOUT	  << 28) |(GPIO93_DATAOUT		<< 29) |(GPIO94_DATAOUT	  << 30) |(GPIO95_DATAOUT		<< 31)),
	
	((GPIO96_DATAOUT	   <<  0) |(GPIO97_DATAOUT 	 <<  1) |(GPIO98_DATAOUT	   <<  2) |(GPIO99_DATAOUT 	 <<  3) |
	(GPIO100_DATAOUT	  <<  4) |(GPIO101_DATAOUT 	<<	5) |(GPIO102_DATAOUT	  <<  6) |(GPIO103_DATAOUT 	<<	7) |
	(GPIO104_DATAOUT	  <<  8) |(GPIO105_DATAOUT 	<<	9) |(GPIO106_DATAOUT	  << 10) |(GPIO107_DATAOUT 	<< 11) |
	(GPIO108_DATAOUT	  << 12) |(GPIO109_DATAOUT 	<< 13) |(GPIO110_DATAOUT	  << 14) |(GPIO111_DATAOUT 	<< 15) |
	(GPIO112_DATAOUT	  << 16) |(GPIO113_DATAOUT    << 17) |(GPIO114_DATAOUT		<< 18) |(GPIO115_DATAOUT	 <<  19) |
	(GPIO116_DATAOUT	 <<  20) |(GPIO117_DATAOUT   << 21) |(GPIO118_DATAOUT    <<  22) |(GPIO119_DATAOUT	<< 23) |
	(GPIO120_DATAOUT	 <<  24) |(GPIO121_DATAOUT   <<  25) |(GPIO122_DATAOUT	   << 26) |(GPIO123_DATAOUT	<< 27) |
	(GPIO124_DATAOUT	 << 28) |(GPIO125_DATAOUT	  << 29) |(GPIO126_DATAOUT    << 30) |(GPIO127_DATAOUT	<< 31)),

	((GPIO128_DATAOUT	  <<  0) |(GPIO129_DATAOUT    <<  1) |(GPIO130_DATAOUT		<<	2) |(GPIO131_DATAOUT	 <<  3) |
	(GPIO132_DATAOUT	 <<  4) |(GPIO133_DATAOUT	  <<  5) |(GPIO134_DATAOUT    <<  6) |(GPIO135_DATAOUT	<<	7) |
	(GPIO136_DATAOUT	 <<  8) |(GPIO137_DATAOUT	  <<  9) |(GPIO138_DATAOUT    << 10) |(GPIO139_DATAOUT	<< 11) |
	(GPIO140_DATAOUT	 << 12) |(GPIO141_DATAOUT	  << 13) |(GPIO142_DATAOUT    << 14) |(GPIO143_DATAOUT	<< 15) |
	(GPIO144_DATAOUT	  << 16)),

}; /*end of gpio_init_dout_data*/
/*----------------------------------------------------------------------------*/
u32 gpio_init_mode_data[] = {
	((GPIO0_MODE      <<  0) |(GPIO1_MODE      <<  3) |(GPIO2_MODE      <<  6) |(GPIO3_MODE      <<  9) |(GPIO4_MODE      << 12) |
	(GPIO5_MODE      <<  15) |(GPIO6_MODE      <<  18) |(GPIO7_MODE      <<  21) |(GPIO8_MODE      <<  24) |(GPIO9_MODE      << 27)),
	((GPIO10_MODE     <<  0) |(GPIO11_MODE     <<  3) |(GPIO12_MODE     <<  6) |(GPIO13_MODE     <<  9) |(GPIO14_MODE     << 12) |
	(GPIO15_MODE     <<  15) |(GPIO16_MODE     <<  18) |(GPIO17_MODE     <<  21) |(GPIO18_MODE     <<  24) |(GPIO19_MODE     << 27)),
	((GPIO20_MODE     <<  0) |(GPIO21_MODE     <<  3) |(GPIO22_MODE     <<  6) |(GPIO23_MODE     <<  9) |(GPIO24_MODE     << 12) |
	(GPIO25_MODE     <<  15) |(GPIO26_MODE     <<  18) |(GPIO27_MODE     <<  21) |(GPIO28_MODE     <<  24) |(GPIO29_MODE     << 27)),
	((GPIO30_MODE     <<  0) |(GPIO31_MODE     <<  3) |(GPIO32_MODE     <<  6) |(GPIO33_MODE     <<  9) |(GPIO34_MODE     << 12) |
	(GPIO35_MODE     <<  15) |(GPIO36_MODE     <<  18) |(GPIO37_MODE     << 21) |(GPIO38_MODE     <<  24) |(GPIO39_MODE     << 27)),
	((GPIO40_MODE     <<  0) |(GPIO41_MODE     <<  3) |(GPIO42_MODE     <<  6) |(GPIO43_MODE     <<  9) |(GPIO44_MODE     << 12) |
	(GPIO45_MODE     <<  15) |(GPIO46_MODE     <<  18) |(GPIO47_MODE     <<  21) |(GPIO48_MODE     <<  24) |(GPIO49_MODE     << 27)),
	((GPIO50_MODE     <<  0) |(GPIO51_MODE     <<  3) |(GPIO52_MODE     <<  6) |(GPIO53_MODE     <<  9) |(GPIO54_MODE     << 12) |
	(GPIO55_MODE     <<  15) |(GPIO56_MODE     <<  18) |(GPIO57_MODE     <<  21) |(GPIO58_MODE     <<  24) |(GPIO59_MODE     << 27)),
	((GPIO60_MODE     <<  0) |(GPIO61_MODE     <<  3) |(GPIO62_MODE     <<  6) |(GPIO63_MODE     <<  9) |(GPIO64_MODE     << 12) |
	(GPIO65_MODE     <<  15) |(GPIO66_MODE     <<  18) |(GPIO67_MODE     << 21) |(GPIO68_MODE     << 24) |(GPIO69_MODE     << 27)),
	((GPIO70_MODE     <<  0) |(GPIO71_MODE     <<  3) |(GPIO72_MODE     <<  6) |(GPIO73_MODE     <<  9) |(GPIO74_MODE     << 12) |
	(GPIO75_MODE     << 15) |(GPIO76_MODE     <<  18) |(GPIO77_MODE     <<  21) |(GPIO78_MODE     <<  24) |(GPIO79_MODE     << 27)),
	((GPIO80_MODE     <<  0) |(GPIO81_MODE     <<  3) |(GPIO82_MODE     <<  6) |(GPIO83_MODE     <<  9) |(GPIO84_MODE     << 12) |
	(GPIO85_MODE     <<  15) |(GPIO86_MODE     << 18) |(GPIO87_MODE     <<  21) |(GPIO88_MODE     <<  24) |(GPIO89_MODE     << 27)),
	((GPIO90_MODE     <<  0) |(GPIO91_MODE     <<  3) |(GPIO92_MODE     <<  6) |(GPIO93_MODE     <<  9) |(GPIO94_MODE     << 12) |
	(GPIO95_MODE     <<  15) |(GPIO96_MODE     <<  18) |(GPIO97_MODE     <<  21) |(GPIO98_MODE     << 24) |(GPIO99_MODE     << 27)),
	((GPIO100_MODE    <<  0) |(GPIO101_MODE    <<  3) |(GPIO102_MODE    <<  6) |(GPIO103_MODE    <<  9) |(GPIO104_MODE    << 12) |
	(GPIO105_MODE    <<  15) |(GPIO106_MODE    <<  18) |(GPIO107_MODE    <<  21) |(GPIO108_MODE    <<  24) |(GPIO109_MODE    << 27)),
	((GPIO110_MODE    <<  0) |(GPIO111_MODE    <<  3) |(GPIO112_MODE    <<  6) |(GPIO113_MODE    <<  9) |(GPIO114_MODE    << 12) |
	(GPIO115_MODE    <<  15) |(GPIO116_MODE    <<  18) |(GPIO117_MODE    <<  21) |(GPIO118_MODE    << 24) |(GPIO119_MODE    << 27)),
	((GPIO120_MODE    <<  0) |(GPIO121_MODE    <<  3) |(GPIO122_MODE    <<  6) |(GPIO123_MODE    <<  9) |(GPIO124_MODE    << 12) |
	(GPIO125_MODE    <<  15) |(GPIO126_MODE    <<  18) |(GPIO127_MODE    <<  21) |(GPIO128_MODE    <<  24) |(GPIO129_MODE    << 27)),
	((GPIO130_MODE    <<  0) |(GPIO131_MODE    <<  3) |(GPIO132_MODE    <<  6) |(GPIO133_MODE    <<  9) |(GPIO134_MODE    << 12) |
	(GPIO135_MODE    <<  15) |(GPIO136_MODE    <<  18) |(GPIO137_MODE    <<  21) |(GPIO138_MODE    <<  24) |(GPIO139_MODE    << 27)),
	((GPIO140_MODE    <<  0) |(GPIO141_MODE    <<  3) |(GPIO142_MODE    <<  6) |(GPIO143_MODE    <<  9) |(GPIO144_MODE    << 12)),
}; /*end of gpio_init_mode_more_data*/


#define ARRAYSIZE(array) (sizeof(array)/sizeof(array[0]))

/*----------------------------------------------------------------------------*/
void mt_gpio_set_default_chip(void)
{
	unsigned int idx;
	u32 val;

	for (idx = 0; idx < ARRAYSIZE(gpio_init_dir_data); idx++) {
		val = gpio_init_dir_data[idx];
		GPIO_WR32(GPIO_BASE + 0x0140 + 0x10 * idx, val);
	}

	for (idx = 0; idx < ARRAYSIZE(gpio_init_pullen_data); idx++) {
		val = gpio_init_pullen_data[idx];
		GPIO_WR32(GPIO_BASE + 0x0860 + 0x10 * idx, val);
	}
	for (idx = 0; idx < ARRAYSIZE(gpio_init_pullsel_data); idx++) {
		val = gpio_init_pullsel_data[idx];
		GPIO_WR32(GPIO_BASE + 0x0900 + 0x10 * idx,val);
	}

	//msdc pin
	for (idx = 0; idx < ARRAYSIZE(msdc_pull_data); idx++) {
		mt_set_gpio_pull_select(msdc_pull_data[idx].pin, msdc_pull_data[idx].pullsel);
		mt_set_gpio_pull_enable(msdc_pull_data[idx].pin, msdc_pull_data[idx].pullen);
	}

	for (idx = 0; idx < ARRAYSIZE(gpio_init_dout_data); idx++) {
		val = gpio_init_dout_data[idx];
		GPIO_WR32(GPIO_BASE+0x00A0 + 0x10 * idx,val);
	}
	for (idx = 0; idx < ARRAYSIZE(gpio_init_mode_data); idx++) {
		val = gpio_init_mode_data[idx];
		GPIO_WR32(GPIO_BASE+0x01E0 + 0x10 * idx,val);
	}
}

void mt_gpio_set_default(void)
{
	mt_gpio_set_default_chip();
	return;
}

