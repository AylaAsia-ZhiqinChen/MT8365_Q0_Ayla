// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2015 Linaro Ltd.
 * Author: Pi-Cheng Chen <pi-cheng.chen@linaro.org>
 * Copyright (c) 2019 MediaTek Inc.
 */

#include <linux/clk.h>
#include <linux/cpu.h>
#include <linux/cpu_cooling.h>
#include <linux/cpufreq.h>
#include <linux/cpumask.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm_opp.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/thermal.h>
#include "mtk_power_throttle.h"
#include "mtk_common_static_power.h"
#include "mt8168-cpufreq.h"

#define MIN_VOLT_SHIFT		(100000)
#define MAX_VOLT_SHIFT		(200000)
#define MAX_VOLT_LIMIT		(1050000)
#define MIN_VOLT_LIMIT		(800000)
#define VOLT_TOL		(6250)

/*
 * The struct mtk_cpu_dvfs_info holds necessary information for doing CPU DVFS
 * on each CPU power/clock domain of Mediatek SoCs. Each CPU cluster in
 * Mediatek SoCs has two voltage inputs, Vproc and Vsram. In some cases the two
 * voltage inputs need to be controlled under a hardware limitation:
 * 100mV < Vsram - Vproc < 200mV
 *
 * When scaling the clock frequency of a CPU clock domain, the clock source
 * needs to be switched to another stable PLL clock temporarily until
 * the original PLL becomes stable at target frequency.
 */
struct mtk_cpu_dvfs_info {
	struct cpumask cpus;
	struct device *cpu_dev;
	struct regulator *proc_reg;
	struct regulator *sram_reg;
	struct clk *cpu_clk;
	struct clk *inter_clk;
	struct list_head list_head;
	struct mutex lock;
	struct notifier_block opp_nb;
	struct thermal_cooling_device *cdev;
	bool need_voltage_tracking;
	int intermediate_voltage;
	int opp_cpu;
	int max_volt_uv;
	unsigned long opp_freq;
};

static LIST_HEAD(dvfs_info_list);

struct mtk_cpu_dvfs tb_cpu_dvfs[NR_MT_CPU_DVFS] = {
	[MT_CPU_DVFS_L] = {
		.name		= "MT_CPU_DVFS_L",
		.id		= MT_CPU_DVFS_L,
		.cpu_id		= 0,
	},
};

struct mtk_cpu_dvfs *id_to_cpu_dvfs(enum mt_cpu_dvfs_id id)
{
	return (id < NR_MT_CPU_DVFS) ? &tb_cpu_dvfs[id] : NULL;
}

static struct mtk_cpu_dvfs_info *mtk_cpu_dvfs_info_lookup(int cpu)
{
	struct mtk_cpu_dvfs_info *info;

	list_for_each_entry(info, &dvfs_info_list, list_head) {
		if (cpumask_test_cpu(cpu, &info->cpus))
			return info;
	}

	return NULL;
}

