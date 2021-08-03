/*
 * mt8168-adsp-pcm.c  --  MT8168 Adsp Pcm driver
 *
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Bo Pan <bo.pan@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <linux/pm_runtime.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/sysfs.h>
#include <linux/errno.h>
#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#endif
#if defined(CONFIG_PM_WAKELOCKS)
#include <linux/pm_wakeup.h>
#endif
#include <sound/memalloc.h>
#include <linux/miscdevice.h>
#include "mach/mtk_hifixdsp_common.h"

#include "mt8168-adsp-utils.h"
#include "mt8168-afe-common.h"
#include "mt8168-afe-utils.h"
#include "mt8168-reg.h"
#include "../common/mtk-base-afe.h"
#include "mtk_sloa_fs.h"
#if defined(CONFIG_MTK_QOS_SUPPORT)
#include <linux/pm_qos.h>
#include <helio-dvfsrc-opp.h>
#endif
#include <pmic_api.h>
#include <linux/mfd/mt6358/rtc_misc.h>
#include <upmu_common.h>

#define CONFIG_ADSP_MISC_DEVICE

struct mt8168_adsp_pcm_format {
	unsigned int channels;
	unsigned int rate;
	unsigned int format;
	unsigned int period_size;
	unsigned int period_count;
};

struct adsp_dma_ring_buf {
	unsigned char *start_addr;
	uint32_t size_bytes;
	uint32_t hw_offset_bytes;
	uint32_t appl_offset_bytes;
};

struct cpu_dma_ring_buf {
	unsigned char *dma_buf_vaddr;
	size_t dma_buf_size;
	size_t dma_offset;
	size_t dma_period_size_bytes;
};

struct mt8168_adsp_dai_memory {
	struct snd_pcm_substream *substream;
	struct adsp_dma_ring_buf adsp_dma;
	struct cpu_dma_ring_buf  cpu_dma;
	struct io_ipc_ring_buf_shared *adsp_dma_control;
	uint32_t adsp_dma_control_paddr;
	struct mt8168_adsp_pcm_format fe_format;
};

struct mt8168_adsp_be_dai_data {
	struct snd_pcm_substream *substream;
	int mem_type;
};

struct mt8168_adsp_pcm_priv {
	struct device *dev;
	bool dsp_boot_run;
	bool dsp_ready;
	bool dsp_loading;
	bool dsp_suspend;
	struct mt8168_adsp_dai_memory dai_mem[MT8168_ADSP_FE_CNT];
	struct mt8168_adsp_be_dai_data be_data[MT8168_ADSP_BE_CNT];
	struct mtk_base_afe *afe;
	int fake26m_mode;
	struct mt8168_adsp_pcm_format hostless_data;
	/* for va hostless mixer control */
	unsigned int va_hostless_enbale;
	struct snd_pcm_substream *va_hostless_substream;
#if defined(CONFIG_MTK_QOS_SUPPORT)
	int max_pll;
	struct pm_qos_request pm_adsp;
#endif
#ifdef CONFIG_DEBUG_FS
	void *dbg_data;
#endif
#ifdef CONFIG_ADSP_MISC_DEVICE
	void *misc_data;
#endif
#if defined(CONFIG_PM_WAKELOCKS)
	struct wakeup_source *va_suspend_lock;
#endif
};

#ifdef CONFIG_ADSP_MISC_DEVICE
struct adsp_model_mgr {
	unsigned char *start_addr;
	uint32_t size_bytes;
};

struct adsp_ioctl_model_data {
	long addr_from_user;
	long size_from_user;
};

#ifdef CONFIG_COMPAT
struct compat_adsp_ioctl_model_data {
	compat_size_t addr_from_user;
	compat_size_t size_from_user;
};
#endif

#define MT8168_ADSP_IOC_MAGIC 'C'
#define ADSP_IOCTL_DYNAMIC_LOAD_MODEL _IOWR(MT8168_ADSP_IOC_MAGIC, 0x00, \
					    unsigned int)
#endif

static struct mt8168_adsp_pcm_priv *g_priv;

#define IS_ADSP_READY() (g_priv->dsp_ready)
#define FAKE26M_DCXO		0
#define ULPLL26M		1
#define ENUM_TO_STR(enum) #enum

static void load_hifi4dsp_callback(void *arg);

#ifdef CONFIG_DEBUG_FS

#define DEFAULT_DUMP_FILE_SIZE			(1*1024*1024)
#define DEFAULT_DUMP_BUFFER_SIZE		(64*1024)
#define DEFAULT_DUMP_WAIT_TIMEOUT_SEC		(30)
#define DEFAULT_DUMP_CHANNEL_NUMBER		(2)
#define DEFAULT_DUMP_BITWIDTH			(16)
#define DEFAULT_DUMP_PERIOD_COUNT		(8)
#define DEFAULT_DUMP_PERIOD_SIZE		(256)

struct host_dma_ring_buf {
	unsigned char *area;
	size_t dma_buf_size;
	size_t host_hw_off;
	size_t host_appl_off;
	size_t period_size_bytes;
};

struct mt8168_adsp_debugfs_attr {
	char *fs_name;
	const struct file_operations *fops;
	int dump_postion;
};
struct mt8168_adsp_debugfs_data {
	struct device *device;
	struct dentry *root;
	u64 size;
	int dump_position;
	u32 buffer_size;
	u32 timeout;
	u32 chnum;
	u32 bitwidth;
	u32 period_size;
	u32 period_count;
	bool enable;
	struct snd_dma_buffer host_dmab;

	struct work_struct dump_work;
	wait_queue_head_t wait_data;

	struct adsp_dma_ring_buf adsp_dma;
	struct host_dma_ring_buf  cpu_dma;
	struct io_ipc_ring_buf_shared *adsp_dma_control;
};

static uint32_t dump_data_from_dsp(struct mt8168_adsp_debugfs_data *dump)
{
	struct adsp_dma_ring_buf *adsp_dma = &dump->adsp_dma;
	struct host_dma_ring_buf *cpu_dma = &dump->cpu_dma;
	struct io_ipc_ring_buf_shared *adsp_dma_control =
				dump->adsp_dma_control;

	unsigned char *adsp_dma_buf_vaddr = adsp_dma->start_addr;
	uint32_t adsp_dma_buf_size = adsp_dma->size_bytes;
	uint32_t adsp_dma_hw_off = 0;
	uint32_t adsp_dma_appl_off = adsp_dma->appl_offset_bytes;

	unsigned char *cpu_dma_buf_vaddr = cpu_dma->area;
	uint32_t cpu_dma_buf_size = cpu_dma->dma_buf_size;
	uint32_t cpu_dma_hw_off = cpu_dma->host_hw_off;
	uint32_t cpu_dma_appl_off = cpu_dma->host_appl_off;
	uint32_t period_size_bytes = cpu_dma->period_size_bytes;
	uint32_t avail_bytes;
	uint32_t cpu_dma_free_bytes;
	uint32_t copy_bytes;
	uint32_t copied = 0;

	adsp_dma_hw_off = adsp_dma_control->ptr_to_hw_offset_bytes;

	if (adsp_dma_hw_off >= adsp_dma_appl_off)
		avail_bytes = adsp_dma_hw_off - adsp_dma_appl_off;
	else
		avail_bytes =
			adsp_dma_buf_size - adsp_dma_appl_off + adsp_dma_hw_off;

	if (cpu_dma_hw_off >= cpu_dma_appl_off)
		cpu_dma_free_bytes =
			cpu_dma_appl_off + cpu_dma_buf_size - cpu_dma_hw_off;
	else
		cpu_dma_free_bytes = cpu_dma_appl_off - cpu_dma_hw_off;

	if (avail_bytes >= cpu_dma_free_bytes)
		avail_bytes = cpu_dma_free_bytes - period_size_bytes;

	if (avail_bytes < period_size_bytes)
		return 0;

	copy_bytes = (avail_bytes / period_size_bytes) * period_size_bytes;

	while (copy_bytes > 0) {
		uint32_t from_bytes = 0;

		if (adsp_dma_hw_off >= adsp_dma_appl_off)
			from_bytes = adsp_dma_hw_off - adsp_dma_appl_off;
		else
			from_bytes = adsp_dma_buf_size - adsp_dma_appl_off;

		if (from_bytes > copy_bytes)
			from_bytes = copy_bytes;

		while (from_bytes > 0) {
			uint32_t to_bytes = 0;

			if (cpu_dma_hw_off + from_bytes < cpu_dma_buf_size)
				to_bytes = from_bytes;
			else
				to_bytes = cpu_dma_buf_size - cpu_dma_hw_off;

			memcpy(cpu_dma_buf_vaddr + cpu_dma_hw_off,
				adsp_dma_buf_vaddr + adsp_dma_appl_off,
				to_bytes);

			from_bytes -= to_bytes;
			copy_bytes -= to_bytes;
			copied += to_bytes;

			cpu_dma_hw_off = (cpu_dma_hw_off + to_bytes) %
				cpu_dma_buf_size;
			adsp_dma_appl_off = (adsp_dma_appl_off + to_bytes) %
				adsp_dma_buf_size;
		}
	}

	adsp_dma_control->ptr_to_appl_offset_bytes = adsp_dma_appl_off;
	adsp_dma->appl_offset_bytes = adsp_dma_appl_off;
	adsp_dma->hw_offset_bytes = adsp_dma_hw_off;
	cpu_dma->host_hw_off = cpu_dma_hw_off;
	return copied;
}

static void dump_process(struct work_struct *work)
{
	struct mt8168_adsp_debugfs_data *dump = container_of(work,
		struct mt8168_adsp_debugfs_data, dump_work);

	if (dump_data_from_dsp(dump) > 0)
		wake_up_interruptible(&dump->wait_data);
}

static ssize_t mt8168_adsp_dump_read_from_dsp(char __user *buf,
	size_t size,
	struct mt8168_adsp_debugfs_data *dump)
{
	struct host_dma_ring_buf *cpu_dma = &dump->cpu_dma;

	unsigned char *host_dma_buf_vaddr = cpu_dma->area;
	uint32_t host_dma_buf_size = cpu_dma->dma_buf_size;
	uint32_t host_dma_hw_off = cpu_dma->host_hw_off;
	uint32_t host_appl_off = cpu_dma->host_appl_off;
	uint32_t timeout_ms = dump->timeout * MSEC_PER_SEC;
	ssize_t read = 0;
	size_t copy = size;
	long ret;

	while (copy > 0) {
		uint32_t from = 0;

		host_dma_hw_off = cpu_dma->host_hw_off;
		host_appl_off = cpu_dma->host_appl_off;

		if (host_dma_hw_off >= host_appl_off)
			from = host_dma_hw_off - host_appl_off;
		else
			from = host_dma_buf_size - host_appl_off;

		if (from > copy)
			from = copy;

		if (from > 0) {
#ifdef MT8168_ADSP_DUMP_DMA_DEBUG
			pr_info("%s [%u] copy %u bytes from %p to %p\n",
				__func__, dump->scene, from,
				host_dma_buf_vaddr + host_appl_off,
				buf + read);
#endif
			if (copy_to_user_fromio(buf + read,
						host_dma_buf_vaddr +
						host_appl_off, from)) {
				pr_info("%s copy error\n", __func__);
				goto endofdump;
			}

			copy -= from;
			read += from;
			host_appl_off += from;
			host_appl_off %= host_dma_buf_size;

			cpu_dma->host_appl_off = host_appl_off;

			continue;
		}

		ret = wait_event_interruptible_timeout(dump->wait_data,
			(cpu_dma->host_hw_off != cpu_dma->host_appl_off),
			msecs_to_jiffies(timeout_ms));
		if (ret == 0 || ret == -ERESTARTSYS) {
			pr_info("%s timeout or nonblocked pending\n", __func__);
			break;
		}

	}

endofdump:
	return read;

}

