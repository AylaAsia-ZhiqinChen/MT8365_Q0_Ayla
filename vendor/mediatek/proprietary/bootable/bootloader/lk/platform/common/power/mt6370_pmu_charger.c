/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */

/* MediaTek Inc. (C) 2015. All rights reserved.
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
 * have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
 * applicable license agreements with MediaTek Inc.
 */

#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <platform/errno.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <platform/mt_gpt.h>
#include <libfdt.h>
#include <printf.h>
#include <string.h>
#include "mt6370_pmu_charger.h"
#include "mtk_charger_intf.h"
#include "mtk_charger.h"
#include <lk_builtin_dtb.h>

#define MT6370_PMU_CHARGER_LK_DRV_VERSION "1.1.2_MTK"

/* ================= */
/* Internal variable */
/* ================= */

struct mt6370_pmu_charger_info {
	struct mtk_charger_info mchr_info;
	struct mt_i2c_t i2c;
	int i2c_log_level;
	unsigned int hidden_mode_cnt;
	unsigned char vendor_id;
	u32 ichg;
	u32 ichg_dis_chg;
	bool chg_en;
};

enum mt6370_charging_status {
	MT6370_CHG_STATUS_READY = 0,
	MT6370_CHG_STATUS_PROGRESS,
	MT6370_CHG_STATUS_DONE,
	MT6370_CHG_STATUS_FAULT,
	MT6370_CHG_STATUS_MAX,
};

/* Charging status name */
static const char *mt6370_chg_status_name[MT6370_CHG_STATUS_MAX] = {
	"ready", "progress", "done", "fault",
};

static const unsigned char mt6370_reg_en_hidden_mode[] = {
	MT6370_PMU_REG_HIDDENPASCODE1,
	MT6370_PMU_REG_HIDDENPASCODE2,
	MT6370_PMU_REG_HIDDENPASCODE3,
	MT6370_PMU_REG_HIDDENPASCODE4,
};

static const unsigned char mt6370_val_en_hidden_mode[] = {
	0x96, 0x69, 0xC3, 0x3C,
};

static const unsigned char mt6370_val_en_test_mode[] = {
	0x69, 0x96, 0x63, 0x70,
};

/* ======================= */
/* Address & Default value */
/* ======================= */


static const unsigned char mt6370_chg_reg_addr[] = {
	MT6370_PMU_REG_CHGCTRL1,
	MT6370_PMU_REG_CHGCTRL2,
	MT6370_PMU_REG_CHGCTRL3,
	MT6370_PMU_REG_CHGCTRL4,
	MT6370_PMU_REG_CHGCTRL5,
	MT6370_PMU_REG_CHGCTRL6,
	MT6370_PMU_REG_CHGCTRL7,
	MT6370_PMU_REG_CHGCTRL8,
	MT6370_PMU_REG_CHGCTRL9,
	MT6370_PMU_REG_CHGCTRL10,
	MT6370_PMU_REG_CHGCTRL11,
	MT6370_PMU_REG_CHGCTRL12,
	MT6370_PMU_REG_CHGCTRL13,
	MT6370_PMU_REG_CHGCTRL14,
	MT6370_PMU_REG_CHGCTRL15,
	MT6370_PMU_REG_CHGCTRL16,
	MT6370_PMU_REG_CHGADC,
	MT6370_PMU_REG_DEVICETYPE,
	MT6370_PMU_REG_QCCTRL1,
	MT6370_PMU_REG_QCCTRL2,
	MT6370_PMU_REG_QC3P0CTRL1,
	MT6370_PMU_REG_QC3P0CTRL2,
	MT6370_PMU_REG_USBSTATUS1,
	MT6370_PMU_REG_QCSTATUS1,
	MT6370_PMU_REG_QCSTATUS2,
	MT6370_PMU_REG_CHGPUMP,
	MT6370_PMU_REG_CHGCTRL17,
	MT6370_PMU_REG_CHGCTRL18,
	MT6370_PMU_REG_CHGDIRCHG1,
	MT6370_PMU_REG_CHGDIRCHG2,
	MT6370_PMU_REG_CHGDIRCHG3,
	MT6370_PMU_REG_CHGSTAT,
	MT6370_PMU_REG_CHGNTC,
	MT6370_PMU_REG_ADCDATAH,
	MT6370_PMU_REG_ADCDATAL,
	MT6370_PMU_REG_CHGCTRL19,
	MT6370_PMU_REG_CHGSTAT1,
	MT6370_PMU_REG_CHGSTAT2,
	MT6370_PMU_REG_CHGSTAT3,
	MT6370_PMU_REG_CHGSTAT4,
	MT6370_PMU_REG_CHGSTAT5,
	MT6370_PMU_REG_CHGSTAT6,
	MT6370_PMU_REG_QCSTAT,
	MT6370_PMU_REG_DICHGSTAT,
	MT6370_PMU_REG_OVPCTRLSTAT,
};


