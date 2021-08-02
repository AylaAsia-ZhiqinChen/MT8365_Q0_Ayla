/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2017. All rights reserved.
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

#include <mt_reg_base.h>
#include "main.h"

/*
 * for irq to host
 */
#define CURR_DSP_CORE_ID 0
#define IPC_TO_HOST_EINT_NUM 0
#define IRQ_TRIIGER_TYPE DSP_GPIO_IRQ_SIGNAL_RISING_EDGE

/*
 * for irq from host
 */
#define IPC_FROM_HOST_EINT_NUM 56
/*
 * adsp info
 */
#define ADSP_COMMON_INFO_ADDR (ADSP_IPI_SHARED_ADDR)
#define ADSP_COMMON_INFO_MAGIC 0x1D901D90

#define IPC_INT_NUM (LX_MCU_IRQ_B)

/* remote adsp info, need access by spi*/
#define IPC_MESSAGE_READY (1<<0)

struct adsp_common_info
{
    int32_t magic;
    int32_t status;
    int32_t adsp_send_obj_addr;
    int32_t adsp_rcv_obj_addr;
    int32_t adsp_to_host_status;
    int32_t host_to_adsp_status;
};

ipi_status adsp_ipi_send_internal(enum ipi_id id, void* buf, uint32_t len, uint32_t wait,enum ipi_dir dir);
ipi_status adsp_ipi_status(enum ipi_id id);
void adsp_ipi_wakeup_ap_registration(enum ipi_id id);
void ipi_status_dump(void);
void ipi_status_dump_id(enum ipi_id id);
void ipi_adsp2spm(void);
void adsp_awake_init(void);
uint32_t is_ipi_busy(void);
void request_ipc(uint32_t ipc_num, ipc_handler_t handler, const char *name);

