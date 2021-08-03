/*
 * Copyright (C) 2018 MediaTek Inc.
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

#include <linux/io.h>
#include "adsp_reg.h"
#include "mtk_hifixdsp_common.h"


#define ADSP_HW_SEMA_HAVE_GOT    (1)
#define ADSP_HW_SEMA_OK          (0)
#define ADSP_HW_SEMA_INVALID     (-1)
#define ADSP_HW_SEMA_TIMEOUT     (-2)
#define ADSP_HW_SEMA_FREE_ERR    (-3)

static unsigned int hw_sema_ref_count[ADSP_HW_SEMA_MAX] = {0};

/*
 * Get HW semaphore
 * @param sema_id: semaphore id
 * @param timeout: 0 - no timeout; other value -retry count
 * @return
 *     - ADSP_HW_SEMA_OK: Success
 *     - ADSP_HW_SEMA_HAVE_GOT: this HW semaphore has been held
 *     - ADSP_HW_SEMA_INVALID: Invalid HW semaphore ID
 *     - ADSP_HW_SEMA_TIMEOUT: timeout
 */
int hw_semaphore_get(enum ADSP_HW_SEMAPHORE_ID sema_id,
			unsigned int timeout)
{
	void __iomem *sema_reg = DSP_RG_SEMAPHORE00 + (sema_id << 2);
	unsigned int try_cnt = 0;

	if (sema_id >= ADSP_HW_SEMA_MAX)
		return ADSP_HW_SEMA_INVALID;

	if (readl(sema_reg) == 0x1) {
		hw_sema_ref_count[sema_id]++;
		return ADSP_HW_SEMA_HAVE_GOT;
	}

	while (1) {
		writel(0x1, sema_reg);
		if (readl(sema_reg) == 0x1) {
			hw_sema_ref_count[sema_id]++;
			break;
		}

		if (timeout > 0 && (try_cnt++ >= timeout))
			return ADSP_HW_SEMA_TIMEOUT;
	}
	return ADSP_HW_SEMA_OK;
}

/*
 * Release HW semaphore
 * @param sema_id: semaphore id
 * @return
 *     - ADSP_HW_SEMA_OK: Success
 *     - ADSP_HW_SEMA_INVALID: Invalid HW semaphore ID
 *     - ADSP_HW_SEMA_HAVE_GOT: this HW semaphore has been held
 *     - ADSP_HW_SEMA_FREE_ERR: Free Hw semaphore  error
 */
int hw_semaphore_release(enum ADSP_HW_SEMAPHORE_ID sema_id)
{
	void __iomem *sema_reg = DSP_RG_SEMAPHORE00 + (sema_id << 2);

	if (sema_id >= ADSP_HW_SEMA_MAX)
		return ADSP_HW_SEMA_INVALID;

	if (hw_sema_ref_count[sema_id] > 1) {
		hw_sema_ref_count[sema_id]--;
		return ADSP_HW_SEMA_HAVE_GOT;
	}

	if (readl(sema_reg) == 0x1) {
		writel(0x1, sema_reg);
		hw_sema_ref_count[sema_id] = 0;
		return ADSP_HW_SEMA_OK;
	}

	return ADSP_HW_SEMA_FREE_ERR;
}

