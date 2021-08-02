/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 * Author: Garlic Tseng <garlic.tseng@mediatek.com>
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

#include "audio_rtos_header_group.h"
#include "audio_task.h"
#include "audio_task_io_buf_ctrl.h"

static const IO_BUF_CTRL_OPS g_io_ring_buf;

int io_ctrl_buf_init(THREAD_IO_CTRL* io_ctrl, IO_BUF_CTRL_INIT_PARAM* param,
		     int buf_type)
{
	IO_RING_BUF* io_ring_buf;

	configASSERT(io_ctrl!=NULL);
	configASSERT(buf_type >=0 && buf_type < RING_BUF_TYPE_NUM);

	io_ring_buf = &(io_ctrl->ring_buf);

	io_ring_buf->type = buf_type;
	io_ring_buf->ops = &g_io_ring_buf;
	return io_ring_buf->ops->init(io_ring_buf, param);
}

int io_ctrl_buf_set_hw_ofs(THREAD_IO_CTRL* io_ctrl, uint32_t hw_ofs_bytes)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->set_hw_ofs(io_ring_buf, hw_ofs_bytes);
}

int io_ctrl_buf_set_appl_ofs(THREAD_IO_CTRL* io_ctrl, uint32_t appl_ofs_bytes)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->set_appl_ofs(io_ring_buf, appl_ofs_bytes);
}

int io_ctrl_buf_get_dir(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_dir(io_ring_buf);
}
void* io_ctrl_buf_get_start_addr(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_start_addr(io_ring_buf);
}

uint32_t io_ctrl_buf_get_size(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_size(io_ring_buf);
}

uint32_t io_ctrl_buf_get_hw_ofs(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_hw_ofs(io_ring_buf);
}


uint32_t io_ctrl_buf_get_appl_ofs(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_appl_ofs(io_ring_buf);
}

uint32_t io_ctrl_buf_get_data_watermark(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_data_watermark(io_ring_buf);
}

uint32_t io_ctrl_buf_get_remain_space(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_remain_space(io_ring_buf);
}

uint32_t* io_ctrl_buf_get_hw_ofs_ptr(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_hw_ofs_ptr(io_ring_buf);
}

uint32_t* io_ctrl_buf_get_appl_ofs_ptr(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_appl_ofs_ptr(io_ring_buf);
}

void* io_ctrl_buf_get_hw_ptr(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_hw_ptr(io_ring_buf);
}

void* io_ctrl_buf_get_appl_ptr(THREAD_IO_CTRL* io_ctrl)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->get_appl_ptr(io_ring_buf);
}

void io_ctrl_buf_reset(THREAD_IO_CTRL* io_ctrl, int reset_to_full)
{
	IO_RING_BUF* io_ring_buf;

	io_ring_buf = &io_ctrl->ring_buf;
	return io_ring_buf->ops->reset(io_ring_buf, reset_to_full);
}

int ring_buf_init(IO_RING_BUF* io_ring_buf, IO_BUF_CTRL_INIT_PARAM* param,
		     int buf_type)
{
	configASSERT(io_ring_buf!=NULL);
	configASSERT(buf_type >=0 && buf_type < RING_BUF_TYPE_NUM);

	io_ring_buf->type = buf_type;
	io_ring_buf->ops = &g_io_ring_buf;
	return io_ring_buf->ops->init(io_ring_buf, param);
}

int ring_buf_set_hw_ofs(IO_RING_BUF* io_ring_buf, uint32_t hw_ofs_bytes)
{
	return io_ring_buf->ops->set_hw_ofs(io_ring_buf, hw_ofs_bytes);
}

int ring_buf_set_appl_ofs(IO_RING_BUF* io_ring_buf, uint32_t appl_ofs_bytes)
{
	return io_ring_buf->ops->set_appl_ofs(io_ring_buf, appl_ofs_bytes);
}

int ring_buf_get_dir(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_dir(io_ring_buf);
}

void* ring_buf_get_start_addr(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_start_addr(io_ring_buf);
}

uint32_t ring_buf_get_size(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_size(io_ring_buf);
}

uint32_t ring_buf_get_hw_ofs(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_hw_ofs(io_ring_buf);
}

uint32_t ring_buf_get_appl_ofs(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_appl_ofs(io_ring_buf);
}

uint32_t ring_buf_get_data_watermark(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_data_watermark(io_ring_buf);
}

uint32_t ring_buf_get_remain_space(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_remain_space(io_ring_buf);
}