enum mt6370_iin_limit_sel {
	MT6370_IIMLMTSEL_AICR_3250 = 0,
	MT6370_IIMLMTSEL_CHR_TYPE,
	MT6370_IINLMTSEL_AICR,
	MT6370_IINLMTSEL_LOWER_LEVEL, /* lower of above three */
};


/* ========================= */
/* I2C operations */
/* ========================= */

static int mt6370_i2c_write_byte(struct mt6370_pmu_charger_info *info, u8 cmd,
	u8 data)
{
	unsigned int ret = I2C_OK;
	unsigned char write_buf[2] = {cmd, data};
	struct mt_i2c_t *i2c = &info->i2c;

	ret = i2c_write(i2c, write_buf, 2);

	if (ret != I2C_OK)
		dprintf(CRITICAL,
			"%s: I2CW[0x%02X] = 0x%02X failed, code = %d\n",
			__func__, cmd, data, ret);
	else
		dprintf(info->i2c_log_level, "%s: I2CW[0x%02X] = 0x%02X\n",
			__func__, cmd, data);

	return ret;
}

static int mt6370_i2c_read_byte(struct mt6370_pmu_charger_info *info, u8 cmd,
	u8 *data)
{
	int ret = I2C_OK;
	u8 ret_data = cmd;
	struct mt_i2c_t *i2c = &info->i2c;

	ret = i2c_write_read(i2c, &ret_data, 1, 1);

	if (ret != I2C_OK)
		dprintf(CRITICAL, "%s: I2CR[0x%02X] failed, code = %d\n",
			__func__, cmd, ret);
	else {
		dprintf(info->i2c_log_level, "%s: I2CR[0x%02X] = 0x%02X\n",
			__func__, cmd, ret_data);
		*data = ret_data;
	}

	return ret;
}

static int mt6370_i2c_block_write(struct mt6370_pmu_charger_info *info, u8 cmd,
	int len, const u8 *data)
{
	unsigned char write_buf[len + 1];
	struct mt_i2c_t *i2c = &info->i2c;

	write_buf[0] = cmd;
	memcpy(&write_buf[1], data, len);

	return i2c_write(i2c, write_buf, len + 1);
}

static int mt6370_i2c_block_read(struct mt6370_pmu_charger_info *info, u8 cmd,
	int len, u8 *data)
{
	struct mt_i2c_t *i2c = &info->i2c;

	data[0] = cmd;

	return i2c_write_read(i2c, data, 1, len);
}

static int mt6370_i2c_test_bit(struct mt6370_pmu_charger_info *info, u8 cmd,
	u8 shift)
{
	int ret = 0;
	u8 data = 0;

	ret = mt6370_i2c_read_byte(info, cmd, &data);
	if (ret != I2C_OK)
		return ret;

	ret = data & (1 << shift);

	return ret;
}

static int mt6370_i2c_update_bits(struct mt6370_pmu_charger_info *info, u8 cmd,
	u8 mask, u8 data)
{
	int ret = 0;
	u8 reg_data = 0;

	ret = mt6370_i2c_read_byte(info, cmd, &reg_data);
	if (ret != I2C_OK)
		return ret;

	reg_data = reg_data & 0xFF;
	reg_data &= ~mask;
	reg_data |= (data & mask);

	return mt6370_i2c_write_byte(info, cmd, reg_data);
}

static inline int mt6370_set_bit(struct mt6370_pmu_charger_info *info, u8 reg,
	u8 mask)
{
    return mt6370_i2c_update_bits(info, reg, mask, mask);
}

static inline int mt6370_clr_bit(struct mt6370_pmu_charger_info *info, u8 reg,
	u8 mask)
{
    return mt6370_i2c_update_bits(info, reg, mask, 0x00);
}

/* ================== */
/* internal functions */
/* ================== */
static int mt_charger_set_ichg(struct mtk_charger_info *mchr_info, u32 ichg);
static int mt_charger_set_aicr(struct mtk_charger_info *mchr_info, u32 aicr);
static int mt_charger_set_mivr(struct mtk_charger_info *mchr_info, u32 mivr);
static int mt_charger_get_ichg(struct mtk_charger_info *mchr_info, u32 *ichg);
static int mt_charger_get_aicr(struct mtk_charger_info *mchr_info, u32 *aicr);

