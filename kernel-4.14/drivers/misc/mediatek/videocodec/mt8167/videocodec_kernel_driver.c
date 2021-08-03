/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <asm/page.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
/* #include <mach/x_define_irq.h> */
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
/* #include <linux/earlysuspend.h> */
#include "sync_write.h"
/* #include "mach/mt_reg_base.h" */
#ifdef CONFIG_OF
#include <linux/clk.h>
#else
#include "mach/mt_clkmgr.h"
#endif
#include "mtk_smi.h"
#ifdef CONFIG_MTK_HIBERNATION
#include <mtk_hibernate_dpm.h>
#endif

#include "drv_api.h"
#include "val_api_private.h"
/*#include "val_log.h"*/
#include "val_types_private.h"
#include "videocodec_kernel_driver.h"
#include "../videocodec_kernel.h"
#include <asm/cacheflush.h>
#include <linux/io.h>
#include <linux/sizes.h>

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/pm_runtime.h>
#if IS_ENABLED(CONFIG_COMPAT)
#include <linux/compat.h>
#include <linux/uaccess.h>
#endif
#include "mtk_sched.h"

#define VDO_HW_WRITE(ptr, data) mt_reg_sync_writel(data, ptr)
#define VDO_HW_READ(ptr) readl((void __iomem *)ptr)

#define VCODEC_DEVNAME "Vcodec"
#define VDECDISP_DEVNAME "VDecDisp"
/*#define VENC_DEVNAME     "Venc"*/
/* #define VENCLT_DEVNAME   "Venclt"*/
#define MT8167_VCODEC_DEV_MAJOR_NUMBER 160 /* 189 */
/* #define MT8167_VENC_USE_L2C */

static dev_t vcodec_devno = MKDEV(MT8167_VCODEC_DEV_MAJOR_NUMBER, 0);
static struct cdev *vcodec_cdev;
static struct class *vcodec_class;
static struct device *vcodec_device;

/* static dev_t venc_devno;*/
/* static struct cdev *venc_cdev;*/
/* static struct class *venc_class;*/
/* static dev_t venclt_devno;*/
/* static struct cdev *venclt_cdev;*/
/* static struct class *venclt_class;*/

static DEFINE_MUTEX(IsOpenedLock);
static DEFINE_MUTEX(PWRLock);
static DEFINE_MUTEX(VdecHWLock);
static DEFINE_MUTEX(VencHWLock);
static DEFINE_MUTEX(EncEMILock);
static DEFINE_MUTEX(L2CLock);
static DEFINE_MUTEX(DecEMILock);
static DEFINE_MUTEX(DriverOpenCountLock);
static DEFINE_MUTEX(DecHWLockEventTimeoutLock);
static DEFINE_MUTEX(EncHWLockEventTimeoutLock);

static DEFINE_MUTEX(VdecPWRLock);
static DEFINE_MUTEX(VencPWRLock);

static DEFINE_SPINLOCK(DecIsrLock);
static DEFINE_SPINLOCK(EncIsrLock);
static DEFINE_SPINLOCK(LockDecHWCountLock);
static DEFINE_SPINLOCK(LockEncHWCountLock);
static DEFINE_SPINLOCK(DecISRCountLock);
static DEFINE_SPINLOCK(EncISRCountLock);

static struct clk *g_clk_venc;
static struct clk *g_clk_vdec_sel;
static struct clk *g_clk_vdec_cken;
/* mutex : HWLockEventTimeoutLock */
static struct VAL_EVENT_T DecHWLockEvent;
/* mutex : HWLockEventTimeoutLock */
static struct VAL_EVENT_T EncHWLockEvent;
/* mutex : HWLockEventTimeoutLock */
static struct VAL_EVENT_T DecIsrEvent;
/* mutex : HWLockEventTimeoutLock */
static struct VAL_EVENT_T EncIsrEvent;
static signed int MT8167Driver_Open_Count; /* mutex : DriverOpenCountLock */
static unsigned int gu4PWRCounter;	  /* mutex : PWRLock */
static unsigned int gu4EncEMICounter;       /* mutex : EncEMILock */
static unsigned int gu4DecEMICounter;       /* mutex : DecEMILock */
static unsigned int gu4L2CCounter;	  /* mutex : L2CLock */
static char bIsOpened = VAL_FALSE;    /* mutex : IsOpenedLock */
static unsigned int
	gu4HwVencIrqStatus; /* hardware VENC IRQ status (VP8/H264) */

static unsigned int gu4VdecPWRCounter; /* mutex : VdecPWRLock */
static unsigned int gu4VencPWRCounter; /* mutex : VencPWRLock */

static unsigned int gLockTimeOutCount;

static unsigned int gu4VdecLockThreadId;

/*#define MT8167_VCODEC_DEBUG*/
#ifdef MT8167_VCODEC_DEBUG
#undef VCODEC_DEBUG
#define VCODEC_DEBUG pr_debug
#undef MODULE_MFV_PR_DEBUG
#define MODULE_MFV_PR_DEBUG pr_debug
#else
#define VCODEC_DEBUG(...)
#undef MODULE_MFV_PR_DEBUG
#define MODULE_MFV_PR_DEBUG(...)
#endif

/* VENC physical base address */
#undef VENC_BASE

#ifdef CONFIG_MACH_MT8167
#define VENC_BASE 0x15009000
#define VENC_LT_BASE 0x19002000
#define VENC_REGION 0x1000
#else
#define VENC_BASE 0x18002000
#define VENC_LT_BASE 0x19002000
#define VENC_REGION 0x1000
#endif

/* VDEC virtual base address */
#define VDEC_BASE_PHY 0x16000000
#define VDEC_REGION 0x29000

#define HW_BASE 0x7FFF000
#define HW_REGION 0x2000

#define INFO_BASE 0x10000000
#define INFO_REGION 0x1000

#define VENC_IRQ_STATUS_SPS 0x1
#define VENC_IRQ_STATUS_PPS 0x2
#define VENC_IRQ_STATUS_FRM 0x4
#define VENC_IRQ_STATUS_DRAM 0x8
#define VENC_IRQ_STATUS_PAUSE 0x10
#define VENC_IRQ_STATUS_SWITCH 0x20
#define VENC_IRQ_STATUS_VPS 0x80
#define VENC_IRQ_STATUS_DRAM_VP8 0x20
#define VENC_SW_PAUSE 0x0AC
#define VENC_SW_HRST_N 0x0A8

unsigned long KVA_VENC_IRQ_ACK_ADDR, KVA_VENC_IRQ_STATUS_ADDR, KVA_VENC_BASE;
unsigned long KVA_VENC_LT_IRQ_ACK_ADDR, KVA_VENC_LT_IRQ_STATUS_ADDR,
	KVA_VENC_LT_BASE;
unsigned long KVA_VDEC_MISC_BASE, KVA_VDEC_VLD_BASE, KVA_VDEC_BASE,
	KVA_VDEC_GCON_BASE;
unsigned int VENC_IRQ_ID, VENC_LT_IRQ_ID, VDEC_IRQ_ID;
unsigned long KVA_VENC_SW_PAUSE, KVA_VENC_SW_HRST_N;
unsigned long KVA_VENC_LT_SW_PAUSE, KVA_VENC_LT_SW_HRST_N;

#ifdef CONFIG_OF
/*static struct clk *clk_venc_lt_clk;*/
/* static struct clk *clk_venc_pwr, *clk_venc_pwr2; */

struct platform_device *pvenc_dev;
struct platform_device *pvenclt_dev;

#endif
static int venc_enableIRQ(struct VAL_HW_LOCK_T *prHWLock);
static int venc_disableIRQ(struct VAL_HW_LOCK_T *prHWLock);

static struct platform_device *vcodec_get_pdev(char *dev_name)
{
	struct device_node *np;
	struct platform_device *pdev;

	np = of_find_compatible_node(NULL, NULL, dev_name);
	if (np == NULL)
		MODULE_MFV_PR_DEBUG("%s np find fail\n", dev_name);

	pdev = of_find_device_by_node(np);

	return pdev;
}
static struct platform_device *vcodec_get_larb(char *dev_name)
{
	struct device_node *vcodec_node;
	struct device_node *larb_node;
	struct platform_device *larb_pdev;

	vcodec_node = of_find_compatible_node(NULL, NULL, dev_name);
	if (vcodec_node == NULL) {
		MODULE_MFV_PR_DEBUG("[ERROR] %s np find fail\n", dev_name);
		return NULL;
	}

	larb_node = of_parse_phandle(vcodec_node, "mediatek,larb", 0);
	if (larb_node == NULL) {
		MODULE_MFV_PR_DEBUG("[ERR]Missing mediatek,larb phandle\n");
		return NULL;
	}


	larb_pdev = of_find_device_by_node(larb_node);
	if (!larb_pdev) {
		MODULE_MFV_PR_DEBUG("[ERR]Waiting for larb device\n");
		return NULL;
	}

	return larb_pdev;
}

void vdec_power_on(void)
{
	struct platform_device *pLarbVdecDev;

	mutex_lock(&VdecPWRLock);
	gu4VdecPWRCounter++;
	mutex_unlock(&VdecPWRLock);

	/* Central power on */
	pLarbVdecDev = vcodec_get_larb("mediatek,mt8167-vdec");

	MODULE_MFV_PR_DEBUG("%s D +\n", __func__);
	pm_runtime_get_sync(&pLarbVdecDev->dev);
	clk_prepare_enable(g_clk_vdec_sel);
	clk_prepare_enable(g_clk_vdec_cken);
	MODULE_MFV_PR_DEBUG("%s D -\n", __func__);
}

void vdec_power_off(void)
{
	struct platform_device *pLarbVdecDev;

	mutex_lock(&VdecPWRLock);
	if (gu4VdecPWRCounter != 0) {
		gu4VdecPWRCounter--;
		/* Central power off */
		pLarbVdecDev = vcodec_get_larb("mediatek,mt8167-vdec");

		clk_disable_unprepare(g_clk_vdec_cken);
		clk_disable_unprepare(g_clk_vdec_sel);
		MODULE_MFV_PR_DEBUG("%s D +\n", __func__);
		pm_runtime_put_sync(&pLarbVdecDev->dev);
		MODULE_MFV_PR_DEBUG("%s D -\n", __func__);
	}
	mutex_unlock(&VdecPWRLock);
}

void venc_power_on(void)
{
	struct platform_device *pLarbDev;

	mutex_lock(&VencPWRLock);
	gu4VencPWRCounter++;
	mutex_unlock(&VencPWRLock);

	MODULE_MFV_PR_DEBUG("%s D +\n", __func__);
	//pVencDev = vcodec_get_pdev("mediatek,mt8167-venc");

	//open larb clock
	pLarbDev = vcodec_get_larb("mediatek,mt8167-venc");
	pm_runtime_get_sync(&pLarbDev->dev);
	//open venc clock
	clk_prepare_enable(g_clk_venc);
	/* clk_prepare(clk_venc_clk);
	 * clk_enable(clk_venc_clk);
	 * clk_prepare(clk_venc_lt_clk);
	 * clk_enable(clk_venc_lt_clk);
	 */
	MODULE_MFV_PR_DEBUG("%s D -\n", __func__);
}

void venc_power_off(void)
{
	struct platform_device *pLarbDev;

	mutex_lock(&VencPWRLock);
	if (gu4VencPWRCounter == 0) {
		MODULE_MFV_PR_DEBUG("%s none +\n", __func__);
	} else {
		gu4VencPWRCounter--;
		MODULE_MFV_PR_DEBUG("%s D+\n", __func__);
		/* clk_disable(clk_venc_clk);
		 * clk_unprepare(clk_venc_clk);
		 * clk_disable(clk_venc_lt_clk);
		 * clk_unprepare(clk_venc_lt_clk);
		 */
		clk_disable_unprepare(g_clk_venc);
		//pVencDev = vcodec_get_pdev("mediatek,mt8167-venc");
		pLarbDev = vcodec_get_larb("mediatek,mt8167-venc");
		pm_runtime_put_sync(&pLarbDev->dev);
		MODULE_MFV_PR_DEBUG("%s D-\n", __func__);
	}
	mutex_unlock(&VencPWRLock);
}

void dec_isr(void)
{
	enum VAL_RESULT_T eValRet;
	unsigned long ulFlags, ulFlagsISR, ulFlagsLockHW;

	unsigned int u4TempDecISRCount = 0;
	unsigned int u4TempLockDecHWCount = 0;
	unsigned int u4CgStatus = 0;
	unsigned int u4DecDoneStatus = 0;

	u4CgStatus = VDO_HW_READ(KVA_VDEC_GCON_BASE);
	if ((u4CgStatus & 0x10) != 0) {
		MODULE_MFV_PR_DEBUG(
			"[MFV][ERROR] DEC ISR, VDEC active is not 0x0 (0x%08x)",
			u4CgStatus);
		return;
	}

	u4DecDoneStatus = VDO_HW_READ(KVA_VDEC_MISC_BASE + 0xA4);
	if ((u4DecDoneStatus & (0x1 << 16)) != 0x10000) {
		MODULE_MFV_PR_DEBUG(
			"[MFV][ERROR] DEC ISR, Decode done status is not 0x1 (0x%08x)",
			u4DecDoneStatus);
		return;
	}

	spin_lock_irqsave(&DecISRCountLock, ulFlagsISR);
	gu4DecISRCount++;
	u4TempDecISRCount = gu4DecISRCount;
	spin_unlock_irqrestore(&DecISRCountLock, ulFlagsISR);

	spin_lock_irqsave(&LockDecHWCountLock, ulFlagsLockHW);
	u4TempLockDecHWCount = gu4LockDecHWCount;
	spin_unlock_irqrestore(&LockDecHWCountLock, ulFlagsLockHW);

	/* Clear interrupt */
	VDO_HW_WRITE(KVA_VDEC_MISC_BASE + 41 * 4,
		     VDO_HW_READ(KVA_VDEC_MISC_BASE + 41 * 4) | 0x11);
	VDO_HW_WRITE(KVA_VDEC_MISC_BASE + 41 * 4,
		     VDO_HW_READ(KVA_VDEC_MISC_BASE + 41 * 4) & ~0x10);

	spin_lock_irqsave(&DecIsrLock, ulFlags);
	eValRet = eVideoSetEvent(&DecIsrEvent, sizeof(struct VAL_EVENT_T));
	if (eValRet != VAL_RESULT_NO_ERROR) {
		/* Add one line comment for avoid kernel */
		/* coding style, WARNING:BRACES: */
		MODULE_MFV_PR_DEBUG(
			"[VCODEC][ERROR] ISR set DecIsrEvent error\n");
	}
	spin_unlock_irqrestore(&DecIsrLock, ulFlags);
}

