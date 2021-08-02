/*
 * MediaTek Inc. (C) 2019. All rights reserved.
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

#include "nandx_util.h"
#include "nandx_errno.h"
#include "nfc.h"
#include "nfc_core.h"

static void nfc_setup_handler(struct nfc_handler *handler)
{
    /* setup nfc handler, defined by each controller with the same name */
    handler->send_command = nfc_send_command;
    handler->send_address = nfc_send_address;
    handler->read_byte = nfc_read_byte;
    handler->write_byte = nfc_write_byte;
    handler->read_sectors = nfc_read_sectors;
    handler->write_page = nfc_write_page;
    handler->change_interface = nfc_change_interface;
    handler->change_mode = nfc_change_mode;
    handler->get_mode = nfc_get_mode;
    handler->select_chip = nfc_select_chip;
    handler->set_format = nfc_set_format;
    handler->enable_randomizer = nfc_enable_randomizer;
    handler->disable_randomizer = nfc_disable_randomizer;
    handler->wait_busy = nfc_wait_busy;
    handler->calculate_ecc = nfc_calculate_ecc;
    handler->correct_ecc = nfc_correct_ecc;
    handler->calibration = nfc_calibration;
    handler->suspend = nfc_suspend;
    handler->resume = nfc_resume;
}

struct nfc_handler *nfc_init(struct nfc_resource *res)
{
    struct nfc_handler *handler;

    /* setup nfc hw according to platform_data and alloc nfc_handler */
    handler = nfc_setup_hw(res);
    nfc_setup_handler(handler);

    return handler;
}

void nfc_exit(struct nfc_handler *handler)
{
    nfc_release(handler);
}
