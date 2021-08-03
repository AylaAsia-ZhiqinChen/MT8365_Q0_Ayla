/*
 * Copyright (C) 2016 MediaTek Inc.
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

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/pm_qos.h>
#include <linux/regulator/consumer.h>
#include <linux/workqueue.h>

#include "mtk_devinfo.h"
#include <mtk_vcorefs_manager.h>

#ifdef MTK_PERF_OBSERVER
#include <mt-plat/mtk_perfobserver.h>
#endif

#include <mt-plat/mtk_secure_api.h>

#define VPU_TRACE_ENABLED

/* #define BYPASS_M4U_DBG */

#include "vpu_algo.h"
#include "vpu_cmn.h"
#include "vpu_dbg.h"
#include "vpu_dvfs.h"
#include "vpu_hw.h"
#include "vpu_platform_config.h"
#include "vpu_pm.h"
#include "vpu_qos.h"
#include "vpu_reg.h"

/* To do : opp, mW */
struct VPU_OPP_INFO vpu_power_table[VPU_OPP_NUM] = {
	{VPU_OPP_0, 466},	/* 466.32            mW */
	{VPU_OPP_1, 342},	/* 341.5196153846154 mW */
	{VPU_OPP_2, 230},	/* 230.05            mW */
	{VPU_OPP_3, 209},	/* 209.3455          mw */
	{VPU_OPP_4, 155},	/* 155.34            mW */
	{VPU_OPP_5, 136},	/* 135.9225          mW */
	{VPU_OPP_6, 68},	/* 67.96125          mW */
	{VPU_OPP_7, 13},	/* 12.945            mW */
};

#define OPP_KEEP_TIME_MS	(3000)
#define OPPTYPE_VCORE		(0)
#define OPPTYPE_DSPFREQ		(1)

#define INIT_OPP_INDEX		(4)
#define DEFAULT_OPP_STEP	(INIT_OPP_INDEX)

#define VCORE_IDX_1		(1)
#define VCORE_IDX_2		(2)

#define VCORE_IDX_1_FREQ_UPPER_BOUND (2)
#define VCORE_IDX_2_FREQ_UPPER_BOUND (4)

#define FREQ_KHZ_TO_MHZ		(1000)

#define vcore_cg_ctl(poweron) \
	mt_secure_call(MTK_APU_VCORE_CG_CTL, poweron, 0, 0, 0)

static int g_dsp_freq_table[] = { /* KHz */
	624000,
	457000,
	400000,
	364000,
	312000,
	273000,
	136500,
	26000,
};

#define NUM_DSP_FREQ_TABLE	ARRAY_SIZE(g_dsp_freq_table)

static inline int map_dsp_freq_table(int freq_opp)
{
	int freq_value = 0;

	switch (freq_opp) {
	case 0 ... (NUM_DSP_FREQ_TABLE - 1):
		freq_value = g_dsp_freq_table[freq_opp];
		break;
	default:
		freq_value = g_dsp_freq_table[0];
		break;
	}

	return freq_value;
}

void vpu_get_opp_freq(struct vpu_device *vpu_device, int *vcore_opp,
			int *apu_freq, int *apu_if_freq)
{
	struct vpu_dvfs_opps *opps = &vpu_device->opps;

	mutex_lock(&vpu_device->opp_mutex);
	*vcore_opp = opps->vcore.index;
	*apu_freq = map_dsp_freq_table(opps->apu.index) / FREQ_KHZ_TO_MHZ;
	*apu_if_freq =
		map_dsp_freq_table(opps->apu_if.index) / FREQ_KHZ_TO_MHZ;
	mutex_unlock(&vpu_device->opp_mutex);
}

void vpu_opp_keep_routine(struct work_struct *work)
{
	struct vpu_device *vpu_device = container_of(work, struct vpu_device,
							opp_keep_work.work);

	LOG_DVFS("%s flag (%d) +\n", __func__, vpu_device->opp_keep_flag);
	mutex_lock(&vpu_device->opp_mutex);
	vpu_device->opp_keep_flag = false;
	mutex_unlock(&vpu_device->opp_mutex);
	LOG_DVFS("%s flag (%d) -\n", __func__, vpu_device->opp_keep_flag);
}

int get_vpu_bw(int core)
{
	return 0;
}

int get_vpu_latency(int core)
{
	return 0;
}

int get_vpu_opp(void)
{
	struct vpu_device *vpu_device = vpu_get_vpu_device();

	if (vpu_device == NULL)
		return 0;

	return vpu_device->opps.apu.index;
}
EXPORT_SYMBOL(get_vpu_opp);

int get_vpu_platform_floor_opp(void)
{
	return (VPU_MAX_NUM_OPPS - 1);
}
EXPORT_SYMBOL(get_vpu_platform_floor_opp);