void enc_isr(void)
{
	enum VAL_RESULT_T eValRet;
	unsigned long ulFlagsISR, ulFlagsLockHW;

	unsigned int u4TempEncISRCount = 0;
	unsigned int u4TempLockEncHWCount = 0;
	/* ---------------------- */
	spin_lock_irqsave(&EncISRCountLock, ulFlagsISR);
	gu4EncISRCount++;
	u4TempEncISRCount = gu4EncISRCount;
	spin_unlock_irqrestore(&EncISRCountLock, ulFlagsISR);

	spin_lock_irqsave(&LockEncHWCountLock, ulFlagsLockHW);
	u4TempLockEncHWCount = gu4LockEncHWCount;
	spin_unlock_irqrestore(&LockEncHWCountLock, ulFlagsLockHW);
	if (u4TempEncISRCount != u4TempLockEncHWCount) {
		/* Add one line comment for avoid kernel coding style,*/
		/* WARNING:BRACES: */
		/* MODULE_MFV_PR_DEBUG("[INFO] Enc ISRCount: 0x%x,*/
		/* * LockHWCount:0x%x\n", */
		/* * u4TempEncISRCount, u4TempLockEncHWCount);*/
	}

	if (grVcodecEncHWLock.pvHandle == 0) {
		MODULE_MFV_PR_DEBUG(
			"[ERROR] NO one Lock Enc HW, please check!!\n");

		/* Clear all status */
		/* VDO_HW_WRITE(KVA_VENC_MP4_IRQ_ACK_ADDR, 1); */
		VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR, VENC_IRQ_STATUS_PAUSE);
		VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR, VENC_IRQ_STATUS_SWITCH);
		VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR, VENC_IRQ_STATUS_DRAM);
		VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR, VENC_IRQ_STATUS_SPS);
		VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR, VENC_IRQ_STATUS_PPS);
		VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR, VENC_IRQ_STATUS_FRM);
		VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR, VENC_IRQ_STATUS_PAUSE);

/*VP8 IRQ reset */
#ifndef CONFIG_MACH_MT8167
		VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR, VENC_IRQ_STATUS_PAUSE);
		VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR, VENC_IRQ_STATUS_SWITCH);
		VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR, VENC_IRQ_STATUS_DRAM);
		VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR, VENC_IRQ_STATUS_SPS);
		VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR, VENC_IRQ_STATUS_PPS);
		VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR, VENC_IRQ_STATUS_FRM);
		VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR, VENC_IRQ_STATUS_PAUSE);
		VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR,
			     VENC_IRQ_STATUS_DRAM_VP8);
#endif
		return;
	}

	if (grVcodecEncHWLock.eDriverType ==
	    VAL_DRIVER_TYPE_H264_ENC) { /*added by bin.liu  hardwire */
		gu4HwVencIrqStatus = VDO_HW_READ(KVA_VENC_IRQ_STATUS_ADDR);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_PAUSE)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_PAUSE);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_SWITCH)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_SWITCH);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_DRAM)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_DRAM);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_SPS)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_SPS);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_PPS)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_PPS);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_FRM)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_FRM);
	} else if (grVcodecEncHWLock.eDriverType == VAL_DRIVER_TYPE_VP8_ENC) {
		gu4HwVencIrqStatus = VDO_HW_READ(KVA_VENC_LT_IRQ_STATUS_ADDR);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_PAUSE)
			VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_PAUSE);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_SWITCH)
			VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_SWITCH);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_DRAM_VP8)
			VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_DRAM_VP8);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_DRAM)
			VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_DRAM);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_SPS)
			VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_SPS);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_PPS)
			VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_PPS);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_FRM)
			VDO_HW_WRITE(KVA_VENC_LT_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_FRM);
	}
#ifdef CONFIG_MTK_VIDEO_HEVC_SUPPORT
	else if (grVcodecEncHWLock.eDriverType ==
		 VAL_DRIVER_TYPE_HEVC_ENC) { /* hardwire */
		/* MODULE_MFV_PR_DEBUG("[enc_isr]  */
		/* VAL_DRIVER_TYPE_HEVC_ENC %d!!\n", gu4HwVencIrqStatus);*/

		gu4HwVencIrqStatus = VDO_HW_READ(KVA_VENC_IRQ_STATUS_ADDR);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_PAUSE)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_PAUSE);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_SWITCH)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_SWITCH);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_DRAM)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_DRAM);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_SPS)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_SPS);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_PPS)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_PPS);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_FRM)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_FRM);
		if (gu4HwVencIrqStatus & VENC_IRQ_STATUS_VPS)
			VDO_HW_WRITE(KVA_VENC_IRQ_ACK_ADDR,
				     VENC_IRQ_STATUS_VPS);
	}
#endif
	else
		MODULE_MFV_PR_DEBUG(
			"%s Invalid lock holder driver type = %d\n",
			__func__, grVcodecEncHWLock.eDriverType);

	eValRet = eVideoSetEvent(&EncIsrEvent, sizeof(struct VAL_EVENT_T));
	if (eValRet != VAL_RESULT_NO_ERROR) {
		/* Add one line comment for avoid kernel coding style,*/
		/* WARNING:BRACES: */
		MODULE_MFV_PR_DEBUG(
			"[VCODEC][ERROR] ISR set EncIsrEvent error\n");
	}
	MODULE_MFV_PR_DEBUG("[MFV] %s ISR set EncIsrEvent done\n",
						__func__);
}

static irqreturn_t video_intr_dlr(int irq, void *dev_id)
{
	dec_isr();
	return IRQ_HANDLED;
}

static irqreturn_t video_intr_dlr2(int irq, void *dev_id)
{
	enc_isr();
	return IRQ_HANDLED;
}

static long vcodec_lockhw_dec_fail(struct VAL_HW_LOCK_T rHWLock,
				   unsigned int FirstUseDecHW)
{
	MODULE_MFV_PR_DEBUG(
		"[ERROR] VCODEC_LOCKHW, DecHWLockEvent TimeOut, CurrentTID = %d\n",
		current->pid);
	if (FirstUseDecHW != 1) {
		mutex_lock(&VdecHWLock);
		if (grVcodecDecHWLock.pvHandle == 0) {
			/* Add one line comment for avoid kernel coding */
			/* style, WARNING:BRACES: */
			MODULE_MFV_PR_DEBUG(
				"[WARNING] VCODEC_LOCKHW, maybe mediaserver restart before, please check!!\n");
		} else {
			/* Add one line comment for avoid kernel */
			/* coding style, WARNING:BRACES: */
			MODULE_MFV_PR_DEBUG(
				"[WARNING] VCODEC_LOCKHW, someone use HW, and check timeout value!!\n");
		}
		mutex_unlock(&VdecHWLock);
	}

	return 0;
}

static long vcodec_lockhw_enc_fail(struct VAL_HW_LOCK_T rHWLock,
				   unsigned int FirstUseEncHW)
{
	MODULE_MFV_PR_DEBUG(
		"[ERROR] VCODEC_LOCKHW EncHWLockEvent TimeOut, CurrentTID = %d\n",
		current->pid);

	if (FirstUseEncHW != 1) {
		mutex_lock(&VencHWLock);
		if (grVcodecEncHWLock.pvHandle == 0) {
			MODULE_MFV_PR_DEBUG(
				"[WARNING] VCODEC_LOCKHW, maybe mediaserver restart before, please check!!\n");
		} else {
			MODULE_MFV_PR_DEBUG(
				"[WARNING] VCODEC_LOCKHW, someone use HW, and check timeout value!! %d\n",
				gLockTimeOutCount);
			++gLockTimeOutCount;
			if (gLockTimeOutCount > 30) {
				MODULE_MFV_PR_DEBUG(
					"[ERROR] VCODEC_LOCKHW - ID %d fail\n",
					current->pid);
				MODULE_MFV_PR_DEBUG(
		"someone locked HW time out more than 30 times 0x%lx,%lx,0x%lx,type:%d\n",
		(unsigned long)grVcodecEncHWLock.pvHandle,
		pmem_user_v2p_video(
		(unsigned long)rHWLock.pvHandle),
		(unsigned long)rHWLock.pvHandle,
		rHWLock.eDriverType);
				gLockTimeOutCount = 0;
				mutex_unlock(&VencHWLock);
				return -EFAULT;
			}

			if (rHWLock.u4TimeoutMs == 0) {
				MODULE_MFV_PR_DEBUG(
					"[ERROR] VCODEC_LOCKHW - ID %d fail\n",
					current->pid);
				MODULE_MFV_PR_DEBUG(
					"someone locked HW already 0x%lx,%lx,0x%lx,type:%d\n",
				(unsigned long)grVcodecEncHWLock.pvHandle,
					pmem_user_v2p_video(
				(unsigned long)rHWLock.pvHandle),
					(unsigned long)rHWLock.pvHandle,
					rHWLock.eDriverType);
				gLockTimeOutCount = 0;
				mutex_unlock(&VencHWLock);
				return -EFAULT;
			}
		}
		mutex_unlock(&VencHWLock);
	}

	return 0;
}