static void do_adsp_dump_release(struct mt8168_adsp_debugfs_data *dump)
{
	cancel_work_sync(&dump->dump_work);

	if (!IS_ADSP_READY())
		return;

	if (dump->enable) {
		struct host_debug_param param;
		param.dump_position = dump->dump_position;

		mt8168_adsp_send_ipi_cmd(NULL,
			TASK_SCENE_AUDIO_CONTROLLER,
			AUDIO_IPI_LAYER_TO_DSP,
			AUDIO_IPI_PAYLOAD,
			AUDIO_IPI_MSG_NEED_ACK,
			MSG_TO_DSP_DEBUG_STOP,
			sizeof(param),
			0,
			(char *)&param);
	}

	if (dump->host_dmab.bytes > 0) {
		snd_dma_free_pages(&dump->host_dmab);
		memset(&dump->host_dmab, 0, sizeof(dump->host_dmab));
	}

	dump->enable = false;
}

static int mt8168_adsp_dump_open(struct inode *inode,
	struct file *file)
{
	struct mt8168_adsp_debugfs_data *dump = inode->i_private;
	struct ipi_msg_t ipi_msg;
	struct host_debug_param param;
	struct host_debug_param ack_param;
	struct io_ipc_ring_buf_shared *shared_buf;
	struct adsp_dma_ring_buf *adsp_dma = &dump->adsp_dma;
	struct host_dma_ring_buf *cpu_dma = &dump->cpu_dma;
	phys_addr_t paddr;
	void __iomem *vaddr;
	int ret;

	if (!IS_ADSP_READY())
		return -ENODEV;

	ret = simple_open(inode, file);
	if (ret)
		return ret;

	memset(&param, 0, sizeof(param));
	memset(&ipi_msg, 0, sizeof(ipi_msg));
	param.dump_position = dump->dump_position;
	param.request_bytes = dump->size;
	param.chnum = dump->chnum;
	param.bitwidth = dump->bitwidth;
	param.period_cnt = dump->period_count;
	param.period_size = dump->period_size;
	param.buffer_size = dump->buffer_size;

	ret = mt8168_adsp_send_ipi_cmd(&ipi_msg,
		TASK_SCENE_AUDIO_CONTROLLER,
		AUDIO_IPI_LAYER_TO_DSP,
		AUDIO_IPI_PAYLOAD,
		AUDIO_IPI_MSG_NEED_ACK,
		MSG_TO_DSP_DEBUG_START,
		sizeof(param),
		0,
		(char *)&param);
	if (ret)
		return ret;

	memcpy((void *)(&ack_param), (void *)(&ipi_msg.payload),
			sizeof(struct host_debug_param));
	if (ack_param.inited == 0) {
		dev_info(dump->device, "%s, dsp dump not inited.\n", __func__);
		return -EPERM;
	}

	ret = snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV,
							dump->device,
							dump->buffer_size,
							&dump->host_dmab);
	if (ret) {
		dev_info(dump->device, "%s, dump alloc fail\n", __func__);
		return ret;
	}

	/* get adsp dma control block between adsp pcm driver and adsp */
	paddr = ack_param.shared_base;
	paddr = adsp_hal_phys_addr_dsp2cpu(paddr);
	vaddr = adsp_get_shared_sysram_phys2virt(paddr);
	shared_buf = (struct io_ipc_ring_buf_shared *)vaddr;

	dump->adsp_dma_control = shared_buf;
	/* config dma between adsp pcm driver and adsp */
	paddr = shared_buf->start_addr;
	paddr = adsp_hal_phys_addr_dsp2cpu(paddr);
	vaddr = adsp_get_shared_sysram_phys2virt(paddr);
	adsp_dma->start_addr = (unsigned char *)vaddr;

	adsp_dma->size_bytes = shared_buf->size_bytes;
	adsp_dma->hw_offset_bytes = shared_buf->ptr_to_hw_offset_bytes;
	adsp_dma->appl_offset_bytes = shared_buf->ptr_to_appl_offset_bytes;

	/* config dma between adsp pcm driver  and user space */
	cpu_dma->area = dump->host_dmab.area;
	cpu_dma->dma_buf_size = dump->host_dmab.bytes;
	cpu_dma->period_size_bytes = dump->buffer_size / dump->period_count;
	cpu_dma->host_appl_off = cpu_dma->host_hw_off = 0;

	dump->enable = true;

	return 0;
}

static int mt8168_adsp_dump_release(struct inode *inode,
	struct file *file)
{
	struct mt8168_adsp_debugfs_data *dump = file->private_data;

	do_adsp_dump_release(dump);

	return 0;
}

static ssize_t mt8168_adsp_dump_read(struct file *file,
	char __user *buf, size_t len, loff_t *ppos)
{
	struct mt8168_adsp_debugfs_data *dump = file->private_data;
	ssize_t read = 0;
	ssize_t to_read = 0;

	if (*ppos < 0 || !len)
		return -EINVAL;

	if (dump->size > 0)
		to_read = (dump->size - *ppos);

	if (to_read > len) {
		to_read = len;
	} else if (to_read <= 0) {
		to_read = 0;
		return to_read;
	}

	read = mt8168_adsp_dump_read_from_dsp(buf + read,
		to_read, dump);
	if (read > 0)
		*ppos += read;

	return read;
}

static const struct file_operations mt8168_adsp_dump_fops = {
	.open = mt8168_adsp_dump_open,
	.read = mt8168_adsp_dump_read,
	.llseek = noop_llseek,
	.release = mt8168_adsp_dump_release,
};

static const struct mt8168_adsp_debugfs_attr adsp_debugfs_attrs[] = {
	{"adsp_dump1", &mt8168_adsp_dump_fops, DSP_DUMP1},
	{"adsp_dump2", &mt8168_adsp_dump_fops, DSP_DUMP2},
	{"adsp_dump3", &mt8168_adsp_dump_fops, DSP_DUMP3},
	{"adsp_dump4", &mt8168_adsp_dump_fops, DSP_DUMP4},
};
#endif //CONFIG_DEBUG_FS

#ifdef CONFIG_ADSP_MISC_DEVICE
struct mt8168_adsp_misc_data {
	struct device *device;
	struct adsp_model_mgr model_mgr;
};

static int write_model_to_dsp(struct file *fp, unsigned char *arg)
{
	struct mt8168_adsp_misc_data *misc = fp->private_data;
	struct adsp_model_mgr *model_mgr = &misc->model_mgr;
	unsigned char *adsp_dma_buf_vaddr = model_mgr->start_addr;
	uint32_t adsp_dma_buf_size = model_mgr->size_bytes;
	int ret;

	pr_info("%s file:%p, arg:%p\n", __func__, fp, arg);
	pr_info("%s adsp_dma_buf_vaddr:%p\n", __func__, adsp_dma_buf_vaddr);
	pr_info("%s adsp_dma_buf_size:%d\n", __func__, adsp_dma_buf_size);

	if (copy_from_user((void *)adsp_dma_buf_vaddr,
			   (const void __user *)arg, adsp_dma_buf_size)) {
		pr_info("%s copy_from_user ERROR!\n", __func__);
		return -EFAULT;
	}

	ret = mt8168_adsp_send_ipi_cmd(NULL,
			TASK_SCENE_VA,
			AUDIO_IPI_LAYER_TO_DSP,
			AUDIO_IPI_MSG_ONLY,
			AUDIO_IPI_MSG_NEED_ACK,
			MSG_TO_DSP_LOAD_MODEL_WRITE,
			0, 0, NULL);
	if (ret)
		return ret;

	return 0;
}

static int release_model_to_dsp(struct file *fp)
{
	int ret = 0;

	ret = mt8168_adsp_send_ipi_cmd(NULL,
			TASK_SCENE_VA,
			AUDIO_IPI_LAYER_TO_DSP,
			AUDIO_IPI_MSG_ONLY,
			AUDIO_IPI_MSG_NEED_ACK,
			MSG_TO_DSP_LOAD_MODEL_CLOSE,
			0, 0, NULL);
	if (ret)
		return ret;

	return 0;
}

static int prepare_model_to_dsp(struct file *fp, uint32_t size)
{
	struct mt8168_adsp_misc_data *misc = fp->private_data;
	struct ipi_msg_t ipi_msg;
	struct host_load_model_init_param param;
	struct host_load_model_init_param ack_param;
	struct adsp_model_mgr *model_mgr = &misc->model_mgr;
	phys_addr_t paddr;
	void __iomem *vaddr;
	int ret;

	memset(&param, 0, sizeof(param));
	memset(&ipi_msg, 0, sizeof(ipi_msg));
	param.model_size = size;

	ret = mt8168_adsp_send_ipi_cmd(&ipi_msg,
		TASK_SCENE_VA,
		AUDIO_IPI_LAYER_TO_DSP,
		AUDIO_IPI_PAYLOAD,
		AUDIO_IPI_MSG_NEED_ACK,
		MSG_TO_DSP_LOAD_MODEL_OPEN,
		sizeof(param),
		0,
		(char *)&param);
	if (ret)
		return ret;

	memcpy((void *)(&ack_param), (void *)(&ipi_msg.payload),
	       sizeof(struct host_load_model_init_param));
	/* check share buffer size allocated if enough */
	if (ack_param.inited == 0) {
		dev_info(misc->device, "%s, dsp model not inited.\n", __func__);
		return -ENOMEM;
	}

	/* config dma between adsp pcm driver and adsp */
	paddr = ack_param.shared_base;
	paddr = adsp_hal_phys_addr_dsp2cpu(paddr);
	vaddr = adsp_get_shared_sysram_phys2virt(paddr);
	model_mgr->start_addr = (unsigned char *)vaddr;
	model_mgr->size_bytes = ack_param.model_size;

	return 0;

}

static int load_model_to_dsp(struct file *fp, unsigned long arg)
{
	struct adsp_ioctl_model_data model_data;
	int ret;

	pr_info("%s: fp = %p arg = %ld\n", __func__, fp, arg);

	if (copy_from_user((void *)(&model_data), (const void __user *)(arg),
			   sizeof(struct adsp_ioctl_model_data)))
		return -EFAULT;

	ret = prepare_model_to_dsp(fp, (uint32_t)model_data.size_from_user);
	if (ret) {
		pr_info("prepare_model_to_dsp fail! ret:%d, model size:%d\n",
			ret, (uint32_t)model_data.size_from_user);
		return ret;
	}
	ret = write_model_to_dsp(fp,
				 (unsigned char *)model_data.addr_from_user);
	if (ret) {
		pr_info("write_model_to_dsp fail! ret:%d, model addr:%p\n", ret,
			(unsigned char *)model_data.addr_from_user);
		return ret;
	}
	ret = release_model_to_dsp(fp);
	if (ret) {
		pr_info("release_model_to_dsp fail! ret:%d\n", ret);
		return ret;
	}

	return ret;
}

static int mt8168_adsp_misc_open(struct inode *inode, struct file *fp)
{
	struct mt8168_adsp_misc_data *misc = g_priv->misc_data;

	pr_info("%s inode:%p, file:%p\n", __func__, inode, fp);

	if (!IS_ADSP_READY())
		return -ENODEV;

	fp->private_data = misc;

	return 0;
}

static int mt8168_adsp_misc_release(struct inode *inode, struct file *fp)
{
	pr_info("%s inode:%p, file:%p\n", __func__, inode, fp);

	if (!(fp->f_mode & FMODE_WRITE || fp->f_mode & FMODE_READ))
		return -ENODEV;

	if (!IS_ADSP_READY())
		return -ENODEV;

	return 0;
}