int get_vpu_ceiling_opp(int core)
{
	struct vpu_core *vpu_core = NULL;

	vpu_core = vpu_get_vpu_core(core);
	if (vpu_core == NULL)
		return 0;
	return vpu_core->max_opp;
}
EXPORT_SYMBOL(get_vpu_ceiling_opp);

int get_vpu_opp_to_freq(uint8_t step)
{
	int freq = 0;

	switch (step) {
	case 0 ... (NUM_DSP_FREQ_TABLE - 1):
		freq = g_dsp_freq_table[step];
		break;
	default:
		LOG_ERR("wrong freq step(%d)", step);
		return -EINVAL;
	}
	return freq;
}
EXPORT_SYMBOL(get_vpu_opp_to_freq);

static int vpu_get_hw_vcore_opp(struct vpu_core *vpu_core)
{
	int core = vpu_core->core;

#ifdef MTK_VPU_FPGA_PORTING
	LOG_DBG("[vpu_%d] FPGA always return 0\n", core);

	return 0;
#else
	int opp_value = 0;
	s32 get_vcore_value = 0;

#ifdef ENABLE_PMQOS
	/* 8168 use pm_qos_request */
	get_vcore_value = pm_qos_request(PM_QOS_VCORE_OPP);
	if (get_vcore_value == VCORE_OPP_0)
		opp_value = 0;
	else if (get_vcore_value == VCORE_OPP_1)
		opp_value = 1;
	else if (get_vcore_value == VCORE_OPP_2)
		opp_value = 2;
	else
		opp_value = 2;
#endif
	LOG_DBG("[vpu_%d] vcore(%d->%d)\n", core, get_vcore_value, opp_value);

	return opp_value;
#endif
}

