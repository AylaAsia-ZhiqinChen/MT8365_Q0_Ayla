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

#ifndef __SSUSB_QMU_H__
#define __SSUSB_QMU_H__

#define USE_SSUSB_QMU

//#include "mt_mu3d_hal_qmu_drv.h"

#ifdef USE_SSUSB_QMU

#define GPD_BUF_SIZE 65532
#define GPD_BUF_SIZE_ALIGN 64512 /* 63 * 1024 */
#define BD_BUF_SIZE 32768 //set to half of 64K of max size

struct qmu_done_isr_data {
	struct musb *musb; 
	u32 qmu_val;
};

//EXTERN void usb_initialize_qmu(void);
void usb_initialize_qmu(void);
//EXTERN void txstate_qmu(struct musb *musb, struct musb_request *req);
//EXTERN void txstate_qmu(struct urb *req);
void txstate_qmu(struct urb *req);

void qmu_exception_interrupt(void *base, u32 qmuval);
void qmu_done_interrupt(u32 qmu_val);

void qmu_handler(u32 qmu_val);

#endif //#ifdef USE_SSUSB_QMU

/* These defines comes from Linux */
//#define USE_LINUX_DEFINES
#ifdef USE_LINUX_DEFINES
#define USB_ENDPOINT_XFERTYPE_MASK  0x03    /* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL   0
#define USB_ENDPOINT_XFER_ISOC      1
#define USB_ENDPOINT_XFER_BULK      2
#define USB_ENDPOINT_XFER_INT       3
#define USB_ENDPOINT_MAX_ADJUSTABLE 0x80

#define USB_ENDPOINT_SYNCTYPE       0x0c
#define USB_ENDPOINT_SYNC_NONE      (0 << 2)
#define USB_ENDPOINT_SYNC_ASYNC     (1 << 2)
#define USB_ENDPOINT_SYNC_ADAPTIVE  (2 << 2)
#define USB_ENDPOINT_SYNC_SYNC      (3 << 2)

#define USB_ENDPOINT_USAGE_MASK     0x30
#define USB_ENDPOINT_USAGE_DATA     0x00
#define USB_ENDPOINT_USAGE_FEEDBACK 0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FB  0x20    /* Implicit feedback Data endpoint */
#endif

#endif //#ifndef __SSUSB_QMU_H__
