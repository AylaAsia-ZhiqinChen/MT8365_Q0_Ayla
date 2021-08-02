// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/kallsyms.h>
#include "met_drv.h"
#include "met_api_tbl.h"
#include "interface.h"
#include "met_plf_chip_init.h"

#ifdef MET_EMI
void *(*mt_chn_emi_base_get_symbol)(int chn);
void *(*mt_dramc_nao_chn_base_get_symbol)(int channel);
void *(*mt_ddrphy_chn_base_get_symbol)(int channel);
void *(*mt_dramc_chn_base_get_symbol)(int channel);
#endif /* MET_EMI */

int met_symbol_extget(void) 
{
#define _MET_SYMBOL_GET(_func_name_) \
	do { \
		_func_name_##_symbol = (void *)symbol_get(_func_name_); \
		if (_func_name_##_symbol == NULL) { \
			pr_debug("MET ext. symbol : %s is not found!\n", #_func_name_); \
			PR_BOOTMSG_ONCE("MET ext. symbol : %s is not found!\n", #_func_name_); \
		} \
	} while (0)
	return 0;
}
int met_symbol_extput(void)
{
#define _MET_SYMBOL_PUT(_func_name_) { \
		if (_func_name_##_symbol) { \
			symbol_put(_func_name_); \
			_func_name_##_symbol = NULL; \
		} \
	}
	return 0;
}
static int met_chip_symbol_get(void)
{
#define _MET_SYMBOL_GET(_func_name_) \
	do { \
		_func_name_##_symbol = (void *)symbol_get(_func_name_); \
		if (_func_name_##_symbol == NULL) { \
			pr_debug("MET ext. symbol : %s is not found!\n", #_func_name_); \
			PR_BOOTMSG_ONCE("MET ext. symbol : %s is not found!\n", #_func_name_); \
		} \
	} while (0)
#ifdef MET_EMI
	_MET_SYMBOL_GET(mt_chn_emi_base_get);
	_MET_SYMBOL_GET(mt_dramc_nao_chn_base_get);
	_MET_SYMBOL_GET(mt_ddrphy_chn_base_get);
	_MET_SYMBOL_GET(mt_dramc_chn_base_get);
#endif

	return 0;
}
static int met_chip_symbol_put(void)
{
#define _MET_SYMBOL_PUT(_func_name_) { \
		if (_func_name_##_symbol) { \
			symbol_put(_func_name_); \
			_func_name_##_symbol = NULL; \
		} \
	}
#ifdef MET_EMI
	_MET_SYMBOL_PUT(mt_chn_emi_base_get);
	_MET_SYMBOL_PUT(mt_dramc_nao_chn_base_get);
	_MET_SYMBOL_PUT(mt_ddrphy_chn_base_get);
	_MET_SYMBOL_PUT(mt_dramc_chn_base_get);
#endif
	return 0;
}
int met_plf_chip_init(void)
{
	/*initial met chip external symbol*/
	met_chip_symbol_get();
#ifdef MET_EMI
	met_register(&met_emi);
#endif
	return 0;
}
void met_plf_chip_exit(void)
{
	/*release met chip external symbol*/
	met_chip_symbol_put();
#ifdef MET_EMI
	met_deregister(&met_emi);
#endif
}
