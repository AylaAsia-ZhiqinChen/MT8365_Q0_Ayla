// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#include <linux/fb.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <linux/string.h>

#include <mtk_dramc.h>
#include <mt-plat/upmu_common.h>

#include <helio-dvfsrc.h>
#include <helio-dvfsrc-opp.h>
#include <mtk_dvfsrc_reg.h>
#include <mtk_gpufreq.h>
#include <mt-plat/mtk_devinfo.h>

#ifdef CONFIG_MTK_SMI_EXT
#include <mmdvfs_mgr.h>
#endif

#define AUTOK_ENABLE

static int spmfw_idx = -1;

#ifdef CONFIG_MTK_DRAMC
static void spm_dram_type_check(void)
{
	int ddr_type = get_ddr_type();
	int ddr_hz = dram_steps_freq(0);

	if (ddr_type == TYPE_LPDDR4 && ddr_hz == 3200)
		spmfw_idx = SPMFW_LP4_2CH_3200;
	else if (ddr_type == TYPE_LPDDR4X && ddr_hz == 3200)
		spmfw_idx = SPMFW_LP4X_2CH_3200;
	else if (ddr_type == TYPE_LPDDR3)
		spmfw_idx = SPMFW_LP3_1CH_1600;
	else if (ddr_type == TYPE_PCDDR3)
		spmfw_idx = SPMFW_PC3_1CH_1333;
	else if (ddr_type == TYPE_PCDDR4)
		spmfw_idx = SPMFW_PC4_2CH_2667;
	else
		spmfw_idx = SPMFW_PC4_1CH_1333;

	pr_info("#@# %s(%d) __spmfw_idx 0x%x, ddr=[%d][%d]\n",
		__func__, __LINE__, spmfw_idx, ddr_type, ddr_hz);
}
#endif /* CONFIG_MTK_DRAMC */

int spm_get_spmfw_idx(void)
{
	if (spmfw_idx == -1) {
		spmfw_idx++;
#ifdef CONFIG_MTK_DRAMC
		spm_dram_type_check();
#endif /* CONFIG_MTK_DRAMC */
	}

	return spmfw_idx;
}