static u8 mt6370_find_closest_reg_value(const u32 min, const u32 max,
	const u32 step, const u32 num, const u32 target)
{
	u32 i = 0, cur_val = 0, next_val = 0;

	/* Smaller than minimum supported value, use minimum one */
	if (target < min)
		return 0;

	for (i = 0; i < num - 1; i++) {
		cur_val = min + i * step;
		next_val = cur_val + step;

		if (cur_val > max)
			cur_val = max;

		if (next_val > max)
			next_val = max;

		if (target >= cur_val && target < next_val)
			return i;
	}

	/* Greater than maximum supported value, use maximum one */
	return num - 1;
}

static u8 mt6370_find_closest_reg_value_via_table(const u32 *value_table,
	const u32 table_size, const u32 target_value)
{
	u32 i = 0;

	/* Smaller than minimum supported value, use minimum one */
	if (target_value < value_table[0])
		return 0;

	for (i = 0; i < table_size - 1; i++) {
		if (target_value >= value_table[i] &&
		    target_value < value_table[i + 1])
			return i;
	}

	/* Greater than maximum supported value, use maximum one */
	return table_size - 1;
}

static u32 mt6370_find_closest_real_value(const u32 min, const u32 max,
	const u32 step, const u8 reg_val)
{
	u32 ret_val = 0;

	ret_val = min + reg_val * step;
	if (ret_val > max)
		ret_val = max;

	return ret_val;
}

static int mt6370_enable_hidden_mode(struct mt6370_pmu_charger_info *info,
	bool en)
{
	int ret = 0;

	if (en) {
		if (info->hidden_mode_cnt == 0) {
			ret = mt6370_i2c_block_write(info,
				mt6370_reg_en_hidden_mode[0],
				ARRAY_SIZE(mt6370_val_en_hidden_mode),
				mt6370_val_en_hidden_mode);
			if (ret != I2C_OK)
				goto err;
		}
		info->hidden_mode_cnt++;
	} else {
		if (info->hidden_mode_cnt == 1) /* last one */
			ret = mt6370_i2c_write_byte(info,
				mt6370_reg_en_hidden_mode[0], 0x00);
		info->hidden_mode_cnt--;
		if (ret < 0)
			goto err;
	}
	dprintf(CRITICAL, "%s: en = %d\n", __func__, en);
	return 0;

err:
	dprintf(CRITICAL, "%s: en = %d fail(%d)\n", __func__, en, ret);
	return ret;
}


/* Hardware pin current limit */
static int mt6370_enable_ilim(struct mt6370_pmu_charger_info *info, bool enable)
{
	int ret = 0;

	dprintf(CRITICAL, "%s: enable ilim = %d\n", __func__, enable);

	ret = (enable ? mt6370_set_bit : mt6370_clr_bit)
		(info, MT6370_PMU_REG_CHGCTRL3, MT6370_MASK_ILIM_EN);

	return ret;
}

/* Select IINLMTSEL to use AICR */
static int mt6370_select_input_current_limit(
	struct mt6370_pmu_charger_info *info, enum mt6370_iin_limit_sel sel)
{
	int ret = 0;

	dprintf(CRITICAL, "%s: select input current limit = %d\n",
		__func__, sel);

	ret = mt6370_i2c_update_bits(
		info,
		MT6370_PMU_REG_CHGCTRL2,
		MT6370_MASK_IINLMTSEL,
		sel << MT6370_SHIFT_IINLMTSEL
	);

	return ret;
}

static bool mt6370_is_hw_exist(struct mt6370_pmu_charger_info *info)
{
	int ret = 0;
	u8 vendor_id = 0, revision = 0;
	u8 data = 0;

	ret = mt6370_i2c_read_byte(info, MT6370_PMU_REG_DEVINFO, &data);
	if (ret != I2C_OK)
		return false;

	vendor_id = data & 0xF0;
	revision = data & 0x0F;

	if (vendor_id != MT6370_VENDOR_ID && vendor_id != 0xf0 &&
	    vendor_id != 0x90 && vendor_id != 0xB0) {
		dprintf(CRITICAL, "%s: vendor id is incorrect\n", __func__);
		return false;
	}
	dprintf(CRITICAL, "%s: E%d(0x%02X)\n", __func__, revision, revision);

	info->mchr_info.device_id = revision;
	info->vendor_id = vendor_id;
	return true;
}

#if 0
/* Set register's value to default */
static int mt6370_chg_reset_chip(struct mt6370_pmu_charger_info *info)
{
	int ret = 0;

	dprintf(CRITICAL, "%s: starts\n", __func__);

	ret = mt6370_set_bit(info, MT6370_PMU_REG_CORECTRL2,
		MT6370_MASK_CHG_RST);

	return ret;
}
#endif