static long mt8168_adsp_misc_ioctl(struct file *fp, unsigned int cmd,
				   unsigned long arg)
{
	int ret = 0;

	pr_info("%s: command  cmd = %x arg = %ld\n", __func__, cmd, arg);

	if (!IS_ADSP_READY())
		return -ENODEV;

	switch (cmd) {
	case ADSP_IOCTL_DYNAMIC_LOAD_MODEL:
		ret = load_model_to_dsp(fp, arg);
		break;
	default:
		break;
	}

	return ret;
}

#ifdef CONFIG_COMPAT
static int compat_get_adsp_ioctl_model_data(
	struct compat_adsp_ioctl_model_data __user *data32,
	struct adsp_ioctl_model_data __user *data)
{
	int err = 0;
	compat_size_t l;

	err = get_user(l, &data32->addr_from_user);
	err |= put_user(l, &data->addr_from_user);
	err |= get_user(l, &data32->size_from_user);
	err |= put_user(l, &data->size_from_user);

	return err;
}
static long compat_mt8168_adsp_misc_ioctl(struct file *fp, unsigned int cmd,
					  unsigned long arg)
{
	int ret = 0;

	pr_info("%s: command  cmd = 0%x arg = %ld\n", __func__, cmd, arg);

	if (!IS_ADSP_READY())
		return -ENODEV;

	if (!fp->f_op || !fp->f_op->unlocked_ioctl)
		return -ENOTTY;

	switch (cmd) {
	case ADSP_IOCTL_DYNAMIC_LOAD_MODEL: {
		struct compat_adsp_ioctl_model_data __user *data32;
		struct adsp_ioctl_model_data __user *data;

		int err = 0;

		data32 = compat_ptr(arg);
		data = compat_alloc_user_space(sizeof(*data));
		if (data == NULL)
			return -EFAULT;

		err = compat_get_adsp_ioctl_model_data(data32, data);
		if (err) {
			pr_info("compat_get_adsp_ioctl_model_data error!!!\n");
			return err;
		}
		ret = fp->f_op->unlocked_ioctl(fp, cmd, (unsigned long)data);
	}
		break;
	default:
		break;
	}

	return ret;
}
#endif

static const struct file_operations mt8168_adsp_misc_fops = {
	.owner   = THIS_MODULE,
	.open    = mt8168_adsp_misc_open,
	.release = mt8168_adsp_misc_release,
	.unlocked_ioctl = mt8168_adsp_misc_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = compat_mt8168_adsp_misc_ioctl,
#endif
};

static struct miscdevice mt8168_adsp_model_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "adsp_misc",
	.fops = &mt8168_adsp_misc_fops,
};

static void mt8168_adsp_init_load_model(struct mt8168_adsp_pcm_priv *priv)
{
	int ret;
	size_t data_size;
	struct mt8168_adsp_misc_data *misc_data;

	data_size = sizeof(struct mt8168_adsp_misc_data);
	priv->misc_data = devm_kzalloc(priv->dev, data_size, GFP_KERNEL);
	if (!priv->misc_data)
		return;

	misc_data = (struct mt8168_adsp_misc_data *)priv->misc_data;
	misc_data->device = priv->dev;

	/* register MISC device */
	ret = misc_register(&mt8168_adsp_model_device);
	if (ret)
		dev_info(priv->dev, "%s, misc_register Fail:%d\n",
			 __func__, ret);
}
#endif

static void mt8168_adsp_init_debug_dump(struct mt8168_adsp_pcm_priv *priv)
{
#ifdef CONFIG_DEBUG_FS
	int i;
	size_t debugfs_count = ARRAY_SIZE(adsp_debugfs_attrs);
	size_t data_size = sizeof(struct mt8168_adsp_debugfs_data) *
			   debugfs_count;
	struct mt8168_adsp_debugfs_data *dump_data_base;
	struct mt8168_adsp_debugfs_data *dump_data;
	const struct mt8168_adsp_debugfs_attr *attr;

	priv->dbg_data = devm_kzalloc(priv->dev, data_size, GFP_KERNEL);
	if (!priv->dbg_data)
		return;

	dump_data_base = (struct mt8168_adsp_debugfs_data *)priv->dbg_data;

	for (i = 0; i < debugfs_count; i++) {
		dump_data = dump_data_base + i;
		attr = &adsp_debugfs_attrs[i];

		dump_data->device = priv->dev;
		dump_data->size = DEFAULT_DUMP_FILE_SIZE;
		dump_data->timeout = DEFAULT_DUMP_WAIT_TIMEOUT_SEC;
		dump_data->chnum = DEFAULT_DUMP_CHANNEL_NUMBER;
		dump_data->bitwidth = DEFAULT_DUMP_BITWIDTH;
		dump_data->period_size = DEFAULT_DUMP_PERIOD_SIZE;
		dump_data->period_count = DEFAULT_DUMP_PERIOD_COUNT;
		dump_data->dump_position = attr->dump_postion;
		dump_data->enable = false;
		dump_data->buffer_size = dump_data->chnum
					 * dump_data->bitwidth
					 * dump_data->period_count
					 * dump_data->period_size
					 /8;

		INIT_WORK(&dump_data->dump_work, dump_process);
		init_waitqueue_head(&dump_data->wait_data);

		dump_data->root = debugfs_create_dir(attr->fs_name, NULL);
		if (dump_data->root == NULL)
			dev_info(priv->dev,
				"%s,dump dir create fail\n", __func__);

		if (IS_ERR_OR_NULL(dump_data->root))
			continue;

		debugfs_create_u64("size",
			0644,
			dump_data->root,
			&dump_data->size);

		debugfs_create_u32("buffer_size",
			0444,
			dump_data->root,
			&dump_data->buffer_size);

		debugfs_create_u32("timeout",
			0644,
			dump_data->root,
			&dump_data->timeout);

		debugfs_create_u32("channel",
			0644,
			dump_data->root,
			&dump_data->chnum);

		debugfs_create_u32("bit",
			0644,
			dump_data->root,
			&dump_data->bitwidth);

		debugfs_create_u32("period_size",
			0644,
			dump_data->root,
			&dump_data->period_size);

		debugfs_create_u32("period_count",
			0644,
			dump_data->root,
			&dump_data->period_count);

		debugfs_create_file_size("data",
			0444,
			dump_data->root,
			dump_data,
			attr->fops,
			0);

		debugfs_create_bool("enable",
			0444,
			dump_data->root,
			&dump_data->enable);
	}

#endif
}

static void mt8168_adsp_cleanup_debug_dump(struct mt8168_adsp_pcm_priv *priv)
{
#ifdef CONFIG_DEBUG_FS
	size_t i;
	size_t debugfs_count = ARRAY_SIZE(adsp_debugfs_attrs);
	struct mt8168_adsp_debugfs_data *dump_data_base;
	struct mt8168_adsp_debugfs_data *dump_data;

	dump_data_base = (struct mt8168_adsp_debugfs_data *)priv->dbg_data;
	if (!dump_data_base)
		return;

	for (i = 0; i < debugfs_count; i++) {
		dump_data = dump_data_base + i;
		debugfs_remove_recursive(dump_data->root);
		dump_data->root = NULL;
	}

#endif
}

static void mt8168_adsp_handle_debug_dump_irq(struct ipi_msg_t *p_ipi_msg)
{
#ifdef CONFIG_DEBUG_FS
	struct dsp_debug_irq_param param;
	size_t i;
	size_t debugfs_count = ARRAY_SIZE(adsp_debugfs_attrs);
	struct mt8168_adsp_debugfs_data *dump_data_base;
	struct mt8168_adsp_debugfs_data *dump_data;

	memcpy((void *)(&param), (void *)(p_ipi_msg->payload),
					sizeof(struct dsp_debug_irq_param));

	dump_data_base = (struct mt8168_adsp_debugfs_data *)g_priv->dbg_data;
	if (!dump_data_base)
		return;
	for (i = 0; i < debugfs_count; i++) {
		dump_data = dump_data_base + i;
		if (dump_data
		    && (dump_data->dump_position == param.dump_position))
			break;
	}

	if (!dump_data || (i == debugfs_count) || (dump_data->enable == 0)) {
		pr_info("%s dump invalid\n", __func__);
		return;
	}

	queue_work(system_unbound_wq, &dump_data->dump_work);

#endif
}

static void mt8168_reset_dai_memory(struct mt8168_adsp_dai_memory *dai_mem)
{
	memset(dai_mem, 0, sizeof(struct mt8168_adsp_dai_memory));
}

static void mt8168_reset_dai_dma_offset(struct mt8168_adsp_dai_memory *dai_mem)
{
	dai_mem->adsp_dma.hw_offset_bytes = 0;
	dai_mem->adsp_dma.appl_offset_bytes = 0;
	dai_mem->cpu_dma.dma_offset = 0;
}

static const struct snd_pcm_hardware mt8168_adsp_pcm_pcm_hardware = {
	.info = SNDRV_PCM_INFO_MMAP |
		SNDRV_PCM_INFO_MMAP_VALID |
		SNDRV_PCM_INFO_INTERLEAVED,
	.buffer_bytes_max = 512 * 1024,
	.period_bytes_min = 320,
	.period_bytes_max = 256 * 1024,
	.periods_min = 2,
	.periods_max = 256,
};

static int mt8168_adsp_get_afe_memif_id(int dai_id)
{
	int memif_id = 0;

	switch (dai_id) {
	case MT8168_ADSP_BE_VUL2_IN:
		memif_id = MT8168_AFE_MEMIF_VUL2;
		break;
	case MT8168_ADSP_BE_TDM_IN:
		memif_id = MT8168_AFE_MEMIF_TDM_IN;
		break;
	case MT8168_ADSP_BE_VUL_IN:
		memif_id = MT8168_AFE_MEMIF_VUL;
		break;
	default:
		memif_id = -EINVAL;
		break;
	}
	return memif_id;
}

static int mt8168_adsp_pcm_fe_startup(struct snd_pcm_substream *substream,
				       struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(rtd->platform);
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	int ret = 0;

	if (!IS_ADSP_READY())
		return -ENODEV;

	snd_soc_set_runtime_hwparams(substream,
				     &mt8168_adsp_pcm_pcm_hardware);

	if (scene < 0)
		return -EINVAL;

	ret = mt8168_adsp_send_ipi_cmd(NULL,
				       scene,
				       AUDIO_IPI_LAYER_TO_DSP,
				       AUDIO_IPI_MSG_ONLY,
				       AUDIO_IPI_MSG_NEED_ACK,
				       MSG_TO_DSP_HOST_PORT_STARTUP,
				       0, 0, NULL);

	priv->dai_mem[id].substream = substream;

	return ret;
}

static void mt8168_adsp_pcm_fe_shutdown(struct snd_pcm_substream *substream,
					 struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(rtd->platform);
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);

	if (!IS_ADSP_READY())
		return;

	if (scene < 0)
		return;

	mt8168_adsp_send_ipi_cmd(NULL,
				 scene,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_MSG_ONLY,
				 AUDIO_IPI_MSG_NEED_ACK,
				 MSG_TO_DSP_HOST_CLOSE,
				 0, 0, NULL);

	mt8168_reset_dai_memory(&priv->dai_mem[id]);
}

