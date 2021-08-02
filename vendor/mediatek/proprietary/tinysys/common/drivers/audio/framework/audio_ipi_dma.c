/*
 * Copyright (C) 2016 MediaTek Inc.
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

#include <audio_ipi_dma.h>

#include <stdbool.h>
#include <string.h>


#include <errno.h>

#include <tinysys_config.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <interrupt.h>

#include <dma.h>
#include <hal_cache.h>

#define ADSP_DVFS_NOT_READY

#ifdef ADSP_DVFS_NOT_READY
#define dvfs_enable_DRAM_resource(...)
#define dvfs_disable_DRAM_resource(...)
#else
#include <dvfs.h>
#endif

#include <audio_log.h>
#include <audio_assert.h>

#include <audio_task.h>
#include <audio_controller_msg_id.h>
#include <audio_messenger_ipi.h>




/*
 * =============================================================================
 *                     MACRO
 * =============================================================================
 */

#define MAX_SIZE_OF_ONE_FRAME (16) /* 32-bits * 4ch */

/* auidio dsp cache limitation: 128 byte alignment */
#define ADSP_CACHE_ALIGN_ORDER (7)
#define ADSP_CACHE_ALIGN_BYTES (1 << (ADSP_CACHE_ALIGN_ORDER))
#define ADSP_CACHE_ALIGN_MASK  ((ADSP_CACHE_ALIGN_BYTES) - 1)

#define DO_BYTE_ALIGN(value, mask) (((value) + mask) & (~mask))


/*
 * =============================================================================
 *                     log
 * =============================================================================
 */

#ifdef ipi_dbg
#undef ipi_dbg
#endif

#if 0
#define ipi_dbg(x...) pr_info(x)
#else
#define ipi_dbg(x...)
#endif

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) "[DMA] %s(), " fmt "\n", __func__


/*
 * =============================================================================
 *                     struct def
 * =============================================================================
 */

/* DMA */
struct audio_region_t {
	uint32_t offset;        /* ex: 0x1000 */
	uint32_t size;          /* ex: 0x100 */
	uint8_t  resv_128[120];

	uint32_t read_idx;      /* ex: 0x0 ~ 0xFF */
	uint8_t  resv_256[124];

	uint32_t write_idx;     /* ex: 0x0 ~ 0xFF */
	uint8_t  resv_384[124];
};


struct audio_ipi_dma_t {
	struct aud_ptr_t base_phy;
	struct aud_ptr_t base_vir;

	uint32_t size;
	uint8_t  resv_128[108];

	struct audio_region_t region[TASK_SCENE_SIZE][NUM_AUDIO_IPI_DMA_PATH];
	uint32_t checksum;

	/* the beginning of pool data: regions allocated from pool_offset */
	uint32_t pool_offset;
};



/*
 * =============================================================================
 *                     global var
 * =============================================================================
 */

static struct audio_ipi_dma_t *g_dma;

SemaphoreHandle_t g_aud_dma_lock;


/*
 * =============================================================================
 *                     utilities
 * =============================================================================
 */

#define CACHE_FLUSH(addr, size) \
	hal_cache_region_operation(HAL_CACHE_TYPE_DATA, \
				   HAL_CACHE_OPERATION_FLUSH, \
				   (void *)(addr), \
				   (size));


#define CACHE_INVALIDATE(addr, size) \
	hal_cache_region_operation(HAL_CACHE_TYPE_DATA, \
				   HAL_CACHE_OPERATION_INVALIDATE, \
				   (void *)(addr), \
				   (size));


#define AUD_DMA_READ(scp_buf, dram_addr, size) \
	do { \
		if (is_in_isr()) { \
			pr_notice("is_in_isr()"); \
		} else { \
			if (xSemaphoreTake(g_aud_dma_lock, portMAX_DELAY) != pdTRUE) { \
				pr_notice("xSemaphoreTake fail"); \
			} else { \
				if ((((uint32_t)scp_buf) & 0x7F) != 0) { \
					pr_notice("%p fail", scp_buf); \
					AUD_WARNING("cache not align!!"); \
				} \
				CACHE_FLUSH((scp_buf), (size)); /* for heap buf info */ \
				scp_dma_transaction((uint32_t)(scp_buf), \
						    (uint32_t)(dram_addr), \
						    (size), \
						    OPENDSP_DMA_ID, \
						    NO_RESERVED); \
				CACHE_INVALIDATE((scp_buf), (size)); \
				xSemaphoreGive(g_aud_dma_lock); \
			} \
		} \
	} while (0)