static int mt6370_set_battery_voreg(struct mt6370_pmu_charger_info *info,
	u32 voreg)
{
	int ret = 0;
	u8 reg_voreg = 0;

	reg_voreg = mt6370_find_closest_reg_value(MT6370_BAT_VOREG_MIN,
		MT6370_BAT_VOREG_MAX, MT6370_BAT_VOREG_STEP,
		MT6370_BAT_VOREG_NUM, voreg);

	dprintf(CRITICAL, "%s: bat voreg = %d\n", __func__, voreg);

	ret = mt6370_i2c_update_bits(
		info,
		MT6370_PMU_REG_CHGCTRL4,
		MT6370_MASK_BAT_VOREG,
		reg_voreg << MT6370_SHIFT_BAT_VOREG
	);

	return ret;
}

static int mt6370_enable_wdt(struct mt6370_pmu_charger_info *info,
	bool en)
{
	int ret = 0;

	dprintf(CRITICAL, "%s: en = %d\n", __func__, en);
	ret = (en ? mt6370_set_bit : mt6370_clr_bit)
		(info, MT6370_PMU_REG_CHGCTRL13, MT6370_MASK_WDT_EN);

	return ret;
}

static int mt6370_check_icc_accuracy(struct mt6370_pmu_charger_info *info)
{
	int ret = 0, tm_try_leave_cnt = 2;
	u8 data;

	dprintf(CRITICAL, "%s\n", __func__);

	/* only need trim icc for MT6371 */
	if (info->vendor_id != 0xf0)
		return 0;

	/* check need icc trim or not */
	ret = mt6370_i2c_read_byte(info, MT6370_PMU_REG_TM_INF, &data);
	if (ret < 0)
		return ret;
	if (data & 0x40) {
		dprintf(CRITICAL, "%s: no need icc trim\n", __func__);
		return ret;
	}

	/* Enter test mode */
	ret = mt6370_i2c_block_write(info, 0xF0,
		ARRAY_SIZE(mt6370_val_en_test_mode), mt6370_val_en_test_mode);
	if (ret != I2C_OK) {
		dprintf(CRITICAL, "%s: enter test mode fail\n", __func__);
		return ret;
	}

	mdelay(1);

	ret = mt6370_i2c_write_byte(info, 0xFF, 0x69);
	if (ret != I2C_OK) {
		dprintf(CRITICAL, "%s: enter test mode part fail\n", __func__);
		goto out;
	}

	mdelay(1);

	/* update icc-1A trim code */
	ret = mt6370_i2c_read_byte(info, MT6370_TM_REG_ICC_1A, &data);
	if (ret != I2C_OK) {
		dprintf(CRITICAL, "%s: read icc trim fail\n", __func__);
		goto out;
	} else
		dprintf(CRITICAL, "%s: reg0x%02x = 0x%02x\n",
			__func__, MT6370_TM_REG_ICC_1A, data);

	if (data + 7 > 255)
		data = 255;
	else
		data += 7;

	ret = mt6370_i2c_write_byte(info, MT6370_TM_REG_ICC_1A, data);
	if (ret != I2C_OK) {
		dprintf(CRITICAL,
			"%s: write icc trim fail\n", __func__);
		goto out;
	}

	ret = mt6370_i2c_read_byte(info, MT6370_TM_REG_ICC_1A, &data);
	if (ret < 0)
		goto out;
	else
		dprintf(CRITICAL, "%s: read tm reg0x%02x is 0x%02x\n",
			__func__, MT6370_TM_REG_ICC_1A, data);

	/* update trim complete flag */
	ret = mt6370_set_bit(info, MT6370_TM_REG_TM_INF, 0x80);
	if (ret < 0)
		dprintf(CRITICAL,
			"%s: update trim complete flag fail\n", __func__);
out:
	do {
		ret = mt6370_i2c_write_byte(info, 0xF0, 0x00);
		if (ret != I2C_OK) {
			dprintf(CRITICAL, "%s: leave tm fail\n", __func__);
			tm_try_leave_cnt--;
		} else
			break;
	} while (tm_try_leave_cnt > 0);

	return ret;
}

