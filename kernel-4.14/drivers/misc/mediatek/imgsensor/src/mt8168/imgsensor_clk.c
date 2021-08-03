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

#include <linux/clk.h>
#include "imgsensor_common.h"
#include "imgsensor_clk.h"

#define PK_DBG(fmt, arg...)  pr_debug(PREFIX fmt, ##arg)
#define PK_PR_ERR(fmt, arg...)    pr_err(fmt, ##arg)

/*by platform settings and elements should not be reordered */
char *gimgsensor_mclk_name[IMGSENSOR_CCF_MAX_NUM] = {
	"CLK_TOP_CAMTG_SEL",
	"CLK_TOP_CAMTG1_SEL",
	"CLK_MCLK_24M",
	"CLK_MCLK_26M",
	"CLK_MCLK_48M",
	"CLK_MCLK_52M",
	"CLK_MCLK_12M",
	"CLK_MCLK_13M",
	"CLK_MCLK_6M",
	"CLK_CAM_SENINF_CG",
	"CLK_MIPI_ANA_0A_CG",
	"CLK_MIPI_ANA_0B_CG",
	"CLK_MIPI_ANA_1A_CG",
	"CLK_MIPI_ANA_1B_CG",
	"CLK_TOP_CAMTM_SEL_CG",
	"CLK_TOP_SENIF_SEL_CG",
	"CLK_TOP_SELIF_SRC_208"
};

enum {
	MCLK_ENU_START,
	MCLK_24MHZ =	MCLK_ENU_START,
	MCLK_26MHZ,
	MCLK_48MHZ,
	MCLK_52MHZ,
	MCLK_12MHZ,
	MCLK_13MHZ,
	MCLK_6MHZ,
	MCLK_MAX,
};

enum {
	FREQ_24MHZ = 24,
	FREQ_26MHZ = 26,
	FREQ_48MHZ = 48,
	FREQ_52MHZ = 52,
	FREQ_12MHZ = 12,
	FREQ_13MHZ = 13,
	FREQ_6MHZ  =  6,
};

#ifdef IMGSENSOR_DFS_CTRL_ENABLE

struct pm_qos_request imgsensor_qos;
#endif
int imgsensor_dfs_ctrl(enum DFS_OPTION option, void *pbuff)
{
	int i4RetValue = 0;

	PK_DBG("%s\n", __func__);

	switch (option) {
	case DFS_CTRL_ENABLE:
		pm_qos_add_request(&imgsensor_qos, PM_QOS_CAM_FREQ, 0);
		PK_DBG("seninf PMQoS turn on\n");
		break;
	case DFS_CTRL_DISABLE:
		pm_qos_remove_request(&imgsensor_qos);
		PK_DBG("seninf PMQoS turn off\n");
		break;
	case DFS_UPDATE:
		PK_DBG(
			"seninf Set isp clock level:%d\n",
			*(unsigned int *)pbuff);
		pm_qos_update_request(&imgsensor_qos, *(unsigned int *)pbuff);

		break;
	case DFS_RELEASE:
		PK_DBG(
			"seninf release and set isp clk request to 0\n");
		pm_qos_update_request(&imgsensor_qos, 0);

		break;
	case DFS_SUPPORTED_ISP_CLOCKS:
	{
		int result = 0;
		uint64_t freq_steps[ISP_CLK_LEVEL_CNT];
		struct IMAGESENSOR_GET_SUPPORTED_ISP_CLK *pIspclks;
		unsigned int lv = 0;

		pIspclks = (struct IMAGESENSOR_GET_SUPPORTED_ISP_CLK *) pbuff;

		/* Call mmdvfs_qos_get_freq_steps
		 * to get supported frequency
		 */
		result = mmdvfs_qos_get_freq_steps(
			PM_QOS_CAM_FREQ,
			freq_steps, (u32 *)&(pIspclks->clklevelcnt));

		if (result < 0) {
			PK_PR_ERR(
			"ERR: get MMDVFS freq steps failed, result: %d\n",
				result);
			i4RetValue = -EFAULT;
			break;
		}

		if ((pIspclks->clklevelcnt) > ISP_CLK_LEVEL_CNT) {
			PK_PR_ERR("ERR: clklevelcnt is exceeded");
			i4RetValue = -EFAULT;
			break;
		}

		for (lv = 0; lv < (pIspclks->clklevelcnt); lv++) {
			/* Save clk from low to high */
			(pIspclks->clklevel)[lv] = freq_steps[lv];
			/*PK_DBG("DFS Clk level[%d]:%d",
			 *	lv, pIspclks->clklevel[lv]);
			 */
		}
	}
		break;
	case DFS_CUR_ISP_CLOCK:
	{
		unsigned int *pGetIspclk;

		pGetIspclk = (unsigned int *) pbuff;
		  *pGetIspclk = (u32)mmdvfs_qos_get_freq(PM_QOS_CAM_FREQ);
		PK_DBG("current isp clock:%d", *pGetIspclk);
	}
		break;
	default:
		PK_PR_ERR("None\n");
		break;
	}
	return i4RetValue;
}