#define AUD_DMA_WRITE(dram_addr, scp_buf, size) \
	do { \
		if (is_in_isr()) { \
			pr_notice("is_in_isr()"); \
		} else { \
			if (xSemaphoreTake(g_aud_dma_lock, portMAX_DELAY) != pdTRUE) { \
				pr_notice("xSemaphoreTake fail"); \
			} else { \
				if ((((uint32_t)scp_buf) & 0x7F) != 0) { \
					pr_notice("%p fail", scp_buf); \
					AUD_WARNING("cache not align!!"); \
				} \
				CACHE_FLUSH((scp_buf), (size)); \
				scp_dma_transaction((uint32_t)(dram_addr), \
						    (uint32_t)(scp_buf), \
						    (size), \
						    OPENDSP_DMA_ID, \
						    NO_RESERVED); \
				xSemaphoreGive(g_aud_dma_lock); \
			} \
		} \
	} while (0)


#define DUMP_REGION(LOG_F, description, p_region, count) \
	do { \
		if (p_region && description) { \
			LOG_F("%s, offset: 0x%x, size: 0x%x" \
			      ", read_idx: 0x%x, write_idx: 0x%x" \
			      ", region_data_count: 0x%x, count: %u", \
			      description, \
			      (p_region)->offset, \
			      (p_region)->size, \
			      (p_region)->read_idx, \
			      (p_region)->write_idx, \
			      audio_region_data_count(p_region), \
			      count); \
		} else { \
			pr_notice("%uL, %p %p", \
				  __LINE__, p_region, description); \
		} \
	} while (0)



/*
 * =============================================================================
 *                     init
 * =============================================================================
 */

int init_audio_ipi_dma(struct ipi_msg_t *msg)
{
	uint32_t checksum = 0;

	if (!msg || msg->msg_id != AUD_CTL_MSG_A2D_DMA_INIT || msg->param1 == 0) {
		DUMP_IPI_MSG("err", msg);
		return -EFAULT;
	}

	if (g_dma != NULL) {
		pr_info("already init. dma %p, phy %p/%lx, sz 0x%x, checksum %u/%u, offset %u",
			g_dma,
			g_dma->base_phy.addr,
			g_dma->base_phy.addr_val,
			g_dma->size,
			checksum,
			g_dma->checksum,
			g_dma->pool_offset);
		return 0;
	}

	g_dma = (struct audio_ipi_dma_t *)ap_to_scp(msg->param1);
	if (!g_dma)
		return -EFAULT;

	dvfs_enable_DRAM_resource(OPENDSP_MEM_ID);

	AUD_ASSERT(g_dma->size == msg->param2);

	checksum = (uint8_t *)(&g_dma->checksum) - (uint8_t *)g_dma;
	AUD_ASSERT(g_dma->checksum == checksum);

	pr_info("dma %p, phy %p/0x%lx, sz 0x%x, checksum %u/%u, offset %u",
		g_dma,
		g_dma->base_phy.addr,
		g_dma->base_phy.addr_val,
		g_dma->size,
		checksum,
		g_dma->checksum,
		g_dma->pool_offset);

	dvfs_disable_DRAM_resource(OPENDSP_MEM_ID);

	g_aud_dma_lock = xSemaphoreCreateMutex();

	return 0;
}


/*
 * =============================================================================
 *                     alloc/free region
 * =============================================================================
 */

