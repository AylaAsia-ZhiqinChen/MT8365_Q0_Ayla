/*
 * mt8168-adsp-utils.h  --  Mediatek 8168 adsp utility
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

#ifndef _MT8168_ADSP_UTILS_H_
#define _MT8168_ADSP_UTILS_H_

#include "audio_task_manager.h"
#include "audio_shared_info.h"
#include "audio_memory.h"
#include <sound/pcm.h>

enum {
	MT8168_ADSP_FE_HOSTLESS_VA = 0,
	MT8168_ADSP_FE_VA,
	MT8168_ADSP_FE_MIC_RECORD,
	MT8168_ADSP_FE_CNT,
	MT8168_ADSP_BE_START = MT8168_ADSP_FE_CNT,
	MT8168_ADSP_BE_VUL2_IN = MT8168_ADSP_BE_START,
	MT8168_ADSP_BE_TDM_IN,
	MT8168_ADSP_BE_VUL_IN,
	MT8168_ADSP_BE_END,
	MT8168_ADSP_BE_CNT = MT8168_ADSP_BE_END - MT8168_ADSP_BE_START,
};

int mt8168_adsp_get_scene_by_dai_id(int id);

int mt8168_adsp_send_ipi_cmd(struct ipi_msg_t *p_msg,
			     uint8_t task_scene,
			     uint8_t target,
			     uint8_t data_type,
			     uint8_t ack_type,
			     uint16_t msg_id,
			     uint32_t param1,
			     uint32_t param2,
			     char *payload);

int mt8168_adsp_dai_id_pack(int dai_id);
int mt8168_adsp_hostless_pcm_open(struct snd_pcm *pcm,
			     struct snd_pcm_substream **substream);
void mt8168_adsp_hostless_pcm_close(struct snd_pcm_substream *substream);
int mt8168_adsp_hostless_pcm_params(struct snd_pcm_substream *substream,
			     unsigned int rate,
			     unsigned int channels,
			     unsigned int format,
			     unsigned int period_size,
			     unsigned int period_count);
int mt8168_adsp_hostless_pcm_start(struct snd_pcm_substream *substream);
int mt8168_adsp_hostless_pcm_stop(struct snd_pcm_substream *substream);
#endif