static long vcodec_lockhw(unsigned long arg)
{
	unsigned char *user_data_addr;
	struct VAL_HW_LOCK_T rHWLock;
	enum VAL_RESULT_T eValRet;
	signed long ret;
	char bLockedHW = VAL_FALSE;
	unsigned int FirstUseDecHW = 0;
	unsigned int FirstUseEncHW = 0;
	struct VAL_TIME_T rCurTime;
	unsigned int u4TimeInterval;
	unsigned long ulFlagsLockHW;

	MODULE_MFV_PR_DEBUG("VCODEC_LOCKHW + tid = %d\n", current->pid);

	user_data_addr = (unsigned char *)arg;
	ret = copy_from_user(&rHWLock,
	user_data_addr, sizeof(struct VAL_HW_LOCK_T));
	if (ret) {
		MODULE_MFV_PR_DEBUG(
			"[ERROR] VCODEC_LOCKHW, copy_from_user failed: %lu\n",
			ret);
		return -EFAULT;
	}

	MODULE_MFV_PR_DEBUG("[VCODEC] LOCKHW eDriverType = %d\n",
			    rHWLock.eDriverType);
	eValRet = VAL_RESULT_INVALID_ISR;
	if (rHWLock.eDriverType == VAL_DRIVER_TYPE_MP4_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_HEVC_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_H264_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_MP1_MP2_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_VC1_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_VC1_ADV_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_VP8_DEC) {
		while (bLockedHW == VAL_FALSE) {
			mutex_lock(&DecHWLockEventTimeoutLock);
			if (DecHWLockEvent.u4TimeoutMs == 1) {
				MODULE_MFV_PR_DEBUG(
					"VCODEC_LOCKHW, First Use Dec HW!!\n");
				FirstUseDecHW = 1;
			} else {
				FirstUseDecHW = 0;
			}
			mutex_unlock(&DecHWLockEventTimeoutLock);

			if (FirstUseDecHW == 1) {
				/* Add one line comment for avoid kernel */
				/* coding style, WARNING:BRACES: */
				eValRet = eVideoWaitEvent(&DecHWLockEvent,
			sizeof(struct VAL_EVENT_T));
			}
			mutex_lock(&DecHWLockEventTimeoutLock);
			if (DecHWLockEvent.u4TimeoutMs != 1000) {
				DecHWLockEvent.u4TimeoutMs = 1000;
				FirstUseDecHW = 1;
			} else {
				FirstUseDecHW = 0;
			}
			mutex_unlock(&DecHWLockEventTimeoutLock);

			mutex_lock(&VdecHWLock);
			/* one process try to lock twice */
			if (grVcodecDecHWLock.pvHandle ==
			    (void *)pmem_user_v2p_video(
				    (unsigned long)rHWLock.pvHandle)) {
				MODULE_MFV_PR_DEBUG(
					"[WARNING] VCODEC_LOCKHW, one decoder instance try to lock twice\n");
				MODULE_MFV_PR_DEBUG(
			"may cause lock HW timeout!! instance = 0x%lx, CurrentTID = %d\n",
			(unsigned long)grVcodecDecHWLock.pvHandle,
			current->pid);
			}
			mutex_unlock(&VdecHWLock);

			if (FirstUseDecHW == 0) {
				MODULE_MFV_PR_DEBUG(
					"VCODEC_LOCKHW, Not first time use HW, timeout = %d\n",
					DecHWLockEvent.u4TimeoutMs);
				eValRet = eVideoWaitEvent(&DecHWLockEvent,
				sizeof(struct VAL_EVENT_T));
			}

			if (eValRet == VAL_RESULT_INVALID_ISR) {
				ret = vcodec_lockhw_dec_fail(rHWLock,
							     FirstUseDecHW);
				if (ret) {
					MODULE_MFV_PR_DEBUG(
						"[ERROR] vcodec_lockhw_dec_fail failed: %lu\n",
						ret);
					return -EFAULT;
				}
			} else if (eValRet == VAL_RESULT_RESTARTSYS) {
				MODULE_MFV_PR_DEBUG(
					"[WARNING] VCODEC_LOCKHW, VAL_RESULT_RESTARTSYS return when HWLock!!\n");
				return -ERESTARTSYS;
			}

			mutex_lock(&VdecHWLock);
			if (grVcodecDecHWLock.pvHandle ==
			    0) { /* No one holds dec hw lock now */
				gu4VdecLockThreadId = current->pid;
				grVcodecDecHWLock.pvHandle =
					(void *)pmem_user_v2p_video(
				(unsigned long)rHWLock.pvHandle);
				grVcodecDecHWLock.eDriverType =
					rHWLock.eDriverType;
				eVideoGetTimeOfDay(
					&grVcodecDecHWLock.rLockedTime,
					sizeof(struct VAL_TIME_T));

				MODULE_MFV_PR_DEBUG(
					"VCODEC_LOCKHW, No process use dec HW, so current process can use HW\n");
				MODULE_MFV_PR_DEBUG(
			"LockInstance = 0x%lx CurrentTID = %d, rLockedTime(s, us) = %d, %d\n",
			(unsigned long)grVcodecDecHWLock.pvHandle,
			current->pid,
			grVcodecDecHWLock.rLockedTime.u4Sec,
			grVcodecDecHWLock.rLockedTime.u4uSec);

				bLockedHW = VAL_TRUE;
				if (eValRet == VAL_RESULT_INVALID_ISR &&
				    FirstUseDecHW != 1) {
					MODULE_MFV_PR_DEBUG(
						"[WARNING] VCODEC_LOCKHW, reset power/irq when HWLock!!\n");
#ifndef KS_POWER_WORKAROUND
					vdec_power_off();
#endif
					disable_irq(VDEC_IRQ_ID);
				}
#ifndef KS_POWER_WORKAROUND
				vdec_power_on();
#endif
#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT /* Morris Yang moved to TEE */
				if (rHWLock.bSecureInst == VAL_FALSE) {
					if (request_irq(VDEC_IRQ_ID,
							(irq_handler_t)
								video_intr_dlr,
							IRQF_TRIGGER_LOW,
							VCODEC_DEVNAME,
							NULL) < 0)
						MODULE_MFV_PR_DEBUG(
							"[VCODEC_DEBUG][ERROR] error to request dec irq\n");
					else
						MODULE_MFV_PR_DEBUG(
							"[VCODEC_DEBUG] success to request dec irq\n");
				}
#else
				enable_irq(VDEC_IRQ_ID);
#endif
			} else { /* Another one holding dec hw now */
				MODULE_MFV_PR_DEBUG("VCODEC_LOCKHW E\n");
				eVideoGetTimeOfDay(&rCurTime,
						   sizeof(struct VAL_TIME_T));
				u4TimeInterval =
					(((((rCurTime.u4Sec -
					     grVcodecDecHWLock.rLockedTime
						     .u4Sec) *
					    1000000) +
					   rCurTime.u4uSec) -
					  grVcodecDecHWLock.rLockedTime
						  .u4uSec) /
					 1000);

				MODULE_MFV_PR_DEBUG(
					"VCODEC_LOCKHW, someone use dec HW, and check timeout value\n");
				MODULE_MFV_PR_DEBUG(
					"TimeInterval(ms) = %d, TimeOutValue(ms)) = %d\n",
					u4TimeInterval, rHWLock.u4TimeoutMs);
				MODULE_MFV_PR_DEBUG(
			"Lock Instance = 0x%lx, Lock TID = %d, CurrentTID = %d\n",
			(unsigned long)grVcodecDecHWLock.pvHandle,
			gu4VdecLockThreadId, current->pid);
				MODULE_MFV_PR_DEBUG(
					"rLockedTime(%d s, %d us), rCurTime(%d s, %d us)\n",
					grVcodecDecHWLock.rLockedTime.u4Sec,
					grVcodecDecHWLock.rLockedTime.u4uSec,
					rCurTime.u4Sec, rCurTime.u4uSec);

				/* 2012/12/16. Cheng-Jung Never */
				/* steal hardware lock */
			}
			mutex_unlock(&VdecHWLock);
			spin_lock_irqsave(&LockDecHWCountLock, ulFlagsLockHW);
			gu4LockDecHWCount++;
			spin_unlock_irqrestore(&LockDecHWCountLock,
					       ulFlagsLockHW);
		}
	} else if (rHWLock.eDriverType == VAL_DRIVER_TYPE_H264_ENC ||
		   rHWLock.eDriverType == VAL_DRIVER_TYPE_HEVC_ENC ||
		   rHWLock.eDriverType == VAL_DRIVER_TYPE_JPEG_ENC) {
		/* TODO: for hybrid */
		/* LQ-TO-DO   hardware */
		while (bLockedHW == VAL_FALSE) {
			/* Early break for JPEG VENC */
			if (rHWLock.u4TimeoutMs == 0) {
				if (grVcodecEncHWLock.pvHandle != 0) {
					/* Add one line comment for avoid */
					/* kernel coding style, */
					/*WARNING:BRACES: */
					break;
				}
			}

			/* Wait to acquire Enc HW lock */
			mutex_lock(&EncHWLockEventTimeoutLock);
			if (EncHWLockEvent.u4TimeoutMs == 1) {
				MODULE_MFV_PR_DEBUG(
					"VCODEC_LOCKHW, First Use Enc HW %d!!\n",
					rHWLock.eDriverType);
				FirstUseEncHW = 1;
			} else {
				FirstUseEncHW = 0;
			}
			mutex_unlock(&EncHWLockEventTimeoutLock);
			if (FirstUseEncHW == 1) {
				/* Add one line comment for avoid kernel */
				/* coding style, WARNING:BRACES: */
				eValRet = eVideoWaitEvent(&EncHWLockEvent,
				sizeof(struct VAL_EVENT_T));
			}

			mutex_lock(&EncHWLockEventTimeoutLock);
			if (EncHWLockEvent.u4TimeoutMs == 1) {
				EncHWLockEvent.u4TimeoutMs = 1000;
				FirstUseEncHW = 1;
			} else {
				FirstUseEncHW = 0;
				if (rHWLock.u4TimeoutMs == 0) {
					/* Add one line comment for avoid */
					/* kernel coding style, */
					/* WARNING:BRACES: */
					EncHWLockEvent.u4TimeoutMs =
						0; /* No wait */
				} else {
					EncHWLockEvent.u4TimeoutMs =
						1000; /* Wait indefinitely */
				}
			}
			mutex_unlock(&EncHWLockEventTimeoutLock);

			mutex_lock(&VencHWLock);
			/* one process try to lock twice */
			if (grVcodecEncHWLock.pvHandle ==
			    (void *)pmem_user_v2p_video(
				    (unsigned long)rHWLock.pvHandle)) {
				MODULE_MFV_PR_DEBUG(
					"[WARNING] VCODEC_LOCKHW, one encoder instance try to lock twice\n");
				MODULE_MFV_PR_DEBUG(
			"may cause lock HW timeout!! instance=0x%lx, CurrentTID=%d, type:%d\n",
			(unsigned long)grVcodecEncHWLock.pvHandle,
			current->pid, rHWLock.eDriverType);
			}
			mutex_unlock(&VencHWLock);

			if (FirstUseEncHW == 0) {
				/* Add one line comment for avoid kernel */
				/* coding style, WARNING:BRACES: */
				eValRet = eVideoWaitEvent(&EncHWLockEvent,
				sizeof(struct VAL_EVENT_T));
			}

			if (eValRet == VAL_RESULT_INVALID_ISR) {
				ret = vcodec_lockhw_enc_fail(rHWLock,
							     FirstUseEncHW);
				if (ret) {
					MODULE_MFV_PR_DEBUG(
						"[ERROR] vcodec_lockhw_enc_fail failed: %lu\n",
						ret);
					return -EFAULT;
				}
			} else if (eValRet == VAL_RESULT_RESTARTSYS) {
				return -ERESTARTSYS;
			}

			mutex_lock(&VencHWLock);
			if (grVcodecEncHWLock.pvHandle ==
			    0) { /* No process use HW, so current process */
				    /* can use HW */
				if (rHWLock.eDriverType ==
					    VAL_DRIVER_TYPE_H264_ENC ||
				    rHWLock.eDriverType ==
					    VAL_DRIVER_TYPE_HEVC_ENC ||
				    rHWLock.eDriverType ==
					    VAL_DRIVER_TYPE_JPEG_ENC) {
					grVcodecEncHWLock
						.pvHandle = (void *)
						pmem_user_v2p_video(
							(unsigned long)rHWLock
								.pvHandle);
					grVcodecEncHWLock.eDriverType =
						rHWLock.eDriverType;
					eVideoGetTimeOfDay(
						&grVcodecEncHWLock.rLockedTime,
						sizeof(struct VAL_TIME_T));

					MODULE_MFV_PR_DEBUG(
						"VCODEC_LOCKHW, No process use HW, so current process can use HW\n");
					MODULE_MFV_PR_DEBUG(
						"VCODEC_LOCKHW, handle = 0x%lx\n",
						(unsigned long)grVcodecEncHWLock
							.pvHandle);
					MODULE_MFV_PR_DEBUG(
						"LockInstance = 0x%lx CurrentTID = %d, rLockedTime(s, us) = %d, %d\n",
						(unsigned long)grVcodecEncHWLock
							.pvHandle,
						current->pid,
						grVcodecEncHWLock.rLockedTime
							.u4Sec,
						grVcodecEncHWLock.rLockedTime
							.u4uSec);

					bLockedHW = VAL_TRUE;
					if (rHWLock.eDriverType ==
						    VAL_DRIVER_TYPE_H264_ENC ||
					    rHWLock.eDriverType ==
						    VAL_DRIVER_TYPE_HEVC_ENC) {
						venc_enableIRQ(&rHWLock);
					}
				}
			} else { /* someone use HW, and check timeout value */
				if (rHWLock.u4TimeoutMs == 0) {
					bLockedHW = VAL_FALSE;
					mutex_unlock(&VencHWLock);
					break;
				}

				eVideoGetTimeOfDay(&rCurTime,
						   sizeof(struct VAL_TIME_T));
				u4TimeInterval =
					(((((rCurTime.u4Sec -
					     grVcodecEncHWLock.rLockedTime
						     .u4Sec) *
					    1000000) +
					   rCurTime.u4uSec) -
					  grVcodecEncHWLock.rLockedTime
						  .u4uSec) /
					 1000);

				MODULE_MFV_PR_DEBUG(
					"VCODEC_LOCKHW, someone use enc HW, and check timeout value\n");
				MODULE_MFV_PR_DEBUG(
					"TimeInterval(ms) = %d, TimeOutValue(ms) = %d\n",
					u4TimeInterval, rHWLock.u4TimeoutMs);
				MODULE_MFV_PR_DEBUG(
					"rLockedTime(s, us) = %d, %d, rCurTime(s, us) = %d, %d\n",
					grVcodecEncHWLock.rLockedTime.u4Sec,
					grVcodecEncHWLock.rLockedTime.u4uSec,
					rCurTime.u4Sec, rCurTime.u4uSec);
				MODULE_MFV_PR_DEBUG(
					"LockInstance = 0x%lx, CurrentInstance = 0x%lx, CurrentTID = %d\n",
			(unsigned long)grVcodecEncHWLock.pvHandle,
			pmem_user_v2p_video(
			(unsigned long)rHWLock.pvHandle),
			current->pid);

				++gLockTimeOutCount;
				if (gLockTimeOutCount > 30) {
					MODULE_MFV_PR_DEBUG(
						"[ERROR] VCODEC_LOCKHW %d fail,someone locked HW over 30 times\n",
						current->pid);
					MODULE_MFV_PR_DEBUG(
						"without timeout 0x%lx,%lx,0x%lx,type:%d\n",
						(unsigned long)grVcodecEncHWLock
							.pvHandle,
						pmem_user_v2p_video(
							(unsigned long)rHWLock
								.pvHandle),
						(unsigned long)rHWLock.pvHandle,
						rHWLock.eDriverType);
					gLockTimeOutCount = 0;
					mutex_unlock(&VencHWLock);
					return -EFAULT;
				}

				/* 2013/04/10. Cheng-Jung Never steal */
				/* hardware lock */
			}

			if (bLockedHW == VAL_TRUE) {
				MODULE_MFV_PR_DEBUG(
			"VCODEC_LOCKHW, Lock ok grVcodecEncHWLock.pvHandle = 0x%lx, va:%lx, type:%d\n",
			(unsigned long)grVcodecEncHWLock.pvHandle,
			(unsigned long)rHWLock.pvHandle,
			rHWLock.eDriverType);
			gLockTimeOutCount = 0;
			}
			mutex_unlock(&VencHWLock);
		}

		if (bLockedHW == VAL_FALSE) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_LOCKHW %d fail,someone locked HW already,0x%lx,%lx,0x%lx,type:%d\n",
				current->pid,
				(unsigned long)grVcodecEncHWLock.pvHandle,
				pmem_user_v2p_video(
					(unsigned long)rHWLock.pvHandle),
				(unsigned long)rHWLock.pvHandle,
				rHWLock.eDriverType);
			gLockTimeOutCount = 0;
			return -EFAULT;
		}

		spin_lock_irqsave(&LockEncHWCountLock, ulFlagsLockHW);
		gu4LockEncHWCount++;
		spin_unlock_irqrestore(&LockEncHWCountLock, ulFlagsLockHW);

		MODULE_MFV_PR_DEBUG("VCODEC_LOCKHW, get locked - ObjId =%d\n",
				    current->pid);

		MODULE_MFV_PR_DEBUG("VCODEC_LOCKHWed - tid = %d\n",
				    current->pid);
	} else {
		MODULE_MFV_PR_DEBUG("[WARNING] VCODEC_LOCKHW Unknown instance\n");
		return -EFAULT;
	}

	MODULE_MFV_PR_DEBUG("VCODEC_LOCKHW - tid = %d\n", current->pid);

	return 0;
}

static long vcodec_unlockhw(unsigned long arg)
{
	unsigned char *user_data_addr;
	struct VAL_HW_LOCK_T rHWLock;
	enum VAL_RESULT_T eValRet;
	signed long ret;

	MODULE_MFV_PR_DEBUG("VCODEC_UNLOCKHW + tid = %d\n", current->pid);

	user_data_addr = (unsigned char *)arg;
	ret = copy_from_user(&rHWLock,
		user_data_addr, sizeof(struct VAL_HW_LOCK_T));
	if (ret) {
		MODULE_MFV_PR_DEBUG(
			"[ERROR] VCODEC_UNLOCKHW, copy_from_user failed: %lu\n",
			ret);
		return -EFAULT;
	}

	MODULE_MFV_PR_DEBUG("VCODEC_UNLOCKHW eDriverType = %d\n",
			    rHWLock.eDriverType);
	eValRet = VAL_RESULT_INVALID_ISR;
	if (rHWLock.eDriverType == VAL_DRIVER_TYPE_MP4_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_HEVC_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_H264_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_MP1_MP2_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_VC1_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_VC1_ADV_DEC ||
	    rHWLock.eDriverType == VAL_DRIVER_TYPE_VP8_DEC) {
		mutex_lock(&VdecHWLock);
		/* Current owner give up hw lock */
		if (grVcodecDecHWLock.pvHandle ==
		    (void *)pmem_user_v2p_video(
			    (unsigned long)rHWLock.pvHandle)) {
			grVcodecDecHWLock.pvHandle = 0;
			grVcodecDecHWLock.eDriverType = VAL_DRIVER_TYPE_NONE;
#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT /* Morris Yang moved to TEE */
			if (rHWLock.bSecureInst == VAL_FALSE)
				free_irq(VDEC_IRQ_ID, NULL);
#else
			disable_irq(VDEC_IRQ_ID);
#endif
/* TODO: check if turning power off is ok */
#ifndef KS_POWER_WORKAROUND
			vdec_power_off();
#endif
		} else { /* Not current owner */
			MODULE_MFV_PR_DEBUG("[ERROR] VCODEC_UNLOCKHW\n");
			MODULE_MFV_PR_DEBUG(
				"Not owner trying to unlock dec hardware 0x%lx\n",
				pmem_user_v2p_video(
					(unsigned long)rHWLock.pvHandle));
			mutex_unlock(&VdecHWLock);
			return -EFAULT;
		}
		mutex_unlock(&VdecHWLock);
		eValRet = eVideoSetEvent(&DecHWLockEvent,
			sizeof(struct VAL_EVENT_T));
	} else if (rHWLock.eDriverType == VAL_DRIVER_TYPE_H264_ENC ||
		   rHWLock.eDriverType == VAL_DRIVER_TYPE_HEVC_ENC ||
		   rHWLock.eDriverType == VAL_DRIVER_TYPE_JPEG_ENC) {
		/* TODO: for hybrid */
		/* LQ-TO-DO   hardware */
		mutex_lock(&VencHWLock);
		/* Current owner give up hw lock */
		if (grVcodecEncHWLock.pvHandle ==
		    (void *)pmem_user_v2p_video(
			    (unsigned long)rHWLock.pvHandle)) {
			grVcodecEncHWLock.pvHandle = 0;
			grVcodecEncHWLock.eDriverType = VAL_DRIVER_TYPE_NONE;
			if (rHWLock.eDriverType == VAL_DRIVER_TYPE_H264_ENC ||
			    rHWLock.eDriverType == VAL_DRIVER_TYPE_HEVC_ENC) {
				/* turn venc power off */
				venc_disableIRQ(&rHWLock);
			}
		} else { /* Not current owner */
			/* [TODO] error handling */
			MODULE_MFV_PR_DEBUG("[ERROR] VCODEC_UNLOCKHW\n");
			MODULE_MFV_PR_DEBUG(
				"Not owner trying to unlock enc hardware 0x%lx, pa:%lx, va:%lx type:%d\n",
				(unsigned long)grVcodecEncHWLock.pvHandle,
				pmem_user_v2p_video(
					(unsigned long)rHWLock.pvHandle),
				(unsigned long)rHWLock.pvHandle,
				rHWLock.eDriverType);
			mutex_unlock(&VencHWLock);
			return -EFAULT;
		}
		mutex_unlock(&VencHWLock);
		eValRet = eVideoSetEvent(&EncHWLockEvent,
			sizeof(struct VAL_EVENT_T));
	} else {
		MODULE_MFV_PR_DEBUG(
			"[WARNING] VCODEC_UNLOCKHW Unknown instance\n");
		return -EFAULT;
	}

	MODULE_MFV_PR_DEBUG("VCODEC_UNLOCKHW - tid = %d\n", current->pid);

	return 0;
}

