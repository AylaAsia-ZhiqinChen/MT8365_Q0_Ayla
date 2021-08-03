/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __MTK_SLOA_FS_H__
#define __MTK_SLOA_FS_H__

enum clk_26m {
	CLK_26M_OFF = 0,
	FAKE_DCXO_26M,
	ULPLL_26M,
};

int sloa_suspend_26m_mode(enum clk_26m mode);
void sloa_suspend_infra_power(bool on);

#endif

