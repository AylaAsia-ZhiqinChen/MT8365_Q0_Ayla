/*
 * Copyright (C) 2018 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/device.h>       /* needed by device_* */
#include <linux/platform_device.h>
#include <linux/uaccess.h>      /* needed by copy_to_user */
#include <linux/mutex.h>
#include <linux/timer.h>
#include <linux/notifier.h>
#include <linux/sizes.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_fdt.h>
#include "adsp_helper.h"
#include "adsp_reg.h"
#include "mtk_hifixdsp_common.h"
#include "adsp_clk.h"


/*
 * HIFIxDSP registers and bits
 */
#define REG_ALT_RESET_VEC       (DSP_ALTRESETVEC)
#define REG_P_DEBUG_BUS0        (DSP_PDEBUGBUS0)
	#define PDEBUG_ENABLE        0
#define REG_SEL_RESET_SW        (DSP_RESET_SW)
	/*reset sw*/
	#define BRESET_SW            0
	#define DRESET_SW            1
	#define PRESET_SW            2
	#define RUNSTALL             3
	#define STATVECTOR_SEL       4
	#define AUTO_BOOT_SW_RESET_B 5


int platform_parse_resource(struct platform_device *pdev, void *data)
{
	int ret = 0;
	struct resource *res;
	struct resource resource;
	struct device_node *mem_region;
	struct device *dev = &pdev->dev;
	struct adsp_chip_info *adsp = data;
	u32 shared_size = SZ_1M; /* default min size for shared-dram */

	/* Parse the start address and size for reserved-mem */
	res = &resource;
	mem_region = of_parse_phandle(dev->of_node, "memory-region", 0);
	if (!mem_region) {
		dev_err(dev, "no 'memory-region' phandle\n");
		return -ENODEV;
	}
	ret = of_address_to_resource(mem_region, 0, res);
	if (ret) {
		dev_err(dev, "of_address_to_resource failed\n");
		return ret;
	}
	adsp->pa_dram = (phys_addr_t)res->start;
	adsp->dramsize = (u32)resource_size(res);
	if (((u32)adsp->pa_dram) & SZ_4K) {
		dev_err(dev, "adsp memory(0x%x) is not 4K-aligned\n",
			(u32)adsp->pa_dram);
		return -EINVAL;
	}

	/* Parse the shared-dram size from reserved-mem */
	if (!of_property_read_u32(dev->of_node,
			"size_shared_dram_from_tail", &shared_size))
		adsp->shared_dram_size = shared_size;

	if (adsp->dramsize < adsp->shared_dram_size) {
		dev_err(dev, "adsp memroy(0x%x) is not enough for share\n",
			(u32)adsp->dramsize);
		return -EINVAL;
	}

	pr_info("[ADSP] dram-pbase=%pa, dram-size=0x%x, shared-dram-size=0x%x\n",
		&adsp->pa_dram,
		adsp->dramsize,
		adsp->shared_dram_size);

	/* Parse ADSP-CFG base */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "no ADSP-CFG register resource\n");
		return -ENXIO;
	}
	/* remap for DSP register accessing */
	adsp->va_cfgreg = devm_ioremap_resource(dev, res);
	if (IS_ERR(adsp->va_cfgreg))
		return PTR_ERR(adsp->va_cfgreg);
	adsp->pa_cfgreg = (phys_addr_t)res->start;
	adsp->cfgregsize = (u32)resource_size(res);

	pr_info("[ADSP] cfgreg-vbase=0x%p, cfgreg-size=0x%x\n",
		adsp->va_cfgreg,
		adsp->cfgregsize);

	/* Parse DTCM */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!res) {
		dev_err(dev, "no DTCM resource\n");
		return -ENXIO;
	}
	adsp->pa_dtcm = (phys_addr_t)res->start;
	adsp->dtcmsize = (u32)resource_size(res);
	adsp->shared_dtcm_size = TOTAL_SIZE_SHARED_DTCM_FROM_TAIL;
	if (adsp->dtcmsize < adsp->shared_dtcm_size) {
		dev_err(dev, "adsp DTCM(0x%x) is not enough for share\n",
			(u32)adsp->dtcmsize);
		return -EINVAL;
	}

	/* Parse ITCM */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
	if (!res) {
		dev_err(dev, "no ITCM resource\n");
		return -ENXIO;
	}
	adsp->pa_itcm = (phys_addr_t)res->start;
	adsp->itcmsize = (u32)resource_size(res);

	pr_info("[ADSP] itcm pbase=%pa,0x%x, dtcm pbase=%pa,0x%x\n",
		 &adsp->pa_itcm, adsp->itcmsize,
		 &adsp->pa_dtcm, adsp->dtcmsize);

	return ret;
}

