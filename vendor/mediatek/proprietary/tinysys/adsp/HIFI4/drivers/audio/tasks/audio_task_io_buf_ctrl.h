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

#ifndef _AUDIO_TASK_IO_BUF_CTRL_H_
#define _AUDIO_TASK_IO_BUF_CTRL_H_

typedef struct thread_io_ctrl THREAD_IO_CTRL;

enum {
    RING_BUF_IO_IPC,
    RING_BUF_IO_LOCAL,
    RING_BUF_INTER_THREAD,
    RING_BUF_LOCAL_THREAD,
    RING_BUF_TYPE_NUM,
};

/* new API */
typedef struct io_buf_ctrl_init_param {
	int buffer_dir;
	void* start_addr;
	uint32_t size_bytes;

	//default reset to zero
	int reset_to_full;

	//only needed if you put offset in shared memory
	uint32_t* hw_offset_ptr;
	uint32_t* appl_offset_ptr;
} IO_BUF_CTRL_INIT_PARAM;

typedef struct io_ring_buf IO_RING_BUF;

typedef struct io_buf_ctrl_ops {
	/* initialize */
	int (*init) (IO_RING_BUF* io_ring_buf, IO_BUF_CTRL_INIT_PARAM* param);

	int (*set_hw_ofs)(IO_RING_BUF* io_ring_buf,
			  uint32_t hw_ofs_bytes);
	int (*set_appl_ofs)(IO_RING_BUF* io_ring_buf,
			    uint32_t appl_ofs_bytes);
	int (*get_dir)(IO_RING_BUF* io_ring_buf);
	void* (*get_start_addr)(IO_RING_BUF* io_ring_buf);
	uint32_t (*get_size)(IO_RING_BUF* io_ring_buf);
	uint32_t (*get_hw_ofs)(IO_RING_BUF* io_ring_buf);
	uint32_t (*get_appl_ofs)(IO_RING_BUF* io_ring_buf);
	uint32_t (*get_data_watermark)(IO_RING_BUF* io_ring_buf);
	uint32_t (*get_remain_space)(IO_RING_BUF* io_ring_buf);
	uint32_t* (*get_hw_ofs_ptr)(IO_RING_BUF* io_ring_buf);
	uint32_t* (*get_appl_ofs_ptr)(IO_RING_BUF* io_ring_buf);
	void* (*get_hw_ptr)(IO_RING_BUF* io_ring_buf);
	void* (*get_appl_ptr)(IO_RING_BUF* io_ring_buf);

	void (*reset) (IO_RING_BUF* io_ring_buf, int reset_to_full);
} IO_BUF_CTRL_OPS;

struct io_ring_buf {
    int type;
    int ring_buffer_dir;
    void* start_addr;
    uint32_t size_bytes;

    union {
        uint32_t hw_offset_bytes;
        volatile uint32_t *ptr_to_hw_offset_bytes;
    };
    union {
        uint32_t appl_offset_bytes;
        volatile uint32_t *ptr_to_appl_offset_bytes;
    };

    /* hw_offset_flag:
     * We treat hw_offset==appl_offset as buffer empty when record.
     * If the buffer is full the hw_offset will be one bytes behind the
     * appl_offset and the hw_offset_flag will set to one.
     * When playback, hw_offset==appl_offset will be treated as full. If
     * the buffer is empty, the hw_offset will be one bytes behind the
     * appl_offset and the hw_offset_flag will set to one.
       */
    uint32_t hw_offset_flag;
    const IO_BUF_CTRL_OPS *ops;
};

int io_ctrl_buf_init(THREAD_IO_CTRL* io_ctrl, IO_BUF_CTRL_INIT_PARAM* param,
		     int buf_type);
int io_ctrl_buf_set_hw_ofs(THREAD_IO_CTRL* io_ctrl, uint32_t hw_ofs_bytes);
int io_ctrl_buf_set_appl_ofs(THREAD_IO_CTRL* io_ctrl, uint32_t appl_ofs_bytes);
int io_ctrl_buf_get_dir(THREAD_IO_CTRL* io_ctrl);
void* io_ctrl_buf_get_start_addr(THREAD_IO_CTRL* io_ctrl);
uint32_t io_ctrl_buf_get_size(THREAD_IO_CTRL* io_ctrl);
uint32_t io_ctrl_buf_get_hw_ofs(THREAD_IO_CTRL* io_ctrl);
uint32_t io_ctrl_buf_get_appl_ofs(THREAD_IO_CTRL* io_ctrl);
uint32_t io_ctrl_buf_get_data_watermark(THREAD_IO_CTRL* io_ctrl);
uint32_t io_ctrl_buf_get_remain_space(THREAD_IO_CTRL* io_ctrl);
uint32_t* io_ctrl_buf_get_hw_ofs_ptr(THREAD_IO_CTRL* io_ctrl);
uint32_t* io_ctrl_buf_get_appl_ofs_ptr(THREAD_IO_CTRL* io_ctrl);
void* io_ctrl_buf_get_hw_ptr(THREAD_IO_CTRL* io_ctrl);
void* io_ctrl_buf_get_appl_ptr(THREAD_IO_CTRL* io_ctrl);

void io_ctrl_buf_reset(THREAD_IO_CTRL* io_ctrl, int reset_to_full);

int ring_buf_init(IO_RING_BUF* io_ring_buf, IO_BUF_CTRL_INIT_PARAM* param,
		     int buf_type);
int ring_buf_set_hw_ofs(IO_RING_BUF* io_ring_buf, uint32_t hw_ofs_bytes);
int ring_buf_set_appl_ofs(IO_RING_BUF* io_ring_buf, uint32_t appl_ofs_bytes);
int ring_buf_get_dir(IO_RING_BUF* io_ring_buf);
void* ring_buf_get_start_addr(IO_RING_BUF* io_ring_buf);
uint32_t ring_buf_get_size(IO_RING_BUF* io_ring_buf);
uint32_t ring_buf_get_hw_ofs(IO_RING_BUF* io_ring_buf);
uint32_t ring_buf_get_appl_ofs(IO_RING_BUF* io_ring_buf);
uint32_t ring_buf_get_data_watermark(IO_RING_BUF* io_ring_buf);
uint32_t ring_buf_get_remain_space(IO_RING_BUF* io_ring_buf);
uint32_t* ring_buf_get_hw_ofs_ptr(IO_RING_BUF* io_ring_buf);
uint32_t* ring_buf_get_appl_ofs_ptr(IO_RING_BUF* io_ring_buf);
void* ring_buf_get_hw_ptr(IO_RING_BUF* io_ring_buf);
void* ring_buf_get_appl_ptr(IO_RING_BUF* io_ring_buf);

void ring_buf_reset(IO_RING_BUF* io_ring_buf, int reset_to_full);

#endif

