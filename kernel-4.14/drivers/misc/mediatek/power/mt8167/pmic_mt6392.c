/*
 * Copyright (c) 2019 MediaTek Inc.
 * Author: Chen Zhong <chen.zhong@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irqdomain.h>
#include <linux/kernel.h>
#include <linux/kernel.h>
#include <linux/mfd/mt6392/registers.h>
#include <linux/mfd/mt6397/core.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/version.h>

#include <mt-plat/upmu_common.h>

#define PMIC6392_E1_CID_CODE 0x1092
#define E_PWR_INVALID_DATA 33

struct regmap *pwrap_regmap;

static DEFINE_SPINLOCK(vcn35_on_ctrl_spinlock);
static int vcn35_on_ctrl_bt;
static int vcn35_on_ctrl_wifi;

unsigned int pmic_read_interface(unsigned int RegNum, unsigned int *val,
				 unsigned int MASK, unsigned int SHIFT)
{
	unsigned int return_value = 0;
	unsigned int pmic_reg = 0;

	return_value = regmap_read(pwrap_regmap, RegNum, &pmic_reg);
	if (return_value != 0) {
		pr_debug("[Power/PMIC][%s] Reg[%x]= pmic_wrap read data fail\n",
		       __func__, RegNum);
		return return_value;
	}

	pmic_reg &= (MASK << SHIFT);
	*val = (pmic_reg >> SHIFT);

	return return_value;
}

unsigned int pmic_config_interface(unsigned int RegNum, unsigned int val,
				   unsigned int MASK, unsigned int SHIFT)
{
	unsigned int return_value = 0;
	unsigned int pmic_reg = 0;

	if (val > MASK) {
		pr_debug("[Power/PMIC][%s] Invalid data, Reg[%x]: MASK = 0x%x, val = 0x%x\n",
		       __func__, RegNum, MASK, val);
		return E_PWR_INVALID_DATA;
	}

	return_value = regmap_read(pwrap_regmap, RegNum, &pmic_reg);
	if (return_value != 0) {
		pr_debug("[Power/PMIC][i%s] Reg[%x]= pmic_wrap read data fail\n",
		       __func__, RegNum);
		return return_value;
	}

	pmic_reg &= ~(MASK << SHIFT);
	pmic_reg |= (val << SHIFT);

	return_value = regmap_write(pwrap_regmap, RegNum, pmic_reg);
	if (return_value != 0) {
		pr_debug("[Power/PMIC][%s] Reg[%x]= pmic_wrap read data fail\n",
		       __func__, RegNum);
		return return_value;
	}

	return return_value;
}

u32 upmu_get_reg_value(u32 reg)
{
	u32 reg_val = 0;

	pmic_read_interface(reg, &reg_val, 0xFFFF, 0x0);

	return reg_val;
}
EXPORT_SYMBOL(upmu_get_reg_value);

void upmu_set_reg_value(u32 reg, u32 reg_val)
{
	pmic_config_interface(reg, reg_val, 0xFFFF, 0x0);
}

u32 g_reg_value;
static ssize_t show_pmic_access(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	pr_notice("[%s] 0x%x\n", __func__,  g_reg_value);
	return sprintf(buf, "%04X\n", g_reg_value);
}

static ssize_t store_pmic_access(struct device *dev,
				 struct device_attribute *attr, const char *buf,
				 size_t size)
{
	int ret = 0;
	char temp_buf[32];
	char *pvalue;
	unsigned int reg_value = 0;
	unsigned int reg_address = 0;

	strncpy(temp_buf, buf, sizeof(temp_buf));
	temp_buf[sizeof(temp_buf) - 1] = 0;
	pvalue = temp_buf;

	if (size != 0) {
		if (size > 5) {
			ret = kstrtouint(strsep(&pvalue, " "), 16,
					 &reg_address);
			if (ret)
				return ret;
			ret = kstrtouint(pvalue, 16, &reg_value);
			if (ret)
				return ret;
			pr_notice(
				"[%s] write PMU reg 0x%x with value 0x%x !\n",
				__func__, reg_address, reg_value);
			ret = pmic_config_interface(reg_address, reg_value,
						    0xFFFF, 0x0);
		} else {
			ret = kstrtouint(pvalue, 16, &reg_address);
			if (ret)
				return ret;
			ret = pmic_read_interface(reg_address, &g_reg_value,
						  0xFFFF, 0x0);
			pr_notice("[%s] read PMU reg 0x%x with value 0x%x !\n",
				__func__, reg_address, g_reg_value);
			pr_notice("[%s] Please use \"cat pmic_access\" to get value\r\n",
				__func__);
		}
	}
	return size;
}

static DEVICE_ATTR(pmic_access, 0664, show_pmic_access,
		   store_pmic_access); /* 664 */