/* expected range, vvpu_index: 0~7 */
/* expected range, freq_index: 0~7 */
void vpu_opp_check(struct vpu_core *vpu_core, uint8_t vcore_index,
			uint8_t freq_index)
{
#ifdef MTK_VPU_FPGA_PORTING
	return;
#else
	struct vpu_device *vpu_device = vpu_core->vpu_device;
	struct vpu_dvfs_opps *opps = &vpu_device->opps;
	int core = vpu_core->core;
	bool freq_check = false;
	int log_freq = 0, log_max_freq = 0;
	int get_vcore_opp = 0;

	if (vpu_device->is_power_debug_lock) {
		vpu_core->force_change_vcore_opp = false;
		vpu_core->force_change_dsp_freq = false;
		goto out;
	}

	log_freq = map_dsp_freq_table(freq_index);

	LOG_DBG("opp_check + (%d/%d/%d), ori vcore(%d)\n", core,
			vcore_index, freq_index, opps->vcore.index);

	mutex_lock(&vpu_device->opp_mutex);
	vpu_core->change_freq_first = false;
	log_max_freq = map_dsp_freq_table(vpu_device->max_dsp_freq);

	/* vcore opp*/
	get_vcore_opp = vpu_get_hw_vcore_opp(vpu_core);
	if (vcore_index == 0xFF)
		vcore_index = opps->vcore.index;
	if (vcore_index < opps->vcore.opp_map[vpu_core->max_opp])
		vcore_index = opps->vcore.opp_map[vpu_core->max_opp];
	if (vcore_index > opps->vcore.opp_map[vpu_core->min_opp])
		vcore_index = opps->vcore.opp_map[vpu_core->min_opp];
	/* thermal */
	if (vcore_index < vpu_device->max_vcore_opp) {
		LOG_INF("vpu bound vcore opp(%d) to %d",
				vcore_index, vpu_device->max_vcore_opp);
		vcore_index = vpu_device->max_vcore_opp;
	}

	/* freq opp */
	if (freq_index == 0xFF)
		freq_index = opps->apu.index;
	if (freq_index < vpu_core->max_opp)
		freq_index = vpu_core->max_opp;
	if (freq_index > vpu_core->min_opp)
		freq_index = vpu_core->min_opp;
	/* thermal */
	if (freq_index < vpu_device->max_dsp_freq) {
		LOG_INF("vpu bound dsp freq(%dKHz) to %dKHz",
				log_freq, log_max_freq);
		freq_index = vpu_device->max_dsp_freq;
	}

	LOG_DVFS("opp_check + max_opp%d, min_opp%d,(%d/%d/%d), ori vcore(%d)",
			vpu_core->max_opp, vpu_core->min_opp, core,
			vcore_index, freq_index, opps->vcore.index);


	if ((vcore_index == opps->vcore.index)
			|| (vcore_index == get_vcore_opp)) {
		LOG_DBG("no need, vcore opp(%d), hw vore opp(%d)\n",
				vcore_index, get_vcore_opp);

		vpu_core->force_change_vcore_opp = false;
		opps->vcore.index = vcore_index;
	} else {
		/* opp down, need change freq first*/
		if (vcore_index > get_vcore_opp)
			vpu_core->change_freq_first = true;

		if (vcore_index >= opps->count) {
			LOG_ERR("wrong vcore opp(%d), max(%d)",
					vcore_index, opps->count - 1);

		} else if ((vcore_index < opps->vcore.index) ||
				((vcore_index > opps->vcore.index) &&
					(!vpu_device->opp_keep_flag))) {
			opps->vcore.index = vcore_index;
			vpu_core->force_change_vcore_opp = true;
			freq_check = true;
		}
	}

	/* dsp freq opp */
	if (freq_index == opps->apu.index) {
		LOG_DBG("no request, freq opp(%d)", freq_index);
		vpu_core->force_change_dsp_freq = false;
	} else {
		if ((opps->apu.index != freq_index) || (freq_check)) {
			/* freq_check for all vcore adjust related operation
			 * in acceptable region
			 */

			/* vcore not change and dsp change */
			if ((vpu_core->force_change_vcore_opp == false) &&
				(freq_index > opps->apu.index) &&
				(vpu_device->opp_keep_flag)) {
				if (vpu_device->vpu_log_level
					> Log_ALGO_OPP_INFO) {
					LOG_INF("%s(%d) %s (%d/%d_%d/%d)\n",
					__func__,
					core,
					"dsp keep high",
					vpu_core->force_change_vcore_opp,
					freq_index,
					opps->apu.index,
					vpu_device->opp_keep_flag);
				}
			} else {
				opps->apu.index = freq_index;

				if ((opps->vcore.index == VCORE_IDX_1) &&
					(opps->apu.index <
						VCORE_IDX_1_FREQ_UPPER_BOUND))
					opps->apu.index =
						VCORE_IDX_1_FREQ_UPPER_BOUND;

				if ((opps->vcore.index == VCORE_IDX_2) &&
					(opps->apu.index <
						VCORE_IDX_2_FREQ_UPPER_BOUND))
					opps->apu.index =
						VCORE_IDX_2_FREQ_UPPER_BOUND;

				opps->apu_if.index = opps->apu.index;
				vpu_core->force_change_dsp_freq = true;

				vpu_device->opp_keep_flag = true;
				mod_delayed_work(vpu_device->wq,
					&vpu_device->opp_keep_work,
					msecs_to_jiffies(OPP_KEEP_TIME_MS));
			}
		} else {
			/* vcore not change & dsp not change */
			if (vpu_device->vpu_log_level > Log_ALGO_OPP_INFO)
				LOG_INF("opp_check(%d) vcore/dsp no change\n",
						core);

			vpu_device->opp_keep_flag = true;

			mod_delayed_work(vpu_device->wq,
					&vpu_device->opp_keep_work,
					msecs_to_jiffies(OPP_KEEP_TIME_MS));
		}
	}

	mutex_unlock(&vpu_device->opp_mutex);
out:
	LOG_DVFS("%s(%d)(%d/%d_%d)(%d/%d)(%d.%d)(%d/%d)(%d/%d/%d/%d)%d\n",
		"opp_check",
		core,
		vpu_device->is_power_debug_lock,
		vcore_index,
		freq_index,
		opps->vcore.index,
		get_vcore_opp,
		opps->apu.index,
		opps->apu_if.index,
		vpu_device->max_vcore_opp,
		vpu_device->max_dsp_freq,
		freq_check,
		vpu_core->force_change_vcore_opp,
		vpu_core->force_change_dsp_freq,
		vpu_core->change_freq_first,
		vpu_device->opp_keep_flag);

	opps->index = opps->apu.index;
#endif
}

