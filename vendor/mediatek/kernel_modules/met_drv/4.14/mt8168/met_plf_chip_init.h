/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 */

#ifndef __MET_PLF_CHIP_INIT_H__
#define __MET_PLF_CHIP_INIT_H__

#ifdef MET_EMI
#include <mtk_dramc.h>
extern void *mt_chn_emi_base_get(void);

/* New APIs for mt_dramc_nao base get */
extern void *mt_dramc_nao_chn_base_get(int channel);
extern void *mt_ddrphy_chn_base_get(int channel);
extern void *mt_dramc_chn_base_get(int channel);

extern void *(*mt_chn_emi_base_get_symbol)(int chn);

/* New APIs for mt_dramc_nao base get */
extern void *(*mt_dramc_nao_chn_base_get_symbol)(int channel);
extern void *(*mt_ddrphy_chn_base_get_symbol)(int channel);
extern void *(*mt_dramc_chn_base_get_symbol)(int channel);
extern struct metdevice met_emi;
#endif

#endif /*__PLF_INIT_H__*/