static int mt6370_enable_vsys_short_protect(struct mt6370_pmu_charger_info *info)
{
	int ret = 0, tm_try_leave_cnt = 2;
	u8 vid = info->vendor_id;
	u8 chip_rev = info->mchr_info.device_id;

	if (vid == 0xe0 || (vid == 0xf0 && chip_rev >= 0x04))
		return 0;

	dprintf(CRITICAL, "%s\n", __func__);

	/* Enable I2C reset */
	ret = mt6370_set_bit(info, MT6370_PMU_REG_CORECTRL1, 0x80);
	if (ret != I2C_OK) {
		dprintf(CRITICAL, "%s: en i2c reset fail\n", __func__);
		return -EINVAL;
	}

	/* Enter test mode */
	ret = mt6370_i2c_block_write(info, 0xF0,
		ARRAY_SIZE(mt6370_val_en_test_mode), mt6370_val_en_test_mode);
	if (ret != I2C_OK) {
		dprintf(CRITICAL, "%s: enter test mode fail\n", __func__);
		return ret;
	}

	mdelay(1);

	ret = mt6370_i2c_write_byte(info, 0xFF, 0x69);
	if (ret != I2C_OK) {
		dprintf(CRITICAL, "%s: enter test mode part fail\n", __func__);
		goto out;
	}

	mdelay(1);

	/* set Power path short protect threshold 150mA */
	ret = mt6370_i2c_update_bits(info, 0x34, 0xC0, 0x80);
	if (ret != I2C_OK) {
		dprintf(CRITICAL, "%s: modify short protection fail\n", __func__);
		goto out;
	}

	mdelay(1);

out:
	do {
		ret = mt6370_i2c_write_byte(info, 0xF0, 0x00);
		if (ret != I2C_OK) {
			dprintf(CRITICAL, "%s: leave tm fail\n", __func__);
			tm_try_leave_cnt--;
		} else
			break;
	} while (tm_try_leave_cnt > 0);

	/* Disable I2C reset */
	ret = mt6370_clr_bit(info, MT6370_PMU_REG_CORECTRL1, 0x80);
	if (ret != I2C_OK)
		dprintf(CRITICAL, "%s: disable i2c reset fail\n", __func__);

	return ret;
}

static int mt6370_enable_jeita(struct mt6370_pmu_charger_info *info,
	bool en)
{
	int ret = 0;

	dprintf(CRITICAL, "%s: en = %d\n", __func__, en);
	ret = (en ? mt6370_set_bit : mt6370_clr_bit)
		(info, MT6370_PMU_REG_CHGCTRL16, MT6370_MASK_JEITA_EN);

	return ret;
}

static int mt6370_chg_init_setting(struct mt6370_pmu_charger_info *info)
{
	int ret = 0;

	dprintf(CRITICAL, "%s: starts\n", __func__);

	ret = mt6370_enable_wdt(info, true);
	if (ret < 0)
		dprintf(CRITICAL, "%s: enable wdt failed\n", __func__);

	ret = mt6370_enable_vsys_short_protect(info);
	if (ret < 0)
		dprintf(CRITICAL, "%s: enable vsys short protect fail\n",
			__func__);

	ret = mt6370_check_icc_accuracy(info);
	if (ret < 0)
		dprintf(CRITICAL, "%s: check icc accuracy fail\n", __func__);

	/* Select input current limit to referenced from AICR */
	ret = mt6370_select_input_current_limit(info,
		MT6370_IINLMTSEL_AICR);
	if (ret < 0)
		dprintf(CRITICAL, "%s: select input current limit failed\n",
			__func__);

	mdelay(5);

	/* Disable HW iinlimit, use SW */
	ret = mt6370_enable_ilim(info, false);
	if (ret < 0)
		dprintf(CRITICAL, "%s: disable ilim failed\n", __func__);

	ret = mt6370_enable_jeita(info, false);
	if (ret < 0)
		dprintf(CRITICAL, "%s: disable jeita failed\n", __func__);

	/* Set ichg 500mA for vsys overshoot */
	ret = mt_charger_set_ichg(&info->mchr_info, 500);
	if (ret < 0)
		dprintf(CRITICAL, "%s: set ichg failed\n", __func__);

	ret = mt_charger_set_aicr(&info->mchr_info, 500);
	if (ret < 0)
		dprintf(CRITICAL, "%s: set aicr failed\n", __func__);

	ret = mt_charger_set_mivr(&info->mchr_info, 4500);
	if (ret < 0)
		dprintf(CRITICAL, "%s: set mivr failed\n", __func__);

	ret = mt6370_set_battery_voreg(info, 4350);
	if (ret < 0)
		dprintf(CRITICAL, "%s: set cv failed\n", __func__);

	return ret;
}

static int mt6370_get_charging_status(struct mt6370_pmu_charger_info *info,
	enum mt6370_charging_status *chg_stat)
{
	int ret = 0;
	u8 data = 0;

	ret = mt6370_i2c_read_byte(info, MT6370_PMU_REG_CHGSTAT, &data);
	if (ret != I2C_OK)
		return ret;

	*chg_stat = (data & MT6370_MASK_CHG_STAT) >> MT6370_SHIFT_CHG_STAT;

	return ret;
}

