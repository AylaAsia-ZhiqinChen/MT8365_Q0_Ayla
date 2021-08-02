/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "logger.h"
#include <stdbool.h>
#include "irq.h"

#ifndef PLATSEC
/* default not to declare at specific section */
#define PLATSEC
#endif
static unsigned int logger_dram_inited PLATSEC;
static struct dram_ctrl_s logger_dram_ctrl PLATSEC;
static unsigned int logger_wakeup_ap PLATSEC;	     /* wake up ap flag */
#if LOGGER_BUFFERABLE
unsigned int logger_w_pos PLATSEC;	     /* next position to write */
unsigned int logger_r_pos PLATSEC;	     /* last position to read */
unsigned char logger_buf[BUF_LEN] PLATSEC;
volatile int logger_data_move_flag PLATSEC;	     /* logger data move flag,
						      * 0: data need move,
						      * 1: data moving */
#endif

/* Implement these functions by platform, even do nothing
 * logger_update_post_process() - any post process after logger updated
 * logger_wake_lock/unlock() - lock/unlock tinsys to avoid sleep
 * logger_update_dram_ready_time() - return dram ready time if dram is disabled
 * logger_addr_remap() - address remap to tinysys view
 * logger_addr_unremap() - drop remap control if needed
 * logger_update_notify_ap() - send a notification to awake ap
 * logger_ipi_registration() - ipi usage may different between tinysys
 * logger_update_dma() - implement DMA operation, return transaction length
 * logger_enter/exit_critical() - enter/exit critical section by platform
 * logger_dcache_flush/invalidate() - data cache operation
 */
int logger_update_notify_ap(void);
#if LOGGER_BUFFERABLE
unsigned int logger_update_dma(unsigned int src_buf,
			       unsigned int src_r_pos, unsigned int dst_buf,
			       unsigned int dst_w_pos, unsigned int src_len,
			       unsigned int dst_len);
#endif
unsigned int logger_update_dram_ready_time(void);
unsigned int logger_addr_remap(unsigned int addr);
void logger_addr_unremap(unsigned int addr);
void logger_ipi_registration(LAST_LOG_INFO logger_info);
void logger_update_post_process(void);
void logger_wake_lock(void);
void logger_wake_unlock(void);
void logger_dcache_flush(void *addr, uint32_t size);
void logger_dcache_invalidate(void *addr, uint32_t size);
void logger_enter_critical(void);
void logger_exit_critical(void);


static BaseType_t _LoggerTimerPendFunction(PendedFunction_t xFunctionToPend,
					   TickType_t xTicksToWait)
{
	BaseType_t ret = pdFALSE;

	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED &&
	    xTaskGetSchedulerState() != taskSCHEDULER_SUSPENDED) {
		if (is_in_isr() || xGetCriticalNesting() > 0)
			ret = xTimerPendFunctionCallFromISR(xFunctionToPend,
							    NULL, 0, NULL);
		else
			ret = xTimerPendFunctionCall(xFunctionToPend,
						     NULL, 0, xTicksToWait);
	}

	return ret;
}

