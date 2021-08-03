/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/interrupt.h>
#include <linux/syscore_ops.h>
#include <linux/platform_device.h>
#include <linux/completion.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/miscdevice.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_fdt.h>
#include <mt-plat/aee.h>
#endif


//#include "mtk_eem.h"
#include "mtk_picachu.h"

/*
 * Little cluster: L = 1
 */
#define NR_EEM_EFUSE_PER_VPROC		(1)

#define PICACHU_SIGNATURE		(0xA5)
#define PICACHU_PTP1_EFUSE_MASK		(0x00FFFFFF)
#define PICACHU_PTP1_BDES_START_BIT	(0)
#define PICACHU_PTP1_MDES_START_BIT	(8)
#define PICACHU_PTP1_MTDES_START_BIT	(16)
#define PICACHU_SIGNATURE_SHIFT_BIT	(24)

#define EEM_BASEADDR			(0x1100B000)
#define EEM_SIZE			(0x1000)
/* clusterL: SIGNATURE-MTDES-MDES-BDES*/
#define EEMSPARE0_OFFSET		(0xF20)
/* Vmin and pi_offset */
#define EEMSPARE1_OFFSET		(0xF24)


#undef TAG
#define TAG     "[Picachu] "

#define PICACHU_INFO2(fmt, args...)	\
	pr_info(TAG"[ERROR]"fmt, ##args)
#define PICACHU_INFO(fmt, args...)	\
	pr_info(TAG""fmt, ##args)

#define picachu_read(addr)		__raw_readl((void __iomem *)(addr))
#define picachu_write(addr, val)	mt_reg_sync_writel(val, addr)

#define PROC_FOPS_RW(name)				\
static int name ## _proc_open(struct inode *inode, struct file *file) \
{ \
	return single_open(file, name ## _proc_show, PDE_DATA(inode));	\
} \
static const struct file_operations name ## _proc_fops = { \
	.owner          = THIS_MODULE,		\
	.open           = name ## _proc_open,	\
	.read           = seq_read,		\
	.llseek         = seq_lseek,		\
	.release        = single_release,	\
	.write          = name ## _proc_write,	\
}

#define PROC_FOPS_RO(name)				\
static int name ## _proc_open(struct inode *inode,	\
	struct file *file)				\
{							\
	return single_open(file, name ## _proc_show,	\
		PDE_DATA(inode));			\
}							\
static const struct file_operations name ## _proc_fops = { \
	.owner          = THIS_MODULE,		\
	.open           = name ## _proc_open,	\
	.read           = seq_read,		\
	.llseek         = seq_lseek,		\
	.release        = single_release,	\
}

#define PROC_ENTRY(name)	{__stringify(name), &name ## _proc_fops}

#define PICACHU_PROC_ENTRY_ATTR (0664)

struct picachu_info {
	/*
	 * Bit[7:0]: BDES
	 * Bit[15:8]: MDES
	 * Bit[23:16]: MTDES
	 */
	unsigned int ptp1_efuse[NR_EEM_EFUSE_PER_VPROC];
	unsigned int vmin : 16;
	unsigned int pi_offset : 8;
};

struct picachu_proc {
	char *name;
	int vproc_id;
	unsigned int spare_reg_offset;
	umode_t mode;
};

struct pentry {
	const char *name;
	const struct file_operations *fops;
};

enum mt_picachu_vproc_id {
	MT_PICACHU_LITTLE_VPROC = 0,	/* Little */
	NR_PICACHU_VPROC,
};

enum mt_cluster_id {
	MT_CLUSTER_L = 0,

	NR_CLUSTERS
};


static struct picachu_data calibrate_data[PICACHU_MAX_NUM];

static void __iomem *picachu_log_addr;
static unsigned int picachu_log_phys_addr;
static const unsigned int dump_log_size = PAGE_SIZE;

static void __iomem *eem_base_addr;
static struct picachu_info picachu_data[NR_PICACHU_VPROC];
static struct picachu_proc picachu_proc_list[NR_PICACHU_VPROC] = {
	{
		"little",
		MT_PICACHU_LITTLE_VPROC,
		EEMSPARE0_OFFSET,
		PICACHU_PROC_ENTRY_ATTR
	},
};
static int eem_ctrl_id[NR_PICACHU_VPROC][NR_EEM_EFUSE_PER_VPROC] = {
	[MT_PICACHU_LITTLE_VPROC] = {MT_CLUSTER_L},
};

static ssize_t addr_show(
	struct device *dev,
	struct device_attribute *attr,
	char *buf);
static ssize_t addr_store(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count);
static DEVICE_ATTR(addr, 0664, addr_show, addr_store);

static ssize_t log_show(
	struct device *dev,
	struct device_attribute *attr,
	char *buf);
static DEVICE_ATTR(log, 0444, log_show, NULL);

static const struct file_operations picachu_ops = {
	.owner = THIS_MODULE
};

struct miscdevice picachu_device = {
	.minor = MISC_DYNAMIC_MINOR-1,
	.name = "picachu",
	.mode = 0664,
	.fops = &picachu_ops
};