static int mt8168_adsp_pcm_fe_hw_params(struct snd_pcm_substream *substream,
					 struct snd_pcm_hw_params *params,
					 struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(rtd->platform);
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	struct host_ipc_msg_hw_param ipc_hw_param;
	struct dsp_ipc_msg_hw_param ack_hw_param;
	struct ipi_msg_t ipi_msg;
	struct mt8168_adsp_dai_memory *dai_mem = &priv->dai_mem[id];
	struct adsp_dma_ring_buf *adsp_dma = &dai_mem->adsp_dma;
	struct cpu_dma_ring_buf *cpu_dma = &dai_mem->cpu_dma;
	phys_addr_t paddr;
	void __iomem *vaddr;
	int ret = 0;

	if (!IS_ADSP_READY())
		return -ENODEV;

	ret = snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(params));
	if (ret < 0)
		return ret;

	if (scene < 0)
		return -EINVAL;

	memset(&ipi_msg, 0, sizeof(ipi_msg));
	/* TODO config share buffer size by DTS */
	ipc_hw_param.dai_id = mt8168_adsp_dai_id_pack(id);
	ipc_hw_param.sample_rate = params_rate(params);
	ipc_hw_param.channel_num = params_channels(params);
	ipc_hw_param.bitwidth = params_width(params);
	ipc_hw_param.period_size = params_period_size(params);
	ipc_hw_param.period_count = params_periods(params);

	ret = mt8168_adsp_send_ipi_cmd(&ipi_msg,
				       scene,
				       AUDIO_IPI_LAYER_TO_DSP,
				       AUDIO_IPI_PAYLOAD,
				       AUDIO_IPI_MSG_NEED_ACK,
				       MSG_TO_DSP_HOST_HW_PARAMS,
				       sizeof(ipc_hw_param),
				       0,
				       (char *)&ipc_hw_param);
	if (ret != 0)
		return ret;

	if (ipi_msg.ack_type != AUDIO_IPI_MSG_ACK_BACK) {
		dev_dbg(priv->dev,
			"unexpected ack type %u\n",
			ipi_msg.ack_type);
		return -EINVAL;
	}

	AUDIO_IPC_COPY_DSP_HW_PARAM(ipi_msg.payload, &ack_hw_param);

	/* NOTICE: now only support DRAM share buffer */
	if (ack_hw_param.adsp_dma.mem_type != AFE_MEM_TYPE_DRAM)
		return -ENOMEM;

	/* get adsp dma control block between adsp pcm driver and adsp */
	paddr = ack_hw_param.adsp_dma.dma_paddr;
	paddr =
		adsp_hal_phys_addr_dsp2cpu(paddr);
	vaddr =
		adsp_get_shared_sysram_phys2virt(paddr);
	dai_mem->adsp_dma_control_paddr = (uint32_t)paddr;
	dai_mem->adsp_dma_control =
	    (struct io_ipc_ring_buf_shared *)vaddr;

	/* config dma between adsp pcm driver and adsp */
	paddr = dai_mem->adsp_dma_control->start_addr;
	paddr =
		adsp_hal_phys_addr_dsp2cpu(paddr);
	vaddr =
		adsp_get_shared_sysram_phys2virt(paddr);
	adsp_dma->start_addr = (unsigned char *)vaddr;
	adsp_dma->size_bytes =
		dai_mem->adsp_dma_control->size_bytes;
	adsp_dma->hw_offset_bytes =
		dai_mem->adsp_dma_control->ptr_to_hw_offset_bytes;
	adsp_dma->appl_offset_bytes =
		dai_mem->adsp_dma_control->ptr_to_appl_offset_bytes;

	/* config dma between adsp pcm driver  and user space */
	cpu_dma->dma_buf_vaddr = substream->runtime->dma_area;
	cpu_dma->dma_buf_size = substream->runtime->dma_bytes;
	cpu_dma->dma_period_size_bytes =
		params_period_bytes(params);

	dai_mem->fe_format.channels = ack_hw_param.channel_num;
	dai_mem->fe_format.rate = ack_hw_param.sample_rate;
	dai_mem->fe_format.format = ack_hw_param.bitwidth;
	dai_mem->fe_format.period_size = ack_hw_param.period_size;
	dai_mem->fe_format.period_count = ack_hw_param.period_count;
	return 0;
}

static int mt8168_adsp_pcm_fe_hw_free(struct snd_pcm_substream *substream,
				       struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	struct host_ipc_msg_hw_free ipc_hw_free;

	if (!IS_ADSP_READY())
		return -ENODEV;

	if (scene < 0)
		return -EINVAL;

	ipc_hw_free.dai_id = mt8168_adsp_dai_id_pack(id);
	mt8168_adsp_send_ipi_cmd(NULL,
			       scene,
			       AUDIO_IPI_LAYER_TO_DSP,
			       AUDIO_IPI_PAYLOAD,
			       AUDIO_IPI_MSG_NEED_ACK,
			       MSG_TO_DSP_HOST_HW_FREE,
			       sizeof(ipc_hw_free),
			       0,
			       (char *)&ipc_hw_free);

	return snd_pcm_lib_free_pages(substream);
}

static int mt8168_adsp_pcm_fe_prepare(struct snd_pcm_substream *substream,
				       struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	int ret = 0;

	if (!IS_ADSP_READY())
		return -ENODEV;

	if (scene < 0)
		return -EINVAL;


	ret = mt8168_adsp_send_ipi_cmd(NULL,
				       scene,
				       AUDIO_IPI_LAYER_TO_DSP,
				       AUDIO_IPI_MSG_ONLY,
				       AUDIO_IPI_MSG_NEED_ACK,
				       MSG_TO_DSP_HOST_PREPARE,
				       0, 0, NULL);

	return ret;
}

static int mt8168_adsp_pcm_fe_trigger(struct snd_pcm_substream *substream,
				       int cmd,
				       struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(rtd->platform);
	int id = rtd->cpu_dai->id;
	struct mt8168_adsp_dai_memory *dai_mem = &priv->dai_mem[id];
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	struct host_ipc_msg_trigger ipc_trigger;
	int ret = 0;

	if (!IS_ADSP_READY())
		return -ENODEV;

	if (scene < 0)
		return -EINVAL;

	ipc_trigger.dai_id = mt8168_adsp_dai_id_pack(id);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
		mt8168_adsp_send_ipi_cmd(NULL,
					 scene,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_DIRECT_SEND,
					 MSG_TO_DSP_HOST_TRIGGER_START,
					 sizeof(ipc_trigger),
					 0,
					 (char *)&ipc_trigger);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		mt8168_adsp_send_ipi_cmd(NULL,
					 scene,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_DIRECT_SEND,
					 MSG_TO_DSP_HOST_TRIGGER_STOP,
					 sizeof(ipc_trigger),
					 0,
					 (char *)&ipc_trigger);
		mt8168_reset_dai_dma_offset(dai_mem);
		break;
	default:
		break;
	}

	return ret;
}

static int mt8168_adsp_pcm_be_startup(struct snd_pcm_substream *substream,
				       struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(rtd->platform);
	struct mtk_base_afe *afe = priv->afe;
	int ret = 0;

	if (!IS_ADSP_READY())
		return -ENODEV;

	if (scene < 0)
		return -EINVAL;

	ret = mt8168_adsp_send_ipi_cmd(NULL,
				       scene,
				       AUDIO_IPI_LAYER_TO_DSP,
				       AUDIO_IPI_MSG_ONLY,
				       AUDIO_IPI_MSG_NEED_ACK,
				       MSG_TO_DSP_DSP_PORT_STARTUP,
				       0, 0, NULL);
	if (ret == 0)
		pm_runtime_get_sync(afe->dev);

	return ret;
}

static void mt8168_adsp_pcm_be_shutdown(struct snd_pcm_substream *substream,
					 struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(rtd->platform);
	struct mtk_base_afe *afe = priv->afe;

	if (!IS_ADSP_READY())
		return;

	if (scene < 0)
		return;

	mt8168_adsp_send_ipi_cmd(NULL,
				 scene,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_MSG_ONLY,
				 AUDIO_IPI_MSG_NEED_ACK,
				 MSG_TO_DSP_DSP_CLOSE,
				 0, 0, NULL);

	pm_runtime_put_sync(afe->dev);
}

static int mt8168_adsp_pcm_be_hw_params(struct snd_pcm_substream *substream,
					 struct snd_pcm_hw_params *params,
					 struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	int memif_id = mt8168_adsp_get_afe_memif_id(id);
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(rtd->platform);
	struct mt8168_adsp_be_dai_data *adsp_be =
		&priv->be_data[id - MT8168_ADSP_BE_START];
	struct mtk_base_afe *afe = priv->afe;
	struct mt8168_afe_private *afe_priv = afe->platform_priv;
	struct mt8168_adsp_data *afe_adsp = &(afe_priv->adsp_data);
	struct mtk_base_afe_memif *memif = &afe->memif[memif_id];
	struct host_ipc_msg_hw_param ipc_hw_param;
	struct dsp_ipc_msg_hw_param ack_hw_param;
	struct ipi_msg_t ipi_msg;
	int ret = 0;

	if (!IS_ADSP_READY())
		return -ENODEV;

	if (scene < 0)
		return -EINVAL;

	memset(&ipi_msg, 0, sizeof(ipi_msg));

	ipc_hw_param.dai_id = id - MT8168_ADSP_BE_START;
	ipc_hw_param.sample_rate = params_rate(params);
	ipc_hw_param.channel_num = params_channels(params);
	ipc_hw_param.bitwidth = params_width(params);
	ipc_hw_param.period_size = params_period_size(params);
	ipc_hw_param.period_count = params_periods(params);

	/* Config AFE DMA buffer */
	memif->buffer_size = params_buffer_bytes(params);
	ipc_hw_param.adsp_dma.dma_paddr = 0;
	if (adsp_be->mem_type == AFE_MEM_TYPE_AFE_SRAM) {
		unsigned int paddr, size;

		afe_adsp->get_afe_memif_sram(afe, memif_id, &paddr, &size);
		if (paddr != 0 && memif->buffer_size < size) {
			ipc_hw_param.adsp_dma.dma_paddr = paddr;
			ipc_hw_param.adsp_dma.mem_type = AFE_MEM_TYPE_AFE_SRAM;
		} else {
			ipc_hw_param.adsp_dma.mem_type = AFE_MEM_TYPE_DRAM;
		}
	} else {
		ipc_hw_param.adsp_dma.mem_type = (uint32_t)adsp_be->mem_type;
	}

	ret = mt8168_adsp_send_ipi_cmd(&ipi_msg,
				       scene,
				       AUDIO_IPI_LAYER_TO_DSP,
				       AUDIO_IPI_PAYLOAD,
				       AUDIO_IPI_MSG_NEED_ACK,
				       MSG_TO_DSP_DSP_HW_PARAMS,
				       sizeof(ipc_hw_param),
				       0,
				       (char *)&ipc_hw_param);

	if (ret != 0)
		return ret;

	if (ipi_msg.ack_type != AUDIO_IPI_MSG_ACK_BACK) {
		dev_dbg(priv->dev,
			"unexpected ack type %u\n",
			ipi_msg.ack_type);
		return -EINVAL;
	}
	AUDIO_IPC_COPY_DSP_HW_PARAM(ipi_msg.payload, &ack_hw_param);
	if (ack_hw_param.adsp_dma.dma_paddr == 0)
		return -ENOMEM;

	/* Notice: for AFE & DTCM do not need address convert */
	if (ack_hw_param.adsp_dma.mem_type == AFE_MEM_TYPE_DRAM)
		memif->phys_buf_addr =
		    adsp_hal_phys_addr_dsp2cpu(ack_hw_param.adsp_dma.dma_paddr);
	else
		memif->phys_buf_addr = ack_hw_param.adsp_dma.dma_paddr;

	ret = afe_adsp->set_afe_memif(afe,
				       memif_id,
				       params_rate(params),
				       params_channels(params),
				       params_format(params));
	if (ret < 0)
		return ret;

	return ret;
}