int audio_ipi_dma_update_region_info(struct ipi_msg_t *msg)
{
	uint8_t task = TASK_SCENE_INVALID;
	struct audio_region_t *region = NULL;

	int i = 0;

	if (g_dma == NULL) {
		pr_notice("dma NULL!! return");
		return -EFAULT;
	}

	if (!msg ||
	    msg->msg_id != AUD_CTL_MSG_A2D_DMA_UPDATE_REGION ||
	    msg->param1 >= TASK_SCENE_SIZE) {
		DUMP_IPI_MSG("err", msg);
		return -EFAULT;
	}

	//DUMP_IPI_MSG("update region", msg);

	task = msg->param1;

	for (i = 0; i < NUM_AUDIO_IPI_DMA_PATH; i++) {
		region = &g_dma->region[task][i];
		CACHE_INVALIDATE(region, sizeof(region));
		ipi_dbg("task %u, i %d, offset: 0x%x, size: 0x%x, read_idx : 0x%x, write_idx: 0x%x",
			task, i, region->offset, region->size,
			region->read_idx, region->write_idx);
	}

	return 0;
}



/*
 * =============================================================================
 *                     region
 * =============================================================================
 */

static uint32_t audio_region_data_count(struct audio_region_t *region)
{
	uint32_t count = 0;

	if (!region)
		return 0;

	if (region->size == 0) {
		DUMP_REGION(pr_notice, "size fail", region, count);
		return 0;
	}

	if (region->read_idx >= region->size) {
		DUMP_REGION(pr_notice, "read_idx fail", region, count);
		region->read_idx %= region->size;
	} else if (region->write_idx >= region->size) {
		DUMP_REGION(pr_notice, "write_idx fail", region, count);
		region->write_idx %= region->size;
	}

	if (region->write_idx >= region->read_idx)
		count = region->write_idx - region->read_idx;

	else
		count = region->size - (region->read_idx - region->write_idx);

	return count;
}


static uint32_t audio_region_free_space(struct audio_region_t *region)
{
	uint32_t count = 0;

	if (!region)
		return 0;

	count = region->size - audio_region_data_count(region);

	if (count >= MAX_SIZE_OF_ONE_FRAME)
		count -= MAX_SIZE_OF_ONE_FRAME;

	else
		count = 0;

	return count;
}


static int audio_region_write_from_linear(
	struct audio_region_t *region,
	const void *linear_buf,
	uint32_t count)
{
	uint32_t count_align = DO_BYTE_ALIGN(count, ADSP_CACHE_ALIGN_MASK);

	uint32_t free_space = 0;
	uint8_t *base = NULL;
	uint32_t w2e = 0;

	if (!region || !linear_buf || !g_dma)
		return -EFAULT;

	if (region->size == 0) {
		DUMP_REGION(pr_notice, "size fail", region, count);
		return -ENODEV;
	}

	if (region->read_idx >= region->size) {
		DUMP_REGION(pr_notice, "read_idx fail", region, count);
		region->read_idx %= region->size;
	}
	if (region->write_idx >= region->size) {
		DUMP_REGION(pr_notice, "write_idx fail", region, count);
		region->write_idx %= region->size;
	}


	DUMP_REGION(ipi_dbg, "in", region, count);

	free_space = audio_region_free_space(region);
	if (free_space < count_align) {
		DUMP_REGION(pr_notice, "free_space < count_align", region, count);
		return -EOVERFLOW;
	}

	base = (uint8_t *)g_dma + region->offset;

	if (region->read_idx <= region->write_idx) {
		w2e = region->size - region->write_idx;
		if (count_align <= w2e) {
			AUD_DMA_WRITE(base + region->write_idx, linear_buf, count);
			region->write_idx += count_align;
			if (region->write_idx == region->size)
				region->write_idx = 0;
		} else {
			AUD_DMA_WRITE(base + region->write_idx, linear_buf, w2e);
			AUD_DMA_WRITE(base, (uint8_t *)linear_buf + w2e, count - w2e);
			region->write_idx = count_align - w2e;
		}
	} else {
		AUD_DMA_WRITE(base + region->write_idx, linear_buf, count);
		region->write_idx += count_align;
	}


	DUMP_REGION(ipi_dbg, "out", region, count);

	return 0;
}