/* should be called in either critical section or ISR (non-preemptable) */
void logger_puts(const char *str, int length)
{
	if (length <= 0)
		return;

#if !LOGGER_BUFFERABLE
	/* If there's no bufferable region for logger, update directly */
	logger_update(str, length);
#else
	unsigned int update_w_pos, update_r_pos;
	unsigned int wrap_length, r_need_update;
	BaseType_t ret;

	if (length >= BUF_LEN)
		return;

	logger_dcache_invalidate(&logger_w_pos, sizeof(unsigned int));
	logger_dcache_invalidate(&logger_r_pos, sizeof(unsigned int));

	update_w_pos = logger_w_pos + length;
	update_r_pos = logger_r_pos;

	wrap_length = 0;
	r_need_update = 0;

	if (update_w_pos >= BUF_LEN) {
		update_w_pos -= BUF_LEN;

		wrap_length = update_w_pos;
		length -= wrap_length;

		if (logger_w_pos < logger_r_pos || update_w_pos >= update_r_pos)
			r_need_update = 1;
	} else if (logger_w_pos < logger_r_pos && update_w_pos > update_r_pos) {
		r_need_update = 1;
	}

	if (r_need_update) {
		update_r_pos = update_w_pos + 1;

		if (update_r_pos >= BUF_LEN)
			update_r_pos -= BUF_LEN;
	}

	if (length) {
		memcpy(&logger_buf[logger_w_pos], str, length);
		logger_dcache_flush(&logger_buf[logger_w_pos], length);
	}

	if (wrap_length) {
		memcpy(&logger_buf, str + length, wrap_length);
		logger_dcache_flush(&logger_buf, wrap_length);
	}

	logger_w_pos = update_w_pos;
	logger_r_pos = update_r_pos;
	logger_dcache_flush(&logger_w_pos, sizeof(unsigned int));
	logger_dcache_flush(&logger_r_pos, sizeof(unsigned int));

	/* verify and send a timer to move logger */
	unsigned int buffer_length;

	if (logger_w_pos >= logger_r_pos)
		buffer_length = logger_w_pos - logger_r_pos;
	else
		buffer_length = BUF_LEN - (logger_r_pos - logger_w_pos);

	/* if buffer_length >= (BUF_LEN/2), move log to dram */
	logger_dcache_invalidate(&logger_dram_ctrl.enable, sizeof(unsigned int));
	logger_dcache_invalidate((int*)&logger_data_move_flag, sizeof(int));
	if (!logger_dram_ctrl.enable)
		return;

	if (((BUF_LEN >> 1) <= buffer_length)
	    && (logger_data_move_flag < LOGGER_MAX_QUEUE)) {
		ret =
		    _LoggerTimerPendFunction((PendedFunction_t) logger_update,
					     10);
		if (ret == pdPASS) {
			logger_data_move_flag++;
			logger_dcache_flush((int*)&logger_data_move_flag, sizeof(int));
		}
	}
#endif

}

