/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __MTK_STATIC_POWER_MTK8168_H__
#define __MTK_STATIC_POWER_MTK8168_H__

/* #define SPOWER_NOT_READY 1 *//* for bring up, remove for MP */

/* #define WITHOUT_LKG_EFUSE */

/* mv */
#define V_OF_FUSE_CPU 1025
#define V_OF_FUSE_VCORE 800
#define V_OF_FUSE_VSRAM_CCI 1050
#define V_OF_FUSE_VSRAM_OTHERS 900
#define T_OF_FUSE 30

/* devinfo offset for each bank */
#define DEVINFO_IDX_LL 174 /* 0CD4 */
#define DEVINFO_IDX_CCI 175 /* 0CD8 NEED FIXED NO VALUE ON FILE */
#define DEVINFO_IDX_VCORE 174 /* 0CD4 */
#define DEVINFO_IDX_VSRAM_CCI 174 /* 0CD4 */
#define DEVINFO_IDX_VSRAM_OTHERS 174 /* 0CD4 */

#define DEVINFO_OFF_LL 0
#define DEVINFO_OFF_CCI 0
#define DEVINFO_OFF_VCORE 16
#define DEVINFO_OFF_VSRAM_CCI 8
#define DEVINFO_OFF_VSRAM_OTHERS 24

/* default leakage value for each bank */
#define DEF_CPULL_LEAKAGE 34
#define DEF_CCI_LEAKAGE 3
#define DEF_VCORE_LEAKAGE 9
#define DEF_VSRAM_CCI_LEAKAGE 3
#define DEF_VSRAM_OTHERS_LEAKAGE 2


enum {
#if 0
	MTK_SPOWER_CPUL,
	MTK_SPOWER_CPUL_CLUSTER,
#endif
	MTK_SPOWER_CPULL,
	MTK_SPOWER_CPULL_CLUSTER,
	MTK_SPOWER_CCI,
	MTK_SPOWER_VCORE,
	MTK_SPOWER_VSRAM_CCI,
	MTK_SPOWER_VSRAM_OTHERS,
	MTK_SPOWER_MAX
};

enum {
#if 0
	MTK_L_LEAKAGE,
#endif
	MTK_LL_LEAKAGE,
	MTK_CCI_LEAKAGE,
	MTK_VCORE_LEAKAGE,
	MTK_VSRAM_CCI_LEAKAGE,
	MTK_VSRAM_OTHERS_LEAKAGE,
	MTK_LEAKAGE_MAX
};

/* record leakage information that read from efuse */
struct spower_leakage_info {
	const char *name;
	unsigned int devinfo_idx;
	unsigned int devinfo_offset;
	unsigned int value;
	unsigned int v_of_fuse;
	int t_of_fuse;
};

extern struct spower_leakage_info spower_lkg_info[MTK_SPOWER_MAX];

/* efuse mapping */
#define L_DEVINFO_DOMAIN \
	(BIT(MTK_SPOWER_CPUL) | BIT(MTK_SPOWER_CPUL_CLUSTER))
#define LL_DEVINFO_DOMAIN \
	(BIT(MTK_SPOWER_CPULL) | BIT(MTK_SPOWER_CPULL_CLUSTER))
#define CCI_DEVINFO_DOMAIN (BIT(MTK_SPOWER_CCI))
#define GPU_DEVINFO_DOMAIN (BIT(MTK_SPOWER_GPU))
#define VCORE_DEVINFO_DOMAIN (BIT(MTK_SPOWER_VCORE))
#define VMD_DEVINFO_DOMAIN (BIT(MTK_SPOWER_VMD))
#define MODEM_DEVINFO_DOMAIN (BIT(MTK_SPOWER_MODEM))
#define VSRAM_CCI_DEVINFO_DOMAIN (BIT(MTK_SPOWER_VSRAM_CCI))
#define VSRAM_GPU_DEVINFO_DOMAIN (BIT(MTK_SPOWER_VSRAM_GPU))
#define VSRAM_OTHERS_DEVINFO_DOMAIN (BIT(MTK_SPOWER_VSRAM_OTHERS))

/* used to calculate total leakage that search from raw table */
#define BIG_CORE_INSTANCE 2
#define DEFAULT_CORE_INSTANCE 4
#define DEFAULT_INSTANCE 1

extern char *spower_name[];
extern char *leakage_name[];
extern int default_leakage[];
extern int devinfo_idx[];
extern int devinfo_offset[];
extern int devinfo_table[];

#endif