static int mt8168_adsp_pcm_be_hw_free(struct snd_pcm_substream *substream,
				       struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);

	if (!IS_ADSP_READY())
		return -ENODEV;

	if (scene < 0)
		return -EINVAL;

	mt8168_adsp_send_ipi_cmd(NULL,
				 scene,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_MSG_ONLY,
				 AUDIO_IPI_MSG_NEED_ACK,
				 MSG_TO_DSP_DSP_HW_FREE,
				 0, 0, NULL);

	return 0;
}

static int mt8168_adsp_pcm_be_prepare(struct snd_pcm_substream *substream,
				       struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	int ret = 0;

	if (!IS_ADSP_READY())
		return -ENODEV;

	if (scene < 0)
		return -EINVAL;

	ret = mt8168_adsp_send_ipi_cmd(NULL,
				 scene,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_MSG_ONLY,
				 AUDIO_IPI_MSG_NEED_ACK,
				 MSG_TO_DSP_DSP_PREPARE,
				 0, 0, NULL);

	return ret;
}

static int mt8168_adsp_pcm_be_trigger(struct snd_pcm_substream *substream,
				       int cmd,
				       struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_pcm_runtime * const runtime = substream->runtime;
	int id = rtd->cpu_dai->id;
	int scene = mt8168_adsp_get_scene_by_dai_id(id);
	int memif_id = mt8168_adsp_get_afe_memif_id(id);
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(rtd->platform);
	struct mtk_base_afe *afe = priv->afe;
	struct mt8168_afe_private *afe_priv = afe->platform_priv;
	struct mt8168_adsp_data *afe_adsp = &(afe_priv->adsp_data);
	struct host_ipc_msg_trigger ipc_trigger;
	int ret = 0;

	if (!IS_ADSP_READY())
		return -ENODEV;

	if (scene < 0)
		return 0;

	ipc_trigger.dai_id = id - MT8168_ADSP_BE_START;
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
		afe_adsp->set_afe_memif_enable(afe,
					       memif_id,
					       runtime->rate,
					       runtime->period_size,
					       1);
		mt8168_adsp_send_ipi_cmd(NULL,
					 scene,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_DIRECT_SEND,
					 MSG_TO_DSP_DSP_TRIGGER_START,
					 sizeof(ipc_trigger),
					 0,
					 (char *)&ipc_trigger);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		afe_adsp->set_afe_memif_enable(afe,
					       memif_id,
					       runtime->rate,
					       runtime->period_size,
					       0);
		mt8168_adsp_send_ipi_cmd(NULL,
					 scene,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_DIRECT_SEND,
					 MSG_TO_DSP_DSP_TRIGGER_STOP,
					 sizeof(ipc_trigger),
					 0,
					 (char *)&ipc_trigger);
		break;
	default:
		break;
	}

	return ret;
}

static int mt8168_adsp_hostless_active(void)
{
	struct snd_pcm_substream *substream;
	struct snd_soc_pcm_runtime *rtd;
	struct snd_soc_dai *dai;

	substream = g_priv->dai_mem[MT8168_ADSP_FE_HOSTLESS_VA].substream;
	if (substream == NULL)
		return 0;
	rtd = substream->private_data;
	if (rtd == NULL)
		return 0;
	dai = rtd->cpu_dai;

	if (dai != NULL)
		return dai->active;
	else
		return 0;
}

static int mt8168_adsp_dai_suspend(struct snd_soc_dai *dai)
{
	struct mt8168_adsp_pcm_priv *priv = dev_get_drvdata(dai->dev);

	/* if dsp is not ready or on, ignore suspend */
	if (!IS_ADSP_READY())
		return 0;

	if (priv->dsp_suspend)
		return 0;

	/* Notify ADSP suspend enter */
	mt8168_adsp_send_ipi_cmd(NULL,
				 TASK_SCENE_AUDIO_CONTROLLER,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_MSG_ONLY,
				 AUDIO_IPI_MSG_NEED_ACK,
				 MSG_TO_DSP_AP_SUSPEND_T,
				 0, 0, NULL);

	/* If there is no hostless active */
	/* Whatever other DAIs is active or not, do nothing now */
	/* DSP may in WFI mode, or clock off? */

	priv->dsp_suspend = true;

	return 0;
}

static int mt8168_adsp_dai_resume(struct snd_soc_dai *dai)
{
	struct mt8168_adsp_pcm_priv *priv = dev_get_drvdata(dai->dev);

	/* if dsp is not ready or on, ignore resume */
	if (!IS_ADSP_READY())
		return 0;

	if (!priv->dsp_suspend)
		return 0;

	/* Notify ADSP suspend exit */
	mt8168_adsp_send_ipi_cmd(NULL,
				 TASK_SCENE_AUDIO_CONTROLLER,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_MSG_ONLY,
				 AUDIO_IPI_MSG_NEED_ACK,
				 MSG_TO_DSP_AP_RESUME_T,
				 0, 0, NULL);
	/* If there is no hostless active */
	/* Whatever other DAIs is active or not, do nothing now */
	/* DSP may in WFI mode */

	priv->dsp_suspend = false;
	return 0;
}

/* FE DAIs */
static const struct snd_soc_dai_ops mt8168_adsp_pcm_fe_dai_ops = {
	.startup	= mt8168_adsp_pcm_fe_startup,
	.shutdown	= mt8168_adsp_pcm_fe_shutdown,
	.hw_params	= mt8168_adsp_pcm_fe_hw_params,
	.hw_free	= mt8168_adsp_pcm_fe_hw_free,
	.prepare	= mt8168_adsp_pcm_fe_prepare,
	.trigger	= mt8168_adsp_pcm_fe_trigger,
};

/* BE DAIs */
static const struct snd_soc_dai_ops mt8168_adsp_pcm_be_dai_ops = {
	.startup	= mt8168_adsp_pcm_be_startup,
	.shutdown	= mt8168_adsp_pcm_be_shutdown,
	.hw_params	= mt8168_adsp_pcm_be_hw_params,
	.hw_free	= mt8168_adsp_pcm_be_hw_free,
	.prepare	= mt8168_adsp_pcm_be_prepare,
	.trigger	= mt8168_adsp_pcm_be_trigger,
};

static struct snd_soc_dai_driver mt8168_adsp_pcm_dais[] = {
	/* FE DAIs */
	{
		.name = "FE_HOSTLESS_VA",
		.id = MT8168_ADSP_FE_HOSTLESS_VA,
		.suspend = mt8168_adsp_dai_suspend,
		.resume = mt8168_adsp_dai_resume,
		.capture = {
			.stream_name = "FE_HOSTLESS_VA",
			.channels_min = 1,
			.channels_max = 8,
			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S32_LE,
		},
		.ops = &mt8168_adsp_pcm_fe_dai_ops,
	}, {
		.name = "FE_VA",
		.id = MT8168_ADSP_FE_VA,
		.suspend = mt8168_adsp_dai_suspend,
		.resume = mt8168_adsp_dai_resume,
		.capture = {
			.stream_name = "FE_VA",
			.channels_min = 1,
			.channels_max = 8,
			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S32_LE,
		},
		.ops = &mt8168_adsp_pcm_fe_dai_ops,
	}, {
		.name = "FE_MICR",
		.id = MT8168_ADSP_FE_MIC_RECORD,
		.suspend = mt8168_adsp_dai_suspend,
		.resume = mt8168_adsp_dai_resume,
		.capture = {
			.stream_name = "FE_MICR",
			.channels_min = 1,
			.channels_max = 8,
			.rates = SNDRV_PCM_RATE_8000_48000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S32_LE,
		},
		.ops = &mt8168_adsp_pcm_fe_dai_ops,
	}, {
	/* BE DAIs */
		.name = "BE_VUL2_IN",
		.id = MT8168_ADSP_BE_VUL2_IN,
		.capture = {
			.stream_name = "BE_VUL2_IN Capture",
			.channels_min = 2,
			.channels_max = 16,
			.rates = SNDRV_PCM_RATE_8000_192000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S32_LE,
		},
		.ops = &mt8168_adsp_pcm_be_dai_ops,
	}, {
		.name = "BE_TDM_IN",
		.id = MT8168_ADSP_BE_TDM_IN,
		.capture = {
			.stream_name = "BE_TDM_IN Capture",
			.channels_min = 1,
			.channels_max = 16,
			.rates = SNDRV_PCM_RATE_8000_192000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S32_LE,
		},
		.ops = &mt8168_adsp_pcm_be_dai_ops,
	}, {
		.name = "BE_VUL_IN",
		.id = MT8168_ADSP_BE_VUL_IN,
		.capture = {
			.stream_name = "BE_VUL_IN Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = SNDRV_PCM_RATE_8000_192000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE |
				   SNDRV_PCM_FMTBIT_S32_LE,
		},
		.ops = &mt8168_adsp_pcm_be_dai_ops,
	},
};

static const char * const vain_text[] = {
	"OPEN", "VUL2_IN", "TDM_IN", "VUL_IN"
};

static const char * const vain_io_text[] = {
	"OPEN", "VUL2_IN_IO", "TDM_IN_IO", "VUL_IN_IO"
};

static SOC_ENUM_SINGLE_VIRT_DECL(vain_enum, vain_text);
static SOC_ENUM_SINGLE_VIRT_DECL(vain_io_enum, vain_io_text);

static const struct snd_kcontrol_new vain_mux =
	    SOC_DAPM_ENUM("VA_IN Source", vain_enum);

static const struct snd_kcontrol_new vain_io_mux =
	    SOC_DAPM_ENUM("VA_IN_IO Source", vain_io_enum);

static const struct snd_kcontrol_new va_vul2_enable_ctl =
	SOC_DAPM_SINGLE_VIRT("Switch", 1);

static const struct snd_kcontrol_new va_tdmin_enable_ctl =
	SOC_DAPM_SINGLE_VIRT("Switch", 1);

static const struct snd_soc_dapm_widget mt8168_adsp_pcm_widgets[] = {
	SND_SOC_DAPM_SWITCH("VA_VUL2_IO", SND_SOC_NOPM, 0, 0,
			    &va_vul2_enable_ctl),
	SND_SOC_DAPM_SWITCH("VA_TDM_IN_IO", SND_SOC_NOPM, 0, 0,
			    &va_tdmin_enable_ctl),
	SND_SOC_DAPM_MIXER("VA_VUL_IO", SND_SOC_NOPM, 0, 0, NULL, 0),

	SND_SOC_DAPM_MUX("VA_IN Mux", SND_SOC_NOPM, 0, 0, &vain_mux),
	SND_SOC_DAPM_MUX("VA_IN_IO Mux", SND_SOC_NOPM, 0, 0, &vain_io_mux),

	SND_SOC_DAPM_INPUT("VA VUL2 In"),
	SND_SOC_DAPM_INPUT("VA TDM_IN In"),
	SND_SOC_DAPM_INPUT("VA VUL In"),
};