int adsp_shared_base_ioremap(struct platform_device *pdev, void *data)
{
	int ret = 0;
	phys_addr_t phy_base;
	u32 shared_size;
	struct device *dev = &pdev->dev;
	struct adsp_chip_info *adsp = data;

	/* remap shared-dtcm base */
	shared_size = adsp->shared_dtcm_size;
	phy_base = adsp->pa_dtcm + adsp->dtcmsize - shared_size;
	adsp->shared_dtcm = devm_ioremap_nocache(dev, phy_base, shared_size);
	if (!adsp->shared_dtcm) {
		dev_err(dev, "ioremap failed at line %d\n", __LINE__);
		ret = -ENOMEM;
		goto tail;
	}
	pr_info("[ADSP] shared-dtcm vbase=0x%p, size=0x%x\n",
		adsp->shared_dtcm, shared_size);

	/* remap shared-memory base */
	shared_size = adsp->shared_dram_size;
	phy_base = adsp->pa_dram + adsp->dramsize - shared_size;
	adsp->shared_dram = devm_ioremap_nocache(dev, phy_base, shared_size);
	if (!adsp->shared_dram) {
		dev_err(dev, "ioremap failed at line %d\n", __LINE__);
		ret = -ENOMEM;
		goto tail;
	}
	pr_info("[ADSP] shared-dram vbase=0x%p, size=0x%x\n",
		adsp->shared_dram, shared_size);

	/* split shared-dram mblock for details used */
	/*init_adsp_sysram_reserve_mblock(phy_base, adsp->shared_dram);*/
tail:
	return ret;
}


static void adsp_tcm_mode_set(int mode)
{
	if (mode == TCM_MODE1)
		writel(0x1, DSP_AUDIO_DSP_SHARED_IRAM);
	else if (mode == TCM_MODE2)
		writel(0x3, DSP_AUDIO_DSP_SHARED_IRAM);
	else
		writel(0x0, DSP_AUDIO_DSP_SHARED_IRAM);
}

/*  Init the basic DSP DRAM address */
static int adsp_memory_remap_init(void)
{
	int err = 0;
	void __iomem *vaddr;
	struct adsp_chip_info *adsp;
	int offset;

	adsp = get_adsp_chip_data();
	if (!adsp) {
		err = -EBUSY;
		goto TAIL;
	}

	/* Assume: pa_dram >= DSP_PHY_BASE */
	offset = adsp->pa_dram - DRAM_PHYS_BASE_FROM_DSP_VIEW;
	WARN_ON(offset < 0);

	/*
	 * offset = CPU Dram base Address - DSP Dram Base Address;
	 * ioremap and write offset to CPU2DSP dram remap control reg.
	 */
	vaddr = ioremap(DSP_EMI_BASE_ADDR, 4);
	if (!vaddr) {
		err = -ENOMEM;
		goto TAIL;
	}
	writel(offset, vaddr);
	iounmap(vaddr);
TAIL:
	return err;
}

int adsp_must_setting_early(struct device *dev)
{
	int ret = 0;

	/* support adsp-power at 'probe' phase */
	ret = adsp_pm_register_early(dev);
	if (ret)
		goto TAIL;

	/* set tcm mode before remap DSP base */
	adsp_tcm_mode_set(TCM_MODE3);

	/*
	 * HIFIxDSP remap setting
	 */
	ret = adsp_memory_remap_init();

TAIL:
	return ret;
}

