/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly
 * * prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * * NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO
 * * SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER
 * * EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN
 * * FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE.
 * * MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES
 * * MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR
 * * OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED
 * * HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK
 * * SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE
 * * PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */

#define LOG_TAG "mtk_bta_ag_cmd"
#include "mediatek/bta/include/mtk_bta_ag_cmd.h"

#include "bta_ag_api.h"
#include "interop_mtk.h"
#include "osi/include/log.h"


 /*******************************************************************************
**
** Function         check_hfp_ag_feature_by_blacklist
**
** Description      This function check if
**                       1. The device address matches the HFP 1.7 blacklist
**                       2. DUT HFP version is 1.7
**                  change HFP ag feature bitmark as version 1.6
**
** Returns          uint32_t features
**
*******************************************************************************/
uint32_t check_hfp_ag_feature_by_blacklist(uint32_t features, const RawAddress& addr)
 {
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
    LOG_INFO(LOG_TAG,"BTA_AG_LOCAL_EVT_BRSF check peer addr");
    if (interop_mtk_match_addr_name(INTEROP_MTK_HFP_17_TO_16, &addr)) {
      LOG_INFO(LOG_TAG,"Set HFP1.6 feature mark");
      features &= HFP_1_6_FEAT_MASK;
    }
#endif
    return features;
 }