static void dump_picachu_info(struct seq_file *m, struct picachu_info *info)
{
	unsigned int i;

	seq_printf(m, "vmin_high_pmic_step=0x%X\n", info->vmin);
	seq_printf(m, "pi_offset=0x%X\n", info->pi_offset);

	for (i = 0; i < NR_EEM_EFUSE_PER_VPROC; i++)
		seq_printf(m, "ptp1_efuse[%d]=0x%X\n", i, info->ptp1_efuse[i]);

	for (i = 0; i < NR_EEM_EFUSE_PER_VPROC; i++) {
		seq_printf(m, "[Picachu] calibrate_data[%d]: ", i);
		seq_printf(m, "valid=%d, BDES=0x%X, MDES=0x%X, MTDES=0x%X\n",
			calibrate_data[i].valid,
			calibrate_data[i].BDES,
			calibrate_data[i].MDES,
			calibrate_data[i].MTDES);
#ifdef PICACHU_DUBUG
		seq_printf(m, "[Picachu] calibrate_data[%d]: ", i);
		seq_printf(m, "vmin_voltage=0x%X, vmin_pmic_step=0x%X\n",
			calibrate_data[i].vmin_voltage,
			calibrate_data[i].vmin_pmic_step);
#endif
	}


}

static int picachu_dump_proc_show(struct seq_file *m, void *v)
{
	dump_picachu_info(m, (struct picachu_info *) m->private);

	return 0;
}

static void dump_picachu_log(struct seq_file *m, struct picachu_info *info)
{
	seq_printf(m, "vmin=0x%X\n", info->vmin);
	seq_printf(m, "pi_offset=0x%X\n", info->pi_offset);
}

PROC_FOPS_RO(picachu_dump);

static int create_procfs_entries(struct proc_dir_entry *dir,
				 struct picachu_proc *proc)
{
	int i, num;
	struct proc_dir_entry *ret;

	struct pentry entries[] = {
		PROC_ENTRY(picachu_dump),
	};

	num = ARRAY_SIZE(entries);
	for (i = 0; i < num; i++) {
		ret = proc_create_data(entries[i].name,
				proc->mode, dir,
				entries[i].fops,
				(void *)&picachu_data[proc->vproc_id]);
		if (ret == NULL) {
			PICACHU_INFO("[%s]: create /proc/picachu/%s failed\n",
					__func__, entries[i].name);
			return -ENOMEM;
		}
	}

	return 0;
}

static int create_procfs(void)
{
	struct proc_dir_entry *root, *dir;
	struct picachu_proc *proc;
	int ret;
	int i;

	root = proc_mkdir("picachu", NULL);
	if (!root) {
		PICACHU_INFO2("[%s]: mkdir /proc/picachu failed\n", __func__);
		return -ENOMEM;
	}

	for (i = 0; i < NR_PICACHU_VPROC; i++) {
		proc = &(picachu_proc_list[i]);
		dir = proc_mkdir(proc->name, root);
		if (!dir) {
			PICACHU_INFO("[%s]: mkdir /proc/picachu/%s failed\n",
					__func__, proc->name);
			return -ENOMEM;
		}

		ret = create_procfs_entries(dir, proc);
		if (ret)
			return ret;
	}

	return 0;
}

static int picachu_get_data(enum mt_picachu_vproc_id vproc_id)
{
	struct picachu_proc *proc;
	unsigned int i, val, tmp;
	struct picachu_info *p;
	void __iomem *reg;

	if (vproc_id >= NR_PICACHU_VPROC)
		return -1;

	p = &picachu_data[vproc_id];

	for (proc = picachu_proc_list; proc->name; proc++) {
		if (proc->vproc_id == vproc_id)
			break;
	}

	reg = eem_base_addr + proc->spare_reg_offset;
	for (i = 0; i < NR_EEM_EFUSE_PER_VPROC; i++) {
		val = picachu_read(reg);
		tmp = (val >> PICACHU_SIGNATURE_SHIFT_BIT) & 0xff;
		reg += 4;
		if (tmp != PICACHU_SIGNATURE) {
			memset(&calibrate_data[i], 0x0,
				sizeof(struct picachu_data));
			return -1;
		}
		p->ptp1_efuse[i] = val & PICACHU_PTP1_EFUSE_MASK;

		calibrate_data[i].valid = 1;
		calibrate_data[i].BDES =
			(val & 0x000000FF) >> PICACHU_PTP1_BDES_START_BIT;
		calibrate_data[i].MDES =
			(val & 0x0000FF00) >> PICACHU_PTP1_MDES_START_BIT;
		calibrate_data[i].MTDES =
			(val & 0x00FF0000) >> PICACHU_PTP1_MTDES_START_BIT;

	}

	for (i = 0; i < NR_EEM_EFUSE_PER_VPROC; i++) {
		val = picachu_read(reg);
		p->vmin = val & 0xFFFF;
		p->pi_offset = (val >> 16) & 0xFF;
#ifdef PICACHU_DUBUG
		calibrate_data[i].vmin_voltage = p->vmin;
		calibrate_data[i].vmin_pmic_step = p->pi_offset;
#endif
		reg += 4;
	}

	return 0;
}