static struct reg_config dvfsrc_init_configs[][128] = {
	/* SPMFW_LP4_2CH_3200 */
	{
		{ DVFSRC_EMI_REQUEST,		0x00000009 },
		{ DVFSRC_EMI_REQUEST3,		0x09000000 },
		{ DVFSRC_EMI_QOS0,		0x00000032 },
		{ DVFSRC_EMI_QOS1,		0x00000055 },

		{ DVFSRC_VCORE_HRT,		0x00000036 },
		{ DVFSRC_EMI_HRT,		0x003E362C },

		{ DVFSRC_TIMEOUT_NEXTREQ,	0x00000014 },
		{ DVFSRC_INT_EN,		0x00000003 },

		{ DVFSRC_LEVEL_LABEL_0_1,	0x00010000 },
		{ DVFSRC_LEVEL_LABEL_2_3,	0x00120011 },
		{ DVFSRC_LEVEL_LABEL_4_5,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_6_7,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_8_9,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_10_11,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_12_13,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_14_15,	0x00220022 },

		{ DVFSRC_SW_REQ, 0x00000009 },
		{ DVFSRC_FORCE, 0x00100000 },
		{ DVFSRC_QOS_EN, 0x0000407F },
		{ DVFSRC_BASIC_CONTROL, 0x0000C07B },
		{ DVFSRC_BASIC_CONTROL, 0x0000017B },
		{ DVFSRC_FORCE, 0x00000000 },

		{ -1, 0 },
	},
	/* SPMFW_LP4X_2CH_3200 */
	{
		{ DVFSRC_EMI_REQUEST,		0x00000009 },
		{ DVFSRC_EMI_REQUEST3,		0x09000000 },
		{ DVFSRC_EMI_QOS0,		0x00000032 },
		{ DVFSRC_EMI_QOS1,		0x00000055 },

		{ DVFSRC_VCORE_HRT,		0x00000036 },
		{ DVFSRC_EMI_HRT,		0x003E362C },

		{ DVFSRC_TIMEOUT_NEXTREQ,	0x00000014 },
		{ DVFSRC_INT_EN,		0x00000003 },

		{ DVFSRC_LEVEL_LABEL_0_1,	0x00010000 },
		{ DVFSRC_LEVEL_LABEL_2_3,	0x00120011 },
		{ DVFSRC_LEVEL_LABEL_4_5,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_6_7,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_8_9,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_10_11,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_12_13,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_14_15,	0x00220022 },

		{ DVFSRC_SW_REQ, 0x00000009 },
		{ DVFSRC_FORCE, 0x00100000 },
		{ DVFSRC_QOS_EN, 0x0000407F },
		{ DVFSRC_BASIC_CONTROL, 0x0000C07B },
		{ DVFSRC_BASIC_CONTROL, 0x0000017B },
		{ DVFSRC_FORCE, 0x00000000 },

		{ -1, 0 },
	},
	/* SPMFW_LP3_1CH_1600 */
	{
		{ DVFSRC_EMI_REQUEST,		0x00000009 },
		{ DVFSRC_EMI_REQUEST3,		0x09000000 },
		{ DVFSRC_EMI_QOS0,		0x00000026 },
		{ DVFSRC_EMI_QOS1,		0x0000002a },

		{ DVFSRC_VCORE_HRT,		0x00000020 },
		{ DVFSRC_EMI_HRT,		0x00000020 },

		{ DVFSRC_TIMEOUT_NEXTREQ,	0x00000014 },
		{ DVFSRC_INT_EN,		0x00000003 },

		{ DVFSRC_LEVEL_LABEL_0_1,	0x00010000 },
		{ DVFSRC_LEVEL_LABEL_2_3,	0x00120011 },
		{ DVFSRC_LEVEL_LABEL_4_5,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_6_7,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_8_9,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_10_11,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_12_13,	0x00220022 },
		{ DVFSRC_LEVEL_LABEL_14_15,	0x00220022 },

		{ DVFSRC_SW_REQ, 0x00000009 },
		{ DVFSRC_FORCE, 0x00100000 },
		{ DVFSRC_QOS_EN, 0x0000407F },
		{ DVFSRC_BASIC_CONTROL, 0x0000C07B },
		{ DVFSRC_BASIC_CONTROL, 0x0000017B },
		{ DVFSRC_FORCE, 0x00000000 },

		{ -1, 0 },
	},
	/* SPMFW_PC4_2CH_2667 */
	{
		{ DVFSRC_TIMEOUT_NEXTREQ,	0x00000014 },
		{ DVFSRC_INT_EN,		0x00000003 },

		{ DVFSRC_LEVEL_LABEL_0_1,	0x00010000 },
		{ DVFSRC_LEVEL_LABEL_2_3,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_4_5,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_6_7,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_8_9,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_10_11,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_12_13,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_14_15,	0x00010001 },

		{ DVFSRC_FORCE, 0x00020000 },
		{ DVFSRC_QOS_EN, 0x0000407F },
		{ DVFSRC_BASIC_CONTROL, 0x0000C07B },
		{ DVFSRC_BASIC_CONTROL, 0x0000017B },
		{ DVFSRC_FORCE, 0x00000000 },

		{ -1, 0 },
	},
	/* SPMFW_PC4_1CH_1333 */
	{
		{ DVFSRC_TIMEOUT_NEXTREQ,	0x00000014 },
		{ DVFSRC_INT_EN,		0x00000003 },

		{ DVFSRC_LEVEL_LABEL_0_1,	0x00010000 },
		{ DVFSRC_LEVEL_LABEL_2_3,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_4_5,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_6_7,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_8_9,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_10_11,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_12_13,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_14_15,	0x00010001 },

		{ DVFSRC_FORCE, 0x00020000 },
		{ DVFSRC_QOS_EN, 0x0000407F },
		{ DVFSRC_BASIC_CONTROL, 0x0000C07B },
		{ DVFSRC_BASIC_CONTROL, 0x0000017B },
		{ DVFSRC_FORCE, 0x00000000 },

		{ -1, 0 },
	},
	/* SPMFW_PC3_1CH_1333 */
	{
		{ DVFSRC_TIMEOUT_NEXTREQ,	0x00000014 },
		{ DVFSRC_INT_EN,		0x00000003 },

		{ DVFSRC_LEVEL_LABEL_0_1,	0x00010000 },
		{ DVFSRC_LEVEL_LABEL_2_3,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_4_5,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_6_7,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_8_9,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_10_11,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_12_13,	0x00010001 },
		{ DVFSRC_LEVEL_LABEL_14_15,	0x00010001 },

		{ DVFSRC_FORCE, 0x00020000 },
		{ DVFSRC_QOS_EN, 0x0000407F },
		{ DVFSRC_BASIC_CONTROL, 0x0000C07B },
		{ DVFSRC_BASIC_CONTROL, 0x0000017B },
		{ DVFSRC_FORCE, 0x00000000 },

		{ -1, 0 },
	},
	/* NULL */
	{
		{ -1, 0 },
	},
};

