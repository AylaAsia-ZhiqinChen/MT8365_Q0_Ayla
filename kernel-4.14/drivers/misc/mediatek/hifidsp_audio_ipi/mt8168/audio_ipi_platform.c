/*
 * Copyright (C) 2018 MediaTek Inc.
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

#include <audio_ipi_platform.h>

#include <linux/printk.h>
#include <linux/bug.h>
#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
#include <adsp_ipi.h>
#include <adsp_ipi_platform.h>
#endif

#include <audio_task.h>


bool audio_opendsp_id_ready(const uint8_t opendsp_id)
{
	bool ret = false;

	switch (opendsp_id) {
	case AUDIO_OPENDSP_USE_HIFI4:
#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
		ret = (is_adsp_ready(ADSP_CORE_0_ID) != 0);
#else
		pr_notice("%s(), opendsp_id %u not build!!\n",
			  __func__, opendsp_id);
		ret = false;
		WARN_ON(1);
#endif
		break;
	default:
		pr_notice("%s(), opendsp_id %u not support!!\n",
			  __func__, opendsp_id);
		WARN_ON(1);
	}

	return ret;
}


bool audio_opendsp_ready(const uint8_t task)
{
	return audio_opendsp_id_ready(audio_get_opendsp_id(task));
}


uint32_t audio_get_opendsp_id(const uint8_t task)
{
	uint32_t opendsp_id = AUDIO_OPENDSP_ID_INVALID;

	switch (task) {
	case TASK_SCENE_AUDIO_CONTROLLER:
		/* FALLTHROUGH */
	case TASK_SCENE_VA:
		opendsp_id = AUDIO_OPENDSP_USE_HIFI4;
		break;
	default:
		pr_notice("%s(), task %d not support!!\n",
			  __func__, task);
		opendsp_id = AUDIO_OPENDSP_ID_INVALID;
		WARN_ON(1);
	}

	return opendsp_id;
}


uint32_t audio_get_ipi_id(const uint8_t task)
{
	uint32_t opendsp_id = audio_get_opendsp_id(task);
	uint32_t ipi_id = 0xFFFFFFFF;

	switch (opendsp_id) {
	case AUDIO_OPENDSP_USE_HIFI4:
#ifdef CONFIG_MTK_HIFIXDSP_SUPPORT
		ipi_id = ADSP_IPI_AUDIO;
#else
		pr_notice("%s(), opendsp_id %u task %d not build!!\n",
			  __func__, opendsp_id, task);
		ipi_id = 0xFFFFFFFF;
		WARN_ON(1);
#endif
		break;
	default:
		pr_notice("%s(), opendsp_id %u task %d not support!!\n",
			  __func__, opendsp_id, task);
		WARN_ON(1);
		ipi_id = 0xFFFFFFFF;
	}

	return ipi_id;
}