void logger_update(const char *str, unsigned int src_len)
{
#if LOGGER_BUFFERABLE
	unsigned int len = src_len;
	unsigned int buffer_length_now;
#else
	unsigned int wrap_len;
#endif
	unsigned int dst_base, dst_buf, dst_w_pos, dst_r_pos;
	unsigned int dst_len, dst_sz;
	unsigned int ready_time;
	struct buffer_info_s *ctrl;
	BaseType_t ret;

#if LOGGER_BUFFERABLE
	/* clear data move flag */
	logger_data_move_flag = 0;
	logger_dcache_flush((int*)&logger_data_move_flag, sizeof(int));
#endif

	logger_dcache_invalidate(&logger_dram_ctrl.enable, sizeof(unsigned int));
	logger_dcache_invalidate(&logger_dram_inited, sizeof(unsigned int));
	if (!logger_dram_ctrl.enable || !logger_dram_inited)
		return;

	ready_time = logger_update_dram_ready_time();
	/* if there's need some time to enable dram, update logger later */
	if (ready_time) {
		ret =
		    _LoggerTimerPendFunction((PendedFunction_t) logger_update,
					     ready_time);
#if LOGGER_BUFFERABLE
		if (ret == pdTRUE) {
			logger_data_move_flag = 1;
			logger_dcache_flush((int*)&logger_data_move_flag, sizeof(int));
		}
#endif
		return;
	}

	dst_base = logger_addr_remap(logger_dram_ctrl.base);

	logger_enter_critical();

#if LOGGER_BUFFERABLE
	/* verify logger length avoid dummy move */
	logger_dcache_invalidate(&logger_w_pos, sizeof(unsigned int));
	logger_dcache_invalidate(&logger_r_pos, sizeof(unsigned int));

	if (logger_w_pos >= logger_r_pos)
		buffer_length_now = logger_w_pos - logger_r_pos;
	else
		buffer_length_now = BUF_LEN - (logger_r_pos - logger_w_pos);
	if ((BUF_LEN >> 1) > buffer_length_now)
		goto exit;
#endif


	ctrl = (struct buffer_info_s *) (dst_base + logger_dram_ctrl.info_ofs);
	dst_buf = dst_base + logger_dram_ctrl.buff_ofs;
	dst_sz = logger_dram_ctrl.buff_size;

	logger_dcache_invalidate((void *)ctrl, sizeof(*ctrl));

	dst_w_pos = ctrl->w_pos;
	dst_r_pos = ctrl->r_pos;

	if (dst_w_pos >= logger_dram_ctrl.buff_size
	    || dst_r_pos >= logger_dram_ctrl.buff_size) {
		printf("%s:err r=%u w=%u l=%u\n", __FUNCTION__, dst_w_pos,
			 dst_r_pos, logger_dram_ctrl.buff_size);
		logger_dram_inited = 0;
		goto exit;
	}

	/* dst resource */
	if (dst_w_pos > dst_r_pos)
		dst_len = dst_sz - dst_w_pos + dst_r_pos - 1;
	else if (dst_w_pos < dst_r_pos)
		dst_len = dst_r_pos - dst_w_pos - 1;
	else
		dst_len = dst_sz - 1;

	/* src resource */
#if LOGGER_BUFFERABLE
	/* caculate src length with r/w position */
	if (logger_w_pos == logger_r_pos) {
		printf("logger_w_pos == logger_r_pos\n");
		goto exit;
	}

	if (logger_w_pos > logger_r_pos)
		src_len = logger_w_pos - logger_r_pos;
	else
		src_len = BUF_LEN - logger_r_pos;
#else
	if (src_len > dst_len)
		src_len = dst_len;

#endif
	if (src_len != 0) {
		if ((dst_w_pos + src_len) > dst_sz) {
#if !LOGGER_BUFFERABLE
			wrap_len = dst_w_pos + src_len - dst_sz;
#endif
			src_len = dst_sz - dst_w_pos;
		}
	}

	if (dst_len) {
		/* dram full and drop all */
		if (src_len > dst_len)
			src_len = dst_len;

		/* when DRAM log buffer over dram buff length/4,
		 * send a buffer full notification to wakeup ap
		 */
		if (logger_wakeup_ap &&
			((logger_dram_ctrl.buff_size >> 2) >= dst_len))
			_LoggerTimerPendFunction((PendedFunction_t)
						  logger_update_notify_ap, 10);
#if LOGGER_BUFFERABLE
		/* when LOGGER_BUFFER enabled, be sured that moving log via
		 * dma to save cpu resource
		 */
		len =
		    logger_update_dma((unsigned int) &logger_buf[0],
						      logger_r_pos,
						      dst_buf, dst_w_pos,
						      src_len, dst_len);
		/* len should be returned with two information,
		 * [30:31] the length which is skipped for 4-Byte alignment
		 * [ 0:29] the length which is copied to dram by dma (max 1GB)
		 */
		src_len = (len & 0x3FFFFFFF);
#else
		memcpy((void *)(dst_buf + dst_w_pos), str, src_len);
		logger_dcache_flush((void *)(dst_buf + dst_w_pos), src_len);

		if (wrap_len) {
			memcpy((void *)(dst_buf), str + src_len, wrap_len);
			logger_dcache_flush((void *)(dst_buf), wrap_len);
			/* restore src_len to original length for dst_w_pos moving */
			src_len += wrap_len;
		}
#endif
	} else
		goto exit;

	dst_w_pos += src_len;

	if (dst_w_pos >= dst_sz)
		dst_w_pos -= dst_sz;

#if LOGGER_BUFFERABLE
	/* len should be returned with two information,
	 * [30:31] the length which is skipped for 4-Byte alignment
	 * [ 0:29] the length which is copied to dram by dma (max 1GB)
	 */
	src_len = src_len + (len >> 30);
	if (src_len) {
		logger_r_pos += src_len;

		if (logger_r_pos >= BUF_LEN)
			logger_r_pos -= BUF_LEN;
	} else
		goto exit;

	logger_dcache_flush(&logger_r_pos, sizeof(unsigned int));
#endif
	/* update dram write pointer */
	ctrl->w_pos = dst_w_pos;

	logger_dcache_flush(&ctrl->w_pos, sizeof(unsigned int));
      exit:
	logger_exit_critical();
	logger_addr_unremap(dst_base);
	logger_update_post_process();
}

