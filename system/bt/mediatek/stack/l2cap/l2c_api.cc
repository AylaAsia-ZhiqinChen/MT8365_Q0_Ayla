/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */


#include "mediatek/stack/include/l2c_api.h"

#include <base/logging.h>
#include <base/strings/stringprintf.h>

#include "stack/l2cap/l2c_int.h"

using base::StringPrintf;


 /*******************************************************************************
 **
 ** Function     L2CA_FixedChnlPending
 **
 ** Description  This function is to check whether the fixed channel connection request
 **              request is pending
 **
 ** Returns      If the fixed channel connection is pending, return true.
 **
 *******************************************************************************/
 bool L2CA_IsFixedChnlPending(uint16_t fixed_cid, const RawAddress& rem_bda) {
   tL2C_LCB* p_lcb;
   tBT_TRANSPORT transport = BT_TRANSPORT_BR_EDR;

   // Check CID is valid and registered
   if ((fixed_cid < L2CAP_FIRST_FIXED_CHNL) ||
       (fixed_cid > L2CAP_LAST_FIXED_CHNL) ||
       (l2cb.fixed_reg[fixed_cid - L2CAP_FIRST_FIXED_CHNL].pL2CA_FixedData_Cb ==
        NULL)) {
     LOG(ERROR) << StringPrintf("%s() Invalid CID: 0x%04x", __func__, fixed_cid);
     return (false);
   }

   if (fixed_cid >= L2CAP_ATT_CID && fixed_cid <= L2CAP_SMP_CID)
     transport = BT_TRANSPORT_LE;

   if (((p_lcb = l2cu_find_lcb_by_bd_addr(rem_bda, transport)) != NULL) &&
       (p_lcb->p_pending_ccb != NULL)) {
     if (p_lcb->p_pending_ccb ==
         p_lcb->p_fixed_ccbs[fixed_cid - L2CAP_FIRST_FIXED_CHNL]) {
       VLOG(1) << StringPrintf(
           "%s() CID: 0x%04x  bda:%s, the fixed channel connection is "
           "pending",  __func__, fixed_cid, rem_bda.ToString().c_str());
       return (true);
     }
   }

   return (false);
 }

