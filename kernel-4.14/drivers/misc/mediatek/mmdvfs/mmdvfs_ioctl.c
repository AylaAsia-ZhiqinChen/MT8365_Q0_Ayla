// SPDX-License-Identifier: GPL-2.0
/*
 * MediaTek MMDVFS driver.
 *
 * Copyright (c) 2018 MediaTek Inc.
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "mmdvfs_mgr.h"
#include "mtk_smi.h"
#include "mmdvfs_pmqos.h"
#include "mmdvfs_scenario.h"

#define DEV_NAME "mtk_mmdvfs"


static s32 mmdvfs_scen_open(struct inode *inode, struct file *file)
{
	file->private_data = kcalloc(SMI_BWC_SCEN_CNT, sizeof(u32), GFP_ATOMIC);
	if (!file->private_data)
		return -ENOMEM;

	return 0;
}

static s32 mmdvfs_scen_release(struct inode *inode, struct file *file)
{
	kfree(file->private_data);
	file->private_data = NULL;
	return 0;
}


static long mmdvfs_handle_ioctl(struct file *file,
		unsigned int cmd, unsigned long arg)
{
	s32 ret = 0;

	if (!file->f_op || !file->f_op->unlocked_ioctl)
		return -ENOTTY;

	switch (cmd) {
	case MTK_IOC_SMI_BWC_CONF:
	{
#ifdef ENABLE_SCENARIO
		struct MTK_SMI_BWC_CONF conf;

		ret = copy_from_user(&conf, (void *)arg, sizeof(conf));
		if (ret)
			pr_notice("CMD%u copy from user failed:%d\n", cmd, ret);
		else
			ret = mmdvfs_config_scenario_concurrency(&conf);
#endif
		break;
	}
	case MTK_IOC_MMDVFS_CMD:
	{
#ifdef ENABLE_SCENARIO
		struct MTK_MMDVFS_CMD conf;

		if (SMI_PARAM_DISABLE_MMDVFS)
			return -EACCES;

		ret = copy_from_user(&conf, (void *)arg,
			sizeof(struct MTK_MMDVFS_CMD));
		if (ret)
			pr_notice("cmd %u copy_from_user fail: %d\n", cmd, ret);
		else {
			mmdvfs_handle_cmd(&conf);
			ret = copy_to_user((void *)arg, (void *)&conf,
				sizeof(struct MTK_MMDVFS_CMD));
			if (ret)
				pr_notice("cmd %u copy_to_user failed: %d\n",
					cmd, ret);
		}
#endif
		break;
	}
	case MTK_IOC_MMDVFS_QOS_CMD:
	{
		struct MTK_MMDVFS_QOS_CMD conf;

		ret = copy_from_user(&conf, (void *)arg,
			sizeof(struct MTK_MMDVFS_QOS_CMD));
		if (ret)
			pr_notice("cmd %u copy_from_user fail: %d\n", cmd, ret);
		else {
			switch (conf.type) {
			case MTK_MMDVFS_QOS_CMD_TYPE_SET:
#if IS_ENABLED(CONFIG_MTK_SMI_EXT)
				mmdvfs_set_max_camera_hrt_bw(conf.max_cam_bw);
#endif
				conf.ret = 0;
				break;
			default:
				pr_notice("invalid mmdvfs QOS cmd\n");
				return -EINVAL;
			}
		}
		break;
	}
	default:
		ret = -ENOIOCTLCMD;
		break;
	}
	return ret;
}

static const struct file_operations mmdvfs_scen_file_opers = {
	.owner = THIS_MODULE,
	.open = mmdvfs_scen_open,
	.release = mmdvfs_scen_release,
	.unlocked_ioctl = mmdvfs_handle_ioctl,
};

s32 mmdvfs_ioctl_register(void)
{
	dev_t			dev_no;
	struct cdev		*cdev;
	struct class		*class;
	struct device		*device;

	/* device */
	dev_no = MKDEV(MTK_MMDVFS_MAJOR_NUMBER, 0);
	if (alloc_chrdev_region(&dev_no, 0, 1, DEV_NAME))
		pr_notice("Allocate chrdev region failed\n");

	cdev = cdev_alloc();
	if (!cdev)
		pr_notice("Allocate cdev failed\n");
	else {
		cdev_init(cdev, &mmdvfs_scen_file_opers);
		cdev->owner = THIS_MODULE;
		cdev->dev = dev_no;
		if (cdev_add(cdev, dev_no, 1))
			pr_notice("Add cdev failed\n");
	}

	class = class_create(THIS_MODULE, DEV_NAME);
	if (IS_ERR(class))
		pr_notice("Create class failed: %ld\n", PTR_ERR(class));
	device = device_create(class, NULL, dev_no, NULL, DEV_NAME);
	if (IS_ERR(device))
		pr_notice("Create device failed: %ld\n", PTR_ERR(device));

	return 0;
}
EXPORT_SYMBOL_GPL(mmdvfs_ioctl_register);
