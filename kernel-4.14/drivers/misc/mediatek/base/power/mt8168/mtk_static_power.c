// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/nvmem-consumer.h>
#include <linux/of.h>
#if defined(__KERNEL__)  /* || !defined (__CTP__) */
#include <linux/export.h>
#include <linux/module.h>
#endif /* #if !defined (__CTP__) */

/* #include <asm/system.h> */
#include "mtk_spower_data.h"
#include "mtk_static_power.h"

#define DEVINFO_CPU_BIT (0)
#define DEVINFO_SOC_BIT (16)

static struct sptbl_t sptab[MT_SPOWER_MAX];

static const int devinfo_table[] = {
	3539, 492, 1038, 106,  231,  17,  46,   2179, 4,    481, 1014, 103,
	225,  17,  45,   2129, 3,    516, 1087, 111,  242,  19,  49,   2282,
	4,    504, 1063, 108,  236,  18,  47,   2230, 4,    448, 946,  96,
	210,  15,  41,   1986, 2,    438, 924,  93,   205,  14,  40,   1941,
	2,    470, 991,  101,  220,  16,  43,   2080, 3,    459, 968,  98,
	215,  16,  42,   2033, 3,    594, 1250, 129,  279,  23,  57,   2621,
	6,    580, 1221, 126,  273,  22,  56,   2561, 6,    622, 1309, 136,
	293,  24,  60,   2745, 7,    608, 1279, 132,  286,  23,  59,   2683,
	6,    541, 1139, 117,  254,  20,  51,   2390, 5,    528, 1113, 114,
	248,  19,  50,   2335, 4,    566, 1193, 123,  266,  21,  54,   2503,
	5,    553, 1166, 120,  260,  21,  53,   2446, 5,    338, 715,  70,
	157,  9,   29,   1505, 3153, 330, 699,  69,   153,  9,   28,   1470,
	3081, 354, 750,  74,   165,  10,  31,   1576, 3302, 346, 732,  72,
	161,  10,  30,   1540, 3227, 307, 652,  63,   142,  8,   26,   1371,
	2875, 300, 637,  62,   139,  7,   25,   1340, 2809, 322, 683,  67,
	149,  8,   27,   1436, 3011, 315, 667,  65,   146,  8,   26,   1404,
	2942, 408, 862,  86,   191,  13,  37,   1811, 1,    398, 842,  84,
	186,  12,  36,   1769, 1,    428, 903,  91,   200,  14,  39,   1896,
	2,    418, 882,  89,   195,  13,  38,   1853, 2,    371, 785,  78,
	173,  11,  33,   1651, 3458, 363, 767,  76,   169,  10,  32,   1613,
	3379, 389, 823,  82,   182,  12,  35,   1729, 1,    380, 804,  80,
	177,  11,  34,   1689,

};

static int interpolate(int x1, int x2, int x3, int y1, int y2)
{
	/* BUG_ON(x1==x2); */
	if (x1 == x2)
		return (y1 + y2) / 2;

	return (x3 - x1) * (y2 - y1) / (x2 - x1) + y1;
}

static int interpolate_2d(struct sptbl_t *tab,
	int v1, int v2, int t1, int t2, int voltage, int degree)
{
	int c1, c2, p1, p2, p;

	if (v1 == v2 && t1 == t2) {
		p = mA(tab, v1, t1);
		return p;
	} else if (v1 == v2) {
		c1 = mA(tab, v1, t1);
		c2 = mA(tab, v1, t2);
		p = interpolate(deg(tab, t1), deg(tab, t2), degree, c1, c2);
		return p;
	} else if (t1 == t2) {
		c1 = mA(tab, v1, t1);
		c2 = mA(tab, v2, t1);
		p = interpolate(mV(tab, v1), mV(tab, v2), voltage, c1, c2);
		return p;
	}

	c1 = mA(tab, v1, t1);
	c2 = mA(tab, v1, t2);
	p1 = interpolate(deg(tab, t1), deg(tab, t2), degree, c1, c2);

	c1 = mA(tab, v2, t1);
	c2 = mA(tab, v2, t2);
	p2 = interpolate(deg(tab, t1), deg(tab, t2), degree, c1, c2);

	p = interpolate(mV(tab, v1), mV(tab, v2), voltage, p1, p2);
	return p;
}

static void interpolate_table(struct sptbl_t *spt,
	int c1, int c2, int c3, struct sptbl_t *tab1, struct sptbl_t *tab2)
{
	int v, t;

	/* avoid divid error, if we have bad raw data table */
	if (unlikely(c1 == c2)) {
		*spt = *tab1;
		pr_debug("sptab equal to tab1:%d/%d\n", c1, c3);
		return;
	}

	pr_debug("make sptab %d, %d, %d\n", c1, c2, c3);
	for (t = 0; t < tsize(spt); t++) {
		for (v = 0; v < vsize(spt); v++) {
			int *p = &mA(spt, v, t);

			p[0] = interpolate(c1, c2, c3,
					   mA(tab1, v, t),
					   mA(tab2, v, t));

			pr_debug("%d ", p[0]);
		}
		pr_debug("\n");
	}
	pr_debug("make sptab done!\n");
}