uint32_t* ring_buf_get_hw_ofs_ptr(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_hw_ofs_ptr(io_ring_buf);
}

uint32_t* ring_buf_get_appl_ofs_ptr(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_appl_ofs_ptr(io_ring_buf);
}

void* ring_buf_get_hw_ptr(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_hw_ptr(io_ring_buf);
}

void* ring_buf_get_appl_ptr(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->ops->get_appl_ptr(io_ring_buf);
}

void ring_buf_reset(IO_RING_BUF* io_ring_buf, int reset_to_full)
{
	return io_ring_buf->ops->reset(io_ring_buf, reset_to_full);
}

/* buffer ops func declaration */
static int io_ring_buf_init(IO_RING_BUF* io_ring_buf,
			      IO_BUF_CTRL_INIT_PARAM* param);
static int io_ring_buf_set_hw_ofs(IO_RING_BUF* io_ring_buf,
				    uint32_t hw_ofs_bytes);
static int io_ring_buf_set_appl_ofs(IO_RING_BUF* io_ring_buf,
				      uint32_t appl_ofs_bytes);
static int io_ring_buf_get_dir(IO_RING_BUF* io_ring_buf);
static void* io_ring_buf_get_start_addr(IO_RING_BUF* io_ring_buf);
static uint32_t io_ring_buf_get_size(IO_RING_BUF* io_ring_buf);
static uint32_t io_ring_buf_get_hw_ofs(IO_RING_BUF* io_ring_buf);
static uint32_t io_ring_buf_get_appl_ofs(IO_RING_BUF* io_ring_buf);
static uint32_t io_ring_buf_get_data_watermark(IO_RING_BUF* io_ring_buf);
static uint32_t io_ring_buf_get_remain_space(IO_RING_BUF* io_ring_buf);
static uint32_t* io_ring_buf_get_hw_ofs_ptr(IO_RING_BUF* io_ring_buf);
static uint32_t* io_ring_buf_get_appl_ofs_ptr(IO_RING_BUF* io_ring_buf);
static void io_ring_buf_reset(IO_RING_BUF* io_ring_buf, int reset_to_full);
static void* io_ring_buf_get_hw_ptr(IO_RING_BUF* io_ring_buf);
static void* io_ring_buf_get_appl_ptr(IO_RING_BUF* io_ring_buf);
/* buffer ops func implementation*/

static int io_ring_buf_init(IO_RING_BUF* io_ring_buf,
			      IO_BUF_CTRL_INIT_PARAM* param)
{
	configASSERT(param!=NULL);

	io_ring_buf->ring_buffer_dir = param->buffer_dir;
	io_ring_buf->start_addr = param->start_addr;
	io_ring_buf->size_bytes = param->size_bytes;
	if ((io_ring_buf->type == RING_BUF_IO_IPC) ||
		(io_ring_buf->type == RING_BUF_INTER_THREAD)) {
		configASSERT(param->hw_offset_ptr!=NULL);
		configASSERT(param->appl_offset_ptr!=NULL);
		io_ring_buf->ptr_to_hw_offset_bytes = param->hw_offset_ptr;
		io_ring_buf->ptr_to_appl_offset_bytes = param->appl_offset_ptr;
	} else {
		io_ring_buf->ptr_to_hw_offset_bytes = NULL;
		io_ring_buf->ptr_to_appl_offset_bytes = NULL;
	}

	//reset to full or empty
	io_ring_buf_reset(io_ring_buf, param->reset_to_full);

	return 0;
}

static int io_ring_buf_set_hw_ofs(IO_RING_BUF* io_ring_buf,
				    uint32_t hw_ofs_bytes)
{
	if (hw_ofs_bytes != io_ring_buf_get_appl_ofs(io_ring_buf)) {
		io_ring_buf->hw_offset_flag = 0;
		if ((io_ring_buf->type == RING_BUF_IO_IPC) ||
			(io_ring_buf->type == RING_BUF_INTER_THREAD))
			*(io_ring_buf->ptr_to_hw_offset_bytes) = hw_ofs_bytes;
		else
			io_ring_buf->hw_offset_bytes = hw_ofs_bytes;
	} else {
		io_ring_buf->hw_offset_flag = 1;
		if (hw_ofs_bytes==0)
			hw_ofs_bytes = io_ring_buf_get_size(io_ring_buf);
		if ((io_ring_buf->type == RING_BUF_IO_IPC) ||
			(io_ring_buf->type == RING_BUF_INTER_THREAD))
			*(io_ring_buf->ptr_to_hw_offset_bytes) = hw_ofs_bytes - 1;
		else
			io_ring_buf->hw_offset_bytes =  hw_ofs_bytes - 1;
	}
	return 0;
}

