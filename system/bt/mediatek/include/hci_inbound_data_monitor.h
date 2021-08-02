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

#pragma once

#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#include <stdint.h>
#include <memory>
#include <string>

#include <base/macros.h>

#include "bt_types.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

enum InboundDataLevel {
  kInboundDataLevelLow,
  kInboundDataLevelMedium,
  kInboundDataLevelHigh,
  kInboundDataLevelUltraHigh,
  kInboundDataLevelUnknown
};

static constexpr uint64_t DATA_MONITOR_TIMEOUT_MS = 1000;
// Criteria
static uint32_t LOW_TIMES = 100;
static float LOW_DATA_PER_SEC = 20.00f;
static uint32_t MEDIUM_TIMES = 300;
static float MEDIUM_DATA_PER_SEC = 40.00f;
static uint32_t HIGH_TIMES = 600;
static float HIGH_DATA_PER_SEC = 80.00f;

// Filter
enum InboundDataFilter {
  kInboundDataFilterNone,
  kInboundDataUTTest,  // For UTTest debug propose only
  kInboundDataVSEFWLog,
  kInboundDataFilterUnknown
};

class InboundDataMonitorImpl;

class InboundDataMonitor {
 public:
  explicit InboundDataMonitor(const std::string& monitor_name);
  ~InboundDataMonitor();

  void InboundDataFilterUpdate(InboundDataFilter filter_type, bool is_on);
  void InboundDataUpdate(const BT_HDR* buffer);
  InboundDataLevel GetDataLevel(InboundDataFilter filter_type) const;

 private:
  std::unique_ptr<InboundDataMonitorImpl> inbound_data_mointor_impl_;

  DISALLOW_COPY_AND_ASSIGN(InboundDataMonitor);
};

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