void PMIC_INIT_SETTING_V1(void)
{
#ifdef CONFIG_MTK_PMIC_VMCH_PG_DISABLE
	unsigned int ret = 0;

	ret = pmic_config_interface(0x48, 0x1, 0x1,
				    12); /* [10]: VMCH_PG_ENB; */
#endif
}

static void pmic_low_power_setting(void)
{
	unsigned int ret = 0;

	ret = pmic_config_interface(0x4E, 0x1, 0x1,
				    5); /* [5:5]: STRUP_AUXADC_RSTB_SW; */
	ret = pmic_config_interface(0x4E, 0x1, 0x1,
				    7); /* [7:7]: STRUP_AUXADC_RSTB_SEL; */
	ret = pmic_config_interface(0x404, 0x1, 0x1,
				    0); /* [0:0]: VAUD22_LP_SEL; */
	ret = pmic_config_interface(0x424, 0x1, 0x1,
				    0); /* [0:0]: VAUD28_LP_SEL; */
	ret = pmic_config_interface(0x428, 0x1, 0x1,
				    0); /* [0:0]: VADC18_LP_SEL; */
	ret = pmic_config_interface(0x500, 0x1, 0x1,
				    0); /* [0:0]: VIO28_LP_SEL; */
	ret = pmic_config_interface(0x502, 0x1, 0x1,
				    0); /* [0:0]: VUSB_LP_SEL; */
	ret = pmic_config_interface(0x552, 0x1, 0x1, 0); /* [0:0]: VM_LP_SEL; */
	ret = pmic_config_interface(0x556, 0x1, 0x1,
				    0); /* [0:0]: VIO18_LP_SEL; */
	ret = pmic_config_interface(0x562, 0x1, 0x1,
				    0); /* [0:0]: VM25_LP_SEL; */
	ret = pmic_config_interface(0x738, 0x0, 0x1,
				    15); /* [15:15]: AUXADC_CK_AON; */
	ret = pmic_config_interface(0x75C, 0x0, 0x1,
				    14); /* [14:14]: AUXADC_START_SHADE_EN */
	ret = pmic_config_interface(0x102, 0x0, 0x1,
				    1); /* [1:1]: RG_CLKSQ_EN; */

#ifndef CONFIG_USB_C_SWITCH_MT6392
	ret = pmic_config_interface(0x10E, 0x1, 0x1,
				    8); /* [8:8]: RG_TYPE_C_CSR_CK_PDN; */
	ret = pmic_config_interface(0x10E, 0x1, 0x1,
				    9); /* [9:9]: RG_TYPE_C_CC_CK_PDN; */
#endif

	pr_notice("[Power/PMIC] low power setting done...\n");
}

void upmu_set_vcn35_on_ctrl_bt(unsigned int val)
{
	unsigned int ret, cid;
	u32 vcn35_on_ctrl;

	cid = upmu_get_cid();
	if (cid == PMIC6392_E1_CID_CODE) {
		pr_debug("%s: MT6392 PMIC CID=0x%x\n", __func__, cid);

		spin_lock(&vcn35_on_ctrl_spinlock);

		if (val)
			vcn35_on_ctrl_bt = 1;
		else
			vcn35_on_ctrl_bt = 0;

		if (vcn35_on_ctrl_bt || vcn35_on_ctrl_wifi)
			vcn35_on_ctrl = 0x1;
		else
			vcn35_on_ctrl = 0x0;

		ret = pmic_config_interface(
			(unsigned int)(MT6392_DIGLDO_CON61),
			(unsigned int)(vcn35_on_ctrl),
			(unsigned int)(MT6392_PMIC_VCN35_ON_CTRL_MASK),
			(unsigned int)(MT6392_PMIC_VCN35_ON_CTRL_SHIFT));

		spin_unlock(&vcn35_on_ctrl_spinlock);
	} else {
		pr_debug("%s: MT6392 PMIC CID=0x%x\n", __func__, cid);
		ret = pmic_config_interface(
			(unsigned int)(MT6392_ANALDO_CON16),
			(unsigned int)(val),
			(unsigned int)(MT6392_PMIC_VCN35_ON_CTRL_BT_MASK),
			(unsigned int)(MT6392_PMIC_VCN35_ON_CTRL_BT_SHIFT));
	}
}