static const struct snd_soc_dapm_route mt8168_adsp_pcm_routes[] = {
	{"VA_VUL2_IO", "Switch", "VUL2 Input Mux"},
	{"VA_IN_IO Mux", "VUL2_IN_IO", "VA_VUL2_IO"},

	{"BE_VUL2_IN Capture", NULL, "VA VUL2 In"},
	{"VA_IN Mux", "VUL2_IN", "BE_VUL2_IN Capture"},

	{"VA_TDM_IN_IO", "Switch", "TDM_IN Input Mux"},
	{"VA_IN_IO Mux", "TDM_IN_IO", "VA_TDM_IN_IO"},

	{"BE_TDM_IN Capture", NULL, "VA TDM_IN In"},
	{"VA_IN Mux", "TDM_IN", "BE_TDM_IN Capture"},

	{"VA_VUL_IO", NULL, "O10"},
	{"VA_VUL_IO", NULL, "O09"},
	{"VA_IN_IO Mux", "VUL_IN_IO", "VA_VUL_IO"},

	{"BE_VUL_IN Capture", NULL, "VA VUL In"},
	{"VA_IN Mux", "VUL_IN", "BE_VUL_IN Capture"},

	{"FE_HOSTLESS_VA", NULL, "VA_IN_IO Mux"},
	{"FE_VA", NULL, "VA_IN_IO Mux"},
	{"FE_MICR", NULL, "VA_IN_IO Mux"},

	{"FE_HOSTLESS_VA", NULL, "VA_IN Mux"},
	{"FE_VA", NULL, "VA_IN Mux"},
	{"FE_MICR", NULL, "VA_IN Mux"},
};

static int wdt_event_notify_callback(struct notifier_block *this,
			  unsigned long code, void *unused)
{
	char *wdt_event[2];

	wdt_event[0] = "ACTION=ADSPWDT";
	wdt_event[1] = NULL;

	pr_notice("ADSP WDT notify\n");
	kobject_uevent_env(&g_priv->dev->kobj, KOBJ_CHANGE, wdt_event);

	return NOTIFY_DONE;
}

static struct notifier_block wdt_event_notifier = {
	.notifier_call  = wdt_event_notify_callback,
	.priority       = 90,
};

static int mt8168_adsp_low_power_init(struct mt8168_adsp_pcm_priv *priv)
{
	struct mtk_base_afe *afe = priv->afe;
	struct mt8168_afe_private *afe_priv = afe->platform_priv;
	struct mt8168_adsp_data *afe_adsp = &(afe_priv->adsp_data);
	struct ipc_clk_mode_params params;

	afe_adsp->set_afe_init(afe);

	/* fake 26M setting */
	if (priv->fake26m_mode == FAKE26M_DCXO) {
		sloa_suspend_26m_mode(FAKE_DCXO_26M);
	} else {
		sloa_suspend_26m_mode(ULPLL_26M);
		rtc_enable_32k1v8_1();
	}

	params.clk_mode = priv->fake26m_mode;
	mt8168_adsp_send_ipi_cmd(NULL,
				 TASK_SCENE_AUDIO_CONTROLLER,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_PAYLOAD,
				 AUDIO_IPI_MSG_BYPASS_ACK,
				 MSG_TO_DSP_CLK_MODE_SET,
				 sizeof(struct ipc_clk_mode_params),
				 sizeof(struct ipc_clk_mode_params),
				 (char *)&params);

#if defined(CONFIG_MTK_QOS_SUPPORT)
	if (priv->max_pll == 400000000)
		pm_qos_update_request(&priv->pm_adsp, VCORE_OPP_1);
	else if (priv->max_pll == 600000000)
		pm_qos_update_request(&priv->pm_adsp, VCORE_OPP_0);
#endif

	register_adsp_wdt_notifier(&wdt_event_notifier);
	return 0;
}

static int mt8168_adsp_low_power_uninit(struct mt8168_adsp_pcm_priv *priv)
{
	struct mtk_base_afe *afe = priv->afe;
	struct mt8168_afe_private *afe_priv = afe->platform_priv;
	struct mt8168_adsp_data *afe_adsp = &(afe_priv->adsp_data);

#if defined(CONFIG_MTK_QOS_SUPPORT)
	pm_qos_update_request(&priv->pm_adsp, PM_QOS_VCORE_OPP_DEFAULT_VALUE);
#endif
	/* fake 26M setting */
	sloa_suspend_26m_mode(CLK_26M_OFF);
	if (priv->fake26m_mode == ULPLL26M)
		rtc_disable_32k1v8_1();

	afe_adsp->set_afe_uninit(afe);
	unregister_adsp_wdt_notifier(&wdt_event_notifier);
	return 0;
}

static int mt8168_adsp_enable_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = IS_ADSP_READY();
	return 0;
}

static int mt8168_adsp_enable_put(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_platform *plat = snd_soc_kcontrol_platform(kcontrol);
	struct mt8168_adsp_pcm_priv *priv = snd_soc_platform_get_drvdata(plat);
	int ret = 0;
	int enable = ucontrol->value.integer.value[0];

	if ((enable && IS_ADSP_READY()) || (!enable && !IS_ADSP_READY()))
		return 0;

	/* already loading */
	if (priv->dsp_loading)
		return -EBUSY;

	if (enable) {
		priv->dsp_loading = true;
		ret =
		async_load_hifixdsp_bin_and_run(load_hifi4dsp_callback, priv);
		if (ret) {
			dev_dbg(plat->dev,
				"%s async_load_hifi4dsp_bin_and_run fail %d\n",
				__func__, ret);
			priv->dsp_loading = false;
		}
	}  else {
		/* TODO add stop function */
		ret = hifixdsp_stop_run();
		if (ret) {
			dev_dbg(plat->dev,
				"%s hifixdsp_stop_run fail %d\n",
				__func__, ret);
		} else {
			priv->dsp_ready = false;
			mt8168_adsp_low_power_uninit(priv);
		}
	}

	return ret;
}

static int mt8168_adsp_ap_suspend_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	/* TODO add suspend/resume value */
	return 0;
}

static int mt8168_adsp_ap_suspend_put(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	if (!IS_ADSP_READY())
		return -ENODEV;
	if (ucontrol->value.integer.value[0]) {
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_AUDIO_CONTROLLER,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_MSG_ONLY,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_AP_SUSPEND_T,
					 0, 0, NULL);
	} else {
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_AUDIO_CONTROLLER,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_MSG_ONLY,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_AP_RESUME_T,
					 0, 0, NULL);
	}

	return 0;
}

static int mt8168_adsp_va_voiceupload_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	return 0;
}

static int mt8168_adsp_va_voiceupload_put(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	if (!IS_ADSP_READY())
		return -ENODEV;
	if (ucontrol->value.integer.value[0]) {
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_MSG_ONLY,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_VOICE_UPLOAD_DONE,
					 0, 0, NULL);
	}
	ucontrol->value.integer.value[0] = 0;

	return 0;
}

static int mt8168_adsp_va_vad_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	return 0;
}

static int mt8168_adsp_va_vad_put(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	struct ipc_va_params params;
	if (!IS_ADSP_READY())
		return -ENODEV;

	params.va_type = VA_VAD;
	if (ucontrol->value.integer.value[0]) {
		params.enable_flag = 1;
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_VAD,
					 sizeof(struct ipc_va_params),
					 sizeof(struct ipc_va_params),
					 (char *)&params);
	} else {
		params.enable_flag = 0;
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_VAD,
					 sizeof(struct ipc_va_params),
					 sizeof(struct ipc_va_params),
					 (char *)&params);
	}

	return 0;
}

static int mt8168_adsp_va_aec_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	return 0;
}

static int mt8168_adsp_va_aec_put(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	struct ipc_va_params params;

	if (!IS_ADSP_READY())
		return -ENODEV;
	params.va_type = VA_AEC;
	if (ucontrol->value.integer.value[0]) {
		params.enable_flag = 1;
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_AEC,
					 sizeof(struct ipc_va_params),
					 sizeof(struct ipc_va_params),
					 (char *)&params);
	} else {
		params.enable_flag = 0;
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_AEC,
					 sizeof(struct ipc_va_params),
					 sizeof(struct ipc_va_params),
					 (char *)&params);
	}

	return 0;
}

static int mt8168_adsp_va_ww_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = 0;
	return 0;
}

static int mt8168_adsp_va_ww_put(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	struct ipc_va_params params;
	if (!IS_ADSP_READY())
		return -ENODEV;

	params.va_type = VA_KEYWORD;
	if (ucontrol->value.integer.value[0]) {
		params.enable_flag = 1;
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_KEYWORD,
					 sizeof(struct ipc_va_params),
					 sizeof(struct ipc_va_params),
					 (char *)&params);
	} else {
		params.enable_flag = 0;
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_PAYLOAD,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_AEC,
					 sizeof(struct ipc_va_params),
					 sizeof(struct ipc_va_params),
					 (char *)&params);
	}

	return 0;
}

static int mt8168_adsp_va_beamforming_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	uint32_t beamforming;
	struct ipi_msg_t ipi_msg;

	if (!IS_ADSP_READY())
		return -ENODEV;

	mt8168_adsp_send_ipi_cmd(&ipi_msg,
				 TASK_SCENE_VA,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_PAYLOAD,
				 AUDIO_IPI_MSG_NEED_ACK,
				 MSG_TO_DSP_SCENE_VA_BEAMFORMING,
				 sizeof(uint32_t),
				 sizeof(uint32_t),
				 (char *)&beamforming);

	if (ipi_msg.ack_type != AUDIO_IPI_MSG_ACK_BACK)
		beamforming = 0;

	ucontrol->value.integer.value[0] = beamforming;

	return 0;
}

static int mt8168_adsp_va_force_ok_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	return 0;
}

static int mt8168_adsp_va_force_ok_put(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	if (!IS_ADSP_READY())
		return -ENODEV;
	if (ucontrol->value.integer.value[0]) {
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_MSG_ONLY,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_VAD_FORCE_OK,
					 0, 0, NULL);
	} else {
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_MSG_ONLY,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_VAD_FORCE_OK,
					 0, 0, NULL);
		msleep(150);
		mt8168_adsp_send_ipi_cmd(NULL,
					 TASK_SCENE_VA,
					 AUDIO_IPI_LAYER_TO_DSP,
					 AUDIO_IPI_MSG_ONLY,
					 AUDIO_IPI_MSG_NEED_ACK,
					 MSG_TO_DSP_SCENE_VA_KEYWORD_FORCE_OK,
					 0, 0, NULL);
	}

	return 0;
}

static int mt8168_adsp_hostless_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component =
		snd_soc_kcontrol_component(kcontrol);
	struct snd_soc_card *card = component->card;
	struct snd_soc_pcm_runtime *rtd;
	struct mt8168_adsp_pcm_priv *priv;

	rtd = snd_soc_get_pcm_runtime(card, "ADSP HOSTLESS_VA");
	if (rtd == NULL) {
		dev_info(card->dev, "%s() runtime is NULL\n",
				__func__);
		return -ENODEV;
	}

	priv = snd_soc_platform_get_drvdata(rtd->platform);

	ucontrol->value.integer.value[0] = priv->va_hostless_enbale;
	return 0;
}

static int mt8168_adsp_hostless_put(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_pcm_runtime *rtd;
	struct snd_soc_component *component =
		snd_soc_kcontrol_component(kcontrol);
	struct snd_soc_card *card = component->card;
	struct snd_pcm *pcm;
	struct snd_pcm_substream *substream;
	struct mt8168_adsp_pcm_priv *priv;
	struct mt8168_adsp_pcm_format *hostless_config;
	int err;

	rtd = snd_soc_get_pcm_runtime(card, "ADSP HOSTLESS_VA");
	if (rtd == NULL) {
		dev_info(card->dev, "%s() runtime is NULL\n",
				__func__);
		return -ENODEV;
	}

	priv = snd_soc_platform_get_drvdata(rtd->platform);

	if (priv->va_hostless_enbale == ucontrol->value.integer.value[0])
		return 0;

	priv->va_hostless_enbale = ucontrol->value.integer.value[0];
	hostless_config = &priv->hostless_data;
	pcm = rtd->pcm;

	if (priv->va_hostless_enbale) {
		err = mt8168_adsp_hostless_pcm_open(pcm, &substream);
		if (err < 0)
			return err;

		err = mt8168_adsp_hostless_pcm_params(
			substream,
			hostless_config->rate,
			hostless_config->channels,
			hostless_config->format,
			hostless_config->period_size,
			hostless_config->period_count);
		if (err < 0)
			return err;

		err = mt8168_adsp_hostless_pcm_start(substream);
		if (err < 0)
			return err;

		priv->va_hostless_substream = substream;
	} else {
		substream = priv->va_hostless_substream;
		err = mt8168_adsp_hostless_pcm_stop(substream);
		if (err < 0)
			return err;

		mt8168_adsp_hostless_pcm_close(substream);
	}

	return 0;
}
static const char *const mt8168_adsp_clk_mode_item[] = {
	ENUM_TO_STR(FAKE26M_DCXO),
	ENUM_TO_STR(ULPLL26M),
};

