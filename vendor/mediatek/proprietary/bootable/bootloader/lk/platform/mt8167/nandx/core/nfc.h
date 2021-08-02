/*
 * MediaTek Inc. (C) 2018. All rights reserved.
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
#ifndef __NFC_H__
#define __NFC_H__

#include "nfc_core.h"

extern void nfc_send_command(struct nfc_handler *handler, u8 cmd);
extern void nfc_send_address(struct nfc_handler *handler, u32 col, u32 row,
                             u32 col_cycle, u32 row_cycle);
extern u8 nfc_read_byte(struct nfc_handler *handler);
extern void nfc_write_byte(struct nfc_handler *handler, u8 data);
extern int nfc_read_sectors(struct nfc_handler *handler, int num, u8 *data,
                            u8 *fdm);
extern int nfc_write_page(struct nfc_handler *handler, u8 *data, u8 *fdm);
extern int nfc_change_interface(struct nfc_handler *handler,
                                enum INTERFACE_TYPE type,
                                struct nand_timing *timing, void *arg);
extern int nfc_change_mode(struct nfc_handler *handler,
                           enum OPS_MODE_TYPE mode, bool enable, void *arg);
extern bool nfc_get_mode(struct nfc_handler *handler,
                         enum OPS_MODE_TYPE mode);
extern void nfc_select_chip(struct nfc_handler *handler, int cs);
extern void nfc_set_format(struct nfc_handler *handler,
                           struct nfc_format *format);
extern void nfc_enable_randomizer(struct nfc_handler *handler, u32 page,
                                  bool encode);
extern void nfc_disable_randomizer(struct nfc_handler *handler);
extern int nfc_wait_busy(struct nfc_handler *handler, int timeout,
                         enum WAIT_TYPE type);
extern int nfc_calculate_ecc(struct nfc_handler *handler, u8 *data, u8 *ecc,
                             u32 len, u8 ecc_strength);
extern int nfc_correct_ecc(struct nfc_handler *handler, u8 *data, u32 len,
                           u8 ecc_strength);
extern int nfc_calibration(struct nfc_handler *handler);
extern struct nfc_handler *nfc_setup_hw(struct nfc_resource *res);
extern void nfc_release(struct nfc_handler *handler);
extern int nfc_suspend(struct nfc_handler *handler);
extern int nfc_resume(struct nfc_handler *handler);

#endif