bool vpu_change_opp(struct vpu_core *vpu_core, int type)
{
#ifdef MTK_VPU_FPGA_PORTING
	LOG_INF("[vpu_%d] %d Skip at FPGA", core, type);

	return true;
#else
	struct vpu_device *vpu_device = vpu_core->vpu_device;
	struct vpu_dvfs_opps *opps = &vpu_device->opps;
	int ret = false;
	int core = vpu_core->core;
#if 0
	if (get_vvpu_DVFS_is_paused_by_ptpod()) {
		LOG_INF("[vpu_%d] dvfs skip by ptpod", core);
		return true;
	}
#endif
	switch (type) {
	/* vcore opp */
	case OPPTYPE_VCORE:
		LOG_DBG("[vpu_%d] wait for changing vcore opp", core);

		ret = wait_to_do_change_vcore_opp(vpu_core);
		if (ret) {
			LOG_ERR("[vpu_%d] timeout to %s, ret=%d\n",
				core,
				"wait_to_do_change_vcore_opp",
				ret);
			goto out;
		}
		LOG_DBG("[vpu_%d] to do vcore opp change", core);
		mutex_lock(&(vpu_core->state_mutex));
		vpu_core->state = VCT_VCORE_CHG;
		mutex_unlock(&(vpu_core->state_mutex));
		mutex_lock(&vpu_device->opp_mutex);
		vpu_trace_begin("vcore:request");

#ifdef ENABLE_PMQOS
		switch (opps->vcore.index) {
		case 0:
			pm_qos_update_request(&vpu_core->vpu_qos_vcore_request,
					      VCORE_OPP_0);
			break;
		case 1:
			pm_qos_update_request(&vpu_core->vpu_qos_vcore_request,
					      VCORE_OPP_1);
			break;
		case 2:
		default:
			pm_qos_update_request(&vpu_core->vpu_qos_vcore_request,
					      VCORE_OPP_2);
			break;
		}
#endif
		vpu_trace_end();

		mutex_lock(&(vpu_core->state_mutex));
		vpu_core->state = VCT_BOOTUP;
		mutex_unlock(&(vpu_core->state_mutex));
		if (ret) {
			LOG_ERR("[vpu_%d]fail to request vcore, step=%d\n",
					core, opps->vcore.index);
			goto out;
		}

		vpu_core->force_change_vcore_opp = false;
		mutex_unlock(&vpu_device->opp_mutex);
		wake_up_interruptible(&vpu_device->waitq_do_core_executing);
		break;
	/* dsp freq opp */
	case OPPTYPE_DSPFREQ:
		mutex_lock(&vpu_device->opp_mutex);
		LOG_DVFS("[vpu_%d] %s setclksrc(%d/%d)\n",
				core,
				__func__,
				opps->apu.index,
				opps->apu_if.index);

		ret = vpu_sel_src(&vpu_core->pm, opps->apu.index);
		if (ret) {
			LOG_ERR("[vpu_%d]fail apu freq, step=%d, ret=%d\n",
					core, opps->apu.index, ret);
			goto out;
		}

		vpu_core->force_change_dsp_freq = false;
		mutex_unlock(&vpu_device->opp_mutex);

#ifdef MTK_PERF_OBSERVER
		{
			struct pob_xpufreq_info pxi;

			pxi.id = core;
			pxi.opp = opps->index;

			pob_xpufreq_update(POB_XPUFREQ_VPU, &pxi);
		}
#endif

		break;
	default:
		LOG_DVFS("unexpected type(%d)", type);
		break;
	}

out:
#if defined(VPU_MET_READY)
	MET_Events_DVFS_Trace(vpu_device);
#endif
	return true;
#endif
}

int vpu_thermal_en_throttle_cb_set(struct vpu_device *vpu_device,
					uint8_t vcore_opp, uint8_t vpu_opp)
{
	struct vpu_core *vpu_core = NULL;
	int i = 0;
	int ret = 0;
	int vcore_opp_index = 0;
	int vpu_freq_index = 0;

#if 0
	bool vpu_down = true;

	for (i = 0 ; i < vpu_device->core_num ; i++) {
		mutex_lock(&power_counter_mutex[i]);
		if (power_counter[i] > 0)
			vpu_down = false;
		mutex_unlock(&power_counter_mutex[i]);
	}
	if (vpu_down) {
		LOG_INF("[vpu] all vpu are off currently, do nothing\n");
		return ret;
	}
#endif

#if 0
	if ((int)vpu_opp < 4) {
		vcore_opp_index = 0;
		vpu_freq_index = vpu_opp;
	} else {
		vcore_opp_index = 1;
		vpu_freq_index = vpu_opp;
	}
#else
	if (vpu_opp < VPU_MAX_NUM_OPPS) {
		vcore_opp_index = vpu_device->opps.vcore.opp_map[vpu_opp];
		vpu_freq_index = vpu_device->opps.apu.opp_map[vpu_opp];
	} else {
		LOG_ERR("vpu_thermal_en wrong opp(%d)\n", vpu_opp);
		return -1;
	}
#endif
	LOG_INF("%s, opp(%d)->(%d)\n", __func__,
		vpu_opp, vpu_freq_index);

	mutex_lock(&vpu_device->opp_mutex);
	vpu_device->max_vcore_opp = vcore_opp_index;
	vpu_device->max_dsp_freq = vpu_freq_index;
	mutex_unlock(&vpu_device->opp_mutex);

	for (i = 0 ; i < vpu_device->core_num ; i++) {
		vpu_core = vpu_device->vpu_core[i];
		mutex_lock(&vpu_device->opp_mutex);

		/* force change for all core under thermal request */
		vpu_device->opp_keep_flag = false;

		mutex_unlock(&vpu_device->opp_mutex);
		vpu_opp_check(vpu_core, vcore_opp_index, vpu_freq_index);
	}

	for (i = 0 ; i < vpu_device->core_num ; i++) {
		vpu_core = vpu_device->vpu_core[i];
		if (vpu_core->force_change_dsp_freq) {
			/* force change freq while running */
			LOG_INF("thermal force bound freq @%dKHz\n",
				g_dsp_freq_table[vpu_freq_index]);

			/*vpu_change_opp(i, OPPTYPE_DSPFREQ);*/
		}
	}

	return ret;
}