static const struct soc_enum mt8168_adsp_clk_mode_enums =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(mt8168_adsp_clk_mode_item),
			    mt8168_adsp_clk_mode_item);

static int mt8168_adsp_clk_mode_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_platform *plat = snd_soc_kcontrol_platform(kcontrol);
	struct mt8168_adsp_pcm_priv *priv = snd_soc_platform_get_drvdata(plat);

	ucontrol->value.enumerated.item[0] = priv->fake26m_mode;

	return 0;
}

static int mt8168_adsp_clk_mode_put(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	struct ipc_clk_mode_params params;
	struct snd_soc_platform *plat = snd_soc_kcontrol_platform(kcontrol);
	struct mt8168_adsp_pcm_priv *priv = snd_soc_platform_get_drvdata(plat);
	int clk_mode = ucontrol->value.enumerated.item[0];

	if ((clk_mode == ULPLL26M) &&
		(pmic_get_register_value(PMIC_SWCID) != PMIC_MT6390_CHIP_ID))
		clk_mode = FAKE26M_DCXO;

	if (clk_mode == priv->fake26m_mode)
		return 0;

	priv->fake26m_mode = clk_mode;
	if (!IS_ADSP_READY())
		return 0;

	/* fake 26M setting */
	if (priv->fake26m_mode == FAKE26M_DCXO)
		sloa_suspend_26m_mode(FAKE_DCXO_26M);
	else
		sloa_suspend_26m_mode(ULPLL_26M);

	params.clk_mode = priv->fake26m_mode;
	mt8168_adsp_send_ipi_cmd(NULL,
				 TASK_SCENE_AUDIO_CONTROLLER,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_PAYLOAD,
				 AUDIO_IPI_MSG_BYPASS_ACK,
				 MSG_TO_DSP_CLK_MODE_SET,
				 sizeof(struct ipc_clk_mode_params),
				 sizeof(struct ipc_clk_mode_params),
				 (char *)&params);

	return 0;
}

static int hostless_pcm_format_get(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_platform *plat = snd_soc_kcontrol_platform(kcontrol);
	struct mt8168_adsp_pcm_priv *priv = snd_soc_platform_get_drvdata(plat);

	ucontrol->value.integer.value[0] = priv->hostless_data.rate;
	ucontrol->value.integer.value[1] = priv->hostless_data.channels;
	ucontrol->value.integer.value[2] = priv->hostless_data.format;
	ucontrol->value.integer.value[3] = priv->hostless_data.period_size;
	ucontrol->value.integer.value[4] = priv->hostless_data.period_count;

	return 0;
}

static int hostless_pcm_format_put(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_platform *plat = snd_soc_kcontrol_platform(kcontrol);
	struct mt8168_adsp_pcm_priv *priv = snd_soc_platform_get_drvdata(plat);

	priv->hostless_data.rate = ucontrol->value.integer.value[0];
	priv->hostless_data.channels = ucontrol->value.integer.value[1];
	priv->hostless_data.format = ucontrol->value.integer.value[2];
	priv->hostless_data.period_size = ucontrol->value.integer.value[3];
	priv->hostless_data.period_count = ucontrol->value.integer.value[4];

	return 0;
}

static int hostless_pcm_format_info(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 5;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1000000;

	return 0;
}

#define SOC_HOSTLESS_PCM_FORMAT_DECL(xname) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.info = hostless_pcm_format_info, .get = hostless_pcm_format_get, \
	.put = hostless_pcm_format_put, }

static const struct snd_kcontrol_new mt8168_adsp_controls[] = {
	SOC_SINGLE_BOOL_EXT("ADSP Enable",
			    0,
			    mt8168_adsp_enable_get,
			    mt8168_adsp_enable_put),
	SOC_SINGLE_BOOL_EXT("VA VAD Enable",
			    0,
			    mt8168_adsp_va_vad_get,
			    mt8168_adsp_va_vad_put),
	SOC_SINGLE_BOOL_EXT("VA AEC Enable",
			    0,
			    mt8168_adsp_va_aec_get,
			    mt8168_adsp_va_aec_put),
	SOC_SINGLE_BOOL_EXT("VA WakeWord Enable",
			    0,
			    mt8168_adsp_va_ww_get,
			    mt8168_adsp_va_ww_put),
	SOC_SINGLE_EXT("VA Beamforming",
			    0,
			    0,
			    0x800000,
			    0,
			    mt8168_adsp_va_beamforming_get,
			    0),
	SOC_SINGLE_BOOL_EXT("VA VOICEUPLOAD Done",
			    0,
			    mt8168_adsp_va_voiceupload_get,
			    mt8168_adsp_va_voiceupload_put),
	SOC_SINGLE_BOOL_EXT("AP Suspend Test",
			    0,
			    mt8168_adsp_ap_suspend_get,
			    mt8168_adsp_ap_suspend_put),

	SOC_SINGLE_BOOL_EXT("VA Force Test",
			    0,
			    mt8168_adsp_va_force_ok_get,
			    mt8168_adsp_va_force_ok_put),
	SOC_SINGLE_BOOL_EXT("VA Hostless Enable",
			    0,
			    mt8168_adsp_hostless_get,
			    mt8168_adsp_hostless_put),
	SOC_ENUM_EXT("ADSP Clock Mode",
			    mt8168_adsp_clk_mode_enums,
			    mt8168_adsp_clk_mode_get,
			    mt8168_adsp_clk_mode_put),
	SOC_HOSTLESS_PCM_FORMAT_DECL("Hostless_Pcm_Format Select"),
};

static const struct snd_soc_component_driver mt8168_adsp_pcm_dai_comp_drv = {
	.name = "mt8168-adsp-pcm-dai",
	.dapm_widgets = mt8168_adsp_pcm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(mt8168_adsp_pcm_widgets),
	.dapm_routes = mt8168_adsp_pcm_routes,
	.num_dapm_routes = ARRAY_SIZE(mt8168_adsp_pcm_routes),
	.controls = mt8168_adsp_controls,
	.num_controls = ARRAY_SIZE(mt8168_adsp_controls),
};

static void mt8168_adsp_pcm_data_copy(struct snd_pcm_substream *substream,
	struct mt8168_adsp_dai_memory *dai_mem)
{
	unsigned char *adsp_dma_buf_vaddr =
		   dai_mem->adsp_dma.start_addr;
	uint32_t adsp_dma_buf_size = dai_mem->adsp_dma.size_bytes;
	uint32_t adsp_dma_hw_off = 0;
	uint32_t adsp_dma_appl_off = dai_mem->adsp_dma.appl_offset_bytes;
	unsigned char *cpu_dma_buf_vaddr =
		    dai_mem->cpu_dma.dma_buf_vaddr;
	uint32_t cpu_dma_buf_size = dai_mem->cpu_dma.dma_buf_size;
	uint32_t cpu_dma_offset = dai_mem->cpu_dma.dma_offset;
	uint32_t cpu_period_size_bytes = dai_mem->cpu_dma.dma_period_size_bytes;
	uint32_t avail_bytes;
	uint32_t cpu_dma_free_bytes;
	uint32_t copy_bytes;
	uint32_t adsp_period_size_bytes =
			   ((dai_mem->fe_format.period_size) *
			   (dai_mem->fe_format.channels) *
			   (dai_mem->fe_format.format == 32 ? 4 : 2));

	adsp_dma_hw_off =
		dai_mem->adsp_dma_control->ptr_to_hw_offset_bytes;

	if (adsp_dma_hw_off >= adsp_dma_appl_off) {
		avail_bytes = adsp_dma_hw_off - adsp_dma_appl_off;
	} else {
		avail_bytes = adsp_dma_buf_size - adsp_dma_appl_off +
			adsp_dma_hw_off;
	}

	cpu_dma_free_bytes = snd_pcm_capture_hw_avail(substream->runtime);
	cpu_dma_free_bytes =
		frames_to_bytes(substream->runtime, cpu_dma_free_bytes);

	if (avail_bytes >= cpu_dma_free_bytes)
		avail_bytes = cpu_dma_free_bytes - cpu_period_size_bytes;

	if (avail_bytes < adsp_period_size_bytes)
		return;

	copy_bytes = (avail_bytes / adsp_period_size_bytes) *
			     adsp_period_size_bytes;

	while (copy_bytes > 0) {
		uint32_t from_bytes = 0;

		if (adsp_dma_hw_off >= adsp_dma_appl_off)
			from_bytes = adsp_dma_hw_off - adsp_dma_appl_off;
		else
			from_bytes = adsp_dma_buf_size - adsp_dma_appl_off;

		if (from_bytes > copy_bytes)
			from_bytes = copy_bytes;

		while (from_bytes > 0) {
			uint32_t to_bytes = 0;

			if (cpu_dma_offset + from_bytes < cpu_dma_buf_size)
				to_bytes = from_bytes;
			else
				to_bytes = cpu_dma_buf_size - cpu_dma_offset;

			memcpy(cpu_dma_buf_vaddr + cpu_dma_offset,
				adsp_dma_buf_vaddr + adsp_dma_appl_off,
				to_bytes);

			from_bytes -= to_bytes;
			copy_bytes -= to_bytes;

			cpu_dma_offset = (cpu_dma_offset + to_bytes) %
				cpu_dma_buf_size;
			adsp_dma_appl_off = (adsp_dma_appl_off + to_bytes) %
				adsp_dma_buf_size;
		}
	}

	dai_mem->adsp_dma_control->ptr_to_appl_offset_bytes =
		adsp_dma_appl_off;
	dai_mem->adsp_dma.appl_offset_bytes = adsp_dma_appl_off;
	dai_mem->adsp_dma.hw_offset_bytes = adsp_dma_hw_off;
	dai_mem->cpu_dma.dma_offset = cpu_dma_offset;
}

static snd_pcm_uframes_t mt8168_adsp_pcm_pointer(
	struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(rtd->platform);
	int id = rtd->cpu_dai->id;
	struct mt8168_adsp_dai_memory *dai_mem = &priv->dai_mem[id];

	mt8168_adsp_pcm_data_copy(substream, dai_mem);

	return bytes_to_frames(runtime, priv->dai_mem[id].cpu_dma.dma_offset);
}

const struct snd_pcm_ops mt8168_adsp_pcm_ops = {
	.ioctl = snd_pcm_lib_ioctl,
	.pointer = mt8168_adsp_pcm_pointer,
};

static void audio_ipi_ul_irq_handler(int id)
{
	struct mt8168_adsp_dai_memory *dai_mem = &g_priv->dai_mem[id];
	struct snd_pcm_substream *substream = dai_mem->substream;

	snd_pcm_period_elapsed(substream);
}

