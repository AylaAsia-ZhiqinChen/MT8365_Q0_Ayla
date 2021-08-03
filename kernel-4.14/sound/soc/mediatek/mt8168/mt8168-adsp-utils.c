/*
 * mt8168-adsp-utils.c  --  Mediatek 8168 adsp utility
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

#include "mt8168-adsp-utils.h"


int mt8168_adsp_get_scene_by_dai_id(int id)
{
	switch (id) {
	case MT8168_ADSP_FE_VA:
	case MT8168_ADSP_FE_MIC_RECORD:
	case MT8168_ADSP_FE_HOSTLESS_VA:
		/* FALLTHROUGH */
	case MT8168_ADSP_BE_VUL2_IN:
	case MT8168_ADSP_BE_TDM_IN:
	case MT8168_ADSP_BE_VUL_IN:
		return TASK_SCENE_VA;
		/* FALLTHROUGH */
	default:
		break;
	}

	return -1;
}

int mt8168_adsp_send_ipi_cmd(struct ipi_msg_t *p_msg,
			     uint8_t task_scene,
			     uint8_t target,
			     uint8_t data_type,
			     uint8_t ack_type,
			     uint16_t msg_id,
			     uint32_t param1,
			     uint32_t param2,
			     char *payload)
{
	struct ipi_msg_t ipi_msg;
	struct ipi_msg_t *msg;
	int ret = 0;

	memset((void *)&ipi_msg, 0, sizeof(struct ipi_msg_t));

	if (p_msg)
		msg = p_msg;
	else
		msg = &ipi_msg;

	ret = audio_send_ipi_msg(msg, task_scene, target, data_type,
				 ack_type, msg_id, param1, param2,
				 (char *)payload);
	if (ret != 0)
		pr_err("%s audio_send_ipi_msg (%d-%d-%d-%d) fail %d\n",
			__func__, task_scene, data_type,
			ack_type, msg_id, ret);

	return ret;
}

int mt8168_adsp_dai_id_pack(int dai_id)
{
	int id = -EINVAL;

	switch (dai_id) {
	case MT8168_ADSP_FE_HOSTLESS_VA:
		id = DAI_PACK_ID(MT8168_ADSP_FE_VA,
			DAI_VA_RECORD_TYPE, DAI_HOSTLESS);
		break;
	case MT8168_ADSP_FE_VA:
		id = DAI_PACK_ID(MT8168_ADSP_FE_VA,
			DAI_VA_RECORD_TYPE, DAI_NON_HOSTLESS);
		break;
	case MT8168_ADSP_FE_MIC_RECORD:
		id = DAI_PACK_ID(MT8168_ADSP_FE_MIC_RECORD,
			DAI_MIC_RECORD_TYPE, DAI_NON_HOSTLESS);
		break;
	default:
		break;
	}
	return id;
}

static void hostless_pcm_param_set_mask(struct snd_pcm_hw_params *p,
			      int n, unsigned int bit)
{
	if (bit >= SNDRV_MASK_MAX)
		return;
	if (hw_is_mask(n)) {
		struct snd_mask *m = hw_param_mask(p, n);

		m->bits[0] = 0;
		m->bits[1] = 0;
		m->bits[bit >> 5] |= (1 << (bit & 31));
	}
}

static void hostless_pcm_param_set_min(struct snd_pcm_hw_params *p,
			      int n, unsigned int val)
{
	if (hw_is_interval(n)) {
		struct snd_interval *i = hw_param_interval(p, n);

		i->min = val;
	}
}

static void hostless_pcm_param_set_int(struct snd_pcm_hw_params *p,
			      int n, unsigned int val)
{
	if (hw_is_interval(n)) {
		struct snd_interval *i = hw_param_interval(p, n);

		i->min = val;
		i->max = val;
		i->integer = 1;
	}
}

int mt8168_adsp_hostless_pcm_open(struct snd_pcm *pcm,
			     struct snd_pcm_substream **substream)
{
	struct file dummy_file;
	int err;

	dummy_file.f_flags = O_RDWR;