int vpu_thermal_dis_throttle_cb_set(struct vpu_device *vpu_device)
{
	int ret = 0;

	LOG_INF("%s +\n", __func__);
	mutex_lock(&vpu_device->opp_mutex);
	vpu_device->max_vcore_opp = 0;
	vpu_device->max_dsp_freq = 0;
	mutex_unlock(&vpu_device->opp_mutex);
	LOG_INF("%s -\n", __func__);

	return ret;
}

int vpu_enable_regulator_and_clock(struct vpu_core *vpu_core)
{
	struct vpu_device *vpu_device = vpu_core->vpu_device;
	int core = vpu_core->core;
	struct timespec start, end;
	uint64_t pm_on_latency = 0;

#ifdef MTK_VPU_FPGA_PORTING
	LOG_INF("%s skip at FPGA\n", __func__);

	vpu_core->is_power_on = true;
	vpu_core->force_change_vcore_opp = false;
	vpu_core->force_change_dsp_freq = false;
	return 0;
#else
	int ret = 0;
	int get_vcore_opp = 0;
	bool adjust_vcore = false;

	LOG_DBG("[vpu_%d] en_rc + (%d)\n", core,
		vpu_device->is_power_debug_lock);
	vpu_trace_begin("%s", __func__);

	get_vcore_opp = vpu_get_hw_vcore_opp(vpu_core);
	if (vpu_device->opps.vcore.index != get_vcore_opp)
		adjust_vcore = true;

	vpu_trace_begin("vcore:request");

	if (adjust_vcore) {
		LOG_DBG("[vpu_%d] en_rc wait for changing vcore opp", core);
		ret = wait_to_do_change_vcore_opp(vpu_core);
		if (ret) {
			/* skip change vcore in these time */
			LOG_WRN("[vpu_%d] timeout to %s(%d/%d), ret=%d\n",
				core,
				"wait_to_do_change_vcore_opp",
				vpu_device->opps.vcore.index,
				get_vcore_opp,
				ret);

			ret = 0;
			goto clk_on;
		}

		LOG_DBG("[vpu_%d] en_rc to do vcore opp change", core);

#ifdef ENABLE_PMQOS
		switch (vpu_device->opps.vcore.index) {
		case 0:
			pm_qos_update_request(&vpu_core->vpu_qos_vcore_request,
					      VCORE_OPP_0);
			break;
		case 1:
			pm_qos_update_request(&vpu_core->vpu_qos_vcore_request,
					      VCORE_OPP_1);
			break;
		case 2:
		default:
			pm_qos_update_request(&vpu_core->vpu_qos_vcore_request,
					      VCORE_OPP_2);
			break;
		}
#endif

	}

	vpu_trace_end();
	if (ret) {
		LOG_ERR("[vpu_%d]fail to request vcore, step=%d\n",
				core, vpu_device->opps.vcore.index);
		goto out;
	}

	LOG_DBG("[vpu_%d] en_rc setmmdvfs(%d) done\n", core,
		vpu_device->opps.vcore.index);

clk_on:
	ktime_get_ts(&start);
	vpu_pw_on(&vpu_core->pm);

	LOG_DBG("[vpu_%d] en_rc setclksrc(%d/%d)\n",
			core,
			vpu_device->opps.apu.index,
			vpu_device->opps.apu_if.index);

	ret = vpu_sel_src(&vpu_core->pm, vpu_device->opps.apu.index);
	if (ret) {
		LOG_ERR("[vpu_%d]fail to set apu freq, step=%d, ret=%d\n",
				core, vpu_device->opps.apu.index, ret);
		goto out;
	}

	ktime_get_ts(&end);

	pm_on_latency += (uint64_t)(timespec_to_ns(&end) -
			timespec_to_ns(&start));
	if (vpu_device->vpu_log_level > Log_STATE_MACHINE) {
		LOG_INF("[vpu_%d]:power on latency[%lld] ",
			core,
			pm_on_latency);
	}

out:
	vpu_trace_end();
	vpu_core->is_power_on = true;
	vpu_core->force_change_vcore_opp = false;
	vpu_core->force_change_dsp_freq = false;
	LOG_DBG("[vpu_%d] en_rc -\n", core);
	return ret;
#endif
}