static void mt8168_adsp_pcm_va_notify(struct dsp_ipc_va_notify *info)
{
	char *message = "ACTION=HOTWORD";
	char *hot_event[] = { message, NULL };


#if defined(CONFIG_PM_WAKELOCKS)
	__pm_wakeup_event(g_priv->va_suspend_lock, 800);
#endif
	/* TODO improve the printf method & how to store information to priv */
	if (info->type == VA_NOTIFY_VAD_PASS)
		pr_notice("get vad ok notify\n");
	else if (info->type == VA_NOTIFY_WAKEWORD_PASS)
		pr_notice("get wakeword(%s) ok notify\n", info->wakeword);
	kobject_uevent_env(&g_priv->dev->kobj, KOBJ_CHANGE, hot_event);
}

static void mt8168_adsp_pcm_ipi_recv_msg(struct ipi_msg_t *p_ipi_msg)
{
	if (!p_ipi_msg)
		return;
	if (p_ipi_msg->task_scene == TASK_SCENE_AUDIO_CONTROLLER) {
		switch (p_ipi_msg->msg_id) {
		case MSG_TO_HOST_DSP_AUDIO_READY:
			{
#if defined(CONFIG_MTK_QOS_SUPPORT)
				struct dsp_info_notify init_info;

				memcpy((void *)&init_info,
					(void *)(p_ipi_msg->payload),
					sizeof(struct dsp_info_notify));
				g_priv->max_pll = (int)init_info.max_pll;
#endif
				g_priv->dsp_loading = false;
			}
			break;
		case MSG_TO_HOST_DSP_DEBUG_IRQ:
			mt8168_adsp_handle_debug_dump_irq(p_ipi_msg);
			break;
		default:
			break;
		}
	} else if (p_ipi_msg->task_scene == TASK_SCENE_VA) {
		switch (p_ipi_msg->msg_id) {
		case MSG_TO_HOST_DSP_IRQUL:
			{
				struct dsp_ipc_msg_irq ipc_irq;

				memcpy((void *)&ipc_irq,
					(void *)(p_ipi_msg->payload),
					sizeof(struct dsp_ipc_msg_irq));
				/* TODO, need add share buffer info */
				audio_ipi_ul_irq_handler(ipc_irq.dai_id);
			}
			break;
		case MSG_TO_HOST_VA_NOTIFY:
			{
				struct dsp_ipc_va_notify info;

				memcpy((void *)&info,
					(void *)(p_ipi_msg->payload),
					sizeof(struct dsp_ipc_va_notify));
				mt8168_adsp_pcm_va_notify(&info);
			}
			break;
		default:
			break;
		}
	}
}
/* TODO remove unused attribute */
static void load_hifi4dsp_callback(void *arg)
{
	struct mt8168_adsp_pcm_priv *priv = arg;

	if (!hifixdsp_run_status())
		dev_warn(priv->dev,
			 "%s hifi4dsp_run_status not done\n",
			 __func__);
	while (priv->dsp_loading)
		usleep_range(10000, 11000);

	mt8168_adsp_low_power_init(priv);
	mt8168_adsp_send_ipi_cmd(NULL,
				 TASK_SCENE_AUDIO_CONTROLLER,
				 AUDIO_IPI_LAYER_TO_DSP,
				 AUDIO_IPI_MSG_ONLY,
				 AUDIO_IPI_MSG_NEED_ACK,
				 MSG_TO_DSP_CREATE_VA_T,
				 0, 0, NULL);
	audio_task_register_callback(TASK_SCENE_VA,
				     mt8168_adsp_pcm_ipi_recv_msg,
				     NULL);
	priv->dsp_ready = true;
}

static int mt8168_adsp_pcm_probe(struct snd_soc_platform *platform)
{
	int ret = 0;
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(platform);
	struct mt8168_adsp_data *afe_adsp;
	struct mt8168_afe_private *afe_priv;

	/* TODO, how to get afe private */
	priv->afe = mt8168_afe_pcm_get_info();
	afe_priv = (struct mt8168_afe_private *)(priv->afe->platform_priv);
	afe_adsp = &(afe_priv->adsp_data);
	afe_adsp->adsp_on = true;
	afe_adsp->hostless_active = mt8168_adsp_hostless_active;

	ret = audio_task_register_callback(TASK_SCENE_AUDIO_CONTROLLER,
					   mt8168_adsp_pcm_ipi_recv_msg,
					   NULL);
	if (ret) {
		dev_err(platform->dev,
			"%s register callback for audio controller fail %d\n",
			__func__, ret);
		return ret;
	}

	mt8168_adsp_init_debug_dump(priv);

#ifdef CONFIG_ADSP_MISC_DEVICE
	mt8168_adsp_init_load_model(priv);
#endif

	if (!priv->dsp_boot_run)
		return 0;

	priv->dsp_loading = true;
	ret = async_load_hifixdsp_bin_and_run(load_hifi4dsp_callback, priv);
	if (ret) {
		dev_err(platform->dev,
			"%s async_load_hifi4dsp_bin_and_run fail %d\n",
			__func__, ret);
		priv->dsp_loading = false;
		return ret;
	}

	return 0;
}
static int mt8168_adsp_pcm_remove(struct snd_soc_platform *platform)
{
	struct mt8168_adsp_pcm_priv *priv =
		snd_soc_platform_get_drvdata(platform);

	mt8168_adsp_cleanup_debug_dump(priv);

	return 0;
}

static int mt8168_adsp_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	size_t size = mt8168_adsp_pcm_pcm_hardware.buffer_bytes_max;
	struct snd_card *card = rtd->card->snd_card;
	struct snd_pcm *pcm = rtd->pcm;

	return snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_DEV,
						     card->dev, size, size);
}

static void mt8168_adsp_pcm_free(struct snd_pcm *pcm)
{
	snd_pcm_lib_preallocate_free_for_all(pcm);
}

const struct snd_soc_platform_driver mt8168_adsp_pcm_platform = {
	.probe = mt8168_adsp_pcm_probe,
	.remove = mt8168_adsp_pcm_remove,
	.ops = &mt8168_adsp_pcm_ops,
	.pcm_new = mt8168_adsp_pcm_new,
	.pcm_free = mt8168_adsp_pcm_free,
};

static int mt8168_adsp_pcm_parse_of(struct mt8168_adsp_pcm_priv *priv,
				     struct device_node *np)
{
	int ret = 0;
	int i;
	char prop[128];
	unsigned int val;
	struct {
		char *name;
		unsigned int val;
	} of_be_table[] = {
		{ "vul2",	MT8168_ADSP_BE_VUL2_IN },
		{ "tdmin",	MT8168_ADSP_BE_TDM_IN },
		{ "vul",	MT8168_ADSP_BE_VUL_IN },
	};

	if (!priv || !np)
		return -EINVAL;

	ret = of_property_read_u32_array(np, "mediatek,dsp-boot-run", &val, 1);
	if (ret)
		priv->dsp_boot_run = false;
	else
		priv->dsp_boot_run = (val == 1) ? true : false;

	/* default adsp memif use low power memory for dma */
	for (i = 0; i < ARRAY_SIZE(of_be_table); i++) {
		snprintf(prop, sizeof(prop), "mediatek,%s-mem-type",
			 of_be_table[i].name);
		ret = of_property_read_u32_array(np, prop, &val, 1);
		if (ret)
			priv->be_data[i].mem_type = AFE_MEM_TYPE_LP;
		else
			priv->be_data[i].mem_type = (int)val;
	}

	ret = of_property_read_u32_array(np,
			 "mediatek,hostless-pcm-rate",
			 &val,
			 1);
	if (ret)
		priv->hostless_data.rate = 16000;
	else
		priv->hostless_data.rate = val;

	ret = of_property_read_u32_array(np,
			 "mediatek,hostless-pcm-ch",
			 &val,
			 1);
	if (ret)
		priv->hostless_data.channels = 1;
	else
		priv->hostless_data.channels = val;

	ret = of_property_read_u32_array(np,
			 "mediatek,hostless-pcm-format",
			 &val,
			 1);
	if (ret)
		priv->hostless_data.format = 16;
	else
		priv->hostless_data.format = val;

	ret = of_property_read_u32_array(np,
			 "mediatek,hostless-pcm-period-size",
			 &val,
			 1);
	if (ret)
		priv->hostless_data.period_size = 160;
	else
		priv->hostless_data.period_size = val;

	ret = of_property_read_u32_array(np,
			 "mediatek,hostless-pcm-period-count",
			 &val,
			 1);
	if (ret)
		priv->hostless_data.period_count = 8;
	else
		priv->hostless_data.period_count = val;

	return ret;
}

static int mt8168_adsp_pcm_dev_probe(struct platform_device *pdev)
{
	int ret;
	struct mt8168_adsp_pcm_priv *priv;
	struct device *dev = &pdev->dev;

	priv = devm_kzalloc(dev,
			    sizeof(struct mt8168_adsp_pcm_priv),
			    GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;

	platform_set_drvdata(pdev, priv);
	g_priv = priv;

	if (pmic_get_register_value(PMIC_SWCID) == PMIC_MT6390_CHIP_ID)
		priv->fake26m_mode = ULPLL26M;
	else
		priv->fake26m_mode = FAKE26M_DCXO;

	mt8168_adsp_pcm_parse_of(priv, dev->of_node);

	ret = snd_soc_register_platform(dev, &mt8168_adsp_pcm_platform);
	if (ret < 0) {
		dev_err(dev, "Failed to register platform\n");
		return ret;
	}

	ret = snd_soc_register_component(dev,
					 &mt8168_adsp_pcm_dai_comp_drv,
					 mt8168_adsp_pcm_dais,
					 ARRAY_SIZE(mt8168_adsp_pcm_dais));
	if (ret < 0) {
		dev_err(dev, "Failed to register component\n");
		goto err_platform;
	}
#if defined(CONFIG_MTK_QOS_SUPPORT)
	pm_qos_add_request(&priv->pm_adsp,
		PM_QOS_VCORE_OPP, PM_QOS_VCORE_OPP_DEFAULT_VALUE);
#endif

#if defined(CONFIG_PM_WAKELOCKS)
	g_priv->va_suspend_lock = wakeup_source_register("va_notify_wakelock");
#endif
	dev_info(dev, "%s initialized.\n", __func__);
	return 0;

err_platform:
	snd_soc_unregister_platform(dev);
	return ret;
}

static int mt8168_adsp_pcm_dev_remove(struct platform_device *pdev)
{
#if defined(CONFIG_MTK_QOS_SUPPORT)
	struct mt8168_adsp_pcm_priv *priv;

	priv = platform_get_drvdata(pdev);
	pm_qos_remove_request(&g_priv->pm_adsp);
#endif

#if defined(CONFIG_PM_WAKELOCKS)
	wakeup_source_unregister(g_priv->va_suspend_lock);
#endif
	snd_soc_unregister_component(&pdev->dev);
	snd_soc_unregister_platform(&pdev->dev);
	return 0;
}

static const struct of_device_id mt8168_adsp_pcm_dt_match[] = {
	{ .compatible = "mediatek,mt8168-adsp-pcm", },
	{ }
};
MODULE_DEVICE_TABLE(of, mt8168_adsp_pcm_dt_match);

static struct platform_driver mt8168_adsp_pcm_driver = {
	.driver = {
		.name = "mt8168-adsp-pcm",
		.of_match_table = mt8168_adsp_pcm_dt_match,
	},
	.probe = mt8168_adsp_pcm_dev_probe,
	.remove = mt8168_adsp_pcm_dev_remove,
};
module_platform_driver(mt8168_adsp_pcm_driver);

MODULE_DESCRIPTION("MT8168 Audio SPI driver");
MODULE_AUTHOR("Bo Pan <bo.pan@mediatek.com>");
MODULE_LICENSE("GPL v2");