struct reg_config *dvfsrc_get_init_conf(void)
{
	int spmfw_idx = spm_get_spmfw_idx();

	if (spmfw_idx < 0)
		spmfw_idx = ARRAY_SIZE(dvfsrc_init_configs) - 1;

	return dvfsrc_init_configs[spmfw_idx];
}

void dvfsrc_opp_level_mapping(void)
{
	int vcore_opp_0_uv, vcore_opp_1_uv, vcore_opp_2_uv;

	set_pwrap_cmd(VCORE_OPP_2, 0);
	set_pwrap_cmd(VCORE_OPP_1, 1);
	set_pwrap_cmd(VCORE_OPP_0, 2);

	vcore_opp_0_uv = 650000;
	vcore_opp_1_uv = 700000;
	vcore_opp_2_uv = 800000;

	set_vcore_uv_table(VCORE_OPP_2, vcore_opp_0_uv);
	set_vcore_uv_table(VCORE_OPP_1, vcore_opp_1_uv);
	set_vcore_uv_table(VCORE_OPP_0, vcore_opp_2_uv);

	switch (spm_get_spmfw_idx()) {
	case SPMFW_LP4X_2CH_3200:
	case SPMFW_LP4_2CH_3200:
	case SPMFW_LP3_1CH_1600:
		helio_dvfsrc_flag_set(SPM_FLAG_RUN_COMMON_SCENARIO |
				      SPM_FLAG_DRAM_LPDDR_TYPE);
		set_vcore_opp(VCORE_DVFS_OPP_0, VCORE_OPP_UNREQ);
		set_vcore_opp(VCORE_DVFS_OPP_1, VCORE_OPP_0);
		set_vcore_opp(VCORE_DVFS_OPP_2, VCORE_OPP_0);
		set_vcore_opp(VCORE_DVFS_OPP_3, VCORE_OPP_1);
		set_vcore_opp(VCORE_DVFS_OPP_4, VCORE_OPP_1);
		set_vcore_opp(VCORE_DVFS_OPP_5, VCORE_OPP_2);

		set_ddr_opp(VCORE_DVFS_OPP_0, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_1, DDR_OPP_0);
		set_ddr_opp(VCORE_DVFS_OPP_2, DDR_OPP_1);
		set_ddr_opp(VCORE_DVFS_OPP_3, DDR_OPP_1);
		set_ddr_opp(VCORE_DVFS_OPP_4, DDR_OPP_2);
		set_ddr_opp(VCORE_DVFS_OPP_5, DDR_OPP_2);
		pr_info("%s: LPDDR vcore_opp_uv %d, %d, %d\n", __func__,
			vcore_opp_0_uv, vcore_opp_1_uv, vcore_opp_2_uv);
		break;
	case SPMFW_PC4_2CH_2667:
	case SPMFW_PC4_1CH_1333:
	case SPMFW_PC3_1CH_1333:
		helio_dvfsrc_flag_set(SPM_FLAG_RUN_COMMON_SCENARIO |
				      SPM_FLAG_DIS_VCORE_DFS);
		set_vcore_opp(VCORE_DVFS_OPP_0, VCORE_OPP_UNREQ);
		set_vcore_opp(VCORE_DVFS_OPP_1, VCORE_OPP_0);
		set_vcore_opp(VCORE_DVFS_OPP_2, VCORE_OPP_0);
		set_vcore_opp(VCORE_DVFS_OPP_3, VCORE_OPP_0);
		set_vcore_opp(VCORE_DVFS_OPP_4, VCORE_OPP_0);
		set_vcore_opp(VCORE_DVFS_OPP_5, VCORE_OPP_1);

		set_ddr_opp(VCORE_DVFS_OPP_0, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_1, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_2, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_3, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_4, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_5, DDR_OPP_UNREQ);
		pr_info("%s: PCDDR vcore_opp_uv %d, %d\n", __func__,
			vcore_opp_1_uv, vcore_opp_2_uv);
		break;
	default:
		set_vcore_opp(VCORE_DVFS_OPP_0, VCORE_OPP_UNREQ);
		set_vcore_opp(VCORE_DVFS_OPP_1, VCORE_OPP_UNREQ);
		set_vcore_opp(VCORE_DVFS_OPP_2, VCORE_OPP_UNREQ);
		set_vcore_opp(VCORE_DVFS_OPP_3, VCORE_OPP_UNREQ);
		set_vcore_opp(VCORE_DVFS_OPP_4, VCORE_OPP_UNREQ);
		set_vcore_opp(VCORE_DVFS_OPP_5, VCORE_OPP_UNREQ);

		set_ddr_opp(VCORE_DVFS_OPP_0, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_1, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_2, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_3, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_4, DDR_OPP_UNREQ);
		set_ddr_opp(VCORE_DVFS_OPP_5, DDR_OPP_UNREQ);

		break;
	}
}