static int sptab_lookup(struct sptbl_t *tab, int voltage, int degree)
{
	int x1, x2, y1, y2, i;
	int mamper;

	/** lookup voltage **/
	for (i = 0; i < vsize(tab); i++) {
		if (voltage <= mV(tab, i))
			break;
	}

	if (unlikely(voltage == mV(tab, i))) {
		x1 = x2 = i;
	} else if (unlikely(i == vsize(tab))) {
		x1 = vsize(tab) - 2;
		x2 = vsize(tab) - 1;
	} else if (i == 0) {
		x1 = 0;
		x2 = 1;
	} else {
		x1 = i - 1;
		x2 = i;
	}

	/** lookup degree **/
	for (i = 0; i < tsize(tab); i++) {
		if (degree <= deg(tab, i))
			break;
	}

	if (unlikely(degree == deg(tab, i))) {
		y1 = y2 = i;
	} else if (unlikely(i == tsize(tab))) {
		y1 = tsize(tab) - 2;
		y2 = tsize(tab) - 1;
	} else if (i == 0) {
		y1 = 0;
		y2 = 1;
	} else {
		y1 = i - 1;
		y2 = i;
	}

	mamper = interpolate_2d(tab, x1, x2, y1, y2, voltage, degree);

	return mamper;
}


static int mt_spower_make_table(struct sptbl_t *spt,
	struct spower_raw_t *spower_raw, int wat, int voltage, int degree)
{
	int i;
	int c1, c2, c = -1;
	struct sptbl_t tab[MAX_TABLE_SIZE], *tab1, *tab2, *tspt;

	/** FIXME, test only; please read efuse to assign. **/
	/* wat = 80; */
	/* voltage = 1150; */
	/* degree = 30; */

	WARN_ON(spower_raw->table_size < 3);

	/** structurize the raw data **/
	spower_tab_construct(&tab, spower_raw);

	/** lookup tables which the chip type locates to **/
	for (i = 0; i < spower_raw->table_size; i++) {
		c = sptab_lookup(&tab[i], voltage, degree);
		/** table order: ff, tt, ss **/
		if (wat >= c)
			break;
	}

	/** FIXME,
	 * There are only 2 tables are used to interpolate to form SPTAB.
	 * Thus, sptab takes use of the container which raw data is not used
	 *anymore.
	 **/
	if (wat == c) {
		/** just match **/
		tab1 = tab2 = &tab[i];
		/** pointer duplicate  **/
		tspt = tab1;

		pr_debug("sptab equal to tab:%d/%d\n", wat, c);

	} else if (i == spower_raw->table_size) {
/** above all **/
#if defined(EXTER_POLATION)
		tab1 = &tab[spower_raw->table_size - 2];
		tab2 = &tab[spower_raw->table_size - 1];

		/** occupy the free container**/
		tspt = &tab[spower_raw->table_size - 3];
#else  /* #if defined (EXTER_POLATION) */
		tspt = tab1 = tab2 = &tab[spower_raw->table_size - 1];
#endif /* #if defined (EXTER_POLATION) */

		pr_debug("sptab max tab:%d/%d\n", wat, c);
	} else if (i == 0) {
#if defined(EXTER_POLATION)
		/** below all **/
		tab1 = &tab[0];
		tab2 = &tab[1];

		/** occupy the free container**/
		tspt = &tab[2];
#else  /* #if defined (EXTER_POLATION) */
		tspt = tab1 = tab2 = &tab[0];
#endif /* #if defined (EXTER_POLATION) */

		pr_debug("sptab min tab:%d/%d\n", wat, c);
	} else {
		/** anyone **/
		tab1 = &tab[i - 1];
		tab2 = &tab[i];

		/** occupy the free container**/
		tspt = &tab[(i + 1) % spower_raw->table_size];

		pr_debug("sptab interpolate tab:%d/%d\n", wat, c);
	}

	/** sptab needs to interpolate 2 tables. **/
	if (tab1 != tab2) {
		c1 = sptab_lookup(tab1, voltage, degree);
		c2 = sptab_lookup(tab2, voltage, degree);

		interpolate_table(tspt, c1, c2, wat, tab1, tab2);
	}

	/** update to global data **/
	*spt = *tspt;

	return 0;
}

/* #define MT_SPOWER_UT 1 */

#if defined(MT_SPOWER_UT)
void mt_spower_ut(void)
{
	int v, t, p, i;

	for (i = 0; i < MT_SPOWER_MAX; i++) {
		struct sptbl_t *spt = &sptab[i];

		v = 950;
		t = 22;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 950;
		t = 25;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 950;
		t = 28;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 950;
		t = 82;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 950;
		t = 120;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1050;
		t = 22;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1050;
		t = 25;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1050;
		t = 28;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1050;
		t = 82;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1050;
		t = 120;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1100;
		t = 22;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1100;
		t = 25;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1100;
		t = 28;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1100;
		t = 82;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1100;
		t = 120;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 950;
		t = 80;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1000;
		t = 85;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);

		v = 1150;
		t = 105;
		p = sptab_lookup(spt, v, t);
		pr_debug("v/t/p: %d/%d/%d\n", v, t, p);
	}
}