static int mtk_cpufreq_voltage_tracking(struct mtk_cpu_dvfs_info *info,
					int new_vproc)
{
	struct regulator *proc_reg = info->proc_reg;
	struct regulator *sram_reg = info->sram_reg;
	int old_vproc, old_vsram, new_vsram, vsram, vproc, ret;

	old_vproc = regulator_get_voltage(proc_reg);
	if (old_vproc < 0) {
		pr_err("%s: invalid Vproc value: %d\n", __func__, old_vproc);
		return old_vproc;
	}
	/* Vsram should not exceed the maximum allowed voltage of SoC. */
	new_vsram = min(new_vproc + MIN_VOLT_SHIFT, MAX_VOLT_LIMIT);
	/* Vsram should exceed the minimum allowed voltage of SoC */
	new_vsram = max(new_vsram, MIN_VOLT_LIMIT);

	if (old_vproc < new_vproc) {
		/*
		 * When scaling up voltages, Vsram and Vproc scale up step
		 * by step. At each step, set Vsram to (Vproc + 200mV) first,
		 * then set Vproc to (Vsram - 100mV).
		 * Keep doing it until Vsram and Vproc hit target voltages.
		 */
		do {
			old_vsram = regulator_get_voltage(sram_reg);
			if (old_vsram < 0) {
				pr_err("%s: invalid Vsram value: %d\n",
				       __func__, old_vsram);
				return old_vsram;
			}
			old_vproc = regulator_get_voltage(proc_reg);
			if (old_vproc < 0) {
				pr_err("%s: invalid Vproc value: %d\n",
				       __func__, old_vproc);
				return old_vproc;
			}

			vsram = min(new_vsram, old_vproc + MAX_VOLT_SHIFT);

			if (vsram + VOLT_TOL >= MAX_VOLT_LIMIT) {
				vsram = MAX_VOLT_LIMIT;

				/*
				 * If the target Vsram hits the maximum voltage,
				 * try to set the exact voltage value first.
				 */
				ret = regulator_set_voltage(sram_reg, vsram,
							    vsram);
				if (ret)
					ret = regulator_set_voltage(sram_reg,
							vsram - VOLT_TOL,
							vsram);

				vproc = new_vproc;

			} else if (vsram <= MIN_VOLT_LIMIT) {
				vsram = MIN_VOLT_LIMIT;

				/*
				 * If the target Vsram hits the minimum voltage,
				 * try to set the exact voltage value first.
				 */
				ret = regulator_set_voltage(sram_reg, vsram,
							    vsram);
				if (ret)
					ret = regulator_set_voltage(sram_reg,
							vsram - VOLT_TOL,
							vsram);

				vproc = new_vproc;

			} else {
				ret = regulator_set_voltage(sram_reg, vsram,
							    vsram + VOLT_TOL);

				vproc = vsram - MIN_VOLT_SHIFT;
			}
			if (ret)
				return ret;

			ret = regulator_set_voltage(proc_reg, vproc,
						    vproc + VOLT_TOL);
			if (ret) {
				regulator_set_voltage(sram_reg, old_vsram,
						      old_vsram);
				return ret;
			}
		} while (vproc < new_vproc || vsram < new_vsram);
	} else if (old_vproc > new_vproc) {
		/*
		 * When scaling down voltages, Vsram and Vproc scale down step
		 * by step. At each step, set Vproc to (Vsram - 200mV) first,
		 * then set Vsram to (Vproc + 100mV).
		 * Keep doing it until Vsram and Vproc hit target voltages.
		 */
		do {
			old_vproc = regulator_get_voltage(proc_reg);
			if (old_vproc < 0) {
				pr_err("%s: invalid Vproc value: %d\n",
				       __func__, old_vproc);
				return old_vproc;
			}
			old_vsram = regulator_get_voltage(sram_reg);
			if (old_vsram < 0) {
				pr_err("%s: invalid Vsram value: %d\n",
				       __func__, old_vsram);
				return old_vsram;
			}

			vproc = max(new_vproc, old_vsram - MAX_VOLT_SHIFT);
			ret = regulator_set_voltage(proc_reg, vproc,
						    vproc + VOLT_TOL);
			if (ret)
				return ret;

			if (vproc == new_vproc)
				vsram = new_vsram;
			else
				vsram = max(new_vsram, vproc + MIN_VOLT_SHIFT);

			if (vsram + VOLT_TOL >= MAX_VOLT_LIMIT) {
				vsram = MAX_VOLT_LIMIT;

				/*
				 * If the target Vsram hits the maximum voltage,
				 * try to set the exact voltage value first.
				 */
				ret = regulator_set_voltage(sram_reg, vsram,
							    vsram);
				if (ret)
					ret = regulator_set_voltage(sram_reg,
							vsram - VOLT_TOL,
							vsram);
			} else {
				ret = regulator_set_voltage(sram_reg, vsram,
							    vsram + VOLT_TOL);
			}

			if (ret) {
				regulator_set_voltage(proc_reg, old_vproc,
						      old_vproc);
				return ret;
			}
		} while (vproc > new_vproc + VOLT_TOL ||
			 vsram > new_vsram + VOLT_TOL);
	}

	return 0;
}

static int mtk_cpufreq_set_voltage(struct mtk_cpu_dvfs_info *info, int vproc)
{
	if (info->need_voltage_tracking)
		return mtk_cpufreq_voltage_tracking(info, vproc);
	else
		return regulator_set_voltage(info->proc_reg, vproc,
					     info->max_volt_uv);
}