static int dvfsrc_fb_notifier_call(struct notifier_block *self,
		unsigned long event, void *data)
{
	struct fb_event *evdata = data;
	int blank;

	if (event != FB_EVENT_BLANK)
		return 0;

	blank = *(int *)evdata->data;

	switch (blank) {
	case FB_BLANK_UNBLANK:
		break;
	case FB_BLANK_POWERDOWN:
		break;
	default:
		break;
	}

	return 0;
}

static struct notifier_block dvfsrc_fb_notifier = {
	.notifier_call = dvfsrc_fb_notifier_call,
};

#ifdef AUTOK_ENABLE
__weak int emmc_autok(void)
{
	pr_info("NOT SUPPORT EMMC AUTOK\n");
	return 0;
}

__weak int sd_autok(void)
{
	pr_info("NOT SUPPORT SD AUTOK\n");
	return 0;
}

__weak int sdio_autok(void)
{
	pr_info("NOT SUPPORT SDIO AUTOK\n");
	return 0;
}

__weak void mt_gpufreq_disable_by_ptpod(void)
{
	pr_info("NOT SUPPORT GPUFREQ\n");
}

__weak void mt_gpufreq_enable_by_ptpod(void)
{
	pr_info("NOT SUPPORT GPUFREQ\n");
}

void begin_autok_task(void)
{
#if defined CONFIG_MTK_SMI_EXT
	struct mmdvfs_prepare_event evt_from_vcore = {
		MMDVFS_EVENT_PREPARE_CALIBRATION_START};
#endif

#if defined CONFIG_MTK_SMI_EXT
	/* notify MM DVFS for msdc autok start */
	mmdvfs_notify_prepare_action(&evt_from_vcore);
#endif
	/* notify GPU DVFS for msdc autok start */
	mt_gpufreq_disable_by_ptpod();
}

void finish_autok_task(void)
{
	/* check if dvfs force is released */
	int force = pm_qos_request(PM_QOS_VCORE_DVFS_FIXED_OPP);
#if defined CONFIG_MTK_SMI_EXT
	struct mmdvfs_prepare_event evt_from_vcore = {
		MMDVFS_EVENT_PREPARE_CALIBRATION_END};
#endif

#if defined CONFIG_MTK_SMI_EXT
	/* notify MM DVFS for msdc autok finish */
	mmdvfs_notify_prepare_action(&evt_from_vcore);
#endif

	/* notify GPU DVFS for msdc autok finish */
	mt_gpufreq_enable_by_ptpod();

	if (force >= 0 && force < 16)
		pr_info("autok task not release force opp: %d\n", force);
}

