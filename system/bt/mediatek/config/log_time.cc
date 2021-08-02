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

#define LOG_TAG "bt_logtime"

#include "log_time.h"

#include <arpa/inet.h>
#include <sys/time.h>
#include <string>

#include <base/memory/singleton.h>

#include "common/time_util.h"

namespace vendor {
namespace mediatek {
namespace bluetooth {
namespace stack {

class LogTimeImpl {
 public:
  LogTimeImpl() : kBTSnoopEpochDelta_(0x00dcddb30f2f8000ULL) {}
  ~LogTimeImpl() = default;

  uint64_t GetEpochTime() const {
    return HToLL(::bluetooth::common::time_gettimeofday_us() + kBTSnoopEpochDelta_);
  }

  std::string GetLogTimeTag() const {
    // TODO(Peng): To implement with C++ template function
    char curtime[64] = {0};
    struct tm* tmp = NULL;
    time_t lt;
    /* get current time */
    lt = time(NULL);
    tmp = localtime(&lt);
    strftime(curtime, sizeof(curtime), "%Y_%m%d_%H%M%S", tmp);
    return std::string(curtime);
  }

 private:
  uint64_t HToLL(uint64_t ll) const {
    const uint32_t l = 1;
    if (*(reinterpret_cast<const uint8_t*>(&l)) == 1)
      return static_cast<uint64_t>(htonl(ll & 0xffffffff)) << 32 |
             htonl(ll >> 32);
    return ll;
  }

  const uint64_t kBTSnoopEpochDelta_;
};

LogTime::LogTime() : logtime_impl_(new LogTimeImpl()) {}

LogTime* LogTime::GetInstance() { return base::Singleton<LogTime>::get(); }

uint64_t LogTime::GetEpochTime() const { return logtime_impl_->GetEpochTime(); }

std::string LogTime::GetLogTimeTag() const {
  return logtime_impl_->GetLogTimeTag();
}

}  // namespace stack
}  // namespace bluetooth
}  // namespace mediatek
}  // namespace vendor

#endif
