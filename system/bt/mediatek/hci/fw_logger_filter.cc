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

#if defined(MTK_STACK_CONFIG_LOG) && (MTK_STACK_CONFIG_LOG == TRUE)

#define LOG_TAG "bt_fw_logger_filter"

#include "fw_logger_filter.h"

#include <base/logging.h>
#include <base/memory/singleton.h>

#include "bt_types.h"
#include "osi/include/allocator.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

class FWLogFilterImpl {
 public:
  FWLogFilterImpl() {}
  ~FWLogFilterImpl() = default;

  bool IsFWLogEvent(const BT_HDR* packet) const {
    CHECK(packet != NULL);
    const uint8_t* stream = packet->data;
    return IsFWLogData(stream);
  }

  bool Intercepted(BT_HDR* packet) const {
    CHECK(packet != NULL);
    const uint8_t* stream = packet->data;
    if (IsFWLogData(stream)) {
      osi_free(packet);
      return true;
    } else {
      return false;
    }
  }

 private:
  bool IsFWLogData(const uint8_t* data) const {
    const uint8_t FW_EVENT_CODE = 0xFF;
    const uint8_t FW_SUB_EVENT_CODE = 0x50;
    uint8_t event_code = 0;
    uint8_t subevent_code = 0;
    STREAM_TO_UINT8(event_code, data);
    STREAM_SKIP_UINT8(data);  // Skip the parameter total length field
    STREAM_TO_UINT8(subevent_code, data);
    //  LOG_INFO(LOG_TAG, "%s: event_code: 0x%x, subevent_code: 0x%x",
    //      __func__, event_code, subevent_code);
    return (event_code == FW_EVENT_CODE && subevent_code == FW_SUB_EVENT_CODE);
  }
};

FWLogFilter::FWLogFilter() : filter_impl_(new FWLogFilterImpl()) {}

FWLogFilter* FWLogFilter::GetInstance() {
  return base::Singleton<FWLogFilter>::get();
}

bool FWLogFilter::IsFWLogEvent(const BT_HDR* packet) const {
  return filter_impl_->IsFWLogEvent(packet);
}

bool FWLogFilter::Intercepted(BT_HDR* packet) const {
  return filter_impl_->Intercepted(packet);
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