void dvfsrc_autok_manager(void)
{
	int r = 0;

	begin_autok_task();

	r = emmc_autok();
	pr_info("EMMC autok done: %s\n", (r == 0) ? "Yes" : "No");

	r = sd_autok();
	pr_info("SD autok done: %s\n", (r == 0) ? "Yes" : "No");

	r = sdio_autok();
	pr_info("SDIO autok done: %s\n", (r == 0) ? "Yes" : "No");

	finish_autok_task();
}
#endif

__weak int mtk_rgu_cfg_dvfsrc(int enable)
{
	pr_info("%s: NOT SUPPORT\n", __func__);
	return 0;
}

int helio_dvfsrc_platform_init(struct helio_dvfsrc *dvfsrc)
{
	int val;
	char buf[1300];

	memset(buf, '\0', sizeof(buf));
	get_dvfsrc_reg(buf);
	pr_info("%s\n", buf);

	memset(buf, '\0', sizeof(buf));
	get_dvfsrc_record(buf);
	pr_info("%s\n", buf);

	memset(buf, '\0', sizeof(buf));
	get_dvfsrc_record_latch(buf);
	pr_info("%s\n", buf);

	mtk_rgu_cfg_dvfsrc(1);
	helio_dvfsrc_enable(1);
	val = dvfsrc_read(DVFSRC_SW_REQ);
	dvfsrc->init_config = dvfsrc_get_init_conf();
	helio_dvfsrc_reg_config(dvfsrc->init_config);
	pr_info("%s: restore sw_req 0x%x\n", __func__, val);
	dvfsrc_write(DVFSRC_SW_REQ, val);

#ifdef AUTOK_ENABLE
	dvfsrc_autok_manager();
#endif

	return fb_register_client(&dvfsrc_fb_notifier);
}

void get_opp_info(char *p)
{
	int pmic_val = pmic_get_register_value(PMIC_DA_VCORE_VOSEL);
	int sram_val = pmic_get_register_value(PMIC_DA_VSRAM_PROC_VOSEL);
	int vcore_uv = vcore_pmic_to_uv(pmic_val);
	int sram_uv = vcore_pmic_to_uv(sram_val);
#ifdef CONFIG_MTK_DRAMC
	int ddr_khz = get_dram_data_rate() * 1000;
#endif
	p += sprintf(p, "%-24s: %-8u uv  (PMIC: 0x%x)\n",
			"Vcore", vcore_uv, vcore_uv_to_pmic(vcore_uv));
	p += sprintf(p, "%-24s: %-8u uv  (PMIC: 0x%x)\n",
			"Vcore_sram", sram_uv, vcore_uv_to_pmic(sram_uv));
#ifdef CONFIG_MTK_DRAMC
	p += sprintf(p, "%-24s: %-8u kbps\n", "DDR", ddr_khz);
	p += sprintf(p, "%-24s: %-8u\n", "DDR_TYPE", get_ddr_type());
#endif
	p += sprintf(p, "%-24s: %-8u\n", "PTPOD10",
			get_devinfo_with_index(56));
	p += sprintf(p, "%-24s: %-8u\n", "INFO3",
			get_devinfo_with_index(133));
	p += sprintf(p, "%-24s: %-8u\n", "hw_opp",
			get_cur_vcore_dvfs_opp() - 1);
}