int adsp_clock_power_on(struct device *dev)
{
	int ret = 0;

	/* Step0: Enable ADSP always on */
	ret = device_init_wakeup(dev, true);
	if (ret)
		goto TAIL;

	/* Step1: Open ADSP clock */
	ret = adsp_default_clk_init(dev, 1);
	if (ret)
		goto TAIL;

	/* Step2: Open ADSP power */
	ret = adsp_power_enable(dev);

TAIL:
	return ret;
}

int adsp_clock_power_off(struct device *dev)
{
	int ret = 0;

	/* Step0: Disable ADSP always on */
	ret = device_init_wakeup(dev, false);
	if (ret)
		goto TAIL;

	/* Step1: Close ADSP power-domains */
	adsp_power_disable(dev);

	/* Step2: Close ADSP clock */
	ret = adsp_default_clk_init(dev, 0);

TAIL:
	return ret;
}

void hifixdsp_boot_sequence(u32 boot_addr)
{
	u32 val;

	/* ADSP bootup base */
	writel(boot_addr, REG_ALT_RESET_VEC);
	val = readl(REG_ALT_RESET_VEC);
	pr_info("[ADSP] HIFIxDSP boot from base : 0x%08X\n", val);

	/*
	 * 1.STATVECTOR_SEL pull high to
	 * select external reset vector : altReserVec
	 * 2. RunStall pull high
	 */
	val = readl(REG_SEL_RESET_SW);
	val |= (0x1 << STATVECTOR_SEL) | (0x1 << RUNSTALL);
	writel(val, REG_SEL_RESET_SW);

	/* DReset & BReset pull high */
	val = readl(REG_SEL_RESET_SW);
	val |= (0x1 << BRESET_SW) | (0x1 << DRESET_SW);
	writel(val, REG_SEL_RESET_SW);

	/* DReset & BReset pull low */
	val = readl(REG_SEL_RESET_SW);
	val &= ~((u32)((0x1 << BRESET_SW) | (0x1 << DRESET_SW)));
	writel(val, REG_SEL_RESET_SW);

	/* Enable PDebug */
	val = readl(REG_P_DEBUG_BUS0);
	val |= (0x1 << PDEBUG_ENABLE);
	writel(val, REG_P_DEBUG_BUS0);

	/* DSP RESET B as high to release DSP */
	val = readl(REG_SEL_RESET_SW);
	val |= (0x1 << AUTO_BOOT_SW_RESET_B);
	writel(val, REG_SEL_RESET_SW);

	/* RUN_STALL pull down */
	val = readl(REG_SEL_RESET_SW);
	val &= ~((u32)(0x1 << RUNSTALL));
	writel(val, REG_SEL_RESET_SW);
}

void hifixdsp_shutdown(void)
{
	u32 val;

	/* Clear to 0 firstly */
	val = 0x0;
	writel(val, REG_SEL_RESET_SW);

	/* RUN_STALL pull high again to reset */
	val = readl(REG_SEL_RESET_SW);
	val |= (0x1 << RUNSTALL);
	writel(val, REG_SEL_RESET_SW);
}

static
int adsp_update_memory_protect_info(void)
{
	/* code will be added later */
	return 0;
}

int adsp_misc_setting_after_poweron(void)
{
	int ret = 0;

	ret = adsp_update_memory_protect_info();
	return ret;
}

int adsp_remove_setting_after_shutdown(void)
{
	int ret = 0;

	writel(0x00, DSP_AUDIO_DSP2SPM_INT);

	return ret;
}

int adsp_shutdown_notify_check(void)
{
	int ret = 0;

	return ret;
}

void __iomem *get_adsp_reg_base(void)
{
	struct adsp_chip_info *adsp;

	adsp = get_adsp_chip_data();
	if (!adsp)
		return NULL;

	/*
	 * adsp->va_cfgreg : DSP-CFG virtual base,
	 * which must be ioremapped before first-use.
	 */
	return adsp->va_cfgreg;
}