static long vcodec_waitisr(unsigned long arg)
{
	unsigned char *user_data_addr;
	struct VAL_ISR_T val_isr;
	char bLockedHW = VAL_FALSE;
	unsigned long ulFlags;
	signed long ret;
	enum VAL_RESULT_T eValRet;

	MODULE_MFV_PR_DEBUG("VCODEC_WAITISR + tid = %d\n", current->pid);

	user_data_addr = (unsigned char *)arg;
	ret = copy_from_user(&val_isr,
		user_data_addr, sizeof(struct VAL_ISR_T));
	if (ret) {
		MODULE_MFV_PR_DEBUG(
			"[ERROR] VCODEC_WAITISR, copy_from_user failed: %lu\n",
			ret);
		return -EFAULT;
	}

	if (val_isr.eDriverType == VAL_DRIVER_TYPE_MP4_DEC ||
	    val_isr.eDriverType == VAL_DRIVER_TYPE_HEVC_DEC ||
	    val_isr.eDriverType == VAL_DRIVER_TYPE_H264_DEC ||
	    val_isr.eDriverType == VAL_DRIVER_TYPE_MP1_MP2_DEC ||
	    val_isr.eDriverType == VAL_DRIVER_TYPE_VC1_DEC ||
	    val_isr.eDriverType == VAL_DRIVER_TYPE_VC1_ADV_DEC ||
	    val_isr.eDriverType == VAL_DRIVER_TYPE_VP8_DEC) {
		mutex_lock(&VdecHWLock);
		if (grVcodecDecHWLock.pvHandle ==
		    (void *)pmem_user_v2p_video(
			    (unsigned long)val_isr.pvHandle)) {
			/* Add one line comment for avoid kernel coding */
			/* style, WARNING:BRACES: */
			bLockedHW = VAL_TRUE;
		} else {
		}
		mutex_unlock(&VdecHWLock);

		if (bLockedHW == VAL_FALSE) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_WAITISR, DO NOT have HWLock, so return fail\n");
			return -EFAULT;
		}

		spin_lock_irqsave(&DecIsrLock, ulFlags);
		DecIsrEvent.u4TimeoutMs = val_isr.u4TimeoutMs;
		spin_unlock_irqrestore(&DecIsrLock, ulFlags);

		eValRet = eVideoWaitEvent(&DecIsrEvent,
			sizeof(struct VAL_EVENT_T));
		if (eValRet == VAL_RESULT_INVALID_ISR) {
			return -2;
		} else if (eValRet == VAL_RESULT_RESTARTSYS) {
			MODULE_MFV_PR_DEBUG(
				"[WARNING] VCODEC_WAITISR, VAL_RESULT_RESTARTSYS return when WAITISR!!\n");
			return -ERESTARTSYS;
		}
	} else if (val_isr.eDriverType == VAL_DRIVER_TYPE_H264_ENC ||
		   val_isr.eDriverType == VAL_DRIVER_TYPE_HEVC_ENC) {
		mutex_lock(&VencHWLock);
		if (grVcodecEncHWLock.pvHandle ==
		    (void *)pmem_user_v2p_video(
			    (unsigned long)val_isr.pvHandle)) {
			/* Add one line comment for avoid kernel  */
			/* coding style, WARNING:BRACES: */
			bLockedHW = VAL_TRUE;
		} else {
		}
		mutex_unlock(&VencHWLock);

		if (bLockedHW == VAL_FALSE) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_WAITISR, DO NOT have enc HWLock, so return fail pa:%lx, va:%lx\n",
				pmem_user_v2p_video(
					(unsigned long)val_isr.pvHandle),
				(unsigned long)val_isr.pvHandle);
			return -EFAULT;
		}

		spin_lock_irqsave(&EncIsrLock, ulFlags);
		EncIsrEvent.u4TimeoutMs = val_isr.u4TimeoutMs;
		spin_unlock_irqrestore(&EncIsrLock, ulFlags);

		eValRet = eVideoWaitEvent(&EncIsrEvent,
			sizeof(struct VAL_EVENT_T));
		if (eValRet == VAL_RESULT_INVALID_ISR) {
			return -2;
		} else if (eValRet == VAL_RESULT_RESTARTSYS) {
			MODULE_MFV_PR_DEBUG(
				"[WARNING] VCODEC_WAITISR, VAL_RESULT_RESTARTSYS return when WAITISR!!\n");
			return -ERESTARTSYS;
		}

		if (val_isr.u4IrqStatusNum > 0) {
			val_isr.u4IrqStatus[0] = gu4HwVencIrqStatus;
			ret = copy_to_user(user_data_addr, &val_isr,
					   sizeof(struct VAL_ISR_T));
			if (ret) {
				MODULE_MFV_PR_DEBUG(
					"[ERROR] VCODEC_WAITISR, copy_to_user failed: %lu\n",
					ret);
				return -EFAULT;
			}
		}
	} else {
		MODULE_MFV_PR_DEBUG(
			"[WARNING] VCODEC_WAITISR Unknown instance\n");
		return -EFAULT;
	}
	MODULE_MFV_PR_DEBUG("VCODEC_WAITISR - tid = %d\n", current->pid);

	return 0;
}

static long vcodec_unlocked_ioctl(struct file *file, unsigned int cmd,
				  unsigned long arg)
{
	signed long ret;
	unsigned char *user_data_addr;
	struct VAL_VCODEC_CORE_LOADING_T rTempCoreLoading;
	struct VAL_VCODEC_CPU_OPP_LIMIT_T rCpuOppLimit;
	signed int temp_nr_cpu_ids;
	struct VAL_POWER_T rPowerParam;
/*char rIncLogCount;*/

	switch (cmd) {
	case VCODEC_SET_THREAD_ID: {
		/* MODULE_MFV_PR_DEBUG("VCODEC_SET_THREAD_ID [EMPTY] + */
		/* tid = %d\n", current->pid); */
		/* MODULE_MFV_PR_DEBUG("VCODEC_SET_THREAD_ID [EMPTY] - */
		/* tid = %d\n", current->pid); */
	} break;

	case VCODEC_ALLOC_NON_CACHE_BUFFER: {
		/* MODULE_MFV_LOGE("VCODEC_ALLOC_NON_CACHE_BUFFER [EMPTY] */
		/* + tid = %d\n", current->pid); */
	} break;

	case VCODEC_FREE_NON_CACHE_BUFFER: {
		/* MODULE_MFV_LOGE("VCODEC_FREE_NON_CACHE_BUFFER [EMPTY] +*/
		/*  tid = %d\n", current->pid); */
	} break;

	case VCODEC_INC_DEC_EMI_USER: {
		MODULE_MFV_PR_DEBUG("VCODEC_INC_DEC_EMI_USER + tid = %d\n",
				    current->pid);

		mutex_lock(&DecEMILock);
		gu4DecEMICounter++;
		/* if (gu4DecEMICounter == 1) {*/
		/*	  SendDvfsRequest(DVFS_LOW_LOW);*/
		/* }*/
		MODULE_MFV_PR_DEBUG("[VCODEC] DEC_EMI_USER = %d\n",
				    gu4DecEMICounter);
		user_data_addr = (unsigned char *)arg;
		ret = copy_to_user(user_data_addr, &gu4DecEMICounter,
				   sizeof(unsigned int));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_INC_DEC_EMI_USER, copy_to_user failed: %lu\n",
				ret);
			mutex_unlock(&DecEMILock);
			return -EFAULT;
		}
		mutex_unlock(&DecEMILock);

		MODULE_MFV_PR_DEBUG("VCODEC_INC_DEC_EMI_USER - tid = %d\n",
				    current->pid);
	} break;

	case VCODEC_DEC_DEC_EMI_USER: {
		MODULE_MFV_PR_DEBUG("VCODEC_DEC_DEC_EMI_USER + tid = %d\n",
				    current->pid);

		mutex_lock(&DecEMILock);
		gu4DecEMICounter--;
		/* if (gu4DecEMICounter == 0) {*/
		/*	* Restore to default when no decoder*/
		/*	* SendDvfsRequest(DVFS_DEFAULT);*/
		/*}*/
		MODULE_MFV_PR_DEBUG("[VCODEC] DEC_EMI_USER = %d\n",
				    gu4DecEMICounter);
		user_data_addr = (unsigned char *)arg;
		ret = copy_to_user(user_data_addr, &gu4DecEMICounter,
				   sizeof(unsigned int));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_DEC_DEC_EMI_USER, copy_to_user failed: %lu\n",
				ret);
			mutex_unlock(&DecEMILock);
			return -EFAULT;
		}
		mutex_unlock(&DecEMILock);

		MODULE_MFV_PR_DEBUG("VCODEC_DEC_DEC_EMI_USER - tid = %d\n",
				    current->pid);
	} break;

	case VCODEC_INC_ENC_EMI_USER: {
		MODULE_MFV_PR_DEBUG("VCODEC_INC_ENC_EMI_USER + tid = %d\n",
				    current->pid);

		mutex_lock(&EncEMILock);
		gu4EncEMICounter++;
		MODULE_MFV_PR_DEBUG("[VCODEC] ENC_EMI_USER = %d\n",
				    gu4EncEMICounter);
		user_data_addr = (unsigned char *)arg;
		ret = copy_to_user(user_data_addr, &gu4EncEMICounter,
				   sizeof(unsigned int));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_INC_ENC_EMI_USER, copy_to_user failed: %lu\n",
				ret);
			mutex_unlock(&EncEMILock);
			return -EFAULT;
		}
		mutex_unlock(&EncEMILock);

		MODULE_MFV_PR_DEBUG("VCODEC_INC_ENC_EMI_USER - tid = %d\n",
				    current->pid);
	} break;

	case VCODEC_DEC_ENC_EMI_USER: {
		MODULE_MFV_PR_DEBUG("VCODEC_DEC_ENC_EMI_USER + tid = %d\n",
				    current->pid);

		mutex_lock(&EncEMILock);
		gu4EncEMICounter--;
		MODULE_MFV_PR_DEBUG("[VCODEC] ENC_EMI_USER = %d\n",
				    gu4EncEMICounter);
		user_data_addr = (unsigned char *)arg;
		ret = copy_to_user(user_data_addr, &gu4EncEMICounter,
				   sizeof(unsigned int));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_DEC_ENC_EMI_USER, copy_to_user failed: %lu\n",
				ret);
			mutex_unlock(&EncEMILock);
			return -EFAULT;
		}
		mutex_unlock(&EncEMILock);

		MODULE_MFV_PR_DEBUG("VCODEC_DEC_ENC_EMI_USER - tid = %d\n",
				    current->pid);
	} break;

	case VCODEC_LOCKHW: {
		ret = vcodec_lockhw(arg);
		if (ret) {
			MODULE_MFV_PR_DEBUG("[ERROR] VCODEC_LOCKHW failed! %lu\n",
					  ret);
			return ret;
		}
	} break;

	case VCODEC_UNLOCKHW: {
		ret = vcodec_unlockhw(arg);
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_UNLOCKHW failed! %lu\n", ret);
			return ret;
		}
	} break;

	case VCODEC_INC_PWR_USER: {
		MODULE_MFV_PR_DEBUG("VCODEC_INC_PWR_USER + tid = %d\n",
				    current->pid);
		user_data_addr = (unsigned char *)arg;
		ret = copy_from_user(&rPowerParam, user_data_addr,
				     sizeof(struct VAL_POWER_T));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_INC_PWR_USER, copy_from_user failed: %lu\n",
				ret);
			return -EFAULT;
		}
		MODULE_MFV_PR_DEBUG("[VCODEC] INC_PWR_USER eDriverType = %d\n",
				    rPowerParam.eDriverType);
		mutex_lock(&L2CLock);

#ifdef VENC_USE_L2C
		if (rPowerParam.eDriverType == VAL_DRIVER_TYPE_H264_ENC) {
			gu4L2CCounter++;
			MODULE_MFV_PR_DEBUG(
				"[VCODEC] INC_PWR_USER L2C counter = %d\n",
				gu4L2CCounter);

			if (gu4L2CCounter == 1) {
				if (config_L2(0)) {
					MODULE_MFV_PR_DEBUG(
						"[VCODEC][ERROR] Switch L2C size to 512K failed\n");
					mutex_unlock(&L2CLock);
					return -EFAULT;
				}
				MODULE_MFV_PR_DEBUG(
					"[VCODEC] Switch L2C size to 512K successful\n");
			}
		}
#endif
		mutex_unlock(&L2CLock);
		MODULE_MFV_PR_DEBUG("VCODEC_INC_PWR_USER - tid = %d\n",
				    current->pid);
	} break;

	case VCODEC_DEC_PWR_USER: {
		MODULE_MFV_PR_DEBUG("VCODEC_DEC_PWR_USER + tid = %d\n",
				    current->pid);
		user_data_addr = (unsigned char *)arg;
		ret = copy_from_user(&rPowerParam, user_data_addr,
				     sizeof(struct VAL_POWER_T));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_DEC_PWR_USER, copy_from_user failed: %lu\n",
				ret);
			return -EFAULT;
		}
		MODULE_MFV_PR_DEBUG("[VCODEC] DEC_PWR_USER eDriverType = %d\n",
				    rPowerParam.eDriverType);

		mutex_lock(&L2CLock);

