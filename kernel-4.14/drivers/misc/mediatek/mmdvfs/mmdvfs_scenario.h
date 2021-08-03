/* SPDX-License-Identifier: GPL-2.0 */
/*
 * MediaTek MMDVFS driver.
 *
 * Copyright (c) 2018 MediaTek Inc.
 */

#ifndef __MMDVFS_SCENARIO_H__
#define __MMDVFS_SCENARIO_H__
#include "mtk_smi.h"

s32 mmdvfs_config_scenario_concurrency(struct MTK_SMI_BWC_CONF *config);
void mmdvfs_scen_init(struct device *dev);
int is_mmdvfs_scen_disabled(void);
#endif /* __MMDVFS_SCENARIO_H__ */