static int mtk_cpufreq_set_target(struct cpufreq_policy *policy,
				  unsigned int index)
{
	struct cpufreq_frequency_table *freq_table = policy->freq_table;
	struct clk *cpu_clk = policy->clk;
	struct clk *armpll = clk_get_parent(cpu_clk);
	struct mtk_cpu_dvfs_info *info = policy->driver_data;
	struct device *cpu_dev = info->cpu_dev;
	struct dev_pm_opp *opp;
	long freq_hz, old_freq_hz;
	int vproc, old_vproc, inter_vproc, target_vproc, ret;

	struct mtk_cpu_dvfs *p = id_to_cpu_dvfs(MT_CPU_DVFS_L);

	inter_vproc = info->intermediate_voltage;

	old_freq_hz = clk_get_rate(cpu_clk);
	old_vproc = regulator_get_voltage(info->proc_reg);
	if (old_vproc < 0) {
		pr_err("%s: invalid Vproc value: %d\n", __func__, old_vproc);
		return old_vproc;
	}

	freq_hz = freq_table[index].frequency * 1000;

	opp = dev_pm_opp_find_freq_ceil(cpu_dev, &freq_hz);
	if (IS_ERR(opp)) {
		pr_err("cpu%d: failed to find OPP for %ld\n",
		       policy->cpu, freq_hz);
		return PTR_ERR(opp);
	}
	vproc = dev_pm_opp_get_voltage(opp);
	dev_pm_opp_put(opp);

	mutex_lock(&info->lock);
	/*
	 * If the new voltage or the intermediate voltage is higher than the
	 * current voltage, scale up voltage first.
	 */
	target_vproc = (inter_vproc > vproc) ? inter_vproc : vproc;
	if (old_vproc < target_vproc) {
		ret = mtk_cpufreq_set_voltage(info, target_vproc);
		if (ret) {
			pr_err("cpu%d: failed to scale up voltage!\n",
			       policy->cpu);
			mtk_cpufreq_set_voltage(info, old_vproc);
			mutex_unlock(&info->lock);
			return ret;
		}
	}

	/* Reparent the CPU clock to intermediate clock. */
	ret = clk_set_parent(cpu_clk, info->inter_clk);
	if (ret) {
		pr_err("cpu%d: failed to re-parent cpu clock!\n",
		       policy->cpu);
		mtk_cpufreq_set_voltage(info, old_vproc);
		WARN_ON(1);
		mutex_unlock(&info->lock);
		return ret;
	}

	/* Set the original PLL to target rate. */
	ret = clk_set_rate(armpll, freq_hz);
	if (ret) {
		pr_err("cpu%d: failed to scale cpu clock rate!\n",
		       policy->cpu);
		clk_set_parent(cpu_clk, armpll);
		mtk_cpufreq_set_voltage(info, old_vproc);
		mutex_unlock(&info->lock);
		return ret;
	}

	/* Set parent of CPU clock back to the original PLL. */
	ret = clk_set_parent(cpu_clk, armpll);
	if (ret) {
		pr_err("cpu%d: failed to re-parent cpu clock!\n",
		       policy->cpu);
		mtk_cpufreq_set_voltage(info, inter_vproc);
		WARN_ON(1);
		mutex_unlock(&info->lock);
		return ret;
	}

	/*
	 * If the new voltage is lower than the intermediate voltage or the
	 * original voltage, scale down to the new voltage.
	 */
	if (vproc < inter_vproc || vproc < old_vproc) {
		ret = mtk_cpufreq_set_voltage(info, vproc);
		if (ret) {
			pr_err("cpu%d: failed to scale down voltage!\n",
			       policy->cpu);
			clk_set_parent(cpu_clk, info->inter_clk);
			clk_set_rate(armpll, old_freq_hz);
			clk_set_parent(cpu_clk, armpll);
			mutex_unlock(&info->lock);
			return ret;
		}
	}

	info->opp_freq = freq_hz;
	p->idx_opp_tbl = index;

	mutex_unlock(&info->lock);

	return 0;
}

#define DYNAMIC_POWER "dynamic-power-coefficient"