#ifdef MTK_VPU_SMI_DEBUG_ON
static unsigned int vpu_read_smi_bus_debug(struct vpu_core *vpu_core)
{
	struct vpu_device *vpu_device = vpu_core->vpu_device;
	unsigned int smi_bus_value = 0x0;
	unsigned int smi_bus_vpu_value = 0x0;
	int core = vpu_core->core;

	if ((int)(vpu_device->smi_cmn_base) != 0) {
		switch (core) {
		case 0:
		default:
			smi_bus_value =
				vpu_read_reg32(vpu_device->smi_cmn_base,
							0x414);
			break;
		case 1:
			smi_bus_value =
				vpu_read_reg32(vpu_device->smi_cmn_base,
							0x418);
			break;
		}
		smi_bus_vpu_value = (smi_bus_value & 0x007FE000) >> 13;
	} else {
		LOG_INF("[vpu_%d] null smi_cmn_base\n", core);
	}
	LOG_INF("[vpu_%d] read_smi_bus (0x%x/0x%x)\n", core,
			smi_bus_value, smi_bus_vpu_value);

	return smi_bus_vpu_value;
}
#endif

int vpu_disable_regulator_and_clock(struct vpu_core *vpu_core)
{
	struct vpu_device *vpu_device = vpu_core->vpu_device;
	int ret = 0;
	int core = vpu_core->core;
	struct timespec start, end;
	uint64_t pm_off_latency = 0;

#ifdef MTK_VPU_FPGA_PORTING
	LOG_INF("%s skip at FPGA\n", __func__);

	vpu_core->is_power_on = false;
	return ret;
#else
	unsigned int smi_bus_vpu_value = 0x0;

	/* check there is un-finished transaction in bus before
	 * turning off vpu power
	 */
#ifdef MTK_VPU_SMI_DEBUG_ON
	smi_bus_vpu_value = vpu_read_smi_bus_debug(vpu_core);

	LOG_INF("[vpu_%d] dis_rc 1 (0x%x)\n", core, smi_bus_vpu_value);

	if ((int)smi_bus_vpu_value != 0) {
		mdelay(1);
		smi_bus_vpu_value = vpu_read_smi_bus_debug(vpu_core);

		LOG_INF("[vpu_%d] dis_rc again (0x%x)\n", core,
				smi_bus_vpu_value);

		if ((int)smi_bus_vpu_value != 0) {
			//smi_debug_bus_hanging_detect_ext2(0x1ff, 1, 0, 1);
			vpu_aee_warn("VPU SMI CHECK",
				"core_%d fail to check smi, value=%d\n",
				core,
				smi_bus_vpu_value);
		}
	}
#else
	LOG_DVFS("[vpu_%d] dis_rc + (0x%x)\n", core, smi_bus_vpu_value);
#endif

	ktime_get_ts(&start);
	vpu_pw_off(&vpu_core->pm);
	ktime_get_ts(&end);

	pm_off_latency += (uint64_t)(timespec_to_ns(&end) -
			timespec_to_ns(&start));
	if (vpu_device->vpu_log_level > Log_STATE_MACHINE) {
		LOG_INF("[vpu_%d]:power off latency[%lld] ",
			core,
			pm_off_latency);
	}

#ifdef ENABLE_PMQOS
	pm_qos_update_request(&vpu_core->vpu_qos_vcore_request,
			      VCORE_OPP_UNREQ);
#endif

	if (ret) {
		LOG_ERR("[vpu_%d]fail to unrequest vcore!\n", core);
		goto out;
	}
	LOG_DBG("[vpu_%d] disable result\n", core);
out:

	/*--disable regulator--*/
	vpu_core->is_power_on = false;
	if (vpu_device->vpu_log_level > Log_STATE_MACHINE)
		LOG_INF("[vpu_%d] dis_rc -\n", core);

	return ret;
#endif
}

void vpu_get_power_set_opp(struct vpu_core *vpu_core)
{
	struct vpu_device *vpu_device = vpu_core->vpu_device;
	int core = vpu_core->core;

	mutex_lock(&vpu_device->opp_mutex);
	if (vpu_core->change_freq_first) {
		LOG_DBG("[vpu_%d] change freq first(%d)\n",
				core, vpu_core->change_freq_first);
		/*mutex_unlock(&opp_mutex);*/
		/*mutex_lock(&opp_mutex);*/
		if (vpu_core->force_change_dsp_freq) {
			mutex_unlock(&vpu_device->opp_mutex);
			/* force change freq while running */
			LOG_DBG("vpu_%d force change dsp freq", core);
			vpu_change_opp(vpu_core, OPPTYPE_DSPFREQ);
		} else {
			mutex_unlock(&vpu_device->opp_mutex);
		}

		mutex_lock(&vpu_device->opp_mutex);
		if (vpu_core->force_change_vcore_opp) {
			mutex_unlock(&vpu_device->opp_mutex);
			/* vcore change should wait */
			LOG_DBG("vpu_%d force change vcore opp", core);
			vpu_change_opp(vpu_core, OPPTYPE_VCORE);
		} else {
			mutex_unlock(&vpu_device->opp_mutex);
		}
	} else {
		/*mutex_unlock(&opp_mutex);*/
		/*mutex_lock(&opp_mutex);*/
		if (vpu_core->force_change_vcore_opp) {
			mutex_unlock(&vpu_device->opp_mutex);
			/* vcore change should wait */
			LOG_DBG("vpu_%d force change vcore opp", core);
			vpu_change_opp(vpu_core, OPPTYPE_VCORE);
		} else {
			mutex_unlock(&vpu_device->opp_mutex);
		}

		mutex_lock(&vpu_device->opp_mutex);
		if (vpu_core->force_change_dsp_freq) {
			mutex_unlock(&vpu_device->opp_mutex);
			/* force change freq while running */
			LOG_DBG("vpu_%d force change dsp freq", core);
			vpu_change_opp(vpu_core, OPPTYPE_DSPFREQ);
		} else {
			mutex_unlock(&vpu_device->opp_mutex);
		}
	}
}