void get_dvfsrc_reg(char *p)
{
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_BASIC_CONTROL",
			dvfsrc_read(DVFSRC_BASIC_CONTROL));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x\n",
			"DVFSRC_SW_REQ(2)",
			dvfsrc_read(DVFSRC_SW_REQ),
			dvfsrc_read(DVFSRC_SW_REQ2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_EMI_QOS0(1)(2)",
			dvfsrc_read(DVFSRC_EMI_QOS0),
			dvfsrc_read(DVFSRC_EMI_QOS1),
			dvfsrc_read(DVFSRC_EMI_QOS2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_EMI_REQUEST(2)(3)",
			dvfsrc_read(DVFSRC_EMI_REQUEST),
			dvfsrc_read(DVFSRC_EMI_REQUEST2),
			dvfsrc_read(DVFSRC_EMI_REQUEST3));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_EMI_MD2SPM0~2",
			dvfsrc_read(DVFSRC_EMI_MD2SPM0),
			dvfsrc_read(DVFSRC_EMI_MD2SPM1),
			dvfsrc_read(DVFSRC_EMI_MD2SPM2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_EMI_MD2SPM0~2_T",
			dvfsrc_read(DVFSRC_EMI_MD2SPM0_T),
			dvfsrc_read(DVFSRC_EMI_MD2SPM1_T),
			dvfsrc_read(DVFSRC_EMI_MD2SPM2_T));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x\n",
			"DVFSRC_VCORE_REQUEST(2)",
			dvfsrc_read(DVFSRC_VCORE_REQUEST),
			dvfsrc_read(DVFSRC_VCORE_REQUEST2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_VCORE_MD2SPM0~2",
			dvfsrc_read(DVFSRC_VCORE_MD2SPM0),
			dvfsrc_read(DVFSRC_VCORE_MD2SPM1),
			dvfsrc_read(DVFSRC_VCORE_MD2SPM2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_VCORE_MD2SPM0~2_T",
			dvfsrc_read(DVFSRC_VCORE_MD2SPM0_T),
			dvfsrc_read(DVFSRC_VCORE_MD2SPM1_T),
			dvfsrc_read(DVFSRC_VCORE_MD2SPM2_T));
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_MD_REQUEST",
			dvfsrc_read(DVFSRC_MD_REQUEST));
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_INT",
			dvfsrc_read(DVFSRC_INT));
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_INT_EN",
			dvfsrc_read(DVFSRC_INT_EN));
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_LEVEL",
			dvfsrc_read(DVFSRC_LEVEL));
	p += sprintf(p, "%-24s: %d, %d, %d, %d, %d\n",
			"DVFSRC_SW_BW_0~4",
			dvfsrc_read(DVFSRC_SW_BW_0),
			dvfsrc_read(DVFSRC_SW_BW_1),
			dvfsrc_read(DVFSRC_SW_BW_2),
			dvfsrc_read(DVFSRC_SW_BW_3),
			dvfsrc_read(DVFSRC_SW_BW_4));
}