#ifdef VENC_USE_L2C
		if (rPowerParam.eDriverType == VAL_DRIVER_TYPE_H264_ENC) {
			gu4L2CCounter--;
			MODULE_MFV_PR_DEBUG(
				"[VCODEC] DEC_PWR_USER L2C counter  = %d\n",
				gu4L2CCounter);

			if (gu4L2CCounter == 0) {
				if (config_L2(1)) {
					MODULE_MFV_PR_DEBUG(
						"[VCODEC][ERROR] Switch L2C size to 0K failed\n");
					mutex_unlock(&L2CLock);
					return -EFAULT;
				}
				MODULE_MFV_PR_DEBUG(
					"[VCODEC] Switch L2C size to 0K successful\n");
			}
		}
#endif
		mutex_unlock(&L2CLock);
		MODULE_MFV_PR_DEBUG("VCODEC_DEC_PWR_USER - tid = %d\n",
				    current->pid);
	} break;

	case VCODEC_WAITISR: {
		ret = vcodec_waitisr(arg);
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_WAITISR failed! %lu\n", ret);
			return ret;
		}
	} break;

	case VCODEC_INITHWLOCK: {
		MODULE_MFV_PR_DEBUG("VCODEC_INITHWLOCK [EMPTY] + - tid = %d\n",
				    current->pid);
		MODULE_MFV_PR_DEBUG("VCODEC_INITHWLOCK [EMPTY] - - tid = %d\n",
				    current->pid);
	} break;

	case VCODEC_DEINITHWLOCK: {
		MODULE_MFV_PR_DEBUG(
			"VCODEC_DEINITHWLOCK [EMPTY] + - tid = %d\n",
			current->pid);
		MODULE_MFV_PR_DEBUG(
			"VCODEC_DEINITHWLOCK [EMPTY] - - tid = %d\n",
			current->pid);
	} break;

	case VCODEC_GET_CPU_LOADING_INFO: {
		unsigned char *user_data_addr;
		struct VAL_VCODEC_CPU_LOADING_INFO_T _temp = {0};

		MODULE_MFV_PR_DEBUG("VCODEC_GET_CPU_LOADING_INFO +\n");
		user_data_addr = (unsigned char *)arg;
		ret = copy_to_user(user_data_addr, &_temp,
			sizeof(struct VAL_VCODEC_CPU_LOADING_INFO_T));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_GET_CPU_LOADING_INFO, copy_to_user failed: %lu\n",
				ret);
			return -EFAULT;
		}

		MODULE_MFV_PR_DEBUG("VCODEC_GET_CPU_LOADING_INFO -\n");
	} break;

	case VCODEC_GET_CORE_LOADING: {
		MODULE_MFV_PR_DEBUG("VCODEC_GET_CORE_LOADING + - tid = %d\n",
				    current->pid);

		user_data_addr = (unsigned char *)arg;
		ret = copy_from_user(&rTempCoreLoading, user_data_addr,
				     sizeof(struct VAL_VCODEC_CORE_LOADING_T));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_GET_CORE_LOADING, copy_from_user failed: %lu\n",
				ret);
			return -EFAULT;
		}
		if (rTempCoreLoading.CPUid > num_possible_cpus()) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] rTempCoreLoading.CPUid(%d) > num_possible_cpus(%d)\n",
				rTempCoreLoading.CPUid, num_possible_cpus());
			return -EFAULT;
		}
		if (rTempCoreLoading.CPUid < 0) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] rTempCoreLoading.CPUid(%d) < 0\n",
				rTempCoreLoading.CPUid);
			return -EFAULT;
		}
		#ifdef CONFIG_MTK_SCHED_CPULOAD
		rTempCoreLoading.Loading =
		sched_get_cpu_load(rTempCoreLoading.CPUid);
		#else
		int rel_load, abs_load;
		int reset = 1;

		sched_get_percpu_load2(rTempCoreLoading.CPUid,
			 reset, &rel_load, &abs_load);
		rTempCoreLoading.Loading = rel_load;
		#endif
		ret = copy_to_user(user_data_addr, &rTempCoreLoading,
				   sizeof(struct VAL_VCODEC_CORE_LOADING_T));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_GET_CORE_LOADING, copy_to_user failed: %lu\n",
				ret);
			return -EFAULT;
		}
		MODULE_MFV_PR_DEBUG("VCODEC_GET_CORE_LOADING - - tid = %d\n",
				    current->pid);
	} break;

	case VCODEC_GET_CORE_NUMBER: {
		MODULE_MFV_PR_DEBUG("VCODEC_GET_CORE_NUMBER + - tid = %d\n",
				    current->pid);

		user_data_addr = (unsigned char *)arg;
		temp_nr_cpu_ids = nr_cpu_ids;
		ret = copy_to_user(user_data_addr, &temp_nr_cpu_ids,
				   sizeof(int));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_GET_CORE_NUMBER, copy_to_user failed: %lu\n",
				ret);
			return -EFAULT;
		}
		MODULE_MFV_PR_DEBUG("VCODEC_GET_CORE_NUMBER - - tid = %d\n",
				    current->pid);
	} break;

	case VCODEC_SET_CPU_OPP_LIMIT: {
		MODULE_MFV_PR_DEBUG(
			"VCODEC_SET_CPU_OPP_LIMIT [EMPTY] + - tid = %d\n",
			current->pid);
		user_data_addr = (unsigned char *)arg;
		ret = copy_from_user(&rCpuOppLimit, user_data_addr,
				     sizeof(struct VAL_VCODEC_CPU_OPP_LIMIT_T));
		if (ret) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] VCODEC_SET_CPU_OPP_LIMIT, copy_from_user failed: %lu\n",
				ret);
			return -EFAULT;
		}
		MODULE_MFV_PR_DEBUG(
			"+VCODEC_SET_CPU_OPP_LIMIT (%d, %d, %d), tid = %d\n",
			rCpuOppLimit.limited_freq, rCpuOppLimit.limited_cpu,
			rCpuOppLimit.enable, current->pid);
		MODULE_MFV_PR_DEBUG(
			"-VCODEC_SET_CPU_OPP_LIMIT tid = %d, ret = %lu\n",
			current->pid, ret);
		MODULE_MFV_PR_DEBUG(
			"VCODEC_SET_CPU_OPP_LIMIT [EMPTY] - - tid = %d\n",
			current->pid);
	} break;

	case VCODEC_MB: {
		/* Allow user of kernel driver to issue mb() */
		/* after register access*/
		mb();
	} break;
	default: {
		MODULE_MFV_PR_DEBUG(
			"========[ERROR] vcodec_ioctl default case======== %u\n",
			cmd);
	} break;
	}
	return 0xFF;
}

#if IS_ENABLED(CONFIG_COMPAT)

enum STRUCT_TYPE {
	VAL_HW_LOCK_TYPE = 0,
	VAL_POWER_TYPE,
	VAL_ISR_TYPE,
	VAL_MEMORY_TYPE
};

enum COPY_DIRECTION {
	COPY_FROM_USER = 0,
	COPY_TO_USER,
};

struct COMPAT_VAL_HW_LOCK_T {
	/* [IN]     The video codec driver handle */
	compat_uptr_t pvHandle;
	/* [IN]     The size of video codec driver handle */
	compat_uint_t u4HandleSize;
	/* [IN/OUT] The Lock discriptor */
	compat_uptr_t pvLock;
	/* [IN]     The timeout ms */
	compat_uint_t u4TimeoutMs;
	/* [IN/OUT] The reserved parameter */
	compat_uptr_t pvReserved;
	/* [IN]     The size of reserved parameter structure */
	compat_uint_t u4ReservedSize;
	/* [IN]     The driver type */
	compat_uint_t eDriverType;
	/* [IN]     True if this is a secure instance */
	/* MTK_SEC_VIDEO_PATH_SUPPORT */
	char bSecureInst;
};

struct COMPAT_VAL_POWER_T {
	/* [IN]     The video codec driver handle */
	compat_uptr_t pvHandle;
	/* [IN]     The size of video codec driver handle */
	compat_uint_t u4HandleSize;
	/* [IN]     The driver type */
	compat_uint_t eDriverType;
	/* [IN]     Enable or not. */
	char fgEnable;
	/* [IN/OUT] The reserved parameter */
	compat_uptr_t pvReserved;
	/* [IN]     The size of reserved parameter structure */
	compat_uint_t u4ReservedSize;
	/* [OUT]    The number of power user right now */
	/* unsigned int        u4L2CUser; */
};

struct COMPAT_VAL_ISR_T {
	/* [IN]     The video codec driver handle */
	compat_uptr_t pvHandle;
	/* [IN]     The size of video codec driver handle */
	compat_uint_t u4HandleSize;
	/* [IN]     The driver type */
	compat_uint_t eDriverType;
	/* [IN]     The isr function */
	compat_uptr_t pvIsrFunction;
	/* [IN/OUT] The reserved parameter */
	compat_uptr_t pvReserved;
	/* [IN]     The size of reserved parameter structure */
	compat_uint_t u4ReservedSize;
	/* [IN]     The timeout in ms */
	compat_uint_t u4TimeoutMs;
	/* [IN]     The num of return registers when HW done */
	compat_uint_t u4IrqStatusNum;
	/* [IN/OUT] The value of return registers when HW done */
	compat_uint_t u4IrqStatus[IRQ_STATUS_MAX_NUM];
};

struct COMPAT_VAL_MEMORY_T {
	/* [IN]     The allocation memory type */
	compat_uint_t eMemType;
	/* [IN]     The size of memory allocation */
	compat_ulong_t u4MemSize;
	/* [IN/OUT] The memory virtual address */
	compat_uptr_t pvMemVa;
	/* [IN/OUT] The memory physical address */
	compat_uptr_t pvMemPa;
	/* [IN]     The memory byte alignment setting */
	compat_uint_t eAlignment;
	/* [IN/OUT] The align memory virtual address */
	compat_uptr_t pvAlignMemVa;
	/* [IN/OUT] The align memory physical address */
	compat_uptr_t pvAlignMemPa;
	/* [IN]     The memory codec for VENC or VDEC */
	compat_uint_t eMemCodec;
	compat_uint_t i4IonShareFd;
	compat_uptr_t pIonBufhandle;
	/* [IN/OUT] The reserved parameter */
	compat_uptr_t pvReserved;
	/* [IN]     The size of reserved parameter structure */
	compat_ulong_t u4ReservedSize;
};