int vpu_set_power_set_opp(struct vpu_core *vpu_core, struct vpu_power *power)
{
	struct vpu_device *vpu_device = vpu_core->vpu_device;
	struct vpu_dvfs_opps *opps = &vpu_device->opps;
	int ret = 0;
	uint8_t vcore_opp_index = 0xFF;
	uint8_t dsp_freq_index = 0xFF;

	if (power->opp_step == 0xFF) {
		vcore_opp_index = 0xFF;
		dsp_freq_index = 0xFF;
	} else {
		if (power->opp_step < VPU_MAX_NUM_OPPS &&
							power->opp_step >= 0) {
			vcore_opp_index = opps->vcore.opp_map[power->opp_step];
			dsp_freq_index = opps->apu.opp_map[power->opp_step];
		} else {
			LOG_ERR("wrong opp step (%d)", power->opp_step);
			ret = -1;
			return ret;
		}
	}

	vpu_opp_check(vpu_core, vcore_opp_index, dsp_freq_index);

	return ret;
}

void vpu_init_opp(struct vpu_device *vpu_device)
{
	struct vpu_core *vpu_core = NULL;
	struct vpu_dvfs_opps *opps = NULL;
	int i;
	int vcore_map[VPU_MAX_NUM_STEPS] = {0, 0, 1, 1, 2, 2, 2, 2};

	vpu_device->max_vcore_opp = 0;
	vpu_device->max_dsp_freq = 0;
	vpu_device->opp_keep_flag = false;

	mutex_init(&vpu_device->opp_mutex);
	vpu_device->is_power_debug_lock = false;

	for (i = 0 ; i < vpu_device->core_num ; i++) {
		vpu_core = vpu_device->vpu_core[i];
		vpu_core->is_power_on = false;
		vpu_core->force_change_vcore_opp = false;
		vpu_core->force_change_dsp_freq = false;
		vpu_core->change_freq_first = false;
	}

	opps = &vpu_device->opps;
	/* opps->vcore.index = 3 - 1; */
	opps->vcore.count = 3;
	opps->vcore.values[0] = 800000;
	opps->vcore.values[1] = 700000;
	opps->vcore.values[2] = 650000;
	opps->vcore.values[3] = 0;
	opps->vcore.values[4] = 0;
	opps->vcore.values[5] = 0;
	opps->vcore.values[6] = 0;
	opps->vcore.values[7] = 0;

	/* opps->apu.index = 8 - 1; */
	opps->apu.count = 8;
	for (i = 0; i < VPU_MAX_NUM_STEPS; i++)
		opps->apu.values[i] = g_dsp_freq_table[i];

	/* opps->apu_if.index = 8 - 1; */
	opps->apu_if.count = 8;
	for (i = 0; i < VPU_MAX_NUM_STEPS; i++)
		opps->apu_if.values[i] = g_dsp_freq_table[i];

	/* default freq */
	for (i = 0; i < VPU_MAX_NUM_STEPS; i++) {
		opps->vcore.opp_map[i] = vcore_map[i];
		opps->apu.opp_map[i] = i;
		opps->apu_if.opp_map[i] = i;
	}

	/* default low opp */
	opps->count = 8;
	opps->index = INIT_OPP_INDEX; /* user space usage*/
	opps->vcore.index = 0;
	opps->apu.index = 0;
	opps->apu_if.index = 0;
}

void vpu_uninit_opp(struct vpu_device *vpu_device)
{
	cancel_delayed_work(&vpu_device->opp_keep_work);
}