static int audio_region_read_to_linear(
	void *linear_buf,
	struct audio_region_t *region,
	uint32_t count)
{
	uint32_t count_align = DO_BYTE_ALIGN(count, ADSP_CACHE_ALIGN_MASK);

	uint32_t available_count = 0;
	uint8_t *base = NULL;
	uint32_t r2e = 0;

	if (!region || !linear_buf || !g_dma)
		return -EFAULT;

	if (region->size == 0) {
		DUMP_REGION(pr_notice, "size fail", region, count);
		return -ENODEV;
	}

	if (region->read_idx >= region->size) {
		DUMP_REGION(pr_notice, "read_idx fail", region, count);
		region->read_idx %= region->size;
	}
	if (region->write_idx >= region->size) {
		DUMP_REGION(pr_notice, "write_idx fail", region, count);
		region->write_idx %= region->size;
	}


	DUMP_REGION(ipi_dbg, "in", region, count);

	available_count = audio_region_data_count(region);
	if (count_align > available_count) {
		DUMP_REGION(pr_notice, "count_align > available_count", region, count);
		return -ENOMEM;
	}

	base = (uint8_t *)g_dma + region->offset;

	if (region->read_idx <= region->write_idx) {
		AUD_DMA_READ(linear_buf, base + region->read_idx, count);
		region->read_idx += count_align;
	} else {
		r2e = region->size - region->read_idx;
		if (r2e >= count_align) {
			AUD_DMA_READ(linear_buf, base + region->read_idx, count);
			region->read_idx += count_align;
			if (region->read_idx == region->size)
				region->read_idx = 0;
		} else {
			AUD_DMA_READ(linear_buf, base + region->read_idx, r2e);
			AUD_DMA_READ((uint8_t *)linear_buf + r2e, base, count - r2e);
			region->read_idx = count_align - r2e;
		}
	}

	DUMP_REGION(ipi_dbg, "out", region, count);

	return 0;
}


static int audio_region_drop(
	struct audio_region_t *region,
	uint32_t count)
{
	uint32_t count_align = DO_BYTE_ALIGN(count, ADSP_CACHE_ALIGN_MASK);

	uint32_t available_count = 0;
	uint8_t *base = NULL;
	uint32_t r2e = 0;

	if (!region || !g_dma)
		return -EFAULT;

	if (region->size == 0) {
		DUMP_REGION(pr_notice, "size fail", region, count);
		return -ENODEV;
	}

	if (region->read_idx >= region->size) {
		DUMP_REGION(pr_notice, "read_idx fail", region, count);
		region->read_idx %= region->size;
	}
	if (region->write_idx >= region->size) {
		DUMP_REGION(pr_notice, "write_idx fail", region, count);
		region->write_idx %= region->size;
	}


	DUMP_REGION(ipi_dbg, "in", region, count);

	available_count = audio_region_data_count(region);
	if (count_align > available_count) {
		DUMP_REGION(pr_notice, "count_align > available_count", region, count);
		return -ENOMEM;
	}

	base = (uint8_t *)g_dma + region->offset;

	if (region->read_idx <= region->write_idx)
		region->read_idx += count_align;

	else {
		r2e = region->size - region->read_idx;
		if (r2e >= count_align) {
			region->read_idx += count_align;
			if (region->read_idx == region->size)
				region->read_idx = 0;
		} else
			region->read_idx = count_align - r2e;
	}

	DUMP_REGION(ipi_dbg, "out", region, count);

	return 0;
}


