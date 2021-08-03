/* SPDX-License-Identifier: GPL-2.0 */
/*
 * MediaTek MMDVFS driver.
 *
 * Copyright (c) 2018 MediaTek Inc.
 */

#ifndef __MMDVFS_CONFIG_MT8168_H__
#define __MMDVFS_CONFIG_MT8168_H__

#include "mmdvfs_config_util.h"

/* Part I MMSVFS HW Configuration (OPP)*/
/* Define the number of mmdvfs, vcore and mm clks opps */

/* Max total MMDVFS opps of the profile support */
#define MT8168_MMDVFS_OPP_MAX 3

struct mmdvfs_profile_mask qos_apply_profiles[] = {
/* #ifdef MMDVFS_QOS_SUPPORT */
#if 1
	/* ISP for opp0 */
	{"ICFP",
		SMI_BWC_SCEN_ICFP,
		MMDVFS_FINE_STEP_OPP0},
	/* ISP for opp1 */
	{"Camera Capture",
		SMI_BWC_SCEN_CAM_CP,
		MMDVFS_FINE_STEP_OPP1},
	{"VSS",
		SMI_BWC_SCEN_VSS,
		MMDVFS_FINE_STEP_OPP1},
	/* ISP for opp2 */
	{"ICFP",
		SMI_BWC_SCEN_ICFP,
		MMDVFS_FINE_STEP_OPP2},
	{"Camera Capture",
		SMI_BWC_SCEN_CAM_CP,
		MMDVFS_FINE_STEP_OPP2},
	{"VSS",
		SMI_BWC_SCEN_VSS,
		MMDVFS_FINE_STEP_OPP2},
	/* debug entry */
	{"DEBUG",
		0,
		MMDVFS_FINE_STEP_UNREQUEST },
#else
	/* debug entry */
	{"DEBUG",
		QOS_ALL_SCENARIO,
		0 },
#endif
};

/* Part II MMDVFS Scenario's Step Confuguration */

#define MT8168_MMDVFS_SENSOR_MIN (13000000)
#define MT8168_MMDVFS_SENSOR_MID (16000000)
/* A.1 [LP4 2-ch] Scenarios of each MM DVFS Step (force kicker) */
/* OPP 0 scenarios */
#define MT8168_MMDVFS_OPP0_NUM 1
struct mmdvfs_profile mt8168_mmdvfs_opp0_profiles[MT8168_MMDVFS_OPP0_NUM] = {
	{"ICFP", SMI_BWC_SCEN_ICFP,
		{MT8168_MMDVFS_SENSOR_MIN, 0, 0, 0}, {0, 0, 0 } },
};

/* OPP 1 scenarios */
#define MT8168_MMDVFS_OPP1_NUM 2
struct mmdvfs_profile mt8168_mmdvfs_opp1_profiles[MT8168_MMDVFS_OPP1_NUM] = {
	{"Camera Capture", SMI_BWC_SCEN_CAM_CP,
		{MT8168_MMDVFS_SENSOR_MIN, 0, 0, 0}, {0, 0, 0 } },
	{"VSS", SMI_BWC_SCEN_VSS,
		{MT8168_MMDVFS_SENSOR_MIN, 0, 0, 0}, {0, 0, 0 } },
};

/* OPP 2 scenarios */
#define MT8168_MMDVFS_OPP2_NUM 3
struct mmdvfs_profile mt8168_mmdvfs_opp2_profiles[MT8168_MMDVFS_OPP2_NUM] = {
	{"ICFP", SMI_BWC_SCEN_ICFP, {0, 0, 0, 0}, {0, 0, 0 } },
	{"Camera Capture", SMI_BWC_SCEN_CAM_CP, {0, 0, 0, 0}, {0, 0, 0 } },
	{"VSS", SMI_BWC_SCEN_VSS, {0, 0, 0, 0}, {0, 0, 0 } },
};

struct mmdvfs_step_to_qos_step legacy_to_qos_step[MT8168_MMDVFS_OPP_MAX] = {
	{0, 0},
	{1, 1},
	{2, 2},
};

struct mmdvfs_step_profile mt8168_step_profile[MT8168_MMDVFS_OPP_MAX] = {
	{0, mt8168_mmdvfs_opp0_profiles, MT8168_MMDVFS_OPP0_NUM, {0} },
	{1, mt8168_mmdvfs_opp1_profiles, MT8168_MMDVFS_OPP1_NUM, {0} },
	{2, mt8168_mmdvfs_opp2_profiles, MT8168_MMDVFS_OPP2_NUM, {0} },
};
#endif /* __MMDVFS_CONFIG_MT8168_H__ */