static int mtk_cpufreq_opp_notifier(struct notifier_block *nb,
unsigned long event, void *data)
{
	struct cpufreq_policy *policy;
	struct dev_pm_opp *opp = data;
	struct dev_pm_opp *opp_item;
	struct mtk_cpu_dvfs_info *info =
		container_of(nb, struct mtk_cpu_dvfs_info, opp_nb);
	unsigned long freq, volt;
	int ret = 0;

	if (event == OPP_EVENT_ADJUST_VOLTAGE) {
		freq = dev_pm_opp_get_freq(opp);

		mutex_lock(&info->lock);
		if (info->opp_freq == freq) {
			volt = dev_pm_opp_get_voltage(opp);
			ret = mtk_cpufreq_set_voltage(info, volt);
			if (ret)
				dev_err(info->cpu_dev,
					"failed to scale voltage: %d\n", ret);
		}
		mutex_unlock(&info->lock);
	} else if (event == OPP_EVENT_DISABLE) {
		freq = info->opp_freq;
		opp_item = dev_pm_opp_find_freq_ceil(info->cpu_dev, &freq);
		if (!IS_ERR(opp_item))
			dev_pm_opp_put(opp_item);
		else
			freq = 0;

		/* case of current opp is disabled */
		if (freq == 0 || freq != info->opp_freq) {
			/* find an enable opp item */
			freq = 1;
			opp_item = dev_pm_opp_find_freq_ceil(info->cpu_dev,
							     &freq);
			if (!IS_ERR(opp_item)) {
				dev_pm_opp_put(opp_item);
				policy = cpufreq_cpu_get(info->opp_cpu);
				if (policy) {
					cpufreq_driver_target(policy,
					freq/1000, CPUFREQ_RELATION_L);
					cpufreq_cpu_put(policy);
				}
			} else
				freq = 0;
		}
	} else if (event == OPP_EVENT_ENABLE) {
		/* sync OPP enable event to re-evaluate cpufreq policy */
		cpufreq_update_policy(0);
	}

	return notifier_from_errno(ret);
}


#define STATIC_POWER "static-power-coefficient"

int plat_static_func(cpumask_t *cpumask, int interval,
	unsigned long voltage, u32 *power)
{
	unsigned int total_power = 0;

	return total_power;
}

static void mtk_cpufreq_ready(struct cpufreq_policy *policy)
{
	struct mtk_cpu_dvfs_info *info = policy->driver_data;
	struct device_node *np = of_node_get(info->cpu_dev->of_node);
	u32 capacitance = 0;
	u32 static_capacitance = 0;

	if (WARN_ON(!np))
		return;

	if (of_find_property(np, "#cooling-cells", NULL)) {
		of_property_read_u32(np, DYNAMIC_POWER, &capacitance);
		of_property_read_u32(np, STATIC_POWER, &static_capacitance);

		info->cdev = of_cpufreq_power_cooling_register(np,
						policy,
						capacitance,
						plat_static_func);

		if (IS_ERR(info->cdev)) {
			dev_err(info->cpu_dev,
				"running cpufreq without cooling device: %ld\n",
				PTR_ERR(info->cdev));

			info->cdev = NULL;
		}
	}

	of_node_put(np);
}

