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

#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <platform/bq24296.h>
#include <printf.h>

//#include <target/cust_charging.h>

int g_bq24296_log_en=0;
//static struct mt_i2c_t bq24296_i2c;

/**********************************************************
  *
  *   [I2C Slave Setting]
  *
  *********************************************************/
#define bq24296_SLAVE_ADDR_WRITE   0xD6
#define bq24296_SLAVE_ADDR_Read    0xD7

/**********************************************************
  *
  *   [Global Variable]
  *
  *********************************************************/
#define bq24296_REG_NUM 11
kal_uint8 bq24296_reg[bq24296_REG_NUM] = {0};

/**********************************************************
  *
  *   [I2C Function For Read/Write bq24296]
  *
  *********************************************************/
#define BQ24296_I2C_ID  I2C1
static struct mt_i2c_t bq24296_i2c;

kal_uint32 bq24296_write_byte(kal_uint8 addr, kal_uint8 value)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;

	write_data[0]= addr;
	write_data[1] = value;

	bq24296_i2c.id = BQ24296_I2C_ID;
	/* Since i2c will left shift 1 bit, we need to set BQ24296 I2C address to >>1 */
	bq24296_i2c.addr = (bq24296_SLAVE_ADDR_WRITE >> 1);
	bq24296_i2c.mode = ST_MODE;
	bq24296_i2c.speed = 100;
	len = 2;

	ret_code = i2c_write(&bq24296_i2c, write_data, len);

	if (I2C_OK != ret_code)
		dprintf(INFO, "%s: i2c_write: ret_code: %d\n", __func__, ret_code);

	return ret_code;
}

kal_uint32 bq24296_read_byte (kal_uint8 addr, kal_uint8 *dataBuffer)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint16 len;
	*dataBuffer = addr;

	bq24296_i2c.id = BQ24296_I2C_ID;
	/* Since i2c will left shift 1 bit, we need to set BQ24296 I2C address to >>1 */
	bq24296_i2c.addr = (bq24296_SLAVE_ADDR_Read >> 1);
	bq24296_i2c.mode = ST_MODE;
	bq24296_i2c.speed = 100;
	len = 1;

	ret_code = i2c_write_read(&bq24296_i2c, dataBuffer, len, len);

	if (I2C_OK != ret_code)
		dprintf(INFO, "%s: i2c_read: ret_code: %d\n", __func__, ret_code);

	return ret_code;
}

/**********************************************************
  *
  *   [Read / Write Function]
  *
  *********************************************************/
kal_uint32 bq24296_read_interface (kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	kal_uint8 bq24296_reg = 0;
	int ret = 0;

	dprintf(INFO, "--------------------------------------------------LK\n");

	ret = bq24296_read_byte(RegNum, &bq24296_reg);
	dprintf(INFO, "[bq24296_read_interface] Reg[%x]=0x%x\n", RegNum, bq24296_reg);

	bq24296_reg &= (MASK << SHIFT);
	*val = (bq24296_reg >> SHIFT);

	if (g_bq24296_log_en>1)
		dprintf(INFO, "%d\n", ret);

	return ret;
}

kal_uint32 bq24296_config_interface (kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	kal_uint8 bq24296_reg = 0;
	kal_uint32 ret = 0;

	dprintf(INFO, "--------------------------------------------------LK\n");

	ret = bq24296_read_byte(RegNum, &bq24296_reg);

	bq24296_reg &= ~(MASK << SHIFT);
	bq24296_reg |= (val << SHIFT);

	ret = bq24296_write_byte(RegNum, bq24296_reg);

	dprintf(INFO, "[bq24296_config_interface] write Reg[%x]=0x%x\n", RegNum, bq24296_reg);

	// Check
	//bq24296_read_byte(RegNum, &bq24296_reg);
	//dprintf(INFO, "[bq24296_config_interface] Check Reg[%x]=0x%x\n", RegNum, bq24296_reg);

	if (g_bq24296_log_en>1)
		dprintf(INFO, "%d\n", ret);

	return ret;
}

/**********************************************************
  *
  *   [Internal Function]
  *
  *********************************************************/
//CON0----------------------------------------------------

