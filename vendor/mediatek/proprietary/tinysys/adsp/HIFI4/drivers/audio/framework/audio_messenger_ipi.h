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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

#ifndef AUDIO_MESSENGER_IPI_H
#define AUDIO_MESSENGER_IPI_H

#include <stdint.h>


/*
 * =============================================================================
 *                     MACRO
 * =============================================================================
 */

#define MAX_IPI_MSG_BUF_SIZE     (272) /* SHARE_BUF_SIZE - 16 */
#define IPI_MSG_HEADER_SIZE      (16)
#define MAX_IPI_MSG_PAYLOAD_SIZE (MAX_IPI_MSG_BUF_SIZE - IPI_MSG_HEADER_SIZE)

#define IPI_MSG_MAGIC_NUMBER     (0x8888)


/*
 * =============================================================================
 *                     typedef
 * =============================================================================
 */

enum { /* audio_ipi_msg_source_layer_t */
    AUDIO_IPI_LAYER_FROM_HAL,
    AUDIO_IPI_LAYER_FROM_KERNEL,
    AUDIO_IPI_LAYER_FROM_DSP,

    AUDIO_IPI_LAYER_FROM_MAX = 15 /* 4-bit only */
};

enum { /* audio_ipi_msg_target_layer_t */
    AUDIO_IPI_LAYER_TO_HAL,
    AUDIO_IPI_LAYER_TO_KERNEL,
    AUDIO_IPI_LAYER_TO_DSP,

    AUDIO_IPI_LAYER_TO_MAX = 15 /* 4-bit only */
};

enum { /* audio_ipi_msg_data_t */
    AUDIO_IPI_MSG_ONLY,
    AUDIO_IPI_PAYLOAD,
    AUDIO_IPI_DMA,
};

enum { /* audio_ipi_msg_ack_t */
    /* bypass ack, but still send to audio queue */
    AUDIO_IPI_MSG_BYPASS_ACK    = 0,

    /* need ack, and block in audio queue until ack back */
    AUDIO_IPI_MSG_NEED_ACK      = 1,
    AUDIO_IPI_MSG_ACK_BACK      = 2,

    /* bypass audio queue, but still send to ipc queue */
    AUDIO_IPI_MSG_DIRECT_SEND   = 3,

    AUDIO_IPI_MSG_CANCELED      = 8
};

/*
 * =============================================================================
 *                     struct definition
 * =============================================================================
 */

struct aud_data_t {
    uint32_t memory_size;           /* buffer size (memory) */
    uint32_t data_size;             /* 0 <= data_size <= memory_size */
    union {
        void    *addr;          /* memory address */
        unsigned long addr_val; /* the value of address */

        uint32_t dummy[2];      /* work between 32/64 bit environment */
    };
};

struct ipi_msg_dma_info_t {
    struct aud_data_t reserve;      /* TODO: remove later */

    struct aud_data_t hal_buf;      /* source data buffer */

    uint32_t rw_idx;                /* region r/w index */
    uint32_t data_size;             /* region data size */

    struct aud_data_t wb_dram;      /* allow target to write data back */
};
#define IPI_MSG_DMA_INFO_SIZE (sizeof(struct ipi_msg_dma_info_t))


struct ipi_msg_t {
    /* header: 16 bytes */
    uint16_t magic;             /* IPI_MSG_MAGIC_NUMBER */
    uint8_t  task_scene;        /* see task_scene_t */
    uint8_t  source_layer: 4;   /* see audio_ipi_msg_source_layer_t */
    uint8_t  target_layer: 4;   /* see audio_ipi_msg_target_layer_t */

    uint8_t  data_type;         /* see audio_ipi_msg_data_t */
    uint8_t  ack_type;          /* see audio_ipi_msg_ack_t */
    uint16_t msg_id;            /* defined by user */

    union {
        uint32_t param1;
        uint32_t payload_size;  /* payload */
        uint32_t scp_ret;
    };

    uint32_t param2;

    /* data: 256 bytes */
    union {
        char payload[MAX_IPI_MSG_PAYLOAD_SIZE]; /* payload only */
        struct ipi_msg_dma_info_t dma_info;     /* dma only */
        char *dma_addr;  /* TODO: remove later */
    };

};

#if 1 /* TODO: remove typedef later */
typedef struct ipi_msg_t ipi_msg_t;
#endif


/*==============================================================================
 *                     public functions - declaration
 *============================================================================*/

void audio_messenger_ipi_init(void);
void audio_ready_notify_host(void *info, uint32_t size);

int audio_send_ipi_msg(
    struct ipi_msg_t *p_ipi_msg,
    uint8_t task_scene, /* task_scene_t */
    uint8_t target_layer, /* audio_ipi_msg_target_layer_t */
    uint8_t data_type, /* audio_ipi_msg_data_t */
    uint8_t ack_type, /* audio_ipi_msg_ack_t */
    uint16_t msg_id,
    uint32_t param1, /* data_size for payload & dma */
    uint32_t param2,
    void    *data_buffer); /* buffer for payload & dma */

int audio_send_ipi_msg_to_kernel(const ipi_msg_t *p_ipi_msg);

int audio_send_ipi_msg_ack_back(ipi_msg_t *ipi_msg);

uint16_t get_message_buf_size(const ipi_msg_t *p_ipi_msg);


int audio_get_dma_from_msg(
    const ipi_msg_t *ipi_msg,
    void **data_buf,
    uint32_t *data_size);


void print_msg_info(
    const char *func_name,
    const char *description,
    const ipi_msg_t *p_ipi_msg);


int packing_ipimsg(
    ipi_msg_t *p_ipi_msg,
    uint8_t task_scene, /* task_scene_t */
    uint8_t target_layer, /* audio_ipi_msg_target_layer_t */
    uint8_t data_type, /* audio_ipi_msg_data_t */
    uint8_t ack_type, /* audio_ipi_msg_ack_t */
    uint16_t msg_id,
    uint32_t param1,
    uint32_t param2,
    void    *data_buffer);



#endif /* end of AUDIO_MESSENGER_IPI_H */