static int mtk_cpu_dvfs_info_init(struct mtk_cpu_dvfs_info *info, int cpu)
{
	struct device *cpu_dev;
	struct regulator *proc_reg = ERR_PTR(-ENODEV);
	struct regulator *sram_reg = ERR_PTR(-ENODEV);
	struct clk *cpu_clk = ERR_PTR(-ENODEV);
	struct clk *inter_clk = ERR_PTR(-ENODEV);
	struct dev_pm_opp *opp;
	unsigned long rate;
	int ret;

	mutex_init(&info->lock);

	cpu_dev = get_cpu_device(cpu);
	if (!cpu_dev) {
		pr_err("failed to get cpu%d device\n", cpu);
		return -ENODEV;
	}

	cpu_clk = clk_get(cpu_dev, "cpu");
	if (IS_ERR(cpu_clk)) {
		if (PTR_ERR(cpu_clk) == -EPROBE_DEFER)
			pr_warn("cpu clk for cpu%d not ready, retry.\n", cpu);
		else
			pr_err("failed to get cpu clk for cpu%d\n", cpu);

		ret = PTR_ERR(cpu_clk);
		return ret;
	}

	inter_clk = clk_get(cpu_dev, "intermediate");
	if (IS_ERR(inter_clk)) {
		if (PTR_ERR(inter_clk) == -EPROBE_DEFER)
			pr_warn("intermediate clk for cpu%d not ready, retry.\n",
				cpu);
		else
			pr_err("failed to get intermediate clk for cpu%d\n",
			       cpu);

		ret = PTR_ERR(inter_clk);
		goto out_free_resources;
	}

	proc_reg = regulator_get_optional(cpu_dev, "proc");
	if (IS_ERR(proc_reg)) {
		if (PTR_ERR(proc_reg) == -EPROBE_DEFER)
			pr_warn("proc regulator for cpu%d not ready, retry.\n",
				cpu);
		else
			pr_err("failed to get proc regulator for cpu%d\n",
			       cpu);

		ret = PTR_ERR(proc_reg);
		goto out_free_resources;
	}

	/* ToFix: Need PMIC to fix API */
	/* Both presence and absence of sram regulator are valid cases. */
	/* sram_reg = regulator_get_exclusive(cpu_dev, "sram"); */
	/* Temporary use */
	sram_reg = regulator_get_optional(cpu_dev, "sram");
	if (IS_ERR(sram_reg)) {
		if (PTR_ERR(sram_reg) == -EPROBE_DEFER)
			pr_warn("sram regulator for cpu%d not ready, retry.\n",
				cpu);
		else
			pr_err("failed to get sram regulator for cpu%d, error code: %ld\n",
			       cpu, PTR_ERR(sram_reg));

		ret = PTR_ERR(sram_reg);
	}

	/* Get OPP-sharing information from "operating-points-v2" bindings */
	ret = dev_pm_opp_of_get_sharing_cpus(cpu_dev, &info->cpus);
	if (ret) {
		pr_err("failed to get OPP-sharing information for cpu%d\n",
		       cpu);
		goto out_free_resources;
	}

	ret = dev_pm_opp_of_cpumask_add_table(&info->cpus);
	if (ret) {
		pr_warn("no OPP table for cpu%d\n", cpu);
		goto out_free_resources;
	}

	ret = clk_prepare_enable(inter_clk);
	if (ret)
		goto out_free_opp_table;

	/* Search a safe voltage for intermediate frequency. */
	rate = clk_get_rate(inter_clk);
	opp = dev_pm_opp_find_freq_ceil(cpu_dev, &rate);
	if (IS_ERR(opp)) {
		pr_err("failed to get intermediate opp for cpu%d\n", cpu);
		ret = PTR_ERR(opp);
		goto out_disable_clock;
	}
	info->intermediate_voltage = dev_pm_opp_get_voltage(opp);
	dev_pm_opp_put(opp);

	info->opp_cpu = cpu;
	info->opp_nb.notifier_call = mtk_cpufreq_opp_notifier;
	ret = dev_pm_opp_register_notifier(cpu_dev, &info->opp_nb);
	if (ret) {
		pr_warn("cannot register opp notification\n");
		goto out_free_opp_table;
	}

	/* get possible max voltage */
	rate = ULONG_MAX;
	opp = dev_pm_opp_find_freq_floor(cpu_dev, &rate);
	if (IS_ERR(opp)) {
		pr_err("failed to get max frequency opp for cpu%d\n", cpu);
		ret = PTR_ERR(opp);
		goto out_free_opp_table;
	}
	info->max_volt_uv = dev_pm_opp_get_voltage(opp);
	dev_pm_opp_put(opp);

	info->cpu_dev = cpu_dev;
	info->proc_reg = proc_reg;
	info->sram_reg = IS_ERR(sram_reg) ? NULL : sram_reg;
	info->cpu_clk = cpu_clk;
	info->inter_clk = inter_clk;
	info->opp_freq = clk_get_rate(cpu_clk);

	/*
	 * If SRAM regulator is present, software "voltage tracking" is needed
	 * for this CPU power domain.
	 */
	info->need_voltage_tracking = !IS_ERR(sram_reg);

	return 0;

out_disable_clock:
	clk_disable_unprepare(inter_clk);

out_free_opp_table:
	dev_pm_opp_of_cpumask_remove_table(&info->cpus);

out_free_resources:
	if (!IS_ERR(proc_reg))
		regulator_put(proc_reg);
	if (!IS_ERR(sram_reg))
		regulator_put(sram_reg);
	if (!IS_ERR(cpu_clk))
		clk_put(cpu_clk);
	if (!IS_ERR(inter_clk))
		clk_put(inter_clk);

	return ret;
}

static void mtk_cpu_dvfs_info_release(struct mtk_cpu_dvfs_info *info)
{
	mutex_destroy(&info->lock);

	if (!IS_ERR(info->proc_reg))
		regulator_put(info->proc_reg);
	if (!IS_ERR(info->sram_reg))
		regulator_put(info->sram_reg);
	if (!IS_ERR(info->cpu_clk))
		clk_put(info->cpu_clk);
	if (!IS_ERR(info->inter_clk))
		clk_put(info->inter_clk);

	dev_pm_opp_of_cpumask_remove_table(&info->cpus);
}