static int mt6370_get_mivr(struct mt6370_pmu_charger_info *info, u32 *mivr)
{
	int ret = 0;
	u8 reg_mivr = 0;
	u8 data = 0;

	ret = mt6370_i2c_read_byte(info, MT6370_PMU_REG_CHGCTRL6, &data);
	if (ret != I2C_OK)
		return ret;
	reg_mivr = ((data & MT6370_MASK_MIVR) >> MT6370_SHIFT_MIVR) & 0xFF;

	*mivr = mt6370_find_closest_real_value(MT6370_MIVR_MIN, MT6370_MIVR_MAX,
		MT6370_MIVR_STEP, reg_mivr);


	return ret;
}

static int mt6370_is_charging_enable(struct mt6370_pmu_charger_info *info, bool *enable)
{
	int ret = 0;
	u8 data = 0;

	ret = mt6370_i2c_read_byte(info, MT6370_PMU_REG_CHGCTRL2, &data);
	if (ret != I2C_OK)
		return ret;

	if (((data & MT6370_MASK_CHG_EN) >> MT6370_SHIFT_CHG_EN) & 0xFF)
		*enable = true;
	else
		*enable = false;

	return ret;
}

static int mt6370_get_ieoc(struct mt6370_pmu_charger_info *info, u32 *ieoc)
{
	int ret = 0;
	u8 reg_ieoc = 0;
	u8 data = 0;

	ret = mt6370_i2c_read_byte(info, MT6370_PMU_REG_CHGCTRL9, &data);
	if (ret != I2C_OK)
		return ret;

	reg_ieoc = (data & MT6370_MASK_IEOC) >> MT6370_SHIFT_IEOC;
	*ieoc = mt6370_find_closest_real_value(MT6370_IEOC_MIN, MT6370_IEOC_MAX,
		MT6370_IEOC_STEP, reg_ieoc);

	return ret;
}


/* =========================================================== */
/* The following is implementation for interface of mt_charger */
/* =========================================================== */


static int mt_charger_dump_register(struct mtk_charger_info *mchr_info)
{
	int ret = 0;
	u32 i = 0;
	u32 ichg = 0, aicr = 0, mivr = 0, ieoc = 0;
	bool chg_enable = 0;
	enum mt6370_charging_status chg_status = MT6370_CHG_STATUS_READY;
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;
	u8 data = 0;

	ret = mt6370_get_charging_status(info, &chg_status);
	if (chg_status == MT6370_CHG_STATUS_FAULT) {
		info->i2c_log_level = CRITICAL;
		for (i = 0; i < ARRAY_SIZE(mt6370_chg_reg_addr); i++)
			ret = mt6370_i2c_read_byte(info, mt6370_chg_reg_addr[i],
				&data);
	} else
		info->i2c_log_level = INFO;

	ret = mt_charger_get_ichg(mchr_info, &ichg);
	ret = mt6370_get_mivr(info, &mivr);
	ret = mt_charger_get_aicr(mchr_info, &aicr);
	ret = mt6370_get_ieoc(info, &ieoc);
	ret = mt6370_is_charging_enable(info, &chg_enable);

	dprintf(CRITICAL,
		"%s: ICHG = %dmA, AICR = %dmA, MIVR = %dmV, IEOC = %dmA\n",
		__func__, ichg, aicr, mivr, ieoc);

	dprintf(CRITICAL, "%s: CHG_EN = %d, CHG_STATUS = %s\n",
		__func__, chg_enable, mt6370_chg_status_name[chg_status]);

	return ret;
}

static int mt_charger_enable_charging(struct mtk_charger_info *mchr_info,
	bool enable)
{
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;
	int ret = 0;
	u32 ichg_ramp_t = 0;

	if (info->chg_en == enable) {
		dprintf(CRITICAL,
			"%s: is the same, en = %d\n", __func__, enable);
		return 0;
	}

	mt6370_enable_hidden_mode(info, true);

	/* Workaround for avoid vsys overshoot when charge disable */
	if (!enable) {
		info->ichg_dis_chg = info->ichg;
		ichg_ramp_t = (info->ichg - 500) / 50 * 2;
		/* Set ichg 500mA */
		ret = mt6370_i2c_update_bits(
			info,
			MT6370_PMU_REG_CHGCTRL7,
			MT6370_MASK_ICHG,
			0x04 << MT6370_SHIFT_ICHG
		);
		if (ret < 0) {
			dprintf(CRITICAL,
				"%s: set ichg fail\n", __func__);
			goto out;
		}
		mdelay(ichg_ramp_t);
	} else {
		if (info->ichg == info->ichg_dis_chg) {
			ret = mt_charger_set_ichg(mchr_info, info->ichg);
			if (ret < 0) {
				dprintf(CRITICAL,
					"%s: set ichg fail\n", __func__);
				goto out;
			}
		}
	}

	/* Workaround for Iprechg 150mA drop to 40mA */
	if (!enable) {
		ret = mt6370_clr_bit(info, MT6370_PMU_REG_CHGHIDDENCTRL9, 0x80);
		if (ret < 0) {
			dprintf(CRITICAL,
				"%s: disable psk mode fail\n", __func__);
			goto out;
		}
	}

	ret = (enable ? mt6370_set_bit : mt6370_clr_bit)
		(info, MT6370_PMU_REG_CHGCTRL2, MT6370_MASK_CHG_EN);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: fail, en = %d\n", __func__, enable);
		goto out;
	}

	if (enable) {
		ret = mt6370_set_bit(info, MT6370_PMU_REG_CHGHIDDENCTRL9, 0x80);
		if (ret < 0)
			dprintf(CRITICAL,
				"%s: enable psk mode fail\n", __func__);
	}