static void picachu_apply_efuse_to_eem(enum mt_picachu_vproc_id id,
				       struct picachu_info *p)
{
	int i;

	for (i = 0; i < NR_EEM_EFUSE_PER_VPROC; i++) {
		if (p->ptp1_efuse[i] == 0 || eem_ctrl_id[id][i] == -1)
			continue;
		//eem_set_pi_efuse(eem_ctrl_id[id][i], p->ptp1_efuse[i]);
	}
}


static int parse_num(const char *str, unsigned int *value, int len)
{
	int ret;

	if (len <= 0)
		return -1;

	if ((len > 2) && ((str[0] == '0') &&
		((str[1] == 'x') || (str[1] == 'X')))) {
		ret = kstrtouint(str, 16, value);
	} else {
		ret = kstrtouint(str, 10, value);
	}

	if (ret != 0)
		return -1;

	return 0;
}

static ssize_t addr_show(
	struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	int len = 0;

	len = snprintf(buf, PAGE_SIZE, "picachu_log_phys_addr=0x%X\n",
		picachu_log_phys_addr);
	len += snprintf(buf+len, PAGE_SIZE-len, "picachu_log_addr=0x%p\n",
		picachu_log_addr);
	len += snprintf(buf+len, PAGE_SIZE-len, "dump_log_size=%d\n",
		dump_log_size);

	return len;
}

static ssize_t addr_store(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	int ret;
	char *endp;

	if ((count == 0) || (buf == NULL))
		return -EINVAL;

	ret = parse_num(buf, &picachu_log_phys_addr, count);
	if (ret != 0) {
		picachu_log_phys_addr = 0;
		picachu_log_addr = NULL;
		return -EINVAL;
	}

	if (picachu_log_addr == NULL) {
		picachu_log_addr = ioremap_wc(picachu_log_phys_addr,
						dump_log_size);
	} else {
		iounmap(picachu_log_addr);
		picachu_log_addr = ioremap_wc(picachu_log_phys_addr,
						dump_log_size);
	}

	return count;
}

static ssize_t log_show(
	struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	if (picachu_log_phys_addr == 0) {
		return snprintf(buf, PAGE_SIZE,
			"picachu_log_phys_addr=0x%X, dump_log_size=%d\n",
			picachu_log_phys_addr, dump_log_size);
	}

	if (picachu_log_addr == NULL) {
		picachu_log_addr = ioremap_wc(picachu_log_phys_addr,
						dump_log_size);
	}
	memcpy(buf, picachu_log_addr, PAGE_SIZE);
	return PAGE_SIZE;
}

static int __init picachu_init(void)
{
	struct picachu_info *p;
	unsigned int i;
	int ret;

	eem_base_addr = ioremap(EEM_BASEADDR, EEM_SIZE);
	if (!eem_base_addr) {
		PICACHU_INFO2("ioremap failed!\n");
		return -ENOMEM;
	}

	/* Update Picachu calibration data if the data is valid. */
	for (i = 0; i < NR_PICACHU_VPROC; i++) {
		ret = picachu_get_data(i);
		if (ret == 0)
			picachu_apply_efuse_to_eem(i, &picachu_data[i]);
	}

	create_procfs();

	ret = misc_register(&picachu_device);
	if (ret != 0) {
		PICACHU_INFO2("misc register failed, minor=%d\n",
		picachu_device.minor);
		return ret;
	}

	ret = device_create_file(picachu_device.this_device, &dev_attr_addr);
	if (ret != 0) {
		PICACHU_INFO2("can not create device file: addr\n");
		return ret;
	}

	ret = device_create_file(picachu_device.this_device, &dev_attr_log);
	if (ret != 0) {
		PICACHU_INFO2("can not create device file: log\n");
		return ret;
	}

	return 0;
}

static void __exit picachu_exit(void)
{
	if (eem_base_addr)
		iounmap(eem_base_addr);

	device_remove_file(picachu_device.this_device, &dev_attr_addr);
	device_remove_file(picachu_device.this_device, &dev_attr_log);
	misc_deregister(&picachu_device);
}


int get_picachu_calibrate_data(
	enum picachu_enum id,
	struct picachu_data *picachu_data)
{
	if (id >= PICACHU_MAX_NUM || picachu_data == NULL)
		return -1;

	picachu_data->valid = calibrate_data[id].valid;
	picachu_data->BDES = calibrate_data[id].BDES;
	picachu_data->MDES = calibrate_data[id].MDES;
	picachu_data->MTDES = calibrate_data[id].MTDES;
#ifdef PICACHU_DUBUG
	picachu_data->vmin_voltage = calibrate_data[id].vmin_voltage;
	picachu_data->vmin_pmic_step = calibrate_data[id].vmin_pmic_step;
#endif

	return 0;
}
EXPORT_SYMBOL(get_picachu_calibrate_data);

subsys_initcall(picachu_init);

MODULE_DESCRIPTION("MediaTek Picachu Driver v0.1");
MODULE_LICENSE("GPL");