static int get_uptr_to_32(compat_uptr_t *p, void __user **uptr)
{
	void __user *p2p;
	int err = get_user(p2p, uptr);
	*p = ptr_to_compat(p2p);
	return err;
}
static int compat_copy_struct(
			enum STRUCT_TYPE eType,
			enum COPY_DIRECTION eDirection,
			void __user *data32,
			void __user *data)
{
	compat_uint_t u;
	compat_ulong_t l;
	compat_uptr_t p;
	char c;
	int err = 0;

	switch (eType) {
	case VAL_HW_LOCK_TYPE: {
		if (eDirection == COPY_FROM_USER) {
			struct COMPAT_VAL_HW_LOCK_T __user *from32 =
				(struct COMPAT_VAL_HW_LOCK_T *)data32;
			struct VAL_HW_LOCK_T __user *to =
				(struct VAL_HW_LOCK_T *)data;

			err = get_user(p, &(from32->pvHandle));
			err |= put_user(compat_ptr(p), &(to->pvHandle));
			err |= get_user(u, &(from32->u4HandleSize));
			err |= put_user(u, &(to->u4HandleSize));
			err |= get_user(p, &(from32->pvLock));
			err |= put_user(compat_ptr(p), &(to->pvLock));
			err |= get_user(u, &(from32->u4TimeoutMs));
			err |= put_user(u, &(to->u4TimeoutMs));
			err |= get_user(p, &(from32->pvReserved));
			err |= put_user(compat_ptr(p), &(to->pvReserved));
			err |= get_user(u, &(from32->u4ReservedSize));
			err |= put_user(u, &(to->u4ReservedSize));
			err |= get_user(u, &(from32->eDriverType));
			err |= put_user(u, &(to->eDriverType));
			err |= get_user(c, &(from32->bSecureInst));
			err |= put_user(c, &(to->bSecureInst));
		} else {
			struct COMPAT_VAL_HW_LOCK_T __user *to32 =
				(struct COMPAT_VAL_HW_LOCK_T *)data32;
			struct VAL_HW_LOCK_T __user *from =
				(struct VAL_HW_LOCK_T *)data;

			err = get_uptr_to_32(&p, &(from->pvHandle));
			err |= put_user(p, &(to32->pvHandle));
			err |= get_user(u, &(from->u4HandleSize));
			err |= put_user(u, &(to32->u4HandleSize));
			err |= get_uptr_to_32(&p, &(from->pvLock));
			err |= put_user(p, &(to32->pvLock));
			err |= get_user(u, &(from->u4TimeoutMs));
			err |= put_user(u, &(to32->u4TimeoutMs));
			err |= get_uptr_to_32(&p, &(from->pvReserved));
			err |= put_user(p, &(to32->pvReserved));
			err |= get_user(u, &(from->u4ReservedSize));
			err |= put_user(u, &(to32->u4ReservedSize));
			err |= get_user(u, &(from->eDriverType));
			err |= put_user(u, &(to32->eDriverType));
			err |= get_user(c, &(from->bSecureInst));
			err |= put_user(c, &(to32->bSecureInst));
		}
	} break;
	case VAL_POWER_TYPE: {
		if (eDirection == COPY_FROM_USER) {
			struct COMPAT_VAL_POWER_T __user *from32 =
				(struct COMPAT_VAL_POWER_T *)data32;
			struct VAL_POWER_T __user *to =
				(struct VAL_POWER_T *)data;

			err = get_user(p, &(from32->pvHandle));
			err |= put_user(compat_ptr(p), &(to->pvHandle));
			err |= get_user(u, &(from32->u4HandleSize));
			err |= put_user(u, &(to->u4HandleSize));
			err |= get_user(u, &(from32->eDriverType));
			err |= put_user(u, &(to->eDriverType));
			err |= get_user(c, &(from32->fgEnable));
			err |= put_user(c, &(to->fgEnable));
			err |= get_user(p, &(from32->pvReserved));
			err |= put_user(compat_ptr(p), &(to->pvReserved));
			err |= get_user(u, &(from32->u4ReservedSize));
			err |= put_user(u, &(to->u4ReservedSize));
		} else {
			struct COMPAT_VAL_POWER_T __user *to32 =
				(struct COMPAT_VAL_POWER_T *)data32;
			struct VAL_POWER_T __user *from =
				(struct VAL_POWER_T *)data;

			err = get_uptr_to_32(&p, &(from->pvHandle));
			err |= put_user(p, &(to32->pvHandle));
			err |= get_user(u, &(from->u4HandleSize));
			err |= put_user(u, &(to32->u4HandleSize));
			err |= get_user(u, &(from->eDriverType));
			err |= put_user(u, &(to32->eDriverType));
			err |= get_user(c, &(from->fgEnable));
			err |= put_user(c, &(to32->fgEnable));
			err |= get_uptr_to_32(&p, &(from->pvReserved));
			err |= put_user(p, &(to32->pvReserved));
			err |= get_user(u, &(from->u4ReservedSize));
			err |= put_user(u, &(to32->u4ReservedSize));
		}
	} break;
	case VAL_ISR_TYPE: {
		int i = 0;

		if (eDirection == COPY_FROM_USER) {
			struct COMPAT_VAL_ISR_T __user *from32 =
				(struct COMPAT_VAL_ISR_T *)data32;
			struct VAL_ISR_T __user *to = (struct VAL_ISR_T *)data;

			err = get_user(p, &(from32->pvHandle));
			err |= put_user(compat_ptr(p), &(to->pvHandle));
			err |= get_user(u, &(from32->u4HandleSize));
			err |= put_user(u, &(to->u4HandleSize));
			err |= get_user(u, &(from32->eDriverType));
			err |= put_user(u, &(to->eDriverType));
			err |= get_user(p, &(from32->pvIsrFunction));
			err |= put_user(compat_ptr(p), &(to->pvIsrFunction));
			err |= get_user(p, &(from32->pvReserved));
			err |= put_user(compat_ptr(p), &(to->pvReserved));
			err |= get_user(u, &(from32->u4ReservedSize));
			err |= put_user(u, &(to->u4ReservedSize));
			err |= get_user(u, &(from32->u4TimeoutMs));
			err |= put_user(u, &(to->u4TimeoutMs));
			err |= get_user(u, &(from32->u4IrqStatusNum));
			err |= put_user(u, &(to->u4IrqStatusNum));
			for (; i < IRQ_STATUS_MAX_NUM; i++) {
				err |= get_user(u, &(from32->u4IrqStatus[i]));
				err |= put_user(u, &(to->u4IrqStatus[i]));
			}
		} else {
			struct COMPAT_VAL_ISR_T __user *to32 =
				(struct COMPAT_VAL_ISR_T *)data32;
			struct VAL_ISR_T __user *from =
				(struct VAL_ISR_T *)data;

			err = get_uptr_to_32(&p, &(from->pvHandle));
			err |= put_user(p, &(to32->pvHandle));
			err |= get_user(u, &(from->u4HandleSize));
			err |= put_user(u, &(to32->u4HandleSize));
			err |= get_user(u, &(from->eDriverType));
			err |= put_user(u, &(to32->eDriverType));
			err |= get_uptr_to_32(&p, &(from->pvIsrFunction));
			err |= put_user(p, &(to32->pvIsrFunction));
			err |= get_uptr_to_32(&p, &(from->pvReserved));
			err |= put_user(p, &(to32->pvReserved));
			err |= get_user(u, &(from->u4ReservedSize));
			err |= put_user(u, &(to32->u4ReservedSize));
			err |= get_user(u, &(from->u4TimeoutMs));
			err |= put_user(u, &(to32->u4TimeoutMs));
			err |= get_user(u, &(from->u4IrqStatusNum));
			err |= put_user(u, &(to32->u4IrqStatusNum));
			for (; i < IRQ_STATUS_MAX_NUM; i++) {
				err |= get_user(u, &(from->u4IrqStatus[i]));
				err |= put_user(u, &(to32->u4IrqStatus[i]));
			}
		}
	} break;
	case VAL_MEMORY_TYPE: {
		if (eDirection == COPY_FROM_USER) {
			struct COMPAT_VAL_MEMORY_T __user *from32 =
				(struct COMPAT_VAL_MEMORY_T *)data32;
			struct VAL_MEMORY_T __user *to =
				(struct VAL_MEMORY_T *)data;

			err = get_user(u, &(from32->eMemType));
			err |= put_user(u, &(to->eMemType));
			err |= get_user(l, &(from32->u4MemSize));
			err |= put_user(l, &(to->u4MemSize));
			err |= get_user(p, &(from32->pvMemVa));
			err |= put_user(compat_ptr(p), &(to->pvMemVa));
			err |= get_user(p, &(from32->pvMemPa));
			err |= put_user(compat_ptr(p), &(to->pvMemPa));
			err |= get_user(u, &(from32->eAlignment));
			err |= put_user(u, &(to->eAlignment));
			err |= get_user(p, &(from32->pvAlignMemVa));
			err |= put_user(compat_ptr(p), &(to->pvAlignMemVa));
			err |= get_user(p, &(from32->pvAlignMemPa));
			err |= put_user(compat_ptr(p), &(to->pvAlignMemPa));
			err |= get_user(u, &(from32->eMemCodec));
			err |= put_user(u, &(to->eMemCodec));
			err |= get_user(u, &(from32->i4IonShareFd));
			err |= put_user(u, &(to->i4IonShareFd));
			err |= get_user(p, &(from32->pIonBufhandle));
			err |= put_user(compat_ptr(p), &(to->pIonBufhandle));
			err |= get_user(p, &(from32->pvReserved));
			err |= put_user(compat_ptr(p), &(to->pvReserved));
			err |= get_user(l, &(from32->u4ReservedSize));
			err |= put_user(l, &(to->u4ReservedSize));
		} else {
			struct COMPAT_VAL_MEMORY_T __user *to32 =
				(struct COMPAT_VAL_MEMORY_T *)data32;

			struct VAL_MEMORY_T __user *from =
				(struct VAL_MEMORY_T *)data;

			err = get_user(u, &(from->eMemType));
			err |= put_user(u, &(to32->eMemType));
			err |= get_user(l, &(from->u4MemSize));
			err |= put_user(l, &(to32->u4MemSize));
			err |= get_uptr_to_32(&p, &(from->pvMemVa));
			err |= put_user(p, &(to32->pvMemVa));
			err |= get_uptr_to_32(&p, &(from->pvMemPa));
			err |= put_user(p, &(to32->pvMemPa));
			err |= get_user(u, &(from->eAlignment));
			err |= put_user(u, &(to32->eAlignment));
			err |= get_uptr_to_32(&p, &(from->pvAlignMemVa));
			err |= put_user(p, &(to32->pvAlignMemVa));
			err |= get_uptr_to_32(&p, &(from->pvAlignMemPa));
			err |= put_user(p, &(to32->pvAlignMemPa));
			err |= get_user(u, &(from->eMemCodec));
			err |= put_user(u, &(to32->eMemCodec));
			err |= get_user(u, &(from->i4IonShareFd));
			err |= put_user(u, &(to32->i4IonShareFd));
			err |= get_uptr_to_32(
				&p, (void __user **)&(from->pIonBufhandle));
			err |= put_user(p, &(to32->pIonBufhandle));
			err |= get_uptr_to_32(&p, &(from->pvReserved));
			err |= put_user(p, &(to32->pvReserved));
			err |= get_user(l, &(from->u4ReservedSize));
			err |= put_user(l, &(to32->u4ReservedSize));
		}
	} break;
	default:
		break;
	}

	return err;
}

static long vcodec_unlocked_compat_ioctl(struct file *file, unsigned int cmd,
					 unsigned long arg)
{
	long ret = 0;

	/* MODULE_MFV_PR_DEBUG("vcodec_unlocked_compat_ioctl: 0x%x\n", cmd); */
	switch (cmd) {
	case VCODEC_ALLOC_NON_CACHE_BUFFER:
	case VCODEC_FREE_NON_CACHE_BUFFER: {
		struct COMPAT_VAL_MEMORY_T __user *data32;
		struct VAL_MEMORY_T __user *data;
		int err;

		data32 = compat_ptr(arg);
		data = compat_alloc_user_space(sizeof(struct VAL_MEMORY_T));
		if (data == NULL)
			return -EFAULT;

		err = compat_copy_struct(VAL_MEMORY_TYPE, COPY_FROM_USER,
					 (void *)data32, (void *)data);
		if (err)
			return err;

		ret = file->f_op->unlocked_ioctl(file, cmd,
						 (unsigned long)data);

		err = compat_copy_struct(VAL_MEMORY_TYPE, COPY_TO_USER,
					 (void *)data32, (void *)data);

		if (err)
			return err;
		return ret;
	} break;
	case VCODEC_LOCKHW:
	case VCODEC_UNLOCKHW: {
		struct COMPAT_VAL_HW_LOCK_T __user *data32;
		struct VAL_HW_LOCK_T __user *data;
		int err;

		data32 = compat_ptr(arg);
		data = compat_alloc_user_space(sizeof(struct VAL_HW_LOCK_T));
		if (data == NULL)
			return -EFAULT;

		err = compat_copy_struct(VAL_HW_LOCK_TYPE,
					COPY_FROM_USER,
					(void *)data32, (void *)data);
		if (err)
			return err;

		ret = file->f_op->unlocked_ioctl(file, cmd,
						 (unsigned long)data);

		err = compat_copy_struct(VAL_HW_LOCK_TYPE, COPY_TO_USER,
					 (void *)data32, (void *)data);

		if (err)
			return err;
		return ret;
	} break;

	case VCODEC_INC_PWR_USER:
	case VCODEC_DEC_PWR_USER: {
		struct COMPAT_VAL_POWER_T __user *data32;
		struct VAL_POWER_T __user *data;
		int err;

		data32 = compat_ptr(arg);
		data = compat_alloc_user_space(sizeof(struct VAL_POWER_T));
		if (data == NULL)
			return -EFAULT;

		err = compat_copy_struct(VAL_POWER_TYPE, COPY_FROM_USER,
					 (void *)data32, (void *)data);

		if (err)
			return err;

		ret = file->f_op->unlocked_ioctl(file, cmd,
						 (unsigned long)data);

		err = compat_copy_struct(VAL_POWER_TYPE, COPY_TO_USER,
					 (void *)data32, (void *)data);

		if (err)
			return err;
		return ret;
	} break;

	case VCODEC_WAITISR: {
		struct COMPAT_VAL_ISR_T __user *data32;
		struct VAL_ISR_T __user *data;
		int err;

		data32 = compat_ptr(arg);
		data = compat_alloc_user_space(sizeof(struct VAL_ISR_T));
		if (data == NULL)
			return -EFAULT;

		err = compat_copy_struct(VAL_ISR_TYPE, COPY_FROM_USER,
					 (void *)data32, (void *)data);
		if (err)
			return err;

		ret = file->f_op->unlocked_ioctl(file, VCODEC_WAITISR,
						 (unsigned long)data);

		err = compat_copy_struct(VAL_ISR_TYPE, COPY_TO_USER,
					 (void *)data32, (void *)data);

		if (err)
			return err;
		return ret;
	} break;

	default: {
		return vcodec_unlocked_ioctl(file, cmd, arg);
	} break;
	}
	return 0;
}
#else
#define vcodec_unlocked_compat_ioctl NULL
#endif
static int vcodec_open(struct inode *inode, struct file *file)
{
	MODULE_MFV_PR_DEBUG("[VCODEC_DEBUG] %s\n", __func__);

	mutex_lock(&DriverOpenCountLock);
	MT8167Driver_Open_Count++;

	MODULE_MFV_PR_DEBUG("%s pid = %d, MT8167Driver_Open_Count %d\n",
			  __func__, current->pid, MT8167Driver_Open_Count);
	mutex_unlock(&DriverOpenCountLock);

	/* TODO: Check upper limit of concurrent users? */

	return 0;
}

static int venc_hw_reset(int type)
{
	unsigned int uValue;
	enum VAL_RESULT_T eValRet;

	MODULE_MFV_PR_DEBUG("Start VENC HW Reset");

	/* //clear irq event */
	/* EncIsrEvent.u4TimeoutMs = 1; */
	/* eValRet = eVideoWaitEvent(&EncIsrEvent, */
	/* sizeof(struct VAL_EVENT_T)); */
	/* MODULE_MFV_PR_DEBUG("ret %d", eValRet); */
	/* if type == 0//Soft Reset */
	/* step 1 */
	VDO_HW_WRITE(KVA_VENC_SW_PAUSE, 1);
	/* step 2 */
	/* EncIsrEvent.u4TimeoutMs = 10000; */
	EncIsrEvent.u4TimeoutMs = 2;
	eValRet = eVideoWaitEvent(&EncIsrEvent,
		sizeof(struct VAL_EVENT_T));
	if (eValRet == VAL_RESULT_INVALID_ISR ||
	    gu4HwVencIrqStatus != VENC_IRQ_STATUS_PAUSE) {
		uValue = VDO_HW_READ(KVA_VENC_IRQ_STATUS_ADDR);
		if (gu4HwVencIrqStatus != VENC_IRQ_STATUS_PAUSE)
			udelay(200);

		MODULE_MFV_PR_DEBUG("irq_status 0x%x", uValue);
		VDO_HW_WRITE(KVA_VENC_SW_PAUSE, 0);
		VDO_HW_WRITE(KVA_VENC_SW_HRST_N, 0);
		uValue = VDO_HW_READ(KVA_VENC_SW_HRST_N);
		MODULE_MFV_PR_DEBUG("3 HRST = %d, isr = 0x%x", uValue,
				  gu4HwVencIrqStatus);
	} else { /* step 4 */
		VDO_HW_WRITE(KVA_VENC_SW_HRST_N, 0);
		uValue = gu4HwVencIrqStatus;
		VDO_HW_WRITE(KVA_VENC_SW_PAUSE, 0);
		MODULE_MFV_PR_DEBUG("4 HRST = %d, isr = 0x%x", uValue,
				    gu4HwVencIrqStatus);
	}

	VDO_HW_WRITE(KVA_VENC_SW_HRST_N, 1);
	uValue = VDO_HW_READ(KVA_VENC_SW_HRST_N);
	MODULE_MFV_PR_DEBUG("HRST = %d", uValue);

	return 1;
}

static int vcodec_flush(struct file *file, fl_owner_t id)
{
	MODULE_MFV_PR_DEBUG("%s, curr_tid =%d\n", __func__, current->pid);
	MODULE_MFV_PR_DEBUG(
		"%s pid = %d, MT8167Driver_Open_Count %d\n",
		__func__, current->pid, MT8167Driver_Open_Count);

	return 0;
}