void bq24296_set_en_hiz(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON0),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON0_EN_HIZ_MASK),
	                                (kal_uint8)(CON0_EN_HIZ_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_vindpm(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON0),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON0_VINDPM_MASK),
	                                (kal_uint8)(CON0_VINDPM_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_iinlim(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON0),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON0_IINLIM_MASK),
	                                (kal_uint8)(CON0_IINLIM_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

//CON1----------------------------------------------------

void bq24296_set_reg_rst(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON1),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON1_REG_RST_MASK),
	                                (kal_uint8)(CON1_REG_RST_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_wdt_rst(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON1),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON1_WDT_RST_MASK),
	                                (kal_uint8)(CON1_WDT_RST_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_chg_config(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON1),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON1_CHG_CONFIG_MASK),
	                                (kal_uint8)(CON1_CHG_CONFIG_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_sys_min(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON1),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON1_SYS_MIN_MASK),
	                                (kal_uint8)(CON1_SYS_MIN_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_boost_lim(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON1),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON1_BOOST_LIM_MASK),
	                                (kal_uint8)(CON1_BOOST_LIM_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

//CON2----------------------------------------------------

void bq24296_set_ichg(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON2),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON2_ICHG_MASK),
	                                (kal_uint8)(CON2_ICHG_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

//CON3----------------------------------------------------

void bq24296_set_iprechg(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON3),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON3_IPRECHG_MASK),
	                                (kal_uint8)(CON3_IPRECHG_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_iterm(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON3),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON3_ITERM_MASK),
	                                (kal_uint8)(CON3_ITERM_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

//CON4----------------------------------------------------

void bq24296_set_vreg(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON4),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON4_VREG_MASK),
	                                (kal_uint8)(CON4_VREG_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_batlowv(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON4),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON4_BATLOWV_MASK),
	                                (kal_uint8)(CON4_BATLOWV_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_vrechg(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON4),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON4_VRECHG_MASK),
	                                (kal_uint8)(CON4_VRECHG_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

//CON5----------------------------------------------------

void bq24296_set_en_term(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON5),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON5_EN_TERM_MASK),
	                                (kal_uint8)(CON5_EN_TERM_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_watchdog(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON5),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON5_WATCHDOG_MASK),
	                                (kal_uint8)(CON5_WATCHDOG_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_en_timer(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON5),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON5_EN_TIMER_MASK),
	                                (kal_uint8)(CON5_EN_TIMER_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_chg_timer(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON5),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON5_CHG_TIMER_MASK),
	                                (kal_uint8)(CON5_CHG_TIMER_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

//CON6----------------------------------------------------

void bq24296_set_treg(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON6),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON6_TREG_MASK),
	                                (kal_uint8)(CON6_TREG_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

//CON7----------------------------------------------------

void bq24296_set_tmr2x_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON7),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON7_TMR2X_EN_MASK),
	                                (kal_uint8)(CON7_TMR2X_EN_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_batfet_disable(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON7),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON7_BATFET_Disable_MASK),
	                                (kal_uint8)(CON7_BATFET_Disable_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

void bq24296_set_int_mask(kal_uint32 val)
{
	kal_uint32 ret=0;

	ret=bq24296_config_interface(   (kal_uint8)(bq24296_CON7),
	                                (kal_uint8)(val),
	                                (kal_uint8)(CON7_INT_MASK_MASK),
	                                (kal_uint8)(CON7_INT_MASK_SHIFT)
	                            );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);
}

//CON8----------------------------------------------------

kal_uint32 bq24296_get_system_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=bq24296_read_interface(     (kal_uint8)(bq24296_CON8),
	                                (&val),
	                                (kal_uint8)(0xFF),
	                                (kal_uint8)(0x0)
	                          );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);

	return val;
}

kal_uint32 bq24296_get_vbus_stat(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=bq24296_read_interface(     (kal_uint8)(bq24296_CON8),
	                                (&val),
	                                (kal_uint8)(CON8_VBUS_STAT_MASK),
	                                (kal_uint8)(CON8_VBUS_STAT_SHIFT)
	                          );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);

	return val;
}