static int io_ring_buf_set_appl_ofs(IO_RING_BUF* io_ring_buf,
				      uint32_t appl_ofs_bytes)
{
	if ((io_ring_buf->type == RING_BUF_IO_IPC) ||
		(io_ring_buf->type == RING_BUF_INTER_THREAD))
		*(io_ring_buf->ptr_to_appl_offset_bytes) = appl_ofs_bytes;
	else
		io_ring_buf->appl_offset_bytes = appl_ofs_bytes;

	if ((io_ring_buf->type == RING_BUF_LOCAL_THREAD ||
			io_ring_buf->type == RING_BUF_IO_LOCAL) &&
			(io_ring_buf->hw_offset_flag == 1) &&
			(io_ring_buf->appl_offset_bytes != io_ring_buf->hw_offset_bytes)) {
		io_ring_buf->hw_offset_bytes += 1;
		io_ring_buf->hw_offset_flag = 0;
	}

	return 0;
}

static int io_ring_buf_get_dir(IO_RING_BUF* io_ring_buf)
{
	return (int)io_ring_buf->ring_buffer_dir;
}

static void* io_ring_buf_get_start_addr(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->start_addr;
}

static uint32_t io_ring_buf_get_size(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf->size_bytes;
}

static uint32_t io_ring_buf_get_hw_ofs(IO_RING_BUF* io_ring_buf)
{
	uint32_t hw_offset;

	if ((io_ring_buf->type == RING_BUF_IO_IPC) ||
		(io_ring_buf->type == RING_BUF_INTER_THREAD))
		hw_offset = *(io_ring_buf->ptr_to_hw_offset_bytes);
	else
		hw_offset = io_ring_buf->hw_offset_bytes;
	hw_offset += io_ring_buf->hw_offset_flag;

	if (hw_offset == io_ring_buf_get_size(io_ring_buf))
		hw_offset = 0;

	return hw_offset;
}

static uint32_t io_ring_buf_get_appl_ofs(IO_RING_BUF* io_ring_buf)
{
	uint32_t appl_offset;

	if ((io_ring_buf->type == RING_BUF_IO_IPC) ||
		(io_ring_buf->type == RING_BUF_INTER_THREAD))
		appl_offset = *(io_ring_buf->ptr_to_appl_offset_bytes);
	else
		appl_offset = io_ring_buf->appl_offset_bytes;

	return appl_offset;
}

static uint32_t io_ring_buf_get_data_watermark(IO_RING_BUF* io_ring_buf)
{
	uint32_t appl_ptr_bytes = io_ring_buf_get_appl_ofs(io_ring_buf);
	uint32_t hw_ptr_bytes = io_ring_buf_get_hw_ofs(io_ring_buf);
	uint32_t ret = 0;

	if (io_ring_buf->ring_buffer_dir==RING_BUF_TYPE_RECORD) {
		//when record, buf watermark is hw_ptr - appl_ptr
		//hw_ptr == appl_ptr without hw_offset_flag means no data
		if (hw_ptr_bytes < appl_ptr_bytes) {
			ret = hw_ptr_bytes + io_ring_buf_get_size(io_ring_buf)
				- appl_ptr_bytes;
		} else if (hw_ptr_bytes > appl_ptr_bytes) {
			ret = hw_ptr_bytes - appl_ptr_bytes;
		} else {
			//equal! check the flag to know if it is empty or full
			if (io_ring_buf->hw_offset_flag)
				ret = io_ring_buf_get_size(io_ring_buf);
			else
				ret = 0;
		}
	} else if (io_ring_buf->ring_buffer_dir==RING_BUF_TYPE_PLAYBACK) {
		//when playback, buf watermark is appl_ptr - hw_ptr
		//hw_ptr == appl_ptr without hw_offset_flag means full
		if (appl_ptr_bytes < hw_ptr_bytes) {
			ret = appl_ptr_bytes + io_ring_buf_get_size(io_ring_buf)
				- hw_ptr_bytes;
		} else if (appl_ptr_bytes > hw_ptr_bytes) {
			ret = appl_ptr_bytes - hw_ptr_bytes;
		} else {
			//equal! check the flag to know if it is empty or full
			if (io_ring_buf->hw_offset_flag)
				ret = 0;
			else
				ret = io_ring_buf_get_size(io_ring_buf);
		}
	} else {
		configASSERT(0);
	}

	return ret;
}