unsigned int logger_dram_register(unsigned int dram_ctrl_phy,
				  unsigned int limit)
{
	unsigned int addr;
	struct dram_ctrl_s *ctrl;
	unsigned int ofs, size;

	if (!logger_dram_inited) {
		/* sync dram address */
		addr = logger_addr_remap(dram_ctrl_phy);

		ctrl = (struct dram_ctrl_s *) addr;

		if (ctrl->base != PLT_LOG_ENABLE)
			goto abandon;

		if (ctrl->size != sizeof (*ctrl))
			goto abandon;

		ofs = ctrl->info_ofs;

		if ((ofs + sizeof (struct buffer_info_s)) > limit)
			goto abandon;

		logger_dram_ctrl.info_ofs = ofs;
		ofs = ctrl->buff_ofs;
		size = ctrl->buff_size;

		if ((ofs + size) > limit)
			goto abandon;

		logger_dram_ctrl.buff_ofs = ofs;
		logger_dram_ctrl.buff_size = size;

		/* enable logger by mobile log */
		logger_dram_ctrl.base = dram_ctrl_phy;
		logger_dram_inited = 1;
		/* already get AP side DRAM info. and release wakelock */
		logger_wake_unlock();
      abandon:
		logger_addr_unremap(addr);
	}

	return logger_dram_inited;
}

/* should be called in either critical section or ISR (non-preemptable) */
unsigned int logger_update_enable(unsigned int enable)
{
	unsigned int on_logger = enable & (LOGGER_ON_CTRL_BIT);
	unsigned int func_sel = enable & (~LOGGER_ON_CTRL_BIT);

	if (on_logger)
		logger_dram_ctrl.enable |= (func_sel);
	else
		logger_dram_ctrl.enable &= (~func_sel);

	return logger_dram_ctrl.enable;
}

/* should be called in either critical section or ISR (non-preemptable) */
unsigned int logger_update_wakeup_ap(unsigned int enable)
{
	enable = (enable) ? 1 : 0;
	logger_wakeup_ap = enable;

	return logger_wakeup_ap;
}

#if LOGGER_BOOTLOG_SUPPORT
static void dram_off_timer(TimerHandle_t xTimer)
{
	logger_enter_critical();
	logger_update_enable(LOGGER_DRAM_OFF);
	logger_exit_critical();
}

static void wait_logger_init_done(void)
{
	static TimerHandle_t logger_timer;
	unsigned int dram_ctrl_info[2];
#ifdef CFG_XGPT_SUPPORT
	unsigned long long timeout =
	    read_xgpt_stamp_ns() + LOGGER_WAIT_TIMEOUT_NS;
#endif

	while (1) {
		/* wait until AP set it */
		if (logger_dram_ctrl.base != 0)
			break;
#ifdef CFG_XGPT_SUPPORT
		if (read_xgpt_stamp_ns() > timeout) {
			printf("wait logger timeout!\n");
			goto _wait_logger_init_end;
		}
#endif
	}

	dram_ctrl_info[0] = logger_dram_ctrl.base;
	dram_ctrl_info[1] = DRAM_BUF_LEN;
	logger_init_handler(0, NULL, dram_ctrl_info);
	logger_update_enable(LOGGER_DRAM_ON);
	/* setup a timer to disable dram useage */
	logger_timer =
	    xTimerCreate("LoggerTimer", pdMS_TO_TICKS(LOGGER_DRAM_OFF_TIME),
			 pdFALSE, (void *) 0, dram_off_timer);
	xTimerStart(logger_timer, 0);

      _wait_logger_init_end:
	printf("Logger init done\n");
}
#endif

void logger_init(void)
{
	LAST_LOG_INFO logger_info;

	logger_wake_lock();

	/* config initial */
	logger_dram_inited = 0;
	logger_wakeup_ap = 0;
#if LOGGER_BUFFERABLE
	logger_data_move_flag = 0;
#endif

	/* init logger structure and sync to AP side */
	logger_info.log_dram_addr = (unsigned int) &logger_dram_ctrl;
#if LOGGER_BUFFERABLE
	logger_info.log_buf_addr = (unsigned int) &logger_buf;
	logger_info.log_start_addr = (unsigned int) &logger_r_pos;
	logger_info.log_end_addr = (unsigned int) &logger_w_pos;
	logger_info.log_buf_maxlen = (unsigned int) BUF_LEN;
#endif

	logger_ipi_registration(logger_info);

#if LOGGER_BOOTLOG_SUPPORT
	/* wait logger ready */
	wait_logger_init_done();
#endif
}
