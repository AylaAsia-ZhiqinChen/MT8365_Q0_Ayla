/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef __RTC_CAPABILITY_SWITCH_UTIL_H__
#define __RTC_CAPABILITY_SWITCH_UTIL_H__

/*****************************************************************************
 * Include
 *****************************************************************************/
#include <telephony/mtk_ril.h>
#include <string>
#include "RfxStringData.h"
#include "RfxStringsData.h"

/*****************************************************************************
 * Define
 *****************************************************************************/

#define RAF_GSM_GROUP (RAF_GSM | RAF_GPRS | RAF_EDGE)
#define RAF_HS_GROUP (RAF_HSUPA | RAF_HSDPA | RAF_HSPA | RAF_HSPAP)
#define RAF_CDMA_GROUP (RAF_IS95A | RAF_IS95B | RAF_1xRTT)
#define RAF_EVDO_GROUP (RAF_EVDO_0 | RAF_EVDO_A | RAF_EVDO_B | RAF_EHRPD)
#define RAF_WCDMA_GROUP (RAF_HS_GROUP | RAF_UMTS | RAF_TD_SCDMA)
#define RAF_LTE_GROUP (RAF_LTE | RAF_LTE_CA)

/*****************************************************************************
 * Class RtcCapabilitySwitchUtil
 *****************************************************************************/

class RtcCapabilitySwitchUtil {

  public:
    static int getRafFromNetworkType(int type);
    static int getNetworkTypeFromRaf(int raf);
    static bool isDisableC2kCapability();
    static bool isDssNoResetSupport();
    static bool isOp01Sim(const String8& imsi);
    static bool isOp02Sim(const String8& imsi);
    static bool isOp09Sim(const String8& imsi);
    static bool isOp18Sim(const String8& imsi);
    static bool isHvolteSupport();
    static bool isSimSwitchEnabled();

  private:
    static int getAdjustedRaf(int raf);
};

#endif /* __RTC_CAPABILITY_SWITCH_UTIL_H__ */