static inline void imgsensor_clk_check(struct IMGSENSOR_CLK *pclk)
{
	int i;

	for (i = 0; i < IMGSENSOR_CCF_MCLK_FREQ_MAX_NUM; i++) {
		if (IS_ERR(pclk->imgsensor_ccf[i]))
			PK_PR_ERR("%s fail %s",
				__func__,
				gimgsensor_mclk_name[i]);
	}
}

/************************************************************************
 * Common Clock Framework (CCF)
 ************************************************************************/
enum IMGSENSOR_RETURN imgsensor_clk_init(struct IMGSENSOR_CLK *pclk)
{
	int i;
	struct platform_device *pplatform_dev = gpimgsensor_hw_platform_device;

	if (pplatform_dev == NULL) {
		PK_PR_ERR("[%s] pdev is null\n", __func__);
		return IMGSENSOR_RETURN_ERROR;
	}
	/* get all possible using clocks */
	for (i = 0; i < IMGSENSOR_CCF_MAX_NUM; i++)
		pclk->imgsensor_ccf[i] =
		    devm_clk_get(&pplatform_dev->dev, gimgsensor_mclk_name[i]);

	return IMGSENSOR_RETURN_SUCCESS;
}

int imgsensor_clk_set(
	struct IMGSENSOR_CLK *pclk,
	struct ACDK_SENSOR_MCLK_STRUCT *pmclk)
{
	int ret = 0;
	int mclk_index = MCLK_ENU_START;
	const int supported_mclk_freq[MCLK_MAX] = {
		FREQ_24MHZ, FREQ_26MHZ, FREQ_48MHZ,
		FREQ_52MHZ, FREQ_12MHZ, FREQ_13MHZ, FREQ_6MHZ};

	for (mclk_index = MCLK_ENU_START; mclk_index < MCLK_MAX; mclk_index++) {
		if (pmclk->freq == supported_mclk_freq[mclk_index])
			break;
	}
	if (pmclk->TG >= IMGSENSOR_CCF_MCLK_TG_MAX_NUM ||
		pmclk->TG < IMGSENSOR_CCF_MCLK_TG_MIN_NUM ||
		mclk_index == MCLK_MAX) {
		PK_PR_ERR(
		"[CAMERA SENSOR]kdSetSensorMclk out of range, tg=%d, freq= %d\n",
			pmclk->TG,
			pmclk->freq);

		return -EFAULT;
	}
	mclk_index += IMGSENSOR_CCF_MCLK_FREQ_MIN_NUM;
	imgsensor_clk_check(pclk);

	if (pmclk->on) {

		/* Workaround for timestamp: TG1 always ON */
		if (clk_prepare_enable(
		    pclk->imgsensor_ccf[IMGSENSOR_CCF_MCLK_TOP_CAMTG_SEL]))

			PK_PR_ERR(
			    "[CAMERA SENSOR] failed tg=%d\n",
			    IMGSENSOR_CCF_MCLK_TOP_CAMTG_SEL);
		else
			atomic_inc(
				&pclk->enable_cnt[
				IMGSENSOR_CCF_MCLK_TOP_CAMTG_SEL]);

		if (clk_prepare_enable(pclk->imgsensor_ccf[pmclk->TG]))
			PK_PR_ERR("[CAMERA SENSOR] failed tg=%d\n", pmclk->TG);
		else
			atomic_inc(&pclk->enable_cnt[pmclk->TG]);

		if (clk_prepare_enable(pclk->imgsensor_ccf[mclk_index]))
			PK_PR_ERR(
			"[CAMERA SENSOR]imgsensor_ccf failed freq= %d, mclk_index %d\n",
				pmclk->freq,
				mclk_index);
		else
			atomic_inc(&pclk->enable_cnt[mclk_index]);

		ret = clk_set_parent(
			pclk->imgsensor_ccf[pmclk->TG],
			pclk->imgsensor_ccf[mclk_index]);

	} else {

		/* Workaround for timestamp: TG1 always ON */
		clk_disable_unprepare(
		    pclk->imgsensor_ccf[IMGSENSOR_CCF_MCLK_TOP_CAMTG_SEL]);

		atomic_dec(&pclk->enable_cnt[IMGSENSOR_CCF_MCLK_TOP_CAMTG_SEL]);

		clk_disable_unprepare(pclk->imgsensor_ccf[pmclk->TG]);
		atomic_dec(&pclk->enable_cnt[pmclk->TG]);
		clk_disable_unprepare(pclk->imgsensor_ccf[mclk_index]);
		atomic_dec(&pclk->enable_cnt[mclk_index]);
	}