void upmu_set_vcn35_on_ctrl_wifi(unsigned int val)
{
	unsigned int ret, cid;
	u32 vcn35_on_ctrl;

	cid = upmu_get_cid();
	if (cid == PMIC6392_E1_CID_CODE) {
		pr_debug("%s: MT6392 PMIC CID=0x%x\n", __func__, cid);

		spin_lock(&vcn35_on_ctrl_spinlock);

		if (val)
			vcn35_on_ctrl_wifi = 1;
		else
			vcn35_on_ctrl_wifi = 0;

		if (vcn35_on_ctrl_bt || vcn35_on_ctrl_wifi)
			vcn35_on_ctrl = 0x1;
		else
			vcn35_on_ctrl = 0x0;

		ret = pmic_config_interface(
			(unsigned int)(MT6392_DIGLDO_CON61),
			(unsigned int)(vcn35_on_ctrl),
			(unsigned int)(MT6392_PMIC_VCN35_ON_CTRL_MASK),
			(unsigned int)(MT6392_PMIC_VCN35_ON_CTRL_SHIFT));

		spin_unlock(&vcn35_on_ctrl_spinlock);
	} else {
		pr_debug("%s: MT6392 PMIC CID=0x%x\n", __func__, cid);
		ret = pmic_config_interface(
			(unsigned int)(MT6392_ANALDO_CON17),
			(unsigned int)(val),
			(unsigned int)(MT6392_PMIC_VCN35_ON_CTRL_WIFI_MASK),
			(unsigned int)(MT6392_PMIC_VCN35_ON_CTRL_WIFI_SHIFT));
	}
}

static irqreturn_t thr_h_int_handler(int irq, void *dev_id)
{
	unsigned int ret = 0;
	unsigned int val = 0;

	pr_info("%s!\n", __func__);

	/* Read PMIC test register to get VMCH PG status */
	pmic_config_interface(0x134, 0x0102, 0xFFFF, 0);
	ret = pmic_read_interface(0x132, &val, 0x1, 2);
	if (val == 0) {
		/* VMCH is not good */
		pmic_config_interface(0x506, 0x0, 0x1, 14);
		pr_notice("%s: VMCH not good with status: 0x%x, turn off!\n",
			  __func__, upmu_get_reg_value(0x132));
	}

	return IRQ_HANDLED;
}

static irqreturn_t thr_l_int_handler(int irq, void *dev_id)
{
	pr_info("%s!\n", __func__);

	return IRQ_HANDLED;
}

static int mt6392_pmic_probe(struct platform_device *dev)
{
	struct resource *res;
	int ret_val = 0;
	int irq_thr_l, irq_thr_h;
	struct mt6397_chip *mt6392_chip = dev_get_drvdata(dev->dev.parent);

	pr_debug("[Power/PMIC] ******** MT6392 pmic driver probe!! ********\n");

	pwrap_regmap = mt6392_chip->regmap;

	/* get PMIC CID */
	ret_val = upmu_get_cid();
	pr_notice("Power/PMIC MT6392 PMIC CID=0x%x\n", ret_val);

	/* pmic initial setting */
	PMIC_INIT_SETTING_V1();

	/* pmic low power setting */
	pmic_low_power_setting();

	res = platform_get_resource(dev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_info(&dev->dev, "no IRQ resource\n");
		return -ENODEV;
	}

	irq_thr_l = irq_create_mapping(mt6392_chip->irq_domain, res->start);
	if (irq_thr_l <= 0)
		return -EINVAL;

	ret_val = request_threaded_irq(irq_thr_l, NULL, thr_l_int_handler,
				       IRQF_ONESHOT | IRQF_TRIGGER_HIGH,
				       "mt6397-thr_l", &dev->dev);
	if (ret_val) {
		dev_info(&dev->dev,
			 "Failed to request mt6397-thr_l IRQ: %d: %d\n",
			 irq_thr_l, ret_val);
	}

	irq_thr_h = irq_create_mapping(mt6392_chip->irq_domain, res->end);
	if (irq_thr_h <= 0)
		return -EINVAL;

	ret_val = request_threaded_irq(irq_thr_h, NULL, thr_h_int_handler,
				       IRQF_ONESHOT | IRQF_TRIGGER_HIGH,
				       "mt6397-thr_h", &dev->dev);
	if (ret_val) {
		dev_info(&dev->dev,
			 "Failed to request mt6397-thr_h IRQ: %d: %d\n",
			 irq_thr_h, ret_val);
	}

	device_create_file(&(dev->dev), &dev_attr_pmic_access);
	return 0;
}

static const struct platform_device_id mt6392_pmic_ids[] = {
	{"mt6392-pmic", 0}, {/* sentinel */},
};
MODULE_DEVICE_TABLE(platform, mt6392_pmic_ids);

static const struct of_device_id mt6392_pmic_of_match[] = {
	{
		.compatible = "mediatek,mt6392-pmic",
	},
	{/* sentinel */},
};
MODULE_DEVICE_TABLE(of, mt6392_pmic_of_match);

static struct platform_driver mt6392_pmic_driver = {
	.driver = {

			.name = "mt6392-pmic",
			.of_match_table = of_match_ptr(mt6392_pmic_of_match),
		},
	.probe = mt6392_pmic_probe,
	.id_table = mt6392_pmic_ids,
};

module_platform_driver(mt6392_pmic_driver);

MODULE_AUTHOR("Chen Zhong <chen.zhong@mediatek.com>");
MODULE_DESCRIPTION("PMIC Misc Setting Driver for MediaTek MT6392 PMIC");
MODULE_LICENSE("GPL v2");