static int vcodec_release(struct inode *inode, struct file *file)
{
	unsigned long ulFlagsLockHW, ulFlagsISR;
	/* dump_stack(); */
	MODULE_MFV_PR_DEBUG("[VCODEC_DEBUG] %s, curr_tid =%d\n",
			    __func__, current->pid);
	mutex_lock(&DriverOpenCountLock);
	MODULE_MFV_PR_DEBUG(
		"vcodec_flush pid = %d, MT8167Driver_Open_Count %d\n",
		current->pid, MT8167Driver_Open_Count);
	MT8167Driver_Open_Count--;

	if (MT8167Driver_Open_Count == 0) {
		mutex_lock(&VencHWLock);
		if (grVcodecEncHWLock.eDriverType == VAL_DRIVER_TYPE_H264_ENC) {
			venc_hw_reset(0);
			disable_irq(VENC_IRQ_ID);
			venc_power_off();
			MODULE_MFV_PR_DEBUG("Clean venc lock\n");
		}
		mutex_unlock(&VencHWLock);

		mutex_lock(&VdecHWLock);
		gu4VdecLockThreadId = 0;
		grVcodecDecHWLock.pvHandle = 0;
		grVcodecDecHWLock.eDriverType = VAL_DRIVER_TYPE_NONE;
		grVcodecDecHWLock.rLockedTime.u4Sec = 0;
		grVcodecDecHWLock.rLockedTime.u4uSec = 0;
		mutex_unlock(&VdecHWLock);

		mutex_lock(&VencHWLock);
		grVcodecEncHWLock.pvHandle = 0;
		grVcodecEncHWLock.eDriverType = VAL_DRIVER_TYPE_NONE;
		grVcodecEncHWLock.rLockedTime.u4Sec = 0;
		grVcodecEncHWLock.rLockedTime.u4uSec = 0;
		mutex_unlock(&VencHWLock);

		mutex_lock(&DecEMILock);
		gu4DecEMICounter = 0;
		mutex_unlock(&DecEMILock);

		mutex_lock(&EncEMILock);
		gu4EncEMICounter = 0;
		mutex_unlock(&EncEMILock);

		mutex_lock(&PWRLock);
		gu4PWRCounter = 0;
		mutex_unlock(&PWRLock);

#ifdef MT8167_VENC_USE_L2C
		mutex_lock(&L2CLock);
		if (gu4L2CCounter != 0) {
			MODULE_MFV_PR_DEBUG(
				"vcodec_flush pid = %d, L2 user = %d, force restore L2 settings\n",
				current->pid, gu4L2CCounter);
			if (config_L2(1))
				MODULE_MFV_PR_DEBUG(
					"restore L2 settings failed\n");
		}
		gu4L2CCounter = 0;
		mutex_unlock(&L2CLock);
#endif
		spin_lock_irqsave(&LockDecHWCountLock, ulFlagsLockHW);
		gu4LockDecHWCount = 0;
		spin_unlock_irqrestore(&LockDecHWCountLock, ulFlagsLockHW);

		spin_lock_irqsave(&LockEncHWCountLock, ulFlagsLockHW);
		gu4LockEncHWCount = 0;
		spin_unlock_irqrestore(&LockEncHWCountLock, ulFlagsLockHW);

		spin_lock_irqsave(&DecISRCountLock, ulFlagsISR);
		gu4DecISRCount = 0;
		spin_unlock_irqrestore(&DecISRCountLock, ulFlagsISR);

		spin_lock_irqsave(&EncISRCountLock, ulFlagsISR);
		gu4EncISRCount = 0;
		spin_unlock_irqrestore(&EncISRCountLock, ulFlagsISR);
	}
	mutex_unlock(&DriverOpenCountLock);

	return 0;
}

void vcodec_vma_open(struct vm_area_struct *vma)
{
	MODULE_MFV_PR_DEBUG("vcodec VMA open, virt %lx, phys %lx\n",
			    vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}

void vcodec_vma_close(struct vm_area_struct *vma)
{
	MODULE_MFV_PR_DEBUG("vcodec VMA close, virt %lx, phys %lx\n",
			    vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}

static const struct vm_operations_struct vcodec_remap_vm_ops = {
	.open = vcodec_vma_open,
	.close = vcodec_vma_close,
};

static int vcodec_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned int u4I = 0;
	unsigned long length;
	unsigned long pfn;

	length = vma->vm_end - vma->vm_start;
	pfn = vma->vm_pgoff << PAGE_SHIFT;

	if (((length > VENC_REGION) || (pfn < VENC_BASE) ||
	     (pfn > VENC_BASE + VENC_REGION)) &&
	    ((length > VENC_REGION) || (pfn < VENC_LT_BASE) ||
	     (pfn > VENC_LT_BASE + VENC_REGION)) &&
	    ((length > VDEC_REGION) || (pfn < VDEC_BASE_PHY) ||
	     (pfn > VDEC_BASE_PHY + VDEC_REGION)) &&
	    ((length > HW_REGION) || (pfn < HW_BASE) ||
	     (pfn > HW_BASE + HW_REGION)) &&
	    ((length > INFO_REGION) || (pfn < INFO_BASE) ||
	     (pfn > INFO_BASE + INFO_REGION))) {
		unsigned long ulAddr, ulSize;

		for (u4I = 0; u4I < VCODEC_MULTIPLE_INSTANCE_NUM_x_10; u4I++) {
			if ((grNonCacheMemoryList[u4I].ulKVA != -1L) &&
			    (grNonCacheMemoryList[u4I].ulKPA != -1L)) {
				ulAddr = grNonCacheMemoryList[u4I].ulKPA;
				ulSize = (grNonCacheMemoryList[u4I].ulSize +
					  0x1000 - 1) &
					 ~(0x1000 - 1);
				if ((length == ulSize) && (pfn == ulAddr)) {
					MODULE_MFV_PR_DEBUG(" cache idx %d\n",
							    u4I);
					break;
				}
			}
		}

		if (u4I == VCODEC_MULTIPLE_INSTANCE_NUM_x_10) {
			MODULE_MFV_PR_DEBUG(
				"[ERROR] mmap region error: Length(0x%lx), pfn(0x%lx)\n",
				(unsigned long)length, pfn);
			return -EAGAIN;
		}
	}

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	MODULE_MFV_PR_DEBUG(
		"[mmap] vma->start 0x%lx, vma->end 0x%lx, vma->pgoff 0x%lx\n",
		(unsigned long)vma->vm_start, (unsigned long)vma->vm_end,
		(unsigned long)vma->vm_pgoff);
	if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
			    vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}

	vma->vm_ops = &vcodec_remap_vm_ops;
	vcodec_vma_open(vma);

	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void vcodec_early_suspend(struct early_suspend *h)
{
	mutex_lock(&PWRLock);
	MODULE_MFV_PR_DEBUG("%s, tid = %d, PWR_USER = %d\n",
			    __func__, current->pid, gu4PWRCounter);
	mutex_unlock(&PWRLock);
	MODULE_MFV_PR_DEBUG("%s - tid = %d\n", __func__, current->pid);
}

static void vcodec_late_resume(struct early_suspend *h)
{
	mutex_lock(&PWRLock);
	MODULE_MFV_PR_DEBUG("%s, tid = %d, PWR_USER = %d\n",
			    __func__, current->pid, gu4PWRCounter);
	mutex_unlock(&PWRLock);
	MODULE_MFV_PR_DEBUG("%s - tid = %d\n", __func__, current->pid);
}

static struct early_suspend vcodec_early_suspend_handler = {
	.level = (EARLY_SUSPEND_LEVEL_DISABLE_FB - 1),
	.suspend = vcodec_early_suspend,
	.resume = vcodec_late_resume,
};
#endif

static const struct file_operations vcodec_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = vcodec_unlocked_ioctl,
	.open = vcodec_open,
	.flush = vcodec_flush,
	.release = vcodec_release,
	.mmap = vcodec_mmap,
#if IS_ENABLED(CONFIG_COMPAT)
	.compat_ioctl = vcodec_unlocked_compat_ioctl,
#endif
};

static int vcodec_probe(struct platform_device *pdev)
{
	int ret;
	struct platform_device *pVdecDev;
	struct platform_device *pVencDev;

	MODULE_MFV_PR_DEBUG("+%s\n", __func__);

	pVdecDev = vcodec_get_pdev("mediatek,mt8167-vdec");
	g_clk_vdec_sel = devm_clk_get(&pVdecDev->dev, "vdec_sel");
	g_clk_vdec_cken = devm_clk_get(&pVdecDev->dev, "vdec_cken");
	pVencDev = vcodec_get_pdev("mediatek,mt8167-venc");
	g_clk_venc = devm_clk_get(&pVencDev->dev, "img_venc");

	mutex_lock(&DecEMILock);
	gu4DecEMICounter = 0;
	mutex_unlock(&DecEMILock);

	mutex_lock(&EncEMILock);
	gu4EncEMICounter = 0;
	mutex_unlock(&EncEMILock);

	mutex_lock(&PWRLock);
	gu4PWRCounter = 0;
	mutex_unlock(&PWRLock);

	mutex_lock(&L2CLock);
	gu4L2CCounter = 0;
	mutex_unlock(&L2CLock);

	ret = register_chrdev_region(vcodec_devno, 1, VCODEC_DEVNAME);
	if (ret)
		MODULE_MFV_PR_DEBUG(
			"[VCODEC_DEBUG][ERROR] Can't Get Major number for VCodec Device\n");

	vcodec_cdev = cdev_alloc();
	vcodec_cdev->owner = THIS_MODULE;
	vcodec_cdev->ops = &vcodec_fops;

	ret = cdev_add(vcodec_cdev, vcodec_devno, 1);
	if (ret)
		MODULE_MFV_PR_DEBUG(
			"[VCODEC_DEBUG][ERROR] Can't add Vcodec Device\n");

	vcodec_class = class_create(THIS_MODULE, VCODEC_DEVNAME);
	if (IS_ERR(vcodec_class)) {
		ret = PTR_ERR(vcodec_class);
		MODULE_MFV_PR_DEBUG("Unable to create class, err = %d", ret);
		return ret;
	}

	vcodec_device = device_create(vcodec_class, NULL, vcodec_devno, NULL,
				      VCODEC_DEVNAME);

#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT
#else
	/* if (request_irq(MT_VDEC_IRQ_ID , (irq_handler_t)video_intr_dlr,*/
	/* IRQF_TRIGGER_LOW, VCODEC_DEVNAME, NULL) < 0) */
	if (request_irq(VDEC_IRQ_ID, (irq_handler_t)video_intr_dlr,
			IRQF_TRIGGER_LOW, VCODEC_DEVNAME, NULL) < 0) {
		MODULE_MFV_PR_DEBUG(
			"[VCODEC_DEBUG][ERROR] error to request dec irq\n");
	} else {
		MODULE_MFV_PR_DEBUG(
			"[VCODEC_DEBUG] success to request dec irq: %d\n",
			VDEC_IRQ_ID);
	}

	/* if (request_irq(MT_VENC_IRQ_ID , (irq_handler_t)video_intr_dlr2,*/
	/* IRQF_TRIGGER_LOW, VCODEC_DEVNAME, NULL) < 0) */
	if (request_irq(VENC_IRQ_ID, (irq_handler_t)video_intr_dlr2,
			IRQF_TRIGGER_LOW, VCODEC_DEVNAME, NULL) < 0) {
		MODULE_MFV_PR_DEBUG(
			"[VCODEC_DEBUG][ERROR] error to request enc irq: %d\n",
			VENC_IRQ_ID);
	} else {
		MODULE_MFV_PR_DEBUG(
			"[VCODEC_DEBUG] success to request enc irq: %d\n",
			VENC_IRQ_ID);
	}

#ifndef CONFIG_MACH_MT8167
	if (request_irq(VENC_LT_IRQ_ID, (irq_handler_t)video_intr_dlr2,
			IRQF_TRIGGER_LOW, VCODEC_DEVNAME, NULL) < 0) {
		MODULE_MFV_PR_DEBUG(
			"[VCODEC_DEBUG][ERROR] error to request enc_LT irq\n");
	} else {
		MODULE_MFV_PR_DEBUG(
			"[VCODEC_DEBUG] success to request enc_LT irq: %d\n",
			VENC_LT_IRQ_ID);
	}
#endif
	/* disable_irq(MT_VDEC_IRQ_ID); */
	disable_irq(VDEC_IRQ_ID);
	/* disable_irq(MT_VENC_IRQ_ID); */
	disable_irq(VENC_IRQ_ID);
#ifndef CONFIG_MACH_MT8167
	disable_irq(VENC_LT_IRQ_ID);
#endif
#endif

	MODULE_MFV_PR_DEBUG("[VCODEC_DEBUG] %s Done\n", __func__);

	return 0;
}

static int venc_disableIRQ(struct VAL_HW_LOCK_T *prHWLock)
{
	unsigned int u4IrqId = VENC_IRQ_ID;

/*if (VAL_DRIVER_TYPE_H264_ENC == prHWLock->eDriverType */
/*	 || VAL_DRIVER_TYPE_HEVC_ENC == prHWLock->eDriverType)*/
/*	 u4IrqId = VENC_IRQ_ID;*/
/* else if (VAL_DRIVER_TYPE_VP8_ENC == prHWLock->eDriverType)*/
/*	u4IrqId = VENC_LT_IRQ_ID; */

#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT
	if (prHWLock->bSecureInst == VAL_FALSE)
		free_irq(u4IrqId, NULL);
#else
	/*disable_irq(u4IrqId);*/
	disable_irq_nosync(u4IrqId);
#endif
	venc_power_off();
	return 0;
}

static int venc_enableIRQ(struct VAL_HW_LOCK_T *prHWLock)
{
	unsigned int u4IrqId = VENC_IRQ_ID;

	MODULE_MFV_PR_DEBUG("%s +\n", __func__);
	/*if (VAL_DRIVER_TYPE_H264_ENC == prHWLock->eDriverType*/
	/*  || VAL_DRIVER_TYPE_HEVC_ENC == prHWLock->eDriverType)*/
	/*	 u4IrqId = VENC_IRQ_ID;*/
	/*else if (VAL_DRIVER_TYPE_VP8_ENC == prHWLock->eDriverType)*/
	/*	 u4IrqId = VENC_LT_IRQ_ID;*/
	venc_power_on();
#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT
	MODULE_MFV_PR_DEBUG("[VCODEC_LOCKHW] ENC rHWLock.bSecureInst 0x%x\n",
			    prHWLock->bSecureInst);
	if (prHWLock->bSecureInst == VAL_FALSE) {
		MODULE_MFV_PR_DEBUG(
			"[VCODEC_LOCKHW]  ENC Request IR by type 0x%x\n",
			prHWLock->eDriverType);
		if (request_irq(u4IrqId, (irq_handler_t)video_intr_dlr2,
				IRQF_TRIGGER_LOW, VCODEC_DEVNAME, NULL) < 0) {
			MODULE_MFV_PR_DEBUG(
				"[VCODEC_LOCKHW] ENC [MFV_DEBUG][ERROR] error to request enc irq\n");
		} else {
			MODULE_MFV_PR_DEBUG(
				"[VCODEC_LOCKHW] ENC [MFV_DEBUG] success to request enc irq\n");
		}
	}
#else
	enable_irq(u4IrqId);
#endif

	MODULE_MFV_PR_DEBUG("%s -\n", __func__);
	return 0;
}

