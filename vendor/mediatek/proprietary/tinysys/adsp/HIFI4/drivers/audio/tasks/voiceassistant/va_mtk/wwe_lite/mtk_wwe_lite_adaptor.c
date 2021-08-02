/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "VowEngine_DSP_testing.h"
#include "wwe_lite_model.h"
#include "va_process.h"
#include "ww_process.h"
#include "mtk_heap.h"
static const char *debug_wakeword = "Hi wakeword";

static int wwe_adaptor_init(struct va_proc_obj *obj, struct va_pcm_format in, struct va_pcm_format *out, int frames)
{
	VOWE_DSP_testing_init_parameters ww_params;
	ww_params.dspParsedModelData.length = WWE_MODEL_SIZE;
	ww_params.dspParsedModelData.beginPtr = (char *)wwe_model;
	VOWE_DSP_testing_init(&ww_params);
	VOWE_DSP_testing_reset();

	return 0;
}

static int wwe_adaptor_uninit(struct va_proc_obj *obj)
{
	VOWE_DSP_testing_release();

	return 0;
}

static int wwe_adaptor_reset(struct va_proc_obj *obj)
{
	struct ww_private *priv = (struct ww_private *)(obj->priv);
	if (priv->detect_result)
		priv->cur_data_idx = 0;

	return 0;
}

static int wwe_adaptor_process(struct va_proc_obj *obj, char *inbuf, char *outbuf, int frames)
{
	RecognitionResults results;
	struct ww_private *priv = (struct ww_private *)(obj->priv);

	results = VOWE_DSP_testing_recognize((const short*)inbuf, (const int)frames);
	if (results.status == vowe_accept) {
		struct ww_config *config = (struct ww_config *)(&(priv->config));

		priv->detect_result = 1;
		if (priv->cur_data_idx > (long long)(config->rate * 2))
			priv->begin_idx = priv->cur_data_idx - (long long)(config->rate * 2);
		else
			priv->begin_idx = 0;
		priv->end_idx = priv->cur_data_idx;
		priv->wakeword = debug_wakeword;
		priv->confidence = (int)results.score;
		VOWE_DSP_testing_reset();
		PRINTF_D("ww ok, begin:%lld, end:%lld, score:%d, cur:%lld\n", priv->begin_idx, priv->end_idx, (int)(results.score), priv->cur_data_idx);
	} else if (results.status == vowe_reject) {
		priv->detect_result = 2;
	} else if (results.status == vowe_bad) {
		PRINTF_E("ww err:%d\n", results.status);
	} else if (results.status == vowe_hold_on){
	} else {
		PRINTF_E("ww unknown ret%d\n", results.status);
	}

	return 0;
}

void mtk_wwe_lite_adaptor_register(struct algo_ops *ops)
{
	ops->init = wwe_adaptor_init;
	ops->uninit = wwe_adaptor_uninit;
	ops->set_params = NULL;
	ops->get_params = NULL;
	ops->reset = wwe_adaptor_reset;
	ops->process = wwe_adaptor_process;
}