#endif /* #if defined (MT_SPOWER_UT) */

static unsigned int raw_cpu;
static unsigned int raw_vcck;

int mt_spower_init(void)
{
	int mp0;
	int vcck;

	/* avoid side effect from multiple invocation */
	if (tab_validate(&sptab[MT_SPOWER_CA7]))
		return 0;

	mp0 = (int)raw_cpu;
	vcck = (int)raw_vcck;

	if (mp0)
		mp0 = (int)devinfo_table[mp0];
	else
		mp0 = 10; /* mA */
	vcck = (int)devinfo_table[vcck];

	pr_info("mp0 = %d\n", mp0);
	pr_info("vcck = %d\n", vcck);

	mt_spower_make_table(&sptab[MT_SPOWER_CA7], &ca53_spower_raw,
			     mp0 * 1 * 1000 / 4, /* mA -> uW, 4 -> 1core */
			     1000,
			     30);

	return 0;
}

/* late_initcall(mt_spower_init); */

/** return 0, means sptab is not yet ready. **/
int mt_spower_get_leakage(int dev, int vol, int deg)
{
	WARN_ON(!(dev < MT_SPOWER_MAX));

	if (!tab_validate(&sptab[dev]))
		return 0;

	return sptab_lookup(&sptab[dev], vol, deg);
}
EXPORT_SYMBOL(mt_spower_get_leakage);

static int spower_probe(struct platform_device *pdev)
{
	unsigned int *calibration_data, *leakage_data;
	struct nvmem_cell *cell_svs, *cell_leakage;
	char *cell_name_svs = "spower_calibration";
	char *cell_name_leakage = "spower_leakage";
	size_t len_svs, len_leakage;
	unsigned int ft_pgm;

	pr_notice("%s\n", __func__);

	cell_svs = nvmem_cell_get(&pdev->dev, cell_name_svs);
	if (IS_ERR(cell_svs)) {
		if (PTR_ERR(cell_svs) == -EPROBE_DEFER)
			pr_warn("nvmem not ready, retry.\n");
		else
			pr_err("cannot get %s from device tree\n",
			       cell_name_svs);

		return PTR_ERR(cell_svs);
	}

	calibration_data = (unsigned int *)nvmem_cell_read(cell_svs, &len_svs);
	nvmem_cell_put(cell_svs);

	if (IS_ERR(calibration_data)) {
		pr_err("get invalid address from nvmem framework\n");
		return -EFAULT;
	} else if (len_svs != sizeof(unsigned int) * 8) {
		pr_err("len = %zu, invalid length of svs calibration data\n",
		       len_svs);
		return -EINVAL;
	}

	cell_leakage = nvmem_cell_get(&pdev->dev, cell_name_leakage);
	if (IS_ERR(cell_leakage)) {
		if (PTR_ERR(cell_leakage) == -EPROBE_DEFER)
			pr_warn("nvmem not ready, retry.\n");
		else
			pr_err("cannot get %s from device tree\n",
			       cell_name_leakage);

		return PTR_ERR(cell_leakage);
	}

	leakage_data = (unsigned int *)nvmem_cell_read(cell_leakage,
						       &len_leakage);
	nvmem_cell_put(cell_leakage);

	if (IS_ERR(leakage_data)) {
		pr_err("get invalid address from nvmem framework\n");
		return -EFAULT;
	} else if (len_leakage != sizeof(unsigned int) * 1) {
		pr_err("len = %zu, invalid length of leakage data\n",
		       len_leakage);
		return -EINVAL;
	}


	ft_pgm = (calibration_data[0] & 0xf0) >> 4;

	raw_cpu = (leakage_data[0] >> DEVINFO_CPU_BIT) & 0x0ff;
	raw_vcck = (leakage_data[0] >> DEVINFO_SOC_BIT) & 0x0ff;
	pr_info("ft_pgm = %d\n", ft_pgm);
	pr_info("raw_cpu = %d\n", raw_cpu);
	pr_info("raw_vcck = %d\n", raw_vcck);

	kfree(calibration_data);
	kfree(leakage_data);

	mt_spower_init();

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id mt_spower_of_match[] = {
	{
		.compatible = "mediatek,mt8168-spower",
	},
	{},
};
#endif
static struct platform_driver spower_driver = {
	.probe = spower_probe,
	.driver = {

			.name = "mt-spower",
#ifdef CONFIG_OF
			.of_match_table = mt_spower_of_match,
#endif
		},
};

static int __init mtk_spower_init(void)
{
	int err = 0;
	struct device_node *node;

	err = platform_driver_register(&spower_driver);
	if (err) {
		pr_err("SPOWER driver callback register failed..\n");
		return err;
	}

	return 0;
}
fs_initcall(mtk_spower_init);