static int vcodec_remove(struct platform_device *pDev)
{
	/*pm_runtime_disable(&pvenc_dev->dev);*/
	/*pm_runtime_disable(&pvenclt_dev->dev);*/
	MODULE_MFV_PR_DEBUG("%s\n", __func__);

#if CONFIG_OF
	{
		struct platform_device *pVdecDev;
		struct platform_device *pVencDev;

		pVdecDev = vcodec_get_pdev("mediatek,mt8167-vdec");
		devm_clk_put(&pVdecDev->dev, g_clk_vdec_sel);
		devm_clk_put(&pVdecDev->dev, g_clk_vdec_cken);
		MODULE_MFV_PR_DEBUG("[vdec-clk] put clk sel:0x%lx cken:0x%lx\n",
				    (unsigned long int)g_clk_vdec_sel,
				    (unsigned long int)g_clk_vdec_cken);

		pVencDev = vcodec_get_pdev("mediatek,mt8167-venc");
		devm_clk_put(&pVencDev->dev, g_clk_venc);
		MODULE_MFV_PR_DEBUG("[venc-clk] put clk 0x%lx\n",
				    (unsigned long int)g_clk_venc);
	}
#endif

	return 0;
}

#ifdef CONFIG_OF
/* VDEC main device */
static const struct of_device_id vcodec_of_ids[] = {
	{
		.compatible = "mediatek,mt8167-vdec_gcon",
	},
	{}
};

static struct platform_driver VCodecDriver = {
	.probe = vcodec_probe,
	.remove = vcodec_remove,
	.driver = {
		.name = VCODEC_DEVNAME,
		.owner = THIS_MODULE,
		.of_match_table = vcodec_of_ids,
	}
};
#endif

#ifdef CONFIG_MTK_HIBERNATION
static int vcodec_pm_restore_noirq(struct device *device)
{
	/* vdec : IRQF_TRIGGER_LOW */
	mt_irq_set_sens(VDEC_IRQ_ID, MT_LEVEL_SENSITIVE);
	mt_irq_set_polarity(VDEC_IRQ_ID, MT_POLARITY_LOW);
	/* venc: IRQF_TRIGGER_LOW */
	mt_irq_set_sens(VENC_IRQ_ID, MT_LEVEL_SENSITIVE);
	mt_irq_set_polarity(VENC_IRQ_ID, MT_POLARITY_LOW);

	return 0;
}
#endif

static int __init vcodec_driver_init(void)
{
	enum VAL_RESULT_T eValHWLockRet;
	unsigned long ulFlags, ulFlagsLockHW, ulFlagsISR;
	struct device_node *node = NULL;

	MODULE_MFV_PR_DEBUG("+vcodec_init !!\n");

	mutex_lock(&DriverOpenCountLock);
	MT8167Driver_Open_Count = 0;
	mutex_unlock(&DriverOpenCountLock);

/* get VENC related */
	node = of_find_compatible_node(NULL, NULL, "mediatek,mt8167-venc");
	KVA_VENC_BASE = (unsigned long)of_iomap(node, 0);
	VENC_IRQ_ID = irq_of_parse_and_map(node, 0);
	KVA_VENC_IRQ_STATUS_ADDR = KVA_VENC_BASE + 0x05C;
	KVA_VENC_IRQ_ACK_ADDR = KVA_VENC_BASE + 0x060;
	KVA_VENC_SW_PAUSE = KVA_VENC_BASE + VENC_SW_PAUSE;
	KVA_VENC_SW_HRST_N = KVA_VENC_BASE + VENC_SW_HRST_N;

#ifndef CONFIG_MACH_MT8167
	node = of_find_compatible_node(NULL, NULL, "mediatek,mt8167-venclt");
	KVA_VENC_LT_BASE = (unsigned long)of_iomap(node, 0);
	VENC_LT_IRQ_ID = irq_of_parse_and_map(node, 0);
	KVA_VENC_LT_IRQ_STATUS_ADDR = KVA_VENC_LT_BASE + 0x05C;
	KVA_VENC_LT_IRQ_ACK_ADDR = KVA_VENC_LT_BASE + 0x060;
	KVA_VENC_LT_SW_PAUSE = KVA_VENC_LT_BASE + VENC_SW_PAUSE;
	KVA_VENC_LT_SW_HRST_N = KVA_VENC_LT_BASE + VENC_SW_HRST_N;
#endif

	{
		struct device_node *node = NULL;

		node = of_find_compatible_node(NULL, NULL,
					       "mediatek,mt8167-vdec");
		KVA_VDEC_BASE = (unsigned long)of_iomap(node, 0);
		VDEC_IRQ_ID = irq_of_parse_and_map(node, 0);
		KVA_VDEC_MISC_BASE = KVA_VDEC_BASE + 0x5000;
		KVA_VDEC_VLD_BASE = KVA_VDEC_BASE + 0x0000;
	}
	{
		struct device_node *node = NULL;

		node = of_find_compatible_node(NULL, NULL,
					       "mediatek,mt8167-vdec_gcon");
		KVA_VDEC_GCON_BASE = (unsigned long)of_iomap(node, 0);
		MODULE_MFV_PR_DEBUG(
			"[DeviceTree] KVA_VENC_BASE(0x%lx),KVA_VDEC_BASE(0x%lx)",
			KVA_VENC_BASE, KVA_VDEC_BASE);
		MODULE_MFV_PR_DEBUG(
			"[DeviceTree] KVA_VDEC_GCON_BASE(0x%lx)",
			KVA_VDEC_GCON_BASE);
		MODULE_MFV_PR_DEBUG(
			"[DeviceTree] VDEC_IRQ_ID(%d), VENC_IRQ_ID(%d)",
			VDEC_IRQ_ID, VENC_IRQ_ID);
	}
/* KVA_VENC_IRQ_STATUS_ADDR =   */
/*(unsigned long)ioremap(VENC_IRQ_STATUS_addr, 4);*/
/* KVA_VENC_IRQ_ACK_ADDR  = (unsigned long)ioremap(VENC_IRQ_ACK_addr, 4); */

	spin_lock_irqsave(&LockDecHWCountLock, ulFlagsLockHW);
	gu4LockDecHWCount = 0;
	spin_unlock_irqrestore(&LockDecHWCountLock, ulFlagsLockHW);

	spin_lock_irqsave(&LockEncHWCountLock, ulFlagsLockHW);
	gu4LockEncHWCount = 0;
	spin_unlock_irqrestore(&LockEncHWCountLock, ulFlagsLockHW);

	spin_lock_irqsave(&DecISRCountLock, ulFlagsISR);
	gu4DecISRCount = 0;
	spin_unlock_irqrestore(&DecISRCountLock, ulFlagsISR);

	spin_lock_irqsave(&EncISRCountLock, ulFlagsISR);
	gu4EncISRCount = 0;
	spin_unlock_irqrestore(&EncISRCountLock, ulFlagsISR);

	mutex_lock(&VdecPWRLock);
	gu4VdecPWRCounter = 0;
	mutex_unlock(&VdecPWRLock);

	mutex_lock(&VencPWRLock);
	gu4VencPWRCounter = 0;
	mutex_unlock(&VencPWRLock);

	mutex_lock(&IsOpenedLock);
	if (bIsOpened == VAL_FALSE) {
		bIsOpened = VAL_TRUE;
		platform_driver_register(&VCodecDriver);
/*platform_driver_register(&VencDriver);*/
/*platform_driver_register(&VencltDriver);*/
	}
	mutex_unlock(&IsOpenedLock);

	mutex_lock(&VdecHWLock);
	gu4VdecLockThreadId = 0;
	grVcodecDecHWLock.pvHandle = 0;
	grVcodecDecHWLock.eDriverType = VAL_DRIVER_TYPE_NONE;
	grVcodecDecHWLock.rLockedTime.u4Sec = 0;
	grVcodecDecHWLock.rLockedTime.u4uSec = 0;
	mutex_unlock(&VdecHWLock);

	mutex_lock(&VencHWLock);
	grVcodecEncHWLock.pvHandle = 0;
	grVcodecEncHWLock.eDriverType = VAL_DRIVER_TYPE_NONE;
	grVcodecEncHWLock.rLockedTime.u4Sec = 0;
	grVcodecEncHWLock.rLockedTime.u4uSec = 0;
	mutex_unlock(&VencHWLock);

	/* MT8167_HWLockEvent part */
	mutex_lock(&DecHWLockEventTimeoutLock);
	DecHWLockEvent.pvHandle = "DECHWLOCK_EVENT";
	DecHWLockEvent.u4HandleSize = sizeof("DECHWLOCK_EVENT") + 1;
	DecHWLockEvent.u4TimeoutMs = 1;
	mutex_unlock(&DecHWLockEventTimeoutLock);
	eValHWLockRet = eVideoCreateEvent(&DecHWLockEvent,
		sizeof(struct VAL_EVENT_T));
	if (eValHWLockRet != VAL_RESULT_NO_ERROR)
		MODULE_MFV_PR_DEBUG(
			"[MFV][ERROR] create dec hwlock event error\n");

	mutex_lock(&EncHWLockEventTimeoutLock);
	EncHWLockEvent.pvHandle = "ENCHWLOCK_EVENT";
	EncHWLockEvent.u4HandleSize = sizeof("ENCHWLOCK_EVENT") + 1;
	EncHWLockEvent.u4TimeoutMs = 1;
	mutex_unlock(&EncHWLockEventTimeoutLock);
	eValHWLockRet = eVideoCreateEvent(&EncHWLockEvent,
		sizeof(struct VAL_EVENT_T));
	if (eValHWLockRet != VAL_RESULT_NO_ERROR)
		MODULE_MFV_PR_DEBUG(
			"[MFV][ERROR] create enc hwlock event error\n");
	/* MT8167_IsrEvent part */
	spin_lock_irqsave(&DecIsrLock, ulFlags);
	DecIsrEvent.pvHandle = "DECISR_EVENT";
	DecIsrEvent.u4HandleSize = sizeof("DECISR_EVENT") + 1;
	DecIsrEvent.u4TimeoutMs = 1;
	spin_unlock_irqrestore(&DecIsrLock, ulFlags);
	eValHWLockRet = eVideoCreateEvent(&DecIsrEvent,
		sizeof(struct VAL_EVENT_T));
	if (eValHWLockRet != VAL_RESULT_NO_ERROR)
		MODULE_MFV_PR_DEBUG("[MFV][ERROR] create dec isr event error\n");

	spin_lock_irqsave(&EncIsrLock, ulFlags);
	EncIsrEvent.pvHandle = "ENCISR_EVENT";
	EncIsrEvent.u4HandleSize = sizeof("ENCISR_EVENT") + 1;
	EncIsrEvent.u4TimeoutMs = 1;
	spin_unlock_irqrestore(&EncIsrLock, ulFlags);
	eValHWLockRet = eVideoCreateEvent(&EncIsrEvent,
		sizeof(struct VAL_EVENT_T));
	if (eValHWLockRet != VAL_RESULT_NO_ERROR)
		MODULE_MFV_PR_DEBUG("[MFV][ERROR] create enc isr event error\n");

	MODULE_MFV_PR_DEBUG("[VCODEC_DEBUG] %s Done\n", __func__);

#ifdef CONFIG_HAS_EARLYSUSPEND
	register_early_suspend(&vcodec_early_suspend_handler);
#endif

#ifdef CONFIG_MTK_HIBERNATION
	register_swsusp_restore_noirq_func(ID_M_VCODEC, vcodec_pm_restore_noirq,
					   NULL);
#endif

	return 0;
}

static void __exit vcodec_driver_exit(void)
{
	enum VAL_RESULT_T eValHWLockRet;

	MODULE_MFV_PR_DEBUG("[VCODEC_DEBUG] %s\n", __func__);

	mutex_lock(&IsOpenedLock);
	if (bIsOpened == VAL_TRUE) {
		MODULE_MFV_PR_DEBUG("+%s remove device !!\n", __func__);
		platform_driver_unregister(&VCodecDriver);
		MODULE_MFV_PR_DEBUG("+%s remove done !!\n", __func__);
	}
	mutex_unlock(&IsOpenedLock);

	cdev_del(vcodec_cdev);
	unregister_chrdev_region(vcodec_devno, 1);

/* [TODO] free IRQ here */
/* free_irq(MT_VENC_IRQ_ID, NULL); */
#ifdef CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT
#else
	free_irq(VENC_IRQ_ID, NULL);
	/* free_irq(MT_VDEC_IRQ_ID, NULL); */
	free_irq(VDEC_IRQ_ID, NULL);
#endif
	/* MT6589_HWLockEvent part */
	eValHWLockRet = eVideoCloseEvent(&DecHWLockEvent,
		sizeof(struct VAL_EVENT_T));
	if (eValHWLockRet != VAL_RESULT_NO_ERROR)
		MODULE_MFV_PR_DEBUG(
			"[MFV][ERROR] close dec hwlock event error\n");

	eValHWLockRet = eVideoCloseEvent(&EncHWLockEvent,
		sizeof(struct VAL_EVENT_T));
	if (eValHWLockRet != VAL_RESULT_NO_ERROR)
		MODULE_MFV_PR_DEBUG(
			"[MFV][ERROR] close enc hwlock event error\n");

	/* MT6589_IsrEvent part */
	eValHWLockRet = eVideoCloseEvent(&DecIsrEvent,
		sizeof(struct VAL_EVENT_T));
	if (eValHWLockRet != VAL_RESULT_NO_ERROR)
		MODULE_MFV_PR_DEBUG("[MFV][ERROR] close dec isr event error\n");

	eValHWLockRet = eVideoCloseEvent(&EncIsrEvent,
		sizeof(struct VAL_EVENT_T));
	if (eValHWLockRet != VAL_RESULT_NO_ERROR)
		MODULE_MFV_PR_DEBUG("[MFV][ERROR] close enc isr event error\n");
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&vcodec_early_suspend_handler);
#endif

#ifdef CONFIG_MTK_HIBERNATION
	unregister_swsusp_restore_noirq_func(ID_M_VCODEC);
#endif
}
module_init(vcodec_driver_init);
module_exit(vcodec_driver_exit);
MODULE_AUTHOR("Legis, Lu <legis.lu@mediatek.com>");
MODULE_DESCRIPTION("8167 Vcodec Driver");
MODULE_LICENSE("GPL");