void get_dvfsrc_record(char *p)
{
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_FORCE",
			dvfsrc_read(DVFSRC_FORCE));
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_SEC_SW_REQ",
			dvfsrc_read(DVFSRC_SEC_SW_REQ));
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_LAST",
			dvfsrc_read(DVFSRC_LAST));
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_MD_SCENARIO",
			dvfsrc_read(DVFSRC_MD_SCENARIO));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_0_0~0_2",
			dvfsrc_read(DVFSRC_RECORD_0_0),
			dvfsrc_read(DVFSRC_RECORD_0_1),
			dvfsrc_read(DVFSRC_RECORD_0_2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_1_0~1_2",
			dvfsrc_read(DVFSRC_RECORD_1_0),
			dvfsrc_read(DVFSRC_RECORD_1_1),
			dvfsrc_read(DVFSRC_RECORD_1_2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_2_0~2_2",
			dvfsrc_read(DVFSRC_RECORD_2_0),
			dvfsrc_read(DVFSRC_RECORD_2_1),
			dvfsrc_read(DVFSRC_RECORD_2_2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_3_0~3_2",
			dvfsrc_read(DVFSRC_RECORD_3_0),
			dvfsrc_read(DVFSRC_RECORD_3_1),
			dvfsrc_read(DVFSRC_RECORD_3_2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_4_0~4_2",
			dvfsrc_read(DVFSRC_RECORD_4_0),
			dvfsrc_read(DVFSRC_RECORD_4_1),
			dvfsrc_read(DVFSRC_RECORD_4_2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_5_0~5_2",
			dvfsrc_read(DVFSRC_RECORD_5_0),
			dvfsrc_read(DVFSRC_RECORD_5_1),
			dvfsrc_read(DVFSRC_RECORD_5_2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_6_0~6_2",
			dvfsrc_read(DVFSRC_RECORD_6_0),
			dvfsrc_read(DVFSRC_RECORD_6_1),
			dvfsrc_read(DVFSRC_RECORD_6_2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_7_0~7_2",
			dvfsrc_read(DVFSRC_RECORD_7_0),
			dvfsrc_read(DVFSRC_RECORD_7_1),
			dvfsrc_read(DVFSRC_RECORD_7_2));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_MD_0~3",
			dvfsrc_read(DVFSRC_RECORD_MD_0),
			dvfsrc_read(DVFSRC_RECORD_MD_1),
			dvfsrc_read(DVFSRC_RECORD_MD_2),
			dvfsrc_read(DVFSRC_RECORD_MD_3));
	p += sprintf(p, "%-24s: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_MD_4~7",
			dvfsrc_read(DVFSRC_RECORD_MD_4),
			dvfsrc_read(DVFSRC_RECORD_MD_5),
			dvfsrc_read(DVFSRC_RECORD_MD_6),
			dvfsrc_read(DVFSRC_RECORD_MD_7));
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_RECORD_COUNT",
			dvfsrc_read(DVFSRC_RECORD_COUNT));
	p += sprintf(p, "%-24s: 0x%08x\n",
			"DVFSRC_RSRV_0",
			dvfsrc_read(DVFSRC_RSRV_0));
}

void get_dvfsrc_record_latch(char *p)
{
	p += sprintf(p, "%-25s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_0_L_0~0_L_2",
			dvfsrc_read(DVFSRC_RECORD_0_L_0),
			dvfsrc_read(DVFSRC_RECORD_0_L_1),
			dvfsrc_read(DVFSRC_RECORD_0_L_2));
	p += sprintf(p, "%-25s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_1_L_0~1_L_2",
			dvfsrc_read(DVFSRC_RECORD_1_L_0),
			dvfsrc_read(DVFSRC_RECORD_1_L_1),
			dvfsrc_read(DVFSRC_RECORD_1_L_2));
	p += sprintf(p, "%-25s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_2_L_0~2_L_2",
			dvfsrc_read(DVFSRC_RECORD_2_L_0),
			dvfsrc_read(DVFSRC_RECORD_2_L_1),
			dvfsrc_read(DVFSRC_RECORD_2_L_2));
	p += sprintf(p, "%-25s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_3_L_0~3_L_2",
			dvfsrc_read(DVFSRC_RECORD_3_L_0),
			dvfsrc_read(DVFSRC_RECORD_3_L_1),
			dvfsrc_read(DVFSRC_RECORD_3_L_2));
	p += sprintf(p, "%-25s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_4_L_0~4_L_2",
			dvfsrc_read(DVFSRC_RECORD_4_L_0),
			dvfsrc_read(DVFSRC_RECORD_4_L_1),
			dvfsrc_read(DVFSRC_RECORD_4_L_2));
	p += sprintf(p, "%-25s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_5_L_0~5_L_2",
			dvfsrc_read(DVFSRC_RECORD_5_L_0),
			dvfsrc_read(DVFSRC_RECORD_5_L_1),
			dvfsrc_read(DVFSRC_RECORD_5_L_2));
	p += sprintf(p, "%-25s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_6_L_0~6_L_2",
			dvfsrc_read(DVFSRC_RECORD_6_L_0),
			dvfsrc_read(DVFSRC_RECORD_6_L_1),
			dvfsrc_read(DVFSRC_RECORD_6_L_2));
	p += sprintf(p, "%-25s: 0x%08x, 0x%08x, 0x%08x\n",
			"DVFSRC_RECORD_7_L_0~7_L_2",
			dvfsrc_read(DVFSRC_RECORD_7_L_0),
			dvfsrc_read(DVFSRC_RECORD_7_L_1),
			dvfsrc_read(DVFSRC_RECORD_7_L_2));
}

/* met profile table */
unsigned int met_vcorefs_info[INFO_MAX];
unsigned int met_vcorefs_src[SRC_MAX];

char *met_info_name[INFO_MAX] = {
	"OPP",
	"FREQ",
	"VCORE",
	"SPM_LEVEL",
};

char *met_src_name[SRC_MAX] = {
	"MD2SPM",
	"QOS_EMI_LEVEL",
	"QOS_VCORE_LEVEL",
	"CM_MGR_LEVEL",
	"TOTAL_EMI_LEVEL_1",
	"TOTAL_EMI_LEVEL_2",
	"TOTAL_EMI_RESULT",
	"QOS_BW_LEVEL1",
	"QOS_BW_LEVEL2",
	"QOS_BW_RESULT",
	"SCP_VCORE_LEVEL",
};

/* met profile function */
int vcorefs_get_num_opp(void)
{
	return VCORE_DVFS_OPP_NUM;
}
EXPORT_SYMBOL(vcorefs_get_num_opp);

int vcorefs_get_opp_info_num(void)
{
	return INFO_MAX;
}
EXPORT_SYMBOL(vcorefs_get_opp_info_num);

int vcorefs_get_src_req_num(void)
{
	return SRC_MAX;
}
EXPORT_SYMBOL(vcorefs_get_src_req_num);

char **vcorefs_get_opp_info_name(void)
{
	return met_info_name;
}
EXPORT_SYMBOL(vcorefs_get_opp_info_name);

char **vcorefs_get_src_req_name(void)
{
	return met_src_name;
}
EXPORT_SYMBOL(vcorefs_get_src_req_name);

unsigned int *vcorefs_get_opp_info(void)
{
	met_vcorefs_info[INFO_OPP_IDX] = get_cur_vcore_dvfs_opp();
	met_vcorefs_info[INFO_FREQ_IDX] = get_cur_ddr_khz();
	met_vcorefs_info[INFO_VCORE_IDX] = get_cur_vcore_uv();
	met_vcorefs_info[INFO_SPM_LEVEL_IDX] = spm_get_dvfs_level();

	return met_vcorefs_info;
}
EXPORT_SYMBOL(vcorefs_get_opp_info);

__weak void pm_qos_trace_dbg_show_request(int pm_qos_class)
{
}

static DEFINE_RATELIMIT_STATE(tracelimit, 5 * HZ, 1);

static void vcorefs_trace_qos(void)
{
	if (__ratelimit(&tracelimit)) {
		pm_qos_trace_dbg_show_request(PM_QOS_EMI_OPP);
		pm_qos_trace_dbg_show_request(PM_QOS_VCORE_OPP);
	}
}

unsigned int *vcorefs_get_src_req(void)
{
	unsigned int qos_total_bw = dvfsrc_read(DVFSRC_SW_BW_0) +
			   dvfsrc_read(DVFSRC_SW_BW_1) +
			   dvfsrc_read(DVFSRC_SW_BW_2) +
			   dvfsrc_read(DVFSRC_SW_BW_3) +
			   dvfsrc_read(DVFSRC_SW_BW_4);
	unsigned int qos0_thres = dvfsrc_read(DVFSRC_EMI_QOS0);
	unsigned int qos1_thres = dvfsrc_read(DVFSRC_EMI_QOS1);
	unsigned int sw_req = dvfsrc_read(DVFSRC_SW_REQ);

	met_vcorefs_src[SRC_QOS_EMI_LEVEL_IDX] =
		(sw_req >> EMI_SW_AP_SHIFT) & EMI_SW_AP_MASK;

	met_vcorefs_src[SRC_QOS_VCORE_LEVEL_IDX] =
		(sw_req >> VCORE_SW_AP_SHIFT) & VCORE_SW_AP_MASK;

	met_vcorefs_src[SRC_CM_MGR_LEVEL_IDX] =
		(dvfsrc_read(DVFSRC_SW_REQ2) >> EMI_SW_AP2_SHIFT) &
			EMI_SW_AP2_MASK;

	met_vcorefs_src[SRC_QOS_BW_LEVEL1_IDX] =
		(qos_total_bw >= qos0_thres) ? 1 : 0;
	met_vcorefs_src[SRC_QOS_BW_LEVEL2_IDX] =
		(qos_total_bw >= qos1_thres) ? 1 : 0;
	met_vcorefs_src[SRC_QOS_BW_RESUT_IDX] =
		qos_total_bw * 100;

	met_vcorefs_src[SRC_SCP_VCORE_LEVEL_IDX] =
	(dvfsrc_read(DVFSRC_VCORE_REQUEST) >> VCORE_SCP_GEAR_SHIFT) &
	VCORE_SCP_GEAR_MASK;

	vcorefs_trace_qos();

	return met_vcorefs_src;
}
EXPORT_SYMBOL(vcorefs_get_src_req);