	err = snd_pcm_open_substream(pcm,
			SNDRV_PCM_STREAM_CAPTURE,
			&dummy_file,
			substream);
	if (err < 0) {
		pr_info("%s hostless pcm open fail err:%d\n",
			__func__, err);
		return err;
	}
	return err;
}

void mt8168_adsp_hostless_pcm_close(struct snd_pcm_substream *substream)
{
	snd_pcm_release_substream(substream);
}

int mt8168_adsp_hostless_pcm_params(struct snd_pcm_substream *substream,
			     unsigned int rate,
			     unsigned int channels,
			     unsigned int format,
			     unsigned int period_size,
			     unsigned int period_count)
{
	struct snd_pcm_hw_params hw_params;
	struct snd_pcm_sw_params sw_params;
	int err;

	_snd_pcm_hw_params_any(&hw_params);

	if (format == 32)
		hostless_pcm_param_set_mask(&hw_params,
			SNDRV_PCM_HW_PARAM_FORMAT,
			SNDRV_PCM_FORMAT_S32_LE);
	else
		hostless_pcm_param_set_mask(&hw_params,
			SNDRV_PCM_HW_PARAM_FORMAT,
			SNDRV_PCM_FORMAT_S16_LE);

	hostless_pcm_param_set_mask(&hw_params,
		SNDRV_PCM_HW_PARAM_SUBFORMAT,
		SNDRV_PCM_SUBFORMAT_STD);
	hostless_pcm_param_set_min(&hw_params,
		SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
		period_size);
	hostless_pcm_param_set_int(&hw_params,
		SNDRV_PCM_HW_PARAM_SAMPLE_BITS,
		format);
	hostless_pcm_param_set_int(&hw_params,
		SNDRV_PCM_HW_PARAM_FRAME_BITS,
		format * channels);
	hostless_pcm_param_set_int(&hw_params,
		SNDRV_PCM_HW_PARAM_CHANNELS,
		channels);
	hostless_pcm_param_set_int(&hw_params,
		SNDRV_PCM_HW_PARAM_PERIODS,
		period_count);
	hostless_pcm_param_set_int(&hw_params,
		SNDRV_PCM_HW_PARAM_RATE,
		rate);
	hostless_pcm_param_set_mask(&hw_params,
		SNDRV_PCM_HW_PARAM_ACCESS,
		SNDRV_PCM_ACCESS_RW_INTERLEAVED);

	err = snd_pcm_kernel_ioctl(substream,
			SNDRV_PCM_IOCTL_HW_PARAMS,
			(void *)&hw_params);
	if (err < 0) {
		pr_info("%s hostless pcm hw params fail err:%d\n",
			__func__, err);
		return err;
	}

	memset(&sw_params, 0, sizeof(sw_params));
	sw_params.start_threshold = 1;
	sw_params.stop_threshold = period_size * period_count * 10;
	sw_params.avail_min = 1;
	sw_params.xfer_align = period_size / 2;
	sw_params.silence_threshold = 0;
	sw_params.silence_size = 0;
	sw_params.boundary = period_size * period_count;
	err = snd_pcm_kernel_ioctl(substream,
			SNDRV_PCM_IOCTL_SW_PARAMS,
			(void *)&sw_params);
	if (err < 0) {
		pr_info("%s hostless pcm sw params fail err:%d\n",
			__func__, err);
		return err;
	}

	return err;
}

int mt8168_adsp_hostless_pcm_start(struct snd_pcm_substream *substream)
{
	int err;

	err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_PREPARE, NULL);
	if (err < 0) {
		pr_info("%s hostless pcm prepare fail err:%d\n",
			__func__, err);
		return err;
	}

	err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_START, NULL);
	if (err < 0) {
		pr_info("%s hostless pcm start fail err:%d\n",
			__func__, err);
		return err;
	}

	return err;
}

int mt8168_adsp_hostless_pcm_stop(struct snd_pcm_substream *substream)
{
	int err;

	err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_DROP, NULL);
	if (err < 0) {
		pr_info("%s hostless pcm stop fail err:%d\n",
			__func__, err);
		return err;
	}

	return err;
}