out:
	mt6370_enable_hidden_mode(info, false);
	info->chg_en = enable;
	return ret;
}

static int mt_charger_enable_power_path(struct mtk_charger_info *mchr_info,
	bool enable)
{
	int ret = 0;
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;

	dprintf(CRITICAL, "%s: enable = %d\n", __func__, enable);
	ret = (enable ? mt6370_clr_bit : mt6370_set_bit)
		(info, MT6370_PMU_REG_CHGCTRL1, MT6370_MASK_HZ_EN);

	return ret;
}

static int mt_charger_set_ichg(struct mtk_charger_info *mchr_info, u32 ichg)
{
	int ret = 0;
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;

	/* Find corresponding reg value */
	u8 reg_ichg = mt6370_find_closest_reg_value(MT6370_ICHG_MIN,
		MT6370_ICHG_MAX, MT6370_ICHG_STEP, MT6370_ICHG_NUM, ichg);

	dprintf(CRITICAL, "%s: ichg = %d\n", __func__, ichg);

	ret = mt6370_i2c_update_bits(
		info,
		MT6370_PMU_REG_CHGCTRL7,
		MT6370_MASK_ICHG,
		reg_ichg << MT6370_SHIFT_ICHG
	);
	if (ret < 0)
		dprintf(CRITICAL, "%s: fail(%d)\n", __func__, ret);
	else
		info->ichg = ichg;
	return ret;
}

static int mt_charger_set_aicr(struct mtk_charger_info *mchr_info, u32 aicr)
{
	int ret = 0;
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;

	/* Find corresponding reg value */
	u8 reg_aicr = mt6370_find_closest_reg_value(MT6370_AICR_MIN,
		MT6370_AICR_MAX, MT6370_AICR_STEP, MT6370_AICR_NUM, aicr);

	dprintf(CRITICAL, "%s: aicr = %d\n", __func__, aicr);

	ret = mt6370_i2c_update_bits(
		info,
		MT6370_PMU_REG_CHGCTRL3,
		MT6370_MASK_AICR,
		reg_aicr << MT6370_SHIFT_AICR
	);

	return ret;
}


static int mt_charger_set_mivr(struct mtk_charger_info *mchr_info, u32 mivr)
{
	int ret = 0;
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;

	/* Find corresponding reg value */
	u8 reg_mivr = mt6370_find_closest_reg_value(MT6370_MIVR_MIN,
		MT6370_MIVR_MAX, MT6370_MIVR_STEP, MT6370_MIVR_NUM, mivr);

	dprintf(CRITICAL, "%s: mivr = %d\n", __func__, mivr);

	ret = mt6370_i2c_update_bits(
		info,
		MT6370_PMU_REG_CHGCTRL6,
		MT6370_MASK_MIVR,
		reg_mivr << MT6370_SHIFT_MIVR
	);

	return ret;
}


static int mt_charger_get_ichg(struct mtk_charger_info *mchr_info, u32 *ichg)
{
	int ret = 0;
	u8 reg_ichg = 0;
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;
	u8 data = 0;

	ret = mt6370_i2c_read_byte(info, MT6370_PMU_REG_CHGCTRL7, &data);
	if (ret != I2C_OK)
		return ret;

	reg_ichg = (data & MT6370_MASK_ICHG) >> MT6370_SHIFT_ICHG;
	*ichg = mt6370_find_closest_real_value(MT6370_ICHG_MIN, MT6370_ICHG_MAX,
		MT6370_ICHG_STEP, reg_ichg);

	return ret;
}

static int mt_charger_get_aicr(struct mtk_charger_info *mchr_info, u32 *aicr)
{
	int ret = 0;
	u8 reg_aicr = 0;
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;
	u8 data = 0;

	ret = mt6370_i2c_read_byte(info, MT6370_PMU_REG_CHGCTRL3, &data);
	if (ret != I2C_OK)
		return ret;

	reg_aicr = (data & MT6370_MASK_AICR) >> MT6370_SHIFT_AICR;
	*aicr = mt6370_find_closest_real_value(MT6370_AICR_MIN, MT6370_AICR_MAX,
		MT6370_AICR_STEP, reg_aicr);

	return ret;
}