kal_uint32 bq24296_get_chrg_stat(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=bq24296_read_interface(     (kal_uint8)(bq24296_CON8),
	                                (&val),
	                                (kal_uint8)(CON8_CHRG_STAT_MASK),
	                                (kal_uint8)(CON8_CHRG_STAT_SHIFT)
	                          );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);

	return val;
}

kal_uint32 bq24296_get_vsys_stat(void)
{
	kal_uint32 ret=0;
	kal_uint8 val=0;

	ret=bq24296_read_interface(     (kal_uint8)(bq24296_CON8),
	                                (&val),
	                                (kal_uint8)(CON8_VSYS_STAT_MASK),
	                                (kal_uint8)(CON8_VSYS_STAT_SHIFT)
	                          );
	if (g_bq24296_log_en>1)
		printf("%d\n", ret);

	return val;
}

/**********************************************************
  *
  *   [Internal Function]
  *
  *********************************************************/
void bq24296_dump_register(void)
{
	int i=0;

	dprintf(CRITICAL, "bq24296_dump_register\r\n");
	for (i=0; i<bq24296_REG_NUM; i++) {
		bq24296_read_byte(i, &bq24296_reg[i]);
		dprintf(CRITICAL, "[0x%x]=0x%x\r\n", i, bq24296_reg[i]);
	}
}
void bq24296_hw_init(void)
{
	mt6392_upmu_set_rg_bc11_bb_ctrl(1);    //BC11_BB_CTRL
	mt6392_upmu_set_rg_bc11_rst(1);        //BC11_RST

#if 0 //no use
	//pull PSEL low
	mt_set_gpio_mode(GPIO_CHR_PSEL_PIN,GPIO_MODE_GPIO);
	mt_set_gpio_dir(GPIO_CHR_PSEL_PIN,GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CHR_PSEL_PIN,GPIO_OUT_ZERO);
#endif

	//pull CE low
#if 0 //If dws default config is high, Need init GPIO_CHR_CE_PIN high first
	mt_set_gpio_mode(GPIO_CHR_CE_PIN,GPIO_MODE_GPIO);
	mt_set_gpio_dir(GPIO_CHR_CE_PIN,GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CHR_CE_PIN,GPIO_OUT_ZERO);
#endif

	bq24296_set_en_hiz(0x0);
	bq24296_set_vindpm(0xA); //VIN DPM check 4.68V
	bq24296_set_reg_rst(0x0);
	bq24296_set_wdt_rst(0x1); //Kick watchdog

	if (0)
		bq24296_set_sys_min(0x0); //Minimum system voltage 3.0V (MT6320 E1 workaround, disable powerpath)
	else
		bq24296_set_sys_min(0x5); //Minimum system voltage 3.5V
	bq24296_set_iprechg(0x3); //Precharge current 512mA
	bq24296_set_iterm(0x0); //Termination current 128mA

	bq24296_set_batlowv(0x1); //BATLOWV 3.0V
	bq24296_set_vrechg(0x0); //VRECHG 0.1V (4.108V)
	bq24296_set_en_term(0x1); //Enable termination
	bq24296_set_watchdog(0x1); //WDT 40s
	bq24296_set_en_timer(0x0); //Disable charge timer
	bq24296_set_int_mask(0x0); //Disable fault interrupt
}

void bq24296_turn_on_charging(void)
{
	CHARGER_TYPE CHR_Type_num = CHARGER_UNKNOWN;
	CHR_Type_num  = mt_charger_type_detection();
	if (CHR_Type_num == STANDARD_HOST) {
		bq24296_set_iinlim(0x2); //IN current limit at 500mA
	} else if (CHR_Type_num == NONSTANDARD_CHARGER) {
		bq24296_set_iinlim(0x6); //IN current limit at 2A
	} else if (CHR_Type_num == STANDARD_CHARGER) {
		bq24296_set_iinlim(0x6); //IN current limit at 2A
	} else if (CHR_Type_num == CHARGING_HOST) {
		bq24296_set_iinlim(0x6); //IN current limit at 2A
	} else {
		bq24296_set_iinlim(0x2); //IN current limit at 500mA
	}
	bq24296_set_en_hiz(0x0);
	bq24296_set_chg_config(0x1); // charger enable
	printf("[BATTERY:bq24296] charger enable !\r\n");
}