unsigned int _mt_cpufreq_get_cpu_level(void)
{
	unsigned int level = CPU_LEVEL_0;
	int bonding, seg;

	/*
	 * 2.001GHz: CPU_LEVEL_0
	 * 2.201GHz: CPU_LEVEL_1
	 */

	bonding = get_devinfo_with_index(4) & 0xF00;
	seg = get_devinfo_with_index(30) & 0xFF;

	/* 2.201GHz: no speed bonding with segment id  */
	if (bonding == 0 && seg != 0)
		level = CPU_LEVEL_1;

	return level;
}

static unsigned int mtk_cpufreq_get(unsigned int cpu)
{
	struct mtk_cpu_dvfs_info *info;

	info = mtk_cpu_dvfs_info_lookup(cpu);
	if (!info)
		return 0;

	return info->opp_freq / 1000;
}

int __attribute__ ((weak)) mt_spower_init(void)
{
	pr_warn("%s() is not implemented\n", __func__);
	return 0;
}

int __attribute__ ((weak)) setup_power_table_tk(void)
{
	pr_warn("%s() is not implemented\n", __func__);
	return 0;
}

static int mtk_cpufreq_init(struct cpufreq_policy *policy)
{
	struct mtk_cpu_dvfs_info *info;
	struct cpufreq_frequency_table *freq_table;
	int ret;
	struct cpufreq_frequency_table *pos;
	unsigned int freq;
	unsigned long freq_long, rate;
	struct dev_pm_opp *opp;
	int opp_idx = 0;
	int lv;

	struct mtk_cpu_dvfs *p =  id_to_cpu_dvfs(MT_CPU_DVFS_L);

	info = mtk_cpu_dvfs_info_lookup(policy->cpu);
	if (!info) {
		pr_err("dvfs info for cpu%d is not initialized.\n",
		       policy->cpu);
		return -EINVAL;
	}

	/*
	 * 2.001GHz is default opp table in dts
	 * 2.201GHz is turbo opp table with opp table update
	 */
	lv = _mt_cpufreq_get_cpu_level();

	/* 2.201GHz: no speed bonding with segment id  */
	if (lv == CPU_LEVEL_1) {
		dev_pm_opp_remove(info->cpu_dev, 2001000000ul);
		dev_pm_opp_remove(info->cpu_dev, 1917000000ul);
		dev_pm_opp_remove(info->cpu_dev, 1834000000ul);
		dev_pm_opp_remove(info->cpu_dev, 1767000000ul);
		dev_pm_opp_remove(info->cpu_dev, 1700000000ul);
		dev_pm_opp_remove(info->cpu_dev, 1633000000ul);
		dev_pm_opp_remove(info->cpu_dev, 1056000000ul);
		dev_pm_opp_remove(info->cpu_dev,  987000000ul);
		dev_pm_opp_remove(info->cpu_dev,  918000000ul);

		dev_pm_opp_add(info->cpu_dev, 2201000000ul, 1025000ul);
		dev_pm_opp_add(info->cpu_dev, 2089000000ul,  993750ul);
		dev_pm_opp_add(info->cpu_dev, 1978000000ul,  962500ul);
		dev_pm_opp_add(info->cpu_dev, 1889000000ul,  937500ul);
		dev_pm_opp_add(info->cpu_dev, 1800000000ul,  912500ul);
		dev_pm_opp_add(info->cpu_dev, 1711000000ul,  887500ul);
		dev_pm_opp_add(info->cpu_dev, 1622000000ul,  862500ul);
		dev_pm_opp_add(info->cpu_dev, 1033000000ul,  700000ul);
		dev_pm_opp_add(info->cpu_dev,  941000000ul,  675000ul);
	}

	ret = dev_pm_opp_init_cpufreq_table(info->cpu_dev, &freq_table);
	if (ret) {
		pr_err("failed to init cpufreq table for cpu%d: %d\n",
		       policy->cpu, ret);
		return ret;
	}

	ret = cpufreq_table_validate_and_show(policy, freq_table);
	if (ret) {
		pr_err("%s: invalid frequency table: %d\n", __func__, ret);
		goto out_free_cpufreq_table;
	}

	rate = clk_get_rate(info->cpu_clk);

	cpufreq_for_each_valid_entry(pos, freq_table) {
		freq = pos->frequency;
		freq_long = freq * 1000;
		opp = dev_pm_opp_find_freq_ceil(info->cpu_dev, &freq_long);
		if (IS_ERR(opp)) {
			pr_err("failed to find OPP for %ld\n", freq_long);
			break;
		}
		opp_tbl_default[opp_idx].cpufreq_khz = freq;
		opp_tbl_default[opp_idx].cpufreq_volt =
			(int)dev_pm_opp_get_voltage(opp);
		dev_pm_opp_put(opp);

		if (freq_long == rate)
			p->idx_opp_tbl = opp_idx;

		opp_idx++;
	}

	p->opp_tbl = opp_tbl_default;
	p->nr_opp_tbl = dev_pm_opp_get_opp_count(info->cpu_dev);

	/* precondition: mt_spower_init() is finished */
	setup_power_table_tk();

	cpumask_copy(policy->cpus, &info->cpus);
	policy->freq_table = freq_table;
	policy->driver_data = info;
	policy->clk = info->cpu_clk;

	return 0;

out_free_cpufreq_table:
	dev_pm_opp_free_cpufreq_table(info->cpu_dev, &freq_table);
	return ret;
}

