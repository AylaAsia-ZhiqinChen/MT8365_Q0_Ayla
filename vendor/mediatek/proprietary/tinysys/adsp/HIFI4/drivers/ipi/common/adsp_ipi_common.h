/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
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

#ifndef ADSP_IPI_COMMON_H
#define ADSP_IPI_COMMON_H

#include <stdint.h>

#define IPC0    0 /* IPI */
#define IPC_MAX  4

#define IPI_PRINT_THRESHOLD 1000

#define IPI_STAMP_DUMP     0

enum ipi_dir {
    IPI_ADSP2AP = 0,
};

enum ipi_status {
    ERROR =-1,
    DONE,
    BUSY,
};

typedef enum ipi_status ipi_status;

typedef int(*ipi_handler_t)(int id, void * data, unsigned int len);
typedef void(*ipc_handler_t)(void);

struct ipi_desc_t {
#ifdef CFG_IPI_STAMP_SUPPORT
#define ADSP_IPI_ID_STAMP_SIZE 3
    uint64_t recv_timestamp[ADSP_IPI_ID_STAMP_SIZE]; //recv_flag[recv_count] recv_timestamp[Timestamp]
    uint64_t send_timestamp[ADSP_IPI_ID_STAMP_SIZE]; //send_flag[send_count] send_timestamp[Timestamp]
    uint32_t recv_flag[ADSP_IPI_ID_STAMP_SIZE];
    uint32_t send_flag[ADSP_IPI_ID_STAMP_SIZE];
#endif
    uint32_t recv_count;
    uint32_t init_count;
    uint32_t success_count;
    uint32_t busy_count;
    uint32_t error_count;
    uint32_t last_handled;
    uint32_t is_wakeup_src;
    ipi_handler_t handler;
    const char  *name;
};

struct ipc_desc_t {
    uint64_t last_enter;
    uint64_t last_exit;
    uint64_t max_duration;
    ipc_handler_t handler;
    const char  *name;
};

#define SHARE_BUF_SIZE 288
struct share_obj {
    int id;
    unsigned int len;
    unsigned char reserve[8];
    unsigned char share_buf[SHARE_BUF_SIZE - 16];
};
#endif