int audio_ipi_dma_write_region(const uint8_t task,
			       const void *data_buf,
			       uint32_t data_size,
			       uint32_t *write_idx)
{
	struct audio_region_t *region = NULL;

	int ret = 0;

	if (task >= TASK_SCENE_SIZE) {
		pr_info("task: %d", task);
		return -EOVERFLOW;
	}
	if (!data_buf || !write_idx || !g_dma) {
		pr_info("buf: %p, idx: %p, dma: %p NULL!!",
			data_buf, write_idx, g_dma);
		return -EFAULT;
	}
	if (data_size == 0) {
		pr_info("task: %d, data_size = 0", task);
		return -ENODATA;
	}

	dvfs_enable_DRAM_resource(OPENDSP_MEM_ID);

	region = &g_dma->region[task][AUDIO_IPI_DMA_SCP_TO_AP];
	CACHE_INVALIDATE(&region->read_idx, sizeof(uint32_t));
	DUMP_REGION(ipi_dbg, "region", region, data_size);

	/* keep the data index before write */
	*write_idx = region->write_idx;

	/* write data */
	ret = audio_region_write_from_linear(region, data_buf, data_size);
	CACHE_FLUSH(&region->write_idx, sizeof(uint32_t));

	dvfs_disable_DRAM_resource(OPENDSP_MEM_ID);

	return ret;
}


int audio_ipi_dma_read_region(const uint8_t task,
			      void *data_buf,
			      uint32_t data_size,
			      uint32_t read_idx)
{
	struct audio_region_t *region = NULL;

	int ret = 0;

	if (task >= TASK_SCENE_SIZE) {
		pr_info("task: %d", task);
		return -EOVERFLOW;
	}
	if (!data_buf || !g_dma) {
		pr_info("buf %p, dma %p NULL!!", data_buf, g_dma);
		return -EFAULT;
	}
	if (data_size == 0) {
		pr_info("task: %d, data_size = 0", task);
		return -ENODATA;
	}

	dvfs_enable_DRAM_resource(OPENDSP_MEM_ID);

	region = &g_dma->region[task][AUDIO_IPI_DMA_AP_TO_SCP];
	CACHE_INVALIDATE(&region->write_idx, sizeof(uint32_t));
	DUMP_REGION(ipi_dbg, "region", region, data_size);

	/* check read index */
	if (read_idx != region->read_idx) {
		pr_debug("read_idx 0x%x != region->read_idx 0x%x!!",
			 read_idx, region->read_idx);
		region->read_idx = read_idx;
	}

	/* read data */
	ret = audio_region_read_to_linear(data_buf, region, data_size);
	CACHE_FLUSH(&region->read_idx, sizeof(uint32_t));

	dvfs_disable_DRAM_resource(OPENDSP_MEM_ID);

	return ret;
}


int audio_ipi_dma_drop_region(const uint8_t task,
			      uint32_t drop_size,
			      uint32_t read_idx)

{
	struct audio_region_t *region = NULL;

	int ret = 0;

	if (task >= TASK_SCENE_SIZE) {
		pr_info("task: %d", task);
		return -EOVERFLOW;
	}
	if (!g_dma) {
		pr_info("dma %p NULL!!", g_dma);
		return -EFAULT;
	}
	if (drop_size == 0) {
		pr_info("task: %d, drop_size = 0", task);
		return -ENODATA;
	}

	dvfs_enable_DRAM_resource(OPENDSP_MEM_ID);

	region = &g_dma->region[task][AUDIO_IPI_DMA_AP_TO_SCP];
	CACHE_INVALIDATE(&region->write_idx, sizeof(uint32_t));
	DUMP_REGION(ipi_dbg, "region", region, drop_size);

	/* check read index */
	if (read_idx != region->read_idx) {
		pr_debug("read_idx 0x%x != region->read_idx 0x%x!!",
			 read_idx, region->read_idx);
		region->read_idx = read_idx;
	}

	/* drop data */
	ret = audio_region_drop(region, drop_size);
	CACHE_FLUSH(&region->read_idx, sizeof(uint32_t));

	dvfs_disable_DRAM_resource(OPENDSP_MEM_ID);

	return ret;
}


void audio_ipi_dma_read(void *scp_buf, void *dram_addr, uint32_t size)
{
	AUD_DMA_READ(scp_buf, dram_addr, size);
}

void audio_ipi_dma_write(void *dram_addr, void *scp_buf, uint32_t size)
{
	AUD_DMA_WRITE(dram_addr, scp_buf, size);
}