static int mtk_cpufreq_exit(struct cpufreq_policy *policy)
{
	struct mtk_cpu_dvfs_info *info = policy->driver_data;

	cpufreq_cooling_unregister(info->cdev);
	dev_pm_opp_free_cpufreq_table(info->cpu_dev, &policy->freq_table);

	return 0;
}

static struct cpufreq_driver mt8168_cpufreq_driver = {
	.flags = CPUFREQ_STICKY | CPUFREQ_NEED_INITIAL_FREQ_CHECK |
		 CPUFREQ_HAVE_GOVERNOR_PER_POLICY,
	.verify = cpufreq_generic_frequency_table_verify,
	.target_index = mtk_cpufreq_set_target,
	.get = mtk_cpufreq_get,
	.init = mtk_cpufreq_init,
	.exit = mtk_cpufreq_exit,
	.ready = mtk_cpufreq_ready,
	.name = "mtk-cpufreq",
	.attr = cpufreq_generic_attr,
};

static int mt8168_cpufreq_probe(struct platform_device *pdev)
{
	struct mtk_cpu_dvfs_info *info, *tmp;
	int cpu, ret;

	for_each_possible_cpu(cpu) {
		info = mtk_cpu_dvfs_info_lookup(cpu);
		if (info)
			continue;

		info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
		if (!info) {
			ret = -ENOMEM;
			goto release_dvfs_info_list;
		}

		ret = mtk_cpu_dvfs_info_init(info, cpu);
		if (ret) {
			dev_err(&pdev->dev,
				"failed to initialize dvfs info for cpu%d\n",
				cpu);
			goto release_dvfs_info_list;
		}

		list_add(&info->list_head, &dvfs_info_list);
	}

	ret = cpufreq_register_driver(&mt8168_cpufreq_driver);
	if (ret) {
		dev_err(&pdev->dev, "failed to register mtk cpufreq driver\n");
		goto release_dvfs_info_list;
	}

	return 0;

release_dvfs_info_list:
	list_for_each_entry_safe(info, tmp, &dvfs_info_list, list_head) {
		mtk_cpu_dvfs_info_release(info);
		list_del(&info->list_head);
	}

	return ret;
}

static struct platform_driver mt8168_cpufreq_platdrv = {
	.driver = {
		.name	= "mt8168-cpufreq",
	},
	.probe		= mt8168_cpufreq_probe,
};

static int mt8168_cpufreq_driver_init(void)
{
	struct platform_device *pdev;
	int err;

	if (!of_machine_is_compatible("mediatek,mt8168"))
		return -ENODEV;

	err = platform_driver_register(&mt8168_cpufreq_platdrv);
	if (err)
		return err;

	/*
	 * Since there's no place to hold device registration code and no
	 * device tree based way to match cpufreq driver yet, both the driver
	 * and the device registration codes are put here to handle defer
	 * probing.
	 */
	pdev = platform_device_register_simple("mt8168-cpufreq", -1, NULL, 0);
	if (IS_ERR(pdev)) {
		pr_err("failed to register mtk-cpufreq platform device\n");
		return PTR_ERR(pdev);
	}

	return 0;
}
device_initcall(mt8168_cpufreq_driver_init);