static int mt_charger_reset_pumpx(struct mtk_charger_info *mchr_info,
	bool reset)
{
	int ret = 0;
	u32 aicr = 0;
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;

	mt6370_enable_hidden_mode(info, true);

	ret = (reset ? mt6370_clr_bit : mt6370_set_bit)(info, 0x38, 0x80);
	aicr = (reset ? 100 : 500);
	ret = mt_charger_set_aicr(mchr_info, aicr);

	mt6370_enable_hidden_mode(info, false);
	return ret;
}

static int mt_charger_enable_wdt(struct mtk_charger_info *mchr_info, bool en)
{
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;
	return mt6370_enable_wdt(info, en);
}

static int mt_charger_reset_wdt(struct mtk_charger_info *mchr_info)
{
	/* Any I2C communication can kick watchdog timer */
	int ret = 0;
	enum mt6370_charging_status chg_status;
	struct mt6370_pmu_charger_info *info =
		(struct mt6370_pmu_charger_info *)mchr_info;

	ret = mt6370_get_charging_status(info, &chg_status);

	return ret;
}

static struct mtk_charger_ops mt6370_mchr_ops = {
	.dump_register = mt_charger_dump_register,
	.enable_charging = mt_charger_enable_charging,
	.get_ichg = mt_charger_get_ichg,
	.set_ichg = mt_charger_set_ichg,
	.set_aicr = mt_charger_set_aicr,
	.set_mivr = mt_charger_set_mivr,
	.enable_power_path = mt_charger_enable_power_path,
	.get_aicr = mt_charger_get_aicr,
	.reset_pumpx = mt_charger_reset_pumpx,
	.enable_wdt = mt_charger_enable_wdt,
	.reset_wdt = mt_charger_reset_wdt,
};


/* Info of primary charger */
static struct mt6370_pmu_charger_info g_mt6370_pmu_charger_info = {
	.mchr_info = {
		.name = "primary_charger",
		.device_id = -1,
		.mchr_ops = &mt6370_mchr_ops,
	},
	.i2c = {
		.id = 0xFF,
		.addr = MT6370_SLAVE_ADDR,
		.mode = HS_MODE,
		.speed = 3400,
		.pushpull = true,
	},
	.i2c_log_level = INFO,
	.hidden_mode_cnt = 0,
	.vendor_id = 0x00,
	.ichg = 2000,
	.ichg_dis_chg = 2000,
	.chg_en = true,
};

static int mt6370_parse_dt(struct mt6370_pmu_charger_info *info)
{
	int offset, sub_offset, val;
	offset = fdt_node_offset_by_compatible(get_lk_overlayed_dtb(), -1, "mediatek,subpmic_pmu");
	sub_offset = fdt_parent_offset(get_lk_overlayed_dtb(), offset);

	if (offset >= 0 && sub_offset >= 0) {
		val = chr_fdt_getprop_u32(get_lk_overlayed_dtb(), sub_offset, "id");
		if (val) {
			info->i2c.id = val;
			dprintf(CRITICAL, "[%s]: subpmic_pmu id = %d\n", __func__, info->i2c.id);
			return 0;
		}
	}
	return -1;
}

int mt6370_chg_probe(void)
{
	int ret = 0;

	ret = mt6370_parse_dt(&g_mt6370_pmu_charger_info);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: parse dt failed\n", __func__);
	}

	/* Check primary charger */
	if (mt6370_is_hw_exist(&g_mt6370_pmu_charger_info)) {
		dprintf(CRITICAL, "%s: %s\n", __func__,
			MT6370_PMU_CHARGER_LK_DRV_VERSION);
#if 0
		ret = mt6370_chg_reset_chip(&g_mt6370_pmu_charger_info);
#endif
		ret = mt6370_chg_init_setting(&g_mt6370_pmu_charger_info);
		mtk_charger_set_info(&(g_mt6370_pmu_charger_info.mchr_info));
	}

	return ret;
}

/*
 * Revision Note
 * 1.1.2
 * (1) Add Iprechg 150mA drop to 40mA workaround
 *
 * 1.1.1
 * (1) Add initial setting about enable vsys short protection
 * when use before MT6371 chip E4 version.
 *
 * 1.1.0
 * (1) Initial release for compatible with MT6370 & MT6371 driver.
 * (2) Move to common folder reduce porting effort.
 * (3) Parse subpmic i2c channel from dts.
 */