static uint32_t io_ring_buf_get_remain_space(IO_RING_BUF* io_ring_buf)
{
	return io_ring_buf_get_size(io_ring_buf)
		- io_ring_buf_get_data_watermark(io_ring_buf);
}

static uint32_t* io_ring_buf_get_hw_ofs_ptr(IO_RING_BUF* io_ring_buf)
{
	uint32_t* ptr;

	if (io_ring_buf->type == RING_BUF_IO_IPC)
		ptr = (uint32_t*)io_ring_buf->ptr_to_hw_offset_bytes;
	else
		ptr = (uint32_t*)(&(io_ring_buf->hw_offset_bytes));

	return ptr;
}

static uint32_t* io_ring_buf_get_appl_ofs_ptr(IO_RING_BUF* io_ring_buf)
{
	uint32_t* ptr;

	if (io_ring_buf->type == RING_BUF_IO_IPC)
		ptr = (uint32_t*)io_ring_buf->ptr_to_appl_offset_bytes;
	else
		ptr = (uint32_t*)(&(io_ring_buf->appl_offset_bytes));
	return ptr;
}

static void* io_ring_buf_get_hw_ptr(IO_RING_BUF* io_ring_buf)
{
	void* ptr;

	ptr = (void *)((char *)io_ring_buf_get_start_addr(io_ring_buf) +
		io_ring_buf_get_hw_ofs(io_ring_buf));

	return ptr;
}

static void* io_ring_buf_get_appl_ptr(IO_RING_BUF* io_ring_buf)
{
	uint32_t* ptr;

	ptr = (void *)((char *)io_ring_buf_get_start_addr(io_ring_buf) +
		io_ring_buf_get_appl_ofs(io_ring_buf));
	return ptr;
}

static void io_ring_buf_reset(IO_RING_BUF* io_ring_buf, int reset_to_full)
{
	//reset to full or empty
	if (io_ring_buf->ring_buffer_dir==RING_BUF_TYPE_RECORD)
		io_ring_buf->hw_offset_flag = !!reset_to_full;
	else
		io_ring_buf->hw_offset_flag = !reset_to_full;

	//if flag is set hw_offset need to (circular) minus one bytes
	if (io_ring_buf->hw_offset_flag) {
		if ((io_ring_buf->type == RING_BUF_IO_IPC) ||
			(io_ring_buf->type == RING_BUF_INTER_THREAD))
			*io_ring_buf->ptr_to_hw_offset_bytes = io_ring_buf->size_bytes - 1;
		else
			io_ring_buf->hw_offset_bytes = io_ring_buf->size_bytes - 1;
	} else {
		if ((io_ring_buf->type == RING_BUF_IO_IPC) ||
			(io_ring_buf->type == RING_BUF_INTER_THREAD))
			*io_ring_buf->ptr_to_hw_offset_bytes = 0;
		else
			io_ring_buf->hw_offset_bytes = 0;
	}
	if ((io_ring_buf->type == RING_BUF_IO_IPC) ||
		(io_ring_buf->type == RING_BUF_INTER_THREAD))
		*io_ring_buf->ptr_to_appl_offset_bytes = 0;
	else
		io_ring_buf->appl_offset_bytes = 0;
}

static const IO_BUF_CTRL_OPS g_io_ring_buf = {
	.init = io_ring_buf_init,
	.set_hw_ofs = io_ring_buf_set_hw_ofs,
	.set_appl_ofs = io_ring_buf_set_appl_ofs,
	.get_dir = io_ring_buf_get_dir,
	.get_start_addr = io_ring_buf_get_start_addr,
	.get_size = io_ring_buf_get_size,
	.get_hw_ofs = io_ring_buf_get_hw_ofs,
	.get_appl_ofs = io_ring_buf_get_appl_ofs,
	.get_data_watermark = io_ring_buf_get_data_watermark,
	.get_remain_space = io_ring_buf_get_remain_space,
	.get_hw_ofs_ptr = io_ring_buf_get_hw_ofs_ptr,
	.get_appl_ofs_ptr = io_ring_buf_get_appl_ofs_ptr,
	.get_hw_ptr = io_ring_buf_get_hw_ptr,
	.get_appl_ptr = io_ring_buf_get_appl_ptr,
	.reset = io_ring_buf_reset,
};