	return ret;
}

void imgsensor_clk_enable_all(struct IMGSENSOR_CLK *pclk)
{
	int i;

	PK_DBG("imgsensor_clk_enable_all_cg\n");
	for (i = IMGSENSOR_CCF_MTCMOS_MIN_NUM;
		i < IMGSENSOR_CCF_MTCMOS_MAX_NUM;
		i++) {
		if (!IS_ERR(pclk->imgsensor_ccf[i])) {
			if (clk_prepare_enable(pclk->imgsensor_ccf[i]))
				PK_PR_ERR(
				"[CAMERA SENSOR]imgsensor_ccf enable cmos fail cg_index = %d\n",
					i);
			else
				atomic_inc(&pclk->enable_cnt[i]);
			PK_DBG("%s %s ok\n",
				__func__,
				gimgsensor_mclk_name[i]);
		}
	}
	for (i = IMGSENSOR_CCF_CG_MIN_NUM; i < IMGSENSOR_CCF_CG_MAX_NUM; i++) {
		if (!IS_ERR(pclk->imgsensor_ccf[i])) {
			if (clk_prepare_enable(pclk->imgsensor_ccf[i]))
				PK_PR_ERR(
				"[CAMERA SENSOR]imgsensor_ccf enable cg fail cg_index = %d\n",
					i);
			else
				atomic_inc(&pclk->enable_cnt[i]);
			PK_DBG("%s %s ok\n",
				__func__,
				gimgsensor_mclk_name[i]);
		}
	}
}

void imgsensor_clk_disable_all(struct IMGSENSOR_CLK *pclk)
{
	int i;

	PK_DBG("%s\n", __func__);
	for (i = IMGSENSOR_CCF_MCLK_TG_MIN_NUM;
		i < IMGSENSOR_CCF_MAX_NUM;
		i++) {
		for (; !IS_ERR(pclk->imgsensor_ccf[i]) &&
			atomic_read(&pclk->enable_cnt[i]) > 0 ;) {
			clk_disable_unprepare(pclk->imgsensor_ccf[i]);
			atomic_dec(&pclk->enable_cnt[i]);
		}
	}
}

int imgsensor_clk_ioctrl_handler(void *pbuff)
{
	struct platform_device *pplatform_dev = gpimgsensor_hw_platform_device;
	char *mclk_camtg_name =
		gimgsensor_mclk_name[IMGSENSOR_CCF_MCLK_TOP_CAMTG_SEL];
	char *cg_camtm_sel_name =
		gimgsensor_mclk_name[IMGSENSOR_CCF_CG_CAMTM_SEL];
	char *cg_seninf_name =
		gimgsensor_mclk_name[IMGSENSOR_CCF_CG_SENINF];

	if (pplatform_dev == NULL) {
		PK_PR_ERR("[%s] pdev is null\n", __func__);
		return IMGSENSOR_RETURN_ERROR;
	}

	if (IS_ERR(devm_clk_get(&pplatform_dev->dev, mclk_camtg_name))
		|| IS_ERR(devm_clk_get(&pplatform_dev->dev, cg_camtm_sel_name))
		|| IS_ERR(devm_clk_get(&pplatform_dev->dev, cg_seninf_name))) {
		PK_DBG("cannot get camtg, camtm, seninf clock\n");
		return IMGSENSOR_RETURN_ERROR;
	}

	/* get camtg, fmm, seninf using clocks */
	PK_DBG("hf_fcamtg_ck = %lu, hf_fmm_ck = %lu, f_fseninf_ck = %lu\n",
	clk_get_rate(
		devm_clk_get(&pplatform_dev->dev,
		   gimgsensor_mclk_name[IMGSENSOR_CCF_MCLK_TOP_CAMTG_SEL])),
	clk_get_rate(
		devm_clk_get(&pplatform_dev->dev,
		   gimgsensor_mclk_name[IMGSENSOR_CCF_CG_CAMTM_SEL])),
	clk_get_rate(
		devm_clk_get(&pplatform_dev->dev,
		   gimgsensor_mclk_name[IMGSENSOR_CCF_CG_SENINF])));

	return 0;
}