void vpu_set_opp_check(struct vpu_core *vpu_core, struct vpu_request *req)
{
	struct vpu_device *vpu_device = vpu_core->vpu_device;
	struct vpu_dvfs_opps *opps = &vpu_device->opps;
	int i;
	int core = vpu_core->core;
	uint8_t vcore_opp_index = 0xFF;
	uint8_t dsp_freq_index = 0xFF;
	uint8_t opp_step = req->power_param.opp_step;

	if (opp_step == 0xFF) {
		vcore_opp_index = 0xFF;
		dsp_freq_index = 0xFF;
	} else {
		if (opp_step >= opps->count)
			opp_step = DEFAULT_OPP_STEP;

		vcore_opp_index = opps->vcore.opp_map[opp_step];
		dsp_freq_index = opps->apu.opp_map[opp_step];

		/* if running req priority < priority list,
		 * increase opp
		 */
		LOG_DBG("[vpu_%d] req->priority:%d\n", core, req->priority);
		for (i = 0 ; i < req->priority ; i++) {
			LOG_DBG("[vpu_%d] priority_list num[%d]:%d\n", core, i,
				vpu_core->priority_list[i]);
			if (vpu_core->priority_list[i] > 0) {
				LOG_INF("+ opp due to priority %d\n",
					req->priority);
				vcore_opp_index = 0;
				dsp_freq_index = 0;
				break;
			}
		}
	}

	LOG_DBG("[vpu_%d] run, opp(%d/%d)\n",
			core,
			opp_step,
			dsp_freq_index);

	vpu_opp_check(vpu_core, vcore_opp_index, dsp_freq_index);
}

void vpu_set_opp_all_index(struct vpu_device *vpu_device, uint8_t index)
{
	struct vpu_dvfs_opps *opps = &vpu_device->opps;

	opps->index		= opps->apu.opp_map[index];
	opps->vcore.index	= opps->vcore.opp_map[index];
	opps->apu.index		= opps->apu.opp_map[index];
	opps->apu_if.index	= opps->apu_if.opp_map[index];
}

int vpu_dump_opp_table(struct seq_file *s, struct vpu_device *gvpu_device)
{
	struct vpu_device *vpu_device = NULL;
	struct vpu_dvfs_opps *opps = NULL;
	int i;
	const char *line_bar =
"  +-----+----------+-----------+-----------+\n";

	if (gvpu_device) {
		vpu_device = gvpu_device;
	} else {
		if (s == NULL || s->private == NULL) {
			LOG_ERR("[vpu seq_file] seq_file error");
			return -EINVAL;
		}

		vpu_device = (struct vpu_device *)s->private;
	}

	opps = &vpu_device->opps;

	vpu_print_seq(s, "%s", line_bar);
	vpu_print_seq(s, "  |%-5s|%-10s|%-11s|%-11s|\n",
				"OPP", "VCORE(uV)", "APU(KHz)", "APU_IF(KHz)");
	vpu_print_seq(s, "%s", line_bar);

	for (i = 0; i < vpu_device->opps.count; i++) {
		vpu_print_seq(s,
				"  |%-5d|[%d]%-7d|[%d]%-8d|[%d]%-8d|\n",
				i,
				opps->vcore.opp_map[i],
				opps->vcore.values[opps->vcore.opp_map[i]],
				opps->apu.opp_map[i],
				opps->apu.values[opps->apu.opp_map[i]],
				opps->apu_if.opp_map[i],
				opps->apu_if.values[opps->apu_if.opp_map[i]]);
	}

	vpu_print_seq(s, "%s", line_bar);

	return 0;
}

uint8_t vpu_boost_value_to_opp(struct vpu_device *vpu_device,
				uint8_t boost_value)
{
	struct vpu_dvfs_opps *opps = &vpu_device->opps;
	int ret = 0;

	uint32_t freq	= 0;
	uint32_t freq0	= opps->apu.values[0];
	uint32_t freq1	= opps->apu.values[1];
	uint32_t freq2	= opps->apu.values[2];
	uint32_t freq3	= opps->apu.values[3];
	uint32_t freq4	= opps->apu.values[4];
	uint32_t freq5	= opps->apu.values[5];
	uint32_t freq6	= opps->apu.values[6];
	uint32_t freq7	= opps->apu.values[7];

	if ((boost_value <= 100) && (boost_value >= 0))
		freq = boost_value * freq0 / 100;
	else
		freq = freq0;

	if (freq <= freq0 && freq > freq1)
		ret = 0;
	else if (freq <= freq1 && freq > freq2)
		ret = 1;
	else if (freq <= freq2 && freq > freq3)
		ret = 2;
	else if (freq <= freq3 && freq > freq4)
		ret = 3;
	else if (freq <= freq4 && freq > freq5)
		ret = 4;
	else if (freq <= freq5 && freq > freq6)
		ret = 5;
	else if (freq <= freq6 && freq > freq7)
		ret = 6;
	else
		ret = 7;

	LOG_DVFS("%s opp %d\n", __func__, ret);
	return ret;
}
